//=============================================================================
//
// エネミー処理 [enemyFlyBounce.h]
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
#define ENEMY_FLY_BOUNCE_MAX		(10)		// エネミーのMax人数

enum 
{
	FLY_BOUNCE_ANIM_MOVE,
	FLY_BOUNCE_ANIM_HIT,
	FLY_BOUNCE_ANIM_DIE,

	FLY_BOUNCE_ANIM_MAX,
};


enum {
	FLY_BOUNCE_DIR_RIGHT,
	FLY_BOUNCE_DIR_LEFT,
};


//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct ENEMY_FLY_BOUNCE
{
	BOOL		spawned;		// true:使っている  false:未使用
	BOOL		alive;			// true生きている   false:死んでいる

	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	float		w, h;			// 幅と高さ
	int			currentSprite;	// 今のアニメーションスプライト
	int			currentAnim;	// アニメーションパターンナンバー
	int			countAnim;		// アニメーションカウント
	int			texNo;			// テクスチャ番号
	XMFLOAT3	moveSpeed;		// 移動速度
	float		moveDir;		// 移動速度

	float		HP;				// 体力

	BOOL		damaged;		//エネミーBounceが傷付いた
	XMFLOAT3	damageOriginPos;//どこからダメージを受けた
	int			dmgTimeCnt;		//ダメージ時間カウント

	XMFLOAT4	enemyColor;		//色の設定

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyFlyBounce(void);
void UninitEnemyFlyBounce(void);
void UpdateEnemyFlyBounce(void);
void DrawEnemyFlyBounce(void);

ENEMY_FLY_BOUNCE* GetEnemyFlyBounce(void);


void EnemyFlyBounceDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage);


void SpawnEnemyFlyBounce(XMFLOAT3 spawnPos);