#include	"CustomScene.hpp"

#include "../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			CustomScene::Load_Sub(void) noexcept {
			{
				SetUseASyncLoadFlag(FALSE);
				//graphs.Load((float)(DrawParts->GetScreenY(1920 * 1 / 2)), (float)(DrawParts->GetScreenY(1080 * 1 / 2)), 0, 1.f, 0.5f, LOGO1);
				//
				SetUseASyncLoadFlag(FALSE);
				int mdata = FileRead_open("data/Cut.txt", FALSE);
				m_LoadUtil.StartLoad();
				//
				while (FileRead_eof(mdata) == 0) {
					m_LoadUtil.LoadOnce(mdata);
					const auto& func = m_LoadUtil.GetFunc();
					if (func == "") { continue; }
					//変数登録
					m_LoadUtil.CheckArgments();
					//モデル読み込み
					m_LoadUtil.LoadModel();
					//新規カット
					m_LoadUtil.SetNewCut();
					//Camposの指定
					m_LoadUtil.SetCamPos_Attach();
					m_LoadUtil.SetCamPos_None();
					//画像描画
					m_LoadUtil.SetDrawGraph();
					//モデル描画
					m_LoadUtil.SetDrawModel();
					//テロップ
					m_LoadUtil.LoadTelop();
					if (ProcessMessage() != 0) {}
				}
				FileRead_close(mdata);
				//
				SetUseASyncLoadFlag(FALSE);
				m_LoadUtil.WaitAsyncLoad();
				//
				m_LoadUtil.WaitModelSet();
				//
				m_LoadUtil.WaitMV1Save();
			}

			auto* SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::StandUp, 3, "data/Sound/SE/move/sliding.wav");
			SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
			SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
			SE->Add((int)SoundEnum::Env, 1, "data/Sound/SE/envi.wav", false);


			SE->Get((int)SoundEnum::StandUp).SetVol_Local(128);
			SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
			SE->Get((int)SoundEnum::StandupFoot).SetVol_Local(128);

			SE->Get((int)SoundEnum::Env).SetVol_Local(128);
		}
		void			CustomScene::Set_Sub(void) noexcept {
			clsDx();
			auto* DrawParts = DXDraw::Instance();
			//
			DrawParts->SetAmbientLight(Vector3DX::vget(-0.2f, -0.8f, -0.4f), GetColorF(1.f, 1.f, 1.f, 0.0f));
			{
				//
				camera_buf.SetCamPos(Vector3DX::vget(0, 20, -20), Vector3DX::vget(0, 20, 0), Vector3DX::up());
				camera_buf.SetCamInfo(deg2rad(15), 1.f, 200.f);
				fog[0] = 128;
				fog[1] = 128;
				fog[2] = 128;
				fog_range[0] = 200.f;
				fog_range[1] = 300000.f;
				//
				m_Counter = 10;
				m_Counter = 0;
				//
				m_LoadUtil.Start(m_Counter);
#ifdef EditMode
				m_LoadUtil.Editer_Init();
#endif
				//
				SetTransColor(0, 255, 0);
				Pic_Scene.resize(14);


				auto itr = Pic_Scene.begin();
				/*
								//
								itr->Set(0, 0, 1, 4); itr++;
								itr->Set(0, 1, 4, 4); itr++;
								itr->Set(0, 2, 5, 3); itr++;
				//*/

				SetUseASyncLoadFlag(FALSE);
				BGM = SoundHandle::Load("data/base_movie2.wav");
				//BGM.vol(0);


				//プレイ用意
				GameSpeed = (float)(spd_x) / 10.f;
				BaseTime = GetMocroSec() - (m_Counter > 0 ? m_LoadUtil.GetCutInfo()[m_Counter - 1].GetTimeLimit() : 0);
				WaitTime = (m_Counter != 0) ? 0 : 1000000;
				NowTimeWait = -WaitTime;
				m_RandcamupBuf = Vector3DX::zero();
				m_RandcamvecBuf = Vector3DX::zero();
				m_RandcamposBuf = Vector3DX::zero();
				ResetPhysics = true;
				Start.Set(true);
			}
#ifdef _USE_EFFEKSEER_
			EffectControl::Init();				//
#endif
		}
		bool			CustomScene::Update_Sub(void) noexcept {
			if (DXDraw::Instance()->IsPause()) {
				return true;
			}
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* ObjMngr = ObjectManager::Instance();

			Pad->ChangeGuide(
				[&]() {
				//auto* KeyGuide = PadControl::Instance();
				//KeyGuide->AddGuide("MM.jpg", "離して見る");
			}
			);

			if (GetIsFirstLoop()) {
				auto* SE = SoundPool::Instance();
				SE->Get((int)SoundEnum::Env).Play(0, DX_PLAYTYPE_LOOP);
			}

			ObjMngr->ExecuteObject();
			ObjMngr->LateExecuteObject();

			{
				if (m_Counter >= m_LoadUtil.GetCutInfo().size()) { return false; }
				auto& NowCut = m_LoadUtil.GetCutInfo().at(m_Counter);

				//auto* PostPassParts = PostPassEffect::Instance();

				auto time = GetMocroSec() - BaseTime;

				SpeedUp.Execute(CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0);
				SpeedDown.Execute(CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0);
				bool isEditActive = true;
#ifdef EditMode
				isEditActive = m_LoadUtil.GetEditIsActive();
				Start.Execute((!isEditActive && CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0) || m_LoadUtil.PutModelEditIn());
#else
				Start.Execute(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0);
#endif
				if ((!isEditActive && (SpeedUp.trigger() || SpeedDown.trigger())) || Start.trigger()) {
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
#ifdef EditMode
							m_LoadUtil.Editer_UpdateMovie((int)(NowTimeWait / 1000));
#endif
						}
						else {
							BGM.stop();
#ifdef EditMode
							m_LoadUtil.Editer_StopMovie();
#endif

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
						SetSoundCurrentTime((LONGLONG)(NowTimeWait / 1000), BGM.get());

						SetFrequencySoundMem((int)((float)BGM_Frequency * GameSpeed), BGM.get());

					}
					else {
						if (issecond) {
							issecond = false;
							DrawParts->Update_Shadow([&] { ShadowDraw_Far_Sub(); }, Vector3DX::zero(), true, false);
						}
					}
				}
				//カットの処理
				{
					m_LoadUtil.FirstUpdate((int)m_Counter, isFirstLoop);
					//
					{
						int SEL = 0;
						//0
						if (m_Counter == SEL) {
						}
						SEL++;

						//
						auto* SE = SoundPool::Instance();
						float sec = 0.f;
						float NowSec = static_cast<float>(NowTimeWait) / 1000.f;
						//
						if (m_Counter == 0 || m_Counter == 1) {
							for (int i = 0; i < 30; i++) {
								sec = 0.5f*i;
								if ((sec*1000.f) < NowSec && NowSec < (sec*1000.f) + 1000.f / 60.f) {
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, Vector3DX::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
									continue;
								}

								sec = 0.15f + 0.5f*i;
								if ((sec*1000.f) < NowSec && NowSec < (sec*1000.f) + 1000.f / 60.f) {
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, Vector3DX::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
									continue;
								}

								sec = 0.1f + 0.5f*i;
								if ((sec*1000.f) < NowSec && NowSec < (sec*1000.f) + 1000.f / 60.f) {
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, Vector3DX::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
									continue;
								}
							}
						}
						if (m_Counter == 1) {
							sec = 3.1f;
							if ((sec*1000.f) < NowSec && NowSec < (sec*1000.f) + 1000.f / 60.f) {
								SE->Get((int)SoundEnum::StandUp).Play_3D(0, Vector3DX::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 2) {
							sec = 3.6f;
							if ((sec*1000.f) < NowSec && NowSec < (sec*1000.f) + 1000.f / 60.f) {
								SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, Vector3DX::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 3) {
							sec = 5.5f;
							if ((sec*1000.f) < NowSec && NowSec < (sec*1000.f) + 1000.f / 60.f) {
								SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, Vector3DX::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 7) {
							for (int i = 0; i < 30; i++) {
								sec = 12.4f + 0.25f*i;
								if ((sec*1000.f) < NowSec && NowSec < (sec*1000.f) + 1000.f / 60.f) {
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, Vector3DX::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
									continue;
								}
							}
						}
						if (m_Counter == 12) {
							sec = 19.3f;
							if ((sec*1000.f) < NowSec && NowSec < (sec*1000.f) + 1000.f / 60.f + 10.f) {
								SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, Vector3DX::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
					}
					for (auto& p : Pic_Scene) {
						p.Execute();
					}
					//
					if (isFirstLoop) {
						if (m_Counter > 0) {
							NowCut.SetPrev(m_LoadUtil.GetCutInfo()[m_Counter - 1]);
						}
						{
							auto& u = m_LoadUtil.GetCutInfoUpdate()[m_Counter];
							if (u.IsUsePrevBuf) {
								//
								auto White_Set = u.IsSetWhite;
								auto White_Per = u.White_Per;
								auto WhitePrev = u.White;

								auto Black_Set = u.IsSetBlack;
								auto Black_Per = u.Black_Per;
								auto BlackPrev = u.Black;
								//
								u = m_LoadUtil.GetCutInfoUpdate()[m_Counter - 1];
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
						//
						if (NowCut.fog[0] >= 0) {
							fog[0] = NowCut.fog[0];
							fog[1] = NowCut.fog[1];
							fog[2] = NowCut.fog[2];
							fog_range[0] = NowCut.fog_range[0];
							fog_range[1] = NowCut.fog_range[1];
						}
						else if (NowCut.fog[0] == -2) {
							fog[0] = 128;
							fog[1] = 128;
							fog[2] = 128;
							fog_range[0] = 200.f;
							fog_range[1] = 300000.f;
						}
						//
						{
							Vector3DX vec;
							bool isforcus = false;
							for (auto&f : NowCut.Forcus) {
								if (f.GetIsUse()) {
									vec += f.GetForce(m_LoadUtil.Getmodels());
									isforcus = true;
								}
							}
							if (isforcus) {
								m_LoadUtil.ChamgeAimCamvec(m_Counter, vec / (float)(NowCut.Forcus.size()));
							}
						}
						//
						if (m_LoadUtil.Getattached().GetSwitch()) {
							m_LoadUtil.ChamgeAimCampos(m_Counter, NowCut.Aim_camera.GetCamVec() + m_LoadUtil.GetattachedDetail());
						}
						if (NowCut.isResetRandCampos) { m_RandcamposBuf = Vector3DX::zero(); }
						if (NowCut.isResetRandCamvec) { m_RandcamvecBuf = Vector3DX::zero(); }
						if (NowCut.isResetRandCamup) { m_RandcamupBuf = Vector3DX::zero(); }
						m_LoadUtil.ChamgeaNotFirstCampos(m_Counter, NowCut.Aim_camera.GetCamPos());
						m_LoadUtil.ChamgeaNotFirstCamvec(m_Counter, NowCut.Aim_camera.GetCamVec());
					}
					else {
						auto& u = m_LoadUtil.GetCutInfoUpdate()[m_Counter];
						u.Update(NowCut, m_LoadUtil.Getmodels(), m_RandcamupBuf, m_RandcamvecBuf, m_RandcamposBuf);

						auto pos_t = u.CameraNotFirst.GetCamPos() + u.CameraNotFirst_Vec.GetCamPos()*(1.f / DrawParts->GetFps() * GameSpeed);
						auto vec_t = u.CameraNotFirst.GetCamVec() + u.CameraNotFirst_Vec.GetCamVec()*(1.f / DrawParts->GetFps() * GameSpeed);
						auto up_t = u.CameraNotFirst.GetCamUp() + u.CameraNotFirst_Vec.GetCamUp()*(1.f / DrawParts->GetFps() * GameSpeed);
						m_LoadUtil.ChamgeaNotFirstCampos(m_Counter, pos_t);
						m_LoadUtil.ChamgeaNotFirstCamvec(m_Counter, vec_t);
						m_LoadUtil.ChamgeaNotFirstCamup(m_Counter, up_t);
						//
						if (m_LoadUtil.Getattached().GetSwitch()) {
							m_LoadUtil.ChamgeAimCampos(m_Counter, NowCut.Aim_camera.GetCamVec() + m_LoadUtil.GetattachedDetail());
						}

						easing_set_SetSpeed(&Black_Buf, u.Black, u.Black_Per);
						easing_set_SetSpeed(&White_Buf, u.White, u.White_Per);
					}
					//
					m_LoadUtil.UpdateCam(m_Counter, &camera_buf);
					//
					auto far_t = camera_buf.GetCamFar();
					if (m_Counter >= 17) {
						float sec = 33.f;
						float NowSec = static_cast<float>(NowTimeWait) / 1000.f;
						if ((sec*1000.f) < NowSec) {
							float per = std::clamp(NowSec / 1000.f - sec, 0.f, 1.f);
							PostPassEffect::Instance()->Set_DoFNearFar(Lerp(1.f * Scale_Rate,far_t,per), far_t, 0.5f*Scale_Rate, far_t);
						}
						else {
							PostPassEffect::Instance()->Set_DoFNearFar(1.f * Scale_Rate, far_t, 0.5f*Scale_Rate, far_t);
						}
					}
					else {
						PostPassEffect::Instance()->Set_DoFNearFar(1.f * Scale_Rate, far_t / 2, 0.5f*Scale_Rate, far_t);
					}
					//
					isFreepos = (CheckHitKeyWithCheck(KEY_INPUT_RETURN) != 0);
					int x_o = x_m;
					int y_o = y_m;
					if (isFreepos) {
						x_o = x_sav;
						y_o = y_sav;
					}
					GetMousePoint(&x_m, &y_m);
					if (!isFreepos) {
						DrawParts->SetMainCamera() = camera_buf;
						x_sav = -1;
						y_sav = -1;
					}
					else {
						x_rm = std::clamp(x_rm + (float)(y_m - y_o) / 10.f, -20.f, 80.f);
						y_rm += (float)(x_m - x_o) / 10.f;

						r_rm = std::clamp(r_rm + (float)(GetMouseWheelRotVol())*10.f, 5.f, 6000.f);

						DrawParts->SetMainCamera().SetCamInfo(deg2rad(45), 1.f, 1000.f);
						Vector3DX pos_t2 = DrawParts->SetMainCamera().GetCamPos();
						Vector3DX vec_t2 = DrawParts->SetMainCamera().GetCamVec();
						Vector3DX up_t2 = DrawParts->SetMainCamera().GetCamUp();
						//pos_t2 = Vector3DX::vget(0, 0, 0) + GetVector(x_rm, y_rm)*r_rm;
						vec_t2 = Vector3DX::vget(0, 0, 0);
						DrawParts->SetMainCamera().SetCamPos(pos_t2, vec_t2, up_t2);

						if (x_sav == -1) {
							x_sav = x_m;
							y_sav = y_m;
						}
						SetMousePoint(x_sav, y_sav);
					}
				}
				bool IsPhysicsResetOverride = false;
#ifdef EditMode
				IsPhysicsResetOverride = m_LoadUtil.PutModelEdit_PhysicsReset();
#endif
				m_LoadUtil.LateUpdate(ResetPhysics || IsPhysicsResetOverride);
				isFirstLoop = false;
				if (NowTimeWait > NowCut.GetTimeLimit()) {
					isFirstLoop = true;
					if (NowCut.IsResetPhysics) {
						ResetPhysics = true;
					}
					++m_Counter;
				}
				else {
					ResetPhysics = false;
				}
			}

			//
			EffectControl::Execute();

			PostPassEffect::Instance()->SetLevelFilter(0, 216, 1.15f);

			return true;
		}
		void			CustomScene::Dispose_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();

			ObjMngr->DeleteAll();
			EffectControl::Dispose();
			m_LoadUtil.Dispose();
			BGM.Dispose();
		}

		void			CustomScene::BG_Draw_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(0, 0, 0), TRUE);
			m_LoadUtil.BG_Draw();
		}
		void			CustomScene::ShadowDraw_Far_Sub(void) noexcept {
			m_LoadUtil.ShadowDraw_Far();
		}
		void			CustomScene::ShadowDraw_Sub(void) noexcept {
			//this->m_BackGround->Shadow_Draw();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->Draw_Shadow();
			m_LoadUtil.ShadowDraw_NearFar();
			m_LoadUtil.ShadowDraw();
		}
		void			CustomScene::MainDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->Draw();
			if (!isFreepos) {
				SetFogEnable(TRUE);
				SetFogDensity(0.01f);
				SetFogColor(fog[0], fog[1], fog[2]);
				SetFogStartEnd(fog_range[0], fog_range[1]);
			}
			m_LoadUtil.MainDraw();
			SetFogEnable(FALSE);
			if (!isFreepos) {
				m_LoadUtil.MainDrawGraph();
			}
			//
			if (isFreepos) {
				Vector3DX vec = (camera_buf.GetCamVec() - camera_buf.GetCamPos());
				float range = vec.magnitude();
				vec = vec.normalized() *camera_buf.GetCamFar();
				DrawCone3D(
					camera_buf.GetCamPos().get(),
					(camera_buf.GetCamPos() + vec).get(),
					std::tan(camera_buf.GetCamFov()) * range,
					8, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
			}
		}

		void			CustomScene::DrawUI_Base_Sub(void) noexcept {
			if (!isFreepos) {
				if (NowTimeWait > 0) {
					m_LoadUtil.DrawUI(NowTimeWait);
				}
				if (Black_Buf != 0.f) {
					auto* DrawParts = DXDraw::Instance();
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*Black_Buf));
					DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(0, 0, 0), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				if (White_Buf != 0.f) {
					auto* DrawParts = DXDraw::Instance();
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*White_Buf));
					DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(255, 255, 255), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
			}
#ifdef EditMode
			m_LoadUtil.Editer_DrawUI(m_Counter, NowTimeWait);
#endif
		}
	};
};
