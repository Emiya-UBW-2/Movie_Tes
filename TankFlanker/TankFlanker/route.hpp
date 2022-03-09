#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	class main_c {
	private:
		bool ending{ true };							//�I�������t���O
		scenes sel_scene{ scenes::NONE_SCENE };			//���݂̃V�[��
		std::shared_ptr<Sceneclass::TEMPSCENE> scenes_ptr{ nullptr };
		bool selend{ true };
		bool selpause{ true };
	public:
		main_c(void) noexcept {
			OPTION::Create();
			auto* OptionParts = OPTION::Instance();
			OptionParts->Load();								//�ݒ�ǂݍ���
			DXDraw::Create("FPS_n2", Frame_Rate);			//�ėp
#ifdef DEBUG
			DeBuG::Create(Frame_Rate);
			auto DebugParts = DeBuG::Instance();					//�f�o�b�O
#endif // DEBUG
			auto* DrawParts = DXDraw::Instance();
			OptionParts->Set_useVR(DrawParts->use_vr);
			//�V�F�[�_�[
			PostPassEffect::Create();
			auto* PostPassParts = PostPassEffect::Instance();				//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)

			effectControl.Init();																						//�G�t�F�N�g
			//�V�[��
			auto MAINLOOPscene = std::make_shared<Sceneclass::MAINLOOP>();
			//�J�n����
			MAINLOOPscene->Awake();
			//�J�ڐ�w��
			MAINLOOPscene->Set_Next(MAINLOOPscene, scenes::MAIN_LOOP);
			//�J�n���J�ڐ�
			sel_scene = scenes::MAIN_LOOP;
			scenes_ptr = MAINLOOPscene;
			//�J��Ԃ�
			do {
				//�J�n
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
						//�X�V
						{
							selpause = false;
							selend = scenes_ptr->Update();
						}
						//VR��ԂɓK�p
						DrawParts->Move_Player();
#ifdef DEBUG
						DebugParts->end_way();
#endif // DEBUG
						//�`��
						{
							//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
							if (!selpause) {
								effectControl.Calc();
							}
							//���ʂ̕`��O�p��
							scenes_ptr->ReadyDraw();
							//UI��������
							PostPassParts->Set_UI_Draw([&] { scenes_ptr->UI_Draw(); });
							//VR�Ɉڂ�
							DrawParts->Draw_VR([&] {
								auto tmp = GetDrawScreen();
								cam_info tmp_cam = scenes_ptr->Get_Camera();
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//��ʑ̐[�x�`��
									PostPassParts->BUF_Draw([&] { scenes_ptr->BG_Draw(); }, [&] { DrawParts->Draw_by_Shadow([&] { scenes_ptr->Main_Draw(); }); }, tmp_cam, effectControl.Update_effect_f);
									//�ŏI�`��
									PostPassParts->Set_MAIN_Draw();
								}
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_, false);
								{
									PostPassParts->MAIN_Draw();											//�f�t�H�`��
									PostPassParts->DrawUI(&scenes_ptr->Get_Camera(), DrawParts->use_vr);	//UI1
									scenes_ptr->Item_Draw();											//UI2
								}
								}, scenes_ptr->Get_Camera());
						}
						//�f�B�X�v���C�`��
						GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
						{
#ifdef DEBUG
							DebugParts->end_way();
#endif // DEBUG
							//�`��
							if (DrawParts->use_vr) {
								DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_y, GetColor(255, 255, 255), TRUE);
								DrawParts->outScreen[0].DrawRotaGraph(DrawParts->disp_x / 2, DrawParts->disp_y / 2, 0.5f, 0, false);
							}
							else {
								DrawParts->outScreen[0].DrawGraph(0, 0, false);
							}
							//��ɏ���
							scenes_ptr->LAST_Draw();
							//�f�o�b�O
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
					//��ʂ̔��f
					DrawParts->Screen_Flip();
					while(float(GetNowHiPerformanceCount() - waits) / 1000.f <=16.66f){}
					//�I������
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						this->ending = false;
						break;
					}
					if (!selend) {
						break;
					}
					//
				}
				//�I������
				{
					//���
					scenes_ptr->Dispose();
					//�J��
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