#include	"CustomScene.hpp"
#include "../ObjectManager.hpp"

#include "../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			CustomScene::Load_Sub(void) noexcept {
			{
				//
				{
					SetUseASyncLoadFlag(FALSE);
					graphs.Load((float)(y_r(1920 * 1 / 2)), (float)(y_r(1080 * 1 / 2)), 0, 1.f, 0.5f, LOGO1);
					//
					TLClass.Init();
					//
					SetUseASyncLoadFlag(FALSE);
					mdata = FileRead_open("data/Cut.txt", FALSE);
					cnt = 0;
					SetUseASyncLoadFlag(TRUE);
					clsDx();
					NowTime = GetNowHiPerformanceCount();
					TotalTime = GetNowHiPerformanceCount();
				}
				//
				while (FileRead_eof(mdata) == 0) {
					LSClass.LoadScript(getparams::Getstr(mdata));
					const auto& args = LSClass.Getargs();
					const auto& func = LSClass.Getfunc();
					if (func == "") { continue; }
					//変数登録
					LSClass.SetArgs(&NAMES);
					//モデル読み込み
					if (func.find("LoadModel") != std::string::npos) {
						for (int i = 0; i < std::stoi(args[1]); i++) {
							models.Load(args[0]);
						}
					}
					//カット
					//新規カット
					if (func.find("SetCut") != std::string::npos) {
						m_CutInfo.resize(m_CutInfo.size() + 1);
						m_CutInfo.back().SetTimeLimit((LONGLONG)(1000000.f * std::stof(args[0])));
						m_CutInfoUpdate.resize(m_CutInfoUpdate.size() + 1);
					}
					//Camposの指定
					else if (func.find("SetCampos_Attach") != std::string::npos) {
						auto startFrame = (int)(m_CutInfo.size()) - 1;
						attached.Switch.resize(attached.Switch.size() + 1);
						attached.Switch.back().SetSwitch(startFrame, startFrame + (std::stoi(args[0]) - 1));
						attachedDetail.resize(attachedDetail.size() + 1);
						attachedDetail.back().poscam.Set(std::stof(args[1]), std::stof(args[2]), std::stof(args[3]));
					}
					else {
						//カメラ座標周り
						if (m_CutInfo.size() > 0) {
							if (m_CutInfo.back().LoadScript(func, args)) {
								m_CutInfoUpdate.back().CameraNotFirst = m_CutInfo.back().Aim_camera;
							}
						}
						if (m_CutInfoUpdate.size() > 0) {
							m_CutInfoUpdate.back().LoadScript(func, args);
						}
						//画像描画
						if (func.find("SetDrawGraph") != std::string::npos) {
							auto startFrame = (int)(m_CutInfo.size()) - 1;
							size_t in_str = args[1].find("~");
							if (in_str != std::string::npos) {
								int start_t = std::stoi(args[1].substr(0, in_str));
								int end_t = std::stoi(args[1].substr(in_str + 1));
								for (int i = start_t; i <= end_t; i++) {
									graphs.Get(args[0], i)->Init(startFrame, std::stoi(args[2]) - 1);
								}
							}
							else {
								graphs.Get(args[0], std::stoi(args[1]))->Init(startFrame, std::stoi(args[2]) - 1);
							}
						}
						else if (func.find("SetGraphBlur") != std::string::npos) {
							auto startFrame = (int)(m_CutInfo.size()) - 1;
							graphs.Get(args[0], std::stoi(args[1]))->CutDetail.back().Blur.Init(startFrame, std::stoi(args[2]) - 1);
						}
						else if (func.find("SetGraphOpacityRate") != std::string::npos) {
							graphs.Get(args[0], std::stoi(args[1]))->CutDetail.back().OpacityRate_Dist = std::stof(args[2]);
							graphs.Get(args[0], std::stoi(args[1]))->CutDetail.back().OpacityRate_Per = 0.f;
							graphs.Get(args[0], std::stoi(args[1]))->CutDetail.back().OpacityRate = std::stof(args[2]);
						}
						else if (func.find("SetGraphOpacityEasing") != std::string::npos) {
							graphs.Get(args[0], std::stoi(args[1]))->CutDetail.back().OpacityRate_Dist = std::stof(args[2]);
							graphs.Get(args[0], std::stoi(args[1]))->CutDetail.back().OpacityRate_Per = std::stof(args[3]);
						}
						//モデル描画
						else if (func.find("SetDrawModel") != std::string::npos) {
							auto startFrame = (int)(m_CutInfo.size()) - 1;
							size_t in_str = args[1].find("~");
							if (in_str != std::string::npos) {
								int start_t = std::stoi(args[1].substr(0, in_str));
								int end_t = std::stoi(args[1].substr(in_str + 1));
								for (int i = start_t; i <= end_t; i++) {
									models.Get(args[0], i)->Init(startFrame, std::stoi(args[2]) - 1);
								}
							}
							else {
								models.Get(args[0], std::stoi(args[1]))->Init(startFrame, std::stoi(args[2]) - 1);
							}
						}
						else if (func.find("SetModelAnime") != std::string::npos) {
							auto* t = models.Get(args[0], std::stoi(args[1]));
							t->CutDetail.back().animsel = std::stoi(args[2]);
							t->CutDetail.back().isloop = (args[3].find("TRUE") != std::string::npos);
							t->CutDetail.back().animspeed = std::stof(args[4]);
							t->CutDetail.back().startframe = std::stof(args[5]);
						}
						else if (func.find("SetModelMat") != std::string::npos) {
							auto* t = models.Get(args[0], std::stoi(args[1]));
							t->CutDetail.back().usemat = true;

							t->CutDetail.back().Yrad1_p = std::stof(args[2]);
							t->CutDetail.back().pos_p = VECTOR_ref::vget(std::stof(args[3]), std::stof(args[4]), std::stof(args[5]));
							t->CutDetail.back().Yrad2_p = std::stof(args[6]);

							t->CutDetail.back().mat_p = MATRIX_ref::RotY(deg2rad(t->CutDetail.back().Yrad1_p)) * MATRIX_ref::Mtrans(t->CutDetail.back().pos_p) * MATRIX_ref::RotY(deg2rad(t->CutDetail.back().Yrad2_p));
						}
						else if (func.find("SetModelPhysicsSpeed") != std::string::npos) {
							auto* t = models.Get(args[0], std::stoi(args[1]));
							t->CutDetail.back().PhysicsSpeed_ = std::stof(args[2]);
						}
						else if (func.find("SetModelOpacityRate") != std::string::npos) {
							auto* t = models.Get(args[0], std::stoi(args[1]));
							t->CutDetail.back().OpacityRate = std::stof(args[2]);
						}
						else if (func.find("SetModelMotionRate") != std::string::npos) {
							auto* t = models.Get(args[0], std::stoi(args[1]));
							t->CutDetail.back().animspeed_Dist = std::stof(args[2]);
							t->CutDetail.back().animspeed_Per = std::stof(args[3]);
						}
						else if (func.find("SetModelOpacityEasing") != std::string::npos) {
							auto* t = models.Get(args[0], std::stoi(args[1]));
							t->CutDetail.back().OpacityRate_Dist = std::stof(args[2]);
							t->CutDetail.back().OpacityRate_Per = std::stof(args[3]);
						}
						//どの距離で描画するかをセット
						else if (func.find("SetModelMode") != std::string::npos) {
							auto* t = models.Get(args[0], std::stoi(args[1]));
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
					//テロップ
					TLClass.LoadTelop(func, args);
					//END
					PutMsg("ロード%3d完了 : %s\n", ++cnt, func.c_str());
					if (ProcessMessage() != 0) {}
				}
				FileRead_close(mdata);
				//
				SetUseASyncLoadFlag(FALSE);
				PutMsg("非同期読み込みオブジェクトの読み込み待ち…\n");
				{
					int prenum = GetASyncLoadNum(), prenumAll = prenum;
					while (ProcessMessage() == 0 && GetASyncLoadNum() != 0) {
						if (prenum != GetASyncLoadNum()) {
							prenum = GetASyncLoadNum();
							PutMsg("ロード%3d完了(%d/%d)\n", cnt, prenum, prenumAll);
							cnt++;
							continue;
						}
					}
				}
				PutMsg("モデルのセット完了\n");
				//モデルの事前処理(非同期)
				models.Set();
				PutMsg("モデルのセット完了\n");
				//モデルのMV1保存
				for (auto& n : NAMES) {
					if (n.find(".pmx") != std::string::npos) {
						MV1SaveModelToMV1File(models.Get(n, 0)->obj.get(), (n.substr(0, n.find(".pmx")) + ".mv1").c_str(), MV1_SAVETYPE_NORMAL, -1, 1, 1, 1, 0, 0);
					}
				}
				PutMsg("モデルのMV1変換完了\n");
			}

			auto* SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::Movievoice1, 1, "data/Sound/SE/voice/death_04.wav");
			SE->Add((int)SoundEnum::Movievoice2, 1, "data/Sound/SE/voice/voice_male_breathing_01.wav");
			SE->Add((int)SoundEnum::Movievoice3, 1, "data/Sound/SE/voice/death_01.wav");
			SE->Add((int)SoundEnum::Movievoice4, 1, "data/Sound/SE/gun/autoM870/2.wav");
			SE->Add((int)SoundEnum::Movievoice5, 3, "data/Sound/SE/gun/auto1911/2.wav");
			SE->Add((int)SoundEnum::Movievoice6, 1, "data/Sound/SE/breakbone.wav");
			SE->Add((int)SoundEnum::Movievoice7, 1, "data/Sound/SE/kick.wav");
			SE->Add((int)SoundEnum::Movievoice8, 1, "data/Sound/SE/moivievoice1.wav");
			SE->Add((int)SoundEnum::Movievoice9, 1, "data/Sound/SE/gun/auto1911/3.wav");
			SE->Add((int)SoundEnum::Movievoice10, 1, "data/Sound/SE/gun/auto1911/4.wav");
			//

			SE->Add((int)SoundEnum::StandUp, 3, "data/Sound/SE/move/sliding.wav");
			SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
			SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
			SE->Add((int)SoundEnum::Env, 1, "data/Sound/SE/envi.wav", false);




			SE->Get((int)SoundEnum::Movievoice1).SetVol_Local(128);
			SE->Get((int)SoundEnum::Movievoice2).SetVol_Local(32);
			SE->Get((int)SoundEnum::Movievoice3).SetVol_Local(128);
			SE->Get((int)SoundEnum::Movievoice8).SetVol_Local(64);
			SE->Get((int)SoundEnum::StandUp).SetVol_Local(128);
			SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
			SE->Get((int)SoundEnum::StandupFoot).SetVol_Local(128);

			SE->Get((int)SoundEnum::Env).SetVol_Local(128);
		}
		void			CustomScene::Set_Sub(void) noexcept {
			clsDx();
			auto* OptionParts = OPTION::Instance();
			//
			SetAmbientLight(VECTOR_ref::vget(-0.2f, -0.8f, -0.4f), GetColorF(1.f, 1.f, 1.f, 0.0f));
			SetNearShadow(VECTOR_ref::vget(Scale_Rate*-6.f, Scale_Rate*-6.f, Scale_Rate*-6.f), VECTOR_ref::vget(Scale_Rate*6.f, Scale_Rate*0.f, Scale_Rate*6.f));
			SetShadowDir(GetLightVec(), 0);
			SetMiddleShadow(VECTOR_ref::vget(Scale_Rate*-6.f, Scale_Rate*-6.f, Scale_Rate*-6.f), VECTOR_ref::vget(Scale_Rate*6.f, Scale_Rate*0.f, Scale_Rate*6.f));
			auto shadow2 = GetLightVec();
			shadow2.x(-shadow2.x());
			shadow2.z(-shadow2.z());
			SetShadowDir(shadow2, 1);
			//
			DeleteLightHandleAll();
			SetLightEnable(TRUE);
			ChangeLightTypeDir(GetLightVec().get());
			SetLightDifColor(GetColorF(1.f, 1.f, 1.f, 1.f));
			SetLightSpcColor(GetColorF(0.01f, 0.01f, 0.01f, 0.f));
			SetLightAmbColor(GetColorF(0.5f, 0.5f, 0.5f, 1.f));
			//
			OptionParts->Set_Shadow(true);
			{
				auto* PostPassParts = PostPassEffect::Instance();
				//
				camera_buf.SetCamPos(VECTOR_ref::vget(0, 20, -20), VECTOR_ref::vget(0, 20, 0), VECTOR_ref::up());
				camera_buf.SetCamInfo(deg2rad(15), 1.f, 200.f);
				fog[0] = 128;
				fog[1] = 128;
				fog[2] = 128;
				fog_range[0] = 200.f;
				fog_range[1] = 300000.f;
				//
				m_Counter = 10;
				m_Counter = 0;
				models.Start(m_Counter);
				graphs.Start(m_Counter);
				attached.Start(m_Counter);

				Pic3Back = GraphHandle::Load(Pic3BackName);
				PicBack1 = GraphHandle::Load(PicBack1Name);
				GraphFilter(PicBack1.get(), DX_GRAPH_FILTER_GAUSS, 16, 1600);
				PicBack1Per.Set(0.f, 0.f, -5.f);
				PicBack1Per2.Set(0.f, 0.f, -5.f);

				SetTransColor(0, 255, 0);
				Pic_Scene.resize(14);


				auto itr = Pic_Scene.begin();
				/*
								//
								itr->Set(0, 0, 1, 4); itr++;
								itr->Set(0, 1, 4, 4); itr++;
								itr->Set(0, 2, 5, 3); itr++;
								//
								//
								itr->Set(2, 0, 9, 6); itr++;
								itr->Set(2, 1, 10, 6); itr++;
								//
								itr->Set(3, 0, 3, 20); itr++;
								itr->Set(3, 1, 3, 8); itr++;
								itr->Set(3, 2, 5, 8); itr++;
								itr->Set(3, 3, 1, 6); itr++;
								itr->Set(3, 4, 2, 6); itr++;
								//
								itr->Set(4, 0, 9, 6); itr++;
								itr->Set(4, 1, 8, 6); itr++;
								itr->Set(4, 2, 12, 4); itr++;
								//
								itr->Set(5, 0, 3, 3); itr++;
				//*/

				SetUseASyncLoadFlag(FALSE);
				BGM = SoundHandle::Load("data/base_movie2.wav");
				//BGM.vol(0);


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
				Start.Set(true);
				m_CutInfo_Buf = m_CutInfo;
				m_CutInfoUpdate_Buf = m_CutInfoUpdate;
#ifdef EditMode
				Editer_Init();
#endif
			}
			m_LightHandle = CreatePointLightHandle(VECTOR_ref::vget(0.f, 0.f, 0.f).get(),
				8.0f*Scale_Rate,
				0.001f,
				0.012f,
				0.004f);
			SetLightDifColorHandle(m_LightHandle, GetColorF(1.f, 1.f, 0.5f, 1.f));
			SetLightSpcColorHandle(m_LightHandle, GetColorF(1.f, 1.f, 0.5f, 1.f));
			SetLightAmbColorHandle(m_LightHandle, GetColorF(1.f, 1.f, 0.5f, 1.f));
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
				auto* KeyGuide = KeyGuideClass::Instance();
				KeyGuide->Reset();
			},
				[&]() {
				auto* KeyGuide = KeyGuideClass::Instance();
				KeyGuide->Reset();
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
				if (Time_Over()) { return false; }

				auto* PostPassParts = PostPassEffect::Instance();

				auto time = GetMocroSec() - BaseTime;

				MouseClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
				SpeedUp.Execute(CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0);
				SpeedDown.Execute(CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0);
				bool isEditActive = true;
#ifdef EditMode
				isEditActive = m_EditModel.m_IsActive;
				Start.Execute((!isEditActive && CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0) || ModelEditIn);
				ModelEditIn = false;
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
							if (GetMovieStateToGraph(movie.get()) == 0) {
								PlayMovieToGraph(movie.get(), 2, DX_MOVIEPLAYTYPE_BCANCEL);
								SeekMovieToGraph(movie.get(), (int)(NowTimeWait / 1000));
							}
							SetPlaySpeedRateMovieToGraph(movie.get(), (double)GameSpeed);
#endif
						}
						else {
							BGM.stop();
#ifdef EditMode
							if (GetMovieStateToGraph(movie.get()) == 1) {
								PauseMovieToGraph(movie.get());
							}
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
							DrawParts->Update_Shadow([&] { ShadowDraw_Far_Sub(); },
								VECTOR_ref::vget(100.f*Scale_Rate, 10.f*Scale_Rate, 100.f*Scale_Rate),
								VECTOR_ref::vget(-100.f*Scale_Rate, -0.1f*Scale_Rate, -100.f*Scale_Rate),
								2);
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
						auto* SE = SoundPool::Instance();
						float sec = 0.f;
						//
						if (m_Counter == 0 || m_Counter == 1) {
							for (int i = 0; i < 30; i++) {
								sec = 0.5f*i;
								if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
									continue;
								}

								sec = 0.15f + 0.5f*i;
								if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
									continue;
								}

								sec = 0.1f + 0.5f*i;
								if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
									continue;
								}
							}
						}
						if (m_Counter == 1) {
							sec = 3.1f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::StandUp).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 2) {
							sec = 3.6f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 3) {
							sec = 5.5f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 3) {
							sec = 7.1f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::Movievoice4).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
								auto mat = models.Get(Soldier, 1)->GetFrameMat("右ひざ");
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_hitblood, mat.pos(), VECTOR_ref::front(), 12.5f, 2.f);
							}
						}
						if (m_Counter == 3) {
							sec = 5.8f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::Movievoice8).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						//
						if (m_Counter == 4) {
							sec = 7.6f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000/60) {
								SE->Get((int)SoundEnum::Movievoice1).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
							sec = 7.9f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::Movievoice2).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 6) {
							sec = 11.5f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::Movievoice3).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 6) {
							sec = 11.3f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::Movievoice4).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
								auto mat = models.Get(Soldier, 0)->GetFrameMat("上半身");
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_hitblood, mat.pos(), VECTOR_ref::front(), 12.5f, 2.f);
							}
						}
						if (m_Counter == 6) {
							sec = 11.6f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60 + 10) {
								SE->Get((int)SoundEnum::Movievoice6).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 7) {
							for (int i = 0; i < 30; i++) {
								sec = 12.4 + 0.25f*i;
								if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
									continue;
								}
							}
						}
						if (m_Counter == 9) {
							sec = 13.6f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::Movievoice5).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
								auto mat = models.Get(Soldier, 1)->GetFrameMat("上半身");
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_reco, mat.pos(), VECTOR_ref::front(), 12.5f, 2.f);
							}
							sec = 14.0f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::Movievoice5).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
								auto mat = models.Get(Soldier, 1)->GetFrameMat("上半身");
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_reco, mat.pos(), VECTOR_ref::front(), 12.5f, 2.f);
							}
							sec = 14.1f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::Movievoice5).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
								auto mat = models.Get(Soldier, 1)->GetFrameMat("上半身");
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_reco, mat.pos(), VECTOR_ref::front(), 12.5f, 2.f);
							}
							sec = 14.3f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::Movievoice5).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
								auto mat = models.Get(Soldier, 1)->GetFrameMat("上半身");
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_hitblood, mat.pos(), VECTOR_ref::front(), 12.5f, 2.f);
							}
						}
						if (m_Counter == 10) {
							sec = 14.9f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60 + 10) {
								SE->Get((int)SoundEnum::Movievoice7).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 10) {
							sec = 15.1f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60 + 10) {
								SE->Get((int)SoundEnum::Movievoice6).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 10) {
							sec = 14.5f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::Movievoice5).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
							sec = 15.6f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::Movievoice5).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 12) {
							sec = 19.3f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60 + 10) {
								SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
						}
						if (m_Counter == 12) {
							sec = 22.6f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60+10) {
								SE->Get((int)SoundEnum::Movievoice5).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
								auto mat = models.Get(Soldier, 2)->GetFrameMat("上半身");
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_hitblood, mat.pos(), VECTOR_ref::front(), 12.5f, 2.f);
							}
						}
						if (m_Counter == 12) {
							sec = 19.1f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60) {
								SE->Get((int)SoundEnum::Movievoice2).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 50.f);
							}
							sec = 22.2f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60 + 10) {
								SE->Get((int)SoundEnum::Movievoice2).StopAll(0);
							}
						}
						if (m_Counter == 14) {
							sec = 26.4f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60 + 10) {
								SE->Get((int)SoundEnum::Movievoice9).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 150.f);
							}
							sec = 27.2f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 1000 / 60 + 10) {
								SE->Get((int)SoundEnum::Movievoice10).Play_3D(0, VECTOR_ref::vget(0.f, 0.f, 0.f), Scale_Rate * 150.f);
							}
						}

						auto mat = models.Get(Suit, 0)->GetFrameMat("銃口先");
						SetLightEnableHandle(m_LightHandle, FALSE);
						SetLightPositionHandle(m_LightHandle, mat.pos().get());
						if (m_Counter == 9) {
							sec = 13.6f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 100) {
								SetLightEnableHandle(m_LightHandle, TRUE);
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec()*-1.f, 0.5f, 2.f);

							}
							sec = 14.0f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 100) {
								SetLightEnableHandle(m_LightHandle, TRUE);
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec()*-1.f, 0.5f, 2.f);

							}
							sec = 14.1f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 100) {
								SetLightEnableHandle(m_LightHandle, TRUE);
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec()*-1.f, 0.5f, 2.f);

							}
							sec = 14.3f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 100) {
								SetLightEnableHandle(m_LightHandle, TRUE);
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec()*-1.f, 0.5f, 2.f);

							}
						}
						if (m_Counter == 10) {
							sec = 14.5f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 100) {
								SetLightEnableHandle(m_LightHandle, TRUE);
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec()*-1.f, 0.5f, 2.f);

							}
							sec = 15.6f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 100) {
								SetLightEnableHandle(m_LightHandle, TRUE);
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec()*-1.f, 0.5f, 2.f);

							}
						}
						if (m_Counter == 12) {
							sec = 22.6f;
							if ((sec*1000.f) < (NowTimeWait / 1000) && (NowTimeWait / 1000) < (sec*1000.f) + 100) {
								SetLightEnableHandle(m_LightHandle, TRUE);
								EffectControl::SetOnce_Any(EffectResource::Effect::ef_fire2, mat.pos(), mat.zvec()*-1.f, 0.5f, 2.f);

							}
						}
					}
					for (auto& p : Pic_Scene) {
						p.Execute();
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
								auto pos_t = m_CutInfo[m_Counter].Aim_camera.GetCamPos();
								auto vec_t = m_CutInfo[m_Counter].Aim_camera.GetCamVec();
								auto up_t = m_CutInfo[m_Counter].Aim_camera.GetCamUp();
								vec_t = vec / (float)(m_CutInfo[m_Counter].Forcus.size());
								m_CutInfo[m_Counter].Aim_camera.SetCamPos(pos_t, vec_t, up_t);
							}
#else
							for (auto&f : m_CutInfo[m_Counter].Forcus) {
								if (f.GetIsUse()) {
									vec += f.GetForce(models) * f.Per;
									isforcus = true;
								}
							}
							if (isforcus) {
								m_CutInfo[m_Counter].Aim_camera.GetCamVec() = vec;
							}
#endif
							//
							if (attached.GetSwitch() && attached_override) {
								auto pos_t = m_CutInfo[m_Counter].Aim_camera.GetCamPos();
								auto vec_t = m_CutInfo[m_Counter].Aim_camera.GetCamVec();
								auto up_t = m_CutInfo[m_Counter].Aim_camera.GetCamUp();
								pos_t = m_CutInfo[m_Counter].Aim_camera.GetCamVec() + attachedDetail[attached.nowcut].poscam;
								m_CutInfo[m_Counter].Aim_camera.SetCamPos(pos_t, vec_t, up_t);
							}
							if (m_CutInfo[m_Counter].isResetRandCampos) { m_RandcamposBuf.clear(); }
							if (m_CutInfo[m_Counter].isResetRandCamvec) { m_RandcamvecBuf.clear(); }
							if (m_CutInfo[m_Counter].isResetRandCamup) { m_RandcamupBuf.clear(); }

							auto pos_t = u.CameraNotFirst.GetCamPos();
							auto vec_t = u.CameraNotFirst.GetCamVec();
							auto up_t = u.CameraNotFirst.GetCamUp();
							pos_t = m_CutInfo[m_Counter].Aim_camera.GetCamPos();
							vec_t = m_CutInfo[m_Counter].Aim_camera.GetCamVec();
							u.CameraNotFirst.SetCamPos(pos_t, vec_t, up_t);
						}
						else {
							u.Update(m_CutInfo[m_Counter], models, m_RandcamupBuf, m_RandcamvecBuf, m_RandcamposBuf);
							auto pos_t = u.CameraNotFirst.GetCamPos();
							auto vec_t = u.CameraNotFirst.GetCamVec();
							auto up_t = u.CameraNotFirst.GetCamUp();
							pos_t += u.CameraNotFirst_Vec.GetCamPos()*(1.f / FPS * GameSpeed);
							vec_t += u.CameraNotFirst_Vec.GetCamVec()*(1.f / FPS * GameSpeed);
							up_t += u.CameraNotFirst_Vec.GetCamUp()*(1.f / FPS * GameSpeed);
							u.CameraNotFirst.SetCamPos(pos_t, vec_t, up_t);
							//
							if (attached.GetSwitch() && attached_override) {
								auto pos_t2 = m_CutInfo[m_Counter].Aim_camera.GetCamPos();
								auto vec_t2 = m_CutInfo[m_Counter].Aim_camera.GetCamVec();
								auto up_t2 = m_CutInfo[m_Counter].Aim_camera.GetCamUp();
								pos_t2 = m_CutInfo[m_Counter].Aim_camera.GetCamVec() + attachedDetail[attached.nowcut].poscam;
								m_CutInfo[m_Counter].Aim_camera.SetCamPos(pos_t2, vec_t2, up_t2);
							}

							easing_set_SetSpeed(&Black_Buf, u.Black, u.Black_Per);
							easing_set_SetSpeed(&White_Buf, u.White, u.White_Per);
						}
					}
					//
					auto pos_t = camera_buf.GetCamPos();
					auto vec_t = camera_buf.GetCamVec();
					auto up_t = camera_buf.GetCamUp();
					auto fov_t = camera_buf.GetCamFov();
					auto near_t = camera_buf.GetCamNear();
					auto far_t = camera_buf.GetCamFar();

					easing_set_SetSpeed(&pos_t, m_CutInfo[m_Counter].Aim_camera.GetCamPos(), m_CutInfo[m_Counter].cam_per);
					easing_set_SetSpeed(&vec_t, m_CutInfo[m_Counter].Aim_camera.GetCamVec(), m_CutInfo[m_Counter].cam_per);
					easing_set_SetSpeed(&up_t, m_CutInfo[m_Counter].Aim_camera.GetCamUp(), m_CutInfo[m_Counter].cam_per);
					easing_set_SetSpeed(&fov_t, m_CutInfo[m_Counter].Aim_camera.GetCamFov(), m_CutInfo[m_Counter].cam_per);
					easing_set_SetSpeed(&far_t, m_CutInfo[m_Counter].Aim_camera.GetCamFar(), m_CutInfo[m_Counter].cam_per);
					easing_set_SetSpeed(&near_t, m_CutInfo[m_Counter].Aim_camera.GetCamNear(), m_CutInfo[m_Counter].cam_per);
					camera_buf.SetCamPos(pos_t, vec_t, up_t);
					camera_buf.SetCamInfo(fov_t, near_t, far_t);
					if (m_Counter >= 17) {
						float sec = 33.f;
						if ((sec*1000.f) < (NowTimeWait / 1000)) {
							float per = std::clamp((float)(NowTimeWait / 1000) / 1000.f - sec, 0.f, 1.f);
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
						auto pos_t2 = DrawParts->SetMainCamera().GetCamPos();
						auto vec_t2 = DrawParts->SetMainCamera().GetCamVec();
						auto up_t2 = DrawParts->SetMainCamera().GetCamUp();
						//pos_t2 = VECTOR_ref::vget(0, 0, 0) + GetVector(x_rm, y_rm)*r_rm;
						vec_t2 = VECTOR_ref::vget(0, 0, 0);
						DrawParts->SetMainCamera().SetCamPos(pos_t2, vec_t, up_t);

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
					models.SetPhysics(ResetPhysics || ModelEdit_PhysicsReset);
					ModelEdit_PhysicsReset = false;
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
			}

			//
			EffectControl::Execute();

			PostPassEffect::Instance()->SetLevelFilter(0, 216, 1.15f);

			return true;
		}
		void			CustomScene::Dispose_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* OptionParts = OPTION::Instance();

			ObjMngr->DisposeObject();
			OptionParts->Set_Shadow(true);
			EffectControl::Dispose();
			{
				m_CutInfo.clear();
				m_CutInfoUpdate.clear();
				BGM.Dispose();
				//grassmodel.Dispose();
#ifdef EditMode
				Editer_Dispose();
#endif
			}
		}

		void			CustomScene::BG_Draw_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);
			models.Draw_Far();
		}
		void			CustomScene::ShadowDraw_Far_Sub(void) noexcept {
			//特殊
			//+12
			SetDrawAlphaTest(DX_CMP_GREATER, 128);
			models.Draw(false, true, true);
			SetDrawAlphaTest(-1, 0);
		}
		void			CustomScene::ShadowDraw_NearFar_Sub(void) noexcept {
			//todo:共通の影モデルを使用
			models.Draw(false, false, true, TRUE);
			//models.Draw(true, false, true, FALSE);
		}

		void			CustomScene::ShadowDraw_Sub(void) noexcept {
			//this->m_BackGround->Shadow_Draw();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
			//+52
			models.Draw(true, false, true, FALSE);
		}
		void			CustomScene::MainDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject();
			{
				auto* DrawParts = DXDraw::Instance();

				if (!isFreepos) {
					SetFogEnable(TRUE);
					SetFogDensity(0.01f);
					SetFogColor(fog[0], fog[1], fog[2]);
					SetFogStartEnd(fog_range[0], fog_range[1]);
				}

				models.CheckInCamera(DrawParts->GetMainCamera().GetCamFar());
				//+201 = 67x3
				auto camfar = GetCameraFar();
				//カメラのfarが適正範囲の設定の場合
				//near
				if (DrawParts->GetMainCamera().GetCamNear() - 1.f < camfar&& camfar < DrawParts->GetMainCamera().GetCamNear() + 1.f) {
					SetDrawMode(DX_DRAWMODE_NEAREST);
				}
				else if (DrawParts->GetMainCamera().GetCamFar() - 1.f < camfar&& camfar < DrawParts->GetMainCamera().GetCamFar() + 1.f) {
					models.Draw(false, false, false, FALSE);
				}
				//far
				else {
					models.Draw(false, false, false, TRUE);
				}
				if (isFreepos) {
					VECTOR_ref vec = (camera_buf.GetCamVec() - camera_buf.GetCamPos());
					float range = vec.size();
					vec = vec.Norm()*camera_buf.GetCamFar();
					DrawCone3D(
						camera_buf.GetCamPos().get(),
						(camera_buf.GetCamPos() + vec).get(),
						std::tan(camera_buf.GetCamFov()) * range,
						8, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
				}

				SetFogEnable(FALSE);
				if (!isFreepos) {
					if (Black_Buf != 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*Black_Buf));
						DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					if (White_Buf != 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*White_Buf));
						DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(255, 255, 255), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					graphs.Draw(DrawParts->m_DispYSize);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				//grassmodel.Draw(camera_buf);
			}
		}

		void			CustomScene::DrawUI_Base_Sub(void) noexcept {
			{
				auto* DrawParts = DXDraw::Instance();

				if (LookMovie.on()) {
					movie1.DrawExtendGraph(0, 0, DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, FALSE);
					movie2.DrawExtendGraph(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, DrawParts->m_DispXSize, DrawParts->m_DispYSize, FALSE);
				}

				if (!isFreepos && NowTimeWait > 0) {
					TLClass.Draw(NowTimeWait);
				}

				if (!isFreepos) {
					if (Black_Buf != 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*Black_Buf));
						DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			{
#ifdef EditMode
				clsDx();
				//printfDx("Time : %.2f\n\n", (float)(NowTimeWait/1000)/1000.f);
				Editer_Calc();

				Editer_Draw();
#endif
			}
		}
	};
};
