#pragma once
#include	"../Header.hpp"
#include	"../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
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
					mode = Model_Type[0];
				}
				else if (!this->m_Ptr->IsNearShadow) {
					mode = Model_Type[1];
				}
				else if (this->m_Ptr->IsFarShadow) {
					mode = Model_Type[2];
				}
				else if (!this->m_Ptr->ShadowDrawActive) {
					mode = Model_Type[3];
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
				auto* Fonts = FontPool::Instance();
				int mouse_x, mouse_y;
				GetMousePoint(&mouse_x, &mouse_y);
				//AnimeSel
				{
					if (in2_(mouse_x, mouse_y, x1, y1, x1 + xs, y1 + ys)) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
						DrawBox(x1 + thick, y1 + thick, x1 + xs - thick, y1 + ys - thick, GetColor(255, 255, 255), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					auto length = Fonts->Get(FontPool::FontType::Nomal_Edge).GetStringWidth(ys, Info);
					if (length > xs / 2) {
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(ys, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, x1 + xs, y1, GetColor(255, 255, 255), GetColor(0, 0, 0), Info);
					}
					else {
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(ys, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x1 + xs / 2, y1, GetColor(255, 255, 255), GetColor(0, 0, 0), Info);
					}

					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(ys, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x1, y1, GetColor(255, 255, 255), GetColor(0, 0, 0), Name);
				}
			}
		};



		class CustomScene : public TEMPSCENE, public EffectControl {
		public:
			CustomScene(void) noexcept { }
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			Depth_Draw_Sub(void) noexcept override {}
			void			BG_Draw_Sub(void) noexcept override;
			void			ShadowDraw_Far_Sub(void) noexcept override;
			void			ShadowDraw_NearFar_Sub(void) noexcept override;
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			void			MainDrawbyDepth_Sub(void) noexcept override {}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept  override;
			void			DrawUI_In_Sub(void) noexcept override {}
		private:
			std::vector<std::string> NAMES;
			ModelControl models;
			GraphControl graphs;
			LoadScriptClass LSClass;		//スクリプト読み込み
			TelopClass TLClass;				//テロップ
			std::vector<Cut_Info_First> m_CutInfo;
			std::vector<Cut_Info_Update> m_CutInfoUpdate;
			CutInfoClass attached;
			std::vector<CutAttachDetail> attachedDetail;
			//Grass grassmodel;
			std::string LOGO1 = "data/picture/logo.png";
			std::string Haron = "data/picture/red.png";
			std::string Suit = "data/model/Suit/model.mv1";
			std::string Soldier = "data/model/Soldier/model.mv1";
			int mdata;
			int cnt;
			LONGLONG TotalTime, NowTime;
		private:
			std::string Pic3BackName = "data/picture/Logo.png";
			GraphHandle Pic3Back;									//
			std::string PicBack1Name = "data/picture/Back1.png";
			GraphHandle PicBack1;									//
			VECTOR_ref	PicBack1Per;
			VECTOR_ref	PicBack1Per2;

			struct Anims {
				std::vector<GraphHandle> Pic;
				int m_FrameID{ 0 };
				int anim{ 0 };
				int animcountper{ 0 };
				int animcountSpeed{ 4 };

				void Set(int FrameID, int LayerID, int totalNum, int FramePer) {
					m_FrameID = FrameID;
					this->animcountSpeed = FramePer;
					for (int i = 0; i < totalNum; i++) {
						std::string FileName = "data/picture/Scene" + std::to_string(FrameID) + "_" + std::to_string(LayerID);
						if (i < 10) {
							FileName += "0" + std::to_string(i);
						}
						else {
							FileName += std::to_string(i);
						}
						this->Pic.emplace_back(GraphHandle::Load(FileName + ".bmp", true));
					}
				}

				void Execute() {
					if (Pic.size() > 0) {
						this->animcountper++;
						if (this->animcountper > this->animcountSpeed) {
							this->animcountper = 0;
							++this->anim %= this->Pic.size();
						}
					}
				}
				void Draw(int x, int y, float scale) {
					this->Pic[this->anim].DrawRotaGraph(x, y, scale, 0.f, true);
				}
			};

			std::vector<Anims> Pic_Scene;

			float xpos_1 = 0.f;
			//
			Camera3DInfo camera_buf;								//
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
			float Black_Buf{ 1.f };									//
			float White_Buf{ 0.f };									//

			float bs_posY{ 0.f };

			float hg_scaleY{ 0.f };
			float sw_speedz{ 0.f };

			float pic_timer{ 0.f };
			float pic_Per{ 0.f };

			GraphHandle movie1;										//
			GraphHandle movie2;										//
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

			int m_LightHandle = -1;
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

				int x_p = DrawParts->m_DispXSize * 5 / 10;
				int x_s = DrawParts->m_DispXSize * 4 / 10;
				int y_p = DrawParts->m_DispYSize * 2 / 10;

				int hight = y_r(20);

				int p2 = 2;

				CharaEdit.resize(5);

				int y1 = y_p + hight + p2;
				CharaEdit[0].Set("AnimeSel", x_p, y1, x_s, hight, p2); y1 += hight + p2;
				CharaEdit[1].Set("DrawMode", x_p, y1, x_s, hight, p2); y1 += hight + p2;
				CharaEdit[2].Set("ModelPhysicsSpeed", x_p, y1, x_s, hight, p2); y1 += hight + p2;
				CharaEdit[3].Set("OpacityRate", x_p, y1, x_s, hight, p2); y1 += hight + p2;
				CharaEdit[4].Set("Matrix", x_p, y1, x_s, hight, p2); y1 += hight + p2;

				LookEditer.Set(false);
				LookMovie.Set(true);

				movie = GraphHandle::Load("data/base_movie2.mp4");
				PauseMovieToGraph(movie.get());


				movie1 = GraphHandle::Load("data/movie1.mp4");
				PauseMovieToGraph(movie1.get());
				movie2 = GraphHandle::Load("data/base_movie2.mp4");
				PauseMovieToGraph(movie2.get());
			}
			void Editer_Calc(void)noexcept {
				auto* DrawParts = DXDraw::Instance();

				int mouse_x, mouse_y;
				GetMousePoint(&mouse_x, &mouse_y);

				BaseWidth = DrawParts->m_DispXSize / 64;

				LookMovie.Execute(CheckHitKeyWithCheck(KEY_INPUT_M) != 0);
				LookEditer.Execute(CheckHitKeyWithCheck(KEY_INPUT_N) != 0);


				if ((LookMovie.trigger() && LookMovie.on()) || (0 <= NowTimeWait && NowTimeWait <= ((1000 * 1000) / 60))) {
					//PlayMovieToGraph(movie.get(), 2, DX_MOVIEPLAYTYPE_BCANCEL);
					//ChangeMovieVolumeToGraph(0, movie.get());
					//SeekMovieToGraph(movie.get(), (int)(NowTimeWait / 1000));
					PlayMovieToGraph(movie1.get(), 2, DX_MOVIEPLAYTYPE_BCANCEL);
					ChangeMovieVolumeToGraph(0, movie1.get());
					SeekMovieToGraph(movie1.get(), (int)(900 * 1000 / 1000));

					PlayMovieToGraph(movie2.get(), 2, DX_MOVIEPLAYTYPE_BCANCEL);
					ChangeMovieVolumeToGraph(0, movie2.get());
					SeekMovieToGraph(movie2.get(), (int)(0 / 1000));
				}
				if (LookEditer.on()) {
					//編集画面
					{
						int x_p = DrawParts->m_DispXSize * 1 / 10;
						int x_s = DrawParts->m_DispXSize * 8 / 10;
						int y_p = DrawParts->m_DispYSize * 5 / 10;
						int y_s = DrawParts->m_DispYSize * 4 / 10;

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
						int x_p = DrawParts->m_DispXSize * 5 / 10;
						int x_s = DrawParts->m_DispXSize * 4 / 10;
						int y_p = DrawParts->m_DispYSize * 2 / 10;
						int y_s = DrawParts->m_DispYSize * 5 / 20;
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
						int x_p = DrawParts->m_DispXSize * 1 / 10;
						int x_s = DrawParts->m_DispXSize * 8 / 10;
						int y_p = DrawParts->m_DispYSize * 5 / 10;
						int y_s = DrawParts->m_DispYSize * 4 / 10;

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
							auto* Fonts = FontPool::Instance();
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
									Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(hight, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x_p, y1, GetColor(255, 255, 255), GetColor(0, 0, 0), " " + sel->Base + "(" + std::to_string(m.BaseID) + ")");
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
						int x_p = DrawParts->m_DispXSize * 5 / 10;
						int x_s = DrawParts->m_DispXSize * 4 / 10;
						int y_p = DrawParts->m_DispYSize * 2 / 10;
						int y_s = DrawParts->m_DispYSize * 5 / 20;

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
									auto* Fonts = FontPool::Instance();
									const auto* sel = LSClass.GetArgFromPath(m_EditModel.m_Ptr->Path);
									if (sel != nullptr) {
										Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(hight, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x_p + x_s / 2, y1, GetColor(255, 255, 255), GetColor(0, 0, 0), sel->Base + "(" + std::to_string(m_EditModel.m_Ptr->BaseID) + ")");
									}
									Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(hight, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x_p, y1, GetColor(255, 255, 255), GetColor(0, 0, 0), "Name");
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
					//movie.DrawExtendGraph(DrawParts->m_DispXSize * 3 / 4, DrawParts->m_DispYSize * 3 / 4, DrawParts->m_DispXSize, DrawParts->m_DispYSize, FALSE);
				}
			}

			void Editer_Dispose() {
				movie1.Dispose();
				movie2.Dispose();

				movie.Dispose();
			}
#endif
		public:
			//Getter
			bool Time_Over(void) const noexcept { return m_Counter >= m_CutInfo.size(); }
		public:
			const float GetTotalTime(void) const noexcept { return float((GetNowHiPerformanceCount() - TotalTime) / 1000) / 1000.f; }
			const auto GetNowTime(void) noexcept {
				float tim = float((GetNowHiPerformanceCount() - NowTime) / 1000) / 1000.f;
				NowTime = GetNowHiPerformanceCount();
				return tim;
			}
			//
			template <typename... Args>
			void PutMsg(std::string FormatString, Args&&... args) {
				auto* DrawParts = DXDraw::Instance();
				float tim = GetNowTime();
				if (tim >= 0.001f) {
					GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
				}
				printfDx(("[TOTAL %7.3f s][NOW %7.3f s] : " + FormatString).c_str(), GetTotalTime(), tim, args...);
				if (tim >= 0.001f) {
					DrawParts->Screen_Flip();
				}
			}
		};
	};
};
