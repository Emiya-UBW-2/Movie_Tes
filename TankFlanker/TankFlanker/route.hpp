#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	class main_c {
	private:
		bool ending{ true };							//終了処理フラグ
		scenes sel_scene{ scenes::NONE_SCENE };			//現在のシーン
		std::shared_ptr<Sceneclass::TEMPSCENE> scenes_ptr{ nullptr };
		bool selend{ true };
		bool selpause{ true };
	public:
		main_c(void) noexcept {
			OPTION::Create();
			auto* OptionParts = OPTION::Instance();
			OptionParts->Load();								//設定読み込み
			DXDraw::Create("FPS_n2", Frame_Rate);			//汎用
#ifdef DEBUG
			DeBuG::Create(Frame_Rate);
			auto DebugParts = DeBuG::Instance();					//デバッグ
#endif // DEBUG
			auto* DrawParts = DXDraw::Instance();
			OptionParts->Set_useVR(DrawParts->use_vr);
			//シェーダー
			PostPassEffect::Create();
			auto* PostPassParts = PostPassEffect::Instance();				//ホストパスエフェクト(VR、フルスクリーン共用)

			effectControl.Init();																						//エフェクト
			//シーン
			auto MAINLOOPscene = std::make_shared<Sceneclass::MAINLOOP>();
			//開始処理
			MAINLOOPscene->Awake();
			//遷移先指定
			MAINLOOPscene->Set_Next(MAINLOOPscene, scenes::MAIN_LOOP);
			//開始時遷移先
			sel_scene = scenes::MAIN_LOOP;
			scenes_ptr = MAINLOOPscene;
			//繰り返し
			do {
				//開始
				scenes_ptr->Set();
				selend = true;
				selpause = false;
				auto OLDwaits = GetNowHiPerformanceCount();
				//
				while (ProcessMessage() == 0) {
					clsDx();
					const auto waits = GetNowHiPerformanceCount();
					FPS = GetFPS();
#ifdef DEBUG
					DebugParts->put_way();
#endif // DEBUG
					{
						//更新
						{
							selpause = false;
							selend = scenes_ptr->Update();
						}
						//VR空間に適用
						DrawParts->Move_Player();
#ifdef DEBUG
						DebugParts->end_way();
#endif // DEBUG
						//描画
						{
							//エフェクシアのアプデを60FPS相当に変更
							if (!selpause) {
								effectControl.Calc();
							}
							//共通の描画前用意
							scenes_ptr->ReadyDraw();
							//UI書き込み
							PostPassParts->Set_UI_Draw([&] { scenes_ptr->UI_Draw(); });
							//VRに移す
							DrawParts->Draw_VR([&] {
								auto tmp = GetDrawScreen();
								cam_info tmp_cam = scenes_ptr->Get_Camera();
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//被写体深度描画
									PostPassParts->BUF_Draw([&] { scenes_ptr->BG_Draw(); }, [&] { DrawParts->Draw_by_Shadow([&] { scenes_ptr->Main_Draw(); }); }, tmp_cam, effectControl.Update_effect_f);
									//最終描画
									PostPassParts->Set_MAIN_Draw();
								}
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_, false);
								{
									PostPassParts->MAIN_Draw();											//デフォ描画
									PostPassParts->DrawUI(&scenes_ptr->Get_Camera(), DrawParts->use_vr);	//UI1
									scenes_ptr->Item_Draw();											//UI2
								}
								}, scenes_ptr->Get_Camera());
						}
						//ディスプレイ描画
						GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
						{
#ifdef DEBUG
							DebugParts->end_way();
#endif // DEBUG
							//描画
							if (DrawParts->use_vr) {
								DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_y, GetColor(255, 255, 255), TRUE);
								DrawParts->outScreen[0].DrawRotaGraph(DrawParts->disp_x / 2, DrawParts->disp_y / 2, 0.5f, 0, false);
							}
							else {
								DrawParts->outScreen[0].DrawGraph(0, 0, false);
							}
							//上に書く
							scenes_ptr->LAST_Draw();
							//デバッグ
#ifdef DEBUG
							DebugParts->end_way();
							DebugParts->debug(10, 100, float(GetNowHiPerformanceCount() - waits) / 1000.f);
#endif // DEBUG
						}
					}
					//MAPPTs->DepthScreen.DrawExtendGraph(0, 0, 960, 540, false);
#ifdef DEBUG
					printfDx("AsyncCount :%d\n", GetASyncLoadNum());
					printfDx("Drawcall   :%d\n", GetDrawCallCount());
					printfDx("DrawTime   :%5.2f ms\n", float(GetNowHiPerformanceCount() - OLDwaits) / 1000.f);
					printfDx("Cut        :%d\n", MAINLOOPscene->GetCut());
					OLDwaits = GetNowHiPerformanceCount();
#endif // DEBUG
					//画面の反映
					DrawParts->Screen_Flip();
					while(float(GetNowHiPerformanceCount() - waits) / 1000.f <=16.66f){}
					//終了判定
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						this->ending = false;
						break;
					}
					if (!selend) {
						break;
					}
					//
				}
				//終了処理
				{
					//解放
					scenes_ptr->Dispose();
					//遷移
					{
						sel_scene = scenes_ptr->Next_scene;
						scenes_ptr = scenes_ptr->Next_ptr;
					}
				}
				WaitKey();
			} while (this->ending);
			effectControl.Dispose();
		}
	};
};