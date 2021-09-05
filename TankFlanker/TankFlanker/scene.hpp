#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	class Sceneclass {
	public:
		class MAINLOOP;
		//
		class TEMPSCENE : public PTR_COMMON {
		private:
			VECTOR_ref Shadow_minpos;
			VECTOR_ref Shadow_maxpos;
			VECTOR_ref Light_vec;
			COLOR_F Light_color = GetColorF(0, 0, 0, 0);
		protected:
			//�����p��
			std::shared_ptr<OPTION> OPTPTs;
			//�J����
			cam_info camera_main;
			float fov_base = DX_PI_F / 2;
			//
			bool use_lens{ false };
			float lens_zoom = 1.f;
			float lens_size = 1.f;
			//
			bool use_bless{ false };
			float bless_ratio = 0.5f;
			float bless = 0.f;
			//
			virtual void Set_EnvLight(VECTOR_ref Shadow_minpos_t, VECTOR_ref Shadow_maxpos_t, VECTOR_ref Light_vec_t, COLOR_F Light_color_t) noexcept {
				Shadow_minpos = Shadow_minpos_t;
				Shadow_maxpos = Shadow_maxpos_t;
				Light_vec = Light_vec_t;
				Light_color = Light_color_t;
			}
		public:
			scenes Next_scene{ scenes::NONE_SCENE };			//���݂̃V�[��
			std::shared_ptr<Sceneclass::TEMPSCENE> Next_ptr{ nullptr };

			TEMPSCENE(
				std::shared_ptr<DXDraw>& DrawPts_t,
				const std::shared_ptr<OPTION>& OPTPTs_t
			) noexcept {
				PTR_COMMON::Set_Ptr_Common(DrawPts_t);
				OPTPTs = OPTPTs_t;
			}
			void Set_Next(const std::shared_ptr<Sceneclass::TEMPSCENE>& Next_scenes_ptr_t, scenes NEXT) {
				Next_ptr = Next_scenes_ptr_t;
				Next_scene = NEXT;
			}

			const VECTOR_ref& Get_Light_vec(void) const noexcept { return Light_vec; }
			cam_info& Get_Camera(void) noexcept { return camera_main; }

			virtual void Awake(void) noexcept {}
			virtual void Set(void) noexcept {
				fov_base = deg2rad(DrawPts->use_vr ? 120 : OPTPTs->Get_Fov());	//fov
				SetUseMaskScreenFlag(FALSE);//���J�X�^����ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
				if (DrawPts->use_vr) {
					camera_main.set_cam_info(fov_base, 0.001f, 100.f);//1P
				}
				else {
					camera_main.set_cam_info(fov_base, 0.05f, 200.f);//1P
				}

				DrawPts->Set_Light_Shadow(Shadow_maxpos, Shadow_minpos, Light_vec, [&] {Shadow_Draw_Far(); });
				SetGlobalAmbientLight(Light_color);
			}
			virtual bool Update(void) noexcept {
				if (use_bless) {
					bless += deg2rad(float(100 + GetRand(600)) / 100.f * Frame_Rate / GetFPS());
					bless_ratio -= (0.03f / 90.f) * Frame_Rate / GetFPS();
					if (bless_ratio <= 0.f) {
						use_bless = false;
					}
				}
				return true;
			}
			virtual void Dispose(void) noexcept {}

			virtual void ReadyDraw(void) noexcept {
				//���ʒu�w��
				Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
				//�e�p��
				DrawPts->Ready_Shadow(camera_main.campos, [&] { Shadow_Draw(); }, [&] { Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.5f, 2.5f, 2.5f), VECTOR_ref::vget(15.f, 2.5f, 15.f));//MAIN_LOOP��near�͂��� (Get_Mine()->Damage.Get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
			}
			virtual void UI_Draw(void) noexcept {}
			virtual void BG_Draw(void) noexcept {
				DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_x, GetColor(192, 192, 192), TRUE);
			}
			virtual void Shadow_Draw_Far(void) noexcept {}
			virtual void Shadow_Draw_NearFar(void) noexcept {}
			virtual void Shadow_Draw(void) noexcept {}
			virtual void Main_Draw(void) noexcept {}

			virtual const bool& is_lens(void) { return use_lens; }
			virtual const float& zoom_lens(void) { return lens_zoom; }
			virtual const float& size_lens(void) { return lens_size; }
			virtual const bool& is_bless(void) { return use_bless; }
			virtual const float& ratio_bless(void) { return bless_ratio; }
			virtual const float& time_bless(void) { return bless; }

			virtual void Item_Draw(void) noexcept {}
			virtual void LAST_Draw(void) noexcept {}
			virtual void KeyOperation_VR(void) noexcept {}
			virtual void KeyOperation(void) noexcept {}
		};
		//
		class MAINLOOP : public TEMPSCENE {
		private:
			class Cut_Info {
			public:
				GraphHandle handle;
				LONGLONG TIME = 0;
			};
			//�f�[�^
			SoundHandle BGM;
			size_t Cut = 0;
			std::vector<Cut_Info> Cut_Pic;
			LONGLONG BaseTime = 0;

			GraphHandle movie;
		private:
			bool Time_Over() {
				return Cut >= Cut_Pic.size();
			}
			auto GetCutTime() {
				return Cut_Pic[Cut%Cut_Pic.size()].TIME;
			}
			void AddCutNum() {
				++Cut;
			}
		public:
			using TEMPSCENE::TEMPSCENE;
			void Awake(void) noexcept override {
				TEMPSCENE::Awake();
			}
		public:
			void Set(void) noexcept override {
				TEMPSCENE::Set_EnvLight(VECTOR_ref::vget(5.f, 5.f, 5.f), VECTOR_ref::vget(-5.f, -5.f, -5.f), VECTOR_ref::vget(0.5f, -0.5f, 0.5f), GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				TEMPSCENE::Set();
				SetMousePoint(DrawPts->disp_x / 2, DrawPts->disp_y / 2);											//

				Cut = 0;
				BGM = SoundHandle::Load("data/base_movie.wav");
				movie = GraphHandle::Load("data/base_movie.mp4");
				PlayMovieToGraph(movie.get(),2, DX_MOVIEPLAYTYPE_BCANCEL);
				ChangeMovieVolumeToGraph(128, movie.get());
				//SeekMovieToGraph(movie.get(), 876);

				//�H�L����O
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/1.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 0.450f);
				//�H�L���蒆
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/2.bmp");//2
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 2.714f);

				//��
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/3.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 3.021f);
				//�w��
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/4.bmp");//2
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 3.258f);
				//�E����
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/5.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 3.512f);
				//����
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/6.bmp");//2
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 3.724f);

				//�H�؂�͂���
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/7.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 4.310f);
				//�H�؂�I���
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/8.bmp");//2
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 5.086f);
				//�a�V
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/9.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 6.448f);
				//����
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/10.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 7.096f);
				//��
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/11.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 7.288f);
				//��2
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/12.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 7.576f);
				//��3
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/13.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 7.864f);
				//||
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/14.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 10.264f);
				//�o�C�N
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/15.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 13.410f);
				//�N���肤���ƂȂ�
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/16.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 15.80f);
				//�S�Ă���
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/17.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 17.80f);
				//���ɂȂ邾��(�V��)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/18.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 19.0f);
				//���@�I
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/19.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 20.334f);
				//�΂ꂵ����(�����)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/20.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 21.452f);
				//覐�
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/21.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 23.422f);
				//���[���@
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/22.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 24.252f);
				//�H���C�v
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/23.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 24.452f);
				//���[���A(�\��)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/24.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 25.164f);
				//�����^���[(�����n�߂āj
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/25.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 26.526f);
				//�o�C�N�߂�(��@���E�����ɖ߂��ɂ͂��j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/26.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 29.790);
				//�o���o�C�N�@(�����@�����j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/27.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 31.670);
				//�o���o�C�N�A(�񂪂ȁj
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/28.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 32.52);
				//�Q�鏗(���@Moving First �����j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/29.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 34.654);
				//�J�����(��̂��j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/30.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 35.522);
				//�J������A(���v�j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/31.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 36.775);
				//�N���X(���点�j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/32.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 38.173);
				//�z(�j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/33.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 38.986);
				//�A(�����́j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/34.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 40.11);
				//��@(���́j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/35.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 41.226);
				//��A(��ցj
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/36.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 42.317);
				//�K�`��(�j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/37.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 42.726);
				//����(�j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/38.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 43.641);
				//�I�t(�j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/39.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 43.971);
				//�I�t2(�j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/40.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 44.311);
				//�I�t�����(�N�̑��݁j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/41.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 46.084);
				//�I�t�ߕt��(�키���j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/42.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 47.122);
				//�I�t�K�`����(�с@���܁j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/43.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 48.090);
				//�I�t�K�`����(��ς��j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/44.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 49.761);
				//�����i�ځj
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/45.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 50.024);
				//���C�v�i�Ɂj
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/46.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 50.447);
				//����(��)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/47.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 50.830);
				//���C�v�i����j
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/48.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 51.601);
				//����(�X�s�[�h�����Ă����[�V��)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/49.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 54.848);
				//��(�� �������������Ȃ�)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/50.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 57.071);
				//���΁[(���@)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/51.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 57.462);
				//�J�����(�N��)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/52.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 58.004);
				//�J�����(�����@�M������)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/53.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 60.290);
				//�N���b�N�A�b�v(���@������)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/54.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 62.775);
				//����(�r�W����)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/55.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 64.074);
				//�߂�(�������Ȃ�)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/56.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 66.165);
				//����(�t���Ă����Ȃ�)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/57.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 70.592);
				BGM.play(DX_PLAYTYPE_BACK, TRUE);
				BGM.vol(255);

				BaseTime = GetNowHiPerformanceCount();
			}
			bool Update(void) noexcept override {
				TEMPSCENE::Update();
				const auto NowTime = GetNowHiPerformanceCount() - BaseTime;
				{
					if (!Time_Over()) {
						if (NowTime > GetCutTime()) {
							AddCutNum();
						}
					}
				}
				if (Time_Over()) {
					return false;
				}
				if (!BGM.check()) {
					return false;
				}
				return true;
			}
			void Dispose(void) noexcept override {
				for (auto& p : Cut_Pic) { p.handle.Dispose(); }
				Cut_Pic.clear();
				BGM.Dispose();
				movie.Dispose();
			}
			void UI_Draw(void) noexcept  override {
				printfDx("Cut : %d\n", Cut);
			}
			void BG_Draw(void) noexcept override {
				if (!Time_Over()) {
					Cut_Pic[Cut].handle.DrawExtendGraph(0, 0, DrawPts->disp_x, DrawPts->disp_y, FALSE);
				}
				movie.DrawExtendGraph(0, 0, DrawPts->disp_x / 4, DrawPts->disp_y / 4, FALSE);
			}
			void Shadow_Draw_Far(void) noexcept override {
			}
			void Shadow_Draw_NearFar(void) noexcept override {
			}
			void Shadow_Draw(void) noexcept override {
			}
			void Main_Draw(void) noexcept override {
			}
			void Item_Draw(void) noexcept override {
				TEMPSCENE::Item_Draw();
			}
			void LAST_Draw(void) noexcept override {
			}
		};
	};
};