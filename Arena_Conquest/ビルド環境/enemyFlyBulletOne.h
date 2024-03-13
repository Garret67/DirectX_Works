//=============================================================================
//
// エネミー処理 [enemyFlyBulletOne.h]
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

#define ENEMY_FLY_ONE_MAX		(10)		// エネミーのMax人数

enum 
{
	FLY_ONE_ANIM_MOVE,
	FLY_ONE_ANIM_ATTACK,
	FLY_ONE_ANIM_HIT,
	FLY_ONE_ANIM_DEATH,
	
	FLY_ONE_ANIM_MAX,
};


enum {
	FLY_ONE_DIR_RIGHT,
	FLY_ONE_DIR_LEFT,
};

enum {
	FLY_ONE_STATE_CONTROLLER,
	FLY_ONE_STATE_STAY,
	FLY_ONE_STATE_MOVE,
	FLY_ONE_STATE_ATTACK,
	FLY_ONE_STATE_HIT,
};


//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct ENEMY_FLY_ONE
{
	BOOL		spawned;		// true:使っている  false:未使用
	BOOL		alive;			// true:生きている  false:死んでいる
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	float		w, h;			// 幅と高さ
	int			currentSprite;	// 今のアニメーションスプライト
	int			currentSpriteMax;	// 今のアニメーションスプライト
	int			currentAnim;	// アニメーションパターンナンバー
	int			countAnim;		// アニメーションカウント
	int			texNo;			// テクスチャ番号
	float		moveSpeed;		// 移動速度
	float		moveDir;		// 移動向き
	XMFLOAT3	WayPointPos;	// 移動向き

	float		HP;				// 体力

	XMFLOAT4	enemyColor;

	int			state;
	int			stateTime;
	int			stateTimeCnt;

	int			attackTimeCnt;

	XMFLOAT3	damageOriginPos;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyFlyOne(void);
void UninitEnemyFlyOne(void);
void UpdateEnemyFlyOne(void);
void DrawEnemyFlyOne(void);

ENEMY_FLY_ONE* GetEnemyFlyOne(void);

void EnemyFlyOneDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage);

void SpawnEnemyFlyOne(XMFLOAT3 spawnPos);
