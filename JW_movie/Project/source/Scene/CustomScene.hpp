#pragma once
#include	"../Header.hpp"
#include	"../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

#define EditMode

		class LoadUtil {
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
				const bool IsEditMode() const noexcept { return this->m_Ptr != nullptr && this->m_IsActive; }
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
				int x1{ 0 };
				int y1{ 0 };
				int xs{ 0 };
				int ys{ 0 };
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
					if (IntoMouse(x1, y1, x1 + xs, y1 + ys)) {
						DoInClick();
					}

					Info = info;
				}

				void Draw(void) const noexcept {
					auto* Fonts = FontPool::Instance();
					//AnimeSel
					{
						if (IntoMouse(x1, y1, x1 + xs, y1 + ys)) {
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
							DrawBox(x1 + thick, y1 + thick, x1 + xs - thick, y1 + ys - thick, GetColor(255, 255, 255), TRUE);
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						}
						auto length = Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->GetStringWidth(ys, Info);
						if (length > xs / 2) {
							Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(ys, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, x1 + xs, y1, GetColor(255, 255, 255), GetColor(0, 0, 0), Info);
						}
						else {
							Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(ys, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x1 + xs / 2, y1, GetColor(255, 255, 255), GetColor(0, 0, 0), Info);
						}

						Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(ys, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x1, y1, GetColor(255, 255, 255), GetColor(0, 0, 0), Name);
					}
				}
			};
		private:
			ModelControl models;
			GraphControl graphs;
			LoadScriptClass LSClass;		//スクリプト読み込み
			TelopClass TLClass;				//テロップ
			std::vector<Cut_Info_First> m_CutInfo;
			std::vector<Cut_Info_Update> m_CutInfoUpdate;
			CutInfoClass attached;
			std::vector<CutAttachDetail> attachedDetail;
		private:
			int cnt;
			LONGLONG TotalTime, NowTime;
			std::vector<std::string> NAMES;
		private:
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
			//
			const float GetTotalTime(void) const noexcept { return float((GetNowHiPerformanceCount() - TotalTime) / 1000) / 1000.f; }
			const auto GetNowTime(void) noexcept {
				float tim = float((GetNowHiPerformanceCount() - NowTime) / 1000) / 1000.f;
				NowTime = GetNowHiPerformanceCount();
				return tim;
			}
		public:
			const auto& GetFunc() { return LSClass.Getfunc(); }
			const auto& GetArgs() { return LSClass.Getargs(); }
			auto& GetCutInfo() { return m_CutInfo; }
			auto& GetCutInfoUpdate() { return m_CutInfoUpdate; }
			auto& Getmodels() { return models; }
			auto& Getattached() { return attached; }
			const auto& GetattachedDetail() { return attachedDetail[attached.nowcut].poscam; }
			

			void ChamgeAimCampos(size_t Counter, const Vector3DX& pos_t) {
				//auto& pos_t = m_CutInfo[Counter].Aim_camera.GetCamPos();
				auto& vec_t = m_CutInfo[Counter].Aim_camera.GetCamVec();
				auto& up_t = m_CutInfo[Counter].Aim_camera.GetCamUp();
				m_CutInfo[Counter].Aim_camera.SetCamPos(pos_t, vec_t, up_t);
			}
			void ChamgeAimCamvec(size_t Counter, const Vector3DX& vec_t) {
				auto& pos_t = m_CutInfo[Counter].Aim_camera.GetCamPos();
				//auto& vec_t = m_CutInfo[Counter].Aim_camera.GetCamVec();
				auto& up_t = m_CutInfo[Counter].Aim_camera.GetCamUp();
				m_CutInfo[Counter].Aim_camera.SetCamPos(pos_t, vec_t, up_t);
			}
			void ChamgeaNotFirstCampos(size_t Counter, const Vector3DX& pos_t) {
				//auto& pos_t = m_CutInfoUpdate[Counter].CameraNotFirst.GetCamPos();
				auto& vec_t = m_CutInfoUpdate[Counter].CameraNotFirst.GetCamVec();
				auto& up_t = m_CutInfoUpdate[Counter].CameraNotFirst.GetCamUp();
				m_CutInfoUpdate[Counter].CameraNotFirst.SetCamPos(pos_t, vec_t, up_t);
			}
			void ChamgeaNotFirstCamvec(size_t Counter, const Vector3DX& vec_t) {
				auto& pos_t = m_CutInfoUpdate[Counter].CameraNotFirst.GetCamPos();
				//auto& vec_t = m_CutInfoUpdate[Counter].CameraNotFirst.GetCamVec();
				auto& up_t = m_CutInfoUpdate[Counter].CameraNotFirst.GetCamUp();
				m_CutInfoUpdate[Counter].CameraNotFirst.SetCamPos(pos_t, vec_t, up_t);
			}
			void ChamgeaNotFirstCamup(size_t Counter, const Vector3DX& up_t) {
				auto& pos_t = m_CutInfoUpdate[Counter].CameraNotFirst.GetCamPos();
				auto& vec_t = m_CutInfoUpdate[Counter].CameraNotFirst.GetCamVec();
				//auto& up_t = m_CutInfoUpdate[Counter].CameraNotFirst.GetCamUp();
				m_CutInfoUpdate[Counter].CameraNotFirst.SetCamPos(pos_t, vec_t, up_t);
			}
			void UpdateCam(size_t Counter, Camera3DInfo* camera3DInfo) {
				Vector3DX pos_t = camera3DInfo->GetCamPos();
				Vector3DX vec_t = camera3DInfo->GetCamVec();
				Vector3DX up_t = camera3DInfo->GetCamUp();
				float fov_t = camera3DInfo->GetCamFov();
				float near_t = camera3DInfo->GetCamNear();
				float far_t = camera3DInfo->GetCamFar();

				Camera3DInfo& Aim = m_CutInfo[Counter].Aim_camera;

				easing_set_SetSpeed(&pos_t, Aim.GetCamPos(), m_CutInfo[Counter].cam_per);
				easing_set_SetSpeed(&vec_t, Aim.GetCamVec(), m_CutInfo[Counter].cam_per);
				easing_set_SetSpeed(&up_t, Aim.GetCamUp(), m_CutInfo[Counter].cam_per);
				easing_set_SetSpeed(&fov_t, Aim.GetCamFov(), m_CutInfo[Counter].cam_per);
				easing_set_SetSpeed(&far_t, Aim.GetCamFar(), m_CutInfo[Counter].cam_per);
				easing_set_SetSpeed(&near_t, Aim.GetCamNear(), m_CutInfo[Counter].cam_per);
				camera3DInfo->SetCamPos(pos_t, vec_t, up_t);
				camera3DInfo->SetCamInfo(fov_t, near_t, far_t);
			}
		public:
			void StartLoad() {
				TLClass.Init();
				cnt = 0;
				SetUseASyncLoadFlag(TRUE);
				clsDx();
				NowTime = GetNowHiPerformanceCount();
				TotalTime = GetNowHiPerformanceCount();
			}
			//
			void LoadOnce(int mdata) {
				LSClass.LoadScript(getparams::Getstr(mdata));
				PutMsg("ロード%3d : %s\n", ++cnt, GetFunc().c_str());
			}
			void CheckArgments() {
				LSClass.SetArgs(&NAMES);
			}
			void LoadModel() {
				if (GetFunc().find("LoadModel") != std::string::npos) {
					int count = std::stoi(GetArgs()[1]);
					for (int i = 0; i < count; i++) {
						models.Load(GetArgs()[0]);
					}
				}
			}

			void SetNewCut() {
				if (GetFunc().find("SetCut") != std::string::npos) {
					m_CutInfo.resize(m_CutInfo.size() + 1);
					m_CutInfo.back().SetTimeLimit((LONGLONG)(1000000.f * std::stof(GetArgs()[0])));
					m_CutInfoUpdate.resize(m_CutInfoUpdate.size() + 1);
				}
			}
			void SetCamPos_Attach() {
				if (GetFunc().find("SetCampos_Attach") != std::string::npos) {
					auto startFrame = (int)(m_CutInfo.size()) - 1;
					attached.Switch.resize(attached.Switch.size() + 1);
					attached.Switch.back().SetSwitch(startFrame, startFrame + (std::stoi(GetArgs()[0]) - 1));
					attachedDetail.resize(attachedDetail.size() + 1);
					attachedDetail.back().poscam.Set(std::stof(GetArgs()[1]), std::stof(GetArgs()[2]), std::stof(GetArgs()[3]));
				}
			}
			void SetCamPos_None() {
				if (m_CutInfo.size() > 0) {
					if (m_CutInfo.back().LoadScript(GetFunc(), GetArgs())) {
						m_CutInfoUpdate.back().CameraNotFirst = m_CutInfo.back().Aim_camera;
					}
				}
				if (m_CutInfoUpdate.size() > 0) {
					m_CutInfoUpdate.back().LoadScript(GetFunc(), GetArgs());
				}
			}
			void SetDrawGraph() {
				if (GetFunc().find("SetDrawGraph") != std::string::npos) {
					auto startFrame = (int)(m_CutInfo.size()) - 1;
					size_t in_str = GetArgs()[1].find("~");
					if (in_str != std::string::npos) {
						int start_t = std::stoi(GetArgs()[1].substr(0, in_str));
						int end_t = std::stoi(GetArgs()[1].substr(in_str + 1));
						for (int i = start_t; i <= end_t; i++) {
							graphs.Get(GetArgs()[0], i)->Init(startFrame, std::stoi(GetArgs()[2]) - 1);
						}
					}
					else {
						graphs.Get(GetArgs()[0], std::stoi(GetArgs()[1]))->Init(startFrame, std::stoi(GetArgs()[2]) - 1);
					}
				}
				else if (GetFunc().find("SetGraphBlur") != std::string::npos) {
					auto startFrame = (int)(m_CutInfo.size()) - 1;
					graphs.Get(GetArgs()[0], std::stoi(GetArgs()[1]))->CutDetail.back().Blur.Init(startFrame, std::stoi(GetArgs()[2]) - 1);
				}
				else if (GetFunc().find("SetGraphOpacityRate") != std::string::npos) {
					graphs.Get(GetArgs()[0], std::stoi(GetArgs()[1]))->CutDetail.back().OpacityRate_Dist = std::stof(GetArgs()[2]);
					graphs.Get(GetArgs()[0], std::stoi(GetArgs()[1]))->CutDetail.back().OpacityRate_Per = 0.f;
					graphs.Get(GetArgs()[0], std::stoi(GetArgs()[1]))->CutDetail.back().OpacityRate = std::stof(GetArgs()[2]);
				}
				else if (GetFunc().find("SetGraphOpacityEasing") != std::string::npos) {
					graphs.Get(GetArgs()[0], std::stoi(GetArgs()[1]))->CutDetail.back().OpacityRate_Dist = std::stof(GetArgs()[2]);
					graphs.Get(GetArgs()[0], std::stoi(GetArgs()[1]))->CutDetail.back().OpacityRate_Per = std::stof(GetArgs()[3]);
				}
			}
			void SetDrawModel() {
				if (GetFunc().find("SetDrawModel") != std::string::npos) {
					auto startFrame = (int)(m_CutInfo.size()) - 1;
					size_t in_str = GetArgs()[1].find("~");
					if (in_str != std::string::npos) {
						int start_t = std::stoi(GetArgs()[1].substr(0, in_str));
						int end_t = std::stoi(GetArgs()[1].substr(in_str + 1));
						for (int i = start_t; i <= end_t; i++) {
							models.Get(GetArgs()[0], i)->Init(startFrame, std::stoi(GetArgs()[2]) - 1);
						}
					}
					else {
						models.Get(GetArgs()[0], std::stoi(GetArgs()[1]))->Init(startFrame, std::stoi(GetArgs()[2]) - 1);
					}
				}
				else if (GetFunc().find("SetModelAnime") != std::string::npos) {
					auto* t = models.Get(GetArgs()[0], std::stoi(GetArgs()[1]));
					t->CutDetail.back().animsel = std::stoi(GetArgs()[2]);
					t->CutDetail.back().isloop = (GetArgs()[3].find("TRUE") != std::string::npos);
					t->CutDetail.back().animspeed = std::stof(GetArgs()[4]);
					t->CutDetail.back().startframe = std::stof(GetArgs()[5]);
				}
				else if (GetFunc().find("SetModelMat") != std::string::npos) {
					auto* t = models.Get(GetArgs()[0], std::stoi(GetArgs()[1]));
					t->CutDetail.back().usemat = true;

					t->CutDetail.back().Yrad1_p = std::stof(GetArgs()[2]);
					t->CutDetail.back().pos_p = Vector3DX::vget(std::stof(GetArgs()[3]), std::stof(GetArgs()[4]), std::stof(GetArgs()[5]));
					t->CutDetail.back().Yrad2_p = std::stof(GetArgs()[6]);

					t->CutDetail.back().mat_p = Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(t->CutDetail.back().Yrad1_p)) * Matrix4x4DX::Mtrans(t->CutDetail.back().pos_p) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(t->CutDetail.back().Yrad2_p));
				}
				else if (GetFunc().find("SetModelPhysicsSpeed") != std::string::npos) {
					auto* t = models.Get(GetArgs()[0], std::stoi(GetArgs()[1]));
					t->CutDetail.back().PhysicsSpeed_ = std::stof(GetArgs()[2]);
				}
				else if (GetFunc().find("SetModelOpacityRate") != std::string::npos) {
					auto* t = models.Get(GetArgs()[0], std::stoi(GetArgs()[1]));
					t->CutDetail.back().OpacityRate = std::stof(GetArgs()[2]);
				}
				else if (GetFunc().find("SetModelMotionRate") != std::string::npos) {
					auto* t = models.Get(GetArgs()[0], std::stoi(GetArgs()[1]));
					t->CutDetail.back().animspeed_Dist = std::stof(GetArgs()[2]);
					t->CutDetail.back().animspeed_Per = std::stof(GetArgs()[3]);
				}
				else if (GetFunc().find("SetModelOpacityEasing") != std::string::npos) {
					auto* t = models.Get(GetArgs()[0], std::stoi(GetArgs()[1]));
					t->CutDetail.back().OpacityRate_Dist = std::stof(GetArgs()[2]);
					t->CutDetail.back().OpacityRate_Per = std::stof(GetArgs()[3]);
				}
				//どの距離で描画するかをセット
				else if (GetFunc().find("SetModelMode") != std::string::npos) {
					auto* t = models.Get(GetArgs()[0], std::stoi(GetArgs()[1]));
					if (GetArgs()[2] == Model_Type[0]) {
						t->isBGModel = true;
					}
					if (GetArgs()[2] == Model_Type[1]) {
						t->IsNearShadow = false;
					}
					if (GetArgs()[2] == Model_Type[2]) {
						t->IsFarShadow = true;
					}
					if (GetArgs()[2] == Model_Type[3]) {
						t->ShadowDrawActive = false;
					}

				}
			}

			void LoadTelop() {
				TLClass.LoadTelop(GetFunc(), GetArgs());
			}
			//
			void WaitAsyncLoad() {
				SetUseASyncLoadFlag(FALSE);
				PutMsg("非同期読み込みオブジェクトの読み込み待ち…\n");
				{
					int prenum = GetASyncLoadNum();
					int prenumAll = prenum;
					while (ProcessMessage() == 0 && GetASyncLoadNum() != 0) {
						if (prenum != GetASyncLoadNum()) {
							prenum = GetASyncLoadNum();
							PutMsg("ロード%3d完了(%d/%d)\n", ++cnt, prenum, prenumAll);
							continue;
						}
					}
				}
				PutMsg("モデルのセット完了\n");
			}
			void WaitModelSet() {
				models.Set();
				PutMsg("モデルのセット完了\n");
			}
			void WaitMV1Save() {
				PutMsg("モデルのセット完了\n");
				//モデルのMV1保存
				for (auto& n : NAMES) {
					if (n.find(".pmx") != std::string::npos) {
						MV1SaveModelToMV1File(models.Get(n, 0)->obj.GetHandle(), (n.substr(0, n.find(".pmx")) + ".mv1").c_str(), MV1_SAVETYPE_NORMAL, -1, 1, 1, 1, 0, 0);
					}
				}
				PutMsg("モデルのMV1変換完了\n");
			}
			//
			void Start(size_t StartCut) {
				models.Start(StartCut);
				graphs.Start(StartCut);
				attached.Start(StartCut);
			}
			//
			void FirstUpdate(size_t Counter, bool isFirstLoop) {
				if (attached.Update_(Counter)) {
					attached.Update_(Counter);
				}
				models.FirstUpdate(Counter, isFirstLoop);
				graphs.FirstUpdate(Counter, isFirstLoop);
#ifdef EditMode
				Editer_Update();
#endif
			}
			void LateUpdate(bool isResetPhysics) {
				graphs.Update();
				models.Update();
				models.SetPhysics(isResetPhysics);
			}
			//
			void			BG_Draw(void) noexcept {
				models.Draw_Far();
			}
			void			ShadowDraw_Far(void) noexcept {
				SetDrawAlphaTest(DX_CMP_GREATER, 128);
				models.Draw(false, true, true);
				SetDrawAlphaTest(-1, 0);
			}
			void			ShadowDraw_NearFar(void) noexcept {
				models.Draw(false, false, true, TRUE);
				//models.Draw(true, false, true, FALSE);
			}
			void			ShadowDraw(void) noexcept {
				models.Draw(true, false, true, FALSE);
			}
			void			MainDraw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				models.CheckInCamera(DrawParts->GetMainCamera().GetCamFar());
				//+201 = 67x3
				auto camfar = GetCameraFar();
				//カメラのfarが適正範囲の設定の場合
				//near
				if (DrawParts->GetMainCamera().GetCamFar() - 1.f < camfar&& camfar < DrawParts->GetMainCamera().GetCamFar() + 1.f) {
					models.Draw(false, false, false, FALSE);
				}
				else {
					models.Draw(false, false, false, TRUE);
				}
			}
			void			MainDrawGraph(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				graphs.Draw(DrawParts->GetScreenY(1920));
			}
			void			DrawUI(LONGLONG NowTimeWait) noexcept {
				TLClass.Draw(NowTimeWait);
			}
			//
			void Dispose() {
				m_CutInfo.clear();
				m_CutInfoUpdate.clear();
#ifdef EditMode
				Editer_Dispose();
#endif
			}

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
			std::vector<Cut_Info_First> m_CutInfo_Buf;				//
			std::vector<Cut_Info_Update> m_CutInfoUpdate_Buf;		//
			switchs MouseClick;										//
		private:
			//calc
			int OffsetCalc(size_t Counter, LONGLONG NowTimeWait, int x_p, int x_s)noexcept {
				LONGLONG now2 = (Counter >= 1) ? m_CutInfo[Counter - 1].GetTimeLimit() : 0;
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
							int sel = i - 2;
							if (i >= 2) {
								LONGLONG base = m_CutInfo.at(static_cast<size_t>(sel)).GetTimeLimit();
								LONGLONG now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
								x1 += BaseWidth * (int)(now - base) / WidthPer;
							}
							else if (i >= 1) {
								LONGLONG now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
								x1 += BaseWidth * (int)(now) / WidthPer;
							}
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
							int sel = i - 2;
							if (i >= 2) {
								LONGLONG base = m_CutInfo.at(static_cast<size_t>(sel)).GetTimeLimit();
								LONGLONG now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
								x1 += BaseWidth * (int)(now - base) / WidthPer;
							}
							else if (i >= 1) {
								LONGLONG now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
								x1 += BaseWidth * (int)(now) / WidthPer;
							}
							//NOW
							if (i == Counter) {
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
							int sel = i - 2;
							if (i >= 2) {
								LONGLONG base = m_CutInfo.at(static_cast<size_t>(sel)).GetTimeLimit();
								LONGLONG now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
								x1 += BaseWidth * (int)(now - base) / WidthPer;
							}
							else if (i >= 1) {
								LONGLONG now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
								x1 += BaseWidth * (int)(now) / WidthPer;
							}
							if (x1 > x_p) {
								CutNow = std::max(i - 1, 0);
								break;
							}
							i++;
							if (static_cast<size_t>(i - 1) >= m_CutInfo.size()) { break; }
						}
					}
					//現在地再計算
					{
						int x1 = x_p;
						int i = CutNow;
						while (true) {
							int sel = i - 2;
							if (i >= 2) {
								LONGLONG base = m_CutInfo.at(static_cast<size_t>(sel)).GetTimeLimit();
								LONGLONG now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
								x1 += BaseWidth * (int)(now - base) / WidthPer;
							}
							else if (i >= 1) {
								LONGLONG now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
								x1 += BaseWidth * (int)(now) / WidthPer;
							}
							//NOW
							if (i == Counter) {
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
				auto* DrawParts = DXDraw::Instance();
				if (MouseClick.press()) {
					if (MouseClick.trigger()) {
						int x1 = x_p + X_now;
						int i = CutNow;
						int y_hight = DrawParts->GetScreenY(24);
						while (true) {
							int sel = i - 2;
							LONGLONG now = 0;
							if (i >= 2) {
								LONGLONG base = m_CutInfo.at(static_cast<size_t>(sel)).GetTimeLimit();
								now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
								x1 += BaseWidth * (int)(now - base) / WidthPer;
							}
							else if (i >= 1) {
								now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
								x1 += BaseWidth * (int)(now) / WidthPer;
							}
							if (x1 > x_p + x_s) { break; }
							//LINE
							if (x1 > x_p) {
								int width_Next = BaseWidth * (int)(m_CutInfo[i].GetTimeLimit() - now) / WidthPer;
								if (IntoMouse(x1, y_p + y_s, x1 + width_Next, y_p + y_s + y_hight)) {
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
						/*
						m_Counter = changeSeekID;
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
						if (Start.on()) {
							ModelEditIn = true;
						}
						//*/
						//Start(m_Counter);
						m_CutInfo = m_CutInfo_Buf;
						m_CutInfoUpdate = m_CutInfoUpdate_Buf;
						ModelEdit_PhysicsReset = true;
						changeSeekID = -1;
					}
				}
			}
			//
			void Editer_Update() {
				MouseClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
			}
			void Editer_Calc(size_t Counter, LONGLONG NowTimeWait)noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* Pad = PadControl::Instance();

				BaseWidth = DrawParts->GetScreenX(1920) / 64;

				LookMovie.Execute(CheckHitKeyWithCheck(KEY_INPUT_M) != 0);
				LookEditer.Execute(CheckHitKeyWithCheck(KEY_INPUT_N) != 0);


				if ((LookMovie.trigger() && LookMovie.on()) || (0 <= NowTimeWait && NowTimeWait <= ((1000 * 1000) / 60))) {
					PlayMovieToGraph(movie.get(), 2, DX_MOVIEPLAYTYPE_BCANCEL);
					ChangeMovieVolumeToGraph(0, movie.get());
					SeekMovieToGraph(movie.get(), (int)(NowTimeWait / 1000));
				}
				if (LookEditer.on()) {
					//編集画面
					{
						int x_p = DrawParts->GetScreenX(1920) * 1 / 10;
						int x_s = DrawParts->GetScreenX(1920) * 8 / 10;
						int y_p = DrawParts->GetScreenY(1920) * 5 / 10;
						int y_s = DrawParts->GetScreenY(1920) * 4 / 10;

						int hight = y_s / (int)(models.GetMax());

						LONGLONG now2 = (Counter >= 1) ? m_CutInfo[Counter - 1].GetTimeLimit() : 0;
						int width_Time = BaseWidth * (int)(NowTimeWait - now2) / WidthPer;

						x_now = OffsetCalc(Counter, NowTimeWait, x_p, x_s);
						//判定演算
						if (!m_EditModel.m_IsActive) {
							SeekBer_Calc(x_p, x_s, y_p, y_s);
							if (!PressSeek) {
								if (MouseClick.press()) {
									int x1 = x_p + X_now;
									int i = CutNow;
									while (true) {
										int sel = i - 2;
										LONGLONG now = 0;
										if (i >= 2) {
											LONGLONG base = m_CutInfo.at(static_cast<size_t>(sel)).GetTimeLimit();
											now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
											x1 += BaseWidth * (int)(now - base) / WidthPer;
										}
										else if (i >= 1) {
											now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
											x1 += BaseWidth * (int)(now) / WidthPer;
										}

										if (x1 > x_p + x_s) { break; }
										if (x1 > x_p) {
											int width_Next = BaseWidth * (int)(m_CutInfo[i].GetTimeLimit() - now) / WidthPer;
											int msel = std::min((Pad->GetMS_Y() - y_p) / hight, (int)(models.GetMax()) - 1);
											if (msel >= 0) {
												int y1 = y_p + msel * hight;
												int xx = x1;
												if (IntoMouse(xx, y1, x1 + width_Next, y1 + hight - 1)) {
													if (x_now >= 0) {
														xx = std::max(xx, x_now + X_now + width_Time);
													}
												}
												if (xx < x1 + width_Next) {
													if (IntoMouse(xx, y1, x1 + width_Next, y1 + hight - 1)) {
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
										if (m_ChangeModel.IsAnimIn((int)Counter)) {
											ModelEdit_PhysicsReset = true;
										}
									}
									//else if (m_EditModel.Switch() && Start.on()) {
									//	ModelEditIn = true;
									//}
								}
							}
						}
						//モデルエディットモード判定(参照変更)
						if (m_EditModel.IsEditMode() && MouseClick.press()) {
							int x1 = x_p + X_now;
							int i = CutNow;
							while (true) {
								int sel = i - 2;
								LONGLONG now = 0;
								if (i >= 2) {
									LONGLONG base = m_CutInfo.at(static_cast<size_t>(sel)).GetTimeLimit();
									now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
									x1 += BaseWidth * (int)(now - base) / WidthPer;
								}
								else if (i >= 1) {
									now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
									x1 += BaseWidth * (int)(now) / WidthPer;
								}
								if (x1 > x_p + x_s) { break; }
								if (x1 > x_p) {
									int width_Next = BaseWidth * (int)(m_CutInfo[i].GetTimeLimit() - now) / WidthPer;
									int msel = std::min((Pad->GetMS_Y() - y_p) / hight, (int)(models.GetMax()) - 1);
									if (msel >= 0) {
										int y1 = y_p + msel * hight;
										if (IntoMouse(x1, y1, x1 + width_Next, y1 + hight)) {
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
						int x_p = DrawParts->GetScreenX(1920) * 5 / 10;
						int x_s = DrawParts->GetScreenX(1920) * 4 / 10;
						int y_p = DrawParts->GetScreenY(1920) * 2 / 10;
						int y_s = DrawParts->GetScreenY(1920) * 5 / 20;
						//EndMode
						{
							int x1 = x_p + x_s - 32 - 6;
							int y1 = y_p + y_s - 32 - 6;
							int x2 = x1 + 32;
							int y2 = y1 + 32;
							if (MouseClick.trigger() && IntoMouse(x1, y1, x2, y2)) {
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
									++c.animsel %= m_EditModel.m_Ptr->obj.GetAnimNum();
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
									sprintfDx(buf, "%5.2f°(%5.2f,%5.2f,%5.2f) %5.2f°", c.Yrad1_p, c.pos_p.x, c.pos_p.y, c.pos_p.z, c.Yrad2_p);
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
			void Editer_Draw(size_t Counter, LONGLONG NowTimeWait)noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* Pad = PadControl::Instance();

				if (LookEditer.on()) {
					//編集画面
					{
						int x_p = DrawParts->GetScreenX(1920) * 1 / 10;
						int x_s = DrawParts->GetScreenX(1920) * 8 / 10;
						int y_p = DrawParts->GetScreenY(1920) * 5 / 10;
						int y_s = DrawParts->GetScreenY(1920) * 4 / 10;

						int hight = y_s / (int)(models.GetMax());

						LONGLONG now2 = (Counter >= 1) ? m_CutInfo[Counter - 1].GetTimeLimit() : 0;
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
									int sel = i - 2;
									LONGLONG now = 0;
									if (i >= 2) {
										LONGLONG base = m_CutInfo.at(static_cast<size_t>(sel)).GetTimeLimit();
										now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
										x1 += BaseWidth * (int)(now - base) / WidthPer;
									}
									else if (i >= 1) {
										now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
										x1 += BaseWidth * (int)(now) / WidthPer;
									}
									if (x1 > x_p + x_s) { break; }
									//LINE
									if (x1 > x_p) {
										DrawLine(x1, y_p, x1, y_p + y_s, GetColor(128, 128, 128), 3);
										int width_Next = BaseWidth * (int)(m_CutInfo[i].GetTimeLimit() - now) / WidthPer;

										int xx = x1;
										if (IntoMouse(xx, y_p, x1 + width_Next, y_p + y_s)) {
											if (x_now >= 0) {
												xx = std::max(xx, x_now + X_now + width_Time);
											}
										}
										if (xx < x1 + width_Next) {
											if (!m_EditModel.m_IsActive && IntoMouse(xx, y_p, x1 + width_Next, y_p + y_s - 1)) {
												int y1 = y_p + ((Pad->GetMS_Y() - y_p) / hight) * hight;
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
								int y_hight = DrawParts->GetScreenY(24);

								DrawBox(x_p, y_p + y_s, x_p + x_s, y_p + y_s + y_hight, GetColor(0, 0, 0), TRUE);

								while (true) {
									int sel = i - 2;
									LONGLONG now = 0;
									if (i >= 2) {
										LONGLONG base = m_CutInfo.at(static_cast<size_t>(sel)).GetTimeLimit();
										now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
										x1 += BaseWidth * (int)(now - base) / WidthPer;
									}
									else if (i >= 1) {
										now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
										x1 += BaseWidth * (int)(now) / WidthPer;
									}
									if (x1 > x_p + x_s) { break; }
									//LINE
									if (x1 > x_p) {
										int width_Next = BaseWidth * (int)(m_CutInfo[i].GetTimeLimit() - now) / WidthPer;

										if (i == Counter) {
											SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
											DrawBox(x1, y_p + y_s, x1 + width_Next, y_p + y_s + y_hight, GetColor(255, 255, 255), TRUE);
											SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
										}

										DrawLine(x1, y_p + y_s, x1, y_p + y_s + y_hight, GetColor(128, 128, 128), 1);

										if (!m_EditModel.m_IsActive && IntoMouse(x1, y_p + y_s, x1 + width_Next, y_p + y_s + y_hight)) {
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
								int sel = i - 2;
								LONGLONG now = 0;
								if (i >= 2) {
									LONGLONG base = m_CutInfo.at(static_cast<size_t>(sel)).GetTimeLimit();
									now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
									x1 += BaseWidth * (int)(now - base) / WidthPer;
								}
								else if (i >= 1) {
									now = m_CutInfo.at(static_cast<size_t>(sel) + 1).GetTimeLimit();
									x1 += BaseWidth * (int)(now) / WidthPer;
								}

								int width_Next = BaseWidth * (int)(m_CutInfo[i].GetTimeLimit() - now) / WidthPer;
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
													color = c.IsIn(Counter) ? GetColor(100, 216, 100) : GetColor(60, 60, 192);
												}
												else {
													color = c.IsIn(Counter) ? GetColor(150, 255, 150) : GetColor(100, 100, 255);
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
										if (c.IsIn(Counter)) {
											SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
											break;
										}
									}
									Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(hight, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x_p, y1, GetColor(255, 255, 255), GetColor(0, 0, 0), " " + sel->Base + "(" + std::to_string(m.BaseID) + ")");
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
						int x_p = DrawParts->GetScreenX(1920) * 5 / 10;
						int x_s = DrawParts->GetScreenX(1920) * 4 / 10;
						int y_p = DrawParts->GetScreenY(1920) * 2 / 10;
						int y_s = DrawParts->GetScreenY(1920) * 5 / 20;

						int hight = DrawParts->GetScreenY(20);
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
								if (IntoMouse(x1, y1, x2, y2)) {
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
										Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(hight, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x_p + x_s / 2, y1, GetColor(255, 255, 255), GetColor(0, 0, 0), sel->Base + "(" + std::to_string(m_EditModel.m_Ptr->BaseID) + ")");
									}
									Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(hight, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x_p, y1, GetColor(255, 255, 255), GetColor(0, 0, 0), "Name");
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
					//printfDx("FPS   : %5.2f\n", DrawParts->GetFps());
					printfDx("Cut   : %d\n", Counter);
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
					//movie.DrawExtendGraph(DrawParts->GetScreenX(1920) * 3 / 4, DrawParts->GetScreenY(1920) * 3 / 4, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), FALSE);
				}
			}

			void Editer_Dispose() {
				movie.Dispose();
			}

		public:
			const auto& GetEditIsActive() const noexcept { return m_EditModel.m_IsActive; }

			const auto PutModelEditIn() {
				bool ret = ModelEditIn;
				ModelEditIn = false;
				return ret;
			}
			const auto PutModelEdit_PhysicsReset() {
				bool ret = ModelEdit_PhysicsReset;
				ModelEdit_PhysicsReset = false;
				return ret;
			}
		public:
			void Editer_Init(void) noexcept {
				m_CutInfo_Buf = m_CutInfo;
				m_CutInfoUpdate_Buf = m_CutInfoUpdate;

				auto* DrawParts = DXDraw::Instance();

				int x_p = DrawParts->GetScreenX(1920) * 5 / 10;
				int x_s = DrawParts->GetScreenX(1920) * 4 / 10;
				int y_p = DrawParts->GetScreenY(1920) * 2 / 10;

				int hight = DrawParts->GetScreenY(20);

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
			}

			void Editer_UpdateMovie(int time) {
				if (GetMovieStateToGraph(movie.get()) == 0) {
					PlayMovieToGraph(movie.get(), 2, DX_MOVIEPLAYTYPE_BCANCEL);
					SeekMovieToGraph(movie.get(), time);
				}
				SetPlaySpeedRateMovieToGraph(movie.get(), (double)GameSpeed);
			}

			void Editer_DrawUI(size_t Counter, LONGLONG NowTimeWait) {
				if (LookMovie.on()) {
					auto* DrawParts = DXDraw::Instance();
					movie.DrawExtendGraph(0, 0, DrawParts->GetScreenX(1920) / 2, DrawParts->GetScreenY(1920) / 2, FALSE);
				}
				clsDx();
				//printfDx("Time : %.2f\n\n", (float)(NowTimeWait/1000)/1000.f);
				Editer_Calc(Counter, NowTimeWait);

				Editer_Draw(Counter, NowTimeWait);
			}

			void Editer_StopMovie() {
				if (GetMovieStateToGraph(movie.get()) == 1) {
					PauseMovieToGraph(movie.get());
				}
			}
#endif

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
			void			BG_Draw_Sub(void) noexcept override;
			void			ShadowDraw_Far_Sub(void) noexcept override;
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept  override;
			void			DrawUI_In_Sub(void) noexcept override {}
		private:
			//std::string LOGO1 = "data/picture/logo.png";
			std::string Suit = "data/model/Suit/model.mv1";
			std::string Soldier = "data/model/Soldier/model.mv1";
			LoadUtil m_LoadUtil;
		private:
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
			//
			Camera3DInfo camera_buf;								//
			int fog[3]{ -1,-1,-1 };									//
			float fog_range[2]{ -1.f,-1.f };						//
			std::vector<size_t> RankID;								//
			size_t camsel{ 0 }, camsel_buf{ 0 };					//
			float Per_Change{ 1.f };								//
			float Change1Time{ 1.f };								//
			size_t m_Counter{ 0 };									//カット
			LONGLONG BaseTime{ 0 }, WaitTime{ 0 }, NowTimeWait{ 0 };//
			bool ResetPhysics{ true };								//
			bool isFirstLoop{ true };								//カット最初のループか
			bool isfast{ true };									//
			bool issecond{ true };									//
			SoundHandle BGM;										//データ

			int BGM_Frequency;										//
			switchs ChangeCamSel, ChangeStart;						//
			switchs SpeedUp, SpeedDown;								//
			switchs Start;											//
			int spd_x{ 10 };										//
			bool isFreepos{ false };								//
			int x_m{ 0 }, y_m{ 0 };									//
			int x_sav{ 0 }, y_sav{ 0 };								//
			float x_rm{ 0.f }, y_rm{ 0.f }, r_rm{ 100.f };			//
			size_t CutSel_Buf{ 0 };									//
			Vector3DX m_RandcamupBuf;								//
			Vector3DX m_RandcamvecBuf;								//
			Vector3DX m_RandcamposBuf;								//
			float Black_Buf{ 1.f };									//
			float White_Buf{ 0.f };									//
		public:
		};
	};
};
