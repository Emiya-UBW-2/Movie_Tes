#include"Header.hpp"

//
#include "Scene/TitleScene.hpp"
#include "Scene/CustomScene.hpp"



int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	SetDoubleStartValidFlag(TRUE);
	SetEnableXAudioFlag(TRUE);//Xaudio(ロードが長いとロストするので必要に応じて)
	DXLib_ref::Create();
	//使用するボタンの指定
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
	Pad->SetIsUseButton(PADS::LEAN_L, true);//UIのみ
	Pad->SetIsUseButton(PADS::LEAN_R, true);//UIのみ
	Pad->SetIsUseButton(PADS::RELOAD, true);//UIのみ
	Pad->SetIsUseButton(PADS::INTERACT, true);//UIのみ
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
	//追加設定
	std::string Name = "AA" + std::to_string(GetNowHiPerformanceCount());
	SetMainWindowText(Name.c_str());						//タイトル
	//SetUseHalfLambertLighting(TRUE);
	MV1SetLoadModelReMakeNormal(TRUE);
	//シーン
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto CustomScenePtr = std::make_shared<FPS_n2::Sceneclass::CustomScene>();


	//遷移先指定
	Titlescene->SetNextSceneList(0, CustomScenePtr);
	CustomScenePtr->SetNextSceneList(0, Titlescene);

	auto* SceneParts = SceneControl::Instance();
	SceneParts->AddList(Titlescene);
	SceneParts->AddList(CustomScenePtr);
	//最初の読み込み
	if (!DXLib_refParts->MainLogic()) { return 0; }
	return 0;
}
