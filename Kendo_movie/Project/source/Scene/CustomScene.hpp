#pragma once
#include "../CutIn.hpp"
#include "../Editer.hpp"
#include "../Header.hpp"
#include "../Script.hpp"
#include "../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CustomScene : public TEMPSCENE, public EffectControl {
		private:
			//std::string LOGO1 = "data/picture/logo.png";
			std::string Suit = "data/model/Suit/model.mv1";
			std::string Soldier = "data/model/Soldier/model.mv1";
		private:
			LoadUtil		m_LoadUtil;
#ifdef EditMode
			LoadEditUtil	m_LoadEditUtil;
#endif
			LONGLONG		m_BaseTime{ 0 }, m_NowTime{ 0 };
			size_t			m_Counter{ 0 };//カット
			int				m_count{ 0 };

			SoundHandle		BGM;//データ
			int				BGM_Frequency{ 0 };

			switchs			Start, SpeedUp, SpeedDown;
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
			//UI表示
			void			DrawUI_Base_Sub(void) const noexcept  override;
			void			DrawUI_In_Sub(void) const noexcept override {}
		};
	};
};
