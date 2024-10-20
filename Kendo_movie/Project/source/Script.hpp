#pragma once
#include	"Header.hpp"

#include "sub.hpp"
#include "CutIn.hpp"
#include "Editer.hpp"
//
namespace FPS_n2 {
	class LoadScriptClass : public SingletonBase<LoadScriptClass> {
	private:
		friend class SingletonBase<LoadScriptClass>;
	private:
	private:
		struct VARIABLE {
			std::string Base;
			std::string After;
		};
	private:
		std::vector<VARIABLE> m_Variable;
		std::string m_Func;
		std::vector<std::string> m_Args;
		std::vector<std::string> m_NAMES;
	private:
		static void			Sub_Func(std::string& func_t, const char& in_str) noexcept {
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
		const auto& Getfunc(void) const noexcept { return m_Func; }
		const auto& Getargs(void) const noexcept { return m_Args; }
		const auto& GetNames(void) const noexcept { return m_NAMES; }
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
		void			LoadScript(std::string_view func_t) noexcept {
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
		bool SetArgs() noexcept {
			//変数登録
			if (m_Func.find("SetArg") != std::string::npos) {
				m_Variable.resize(m_Variable.size() + 1);
				m_Variable.back().Base = m_Args[0];
				if (m_Args[1].find(".pmx") != std::string::npos) {
					auto mv1s = m_Args[1].substr(0, m_Args[1].find(".pmx")) + ".mv1";
					if (std::filesystem::exists(mv1s.c_str())) {
						m_Variable.back().After = mv1s;
					}
					else {
						m_Variable.back().After = m_Args[1];
					}

					m_NAMES.emplace_back(m_Variable.back().After);
				}
				else {
					m_Variable.back().After = m_Args[1];
				}
				return true;
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
				return false;
			}
			//
		}
	};
	class LoadUtil {
	public:
		std::vector<Cut_Info_First>		m_CutInfo;
		std::vector<Cut_Info_Update>	m_CutInfoUpdate;
		CutInfoClass					m_attached;
		std::vector<Vector3DX>			m_PosCam;
		LONGLONG						m_NowTime{ 0 };
		Vector3DX		m_RandcamupBuf;
		Vector3DX		m_RandcamvecBuf;
		Vector3DX		m_RandcamposBuf;
	public:
		auto			GetNowTime(size_t Counter) { return (Counter >= 0 ? m_CutInfo[Counter].GetTimeLimit() : 0); }
		auto			IsEnd(size_t Counter) { return (Counter >= m_CutInfo.size()); }
		auto			IsResetPhysics(size_t Counter) { return m_CutInfo[Counter].IsResetPhysics; }
	public:
		void			Load(void) noexcept {
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			//auto* GraphParts = FPS_n2::GraphControl::Instance();
			//GraphParts->Load((float)(DrawParts->GetScreenY(1920 * 1 / 2)), (float)(DrawParts->GetScreenY(1080 * 1 / 2)), 0, 1.f, 0.5f, LOGO1);
			int mdata = FileRead_open("data/Cut.txt", FALSE);
			TelopClass::Instance()->Init();
			int counts = 0;
			SetUseASyncLoadFlag(TRUE);
			clsDx();
			m_NowTime = GetNowHiPerformanceCount();
			while (FileRead_eof(mdata) == 0) {
				FPS_n2::LoadScriptClass::Instance()->LoadScript(getparams::Getstr(mdata));
				const auto& func = FPS_n2::LoadScriptClass::Instance()->Getfunc();
				const auto& args = FPS_n2::LoadScriptClass::Instance()->Getargs();
				if ((func == "")) { continue; }
				PutMsg("ロード%3d : %s\n", ++counts, func.c_str());
				//変数登録
				FPS_n2::LoadScriptClass::Instance()->SetArgs();
				//モデル読み込み
				if (func.find("LoadModel") != std::string::npos) {
					int count = std::stoi(args[1]);
					for (int i = 0; i < count; i++) {
						ModelParts->Load(args[0]);
					}
				}
				//新規カット
				else if (func.find("SetCut") != std::string::npos) {
					m_CutInfo.emplace_back(Cut_Info_First((LONGLONG)(1000000.f * std::stof(args[0]))));
					m_CutInfoUpdate.resize(m_CutInfoUpdate.size() + 1);
				}
				//Camposの指定
				else if (func.find("SetCampos_Attach") != std::string::npos) {
					auto startFrame = (int)(m_CutInfo.size()) - 1;
					m_attached.Switch.emplace_back(CutInfoClass::On_Off(startFrame, startFrame + (std::stoi(args[0]) - 1)));
					m_PosCam.emplace_back(Vector3DX::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])));
				}
				if (m_CutInfo.size() > 0) {
					if (m_CutInfo.back().LoadScript(func, args)) {
						m_CutInfoUpdate.back().CameraNotFirst = m_CutInfo.back().Aim_camera;
					}
				}
				if (m_CutInfoUpdate.size() > 0) {
					m_CutInfoUpdate.back().LoadScript(func, args);
				}
				//画像描画
				SetDrawGraph();
				//モデル描画
				SetDrawModel();
				//テロップ
				TelopClass::Instance()->LoadTelop(func, args);
				//
				if (ProcessMessage() != 0) {}
			}
			FileRead_close(mdata);
			//
			SetUseASyncLoadFlag(FALSE);
			PutMsg("非同期読み込みオブジェクトの読み込み待ち…\n");
			{
				int prenum = GetASyncLoadNum();
				int prenumAll = prenum;
				while (ProcessMessage() == 0 && GetASyncLoadNum() != 0) {
					if (prenum != GetASyncLoadNum()) {
						prenum = GetASyncLoadNum();
						PutMsg("ロード%3d完了(%d/%d)\n", ++counts, prenum, prenumAll);
						continue;
					}
				}
			}
			PutMsg("モデルのセット完了\n");
			ModelParts->Set();
			//モデルのMV1保存
			for (auto& n : FPS_n2::LoadScriptClass::Instance()->GetNames()) {
				if (n.find(".pmx") != std::string::npos) {
					ModelParts->Get(n, 0)->obj.SaveModelToMV1File((n.substr(0, n.find(".pmx")) + ".mv1").c_str(), MV1_SAVETYPE_NORMAL, -1, 1, 1, 1, 0, 0);
				}
			}
			PutMsg("モデルのMV1変換完了\n");
		}
		void			Start(size_t StartCut) {
			m_attached.Start(StartCut);
			m_RandcamupBuf = Vector3DX::zero();
			m_RandcamvecBuf = Vector3DX::zero();
			m_RandcamposBuf = Vector3DX::zero();
		}
		void			FirstUpdate(size_t Counter) {
			m_attached.Update_(Counter);
		}
		void			Flip(size_t Counter, FogParam* pFog) {
			SetupByPrev(Counter);
			auto& ci = m_CutInfo[Counter];
			if (ci.Fog.fog[0] >= 0) {
				*pFog = ci.Fog;
			}
			else if (ci.Fog.fog[0] == -2) {
				pFog->Reset();
			}
			ResetRandom(Counter);
			UpdateCam(Counter);
		}
		void			Update(size_t Counter, float* Black_Buf, float* White_Buf) {
			auto* DrawParts = DXDraw::Instance();
			auto& ci = m_CutInfo[Counter];
			auto& u = m_CutInfoUpdate[Counter];
			u.Update(ci, &m_RandcamupBuf, &m_RandcamvecBuf, &m_RandcamposBuf);
			u.CameraNotFirst.SetCamPos(
				u.CameraNotFirst.GetCamPos() + u.CameraNotFirst_Vec.GetCamPos() * (1.f / DrawParts->GetFps() * GameSpeed),
				u.CameraNotFirst.GetCamVec() + u.CameraNotFirst_Vec.GetCamVec() * (1.f / DrawParts->GetFps() * GameSpeed),
				u.CameraNotFirst.GetCamUp() + u.CameraNotFirst_Vec.GetCamUp() * (1.f / DrawParts->GetFps() * GameSpeed)
			);
			//
			if (m_attached.GetSwitch()) {
				ci.Aim_camera.SetCamPos(ci.Aim_camera.GetCamVec() + m_PosCam[m_attached.nowcut], ci.Aim_camera.GetCamVec(), ci.Aim_camera.GetCamUp());
			}
			easing_set_SetSpeed(Black_Buf, u.Black, u.Black_Per);
			easing_set_SetSpeed(White_Buf, u.White, u.White_Per);
			UpdateCam(Counter);
		}
		void			Dispose(void) noexcept {
			m_CutInfo.clear();
			m_CutInfoUpdate.clear();
#ifdef EditMode
			m_LoadEditUtil.Editer_Dispose();
#endif
		}
	private:
		template <typename... Args>
		void			PutMsg(std::string FormatString, Args&&... args) {
			float tim = static_cast<float>((GetNowHiPerformanceCount() - m_NowTime) / 1000) / 1000.f;
			m_NowTime = GetNowHiPerformanceCount();
			if (tim >= 0.001f) {
				GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
			}
			printfDx(("[NOW %7.3f s] : " + FormatString).c_str(), tim, args...);
			if (tim >= 0.001f) {
				auto* DrawParts = DXDraw::Instance();
				DrawParts->Screen_Flip();
			}
		}
		//
		void			SetupByPrev(size_t Counter) {
			auto& ci = m_CutInfo[Counter];
			auto& u = m_CutInfoUpdate[Counter];
			if (Counter > 0) {
				ci.SetPrev(m_CutInfo[Counter - 1]);
			}
			if (u.IsUsePrevBuf) {
				//
				auto White_Set = u.IsSetWhite;
				auto White_Per = u.White_Per;
				auto WhitePrev = u.White;

				auto Black_Set = u.IsSetBlack;
				auto Black_Per = u.Black_Per;
				auto BlackPrev = u.Black;
				//
				u = m_CutInfoUpdate[Counter - 1];
				//
				if (White_Set) {
					u.IsSetWhite = White_Set;
					u.White_Per = White_Per;
					u.White = WhitePrev;
				}
				if (Black_Set) {
					u.IsSetBlack = Black_Set;
					u.Black_Per = Black_Per;
					u.Black = BlackPrev;
				}
			}
		}
		void			ResetRandom(size_t Counter) {
			auto& ci = m_CutInfo[Counter];
			auto& u = m_CutInfoUpdate[Counter];
			{
				Vector3DX vec;
				bool isforcus = false;
				for (auto& f : ci.Forcus) {
					if (f.GetIsUse()) {
						vec += f.GetForce();
						isforcus = true;
					}
				}
				if (isforcus) {
					ci.Aim_camera.SetCamPos(ci.Aim_camera.GetCamPos(), vec / (float)(ci.Forcus.size()), ci.Aim_camera.GetCamUp());
				}
			}
			//
			if (m_attached.GetSwitch()) {
				ci.Aim_camera.SetCamPos(ci.Aim_camera.GetCamVec() + m_PosCam[m_attached.nowcut], ci.Aim_camera.GetCamVec(), ci.Aim_camera.GetCamUp());
			}
			if (ci.isResetRandCampos) { m_RandcamposBuf = Vector3DX::zero(); }
			if (ci.isResetRandCamvec) { m_RandcamvecBuf = Vector3DX::zero(); }
			if (ci.isResetRandCamup) { m_RandcamupBuf = Vector3DX::zero(); }
			u.CameraNotFirst.SetCamPos(ci.Aim_camera.GetCamPos(), ci.Aim_camera.GetCamVec(), u.CameraNotFirst.GetCamUp());
		}
		void			UpdateCam(size_t Counter) {
			auto* DrawParts = DXDraw::Instance();
			auto& ci = m_CutInfo[Counter];
			Vector3DX pos_t = DrawParts->SetMainCamera().GetCamPos();
			Vector3DX vec_t = DrawParts->SetMainCamera().GetCamVec();
			Vector3DX up_t = DrawParts->SetMainCamera().GetCamUp();
			float fov_t = DrawParts->SetMainCamera().GetCamFov();
			float near_t = DrawParts->SetMainCamera().GetCamNear();
			float far_t = DrawParts->SetMainCamera().GetCamFar();

			easing_set_SetSpeed(&pos_t, ci.Aim_camera.GetCamPos(), ci.cam_per);
			easing_set_SetSpeed(&vec_t, ci.Aim_camera.GetCamVec(), ci.cam_per);
			easing_set_SetSpeed(&up_t, ci.Aim_camera.GetCamUp(), ci.cam_per);
			easing_set_SetSpeed(&fov_t, ci.Aim_camera.GetCamFov(), ci.cam_per);
			easing_set_SetSpeed(&far_t, ci.Aim_camera.GetCamFar(), ci.cam_per);
			easing_set_SetSpeed(&near_t, ci.Aim_camera.GetCamNear(), ci.cam_per);
			DrawParts->SetMainCamera().SetCamPos(pos_t, vec_t, up_t);
			DrawParts->SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
		}
		//
		void			SetDrawGraph(void) noexcept {
			const auto& func = FPS_n2::LoadScriptClass::Instance()->Getfunc();
			const auto& args = FPS_n2::LoadScriptClass::Instance()->Getargs();
			auto* GraphParts = FPS_n2::GraphControl::Instance();
			if (func.find("SetDrawGraph") != std::string::npos) {
				auto startFrame = (int)(m_CutInfo.size()) - 1;
				size_t in_str = args[1].find("~");
				if (in_str != std::string::npos) {
					int start_t = std::stoi(args[1].substr(0, in_str));
					int end_t = std::stoi(args[1].substr(in_str + 1));
					for (int i = start_t; i <= end_t; i++) {
						GraphParts->Get(args[0], i)->Init(startFrame, std::stoi(args[2]) - 1);
					}
				}
				else {
					GraphParts->Get(args[0], std::stoi(args[1]))->Init(startFrame, std::stoi(args[2]) - 1);
				}
			}
			else if (func.find("SetGraphBlur") != std::string::npos) {
				auto* t = GraphParts->Get(args[0], std::stoi(args[1]));
				auto startFrame = (int)(m_CutInfo.size()) - 1;
				t->CutDetail.back().Blur.Init(startFrame, std::stoi(args[2]) - 1);
			}
			else if (func.find("SetGraphOpacityRate") != std::string::npos) {
				auto* t = GraphParts->Get(args[0], std::stoi(args[1]));
				t->CutDetail.back().OpacityRate_Dist = std::stof(args[2]);
				t->CutDetail.back().OpacityRate_Per = 0.f;
				t->CutDetail.back().OpacityRate = std::stof(args[2]);
			}
			else if (func.find("SetGraphOpacityEasing") != std::string::npos) {
				auto* t = GraphParts->Get(args[0], std::stoi(args[1]));
				t->CutDetail.back().OpacityRate_Dist = std::stof(args[2]);
				t->CutDetail.back().OpacityRate_Per = std::stof(args[3]);
			}
		}
		void			SetDrawModel(void) noexcept {
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			const auto& func = FPS_n2::LoadScriptClass::Instance()->Getfunc();
			const auto& args = FPS_n2::LoadScriptClass::Instance()->Getargs();
			if (func.find("SetDrawModel") != std::string::npos) {
				auto startFrame = (int)(m_CutInfo.size()) - 1;
				size_t in_str = args[1].find("~");
				if (in_str != std::string::npos) {
					int start_t = std::stoi(args[1].substr(0, in_str));
					int end_t = std::stoi(args[1].substr(in_str + 1));
					for (int i = start_t; i <= end_t; i++) {
						ModelParts->Get(args[0], i)->Init(startFrame, std::stoi(args[2]) - 1);
					}
				}
				else {
					ModelParts->Get(args[0], std::stoi(args[1]))->Init(startFrame, std::stoi(args[2]) - 1);
				}
			}
			else if (func.find("SetModelAnime") != std::string::npos) {
				auto* t = ModelParts->Get(args[0], std::stoi(args[1]));
				t->CutDetail.back().animsel = std::stoi(args[2]);
				t->CutDetail.back().isloop = (args[3].find("TRUE") != std::string::npos);
				t->CutDetail.back().animspeed = std::stof(args[4]);
				t->CutDetail.back().startframe = std::stof(args[5]);
			}
			else if (func.find("SetModelMat") != std::string::npos) {
				auto* t = ModelParts->Get(args[0], std::stoi(args[1]));
				t->CutDetail.back().usemat = true;

				t->CutDetail.back().Yrad1_p = std::stof(args[2]);
				t->CutDetail.back().pos_p = Vector3DX::vget(std::stof(args[3]), std::stof(args[4]), std::stof(args[5]));
				t->CutDetail.back().Yrad2_p = std::stof(args[6]);

				t->CutDetail.back().mat_p = Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(t->CutDetail.back().Yrad1_p)) * Matrix4x4DX::Mtrans(t->CutDetail.back().pos_p) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(t->CutDetail.back().Yrad2_p));
			}
			else if (func.find("SetModelPhysicsSpeed") != std::string::npos) {
				auto* t = ModelParts->Get(args[0], std::stoi(args[1]));
				t->CutDetail.back().PhysicsSpeed_ = std::stof(args[2]);
			}
			else if (func.find("SetModelOpacityRate") != std::string::npos) {
				auto* t = ModelParts->Get(args[0], std::stoi(args[1]));
				t->CutDetail.back().OpacityRate = std::stof(args[2]);
			}
			else if (func.find("SetModelMotionRate") != std::string::npos) {
				auto* t = ModelParts->Get(args[0], std::stoi(args[1]));
				t->CutDetail.back().animspeed_Dist = std::stof(args[2]);
				t->CutDetail.back().animspeed_Per = std::stof(args[3]);
			}
			else if (func.find("SetModelOpacityEasing") != std::string::npos) {
				auto* t = ModelParts->Get(args[0], std::stoi(args[1]));
				t->CutDetail.back().OpacityRate_Dist = std::stof(args[2]);
				t->CutDetail.back().OpacityRate_Per = std::stof(args[3]);
			}
			//どの距離で描画するかをセット
			else if (func.find("SetModelMode") != std::string::npos) {
				auto* t = ModelParts->Get(args[0], std::stoi(args[1]));
				if (args[2] == Model_Type[0]) {
					t->isBGModel = true;
				}
				if (args[2] == Model_Type[1]) {
					t->IsNearShadow = false;
				}
				if (args[2] == Model_Type[2]) {
					t->IsFarShadow = true;
				}
				if (args[2] == Model_Type[3]) {
					t->ShadowDrawActive = false;
				}

			}
		}
	};
};
