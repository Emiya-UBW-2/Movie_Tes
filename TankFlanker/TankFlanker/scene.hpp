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
			//引き継ぐ
			std::shared_ptr<OPTION> OPTPTs;
			//カメラ
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
			scenes Next_scene{ scenes::NONE_SCENE };			//現在のシーン
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
				SetUseMaskScreenFlag(FALSE);//←カスタム画面でエフェクトが出なくなるため入れる
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
				//音位置指定
				Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
				//影用意
				DrawPts->Ready_Shadow(camera_main.campos, [&] { Shadow_Draw(); }, [&] { Shadow_Draw_NearFar(); }, VECTOR_ref::vget(40.5f, 40.5f, 42.5f), VECTOR_ref::vget(85.f, 8.5f, 85.f));//MAIN_LOOPのnearはこれ (Get_Mine()->Damage.Get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
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
			MV1 Takion;
			class Cut_Info {
			public:
				cam_info Aim_camera;
				float cam_per = 0.0f;
				GraphHandle handle;
				LONGLONG TIME = 0;

				Cut_Info() {
					Aim_camera.campos = VECTOR_ref::vget(0, 10, -30);
					Aim_camera.camvec = VECTOR_ref::vget(0, 10, 0);
					Aim_camera.camup = VECTOR_ref::up();
					Aim_camera.set_cam_info(deg2rad(15), 1.0f, 100.f);
					cam_per = 0.95f;
				}
			};
			//データ
			SoundHandle BGM;
			size_t Cut = 0;
			std::vector<Cut_Info> Cut_Pic;
			LONGLONG BaseTime = 0;

			GraphHandle movie;
		private:
			void Sel_AnimNum(MV1&model,int sel) {
				for (auto& anim : model.get_anime()) {
					if (&anim - &model.get_anime().front() == sel) {
						model.get_anime(&anim - &model.get_anime().front()).per = 1.0f;
					}
					else {
						model.get_anime(&anim - &model.get_anime().front()).per = 0.0f;
					}
				}
			}


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

				MV1::LoadonAnime("data/umamusume/Tachyon/model.mv1", &Takion);

				camera_main.campos = VECTOR_ref::vget(0, 20, -20);
				camera_main.camvec = VECTOR_ref::vget(0, 20, 0);
				camera_main.camup = VECTOR_ref::up();
				camera_main.set_cam_info(deg2rad(15), 1.0f, 100.f);
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

				//羽広がり前
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/1.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 0.450f);
				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, -10, 0);
				Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(0, -10, -10);
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().Aim_camera.fov = deg2rad(45);
				Cut_Pic.back().cam_per = 0.f;
				//羽広がり中
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/2.bmp");//2
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 2.714f);
				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, -10, 0);
				Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(0, -10, -10);
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().Aim_camera.fov = deg2rad(45);
				Cut_Pic.back().cam_per = 0.f;
				//胸
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/3.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 3.021f);
				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(1, 13.82f, -2.08);
				Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
					VECTOR_ref::vget(
						-cos(deg2rad(20.1))*sin(-deg2rad(22.9)),
						sin(deg2rad(20.1)),
						-cos(deg2rad(20.1))*cos(-deg2rad(22.9))
					)*10.3f;
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().cam_per = 0.f;
				//背中
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/4.bmp");//2
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 3.258f);
				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-2.57f, 10.04f,0.31f);
				Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
					VECTOR_ref::vget(
						-cos(deg2rad(6.3))*sin(-deg2rad(225.7)),
						sin(deg2rad(6.3)),
						-cos(deg2rad(6.3))*cos(-deg2rad(225.7))
					)*10.3f;
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().cam_per = 0.f;
				//右足→
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/5.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 3.512f);
				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-0.85f, 16.69f, 0.61f);
				Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
					VECTOR_ref::vget(
						-cos(deg2rad(15.5))*sin(-deg2rad(202.8)),
						sin(deg2rad(15.5)),
						-cos(deg2rad(15.5))*cos(-deg2rad(202.8))
					)*10.3f;
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().cam_per = 0.f;
				//頭←
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/6.bmp");//2
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 3.724f);
				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-0.54f, 16.05f, -1.28f);
				Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
					VECTOR_ref::vget(
						-cos(deg2rad(8.6))*sin(-deg2rad(73.9)),
						sin(deg2rad(8.6)),
						-cos(deg2rad(8.6))*cos(-deg2rad(73.9))
					)*10.3f;
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().cam_per = 0.f;
				//羽切るはじめ
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/7.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 4.310f);
				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, -10, 0);
				Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(0, -10, -10);
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().Aim_camera.fov = deg2rad(45);
				Cut_Pic.back().cam_per = 0.f;
				//羽切る終わり
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/8.bmp");//2
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 5.086f);
				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(1.11f, 14.36f, -1.74f);
				Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
					VECTOR_ref::vget(
						-cos(deg2rad(6.9))*sin(-deg2rad(19.5)),
						sin(deg2rad(6.9)),
						-cos(deg2rad(6.9))*cos(-deg2rad(19.5))
					)*27.0f;
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().cam_per = 0.f;
				//斬新
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/9.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 6.448f);
				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(1.11f, 14.36f, -1.74f);
				Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
					VECTOR_ref::vget(
						-cos(deg2rad(6.9))*sin(-deg2rad(19.5)),
						sin(deg2rad(6.9)),
						-cos(deg2rad(6.9))*cos(-deg2rad(19.5))
					)*27.0f;
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().cam_per = 0.f;
				//＜＞
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/10.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 7.096f);

				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-14.621624f, 7.440908f, -16.774031f);
				Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(17.178366f, 9.319146f, -36.016518f);
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().Aim_camera.fov = deg2rad(45);
				Cut_Pic.back().cam_per = 0.f;

				//顔
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/11.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 7.288f);

				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-4.494195f, 12.919953f, -28.361502f);
				Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(2.366895f, 16.169621f, -21.344687f);
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().Aim_camera.fov = deg2rad(45);
				Cut_Pic.back().cam_per = 0.f;

				//顔2
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/12.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 7.576f);

				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-0.106448f, 3.030100f, -23.623045f);
				Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(2.044426f, 4.483082f, -26.842037f);
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().Aim_camera.fov = deg2rad(45);
				Cut_Pic.back().cam_per = 0.f;

				//顔3
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/13.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 7.864f);

				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(2.589150f, 10.542250f, -24.661646f);
				Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(3.584894f, 12.149980f, -20.973043f);
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().Aim_camera.fov = deg2rad(45);
				Cut_Pic.back().cam_per = 0.f;

				//||
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/14.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 10.264f);

				Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-14.621624f, 7.440908f, -16.774031f);
				Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(17.178366f, 9.319146f, -36.016518f);
				Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
				Cut_Pic.back().Aim_camera.fov = deg2rad(45);
				Cut_Pic.back().cam_per = 0.f;

				//バイク
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/15.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 13.410f);
				//君が願うことなら
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/16.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 15.80f);
				//全てが現
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/17.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 17.80f);
				//実になるだろ(天道)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/18.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 19.0f);
				//う　選
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/19.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 20.334f);
				//ばれしもの(加賀美)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/20.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 21.452f);
				//隕石
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/21.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 23.422f);
				//ワーム①
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/22.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 24.252f);
				//羽ワイプ
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/23.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 24.452f);
				//ワーム②(暴そ)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/24.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 25.164f);
				//東京タワー(うを始めて）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/25.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 26.526f);
				//バイク戻る(る　世界を元に戻すにはぁ）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/26.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 29.790);
				//バンバイク①(もう　時か）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/27.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 31.670);
				//バンバイク②(んがな）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/28.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 32.52);
				//寝る女(い　Moving First ここ）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/29.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 34.654);
				//雨加賀美(ろのぉ）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/30.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 35.522);
				//雨加賀美②(時計）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/31.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 36.175);
				//雨加賀美②(時計）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/32.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 36.775);
				//クロス(走らせ）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/33.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 38.173);
				//陽(）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/34.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 38.986);
				//陰(明日の）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/35.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 40.11);
				//手①(その）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/36.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 41.226);
				//手②(先へ）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/37.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 42.317);
				//ガチャ(）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/38.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 42.726);
				//握手(）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/39.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 43.641);
				//オフ(）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/40.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 43.971);
				//オフ2(）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/41.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 44.311);
				//オフ離れる(君の存在）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/42.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 46.084);
				//オフ近付く(戦うた）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/43.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 47.122);
				//オフガチャン(び　生ま）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/44.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 48.090);
				//オフガチャン(れ変わる）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/45.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 49.761);
				//爆発（目）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/46.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 50.024);
				//ワイプ（に）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/47.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 50.447);
				//爆発(見)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/48.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 50.830);
				//ワイプ（える）
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/49.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 51.601);
				//走る(スピード超えてくモーショ)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/50.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 54.848);
				//女(ン いったい自分以が)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/51.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 57.071);
				//だばー(い　)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/52.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 57.462);
				//雨加賀美(誰の)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/53.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 58.004);
				//雨加賀美(強さ　信じられる)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/54.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 60.290);
				//クロックアップ(ぅ　高速の)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/55.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 62.775);
				//遠い(ビジョン)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/56.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 64.074);
				//近い(見逃すなぁ)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/57.bmp");//1
				Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 66.165);
				//覆う(付いてこれるなら)
				Cut_Pic.resize(Cut_Pic.size() + 1);
				Cut_Pic.back().handle = GraphHandle::Load("data/cut/58.bmp");//1
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
						easing_set(&camera_main.campos, Cut_Pic[Cut].Aim_camera.campos, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.camvec, Cut_Pic[Cut].Aim_camera.camvec, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.camup, Cut_Pic[Cut].Aim_camera.camup, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.fov, Cut_Pic[Cut].Aim_camera.fov, Cut_Pic[Cut].cam_per);
						camera_main.set_cam_info(camera_main.fov, 1.0f, 200.f);
						if (Cut < 7) {
							Sel_AnimNum(Takion, 0);
						}
						else if (Cut < 9) {
							Sel_AnimNum(Takion, 1);
							Takion.get_anime(2).time = 33.f;
						}
						else if (Cut < 15) {
							Sel_AnimNum(Takion, 2);
							if (Cut >= 9 && Cut <= 12) {
								Takion.get_anime(2).Update(true, 0.01f);
							}
							if (Cut >= 13) {
								if (NowTime > (LONGLONG)(1000000.f * 8.064f)) {
									Takion.get_anime(2).Update(true, 0.6f);
									Cut_Pic[Cut].Aim_camera.camvec = Takion.frame(6) + VECTOR_ref::vget(-3.f + float(GetRand(2 * 3 * 10)) / 10.f, -3.f + float(GetRand(2 * 3 * 10)) / 10.f, -3.f + float(2 * 3 * 10) / 10.f);
									Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(17.178366f, 4.319146f, -36.016518f);
									Cut_Pic[Cut].cam_per = 0.9f;
								}
							}
						}

						Takion.get_anime(0).Update(true, 1.f);
						Takion.get_anime(1).Update(true, 1.f);
						//Takion.get_anime(2).Update(true, 1.f);
					}
					if (!Time_Over()) {
						if (NowTime > GetCutTime()) {
							AddCutNum();
						}
					}
					Takion.work_anime();
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
				Takion.DrawModel();
			}
			void Main_Draw(void) noexcept override {
				Takion.DrawModel();
			}
			void Item_Draw(void) noexcept override {
				TEMPSCENE::Item_Draw();
			}
			void LAST_Draw(void) noexcept override {
			}
		};
	};
};