//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"
#include "playerUI.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_MAX			(1)		// プレイヤーのMax人数

#define	PLAYER_OFFSET_CNT	(10)	// ダッシュの影数

//プレイヤーの向き
enum
{
	CHAR_DIR_UP,
	CHAR_DIR_RIGHT,
	CHAR_DIR_DOWN,
	CHAR_DIR_LEFT,

	CHAR_DIR_MAX
};

//スラッシュの向き
enum
{
	SLASH_DIR_UP,
	SLASH_DIR_RIGHT,
	SLASH_DIR_DOWN,
	SLASH_DIR_LEFT,

	SLASH_DIR_MAX
};

//プレイヤーののアニメーション
enum
{
	ANIM_IDLE,
	ANIM_RUN,
	ANIM_JUMP,
	ANIM_FALL,
	ANIM_ATTACK,
	ANIM_DASH,
	ANIM_DASH_OFFSET,
	ANIM_HIT,
	ANIM_FOCUS,
	ANIM_DEATH,
	ANIM_SLASH,
	ANIM_SLASHHIT,
	ANIM_FOCUS_EFFECT,
	ANIM_EXCLAMATION,

	ANIM_TEX_MAX
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct PLAYER
{
	XMFLOAT3	pos;				// ポリゴンの座標
	XMFLOAT3	rot;				// ポリゴンの回転量
	BOOL		alive;				// true:使っている  false:未使用
	float		w, h;				// 幅と高さ
	float		countAnim;			// アニメーションカウント
	int			patternAnim;		// アニメーションパターンナンバー
	int			texNo;				// テクスチャ番号

	int			lifes;				// 今の体力
	int			lifesMax;			// 体力Max
	int			energy;				// 今のエネルギー
	int			energyMax;			// エネルギーMax
	
	int			dir;				// 向き（0:上 1:右 2:下 3:左）
	int			Lastdir;			// 向き（0:上 1:右 2:下 3:左）
	BOOL		moving;				// 移動中フラグ

	BOOL		dash;				// ダッシュ中フラグ
	BOOL		dashReload;			// 
	int			dashFrames;			// ダッシュのフレーム数
	int			dashCnt;			// 今のダッシュのフレーム数
	float		dashDistance;		// ダッシュの距離

	float		moveSpeed;			// 移動速度
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// 残像ポリゴンの座標

	BOOL		jump;				// ジャンプフラグ
	int			jumpCnt;			// ジャンプ中のカウント
	float		jumpYMax;			// ジャンプのフレーム数
	BOOL		extendJump;			// ジャンプの延長
	int			extendJumpCnt;		// 今のジャンプの延長のフレーム数
	int			extendJumpMax;		// ジャンプの延長のフレーム数
	

	
	BOOL		slash;				// Slash中フラグ
	int			slashFrames;		// Slashのフレーム数
	BOOL		slashCollider;		// Slash当たり判定有効
	int			slashColliderFrames;// Slashのフレーム数
	int			slashCnt;			// 今のSlashのフレーム数
	float		slashDistance;		// Slashの距離
	int			slashDir;			// Slashの距離
	float		slashDmg;			// Slashダメージ

	BOOL		onGround;			// 地面に触れている
	int			onAirCnt;			// 地面に触れずに一定フレーム数空中にあれば、飛んでいる
	float		gravityVelocity;	

	BOOL		damaged;			//エネミーが傷付いた
	XMFLOAT3	damageOriginPos;	//どこからダメージを受けた
	int			dmgTime;
	int			dmgTimeCnt;

	BOOL		invincible;			//無敵状態
	int			invincibleTime;		//無敵のフレーム数
	int			invincibleTimeCnt;	//今の無敵のフレーム数

	BOOL		focus;				//集中状態
	int			focusTimeMax;		//集中のフレーム数
	int			focusTimeCnt;		//今の集中のフレーム数
	int			focusEffectSprite;	//今の集中エフェクトのスプライト数


	XMFLOAT4	playerColor;

};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);

void PlayerDamaged(XMFLOAT3 enemyPos);

void SetPlayerStats(int hp, int energy);