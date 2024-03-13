//=============================================================================
//
// エネミー処理 [enemyBall.h]
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
#define ENEMY_BALL_MAX			(10)		// エネミーのMax数

enum 
{
	BALL_ANIM_MOVE,
	BALL_ANIM_HIT,
	BALL_ANIM_DIE,
	BALL_ANIM_SEARCH,

	BALL_ANIM_MAX,
};


enum {
	BALL_DIR_RIGHT,
	BALL_DIR_LEFT,
};


enum {
	BALL_STATE_SEARCH,
	BALL_STATE_RUSH,
	BALL_STATE_JUMP,
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct ENEMY_BALL
{
	BOOL		spawned;		// true:使っている  false:未使用
	BOOL		alive;			// true生きている   false:死んでいる
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			currentSprite;	// 今のアニメーションスプライト
	int			currentAnim;	// 今のアニメーションパターン
	int			countAnim;		// アニメーションカウント
	int			texNo;			// テクスチャ番号
	float		moveSpeed;		// 移動速度
	int			moveDir;		// エネミーの向き

	float		HP;				// 体力

	BOOL		damaged;		//エネミーが傷付いたフラグ
	XMFLOAT3	damageOriginPos;//どこからダメージを受けた
	int			dmgTimeCnt;		//ダメージカウント

	XMFLOAT4	enemyColor;		//色の設定

	int			state;			//アクション状態
	int			stateTime;		//アクション状態時間
	int			stateTimeCnt;	//アクション状態時間カウント
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyBall(void);
void UninitEnemyBall(void);
void UpdateEnemyBall(void);
void DrawEnemyBall(void);


ENEMY_BALL* GetEnemyBall(void);


void EnemyBallDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage);

void SpawnEnemyBall(XMFLOAT3 spawnPos);
