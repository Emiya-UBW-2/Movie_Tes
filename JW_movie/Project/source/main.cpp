#include"Header.hpp"

//
#include "Scene/TitleScene.hpp"
#include "Scene/CustomScene.hpp"
#include "ObjectManager.hpp"


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	DXDraw::Create();								//�ėp
	SetMainWindowText("JW");						//�^�C�g��
	//MV1SetLoadModelUsePackDraw(TRUE);
	PostPassEffect::Create();						//�V�F�[�_�[
	FPS_n2::Sceneclass::ObjectManager::Create();
#ifdef DEBUG
	auto* DebugParts = DebugClass::Instance();		//�f�o�b�O
#endif // DEBUG
	auto* DrawParts = DXDraw::Instance();
	//�V�[��
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto Customscene = std::make_shared<FPS_n2::Sceneclass::CustomScene>();
	//�V�[���R���g���[��
	auto scene = std::make_unique<SceneControl>(Titlescene);
	//�J�ڐ�w��
	Titlescene->Set_Next(Customscene);
	Customscene->Set_Next(Titlescene);
	//�J��Ԃ�
	while (true) {
		scene->StartScene();
		while (true) {
			if ((ProcessMessage() != 0) || (CheckHitKeyWithCheck(KEY_INPUT_ESCAPE) != 0)) { return 0; }
			FPS = GetFPS();
#ifdef DEBUG
			clsDx();
			DebugParts->SetStartPoint();
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("Execute start");
#endif // DEBUG
			if (scene->Execute()) { break; }		//�X�V
			scene->Draw();							//�`��
			//�f�o�b�O
#ifdef DEBUG
			DebugParts->DebugWindow(1920 - 300, 50);
			//TestDrawShadowMap(DrawParts->m_Shadow[0].GetHandle(), 0, 0, 960, 540);
#else
			{
				auto* Fonts = FontPool::Instance();
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, y_r(1920 - 8), y_r(8), GetColor(255, 255, 255), GetColor(0, 0, 0), "%5.2f FPS", FPS);
			}
#endif // DEBUG
			DrawParts->Screen_Flip();				//��ʂ̔��f
		}
		bool isTitle = (scene->GetNowScene() == Titlescene);
		if (isTitle) {
			Titlescene->Set_Next(Customscene);
		}
		scene->NextScene();							//���̃V�[���ֈڍs
	}
	return 0;
}
