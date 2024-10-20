#include	"TitleScene.hpp"
#include "../sub.hpp"
#include "../Script.hpp"
#include "../CutIn.hpp"
#include "../Editer.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Set_Sub(void) noexcept {
			Get_Next()->Load();
			
			select = 0;
			GameStart = 0.f;
			for (auto& y : SelYadd) {
				y = 0.f;
			}
		}
		//
		bool			TitleScene::Update_Sub(void) noexcept {
			if (DXDraw::Instance()->IsPause()) {
				return true;
			}
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			auto* DrawParts = DXDraw::Instance();

			Pad->ChangeGuide(
				[&]() {
					//auto* KeyGuide = PadControl::Instance();
				}
			);
			if (!OptionWindowClass::Instance()->IsActive()) {
				if (GameStart == 0.f) {
					if (Pad->GetKey(PADS::DIR_DOWN).trigger()) {
						--select;
						if (select < 0) { select = 2 - 1; }
						SelYadd[select] = 10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (Pad->GetKey(PADS::DIR_DOWN).trigger()) {
						++select;
						if (select > 2 - 1) { select = 0; }
						SelYadd[select] = -10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
				}
				for (int i = 0; i < 2; i++) {
					Easing(&SelYadd[i], 0.f, 0.95f, EasingType::OutExpo);
				}

				switch (select) {
				case 0:
					if (Pad->GetKey(PADS::INTERACT).trigger()) {
						SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (!(GameStart == 0.f && !Pad->GetKey(PADS::INTERACT).trigger())) {
						GameStart += 1.f / DrawParts->GetFps() / 0.5f;
					}
					break;
				case 1:
					if (Pad->GetKey(PADS::INTERACT).trigger()) {
						SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (Pad->GetKey(PADS::INTERACT).trigger()) {
						OptionWindowClass::Instance()->SetActive();
					}
					break;
				default:
					break;
				}
			}

			if (GameStart >= 1.f) {
				return false;
			}
			return true;
		}

		void			TitleScene::DrawUI_Base_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			int xp1, yp1;
			//
			xp1 = DrawParts->GetScreenY(1920 - 256 - 54 * 1 - (int)(GameStart*100.f));
			yp1 = DrawParts->GetScreenY(1080 - 108 - 108 * 1 + (int)SelYadd[0]);
			//DrawFetteString(xp1, yp1, 1.f, (select == 0), "Go");
			//
			xp1 = DrawParts->GetScreenY(1920 - 256);
			yp1 = DrawParts->GetScreenY(1080 - 108 + (int)SelYadd[1]);
			//DrawFetteString(xp1, yp1, 1.f, (select == 1), "Option");
			//
			{
				auto per = std::clamp((GameStart), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				DrawBox(0, 0, DrawParts->GetScreenX(1920), DrawParts->GetScreenY(1920), GetColor(0, 0, 0), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	};
};
