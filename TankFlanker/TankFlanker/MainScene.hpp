#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
			class Change_Model {
				ModelControl::Model* ChangeModel{ nullptr };
				int SetAnimStart{ -1 };
				int SetAnimEnd{ -1 };
			private:
				void Reset(void) noexcept {
					ChangeModel = nullptr;
					SetAnimStart = -1;
					SetAnimEnd = -1;
				}
			public:
				const auto IsChangeModel(const ModelControl::Model* tgt) const noexcept { return (tgt != nullptr && ChangeModel == tgt); }
				const auto IsAnimIn(int In) const noexcept { return SetAnimStart <= In && In <= SetAnimEnd; }
				//
				void Set(ModelControl::Model* tmp, int value) noexcept {
					if (ChangeModel == nullptr) { ChangeModel = tmp; }
					if (SetAnimStart == -1) { SetAnimStart = value; }
					SetAnimEnd = std::max(value, SetAnimStart);
				}
				//被りチェック
				void Check(void) noexcept {
					if (this->ChangeModel != nullptr) {
						//修正
						for (const auto& c : this->ChangeModel->Cutinfo.Switch) {
							if (this->SetAnimStart >= c.Off) { this->SetAnimStart = std::max(this->SetAnimStart, c.Off + 1); }
							if (this->SetAnimStart < c.On) { this->SetAnimEnd = std::min(this->SetAnimEnd, c.On - 1); }
						}
						//確認
						for (const auto& c : this->ChangeModel->Cutinfo.Switch) {
							if (this->SetAnimStart == c.On || this->SetAnimEnd == c.Off) {
								this->Reset();
								break;
							}
						}
					}
				}
				//Create
				bool Create(void) noexcept {
					bool isResetPhusics = false;
					if (this->ChangeModel != nullptr) {
						for (const auto& c : this->ChangeModel->Cutinfo.Switch) {
							if (this->SetAnimStart < c.On) {
								int id = (int)(&c - &this->ChangeModel->Cutinfo.Switch.front());
								this->ChangeModel->Cutinfo.Insert(id, this->SetAnimStart, this->SetAnimEnd);
								this->ChangeModel->CutDetail.insert(this->ChangeModel->CutDetail.begin() + id, CutinfoDetail());
								auto& info = this->ChangeModel->CutDetail[id];
								info.animsel = 0;
								info.isloop = true;
								info.animspeed = 1.0f;
								info.startframe = 0.f;
								isResetPhusics = true;
								break;
							}
						}
						this->Reset();
					}
					return isResetPhusics;
				}
			};
			class Edit_Model {
			public:
				ModelControl::Model* m_Ptr{ nullptr };
				size_t m_CunNum{ 0 };
				bool m_IsActive = false;

				const auto IsEditModel(const ModelControl::Model* tgt, size_t id) const noexcept { return (this->m_IsActive && m_Ptr == tgt && m_CunNum == id); }
				bool IsEditMode() { return this->m_Ptr != nullptr && this->m_IsActive; }
				bool Switch() {
					if (this->m_Ptr != nullptr && !m_IsActive) {
						m_IsActive = true;
						return true;
					}
					return false;
				}
				void SetModelEdit(ModelControl::Model* tmp, int value) noexcept {
					m_Ptr = tmp;
					m_CunNum = value;
				}
				void ResetModelEdit(void) noexcept {
					m_Ptr = nullptr;
					m_CunNum = 0;
				}
				auto GetModeName(void) const noexcept {
					std::string mode = "NORMAL";
					if (this->m_Ptr->isBGModel) {
						mode = ModelType[0];
					}
					else if (!this->m_Ptr->IsNearShadow) {
						mode = ModelType[1];
					}
					else if (this->m_Ptr->IsFarShadow) {
						mode = ModelType[2];
					}
					return mode;
				}
				void ChangeMode(void) noexcept {
					if (
						!this->m_Ptr->isBGModel &&
						this->m_Ptr->IsNearShadow &&
						!this->m_Ptr->IsFarShadow
						) {
						//ノーマルならBGモデルに
						this->m_Ptr->isBGModel = true;
					}
					else if (
						this->m_Ptr->isBGModel &&
						this->m_Ptr->IsNearShadow &&
						!this->m_Ptr->IsFarShadow
						) {
						//BGなら近距離影なしモデルに
						this->m_Ptr->isBGModel = false;
						this->m_Ptr->IsNearShadow = false;
					}
					else if (
						!this->m_Ptr->isBGModel &&
						!this->m_Ptr->IsNearShadow &&
						!this->m_Ptr->IsFarShadow
						) {
						//近距離影なしなら遠距離影なしモデルに
						this->m_Ptr->IsNearShadow = true;
						this->m_Ptr->IsFarShadow = true;
					}
					else if (
						!this->m_Ptr->isBGModel &&
						this->m_Ptr->IsNearShadow &&
						this->m_Ptr->IsFarShadow
						) {
						//遠距離影なしならノーマルモデルに
						this->m_Ptr->IsFarShadow = false;
						this->m_Ptr->isBGModel = false;
					}
					else {
						//何か問題があったらnormalに
						this->m_Ptr->isBGModel = false;
						this->m_Ptr->IsNearShadow = true;
						this->m_Ptr->IsFarShadow = false;
					}
				}
			};
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
				CharaInfoEdit(void) noexcept {}
				~CharaInfoEdit(void) noexcept {}

				void Set(std::string_view name, int x_1, int y_1, int xsize, int ysize, int Thickness) noexcept {
					Name = name;
					x1 = x_1;
					y1 = y_1;
					xs = xsize;
					ys = ysize;
					thick = Thickness;
				}

				void Update(std::function<void()> DoInClick, std::string_view info) noexcept {
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
		private:
			std::string SUN = "data/model/sun/model.mv1";
			std::string MAP = "data/model/map/model.mv1";
			std::string GATE = "data/model/map/model_gate.mv1";
			//
			cam_info camera_buf;									//
			int fog[3]{ -1,-1,-1 };									//
			float fog_range[2]{ -1.f,-1.f };						//
			std::vector<size_t> RankID;								//
			size_t camsel{ 0 }, camsel_buf{ 0 };					//
			float Per_Change{ 1.f };								//
			float Change1Time{ 1.f };								//
			size_t m_Counter{ 0 };									//カット
			std::vector<Cut_Info_First> m_CutInfo_Buf;				//
			std::vector<Cut_Info_Update> m_CutInfoUpdate_Buf;		//
			LONGLONG BaseTime{ 0 }, WaitTime{ 0 }, NowTimeWait{ 0 };//
			bool ResetPhysics{ true };								//
			bool isFirstLoop{ true };								//カット最初のループか
			bool isfast{ true };									//
			bool issecond{ true };									//
			bool attached_override{ true };							//
			SoundHandle BGM;										//データ

			std::vector<GraphHandle> boards;

			int BGM_Frequency;										//
			switchs ChangeCamSel, ChangeStart;						//
			switchs SpeedUp, SpeedDown;								//
			switchs Start;											//
			switchs MouseClick;										//
			int spd_x{ 10 };										//
			bool isFreepos{ false };								//
			int x_m{ 0 }, y_m{ 0 };									//
			int x_sav{ 0 }, y_sav{ 0 };								//
			float x_rm{ 0.f }, y_rm{ 0.f }, r_rm{ 100.f };			//
			size_t CutSel_Buf{ 0 };									//
			VECTOR_ref m_RandcamupBuf;								//
			VECTOR_ref m_RandcamvecBuf;								//
			VECTOR_ref m_RandcamposBuf;								//
			float Black_Buf{ 0.f };									//
			float White_Buf{ 0.f };									//


			std::string AirCarrier = "data/model/aircraftcarrier/model.mv1";
			std::string TomCat = "data/model/F14/model.mv1";
			struct F14Args_Wheel {
				float Rad{ 0.f };
				VECTOR_ref Prev;
			};
			struct F14Args {
				float pWingOpen{ 1.f }, pWopentime{ 0.f };
				float pCheck{ 0.f };
				std::vector<F14Args_Wheel> Wheels;

				int FrontWheel = -1;
				float FrontYBase = 0.f;

				moves move;
				float Yrad, YradAdd;
				float Xrad, XradAim;
				float Yadd, Zadd;
				bool AimFlag = false;
				bool AimFlag2 = false;
				VECTOR_ref Aim;

				bool TaxStop = false;
				float DiffTimer{ 0.f };
				bool DiffOn = false;

				float StartTimer{ 0.f };
			};
			std::array<F14Args, 64> Check_F14;
			float DiffPer = 0.f;

			MATRIX_ref Frontmat;
			struct F14Wheels {
				int sel{ 0 };
				MATRIX_ref mat;
			};
			std::vector<F14Wheels> WheelFrames;

#define EditMode
#ifdef EditMode
			//エディター
		private:
			GraphHandle movie;										//
			switchs LookMovie;										//
			switchs LookEditer;										//ビュワー
			bool ModelEdit_PhysicsReset{ false };					//
			bool ModelEditIn{ false };								//
			bool PressSeek{ false };								//
			std::vector<CharaInfoEdit> CharaEdit;					//
			int BaseWidth{ 1920 / 64 };								//
			int changeSeekID{ -1 };									//
			int x_now{ -1 };										//
			int WidthPer{ 600000 };								//
			int X_now{ 0 };											//
			int CutNow{ 0 };										//
			Change_Model m_ChangeModel;								//
			Edit_Model m_EditModel;									//
		private:
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
								}
							}
							i++;
							if (i >= m_CutInfo.size()) { break; }
						}
					}
				}
				else {
					if (PressSeek) {
						PressSeek = false;
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
						changeSeekID = -1;
						if (Start.on()) {
							ModelEditIn = true;
						}
					}
				}
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

				LookEditer.Init(false);
				LookMovie.Init(false);

				movie = GraphHandle::Load("data/base_movie2.mp4");
				PauseMovieToGraph(movie.get());
			}
			void Editer_Calc(void)noexcept {
				auto* DrawParts = DXDraw::Instance();

				int mouse_x, mouse_y;
				GetMousePoint(&mouse_x, &mouse_y);

				BaseWidth = DrawParts->disp_x / 64;

				LookMovie.GetInput(CheckHitKey(KEY_INPUT_M) != 0);
				LookEditer.GetInput(CheckHitKey(KEY_INPUT_N) != 0);


				if (LookMovie.trigger() && LookMovie.on()) {
					PlayMovieToGraph(movie.get(), 2, DX_MOVIEPLAYTYPE_BCANCEL);
					ChangeMovieVolumeToGraph(0, movie.get());
					SeekMovieToGraph(movie.get(), (int)(NowTimeWait / 1000));
				}
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
						if (!m_EditModel.m_IsActive) {
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
																m_EditModel.SetModelEdit(tmp, (int)(&c - &tmp->Cutinfo.Switch.front()));
																break;
															}
														}
														if (!EditModelInfo || m_EditModel.m_Ptr == nullptr) {
															m_ChangeModel.Set(tmp, i);
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
									m_ChangeModel.Check();
									//登録できるなら作成
									if (m_ChangeModel.Create()) {
										if (m_ChangeModel.IsAnimIn((int)m_Counter)) {
											ModelEdit_PhysicsReset = true;
										}
									}
									else if (m_EditModel.Switch() && Start.on()) {
										ModelEditIn = true;
									}
								}
							}
						}
						//モデルエディットモード判定(参照変更)
						if (m_EditModel.IsEditMode() && MouseClick.press()) {
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
													m_EditModel.SetModelEdit(tmp, (int)(&c - &tmp->Cutinfo.Switch.front()));
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
						//
					}
					//モデルエディットモード
					if (m_EditModel.IsEditMode()) {
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
								m_EditModel.ResetModelEdit();
								m_EditModel.m_IsActive = false;
								/*
								if (!Start.on()) {
									ModelEditIn = true;
								}
								//*/
							}
						}
						//info
						if (m_EditModel.m_IsActive) {
							auto& c = m_EditModel.m_Ptr->CutDetail[m_EditModel.m_CunNum];
							//AnimeSel
							CharaEdit[0].Update([&]() {
								if (MouseClick.trigger()) {
									++c.animsel %= m_EditModel.m_Ptr->obj.get_anime().size();
									ModelEdit_PhysicsReset = true;
								}
							}, std::to_string(c.animsel));
							//DrawMode
							CharaEdit[1].Update([&]() {
								if (MouseClick.trigger()) { m_EditModel.ChangeMode(); }
							}, m_EditModel.GetModeName());
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
							std::string mode;
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
											if (!m_EditModel.m_IsActive && in2_(mouse_x, mouse_y, xx, y_p, x1 + width_Next, y_p + y_s - 1)) {
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

										if (!m_EditModel.m_IsActive && in2_(mouse_x, mouse_y, x1, y_p + y_s, x1 + width_Next, y_p + y_s + y_hight)) {
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
									if (m_ChangeModel.IsChangeModel(&m) && m_ChangeModel.IsAnimIn(i)) {
										if (x1 < x1 + width_Next) {
											DrawBox(x1, y1 + yp, x1 + width_Next, y1 + hight - yp, GetColor(255, 255, 0), FALSE);
										}
									}
									for (const auto& c : m.Cutinfo.Switch) {
										if (c.IsIn(i)) {
											int x_b2 = std::max(x1 + ((i == c.On) ? p2 : 0), x_p);
											int x_b1 = std::max(x1 + ((i == c.On) ? xp : 0), x_p);
											int x_b4 = std::min(x1 + width_Next - ((i == c.Off) ? p2 : 0), x_p + x_s);
											int x_b3 = std::min(x1 + width_Next - ((i == c.Off) ? xp : 0), x_p + x_s);

											unsigned int color;
											if (m_EditModel.IsEditModel(&m, &c - &m.Cutinfo.Switch.front())) {
												color = GetColor(255, 255, 0);
											}
											else {
												if (m.isBGModel) {
													color = c.IsIn(m_Counter) ? GetColor(100, 216, 100) : GetColor(60, 60, 192);
												}
												else {
													color = c.IsIn(m_Counter) ? GetColor(150, 255, 150) : GetColor(100, 100, 255);
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
						//NOWline
						if (x_now >= 0) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
							DrawBox(x_p, y_p, x_now + X_now + width_Time, y_p + y_s, GetColor(0, 0, 0), TRUE);
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
							DrawLine(x_now + X_now + width_Time, y_p, x_now + X_now + width_Time, y_p + y_s, GetColor(255, 255, 255), 3);
						}
						//OverRay
						if (m_EditModel.m_IsActive) {
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
					if (m_EditModel.IsEditMode()) {
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
									const auto* sel = LSClass.GetArgFromPath(m_EditModel.m_Ptr->Path);
									if (sel != nullptr) {
										Fonts.Get(hight).Get_handle().DrawStringFormat(x_p + x_s / 2, y1, GetColor(255, 255, 255), sel->Base + "(" + std::to_string(m_EditModel.m_Ptr->BaseID) + ")");
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
				/*
				for (const auto& m : models.GetModel()) {
					const auto* sel = LSClass.GetArgFromPath(m.Path);
					if (sel != nullptr) {
						printfDx((" " + sel->Base + "(" + std::to_string(m.BaseID) + ") = %d\n").c_str(), m.DrawCount);
					}
				}
				//*/

				if (LookMovie.on()) {
					movie.DrawExtendGraph(DrawParts->disp_x * 3 / 4, DrawParts->disp_y * 1 / 5, DrawParts->disp_x, DrawParts->disp_y, FALSE);
				}
			}

			void Editer_Dispose() {
				movie.Dispose();
			}
#endif
		public:
			//Getter
			bool Time_Over(void) const noexcept { return m_Counter >= m_CutInfo.size(); }
		public:
			using TEMPSCENE::TEMPSCENE;
			void Set(void) noexcept override {
				auto* PostPassParts = PostPassEffect::Instance();
				//
				camera_buf.campos = VECTOR_ref::vget(0, 20, -20);
				camera_buf.camvec = VECTOR_ref::vget(0, 20, 0);
				camera_buf.camup = VECTOR_ref::up();
				camera_buf.set_cam_info(deg2rad(15), 1.f, 200.f);
				fog[0] = 128;
				fog[1] = 128;
				fog[2] = 128;
				fog_range[0] = 200.f;
				fog_range[1] = 300000.f;
				//

				TEMPSCENE::Set_EnvLight(VECTOR_ref::vget(5000.f, 50.f, 5000.f), VECTOR_ref::vget(-5000.f, -10.f, -5000.f), VECTOR_ref::vget(-0.3f, -0.5f, -0.2f), GetColorF(0.42f, 0.41f, 0.40f, 0.f));
				TEMPSCENE::Set();
				models.Get(SUN, 0)->obj.SetMatrix(MATRIX_ref::RotVec2(VECTOR_ref::up(), (VECTOR_ref)(Get_Light_vec().Norm())) * MATRIX_ref::Mtrans(Get_Light_vec().Norm() * -1500.f));
				//m_Counter = 25;
				m_Counter = 0;
				models.Start(m_Counter);
				graphs.Start(m_Counter);
				attached.Start(m_Counter);

				SetUseASyncLoadFlag(FALSE);
				BGM = SoundHandle::Load("data/sound.wav");
				//BGM.vol(0);


				boards.resize(boards.size() + 1);
				boards.back() = GraphHandle::Load("data/picture/amazon.png");
				boards.resize(boards.size() + 1);
				boards.back() = GraphHandle::Load("data/picture/teio.png");
				boards.resize(boards.size() + 1);
				boards.back() = GraphHandle::Load("data/picture/mac.png");

				boards.resize(boards.size() + 1);
				boards.back() = GraphHandle::Load("data/picture/brian.png");

				boards.resize(boards.size() + 1);
				boards.back() = GraphHandle::Load("data/picture/laurel.png");
				//プレイ用意
				GameSpeed = (float)(spd_x) / 10.f;
				PostPassParts->Set_Bright(255, 240, 234);
				BaseTime = GetMocroSec() - (m_Counter > 0 ? m_CutInfo[m_Counter - 1].GetTimeLimit() : 0);
				WaitTime = (m_Counter != 0) ? 0 : 1000000;
				NowTimeWait = -WaitTime;
				m_RandcamupBuf.clear();
				m_RandcamvecBuf.clear();
				m_RandcamposBuf.clear();
				ResetPhysics = true;
				Start.Init(true);
				m_CutInfo_Buf = m_CutInfo;
				m_CutInfoUpdate_Buf = m_CutInfoUpdate;
#ifdef EditMode
				Editer_Init();
#endif
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
				bool isEditActive = true;
#ifdef EditMode
				isEditActive = m_EditModel.m_IsActive;
				Start.GetInput((!isEditActive && CheckHitKey(KEY_INPUT_SPACE) != 0) || ModelEditIn);
				ModelEditIn = false;
#else
				Start.GetInput(CheckHitKey(KEY_INPUT_SPACE) != 0);
#endif
				if ((!isEditActive && (SpeedUp.trigger() || SpeedDown.trigger())) || Start.trigger()) {
					if (SpeedUp.trigger()) { spd_x++; }
					if (SpeedDown.trigger()) { spd_x--; }
					if (Start.trigger()) { spd_x = Start.on() ? 10 : 0; }
					spd_x = std::clamp(spd_x, 0, 20);
					GameSpeed = (float)(spd_x) / 10.f;
					if (NowTimeWait >= 0) {
						if (GameSpeed >= 0.1f) {
							//SetSoundCurrentTime((LONGLONG)(NowTimeWait / 1000), BGM.get());
							SetFrequencySoundMem((int)((float)BGM_Frequency * GameSpeed), BGM.get());
							if (!BGM.check()) {
								BGM.play(DX_PLAYTYPE_BACK, FALSE);
							}
						}
						else {
							BGM.stop();
						}
					}
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
						BGM.vol(192);
						//SetSoundCurrentTime((LONGLONG)(NowTimeWait / 1000), BGM.get());

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
						}
						SEL++;

						//
						{
							if (isFirstLoop && m_Counter == 0) {
								//
								WheelFrames.clear();
								int FrontWheel = 0;
								for (int f = 0; f < models.Get(TomCat, 0)->obj.frame_num(); f++) {
									if (models.Get(TomCat, 0)->obj.frame_name(f).find("回転") != std::string::npos) {
										WheelFrames.resize(WheelFrames.size() + 1);
										WheelFrames.back().sel = f;

										auto f2 = models.Get(TomCat, 0)->obj.frame_parent(f);
										if (models.Get(TomCat, 0)->obj.frame_name(f2).find("先") != std::string::npos) {
											FrontWheel = (int)f2;
										}
									}
								}
								//
								for (int i = 0; i < 1; i++) {
									Check_F14[i].pWingOpen = 1.f;
									Check_F14[i].pWopentime = 0.5f;
									Check_F14[i].Wheels.resize(WheelFrames.size());
									Check_F14[i].FrontWheel = FrontWheel;
									Check_F14[i].move.pos.Set(-300.f, 0.f, 0.f);

									Check_F14[i].Xrad = 0.f;
									Check_F14[i].XradAim = 0.f;
									Check_F14[i].Yrad = -90.f;
									Check_F14[i].YradAdd = 0.f;

									Check_F14[i].AimFlag = false;
									Check_F14[i].AimFlag2 = false;
									Check_F14[i].TaxStop = false;
									Check_F14[i].DiffOn = false;
									Check_F14[i].DiffTimer = 3.f;
									Check_F14[i].Yadd = 0.f;
									Check_F14[i].Zadd = 0.f;

									Check_F14[i].StartTimer = 3.f;
									DiffPer = 0.f;
								}
								//
							}

							auto* A = models.Get(AirCarrier, 0);
							for (int i = 0; i < 1; i++) {
								auto* M = models.Get(TomCat, i);
								if (!Check_F14[i].AimFlag2) {
									if (!Check_F14[i].AimFlag) {
										if (Check_F14[i].Yrad < -5.f) {
											easing_set(&Check_F14[i].YradAdd, 9.f, 0.95f);
										}
										else {
											easing_set(&Check_F14[i].YradAdd, 0.f, 0.95f);
											if (Check_F14[i].YradAdd <= 0.01f) {
												Check_F14[i].YradAdd = 0.f;
												Check_F14[i].AimFlag = true;
												Check_F14[i].Aim = A->obj.frame(47);
											}
										}
									}
									else {
										auto aim = (Check_F14[i].Aim - Check_F14[i].move.pos);
										if (
											(!Check_F14[i].TaxStop && aim.size() > 5.f) ||
											(Check_F14[i].TaxStop && aim.size() > 50.f)
											) {
											aim.y(0);
											auto zvec = MATRIX_ref::RotY(deg2rad(Check_F14[i].Yrad)).zvec();
											zvec.y(0);
											auto zasin = zvec.Norm().cross(aim.Norm()).y();
											if (zasin > 0.01f) {
												easing_set(&Check_F14[i].YradAdd, -5.f, 0.9f);
											}
											else if (zasin < -0.01f) {
												easing_set(&Check_F14[i].YradAdd, 5.f, 0.9f);
											}
											else {
												easing_set(&Check_F14[i].YradAdd, 0.f, 0.95f);
												if (Check_F14[i].TaxStop && !Check_F14[i].DiffOn) {
													Check_F14[i].DiffOn = true;
												}
											}
										}
										else {
											if (!Check_F14[i].TaxStop) {
												Check_F14[i].TaxStop = true;
												Check_F14[i].Aim = A->obj.frame(58);
											}
											else {
												Check_F14[i].Yadd = -100.f;
												Check_F14[i].AimFlag2 = true;
												Check_F14[i].YradAdd = 0.f;
											}
										}
									}
								}
								else {
									if (Check_F14[i].Yadd <= 100.f) {
										Check_F14[i].Yadd += 300.f / FPS * GameSpeed;
									}
								}
								//
								{
									auto OLD = Check_F14[i].Zadd;
									if (Check_F14[i].StartTimer > 0.f) {
										if (!Check_F14[i].TaxStop) {
											easing_set(&Check_F14[i].Zadd, -24.f, 0.95f);
										}
										else {
											easing_set(&Check_F14[i].Zadd, 0.f, 0.95f);
										}
									}
									else {
										float temp = -84.f*60.f;
										if (Check_F14[i].Zadd > temp) {
											Check_F14[i].Zadd += -5.f*60.f / FPS * GameSpeed;
										}
										else {
											Check_F14[i].Zadd = temp;
										}
									}
									if (!Check_F14[i].AimFlag2) {
										float temp = -std::abs(Check_F14[i].Zadd - OLD)*3.f;
										float rand = Check_F14[i].YradAdd / 2.f;
										float aim = std::clamp(temp + GetRandf(rand), -4.f, 4.f);
										easing_set(&Check_F14[i].XradAim, aim, 0.95f);
									}
									else {
										easing_set(&Check_F14[i].XradAim, 7.f, 0.95f);
									}
									if (isFirstLoop && m_Counter == 0) {
										Check_F14[i].XradAim = 0.f;
									}
									easing_set(&Check_F14[i].Xrad, Check_F14[i].XradAim, 0.95f);
								}
								if (Check_F14[i].DiffOn) {
									if (Check_F14[i].DiffTimer > 0.f) {
										Check_F14[i].DiffTimer -= 1.f / FPS * GameSpeed;
									}
									else {
										Check_F14[i].DiffTimer = 0.f;

										if (A->obj.get_anime(0).per <= 0.95f) {
											easing_set(&DiffPer, 0.5f / 60.f, 0.95f);
										}
										else {
											if (A->obj.get_anime(0).per >= 1.f) {
												A->obj.get_anime(0).per = 1.f;
												DiffPer = 0.f;
												if (Check_F14[i].StartTimer > 0.f) {
													Check_F14[i].StartTimer -= 1.f / FPS * GameSpeed;
												}
												else {
													Check_F14[i].StartTimer = 0.f;
												}
											}
											else {
												easing_set(&DiffPer, 0.f, 0.95f);
											}
										}
										A->obj.get_anime(0).per += DiffPer;
									}
								}

								Check_F14[i].Yrad += Check_F14[i].YradAdd / FPS * GameSpeed;

								Check_F14[i].move.mat = MATRIX_ref::RotX(deg2rad(Check_F14[i].Xrad)) * MATRIX_ref::RotY(deg2rad(Check_F14[i].Yrad));
								Check_F14[i].move.vec = MATRIX_ref::Vtrans(VECTOR_ref::vget(0, Check_F14[i].Yadd, Check_F14[i].Zadd) * (1.f / FPS * GameSpeed), MATRIX_ref::RotY(deg2rad(Check_F14[i].Yrad)));
								Check_F14[i].move.pos += Check_F14[i].move.vec;

								M->obj.get_anime(0).per = 1.f;
								M->obj.get_anime(1).per = Check_F14[i].pWingOpen;
								easing_set(&M->obj.get_anime(2).per, sin(deg2rad(std::min(Check_F14[i].pCheck * 2.0f + 180.f, 360.f))), 0.9f);
								easing_set(&M->obj.get_anime(3).per, sin(deg2rad(std::max(Check_F14[i].pCheck * 2.0f - 180.f, 0.f))), 0.9f);
								easing_set(&M->obj.get_anime(4).per, sin(deg2rad(Check_F14[i].pCheck * 1.5f)), 0.9f);
								//
								if (Check_F14[i].AimFlag) {
									if (Check_F14[i].pWingOpen > 0.1f) {
										Check_F14[i].pWingOpen -= 0.5f / FPS * GameSpeed;
									}
									else {
										Check_F14[i].pWingOpen -= 0.15f / FPS * GameSpeed;
										if (Check_F14[i].pWingOpen <= 0.f) {
											Check_F14[i].pWingOpen = 0.f;
											if (Check_F14[i].pWopentime > 0.f) {
												Check_F14[i].pWopentime -= 1.f / FPS * GameSpeed;
											}
											else {
												Check_F14[i].pWopentime = 0.f;
												if (Check_F14[i].pCheck < 10.f) {
													Check_F14[i].pCheck += 60.f / FPS * GameSpeed;
												}
												else {
													if (Check_F14[i].pCheck < 350.f) {
														Check_F14[i].pCheck += 120.f / FPS * GameSpeed;
													}
													else {
														if (Check_F14[i].pCheck < 360.f) {
															Check_F14[i].pCheck += 60.f / FPS * GameSpeed;
														}
														else {
															Check_F14[i].pCheck = 360.f;
														}
													}
												}
											}
										}
									}
								}
								//
							}
						}
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
							VECTOR_ref vec;
							bool isforcus = false;
#if 1
							for (auto&f : m_CutInfo[m_Counter].Forcus) {
								if (f.GetIsUse()) {
									vec += f.GetForce(models);
									isforcus = true;
								}
							}
							if (isforcus) {
								m_CutInfo[m_Counter].Aim_camera.camvec = vec / (float)(m_CutInfo[m_Counter].Forcus.size());
							}
#else
							for (auto&f : m_CutInfo[m_Counter].Forcus) {
								if (f.GetIsUse()) {
									vec += f.GetForce(models) * f.Per;
									isforcus = true;
								}
							}
							if (isforcus) {
								m_CutInfo[m_Counter].Aim_camera.camvec = vec;
							}
#endif
							//
							if (attached.GetSwitch() && attached_override) {
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
							if (attached.GetSwitch() && attached_override) {
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
					//*
					for (int i = 0; i < 1; i++) {
						auto* M = models.Get(TomCat, i);
						//全体移動
						M->obj.SetMatrix(Check_F14[i].move.MatIn());
						//フレーム移動
						if (!Check_F14[i].AimFlag2) {
							if (Check_F14[i].FrontWheel >= 0) {
								auto sel = Check_F14[i].FrontWheel;
								M->obj.frame_Reset(sel);
								auto Ysus = M->obj.frame(sel).y();
								if (isFirstLoop) {
									Check_F14[i].FrontYBase = Ysus;
									Frontmat = M->obj.GetFrameLocalMatrix(sel);
								}
								M->obj.SetFrameLocalMatrix(sel,
									Frontmat
									*
									MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -(Check_F14[i].FrontYBase - Ysus), 0))
								);
							}
						}
						for (auto& Sel : WheelFrames) {
							M->obj.frame_Reset(Sel.sel);
							auto frame = M->obj.frame(Sel.sel);
							auto& W = Check_F14[i].Wheels[&Sel - &WheelFrames.front()];
							if (isFirstLoop) {
								Sel.mat = M->obj.GetFrameLocalMatrix(Sel.sel);
								W.Prev = frame;
							}
							//
							auto vec1 = frame - W.Prev;

							W.Rad += vec1.size() * M->obj.GetMatrix().zvec().dot(vec1.Norm()) * 180.f / 15.f;
							W.Prev = frame;
							//
							M->obj.SetFrameLocalMatrix(Sel.sel,
								MATRIX_ref::RotAxis(MATRIX_ref::Vtrans(Check_F14[i].move.mat.xvec(), M->obj.GetFrameLocalWorldMatrix(Sel.sel).GetRot().Inverse()),
									deg2rad(W.Rad)) * Sel.mat);
						}
					}
					//*/
					//
				}
				{
					//models.SetPhysics(ResetPhysics || ModelEdit_PhysicsReset);
					//ModelEdit_PhysicsReset = false;
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
				BGM.Dispose();
				//grassmodel.Dispose();
#ifdef EditMode
				Editer_Dispose();
#endif
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
				models.Draw(false, false, TRUE);
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

				if(m_Counter==6){
					DrawBillboard3D(VECTOR_ref::vget(898.f, 107.f, 287.f).get(),
						0.5f, 0.5f,
						22.f, 0.f,
						boards[0].get(),
						TRUE
					);

					DrawBillboard3D(VECTOR_ref::vget(820.f, 106.f, 275.f).get(),
						0.5f, 0.5f,
						21.f, 0.f,
						boards[1].get(),
						TRUE
					);

					DrawBillboard3D(VECTOR_ref::vget(826.f, 106.f, 280.f).get(),
						0.5f, 0.5f,
						23.f, 0.f,
						boards[2].get(),
						TRUE
					);
				}

				if (m_Counter == 7) {
					DrawBillboard3D(VECTOR_ref::vget(86.4f, 18.5f, 105.2f).get(),
						0.5f, 0.5f,
						8.5f, 0.f,
						boards[3].get(),
						TRUE
					);
				}

				if (m_Counter == 8) {
					DrawBillboard3D(VECTOR_ref::vget(-220.0f, 26.3f, 351.9f).get(),
						0.5f, 0.5f,
						68.5f, 0.f,
						boards[4].get(),
						TRUE
					);
				}

				models.CheckInCamera(camera_main.far_);
				//+201 = 67x3
				auto camfar = GetCameraFar();
				//カメラのfarが適正範囲の設定の場合
				//near
				if (camera_main.near_ - 1.f < camfar&& camfar < camera_main.near_ + 1.f) {
				}
				else if (camera_main.far_ - 1.f < camfar&& camfar < camera_main.far_ + 1.f) {
					models.Draw(false, false, FALSE);
				}
				//far
				else {
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
				//grassmodel.Draw(camera_buf);
			}
			//
			void LAST_Draw(void) noexcept override {
#ifdef EditMode
				Editer_Calc();

				Editer_Draw();
#endif
			}
		};
	};
};