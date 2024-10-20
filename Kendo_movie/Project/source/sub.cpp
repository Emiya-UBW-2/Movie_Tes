#include	"sub.hpp"

const FPS_n2::Sceneclass::CommonBattleResource* SingletonBase<FPS_n2::Sceneclass::CommonBattleResource>::m_Singleton = nullptr;

const FPS_n2::TelopClass* SingletonBase<FPS_n2::TelopClass>::m_Singleton = nullptr;

namespace FPS_n2 {
	namespace Sceneclass {
		void			CommonBattleResource::Load(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::StandUp, 3, "data/Sound/SE/move/sliding.wav");
			SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
			SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
			SE->Add((int)SoundEnum::Env, 1, "data/Sound/SE/envi.wav", false);
		}
		void			CommonBattleResource::Set(void) noexcept {
			auto* SE = SoundPool::Instance();
			SE->Get((int)SoundEnum::StandUp).SetVol_Local(128);
			SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
			SE->Get((int)SoundEnum::StandupFoot).SetVol_Local(128);

			SE->Get((int)SoundEnum::Env).SetVol_Local(128);
		}
		void			CommonBattleResource::Dispose(void) noexcept {
		}
	};
};
