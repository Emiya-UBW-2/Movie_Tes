#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//íËêî
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };

	enum class ObjType : uint8_t {
		MovieObj,
	};
	enum class SoundEnum : uint8_t {
		RunFoot = (int)SoundEnumCommon::Num,
		KendoKun,
		Taiko1,
	};
	static const char* Model_Type[4] = { "SKY_TRUE","NEAR_FALSE","FAR_TRUE","SHADOW_DISACTIVE" };
};
