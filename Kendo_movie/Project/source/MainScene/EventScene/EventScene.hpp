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
			void			Start(size_t Counter) noexcept {
				this->isCutChange = false;
				this->isActive = false;
				this->isFirstCut = false;

				this->nowcut = 0;
				while (true) {
					if (this->Switch.size() > this->nowcut) {
						auto& inf_b = this->Switch[this->nowcut];
						if (inf_b.IsOnCut(Counter)) {
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
				if (0 <= this->nowcut - 1 && static_cast<size_t>(this->nowcut - 1) < this->Switch.size()) {
					auto& inf_b = this->Switch[static_cast<size_t>(this->nowcut - 1)];
					this->isFirstCut = inf_b.isFirstCut(Counter);
					if (inf_b.IsIn(Counter)) {
						this->nowcut--;
						this->isActive = true;
					}
				}
			}
			void			Update(size_t Counter) noexcept {
				this->isCutChange = false;
				if (this->Switch.size() > this->nowcut) {
					auto& inf_b = this->Switch[this->nowcut];
					this->isFirstCut = inf_b.isFirstCut(Counter);
					if (this->isFirstCut) {
						this->isActive = true;
					}
					if (this->isActive) {
						if (inf_b.IsEndCut(Counter)) {
							this->isActive = false;
							this->nowcut++;
							this->isCutChange = true;
						}
					}
				}
			}
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
			};
		private:
			std::vector<Model> model;
			size_t Max = 0;
			size_t P_cnt = 0;
		public:
			const auto& GetMax(void) const noexcept { return Max; }
			const auto& GetModel(void) const noexcept { return model; }
		public:
			ModelControl(void) noexcept {
				model.resize(64);
				Max = 0;
			}
			~ModelControl(void) noexcept {}
		public:
			void			Load(std::string_view Path) noexcept {
				MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
				for (int i = 0; i < 256; i++) {
					MV1SetLoadCalcPhysicsWorldGravity(i, Vector3DX::vget(0, 0, 1.f).get());
				}
				MV1SetLoadModelPhysicsCalcPrecision(1);

				for (size_t i = 0; i < Max; i++) {
					if (model[i].isBase && model[i].Path == Path) {
						model[Max].Path = Path;
						model[Max].isBase = false;
						model[Max].obj.Duplicate(model[i].obj);
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
				MV1::Load(model[Max].Path, &(model[Max].obj), DX_LOADMODEL_PHYSICS_REALTIME);/*DX_LOADMODEL_PHYSICS_REALTIME*/
				Max++;
			}
			const Model* Get(std::string_view Path, size_t Sel = 0) const noexcept {
				for (size_t i = 0; i < Max; i++) {
					if (model[i].Path == Path && model[i].BaseID == Sel) {
						return &(model[i]);
					}
				}
				return nullptr;
			}
			Model* Get(std::string_view Path, size_t Sel = 0) noexcept {
				for (size_t i = 0; i < Max; i++) {
					if (model[i].Path == Path && model[i].BaseID == Sel) {
						return &(model[i]);
					}
				}
				return nullptr;
			}
			void			Start(size_t Counter) noexcept {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					m.Cutinfo.Start(Counter);
				}
			}
			void			FirstUpdate(size_t Counter, bool isFirstLoop, bool reset_p) noexcept {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					while (true) {
						m.Cutinfo.Update(Counter);
						if (m.Cutinfo.GetisActive() && (m.Cutinfo.GetNowCut() < m.CutDetail.size())) {
							m.CutDetail[m.Cutinfo.GetNowCut()].UpdataModel(isFirstLoop, &m);
						}
						if (m.Cutinfo.GetisCutChange()) {
							continue;
						}
						break;
					}
				}

				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					m.canUpdate = true;
				}
				//++P_cnt %= 2;

				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					if (m.Cutinfo.GetisActive()) {
						m.Update(m.canUpdate);/**/
					}
				}
				SetPhysics(reset_p);
			}
			void			SetPhysics(bool reset_p) noexcept {
				if (reset_p) {
					for (size_t i = 0; i < Max; i++) {
						model[i].SetPhysics(true, 1.f);
					}
				}
				else {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						if (!m.isFarPhysics) {
							m.SetPhysics(true, 1.f);
							m.isFarPhysics = true;
						}
					}
					//
				}
			}
			void			SetAfterLoad(void) noexcept {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					//
					if (!m.isEndLoad && CheckHandleASyncLoad(m.obj.get()) == FALSE) {
						m.isEndLoad = true;
						m.AddFrame("上半身");
						m.AddFrame("上半身2");
						m.AddFrame("首");
						m.AddFrame("頭");
						m.AddFrame("右目");
						m.AddFrame("左目");
						m.AddFrame("右人指２");
						m.AddFrame("右ひざ");
						m.AddFrame("左人指２");
						m.AddFrame("右つま先");
						m.AddFrame("左つま先");

						m.AddFrame("ｷｬﾉﾋﾟｰ");
						m.AddFrame("前回転");
						m.AddFrame("銃口先");

						MV1::SetAnime(&(m.obj), m.obj);
					}
				}
				//モデルのMV1保存
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					if ((m.Path.find(".pmx") != std::string::npos) && (m.BaseID == 0)) {
						m.obj.SaveModelToMV1File((m.Path.substr(0, m.Path.find(".pmx")) + ".mv1").c_str(), MV1_SAVETYPE_NORMAL, -1, 1, 1, 1, 0, 0);
						//m.obj.SaveModelToMV1File((m.Path.substr(0, m.Path.find(".pmx")) + ".mv1").c_str());
					}
				}
			}
			void			Draw_Far(void) const noexcept {
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
			void			Draw(bool innearshadow, bool infarshadow, bool ShadowActive, int isCheckFar = -1) const noexcept {
				if (infarshadow) {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						if (!m.isBGModel && m.IsFarShadow) {
							if (ShadowActive && !m.ShadowDrawActive) { continue; }
							m.Draw(isCheckFar);
						}
					}
				}
				else if (innearshadow) {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						if (!m.isBGModel && m.IsNearShadow) {
							if (ShadowActive && !m.ShadowDrawActive) { continue; }
							m.Draw(isCheckFar);
						}
					}
				}
				else {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						if (!m.isBGModel) {
							if (ShadowActive && !m.ShadowDrawActive) { continue; }
							m.Draw(isCheckFar);
						}
					}
				}
			}
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
			size_t Max = 0;
		public:
			GraphControl(void) noexcept {
				model.resize(64);
				Max = 0;
			}
			void			Load(float xp, float yp, float rad, float alpha, float scale, std::string_view Path) noexcept {
				model[Max].Path = Path;
				model[Max].Set(xp, yp, rad, alpha, scale, Path);
				Max++;
			}
			Graph* Get(std::string_view Path, size_t Sel = 0) noexcept {
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
			void			Start(size_t Counter) noexcept {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					m.Cutinfo.Start(Counter);
				}
			}
			void			FirstUpdate(size_t Counter, bool isFirstLoop) noexcept {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					while (true) {
						m.Cutinfo.Update(Counter);
						if (m.Cutinfo.GetisActive() && (m.Cutinfo.GetNowCut() < m.CutDetail.size())) {
							m.CutDetail[m.Cutinfo.GetNowCut()].UpdataGraph(isFirstLoop, &m);
						}
						if (m.Cutinfo.GetisCutChange()) {
							continue;
						}
						break;
					}
				}
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					m.Update();
				}
			}
			void			Draw(void) const noexcept {
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					m.Draw();
				}
			}
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
			size_t Max = 0;
		public:
			SEControl(void) noexcept {
				model.resize(64);
				Max = 0;
			}

			SE* Get(size_t Sel) noexcept { return &(model[Sel]); }

			SE* GetLast() noexcept {
				Max++;
				return &(model[Max - 1]);
			}


			void			Update(size_t Counter, bool isFirstLoop) noexcept {
				auto* SE = SoundPool::Instance();
				for (size_t i = 0; i < Max; i++) {
					auto& m = model[i];
					if (m.m_PlayCounter == Counter && isFirstLoop) {
						SE->Get((int)m.m_SoundEnum).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
				}
			}
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
				Cut_tex(void) noexcept {
					xpos = 0;
					ypos = 0;
					size = 12;
					str = "test";
					START_TIME = (LONGLONG)(1000000.f * 0.01f);
					END_TIME = (LONGLONG)(1000000.f * 1.01f);
				}
				void			Set(int xp, int yp, int Fontsize, std::string_view mag, LONGLONG StartF, LONGLONG ContiF, int m_LMR) noexcept {
					this->xpos = xp;
					this->ypos = yp;
					this->size = Fontsize;
					this->str = mag;
					this->START_TIME = StartF;
					this->END_TIME = StartF + ContiF;;
					this->LMR = m_LMR;
				}
				void			Draw(LONGLONG nowTimeWait) const noexcept {
					if (this->START_TIME < nowTimeWait && nowTimeWait < this->END_TIME) {
						auto* Fonts = FontPool::Instance();
						switch (this->LMR) {
						case 0:
							Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(this->size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
							break;
						case 1:
							Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(this->size, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
							break;
						case 2:
							Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(this->size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
							break;
						default:
							break;
						}
					}
				}
			};
		private:
			std::vector<Cut_tex> Texts;
			LONGLONG StartF = 0;
			LONGLONG ContiF = 0;
		public:
			void			Init(void) noexcept {
				StartF = 0;
				ContiF = 0;
			}
			void			LoadTelop(const std::string& func, const std::vector<std::string>& args) noexcept {
				auto* DrawParts = DXDraw::Instance();
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
					Texts.back().Set(DrawParts->GetScreenY(std::stoi(args[0])), DrawParts->GetScreenY(std::stoi(args[1])), DrawParts->GetScreenY(std::stoi(args[2])), args[3], StartF, ContiF, t);
				}
			}
			void			Draw(LONGLONG nowTimeWait) const noexcept {
				for (auto& t : Texts) {
					t.Draw(nowTimeWait);
				}
			}
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
			ForcusControl(std::string_view path, size_t id, std::string_view frame, const Vector3DX& add) noexcept {
				this->Use = true;
				this->Path = path;
				this->ID = id;
				if (frame == "UPPER") {
					this->Frame = "上半身";
				}
				else if (frame == "UPPER_2") {
					this->Frame = "上半身2";
				}
				else if (frame == "NECK") {
					this->Frame = "首";
				}
				else if (frame == "HEAD") {
					this->Frame = "頭";
				}

				else if (frame == "LEFT_EYE") {
					this->Frame = "右目";
				}
				else if (frame == "RIGHT_EYE") {
					this->Frame = "左目";
				}
				else if (frame == "RIGHT_FINGER") {
					this->Frame = "右人指２";
				}
				else if (frame == "LEFT_FINGER") {
					this->Frame = "左人指２";
				}
				else if (frame == "RIGHTLEG") {
					this->Frame = "右ひざ";
				}
				else if (frame == "RIGHTFOOT") {
					this->Frame = "右つま先";
				}
				else if (frame == "LEFTFOOT") {
					this->Frame = "左つま先";
				}

				else if (frame == "CANOPY") {
					this->Frame = "ｷｬﾉﾋﾟｰ";
				}
				else if (frame == "FRONTWHEEL") {
					this->Frame = "前回転";
				}
				else if (frame == "MOVE") {
					this->Frame = "移動";
				}
				else if (frame == "MUZZLE") {
					this->Frame = "銃口先";
				}
				this->Add = add;
			}
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
			//Getter
			const auto& GetIsResetPhysics(void) const noexcept { return IsResetPhysics; }
			const auto& GetTimeLimit(void) const noexcept { return TimeLimit; }
			bool			GetForcusCenter(Vector3DX* vec, const ModelControl& model) const noexcept {
				bool isforcus = false;
				for (auto& f : Forcus) {
					if (f.GetIsUse()) {
						if (!isforcus) {
							*vec = Vector3DX::zero();
						}
						*vec += f.GetForce(model);
						isforcus = true;
					}
				}
				if (isforcus) {
					*vec = *vec / (float)(Forcus.size());
				}
				return isforcus;
			}
			void			UpdateCam(Camera3DInfo* cam) const noexcept {
				Vector3DX pos_t = cam->GetCamPos();
				Vector3DX vec_t = cam->GetCamVec();
				Vector3DX up_t = cam->GetCamUp();
				float fov_t = cam->GetCamFov();
				float near_t = cam->GetCamNear();
				float far_t = cam->GetCamFar();
				Easing(&pos_t, Aim_camera.GetCamPos(), cam_per, EasingType::OutExpo);
				Easing(&vec_t, Aim_camera.GetCamVec(), cam_per, EasingType::OutExpo);
				Easing(&up_t, Aim_camera.GetCamUp(), cam_per, EasingType::OutExpo);
				Easing(&fov_t, Aim_camera.GetCamFov(), cam_per, EasingType::OutExpo);
				Easing(&far_t, Aim_camera.GetCamFar(), cam_per, EasingType::OutExpo);
				Easing(&near_t, Aim_camera.GetCamNear(), cam_per, EasingType::OutExpo);
				cam->SetCamPos(pos_t, vec_t, up_t);
				cam->SetCamInfo(fov_t, near_t, far_t);
			}
			void			SetUpFog(void) noexcept {
				if (Fog.fog[0] >= 0) {
					SetFogEnable(TRUE);
					Fog.SetFog();
				}
				else if (Fog.fog[0] == -2) {
					FogParam Tmp{}; Tmp.Reset();
					SetFogEnable(TRUE);
					Tmp.SetFog();
				}
			}
			//
			Cut_Info_First(LONGLONG value) noexcept {
				TimeLimit = value;
				Aim_camera.SetCamPos(Vector3DX::vget(0, 10, -30), Vector3DX::vget(0, 10, 0), Vector3DX::up());
				Aim_camera.SetCamInfo(deg2rad(15), 1.f, 300.f);
				cam_per = 0.95f;
				IsResetPhysics = false;
			}
			~Cut_Info_First(void) noexcept {}
			void			SetPrev(const Cut_Info_First& tgt) noexcept {
				if (this->UsePrevAim) {
					this->Aim_camera = tgt.Aim_camera;
					this->cam_per = tgt.cam_per;
				}
			}
			void LoadScript(const std::string& func, const std::vector<std::string>& args) noexcept {
				//Campos
				if (func.find("SetCampos_NoneRad") != std::string::npos) {
					this->Aim_camera.SetCamPos(
						Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
						this->Aim_camera.GetCamVec(),
						this->Aim_camera.GetCamUp()
					);
				}
				//Camvec
				else if (func.find("SetCamvec") != std::string::npos) {
					this->Aim_camera.SetCamPos(
						this->Aim_camera.GetCamPos(),
						Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
						this->Aim_camera.GetCamUp()
					);
				}
				else if (func.find("SetCamForcus") != std::string::npos) {
					this->Forcus.emplace_back(ForcusControl(args[0], std::stol(args[1]), args[2], Vector3DX::vget(std::stof(args[3]), std::stof(args[4]), std::stof(args[5]))));
				}
				//CamUp
				else if (func.find("SetCamup") != std::string::npos) {
					this->Aim_camera.SetCamPos(
						this->Aim_camera.GetCamPos(),
						this->Aim_camera.GetCamVec(),
						Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]))
					);
				}
				//Else
				else if (func.find("SetCamInfo") != std::string::npos) {
					this->Aim_camera.SetCamInfo(deg2rad(std::stof(args[0])), std::stof(args[1]), std::stof(args[2]));
					this->cam_per = std::stof(args[3]);
				}
				//物理を次でリセットする
				else if (func.find("SetNextResetPhysics") != std::string::npos) {
					this->IsResetPhysics = true;
				}
				//前のカットカメラを使用
				else if (func.find("SetPrevCamInfo") != std::string::npos) {
					this->UsePrevAim = true;
				}
				//どの距離で描画するかをセット
				else if (func.find("ResetCamPosRand") != std::string::npos) {
					this->isResetRandCampos = true;
				}
				else if (func.find("ResetCamVecRand") != std::string::npos) {
					this->isResetRandCamvec = true;
				}
				else if (func.find("ResetCamUpRand") != std::string::npos) {
					this->isResetRandCamup = true;
				}
				else if (func.find("SetFog") != std::string::npos) {
					if (args.size() == 5) {
						this->Fog.fog[0] = std::stoi(args[0]);
						this->Fog.fog[1] = std::stoi(args[1]);
						this->Fog.fog[2] = std::stoi(args[2]);
						this->Fog.fog_range[0] = std::stof(args[3]);
						this->Fog.fog_range[1] = std::stof(args[4]);
					}
					else {
						this->Fog.fog[0] = -2;
					}
				}
			}
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
			Cut_Info_Update(void) noexcept {
				isUseNotFirst = false;
				IsUsePrevBuf = false;

				campos_per = 1.f;
				camvec_per = 1.f;
				camup_per = 1.f;
				fov_per = 1.f;

				NotFirst_per = -1.f;

				m_RandcamupPer = 0.f;
				m_RandcamupSet = Vector3DX::zero();
				m_RandcamvecPer = 0.f;
				m_RandcamvecSet = Vector3DX::zero();
				m_RandcamposPer = 0.f;
				m_RandcamposSet = Vector3DX::zero();
				Forcus.clear();
			}
			~Cut_Info_Update(void) noexcept {}
		private:
			void			SetForce(float camvecPer, std::string_view ModelPath, int ModelID, std::string_view Frame, const Vector3DX& Add) noexcept {
				this->camvec_per = camvecPer;
				this->Forcus.emplace_back(ForcusControl(ModelPath, ModelID, Frame, Add));
			}
			bool GetForcusCenter(Vector3DX* vec, const ModelControl& model) const noexcept {
				bool isforcus = false;
				for (auto& f : Forcus) {
					if (f.GetIsUse()) {
						if (!isforcus) {
							*vec = Vector3DX::zero();
						}
						*vec += f.GetForce(model);
						isforcus = true;
					}
				}
				if (isforcus) {
					*vec = *vec / (float)(Forcus.size());
				}
				return isforcus;
			}
		public:
			void			SetupCam(const Camera3DInfo& cam) noexcept { CameraNotFirst = cam; }
			void			LoadScript(const std::string& func, const std::vector<std::string>& args) noexcept {
				//カメラのアップデート
				if (func.find("SetUpdateEnable") != std::string::npos) {
					this->isUseNotFirst = true;
				}
				//camvec
				else if (func.find("SetUpdateCamvec") != std::string::npos) {
					this->camvec_per = std::stof(args[0]);
					if (args.size() > 1) {
						this->CameraNotFirst.SetCamPos(
							this->CameraNotFirst.GetCamPos(),
							Vector3DX::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])),
							this->CameraNotFirst.GetCamUp());
					}
				}
				else if (func.find("SetUpdateCamForcus") != std::string::npos) {
					this->SetForce(std::stof(args[0]), args[1], std::stoi(args[2]), args[3], Vector3DX::vget(std::stof(args[4]), std::stof(args[5]), std::stof(args[6])));
				}
				else if (func.find("SetVectorUpdateCamvec") != std::string::npos) {
					this->CameraNotFirst_Vec.SetCamPos(
						this->CameraNotFirst_Vec.GetCamPos(),
						Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
						this->CameraNotFirst_Vec.GetCamUp());
				}
				//campos
				else if (func.find("SetUpdateCampos") != std::string::npos) {
					this->campos_per = std::stof(args[0]);
					if (args.size() > 1) {
						this->CameraNotFirst.SetCamPos(
							Vector3DX::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])),
							this->CameraNotFirst.GetCamVec(),
							this->CameraNotFirst.GetCamUp());
					}
				}
				else if (func.find("SetVectorUpdateCampos") != std::string::npos) {
					this->CameraNotFirst_Vec.SetCamPos(
						Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])),
						this->CameraNotFirst_Vec.GetCamVec(),
						this->CameraNotFirst_Vec.GetCamUp());
				}
				//camup
				else if (func.find("SetUpdateCamup") != std::string::npos) {
					this->camup_per = std::stof(args[0]);
					if (args.size() > 1) {
						this->CameraNotFirst.SetCamPos(
							this->CameraNotFirst.GetCamPos(),
							this->CameraNotFirst.GetCamVec(),
							Vector3DX::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])));
					}
				}
				else if (func.find("SetVectorUpdateCamup") != std::string::npos) {
					this->CameraNotFirst_Vec.SetCamPos(
						this->CameraNotFirst_Vec.GetCamPos(),
						this->CameraNotFirst_Vec.GetCamVec(),
						Vector3DX::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2])));
				}
				//fov
				else if (func.find("SetUpdateCamfov") != std::string::npos) {
					this->fov_per = std::stof(args[0]);
					this->CameraNotFirst.SetCamInfo(deg2rad(std::stof(args[1])), this->CameraNotFirst.GetCamNear(), this->CameraNotFirst.GetCamFar());
				}
				//easing
				else if (func.find("SetUpdatePer") != std::string::npos) {
					this->NotFirst_per = std::stof(args[0]);
				}
				//前のカメラアップデートを使用
				else if (func.find("SetPrevCamUpdate") != std::string::npos) {
					this->IsUsePrevBuf = true;
				}
				//アップデートカメラにランダムを指定
				else if (func.find("SetCamPosRand") != std::string::npos) {
					this->m_RandcamposSet.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
					this->m_RandcamposPer = std::stof(args[3]);
				}
				else if (func.find("SetCamVecRand") != std::string::npos) {
					this->m_RandcamvecSet.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
					this->m_RandcamvecPer = std::stof(args[3]);
				}
				else if (func.find("SetCamUpRand") != std::string::npos) {
					this->m_RandcamupSet.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
					this->m_RandcamupPer = std::stof(args[3]);
				}
				//
				else if (func.find("WhiteOut") != std::string::npos) {
					this->IsSetWhite = true;
					this->White_Per = std::stof(args[0]);
					this->White = 1.f;
				}
				else if (func.find("WhiteIn") != std::string::npos) {
					this->IsSetWhite = true;
					this->White_Per = std::stof(args[0]);
					this->White = 0.f;
				}
				//
				else if (func.find("BlackOut") != std::string::npos) {
					this->IsSetBlack = true;
					this->Black_Per = std::stof(args[0]);
					this->Black = 1.f;
				}
				else if (func.find("BlackIn") != std::string::npos) {
					this->IsSetBlack = true;
					this->Black_Per = std::stof(args[0]);
					this->Black = 0.f;
				}
				//
				CameraNotFirst_After = CameraNotFirst;
			}
			void			ResetCam(const Camera3DInfo& cam) noexcept {
				CameraNotFirst_After.SetCamPos(cam.GetCamPos(), cam.GetCamVec(), CameraNotFirst_After.GetCamUp());
			}
			void			Update(Cut_Info_First& Camera,
				Vector3DX* m_RandcamupBuf,
				Vector3DX* m_RandcamvecBuf,
				Vector3DX* m_RandcamposBuf,
				float* pBlack,
				float* pWhite,
				const ModelControl& model
			) noexcept {
				auto* DrawParts = DXDraw::Instance();
				if (this->NotFirst_per >= 0.f) {
					Camera.cam_per = this->NotFirst_per;
				}
				Easing(m_RandcamposBuf, Vector3DX::vget(GetRandf(this->m_RandcamposSet.x), GetRandf(this->m_RandcamposSet.y), GetRandf(this->m_RandcamposSet.z)), this->m_RandcamposPer, EasingType::OutExpo);
				Easing(m_RandcamvecBuf, Vector3DX::vget(GetRandf(this->m_RandcamvecSet.x), GetRandf(this->m_RandcamvecSet.y), GetRandf(this->m_RandcamvecSet.z)), this->m_RandcamvecPer, EasingType::OutExpo);
				Easing(m_RandcamupBuf, Vector3DX::vget(GetRandf(this->m_RandcamupSet.x), GetRandf(this->m_RandcamupSet.y), GetRandf(this->m_RandcamupSet.z)), this->m_RandcamupPer, EasingType::OutExpo);
				if (this->isUseNotFirst) {
					Vector3DX vec;
					if (GetForcusCenter(&vec, model)) {
						this->CameraNotFirst_After.SetCamPos(this->CameraNotFirst_After.GetCamPos(), vec, this->CameraNotFirst_After.GetCamUp());
					}
					{
						Vector3DX pos_t = Camera.Aim_camera.GetCamPos();
						Vector3DX vec_t = Camera.Aim_camera.GetCamVec();
						Vector3DX up_t = Camera.Aim_camera.GetCamUp();
						float fov_t = Camera.Aim_camera.GetCamFov();
						Easing(&pos_t, this->CameraNotFirst_After.GetCamPos() + *m_RandcamposBuf, this->campos_per, EasingType::OutExpo);
						Easing(&vec_t, this->CameraNotFirst_After.GetCamVec() + *m_RandcamvecBuf, this->camvec_per, EasingType::OutExpo);
						Easing(&up_t, this->CameraNotFirst_After.GetCamUp() + *m_RandcamupBuf, this->camup_per, EasingType::OutExpo);
						Easing(&fov_t, this->CameraNotFirst_After.GetCamFov(), this->fov_per, EasingType::OutExpo);
						Camera.Aim_camera.SetCamPos(pos_t, vec_t, up_t);
						Camera.Aim_camera.SetCamInfo(fov_t, Camera.Aim_camera.GetCamNear(), Camera.Aim_camera.GetCamFar());
					}
				}

				CameraNotFirst_After.SetCamPos(
					CameraNotFirst_After.GetCamPos() + CameraNotFirst_Vec.GetCamPos() * (1.f / DrawParts->GetFps()),
					CameraNotFirst_After.GetCamVec() + CameraNotFirst_Vec.GetCamVec() * (1.f / DrawParts->GetFps()),
					CameraNotFirst_After.GetCamUp() + CameraNotFirst_Vec.GetCamUp() * (1.f / DrawParts->GetFps())
				);
				Easing(pBlack, Black, Black_Per, EasingType::OutExpo);
				Easing(pWhite, White, White_Per, EasingType::OutExpo);
			}
			void			SetupByPrev(const Cut_Info_Update& Prev) {
				if (!IsUsePrevBuf) {
					return;
				}
				//
				auto PrevWhite_Set = IsSetWhite;
				auto PrevWhite_Per = White_Per;
				auto PrevWhitePrev = White;

				auto PrevBlack_Set = IsSetBlack;
				auto PrevBlack_Per = Black_Per;
				auto PrevBlackPrev = Black;
				//
				*this = Prev;
				//
				if (PrevWhite_Set) {
					IsSetWhite = PrevWhite_Set;
					White_Per = PrevWhite_Per;
					White = PrevWhitePrev;
				}
				if (PrevBlack_Set) {
					IsSetBlack = PrevBlack_Set;
					Black_Per = PrevBlack_Per;
					Black = PrevBlackPrev;
				}
			}
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
		bool			LoadOnce(std::string_view func_t) noexcept {
			std::string					m_Func; m_Func = func_t;
			std::vector<std::string>	m_Args;
			{
				// //を削除
				size_t sls = m_Func.find("//");
				if (sls != std::string::npos) { m_Func = m_Func.substr(0, sls); }
				//いらない要素を排除
				Sub_Func(m_Func, '{');
				Sub_Func(m_Func, '}');
				Sub_Func(m_Func, ' ');
				Sub_Func(m_Func, '\t');
				Sub_Func(m_Func, ';');
				Sub_Func(m_Func, '\"');
			}
			if (m_Func == "") { return false; }
			//()と,で囲われた部分から引数を取得
			{
				std::string tmp_func = m_Func;
				size_t left = tmp_func.find("(");
				size_t right = tmp_func.rfind(")");
				if (left != std::string::npos && right != std::string::npos) {
					tmp_func = tmp_func.substr(left + 1, right - 1 - left);
				}
				while (true) {
					size_t in_str = tmp_func.find(",");
					if (in_str == std::string::npos) {
						m_Args.emplace_back(tmp_func);
						break;
					}
					else {
						std::string arg = tmp_func.substr(0, in_str);
						tmp_func = tmp_func.substr(in_str + 1);
						m_Args.emplace_back(arg);
					}
				}
			}
			//変数登録
			if (m_Func.find("SetArg") != std::string::npos) {
				std::string After = m_Args[1];
				if (After.find(".pmx") != std::string::npos) {
					auto mv1s = After.substr(0, After.find(".pmx")) + ".mv1";
					if (std::filesystem::exists(mv1s.c_str())) { After = mv1s; }
				}
				m_Variable.Add(m_Args[0], After);
			}
			//変数変換処理
			else {
				for (auto& a1 : m_Args) {
					m_Variable.ChangeStr(&a1);
				}
			}
			//モデル描画
			if (m_Func.find("LoadModel") != std::string::npos) {
				for (int i = 0, count = std::stoi(m_Args[1]); i < count; i++) {
					m_ModelControl.Load(m_Args[0]);
				}
			}
			else if (m_Args.size() >= 2 && (std::all_of(m_Args[1].cbegin(), m_Args[1].cend(), isdigit))) {
				auto* t = m_ModelControl.Get(m_Args[0], std::stoi(m_Args[1]));
				if (t) {
					//どの距離で描画するかをセット
					if (m_Func.find("SetDrawModel") != std::string::npos) {
						t->Init(NowCut, std::stoi(m_Args[2]) - 1);
					}
					else if (m_Func.find("SetModelMode") != std::string::npos) {
						static const char* Model_Type[4] = { "SKY_TRUE","NEAR_FALSE","FAR_TRUE","SHADOW_DISACTIVE" };
						if (m_Args[2] == Model_Type[0]) {
							t->isBGModel = true;
						}
						if (m_Args[2] == Model_Type[1]) {
							t->IsNearShadow = false;
						}
						if (m_Args[2] == Model_Type[2]) {
							t->IsFarShadow = true;
						}
						if (m_Args[2] == Model_Type[3]) {
							t->ShadowDrawActive = false;
						}
					}
					else {
						t->CutDetail.back().Load(m_Func, m_Args);
					}
				}
			}
			//画像描画
			if (m_Func.find("LoadGraph") != std::string::npos) {
				for (int i = 0, count = std::stoi(m_Args[1]); i < count; i++) {
					m_GraphControl.Load(
						(float)(std::stoi(m_Args[2])), (float)(std::stoi(m_Args[3])),
						std::stof(m_Args[4]), std::stof(m_Args[5]), std::stof(m_Args[6]),
						m_Args[0]);
				}
			}
			else if (m_Args.size() >= 2 && (std::all_of(m_Args[1].cbegin(), m_Args[1].cend(), isdigit))) {
				auto* t = m_GraphControl.Get(m_Args[0], std::stoi(m_Args[1]));
				if (t) {
					if (m_Func.find("SetDrawGraph") != std::string::npos) {
						t->Init(NowCut, std::stoi(m_Args[2]) - 1);
					}
					else {
						t->CutDetail.back().Load(m_Func, m_Args);
					}
				}
			}
			//SE
			if (m_Func.find("SetDrawSE") != std::string::npos) {
				m_SEControl.GetLast()->Init(m_Args[0], NowCut);
			}
			//新規カット
			if (m_Func.find("SetCut") != std::string::npos) {
				m_CutInfo.emplace_back(Cut_Info_First((LONGLONG)(1000000.f * std::stof(m_Args[0]))));
				m_CutInfoUpdate.resize(m_CutInfoUpdate.size() + 1);
				NowCut = (int)(m_CutInfo.size()) - 1;
			}
			//Camposの指定
			else if (m_Func.find("SetCampos_Attach") != std::string::npos) {
				m_PosCamCut.AddCutSwitch(NowCut, std::stoi(m_Args[0]) - 1);
				m_PosCam.emplace_back(Vector3DX::vget(std::stof(m_Args[1]), std::stof(m_Args[2]), std::stof(m_Args[3])));
			}
			else {
				//テロップ
				m_TelopClass.LoadTelop(m_Func, m_Args);
			}
			if (m_CutInfo.size() > 0) {
				m_CutInfo.back().LoadScript(m_Func, m_Args);
				m_CutInfoUpdate.back().SetupCam(m_CutInfo.back().Aim_camera);
				m_CutInfoUpdate.back().LoadScript(m_Func, m_Args);
			}
			return true;
		}
	public:
		auto			IsEnd() const noexcept { return (m_Counter >= m_CutInfo.size()); }
	public:
		void			Load(void) noexcept {
			m_TelopClass.Init();
			m_CutInfo.clear();
			m_CutInfoUpdate.clear();
			//
			int mdata = FileRead_open("data/Cut.txt", FALSE);
			SetUseASyncLoadFlag(TRUE);
			//
			while (FileRead_eof(mdata) == 0) {
				if (ProcessMessage() != 0) {}
				if (!LoadOnce(getparams::Getstr(mdata))) { continue; }
			}
			FileRead_close(mdata);
			//
			SetUseASyncLoadFlag(FALSE);
			while (ProcessMessage() == 0 && GetASyncLoadNum() != 0) {}
			m_ModelControl.SetAfterLoad();
		}
		void			Start(size_t StartCut) noexcept {
			m_Counter = StartCut;
			m_ModelControl.Start(m_Counter);
			m_GraphControl.Start(m_Counter);
			m_PosCamCut.Start(m_Counter);
			m_RandcamupBuf = Vector3DX::zero();
			m_RandcamvecBuf = Vector3DX::zero();
			m_RandcamposBuf = Vector3DX::zero();

			m_BaseTime = GetNowHiPerformanceCount();
			m_NowTime = (m_Counter != 0) ? m_CutInfo[m_Counter - 1].GetTimeLimit() : 0;
			m_count = 0;
		}
		void			GetDeltaTime() noexcept {
			deltatime = (GetNowHiPerformanceCount() - m_BaseTime);
			m_BaseTime = GetNowHiPerformanceCount();
		}
		void			Update() noexcept {
			auto* DrawParts = DXDraw::Instance();
			if (IsEnd()) { return; }

			if (m_NowTime >= 0) {
				if (m_count == 0) {
					m_count = 1;
					DrawParts->Update_Shadow([&] { ShadowFarDraw(); }, Vector3DX::zero(), 4.f, false);
				}
				bool isFirstLoop = (m_NowTime == 0);
				bool ResetPhysics = false;
				if (m_NowTime > m_CutInfo[m_Counter].GetTimeLimit()) {
					ResetPhysics = m_CutInfo[m_Counter].GetIsResetPhysics();
					++m_Counter;
					isFirstLoop = true;
				}

				if (IsEnd()) { return; }
				//カットの処理
				{
					m_PosCamCut.Update(m_Counter);
					m_ModelControl.FirstUpdate(m_Counter, isFirstLoop, ResetPhysics);
					m_GraphControl.FirstUpdate(m_Counter, isFirstLoop);
					m_SEControl.Update(m_Counter, isFirstLoop);
					auto& CutInfo = m_CutInfo[m_Counter];
					auto& CutInfoUpdate = m_CutInfoUpdate[m_Counter];
					if (isFirstLoop) {
						if (m_Counter > 0) {
							CutInfo.SetPrev(m_CutInfo[m_Counter - 1]);
						}
						CutInfoUpdate.SetupByPrev(m_CutInfoUpdate[m_Counter - 1]);
						CutInfo.SetUpFog();
						//
						Vector3DX vec;
						if (CutInfo.GetForcusCenter(&vec, m_ModelControl)) {
							CutInfo.Aim_camera.SetCamPos(CutInfo.Aim_camera.GetCamPos(), vec, CutInfo.Aim_camera.GetCamUp());
						}
						if (CutInfo.isResetRandCampos) { m_RandcamposBuf = Vector3DX::zero(); }
						if (CutInfo.isResetRandCamvec) { m_RandcamvecBuf = Vector3DX::zero(); }
						if (CutInfo.isResetRandCamup) { m_RandcamupBuf = Vector3DX::zero(); }

						if (m_PosCamCut.GetisActive()) {
							CutInfo.Aim_camera.SetCamPos(CutInfo.Aim_camera.GetCamVec() + m_PosCam[m_PosCamCut.GetNowCut()], CutInfo.Aim_camera.GetCamVec(), CutInfo.Aim_camera.GetCamUp());
						}
						CutInfoUpdate.ResetCam(CutInfo.Aim_camera);
					}
					else {
						CutInfoUpdate.Update(CutInfo, &m_RandcamupBuf, &m_RandcamvecBuf, &m_RandcamposBuf, &Black_Buf, &White_Buf, m_ModelControl);
						//
						if (m_PosCamCut.GetisActive()) {
							CutInfo.Aim_camera.SetCamPos(CutInfo.Aim_camera.GetCamVec() + m_PosCam[m_PosCamCut.GetNowCut()], CutInfo.Aim_camera.GetCamVec(), CutInfo.Aim_camera.GetCamUp());
						}
					}
					CutInfo.UpdateCam(&DrawParts->SetMainCamera());
				}
				//
				auto far_t = DrawParts->SetMainCamera().GetCamFar();
				PostPassEffect::Instance()->Set_DoFNearFar(1.f * Scale3DRate, far_t / 2, 0.5f * Scale3DRate, far_t);
			}
			//経過時間測定
			m_NowTime += (LONGLONG)((float)deltatime);
		}
		void			Dispose_Load(void) noexcept {
			m_CutInfo.clear();
			m_CutInfoUpdate.clear();
		}

		void			BGDraw(void) const noexcept {
			if (IsEnd()) { return; }
			auto* DrawParts = DXDraw::Instance();
			DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(0, 0, 0), TRUE);
			m_ModelControl.Draw_Far();
		}
		void			ShadowFarDraw(void) const noexcept {
			if (IsEnd()) { return; }
			SetDrawAlphaTest(DX_CMP_GREATER, 128);
			//m_ModelControl.Draw(false, true, true);
			SetDrawAlphaTest(-1, 0);
		}
		void			ShadowDraw(void) const noexcept {
			if (IsEnd()) { return; }
			m_ModelControl.Draw(false, false, true, TRUE);
			m_ModelControl.Draw(true, false, true, FALSE);
		}
		void			SetShadowDraw(void) const noexcept {
			if (IsEnd()) { return; }
			m_ModelControl.Draw(false, false, true, TRUE);
			m_ModelControl.Draw(true, false, true, FALSE);
		}
		void			MainDraw(void) const noexcept {
			if (IsEnd()) { return; }
			auto* DrawParts = DXDraw::Instance();
			auto camfar = GetCameraFar();
			if (DrawParts->GetMainCamera().GetCamFar() - 1.f < camfar && camfar < DrawParts->GetMainCamera().GetCamFar() + 1.f) {
				m_ModelControl.Draw(false, false, false, FALSE);
			}
			else {
				m_ModelControl.Draw(false, false, false, TRUE);
			}
		}
		void			UIDraw() const noexcept {
			if (IsEnd()) { return; }
			auto* DrawParts = DXDraw::Instance();

			if (m_NowTime > 0) {
				m_TelopClass.Draw(m_NowTime);
			}
			if (Black_Buf != 0.f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f * Black_Buf));
				DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(0, 0, 0), TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			if (White_Buf != 0.f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f * White_Buf));
				DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(255, 255, 255), TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}

			SetDrawMode(DX_DRAWMODE_BILINEAR);
			m_GraphControl.Draw();
			SetDrawMode(DX_DRAWMODE_NEAREST);

		}
	};
};
