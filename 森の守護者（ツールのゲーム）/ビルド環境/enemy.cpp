//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "modelBinaryFBX.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "fade.h"
#include "player.h"
#include "fightUI.h"
#include "gameEnvironment.h"
#include "collision.h"
#include "sound.h"
#include "particle.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define	MODEL_ENEMY			"data/MODEL/enemy.obj"						// 読み込むモデル名
#define	MODEL_SQUELETON		"data\\MODEL\\Squeleton_ModelData.bin"		// 読み込むモデル名

#define	VALUE_MOVE			(1.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ

#define SLOW_OUT_DISTANCE	(25.0f)						// SLOW OUTが始まる距離
#define SLOW_IN_FRAMES		(30.0f)						// SLOW INがかかるフレーム数

#define PATROL_WAIT_FRAMES	(120.0f)					// SLOW INがかかるフレーム数
#define LERP_ROT_FACTOR		(0.1f)						// 線形補間の速さ

#define FIGHT_POS_MAX		(5)							// エネミーの立ち場所

#define FIGHT_SPEED			(1.5f)						// エネミーのFIGHT移動速度
#define GAME_SPEED			(0.5f)						// エネミーのGAME 移動速度


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadEnemyMapInfo(void);
void UpdateEnemyGameMode(void);
void UpdateEnemyFightMode(void);

void UpdateMovement(float moveX, float moveZ, float moveVectorMagnitude, int Idx, float speedMultiplier, float slowInFrames, float slowOutDistance);
BOOL Jumping(int Idx, int attackedIdx, XMFLOAT3 initPos, XMFLOAT3 finalPos);
void EnemyAttack(int Idx);
void EnemyAttack2(int Idx);

void EnemyCollider(int idx);

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY_MAP_INFO
{
	XMFLOAT3		pos;		// モデルの位置
	XMFLOAT3		rot;		// モデルの向き(回転)
	BOOL			use;		// 使用フラグ
	int				spawnedEnemies;

	XMFLOAT3		patrolDest;	//目的地

	BOOL			saved;		// 使用フラグ
};


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー

static ENEMY_MAP_INFO	g_EnemyMapInfo[MAX_ENEMY];		// エネミーのセーブデータ

static XMFLOAT3			g_FightPos[FIGHT_POS_MAX] =		//バトルの座標
{
	XMFLOAT3( 70.0f, 0.0f,  60.0f),
	XMFLOAT3(  0.0f, 0.0f,  60.0f),
	XMFLOAT3(-70.0f, 0.0f,  60.0f),
	XMFLOAT3( 35.0f, 0.0f, 110.0f),
	XMFLOAT3(-35.0f, 0.0f, 110.0f),
};

static int g_SpawnedEnemies = 0;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		g_Enemy[i].HP			= 25;
		g_Enemy[i].agility		= 10;
		g_Enemy[i].agilityCnt	= g_Enemy[i].agility;
		g_Enemy[i].attack		= 8;
		g_Enemy[i].deffense		= 10;
		g_Enemy[i].myTurn		= FALSE;
		g_Enemy[i].turnFinished = FALSE;
		g_Enemy[i].dead			= FALSE;

		g_Enemy[i].use = FALSE;			// TRUE:生きてる

		g_Enemy[i].patrolDest = g_Enemy[i].pos;
		g_Enemy[i].patrolFrames = 0;
		g_Enemy[i].patrolWaitFrames = 0;

		g_Enemy[i].shadowAlpha = 0.2f;
	}

	//モードによって初期化する
	switch (GetMode())
	{
	case MODE_GAME:
		g_SpawnedEnemies = MAX_ENEMY;

		for (int i = 0; i < g_SpawnedEnemies; i++)
		{
			InitModelBinaryFBX(MODEL_SQUELETON, &g_Enemy[i].model, &g_Enemy[i].modelData, &g_Enemy[i].animator);
			g_Enemy[i].load = TRUE;

			if(i < 5)		g_Enemy[i].pos = XMFLOAT3( GROUND_WIDTH , 0.0f, -400 + (i * 200.0f));
			else if(i < 10)	g_Enemy[i].pos = XMFLOAT3(-GROUND_WIDTH , 0.0f, -400 + ((i-5) * 200.0f));
			else if(i < 15)	g_Enemy[i].pos = XMFLOAT3(-400.0f + ((i-10) * 200.0f), 0.0f,  GROUND_HEIGHT);
			else if(i < 20)	g_Enemy[i].pos = XMFLOAT3(-400.0f + ((i-15) * 200.0f), 0.0f, -GROUND_HEIGHT);

			g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Enemy[i].patrolDest = g_Enemy[i].pos;
			g_Enemy[i].use = TRUE;

			g_Enemy[i].dissolve.Enable = FALSE;
			g_Enemy[i].dissolve.threshold = 0.0f;
			g_Enemy[i].shadowAlpha = 0.2f;
		}
		LoadEnemyMapInfo();

		break;

	case MODE_FIGHT:
		g_SpawnedEnemies = (rand() % 5) + 1;
		//g_SpawnedEnemies = 3;

		for (int i = 0; i < g_SpawnedEnemies; i++)
		{
			InitModelBinaryFBX(MODEL_SQUELETON, &g_Enemy[i].model, &g_Enemy[i].modelData, &g_Enemy[i].animator);
			g_Enemy[i].load = TRUE;

			g_Enemy[i].pos = g_FightPos[i];
			g_Enemy[i].rot = XMFLOAT3(0.0f, XM_PI, 0.0f);
			g_Enemy[i].patrolDest = g_Enemy[i].pos;
			g_Enemy[i].animator.animation[SQUELETON_JUMP].speed = 1.5f;
			g_Enemy[i].use = TRUE;

			g_Enemy[i].dissolve.Enable = TRUE;
			g_Enemy[i].dissolve.threshold = 0.0f;
			g_Enemy[i].shadowAlpha = 0.2f;

		}

		break;

	default:
		g_SpawnedEnemies = MAX_ENEMY;

		for (int i = 0; i < g_SpawnedEnemies; i++)
		{
			InitModelBinaryFBX(MODEL_SQUELETON, &g_Enemy[i].model, &g_Enemy[i].modelData, &g_Enemy[i].animator);
			g_Enemy[i].load = TRUE;

			g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, 0.0f, 20.0f);
			g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Enemy[i].patrolDest = g_Enemy[i].pos;
			g_Enemy[i].use = TRUE;

			g_Enemy[i].dissolve.Enable = FALSE;
			g_Enemy[i].dissolve.threshold = 0.0f;
			g_Enemy[i].shadowAlpha = 0.2f;
			break;
		}
	}
	

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UninitModelBinaryFBX(&g_Enemy[i].model, &g_Enemy[i].modelData, &g_Enemy[i].animator);
			g_Enemy[i].load = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	switch (GetMode())
	{
	case MODE_GAME:
		UpdateEnemyGameMode();
		break;

	case MODE_FIGHT:
		UpdateEnemyFightMode();
		break;

	default:
		break;
	}


	//***********
	// アニメーション
	//***********
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use && g_Enemy[i].load)		// このエネミーが使われている？
		{
			//アニメーション処理
			if (g_Enemy[i].animator.curAnim != -1 && g_Enemy[i].animator.animationNum > 0)
			{
				UpdateModelBinaryFBX(&g_Enemy[i].modelData, g_Enemy[i].animator);
			}
		}
	}


#ifdef _DEBUG


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_BACK);


	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (!g_Enemy[i].use  || !g_Enemy[i].load) continue;

		SetDissolve(&g_Enemy[i].dissolve);
		SetAlphaTestEnable(g_Enemy[i].dissolve.Enable);

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y/* + XM_PI*/, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// モデル描画
		//DrawModel(&g_Enemy[i].model);
		DrawModelBinaryFBX(&g_Enemy[i].model, g_Enemy[i].modelData, mtxWorld);

		SetDissolveEnable(FALSE);
		SetAlphaTestEnable(FALSE);


		//*************
		// 影の描画
		//*************


		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, 0.01f, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y/* + XM_PI*/, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, 0.01f, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		XMFLOAT4* MaterialDiffuse = new XMFLOAT4[100];
		int matIdx = 0;

		for (int z = 0; z < g_Enemy[i].model.BuffersNum; z++)
		{
			for (int j = 0; j < g_Enemy[i].model.Buffers[z].SubsetNum; j++)
			{
				MaterialDiffuse[matIdx] = g_Enemy[i].model.Buffers[z].SubsetArray[j].Material.Material.Diffuse;
				g_Enemy[i].model.Buffers[z].SubsetArray[j].Material.Material.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, g_Enemy[i].shadowAlpha);
				matIdx++;
			}
		}

		// 影描画
		DrawModelBinaryFBX(&g_Enemy[i].model, g_Enemy[i].modelData, mtxWorld);


		matIdx = 0;
		for (int z = 0; z < g_Enemy[i].model.BuffersNum; z++)
		{
			for (int j = 0; j < g_Enemy[i].model.Buffers[z].SubsetNum; j++)
			{
				g_Enemy[i].model.Buffers[z].SubsetArray[j].Material.Material.Diffuse = MaterialDiffuse[matIdx];
				matIdx++;
			}
		}


		delete[] MaterialDiffuse;


	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

//=============================================================================
// セーブエネミーデータ
//=============================================================================
void SaveEnemyMapInfo(int enemyIdx)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		g_EnemyMapInfo[i].pos		 = g_Enemy[i].pos;
		g_EnemyMapInfo[i].rot		 = g_Enemy[i].rot;
		g_EnemyMapInfo[i].use		 = g_Enemy[i].use;
		g_EnemyMapInfo[i].patrolDest = g_Enemy[i].patrolDest;
		g_EnemyMapInfo[i].saved		 = TRUE;
	}

	g_EnemyMapInfo[enemyIdx].pos.z -= GROUND_HEIGHT;
}

//=============================================================================
// ロードエネミーデータ
//=============================================================================
void LoadEnemyMapInfo(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_EnemyMapInfo[i].saved)
		{
			g_Enemy[i].pos		  = g_EnemyMapInfo[i].pos;
			g_Enemy[i].rot		  = g_EnemyMapInfo[i].rot;
			g_Enemy[i].use		  = g_EnemyMapInfo[i].use;
			g_Enemy[i].patrolDest = g_EnemyMapInfo[i].patrolDest;

			g_EnemyMapInfo[i].saved = FALSE;
		}
	}
}

//=============================================================================
// 更新ゲームモードエネミー
//=============================================================================
void UpdateEnemyGameMode(void)
{
	for (int i = 0; i < g_SpawnedEnemies; i++)
	{
		if (g_Enemy[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes
				// 移動処理			
			float moveX = g_Enemy[i].patrolDest.x - g_Enemy[i].pos.x;
			float moveZ = g_Enemy[i].patrolDest.z - g_Enemy[i].pos.z;


			// Calcular la magnitud del movimiento total
			float moveVectorMagnitude = sqrtf(moveX * moveX + moveZ * moveZ);

			if (moveVectorMagnitude < 2)
			{
				//パトロールするまでの時間
				if (g_Enemy[i].patrolFrames < SLOW_IN_FRAMES + g_Enemy[i].patrolWaitFrames)
				{
					g_Enemy[i].patrolFrames++;
				}
				else  //パトロール開始
				{
					PLAYER* player = GetPlayer();
					g_Enemy[i].patrolDest.x = (float)(rand() % 2000 - 1000) + player->pos.x;
					g_Enemy[i].patrolDest.z = (float)(rand() % 2000 - 1000) + player->pos.z;

					g_Enemy[i].patrolWaitFrames = (float)(rand() % 300 + 60);
					g_Enemy[i].patrolFrames = 0;
				}

				if (g_Enemy[i].animator.curAnim != SQUELETON_IDLE) g_Enemy[i].animator.nextAnim = SQUELETON_IDLE;

			}
			else
			{
				UpdateMovement(moveX, moveZ, moveVectorMagnitude, i, GAME_SPEED, SLOW_IN_FRAMES, 7.0f);
			}


			EnemyCollider(i);
		}
	}




}

//=============================================================================
// 更新バトルモードエネミー
//=============================================================================
void UpdateEnemyFightMode(void)
{
	for (int i = 0; i < g_SpawnedEnemies; i++)
	{
		if (g_Enemy[i].use && g_Enemy[i].load)		// このエネミーが使われている？
		{								// Yes
			//誰の番でもない（このエネミー以外）
			if (g_Enemy[i].myTurn)	//このエネミーのターンでもない時
			{
				EnemyAttack(i);		//歩き攻撃

				//EnemyAttack2(i);	//ジャンプ攻撃
			}

			//エネミーが倒れている（消える処理）
			if (g_Enemy[i].dead)
			{
				if (g_Enemy[i].animator.animation[SQUELETON_DEATH].AnimTimeCnt > g_Enemy[i].animator.animation[SQUELETON_DEATH].keyFrameNum - 1.5f)
				{
					g_Enemy[i].dissolve.threshold += 0.01f;

					//影を消す処理
					if (g_Enemy[i].dissolve.threshold <= 0.2f)	g_Enemy[i].shadowAlpha = 0.2f - g_Enemy[i].dissolve.threshold;

					//エネミーを消す
					if (g_Enemy[i].dissolve.threshold >= 1.0f) g_Enemy[i].use = FALSE;

				}
			}

		}
	}




}

//=============================================================================
// 更新エネミーの動き
//=============================================================================
void UpdateMovement(float moveX, float moveZ, float moveVectorMagnitude, int Idx, float speedMultiplier, float slowInFrames, float slowOutDistance)
{
	
	// 合計の大きさが1より大きい場合、正規化する。// Si la magnitud total es mayor que 1, normalizar
	if (moveVectorMagnitude > 1.0f)
	{
		moveX /= moveVectorMagnitude;
		moveZ /= moveVectorMagnitude;
	}

	//Speed 計算
	float speed = 1.0f;

	//Move Angle
	float moveAngle = atan2f(moveX, moveZ);

	if (moveVectorMagnitude < slowOutDistance)	//Slow out
	{
		speed = moveVectorMagnitude / slowOutDistance;
		if (GetMode() == MODE_FIGHT) moveAngle = XM_PI;
	}
	else if (g_Enemy[Idx].patrolFrames < slowInFrames)	//Slow in
	{
		g_Enemy[Idx].patrolFrames++;
		speed = g_Enemy[Idx].patrolFrames / slowInFrames;
	}
		

	g_Enemy[Idx].pos.x += moveX * speed * speedMultiplier;
	g_Enemy[Idx].pos.z += moveZ * speed * speedMultiplier;


	float angleDifference = moveAngle - g_Enemy[Idx].rot.y;		// 2つの方向間の最短角度を計算する  (ES)Calcular el angulo mas corto entre las dos direcciones
	angleDifference = NormalizeAngle(angleDifference);		// 角度差を正規化する	(ES) Normalizar la diferencia angular
	g_Enemy[Idx].rot.y = lerp(g_Enemy[Idx].rot.y, g_Enemy[Idx].rot.y + angleDifference, LERP_ROT_FACTOR);	//線形補間



	g_Enemy[Idx].animator.animation[SQUELETON_WALK].speed = speed * speedMultiplier * 1.5f;
	if (g_Enemy[Idx].animator.curAnim != SQUELETON_WALK) g_Enemy[Idx].animator.nextAnim = SQUELETON_WALK;
}



//=============================================================================
// エネミーの攻撃（歩き）
//=============================================================================
void EnemyAttack(int Idx)
{
	PLAYER* player = GetPlayer();
	static float slowIn;
	static float slowOut;
	static int attackedIdx;
	static BOOL playerHitted;

	// 移動処理			
	float moveX = g_Enemy[Idx].patrolDest.x - g_Enemy[Idx].pos.x;
	float moveZ = g_Enemy[Idx].patrolDest.z - g_Enemy[Idx].pos.z;


	// 移動量を計算する　　// Calcular la magnitud del movimiento total
	float moveVectorMagnitude = sqrtf(moveX * moveX + moveZ * moveZ);

	if (moveVectorMagnitude < 3.0f)
	{
		static BOOL attacked;

		//パトロールするまでの時間
		if (g_Enemy[Idx].patrolDest.z == player[attackedIdx].pos.z + 50.0f)
		{
			if (attacked)	//既に攻撃した
			{
				if (g_FightPos[Idx].z > g_FightPos->z + 10.0f)	//エネミーが後ろの列に戻りたかったら
				{
					g_Enemy[Idx].patrolDest = g_FightPos[Idx];
					g_Enemy[Idx].patrolDest.z -= 60.0f;
					slowIn = SLOW_IN_FRAMES;
					slowOut = g_FightPos[Idx].z == g_FightPos->z ? SLOW_OUT_DISTANCE : 1.0f;	//先頭の列のエネミーだったら、SLOW OUTする;
				}
				else//エネミーが先頭の列に戻りたかったら
				{
					g_Enemy[Idx].patrolDest = g_FightPos[Idx];
					slowIn = SLOW_IN_FRAMES;
					slowOut = SLOW_OUT_DISTANCE;
				}
				

			}
			else//まだ攻撃していない
			{
				if (g_Enemy[Idx].animator.curAnim != SQUELETON_ATTACK_PUNCH && g_Enemy[Idx].animator.nextAnim != SQUELETON_ATTACK_PUNCH)//攻撃アニメーションが始まっていない 
				{
					g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].AnimTimeCnt = 0.0f;
					g_Enemy[Idx].animator.nextAnim = SQUELETON_ATTACK_PUNCH;
				}
				else//攻撃アニメーション中
				{
					static BOOL attackBlocked;
					static BOOL attackBlockedFailed;
					//
					if (g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].AnimTimeCnt > g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].keyFrameNum - 5.0f &&
						g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].AnimTimeCnt < g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].keyFrameNum - 1.05f)//攻撃アニメーションが終わろうとしている時
					{
						//防御 チャンス　defence chance indicator on screen
						SetActionIndicator(player[attackedIdx].pos, TRUE);
						if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B))
						{
							attackBlocked = TRUE;
						}

					}
					else if (g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].AnimTimeCnt > g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].keyFrameNum - 1.05f)//攻撃アニメーションが終わろうとしている時
					{
						if (!playerHitted)
						{
							//防御	attack blocked
							if (attackBlocked && !attackBlockedFailed)
							{
								PlayerDamaged(0, attackedIdx);						//防御	attack blocked
								SetDamageUI(player[attackedIdx].pos, 0, TRUE, FALSE);
								PlaySound(SOUND_LABEL_SE_Slap);
							}
							//攻撃をくらった
							else
							{
								PlayerDamaged(g_Enemy[Idx].attack, attackedIdx);	//攻撃をくらった
								SetDamageUI(player[attackedIdx].pos, g_Enemy[Idx].attack, TRUE, FALSE);
								SetHitParticle(player[attackedIdx].pos, FALSE);
								PlaySound(SOUND_LABEL_SE_Punch);
							}

							SetActionIndicator(player[attackedIdx].pos, FALSE);

							playerHitted = TRUE;
							attackBlocked = FALSE;
							attackBlockedFailed = FALSE;
						}
					}
					if (g_Enemy[Idx].animator.curAnim == SQUELETON_IDLE)
					{
						attacked = TRUE;
					}

					if ((GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B)) && !attackBlockedFailed && !attackBlocked)
					{
						attackBlockedFailed = TRUE;
					}

				}
			}
		}
		else  //パトロール開始
		{
			if (attacked)	//既に攻撃した
			{
				if (g_Enemy[Idx].patrolDest.z != g_FightPos[Idx].z)
				{//4444
					g_Enemy[Idx].patrolDest = g_FightPos[Idx];
					slowIn = 1.0f;
					slowOut = SLOW_OUT_DISTANCE;
				}
				else
				{//5555
					attacked = FALSE;
					playerHitted = FALSE;
					g_Enemy[Idx].myTurn = FALSE;
					g_Enemy[Idx].turnFinished = TRUE;
					g_Enemy[Idx].animator.nextAnim = SQUELETON_IDLE;
				}
			}
			else
			{
				if (g_Enemy[Idx].pos.z > g_FightPos->z + 10.0f)	//エネミーが後ろの列にいたら、ちょっと進んでからプレイヤーの所に行く
				{
					g_Enemy[Idx].patrolDest = g_FightPos[Idx];
					g_Enemy[Idx].patrolDest.z -= 60.0f;
					slowIn = SLOW_IN_FRAMES;
					slowOut = 1.0f;
				}
				else
				{
					BOOL invalidPlayer = TRUE;
					while (invalidPlayer)
					{
						attackedIdx = rand() % MAX_PLAYER;
						if (player[attackedIdx].use && player[attackedIdx].load && !player[attackedIdx].dead) invalidPlayer = FALSE;
					}

					g_Enemy[Idx].patrolDest = player[attackedIdx].pos;
					g_Enemy[Idx].patrolDest.z += 50.0f;
					slowIn = g_FightPos[Idx].z == g_FightPos->z ? SLOW_IN_FRAMES : 1.0f;	//先頭の列のエネミーだったら、SLOW INする
					slowOut = SLOW_OUT_DISTANCE;
				}
			}
		}

	}
	else
	{
		UpdateMovement(moveX, moveZ, moveVectorMagnitude, Idx, FIGHT_SPEED, slowIn, slowOut);
	}

}


//=============================================================================
// エネミーの攻撃（ジャンプ）
//=============================================================================
void EnemyAttack2(int Idx)
{
	PLAYER* player = GetPlayer();

	static BOOL attacked;
	static BOOL jumped;
	static int attackedIdx;
	static BOOL startJump;

	if (jumped/*g_Enemy[Idx].pos.z - 5 <= player[attackedIdx].pos.z + 50.0f*/) //プレイヤーの前にいる
	{
		if (attacked)	//既に攻撃した
		{
			if (Jumping(Idx, attackedIdx, player[attackedIdx].pos, g_FightPos[Idx]))
			{//333
				attacked = FALSE;
				jumped = FALSE;
				startJump = FALSE;
				g_Enemy[Idx].myTurn = FALSE;
				g_Enemy[Idx].turnFinished = TRUE;
				g_Enemy[Idx].animator.nextAnim = SQUELETON_IDLE;
			}
		}
		else//まだ攻撃していない
		{//222
			if (g_Enemy[Idx].animator.curAnim != SQUELETON_ATTACK_PUNCH && g_Enemy[Idx].animator.nextAnim != SQUELETON_ATTACK_PUNCH)//攻撃アニメーションが始まっていない 
			{
				g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].AnimTimeCnt = 0.0f;
				g_Enemy[Idx].animator.nextAnim = SQUELETON_ATTACK_PUNCH;
			}
			else//攻撃アニメーション中
			{
				if (g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].AnimTimeCnt > g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].keyFrameNum - 1.05f)//攻撃アニメーションが終わろうとしている時
				{
					//player[attackedIdx].animator.nextAnim = P_ANIM_HIT;
					PlayerDamaged(g_Enemy[Idx].attack, attackedIdx);
					SetDamageUI(player[attackedIdx].pos, g_Enemy[Idx].attack, TRUE, FALSE);
					SetHitParticle(player[attackedIdx].pos, FALSE);
					attacked = TRUE;
				}
				//if (g_Enemy[Idx].animator.curAnim == SQUELETON_IDLE)
				//{
				//	attacked = TRUE;
				//}
			}

		}
	}
	else  //パトロール開始
	{
		if (attacked)	//既に攻撃した
		{
			attacked = FALSE;
			//g_Enemy[Idx].attacking = FALSE;
			g_Enemy[Idx].animator.nextAnim = SQUELETON_IDLE;
		}
		else
		{//111
			if (!startJump)
			{
				BOOL invalidPlayer = TRUE;
				while (invalidPlayer)
				{
					attackedIdx = rand() % MAX_PLAYER;
					if (player[attackedIdx].use && player[attackedIdx].load && !player[attackedIdx].dead) invalidPlayer = FALSE;
				}
				startJump = TRUE;
			}

			jumped = Jumping(Idx, attackedIdx, g_FightPos[Idx], player[attackedIdx].pos);
		}
	}
}


//=============================================================================
// 更新エネミーのジャンプ
//=============================================================================
BOOL Jumping(int Idx, int attackedIdx, XMFLOAT3 initPos, XMFLOAT3 finalPos)
{
	//PLAYER* player = GetPlayer();

	if (g_Enemy[Idx].animator.curAnim != SQUELETON_JUMP && g_Enemy[Idx].animator.nextAnim != SQUELETON_JUMP)
	{
		g_Enemy[Idx].animator.nextAnim = SQUELETON_JUMP;
		g_Enemy[Idx].animator.animation[SQUELETON_JUMP].AnimTimeCnt = 0.0f;
	}

	float t = g_Enemy[Idx].animator.animation[SQUELETON_JUMP].AnimTimeCnt / (g_Enemy[Idx].animator.animation[SQUELETON_JUMP].keyFrameNum - 1);

	XMFLOAT3 p0 = initPos;
	if (p0.z < 0)p0.z += 50.0f;
	XMFLOAT3 p1 = p0;
	XMFLOAT3 p2 = finalPos;
	if(p2.z < 0)p2.z += 50.0f;
	XMFLOAT3 p3 = p2;

	float moveX = p3.x - p0.x;
	float moveZ = p3.z - p0.z;
	float moveVectorDistance = sqrtf(moveX * moveX + moveZ * moveZ);

	p1.y += moveVectorDistance * 0.3f;
	p2.y += moveVectorDistance * 0.3f;

	if (t >= 0.97f)
	{
		t = 1;
		//return TRUE;//ジャンル済み
	}
	g_Enemy[Idx].pos = BezierFunction(t, p0, p1, p2, p3);

	return t >= 1 ? TRUE : FALSE; //ジャンル中
}

//**************************
// 生成したエネミーを取得
//**************************
int GetSpawnedEnemies(void)
{
	return g_SpawnedEnemies;
}

//**************************
// エネミーが攻撃をくらう
//**************************
void EnemyDamaged(int damage, int enemyIdx)
{
	if (g_Enemy[enemyIdx].HP <= 0)return;

	g_Enemy[enemyIdx].HP -= damage;

	if (g_Enemy[enemyIdx].HP <= 0)
	{
		g_Enemy[enemyIdx].dead = TRUE;
		g_Enemy[enemyIdx].animator.nextAnim = SQUELETON_DEATH;
		if (g_Enemy[enemyIdx].animator.curAnim == SQUELETON_HIT) g_Enemy[enemyIdx].animator.animation[SQUELETON_HIT].nextAnimation = SQUELETON_DEATH;
	}
	else
	{
		if (g_Enemy[enemyIdx].animator.curAnim == SQUELETON_HIT || g_Enemy[enemyIdx].animator.nextAnim == SQUELETON_HIT)
		{
			g_Enemy[enemyIdx].animator.animation[SQUELETON_HIT].AnimTimeCnt = 0;
		}
		else
		{
			g_Enemy[enemyIdx].animator.nextAnim = SQUELETON_HIT;
		}
	}
}

//**************************
// エネミーの再配置
//**************************
void EnemyRelocation(BOOL Xmov, BOOL positiveMov, float limitPos)
{
	//横移動
	if (Xmov)
	{
		//右移動
		if (positiveMov)
		{
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].pos.x < limitPos)
				{
					g_Enemy[i].pos.x		+= GROUND_WIDTH * 2;
					g_Enemy[i].patrolDest.x += GROUND_WIDTH * 2;
				}
			}
		}
		//左移動
		else
		{
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].pos.x > limitPos)
				{
					g_Enemy[i].pos.x		-= GROUND_WIDTH * 2;
					g_Enemy[i].patrolDest.x -= GROUND_WIDTH * 2;
				}
			}
		}

	}
	//縦移動
	else
	{
		//上移動
		if (positiveMov)
		{
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].pos.z < limitPos)
				{
					g_Enemy[i].pos.z		+= GROUND_HEIGHT * 2;
					g_Enemy[i].patrolDest.z += GROUND_HEIGHT * 2;
				}
			}
		}
		//下移動
		else
		{
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].pos.z > limitPos)
				{
					g_Enemy[i].pos.z		-= GROUND_HEIGHT * 2;
					g_Enemy[i].patrolDest.z -= GROUND_HEIGHT * 2;
				}
			}
		}

	}
}

//****************************
//　エネルギーの当たり判定
//****************************
void EnemyCollider(int idx)
{
	//エネミー同士の当たり判定
	for (int e = 0; e < MAX_ENEMY; e++)
	{
		if ((e == idx) || (g_Enemy[e].use == FALSE))
		{
			continue;
		}

		BOOL ans = TRUE;
		while (ans == TRUE)
		{
			ans = CollisionBC(g_Enemy[idx].pos, g_Enemy[e].pos, g_Enemy[idx].size,
				g_Enemy[e].size);

			// 当たっている？
			if (ans == TRUE)
			{
				XMVECTOR currentEnemy = XMLoadFloat3(&g_Enemy[e].pos);                         //今のエネミーの座標
				XMVECTOR vectorToEnemy = XMLoadFloat3(&g_Enemy[idx].pos) - currentEnemy;       //2つ目のエネミーから1つ目のエネミーまでのVector

				float angle = atan2f(vectorToEnemy.m128_f32[1], vectorToEnemy.m128_f32[0]);

				float XMove = cosf(angle);
				float YMove = sinf(angle);

				g_Enemy[idx].pos.x += XMove;
				g_Enemy[idx].pos.z += YMove;
			}
		}
	}
}