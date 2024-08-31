#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TitleScene : public TEMPSCENE {
		private:
			int select{ 0 };
			std::array<float, 2> SelYadd{};

			float GameStart{ 0.f };
		public:
			const auto SelMode() const noexcept { return select; }
		public:
			TitleScene(void) noexcept { }
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override{}
			//
			void			BG_Draw_Sub(void) noexcept override {}
			void			ShadowDraw_Far_Sub(void) noexcept override {}
			void			ShadowDraw_Sub(void) noexcept override {}
			void			MainDraw_Sub(void) noexcept override {}
			//UI•\Ž¦
			void			DrawUI_Base_Sub(void) noexcept  override;
			void			DrawUI_In_Sub(void) noexcept override {}
		};
	};
};
