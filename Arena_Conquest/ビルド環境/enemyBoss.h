//=============================================================================
//
// エネミー処理 [enemyBoss.h]
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

//ボスのテクスチャ
enum
{
	BOSS_TEX_BOSS_SPRITE_SHEET,
	BOSS_TEX_PARTIBLE,
	BOSS_TEX_LASER_SIGN,
	BOSS_TEX_LASER,
	BOSS_TEX_LIGHTNING_BALL,
	BOSS_TEX_BOSS_UI,
	BOSS_TEX_BOSS_UI_HP,

	BOSS_TEX_MAX,
};

//ボスのアニメーション
enum 
{
	BOSS_ANIM_CHASE,
	BOSS_ANIM_SHOOTING,
	BOSS_ANIM_LASER,
	BOSS_ANIM_DEATH1,
	BOSS_ANIM_DEATH2,
	
	BOSS_ANIM_MAX,
};

//ボスの状態
enum 
{
	BOSS_STATE_CHASE,
	BOSS_STATE_SHOOTING,
	BOSS_STATE_LASER,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
//ボスの構造体
struct BOSS
{
	BOOL		spawned;		// true:使っている  false:未使用
	BOOL		alive;			// true生きている   false:死んでいる
	BOOL		shoot;			// 電気玉の発射フラグ
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	float		w, h;			// 幅と高さ

	int			currentSprite;	// 今のアニメーションスプライト
	int			currentAnim;	// アニメーションパターンナンバー
	int			countAnim;		// アニメーションカウント
	int			texNo;			// テクスチャ番号
	XMFLOAT4	enemyColor;		// 色の設定

	XMFLOAT3	moveStartPos;	// 移動開始点
	XMFLOAT3	moveEndPos;		// 移動終了点

	float		HP;				// 体力

	BOOL		damaged;		// エネミーが傷付いた
	int			dmgTimeCnt;		// ダメージカウント
								   
	int			phase;			// フェーズ
	int			attacks;		// 攻撃の数
	int			attackCnt;		// 攻撃のカウント
	float		attackTime;		// 攻撃時間
	float		attackTimeCnt;	// 攻撃時間カウント

	int			projectileNum;	// 電気玉の数

	int			state;			// アクション状態

	XMFLOAT3	attackPos;		// ボスの攻撃場所

	XMFLOAT4	hpUIColor;		// ボスの体力UI色
};

//ボスの電気玉の構造体
struct BOSS_PROJECTILE
{
	BOOL		use;			// true:使っている  false:未使用
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号
	int			currentSprite;	// アニメーションパターンナンバー
	int			countAnim;		// アニメーションカウント

	float		moveSpeed;		// 移動速度
	XMFLOAT3	moveDir;		// 移動方向

	float		angle;			// 敵に対する電気玉の回転角

	XMFLOAT4	color;			// 色の設定
};

//ボスのレーザーの構造体
struct BOSS_LASER
{
	BOOL		use;			// true:使っている  false:未使用
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号

	XMFLOAT4	color;			// 色の設定

};

//ボスのパーティクルの構造体
struct BOSS_PARTICLE
{
	BOOL		use;			// true:使っている  false:未使用
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号

	float		moveSpeed;		// 移動速度
	XMFLOAT3	moveDir;		// 移動速度

	XMFLOAT4	color;			// 色の設定

	int			lifeTime;		//使う時間
	int			lifeTimeCnt;	//使う時間カウント
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBoss(void);
void UninitBoss(void);
void UpdateBoss(void);
void DrawBoss(void);

BOSS GetBoss(void);


void BossDamaged(float damage);

void SpawnBoss(XMFLOAT3 spawnPos);
