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
		//�V�F�[�_�[
		shaders::shader_Vertex Screen_vertex;					// ���_�f�[�^
		std::array<shaders, 2> shader2D;
	public:
		main_c(void) noexcept {
			auto OPTPTs = std::make_shared<OPTION>();								//�ݒ�ǂݍ���
			auto DrawPts = std::make_shared<DXDraw>("FPS_n2", OPTPTs, Frame_Rate);		//�ėp
#ifdef DEBUG
			auto DebugPTs = std::make_shared<DeBuG>(Frame_Rate);					//�f�o�b�O
#endif // DEBUG
			OPTPTs->Set_useVR(DrawPts->use_vr);
			//�V�F�[�_�[
			auto HostpassPTs = std::make_shared<HostPassEffect>(OPTPTs, DrawPts->disp_x, DrawPts->disp_y);				//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
			//�V�F�[�_�[
			Screen_vertex.Set(DrawPts);																					// ���_�f�[�^�̏���
			shader2D[0].Init("VS_lens.vso", "PS_lens.pso");																//�����Y
			shader2D[1].Init("ShaderPolygon3DTestVS.vso", "ShaderPolygon3DTestPS.pso");									//�c��
			effectControl.Init();																						//�G�t�F�N�g
			//�V�[��
			auto MAINLOOPscene = std::make_shared<Sceneclass::MAINLOOP>(DrawPts, OPTPTs);
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
				//
				while (ProcessMessage() == 0) {
#ifdef DEBUG
					clsDx();
#endif // DEBUG
					const auto waits = GetNowHiPerformanceCount();
					FPS = GetFPS();
#ifdef DEBUG
					DebugPTs->put_way();
#endif // DEBUG
					{
						//�X�V
						{
							selpause = false;
							selend = scenes_ptr->Update();
						}
						//VR��ԂɓK�p
						DrawPts->Move_Player();
						//�`��
						{
							//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
							if (!selpause) {
								effectControl.Calc();
							}
							//���ʂ̕`��O�p��
							scenes_ptr->ReadyDraw();
							//UI��������
							HostpassPTs->Set_UI_Draw([&] { scenes_ptr->UI_Draw(); });
							//VR�Ɉڂ�
							DrawPts->Draw_VR([&] {
								auto tmp = GetDrawScreen();
								cam_info tmp_cam = scenes_ptr->Get_Camera();
								tmp_cam.campos = GetCameraPosition();
								tmp_cam.camvec = GetCameraTarget();
								{
									//��ʑ̐[�x�`��
									HostpassPTs->BUF_Draw([&] { scenes_ptr->BG_Draw(); }, [&] { DrawPts->Draw_by_Shadow([&] { scenes_ptr->Main_Draw(); }); }, tmp_cam, effectControl.Update_effect_f);
									//�ŏI�`��
									HostpassPTs->Set_MAIN_Draw();
								}
								//*
								GraphHandle::SetDraw_Screen(tmp);
								{
									SetUseTextureToShader(0, HostpassPTs->Get_MAIN_Screen().get());	//�g�p����e�N�X�`�����Z�b�g
									if (scenes_ptr->is_lens()) {
										//�����Y�`��
										shader2D[0].Set_dispsize(DrawPts->disp_x, DrawPts->disp_y);
										shader2D[0].Set_param(float(DrawPts->disp_x) / 2.f, float(DrawPts->disp_y) / 2.f, scenes_ptr->size_lens(), scenes_ptr->zoom_lens());
										HostpassPTs->Get_BUF_Screen().SetDraw_Screen();
										{
											shader2D[0].Draw(Screen_vertex);
										}
										HostpassPTs->Set_MAIN_Draw_nohost();
									}

									if (scenes_ptr->is_bless()) {
										//�c�ݕ`��
										shader2D[1].Set_dispsize(DrawPts->disp_x, DrawPts->disp_y);
										shader2D[1].Set_param(0, 0, scenes_ptr->ratio_bless(), (1.f - cos(scenes_ptr->time_bless())) / 2.f);
										HostpassPTs->Get_BUF_Screen().SetDraw_Screen();
										{
											shader2D[1].Draw(Screen_vertex);
										}
										HostpassPTs->Set_MAIN_Draw_nohost();
									}
									SetUseTextureToShader(0, -1);	//�g�p����e�N�X�`�����Z�b�g
								}
								//*/
								GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_, false);
								{
									HostpassPTs->MAIN_Draw();											//�f�t�H�`��
									HostpassPTs->DrawUI(&scenes_ptr->Get_Camera(), DrawPts->use_vr);	//UI1
									scenes_ptr->Item_Draw();											//UI2
								}
								}, scenes_ptr->Get_Camera());
						}
						//�f�B�X�v���C�`��
						GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
						{
							//�`��
							if (DrawPts->use_vr) {
								DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(255, 255, 255), TRUE);
								DrawPts->outScreen[0].DrawRotaGraph(DrawPts->disp_x / 2, DrawPts->disp_y / 2, 0.5f, 0, false);
							}
							else {
								DrawPts->outScreen[0].DrawGraph(0, 0, false);
							}
							//��ɏ���
							scenes_ptr->LAST_Draw();
							//�f�o�b�O
#ifdef DEBUG
							DebugPTs->end_way();
							DebugPTs->debug(10, 100, float(GetNowHiPerformanceCount() - waits) / 1000.f);
#endif // DEBUG
						}
					}
					//MAPPTs->DepthScreen.DrawExtendGraph(0, 0, 960, 540, false);
#ifdef DEBUG
					printfDx("call  :%d\n", GetDrawCallCount());
					printfDx("Async :%d\n", GetASyncLoadNum());
#endif // DEBUG
					//��ʂ̔��f
					DrawPts->Screen_Flip();
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
			} while (this->ending);
			effectControl.Dispose();
		}
	};
};