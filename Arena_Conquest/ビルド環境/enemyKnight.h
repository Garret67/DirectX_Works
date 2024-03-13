//=============================================================================
//
// エネミー処理 [enemyKnight.h]
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
#define KNIGHT_TEXTURE_WIDTH		(132.0f * 2.5f)				// キャラサイズ
#define KNIGHT_TEXTURE_HEIGHT		(60.0f  * 2.5f)				// キャラサイズ

#define KNIGHT_COLLIDER_WIDTH		(KNIGHT_TEXTURE_WIDTH / 3)	// コライダーの横幅

#define ENEMY_KNIGHT_MAX			(10)						// エネミーのMax人数

//KNIGHTアニメーション
enum 
{
	KNIGHT_ANIM_IDLE,
	KNIGHT_ANIM_MOVE,
	KNIGHT_ANIM_ATTACK,
	KNIGHT_ANIM_DEFENCE_FRONT,
	KNIGHT_ANIM_DEFENCE_UP,
	KNIGHT_ANIM_HIT,
	KNIGHT_ANIM_DEATH,

	KNIGHT_ANIM_MAX,
};

//KNIGHT方向
enum {
	KNIGHT_DIR_RIGHT,
	KNIGHT_DIR_LEFT,
};

//KNIGHT状態
enum {
	KNIGHT_STATE_CONTROLLER,
	KNIGHT_STATE_MOVE_STAY,
	KNIGHT_STATE_MOVE_FORWARD,
	KNIGHT_STATE_MOVE_BACKWARD,
	KNIGHT_STATE_ATTACK,
	KNIGHT_STATE_DEFENCE_FRONT,
	KNIGHT_STATE_DEFENCE_UP,
	KNIGHT_STATE_HIT,
};

//KNIGHTとプレイヤー間の距離
enum {
	KNIGHT_DISTANCE_FAR,
	KNIGHT_DISTANCE_MIDWAY,
	KNIGHT_DISTANCE_NEAR_FRONT,
	KNIGHT_DISTANCE_NEAR_UP,
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct ENEMY_KNIGHT
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
	float		moveDir;		// 移動速度

	float		HP;				// 体力

	XMFLOAT4	enemyColor;

	int			state;
	int			stateTime;
	int			stateTimeCnt;

	int			playerDistance;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyKnight(void);
void UninitEnemyKnight(void);
void UpdateEnemyKnight(void);
void DrawEnemyKnight(void);

ENEMY_KNIGHT* GetEnemyKnight(void);

void EnemyKnightDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage);

void SpawnEnemyKnight(XMFLOAT3 spawnPos);
