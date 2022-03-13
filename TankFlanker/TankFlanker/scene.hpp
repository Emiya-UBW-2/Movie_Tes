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
					bless += deg2rad(float(100 + GetRand(600)) / 100.f * Frame_Rate / FPS * GameSpeed);
					bless_ratio -= (0.03f / 90.f) * Frame_Rate / FPS * GameSpeed;
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
		class LoadScriptClass {
		private:
			class ARG {
			public:
				std::string Base;
				std::string After;
			};
		private:
			std::vector<ARG> args2;
		public:
			std::string func;
			std::vector<std::string> args;
		public:
			const auto* GetArgFromPath(std::string_view Path) {
				const ARG* sel = nullptr;
				for (const auto& a : args2) {
					if (a.After == Path) {
						sel = &a;
						break;
					}
				}
				return sel;
			}
			void Calc() {
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
				//
			}
		};
		//
		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			class Cutinfo_Common {
			public:
				class On_Off {
				public:
					int On = 0;
					int Off = 0;
					void SetSwitch(int on, int off) {
						On = on;
						Off = off;
					}
				};
			private:
				bool a_switch = false;
			public:
				std::vector<On_Off> Switch;
				int nowcut = 0;
				bool isFirstCut = false;

				const bool GetSwitch() { return a_switch; }
				void Start(int Cut) {
					this->nowcut = 0;
					while (true) {
						if (this->Switch.size() > this->nowcut) {
							auto& inf_b = this->Switch[this->nowcut];
							if (Cut > inf_b.On) {
								this->nowcut++;
							}
							else {
								break;
							}
						}
						else {
							break;
						}
					}
				}
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

				void SortSwitch() {
					std::sort(Switch.begin(), Switch.end(), [](On_Off a, On_Off b) { return a.On < b.On; });
				}
				void Insert(int ID, int Start, int End) {
					Switch.insert(Switch.begin() + ID, On_Off());
					Switch[ID].SetSwitch(Start, End);
				}
			};
			class Cutinfo_g {
			public:
				Cutinfo_Common Blur;
				int animsel = -1;
				bool isloop = false;
				float animspeed = 0.f;
				MATRIX_ref mat_p;
				//matrix用情報
				VECTOR_ref pos_p;
				float Yrad1_p;
				float Yrad2_p;

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
					bool isBGModel = false;
					bool IsNearShadow = true;
					bool IsFarShadow = false;
					bool isBase = true;
					std::string Path;
					int BaseID = 0;

					size_t numBase = 0;
					float PhysicsSpeed = 1.f;

					float b_run = 0;
					float b_runrange = 0;

					std::vector<std::pair<int, std::string>> FrameNum;
					MV1 obj;
					moves move;
				//private:
					bool isDraw = false;
					bool isEndLoad = false;
				public:
					Cutinfo_Common Cutinfo;
					std::vector<Cutinfo_g> CutDetail;//カットの詳細
					float OpacityRate = 1.f;
					int Anim_Sel = 0;

					Model() {
						isDraw = false;
						isEndLoad = false;
					}

					void AddFrame(std::string_view FrameName) {
						size_t siz = obj.frame_num();
						for (size_t i = 0; i < siz; i++) {
							if (obj.frame_name(i) == FrameName) {
								FrameNum.resize(FrameNum.size() + 1);
								FrameNum.back().first = (int)i;
								FrameNum.back().second = FrameName;
								return;
							}
						}
					}
					const auto GetFrame(std::string_view FrameName) {
						for (auto& F : FrameNum) {
							if (F.second == FrameName) {
								return obj.frame(F.first);
							}
						}
						return VECTOR_ref::zero();
					}

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
								this->obj.PhysicsCalculation(1000.0f / (FPS * std::max(GameSpeed, 0.1f) * (120.f / 60.f)) * this->PhysicsSpeed);
							}
						}
					}
				};
			private:
				std::vector<Model> model;
				size_t Max = 0;
			public:
				const auto& GetMax() noexcept { return Max; }
				const auto& GetModel() noexcept { return model; }

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
							model[i].numBase++;
							model[Max].BaseID = model[i].numBase;
							Max++;
							return;
						}
					}
					model[Max].Path = Path;
					model[Max].isBase = true;
					model[Max].numBase = 0;
					model[Max].BaseID = 0;
					MV1::Load(model[Max].Path, &(model[Max].obj), DX_LOADMODEL_PHYSICS_REALTIME);
					Max++;
				}
				Model* Get(std::string_view Path, size_t Sel = 0) {
					for (size_t i = 0; i < Max; i++) {
						if (model[i].Path == Path && model[i].BaseID == Sel) {
							return &(model[i]);
						}
					}
					return nullptr;
				}
				void Start(int Cut) {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.Cutinfo.Start(Cut);
					}
				}
				void FirstUpdate(int Cut, bool isFirstLoop) {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						while (true) {
							bool tt = m.Cutinfo.Update_(Cut);
							m.isDraw = m.Cutinfo.GetSwitch();
							if (m.isDraw) {
								auto& inf = m.CutDetail[m.Cutinfo.nowcut];
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
									m.UpdateAnim(inf.animsel, inf.isloop, inf.animspeed * GameSpeed);
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
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.SetPhysics(reset_p);
					}
				}

				void Set() {
					for (size_t i = 0; i < Max; i++) {
						//
						if (!model[i].isEndLoad && CheckHandleASyncLoad(model[i].obj.get()) == FALSE) {
							model[i].isEndLoad = true;

							model[i].AddFrame("上半身2");
							model[i].AddFrame("首");
							model[i].AddFrame("頭");
							model[i].AddFrame("右目");
							model[i].AddFrame("左目");
							model[i].AddFrame("右人指２");
							model[i].AddFrame("右ひざ");

							MV1::SetAnime(&(model[i].obj), model[i].obj);
						}
					}
				}
				void Update() {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.Update();
					}
				}
				void Draw_Far() {
					auto fog_e = GetFogEnable();
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					{
						for (size_t i = 0; i < Max; i++) {
							auto& m = model[i];
							if (m.isBGModel) {
								m.Draw();
							}
						}
					}
					SetUseLighting(TRUE);
					SetFogEnable(fog_e);
				}
				void Draw(bool innearshadow, bool infarshadow) {
					if (infarshadow) {
						for (size_t i = 0; i < Max; i++) {
							auto& m = model[i];
							if (!m.isBGModel && m.IsFarShadow) {
								m.Draw();
							}
						}
					}
					else if (innearshadow) {
						for (size_t i = 0; i < Max; i++) {
							auto& m = model[i];
							if (!m.isBGModel && m.IsNearShadow) {
								m.Draw();
							}
						}
					}
					else {
						for (size_t i = 0; i < Max; i++) {
							auto& m = model[i];
							if (!m.isBGModel) {
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
								easing_set_SetSpeed(&this->Ans, GetRandf(this->Base), this->Per);
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
							easing_set_SetSpeed(&this->Ans, this->Base + this->Rand_.Ans, this->Per);
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
					bool isDraw = false;
					bool isblur = false;
					int xsize = -1;
					int ysize = -1;
				public:
					Cutinfo_Common Cutinfo;
					std::vector<Cutinfo_g> CutDetail;//オンにするカット
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
				void Start(int Cut) {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.Cutinfo.Start(Cut);
						for (auto& B : m.CutDetail) {
							B.Blur.Start(Cut);
						}
					}
				}
				void FirstUpdate(int Cut, bool isFirstLoop) {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						while (true) {
							bool tt = m.Cutinfo.Update_(Cut);
							m.isDraw = m.Cutinfo.GetSwitch();
							if (m.isDraw) {
								auto& inf = m.CutDetail[m.Cutinfo.nowcut];
								//ブラー操作
								inf.Blur.Update_(Cut);
								m.isblur = inf.Blur.GetSwitch();

								if (isFirstLoop && m.Cutinfo.isFirstCut) {
									//最初のアニメーション動作
									if (inf.OpacityRate >= 0.f) {
										m.Alpha.Set(inf.OpacityRate, 0.f);
									}
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
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.isDraw = false;
					}
				}
				void Update() {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.Update();
					}
				}
				void Draw(int Disp_y) {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.Draw((float)Disp_y / m.ysize);
					}
				}
			};
			class Cut_Info_First {
			public:
				bool UsePrevAim{ false };
				bool IsResetPhysics{ false };
				bool UseForcus{ false };

				cam_info Aim_camera;
				float cam_per{ 0.f };
				LONGLONG TIME = 0;
				//GraphHandle Cut;

				std::string ForcusModel;
				std::string ForcusFrame;
				VECTOR_ref ForcusAdd;

				bool isResetRandCampos{ false };
				bool isResetRandCamvec{ false };
				bool isResetRandCamup{ false };

				Cut_Info_First() {
					Aim_camera.campos = VECTOR_ref::vget(0, 10, -30);
					Aim_camera.camvec = VECTOR_ref::vget(0, 10, 0);
					Aim_camera.camup = VECTOR_ref::up();
					Aim_camera.set_cam_info(deg2rad(15), 1.f, 300.f);
					cam_per = 0.95f;
					IsResetPhysics = false;
				}
				~Cut_Info_First() {
				}
			};
			class Cut_Info_Update {
			public:
				bool isUseNotFirst{ false };
				bool IsUsePrevBuf{ false };
				bool UseForcus{ false };

				float campos_per{ 0.f };
				float camvec_per{ 0.f };
				float camup_per{ 0.f };
				float fov_per{ 0.f };

				cam_info CameraNotFirst;

				float NotFirst_per = -1.f;

				float m_RandcamupPer;
				VECTOR_ref m_RandcamupSet;
				float m_RandcamvecPer;
				VECTOR_ref m_RandcamvecSet;
				float m_RandcamposPer;
				VECTOR_ref m_RandcamposSet;

				std::string ForcusModel;
				std::string ForcusFrame;
				VECTOR_ref ForcusAdd;

				Cut_Info_Update() {
					isUseNotFirst = false;
					IsUsePrevBuf = false;
					UseForcus = false;

					campos_per = 1.f;
					camvec_per = 1.f;
					camup_per = 1.f;
					fov_per = 1.f;

					NotFirst_per = -1.f;

					m_RandcamupPer = 0.f;
					m_RandcamupSet.clear();
					m_RandcamvecPer = 0.f;
					m_RandcamvecSet.clear();
					m_RandcamposPer = 0.f;
					m_RandcamposSet.clear();

					ForcusModel = "";
					ForcusFrame = "";
					ForcusAdd.clear();
				}

				~Cut_Info_Update() {
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
				void Draw(LONGLONG nowTimeWait) {
					if (this->START_TIME < nowTimeWait && nowTimeWait < this->END_TIME) {
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

			class CamAttachDetail {
			public:
				bool isradcam = false;//角度、距離指定か座標指定か
				float yradcam = 0.f;
				float xradcam = 0.f;
				float rangecam = 0.f;
				VECTOR_ref poscam;
			};
		private:
			std::string LOGO1 = "data/picture/logo.png";
			std::string BB = "data/picture/pic.png";

			std::string GATE = "data/model/map/model_gate.mv1";
			std::string LOGO = "data/model/logo/model.mv1";
			std::string SUN = "data/model/sun/model.mv1";

			std::string Rudolf2 = "data/umamusume/rudolf2/model.mv1";
			std::string Tanhoiza = "data/umamusume/tanhoiza/model.mv1";
			std::string spe = "data/umamusume/spe/model.mv1";
			std::string szk = "data/umamusume/suzuka/model.mv1";
		private:
			//スクリプト読み込み
			LoadScriptClass LSClass;
			//カット
			size_t Cut = 0;
			std::vector<Cut_Info_First> m_CutInfo;
			std::vector<Cut_Info_Update> m_CutInfoUpdate;

			LONGLONG BaseTime = 0, WaitTime = 0, NowTimeWait = 0;
			bool ResetPhysics = true;
			bool isFirstLoop = true;//カット最初のループか
			bool isfast = true;
			//
			Cutinfo_Common attached;
			std::vector<CamAttachDetail> attachedDetail;
			//
			std::vector<Cut_tex> Texts;
			//データ
			ModelControl models;
			GraphControl graphs;
			SoundHandle BGM;
			int BGM_Frequency;
			GraphHandle m_BB;
			//ビュワー
			GraphHandle movie;
			switchs LookMovie;
			//
			switchs LookEditer;
			switchs SpeedUp;
			switchs SpeedDown;
			switchs Start;
			switchs MouseClick;
			//
			int X_now = 0;
			int CutNow = 0;
			int SetAnimStart = -1;
			int SetAnimEnd = -1;
			ModelControl::Model* ChangeModel = nullptr;
			ModelControl::Model* ModelEdit = nullptr;
			size_t ModelEditCutNum = 0;
			bool ModelEditMode = false;
			bool ModelEditIn = false;
			//
			int spd_x = 10;
			cam_info camera_buf;
			bool isFreepos = false;
			int x_m, y_m;
			float x_rm = 0;
			float y_rm = 0;
			float r_rm = 100.f;
			//campos
			bool issetcampos = false;
			bool isradcam = false;//角度、距離指定か座標指定か
			float yradcam = 0.f, xradcam = 0.f, rangecam = 0.f;
			VECTOR_ref poscam;
			//
			VECTOR_ref m_RandcamupBuf;
			VECTOR_ref m_RandcamvecBuf;
			VECTOR_ref m_RandcamposBuf;
			//
			float Box_ALPHA = 0.f;
			float Box_ALPHA2 = 0.f;
			//後で消す
			float camxb_15 = 0.f;
			float camupxb_18 = 0.f;
			//
			std::array<std::string, 3> ModelType{ "SKY_TRUE","NEAR_FALSE","FAR_TRUE" };
		private:
			//スクリプト読み込み処理
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
				LSClass.args.clear();
				LSClass.func = func_t;
				{
					// //を削除
					size_t sls = LSClass.func.find("//");
					if (sls != std::string::npos) { LSClass.func = LSClass.func.substr(0, sls); }
					//いらない要素を排除
					Sub_Func(LSClass.func, '{');
					Sub_Func(LSClass.func, '}');
					Sub_Func(LSClass.func, ' ');
					Sub_Func(LSClass.func, '\t');
					Sub_Func(LSClass.func, ';');
					Sub_Func(LSClass.func, '\"');
				}
				//()と,で囲われた部分から引数を取得
				if (LSClass.func != "") {
					std::string tmp_func = LSClass.func;
					size_t left = tmp_func.find("(");
					size_t right = tmp_func.rfind(")");
					if (left != std::string::npos && right != std::string::npos) {
						tmp_func = tmp_func.substr(left + 1, right - 1 - left);
					}
					while (true) {
						size_t in_str = tmp_func.find(",");
						if (in_str == std::string::npos) {
							LSClass.args.emplace_back(tmp_func);
							break;
						}
						else {
							std::string arg = tmp_func.substr(0, in_str);
							tmp_func = tmp_func.substr(in_str + 1);
							LSClass.args.emplace_back(arg);
						}
					}
				}
			}
		public:
			//Getter
			bool Time_Over() { return Cut >= m_CutInfo.size(); }
			const auto GetCut() { return Cut; }
		public:
			using TEMPSCENE::TEMPSCENE;
			void Awake(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				TEMPSCENE::Awake();
				//
				camera_buf.campos = VECTOR_ref::vget(0, 20, -20);
				camera_buf.camvec = VECTOR_ref::vget(0, 20, 0);
				camera_buf.camup = VECTOR_ref::up();
				camera_buf.set_cam_info(deg2rad(15), 1.f, 200.f);
				//
				{
					SetUseASyncLoadFlag(FALSE);
					{
						graphs.Load(
							(float)(y_r(1920 * 1 / 2)), (float)(y_r(1080 * 1 / 2)),
							0, 1.f, 0.5f, LOGO1);
						m_BB = GraphHandle::Load(BB);
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
						if (LSClass.func == "") { continue; }
						auto& args = LSClass.args;
						auto& func = LSClass.func;
						//変数登録
						LSClass.Calc();//todo
						//モデル読み込み
						{
							if (LSClass.func.find("LoadModel") != std::string::npos) {
								for (int i = 0; i < std::stoi(args[1]); i++) {
									models.Load(args[0]);					//origin
								}
							}
						}
						//カット
						{

							//新規カット
							if (func.find("SetCut") != std::string::npos) {
								m_CutInfo.resize(m_CutInfo.size() + 1);
								m_CutInfo.back().TIME = (LONGLONG)(1000000.f * std::stof(args[0]));
								//m_CutInfo.back().Cut = GraphHandle::Load("data/cut/" + std::to_string(m_CutInfo.size()) + ".bmp");
								m_CutInfoUpdate.resize(m_CutInfoUpdate.size() + 1);
							}
							//Campos
							else if (func.find("SetCampos_NoneRad") != std::string::npos) {
								m_CutInfo.back().Aim_camera.campos = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
							}
							else if (func.find("SetCampos_Attach") != std::string::npos) {
								attached.Switch.resize(attached.Switch.size() + 1);
								attached.Switch.back().SetSwitch(
									(int)(m_CutInfo.size()) - 1,//-1
									((int)(m_CutInfo.size()) - 1) + (std::stoi(args[0]) - 1)//0
								);
								attachedDetail.resize(attachedDetail.size() + 1);
								attachedDetail.back().isradcam = (args[1].find("TRUE") != std::string::npos);
								if (attachedDetail.back().isradcam) {
									attachedDetail.back().xradcam = std::stof(args[2]);
									attachedDetail.back().yradcam = std::stof(args[3]);
									attachedDetail.back().rangecam = std::stof(args[4]);
								}
								else {
									attachedDetail.back().poscam.Set(std::stof(args[2]), std::stof(args[3]), std::stof(args[4]));
								}
							}
							//Camvec
							else if (func.find("SetCamvec") != std::string::npos) {
								m_CutInfo.back().Aim_camera.camvec = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
							}
							else if (func.find("SetCamForcus") != std::string::npos) {
								m_CutInfo.back().UseForcus = true;
								m_CutInfo.back().ForcusModel = args[0];

								//std::stoi(args[1]);

								if (args[2] == "UPPER_2") {
									m_CutInfo.back().ForcusFrame = "上半身2";
								}
								else if (args[2] == "NECK") {
									m_CutInfo.back().ForcusFrame = "首";
								}
								else if (args[2] == "HEAD") {
									m_CutInfo.back().ForcusFrame = "頭";
								}

								else if (args[2] == "LEFT_EYE") {
									m_CutInfo.back().ForcusFrame = "右目";
								}
								else if (args[2] == "RIGHT_EYE") {
									m_CutInfo.back().ForcusFrame = "左目";
								}
								else if (args[2] == "RIGHT_FINGER") {
									m_CutInfo.back().ForcusFrame = "右人指２";
								}
								else if (args[2] == "FAR_TRUE") {
									m_CutInfo.back().ForcusFrame = "右ひざ";
								}

								m_CutInfo.back().ForcusAdd.Set(std::stof(args[3]), std::stof(args[4]), std::stof(args[5]));
							}
							//CamUp
							else if (func.find("SetCamup") != std::string::npos) {
								m_CutInfo.back().Aim_camera.camup = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
							}
							//Else
							else if (func.find("SetCamInfo") != std::string::npos) {
								m_CutInfo.back().Aim_camera.fov = deg2rad(std::stof(args[0]));
								m_CutInfo.back().Aim_camera.near_ = std::stof(args[1]);
								m_CutInfo.back().Aim_camera.far_ = std::stof(args[2]);
								m_CutInfo.back().cam_per = std::stof(args[3]);
							}
							//物理を次でリセットする
							else if (func.find("SetNextResetPhysics") != std::string::npos) {
								m_CutInfo.back().IsResetPhysics = true;
							}
							//前のカットカメラを使用
							else if (func.find("SetPrevCamInfo") != std::string::npos) {
								m_CutInfo.back().UsePrevAim = true;
							}

							//画像描画
							else if (func.find("SetDrawGraph") != std::string::npos) {
								size_t in_str = args[1].find("~");
								if (in_str != std::string::npos) {
									int start_t = std::stoi(args[1].substr(0, in_str));
									int end_t = std::stoi(args[1].substr(in_str + 1));
									for (int i = start_t; i <= end_t; i++) {
										auto* t = graphs.Get(args[0], i);
										t->Cutinfo.Switch.resize(t->Cutinfo.Switch.size() + 1);
										t->Cutinfo.Switch.back().SetSwitch(
											(int)(m_CutInfo.size()) - 1,//-1
											((int)(m_CutInfo.size()) - 1) + (std::stoi(args[2]) - 1)//0
										);
										t->CutDetail.resize(t->CutDetail.size() + 1);
									}
								}
								else {
									auto* t = graphs.Get(args[0], std::stoi(args[1]));
									t->Cutinfo.Switch.resize(t->Cutinfo.Switch.size() + 1);
									t->Cutinfo.Switch.back().SetSwitch(
										(int)(m_CutInfo.size()) - 1,//-1
										((int)(m_CutInfo.size()) - 1) + (std::stoi(args[2]) - 1)//0
									);
									t->CutDetail.resize(t->CutDetail.size() + 1);
								}
							}
							else if (func.find("SetGraphBlur") != std::string::npos) {
								auto* t = graphs.Get(args[0], std::stoi(args[1]));
								t->CutDetail.back().Blur.Switch.resize(t->CutDetail.back().Blur.Switch.size() + 1);
								t->CutDetail.back().Blur.Switch.back().SetSwitch(
									(int)(m_CutInfo.size()) - 1,//-1
									((int)(m_CutInfo.size()) - 1) + (std::stoi(args[2]) - 1)//0
								);
							}
							else if (func.find("SetGraphOpacityRate") != std::string::npos) {
								auto* t = graphs.Get(args[0], std::stoi(args[1]));
								t->CutDetail.back().OpacityRate = std::stof(args[2]);
							}

							//モデル描画
							else if (func.find("SetDrawModel") != std::string::npos) {
								size_t in_str = args[1].find("~");
								if (in_str != std::string::npos) {
									int start_t = std::stoi(args[1].substr(0, in_str));
									int end_t = std::stoi(args[1].substr(in_str + 1));
									for (int i = start_t; i <= end_t; i++) {
										auto* t = models.Get(args[0], i);
										t->Cutinfo.Switch.resize(t->Cutinfo.Switch.size() + 1);
										t->Cutinfo.Switch.back().SetSwitch(
											(int)(m_CutInfo.size()) - 1,//-1
											((int)(m_CutInfo.size()) - 1) + (std::stoi(args[2]) - 1)//0
										);
										t->CutDetail.resize(t->CutDetail.size() + 1);
									}
								}
								else {
									auto* t = models.Get(args[0], std::stoi(args[1]));
									t->Cutinfo.Switch.resize(t->Cutinfo.Switch.size() + 1);
									t->Cutinfo.Switch.back().SetSwitch(
										(int)(m_CutInfo.size()) - 1,//-1
										((int)(m_CutInfo.size()) - 1) + (std::stoi(args[2]) - 1)//0
									);
									t->CutDetail.resize(t->CutDetail.size() + 1);
								}
							}
							else if (func.find("SetModelAnime") != std::string::npos) {
								auto* t = models.Get(args[0], std::stoi(args[1]));
								t->CutDetail.back().animsel = std::stoi(args[2]);
								t->CutDetail.back().isloop = (args[3].find("TRUE") != std::string::npos);
								t->CutDetail.back().animspeed = std::stof(args[4]);
								t->CutDetail.back().startframe = std::stof(args[5]);
							}
							else if (func.find("SetModelMat") != std::string::npos) {
								auto* t = models.Get(args[0], std::stoi(args[1]));
								t->CutDetail.back().usemat = true;

								t->CutDetail.back().Yrad1_p = std::stof(args[2]);
								t->CutDetail.back().pos_p = VECTOR_ref::vget(std::stof(args[3]), std::stof(args[4]), std::stof(args[5]));
								t->CutDetail.back().Yrad2_p = std::stof(args[6]);

								t->CutDetail.back().mat_p = MATRIX_ref::RotY(deg2rad(t->CutDetail.back().Yrad1_p)) * MATRIX_ref::Mtrans(t->CutDetail.back().pos_p) * MATRIX_ref::RotY(deg2rad(t->CutDetail.back().Yrad2_p));
							}
							else if (func.find("SetModelPhysicsSpeed") != std::string::npos) {
								auto* t = models.Get(args[0], std::stoi(args[1]));
								t->CutDetail.back().PhysicsSpeed_ = std::stof(args[2]);
							}
							else if (func.find("SetModelOpacityRate") != std::string::npos) {
								auto* t = models.Get(args[0], std::stoi(args[1]));
								t->CutDetail.back().OpacityRate = std::stof(args[2]);
							}
							//どの距離で描画するかをセット
							else if (func.find("SetModelMode") != std::string::npos) {
								auto* t = models.Get(args[0], std::stoi(args[1]));
								if (args[2] == ModelType[0]) {
									t->isBGModel = true;
								}
								if (args[2] == ModelType[1]) {
									t->IsNearShadow = false;
								}
								if (args[2] == ModelType[2]) {
									t->IsFarShadow = true;
								}
							}

							//カメラのアップデート
							else if (func.find("SetUpdateEnable") != std::string::npos) {
								m_CutInfoUpdate.back().isUseNotFirst = true;
							}
							//camvec
							else if (func.find("SetUpdateCamvec") != std::string::npos) {
								m_CutInfoUpdate.back().camvec_per = std::stof(args[0]);
								m_CutInfoUpdate.back().CameraNotFirst.camvec.Set(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
							}
							else if (func.find("SetUpdateCamForcus") != std::string::npos) {
								m_CutInfoUpdate.back().UseForcus = true;
								m_CutInfoUpdate.back().camvec_per = std::stof(args[0]);
								m_CutInfoUpdate.back().ForcusModel = args[1];
								if (args[3] == "UPPER_2") {
									m_CutInfoUpdate.back().ForcusFrame = "上半身2";
								}
								else if (args[3] == "NECK") {
									m_CutInfoUpdate.back().ForcusFrame = "首";
								}
								else if (args[3] == "HEAD") {
									m_CutInfoUpdate.back().ForcusFrame = "頭";
								}

								else if (args[3] == "LEFT_EYE") {
									m_CutInfoUpdate.back().ForcusFrame = "右目";
								}
								else if (args[3] == "RIGHT_EYE") {
									m_CutInfoUpdate.back().ForcusFrame = "左目";
								}
								else if (args[3] == "RIGHT_FINGER") {
									m_CutInfoUpdate.back().ForcusFrame = "右人指２";
								}
								else if (args[3] == "FAR_TRUE") {
									m_CutInfoUpdate.back().ForcusFrame = "右ひざ";
								}
								m_CutInfoUpdate.back().ForcusAdd.Set(std::stof(args[4]), std::stof(args[5]), std::stof(args[6]));
							}
							//campos
							else if (func.find("SetUpdateCampos") != std::string::npos) {
								m_CutInfoUpdate.back().campos_per = std::stof(args[0]);
								m_CutInfoUpdate.back().CameraNotFirst.campos.Set(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
							}
							//camup
							else if (func.find("SetUpdateCamup") != std::string::npos) {
								m_CutInfoUpdate.back().camup_per = std::stof(args[0]);
								m_CutInfoUpdate.back().CameraNotFirst.camup.Set(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
							}
							//fov
							else if (func.find("SetUpdateCamfov") != std::string::npos) {
								m_CutInfoUpdate.back().fov_per = std::stof(args[0]);
								m_CutInfoUpdate.back().CameraNotFirst.fov = deg2rad(std::stof(args[1]));
							}
							//easing
							else if (func.find("SetUpdatePer") != std::string::npos) {
								m_CutInfoUpdate.back().NotFirst_per = std::stof(args[0]);
							}
							//前のカメラアップデートを使用
							else if (func.find("SetPrevCamUpdate") != std::string::npos) {
								m_CutInfoUpdate.back().IsUsePrevBuf = true;
							}
							//アップデートカメラにランダムを指定
							else if (func.find("ResetCamPosRand") != std::string::npos) {
								m_CutInfo.back().isResetRandCampos = true;
							}
							else if (func.find("SetCamPosRand") != std::string::npos) {
								m_CutInfoUpdate.back().m_RandcamposSet.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
								m_CutInfoUpdate.back().m_RandcamposPer = std::stof(args[3]);
							}

							else if (func.find("ResetCamVecRand") != std::string::npos) {
								m_CutInfo.back().isResetRandCamvec = true;
							}
							else if (func.find("SetCamVecRand") != std::string::npos) {
								m_CutInfoUpdate.back().m_RandcamvecSet.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
								m_CutInfoUpdate.back().m_RandcamvecPer = std::stof(args[3]);
							}

							else if (func.find("ResetCamUpRand") != std::string::npos) {
								m_CutInfo.back().isResetRandCamup = true;
							}
							else if (func.find("SetCamUpRand") != std::string::npos) {
								m_CutInfoUpdate.back().m_RandcamupSet.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
								m_CutInfoUpdate.back().m_RandcamupPer = std::stof(args[3]);
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
							float tim = float((GetNowHiPerformanceCount() - NowTime) / 1000) / 1000.f;
							if (tim >= 0.001f) {
								GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
							}
							printfDx("ロード%3d完了 [%7.3f s] : %s\n", cnt, tim, func.c_str());
							if (tim >= 0.001f) {
								DrawParts->Screen_Flip();
							}
							NowTime = GetNowHiPerformanceCount();
							cnt++;
						}
						else {
							return;
						}
					}
					FileRead_close(mdata);
					SetUseASyncLoadFlag(FALSE);
					printfDx("非同期読み込みオブジェクトの読み込み待ち…\n");
					int prenum = GetASyncLoadNum(), prenumAll = prenum;
					while (ProcessMessage() == 0 && GetASyncLoadNum() != 0) {
						if (prenum != GetASyncLoadNum()) {
							prenum = GetASyncLoadNum();
							//END
							{
								float tim = float((GetNowHiPerformanceCount() - NowTime) / 1000) / 1000.f;
								if (tim >= 0.001f) {
									GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
								}
								printfDx("ロード%3d完了 [%7.3f s] : %d / %d\n", cnt, tim, prenum, prenumAll);
								if (tim >= 0.001f) {
									DrawParts->Screen_Flip();
								}
								NowTime = GetNowHiPerformanceCount();
								cnt++;
							}
							continue;
						}
					}
					if (ProcessMessage() == 0) {
						GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
						printfDx("ロード全部完了　キーを押してください : total = [%7.3f s]\n", float((GetNowHiPerformanceCount() - TotalTime) / 1000) / 1000.f);
						DrawParts->Screen_Flip();
						WaitKey();
					}
					else {
						return;
					}
				}
				//モデルの事前処理(非同期)
				models.Set();
			}
		public:
			void Set(void) noexcept override {
				auto* PostPassParts = PostPassEffect::Instance();
				TEMPSCENE::Set_EnvLight(VECTOR_ref::vget(500.f, 50.f, 500.f), VECTOR_ref::vget(-500.f, -50.f, -500.f), VECTOR_ref::vget(-0.3f, -0.5f, -0.2f), GetColorF(0.42f, 0.41f, 0.40f, 0.f));
				TEMPSCENE::Set();
				models.Get(SUN, 0)->obj.SetMatrix(MATRIX_ref::RotVec2(VECTOR_ref::up(), (VECTOR_ref)(Get_Light_vec().Norm())) * MATRIX_ref::Mtrans(Get_Light_vec().Norm() * -1500.f));
				Cut = 32;
				Cut = 0;
				models.Start(Cut);
				graphs.Start(Cut);
				attached.Start(Cut);
				SetUseASyncLoadFlag(FALSE);
				BGM = SoundHandle::Load("data/sound.wav");
				///*
				movie = GraphHandle::Load("data/base_movie.mp4");
				PauseMovieToGraph(movie.get());
				//*/
				//プレイ用意
				GameSpeed = (float)(spd_x) / 10.f;

				PostPassParts->Set_Bright(255, 255, 255);
				BaseTime = GetMocroSec() - (Cut > 0 ? m_CutInfo[Cut - 1].TIME : 0);
				WaitTime = (Cut != 0) ? 0 : 1000000;
				NowTimeWait = -WaitTime;

				m_RandcamupBuf.clear();
				m_RandcamvecBuf.clear();
				m_RandcamposBuf.clear();

				LookEditer.Init(false);
				LookMovie.Init(false);
				Start.Init(true);
			}
			bool Update(void) noexcept override {

				auto* PostPassParts = PostPassEffect::Instance();
				//auto* DrawParts = DXDraw::Instance();
				TEMPSCENE::Update();
				auto time = GetMocroSec() - BaseTime;

				MouseClick.GetInput((GetMouseInput_M() & MOUSE_INPUT_LEFT) != 0);
				SpeedUp.GetInput(CheckHitKey(KEY_INPUT_RIGHT) != 0);
				SpeedDown.GetInput(CheckHitKey(KEY_INPUT_LEFT) != 0);
				LookMovie.GetInput(CheckHitKey(KEY_INPUT_M) != 0);
				LookEditer.GetInput(CheckHitKey(KEY_INPUT_N) != 0);
				Start.GetInput((!ModelEditMode && CheckHitKey(KEY_INPUT_SPACE) != 0) || ModelEditIn);
				ModelEditIn = false;
				if (
					(!ModelEditMode && (SpeedUp.trigger() || SpeedDown.trigger())) ||
					Start.trigger()) {
					if (SpeedUp.trigger()) {
						spd_x++;
					}
					if (SpeedDown.trigger()) {
						spd_x--;
					}
					if (Start.trigger()) {
						if (Start.on()) {
							spd_x = 10;
						}
						else {
							spd_x = 0;
						}
					}

					spd_x = std::clamp(spd_x, 0, 20);

					GameSpeed = (float)(spd_x) / 10.f;

					if (GameSpeed >= 0.1f) {
						SetSoundCurrentTime((LONGLONG)(NowTimeWait / 1000), BGM.get());
						SetFrequencySoundMem((int)((float)BGM_Frequency * GameSpeed), BGM.get());
						if (!BGM.check()) {
							BGM.play(DX_PLAYTYPE_BACK, FALSE);
						}
					}
					else {
						BGM.stop();
					}
				}

				if (LookMovie.trigger() && LookMovie.on()) {
					PlayMovieToGraph(movie.get(), 2, DX_MOVIEPLAYTYPE_BCANCEL);
					ChangeMovieVolumeToGraph(0, movie.get());
					SeekMovieToGraph(movie.get(), (int)(NowTimeWait / 1000));
				}
				NowTimeWait += (time);
				BaseTime = GetMocroSec();
				//待ち
				if (NowTimeWait < 0) {
					isfast = true;
					return true;
				}
				else {
					if (isfast) {
						isfast = false;
						BGM_Frequency = GetFrequencySoundMem(BGM.get());
						BGM.play(DX_PLAYTYPE_BACK, TRUE);
						//BGM.vol(64);
						SetSoundCurrentTime((LONGLONG)(NowTimeWait / 1000), BGM.get());

						SetFrequencySoundMem((int)((float)BGM_Frequency * GameSpeed), BGM.get());
					}
				}
				//カットの処理
				if (!Time_Over()) {
					{
						if (attached.Update_((int)Cut)) {
							attached.Update_((int)Cut);
						}
						models.FirstUpdate((int)Cut, isFirstLoop);
						graphs.FirstUpdate((int)Cut, isFirstLoop);
						issetcampos = attached.GetSwitch();
						isradcam = (attachedDetail.size() > attached.nowcut) ? attachedDetail[attached.nowcut].isradcam : false;
						if (isFirstLoop && attached.isFirstCut) {
							if (isradcam) {
								xradcam = attachedDetail[attached.nowcut].xradcam;
								yradcam = attachedDetail[attached.nowcut].yradcam;
								rangecam = attachedDetail[attached.nowcut].rangecam;
							}
							else {
								poscam = attachedDetail[attached.nowcut].poscam;
							}
						}
					}
					int SEL = 0;
					{
						//0
						if (Cut == SEL) {
							if (!isFirstLoop) {
								m_CutInfoUpdate[Cut].CameraNotFirst.camvec.xadd(2.f / FPS * GameSpeed);

								m_CutInfoUpdate[Cut].CameraNotFirst.campos.xadd(2.f / FPS * GameSpeed);
							}
						}
						SEL += 1;
						//1
						if (Cut == SEL) {
							if (!isFirstLoop) {
								m_CutInfoUpdate[Cut].CameraNotFirst.camvec.zadd(-10.f / FPS * GameSpeed);

								m_CutInfoUpdate[Cut].CameraNotFirst.campos.zadd(-10.f / FPS * GameSpeed);
							}
						}
						SEL += 3;
						//4
						if (Cut == SEL) {
							if (!isFirstLoop) {
								m_CutInfoUpdate[Cut].CameraNotFirst.camvec.zadd(-30.f / FPS * GameSpeed);

								m_CutInfoUpdate[Cut].CameraNotFirst.campos.zadd(-30.f / FPS * GameSpeed);
							}
						}
						SEL += 5;
						//9
						if (Cut == SEL) {
							if (!isFirstLoop) {
								m_CutInfoUpdate[Cut].CameraNotFirst.campos.xadd(-15.f / FPS * GameSpeed);
							}
						}
						SEL += 6;
						//15
						if (Cut == SEL) {
							easing_set_SetSpeed(&Box_ALPHA, 1.f, 0.95f);
							easing_set_SetSpeed(&models.Get(LOGO, 0)->OpacityRate, 0.f, 0.95f);
							graphs.Get(LOGO1, 0)->Alpha.Set(1.f, 0.95f);
						}
						SEL++;
						//16
						if (Cut == SEL) {
							if (isFirstLoop) {
								Box_ALPHA = 0.f;
								PostPassParts->Set_Bright(255, 216, 192);
							}
							else {
								m_CutInfoUpdate[Cut].CameraNotFirst.campos.yadd(1.f / FPS * GameSpeed);
							}
						}
						SEL++;
						//17
						if (Cut == SEL) {
							if (isFirstLoop) {
								PostPassParts->Set_Bright(255, 255, 255);
								camxb_15 = -3.f;
							}
							m_CutInfoUpdate[Cut].campos_per = 0.9f;
							m_CutInfoUpdate[Cut].CameraNotFirst.campos = m_CutInfoUpdate[Cut].CameraNotFirst.camvec + VECTOR_ref::vget(camxb_15, 0.f, -60.f);
							camxb_15 += 1.f / FPS * GameSpeed;
						}
						SEL++;
						//18
						if (Cut == SEL) {
							if (isFirstLoop) {
								m_CutInfoUpdate[Cut].campos_per = 0.f;
								m_CutInfoUpdate[Cut].CameraNotFirst.campos = models.Get(Tanhoiza, 0)->GetFrame("首") + VECTOR_ref::vget(30.f, 0.f, -10.f);

								m_CutInfoUpdate[Cut].camvec_per = 0.f;
								m_CutInfoUpdate[Cut].ForcusModel = Tanhoiza;
								m_CutInfoUpdate[Cut].ForcusFrame = "首";
								m_CutInfoUpdate[Cut].ForcusAdd.Set(0.f, 0.f, 0.f);
								m_CutInfoUpdate[Cut].CameraNotFirst.camvec = models.Get(m_CutInfoUpdate[Cut].ForcusModel, 0)->GetFrame(m_CutInfoUpdate[Cut].ForcusFrame) + m_CutInfoUpdate[Cut].ForcusAdd;
							}
							else {
								m_CutInfoUpdate[Cut].CameraNotFirst.camvec.zadd(50.f / FPS * GameSpeed);
								m_CutInfoUpdate[Cut].CameraNotFirst.campos.zadd(50.f / FPS * GameSpeed);
							}
						}
						SEL += 2;
						//19
						if (Cut == SEL) {
							if (isFirstLoop) {
								camupxb_18 = 0.f;
							}
							m_CutInfoUpdate[Cut].camup_per = 0.95f;
							auto nowupxb = m_CutInfo[Cut].Aim_camera.camvec.x();
							m_CutInfoUpdate[Cut].CameraNotFirst.camup = VECTOR_ref::vget(std::clamp((camupxb_18 - nowupxb)*1.5f, -0.5f, 0.5f), 1.f, 0);
							camupxb_18 = nowupxb;
						}
						SEL += 3;
						//22
						if (Cut == SEL) {
							if (!isFirstLoop) {
								m_CutInfoUpdate[Cut].CameraNotFirst.camvec.xadd(1.f / FPS * GameSpeed);
							}
						}
						SEL++;
						//23
						if (Cut == SEL) {
							if (!isFirstLoop) {
								m_CutInfoUpdate[Cut].CameraNotFirst.camvec.zadd(-1.f / FPS * GameSpeed);
							}
						}
						SEL++;
						//24
						if (Cut == SEL) {
							if (!isFirstLoop) {
								m_CutInfoUpdate[Cut].CameraNotFirst.camvec.xadd(-1.f / FPS * GameSpeed);
							}
						}
						SEL++;
						//25
						if (Cut == SEL) {
							if (!isFirstLoop) {
								m_CutInfoUpdate[Cut].CameraNotFirst.camvec.zadd(1.f / FPS * GameSpeed);
							}
						}
						SEL++;
						//26
						if (Cut == SEL) {
							if (!isFirstLoop) {
								m_CutInfoUpdate[Cut].CameraNotFirst.camvec.xadd(1.f / FPS * GameSpeed);
							}
						}
						SEL += 4;
						//30
						if (Cut == SEL) {
							auto BUF = (models.Get(spe, 0)->GetFrame("左目") + models.Get(szk, 0)->GetFrame("右目")) / 2.f;
							if (isFirstLoop) {
								m_CutInfo[Cut].Aim_camera.camvec = BUF;

								m_CutInfoUpdate[Cut].campos_per = 0.f;
								m_CutInfoUpdate[Cut].CameraNotFirst.campos = BUF + VECTOR_ref::vget(0.f, -15.f, -20.f);
							}
							else {
								m_CutInfoUpdate[Cut].CameraNotFirst.campos.zadd(100.f / FPS * GameSpeed);
							}
							m_CutInfoUpdate[Cut].camvec_per = 0.9f;
							m_CutInfoUpdate[Cut].CameraNotFirst.camvec = BUF;
						}
						SEL += 7;
						//37
						if (Cut == SEL) {
							if (NowTimeWait > (LONGLONG)(1000000.f * 64.4f)) {
								easing_set_SetSpeed(&Box_ALPHA2, 1.f, 0.975f);
							}
						}
						SEL++;
						//38
						if (Cut == SEL) {
							if (isFirstLoop) {
								models.Get(Rudolf2, 0)->OpacityRate = 1.f;
							}
							else {
								easing_set_SetSpeed(&Box_ALPHA2, 0.f, 0.95f);
								m_CutInfoUpdate[Cut].CameraNotFirst.campos.xadd(2.f / FPS * GameSpeed);
							}
						}
						SEL++;
						//39
						if (Cut == SEL) {
							if (!isFirstLoop) {
								m_CutInfoUpdate[Cut].CameraNotFirst.campos.xadd(2.f / FPS * GameSpeed);
							}
						}
						SEL += 3;
						//42
						if (Cut == SEL) {
							if (!isFirstLoop) {
								easing_set_SetSpeed(&models.Get(GATE, 0)->OpacityRate, 1.f, 0.9f);
							}
						}
						SEL++;
						//43
						if (Cut == SEL) {
							if (!isFirstLoop) {
								m_CutInfoUpdate[Cut].CameraNotFirst.campos.xadd(-20.f / FPS * GameSpeed);
							}
						}
						SEL += 3;
						//46
						if (Cut == SEL) {
							if (!isFirstLoop) {
								easing_set_SetSpeed(&Box_ALPHA2, 1.f, 0.95f);
							}
						}
						SEL++;
					}
					if (isFirstLoop) {
						if (m_CutInfo[Cut].UsePrevAim) {
							m_CutInfo[Cut].Aim_camera = m_CutInfo[Cut - 1].Aim_camera;
							m_CutInfo[Cut].cam_per = m_CutInfo[Cut - 1].cam_per;
						}
						if (m_CutInfoUpdate[Cut].IsUsePrevBuf) {
							m_CutInfoUpdate[Cut] = m_CutInfoUpdate[Cut - 1];
						}
						//
						if (m_CutInfo[Cut].UseForcus) {
							m_CutInfo[Cut].Aim_camera.camvec = models.Get(m_CutInfo[Cut].ForcusModel, 0)->GetFrame(m_CutInfo[Cut].ForcusFrame) + m_CutInfo[Cut].ForcusAdd;
						}
						//
						if (issetcampos) {
							if (isradcam) {
								m_CutInfo[Cut].Aim_camera.campos = m_CutInfo[Cut].Aim_camera.camvec + GetVector(xradcam, yradcam)*rangecam;
							}
							else {
								m_CutInfo[Cut].Aim_camera.campos = m_CutInfo[Cut].Aim_camera.camvec + poscam;
							}
						}
						if (m_CutInfo[Cut].isResetRandCampos) { m_RandcamposBuf.clear(); }
						if (m_CutInfo[Cut].isResetRandCamvec) { m_RandcamvecBuf.clear(); }
						if (m_CutInfo[Cut].isResetRandCamup) { m_RandcamupBuf.clear(); }
					}
					else {
						if (m_CutInfoUpdate[Cut].NotFirst_per >= 0.f) {
							m_CutInfo[Cut].cam_per = m_CutInfoUpdate[Cut].NotFirst_per;
						}
						easing_set_SetSpeed(&m_RandcamposBuf, VECTOR_ref::vget(GetRandf(m_CutInfoUpdate[Cut].m_RandcamposSet.x()), GetRandf(m_CutInfoUpdate[Cut].m_RandcamposSet.y()), GetRandf(m_CutInfoUpdate[Cut].m_RandcamposSet.z())), m_CutInfoUpdate[Cut].m_RandcamposPer);
						easing_set_SetSpeed(&m_RandcamvecBuf, VECTOR_ref::vget(GetRandf(m_CutInfoUpdate[Cut].m_RandcamvecSet.x()), GetRandf(m_CutInfoUpdate[Cut].m_RandcamvecSet.y()), GetRandf(m_CutInfoUpdate[Cut].m_RandcamvecSet.z())), m_CutInfoUpdate[Cut].m_RandcamvecPer);
						if (m_CutInfoUpdate[Cut].isUseNotFirst) {
							if (m_CutInfoUpdate[Cut].UseForcus) {
								m_CutInfoUpdate[Cut].CameraNotFirst.camvec = models.Get(m_CutInfoUpdate[Cut].ForcusModel, 0)->GetFrame(m_CutInfoUpdate[Cut].ForcusFrame) + m_CutInfoUpdate[Cut].ForcusAdd;
							}
							easing_set_SetSpeed(&m_CutInfo[Cut].Aim_camera.camvec, m_CutInfoUpdate[Cut].CameraNotFirst.camvec + m_RandcamvecBuf, m_CutInfoUpdate[Cut].camvec_per);
							easing_set_SetSpeed(&m_CutInfo[Cut].Aim_camera.campos, m_CutInfoUpdate[Cut].CameraNotFirst.campos + m_RandcamposBuf, m_CutInfoUpdate[Cut].campos_per);
							easing_set_SetSpeed(&m_CutInfo[Cut].Aim_camera.camup, m_CutInfoUpdate[Cut].CameraNotFirst.camup, m_CutInfoUpdate[Cut].camup_per);
							easing_set_SetSpeed(&m_CutInfo[Cut].Aim_camera.fov, m_CutInfoUpdate[Cut].CameraNotFirst.fov, m_CutInfoUpdate[Cut].fov_per);
						}
					}
					//
					easing_set_SetSpeed(&camera_buf.campos, m_CutInfo[Cut].Aim_camera.campos, m_CutInfo[Cut].cam_per);
					easing_set_SetSpeed(&camera_buf.camvec, m_CutInfo[Cut].Aim_camera.camvec, m_CutInfo[Cut].cam_per);
					easing_set_SetSpeed(&camera_buf.camup, m_CutInfo[Cut].Aim_camera.camup, m_CutInfo[Cut].cam_per);
					easing_set_SetSpeed(&camera_buf.fov, m_CutInfo[Cut].Aim_camera.fov, m_CutInfo[Cut].cam_per);
					easing_set_SetSpeed(&camera_buf.far_, m_CutInfo[Cut].Aim_camera.far_, m_CutInfo[Cut].cam_per);
					easing_set_SetSpeed(&camera_buf.near_, m_CutInfo[Cut].Aim_camera.near_, m_CutInfo[Cut].cam_per);
					camera_buf.set_cam_info(camera_buf.fov, camera_buf.near_, camera_buf.far_);
					//
					isFreepos = (CheckHitKey(KEY_INPUT_RETURN) != 0);
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
				graphs.Update();
				models.Update();
				if (!Time_Over()) {
					models.SetPhysics(ResetPhysics);
					isFirstLoop = false;
					if (NowTimeWait > m_CutInfo[Cut%m_CutInfo.size()].TIME) {
						bool IsResetPhysics = m_CutInfo[Cut].IsResetPhysics;
						++Cut;
						isFirstLoop = true;
						if (IsResetPhysics) {
							ResetPhysics = true;
						}
					}
					else {
						ResetPhysics = false;
					}
				}
				Effect_UseControl::Update_Effect();
				if (Time_Over()) { return false; }
				//if (!BGM.check()) { return false; }
				return true;
			}
			void Dispose(void) noexcept override {
				Effect_UseControl::Dispose_Effect();
				m_CutInfo.clear();
				m_CutInfoUpdate.clear();
				movie.Dispose();
				BGM.Dispose();
			}
			//
			void UI_Draw(void) noexcept  override {
				if (!isFreepos && NowTimeWait > 0) {
					for (auto& t : Texts) {
						t.Draw(NowTimeWait);
					}
				}
			}
			//他 32
			void BG_Draw(void) noexcept override {
				//+3
				models.Draw_Far();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				//+12
				models.Draw(false, true);
			}
			void Shadow_Draw(void) noexcept override {
				//+52
				models.Draw(true, false);
			}
			void Main_Draw(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				if (!Time_Over()) {
					{
						SetFogEnable(TRUE);
						SetFogDensity(0.01f);
						SetFogColor(128, 128, 128);
						SetFogStartEnd(200, 300000);
					}
					//*
					if (Cut == 28) {
						SetDrawBright(192, 192, 192);
						DrawBillboard3D(VECTOR_ref::vget(0, 15.f, 20.f).get(), 0.5f, 0.5f, 15.f, 0.f, m_BB.get(), TRUE);
						SetDrawBright(255, 255, 255);
						{
							SetFogColor(0, 0, 0);
							SetFogStartEnd(10, 150);
						}
					}
					if (Cut == 36 || Cut == 37) {
						SetFogColor(224, 224, 224);
						SetFogStartEnd(250, 900);
					}
					if (Cut == 38) {
						SetFogColor(128, 128, 128);
						SetFogStartEnd(200, 3000);
					}
					if (Cut >= 45) {
						SetFogColor(255, 255, 255);
						SetFogStartEnd(50, 300);
					}
					//*/
				}
				//+201 = 67x3
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
					if (Box_ALPHA2 != 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*Box_ALPHA2));
						DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_y, GetColor(255, 255, 255), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					graphs.Draw(DrawParts->disp_y);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			//
			void LAST_Draw(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();

				int mouse_x, mouse_y;
				GetMousePoint(&mouse_x, &mouse_y);

				if (LookMovie.on()) {
					auto* DrawParts = DXDraw::Instance();
					movie.DrawExtendGraph(DrawParts->disp_x * 3 / 4, DrawParts->disp_y * 3 / 4, DrawParts->disp_x, DrawParts->disp_y, FALSE);
				}
				if (LookEditer.on()) {
					//編集画面
					{
						int x_p = DrawParts->disp_x * 1 / 10;
						int x_s = DrawParts->disp_x * 8 / 10;
						int y_p = DrawParts->disp_y * 5 / 10;
						int y_s = DrawParts->disp_y * 4 / 10;

						int BaseWidth = DrawParts->disp_x / 64;
						int hight = y_s / (int)(models.GetMax());
						int x_now = -1;
						LONGLONG now2 = (Cut >= 1) ? m_CutInfo[Cut - 1].TIME : 0;
						int width_Time = BaseWidth * (int)(NowTimeWait - now2) / 1000000;
						//オフセット計算
						{
							//現在地
							int position = 5;// x_s / 4;
							{
								int x1 = x_p;
								int i = 0;
								while (true) {
									LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].TIME : 0;
									LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].TIME : 0;
									x1 += BaseWidth * (int)(now - base) / 1000000;
									//NOW
									if (i == Cut) {
										x_now = x1;
										break;
									}
									i++;
									if (i >= m_CutInfo.size()) { break; }
								}
							}
							//SetNOW
							if (x_now >= 0) {
								X_now = std::min(-(x_now + width_Time - (x_p + position)), 0);
							}
							//CutNow計算
							{
								int x1 = x_p + X_now;
								int i = 0;
								while (true) {
									LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].TIME : 0;
									LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].TIME : 0;

									x1 += BaseWidth * (int)(now - base) / 1000000;
									if (x1 > x_p) {
										CutNow = std::max(i - 1, 0);
										break;
									}
									i++;
									if (i - 1 >= m_CutInfo.size()) { break; }
								}
							}
							//現在地再計算
							{
								int x1 = x_p;
								int i = CutNow;
								while (true) {
									LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].TIME : 0;
									LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].TIME : 0;
									x1 += BaseWidth * (int)(now - base) / 1000000;
									//NOW
									if (i == Cut) {
										x_now = x1;
										break;
									}
									i++;
									if (i >= m_CutInfo.size()) { break; }
								}
							}
							//SetNOW再計算
							if (x_now >= 0) {
								X_now = std::min(-(x_now + width_Time - (x_p + position)), 0);
							}
						}
						//判定演算
						if (!ModelEditMode) {
							if (MouseClick.press()) {
								int x1 = x_p + X_now;
								int i = CutNow;
								while (true) {
									LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].TIME : 0;
									LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].TIME : 0;

									x1 += BaseWidth * (int)(now - base) / 1000000;
									if (x1 > x_p + x_s) { break; }
									if (x1 > x_p) {
										int width_Next = BaseWidth * (int)(m_CutInfo[i].TIME - now) / 1000000;
										int msel = (mouse_y - y_p) / hight;
										if (msel >= 0) {
											int y1 = y_p + msel * hight;
											if (in2_(mouse_x, mouse_y, x1, y1, x1 + width_Next, y1 + hight)) {
												auto* tmp = models.Get(models.GetModel()[msel].Path, models.GetModel()[msel].BaseID);
												bool EditModelInfo = false;
												for (auto& c : tmp->Cutinfo.Switch) {
													if (c.On <= i && i <= c.Off) {
														EditModelInfo = true;
														ModelEdit = tmp;
														ModelEditCutNum = &c - &tmp->Cutinfo.Switch.front();
														break;
													}
												}
												if (!EditModelInfo || ModelEdit == nullptr) {
													if (ChangeModel == nullptr) { ChangeModel = tmp; }
													if (SetAnimStart == -1) { SetAnimStart = i; }
													SetAnimEnd = std::max(i, SetAnimStart);
												}
											}
										}
									}
									i++;
									if (i >= m_CutInfo.size()) { break; }
								}
							}
							else {
								if (ChangeModel != nullptr) {
									//被りの修正
									for (const auto& c : ChangeModel->Cutinfo.Switch) {
										if (SetAnimStart >= c.Off) {
											SetAnimStart = std::max(SetAnimStart, c.Off + 1);
										}
										if (SetAnimStart < c.On) {
											SetAnimEnd = std::min(SetAnimEnd, c.On - 1);
										}
									}
									//被りの確認
									for (const auto& c : ChangeModel->Cutinfo.Switch) {
										if (SetAnimStart == c.On || SetAnimEnd == c.Off) {
											ChangeModel = nullptr;
											break;
										}
									}
									//登録できるなら作成
									if (ChangeModel != nullptr) {
										int id = 0;
										for (const auto& c : ChangeModel->Cutinfo.Switch) {
											if (SetAnimStart == c.On || SetAnimEnd == c.Off) {
												ChangeModel = nullptr;
												break;
											}
											if (SetAnimStart < c.On) {
												break;
											}
											id++;
										}
										ChangeModel->Cutinfo.Insert(id, SetAnimStart, SetAnimEnd);
										ChangeModel->CutDetail.insert(ChangeModel->CutDetail.begin() + id, Cutinfo_g());
										auto& info = ChangeModel->CutDetail[id];
										info.animsel = 0;
										info.isloop = true;
										info.animspeed = 1.0f;
										info.startframe = 0.f;

										ChangeModel = nullptr;
									}
									SetAnimStart = -1;
									SetAnimEnd = -1;
								}
								else if (ModelEdit != nullptr && !ModelEditMode) {
									ModelEditMode = true;
									if (Start.on()) {
										ModelEditIn = true;
									}
								}
							}
						}
						//BACK
						{
							//BG
							{
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
								DrawBox(x_p, y_p, x_p + x_s, y_p + y_s, GetColor(0, 0, 0), TRUE);
								SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
							}
							//line
							{
								for (int i = 0; i < models.GetMax() + 1; i++) {
									int y1 = y_p + i * hight;
									DrawLine(x_p, y1, x_p + x_s, y1, GetColor(128, 128, 128), 3);
								}
								int x1 = x_p + X_now;
								int i = CutNow;
								while (true) {
									LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].TIME : 0;
									LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].TIME : 0;

									x1 += BaseWidth * (int)(now - base) / 1000000;
									if (x1 > x_p + x_s) { break; }
									//LINE
									if (x1 > x_p) {
										DrawLine(x1, y_p, x1, y_p + y_s, GetColor(128, 128, 128), 3);
										int width_Next = BaseWidth * (int)(m_CutInfo[i].TIME - now) / 1000000;

										if (!ModelEditMode && in2_(mouse_x, mouse_y, x1, y_p, x1 + width_Next, y_p + y_s)) {
											int y1 = y_p + ((mouse_y - y_p) / hight) * hight;
											SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
											DrawBox(x1, y1, x1 + width_Next, y1 + hight, GetColor(255, 255, 255), TRUE);
											SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
										}
									}
									i++;
									if (i >= m_CutInfo.size()) { break; }
								}
							}
						}
						//timeline
						{
							int x1 = x_p + X_now;
							int i = CutNow;
							while (true) {
								LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].TIME : 0;
								LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].TIME : 0;
								int width_Next = BaseWidth * (int)(m_CutInfo[i].TIME - now) / 1000000;

								x1 += BaseWidth * (int)(now - base) / 1000000;
								if (x1 > x_p + x_s) { break; }
								//
								int xp = 3, yp = 3;
								int p2 = 2;
								int y1 = y_p;
								for (const auto& m : models.GetModel()) {
									if (ChangeModel != nullptr && ChangeModel == (ModelControl::Model*)&m) {
										if (SetAnimStart <= i && i <= SetAnimEnd) {
											DrawBox(x1, y1 + yp, x1 + width_Next, y1 + hight - yp, GetColor(255, 255, 0), FALSE);
										}
									}
									for (const auto& c : m.Cutinfo.Switch) {
										if (c.On <= i && i <= c.Off) {
											int x_b2 = std::max(x1 + ((i == c.On) ? p2 : 0), x_p);
											int x_b1 = std::max(x1 + ((i == c.On) ? xp : 0), x_p);
											int x_b4 = std::min(x1 + width_Next - ((i == c.Off) ? p2 : 0), x_p + x_s);
											int x_b3 = std::min(x1 + width_Next - ((i == c.Off) ? xp : 0), x_p + x_s);

											unsigned int color;
											if (m.isBGModel) {
												if (c.On <= Cut && Cut <= c.Off) {
													color = GetColor(100, 216, 100);
												}
												else {
													color = GetColor(60, 60, 192);
												}
											}
											else {
												if (c.On <= Cut && Cut <= c.Off) {
													color = GetColor(150, 255, 150);
												}
												else {
													color = GetColor(100, 100, 255);
												}
											}
											if (ModelEdit != nullptr && ModelEditMode) {
												if (ModelEdit == (ModelControl::Model*)&m && ModelEditCutNum == &c - &m.Cutinfo.Switch.front()) {
													color = GetColor(255, 255, 0);
												}
											}

											DrawBox(x_b2, y1 + yp - p2, x_b4, y1 + hight - yp + p2, GetColor(0, 0, 0), TRUE);
											DrawBox(x_b1, y1 + yp, x_b3, y1 + hight - yp, color, TRUE);
										}
									}
									y1 += hight;
								}
								i++;
								if (i >= m_CutInfo.size()) { break; }
							}
						}
						//modelName
						{
							int y1 = y_p;
							for (const auto& m : models.GetModel()) {
								const auto* sel = LSClass.GetArgFromPath(m.Path);
								if (sel != nullptr) {
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
									for (const auto& c : m.Cutinfo.Switch) {
										if (c.On <= Cut && Cut <= c.Off) {
											SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
										}
									}
									Fonts.Get(hight).Get_handle().DrawString(x_p, y1, " " + sel->Base + "(" + std::to_string(m.BaseID) + ")", GetColor(255, 255, 255));
									SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
								}
								y1 += hight;
							}
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						}
						//NOWline
						if (x_now >= 0) {
							DrawLine(x_now + X_now + width_Time, y_p, x_now + X_now + width_Time, y_p + y_s, GetColor(255, 255, 255), 3);
						}
						//OverRay
						if (ModelEditMode) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
							DrawBox(x_p, y_p, x_p + x_s, y_p + y_s, GetColor(0, 0, 0), TRUE);
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						}

						//モデルエディットモード判定(参照変更)
						if (ModelEdit != nullptr && ModelEditMode) {
							if (MouseClick.press()) {
								int x1 = x_p + X_now;
								int i = CutNow;
								while (true) {
									LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].TIME : 0;
									LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].TIME : 0;

									x1 += BaseWidth * (int)(now - base) / 1000000;
									if (x1 > x_p + x_s) { break; }
									if (x1 > x_p) {
										int width_Next = BaseWidth * (int)(m_CutInfo[i].TIME - now) / 1000000;
										int msel = (mouse_y - y_p) / hight;
										if (msel >= 0) {
											int y1 = y_p + msel * hight;
											if (in2_(mouse_x, mouse_y, x1, y1, x1 + width_Next, y1 + hight)) {
												auto* tmp = models.Get(models.GetModel()[msel].Path, models.GetModel()[msel].BaseID);
												for (auto& c : tmp->Cutinfo.Switch) {
													if (c.On <= i && i <= c.Off) {
														ModelEdit = tmp;
														ModelEditCutNum = &c - &tmp->Cutinfo.Switch.front();
														break;
													}
												}
											}
										}
									}
									i++;
									if (i >= m_CutInfo.size()) { break; }
								}
							}
						}
						//
					}
					//モデルエディットモード
					if (ModelEditMode) {
						int x_p = DrawParts->disp_x * 5 / 10;
						int x_s = DrawParts->disp_x * 4 / 10;
						int y_p = DrawParts->disp_y * 2 / 10;
						int y_s = DrawParts->disp_y * 5 / 20;
						//モデルエディットモード判定
						if (ModelEdit != nullptr && ModelEditMode) {
							bool isend = false;
							//EndMode
							{
								int x1 = x_p + x_s - 32 - 6;
								int y1 = y_p + y_s - 32 - 6;
								int x2 = x1 + 32;
								int y2 = y1 + 32;
								if (MouseClick.trigger()) {
									if (in2_(mouse_x, mouse_y, x1, y1, x2, y2)) {
										isend = true;
									}
								}
							}

							//適応
							if (isend) {
								//ModelEdit->CutDetail[ModelEditCutNum].animsel = 2;
								ModelEdit = nullptr;
								ModelEditCutNum = 0;
								ModelEditMode = false;
								/*
								if (!Start.on()) {
									ModelEditIn = true;
								}
								//*/
							}
						}
						//BACK
						{
							//BG
							{
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
								DrawBox(x_p, y_p, x_p + x_s, y_p + y_s, GetColor(0, 0, 0), TRUE);
								SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
							}
						}
						//EndMode
						{
							int x1 = x_p + x_s - 32 - 6;
							int y1 = y_p + y_s - 32 - 6;
							int x2 = x1 + 32;
							int y2 = y1 + 32;
							unsigned int color;
							if (in2_(mouse_x, mouse_y, x1, y1, x2, y2)) {
								color = GetColor(200, 0, 0);
							}
							else {
								color = GetColor(255, 0, 0);
							}

							DrawBox(x1, y1, x2, y2, color, TRUE);
						}
						//info
						{
							int hight = y_r(20);
							int y1 = y_p;
							int p2 = 2;
							if (ModelEdit != nullptr) {
								auto& c = ModelEdit->CutDetail[ModelEditCutNum];
								//Name
								{
									const auto* sel = LSClass.GetArgFromPath(ModelEdit->Path);
									if (sel != nullptr) {
										Fonts.Get(hight).Get_handle().DrawStringFormat(x_p + x_s / 2, y1, GetColor(255, 255, 255), sel->Base + "(" + std::to_string(ModelEdit->BaseID) + ")");
									}
									Fonts.Get(hight).Get_handle().DrawString(x_p, y1, "Name", GetColor(255, 255, 255));
									y1 += hight + p2;
								}
								//info
								{
									//AnimeSel
									{
										if (in2_(mouse_x, mouse_y, x_p, y1, x_p + x_s, y1 + hight)) {
											SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
											DrawBox(x_p + p2, y1 + p2, x_p + x_s - p2, y1 + hight - p2, GetColor(255, 255, 255), TRUE);
											SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
										}
										Fonts.Get(hight).Get_handle().DrawStringFormat(x_p + x_s / 2, y1, GetColor(255, 255, 255), std::to_string(c.animsel));

										Fonts.Get(hight).Get_handle().DrawString(x_p, y1, "AnimeSel", GetColor(255, 255, 255));
										y1 += hight + p2;
									}
									//DrawMode
									{
										if (in2_(mouse_x, mouse_y, x_p, y1, x_p + x_s, y1 + hight)) {
											SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
											DrawBox(x_p + p2, y1 + p2, x_p + x_s - p2, y1 + hight - p2, GetColor(255, 255, 255), TRUE);
											SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
										}
										std::string mode = "NORMAL";
										if (ModelEdit->isBGModel) {
											mode = ModelType[0];
										}
										else if (!ModelEdit->IsNearShadow) {
											mode = ModelType[1];
										}
										else if (ModelEdit->IsFarShadow) {
											mode = ModelType[2];
										}
										Fonts.Get(hight).Get_handle().DrawStringFormat(x_p + x_s / 2, y1, GetColor(255, 255, 255), mode);

										Fonts.Get(hight).Get_handle().DrawString(x_p, y1, "DrawMode", GetColor(255, 255, 255));
										y1 += hight + p2;
									}
									//ModelPhysicsSpeed
									{
										if (in2_(mouse_x, mouse_y, x_p, y1, x_p + x_s, y1 + hight)) {
											SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
											DrawBox(x_p + p2, y1 + p2, x_p + x_s - p2, y1 + hight - p2, GetColor(255, 255, 255), TRUE);
											SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
										}
										if (c.PhysicsSpeed_ >= 0.f) {
											Fonts.Get(hight).Get_handle().DrawStringFormat(x_p + x_s / 2, y1, GetColor(255, 255, 255), "%4.2f", c.PhysicsSpeed_);
										}
										else {
											Fonts.Get(hight).Get_handle().DrawStringFormat(x_p + x_s / 2, y1, GetColor(255, 255, 255), "continuous");
										}
										Fonts.Get(hight).Get_handle().DrawString(x_p, y1, "ModelPhysicsSpeed", GetColor(255, 255, 255));
										y1 += hight + p2;
									}
									//OpacityRate
									{
										if (in2_(mouse_x, mouse_y, x_p, y1, x_p + x_s, y1 + hight)) {
											SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
											DrawBox(x_p + p2, y1 + p2, x_p + x_s - p2, y1 + hight - p2, GetColor(255, 255, 255), TRUE);
											SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
										}
										if (c.OpacityRate >= 0.f) {
											Fonts.Get(hight).Get_handle().DrawStringFormat(x_p + x_s / 2, y1, GetColor(255, 255, 255), "%4.2f", c.OpacityRate);
										}
										else {
											Fonts.Get(hight).Get_handle().DrawStringFormat(x_p + x_s / 2, y1, GetColor(255, 255, 255), "continuous");
										}
										Fonts.Get(hight).Get_handle().DrawString(x_p, y1, "OpacityRate", GetColor(255, 255, 255));
										y1 += hight + p2;
									}
									//Matrix
									{
										if (in2_(mouse_x, mouse_y, x_p, y1, x_p + x_s, y1 + hight)) {
											SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
											DrawBox(x_p + p2, y1 + p2, x_p + x_s - p2, y1 + hight - p2, GetColor(255, 255, 255), TRUE);
											SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
										}
										if (c.usemat) {
											//c.mat_p = MATRIX_ref::RotY(deg2rad(c.Yrad1_p)) * MATRIX_ref::Mtrans(c.pos_p) * MATRIX_ref::RotY(deg2rad(c.Yrad2_p));
											std::string str = "%5.2f°(%5.2f,%5.2f,%5.2f) %5.2f°";
											auto length = Fonts.Get(hight).Get_handle().GetDrawWidthFormat(str, c.Yrad1_p, c.pos_p.x(), c.pos_p.y(), c.pos_p.z(), c.Yrad2_p);
											if (length > x_s / 2) {
												Fonts.Get(hight).Get_handle().DrawStringFormat_RIGHT(x_p + x_s, y1, GetColor(255, 255, 255), str, c.Yrad1_p, c.pos_p.x(), c.pos_p.y(), c.pos_p.z(), c.Yrad2_p);
											}
											else {
												Fonts.Get(hight).Get_handle().DrawStringFormat(x_p + x_s / 2, y1, GetColor(255, 255, 255), str, c.Yrad1_p, c.pos_p.x(), c.pos_p.y(), c.pos_p.z(), c.Yrad2_p);
											}
										}
										else {
											Fonts.Get(hight).Get_handle().DrawStringFormat(x_p + x_s / 2, y1, GetColor(255, 255, 255), "continuous");
										}
										Fonts.Get(hight).Get_handle().DrawString(x_p, y1, "OpacityRate", GetColor(255, 255, 255));
										y1 += hight + p2;
									}
									//End
									/*
										else if (func.find("SetModelMat") != std::string::npos) {
											auto* t = models.Get(args[0], std::stoi(args[1]));
											t->CutDetail.back().usemat = true;
											t->CutDetail.back().mat_p =
												MATRIX_ref::RotY(deg2rad(std::stof(args[2])))*
												MATRIX_ref::Mtrans(VECTOR_ref::vget(std::stof(args[3]), std::stof(args[4]), std::stof(args[5])))*
												MATRIX_ref::RotY(deg2rad(std::stof(args[6])));
										}
									*/
								}
								//
							}
						}
						//
					}
				}
				//
			}
		};
	};
};