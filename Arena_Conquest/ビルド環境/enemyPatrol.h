//=============================================================================
//
// エネミー処理 [enemyPatrol.h]
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
#define ENEMY_PATROL_MAX		(10)		// エネミーのMax人数

enum 
{
	SLIME_ANIM_MOVE,
	SLIME_ANIM_HIT,
	SLIME_ANIM_DIE,

	SLIME_ANIM_MAX,
};


enum {
	SLIME_DIR_RIGHT,
	SLIME_DIR_LEFT,
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct ENEMY_PATROL
{
	BOOL		spawned;		// true:使っている  false:未使用
	BOOL		alive;			// true:生きている  false:死んでいる
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	float		w, h;			// 幅と高さ
	int			currentSprite;	// 今のアニメーションスプライト
	int			currentAnim;	// アニメーションパターンナンバー
	int			countAnim;		// アニメーションカウント
	int			texNo;			// テクスチャ番号
	float		moveSpeed;		// 移動速度
	float		moveDir;		// 移動方向
	int			moveTimeCnt;	// 移動時間

	float		HP;				// 体力

	BOOL		damaged;		//エネミーが傷付いた
	XMFLOAT3	damageOriginPos;//どこからダメージを受けた
	int			dmgTimeCnt;		

	XMFLOAT4	enemyColor;

	int			platformIndex;	//今立っている床
	BOOL		InGround;		//今立っている床
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyPatrol(void);
void UninitEnemyPatrol(void);
void UpdateEnemyPatrol(void);
void DrawEnemyPatrol(void);

ENEMY_PATROL* GetEnemyPatrol(void);


void EnemyDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage);

void SpawnEnemyPatrol(XMFLOAT3 spawnPos);
