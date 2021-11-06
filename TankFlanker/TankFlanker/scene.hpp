#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	class Sceneclass {
	public:
		static float GetRandf(float m_arg) { return -m_arg + (float)(GetRand(2 * (int)m_arg * 100)) / 100.f; }
		static VECTOR_ref GetVector(float Xrad, float Yrad) { return VECTOR_ref::vget(-cos(deg2rad(Xrad))*sin(-deg2rad(Yrad)), sin(deg2rad(Xrad)), -cos(deg2rad(Xrad))*cos(-deg2rad(Yrad))); }
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
			std::shared_ptr<HostPassEffect> HostpassPTs;
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
				DrawPts->Ready_Shadow(camera_main.campos, [&] { Shadow_Draw(); }, [&] { Shadow_Draw_NearFar(); }, VECTOR_ref::vget(100.f, 30.f, 100.f), VECTOR_ref::vget(camera_main.far_, 40.f, camera_main.far_));//MAIN_LOOP��near�͂��� (Get_Mine()->Damage.Get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
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
			class Model {
			private:
				void Sel_AnimNum(MV1&model, int sel) {
					for (auto& anim_t : model.get_anime()) {
						model.get_anime(&anim_t - &model.get_anime().front()).per = (&anim_t - &model.get_anime().front() == sel) ? 1.f : 0.f;
					}
				}
			public:
				bool isBase = true;
				size_t numBase = 0;
				float PhysicsSpeed = 1.f;

				float b_run = 0;
				float b_runrange = 0;

				std::string Path;
				MV1 obj;
				moves move;
				bool isDraw = true;
				float OpacityRate = 1.f;
				int Anim_Sel = 0;
				void UpdateAnim(int ID, bool isloop, float speed) {
					Sel_AnimNum(this->obj, ID);
					this->obj.get_anime(ID).Update(isloop, speed);
				}
				void Update() {
					MV1SetOpacityRate(this->obj.get(), this->OpacityRate);
					if (this->isDraw) {
						this->obj.work_anime();
					}
				}
				void Draw() {
					if (this->isDraw && this->OpacityRate > 0.f) {
						this->obj.DrawModel();
					}
				}
				void SetPhysics(bool isReset) {
					if (this->isDraw) {
						if (isReset) {
							this->obj.PhysicsResetState();
						}
						else {
							this->obj.PhysicsCalculation(30.f / GetFPS()*this->PhysicsSpeed);
						}
					}
				}
			};
			class ModelControl {
				std::vector<Model> model;
				size_t Max = 0;
			public:
				ModelControl() {
					model.resize(64);
					Max = 0;
				}
				void Load_onAnime(std::string_view Path) {
					for (size_t i = 0; i < Max; i++) {
						if (model[i].isBase && model[i].Path == Path) {
							model[Max].Path = Path;
							model[Max].isBase = false;
							model[i].obj.DuplicateonAnime(&(model[Max].obj), &(model[Max].obj));
							Max++;
							return;
						}
					}
					model[Max].Path = Path;
					model[Max].isBase = true;
					MV1::LoadonAnime(model[Max].Path, &(model[Max].obj));
					Max++;
				}
				Model* Get(std::string_view Path, size_t Sel = 0) {
					int Cnt = 0;
					for (size_t i = 0; i < Max; i++) {
						if (model[i].Path == Path) {
							if (Cnt >= Sel) {
								return &(model[i]);
							}
							Cnt++;
						}
					}
					return nullptr;
				}

				void Set() {
					for (auto& m : model) {
						m.isDraw = false;
					}
				}
				void Update() {
					for (auto& m : model) {
						m.Update();
					}
				}
				void Draw() {
					for (auto& m : model) {
						m.Draw();
					}
				}
			};
			class Cut_Info {
			public:
				cam_info Aim_camera;
				float cam_per = 0.0f;
				LONGLONG TIME = 0;
				bool isNextreset = false;

				Cut_Info() {
					Aim_camera.campos = VECTOR_ref::vget(0, 10, -30);
					Aim_camera.camvec = VECTOR_ref::vget(0, 10, 0);
					Aim_camera.camup = VECTOR_ref::up();
					Aim_camera.set_cam_info(deg2rad(15), 1.0f, 300.f);
					cam_per = 0.95f;
					isNextreset = false;
				}

				void Dispose() {

				}
			};
			class Cut_tex {
				int xpos = 0;
				int ypos = 0;
				int size = 0;
				int LMR = 1;
				std::string str;
				LONGLONG START_TIME = 0;
				LONGLONG END_TIME = 0;
			public:
				Cut_tex() {
					xpos = 0;
					ypos = 0;
					size = 12;
					str = "test";
					START_TIME = (LONGLONG)(1000000.f * 0.01f);
					END_TIME = (LONGLONG)(1000000.f * 1.01f);
				}
				void Set(int xp, int yp, int Fontsize, std::string_view mag, LONGLONG StartF, LONGLONG ContiF, int m_LMR) {
					this->xpos = xp;
					this->ypos = yp;
					this->size = Fontsize;
					this->str = mag;
					this->START_TIME = StartF;
					this->END_TIME = StartF + ContiF;;
					this->LMR = m_LMR;
				}
				void Draw(LONGLONG NowTime) {
					if (NowTime > this->START_TIME && NowTime < this->END_TIME) {
						switch (this->LMR)
						{
						case 0:
							Fonts.Get(this->size).Get_handle().DrawString(this->xpos, this->ypos, this->str, GetColor(255, 255, 255));
							break;
						case 1:
							Fonts.Get(this->size).Get_handle().DrawString_MID(this->xpos, this->ypos, this->str, GetColor(255, 255, 255));
							break;
						case 2:
							Fonts.Get(this->size).Get_handle().DrawString_RIGHT(this->xpos, this->ypos, this->str, GetColor(255, 255, 255));
							break;
						default:
							break;
						}
					}
				}
			};
			class GraphControl {
				float rad_base = 0;
				float Scale_base = 1.f;

				float xpos_rand = 0;
				float ypos_rand = 0;
				float rad_rand = 0;

				float Scale_rand = 0;


				GraphHandle handle;
				int Bright_R = 255;
				int Bright_G = 255;
				int Bright_B = 255;
			public:
				int xsize = -1;
				int ysize = -1;

				float x_per = 1.f;
				float y_per = 1.f;
				float xpos_base = 0;
				float ypos_base = 0;
				float Alpha_base = 1.f;

				bool isDraw = false;

				float xpos = 0;
				float ypos = 0;
				float rad = 0;
				float Alpha = 1.f;
				float Scale = 1.f;

				void SetBright(int b_r, int b_g, int b_b) {
					Bright_R = b_r;
					Bright_G = b_g;
					Bright_B = b_b;
				}
				void Set(float xp, float yp, float rd, float al, float sc, std::string_view Path) {
					this->xpos = xp;
					this->ypos = yp;
					this->rad = rd;
					this->Alpha = al;
					this->Scale = sc;
					this->isDraw = false;
					this->handle = GraphHandle::Load(Path);
					this->handle.GetSize(&xsize, &ysize);
				}
				void Set_Base(float xp, float xper, float yp, float yper, float rd,float scl) {
					this->xpos_base = xp;
					this->ypos_base = yp;
					this->x_per = xper;
					this->y_per = yper;
					this->rad_base = rd;
					this->Scale_base = scl;
				}
				void Set_Rand(float xp, float xper, float yp, float yper, float rd, float rdper, float scl, float sclper) {
					easing_set(&this->xpos_rand, GetRandf(xp), xper);
					easing_set(&this->ypos_rand, GetRandf(yp), yper);
					easing_set(&this->rad_rand, deg2rad(GetRandf(rd)), rdper);
					easing_set(&this->Scale_rand, GetRandf(scl), sclper);
				}
				void Update_Rand(float xper, float yper, float rdper, float sclper) {
					easing_set(&this->xpos, this->xpos_base + this->xpos_rand, xper);
					easing_set(&this->ypos, this->ypos_base + this->ypos_rand, yper);
					easing_set(&this->rad, this->rad_base + this->rad_rand, rdper);
					easing_set(&this->Scale, this->Scale_base + this->Scale_rand, sclper);
				}
				void Draw(float scale_) {
					if (this->isDraw && this->Alpha > 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*this->Alpha));
						SetDrawBright(Bright_R, Bright_G, Bright_B);
						this->handle.DrawRotaGraph((int)(this->xpos), (int)(this->ypos), scale_*this->Scale, this->rad, true);
						SetDrawBright(255, 255, 255);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
				}
				void Draw_Ex(int disp_x, int disp_y) {
					if (this->isDraw && this->Alpha > 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*this->Alpha));
						SetDrawBright(Bright_R, Bright_G, Bright_B);
						this->handle.DrawExtendGraph((int)(this->xpos), (int)(this->ypos), disp_x + (int)(this->xpos), disp_y + (int)(this->ypos), true);
						SetDrawBright(255, 255, 255);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
				}
			};
			//�X�N���v�g�ǂݍ��ݏ���
			std::string func;
			std::vector<std::string> args;
			static void Sub_Func(std::string& func_t, const char& in_str) {
				size_t str_switch = 0;
				size_t str_in = std::string::npos;
				bool in = false;
				while (true) {
					if (str_switch != std::string::npos) { str_switch = func_t.find('\"', str_switch + 1); in ^= 1; }
					str_in = func_t.find(in_str, str_in + 1);
					if (str_in != std::string::npos) {
						if (str_switch != std::string::npos && str_switch < str_in && in) {
							continue;
						}
						func_t = func_t.erase(str_in, 1);
						continue;
					}
					break;
				}
			}
			void LoadScript(std::string_view func_t) {
				args.clear();
				func = func_t;
				{
					// //���폜
					size_t sls = func.find("//");
					if (sls != std::string::npos) { func = func.substr(0, sls); }
					//����Ȃ��v�f��r��
					Sub_Func(func, '{');
					Sub_Func(func, '}');
					Sub_Func(func, ' ');
					Sub_Func(func, '\t');
					Sub_Func(func, ';');
					Sub_Func(func, '\"');
				}
				//()��,�ň͂�ꂽ��������������擾
				if (func != "") {
					std::string tmp_func = func;
					size_t left = tmp_func.find("(");
					size_t right = tmp_func.rfind(")");
					if (left != std::string::npos && right != std::string::npos) {
						tmp_func = tmp_func.substr(left + 1, right - 1 - left);
					}
					while (true) {
						size_t in_str = tmp_func.find(",");
						if (in_str == std::string::npos) {
							args.emplace_back(tmp_func);
							break;
						}
						else {
							std::string arg = tmp_func.substr(0, in_str);
							tmp_func = tmp_func.substr(in_str + 1);
							args.emplace_back(arg);
						}
					}
				}
			}
		private:
			std::string Tachyon = "data/umamusume/Tachyon/model.mv1";
			std::string Tachyon2 = "data/umamusume/Tachyon2/model.mv1";
			std::string Cafe = "data/umamusume/Cafe/model.mv1";
			std::string Scarlet = "data/umamusume/scarlet/model.mv1";
			std::string Vodka = "data/umamusume/vodka/model.mv1";
			std::string GoldShip = "data/umamusume/gold/model.mv1";
			std::string Speweek = "data/umamusume/spe/model.mv1";
			std::string Teio = "data/umamusume/teio/model.mv1";
			std::string Macin = "data/umamusume/mac/model.mv1";
			std::string Karen = "data/umamusume/karen/model.mv1";
			std::string Trainer = "data/umamusume/trainer/model.mv1";
			std::string Opera = "data/umamusume/opera/model.mv1";
			std::string Doto = "data/umamusume/doto/model.mv1";
			std::string Mobu = "data/umamusume/mobu_black/model.mv1";

			std::string GATE = "data/map/model_gate.mv1";
			std::string MAP = "data/map/model.mv1";
			std::string BOARD = "data/board/model.mv1";
			std::string SHIP = "data/ship/model.mv1";
			std::string SCHOOL = "data/school/model.mv1";
			std::string NEWS = "data/paper/news.mv1";
			std::string SKY = "data/sky/model.mv1";

			Model Board;
			Model Map;
			Model sky;						//��
			Model Ship;

			GraphHandle sun_pic;			//���z
			VECTOR_ref sun_pos;				//���z

			ModelControl models;

			GraphControl face;
			std::vector<GraphControl> news_p;
			std::vector<GraphControl> sode;
			std::vector<GraphControl> anim;
			std::vector<GraphControl> sode_last;
			std::vector<GraphControl> First;
			std::vector<GraphControl> First2;
			std::vector<GraphControl> First3;
			GraphControl Logo;
			float per_logo = 1.f;
			//�f�[�^
			SoundHandle BGM;
			size_t Cut = 0;
			std::vector<Cut_Info> Cut_Pic;
			std::vector<Cut_tex> Texts;
			LONGLONG BaseTime = 0;
			//GraphHandle movie;
			bool reset_p = true;
			bool isNextreset = true;//�J�b�g�I�����ɕ��������Z�b�g���邩�ǂ���
			bool isFirstLoop = true;//�J�b�g�ŏ��̃��[�v���擾
			//
			float radc = 0.f;
			float xradadd_r = 0.f;
			float spd_takion = 0.6f;
			bool return_walk = false;
			float Gate_Xpos = 0;
			float Box_ALPHA = 0.f;
			float camzb_28 = 0.f;
			float camyup = 0.f;
			float camzb_41 = 0.f;
			float bright_43 = 0.f;
			float ship_z = 0.f;
			float ship_zadd = 0.f;
			float cam_yure = 0.f;
			VECTOR_ref rand_vp;
		private:
			bool Time_Over() { return Cut >= Cut_Pic.size(); }
		public:
			using TEMPSCENE::TEMPSCENE;
			void Awake(void) noexcept override {
				TEMPSCENE::Awake();
				//
				SetUseASyncLoadFlag(TRUE);
				//*
				{
					//3D
					{
						MV1::LoadonAnime(MAP, &Map.obj, 0);
						MV1::LoadonAnime(BOARD, &Board.obj, 0);
						MV1::LoadonAnime(SHIP, &Ship.obj, 0);			//��
						MV1::LoadonAnime(SKY, &sky.obj, 0);				//��
					}
					//2D
					{
						this->sun_pic = GraphHandle::Load("data/sun.png");					//sun
						face.Set((float)(DrawPts->disp_x / 2), (float)(DrawPts->disp_y / 2), 0, 1.f, 1.f, "data/Cut.png");
						for (int i = 0; i < 10; i++) {
							news_p.resize(news_p.size() + 1);
							news_p.back().Set((float)(DrawPts->disp_x / 2 - DrawPts->disp_x), (float)(DrawPts->disp_y / 2 - DrawPts->disp_y), deg2rad(-30), (float)(i + 1) / 10.f, 1.5f, "data/Cut2.png");
						}
						sode.resize(sode.size() + 1);
						sode.back().Set((float)(DrawPts->disp_x / 2) + (float)(DrawPts->disp_x / 5), (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 1.f, 1.f, "data/second_sode/2.png");
						sode.resize(sode.size() + 1);
						sode.back().Set((float)(DrawPts->disp_x / 2) + (float)(-DrawPts->disp_x / 5), (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 1.f, 1.f, "data/second_sode/1.png");
						sode.resize(sode.size() + 1);
						sode.back().Set((float)(DrawPts->disp_x / 2) + (float)(DrawPts->disp_x / 5), (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 1.f, 1.f, "data/second_sode/2.png");
						sode.resize(sode.size() + 1);
						sode.back().Set((float)(DrawPts->disp_x / 2) + (float)(-DrawPts->disp_x / 5), (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 1.f, 1.f, "data/second_sode/1.png");
						sode.resize(sode.size() + 1);
						sode.back().Set((float)(DrawPts->disp_x / 2) + (float)(-DrawPts->disp_x / 5), (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 1.f, 1.f, "data/first_sode/4.png");
						sode.resize(sode.size() + 1);
						sode.back().Set((float)(DrawPts->disp_x / 2) + (float)(DrawPts->disp_x / 5), (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 1.f, 1.f, "data/first_sode/3.png");
						sode.resize(sode.size() + 1);
						sode.back().Set((float)(DrawPts->disp_x / 2) + (float)(-DrawPts->disp_x / 5), (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 1.f, 1.f, "data/first_sode/2.png");
						sode.resize(sode.size() + 1);
						sode.back().Set((float)(DrawPts->disp_x / 2) + (float)(DrawPts->disp_x / 5), (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 1.f, 1.f, "data/first_sode/1.png");

						sode[0].SetBright(108, 108, 108);
						sode[1].SetBright(144, 144, 144);
						sode[2].SetBright(192, 192, 192);

						Logo.Set((float)(DrawPts->disp_x / 2), (float)(DrawPts->disp_y / 2), 0, 1.f, 0.5f, "data/logo.png");
						Logo.Set_Base((float)(DrawPts->disp_x / 2), 0.f, (float)(DrawPts->disp_y / 2), 0.f, 0.f, 0.5f);

						sode_last.resize(10);
						sode_last[0].Set((float)(DrawPts->disp_x / 2) + (float)(y_r(-150)) * 10.f, (float)(DrawPts->disp_y / 2) + (float)(y_r(100)) * 10.f, deg2rad(-260), 1.f, 1.f, "data/second_sode/1.png");
						sode_last[1].Set((float)(DrawPts->disp_x / 2) + (float)(y_r(-300)) * 10.f, (float)(DrawPts->disp_y / 2) + (float)(y_r(-500)) * 10.f, deg2rad(-230), 1.f, 1.f, "data/second_sode/1.png");
						sode_last[2].Set((float)(DrawPts->disp_x / 2) + (float)(y_r(0)) * 10.f, (float)(DrawPts->disp_y / 2) + (float)(y_r(-600)) * 10.f, deg2rad(-180), 1.f, 1.f, "data/second_sode/1.png");
						sode_last[3].Set((float)(DrawPts->disp_x / 2) + (float)(y_r(-500)) * 10.f, (float)(DrawPts->disp_y / 2) + (float)(y_r(300)) * 10.f, deg2rad(-270), 1.f, 1.f, "data/second_sode/1.png");
						sode_last[4].Set((float)(DrawPts->disp_x / 2) + (float)(y_r(400)) * 10.f, (float)(DrawPts->disp_y / 2) + (float)(y_r(-550)) * 10.f, deg2rad(-140), 1.f, 1.f, "data/second_sode/1.png");
						sode_last[5].Set((float)(DrawPts->disp_x / 2) + (float)(y_r(500)) * 10.f, (float)(DrawPts->disp_y / 2) + (float)(y_r(500)) * 10.f, deg2rad(-10), 1.f, 1.f, "data/second_sode/1.png");
						sode_last[6].Set((float)(DrawPts->disp_x / 2) + (float)(y_r(600)) * 10.f, (float)(DrawPts->disp_y / 2) + (float)(y_r(-300)) * 10.f, deg2rad(-110), 1.f, 1.f, "data/second_sode/1.png");
						sode_last[7].Set((float)(DrawPts->disp_x / 2) + (float)(y_r(700)) * 10.f, (float)(DrawPts->disp_y / 2) + (float)(y_r(100)) * 10.f, deg2rad(-90), 1.f, 1.f, "data/second_sode/1.png");
						sode_last[8].Set((float)(DrawPts->disp_x / 2) + (float)(y_r(50)) * 10.f, (float)(DrawPts->disp_y / 2) + (float)(y_r(700)) * 10.f, deg2rad(30), 1.f, 1.f, "data/second_sode/1.png");
						sode_last[9].Set((float)(DrawPts->disp_x / 2) + (float)(y_r(-450)) * 10.f, (float)(DrawPts->disp_y / 2) + (float)(y_r(550)) * 10.f, deg2rad(50), 1.f, 1.f, "data/second_sode/1.png");

						for (auto& sl : sode_last) {
							sl.Set_Base(
								(float)(DrawPts->disp_x / 2) + (sl.xpos - (float)(DrawPts->disp_x / 2)) / 10.f, 0.f,
								(float)(DrawPts->disp_y / 2) + (sl.ypos - (float)(DrawPts->disp_y / 2)) / 10.f, 0.f,
								sl.rad - deg2rad(90), 1.f);
						}
						First.resize(First.size() + 1);
						First[0].Set((float)(DrawPts->disp_x / 2 - y_r(1920 * 3 / 10)), (float)(DrawPts->disp_y / 2 + y_r(100)), 0.f, 1.f, 172.f / 540.f * 0.5f, "data/FIRST.png");
						First.resize(First.size() + 1);
						First[1].Set((float)(DrawPts->disp_x / 2 + y_r(1920 / 4)), (float)(DrawPts->disp_y / 2 - y_r(1080 * 3 / 10)), 0.f, 1.f, 172.f / 540.f * 0.5f, "data/FIRST.png");
						First.resize(First.size() + 1);
						First[2].Set((float)(DrawPts->disp_x / 2 + y_r(1920 / 6)), (float)(DrawPts->disp_y / 2 + y_r(1080 * 3 / 10)), 0.f, 1.f, 172.f / 540.f * 0.5f, "data/FIRST.png");
						First2.resize(First2.size() + 1);
						First2[0].Set((float)(DrawPts->disp_x / 2 - y_r(1920 * 3 / 10)), (float)(DrawPts->disp_y / 2 + y_r(100)), 0.f, 196.f / 255.f, 128.f / 540.f * 0.5f, "data/sun.png");
						First2.resize(First2.size() + 1);
						First2[1].Set((float)(DrawPts->disp_x / 2 + y_r(1920 / 4)), (float)(DrawPts->disp_y / 2 - y_r(1080 * 3 / 10)), 0.f, 196.f / 255.f, 128.f / 540.f * 0.5f, "data/sun.png");
						First2.resize(First2.size() + 1);
						First2[2].Set((float)(DrawPts->disp_x / 2 + y_r(1920 / 6)), (float)(DrawPts->disp_y / 2 + y_r(1080 * 3 / 10)), 0.f, 196.f / 255.f, 128.f / 540.f * 0.5f, "data/sun.png");

						First3.resize(First3.size() + 1);
						First3.back().Set((float)(DrawPts->disp_x / 2 - y_r(1920 * 3 / 10)), (float)(DrawPts->disp_y / 2 + y_r(100)), 0.f, 0.f, 128.f / 540.f * 0.9f, "data/sun.png");
						First3.resize(First3.size() + 1);
						First3.back().Set((float)(DrawPts->disp_x / 2 + y_r(1920 / 4)), (float)(DrawPts->disp_y / 2 - y_r(1080 * 3 / 10)), 0.f, 0.f, 128.f / 540.f * 0.9f, "data/sun.png");
						First3.resize(First3.size() + 1);
						First3.back().Set((float)(DrawPts->disp_x / 2 + y_r(1920 / 6)), (float)(DrawPts->disp_y / 2 + y_r(1080 * 3 / 10)), 0.f, 0.f, 128.f / 540.f * 0.9f, "data/sun.png");
						anim.resize(anim.size() + 1);
						anim.back().Set((float)(DrawPts->disp_x) / 2.f, (float)(DrawPts->disp_y) / 2.f, 0, 1.f, 1.f, "data/anime/0.png");
						anim.resize(anim.size() + 1);
						anim.back().Set((float)(DrawPts->disp_x) / 2.f, (float)(DrawPts->disp_y) / 2.f + (float)(DrawPts->disp_y), 0, 1.f, 1.f, "data/anime/1.png");
					}
				}
				//*/
				SetUseASyncLoadFlag(FALSE);
				//
				camera_main.campos = VECTOR_ref::vget(0, 20, -20);
				camera_main.camvec = VECTOR_ref::vget(0, 20, 0);
				camera_main.camup = VECTOR_ref::up();
				camera_main.set_cam_info(deg2rad(15), 1.0f, 200.f);
				//
				SetUseASyncLoadFlag(FALSE);
				{
					LONGLONG StartF = 0;
					LONGLONG ContiF = 0;
					int mdata = FileRead_open("data/Cut.txt", FALSE);
					int cnt = 0;

					clsDx();
					const auto NowTime = GetNowHiPerformanceCount();
					while (FileRead_eof(mdata) == 0) {
						LoadScript(getparams::get_str(mdata));
						if (func == "") { continue; }
						//���f���ǂݍ���
						{
							if (func.find("LoadModel") != std::string::npos) {
								SetUseASyncLoadFlag(TRUE);
								for (int i = 0; i < std::stoi(args[1]); i++) {
									models.Load_onAnime(args[0]);					//origin
								}
								SetUseASyncLoadFlag(FALSE);
							}
						}
						//�J�b�g
						{
							if (func.find("SetCut") != std::string::npos) {
								Cut_Pic.resize(Cut_Pic.size() + 1);
								Cut_Pic.back().TIME = (LONGLONG)(1000000.f * std::stof(args[0]));
							}
							else if (func.find("SetCampos_NoneRad") != std::string::npos) {
								Cut_Pic.back().Aim_camera.campos = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
							}
							else if (func.find("SetCampos_ByRad") != std::string::npos) {
								Cut_Pic.back().Aim_camera.campos = Cut_Pic.back().Aim_camera.camvec + GetVector(std::stof(args[0]), std::stof(args[1])) *std::stof(args[2]);
							}
							else if (func.find("SetCamvec") != std::string::npos) {
								Cut_Pic.back().Aim_camera.camvec = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
							}
							else if (func.find("SetCamup") != std::string::npos) {
								Cut_Pic.back().Aim_camera.camup = VECTOR_ref::vget(std::stof(args[0]), std::stof(args[1]), std::stof(args[2]));
							}
							else if (func.find("SetCamInfo") != std::string::npos) {
								Cut_Pic.back().Aim_camera.fov = deg2rad(std::stof(args[0]));
								Cut_Pic.back().Aim_camera.near_ = std::stof(args[1]);
								Cut_Pic.back().Aim_camera.far_ = std::stof(args[2]);
								Cut_Pic.back().cam_per = std::stof(args[3]);
							}
							else if (func.find("SetNextResetPhysics") != std::string::npos) {
								Cut_Pic.back().isNextreset = true;
							}
						}
						//�e���b�v
						{
							if (func.find("SetTelopTime") != std::string::npos) {
								StartF = (LONGLONG)(1000000.f * std::stof(args[0]));
								ContiF = (LONGLONG)(1000000.f * std::stof(args[1]));
							}
							else if (func.find("AddTelopTime") != std::string::npos) {
								StartF += (LONGLONG)(1000000.f * std::stof(args[0]));
								ContiF = (LONGLONG)(1000000.f * std::stof(args[1]));
							}
							else if (func.find("SetTelop") != std::string::npos) {
								int t = 0;
								if (args[4].find("LEFT") != std::string::npos) { t = 0; }
								else if (args[4].find("MIDDLE") != std::string::npos) { t = 1; }
								else if (args[4].find("RIGHT") != std::string::npos) { t = 2; }
								Texts.resize(Texts.size() + 1);
								Texts.back().Set(y_r(std::stoi(args[0])), y_r(std::stoi(args[1])), y_r(std::stoi(args[2])), args[3], StartF, ContiF, t);
							}
						}
						//END
						if (ProcessMessage() == 0) {
							GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
							printfDx("���[�h%3d���� [%6.2f s] : %s\n", cnt, float((GetNowHiPerformanceCount() - NowTime) / 1000) / 1000.f, func.c_str());
							DrawPts->Screen_Flip();
						}
						cnt++;
					}
					FileRead_close(mdata);
					if (ProcessMessage() == 0) {
						GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
						printfDx("���[�h�S�������@�L�[�������Ă�������[%6.2f s]\n", float((GetNowHiPerformanceCount() - NowTime) / 1000) / 1000.f);
						DrawPts->Screen_Flip();
					}
				}
				WaitKey();
				SetUseASyncLoadFlag(FALSE);
				//���f���̎��O����
				{
					models.Set();
					Map.isDraw = false;
					Board.isDraw = false;
					Ship.isDraw = false;
					sky.isDraw = false;
					for (int i = 0; i < 12; i++) {
						models.Get(Mobu, i)->b_run = 0.8f + (float)(GetRand(195)) / 1000.f;
						models.Get(Mobu, i)->b_runrange = (float)(GetRand(100)) / 10.f;
						while (true) {
							models.Get(Mobu, i)->Anim_Sel = GetRand(2);
							if (i == 0 || models.Get(Mobu, i)->Anim_Sel != models.Get(Mobu, i - 1)->Anim_Sel) {
								break;
							}
						}
					}
					Map.obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.25f, -2394.f))*MATRIX_ref::RotY(deg2rad(90)));
					Board.obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.25f, 0.f)));
					models.Get(SCHOOL, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(-819.f, -28.2f, -590.f))*MATRIX_ref::RotY(deg2rad(-90)));
				}
			}
		public:
			void Set(void) noexcept override {
				TEMPSCENE::Set_EnvLight(VECTOR_ref::vget(500.f, 50.f, 500.f), VECTOR_ref::vget(-500.f, -50.f, -500.f), VECTOR_ref::vget(-0.5f, -0.5f, 0.5f), GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				TEMPSCENE::Set();
				this->sun_pos = Get_Light_vec().Norm() * -1500.f;
				Cut = 41;
				Cut = 0;
				BGM = SoundHandle::Load("data/sound2.wav");
				/*
				movie = GraphHandle::Load("data/base_movie.mp4");
				PlayMovieToGraph(movie.get(), 2, DX_MOVIEPLAYTYPE_BCANCEL);
				ChangeMovieVolumeToGraph(0, movie.get());
				*/
				SetUseASyncLoadFlag(FALSE);
				//�v���C�p��
				BGM.play(DX_PLAYTYPE_BACK, TRUE);
				//BGM.vol(64);
				BaseTime = GetNowHiPerformanceCount() - (Cut > 0 ? Cut_Pic[Cut - 1].TIME : 0);
				//SeekMovieToGraph(movie.get(), (int)(Cut > 0 ? Cut_Pic[Cut - 1].TIME : 0) / 1000);
				HostpassPTs->Set_Bright(226, 226, 216);
			}
			bool Update(void) noexcept override {
				TEMPSCENE::Update();
				const auto NowTime = GetNowHiPerformanceCount() - BaseTime;
				{
					if (!Time_Over()) {
						int SEL = 0;
						{
							//0
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
							}
							SEL++;
							//1
							if (Cut == SEL) {
								if (isFirstLoop) {
									sode[7].isDraw = true;
									sode[6].isDraw = true;
									sode[5].isDraw = true;
									sode[4].isDraw = true;
									models.Get(Tachyon2, 0)->isDraw = true;
								}
								sode[7].Set_Base((float)(DrawPts->disp_x / 2), 0.9f, (float)(DrawPts->disp_y / 2), 0.9f, 0, 1.f);
								sode[6].Set_Base((float)(DrawPts->disp_x / 2), 0.875f, (float)(DrawPts->disp_y / 2), 0.875f, 0, 1.f);
								sode[5].Set_Base((float)(DrawPts->disp_x / 2), 0.85f, (float)(DrawPts->disp_y / 2), 0.85f, 0, 1.f);
								sode[4].Set_Base((float)(DrawPts->disp_x / 2), 0.825f, (float)(DrawPts->disp_y / 2), 0.825f, 0, 1.f);
								if (NowTime > (LONGLONG)(1000000.f * 1.421f)) {
									First[0].isDraw = true;
									First2[0].isDraw = true;
									First3[0].isDraw = true;
									if (First3[0].Alpha_base < 0.f) {
										First3[0].Alpha_base = 1.f;
									}
								}
								if (NowTime > (LONGLONG)(1000000.f * 1.637f)) {
									First[1].isDraw = true;
									First2[1].isDraw = true;
									First3[1].isDraw = true;
									if (First3[1].Alpha_base < 0.f) {
										First3[1].Alpha_base = 1.f;
									}
								}
								if (NowTime > (LONGLONG)(1000000.f * 1.820f)) {
									First[2].isDraw = true;
									First2[2].isDraw = true;
									First3[2].isDraw = true;
									if (First3[2].Alpha_base < 0.f) {
										First3[2].Alpha_base = 1.f;
									}
								}
								for (auto& sl : First3) {
									if (sl.Alpha_base > 0.f) {
										sl.Alpha_base = std::max(sl.Alpha_base - 6.f / GetFPS(), 0.f);
										sl.Scale += 6.f*(128.f / 540.f * 0.9f) / GetFPS();
									}
									if (sl.Alpha_base >= 0.f) {
										easing_set(&sl.Alpha, sl.Alpha_base, 0.9f);
									}
								}
							}
							SEL++;
							//2
							if (Cut == SEL) {
								if (isFirstLoop) {
									sode[7].isDraw = false;
									sode[6].isDraw = false;
									sode[5].isDraw = false;
									sode[4].isDraw = false;
									for (auto& sl : First) {
										sl.isDraw = false;
									}
									for (auto& sl : First2) {
										sl.isDraw = false;
									}
									Board.isDraw = true;
								}
								models.Get(Tachyon2, 0)->UpdateAnim(0, false, 0.8f);
							}
							SEL++;
							//3
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
								models.Get(Tachyon2, 0)->UpdateAnim(0, false, 0.8f);
							}
							SEL++;
							//4
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
								models.Get(Tachyon2, 0)->UpdateAnim(0, false, 0.8f);
							}
							SEL++;
							//5
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
								models.Get(Tachyon2, 0)->UpdateAnim(0, false, 0.8f);
							}
							SEL++;
							//6
							if (Cut == SEL) {
								if (isFirstLoop) {
									sode[7].isDraw = true;
									sode[6].isDraw = true;
									sode[5].isDraw = true;
									sode[4].isDraw = true;
									for (auto& sl : First) {
										sl.isDraw = true;
									}
									for (auto& sl : First2) {
										sl.isDraw = true;
									}
								}
								sode[7].Set_Base((float)(DrawPts->disp_x / 2), 0.f, (float)(DrawPts->disp_y / 2), 0.f, 0, 1.f);
								sode[6].Set_Base((float)(DrawPts->disp_x / 2), 0.f, (float)(DrawPts->disp_y / 2), 0.f, 0, 1.f);
								sode[5].Set_Base((float)(DrawPts->disp_x / 2), 0.f, (float)(DrawPts->disp_y / 2), 0.f, 0, 1.f);
								sode[4].Set_Base((float)(DrawPts->disp_x / 2), 0.f, (float)(DrawPts->disp_y / 2), 0.f, 0, 1.f);
								models.Get(Tachyon2, 0)->UpdateAnim(0, false, 0.8f);
							}
							SEL++;
							//7
							if (Cut == SEL) {
								if (isFirstLoop) {
									Set_Effect(Effect::ef_reco, VECTOR_ref::vget(0.f, 0.f, 0.f), VECTOR_ref::front(), 3.f);
									cam_yure = 0.f;
									for (auto& sl : First) {
										sl.isDraw = false;
									}
									for (auto& sl : First2) {
										sl.isDraw = false;
									}
								}
								sode[7].Set_Base((float)(DrawPts->disp_x / 2) + (float)(DrawPts->disp_x), 0.9f, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y / 5), 0.9f, 0, 1.f);
								sode[6].Set_Base((float)(DrawPts->disp_x / 2) + (float)(-DrawPts->disp_x), 0.875f, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y / 5), 0.875f, 0, 1.f);
								sode[5].Set_Base((float)(DrawPts->disp_x / 2) + (float)(DrawPts->disp_x), 0.85f, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y / 5), 0.85f, 0, 1.f);
								sode[4].Set_Base((float)(DrawPts->disp_x / 2) + (float)(-DrawPts->disp_x), 0.825f, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y / 5), 0.825f, 0, 1.f);
								models.Get(Tachyon2, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.f, 0.f))*MATRIX_ref::RotY(deg2rad(180)));
								if (models.Get(Tachyon2, 0)->obj.get_anime(0).per == 0) {
									SetScale_Effect(Effect::ef_reco, 3.f + cam_yure * 1.f / 3.f);
									SetSpeed_Effect(Effect::ef_reco, 1.05f);
									cam_yure += 5.f / GetFPS();
									easing_set(&Cut_Pic[Cut].Aim_camera.campos, Cut_Pic[Cut].Aim_camera.camvec + GetVector(6.9f, 180.f + 19.5f)*(27.0f + cam_yure) + VECTOR_ref::vget(GetRandf(cam_yure), GetRandf(cam_yure), GetRandf(cam_yure)), 0.9f);
									easing_set(&Cut_Pic[Cut].Aim_camera.camup, VECTOR_ref::vget(GetRandf(5.f) / 10.f + cam_yure / 50.f, 1.f, 0.f).Norm(), 0.95f);
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(45);
									Cut_Pic[Cut].cam_per = 0.975f;
								}
								models.Get(Tachyon2, 0)->UpdateAnim(1, true, 1.f);
							}
							SEL++;
							//8
							if (Cut == SEL) {
								if (isFirstLoop) {
									sode[7].isDraw = false;
									sode[6].isDraw = false;
									sode[5].isDraw = false;
									sode[4].isDraw = false;
								}
								if (models.Get(Tachyon2, 0)->obj.get_anime(0).per == 0) {
									SetScale_Effect(Effect::ef_reco, 3.f + cam_yure * 1.f / 3.f);
									SetSpeed_Effect(Effect::ef_reco, 1.05f);
									cam_yure += 5.f / GetFPS();
									easing_set(&Cut_Pic[Cut].Aim_camera.campos, Cut_Pic[Cut].Aim_camera.camvec + GetVector(6.9f, 180.f + 19.5f)*(27.0f + cam_yure) + VECTOR_ref::vget(GetRandf(cam_yure), GetRandf(cam_yure), GetRandf(cam_yure)), 0.9f);
									easing_set(&Cut_Pic[Cut].Aim_camera.camup, VECTOR_ref::vget(GetRandf(5.f) / 10.f + cam_yure / 50.f, 1.f, 0.f).Norm(), 0.95f);
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(45);
									Cut_Pic[Cut].cam_per = 0.975f;
								}
								models.Get(Tachyon2, 0)->UpdateAnim(1, true, 1.f);
							}
							SEL++;
							//9
							if (Cut == SEL) {
								if (isFirstLoop) {
									Stop_Effect(Effect::ef_reco);
									Board.isDraw = false;
									Map.isDraw = true;
									sky.isDraw = true;
									models.Get(Tachyon2, 0)->isDraw = false;
									models.Get(Tachyon, 0)->isDraw = true;
									models.Get(Tachyon, 0)->PhysicsSpeed = 0.01f;
									models.Get(Tachyon, 0)->obj.get_anime(2).time = 30.f;
									models.Get(Tachyon, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.f, 0.f)));
									models.Get(Tachyon, 0)->OpacityRate = 0.f;
								}
								if (models.Get(Tachyon, 0)->obj.get_anime(2).time > 32.f) {
									easing_set(&spd_takion, 0.01f, 0.9f);
									models.Get(Tachyon, 0)->OpacityRate = 1.f;
								}
								else {
									spd_takion = 0.6f;
									easing_set(&models.Get(Tachyon, 0)->OpacityRate, 1.f, 0.95f);
								}
								models.Get(Tachyon, 0)->UpdateAnim(2, true, spd_takion);
							}
							SEL++;
						}
						{}
						{
							//10
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
								if (models.Get(Tachyon, 0)->obj.get_anime(2).time > 32.f) {
									easing_set(&spd_takion, 0.01f, 0.9f);
									models.Get(Tachyon, 0)->OpacityRate = 1.f;
								}
								else {
									spd_takion = 0.6f;
									easing_set(&models.Get(Tachyon, 0)->OpacityRate, 1.f, 0.95f);
								}
								models.Get(Tachyon, 0)->UpdateAnim(2, true, spd_takion);
							}
							SEL++;
							//11
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
								if (models.Get(Tachyon, 0)->obj.get_anime(2).time > 32.f) {
									easing_set(&spd_takion, 0.01f, 0.9f);
									models.Get(Tachyon, 0)->OpacityRate = 1.f;
								}
								else {
									spd_takion = 0.6f;
									easing_set(&models.Get(Tachyon, 0)->OpacityRate, 1.f, 0.95f);
								}
								models.Get(Tachyon, 0)->UpdateAnim(2, true, spd_takion);
							}
							SEL++;
							//12
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
								if (models.Get(Tachyon, 0)->obj.get_anime(2).time > 32.f) {
									easing_set(&spd_takion, 0.01f, 0.9f);
									models.Get(Tachyon, 0)->OpacityRate = 1.f;
								}
								else {
									spd_takion = 0.6f;
									easing_set(&models.Get(Tachyon, 0)->OpacityRate, 1.f, 0.95f);
								}
								models.Get(Tachyon, 0)->UpdateAnim(2, true, spd_takion);
							}
							SEL++;
							//13
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
								if (NowTime > (LONGLONG)(1000000.f * 8.864f)) {
									models.Get(Tachyon, 0)->UpdateAnim(2, true, 0.6f);
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon, 0)->obj.frame(6);
									Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(17.178366f, 4.319146f, -36.016518f);
									Cut_Pic[Cut].cam_per = 0.9f;
									models.Get(Tachyon, 0)->PhysicsSpeed = 0.02f;
								}
								else {
									models.Get(Tachyon, 0)->UpdateAnim(2, true, 0.01f);
									models.Get(Tachyon, 0)->PhysicsSpeed = 0.01f;
								}
							}
							SEL++;
							//14
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Tachyon, 0)->isDraw = false;
									models.Get(Tachyon, 0)->PhysicsSpeed = 1.f;
									models.Get(Tachyon2, 0)->isDraw = true;
									models.Get(Tachyon2, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.f, 0.f)));
									models.Get(Tachyon2, 0)->obj.get_anime(3).time = 33.f;
									models.Get(Tachyon2, 0)->PhysicsSpeed = 0.6f;
								}
								models.Get(Tachyon2, 0)->UpdateAnim(3, true, 0.6f);
								Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon2, 0)->obj.frame(6);
							}
							SEL++;
							//15
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Tachyon, 1)->isDraw = true;
									models.Get(Tachyon, 1)->obj.get_anime(2).time = 33.f;
									models.Get(Tachyon, 1)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
									models.Get(Tachyon2, 0)->isDraw = false;
									models.Get(Tachyon2, 0)->PhysicsSpeed = 0.45f;
									radc = 0.f;
								}
								models.Get(Tachyon, 1)->UpdateAnim(2, true, 0.45f);
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, float(90 - 10) + radc)*88.0f;
								radc += 12.5f / GetFPS();
							}
							SEL++;
							//16
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Tachyon2, 0)->isDraw = true;
									models.Get(Tachyon2, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(-90 + 0))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -140)));
									models.Get(Tachyon2, 0)->obj.get_anime(3).time = 33.f;
									models.Get(Tachyon2, 0)->OpacityRate = 0.f;
									models.Get(Cafe, 0)->isDraw = true;
									models.Get(Cafe, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(-90 + 180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -140)));
									models.Get(Cafe, 0)->obj.get_anime(0).time = 33.f;
									models.Get(Cafe, 0)->OpacityRate = 0.f;
									models.Get(Cafe, 0)->PhysicsSpeed = 0.45f;
								}
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, float(90 - 10) + radc)*88.0f;
								radc += 12.5f / GetFPS();

								models.Get(Cafe, 0)->OpacityRate = std::min(models.Get(Cafe, 0)->OpacityRate + 2.f / GetFPS(), 1.f);

								models.Get(Tachyon, 1)->OpacityRate = std::max(models.Get(Tachyon, 1)->OpacityRate - 2.f / GetFPS(), 0.f);
								models.Get(Tachyon, 1)->UpdateAnim(2, true, 0.45f);
								models.Get(Tachyon2, 0)->OpacityRate = std::min(models.Get(Tachyon2, 0)->OpacityRate + 2.f / GetFPS(), 1.f);
								models.Get(Tachyon2, 0)->UpdateAnim(3, true, 0.45f);

								models.Get(Cafe, 0)->UpdateAnim(0, true, 0.45f);
							}
							SEL++;
							//17
							if (Cut == SEL) {
								if (isFirstLoop) {
									radc = 0.f;
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon2, 0)->obj.frame(9);
									Cut_Pic[Cut].cam_per = 0.f;
								}
								else {
									easing_set(&Cut_Pic[Cut].Aim_camera.camvec, models.Get(Tachyon2, 0)->obj.frame(9) + VECTOR_ref::vget(0.f, GetRandf(5.5f), GetRandf(13.5f)), 0.975f);
									Cut_Pic[Cut].cam_per = 0.975f;
								}
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-8.5f + xradadd_r * 1.5f, float(90 - 10) + radc)*38.0f;
								radc += 12.5f / GetFPS();

								models.Get(Cafe, 0)->OpacityRate = std::min(models.Get(Cafe, 0)->OpacityRate + 2.f / GetFPS(), 1.f);

								models.Get(Tachyon, 1)->OpacityRate = std::max(models.Get(Tachyon, 1)->OpacityRate - 2.f / GetFPS(), 0.f);
								models.Get(Tachyon, 1)->UpdateAnim(2, true, 0.45f);
								models.Get(Tachyon2, 0)->OpacityRate = std::min(models.Get(Tachyon2, 0)->OpacityRate + 2.f / GetFPS(), 1.f);
								models.Get(Tachyon2, 0)->UpdateAnim(3, true, 0.45f);

								models.Get(Cafe, 0)->UpdateAnim(0, true, 0.45f);
							}
							SEL++;
							//18
							if (Cut == SEL) {
								if (isFirstLoop) {
									radc = 0.f;
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Cafe, 0)->obj.frame(8);
									Cut_Pic[Cut].cam_per = 0.f;
								}
								else {
									Cut_Pic[Cut].cam_per = 0.975f;
								}
								easing_set(&Cut_Pic[Cut].Aim_camera.camvec, models.Get(Cafe, 0)->obj.frame(8) + VECTOR_ref::vget(0.f, GetRandf(5.5f), GetRandf(13.5f)), 0.975f);
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, float(90 + 180 - 10) + radc)*34.0f;
								radc += 12.5f / GetFPS();
								models.Get(Cafe, 0)->OpacityRate = std::min(models.Get(Cafe, 0)->OpacityRate + 2.f / GetFPS(), 1.f);

								models.Get(Tachyon, 1)->OpacityRate = std::max(models.Get(Tachyon, 1)->OpacityRate - 2.f / GetFPS(), 0.f);
								models.Get(Tachyon, 1)->UpdateAnim(2, true, 0.45f);
								models.Get(Tachyon2, 0)->OpacityRate = std::min(models.Get(Tachyon2, 0)->OpacityRate + 2.f / GetFPS(), 1.f);
								models.Get(Tachyon2, 0)->UpdateAnim(3, true, 0.45f);

								models.Get(Cafe, 0)->UpdateAnim(0, true, 0.45f);
							}
							SEL++;
							//19
							if (Cut == SEL) {
								if (isFirstLoop) {
									radc = 0.f;
								}
								Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon2, 0)->obj.frame(103);
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(25.5f + xradadd_r * 1.5f, float(90 - 10) + radc)*38.0f;
								radc += 12.5f / GetFPS();
								models.Get(Cafe, 0)->OpacityRate = std::min(models.Get(Cafe, 0)->OpacityRate + 2.f / GetFPS(), 1.f);

								models.Get(Tachyon, 1)->OpacityRate = std::max(models.Get(Tachyon, 1)->OpacityRate - 2.f / GetFPS(), 0.f);
								models.Get(Tachyon, 1)->UpdateAnim(2, true, 0.45f);
								models.Get(Tachyon2, 0)->OpacityRate = std::min(models.Get(Tachyon2, 0)->OpacityRate + 2.f / GetFPS(), 1.f);
								models.Get(Tachyon2, 0)->UpdateAnim(3, true, 0.45f);

								models.Get(Cafe, 0)->UpdateAnim(0, true, 0.45f);
							}
							SEL++;
						}
						{}
						{
							//20
							if (Cut == SEL) {
								if (isFirstLoop) {
									Map.isDraw = false;
									sky.isDraw = false;
									models.Get(Tachyon, 1)->isDraw = false;
									models.Get(Tachyon2, 0)->isDraw = false;
									models.Get(Cafe, 0)->isDraw = false;
									models.Get(Cafe, 0)->obj.get_anime(0).time = 0.f;
									for (int i = 0; i < 12; i++) {
										models.Get(NEWS, i)->move.mat = MATRIX_ref::RotY(deg2rad(GetRand(360)))*MATRIX_ref::RotX(deg2rad(GetRandf(30.f)));
										models.Get(NEWS, i)->move.pos = VECTOR_ref::vget(GetRandf(20.f), (float)(7 + GetRand(10)), GetRandf(20.f));
										models.Get(NEWS, i)->move.vec.y(-(float)(GetRand(100)) / 1000.f);
										models.Get(NEWS, i)->isDraw = true;
									}
									models.Get(Tachyon2, 0)->PhysicsSpeed = 1.f;
									models.Get(Cafe, 0)->PhysicsSpeed = 1.f;
								}
								for (int i = 0; i < 12; i++) {
									if (models.Get(NEWS, i)->move.pos.y() >= 3.5f) {
										models.Get(NEWS, i)->move.vec.yadd(M_GR / powf((FPS / 0.75f), 2.f));
									}
									else {
										float yp = models.Get(NEWS, i)->move.vec.y();
										easing_set(&yp, 0.f, 0.995f);
										models.Get(NEWS, i)->move.vec.y(yp);
									}
									models.Get(NEWS, i)->move.pos += models.Get(NEWS, i)->move.vec;
									models.Get(NEWS, i)->obj.SetMatrix(models.Get(NEWS, i)->move.mat*MATRIX_ref::Mtrans(models.Get(NEWS, i)->move.pos));
								}
							}
							SEL++;
							//21
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Tachyon, 2)->isDraw = true;
									models.Get(Tachyon, 2)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(-6))*MATRIX_ref::Mtrans(VECTOR_ref::vget(5, 1, -10)));
									models.Get(Tachyon, 3)->isDraw = true;
									models.Get(Tachyon, 3)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(5))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-3, 0, 10)));
									models.Get(Tachyon, 4)->isDraw = true;
									models.Get(Tachyon, 4)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(-25))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-18, 0, 30)));
									models.Get(Tachyon, 5)->isDraw = true;
									models.Get(Tachyon, 5)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(-30, 0.5, 40)));
									face.isDraw = true;
									for (auto&n : news_p) { n.isDraw = true; }
									for (int i = 0; i < 12; i++) { models.Get(NEWS, i)->isDraw = false; }
									Map.isDraw = true;
									sky.isDraw = true;
								}
								models.Get(Tachyon, 2)->UpdateAnim(4, false, 0.f);
								models.Get(Tachyon, 3)->UpdateAnim(5, false, 0.f);
								models.Get(Tachyon, 4)->UpdateAnim(6, false, 0.f);
								models.Get(Tachyon, 5)->UpdateAnim(7, false, 0.f);
								for (auto n = news_p.begin(); n != news_p.end() - 1; n++) {
									n->xpos = (n + 1)->xpos;
									n->ypos = (n + 1)->ypos;
									n->rad = (n + 1)->rad;
								}
								news_p.back().xpos += (int)((float)(DrawPts->disp_x / 2)*4.f / GetFPS());
								news_p.back().ypos += (int)((float)(DrawPts->disp_y / 2)*4.f / GetFPS());
								news_p.back().rad += deg2rad(6) / GetFPS();
							}
							SEL++;
							//22
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Tachyon, 2)->isDraw = false;
									models.Get(Tachyon, 3)->isDraw = false;
									models.Get(Tachyon, 4)->isDraw = false;
									models.Get(Tachyon, 5)->isDraw = false;
									models.Get(GoldShip, 0)->isDraw = true;
									models.Get(GoldShip, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(-6))*MATRIX_ref::Mtrans(VECTOR_ref::vget(5, -2, -10)));
									models.Get(Speweek, 0)->isDraw = true;
									models.Get(Speweek, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(5))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-3, 0, 10)));
									models.Get(Teio, 0)->isDraw = true;
									models.Get(Teio, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(-25))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-18, 0, 30)));
									models.Get(Macin, 0)->isDraw = true;
									models.Get(Macin, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(-30, 0.5, 40)));
								}
								for (auto n = news_p.begin(); n != news_p.end() - 1; n++) {
									n->xpos = (n + 1)->xpos;
									n->ypos = (n + 1)->ypos;
									n->rad = (n + 1)->rad;
								}
								news_p.back().xpos += (int)((float)(DrawPts->disp_x / 2)*4.f / GetFPS());
								news_p.back().ypos += (int)((float)(DrawPts->disp_y / 2)*4.f / GetFPS());
								news_p.back().rad += deg2rad(6) / GetFPS();
								if (news_p.back().ypos < DrawPts->disp_y / 2) {
									models.Get(GoldShip, 0)->UpdateAnim(0, false, 0.f);
								}
								else {
									models.Get(GoldShip, 0)->UpdateAnim(1, true, 0.45f);
								}
								models.Get(Speweek, 0)->UpdateAnim(0, false, 0.f);
								models.Get(Teio, 0)->UpdateAnim(0, false, 0.f);
								models.Get(Macin, 0)->UpdateAnim(0, false, 0.f);
							}
							SEL++;
							//23
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
								for (auto n = news_p.begin(); n != news_p.end() - 1; n++) {
									n->xpos = (n + 1)->xpos;
									n->ypos = (n + 1)->ypos;
									n->rad = (n + 1)->rad;
								}
								news_p.back().xpos += (int)((float)(DrawPts->disp_x / 2)*4.f / GetFPS());
								news_p.back().ypos += (int)((float)(DrawPts->disp_y / 2)*4.f / GetFPS());
								news_p.back().rad += deg2rad(6) / GetFPS();
								if (news_p.back().ypos < DrawPts->disp_y / 2) {
									models.Get(GoldShip, 0)->UpdateAnim(0, false, 0.f);
								}
								else {
									models.Get(GoldShip, 0)->UpdateAnim(1, true, 0.45f);
								}
								models.Get(Speweek, 0)->UpdateAnim(0, false, 0.f);
								models.Get(Teio, 0)->UpdateAnim(0, false, 0.f);
								models.Get(Macin, 0)->UpdateAnim(0, false, 0.f);
							}
							SEL++;
							//24
							if (Cut == SEL) {
								if (isFirstLoop) {
									face.isDraw = false;
									for (auto&n : news_p) { n.isDraw = false; }
									Map.isDraw = false;
									models.Get(GoldShip, 0)->isDraw = false;
									models.Get(Speweek, 0)->isDraw = false;
									models.Get(Teio, 0)->isDraw = false;
									models.Get(Macin, 0)->isDraw = false;
								}
							}
							SEL++;
							//25
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Tachyon, 0)->obj.get_anime(8).time = 33.f;
									models.Get(SCHOOL, 0)->isDraw = true;
									models.Get(Tachyon, 0)->isDraw = true;
									models.Get(Tachyon, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
									models.Get(Tachyon, 1)->isDraw = false;
									models.Get(Tachyon, 1)->OpacityRate = 1.f;
								}
								if (models.Get(Tachyon, 0)->obj.get_anime(8).time == models.Get(Tachyon, 0)->obj.get_anime(8).alltime) {
									if (!return_walk) {
										models.Get(Tachyon, 1)->isDraw = true;
										models.Get(Tachyon, 1)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
										models.Get(Tachyon, 1)->UpdateAnim(9, false, 0.f);
										models.Get(Tachyon, 1)->obj.work_anime();
										models.Get(Tachyon, 1)->obj.PhysicsResetState();
									}
									return_walk = true;
								}
								models.Get(Tachyon, 0)->UpdateAnim(8, false, return_walk ? -4.f : 4.f);
								if (return_walk) {
									models.Get(Tachyon, 1)->UpdateAnim(9, false, 0.7f);
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon, 1)->obj.frame(9);
									Cut_Pic[Cut].cam_per = 0.95f;
									Cut_Pic[Cut].Aim_camera.near_ = 5.f;
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(7);
									models.Get(Tachyon, 0)->PhysicsSpeed = 1.f;
								}
								else {
									models.Get(Tachyon, 0)->PhysicsSpeed = 4.f;
								}
								radc = 0.f;
							}
							SEL++;
							//26
							if (Cut == SEL) {
								if (isFirstLoop) {
									Map.isDraw = true;
									models.Get(SCHOOL, 0)->isDraw = false;
									models.Get(GATE, 0)->isDraw = true;
									models.Get(GATE, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.25f, 0.f)));
									models.Get(Tachyon, 0)->isDraw = false;
									models.Get(Tachyon, 0)->PhysicsSpeed = 1.f;
									models.Get(Tachyon, 1)->isDraw = false;
									return_walk = true;
								}
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, float(90 - 10) + radc)*88.0f;
								radc += 12.5f / GetFPS();
							}
							SEL++;
							//27
							if (Cut == SEL) {
								if (isFirstLoop) {
									Gate_Xpos = -20.f;
									Cut_Pic[Cut].cam_per = 0.f;
								}
								else {
									float pp = 7.f;
									easing_set(&rand_vp, VECTOR_ref::vget(GetRandf(pp), GetRandf(pp), GetRandf(pp)), 0.9f);
									Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(10.122698f, 10.028077f, -50.985756f) + rand_vp;
									Cut_Pic[Cut].cam_per = 0.95f;
								}
								models.Get(GATE, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(Gate_Xpos, 0.25f, 0.f)));
								if (Gate_Xpos < 20.f) {
									Gate_Xpos += 20.f *1.f / GetFPS();
								}
							}
							SEL++;
							//28
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(GATE, 0)->isDraw = false;
									models.Get(Karen, 0)->isDraw = true;
									models.Get(Karen, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(2, 0, 0.f)));
									models.Get(Karen, 0)->obj.get_anime(0).time = 6.25f;
									Ship.isDraw = true;
									Ship.OpacityRate = 0.3f;
									Ship.obj.SetScale(VECTOR_ref::vget(1.f, 1.f, 1.f)*0.01f);
									ship_z = -5.f;
									ship_zadd = 0.f;
									camzb_28 = 0.f;
								}
								else {
									Cut_Pic[Cut].cam_per = (camzb_28 > -8.f) ? 0.9f : 0.95f;
								}
								models.Get(Karen, 0)->UpdateAnim(0, false, 1.0f);
								Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(0, 0, camzb_28);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + VECTOR_ref::vget(2.f, 12.f, 6.f)*1.0f;
								camzb_28 = std::max(camzb_28 - 100.f / GetFPS(), -10.f);
								Ship.obj.SetPosition(camera_main.camvec + VECTOR_ref::up()*3.f + VECTOR_ref::right()*-2.f + VECTOR_ref::front()*-1.f*ship_z);
								ship_z += ship_zadd / GetFPS();
								if (ship_zadd <= 0.95f) {
									easing_set(&ship_zadd, 1.f, 0.95f);
								}
								else {
									ship_zadd = 1.f;
								}
							}
							SEL++;
							//29
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
								models.Get(Karen, 0)->UpdateAnim(0, false, 1.0f);
								Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(0, 0, camzb_28);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + VECTOR_ref::vget(2.f, 12.f, 6.f)*1.0f;
								if (camzb_28 != 0.f) {
									Cut_Pic[Cut].cam_per = (camzb_28 > -8.f) ? 0.9f : 0.95f;
								}
								camzb_28 = std::max(camzb_28 - 100.f / GetFPS(), -10.f);
								Ship.obj.SetPosition(camera_main.camvec + VECTOR_ref::up()*3.f + VECTOR_ref::right()*-2.f + VECTOR_ref::front()*-1.f*ship_z);
								ship_z += ship_zadd / GetFPS();
								if (ship_zadd <= 0.95f) {
									easing_set(&ship_zadd, 1.f, 0.95f);
								}
								else {
									ship_zadd = 1.f;
								}
							}
							SEL++;
						}
						{}
						{
							//30
							if (Cut == SEL) {
								if (isFirstLoop) {
									Board.isDraw = true;
									Ship.isDraw = false;
									Map.isDraw = false;
									sky.isDraw = false;
									models.Get(Tachyon, 0)->isDraw = false;
									models.Get(Karen, 0)->isDraw = false;
									models.Get(Cafe, 0)->isDraw = true;
									models.Get(Cafe, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
								}
								models.Get(Cafe, 0)->UpdateAnim(0, true, 0.f);
							}
							SEL++;
							//31
							if (Cut == SEL) {
								if (isFirstLoop) {
									Board.isDraw = false;
									models.Get(Tachyon, 0)->isDraw = true;
									models.Get(Scarlet, 0)->OpacityRate = 0.5f;
								}
								models.Get(Cafe, 0)->UpdateAnim(1, true, 0.f);
							}
							SEL++;
							//32
							if (Cut == SEL) {
								if (isFirstLoop) {
									Map.isDraw = true;
									sky.isDraw = true;
									models.Get(Tachyon, 0)->isDraw = false;
									models.Get(Tachyon2, 0)->isDraw = true;
									models.Get(Tachyon2, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -14)));
									models.Get(Cafe, 0)->isDraw = false;
									models.Get(Scarlet, 0)->isDraw = true;
									models.Get(Scarlet, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
								}
								models.Get(Scarlet, 0)->UpdateAnim(0, false, 0.6f);
								if (models.Get(Scarlet, 0)->obj.get_anime(0).time > 10.f) {
									easing_set(&models.Get(Scarlet, 0)->OpacityRate, 1.f, 0.95f);
								}
								models.Get(Tachyon2, 0)->UpdateAnim(10, false, 0.6f);
							}
							SEL++;
							//33
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Vodka, 0)->isDraw = true;
									models.Get(Vodka, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -50)));
									models.Get(Vodka, 0)->UpdateAnim(0, false, 1.f);
								}
							}
							SEL++;
							//34
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Vodka, 0)->isDraw = false;
								}
							}
							SEL++;
							//35
							if (Cut == SEL) {
								if (isFirstLoop) {
									Map.isDraw = false;
									sky.isDraw = false;
									models.Get(Scarlet, 0)->isDraw = false;
									models.Get(Tachyon2, 0)->isDraw = false;
									models.Get(Cafe, 0)->isDraw = true;
									models.Get(Cafe, 0)->OpacityRate = 1.f;
								}
								models.Get(Cafe, 0)->UpdateAnim(2, false, 0.6f);
							}
							SEL++;
							//36
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Tachyon, 0)->isDraw = true;
									models.Get(Tachyon, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(270))*MATRIX_ref::Mtrans(VECTOR_ref::vget(5.f, 1.6f, -10.f)));
								}
								models.Get(Cafe, 0)->UpdateAnim(2, false, 0.6f);
								models.Get(Cafe, 0)->OpacityRate = std::max(models.Get(Cafe, 0)->OpacityRate - 2.f / GetFPS(), 0.f);
								models.Get(Tachyon, 0)->UpdateAnim(11, false, 0.5f);
								models.Get(Tachyon, 0)->OpacityRate = std::min(models.Get(Tachyon, 0)->OpacityRate + 2.f / GetFPS(), 1.f);
							}
							SEL++;
							//37
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Tachyon, 0)->isDraw = false;
									models.Get(Cafe, 0)->isDraw = false;
									models.Get(Trainer, 0)->isDraw = true;
								}
								models.Get(Trainer, 0)->UpdateAnim(0, false, 1.2f);
							}
							SEL++;
							//38
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Trainer, 0)->isDraw = false;
									models.Get(Tachyon, 0)->isDraw = true;
									models.Get(Tachyon, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(270))*MATRIX_ref::Mtrans(VECTOR_ref::vget(5.f, 1.6f, -10.f)));
								}
								models.Get(Tachyon, 0)->UpdateAnim(12, false, 0.5f);
							}
							SEL++;
							//39
							if (Cut == SEL) {
								if (isFirstLoop) {
									Map.isDraw = true;
									sky.isDraw = true;
									models.Get(Tachyon, 0)->isDraw = false;
									models.Get(Tachyon2, 0)->isDraw = true;
									models.Get(Tachyon2, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
								}
								models.Get(Tachyon2, 0)->UpdateAnim(13, false, 1.2f);
							}
							SEL++;
						}
						{}
						{
							//40
							if (Cut == SEL) {
								if (isFirstLoop) {
									camzb_28 = 0.f;
									camzb_41 = 0.1f;
									models.Get(Tachyon2, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
								}
								models.Get(Tachyon2, 0)->UpdateAnim(14, false, 2.8f);
								for (int i = 0; i < 12; i++) {
									models.Get(Mobu, i)->UpdateAnim(models.Get(Mobu, i)->Anim_Sel, false, 0.f);
								}
							}
							SEL++;
							//41
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(GATE, 0)->isDraw = true;
									for (int i = 0; i < 12; i++) {
										models.Get(Mobu, i)->isDraw = true;
									}
									models.Get(Tachyon2, 0)->isDraw = false;
								}
								Gate_Xpos = 0.f;
								models.Get(GATE, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(Gate_Xpos, 0.25f, 0.f)));
								models.Get(GATE, 0)->UpdateAnim(0, false, 0.f);
								Gate_Xpos += -7.f;
								for (int i = 0; i < 12; i++) {
									models.Get(Mobu, i)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(Gate_Xpos, 0.25f, -10.f)));
									Gate_Xpos += -14.f;
								}
								float pp = 10.f;
								if (camzb_28 < -9.5f) {
									pp = 2.f;
								}
								if (camzb_28 <= -10.f) {
									pp = 0.f;
								}
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(0.f, 30.f)*(20.f - camzb_28 * 20.8f) + VECTOR_ref::vget(GetRandf(pp), GetRandf(pp), GetRandf(pp));
								Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(camzb_41, 0.9f, 0);
								easing_set(&camzb_41, 0.3f, 0.995f);
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
							SEL++;
							//42
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
								float pp = 10.f;
								if (camzb_28 > -5.f) {
									pp = 2.f;
								}
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(0.f, 30.f)*(10.f - camzb_28 * 20.8f) + VECTOR_ref::vget(GetRandf(pp), GetRandf(pp), GetRandf(pp));
								Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::vget(camzb_41, 0.9f, 0);
								easing_set(&camzb_41, 0.3f, 0.995f);
								HostpassPTs->Set_Bright(226 + (int)((float)(255 - 226)*bright_43), 226 + (int)((float)(255 - 226)*bright_43), 196 + (int)((float)(255 - 196)*bright_43));
								easing_set(&bright_43, 1.f, 0.9f);
								if (camzb_28 < 0.f) {
									camzb_28 += 70.f / GetFPS();
								}
								else {
									camzb_28 = 0.f;
								}
							}
							SEL++;
							//43
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-20.f, -10.f + camzb_28)*20.f;
								camzb_28 += 40.f / GetFPS();
							}
							SEL++;
							//44
							if (Cut == SEL) {
								if (isFirstLoop) {
									camyup = 0.f;
								}
								if (NowTime > (LONGLONG)(1000000.f * 49.561)) {
									models.Get(GATE, 0)->UpdateAnim(1, false, 1.f);
									Gate_Xpos = 0.f;
									Gate_Xpos += -7.f;
									for (int i = 0; i < 12; i++) {
										//models.Get(Mobu, i)->
										models.Get(Mobu, i)->isDraw = true;
										easing_set(&models.Get(Mobu, i)->obj.get_anime(models.Get(Mobu, i)->Anim_Sel).per, 0.f, models.Get(Mobu, i)->b_run);
										easing_set(&models.Get(Mobu, i)->obj.get_anime(3).per, 1.f, models.Get(Mobu, i)->b_run);

										models.Get(Mobu, i)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(Gate_Xpos, 0.25f, -10.f - models.Get(Mobu, i)->b_runrange * models.Get(Mobu, i)->obj.get_anime(3).per)));
										Gate_Xpos += -14.f;
									}
								}
								if (NowTime > (LONGLONG)(1000000.f * 49.681)) {
									Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(-39.67f, 5.48f, -51.8f);
									Cut_Pic[Cut].Aim_camera.camvec = Cut_Pic[Cut].Aim_camera.campos + VECTOR_ref::vget(1, -0.3f, 0.6f);
									Cut_Pic[Cut].cam_per = 0.9f;
								}
								else {
									Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(-40.67f, 5.53f, -42.f);
									Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + VECTOR_ref::vget(1, -0.1f - 0.1f*camyup, -0.13f);
									camyup += 1.f / GetFPS();
									Cut_Pic[Cut].cam_per = 0.f;
								}
							}
							SEL++;
							//45
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Tachyon2, 0)->isDraw = false;
									Board.isDraw = true;
									Map.isDraw = false;
									sky.isDraw = false;
									models.Get(GATE, 0)->isDraw = false;
									for (int i = 0; i < 12; i++) {
										models.Get(Mobu, i)->isDraw = false;
									}
									anim[0].isDraw = true;
									VECTOR_ref vec_x = (Cut_Pic[Cut].Aim_camera.camvec - Cut_Pic[Cut].Aim_camera.campos).Norm().cross(Cut_Pic[Cut].Aim_camera.camup);
									Set_Effect(Effect::ef_greexp, VECTOR_ref::vget(-4.f, 4.6f, -2.f) + vec_x * 13.f, VECTOR_ref::front(), 5.f);
									Set_Effect(Effect::ef_greexp2, VECTOR_ref::vget(8.f, 5.5f, 4.f) - vec_x * 8.f, VECTOR_ref::front(), 4.f);
								}
								SetSpeed_Effect(Effect::ef_greexp, 0.5f);
								SetSpeed_Effect(Effect::ef_greexp2, 0.5f);
							}
							SEL++;
							//46
							if (Cut == SEL) {
								if (isFirstLoop) {
									sode[3].isDraw = true;
									sode[2].isDraw = true;
									sode[1].isDraw = true;
									sode[0].isDraw = true;
									sode[3].Set_Base((float)(DrawPts->disp_x / 2) + (float)(-DrawPts->disp_x / 5), 0, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 0, 1.f);
									sode[2].Set_Base((float)(DrawPts->disp_x / 2) + (float)(DrawPts->disp_x / 5), 0, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 0, 1.f);
									sode[1].Set_Base((float)(DrawPts->disp_x / 2) + (float)(-DrawPts->disp_x / 5), 0, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 0, 1.f);
									sode[0].Set_Base((float)(DrawPts->disp_x / 2) + (float)(DrawPts->disp_x / 5), 0, (float)(DrawPts->disp_y / 2) + (float)(DrawPts->disp_y), 0, 0, 1.f);
								}
								sode[3].Set_Base(sode[3].xpos_base + (float)(y_r(300)) / GetFPS(), 0, sode[3].ypos_base - (float)(y_r(2800)) / GetFPS(), 0, 0, 1.f);
								sode[2].Set_Base(sode[2].xpos_base - (float)(y_r(300)) / GetFPS(), 0, sode[2].ypos_base - (float)(y_r(2400)) / GetFPS(), 0, 0, 1.f);
								sode[1].Set_Base(sode[1].xpos_base + (float)(y_r(300)) / GetFPS(), 0, sode[1].ypos_base - (float)(y_r(2200)) / GetFPS(), 0, 0, 1.f);
								sode[0].Set_Base(sode[0].xpos_base - (float)(y_r(300)) / GetFPS(), 0, sode[0].ypos_base - (float)(y_r(2000)) / GetFPS(), 0, 0, 1.f);
							}
							SEL++;
							//47
							if (Cut == SEL) {
								if (isFirstLoop) {
									camzb_41 = (float)(DrawPts->disp_y)*0.4f;
									SetSpeed_Effect(Effect::ef_greexp, 0.1f);
									SetSpeed_Effect(Effect::ef_greexp2, 0.1f);
									anim[0].isDraw = false;
									anim[1].isDraw = true;
								}
								Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(48.211510f, 15.f - 1.5f*(anim[1].ypos - (float)(DrawPts->disp_y) / 2.f) / (float)(DrawPts->disp_y), 0.595667f);
								Cut_Pic[Cut].Aim_camera.camvec = Cut_Pic[Cut].Aim_camera.campos + VECTOR_ref::vget(-1.f, 0.3f, 0.f)*(1.f);
								easing_set(&anim[1].ypos, (float)(DrawPts->disp_y) / 2.f + camzb_41, 0.7f);
								if (camzb_41 > 0.f) {
									camzb_41 -= (float)(y_r(500)) / GetFPS();
								}
								sode[3].Set_Base(sode[3].xpos_base + (float)(y_r(300)) / GetFPS(), 0, sode[3].ypos_base - (float)(y_r(2800)) / GetFPS(), 0, 0, 1.f);
								sode[2].Set_Base(sode[2].xpos_base - (float)(y_r(300)) / GetFPS(), 0, sode[2].ypos_base - (float)(y_r(2400)) / GetFPS(), 0, 0, 1.f);
								sode[1].Set_Base(sode[1].xpos_base + (float)(y_r(300)) / GetFPS(), 0, sode[1].ypos_base - (float)(y_r(2200)) / GetFPS(), 0, 0, 1.f);
								sode[0].Set_Base(sode[0].xpos_base - (float)(y_r(300)) / GetFPS(), 0, sode[0].ypos_base - (float)(y_r(2000)) / GetFPS(), 0, 0, 1.f);
							}
							SEL++;
							//48
							if (Cut == SEL) {
								if (isFirstLoop) {
									anim[0].ypos = (float)(DrawPts->disp_y) / 2.f;
									sode[3].Set_Base((float)(DrawPts->disp_x / 2) + (float)(0), 0, (float)(DrawPts->disp_y / 2) + (float)(-DrawPts->disp_y), 0, 0, 1.f);
									sode[2].Set_Base((float)(DrawPts->disp_x / 2) + (float)(0), 0, (float)(DrawPts->disp_y / 2) + (float)(-DrawPts->disp_y * 5 / 4), 0, 0, 1.f);
									sode[1].Set_Base((float)(DrawPts->disp_x / 2) + (float)(0), 0, (float)(DrawPts->disp_y / 2) + (float)(-DrawPts->disp_y * 6 / 4), 0, 0, 1.f);
									sode[0].Set_Base((float)(DrawPts->disp_x / 2) + (float)(0), 0, (float)(DrawPts->disp_y / 2) + (float)(-DrawPts->disp_y * 7 / 4), 0, 0, 1.f);
								}
								Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(48.211510f, 15.f - 1.5f*(anim[1].ypos - (float)(DrawPts->disp_y) / 2.f) / (float)(DrawPts->disp_y), 0.595667f);
								Cut_Pic[Cut].Aim_camera.camvec = Cut_Pic[Cut].Aim_camera.campos + VECTOR_ref::vget(-1.f, 0.3f, 0.f)*(1.f);
								easing_set(&anim[1].ypos, (float)(DrawPts->disp_y) / 2.f + camzb_41, 0.7f);
								if (camzb_41 > 0.f) {
									camzb_41 -= (float)(y_r(500)) / GetFPS();
								}
								sode[3].Set_Base(sode[3].xpos_base - (float)(y_r(300)) / GetFPS(), 0, sode[3].ypos_base + (float)(y_r(2800)) / GetFPS(), 0, 0, 1.f);
								sode[2].Set_Base(sode[2].xpos_base + (float)(y_r(300)) / GetFPS(), 0, sode[2].ypos_base + (float)(y_r(2400)) / GetFPS(), 0, 0, 1.f);
								sode[1].Set_Base(sode[1].xpos_base - (float)(y_r(300)) / GetFPS(), 0, sode[1].ypos_base + (float)(y_r(2200)) / GetFPS(), 0, 0, 1.f);
								sode[0].Set_Base(sode[0].xpos_base + (float)(y_r(300)) / GetFPS(), 0, sode[0].ypos_base + (float)(y_r(2000)) / GetFPS(), 0, 0, 1.f);
							}
							SEL++;
							//49
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Tachyon2, 0)->isDraw = true;
									models.Get(Tachyon2, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(-10, 0, 30.f)));
									models.Get(Tachyon2, 0)->obj.get_anime(2).time = 35.f;
									models.Get(Karen, 0)->isDraw = false;
									anim[1].isDraw = false;
								}
								VECTOR_ref aim = models.Get(Tachyon2, 0)->obj.frame(2);
								aim.y(0.f);
								Cut_Pic[Cut].Aim_camera.campos = VECTOR_ref::vget(32.211510f, 8.997231f, -18.595667f) + aim;
								Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(-0.828184f, 16.546323f, -0.092557f) + aim;
								models.Get(Tachyon2, 0)->UpdateAnim(2, false, 0.1f);
								sode[3].Set_Base(sode[3].xpos_base - (float)(y_r(300)) / GetFPS(), 0, sode[3].ypos_base + (float)(y_r(2800)) / GetFPS(), 0, 0, 1.f);
								sode[2].Set_Base(sode[2].xpos_base + (float)(y_r(300)) / GetFPS(), 0, sode[2].ypos_base + (float)(y_r(2400)) / GetFPS(), 0, 0, 1.f);
								sode[1].Set_Base(sode[1].xpos_base - (float)(y_r(300)) / GetFPS(), 0, sode[1].ypos_base + (float)(y_r(2200)) / GetFPS(), 0, 0, 1.f);
								sode[0].Set_Base(sode[0].xpos_base + (float)(y_r(300)) / GetFPS(), 0, sode[0].ypos_base + (float)(y_r(2000)) / GetFPS(), 0, 0, 1.f);
							}
							SEL++;
						}
						{}
						{
							//50
							if (Cut == SEL) {
								if (isFirstLoop) {
									sode[3].isDraw = false;
									sode[2].isDraw = false;
									sode[1].isDraw = false;
									sode[0].isDraw = false;
									sky.isDraw = true;
									Board.isDraw = false;
									Map.isDraw = true;
									Stop_Effect(Effect::ef_greexp);
									Stop_Effect(Effect::ef_greexp2);
									models.Get(Tachyon2, 0)->isDraw = false;
									models.Get(Karen, 0)->isDraw = true;
									models.Get(Karen, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(2, 0, 0.f)));
								}
								else {
									Cut_Pic[Cut].cam_per = 0.995f;
								}
								models.Get(Karen, 0)->UpdateAnim(1, false, 0.95f);
								if (!isFirstLoop) {
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Karen, 0)->obj.frame(8);
								}
							}
							SEL++;
							//51
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Karen, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(2, 0, 0.f)));
								}
								models.Get(Karen, 0)->UpdateAnim(2, false, 1.15f);
								Cut_Pic[Cut].Aim_camera.camvec = models.Get(Karen, 0)->obj.frame(10) + VECTOR_ref::vget(2.3f, 0.f, 0.f);
							}
							SEL++;
							//52
							if (Cut == SEL) {
								if (isFirstLoop) {
									Board.isDraw = true;
									Map.isDraw = false;
									sky.isDraw = false;
									models.Get(Karen, 0)->isDraw = false;
									models.Get(Cafe, 0)->isDraw = true;
									models.Get(Cafe, 0)->OpacityRate = 1.f;
									models.Get(Cafe, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, 0)));
								}
								models.Get(Cafe, 0)->UpdateAnim(3, false, 0.8f);
							}
							SEL++;
							//53
							if (Cut == SEL) {
								if (isFirstLoop) {
								}
								if (models.Get(Cafe, 0)->obj.get_anime(3).time > 30.f) {
									Cut_Pic[Cut].Aim_camera.camvec = VECTOR_ref::vget(0, 17, -4.5);
									Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + VECTOR_ref::vget(-5.f, -3.5f, -20.f)*2.0f;
									Cut_Pic[Cut].Aim_camera.camup = VECTOR_ref::up();
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(25);
									Cut_Pic[Cut].Aim_camera.near_ = 1.f;
									Cut_Pic[Cut].Aim_camera.far_ = 5000.f;
									Cut_Pic[Cut].cam_per = 0.9975f;
								}
								else {
									if (models.Get(Cafe, 0)->obj.get_anime(3).time > 15.f) {
										Cut_Pic[Cut].cam_per = 0.f;
									}
									else {
										Cut_Pic[Cut].cam_per = 1.f;
									}
								}
								models.Get(Cafe, 0)->UpdateAnim(3, false, 0.8f);
							}
							SEL++;
							//54
							if (Cut == SEL) {
								if (isFirstLoop) {
									sky.isDraw = true;
									Board.isDraw = false;
									Map.isDraw = true;
									models.Get(GATE, 0)->isDraw = true;

									models.Get(Mobu, 0)->isDraw = true;
									models.Get(Mobu, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.f, -2000.f - 25.f)));
									models.Get(Mobu, 0)->obj.get_anime(4).time = (float)GetRand(100) / 10.f;

									models.Get(Doto, 0)->isDraw = true;
									models.Get(Doto, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(10.f, 0.f, -2000.f - 5.f)));
									models.Get(Doto, 0)->obj.get_anime(0).time = 30.f + (float)GetRand(100) / 10.f;

									models.Get(Cafe, 0)->isDraw = true;
									models.Get(Cafe, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(0.f, 0.f, -2000.f - 30.f)));
									models.Get(Cafe, 0)->obj.get_anime(4).time = (float)GetRand(100) / 10.f;

									models.Get(Opera, 0)->isDraw = true;
									models.Get(Opera, 0)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(-20.f, 0.f, -2000.f + 10.f)));
									models.Get(Opera, 0)->obj.get_anime(0).time = (float)GetRand(100) / 10.f;

									models.Get(Mobu, 1)->isDraw = true;
									models.Get(Mobu, 1)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(5.f, 0.f, -2000.f - 5.f)));
									models.Get(Mobu, 1)->obj.get_anime(4).time = (float)GetRand(100) / 10.f;

									models.Get(Mobu, 2)->isDraw = true;
									models.Get(Mobu, 2)->obj.SetMatrix(MATRIX_ref::Mtrans(VECTOR_ref::vget(20.f, 0.f, -2000.f - 40.f)));
									models.Get(Mobu, 2)->obj.get_anime(4).time = (float)GetRand(100) / 10.f;

									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Cafe, 0)->obj.frame(7 + GetRand(3)) + VECTOR_ref::vget(0.f, 2.f, -30.f);
									Cut_Pic[Cut].cam_per = 0.f;
								}
								else {
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Cafe, 0)->obj.frame(8) + VECTOR_ref::vget(0.f, 2.f, -30.f);
									Cut_Pic[Cut].cam_per = 0.975f;
								}
								models.Get(Mobu, 0)->UpdateAnim(4, false, 0.84f);
								models.Get(Mobu, 2)->UpdateAnim(4, false, 0.8f);
								models.Get(Doto, 0)->UpdateAnim(0, false, 0.82f);
								models.Get(Cafe, 0)->UpdateAnim(4, false, 0.85f);
								models.Get(Mobu, 1)->UpdateAnim(4, false, 0.82f);
								models.Get(Opera, 0)->UpdateAnim(0, false, 0.84f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(0.5f, float(90 - 10 + 180))*608.0f;
							}
							SEL++;
							//55
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Mobu, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-2394.f*1.95f + 5.f, 0.f, -2200.f - 45.f)));
									models.Get(Mobu, 1)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-2394.f*1.95f - 7.f, 0.f, -2200.f - 70.f)));
									models.Get(Mobu, 2)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-2394.f*1.95f + 10.f, 0.f, -2200.f - 65.f)));
									models.Get(Doto, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-2394.f*1.95f - 6.f, 0.f, -2200.f - 35.f)));
									models.Get(Cafe, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-2394.f*1.95f - 5.f, 0.f, -2200.f - 0.f)));
									models.Get(Opera, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(180))*MATRIX_ref::Mtrans(VECTOR_ref::vget(-2394.f*1.95f - 12.5f, 0.f, -2200.f - 40.f)));
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Cafe, 0)->obj.frame(8) + VECTOR_ref::vget(0.f, 10.f, 30.f);
									Cut_Pic[Cut].cam_per = 0.f;
								}
								else {
									Cut_Pic[Cut].Aim_camera.camvec = models.Get(Cafe, 0)->obj.frame(8) + VECTOR_ref::vget(0.f, 0.3f, 20.f);
									Cut_Pic[Cut].cam_per = 0.95f;
								}
								models.Get(Mobu, 0)->UpdateAnim(4, false, 0.8f);
								models.Get(Mobu, 1)->UpdateAnim(4, false, 0.8f);
								models.Get(Mobu, 2)->UpdateAnim(4, false, 0.8f);
								models.Get(Doto, 0)->UpdateAnim(0, false, 1.1f);
								models.Get(Cafe, 0)->UpdateAnim(4, false, 0.98f);
								models.Get(Opera, 0)->UpdateAnim(0, false, 0.95f);
								Cut_Pic[Cut].Aim_camera.campos = models.Get(Cafe, 0)->obj.frame(8) + VECTOR_ref::vget(0.f, 0.3f, 20.f) + GetVector(1.5f, float(90 - 80 + 180))*468.0f;
								if (NowTime > (LONGLONG)(1000000.f * 62.755)) {
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(0.85f);
								}
								else {
									Cut_Pic[Cut].Aim_camera.fov = deg2rad(2);
								}
							}
							SEL++;
							//56
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(Mobu, 0)->isDraw = false;
									models.Get(Mobu, 1)->isDraw = false;
									models.Get(Mobu, 2)->isDraw = false;
									models.Get(Doto, 0)->isDraw = false;
									models.Get(Opera, 0)->isDraw = false;
									models.Get(Tachyon, 0)->isDraw = true;
									models.Get(Tachyon, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(0 + 0))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -140)));
									radc = 0.f;
								}
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, float(90 - 10) + radc)*88.0f;
								radc += 12.5f / GetFPS();
								models.Get(Tachyon, 0)->UpdateAnim(15, false, 0.2f);
							}
							SEL++;
							//57
							if (Cut == SEL) {
								if (isFirstLoop) {
									models.Get(GATE, 0)->isDraw = false;
									models.Get(Tachyon, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(180 + 0))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -140)));
								}
								Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon, 0)->obj.frame(9);
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, float(90 - 10) + radc)*88.0f;
								radc += 12.5f / GetFPS();
								models.Get(Tachyon, 0)->UpdateAnim(15, false, 0.75f);
							}
							SEL++;
							//58
							if (Cut == SEL) {
								if (isFirstLoop) {
									Box_ALPHA = 0.f;
									models.Get(Tachyon, 0)->obj.SetMatrix(MATRIX_ref::RotY(deg2rad(180 + 0))*MATRIX_ref::Mtrans(VECTOR_ref::vget(0, 0, -140)));
									for (auto& sl : sode_last) {
										sl.isDraw = true;
									}
								}
								Cut_Pic[Cut].Aim_camera.camvec = models.Get(Tachyon, 0)->obj.frame(9);
								easing_set(&xradadd_r, GetRandf(2.f), 0.975f);
								Cut_Pic[Cut].Aim_camera.campos = Cut_Pic[Cut].Aim_camera.camvec + GetVector(-5.5f + xradadd_r * 1.5f, float(90 - 10) + radc)*88.0f;
								radc += 12.5f / GetFPS();
								models.Get(Tachyon, 0)->UpdateAnim(15, false, 0.75f);
								if (Box_ALPHA < 1.f) {
									Box_ALPHA += 1.f / GetFPS();
									per_logo = 1.f;
								}
								else {
									Box_ALPHA = 1.f;
									if (per_logo > 0.f) {
										per_logo -= 1.f / GetFPS() / 1.5f;
										Logo.Update_Rand(0.975f, 0.975f, 1.f, 0.975f);
										Logo.Set_Rand((float)(y_r(1000)) * per_logo, 0.8f, (float)(y_r(1000)) * per_logo, 0.8f, 0.f, 0.f, 0.1f * per_logo, 0.f);
									}
									else {
										per_logo = 0.f;
										Logo.Update_Rand(0.95f, 0.95f, 1.f, 0.95f);
										Logo.Set_Rand(0.f, 0.f, 0.f, 0.f, 10.f, 0.95f, 0.f, 0.f);
									}
									Logo.isDraw = (int)(per_logo*100.f) <= GetRand(99);
								}
								for (auto& sl : sode_last) {
									sl.Update_Rand(0.945f, 0.925f, 0.925f, 1.f);
									sl.Set_Rand((float)(y_r(50)), 0.95f, (float)(y_r(50)), 0.95f, 10.f, 0.95f, 0.f, 0.f);
									int per_b = 127 + (int)(128.f* (&sl - &sode_last.front()) / sode_last.size());
									sl.SetBright(per_b, per_b, per_b);
								}
							}
						}
						//
						for (auto& m_sode : sode) {
							m_sode.Update_Rand(m_sode.x_per, m_sode.y_per, 0.f, 0.f);
						}
						if (Cut_Pic[Cut].isNextreset) {
							isNextreset = true;
						}
						easing_set(&camera_main.campos, Cut_Pic[Cut].Aim_camera.campos, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.camvec, Cut_Pic[Cut].Aim_camera.camvec, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.camup, Cut_Pic[Cut].Aim_camera.camup, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.fov, Cut_Pic[Cut].Aim_camera.fov, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.far_, Cut_Pic[Cut].Aim_camera.far_, Cut_Pic[Cut].cam_per);
						easing_set(&camera_main.near_, Cut_Pic[Cut].Aim_camera.near_, Cut_Pic[Cut].cam_per);
						camera_main.set_cam_info(camera_main.fov, camera_main.near_, camera_main.far_);
					}
					models.Update();
					Ship.Update();
					if (!Time_Over()) {
						isFirstLoop = false;
						if (NowTime > Cut_Pic[Cut%Cut_Pic.size()].TIME) {
							++Cut;
							isFirstLoop = true;
							if (isNextreset) {
								reset_p = true;
								isNextreset = false;
							}
						}
						else {
							models.Get(Tachyon, 0)->SetPhysics(reset_p);
							models.Get(Tachyon, 1)->SetPhysics(reset_p);
							models.Get(Tachyon2, 0)->SetPhysics(reset_p);
							models.Get(Cafe, 0)->SetPhysics(reset_p);
							models.Get(Scarlet, 0)->SetPhysics(reset_p);
							models.Get(Vodka, 0)->SetPhysics(reset_p);
							models.Get(Karen, 0)->SetPhysics(reset_p);
							models.Get(Opera, 0)->SetPhysics(reset_p);
							models.Get(Doto, 0)->SetPhysics(reset_p);
							reset_p = false;
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
				for (auto& p : Cut_Pic) { p.Dispose(); }
				Cut_Pic.clear();
				//BGM.Dispose();
				//movie.Dispose();
			}
			void UI_Draw(void) noexcept  override {
				//printfDx("Cut : %d\n", Cut);
				const auto NowTime = GetNowHiPerformanceCount() - BaseTime;
				for (auto& t : Texts) {
					t.Draw(NowTime);
				}
				//
			}
			void BG_Draw(void) noexcept override {
				if (!Time_Over()) {
					if (sky.isDraw) {
						auto fog_e = GetFogEnable();
						SetFogEnable(FALSE);
						SetUseLighting(FALSE);
						{
							sky.Draw();
							DrawBillboard3D(this->sun_pos.get(), 0.5f, 0.5f, 200.f, 0.f, this->sun_pic.get(), TRUE);
						}
						SetUseLighting(TRUE);
						SetFogEnable(fog_e);
					}
				}
			}
			void Shadow_Draw_Far(void) noexcept override {
				Map.obj.DrawModel();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				models.Get(SCHOOL, 0)->Draw();
				models.Get(GATE, 0)->Draw();
			}
			void Shadow_Draw(void) noexcept override {
				models.Draw();
			}
			void Main_Draw(void) noexcept override {
				if (Cut == 2 || Cut == 3 || Cut == 4 || Cut == 6 || Cut == 7 || Cut == 8 || Cut == 34 || Cut == 30 || (Cut >= 45 && Cut <= 49) || Cut == 52 || Cut == 53) {
					SetFogEnable(TRUE);
					SetFogDensity(0.01f);
					if (Cut == 34) {
						SetFogColor(128, 128, 128);
						SetFogStartEnd(10, 60);
					}
					if (Cut == 2 || Cut == 3 || Cut == 4 || Cut == 6 || Cut == 7 || Cut == 8) {
						SetFogColor(64, 64, 64);
						SetFogStartEnd(10, 500);
					}
					if (Cut == 30 || (Cut >= 45 && Cut <= 49) || Cut == 52 || Cut == 53) {
						SetFogColor(128, 128, 128);
						SetFogStartEnd(10, 300);
					}
				}
				Board.Draw();
				Map.Draw();
				models.Draw();
				Ship.Draw();

				SetFogEnable(FALSE);
				{
					if (Box_ALPHA != 0.f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*Box_ALPHA));
						DrawBox(0, 0, DrawPts->disp_x, DrawPts->disp_y, GetColor(0, 0, 0), TRUE);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					}
					for (auto& sl : anim) { sl.Draw((float)(DrawPts->disp_y) / sl.ysize); }
					for (auto& sl : sode) { sl.Draw((float)(DrawPts->disp_y) / sl.ysize); }
					for (auto& sl : First2) { sl.Draw((float)(DrawPts->disp_y) / sl.ysize); }
					for (auto& sl : First) { sl.Draw((float)(DrawPts->disp_y) / sl.ysize); }
					for (auto& sl : First3) { sl.Draw((float)(DrawPts->disp_y) / sl.ysize); }
					for (auto& sl : news_p) { sl.Draw((float)(DrawPts->disp_y) / sl.ysize); }
					for (auto& sl : sode_last) { sl.Draw((float)(DrawPts->disp_y) / sl.ysize); }
					Logo.Draw((float)(DrawPts->disp_y) / Logo.ysize);
					face.Draw((float)(DrawPts->disp_y) / face.ysize);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			void Item_Draw(void) noexcept override {
				TEMPSCENE::Item_Draw();
			}
			void LAST_Draw(void) noexcept override {
				/*
				int xxx_m = DrawPts->disp_x * 3 / 4 - y_r(200);
				int yyy_m = DrawPts->disp_y * 3 / 4;
				movie.DrawExtendGraph(xxx_m, yyy_m, DrawPts->disp_x / 4 + xxx_m, DrawPts->disp_y / 4 + yyy_m, FALSE);
				*/
			}
		};
	};
};