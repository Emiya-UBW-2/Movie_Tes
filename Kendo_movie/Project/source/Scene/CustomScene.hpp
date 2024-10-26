#pragma once
#include "../Header.hpp"
#include "../Script.hpp"
#include "../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CustomScene : public TEMPSCENE, public EffectControl {
		private:
			LoadUtil		m_LoadUtil;
		public:
			CustomScene(void) noexcept {}
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			BG_Draw_Sub(void) const noexcept override;
			void			ShadowDraw_Far_Sub(void) const noexcept override;
			void			ShadowDraw_Sub(void) const noexcept override;
			void			SetShadowDraw_Sub(void) const noexcept override;
			void			MainDraw_Sub(void) const noexcept override;
			//UI•\Ž¦
			void			DrawUI_Base_Sub(void) const noexcept  override;
			void			DrawUI_In_Sub(void) const noexcept override {}
		};
	};
};
