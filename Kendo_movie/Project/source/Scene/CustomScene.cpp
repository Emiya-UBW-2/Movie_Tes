#include	"CustomScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			CustomScene::Load_Sub(void) noexcept {
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->Load();
			BattleResourceMngr->Set();
			m_LoadUtil.Load();
		}
		void			CustomScene::Set_Sub(void) noexcept {
			clsDx();
			auto* DrawParts = DXDraw::Instance();
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			auto* GraphParts = FPS_n2::GraphControl::Instance();
			//
			DrawParts->SetAmbientLight(Vector3DX::vget(-0.2f, -0.8f, -0.4f), GetColorF(1.f, 1.f, 1.f, 0.0f));
			{
				//
				DrawParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 20, -20), Vector3DX::vget(0, 20, 0), Vector3DX::up());
				DrawParts->SetMainCamera().SetCamInfo(deg2rad(15), 1.f, 200.f);
				//
				m_Counter = 0;
				//
				ModelParts->Start(m_Counter);
				GraphParts->Start(m_Counter);
				m_LoadUtil.Start(m_Counter);
#ifdef EditMode
				m_LoadEditUtil.Editer_Init(&m_LoadUtil);
#endif
				//
				SetUseASyncLoadFlag(FALSE);
				BGM.Load("data/base_movie2.wav");
				//BGM.vol(0);

				//プレイ用意
				GameSpeed = (float)(spd_x) / 10.f;
				m_BaseTime = GetNowHiPerformanceCount() - m_LoadUtil.GetNowTime(m_Counter - 1);
				m_NowTimeWait = (m_Counter != 0) ? 0 : -1000000;
				ResetPhysics = true;
				Start.Set(true);
			}
#ifdef _USE_EFFEKSEER_
			EffectControl::Init();				//
#endif
		}
		bool			CustomScene::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			auto* GraphParts = FPS_n2::GraphControl::Instance();
			Pad->ChangeGuide([&]() {});
			if (DXDraw::Instance()->IsPause()) {
				return true;
			}

			ObjMngr->ExecuteObject();
			ObjMngr->LateExecuteObject();
			{
				if (m_LoadUtil.IsEnd(m_Counter)) { return false; }
				SpeedUp.Execute(CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0);
				SpeedDown.Execute(CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0);
				bool isEditActive = true;
#ifdef EditMode
				isEditActive = m_LoadEditUtil.GetEditIsActive();
				Start.Execute((!isEditActive && CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0) || m_LoadEditUtil.PutModelEditIn());
#else
				Start.Execute(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0);
#endif
				if ((!isEditActive && (SpeedUp.trigger() || SpeedDown.trigger())) || Start.trigger()) {
					if (SpeedUp.trigger()) { spd_x++; }
					if (SpeedDown.trigger()) { spd_x--; }
					if (Start.trigger()) { spd_x = Start.on() ? 10 : 0; }
					spd_x = std::clamp(spd_x, 0, 20);
					GameSpeed = (float)(spd_x) / 10.f;
					if (m_NowTimeWait >= 0) {
						if (GameSpeed >= 0.1f) {
							SetSoundCurrentTime((LONGLONG)(m_NowTimeWait / 1000), BGM.get());
							SetFrequencySoundMem((int)((float)BGM_Frequency * GameSpeed), BGM.get());
							if (!BGM.CheckPlay()) {
								BGM.Play(DX_PLAYTYPE_BACK, FALSE);
							}
#ifdef EditMode
							m_LoadEditUtil.Editer_UpdateMovie((int)(m_NowTimeWait / 1000));
#endif
						}
						else {
							BGM.Stop();
#ifdef EditMode
							m_LoadEditUtil.Editer_StopMovie();
#endif
						}
					}
				}
				m_NowTimeWait += (GetNowHiPerformanceCount() - m_BaseTime);
				m_BaseTime = GetNowHiPerformanceCount();
				//待ち
				if (m_NowTimeWait < 0) {
					m_count = 0;
					return true;
				}
				else {
					if (m_count == 0) {
						m_count = 1;
						BGM_Frequency = GetFrequencySoundMem(BGM.get());
						BGM.Play(DX_PLAYTYPE_BACK, TRUE);
						BGM.SetVol(192);
						SetSoundCurrentTime((LONGLONG)(m_NowTimeWait / 1000), BGM.get());

						SetFrequencySoundMem((int)((float)BGM_Frequency * GameSpeed), BGM.get());

					}
					else {
						if (m_count == 1) {
							m_count = 2;
							DrawParts->Update_Shadow([&] { ShadowDraw_Far_Sub(); }, Vector3DX::zero(), true, false);
						}
					}
				}
				//カットの処理
				m_LoadUtil.FirstUpdate(m_Counter);
				ModelParts->FirstUpdate(m_Counter, isFirstLoop);
				GraphParts->FirstUpdate(m_Counter, isFirstLoop);
#ifdef EditMode
				m_LoadEditUtil.Editer_Update();
#endif
				//追加処理
				if (m_Counter == 0) {
					if (isFirstLoop) {
						auto* SE = SoundPool::Instance();
						SE->Get((int)SoundEnum::Taiko1).Play(0, DX_PLAYTYPE_BACK, TRUE);
						SE->Get((int)SoundEnum::KendoKun).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
				}
				//
				if (isFirstLoop) {
					FogParam		Fog{};
					Fog.Reset();
					m_LoadUtil.Flip(m_Counter, &Fog);
					SetFogEnable(TRUE);
					Fog.SetFog();
				}
				else {
					m_LoadUtil.Update(m_Counter, &Black_Buf, &White_Buf);
				}
				//
				auto far_t = DrawParts->SetMainCamera().GetCamFar();
				PostPassEffect::Instance()->Set_DoFNearFar(1.f * Scale_Rate, far_t / 2, 0.5f * Scale_Rate, far_t);
				//
				bool IsPhysicsResetOverride = false;
#ifdef EditMode
				IsPhysicsResetOverride = m_LoadEditUtil.PutModelEdit_PhysicsReset();
#endif
				GraphParts->Update();
				ModelParts->Update();
				ModelParts->SetPhysics(ResetPhysics || IsPhysicsResetOverride);
				isFirstLoop = (m_NowTimeWait > m_LoadUtil.GetNowTime(m_Counter));
				if (isFirstLoop) {
					ResetPhysics = m_LoadUtil.IsResetPhysics(m_Counter);
					++m_Counter;
				}
				else {
					ResetPhysics = false;
				}
			}
			//
			EffectControl::Execute();
			return true;
		}
		void			CustomScene::Dispose_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* BattleResourceMngr = CommonBattleResource::Instance();

			BattleResourceMngr->Dispose();
			ObjMngr->DeleteAll();
			EffectControl::Dispose();
			m_LoadUtil.Dispose();
			BGM.Dispose();
		}
		//
		void			CustomScene::BG_Draw_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(0, 0, 0), TRUE);
			ModelParts->Draw_Far();
		}
		void			CustomScene::ShadowDraw_Far_Sub(void) noexcept {
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			SetDrawAlphaTest(DX_CMP_GREATER, 128);
			ModelParts->Draw(false, true, true);
			SetDrawAlphaTest(-1, 0);
		}
		void			CustomScene::ShadowDraw_Sub(void) noexcept {
			//this->m_BackGround->Shadow_Draw();
			auto* ObjMngr = ObjectManager::Instance();
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			ObjMngr->Draw_Shadow();
			ModelParts->Draw(false, false, true, TRUE);
			ModelParts->Draw(true, false, true, FALSE);
		}
		void			CustomScene::MainDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* ModelParts = FPS_n2::ModelControl::Instance();
			auto* GraphParts = FPS_n2::GraphControl::Instance();
			ObjMngr->Draw();
			ModelParts->CheckInCamera();
			//
			auto* DrawParts = DXDraw::Instance();
			auto camfar = GetCameraFar();
			if (DrawParts->GetMainCamera().GetCamFar() - 1.f < camfar && camfar < DrawParts->GetMainCamera().GetCamFar() + 1.f) {
				ModelParts->Draw(false, false, false, FALSE);
			}
			else {
				ModelParts->Draw(false, false, false, TRUE);
			}

			SetDrawMode(DX_DRAWMODE_BILINEAR);
			GraphParts->Draw();
			SetDrawMode(DX_DRAWMODE_NEAREST);
		}
		//
		void			CustomScene::DrawUI_Base_Sub(void) noexcept {
			if (m_NowTimeWait > 0) {
				TelopClass::Instance()->Draw(m_NowTimeWait);
			}
			if (Black_Buf != 0.f) {
				auto* DrawParts = DXDraw::Instance();
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f * Black_Buf));
				DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(0, 0, 0), TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			if (White_Buf != 0.f) {
				auto* DrawParts = DXDraw::Instance();
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f * White_Buf));
				DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(255, 255, 255), TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
#ifdef EditMode
			m_LoadEditUtil.Editer_DrawUI(m_Counter, m_NowTimeWait);
#endif
		}
	};
};
