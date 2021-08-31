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
				DrawPts->Ready_Shadow(camera_main.campos, [&] { Shadow_Draw(); }, [&] { Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.5f, 2.5f, 2.5f), VECTOR_ref::vget(15.f, 2.5f, 15.f));//MAIN_LOOPのnearはこれ (Get_Mine()->Damage.Get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
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
#define USE_TPS
			//データ
			MV1 body_obj, body_obj_lag, body_col;								//身体モデル
			GraphHandle light;													//ライトテクスチャ
		public:
			using TEMPSCENE::TEMPSCENE;
			void Awake(void) noexcept override {
				TEMPSCENE::Awake();
				light = GraphHandle::Load("data/light.png");								//ライト
				MV1::Load("data/model/body/model.mv1", &this->body_obj, true);				//身体モデル
				MV1::Load("data/model/body/col.mv1", &this->body_col, true);				//身体colモデル
				MV1::Load("data/model/body/model_lag.mv1", &this->body_obj_lag, true, -1.f);//身体ラグドール
			}

			void Start(void) noexcept {
			}
		public:
			void Set(void) noexcept override {
				TEMPSCENE::Set_EnvLight(VECTOR_ref::vget(5.f, 5.f, 5.f), VECTOR_ref::vget(-5.f, -5.f, -5.f), VECTOR_ref::vget(0.5f, -0.5f, 0.5f), GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				TEMPSCENE::Set();
				SetMousePoint(DrawPts->disp_x / 2, DrawPts->disp_y / 2);											//
			}
			bool Update(void) noexcept override {
				TEMPSCENE::Update();
				{
				}
				return true;
			}
			void Dispose(void) noexcept override {
			}
			void UI_Draw(void) noexcept  override {
			}
			void BG_Draw(void) noexcept override {
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