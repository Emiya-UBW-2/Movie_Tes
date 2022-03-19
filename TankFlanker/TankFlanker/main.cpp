#include"Header.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	auto scene = std::make_unique<FPS_n2::Sceneclass::SceneControl>();

	OPTION::Create();
	auto* OptionParts = OPTION::Instance();
	OptionParts->Load();							//設定読み込み
	DXDraw::Create("FPS_n2", FPS_n2::Frame_Rate);	//汎用
#ifdef DEBUG
	DeBuG::Create(FPS_n2::Frame_Rate);
#endif // DEBUG
	OptionParts->Set_useVR(DXDraw::Instance()->use_vr);
	//シェーダー
	PostPassEffect::Create();
	//シーン
	auto LoadScene = std::make_shared<FPS_n2::Sceneclass::LOADDATA>();
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	//開始処理
	MAINLOOPscene->Awake();
	if (!(ProcessMessage() == 0)) { return 0; }
	//
	FPS_n2::effectControl.Init();																						//エフェクト
	//遷移先指定
	LoadScene->Set_Next(MAINLOOPscene, FPS_n2::Sceneclass::scenes::MAIN_LOOP);
	MAINLOOPscene->Set_Next(LoadScene, FPS_n2::Sceneclass::scenes::LOAD_SCENE);
	//開始時遷移先
	scene->ChangeScene(FPS_n2::Sceneclass::scenes::LOAD_SCENE, (std::shared_ptr<FPS_n2::Sceneclass::TEMPSCENE>&)LoadScene);
	//繰り返し
	do {
		//
		scene->StartScene();
		//
		while (true) {
			if (scene->Execute()) { break; }
			//エフェクシアのアプデを60FPS相当に変更
			if (!scene->isPause()) {
				FPS_n2::effectControl.Calc();
			}
			scene->Draw();
			//60FPSを待機
			scene->Vsync();
		}
		//終了処理
		scene->NextScene();
	} while (!scene->isEnd());
	FPS_n2::effectControl.Dispose();
	return 0;
}
