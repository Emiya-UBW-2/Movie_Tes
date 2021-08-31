#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	//プレイヤー
	class PLAYERclass {
	public:
		static void BodyFrameSetMatrix(const MV1& obj_body_t, const frames& id, const MATRIX_ref& mat_t = MGetIdent()) noexcept { if (id.first >= 0) { obj_body_t.SetFrameLocalMatrix(id.first, mat_t * MATRIX_ref::Mtrans(id.second)); } }
		static const auto BodyFrameMatrix(const MV1& obj_body_t, const frames& id) noexcept { return obj_body_t.GetFrameLocalWorldMatrix(id.first); }
		static const auto Head_bobbing(MV1::ani* ani_ptr) noexcept { return sin(DX_PI_F * 2.f * (ani_ptr->time / ani_ptr->alltime)) * ani_ptr->per; }		//頭の揺れ
		static auto* CreateB2Body(std::shared_ptr<b2World>& world, b2BodyType type, float32 x_, float32 y_, float angle = 0) noexcept {
			b2BodyDef f_bodyDef;
			f_bodyDef.type = type;
			f_bodyDef.position.Set(x_, y_);
			f_bodyDef.angle = angle;
			return world->CreateBody(&f_bodyDef);
		}
		//マウス角取得
		static void Get_MouseVec(float* x_m, float* y_m, const float fov_per, int disp_x, int disp_y) noexcept {
			int x_t, y_t;
			GetMousePoint(&x_t, &y_t);//~0.01
			*x_m = float(std::clamp(x_t - disp_x / 2, -120, 120)) * fov_per;
			*y_m = float(std::clamp(y_t - disp_y / 2, -120, 120)) * fov_per;
			SetMousePoint(disp_x / 2, disp_y / 2);//ウィンドウだと0.05～0.10ms掛かる?!
			SetMouseDispFlag(FALSE);
		}

		class PLAYER_VEHICLE;
		class PLAYER_CHARA;
		//キャラポインタ共通
		class SHARED_COMMON {
		protected:
			bool chage_ride = false;
		public:
			std::shared_ptr<PLAYER_CHARA>* MINE_c{ nullptr };
			std::vector<std::shared_ptr<PLAYER_CHARA>>* ALL_c{ nullptr };
			std::shared_ptr<PLAYER_VEHICLE>* MINE_v{ nullptr };
			std::vector<std::shared_ptr<PLAYER_VEHICLE>>* ALL_v{ nullptr };
		public:
			bool isRide(void) const noexcept { return this->MINE_v != nullptr && MINE_c != nullptr; }
			bool changeRide(void) const noexcept { return this->chage_ride; }
			void Ride_on(std::shared_ptr<PLAYER_VEHICLE>* MINE_v_t) noexcept {
				this->chage_ride = true;
				if (MINE_v_t != nullptr) {
					if (isRide()) {
						Ride_on(nullptr);
					}
					this->MINE_v = MINE_v_t;
					(*this->MINE_v)->RideOn(this->MINE_c);
				}
				else {
					(*this->MINE_v)->GetOff(this->MINE_c);
					this->MINE_v = MINE_v_t;
				}
			}
			//使用するポインタ設定
			virtual void Set_Ptr(
				std::vector<std::shared_ptr<PLAYER_CHARA>>* ALL_c_t, std::shared_ptr<PLAYER_CHARA>* MINE_c_t,
				std::vector<std::shared_ptr<PLAYER_VEHICLE>>* ALL_v_t, std::shared_ptr<PLAYER_VEHICLE>* MINE_v_t
			) noexcept {
				ALL_c = ALL_c_t;
				MINE_c = MINE_c_t;
				ALL_v = ALL_v_t;
				MINE_v = MINE_v_t;
			}
		};
		//操作共通
		class PLAYER_COMMON : public PTR_COMMON, public SHARED_COMMON, public Effect_UseControl {
		public:
			//HP関連
			class Damages {
			private:
				//被弾角度
				class damage_rad {
				public:
					float alpfa{ 1.f };
					float rad{ 0.f };
				};
			private:
				unsigned int got_damage_color{ 0 };
				int got_damage_x{ 0 };
				float got_damage_f{ 0.f };
				std::vector<damage_rad> got_damage_rad;
				int got_damage{ 0 };
				int HP{ 100 };
				float HP_r{ 100.f };
				int HP_full{ 100 };
				std::vector<int> HP_parts;
			public:
				//getter
				const auto IsShow(void) const noexcept { return (int(this->HP_r) - this->HP) >= 2; }
				const auto Get_alive(void) const noexcept { return this->HP != 0; }																								//生きているか
				const auto HP_Per(void) const noexcept { return float(this->HP) / this->HP_full; }
				const auto HP_r_Per(void) const noexcept { return this->HP_r / this->HP_full; }
				const auto& Get_HP()const noexcept { return HP; }
				const auto& Get_HP_r()const noexcept { return HP_r; }
				const auto& Get_HP_full()const noexcept { return HP_full; }
				const auto& Get_got_damage()const noexcept { return got_damage; }
				const auto& Get_got_damage_rad(void) const noexcept { return got_damage_rad; }
				const auto& Get_got_damage_f()const noexcept { return got_damage_f; }
				const auto& Get_got_damage_x()const noexcept { return got_damage_x; }
				const auto& Get_got_damage_color()const noexcept { return got_damage_color; }
				const auto& Get_HP_parts()const noexcept { return HP_parts; }
			public:
				void SubHP_Parts(int damage_t, int parts_Set_t) noexcept {
					this->HP_parts[parts_Set_t] = std::max<int>(this->HP_parts[parts_Set_t] - damage_t, 0); //
				}
				void SubHP(int damage_t, float rad_t, bool isshow = true)  noexcept {
					auto old = this->HP;
					this->HP = std::max<int>(this->HP - damage_t, 0); //
					this->got_damage = old - this->HP;
					this->got_damage_color = GetColor(255, 255, 255);

					if (float(this->HP) / float(this->HP_full) <= 0.66) {
						this->got_damage_color = GetColor(255, 255, 0);
					}
					if (float(this->HP) / float(this->HP_full) <= 0.33) {
						this->got_damage_color = GetColor(255, 128, 0);
					}
					if (damage_t != this->got_damage) {
						this->got_damage_color = GetColor(255, 0, 0);
					}
					this->got_damage_x = -255 + GetRand(255 * 2);
					this->got_damage_f = 1.f;

					this->got_damage_rad.resize(this->got_damage_rad.size() + 1);
					this->got_damage_rad.back().alpfa = 1.f;
					this->got_damage_rad.back().rad = rad_t;

					if (!isshow) {
						this->HP_r = float(this->HP);
					}
				}
				void AddHP(int repair_t) noexcept {
					this->HP = std::clamp<int>(this->HP + repair_t, 0, this->HP_full);
				}
			public:
				void Set(int parts_num, int Full_t = -1) noexcept {
					if (Full_t > 0) {
						this->HP_full = Full_t;
					}
					this->HP = this->HP_full;
					this->HP_parts.resize(parts_num);
					for (auto& h : this->HP_parts) { h = this->HP_full; }//モジュール耐久

				}
				void Update(void) noexcept {
					easing_set(&this->HP_r, float(this->HP), 0.95f);
					easing_set(&this->got_damage_f, 0.f, 0.95f);

					{
						for (auto& d : this->got_damage_rad) { easing_set(&d.alpfa, 0.f, 0.975f); }
						while (true) {
							bool none = true;
							for (auto& d : this->got_damage_rad) {
								if (std::clamp(int(255.f * (1.f - powf(1.f - d.alpfa, 5.f))), 0, 255) < 5) {
									auto i = (&d - &this->got_damage_rad.front());
									std::iter_swap(this->got_damage_rad.begin() + i, this->got_damage_rad.end() - 1);
									this->got_damage_rad.pop_back();
									none = false;
									break;
								}
							}
							if (none) {
								break;
							}
						}
					}
				}
				void Dispose(void) noexcept {
					this->HP = 0;
					this->HP_parts.clear();
				}
			};
			//操作全般
			class opes {
			public:
				//
				bool wkey{ false };
				bool skey{ false };
				bool akey{ false };
				bool dkey{ false };
				bool qkey{ false };
				bool ekey{ false };
				//
				bool running{ false };			//走るか否か
				bool jamp{ false };				//ジャンプ
				bool shoot{ false };			//射撃
				bool reload{ false };			//リロード開始
				bool have_mag{ false };			//マガジンを持つか
				bool have_item{ false };		//アイテム取得
				bool sort_magazine{ false };	//マガジンソート
				bool view_gun{ false };			//マガジンソート
				bool drop_{ false };			//医療品落とす
				bool throw_gre{ false };		//グレ投げ
				bool aim{ false };				//エイム
				bool TPS{ false };				//TPS
				bool squat{ false };			//しゃがむ
				bool select{ false };			//セレクター
				bool ride{ false };				//乗る

				void ReSet_(void) noexcept {
					this->wkey = false;
					this->skey = false;
					this->akey = false;
					this->dkey = false;
					this->shoot = false;
					this->running = false;
					this->qkey = false;
					this->ekey = false;
					this->aim = false;
					this->TPS = false;
					this->reload = false;
					this->have_item = false;
					this->sort_magazine = false;
					this->view_gun = false;
					this->drop_ = false;
					this->throw_gre = false;
					this->select = false;
					this->have_mag = false;
					this->jamp = false;
					this->squat = false;
					this->ride = false;
				}
				void rule_(void) noexcept {
					if ((this->skey) || (this->aim) || (!this->wkey) || (!this->wkey && !this->skey && !this->akey && !this->dkey)) {
						this->running = false;
					}
					if (this->running) {
						this->squat = false;
						this->skey = false;
						this->qkey = false;
						this->ekey = false;
					}
				}
			};
		protected:
			MV1 obj_body;										//本体オブジェ
			MV1 obj_col;										//本体コリジョン
			moves move;											//プレイヤー座標
			moves spawn;										//プレイヤースポーン座標
			float body_xtrun{ 0.f };							//胴体角度
			float body_ztrun{ 0.f };							//胴体角度
			float speed_base{ 1.f };							//移動速度
			bool Flag_start_loop{ true };						//演算開始フラグ
			bool Flag_calc_body{ true };						//身体の物理演算フラグ
			bool nearhit{ false };								//

			float body_yrad{ 0.f };								//胴体角度
		private:
			std::array<EffectS, 12> effcs_gndhit;				/*エフェクト*/
			size_t use_effcsgndhit{ 0 };						/*エフェクト*/
			float dist = 100.f;
			float xrad_p{ 0.f };							//マウスエイム用変数確保
		public:
			VECTOR_ref add_vec_real;							//移動ベクトルバッファ
			Damages Damage;
		public:
			virtual const MATRIX_ref Head_direction(void)const noexcept { return MGetIdent(); }
			//setter
			void Set_xradP(float value) noexcept { this->xrad_p = value; }
			void Set_SpawnPos(const moves& move_) noexcept { this->spawn = move_; }			/*キャラスポーン*/
			//getter
			virtual const float Get_body_yrad(void) const noexcept { return this->body_yrad; }
			virtual const VECTOR_ref Get_pos(void) const noexcept { return this->move.pos; }
			virtual const bool is_ADS(void) const noexcept { return false; }						//ADS中
			//getter
			const auto& Get_xradP() const noexcept { return this->xrad_p; }
			const auto& Get_dist(void) const noexcept { return this->dist; }		//UI用
			const auto& Get_move(void) const noexcept { return this->move; }
			const auto Get_pseed_per(void) const noexcept {
				if (!isRide()) {
					return add_vec_real.size() / speed_base;
				}
				else {
					return (*MINE_v)->add_vec_real.size() / (*MINE_v)->speed_base;
				}
			}//移動速度のパーセンテージ
		protected:
			const float Add_xradP(float xrd, float min, float max) noexcept {
				this->xrad_p = std::clamp(this->xrad_p + xrd, min, max);
				return this->xrad_p;
			}
			//コリジョンの設定
			void SetupCollInfo(void) const noexcept {
				for (int i = 0; i < this->obj_col.mesh_num(); ++i) { this->obj_col.SetupCollInfo(8, 8, 8, -1, i); }
			}
			/*レーザー、ライト描画*/
			void Draw_Lazer_Effect(const VECTOR_ref& start_pos, const VECTOR_ref& vec_t, bool isDraw, float laser_siz) noexcept {
				auto EndPos = start_pos + vec_t.Norm() * 100.f;
				MAPPTs->map_col_nearest(start_pos, &EndPos);
				for (auto& tgt : *ALL_c) {
					if ((MINE_c != nullptr && tgt == *MINE_c) || !tgt->Damage.Get_alive()) { continue; }
					if (tgt->Set_ref_col(start_pos, EndPos, 2.f)) {
						tgt->col_nearest(start_pos, &EndPos);
					}
				}
				for (auto& tgt : *ALL_v) {
					if ((MINE_c != nullptr && tgt->MINE_c == MINE_c) || !tgt->Damage.Get_alive()) { continue; }
					if (tgt->Set_ref_col(start_pos, EndPos, 10.f)) {
						tgt->col_nearest(start_pos, &EndPos);
					}
				}
				dist = (start_pos - EndPos).size();
				if (isDraw) {
					SetUseLighting(FALSE);
					SetFogEnable(FALSE);
					{
						DXDraw::Capsule3D(start_pos, EndPos, 0.01f * laser_siz, GetColor(255, 0, 0), GetColor(255, 0, 0));
						DXDraw::Sphere3D(EndPos, std::clamp(powf((EndPos - GetCameraPosition()).size() + 0.5f, 2) * 0.002f + 0.08f * laser_siz, 0.001f, 0.02f + 0.03f * laser_siz), GetColor(255, 0, 0), GetColor(255, 0, 0));
					}
					SetUseLighting(TRUE);
					SetFogEnable(TRUE);
				}
			}
			/*HPバーを表示する場所*/
			const auto Set_HP_UI_Common(const VECTOR_ref& head) noexcept {
				if (this->Damage.IsShow()) {
					return VECTOR_ref(ConvWorldPosToScreenPos((head + VECTOR_ref::vget(0, 0.3f + 2.7f * std::max((head - GetCameraPosition()).size(), 1.f) / 100.f, 0)).get()));
				}
				else {
					return VECTOR_ref::front() * -1.f;
				}
			}
			//物理
			void Calc_Physics(void) noexcept {
				if (this->Flag_start_loop) {
					this->obj_body.PhysicsResetState();
					this->Flag_start_loop = false;
				}
				else {
					if (this->Flag_calc_body) {
						this->obj_body.PhysicsResetState();
						this->Flag_calc_body = false;
					}
					else {
						this->obj_body.PhysicsCalculation(1000.f / FPS);
					}
				}
			}
		public:
			bool Damage_Calc(int sel, int damage, const std::shared_ptr<PLAYER_COMMON>& tgt) {
				float rad = 0.f;
				float x_1 = sinf(tgt->body_yrad);
				float y_1 = cosf(tgt->body_yrad);
				auto vc = ((*MINE_c)->Get_Pos_Map() - tgt->Get_pos()).Norm();
				rad = atan2f(y_1 * vc.x() - x_1 * vc.z(), x_1 * vc.x() + y_1 * vc.z());
				tgt->Damage.SubHP(damage, rad);
				tgt->Damage.SubHP_Parts(damage, sel);
				if (!tgt->Damage.Get_alive()) {
					if (tgt->MINE_c != nullptr) {
						(*MINE_c)->scores.Set_kill(tgt->MINE_c - &ALL_c->front(), 70);
						(*tgt->MINE_c)->scores.Set_death(MINE_c - &ALL_c->front());
					}
					if (tgt->isRide()) {
						(*tgt->MINE_c)->Damage.SubHP(100, rad, false);
						(*tgt->MINE_c)->Ride_on(nullptr);
					}
				}
				return (!tgt->Damage.Get_alive());
			}
			void Set_gndhit(const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale) noexcept {
				this->effcs_gndhit[this->use_effcsgndhit].Set(pos_t, nomal_t, scale);
				++this->use_effcsgndhit %= this->effcs_gndhit.size();
			}
			//エフェクトの更新
			void Update_effect(void) noexcept {
				Effect_UseControl::Update_Effect();
				for (auto& t : this->effcs_gndhit) { t.put(effectControl.effsorce[(int)Effect::ef_gndsmoke]); }
			}
			//判定取得
			const MV1_COLL_RESULT_POLY col_line(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos, const int sel) const noexcept { return obj_col.CollCheck_Line(StartPos, EndPos, -1, sel); }
			void col_nearest(const VECTOR_ref& StartPos, VECTOR_ref* EndPos) const noexcept {
				MV1_COLL_RESULT_POLY colres;
				for (int i = 0; i < this->obj_col.mesh_num(); ++i) {
					colres = col_line(StartPos, *EndPos, i);
					if (colres.HitFlag == TRUE) {
						*EndPos = colres.HitPosition;
					}
				}
			}
			//判定起動
			virtual const bool Set_ref_col(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos, float range) noexcept {
				//すでに起動しているなら無視
				if (this->nearhit) { return true; }
				if (Segment_Point_MinLen(StartPos, EndPos, this->Get_pos()) <= range) {
					//判定起動
					this->nearhit = true;
					for (int i = 0; i < this->obj_col.mesh_num(); i++) {
						this->obj_col.RefreshCollInfo(-1, i);
					}
					return true;
				}
				return false;
			}
		public:
			/*コンストラクタ*/
			PLAYER_COMMON(void) noexcept { }
			/*スポーン*/
			void Spawn_Common(void) noexcept {
				this->xrad_p = 0;
				Damage.Set((int)this->obj_col.mesh_num());
			}
			/*最初*/
			virtual void Start(void) noexcept { }
			/*おわり*/
			virtual void Dispose(void) noexcept {
				Effect_UseControl::Dispose_Effect();
				for (auto& t : this->effcs_gndhit) { t.handle.Dispose(); }
			}
			virtual void Reset(void) noexcept {
				this->obj_body.Dispose();
				this->obj_col.Dispose();
				this->Damage.Dispose();
				this->speed_base = 1.f;
				this->move.vec.clear();
			}
		};

		//人間のフレーム、アニメ管理
		class HumanControl {
		private:
			//腕アニメ関連
			class anime_arm {
			public:
				float pers{ 0.f };
				float total{ 1.f };
			};
			class anime_vector {
			public:
				std::array<VECTOR_ref, 3> vec;		//座標
			};
			//体のフレーム情報
			class frame_body {
			public:
				//頭
				frames head_f;
				//左目
				frames LEFTeye_f;
				frames RIGHTeye_f;
				//胴体
				frames bodyg_f;
				frames bodyc_f;
				frames bodyb_f;
				frames body_f;
				//右手座標系
				frames RIGHThand2_f;
				frames RIGHThand_f;
				frames RIGHTarm2_f;
				frames RIGHTarm1_f;
				//左手座標系
				frames LEFThand2_f;
				frames LEFThand_f;
				frames LEFTarm2_f;
				frames LEFTarm1_f;

				//右手座標系
				frames RIGHTreg2_f;
				frames RIGHTreg_f;
				frames RIGHTfoot2_f;
				frames RIGHTfoot1_f;
				//左手座標系
				frames LEFTreg2_f;
				frames LEFTreg_f;
				frames LEFTfoot2_f;
				frames LEFTfoot1_f;
				//
				std::array<frames, 3> LEFTBodyFrame;		//左手座標(マガジン保持時)
			public:
				//
				void Get_frame(MV1& obj_) noexcept {
					for (int i = 0; i < int(obj_.frame_num()); ++i) {
						std::string p = obj_.frame_name(i);
						if (p == std::string("グルーブ")) {
							this->bodyg_f.Set_Local(i, obj_);
						}
						else if (p == std::string("下半身")) {
							this->bodyc_f.Set_Local(i, obj_);
						}

						else if (p.find("左足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
							this->LEFTfoot1_f.Set_Local(i, obj_);
						}
						else if (p.find("左ひざ") != std::string::npos) {
							this->LEFTfoot2_f.Set_Local(i, obj_);
						}
						else if (p.find("左足首") != std::string::npos && p.find("先") == std::string::npos) {
							this->LEFTreg_f.Set_Local(i, obj_);
						}
						else if (p.find("左足首先") != std::string::npos) {
							this->LEFTreg2_f.Set_Local(i, obj_);
						}

						else if (p.find("右足") != std::string::npos && p.find("首") == std::string::npos && p.find("先") == std::string::npos && p.find("ＩＫ") == std::string::npos) {
							this->RIGHTfoot1_f.Set_Local(i, obj_);
						}
						else if (p.find("右ひざ") != std::string::npos) {
							this->RIGHTfoot2_f.Set_Local(i, obj_);
						}
						else if (p.find("右足首") != std::string::npos && p.find("先") == std::string::npos) {
							this->RIGHTreg_f.Set_Local(i, obj_);
						}
						else if (p.find("右足首先") != std::string::npos) {
							this->RIGHTreg2_f.Set_Local(i, obj_);
						}
						else if (p.find("上半身") != std::string::npos && p.find("上半身2") == std::string::npos) {
							this->bodyb_f.Set_Local(i, obj_);
						}
						else if (p.find("上半身2") != std::string::npos) {
							this->body_f.Set_Local(i, obj_);
						}
						else if (p.find("頭") != std::string::npos && p.find("先") == std::string::npos) {
							this->head_f.Set_Local(i, obj_);
							//head_hight = obj_.frame(this->head_f.first).y();
						}
						else if (p.find("右目先") != std::string::npos) {
							this->RIGHTeye_f.Set_Local(i, obj_);
						}
						else if (p.find("左目先") != std::string::npos) {
							this->LEFTeye_f.Set_Local(i, obj_);
						}

						else if (p.find("右腕") != std::string::npos && p.find("捩") == std::string::npos) {
							this->RIGHTarm1_f.Set_Local(i, obj_);
						}
						else if (p.find("右ひじ") != std::string::npos) {
							this->RIGHTarm2_f.Set_Local(i, obj_);
						}
						else if (p == std::string("右手首")) {
							this->RIGHThand_f.Set_Local(i, obj_);
						}
						else if (p == std::string("右手先") || p == std::string("右手首先")) {
							this->RIGHThand2_f.Set_Local(i, obj_);
						}

						else if (p.find("左腕") != std::string::npos && p.find("捩") == std::string::npos) {
							this->LEFTarm1_f.Set_Local(i, obj_);
						}
						else if (p.find("左ひじ") != std::string::npos) {
							this->LEFTarm2_f.Set_Local(i, obj_);
						}
						else if (p == std::string("左手首")) {
							this->LEFThand_f.Set_Local(i, obj_);
						}
						else if (p == std::string("左手先") || p == std::string("左手首先")) {
							this->LEFThand2_f.Set_Local(i, obj_);
						}
						if (p == std::string("LEFT_mag")) {//LEFT
							this->LEFTBodyFrame[0].Set_World(i, obj_);
							this->LEFTBodyFrame[1].Set_Local(i + 1, obj_);
							this->LEFTBodyFrame[2].Set_Local(i + 2, obj_);
						}
					}
				}
				//
				void copy_frame(MV1& mine, frame_body& frame_tgt_, MV1* tgt) noexcept {
					tgt->SetMatrix(mine.GetMatrix());
					//
					tgt->SetFrameLocalMatrix(frame_tgt_.head_f.first, mine.GetFrameLocalMatrix(frame_tgt_.head_f.first));
					//
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTeye_f.first, mine.GetFrameLocalMatrix(frame_tgt_.LEFTeye_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTeye_f.first, mine.GetFrameLocalMatrix(frame_tgt_.RIGHTeye_f.first));
					//
					tgt->SetFrameLocalMatrix(frame_tgt_.bodyg_f.first, mine.GetFrameLocalMatrix(frame_tgt_.bodyg_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.bodyc_f.first, mine.GetFrameLocalMatrix(frame_tgt_.bodyc_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.bodyb_f.first, mine.GetFrameLocalMatrix(frame_tgt_.bodyb_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.body_f.first, mine.GetFrameLocalMatrix(frame_tgt_.body_f.first));
					//右手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.first, mine.GetFrameLocalMatrix(frame_tgt_.RIGHThand2_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHThand_f.first, mine.GetFrameLocalMatrix(frame_tgt_.RIGHThand_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.first, mine.GetFrameLocalMatrix(frame_tgt_.RIGHTarm2_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.first, mine.GetFrameLocalMatrix(frame_tgt_.RIGHTarm1_f.first));
					//左手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand2_f.first, mine.GetFrameLocalMatrix(frame_tgt_.LEFThand2_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFThand_f.first, mine.GetFrameLocalMatrix(frame_tgt_.LEFThand_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.first, mine.GetFrameLocalMatrix(frame_tgt_.LEFTarm2_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.first, mine.GetFrameLocalMatrix(frame_tgt_.LEFTarm1_f.first));
					//右手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.first, mine.GetFrameLocalMatrix(frame_tgt_.RIGHTreg2_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.first, mine.GetFrameLocalMatrix(frame_tgt_.RIGHTreg_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.first, mine.GetFrameLocalMatrix(frame_tgt_.RIGHTfoot2_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.first, mine.GetFrameLocalMatrix(frame_tgt_.RIGHTfoot1_f.first));
					//左手座標系
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg2_f.first, mine.GetFrameLocalMatrix(frame_tgt_.LEFTreg2_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTreg_f.first, mine.GetFrameLocalMatrix(frame_tgt_.LEFTreg_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot2_f.first, mine.GetFrameLocalMatrix(frame_tgt_.LEFTfoot2_f.first));
					tgt->SetFrameLocalMatrix(frame_tgt_.LEFTfoot1_f.first, mine.GetFrameLocalMatrix(frame_tgt_.LEFTfoot1_f.first));
					for (int i = 0; i < tgt->get_anime().size(); ++i) {
						tgt->get_anime(i).per = mine.get_anime(i).per;
						tgt->get_anime(i).time = mine.get_anime(i).time;
					}
				}
				//
			};
			//
			class Pos_VRJoint {
			public:
				moves move;
				MATRIX_ref mat_notlean;
#ifdef _USE_OPENVR_
			public:
				moves move_start;
			private:
				std::shared_ptr<DXDraw> DrawPts{ nullptr };
				bool oldv_2{ false };
				bool oldv_2_2{ false };
			public:
				auto Get_pos_noY(void) const noexcept { return VECTOR_ref::vget(this->move.pos.x(), 0.f, this->move.pos.z()); }
				auto Get_pos_old_noY(void) const noexcept { return VECTOR_ref::vget(this->move.repos.x(), 0.f, this->move.repos.z()); }
				MATRIX_ref Get_mat_XZ(void) const noexcept { return MATRIX_ref::Axis1(this->move.mat.xvec() * -1.f, this->move.mat.yvec(), this->move.mat.zvec() * -1.f); }

				void Set(std::shared_ptr<DXDraw>& DrawPts_t) noexcept {
					DrawPts = DrawPts_t;
					oldv_2 = true;
					oldv_2_2 = true;
				}
				void Update_none(const char& device_num) noexcept {
					DrawPts->GetDevicePositionVR(device_num, &this->move.pos, &this->move.mat);
					this->move.mat = Get_mat_XZ();
				}
				void Update_pos(const char& device_num, const bool Flag_start_loop_t) noexcept {
					DrawPts->GetDevicePositionVR(device_num, &this->move.pos, &this->move.mat);

					auto ptr_ = &(*DrawPts->get_device())[device_num];
					if (Flag_start_loop_t || (ptr_->turn && ptr_->now) != this->oldv_2) {
						this->move_start.SetPos(Get_pos_noY());
					}
					this->oldv_2 = ptr_->turn && ptr_->now;
				}
				void Update_mat(const char& device_num, const bool Flag_start_loop_t) noexcept {
					DrawPts->GetDevicePositionVR(device_num, &this->move.pos, &this->move.mat);
					this->move.mat = Get_mat_XZ();

					auto ptr_ = &(*DrawPts->get_device())[device_num];
					if ((Flag_start_loop_t || (ptr_->turn && ptr_->now) != this->oldv_2) && this->oldv_2_2) {
						this->move_start.mat = this->move.mat;
					}
					this->oldv_2 = ptr_->turn && ptr_->now;
				}
#endif // _USE_OPENVR_
			};
		private:
			anime_vector* prev_ani_lefthand{ nullptr };
			anime_vector* next_ani_lefthand{ nullptr };
			anime_arm* now_timeline{ nullptr };
			std::vector<anime_arm> ani_timeline;
			std::vector<anime_vector> ani_lefthand;
			std::vector<anime_vector> ani_righthand;
			VECTOR_ref vec_gunani{ VECTOR_ref::front() };
			VECTOR_ref pos_gunani;
			//体のアニメ情報
			MV1::ani* anime_sit{ nullptr };					//脚ポーズ
			MV1::ani* anime_wake{ nullptr };				//脚ポーズ
			MV1::ani* anime_jump{ nullptr };				//脚ポーズ
			MV1::ani* anime_hand_nomal{ nullptr };			//基本手ポーズ
			MV1::ani* anime_hand_trigger{ nullptr };		//引き金ポーズ
			MV1::ani* anime_hand_trigger_pull{ nullptr };	//引き金引くポーズ
			//体のフレーム情報
			frame_body lagframe_;							//フレーム
			frame_body colframe_;							//フレーム
			float zrad_gunani{ 0.f };
		protected:
			//体のアニメ情報
			MV1::ani* anime_walk{ nullptr };				//脚anime
			MV1::ani* anime_swalk{ nullptr };				//脚anime
			MV1::ani* anime_run{ nullptr };					//脚anime
			//VR用の関節
			Pos_VRJoint HMD;								//頭部座標系
			Pos_VRJoint RIGHTHAND;							//右手座標系
			Pos_VRJoint LEFTHAND;							//左手座標系
			Pos_VRJoint RIGHTREG;							//右脚座標系
			Pos_VRJoint LEFTREG;							//左脚座標系
			Pos_VRJoint WAIST;								//腰座標
			bool LEFT_hand{ false };						//左手を添えるフラグ
			float ratio_move{ 0.f };						//移動の度合い(アニメ)
			float add_ypos{ 0.f };							//垂直加速度
			//体のフレーム情報
			frame_body frame_s;								//フレーム
		public:
			//getter
			virtual const bool isJamping(void) const noexcept { return this->add_ypos != 0.f; }																			//ジャンプ中
			virtual const bool isSquat(void) const noexcept { return false; }
			virtual const bool isRunning(void) const noexcept { return false; }
			const auto& Get_pos_LEFTHAND(void) const noexcept { return LEFTHAND.move.pos; }																					//item関連
			const auto& Get_mat_LEFTHAND(void) const noexcept { return LEFTHAND.move.mat; }																					//item関連
		private:
			//腕アニメの指定関数
			auto Func_Set_LEFT_pos_Anim(anime_arm* anim, anime_vector* next_ani, const VECTOR_ref& vec_gun, const VECTOR_ref& pos_gun_t, float zrad_gun) noexcept {
				now_timeline = anim;
				easing_set(&vec_gunani, vec_gun, 0.8f);
				easing_set(&pos_gunani, pos_gun_t, 0.5f);
				easing_set(&zrad_gunani, zrad_gun, 0.9f);
				//更新
				if (now_timeline->pers < now_timeline->total) {
					next_ani_lefthand = next_ani;
					now_timeline->pers += 1.f / FPS;
				}
				if (now_timeline->pers >= now_timeline->total) {
					prev_ani_lefthand = next_ani_lefthand;
					now_timeline->pers = now_timeline->total;
				}
				//リセット処理
				for (auto& tt : this->ani_timeline) {
					if (now_timeline != &tt) {
						tt.pers = 0.f;
					}
				}
				return now_timeline->pers == now_timeline->total;//終了フラグ
			}
		protected:
			void Frame_Copy_Col(MV1& obj_body_t, MV1* obj_col_t) { this->frame_s.copy_frame(obj_body_t, this->colframe_, obj_col_t); }
			void Frame_Copy_Lag(MV1& obj_body_t, MV1* obj_lag_t) { this->frame_s.copy_frame(obj_body_t, this->lagframe_, obj_lag_t); }
			//
			void Set_ReloadTime(float reload_time) noexcept {
				this->ani_timeline[3].total = reload_time -
					(this->ani_timeline[2].total + this->ani_timeline[15].total + this->ani_timeline[16].total + this->ani_timeline[17].total + this->ani_timeline[4].total);//マガジン装着
			}
			void Set_Body(MV1& obj_body_t, MV1& obj_lag_t, MV1& obj_col_t) noexcept {
				//身体
				this->frame_s.Get_frame(obj_body_t);
				this->anime_hand_nomal = &obj_body_t.get_anime(0);
				this->anime_hand_trigger = &obj_body_t.get_anime(1);
				this->anime_hand_trigger_pull = &obj_body_t.get_anime(2);
				this->anime_swalk = &obj_body_t.get_anime(3);
				this->anime_walk = &obj_body_t.get_anime(4);
				this->anime_run = &obj_body_t.get_anime(5);
				this->anime_wake = &obj_body_t.get_anime(6);
				this->anime_sit = &obj_body_t.get_anime(7);
				this->anime_jump = &obj_body_t.get_anime(8);
				//ラグドール
				this->lagframe_.Get_frame(obj_lag_t);
				//身体コリジョン
				this->colframe_.Get_frame(obj_col_t);
			}
			//
			void Init_Anim(void) noexcept {
				this->ani_timeline.resize(18);
				this->ani_lefthand.resize(4);
				this->ani_righthand.resize(1);
				this->ani_timeline[0].total = 0.4f;//構える
				this->ani_timeline[1].total = 0.4f;//走る

				this->ani_timeline[2].total = 0.85f;//マガジンリリース
				this->ani_timeline[15].total = 0.55f;//マガジン装着//1.4
				this->ani_timeline[16].total = 0.25f;//マガジン装着//1.65
				this->ani_timeline[3].total = 0.3f;//マガジン装着(可変)//1.95
				this->ani_timeline[17].total = 0.15f;//マガジン装着//2.1
				this->ani_timeline[4].total = 0.2f;//マガジン装着//2.3

				this->ani_timeline[5].total = 0.2f;//マグ整頓
				this->ani_timeline[6].total = 0.2f;//初段装填
				this->ani_timeline[7].total = 0.4f;//眺め1
				this->ani_timeline[8].total = 1.6f;//眺め2
				this->ani_timeline[9].total = 0.4f;//眺め3
				this->ani_timeline[10].total = 1.6f;//眺め4

				this->ani_timeline[11].total = 0.4f;//左腕が落ちる
				this->ani_timeline[12].total = 0.8f;//グレ投擲
				this->ani_timeline[13].total = 0.8f;//セレクターセット

				this->ani_timeline[14].total = 0.4f;//構える
			}
			void Init_Left(void) noexcept {
				this->prev_ani_lefthand = &this->ani_lefthand[0];
				this->next_ani_lefthand = &this->ani_lefthand[0];
			}
			//腕アニメで使う座標の決定
			void Set_lefthand(int sel_anim, int sel_t, const VECTOR_ref& vec) noexcept {
				ani_lefthand[sel_anim].vec[sel_t] = vec;
			}
			void Set_Righthand(int sel_anim, int sel_t, const VECTOR_ref& vec) noexcept {
				ani_righthand[sel_anim].vec[sel_t] = vec;
			}
			//腕アニメの指定関数
			auto Func_Set_LEFT_pos_Anim(int anim, int next_ani, const VECTOR_ref& vec_gun, const VECTOR_ref& pos_gun_t, float zrad_gun) noexcept {
				return Func_Set_LEFT_pos_Anim(&this->ani_timeline[anim], &this->ani_lefthand[next_ani], vec_gun, pos_gun_t, zrad_gun);
			}
			//
			//腕アニメを参照
			const auto LEFT_pos_Anim(size_t sel_t) noexcept {
				if (now_timeline != nullptr) {
					if (now_timeline->pers < now_timeline->total) {
						auto pers = now_timeline->pers / now_timeline->total;
						return prev_ani_lefthand->vec[sel_t] * (1.f - pers) + next_ani_lefthand->vec[sel_t] * pers;
					}
					else {
						return next_ani_lefthand->vec[sel_t];
					}
				}
				else {
					return next_ani_lefthand->vec[sel_t];
				}
			}
			//右腕座標(固定)
			const auto RIGHT_pos_Anim(size_t sel_t) noexcept { return ani_righthand[0].vec[sel_t]; }
			//
			void Put_LeftHand(moves* tmp) noexcept {
				moves base = *tmp;
				tmp->pos = base.pos + MATRIX_ref::Vtrans(pos_gunani, base.mat);
				tmp->mat = MATRIX_ref::RotZ(deg2rad(zrad_gunani)) * MATRIX_ref::RotVec2(VECTOR_ref::front(), vec_gunani) * base.mat;
			}
			//腕のフレーム操作
			void move_LeftArm(std::shared_ptr<DXDraw>& DrawPts, const MV1& obj_body_t, const bool isusewaist, const MATRIX_ref& m_inv) noexcept {
				//基準
				auto vec_a1 = MATRIX_ref::Vtrans((this->LEFTHAND.move.pos - obj_body_t.frame(this->frame_s.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//基準
				auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(0.f, -1.f, vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0とする
				float cos_t = getcos_tri((obj_body_t.frame(this->frame_s.LEFThand_f.first) - obj_body_t.frame(this->frame_s.LEFTarm2_f.first)).size(), (obj_body_t.frame(this->frame_s.LEFTarm2_f.first) - obj_body_t.frame(this->frame_s.LEFTarm1_f.first)).size(), (obj_body_t.frame(this->frame_s.LEFTarm1_f.first) - this->LEFTHAND.move.pos).size());
				auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
				//上腕
				if ((DrawPts->use_vr && !isusewaist) || !DrawPts->use_vr) {
					BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFTarm1_f);
				}
				if (DrawPts->use_vr && isusewaist) {
					BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFTarm1_f, (this->WAIST.move.mat * m_inv.Inverse()).Inverse());
				}
				MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(obj_body_t.frame(this->frame_s.LEFTarm2_f.first) - obj_body_t.frame(this->frame_s.LEFTarm1_f.first), m_inv.Inverse()), vec_t);
				if ((DrawPts->use_vr && !isusewaist) || !DrawPts->use_vr) {
					BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFTarm1_f, a1_inv);
				}
				if (DrawPts->use_vr && isusewaist) {
					BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFTarm1_f, a1_inv * (this->WAIST.move.mat * m_inv.Inverse()).Inverse());
				}
				//下腕
				BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFTarm2_f);
				MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(obj_body_t.frame(this->frame_s.LEFThand_f.first) - obj_body_t.frame(this->frame_s.LEFTarm2_f.first), m_inv.Inverse() * a1_inv.Inverse()), MATRIX_ref::Vtrans(this->LEFTHAND.move.pos - obj_body_t.frame(this->frame_s.LEFTarm2_f.first), m_inv.Inverse() * a1_inv.Inverse()));
				BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFTarm2_f, a2_inv);
				//手
				BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFThand_f);
				if (!this->LEFT_hand) {
					BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFThand_f, MATRIX_ref::RotZ(deg2rad(0)) * MATRIX_ref::RotX(deg2rad(80)) * this->LEFTHAND.move.mat * m_inv.Inverse() * a1_inv.Inverse() * a2_inv.Inverse());
				}
				else {
					auto ans1_ = this->LEFT_pos_Anim(0);
					auto ans2_ = this->LEFT_pos_Anim(1);
					auto ans3_ = this->LEFT_pos_Anim(2);
					MATRIX_ref a3_inv = MATRIX_ref::RotVec2(
						MATRIX_ref::Vtrans(obj_body_t.frame(this->frame_s.LEFThand2_f.first) - obj_body_t.frame(this->frame_s.LEFThand_f.first), m_inv.Inverse() * a1_inv.Inverse() * a2_inv.Inverse()),
						MATRIX_ref::Vtrans(ans2_ - ans1_, m_inv.Inverse() * a1_inv.Inverse() * a2_inv.Inverse()));
					BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFThand_f, a3_inv);
					MATRIX_ref a3_yvec = MATRIX_ref::RotVec2(
						MATRIX_ref::Vtrans(BodyFrameMatrix(obj_body_t, this->frame_s.LEFThand_f).zvec() * -1.f, m_inv.Inverse() * a1_inv.Inverse() * a2_inv.Inverse() * a3_inv.Inverse()),
						MATRIX_ref::Vtrans(ans3_ - ans1_, m_inv.Inverse() * a1_inv.Inverse() * a2_inv.Inverse() * a3_inv.Inverse()));
					BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFThand_f, a3_yvec * a3_inv);
				}
			}
			void move_RightArm(std::shared_ptr<DXDraw>& DrawPts, const MV1& obj_body_t, const bool isusewaist, const MATRIX_ref& m_inv) noexcept {
				//基準
				auto vec_a1 = MATRIX_ref::Vtrans((this->RIGHTHAND.move.pos - obj_body_t.frame(this->frame_s.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());
				auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(-1.f * sin(deg2rad(30)), -1.f * cos(deg2rad(30)), vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0とする
				float cos_t = getcos_tri((obj_body_t.frame(this->frame_s.RIGHThand_f.first) - obj_body_t.frame(this->frame_s.RIGHTarm2_f.first)).size(), (obj_body_t.frame(this->frame_s.RIGHTarm2_f.first) - obj_body_t.frame(this->frame_s.RIGHTarm1_f.first)).size(), (obj_body_t.frame(this->frame_s.RIGHTarm1_f.first) - this->RIGHTHAND.move.pos).size());
				auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
				//上腕
				if ((DrawPts->use_vr && !isusewaist) || !DrawPts->use_vr) {
					BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHTarm1_f);
				}
				if (DrawPts->use_vr && isusewaist) {
					BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHTarm1_f, (this->WAIST.move.mat * m_inv.Inverse()).Inverse());
				}
				MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(obj_body_t.frame(this->frame_s.RIGHTarm2_f.first) - obj_body_t.frame(this->frame_s.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
				if ((DrawPts->use_vr && !isusewaist) || !DrawPts->use_vr) {
					BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHTarm1_f, a1_inv);
				}
				if (DrawPts->use_vr && isusewaist) {
					BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHTarm1_f, a1_inv * (this->WAIST.move.mat * m_inv.Inverse()).Inverse());
				}
				//下腕
				BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHTarm2_f);
				MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(obj_body_t.frame(this->frame_s.RIGHThand_f.first) - obj_body_t.frame(this->frame_s.RIGHTarm2_f.first), m_inv.Inverse() * a1_inv.Inverse()), MATRIX_ref::Vtrans(this->RIGHTHAND.move.pos - obj_body_t.frame(this->frame_s.RIGHTarm2_f.first), m_inv.Inverse() * a1_inv.Inverse()));
				BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHTarm2_f, a2_inv);
				//手
				BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHThand_f);
				{
					auto ans1_ = this->RIGHT_pos_Anim(0);
					auto ans2_ = this->RIGHT_pos_Anim(1);
					auto ans3_ = this->RIGHT_pos_Anim(2);
					MATRIX_ref a3_inv = MATRIX_ref::RotVec2(
						MATRIX_ref::Vtrans(obj_body_t.frame(this->frame_s.RIGHThand2_f.first) - obj_body_t.frame(this->frame_s.RIGHThand_f.first), m_inv.Inverse() * a1_inv.Inverse() * a2_inv.Inverse()),
						MATRIX_ref::Vtrans(ans2_ - ans1_, m_inv.Inverse() * a1_inv.Inverse() * a2_inv.Inverse()));
					BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHThand_f, a3_inv);
					MATRIX_ref a3_yvec = MATRIX_ref::RotVec2(
						MATRIX_ref::Vtrans(BodyFrameMatrix(obj_body_t, this->frame_s.RIGHThand_f).zvec() * -1.f, m_inv.Inverse() * a1_inv.Inverse() * a2_inv.Inverse() * a3_inv.Inverse()),
						MATRIX_ref::Vtrans(ans3_ - ans1_, m_inv.Inverse() * a1_inv.Inverse() * a2_inv.Inverse() * a3_inv.Inverse()));
					BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHThand_f, a3_yvec * a3_inv);
				}
			}
			//脚のフレーム操作
			void move_LeftLeg(const MV1& obj_body_t, const MATRIX_ref& m_inv) noexcept {
				{
					//基準
					auto tgt_pt = this->LEFTREG.move.pos;
					auto vec_a1 = MATRIX_ref::Vtrans((tgt_pt - obj_body_t.frame(this->frame_s.LEFTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
					//auto vec_a1L1 = (this->LEFTREG.mat*this->HMD.mat.Inverse()).zvec()*-1.f;//x=0とする

					auto vec_a1L1 = VECTOR_ref::front() * -1.f;

					float cos_t = getcos_tri((obj_body_t.frame(this->frame_s.LEFTreg_f.first) - obj_body_t.frame(this->frame_s.LEFTfoot2_f.first)).size(), (obj_body_t.frame(this->frame_s.LEFTfoot2_f.first) - obj_body_t.frame(this->frame_s.LEFTfoot1_f.first)).size(), (obj_body_t.frame(this->frame_s.LEFTfoot1_f.first) - tgt_pt).size());
					auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
					//上腕
					BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFTfoot1_f);
					MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(obj_body_t.frame(this->frame_s.LEFTfoot2_f.first) - obj_body_t.frame(this->frame_s.LEFTfoot1_f.first), m_inv.Inverse()), vec_t);
					BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFTfoot1_f, a1_inv);

					//下腕
					BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFTfoot2_f);
					MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(obj_body_t.frame(this->frame_s.LEFTreg_f.first) - obj_body_t.frame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse() * a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - obj_body_t.frame(this->frame_s.LEFTfoot2_f.first), m_inv.Inverse() * a1_inv.Inverse()));
					BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFTfoot2_f, a2_inv);
					//手
					BodyFrameSetMatrix(obj_body_t, this->frame_s.LEFTreg_f, this->LEFTREG.move.mat * m_inv.Inverse() * a1_inv.Inverse() * a2_inv.Inverse());
				}
			}
			void move_RightLeg(const MV1& obj_body_t, const MATRIX_ref& m_inv) noexcept {
				{
					//基準
					auto tgt_pt = this->RIGHTREG.move.pos;
					auto vec_a1 = MATRIX_ref::Vtrans((tgt_pt - obj_body_t.frame(this->frame_s.RIGHTfoot1_f.first)).Norm(), m_inv.Inverse());//基準
					//auto vec_a1L1 = (this->RIGHTREG.mat*this->HMD.mat.Inverse()).zvec()*-1.f;//x=0とする


					auto vec_a1L1 = VECTOR_ref::front() * -1.f;

					float cos_t = getcos_tri((obj_body_t.frame(this->frame_s.RIGHTreg_f.first) - obj_body_t.frame(this->frame_s.RIGHTfoot2_f.first)).size(), (obj_body_t.frame(this->frame_s.RIGHTfoot2_f.first) - obj_body_t.frame(this->frame_s.RIGHTfoot1_f.first)).size(), (obj_body_t.frame(this->frame_s.RIGHTfoot1_f.first) - tgt_pt).size());
					auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
					//上腕
					BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHTfoot1_f);
					MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(obj_body_t.frame(this->frame_s.RIGHTfoot2_f.first) - obj_body_t.frame(this->frame_s.RIGHTfoot1_f.first), m_inv.Inverse()), vec_t);
					BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHTfoot1_f, a1_inv);
					//下腕
					BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHTfoot2_f);
					MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(obj_body_t.frame(this->frame_s.RIGHTreg_f.first) - obj_body_t.frame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse() * a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - obj_body_t.frame(this->frame_s.RIGHTfoot2_f.first), m_inv.Inverse() * a1_inv.Inverse()));
					BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHTfoot2_f, a2_inv);
					//手
					BodyFrameSetMatrix(obj_body_t, this->frame_s.RIGHTreg_f, this->RIGHTREG.move.mat * m_inv.Inverse() * a1_inv.Inverse() * a2_inv.Inverse());
				}
			}
			//アニメによる脚操作
			void move_Leg_Anim(void) noexcept {
				float ani_per = 0.95f;//7

				float ani_walk = 0.f;//1
				float ani_run = 0.f;//2
				float ani_sit = 0.f;//7
				float ani_jump = 0.f;//7
				float ani_wake = 0.f;//11
				float ani_swalk = 0.f;//8

				if (this->isRunning()) {
					ani_run = this->ratio_move;
				}
				else {
					if (!this->isSquat()) {
						ani_wake = 1.f - this->ratio_move;
						ani_walk = this->ratio_move;
					}
					else {
						ani_sit = 1.f - this->ratio_move;
						ani_swalk = this->ratio_move;
					}
				}
				if (this->isJamping()) {
					ani_jump = 1.f;
					ani_walk = 0.f;//1
					ani_run = 0.f;//2
					ani_sit = 0.f;//7
					ani_wake = 0.f;//11
					ani_swalk = 0.f;//8
					ani_per = 0.8f;
				}

				//しゃがみ
				easing_set(&this->anime_jump->per, ani_jump, ani_per);
				//しゃがみ
				easing_set(&this->anime_sit->per, ani_sit, ani_per);
				//立ち
				easing_set(&this->anime_wake->per, ani_wake, ani_per);
				//走り
				easing_set(&this->anime_run->per, ani_run, ani_per);
				this->anime_run->Update(true, 1.f);
				//歩き
				easing_set(&this->anime_walk->per, ani_walk, ani_per);
				this->anime_walk->Update(true, 1.f);
				//しゃがみ歩き
				easing_set(&this->anime_swalk->per, ani_swalk, ani_per);
				this->anime_swalk->Update(true, 1.f);
			}
			//アニメによる指操作
			void move_Finger(float trigger_per) noexcept {
				this->anime_hand_nomal->per = 1.f;
				this->anime_hand_trigger_pull->per = trigger_per;
				this->anime_hand_trigger->per = 1.f - this->anime_hand_trigger_pull->per;
			}
			//リセット
			void Anim_Reset(void) noexcept {
				easing_set(&this->anime_swalk->per, 0.f, 0.9f);
				easing_set(&this->anime_walk->per, 0.f, 0.9f);
				easing_set(&this->anime_run->per, 0.f, 0.9f);

				easing_set(&this->anime_jump->per, 0.f, 0.9f);
				easing_set(&this->anime_sit->per, 0.f, 0.9f);
				easing_set(&this->anime_wake->per, 0.f, 0.9f);

				easing_set(&this->anime_hand_nomal->per, 0.f, 0.9f);
				easing_set(&this->anime_hand_trigger->per, 0.f, 0.9f);
				easing_set(&this->anime_hand_trigger_pull->per, 0.f, 0.9f);
			}
			void Frame_Reset(const MV1& obj_body_t) noexcept {
				obj_body_t.frame_Reset(this->frame_s.bodyg_f.first);
				obj_body_t.frame_Reset(this->frame_s.bodyb_f.first);
				obj_body_t.frame_Reset(this->frame_s.body_f.first);
				obj_body_t.frame_Reset(this->frame_s.head_f.first);
				obj_body_t.frame_Reset(this->frame_s.RIGHTarm1_f.first);
				obj_body_t.frame_Reset(this->frame_s.RIGHTarm1_f.first);
				obj_body_t.frame_Reset(this->frame_s.RIGHTarm2_f.first);
				obj_body_t.frame_Reset(this->frame_s.RIGHTarm2_f.first);
				obj_body_t.frame_Reset(this->frame_s.RIGHThand_f.first);
				obj_body_t.frame_Reset(this->frame_s.LEFTarm1_f.first);
				obj_body_t.frame_Reset(this->frame_s.LEFTarm1_f.first);
				obj_body_t.frame_Reset(this->frame_s.LEFTarm2_f.first);
				obj_body_t.frame_Reset(this->frame_s.LEFTarm2_f.first);
				obj_body_t.frame_Reset(this->frame_s.LEFThand_f.first);
			}
		};
		//実際に発射される弾
		class BULLETS : public PTR_COMMON, public SHARED_COMMON {
		private:
			bool First_Flag{ false };
			bool Hit_Flag{ false };
			float Hit_cnt{ 0.f };
			bool Flag{ false };
			int pene_cnt{ 0 };
			bool DrawFlag{ false };
			int cnt{ 0 };
			Ammos* spec{ nullptr };
			moves move;
			float speed{ 0.f };
			float pene{ 0.f };
			VECTOR_ref base_pos;
		public:
			float Hit_alpha{ 0.f };
			int Hit_window_x{ 0 };
			int Hit_window_y{ 0 };
		public:
			void Penetration(const std::pair<int, float>& a, const std::shared_ptr<PLAYER_VEHICLE>& tgt, const  VECTOR_ref& position, const  VECTOR_ref& normal) noexcept {
				//貫通
				if (this->pene > a.second * (1.0f / std::abs(this->move.vec.Norm().dot(normal)))) {
					SE.Get(EnumSound::Tank_Damage).Play_3D(0, position, 50.f, 64);
					if ((*MINE_v)->Damage_Calc(a.first, this->spec->Get_damage(), tgt)) {
						//撃破時エフェクト
					}
					else {

					}
					this->Flag = false;									//弾フラグ削除
				}
				//はじく
				else {
					//this->pene *= 0.8f;
					SE.Get(EnumSound::Tank_Ricochet).Play_3D(0, position, 50.f, 64);
					this->pene_cnt++;
				}
				//はじく処理
				this->move.vec = (this->move.vec + normal * ((this->move.vec.dot(normal)) * -2.0f)).Norm();
				this->move.pos = this->move.vec * (0.1f) + position;
				//
				hit_effect(normal);
				//弾痕のセット
				tgt->SetHit(tgt->Get_move(), position, normal, this->move.vec.Norm(), this->spec->Get_caliber(), !this->Flag);
			}
			void hit_effect(const VECTOR_ref& nomal) noexcept {
				if (isRide()) {
					(*this->MINE_v)->Set_gndhit(this->move.pos, nomal, ((this->spec->Get_caliber() >= 0.020f) ? this->spec->Get_caliber() : 0.025f) / 0.1f);
				}
				else {
					(*this->MINE_c)->Set_gndhit(this->move.pos, nomal, ((this->spec->Get_caliber() >= 0.020f) ? this->spec->Get_caliber() : 0.025f) / 0.1f);
				}
			}
		public:
			void Set(std::shared_ptr<PLAYER_CHARA>* mine_c, std::shared_ptr<PLAYER_VEHICLE>* mine_v) noexcept {
				MINE_c = mine_c;
				MINE_v = mine_v;
				this->Flag = false;
				this->pene_cnt = 0;
				this->First_Flag = false;
				this->DrawFlag = false;
			}
			void Put(Ammos* spec_t, const moves& move_) noexcept {
				this->Hit_Flag = false;
				this->Flag = true;
				this->pene_cnt = 0;
				this->First_Flag = true;
				this->DrawFlag = true;
				this->cnt = 0;
				this->spec = spec_t;
				this->move = move_;
				this->move.repos = this->move.pos;
				this->base_pos = this->move.pos;
				this->speed = this->spec->Get_speed();
				this->pene = this->spec->Get_pene();
			}
		private:
			bool CheckHit(int sel, int damage, const std::shared_ptr<PLAYER_CHARA>& tgt) noexcept {
				auto q = tgt->col_line(this->move.repos, this->move.pos, sel);
				if (q.HitFlag == TRUE) {
					this->Hit_Flag = true;
					this->Flag = false;
					this->move.pos = q.HitPosition;
					(*MINE_c)->subHP(q.HitPosition, q.Normal, sel, damage, tgt);
					return true;
				}
				return false;
			}
		public:
			void Update(void) noexcept {
				if (this->Flag) {
					//移動確定
					this->move.SetPos(this->move.pos + (this->move.vec * (this->speed / FPS)));
					//判定
					{
						VECTOR_ref repos;
						if (this->First_Flag) {
							float range_base = (this->base_pos - this->move.pos).size();
							repos = this->move.pos + (this->move.repos - this->move.pos).Norm() * range_base;
						}
						else {
							float range = ((this->move.repos - this->move.pos).size() * FPS / 60.f);
							repos = this->move.pos + (this->move.repos - this->move.pos).Norm() * range;
						}
						auto ColResGround = MAPPTs->map_col_line(repos, this->move.pos);
						if (ColResGround.HitFlag == TRUE) {
							this->move.pos = ColResGround.HitPosition;
						}
						//*
						bool is_Hit = false;
						bool is_Hitall = false;
						std::optional<size_t> Hitnear;
						for (auto& tgt : *(*MINE_c)->ALL_v) {
							if (&tgt == MINE_v) {
								continue;
							}
							if (tgt->Set_ref_col(repos, this->move.pos, 10.0f)) {
								is_Hit = false;
								//とりあえず当たったかどうか探す
								{
									for (auto& m : tgt->use_veh->Get_module_mesh()) { is_Hit |= tgt->HitCheck_Tank(m, this->move); }	//モジュール
									for (auto& m : tgt->use_veh->Get_space_mesh()) { is_Hit |= tgt->HitCheck_Tank(m, this->move); }		//空間装甲
									for (auto& m : tgt->use_veh->Get_armer_mesh()) { is_Hit |= tgt->HitCheck_Tank(m.first, this->move); }		//装甲
								}
								is_Hitall |= is_Hit;
								//当たってない場合抜ける
								if (!is_Hit) {
									continue;
								}
								//当たり判定を近い順にソート
								tgt->hitControl.sort_Hit();
								//ダメージ面に届くまで判定
								for (auto& tt : tgt->hitControl.Get_hitssort()) {
									if (!tt.Hit_Check(Hitnear, *this, tgt)) { break; }
								}
								this->Hit_Flag = true;
								if (Hitnear.has_value()) {
									break;
								}
							}
						}
						for (auto& tgt : *(*MINE_c)->ALL_c) {
							if (&tgt == MINE_c || !tgt->Damage.Get_alive() || tgt->isRide()) {
								continue;
							}
							if (tgt->Set_ref_col(repos, this->move.pos, 2.f)) {
								//HEAD
								if (CheckHit(0, this->spec->Get_damage() * 3, tgt)) {
									break;
								}
								//BODY
								if (CheckHit(1, this->spec->Get_damage(), tgt)) {
									break;
								}
								//LEFTHAND
								if (CheckHit(2, this->spec->Get_damage() * 2 / 5, tgt)) {
									break;
								}
								//RIGHTHAND
								if (CheckHit(3, this->spec->Get_damage() * 2 / 5, tgt)) {
									break;
								}
								//LEG
								if (CheckHit(4, this->spec->Get_damage() * 4 / 5, tgt)) {
									break;
								}
							}
						}
						if (ColResGround.HitFlag == TRUE && !is_Hitall) {
							if (this->Flag) {
								this->Flag = false;
								//弾痕
								(*MINE_c)->Set_gndhit(this->move.pos, ColResGround.Normal, 0.025f / 0.1f);
								hit_obj_p.Set(this->spec->Get_caliber(), this->move.pos, ColResGround.Normal, this->move.vec);
								return;
							}
						}
						//血痕
						if (!this->Flag && !is_Hitall) {
							if (!(ColResGround.HitFlag == TRUE)) {
								auto vec = (this->move.pos - repos).Norm();
								ColResGround = MAPPTs->map_col_line(this->move.pos, this->move.pos + vec * 5.f);
							}
							if (ColResGround.HitFlag == TRUE) {
								(*MINE_c)->Set_gndhit(ColResGround.HitPosition, ColResGround.Normal, 0.025f / 0.1f);
								hit_b_obj_p.Set(0.35f, ColResGround.HitPosition, ColResGround.Normal, this->move.vec);
							}
						}
						//*/
					}
					//消す(スピードが0以下、貫通が0以下、5回反射する)
					if (this->speed <= 0.f || this->pene <= 0.f || this->pene_cnt > 5) {
						this->pene_cnt = 0;
						this->Flag = false;
					}
					if (!this->Flag) {
						this->Hit_Flag = true;
					}
					//this->speed -= 5.f / FPS;
					//this->pene -= 5.f / FPS;
					//
				}
			}
			void Set_Draw(void) noexcept {
				if (this->Hit_Flag) {
					this->Hit_Flag = false;
					this->Hit_cnt = 0.25f;
					auto p = ConvWorldPosToScreenPos(this->move.pos.get());
					if (p.z >= 0.f && p.z <= 1.f) {
						this->Hit_alpha = 1.f;
						this->Hit_window_x = int(p.x);
						this->Hit_window_y = int(p.y);
					}
				}
				if (this->Hit_cnt > 0.f) {
					easing_set(&this->Hit_alpha, 2.f, 0.95f);
					this->Hit_cnt -= 1.f / FPS;
				}
				else {
					easing_set(&this->Hit_alpha, 0.f, 0.8f);
					this->Hit_cnt = 0.f;
				}
			}
			void Draw(void) noexcept {
				if (this->DrawFlag) {

					float range = ((this->move.repos - this->move.pos).size() * FPS / 60.f);
					VECTOR_ref repos = this->move.pos + (this->move.repos - this->move.pos).Norm() * range;
					if (this->First_Flag) {
						float range_base = (this->base_pos - this->move.pos).size();
						repos = this->move.pos + (this->move.repos - this->move.pos).Norm() * range_base;
						if (range_base > range) {
							this->First_Flag = false;
						}
					}
					DXDraw::Capsule3D(this->move.pos, repos, ((this->spec->Get_caliber() - 0.00762f) * 0.1f + 0.00762f), GetColor(255, 255, 172), GetColor(255, 255, 255));
					if (!this->Flag) {
						this->DrawFlag = false;
					}
				}
			}
			/*おわり*/
			void Dispose(void) noexcept {
				MINE_c = nullptr;
				MINE_v = nullptr;
				PTR_COMMON::Dispose_Ptr_Common();
			}
		};
		//弾処理
		class BulletControl_Common {
		protected:
			std::array<BULLETS, max_bullet> bullet;			/*確保する弾*/
			size_t use_bullet{ 0 };							/*確保する弾*/
		public:
			//UI描画用用意
			virtual void Set_Draw_bullet(void) noexcept {
				for (auto& a : this->bullet) { a.Set_Draw(); }
			}
			virtual void Update_bullet(void) noexcept {
				for (auto& a : this->bullet) { a.Update(); }
			}
			//弾の生成
			void Create_bullet(Ammos* spec_t, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t) noexcept {
				moves tmp;
				tmp.SetPos(pos_t);
				tmp.vec = vec_t;
				this->bullet[this->use_bullet].Put(spec_t, tmp);
				++this->use_bullet %= this->bullet.size();
			}
			//弾
			virtual void SetUp_bullet(std::shared_ptr<MAPclass::Map>& MAPPTs_t, std::shared_ptr<DXDraw>& DrawPts_t) noexcept {
				for (auto& a : this->bullet) {
					a.Set_Ptr_Common(MAPPTs_t, DrawPts_t);
				}
			}
			virtual void SetUp_bullet_Ptr(std::shared_ptr<PLAYER_CHARA>* mine_c, std::shared_ptr<PLAYER_VEHICLE>* mine_v) noexcept {
				for (auto& a : this->bullet) {
					a.Set(mine_c, mine_v);
				}
			}
			/*弾道描画*/
			virtual void Draw_ammo(void) noexcept {
				for (auto& a : this->bullet) { a.Draw(); }
			}
			/* UI向けにヒット部分を表示*/
			virtual void Draw_Hit_UI(GraphHandle& Hit_Graph) noexcept {
				for (auto& a : this->bullet) {
					if (a.Hit_alpha >= 10.f / 255.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f * a.Hit_alpha));
						Hit_Graph.DrawRotaGraph(a.Hit_window_x, a.Hit_window_y, (float)y_r(a.Hit_alpha * 0.5f * 100.0f) / 100.f, 0.f, true);
					}
				}
			}
			/*おわり*/
			virtual void Dispose(void) noexcept {
				for (auto& a : this->bullet) { a.Dispose(); }			//弾
			}
		};
		//銃のフレーム、アニメ管理
		class GunControl: public BulletControl_Common {
		public:
			//所持弾数などの武器データ
			class MAG_STATUS {
			public:
			private:
				GUNPARTs* ptr_mag{ nullptr };			//パーツデータ
				MV1 obj_mag_;							//マガジンオブジェ
				std::array<MV1, 2> obj_ammo;			//弾オブジェ
				int Ammo_cnt{ 0 };					//マガジン内の弾数
			public:
				//setter
				void Add_Cnt(int ad) noexcept { this->Ammo_cnt = this->Ammo_cnt + ad; }
				//getter
				auto* Get_Info(void) noexcept { return this->ptr_mag; }
				const auto& Get_Cnt()const noexcept { return this->Ammo_cnt; }
				const auto& Get_Cap()const noexcept { return this->ptr_mag->Get_Ammo_Cap(); }
				const auto& Get_magazine_f(int i) const noexcept { return this->ptr_mag->mod.Get_magazine_f(i); }
				const auto Get_mag_frame(int ID_t) const noexcept { return this->obj_mag_.frame(ID_t); }
				const auto Get_mag_frameLocalMatrix(int ID_t) const noexcept { return this->obj_mag_.GetFrameLocalMatrix(ID_t); }
				const auto Get_L_mag_pos(void) const noexcept { return this->Get_mag_frame(this->ptr_mag->mod.Get_LEFT_mag_frame(0).first); }
				const auto isFull()const noexcept { return this->Ammo_cnt == int(this->Get_Cap()); }

				void Set(GUNPARTs* ptr_mag_t, int Ammo_cnt_t) {
					if (ptr_mag_t != nullptr) {
						this->Ammo_cnt = Ammo_cnt_t;
						this->ptr_mag = ptr_mag_t;
						this->obj_mag_ = this->ptr_mag->mod.Get_model().Duplicate();
						this->obj_ammo[0] = this->ptr_mag->Get_Ammo(0).Get_model_full().Duplicate();
						this->obj_ammo[1] = this->ptr_mag->Get_Ammo(0).Get_model_full().Duplicate();
					}
				}
				void SetMat(const MATRIX_ref& mat_t) {
					this->obj_mag_.SetMatrix(mat_t);
					this->obj_ammo[0].SetMatrix(mat_t * MATRIX_ref::Mtrans(this->Get_mag_frame(this->ptr_mag->mod.Get_magazine_ammo_f(0).first) - mat_t.pos()));
					this->obj_ammo[1].SetMatrix(mat_t * MATRIX_ref::Mtrans(this->Get_mag_frame(this->ptr_mag->mod.Get_magazine_ammo_f(1).first) - mat_t.pos()));
				}
				void Draw(void) const noexcept {
					obj_mag_.DrawModel();
					if (Ammo_cnt >= 1) {
						obj_ammo[0].DrawModel();
					}
					if (Ammo_cnt >= 2) {
						obj_ammo[1].DrawModel();
					}
				}
			};
			//所持弾数などの武器データ
			class GUN_STATUS {
			public:
				size_t ID = 0;//武器ID
			private:
				std::vector<MAG_STATUS> magazine_in;				//マガジン内データ
			private:
				size_t Get_magazine_in_not_full(void) noexcept {
					auto result = std::find_if(this->magazine_in.begin() + 1, this->magazine_in.end(), [](MAG_STATUS& m) { return !m.isFull(); });
					return (result != this->magazine_in.end()) ? result - this->magazine_in.begin() : 0;
				}
			public:
				auto& Get_magazine_in(void) noexcept { return magazine_in; }
				//
				GUN_STATUS(void) noexcept {
					Reset();
				}
				GUN_STATUS(const GUN_STATUS&) {
				}
				~GUN_STATUS(void) noexcept {
				}
				//
				void Get_magazine_in_pop_front(void) noexcept { magazine_in.erase(magazine_in.begin()); }
				void Get_magazine_in_pop_back(void) noexcept { magazine_in.pop_back(); }
				//
				//マガジンを複数持っているか
				bool hav_mags(void) const noexcept { return magazine_in.size() > 1; }
				//マガジンを1つ以上持っているか
				bool hav_mag(void) const noexcept { return magazine_in.size() > 0; }
				//リセット
				void Reset(void) noexcept {
					this->magazine_in.clear();
				}
				//マガジンを1つ追加(装填無し)
				void magazine_plus(Items* magazine_item) noexcept {
					this->magazine_in.resize(this->magazine_in.size() + 1);
					this->magazine_in.back().Set(magazine_item->Get_ptr_mag(), (int)magazine_item->Get_Ammo_Cnt());
				}
				//マガジンを1つ追加
				bool magazine_pushback(GUNPARTs* magazine_parts) noexcept {
					this->magazine_in.resize(this->magazine_in.size() + 1);
					this->magazine_in.back().Set(magazine_parts, (int)magazine_parts->Get_Ammo_Cap());

					if (this->magazine_in.size() == 1) {
						this->magazine_in.back().Add_Cnt(-1);
						return true;
					}
					return false;
				}
				//撃った時の弾数管理
				bool magazine_shot(const bool reload_f) noexcept {
					if (!reload_f) {
						//マガジンを手に持っているなら
						if (this->hav_mag()) {
							//チャンバーに1発残っているなら
							if (this->magazine_in.front().Get_Cnt() > 0) {
								//マガジンからチャンバーのカウントを抜く
								this->magazine_in.front().Add_Cnt(-1);
							}
							else {
								return true;
							}
						}
						else {
							return true;
						}
					}
					return false;
				}
				//チャンバーに装填
				void chamber_in(void) noexcept {
					this->magazine_in.front().Add_Cnt(-1);
				}
				//マグ順ソート
				void sort_magazine(void) noexcept {
					std::sort(this->magazine_in.begin() + 1, this->magazine_in.end(), [](MAG_STATUS& a, MAG_STATUS& b) { return a.Get_Cnt() > b.Get_Cnt(); });
				}
				//弾抜き、装填
				void load_magazine(void) noexcept {
					size_t siz = this->Get_magazine_in_not_full();
					auto be_ = (int)std::clamp<size_t>(this->magazine_in.back().Get_Cnt(), 0, magazine_in[siz].Get_Cap() - this->magazine_in[siz].Get_Cnt());
					this->magazine_in.back().Add_Cnt(-be_);
					this->magazine_in[siz].Add_Cnt(be_);
				}
				//
			};
		private:
			//薬莢
			class CARTS {
				MV1 obj;
				bool Flag{ false };
				float cnt{ -1.f };
				bool se_use{ false };
				float rate{ 0.f };
				bool stay{ false };
				moves move;

			public:
				auto& Get_Flag(void) const noexcept { return Flag; }
				auto& Get_pos(void) noexcept { return move.pos; }

				void Set(void) noexcept {
					this->Dispose();
				}
				void Put_first(const MV1& model, const VECTOR_ref& pos_t, const MATRIX_ref& mat_t) noexcept {
					this->Dispose();
					this->Flag = true;
					this->se_use = false;
					this->obj = model.Duplicate();
					this->move.SetPos(pos_t);//排莢
					this->move.mat = mat_t;
					this->stay = true;
				}

				void Put_second(const VECTOR_ref& pos_t, const MATRIX_ref& mat_t) noexcept {
					if (this->Flag) {
						if (this->stay) {
							this->move.SetPos(pos_t);//排莢
							this->move.mat = mat_t;
						}
					}
				}
				void Put_stay(const VECTOR_ref& vec_t, const float rate_t = 1) noexcept {
					if (this->Flag) {
						if (this->stay) {
							this->cnt = 1.5f;
							this->rate = rate_t;
							this->move.vec = vec_t * this->rate;//排莢ベクトル
							this->stay = false;
						}
					}
				}

				void Update_pos(void) noexcept {
					if (this->Flag) {
						if (!this->stay) {
							this->cnt -= this->rate / FPS;
							this->move.Update_Physics(0.5f, this->rate);
						}
					}
				}
				void Update_fall(const MV1_COLL_RESULT_POLY& mapcol) noexcept {
					if (this->Flag) {
						if (this->stay) {
						}
						else {
							if (mapcol.HitFlag == TRUE) {
								VECTOR_ref map_nomal = mapcol.Normal;
								this->move.SetPos(VECTOR_ref(mapcol.HitPosition) + (VECTOR_ref::up() * 0.008f));
								this->move.vec += map_nomal * (map_nomal.dot(this->move.vec * -1.f) * 1.25f);
								easing_set(&this->move.vec, VECTOR_ref::zero(), 0.95f);
								if (!this->se_use) {
									SE.Get(EnumSound::Cate_Down).Play_3D(0, this->move.pos, 2.f);
									this->se_use = true;
								}
								this->move.mat *= MATRIX_ref::RotVec2(this->move.mat.yvec(), map_nomal);
							}
							else {
								this->move.mat *= MATRIX_ref::RotY(deg2rad(360 * 3 + GetRand(360 * 3 * 2)) * this->rate / FPS);
							}
						}
						this->obj.SetMatrix(this->move.MatIn());
						if (this->cnt <= 0.f) {
							this->Flag = false;
						}
					}
				}
				void Draw(void) const noexcept {
					if (this->Flag) {
						this->obj.DrawModel();
					}
				}
				/*おわり*/
				void Dispose(void) noexcept {
					this->Flag = false;
					this->obj.Dispose();
				}
			};
			//銃に着けるパーツ
			class GParts_COMMON {
				class PointFrame {
				public:
					frames first;
					frames second;
				};
			protected:
				//共通
				bool attach{ false };										//取り付いているか
				EnumGunParts type{ EnumGunParts::NONE };				//パーツの種類
				VECTOR_ref vec_parts;										//根元からのベクトル
				std::array<PointFrame, int(EnumAttachPoint::NUM)> rail_frame;	//アタッチできるポイント
				frames rad_parts_frame;										//傾き
				frames attach_frame;										//根元
				MV1 obj;													//モデル
			public:
				GUNPARTs* thisparts{ nullptr };	//自身のデータ
				GParts_COMMON* parent{ nullptr };		//親パーツ
			public:
				const auto& IsActive(void) const noexcept { return this->attach; }
				const auto& Get_type(void) const noexcept { return this->type; }
				const auto& Get_rail_frame(EnumAttachPoint sel_t) const noexcept { return this->rail_frame[(int)sel_t].first; }
				const auto& Get_attach_frame(void) const noexcept { return this->attach_frame.second; }
				const auto Get_rail_pos(EnumAttachPoint sel_t) const noexcept { return (IsActive()) ? this->obj.frame(this->rail_frame[(int)sel_t].first.first) : VECTOR_ref::zero(); }
				//フレームリセット
				virtual void ReSet_frame(void) noexcept {
					for (auto& r : this->rail_frame) {
						r.first.first = -1;
						r.first.first = -1;
					}
					this->rad_parts_frame.first = -1;
					this->attach_frame.first = -1;
				}
				virtual void Get_frame_(GUNPARTs& parts, EnumGunParts type_t) noexcept {
					if (IsActive()) {
						this->ReSet_frame();
						this->thisparts = &parts;
						this->type = type_t;
						SetUseASyncLoadFlag(FALSE);
						if (this->type == EnumGunParts::BASE) {
							this->thisparts->mod.Get_model().DuplicateonAnime(&this->obj, &this->obj);
						}
						else {
							this->obj = this->thisparts->mod.Get_model().Duplicate();
						}
						for (int i = 0; i < this->obj.frame_num(); ++i) {
							std::string p = this->obj.frame_name(i);
							if (p == "rail") {
								this->rail_frame[(int)EnumAttachPoint::UPER_RAIL].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::UPER_RAIL].second.Set_Local(i + 1, this->obj);
							}
							else if (p == "underrail") {
								this->rail_frame[(int)EnumAttachPoint::UNDER_RAIL].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::UNDER_RAIL].second.Set_Local(i + 1, this->obj);
							}
							else if (p == "siderail") {
								this->rail_frame[(int)EnumAttachPoint::LEFTSIDE_RAIL].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::LEFTSIDE_RAIL].second.Set_Local(i + 1, this->obj);
							}
							else if (p == "rightrail") {
								this->rail_frame[(int)EnumAttachPoint::RIGHTSIDE_RAIL].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::RIGHTSIDE_RAIL].second.Set_Local(i + 1, this->obj);
							}
							else if (p == "sidemount") {
								this->rail_frame[(int)EnumAttachPoint::SIDEMOUNT].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::SIDEMOUNT].second.Set_Local(i + 1, this->obj);
							}
							else if (p == "sidemount_base") {
								this->rail_frame[(int)EnumAttachPoint::SIDEMOUNT_BASE].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::SIDEMOUNT_BASE].second.Set_Local(i + 1, this->obj);
							}
							else if (p == "stock") {
								this->rail_frame[(int)EnumAttachPoint::STOCK_BASE].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::STOCK_BASE].second.Set_Local(i + 1, this->obj);
							}
							else if (p == "dustcover") {
								this->rail_frame[(int)EnumAttachPoint::DUSTCOVER_BASE].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::DUSTCOVER_BASE].second.Set_Local(i + 1, this->obj);
							}
							else if (p == "uper_handguard") {
								this->rail_frame[(int)EnumAttachPoint::UPER_HANDGUARD].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::UPER_HANDGUARD].second.Set_Local(i + 1, this->obj);
							}
							else if (p == "under_handguard") {
								this->rail_frame[(int)EnumAttachPoint::UNDER_HANDGUARD].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::UNDER_HANDGUARD].second.Set_Local(i + 1, this->obj);
							}
							else if (p == "mazzule") {
								this->rail_frame[(int)EnumAttachPoint::MAZZULE_BASE].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::MAZZULE_BASE].second.Set_Local(i + 1, this->obj);
							}
							else if (p == "grip") {
								this->rail_frame[(int)EnumAttachPoint::GRIP_BASE].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::GRIP_BASE].second.Set_Local(i + 1, this->obj);
							}
							else if (p == "rad_parts") {
								this->rad_parts_frame.Set_World(i, this->obj);
							}
							else if (p == "magazine_fall") {
								this->rail_frame[(int)EnumAttachPoint::MAGAZINE_BASE].first.Set_World(i, this->obj);
								this->rail_frame[(int)EnumAttachPoint::MAGAZINE_BASE].second.Set_Local(i + 1, this->obj);
							}
						}
					}
				}
				//描画
				virtual void Draw(void) const noexcept {
					if (IsActive()) {
						this->obj.DrawModel();
					}
				}
				//パーツを外す
				virtual void Detach(Performance& per_all) noexcept {
					if (IsActive()) {
						this->attach = false;
						this->ReSet_frame();
						per_all -= this->thisparts->Get_Performance();
						this->thisparts = nullptr;
						this->parent = nullptr;
						this->type = EnumGunParts::NONE;
						this->obj.Dispose();
					}
				}
				//ベース用座標指定
				virtual void Setpos_Nomal(const MATRIX_ref& mat_t) noexcept {
					if (IsActive()) {
						this->obj.SetMatrix(mat_t);
					}
				}
				//パーツ用の座標指定
				virtual void Setpos_parts(const MATRIX_ref& mat_t) noexcept {
					if (IsActive()) {
						this->obj.SetMatrix((MATRIX_ref::RotVec2(this->rad_parts_frame.second.Norm(), this->vec_parts) * mat_t) * MATRIX_ref::Mtrans(this->parent->obj.frame(this->attach_frame.first)));
					}
				}
				//アニメ更新
				virtual void Update_Anim(void) { this->obj.work_anime(); }
				//便利系
				virtual const MATRIX_ref Get_objMatrix(void) const noexcept { return this->obj.GetMatrix(); }
				virtual const MATRIX_ref Get_objFrameLocalMatrix(size_t sel_t) const noexcept { return obj.GetFrameLocalMatrix(int(sel_t)); }
				virtual MV1::ani* Get_Anim(size_t sel_t) noexcept { return &this->obj.get_anime(sel_t); }
			protected:
				//親を設定する
				void Set_parent(GParts_COMMON* parents, EnumAttachPoint side) {
					this->parent = parents;
					this->attach_frame = parents->rail_frame[(int)side].first;
					this->vec_parts = parents->rail_frame[(int)side].second.second.Norm();
				}
				//親の座標を探す
				void search_parts(EnumGunParts PARTS_, VECTOR_ref* pv, const GParts_COMMON* ptr) noexcept {
					if (ptr->parent->Get_type() != PARTS_ && ptr->parent != nullptr) {
						*pv += ptr->parent->Get_attach_frame();
						search_parts(EnumGunParts::BASE, pv, ptr->parent);
					}
				}
			};
		public:
			//銃に着けるパーツ詳細版
			class g_parts :public GParts_COMMON {
			private:
				/*汎用的専用*/
				frames mag_frame;						//マガジン
				frames Cart_frame;						//排莢
				frames slide_frame;						//スライド
				std::array<frames, 3> RIGHT_frame;		//右手座標
				std::array<frames, 3> LEFT_frame;		//左手座標
				std::array<frames, 3> LEFT_mag_frame;	//左手座標(マガジン保持時)
				std::array<frames, 3> LEFT_mag_frame_ready;	//左手座標(マガジン装着用意時)
				/*LAM専用*/
				frames Lightsource_frame;				//光源
				LightHandle Light;						//ライト
				/*サイト専用*/
				VECTOR_ref sight_vec;					//サイトが見ている場所
				frames sight_frame;						//サイト
				GraphHandle reticle;					//レティクル
				/*マガジン専用*/
				std::array<MV1, 2> obj_ammo;			//マガジン用弾
			public:
				/*getter*/
				//frame
				const auto& Get_slide_frame(void) const noexcept { return this->slide_frame; }
				const auto& Get_sight_frame(void) const noexcept { return this->sight_frame.second; }
				//type
				const auto Get_mazzuletype(void) const noexcept { return (IsActive()) ? int(this->thisparts->Get_mazzuletype()) : -1; }
				const auto Get_stocktype(void) const noexcept { return (IsActive()) ? int(this->thisparts->Get_stocktype()) : -1; }
				const auto Get_lamtype(size_t sel_t) const noexcept { return (IsActive()) ? this->thisparts->Get_lamtype(sel_t) : EnumSELECT_LAM::NUM; }
				//obj.frame
				const auto Get_mazzule_pos(void) const noexcept { return (IsActive()) ? this->obj.frame(this->Get_rail_frame(EnumAttachPoint::MAZZULE_BASE).first) : VECTOR_ref::zero(); }
				const auto Get_magf_pos(void) const noexcept { return (IsActive()) ? this->obj.frame(this->mag_frame.first) : VECTOR_ref::zero(); }
				const auto Get_mag2f_pos(void) const noexcept { return (IsActive()) ? this->obj.frame(this->mag_frame.first + 1) : VECTOR_ref::zero(); }
				const auto Get_source_pos(void) const noexcept { return (IsActive()) ? this->obj.frame(this->Lightsource_frame.first) : VECTOR_ref::zero(); }
				const auto Get_RIGHT_pos(size_t sel_t) const noexcept { return (IsActive()) ? this->obj.frame(this->RIGHT_frame[sel_t].first) : VECTOR_ref::zero(); }
				const auto Get_LEFT_pos(size_t sel_t) const noexcept { return (IsActive()) ? this->obj.frame(this->LEFT_frame[sel_t].first) : VECTOR_ref::zero(); }
				const auto Get_L_mag_pos(size_t sel_t) const noexcept { return (IsActive()) ? this->obj.frame(this->LEFT_mag_frame[sel_t].first) : VECTOR_ref::zero(); }
				const auto Get_L_mag_pos_ready(size_t sel_t) const noexcept { return (IsActive()) ? this->obj.frame(this->LEFT_mag_frame_ready[sel_t].first) : VECTOR_ref::zero(); }

				const auto Get_Cart_pos(void) const noexcept { return (IsActive()) ? this->obj.frame(this->Cart_frame.first) : VECTOR_ref::zero(); }
				const auto Get_Cart_vec(void) const noexcept { return (this->obj.frame(this->Cart_frame.first + 1) - this->Get_Cart_pos()).Norm(); }
				//
				const auto Get_sightvec(void) const noexcept { return (IsActive() && this->type == EnumGunParts::SIGHT) ? this->sight_vec : VECTOR_ref::zero(); }
				const auto Get_reticle_size(void) const noexcept { return (IsActive() && this->type == EnumGunParts::SIGHT) ? float(y_r(this->thisparts->Get_zoom_size() / 2.f)) : 1.f; }
				/*コンストラクタ*/
				g_parts(void) noexcept {
					this->Light.Dispose();
				}

				void Get_frame_(GUNPARTs& parts, EnumGunParts type_t) noexcept override {
					GParts_COMMON::Get_frame_(parts, type_t);
					if (IsActive()) {
						this->mag_frame.first = -1;
						this->sight_frame.first = -1;
						this->Cart_frame.first = -1;
						//this->sight_frame.second.clear();
						if (this->type == EnumGunParts::MAGAZINE) {
							this->obj_ammo[0] = this->thisparts->Get_Ammo(0).Get_model_full().Duplicate();
							this->obj_ammo[1] = this->thisparts->Get_Ammo(0).Get_model_full().Duplicate();
						}
						for (int i = 0; i < this->obj.frame_num(); ++i) {
							std::string p = this->obj.frame_name(i);
							if (p == "sight" || p == "site") {
								this->sight_frame.Set_World(i, this->obj);
							}
							else if (p == "LEFT") {
								this->LEFT_frame[0].Set_World(i, this->obj);
								this->LEFT_frame[1].Set_Local(i + 1, this->obj);
								this->LEFT_frame[2].Set_Local(i + 2, this->obj);
							}
							else if (p == "LEFT_mag") {
								this->LEFT_mag_frame[0] = this->thisparts->mod.Get_LEFT_mag_frame(0);
								this->LEFT_mag_frame[1] = this->thisparts->mod.Get_LEFT_mag_frame(1);
								this->LEFT_mag_frame[2] = this->thisparts->mod.Get_LEFT_mag_frame(2);
							}
							else if (p == "LEFT_mag_ready") {
								this->LEFT_mag_frame_ready[0] = this->thisparts->mod.Get_LEFT_mag_frame_ready(0);
								this->LEFT_mag_frame_ready[1] = this->thisparts->mod.Get_LEFT_mag_frame_ready(1);
								this->LEFT_mag_frame_ready[2] = this->thisparts->mod.Get_LEFT_mag_frame_ready(2);
							}
							else if (p == "RIGHT") {
								this->RIGHT_frame[0].Set_World(i, this->obj);
								this->RIGHT_frame[1].Set_Local(i + 1, this->obj);
								this->RIGHT_frame[2].Set_Local(i + 2, this->obj);
							}
							else if (p == "source") {
								this->Lightsource_frame.Set_World(i, this->obj);
							}
							else if (p == "mag_fall") {
								this->mag_frame.Set_World(i, this->obj);//マガジン
							}
							else if (p == "cate") {
								this->Cart_frame.Set_World(i, this->obj);//排莢
							}
							else if (p == "slide") {
								this->slide_frame.Set_World(i, this->obj);//
							}
						}
						switch (this->type) {
						case EnumGunParts::SIGHT:
						{
							this->reticle = this->thisparts->Get_reticle().Duplicate();
							this->obj.SetupCollInfo(1, 1, 1, -1, 1);//
							break;
						}
						case EnumGunParts::LAM:
						{
							this->Light.Dispose();
							break;
						}
						default:
							break;
						}
					}
				}
				const auto Draw_reticle_UI(bool ads) noexcept {
					if (IsActive() && ads) {
						this->obj.RefreshCollInfo(-1, 1);
						VECTOR_ref sightpos = this->obj.frame(3);
						if (
							this->obj.CollCheck_Line(GetCameraPosition(), sightpos, -1, 1).HitFlag == TRUE
							) {
							VECTOR_ref tmp = ConvWorldPosToScreenPos(sightpos.get());
							if (tmp.z() >= 0.f && tmp.z() <= 1.f) {
								const auto siz = int(this->thisparts->Get_reticle_size() / 2.f);
								//DrawLine(int(tmp.x()) - siz, int(tmp.y()) - siz, int(tmp.x()) + siz, int(tmp.y()) + siz, GetColor(255, 0, 0), 3);
								this->reticle.DrawExtendGraph(int(tmp.x()) - siz, int(tmp.y()) - siz, int(tmp.x()) + siz, int(tmp.y()) + siz, true);
								return this->thisparts->Get_zoom();
							}
						}
					}
					return 1.f;
				}
				//パーツを付ける
				void attach_mini(Performance& per_all, GUNPARTs& partsptr, EnumGunParts type_t) noexcept {
					this->attach = true;
					this->Get_frame_(partsptr, type_t);
					per_all += this->thisparts->Get_Performance();
				}
				void attach_(Performance& per_all, GUNPARTs& partsptr, EnumGunParts type_t, Performance& per_minimam) noexcept {
					this->Detach(per_all);
					per_minimam.recoil = per_all.recoil;
					per_minimam.weight = per_all.weight;
					attach_mini(per_all, partsptr, type_t);
					per_minimam.recoil = per_all.recoil - per_minimam.recoil;
					per_minimam.weight = per_all.weight - per_minimam.weight;
				}
				void attach_(Performance& per_all, GUNPARTs& partsptr, EnumGunParts type_t) noexcept {
					this->Detach(per_all);
					attach_mini(per_all, partsptr, type_t);
				}
				void attach_(Performance& per_all, GUNPARTs& partsptr, EnumGunParts type_t, GParts_COMMON* parents, EnumAttachPoint side) noexcept {
					this->Detach(per_all);
					this->attach = false;
					if (parents != nullptr) {
						if (parents->thisparts != nullptr) {
							for (auto& p : parents->thisparts->Get_can_attach()) {
								if ((p == partsptr.mod.Get_name()) && (parents->Get_rail_frame(side).first > 0)) {
									attach_mini(per_all, partsptr, type_t);
									Set_parent(parents, side);
									break;
								}
							}
						}
					}
				}
				//パーツを外す
				void Detach(Performance& per_all) noexcept override {
					if (IsActive()) {
						GParts_COMMON::Detach(per_all);
						this->reticle.Dispose();
						this->Light.Dispose();
					}
				}
				//ライトの設定
				void Set_LightHandle(const VECTOR_ref& Vec) noexcept {
					switch (this->Get_lamtype(0)) {
					case EnumSELECT_LAM::LIGHT:
					{
						auto StartPos = this->Get_source_pos();
						if (this->Light.get() == -1) {
							this->Light = LightHandle::Create(StartPos, Vec, DX_PI_F / 16.0f, DX_PI_F / 24.0f, 25.0f, 0.1f, 0.35f, 0.0f);
						}
						this->Light.SetPos(StartPos, Vec);
						break;
					}
					default:
						this->Light.Dispose();
						break;
					}
				}
				//サイトの親の座標を探す
				void Set_sight_vec(void) noexcept {
					if (IsActive()) {
						this->sight_vec = this->Get_attach_frame() + this->Get_sight_frame();
						search_parts(EnumGunParts::BASE, &this->sight_vec, this);
					}
				}
				//サイトを乗せるかいなか
				bool Set_sight_at(const std::vector<GUNPARTs>& parts_data) noexcept {
					bool trm = false;
					if (IsActive()) {
						for (auto& can : this->thisparts->Get_can_attach()) {
							for (auto& tgt : parts_data) {
								if (can == tgt.mod.Get_name()) { trm = true; }
								if (trm) { break; }
							}
							if (trm) { break; }
						}
					}
					return trm;
				}
			};
		private:
			MV1::ani* gunanime_shot{ nullptr };					//銃anime
			MV1::ani* gunanime_first{ nullptr };				//銃anime
			MV1::ani* gunanime_shot_last{ nullptr };			//銃anime
			MV1::ani* gunanime_trigger{ nullptr };				//銃anime
			MV1::ani* gunanime_magcatch{ nullptr };				//銃anime
			std::vector<MV1::ani*>gunanime_sel{ nullptr };		//銃anime
			MV1::ani* gunanime_Changesel{ nullptr };			//銃anime
			std::array<CARTS, max_bullet> Cart;					/*確保する弾*/
			size_t select_sight{ 0 };							//使用するサイト
			VECTOR_ref blur_vec, blur_vec_res;					//
			VECTOR_ref recoil_vec, recoil_vec_res;				//
			size_t use_Cart{ 0 };								/*確保する弾*/
			g_parts base;										/*モデル*/
			g_parts magazine;									/*モデル*/
			g_parts dustcover;									/*モデル*/
			g_parts stock;										/*モデル*/
			g_parts foregrip;									/*モデル*/
			g_parts lam;										/*モデル*/
			g_parts mazzule;									/*モデル*/
			g_parts grip;										/*モデル*/
			g_parts uperhandguard;								/*モデル*/
			g_parts underhandguard;								/*モデル*/
			g_parts mount_base;									/*モデル*/
			g_parts mount_;										/*モデル*/
			std::vector<g_parts> sight_;						/*モデル*/
			moves mag_m;										//マガジン座標系
			Performance per_all;								//性能
			Performance per_minimam;							//性能
			switchs trigger;									//トリガー
			bool in_chamber{ false };							//チャンバー内
			EnumSELECTER selecter{ EnumSELECTER::SEMI };		//セレクター
			GUNPARTs* parts_ptr{ nullptr };						//保存用
			std::vector<PresetSaveControl::save_c> save_parts;	//保存用
			GUN_STATUS* gun_stat_now{ nullptr };				/*所持弾数などの武器データ*/
		protected:
			bool slide_sound_f = false;							//
			moves gun_m;										//銃座標
		public:
			bool Flag_gun{ false };								//射撃フラグ
			Audios_Gun Audio;									/*音声*/
			float distance_to_cam{ -1.f };						//カメラとの距離
			bool CanUse_Physics = false;
			bool IsDrawmagazine = false;
		public:
			void Set_gun_stat_now(GUN_STATUS*  in_c) noexcept { gun_stat_now = in_c; }				//チャンバーに弾があるか否か
			const auto Get_gun_stat_now(void) const noexcept {
				if (gun_stat_now == nullptr) {
					int a = 0;
				}
				return gun_stat_now;
			}
			//setter
			g_parts* Get_parts(EnumGunParts type_sel, int mount_t = 0) noexcept {
				switch (type_sel) {
				case EnumGunParts::BASE:
					return &this->base;
				case EnumGunParts::MAGAZINE:
					return &this->magazine;
				case EnumGunParts::GRIP:
					return &this->grip;
				case EnumGunParts::UPER_HANDGUARD:
					return &this->uperhandguard;
				case EnumGunParts::UNDER_HANDGUARD:
					return &this->underhandguard;
				case EnumGunParts::MAZZULE:
					return &this->mazzule;
				case EnumGunParts::DUSTCOVER:
					return &this->dustcover;
				case EnumGunParts::STOCK:
					return &this->stock;
				case EnumGunParts::LAM:
					return &this->lam;
				case EnumGunParts::FOREGRIP:
					return &this->foregrip;
				case EnumGunParts::MOUNT_BASE:
					return &this->mount_base;
				case EnumGunParts::MOUNT:
					return &this->mount_;
				case EnumGunParts::SIGHT:
					return &this->sight_[mount_t];
				default:
					return nullptr;
				}
			}
			auto& Set_mag_in(void) noexcept { return this->gun_stat_now->Get_magazine_in(); }																//刺さっているマガジンのMV1
			auto& Set_ammoSpec(void) noexcept { return this->base.thisparts->Set_Ammo(0); }
			void Set_gunanime_trigger(float per) noexcept { easing_set(&this->gunanime_trigger->per, per, 0.5f); }													//
			void Set_gunanime_magcatch(float per) noexcept { easing_set(&this->gunanime_magcatch->per, per, 0.5f); }													//
			void Set_gunanime_shot(float per) noexcept { this->gunanime_shot->per = per; }																			//
			void ReSet_gunanime_Changesel(void) const noexcept { gunanime_Changesel->Reset(); }																//
			void Set_move_gun(const moves& move_t) noexcept { this->gun_m = move_t; }																		//銃座標指定
			void Set_selecter(EnumSELECTER sel) noexcept { selecter = sel; }
			void Set_in_chamber(bool in_c) noexcept { in_chamber = in_c; }				//チャンバーに弾があるか否か
			//getter
			const auto& Get_gunanime_trigger(void) const noexcept { return gunanime_trigger; }																//トリガーアニメーション取り出し(メニューのみ)
			const auto& Get_gunanime_Changesel(void) const noexcept { return gunanime_Changesel; }															//
			const auto& Get_gunanime_first(void) const noexcept { return gunanime_first; }																	//
			const auto& Get_move_gun(void) const noexcept { return gun_m; }																					//
			const auto& Get_move_mag(void) const noexcept { return mag_m; }																					//
			const auto& Get_mag_in(void) const noexcept { return this->gun_stat_now->Get_magazine_in(); }													//刺さっているマガジンのMV1
			const auto& Get_now_selector(void) const noexcept { return this->base.thisparts->Get_select((int)this->selecter); }								//
			const auto& Get_per_all(void) const noexcept { return per_all; }																				//性能(UI表示用)
			const auto& Get_ammoSpec(void) const noexcept { return this->base.thisparts->Get_Ammo(0); }
			const auto& Get_in_chamber(void) const noexcept { return in_chamber; }																			//チャンバーに弾があるか否か
			const auto Get_magf_pos(void) const noexcept { return this->base.Get_magf_pos(); }
			const auto Get_mag2f_pos(void) const noexcept { return this->base.Get_mag2f_pos(); }
			const auto Get_mazzuletype(void) const noexcept { return mazzule.Get_mazzuletype(); }															//
			const auto Get_reloadtime(void) const noexcept { return this->base.thisparts->Get_reloadtime(); }
			const auto Get_Per_Sub(float Base, float Direction) const noexcept { return Base + (Direction - Base) * (this->per_all.weight - this->per_minimam.weight) / 3.f; }
			const auto Get_sightVec(void) const noexcept {
				auto vec = (select_sight != SIZE_MAX) ? this->sight_[select_sight].Get_sightvec() : this->base.Get_sight_frame();
				vec.z(0);
				return vec * -1.f;
			}//サイト
			const auto Get_lamtype(void) const noexcept { return this->lam.Get_lamtype(0); }
			const auto Get_source_pos(void) const noexcept { return this->lam.Get_source_pos(); }
			const auto Get_maz(void) const noexcept { return (Get_mazzuletype() > 0) ? mazzule.Get_mazzule_pos() : base.Get_mazzule_pos(); }				//
			const auto Get_reticle_size(void) const noexcept { return (select_sight != SIZE_MAX) ? this->sight_[select_sight].Get_reticle_size() : 1.f; }	//
			const auto Get_res_blur(float persent) const noexcept {
				auto v_x = this->blur_vec;
				auto v_z = this->recoil_vec;
				if (persent != 1.f) {
					v_x.y(v_x.y() * persent);
					v_x = v_x.Norm();
					v_z.y(v_z.y() * persent);
					v_z = v_z.Norm();
				}
				return MATRIX_ref::RotVec2(VECTOR_ref::front(), v_x) * MATRIX_ref::RotVec2(VECTOR_ref::front(), v_z);
			}
			const auto Get_sight_size(void) const noexcept { return this->sight_.size(); }
		public:
			void Draw_LAM_Effect(GraphHandle& light_pic) noexcept {
				switch (Get_lamtype()) {
				case EnumSELECT_LAM::LASER:
					//Draw_Lazer_Effect(Get_source_pos(), gun_m.mat.zvec() * -1.f, true, 0.1f);
					break;
				case EnumSELECT_LAM::LIGHT:
					DrawBillboard3D(Get_source_pos().get(), 0.5f, 0.5f, 0.1f, 0.f, light_pic.get(), TRUE);
					break;
				default:
					break;
				}
			}
			//弾の生成
			void Create_bullet(void) noexcept {
				BulletControl_Common::Create_bullet(&Set_ammoSpec(), Get_maz(), gun_m.mat.zvec() * -1.f);
			}
		public:
			//右腕座標(銃持ち)
			const auto RIGHT_pos_gun(size_t sel_t) const noexcept {
				return this->base.Get_RIGHT_pos(sel_t);
			}
			//左腕座標(銃添え)
			const auto LEFT_pos_gun(size_t sel_t) const noexcept {
				auto ans = this->underhandguard.Get_LEFT_pos(sel_t);
				auto f_ = this->foregrip.Get_LEFT_pos(sel_t);
				if (f_ != VECTOR_ref::zero()) { ans = f_; }
				return ans;
			}
			//左腕座標(マガジン)
			const auto LEFT_pos_mag(size_t sel_t) const noexcept { return base.Get_L_mag_pos(sel_t); }
			const auto LEFT_pos_mag_ready(size_t sel_t) const noexcept { return base.Get_L_mag_pos_ready(sel_t); }
			//銃座標指定
			void Set_gun_posmat(const VECTOR_ref& pos_t, const MATRIX_ref& mat_t) noexcept {
				this->gun_m.SetPos(pos_t);
				this->gun_m.mat = mat_t;
			}
			//マグ座標決定
			void Set_Magazine(const VECTOR_ref& pos_t, const MATRIX_ref& mat_t) noexcept {
				this->mag_m.SetPos(pos_t);
				this->mag_m.mat = mat_t;
				if (this->gun_stat_now != nullptr && this->gun_stat_now->hav_mag()) {
					Set_mag_in().front().SetMat(mag_m.MatIn());
				}
			}
			//刺さっている時のマガジン座標
			void Set_Magazine(void) noexcept {
				Set_Magazine(Get_mag2f_pos(), gun_m.mat);
			}
			//
			void Set_selecter(bool key) {
				if (key && this->gunanime_Changesel->per == 0.0f) {
					this->gunanime_Changesel->per = 1.0f;
					size_t tmp = (size_t)this->selecter;
					++tmp %= this->base.thisparts->Get_selectsize();
					this->Set_selecter((EnumSELECTER)tmp);
				}
				if (this->gunanime_Changesel->per == 1.f) {
					this->gunanime_Changesel->Update(false, 0.5f);
					if (this->gunanime_Changesel->time >= this->gunanime_Changesel->alltime) {
						this->gunanime_Changesel->Reset();
					}
				}
			}
			//LAM
			void Set_LightHandle(void) noexcept {
				this->lam.Set_LightHandle(gun_m.mat.zvec() * -1.f);
			}
			//
			void Init_Gun() {
				this->Flag_gun = false;
				this->blur_vec = VECTOR_ref::front();
				this->blur_vec_res = this->blur_vec;
				this->recoil_vec = VECTOR_ref::front();
				this->recoil_vec_res = this->recoil_vec;
				this->per_all.recoil = 0.f;
				this->per_all.weight = 0.f;
			}
			void Set_GunBase(GUNPARTs* parts_ptr_t, GUN_STATUS& gun_stat_t) {
				//base
				this->parts_ptr = parts_ptr_t;
				Attach_parts(parts_ptr, EnumGunParts::BASE);
				//baseanime
				this->gunanime_first = this->base.Get_Anim(0);
				this->gunanime_shot = this->base.Get_Anim(1);
				this->gunanime_shot_last = this->base.Get_Anim(2);
				this->gunanime_trigger = this->base.Get_Anim(3);
				this->gunanime_magcatch = this->base.Get_Anim(4);
				this->gunanime_sel.clear();
				this->gunanime_sel.resize(this->base.thisparts->Get_selectsize());
				this->gunanime_sel[0] = this->base.Get_Anim(5);
				this->gunanime_sel[1] = this->base.Get_Anim(6);
				this->gunanime_Changesel = this->base.Get_Anim(7);
				this->gun_stat_now = &gun_stat_t;
				gun_stat_t.ID = this->base.thisparts->id_t;	//todo:シリアルナンバー割り振り

				Audio.Duplicate(this->base.thisparts->Get_Audio());
			}
		private:
			/*おわり*/
			void Dispose_Gun_Buf(void) noexcept {
				//パーツリセット
				Detach_parts(EnumGunParts::BASE);
				Detach_parts(EnumGunParts::MAGAZINE);
				Detach_parts(EnumGunParts::DUSTCOVER);
				Detach_parts(EnumGunParts::MAZZULE);
				Detach_parts(EnumGunParts::GRIP);
				Detach_parts(EnumGunParts::STOCK);
				Detach_parts(EnumGunParts::MOUNT_BASE);
				Detach_parts(EnumGunParts::MOUNT);

				Detach_parts(EnumGunParts::UPER_HANDGUARD);
				Detach_parts(EnumGunParts::UNDER_HANDGUARD);
				Detach_parts(EnumGunParts::LAM);
				Detach_parts(EnumGunParts::FOREGRIP);

				Detach_parts(EnumGunParts::SIGHT);
				Detach_parts(EnumGunParts::SIGHT);
				Detach_parts(EnumGunParts::SIGHT);

				for (auto& a : this->Cart) { a.Dispose(); }
				this->Audio.Dispose();
			}
		public:
			/*銃データのセット*/
			void Set_Gun(std::vector<GUNPARTs>& gun_data, size_t itr, GUN_STATUS& gun_stat_t) {
				Init_Gun();
				Set_GunBase(&gun_data[itr], gun_stat_t);					//base
			}
			//Set_Gunを1回やっている前提でコピー
			void Copy_Gun(const GunControl& tgt, const std::shared_ptr<GunPartsControl>& GunPartses, GUN_STATUS& gun_stat_t) {
				Dispose_Gun_Buf();
				Init_Gun();
				Set_GunBase(tgt.parts_ptr, gun_stat_t);
				for (auto& a : this->Cart) { a.Set(); }						//薬莢
				Load_Gun(tgt.save_parts, GunPartses);
			}
			//セーブデータからパーツを組み込み
			void Load_Gun(const std::vector<PresetSaveControl::save_c>& save_parts_t, const std::shared_ptr<GunPartsControl>& GunPartses) {
				int pppp = 0;
				for (const auto& tmp_save : save_parts_t) {
					GUNPARTs* temp_p = nullptr;
					g_parts* temp_ptr = nullptr;
					if (tmp_save.cang_ != SIZE_MAX) {
						auto& vec_data = GunPartses->Get_Parts_Data(tmp_save.type_);
						if (&vec_data != nullptr) {
							temp_p = &vec_data[tmp_save.cang_];
						}
					}
					{
						switch (tmp_save.pt_type_) {
						case EnumGunParts::BASE:
						case EnumGunParts::UPER_HANDGUARD:
						case EnumGunParts::UNDER_HANDGUARD:
						case EnumGunParts::DUSTCOVER:
						case EnumGunParts::MOUNT_BASE:
						case EnumGunParts::MOUNT:
							temp_ptr = this->Get_parts(tmp_save.pt_type_);
							break;
						default:
							break;
						}
					}
					if (tmp_save.type_ == EnumGunParts::SIGHT) { ++pppp; }
					this->Attach_parts(temp_p, tmp_save.type_, temp_ptr, tmp_save.pt_cat_, pppp);
				}
				save_parts = save_parts_t;
			}
		public:
			//薬莢の生成
			void Create_Cart(void) noexcept {
				this->Cart[use_Cart].Put_first(this->Get_ammoSpec().Get_model(), this->base.Get_Cart_pos(), gun_m.mat);
				++this->use_Cart %= this->Cart.size();
			}
		private:
				//自分についているパーツがある場合削除
			void Detach_child(GParts_COMMON* parent_ptr, EnumGunParts type_t, int sight_s = 0) noexcept {
				if (parent_ptr == nullptr) { return; }
				if (!parent_ptr->IsActive()) { return; }
				if (this->mazzule.parent == parent_ptr) { Detach_child(&this->mazzule, EnumGunParts::MAZZULE); }
				if (this->grip.parent == parent_ptr) { Detach_child(&this->grip, EnumGunParts::GRIP); }
				if (this->uperhandguard.parent == parent_ptr) { Detach_child(&this->uperhandguard, EnumGunParts::UPER_HANDGUARD); }
				if (this->underhandguard.parent == parent_ptr) { Detach_child(&this->underhandguard, EnumGunParts::UNDER_HANDGUARD); }
				if (this->mount_base.parent == parent_ptr) { Detach_child(&this->mount_base, EnumGunParts::MOUNT_BASE); }
				if (this->dustcover.parent == parent_ptr) { Detach_child(&this->dustcover, EnumGunParts::DUSTCOVER); }
				if (this->stock.parent == parent_ptr) { Detach_child(&this->stock, EnumGunParts::STOCK); }
				//
				if (this->mount_.parent == parent_ptr) { Detach_child(&this->mount_, EnumGunParts::MOUNT); }
				bool contcheck = false;
				do {
					contcheck = false;
					for (auto& s : this->sight_) {
						if (s.parent == parent_ptr) {
							s.Detach(this->per_all);/*Detach_child(&s, EnumGunParts::SIGHT);*/
							this->sight_.erase(this->sight_.begin() + (&s - &this->sight_.front()));
							contcheck = true;
							break;
						}
					}
				} while (contcheck);
				if (this->foregrip.parent == parent_ptr) { Detach_child(&this->foregrip, EnumGunParts::FOREGRIP); }
				if (this->lam.parent == parent_ptr) { Detach_child(&this->lam, EnumGunParts::LAM); }
				//
				switch (type_t) {
				case EnumGunParts::SIGHT:
					if (this->sight_.size() > sight_s) {
						this->sight_[sight_s].Detach(this->per_all);
						this->sight_.erase(this->sight_.begin() + sight_s);
					}
					break;
				default:
					parent_ptr->Detach(this->per_all);
					break;
				}
			}
		public:
			//パーツアタッチ
			void Attach_parts(GUNPARTs* partsptr, EnumGunParts type_t, GParts_COMMON* parents = nullptr, EnumAttachPoint side = EnumAttachPoint::NONE, size_t mount_t = 0) noexcept {
				if (partsptr != nullptr) {
					switch (type_t) {
					case EnumGunParts::NONE:
					{
						break;
					}
					case EnumGunParts::BASE:
					{
						this->Get_parts(type_t)->attach_(this->per_all, *partsptr, type_t, this->per_minimam);
						break;
					}
					case EnumGunParts::MAGAZINE:
					{
						this->Get_parts(type_t)->attach_(this->per_all, *partsptr, type_t);
						break;
					}
					case EnumGunParts::SIGHT:
					{
						if (parents != nullptr && mount_t > 0) {
							if (this->sight_.size() < mount_t) {
								this->sight_.resize(mount_t);
							}
							this->Get_parts(type_t, int(mount_t - 1))->attach_(this->per_all, *partsptr, type_t, parents, side);
						}
						break;
					}
					default:
						this->Get_parts(type_t)->attach_(this->per_all, *partsptr, type_t, parents, side);
						break;
					}
				}
			}
			//パーツ外し
			void Detach_parts(EnumGunParts type_t, int mount_t = 0) noexcept {
				switch (type_t) {
				default:
				{
					this->Detach_child(this->Get_parts(type_t), type_t);
					break;
				}
				case EnumGunParts::SIGHT:
				{
					if (this->sight_.size() > mount_t) { this->Detach_child(this->Get_parts(type_t, mount_t), type_t, mount_t); }
					break;
				}
				case EnumGunParts::NONE:
				{
					break;
				}
				}
			}
		private:
		public:
			//Flag_gunをOFFからONにする
			const auto Set_Flag_gun(void) noexcept {
				if (!this->Flag_gun) {
					this->Flag_gun = true;
					return true;
				}
				return false;
			}
			/*銃pos反映*/
			void Put_gun(bool needonly = false) noexcept {
				this->base.Setpos_Nomal(gun_m.MatIn());
				if (needonly) {
					this->mazzule.Setpos_parts(gun_m.mat);
				}
				else {
					this->underhandguard.Setpos_parts(gun_m.mat);
					this->uperhandguard.Setpos_parts(gun_m.mat);
					this->grip.Setpos_parts(gun_m.mat);
					this->mazzule.Setpos_parts(gun_m.mat);
					this->dustcover.Setpos_parts(gun_m.mat);
					this->stock.Setpos_parts(gun_m.mat);
					this->foregrip.Setpos_parts(gun_m.mat);
					this->lam.Setpos_parts(gun_m.mat);
					this->mount_base.Setpos_parts(gun_m.mat);
					this->mount_.Setpos_parts(gun_m.mat);
					for (auto& s : this->sight_) { s.Setpos_parts(gun_m.mat); }
				}
			}
			//銃セレクターのアニメセット
			void Set_select_anime(void) noexcept {
				this->base.thisparts->Set_gun_select(this->gunanime_sel, this->selecter);
			}
			//銃発砲アニメのセット
			void Set_shot_anime(const float& rate, const bool isNotLast) noexcept {
				if (isNotLast) {
					this->gunanime_shot->per = 1.f;
					if (this->Flag_gun) {
						this->gunanime_shot->Update(true, rate * 2.f * this->gunanime_shot->alltime / (Frame_Rate / (600.f / 60.f)));
						if (this->gunanime_shot->time == 0.f) {
							this->Flag_gun = false;
						}
					}
				}
				else {
					this->gunanime_shot_last->per = 1.f;
					this->gunanime_shot_last->Update(false, rate * 2.f * this->gunanime_shot_last->alltime / (Frame_Rate / (600.f / 60.f)));
					if (this->gunanime_shot_last->time == 0.f) {
						this->Flag_gun = false;
					}
				}
			}
			//
			void calc_shot_effect(Effect_UseControl* effectowner) noexcept {
				switch (this->mazzule.Get_mazzuletype())
				{
				case 0:
					effectowner->Set_Effect(Effect::ef_fire, this->Get_maz(), this->gun_m.mat.zvec() * -1.f, 0.0025f / 0.1f);//ノーマル
					break;
				case 1:
					effectowner->Set_Effect(Effect::ef_fire2, this->Get_maz(), this->gun_m.mat.zvec() * -1.f, 0.0025f / 0.1f);//サプ
					break;
				default:
					effectowner->Set_Effect(Effect::ef_fire, this->Get_maz(), this->gun_m.mat.zvec() * -1.f, 0.0025f / 0.1f);//何もついてない
					break;
				}
			}
		public:
			//
			void Init(void) noexcept {
				this->in_chamber = false;
				this->gun_stat_now->Reset();
				for (int i = 0; i < 6; ++i) {
					if (this->gun_stat_now->magazine_pushback(this->magazine.thisparts)) {
						this->in_chamber = true;
					}
				}
				this->Flag_gun = false;
				this->gunanime_shot->Reset();
				this->gunanime_shot_last->Reset();
				this->gunanime_trigger->Reset();
				this->gunanime_magcatch->Reset();
				this->gunanime_first->Reset();
				for (auto& s : this->gunanime_sel) { s->Reset(); }
				this->gunanime_Changesel->Reset();
				this->distance_to_cam = -1.f;
				//start
				this->gunanime_first->per = 1.f;
				{
					int sel_s = -1;
					for (auto& s : this->sight_) {
						s.Set_sight_vec();
						if (s.IsActive()) {
							sel_s = int(&s - &this->sight_.front());
						}
					}
					select_sight = (sel_s != -1) ? size_t(sel_s) : SIZE_MAX;
				}
			}
			//
			void Load_Mag() {
				this->gunanime_shot->per = 1.f;
				this->gunanime_shot_last->Reset();
				if (!this->in_chamber) {
					this->in_chamber = true;
					this->gunanime_first->Reset();
					this->gunanime_first->per = 1.f;
					this->gun_stat_now->chamber_in();//チャンバーに装填
				}
				SE.Get(EnumSound::MAG_Set).Play_3D(Audio.use_magset, gun_m.pos, 15.f);
			}
			//
			void Calc_blur(void) noexcept {
				//手振れ
				easing_set(&this->blur_vec, this->blur_vec_res, 0.975f);
				//this->blur_vec = VECTOR_ref::front();
				easing_set(&this->blur_vec_res, VECTOR_ref::front(), 0.95f);
				//複座
				easing_set(&this->recoil_vec, this->recoil_vec_res, 0.6f);
				easing_set(&this->recoil_vec_res, VECTOR_ref::front(), 0.95f);
				//リコイルアニメーション
				if (this->Flag_gun) {
					Set_shot_anime(1.f, this->in_chamber);
					if (this->in_chamber) {
						this->gunanime_shot_last->Reset();
					}
					else {
						this->gunanime_shot->Reset();
					}
				}
			}
			bool Calc(float per_blur, bool isreload, bool isview) {
				bool ans = false;
				if (!this->Flag_gun && this->in_chamber) {
					if (this->base.thisparts->Select_Chose(EnumSELECTER::FULL) == int(this->selecter)) {
						this->trigger.second = 0;
					}
				}
				//ブレをセット
				{
					float xup = 6.f;
					float xdn = -6.f;
					float yup = 6.f;
					float ydn = -6.f;

					switch (this->stock.Get_stocktype()) {
					case 1://チクパなしストック
						xup = 4.f;
						xdn = -4.f;
						yup = 4.f;
						ydn = -4.f;
						break;
					case 2://チクパありストック
						xup = 4.f;
						xdn = -4.f;
						yup = 4.f;
						ydn = -4.f;
						break;
					default://ストック無し
						xup = 6.f;
						xdn = -6.f;
						yup = 6.f;
						ydn = -6.f;
						break;
					}

					easing_set(&this->blur_vec_res,
						MATRIX_ref::Vtrans(this->blur_vec_res,
							MATRIX_ref::RotY(deg2rad(float((int32_t)(xdn * 100.f) + GetRand((int32_t)((xup - xdn) * 100.f))) / (100.f * per_blur))) *
							MATRIX_ref::RotX(deg2rad(float((int32_t)(ydn * 100.f) + GetRand((int32_t)((yup - ydn) * 100.f))) / (100.f * per_blur))))
						, 0.8f);
				}
				//
				this->trigger.GetInput(this->gunanime_trigger->per >= 0.5f);
				if (this->trigger.trigger()) {
					if (!this->Flag_gun) {
						/*
						//サウンド
						if (!Get_Audio().trigger.check()) {
							if (this->Get_mazzuletype() == 1) {
								Get_Audio().trigger.vol(164);
							}
							else {
								Get_Audio().trigger.vol(255);
							}
							Get_Audio().trigger.play_3D(gun_m.pos, 5.f);
						}
						*/
					}
					if (!this->Flag_gun && this->in_chamber && this->gunanime_first->per == 0.f && !isview) {
						this->Flag_gun = true;
						//弾数管理
						if (this->gun_stat_now->magazine_shot(isreload) || isreload) {
							this->in_chamber = false;							//チャンバーを空にする
							this->in_chamber = false;
						}
						{
							float xup = this->base.thisparts->Get_recoil_xup() * this->per_all.recoil / 100.f;
							float xdn = this->base.thisparts->Get_recoil_xdn() * this->per_all.recoil / 100.f;
							float yup = this->base.thisparts->Get_recoil_yup() * this->per_all.recoil / 100.f;
							float ydn = this->base.thisparts->Get_recoil_ydn() * this->per_all.recoil / 100.f;

							auto ppud = per_blur;							//持ち手を持つとココが相殺される

							this->recoil_vec_res = MATRIX_ref::Vtrans(this->recoil_vec_res,
								MATRIX_ref::RotY(deg2rad(float((int32_t)(xdn * 100.f) + GetRand((int32_t)((xup - xdn) * 100.f))) / (100.f * ppud))) *
								MATRIX_ref::RotX(deg2rad(float((int32_t)(ydn * 100.f) + GetRand((int32_t)((yup - ydn) * 100.f))) / (100.f * ppud))));
						}
						ans = true;
					}
				}
				return ans;
			}
			void Calc_Gunanime(int Slide_Audio) {
				if (this->gunanime_first->per == 1.f) {
					if ((this->gunanime_first->time > this->gunanime_first->alltime / 3.f) && this->slide_sound_f) {
						SE.Get(EnumSound::Slide).Play_3D(Slide_Audio, this->gun_m.pos, 15.f);
						this->slide_sound_f = false;
					}
					this->gunanime_first->Update(false, 0.35f);
					if (this->gunanime_first->time >= this->gunanime_first->alltime) {
						this->gunanime_first->Reset();
						this->gunanime_shot->per = 1.f;
					}
				}
				else {
					this->slide_sound_f = true;
				}
				this->Calc_blur();
			}
			//銃の落下
			void Set_Gun_Physics(const VECTOR_ref& vec_t) noexcept {
				this->gun_m.vec = vec_t;
				this->CanUse_Physics = false;
			}
			void Start_Gun_Physics() noexcept {
				this->CanUse_Physics = true;
			}
			void Update_Gun_Physics(std::shared_ptr<MAPclass::Map>& MAPPTs) {
				if (CanUse_Physics) {
					this->gun_m.Update_Physics();
					auto ColResGround = MAPPTs->map_col_line(gun_m.pos + VECTOR_ref::up(), gun_m.pos - (VECTOR_ref::up() * 0.05f));
					if (ColResGround.HitFlag == TRUE) {
						this->gun_m.HitGround(ColResGround, 0.05f);
						easing_set(&this->gun_m.vec, VECTOR_ref::zero(), 0.8f);
					}
					Put_gun();
				}
			}
			//
			void Update_Gun(std::shared_ptr<MAPclass::Map>& MAPPTs, const float Cart_rate_t) {
				//薬莢の処理
				for (auto& a : this->Cart) {
					//待機中の移動
					a.Put_second(this->base.Get_Cart_pos(), gun_m.mat);
					//解放
					a.Put_stay(this->base.Get_Cart_vec() * 2.5f / FPS, Cart_rate_t);
					//解放後の移動
					if (a.Get_Flag()) {
						a.Update_pos();
						a.Update_fall(MAPPTs->map_col_line(a.Get_pos() + VECTOR_ref::up(), a.Get_pos() - (VECTOR_ref::up() * 0.008f)));
					}
				}
				//銃アニメ更新
				this->base.Update_Anim();

			}
			//サイトを画面に表示
			const float DrawReticle_UI(bool ads) noexcept {
				if (select_sight != SIZE_MAX) {
					return this->sight_[select_sight].Draw_reticle_UI(ads);
				}
				else {
					return 1.f;
				}
			}
			//銃描画
			void Draw_Gun_Common(void)const noexcept {
				this->base.Draw();
				if (this->distance_to_cam <= 5.f) {
					for (auto& a : this->Cart) { a.Draw(); }
				}
				if (this->distance_to_cam <= 10.f) {
					this->mazzule.Draw();
					this->grip.Draw();
					this->uperhandguard.Draw();
					this->underhandguard.Draw();
					this->mount_base.Draw();
					this->mount_.Draw();
					this->dustcover.Draw();
					this->stock.Draw();
					this->foregrip.Draw();
					this->lam.Draw();
					for (auto& s : sight_) { s.Draw(); }
				}
				if (this->IsDrawmagazine) {
					if (this->gun_stat_now != nullptr && this->gun_stat_now->hav_mag()) {
						Get_mag_in().front().Draw();
					}
				}
			}
			/*おわり*/
			void Dispose_Gun(void) noexcept {
				Dispose_Gun_Buf();
			}
		};
		//戦車のフレーム、アニメ管理
		class VEHICLE_HitControl {
		private:
			class pair_hit {
				size_t first{ SIZE_MAX };
				float second{ -1 };
			public:
				bool operator<(const pair_hit& tgt) const noexcept {
					return this->second < tgt.second;
				}
				void Set(size_t f_t, float s_t)noexcept {
					first = f_t;
					second = s_t;
				}
				bool Hit_Check(std::optional<size_t>& Hitnear, BULLETS& mine_b, const std::shared_ptr<PLAYER_VEHICLE>& tgt) noexcept {
					//装甲面に当たらなかったならスルー
					if (this->second == (std::numeric_limits<float>::max)()) {
						return false;
					}
					//装甲面に当たったのでHitnearに代入して終了
					for (auto& a : tgt->use_veh->Get_armer_mesh()) {
						if (this->first == a.first) {
							Hitnear = this->first;
							//ダメージ面に当たった時に装甲値に勝てるかどうか
							mine_b.Penetration(a, tgt, tgt->hitControl.Get_hitres(Hitnear.value()).HitPosition, tgt->hitControl.Get_hitres(Hitnear.value()).Normal);
							break;
						}
					}
					if (Hitnear.has_value()) {
						return false;
					}
					//空間装甲、モジュールに当たったのでモジュールに30ダメ、貫徹力を1/2に
					for (auto& a : tgt->use_veh->Get_space_mesh()) {
						if (this->first == a) {
							mine_b.hit_effect(tgt->hitControl.Get_hitres(a).Normal);
							tgt->Damage.SubHP_Parts(30, a);
						}
					}
					for (auto& a : tgt->use_veh->Get_module_mesh()) {
						if (this->first == a) {
							mine_b.hit_effect(tgt->hitControl.Get_hitres(a).Normal);
							tgt->Damage.SubHP_Parts(30, a);
						}
					}
					return true;
				}
			};
		private:
			std::vector<pair_hit> hitssort;												/*フレームに当たった順番*/
			std::vector<MV1_COLL_RESULT_POLY> hitres;									/*確保*/
		public:
			void sort_Hit(void) noexcept { std::sort(this->hitssort.begin(), this->hitssort.end(), [](const pair_hit& x, const pair_hit& y) { return x < y; }); }
			std::vector<pair_hit>& Get_hitssort(void) noexcept { return this->hitssort; }
			const MV1_COLL_RESULT_POLY& Get_hitres(size_t ID_t) const noexcept { return this->hitres[ID_t]; }
			bool HitCheck(int m, const VECTOR_ref& repos, const MV1_COLL_RESULT_POLY& hitres_t) {
				this->hitres[m] = hitres_t;
				if (this->hitres[m].HitFlag == TRUE) {
					this->hitssort[m].Set(m, (repos - this->hitres[m].HitPosition).size());
					return true;
				}
				else {
					this->hitssort[m].Set(m, (std::numeric_limits<float>::max)());
					return false;
				}
			}
			void Set(size_t siz_t) {
				this->hitres.resize(siz_t);		//モジュールごとの当たり判定結果を確保
				this->hitssort.resize(siz_t);	//モジュールごとの当たり判定順序を確保
			}
			void Reset(void) noexcept {
				this->hitres.clear();
				this->hitssort.clear();
			}
		};
		//人間
		class PLAYER_CHARA : public PLAYER_COMMON, public HumanControl {
		private:
			//スコア
			class score_s {
				float death_timer{ 0.f };
			public:
				float kill_timer{ 0.f };

				int kill_cnt{ 0 };
				int death_cnt{ 0 };
				size_t kill_id{ 0 };
				size_t death_id{ 0 };
				bool kill_f{ false };
				bool death_f{ false };

				int score{ 0 };
				int kill_streak{ 0 };
				//
				void Reset(bool allReset) {
					this->kill_f = false;
					this->death_f = false;
					this->kill_id = 0;
					this->kill_streak = 0;
					this->kill_timer = 0.f;
					this->death_id = 0;
					if (allReset) {
						this->score = 0;
						this->kill_cnt = 0;
						this->death_cnt = 0;
					}
				}
				void Update(void) noexcept {
					if (this->death_f) {
						this->death_timer -= 1.f / FPS;
						if (this->death_timer <= 0.f) {
							this->death_timer = 0.f;
							this->death_f = false;
						}
					}

					if (this->kill_f) {
						this->kill_timer -= 1.f / FPS;
						if (this->kill_timer <= 0.f) {
							this->kill_timer = 0.f;
							this->kill_streak = 0;
							this->kill_f = false;
						}
					}
				}
				void Set_kill(size_t deathid, int addpoint) {
					this->kill_f = true;
					this->kill_id = deathid;
					this->kill_timer = 7.f;
					++this->kill_cnt;
					//
					if (this->kill_timer != 0.f) {
						++this->kill_streak;
						this->score += std::clamp(25 - this->kill_streak * 5, 0, 25);
					}
					this->score += addpoint;
				}
				void Set_death(size_t killid) {
					this->death_f = true;
					this->death_id = killid;
					this->death_timer = 3.f;
					++this->death_cnt;
				}
				bool start_death(void) const noexcept { return death_timer == 3.f; }
			};
			//AI用
			class AI {
			public:
				float ai_time_a{ 0.f };
				float ai_time_d{ 0.f };
				float ai_time_q{ 0.f };
				float ai_time_e{ 0.f };
				float ai_time_shoot{ 0.f };
				float ai_time_tankback{ 0.f };
				float ai_time_tankback_ing{ 0.f };
				int ai_tankback_cnt = 0;
				float ai_time_walk{ 0.f };
				bool ai_reload{ false };
				int ai_phase{ 0 };
				std::vector<int> wayp_pre{ 0 };

				AI(void) noexcept {
					wayp_pre.resize(6);
				}
				~AI(void) noexcept {
					wayp_pre.clear();
				}

				void Spawn(int now) {
					this->ai_phase = 0;
					this->ai_time_shoot = 0.f;
					this->ai_time_a = 0.f;
					this->ai_time_d = 0.f;
					this->ai_time_e = 0.f;
					this->ai_time_tankback = 0.f;
					this->ai_time_tankback_ing = 0.f;
					this->ai_tankback_cnt = 0;
					this->fill_wayp_pre(now);
				}
				void fill_wayp_pre(int now) {
					for (auto& w : this->wayp_pre) { w = now; }
				}
				void Set_wayp_pre(int now) {
					for (size_t i = (this->wayp_pre.size() - 1); i >= 1; i--) { this->wayp_pre[i] = this->wayp_pre[i - 1]; }
					this->wayp_pre[0] = now;
				}
				void Set_wayp_return(void) noexcept {
					auto ppp = this->wayp_pre[1];
					for (size_t i = (this->wayp_pre.size() - 1); i >= 1; i--) { this->wayp_pre[i] = this->wayp_pre[0]; }
					this->wayp_pre[0] = ppp;
				}

				void Draw_Debug(std::shared_ptr<MAPclass::Map>& MAPPTs_t) {
					for (size_t i = 0; i < this->wayp_pre.size() - 1; i++) {
						VECTOR_ref startpos = MAPPTs_t->Get_waypoint()[this->wayp_pre[i]];
						VECTOR_ref endpos = MAPPTs_t->Get_waypoint()[this->wayp_pre[i + 1]];
						startpos.y(3.f);
						endpos.y(3.f);
						DXDraw::Capsule3D(startpos, endpos, 1.f, GetColor(0, 255, 0), GetColor(0, 255, 0));
					}
				}
			};
		public:
			score_s scores;
			std::vector<GunControl::GUN_STATUS> gun_stat;	/*所持弾数などの武器データ*/
		private:
			size_t sel_Gun = 0;
			std::shared_ptr<PLAYERclass::GunControl> Gun_Ptr{ nullptr };
			AI cpu_do;										//AI用
			bool have_magazine{ true };						//マガジンを持つか
			bool throw_grenades{ false };					//グレネードを投げるか
			bool throw_grenade_old{ false };				//グレネードを投げるか()
			MV1 obj_lag;									//ラグドール
			VECTOR_ref add_vec_buf;							//移動ベクトルバッファ
			VECTOR_ref pos_tt;								/*プレイヤー位置*/
			float foot_timer{ 0.f };						/*足音タイマー*/
			float breath_timer{ 0.f };						/*息タイマー*/
			float reload_cnt{ 0.f };						//リロード開始までのカウント
			MATRIX_ref mat_body;							//位置
			bool sort_ing{ false };							//整頓
			float sorting_timer{ 0.f };
			bool view_ing{ false };							//眺める
			size_t view_ings{ 0 };							//
			size_t reload_ings{ 0 };						//
			size_t throw_gre_ings{ 0 };						//
			VECTOR_ref gunpos;								//マウスエイム用銃座標
			opes key_;										//入力関連
			float rad_gun{ 0.f };							//
			float add_ypos{ 0.f };							//垂直加速度
			float body_xrad{ 0.f };							//胴体角度
			float body_zrad{ 0.f };							//胴体角度
			size_t canget_id{ 0 };							//取得可能アイテムID
			std::string canget_mag;							//取得可能アイテム(マガジン)
			std::string canget_med;							//取得可能アイテム(衣料品)
			/*フラグ*/
			bool reloadf{ false };
			bool sort_f{ false };
			bool gre_eff{ false };
			bool canget_magitem_f{ false };					//取得可能アイテム(マガジン)
			bool canget_meditem_f{ false };					//取得可能アイテム(衣料品)
			bool canride_f{ false };						//
			bool Flag_canlook_player{ true };				//プレイヤーが視界内か否かのフラグ
			bool Flag_calc_lag{ true };						//ラグドールの物理演算フラグ
		public:
			const MATRIX_ref Head_direction(void)const noexcept override { return BodyFrameMatrix(this->obj_body, this->frame_s.head_f); }
			const VECTOR_ref Head_Position(void) const noexcept { return this->Get_head_pos(); }
			//
			void subHP(const VECTOR_ref& Pos, const VECTOR_ref& Nomal, int sel, int damage, const std::shared_ptr<PLAYER_CHARA>& tgt) noexcept {
				//Hit
				if (isRide()) {
					(*MINE_v)->Set_Effect(Effect::ef_hitblood, Pos, Nomal, 0.1f / 0.1f);//Effect::ef_reco
				}
				else {
					(*MINE_c)->Set_Effect(Effect::ef_hitblood, Pos, Nomal, 0.1f / 0.1f);//Effect::ef_reco
				}
				//
				if ((*MINE_c)->Damage_Calc(sel, damage, tgt)) {
					VOICE.Get(EnumSound::Voice_Death).Play_3D(0, tgt->Get_pos(), 10.f);
				}
				else {
					VOICE.Get(EnumSound::Voice_Damage).Play_3D(0, tgt->Get_pos(), 10.f);
				}
				return;
			}
			//setter
			auto& Set_Gun_(void) noexcept { return *this->Gun_Ptr; }
			auto& Set_key_(void) noexcept { return key_; }	//key
			void Set_gre_eff(bool f_t) noexcept { gre_eff = f_t; }																			//item関連
			void Set_reloadf(bool f_t) noexcept { reloadf = f_t; }																			//item関連
			void Set_sort_f(bool f_t) noexcept { sort_f = f_t; }																			//item関連
			//getter
			const VECTOR_ref Get_Pos_Map(void) const noexcept { return (isRide()) ? (*MINE_v)->Get_pos() : this->Get_pos(); }				//ミニマップ用+
			const auto Get_yrad(void) const noexcept { return (isRide()) ? (*MINE_v)->Get_body_yrad() : this->Get_body_yrad(); }			//ミニマップ用
			const VECTOR_ref Get_head_pos(void) const noexcept { return this->obj_body.frame(this->frame_s.head_f.first); }						//頭部座標
			const auto Get_position(bool no_y = false) const noexcept {
				if (DrawPts->use_vr) {
#ifdef _USE_OPENVR_
					return (!no_y) ? (this->pos_tt + this->HMD.move.pos - this->HMD.move_start.pos) : (this->pos_tt + this->HMD.Get_pos_noY() - this->HMD.move_start.pos);
#endif // _USE_OPENVR_
				}
				else {
					return (!no_y) ? this->pos_tt + this->HMD.move.pos : this->pos_tt;
				}
			}
			const auto& Get_key_(void) const noexcept { return key_; }																		//key
			const auto& getItem_push(void) const noexcept { return this->key_.have_item; }												//item関連
			const auto& Get_gre_eff(void) const noexcept { return gre_eff; }																//item関連
			const auto& Get_canride_f(void) const noexcept { return canride_f; }															//item関連
			const auto& Get_canget_mag_f(void) const noexcept { return canget_magitem_f; }													//item関連
			const auto& Get_canget_med_f(void) const noexcept { return canget_meditem_f; }													//item関連
			const auto& Get_canget_mag_s(void) const noexcept { return canget_mag; }														//item関連
			const auto& Get_canget_med(void) const noexcept { return canget_med; }															//item関連
			const auto& Get_Gun_(void) const noexcept { return *this->Gun_Ptr; }

			const bool is_ADS(void) const noexcept override { return this->key_.aim && !this->view_ing; }									//ADS中
			const bool isJamping(void) const noexcept override { return this->add_ypos != 0.f; }											//ジャンプ中
			const bool isSquat(void) const noexcept override { return this->key_.squat; }
			const bool isRunning(void) const noexcept override { return this->key_.running; }
			const VECTOR_ref Get_pos(void) const noexcept override { return Get_position(); }
		public:
			//
			void Set_basepos(const VECTOR_ref& pos_t) noexcept { this->pos_tt = pos_t; }
			//
			void calc_gredamage(const std::shared_ptr<PLAYER_CHARA>& killer) noexcept {
				if (this->Damage.Get_alive()) {
					float scale = (this->move.pos - this->Get_head_pos()).size();
					if (scale < 10.f) {
						if (!(MAPPTs->map_col_line(this->move.pos, this->Get_head_pos()).HitFlag == TRUE)) {
							int damage = std::clamp(int(150.f * (10.f - scale) / 10.f), 0, 100);
							killer->subHP(this->Get_head_pos(), VECTOR_ref::front(), 0, damage, *MINE_c);
							this->gre_eff = true;
							return;
						}
					}
				}
				return;
			}
			//item
			void ReSet_canget_item(void) noexcept {
				canget_magitem_f = false;
				canget_meditem_f = false;
				return;
			}
			void addf_canget_magitem(bool zz) noexcept {
				canget_magitem_f |= zz;
				return;
			}
			void addf_canget_meditem(bool zz) noexcept {
				canget_meditem_f |= zz;
				return;
			}
			void Set_canget_mag(size_t id_t, const std::string& itemname) noexcept {
				canget_id = id_t;
				canget_mag = itemname;
			}
			void Set_canget_med(size_t id_t, const std::string& itemname) noexcept {
				canget_id = id_t;
				canget_med = itemname;
			}
		public:
			//視点取得
			void Set_HMDpos(void) noexcept {
				this->HMD.move.pos = (this->obj_body.frame(this->frame_s.RIGHTeye_f.first) + (this->obj_body.frame(this->frame_s.LEFTeye_f.first) - this->obj_body.frame(this->frame_s.RIGHTeye_f.first)) * 0.5f) - this->pos_tt;
			}
			const MATRIX_ref& GetHMDmat(void) noexcept { return HMD.move.mat; }
			void SetHMDmat(const MATRIX_ref& value_t) noexcept { HMD.move.mat = value_t; }
		private:
			//腕アニメで使う座標の決定
			void Set_GUN_pos_Anim(void) noexcept {
				for (int sel_t = 0; sel_t < 3; ++sel_t) {
					Set_lefthand(0, sel_t, Set_Gun_().LEFT_pos_gun(sel_t));
					Set_lefthand(1, sel_t, Set_Gun_().LEFT_pos_mag(sel_t));
					Set_lefthand(2, sel_t, this->obj_body.frame(this->frame_s.LEFTBodyFrame[sel_t].first));
					Set_lefthand(3, sel_t, Set_Gun_().LEFT_pos_mag_ready(sel_t));
				}
			}
		private:
			//リロード中か否か
			const auto isReload(void) const noexcept { return this->reloadf && Get_Gun_().Get_gun_stat_now()->hav_mag(); }//リロード中
			//腕アニメの決定
			void Set_LEFT_pos_Anim(void) noexcept {
				if (!isReload()) {
					this->have_magazine = false;					//リロード初期化
					this->reload_ings = 0;							//
				}
				//
				Set_ReloadTime(Set_Gun_().Get_reloadtime());
				//
				if (this->isRunning() && !isReload() && !this->sort_ing && !this->view_ing) {
					//走る
					Func_Set_LEFT_pos_Anim(1, 0, 
						VECTOR_ref::vget(-0.75f, 0.15f, 0.35f),
						VECTOR_ref::vget(0.05f + (Head_bobbing(this->anime_run) * 0.04f), -0.04f + (Head_bobbing(this->anime_run) * 0.012f), -0.1f), -80.f);
					return;
				}
				else {
					if (this->isReload()) {
						switch (this->reload_ings) {
						case 0:
						{
							//リロード開始
							if (Func_Set_LEFT_pos_Anim(2, 2, VECTOR_ref::vget(0.35f, -0.25f, 0.9f), VECTOR_ref::vget(0, 0.1f, -0.025f), 15.f)) {
								this->reload_ings++;
								this->have_magazine = true;
							}
							return;
						}
						case 1:
						{
							//マガジン装着
							if (Func_Set_LEFT_pos_Anim(15, 3, VECTOR_ref::vget(0.f, -0.25f, 0.9f), VECTOR_ref::vget(0.f, 0.05f, -0.025f), 0.f)) {
								this->reload_ings++;
							}
							return;
						}
						case 2:
						{
							//マガジン装着
							if (Func_Set_LEFT_pos_Anim(16, 3, VECTOR_ref::vget(0.f, -0.25f, 0.9f), VECTOR_ref::vget(0.f, 0.05f, -0.025f), 10.f)) {
								this->reload_ings++;
							}
							return;
						}
						case 3:
						{
							//マガジン装着
							if (Func_Set_LEFT_pos_Anim(3, 1, VECTOR_ref::vget(0.f, -0.25f, 0.9f), VECTOR_ref::vget(0.f, 0.05f, -0.025f), 10.f)) {
								this->reload_ings++;
							}
							return;
						}
						case 4:
						{
							//マガジン装着
							if (Func_Set_LEFT_pos_Anim(17, 1, VECTOR_ref::vget(0.f, -0.25f, 0.9f), VECTOR_ref::vget(0.f, 0.05f, -0.025f), 10.f)) {
								this->reload_ings++;
							}
							return;
						}
						case 5:
						{
							//マガジン装着後
							if (Func_Set_LEFT_pos_Anim(4, 1, VECTOR_ref::vget(0.f, -0.25f, 0.9f), VECTOR_ref::vget(0.f, 0.05f, -0.025f), 0.f)) {
								this->reload_ings = 0;
							}
							return;
						}
						default:
							break;
						}
					}
					//マグ整頓
					if (this->sort_ing) {
						Func_Set_LEFT_pos_Anim(5, 2, VECTOR_ref::vget(0.35f, -0.25f, 0.9f), VECTOR_ref::vget(0, 0.1f, -0.025f), 15.f);
						return;
					}
					//初段装填
					if (Set_Gun_().Get_gunanime_first()->per == 1.f) {
						Func_Set_LEFT_pos_Anim(6, 0, VECTOR_ref::vget(0.f, -0.75f, 0.9f), VECTOR_ref::vget(0.05f, -0.035f, -0.055f), -40.f);
						return;
					}
					//グレ投擲
					if (this->throw_grenades) {
						switch (this->throw_gre_ings) {
						case 0:
						{
							if (Func_Set_LEFT_pos_Anim(12, 2, VECTOR_ref::vget(0.f, 0.5f, 0.5f), VECTOR_ref::vget(0.f, -0.05f, 0.f), 0.f)) {
								this->throw_gre_ings = 0;
								this->throw_grenades = false;
							}
							return;
						}
						default:
							break;
						}
						return;
					}
					//眺める
					if (this->view_ing) {
						switch (this->view_ings) {
						case 0:
						{
							if (Func_Set_LEFT_pos_Anim(7, 2, VECTOR_ref::vget(-0.55f, -0.05f, 0.45f), VECTOR_ref::vget(0.05f, 0.1f, -0.125f), -85.f)) {
								this->view_ings++;
							}
							return;
						}
						case 1:
						{
							if (Func_Set_LEFT_pos_Anim(8, 2, VECTOR_ref::vget(-0.55f, -0.05f, 0.45f), VECTOR_ref::vget(0.05f, 0.1f, -0.125f), -95.f)) {
								this->view_ings++;
							}
							return;
						}
						case 2:
						{
							if (Func_Set_LEFT_pos_Anim(9, 0, VECTOR_ref::vget(-0.55f, -0.15f, 0.65f), VECTOR_ref::vget(0.05f, 0.1f, -0.2f), 40.f)) {
								this->view_ings++;
							}
							return;
						}
						case 3:
						{
							if (Func_Set_LEFT_pos_Anim(10, 0, VECTOR_ref::vget(-0.55f, -0.15f, 0.65f), VECTOR_ref::vget(0.05f, 0.1f, -0.2f), 40.f)) {
								this->view_ings = 0;
								this->view_ing = false;
							}
							return;
						}
						default:
							break;
						}
					}
					if (Set_Gun_().Get_gunanime_Changesel()->per == 1.f) {
						if (Func_Set_LEFT_pos_Anim(13, 0, VECTOR_ref::vget(0.f, -0.75f, 0.9f), VECTOR_ref::vget(0.05f, -0.0025f, -0.075f), -60.f)) {
							Set_Gun_().ReSet_gunanime_Changesel();
						}
						return;
					}
					//ノーマル
					if (isJamping()) {
						if (this->Damage.Get_HP_parts()[2] == 0) {
							Func_Set_LEFT_pos_Anim(11, 2, VECTOR_ref::vget(-0.4f, 0.05f, 0.9f), VECTOR_ref::vget(0.f, -0.05f, 0.f), 0.f);
						}
						else {
							Func_Set_LEFT_pos_Anim(14, 0, VECTOR_ref::vget(-0.4f, 0.05f, 0.9f), VECTOR_ref::vget(0.f, -0.05f, 0.f), 0.f);
						}
					}
					else {
						float buf = 0.f;
						if (!this->isSquat()) {
							buf = DX_PI_F / 2.f*Head_bobbing(this->anime_walk);
						}
						else {
							buf = DX_PI_F / 2.f*Head_bobbing(this->anime_swalk);
						}

						VECTOR_ref vec = VECTOR_ref::front();
						vec.x(0.001f * sin(buf));
						vec.y(0.002f * sin(buf*2.f));
						vec = vec.Norm();

						VECTOR_ref pos = VECTOR_ref::zero();
						pos.x(-0.001f * sin(buf)*sin(buf / 2.f));
						pos.y(0.001f * sin(buf));
						pos.z(0.0015f * sin(buf / 2.f)*sin(buf / 3.f));
						float rad = 0.f;

						rad = 1.5f*sin(buf)*sin(buf/3.f);


						if (this->Damage.Get_HP_parts()[2] == 0) {
							Func_Set_LEFT_pos_Anim(11, 2, vec, pos, rad);
						}
						else {
							Func_Set_LEFT_pos_Anim(0, 0, vec, pos, rad);
						}
					}
					return;
				}
			}
			//アイテムリリース
			void release(GunControl::MAG_STATUS* mag) noexcept {
				auto& item = MAPPTs->item;
				moves tmp = Set_Gun_().Get_move_mag();
				tmp.vec = (Set_Gun_().Get_magf_pos() - Set_Gun_().Get_mag2f_pos()).Norm() * 1.f / FPS;
				for (auto& i : item) {
					if (i->Set_item_(mag->Get_Info(), tmp, mag->Get_Cnt())) { return; }
				}
				item.emplace_back(std::make_shared<Items>(item.size(), mag->Get_Info(), tmp, mag->Get_Cnt()));
			}
			void release(Meds* meddata) noexcept {
				auto& item = MAPPTs->item;
				moves tmp = Set_Gun_().Get_move_mag();
				tmp.pos = Set_Gun_().Get_maz();
				tmp.vec = (Set_Gun_().Get_move_gun().mat.zvec()).Norm() * -5.f / FPS;
				for (auto& i : item) {
					if (i->Set_item_(meddata, tmp)) {
						return;
					}
				}
				item.emplace_back(std::make_shared<Items>(item.size(), meddata, tmp));
			}
			void release(Grenades* gredata) noexcept {
				auto& item = MAPPTs->item;
				moves tmp = Set_Gun_().Get_move_mag();
				tmp.pos = this->LEFTHAND.move.pos;
				tmp.vec = (MATRIX_ref::Vtrans(this->GetHMDmat().zvec(), MATRIX_ref::RotAxis(this->GetHMDmat().xvec(), deg2rad(15)))).Norm() * -17.5f / FPS;
				for (auto& i : item) {
					if (i->Set_item_(gredata, tmp)) {
						return;
					}
				}
				item.emplace_back(std::make_shared<Items>(item.size(), gredata, tmp));
			}
			//マガジン排出
			void magazine_release(void) noexcept {
				release(&Set_Gun_().Set_mag_in().front());//アイテムを落とす
				Set_Gun_().Get_gun_stat_now()->Get_magazine_in_pop_front();//先頭削除
			}
			void magrelease_t(size_t test_) noexcept {
				if (Set_Gun_().Get_gun_stat_now()->hav_mag()) {
					//音
					SE.Get(EnumSound::MAG_Down).Play_3D(Set_Gun_().Audio.use_magdown, Set_Gun_().Get_move_gun().pos, 15.f, 255);
					//弾数
					if (test_ == 0) {//全排出
						while (true) {
							magazine_release();
							if (!Set_Gun_().Get_gun_stat_now()->hav_mag()) { break; }
						}
					}
					if (test_ == 1) {//通常排出
						magazine_release();
					}

					if (test_ == 2) {//末尾排出
						release(&Set_Gun_().Set_mag_in().back());//アイテムを落とす
						Set_Gun_().Get_gun_stat_now()->Get_magazine_in_pop_back();//末尾削除
					}
					this->reload_cnt = 0.f;
					return;
				}
			}
			//腕のフレーム操作
			void move_Lefthand(const bool isusewaist, const MATRIX_ref& m_inv) noexcept {
				//左
				move_LeftArm(DrawPts, this->obj_body, isusewaist, m_inv);
			}
			void move_Righthand(const bool isusewaist, const MATRIX_ref& m_inv) noexcept {
				//右
				Set_Righthand(0, 0, Set_Gun_().RIGHT_pos_gun(0));
				Set_Righthand(0, 1, Set_Gun_().RIGHT_pos_gun(1));
				Set_Righthand(0, 2, Set_Gun_().RIGHT_pos_gun(2));
				move_RightArm(DrawPts, this->obj_body, isusewaist, m_inv);
			}
			//銃器の位置指定(右手が沿うため)
			void move_nomal_gun(void) noexcept {
				{
					VECTOR_ref Pos;
					//指定
					if (this->is_ADS()) {
						Pos = VECTOR_ref::vget(-0.035f, Head_bobbing(this->anime_walk) * 0.001f, -0.15f);
					}
					else {
						Pos = VECTOR_ref::vget(-0.15f, -0.07f + Head_bobbing(this->anime_walk) * 0.002f, -0.15f);
					}
					easing_set(&this->gunpos, Set_Gun_().Get_sightVec() + Pos, std::min(Set_Gun_().Get_Per_Sub(0.75f, 0.9f), 0.935f));
				}
				//視点を一時取得(getpos対処)
				Set_HMDpos();
				//通常
				{
					MATRIX_ref recoil = Set_Gun_().Get_res_blur(1.f) * this->GetHMDmat();//リコイル
					if (DrawPts->use_vr) {
#ifdef _USE_OPENVR_
						Set_Gun_().Set_gun_posmat(this->Get_pos() + MATRIX_ref::Vtrans(this->gunpos, recoil) - this->HMD.move_start.pos, recoil);
#endif // _USE_OPENVR_
					}
					else {
						Set_Gun_().Set_gun_posmat(this->Get_pos() + MATRIX_ref::Vtrans(this->gunpos, recoil), recoil);
					}
				}
				//追加
				{
					moves tmp = Set_Gun_().Get_move_gun();
					//左右の揺れ
					tmp.mat = MATRIX_ref::RotZ(this->body_ztrun) * MATRIX_ref::RotVec2(VECTOR_ref::front(), VECTOR_ref::vget(this->body_ztrun / 1.5f, this->body_xtrun * 1.5f, 1.f)) * tmp.mat;
					//左手アニメ
					Put_LeftHand(&tmp);
					//壁沿い
					{
						Set_Gun_().Set_move_gun(tmp);	//反映
						int radd = 0;
						MATRIX_ref mat_gun_old = Set_Gun_().Get_move_gun().mat;
						while (true) {
							Set_Gun_().Put_gun(true);//一時的
							if (MAPPTs->map_col_line(Set_Gun_().RIGHT_pos_gun(0), Set_Gun_().Get_maz()).HitFlag == TRUE) {
								tmp.pos = Set_Gun_().Get_move_gun().pos + mat_gun_old.zvec() * 0.006f;
								tmp.mat = MATRIX_ref::RotX(deg2rad(2)) * Set_Gun_().Get_move_gun().mat;
								Set_Gun_().Set_move_gun(tmp);//ひっこめ(仮)
							}
							else { break; }
							radd += 2;
							if (radd > 60) { break; }
						}
						easing_set(&this->rad_gun, float(radd), 0.9f);
						tmp.pos = Set_Gun_().Get_move_gun().pos;
						tmp.mat = MATRIX_ref::RotX(deg2rad(this->rad_gun)) * mat_gun_old;
						Set_Gun_().Set_move_gun(tmp);//ひっこめ
					}
					Set_Gun_().Put_gun();//一時的に銃位置更新
				}
			}
			void move_VR_Gun(void) noexcept {
				if (this->Damage.Get_alive()) {
					Set_Gun_().Set_move_gun(this->RIGHTHAND.move);
				}
			}
			//VR操作
			void vr_move(void) noexcept {
#ifdef _USE_OPENVR_
				if (DrawPts->get_hand2_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand2();
					if (ptr_->turn && ptr_->now) {
						//移動
						if ((ptr_->on[1] & BUTTON_TOUCHPAD) != 0) {
							this->speed_base = (this->isRunning() ? 8.f : 6.f) / FPS;

							if (DrawPts->tracker_num.size() > 0) {
								auto p = BodyFrameMatrix(this->obj_body, this->frame_s.bodyb_f);
								easing_set(&this->add_vec_buf, (p.zvec() * -ptr_->touch.y() + p.xvec() * -ptr_->touch.x()) * this->speed_base, 0.95f);
							}
							else {
								easing_set(&this->add_vec_buf, (this->GetHMDmat().zvec() * ptr_->touch.y() + this->GetHMDmat().xvec() * ptr_->touch.x()) * this->speed_base, 0.95f);
							}
						}
						else {
							easing_set(&this->add_vec_buf, VECTOR_ref::zero(), 0.95f);
						}
					}
				}
#endif // _USE_OPENVR_
			}
			//AI操作
			const VECTOR_ref Get_NowWaypoint() {
				//向き指定
				VECTOR_ref vec_to = MAPPTs->Get_waypoint()[this->cpu_do.wayp_pre.front()] - this->Get_Pos_Map();
				vec_to.y(0.f);
				return vec_to;
			}
			void SetNextWaypoint(const VECTOR_ref& vec_z) {
				int now = MAPPTs->Get_next_waypoint(this->cpu_do.wayp_pre, this->Get_Pos_Map(), vec_z);
				if (now != -1) {
					this->cpu_do.Set_wayp_pre(now);
				}
			}
			void SetNextWaypoint(const VECTOR_ref& vec_to, const VECTOR_ref& vec_z, float range = 0.5f) {
				//到達時に判断
				if (vec_to.size() <= range) {
					SetNextWaypoint(vec_z);
				}
			}
			bool AI_choice(const VECTOR_ref& StartPos, VECTOR_ref* vec_to, bool* is_player, int* turn) {
				bool pp = false;
				VECTOR_ref EndPos;
				for (auto& tgt : *ALL_c) {
					if (MINE_c == &tgt || tgt->isRide()) { continue; }
					const size_t index = &tgt - &ALL_c->front();
					if (!tgt->Damage.Get_alive()) { continue; }
					if (tgt->Set_Gun_().Flag_gun) { *turn = int(index); }
					EndPos = (this->cpu_do.ai_time_shoot < 0.f) ? tgt->Get_head_pos() : tgt->obj_body.frame(tgt->frame_s.bodyb_f.first);
					if (*vec_to == VECTOR_ref::zero()) { *vec_to = EndPos - StartPos; }//基準の作成
					if (MAPPTs->map_col_line(StartPos, EndPos).HitFlag == TRUE) { continue; }
					EndPos = EndPos - StartPos;
					if (vec_to->size() >= EndPos.size()) {
						*vec_to = EndPos;
						*is_player = (index == 0);
						pp = true;
					}
				}
				for (auto& tgt : *ALL_v) {
					if (MINE_v == &tgt || !tgt->isRide()) { continue; }
					const size_t index = &tgt - &ALL_v->front();
					EndPos = tgt->Get_pos() + VECTOR_ref::vget(0.f, 1.5f, 0.f);
					if (*vec_to == VECTOR_ref::zero()) { *vec_to = EndPos - StartPos; }//基準の作成
					if (!tgt->Damage.Get_alive()) { continue; }
					if (MAPPTs->map_col_line(StartPos, EndPos).HitFlag == TRUE) { continue; }
					EndPos = EndPos - StartPos;
					if (vec_to->size() >= EndPos.size()) {
						*vec_to = EndPos;
						*is_player = (index == 0);
						pp = true;
					}
				}
				return pp;
			}
			void AI_move(float& x_t, float& y_t) noexcept {
				int32_t x_m, y_m;
				//AI
				int turn = -1;
				bool is_player = false;
				//操作
				{
					this->key_.wkey = false;
					this->key_.skey = false;
					//this->akey = false;
					//this->dkey = false;
					this->key_.shoot = false;
					this->key_.running = false;
					this->key_.aim = false;
					//this->TPS = false;
					this->key_.reload = false;
					this->key_.have_item = false;
					this->key_.sort_magazine = false;
					this->key_.view_gun = false;
					this->key_.drop_ = false;
					this->key_.throw_gre = false;
					this->key_.select = false;
					this->key_.have_mag = true;
					this->key_.jamp = false;
					this->key_.squat = false;
					this->key_.ride = false;
				}
				//
				VECTOR_ref vec_to;
				//
				auto vec_x = VECTOR_ref::right();
				auto vec_y = VECTOR_ref::up();
				auto vec_z = VECTOR_ref::front();
				//狙うキャラを探索+AIのフェーズ選択
				if (isRide()) {
					auto vec_mat = (*MINE_v)->Head_direction();
					vec_x = vec_mat.xvec();
					vec_y = vec_mat.yvec();
					vec_z = vec_mat.zvec() * -1.f;
					if (!AI_choice((*MINE_v)->Head_Position(), &vec_to, &is_player, &turn)) {
						this->cpu_do.ai_phase = 3;
					}
					else if (vec_z.dot(vec_to.Norm()) >= 0 && vec_to.size() <= 300.f) {
						this->cpu_do.ai_phase = 4;
					}
				}
				else {
					auto vec_mat = this->Head_direction();
					vec_x = vec_mat.xvec();
					vec_y = vec_mat.yvec();
					vec_z = vec_mat.zvec() * -1.f;
					auto ai_phase_old = this->cpu_do.ai_phase;
					if (!AI_choice(this->Head_Position(), &vec_to, &is_player, &turn)) {
						this->cpu_do.ai_phase = 0;
					}
					else if (vec_z.dot(vec_to.Norm()) >= 0 && vec_to.size() <= 60.f) {
						this->cpu_do.ai_phase = 1;
					}
					if (isReload()) {
						this->cpu_do.ai_phase = 2;
					}
					if ((ai_phase_old == 1 && this->cpu_do.ai_phase != 1) || (this->add_vec_real.size() <= this->move.vec.size() * 0.8f)) {
						int now = MAPPTs->Get_next_waypoint(this->cpu_do.wayp_pre, this->Get_Pos_Map(), vec_z);
						if (now != -1) {
							this->cpu_do.Set_wayp_pre(now);
						}
					}
				}
				//
				if (this->key_.ekey) {
					this->cpu_do.ai_time_e += 1.f / FPS;
					if (this->cpu_do.ai_time_e >= 2) {
						this->key_.ekey = false;
						this->cpu_do.ai_time_e = 0.f;
					}
				}
				for (const auto& w : MAPPTs->Get_leanpoint_e()) {
					if ((w - this->Get_Pos_Map()).size() <= 0.5f) {
						this->key_.ekey = true;
						this->key_.qkey = false;
						this->cpu_do.ai_time_e = 0.f;
						break;
					}
				}
				if (this->key_.qkey) {
					this->cpu_do.ai_time_q += 1.f / FPS;
					if (this->cpu_do.ai_time_q >= 2) {
						this->key_.qkey = false;
						this->cpu_do.ai_time_q = 0.f;
					}
				}
				for (const auto& w : MAPPTs->Get_leanpoint_q()) {
					if ((w - this->Get_Pos_Map()).size() <= 0.5f) {
						this->key_.ekey = false;
						this->key_.qkey = true;
						this->cpu_do.ai_time_q = 0.f;
						break;
					}
				}
				switch (this->cpu_do.ai_phase) {
				case 0://通常フェイズ
				{
					this->cpu_do.ai_reload = false;

					this->key_.akey = false;
					this->key_.dkey = false;
					if (this->cpu_do.ai_time_walk < 4.f) {
						this->key_.wkey = true;
					}
					{
						if (this->cpu_do.ai_time_walk >= 6.f) {
							this->cpu_do.ai_time_walk = 0.f;
						}
						this->cpu_do.ai_time_walk += 1.f / FPS;
					}

					this->cpu_do.ai_time_shoot = 0.f;
					//向き指定
					if (turn >= 0) {
						vec_to = (*ALL_c)[turn]->Get_Pos_Map() - this->Get_Pos_Map();
					}
					else {
						vec_to = Get_NowWaypoint();
					}
					SetNextWaypoint(vec_to, vec_z);
					x_m = -int(vec_x.dot(vec_to.Norm()) * 120);
					y_m = -int(vec_y.dot(vec_to.Norm()) * 10);
				}
				break;
				case 1://戦闘フェイズ
				{
					this->cpu_do.ai_reload = false;
					int range = int(1000.f + 1000.f * vec_to.size() / 20.f);
					x_m = -int(vec_x.dot(vec_to) * 25) + int(float(-range + GetRand(range * 2)) / 100.f);
					y_m = -int(vec_y.dot(vec_to) * 25) + int(float(GetRand(range * 2)) / 100.f);

					this->cpu_do.ai_time_shoot += 1.f / FPS;
					if (this->cpu_do.ai_time_shoot < 0.f) {
						this->key_.akey = false;
						this->key_.dkey = false;
						if (this->cpu_do.ai_time_shoot >= -5) {
							if (is_player) {
								this->key_.shoot = GetRand(100) <= 8;
							}
							else {
								this->key_.shoot = GetRand(100) <= 20;
							}
							this->key_.aim = true;
							this->key_.squat = true;
						}
					}
					else {
						if (is_player) {
							this->key_.shoot = GetRand(100) <= 20;
						}
						else {
							this->key_.shoot = GetRand(100) <= 50;
						}

						if (this->cpu_do.ai_time_shoot >= GetRand(20) + 5) {
							this->cpu_do.ai_time_shoot = float(-8);
						}
						if (this->key_.ekey && !this->key_.akey) {
							this->key_.akey = true;
							this->key_.dkey = false;
							this->cpu_do.ai_time_d = 0.f;
							this->cpu_do.ai_time_a = 0.f;
						}
						if (this->key_.qkey && !this->key_.dkey) {
							this->key_.dkey = true;
							this->key_.akey = false;
							this->cpu_do.ai_time_d = 0.f;
							this->cpu_do.ai_time_a = 0.f;
						}

						if (!this->key_.akey) {
							this->cpu_do.ai_time_d += 1.f / FPS;
							if (this->cpu_do.ai_time_d > GetRand(3) + 1) {
								this->key_.akey = true;
								this->cpu_do.ai_time_d = 0.f;
							}
						}
						else {
							this->cpu_do.ai_time_a += 1.f / FPS;
							if (this->cpu_do.ai_time_a > GetRand(3) + 1) {
								this->key_.akey = false;
								this->cpu_do.ai_time_a = 0.f;
							}
						}
						this->key_.dkey = !this->key_.akey;
					}
					if (!Set_Gun_().Get_in_chamber()) {
						if (!isReload()) {
							this->key_.reload = true;
							this->cpu_do.ai_reload = true;
						}
					}
				}
				break;
				case 2://リロードフェイズ
				{
					this->key_.akey = false;
					this->key_.dkey = false;

					this->key_.wkey = true;
					this->key_.running = true;

					if (this->cpu_do.ai_reload) {
						this->cpu_do.Set_wayp_return();
						this->cpu_do.ai_reload = false;
					}
					vec_to = Get_NowWaypoint();
					SetNextWaypoint(vec_to, vec_z);
					x_m = -int(vec_x.dot(vec_to.Norm()) * 40);
					y_m = -int(vec_y.dot(vec_to.Norm()) * 40);
				}
				break;
				case 3://戦車乗車中通常フェイズ
				{
					this->key_.wkey = true;

					vec_to = Get_NowWaypoint();
					vec_z = (*MINE_v)->Get_move().mat.zvec();
					SetNextWaypoint(vec_to, vec_z, 1.5f);

					x_m = -int(vec_x.dot(vec_to.Norm()) * 40);
					y_m = -int(vec_y.dot(vec_to.Norm()) * 40);

					if (this->cpu_do.ai_tankback_cnt >= 2) {
						this->cpu_do.ai_tankback_cnt = 0;
						SetNextWaypoint(vec_z*-1.f);
					}


					float z_hyp = std::hypotf(vec_z.x(), vec_z.z());
					float a_hyp = std::hypotf(vec_to.x(), vec_to.z());
					float cost = (vec_to.z() * vec_z.x() - vec_to.x() * vec_z.z()) / (a_hyp * z_hyp);

					auto view_yrad = (atan2f(cost, sqrtf(std::abs(1.f - cost * cost)))) / 5.f; //cos取得2D

					if (this->cpu_do.ai_time_tankback_ing > 0.f) {
						this->cpu_do.ai_time_tankback_ing -= 1.f / FPS;
						this->key_.wkey = false;
						this->key_.skey = true;
						this->key_.akey = (GetRand(100) > 50);
						this->key_.dkey = !this->key_.akey;
					}
					else {
						this->cpu_do.ai_time_tankback_ing = 0.f;
						if (std::abs(view_yrad) > 0.2f) {
							this->key_.wkey = false;
							if (x_m < 0) {
								//this->key_.skey = true;
							}
						}
						if ((*MINE_v)->add_vec_real.size() <= (*MINE_v)->Get_move().vec.size() * 0.8f) {
							this->cpu_do.ai_time_tankback += 1.f / FPS;
						}
						else {
							this->cpu_do.ai_time_tankback = 0.f;
						}
						if (this->cpu_do.ai_time_tankback > 3.f) {
							this->cpu_do.ai_time_tankback = 0.f;
							this->cpu_do.ai_time_tankback_ing = 5.f;
							this->cpu_do.ai_tankback_cnt++;
						}
						if (view_yrad > 0) {
							this->key_.akey = false;
							this->key_.dkey = true;
						}
						else if (view_yrad < 0) {
							this->key_.akey = true;
							this->key_.dkey = false;
						}
					}
				}
				break;
				case 4://戦車乗車中戦闘フェイズ
				{
					this->key_.akey = (GetRand(100) > 50);
					this->key_.dkey = !this->key_.akey;
					this->key_.skey = true;
				}
				break;
				default:
					break;
				}
				x_m = std::clamp(x_m, -40, 40);
				y_m = std::clamp(y_m, -40, 40);
				x_t = float(x_m);
				y_t = float(y_m);
			}
		private:
			//操作
			void operation_common(const bool cannotmove, const float fov_per, int move_mode, float* x_m, float* y_m) noexcept {
				const bool& alive = this->Damage.Get_alive();
				if (alive) {
					switch (move_mode) {
					case 0://player move
						if (DrawPts->use_vr) {
							vr_move();//player move(VR)
						}
						else if (!isRide()) {
							Get_MouseVec(x_m, y_m, fov_per, DrawPts->disp_x, DrawPts->disp_y);//player move
						}
						break;
					case 1://AI move
						AI_move(*x_m, *y_m);
						break;
					default://cannot move
						this->key_.ReSet_();
						break;
					}
				}
				if (cannotmove || !alive) {
					this->key_.ReSet_();
					//移動
					easing_set(&this->add_vec_buf, VECTOR_ref::zero(), 0.95f);
				}
				if (DrawPts->use_vr) {
					//HMD_mat+視点取得
#ifdef _USE_OPENVR_
					this->HMD.move.repos = this->HMD.move.pos;
					this->HMD.Update_pos(DrawPts->get_hmd_num(), Flag_start_loop);
#endif // _USE_OPENVR_
				}
			}
			void operation_common2(float x_m, float y_m) noexcept {
				this->key_.rule_();
				const bool& alive = this->Damage.Get_alive();
				if (alive) {
					easing_set(&this->body_ztrun, -deg2rad(x_m * 25 / 120) * FPS / 90.f * 1.f, 0.9f);
					easing_set(&this->body_xtrun, -deg2rad(y_m * 25 / 120) * FPS / 90.f * 1.f, 0.9f);
					//z軸回転(リーン)
					if (this->key_.qkey) {
						easing_set(&this->body_zrad, deg2rad(-25), 0.9f);
					}
					else if (this->key_.ekey) {
						easing_set(&this->body_zrad, deg2rad(25), 0.9f);
					}
					else {
						easing_set(&this->body_zrad, this->body_ztrun, 0.8f);
					}
					this->HMD.mat_notlean = MATRIX_ref::RotX(-Get_xradP()) * this->HMD.mat_notlean;													//x軸回転リセット
					this->HMD.mat_notlean *= MATRIX_ref::RotY(deg2rad(x_m) * 0.1f);																	//y軸回転(旋回)
					this->HMD.mat_notlean = MATRIX_ref::RotX(Add_xradP(-deg2rad(y_m) * 0.1f, deg2rad(-80), deg2rad(60))) * this->HMD.mat_notlean;	//x軸回転(仰俯)
					//anser
					this->HMD.move.mat = this->HMD.mat_notlean * MATRIX_ref::RotAxis(this->HMD.mat_notlean.zvec(), this->body_zrad);
				}
			}
		private:
			//壁、床判定
			void wall_col(void) noexcept {
				VECTOR_ref pos_t2, pos_t;
				//VR専用
				if (DrawPts->use_vr) {
#ifdef _USE_OPENVR_
					pos_t2 = this->pos_tt + (this->HMD.Get_pos_old_noY() - this->HMD.move_start.pos);
					pos_t = this->Get_position(true);
					MAPPTs->map_col_wall(pos_t2, &pos_t);//壁
					this->pos_tt = pos_t - (this->HMD.Get_pos_noY() - this->HMD.move_start.pos);
#endif // _USE_OPENVR_
				}
				//共通
				{
					if (DrawPts->use_vr) {
#ifdef _USE_OPENVR_
						pos_t2 = this->pos_tt - this->HMD.move_start.pos;
#endif // _USE_OPENVR_
					}
					else {
						pos_t2 = this->pos_tt;
					}
					//壁
					{
						//pos_t = pos_t2 + (this->move.vec / 2);
						pos_t = pos_t2 + this->move.vec;
						MAPPTs->map_col_wall(pos_t2, &pos_t);//壁
					}
					//落下
					{
						auto ColResGround = MAPPTs->map_col_line(pos_t + (VECTOR_ref::up() * 1.8f), pos_t + (VECTOR_ref::up() * -0.05f));
						if (this->add_ypos <= 0.f && ColResGround.HitFlag == TRUE) {
							pos_t = ColResGround.HitPosition;
							this->add_ypos = 0.f;
						}
						else {
							pos_t.yadd(this->add_ypos);
							this->add_ypos += M_GR / std::powf(FPS, 2.f);
							//復帰
							if (pos_t.y() <= -5.f) {
								(*MINE_c)->Damage_Calc(0, 100, *MINE_c);
							}
						}
					}
					//反映
					if (DrawPts->use_vr) {
#ifdef _USE_OPENVR_
						this->pos_tt = pos_t + this->HMD.move_start.pos;
#endif // _USE_OPENVR_
					}
					else {
						this->pos_tt = pos_t;
					}

					this->add_vec_real = pos_t - pos_t2;
				}
			}
		private:
			//体の動作を決定
			void Set_body(bool ismine) noexcept {
				if (DrawPts->use_vr && ismine) {
#ifdef _USE_OPENVR_
					//動き以外の操作
					{
						//角度取得
						if (this->Damage.Get_alive()) {
							{
								auto v_ = this->GetHMDmat().zvec();
								if (DrawPts->tracker_num.size() > 0) {
									v_ = this->WAIST.move.mat.zvec();
								}
								float x_1 = -sinf(this->body_yrad);
								float y_1 = cosf(this->body_yrad);
								float x_2 = v_.x();
								float y_2 = -v_.z();
								this->body_yrad += std::atan2f(x_1 * y_2 - x_2 * y_1, x_1 * x_2 + y_1 * y_2) * Frame_Rate / FPS / 10.f;
							}
						}
					}
					//身体の処理
					if (this->Damage.Get_alive()) {
						//身体,頭部,腰
						MATRIX_ref m_inv;
						{
							if (DrawPts->tracker_num.size() > 0) {
								m_inv = MATRIX_ref::RotY(DX_PI_F);
								this->obj_body.SetMatrix(m_inv);
								//腰
								BodyFrameSetMatrix(this->obj_body, this->frame_s.bodyb_f, this->WAIST.move.mat * m_inv.Inverse());
								//頭部
								BodyFrameSetMatrix(this->obj_body, this->frame_s.head_f, this->HMD.Get_mat_XZ() * m_inv.Inverse() * (this->WAIST.move.mat * m_inv.Inverse()).Inverse());
							}
							else {
								m_inv = MATRIX_ref::RotY(this->body_yrad);
								this->obj_body.SetMatrix(m_inv);
								//頭部
								BodyFrameSetMatrix(this->obj_body, this->frame_s.head_f, this->HMD.Get_mat_XZ() * m_inv.Inverse());
							}
							this->obj_body.SetMatrix(m_inv * MATRIX_ref::Mtrans(this->Get_pos() - (this->obj_body.frame(this->frame_s.RIGHTeye_f.first) + (this->obj_body.frame(this->frame_s.LEFTeye_f.first) - this->obj_body.frame(this->frame_s.RIGHTeye_f.first)) * 0.5f)));
						}
						//足
						{
							//左
							if (DrawPts->tracker_num.size() > 1) {

								LEFTREG.Update_mat(DrawPts->tracker_num[1], Flag_start_loop);
								this->LEFTREG.move.mat = MATRIX_ref::RotY(deg2rad(90 + 60 - 10)) * this->LEFTREG.move_start.mat.Inverse() * this->LEFTREG.move.mat;
								this->LEFTREG.move.pos = this->LEFTREG.move.pos + this->pos_tt - this->HMD.move_start.pos;
								move_LeftLeg(this->obj_body, m_inv);
								{
									/*
									auto ColResGround = MAPPTs->map_col_line(this->obj_body.frame(this->frame_s.LEFTreg2_f.first) + (VECTOR_ref::up() * 1.8f), this->obj_body.frame(this->frame_s.LEFTreg2_f.first));
									if (ColResGround.HitFlag == TRUE) {
										this->LEFTREG.pos = VECTOR_ref(ColResGround.HitPosition) - (this->obj_body.frame(this->frame_s.LEFTreg2_f.first) - this->obj_body.frame(this->frame_s.LEFTreg_f.first));
									}
									//*/
								}
								move_LeftLeg(this->obj_body, m_inv);
							}
							//右
							if (DrawPts->tracker_num.size() > 2) {
								this->RIGHTREG.Update_mat(DrawPts->tracker_num[2], Flag_start_loop);
								this->RIGHTREG.move.mat = MATRIX_ref::RotY(deg2rad(180 - 22 - 10)) * this->RIGHTREG.move_start.mat.Inverse() * this->RIGHTREG.move.mat;
								this->RIGHTREG.move.pos += this->pos_tt - this->HMD.move_start.pos;
								move_RightLeg(this->obj_body, m_inv);
								{
									/*
									auto ColResGround = MAPPTs->map_col_line(this->obj_body.frame(this->frame_s.RIGHTreg2_f.first) + (VECTOR_ref::up() * 1.8f), this->obj_body.frame(this->frame_s.RIGHTreg2_f.first));
									if (ColResGround.HitFlag == TRUE) {
										this->RIGHTREG.pos = VECTOR_ref(ColResGround.HitPosition) - (this->obj_body.frame(this->frame_s.RIGHTreg2_f.first) - this->obj_body.frame(this->frame_s.RIGHTreg_f.first));
									}
									*/
								}
								move_RightLeg(this->obj_body, m_inv);
							}
						}
						//手+銃器
						{
							//右手+銃器の位置決め
							{
								this->RIGHTHAND.Update_none(DrawPts->get_hand1_num());
								this->RIGHTHAND.move.mat = this->RIGHTHAND.move.mat * MATRIX_ref::RotAxis(this->RIGHTHAND.move.mat.xvec(), deg2rad(-60));
								this->RIGHTHAND.move.pos += (this->pos_tt - this->HMD.move_start.pos);
								this->RIGHTHAND.move.mat = Set_Gun_().Get_res_blur(1.f) * this->RIGHTHAND.move.mat;//リコイル
								//銃器の一時的な位置指定(右手が沿うため)
								move_VR_Gun();
								Set_Gun_().Put_gun();
								this->Set_GUN_pos_Anim();
								//右手
								this->RIGHTHAND.move.pos = Set_Gun_().RIGHT_pos_gun(0);
								move_Righthand(DrawPts->tracker_num.size() > 0, m_inv);
							}
							//左手
							{
								this->LEFTHAND.Update_none(DrawPts->get_hand2_num());
								this->LEFTHAND.move.mat *= MATRIX_ref::RotAxis(this->LEFTHAND.move.mat.xvec(), deg2rad(-60));
								this->LEFTHAND.move.pos += (this->pos_tt - this->HMD.move_start.pos);
								this->LEFT_hand = (this->LEFTHAND.move.pos - this->LEFT_pos_Anim(0)).size() <= 0.1f && (!isReload() || !this->key_.have_mag);
								if (this->LEFT_hand) { this->LEFTHAND.move.pos = this->LEFT_pos_Anim(0); }
								move_Lefthand(DrawPts->tracker_num.size() > 0, m_inv);
							}
							//指
							move_Finger(Set_Gun_().Get_gunanime_trigger()->per);
						}
					}
					else {
						this->obj_body.SetMatrix(MATRIX_ref::RotY(DX_PI_F + this->body_yrad) * MATRIX_ref::Mtrans(this->Get_position(true)));
					}
#endif // _USE_OPENVR_
				}
				else {
					//動き以外の操作
					{
						//角度取得
						if (this->Damage.Get_alive()) {
							{
								auto v_ = this->mat_body.zvec();
								float x_1 = -sinf(this->body_yrad);
								float y_1 = cosf(this->body_yrad);
								float x_2 = v_.x();
								float y_2 = -v_.z();
								this->body_yrad += std::atan2f(x_1 * y_2 - x_2 * y_1, x_1 * x_2 + y_1 * y_2) * Frame_Rate / FPS / 2.5f;
							}
							{
								auto v_ = this->mat_body.zvec();
								float x_1 = sinf(this->body_xrad);
								float y_1 = -cosf(this->body_xrad);
								float x_2 = -v_.y();
								float y_2 = -std::hypotf(v_.x(), v_.z());
								this->body_xrad += std::atan2f(x_1 * y_2 - x_2 * y_1, x_1 * x_2 + y_1 * y_2);
							}
						}
						//足サウンド
						if (!this->isJamping()) {
							if (this->foot_timer == 0.f) {
								if (this->isRunning()) {
									if (abs(Head_bobbing(this->anime_run)) >= 0.8f) {
										SE.Get(EnumSound::Foot_Sound).Play_3D(0, Set_Gun_().Get_move_gun().pos, 15.f, 255);
										this->foot_timer = 0.2f;
									}
								}
								else {
									if (!this->isSquat()) {
										if (abs(Head_bobbing(this->anime_walk)) >= 0.8f) {
											SE.Get(EnumSound::Foot_Sound).Play_3D(0, Set_Gun_().Get_move_gun().pos, 5.f, 128);
											this->foot_timer = 0.2f;
										}
									}
									else {
										if (abs(Head_bobbing(this->anime_swalk)) >= 0.8f) {
											SE.Get(EnumSound::Foot_Sound).Play_3D(0, Set_Gun_().Get_move_gun().pos, 1.5f, 64);
											this->foot_timer = 0.2f;
										}
									}
								}
							}
							else {
								this->foot_timer -= 1.f / FPS;
								if (this->foot_timer <= 0.f) {
									this->foot_timer = 0.f;
								}
							}
						}
						//ソート
						if (this->sort_ing) {
							this->sorting_timer -= 1.f / FPS;
							if (this->sorting_timer <= 0.f) {
								this->sorting_timer = 0.f;
								this->sort_ing = false;
							}
						}
					}
					//身体の処理
					if (this->Damage.Get_alive()) {
						this->Flag_calc_lag = true;

						MATRIX_ref mg_inv = MATRIX_ref::RotY(DX_PI_F + this->body_yrad);
						MATRIX_ref mb_inv = MATRIX_ref::RotY(deg2rad(18)) * MATRIX_ref::RotZ(this->body_zrad);
						MATRIX_ref m_inv = MATRIX_ref::RotY(deg2rad(36)) * MATRIX_ref::RotZ(this->body_zrad) * MATRIX_ref::RotX(this->body_xrad) * mg_inv;

						this->obj_body.SetMatrix(MATRIX_ref::Mtrans(this->Get_position(true)));//

						BodyFrameSetMatrix(this->obj_body, this->frame_s.bodyg_f, mg_inv);
						BodyFrameSetMatrix(this->obj_body, this->frame_s.bodyb_f, mb_inv);
						BodyFrameSetMatrix(this->obj_body, this->frame_s.body_f, m_inv * (mb_inv * mg_inv).Inverse());
						//頭部
						BodyFrameSetMatrix(this->obj_body, this->frame_s.head_f, this->GetHMDmat() * m_inv.Inverse());
						//足
						{
							//足アニメのブレンド率
							{
								auto ratio_t = 0.f;
								if (this->isRunning()) {
									if (this->key_.wkey || this->key_.skey || this->key_.akey) {
										ratio_t = 1.f;
									}
								}
								else {
									if (this->key_.wkey || this->key_.skey || this->key_.akey || this->key_.dkey) {
										ratio_t = 1.f;
									}
								}
								easing_set(&this->ratio_move, ratio_t, 0.95f);
							}
							//
							move_Leg_Anim();
						}
						//右手+銃器の位置決め
						{
							//銃器の一時的な位置指定(右手が沿うため)
							move_nomal_gun();
							this->Set_GUN_pos_Anim();
							//右手
							this->RIGHTHAND.move.pos = Set_Gun_().RIGHT_pos_gun(0);
							this->RIGHTHAND.move.mat = Set_Gun_().Get_move_gun().mat;
							move_Righthand(false, m_inv);
						}
						//左手
						{
							this->LEFTHAND.move.pos = this->LEFT_pos_Anim(0);
							this->LEFTHAND.move.mat = Set_Gun_().Get_move_gun().mat;
							this->LEFT_hand = true;
							move_Lefthand(false, m_inv);
						}
						//指
						move_Finger(Set_Gun_().Get_gunanime_trigger()->per);
					}
				}
				//銃器
				if (this->Damage.Get_alive()) {
					//アニメ更新
					this->obj_body.work_anime();
					//obj演算
					this->Set_LEFT_pos_Anim();
					Set_Gun_().Set_Gun_Physics(this->move.vec);
				}
				//nomal限定
				if (!(DrawPts->use_vr && ismine)) {
					if (this->Damage.Get_alive()) {
						move_nomal_gun();
					}
			}
			}
		public:
			//銃発砲エフェクトのセット
			void calc_shot_effect(void) noexcept {
				Set_Gun_().calc_shot_effect(this);
			}
		public:
			using PLAYER_COMMON::PLAYER_COMMON;
			void Set(MV1& body_, MV1& bodylag_, MV1& col_) noexcept {
				//身体
				{
					//身体
					body_.DuplicateonAnime(&this->obj_body, &this->obj_body);
					//ラグドール
					bodylag_.DuplicateonAnime(&this->obj_lag, &this->obj_body);
					this->Flag_calc_lag = true;
					//身体コリジョン
					col_.DuplicateonAnime(&this->obj_col, &this->obj_body);
					PLAYER_COMMON::SetupCollInfo();
					Set_Body(this->obj_body, this->obj_lag, this->obj_col);
				}
				//変数
				{
					this->reloadf = false;
					this->LEFT_hand = false;
					this->scores.Reset(true);
					//腕アニメ
					Init_Anim();
					this->throw_gre_ings = 0;
					this->view_ings = 0;
					this->reload_ings = 0;
					//
					Damage.Set((int)this->obj_col.mesh_num());
				}
			}
			//銃を持つ
			void Add_Guninfo(std::vector<std::shared_ptr<PLAYERclass::GunControl>>& Gun_S, const std::shared_ptr<GunPartsControl>& GunPartses) {
				Gun_S.emplace_back(std::make_shared<PLAYERclass::GunControl>());
				this->Gun_Ptr = Gun_S.back();
				this->gun_stat.emplace_back(GunControl::GUN_STATUS());				//gunstat
				Gun_S.back()->Set_Gun(GunPartses->Get_Parts_Data(EnumGunParts::GUN), 0, this->gun_stat.back());
			}
			//銃を捨てる
			void Dispose_GunInfo() {
				if (this->Gun_Ptr != nullptr) {
					this->Gun_Ptr->Set_gun_stat_now(nullptr);
					this->Gun_Ptr.reset();
					this->Gun_Ptr = nullptr;
				}
			}
			//弾
			void Set_bullet_Ptr(void) noexcept {
				Set_Gun_().SetUp_bullet_Ptr(MINE_c, (*MINE_c)->MINE_v);
			}
			/*キャラスポーン*/
			void Spawn(bool isrespawn, bool ismine, std::vector<std::shared_ptr<PLAYERclass::GunControl>>& Gun_S, const std::shared_ptr<GunPartsControl>& GunPartses) noexcept {
				PLAYER_COMMON::Spawn_Common();
				//
				this->pos_tt = this->spawn.pos;
				this->HMD.move.mat = this->spawn.mat;
				this->HMD.mat_notlean = this->HMD.move.mat;

				this->key_.ReSet_();

				this->add_ypos = 0.f;
				this->add_vec_buf.clear();
				this->Flag_calc_body = true;
				this->Flag_calc_lag = true;

				this->scores.Reset(false);

				this->body_xrad = 0.f;//胴体角度
				this->body_yrad = 0.f;//胴体角度
				this->body_zrad = 0.f;//胴体角度

				if (isrespawn) {
					Add_Guninfo(Gun_S, GunPartses);
				}
				auto& g = this->Set_Gun_();
				//敵かリスポーンなら
				if(!ismine || isrespawn){
					g.Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::MAGAZINE)[0], EnumGunParts::MAGAZINE);
					g.Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::GRIP)[0], EnumGunParts::GRIP, g.Get_parts(EnumGunParts::BASE), EnumAttachPoint::GRIP_BASE);
					g.Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::UPER_HANDGUARD)[0], EnumGunParts::UPER_HANDGUARD, g.Get_parts(EnumGunParts::BASE), EnumAttachPoint::UPER_HANDGUARD);
					g.Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::UNDER_HANDGUARD)[0], EnumGunParts::UNDER_HANDGUARD, g.Get_parts(EnumGunParts::BASE), EnumAttachPoint::UNDER_HANDGUARD);
					g.Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::MOUNT_BASE)[0], EnumGunParts::MOUNT_BASE, g.Get_parts(EnumGunParts::BASE), EnumAttachPoint::SIDEMOUNT_BASE);
					g.Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::MOUNT)[0], EnumGunParts::MOUNT, g.Get_parts(EnumGunParts::MOUNT_BASE), EnumAttachPoint::SIDEMOUNT);
					g.Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::SIGHT)[0], EnumGunParts::SIGHT, g.Get_parts(EnumGunParts::MOUNT), EnumAttachPoint::UPER_RAIL, 1);
					g.Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::MAZZULE)[0], EnumGunParts::MAZZULE, g.Get_parts(EnumGunParts::BASE), EnumAttachPoint::MAZZULE_BASE);
					g.Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::DUSTCOVER)[0], EnumGunParts::DUSTCOVER, g.Get_parts(EnumGunParts::BASE), EnumAttachPoint::DUSTCOVER_BASE);
					g.Attach_parts(&GunPartses->Get_Parts_Data(EnumGunParts::STOCK)[0], EnumGunParts::STOCK, g.Get_parts(EnumGunParts::BASE), EnumAttachPoint::STOCK_BASE);
				}
				//装備パターンの保存、読み出し
				{

				}
				g.Init();
				g.SetUp_bullet(MAPPTs, DrawPts);
				this->Set_bullet_Ptr();
				//g.Set_gunanime_shot(1.f);
				Init_Left();
				this->nearhit = false;
				//AIの選択をリセット
				int now = MAPPTs->Get_next_waypoint(this->cpu_do.wayp_pre, this->Get_Pos_Map());
				this->cpu_do.Spawn((now != -1) ? now : 0);
			}
			/*最初*/
			void Start(void) noexcept override {
				PLAYER_COMMON::Start();
#ifdef _USE_OPENVR_
				this->HMD.Set(DrawPts);
				this->WAIST.Set(DrawPts);
				this->LEFTREG.Set(DrawPts);
				this->RIGHTREG.Set(DrawPts);
				this->RIGHTHAND.Set(DrawPts);							//右手座標系
				this->LEFTHAND.Set(DrawPts);							//左手座標系
#endif // _USE_OPENVR_
				this->Flag_start_loop = true;
				this->ratio_move = 0.f;
			}
			//判定起動
			const bool Set_ref_col(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos, float range) noexcept override {
				//すでに起動しているなら無視
				if (this->nearhit) { return true; }
				if (Segment_Point_MinLen(StartPos, EndPos, this->Get_pos()) <= range) {
					//判定起動
					this->nearhit = true;
					HumanControl::Frame_Copy_Col(this->obj_body, &this->obj_col);
					for (int i = 0; i < this->obj_col.mesh_num(); ++i) {
						this->obj_col.RefreshCollInfo(-1, i);
					}
					return true;
				}
				return false;
			}
			/*更新*/
			void Update(
				const bool playing, const float fov_per, std::vector <Meds>& meds_data, std::vector<Grenades>& gres_data, bool ismine,
				std::vector<std::shared_ptr<PLAYERclass::GunControl>>& Gun_S, const std::shared_ptr<GunPartsControl>& GunPartses
			) noexcept {
				this->chage_ride = false;
				this->nearhit = false;
				{
					Damage.Update();
					//

					if (!this->Damage.Get_alive()) {
						this->HMD.move.pos.clear();
						HumanControl::Anim_Reset();
						if (this->scores.start_death()) {
							HumanControl::Frame_Reset(this->obj_body);	//
							magrelease_t(0);							//マガジン全排出
							Set_Gun_().Start_Gun_Physics();				//死んだ歳の物理スタートフラグ
							Dispose_GunInfo();							//銃を捨てる	
						}
						if (!this->scores.death_f) {
							this->Spawn(true, false, Gun_S, GunPartses);
						}
					}
					this->scores.Update();
				}
				//座標取得
				if (ismine) {
#ifdef _USE_OPENVR_
					if (DrawPts->tracker_num.size() > 0) {
						this->WAIST.Update_pos(DrawPts->tracker_num[0], Flag_start_loop);
						this->WAIST.move.pos = this->WAIST.move.pos - this->WAIST.move_start.pos;
					}
#endif // _USE_OPENVR_
				}
				//プレイヤー操作
				{
					float x_m = 0.f, y_m = 0.f;
					operation_common(!playing, fov_per, ismine ? 0 : 1, &x_m, &y_m);
					if (isRide()) {
						operation_common2(x_m, y_m);
					}
					else {
						if (!DrawPts->use_vr) {
							operation_common2(x_m, y_m);
							this->speed_base = (this->isRunning() ? 6.f : ((this->is_ADS() ? 2.f : 4.f) * (this->isSquat() ? 0.7f : 1.f))) / FPS;
							auto zv_t = this->GetHMDmat().zvec();
							zv_t.y(0.f);
							zv_t = zv_t.Norm();
							auto xv_t = this->GetHMDmat().xvec();
							xv_t.y(0.f);
							xv_t = xv_t.Norm();
							if (this->isRunning()) {
								xv_t = xv_t.Norm() * 0.5f;
							}
							easing_set(&this->add_vec_buf,
								VECTOR_ref(VECTOR_ref::zero())
								+ (this->key_.wkey ? (zv_t * -this->speed_base) : VECTOR_ref::zero())
								+ (this->key_.skey ? (zv_t * this->speed_base) : VECTOR_ref::zero())
								+ (this->key_.akey ? (xv_t * this->speed_base) : VECTOR_ref::zero())
								+ (this->key_.dkey ? (xv_t * -this->speed_base) : VECTOR_ref::zero())
								, 0.95f);
							this->mat_body = this->GetHMDmat();
						}
						//ジャンプ
						{
							if (!this->isJamping()) {
								if (this->key_.jamp && this->Damage.Get_alive()) {
									this->add_ypos = 0.05f * Frame_Rate / FPS;
								}
								this->move.vec = this->add_vec_buf;
							}
							else {
								easing_set(&this->move.vec, VECTOR_ref::zero(), 0.995f);
							}
						}
						//操作
						if (this->Damage.Get_alive()) {
							if (isReload()) {
								this->key_.aim = false;
							}
							Set_Gun_().Set_gunanime_trigger(float(this->key_.shoot && !this->isRunning()));	//引き金(左クリック)
							Set_Gun_().Set_gunanime_magcatch(float(this->key_.reload));						//マグキャッチ(Rキー)
						}
						//壁その他の判定
						this->wall_col();
						this->Set_body(ismine);
						//lag演算
						//if (!this->Damage.Get_alive()) {
						HumanControl::Frame_Copy_Lag(this->obj_body, &this->obj_lag);
						this->obj_lag.work_anime();
						//}
						//obj_col演算
						if (this->Damage.Get_alive()) {
							this->obj_col.work_anime();
						}
					}
				}
				{
					//乗り降り
					this->canride_f = false;
					if (!this->isRide()) {
						for (auto& v : *ALL_v) {
							if (((this->Get_pos() - v->Get_pos()).size() < 3.f) && !v->isRide() && v->Damage.Get_alive()) {
								this->canride_f = true;
								if (this->key_.ride) {
									this->Ride_on(&v);
									break;
								}
							}
						}
					}
					else {
						if (this->key_.ride) { this->Ride_on(nullptr); }
					}
				}
				//
				if (!(DrawPts->use_vr && ismine)) {
					Set_HMDpos();								//視点取得
					//銃器
					if (this->Damage.Get_alive()) {
						Set_Gun_().Put_gun();
						if (isReload()) {
							auto ans1_ = this->LEFT_pos_Anim(0);
							auto ans2_ = this->LEFT_pos_Anim(1);
							auto ans3_ = this->LEFT_pos_Anim(2);
							Set_Gun_().Set_Magazine(ans1_, Set_Gun_().Get_move_gun().mat);
							VECTOR_ref ans1_t = ans1_;
							if (Set_Gun_().Get_gun_stat_now()->hav_mag()) {
								ans1_t += ans1_ - Set_Gun_().Get_mag_in().front().Get_L_mag_pos();
							}
							Set_Gun_().Set_Magazine(ans1_t, MATRIX_ref::Axis1_YZ((ans3_ - ans1_).Norm(), (ans2_ - ans1_).Norm() * -1.f));
						}
					}
				}
				//
				if (!isRide()) {
					//射撃関連
					if(this->Damage.Get_alive()){
						Set_Gun_().Calc_Gunanime(Set_Gun_().Audio.use_slide);
						//マガジン排出(通常)
						if (this->key_.reload && (!this->reloadf && Set_Gun_().Get_gun_stat_now()->hav_mag())) {
							this->reloadf = true;
							magrelease_t(1);
						}
						//medkit生成
						if (this->key_.drop_) {
							release(&meds_data[0]);
						}
						//gre
						if (this->key_.throw_gre) {
							this->throw_grenades = true;
						}

						if ((this->throw_grenade_old != this->throw_grenades) && !this->throw_grenades) {
							release(&gres_data[0]);
						}
						this->throw_grenade_old = this->throw_grenades;
						//眺める
						if (!this->view_ing && this->key_.view_gun) {
							this->view_ing = true;
						}
						//マガジン整頓
						if (!this->sort_ing && this->key_.sort_magazine && Set_Gun_().Get_gun_stat_now()->hav_mags()) {
							this->sort_ing = true;
							if (!this->sort_f) {
								this->sort_f = true;
								this->sorting_timer = 1.f;
								SE.Get(EnumSound::Sort_MAG).Play_3D(0, Set_Gun_().Get_move_gun().pos, 15.f);
								Set_Gun_().Get_gun_stat_now()->sort_magazine();
							}
							else {
								this->sorting_timer = 3.f;
								SE.Get(EnumSound::Cate_Load).Play_3D(0, Set_Gun_().Get_move_gun().pos, 15.f);
								Set_Gun_().Get_gun_stat_now()->load_magazine();
								//マガジン輩出(空)
								if (Set_Gun_().Get_mag_in().back().Get_Cnt() == 0) {
									magrelease_t(2);
								}
							}
						}
						//マガジン挿入
						if (isReload()) {
							if (DrawPts->use_vr && this->reload_cnt < Set_Gun_().Get_reloadtime()) {
								this->key_.have_mag = false;
							}
							if (this->key_.have_mag) {
								if (Set_Gun_().Get_gun_stat_now()->hav_mag()) {
									auto mag0 = Set_Gun_().Get_mag_in().front().Get_magazine_f(0).first;
									auto mag1 = Set_Gun_().Get_mag_in().front().Get_magazine_f(1).first;
									if (
										(DrawPts->use_vr) ?
										((Set_Gun_().Get_mag_in().front().Get_mag_frame(mag1) - Set_Gun_().Get_magf_pos()).size() <= 0.1f) :
										(this->reload_cnt > Set_Gun_().Get_reloadtime())
										) {
										Set_Gun_().Load_Mag();
										this->reloadf = false;
									}
									if (DrawPts->use_vr) {
										Set_Gun_().Set_Magazine(
											this->LEFTHAND.move.pos,
											Set_Gun_().Get_mag_in().front().Get_mag_frameLocalMatrix(mag1) * Set_Gun_().Get_mag_in().front().Get_mag_frameLocalMatrix(mag0) *
											(this->LEFTHAND.move.mat * MATRIX_ref::RotVec2(this->LEFTHAND.move.mat.yvec(), Set_Gun_().Get_mag2f_pos() - this->LEFTHAND.move.pos))
										);
									}
								}
								this->reload_cnt += 1.f / FPS;//挿入までのカウント
							}
						}
						else {
							this->key_.have_mag = false;
							Set_Gun_().Set_Magazine();
						}
						Set_Gun_().Set_select_anime();						//セレクター
						Set_Gun_().Set_selecter(this->key_.select);
						//射撃
						if (Set_Gun_().Calc((this->LEFT_hand ? 3.f : 1.f) * (this->isSquat() ? 1.75f : 1.0f), isReload(), this->view_ing)) {
							//排莢、弾
							Set_Gun_().Create_Cart();
							Set_Gun_().Create_bullet();
							Light_pool.Put(Set_Gun_().Get_maz());
							//エフェクト
							calc_shot_effect();
							//サウンド
							SE.Get(EnumSound::Shot).Play_3D(Set_Gun_().Audio.use_shot, Set_Gun_().Get_move_gun().pos, 200.f, (Set_Gun_().Get_mazzuletype() == 1) ? 192 : 255);
						}
						Set_Gun_().Update_bullet();						//弾の処理
						Set_Gun_().Update_Gun(MAPPTs, 1.f);				//銃の処理
						Set_Gun_().Set_LightHandle();					//ライト
						Set_Gun_().IsDrawmagazine = !isReload() || this->have_magazine;//マガジンの表示を行うか否か
					}
					//アイテム拾う
					MAPPTs->Get_item(this->Get_pos_LEFTHAND(), this->Get_pos_LEFTHAND() - this->Get_mat_LEFTHAND().zvec() * 2.6f, *MINE_c);
					//物理演算、アニメーション
					{
						this->Calc_Physics();
						if (this->Flag_calc_lag) {
							this->obj_lag.PhysicsResetState();
							this->Flag_calc_lag = false;
						}
						else {
							this->obj_lag.PhysicsCalculation(1000.f / FPS);
						}
					}
					if (ismine) {
						//息
						if (this->breath_timer == 0.f) {
							if (this->isRunning()) {
								VOICE.Get(EnumSound::Voice_Breath_Run).Play_3D(0, this->Get_pos(), 15.f, 163);
								this->breath_timer = 0.5f;
							}
							else {
								VOICE.Get(EnumSound::Voice_Breath).Play_3D(0, this->Get_pos(), 10.f, 128);
								this->breath_timer = 1.2f;
							}
						}
						else {
							this->breath_timer -= 1.f / FPS;
							if (this->breath_timer <= 0.f) {
								this->breath_timer = 0.f;
							}
						}
					}
				}
				//エフェクトの処理
				Update_effect();
			}
			/*視界外か否かを判断*/
			void Check_CameraViewClip(bool use_occlusion) noexcept {
				if (isRide()) {
					this->Flag_canlook_player = false;
					return;
				}
				this->Flag_canlook_player = true;
				auto ttt = this->Get_position(true);

				if (this->Damage.Get_alive()) {
					this->Set_Gun_().distance_to_cam = (ttt - GetCameraPosition()).size();
				}
				if (CheckCameraViewClip_Box((ttt + VECTOR_ref::vget(-1.8f, 0, -1.8f)).get(), (ttt + VECTOR_ref::vget(1.8f, 1.8f, 1.8f)).get())) {
					this->Flag_canlook_player = false;
					this->Flag_calc_body = true;
					if (this->Damage.Get_alive()) {
						this->Set_Gun_().distance_to_cam = -1.f;
					}
					return;
				}
				if (use_occlusion) {
					if (MAPPTs->map_col_line(GetCameraPosition(), ttt + (VECTOR_ref::up() * 1.8f)).HitFlag == TRUE &&
						MAPPTs->map_col_line(GetCameraPosition(), ttt + VECTOR_ref::zero()).HitFlag == TRUE) {
						this->Flag_canlook_player = false;
						this->Flag_calc_body = true;
						if (this->Damage.Get_alive()) {
							this->Set_Gun_().distance_to_cam = -1.f;
						}
						return;
					}
				}
			}
			/*カメラ指定*/
			void Set_cam(cam_info& camera_main, const float fov_base) noexcept {
				if (this->Damage.Get_alive()) {
					auto mat_T = Set_Gun_().Get_res_blur(0.7f) * this->GetHMDmat();//リコイル

					auto ppsh = MATRIX_ref::Vtrans(VECTOR_ref::right() * -0.035f, mat_T);
					if (DrawPts->use_vr) {
						mat_T = this->GetHMDmat();
						ppsh.clear();
					}
					camera_main.set_cam_pos(this->Get_pos() + ppsh, this->Get_pos() + ppsh + mat_T.zvec() * (DrawPts->use_vr ? 1.f : -1.f), mat_T.yvec());
					camera_main.near_ = 0.1f;
					if (this->is_ADS()) {
						
						easing_set(&camera_main.fov, deg2rad(25), std::min(Set_Gun_().Get_Per_Sub(0.8f, 0.9f), 0.925f));
					}
					else {
						easing_set(&camera_main.fov, fov_base, 0.9f);
					}
				}
				else {
					//デスカメラ
					if (this->scores.death_id < ALL_c->size()) {
						easing_set(&camera_main.camvec, (*ALL_c)[this->scores.death_id]->Get_pos(), 0.9f);
					}
					auto rad = atan2f((camera_main.camvec - camera_main.campos).x(), (camera_main.camvec - camera_main.campos).z());
					easing_set(&camera_main.campos, this->Get_Pos_Map() + VECTOR_ref::vget(-5.f * sin(rad), 2.f, -5.f * cos(rad)), 0.9f);
					camera_main.camup = VECTOR_ref::up();
					MAPPTs->map_col_nearest(camera_main.camvec, &camera_main.campos);
					easing_set(&camera_main.fov, fov_base, 0.9f);
				}
			}
			void Setcamera(cam_info& camera_main, const float fov_base) noexcept {
				if (!isRide()) {
					this->Set_cam(camera_main, fov_base);
				}
				else {
					(*this->MINE_v)->Set_cam(camera_main, fov_base);
				}
			}
			/*キャラ+銃描画*/
			void Draw_chara(void) noexcept {
				if (this->Flag_canlook_player) {
					if (this->Damage.Get_alive()) {
						this->obj_body.DrawModel();
						//this->obj_col.DrawModel();
					}
					else {
						this->obj_lag.DrawModel();
					}
				}
				if (MINE_c == &(*ALL_c)[1]) {
					//cpu_do.Draw_Debug(MAPPTs);
				}
			}
			/*レーザー、ライト描画*/
			void Draw_LAM_Effect(void) noexcept {
				if (Gun_Ptr != nullptr) {
					if (Set_Gun_().Get_lamtype() == EnumSELECT_LAM::LASER) {
						Draw_Lazer_Effect(Set_Gun_().Get_source_pos(), Set_Gun_().Get_move_gun().mat.zvec() * -1.f, true, 0.05f);
					}
				}
			}
			/*HPバーを表示する場所*/
			const auto Set_HP_UI(void) noexcept {
				return Set_HP_UI_Common(this->Get_head_pos());
			}
			/*おわり*/
			void Dispose(void) noexcept override {
				PLAYER_COMMON::Reset();
				PLAYER_COMMON::Dispose();
				this->gun_stat.clear();
				Dispose_GunInfo();
			}
		};
		//戦車
		class PLAYER_VEHICLE : public PLAYER_COMMON {
		private:
			class Guns : public BulletControl_Common {
			private:
				moves Gun_move;
				std::shared_ptr<PLAYER_VEHICLE>* MINE_v{ nullptr };	/**/
				float loadcnt{ 0 };								/*装てんカウンター*/
				bool reload_se_f{ true };
				float fired{ 0.f };								/*駐退*/
				float firereact{ 0.f };							/*反動*/
				int rounds{ 0 };								/*弾数*/
				gun_frame gun_info;								/**/
				std::vector<Ammos> Spec;						/**/
				float xrad = 0.f, yrad = 0.f;
				float xrad_shot = 0.f;														//射撃反動x
				float zrad_shot = 0.f;														//射撃反動z
			private:
				bool Calc_Gun(bool key) {
					bool ans = (key && this->loadcnt == 0 && this->rounds > 0);
					if (ans) {
						this->loadcnt = this->gun_info.Get_load_time();
						this->reload_se_f = true;
						this->rounds = std::max<int>(this->rounds - 1, 0);
						this->fired = 1.f;
						this->firereact = std::clamp(this->firereact + this->Spec[0].Get_caliber() * 10.f, 0.f, 3.f);
					}
					if (this->reload_se_f && this->loadcnt < 1.f) {
						this->reload_se_f = false;
						//サウンド
						SE.Get(EnumSound::Tank_Reload).Play_3D((*MINE_v)->use_veh->Reload_ID, (*MINE_v)->Get_pos(), 3.f, 128);
					}
					this->loadcnt = std::max(this->loadcnt - 1.f / FPS, 0.f);
					this->fired = std::max(this->fired - 1.f / FPS, 0.f);
					this->firereact = std::max(this->firereact - 1.f / FPS, 0.f);
					return ans;
				}
				//弾の生成
				void Create_bullet(Ammos* spec_t, const VECTOR_ref& pos_t) noexcept {
					BulletControl_Common::Create_bullet(spec_t, pos_t, Gun_move.mat.zvec() * -1.f);
				}
			public:
				const auto& Getrounds(void) const noexcept { return rounds; }
				const auto& Getloadtime(void) const noexcept { return loadcnt; }
				const auto& Getgun_info(void) const noexcept { return gun_info; }
				//銃反動
				void FireReaction(MATRIX_ref* mat_t) noexcept {
					auto firereact_t = deg2rad(-this->firereact * this->Spec[0].Get_caliber() * 50.f);
					easing_set(&this->xrad_shot, firereact_t * cos(this->yrad), 0.85f);
					easing_set(&this->zrad_shot, firereact_t * sin(this->yrad), 0.85f);
					(*mat_t) *= MATRIX_ref::RotAxis(mat_t->xvec(), -this->xrad_shot) * MATRIX_ref::RotAxis(mat_t->zvec(), this->zrad_shot);
				}
			public:
				//弾
				void Set_bullet_Ptr(void) noexcept {
					SetUp_bullet_Ptr((*MINE_v)->MINE_c, MINE_v);
				}
				//角度指示
				void SetGunRad(const float view_xrad_t, const float view_yrad_t, float limit) {
					this->yrad += std::clamp(view_yrad_t, -limit, limit); //this->yrad = std::clamp(this->yrad + std::clamp(view_yrad_t, -limit, limit),deg2rad(-30.0)+this->yrad,deg2rad(30.0)+this->yrad);//射界制限
					this->xrad = std::clamp(this->xrad + std::clamp(view_xrad_t, -limit, limit), deg2rad(-10), deg2rad(20));
				}
				//銃発砲エフェクトのセット
				void calc_shot_effect(const VECTOR_ref& pos_t) noexcept {
					(*MINE_v)->Set_Effect(Effect::ef_fire, pos_t, Gun_move.mat.zvec() * -1.f, this->Spec[0].Get_caliber() / 0.1f);//ノーマル
				}
				//
				void Set_Frame(MV1* obj_body_t)const noexcept {
					BodyFrameSetMatrix(*obj_body_t, this->gun_info.Get_frame(0), MATRIX_ref::RotY(this->yrad));
					BodyFrameSetMatrix(*obj_body_t, this->gun_info.Get_frame(1), MATRIX_ref::RotX(this->xrad));
					BodyFrameSetMatrix(*obj_body_t, this->gun_info.Get_frame(2), MATRIX_ref::Mtrans(VECTOR_ref::front() * (this->fired * 0.5f)));
				}
				//
				Guns(void) noexcept { }
				Guns(const Guns&) { }
				//
				void Set(const gun_frame& resorce, std::shared_ptr<PLAYER_VEHICLE>* MINE_v_t) {
					MINE_v = MINE_v_t;
					this->gun_info = resorce;
					this->rounds = this->gun_info.Get_Ammo_Cap();
					//使用砲弾
					this->Spec = this->gun_info.Get_Spec();
				}
				//銃演算
				void Update(bool key, const VECTOR_ref& pos_t, const MATRIX_ref& mat_t) {
					Gun_move.mat = mat_t;
					//射撃
					if (Calc_Gun(key)) {
						//排莢、弾
						//GunControl::Create_Cart();
						Create_bullet(&this->Spec[0], pos_t);
						//エフェクト
						calc_shot_effect(pos_t);
						//サウンド
						SE.Get(EnumSound::Tank_Shot).Play_3D(this->gun_info.Get_sound(), (*MINE_v)->Get_pos(), 250.f, 128);
					}
					//弾の処理
					BulletControl_Common::Update_bullet();
				}
				/*おわり*/
				void Dispose(void) noexcept {
					BulletControl_Common::Dispose();
					MINE_v = nullptr;
				}
				void Reset(void) noexcept {
					this->fired = 0.f;
					this->loadcnt = 0.f;
					this->reload_se_f = true;
					this->rounds = 0;
					this->Spec.clear();
				}
			};
			class foot_frame {
			public:
				frames frame;
				float will_y = 0.f;
			};
			class cat_frame {
			public:
				frames frame;
				float will_y = 0.f;
				MV1_COLL_RESULT_POLY colres{};
				EffectS gndsmkeffcs;
				float gndsmksize = 1.f;

				cat_frame(void) noexcept {
					frame.first = -1;
					will_y = 0.f;
					gndsmksize = 1.f;
				}
				cat_frame(const cat_frame& tgt) {
					frame = tgt.frame;
					will_y = 0.f;
					gndsmksize = 1.f;
				}
				void operator=(const cat_frame& tgt) {
					frame = tgt.frame;
				}
			};
			//履帯BOX2D
			class FootWorld {
			public:
				std::shared_ptr<b2World> world{ nullptr };		/*足world*/
				b2RevoluteJointDef f_jointDef;					/*ジョイント*/
				std::vector<b2Pats> Foot, Wheel, Yudo;			/**/
			};
		private:
			float wheel_Left = 0.f, wheel_Right = 0.f;									//転輪回転
			std::vector<Guns> Gun_;														/**/
			float speed{ 0.f };															//移動速度
			float speed_add = 0.f, speed_sub = 0.f;										/**/
			float xrad = 0.f, xradadd = 0.f, xradadd_left = 0.f, xradadd_right = 0.f;	/**/
			float yrad = 0.f, yradadd = 0.f, yradadd_left = 0.f, yradadd_right = 0.f;	/**/
			float zrad = 0.f, zradadd = 0.f, zradadd_left = 0.f, zradadd_right = 0.f;	/**/
			b2Pats b2mine;																/*box2d*/
			float spd_buf = 0.f;														/*box2d*/
			float wheel_Leftadd = 0.f, wheel_Rightadd = 0.f;							//転輪回転
			VECTOR_ref wheel_normal{ VECTOR_ref::up() };								/**/
			std::array<std::vector<cat_frame>, 2> b2downsideframe;						/*履帯*/
			std::vector<foot_frame> wheelframe;											/**/
			std::array<FootWorld, 2> foot;												//履帯BOX2D
			float spd_rec;																/**/
			float view_xrad = 0.f;														/**/
			float view_yrad = 0.f;														/**/
			std::array<bool, 6> key{ false };											//キー
			float engine_time = 0.f;													/*エンジン音声*/
			float range = 6.0f;															/**/
			float range_r = range;														/**/
			float range_change = range / 10.f;											/**/
			float ratio = 1.f;															/**/
			bool changeview = false;													/**/
			MATRIX_ref lookvec;															/**/
			HIT_ACTIVE Hit_active;														/**/
		public:
			VEHICLE_HitControl hitControl;
			Vehcs* use_veh;																/*固有値*/
		private:
			void Set_Tank() {
				VECTOR_ref minmaxsub = this->use_veh->Get_maxpos() - this->use_veh->Get_minpos();
				VECTOR_ref minmaxadd = this->use_veh->Get_minpos() + this->use_veh->Get_maxpos();
				b2PolygonShape dynamicBox;
				dynamicBox.SetAsBox(minmaxsub.x() / 2, minmaxsub.z() / 2, b2Vec2(minmaxadd.x() / 2, -minmaxadd.z() / 2), 0.f);	/*ダイナミックボディに別のボックスシェイプを定義します。*/
				this->b2mine.Set(CreateB2Body(MAPPTs->world, b2_dynamicBody, move.pos.x(), move.pos.z(), atan2f(-move.mat.zvec().x(), -move.mat.zvec().z())), &dynamicBox);	/*シェイプをボディに追加*/
				for (auto& f : foot) {
					f.world = std::make_unique<b2World>(b2Vec2(0.0f, 0.0f));
					float LorR = ((&f == &foot.front()) ? 1.f : -1.f);
					//履帯
					f.Foot.clear();
					{
						b2BodyDef bd;
						b2Body* BodyB = f.world->CreateBody(&bd);
						b2EdgeShape shape;
						shape.Set(b2Vec2(-40.0f, -10.0f), b2Vec2(40.0f, -10.0f));
						BodyB->CreateFixture(&shape, 0.0f);
						b2Body* BodyA = BodyB;
						b2Vec2 anchor;
						for (const auto& w : this->use_veh->Get_b2upsideframe(&f - &foot.front())) {
							anchor = b2Vec2(w.second.z(), w.second.y());
							if (w.second.x() * LorR > 0) {
								f.Foot.resize(f.Foot.size() + 1);
								b2PolygonShape f_dynamicBox; /*ダイナミックボディに別のボックスシェイプを定義します。*/
								f_dynamicBox.SetAsBox(0.2f, 0.05f);
								f.Foot.back().Set(CreateB2Body(f.world, b2_dynamicBody, anchor.x, anchor.y), &f_dynamicBox);
								f.f_jointDef.Initialize(BodyA, f.Foot.back().Get(), anchor);
								f.world->CreateJoint(&f.f_jointDef);
								BodyA = f.Foot.back().Get();
							}
						}
						if (!f.Foot.empty()) {
							f.f_jointDef.Initialize(BodyA, BodyB, anchor);
							f.world->CreateJoint(&f.f_jointDef);
						}
					}
					for (const auto& w : this->use_veh->Get_b2downsideframe()) {
						auto& w2 = this->b2downsideframe[&w - &this->use_veh->Get_b2downsideframe().front()];
						w2.resize(w.size());
						for (auto& t : w2) {
							t.frame = w[&t - &w2.front()];
							t.gndsmkeffcs.set_loop(effectControl.effsorce.back());
							t.gndsmkeffcs.put_loop(VECTOR_ref::vget(0, -1, 0), VECTOR_ref::vget(0, 0, 1), 0.1f);
							t.gndsmksize = 0.1f;
						}
					}
					//転輪
					f.Wheel.clear();
					f.Yudo.clear();
					if (!f.Foot.empty()) {
						//転輪(動く)
						for (const auto& w : this->use_veh->Get_wheelframe()) {
							this->wheelframe.resize(this->wheelframe.size() + 1);
							this->wheelframe.back().frame = w;
							VECTOR_ref vects = this->obj_body.GetFrameLocalMatrix(w.first).pos();
							if (vects.x() * LorR > 0) {
								f.Wheel.resize(f.Wheel.size() + 1);
								b2CircleShape shape;
								shape.m_radius = vects.y() - 0.1f;
								f.Wheel.back().Set(CreateB2Body(f.world, b2_kinematicBody, vects.z(), vects.y()), &shape);
							}
						}
						//誘導輪(動かない)
						for (const auto& w : this->use_veh->Get_wheelframe_nospring()) {
							VECTOR_ref vects = this->obj_body.GetFrameLocalMatrix(w.first).pos();
							if (vects.x() * LorR > 0) {
								f.Yudo.resize(f.Yudo.size() + 1);
								b2CircleShape shape;
								shape.m_radius = 0.05f;
								f.Yudo.back().Set(CreateB2Body(f.world, b2_kinematicBody, vects.z(), vects.y()), &shape);
							}
						}
					}
				}

				//砲
				this->Gun_.resize(this->use_veh->Get_gunframe().size());
				for (auto& g : this->Gun_) { g.Set(this->use_veh->Get_gunframe(&g - &this->Gun_.front()), MINE_v); }
			}
			void Update_Tank() {
				bool isfloat = (this->Get_pos().y() == -this->use_veh->Get_down_in_water());
				this->spd_rec = this->speed;
				//砲塔旋回
				{
					float limit = this->use_veh->Get_turret_rad_limit() / FPS;
					for (auto& g : this->Gun_) {
						//角度指示
						g.SetGunRad(this->view_xrad, this->view_yrad, limit);
						//反映
						g.Set_Frame(&this->obj_body);
						g.Set_Frame(&this->obj_col);
					}
				}
				{
					auto y_vec = move.mat.yvec();
					for (auto& f : this->wheelframe) {
						MATRIX_ref tmp;
						this->obj_body.frame_Reset(f.frame.first);
						auto startpos = this->obj_body.frame(f.frame.first);
						auto ColResGround = MAPPTs->map_col_line(startpos + y_vec * ((-f.frame.second.y()) + 2.f), startpos + y_vec * ((-f.frame.second.y()) - 0.3f));

						easing_set(&f.will_y, (ColResGround.HitFlag == TRUE) ? (ColResGround.HitPosition.y + y_vec.y() * f.frame.second.y() - startpos.y()) : -0.3f, 0.9f);
						tmp = MATRIX_ref::Mtrans(VECTOR_ref::up() * f.will_y);

						BodyFrameSetMatrix(this->obj_body, f.frame, MATRIX_ref::RotX((f.frame.second.x() >= 0) ? this->wheel_Left : this->wheel_Right) * tmp);
					}
					for (const auto& f : this->use_veh->Get_wheelframe_nospring()) {
						BodyFrameSetMatrix(this->obj_body, f, MATRIX_ref::RotX((f.second.x() >= 0) ? this->wheel_Left : this->wheel_Right));
					}
				}
				easing_set(
					&wheel_normal,
					((this->obj_body.frame(this->use_veh->Get_square(0)) - this->obj_body.frame(this->use_veh->Get_square(3))).cross(this->obj_body.frame(this->use_veh->Get_square(1)) - this->obj_body.frame(this->use_veh->Get_square(2)))).Norm(),
					0.95f);
				//履帯
				{
					for (auto& g : this->b2downsideframe) {
						for (auto& t : g) {
							MATRIX_ref tmp;
							this->obj_body.frame_Reset(t.frame.first);
							auto startpos = this->obj_body.frame(t.frame.first);
							t.colres = MAPPTs->map_col_line(startpos + move.mat.yvec() * ((-t.frame.second.y()) + 2.f), startpos + move.mat.yvec() * ((-t.frame.second.y()) - 0.3f));
							if (t.colres.HitFlag == TRUE) {
								tmp = MATRIX_ref::Mtrans(VECTOR_ref::up() * (t.colres.HitPosition.y + move.mat.yvec().y() * t.frame.second.y() - startpos.y()));
							}
							else {
								tmp = MATRIX_ref::Mtrans(VECTOR_ref::up() * -0.4f);
							}
							BodyFrameSetMatrix(this->obj_body, t.frame, tmp);
						}
					}
				}
				//移動
				{
					//リセット
					{
						const auto yvec = (this->use_veh->Get_isfloat() && isfloat) ? VECTOR_ref::up() : wheel_normal;
						auto pp = (move.mat * MATRIX_ref::RotVec2(VECTOR_ref::up(), yvec).Inverse()).zvec() * -1.f;
						auto yrad_p = 0.f;
						yrad_p = atan2f(pp.x(), pp.z());
						//printfDx("%5.2f\n", rad2deg(yrad_p));

						const auto zvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(-sinf(yrad_p), 0.f, -cosf(yrad_p)), MATRIX_ref::RotVec2(VECTOR_ref::up(), yvec));
						move.mat = MATRIX_ref::Axis1_YZ(yvec, zvec);
					}
					//地面判定
					{
						auto yp = move.pos.y();
						float hight_t = 0.f;
						int cnt_t = 0;
						bool tatch = false;
						for (const auto& g : this->b2downsideframe) {
							for (const auto& t : g) {
								hight_t += this->obj_body.frame(t.frame.first).y();
								cnt_t++;
								if (t.colres.HitFlag == TRUE) {
									tatch = true;
								}
							}
						}
						//*
						for (const auto& s : this->use_veh->Get_square()) {
							auto p_t = this->obj_body.frame(s);
							auto ColResGround = MAPPTs->map_col_line(p_t + (VECTOR_ref::up() * 2.f), p_t + (VECTOR_ref::up() * -0.3f));
							if (ColResGround.HitFlag == TRUE) {
								hight_t += ColResGround.HitPosition.y;
								cnt_t++;
								tatch = true;
							}
						}
						//*/
						if (tatch) {
							easing_set(&yp, (hight_t / cnt_t), 0.9f);
						}
						move.pos.y(yp);
						//地面or水面にいるかどうか
						if (tatch || (this->use_veh->Get_isfloat() && isfloat)) {
							//前進後退
							{
								const auto old = this->speed_add + this->speed_sub;
								if (key[2]) {
									this->speed_add = (this->speed_add < (this->use_veh->Get_front_speed_limit() / 3.6f)) ? (this->speed_add + (0.06f / 3.6f) * (60.f / FPS)) : this->speed_add;
									this->speed_sub = (this->speed_sub < 0.f) ? (this->speed_sub + (0.7f / 3.6f) * (60.f / FPS)) : this->speed_sub;
								}
								if (key[3]) {
									this->speed_sub = (this->speed_sub > (this->use_veh->Get_back_speed_limit() / 3.6f)) ? (this->speed_sub - (0.06f / 3.6f) * (60.f / FPS)) : this->speed_sub;
									this->speed_add = (this->speed_add > 0.f) ? (this->speed_add - (0.7f / 3.6f) * (60.f / FPS)) : this->speed_add;
								}
								if (!(key[2]) && !(key[3])) {
									this->speed_add = (this->speed_add > 0.f) ? (this->speed_add - (0.35f / 3.6f) * (60.f / FPS)) : 0.f;
									this->speed_sub = (this->speed_sub < 0.f) ? (this->speed_sub + (0.35f / 3.6f) * (60.f / FPS)) : 0.f;
								}
								this->speed = (old + ((this->speed_add + this->speed_sub) - old) * 0.1f) / FPS;
								move.vec = move.mat.zvec() * -this->speed;
							}
							//旋回
							{
								this->yradadd_left = (key[4]) ? std::max(this->yradadd_left - deg2rad(3.5f * (60.f / FPS)), deg2rad(-this->use_veh->Get_body_rad_limit())) : std::min(this->yradadd_left + deg2rad(2.1f * (60.f / FPS)), 0.f);
								this->yradadd_right = (key[5]) ? std::min(this->yradadd_right + deg2rad(3.5f * (60.f / FPS)), deg2rad(this->use_veh->Get_body_rad_limit())) : std::max(this->yradadd_right - deg2rad(2.1f * (60.f / FPS)), 0.f);
								this->yradadd = (this->yradadd_left + this->yradadd_right) / FPS;
							}
							//慣性
							{
								const auto xradold = this->xradadd;
								this->xradadd = deg2rad(-((this->speed) / (60.f / FPS)) / (0.1f / 3.6f) * 50.f);
								easing_set(&this->xrad, std::clamp(this->xradadd - xradold, deg2rad(-30.f), deg2rad(30.f)), 0.995f);

								auto zradold = this->zradadd;
								this->zradadd = deg2rad(-this->yradadd / (deg2rad(5.f) / FPS) * 30.f);
								easing_set(&this->zrad, std::clamp(this->zradadd - zradold, deg2rad(-15.f), deg2rad(15.f)), 0.995f);

								move.mat *= MATRIX_ref::RotAxis(move.mat.xvec(), -this->xrad) * MATRIX_ref::RotAxis(move.mat.zvec(), this->zrad);
							}
							//
						}
						else {
							move.vec.yadd(M_GR / powf(FPS, 2.f));
						}
					}
					move.pos += move.vec;
				}
				//射撃反動
				for (auto& g : this->Gun_) {
					g.FireReaction(&move.mat);
				}
				//転輪
				this->wheel_Leftadd = -(this->speed * 2.f - this->yradadd * 5.f);
				this->wheel_Rightadd = -(this->speed * 2.f + this->yradadd * 5.f);

				this->wheel_Left += this->wheel_Leftadd;  // -this->yradadd * 5.f;
				this->wheel_Right += this->wheel_Rightadd; // +this->yradadd * 5.f;
				//戦車壁判定
				this->b2mine.Update(move.vec, this->yradadd);
				//浮く
				if (this->use_veh->Get_isfloat()) {
					move.pos.y(std::max(move.pos.y(), -this->use_veh->Get_down_in_water()));
				}
				//射撃
				for (auto& cg : this->Gun_) {
					cg.Update(key[(&cg == &this->Gun_.front()) ? 0 : 1], this->obj_body.frame(cg.Getgun_info().Get_frame(2).first), BodyFrameMatrix(this->obj_body, cg.Getgun_info().Get_frame(1)));
				}
			}
			void Update_Tank2() {
				auto pp = move.mat.zvec();
				this->body_yrad = atan2f(-pp.x(), -pp.z());
				//戦車座標反映
				move.mat *= MATRIX_ref::RotY(-this->b2mine.Rad() - this->body_yrad);
				move.pos = VECTOR_ref::vget(this->b2mine.Pos().x, move.pos.y(), this->b2mine.Pos().y);
				float spdrec = this->spd_buf;
				easing_set(&this->spd_buf, this->b2mine.Speed() * ((this->spd_buf > 0) ? 1.f : -1.f), 0.99f);
				this->speed = this->spd_buf - spdrec;
				//move.mat *= MATRIX_ref::RotAxis(move.mat.yvec(),this->yradadd);

				//転輪
				b2Vec2 tmpb2 = b2Vec2((M_GR / FPS * 0.5f) * (move.mat.zvec().dot(VECTOR_ref::up())), (M_GR / FPS * 0.5f) * (move.mat.yvec().dot(VECTOR_ref::up())));
				int LorR = 0;
				for (auto& f : foot) {
					size_t i = 0;
					VECTOR_ref vects;
					int LR_t = ((LorR == 0) ? 1 : -1);
					if (f.Foot.size() != 0) {
						auto Wheel_t = f.Wheel.begin();
						for (const auto& w : this->use_veh->Get_wheelframe()) {
							vects = this->obj_body.GetFrameLocalMatrix(w.first).pos();
							if (vects.x() * LR_t > 0) {
								Wheel_t->SetTransform(b2Vec2(vects.z(), vects.y()), (LorR == 0) ? this->wheel_Left : this->wheel_Right);
								Wheel_t++;
							}
						}
						i = 0;
						for (const auto& w : this->use_veh->Get_wheelframe_nospring()) {
							vects = this->obj_body.GetFrameLocalMatrix(w.first).pos();
							if (vects.x() * LR_t > 0) {
								f.Yudo[i++].SetTransform(b2Vec2(vects.z(), vects.y()), (LorR == 0) ? this->wheel_Left : this->wheel_Right);
							}
						}
						for (auto& t : f.Foot) {
							t.SetLinearVelocity(tmpb2); //
						}

						f.world->Step(0.1f, 3, 3);

						for (auto& t : f.Foot) {
							t.pos = VECTOR_ref::vget(t.pos.x(), t.Pos().y, t.Pos().x);
						}
					}
					{
						i = 0;
						for (const auto& w : this->use_veh->Get_b2upsideframe(LorR)) {
							if (w.second.x() * LR_t > 0) {
								auto& t = f.Foot[i++];
								t.pos = VECTOR_ref::vget(w.second.x(), t.pos.y(), t.pos.z());
								this->obj_body.SetFrameLocalMatrix(w.first, MATRIX_ref::Mtrans(t.pos));
							}
						}
					}
					LorR++;
				}

				this->obj_body.SetMatrix(move.MatIn());

				for (auto& g : this->b2downsideframe) {
					for (auto& t : g) {
						if (t.colres.HitFlag == TRUE) {
							easing_set(&t.gndsmksize, 0.1f + std::abs(this->speed - this->spd_rec) / ((0.01f / 3.6f) / FPS) * 1.f, 0.95f);
						}
						else {
							t.gndsmksize = 0.1f;
						}
						t.gndsmkeffcs.handle.SetPos(this->obj_body.frame(t.frame.first) + move.mat.yvec() * (-t.frame.second.y()));
						t.gndsmkeffcs.handle.SetScale(t.gndsmksize);
					}
				}
			}
			/*おわり*/
			void Dispose_Tank(void) noexcept {
				this->b2mine.Dispose();			/*Box2D*/
				for (auto& t : foot) {
					for (auto& f : t.Foot) { f.Dispose(); }
					t.Foot.clear();
					for (auto& f : t.Wheel) { f.Dispose(); }
					t.Wheel.clear();
					for (auto& f : t.Yudo) { f.Dispose(); }
					t.Yudo.clear();
				}
				for (auto& g : this->b2downsideframe) {
					for (auto& t : g) {
						t.gndsmkeffcs.handle.Dispose();
					}
					g.clear();
				}
				for (auto& cg : this->Gun_) { cg.Dispose(); }
			}
			void ReSet_Tank(void) noexcept {
				this->speed = 0.f;
				this->speed_add = 0.f;
				this->speed_sub = 0.f;
				this->wheel_Left = 0.f;
				this->wheel_Right = 0.f;
				this->wheel_Leftadd = 0.f;
				this->wheel_Rightadd = 0.f;
				this->xrad = 0.f;
				this->xradadd = 0.f;
				this->xradadd_left = 0.f;
				this->xradadd_right = 0.f;
				this->yrad = 0.f;
				this->yradadd = 0.f;
				this->yradadd_left = 0.f;
				this->yradadd_right = 0.f;
				this->zrad = 0.f;
				this->zradadd = 0.f;
				this->zradadd_left = 0.f;
				this->zradadd_right = 0.f;
				for (auto& f : foot) {
					f.world.reset();
				}
				for (auto& g : this->Gun_) { g.Reset(); }
				this->Gun_.clear();
			}
		public:
			const MATRIX_ref Head_direction(void)const noexcept override { return BodyFrameMatrix(this->obj_body, Gun_[0].Getgun_info().Get_frame(1)); }
			const VECTOR_ref Head_Position(void) const noexcept { return this->Get_EyePos_Base(); }
			//getter
			const bool is_ADS(void) const noexcept override { return this->range == 0.f; }								//ADS中
			//getter
			const auto& Gunloadtime(size_t id_t) const noexcept { return this->Gun_[id_t].Getloadtime(); }
			const auto& Gunround(size_t id_t) const noexcept { return this->Gun_[id_t].Getrounds(); }
			const auto& Guninfo(size_t id_t) const noexcept { return this->Gun_[id_t].Getgun_info(); }
			const auto Get_Gunsize(void) const noexcept { return this->Gun_.size(); }
			const auto& Get_ratio(void) const noexcept { return this->ratio; }											//UI用
			const auto& Get_changeview(void) const noexcept { return this->changeview; }								//照準変更時
			const VECTOR_ref Get_EyePos_Base(void) const noexcept { return this->Get_pos() + (this->move.mat.yvec() * 3.f); }
			//
			void ReSet_range(void) noexcept { range = 6.f; }
			//
			void Set_bullet_Ptr(void) noexcept { for (auto& g : this->Gun_) { g.Set_bullet_Ptr(); } }
			//
			void SetUp_bullet(std::shared_ptr<MAPclass::Map>& MAPPTs_t, std::shared_ptr<DXDraw>& DrawPts_t) { for (auto& g : this->Gun_) { g.SetUp_bullet(MAPPTs_t, DrawPts_t); } }
			void Set_Draw_bullet() { for (auto& g : this->Gun_) { g.Set_Draw_bullet(); } }
			//弾痕セット
			void SetHit(const moves& this_move, const  VECTOR_ref& Put_pos, const  VECTOR_ref& Put_normal, const VECTOR_ref& ammo_nomal, const float& caliber, bool isPene) { this->Hit_active.Set(this_move, Put_pos, Put_normal, ammo_nomal, caliber, isPene); }
			//命中判定
			bool HitCheck_Tank(int m, moves& ray) { return hitControl.HitCheck(m, ray.repos, this->col_line(ray.repos, ray.pos, m)); }
			//乗車下車
			void RideOn(std::shared_ptr<PLAYER_CHARA>* MINE_c_t) {
				this->MINE_c = MINE_c_t;
				this->Set_xradP((*this->MINE_c)->Get_xradP());
				this->lookvec = (*this->MINE_c)->GetHMDmat();
				this->Set_bullet_Ptr();
				(*this->MINE_c)->Set_bullet_Ptr();
			}
			void GetOff(std::shared_ptr<PLAYER_CHARA>* MINE_c_t) {
				this->MINE_c = nullptr;
				this->ReSet_range();
				this->Set_bullet_Ptr();
				(*MINE_c_t)->Set_basepos(this->Get_pos());
				(*MINE_c_t)->SetHMDmat(this->lookvec);
				(*MINE_c_t)->Set_bullet_Ptr();
			}
			/*HPバーを表示する場所*/
			const auto Set_HP_UI(void) noexcept {
				return Set_HP_UI_Common(this->Get_EyePos_Base());
			}
		public:
			using PLAYER_COMMON::PLAYER_COMMON;
			void Set(Vehcs* vehc_data_t, const MV1& hit_pic) {
				this->move = this->spawn;
				this->use_veh = vehc_data_t;
				//本体
				this->obj_body = this->use_veh->Get_obj().Duplicate();
				this->obj_body.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
				//コリジョン
				this->obj_col = this->use_veh->Get_col().Duplicate();
				PLAYER_COMMON::SetupCollInfo();
				hitControl.Set(this->obj_col.mesh_num());
				//戦車set
				Set_Tank();
				//ヒットポイント
				Damage.Set((int)this->obj_col.mesh_num(), this->use_veh->Get_HP());
				//弾痕
				Hit_active.Set(hit_pic);
			}
			/*戦車スポーン*/
			void Spawn(void) noexcept {
				PLAYER_COMMON::Spawn_Common();
				this->move = this->spawn;
				this->b2mine.SetTransform(b2Vec2(move.pos.x(), move.pos.z()), atan2f(-move.mat.zvec().x(), -move.mat.zvec().z()));
			}
			/**/
			void Update(const cam_info& cams, float fov_per) {
				Damage.Update();
				if (isRide() && Damage.Get_alive()) {
					key[0] = (*MINE_c)->Get_key_().shoot;	//射撃
					key[1] = (*MINE_c)->Get_key_().aim;		//マシンガン(現状無し)
					key[2] = (*MINE_c)->Get_key_().wkey;		//前進
					key[3] = (*MINE_c)->Get_key_().skey;		//後退
					key[4] = (*MINE_c)->Get_key_().dkey;		//右
					key[5] = (*MINE_c)->Get_key_().akey;		//左
					{
						if (false) { //砲塔ロック
							view_yrad = 0.f;
							view_xrad = 0.f;
						}
						else {
							//狙い
							float x_m = 0.f, y_m = 0.f;
							if (MINE_c == &ALL_c->front()) {
								Get_MouseVec(&x_m, &y_m, fov_per, DrawPts->disp_x, DrawPts->disp_y);
								x_m = deg2rad(x_m) * 0.1f;
								y_m = deg2rad(-y_m) * 0.1f;
							}
							if (is_ADS()) {
								easing_set(&view_yrad, x_m, 0.1f);
								easing_set(&view_xrad, y_m, 0.1f);
								float limit = this->use_veh->Get_turret_rad_limit() / FPS;
								lookvec = MATRIX_ref::RotX(-this->Get_xradP()) * lookvec;
								lookvec *= MATRIX_ref::RotY(std::clamp(view_yrad, -limit, limit));										//y軸回転(旋回)
								lookvec = MATRIX_ref::RotX(this->Add_xradP(view_xrad, -limit, limit)) * lookvec;						//x軸回転(仰俯)
							}
							else {
								if (MINE_c == &ALL_c->front()) {
									VECTOR_ref vec_a;
									lookvec = MATRIX_ref::RotX(-this->Get_xradP()) * lookvec;
									lookvec *= MATRIX_ref::RotY(x_m);																		//y軸回転(旋回)
									lookvec = MATRIX_ref::RotX(this->Add_xradP(y_m, deg2rad(-80), deg2rad(60))) * lookvec;					//x軸回転(仰俯)
									VECTOR_ref startpos = Get_EyePos_Base();
									VECTOR_ref endpos = startpos + (cams.camvec - cams.campos).Norm() * (100.f);
									MAPPTs->map_col_nearest(startpos, &endpos);															//マップに当たったか
									vec_a = (this->obj_body.frame(Guninfo(0).Get_frame(1).first) - endpos).Norm();
									//反映
									auto vec_z = this->obj_body.frame(Guninfo(0).Get_frame(2).first) - this->obj_body.frame(Guninfo(0).Get_frame(1).first);
									float z_hyp = std::hypotf(vec_z.x(), vec_z.z());
									float a_hyp = std::hypotf(vec_a.x(), vec_a.z());
									float cost = (vec_a.z() * vec_z.x() - vec_a.x() * vec_z.z()) / (a_hyp * z_hyp);

									view_yrad = (atan2f(cost, sqrtf(std::abs(1.f - cost * cost)))) / 5.f; //cos取得2D
									view_xrad = (atan2f(-vec_z.y(), z_hyp) - atan2f(vec_a.y(), a_hyp)) / 5.f;
								}
								else {
									easing_set(&view_yrad, x_m, 0.1f);
									easing_set(&view_xrad, y_m, 0.1f);
									float limit = this->use_veh->Get_turret_rad_limit() / FPS;
									lookvec = MATRIX_ref::RotX(-this->Get_xradP()) * lookvec;
									lookvec *= MATRIX_ref::RotY(std::clamp(view_yrad, -limit, limit));										//y軸回転(旋回)
									lookvec = MATRIX_ref::RotX(this->Add_xradP(view_xrad, -limit, limit)) * lookvec;						//x軸回転(仰俯)
								}
							}
						}
					}
				}
				else {
					key[0] = false;	//射撃
					key[1] = false;	//マシンガン
					key[2] = false;	//前進
					key[3] = false;	//後退
					key[4] = false;	//右
					key[5] = false;	//左
					view_yrad = 0.f;
					view_xrad = 0.f;
				}
				this->speed_base = this->use_veh->Get_front_speed_limit() / 3.6f;
				this->nearhit = false;
				//戦車演算
				Update_Tank();
			}
			void Update_after(void) noexcept {
				//戦車演算
				{
					auto pos_t2 = this->obj_body.GetMatrix().pos();
					//戦車座標反映
					Update_Tank2();
					this->add_vec_real = this->Get_pos() - pos_t2;
					this->obj_col.SetMatrix(this->obj_body.GetMatrix());
				}
				//エンジン音
				if (this->engine_time == 0.f) {
					SE.Get(EnumSound::Tank_engine).Play_3D(0, this->Get_pos(), 50.f, 64);
					this->engine_time = 1.f;
				}
				else {
					this->engine_time -= 1.f / FPS;
					if (this->engine_time <= 0.f) {
						this->engine_time = 0.f;
					}
				}
				//エフェクトの処理
				Update_effect();
				//弾痕
				Hit_active.Update(this->move);
			}
			/*カメラ指定*/
			void Set_cam(cam_info& camera_main, const float fov_base) noexcept {
				VECTOR_ref eyepos, eyetgt;
				auto OLD = range;
				if (is_ADS()) {
					auto vec_z = this->obj_body.frame(Guninfo(0).Get_frame(2).first) - this->obj_body.frame(Guninfo(0).Get_frame(1).first);
					eyepos = this->obj_body.frame(Guninfo(0).Get_frame(1).first);
					eyetgt = eyepos + vec_z * 1.f;

					ratio = std::clamp(ratio + float(GetMouseWheelRotVol()) * 2.0f, 0.0f, 10.f);
					if (ratio == 0.f) {
						range = 0.f + range_change;
						range_r = range;
					}
					else {
						easing_set(&camera_main.fov, fov_base / ratio, 0.9f);
					}
				}
				else {
					ratio = 2.0f;
					range = std::clamp(range - float(GetMouseWheelRotVol()) * range_change, 0.f, 9.f);

					easing_set(&range_r, range, 0.8f);
					eyepos = Get_EyePos_Base() + lookvec.zvec() * range_r;
					eyetgt = eyepos + lookvec.zvec() * (-range_r);
					if (MAPPTs->map_col_nearest(eyepos, &eyetgt)) {
						eyepos = eyetgt;
					}
					eyetgt = eyepos + lookvec.zvec() * (-std::max(range_r, 1.f));
					easing_set(&camera_main.fov, fov_base, 0.9f);
				}
				changeview = ((range != OLD) && (range == 0.f || OLD == 0.f));
				camera_main.set_cam_pos(eyepos, eyetgt, this->move.mat.yvec());
				camera_main.near_ = 0.1f;
			}
			/*描画*/
			void Draw(void) noexcept {
				if (is_ADS()) {
				}
				else {
					MV1SetFrameTextureAddressTransform(this->obj_body.get(), 0, -this->wheel_Left * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
					this->obj_body.DrawMesh(0);
					MV1SetFrameTextureAddressTransform(this->obj_body.get(), 0, -this->wheel_Right * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
					this->obj_body.DrawMesh(1);
					MV1ResetFrameTextureAddressTransform(this->obj_body.get(), 0);
					for (int i = 2; i < this->obj_body.mesh_num(); i++) {
						this->obj_body.DrawMesh(i);
					}
					//obj_col.DrawModel();
					Hit_active.Draw();
				}
			}
			/*レーザー、ライト描画*/
			void Draw_LAM_Effect(void) noexcept {
				if (isRide()) {
					auto StartPos = this->obj_body.frame(Guninfo(0).Get_frame(2).first);
					Draw_Lazer_Effect(StartPos, StartPos - this->obj_body.frame(Guninfo(0).Get_frame(1).first), !is_ADS(), 1.f);
				}
			}
			//UI描画用用意
			void Draw_Hit_UI(GraphHandle& hit_Graph) noexcept {
				for (auto& cg : this->Gun_) { cg.Draw_Hit_UI(hit_Graph); }
			}
			//
			void Draw_ammo(void) noexcept {
				for (auto& cg : this->Gun_) { cg.Draw_ammo(); }
			}
			/*おわり*/
			void Dispose(void) noexcept override {
				Dispose_Tank();
				PLAYER_COMMON::Dispose();
				Hit_active.Dispose();
				this->Reset();
			}
			void Reset(void) noexcept override {
				ReSet_Tank();
				PLAYER_COMMON::Reset();
				hitControl.Reset();
			}
		};
	};
	//銃
	std::vector<std::shared_ptr<PLAYERclass::GunControl>> Gun_S;
}