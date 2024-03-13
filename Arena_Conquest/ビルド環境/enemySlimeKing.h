//=============================================================================
//
// エネミー処理 [enemySlimeKing.h]
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
#define SLIME_KING_PARTICLE_MAX	(40)


enum 
{
	SLIME_KING_ANIM_WAKE_UP,
	SLIME_KING_ANIM_PREPARATION,
	SLIME_KING_ANIM_TIRED,
	SLIME_KING_ANIM_DEATH,
	
	SLIME_KING_ANIM_MAX,
};


enum {
	SLIME_KING_DIR_RIGHT,
	SLIME_KING_DIR_LEFT,
};

enum {
	SLIME_KING_ATTACK_HORIZONTAL,
	SLIME_KING_ATTACK_VERTICAL,
};


enum {
	SLIME_KING_STATE_IDLE,
	SLIME_KING_STATE_PREPARATION,
	SLIME_KING_STATE_TIRED,
	SLIME_KING_STATE_RUSH,
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct SLIME_KING
{
	BOOL		spawned;			// true生きている   false:死んでいる
	BOOL		alive;			// true生きている   false:死んでいる
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

	BOOL		damaged;		//エネミーが傷付いた
	int			dmgTimeCnt;		

	XMFLOAT4	enemyColor;

	int			attackType;

	int			phase;
	int			portal;
	int			state;
	int			stateTime;
	int			stateTimeCnt;
};


struct SLIME_KING_PARTICLE
{
	BOOL		use;			// true生きている   false:死んでいる
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号

	float		moveSpeed;		// 移動速度
	XMFLOAT3	moveDir;		// 移動速度

	XMFLOAT4	color;

	int			lifeTime;
	int			lifeTimeCnt;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSlimeKing(void);
void UninitSlimeKing(void);
void UpdateSlimeKing(void);
void DrawSlimeKing(void);

SLIME_KING GetSlimeKing(void);


void SlimeKingDamaged(float damage);

void SpawnSlimeKing(XMFLOAT3 spawnPos);
