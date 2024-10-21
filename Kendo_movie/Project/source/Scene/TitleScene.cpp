#include	"TitleScene.hpp"
#include "../sub.hpp"
#include "../Script.hpp"
#include "../CutIn.hpp"
#include "../Editer.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Set_Sub(void) noexcept {
			Get_Next()->Load();
		}
		//
		bool			TitleScene::Update_Sub(void) noexcept {
			return false;
			if (DXDraw::Instance()->IsPause()) {
				return true;
			}
			auto* Pad = PadControl::Instance();
			if (Pad->GetKey(PADS::INTERACT).trigger()) {
				return false;
			}
			return true;
		}

		void			TitleScene::DrawUI_Base_Sub(void) noexcept {}
	};
};
