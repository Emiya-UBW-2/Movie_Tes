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
			switchs ChangeCamSel;									//
			switchs ChangeStart;									//
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
			int BGM_Frequency;										//
			MV1 Guide;												//
			switchs SpeedUp;										//
			switchs SpeedDown;										//
			switchs Start;											//
			switchs MouseClick;										//
			int changeSeekID{ -1 };									//
			int x_now{ -1 };										//
			int BaseWidth{ 1920 / 64 };								//
			int WidthPer{ 2000000 };								//
			int X_now{ 0 };											//
			int CutNow{ 0 };										//
			Change_Model m_ChangeModel;								//
			Edit_Model m_EditModel;									//
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
				for (const auto& m : models.GetModel()) {
					const auto* sel = LSClass.GetArgFromPath(m.Path);
					if (sel != nullptr) {
						printfDx((" " + sel->Base + "(" + std::to_string(m.BaseID) + ") = %d\n").c_str(), m.DrawCount);
					}
				}

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
				BGM.vol(0);
				///*
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
				Start.GetInput((!m_EditModel.m_IsActive && CheckHitKey(KEY_INPUT_SPACE) != 0) || ModelEditIn);
				ModelEditIn = false;
				if ((!m_EditModel.m_IsActive && (SpeedUp.trigger() || SpeedDown.trigger())) || Start.trigger()) {
					if (SpeedUp.trigger()) { spd_x++; }
					if (SpeedDown.trigger()) { spd_x--; }
					if (Start.trigger()) { spd_x = Start.on() ? 10 : 0; }
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
								RankID.resize(NAMES.size());
								Guide.SetMatrix(models.Get(MAP, 0)->obj.GetMatrix());
								for (int i = 0; i < Guide.frame_num(); i++) {
									if (Guide.frame_child_num(i) > 0) {
										GudeStart = i;
										break;
									}
								}
								{
									auto* m = models.Get(GATE, 0);

									m->pos_p.Set(0, 0, 160.f);
									m->Yrad1_p = -90.f;
									m->mat_p =
										MATRIX_ref::RotY(deg2rad(m->Yrad1_p))
										* MATRIX_ref::RotAxis(MATRIX_ref::RotY(deg2rad(m->Yrad1_p)).zvec(), deg2rad(m->Zrad1_p))
										* MATRIX_ref::Mtrans(m->pos_p) * MATRIX_ref::RotY(deg2rad(m->Yrad2_p));
									if (m->canUpdate) {
										m->obj.SetMatrix(m->mat_p);
									}
								}
								for (auto& n : NAMES) {
									auto* m = models.Get(n, 0);
									m->firststart = false;
									m->pos_z_p = 0.f;
								}
							}

							ChangeStart.GetInput(!ChangeStart.on() && (CheckHitKey(KEY_INPUT_V) != 0));
							if (ChangeStart.trigger()) {
								for (auto& n : NAMES) {
									auto* m = models.Get(n, 0);
									m->firststart = true;
								}
							}

							{
								float xof = 0.f;
								float xof2 = 0.f;
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
											xof += 13.8f;
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
													if (ChangeStart.on()) {
														m->Aim_p = Aim * (1.f / (float)aimcnt);
													}
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
										if (ChangeStart.on()) {
											m->pos_p += MATRIX_ref::Vtrans(VECTOR_ref::vget(0, 0, m->pos_z_p), MATRIX_ref::RotY(deg2rad(m->Yrad1_p)))*(1.f / FPS * GameSpeed);
										}
										auto total = -19.4f * 12.5f;
										if (m->GuideNum[0] >= 0) {
											if (m->pos_z_p != 0.f) {
												if (m->pos_z_p >= total + GetRandf(1.0f*12.5f) - m->TurboSpeed) { m->pos_z_p += -0.6f; }
												else { m->pos_z_p -= -0.6f; }
											}
											//
											if (m->firststart) {
												if (m->pos_z_p >= total / 2.f) {
													m->pos_z_p += -12.f;
												}
												else {
													m->firststart = false;
												}
											}
											//
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
										auto xpos_buf = m->xposition;
										switch (m->Rank) {
										case 1:
											xpos_buf = 30.f + GetRandf(20.f);
											break;
										default:
										{
											auto xvb = MATRIX_ref::RotY(deg2rad(m->Yrad1_p)).xvec();
											auto Lcos = xvb.dot(m->Nearest);
											if (Lcos > 0 && m->Nearest.size() > 80.f) {
												xpos_buf = std::max(xpos_buf - 10.f / FPS * GameSpeed, 20.f + GetRandf(20.f));
												xof2 = std::max(0.f, xof2 - 72.f);
											}
											else if (m->Nearest.size() > 240.f) {
												xpos_buf = std::max(xpos_buf - 5.f / FPS * GameSpeed, 20.f + GetRandf(20.f));
												xof2 = std::max(0.f, xof2 - 72.f);
											}
											else {
												if (m->Rank > NAMES.size() / 2) {
													xof2 -= 16.f;
												}
												else {
													xof2 += 24.f;
												}
												xof2 = std::max(0.f, xof2);
												xpos_buf = xof2 + GetRandf(50.f);
											}
											break;
										}
										}
										if (m->xposition > xpos_buf) {
											m->xposition -= 5.f / FPS * GameSpeed;
										}
										else {
											m->xposition += 5.f / FPS * GameSpeed;
										}
										m->xposition = std::max(0.f, m->xposition);
									}
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
								for (auto& n : NAMES) {
									auto* m = models.Get(n, 0);
									m->Nearest = VECTOR_ref::up()*10000.f;
									m->FrontLength = 10000.f;
									m->FrontID = "";
									auto zvb = MATRIX_ref::RotY(deg2rad(m->Yrad1_p)).zvec();
									for (auto& n2 : NAMES) {
										auto* m2 = models.Get(n2, 0);
										if (n != n2) {
											auto vec = m2->pos_p - m->pos_p;
											//一番近い相手を更新
											if (vec.size() < m->Nearest.size()) {
												m->NearID = n2;
												m->Nearest = vec;
											}
											//前後にいるIDを更新
											{
												auto FR = zvb.dot(vec);
												if (FR <= 0) {
													if (-FR < m->FrontLength) {
														m->FrontID = n2;
														m->FrontLength = -FR;
													}
												}
											}
											//
										}
									}
								}
								//順位判定
								size_t RankBuf = NAMES.size();
								for (int Rank = 0; Rank < NAMES.size(); Rank++) {
									for (auto& n : NAMES) {
										auto* m = models.Get(n, 0);
										if (Rank == 0) {
											if (m->FrontID == "") {
												m->Rank = Rank + 1;
												RankBuf = &n - &NAMES.front();
												break;
											}
										}
										else {
											if (m->FrontID == NAMES[RankBuf]) {
												m->Rank = Rank + 1;
												RankBuf = &n - &NAMES.front();
												break;
											}
										}
									}
									if (Rank == 0 && RankBuf == NAMES.size()) { break; }
								}
								for (auto& n : NAMES) {
									auto* m = models.Get(n, 0);
									RankID[m->Rank - 1] = &n - &NAMES.front();

									printfDx("Rank%d = %s,(%s)\n", m->Rank, m->FrontID.c_str(), n.c_str());
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
									//auto dist = (models.Get(NAMES[camsel], 0)->pos_p - m_CutInfo[m_Counter].Aim_camera.campos).size() + 50.f;
									//printfDx("FOV = %5.2f", 100.f / dist * 45.f);
									//easing_set(&m_CutInfo[m_Counter].Aim_camera.fov, deg2rad(std::clamp(100.f / dist * 45.f, 1.f, 50.f)), 0.9f);

									if (cam_far > 6000.f) {
										cam_near = std::clamp(cam_far / 1.8f, 2.f, 3000.f);
									}
									else if (cam_far > 4000.f) {
										cam_near = std::clamp(cam_far / 3.f, 2.f, 2000.f);
									}
									else if (cam_far > 2000.f) {
										cam_near = std::clamp(cam_far / 6.f, 2.f, 2000.f);
									}
									else if (cam_far > 1000.f) {
										cam_near = std::clamp(cam_far / 8.f, 2.f, 2000.f);
									}
									else {
										cam_near = std::min(cam_near, std::clamp(cam_far / 10.f, 2.f, 1000.f));
									}
									printfDx("FAR  = %.2f\n", cam_far);
									printfDx("NEAR = %.2f\n", cam_near);

									if (std::abs(m_CutInfo[m_Counter].Aim_camera.near_ - cam_near) > 1000.f) { m_CutInfo[m_Counter].Aim_camera.near_ = cam_near; }
									easing_set(&m_CutInfo[m_Counter].Aim_camera.near_, cam_near, 0.5f);
									easing_set(&m_CutInfo[m_Counter].Aim_camera.far_, cam_far, 0.5f);
								}
								//
							}
							//カット設定
							{
								m_CutInfoUpdate[m_Counter].CutSel = CutSel_Buf;
							}
							//init
							enum {
								CAMERA_1,

								CAMERA_A,

								CAMERA_2,
								CAMERA_2_2,

								CAMERA_B,

								CAMERA_3,

								CAMERA_4,

								CAMERA_5,

								CAMERA_6,

								CAMERA_7,
							};
							{

								if (m_CutInfoUpdate[m_Counter].OLDCutSel != m_CutInfoUpdate[m_Counter].CutSel) {
									switch (m_CutInfoUpdate[m_Counter].CutSel) {
									case CAMERA_1:
									{
										camsel_buf = NAMES.size() / 2;
										camsel = RankID[camsel_buf];

										m_CutInfoUpdate[m_Counter].m_RandcamvecSet.Set(0.f, 0.f, 0.f);
										m_CutInfoUpdate[m_Counter].m_RandcamvecPer = 0.f;
										m_CutInfoUpdate[m_Counter].camvec_per = 0.f;

										m_CutInfoUpdate[m_Counter].Forcus.resize(1);
										m_CutInfoUpdate[m_Counter].Forcus[0].Set(NAMES[camsel], 0, "HEAD", VECTOR_ref::zero());

										attachedDetail.back().poscam.Set(3000.f, 660.f, -5000.f);
										attached_override = true;

										m_CutInfo[m_Counter].Aim_camera.fov = deg2rad(1.5f);

										Change1Time = 3.f;
									}
									break;

									case CAMERA_A:
									{
										camsel_buf = 0;
										camsel = RankID[camsel_buf];

										m_CutInfoUpdate[m_Counter].m_RandcamvecSet.Set(0.f, 0.f, 0.f);
										m_CutInfoUpdate[m_Counter].m_RandcamvecPer = 0.f;
										m_CutInfoUpdate[m_Counter].camvec_per = 0.f;

										m_CutInfoUpdate[m_Counter].Forcus.resize(1);
										m_CutInfoUpdate[m_Counter].Forcus[0].Set(NAMES[camsel], 0, "HEAD", VECTOR_ref::zero());

										attachedDetail.back().poscam =
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).xvec() * -90.f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).yvec() * 5.f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).zvec() * -30.f;
										attached_override = true;

										m_CutInfo[m_Counter].Aim_camera.fov = deg2rad(14.f);

										Change1Time = 3.f;
									}
									break;

									case CAMERA_2:
									{
										camsel_buf = 0;
										camsel = RankID[camsel_buf];

										m_CutInfoUpdate[m_Counter].m_RandcamvecSet.Set(0.f, 0.f, 0.f);
										m_CutInfoUpdate[m_Counter].m_RandcamvecPer = 0.f;
										m_CutInfoUpdate[m_Counter].camvec_per = 0.f;

										m_CutInfoUpdate[m_Counter].Forcus.resize(1);
										m_CutInfoUpdate[m_Counter].Forcus[0].Set(NAMES[camsel], 0, "HEAD", VECTOR_ref::zero());

										attachedDetail.back().poscam =
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).xvec() * -10.f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).yvec() * 25.f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).zvec() * -90.f;
										attached_override = true;

										m_CutInfo[m_Counter].Aim_camera.fov = deg2rad(14.f);

										Change1Time = 1.5f;
									}
									break;

									case CAMERA_2_2:
									{
										m_CutInfo[m_Counter].Aim_camera.fov = deg2rad(6.5f);
										Change1Time = 3.f;
									}
									break;

									case CAMERA_B:
									{
										camsel_buf = 0;
										camsel = RankID[camsel_buf];

										m_CutInfoUpdate[m_Counter].Forcus.resize(1);
										m_CutInfoUpdate[m_Counter].Forcus[0].Set(NAMES[camsel], 0, "NECK", VECTOR_ref::zero());


										m_CutInfo[m_Counter].Aim_camera.campos = m_CutInfo[m_Counter].Aim_camera.camvec +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).xvec() * -150.f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).yvec() * 3.f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).zvec() * -400.f;
										attached_override = false;


										m_CutInfoUpdate[m_Counter].m_RandcamvecSet.Set(0.f, 0.f, 0.f);
										m_CutInfoUpdate[m_Counter].m_RandcamvecPer = 0.f;
										m_CutInfoUpdate[m_Counter].camvec_per = 0.f;

										Change1Time = 2.25f;

										m_CutInfo[m_Counter].Aim_camera.fov = deg2rad(25.f);
									}
									break;

									case CAMERA_3:
									{
										m_CutInfoUpdate[m_Counter].Forcus.resize(2);

										camsel_buf = 0;
										camsel = RankID[camsel_buf];
										m_CutInfoUpdate[m_Counter].Forcus[0].Set(NAMES[camsel], 0, "NECK", VECTOR_ref::zero());
										m_CutInfoUpdate[m_Counter].Forcus[1].Set(NAMES[camsel], 0, "NECK", VECTOR_ref::zero());

										Per_Change = 1.f;
										m_CutInfo[m_Counter].Aim_camera.fov = deg2rad(30.f);

										attachedDetail.back().poscam =
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).yvec() * 25.f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).zvec() * -90.f;
										attached_override = true;
									}
									break;

									case CAMERA_4:
									{
										camsel_buf = 0;
										camsel = RankID[camsel_buf];

										m_CutInfoUpdate[m_Counter].Forcus.resize(1);
										m_CutInfoUpdate[m_Counter].Forcus[0].Set(NAMES[camsel], 0, "NECK", VECTOR_ref::zero());


										m_CutInfo[m_Counter].Aim_camera.campos = m_CutInfo[m_Counter].Aim_camera.camvec +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).xvec() * 50.f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).yvec() * -12.f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).zvec() * -400.f;
										attached_override = false;


										m_CutInfoUpdate[m_Counter].m_RandcamvecSet.Set(30.f, 30.f, 30.f);
										m_CutInfoUpdate[m_Counter].m_RandcamvecPer = 0.9f;
										m_CutInfoUpdate[m_Counter].camvec_per = 0.9f;

										Change1Time = 2.25f;

										m_CutInfo[m_Counter].Aim_camera.fov = deg2rad(25.f);
									}
									break;

									case CAMERA_5:
									{
										camsel_buf = 0;
										camsel = RankID[camsel_buf];

										m_CutInfoUpdate[m_Counter].m_RandcamvecSet.Set(0.f, 0.f, 0.f);
										m_CutInfoUpdate[m_Counter].m_RandcamvecPer = 0.f;
										m_CutInfoUpdate[m_Counter].camvec_per = 0.f;

										m_CutInfoUpdate[m_Counter].Forcus.resize(1);
										m_CutInfoUpdate[m_Counter].Forcus[0].Set(NAMES[camsel], 0, "HEAD", VECTOR_ref::zero());

										attachedDetail.back().poscam =
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).xvec() * 60.f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).yvec() * 0.2f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).zvec() * -90.f;
										attached_override = true;

										m_CutInfo[m_Counter].Aim_camera.fov = deg2rad(10.f);

										Change1Time = 3.f;
									}
									break;

									case CAMERA_6:
									{
										camsel_buf = 1;
										camsel = RankID[camsel_buf];

										m_CutInfoUpdate[m_Counter].m_RandcamvecSet.Set(0.f, 0.f, 0.f);
										m_CutInfoUpdate[m_Counter].m_RandcamvecPer = 0.f;
										m_CutInfoUpdate[m_Counter].camvec_per = 0.f;

										m_CutInfoUpdate[m_Counter].Forcus.resize(1);
										m_CutInfoUpdate[m_Counter].Forcus[0].Set(NAMES[camsel], 0, "HEAD", VECTOR_ref::zero());

										attachedDetail.back().poscam.Set(20.f, 2.5f, 25.f);
										attached_override = true;

										m_CutInfo[m_Counter].Aim_camera.fov = deg2rad(25.f);

										Change1Time = 3.f;
									}
									break;

									case CAMERA_7:
									{
										camsel_buf = 0;
										camsel = RankID[camsel_buf];

										m_CutInfoUpdate[m_Counter].Forcus.resize(1);
										m_CutInfoUpdate[m_Counter].Forcus[0].Set(NAMES[camsel], 0, "NECK", VECTOR_ref::zero());


										m_CutInfo[m_Counter].Aim_camera.campos = m_CutInfo[m_Counter].Aim_camera.camvec +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).xvec() * 70.f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).yvec() * 12.f +
											MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).zvec() * -700.f;
										attached_override = false;

										Change1Time = 5.f;

										m_CutInfoUpdate[m_Counter].m_RandcamvecSet.Set(5.f, 5.f, 5.f);
										m_CutInfoUpdate[m_Counter].m_RandcamvecPer = 0.95f;
										m_CutInfoUpdate[m_Counter].camvec_per = 0.95f;

										m_CutInfo[m_Counter].Aim_camera.fov = deg2rad(45.f);
									}
									break;

									default:
										break;
									}
								}
								m_CutInfoUpdate[m_Counter].OLDCutSel = m_CutInfoUpdate[m_Counter].CutSel;
							}
							//update
							{
								switch (m_CutInfoUpdate[m_Counter].CutSel) {
								case CAMERA_1:
								{
									if (ChangeStart.on()) {
										Change1Time -= 1.f / FPS * GameSpeed;
										if (Change1Time <= 0.f) {
											CutSel_Buf++;
										}
									}
								}
								break;

								case CAMERA_A:
								{
									Change1Time -= 1.f / FPS * GameSpeed;
									if (Change1Time <= 0.f) {
										CutSel_Buf++;
									}
								}
								break;

								case CAMERA_2:
								{
									m_CutInfo[m_Counter].Aim_camera.fov -= deg2rad(2.f * 1.f / FPS * GameSpeed);

									Change1Time -= 1.f / FPS * GameSpeed;
									if (Change1Time <= 0.f) {
										CutSel_Buf++;
									}
								}
								break;

								case CAMERA_2_2:
								{
									Change1Time -= 1.f / FPS * GameSpeed;
									if (Change1Time <= 0.f) {
										CutSel_Buf++;
									}
								}
								break;

								case CAMERA_B:
								{
									m_CutInfo[m_Counter].Aim_camera.near_ = 3.f;
									Change1Time -= 1.f / FPS * GameSpeed;
									if (Change1Time <= 0.f) {
										CutSel_Buf++;
									}
								}
								break;

								case CAMERA_3:
								{
									if (m_CutInfoUpdate[m_Counter].Forcus.size() == 2) {
										ChangeCamSel.GetInput((CheckHitKey(KEY_INPUT_DOWN) != 0) && (Per_Change <= 0.001f));
										if (ChangeCamSel.trigger() || (Per_Change <= 0.01f)) {
											m_CutInfoUpdate[m_Counter].Forcus[0].Set(NAMES[camsel], 0, "NECK", VECTOR_ref::zero());
											++camsel_buf %= NAMES.size();
											camsel = RankID[camsel_buf];
											m_CutInfoUpdate[m_Counter].Forcus[1].Set(NAMES[camsel], 0, "NECK", VECTOR_ref::zero());
											Per_Change = 1.f;

											if (camsel_buf == 0) {
												CutSel_Buf++;
											}
										}
										else {
											easing_set(&Per_Change, 0.f, 0.95f);
										}
										m_CutInfoUpdate[m_Counter].Forcus[0].Per = Per_Change;
										m_CutInfoUpdate[m_Counter].Forcus[1].Per = 1.f - Per_Change;
									}
									easing_set(&attachedDetail.back().poscam,
										MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).yvec() * 25.f +
										MATRIX_ref::RotY(deg2rad(models.Get(NAMES[camsel], 0)->Yrad1_p)).zvec() * -90.f
										, 0.95f);
									easing_set(&m_CutInfo[m_Counter].Aim_camera.fov, deg2rad(25.f), 0.9f);
								}
								break;

								case CAMERA_4:
								{
									m_CutInfo[m_Counter].Aim_camera.near_ = 3.f;
									Change1Time -= 1.f / FPS * GameSpeed;
									if (Change1Time <= 0.f) {
										CutSel_Buf++;
									}
								}
								break;

								case CAMERA_5:
								{
									Change1Time -= 1.f / FPS * GameSpeed;
									if (Change1Time <= 0.f) {
										CutSel_Buf++;
									}
								}
								break;

								case CAMERA_6:
								{
									m_CutInfo[m_Counter].Aim_camera.near_ = 3.f;

									Change1Time -= 1.f / FPS * GameSpeed;
									if (Change1Time <= 0.f) {
										CutSel_Buf++;
									}
								}
								break;

								case CAMERA_7:
								{
									m_CutInfo[m_Counter].Aim_camera.near_ = 3.f;

									Change1Time -= 1.f / FPS * GameSpeed;
									if (Change1Time <= 0.f) {
										CutSel_Buf = 1;
									}
								}
								break;

								default:
									break;
								}
							}
							//
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
				grassmodel.Dispose();

				Editer_Dispose();
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
#ifdef EditMode
				Editer_Calc();

				Editer_Draw();
#endif
			}
		};
	};
};