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
		ef_greexp2,	//�O������2
		effects,	//�ǂݍ���
	};
	enum class scenes
	{
		NONE_SCENE,
		MAIN_LOOP
	};

}
