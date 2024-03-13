//=============================================================================
//
// モデル処理 [player.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"

#include "modelBinaryFBX.h"
#include "player.h"
#include "debugproc.h"
#include "fade.h"
#include "enemy.h"
#include "fightUI.h"
#include "sound.h"
#include "fightUI.h"
#include "particle.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_WOODCUTTER	"data\\MODEL\\Woodcutter_Model_Data.bin"	// 読み込むモデル名
#define	MODEL_GNOME			"data\\MODEL\\Gnome_Model_Data.bin"			// 読み込むモデル名

#define	VALUE_MOVE			(2.0f)		// 移動量
#define FIGHT_SPEED			(1.5f)		// 攻撃移動速度

#define LERP_GAME_FACTOR	(0.3f)		// LERP速度
#define LERP_FIGHT_FACTOR	(0.1f)		// 回転LERPの速度

#define SLOW_OUT_DISTANCE	(25.0f)		// SLOW OUTが始まる距離
#define SLOW_IN_FRAMES		(30.0f)		// SLOW INがかかるフレーム数

#define DISTANCE_BTW_ENEMY_ROWS (50.0f)	//敵列間の距離// distance Between Enemy Rows

#define OBJ_IDX_AXE (7)					//斧のインデックス

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadPlayerGameInfo(void);
void LoadPlayerFightInfo(void);

void UpdatePlayerGameMode(void);
void UpdatePlayerFightMode(void);

void CheckFightTurn(int idx);

void SelectTargetEnemy(int& targetIdx);
void SelectTargetPlayer(int& targetIdx);

void UpdatePMovement(float moveX, float moveZ, float moveVectorMagnitude, int Idx, float speedMultiplier, float slowInFrames, float slowOutDistance);
void PlayerAttack(int Idx, int& attackedIdx, BOOL& attack);

void PlayerPunchAttack(int Idx, int attackedIdx, BOOL& critFail, BOOL& enemyHitted, BOOL& attacked);
void PlayerAxeAttack(int Idx, int attackedIdx, BOOL& enemyHitted, BOOL& attacked);


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PLAYER_MAP_INFO
{
	XMFLOAT3		pos[MAX_PLAYER];		// モデルの位置
	XMFLOAT3		rot[MAX_PLAYER];		// モデルの向き(回転)
	BOOL			use[MAX_PLAYER];		// 使用フラグ

	int				HPmax	[MAX_PLAYER];	// 最大HP
	int				HPcur	[MAX_PLAYER];	// 現在のHP
	int				agility	[MAX_PLAYER];	// 速度
	int				attack	[MAX_PLAYER];	// 攻撃値
	int				deffense[MAX_PLAYER];	// 防御値

	BOOL			saved;		// 使用フラグ
};

//攻撃状態
enum ATTACK_STATUS
{
	ATTACK_NONE,
	ATTACK_PUNCH,
	ATTACK_AXE,
};



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER			g_Player[MAX_PLAYER];			// プレイヤー

static PLAYER_MAP_INFO	g_PlayerMapInfo;				// ファイトに入る時の為のプレイヤー情報

static XMFLOAT3			g_FightPos[MAX_PLAYER] =		// バトルの待機座標
{
	XMFLOAT3(  0.0f, 0.0f, -70.0f),
	XMFLOAT3( 60.0f, 0.0f, -70.0f),
	XMFLOAT3(-60.0f, 0.0f, -70.0f),
};

static char* g_PlayerModelFile[MAX_PLAYER] =			// モデルファイル
{
	MODEL_WOODCUTTER,
	MODEL_GNOME,
	MODEL_GNOME,
};

static BOOL selectionEnemy;		// 対象エネミーを選んでいる状態
static BOOL selectionPlayer;	// 対象プレイヤーを選んでいる状態

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	//モードによって初期化する
	switch (GetMode())
	{
	case MODE_GAME:
		InitModelBinaryFBX(MODEL_WOODCUTTER, &g_Player[0].model, &g_Player[0].modelData, &g_Player[0].animator);
		g_Player[0].load = TRUE;

		if (g_PlayerMapInfo.saved)
		{
			LoadPlayerGameInfo();
		}
		else
		{
			g_Player[0].pos			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Player[0].rot			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Player[0].scl			 = XMFLOAT3(1.0f, 1.0f, 1.0f);
									 
			g_Player[0].use			 = TRUE;			// TRUE:生きてる
			g_Player[0].size		 = PLAYER_SIZE;		// 当たり判定の大きさ
									 
			for (int i = 0; i < MAX_PLAYER; i++)
			{
				g_Player[i].HPmax		 = 20;
				g_Player[i].HPcur		 = 20;
				g_Player[i].agility		 = 12;
				g_Player[i].attack		 = 10;
				g_Player[i].deffense	 = 10;
				g_Player[i].myTurn		 = FALSE;
				g_Player[i].turnFinished = FALSE;
			}
		}

		break;

	case MODE_FIGHT:
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			InitModelBinaryFBX(g_PlayerModelFile[i], &g_Player[i].model, &g_Player[i].modelData, &g_Player[i].animator);
			g_Player[i].load = TRUE;

			g_Player[i].pos			 = g_FightPos[i];
			g_Player[i].rot			 = XMFLOAT3(0.0f, XM_PI, 0.0f);
			g_Player[i].scl			 = XMFLOAT3(1.0f, 1.0f, 1.0f);

			g_Player[i].use			 = TRUE;			// TRUE:生きてる
			g_Player[i].size		 = PLAYER_SIZE;		// 当たり判定の大きさ

			g_Player[i].myTurn		 = FALSE;
			g_Player[i].turnFinished = FALSE;

			g_Player[i].patrolDest	 = g_Player[i].pos;
			g_Player[i].patrolFrames = 0;

		}
		LoadPlayerFightInfo();

		break;

	default:
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			g_Player[i].pos			 = XMFLOAT3(-10.0f, 0.0f, -50.0f);
			g_Player[i].rot			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Player[i].scl			 = XMFLOAT3(1.0f, 1.0f, 1.0f);

			g_Player[i].use			 = TRUE;			// TRUE:生きてる
			g_Player[i].size		 = PLAYER_SIZE;	// 当たり判定の大きさ

			g_Player[i].HPmax		 = 20;
			g_Player[i].HPcur		 = 20;
			g_Player[i].agility		 = 4;
			g_Player[i].attack		 = 10;
			g_Player[i].deffense	 = 10;
			g_Player[i].myTurn		 = FALSE;
			g_Player[i].turnFinished = FALSE;

			g_Player[i].patrolDest	 = g_Player[i].pos;
			g_Player[i].patrolFrames = 0;

		}

		break;
	}

	

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	//誰かが倒れていたら、HP１にする
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].HPcur <= 0)
		{
			g_Player[i].HPcur = 1;
			g_Player[i].dead = FALSE;
		}
	}

	//MODEを出る時にプレイヤー情報をセーブする
	if (GetMode() == MODE_GAME) SavePlayerGameInfo();		
	if (GetMode() == MODE_FIGHT) SavePlayerFightInfo();	

	//プレイヤーモデルのメモリー解放する
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].load == TRUE)
		{
			UninitModelBinaryFBX(&g_Player[i].model, &g_Player[i].modelData, &g_Player[i].animator);
			g_Player[i].load = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	switch (GetMode())
	{
	case MODE_GAME:
		UpdatePlayerGameMode();
		break;

	case MODE_FIGHT:
		UpdatePlayerFightMode();
		break;

	default:

		break;
	}

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].load && g_Player[i].use)
		{
			if (g_Player[i].animator.curAnim != -1 && g_Player[i].animator.animationNum > 0)
			{
				UpdateModelBinaryFBX(&g_Player[i].modelData, g_Player[i].animator);
			}
		}
	}

#ifdef _DEBUG
	// デバッグ表示
		
	//プレイヤーの座標
	PrintDebugProc("Player X:%f Y:%f Z:% N:%f\n", g_Player[0].pos.x, g_Player[0].pos.y, g_Player[0].pos.z);
	//プレイヤーの回転
	PrintDebugProc("Rotation Y: %f \n", XMConvertToDegrees(g_Player[0].rot.y));
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_BACK);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	//*************
	// プレイヤーの描画
	//*************
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].load == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Player[i].scl.x, g_Player[i].scl.y, g_Player[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);

		// モデル描画
		//DrawModel(&g_Player[i].model);
		DrawModelBinaryFBX(&g_Player[i].model, g_Player[i].modelData, mtxWorld);


		//*************
		// 影の描画
		//*************

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x /*+ XM_PIDIV4*/, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Player[i].scl.x, 0.01f, g_Player[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, 0.01f, g_Player[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);


		//マテリアルのDiffuseを変える
		XMFLOAT4* MaterialDiffuse = new XMFLOAT4[100];
		int matIdx = 0;

		for (int z = 0; z < g_Player[i].model.BuffersNum; z++)
		{
			for (int j = 0; j < g_Player[i].model.Buffers[z].SubsetNum; j++)
			{
				MaterialDiffuse[matIdx] = g_Player[i].model.Buffers[z].SubsetArray[j].Material.Material.Diffuse;
				g_Player[i].model.Buffers[z].SubsetArray[j].Material.Material.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.2f);
				matIdx++;
			}
		}
		

		// 影描画
		DrawModelBinaryFBX(&g_Player[i].model, g_Player[i].modelData, mtxWorld);

		//マテリアルのDiffuseを戻す
		matIdx = 0;
		for (int z = 0; z < g_Player[i].model.BuffersNum; z++)
		{
			for (int j = 0; j < g_Player[i].model.Buffers[z].SubsetNum; j++)
			{
				g_Player[i].model.Buffers[z].SubsetArray[j].Material.Material.Diffuse = MaterialDiffuse[matIdx];
				matIdx++;
			}
		}
		//メモリー解放
		delete[] MaterialDiffuse;

	}


	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player[0];
}


//=============================================================================
// プレイヤーのGAME情報をセーブする
//=============================================================================
void SavePlayerGameInfo(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		g_PlayerMapInfo.pos[i]		= g_Player[i].pos;
		g_PlayerMapInfo.rot[i]		= g_Player[i].rot;
		g_PlayerMapInfo.use[i]		= g_Player[i].use;
		g_PlayerMapInfo.HPmax[i]	= g_Player[i].HPmax;
		g_PlayerMapInfo.HPcur[i]	= g_Player[i].HPcur;
		g_PlayerMapInfo.agility[i]	= g_Player[i].agility;
		g_PlayerMapInfo.attack[i]	= g_Player[i].attack;
		g_PlayerMapInfo.deffense[i] = g_Player[i].deffense;
	}

	g_PlayerMapInfo.saved = TRUE;
}

//=============================================================================
// プレイヤーのFIGHT情報をセーブする
//=============================================================================
void SavePlayerFightInfo(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		g_PlayerMapInfo.HPmax[i]	= g_Player[i].HPmax;
		g_PlayerMapInfo.HPcur[i]	= g_Player[i].HPcur;
		g_PlayerMapInfo.agility[i]	= g_Player[i].agility;
		g_PlayerMapInfo.attack[i]	= g_Player[i].attack;
		g_PlayerMapInfo.deffense[i] = g_Player[i].deffense;
	}

	g_PlayerMapInfo.saved = TRUE;
}


//=============================================================================
// プレイヤーのGAME情報を取得
//=============================================================================
void LoadPlayerGameInfo(void)
{
	if (g_PlayerMapInfo.saved)
	{
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			g_Player[i].pos		 = g_PlayerMapInfo.pos[i];
			g_Player[i].rot		 = g_PlayerMapInfo.rot[i];
			g_Player[i].use		 = g_PlayerMapInfo.use[i];
			g_Player[i].HPmax	 = g_PlayerMapInfo.HPmax[i];
			g_Player[i].HPcur	 = g_PlayerMapInfo.HPcur[i];
			g_Player[i].agility  = g_PlayerMapInfo.agility[i];
			g_Player[i].attack	 = g_PlayerMapInfo.attack[i];
			g_Player[i].deffense = g_PlayerMapInfo.deffense[i];
		}

		g_PlayerMapInfo.saved = FALSE;
	}
}

//=============================================================================
// プレイヤーのFIGHT情報を取得
//=============================================================================
void LoadPlayerFightInfo(void)
{
	if (g_PlayerMapInfo.saved)
	{
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			g_Player[i].HPmax	 = g_PlayerMapInfo.HPmax[i];
			g_Player[i].HPcur	 = g_PlayerMapInfo.HPcur[i];
			g_Player[i].agility  = g_PlayerMapInfo.agility[i];
			g_Player[i].attack	 = g_PlayerMapInfo.attack[i];
			g_Player[i].deffense = g_PlayerMapInfo.deffense[i];
		}

		g_PlayerMapInfo.saved = FALSE;
	}
}


//=============================================================================
// プレイヤーのGAME Update処理
//=============================================================================
void UpdatePlayerGameMode(void)
{
	XMFLOAT3 lastPos = g_Player[0].pos;

	CAMERA* cam = GetCamera();

	// 移動処理
	float moveX = 0.0f;
	float moveZ = 0.0f;

	//キーボード移動入力
	if (GetKeyboardPress(DIK_LEFT)	) moveX = -1.0f;
	if (GetKeyboardPress(DIK_RIGHT)	) moveX =  1.0f;
	if (GetKeyboardPress(DIK_UP)	) moveZ =  1.0f;
	if (GetKeyboardPress(DIK_DOWN)	) moveZ = -1.0f;

	//移動量
	float moveMagnitude = sqrtf(moveX * moveX + moveZ * moveZ);
	//移動量正規化
	if (moveMagnitude > 1.0f)
	{
		moveX /= moveMagnitude;
		moveZ /= moveMagnitude;
	}

	//ゲームパッド移動入力
	if (GetJoystickLevelX(0)) moveX =  GetJoystickLevelX(0);
	if (GetJoystickLevelY(0)) moveZ = -GetJoystickLevelY(0);

	//移動量
	moveMagnitude = sqrtf(moveX * moveX + moveZ * moveZ);
	//移動量正規化
	if (moveMagnitude > 1.0f)
	{
		moveX /= moveMagnitude;
		moveZ /= moveMagnitude;
	}

	//移動処理
	if (moveX != 0.0f || moveZ != 0.0f)
	{
		// 正規化した移動ベクトルの長さをはかる（移動量）
		moveMagnitude = sqrtf(moveX * moveX + moveZ * moveZ);

		// 実際の移動速度をかける
		float speed = VALUE_MOVE * moveMagnitude;

		// 速度によってアニメーターの速度を調整
		g_Player[0].animator.animation[P_ANIM_WALK].speed = speed;


		// 動きを8方向に制限する
		float moveAngle = atan2f(moveZ, moveX);
		float roundedAngle = roundf(moveAngle / XM_PIDIV4) * XM_PIDIV4;
		float adjustedAngle = roundedAngle + XM_PIDIV4;

		// 移動のX成分とZ成分を計算する
		float rotatedMoveX = cosf(adjustedAngle);
		float rotatedMoveZ = sinf(adjustedAngle);

		// スピードを適用し、キャラクターの位置を更新する
		g_Player[0].pos.x += rotatedMoveX * speed;
		g_Player[0].pos.z += rotatedMoveZ * speed;

		// 線形補間（lerp）で回転を滑らかに
		float avanceX = lastPos.x - g_Player[0].pos.x;
		float avanceZ = lastPos.z - g_Player[0].pos.z;
		float targetRotation = atan2f(avanceX, avanceZ);

		float angleDifference = targetRotation - g_Player[0].rot.y;		// 2つの方向間の最短角度を計算する  (ES)Calcular el angulo mas corto entre las dos direcciones

		angleDifference = NormalizeAngle(angleDifference);		// 角度差を正規化する	(ES) Normalizar la diferencia angular


		g_Player[0].rot.y = lerp(g_Player[0].rot.y, g_Player[0].rot.y + angleDifference, LERP_GAME_FACTOR);	//線形補間

		//歩きのアニメーションに変わる
		if (g_Player[0].animator.curAnim != P_ANIM_WALK) g_Player[0].animator.nextAnim = P_ANIM_WALK;

	}
	else
	{
		//回転が大きくなりすぎないように
		if (g_Player[0].rot.y > XM_2PI) g_Player[0].rot.y -= XM_2PI;
		else if (g_Player[0].rot.y < -XM_2PI) g_Player[0].rot.y += XM_2PI;

		//立つアニメーションに変わる
		if (g_Player[0].animator.curAnim != P_ANIM_IDLE) g_Player[0].animator.nextAnim = P_ANIM_IDLE;
	}
}

//=============================================================================
// ターンを決めます
//=============================================================================
void CheckFightTurn(int idx)
{
	ENEMY* enemy = GetEnemy();

	//現在のターンのチェック
	for (int j = 0; j < MAX_PLAYER; j++)
	{
		if (g_Player[j].myTurn) return;
	}
	for (int j = 0; j < MAX_ENEMY; j++)
	{
		if (enemy[j].myTurn) return;
	}

	//誰の番でもないので、次にアクションする人を決める。
	int Idx		   = 0;
	int maxAgility = 0;
	BOOL enemyTurn = FALSE;

	//まだアクションしていないプレイヤーの速さを確認する
	int deadPlayer = 0;
	for (int j = 0; j < MAX_PLAYER; j++)
	{
		if (g_Player[j].dead) deadPlayer++;
		if (!g_Player[j].use || !g_Player[j].load || g_Player[j].dead) continue;
		if (!g_Player[j].turnFinished && g_Player[j].agility > maxAgility)
		{
			Idx = j;
			maxAgility = g_Player[j].agility;
		}
	}
	if (deadPlayer == MAX_PLAYER)//プレイヤー全員倒れたら、タイトル画面へ
	{
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_TITLE);
		return;
	}

	//まだアクションしていないエネルギーの速さを確認する
	int deadEnemy = 0;
	for (int j = 0; j < MAX_ENEMY; j++)
	{
		if (enemy[j].dead) deadEnemy++;
		if (!enemy[j].use || !enemy[j].load || enemy[j].dead) continue;
		if (!enemy[j].turnFinished && enemy[j].agility > maxAgility)
		{
			Idx = j;
			maxAgility = enemy[j].agility;
			enemyTurn = TRUE;
		}
	}
	if (deadEnemy == GetSpawnedEnemies())//エネミー全員倒れたら、ゲーム画面へ
	{
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_GAME);
		return;
	}

	//結果発表（誰が1番速い）
	if (maxAgility == 0)	//ラウンドが終わって、みんなのターンをリセットする
	{
		for (int j = 0; j < MAX_PLAYER; j++)
		{
			if (!g_Player[j].use || !g_Player[j].load) continue;
			g_Player[j].turnFinished = FALSE;
		}
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			if (!enemy[j].use || !enemy[j].load) continue;
			enemy[j].turnFinished = FALSE;
		}
	}
	else if (enemyTurn)//エネミーのターン
	{
		enemy[Idx].myTurn = TRUE;
	}
	else //プレイヤーのターン
	{
		g_Player[Idx].myTurn = TRUE;
		SetActionButton(g_Player[Idx].pos, TRUE);
	}
}


//***********************************
// プレイヤーのバトルモードを更新
//***********************************
void UpdatePlayerFightMode(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes

			{//誰かのターン

				//if (CheckOthersTurnP(i)) continue;
				CheckFightTurn(i);
			}

			//誰の番でもない（このエネミー以外）
			if (g_Player[i].myTurn)	//このエネミーのターンでもない時
			{
				ENEMY* enemy = GetEnemy();

				static int attack;
				static int targetIdx;
				PrintDebugProc("targetIdx: %d \n", targetIdx);

				//攻撃の処理
				if (attack != ATTACK_NONE)
				{
					PlayerAttack(i, targetIdx, attack);
					return;
				}

				//ACTIONボタン
				if (!selectionEnemy && !GetActionMenuUse())
				{
					//ACTION選択
					if (GetKeyboardTrigger(DIK_D) || IsButtonTriggered(0, BUTTON_B))	//通常攻撃
					{
						selectionEnemy = TRUE;
						SetButtonSelected(BUTTON_RIGHT_ATTACK);
						PlaySound(SOUND_LABEL_SE_ChangeButton);
						break;
					}
					else if (GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_A))//そのた（にげる）
					{
						SetActionsMenu(TRUE, BUTTON_DOWN_OTHER);
						SetButtonSelected(BUTTON_DOWN_OTHER);
						PlaySound(SOUND_LABEL_SE_ChangeButton);
						break;
					}
					else if (GetKeyboardTrigger(DIK_A) || IsButtonTriggered(0, BUTTON_X))//スペシャル（特殊攻撃）
					{
						SetActionsMenu(TRUE, BUTTON_LEFT_SPECIAL);
						SetButtonSelected(BUTTON_LEFT_SPECIAL);
						PlaySound(SOUND_LABEL_SE_ChangeButton);
						break;
					}
					else if (GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_Y))//アイテム（回復薬）
					{
						SetActionsMenu(TRUE, BUTTON_UP_ITEM);
						SetButtonSelected(BUTTON_UP_ITEM);
						PlaySound(SOUND_LABEL_SE_ChangeButton);
						break;
					}
				}

				//選択したACTIONを取り消して
				if (selectionEnemy || GetActionMenuUse())
				{
					if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_A))
					{
						selectionEnemy = FALSE;
						selectionPlayer = FALSE;
						SetSelectionPointer(enemy[targetIdx].pos, FALSE);	// Reset Pointer
						SetActionsMenu(FALSE, ACTION_BUTTONS_OFF);
						SetButtonSelected(0);

						PlaySound(SOUND_LABEL_SE_PulseButtonBack);
					}
				}


				//ターゲットを選ぶ処理
				if (selectionEnemy)
				{
					SelectTargetEnemy(targetIdx);	//エネミーを選ぶ処理

					if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B))
					{
						//特殊攻撃
						if (GetActionMenuUse())
						{
							switch (GetSelecteOption())
							{
							case 0://AXE
								attack = ATTACK_AXE;
								break;

							default:
								break;
							}

						}
						//通常攻撃
						else
						{
							attack = ATTACK_PUNCH;
						}
						selectionEnemy = FALSE;

						SetSelectionPointer(enemy[targetIdx].pos, FALSE);	// Reset Pointer
						SetButtonSelected(0);
						SetActionButton(g_Player[0].pos, FALSE);			// Reset Action Button
						SetActionsMenu(FALSE, ACTION_BUTTONS_OFF);

						PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
					}
				}
				else if (selectionPlayer)
				{
					SelectTargetPlayer(targetIdx);//プレイヤーを選ぶ処理

					if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B))
					{
						//　回復効果
						switch (GetSelecteOption())
						{
						case IT_HEAL_POTION_SMALL:
							g_Player[targetIdx].HPcur += 5;
							break;

						case IT_HEAL_POTION_MID:
							g_Player[targetIdx].HPcur += 10;
							break;

						case IT_HEAL_POTION_BIG:
							g_Player[targetIdx].HPcur += 20;
							break;

						default:
							break;
						}

						if (g_Player[targetIdx].HPcur > g_Player[targetIdx].HPmax) g_Player[targetIdx].HPcur = g_Player[targetIdx].HPmax;
						SetHitParticle(g_Player[targetIdx].pos);

						selectionPlayer = FALSE;
						g_Player[i].myTurn = FALSE;				//ターン	フラグ
						g_Player[i].turnFinished = TRUE;		//このラウンドで既にACTIONを行った　フラグ

						SetSelectionPointer(g_Player[targetIdx].pos, FALSE);	// Reset Pointer
						SetButtonSelected(0);
						SetActionButton(g_Player[0].pos, FALSE);			// Reset Action Button
						SetActionsMenu(FALSE, ACTION_BUTTONS_OFF);

						PlaySound(SOUND_LABEL_SE_Heal);
						break;
					}

				}

			
#ifdef _DEBUG
				//プレイヤーターンを飛ばす
				if (GetKeyboardTrigger(DIK_G))
				{
					g_Player[i].myTurn = FALSE;
					g_Player[i].turnFinished = TRUE;
					SetActionButton(g_Player[i].pos, FALSE);
				}
#endif // DEBUG


			}
		}
	}
}


//******************************************
// エネルギーターゲットを選択
//******************************************
void SelectTargetEnemy(int& targetIdx)
{
	ENEMY* enemy = GetEnemy();

	if (!GetSelectionPointerState())
	{
		for (int e = 0; e < MAX_ENEMY + 1; e++)//最初のターゲット
		{
			targetIdx = e;
			if (enemy[e].use && !enemy[e].dead) break;
		}
		SetSelectionPointer(enemy[targetIdx].pos, TRUE);
	}


	BOOL invalidTarget = TRUE;

	if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_JS_RIGHT) || IsButtonTriggered(0, BUTTON_ARROW_RIGHT))
	{
		while (invalidTarget)
		{
			targetIdx = targetIdx <= 0 ? GetSpawnedEnemies() - 1 : --targetIdx;

			if (enemy[targetIdx].use && !enemy[targetIdx].dead) invalidTarget = FALSE;
		}
		SetSelectionPointer(enemy[targetIdx].pos, TRUE);
	}
	else if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_JS_LEFT) || IsButtonTriggered(0, BUTTON_ARROW_LEFT))
	{
		while (invalidTarget)
		{
			targetIdx = (targetIdx + 1) % GetSpawnedEnemies();

			if (enemy[targetIdx].use && !enemy[targetIdx].dead) invalidTarget = FALSE;
		}
		SetSelectionPointer(enemy[targetIdx].pos, TRUE);
	}
	else if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_JS_DOWN) || IsButtonTriggered(0, BUTTON_ARROW_DOWN))
	{
		while (invalidTarget)
		{
			for (int i = 0; i < 3; i++)
			{
				targetIdx = targetIdx <= 0 ? GetSpawnedEnemies() - 1 : --targetIdx;
			}

			if (enemy[targetIdx].use && !enemy[targetIdx].dead) invalidTarget = FALSE;
		}
		SetSelectionPointer(enemy[targetIdx].pos, TRUE);
	}
	else if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_JS_UP) || IsButtonTriggered(0, BUTTON_ARROW_UP))
	{
		while (invalidTarget)
		{
			for (int i = 0; i < 3; i++)
			{
				targetIdx = (targetIdx + 1) % GetSpawnedEnemies();
			}

			if (enemy[targetIdx].use && !enemy[targetIdx].dead) invalidTarget = FALSE;
		}
		SetSelectionPointer(enemy[targetIdx].pos, TRUE);
	}
}

//******************************************
// プレイヤーターゲットを選択
//******************************************
void SelectTargetPlayer(int& targetIdx)
{
	if (!GetSelectionPointerState())
	{
		for (int p = 0; p < MAX_PLAYER + 1; p++)//最初のターゲット
		{
			targetIdx = p;
			if (g_Player[p].use && !g_Player[p].dead) break;
		}
		SetSelectionPointer(g_Player[targetIdx].pos, TRUE);
	}

	BOOL invalidTarget = TRUE;

	if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_JS_LEFT) || IsButtonTriggered(0, BUTTON_ARROW_LEFT))
	{
		while (invalidTarget)
		{
			targetIdx = targetIdx <= 0 ? MAX_PLAYER - 1 : --targetIdx;

			if (g_Player[targetIdx].use && !g_Player[targetIdx].dead) invalidTarget = FALSE;
		}
		SetSelectionPointer(g_Player[targetIdx].pos, TRUE);
	}
	else if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_JS_RIGHT) || IsButtonTriggered(0, BUTTON_ARROW_RIGHT))
	{
		while (invalidTarget)
		{
			targetIdx = (targetIdx + 1) % MAX_PLAYER;

			if (g_Player[targetIdx].use && !g_Player[targetIdx].dead) invalidTarget = FALSE;
		}
		SetSelectionPointer(g_Player[targetIdx].pos, TRUE);
	}
}


//******************************************
// プレイヤーの攻撃
//******************************************
void PlayerAttack(int Idx, int& attackedIdx, BOOL& attack)
{
	ENEMY* enemy = GetEnemy();
	static float slowIn;		//動きの Slow In
	static float slowOut;		//動きの Slow Out
	static BOOL enemyHitted;	//エネミーに攻撃した
	static BOOL critFail;		//クリティカルが失敗した

	// 移動処理			
	float moveX = g_Player[Idx].patrolDest.x - g_Player[Idx].pos.x;
	float moveZ = g_Player[Idx].patrolDest.z - g_Player[Idx].pos.z;


	// 移動量を計算する		// Calcular la magnitud del movimiento total
	float moveVectorMagnitude = sqrtf(moveX * moveX + moveZ * moveZ);

	if (moveVectorMagnitude < 3.0f)
	{
		static BOOL attacked;

		//パトロールするまでの時間
		if (g_Player[Idx].patrolDest.z == enemy[attackedIdx].pos.z - DISTANCE_BTW_ENEMY_ROWS)
		{
			if (attacked)	//既に攻撃した状態なら、待機の場所に戻る
			{
				{
					g_Player[Idx].patrolDest = g_FightPos[Idx];
					slowIn = SLOW_IN_FRAMES;
					slowOut = SLOW_OUT_DISTANCE;
					g_Player[Idx].animator.hiddenObj[0] = OBJ_IDX_AXE;	//斧を非表示
				}


			}
			else//まだ攻撃していない
			{
				if (attack == ATTACK_PUNCH)
				{
					PlayerPunchAttack(Idx, attackedIdx, critFail, enemyHitted, attacked);
				}
				else if (attack == ATTACK_AXE)
				{
					PlayerAxeAttack(Idx, attackedIdx, enemyHitted, attacked);
				}
			}
		}
		else  //パトロール開始
		{
			if (attacked)	//既に攻撃した
			{
				//5
				attack = ATTACK_NONE;					//攻撃状態	
				attacked = FALSE;						//既に攻撃した	フラグ
				critFail = FALSE;
				enemyHitted = FALSE;					//エネミーがダメージを受けたフラグ
				g_Player[Idx].myTurn = FALSE;			//ターン		フラグ
				g_Player[Idx].turnFinished = TRUE;		//このラウンドで既にACTIONを行った　フラグ
				g_Player[Idx].animator.nextAnim = P_ANIM_IDLE;
				g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].nextAnimation = P_ANIM_IDLE;

				attackedIdx = 0;
				
			}
			else
			{
				//1
				g_Player[Idx].patrolDest = enemy[attackedIdx].pos;
				g_Player[Idx].patrolDest.z -= DISTANCE_BTW_ENEMY_ROWS;
				slowIn = SLOW_IN_FRAMES;
				slowOut = SLOW_OUT_DISTANCE;
				
			}
		}

	}
	else
	{
		UpdatePMovement(moveX, moveZ, moveVectorMagnitude, Idx, FIGHT_SPEED, slowIn, slowOut);
	}

}



//******************************************
// プレイヤーのパンチ攻撃
//******************************************
void PlayerPunchAttack(int Idx, int attackedIdx, BOOL &critFail, BOOL& enemyHitted, BOOL& attacked)
{
	ENEMY* enemy = GetEnemy();

	if (g_Player[Idx].animator.curAnim != P_ANIM_ATTACK_PUNCH && g_Player[Idx].animator.nextAnim != P_ANIM_ATTACK_PUNCH && g_Player[Idx].animator.curAnim != P_ANIM_ATTACK_CRIT)//攻撃アニメーションが始まっていない 
	{
		g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].AnimTimeCnt = 0.0f;
		g_Player[Idx].animator.nextAnim = P_ANIM_ATTACK_PUNCH;
	}
	else//攻撃アニメーション中
	{
		//クリティカル可能時間範囲	//Critical chance range
		if (g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].AnimTimeCnt > g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].keyFrameNum - 2.0f &&
			g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].AnimTimeCnt < g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].keyFrameNum - 1.05f)//攻撃アニメーションが終わろうとしている時
		{

			SetActionIndicator(g_Player[Idx].pos, TRUE);	//画面にクリティカルチャンスを表示	//critical chance indicator on screen

			if ((GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B)) && !critFail)
			{
				g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].nextAnimation = P_ANIM_ATTACK_CRIT;
			}
		}
		//Enemyに当たった時
		if (g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].AnimTimeCnt > g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].keyFrameNum - 1.05f)//攻撃アニメーションが終わろうとしている時
		{
			//エネミーがダメージを受けた	//Enemy damaged
			if (!enemyHitted)
			{
				EnemyDamaged(g_Player[Idx].attack, attackedIdx);
				SetDamageUI(enemy[attackedIdx].pos, g_Player[Idx].attack, TRUE, FALSE);
				enemyHitted = TRUE;
				SetActionIndicator(g_Player[Idx].pos, FALSE);
				SetHitParticle(enemy[attackedIdx].pos, FALSE);
				PlaySound(SOUND_LABEL_SE_Punch);
			}
		}
		else if (enemyHitted == TRUE && g_Player[Idx].animator.animation[P_ANIM_ATTACK_CRIT].AnimTimeCnt < 1.0f)
		{
			enemyHitted = FALSE;
		}
		else if (g_Player[Idx].animator.animation[P_ANIM_ATTACK_CRIT].AnimTimeCnt > g_Player[Idx].animator.animation[P_ANIM_ATTACK_CRIT].keyFrameNum - 7.5f)//攻撃アニメーションが終わろうとしている時
		{
			//Enemy damaged
			if (!enemyHitted)
			{
				EnemyDamaged((int)(g_Player[Idx].attack * 0.5f), attackedIdx);
				SetDamageUI(enemy[attackedIdx].pos, g_Player[Idx].attack + (int)(g_Player[Idx].attack * 0.5f), TRUE, TRUE);

				enemyHitted = TRUE;
				SetActionIndicator(g_Player[Idx].pos, FALSE);

				SetHitParticle(enemy[attackedIdx].pos, TRUE);

				PlaySound(SOUND_LABEL_SE_Punch);
			}
		}
		if (g_Player[Idx].animator.curAnim == P_ANIM_IDLE)
		{
			attacked = TRUE;
		}

		if ((GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B)) && !critFail)
		{
			critFail = TRUE;
		}
	}
			
}


//******************************************
// プレイヤーの斧攻撃
//******************************************
void PlayerAxeAttack(int Idx, int attackedIdx, BOOL& enemyHitted, BOOL& attacked)
{
	ENEMY* enemy = GetEnemy();

	if (g_Player[Idx].animator.curAnim != P_ANIM_ATTACK_AXE && g_Player[Idx].animator.nextAnim != P_ANIM_ATTACK_AXE && g_Player[Idx].animator.curAnim != P_ANIM_ATTACK_CRIT)//攻撃アニメーションが始まっていない 
	{
		g_Player[Idx].animator.animation[P_ANIM_ATTACK_AXE].AnimTimeCnt = 0.0f;
		g_Player[Idx].animator.nextAnim = P_ANIM_ATTACK_AXE;
		g_Player[Idx].animator.hiddenObj[0] = -1;
	}
	else//攻撃アニメーション中
	{
		//Enemyに当たった時
		if (g_Player[Idx].animator.animation[P_ANIM_ATTACK_AXE].AnimTimeCnt > g_Player[Idx].animator.animation[P_ANIM_ATTACK_AXE].keyFrameNum - 6.05f)//攻撃アニメーションが終わろうとしている時
		{
			//エネミーがダメージを受けた	//Enemy damaged
			if (!enemyHitted)
			{
				EnemyDamaged(g_Player[Idx].attack * 2, attackedIdx);
				SetDamageUI(enemy[attackedIdx].pos, g_Player[Idx].attack * 2, TRUE, FALSE);
				enemyHitted = TRUE;
				SetActionIndicator(g_Player[Idx].pos, FALSE);
				SetHitParticle(enemy[attackedIdx].pos, TRUE);
				SetHitParticle(enemy[attackedIdx].pos, TRUE);
				PlaySound(SOUND_LABEL_SE_Punch);
			}
		}
		if (g_Player[Idx].animator.curAnim == P_ANIM_IDLE)
		{
			attacked = TRUE;
		}
	}

}




//******************************************
// プレイヤー移動　更新処理
//******************************************
void UpdatePMovement(float moveX, float moveZ, float moveVectorMagnitude, int Idx, float speedMultiplier, float slowInFrames, float slowOutDistance)
{
	// 正規化
	if (moveVectorMagnitude > 1.0f)
	{
		moveX /= moveVectorMagnitude;
		moveZ /= moveVectorMagnitude;
	}

	//Speed 計算
	float speed = 1.0f;

	//Move Angle
	float moveAngle = atan2f(moveX, moveZ) + XM_PI;

	if (moveVectorMagnitude < slowOutDistance)	//Slow out
	{
		speed = moveVectorMagnitude / slowOutDistance;
		if (GetMode() == MODE_FIGHT) moveAngle = XM_PI;
	}
	else if (g_Player[Idx].patrolFrames < slowInFrames)	//Slow in
	{
		g_Player[Idx].patrolFrames++;
		speed = g_Player[Idx].patrolFrames / slowInFrames;
	}


	g_Player[Idx].pos.x += moveX * speed * speedMultiplier;
	g_Player[Idx].pos.z += moveZ * speed * speedMultiplier;


	float angleDifference = moveAngle - g_Player[Idx].rot.y;		// 2つの方向間の最短角度を計算する  (ES)Calcular el angulo mas corto entre las dos direcciones
	angleDifference = NormalizeAngle(angleDifference);		// 角度差を正規化する	(ES) Normalizar la diferencia angular
	g_Player[Idx].rot.y = lerp(g_Player[Idx].rot.y, g_Player[Idx].rot.y + angleDifference, LERP_FIGHT_FACTOR);	//線形補間



	g_Player[Idx].animator.animation[P_ANIM_WALK].speed = speed * speedMultiplier * 1.5f;
	if (g_Player[Idx].animator.curAnim != P_ANIM_WALK) g_Player[Idx].animator.nextAnim = P_ANIM_WALK;

}


//******************************************
// プレイヤーがダメージを受けた
//******************************************
void PlayerDamaged(int damage, int playerIdx)
{
	if (damage == 0)
	{
		//block anim
		g_Player[playerIdx].animator.nextAnim = P_ANIM_BLOCK;
		return;
	}
	g_Player[playerIdx].HPcur -= damage;

	if (g_Player[playerIdx].HPcur <= 0)
	{
		//g_Enemy[enemyIdx].use = FALSE;
		g_Player[playerIdx].HPcur = 0;
		g_Player[playerIdx].dead = TRUE;
		g_Player[playerIdx].animator.nextAnim = P_ANIM_DEATH;
	}
	else
	{
		g_Player[playerIdx].animator.nextAnim = P_ANIM_HIT;
	}
}


//******************************************
// エネミーターゲット選択モードをTRUEにする
//******************************************
void SetSelectionEnemy(void)
{
	selectionEnemy = TRUE;
}

//******************************************
// プレイヤーターゲット選択モードをTRUEにする
//******************************************
void SetSelectionPlayer(void)
{
	selectionPlayer = TRUE;
}