#pragma once
#include"Header.hpp"

// プレイヤー関係の定義
#define PLAYER_ENUM_DEFAULT_SIZE	1.8f	// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			0.4f	// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			1.8f	// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			16		// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH		0.015f	// 一度の壁押し出し処理でスライドさせる距離

namespace FPS_n2 {
};