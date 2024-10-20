#pragma once
#include	"../Header.hpp"
#include	"../sub.hpp"
#include "../Script.hpp"
#include "../CutIn.hpp"
#include "../Editer.hpp"

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
			size_t			m_Counter{ 0 };//カット

			SoundHandle		BGM;//データ
			int				BGM_Frequency{ 0 };

			int				spd_x{ 10 };
			LONGLONG		m_BaseTime{ 0 }, m_NowTimeWait{ 0 };
			bool			isFirstLoop{ true };//カット最初のループか
			bool			ResetPhysics{ true };

			switchs			Start, SpeedUp, SpeedDown;

			int				m_count{ 0 };
			float			Black_Buf{ 1.f };
			float			White_Buf{ 0.f };
		public:
			CustomScene(void) noexcept { }
		public:
			void			Load_Sub(void) noexcept override;
			void			Set_Sub(void) noexcept override;
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			BG_Draw_Sub(void) noexcept override;
			void			ShadowDraw_Far_Sub(void) noexcept override;
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept  override;
			void			DrawUI_In_Sub(void) noexcept override {}
		};
	};
};
