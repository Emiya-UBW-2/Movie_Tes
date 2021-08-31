#pragma once

namespace FPS_n2 {
	//�萔
	static const float Frame_Rate{ 90.f };
	static const size_t max_bullet{ 64 };

	enum class Effect {
		ef_fire,	//���C��
		ef_reco,	//�����a���e
		ef_smoke,	//�e�̋O��
		ef_gndsmoke,//�n�ʂ̋O��
		ef_fire2,	//���C��
		ef_hitblood,//�����Ԃ�
		ef_greexp,	//�O������
		effects,	//�ǂݍ���
	};
	enum class EnumGunParts {
		NONE,
		BASE,
		MAGAZINE,
		MAZZULE,
		GRIP,
		UPER_HANDGUARD,
		UNDER_HANDGUARD,
		DUSTCOVER,
		STOCK,
		LAM,
		MOUNT_BASE,
		MOUNT,
		SIGHT,
		FOREGRIP,
		GUN,
	};
	enum class EnumAttachPoint {
		NONE,
		UPER_RAIL,
		UNDER_RAIL,
		LEFTSIDE_RAIL,
		RIGHTSIDE_RAIL,
		SIDEMOUNT,
		SIDEMOUNT_BASE,
		STOCK_BASE,
		DUSTCOVER_BASE,
		UPER_HANDGUARD,
		UNDER_HANDGUARD,
		MAZZULE_BASE,
		MAGAZINE_BASE,
		GRIP_BASE,
		NUM,
	};
	enum class EnumSELECTER {
		SEMI,
		FULL,
		_3B,
		_2B,
		NUM,
	};
	enum class EnumSELECT_LAM {
		LASER,
		LIGHT,
		NUM,
	};
	enum class scenes
	{
		NONE_SCENE,
		ITEM_LOAD,
		MAP_LOAD,
		LOAD,
		SELECT,
		MAIN_LOOP
	};

	enum class EnumKeyBind {
		FRONT,
		BACK,
		RIGHT,
		LEFT,
		Q,
		E,
		RELOAD,
		GET_ITEM,
		THROW_GRE,
		SIT,
		ESCAPE,
		SORT_MAG,
		RUN,
		JUMP,
		CHANGE_VIEW,
		INFO,
		LOOKGUN,
		PAUSE,
		MED,
		RIDE_ON,
	};

	enum class EnumMouseBind {
		SHOOT,
		CHANGE_SEL,
		AIM,
	};

	enum class EnumSound {
		//UI

		CURSOR,
		CANCEL,
		TIMER,

		//human

		Sort_MAG,
		Cate_Load,
		Foot_Sound,
		Explosion,

		Voice_Damage,
		Voice_Death,
		Voice_Breath,
		Voice_Breath_Run,

		//this->Gun

		Assemble,
		Shot,
		Slide,
		Trigger,
		MAG_Down,
		MAG_Set,
		Cate_Down,

		//Tank

		Tank_Damage,
		Tank_Shot,
		Tank_Reload,
		Tank_Ricochet,
		Tank_engine,

		//Item

		//MAP
		MAP0_ENVI,
	};
}
