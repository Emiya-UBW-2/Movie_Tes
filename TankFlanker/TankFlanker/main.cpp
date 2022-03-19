#include"Header.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	auto scene = std::make_unique<FPS_n2::Sceneclass::SceneControl>();

	OPTION::Create();
	auto* OptionParts = OPTION::Instance();
	OptionParts->Load();							//�ݒ�ǂݍ���
	DXDraw::Create("FPS_n2", FPS_n2::Frame_Rate);	//�ėp
#ifdef DEBUG
	DeBuG::Create(FPS_n2::Frame_Rate);
#endif // DEBUG
	OptionParts->Set_useVR(DXDraw::Instance()->use_vr);
	//�V�F�[�_�[
	PostPassEffect::Create();
	//�V�[��
	auto LoadScene = std::make_shared<FPS_n2::Sceneclass::LOADDATA>();
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	//�J�n����
	MAINLOOPscene->Awake();
	if (!(ProcessMessage() == 0)) { return 0; }
	//
	FPS_n2::effectControl.Init();																						//�G�t�F�N�g
	//�J�ڐ�w��
	LoadScene->Set_Next(MAINLOOPscene, FPS_n2::Sceneclass::scenes::MAIN_LOOP);
	MAINLOOPscene->Set_Next(LoadScene, FPS_n2::Sceneclass::scenes::LOAD_SCENE);
	//�J�n���J�ڐ�
	scene->ChangeScene(FPS_n2::Sceneclass::scenes::LOAD_SCENE, (std::shared_ptr<FPS_n2::Sceneclass::TEMPSCENE>&)LoadScene);
	//�J��Ԃ�
	do {
		//
		scene->StartScene();
		//
		while (true) {
			if (scene->Execute()) { break; }
			//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
			if (!scene->isPause()) {
				FPS_n2::effectControl.Calc();
			}
			scene->Draw();
			//60FPS��ҋ@
			scene->Vsync();
		}
		//�I������
		scene->NextScene();
	} while (!scene->isEnd());
	FPS_n2::effectControl.Dispose();
	return 0;
}
