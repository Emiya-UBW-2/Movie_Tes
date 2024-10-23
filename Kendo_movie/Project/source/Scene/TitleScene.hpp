#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TitleScene : public TEMPSCENE {
		public:
			TitleScene(void) noexcept { }
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override{}
			//
			void			BG_Draw_Sub(void) const noexcept override {}
			void			ShadowDraw_Far_Sub(void) const noexcept override {}
			void			ShadowDraw_Sub(void) const noexcept override {}
			void			MainDraw_Sub(void) const noexcept override {}
			//UI•\Ž¦
			void			DrawUI_Base_Sub(void) const noexcept  override;
			void			DrawUI_In_Sub(void) const noexcept override {}
		};
	};
};
