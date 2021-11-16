#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	class Sceneclass {
	public:
		static float GetRandf(float m_arg) { return -m_arg + (float)(GetRand((int)(m_arg * 2.f * 10000.f))) / 10000.f; }
		static VECTOR_ref GetVector(float Xrad, float Yrad) { return VECTOR_ref::vget(-cos(deg2rad(Xrad))*sin(-deg2rad(Yrad)), sin(deg2rad(Xrad)), -cos(deg2rad(Xrad))*cos(-deg2rad(Yrad))); }
		class MAINLOOP;
		//
		class TEMPSCENE : public PTR_COMMON {
		private:
			VECTOR_ref Shadow_minpos;
			VECTOR_ref Shadow_maxpos;
			VECTOR_ref Light_vec;
			COLOR_F Light_color = GetColorF(0, 0, 0, 0);
		protected:
			//引き継ぐ
			std::shared_ptr<OPTION> OPTPTs;
			std::shared_ptr<HostPassEffect> HostpassPTs;
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
				std::shared_ptr<DXDraw>& DrawPts_t,
				const std::shared_ptr<OPTION>& OPTPTs_t,
				std::shared_ptr<HostPassEffect>& HostpassPTs_t
			) noexcept {
				PTR_COMMON::Set_Ptr_Common(DrawPts_t);
				OPTPTs = OPTPTs_t;
				HostpassPTs = HostpassPTs_t;
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
					bless += deg2rad(float(100 + GetRand(600)) / 100.f * Frame_Rate / FPS);
					bless_ratio -= (0.03f / 90.f) * Frame_Rate / FPS;
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
				DrawPts->Ready_Shadow(camera_main.campos, [&] { Shadow_Draw(); }, [&] { Shadow_Draw_NearFar(); }, VECTOR_ref::vget(100.f, 30.f, 100.f), VECTOR_ref::vget(camera_main.far_, 40.f, camera_main.far_));//MAIN_LOOPのnearはこれ (Get_Mine()->Damage.Get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
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
		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			class Cutinfo_Common {
				class On_Off {
				public:
					int On = 0;
					int Off = 0;
				};
			public:
				std::vector<On_Off> Switch;
				int nowcut = 0;
				bool a_switch = false;
				bool isFirstCut = false;

				bool Update_(int Cut) {
					if (this->Switch.size() > this->nowcut) {
						auto& inf_b = this->Switch[this->nowcut];
						isFirstCut = (Cut == inf_b.On);
						if (isFirstCut) {
							this->a_switch = true;
						}
						if (this->a_switch) {
							if (Cut > inf_b.Off) {
								this->a_switch = false;
								this->nowcut++;
								return true;
							}
						}
					}
					return false;
				}
			};
			class Cutinfo_g {
			public:
				Cutinfo_Common Blur;
				int animsel = -1;
				bool isloop = false;
				float animspeed = 0.f;
				MATRIX_ref mat_p;
				bool usemat = false;
				float startframe = 0.f;
				float PhysicsSpeed_ = -1.f;
				float OpacityRate = -1.f;
			};

			class ModelControl {
			public:
				class Model {
				private:
					void Sel_AnimNum(MV1&model, int sel) {
						for (auto& anim_t : model.get_anime()) {
							model.get_anime(&anim_t - &model.get_anime().front()).per = (&anim_t - &model.get_anime().front() == sel) ? 1.f : 0.f;
						}
					}
				public:
					bool isFar = false;
					bool IsNearShadow = true;
					bool IsFarShadow = false;
					bool isBase = true;
					std::string Path;

					size_t numBase = 0;
					float PhysicsSpeed = 1.f;

					float b_run = 0;
					float b_runrange = 0;

					MV1 obj;
					moves move;
					bool isDraw = false;

					Cutinfo_Common Cutinfo;
					std::vector<Cutinfo_g> Cutinfo_;//オンにするカット
					float OpacityRate = 1.f;
					int Anim_Sel = 0;
					void UpdateAnim(int ID, bool isloop, float speed) {
						Sel_AnimNum(this->obj, ID);
						this->obj.get_anime(ID).Update(isloop, speed);
					}
					void Update() {
						MV1SetOpacityRate(this->obj.get(), this->OpacityRate);
						if (this->isDraw) {
							this->obj.work_anime();
						}
					}
					void Draw() {
						if (this->isDraw && this->OpacityRate > 0.f) {
							this->obj.DrawModel();
						}
					}
					void SetPhysics(bool isReset) {
						if (this->isDraw) {
							if (isReset) {
								this->obj.PhysicsResetState();
							}
							else {
								this->obj.PhysicsCalculation(30.f / FPS * this->PhysicsSpeed);
							}
						}
					}
				};
			private:
				std::vector<Model> model;
				size_t Max = 0;
			public:
				ModelControl() {
					model.resize(64);
					Max = 0;
				}
				void Load_onAnime(std::string_view Path) {
					for (size_t i = 0; i < Max; i++) {
						if (model[i].isBase && model[i].Path == Path) {
							model[Max].Path = Path;
							model[Max].isBase = false;
							model[i].obj.DuplicateonAnime(&(model[Max].obj), &(model[Max].obj));
							Max++;
							return;
						}
					}
					model[Max].Path = Path;
					model[Max].isBase = true;
					MV1::LoadonAnime(model[Max].Path, &(model[Max].obj));
					Max++;
				}
				Model* Get(std::string_view Path, size_t Sel = 0) {
					int Cnt = 0;
					for (size_t i = 0; i < Max; i++) {
						if (model[i].Path == Path) {
							if (Cnt >= Sel) {
								return &(model[i]);
							}
							Cnt++;
						}
					}
					return nullptr;
				}
				void FirstUpdate(int Cut, bool isFirstLoop) {
					for (auto& m : model) {
						while (true) {
							bool tt = m.Cutinfo.Update_(Cut);
							m.isDraw = m.Cutinfo.a_switch;
							if (m.isDraw) {
								auto& inf = m.Cutinfo_[m.Cutinfo.nowcut];
								if (isFirstLoop && m.Cutinfo.isFirstCut) {
									//最初のアニメーション動作
									if (inf.usemat) {
										m.obj.SetMatrix(inf.mat_p);
									}
									if (inf.animsel >= 0 && inf.startframe >= 0.f) {
										m.obj.get_anime(inf.animsel).time = inf.startframe;
									}
									if (inf.PhysicsSpeed_ >= 0.f) {
										m.PhysicsSpeed = inf.PhysicsSpeed_;
									}
									if (inf.OpacityRate >= 0.f) {
										m.OpacityRate = inf.OpacityRate;
									}
								}
								//アニメーション動作
								if (inf.animsel >= 0) {
									m.UpdateAnim(inf.animsel, inf.isloop, inf.animspeed);
								}
							}
							if (tt) {
								continue;
							}
							break;
						}
					}
				}
				void SetPhysics(bool reset_p) {
					for (auto& m : model) {
						m.SetPhysics(reset_p);
					}
				}
				void Set() {
					for (auto& m : model) {
						m.isDraw = false;
					}
				}
				void Update() {
					for (auto& m : model) {
						m.Update();
					}
				}
				void Draw_Far() {
					auto fog_e = GetFogEnable();
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					{
						for (auto& m : model) {
							if (m.isFar) {
								m.Draw();
							}
						}
					}
					SetUseLighting(TRUE);
					SetFogEnable(fog_e);
				}
				void Draw(bool innearshadow, bool infarshadow) {
					if (infarshadow) {
						for (auto& m : model) {
							if (!m.isFar && m.IsFarShadow) {
								m.Draw();
							}
						}
					}
					else if (innearshadow) {
						for (auto& m : model) {
							if (!m.isFar && m.IsNearShadow) {
								m.Draw();
							}
						}
					}
					else {
						for (auto& m : model) {
							if (!m.isFar) {
								m.Draw();
							}
						}
					}
				}
			};
			class GraphControl {
				class Graph {
					class infos {
						class infos_rand {
						public:
							float Ans = 0;
							float Base = 0;
							float Per = 1.f;
							void Set(float m_p, float m_base, float m_per) {
								Ans = m_p;
								Base = m_base;
								Per = m_per;
							}
							void Set(float m_base, float m_per) {
								Base = m_base;
								Per = m_per;
							}
							void UpdateRand() {
								easing_set(&this->Ans, GetRandf(this->Base), this->Per);
							}
						};
					public:
						float Ans = 0;
						float Base = 0;
						float Per = 1.f;
						infos_rand Rand_;
						void Set(float m_p, float m_base, float m_per) {
							Ans = m_p;
							Base = m_base;
							Per = m_per;
							Rand_.Set(0, 0, 1.f);
						}
						void Set(float m_base, float m_per) {
							Base = m_base;
							Per = m_per;
						}
						/*
						void Set(float m_p, float m_base, float m_per, float m_rand, float m_randbase, float m_randper) {
							Ans = m_p;
							Base = m_base;
							Per = m_per;
							Rand_.Set(m_rand, m_randbase, m_randper);
						}
						*/
						void Update() {
							Rand_.UpdateRand();
							easing_set(&this->Ans, this->Base + this->Rand_.Ans, this->Per);
						}
					};
				private:
					std::vector<Graph> blur_t;
					GraphHandle handle;
					int Bright_R = 255;
					int Bright_G = 255;
					int Bright_B = 255;
					bool isblurstart = false;
				public:
					infos X;
					infos Y;
					infos Rad;
					infos Scale;
					infos Alpha;
				public:
					bool isDraw = false;
					bool isblur = false;
					int xsize = -1;
					int ysize = -1;
				public:
					Cutinfo_Common Cutinfo;
					std::vector<Cutinfo_g> Cutinfo_;//オンにするカット
					std::string Path;
					void SetBright(int b_r, int b_g, int b_b) {
						Bright_R = b_r;
						Bright_G = b_g;
						Bright_B = b_b;
					}
					void Set(float xp, float yp, float rd, float al, float sc, std::string_view Path_t) {
						this->X.Set(xp, 0.f, 1.f);
						this->Y.Set(yp, 0.f, 1.f);
						this->Rad.Set(rd, 0.f, 1.f);
						this->Alpha.Set(al, 1.f, 1.f);
						this->Scale.Set(sc, 0.f, 1.f);
						this->isDraw = false;
						this->isblur = false;
						this->handle = GraphHandle::Load(Path_t);
						this->handle.GetSize(&xsize, &ysize);
						this->blur_t.resize(15);
					}
					void Set_Base(float xp, float xper, float yp, float yper, float rd, float scl, float rdper, float sclper, float alp, float alpper) {
						this->X.Base = xp;
						this->X.Per = xper;

						this->Y.Base = yp;
						this->Y.Per = yper;

						this->Rad.Base = rd;
						this->Rad.Per = rdper;

						this->Scale.Base = scl;
						this->Scale.Per = sclper;

						this->Alpha.Base = alp;
						this->Alpha.Per = alpper;
					}
					void Set_Rand(float xp, float xper, float yp, float yper, float rd, float rdper, float scl, float sclper) {
						this->X.Rand_.Set(xp, xper);
						this->Y.Rand_.Set(yp, yper);
						this->Rad.Rand_.Set(deg2rad(rd), rdper);
						this->Scale.Rand_.Set(scl, sclper);
					}
					void Update() {
						this->X.Update();
						this->Y.Update();
						this->Rad.Update();
						this->Scale.Update();
						this->Alpha.Update();
						if (this->isblur) {
							if (!this->isblurstart) {
								this->isblurstart = true;
								for (int i = 0; i < this->blur_t.size(); i++) {
									this->blur_t[i].X.Ans = this->X.Ans;
									this->blur_t[i].Y.Ans = this->Y.Ans;
									this->blur_t[i].Rad.Ans = this->Rad.Ans;
									this->blur_t[i].isDraw = false;
								}
							}
							for (int i = 0; i < (int)(this->blur_t.size()) - 1; i++) {
								this->blur_t[i].X.Ans = this->blur_t[i + 1].X.Ans;
								this->blur_t[i].Y.Ans = this->blur_t[i + 1].Y.Ans;
								this->blur_t[i].Rad.Ans = this->blur_t[i + 1].Rad.Ans;
								this->blur_t[i].Scale.Ans = this->blur_t[i + 1].Scale.Ans;
								this->blur_t[i].Alpha.Ans = (float)(i + 1) / (float)(this->blur_t.size()) * this->Alpha.Ans;
								this->blur_t[i].isDraw = this->blur_t[i + 1].isDraw;
							}
							this->blur_t.back().X.Ans = this->X.Ans;
							this->blur_t.back().Y.Ans = this->Y.Ans;
							this->blur_t.back().Rad.Ans = this->Rad.Ans;
							this->blur_t.back().Scale.Ans = this->Scale.Ans;
							this->blur_t.back().isDraw = this->isDraw;
						}
						else {
							this->isblurstart = false;
						}
					}
					void Draw_Common(GraphHandle& m_handle, float scale_) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*this->Alpha.Ans));
						m_handle.DrawRotaGraph((int)(this->X.Ans), (int)(this->Y.Ans), scale_*this->Scale.Ans, this->Rad.Ans, true);
					}
					void Draw(float scale_) {
						if (this->isDraw && this->Alpha.Ans > 0.f) {
							SetDrawBright(Bright_R, Bright_G, Bright_B);
							if (this->isblur) {
								for (int i = 0; i < this->blur_t.size(); i++) {
									if (this->blur_t[i].isDraw) {
										this->blur_t[i].Draw_Common(this->handle, scale_);
									}
								}
							}
							Draw_Common(this->handle, scale_);
							SetDrawBright(255, 255, 255);
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						}
					}
					//
				};
				std::vector<Graph> model;
				size_t Max = 0;
			public:
				GraphControl() {
					model.resize(64);
					Max = 0;
				}
				void Load(float xp, float yp, float rd, float al, float sc, std::string_view Path) {
					model[Max].Path = Path;
					model[Max].Set(xp, yp, rd, al, sc, Path);
					Max++;
				}
				Graph* Get(std::string_view Path, size_t Sel = 0) {
					int Cnt = 0;
					for (size_t i = 0; i < Max; i++) {
						if (model[i].Path == Path) {
							if (Cnt >= Sel) {
								return &(model[i]);
							}
							Cnt++;
						}
					}
					return nullptr;
				}
				void FirstUpdate(int Cut, bool isFirstLoop) {
					for (auto& m : model) {
						while (true) {
							bool tt = m.Cutinfo.Update_(Cut);
							m.isDraw = m.Cutinfo.a_switch;
							if (m.isDraw) {
								//ブラー操作
								m.Cutinfo_[m.Cutinfo.nowcut].Blur.Update_(Cut);
								m.isblur = m.Cutinfo_[m.Cutinfo.nowcut].Blur.a_switch;
								if (isFirstLoop && m.Cutinfo.isFirstCut) {
									//最初のアニメーション動作

								}
								//アニメーション動作
							}
							if (tt) {
								m.isblur = false;
								continue;
							}
							break;
						}
					}
				}
				void Set() {
					for (auto& m : model) {
						m.isDraw = false;
					}
				}
				void Update() {
					for (auto& m : model) {
						m.Update();
					}
				}
				void Draw(int Disp_y) {
					for (auto& m : model) {
						m.Draw((float)Disp_y / m.ysize);
					}
				}
			};
			class Cut_Info {
			public:
				cam_info Aim_camera;
				float cam_per = 0.0f;
				LONGLONG TIME = 0;
				bool isNextreset = false;

				Cut_Info() {
					Aim_camera.campos = VECTOR_ref::vget(0, 10, -30);
					Aim_camera.camvec = VECTOR_ref::vget(0, 10, 0);
					Aim_camera.camup = VECTOR_ref::up();
					Aim_camera.set_cam_info(deg2rad(15), 1.0f, 300.f);
					cam_per = 0.95f;
					isNextreset = false;
				}

				void Dispose() {

				}
			};
			class Cut_tex {
				int xpos = 0;
				int ypos = 0;
				int size = 0;
				int LMR = 1;
				std::string str;
				LONGLONG START_TIME = 0;
				LONGLONG END_TIME = 0;
			public:
				Cut_tex() {
					xpos = 0;
					ypos = 0;
					size = 12;
					str = "test";
					START_TIME = (LONGLONG)(1000000.f * 0.01f);
					END_TIME = (LONGLONG)(1000000.f * 1.01f);
				}
				void Set(int xp, int yp, int Fontsize, std::string_view mag, LONGLONG StartF, LONGLONG ContiF, int m_LMR) {
					this->xpos = xp;
					this->ypos = yp;
					this->size = Fontsize;
					this->str = mag;
					this->START_TIME = StartF;
					this->END_TIME = StartF + ContiF;;
					this->LMR = m_LMR;
				}
				void Draw(LONGLONG NowTime) {
					if (NowTime > this->START_TIME && NowTime < this->END_TIME) {
						switch (this->LMR)
						{
						case 0:
							Fonts.Get(this->size).Get_handle().DrawString(this->xpos, this->ypos, this->str, GetColor(255, 255, 255));
							break;
						case 1:
							Fonts.Get(this->size).Get_handle().DrawString_MID(this->xpos, this->ypos, this->str, GetColor(255, 255, 255));
							break;
						case 2:
							Fonts.Get(this->size).Get_handle().DrawString_RIGHT(this->xpos, this->ypos, this->str, GetColor(255, 255, 255));
							break;
						default:
							break;
						}
					}
				}
			};
			//スクリプト読み込み処理
			std::string func;
			std::vector<std::string> args;
			class ARG {
			public:
				std::string Base;
				std::string After;
			};
			std::vector<ARG> args2;
			static void Sub_Func(std::string& func_t, const char& in_str) {
				size_t str_switch = 0;
				size_t str_in = std::string::npos;
				bool in = false;
				while (true) {
					if (str_switch != std::string::npos) { str_switch = func_t.find('\"', str_switch + 1); in ^= 1; }
					str_in = func_t.find(in_str, str_in + 1);
					if (str_in != std::string::npos) {
						if (str_switch != std::string::npos && str_switch < str_in && in) {
							continue;
						}
						func_t = func_t.erase(str_in, 1);
						continue;
					}
					break;
				}
			}
			void LoadScript(std::string_view func_t) {
				args.clear();
				func = func_t;
				{
					// //を削除
					size_t sls = func.find("//");
					if (sls != std::string::npos) { func = func.substr(0, sls); }
					//いらない要素を排除
					Sub_Func(func, '{');
					Sub_Func(func, '}');
					Sub_Func(func, ' ');
					Sub_Func(func, '\t');
					Sub_Func(func, ';');
					Sub_Func(func, '\"');
				}
				//()と,で囲われた部分から引数を取得
				if (func != "") {
					std::string tmp_func = func;
					size_t left = tmp_func.find("(");
					size_t right = tmp_func.rfind(")");
					if (left != std::string::npos && right != std::string::npos) {
						tmp_func = tmp_func.substr(left + 1, right - 1 - left);
					}
					while (true) {
						size_t in_str = tmp_func.find(",");
						if (in_str == std::string::npos) {
							args.emplace_back(tmp_func);
							break;
						}
						else {
							std::string arg = tmp_func.substr(0, in_str);
							tmp_func = tmp_func.substr(in_str + 1);
							args.emplace_back(arg);
						}
					}
				}
			}
		private:
			std::string Tachyon = "data/umamusume/Tachyon/model.mv1";
			std::string Tachyon2 = "data/umamusume/Tachyon2/model.mv1";
			std::string Cafe = "data/umamusume/Cafe/model.mv1";
			std::string Karen = "data/umamusume/karen/model.mv1";
			std::string Mobu = "data/umamusume/mobu_black/model.mv1";
			std::string GATE = "data/model/map/model_gate.mv1";
			std::string SHIP = "data/model/ship/model.mv1";
			std::string SUN = "data/model/sun/model.mv1";

			std::string FACE = "data/picture/Cut.png";
			std::string NEWSP = "data/picture/Cut2.png";
			std::string SODE1 = "data/picture/sode1.png";
			std::string SODE2 = "data/picture/sode2.png";
			std::string SODE3 = "data/picture/sode3.png";
			std::string SODE4 = "data/picture/sode4.png";
			std::string SODE5 = "data/picture/sode5.png";
			std::string SODE6 = "data/picture/sode6.png";
			std::string ANIME1 = "data/picture/anime0.png";
			std::string ANIME2 = "data/picture/anime1.png";
			std::string FIRST1 = "data/picture/FIRST.png";
			std::string FLASH1 = "data/picture/sun.png";
			std::string LOGO1 = "data/picture/logo.png";

			float per_logo = 1.f;
			//データ
			ModelControl models;
			GraphControl graphs;
			SoundHandle BGM;
			size_t Cut = 0;
			std::vector<Cut_Info> Cut_Pic;
			std::vector<Cut_tex> Texts;
			LONGLONG BaseTime = 0;
			//GraphHandle movie;
			bool reset_p = true;
			bool isNextreset = true;//カット終了時に物理をリセットするかどうか
			bool isFirstLoop = true;//カット最初のループ化取得
			//
			float radc = 0.f;
			float xradadd_r = 0.f;
			float spd_takion = 0.6f;
			bool return_walk = false;
			float Gate_Xpos = 0;
			float Box_ALPHA = 0.f;
			float camzb_28 = 0.f;
			float camyup = 0.f;
			float camzb_41 = 0.f;
			float bright_43 = 0.f;
			float ship_z = 0.f;
			float ship_zadd = 0.f;
			float cam_yure = 0.f;
			VECTOR_ref rand_vp;
		private:
			bool Time_Over() { return Cut >= Cut_Pic.size(); }
		public:
			using TEMPSCENE::TEMPSCENE;
			void Awake(void) noexcept override {
				TEMPSCENE::Awake();
				//
				camera_main.campos = VECTOR_ref::vget(0, 20, -20);
				camera_main.camvec = VECTOR_ref::vget(0, 20, 0);
				camera_main.camup = VECTOR_ref::up();
				camera_main.set_cam_info(deg2rad(15), 1.0f, 200.f);
				//
				SetUseASyncLoadFlag(FALSE);
				{
					{
						//newsp
						graphs.Load(0, 0, 0, 1.f, 1.f, FACE);
						graphs.Load(0, 0, 0, 1.f, 1.5f, NEWSP);
						//anim
						graphs.Load((float)(y_r(1920 * 1 / 2)), (float)(y_r(1080 * 1 / 2)), 0, 1.f, 1.f, ANIME1);
						graphs.Load((float)(y_r(1920 * 1 / 2)), (float)(y_r(1080 * 3 / 2)), 0, 1.f, 1.f, ANIME2);
						//sode
						graphs.Load((float)(y_r(1920 * 7 / 10)), (float)(y_r(1080 * 3 / 2)), 0, 1.f, 1.f, SODE6);
						graphs.Load((float)(y_r(1920 * 3 / 10)), (float)(y_r(1080 * 3 / 2)), 0, 1.f, 1.f, SODE5);
						graphs.Load((float)(y_r(1920 * 7 / 10)), (float)(y_r(1080 * 3 / 2)), 0, 1.f, 1.f, SODE6);
						graphs.Load((float)(y_r(1920 * 3 / 10)), (float)(y_r(1080 * 3 / 2)), 0, 1.f, 1.f, SODE5);
						graphs.Get(SODE5, 1)->Set_Base((float)(DrawPts->disp_x / 2) + (float)(-DrawPts->disp_x / 5), 0, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 0, 1.f, 0, 0, 1.f, 0.f);
						graphs.Get(SODE6, 1)->Set_Base((float)(DrawPts->disp_x / 2) + (float)(DrawPts->disp_x / 5), 0, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 0, 1.f, 0, 0, 1.f, 0.f);
						graphs.Get(SODE5, 0)->Set_Base((float)(DrawPts->disp_x / 2) + (float)(-DrawPts->disp_x / 5), 0, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 0, 1.f, 0, 0, 1.f, 0.f);
						graphs.Get(SODE6, 0)->Set_Base((float)(DrawPts->disp_x / 2) + (float)(DrawPts->disp_x / 5), 0, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 0, 1.f, 0, 0, 1.f, 0.f);

						graphs.Load((float)(y_r(1920 * 3 / 10)), (float)(y_r(1080 * 3 / 2)), 0, 1.f, 1.f, SODE4);
						graphs.Load((float)(y_r(1920 * 7 / 10)), (float)(y_r(1080 * 3 / 2)), 0, 1.f, 1.f, SODE3);
						graphs.Load((float)(y_r(1920 * 3 / 10)), (float)(y_r(1080 * 3 / 2)), 0, 1.f, 1.f, SODE2);
						graphs.Load((float)(y_r(1920 * 7 / 10)), (float)(y_r(1080 * 3 / 2)), 0, 1.f, 1.f, SODE1);
						//
						graphs.Load((float)(y_r(1920 * 1 / 5)), (float)(y_r(1080 * 3 / 5)), 0.f, 196.f / 255.f, 128.f / 540.f * 0.5f, FLASH1);
						graphs.Load((float)(y_r(1920 * 3 / 4)), (float)(y_r(1080 * 1 / 5)), 0.f, 196.f / 255.f, 128.f / 540.f * 0.5f, FLASH1);
						graphs.Load((float)(y_r(1920 * 2 / 3)), (float)(y_r(1080 * 4 / 5)), 0.f, 196.f / 255.f, 128.f / 540.f * 0.5f, FLASH1);

						graphs.Load((float)(y_r(1920 * 1 / 5)), (float)(y_r(1080 * 3 / 5)), 0.f, 1.f, 172.f / 540.f * 0.5f, FIRST1);//0
						graphs.Load((float)(y_r(1920 * 3 / 4)), (float)(y_r(1080 * 1 / 5)), 0.f, 1.f, 172.f / 540.f * 0.5f, FIRST1);//1
						graphs.Load((float)(y_r(1920 * 2 / 3)), (float)(y_r(1080 * 4 / 5)), 0.f, 1.f, 172.f / 540.f * 0.5f, FIRST1);//2

						graphs.Load((float)(y_r(1920 * 1 / 5)), (float)(y_r(1080 * 3 / 5)), 0.f, 0.f, 128.f / 540.f * 0.9f, FLASH1);//3
						graphs.Load((float)(y_r(1920 * 3 / 4)), (float)(y_r(1080 * 1 / 5)), 0.f, 0.f, 128.f / 540.f * 0.9f, FLASH1);//4
						graphs.Load((float)(y_r(1920 * 2 / 3)), (float)(y_r(1080 * 4 / 5)), 0.f, 0.f, 128.f / 540.f * 0.9f, FLASH1);//5
						//sode_last
						graphs.Load((float)(y_r(1920 * 1 / 2) + y_r(-150) * 10.f), (float)(y_r(1080 * 1 / 2) + y_r(100) * 10.f), deg2rad(-260), 1.f, 1.f, SODE5);
						graphs.Load((float)(y_r(1920 * 1 / 2) + y_r(-300) * 10.f), (float)(y_r(1080 * 1 / 2) + y_r(-500) * 10.f), deg2rad(-230), 1.f, 1.f, SODE5);
						graphs.Load((float)(y_r(1920 * 1 / 2)), (float)(y_r(1080 * 1 / 2) + y_r(-600) * 10.f), deg2rad(-180), 1.f, 1.f, SODE5);
						graphs.Load((float)(y_r(1920 * 1 / 2) + y_r(-500) * 10.f), (float)(y_r(1080 * 1 / 2) + y_r(300) * 10.f), deg2rad(-270), 1.f, 1.f, SODE5);
						graphs.Load((float)(y_r(1920 * 1 / 2) + y_r(400) * 10.f), (float)(y_r(1080 * 1 / 2) + y_r(-550) * 10.f), deg2rad(-140), 1.f, 1.f, SODE5);
						graphs.Load((float)(y_r(1920 * 1 / 2) + y_r(500) * 10.f), (float)(y_r(1080 * 1 / 2) + y_r(500) * 10.f), deg2rad(-10), 1.f, 1.f, SODE5);
						graphs.Load((float)(y_r(1920 * 1 / 2) + y_r(600) * 10.f), (float)(y_r(1080 * 1 / 2) + y_r(-300) * 10.f), deg2rad(-110), 1.f, 1.f, SODE5);
						graphs.Load((float)(y_r(1920 * 1 / 2) + y_r(700) * 10.f), (float)(y_r(1080 * 1 / 2) + y_r(100) * 10.f), deg2rad(-90), 1.f, 1.f, SODE5);
						graphs.Load((float)(y_r(1920 * 1 / 2) + y_r(50) * 10.f), (float)(y_r(1080 * 1 / 2) + y_r(700) * 10.f), deg2rad(30), 1.f, 1.f, SODE5);
						graphs.Load((float)(y_r(1920 * 1 / 2) + y_r(-450) * 10.f), (float)(y_r(1080 * 1 / 2) + y_r(550) * 10.f), deg2rad(50), 1.f, 1.f, SODE5);

						graphs.Load((float)(y_r(1920 * 1 / 2)), (float)(y_r(1080 * 1 / 2)), 0, 1.f, 0.5f, LOGO1);
					}
					LONGLONG StartF = 0;
					LONGLONG ContiF = 0;
					int mdata = FileRead_open("data/Cut.txt", FALSE);
					int cnt = 0;

					clsDx();
					auto NowTime = GetNowHiPerformanceCount();
					auto TotalTime = GetNowHiPerformanceCount();
					while (FileRead_eof(mdata) == 0) {
						LoadScript(getparams::get_str(mdata));
						if (func == "") { continue; }
						//変数登録
						{
							if (func.find("SetArg") != std::string::npos) {
								args2.resize(args2.size() + 1);
								args2.back().Base = args[0];
								args2.back().After = args[1];
							}
							//変数変換処理
							else {
								for (auto& a1 : args) {
									for (auto& a2 : args2) {
										if (a1 == a2.Base) {
											a1 = a2.After;
											break;
										}
									}
								}
							}
						}
						//モデル読み込み
						{
							if (func.find("LoadModel") != std::string::npos) {
								SetUseASyncLoadFlag(TRUE);
								for (int i = 0; i < std::stoi(args[1]); i++) {
									models.Load_onAnime(args[0]);					//origin
								}
								SetUseASyncLoadFlag(FALSE);
							}
						}
						//カット
						{
							if (func.find("SetCut") != std::string::npos) {
								Cut_Pic.resize(Cut_Pic.size() + 1);
								Cut_Pic.back().TIME = (LONGLONG)(1000000.f * std::stof(args[0]));
							}
							else if (func.find("SetCampos_NoneRad") != std::string::npos) {
								Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
							}
							else if (func.find("SetCampos_ByRad") != std::string::npos) {
								Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec + GetVector(std::stof(args[0]), std::stof(args[1])) *std::stof(args[2]);
							}
							else if (func.find("SetCamvec") != std::string::npos) {
								Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
							}
							else if (func.find("SetCamup") != std::string::npos) {
								Cut_Pic.back().Aim_camera.camup = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
							}
							else if (func.find("SetCamInfo") != std::string::npos) {
								Cut_Pic.back().Aim_camera.fov = deg2rad(std::stof(args[0]));
								Cut_Pic.back().Aim_camera.near_ = std::stof(args[1]);
								Cut_Pic.back().Aim_camera.far_ = std::stof(args[2]);
								Cut_Pic.back().cam_per = std::stof(args[3]);
							}
							else if (func.find("SetNextResetPhysics") != std::string::npos) {
								Cut_Pic.back().isNextreset = true;
							}
							else if (func.find("SetDrawGraph") != std::string::npos) {
								size_t in_str = args[1].find("~");
								if (in_str != std::string::npos) {
									int start_t = std::stoi(args[1].substr(0, in_str));
									int end_t = std::stoi(args[1].substr(in_str + 1));
									for (int i = start_t; i <= end_t; i++) {
										auto* t = graphs.Get(args[0], i);
										t->Cutinfo.Switch.resize(t->Cutinfo.Switch.size() + 1);
										t->Cutinfo.Switch.back().On = (int)(Cut_Pic.size()) - 1;
										t->Cutinfo.Switch.back().Off = std::stoi(args[2]);
										t->Cutinfo_.resize(t->Cutinfo_.size() + 1);
									}
								}
								else {
									auto* t = graphs.Get(args[0], std::stoi(args[1]));
									t->Cutinfo.Switch.resize(t->Cutinfo.Switch.size() + 1);
									t->Cutinfo.Switch.back().On = (int)(Cut_Pic.size()) - 1;
									t->Cutinfo.Switch.back().Off = std::stoi(args[2]);
									t->Cutinfo_.resize(t->Cutinfo_.size() + 1);
								}
							}
							else if (func.find("SetGraphBlur") != std::string::npos) {
								auto* t = graphs.Get(args[0], std::stoi(args[1]));
								t->Cutinfo_.back().Blur.Switch.resize(t->Cutinfo_.back().Blur.Switch.size() + 1);
								t->Cutinfo_.back().Blur.Switch.back().On = (int)(Cut_Pic.size()) - 1;
								t->Cutinfo_.back().Blur.Switch.back().Off = std::stoi(args[2]);
							}
							else if (func.find("SetDrawModel") != std::string::npos) {
								size_t in_str = args[1].find("~");
								if (in_str != std::string::npos) {
									int start_t = std::stoi(args[1].substr(0, in_str));
									int end_t = std::stoi(args[1].substr(in_str + 1));
									for (int i = start_t; i <= end_t; i++) {
										auto* t = models.Get(args[0], i);
										t->Cutinfo.Switch.resize(t->Cutinfo.Switch.size() + 1);
										t->Cutinfo.Switch.back().On = (int)(Cut_Pic.size()) - 1;
										t->Cutinfo.Switch.back().Off = std::stoi(args[2]);

										t->Cutinfo_.resize(t->Cutinfo_.size() + 1);
									}
								}
								else {
									auto* t = models.Get(args[0], std::stoi(args[1]));
									t->Cutinfo.Switch.resize(t->Cutinfo.Switch.size() + 1);
									t->Cutinfo.Switch.back().On = (int)(Cut_Pic.size()) - 1;
									t->Cutinfo.Switch.back().Off = std::stoi(args[2]);

									t->Cutinfo_.resize(t->Cutinfo_.size() + 1);
								}
							}
							else if (func.find("SetModelAnime") != std::string::npos) {
								auto* t = models.Get(args[0], std::stoi(args[1]));
								t->Cutinfo_.back().animsel = std::stoi(args[2]);
								t->Cutinfo_.back().isloop = (args[3].find("TRUE") != std::string::npos);
								t->Cutinfo_.back().animspeed = std::stof(args[4]);
								t->Cutinfo_.back().startframe = std::stof(args[5]);
							}
							else if (func.find("SetModelMat") != std::string::npos) {
								auto* t = models.Get(args[0], std::stoi(args[1]));
								t->Cutinfo_.back().usemat = true;
								t->Cutinfo_.back().mat_p =
									MATRIX_ref::RotY(deg2rad(std::stof(args[2])))*
									MATRIX_ref::Mtrans(VECTOR_ref::vget(std::stof(args[3]), std::stof(args[4]), std::stof(args[5])))*
									MATRIX_ref::RotY(deg2rad(std::stof(args[6])));
							}
							else if (func.find("SetModelPhysicsSpeed") != std::string::npos) {
								auto* t = models.Get(args[0], std::stoi(args[1]));
								t->Cutinfo_.back().PhysicsSpeed_ = std::stof(args[2]);
							}
							else if (func.find("SetModelOpacityRate") != std::string::npos) {
								auto* t = models.Get(args[0], std::stoi(args[1]));
								t->Cutinfo_.back().OpacityRate = std::stof(args[2]);
							}
							else if (func.find("SetModelMode") != std::string::npos) {
								auto* t = models.Get(args[0], std::stoi(args[1]));
								if (args[2] == "SKY_TRUE") {
									t->isFar = true;
								}
								if (args[2] == "NEAR_FALSE") {
									t->IsNearShadow = false;
								}
								if (args[2] == "FAR_TRUE") {
									t->IsFarShadow = true;
								}
							}
						}
						//テロップ
						{
							if (func.find("SetTelopTime") != std::string::npos) {
								StartF = (LONGLONG)(1000000.f * std::stof(args[0]));
								ContiF = (LONGLONG)(1000000.f * std::stof(args[1]));
							}
							else if (func.find("AddTelopTime") != std::string::npos) {
								StartF += (LONGLONG)(1000000.f * std::stof(args[0]));
								ContiF = (LONGLONG)(1000000.f * std::stof(args[1]));
							}
							else if (func.find("SetTelop") != std::string::npos) {
								int t = 0;
								if (args[4].find("LEFT") != std::string::npos) { t = 0; }
								else if (args[4].find("MIDDLE") != std::string::npos) { t = 1; }
								else if (args[4].find("RIGHT") != std::string::npos) { t = 2; }
								Texts.resize(Texts.size() + 1);
								Texts.back().Set(y_r(std::stoi(args[0])), y_r(std::stoi(args[1])), y_r(std::stoi(args[2])), args[3], StartF, ContiF, t);
							}
						}
						//END
						if (ProcessMessage() == 0) {
							GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
							float tim = float((GetNowHiPerformanceCount() - NowTime) / 1000) / 1000.f;
							printfDx("ロード%3d完了 [%7.3f s] : %s\n", cnt, tim, func.c_str());
							if (tim >= 0.001f) {
								DrawPts->Screen_Flip();
							}
							NowTime = GetNowHiPerformanceCount();
						}
						cnt++;
					}
					FileRead_close(mdata);
					if (ProcessMessage() == 0) {
						GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
						printfDx("ロード全部完了　キーを押してください : total = [%7.3f s]\n", float((GetNowHiPerformanceCount() - TotalTime) / 1000) / 1000.f);
						DrawPts->Screen_Flip();
					}
				}
				WaitKey();
				SetUseASyncLoadFlag(FALSE);
				models.Set();
				{
					//モデルの事前処理
					std::string MAP = "data/model/map/model.mv1";
					std::string BOARD = "data/model/board/model.mv1";
					models.Get(MAP, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.25f, -2394.f))*MATRIX_ref::RotY(deg2rad(90)));
					models.Get(BOARD, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.25f, 0.f)));
					models.Get(SHIP)->obj.SetScale(VECTOR_ref::vget(1.f, 1.f, 1.f)*0.01f);
					//モデル座標事前準備
					graphs.Get(FACE)->X.Set((float)(y_r(1920 * 1 / 2)), 0.f);
					graphs.Get(FACE)->Y.Set((float)(y_r(1080 * 1 / 2)), 0.f);
					graphs.Get(NEWSP)->X.Set((float)(y_r(1920 * -1 / 2)), 0.f);
					graphs.Get(NEWSP)->Y.Set((float)(y_r(1080 * -1 / 2)), 0.f);
					graphs.Get(NEWSP)->Rad.Set(deg2rad(-30), 0.f);
					graphs.Get(SODE6, 0)->SetBright(108, 108, 108);
					graphs.Get(SODE5, 0)->SetBright(144, 144, 144);
					graphs.Get(SODE6, 1)->SetBright(192, 192, 192);
					graphs.Get(FLASH1, 3)->Set_Base(0, 1, 0, 1, 0, graphs.Get(FLASH1, 3)->Scale.Ans, 1, 0, 1.f, 0.9f);
					graphs.Get(FLASH1, 4)->Set_Base(0, 1, 0, 1, 0, graphs.Get(FLASH1, 4)->Scale.Ans, 1, 0, 1.f, 0.9f);
					graphs.Get(FLASH1, 5)->Set_Base(0, 1, 0, 1, 0, graphs.Get(FLASH1, 5)->Scale.Ans, 1, 0, 1.f, 0.9f);
					graphs.Get(LOGO1, 0)->Set_Base((float)(DrawPts->disp_x / 2), 0.f, (float)(DrawPts->disp_y / 2), 0.f, 0.f, 0.5f, 0, 0, 1.f, 0.f);
				}
			}
		public:
			void Set(void) noexcept override {
				TEMPSCENE::Set_EnvLight(VECTOR_ref::vget(500.f, 50.f, 500.f), VECTOR_ref::vget(-500.f, -50.f, -500.f), VECTOR_ref::vget(-0.5f, -0.5f, 0.5f), GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				TEMPSCENE::Set();
				models.Get(SUN, 0)->obj.SetMatrix( MATRIX_ref::RotVec2(VECTOR_ref::up(), (VECTOR_ref)(Get_Light_vec().Norm())) * MATRIX_ref::Mtrans(Get_Light_vec().Norm() * -1500.f));
				Cut = 41;
				Cut = 0;
				SetUseASyncLoadFlag(FALSE);
				BGM = SoundHandle::Load("data/sound2.wav");
				/*
				movie = GraphHandle::Load("data/base_movie.mp4");
				PlayMovieToGraph(movie.get(), 2, DX_MOVIEPLAYTYPE_BCANCEL);
				ChangeMovieVolumeToGraph(0, movie.get());
				*/
				//プレイ用意
				BGM.play(DX_PLAYTYPE_BACK, TRUE);
				//BGM.vol(64);
				BaseTime = GetNowHiPerformanceCount() - (Cut > 0 ? Cut_Pic[Cut - 1].TIME : 0);
				//SeekMovieToGraph(movie.get(), (int)(Cut > 0 ? Cut_Pic[Cut - 1].TIME : 0) / 1000);
				HostpassPTs->Set_Bright(226, 226, 216);
			}
			bool Update(void) noexcept override {
				TEMPSCENE::Update();
				auto NowTime = GetNowHiPerformanceCount() - BaseTime;
				{
					if (!Time_Over()) {
						models.FirstUpdate((int)Cut, isFirstLoop);
						graphs.FirstUpdate((int)Cut, isFirstLoop);
						int SEL = 1;
						{
							//1
							if (Cut == SEL) {
								graphs.Get(SODE1, 0)->Set_Base((float)(DrawPts->disp_x / 2), 0.9f, (float)(DrawPts->disp_y / 2), 0.9f, 0, 1.f, 0, 0, 1.f, 0.f);
								graphs.Get(SODE2, 0)->Set_Base((float)(DrawPts->disp_x / 2), 0.875f, (float)(DrawPts->disp_y / 2), 0.875f, 0, 1.f, 0, 0, 1.f, 0.f);
								graphs.Get(SODE3, 0)->Set_Base((float)(DrawPts->disp_x / 2), 0.85f, (float)(DrawPts->disp_y / 2), 0.85f, 0, 1.f, 0, 0, 1.f, 0.f);
								graphs.Get(SODE4, 0)->Set_Base((float)(DrawPts->disp_x / 2), 0.825f, (float)(DrawPts->disp_y / 2), 0.825f, 0, 1.f, 0, 0, 1.f, 0.f);
								if (NowTime > (LONGLONG)(1000000.f * 1.421f)) {
									graphs.Get(FIRST1, 0)->isDraw = true;
									graphs.Get(FLASH1, 0)->isDraw = true;
									graphs.Get(FLASH1, 3)->isDraw = true;
								}
								if (NowTime > (LONGLONG)(1000000.f * 1.637f)) {
									graphs.Get(FIRST1, 1)->isDraw = true;
									graphs.Get(FLASH1, 1)->isDraw = true;
									graphs.Get(FLASH1, 4)->isDraw = true;
								}
								if (NowTime > (LONGLONG)(1000000.f * 1.820f)) {
									graphs.Get(FIRST1, 2)->isDraw = true;
									graphs.Get(FLASH1, 2)->isDraw = true;
									graphs.Get(FLASH1, 5)->isDraw = true;
								}
								for (int i = 0; i < 3; i++) {
									auto* t = graphs.Get(FLASH1, 3 + i);
									if (t->isDraw) {
										t->Alpha.Base = std::max(t->Alpha.Base - 6.f / FPS, 0.f);
										t->Scale.Base += 6.f*(128.f / 540.f * 0.9f) / FPS;
									}
								}
							}
							SEL+=5;
							//6
							if (Cut == SEL) {
								graphs.Get(SODE1, 0)->Set_Base((float)(DrawPts->disp_x / 2), 0.f, (float)(DrawPts->disp_y / 2), 0.f, 0, 1.f, 0, 0, 1.f, 0.f);
								graphs.Get(SODE2, 0)->Set_Base((float)(DrawPts->disp_x / 2), 0.f, (float)(DrawPts->disp_y / 2), 0.f, 0, 1.f, 0, 0, 1.f, 0.f);
								graphs.Get(SODE3, 0)->Set_Base((float)(DrawPts->disp_x / 2), 0.f, (float)(DrawPts->disp_y / 2), 0.f, 0, 1.f, 0, 0, 1.f, 0.f);
								graphs.Get(SODE4, 0)->Set_Base((float)(DrawPts->disp_x / 2), 0.f, (float)(DrawPts->disp_y / 2), 0.f, 0, 1.f, 0, 0, 1.f, 0.f);
							}
							SEL++;
							//7
							if (Cut == SEL) {
								if (isFirstLoop) {
									Set_Effect(Effect::ef_reco, VECTOR_ref::vget(0.f, 0.f, 0.f), VECTOR_ref::front(), 3.f);
									cam_yure = 0.f;
								}
								else {
									SetScale_Effect(Effect::ef_reco, 3.f + cam_yure * 1.f / 3.f);
									SetSpeed_Effect(Effect::ef_reco, 1.05f);
									cam_yure += 5.f / FPS;
									easing_set(&Cut_Pic[Cut].Aim_camera.campos, Cut_Pic[Cut].Aim_camera.camvec + GetVector(6.9f, 180.f + 19.5f)*(27.0f + cam_yure) + VECTOR_ref::vget(GetRandf(cam_yure), GetRandf(cam_yure), GetRandf(cam_yure)), 0.9f);
									easing_set(&Cut_Pic[Cut].Aim_camera.camup, VECTOR_ref::vget(GetRandf(5.f) / 10.f + cam_yure / 50.f, 1.f, 0.f).Norm(), 0.95f);
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(45);
									Cut_Pic[Cut].cam_per = 0.975f;
								}
								graphs.Get(SODE1, 0)->Set_Base((float)(DrawPts->disp_x / 2) + (float)(DrawPts->disp_x), 0.9f, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y / 5), 0.9f, 0, 1.f, 0, 0, 1.f, 0.f);
								graphs.Get(SODE2, 0)->Set_Base((float)(DrawPts->disp_x / 2) + (float)(-DrawPts->disp_x), 0.875f, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y / 5), 0.875f, 0, 1.f, 0, 0, 1.f, 0.f);
								graphs.Get(SODE3, 0)->Set_Base((float)(DrawPts->disp_x / 2) + (float)(DrawPts->disp_x), 0.85f, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y / 5), 0.85f, 0, 1.f, 0, 0, 1.f, 0.f);
								graphs.Get(SODE4, 0)->Set_Base((float)(DrawPts->disp_x / 2) + (float)(-DrawPts->disp_x), 0.825f, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y / 5), 0.825f, 0, 1.f, 0, 0, 1.f, 0.f);
							}
							SEL++;
							//8
							if (Cut == SEL) {
								SetScale_Effect(Effect::ef_reco, 3.f + cam_yure * 1.f / 3.f);
								SetSpeed_Effect(Effect::ef_reco, 1.05f);
								cam_yure += 5.f / FPS;
								easing_set(&Cut_Pic[Cut].Aim_camera.campos, Cut_Pic[Cut].Aim_camera.camvec + GetVector(6.9f, 180.f + 19.5f)*(27.0f + cam_yure) + VECTOR_ref::vget(GetRandf(cam_yure), GetRandf(cam_yure), GetRandf(cam_yure)), 0.9f);
								easing_set(&Cut_Pic[Cut].Aim_camera.camup, VECTOR_ref::vget(GetRandf(5.f) / 10.f + cam_yure / 50.f, 1.f, 0.f).Norm(), 0.95f);
							}
							SEL++;
							//9
							if (Cut == SEL) {
								if (isFirstLoop) {
									Stop_Effect(Effect::ef_reco);
									models.Get(Tachyon, 0)->obj.get_anime(2).time = 30.f;
									spd_takion = 0.6f;
								}
								if (models.Get(Tachyon, 0)->obj.get_anime(2).time <= 32.f) {
									easing_set(&models.Get(Tachyon, 0)->OpacityRate, 1.f, 0.95f);
								}
								else {
									easing_set(&spd_takion, 0.01f, 0.9f);
									models.Get(Tachyon, 0)->OpacityRate = 1.f;
								}
								models.Get(Tachyon, 0)->UpdateAnim(2, true, spd_takion);
							}
							SEL+=4;
							//13
							if (Cut == SEL) {
								if (NowTime > (LONGLONG)(1000000.f * 8.864f)) {
									models.Get(Tachyon, 0)->UpdateAnim(2, true, 0.6f);
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon, 0)->obj.frame(6);
									Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(17.178366f, 4.319146f, -36.016518f);
									Cut_Pic[Cut].cam_per = 0.9f;
								}
								else {
									models.Get(Tachyon, 0)->UpdateAnim(2, true, 0.01f);
								}
							}
							SEL++;
							//14
							if (Cut == SEL) {
								Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon2, 0)->obj.frame(6);
							}
							SEL++;
							//15
							if (Cut == SEL) {
								if (isFirstLoop) {
									radc = 0.f;
								}
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, 80.f + radc)*88.0f;
								radc += 12.5f / FPS;
							}
							SEL++;
							//16
							if (Cut == SEL) {
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, 80.f + radc)*88.0f;
								radc += 12.5f / FPS;
								models.Get(Tachyon, 0)->OpacityRate = std::max(models.Get(Tachyon, 0)->OpacityRate - 2.f / FPS, 0.f);
								models.Get(Tachyon2, 0)->OpacityRate = std::min(models.Get(Tachyon2, 0)->OpacityRate + 2.f / FPS, 1.f);
								models.Get(Cafe, 0)->OpacityRate = std::min(models.Get(Cafe, 0)->OpacityRate + 2.f / FPS, 1.f);
							}
							SEL++;
							//17
							if (Cut == SEL) {
								if (isFirstLoop) {
									radc = 0.f;
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon2, 0)->obj.frame(9);
								}
								else {
									easing_set(&Cut_Pic[Cut].Aim_camera.camvec, models.Get(Tachyon2, 0)->obj.frame(9) + VECTOR_ref::vget(0.f, GetRandf(5.5f), GetRandf(13.5f)), 0.975f);
									Cut_Pic[Cut].cam_per = 0.975f;
								}
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-8.5f + xradadd_r * 1.5f, 80.f + radc)*38.0f;
								radc += 12.5f / FPS;
							}
							SEL++;
							//18
							if (Cut == SEL) {
								if (isFirstLoop) {
									radc = 0.f;
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Cafe, 0)->obj.frame(8);
								}
								else {
									Cut_Pic[Cut].cam_per = 0.975f;
								}
								easing_set(&Cut_Pic[Cut].Aim_camera.camvec, models.Get(Cafe, 0)->obj.frame(8) + VECTOR_ref::vget(0.f, GetRandf(5.5f), GetRandf(13.5f)), 0.975f);
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, 260.f + radc)*34.0f;
								radc += 12.5f / FPS;
							}
							SEL++;
							//19
							if (Cut == SEL) {
								if (isFirstLoop) {
									radc = 0.f;
								}
								else {
									Cut_Pic[Cut].cam_per = 0.975f;
								}
								Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon2, 0)->obj.frame(103);
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(25.5f + xradadd_r * 1.5f, 80.f + radc)*38.0f;
								radc += 12.5f / FPS;
							}
							SEL++;
						}
						{}
						{
							//20
							if (Cut == SEL) {
								std::string NEWS = "data/model/paper/news.mv1";
								if (isFirstLoop) {
									for (int i = 0; i < 12; i++) {
										auto* t = models.Get(NEWS, i);
										t->move.mat = MATRIX_ref::RotY(deg2rad(GetRand(360)))*MATRIX_ref::RotX(deg2rad(GetRandf(30.f)));
										t->move.pos = VECTOR_ref::vget(GetRandf(20.f), (float)(37 + GetRand(10)), GetRandf(20.f));
										t->move.vec.y(-(float)(GetRand(100)) / 1000.f);
									}
								}
								for (int i = 0; i < 12; i++) {
									auto* t = models.Get(NEWS, i);
									t->move.Update_Physics(0.f, 0.75f);
									t->obj.SetMatrix(t->move.MatIn());
								}
							}
							SEL++;
							//21
							if (Cut == SEL) {
								graphs.Get(NEWSP)->X.Base += (int)((float)(DrawPts->disp_x / 2)*4.f / FPS);
								graphs.Get(NEWSP)->Y.Base += (int)((float)(DrawPts->disp_y / 2)*4.f / FPS);
								graphs.Get(NEWSP)->Rad.Base += deg2rad(6) / FPS;
							}
							SEL++;
							//22
							if (Cut == SEL) {
								graphs.Get(NEWSP)->X.Base += (int)((float)(DrawPts->disp_x / 2)*4.f / FPS);
								graphs.Get(NEWSP)->Y.Base += (int)((float)(DrawPts->disp_y / 2)*4.f / FPS);
								graphs.Get(NEWSP)->Rad.Base += deg2rad(6) / FPS;
							}
							SEL++;
							//23
							if (Cut == SEL) {
								graphs.Get(NEWSP)->X.Base += (int)((float)(DrawPts->disp_x / 2)*4.f / FPS);
								graphs.Get(NEWSP)->Y.Base += (int)((float)(DrawPts->disp_y / 2)*4.f / FPS);
								graphs.Get(NEWSP)->Rad.Base += deg2rad(6) / FPS;
								std::string GoldShip = "data/umamusume/gold/model.mv1";
								models.Get(GoldShip, 0)->UpdateAnim(1, true, (graphs.Get(NEWSP)->Y.Ans < DrawPts->disp_y / 2) ? 0.f : 0.45f);
							}
							SEL+=2;
							//25
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Tachyon, 0)->obj.get_anime(8).time = 33.f;
									models.Get(Tachyon, 1)->UpdateAnim(9, false, 0.f);
								}
								if (!return_walk) {
									if (models.Get(Tachyon, 0)->obj.get_anime(8).TimeEnd()) {
										return_walk = true;
										models.Get(Tachyon, 1)->OpacityRate = 1.f;
										Cut_Pic[Cut].cam_per = 0.95f;
										Cut_Pic[Cut].Aim_camera.fov = deg2rad(7);
									}
								}
								else {
									models.Get(Tachyon, 1)->UpdateAnim(9, false, 0.7f);
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon, 1)->obj.frame(9);
								}
								models.Get(Tachyon, 0)->UpdateAnim(8, false, return_walk ? -4.f : 4.f);
							}
							SEL++;
							//26
							if (Cut == SEL) {
								if (isFirstLoop) {
									radc = 0.f;
								}
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, 80.f + radc)*88.0f;
								radc += 12.5f / FPS;
							}
							SEL++;
							//27
							if (Cut == SEL) {
								if (isFirstLoop) {
									Gate_Xpos = -20.f;
								}
								else {
									easing_set(&rand_vp, VECTOR_ref::vget(GetRandf(7.f), GetRandf(7.f), GetRandf(7.f)), 0.9f);
									Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(10.122698f, 10.028077f, -50.985756f) + rand_vp;
									Cut_Pic[Cut].cam_per = 0.95f;
								}
								models.Get(GATE, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(Gate_Xpos, 0.25f, 0.f)));
								if (Gate_Xpos < 20.f) {
									Gate_Xpos += 20.f *1.f / FPS;
								}
							}
							SEL++;
							//28
							if (Cut == SEL) {
								if (isFirstLoop) {
									ship_z = -5.f;
									ship_zadd = 0.f;
									camzb_28 = 0.f;
								}
								else {
									Cut_Pic[Cut].cam_per = (camzb_28 > -8.f) ? 0.9f : 0.95f;
								}
								{
									Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(0, 0, camzb_28);
									Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + VECTOR_ref::vget(2.f, 12.f, 6.f);
									camzb_28 = std::max(camzb_28 - 100.f / FPS, -10.f);
									models.Get(SHIP)->obj.SetPosition(camera_main.camvec + VECTOR_ref::vget(-2.f, 3.f, -ship_z));
									ship_z += ship_zadd / FPS;
									if (ship_zadd <= 0.95f) {
										easing_set(&ship_zadd, 1.f, 0.95f);
									}
									else {
										ship_zadd = 1.f;
									}
								}
							}
							SEL++;
							//29
							if (Cut == SEL) {
								Cut_Pic[Cut].cam_per = (camzb_28 > -8.f) ? 0.9f : 0.95f;
								{
									Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(0, 0, camzb_28);
									Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + VECTOR_ref::vget(2.f, 12.f, 6.f);
									camzb_28 = std::max(camzb_28 - 100.f / FPS, -10.f);
									models.Get(SHIP)->obj.SetPosition(camera_main.camvec + VECTOR_ref::vget(-2.f, 3.f, -ship_z));
									ship_z += ship_zadd / FPS;
									if (ship_zadd <= 0.95f) {
										easing_set(&ship_zadd, 1.f, 0.95f);
									}
									else {
										ship_zadd = 1.f;
									}
								}
							}
							SEL+=3;
							//32
							if (Cut == SEL) {
								std::string Scarlet = "data/umamusume/scarlet/model.mv1";
								if (models.Get(Scarlet, 0)->obj.get_anime(0).time > 10.f) {
									easing_set(&models.Get(Scarlet, 0)->OpacityRate, 1.f, 0.95f);
								}
							}
							SEL+=4;
							//36
							if (Cut == SEL) {
								models.Get(Tachyon, 0)->OpacityRate = std::min(models.Get(Tachyon, 0)->OpacityRate + 2.f / FPS, 1.f);
								models.Get(Cafe, 0)->OpacityRate = std::max(models.Get(Cafe, 0)->OpacityRate - 2.f / FPS, 0.f);
							}
							SEL+=5;
							//41
							if (Cut == SEL) {
								if (isFirstLoop) {
									camzb_28 = 0.f;
									camzb_41 = 0.1f;
									Gate_Xpos = -7.f;
									for (int i = 0; i < 12; i++) {
										auto* t = models.Get(Mobu, i);
										t->b_run = 0.8f + (float)(GetRand(195)) / 1000.f;
										t->b_runrange = (float)(GetRand(100)) / 10.f;
										while (true) {
											t->Anim_Sel = GetRand(2);
											if (i == 0 || t->Anim_Sel != models.Get(Mobu, i - 1)->Anim_Sel) {
												break;
											}
										}
										t->UpdateAnim(t->Anim_Sel, false, 0.f);
										t->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(Gate_Xpos, 0.25f, -10.f)));
										Gate_Xpos += -14.f;
									}
								}
								float pp = 10.f;
								if (camzb_28 < -9.5f) {
									pp = 2.f;
								}
								if (camzb_28 <= -10.f) {
									pp = 0.f;
								}
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(0.f, 30.f)*(20.f - camzb_28 * 20.8f) + VECTOR_ref::vget(GetRandf(pp), GetRandf(pp), GetRandf(pp));
								Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(camzb_41, 0.9f, 0);
								easing_set(&camzb_41, 0.3f, 0.995f);
								Cut_Pic[Cut].cam_per = 0.f;
								if (camzb_28 != 0.f) {
									if (camzb_28 > -8.f) {
										Cut_Pic[Cut].cam_per = 0.9f;
									}
									else {
										Cut_Pic[Cut].cam_per = 0.95f;
									}
								}
								if (camzb_28 > -10.f) {
									camzb_28 -= 100.f / FPS;
								}
								else {
									camzb_28 = -10.f;
								}
							}
							SEL++;
							//42
							if (Cut == SEL) {
								if (isFirstLoop) {
									bright_43 = 0.f;
								}
								float pp = 10.f;
								if (camzb_28 > -5.f) {
									pp = 2.f;
								}
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(0.f, 30.f)*(10.f - camzb_28 * 20.8f) + VECTOR_ref::vget(GetRandf(pp), GetRandf(pp), GetRandf(pp));
								Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(camzb_41, 0.9f, 0);
								easing_set(&camzb_41, 0.3f, 0.995f);
								HostpassPTs->Set_Bright(226 + (int)((float)(255 - 226)*bright_43), 226 + (int)((float)(255 - 226)*bright_43), 196 + (int)((float)(255 - 196)*bright_43));
								easing_set(&bright_43, 1.f, 0.9f);
								if (camzb_28 < 0.f) {
									camzb_28 += 70.f / FPS;
								}
								else {
									camzb_28 = 0.f;
								}
							}
							SEL++;
							//43
							if (Cut == SEL) {
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-20.f, -10.f + camzb_28)*20.f;
								camzb_28 += 40.f / FPS;
							}
							SEL++;
							//44
							if (Cut == SEL) {
								if (isFirstLoop) {
									camyup = 0.f;
								}
								if (NowTime > (LONGLONG)(1000000.f * 49.561)) {
									models.Get(GATE, 0)->UpdateAnim(1, false, 1.f);
									Gate_Xpos = 0.f;
									Gate_Xpos += -7.f;
									for (int i = 0; i < 12; i++) {
										auto* t = models.Get(Mobu, i);
										easing_set(&t->obj.get_anime(t->Anim_Sel).per, 0.f, t->b_run);
										easing_set(&t->obj.get_anime(3).per, 1.f, t->b_run);
										t->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(Gate_Xpos, 0.25f, -10.f - t->b_runrange * t->obj.get_anime(3).per)));
										Gate_Xpos += -14.f;
									}
								}
								if (NowTime > (LONGLONG)(1000000.f * 49.681)) {
									Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(-39.67f, 5.48f, -51.8f);
									Cut_Pic[Cut].Aim_camera.camvec = Cut_Pic[Cut].Aim_camera.campos + VECTOR_ref::vget(1, -0.3f, -0.6f);
									Cut_Pic[Cut].cam_per = 0.9f;
								}
								else {
									Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + VECTOR_ref::vget(1, -0.1f - 0.1f*camyup, -0.13f);
									camyup += 1.f / FPS;
								}
							}
							SEL++;
							//45
							if (Cut == SEL) {
								if (isFirstLoop) {
									VECTOR_ref vec_x = (Cut_Pic[Cut].Aim_camera.camvec - Cut_Pic[Cut].Aim_camera.campos).Norm().cross(Cut_Pic[Cut].Aim_camera.camup);
									Set_Effect(Effect::ef_greexp, VECTOR_ref::vget(-4.f, 4.6f, -2.f) + vec_x * 13.f, VECTOR_ref::front(), 5.f);
									Set_Effect(Effect::ef_greexp2, VECTOR_ref::vget(8.f, 5.5f, 4.f) - vec_x * 8.f, VECTOR_ref::front(), 4.f);
								}
								SetSpeed_Effect(Effect::ef_greexp, 0.5f);
								SetSpeed_Effect(Effect::ef_greexp2, 0.5f);
							}
							SEL++;
							//46
							if (Cut == SEL) {
								graphs.Get(SODE5, 1)->X.Base += (float)(y_r(300)) / FPS;
								graphs.Get(SODE5, 1)->Y.Base -= (float)(y_r(2800)) / FPS;
								graphs.Get(SODE6, 1)->X.Base -= (float)(y_r(300)) / FPS;
								graphs.Get(SODE6, 1)->Y.Base -= (float)(y_r(2400)) / FPS;
								graphs.Get(SODE5, 0)->X.Base += (float)(y_r(300)) / FPS;
								graphs.Get(SODE5, 0)->Y.Base -= (float)(y_r(2200)) / FPS;
								graphs.Get(SODE6, 0)->X.Base -= (float)(y_r(300)) / FPS;
								graphs.Get(SODE6, 0)->Y.Base -= (float)(y_r(2000)) / FPS;
							}
							SEL++;
							//47
							if (Cut == SEL) {
								if (isFirstLoop) {
									camzb_41 = (float)(DrawPts->disp_y)*0.4f;
									SetSpeed_Effect(Effect::ef_greexp, 0.1f);
									SetSpeed_Effect(Effect::ef_greexp2, 0.1f);
								}
								Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(48.211510f, 15.f - 1.5f*(graphs.Get(ANIME2, 0)->Y.Ans - (float)(DrawPts->disp_y) / 2.f) / (float)(DrawPts->disp_y), 0.595667f);
								Cut_Pic[Cut].Aim_camera.camvec = Cut_Pic[Cut].Aim_camera.campos + VECTOR_ref::vget(-1.f, 0.3f, 0.f);
								easing_set(&graphs.Get(ANIME2, 0)->Y.Ans, (float)(DrawPts->disp_y) / 2.f + camzb_41, 0.7f);
								if (camzb_41 > 0.f) {
									camzb_41 -= (float)(y_r(500)) / FPS;
								}
								graphs.Get(SODE5, 1)->X.Base += (float)(y_r(300)) / FPS;
								graphs.Get(SODE5, 1)->Y.Base -= (float)(y_r(2800)) / FPS;
								graphs.Get(SODE6, 1)->X.Base -= (float)(y_r(300)) / FPS;
								graphs.Get(SODE6, 1)->Y.Base -= (float)(y_r(2400)) / FPS;
								graphs.Get(SODE5, 0)->X.Base += (float)(y_r(300)) / FPS;
								graphs.Get(SODE5, 0)->Y.Base -= (float)(y_r(2200)) / FPS;
								graphs.Get(SODE6, 0)->X.Base -= (float)(y_r(300)) / FPS;
								graphs.Get(SODE6, 0)->Y.Base -= (float)(y_r(2000)) / FPS;
							}
							SEL++;
							//48
							if (Cut == SEL) {
								if (isFirstLoop) {
									graphs.Get(ANIME1, 0)->Y.Ans = (float)(DrawPts->disp_y) / 2.f;
									graphs.Get(SODE5, 1)->Set_Base((float)(DrawPts->disp_x / 2) + (float)(0), 0, (float)(DrawPts->disp_y / 2) + (float)(-DrawPts->disp_y), 0, 0, 1.f, 0, 0, 1.f, 0.f);
									graphs.Get(SODE6, 1)->Set_Base((float)(DrawPts->disp_x / 2) + (float)(0), 0, (float)(DrawPts->disp_y / 2) + (float)(-DrawPts->disp_y * 5 / 4), 0, 0, 1.f, 0, 0, 1.f, 0.f);
									graphs.Get(SODE5, 0)->Set_Base((float)(DrawPts->disp_x / 2) + (float)(0), 0, (float)(DrawPts->disp_y / 2) + (float)(-DrawPts->disp_y * 6 / 4), 0, 0, 1.f, 0, 0, 1.f, 0.f);
									graphs.Get(SODE6, 0)->Set_Base((float)(DrawPts->disp_x / 2) + (float)(0), 0, (float)(DrawPts->disp_y / 2) + (float)(-DrawPts->disp_y * 7 / 4), 0, 0, 1.f, 0, 0, 1.f, 0.f);
								}
								Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(48.211510f, 15.f - 1.5f*(graphs.Get(ANIME2, 0)->Y.Ans - (float)(DrawPts->disp_y) / 2.f) / (float)(DrawPts->disp_y), 0.595667f);
								Cut_Pic[Cut].Aim_camera.camvec = Cut_Pic[Cut].Aim_camera.campos + VECTOR_ref::vget(-1.f, 0.3f, 0.f);
								easing_set(&graphs.Get(ANIME2, 0)->Y.Ans, (float)(DrawPts->disp_y) / 2.f + camzb_41, 0.7f);
								if (camzb_41 > 0.f) {
									camzb_41 -= (float)(y_r(500)) / FPS;
								}
								graphs.Get(SODE5, 1)->X.Base -= (float)(y_r(300)) / FPS;
								graphs.Get(SODE5, 1)->Y.Base += (float)(y_r(2800)) / FPS;
								graphs.Get(SODE6, 1)->X.Base += (float)(y_r(300)) / FPS;
								graphs.Get(SODE6, 1)->Y.Base += (float)(y_r(2400)) / FPS;
								graphs.Get(SODE5, 0)->X.Base -= (float)(y_r(300)) / FPS;
								graphs.Get(SODE5, 0)->Y.Base += (float)(y_r(2200)) / FPS;
								graphs.Get(SODE6, 0)->X.Base += (float)(y_r(300)) / FPS;
								graphs.Get(SODE6, 0)->Y.Base += (float)(y_r(2000)) / FPS;
							}
							SEL++;
							//49
							if (Cut == SEL) {
								VECTOR_ref aim = models.Get(Tachyon2, 0)->obj.frame(2);
								aim.y(0.f);
								Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(32.211510f, 8.997231f, -18.595667f) + aim;
								Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(-0.828184f, 16.546323f, -0.092557f) + aim;
								graphs.Get(SODE5, 1)->X.Base -= (float)(y_r(300)) / FPS;
								graphs.Get(SODE5, 1)->Y.Base += (float)(y_r(2800)) / FPS;
								graphs.Get(SODE6, 1)->X.Base += (float)(y_r(300)) / FPS;
								graphs.Get(SODE6, 1)->Y.Base += (float)(y_r(2400)) / FPS;
								graphs.Get(SODE5, 0)->X.Base -= (float)(y_r(300)) / FPS;
								graphs.Get(SODE5, 0)->Y.Base += (float)(y_r(2200)) / FPS;
								graphs.Get(SODE6, 0)->X.Base += (float)(y_r(300)) / FPS;
								graphs.Get(SODE6, 0)->Y.Base += (float)(y_r(2000)) / FPS;
							}
							SEL++;
						}
						{}
						{
							//50
							if (Cut == SEL) {
								if (isFirstLoop) {
									Stop_Effect(Effect::ef_greexp);
									Stop_Effect(Effect::ef_greexp2);
								}
								else {
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Karen, 0)->obj.frame(8);
									Cut_Pic[Cut].cam_per = 0.995f;
								}
							}
							SEL++;
							//51
							if (Cut == SEL) {
								Cut_Pic[Cut].Aim_camera.camvec = models.Get(Karen, 0)->obj.frame(10) + VECTOR_ref::vget(2.3f, 0.f, 0.f);
							}
							SEL+=2;
							//53
							if (Cut == SEL) {
								if (models.Get(Cafe, 0)->obj.get_anime(3).time > 30.f) {
									Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(0, 17, -4.5);
									Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + VECTOR_ref::vget(-5.f, -3.5f, -20.f)*2.0f;
									Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(25);
									Cut_Pic[Cut].Aim_camera.near_ = 1.f;
									Cut_Pic[Cut].Aim_camera.far_ = 5000.f;
									Cut_Pic[Cut].cam_per = 0.9975f;
								}
								else if (models.Get(Cafe, 0)->obj.get_anime(3).time > 15.f) {
									Cut_Pic[Cut].cam_per = 0.f;
								}
							}
							SEL++;
							//54
							if (Cut == SEL) {
								if (isFirstLoop) {
									std::string Opera = "data/umamusume/opera/model.mv1";
									std::string Doto = "data/umamusume/doto/model.mv1";
									models.Get(Mobu, 0)->obj.get_anime(4).time = (float)GetRand(100) / 10.f;
									models.Get(Mobu, 1)->obj.get_anime(4).time = (float)GetRand(100) / 10.f;
									models.Get(Mobu, 2)->obj.get_anime(4).time = (float)GetRand(100) / 10.f;
									models.Get(Doto, 0)->obj.get_anime(0).time = 30.f + (float)GetRand(100) / 10.f;
									models.Get(Cafe, 0)->obj.get_anime(4).time = (float)GetRand(100) / 10.f;
									models.Get(Opera, 0)->obj.get_anime(0).time = (float)GetRand(100) / 10.f;
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Cafe, 0)->obj.frame(7 + GetRand(3)) + VECTOR_ref::vget(0.f, 2.f, -30.f);
								}
								else {
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Cafe, 0)->obj.frame(8) + VECTOR_ref::vget(0.f, 2.f, -30.f);
									Cut_Pic[Cut].cam_per = 0.975f;
								}
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(0.5f, 260)*608.0f;
							}
							SEL++;
							//55
							if (Cut == SEL) {
								if (!isFirstLoop) {
									Cut_Pic[Cut].cam_per = 0.95f;
								}
								easing_set(&rand_vp, VECTOR_ref::vget(GetRandf(7.f), GetRandf(7.f), GetRandf(7.f)), 0.95f);
								Cut_Pic[Cut].Aim_camera.camvec = models.Get(Cafe, 0)->obj.frame(7 + GetRand(3)) + VECTOR_ref::vget(0.f, 0.3f, 20.f) + rand_vp;
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(1.5f, 190.f)*468.0f;
								if (NowTime > (LONGLONG)(1000000.f * 62.655)) {
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(0.85f);
								}
							}
							SEL++;
							//56
							if (Cut == SEL) {
								if (isFirstLoop) {
									radc = 0.f;
								}
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, 80.f + radc)*88.0f;
								radc += 12.5f / FPS;
							}
							SEL++;
							//57
							if (Cut == SEL) {
								Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon, 0)->obj.frame(9);
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, 80.f + radc)*88.0f;
								radc += 12.5f / FPS;
							}
							SEL++;
							//58
							if (Cut == SEL) {
								if (isFirstLoop) {
									Box_ALPHA = 0.f;
									per_logo = 1.f;
									for (int i = 0; i < 10; i++) {
										auto& sl = *graphs.Get(SODE5, 2 + i);
										sl.Set_Base((float)(DrawPts->disp_x / 2) + (sl.X.Ans - (float)(DrawPts->disp_x / 2)) / 10.f, 0.945f, (float)(DrawPts->disp_y / 2) + (sl.Y.Ans - (float)(DrawPts->disp_y / 2)) / 10.f, 0.925f, sl.Rad.Ans - deg2rad(90), 1.f, 0.925f, 1.f, 1.f, 0.f);
										int per_b = 127 + (int)(128.f* i / 10);
										sl.SetBright(per_b, per_b, per_b);
									}
								}
								Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon, 0)->obj.frame(9);
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, 80.f + radc)*88.0f;
								radc += 12.5f / FPS;
								if (Box_ALPHA < 1.f) {
									Box_ALPHA += 1.f / FPS;
								}
								else {
									Box_ALPHA = 1.f;
									if (per_logo > 0.f) {
										per_logo -= 1.f / FPS / 1.5f;
										graphs.Get(LOGO1, 0)->X.Per = 0.975f;
										graphs.Get(LOGO1, 0)->Y.Per = 0.975f;
										graphs.Get(LOGO1, 0)->Rad.Per = 1.f;
										graphs.Get(LOGO1, 0)->Scale.Per = 0.975f;
										graphs.Get(LOGO1, 0)->Set_Rand((float)(y_r(1000)) * per_logo, 0.8f, (float)(y_r(1000)) * per_logo, 0.8f, 10.f, 0.95f, 0.1f * per_logo, 0.f);
									}
									else {
										per_logo = 0.f;
										graphs.Get(LOGO1, 0)->X.Per = 0.95f;
										graphs.Get(LOGO1, 0)->Y.Per = 0.95f;
										graphs.Get(LOGO1, 0)->Rad.Per = 1.f;
										graphs.Get(LOGO1, 0)->Scale.Per = 0.95f;
										graphs.Get(LOGO1, 0)->Set_Rand(0.f, 0.8f, 0.f, 0.8f, 10.f, 0.95f, 0.f, 0.f);
									}
									graphs.Get(LOGO1, 0)->isDraw = (int)(per_logo*100.f) <= GetRand(99);
								}
								for (int i = 0; i < 10; i++) {
									graphs.Get(SODE5, 2 + i)->Set_Rand((float)(y_r(50)), 0.95f, (float)(y_r(50)), 0.95f, 10.f, 0.95f, 0.f, 0.f);
								}
							}
						}
						//
						graphs.Update();
						//
						if (Cut_Pic[Cut].isNextreset) { isNextreset = true; }
						easing_set(&camera_main.campos, Cut_Pic[Cut].Aim_camera.campos, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.camvec, Cut_Pic[Cut].Aim_camera.camvec, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.camup, Cut_Pic[Cut].Aim_camera.camup, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.fov, Cut_Pic[Cut].Aim_camera.fov, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.far_, Cut_Pic[Cut].Aim_camera.far_, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.near_, Cut_Pic[Cut].Aim_camera.near_, Cut_Pic[Cut].cam_per);
						camera_main.set_cam_info(camera_main.fov, camera_main.near_, camera_main.far_);
					}
					models.Update();
					if (!Time_Over()) {
						isFirstLoop = false;
						if (NowTime > Cut_Pic[Cut%Cut_Pic.size()].TIME) {
							++Cut;
							isFirstLoop = true;
							if (isNextreset) {
								reset_p = true;
								isNextreset = false;
							}
						}
						else {
							models.SetPhysics(reset_p);
							reset_p = false;
						}
					}
				}
				Effect_UseControl::Update_Effect();
				if (Time_Over()) {
					return false;
				}
				if (!BGM.check()) {
					return false;
				}
				return true;
			}
			void Dispose(void) noexcept override {
				Effect_UseControl::Dispose_Effect();
				for (auto& p : Cut_Pic) { p.Dispose(); }
				Cut_Pic.clear();
				//BGM.Dispose();
				//movie.Dispose();
			}
			void UI_Draw(void) noexcept  override {
				printfDx("Cut : %d\n", Cut);
				auto NowTime = GetNowHiPerformanceCount() - BaseTime;
				for (auto& t : Texts) {
					t.Draw(NowTime);
				}
				//
			}
			void BG_Draw(void) noexcept override {
				models.Draw_Far();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				models.Draw(false, true);
			}
			void Shadow_Draw(void) noexcept override {
				models.Draw(true, false);
			}
			void Main_Draw(void) noexcept override {
				if (Cut == 2 || Cut == 3 || Cut == 4 || Cut == 6 || Cut == 7 || Cut == 8 || Cut == 34 || Cut == 30 || (Cut >= 45 && Cut <= 49) || Cut == 52 || Cut == 53) {
					SetFogEnable(TRUE);
					SetFogColor(128, 128, 128);
					SetFogDensity(0.01f);
					if (Cut == 34) {
						SetFogStartEnd(10, 60);
					}
					if (Cut == 2 || Cut == 3 || Cut == 4 || Cut == 6 || Cut == 7 || Cut == 8) {
						SetFogStartEnd(10, 500);
					}
					if (Cut == 30 || (Cut >= 45 && Cut <= 49) || Cut == 52 || Cut == 53) {
						SetFogStartEnd(10, 300);
					}
				}
				models.Draw(false, false);
				SetFogEnable(FALSE);
				{
					if (Box_ALPHA != 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*Box_ALPHA));
						DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(0, 0, 0), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					graphs.Draw(DrawPts->disp_y);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			void LAST_Draw(void) noexcept override {
				//movie.DrawGraph(DrawPts->disp_x * 3 / 4 - y_r(200), DrawPts->disp_y * 3 / 4, FALSE);
			}
		};
	};
};