#pragma once
#include	"Header.hpp"

#include "CutIn.hpp"
#include "Editer.hpp"
#include "sub.hpp"
//
namespace FPS_n2 {
	class LoadScriptClass : public SingletonBase<LoadScriptClass> {
	private:
		friend class SingletonBase<LoadScriptClass>;
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
		const auto&		Getfunc(void) const noexcept { return m_Func; }
		const auto&		Getargs(void) const noexcept { return m_Args; }
		const auto*		GetArgFromPath(std::string_view Path) const noexcept { return m_Variable.GetArgFromPath(Path); }
	public:
		//読み込み作業前
		void			BeforeLoad() noexcept {
			TelopClass::Instance()->Init();
		}
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
			//テロップ
			TelopClass::Instance()->LoadTelop(m_Func, m_Args);
			//モデル描画
			SetDrawModel(NowCut);
			//画像描画
			SetDrawGraph(NowCut);
			return true;
		}
		//完全な読み込み終了後
		void			AfterLoad() noexcept {
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			ModelParts->SetAfterLoad();
		}
	};
	class LoadUtil {
	public:
		std::vector<Cut_Info_First>		m_CutInfo;
		std::vector<Cut_Info_Update>	m_CutInfoUpdate;
		CutInfoClass					m_attached;
		std::vector<Vector3DX>			m_PosCam;
		LONGLONG						m_PrevLoadTime{ 0 };
		Vector3DX		m_RandcamupBuf;
		Vector3DX		m_RandcamvecBuf;
		Vector3DX		m_RandcamposBuf;
	public:
		auto			GetNowTime(size_t Counter) { return (Counter >= 0 ? m_CutInfo[Counter].GetTimeLimit() : 0); }
		auto			IsEnd(size_t Counter) { return (Counter >= m_CutInfo.size()); }
		auto			IsResetPhysics(size_t Counter) { return m_CutInfo[Counter].IsResetPhysics; }
	public:
		void			Load(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* LSParts = FPS_n2::LoadScriptClass::Instance();
			//
			LSParts->BeforeLoad();
			//
			int mdata = FileRead_open("data/Cut.txt", FALSE);
			SetUseASyncLoadFlag(TRUE);
			clsDx();
			m_PrevLoadTime = GetNowHiPerformanceCount();
			int NowCut = 0;
			//
			while (FileRead_eof(mdata) == 0) {
				LONGLONG tim = (GetNowHiPerformanceCount() - m_PrevLoadTime);
				if (tim >= 1000 * 1000 / FrameRate) {
					m_PrevLoadTime = GetNowHiPerformanceCount();
					GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
				}
				if (!LSParts->LoadOnce(getparams::Getstr(mdata), NowCut)) { continue; }
				const auto& func = LSParts->Getfunc();
				const auto& args = LSParts->Getargs();
				if (ProcessMessage() != 0) {}
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
				if (m_CutInfo.size() > 0) {
					if (m_CutInfo.back().LoadScript(func, args)) {
						m_CutInfoUpdate.back().CameraNotFirst = m_CutInfo.back().Aim_camera;
					}
				}
				if (m_CutInfoUpdate.size() > 0) {
					m_CutInfoUpdate.back().LoadScript(func, args);
				}
				printfDx("ロード : %s\n", func.c_str());
				if (tim >= 1000 * 1000 / FrameRate) {
					DrawParts->Screen_Flip();
				}
			}
			FileRead_close(mdata);
			//
			SetUseASyncLoadFlag(FALSE);
			printfDx("非同期読み込みオブジェクトの読み込み待ち…(%d)\n", GetASyncLoadNum());
			DrawParts->Screen_Flip();
			while (ProcessMessage() == 0 && GetASyncLoadNum() != 0) {}
			printfDx("読み込み完了\n");
			LSParts->AfterLoad();
			printfDx("モデルのMV1変換完了\n");
			DrawParts->Screen_Flip();
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
			easing_set_SetSpeed(&DrawParts->SetMainCamera(), ci.Aim_camera, ci.cam_per);
		}
	};
};
