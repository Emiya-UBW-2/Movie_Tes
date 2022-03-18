#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	class main_c {
	private:
		Sceneclass::SceneControl scene;
	public:
		main_c(void) noexcept {

			OPTION::Create();
			auto* OptionParts = OPTION::Instance();
			OptionParts->Load();							//設定読み込み
			DXDraw::Create("FPS_n2", Frame_Rate);			//汎用
#ifdef DEBUG
			DeBuG::Create(Frame_Rate);
#endif // DEBUG
			OptionParts->Set_useVR(DXDraw::Instance()->use_vr);
			//シェーダー
			PostPassEffect::Create();
			//シーン
			auto LoadScene = std::make_shared<Sceneclass::LOADDATA>();
			auto MAINLOOPscene = std::make_shared<Sceneclass::MAINLOOP>();
			//開始処理
			MAINLOOPscene->Awake();
			if (!(ProcessMessage() == 0)) { return; }
			//
			effectControl.Init();																						//エフェクト
			//遷移先指定
			LoadScene->Set_Next(MAINLOOPscene, scenes::MAIN_LOOP);
			MAINLOOPscene->Set_Next(LoadScene, scenes::LOAD_SCENE);
			//開始時遷移先
			scene.ChangeScene(scenes::LOAD_SCENE, (std::shared_ptr<Sceneclass::TEMPSCENE>&)LoadScene);
			//繰り返し
			do {
				//
				scene.StartScene();
				//
				while (true) {
					if (scene.Execute()) { break; }
					//エフェクシアのアプデを60FPS相当に変更
					if (!scene.isPause()) {
						effectControl.Calc();
					}
					scene.Draw();
					//60FPSを待機
					scene.Vsync();
				}
				//終了処理
				scene.NextScene();
			} while (!scene.isEnd());
		}
		~main_c() {
			effectControl.Dispose();
		}
	};
};