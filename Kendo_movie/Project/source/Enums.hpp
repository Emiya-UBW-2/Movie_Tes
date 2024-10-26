#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//’è”
	enum class SoundEnum : uint8_t {
		RunFoot = (int)SoundEnumCommon::Num,
		KendoKun,
		Taiko1,
		Max,
	};

	static const char* SoundID[(int)SoundEnum::Max] = {
		"",
		"",
		"",
		"",
		"RunFoot",
		"KendoKun",
		"Taiko1",
	};
};
