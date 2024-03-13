//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"

#include "gameEnvironment.h"
#include "write.h"
#include "fightUI.h"
#include "pauseMenu.h"

#include "modelBinaryFBX.h"
#include "player.h"
#include "enemy.h"
#include "particle.h"
#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static int 	g_PauseMode;	// ポーズON/OFF




//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	InitGameEnvironment();	//環境の初期化

	InitPlayer();			// プレイヤーの初期化
	InitEnemy();			// エネミーの初期化
	InitParticle();			// パーティクルの初期化

	InitDirectWrite();
	InitFightUI();			// スコアの初期化
	InitPauseMenu();		// スコアの初期化

	g_PauseMode = PAUSE_MODE_OFF;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	UninitParticle();		// パーティクルの終了処理
	UninitEnemy();			// エネミーの終了処理
	UninitPlayer();			// プレイヤーの終了処理

	UninitPauseMenu();
	UninitFightUI();		// スコアの終了処理
	InitDirectWrite();

	UninitGameEnvironment(); //環境の終了処理
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
	if ((GetKeyboardTrigger(DIK_P) || IsButtonTriggered(0, BUTTON_START)) && g_PauseMode != PAUSE_MODE_SCREEN)
	{
		g_PauseMode = g_PauseMode ? FALSE : PAUSE_MODE_MENU;
		MoveMenu();
		PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
	}


	if(g_PauseMode == PAUSE_MODE_SCREEN)return;

	if (g_PauseMode == PAUSE_MODE_MENU)
	{
		UpdatePauseMenu();		// メニューの更新処理
		return;
	}
		
		

	UpdateGameEnvironment();	//環境の終了処理

	UpdatePlayer();				// プレイヤーの更新処理
	UpdateEnemy();				// エネミーの更新処理
	UpdateParticle();			// パーティクルの更新処理
	CheckHit();					// 当たり判定処理
	UpdateFightUI();				// スコアの更新処理
	UpdatePauseMenu();			// PAUSE MENUの更新処理

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame3D(void)
{
	// 3Dの物を描画する処理
	DrawGameEnvironment();	//環境の描画処理

	DrawEnemy();			// エネミーの描画処理
	DrawPlayer();			// プレイヤーの描画処理
	DrawParticle();			// パーティクルの描画処理
}

void DrawGame2D(void)
{
	// 2Dの物を描画する処理
	SetDepthEnable(FALSE);	// Z比較なし
	SetLightEnable(FALSE);	// ライティングを無効

	DrawFightUI();			// スコアの描画処理
	DrawPauseMenu();			// スコアの描画処理

	SetLightEnable(TRUE);	// ライティングを有効に
	SetDepthEnable(TRUE);	// Z比較あり

}


void DrawGame(void)
{
	XMFLOAT3 pos;

	switch (GetMode())
	{
	case MODE_GAME:
		// プレイヤー視点
		pos = GetPlayer()->pos;
		pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
		SetCameraAT(pos);
		SetCamera();
		break;

	case MODE_FIGHT:
		// プレイヤー視点
		pos = XMFLOAT3(0.0f, 0.0f, /*-2*/0.0f);
		SetCameraAT(pos);
		SetCamera();

		break;

	default:
		break;
	}


	SetViewPort(TYPE_FULL_SCREEN);
	DrawGame3D();
	DrawGame2D();

}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// エネミーのポインターを初期化
	PLAYER *player = GetPlayer();	// プレイヤーのポインターを初期化
	BILLBOARD *tree = GetTree();	// 弾のポインターを初期化

	// 敵とプレイヤーキャラ
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//敵の有効フラグをチェックする
		if (enemy[i].use == FALSE)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
		{
			g_PauseMode = PAUSE_MODE_SCREEN;
			SaveEnemyMapInfo(i);
			SavePlayerGameInfo();

			SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_FIGHT);
		}
	}


	if (GetMode() == MODE_GAME)
	{
		for (int i = 0; i < MAX_TREE; i++)
		{
			if (tree[i].use == FALSE)
				continue;


			XMFLOAT3 treePos = tree[i].pos;
			treePos.x -= 8.0f;
			treePos.z -= 8.0f;

			BOOL ans = TRUE;
			while (ans == TRUE)
			{
				ans = CollisionBC(player[0].pos, treePos, player[0].size, 15);

				// 当たっている？
				if (ans == TRUE)
				{
					XMVECTOR currentEnemy = XMLoadFloat3(&tree[i].pos);                         //今のエネミーの座標
					XMVECTOR vectorToEnemy = XMLoadFloat3(&player[0].pos) - currentEnemy;       //2つ目のエネミーから1つ目のエネミーまでのVector

					float angle = atan2f(vectorToEnemy.m128_f32[1], vectorToEnemy.m128_f32[0]);

					float XMove = cosf(angle);
					float YMove = sinf(angle);

					player[0].pos.x += XMove;
					player[0].pos.z += YMove;
				}
			}
		}
	}
}

//=============================================================================
// ポーズモードを取得
//=============================================================================
int GetPauseMode(void)
{
	return g_PauseMode;
}

//=============================================================================
// ポーズモードを設定
//=============================================================================
void SetPauseMode(int pauseMode)
{
	g_PauseMode = pauseMode;
}