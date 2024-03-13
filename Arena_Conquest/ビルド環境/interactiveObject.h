//=============================================================================
//
// エネミー処理 [interactiveObject.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define WOODENSIGN_MAX				(5)		// PLATFORMSのMax数
#define INTERACTIVEOBJ_HIT_HOUSE_MAX	(1)		// PLATFORMSのMax数

enum
{
	TEX_SIGN,
	TEX_DOOR_ROCKS,
	TEX_EXIT_HOUSE,
	TEX_MESSAGE_EXAMINE,
	TEX_MESSAGE_EXIT,
	TEX_EXPLAIN_SIGN,
	TEX_EXPLAIN_ATTACKROCK,
	TEX_EXPLAIN_RUN,
	TEX_EXPLAIN_BULLET,
	TEX_EXPLAIN_DASH,
	TEX_EXPLAIN_FOCUS,
	TEX_EXPLAIN_JUMP,
	TEX_EXPLAIN_ENERGY,

	TEX_MAX,
	
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct INTERACTIVEOBJ
{
	BOOL		active;			// 有効
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号
	int			texAnimSpritesMax;	// 使用しているテクスチャ番号

	int			currentSprite;
	int			countAnim;
	int			animWait;

	int			messageTexNo;
};


struct BREAKABLE_WALL
{
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号
	int			messageTexNo;

	int			HP;
	
	BOOL		invincible;			//無敵状態
	int			invincibleTime;		//無敵のフレーム数
	int			invincibleTimeCnt;	//今の無敵のフレーム数
};


struct MESSAGE
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号

	BOOL		isActive;
};




//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitInteractiveObj(void);
void UninitInteractiveObj(void);
void UpdateInteractiveObj(void);
void DrawInteractiveObj(void);

BREAKABLE_WALL GetBreakableWall(void);

void BreakableWallHit(void);




