//=============================================================================
//
// エネミー処理 [spawnController.h]
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
#define SPAWN_ENEMY_MAX		(45)
#define SPAWN_PLATFORM_MAX	(15)
#define SPAWNER_MAX			(8)	

// スポーンの種類
enum 
{
	SPAWN_PATROL,
	SPAWN_BALL,
	SPAWN_KNIGHT,
	SPAWN_FLY_BOUNCE,
	SPAWN_FLY_ONE,
	SPAWN_SLIME_KING,
	SPAWN_BOSS,
};

// スポーン画像
enum 
{
	SPAWN_TEX_ELEVATOR,
	SPAWN_TEX_ENEMY_BOX,
	SPAWN_TEX_ROUND_UI,

	SPAWN_TEX_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct SPAWN_ENEMY
{
	BOOL		spawned;
	int			round;
	int			enemy;
	int			killCondition;
	XMFLOAT3	spawnPos;
};


struct SPAWN_PLATFORM
{
	BOOL		spawned;
	int			round;
	int			killCondition;
	XMFLOAT3	spawnPos;
};


struct SPAWNER
{
	BOOL		use;			// true:使っている  false:未使用
	XMFLOAT3	pos;			// ポリゴンの座標
	float		movePosHeight;	// ポリゴンのさいごのY座標
	float		w, h;			// 幅と高さ
	int			currentSprite;	// 今のアニメーションスプライト
	int			countAnim;		// アニメーションカウント
	int			texNo;			// テクスチャ番号

	int			spawnEnemyIndex;
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSpawnController(void);
void UninitSpawnController(void);
void UpdateSpawnController(void);
void DrawSpawnController(void);


void RoundKill(void);
int GetRound(void);