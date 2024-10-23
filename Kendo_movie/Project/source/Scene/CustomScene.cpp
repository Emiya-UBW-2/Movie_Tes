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
			auto* DrawParts = DXDraw::Instance();
			Vector3DX LightVec = Vector3DX::vget(1.f, -0.5f, 0.05f); LightVec = LightVec.normalized();
			DrawParts->SetAmbientLight(LightVec, GetColorF(1.0f / 3.f, 0.96f / 3.f, 0.94f / 3.f, 1.0f));
			SetLightDifColor(GetColorF(1.0f, 0.96f, 0.94f, 1.0f));																// デフォルトライトのディフューズカラーを設定する
			DrawParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 20, -20), Vector3DX::vget(0, 20, 0), Vector3DX::up());
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(15), 1.f, 200.f);
#ifdef _USE_EFFEKSEER_
			EffectControl::Init();				//
#endif
			m_BaseTime = GetNowHiPerformanceCount();
		}
		bool			CustomScene::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			Pad->ChangeGuide([&]() {});
			LONGLONG deltatime = (GetNowHiPerformanceCount() - m_BaseTime);
			m_BaseTime = GetNowHiPerformanceCount();
			if (DXDraw::Instance()->IsPause()) { return true; }

			if (GetIsFirstLoop()) {
				//
				m_Counter = 0;
				//
				m_LoadUtil.Start(m_Counter);
#ifdef EditMode
				m_LoadEditUtil.Editer_Init(&m_LoadUtil);
#endif
				//
				SetUseASyncLoadFlag(FALSE);
				BGM.Load("data/base_movie2.wav");
				//BGM.vol(0);

				//プレイ用意
				GameSpeed = 1.0f;
				m_NowTime = (m_Counter != 0) ? m_LoadUtil.GetNowTime(m_Counter - 1) : 0;
				Start.Set(true);
				m_count = 0;
			}

			{
				//時間操作
				{
#ifdef EditMode
					bool isEditActive = m_LoadEditUtil.GetEditIsActive();
					Start.Execute((!isEditActive && CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0) || m_LoadEditUtil.PutModelEditIn());
					SpeedUp.Execute(!isEditActive && CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0);
					SpeedDown.Execute(!isEditActive && CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0);
#else
					Start.Execute(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0);
					SpeedUp.Execute(CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0);
					SpeedDown.Execute(CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0);
#endif
					if (SpeedUp.trigger() || SpeedDown.trigger() || Start.trigger()) {
						if (SpeedUp.trigger()) { GameSpeed += 0.1f; }
						if (SpeedDown.trigger()) { GameSpeed -= 0.1f; }
						if (Start.trigger()) { GameSpeed = Start.on() ? 1.f : 0.f; }
						GameSpeed = std::clamp(GameSpeed, 0.f, 2.f);
						if (m_NowTime >= 0) {
							if (GameSpeed >= 0.1f) {
								SetSoundCurrentTime((LONGLONG)(m_NowTime / 1000), BGM.get());
								SetFrequencySoundMem((int)((float)BGM_Frequency * GameSpeed), BGM.get());
								if (!BGM.CheckPlay()) {
									BGM.Play(DX_PLAYTYPE_BACK, FALSE);
								}
#ifdef EditMode
								m_LoadEditUtil.Editer_UpdateMovie((int)(m_NowTime / 1000));
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
				}
				//待ち
				if (m_NowTime >= 0) {
					if (m_count == 0) {
						m_count = 1;
						BGM_Frequency = GetFrequencySoundMem(BGM.get());
						BGM.Play(DX_PLAYTYPE_BACK, TRUE);
						BGM.SetVol(192);
						SetSoundCurrentTime((LONGLONG)(m_NowTime / 1000), BGM.get());

						SetFrequencySoundMem((int)((float)BGM_Frequency * GameSpeed), BGM.get());

					}
					else {
						if (m_count == 1) {
							m_count = 2;
							DrawParts->Update_Shadow([&] { ShadowDraw_Far_Sub(); }, Vector3DX::zero(), 4.f, false);
						}
					}

					bool isFirstLoop = (m_NowTime==0);
					bool ResetPhysics = false;
					if (m_NowTime > m_LoadUtil.GetNowTime(m_Counter)) {
						ResetPhysics = m_LoadUtil.IsResetPhysics(m_Counter);
						++m_Counter;
						isFirstLoop = true;
					}
					if (m_LoadUtil.IsEnd(m_Counter)) { return false; }


					//追加処理
					if (m_Counter == 0) {
						if (isFirstLoop) {
							auto* SE = SoundPool::Instance();
							SE->Get((int)SoundEnum::Taiko1).Play(0, DX_PLAYTYPE_BACK, TRUE);
							SE->Get((int)SoundEnum::KendoKun).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//カットの処理
#ifdef EditMode
					m_LoadEditUtil.Editer_Update();
					ResetPhysics |= m_LoadEditUtil.PutModelEdit_PhysicsReset();
#endif
					m_LoadUtil.FirstUpdate(m_Counter, isFirstLoop, ResetPhysics);
					//
					auto far_t = DrawParts->SetMainCamera().GetCamFar();
					PostPassEffect::Instance()->Set_DoFNearFar(1.f * Scale3DRate, far_t / 2, 0.5f * Scale3DRate, far_t);
				}
				//経過時間測定
				m_NowTime += (LONGLONG)((float)deltatime * GameSpeed);
			}
			//
#ifdef _USE_EFFEKSEER_
			EffectControl::Execute();
#endif
			return true;
		}
		void			CustomScene::Dispose_Sub(void) noexcept {
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->Dispose();
#ifdef _USE_EFFEKSEER_
			EffectControl::Dispose();
#endif

			m_LoadUtil.Dispose();
			BGM.Dispose();
		}
		//
		void			CustomScene::BG_Draw_Sub(void) const noexcept { m_LoadUtil.BGDraw(); }
		void			CustomScene::ShadowDraw_Far_Sub(void) const noexcept { m_LoadUtil.ShadowFarDraw(); }
		void			CustomScene::ShadowDraw_Sub(void) const noexcept { m_LoadUtil.ShadowDraw(); }
		void			CustomScene::SetShadowDraw_Sub(void) const noexcept { m_LoadUtil.SetShadowDraw(); }
		void			CustomScene::MainDraw_Sub(void) const noexcept { m_LoadUtil.MainDraw(); }
		//
		void			CustomScene::DrawUI_Base_Sub(void) const noexcept {
			m_LoadUtil.UIDraw(m_NowTime);
#ifdef EditMode
			m_LoadEditUtil.Editer_DrawUI(m_Counter, m_NowTime);
#endif
		}
	};
};
