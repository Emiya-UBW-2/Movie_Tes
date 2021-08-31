#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	class Sceneclass {
	public:
		class LOAD;
		class SELECT;
		class MAINLOOP;
		//
		class TEMPSCENE : public PTR_COMMON {
		private:
			VECTOR_ref Shadow_minpos;
			VECTOR_ref Shadow_maxpos;
			VECTOR_ref Light_vec;
			COLOR_F Light_color = GetColorF(0, 0, 0, 0);
		protected:
			//キー
			PLAYERclass::PLAYER_COMMON::opes key_;
			//引き継ぐ
			std::shared_ptr<OPTION> OPTPTs;
			std::shared_ptr<GunPartsControl> GunPartses;
			std::shared_ptr<key_bind> KeyBind;
			//カメラ
			cam_info camera_main;
			float fov_base = DX_PI_F / 2;
			//
			bool use_lens{ false };
			float lens_zoom = 1.f;
			float lens_size = 1.f;
			//
			bool use_bless{ false };
			float bless_ratio = 0.5f;
			float bless = 0.f;
			//
			virtual void Set_EnvLight(VECTOR_ref Shadow_minpos_t, VECTOR_ref Shadow_maxpos_t, VECTOR_ref Light_vec_t, COLOR_F Light_color_t) noexcept {
				Shadow_minpos = Shadow_minpos_t;
				Shadow_maxpos = Shadow_maxpos_t;
				Light_vec = Light_vec_t;
				Light_color = Light_color_t;
			}
		public:
			scenes Next_scene{ scenes::NONE_SCENE };			//現在のシーン
			std::shared_ptr<Sceneclass::TEMPSCENE> Next_ptr{ nullptr };

			TEMPSCENE(
				std::shared_ptr<MAPclass::Map>& MAPPTs_t,
				std::shared_ptr<DXDraw>& DrawPts_t,
				const std::shared_ptr<OPTION>& OPTPTs_t,
				const std::shared_ptr<GunPartsControl>& GunPartses_t,
				const std::shared_ptr<key_bind>& KeyBind_t
			) noexcept {
				PTR_COMMON::Set_Ptr_Common(MAPPTs_t, DrawPts_t);
				OPTPTs = OPTPTs_t;
				GunPartses = GunPartses_t;
				KeyBind = KeyBind_t;
			}
			void Set_Next(const std::shared_ptr<Sceneclass::TEMPSCENE>& Next_scenes_ptr_t, scenes NEXT) {
				Next_ptr = Next_scenes_ptr_t;
				Next_scene = NEXT;
			}

			const VECTOR_ref& Get_Light_vec(void) const noexcept { return Light_vec; }
			cam_info& Get_Camera(void) noexcept { return camera_main; }

			virtual void Awake(void) noexcept {}
			virtual void Set(void) noexcept {
				fov_base = deg2rad(DrawPts->use_vr ? 120 : OPTPTs->Get_Fov());	//fov
				SetUseMaskScreenFlag(FALSE);//←カスタム画面でエフェクトが出なくなるため入れる
				if (DrawPts->use_vr) {
					camera_main.set_cam_info(fov_base, 0.001f, 100.f);//1P
				}
				else {
					camera_main.set_cam_info(fov_base, 0.05f, 200.f);//1P
				}

				DrawPts->Set_Light_Shadow(Shadow_maxpos, Shadow_minpos, Light_vec, [&] {Shadow_Draw_Far(); });
				SetGlobalAmbientLight(Light_color);
			}
			virtual bool Update(void) noexcept {
				if (use_bless) {
					bless += deg2rad(float(100 + GetRand(600)) / 100.f * Frame_Rate / GetFPS());
					bless_ratio -= (0.03f / 90.f) * Frame_Rate / GetFPS();
					if (bless_ratio <= 0.f) {
						use_bless = false;
					}
				}
				return true;
			}
			virtual void Dispose(void) noexcept {}

			virtual void ReadyDraw(void) noexcept {
				//音位置指定
				Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
				//影用意
				DrawPts->Ready_Shadow(camera_main.campos, [&] { Shadow_Draw(); }, [&] { Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.5f, 2.5f, 2.5f), VECTOR_ref::vget(15.f, 2.5f, 15.f));//MAIN_LOOPのnearはこれ (Get_Mine()->Damage.Get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
			}
			virtual void UI_Draw(void) noexcept {}
			virtual void BG_Draw(void) noexcept {
				DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_x, GetColor(192, 192, 192), TRUE);
			}
			virtual void Shadow_Draw_Far(void) noexcept {}
			virtual void Shadow_Draw_NearFar(void) noexcept {}
			virtual void Shadow_Draw(void) noexcept {}
			virtual void Main_Draw(void) noexcept {}

			virtual const bool& is_lens(void) { return use_lens; }
			virtual const float& zoom_lens(void) { return lens_zoom; }
			virtual const float& size_lens(void) { return lens_size; }
			virtual const bool& is_bless(void) { return use_bless; }
			virtual const float& ratio_bless(void) { return bless_ratio; }
			virtual const float& time_bless(void) { return bless; }

			virtual void Item_Draw(void) noexcept {}
			virtual void LAST_Draw(void) noexcept {}
			virtual void KeyOperation_VR(void) noexcept {}
			virtual void KeyOperation(void) noexcept {}
		};
		//
		class LOADING : public TEMPSCENE {
		private:
			std::string title;
			std::unique_ptr<UIclass::UI_LOADING> UIparts;
		public:
			using TEMPSCENE::TEMPSCENE;

			void settitle(const std::string_view mes) {
				title = mes;
			}

			void Awake(void) noexcept override {
				TEMPSCENE::Awake();
				UIparts = std::make_unique<UIclass::UI_LOADING>();
			}
			void Set(void) noexcept override {
				TEMPSCENE::Set();
				UIparts->Set_Ptr_Common(MAPPTs, DrawPts);
				UIparts->Set(title);
			}
			bool Update(void) noexcept override {
				TEMPSCENE::Update();
				return UIparts->Update();
			}
			void UI_Draw(void) noexcept override {
				UIparts->UI_Draw();
			}
		};

		class LOAD : public TEMPSCENE, public PresetSaveControl {
		private:
			switchs left;			//キー
			switchs right;			//キー
			GUNPARTs* temp_p{ nullptr };			//データ
			std::vector<std::string> Save_Presets;			//
			std::unique_ptr<UIclass::UI_LOAD> UIparts;
		public:
			const auto& putout_Preset(void) const noexcept { return Save_Presets[P_select]; }
			//
			using TEMPSCENE::TEMPSCENE;
			void Awake(void) noexcept override {
				TEMPSCENE::Awake();
				UIparts = std::make_unique<UIclass::UI_LOAD>();
			}
			void Set(void) noexcept override {
				TEMPSCENE::Set();
				UIparts->Set_Ptr_Common(MAPPTs, DrawPts);
				left.Init(false);
				right.Init(false);
				Save_Presets.clear();
				GetFileNames("data/save/");
				for (auto& d : data_t) {
					Save_Presets.emplace_back(d.cFileName);
				}
				P_select_max = int(Save_Presets.size());
				P_select = 0;
				/*パーツデータをロード*/
				Load(putout_Preset());
			}
			bool Update(void) noexcept override {
				TEMPSCENE::Update();
				bool changef = false;
				//演算
				{
					SetMouseDispFlag(TRUE);
					left.GetInput(key_.akey);
					right.GetInput(key_.dkey);
					if (left.trigger()) {
						SE.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
						--P_select;
						changef = true;
					}
					if (right.trigger()) {
						SE.Get(EnumSound::CURSOR).Play(0, DX_PLAYTYPE_BACK, TRUE);
						++P_select;
						changef = true;
					}
					if (key_.jamp) {
						SE.Get(EnumSound::Assemble).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					ControlSel();
					if (changef) {
						/*パーツデータをロード*/
						Load(putout_Preset());
					}
				}
				return (!key_.jamp);
			}
			void UI_Draw(void) noexcept override {
				UIparts->UI_Draw(GunPartses, save_parts, putout_Preset());
			}
			void KeyOperation_VR(void) noexcept override {
#ifdef _USE_OPENVR_
				if (DrawPts->get_hand2_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand2();
					if (ptr_->turn && ptr_->now) {
						key_.dkey = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() > 0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);		//running
						key_.akey = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() < -0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);		//running
						key_.jamp = ((ptr_->on[0] & BUTTON_SIDE) != 0);																							//jamp
					}
				}
#endif // _USE_OPENVR_
			}
			void KeyOperation(void) noexcept override {
				//設定
				KeyBind->Set_Mode(1);
				key_.dkey = KeyBind->Get_key_use(EnumKeyBind::RIGHT);
				key_.akey = KeyBind->Get_key_use(EnumKeyBind::LEFT);
				key_.jamp = KeyBind->Get_key_use(EnumKeyBind::JUMP);
			}
		};
		//
		class SELECT : public TEMPSCENE, public PresetSaveControl, public Effect_UseControl {
		private:
			//sound
			SoundHandle shot_se;
			SoundHandle slide_se;
			SoundHandle trigger_se;
			//キー
			switchs up;
			switchs down;
			switchs left;
			switchs right;
			switchs shot;
			//データ
			GUNPARTs* parts_p{ nullptr };
			//
			EnumGunParts parts_cat = EnumGunParts::NONE;
			EnumAttachPoint port_cat = EnumAttachPoint::NONE;
			EnumGunParts port_type = EnumGunParts::NONE;
			PLAYERclass::GunControl::g_parts* port_ptr{ nullptr };
			std::vector<PLAYERclass::GunControl::g_parts*> sight_ptr;
			int sight_p_s = 0;
			//
			int change_select = 0;
			int change_select_max = 0;
			//
			VECTOR_ref viewparts;
			VECTOR_ref viewparts_buf;
			VECTOR_ref camparts;
			float xrad_t = 30.f;
			float yrad_t = -135.f;
			float range_t = 0.25f;
			float range_tgt = 0.25f;
			switchs Rot;
			bool Start_b{ true };
			float rate = 1.0f;
			//
			float change_per = 1.f;
			//
			std::string save_tgt = "1.dat";
			std::unique_ptr<UIclass::UI_CUSTOM> UIparts;
			//
			std::string Preset;

			std::shared_ptr<PLAYERclass::GunControl> mine_Gun{ nullptr };
			GraphHandle light;												//ライトテクスチャ
		private:
			//必須品
			void Set_Parts_Vital(EnumGunParts port_type_t, EnumAttachPoint port_cat_t, EnumGunParts parts_cat_t) {
				if (P_select == P_select_max) {
					port_type = port_type_t;
					port_ptr = mine_Gun->Get_parts(port_type);
					port_cat = port_cat_t;
					parts_cat = parts_cat_t;
					{
						change_select_max = (int)GunPartses->Get_Parts_Data(parts_cat).size();
						change_select = std::clamp(change_select, 0, change_select_max - 1);
						parts_p = &(GunPartses->Get_Parts_Data(parts_cat))[change_select];
						/*
						{
							bool aaa = false;
							auto total_t = (int)GunPartses.Get_Parts_Data(parts_cat)->size();
							change_select_max = (int)GunPartses.Get_Parts_Data(parts_cat)->size();
							change_select = std::clamp(change_select, 0, total_t - 1);
							while (true) {
								auto tmp = &(*GunPartses.Get_Parts_Data(parts_cat))[change_select];

								aaa = false;
								for (auto& p : port_ptr->thisparts->can_attach) {
									if (p == tmp->mod.Get_name()) {
										aaa = true;
										break;
									}
								}
								if (aaa) {
									//ok
									parts_p = tmp;
									change_select_max = total_t;
									break;
								}
								else {
									//NG
									total_t--;
									change_select = std::clamp(change_select++, 0, total_t - 1);
								}
								if (total_t == 0) {
									//NG
									break;
								}
							}
						}
						//*/
						viewparts_buf = port_ptr->Get_rail_pos(port_cat);
						if (parts_cat == EnumGunParts::MAGAZINE) {
							mine_Gun->Get_gun_stat_now()->Reset();
						}
						mine_Gun->Detach_parts(parts_cat);
						mine_Gun->Attach_parts(parts_p, parts_cat, port_ptr, port_cat);
						if (parts_cat == EnumGunParts::MAGAZINE) {
							if (mine_Gun->Get_gun_stat_now()->magazine_pushback(mine_Gun->Get_parts(EnumGunParts::MAGAZINE)->thisparts)) {
								mine_Gun->Set_in_chamber(true);
							}
						}
					}
					Set_PartsSave(size_t(change_select), parts_cat, port_cat, port_type);
				}
				++P_select_max;

				//サイト判定
				if (mine_Gun->Get_parts(parts_cat_t)->Set_sight_at(GunPartses->Get_Parts_Data(EnumGunParts::SIGHT))) {
					sight_ptr[sight_p_s] = mine_Gun->Get_parts(parts_cat_t); ++sight_p_s;
				}
			}
			//非必須品
			void Set_Change_Optional(EnumGunParts port_type_t, EnumAttachPoint pot_cat, int sel = 0) noexcept {
				parts_cat = port_type_t;
				change_select_max = (int)GunPartses->Get_Parts_Data(parts_cat).size() + 1;
				change_select = std::clamp(change_select, 0, change_select_max - 1);
				if (change_select == 0) { parts_p = nullptr; }
				else { parts_p = &GunPartses->Get_Parts_Data(port_type_t)[std::max(change_select - 1, 0)]; }

				viewparts_buf = port_ptr->Get_rail_pos(pot_cat);
				mine_Gun->Detach_parts(port_type_t, (sel == 0) ? sel : (sel - 1));
				if (change_select != 0) {
					mine_Gun->Attach_parts(parts_p, port_type_t, port_ptr, pot_cat, sel);
				}
				Set_PartsSave(size_t(change_select) - 1, parts_cat, port_cat, port_type);
			}
			void Set_Parts_Optional(EnumGunParts port_type_t, EnumAttachPoint port_cat_t, EnumGunParts parts_cat_t) {
				port_type = port_type_t;
				port_ptr = mine_Gun->Get_parts(port_type);
				port_cat = port_cat_t;
				if (port_ptr->Get_rail_frame(port_cat).first > 0) {
					if (P_select == P_select_max) {
						Set_Change_Optional(parts_cat_t, port_cat);
					}
					++P_select_max;
				}
				//サイト判定
				if (mine_Gun->Get_parts(parts_cat_t)->Set_sight_at(GunPartses->Get_Parts_Data(EnumGunParts::SIGHT))) {
					sight_ptr[sight_p_s] = mine_Gun->Get_parts(parts_cat_t); ++sight_p_s;
				}
			}
			//sight
			void Set_Parts_Sight(PLAYERclass::GunControl::g_parts* s, int sel) {
				if (s != nullptr) {
					port_ptr = s;
					port_type = port_ptr->Get_type();

					port_cat = EnumAttachPoint::UPER_RAIL;
					if (port_ptr->Get_rail_frame(port_cat).first > 0) {
						if (P_select == P_select_max) {
							Set_Change_Optional(EnumGunParts::SIGHT, port_cat, sel);
							if (port_type == EnumGunParts::MOUNT) {
								save_parts[P_select_max].pt_sel_ = 1;//改善
							}
						}
						++P_select_max;
						return;
					}

					port_cat = EnumAttachPoint::SIDEMOUNT;
					if (port_ptr->Get_rail_frame(port_cat).first > 0) {
						if (P_select == P_select_max) {
							Set_Change_Optional(EnumGunParts::SIGHT, port_cat, sel);
							if (port_type == EnumGunParts::MOUNT) {
								save_parts[P_select_max].pt_sel_ = 1;//改善
							}
						}
						++P_select_max;
						return;
					}
				}
			}
		public:
			using TEMPSCENE::TEMPSCENE;
			void Awake(void) noexcept override {
				TEMPSCENE::Awake();
				UIparts = std::make_unique<UIclass::UI_CUSTOM>();
				light = GraphHandle::Load("data/light.png");								//ライト
			}

			void Start(std::string_view Set_str) {
				Preset = Set_str;
				mine_Gun = Gun_S[0];
			}

			void Set(void) noexcept  override {
				//ライティング
				TEMPSCENE::Set_EnvLight(VECTOR_ref::vget(1.f, 1.f, 1.f), VECTOR_ref::vget(-1.f, -1.f, -1.f), VECTOR_ref::vget(0.5f, -0.5f, 0.5f), GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				TEMPSCENE::Set();
				SetMousePoint(DrawPts->disp_x / 2, DrawPts->disp_y / 2);											//
				UIparts->Set_Ptr_Common(MAPPTs, DrawPts);
				{
					up.Init(false);
					down.Init(false);
					left.Init(false);
					right.Init(false);
					shot.Init(false);
					Rot.Init(false);
					parts_p = nullptr;
					parts_cat = EnumGunParts::NONE;

					P_select = 0;
					P_select_max = 0;

					change_select = 0;
					change_select_max = 0;
					viewparts_buf.clear();
					camparts.clear();
					xrad_t = 30.f;
					yrad_t = -135.f;
					range_t = 0.25f;
					range_tgt = 0.25f;
					Start_b = true;
					rate = 0.1f;
				}
				{
					SetCreateSoundTimeStretchRate(1.f / std::clamp(rate, 0.9f, 1.1f));
					shot_se = SoundHandle::Load(mine_Gun->Audio.shot_path);
					slide_se = SoundHandle::Load(mine_Gun->Audio.slide_path);
					trigger_se = SoundHandle::Load("data/Audio/gun/trigger.wav");
					SetCreateSoundTimeStretchRate(1.f);
				}
				/*パーツデータをロード*/
				if (Preset != "") {
					Load(Preset);
				}
				else {
					CreateData();
				}
				//読み込み
				mine_Gun->Load_Gun(save_parts, GunPartses);
				//セーブデータのクリーンアップ
				{
					bool sav = true;
					int cnt = 0;
					do {
						sav = false;
						for (auto&s : save_parts) {
							if (s.type_ != EnumGunParts::SIGHT) {
								if (erase_savedata(mine_Gun->Get_parts(s.type_))) {
									save_parts.erase(save_parts.begin() + (&s - &save_parts.front()));
									sav = true;
									break;
								}
							}
							else if (cnt < mine_Gun->Get_sight_size()) {
								auto* stt = mine_Gun->Get_parts(s.type_, cnt);
								cnt++;
								if (erase_savedata(stt) || s.cang_ > 10) {
									save_parts.erase(save_parts.begin() + (&s - &save_parts.front()));
									sav = true;
									break;
								}
							}
						}
					} while (sav);
				}
				if (mine_Gun->Get_gun_stat_now()->magazine_pushback(mine_Gun->Get_parts(EnumGunParts::MAGAZINE)->thisparts)) {
					mine_Gun->Set_in_chamber(true);
				}
			}
			bool erase_savedata(PLAYERclass::GunControl::g_parts* parts) {
				return ((parts == nullptr) || (!parts->IsActive()));
			}
			bool Update(void) noexcept override {
				TEMPSCENE::Update();
				bool changef = false;
				//演算
				{
					{
						moves tmp;
						mine_Gun->Set_move_gun(tmp);
						mine_Gun->Put_gun();
						mine_Gun->Set_Magazine();
						//キー
						up.GetInput(key_.wkey);
						down.GetInput(key_.skey);
						left.GetInput(key_.akey);
						right.GetInput(key_.dkey);
						shot.GetInput(key_.shoot);
						Rot.GetInput(key_.select);
						//
						if (Start_b) {
							Start_b = false;
							//changef = true;
							int pp = mine_Gun->Get_parts(EnumGunParts::BASE)->thisparts->Select_Chose(EnumSELECTER::SEMI);
							if (pp != -1) {
								mine_Gun->Set_selecter((EnumSELECTER)pp);
							}
						}
						if (left.trigger()) {
							P_select--;
							change_select = 0;
							changef = true;
						}
						if (right.trigger()) {
							++P_select;
							change_select = 0;
							changef = true;
						}
						ControlSel();
						if (up.trigger()) {
							change_select--;
							changef = true;
						}
						if (down.trigger()) {
							++change_select;
							changef = true;
						}
						if (change_select < 0) { change_select = change_select_max - 1; }
						if (change_select > change_select_max - 1) { change_select = 0; }
					}
					if (changef) {
						{
							Rot.first = false;
							port_ptr = nullptr;
							P_select_max = 0;
							//サイト判定
							sight_ptr.resize(5);
							for (auto& s : sight_ptr) {
								s = nullptr;
							}
							sight_p_s = 0;
						}
						Set_Parts_Vital(EnumGunParts::BASE, EnumAttachPoint::MAGAZINE_BASE, EnumGunParts::MAGAZINE);			//magazine
						Set_Parts_Vital(EnumGunParts::BASE, EnumAttachPoint::GRIP_BASE, EnumGunParts::GRIP);					//grip
						Set_Parts_Vital(EnumGunParts::BASE, EnumAttachPoint::UPER_HANDGUARD, EnumGunParts::UPER_HANDGUARD);		//uperhandguard
						Set_Parts_Vital(EnumGunParts::BASE, EnumAttachPoint::UNDER_HANDGUARD, EnumGunParts::UNDER_HANDGUARD);	//underhandguard
						Set_Parts_Optional(EnumGunParts::UNDER_HANDGUARD, EnumAttachPoint::UNDER_RAIL, EnumGunParts::FOREGRIP);	//foregrip
						Set_Parts_Optional(EnumGunParts::UNDER_HANDGUARD, EnumAttachPoint::LEFTSIDE_RAIL, EnumGunParts::LAM);	//lam
						Set_Parts_Optional(EnumGunParts::BASE, EnumAttachPoint::SIDEMOUNT_BASE, EnumGunParts::MOUNT_BASE);	//sidemount
						Set_Parts_Optional(EnumGunParts::MOUNT_BASE, EnumAttachPoint::SIDEMOUNT, EnumGunParts::MOUNT);		//sidemount_2
						Set_Parts_Optional(EnumGunParts::BASE, EnumAttachPoint::MAZZULE_BASE, EnumGunParts::MAZZULE);		//マズル
						Set_Parts_Optional(EnumGunParts::BASE, EnumAttachPoint::DUSTCOVER_BASE, EnumGunParts::DUSTCOVER);	//ダストカバー
						Set_Parts_Optional(EnumGunParts::BASE, EnumAttachPoint::STOCK_BASE, EnumGunParts::STOCK);			//ストック
						//sight
						{
							int ssp = P_select_max;
							for (auto& s : sight_ptr) {
								Set_Parts_Sight(s, (P_select_max - ssp + 1));
							}
						}
						//
						{
							change_per = 1.f;

							SE.Get(EnumSound::Assemble).Play(0, DX_PLAYTYPE_BACK, TRUE);

							xrad_t = viewparts_buf.y() * 1000.f;
							yrad_t = ((viewparts_buf.x() > 0.f) ? 90.f - viewparts_buf.z() * 100.f : -90.f + viewparts_buf.z() * 100.f);
						}
					}
					//
					easing_set(&mine_Gun->Get_gunanime_trigger()->per, float(shot.press()), 0.5f);
					if (shot.trigger()) {
						if (mine_Gun->Set_Flag_gun()) {
							//todo ディレイつける
							shot_se.play(DX_PLAYTYPE_BACK, TRUE);
							slide_se.play(DX_PLAYTYPE_BACK, TRUE);
							trigger_se.play(DX_PLAYTYPE_BACK, TRUE);
							//薬莢
							mine_Gun->Create_Cart();
							//エフェクト
							mine_Gun->calc_shot_effect(this);
						}
					}
					mine_Gun->Set_select_anime();				//銃セレクターアニメ
					mine_Gun->Set_shot_anime(rate, true);		//銃発砲アニメ
					mine_Gun->Update_Gun(MAPPTs, rate);			//銃更新
					mine_Gun->Set_LightHandle();					//ライト
					mine_Gun->IsDrawmagazine = true;
					Update_Effect();							//エフェクトの更新
				}
				//campos,camvec,camupの指定
				{}
				{
					{
						int x_m, y_m;
						GetMousePoint(&x_m, &y_m);
						x_m -= DrawPts->disp_x / 2;
						y_m -= DrawPts->disp_y / 2;
						SetMousePoint(DrawPts->disp_x / 2, DrawPts->disp_y / 2);
						SetMouseDispFlag(FALSE);
						xrad_t = std::clamp(xrad_t + float(std::clamp(y_m, -60, 60)) * 0.1f, -80.f, 80.f);
						yrad_t += float(std::clamp(x_m, -60, 60)) * 0.1f;
						yrad_t = rad2deg(atan2f(sin(deg2rad(yrad_t)), cos(deg2rad(yrad_t))));
					}
					{
						if (Rot.on()) {
							viewparts.clear();
						}
						else {
							viewparts = viewparts_buf;
						}
					}
					range_tgt = std::hypotf(sin(deg2rad(yrad_t)) * 0.25f, cos(deg2rad(yrad_t)) * (std::abs((((std::abs(yrad_t) > 90) ? mine_Gun->Get_maz().z() : 0.5f) - viewparts.z())) + camera_main.near_ * 2.f));
					range_t = (Rot.on() && !changef) ? std::clamp(range_t - float(GetMouseWheelRotVol()) * 0.1f, range_tgt, 5.f) : range_tgt;

					easing_set(&camparts, VECTOR_ref::vget(cos(deg2rad(xrad_t)) * sin(deg2rad(yrad_t)), sin(deg2rad(xrad_t)), cos(deg2rad(xrad_t)) * cos(deg2rad(yrad_t))) * range_t, 0.8f);
					camera_main.camvec = viewparts;
					camera_main.campos = camera_main.camvec + camparts;
					camera_main.camup = VECTOR_ref::up();
				}
				if (key_.jamp) {
					SE.Get(EnumSound::Assemble).Play(0, DX_PLAYTYPE_BACK, TRUE);
					//セーブデータのクリーンアップ
					{
						bool sav = true;
						int cnt = 0;
						do {
							sav = false;
							for (auto&s : save_parts) {
								if (s.type_ != EnumGunParts::SIGHT) {
									if (erase_savedata(mine_Gun->Get_parts(s.type_))) {
										save_parts.erase(save_parts.begin() + (&s - &save_parts.front()));
										sav = true;
										break;
									}
								}
								else if (cnt < mine_Gun->Get_sight_size()) {
									auto* stt = mine_Gun->Get_parts(s.type_, cnt);
									cnt++;
									if (erase_savedata(stt) || s.cang_ > 10) {
										save_parts.erase(save_parts.begin() + (&s - &save_parts.front()));
										sav = true;
										break;
									}
								}
							}
						} while (sav);
					}
					return false;
				}
				return true;
				//
			}
			void Dispose(void) noexcept override {
				//データセーブ
				Save(save_tgt);
				mine_Gun->Get_gun_stat_now()->Reset();
				shot_se.Dispose();
				slide_se.Dispose();
				trigger_se.Dispose();
			}
			void UI_Draw(void) noexcept  override {
				UIparts->UI_Draw(GunPartses, parts_cat, Rot.on(), mine_Gun, parts_p, change_per);
				easing_set(&change_per, 0.f, 0.5f);
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				mine_Gun->Draw_Gun_Common();
			}
			void Shadow_Draw(void) noexcept override {
				mine_Gun->Draw_Gun_Common();
			}
			void Main_Draw(void) noexcept override {
				mine_Gun->Draw_Gun_Common();
				mine_Gun->Draw_LAM_Effect(light);
			}
			void KeyOperation_VR(void) noexcept override {
#ifdef _USE_OPENVR_
				if (DrawPts->get_hand1_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand1();
					if (ptr_->turn && ptr_->now) {
						key_.shoot = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);																					//射撃
						key_.select = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() > 0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);	//セレクター
					}
				}
				if (DrawPts->get_hand2_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand2();
					if (ptr_->turn && ptr_->now) {
						key_.wkey = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.y() > 0.5f && ptr_->touch.x() < 0.5f && ptr_->touch.x() > -0.5f);		//running
						key_.skey = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.y() < -0.5f && ptr_->touch.x() < 0.5f && ptr_->touch.x() > -0.5f);		//running
						key_.dkey = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() > 0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);		//running
						key_.akey = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() < -0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);		//running
						key_.jamp = ((ptr_->on[0] & BUTTON_SIDE) != 0);									//jamp
					}
				}
#endif // _USE_OPENVR_
			}
			void KeyOperation(void) noexcept override {
				//設定
				KeyBind->Set_Mode(1);
				key_.wkey = KeyBind->Get_key_use(EnumKeyBind::FRONT);
				key_.skey = KeyBind->Get_key_use(EnumKeyBind::BACK);
				key_.dkey = KeyBind->Get_key_use(EnumKeyBind::RIGHT);
				key_.akey = KeyBind->Get_key_use(EnumKeyBind::LEFT);
				key_.jamp = KeyBind->Get_key_use(EnumKeyBind::JUMP);
				key_.shoot = KeyBind->Get_mouse_use(EnumMouseBind::SHOOT);
				key_.select = KeyBind->Get_mouse_use(EnumMouseBind::CHANGE_SEL);
			}
		};
		//
		class MAINLOOP : public TEMPSCENE {
#define USE_TPS

			//TPS操作
			class TPS_parts : public PTR_COMMON {
				float xr_cam = 0.f;
				float yr_cam = 0.f;
				//カメラ
				cam_info camera_TPS;
			public:
				switchs key_TPS;
				int sel_cam = 0;
				void Set(const float& fov_pc) noexcept {
					this->key_TPS.Init(false);
					this->camera_TPS.campos = VECTOR_ref::vget(0, 1.8f, -10);
					this->camera_TPS.set_cam_info(deg2rad(fov_pc), 0.1f, 200.f);
				}
				void Set_info(std::vector<std::shared_ptr<PLAYERclass::PLAYER_CHARA>>& chara) noexcept {
					if (this->key_TPS.on()) {
						//cam
						for (int i = 0; i < std::min<size_t>(chara.size(), 10); ++i) {
							if (CheckHitKey(KEY_INPUT_1 + i) != 0) {
								this->sel_cam = i;
							}
						}
						//pos
						{
							if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() - 10.f / FPS * cos(this->yr_cam));
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() + 10.f / FPS * sin(this->yr_cam));
							}
							if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() + 10.f / FPS * cos(this->yr_cam));
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() - 10.f / FPS * sin(this->yr_cam));
							}
							if (CheckHitKey(KEY_INPUT_UP) != 0) {
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() + 10.f / FPS * cos(this->yr_cam));
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() + 10.f / FPS * sin(this->yr_cam));
							}
							if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() - 10.f / FPS * cos(this->yr_cam));
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() - 10.f / FPS * sin(this->yr_cam));
							}
							this->camera_TPS.campos.x(std::clamp(this->camera_TPS.campos.x(), MAPPTs->map_col_get().mesh_minpos(0).x(), MAPPTs->map_col_get().mesh_maxpos(0).x()));
							this->camera_TPS.campos.z(std::clamp(this->camera_TPS.campos.z(), MAPPTs->map_col_get().mesh_minpos(0).z(), MAPPTs->map_col_get().mesh_maxpos(0).z()));
						}
						//rad
						{
							VECTOR_ref vec_2 = ((chara[this->sel_cam]->Get_Pos_Map() + VECTOR_ref::vget(0, 1.5f, 0)) - this->camera_TPS.campos).Norm();
							VECTOR_ref vec_z = (this->camera_TPS.camvec - this->camera_TPS.campos).Norm();
							VECTOR_ref vec_x = vec_z.cross(this->camera_TPS.camup);

							this->xr_cam -= deg2rad(int(vec_z.cross(vec_2).dot(vec_x) * 50)) * 0.1f;
							this->yr_cam -= deg2rad(int(vec_x.dot(vec_2) * 50)) * 0.1f;
							this->xr_cam = std::clamp(this->xr_cam, deg2rad(-89), deg2rad(89));
						}
						//
						this->camera_TPS.camvec = this->camera_TPS.campos + MATRIX_ref::Vtrans(VECTOR_ref::front(), MATRIX_ref::RotX(this->xr_cam) * MATRIX_ref::RotY(this->yr_cam));
						this->camera_TPS.camup = VECTOR_ref::up();
					}
				}
				const auto ON(void) const noexcept { return key_TPS.on(); }
				auto& Get_camera(void) noexcept { return camera_TPS; }
			};
			std::unique_ptr<TPS_parts> TPSparts;
			std::unique_ptr<HostPassEffect> Hostpassparts_TPS;
			//データ
			MV1 body_obj, body_obj_lag, body_col;								//身体モデル
			MV1 hit_pic;														//弾痕  
			GraphHandle light;													//ライトテクスチャ
			std::vector<Meds> meds_data;										//GUNデータ
			std::vector<Grenades> gres_data;									//GUNデータ
			std::vector<std::shared_ptr<PLAYERclass::PLAYER_CHARA>> chara;		//キャラ
			std::unique_ptr<UIclass::UI_MAINLOOP> UIparts;
			std::shared_ptr<RULE_parts> RULEparts;
			std::unique_ptr<MAPclass::MiniMap> MiniMAPPTs;

			std::vector<Vehcs> vehc_data;
			std::vector<std::shared_ptr<PLAYERclass::PLAYER_VEHICLE>> vehicle;
		public:
			std::shared_ptr<PLAYERclass::PLAYER_CHARA>& Get_Mine(void) noexcept { return this->chara[0]; }
			const bool CanGetMine(void) const noexcept { return this->chara.size() > 0; }
			std::vector<Meds>& Get_meds_data(void) noexcept { return this->meds_data; }
			std::vector<Grenades>& Get_gres_data(void) noexcept { return this->gres_data; }
		public:
			using TEMPSCENE::TEMPSCENE;
			void Awake(void) noexcept override {
				TEMPSCENE::Awake();
				TPSparts = std::make_unique<TPS_parts>();
				TPSparts->Set_Ptr_Common(MAPPTs, DrawPts);
				Hostpassparts_TPS = std::make_unique<HostPassEffect>(OPTPTs, deskx, desky);	//ホストパスエフェクト(フルスクリーン向け、TPS用)
				RULEparts = std::make_shared<RULE_parts>();
				UIparts = std::make_unique<UIclass::UI_MAINLOOP>(RULEparts);
				MiniMAPPTs = std::make_unique<MAPclass::MiniMap>(MAPPTs);					//ミニマップ
				light = GraphHandle::Load("data/light.png");								//ライト
				MV1::Load("data/model/hit/model.mv1", &this->hit_pic, true);				//弾痕モデル
				MV1::Load("data/model/body/model.mv1", &this->body_obj, true);				//身体モデル
				MV1::Load("data/model/body/col.mv1", &this->body_col, true);				//身体colモデル
				MV1::Load("data/model/body/model_lag.mv1", &this->body_obj_lag, true, -1.f);//身体ラグドール
				Meds::Set_Pre(&this->meds_data, "data/Items/medkit/");						//MEDデータ
				Grenades::Set_Pre(&this->gres_data, "data/Items/grenade/");					//グレネード
				Vehcs::Set_Pre(&this->vehc_data, "data/tank/");								//戦車
			}

			void Start(void) noexcept {
				for (auto& g : this->meds_data) { g.Set(&g - &this->meds_data.front()); }						//MEDデータ2
				for (auto& g : this->gres_data) { g.Set(&g - &this->gres_data.front()); }						//グレネード2
				for (auto& t : this->vehc_data) { t.Set(); }													//戦車2
			}
			void Ready_Chara(const size_t& spawn_total) noexcept {
				//キャラ設定
				this->chara.resize(spawn_total);
				for (auto& c : this->chara) {
					c = std::make_shared<PLAYERclass::PLAYER_CHARA>();
					c->Set_Ptr_Common(MAPPTs, DrawPts);
					c->Set_Ptr(&this->chara, &c, &this->vehicle, nullptr);
					c->Set(body_obj, body_obj_lag, body_col);
					c->Add_Guninfo(Gun_S, GunPartses);
				}
			}
			void Ready_Tank(const size_t& spawn_total) {
				//戦車設定
				vehicle.resize(spawn_total);
				for (auto& v : this->vehicle) {
					v = std::make_shared<PLAYERclass::PLAYER_VEHICLE>();
					v->Set_Ptr_Common(MAPPTs, DrawPts);
					v->Set_Ptr(&this->chara, nullptr, &this->vehicle, &v);
					v->Set(&vehc_data[/*(&v - &this->vehicle.front())% vehc_data.size()*/0], hit_pic);
				}
			}
		public:
			void Set(void) noexcept override {
				TEMPSCENE::Set_EnvLight(MAPPTs->map_col_get().mesh_maxpos(0), MAPPTs->map_col_get().mesh_minpos(0), VECTOR_ref::vget(0.5f, -0.5f, 0.5f), GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				TEMPSCENE::Set();
				MAPPTs->Start_Ray(Get_Light_vec());
				SetMousePoint(DrawPts->disp_x / 2, DrawPts->disp_y / 2);											//
				//初回スポーン位置設定
				moves temp;
				for (auto& c : this->chara) {
					auto& wp = MAPPTs->Get_spawn_point()[&c - &this->chara.front()];
					temp.pos = wp;
					temp.mat = MATRIX_ref::RotY(atan2f(wp.x(), wp.z()));
					c->Set_SpawnPos(temp);
					c->Spawn(false, (c == Get_Mine()), Gun_S, GunPartses);
					c->Start();
				}
				for (auto& v : this->vehicle) {
					auto& wp = MAPPTs->Get_spawn_point()[&v - &this->vehicle.front()];
					temp.pos = wp;
					temp.mat = MATRIX_ref::RotY(-atan2f(wp.x(), wp.z()));
					v->Set_SpawnPos(temp);
					v->Spawn();
					v->Start();
					//弾薬設定
					v->SetUp_bullet(MAPPTs, DrawPts);
					v->Set_bullet_Ptr();
				}
				/*
				for (auto& c : this->chara) {
					c->Ride_on(&this->vehicle[&c - &this->chara.front()]);
				}
				*/
				UIparts->Set_Ptr_Common(MAPPTs, DrawPts);	//UI
				TPSparts->Set(OPTPTs->Get_Fov());			//TPS
				RULEparts->Set();							//ルール
				MAPPTs->Set();								//環境
			}
			bool Update(void) noexcept override {
				TEMPSCENE::Update();
				//アイテム演算
				{
					auto& item = MAPPTs->item;
					for (auto& g : item) { g->Update(item, MAPPTs->Map_col_line); }
					//時間が経ったマガジンを削除する
					bool fal = true;
					while (true) {
						fal = true;
						for (auto& i : item) {
							auto now = &i - &item.front();
							if (i->Detach_mag()) {
								std::iter_swap(item.begin() + now, item.end() - 1);
								item.pop_back();
								fal = false;
								break;
							}
							if (i->Detach_gre(this->chara[0], this->chara, MAPPTs->Map_col_line)) {
								std::iter_swap(item.begin() + now, item.end() - 1);
								item.pop_back();
								fal = false;
								break;
							}
						}
						if (fal) {
							break;
						}
					}
				}
				//共通演算//2～3ms
				for (auto& c : this->chara) {
					c->Update(RULEparts->Get_Playing(), this->camera_main.fov / this->fov_base, this->meds_data, this->gres_data, &c == &this->Get_Mine(), Gun_S, GunPartses);
				}
				for (auto& g : Gun_S) {
					g->Update_Gun_Physics(MAPPTs);
				}
				for (auto& v : this->vehicle) {
					v->Update(this->camera_main, this->camera_main.fov / this->fov_base);
				}
				MAPPTs->world->Step(1.f, 1, 1);
				for (auto& v : this->vehicle) {
					v->Update_after();
				}
				//campos,camvec,camupの指定
				this->Get_Mine()->Setcamera(this->camera_main, this->fov_base);
				this->camera_main.camup = MATRIX_ref::Vtrans(this->camera_main.camup, MATRIX_ref::RotAxis((this->camera_main.camvec - this->camera_main.campos), deg2rad(20.f * ratio_bless() * sin(bless))));

				//ルール保存
				UIparts->Update();
				RULEparts->Update();
				//TPS描画
				TPSparts->Set_info(this->chara);
				TPSparts->key_TPS.GetInput(key_.TPS);
				if (TPSparts->ON()) {
					//影用意
					DrawPts->Ready_Shadow(TPSparts->Get_camera().campos, [&] {Shadow_Draw(); }, [&] {Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.f, 2.5f, 2.f), VECTOR_ref::vget(15.f, 12.5f, 15.f));
					//書き込み
					Hostpassparts_TPS->BUF_Draw([&] { BG_Draw(); }, [&] {DrawPts->Draw_by_Shadow([&] {Main_Draw(); }); }, TPSparts->Get_camera(), false);	//被写体深度描画
					Hostpassparts_TPS->Set_MAIN_Draw();																												//最終描画
				}
				//表示の如何
				{
					cam_info* camp = TPSparts->ON() ? &TPSparts->Get_camera() : &this->camera_main;
					GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camp->campos, camp->camvec, camp->camup, camp->fov, camp->near_, camp->far_);
					{
						for (auto& c : this->chara) {
							c->Check_CameraViewClip(false);
						}
						for (auto& i : MAPPTs->item) {
							i->Check_CameraViewClip(false, MAPPTs->Map_col_line);
						}

						MAPPTs->Check_CameraViewClip(false);
					}
				}
				//終了判定
				if (RULEparts->Get_timer() <= -5.f) {
					return false;
				}
				return true;
			}
			void Dispose(void) noexcept override {
				for (auto& c : this->chara) {
					c->Dispose();
				}
				this->chara.clear();
				//
				for (auto& g : Gun_S) {
					g->Dispose_Gun();
					g->Dispose();
				}
				Gun_S.clear();
				//
				for (auto& v : this->vehicle) {
					v->Dispose();
				}
				this->vehicle.clear();
			}
			void UI_Draw(void) noexcept  override {
				UIparts->UI_Draw(this->Get_Mine(), KeyBind);
				//ミニマップ
				MiniMAPPTs->UI_Draw(this->chara, (TPSparts->ON()) ? this->chara[TPSparts->sel_cam] : this->Get_Mine());
			}
			void BG_Draw(void) noexcept override {
				//map
				MAPPTs->sky_Draw();
			}
			void Shadow_Draw_Far(void) noexcept override {
				//map
				MAPPTs->Shadow_Draw_Far();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				//キャラ
				for (auto& c : this->chara) {
					if ((c->Get_pos() - GetCameraPosition()).size() > 1.f) {
						c->Draw_chara();
					}
				}
				for (auto& g : Gun_S) {
					g->Draw_Gun_Common();
				}
				for (auto& v : this->vehicle) {
					v->Draw();
				}
			}
			void Shadow_Draw(void) noexcept override {
				//map
				MAPPTs->item_Draw();
				//キャラ
				for (auto& c : this->chara) {
					if ((c->Get_pos() - GetCameraPosition()).size() <= 2.5f) {
						c->Draw_chara();
					}
				}
				for (auto& g : Gun_S) {
					g->Draw_Gun_Common();
				}
				for (auto& v : this->vehicle) {
					v->Draw();
				}
			}
			void Main_Draw(void) noexcept override {
				//map
				MAPPTs->main_Draw();
				//命中痕
				hit_obj_p.Draw();
				hit_b_obj_p.Draw();
				//キャラ
				for (auto& c : this->chara) {
					c->Draw_chara();
					c->Draw_LAM_Effect();				//レーザー
				}
				for (auto& g : Gun_S) {
					g->Draw_Gun_Common();
					g->Draw_LAM_Effect(light);				//レーザー
				}
				for (auto& v : this->vehicle) {
					v->Draw();
					if (v->MINE_c == &this->Get_Mine()) {
						v->Draw_LAM_Effect();//レーザー
					}
				}
				//銃弾
				SetFogEnable(FALSE);
				SetUseLighting(FALSE);
				for (auto& g : Gun_S) {
					g->Draw_ammo();
				}
				for (auto& v : this->vehicle) { v->Draw_ammo(); }
				SetUseLighting(TRUE);
				SetFogEnable(TRUE);
				//
			}
			void Item_Draw(void) noexcept override {
				TEMPSCENE::Item_Draw();
				//
				use_lens = false;
				if (this->Get_Mine()->Damage.Get_alive()) {
					lens_zoom = this->Get_Mine()->Set_Gun_().DrawReticle_UI(this->Get_Mine()->is_ADS());
					lens_size = this->Get_Mine()->Set_Gun_().Get_reticle_size();
					if (lens_zoom > 1.0f) {
						use_lens = this->Get_Mine()->is_ADS();
					}
				}

				//歪み出し
				if (!use_bless && this->Get_Mine()->Get_gre_eff()) {
					use_bless = true;
					bless = 0.f;
					bless_ratio = 0.15f;
				}
				this->Get_Mine()->Set_gre_eff(false);
				for (auto& g : Gun_S) { g->Set_Draw_bullet(); }
				for (auto& v : this->vehicle) { v->Set_Draw_bullet(); }
				UIparts->item_Draw(this->chara, this->vehicle, this->Get_Mine());
			}
			void LAST_Draw(void) noexcept override {
				//TPS視点
				if (TPSparts->ON()) {
					Hostpassparts_TPS->MAIN_Draw();
				}
				//minimap
				MiniMAPPTs->Draw();
			}

			void KeyOperation_VR(void) noexcept override {
#ifdef _USE_OPENVR_
				if (DrawPts->get_hand1_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand1();
					if (ptr_->turn && ptr_->now) {
						key_.shoot = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);																					//射撃
						key_.reload = ((ptr_->on[0] & BUTTON_SIDE) != 0);																						//マグキャッチ
						key_.select = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_->touch.x() > 0.5f && ptr_->touch.y() < 0.5f && ptr_->touch.y() > -0.5f);	//セレクター
					}
				}
				if (DrawPts->get_hand2_num() != -1) {
					auto ptr_ = DrawPts->get_device_hand2();
					if (ptr_->turn && ptr_->now) {
						key_.have_mag = ((ptr_->on[0] & BUTTON_TRIGGER) != 0);		//マガジン持つ
						key_.have_item = ((ptr_->on[0] & BUTTON_TOPBUTTON_B) != 0);	//アイテム取得
						key_.sort_magazine = false;									//
						key_.view_gun = false;										//
						key_.drop_ = false;											//
						key_.throw_gre = false;										//
						key_.running = ((ptr_->on[0] & BUTTON_TOUCHPAD) != 0);		//running
						key_.jamp = ((ptr_->on[0] & BUTTON_SIDE) != 0);				//jamp
					}
				}
				Get_Mine()->Set_key_() = key_;
#endif // _USE_OPENVR_
			}
			void KeyOperation(void) noexcept override {
				//設定
				KeyBind->Set_Mode(0);
				key_.have_mag = true;
				key_.wkey = KeyBind->Get_key_use(EnumKeyBind::FRONT);
				key_.skey = KeyBind->Get_key_use(EnumKeyBind::BACK);
				key_.dkey = KeyBind->Get_key_use(EnumKeyBind::RIGHT);
				key_.akey = KeyBind->Get_key_use(EnumKeyBind::LEFT);
				key_.qkey = KeyBind->Get_key_use(EnumKeyBind::Q);
				key_.ekey = KeyBind->Get_key_use(EnumKeyBind::E);
				key_.reload = KeyBind->Get_key_use(EnumKeyBind::RELOAD);
				key_.have_item = KeyBind->Get_key_use(EnumKeyBind::GET_ITEM);
				key_.throw_gre = KeyBind->Get_key_use(EnumKeyBind::THROW_GRE);
				KeyBind->Get_key_use_ID(EnumKeyBind::SIT).Set_key(key_.squat);	//記憶
				key_.squat = KeyBind->Get_key_use(EnumKeyBind::SIT);
				key_.sort_magazine = KeyBind->Get_key_use(EnumKeyBind::SORT_MAG);
				key_.running = KeyBind->Get_key_use(EnumKeyBind::RUN);
				key_.jamp = KeyBind->Get_key_use(EnumKeyBind::JUMP);
				key_.TPS = KeyBind->Get_key_use(EnumKeyBind::CHANGE_VIEW);
				key_.view_gun = KeyBind->Get_key_use(EnumKeyBind::LOOKGUN);
				key_.drop_ = KeyBind->Get_key_use(EnumKeyBind::MED);
				key_.shoot = KeyBind->Get_mouse_use(EnumMouseBind::SHOOT);
				key_.select = KeyBind->Get_mouse_use(EnumMouseBind::CHANGE_SEL);
				key_.aim = KeyBind->Get_mouse_use(EnumMouseBind::AIM);
				key_.ride = KeyBind->Get_key_use(EnumKeyBind::RIDE_ON);

				Get_Mine()->Set_key_() = key_;
			}

		};
	};
};