#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	class Sceneclass {
	public:
		template <class T>
		static T Leap(const T& A, const T& B, float Per) { return A + (T)((float)(B - A)*Per); }
		static float GetRandf(float m_arg) { return -m_arg + (float)(GetRand((int)(m_arg * 2.f * 10000.f))) / 10000.f; }
		static VECTOR_ref GetVector(float Xrad, float Yrad) { return VECTOR_ref::vget(-cos(deg2rad(Xrad))*sin(-deg2rad(Yrad)), sin(deg2rad(Xrad)), -cos(deg2rad(Xrad))*cos(-deg2rad(Yrad))); }
		class MAINLOOP;
		//
		class TEMPSCENE {
		private:
			VECTOR_ref Shadow_minpos;
			VECTOR_ref Shadow_maxpos;
			VECTOR_ref Light_vec;
			COLOR_F Light_color = GetColorF(0, 0, 0, 0);
		protected:
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

			TEMPSCENE() noexcept {
			}
			void Set_Next(const std::shared_ptr<Sceneclass::TEMPSCENE>& Next_scenes_ptr_t, scenes NEXT) {
				Next_ptr = Next_scenes_ptr_t;
				Next_scene = NEXT;
			}

			const VECTOR_ref& Get_Light_vec(void) const noexcept { return Light_vec; }
			cam_info& Get_Camera(void) noexcept { return camera_main; }

			virtual void Awake(void) noexcept {}
			virtual void Set(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				fov_base = deg2rad(DrawParts->use_vr ? 120 : OPTION::Instance()->Get_Fov());	//fov
				SetUseMaskScreenFlag(FALSE);//←カスタム画面でエフェクトが出なくなるため入れる
				if (DrawParts->use_vr) {
					camera_main.set_cam_info(fov_base, 0.001f, 100.f);//1P
				}
				else {
					camera_main.set_cam_info(fov_base, 0.05f, 200.f);//1P
				}

				DrawParts->Set_Light_Shadow(Shadow_maxpos, Shadow_minpos, Light_vec, [&] {Shadow_Draw_Far(); });
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
				auto* DrawParts = DXDraw::Instance();
				//音位置指定
				Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
				//影用意
				DrawParts->Ready_Shadow(camera_main.campos, [&] { Shadow_Draw(); }, [&] { Shadow_Draw_NearFar(); }, VECTOR_ref::vget(60.f, 30.f, 60.f), VECTOR_ref::vget(camera_main.far_, 40.f, camera_main.far_));//MAIN_LOOPのnearはこれ (Get_Mine()->Damage.Get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
			}
			virtual void UI_Draw(void) noexcept {}
			virtual void BG_Draw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_x, GetColor(192, 192, 192), TRUE);
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
					void SetSwitch(int on, int off) {
						On = on;
						Off = off;
					}
				};
				bool a_switch = false;
			public:
				std::vector<On_Off> Switch;
				int nowcut = 0;
				bool isFirstCut = false;

				const bool GetSwitch() { return a_switch; }
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
				void Load(std::string_view Path) {
					for (size_t i = 0; i < Max; i++) {
						if (model[i].isBase && model[i].Path == Path) {
							model[Max].Path = Path;
							model[Max].isBase = false;
							model[Max].obj = model[i].obj.Duplicate();
							Max++;
							return;
						}
					}
					model[Max].Path = Path;
					model[Max].isBase = true;
					MV1::Load(model[Max].Path, &(model[Max].obj), DX_LOADMODEL_PHYSICS_REALTIME,-127.f);
					Max++;
				}
				void Set_Anime(void) {
					for (size_t i = 0; i < Max; i++) {
						MV1::SetAnime(&(model[i].obj), model[i].obj);
					}
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
							m.isDraw = m.Cutinfo.GetSwitch();
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
							m.isDraw = m.Cutinfo.GetSwitch();
							if (m.isDraw) {
								//ブラー操作
								m.Cutinfo_[m.Cutinfo.nowcut].Blur.Update_(Cut);
								m.isblur = m.Cutinfo_[m.Cutinfo.nowcut].Blur.GetSwitch();
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
				GraphHandle Cut;
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
			std::string LOGO1 = "data/picture/logo.png";
			std::string BB = "data/picture/BACK.png";
			GraphHandle m_BB[2];
			float m_BB_x[2];
			float m_BB_x_buf[2];

			float per_logo = 1.f;
			//データ
			ModelControl models;
			GraphControl graphs;
			SoundHandle BGM;
			size_t Cut = 0;
			std::vector<Cut_Info> Cut_Pic;
			class CamAttachInfo {
			public:
				bool isradcam = false;//角度、距離指定か座標指定か
				float yradcam = 0.f;
				float xradcam = 0.f;
				float rangecam = 0.f;
				VECTOR_ref poscam;
				VECTOR_ref randcam;
			};
			Cutinfo_Common attached;
			std::vector<CamAttachInfo> camattachInfo;

			std::vector<Cut_tex> Texts;
			LONGLONG BaseTime = 0;
			LONGLONG WaitTime = 0;
			GraphHandle movie;
			bool reset_p = true;
			bool isNextreset = true;//カット終了時に物理をリセットするかどうか
			bool isFirstLoop = true;//カット最初のループ化取得
			//ビュワー
			cam_info camera_buf;
			bool isFreepos = false;
			int x_m, y_m;
			float x_rm = 0;
			float y_rm = 0;
			float r_rm = 100.f;
			//campos
			bool issetcampos = false;
			bool isradcam = false;//角度、距離指定か座標指定か
			float yradcam = 0.f;
			float xradcam = 0.f;
			float rangecam = 0.f;
			VECTOR_ref poscam;
			VECTOR_ref randcam;
			//camvec
			float zposcamv = 0.f;
			//camup
			float xcamup = 0.f;
			//
			float spd_takion = 0.6f;
			bool return_walk = false;
			//
			float Gate_Xpos = 0;
			//
			float Box_ALPHA = 0.f;
			//
			float ship_z = 0.f;
			float ship_zadd = 0.f;
			//
			float range_b = 0.f;
			//
			float camxb_0 = 0.f;
			float camzb_1 = 0.f;
			float camzb_4 = 0.f;
			float camxb_9 = 0.f;
			float camyb_14 = 0.f;
			float camxb_15 = 0.f;
			//
		private:
			bool Time_Over() { return Cut >= Cut_Pic.size(); }
		public:
			using TEMPSCENE::TEMPSCENE;
			void Awake(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				TEMPSCENE::Awake();
				//
				camera_buf.campos = VECTOR_ref::vget(0, 20, -20);
				camera_buf.camvec = VECTOR_ref::vget(0, 20, 0);
				camera_buf.camup = VECTOR_ref::up();
				camera_buf.set_cam_info(deg2rad(15), 1.0f, 200.f);
				//
				{
					SetUseASyncLoadFlag(FALSE);
					{
						graphs.Load((float)(y_r(1920 * 1 / 2)), (float)(y_r(1080 * 1 / 2)), 0, 1.f, 0.5f, LOGO1);
						m_BB[0] = GraphHandle::Load(BB);
						m_BB_x[0] = DrawParts->disp_x / 2;
						m_BB_x_buf[0] = 0.f;
						m_BB[1] = GraphHandle::Load(BB);
						m_BB_x[1] = -DrawParts->disp_x / 2;
						m_BB_x_buf[1] = 0.f;
					}
					LONGLONG StartF = 0;
					LONGLONG ContiF = 0;
					int mdata = FileRead_open("data/Cut.txt", FALSE);
					int cnt = 0;
					SetUseASyncLoadFlag(TRUE);
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
								for (int i = 0; i < std::stoi(args[1]); i++) {
									models.Load(args[0]);					//origin
								}
							}
						}
						//カット
						{
							if (func.find("SetCut") != std::string::npos) {
								Cut_Pic.resize(Cut_Pic.size() + 1);
								Cut_Pic.back().TIME = (LONGLONG)(1000000.f * std::stof(args[0]));
								Cut_Pic.back().Cut = GraphHandle::Load("data/cut/" + std::to_string(Cut_Pic.size()) + ".bmp");
							}
							else if (func.find("SetCampos_NoneRad") != std::string::npos) {
								Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
							}
							else if (func.find("SetCampos_ByRad") != std::string::npos) {
								Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec + GetVector(std::stof(args[0]), std::stof(args[1])) *std::stof(args[2]);
							}
							else if (func.find("SetCampos_Attach") != std::string::npos) {
								attached.Switch.resize(attached.Switch.size() + 1);
								attached.Switch.back().SetSwitch((int)(Cut_Pic.size()) - 1, std::stoi(args[0]));
								camattachInfo.resize(camattachInfo.size() + 1);
								camattachInfo.back().isradcam = (args[1].find("TRUE") != std::string::npos);
								if (camattachInfo.back().isradcam) {
									camattachInfo.back().xradcam = std::stof(args[2]);
									camattachInfo.back().yradcam = std::stof(args[3]);
									camattachInfo.back().rangecam = std::stof(args[4]);
								}
								else {
									camattachInfo.back().poscam.Set(std::stof(args[2]), std::stof(args[3]), std::stof(args[4]));
								}
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
										t->Cutinfo.Switch.back().SetSwitch((int)(Cut_Pic.size()) - 1, std::stoi(args[2]));
										t->Cutinfo_.resize(t->Cutinfo_.size() + 1);
									}
								}
								else {
									auto* t = graphs.Get(args[0], std::stoi(args[1]));
									t->Cutinfo.Switch.resize(t->Cutinfo.Switch.size() + 1);
									t->Cutinfo.Switch.back().SetSwitch((int)(Cut_Pic.size()) - 1, std::stoi(args[2]));
									t->Cutinfo_.resize(t->Cutinfo_.size() + 1);
								}
							}
							else if (func.find("SetGraphBlur") != std::string::npos) {
								auto* t = graphs.Get(args[0], std::stoi(args[1]));
								t->Cutinfo_.back().Blur.Switch.resize(t->Cutinfo_.back().Blur.Switch.size() + 1);
								t->Cutinfo_.back().Blur.Switch.back().SetSwitch((int)(Cut_Pic.size()) - 1, std::stoi(args[2]));
							}
							else if (func.find("SetDrawModel") != std::string::npos) {
								size_t in_str = args[1].find("~");
								if (in_str != std::string::npos) {
									int start_t = std::stoi(args[1].substr(0, in_str));
									int end_t = std::stoi(args[1].substr(in_str + 1));
									for (int i = start_t; i <= end_t; i++) {
										auto* t = models.Get(args[0], i);
										t->Cutinfo.Switch.resize(t->Cutinfo.Switch.size() + 1);
										t->Cutinfo.Switch.back().SetSwitch((int)(Cut_Pic.size()) - 1, std::stoi(args[2]));
										t->Cutinfo_.resize(t->Cutinfo_.size() + 1);
									}
								}
								else {
									auto* t = models.Get(args[0], std::stoi(args[1]));
									t->Cutinfo.Switch.resize(t->Cutinfo.Switch.size() + 1);
									t->Cutinfo.Switch.back().SetSwitch((int)(Cut_Pic.size()) - 1, std::stoi(args[2]));
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
								DrawParts->Screen_Flip();
							}
							NowTime = GetNowHiPerformanceCount();
						}
						cnt++;
					}
					FileRead_close(mdata);
					SetUseASyncLoadFlag(FALSE);
					printfDx("非同期読み込みオブジェクトの読み込み待ち…\n");
					int prenum = GetASyncLoadNum();
					while (ProcessMessage() == 0) {
						if (prenum!=GetASyncLoadNum()) {
							printfDx("非同期読み込みオブジェクトの読み込み待ち…\n");
							prenum = GetASyncLoadNum();
							continue;
						}
						if (GetASyncLoadNum() == 0) {
							break;
						}
					}
					if (ProcessMessage() == 0) {
						GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
						printfDx("ロード全部完了　キーを押してください : total = [%7.3f s]\n", float((GetNowHiPerformanceCount() - TotalTime) / 1000) / 1000.f);
						DrawParts->Screen_Flip();
					}
				}
				WaitKey();
				//モデルの事前処理
				models.Set_Anime();
				models.Set();
			}
		public:
			void Set(void) noexcept override {
				auto* PostPassParts = PostPassEffect::Instance();
				TEMPSCENE::Set_EnvLight(VECTOR_ref::vget(500.f, 50.f, 500.f), VECTOR_ref::vget(-500.f, -50.f, -500.f), VECTOR_ref::vget(-0.3f, -0.5f, -0.2f), GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				TEMPSCENE::Set();
				std::string SUN = "data/model/sun/model.mv1";
				models.Get(SUN, 0)->obj.SetMatrix( MATRIX_ref::RotVec2(VECTOR_ref::up(), (VECTOR_ref)(Get_Light_vec().Norm())) * MATRIX_ref::Mtrans(Get_Light_vec().Norm() * -1500.f));
				Cut = 13;
				Cut = 0;
				SetUseASyncLoadFlag(FALSE);
				BGM = SoundHandle::Load("data/sound2.wav");
				///*
				movie = GraphHandle::Load("data/base_movie.mp4");
				PauseMovieToGraph(movie.get());
				//*/
				//プレイ用意
				PostPassParts->Set_Bright(255, 255, 255);
				BaseTime = GetNowHiPerformanceCount() - (Cut > 0 ? Cut_Pic[Cut - 1].TIME : 0);
				WaitTime = (Cut != 0) ? 0 : 1000000;
			}
			bool isfast = true;
			bool Update(void) noexcept override {
				auto* PostPassParts = PostPassEffect::Instance();
				//auto* DrawParts = DXDraw::Instance();
				TEMPSCENE::Update();
				auto NowTime = GetNowHiPerformanceCount() - BaseTime;
				{
					//待ち
					if (NowTime < WaitTime) {
						isfast = true;
						return true;
					}
					else {
						if (isfast) {
							//BGM.play(DX_PLAYTYPE_BACK, TRUE);
							//BGM.vol(64);
							PlayMovieToGraph(movie.get(), 2, DX_MOVIEPLAYTYPE_BCANCEL);
							//ChangeMovieVolumeToGraph(0, movie.get());
							SeekMovieToGraph(movie.get(), (int)(Cut > 0 ? Cut_Pic[Cut - 1].TIME : 0) / 1000);
						}
						isfast = false;
					}

					if (!Time_Over()) {
						if (attached.Update_((int)Cut)) {
							attached.Update_((int)Cut);
						}
						models.FirstUpdate((int)Cut, isFirstLoop);
						graphs.FirstUpdate((int)Cut, isFirstLoop);

						issetcampos = attached.GetSwitch();
						isradcam = (camattachInfo.size()>0) ? camattachInfo[attached.nowcut].isradcam : false;
						if (isFirstLoop && attached.isFirstCut) {
							if (isradcam) {
								xradcam = camattachInfo[attached.nowcut].xradcam;
								yradcam = camattachInfo[attached.nowcut].yradcam;
								rangecam = camattachInfo[attached.nowcut].rangecam;
							}
							else {
								poscam = camattachInfo[attached.nowcut].poscam;
							}
						}
						int SEL = 0;
						{
							//0
							if (Cut == SEL) {
								if (isFirstLoop) {
									camxb_0 = 0.f;
								}
								else {
									Cut_Pic[Cut].Aim_camera.camvec.Set(-1.f + camxb_0, 3.f, 0.f);
									Cut_Pic[Cut].Aim_camera.campos.Set(-1.f + camxb_0, 5.f, -16.f);
									Cut_Pic[Cut].cam_per = 0.95f;
									camxb_0 += 2.f / FPS;
								}
							}
							SEL += 1;
							//1
							if (Cut == SEL) {
								if (isFirstLoop) {
									camzb_1 = 0.f;
								}
								else {
									Cut_Pic[Cut].Aim_camera.camvec.Set(0.f, 3.f, -7.f+ camzb_1);
									Cut_Pic[Cut].Aim_camera.campos.Set(-14.f, 5.f, -14.f + camzb_1);
									Cut_Pic[Cut].cam_per = 0.95f;
									camzb_1 += -10.f / FPS;
								}
							}
							SEL += 2;
							//3
							if (Cut == SEL) {
								std::string TEIO = "data/umamusume/teio/model.mv1";

								range_b = 10.f;
								easing_set(&randcam, VECTOR_ref::vget(GetRandf(range_b), GetRandf(range_b), GetRandf(range_b)), 0.8f);

								easing_set(&Cut_Pic[Cut].Aim_camera.camvec, models.Get(TEIO, 0)->obj.frame(162) + VECTOR_ref::vget(0, 15.f, 0) + randcam, 0.8f);//右ひざ
							}
							SEL += 1;
							//4
							if (Cut == SEL) {
								if (isFirstLoop) {
									camzb_4 = 0.f;
								}
								else {
									Cut_Pic[Cut].Aim_camera.camvec.Set(-1.5f, 35.f, -45.f + camzb_4);
									Cut_Pic[Cut].Aim_camera.campos.Set(10.f, 60.f, -35.f + camzb_4);
									Cut_Pic[Cut].cam_per = 0.95f;
									camzb_4 += -30.f / FPS;
								}
							}
							SEL += 1;
							//5
							if (Cut == SEL) {
								if (!isFirstLoop) {
									std::string TEIO = "data/umamusume/teio/model.mv1";

									easing_set(&Cut_Pic[Cut].Aim_camera.camvec, models.Get(TEIO, 0)->obj.frame(6) + VECTOR_ref::vget(0, 3.f, 0), 0.995f);//右ひざ
									Cut_Pic[Cut].cam_per = 0.95f;
								}
							}
							SEL += 4;
							//9
							if (Cut == SEL) {
								if (isFirstLoop) {
									camxb_9 = 0.f;
									randcam.clear();
								}else {
									std::string TEIO = "data/umamusume/teio/model.mv1";

									easing_set(&Cut_Pic[Cut].Aim_camera.camvec, models.Get(TEIO, 0)->obj.frame(6) + VECTOR_ref::vget(0, 3.f, 0), 0.995f);//右ひざ
									range_b = 20.f;
									easing_set(&randcam, VECTOR_ref::vget(GetRandf(range_b), GetRandf(range_b / 3.f), GetRandf(0.f)), 0.9f);

									easing_set(&Cut_Pic[Cut].Aim_camera.campos, VECTOR_ref::vget(10.f - camxb_9, 10.f, 60.f) + randcam, 0.9f);
									Cut_Pic[Cut].cam_per = 0.95f;
									camxb_9 += 15.f / FPS;
								}
							}
							SEL += 4;
							//13
							if (Cut == SEL) {
								std::string LOGO = "data/model/logo/model.mv1";
								std::string MAP =  "data/model/map/model.mv1";
								if (NowTime - WaitTime > (LONGLONG)(1000000.f * 19.55f)) {
									easing_set(&Box_ALPHA, 1.f, 0.95f);
									easing_set(&models.Get(LOGO, 0)->OpacityRate, 0.f, 0.95f);
									graphs.Get(LOGO1, 0)->Alpha.Set(1.f, 0.95f);
								}
								else {
									if (NowTime - WaitTime > (LONGLONG)(1000000.f * 16.55f)) {
										models.Get(LOGO, 0)->OpacityRate = 1.f;
									}
									else {
										models.Get(LOGO, 0)->OpacityRate = 0.f;
									}
									graphs.Get(LOGO1, 0)->Alpha.Set(0.f, 0.f);
								}
							}
							SEL++;
							//14
							if (Cut == SEL) {
								if (isFirstLoop) {
									PostPassParts->Set_Bright(255, 216, 192);
									camyb_14 = 0.f;
									Box_ALPHA = 0.f;
									randcam.clear();

									std::string Nice = "data/umamusume/nice/model.mv1";
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Nice, 0)->obj.frame(92);
								}
								else {
									range_b = 10.f;
									easing_set(&randcam, VECTOR_ref::vget(GetRandf(range_b), GetRandf(range_b / 2.f), GetRandf(0.f)), 0.9f);

									std::string Nice = "data/umamusume/nice/model.mv1";
									easing_set(&Cut_Pic[Cut].Aim_camera.camvec, models.Get(Nice, 0)->obj.frame(92)+ randcam, 0.95f);//右ひざ

									easing_set(&Cut_Pic[Cut].Aim_camera.campos, VECTOR_ref::vget(0.f, 10.f+ camyb_14, -60.f), 0.9f);
									Cut_Pic[Cut].cam_per = 0.95f;
									camyb_14 += 1.f / FPS;
								}
							}
							SEL++;
							//15
							if (Cut == SEL) {
								if (isFirstLoop) {
									PostPassParts->Set_Bright(255, 255, 255);
									randcam.clear();
									camxb_15 = -3.f;
									std::string Tanhoiza = "data/umamusume/tanhoiza/model.mv1";
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tanhoiza, 0)->obj.frame(11) + VECTOR_ref::vget(0.f, -1.5f, 0.f);
								}
								else {
									range_b = 3.f;
									easing_set(&randcam, VECTOR_ref::vget(GetRandf(range_b), GetRandf(range_b / 2.f), GetRandf(0.f)), 0.9f);

									std::string Tanhoiza = "data/umamusume/tanhoiza/model.mv1";
									easing_set(&Cut_Pic[Cut].Aim_camera.camvec, models.Get(Tanhoiza, 0)->obj.frame(11) + VECTOR_ref::vget(0.f, -1.5f, 0.f) + randcam, 0.95f);//右ひざ

									Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + VECTOR_ref::vget(camxb_15, 0.f, -60.f);
									camxb_15 += 1.f / FPS;
									easing_set(&Cut_Pic[Cut].Aim_camera.fov, deg2rad(15), 0.95f);
								}
							}
							SEL++;
							//16
							if (Cut == SEL) {
								if (isFirstLoop) {
									randcam.clear();
									std::string Tanhoiza = "data/umamusume/tanhoiza/model.mv1";
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tanhoiza, 0)->obj.frame(11);
								}
								else {
									range_b = 3.f;
									easing_set(&randcam, VECTOR_ref::vget(GetRandf(range_b), GetRandf(range_b / 2.f), GetRandf(range_b)), 0.9f);

									std::string Tanhoiza = "data/umamusume/tanhoiza/model.mv1";
									easing_set(&Cut_Pic[Cut].Aim_camera.camvec, models.Get(Tanhoiza, 0)->obj.frame(11) + randcam, 0.95f);//右ひざ
									Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + VECTOR_ref::vget(30.f, 0.f, -10.f);

									Cut_Pic[Cut].Aim_camera.fov = deg2rad(25);
								}
							}
							SEL++;
						}
						if (issetcampos) {
							if (isradcam) {
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(xradcam, yradcam)*rangecam + randcam;
							}
							else {
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + poscam + randcam;
							}
						}
						graphs.Update();
						if (Cut_Pic[Cut].isNextreset) { isNextreset = true; }
						easing_set(&camera_buf.campos, Cut_Pic[Cut].Aim_camera.campos, Cut_Pic[Cut].cam_per);
						easing_set(&camera_buf.camvec, Cut_Pic[Cut].Aim_camera.camvec, Cut_Pic[Cut].cam_per);
						easing_set(&camera_buf.camup, Cut_Pic[Cut].Aim_camera.camup, Cut_Pic[Cut].cam_per);
						easing_set(&camera_buf.fov, Cut_Pic[Cut].Aim_camera.fov, Cut_Pic[Cut].cam_per);
						easing_set(&camera_buf.far_, Cut_Pic[Cut].Aim_camera.far_, Cut_Pic[Cut].cam_per);
						easing_set(&camera_buf.near_, Cut_Pic[Cut].Aim_camera.near_, Cut_Pic[Cut].cam_per);
						camera_buf.set_cam_info(camera_buf.fov, camera_buf.near_, camera_buf.far_);


						isFreepos = (CheckHitKey(KEY_INPUT_SPACE) != 0);
						int x_o = x_m;
						int y_o = y_m;
						GetMousePoint(&x_m, &y_m);

						if (!isFreepos) {
							camera_main = camera_buf;
						}
						else {
							x_rm = std::clamp(x_rm + (float)(y_m - y_o) / 10.f, -20.f, 80.f);
							y_rm += (float)(x_m - x_o) / 10.f;

							r_rm = std::clamp(r_rm + (float)(GetMouseWheelRotVol())*10.f, 5.f, 300.f);

							camera_main.set_cam_info(deg2rad(45), 1.f, 1000.f);
							camera_main.camvec = VECTOR_ref::vget(0, 0, 0);
							camera_main.campos = camera_main.camvec + GetVector(x_rm, y_rm)*r_rm;
						}
					}
					models.Update();
					if (!Time_Over()) {
						isFirstLoop = false;
						if ((NowTime - WaitTime) > Cut_Pic[Cut%Cut_Pic.size()].TIME) {
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
				/*
				if (!BGM.check()) {
					return false;
				}
				//*/
				return true;
			}
			void Dispose(void) noexcept override {
				Effect_UseControl::Dispose_Effect();
				for (auto& p : Cut_Pic) { p.Dispose(); }
				Cut_Pic.clear();
				//BGM.Dispose();
				movie.Dispose();
			}
			void UI_Draw(void) noexcept  override {
				//printfDx("Cut : %d\n", Cut);
				if (!isFreepos) {
					auto NowTime = GetNowHiPerformanceCount() - BaseTime;
					if (NowTime < 1000000) {
						return;
					}
					for (auto& t : Texts) {
						t.Draw(NowTime - WaitTime);
					}
				}
				//
			}
			void BG_Draw(void) noexcept override {
				if (!Time_Over()) {
					auto* DrawParts = DXDraw::Instance();
					//if (Cut == 14) {
					//	DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_y, GetColor(128, 128, 142), TRUE);
					//}
				}
				models.Draw_Far();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				models.Draw(false, true);
			}
			void Shadow_Draw(void) noexcept override {
				models.Draw(true, false);
			}
			void Main_Draw(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				if (!Time_Over()) {
					auto* DrawParts = DXDraw::Instance();
					//Cut_Pic[Cut%Cut_Pic.size()].Cut.DrawExtendGraph(0, 0, DrawParts->disp_x, DrawParts->disp_y, false);
					/*
					if (Cut == 14) {
						auto NowTime = GetNowHiPerformanceCount() - BaseTime;
						if ((NowTime - WaitTime) > (LONGLONG)(1000000.f * 25.5f)) {
							m_BB_x_buf[0] += -15.f / FPS;
							m_BB_x_buf[1] += 15.f / FPS;
							easing_set(&m_BB_x[0], m_BB_x_buf[0], 0.9f);
							easing_set(&m_BB_x[1], m_BB_x_buf[1], 0.9f);
						}
						m_BB[0].DrawRotaGraph(DrawParts->disp_x / 2 + m_BB_x[0], DrawParts->disp_y / 2, 1.f, 0.f, true);
						m_BB[1].DrawRotaGraph(DrawParts->disp_x / 2 + m_BB_x[1], DrawParts->disp_y / 2, 1.f, DX_PI_F, true);
					}
					//*/
				}
				{
					SetFogEnable(TRUE);
					SetFogColor(128, 128, 128);
					SetFogDensity(0.01f);
					SetFogStartEnd(200, 300000);
				}
				models.Draw(false, false);
				if (isFreepos) {
					VECTOR_ref vec = (camera_buf.camvec - camera_buf.campos);
					float range = vec.size();
					vec = vec.Norm()*camera_buf.far_;
					DrawCone3D(
						camera_buf.campos.get(),
						(camera_buf.campos + vec).get(),
						std::tan(camera_buf.fov) * range,
						8, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
				}

				SetFogEnable(FALSE);
				if (!isFreepos) {
					if (Box_ALPHA != 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*Box_ALPHA));
						DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_y, GetColor(0, 0, 0), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					graphs.Draw(DrawParts->disp_y);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			void LAST_Draw(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				movie.DrawExtendGraph(DrawParts->disp_x * 3 / 4, DrawParts->disp_y * 3 / 4, DrawParts->disp_x, DrawParts->disp_y, FALSE);
			}
		};
	};
};