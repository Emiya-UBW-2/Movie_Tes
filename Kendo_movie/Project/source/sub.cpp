#include	"sub.hpp"

const FPS_n2::Sceneclass::CommonBattleResource* SingletonBase<FPS_n2::Sceneclass::CommonBattleResource>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::Taiko1, 1, "data/Sound/SE/taiko.wav", false);
			SE->Add((int)SoundEnum::KendoKun, 1, "data/Sound/Voice/kendokun.wav", false);
		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Get((int)SoundEnum::Taiko1).SetVol_Local(128);
		}
		void			CommonBattleResource::Dispose(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Delete((int)SoundEnum::Taiko1);
			SE->Delete((int)SoundEnum::KendoKun);
		}
	};
};
