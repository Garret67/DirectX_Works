//=============================================================================
//
// エネミーモデル処理 [enemy.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ENEMY		(20)					// エネミーの数

#define	ENEMY_SIZE		(8.0f)				// 当たり判定の大きさ

enum SQUELETON_ANIM
{
	SQUELETON_IDLE,
	SQUELETON_WALK,
	SQUELETON_JUMP,
	SQUELETON_HIT,
	SQUELETON_DEATH,
	SQUELETON_ATTACK_PUNCH,

	SQUELETON_ANIM_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY
{
	XMFLOAT4X4		mtxWorld;			// ワールドマトリックス
	XMFLOAT3		pos;				// モデルの位置
	XMFLOAT3		rot;				// モデルの向き(回転)
	XMFLOAT3		scl;				// モデルの大きさ(スケール)

	BOOL			use;
	BOOL			load;
	DX11_MODEL		model;		// モデル情報
	MODEL_DATA		modelData;
	FBX_ANIMATOR	animator;

	float			spd;				// 移動スピード
	float			size;				// 当たり判定の大きさ

	int				HP;
	int				agility;
	int				agilityCnt;
	int				attack;
	int				deffense;
	BOOL			myTurn;			//このエネミーのターン
	BOOL			turnFinished;	//このエネミーのターンが終わった
	BOOL			dead;			

	//int				shadowIdx;			// 影のインデックス番号

	XMFLOAT3		patrolDest;			//目的地
	float			patrolFrames;		//移動開始からのフレーム数	
	float			patrolWaitFrames;		//移動開始からのフレーム数	

	DISSOLVE		dissolve;
	float			shadowAlpha;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
int GetSpawnedEnemies(void);

void SaveEnemyMapInfo(int enemyIdx);

void EnemyDamaged(int damage, int enemyIdx);

void EnemyRelocation(BOOL Xmov, BOOL positiveMov, float limitPos);