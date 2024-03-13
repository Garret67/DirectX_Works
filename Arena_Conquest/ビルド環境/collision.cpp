//=============================================================================
//
// 当たり判定処理 [collision.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "collision.h"



//*****************************************************************************
// BBによる当たり判定処理
// 戻り値：どの方向から当たっている。当たっていなかったらNoneを戻る
//*****************************************************************************
int CheckCollisionAndGetDirection(float boxAX, float boxAY, float boxAWidth, float boxAHeight,
								  float boxBX, float boxBY, float boxBWidth, float boxBHeight) 
{
	// 座標の差を計算する	
	float dx = boxAX - boxBX;
	float dy = boxAY - boxBY;

	float boxAHalfWidth  = boxAWidth * 0.5f;
	float boxBHalfWidth  = boxBWidth * 0.5f;
	float boxAHalfHeight = boxAHeight * 0.5f;
	float boxBHalfHeight = boxBHeight * 0.5f;

	// 幅と高さの半分を合計する
	float combinedHalfWidths  = boxAHalfWidth  + boxBHalfWidth;
	float combinedHalfHeights = boxAHalfHeight + boxBHalfHeight;

	// 横と縦方向のあたり判定を確認する
	if (fabsf(dx) < combinedHalfWidths && fabsf(dy) < combinedHalfHeights) 
	{
		float overlapX = combinedHalfWidths - fabsf(dx);
		float overlapY = combinedHalfHeights - fabsf(dy);

		if (overlapX >= overlapY) 
		{
			if (dy > 0)
				return FromTop;
			else
				return FromBottom;
		}
		else 
		{
			if (dx > 0)
				return FromLeft;
			else
				return FromRight;
		}
	}

	return None;
}


//=============================================================================
// BBによる当たり判定処理
// 回転は考慮しない
// 戻り値：当たってたらtrue
//=============================================================================
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh,
	XMFLOAT3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;	// 外れをセットしておく

	// 座標が中心点なので計算しやすく半分にしている
	mw /= 2;
	mh /= 2;
	yw /= 2;
	yh /= 2;

	// バウンディングボックス(BB)の処理
	if ((mpos.x + mw > ypos.x - yw) &&
		(mpos.x - mw < ypos.x + yw) &&
		(mpos.y + mh > ypos.y - yh) &&
		(mpos.y - mh < ypos.y + yh))
	{
		// 当たった時の処理
		ans = TRUE;
	}

	return ans;
}



//=============================================================================
// BBによる当たり判定処理
// 回転は考慮しない
// 戻り値：当たってたらtrue
//=============================================================================
BOOL CollisionBBCenterAndLT(XMFLOAT3 mpos, float mw, float mh,
	XMFLOAT3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;	// 外れをセットしておく

	// 座標が中心点なので計算しやすく半分にしている
	mw /= 2;
	mh /= 2;
	//yw /= 2;
	//yh /= 2;

	// バウンディングボックス(BB)の処理
	if ((mpos.x + mw > ypos.x/* - yw*/) &&
		(mpos.x - mw < ypos.x + yw) &&
		(mpos.y + mh > ypos.y/* - yh*/) &&
		(mpos.y - mh < ypos.y + yh))
	{
		// 当たった時の処理
		ans = TRUE;
	}

	return ans;
}



//=============================================================================
// BCによる当たり判定処理
// サイズは半径
// 戻り値：当たってたらTRUE
//=============================================================================
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2)
{
	BOOL ans = FALSE;						// 外れをセットしておく

	float len = (r1 + r2) * (r1 + r2);		// 半径を2乗した物
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2点間の距離（2乗した物）
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);

	// 半径を2乗した物より距離が短い？
	if (len > lenSq)
	{
		ans = TRUE;	// 当たっている
	}

	return ans;
}



