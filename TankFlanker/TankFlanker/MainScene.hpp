#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
			class LoadScriptClass {
			private:
				class VARIABLE {
				public:
					std::string Base;
					std::string After;
				};
			private:
				std::vector<VARIABLE> m_Variable;
				std::string m_Func;
				std::vector<std::string> m_Args;
			private:
				static void Sub_Func(std::string& func_t, const char& in_str) noexcept {
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
			public:
				//Getter
				const auto& Getfunc() const noexcept { return m_Func; }
				const auto& Getargs() const noexcept { return m_Args; }
				const auto* GetArgFromPath(std::string_view Path) const noexcept {
					const VARIABLE* sel = nullptr;
					for (const auto& a : m_Variable) {
						if (a.After == Path) {
							sel = &a;
							break;
						}
					}
					return sel;
				}
				//スクリプト読み込み処理
				void LoadScript(std::string_view func_t) noexcept {
					m_Args.clear();
					m_Func = func_t;
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
					//()と,で囲われた部分から引数を取得
					if (m_Func != "") {
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
				}
				//処理
				void SetArgs(void) noexcept {
					//変数登録
					{
						if (m_Func.find("SetArg") != std::string::npos) {
							m_Variable.resize(m_Variable.size() + 1);
							m_Variable.back().Base = m_Args[0];
							m_Variable.back().After = m_Args[1];
						}
						//変数変換処理
						else {
							for (auto& a1 : m_Args) {
								for (auto& a2 : m_Variable) {
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
					void Set(int xp, int yp, int Fontsize, std::string_view mag, LONGLONG StartF, LONGLONG ContiF, int m_LMR) noexcept {
						this->xpos = xp;
						this->ypos = yp;
						this->size = Fontsize;
						this->str = mag;
						this->START_TIME = StartF;
						this->END_TIME = StartF + ContiF;;
						this->LMR = m_LMR;
					}
					void Draw(LONGLONG nowTimeWait) const noexcept {
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
			private:
				std::vector<Cut_tex> Texts;
				LONGLONG StartF = 0;
				LONGLONG ContiF = 0;
			public:
				void Init(void) noexcept {
					StartF = 0;
					ContiF = 0;
				}
				void LoadTelop(const std::string &func, const std::vector<std::string>& args) noexcept {
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
				void Draw(LONGLONG nowTimeWait) const noexcept {
					for (auto& t : Texts) {
						t.Draw(nowTimeWait);
					}
				}
			};
		private:
			//
			class CutInfoClass {
				class On_Off {
				public:
					int On = 0;
					int Off = 0;
					void SetSwitch(int on, int off) noexcept {
						On = on;
						Off = off;
					}

					bool IsIn(size_t value) const noexcept {
						return On <= value && value <= Off;
					}
				};
			private:
				bool a_switch = false;
			public:
				std::vector<On_Off> Switch;
				int nowcut = 0;
				bool isFirstCut = false;
			public:
				const auto& GetSwitch() const noexcept { return a_switch; }

				void Init(int startFrame, int ofset) noexcept {
					this->Switch.resize(this->Switch.size() + 1);
					this->Switch.back().SetSwitch(startFrame, startFrame + ofset);
				}
				void Start(size_t Counter) noexcept {
					this->a_switch = false;
					this->isFirstCut = false;

					this->nowcut = 0;
					while (true) {
						if (this->Switch.size() > this->nowcut) {
							auto& inf_b = this->Switch[this->nowcut];
							if (Counter > inf_b.On) {
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
					if (0 <= this->nowcut - 1 && this->nowcut - 1 < this->Switch.size()) {
						auto& inf_b = this->Switch[this->nowcut - 1];
						isFirstCut = (Counter == inf_b.On);
						if (inf_b.IsIn(Counter)) {
							this->nowcut--;
							this->a_switch = true;
						}
					}
				}
				bool Update_(size_t Counter) noexcept {
					if (this->Switch.size() > this->nowcut) {
						auto& inf_b = this->Switch[this->nowcut];
						isFirstCut = (Counter == inf_b.On);
						if (isFirstCut) {
							this->a_switch = true;
						}
						if (this->a_switch) {
							if (Counter > inf_b.Off) {
								this->a_switch = false;
								this->nowcut++;
								return true;
							}
						}
					}
					return false;
				}
				void Insert(int ID, int Start, int End) noexcept {
					Switch.insert(Switch.begin() + ID, On_Off());
					Switch[ID].SetSwitch(Start, End);
				}
			};
			//詳細
			class CutinfoDetail {
			public:
				CutInfoClass Blur;
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
				float OpacityRate_Dist = -1.f;
				float OpacityRate_Per = -1.f;
			};
			class CutAttachDetail {
			public:
				VECTOR_ref poscam;
			};
			//モデルコントロール
			class ModelControl {
			public:
				class Model {
				public:
					bool isBGModel = false;
					bool IsNearShadow = true;
					bool IsFarShadow = false;
					bool isBase = true;
					std::string Path;
					size_t BaseID = 0;

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
					CutInfoClass Cutinfo;
					std::vector<CutinfoDetail> CutDetail;//カットの詳細
					float OpacityRate = 1.f;
					float OpacityRate_Dist = 1.f;
					float OpacityRate_Per = 1.f;
					int Anim_Sel = 0;

					bool isFarPhysics = false;
					bool isOutFrustum = false;
					bool isBigModel = false;
					int DrawCount;

					MATRIX_ref mat_p;
					//matrix用情報
					VECTOR_ref pos_p;

					float Yrad1_p;
					float Zrad1_p;
					float Yrad2_p;

					VECTOR_ref Aim_p;
					VECTOR_ref OLDPos_p;
					VECTOR_ref OLDVec_p;
					float dist;
					float OLDDist;
					float pos_z_p;
					float Yradaddbuf_p;
					float Yradadd_p;
					float Zrad1buf_p;
					float animspd;
					float xposition = 0.f;

					VECTOR_ref footEffPos;
					bool footflag = true;
					bool footok = false;
					float foottime = 0.f;
					int footLR{ 0 };

					std::array<int, 2> GuideNum;

					bool canUpdate = true;

					bool Lookok = false;
					bool LookLR = false;
					float Looktime = 0.f;

					bool Turbook = false;
					float Turbotime = 0.f;
					float TurboSpeed = 0.f;

					std::string NearID = "";
					VECTOR_ref Nearest;

					int prevID = -1;
					float AnimChange = 0.f;

					Model(void) noexcept {
						isDraw = false;
						isEndLoad = false;
						isFarPhysics = false;
						isOutFrustum = false;
						isBigModel = true;

						animspd = 1.f;
					}

					void Init(int startFrame, int ofset) noexcept {
						this->CutDetail.resize(this->CutDetail.size() + 1);
						this->Cutinfo.Switch.resize(this->Cutinfo.Switch.size() + 1);
						this->Cutinfo.Switch.back().SetSwitch(startFrame, startFrame + ofset);
					}

					void AddFrame(std::string_view FrameName) noexcept {
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
					const auto GetFrame(std::string_view FrameName) const noexcept {
						for (auto& F : FrameNum) {
							if (F.second == FrameName) {
								return obj.frame(F.first);
							}
						}
						return VECTOR_ref::zero();
					}

					static void Sel_AnimNum(MV1&model, int sel, float pers) noexcept {
						for (auto& anim_t : model.get_anime()) {
							model.get_anime(&anim_t - &model.get_anime().front()).per = (&anim_t - &model.get_anime().front() == sel) ? pers : (1.f - pers);
						}
					}
					void ChangeAnim(int ID, bool isloop, float speed, bool isfastchange) noexcept {
						if (isfastchange) {
							AnimChange = 1.f;
						}
						if (prevID != ID) {
							AnimChange = 1.f;
							this->obj.get_anime(ID).time = this->obj.get_anime(prevID).time;
						}
						prevID = ID;
						Sel_AnimNum(this->obj, ID, AnimChange);
						AnimChange = std::clamp(AnimChange + 1.f / FPS, 0.f, 1.f);
						if (ID < this->obj.get_anime().size()) {
							this->obj.get_anime(ID).Update(isloop, speed);
						}
					}
					void Update(bool isUpdateAnim) noexcept {
						this->DrawCount = 0;
						if (this->OpacityRate_Per < 1.f) {
							easing_set_SetSpeed(&this->OpacityRate, this->OpacityRate_Dist, this->OpacityRate_Per);
						}
						MV1SetOpacityRate(this->obj.get(), this->OpacityRate);
						if (this->isDraw && isUpdateAnim) {
							this->obj.work_anime();
						}
					}
					void Draw(int isfar_ = -1) noexcept {
						if (this->isDraw && this->OpacityRate > 0.f) {
							if (!this->isBigModel) {
								if (isfar_ == TRUE) {
									if (this->isOutFrustum) {
										this->DrawCount++;
										this->obj.DrawModel();
									}
								}
								else if (isfar_ == FALSE) {
									if (!this->isOutFrustum) {
										this->DrawCount++;
										this->obj.DrawModel();
									}
								}
								else {
									this->DrawCount++;
									this->obj.DrawModel();
								}
							}
							else {
								this->DrawCount++;
								this->obj.DrawModel();
							}
						}
					}
					void SetPhysics(bool isReset, float x) noexcept {
						if (this->isDraw) {
							if (isReset) {
								this->obj.PhysicsResetState();
							}
							else {
								this->obj.PhysicsCalculation(1000.0f / (FPS * std::max(GameSpeed, 0.1f) * (120.f / 60.f)) * this->PhysicsSpeed * x);
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

				ModelControl(void) noexcept {
					model.resize(64);
					Max = 0;
				}
				~ModelControl(void) noexcept {

				}
				void Load(std::string_view Path) noexcept {
					MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
					for (int i = 0; i < 256; i++) {
						MV1SetLoadCalcPhysicsWorldGravity(i, VECTOR_ref::vget(0, 0, 1.f).get());
					}
					MV1SetLoadModelPhysicsCalcPrecision(1);

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
					MV1::Load(model[Max].Path, &(model[Max].obj), DX_LOADMODEL_PHYSICS_LOADCALC);/*DX_LOADMODEL_PHYSICS_REALTIME*/
					Max++;
				}
				Model* Get(std::string_view Path, size_t Sel = 0) noexcept {
					for (size_t i = 0; i < Max; i++) {
						if (model[i].Path == Path && model[i].BaseID == Sel) {
							return &(model[i]);
						}
					}
					return nullptr;
				}
				void Start(size_t Counter) noexcept {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.Cutinfo.Start(Counter);
						m.isDraw = m.Cutinfo.GetSwitch();
					}
				}
				void FirstUpdate(int Counter, bool isFirstLoop) noexcept {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						while (true) {
							bool tt = m.Cutinfo.Update_(Counter);
							m.isDraw = m.Cutinfo.GetSwitch();
							if (m.isDraw && (m.Cutinfo.nowcut < m.CutDetail.size())) {
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
									if (inf.OpacityRate_Dist >= 0.f) {
										m.OpacityRate_Dist = inf.OpacityRate_Dist;
									}
									if (inf.OpacityRate_Per >= 0.f) {
										m.OpacityRate_Per = inf.OpacityRate_Per;
									}
								}
								//アニメーション動作
								if (inf.animsel >= 0) {
									m.ChangeAnim(inf.animsel, inf.isloop, inf.animspeed * m.animspd * GameSpeed, isFirstLoop && m.Cutinfo.isFirstCut);
								}
							}
							if (tt) {
								continue;
							}
							break;
						}
					}

					int sp = 0;
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.canUpdate = true;
						if (!m.isOutFrustum && m.isDraw) {
							m.canUpdate = (sp % 2 == P_cnt);
							//sp++;
						}
					}
					//++P_cnt %= 2;
				}
				void SetPhysics(bool reset_p) noexcept {
					if (reset_p) {
						for (size_t i = 0; i < Max; i++) {
							model[i].SetPhysics(true, 1.f);
						}
					}
					else {
						for (size_t i = 0; i < Max; i++) {
							auto& m = model[i];
							if (m.isOutFrustum) {
								if (!m.isFarPhysics) {
									m.SetPhysics(true, 1.f);
									m.isFarPhysics = true;
								}
							}
							else {
								m.isFarPhysics = false;
								if (m.isDraw) {
									//m.SetPhysics(false, 1.f);
									//*
									if (m.canUpdate) {
										m.SetPhysics(false, 2.f);
									}
									//*/
								}
							}
						}
						//
					}
				}

				void Set(void) noexcept {
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
							m.AddFrame("右つま先");
							m.AddFrame("左つま先");
							MV1::SetAnime(&(m.obj), m.obj);
						}
					}
				}
				void Update(void) noexcept {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						if (m.isDraw) {
							m.Update(m.canUpdate);/**/
						}
					}
				}
				void Draw_Far(void) noexcept {
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
				void Draw(bool innearshadow, bool infarshadow, int isCheckFar = -1) noexcept {
					if (infarshadow) {
						for (size_t i = 0; i < Max; i++) {
							auto& m = model[i];
							if (!m.isBGModel && m.IsFarShadow) {
								m.Draw(isCheckFar);
							}
						}
					}
					else if (innearshadow) {
						for (size_t i = 0; i < Max; i++) {
							auto& m = model[i];
							if (!m.isBGModel && m.IsNearShadow) {
								m.Draw(isCheckFar);
							}
						}
					}
					else {
						for (size_t i = 0; i < Max; i++) {
							auto& m = model[i];
							if (!m.isBGModel) {
								m.Draw(isCheckFar);
							}
						}
					}
				}
				void CheckInCamera(float far_) noexcept {
					auto camfar = GetCameraFar();
					//カメラのfarが適正範囲の設定の場合
					if (far_ - 1.f < camfar&& camfar < far_ + 1.f) {
						for (size_t i = 0; i < Max; i++) {
							auto& m = model[i];
							m.isOutFrustum = false;
							m.isBigModel = false;
							auto center = m.GetFrame("上半身");
							if (center != VECTOR_ref::zero()) {
								//center += (center - GetCameraPosition()).Norm()*-7.f;

								if (CheckCameraViewClip(center.get()) == TRUE) {
									m.isOutFrustum = true;
								}
							}
							else {
								m.isBigModel = true;
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
							void Set(float m_p, float m_base, float m_per) noexcept {
								Ans = m_p;
								Base = m_base;
								Per = m_per;
							}
							void Set(float m_base, float m_per) noexcept {
								Base = m_base;
								Per = m_per;
							}
							void UpdateRand(void) noexcept {
								easing_set_SetSpeed(&this->Ans, GetRandf(this->Base), this->Per);
							}
						};
					public:
						float Ans = 0;
						float Base = 0;
						float Per = 1.f;
						infos_rand Rand_;
						void Set(float m_p, float m_base, float m_per) noexcept {
							Ans = m_p;
							Base = m_base;
							Per = m_per;
							Rand_.Set(0, 0, 1.f);
						}
						void Set(float m_base, float m_per) noexcept {
							Base = m_base;
							Per = m_per;
						}
						/*
						void Set(float m_p, float m_base, float m_per, float m_rand, float m_randbase, float m_randper) noexcept {
							Ans = m_p;
							Base = m_base;
							Per = m_per;
							Rand_.Set(m_rand, m_randbase, m_randper);
						}
						*/
						void Update(void) noexcept {
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
					CutInfoClass Cutinfo;
					std::vector<CutinfoDetail> CutDetail;//オンにするカット
					std::string Path;

					void Init(int startFrame, int ofset) noexcept {
						this->CutDetail.resize(this->CutDetail.size() + 1);
						this->Cutinfo.Switch.resize(this->Cutinfo.Switch.size() + 1);
						this->Cutinfo.Switch.back().SetSwitch(startFrame, startFrame + ofset);
					}

					void SetBright(int b_r, int b_g, int b_b) noexcept {
						Bright_R = b_r;
						Bright_G = b_g;
						Bright_B = b_b;
					}
					void Set(float xp, float yp, float rd, float al, float sc, std::string_view Path_t) noexcept {
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
					void Set_Base(float xp, float xper, float yp, float yper, float rd, float scl, float rdper, float sclper, float alp, float alpper) noexcept {
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
					void Set_Rand(float xp, float xper, float yp, float yper, float rd, float rdper, float scl, float sclper) noexcept {
						this->X.Rand_.Set(xp, xper);
						this->Y.Rand_.Set(yp, yper);
						this->Rad.Rand_.Set(deg2rad(rd), rdper);
						this->Scale.Rand_.Set(scl, sclper);
					}
					void Update(void) noexcept {
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
					void Draw_Common(const GraphHandle& m_handle, float scale_) const noexcept {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*this->Alpha.Ans));
						m_handle.DrawRotaGraph((int)(this->X.Ans), (int)(this->Y.Ans), scale_*this->Scale.Ans, this->Rad.Ans, true);
					}
					void Draw(float scale_) const noexcept {
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
				GraphControl(void) noexcept {
					model.resize(64);
					Max = 0;
				}
				void Load(float xp, float yp, float rd, float al, float sc, std::string_view Path) noexcept {
					model[Max].Path = Path;
					model[Max].Set(xp, yp, rd, al, sc, Path);
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
				void Start(size_t Counter) noexcept {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.Cutinfo.Start(Counter);
						m.isDraw = false;
						for (auto& B : m.CutDetail) {
							B.Blur.Start(Counter);
						}
					}
				}
				void FirstUpdate(int Counter, bool isFirstLoop) noexcept {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						while (true) {
							bool tt = m.Cutinfo.Update_(Counter);
							m.isDraw = m.Cutinfo.GetSwitch();
							if (m.isDraw && (m.Cutinfo.nowcut < m.CutDetail.size())) {
								auto& inf = m.CutDetail[m.Cutinfo.nowcut];
								//ブラー操作
								inf.Blur.Update_(Counter);
								m.isblur = inf.Blur.GetSwitch();

								if (inf.OpacityRate_Dist >= 0.f) {
									if (inf.OpacityRate_Per == 0) {
										if (isFirstLoop && m.Cutinfo.isFirstCut) {
											//最初のアニメーション動作
											m.Alpha.Set(inf.OpacityRate_Dist, inf.OpacityRate_Per);
										}
									}
									//0以外はいーじんぐ
									else  if (inf.OpacityRate_Per >= 0.f) {
										m.Alpha.Set(inf.OpacityRate_Dist, inf.OpacityRate_Per);

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
				void Set(void) noexcept {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.isDraw = false;
					}
				}
				void Update(void) noexcept {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.Update();
					}
				}
				void Draw(int Disp_y) const noexcept {
					for (size_t i = 0; i < Max; i++) {
						auto& m = model[i];
						m.Draw((float)Disp_y / m.ysize);
					}
				}
			};
			//
			class ForcusControl {
				bool Use{ false };
				std::string Path;
				size_t ID;
				std::string Frame;
				VECTOR_ref Add;
			public:
				void SetUse(bool value) noexcept { Use = value; }
				const auto& GetIsUse() const noexcept { return Use; }
				void Init(void) noexcept {
					Use = false;
					Path = "";
					ID = 0;
					Frame = "";
					Add.clear();
				}
				void Set(std::string_view path, size_t id, std::string_view frame, const VECTOR_ref& add) noexcept {
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
					else if (frame == "FAR_TRUE") {
						this->Frame = "右ひざ";
					}
					this->Add = add;
				}
				const auto GetForce(ModelControl& models) const noexcept {
					return models.Get(this->Path, this->ID)->GetFrame(this->Frame) + this->Add;
				}
			};

			class Cut_Info_First {
			public:
				LONGLONG TimeLimit{ 0 };

				bool UsePrevAim{ false };
				bool IsResetPhysics{ false };
				cam_info Aim_camera;
				float cam_per{ 0.f };
				bool isResetRandCampos{ false };
				bool isResetRandCamvec{ false };
				bool isResetRandCamup{ false };

				int bright[3]{ -1,-1,-1 };

				int fog[3]{ -1,-1,-1 };
				float fog_range[2]{ -1.f,-1.f };
			public:
				ForcusControl Forcus;
			public:
				//Getter
				const auto& GetTimeLimit() const noexcept { return TimeLimit; }
				void SetTimeLimit(LONGLONG value) noexcept { TimeLimit = value; }
			public:
				Cut_Info_First(void) noexcept {
					Aim_camera.campos = VECTOR_ref::vget(0, 10, -30);
					Aim_camera.camvec = VECTOR_ref::vget(0, 10, 0);
					Aim_camera.camup = VECTOR_ref::up();
					Aim_camera.set_cam_info(deg2rad(15), 1.f, 300.f);
					cam_per = 0.95f;
					IsResetPhysics = false;
				}
				~Cut_Info_First(void) noexcept {
				}
				//
				void SetPrev(const Cut_Info_First& tgt) noexcept {
					if (this->UsePrevAim) {
						this->Aim_camera = tgt.Aim_camera;
						this->cam_per = tgt.cam_per;
					}
				}
				//
				bool LoadScript(const std::string &func, const std::vector<std::string>& args) noexcept {
					//Campos
					if (func.find("SetCampos_NoneRad") != std::string::npos) {
						this->Aim_camera.campos = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
						return true;
					}
					//Camvec
					else if (func.find("SetCamvec") != std::string::npos) {
						this->Aim_camera.camvec = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
						return true;
					}
					else if (func.find("SetCamForcus") != std::string::npos) {
						this->Forcus.Set(args[0], std::stol(args[1]), args[2], VECTOR_ref::vget(std::stof(args[3]), std::stof(args[4]), std::stof(args[5])));
					}
					//CamUp
					else if (func.find("SetCamup") != std::string::npos) {
						this->Aim_camera.camup = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
						return true;
					}
					//Else
					else if (func.find("SetCamInfo") != std::string::npos) {
						this->Aim_camera.fov = deg2rad(std::stof(args[0]));
						this->Aim_camera.near_ = std::stof(args[1]);
						this->Aim_camera.far_ = std::stof(args[2]);
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
					else if (func.find("SetBright") != std::string::npos) {
						this->bright[0] = std::stoi(args[0]);
						this->bright[1] = std::stoi(args[1]);
						this->bright[2] = std::stoi(args[2]);
					}
					else if (func.find("SetFog") != std::string::npos) {
						if (args.size() == 5) {
							this->fog[0] = std::stoi(args[0]);
							this->fog[1] = std::stoi(args[1]);
							this->fog[2] = std::stoi(args[2]);
							this->fog_range[1] = std::stof(args[3]);
							this->fog_range[2] = std::stof(args[4]);
						}
						else {
							this->fog[0] = -2;
						}
					}
					return false;
				}
				//
			};
			class Cut_Info_Update {
			private:
				bool isUseNotFirst{ false };
				float NotFirst_per = -1.f;
				float fov_per{ 0.f };
				float m_RandcamupPer;
				VECTOR_ref m_RandcamupSet;
				float m_RandcamvecPer;
				VECTOR_ref m_RandcamvecSet;
				float m_RandcamposPer;
				VECTOR_ref m_RandcamposSet;
			public:
				ForcusControl Forcus;
				bool IsUsePrevBuf{ false };

				float campos_per{ 0.f };
				float camvec_per{ 0.f };
				float camup_per{ 0.f };

				cam_info CameraNotFirst;
				cam_info CameraNotFirst_Vec;

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
					m_RandcamupSet.clear();
					m_RandcamvecPer = 0.f;
					m_RandcamvecSet.clear();
					m_RandcamposPer = 0.f;
					m_RandcamposSet.clear();

					Forcus.Init();
				}
				~Cut_Info_Update(void) noexcept {
				}

				void SetForce(float camvecPer, std::string_view ModelPath, int ModelID, std::string_view Frame, const VECTOR_ref& Add) noexcept {
					this->camvec_per = camvecPer;
					this->Forcus.Set(ModelPath, ModelID, Frame, Add);
				}
				void LoadScript(const std::string &func, const std::vector<std::string>& args) noexcept {
					//カメラのアップデート
					if (func.find("SetUpdateEnable") != std::string::npos) {
						this->isUseNotFirst = true;
					}
					//camvec
					else if (func.find("SetUpdateCamvec") != std::string::npos) {
						this->camvec_per = std::stof(args[0]);
						if (args.size() > 1) {
							this->CameraNotFirst.camvec.Set(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
						}
					}
					else if (func.find("SetUpdateCamForcus") != std::string::npos) {
						this->SetForce(std::stof(args[0]), args[1], std::stoi(args[2]), args[3], VECTOR_ref::vget(std::stof(args[4]), std::stof(args[5]), std::stof(args[6])));
					}
					else if (func.find("SetVectorUpdateCamvec") != std::string::npos) {
						this->CameraNotFirst_Vec.camvec.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
					}
					//campos
					else if (func.find("SetUpdateCampos") != std::string::npos) {
						this->campos_per = std::stof(args[0]);
						if (args.size() > 1) {
							this->CameraNotFirst.campos.Set(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
						}
					}
					else if (func.find("SetVectorUpdateCampos") != std::string::npos) {
						this->CameraNotFirst_Vec.campos.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
					}
					//camup
					else if (func.find("SetUpdateCamup") != std::string::npos) {
						this->camup_per = std::stof(args[0]);
						if (args.size() > 1) {
							this->CameraNotFirst.camup.Set(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
						}
					}
					else if (func.find("SetVectorUpdateCamup") != std::string::npos) {
						this->CameraNotFirst_Vec.camup.Set(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
					}
					//fov
					else if (func.find("SetUpdateCamfov") != std::string::npos) {
						this->fov_per = std::stof(args[0]);
						this->CameraNotFirst.fov = deg2rad(std::stof(args[1]));
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
				}
				void Update(Cut_Info_First& Camera, ModelControl& models,
					VECTOR_ref& m_RandcamupBuf,
					VECTOR_ref& m_RandcamvecBuf,
					VECTOR_ref& m_RandcamposBuf
				) noexcept {
					if (this->NotFirst_per >= 0.f) {
						Camera.cam_per = this->NotFirst_per;
					}
					easing_set_SetSpeed(&m_RandcamposBuf, VECTOR_ref::vget(GetRandf(this->m_RandcamposSet.x()), GetRandf(this->m_RandcamposSet.y()), GetRandf(this->m_RandcamposSet.z())), this->m_RandcamposPer);
					easing_set_SetSpeed(&m_RandcamvecBuf, VECTOR_ref::vget(GetRandf(this->m_RandcamvecSet.x()), GetRandf(this->m_RandcamvecSet.y()), GetRandf(this->m_RandcamvecSet.z())), this->m_RandcamvecPer);
					easing_set_SetSpeed(&m_RandcamupBuf, VECTOR_ref::vget(GetRandf(this->m_RandcamupSet.x()), GetRandf(this->m_RandcamupSet.y()), GetRandf(this->m_RandcamupSet.z())), this->m_RandcamupPer);
					if (this->isUseNotFirst) {
						if (this->Forcus.GetIsUse()) {
							this->CameraNotFirst.camvec = this->Forcus.GetForce(models);
						}
						easing_set_SetSpeed(&Camera.Aim_camera.campos, this->CameraNotFirst.campos + m_RandcamposBuf, this->campos_per);
						easing_set_SetSpeed(&Camera.Aim_camera.camvec, this->CameraNotFirst.camvec + m_RandcamvecBuf, this->camvec_per);
						easing_set_SetSpeed(&Camera.Aim_camera.camup, this->CameraNotFirst.camup + m_RandcamupBuf, this->camup_per);
						easing_set_SetSpeed(&Camera.Aim_camera.fov, this->CameraNotFirst.fov, this->fov_per);
					}
				}
			};
			//インスタシング
			class Model_Instance {
			public:
				int hitss = 0;					/*hitsの数*/
				std::vector<VERTEX3D> hitsver;	/*hits*/
				std::vector<DWORD> hitsind;	    /*hits*/
				int VerBuf = -1, IndexBuf = -1;	/*hits*/
				MV1 hits;						/*hitsモデル*/
				GraphHandle hits_pic;			/*画像ハンドル*/
				int IndexNum = -1, VerNum = -1;	/*hits*/
				int vnum = -1, pnum = -1;		/*hits*/
				MV1_REF_POLYGONLIST RefMesh{};	/*hits*/
				//初期化
				void Init(std::string pngpath, std::string mv1path) noexcept {
					SetUseASyncLoadFlag(FALSE);
					this->hits_pic = GraphHandle::Load(pngpath);		 /*grass*/
					MV1::Load(mv1path, &this->hits, false);	//弾痕
					Init_one();
				}
				void Init_one(void) noexcept {
					MV1RefreshReferenceMesh(this->hits.get(), -1, TRUE);			/*参照用メッシュの更新*/
					this->RefMesh = MV1GetReferenceMesh(this->hits.get(), -1, TRUE);	/*参照用メッシュの取得*/
				}
				//毎回のリセット
				void Clear(void) noexcept {
					this->hitss = 0;
					this->vnum = 0;
					this->pnum = 0;
					this->hitsver.clear();								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
					this->hitsind.clear();								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
					this->hitsver.reserve(2000);							/*頂点データとインデックスデータを格納するメモリ領域の確保*/
					this->hitsind.reserve(2000);							/*頂点データとインデックスデータを格納するメモリ領域の確保*/
				}

				void Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
					this->hitss++;
					Set_start();
					{
						float asize = 200.f * caliber;
						const auto& y_vec = Normal;
						auto z_vec = y_vec.cross(Zvec).Norm();
						auto scale = VECTOR_ref::vget(asize / std::abs(y_vec.dot(Zvec)), asize, asize);
						auto pos = Position + y_vec * 0.02f;
						MATRIX_ref mat = MATRIX_ref::GetScale(scale) * MATRIX_ref::Axis1_YZ(y_vec, z_vec);

						this->hits.SetMatrix(mat * MATRIX_ref::Mtrans(pos));
					}
					Set_one();
				}
				void Set_start(void) noexcept {
					this->IndexNum = this->RefMesh.PolygonNum * 3 * this->hitss;				/*インデックスの数を取得*/
					this->VerNum = this->RefMesh.VertexNum * this->hitss;						/*頂点の数を取得*/
					this->hitsver.resize(this->VerNum);									/*頂点データとインデックスデータを格納するメモリ領域の確保*/
					this->hitsind.resize(this->IndexNum);								/*頂点データとインデックスデータを格納するメモリ領域の確保*/
				}
				void Set_one(void) noexcept {
					Init_one();
					for (size_t j = 0; j < size_t(this->RefMesh.VertexNum); ++j) {
						auto& g = this->hitsver[j + this->vnum];
						g.pos = this->RefMesh.Vertexs[j].Position;
						g.norm = this->RefMesh.Vertexs[j].Normal;
						g.dif = this->RefMesh.Vertexs[j].DiffuseColor;
						g.spc = this->RefMesh.Vertexs[j].SpecularColor;
						g.u = this->RefMesh.Vertexs[j].TexCoord[0].u;
						g.v = this->RefMesh.Vertexs[j].TexCoord[0].v;
						g.su = this->RefMesh.Vertexs[j].TexCoord[1].u;
						g.sv = this->RefMesh.Vertexs[j].TexCoord[1].v;
					}
					for (size_t j = 0; j < size_t(this->RefMesh.PolygonNum); ++j) {
						for (size_t k = 0; k < std::size(this->RefMesh.Polygons[j].VIndex); ++k)
							this->hitsind[j * 3 + k + this->pnum] = WORD(this->RefMesh.Polygons[j].VIndex[k] + this->vnum);
					}
					this->vnum += this->RefMesh.VertexNum;
					this->pnum += this->RefMesh.PolygonNum * 3;
				}

				void Update(void) noexcept {
					this->VerBuf = CreateVertexBuffer(this->VerNum, DX_VERTEX_TYPE_NORMAL_3D);
					this->IndexBuf = CreateIndexBuffer(this->IndexNum, DX_INDEX_TYPE_32BIT);
					SetVertexBufferData(0, this->hitsver.data(), this->VerNum, this->VerBuf);
					SetIndexBufferData(0, this->hitsind.data(), this->IndexNum, this->IndexBuf);
				}
				void Draw(void) noexcept {
					DrawPolygonIndexed3D_UseVertexBuffer(this->VerBuf, this->IndexBuf, this->hits_pic.get(), TRUE);
				}
			};
			//kusa
			class Grass {
				class grass_t {
				private:
					//std::shared_ptr<Map> MAPPTs{ nullptr };
				public:
					bool canlook = true;
					Model_Instance inst;
				public:

					void Init(int total, int sel) {
						switch (sel) {
						case 0:
							this->inst.Init("data/model/grass/grass.png", "data/model/grass/model.mv1");
							break;
						case 1:
							this->inst.Init("data/model/grass/grass2.png", "data/model/grass/model.mv1");
							break;
						case 2:
							this->inst.Init("data/model/grass/grass3.png", "data/model/grass/model.mv1");
							break;
						}
						this->inst.Clear();
						this->inst.hitss = total;

						this->inst.Set_start();
					}

					void Set_one(void) noexcept {
						this->inst.Set_one();
					}
					void put(void) noexcept {
						canlook = true;
						this->inst.Update();
					}

					void Dispose(void) noexcept {
						this->inst.hitsver.clear();
						this->inst.hitsind.clear();

						this->inst.hits.Dispose();
						this->inst.hits_pic.Dispose();
					}

					/*視界外か否かを判断*/
					void Check_CameraViewClip(const VECTOR_ref& mini, const VECTOR_ref& maxi) noexcept {
						VECTOR_ref min = mini;
						VECTOR_ref max = maxi;
						min.y(-5.f);
						max.y(5.f);
						this->canlook = true;
						if (CheckCameraViewClip_Box(min.get(), max.get())) {
							this->canlook = false;
							return;
						}
					}

					void Draw(void) noexcept {
						if (this->canlook) {
							this->inst.Draw();
						}
					}
				};
			public:
				static const int grassDiv{ 12 };//6;
				const int grassEdge = 50;					/*grassの数*/
				const float size{ 24.f };
			private:
				const int grasss = grassEdge * grassEdge;	/*grassの数*/
				std::array<grass_t, grassDiv>grass__;
				std::array<VECTOR_ref, grassDiv>grassPosMin;
				std::array<VECTOR_ref, grassDiv>grassPosMax;
				int grasss2 = 20 * 20;					/*grassの数*/
				std::array<grass_t, grassDiv>grass2__;
				int grasss3 = 12 * 12;					/*grassの数*/
				std::array<grass_t, grassDiv>grass3__;
			public:
				int GetColorSoftImage(int softimage, int x_, int y_) {
					int _r_, _g_, _b_;
					int CCC = 0;
					GetPixelSoftImage(softimage, x_, y_, &_r_, &_g_, &_b_, nullptr);
					if (_r_ <= 64) {}
					else if (_r_ <= 192) { CCC |= (1 << 1); }
					else if (_r_ <= 256) { CCC |= (1 << 2); }
					if (_g_ <= 64) {}
					else if (_g_ <= 192) { CCC |= (1 << 4); }
					else if (_g_ <= 256) { CCC |= (1 << 5); }
					if (_b_ <= 64) {}
					else if (_b_ <= 192) { CCC |= (1 << 7); }
					else if (_b_ <= 256) { CCC |= (1 << 8); }
					return CCC;
				}
				int Flag = 0;

				struct GrassPos {
					int X_PosMin = 0;
					int Y_PosMin = 0;
					int X_PosMax = 0;
					int Y_PosMax = 0;
				};
				std::array<GrassPos, 12> grassPos;
				//y方向に操作する前提
				void SetMinMax(int CCC, int ID, int softimage, int x_t, int y_t, int sizex, int sizey) {
					int BufC = -1;
					if ((Flag & (1 << ID)) == 0) {
						Flag |= (1 << ID);
						//xmin
						grassPos[ID].X_PosMin = x_t;
						//ymin
						BufC = -1;
						for (int y_ = 0; y_ < sizey; y_++) {
							for (int x_ = grassPos[ID].X_PosMin; x_ < sizex; x_++) {
								BufC = GetColorSoftImage(softimage, x_, y_);
								if (BufC == CCC) {
									grassPos[ID].Y_PosMin = y_;
									break;
								}
								else {
									BufC = -1;
								}
							}
							if (BufC >= 0) { break; }
						}
						//xmax
						BufC = -1;
						for (int x_ = sizex - 1; x_ >= grassPos[ID].X_PosMin; x_--) {
							for (int y_ = sizey - 1; y_ >= grassPos[ID].Y_PosMin; y_--) {
								BufC = GetColorSoftImage(softimage, x_, y_);
								if (BufC == CCC) {
									grassPos[ID].X_PosMax = x_ + 1;
									break;
								}
								else {
									BufC = -1;
								}
							}
							if (BufC >= 0) { break; }
						}
						//ymax
						BufC = -1;
						for (int y_ = sizey - 1; y_ >= grassPos[ID].Y_PosMin; y_--) {
							for (int x_ = grassPos[ID].X_PosMax - 1; x_ >= grassPos[ID].X_PosMin; x_--) {
								BufC = GetColorSoftImage(softimage, x_, y_);
								if (BufC == CCC) {
									grassPos[ID].Y_PosMax = y_ + 1;
									break;
								}
								else {
									BufC = -1;
								}
							}
							if (BufC >= 0) { break; }
						}
						//ok
					}
				}
				void Init(void) noexcept {
					float MAPX = 8367.5f;
					float MAPZ = 5063.76f;
					float PosX = 0.f;
					float PosZ = -2300.0f;

					float MINX = -MAPX / 2.f + PosX;
					float MINZ = -MAPZ / 2.f + PosZ;

					float MAXX = MAPX / 2.f + PosX;
					float MAXZ = MAPZ / 2.f + PosZ;

					auto softimage = LoadSoftImage("data/model/map/grass.png");

					int sizex = 0, sizey = 0;
					GetSoftImageSize(softimage, &sizex, &sizey);

					float x_t = 0.f;
					float z_t = 0.f;
					Flag = 0;
					for (int x_ = 0; x_ < sizex; x_++) {
						for (int y_ = 0; y_ < sizey; y_++) {
							int CCC = GetColorSoftImage(softimage, x_, y_);
							//255,0,0
							if (CCC == (1 << 2)) {
								SetMinMax(CCC, 0, softimage, x_, y_, sizex, sizey);
							}
							//255,128,0
							else if (CCC == ((1 << 2) | (1 << 4))) {
								SetMinMax(CCC, 1, softimage, x_, y_, sizex, sizey);
							}
							//255,255,0
							else if (CCC == ((1 << 2) | (1 << 5))) {
								SetMinMax(CCC, 2, softimage, x_, y_, sizex, sizey);
							}
							//128,255,0
							else if (CCC == ((1 << 1) | (1 << 5))) {
								SetMinMax(CCC, 3, softimage, x_, y_, sizex, sizey);
							}
							//0,255,0
							else if (CCC == (1 << 5)) {
								SetMinMax(CCC, 4, softimage, x_, y_, sizex, sizey);
							}
							//0,255,128
							else if (CCC == ((1 << 5) | (1 << 7))) {
								SetMinMax(CCC, 5, softimage, x_, y_, sizex, sizey);
							}
							//0,255,255
							else if (CCC == ((1 << 5) | (1 << 8))) {
								SetMinMax(CCC, 6, softimage, x_, y_, sizex, sizey);
							}
							//0,128,255
							else if (CCC == ((1 << 4) | (1 << 8))) {
								SetMinMax(CCC, 7, softimage, x_, y_, sizex, sizey);
							}
							//0,0,255
							else if (CCC == (1 << 8)) {
								SetMinMax(CCC, 8, softimage, x_, y_, sizex, sizey);
							}
							//128,0,255
							else if (CCC == ((1 << 1) | (1 << 8))) {
								SetMinMax(CCC, 9, softimage, x_, y_, sizex, sizey);
							}
							//255,0,255
							else if (CCC == ((1 << 2) | (1 << 8))) {
								SetMinMax(CCC, 10, softimage, x_, y_, sizex, sizey);
							}
							//255,0,128
							else if (CCC == ((1 << 2) | (1 << 7))) {
								SetMinMax(CCC, 11, softimage, x_, y_, sizex, sizey);
							}

							//MINX + (MAXX - MINX) * x_ / sizex = x_t 
							//MINZ + (MAXZ - MINZ) * y_ / sizey = z_t 
						}
					}
					for (int ID = 0; ID < grassDiv; ID++) {
						//ポジション決定
						float xp = MINX + (MAXX - MINX) * grassPos[ID].X_PosMin / sizex;
						float zp = MINZ + (MAXZ - MINZ) * grassPos[ID].Y_PosMin / sizey;
						float xp2 = MINX + (MAXX - MINX) * grassPos[ID].X_PosMax / sizex;
						float zp2 = MINZ + (MAXZ - MINZ) * grassPos[ID].Y_PosMax / sizey;
						float xsize = xp2 - xp;
						float zsize = zp2 - zp;
						//
						{
							grassPosMin[ID] = VECTOR_ref::vget(xp, 0.2f, zp);
							grassPosMax[ID] = grassPosMin[ID] + VECTOR_ref::vget(xsize, 1.f, zsize);
							float xmid = xsize / 2.f;
							float zmid = zsize / 2.f;
							if (grasss != 0) {
								auto& tgt_g = grass__[ID];
								tgt_g.Init(grasss, 0);
								for (int i = 0; i < grasss; ++i) {
									//float x1 = (float)(i % grassEdge) * size + GetRandf(size / 4.f);
									//float z1 = (float)(i / grassEdge) * size + GetRandf(size / 4.f);
									float x1 = xmid + GetRandf(xmid);
									float z1 = zmid + GetRandf(zmid);
									while (true) {
										int CCC = GetColorSoftImage(softimage,
											(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
											(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
										);
										if (CCC != 0) {
											break;
										}
										x1 = xmid + GetRandf(xmid);
										z1 = zmid + GetRandf(zmid);
									}

									auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.2f, z1);
									auto tmpscale = VECTOR_ref::vget(size*1.5f, 1.0f + GetRandf(0.5f), size*1.5f);
									tgt_g.inst.hits.SetMatrix(MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale) * MATRIX_ref::Mtrans(tmpvect));
									tgt_g.Set_one();
								}
								tgt_g.put();
							}
							if (grasss2 != 0) {
								auto& tgt_g = grass2__[ID];
								tgt_g.Init(grasss2, 1);
								for (int i = 0; i < grasss2; ++i) {
									float x1 = xmid + GetRandf(xmid);
									float z1 = zmid + GetRandf(zmid);
									while (true) {
										int CCC = GetColorSoftImage(softimage,
											(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
											(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
										);
										if (CCC != 0) {
											break;
										}
										x1 = xmid + GetRandf(xmid);
										z1 = zmid + GetRandf(zmid);
									}
									auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.f, z1);
									auto tmpscale = VECTOR_ref::vget(size*1.f, 1.2f + GetRandf(0.5f), size*1.f);
									tgt_g.inst.hits.SetMatrix(MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale) * MATRIX_ref::Mtrans(tmpvect));
									tgt_g.Set_one();
								}
								tgt_g.put();
							}
							if (grasss3 != 0) {
								auto& tgt_g = grass3__[ID];
								tgt_g.Init(grasss3, 2);
								for (int i = 0; i < grasss3; ++i) {
									float x1 = xmid + GetRandf(xmid);
									float z1 = zmid + GetRandf(zmid);
									while (true) {
										int CCC = GetColorSoftImage(softimage,
											(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
											(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
										);
										if (CCC != 0) {
											break;
										}
										x1 = xmid + GetRandf(xmid);
										z1 = zmid + GetRandf(zmid);
									}
									auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.f, z1);
									auto tmpscale = VECTOR_ref::vget(size*1.f, 1.2f + GetRandf(0.5f), size*1.f);
									tgt_g.inst.hits.SetMatrix(MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale) * MATRIX_ref::Mtrans(tmpvect));
									tgt_g.Set_one();
								}
								tgt_g.put();
							}
						}
						//
					}
					DeleteSoftImage(softimage);
				}
				void Dispose(void) noexcept {
					for (int ID = 0; ID < grassDiv; ID++) {
						if (grasss != 0) {
							grass__[ID].Dispose();
						}
						if (grasss2 != 0) {
							grass2__[ID].Dispose();
						}
						if (grasss3 != 0) {
							grass3__[ID].Dispose();
						}
					}
				}
				void Check_CameraViewClip(void) noexcept {
					for (int ID = 0; ID < grassDiv; ID++) {
						if (grasss != 0) {
							this->grass__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
						}
						if (grasss2 != 0) {
							this->grass2__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
						}
						if (grasss3 != 0) {
							this->grass3__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
						}
					}
				}
				void Draw(cam_info camera_buf) noexcept {
					SetFogEnable(TRUE);
					SetFogStartEnd(camera_buf.near_*3.f, camera_buf.far_*3.f);
					SetFogColor(184, 187, 118);

					SetDrawAlphaTest(DX_CMP_GREATER, 128);

					SetUseLighting(FALSE);

					//auto dir=GetLightDirection();
					//VECTOR_ref vec = (VECTOR_ref)GetCameraPosition() - GetCameraTarget();
					//SetLightDirection(vec.Norm().get());

					for (int ID = 0; ID < grassDiv; ID++) {
						DrawCube3D(grassPosMin[ID].get(), grassPosMax[ID].get(), GetColor(0, 0, 0), GetColor(0, 0, 0), FALSE);
						if (grasss != 0) {
							grass__[ID].Draw();
						}
						if (grasss2 != 0) {
							grass2__[ID].Draw();
						}
						if (grasss3 != 0) {
							grass3__[ID].Draw();
						}
					}
					//SetLightDirection(dir);

					SetUseLighting(TRUE);

					SetDrawAlphaTest(-1, 0);
					SetFogEnable(FALSE);
				}
			};
		private:
			std::string LOGO1 = "data/picture/logo.png";
			std::string SUN = "data/model/sun/model.mv1";

			std::string Teio = "data/umamusume/teio/model.pmx";
			std::string Nice = "data/umamusume/nice/model.pmx";
			std::string Tanhoiza = "data/umamusume/tanhoiza/model.pmx";
			std::string Turbo = "data/umamusume/turbo/model.pmx";
			std::string Ikuno = "data/umamusume/ikuno/model.pmx";
			std::string Rice = "data/umamusume/rice/model.pmx";
			std::string Burbon = "data/umamusume/burbon/model.pmx";
			std::string MAC = "data/umamusume/mac/model.pmx";
			std::string SPE = "data/umamusume/spe/model.pmx";
			std::string SZK = "data/umamusume/suzuka/model.pmx";
			std::string Rudolf2 = "data/umamusume/rudolf2/model.pmx";

			std::string MAP = "data/model/map/model.mv1";
			std::vector<std::string> NAMES;

			int camsel = 0;
			switchs ChangeCamSel;
		private:
			LoadScriptClass LSClass;		//スクリプト読み込み
			TelopClass TLClass;				//テロップ
			size_t m_Counter = 0;			//カット
			std::vector<Cut_Info_First> m_CutInfo;
			std::vector<Cut_Info_Update> m_CutInfoUpdate;

			std::vector<Cut_Info_First> m_CutInfo_Buf;
			std::vector<Cut_Info_Update> m_CutInfoUpdate_Buf;
			//
			LONGLONG BaseTime = 0, WaitTime = 0, NowTimeWait = 0;
			bool ResetPhysics = true;
			bool isFirstLoop = true;		//カット最初のループか
			bool isfast = true;
			bool issecond = true;
			//
			CutInfoClass attached;
			std::vector<CutAttachDetail> attachedDetail;
			//データ
			ModelControl models;
			GraphControl graphs;
			SoundHandle BGM;
			int BGM_Frequency;

			MV1 Guide;
			Grass grassmodel;
			//ビュワー
			GraphHandle movie;
			switchs LookMovie;
			//
			switchs LookEditer;
			switchs SpeedUp;
			switchs SpeedDown;
			switchs Start;
			switchs MouseClick;
			bool PressSeek = false;
			int changeSeekID = -1;
			//
			int x_now = -1;
			int BaseWidth = 1920 / 64;
			int WidthPer = 2000000;
			int X_now = 0;
			int CutNow = 0;
			int SetAnimStart = -1;
			int SetAnimEnd = -1;
			ModelControl::Model* ChangeModel = nullptr;
			ModelControl::Model* ModelEdit = nullptr;
			size_t ModelEditCutNum = 0;
			bool ModelEditMode = false;
			bool ModelEditIn = false;
			bool ModelEdit_PhysicsReset = false;
			//
			int spd_x = 10;
			cam_info camera_buf;
			bool isFreepos = false;
			int x_m, y_m;
			int x_sav, y_sav;
			float x_rm = 0;
			float y_rm = 0;
			float r_rm = 100.f;
			//
			VECTOR_ref m_RandcamupBuf;
			VECTOR_ref m_RandcamvecBuf;
			VECTOR_ref m_RandcamposBuf;
			//
			float Black_Buf = 0.f;
			float White_Buf = 0.f;
			//
			int fog[3]{ -1,-1,-1 };
			float fog_range[2]{ -1.f,-1.f };
			//
			std::array<std::string, 3> ModelType{ "SKY_TRUE","NEAR_FALSE","FAR_TRUE" };
		public:
			//Getter
			bool Time_Over(void) const noexcept { return m_Counter >= m_CutInfo.size(); }
		public:
			using TEMPSCENE::TEMPSCENE;
			void Awake(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
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
					}
					TLClass.Init();
					int mdata = FileRead_open("data/Cut.txt", FALSE);
					int cnt = 0;
					SetUseASyncLoadFlag(TRUE);
					clsDx();
					auto NowTime = GetNowHiPerformanceCount();
					auto TotalTime = GetNowHiPerformanceCount();

					while (FileRead_eof(mdata) == 0) {
						LSClass.LoadScript(getparams::get_str(mdata));
						const auto& args = LSClass.Getargs();
						const auto& func = LSClass.Getfunc();
						if (func == "") { continue; }
						//変数登録
						LSClass.SetArgs();
						//モデル読み込み
						if (func.find("LoadModel") != std::string::npos) {
							for (int i = 0; i < std::stoi(args[1]); i++) {
								models.Load(args[0]);
							}
						}
						//カット
						//新規カット
						if (func.find("SetCut") != std::string::npos) {
							m_CutInfo.resize(m_CutInfo.size() + 1);
							m_CutInfo.back().SetTimeLimit((LONGLONG)(1000000.f * std::stof(args[0])));
							m_CutInfoUpdate.resize(m_CutInfoUpdate.size() + 1);
						}
						//Camposの指定
						else if (func.find("SetCampos_Attach") != std::string::npos) {
							auto startFrame = (int)(m_CutInfo.size()) - 1;
							attached.Switch.resize(attached.Switch.size() + 1);
							attached.Switch.back().SetSwitch(startFrame, startFrame + (std::stoi(args[0]) - 1));
							attachedDetail.resize(attachedDetail.size() + 1);
							attachedDetail.back().poscam.Set(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
						}
						else {
							//カメラ座標周り
							if (m_CutInfo.size() > 0) {
								if (m_CutInfo.back().LoadScript(func, args)) {
									m_CutInfoUpdate.back().CameraNotFirst = m_CutInfo.back().Aim_camera;
								}
							}
							if (m_CutInfoUpdate.size() > 0) {
								m_CutInfoUpdate.back().LoadScript(func, args);
							}
							//画像描画
							if (func.find("SetDrawGraph") != std::string::npos) {
								auto startFrame = (int)(m_CutInfo.size()) - 1;
								size_t in_str = args[1].find("~");
								if (in_str != std::string::npos) {
									int start_t = std::stoi(args[1].substr(0, in_str));
									int end_t = std::stoi(args[1].substr(in_str + 1));
									for (int i = start_t; i <= end_t; i++) {
										graphs.Get(args[0], i)->Init(startFrame, std::stoi(args[2]) - 1);
									}
								}
								else {
									graphs.Get(args[0], std::stoi(args[1]))->Init(startFrame, std::stoi(args[2]) - 1);
								}
							}
							else if (func.find("SetGraphBlur") != std::string::npos) {
								auto startFrame = (int)(m_CutInfo.size()) - 1;
								graphs.Get(args[0], std::stoi(args[1]))->CutDetail.back().Blur.Init(startFrame, std::stoi(args[2]) - 1);
							}
							else if (func.find("SetGraphOpacityRate") != std::string::npos) {
								graphs.Get(args[0], std::stoi(args[1]))->CutDetail.back().OpacityRate_Dist = std::stof(args[2]);
								graphs.Get(args[0], std::stoi(args[1]))->CutDetail.back().OpacityRate_Per = 0.f;
								graphs.Get(args[0], std::stoi(args[1]))->CutDetail.back().OpacityRate = std::stof(args[2]);
							}
							else if (func.find("SetGraphOpacityEasing") != std::string::npos) {
								graphs.Get(args[0], std::stoi(args[1]))->CutDetail.back().OpacityRate_Dist = std::stof(args[2]);
								graphs.Get(args[0], std::stoi(args[1]))->CutDetail.back().OpacityRate_Per = std::stof(args[3]);
							}
							//モデル描画
							else if (func.find("SetDrawModel") != std::string::npos) {
								auto startFrame = (int)(m_CutInfo.size()) - 1;
								size_t in_str = args[1].find("~");
								if (in_str != std::string::npos) {
									int start_t = std::stoi(args[1].substr(0, in_str));
									int end_t = std::stoi(args[1].substr(in_str + 1));
									for (int i = start_t; i <= end_t; i++) {
										models.Get(args[0], i)->Init(startFrame, std::stoi(args[2]) - 1);
									}
								}
								else {
									models.Get(args[0], std::stoi(args[1]))->Init(startFrame, std::stoi(args[2]) - 1);
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
							else if (func.find("SetModelOpacityEasing") != std::string::npos) {
								auto* t = models.Get(args[0], std::stoi(args[1]));
								t->CutDetail.back().OpacityRate_Dist = std::stof(args[2]);
								t->CutDetail.back().OpacityRate_Per = std::stof(args[3]);
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
						}
						//テロップ
						TLClass.LoadTelop(func, args);
						//END
						{
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
						if (ProcessMessage() != 0) { return; }
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
						//WaitKey();
					}
					else {
						return;
					}
				}
				//モデルの事前処理(非同期)
				models.Set();
				//
				fog[0] = 128;
				fog[1] = 128;
				fog[2] = 128;
				fog_range[0] = 200.f;
				fog_range[1] = 300000.f;

				grassmodel.Init();
				WaitKey();
			}
		private:
			class CharaInfoEdit {
			private:
				std::string Name;
				std::string Info;
				int x1;
				int y1;
				int xs;
				int ys;
				int thick = 2;
			public:
				CharaInfoEdit() {}
				~CharaInfoEdit() {}

				void Set(std::string_view name, int x_1, int y_1, int xsize, int ysize, int Thickness) noexcept {
					Name = name;
					x1 = x_1;
					y1 = y_1;
					xs = xsize;
					ys = ysize;
					thick = Thickness;
				}

				void Update(std::function<void()> DoInClick, std::string_view info) {
					int mouse_x, mouse_y;
					GetMousePoint(&mouse_x, &mouse_y);

					if (in2_(mouse_x, mouse_y, x1, y1, x1 + xs, y1 + ys)) {
						DoInClick();
					}

					Info = info;
				}

				void Draw(void) noexcept {
					int mouse_x, mouse_y;
					GetMousePoint(&mouse_x, &mouse_y);
					//AnimeSel
					{
						if (in2_(mouse_x, mouse_y, x1, y1, x1 + xs, y1 + ys)) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
							DrawBox(x1 + thick, y1 + thick, x1 + xs - thick, y1 + ys - thick, GetColor(255, 255, 255), TRUE);
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						}
						auto length = Fonts.Get(ys).Get_handle().GetDrawWidth(Info);
						if (length > xs / 2) {
							Fonts.Get(ys).Get_handle().DrawString_RIGHT(x1 + xs, y1, Info, GetColor(255, 255, 255));
						}
						else {
							Fonts.Get(ys).Get_handle().DrawString(x1 + xs / 2, y1, Info, GetColor(255, 255, 255));
						}
						Fonts.Get(ys).Get_handle().DrawString(x1, y1, Name, GetColor(255, 255, 255));
					}
				}
			};
			std::vector<CharaInfoEdit> CharaEdit;
			//calc
			int OffsetCalc(int x_p, int x_s)noexcept {
				LONGLONG now2 = (m_Counter >= 1) ? m_CutInfo[m_Counter - 1].GetTimeLimit() : 0;
				int width_Time = BaseWidth * (int)(NowTimeWait - now2) / WidthPer;

				int ans = -1;
				//オフセット計算
				{
					//現在地
					int position = x_s / 5;
					//スクロールいらないか判定
					{
						int x1 = x_p;
						int i = 0;
						while (true) {
							LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].GetTimeLimit() : 0;
							LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].GetTimeLimit() : 0;
							x1 += BaseWidth * (int)(now - base) / WidthPer;
							i++;
							if (i >= m_CutInfo.size()) {
								position = x_s;
								break;
							}
						}
					}
					//
					{
						int x1 = x_p;
						int i = 0;
						while (true) {
							LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].GetTimeLimit() : 0;
							LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].GetTimeLimit() : 0;
							x1 += BaseWidth * (int)(now - base) / WidthPer;
							//NOW
							if (i == m_Counter) {
								ans = x1;
								break;
							}
							i++;
							if (i >= m_CutInfo.size()) { break; }
						}
					}
					//SetNOW
					if (ans >= 0) {
						X_now = std::min(-(ans + width_Time - (x_p + position)), 0);
					}
					//CutNow計算
					{
						int x1 = x_p + X_now;
						int i = 0;
						while (true) {
							LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].GetTimeLimit() : 0;
							LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].GetTimeLimit() : 0;

							x1 += BaseWidth * (int)(now - base) / WidthPer;
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
							LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].GetTimeLimit() : 0;
							LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].GetTimeLimit() : 0;
							x1 += BaseWidth * (int)(now - base) / WidthPer;
							//NOW
							if (i == m_Counter) {
								ans = x1;
								break;
							}
							i++;
							if (i >= m_CutInfo.size()) { break; }
						}
					}
					//SetNOW再計算
					if (ans >= 0) {
						X_now = std::min(-(ans + width_Time - (x_p + position)), 0);
					}
				}
				return ans;
			}
			void SeekBer_Calc(int x_p, int x_s, int y_p, int y_s) noexcept {
				int mouse_x, mouse_y;
				GetMousePoint(&mouse_x, &mouse_y);

				if (MouseClick.press()) {
					//
					if (MouseClick.trigger()) {
						int x1 = x_p + X_now;
						int i = CutNow;
						int y_hight = y_r(24);
						while (true) {
							LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].GetTimeLimit() : 0;
							LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].GetTimeLimit() : 0;

							x1 += BaseWidth * (int)(now - base) / WidthPer;
							if (x1 > x_p + x_s) { break; }
							//LINE
							if (x1 > x_p) {
								int width_Next = BaseWidth * (int)(m_CutInfo[i].GetTimeLimit() - now) / WidthPer;
								if (in2_(mouse_x, mouse_y, x1, y_p + y_s, x1 + width_Next, y_p + y_s + y_hight)) {
									changeSeekID = std::max(i - 1, 0);
									PressSeek = true;
									//
								}
							}
							i++;
							if (i >= m_CutInfo.size()) { break; }
						}
					}
					//
				}
				else {
					//
					if (PressSeek) {
						PressSeek = false;
						//
						m_Counter = changeSeekID;
						models.Start(m_Counter);
						graphs.Start(m_Counter);
						attached.Start(m_Counter);
						BaseTime = GetMocroSec();
						WaitTime = 0;
						NowTimeWait = (m_Counter > 0 ? m_CutInfo[m_Counter - 1].GetTimeLimit() : 0);

						fog[0] = 128;
						fog[1] = 128;
						fog[2] = 128;
						fog_range[0] = 200.f;
						fog_range[1] = 300000.f;
						Black_Buf = 0.f;
						White_Buf = 0.f;

						auto* PostPassParts = PostPassEffect::Instance();

						PostPassParts->Set_Bright(255, 255, 255);


						m_CutInfo = m_CutInfo_Buf;
						m_CutInfoUpdate = m_CutInfoUpdate_Buf;

						ModelEdit_PhysicsReset = true;
						//
						changeSeekID = -1;

						if (Start.on()) {
							ModelEditIn = true;
						}

					}
					//
				}
			}
			void SetModelSel(ModelControl::Model* tmp, int value) noexcept {
				if (ChangeModel == nullptr) { ChangeModel = tmp; }
				if (SetAnimStart == -1) { SetAnimStart = value; }
				SetAnimEnd = std::max(value, SetAnimStart);
			}
			void ResetModelSel(void) noexcept {
				ChangeModel = nullptr;
				SetAnimStart = -1;
				SetAnimEnd = -1;
			}
			void SetModelEdit(ModelControl::Model* tmp, int value) noexcept {
				ModelEdit = tmp;
				ModelEditCutNum = value;
			}
			void ResetModelEdit(void) noexcept {
				ModelEdit = nullptr;
				ModelEditCutNum = 0;
			}
			void Editer_Init(void) noexcept {
				auto* DrawParts = DXDraw::Instance();

				int x_p = DrawParts->disp_x * 5 / 10;
				int x_s = DrawParts->disp_x * 4 / 10;
				int y_p = DrawParts->disp_y * 2 / 10;

				int hight = y_r(20);

				int p2 = 2;

				CharaEdit.resize(5);

				int y1 = y_p + hight + p2;
				CharaEdit[0].Set("AnimeSel", x_p, y1, x_s, hight, p2); y1 += hight + p2;
				CharaEdit[1].Set("DrawMode", x_p, y1, x_s, hight, p2); y1 += hight + p2;
				CharaEdit[2].Set("ModelPhysicsSpeed", x_p, y1, x_s, hight, p2); y1 += hight + p2;
				CharaEdit[3].Set("OpacityRate", x_p, y1, x_s, hight, p2); y1 += hight + p2;
				CharaEdit[4].Set("Matrix", x_p, y1, x_s, hight, p2); y1 += hight + p2;
			}
			void Editer_Calc(void)noexcept {
				auto* DrawParts = DXDraw::Instance();

				int mouse_x, mouse_y;
				GetMousePoint(&mouse_x, &mouse_y);

				BaseWidth = DrawParts->disp_x / 64;

				if (LookEditer.on()) {
					//編集画面
					{
						int x_p = DrawParts->disp_x * 1 / 10;
						int x_s = DrawParts->disp_x * 8 / 10;
						int y_p = DrawParts->disp_y * 5 / 10;
						int y_s = DrawParts->disp_y * 4 / 10;

						int hight = y_s / (int)(models.GetMax());

						LONGLONG now2 = (m_Counter >= 1) ? m_CutInfo[m_Counter - 1].GetTimeLimit() : 0;
						int width_Time = BaseWidth * (int)(NowTimeWait - now2) / WidthPer;

						x_now = OffsetCalc(x_p, x_s);
						//判定演算
						if (!ModelEditMode) {
							SeekBer_Calc(x_p, x_s, y_p, y_s);
							if (!PressSeek) {
								if (MouseClick.press()) {
									int x1 = x_p + X_now;
									int i = CutNow;
									while (true) {
										LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].GetTimeLimit() : 0;
										LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].GetTimeLimit() : 0;

										x1 += BaseWidth * (int)(now - base) / WidthPer;
										if (x1 > x_p + x_s) { break; }
										if (x1 > x_p) {
											int width_Next = BaseWidth * (int)(m_CutInfo[i].GetTimeLimit() - now) / WidthPer;
											int msel = std::min((mouse_y - y_p) / hight, (int)(models.GetMax()) - 1);
											if (msel >= 0) {
												int y1 = y_p + msel * hight;
												int xx = x1;
												if (in2_(mouse_x, mouse_y, xx, y1, x1 + width_Next, y1 + hight - 1)) {
													if (x_now >= 0) {
														xx = std::max(xx, x_now + X_now + width_Time);
													}
												}
												if (xx < x1 + width_Next) {
													if (in2_(mouse_x, mouse_y, xx, y1, x1 + width_Next, y1 + hight - 1)) {
														auto* tmp = models.Get(models.GetModel()[msel].Path, models.GetModel()[msel].BaseID);
														bool EditModelInfo = false;
														for (auto& c : tmp->Cutinfo.Switch) {
															if (c.IsIn(i)) {
																EditModelInfo = true;
																SetModelEdit(tmp, (int)(&c - &tmp->Cutinfo.Switch.front()));
																break;
															}
														}
														if (!EditModelInfo || ModelEdit == nullptr) {
															SetModelSel(tmp, i);
														}
													}
												}
											}
										}
										i++;
										if (i >= m_CutInfo.size()) { break; }
									}
								}
								else {
									//被りチェック
									if (ChangeModel != nullptr) {
										//修正
										for (const auto& c : ChangeModel->Cutinfo.Switch) {
											if (SetAnimStart >= c.Off) { SetAnimStart = std::max(SetAnimStart, c.Off + 1); }
											if (SetAnimStart < c.On) { SetAnimEnd = std::min(SetAnimEnd, c.On - 1); }
										}
										//確認
										for (const auto& c : ChangeModel->Cutinfo.Switch) {
											if (SetAnimStart == c.On || SetAnimEnd == c.Off) {
												ResetModelSel();
												break;
											}
										}
									}
									//登録できるなら作成
									if (ChangeModel != nullptr) {
										for (const auto& c : ChangeModel->Cutinfo.Switch) {
											if (SetAnimStart < c.On) {
												int id = (int)(&c - &ChangeModel->Cutinfo.Switch.front());
												ChangeModel->Cutinfo.Insert(id, SetAnimStart, SetAnimEnd);
												ChangeModel->CutDetail.insert(ChangeModel->CutDetail.begin() + id, CutinfoDetail());
												auto& info = ChangeModel->CutDetail[id];
												info.animsel = 0;
												info.isloop = true;
												info.animspeed = 1.0f;
												info.startframe = 0.f;

												if (SetAnimStart <= m_Counter && m_Counter <= SetAnimEnd) {
													ModelEdit_PhysicsReset = true;
												}
												break;
											}
										}
										ResetModelSel();
									}
									else if (ModelEdit != nullptr && !ModelEditMode) {
										ModelEditMode = true;
										if (Start.on()) {
											ModelEditIn = true;
										}
									}
									//
									//
								}
							}
						}
						//モデルエディットモード判定(参照変更)
						if (ModelEdit != nullptr && ModelEditMode) {
							if (MouseClick.press()) {
								int x1 = x_p + X_now;
								int i = CutNow;
								while (true) {
									LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].GetTimeLimit() : 0;
									LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].GetTimeLimit() : 0;

									x1 += BaseWidth * (int)(now - base) / WidthPer;
									if (x1 > x_p + x_s) { break; }
									if (x1 > x_p) {
										int width_Next = BaseWidth * (int)(m_CutInfo[i].GetTimeLimit() - now) / WidthPer;
										int msel = std::min((mouse_y - y_p) / hight, (int)(models.GetMax()) - 1);
										if (msel >= 0) {
											int y1 = y_p + msel * hight;
											if (in2_(mouse_x, mouse_y, x1, y1, x1 + width_Next, y1 + hight)) {
												auto* tmp = models.Get(models.GetModel()[msel].Path, models.GetModel()[msel].BaseID);
												for (auto& c : tmp->Cutinfo.Switch) {
													if (c.IsIn(i)) {
														SetModelEdit(tmp, (int)(&c - &tmp->Cutinfo.Switch.front()));
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
					if (ModelEdit != nullptr && ModelEditMode) {
						int x_p = DrawParts->disp_x * 5 / 10;
						int x_s = DrawParts->disp_x * 4 / 10;
						int y_p = DrawParts->disp_y * 2 / 10;
						int y_s = DrawParts->disp_y * 5 / 20;
						//EndMode
						{
							int x1 = x_p + x_s - 32 - 6;
							int y1 = y_p + y_s - 32 - 6;
							int x2 = x1 + 32;
							int y2 = y1 + 32;
							if (MouseClick.trigger() && in2_(mouse_x, mouse_y, x1, y1, x2, y2)) {
								ResetModelEdit();
								ModelEditMode = false;
								/*
								if (!Start.on()) {
									ModelEditIn = true;
								}
								//*/
							}
						}
						//info
						if (ModelEditMode) {
							std::string mode;
							auto& c = ModelEdit->CutDetail[ModelEditCutNum];
							//AnimeSel
							CharaEdit[0].Update([&]() {
								if (MouseClick.trigger()) {
									++c.animsel %= ModelEdit->obj.get_anime().size();
									ModelEdit_PhysicsReset = true;
								}
							}, std::to_string(c.animsel));
							//DrawMode
							{
								mode = "NORMAL";
								if (ModelEdit->isBGModel) {
									mode = ModelType[0];
								}
								else if (!ModelEdit->IsNearShadow) {
									mode = ModelType[1];
								}
								else if (ModelEdit->IsFarShadow) {
									mode = ModelType[2];
								}
							}
							CharaEdit[1].Update([&]() {
								if (MouseClick.trigger()) {
									if (
										!ModelEdit->isBGModel &&
										ModelEdit->IsNearShadow &&
										!ModelEdit->IsFarShadow
										) {
										//ノーマルならBGモデルに
										ModelEdit->isBGModel = true;
									}
									else if (
										ModelEdit->isBGModel &&
										ModelEdit->IsNearShadow &&
										!ModelEdit->IsFarShadow
										) {
										//BGなら近距離影なしモデルに
										ModelEdit->isBGModel = false;
										ModelEdit->IsNearShadow = false;
									}
									else if (
										!ModelEdit->isBGModel &&
										!ModelEdit->IsNearShadow &&
										!ModelEdit->IsFarShadow
										) {
										//近距離影なしなら遠距離影なしモデルに
										ModelEdit->IsNearShadow = true;
										ModelEdit->IsFarShadow = true;
									}
									else if (
										!ModelEdit->isBGModel &&
										ModelEdit->IsNearShadow &&
										ModelEdit->IsFarShadow
										) {
										//遠距離影なしならノーマルモデルに
										ModelEdit->IsFarShadow = false;
										ModelEdit->isBGModel = false;
									}
									else {
										//何か問題があったらnormalに
										ModelEdit->isBGModel = false;
										ModelEdit->IsNearShadow = true;
										ModelEdit->IsFarShadow = false;
									}
								}
							}, mode);
							//ModelPhysicsSpeed
							CharaEdit[2].Update([&]() {
								if (MouseClick.trigger()) {
								}
							}, (c.PhysicsSpeed_ >= 0.f) ? std::to_string(c.PhysicsSpeed_) : "continuous");
							//OpacityRate
							CharaEdit[3].Update([&]() {
								if (MouseClick.trigger()) {
								}
							}, (c.OpacityRate >= 0.f) ? std::to_string(c.OpacityRate) : "continuous");
							//Matrix
							{
								if (c.usemat) {
									char buf[256] = "";
									sprintfDx(buf, "%5.2f°(%5.2f,%5.2f,%5.2f) %5.2f°", c.Yrad1_p, c.pos_p.x(), c.pos_p.y(), c.pos_p.z(), c.Yrad2_p);
									mode = buf;
								}
								else {
									mode = "continuous";
								}
							}
							CharaEdit[4].Update([&]() {
								if (MouseClick.trigger()) {
								}
							}, mode);
						}
						//End
					}
				}
			}
			//draw
			void DrawBackGround(int x1, int y1, int x2, int y2) noexcept {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				DrawBox(x1, y1, x2, y2, GetColor(0, 0, 0), TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			void Editer_Draw(void)noexcept {
				auto* DrawParts = DXDraw::Instance();

				int mouse_x, mouse_y;
				GetMousePoint(&mouse_x, &mouse_y);

				if (LookEditer.on()) {
					//編集画面
					{
						int x_p = DrawParts->disp_x * 1 / 10;
						int x_s = DrawParts->disp_x * 8 / 10;
						int y_p = DrawParts->disp_y * 5 / 10;
						int y_s = DrawParts->disp_y * 4 / 10;

						int hight = y_s / (int)(models.GetMax());

						LONGLONG now2 = (m_Counter >= 1) ? m_CutInfo[m_Counter - 1].GetTimeLimit() : 0;
						int width_Time = BaseWidth * (int)(NowTimeWait - now2) / WidthPer;

						//BACK
						{
							//BG
							DrawBackGround(x_p, y_p, x_p + x_s, y_p + y_s);
							//line
							{
								for (int i = 0; i < models.GetMax() + 1; i++) {
									int y1 = y_p + i * hight;
									DrawLine(x_p, y1, x_p + x_s, y1, GetColor(128, 128, 128), 3);
								}
								int x1 = x_p + X_now;
								int i = CutNow;
								while (true) {
									LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].GetTimeLimit() : 0;
									LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].GetTimeLimit() : 0;

									x1 += BaseWidth * (int)(now - base) / WidthPer;
									if (x1 > x_p + x_s) { break; }
									//LINE
									if (x1 > x_p) {
										DrawLine(x1, y_p, x1, y_p + y_s, GetColor(128, 128, 128), 3);
										int width_Next = BaseWidth * (int)(m_CutInfo[i].GetTimeLimit() - now) / WidthPer;

										int xx = x1;
										if (in2_(mouse_x, mouse_y, xx, y_p, x1 + width_Next, y_p + y_s)) {
											if (x_now >= 0) {
												xx = std::max(xx, x_now + X_now + width_Time);
											}
										}
										if (xx < x1 + width_Next) {
											if (!ModelEditMode && in2_(mouse_x, mouse_y, xx, y_p, x1 + width_Next, y_p + y_s - 1)) {
												int y1 = y_p + ((mouse_y - y_p) / hight) * hight;
												SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
												DrawBox(xx, y1, x1 + width_Next, y1 + hight, GetColor(255, 255, 255), TRUE);
												SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
											}
										}
									}
									i++;
									if (i >= m_CutInfo.size()) { break; }
								}
							}
							//seekbar
							{
								int x1 = x_p + X_now;
								int i = CutNow;
								int y_hight = y_r(24);

								DrawBox(x_p, y_p + y_s, x_p + x_s, y_p + y_s + y_hight, GetColor(0, 0, 0), TRUE);

								while (true) {
									LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].GetTimeLimit() : 0;
									LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].GetTimeLimit() : 0;

									x1 += BaseWidth * (int)(now - base) / WidthPer;
									if (x1 > x_p + x_s) { break; }
									//LINE
									if (x1 > x_p) {
										int width_Next = BaseWidth * (int)(m_CutInfo[i].GetTimeLimit() - now) / WidthPer;

										if (i == m_Counter) {
											SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
											DrawBox(x1, y_p + y_s, x1 + width_Next, y_p + y_s + y_hight, GetColor(255, 255, 255), TRUE);
											SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
										}

										DrawLine(x1, y_p + y_s, x1, y_p + y_s + y_hight, GetColor(128, 128, 128), 1);

										if (!ModelEditMode && in2_(mouse_x, mouse_y, x1, y_p + y_s, x1 + width_Next, y_p + y_s + y_hight)) {
											DrawBox(x1, y_p + y_s, x1 + width_Next, y_p + y_s + y_hight, GetColor(255, 0, 0), TRUE);
										}
									}
									i++;
									if (i >= m_CutInfo.size()) { break; }
								}
							}
							//
						}
						//timeline
						{
							int x1 = x_p + X_now;
							int i = CutNow;
							while (true) {
								LONGLONG base = (i >= 2) ? m_CutInfo[i - 2].GetTimeLimit() : 0;
								LONGLONG now = (i >= 1) ? m_CutInfo[i - 1].GetTimeLimit() : 0;
								int width_Next = BaseWidth * (int)(m_CutInfo[i].GetTimeLimit() - now) / WidthPer;

								x1 += BaseWidth * (int)(now - base) / WidthPer;
								if (x1 > x_p + x_s) { break; }
								//
								int xp = 3, yp = 3;
								int p2 = 2;
								int y1 = y_p;
								for (const auto& m : models.GetModel()) {
									if (ChangeModel != nullptr && ChangeModel == (ModelControl::Model*)&m) {
										if (SetAnimStart <= i && i <= SetAnimEnd) {
											if (x1 < x1 + width_Next) {
												DrawBox(x1, y1 + yp, x1 + width_Next, y1 + hight - yp, GetColor(255, 255, 0), FALSE);
											}
										}
									}
									for (const auto& c : m.Cutinfo.Switch) {
										if (c.IsIn(i)) {
											int x_b2 = std::max(x1 + ((i == c.On) ? p2 : 0), x_p);
											int x_b1 = std::max(x1 + ((i == c.On) ? xp : 0), x_p);
											int x_b4 = std::min(x1 + width_Next - ((i == c.Off) ? p2 : 0), x_p + x_s);
											int x_b3 = std::min(x1 + width_Next - ((i == c.Off) ? xp : 0), x_p + x_s);

											unsigned int color;
											if (m.isBGModel) {
												if (c.IsIn(m_Counter)) {
													color = GetColor(100, 216, 100);
												}
												else {
													color = GetColor(60, 60, 192);
												}
											}
											else {
												if (c.IsIn(m_Counter)) {
													color = GetColor(150, 255, 150);
												}
												else {
													color = GetColor(100, 100, 255);
												}
											}
											if (
												ModelEditMode &&
												ModelEdit == (ModelControl::Model*)&m &&
												ModelEditCutNum == (size_t)(&c - &m.Cutinfo.Switch.front())
												) {
												color = GetColor(255, 255, 0);
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
						//NOWline
						if (x_now >= 0) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
							DrawBox(x_p, y_p, x_now + X_now + width_Time, y_p + y_s, GetColor(0, 0, 0), TRUE);
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
							DrawLine(x_now + X_now + width_Time, y_p, x_now + X_now + width_Time, y_p + y_s, GetColor(255, 255, 255), 3);
						}
						//OverRay
						if (ModelEditMode) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
							DrawBox(x_p, y_p, x_p + x_s, y_p + y_s, GetColor(0, 0, 0), TRUE);
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						}
						//modelName
						{
							int y1 = y_p;
							for (const auto& m : models.GetModel()) {
								const auto* sel = LSClass.GetArgFromPath(m.Path);
								if (sel != nullptr) {
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
									for (const auto& c : m.Cutinfo.Switch) {
										if (c.IsIn(m_Counter)) {
											SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
											break;
										}
									}
									Fonts.Get(hight).Get_handle().DrawString(x_p, y1, " " + sel->Base + "(" + std::to_string(m.BaseID) + ")", GetColor(255, 255, 255));
									SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
								}
								y1 += hight;
							}
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						}
						//
					}
					//モデルエディットモード
					if (ModelEdit != nullptr && ModelEditMode) {
						int x_p = DrawParts->disp_x * 5 / 10;
						int x_s = DrawParts->disp_x * 4 / 10;
						int y_p = DrawParts->disp_y * 2 / 10;
						int y_s = DrawParts->disp_y * 5 / 20;

						int hight = y_r(20);
						{
							//BACK
							{
								//BG
								DrawBackGround(x_p, y_p, x_p + x_s, y_p + y_s);
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
									DrawBox(x1, y1, x2, y2, color, TRUE);
								}
								else {
									color = GetColor(255, 0, 0);
									DrawBox(x1, y1, x2, y2, color, TRUE);
								}
							}
							//info
							{
								int y1 = y_p;
								int p2 = 2;
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
								for (auto& ce : CharaEdit) {
									ce.Draw();
								}
								//
							}
						}
						//
					}
					//
					//printfDx("FPS   : %5.2f\n", FPS);
					printfDx("Cut   : %d\n", m_Counter);
					printfDx("\n");
					printfDx("ENTER : View Change\n");
					printfDx("M     : Movie Switch\n");
					printfDx("N     : Editer Switch\n");
					printfDx("←→  : Speed Change(x0.0～x2.0)\n");
					printfDx("SPACE : STOP \n");
					printfDx("\n");
				}
				//
				for (const auto& m : models.GetModel()) {
					const auto* sel = LSClass.GetArgFromPath(m.Path);
					if (sel != nullptr) {
						printfDx((" " + sel->Base + "(" + std::to_string(m.BaseID) + ") = %d\n").c_str(), m.DrawCount);
					}
				}
			}
		public:
			void Set(void) noexcept override {
				auto* PostPassParts = PostPassEffect::Instance();
				TEMPSCENE::Set_EnvLight(VECTOR_ref::vget(5000.f, 50.f, 5000.f), VECTOR_ref::vget(-5000.f, -10.f, -5000.f), VECTOR_ref::vget(-0.3f, -0.5f, -0.2f), GetColorF(0.42f, 0.41f, 0.40f, 0.f));
				TEMPSCENE::Set();
				models.Get(SUN, 0)->obj.SetMatrix(MATRIX_ref::RotVec2(VECTOR_ref::up(), (VECTOR_ref)(Get_Light_vec().Norm())) * MATRIX_ref::Mtrans(Get_Light_vec().Norm() * -1500.f));
				m_Counter = 25;
				m_Counter = 0;
				models.Start(m_Counter);
				graphs.Start(m_Counter);
				attached.Start(m_Counter);

				SetUseASyncLoadFlag(FALSE);
				BGM = SoundHandle::Load("data/sound.wav");
				BGM.vol(0);
				///*
				movie = GraphHandle::Load("data/base_movie.mp4");
				PauseMovieToGraph(movie.get());
				//*/

				MV1::Load("data/model/map/way.pmd", &Guide);
				//プレイ用意
				GameSpeed = (float)(spd_x) / 10.f;
				PostPassParts->Set_Bright(255, 255, 255);
				BaseTime = GetMocroSec() - (m_Counter > 0 ? m_CutInfo[m_Counter - 1].GetTimeLimit() : 0);
				WaitTime = (m_Counter != 0) ? 0 : 1000000;
				NowTimeWait = -WaitTime;

				m_RandcamupBuf.clear();
				m_RandcamvecBuf.clear();
				m_RandcamposBuf.clear();

				ResetPhysics = true;

				LookEditer.Init(false);
				LookMovie.Init(false);
				Start.Init(true);

				m_CutInfo_Buf = m_CutInfo;
				m_CutInfoUpdate_Buf = m_CutInfoUpdate;
				/*
				m_Counter = 0;
				models.Start(m_Counter);
				graphs.Start(m_Counter);
				attached.Start(m_Counter);
				BaseTime = GetMocroSec() - (m_Counter > 0 ? m_CutInfo[m_Counter - 1].GetTimeLimit() : 0);
				WaitTime = 0;
				NowTimeWait = (m_Counter > 0 ? m_CutInfo[m_Counter - 1].GetTimeLimit() : 0);
				//*/
				Editer_Init();
			}
			bool Update(void) noexcept override {
				if (Time_Over()) { return false; }

				auto* PostPassParts = PostPassEffect::Instance();
				auto* DrawParts = DXDraw::Instance();

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

					if (NowTimeWait >= 0) {
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
					//
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
					issecond = true;
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
					else {
						if (issecond) {
							issecond = false;
							DrawParts->UpdateFarShadow([&] {Shadow_Draw_Far(); });
						}
					}
				}
				//カットの処理
				{
					{
						if (attached.Update_((int)m_Counter)) {
							attached.Update_((int)m_Counter);
						}
						models.FirstUpdate((int)m_Counter, isFirstLoop);
						graphs.FirstUpdate((int)m_Counter, isFirstLoop);
					}
					//
					{
						int SEL = 0;
						//0
						if (m_Counter == SEL) {
							int GudeStart = 0;
							if (isFirstLoop) {
								NAMES.resize(NAMES.size() + 1);
								NAMES.back() = Teio;
								NAMES.resize(NAMES.size() + 1);
								NAMES.back() = Nice;
								NAMES.resize(NAMES.size() + 1);
								NAMES.back() = Tanhoiza;
								NAMES.resize(NAMES.size() + 1);
								NAMES.back() = Turbo;
								NAMES.resize(NAMES.size() + 1);
								NAMES.back() = Ikuno;
								NAMES.resize(NAMES.size() + 1);
								NAMES.back() = Rice;
								NAMES.resize(NAMES.size() + 1);
								NAMES.back() = Burbon;
								NAMES.resize(NAMES.size() + 1);
								NAMES.back() = MAC;
								NAMES.resize(NAMES.size() + 1);
								NAMES.back() = SPE;
								NAMES.resize(NAMES.size() + 1);
								NAMES.back() = SZK;
								NAMES.resize(NAMES.size() + 1);
								NAMES.back() = Rudolf2;


								Guide.SetMatrix(models.Get(MAP, 0)->obj.GetMatrix());
								for (int i = 0; i < Guide.frame_num(); i++) {
									if (Guide.frame_child_num(i) > 0) {
										GudeStart = i;
										break;
									}
								}
							}
							{
								float xof = 0.f;
								for (auto& n : NAMES) {
									auto* m = models.Get(n, 0);
									auto& inf = m->CutDetail[m->Cutinfo.nowcut];
									if (isFirstLoop) {
										if (inf.usemat) {
											m->pos_p = VECTOR_ref::vget(0.f, 0.f, xof);
											m->xposition = xof;
											m->Yrad1_p = inf.Yrad1_p;
											m->Zrad1_p = 0.f;
											m->Yrad2_p = inf.Yrad2_p;
											m->pos_z_p = 0.f;
										}
										m->animspd = 0.f;
										m->GuideNum[0] = GudeStart;
										for (int i = 1; i < m->GuideNum.size(); i++) {
											if (Guide.frame_child_num(m->GuideNum[i]) > 0) { m->GuideNum[i] = (int)Guide.frame_child(m->GuideNum[i - 1], 0); }
											else {
												m->GuideNum[i] = -1;
												break;
											}
										}
										m->PhysicsSpeed = 10.f;


										m->Aim_p = Guide.frame(m->GuideNum[0]);
										auto vec = (m->Aim_p - m->pos_p);
										vec.y(0.f);
										//float length = vec.size();
										vec = vec.Norm();
										auto vec2 = MATRIX_ref::RotY(deg2rad(m->Yrad1_p)).zvec();
										vec2.y(0.f);
										vec2 = vec2.Norm();
										float cosn = vec2.cross(vec).y();
										float radn = -rad2deg(cosn);
										m->Yradaddbuf_p = radn;
										m->Yradadd_p = m->Yradaddbuf_p;
										m->Zrad1buf_p = 0.f;

										m->OLDPos_p = m->pos_p;
										m->OLDVec_p = m->Aim_p - m->OLDPos_p;
										m->OLDVec_p.y(0);
										m->OLDDist = m->OLDVec_p.size();
										m->OLDVec_p = m->OLDVec_p.Norm();
										m->dist = 1000000.f;
									}
									else {
										if (m->GuideNum[0] >= 0) {
											{
												VECTOR_ref Aim;
												int aimcnt = 0;
												for (int i = 0; i < m->GuideNum.size(); i++) {
													if (m->GuideNum[i] >= 0) {
														auto pb = Guide.frame(m->GuideNum[i]);
														auto zvb = (Guide.frame((int)Guide.frame_child(m->GuideNum[i], 0)) - pb).Norm();
														auto xvb = zvb.cross(VECTOR_ref::up());
														Aim += pb + xvb * (50.f + m->xposition); aimcnt++;
													}
												}
												if (aimcnt > 0) {
													m->Aim_p = Aim * (1.f / (float)aimcnt);
												}
											}

											auto vec = m->Aim_p - m->pos_p;
											vec.y(0.f);

											float length = vec.size();

											vec = vec.Norm();


											float per_s = std::clamp(((length - 100.f) / m->dist) / 10.f, 0.0f, 1.0f);//0=>1
											if (m->OLDDist > 1000.f) {
												per_s = std::clamp(((length - 500.f) / m->dist) / 20.f, 0.0f, 1.0f);
											}

											//printfDx("PER = %5.2f(%5.2f)\n", per_s, ((length - 100.f) / m->dist) / 10.f);
											vec = vec * (1.f - per_s) + m->OLDVec_p * per_s;

											auto vec2 = MATRIX_ref::RotY(deg2rad(m->Yrad1_p)).zvec();
											vec2.y(0.f);
											vec2 = vec2.Norm();

											float cosn = vec2.cross(vec).y();
											float radn = std::clamp(-rad2deg(cosn), -45.f, 45.f);
											//printfDx("m->OLDDist = %f\n", m->OLDDist);
											if (m->OLDDist > 1000.f) {
												easing_set(&m->Yradaddbuf_p, radn, 0.f);
												easing_set(&m->Yradadd_p, m->Yradaddbuf_p, 0.f);
											}
											else {
												easing_set(&m->Yradaddbuf_p, radn, 0.5f);
												easing_set(&m->Yradadd_p, m->Yradaddbuf_p, 0.5f);
											}
											if (Guide.frame_child_num(m->GuideNum[0]) > 0) {
												//
												if (length <= 20.f) {
													m->GuideNum[0] = (int)Guide.frame_child(m->GuideNum[0], 0);
													for (int i = 1; i < m->GuideNum.size(); i++) {
														if (Guide.frame_child_num(m->GuideNum[i]) > 0) { m->GuideNum[i] = (int)Guide.frame_child(m->GuideNum[i - 1], 0); }
														else {
															m->GuideNum[i] = -1;
															break;
														}
													}
													//
													auto aa = m->Aim_p - m->pos_p;
													aa.y(0);
													m->dist = aa.size();
													//
													{
														VECTOR_ref Aim;
														int aimcnt = 0;
														for (int i = 0; i < m->GuideNum.size(); i++) {
															if (m->GuideNum[i] >= 0) {
																auto pb = Guide.frame(m->GuideNum[i]);
																auto zvb = (Guide.frame((int)Guide.frame_child(m->GuideNum[i], 0)) - pb).Norm();
																auto xvb = zvb.cross(VECTOR_ref::up());
																Aim += pb + xvb * (50.f + m->xposition); aimcnt++;
															}
														}
														if (aimcnt > 0) {
															m->Aim_p = Aim * (1.f / (float)aimcnt);
														}
													}
													//m->Zrad1buf_p = -radn * 1.f;
													m->OLDVec_p = m->Aim_p - m->OLDPos_p;
													m->OLDVec_p.y(0);
													m->OLDDist = m->OLDVec_p.size();
													m->OLDVec_p = m->OLDVec_p.Norm();
													m->OLDPos_p = m->pos_p;

												}
												//
											}
											else {
												//
												if (length <= 20.f) {
													m->GuideNum[0] = -1;
													m->Yradadd_p = 0.f;
												}
												//
											}
										}
										else {
											m->Yradadd_p = 0.f;
										}

										m->Yrad1_p += m->Yradadd_p;

										easing_set(&m->Zrad1_p, std::clamp(m->Zrad1buf_p, -10.f, 10.f), 0.95f);
										easing_set(&m->Zrad1buf_p, m->Yradaddbuf_p*50.f, 0.95f);

										m->pos_p += MATRIX_ref::Vtrans(VECTOR_ref::vget(0, 0, m->pos_z_p), MATRIX_ref::RotY(deg2rad(m->Yrad1_p)))*(1.f / FPS * GameSpeed);

										auto total = -19.4f * 12.5f;
										if (m->GuideNum[0] >= 0) {
											if (m->pos_z_p >= total + GetRandf(1.0f*12.5f) - m->TurboSpeed) { m->pos_z_p += -0.6f; }
											else { m->pos_z_p -= -0.6f; }
										}
										else {
											if (m->pos_z_p <= 0.f) { m->pos_z_p -= -1.2f; }
										}
										//anim
										{
											if (inf.animsel <= 2) {
												if (m->pos_z_p <= total * 2 / 3) {
													inf.animsel = 2;
												}
												else  if (m->pos_z_p <= total / 3) {
													inf.animsel = 1;
												}
												else  if (m->pos_z_p <= 0) {
													inf.animsel = 0;
												}
											}
										}
										m->animspd = m->pos_z_p / total + GetRandf(0.05f);
										//debug
										if (m->footflag) {
											if (!m->isOutFrustum) {
												if (m->footLR == 0) {
													auto f = m->GetFrame("右つま先");
													if (f.y() - m->pos_p.y() <= 0.1f) {
														m->footEffPos = f;
														m->footLR = 1;
														m->footflag = false;
														m->footok = true;
													}
												}
												else {
													auto f = m->GetFrame("左つま先");
													if (f.y() - m->pos_p.y() <= 0.1f) {
														m->footEffPos = f;
														m->footLR = 0;
														m->footflag = false;
														m->footok = true;
													}
												}
											}
										}
										if (m->xposition > xof + GetRandf(500.f)) {
											m->xposition = std::max(m->xposition - 10.f / FPS * GameSpeed, 0.f);
										}
										else {
											m->xposition += 10.f / FPS * GameSpeed;
										}
									}
									xof += 16.f;
								}
								for (auto& n : NAMES) {
									auto* m = models.Get(n, 0);
									if (!m->footflag) {
										if (Effect_UseControl::Check_FootEffectCnt() <= 2) {
											if (m->footok) {
												m->footok = false;
												Effect_UseControl::Set_FootEffect(m->footEffPos, VECTOR_ref::up(), 5.f);
												m->foottime = 0.1f;
											}
										}
										if (!m->footok) {
											if (m->foottime <= 0.f) { m->footflag = true; }
											m->foottime -= 1.f / FPS * GameSpeed;
										}
									}
								}
								//当たり判定
								for (auto& n : NAMES) {
									auto* m = models.Get(n, 0);
									m->Nearest = VECTOR_ref::up()*10000.f;
									for (auto& n2 : NAMES) {
										auto* m2 = models.Get(n2, 0);
										if (n != n2) {
											auto vec = m2->pos_p - m->pos_p;
											if (vec.size() > m->Nearest.size()) {
												m->NearID = n2;
												m->Nearest = vec;
											}
										}
									}
								}

								for (auto& n : NAMES) {
									auto* m = models.Get(n, 0);
									auto& inf = m->CutDetail[m->Cutinfo.nowcut];
									auto zvb = MATRIX_ref::RotY(deg2rad(m->Yrad1_p)).zvec();
									//当たり判定
									if (m->Nearest.size() <= 15.f) {
										auto vec2 = m->Nearest.Norm() * 5.f*(1.f / FPS * GameSpeed);
										m->pos_p -= vec2 / 2.f;
										models.Get(m->NearID, 0)->pos_p += vec2 / 2.f;
									}
									//ターボ開始
									if (!m->Turbook && inf.animsel == 2) {
										m->Turbook = true;
									}
									//ターボしてないとき振り向き判定
									if (!m->Turbook) {
										if (!m->Lookok && inf.animsel == 2) {
											if (m->Nearest.size() <= 60.f) {
												auto vec2 = m->Nearest.Norm();
												auto LR = -zvb.cross(vec2).y();
												auto FR = zvb.dot(vec2);
												if (FR < 0) {
													m->LookLR = LR > 0;
													m->Lookok = true;
												}
											}
										}
									}
									//振り向き
									if (m->Lookok) {
										if (m->Looktime == 0.f) {
											if (m->obj.get_anime(inf.animsel).time == 0.0f) {
												m->Looktime = (float)(10 + GetRand(10));
												inf.animsel = m->LookLR ? 4 : 3;
											}
										}
										else {
											if (inf.animsel == 3 || inf.animsel == 4) {
												if (m->obj.get_anime(inf.animsel).time == 0.0f) {
													inf.animsel = 2;
												}
											}
											m->Looktime -= 1.f / FPS * GameSpeed;
											if (m->Looktime < 0.f) {
												m->Looktime = 0.f;
												m->Lookok = false;
											}
										}
									}
									//ターボ
									if (m->Turbook) {
										if (m->Turbotime == 0.f) {
											if (m->obj.get_anime(inf.animsel).time == 0.0f) {
												m->Turbotime = (float)(20 + GetRand(20));
												inf.animsel = 5;

											}
										}
										else {
											if (inf.animsel == 5) {
												if (m->obj.get_anime(inf.animsel).time == 0.0f) {
													inf.animsel = 2;
												}
											}
											m->Turbotime -= 1.f / FPS * GameSpeed;
											if (m->Turbotime < 0.f) {
												m->Turbotime = 0.f;
												m->Turbook = false;
											}

											easing_set(&m->TurboSpeed, m->Turbotime / 10.f, 0.9f);
										}
									}
									//
								}
								//座標の反映
								for (auto& n : NAMES) {
									auto* m = models.Get(n, 0);
									m->mat_p =
										MATRIX_ref::RotY(deg2rad(m->Yrad1_p))
										* MATRIX_ref::RotAxis(MATRIX_ref::RotY(deg2rad(m->Yrad1_p)).zvec(), deg2rad(m->Zrad1_p))
										* MATRIX_ref::Mtrans(m->pos_p) * MATRIX_ref::RotY(deg2rad(m->Yrad2_p));
									if (m->canUpdate) {
										m->obj.SetMatrix(m->mat_p);
									}
								}
								//
								float cam_near = 100000.f;
								float cam_far = 10.f;
								auto camv = m_CutInfo[m_Counter].Aim_camera.camvec - m_CutInfo[m_Counter].Aim_camera.campos;
								for (auto& n : NAMES) {
									auto* m = models.Get(n, 0);
									auto camp = m->pos_p - m_CutInfo[m_Counter].Aim_camera.campos;
									if (camv.dot(camp) > 0.f) {//カメラの向き
										auto dist = (camp).size() + 50.f;
										if (dist > cam_far) {
											cam_far = dist;
										}
										if (dist < cam_near) {
											cam_near = dist;
										}
									}
								}
								cam_far += 50.f;
								cam_near = std::max(cam_near - 100.f, 2.f);
								//cam
								{
									auto dist = (models.Get(NAMES[camsel], 0)->pos_p - m_CutInfo[m_Counter].Aim_camera.campos).size() + 50.f;
									//printfDx("FOV = %5.2f", 100.f / dist * 45.f);
									easing_set(&m_CutInfo[m_Counter].Aim_camera.fov, deg2rad(std::clamp(100.f / dist * 45.f, 1.f, 50.f)), 0.9f);

									if (cam_far > 6000.f) {
										cam_near = std::clamp(cam_far / 2.f, 2.f, 3000.f);
									}
									else if (cam_far > 4000.f) {
										cam_near = std::clamp(cam_far / 3.f, 2.f, 2000.f);
									}
									else if (cam_far > 2000.f) {
										cam_near = std::clamp(cam_far / 6.f, 2.f, 2000.f);
									}
									else if (cam_far > 1000.f) {
										cam_near = std::clamp(cam_far / 7.f, 2.f, 2000.f);
									}
									else {
										cam_near = std::min(cam_near, std::clamp(cam_far / 7.f, 2.f, 1000.f));
									}
									printfDx("FAR  = %.2f\n", cam_far);
									printfDx("NEAR = %.2f\n", cam_near);

									easing_set(&m_CutInfo[m_Counter].Aim_camera.near_, cam_near, 0.5f);
									easing_set(&m_CutInfo[m_Counter].Aim_camera.far_, cam_far, 0.5f);
								}
								//
							}
							//
							ChangeCamSel.GetInput(CheckHitKey(KEY_INPUT_DOWN) != 0);
							if (ChangeCamSel.trigger()) {
								m_CutInfoUpdate[m_Counter].Forcus.Set(NAMES[camsel], 0, "NECK", VECTOR_ref::zero());
								++camsel %= (int)(NAMES.size());
							}
						}
						SEL++;
					}
					//
					{
						auto& u = m_CutInfoUpdate[m_Counter];
						if (isFirstLoop) {
							if (m_Counter > 0) {
								m_CutInfo[m_Counter].SetPrev(m_CutInfo[m_Counter - 1]);
							}
							if (u.IsUsePrevBuf) {
								//
								auto White_Set = u.IsSetWhite;
								auto White_Per = u.White_Per;
								auto White = u.White;

								auto Black_Set = u.IsSetBlack;
								auto Black_Per = u.Black_Per;
								auto Black = u.Black;
								//
								u = m_CutInfoUpdate[m_Counter - 1];
								//
								if (White_Set) {
									u.IsSetWhite = White_Set;
									u.White_Per = White_Per;
									u.White = White;
								}
								if (Black_Set) {
									u.IsSetBlack = Black_Set;
									u.Black_Per = Black_Per;
									u.Black = Black;
								}
							}
							//
							if (m_CutInfo[m_Counter].bright[0] >= 0) {
								PostPassParts->Set_Bright(m_CutInfo[m_Counter].bright[0], m_CutInfo[m_Counter].bright[1], m_CutInfo[m_Counter].bright[2]);
							}

							if (m_CutInfo[m_Counter].fog[0] >= 0) {
								fog[0] = m_CutInfo[m_Counter].fog[0];
								fog[1] = m_CutInfo[m_Counter].fog[1];
								fog[2] = m_CutInfo[m_Counter].fog[2];
								fog_range[0] = m_CutInfo[m_Counter].fog_range[0];
								fog_range[1] = m_CutInfo[m_Counter].fog_range[1];
							}
							else if (m_CutInfo[m_Counter].fog[0] == -2) {
								fog[0] = 128;
								fog[1] = 128;
								fog[2] = 128;
								fog_range[0] = 200.f;
								fog_range[1] = 300000.f;
							}
							//
							if (m_CutInfo[m_Counter].Forcus.GetIsUse()) {
								m_CutInfo[m_Counter].Aim_camera.camvec = m_CutInfo[m_Counter].Forcus.GetForce(models);
							}
							//
							if (attached.GetSwitch()) {
								m_CutInfo[m_Counter].Aim_camera.campos = m_CutInfo[m_Counter].Aim_camera.camvec + attachedDetail[attached.nowcut].poscam;
							}
							if (m_CutInfo[m_Counter].isResetRandCampos) { m_RandcamposBuf.clear(); }
							if (m_CutInfo[m_Counter].isResetRandCamvec) { m_RandcamvecBuf.clear(); }
							if (m_CutInfo[m_Counter].isResetRandCamup) { m_RandcamupBuf.clear(); }
						}
						else {
							u.Update(m_CutInfo[m_Counter], models, m_RandcamupBuf, m_RandcamvecBuf, m_RandcamposBuf);

							u.CameraNotFirst.camvec += u.CameraNotFirst_Vec.camvec*(1.f / FPS * GameSpeed);
							u.CameraNotFirst.campos += u.CameraNotFirst_Vec.campos*(1.f / FPS * GameSpeed);

							//
							if (attached.GetSwitch()) {
								m_CutInfo[m_Counter].Aim_camera.campos = m_CutInfo[m_Counter].Aim_camera.camvec + attachedDetail[attached.nowcut].poscam;
							}

							easing_set_SetSpeed(&Black_Buf, u.Black, u.Black_Per);
							easing_set_SetSpeed(&White_Buf, u.White, u.White_Per);
						}
					}
					//
					easing_set_SetSpeed(&camera_buf.campos, m_CutInfo[m_Counter].Aim_camera.campos, m_CutInfo[m_Counter].cam_per);
					easing_set_SetSpeed(&camera_buf.camvec, m_CutInfo[m_Counter].Aim_camera.camvec, m_CutInfo[m_Counter].cam_per);
					easing_set_SetSpeed(&camera_buf.camup, m_CutInfo[m_Counter].Aim_camera.camup, m_CutInfo[m_Counter].cam_per);
					easing_set_SetSpeed(&camera_buf.fov, m_CutInfo[m_Counter].Aim_camera.fov, m_CutInfo[m_Counter].cam_per);
					easing_set_SetSpeed(&camera_buf.far_, m_CutInfo[m_Counter].Aim_camera.far_, m_CutInfo[m_Counter].cam_per);
					easing_set_SetSpeed(&camera_buf.near_, m_CutInfo[m_Counter].Aim_camera.near_, m_CutInfo[m_Counter].cam_per);
					camera_buf.set_cam_info(camera_buf.fov, camera_buf.near_, camera_buf.far_);
					//
					isFreepos = (CheckHitKey(KEY_INPUT_RETURN) != 0);
					int x_o = x_m;
					int y_o = y_m;
					if (isFreepos) {
						x_o = x_sav;
						y_o = y_sav;
					}
					GetMousePoint(&x_m, &y_m);
					if (!isFreepos) {
						camera_main = camera_buf;
						x_sav = -1;
						y_sav = -1;
					}
					else {
						x_rm = std::clamp(x_rm + (float)(y_m - y_o) / 10.f, -20.f, 80.f);
						y_rm += (float)(x_m - x_o) / 10.f;

						r_rm = std::clamp(r_rm + (float)(GetMouseWheelRotVol())*10.f, 5.f, 6000.f);

						camera_main.set_cam_info(deg2rad(45), 1.f, 1000.f);
						camera_main.camvec = VECTOR_ref::vget(0, 0, 0);
						camera_main.campos = camera_main.camvec + GetVector(x_rm, y_rm)*r_rm;

						if (x_sav == -1) {
							x_sav = x_m;
							y_sav = y_m;
						}
						SetMousePoint(x_sav, y_sav);
					}
				}
				graphs.Update();
				models.Update();
				{
					//models.SetPhysics(ResetPhysics || ModelEdit_PhysicsReset);
					ModelEdit_PhysicsReset = false;
					isFirstLoop = false;
					if (NowTimeWait > m_CutInfo[m_Counter%m_CutInfo.size()].GetTimeLimit()) {
						isFirstLoop = true;
						if (m_CutInfo[m_Counter].IsResetPhysics) {
							ResetPhysics = true;
						}
						++m_Counter;
					}
					else {
						ResetPhysics = false;
					}
				}
				Effect_UseControl::Update_Effect();
				//if (!BGM.check()) { return false; }
				return true;
			}
			void Dispose(void) noexcept override {
				Effect_UseControl::Dispose_Effect();
				m_CutInfo.clear();
				m_CutInfoUpdate.clear();
				movie.Dispose();
				BGM.Dispose();
				grassmodel.Dispose();
			}
			//
			void UI_Draw(void) noexcept  override {
				if (!isFreepos && NowTimeWait > 0) {
					TLClass.Draw(NowTimeWait);
				}
			}
			//他 32
			void BG_Draw(void) noexcept override {
				//+3
				models.Draw_Far();
			}
			//
			void Shadow_Draw_Far(void) noexcept {
				//特殊
				//+12
				SetDrawAlphaTest(DX_CMP_GREATER, 128);
				models.Draw(false, true);
				SetDrawAlphaTest(-1, 0);
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				//todo:共通の影モデルを使用
				//models.Draw(false, false, TRUE);
			}
			void Shadow_Draw(void) noexcept override {
				//+52
				models.Draw(true, false, FALSE);
			}
			void Main_Draw(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();

				if (!isFreepos) {
					SetFogEnable(TRUE);
					SetFogDensity(0.01f);
					SetFogColor(fog[0], fog[1], fog[2]);
					SetFogStartEnd(fog_range[0], fog_range[1]);
				}

				models.CheckInCamera(camera_main.far_);
				//+201 = 67x3
				auto camfar = GetCameraFar();
				//カメラのfarが適正範囲の設定の場合
				//near
				if (camera_main.near_ - 1.f < camfar&& camfar < camera_main.near_ + 1.f) {
				}
				else if (camera_main.far_ - 1.f < camfar&& camfar < camera_main.far_ + 1.f) {
					grassmodel.Check_CameraViewClip();
					models.Draw(false, false, FALSE);
				}
				//far
				else {
					grassmodel.Check_CameraViewClip();
					models.Draw(false, false, TRUE);
				}
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
					if (Black_Buf != 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*Black_Buf));
						DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_y, GetColor(0, 0, 0), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					if (White_Buf != 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*White_Buf));
						DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_y, GetColor(255, 255, 255), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					graphs.Draw(DrawParts->disp_y);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				for (int i = 0; i < Guide.frame_num(); i++) {
					auto pos = Guide.frame(i);
					DrawLine3D((pos).get(), (pos + VECTOR_ref::up()*20.f).get(), GetColor(255, 0, 0));
				}
				for (auto& n : NAMES) {
					auto* m = models.Get(n, 0);
					auto pos = m->pos_p;
					auto pos2 = m->Aim_p;
					DrawLine3D((pos + VECTOR_ref::up()*5.f).get(), (pos2 + VECTOR_ref::up()*5.f).get(), GetColor(0, 0, 255));

					pos = Guide.frame(m->GuideNum[0]);
					DrawSphere3D((pos + VECTOR_ref::up()*20.f).get(), 1.f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);
				}
				grassmodel.Draw(camera_buf);
			}
			//
			void LAST_Draw(void) noexcept override {
				if (LookMovie.on()) {
					auto* DrawParts = DXDraw::Instance();
					movie.DrawExtendGraph(DrawParts->disp_x * 3 / 4, DrawParts->disp_y * 3 / 4, DrawParts->disp_x, DrawParts->disp_y, FALSE);
				}
				Editer_Calc();

				Editer_Draw();
			}
		};
	};
};