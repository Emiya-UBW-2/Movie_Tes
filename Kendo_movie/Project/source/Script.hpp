#pragma once
#include	"Header.hpp"

#include "CutIn.hpp"
#include "Editer.hpp"
#include "sub.hpp"
//
namespace FPS_n2 {
	static const char* Model_Type[4] = { "SKY_TRUE","NEAR_FALSE","FAR_TRUE","SHADOW_DISACTIVE" };

	class LoadScriptClass {
	private:
		struct VARIABLE {
			std::string Base;
			std::string After;
		};
		struct Variable {
			std::vector<VARIABLE> m_Var;
		public:
			const auto* GetArgFromPath(std::string_view Path) const noexcept {
				const VARIABLE* sel = nullptr;
				for (const auto& a : m_Var) {
					if (a.After == Path) {
						sel = &a;
						break;
					}
				}
				return sel;
			}
			void ChangeStr(std::string* pBase) const noexcept {
				for (auto& a2 : m_Var) {
					if (*pBase == a2.Base) {
						*pBase = a2.After;
						return;
					}
				}
			}
		public:
			void Add(std::string Base, std::string After) noexcept {
				m_Var.resize(m_Var.size() + 1);
				m_Var.back().Base = Base;
				m_Var.back().After = After;
			}
		};
	private:
		Variable					m_Variable;
		std::string					m_Func;
		std::vector<std::string>	m_Args;
	private:
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
	private:
		void			SetDrawModel(int NowCut) noexcept {
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			if (m_Func.find("LoadModel") != std::string::npos) {
				for (int i = 0, count = std::stoi(m_Args[1]); i < count; i++) {
					ModelParts->Load(m_Args[0]);
				}
			}
			else if (m_Func.find("SetDrawModel") != std::string::npos) {
				size_t in_str = m_Args[1].find("~");
				if (in_str != std::string::npos) {
					int start_t = std::stoi(m_Args[1].substr(0, in_str));
					int end_t = std::stoi(m_Args[1].substr(in_str + 1));
					for (int i = start_t; i <= end_t; i++) {
						ModelParts->Get(m_Args[0], i)->Init(NowCut, std::stoi(m_Args[2]) - 1);
					}
				}
				else {
					ModelParts->Get(m_Args[0], std::stoi(m_Args[1]))->Init(NowCut, std::stoi(m_Args[2]) - 1);
				}
			}
			else if (m_Func.find("SetModelAnime") != std::string::npos) {
				auto* t = ModelParts->Get(m_Args[0], std::stoi(m_Args[1]));
				t->CutDetail.back().animsel = std::stoi(m_Args[2]);
				t->CutDetail.back().isloop = (m_Args[3].find("TRUE") != std::string::npos);
				t->CutDetail.back().animspeed = std::stof(m_Args[4]);
				t->CutDetail.back().startframe = std::stof(m_Args[5]);
			}
			else if (m_Func.find("SetModelMat") != std::string::npos) {
				auto* t = ModelParts->Get(m_Args[0], std::stoi(m_Args[1]));
				t->CutDetail.back().usemat = true;

				t->CutDetail.back().Yrad1_p = std::stof(m_Args[2]);
				t->CutDetail.back().pos_p = Vector3DX::vget(std::stof(m_Args[3]), std::stof(m_Args[4]), std::stof(m_Args[5]));
				t->CutDetail.back().Yrad2_p = std::stof(m_Args[6]);

				t->CutDetail.back().mat_p = Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(t->CutDetail.back().Yrad1_p)) * Matrix4x4DX::Mtrans(t->CutDetail.back().pos_p) * Matrix4x4DX::RotAxis(Vector3DX::up(), deg2rad(t->CutDetail.back().Yrad2_p));
			}
			else if (m_Func.find("SetModelPhysicsSpeed") != std::string::npos) {
				auto* t = ModelParts->Get(m_Args[0], std::stoi(m_Args[1]));
				t->CutDetail.back().PhysicsSpeed_ = std::stof(m_Args[2]);
			}
			else if (m_Func.find("SetModelOpacityRate") != std::string::npos) {
				auto* t = ModelParts->Get(m_Args[0], std::stoi(m_Args[1]));
				t->CutDetail.back().OpacityRate = std::stof(m_Args[2]);
			}
			else if (m_Func.find("SetModelMotionRate") != std::string::npos) {
				auto* t = ModelParts->Get(m_Args[0], std::stoi(m_Args[1]));
				t->CutDetail.back().animspeed_Dist = std::stof(m_Args[2]);
				t->CutDetail.back().animspeed_Per = std::stof(m_Args[3]);
			}
			else if (m_Func.find("SetModelOpacityEasing") != std::string::npos) {
				auto* t = ModelParts->Get(m_Args[0], std::stoi(m_Args[1]));
				t->CutDetail.back().OpacityRate_Dist = std::stof(m_Args[2]);
				t->CutDetail.back().OpacityRate_Per = std::stof(m_Args[3]);
			}
			//どの距離で描画するかをセット
			else if (m_Func.find("SetModelMode") != std::string::npos) {
				auto* t = ModelParts->Get(m_Args[0], std::stoi(m_Args[1]));
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
		}
		void			SetDrawGraph(int NowCut) noexcept {
			auto* GraphParts = FPS_n2::GraphControl::Instance();
			if (m_Func.find("LoadGraph") != std::string::npos) {
				auto* DrawParts = DXDraw::Instance();
				for (int i = 0, count = std::stoi(m_Args[1]); i < count; i++) {
					GraphParts->Load(
						(float)(DrawParts->GetScreenY(std::stoi(m_Args[2]))), (float)(DrawParts->GetScreenY(std::stoi(m_Args[3]))),
						std::stof(m_Args[4]), std::stof(m_Args[5]), std::stof(m_Args[6]),
						m_Args[0]);
				}
			}
			else if (m_Func.find("SetDrawGraph") != std::string::npos) {
				size_t in_str = m_Args[1].find("~");
				if (in_str != std::string::npos) {
					int start_t = std::stoi(m_Args[1].substr(0, in_str));
					int end_t = std::stoi(m_Args[1].substr(in_str + 1));
					for (int i = start_t; i <= end_t; i++) {
						GraphParts->Get(m_Args[0], i)->Init(NowCut, std::stoi(m_Args[2]) - 1);
					}
				}
				else {
					GraphParts->Get(m_Args[0], std::stoi(m_Args[1]))->Init(NowCut, std::stoi(m_Args[2]) - 1);
				}
			}
			else if (m_Func.find("SetGraphBlur") != std::string::npos) {
				auto* t = GraphParts->Get(m_Args[0], std::stoi(m_Args[1]));
				t->CutDetail.back().Blur.Init(NowCut, std::stoi(m_Args[2]) - 1);
			}
			else if (m_Func.find("SetGraphOpacityRate") != std::string::npos) {
				auto* t = GraphParts->Get(m_Args[0], std::stoi(m_Args[1]));
				t->CutDetail.back().OpacityRate_Dist = std::stof(m_Args[2]);
				t->CutDetail.back().OpacityRate_Per = 0.f;
				t->CutDetail.back().OpacityRate = std::stof(m_Args[2]);
			}
			else if (m_Func.find("SetGraphOpacityEasing") != std::string::npos) {
				auto* t = GraphParts->Get(m_Args[0], std::stoi(m_Args[1]));
				t->CutDetail.back().OpacityRate_Dist = std::stof(m_Args[2]);
				t->CutDetail.back().OpacityRate_Per = std::stof(m_Args[3]);
			}
		}
	public:
		//Getter
		const auto& Getfunc(void) const noexcept { return m_Func; }
		const auto& Getargs(void) const noexcept { return m_Args; }
		const auto* GetArgFromPath(std::string_view Path) const noexcept { return m_Variable.GetArgFromPath(Path); }
	public:
		//スクリプト読み込み処理
		bool			LoadOnce(std::string_view func_t, int NowCut) noexcept {
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
			SetDrawModel(NowCut);
			//画像描画
			SetDrawGraph(NowCut);
			return true;
		}
	};
	class LoadUtil {
	public:
		LoadScriptClass					m_LoadScriptClass;
		TelopClass						m_TelopClass;

		std::vector<Cut_Info_First>		m_CutInfo;
		std::vector<Cut_Info_Update>	m_CutInfoUpdate;
		CutInfoClass					m_attached;
		std::vector<Vector3DX>			m_PosCam;

		Vector3DX		m_RandcamupBuf;
		Vector3DX		m_RandcamvecBuf;
		Vector3DX		m_RandcamposBuf;
		float			Black_Buf{ 0.f };
		float			White_Buf{ 1.f };
	public:
		auto			GetNowTime(size_t Counter) const noexcept { return (Counter >= 0 ? m_CutInfo[Counter].GetTimeLimit() : 0); }
		auto			IsEnd(size_t Counter) const noexcept { return (Counter > m_CutInfo.size()); }
		auto			IsResetPhysics(size_t Counter) const noexcept { return m_CutInfo[Counter].GetIsResetPhysics(); }
	public:
		void			Load(void) noexcept {
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			//
			m_TelopClass.Init();
			//
			int mdata = FileRead_open("data/Cut.txt", FALSE);
			SetUseASyncLoadFlag(TRUE);
			int NowCut = 0;
			//
			while (FileRead_eof(mdata) == 0) {
				if (ProcessMessage() != 0) {}
				if (!m_LoadScriptClass.LoadOnce(getparams::Getstr(mdata), NowCut)) { continue; }
				const auto& func = m_LoadScriptClass.Getfunc();
				const auto& args = m_LoadScriptClass.Getargs();
				//新規カット
				if (func.find("SetCut") != std::string::npos) {
					m_CutInfo.emplace_back(Cut_Info_First((LONGLONG)(1000000.f * std::stof(args[0]))));
					m_CutInfoUpdate.resize(m_CutInfoUpdate.size() + 1);
					NowCut = (int)(m_CutInfo.size()) - 1;
				}
				//Camposの指定
				else if (func.find("SetCampos_Attach") != std::string::npos) {
					m_attached.Switch.emplace_back(CutInfoClass::On_Off(NowCut, NowCut + (std::stoi(args[0]) - 1)));
					m_PosCam.emplace_back(Vector3DX::vget(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])));
				}
				else {
					//テロップ
					m_TelopClass.LoadTelop(func, args);
				}
				if (m_CutInfo.size() > 0) {
					m_CutInfo.back().LoadScript(func, args);
					m_CutInfoUpdate.back().SetupCam(m_CutInfo.back().Aim_camera);
					m_CutInfoUpdate.back().LoadScript(func, args);
				}
			}
			FileRead_close(mdata);
			//
			SetUseASyncLoadFlag(FALSE);
			while (ProcessMessage() == 0 && GetASyncLoadNum() != 0) {}
			ModelParts->SetAfterLoad();
		}
		void			Start(size_t StartCut) noexcept {
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			auto* GraphParts = FPS_n2::GraphControl::Instance();
			ModelParts->Start(StartCut);
			GraphParts->Start(StartCut);
			m_attached.Start(StartCut);
			m_RandcamupBuf = Vector3DX::zero();
			m_RandcamvecBuf = Vector3DX::zero();
			m_RandcamposBuf = Vector3DX::zero();
		}
		void			FirstUpdate(size_t Counter, bool isFirstLoop, bool reset_p) noexcept {
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			auto* GraphParts = FPS_n2::GraphControl::Instance();
			m_attached.Update_(Counter);
			ModelParts->FirstUpdate(Counter, isFirstLoop, reset_p);
			GraphParts->FirstUpdate(Counter, isFirstLoop);
			if (isFirstLoop) {
				SetupByPrev(Counter);
				SetUpFog(Counter);
				ResetRandom(Counter);
			}
			else {
				Update(Counter);
			}
			UpdateCam(Counter);
		}
		void			Dispose(void) noexcept {
			m_CutInfo.clear();
			m_CutInfoUpdate.clear();
#ifdef EditMode
			m_LoadEditUtil.Editer_Dispose();
#endif
		}

		void			BGDraw(void) const noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(0, 0, 0), TRUE);
			ModelParts->Draw_Far();
		}
		void			ShadowFarDraw(void) const noexcept {
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			SetDrawAlphaTest(DX_CMP_GREATER, 128);
			//ModelParts->Draw(false, true, true);
			SetDrawAlphaTest(-1, 0);
		}
		void			ShadowDraw(void) const noexcept {
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			ModelParts->Draw(false, false, true, TRUE);
			ModelParts->Draw(true, false, true, FALSE);
		}
		void			SetShadowDraw(void) const noexcept {
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			ModelParts->Draw(false, false, true, TRUE);
			ModelParts->Draw(true, false, true, FALSE);
		}
		void			MainDraw(void) const noexcept {
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			ModelParts->CheckInCamera();
			auto* DrawParts = DXDraw::Instance();
			auto camfar = GetCameraFar();
			if (DrawParts->GetMainCamera().GetCamFar() - 1.f < camfar && camfar < DrawParts->GetMainCamera().GetCamFar() + 1.f) {
				ModelParts->Draw(false, false, false, FALSE);
			}
			else {
				ModelParts->Draw(false, false, false, TRUE);
			}
		}
		void			UIDraw(LONGLONG NowTime) const noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* GraphParts = FPS_n2::GraphControl::Instance();

			if (NowTime > 0) {
				m_TelopClass.Draw(NowTime);
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
			GraphParts->Draw();
			SetDrawMode(DX_DRAWMODE_NEAREST);

		}
	private:
		void			SetupByPrev(size_t Counter) noexcept {
			auto& ci = m_CutInfo[Counter];
			auto& u = m_CutInfoUpdate[Counter];
			if (Counter > 0) {
				ci.SetPrev(m_CutInfo[Counter - 1]);
			}
			u.SetupByPrev(m_CutInfoUpdate[Counter - 1]);
		}
		void			SetUpFog(size_t Counter) noexcept {
			auto& ci = m_CutInfo[Counter];
			ci.SetUpFog();
		}
		void			ResetRandom(size_t Counter) noexcept {
			auto& ci = m_CutInfo[Counter];
			auto& u = m_CutInfoUpdate[Counter];
			Vector3DX vec;
			if (ci.GetForcusCenter(&vec)) {
				ci.Aim_camera.SetCamPos(ci.Aim_camera.GetCamPos(), vec, ci.Aim_camera.GetCamUp());
			}
			if (m_attached.GetSwitch()) {
				ci.Aim_camera.SetCamPos(ci.Aim_camera.GetCamVec() + m_PosCam[m_attached.nowcut], ci.Aim_camera.GetCamVec(), ci.Aim_camera.GetCamUp());
			}
			if (ci.isResetRandCampos) { m_RandcamposBuf = Vector3DX::zero(); }
			if (ci.isResetRandCamvec) { m_RandcamvecBuf = Vector3DX::zero(); }
			if (ci.isResetRandCamup) { m_RandcamupBuf = Vector3DX::zero(); }
			u.ResetCam(ci.Aim_camera);
		}
		void			Update(size_t Counter) noexcept {
			auto& ci = m_CutInfo[Counter];
			auto& u = m_CutInfoUpdate[Counter];
			u.Update(ci, &m_RandcamupBuf, &m_RandcamvecBuf, &m_RandcamposBuf, &Black_Buf, &White_Buf);
			//
			if (m_attached.GetSwitch()) {
				ci.Aim_camera.SetCamPos(ci.Aim_camera.GetCamVec() + m_PosCam[m_attached.nowcut], ci.Aim_camera.GetCamVec(), ci.Aim_camera.GetCamUp());
			}
		}
		void			UpdateCam(size_t Counter) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto& ci = m_CutInfo[Counter];
			ci.UpdateCam(&DrawParts->SetMainCamera());
		}
	};
};
