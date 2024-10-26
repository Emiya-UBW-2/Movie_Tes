#pragma once
#include "../../Header.hpp"
//
namespace FPS_n2 {
	static void		Sub_Func(std::string& func_t, const char& in_str) noexcept {
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
	//
	class EventScene {
	private:
		class Variable {
			class VARIABLE {
				std::string Base;
				std::string After;
			public:
				bool ChangeStr(std::string* pBase) const noexcept {
					if (*pBase == this->Base) {
						*pBase = this->After;
						return true;
					}
					return false;
				}
			public:
				void Add(std::string B, std::string A) noexcept {
					this->Base = B;
					this->After = A;
				}
			};
		public:
			std::vector<VARIABLE> m_Var;
		public:
			void ChangeStr(std::string* pBase) const noexcept {
				for (auto& a2 : m_Var) {
					if (a2.ChangeStr(pBase)) {
						return;
					}
				}
			}
		public:
			void Add(std::string Base, std::string After) noexcept {
				m_Var.resize(m_Var.size() + 1);
				m_Var.back().Add(Base, After);
			}
			void			Dispose(void) noexcept {
				m_Var.clear();
			}
		};
		//
		class CutInfoClass {
			class On_Off {
				size_t OnFrame = 0;
				size_t OffFrame = 0;
			public:
				On_Off(size_t on, size_t off) noexcept {
					OnFrame = on;
					OffFrame = off;
				}
				bool isFirstCut(size_t value) const noexcept { return OnFrame == value; }
				bool IsOnCut(size_t value) const noexcept { return OnFrame < value; }
				bool IsIn(size_t value) const noexcept { return OnFrame <= value && value <= OffFrame; }
				bool IsEndCut(size_t value) const noexcept { return OffFrame < value; }
			};
		private:
			std::vector<On_Off> Switch;
			bool isCutChange = false;
			bool isActive = false;
			int nowcut = 0;
			bool isFirstCut = false;
		public:
			const auto& GetisCutChange(void) const noexcept { return isCutChange; }
			const auto& GetisActive(void) const noexcept { return isActive; }
			const auto& GetNowCut(void) const noexcept { return nowcut; }
			const auto& GetisFirstCut(void) const noexcept { return isFirstCut; }
		public:
			void			AddCutSwitch(int startFrame, int ofset) noexcept { this->Switch.emplace_back(On_Off(startFrame, startFrame + ofset)); }
			void			Start(size_t Counter) noexcept;
			void			Update(size_t Counter) noexcept;
			void			Dispose(void) noexcept;
		};
		//モデルコントロール
		class ModelControl {
		private:
			class Model {
				class CutinfoDetail {
					int animsel = -1;
					bool isloop{ false };
					float animspeed{ 0.f };
					float animspeed_Dist{ -1.f };
					float animspeed_Per{ -1.f };


					Matrix4x4DX mat_p;
					//matrix用情報
					Vector3DX pos_p;
					float Yrad1_p{ 0.f };
					float Yrad2_p{ 0.f };
					bool usemat{ false };
					float startframe{ 0.f };
					float PhysicsSpeed_{ -1.f };
					float OpacityRate{ -1.f };
					float OpacityRate_Dist{ -1.f };
					float OpacityRate_Per{ -1.f };
				public:
					void Load(const std::string& func, const std::vector<std::string>& args) {
						if (func.find("SetModelAnime") != std::string::npos) {
							this->animsel = std::stoi(args[2]);
							this->isloop = (args[3].find("TRUE") != std::string::npos);
							this->animspeed = std::stof(args[4]);
							this->startframe = std::stof(args[5]);
						}
						else if (func.find("SetModelMat") != std::string::npos) {
							this->usemat = true;

							this->Yrad1_p = std::stof(args[2]);
							this->pos_p = Vector3DX::vget(std::stof(args[3]), std::stof(args[4]), std::stof(args[5]));
							this->Yrad2_p = std::stof(args[6]);

							this->mat_p =
								Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(this->Yrad1_p))
								* Matrix4x4DX::Mtrans(this->pos_p)
								* Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(this->Yrad2_p));
						}
						else if (func.find("SetModelPhysicsSpeed") != std::string::npos) {
							this->PhysicsSpeed_ = std::stof(args[2]);
						}
						else if (func.find("SetModelOpacityRate") != std::string::npos) {
							this->OpacityRate = std::stof(args[2]);
						}
						else if (func.find("SetModelMotionRate") != std::string::npos) {
							this->animspeed_Dist = std::stof(args[2]);
							this->animspeed_Per = std::stof(args[3]);
						}
						else if (func.find("SetModelOpacityEasing") != std::string::npos) {
							this->OpacityRate_Dist = std::stof(args[2]);
							this->OpacityRate_Per = std::stof(args[3]);
						}
					}

					void UpdataModel(bool isFirstLoop, Model* m) const {
						bool isFirst = isFirstLoop && m->Cutinfo.GetisFirstCut();
						if (isFirst) {
							//最初のアニメーション動作
							if (this->usemat) {
								m->obj.SetMatrix(this->mat_p);
							}
							if (this->animsel >= 0 && this->startframe >= 0.f) {
								m->obj.SetAnim(this->animsel).SetTime(this->startframe);
							}
							if (this->PhysicsSpeed_ >= 0.f) {
								m->PhysicsSpeed = this->PhysicsSpeed_;
							}
							if (this->OpacityRate >= 0.f) {
								m->OpacityRate = this->OpacityRate;
							}
							if (this->OpacityRate_Dist >= 0.f) {
								m->OpacityRate_Dist = this->OpacityRate_Dist;
							}
							if (this->OpacityRate_Per >= 0.f) {
								m->OpacityRate_Per = this->OpacityRate_Per;
							}
							if (this->animspeed_Dist >= 0.f) {
								m->animspd_Dist = this->animspeed_Dist;
							}
							if (this->animspeed_Per >= 0.f) {
								m->animspd_Per = this->animspeed_Per;
							}
						}
						//アニメーション動作
						if (this->animsel >= 0) {
							m->ChangeAnim(this->animsel, this->isloop, this->animspeed * m->animspd, isFirst);
						}
					}
				};
			private:
				float b_run{ 0.f };
				float b_runrange{ 0.f };
				std::vector<std::pair<int, std::string>> FrameNum;
				moves move;
				int Anim_Sel = 0;
				Matrix4x4DX mat_p;			//matrix用情報
				Vector3DX pos_p;
				float Yrad1_p{ 0.f };
				float Zrad1_p{ 0.f };
				float Yrad2_p{ 0.f };
				int prevID = -1;
				float AnimChange{ 0.f };
			public:
				bool isBGModel{ false };
				bool ShadowDrawActive{ true };//影を書き込むか
				bool IsNearShadow = true;
				bool IsFarShadow{ false };
				bool isBase = true;
				std::string Path;
				size_t BaseID = 0;
				size_t numBase = 0;
				MV1 obj;
				float PhysicsSpeed{ 1.f };
				float OpacityRate{ 1.f };
				float OpacityRate_Dist{ 1.f };
				float OpacityRate_Per{ 1.f };
				CutInfoClass Cutinfo;
				std::vector<CutinfoDetail> CutDetail;//カットの詳細
				bool isEndLoad{ false };
				bool isFarPhysics{ false };
				bool canUpdate{ true };
				float animspd{ 0.f };
				float animspd_Dist{ 1.f };
				float animspd_Per{ 1.f };
			public:
				Model(void) noexcept {
					isEndLoad = false;
					isFarPhysics = false;
					animspd = 1.f;
				}
				void			Init(int startFrame, int ofset) noexcept {
					this->CutDetail.resize(this->CutDetail.size() + 1);
					this->Cutinfo.AddCutSwitch(startFrame, ofset);
				}
				void			AddFrame(std::string_view FrameName) noexcept {
					int siz = obj.GetFrameNum();
					for (int i = 0; i < siz; i++) {
						if (obj.GetFrameName(i) == FrameName) {
							FrameNum.emplace_back(std::make_pair((int)i, FrameName));
							return;
						}
					}
				}
				const auto		GetFrame(std::string_view FrameName) const noexcept {
					for (auto& F : FrameNum) {
						if (F.second == FrameName) {
							return static_cast<Vector3DX>(obj.GetFramePosition(F.first));
						}
					}
					return Vector3DX::zero();
				}
				const auto		GetFrameMat(std::string_view FrameName) const noexcept {
					for (auto& F : FrameNum) {
						if (F.second == FrameName) {
							return obj.GetFrameLocalWorldMatrix(F.first);
						}
					}
					return Matrix4x4DX::identity();
				}
				static void		Sel_AnimNum(MV1& model, int sel, float pers) noexcept {
					for (int i = 0, Num = static_cast<int>(model.GetAnimNum()); i < Num; ++i) {
						model.SetAnim(i).SetPer((i == sel) ? pers : (1.f - pers));
					}
				}
				void			ChangeAnim(int ID, bool isloop, float speed, bool isfastchange) noexcept {
					auto* DrawParts = DXDraw::Instance();
					if (isfastchange) {
						AnimChange = 1.f;
					}
					if (prevID != ID) {
						AnimChange = 1.f;
						//this->obj.SetAnim(ID).time = this->obj.GetAnim(prevID).time;
					}
					prevID = ID;
					Sel_AnimNum(this->obj, ID, AnimChange);
					AnimChange = std::clamp(AnimChange + 1.f / DrawParts->GetFps(), 0.f, 1.f);
					if (ID < this->obj.GetAnimNum()) {
						this->obj.SetAnim(ID).Update(isloop, speed);
					}
				}
				void			Update(bool isUpdateAnim) noexcept {
					if (this->OpacityRate_Per < 1.f) {
						Easing(&this->OpacityRate, this->OpacityRate_Dist, this->OpacityRate_Per, EasingType::OutExpo);
					}
					if (this->animspd_Per < 1.f) {
						Easing(&this->animspd, this->animspd_Dist, this->animspd_Per, EasingType::OutExpo);
					}

					MV1SetOpacityRate(this->obj.get(), this->OpacityRate);
					if (this->Cutinfo.GetisActive() && isUpdateAnim) {
						this->obj.UpdateAnimAll();
					}
				}
				void			Draw(int isfar_ = -1) const noexcept {
					if (this->Cutinfo.GetisActive() && this->OpacityRate > 0.f) {
						if (isfar_ == TRUE) {
							this->obj.DrawModel();
						}
						else {
							this->obj.DrawModel();
						}
					}
				}
				void			SetPhysics(bool isReset, float x) const noexcept {
					if (this->Cutinfo.GetisActive()) {
						if (isReset) {
							this->obj.PhysicsResetState();
						}
						else {
							auto* DrawParts = DXDraw::Instance();
							this->obj.PhysicsCalculation(1000.0f / (DrawParts->GetFps() * (120.f / 60.f)) * this->PhysicsSpeed * x);
						}
					}
				}
				void			Dispose(void) noexcept {
					FrameNum.clear();
					CutDetail.clear();
				}
			};
		private:
			std::vector<Model> model;
			size_t P_cnt = 0;
		public:
			ModelControl(void) noexcept {
				Dispose();
			}
			~ModelControl(void) noexcept {
				Dispose();
			}
		public:
			void			Load(std::string_view Path) noexcept;
			const Model* Get(std::string_view Path, size_t Sel = 0) const noexcept {
				for (auto& m : model) {
					if (m.Path == Path && m.BaseID == Sel) {
						return &(m);
					}
				}
				return nullptr;
			}
			Model* Get(std::string_view Path, size_t Sel = 0) noexcept {
				for (auto& m : model) {
					if (m.Path == Path && m.BaseID == Sel) {
						return &(m);
					}
				}
				return nullptr;
			}
			void			Start(size_t Counter) noexcept;
			void			FirstUpdate(size_t Counter, bool isFirstLoop, bool reset_p) noexcept;
			void			SetPhysics(bool reset_p) noexcept;
			void			SetAfterLoad(void) noexcept;
			void			Draw_Far(void) const noexcept;
			void			Draw(bool innearshadow, bool infarshadow, bool ShadowActive, int isCheckFar = -1) const noexcept;
			void			Dispose(void) noexcept;
		};
		class GraphControl {
		private:
			//詳細
			class Graph {
				class CutinfoDetail {
					float OpacityRate{ -1.f };
					float OpacityRate_Dist{ -1.f };
					float OpacityRate_Per{ -1.f };
				public:
					void Load(const std::string& func, const std::vector<std::string>& args) {
						if (func.find("SetGraphOpacityRate") != std::string::npos) {
							this->OpacityRate_Dist = std::stof(args[2]);
							this->OpacityRate_Per = 0.f;
							this->OpacityRate = std::stof(args[2]);
						}
						else if (func.find("SetGraphOpacityEasing") != std::string::npos) {
							this->OpacityRate_Dist = std::stof(args[2]);
							this->OpacityRate_Per = std::stof(args[3]);
						}
					}

					void UpdataGraph(bool isFirstLoop, Graph* m) const {
						if (this->OpacityRate_Dist >= 0.f) {
							if (this->OpacityRate_Per == 0) {
								if (isFirstLoop && m->Cutinfo.GetisFirstCut()) {
									//最初のアニメーション動作
									m->Alpha.Set(this->OpacityRate_Dist, this->OpacityRate_Per);
								}
							}
							//0以外はいーじんぐ
							else  if (this->OpacityRate_Per >= 0.f) {
								m->Alpha.Set(this->OpacityRate_Dist, this->OpacityRate_Per);
							}
						}
						//アニメーション動作

					}
				};
			private:
				class infos_rand {
				public:
					float Ans = 0;
					float Base = 0;
					float Per = 1.f;
					void			Set(float m_p, float m_base, float m_per) noexcept {
						Ans = m_p;
						Base = m_base;
						Per = m_per;
					}
					void			Set(float m_base, float m_per) noexcept {
						Base = m_base;
						Per = m_per;
					}
					void			UpdateRand(void) noexcept {
						Easing(&this->Ans, GetRandf(this->Base), this->Per, EasingType::OutExpo);
					}
				};
				class infos {
				public:
					float Ans = 0;
					float Base = 0;
					float Per = 1.f;
					infos_rand Rand_;
					void			Set(float m_p, float m_base, float m_per) noexcept {
						Ans = m_p;
						Base = m_base;
						Per = m_per;
						Rand_.Set(0, 0, 1.f);
					}
					void			Set(float m_base, float m_per) noexcept {
						Base = m_base;
						Per = m_per;
					}
					void			Update(void) noexcept {
						Rand_.UpdateRand();
						Easing(&this->Ans, this->Base + this->Rand_.Ans, this->Per, EasingType::OutExpo);
					}
				};
			private:
				GraphHandle handle;
				int Bright_R = 255;
				int Bright_G = 255;
				int Bright_B = 255;
			public:
				infos X;
				infos Y;
				infos Rad;
				infos Scale;
				infos Alpha;
				int xsize = -1;
				int ysize = -1;
				CutInfoClass Cutinfo;
				std::vector<CutinfoDetail> CutDetail;//オンにするカット
				std::string Path;
			public:
				Graph(void) noexcept {
					CutDetail.clear();
				}
				void			Dispose(void) noexcept {
					CutDetail.clear();
				}

				void			Init(int startFrame, int ofset) noexcept {
					this->CutDetail.resize(this->CutDetail.size() + 1);
					this->Cutinfo.AddCutSwitch(startFrame, ofset);
				}
				void			SetBright(int b_r, int b_g, int b_b) noexcept {
					Bright_R = b_r;
					Bright_G = b_g;
					Bright_B = b_b;
				}
				void			Set(float xp, float yp, float rd, float al, float sc, std::string_view Path_t) noexcept {
					this->X.Set(xp, 0.f, 1.f);
					this->Y.Set(yp, 0.f, 1.f);
					this->Rad.Set(rd, 0.f, 1.f);
					this->Alpha.Set(al, 1.f, 1.f);
					this->Scale.Set(sc, 0.f, 1.f);
					this->handle.Load(Path_t);
					this->handle.GetSize(&xsize, &ysize);
				}
				void			Set_Base(float xp, float xper, float yp, float yper, float rd, float scl, float rdper, float sclper, float alp, float alpper) noexcept {
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
				void			Set_Rand(float xp, float xper, float yp, float yper, float rd, float rdper, float scl, float sclper) noexcept {
					this->X.Rand_.Set(xp, xper);
					this->Y.Rand_.Set(yp, yper);
					this->Rad.Rand_.Set(deg2rad(rd), rdper);
					this->Scale.Rand_.Set(scl, sclper);
				}
				void			Update(void) noexcept {
					this->X.Update();
					this->Y.Update();
					this->Rad.Update();
					this->Scale.Update();
					this->Alpha.Update();
				}
				void			Draw(void) const noexcept {
					if (this->Cutinfo.GetisActive() && this->Alpha.Ans > 0.f) {
						SetDrawBright(Bright_R, Bright_G, Bright_B);

						auto* DrawParts = DXDraw::Instance();

						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f * this->Alpha.Ans));

						this->handle.DrawRotaGraph(DrawParts->GetUIY((int)(this->X.Ans)), DrawParts->GetUIY((int)(this->Y.Ans)), (float)DrawParts->GetUIY(1920) / this->ysize * this->Scale.Ans, this->Rad.Ans, true);

						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

						SetDrawBright(255, 255, 255);
					}
				}
			};
			std::vector<Graph> model;
		public:
			GraphControl(void) noexcept;
			void			Load(float xp, float yp, float rad, float alpha, float scale, std::string_view Path) noexcept;
			Graph* Get(std::string_view Path, size_t Sel = 0) noexcept {
				int Cnt = 0;
				for (auto& m : model) {
					if (m.Path == Path) {
						if (Cnt >= Sel) {
							return &(m);
						}
						Cnt++;
					}
				}
				return nullptr;
			}
			void			Start(size_t Counter) noexcept;
			void			FirstUpdate(size_t Counter, bool isFirstLoop) noexcept;
			void			Draw(void) const noexcept;
			void			Dispose(void) noexcept;
		};
		class SEControl {
		private:
			class SE {
			public:
				SoundEnum	m_SoundEnum{};
				int			m_PlayCounter{ -1 };
			public:
				void			Init(std::string_view Path, int startFrame) noexcept {
					for (int i = 0; i < (int)SoundEnum::Max; i++) {
						if (Path == SoundID[i]) {
							m_SoundEnum = (SoundEnum)i;
							break;
						}
					}
					m_PlayCounter = startFrame;
				}
			};
			std::vector<SE> model;
		public:
			SEControl(void) noexcept {
				Dispose();
			}
			SE* GetLast(void) noexcept {
				model.resize(model.size() + 1);
				return &(model.back());
			}
			void			Update(size_t Counter, bool isFirstLoop) noexcept {
				auto* SE = SoundPool::Instance();
				for (auto& m : model) {
					if (m.m_PlayCounter == Counter && isFirstLoop) {
						SE->Get((int)m.m_SoundEnum).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
				}
			}
			void			Dispose(void) noexcept { model.clear(); }
		};
		class TelopClass {
		private:
			class Cut_tex {
				int xpos = 0;
				int ypos = 0;
				int size = 0;
				int LMR = 1;
				std::string str;
				LONGLONG START_TIME = 0;
				LONGLONG END_TIME = 0;
			public:
				Cut_tex(void) noexcept;
				void			Set(int xp, int yp, int Fontsize, std::string_view mag, LONGLONG StartF, LONGLONG ContiF, int m_LMR) noexcept;
				void			Draw(LONGLONG nowTimeWait) const noexcept;
			};
		private:
			std::vector<Cut_tex> Texts;
			LONGLONG StartF = 0;
			LONGLONG ContiF = 0;
		public:
			void			Init(void) noexcept;
			void			LoadTelop(const std::string& func, const std::vector<std::string>& args) noexcept;
			void			Draw(LONGLONG nowTimeWait) const noexcept;
			void			Dispose(void) noexcept;
		};

		class FogParam {
		public:
			int fog[3]{ -1,-1,-1 };
			float fog_range[2]{ -1.f,-1.f };
		public:
			void Reset(void) noexcept {
				fog[0] = 128;
				fog[1] = 128;
				fog[2] = 128;
				fog_range[0] = 200.f;
				fog_range[1] = 300000.f;
			}
			void SetFog() const noexcept {
				SetFogDensity(0.01f);
				SetFogColor(fog[0], fog[1], fog[2]);
				SetFogStartEnd(fog_range[0], fog_range[1]);
			}
		};
		class ForcusControl {
			bool		Use{ false };
			std::string	Path;
			size_t		ID{};
			std::string	Frame;
			Vector3DX	Add;
		public:
			ForcusControl(std::string_view path, size_t id, std::string_view frame, const Vector3DX& add) noexcept;
		public:
			const auto& GetIsUse(void) const noexcept { return Use; }
			const auto	GetForce(const ModelControl& model) const noexcept { return model.Get(this->Path, this->ID)->GetFrame(this->Frame) + this->Add; }
		};
		class Cut_Info_First {
		public:
			Camera3DInfo Aim_camera;
			float cam_per{ 0.f };
			bool isResetRandCampos{ false };
			bool isResetRandCamvec{ false };
			bool isResetRandCamup{ false };
		private:
			bool IsResetPhysics{ false };
			FogParam Fog{};
			std::vector<ForcusControl> Forcus;
			LONGLONG TimeLimit{ 0 };
			bool UsePrevAim{ false };
		public:
			Cut_Info_First(LONGLONG value) noexcept;
			~Cut_Info_First(void) noexcept;
		public:
			const auto&		GetIsResetPhysics(void) const noexcept { return IsResetPhysics; }
			const auto&		GetTimeLimit(void) const noexcept { return TimeLimit; }
			bool			GetForcusCenter(Vector3DX* vec, const ModelControl& model) const noexcept;
			void			UpdateCam(Camera3DInfo* cam) const noexcept;
			void			SetUpFog(void) noexcept;
			void			SetPrev(const Cut_Info_First& tgt) noexcept;
			void			LoadScript(const std::string& func, const std::vector<std::string>& args) noexcept;
		};
		class Cut_Info_Update {
		private:
			bool isUseNotFirst{ false };
			float NotFirst_per = -1.f;
			float fov_per{ 0.f };
			Camera3DInfo CameraNotFirst;
			Camera3DInfo CameraNotFirst_After;
			Camera3DInfo CameraNotFirst_Vec;
			float m_RandcamupPer;
			Vector3DX m_RandcamupSet;
			float m_RandcamvecPer;
			Vector3DX m_RandcamvecSet;
			float m_RandcamposPer;
			Vector3DX m_RandcamposSet;
			std::vector<ForcusControl> Forcus;
			size_t CutSel = 0;
			size_t OLDCutSel = SIZE_MAX;
			float campos_per{ 0.f };
			float camvec_per{ 0.f };
			float camup_per{ 0.f };
			bool IsUsePrevBuf{ false };
			bool IsSetBlack{ false };
			float Black_Per = 1.f;
			float Black = 0.f;
			bool IsSetWhite{ false };
			float White_Per = 1.f;
			float White = 0.f;;
		public:
			Cut_Info_Update(void) noexcept;
			~Cut_Info_Update(void) noexcept;
		private:
			void			SetForce(float camvecPer, std::string_view ModelPath, int ModelID, std::string_view Frame, const Vector3DX& Add) noexcept;
			bool			GetForcusCenter(Vector3DX* vec, const ModelControl& model) const noexcept;
		public:
			void			SetupCam(const Camera3DInfo& cam) noexcept { CameraNotFirst = cam; }
			void			LoadScript(const std::string& func, const std::vector<std::string>& args) noexcept;
			void			ResetCam(const Camera3DInfo& cam) noexcept;
			void			Update(Cut_Info_First& Camera,
				Vector3DX* m_RandcamupBuf, Vector3DX* m_RandcamvecBuf, Vector3DX* m_RandcamposBuf,
				float* pBlack, float* pWhite,
				const ModelControl& model
			) noexcept;
			void			SetupByPrev(const Cut_Info_Update& Prev) noexcept;
		};
	private:
		Variable					m_Variable;

		ModelControl					m_ModelControl;
		GraphControl					m_GraphControl;
		SEControl						m_SEControl;
		TelopClass						m_TelopClass;

		std::vector<Cut_Info_First>		m_CutInfo;
		std::vector<Cut_Info_Update>	m_CutInfoUpdate;
		CutInfoClass					m_PosCamCut;
		std::vector<Vector3DX>			m_PosCam;
		Vector3DX		m_RandcamupBuf;
		Vector3DX		m_RandcamvecBuf;
		Vector3DX		m_RandcamposBuf;
		float			Black_Buf{ 0.f };
		float			White_Buf{ 1.f };
		LONGLONG		deltatime{ 0 };
		LONGLONG		m_BaseTime{ 0 }, m_NowTime{ 0 };
		size_t			m_Counter{ 0 };//カット
		int				m_count{ 0 };

		int NowCut = 0;
	private:
		//スクリプト読み込み処理
		bool			LoadOnce(std::string_view func_t) noexcept;
	public:
		auto			IsEnd() const noexcept { return (m_Counter >= m_CutInfo.size()); }
	public:
		void			Load(void) noexcept;
		void			Start(size_t StartCut) noexcept;
		void			GetDeltaTime(void) noexcept;
		void			Update(void) noexcept;
		void			Dispose_Load(void) noexcept;

		void			BGDraw(void) const noexcept;
		void			ShadowFarDraw(void) const noexcept;
		void			ShadowDraw(void) const noexcept;
		void			SetShadowDraw(void) const noexcept;
		void			MainDraw(void) const noexcept;
		void			UIDraw() const noexcept;
	};
};
