//=============================================================================
//
// モデル処理 [player.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once
//#include "modelBinaryFBX.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER		(3)					// プレイヤーの数

#define	PLAYER_SIZE		(8.0f)				// 当たり判定の大きさ

enum PLAYER_ANIM 
{
	P_ANIM_IDLE,
	P_ANIM_WALK,
	P_ANIM_HIT,
	P_ANIM_DEATH,
	P_ANIM_BLOCK,
	P_ANIM_ATTACK_PUNCH,
	P_ANIM_ATTACK_CRIT,
	P_ANIM_ATTACK_AXE,

	P_ANIM_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PLAYER
{
	XMFLOAT4X4		mtxWorld;	// ワールドマトリックス
	XMFLOAT3		pos;		// モデルの位置
	XMFLOAT3		rot;		// モデルの向き(回転)
	XMFLOAT3		scl;		// モデルの大きさ(スケール)
	
	BOOL			load;		// モデルロードフラグ
	DX11_MODEL		model;		// モデルバッファー情報
	MODEL_DATA		modelData;	// モデルのメッシュ・トランスフォーム情報
	FBX_ANIMATOR	animator;	// アニメーター

	BOOL			use;		// 使用フラグ
	float			size;		// 当たり判定の大きさ
	
	int				HPmax;			// 最大HP
	int				HPcur;			// 現在のHP
	int				agility;		// 速さ
	int				attack;			// 攻撃
	int				deffense;		// 防御
	BOOL			myTurn;			// このプレイヤーのターン
	BOOL			turnFinished;	// このプレイヤーのターンが終わった
	BOOL			dead;			// 倒れている状態
									   
	XMFLOAT3		patrolDest;		// 目的地
	float			patrolFrames;	// 移動開始からのフレーム数	

};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

void SavePlayerGameInfo(void);
void SavePlayerFightInfo(void);

void SetSelectionEnemy(void);
void SetSelectionPlayer(void);


void PlayerDamaged(int damage, int playerIdx);