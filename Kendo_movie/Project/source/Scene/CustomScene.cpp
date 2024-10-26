#include	"CustomScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			CustomScene::Load_Sub(void) noexcept {
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->Load();
			BattleResourceMngr->Set();
			m_EventScene.Load();
		}
		void			CustomScene::Set_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			Vector3DX LightVec = Vector3DX::vget(1.f, -0.5f, 0.05f); LightVec = LightVec.normalized();
			DrawParts->SetAmbientLight(LightVec, GetColorF(1.0f / 3.f, 0.96f / 3.f, 0.94f / 3.f, 1.0f));
			SetLightDifColor(GetColorF(1.0f, 0.96f, 0.94f, 1.0f));																// デフォルトライトのディフューズカラーを設定する
			DrawParts->SetMainCamera().SetCamPos(Vector3DX::vget(0, 20, -20), Vector3DX::vget(0, 20, 0), Vector3DX::up());
			DrawParts->SetMainCamera().SetCamInfo(deg2rad(15), 1.f, 200.f);
#ifdef _USE_EFFEKSEER_
			EffectControl::Init();				//
#endif
			m_EventScene.Start(0);
			DrawParts->SetDistortionPer(120.f*4);
		}
		bool			CustomScene::Update_Sub(void) noexcept {
			auto* Pad = PadControl::Instance();
			Pad->ChangeGuide([&]() {});

			m_EventScene.GetDeltaTime();
			if (DXDraw::Instance()->IsPause()) { return true; }
			if (m_EventScene.IsEnd()) { return false; }
			m_EventScene.Update();
			//
#ifdef _USE_EFFEKSEER_
			EffectControl::Execute();
#endif
			return true;
		}
		void			CustomScene::Dispose_Sub(void) noexcept {
#ifdef _USE_EFFEKSEER_
			EffectControl::Dispose();
#endif
		}
		void CustomScene::Dispose_Load_Sub(void) noexcept {
			auto* BattleResourceMngr = CommonBattleResource::Instance();
			BattleResourceMngr->Dispose();
			m_EventScene.Dispose_Load();
		}
		//
		void			CustomScene::BG_Draw_Sub(void) const noexcept { m_EventScene.BGDraw(); }
		void			CustomScene::ShadowDraw_Far_Sub(void) const noexcept { }
		void			CustomScene::ShadowDraw_Sub(void) const noexcept { m_EventScene.ShadowDraw(); }
		void			CustomScene::SetShadowDraw_Sub(void) const noexcept { m_EventScene.SetShadowDraw(); }
		void			CustomScene::MainDraw_Sub(void) const noexcept { m_EventScene.MainDraw(); }
		//
		void			CustomScene::DrawUI_Base_Sub(void) const noexcept { m_EventScene.UIDraw(); }
	};
};
