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
			std::shared_ptr<HostPassEffect> HostpassPTs;
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
				const std::shared_ptr<OPTION>& OPTPTs_t,
				std::shared_ptr<HostPassEffect>& HostpassPTs_t
			) noexcept {
				PTR_COMMON::Set_Ptr_Common(DrawPts_t);
				OPTPTs = OPTPTs_t;
				HostpassPTs = HostpassPTs_t;
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
				DrawPts->Ready_Shadow(camera_main.campos, [&] { Shadow_Draw(); }, [&] { Shadow_Draw_NearFar(); }, VECTOR_ref::vget(100.f, 30.f, 100.f), VECTOR_ref::vget(camera_main.far_, 40.f, camera_main.far_));//MAIN_LOOPのnearはこれ (Get_Mine()->Damage.Get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
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
		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			MV1 Gate;
			MV1 Board;
			MV1 Map;
			MV1 Map_school;
			MV1 sky;							//空
			MV1 Ship;
			//太陽
			GraphHandle sun_pic;				/*画像ハンドル*/
			VECTOR_ref sun_pos;

			MV1 Takion;
			bool draw_t = true;
			MV1 Takion2;
			bool draw_t2 = true;
			MV1 Takion3_1;
			bool draw_t3_1 = true;
			MV1 Takion3_2;
			bool draw_t3_2 = true;
			MV1 Takion3_3;
			bool draw_t3_3 = true;
			MV1 Takion3_4;
			bool draw_t3_4 = true;

			MV1 Cafe;
			bool draw_cafe = true;
			MV1 scarlet;
			bool draw_scar = true;
			MV1 vodka;
			bool draw_vodka = true;
			MV1 goldship;
			bool draw_gold = true;
			MV1 speweek;
			bool draw_spe = true;
			MV1 teio;
			bool draw_te = true;
			MV1 macin;
			bool draw_mc = true;
			MV1 karen;
			bool draw_kr = true;
			MV1 trainer;
			bool draw_tnr = true;

			MV1 opera;
			bool draw_opr = true;
			MV1 doto;
			bool draw_dto = true;

			MV1 mobu_b_Base;
			std::vector<MV1> mobu_b;
			std::vector<int> mobu_b_anim;
			std::vector<float> mobu_b_run;
			std::vector<float> mobu_b_runrange;

			MV1 NewsPaper_Base;
			class newsmov {
			public:
				moves move;
				MV1 model;
			};
			std::vector<newsmov> Newspaper;

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
					Aim_camera.set_cam_info(deg2rad(15), 1.0f, 300.f);
					cam_per = 0.95f;
				}
			};

			class Cut_tex {
			public:
				int xpos = 0;
				int ypos = 0;
				int size = 0;
				std::string str;
				LONGLONG START_TIME = 0;
				LONGLONG END_TIME = 0;

				Cut_tex() {
					xpos = 0;
					ypos = 0;
					size = 12;
					str = "test";
					START_TIME = (LONGLONG)(1000000.f * 0.01f);
					END_TIME = (LONGLONG)(1000000.f * 1.01f);
				}
			};

			//データ
			SoundHandle BGM;
			size_t Cut = 0;
			std::vector<Cut_Info> Cut_Pic;
			std::vector<Cut_tex> Texts;
			LONGLONG BaseTime = 0;
			GraphHandle movie;


			GraphHandle face;

			int xp_n, yp_n;
			class newspp {
			public:
				int xns_pos, yns_pos;
				float rns_rad = 0.f;
				GraphHandle graph;
			};
			std::vector<newspp> news_p;

			class sodes {
			public:
				float xpos_base = 0;
				float ypos_base = 0;
				float rad_base = 0;

				float xpos_rand = 0;
				float ypos_rand = 0;
				float rad_rand = 0;

				float xpos = 0;
				float ypos = 0;
				float rad = 0;
				float Alpha = 1.f;
				float Alpha_R = 1.f;
				float Scale = 1.f;
				GraphHandle handle;
			};

			std::vector<sodes> sode;
			std::vector<sodes> anim;
			std::vector<sodes> sode_last;
			std::vector<sodes> First;
			std::vector<sodes> First2;
			std::vector<sodes> First3;

			GraphHandle Logo;
			float per_logo = 100.f;
			float size_rand = 0.f;
			float xpos_rand = 0.f;
			float xpos_rand2 = 0.f;
			float ypos_rand = 0.f;
			float ypos_rand2 = 0.f;
		private:
			void Sel_AnimNum(MV1&model, int sel) {
				for (auto& anim_t : model.get_anime()) {
					if (&anim_t - &model.get_anime().front() == sel) {
						model.get_anime(&anim_t - &model.get_anime().front()).per = 1.0f;
					}
					else {
						model.get_anime(&anim_t - &model.get_anime().front()).per = 0.0f;
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
				//
				SetUseASyncLoadFlag(TRUE);
				{
					this->sun_pic = GraphHandle::Load("data/sun.png");					/*sun*/
					MV1::LoadonAnime("data/umamusume/Cafe/model.mv1", &Cafe);
					MV1::LoadonAnime("data/umamusume/scarlet/model.mv1", &scarlet);
					MV1::LoadonAnime("data/umamusume/vodka/model.mv1", &vodka);
					MV1::LoadonAnime("data/umamusume/spe/model.mv1", &speweek);
					MV1::LoadonAnime("data/umamusume/gold/model.mv1", &goldship);
					MV1::LoadonAnime("data/umamusume/teio/model.mv1", &teio);
					MV1::LoadonAnime("data/umamusume/mac/model.mv1", &macin);
					MV1::LoadonAnime("data/umamusume/karen/model.mv1", &karen);

					MV1::LoadonAnime("data/umamusume/trainer/model.mv1", &trainer);

					MV1::LoadonAnime("data/umamusume/opera/model.mv1", &opera);
					MV1::LoadonAnime("data/umamusume/doto/model.mv1", &doto);


					face = GraphHandle::Load("data/Cut.png");
					news_p.resize(news_p.size() + 1);
					for (int i = 1; i < 10; i++) {
						news_p.resize(news_p.size() + 1);
						news_p.back().graph = GraphHandle::Load("data/Cut2.png");
					}
					news_p.back().graph.GetSize(&xp_n, &yp_n);
					MV1::LoadonAnime("data/map/model_gate.mv1", &Gate);
					MV1::Load("data/map/model.mv1", &Map, true);
					MV1::Load("data/board/model.mv1", &Board, true);
					MV1::Load("data/school/model.mv1", &Map_school, true);
					MV1::Load("data/sky/model.mv1", &sky, true);				//空
					MV1::Load("data/ship/model.mv1", &Ship, true);				//空
				}
				SetUseASyncLoadFlag(FALSE);
				{
					MV1::LoadonAnime("data/umamusume/mobu_black/model.mv1", &mobu_b_Base);
					mobu_b.resize(12);
					mobu_b_run.resize(12);
					mobu_b_runrange.resize(12);
					mobu_b_anim.resize(12);
					for (int i = 0; i < 12; i++) {
						mobu_b_Base.DuplicateonAnime(&mobu_b[i], &mobu_b[i]);
						mobu_b[i].SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, -100.f, 0.f)));
						mobu_b_run[i] = 0.8f + (float)(GetRand(195)) / 1000.f;
						mobu_b_runrange[i] = (float)(GetRand(100)) / 10.f;
						while (true)
						{
							mobu_b_anim[i] = GetRand(2);
							if (i == 0 || mobu_b_anim[i] != mobu_b_anim[i - 1]) {
								break;
							}
						}
					}

					MV1::Load("data/paper/news.mv1", &NewsPaper_Base, false);
					for (int i = 0; i < 12; i++) {
						Newspaper.resize(Newspaper.size() + 1);
						Newspaper.back().model = NewsPaper_Base.Duplicate();
					}

					MV1::LoadonAnime("data/umamusume/Tachyon/model.mv1", &Takion);
					Takion.DuplicateonAnime(&Takion2, &Takion2);
					Takion.DuplicateonAnime(&Takion3_1, &Takion3_1);
					Takion.DuplicateonAnime(&Takion3_2, &Takion3_2);
					Takion.DuplicateonAnime(&Takion3_3, &Takion3_3);
					Takion.DuplicateonAnime(&Takion3_4, &Takion3_4);
					sode.resize(sode.size() + 1);
					sode.back().xpos = (float)(DrawPts->disp_x / 5);
					sode.back().ypos = (float)(DrawPts->disp_y);
					sode.back().handle = GraphHandle::Load("data/first_sode/1.png");
					sode.resize(sode.size() + 1);
					sode.back().xpos = (float)(-DrawPts->disp_x / 5);
					sode.back().ypos = (float)(DrawPts->disp_y);
					sode.back().handle = GraphHandle::Load("data/first_sode/2.png");
					sode.resize(sode.size() + 1);
					sode.back().xpos = (float)(DrawPts->disp_x / 5);
					sode.back().ypos = (float)(DrawPts->disp_y);
					sode.back().handle = GraphHandle::Load("data/first_sode/3.png");
					sode.resize(sode.size() + 1);
					sode.back().xpos = (float)(-DrawPts->disp_x / 5);
					sode.back().ypos = (float)(DrawPts->disp_y);
					sode.back().handle = GraphHandle::Load("data/first_sode/4.png");

					sode.resize(sode.size() + 1);
					sode.back().xpos = (float)(-DrawPts->disp_x / 5);
					sode.back().ypos = (float)(DrawPts->disp_y);
					sode.back().handle = GraphHandle::Load("data/second_sode/1.png");
					sode.resize(sode.size() + 1);
					sode.back().xpos = (float)(DrawPts->disp_x / 5);
					sode.back().ypos = (float)(DrawPts->disp_y);
					sode.back().handle = GraphHandle::Load("data/second_sode/2.png");
					sode.resize(sode.size() + 1);
					sode.back().xpos = (float)(-DrawPts->disp_x / 5);
					sode.back().ypos = (float)(DrawPts->disp_y);
					sode.back().handle = GraphHandle::Load("data/second_sode/1.png");
					sode.resize(sode.size() + 1);
					sode.back().xpos = (float)(DrawPts->disp_x / 5);
					sode.back().ypos = (float)(DrawPts->disp_y);
					sode.back().handle = GraphHandle::Load("data/second_sode/2.png");

					Logo = GraphHandle::Load("data/logo.png");
					{
						for (int i = 0; i < 10; i++) {
							sode_last.resize(sode_last.size() + 1);
							sode_last.back().xpos = (float)(DrawPts->disp_x / 2 + DrawPts->disp_x);
							sode_last.back().ypos = (float)(DrawPts->disp_y / 2);
							sode_last.back().rad = (float)(0.f);
							sode_last.back().handle = GraphHandle::Load("data/second_sode/1.png");
						}

						sode_last[0].xpos_base = (float)(DrawPts->disp_x / 2 - y_r(150));
						sode_last[0].ypos_base = (float)(DrawPts->disp_y / 2 + y_r(100));
						sode_last[0].rad_base = deg2rad(-350);

						sode_last[1].xpos_base = (float)(DrawPts->disp_x / 2 - y_r(300));
						sode_last[1].ypos_base = (float)(DrawPts->disp_y / 2 - y_r(500));
						sode_last[1].rad_base = deg2rad(-320);

						sode_last[2].xpos_base = (float)(DrawPts->disp_x / 2);
						sode_last[2].ypos_base = (float)(DrawPts->disp_y / 2 - y_r(600));
						sode_last[2].rad_base = deg2rad(-270);

						sode_last[3].xpos_base = (float)(DrawPts->disp_x / 2 - y_r(500));
						sode_last[3].ypos_base = (float)(DrawPts->disp_y / 2 + y_r(300));
						sode_last[3].rad_base = deg2rad(-360);

						sode_last[4].xpos_base = (float)(DrawPts->disp_x / 2 + y_r(400));
						sode_last[4].ypos_base = (float)(DrawPts->disp_y / 2 - y_r(550));
						sode_last[4].rad_base = deg2rad(-230);

						sode_last[5].xpos_base = (float)(DrawPts->disp_x / 2 + y_r(500));
						sode_last[5].ypos_base = (float)(DrawPts->disp_y / 2 + y_r(500));
						sode_last[5].rad_base = deg2rad(-100);

						sode_last[6].xpos_base = (float)(DrawPts->disp_x / 2 + y_r(600));
						sode_last[6].ypos_base = (float)(DrawPts->disp_y / 2 - y_r(300));
						sode_last[6].rad_base = deg2rad(-200);

						sode_last[7].xpos_base = (float)(DrawPts->disp_x / 2 + y_r(700));
						sode_last[7].ypos_base = (float)(DrawPts->disp_y / 2 + y_r(100));
						sode_last[7].rad_base = deg2rad(-180);

						sode_last[8].xpos_base = (float)(DrawPts->disp_x / 2 + y_r(50));
						sode_last[8].ypos_base = (float)(DrawPts->disp_y / 2 + y_r(700));
						sode_last[8].rad_base = deg2rad(-60);

						sode_last[9].xpos_base = (float)(DrawPts->disp_x / 2 - y_r(450));
						sode_last[9].ypos_base = (float)(DrawPts->disp_y / 2 + y_r(550));
						sode_last[9].rad_base = deg2rad(-40);

						for (auto& sl : sode_last) {
							sl.xpos = (float)(DrawPts->disp_x / 2) + (sl.xpos_base - (float)(DrawPts->disp_x / 2)) * 10.f;
							sl.ypos = (float)(DrawPts->disp_y / 2) + (sl.ypos_base - (float)(DrawPts->disp_y / 2)) * 10.f;
							sl.rad = sl.rad_base + deg2rad(90);
						}
					}

					{
						for (int i = 0; i < 3; i++) {
							First.resize(First.size() + 1);
							First.back().xpos = (float)(DrawPts->disp_x / 2 + DrawPts->disp_x);
							First.back().ypos = (float)(DrawPts->disp_y / 2);
							First.back().rad = (float)(0.f);
							First.back().handle = GraphHandle::Load("data/FIRST.png");
						}

						First[0].xpos_base = (float)(DrawPts->disp_x / 2 - y_r(1920 * 3 / 10));
						First[0].ypos_base = (float)(DrawPts->disp_y / 2 + y_r(100));

						First[1].xpos_base = (float)(DrawPts->disp_x / 2 + y_r(1920 / 4));
						First[1].ypos_base = (float)(DrawPts->disp_y / 2 - y_r(1080 * 3 / 10));

						First[2].xpos_base = (float)(DrawPts->disp_x / 2 + y_r(1920 / 6));
						First[2].ypos_base = (float)(DrawPts->disp_y / 2 + y_r(1080 * 3 / 10));


						for (int i = 0; i < 3; i++) {
							First2.resize(First2.size() + 1);
							First2.back().xpos = (float)(DrawPts->disp_x / 2 + DrawPts->disp_x);
							First2.back().ypos = (float)(DrawPts->disp_y / 2);
							First2.back().rad = (float)(0.f);
							First2.back().handle = GraphHandle::Load("data/sun.png");
						}

						First2[0].xpos_base = (float)(DrawPts->disp_x / 2 - y_r(1920 * 3 / 10));
						First2[0].ypos_base = (float)(DrawPts->disp_y / 2 + y_r(100));

						First2[1].xpos_base = (float)(DrawPts->disp_x / 2 + y_r(1920 / 4));
						First2[1].ypos_base = (float)(DrawPts->disp_y / 2 - y_r(1080 * 3 / 10));

						First2[2].xpos_base = (float)(DrawPts->disp_x / 2 + y_r(1920 / 6));
						First2[2].ypos_base = (float)(DrawPts->disp_y / 2 + y_r(1080 * 3 / 10));

						for (int i = 0; i < 3; i++) {
							First3.resize(First3.size() + 1);
							First3.back().xpos = (float)(DrawPts->disp_x / 2 + DrawPts->disp_x);
							First3.back().ypos = (float)(DrawPts->disp_y / 2);
							First3.back().Alpha = -1.f;
							First3.back().Alpha_R = 0.5f;
							First3.back().Scale = 1.f;
							First3.back().handle = GraphHandle::Load("data/sun.png");
						}

						First3[0].xpos_base = (float)(DrawPts->disp_x / 2 - y_r(1920 * 3 / 10));
						First3[0].ypos_base = (float)(DrawPts->disp_y / 2 + y_r(100));

						First3[1].xpos_base = (float)(DrawPts->disp_x / 2 + y_r(1920 / 4));
						First3[1].ypos_base = (float)(DrawPts->disp_y / 2 - y_r(1080 * 3 / 10));

						First3[2].xpos_base = (float)(DrawPts->disp_x / 2 + y_r(1920 / 6));
						First3[2].ypos_base = (float)(DrawPts->disp_y / 2 + y_r(1080 * 3 / 10));
					}

					anim.resize(anim.size() + 1);
					anim.back().xpos = (float)(0.f);
					anim.back().ypos = (float)(DrawPts->disp_y);
					anim.back().handle = GraphHandle::Load("data/anime/0.png");
					anim.resize(anim.size() + 1);
					anim.back().xpos = (float)(0.f);
					anim.back().ypos = (float)(DrawPts->disp_y);
					anim.back().handle = GraphHandle::Load("data/anime/1.png");
				}
				//
				{
					Gate.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.25f, 0.f))*MATRIX_ref::RotY(deg2rad(0)));
					Map.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.25f, -2394.f))*MATRIX_ref::RotY(deg2rad(90)));
					Board.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.25f, 0.f))*MATRIX_ref::RotY(deg2rad(0)));
					Map_school.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(-819.f, -28.2f, -590.f))*MATRIX_ref::RotY(deg2rad(-90)));

					Takion.PhysicsResetState();
					draw_tnr = false;
					draw_t = true;
					draw_scar = false;
					draw_cafe = false;
					draw_vodka = false;
					draw_t2 = false;
					draw_dto = false;
					draw_opr = false;
					Takion3_1.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
					Takion3_2.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
					Takion3_3.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
					Takion3_4.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
					speweek.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
					goldship.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
					teio.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
					macin.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
					draw_kr = false;
				}
				//
				camera_main.campos = VECTOR_ref::vget(0, 20, -20);
				camera_main.camvec = VECTOR_ref::vget(0, 20, 0);
				camera_main.camup = VECTOR_ref::up();
				camera_main.set_cam_info(deg2rad(15), 1.0f, 200.f);
			}
		public:
			void Set(void) noexcept override {
				TEMPSCENE::Set_EnvLight(VECTOR_ref::vget(500.f, 50.f, 500.f), VECTOR_ref::vget(-500.f, -50.f, -500.f), VECTOR_ref::vget(-0.5f, -0.5f, 0.5f), GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				TEMPSCENE::Set();
				this->sun_pos = Get_Light_vec().Norm() * -1500.f;
				Cut =  18;
				Cut = 0;
				//BGM = SoundHandle::Load("data/base_movie.wav");
				BGM = SoundHandle::Load("data/sound2.wav");
				
				movie = GraphHandle::Load("data/base_movie.mp4");
				PlayMovieToGraph(movie.get(), 2, DX_MOVIEPLAYTYPE_BCANCEL);
				ChangeMovieVolumeToGraph(0, movie.get());
				SetUseASyncLoadFlag(TRUE);
				{
					//羽広がり前
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/1.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 0.450f);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, -10, 0);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(0, -10, -10);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 10.f;
					Cut_Pic.back().cam_per = 0.f;
					//羽広がり中
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/2.bmp");//2
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 2.614f);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, -10, 0);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(0, -10, -10);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 10.f;
					Cut_Pic.back().cam_per = 0.f;
					//胸
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/3.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 3.021f);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(1.f, 13.82f, -2.08f);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-cos(deg2rad(20.1))*sin(-deg2rad(22.9)),
							sin(deg2rad(20.1)),
							-cos(deg2rad(20.1))*cos(-deg2rad(22.9))
						)*10.3f;
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.far_ = 20.f;
					Cut_Pic.back().cam_per = 0.f;
					//背中
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/4.bmp");//2
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 3.258f);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-2.57f, 10.04f, 0.31f);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-cos(deg2rad(6.3))*sin(-deg2rad(225.7)),
							sin(deg2rad(6.3)),
							-cos(deg2rad(6.3))*cos(-deg2rad(225.7))
						)*10.3f;
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.far_ = 20.f;
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
					Cut_Pic.back().Aim_camera.far_ = 20.f;
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
					Cut_Pic.back().Aim_camera.far_ = 20.f;
					Cut_Pic.back().cam_per = 0.f;
					//羽切るはじめ
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/7.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 4.310f);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, -10, 0);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(0, -10, -10);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 10.f;
					Cut_Pic.back().cam_per = 0.f;
					//羽切る終わり
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/8.bmp");//2
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 5.086f);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(1.11f, 14.36f, -1.74f);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-cos(deg2rad(6.9))*sin(-deg2rad(180 + 19.5)),
							sin(deg2rad(6.9)),
							-cos(deg2rad(6.9))*cos(-deg2rad(180 + 19.5))
						)*27.0f;
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.far_ = 40.f;
					Cut_Pic.back().cam_per = 0.f;
					//斬新
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/9.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 6.448f);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(1.11f, 14.36f, -1.74f);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-cos(deg2rad(6.9))*sin(-deg2rad(180 + 19.5)),
							sin(deg2rad(6.9)),
							-cos(deg2rad(6.9))*cos(-deg2rad(180 + 19.5))
						)*27.0f;
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.far_ = 40.f;
					Cut_Pic.back().cam_per = 0.f;
					//＜＞
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/10.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 7.096f);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-14.621624f, 7.440908f, -16.774031f);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(17.178366f, 9.319146f, -36.016518f);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					Cut_Pic.back().cam_per = 0.f;

					//顔
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/11.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 7.288f);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-4.494195f, 12.919953f, -28.361502f);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(2.366895f, 16.169621f, -21.344687f);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					Cut_Pic.back().cam_per = 0.f;

					//顔2
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/12.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 7.576f);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-0.106448f, 3.030100f, -23.623045f);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(2.044426f, 4.483082f, -26.842037f);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					Cut_Pic.back().cam_per = 0.f;

					//顔3
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/13.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 7.864f);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(2.589150f, 10.542250f, -24.661646f);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(3.584894f, 12.149980f, -16.973043f);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					Cut_Pic.back().cam_per = 0.f;

					//||
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/14.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 10.264f);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-14.621624f, 7.440908f, -16.774031f);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(17.178366f, 9.319146f, -36.016518f);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					Cut_Pic.back().cam_per = 0.f;

					//バイク
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/15.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 13.410f);

					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(-7.410444f, 11.652717f, 2.655200f);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(-6.079808f, 12.230768f, -1.217081f);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					Cut_Pic.back().cam_per = 0.f;

					//君が願うことなら
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/16.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 15.80f);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(10.122698f, 18.028077f, -100.985756f);

					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-cos(deg2rad(-5.5f))*sin(-deg2rad(float(90 - 10))),
							sin(deg2rad(-5.5)),
							-cos(deg2rad(-5.5))*cos(-deg2rad(float(90 - 10)))
						)*88.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					Cut_Pic.back().cam_per = 0.f;

					//全てが現
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/17.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 17.80f);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(10.122698f, 18.028077f, -100.985756f);

					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-cos(deg2rad(-5.5f))*sin(-deg2rad(float(90 - 10))),
							sin(deg2rad(-5.5)),
							-cos(deg2rad(-5.5))*cos(-deg2rad(float(90 - 10)))
						)*88.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					Cut_Pic.back().cam_per = 0.f;

					//実になるだろ(天道)
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/18.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 19.0f);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					//う　選
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/19.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 20.334f);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					//ばれしもの(加賀美)
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/20.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 21.452f);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					//隕石
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/21.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 23.422f);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 2, 0);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(0, 20, -20);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 200.f;
					Cut_Pic.back().cam_per = 0.f;
					//ワーム①
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/22.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 24.252f);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 15, 0);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(0, 15, -20);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 200.f;
					Cut_Pic.back().cam_per = 0.f;
					//羽ワイプ
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/23.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 24.452f);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 15, 0);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(0, 15, -20);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 200.f;
					Cut_Pic.back().cam_per = 0.f;
					//ワーム②(暴そ)
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/24.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 25.164f);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 15, 0);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(0, 15, -20);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 200.f;
					Cut_Pic.back().cam_per = 0.f;
					//東京タワー(うを始めて）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/25.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 26.526f);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 20, 0);
					Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(0, 5, -20);
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().cam_per = 0.f;

					//バイク戻る(る　世界を元に戻すにはぁ）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/26.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 29.790);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(10.122698f, 18.028077f, -30.985756f);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-cos(deg2rad(-5.5f))*sin(-deg2rad(float(90 - 10))),
							sin(deg2rad(-5.5)),
							-cos(deg2rad(-5.5))*cos(-deg2rad(float(90 - 10)))
						)*108.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(25);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					Cut_Pic.back().cam_per = 0.f;
					//バンバイク①(もう　時か）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/27.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 31.670);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(10.122698f, 18.028077f, -100.985756f);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-cos(deg2rad(-5.5f))*sin(-deg2rad(float(90 - 10))),
							sin(deg2rad(-5.5)),
							-cos(deg2rad(-5.5))*cos(-deg2rad(float(90 - 10)))
						)*88.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					Cut_Pic.back().cam_per = 0.f;

					//バンバイク②(んがな）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/28.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 32.52);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(10.122698f, 10.028077f, -50.985756f);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-cos(deg2rad(0.f))*sin(-deg2rad(float(0 - 10))),
							sin(deg2rad(0.f)),
							-cos(deg2rad(0.f))*cos(-deg2rad(float(0 - 10)))
						)*88.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.far_ = 3000.f;
					Cut_Pic.back().cam_per = 0.f;
					//寝る女(い　Moving First ここ）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/29.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 34.654);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 0, 0);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							2.f,
							12.f,
							6.f
						)*1.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.near_ = 1.f;
					Cut_Pic.back().Aim_camera.far_ = 100.f;
					Cut_Pic.back().cam_per = 0.f;

					//雨加賀美(ろのぉ）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/30.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 35.522);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 0, 0);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							2.f,
							10.f,
							8.f
						)*1.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.near_ = 1.f;
					Cut_Pic.back().Aim_camera.far_ = 100.f;
					Cut_Pic.back().cam_per = 0.f;

					Cut_Pic.back().cam_per = 0.f;
					//雨加賀美②(時計）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/31.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 36.175);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 0, 0);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-2.f,
							12.f,
							-6.f
						)*2.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.near_ = 1.f;
					Cut_Pic.back().Aim_camera.far_ = 100.f;
					Cut_Pic.back().cam_per = 0.f;

					//雨加賀美②(時計）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/32.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 36.775);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 17, -4.5);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-10.f,
							0.f,
							0.f
						)*1.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(15);
					Cut_Pic.back().Aim_camera.near_ = 1.f;
					Cut_Pic.back().Aim_camera.far_ = 100.f;
					Cut_Pic.back().cam_per = 0.f;
					//クロス(走らせ）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/33.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 38.173);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 20, -7);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							20.f,
							0.f,
							0.f
						)*1.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.near_ = 1.f;
					Cut_Pic.back().Aim_camera.far_ = 100.f;
					Cut_Pic.back().cam_per = 0.f;
					//陽(）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/34.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 38.986);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 20, -7);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							0.f,
							6.f,
							10.f
						)*1.0f;
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.near_ = 1.f;
					Cut_Pic.back().Aim_camera.far_ = 300.f;
					Cut_Pic.back().cam_per = 0.f;
					//陰(明日の）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/35.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 40.11);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 20, -7);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							0.f,
							6.f,
							-10.f
						)*1.0f;
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(45);
					Cut_Pic.back().Aim_camera.near_ = 1.f;
					Cut_Pic.back().Aim_camera.far_ = 300.f;
					Cut_Pic.back().cam_per = 0.f;
					//手①(その）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/36.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 41.226);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 17, -4.5);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-10.f,
							0.f,
							0.f
						)*1.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(15);
					Cut_Pic.back().Aim_camera.near_ = 1.f;
					Cut_Pic.back().Aim_camera.far_ = 100.f;
					Cut_Pic.back().cam_per = 0.f;
					//手②(先へ）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/37.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 42.317);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 17, -4.5);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-10.f,
							0.f,
							0.f
						)*1.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(15);
					Cut_Pic.back().Aim_camera.near_ = 1.f;
					Cut_Pic.back().Aim_camera.far_ = 100.f;
					Cut_Pic.back().cam_per = 0.f;
					//ガチャ(）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/38.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 42.726);


					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0.f, 19.f, 0.f);

					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-cos(deg2rad(-25.5f))*sin(-deg2rad(float(90 - 50))),
							sin(deg2rad(-25.5f)),
							-cos(deg2rad(-25.5f))*cos(-deg2rad(float(90 - 50)))
						)*10.0f;
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(25);
					Cut_Pic.back().Aim_camera.far_ = 100.f;
					Cut_Pic.back().cam_per = 0.f;
					//握手(）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/39.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 43.641);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 17, -4.5);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-10.f,
							0.f,
							0.f
						)*1.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(15);
					Cut_Pic.back().Aim_camera.near_ = 1.f;
					Cut_Pic.back().Aim_camera.far_ = 100.f;
					Cut_Pic.back().cam_per = 0.f;
					//オフ(）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/40.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 43.971);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0.23f, 18, 0.0);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							0.f,
							0.f,
							10.f
						)*1.0f;
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(4);
					Cut_Pic.back().Aim_camera.near_ = 0.1f;
					Cut_Pic.back().Aim_camera.far_ = 20.f;
					Cut_Pic.back().cam_per = 0.f;
					//オフ2(）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/41.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 44.311);

					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0., 18, 0.);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							0.f,
							0.f,
							-10.f
						)*1.0f;
					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(19);
					Cut_Pic.back().Aim_camera.near_ = 1.f;
					Cut_Pic.back().Aim_camera.far_ = 100.f;
					Cut_Pic.back().cam_per = 0.f;

					//オフ離れる(君の存在）
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/42.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 46.084);

					Cut_Pic.back().cam_per = 0.f;
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
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 56.271);
					//だばー(い　)
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/52.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 57.462);
					//雨加賀美(誰の)
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/53.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 58.004);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 17, -4.5);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							-10.f,
							0.f,
							0.f
						)*1.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(15);
					Cut_Pic.back().Aim_camera.near_ = 1.f;
					Cut_Pic.back().Aim_camera.far_ = 30.f;
					Cut_Pic.back().cam_per = 0.f;
					//雨加賀美(強さ　信じられる)
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/54.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 60.290);
					Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 17, -4.5);
					Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
						VECTOR_ref::vget(
							0.f,
							0.f,
							-10.f
						)*1.0f;

					Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
					Cut_Pic.back().Aim_camera.fov = deg2rad(25);
					Cut_Pic.back().Aim_camera.near_ = 3.f;
					Cut_Pic.back().Aim_camera.far_ = 1000.f;
					Cut_Pic.back().cam_per = 0.9f;
					//クロックアップ(ぅ　高速の)
					Cut_Pic.resize(Cut_Pic.size() + 1);
					Cut_Pic.back().handle = GraphHandle::Load("data/cut/55.bmp");//1
					Cut_Pic.back().TIME = (LONGLONG)(1000000.f * 61.440);
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

				}
				{}
				{
					LONGLONG StartF = 0;
					LONGLONG ContiF = 0;

					StartF = (LONGLONG)(1000000.f * 2.614f);
					ContiF = (LONGLONG)(1000000.f * 1.5f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960-300);
						Texts.back().ypos = y_r(1080-200);
						Texts.back().size = y_r(12);
						Texts.back().str = "原作";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960-300);
						Texts.back().ypos = y_r(1080 - 200+12);
						Texts.back().size = y_r(32);
						Texts.back().str = "ウマ娘プリティーダービー";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 1.696f);//4.310
					ContiF = (LONGLONG)(1000000.f * 3.290f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 200);
						Texts.back().ypos = y_r(1080 - 200);
						Texts.back().size = y_r(12);
						Texts.back().str = "皐月賞馬";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 200);
						Texts.back().ypos = y_r(1080 - 200 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "アグネスタキオン";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 3.297f);//(LONGLONG)(1000000.f * 7.288f)
					ContiF = (LONGLONG)(1000000.f * 2.97f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 200);
						Texts.back().ypos = y_r(1080 - 200);
						Texts.back().size = y_r(12);
						Texts.back().str = "菊花賞馬";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 200);
						Texts.back().ypos = y_r(1080 - 200 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "マンハッタンカフェ";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 2.976f);//(LONGLONG)(1000000.f * 9.264f)
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 180);
						Texts.back().ypos = y_r(1080 - 200);
						Texts.back().size = y_r(12);
						Texts.back().str = "クロフネの子";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 180);
						Texts.back().ypos = y_r(1080 - 200 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "カレンチャン";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 2.146f);//(LONGLONG)(1000000.f * 11.410f)
					ContiF = (LONGLONG)(1000000.f * 1.925f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 100);
						Texts.back().ypos = y_r(1080 - 200);
						Texts.back().size = y_r(12);
						Texts.back().str = "全身発行体";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 100);
						Texts.back().ypos = y_r(1080 - 200 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "トレーナー";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 1.925f);//(LONGLONG)(1000000.f * 13.410f)
					ContiF = (LONGLONG)(1000000.f * 1.925f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 100);
						Texts.back().ypos = y_r(540);
						Texts.back().size = y_r(12);
						Texts.back().str = "トレセン学園理事長";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 100);
						Texts.back().ypos = y_r(540 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "秋川やよい";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 100);
						Texts.back().ypos = y_r(1080 - 200);
						Texts.back().size = y_r(12);
						Texts.back().str = "理事長秘書";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 100);
						Texts.back().ypos = y_r(1080 - 200 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "駿川たづな";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 1.925f);
					ContiF = (LONGLONG)(1000000.f * 1.925f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 100);
						Texts.back().ypos = y_r(540);
						Texts.back().size = y_r(12);
						Texts.back().str = "チームスピカ担当";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 100);
						Texts.back().ypos = y_r(540 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "沖野T";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 190);
						Texts.back().ypos = y_r(1080 - 200);
						Texts.back().size = y_r(12);
						Texts.back().str = "あげません";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 190);
						Texts.back().ypos = y_r(1080 - 200 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "スペシャルウィーク";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 1.925f);
					ContiF = (LONGLONG)(1000000.f * 1.925f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 190);
						Texts.back().ypos = y_r(540 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "トウカイ テイオー";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 190);
						Texts.back().ypos = y_r(540 + 12 + 42);
						Texts.back().size = y_r(32);
						Texts.back().str = "メジロマックイーン";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 190);
						Texts.back().ypos = y_r(540 + 12 + 42 * 3);
						Texts.back().size = y_r(32);
						Texts.back().str = "ウオッカ";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 190);
						Texts.back().ypos = y_r(540 + 12 + 42 * 4);
						Texts.back().size = y_r(32);
						Texts.back().str = "ダイワスカーレット";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 190);
						Texts.back().ypos = y_r(1080 - 200);
						Texts.back().size = y_r(12);
						Texts.back().str = "ナレーション";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1560 - 190);
						Texts.back().ypos = y_r(1080 - 200 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "ゴールドシップ";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 1.925f);
					ContiF = (LONGLONG)(1000000.f * 1.925f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 100);
						Texts.back().ypos = y_r(1080 - 200);
						Texts.back().size = y_r(12);
						Texts.back().str = "理事長代理";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 100);
						Texts.back().ypos = y_r(1080 - 200 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "樫本理子";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 1.925f);//(LONGLONG)(1000000.f * 23.422f)
					ContiF = (LONGLONG)(1000000.f * 1.925f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(480 - 100);
						Texts.back().ypos = y_r(810);
						Texts.back().size = y_r(32);
						Texts.back().str = " ";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1440 - 100);
						Texts.back().ypos = y_r(810 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF = (LONGLONG)(1000000.f * 25.164f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 200);
						Texts.back().ypos = y_r(1080 - 200);
						Texts.back().size = y_r(12);
						Texts.back().str = "脚本";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 200);
						Texts.back().ypos = y_r(1080 - 200 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "アグネスデジタル";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 2.000f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 200);
						Texts.back().ypos = y_r(1080 - 200);
						Texts.back().size = y_r(12);
						Texts.back().str = "音楽";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 200);
						Texts.back().ypos = y_r(1080 - 200 + 12);
						Texts.back().size = y_r(32);
						Texts.back().str = "アグネスデジタル";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 2.000f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1440 - 60);
						Texts.back().ypos = y_r(720);
						Texts.back().size = y_r(10);
						Texts.back().str = "オープニングテーマ";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1440 - 120);
						Texts.back().ypos = y_r(720 + 10);
						Texts.back().size = y_r(36);
						Texts.back().str = "『NEXT LEVEL』";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1440 - 90);
						Texts.back().ypos = y_r(720 + 10 + 36);
						Texts.back().size = y_r(36);
						Texts.back().str = "YU-KI";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1440 + 50);
						Texts.back().ypos = y_r(720 + 10 + 36 + 12);
						Texts.back().size = y_r(24);
						Texts.back().str = "(TRF)";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1440 - 90);
						Texts.back().ypos = y_r(720 + 10 + 36 + 4 + 32 + 11);
						Texts.back().size = y_r(10);
						Texts.back().str = "作　　詞";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1440 - 90);
						Texts.back().ypos = y_r(720 + 10 + 36 + 4 + 32*2 + 11);
						Texts.back().size = y_r(10);
						Texts.back().str = "作・編曲";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1440 - 30);
						Texts.back().ypos = y_r(720 + 10 + 36 + 4 + 32);
						Texts.back().size = y_r(32);
						Texts.back().str = "藤田聖子";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1440 - 30);
						Texts.back().ypos = y_r(720 + 10 + 36 + 4 + 32 * 2);
						Texts.back().size = y_r(32);
						Texts.back().str = "渡部チェル";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF = (LONGLONG)(1000000.f * 32.52);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(400 - 60);
						Texts.back().ypos = y_r(650);
						Texts.back().size = y_r(10);
						Texts.back().str = "使用モデル";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(400 - 60);
						Texts.back().ypos = y_r(650+10);
						Texts.back().size = y_r(24);
						Texts.back().str = "S.Aikawa氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(400 - 120);
						Texts.back().ypos = y_r(650 + 10 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "[MMD] アグネスタキオン (ウマ娘)";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						//
					}
					StartF += (LONGLONG)(1000000.f * 2.000f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 60);
						Texts.back().ypos = y_r(880);
						Texts.back().size = y_r(10);
						Texts.back().str = "使用モデル";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 60);
						Texts.back().ypos = y_r(880 + 10);
						Texts.back().size = y_r(24);
						Texts.back().str = "ShiniNet氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 120);
						Texts.back().ypos = y_r(880 + 10 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "マン〇ッタンカフェ";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						//
					}
					StartF += (LONGLONG)(1000000.f * 2.000f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 60);
						Texts.back().ypos = y_r(880);
						Texts.back().size = y_r(10);
						Texts.back().str = "使用モデル";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 60);
						Texts.back().ypos = y_r(880 + 10);
						Texts.back().size = y_r(24);
						Texts.back().str = "kachin氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 60);
						Texts.back().ypos = y_r(880 + 10 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "かれん";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF = (LONGLONG)(1000000.f * 38.986f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(400 - 60);
						Texts.back().ypos = y_r(640 + 42 - 12);
						Texts.back().size = y_r(12);
						Texts.back().str = "ShiniNet氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(400 - 120);
						Texts.back().ypos = y_r(640 + 42);
						Texts.back().size = y_r(24);
						Texts.back().str = "ゴー〇ドシップ";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 120);
						Texts.back().ypos = y_r(640 + 42);
						Texts.back().size = y_r(24);
						Texts.back().str = "メジ〇マoクイーン";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 2.000f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{//
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(400 - 60);
						Texts.back().ypos = y_r(540 + 42);
						Texts.back().size = y_r(24);
						Texts.back().str = "つかさ氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(400 - 120);
						Texts.back().ypos = y_r(540 + 42 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "つかさ式ていおー";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						//
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 60);
						Texts.back().ypos = y_r(540 + 42);
						Texts.back().size = y_r(24);
						Texts.back().str = "WARPSTAR氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 120);
						Texts.back().ypos = y_r(540 + 42 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "【ウマ娘】スペシャルウィーク";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 2.000f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 60);
						Texts.back().ypos = y_r(880);
						Texts.back().size = y_r(10);
						Texts.back().str = "トレーナーモデル";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 60);
						Texts.back().ypos = y_r(880 + 10);
						Texts.back().size = y_r(24);
						Texts.back().str = "モノゾフ氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 120);
						Texts.back().ypos = y_r(880 + 10 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "【モブモデル】モブ用親父セット【配布中】";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 2.000f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(400 - 60);
						Texts.back().ypos = y_r(540 + 42);
						Texts.back().size = y_r(24);
						Texts.back().str = "Jean氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(400 - 120);
						Texts.back().ypos = y_r(540 + 42 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "【MMDウマ娘】テイエムオペラオー【モデル配布】";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						//
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 60);
						Texts.back().ypos = y_r(540 + 42);
						Texts.back().size = y_r(24);
						Texts.back().str = "EndressStorm氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 120);
						Texts.back().ypos = y_r(540 + 42 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "メイショウドトウ";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF = (LONGLONG)(1000000.f * 47.200f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(400 - 60);
						Texts.back().ypos = y_r(540 + 42);
						Texts.back().size = y_r(24);
						Texts.back().str = "tomoyo氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(400 - 120);
						Texts.back().ypos = y_r(540 + 42 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "tmy式ダスカ　ver.0.92";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						//
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 60);
						Texts.back().ypos = y_r(540 + 42);
						Texts.back().size = y_r(24);
						Texts.back().str = "Jean氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1500 - 120);
						Texts.back().ypos = y_r(540 + 42 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "【MMDウマ娘】ウオッカ【モデル配布】";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF = (LONGLONG)(1000000.f * 50.447f);
					ContiF = (LONGLONG)(1000000.f * 1.800f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 60);
						Texts.back().ypos = y_r(880);
						Texts.back().size = y_r(10);
						Texts.back().str = "黒子モデル";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 60);
						Texts.back().ypos = y_r(880 + 10);
						Texts.back().size = y_r(24);
						Texts.back().str = "神々の宴氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 120);
						Texts.back().ypos = y_r(880 + 10 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "【ＭＭＤ】ウマ娘モブセット";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 2.000f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 60);
						Texts.back().ypos = y_r(880);
						Texts.back().size = y_r(10);
						Texts.back().str = "競馬場モデル";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 60);
						Texts.back().ypos = y_r(880 + 10);
						Texts.back().size = y_r(24);
						Texts.back().str = "Led/折鶴P氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 140);
						Texts.back().ypos = y_r(880 + 10 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "なんちゃって競馬場セット";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 2.000f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 60);
						Texts.back().ypos = y_r(880);
						Texts.back().size = y_r(10);
						Texts.back().str = "トレセン学園モデル(代演)";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 60);
						Texts.back().ypos = y_r(880 + 10);
						Texts.back().size = y_r(24);
						Texts.back().str = "じん氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 240);
						Texts.back().ypos = y_r(880 + 10 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "【MMDギアス】アッシュフォード学園【ステージ配布】";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 2.000f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 60);
						Texts.back().ypos = y_r(880);
						Texts.back().size = y_r(10);
						Texts.back().str = "クロフネ(代演)";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 60);
						Texts.back().ypos = y_r(880 + 10);
						Texts.back().size = y_r(24);
						Texts.back().str = "Tansoku102cm-沼地人氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 180);
						Texts.back().ypos = y_r(880 + 10 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "MMD用モブ前弩級戦艦1890セット";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 2.000f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 60);
						Texts.back().ypos = y_r(880);
						Texts.back().size = y_r(10);
						Texts.back().str = "客席";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 60);
						Texts.back().ypos = y_r(880 + 10);
						Texts.back().size = y_r(24);
						Texts.back().str = "ppp21氏";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;

						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(960 - 180);
						Texts.back().ypos = y_r(880 + 10 + 48);
						Texts.back().size = y_r(24);
						Texts.back().str = "【MMD】笹かまスタジアム";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF += (LONGLONG)(1000000.f * 2.000f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1440 - 60);
						Texts.back().ypos = y_r(720);
						Texts.back().size = y_r(32);
						Texts.back().str = "";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
					StartF = (LONGLONG)(1000000.f * 62.775f);
					ContiF = (LONGLONG)(1000000.f * 2.000f);
					{
						Texts.resize(Texts.size() + 1);
						Texts.back().xpos = y_r(1440 - 60);
						Texts.back().ypos = y_r(720);
						Texts.back().size = y_r(32);
						Texts.back().str = "";
						Texts.back().START_TIME = StartF;
						Texts.back().END_TIME = StartF + ContiF;
					}
				}
				SetUseASyncLoadFlag(FALSE);
				//
				BGM.play(DX_PLAYTYPE_BACK, TRUE);
				//BGM.vol(64);
				BaseTime = GetNowHiPerformanceCount() - (Cut > 0 ? Cut_Pic[Cut - 1].TIME : 0);
				SeekMovieToGraph(movie.get(), (int)(Cut > 0 ? Cut_Pic[Cut - 1].TIME : 0) / 1000);

				HostpassPTs->Set_Bright(226, 226, 216);
			}

			bool reset_p = true;
			bool isNextreset = true;
			float scarletOpac = 1.0f;
			float TakiOpac = 1.0f;
			float Taki2Opac = 1.0f;
			float CafeOpac = 1.0f;
			float radc = 0.f;
			float radc2 = 0.f;
			float xradadd_r = 0.f;
			float spd_takion = 0.6f;
			bool return_walk = false;
			float Gate_Xpos = 0;
			float Line_ALPHA = 0.f;
			float Box_ALPHA = 0.f;
			float Line_time = 0.f;
			float Thickness = 100.f;
			float camzb_28 = 0.f;
			float camyup = 0.f;
			float camzb_41 = 0.f;
			float bright_43 = 0.f;
			float ship_z = 0.f;
			float ship_zadd = 0.f;
			bool nex_cut = false;
			float cam_yure = 0.f;
			bool isfirst_14 = false;
			VECTOR_ref rand_vp;
			bool Update(void) noexcept override {
				float spd_taki = 1.f;
				float spd_cafe = 1.f;
				TEMPSCENE::Update();
				const auto NowTime = GetNowHiPerformanceCount() - BaseTime;
				{
					if (!Time_Over()) {
						if (Cut <= 1) {
							if (NowTime > (LONGLONG)(1000000.f * 0.450f)) {
								easing_set(&sode[0].xpos, 0.f, 0.9f);
								easing_set(&sode[0].ypos, 0.f, 0.9f);
								easing_set(&sode[1].xpos, 0.f, 0.875f);
								easing_set(&sode[1].ypos, 0.f, 0.875f);
								easing_set(&sode[2].xpos, 0.f, 0.85f);
								easing_set(&sode[2].ypos, 0.f, 0.85f);
								easing_set(&sode[3].xpos, 0.f, 0.825f);
								easing_set(&sode[3].ypos, 0.f, 0.825f);
								if (NowTime > (LONGLONG)(1000000.f * 1.421f)) {
									First[0].xpos = First[0].xpos_base;
									First[0].ypos = First[0].ypos_base;
									First2[0].xpos = First2[0].xpos_base;
									First2[0].ypos = First2[0].ypos_base;
									First3[0].xpos = First3[0].xpos_base;
									First3[0].ypos = First3[0].ypos_base;
									if (First3[0].Alpha < 0.f) {
										First3[0].Alpha = 1.f;
										First3[0].Scale = 1.f;
									}
								}
								if (NowTime > (LONGLONG)(1000000.f * 1.637f)) {
									First[1].xpos = First[1].xpos_base;
									First[1].ypos = First[1].ypos_base;
									First2[1].xpos = First2[1].xpos_base;
									First2[1].ypos = First2[1].ypos_base;
									First3[1].xpos = First3[1].xpos_base;
									First3[1].ypos = First3[1].ypos_base;
									if (First3[1].Alpha < 0.f) {
										First3[1].Alpha = 1.f;
										First3[1].Scale = 1.f;
									}
								}
								if (NowTime > (LONGLONG)(1000000.f * 1.820f)) {
									First[2].xpos = First[2].xpos_base;
									First[2].ypos = First[2].ypos_base;
									First2[2].xpos = First2[2].xpos_base;
									First2[2].ypos = First2[2].ypos_base;
									First3[2].xpos = First3[2].xpos_base;
									First3[2].ypos = First3[2].ypos_base;
									if (First3[2].Alpha < 0.f) {
										First3[2].Alpha = 1.f;
										First3[2].Scale = 1.f;
									}
								}
							}
							for (auto& sl : First3) {
								if (sl.Alpha > 0.f) {
									sl.Alpha -= 6.f / GetFPS();
									if (sl.Alpha <= 0.f) {
										sl.Alpha = 0.f;
									}
								}
								if (sl.Scale < 3.f) {
									sl.Scale += 6.f / GetFPS();
								}
							}
						}
						if (Cut == 2) {
							sode[0].xpos = (float)(DrawPts->disp_x / 5);
							sode[0].ypos = (float)(DrawPts->disp_y);
							sode[1].xpos = (float)(-DrawPts->disp_x / 5);
							sode[1].ypos = (float)(DrawPts->disp_y);
							sode[2].xpos = (float)(DrawPts->disp_x / 5);
							sode[2].ypos = (float)(DrawPts->disp_y);
							sode[3].xpos = (float)(-DrawPts->disp_x / 5);
							sode[3].ypos = (float)(DrawPts->disp_y);
							for (auto& sl : First) {
								sl.xpos = DrawPts->disp_x*2;
								sl.ypos = DrawPts->disp_y * 2;
							}
							for (auto& sl : First2) {
								sl.xpos = DrawPts->disp_x * 2;
								sl.ypos = DrawPts->disp_y * 2;
							}
						}

						if (Cut == 6) {
							sode[0].xpos = 0.f;
							sode[0].ypos = 0.f;
							sode[1].xpos = 0.f;
							sode[1].ypos = 0.f;
							sode[2].xpos = 0.f;
							sode[2].ypos = 0.f;
							sode[3].xpos = 0.f;
							sode[3].ypos = 0.f;
							Line_ALPHA = 1.f;
							if (Line_time < 1.f) {
								Line_time += 5.f / GetFPS();
							}
							else {
								Line_time = 1.5f;
							}
							Thickness = 5.f;
							for (auto& sl : First) {
								sl.xpos = sl.xpos_base;
								sl.ypos = sl.ypos_base;
							}
							for (auto& sl : First2) {
								sl.xpos = sl.xpos_base;
								sl.ypos = sl.ypos_base;
							}
						}
						if (Cut == 7 || Cut == 8) {
							Thickness += 6000.f / GetFPS();
							if (Thickness > 700) {
								if (Line_ALPHA > 2.f / 255.f) {
									easing_set(&Line_ALPHA, 0.f, 0.95f);
								}
								else {
									Line_ALPHA = 0.f;
								}
							}
						}
						if (Cut == 7) {
							easing_set(&sode[0].xpos, (float)(DrawPts->disp_x), 0.9f);
							easing_set(&sode[0].ypos, (float)(DrawPts->disp_y / 5), 0.9f);
							easing_set(&sode[1].xpos, (float)(-DrawPts->disp_x), 0.875f);
							easing_set(&sode[1].ypos, (float)(DrawPts->disp_y / 5), 0.875f);
							easing_set(&sode[2].xpos, (float)(DrawPts->disp_x), 0.85f);
							easing_set(&sode[2].ypos, (float)(DrawPts->disp_y / 5), 0.85f);
							easing_set(&sode[3].xpos, (float)(-DrawPts->disp_x), 0.825f);
							easing_set(&sode[3].ypos, (float)(DrawPts->disp_y / 5), 0.825f);
							for (auto& sl : First) {
								sl.xpos = DrawPts->disp_x * 2;
								sl.ypos = DrawPts->disp_y * 2;
							}
							for (auto& sl : First2) {
								sl.xpos = DrawPts->disp_x * 2;
								sl.ypos = DrawPts->disp_y * 2;
							}
						}
						if (Cut == 8) {
							sode[0].xpos = (float)(DrawPts->disp_x);
							sode[0].ypos = (float)(DrawPts->disp_y / 5);
							sode[1].xpos = (float)(-DrawPts->disp_x);
							sode[1].ypos = (float)(DrawPts->disp_y / 5);
							sode[2].xpos = (float)(DrawPts->disp_x);
							sode[2].ypos = (float)(DrawPts->disp_y / 5);
							sode[3].xpos = (float)(-DrawPts->disp_x);
							sode[3].ypos = (float)(DrawPts->disp_y / 5);
						}

						if (Cut < 2) {
							isNextreset = true;
						}
						else if (Cut < 7) {
							Sel_AnimNum(Takion, 0);
							Takion.get_anime(0).Update(false, 0.8f);
							if (Cut == 6) {
								isNextreset = true;
							}
						}
						else if (Cut < 9) {
							Takion.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.f, 0.f))*MATRIX_ref::RotY(deg2rad(180)));
							if (Takion.get_anime(0).per > 0) {
								Set_Effect(Effect::ef_reco, VECTOR_ref::vget(0.f, 0.f, 0.f), VECTOR_ref::front(), 3.f);
								Cut_Pic[Cut].Aim_camera.fov = deg2rad(25);
								cam_yure = 0.f;
							}
							else {
								SetScale_Effect(Effect::ef_reco, 3.f + cam_yure * 1.f / 3.f);

								Cut_Pic[Cut].Aim_camera.far_ = 80.f;
								Cut_Pic[Cut].Aim_camera.fov = deg2rad(45);
								Cut_Pic[Cut].cam_per = 0.975f;
								cam_yure += 5.f / GetFPS();

								easing_set(&Cut_Pic[Cut].Aim_camera.campos,
									Cut_Pic[Cut].Aim_camera.camvec
									+
									VECTOR_ref::vget(
										-cos(deg2rad(6.9))*sin(-deg2rad(180.f + 19.5f)),
										sin(deg2rad(6.9)),
										-cos(deg2rad(6.9))*cos(-deg2rad(180.f + 19.5f))
									)*(27.0f + cam_yure)
									+
									VECTOR_ref::vget(
										-cam_yure + (float)(GetRand(2 * (int)cam_yure * 10)) / 10.f,
										-cam_yure + (float)(GetRand(2 * (int)cam_yure * 10)) / 10.f,
										-cam_yure + (float)(GetRand(2 * (int)cam_yure * 10)) / 10.f),
									0.9f);
								easing_set(&Cut_Pic[Cut].Aim_camera.camup, VECTOR_ref::vget(-0.5f + (float)(GetRand((int)(2.f * 0.5f) * 10)) / 10.f + cam_yure/50.f, 1.f, 0.f).Norm(), 0.95f);
							}
							SetSpeed_Effect(Effect::ef_reco, 1.05f);
							Sel_AnimNum(Takion, 1);
							Takion.get_anime(1).Update(true, 1.f);

							Takion.get_anime(2).time = 30.f;
						}
						else if (Cut < 14) {
							if (Takion.get_anime(1).per > 0) {
								Takion.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.f, 0.f))*MATRIX_ref::RotY(deg2rad(0)));
								Stop_Effect(Effect::ef_reco);
							}
							Sel_AnimNum(Takion, 2);

							if (Cut <= 12) {
								if (Takion.get_anime(2).time == 30.f) {
									TakiOpac = 0.f;
									spd_takion = 0.6f;
								}
								easing_set(&TakiOpac, 1.f, 0.95f);
								MV1SetOpacityRate(Takion.get(), TakiOpac);

								if (Takion.get_anime(2).time <= 32.f) {
									Takion.get_anime(2).Update(true, spd_takion);
								}
								else {
									easing_set(&spd_takion, 0.01f, 0.9f);
									TakiOpac = 1.f;
									MV1SetOpacityRate(Takion.get(), TakiOpac);
									Takion.get_anime(2).Update(true, spd_takion);
								}
								spd_taki = 0.01f;
							}
							else {
								if (NowTime > (LONGLONG)(1000000.f * 8.864f)) {
									Takion.get_anime(2).Update(true, 0.6f);
									Cut_Pic[Cut].Aim_camera.camvec = Takion.frame(6);
									Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(17.178366f, 4.319146f, -36.016518f);
									Cut_Pic[Cut].cam_per = 0.9f;
									spd_taki = 0.02f;
								}
								else {
									Takion.get_anime(2).Update(true, 0.01f);
									spd_taki = 0.01f;
								}
							}

							Takion.get_anime(3).time = 33.f;
							if (Cut == 13) {
								isNextreset = true;
							}
						}
						else if (Cut < 20) {

							if (Cut >= 15 && Cut <= 16) {
								//Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(116.485695f, 18.346067f, -56.928761f);

								Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(10.122698f, 18.028077f, -100.985756f);

								float xradadd = -2.f + float(GetRand(2 * 2 * 100)) / 100.f;
								easing_set(&xradadd_r, xradadd, 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
									VECTOR_ref::vget(
										-cos(deg2rad(-5.5f + xradadd_r * 1.5f))*sin(-deg2rad(float(90 - 10) + radc2)),
										sin(deg2rad(-5.5 + xradadd_r * 1.5f)),
										-cos(deg2rad(-5.5 + xradadd_r * 1.5f))*cos(-deg2rad(float(90 - 10) + radc2))
									)*88.0f;

								Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
								Cut_Pic[Cut].Aim_camera.fov = deg2rad(45);
								Cut_Pic[Cut].Aim_camera.far_ = 3000.f;
								Cut_Pic[Cut].cam_per = 0.f;
								radc2 += 12.5f / GetFPS();
							}


							if (Cut < 15) {
								Sel_AnimNum(Takion, 3);
								Takion.get_anime(3).Update(true, 0.6f);
								Cut_Pic[Cut].Aim_camera.camvec = Takion.frame(6);
								Cut_Pic[Cut].cam_per = 0.f;

								Takion2.get_anime(2).time = 33.f;
								spd_taki = 0.6f;
								if (Cut == 14) {
									isNextreset = true;
								}
							}
							else if (Cut < 16) {
								if (Takion2.get_anime(2).time == 33.f) {
									Takion2.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
									draw_t = false;
									draw_t2 = true;
								}

								Sel_AnimNum(Takion2, 2);
								Takion2.get_anime(2).Update(true, 0.45f);
								spd_taki = 0.45f;

								Takion.get_anime(3).time = 33.f;
								Cafe.get_anime(0).time = 33.f;
								if (Cut == 15) {
									isNextreset = true;
								}

								TakiOpac = 0.f;
								CafeOpac = 0.f;
								isfirst_14 = false;
							}
							else if (Cut < 20) {
								draw_t = true;
								draw_cafe = true;
								if (Takion.get_anime(3).time == 33.f) {
									Takion.SetMatrix(MATRIX_ref::RotY(deg2rad(-90 + 0))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -140)));
									Cafe.SetMatrix(MATRIX_ref::RotY(deg2rad(-90 + 180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -140)));
								}

								MV1SetOpacityRate(Takion2.get(), Taki2Opac);
								if (Taki2Opac > 0.f) {
									Taki2Opac -= 2.0f / GetFPS();
								}
								else {
									Taki2Opac = 0.0f;
								}
								MV1SetOpacityRate(Takion.get(), TakiOpac);
								if (TakiOpac < 1.f) {
									TakiOpac += 2.0f / GetFPS();
								}
								else {
									TakiOpac = 1.0f;
								}
								MV1SetOpacityRate(Cafe.get(), CafeOpac);
								if (CafeOpac < 1.f) {
									CafeOpac += 2.0f / GetFPS();
								}
								else {
									CafeOpac = 1.0f;
								}


								Takion2.get_anime(2).Update(true, 0.45f);



								Sel_AnimNum(Takion, 3);
								Takion.get_anime(3).Update(true, 0.45f);
								spd_taki = 0.45f;

								Sel_AnimNum(Cafe, 0);
								Cafe.get_anime(0).Update(true, 0.45f);
								spd_cafe = 0.45f;

								if (Cut == 17) {
									easing_set(&Cut_Pic[Cut].Aim_camera.camvec,
										Takion.frame(9) + VECTOR_ref::vget(0.f, -5.5f + (float)(GetRand((int)(2.f * 5.5f) * 10)) / 10.f, -13.5f + (float)(GetRand((int)(2.f * 13.5f) * 10)) / 10.f),
										0.975f);
									if (!isfirst_14) {
										Cut_Pic[Cut].Aim_camera.camvec = Takion.frame(9);
									}
									float xradadd = -2.f + float(GetRand(2 * 2 * 100)) / 100.f;
									easing_set(&xradadd_r, xradadd, 0.975f);
									Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
										VECTOR_ref::vget(
											-cos(deg2rad(-8.5f + xradadd_r * 1.5f))*sin(-deg2rad(float(90 - 10) + radc)),
											sin(deg2rad(-8.5 + xradadd_r * 1.5f)),
											-cos(deg2rad(-8.5 + xradadd_r * 1.5f))*cos(-deg2rad(float(90 - 10) + radc))
										)*38.0f;
									Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
									if (!isfirst_14) {
										Cut_Pic[Cut].cam_per = 0.f;
										isfirst_14 = true;
									}
									else {
										Cut_Pic[Cut].cam_per = 0.975f;
									}
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(15);
									radc += 12.5f / GetFPS();
								}
								else if (Cut == 18) {
									if (isfirst_14) {
										Cut_Pic[Cut].Aim_camera.camvec = Cafe.frame(8);
										Cut_Pic[Cut].cam_per = 0.f;
										isfirst_14 = false;
									}
									else {
										easing_set(&Cut_Pic[Cut].Aim_camera.camvec,
											Cafe.frame(8) + VECTOR_ref::vget(0.f, -5.5f + (float)(GetRand((int)(2.f * 5.5f) * 10)) / 10.f, -13.5f + (float)(GetRand((int)(2.f * 13.5f) * 10)) / 10.f),
											0.975f);
										Cut_Pic[Cut].cam_per = 0.975f;
									}
									float xradadd = -2.f + float(GetRand(2 * 2 * 100)) / 100.f;
									easing_set(&xradadd_r, xradadd, 0.975f);
									Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
										VECTOR_ref::vget(
											-cos(deg2rad(-5.5f + xradadd_r * 1.5f))*sin(-deg2rad(float(90 + 180 - 10) + radc)),
											sin(deg2rad(-5.5f + xradadd_r * 1.5f)),
											-cos(deg2rad(-5.5f + xradadd_r * 1.5f))*cos(-deg2rad(float(90 + 180 - 10) + radc))
										)*34.0f;
									Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(15);
									radc += 12.5f / GetFPS();
								}
								else if (Cut == 19) {
									Cut_Pic[Cut].Aim_camera.camvec = Takion.frame(133);
									float xradadd = -2.f + float(GetRand(2 * 2 * 100)) / 100.f;
									easing_set(&xradadd_r, xradadd, 0.975f);
									Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
										VECTOR_ref::vget(
											-cos(deg2rad(25.5f + xradadd_r * 1.5f))*sin(-deg2rad(float(90 - 10) + radc)),
											sin(deg2rad(25.5 + xradadd_r * 1.5f)),
											-cos(deg2rad(25.5 + xradadd_r * 1.5f))*cos(-deg2rad(float(90 - 10) + radc))
										)*38.0f;
									Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
									Cut_Pic[Cut].cam_per = 0.f;
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(15);
									radc += 12.5f / GetFPS();
								}
							}

							for (auto&n : Newspaper) {
								n.move.mat = MATRIX_ref::RotY(deg2rad(GetRand(360)))*MATRIX_ref::RotX(deg2rad(-30 + GetRand(2 * 30)));
								n.move.pos = VECTOR_ref::vget((float)(-20 + GetRand(2 * 20)), (float)(7 + GetRand(10)), (float)(-20 + GetRand(2 * 20)));
								n.move.vec.y(-(float)(GetRand(100)) / 1000.f);
							}
							isNextreset = true;
						}
						else if (Cut < 21) {
							draw_t2 = false;
							if (Cafe.get_anime(0).time != 0.f) {
								Cafe.get_anime(0).time = 0.f;
								draw_cafe = false;
							}
							for (auto&n : Newspaper) {
								if (n.move.pos.y() >= 3.5f) {
									n.move.vec.yadd(M_GR / powf((FPS / 0.75f), 2.f));
								}
								else {
									float yp = n.move.vec.y();
									easing_set(&yp, 0.f, 0.995f);
									n.move.vec.y(yp);
								}

								n.move.pos += n.move.vec;
								n.model.SetMatrix(n.move.mat*MATRIX_ref::Mtrans(n.move.pos));
							}

							for (auto& n : news_p) {
								n.xns_pos = DrawPts->disp_x / 2 - DrawPts->disp_x;
								n.yns_pos = DrawPts->disp_y / 2 - DrawPts->disp_y;
								n.rns_rad = deg2rad(-30);
							}
						}
						else if (Cut < 22) {
							for (auto n = news_p.rbegin(); n != news_p.rend() - 1; n++) {
								n->xns_pos = (n + 1)->xns_pos;
								n->yns_pos = (n + 1)->yns_pos;
								n->rns_rad = (n + 1)->rns_rad;
							}
							news_p[0].xns_pos += (int)((float)(DrawPts->disp_x / 2)*4.f / GetFPS());
							news_p[0].yns_pos += (int)((float)(DrawPts->disp_y / 2)*4.f / GetFPS());
							news_p[0].rns_rad += deg2rad(6) / GetFPS();

							Sel_AnimNum(Takion3_1, 4);
							Takion3_1.SetMatrix(MATRIX_ref::RotY(deg2rad(-6))*MATRIX_ref::Mtrans(VECTOR_ref::vget(5, 1, -10)));
							Sel_AnimNum(Takion3_2, 5);
							Takion3_2.SetMatrix(MATRIX_ref::RotY(deg2rad(5))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-3, 0, 10)));
							Sel_AnimNum(Takion3_3, 6);
							Takion3_3.SetMatrix(MATRIX_ref::RotY(deg2rad(-25))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-18, 0, 30)));
							Sel_AnimNum(Takion3_4, 7);
							Takion3_4.SetMatrix(MATRIX_ref::RotY(deg2rad(0))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-30, 0.5, 40)));
						}
						else if (Cut < 24) {
							for (auto n = news_p.rbegin(); n != news_p.rend() - 1; n++) {
								n->xns_pos = (n + 1)->xns_pos;
								n->yns_pos = (n + 1)->yns_pos;
								n->rns_rad = (n + 1)->rns_rad;
							}
							news_p[0].xns_pos += (int)((float)(DrawPts->disp_x / 2)*4.f / GetFPS());
							news_p[0].yns_pos += (int)((float)(DrawPts->disp_y / 2)*4.f / GetFPS());
							news_p[0].rns_rad += deg2rad(6) / GetFPS();

							Takion3_1.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
							Takion3_2.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
							Takion3_3.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
							Takion3_4.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));

							if (news_p[0].yns_pos < DrawPts->disp_y / 2) {
								Sel_AnimNum(goldship, 0);
							}
							else {
								Sel_AnimNum(goldship, 1);
								goldship.get_anime(1).Update(true, 0.45f);
							}
							goldship.SetMatrix(MATRIX_ref::RotY(deg2rad(-6))*MATRIX_ref::Mtrans(VECTOR_ref::vget(5, -2, -10)));
							Sel_AnimNum(speweek, 0);
							speweek.SetMatrix(MATRIX_ref::RotY(deg2rad(5))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-3, 0, 10)));
							Sel_AnimNum(teio, 0);
							teio.SetMatrix(MATRIX_ref::RotY(deg2rad(-25))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-18, 0, 30)));
							Sel_AnimNum(macin, 0);
							macin.SetMatrix(MATRIX_ref::RotY(deg2rad(0))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-30, 0.5, 40)));
						}
						else if (Cut < 25) {
							goldship.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
							speweek.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
							teio.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
							macin.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, -100, 0)));
							Takion.get_anime(8).time = 33.f;
							isNextreset = true;
							return_walk = false;
						}
						else if (Cut < 26) {
							if (Takion.get_anime(8).time == 33.f) {
								Takion.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
								draw_t2 = false;
								Taki2Opac = 1.f;
								MV1SetOpacityRate(Takion2.get(), Taki2Opac);
							}
							Sel_AnimNum(Takion, 8);
							if (Takion.get_anime(8).time == Takion.get_anime(8).alltime) {
								if (!return_walk) {
									draw_t2 = true;
									Takion2.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
									Sel_AnimNum(Takion2, 9);
									Takion2.work_anime();
									Takion2.PhysicsResetState();
								}
								return_walk = true;
							}
							Takion.get_anime(8).Update(false, return_walk ? -4.f : 4.f);
							if (return_walk) {
								Takion2.get_anime(9).Update(false, 0.7f);
								Cut_Pic[Cut].Aim_camera.camvec = Takion2.frame(9);
								Cut_Pic[Cut].cam_per = 0.95f;
								Cut_Pic[Cut].Aim_camera.near_ = 5.f;
								Cut_Pic[Cut].Aim_camera.fov = deg2rad(7);
							}
							else {
								spd_taki = 4.f;
							}
							radc2 = 0.f;
							isNextreset = true;
						}
						else if (Cut < 27) {
							{
								draw_t = false;
								draw_t2= false;
								return_walk = true;

								//Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(116.485695f, 18.346067f, -56.928761f);

								Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(10.122698f, 18.028077f, -100.985756f);

								float xradadd = -2.f + float(GetRand(2 * 2 * 100)) / 100.f;
								easing_set(&xradadd_r, xradadd, 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
									VECTOR_ref::vget(
										-cos(deg2rad(-5.5f + xradadd_r * 1.5f))*sin(-deg2rad(float(90 - 10) + radc2)),
										sin(deg2rad(-5.5 + xradadd_r * 1.5f)),
										-cos(deg2rad(-5.5 + xradadd_r * 1.5f))*cos(-deg2rad(float(90 - 10) + radc2))
									)*88.0f;

								Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
								Cut_Pic[Cut].Aim_camera.fov = deg2rad(45);
								Cut_Pic[Cut].Aim_camera.far_ = 3000.f;
								Cut_Pic[Cut].cam_per = 0.f;
								radc2 += 12.5f / GetFPS();
							}
							Gate_Xpos = -20.f;
							camzb_28 = 1.f;
						}
						else if (Cut < 28) {
							Gate.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(Gate_Xpos, 0.25f, 0.f))*MATRIX_ref::RotY(deg2rad(0)));
							if (Gate_Xpos < 20.f) {
								Gate_Xpos += 20.f *1.f / GetFPS();
							}
							isNextreset = true;
							karen.get_anime(0).time = 6.25f;
							if (camzb_28 == 1.f) {
								Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(10.122698f, 10.028077f, -50.985756f);
								Cut_Pic[Cut].cam_per = 0.f;
							}
							else {
								float pp = 7.f;
								easing_set(&rand_vp, VECTOR_ref::vget(-pp + float(GetRand((int)pp * 2 * 100)) / 100.f, -pp + float(GetRand((int)pp * 2 * 100)) / 100.f, -pp + float(GetRand((int)pp * 2 * 100)) / 100.f), 0.9f);
								Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(10.122698f, 10.028077f, -50.985756f) + rand_vp;
								Cut_Pic[Cut].cam_per = 0.95f;
							}
							camzb_28 = 0.f;
							ship_z = -5.f;
							ship_zadd = 0.f;
						}
						else if (Cut < 30) {
							draw_kr = true;
							Sel_AnimNum(karen, 0);
							karen.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(2, 0, 0.f)));
							karen.get_anime(0).Update(false, 1.0f);

							Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(0, 0, camzb_28);
							Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
								VECTOR_ref::vget(
									2.f,
									12.f,
									6.f
								)*1.0f;

							if (camzb_28 != 0.f) {
								if (camzb_28 > -8.f) {
									Cut_Pic[Cut].cam_per = 0.9f;
								}
								else {
									Cut_Pic[Cut].cam_per = 0.95f;
								}
							}
							if (camzb_28 > -10.f) {
								camzb_28 -= 100.f / GetFPS();
							}
							else {
								camzb_28 = -10.f;
							}

							MV1SetOpacityRate(Ship.get(), 0.3f);
							Ship.SetScale(VECTOR_ref::vget(1.f, 1.f, 1.f)*0.01f);
							Ship.SetPosition(camera_main.camvec + VECTOR_ref::up()*3.f + VECTOR_ref::right()*-2.f + VECTOR_ref::front()*-1.f*ship_z);
							ship_z += ship_zadd / GetFPS();
							if (ship_zadd <= 0.95f) {
								easing_set(&ship_zadd, 1.f, 0.95f);
							}
							else {
								ship_zadd = 1.f;
							}
							if (Cut == 29) {
								isNextreset = true;
							}
						}
						else if (Cut < 31) {
							draw_t = false;
							draw_kr = false;
							draw_cafe = true;
							Cafe.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
							Sel_AnimNum(Cafe, 0);
							Cafe.get_anime(0).Update(true, 0.f);
						}
						else if (Cut < 32) {
							draw_t = true;
							Sel_AnimNum(Cafe, 1);
							Cafe.get_anime(1).Update(true, 0.f);
							isNextreset = true;
							scarletOpac = 0.5f;
						}
						else if (Cut < 33) {
							draw_t = true;
							draw_cafe = false;
							draw_scar = true;
							scarlet.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
							Sel_AnimNum(scarlet, 0);
							scarlet.get_anime(0).Update(false, 0.6f);
							if (scarlet.get_anime(0).time > 10.f) {
								easing_set(&scarletOpac, 1.f, 0.95f);
							}
							MV1SetOpacityRate(scarlet.get(), scarletOpac);

							Takion.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -14)));
							Sel_AnimNum(Takion, 10);
							Takion.get_anime(10).Update(false, 0.6f);
							isNextreset = true;
						}
						else if (Cut < 34) {
							draw_vodka = true;
							vodka.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -50)));
							Sel_AnimNum(vodka, 0);
							vodka.get_anime(0).Update(false, 1.f);
						}
						else if (Cut < 35) {
							draw_vodka = false;
							isNextreset = true;
						}
						else if (Cut < 36) {
							draw_scar = false;
							draw_t = false;
							draw_cafe = true;
							Sel_AnimNum(Cafe, 2);
							Cafe.get_anime(2).Update(false, 0.6f);
							CafeOpac = 1.f;
							/*
							Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(0, 17, -4.5);
							Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec +
								VECTOR_ref::vget(
									-10.f,
									0.f,
									0.f
								)*1.0f;

							Cut_Pic.back().Aim_camera.camup = VECTOR_ref::up();
							Cut_Pic.back().Aim_camera.fov = deg2rad(15);
							Cut_Pic.back().Aim_camera.near_ = 1.f;
							Cut_Pic.back().Aim_camera.far_ = 100.f;
							Cut_Pic.back().cam_per = 0.f;
							*/
							isNextreset = true;
						}
						else if (Cut < 37) {//手
							Sel_AnimNum(Cafe, 2);
							Cafe.get_anime(2).Update(false, 0.6f);
							MV1SetOpacityRate(Cafe.get(), CafeOpac);
							if (CafeOpac > 0.f) {
								CafeOpac -= 2.0f / GetFPS();
							}
							else {
								CafeOpac = 0.f;
							}
							draw_t = true;

							Takion.SetMatrix(MATRIX_ref::RotY(deg2rad(270))*MATRIX_ref::Mtrans(VECTOR_ref::vget(5.f, 1.6f, -10.f)));
							Sel_AnimNum(Takion, 11);
							Takion.get_anime(11).Update(false, 0.5f);
							MV1SetOpacityRate(Takion.get(), TakiOpac);
							if (TakiOpac < 1.f) {
								TakiOpac += 2.0f / GetFPS();
							}
							else {
								TakiOpac = 1.0f;
							}

							//Cut_Pic[Cut].Aim_camera.fov = deg2rad(95);
						}
						else if (Cut < 38) {//光るトレーナー
							draw_t = false;
							draw_cafe = false;
							draw_tnr = true;
							Sel_AnimNum(trainer, 0);
							trainer.get_anime(0).Update(false, 1.2f);
						}
						else if (Cut < 39) {
							draw_tnr = false;
							draw_t = true;
							Takion.SetMatrix(MATRIX_ref::RotY(deg2rad(270))*MATRIX_ref::Mtrans(VECTOR_ref::vget(5.f, 1.6f, -10.f)));
							Sel_AnimNum(Takion, 12);
							Takion.get_anime(12).Update(false, 0.5f);
							isNextreset = true;
						}
						else if (Cut < 40) {
							Takion.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
							Sel_AnimNum(Takion, 13);
							Takion.get_anime(13).Update(false, 1.2f);
							isNextreset = true;
						}
						else if (Cut < 41) {
							Takion.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
							Sel_AnimNum(Takion, 14);
							Takion.get_anime(14).Update(false, 2.8f);
							isNextreset = true;
							camzb_28 = 0.f;
							camzb_41 = 0.1f;
							for (auto& m : mobu_b) {
								int cnt = (int)(&m - &mobu_b.front());
								Sel_AnimNum(m, mobu_b_anim[cnt]);
								m.get_anime(mobu_b_anim[cnt]).Update(false, 0.f);
							}
						}
						else if (Cut < 42) {
							Gate_Xpos = 0.f;
							Gate.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(Gate_Xpos, 0.25f, 0.f))*MATRIX_ref::RotY(deg2rad(0)));
							Sel_AnimNum(Gate, 0);
							Gate.get_anime(0).Update(false, 0.f);

							Gate_Xpos += -7.f;
							for (auto& m : mobu_b) {
								m.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(Gate_Xpos, 0.25f, -10.f))*MATRIX_ref::RotY(deg2rad(0)));
								Gate_Xpos += -14.f;
							}

							draw_t = false;
							Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(-60.122698f, 16.028077f, -50.985756f);

							float pp = 10.f;
							if (camzb_28 < -9.5f) {
								pp = 2.f;
							}
							if (camzb_28 <= -10.f) {
								pp = 0.f;
							}
							rand_vp = VECTOR_ref::vget(-pp + float(GetRand((int)pp * 2 * 100)) / 100.f, -pp + float(GetRand((int)pp * 2 * 100)) / 100.f, -pp + float(GetRand((int)pp * 2 * 100)) / 100.f);

							Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
								VECTOR_ref::vget(
									-cos(deg2rad(0.f))*sin(-deg2rad(float(0 + 30))),
									sin(deg2rad(0.f)),
									-cos(deg2rad(0.f))*cos(-deg2rad(float(0 + 30)))
								)*(20.f - camzb_28 * 20.8f) +
								rand_vp;

							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(camzb_41, 0.9f, 0);
							easing_set(&camzb_41, 0.3f, 0.995f);
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(45);
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;
							Cut_Pic[Cut].cam_per = 0.f;
							if (camzb_28 != 0.f) {
								if (camzb_28 > -8.f) {
									Cut_Pic[Cut].cam_per = 0.9f;
								}
								else {
									Cut_Pic[Cut].cam_per = 0.95f;
								}
							}
							if (camzb_28 > -10.f) {
								camzb_28 -= 100.f / GetFPS();
							}
							else {
								camzb_28 = -10.f;
							}
						}
						else if (Cut < 43) {
							Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(-60.122698f, 16.028077f, -50.985756f);

							float pp = 10.f;
							if (camzb_28 > -5.f) {
								pp = 2.f;
							}
							rand_vp = VECTOR_ref::vget(-pp + float(GetRand((int)pp * 2 * 100)) / 100.f, -pp + float(GetRand((int)pp * 2 * 100)) / 100.f, -pp + float(GetRand((int)pp * 2 * 100)) / 100.f);

							Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
								VECTOR_ref::vget(
									-cos(deg2rad(0.f))*sin(-deg2rad(float(0 + 30))),
									sin(deg2rad(0.f)),
									-cos(deg2rad(0.f))*cos(-deg2rad(float(0 + 30)))
								)*(10.f - camzb_28 * 20.8f) +
								rand_vp;
							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(camzb_41, 0.9f, 0);
							easing_set(&camzb_41, 0.3f, 0.995f);
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(45);
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;
							Cut_Pic[Cut].cam_per = 0.95f;
							HostpassPTs->Set_Bright(
								226 + (int)((float)(255 - 226)*bright_43),
								226 + (int)((float)(255 - 226)*bright_43),
								196 + (int)((float)(255 - 196)*bright_43)
							);
							easing_set(&bright_43, 1.f, 0.9f);
							if (camzb_28 < 0.f) {
								camzb_28 += 70.f / GetFPS();
							}
							else {
								camzb_28 = 0.f;
							}
						}
						else if (Cut < 44) {
							Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(-120.f, 37.f, -20.f);
							Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
								VECTOR_ref::vget(
									-cos(deg2rad(-20.f))*sin(-deg2rad(float(0 - 10) + camzb_28)),
									sin(deg2rad(-20.f)),
									-cos(deg2rad(-20.f))*cos(-deg2rad(float(0 - 10) + camzb_28))
								)*(20.f);
							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(0, 1.f, 0);
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(25);
							Cut_Pic[Cut].Aim_camera.far_ = 300.f;

							Cut_Pic[Cut].cam_per = 0.f;

							camzb_28 += 40.f / GetFPS();
							camyup = 0.f;
						}
						else if (Cut < 45) {
							if (NowTime > (LONGLONG)(1000000.f * 49.561)) {
								Sel_AnimNum(Gate, 1);
								Gate.get_anime(1).Update(false, 1.f);

								Gate_Xpos = 0.f;
								Gate_Xpos += -7.f;
								for (auto& m : mobu_b) {
									int cnt = (int)(&m - &mobu_b.front());
									easing_set(&m.get_anime(mobu_b_anim[cnt]).per, 0.f, mobu_b_run[cnt]);
									easing_set(&m.get_anime(3).per, 1.f, mobu_b_run[cnt]);

									m.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(Gate_Xpos, 0.25f, -10.f - mobu_b_runrange[cnt] * m.get_anime(3).per))*MATRIX_ref::RotY(deg2rad(0)));
									Gate_Xpos += -14.f;
								}
							}
							if (NowTime > (LONGLONG)(1000000.f * 49.681)) {
								Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(-40.67f + 1.f, 5.53f + -0.05f, -52.f + 0.2f);
								Cut_Pic[Cut].Aim_camera.camvec = Cut_Pic[Cut].Aim_camera.campos +
									VECTOR_ref::vget(
										1,
										-0.3f,
										0.6f
									)*(-1.f);

								Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(0, 1.f, 0);
								Cut_Pic[Cut].Aim_camera.fov = deg2rad(35);
								Cut_Pic[Cut].Aim_camera.far_ = 3000.f;

								Cut_Pic[Cut].cam_per = 0.9f;
							}
							else {
								Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(-40.67f, 5.53f, -42.f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
									VECTOR_ref::vget(
										1,
										-0.1f-0.1f*camyup,
										-0.13f
									)*(1.f);
								camyup += 1.f / GetFPS();
								Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(0, 1.f, 0);
								Cut_Pic[Cut].Aim_camera.fov = deg2rad(35);
								Cut_Pic[Cut].Aim_camera.far_ = 3000.f;

								Cut_Pic[Cut].cam_per = 0.f;
							}

						}
						else if (Cut < 46) {
							Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(32.211510f, 14.997231f, -18.595667f);
							Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(-0.828184f, 4.546323f, -0.092557f);
							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(0, 1.f, 0);
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(35);
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;

							Cut_Pic[Cut].cam_per = 0.f;

							if (anim[0].ypos != 0.f) {
								VECTOR_ref vec_x = (Cut_Pic[Cut].Aim_camera.camvec - Cut_Pic[Cut].Aim_camera.campos).Norm().cross(Cut_Pic[Cut].Aim_camera.camup);

								Set_Effect(Effect::ef_greexp, VECTOR_ref::vget(-4.f, 4.6f, -2.f) + vec_x * 13.f, VECTOR_ref::front(), 5.f);
								Set_Effect(Effect::ef_greexp2, VECTOR_ref::vget(8.f, 5.5f, 4.f) - vec_x * 8.f, VECTOR_ref::front(), 4.f);
							}
							SetSpeed_Effect(Effect::ef_greexp, 0.5f);
							SetSpeed_Effect(Effect::ef_greexp2, 0.5f);
							anim[0].ypos = 0.f;
						}
						else if (Cut < 47) {
							Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(32.211510f, 14.997231f, -18.595667f);
							Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(-0.828184f, 4.546323f, -0.092557f);
							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(0, 1.f, 0);
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(35);
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;

							Cut_Pic[Cut].cam_per = 0.f;

							camzb_41 = (float)(DrawPts->disp_y)*0.4f;
							sode[4].xpos += (float)(y_r(300)) / GetFPS();
							sode[4].ypos -= (float)(y_r(2800)) / GetFPS();
							sode[5].xpos -= (float)(y_r(300)) / GetFPS();
							sode[5].ypos -= (float)(y_r(2400)) / GetFPS();
							sode[6].xpos += (float)(y_r(300)) / GetFPS();
							sode[6].ypos -= (float)(y_r(2200)) / GetFPS();
							sode[7].xpos -= (float)(y_r(300)) / GetFPS();
							sode[7].ypos -= (float)(y_r(2000)) / GetFPS();
						}
						else if (Cut < 48) {
							Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(48.211510f, 15.f - 1.5f*anim[1].ypos / (float)(DrawPts->disp_y), 0.595667f);
							Cut_Pic[Cut].Aim_camera.camvec = Cut_Pic[Cut].Aim_camera.campos +
								VECTOR_ref::vget(
									-1.f,
									0.3f,
									0.f
								)*(1.f);
							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(0, 1.f, 0);
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(35);
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;

							Cut_Pic[Cut].cam_per = 0.f;
							anim[0].ypos = (float)(DrawPts->disp_y);
							easing_set(&anim[1].ypos, camzb_41, 0.7f);
							if (camzb_41 == (float)(DrawPts->disp_y)*0.4f) {
								SetSpeed_Effect(Effect::ef_greexp, 0.1f);
								SetSpeed_Effect(Effect::ef_greexp2, 0.1f);
							}
							if (camzb_41 > 0.f) {
								camzb_41 -= (float)(y_r(500)) / GetFPS();
							}
							sode[4].xpos += (float)(y_r(300)) / GetFPS();
							sode[4].ypos -= (float)(y_r(2800)) / GetFPS();
							sode[5].xpos -= (float)(y_r(300)) / GetFPS();
							sode[5].ypos -= (float)(y_r(2400)) / GetFPS();
							sode[6].xpos += (float)(y_r(300)) / GetFPS();
							sode[6].ypos -= (float)(y_r(2200)) / GetFPS();
							sode[7].xpos -= (float)(y_r(300)) / GetFPS();
							sode[7].ypos -= (float)(y_r(2000)) / GetFPS();
						}
						else if (Cut < 49) {
							Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(48.211510f, 15.f - 1.5f*anim[1].ypos / (float)(DrawPts->disp_y), 0.595667f);
							Cut_Pic[Cut].Aim_camera.camvec = Cut_Pic[Cut].Aim_camera.campos +
								VECTOR_ref::vget(
									-1.f,
									0.3f,
									0.f
								)*(1.f);
							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(0, 1.f, 0);
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(35);
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;

							Cut_Pic[Cut].cam_per = 0.f;
							if (anim[0].ypos == (float)(DrawPts->disp_y)) {

								sode[4].xpos = (float)(0);
								sode[4].ypos = (float)(-DrawPts->disp_y);
								sode[5].xpos = (float)(0);
								sode[5].ypos = (float)(-DrawPts->disp_y * 5 / 4);
								sode[6].xpos = (float)(0);
								sode[6].ypos = (float)(-DrawPts->disp_y * 6 / 4);
								sode[7].xpos = (float)(0);
								sode[7].ypos = (float)(-DrawPts->disp_y * 7 / 4);
							}
							anim[0].ypos = (float)(DrawPts->disp_y) + 2;
							easing_set(&anim[1].ypos, camzb_41, 0.7f);
							if (camzb_41 > 0.f) {
								camzb_41 -= (float)(y_r(500)) / GetFPS();
							}
							sode[4].xpos -= (float)(y_r(300)) / GetFPS();
							sode[4].ypos += (float)(y_r(2800)) / GetFPS();
							sode[5].xpos += (float)(y_r(300)) / GetFPS();
							sode[5].ypos += (float)(y_r(2600)) / GetFPS();
							sode[6].xpos -= (float)(y_r(300)) / GetFPS();
							sode[6].ypos += (float)(y_r(2400)) / GetFPS();
							sode[7].xpos += (float)(y_r(300)) / GetFPS();
							sode[7].ypos += (float)(y_r(2000)) / GetFPS();
							isNextreset = true;
							Takion.get_anime(2).time = 35.f;
						}
						else if (Cut < 50) {

							VECTOR_ref aim = Takion.frame(2);
							aim.y(0.f);
							Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(32.211510f, 8.997231f, -18.595667f) + aim;
							Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(-0.828184f, 16.546323f, -0.092557f) + aim;
							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(0, 1.f, 0);
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(35);
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;


							Cut_Pic[Cut].cam_per = 0.f;
							anim[1].ypos = (float)(DrawPts->disp_y);
							sode[4].xpos -= (float)(y_r(300)) / GetFPS();
							sode[4].ypos += (float)(y_r(2800)) / GetFPS();
							sode[5].xpos += (float)(y_r(300)) / GetFPS();
							sode[5].ypos += (float)(y_r(2600)) / GetFPS();
							sode[6].xpos -= (float)(y_r(300)) / GetFPS();
							sode[6].ypos += (float)(y_r(2400)) / GetFPS();
							sode[7].xpos += (float)(y_r(300)) / GetFPS();
							sode[7].ypos += (float)(y_r(2000)) / GetFPS();

							draw_t = true;
							Takion.SetMatrix(MATRIX_ref::RotY(deg2rad(0))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-10, 0, 30.f)));
							Sel_AnimNum(Takion, 2);
							Takion.get_anime(2).Update(false, 0.1f);
							isNextreset = true;
							draw_kr = false;
						}
						else if (Cut < 51) {
							Stop_Effect(Effect::ef_greexp);
							Stop_Effect(Effect::ef_greexp2);
							draw_t = false;

							Sel_AnimNum(karen, 1);
							karen.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(2, 0, 0.f)));
							karen.get_anime(1).Update(false, 0.95f);
							if (!draw_kr) {
								Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(0.3f, 18.f, -28.595667f);

								Cut_Pic[Cut].Aim_camera.camvec = Cut_Pic[Cut].Aim_camera.campos + VECTOR_ref::vget(-0.3f, 0.f, 28.f);

								Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(0, 1.f, 0);
								Cut_Pic[Cut].Aim_camera.fov = deg2rad(25);
								Cut_Pic[Cut].Aim_camera.far_ = 3000.f;
								Cut_Pic[Cut].cam_per = 0.f;
							}
							else {
								Cut_Pic[Cut].Aim_camera.camvec = karen.frame(8);
								Cut_Pic[Cut].cam_per = 0.995f;
							}
							draw_kr = true;
							isNextreset = true;
						}
						else if (Cut < 52) {

							Sel_AnimNum(karen, 2);
							karen.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(2, 0, 0.f)));
							karen.get_anime(2).Update(false, 1.15f);

							Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(0.3f, 18.f, -28.595667f);
							Cut_Pic[Cut].Aim_camera.camvec = karen.frame(10) + VECTOR_ref::vget(2.3f,0.f, 0.f);
							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(0, 1.f, 0);
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(15);
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;
							Cut_Pic[Cut].cam_per = 0.f;
							isNextreset = true;
						}
						else if (Cut < 54) {
							draw_kr = false;
							draw_cafe = true;
							Cafe.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
							Sel_AnimNum(Cafe, 3);
							Cafe.get_anime(3).Update(false, 0.8f);
							CafeOpac = 1.f;
							MV1SetOpacityRate(Cafe.get(), CafeOpac);
							if (Cut ==53) {
								if (Cafe.get_anime(3).time > 30.f) {
									Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(0, 17, -4.5);
									Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
										VECTOR_ref::vget(
											-5.f,
											-3.5f,
											-20.f
										)*2.0f;

									Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(25);
									Cut_Pic[Cut].Aim_camera.near_ = 1.f;
									Cut_Pic[Cut].Aim_camera.far_ = 5000.f;
									Cut_Pic[Cut].cam_per = 0.9975f;
								}
								else {
									if (Cafe.get_anime(3).time > 15.f) {
										Cut_Pic[Cut].cam_per = 0.f;
									}
									else {
										Cut_Pic[Cut].cam_per = 1.f;
									}
								}
								isNextreset = true;

								mobu_b[0].get_anime(4).time = (float)GetRand(100) / 10.f;

								mobu_b[2].get_anime(4).time = (float)GetRand(100) / 10.f;

								doto.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(10.f, 0.f, -2000.f - 5.f)));
								doto.get_anime(0).time = 30.f + (float)GetRand(100) / 10.f;

								Cafe.get_anime(4).time = (float)GetRand(100) / 10.f;

								mobu_b[1].get_anime(4).time = (float)GetRand(100) / 10.f;

								opera.get_anime(0).time = (float)GetRand(100) / 10.f;

							}
						}
						else if (Cut < 55) {
							mobu_b[0].SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.f, -2000.f - 25.f)));
							Sel_AnimNum(mobu_b[0], 4);
							mobu_b[0].get_anime(4).Update(false, 0.84f);

							mobu_b[2].SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(20.f, 0.f, -2000.f - 40.f)));
							Sel_AnimNum(mobu_b[2], 4);
							mobu_b[2].get_anime(4).Update(false, 0.8f);

							Sel_AnimNum(doto, 0);
							doto.get_anime(0).Update(false, 0.82f);

							Cafe.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.f, -2000.f - 30.f)));
							Sel_AnimNum(Cafe, 4);
							Cafe.get_anime(4).Update(false, 0.85f);

							mobu_b[1].SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(5.f, 0.f, -2000.f - 5.f)));
							Sel_AnimNum(mobu_b[1], 4);
							mobu_b[1].get_anime(4).Update(false, 0.82f);

							opera.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(-20.f, 0.f, -2000.f + 10.f)));
							Sel_AnimNum(opera, 0);
							opera.get_anime(0).Update(false, 0.84f);

							if (draw_opr) {
								Cut_Pic[Cut].Aim_camera.camvec = Cafe.frame(7+GetRand(3)) + VECTOR_ref::vget(0.f, 2.f, -30.f);
							}
							else {
								Cut_Pic[Cut].Aim_camera.camvec = Cafe.frame(8) + VECTOR_ref::vget(0.f, 2.f, -30.f);
							}
							Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
								VECTOR_ref::vget(
									-cos(deg2rad(0.5f))*sin(-deg2rad(float(90 - 10 + 180))),
									sin(deg2rad(0.5f)),
									-cos(deg2rad(0.5f))*cos(-deg2rad(float(90 - 10 + 180)))
								)*608.0f;

							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(1.5);
							Cut_Pic[Cut].Aim_camera.near_ = 300.f;
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;

							if (draw_opr) {
								Cut_Pic[Cut].cam_per = 0.975f;
							}
							else {
								Cut_Pic[Cut].cam_per = 0.f;
							}

							isNextreset = true;
							radc2 = -1.f;
							draw_cafe = true;
							draw_dto = true;
							draw_opr = true;
							nex_cut = false;
						}
						else if (Cut < 56) {
							mobu_b[0].SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-2394.f*1.95f + 5.f, 0.f, -2200.f - 45.f)));
							Sel_AnimNum(mobu_b[0], 4);
							mobu_b[0].get_anime(4).Update(false, 0.8f);

							mobu_b[1].SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-2394.f*1.95f - 7.f, 0.f, -2200.f - 70.f)));
							Sel_AnimNum(mobu_b[1], 4);
							mobu_b[1].get_anime(4).Update(false, 0.8f);

							mobu_b[2].SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-2394.f*1.95f + 10.f, 0.f, -2200.f - 65.f)));
							Sel_AnimNum(mobu_b[2], 4);
							mobu_b[2].get_anime(4).Update(false, 0.8f);

							doto.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-2394.f*1.95f - 6.f, 0.f, -2200.f - 35.f)));
							Sel_AnimNum(doto, 0);
							doto.get_anime(0).Update(false, 1.1f);

							Cafe.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-2394.f*1.95f -5.f, 0.f, -2200.f - 0.f)));
							Sel_AnimNum(Cafe, 4);
							Cafe.get_anime(4).Update(false, 0.98f);

							opera.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-2394.f*1.95f - 12.5f, 0.f, -2200.f - 40.f)));
							Sel_AnimNum(opera, 0);
							opera.get_anime(0).Update(false, 0.95f);


							if (nex_cut) {
								Cut_Pic[Cut].Aim_camera.camvec = Cafe.frame(8) + VECTOR_ref::vget(0.f, 0.3f, 20.f);
							}
							else {
								Cut_Pic[Cut].Aim_camera.camvec = Cafe.frame(8) + VECTOR_ref::vget(0.f, 10.f, 30.f);
							}
							if (radc2 < 0.f) {
								Cut_Pic[Cut].Aim_camera.campos = Cafe.frame(8) + VECTOR_ref::vget(0.f, 0.3f, 20.f) +
									VECTOR_ref::vget(
										-cos(deg2rad(1.5f))*sin(-deg2rad(float(90 - 80 + 180) + radc2)),
										sin(deg2rad(1.5f)),
										-cos(deg2rad(1.5f))*cos(-deg2rad(float(90 - 80 + 180) + radc2))
									)*468.0f;
							}
							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
							Cut_Pic[Cut].Aim_camera.near_ = 100.f;
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;

							if (NowTime > (LONGLONG)(1000000.f * 62.755)) {
								Cut_Pic[Cut].Aim_camera.fov = deg2rad(0.85f);
							}
							else {
								Cut_Pic[Cut].Aim_camera.fov = deg2rad(2);
							}

							if (nex_cut) {
								Cut_Pic[Cut].cam_per = 0.95f;
							}
							else {
								Cut_Pic[Cut].cam_per = 0.f;
							}
							nex_cut = true;
							radc2 = 0.f;
							isNextreset = true;
						}
						else if (Cut < 57) {
							draw_dto = false;
							draw_opr = false;
							draw_t = true;
							//Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(116.485695f, 18.346067f, -56.928761f);

							Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(10.122698f, 18.028077f, -100.985756f);

							float xradadd = -2.f + float(GetRand(2 * 2 * 100)) / 100.f;
							easing_set(&xradadd_r, xradadd, 0.975f);
							Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
								VECTOR_ref::vget(
									-cos(deg2rad(-5.5f + xradadd_r * 1.5f))*sin(-deg2rad(float(90 - 10) + radc2)),
									sin(deg2rad(-5.5 + xradadd_r * 1.5f)),
									-cos(deg2rad(-5.5 + xradadd_r * 1.5f))*cos(-deg2rad(float(90 - 10) + radc2))
								)*88.0f;

							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(45);
							Cut_Pic[Cut].Aim_camera.near_ = 10.f;
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;
							Cut_Pic[Cut].cam_per = 0.f;
							radc2 += 12.5f / GetFPS();

							Takion.SetMatrix(MATRIX_ref::RotY(deg2rad(0 + 0))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -140)));
							Sel_AnimNum(Takion, 15);
							Takion.get_anime(15).Update(false, 0.2f);
							isNextreset = true;
						}
						else if (Cut < 58) {
							Cut_Pic[Cut].Aim_camera.camvec = Takion.frame(9);

							float xradadd = -2.f + float(GetRand(2 * 2 * 100)) / 100.f;
							easing_set(&xradadd_r, xradadd, 0.975f);
							Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
								VECTOR_ref::vget(
									-cos(deg2rad(-5.5f + xradadd_r * 1.5f))*sin(-deg2rad(float(90 - 10) + radc2)),
									sin(deg2rad(-5.5 + xradadd_r * 1.5f)),
									-cos(deg2rad(-5.5 + xradadd_r * 1.5f))*cos(-deg2rad(float(90 - 10) + radc2))
								)*88.0f;

							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(8);
							Cut_Pic[Cut].Aim_camera.near_ = 10.f;
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;
							Cut_Pic[Cut].cam_per = 0.f;
							radc2 += 12.5f / GetFPS();

							Takion.SetMatrix(MATRIX_ref::RotY(deg2rad(180 + 0))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -140)));
							Sel_AnimNum(Takion, 15);
							Takion.get_anime(15).Update(false, 0.75f);
							Box_ALPHA = 0.f;
						}
						else if (Cut < 59) {
							Cut_Pic[Cut].Aim_camera.camvec = Takion.frame(9);

							float xradadd = -2.f + float(GetRand(2 * 2 * 100)) / 100.f;
							easing_set(&xradadd_r, xradadd, 0.975f);
							Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec +
								VECTOR_ref::vget(
									-cos(deg2rad(-5.5f + xradadd_r * 1.5f))*sin(-deg2rad(float(90 - 10) + radc2)),
									sin(deg2rad(-5.5 + xradadd_r * 1.5f)),
									-cos(deg2rad(-5.5 + xradadd_r * 1.5f))*cos(-deg2rad(float(90 - 10) + radc2))
								)*88.0f;

							Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
							Cut_Pic[Cut].Aim_camera.fov = deg2rad(8);
							Cut_Pic[Cut].Aim_camera.near_ = 10.f;
							Cut_Pic[Cut].Aim_camera.far_ = 3000.f;
							Cut_Pic[Cut].cam_per = 0.f;
							radc2 += 12.5f / GetFPS();

							Takion.SetMatrix(MATRIX_ref::RotY(deg2rad(180 + 0))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -140)));
							Sel_AnimNum(Takion, 15);
							Takion.get_anime(15).Update(false, 0.75f);

							if (Box_ALPHA < 1.f) {
								Box_ALPHA += 1.f / GetFPS();
								per_logo = 100.f;
							}
							else {
								Box_ALPHA = 1.f;
								if (per_logo > 0.f) {
									per_logo -= 100.f / GetFPS()/1.5f;
									easing_set(&xpos_rand, xpos_rand2, 0.975f);
									easing_set(&ypos_rand, ypos_rand2, 0.975f);
									float per_pow = per_logo / 100.f;
									easing_set(&size_rand, -((float)(0.1f) * per_pow) + (float)(GetRand((int)((float)(0.1f) * per_pow) * 2 * 100)) / 100.f, 0.975f);
									easing_set(&xpos_rand2, -((float)(y_r(1000)) * per_pow) + (float)(GetRand((int)((float)(y_r(1000)) * per_pow) * 2 * 100)) / 100.f, 0.8f);
									easing_set(&ypos_rand2, -((float)(y_r(1000)) * per_pow) + (float)(GetRand((int)((float)(y_r(1000)) * per_pow) * 2 * 100)) / 100.f, 0.8f);
								}
								else {
									per_logo = 0.f;
									easing_set(&size_rand, 0.f, 0.95f);
									easing_set(&xpos_rand, 0.f, 0.95f);
									easing_set(&ypos_rand, 0.f, 0.95f);
								}
							}

							for (auto& sl : sode_last) {
								easing_set(&sl.xpos, sl.xpos_base + y_r(sl.xpos_rand), 0.945f);
								easing_set(&sl.ypos, sl.ypos_base + y_r(sl.ypos_rand), 0.925f);
								easing_set(&sl.rad, sl.rad_base + sl.rad_rand, 0.925f);
								easing_set(&sl.xpos_rand, deg2rad(-50.f + (float)(GetRand(50 * 2 * 100)) / 100.f), 0.95f);
								easing_set(&sl.ypos_rand, deg2rad(-50.f + (float)(GetRand(50 * 2 * 100)) / 100.f), 0.95f);
								easing_set(&sl.rad_rand, deg2rad(-10.f + (float)(GetRand(10 * 2 * 100)) / 100.f), 0.95f);
							}

						}
						easing_set(&camera_main.campos, Cut_Pic[Cut].Aim_camera.campos, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.camvec, Cut_Pic[Cut].Aim_camera.camvec, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.camup, Cut_Pic[Cut].Aim_camera.camup, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.fov, Cut_Pic[Cut].Aim_camera.fov, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.far_, Cut_Pic[Cut].Aim_camera.far_, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.near_, Cut_Pic[Cut].Aim_camera.near_, Cut_Pic[Cut].cam_per);
						camera_main.set_cam_info(camera_main.fov, camera_main.near_, camera_main.far_);
					}

					if (draw_opr) {
						opera.work_anime();
					}
					if (draw_dto) {
						doto.work_anime();
					}
					if (draw_t) {
						Takion.work_anime();
					}
					if (draw_tnr) {
						trainer.work_anime();
					}
					if (draw_t2) {
						Takion2.work_anime();
					}
					if (draw_cafe) {
						Cafe.work_anime();
					}
					if (draw_scar) {
						scarlet.work_anime();
					}
					if (draw_vodka) {
						vodka.work_anime();
					}
					Takion3_1.work_anime();
					Takion3_2.work_anime();
					Takion3_3.work_anime();
					Takion3_4.work_anime();

					goldship.work_anime();
					speweek.work_anime();
					teio.work_anime();
					macin.work_anime();
					Gate.work_anime();
					for (auto& m : mobu_b) {
						m.work_anime();
					}
					if (draw_kr) {
						karen.work_anime();
					}
					if (!Time_Over()) {
						if (NowTime > GetCutTime()) {
							AddCutNum();
							if (Cut == 6) {
								isNextreset = true;
							}

							if (Cut == 9) {
								isNextreset = true;
							}


							if (isNextreset) {
								reset_p = true;
								isNextreset = false;
							}
							radc = 0.f;
						}
						else {
							if (reset_p) {
								reset_p = false;
								if (draw_t) {
									Takion.PhysicsResetState();
								}
								if (draw_t2) {
									Takion2.PhysicsResetState();
								}
								if (draw_cafe) {
									Cafe.PhysicsResetState();
								}
								if (draw_scar) {
									scarlet.PhysicsResetState();
								}
								if (draw_vodka) {
									vodka.PhysicsResetState();
								}
								if (draw_kr) {
									karen.PhysicsResetState();
								}
								if (draw_opr) {
									opera.PhysicsResetState();
								}
								if (draw_dto) {
									doto.PhysicsResetState();
								}
							}
							else {
								if (draw_t) {
									Takion.PhysicsCalculation(30.f / GetFPS()*spd_taki);
								}
								if (draw_t2) {
									Takion2.PhysicsCalculation(30.f / GetFPS()*spd_taki);
								}
								if (draw_cafe) {
									Cafe.PhysicsCalculation(30.f / GetFPS()*spd_cafe);
								}
								if (draw_scar) {
									scarlet.PhysicsCalculation(30.f / GetFPS());
								}
								if (draw_vodka) {
									vodka.PhysicsCalculation(30.f / GetFPS());
								}
								if (draw_kr) {
									karen.PhysicsCalculation(30.f / GetFPS());
								}
								if (draw_opr) {
									opera.PhysicsCalculation(30.f / GetFPS());
								}
								if (draw_dto) {
									doto.PhysicsCalculation(30.f / GetFPS());
								}
							}
						}
					}

				}

				Effect_UseControl::Update_Effect();
				if (Time_Over()) {
					return false;
				}
				if (!BGM.check()) {
					return false;
				}
				return true;
			}
			void Dispose(void) noexcept override {
				Effect_UseControl::Dispose_Effect();

				for (auto& p : Cut_Pic) { p.handle.Dispose(); }
				Cut_Pic.clear();
				//BGM.Dispose();
				movie.Dispose();
			}
			void UI_Draw(void) noexcept  override {
				//printfDx("Cut : %d\n", Cut);


				const auto NowTime = GetNowHiPerformanceCount() - BaseTime;
				for (auto& t : Texts) {
					if (NowTime > t.START_TIME && NowTime < t.END_TIME) {
						Fonts.Get(t.size).Get_handle().DrawString(t.xpos, t.ypos, t.str, GetColor(255, 255, 255));
					}
				}
				//
			}
			void BG_Draw(void) noexcept override {
				if (!Time_Over()) {
					if (Cut > 8 && Cut != 20 && !(Cut >= 30 && Cut <= 31) && !(Cut >= 35 && Cut <= 38) && !((Cut >= 45 && Cut <= 49) || Cut == 52 || Cut == 53)) {
						auto fog_e = GetFogEnable();
						SetFogEnable(FALSE);
						SetUseLighting(FALSE);
						{
							sky.DrawModel();
							DrawBillboard3D(this->sun_pos.get(), 0.5f, 0.5f, 200.f, 0.f, this->sun_pic.get(), TRUE);
						}
						SetUseLighting(TRUE);
						
						SetFogEnable(fog_e);
					}
					//Cut_Pic[Cut].handle.DrawExtendGraph(0, 0, DrawPts->disp_x, DrawPts->disp_y, FALSE);
				}
			}
			void Shadow_Draw_Far(void) noexcept override {
				Map.DrawModel();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				if (Cut == 25) {
					Map_school.DrawModel();
				}
				if (Cut == 26 || Cut == 27 || Cut == 41 || Cut == 42 || Cut == 43 || Cut == 44 || Cut == 54 || Cut == 55 || Cut == 56) {
					Gate.DrawModel();
				}
			}
			void Shadow_Draw(void) noexcept override {
				if (Cut == 25) {
					Map_school.DrawModel();
				}
				if (Cut == 26 || Cut == 27 || Cut == 41 || Cut == 42 || Cut == 43 || Cut == 44 || Cut == 54 || Cut == 55 || Cut == 56) {
					for (auto& m : mobu_b) {
						m.DrawModel();
					}
					Gate.DrawModel();
				}
				if (draw_t2) {
					if (Taki2Opac > 0.f) {
						Takion2.DrawModel();
					}
				}
				if (draw_vodka) {
					vodka.DrawModel();
				}
				if (draw_opr) {
					opera.DrawModel();
				}
				if (draw_dto) {
					doto.DrawModel();
				}
				if (draw_scar) {
					if (scarletOpac > 0.f) {
						scarlet.DrawModel();
					}
				}
				if (draw_t) {
					if (TakiOpac > 0.f) {
						Takion.DrawModel();
					}
				}
				if (draw_cafe) {
					if (CafeOpac > 0.f) {
						Cafe.DrawModel();
					}
				}

				Takion3_1.DrawModel();
				Takion3_2.DrawModel();
				Takion3_3.DrawModel();
				Takion3_4.DrawModel();

				goldship.DrawModel();
				speweek.DrawModel();
				teio.DrawModel();
				macin.DrawModel();
				if (draw_kr) {
					karen.DrawModel();
				}
				if (Cut == 20) {
					for (auto&n : Newspaper) {
						n.model.DrawModel();
					}
				}
			}
			void Main_Draw(void) noexcept override {
				if (Cut == 34) {
					SetFogEnable(TRUE);
					SetFogColor(128, 128, 128);
					SetFogStartEnd(10, 60);
					SetFogDensity(0.01f);
				}
				if (Cut == 2 || Cut == 3 || Cut == 4 || Cut == 6 || Cut == 7 || Cut == 8) {
					SetFogEnable(TRUE);
					SetFogColor(64, 64, 64);
					SetFogStartEnd(10, 500);
					SetFogDensity(0.01f);
					Board.DrawModel();
				}

				if (Cut == 30 || (Cut >= 45 && Cut <= 49) || Cut == 52 || Cut == 53) {
					SetFogEnable(TRUE);
					SetFogColor(128, 128, 128);
					SetFogStartEnd(10, 300);
					SetFogDensity(0.01f);

					Board.DrawModel();
				}
				if (Cut >= 9 && Cut != 20 && Cut != 25 && !(Cut >= 30 && Cut <= 31) && !(Cut >= 35 && Cut <= 38) && !((Cut >= 45 && Cut <= 49) || Cut == 52 || Cut == 53)) {
					Map.DrawModel();
				}
				if (Cut == 25) {
					Map_school.DrawModel();
				}
				if (Cut == 26 || Cut == 27 || Cut == 41 || Cut == 42 || Cut == 43 || Cut == 44 || Cut == 54 || Cut == 55 || Cut == 56) {
					for (auto& m : mobu_b) {
						m.DrawModel();
					}
					Gate.DrawModel();
				}
				if (draw_t2) {
					if (Taki2Opac > 0.f) {
						Takion2.DrawModel();
					}
				}
				if (draw_vodka) {
					vodka.DrawModel();
				}
				if (draw_scar) {
					if (scarletOpac > 0.f) {
						scarlet.DrawModel();
					}
				}
				if (draw_t) {
					if (TakiOpac > 0.f) {
						Takion.DrawModel();
					}
				}
				if (draw_opr) {
					opera.DrawModel();
				}
				if (draw_dto) {
					doto.DrawModel();
				}
				if (draw_tnr) {
					trainer.DrawModel();
				}
				if (draw_cafe) {
					if (CafeOpac > 0.f) {
						Cafe.DrawModel();
					}
				}

				Takion3_1.DrawModel();
				Takion3_2.DrawModel();
				Takion3_3.DrawModel();
				Takion3_4.DrawModel();

				goldship.DrawModel();
				speweek.DrawModel();
				teio.DrawModel();
				macin.DrawModel();
				if (draw_kr) {
					karen.DrawModel();
				}
				if (Cut == 28 || Cut == 29) {
					Ship.DrawModel();
				}
				if (Cut == 20) {
					for (auto&n : Newspaper) {
						n.model.DrawModel();
					}
				}

				SetFogEnable(FALSE);

				anim[0].handle.DrawExtendGraph((int)(anim[0].xpos), (int)(anim[0].ypos), DrawPts->disp_x + (int)(anim[0].xpos), DrawPts->disp_y + (int)(anim[0].ypos), true);
				anim[1].handle.DrawExtendGraph((int)(anim[1].xpos), (int)(anim[1].ypos), DrawPts->disp_x + (int)(anim[1].xpos), DrawPts->disp_y + (int)(anim[1].ypos), true);

				SetDrawBright(108, 108, 108);
				sode[7].handle.DrawExtendGraph((int)(sode[7].xpos), (int)(sode[7].ypos), DrawPts->disp_x + (int)(sode[7].xpos), DrawPts->disp_y + (int)(sode[7].ypos), true);
				SetDrawBright(144, 144, 144);
				sode[6].handle.DrawExtendGraph((int)(sode[6].xpos), (int)(sode[6].ypos), DrawPts->disp_x + (int)(sode[6].xpos), DrawPts->disp_y + (int)(sode[6].ypos), true);
				SetDrawBright(192, 192, 192);
				sode[5].handle.DrawExtendGraph((int)(sode[5].xpos), (int)(sode[5].ypos), DrawPts->disp_x + (int)(sode[5].xpos), DrawPts->disp_y + (int)(sode[5].ypos), true);
				SetDrawBright(255, 255, 255);
				sode[4].handle.DrawExtendGraph((int)(sode[4].xpos), (int)(sode[4].ypos), DrawPts->disp_x + (int)(sode[4].xpos), DrawPts->disp_y + (int)(sode[4].ypos), true);
				sode[3].handle.DrawExtendGraph((int)(sode[3].xpos), (int)(sode[3].ypos), DrawPts->disp_x + (int)(sode[3].xpos), DrawPts->disp_y + (int)(sode[3].ypos), true);
				sode[2].handle.DrawExtendGraph((int)(sode[2].xpos), (int)(sode[2].ypos), DrawPts->disp_x + (int)(sode[2].xpos), DrawPts->disp_y + (int)(sode[2].ypos), true);
				sode[1].handle.DrawExtendGraph((int)(sode[1].xpos), (int)(sode[1].ypos), DrawPts->disp_x + (int)(sode[1].xpos), DrawPts->disp_y + (int)(sode[1].ypos), true);
				sode[0].handle.DrawExtendGraph((int)(sode[0].xpos), (int)(sode[0].ypos), DrawPts->disp_x + (int)(sode[0].xpos), DrawPts->disp_y + (int)(sode[0].ypos), true);

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(196.f));
				for (auto& sl : First2) {
					sl.handle.DrawRotaGraph((int)(sl.xpos), (int)(sl.ypos), (float)(DrawPts->disp_y) / 540.f*0.5f, sl.rad, true);
				}

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				for (auto& sl : First) {
					sl.handle.DrawRotaGraph((int)(sl.xpos), (int)(sl.ypos), (float)(DrawPts->disp_y) / 540.f*0.5f, sl.rad, true);
				}

				for (auto& sl : First3) {
					easing_set(&sl.Alpha_R, sl.Alpha, 0.9f);
					if (sl.Alpha_R > 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*sl.Alpha_R));
						sl.handle.DrawRotaGraph((int)(sl.xpos), (int)(sl.ypos), (float)(DrawPts->disp_y) / 540.f*0.9f*sl.Scale, sl.rad, true);
					}
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				{
					if (Box_ALPHA != 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*Box_ALPHA));
						DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(0, 0, 0), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					if (Box_ALPHA != 0.f) {
						for (auto& sl : sode_last) {
							int per_b = 127 + (int)(128.f* (&sl - &sode_last.front()) / sode_last.size());
							SetDrawBright(per_b, per_b, per_b);
							sl.handle.DrawRotaGraph((int)(sl.xpos), (int)(sl.ypos), (float)(DrawPts->disp_y) / 540.f, sl.rad, true);
						}
						SetDrawBright(255, 255, 255);
					}
					if ((int)per_logo <= GetRand(99)) {
						Logo.DrawRotaGraph(DrawPts->disp_x / 2 + (int)xpos_rand, DrawPts->disp_y / 2 + (int)ypos_rand, (float)(DrawPts->disp_y) / 540.f / 2.f*(1.f + size_rand), 0.f, true);
					}
				}
				/*
				if (Line_ALPHA != 0.f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*Line_ALPHA));
					DrawLine(y_r(600) - y_r(288*4), -y_r(162 * 4), y_r(600) + (int)((float)(y_r(720))*Line_time), y_r(0) + (int)((float)(y_r(1080))*Line_time), GetColor(255, 255, 255), (int)Thickness);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				*/
			}
			void Item_Draw(void) noexcept override {
				TEMPSCENE::Item_Draw();

				if (Cut == 21 || Cut == 22 || Cut == 23) {
					face.DrawExtendGraph(0, 0, DrawPts->disp_x, DrawPts->disp_y, true);
					int cnt = 1;
					for (auto n = news_p.rbegin(); n != news_p.rend(); n++) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * cnt / (int)news_p.size());
						n->graph.DrawRotaGraph(n->xns_pos, n->yns_pos, (float)DrawPts->disp_x / (float)xp_n*1.5f, n->rns_rad, false);
						cnt++;
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
			}
			void LAST_Draw(void) noexcept override {
				int xxx_m = DrawPts->disp_x * 3 / 4 - y_r(200);
				int yyy_m = DrawPts->disp_y * 3 / 4;
				//movie.DrawExtendGraph(xxx_m, yyy_m, DrawPts->disp_x / 4 + xxx_m, DrawPts->disp_y / 4 + yyy_m, FALSE);
			}
		};
	};
};