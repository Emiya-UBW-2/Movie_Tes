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
			OptionParts->Load();							//�ݒ�ǂݍ���
			DXDraw::Create("FPS_n2", Frame_Rate);			//�ėp
#ifdef DEBUG
			DeBuG::Create(Frame_Rate);
#endif // DEBUG
			OptionParts->Set_useVR(DXDraw::Instance()->use_vr);
			//�V�F�[�_�[
			PostPassEffect::Create();
			//�V�[��
			auto LoadScene = std::make_shared<Sceneclass::LOADDATA>();
			auto MAINLOOPscene = std::make_shared<Sceneclass::MAINLOOP>();
			//�J�n����
			MAINLOOPscene->Awake();
			if (!(ProcessMessage() == 0)) { return; }
			//
			effectControl.Init();																						//�G�t�F�N�g
			//�J�ڐ�w��
			LoadScene->Set_Next(MAINLOOPscene, scenes::MAIN_LOOP);
			MAINLOOPscene->Set_Next(LoadScene, scenes::LOAD_SCENE);
			//�J�n���J�ڐ�
			scene.ChangeScene(scenes::LOAD_SCENE, (std::shared_ptr<Sceneclass::TEMPSCENE>&)LoadScene);
			//�J��Ԃ�
			do {
				//
				scene.StartScene();
				//
				while (true) {
					if (scene.Execute()) { break; }
					//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
					if (!scene.isPause()) {
						effectControl.Calc();
					}
					scene.Draw();
					//60FPS��ҋ@
					scene.Vsync();
				}
				//�I������
				scene.NextScene();
			} while (!scene.isEnd());
		}
		~main_c() {
			effectControl.Dispose();
		}
	};
};