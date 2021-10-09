#pragma once

namespace FPS_n2 {
	//’è”
	static const float Frame_Rate{ 90.f };
	static const size_t max_bullet{ 64 };

	enum class Effect {
		ef_fire,	//”­–C‰Š
		ef_reco,	//¬ŒûŒa’µ’e
		ef_smoke,	//e‚Ì‹OÕ
		ef_gndsmoke,//’n–Ê‚Ì‹OÕ
		ef_fire2,	//”­–C‰Š
		ef_hitblood,//ŒŒ‚µ‚Ô‚«
		ef_greexp,	//ƒOƒŒ”š”­
		ef_greexp2,	//ƒOƒŒ”š”­2
		effects,	//“Ç‚İ‚Ş
	};
	enum class scenes
	{
		NONE_SCENE,
		MAIN_LOOP
	};

}
