#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//íËêî
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };

	namespace Sceneclass {
		enum class ObjType {
			Human,
			Ammo,
			FallObj,
			Gun,
			Magazine,
			Lower,
			Upper,
			Barrel,
			UnderRail,
			Sight,
			MuzzleAdapter,
			MovieObj,
			Max,
		};
		enum class SoundEnum {
			RunFoot = (int)SoundEnumCommon::Num,
			StandUp,
			SlideFoot,
			StandupFoot,
			Heart,
			//
			Env,
			Env2,
			//
			Movievoice1,
			Movievoice2,
			Movievoice3,
			Movievoice4,
			Movievoice5,
			Movievoice6,
			Movievoice7,
			Movievoice8,
			Movievoice9,
			Movievoice10,
		};
		enum class CharaTypeID {
			Mine,
			Team,
			Enemy,
		};
	};

	typedef char PlayerID;
	typedef short HitPoint;

	static const char* Model_Type[4] = { "SKY_TRUE","NEAR_FALSE","FAR_TRUE","SHADOW_DISACTIVE" };
};
