//=============================================================================
//
// バレット処理 [bullet.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************

enum
{
	BULLET_TEX_FIRE_BULLET,
	BULLET_TEX_COLLIDER,
	BULLET_TEX_BULLET_EXPLOSION,

	BULLET_TEX_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

// 弾の構造体
struct BULLET
{
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	XMFLOAT3	pos;			// バレットの座標
	XMFLOAT3	rot;			// バレットの回転量
	XMFLOAT3	move;			// バレットの移動量
	int			countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// 何番目のテクスチャーを使用するのか

	XMFLOAT4	color;			// 何番目のテクスチャーを使用するのか

};

// 弾の爆発構造体
struct EXPLOSION
{
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	XMFLOAT3	pos;			// バレットの座標
	int			countAnim;		// アニメーションカウント
	int			currentSprite;	// 今のスプライト
	int			texNo;			// 何番目のテクスチャーを使用するのか

	XMFLOAT4	color;			// 何番目のテクスチャーを使用するのか

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET *GetBullet(void);		//弾を取得する

void SetBulletPlayer(XMFLOAT3 pos, int direction);						//プレイヤーの弾設定
void SetBulletEnemy(XMFLOAT3 pos, float rotation, XMFLOAT3 MoveDir);	//エネミー　の弾設定

