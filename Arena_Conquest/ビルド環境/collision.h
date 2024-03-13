//=============================================================================
//
// 当たり判定処理 [collision.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
 enum CollisionDirection {
	None,		//当たっていない
	FromTop,	//上から
	FromBottom,	//下から
	FromLeft,	//左から
	FromRight	//右から
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

 int CheckCollisionAndGetDirection(float boxAX, float boxAY, float boxAWidth, float boxAHeight,
								   float boxBX, float boxBY, float boxBWidth, float boxBHeight);

BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh, XMFLOAT3 ypos, float yw, float yh);
BOOL CollisionBBCenterAndLT(XMFLOAT3 mpos, float mw, float mh, XMFLOAT3 ypos, float yw, float yh);

BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2);


