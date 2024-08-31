#include"Header.hpp"

//
#include "Scene/TitleScene.hpp"
#include "Scene/CustomScene.hpp"



int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	SetDoubleStartValidFlag(TRUE);
	SetEnableXAudioFlag(TRUE);//Xaudio(���[�h�������ƃ��X�g����̂ŕK�v�ɉ�����)
	DXLib_ref::Create();
	//�g�p����{�^���̎w��
	auto* Pad = PadControl::Instance();
	Pad->SetIsUseButton(PADS::MOVE_W, true);
	Pad->SetIsUseButton(PADS::MOVE_A, true);
	Pad->SetIsUseButton(PADS::MOVE_S, true);
	Pad->SetIsUseButton(PADS::MOVE_D, true);
	Pad->SetIsUseButton(PADS::MOVE_STICK, true);
	Pad->SetIsUseButton(PADS::DIR_UP, false);
	Pad->SetIsUseButton(PADS::DIR_DOWN, false);
	Pad->SetIsUseButton(PADS::DIR_LEFT, false);
	Pad->SetIsUseButton(PADS::DIR_RIGHT, false);
	Pad->SetIsUseButton(PADS::DIR_STICK, true);
	Pad->SetIsUseButton(PADS::LEAN_L, true);//UI�̂�
	Pad->SetIsUseButton(PADS::LEAN_R, true);//UI�̂�
	Pad->SetIsUseButton(PADS::RELOAD, true);//UI�̂�
	Pad->SetIsUseButton(PADS::INTERACT, true);//UI�̂�
	Pad->SetIsUseButton(PADS::THROW, false);
	Pad->SetIsUseButton(PADS::MELEE, false);
	Pad->SetIsUseButton(PADS::JUMP, true);
	Pad->SetIsUseButton(PADS::INVENTORY, false);
	Pad->SetIsUseButton(PADS::RUN, false);
	Pad->SetIsUseButton(PADS::WALK, true);
	Pad->SetIsUseButton(PADS::SHOT, true);
	Pad->SetIsUseButton(PADS::AIM, true);
	Pad->SetIsUseButton(PADS::ULT, true);
	Pad->SetIsUseButton(PADS::SQUAT, false);
	Pad->SetIsUseButton(PADS::PRONE, false);
	Pad->SetIsUseButton(PADS::CHECK, false);
	//
	auto* DXLib_refParts = DXLib_ref::Instance();
	if (!DXLib_refParts->StartLogic()) { return 0; }
	//�ǉ��ݒ�
	std::string Name = "AA" + std::to_string(GetNowHiPerformanceCount());
	SetMainWindowText(Name.c_str());						//�^�C�g��
	//SetUseHalfLambertLighting(TRUE);
	MV1SetLoadModelReMakeNormal(TRUE);
	//�V�[��
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto CustomScenePtr = std::make_shared<FPS_n2::Sceneclass::CustomScene>();


	//�J�ڐ�w��
	Titlescene->SetNextSceneList(0, CustomScenePtr);
	CustomScenePtr->SetNextSceneList(0, Titlescene);

	auto* SceneParts = SceneControl::Instance();
	SceneParts->AddList(Titlescene);
	SceneParts->AddList(CustomScenePtr);
	//�ŏ��̓ǂݍ���
	if (!DXLib_refParts->MainLogic()) { return 0; }
	return 0;
}
