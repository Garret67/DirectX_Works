//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "player.h"
#include "input.h"
#include "bg.h"
#include "platforms.h"
#include "interactiveObject.h"
#include "bullet.h"
#include "enemyFlyBulletOne.h"
#include "enemyFlyBounce.h"
#include "enemyKnight.h"
#include "enemyBall.h"
#include "enemyPatrol.h"
#include "enemySlimeKing.h"
#include "enemyBoss.h"
#include "collision.h"
#include "result.h"
#include "screenGameOver.h"
#include "file.h"
#include "fade.h"
#include "sound.h"

#include "d3d9.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(60.0f)		// キャラサイズ
#define TEXTURE_HEIGHT				(81.6f)		// キャラサイズ

#define TEXTURE_PATTERN_DIVIDE_X	(9)			// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)			// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)			// アニメーションの切り替わるWait値

// プレイヤーの画面内配置座標
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

// プレイヤーのUI
#define PLAYER_HP_MAX				(10)
#define PLAYER_ENERGY_MAX			(8)

// ジャンプ処理
#define	PLAYER_JUMP_CNT_MAX			(30)		// 30フレームで着地する
#define	PLAYER_JUMP_Y_MAX			(100.0f)	// ジャンプの高さ

//ダッシュ処理
#define	PLAYER_DASH_DISTANCE		(300.0f)	// ダッシュの距離

//Slash処理
#define	PLAYER_SLASH_DISTANCE		(70.0f)		// Slashの距離
#define SLASH_TEXTURE_WIDTH			(465 / 4)	// Slash 幅
#define SLASH_TEXTURE_HEIGHT		(348 / 4)	// Slash 高さ

//エナジー消費
#define	BULLET_ENERGY_COST			(2)			// 魔法の消費
#define HEAL_ENERGY_COST			(2)			// 回復の消費

//無敵状態
#define INVINCIBLE_TIME				(80)		//無敵時間

//重力
#define GRAVITY_FORCE_PER_FRAME		(11.5f)		//重力

//SlowMotionTime
#define SLOW_MOTION_TIME			(80)		//スローモーション時間



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void JumpUpdate(void);
void SlashUpdate(void);
void DashUpdate(void);
void DamageUpdate(void);
void InvincibleUpdate(void);
void FocusUpdate(void);

void AnimationUpdate(void);

void CollisionPlatforms(void);
void CollisionSlash(void);

void PlayerInvincible(int invincibleTime);

void DrawPlayerOffset(int no);
void DrawPlayerSlash(void);
void DrawSlashHit(void);
void DrawFocusEffect(void);
void DrawExclamation(void);

void EnemyHitted(XMFLOAT3 slashPos, XMFLOAT3 enemyPos);

float Lerp(float start, float end, float t);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[ANIM_TEX_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[ANIM_TEX_MAX] = {
	
	"data/TEXTURE/Player/Idle.png",
	"data/TEXTURE/Player/Run.png",
	"data/TEXTURE/Player/Jump.png",
	"data/TEXTURE/Player/Fall.png",
	"data/TEXTURE/Player/Attack2.png",
	"data/TEXTURE/Player/Dash.png",
	"data/TEXTURE/Player/DashOffset.png",
	"data/TEXTURE/Player/Hit.png",
	"data/TEXTURE/Player/Focus.png",
	"data/TEXTURE/Player/Dead.png",
	"data/TEXTURE/Effect/Slash.png",
	"data/TEXTURE/Effect/SlashHit.png",
	"data/TEXTURE/Effect/FocusEffect.png",
	"data/TEXTURE/Effect/Exclamation.png",
};


static PLAYER	g_Player[PLAYER_MAX];					// プレイヤー構造体

static int		g_Texture_Divide_X		= 5;			// 今のアニメパターンのテクスチャ内分割数（X)
static int		g_P_Texture_No			= ANIM_IDLE;	// 今のテクスチャ

static XMFLOAT3 g_SlashHitPos;							//SlashHit座標
static float	g_SlashHitRot;							//SlashHit回転
static int		g_SlashHitFrames		= 14;			//SlashHitフレーム
static int		g_SlashHitFramesCurrent = 16;			//SlashHitフレームカウント
static int		g_SlashHitTexPatter		= 0;			//SlashHitテクスチャパターン

DWORD			prevTime = timeGetTime();				// ループの最初の時間
static int		g_SlowMotionCnt;						// スローモーションカウント
BOOL			g_Exclamation;							//チュートリアルのはてな

static BOOL		g_Load = FALSE;							// 初期化を行ったかのフラグ



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < ANIM_TEX_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	g_SlowMotionCnt = 0;
	SetFinalScreen(SCREEN_INGAME);
	SetPauseScreen(FALSE);
	SetGameNormalSpeed();

	BG* bg = GetBG();
	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].alive	  = TRUE;

		switch (GetMode())
		{
		case MODE_TUTORIAL_1:
			g_Player[i].pos		= XMFLOAT3(SCREEN_CENTER_X, bg->h - 150.0f, 0.0f);
			g_Player[i].lifes	= 7;	//チュートリアルでは体力が下がる
			g_Player[i].energy	= 5;	//チュートリアルではエネルギーが下がる
			break;

		case MODE_TUTORIAL_2:
			g_Player[i].pos		= XMFLOAT3(400.0f,			bg->h - 150.0f, 0.0f);
			g_Player[i].lifes	= 7;	//チュートリアルでは体力が下がる
			g_Player[i].energy	= 5;	//チュートリアルではエネルギーが下がる;
			break;

		case MODE_COLISEUM:
			g_Player[i].pos		= XMFLOAT3(bg->w / 2,		bg->h - 150.0f, 0.0f);
			g_Player[i].lifes	= PLAYER_HP_MAX;
			g_Player[i].energy	= PLAYER_ENERGY_MAX;
			break;
		}
		
		g_Player[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w	  = TEXTURE_WIDTH;
		g_Player[i].h	  = TEXTURE_HEIGHT;
		g_Player[i].texNo = 0;


		g_Player[i].lifesMax  = PLAYER_HP_MAX;
		g_Player[i].energyMax = PLAYER_ENERGY_MAX;


		//動きの初期化
		g_Player[i].moveSpeed = 6.0f;				// 移動量
		g_Player[i].dir		  = CHAR_DIR_RIGHT;		// 右向き
		g_Player[i].Lastdir	  = CHAR_DIR_RIGHT;		// 右向き
		g_Player[i].moving	  = FALSE;				// 移動中フラグ


		//アニメーション初期化
		g_Player[i].countAnim   = 0;
		g_Player[i].patternAnim = 0;		//最初にプレイヤーが下に向けるように
		g_Player[i].patternAnim = g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X;


		// ジャンプの初期化
		g_Player[i].jump		   = FALSE;
		g_Player[i].extendJump	   = FALSE;
		g_Player[i].extendJumpCnt = 0;
		g_Player[i].extendJumpMax = 15;
		g_Player[i].jumpCnt	   = 0;
		g_Player[i].jumpYMax	   = PLAYER_JUMP_Y_MAX;

		g_Player[i].onGround	   = FALSE;


		// dashの初期化
		g_Player[i].dash		 = FALSE;
		g_Player[i].dashReload	 = TRUE;
		g_Player[i].dashFrames   = 15;
		g_Player[i].dashCnt		 = 0;
		g_Player[i].dashDistance = 300.0f;
		g_Player[i].slashDmg	 = 10.0f;

		// Slashの初期化
		g_Player[i].slash				= FALSE;
		g_Player[i].slashCollider		= FALSE;
		g_Player[i].slashFrames			= 20;
		g_Player[i].slashColliderFrames = 5;
		g_Player[i].slashCnt			= 0;
		g_Player[i].slashDistance		= PLAYER_SLASH_DISTANCE;

		//ダメージを受ける時の初期化
		g_Player[i].damaged			= FALSE;
		g_Player[i].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].dmgTime			= 20;
		g_Player[i].dmgTimeCnt		= 0;

		g_Player[i].playerColor		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


		//無敵初期化
		g_Player[i].invincible		  = FALSE;
		g_Player[i].invincibleTime	  = 0;
		g_Player[i].invincibleTimeCnt = 0;

		//集中初期化
		g_Player[i].focus			  = FALSE;
		g_Player[i].focusTimeMax	  = 60;
		g_Player[i].focusTimeCnt	  = 0;
		g_Player[i].focusEffectSprite = 0;



		// 分身用
		for (int j = 0; j < PLAYER_OFFSET_CNT; j++)
		{
			g_Player[i].offset[j] = g_Player[i].pos;
		}
	}

	g_Exclamation = FALSE;

	InitPlayerUI();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < ANIM_TEX_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	UninitPlayerUI();

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{

	for (int i = 0; i < PLAYER_MAX; i++)
	{

		// アニメーション  
		AnimationUpdate();
		
		
		// 地形との当たり判定用に座標のバックアップを取っておく
		XMFLOAT3 pos_old = g_Player[i].pos;

		// ダッシュの影
		for (int j = PLAYER_OFFSET_CNT - 1; j > 0; j--)
		{
			g_Player[i].offset[j] = g_Player[i].offset[j - 1];
		}
		g_Player[i].offset[0] = pos_old;


		//*****************************************************************************
		//  キー入力
		//*****************************************************************************
			
		//ポーズ Mode
		if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_START))
		{
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
			SetPauseScreen(TRUE);
		}

		//プレイヤーが生きている間の処理
		if(g_Player[i].alive == TRUE)
		{
			//ダッシュしていたら、処理をしない
			if (g_Player[i].dash == FALSE) 
			{
				g_Player[i].moving = FALSE;


				// キー入力で上下向き
				if (GetKeyboardPress(DIK_DOWN)	  || IsButtonPressed(0, BUTTON_DOWN))		// 下
				{
					g_Player[i].dir = CHAR_DIR_DOWN;
				}
				else if (GetKeyboardPress(DIK_UP) || IsButtonPressed(0, BUTTON_UP))			// 上
				{
					g_Player[i].dir = CHAR_DIR_UP;
				}
				else
				{
					g_Player[i].dir = g_Player[i].Lastdir;
				}

				// キー入力で左右移動 
				if (GetKeyboardPress(DIK_RIGHT)		|| IsButtonPressed(0, BUTTON_RIGHT))	// 右
				{
					g_Player[i].pos.x += g_Player[i].moveSpeed;
					g_Player[i].dir = CHAR_DIR_RIGHT;
					g_Player[i].Lastdir = CHAR_DIR_RIGHT;
					g_Player[i].moving = TRUE;
				}
				else if (GetKeyboardPress(DIK_LEFT) || IsButtonPressed(0, BUTTON_LEFT))		// 左
				{
					g_Player[i].pos.x -= g_Player[i].moveSpeed;
					g_Player[i].dir = CHAR_DIR_LEFT;
					g_Player[i].Lastdir = CHAR_DIR_LEFT;
					g_Player[i].moving = TRUE;
				}


				// 魔法設定
				if (GetKeyboardTrigger(DIK_F) || IsButtonTriggered(0, BUTTON_R))	// バレット処理
				{
					if (g_Player[0].energy >= BULLET_ENERGY_COST) //Bullet cost
					{
						PlaySound(SOUND_LABEL_SE_MagicCast);
						g_Player[0].energy -= BULLET_ENERGY_COST;
						UpdateEnergyUI();
						SetBulletPlayer(g_Player[i].pos, g_Player[i].dir);
					}
				}

	
				FocusUpdate();	// 集中    処理
				JumpUpdate();	// ジャンプ処理
				SlashUpdate();	// Slash   処理
			}

			DashUpdate();	//ダッシュ処理



			// 現状をセーブする
			/*if (GetKeyboardTrigger(DIK_S))
			{
				SaveData();
			}*/
		}

		//*****************************************************************************
		//  状態
		//*****************************************************************************

		//重力
		if (g_Player[i].dash == FALSE)
		{
			g_Player[i].pos.y += GRAVITY_FORCE_PER_FRAME;
		}


		DamageUpdate();		//傷付く処理

		InvincibleUpdate(); //無敵処理

			


		//*****************************************************************************
		//  当たり判定
		//*****************************************************************************
		CollisionPlatforms();	// PLATFORMSとの当たり判定
		CollisionSlash();		// SLASHとエネミーの当たり判定

		// MAP外チェック
		BG* bg = GetBG();

		if (g_Player[i].pos.x < g_Player[i].w * 0.5f)
		{
			g_Player[i].pos.x = g_Player[i].w * 0.5f;
		}

		if (g_Player[i].pos.x > bg->w - g_Player[i].w * 0.5f)
		{
			g_Player[i].pos.x = bg->w - g_Player[i].w * 0.5f;
		}

		if (g_Player[i].pos.y < g_Player[i].h * 0.5f)
		{
			g_Player[i].pos.y = g_Player[i].h * 0.5f;
		}

		if (g_Player[i].pos.y > bg->h - g_Player[i].h * 0.5f)
		{
			g_Player[i].pos.y = bg->h - g_Player[i].h * 0.5f;
		}


		//*****************************************************************************
		//  MAPのスクロール
		// プレイヤーの立ち位置からMAPのスクロール座標を計算する
		//*****************************************************************************
		{
			DWORD currentTime = timeGetTime();						// ループ開始時間
			float deltaTime = (currentTime - prevTime) / 1000.0f;	// deltaTime を秒単位で計算
			prevTime = currentTime;									// 次のフレームのために現在時刻を保存する

			float newCameraX = Lerp(bg->pos.x, g_Player[i].pos.x - PLAYER_DISP_X, deltaTime * 3);
			float newCameraY = Lerp(bg->pos.y, g_Player[i].pos.y - PLAYER_DISP_Y + 200.0f, deltaTime * 3);

			// カメラが端から外れないように制限をかける
			if (newCameraX < 0) newCameraX = 0;
			if (newCameraX > bg->w - SCREEN_WIDTH) newCameraX = bg->w - SCREEN_WIDTH;

			if (newCameraY < 0) newCameraY = 0;
			if (newCameraY > bg->h - SCREEN_HEIGHT) newCameraY = bg->h - SCREEN_HEIGHT;

			bg->pos.x = newCameraX;
			bg->pos.y = newCameraY;

		}
		
		
	}

	UpdatePlayerUI();	//UI処理

	//プレイヤーが死んだら、スローモーション
	if (g_Player[0].alive == FALSE)
	{
		g_SlowMotionCnt++;

		if (g_SlowMotionCnt == SLOW_MOTION_TIME)
		{
			SetGameNormalSpeed();
			SetScreenGameOver();
		}
	}


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		{
			// プレイヤーのSlash
			DrawPlayerSlash();
			DrawSlashHit();
			

			// プレイヤーの分身を描画
			DrawPlayerOffset(i);
			

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_P_Texture_No]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_Player[i].pos.x - bg->pos.x;			// プレイヤーの表示位置X
			float py = g_Player[i].pos.y - bg->pos.y;			// プレイヤーの表示位置Y
			float pw = g_Player[i].w;							// プレイヤーの表示幅
			float ph = g_Player[i].h;							// プレイヤーの表示高さ

			// アニメーション用
			float tw = 1.0f / g_Texture_Divide_X;				// テクスチャの幅
			float th = 1.0f;									// テクスチャの高さ
			float tx = (float)(g_Player[i].patternAnim) * tw;	// テクスチャの左上X座標
			float ty = 0.0f;									// テクスチャの左上Y座標

			//左向きの調整
			if (g_Player[i].dir == CHAR_DIR_LEFT)
			{
				tx += tw;
				tw *= -1.0f;
			}
			

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_Player[i].playerColor,
				g_Player[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}
	}
	DrawExclamation();		//はてなの描画

	DrawFocusEffect();		//回復のエフェクト描画

	DrawPlayerUI();			//UI描画
}


//=============================================================================
// Player構造体の先頭アドレスを取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}


//=============================================================================
// プレイヤーの分身を描画
//=============================================================================
void DrawPlayerOffset(int no)
{
	if (!g_Player[0].dash) return;

	BG* bg = GetBG();
	float alpha = 0.0f;

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ANIM_DASH_OFFSET]);

	for (int j = PLAYER_OFFSET_CNT - 1; j >= 0; j--)
	{
		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_Player[no].offset[j].x - bg->pos.x;	// プレイヤーの表示位置X
		float py = g_Player[no].offset[j].y - bg->pos.y;	// プレイヤーの表示位置Y
		float pw = g_Player[no].w;							// プレイヤーの表示幅
		float ph = g_Player[no].h;							// プレイヤーの表示高さ

		// アニメーション用
		float tw = 1.0f;									// テクスチャの幅
		float th = 1.0f;									// テクスチャの高さ
		float tx = 0.0f;									// テクスチャの左上X座標
		float ty = 0.0f;									// テクスチャの左上Y座標
		
		


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// プレイヤーのSlashを描画
//=============================================================================
void DrawPlayerSlash(void)
{
	if (!g_Player[0].slashCollider) return;

	BG* bg = GetBG();

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ANIM_SLASH]);

	//プレイヤーの位置やテクスチャー座標を反映
	float px = g_Player[0].pos.x - bg->pos.x;	// プレイヤーの表示位置X
	float py = g_Player[0].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
	float pw = SLASH_TEXTURE_WIDTH;				// プレイヤーの表示幅
	float ph = SLASH_TEXTURE_HEIGHT;			// プレイヤーの表示高さ

	float rotSlash = 0;

	//SLASHの向き
	switch (g_Player[0].slashDir)
	{
	case SLASH_DIR_RIGHT:
		px += g_Player[0].slashDistance;
		break;

	case SLASH_DIR_LEFT:
		px -= g_Player[0].slashDistance;
		rotSlash = XM_PI;
		break;

	case SLASH_DIR_DOWN:
		py += g_Player[0].slashDistance;
		rotSlash = XM_PIDIV2;
		break;

	case SLASH_DIR_UP:
		py -= g_Player[0].slashDistance;
		rotSlash = -XM_PIDIV2;
		break;
	}

	float tx = 0.0f;	// テクスチャの左上X座標
	float ty = 0.0f;	// テクスチャの左上Y座標
	float tw = 1.0f;	// テクスチャの幅
	float th = 1.0f;
	



	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, 
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		rotSlash);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
	
}


//=============================================================================
// プレイヤーのSlash HITを描画
//=============================================================================
void DrawSlashHit(void)
{

	if (g_SlashHitFramesCurrent <= g_SlashHitFrames)
	{
		if (g_SlashHitFramesCurrent != 0 && g_SlashHitFramesCurrent % 5 == 0)
		{
			g_SlashHitTexPatter++;
		}




		BG* bg = GetBG();

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ANIM_SLASHHIT]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_SlashHitPos.x - bg->pos.x;	// プレイヤーの表示位置X
		float py = g_SlashHitPos.y - bg->pos.y;	// プレイヤーの表示位置Y
		float pw = 75.0f;						// プレイヤーの表示幅
		float ph = 500.0f;						// プレイヤーの表示高さ

		float rotSlashHit = 0;


		float tw = 1.0f / 3.0f;					// テクスチャの幅
		float th = 1.0f;						// テクスチャの高さ
		float tx = tw * (g_SlashHitTexPatter);	// テクスチャの左上X座標
		float ty = 0.0f;						// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			g_SlashHitRot);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		g_SlashHitFramesCurrent++;
	}
}


//=============================================================================
// 回復エフェクトを描画
//=============================================================================
void DrawFocusEffect(void)
{
	if (g_Player[0].focus == FALSE) return;

	BG* bg = GetBG();

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ANIM_FOCUS_EFFECT]);

	//プレイヤーの位置やテクスチャー座標を反映
	float px = g_Player[0].pos.x - bg->pos.x;		// プレイヤーの表示位置X
	float py = g_Player[0].pos.y - bg->pos.y;		// プレイヤーの表示位置Y
	float pw = 100.0f;								// プレイヤーの表示幅
	float ph = 120.0f;								// プレイヤーの表示高さ


	float tw = 1.0f / 15.0f ;						// テクスチャの幅
	float th = 1.0f;								// テクスチャの高さ
	float tx = tw * g_Player[0].focusEffectSprite;	// テクスチャの左上X座標
	float ty = 0.0f;								// テクスチャの左上Y座標


	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

}



void DrawExclamation(void)
{
	if (g_Exclamation == TRUE)
	{
		BG* bg = GetBG();

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ANIM_EXCLAMATION]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_Player[0].pos.x - bg->pos.x;			// プレイヤーの表示位置X
		float py = g_Player[0].pos.y - bg->pos.y - 100.0f;	// プレイヤーの表示位置Y
		float pw = 70.0f;									// プレイヤーの表示幅
		float ph = 70.0f;									// プレイヤーの表示高さ


		float tw = 1.0f;									// テクスチャの幅
		float th = 1.0f;									// テクスチャの高さ
		float tx = 0.0f;									// テクスチャの左上X座標
		float ty = 0.0f;									// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
}



//=============================================================================
// プレイヤーが傷付いた
// 引数：傷付いた方向
//=============================================================================
void PlayerDamaged(XMFLOAT3 enemyPos)
{
	g_Player[0].damaged			= TRUE;
	g_Player[0].damageOriginPos = enemyPos;
	g_Player[0].playerColor		= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	g_Player[0].lifes--;
	UpdateLifesUI();
	ResultHit();

	//死んだらの設定
	if (g_Player[0].lifes <= 0)
	{
		g_Player[0].alive			= FALSE;
		g_Player[0].slash			= FALSE;
		g_Player[0].slashCollider	= FALSE;
		g_Player[0].patternAnim		= 0;

		SetSlowMotion();

		if (GetMode() != MODE_COLISEUM)
		{
			SetFade(FADE_OUT, GetMode());
		}
	}
	else
	{
		PlayerInvincible(INVINCIBLE_TIME);
	}

}


//=============================================================================
// 傷付く処理
//=============================================================================
void DamageUpdate(void)
{
	if (g_Player[0].damaged == TRUE)
	{
		//アニメーション用
		if (g_Player[0].alive)
		{
			g_P_Texture_No		= ANIM_HIT;
			g_Texture_Divide_X	= 1;
		}
		else
		{
			g_P_Texture_No		= ANIM_DEATH;
			g_Texture_Divide_X	= 4;
		}
		

		//ノックバック処理
		XMVECTOR Ppos = XMLoadFloat3(&g_Player[0].pos);
		XMVECTOR vec = (Ppos - XMLoadFloat3(&g_Player[0].damageOriginPos));		//自分とプレイヤーとの差分を求めて
		float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);					//その差分を使って角度を求めている

		if (fabsf(angle) < XM_PIDIV2)	//ダメージが左からきた場合	（左→π, 右→0、上→負数、下→正数）
		{
			angle = -XM_PIDIV4;		//左上の角度
		}
		else
		{
			angle = -XM_PIDIV4 * 3;	//右上の角度
		}

		float repealDisstance		  = 25.0f;										//スピードはちょっと遅くしてみる
		float repealDisstancePerFrame = repealDisstance / g_Player[0].dmgTime;

		float repealDistNow	= repealDisstancePerFrame * (g_Player[0].dmgTime - g_Player[0].dmgTimeCnt);


		g_Player[0].pos.x += cosf(angle) * repealDistNow;					//angleの方向へ移動
		g_Player[0].pos.y += sinf(angle) * repealDistNow;					//angleの方向へ移動

		g_Player[0].dmgTimeCnt++;

		if (g_Player[0].dmgTimeCnt >= g_Player[0].dmgTime)
		{
			g_Player[0].damaged		= FALSE;
			g_Player[0].dmgTimeCnt	= 0;
			g_Player[0].playerColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}


//=============================================================================
// プレイヤーの無敵状態
// 引数：無敵時間/フレーム（60 → 1秒）
//=============================================================================
void PlayerInvincible(int invincibleTime)
{
	g_Player[0].invincible		= TRUE;
	g_Player[0].invincibleTime	= invincibleTime;
}

//=============================================================================
// プレイヤーの無敵処理
//=============================================================================
void InvincibleUpdate(void)
{
	if (g_Player[0].invincible == TRUE)
	{
		if (g_Player[0].damaged == FALSE)
		{
			g_Player[0].playerColor.w = 0.7f;
		}

		g_Player[0].invincibleTimeCnt++;

		if (g_Player[0].invincibleTimeCnt >= g_Player[0].invincibleTime)
		{
			g_Player[0].invincible		  = FALSE;
			g_Player[0].invincibleTimeCnt = 0;
			g_Player[0].invincibleTime    = 0;
			g_Player[0].playerColor.w	  = 1.0f;
		}
	}
}


//=============================================================================
// PLATFORMとの当たり判定
//=============================================================================
void CollisionPlatforms(void)
{
	BOOL ans;

	//チュートリアルにだけ壁の当たり判定を行う
	switch (GetMode())
	{
	case MODE_TUTORIAL_1:
		{
			PLATFORM* HouseGround = GetGround();
			ans = CollisionBB(HouseGround[0].pos,   HouseGround[0].w ,  HouseGround[0].h,
							  g_Player[0].pos,		g_Player[0].w,		g_Player[0].h);

			if (ans == TRUE)
			{
				g_Player[0].pos.y	 = HouseGround[0].pos.y - (HouseGround[0].h / 2) - (g_Player[0].h / 2);
				g_Player[0].onGround = TRUE;
				g_Player[0].onAirCnt = 0;
			}

			BREAKABLE_WALL BreakableWall = GetBreakableWall();
			if (BreakableWall.HP > 0)
			{
				//プレイヤーが壁を超えられない為のあたり判定
				ans = CollisionBB(BreakableWall.pos,	BreakableWall.w,	BreakableWall.h,
								  g_Player[0].pos,		g_Player[0].w,		g_Player[0].h);

				if (ans == TRUE)
				{
					g_Player[0].pos.x = BreakableWall.pos.x - (BreakableWall.w / 2) - (g_Player[0].w / 2);
				}

				//プレイヤーの上にビックリマークを表す為のあたり判定
				ans = CollisionBB(BreakableWall.pos,	BreakableWall.w * 3,	BreakableWall.h,
								  g_Player[0].pos,		g_Player[0].w,			g_Player[0].h);

				if (ans == TRUE && g_Exclamation == FALSE)
				{
					g_Exclamation = TRUE;
					PlaySound(SOUND_LABEL_SE_Exclamation);
				}
				else if (ans == FALSE && g_Exclamation == TRUE)
				{
					g_Exclamation = FALSE;
				}
			}
			
		}
		break;

	default:

		{
			PLATFORM* ground = GetGround();

			// groundSの数分当たり判定を行う
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;

				ans = CheckCollisionAndGetDirection(ground[j].pos.x,	ground[j].pos.y,	(ground[j].w),		(ground[j].h),
													g_Player[0].pos.x,  g_Player[0].pos.y,	(g_Player[0].w),	(g_Player[0].h));
				
				// 当たっている？
				switch (ans)
				{
				case FromTop:
					g_Player[0].pos.y	 = ground[j].pos.y - (ground[j].h / 2) - (g_Player[0].h / 2);
					g_Player[0].onGround = TRUE;
					g_Player[0].onAirCnt = 0;
					break;

				case FromBottom:
					g_Player[0].pos.y = ground[j].pos.y + (ground[j].h / 2) + (g_Player[0].h / 2);
					break;

				case FromLeft:
					g_Player[0].pos.x = ground[j].pos.x - (ground[j].w / 2) - (g_Player[0].w / 2);
					break;

				case FromRight:
					g_Player[0].pos.x = ground[j].pos.x + (ground[j].w / 2) + (g_Player[0].w / 2);
					break;

				default:
					if (g_Player[0].onAirCnt > 10)
					{
						g_Player[0].onGround = FALSE;
					}
					else
					{
						g_Player[0].onAirCnt++;
					}
					break;
				}
			}
		}

		{
			PLATFORM_ANIM* platform = GetPlatforms();

			// PLATFORMSの数分当たり判定を行う
			for (int j = 0; j < PLATFORM_MAX; j++)
			{
				if (platform[j].collider == FALSE) continue;

				float platformH = 50.0f;

				ans = CheckCollisionAndGetDirection(platform[j].pos.x, platform[j].pos.y, (platform[j].w), (platformH),
					g_Player[0].pos.x, g_Player[0].pos.y, (g_Player[0].w), (g_Player[0].h));
				// 当たっている？

				switch (ans)
				{
				case FromTop:
					g_Player[0].pos.y = platform[j].pos.y - (platformH / 2) - (g_Player[0].h / 2);
					g_Player[0].onGround = TRUE;
					g_Player[0].onAirCnt = 0;
					break;

				case FromBottom:
					g_Player[0].pos.y = platform[j].pos.y + (platformH / 2) + (g_Player[0].h / 2);
					break;

				case FromLeft:
					g_Player[0].pos.x = platform[j].pos.x - (platform[j].w / 2) - (g_Player[0].w / 2);
					break;

				case FromRight:
					g_Player[0].pos.x = platform[j].pos.x + (platform[j].w / 2) + (g_Player[0].w / 2);
					break;

				default:
					if (g_Player[0].onAirCnt > 10)
					{
						g_Player[0].onGround = FALSE;
					}
					else
					{
						g_Player[0].onAirCnt++;
					}
					break;
				}
			}
		}

		{
			PLATFORM* Spikes = GetSpikes();
			for (int i = 0; i < SPIKES_MAX; i++)
			{
				ans = CollisionBB(Spikes[i].pos, Spikes[i].w, Spikes[i].h,
					g_Player[0].pos, g_Player[0].w, g_Player[0].h);

				if (ans == TRUE && g_Player[0].lifes >= 1)
				{
					g_Player[0].lifes = 1;
					PlayerDamaged(Spikes[i].pos);
				}
			}

			
			
		}
		
		break;
	}

	
}


//=============================================================================
// SLASHとエネミーの当たり判定
//=============================================================================
void CollisionSlash(void)
{
	if (g_Player[0].slashCollider == TRUE)
	{
		//SLASHの位置を計算する
		float slashW = SLASH_TEXTURE_WIDTH;
		float slashH = SLASH_TEXTURE_HEIGHT;
		XMFLOAT3 slashPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		slashPos.x = g_Player[0].pos.x;
		slashPos.y = g_Player[0].pos.y;

		switch (g_Player[0].slashDir)
		{
		case SLASH_DIR_RIGHT:
			slashPos.x += g_Player[0].slashDistance;
			break;

		case SLASH_DIR_LEFT:
			slashPos.x -= g_Player[0].slashDistance;
			break;

		case SLASH_DIR_DOWN:
			slashPos.y += g_Player[0].slashDistance;
			slashW = SLASH_TEXTURE_HEIGHT;	//90度回転しているのでｗとｈを変える
			slashH = SLASH_TEXTURE_WIDTH;
			break;

		case SLASH_DIR_UP:
			slashPos.y -= g_Player[0].slashDistance;
			slashW = SLASH_TEXTURE_HEIGHT;	//90度回転しているのでｗとｈを変える
			slashH = SLASH_TEXTURE_WIDTH;
			break;
		}


		//壁の当たり判定
		if(GetMode() == MODE_TUTORIAL_1)
		{
			BREAKABLE_WALL BreakableWall = GetBreakableWall();

			if (BreakableWall.HP > 0)
			{
				BOOL ans = CollisionBB(slashPos, slashW, slashH,
					BreakableWall.pos, BreakableWall.w, BreakableWall.h);
				// 当たっている？
				if (ans == TRUE)
				{
					//IteractiveObjHit[j].HP--;
					BreakableWallHit();
					g_SlashHitPos = slashPos;
					g_SlashHitRot = 0.0f;
					g_SlashHitFramesCurrent = 0;
					g_SlashHitTexPatter = 0;
						
				}
			}
				
		}
		

		{
			BOSS Boss = GetBoss();


			// 生きてるエネミーと当たり判定をする
			if (Boss.alive == TRUE && (Boss.damaged == FALSE))	//敵は死んでいない＆まだ傷ついていない状態
			{
				BOOL ans = CollisionBB(slashPos, slashW, slashH,
					Boss.pos, Boss.w, Boss.h);
				// 当たっている？
				if (ans == TRUE)
				{
					// 当たった時の処理

					BossDamaged(g_Player[0].slashDmg);

					EnemyHitted(slashPos, Boss.pos);
				}
			}
		}



		{
			SLIME_KING slimeKing = GetSlimeKing();


			// 生きてるエネミーと当たり判定をする
			if (slimeKing.alive == TRUE && (slimeKing.damaged == FALSE))	//敵は死んでいない＆まだ傷ついていない状態
			{
				BOOL ans = CollisionBB(slashPos, slashW, slashH,
					slimeKing.pos, slimeKing.w, slimeKing.h);
				// 当たっている？
				if (ans == TRUE)
				{
					// 当たった時の処理

					SlimeKingDamaged(g_Player[0].slashDmg);

					EnemyHitted(slashPos, slimeKing.pos);

				}
			}
			
		}


		{
			ENEMY_FLY_ONE* enemyFlyOne = GetEnemyFlyOne();

			// エネミーの数分当たり判定を行う
			for (int j = 0; j < ENEMY_FLY_ONE_MAX; j++)
			{
				// 生きてるエネミーと当たり判定をする
				if (enemyFlyOne[j].alive == TRUE && (enemyFlyOne[j].state != FLY_ONE_STATE_HIT))	//敵は死んでいない＆まだ傷ついていない状態
				{
					BOOL ans = CollisionBB(slashPos, slashW, slashH,
						enemyFlyOne[j].pos, enemyFlyOne[j].w, enemyFlyOne[j].h);
					// 当たっている？
					if (ans == TRUE)
					{
						// 当たった時の処理

						EnemyFlyOneDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);

						EnemyHitted(slashPos, enemyFlyOne[j].pos);

					}
				}
			}
		}


		{
			ENEMY_FLY_BOUNCE* enemyFlyBounce = GetEnemyFlyBounce();

			// エネミーの数分当たり判定を行う
			for (int j = 0; j < ENEMY_FLY_BOUNCE_MAX; j++)
			{
				// 生きてるエネミーと当たり判定をする
				if (enemyFlyBounce[j].alive == TRUE && enemyFlyBounce[j].damaged == FALSE)	//敵は死んでいない＆まだ傷ついていない状態
				{
					BOOL ans = CollisionBB(slashPos, slashW, slashH,
						enemyFlyBounce[j].pos, enemyFlyBounce[j].w, enemyFlyBounce[j].h);
					// 当たっている？
					if (ans == TRUE)
					{
						// 当たった時の処理

						EnemyFlyBounceDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);

						EnemyHitted(slashPos, enemyFlyBounce[j].pos);

					}
				}
			}
		}



		{
			ENEMY_KNIGHT* enemyKnight = GetEnemyKnight();

			// エネミーの数分当たり判定を行う
			for (int j = 0; j < ENEMY_KNIGHT_MAX; j++)
			{
				// 生きてるエネミーと当たり判定をする
				if ((enemyKnight[j].alive == TRUE) && (enemyKnight[j].state != KNIGHT_STATE_HIT))	//敵は死んでいない＆まだ傷ついていない状態
				{
					if (g_Player[0].slashCnt == 0)	//SLASHの最初のフレームだけ有効
					{
						BOOL ans = CheckCollisionAndGetDirection(enemyKnight[j].pos.x, enemyKnight[j].pos.y, KNIGHT_COLLIDER_WIDTH, enemyKnight[j].h,
																slashPos.x, slashPos.y, slashW, slashH);
						// 当たっている？

						if (ans > 0)
						{
							g_Player[0].slashCnt = g_Player[0].slashCnt;
						}

						switch (ans)
						{
							case FromTop:
								if (enemyKnight[j].state == KNIGHT_STATE_DEFENCE_UP)
								{
									PlaySound(SOUND_LABEL_SE_shieldGuard); //Defence sound
									//enemyKnight[j].state = KNIGHT_STATE_ATTACK;		//(JP)Attack stateに変わる　(EN)Change to Attack state
									if (g_Player[0].slashDir == SLASH_DIR_DOWN)
									{
										
										g_Player[0].jumpCnt = 0;
										g_Player[0].jump = TRUE;
									}
								}
								else
								{
									EnemyKnightDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);
									EnemyHitted(slashPos, enemyKnight[j].pos);
								}
								
								break;

							case FromBottom:
								EnemyKnightDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);
								EnemyHitted(slashPos, enemyKnight[j].pos);
								break;

							case FromLeft:

								if (enemyKnight[j].state == KNIGHT_STATE_DEFENCE_FRONT ||
									(enemyKnight[j].state == KNIGHT_STATE_ATTACK && enemyKnight[j].currentSprite < 3))
								{
									PlaySound(SOUND_LABEL_SE_shieldGuard); //Defence sound
									if (enemyKnight[j].state != KNIGHT_STATE_ATTACK)
									{
										enemyKnight[j].state = KNIGHT_STATE_ATTACK;		//(JP)Attack stateに変わる　(EN)Change to Attack state
										enemyKnight[j].stateTime = 60;
										enemyKnight[j].stateTimeCnt = 0;

										enemyKnight[j].countAnim = 0;
										enemyKnight[j].currentSprite = 0;
									}
								}
								else
								{
									EnemyKnightDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);
									EnemyHitted(slashPos, enemyKnight[j].pos);
								}

								break;

						case FromRight:
							if (enemyKnight[j].state == KNIGHT_STATE_DEFENCE_FRONT ||
								(enemyKnight[j].state == KNIGHT_STATE_ATTACK && enemyKnight[j].currentSprite < 3))
							{
								PlaySound(SOUND_LABEL_SE_shieldGuard); //Defence sound
								if (enemyKnight[j].state != KNIGHT_STATE_ATTACK)
								{
									enemyKnight[j].state = KNIGHT_STATE_ATTACK;		//(JP)Attack stateに変わる　(EN)Change to Attack state
									enemyKnight[j].stateTime = 60;
									enemyKnight[j].stateTimeCnt = 0;

									enemyKnight[j].countAnim = 0;
									enemyKnight[j].currentSprite = 0;
								}
							}
							else
							{
								EnemyKnightDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);
								EnemyHitted(slashPos, enemyKnight[j].pos);
							}
							break;
						}
					}
				}
			}
		}



		{
			ENEMY_BALL* enemyBall = GetEnemyBall();

			// エネミーの数分当たり判定を行う
			for (int j = 0; j < ENEMY_BALL_MAX; j++)
			{
				// 生きてるエネミーと当たり判定をする
				if (enemyBall[j].alive == TRUE && enemyBall[j].damaged == FALSE)	//敵は死んでいない＆まだ傷ついていない状態
				{
						

					BOOL ans = CollisionBB(slashPos, slashW, slashH,
						enemyBall[j].pos, enemyBall[j].w, enemyBall[j].h);
					// 当たっている？
					if (ans == TRUE)
					{
						// 当たった時の処理
							
						EnemyBallDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);
						enemyBall[j].state = BALL_STATE_SEARCH;
						enemyBall[j].stateTime = 30 + (rand() % 200);
						enemyBall[j].stateTimeCnt = 0;

						EnemyHitted(slashPos, enemyBall[j].pos);

					}
				}
			}
		}
			
		{
			ENEMY_PATROL* enemyPatrol = GetEnemyPatrol();

			// エネミーの数分当たり判定を行う
			for (int j = 0; j < ENEMY_PATROL_MAX; j++)
			{
				// 生きてるエネミーと当たり判定をする
				if (enemyPatrol[j].alive == TRUE && enemyPatrol[j].damaged == FALSE)	//敵は死んでいない＆まだ傷ついていない状態
				{


					BOOL ans = CollisionBB(slashPos, slashW, slashH,
						enemyPatrol[j].pos, enemyPatrol[j].w, enemyPatrol[j].h);
					// 当たっている？
					if (ans == TRUE)
					{
						// 当たった時の処理

						EnemyDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);

						EnemyHitted(slashPos, enemyPatrol[j].pos);

					}
				}
			}
		}
		
	}
}


//=============================================================================
// ジャンプ処理
//=============================================================================
void JumpUpdate(void)
{
	if (g_Player[0].jump == TRUE)
	{
		if (g_Player[0].damaged == TRUE)
		{
			g_Player[0].jump = FALSE;
			return;
		}

		g_Player[0].pos.y -= GRAVITY_FORCE_PER_FRAME;	//重力を無効にする

		float angle = (XM_PI / PLAYER_JUMP_CNT_MAX) * g_Player[0].jumpCnt;		//PIをジャンプ段階に割る

		//ジャンプの最初のジャンプ
		if (g_Player[0].jumpCnt <= PLAYER_JUMP_CNT_MAX * 0.5f)
		{
			g_P_Texture_No		= ANIM_JUMP;
			g_Texture_Divide_X  = 1;
			float y = GRAVITY_FORCE_PER_FRAME * (sinf(XM_PI + angle) + 1);
			g_Player[0].pos.y -= y;
		}
		else	//落ち始める
		{
			float y = GRAVITY_FORCE_PER_FRAME * (sinf(XM_PI + angle) + 1);
			g_Player[0].pos.y += y;
			g_P_Texture_No = ANIM_FALL;
			g_Texture_Divide_X = 1;
		}

		//ジャンプの延長処理
		if ((GetKeyboardPress(DIK_Z) || IsButtonPressed(0, BUTTON_A)) && g_Player[0].extendJump == TRUE)
		{
			g_Player[0].extendJumpCnt++;
			if (g_Player[0].extendJumpCnt >= g_Player[0].extendJumpMax)
			{
				g_Player[0].extendJump	  = FALSE;
				g_Player[0].extendJumpCnt = 0;
			}
		}
		else
		{
			g_Player[0].onGround		= FALSE;
			g_Player[0].extendJump		= FALSE;
			g_Player[0].extendJumpCnt	= 0;
			g_Player[0].jumpCnt++;

		}

		//ジャンプの終わり
		if (g_Player[0].jumpCnt > PLAYER_JUMP_CNT_MAX)
		{			 
			g_Player[0].jump = FALSE;
			g_Player[0].jumpCnt = 0;
		}

	}
	// ジャンプボタン押した？
	else if ((g_Player[0].jump == FALSE) && (GetKeyboardPress(DIK_Z) || IsButtonPressed(0, BUTTON_A)) && g_Player[0].onGround == TRUE)
	{
		g_Player[0].jump = TRUE;
		g_Player[0].extendJump = TRUE;
		g_Player[0].onGround = FALSE;
		g_Player[0].jumpCnt = 0;
		
	}
}


//=============================================================================
// 集中処理
//=============================================================================
void FocusUpdate(void)
{
	if (g_Player[0].energy < HEAL_ENERGY_COST || g_Player[0].lifes >= g_Player[0].lifesMax) return;


	if (g_Player[0].focus == TRUE)
	{
		if (g_Player[0].onGround == FALSE || g_Player[0].moving == TRUE) 
		{
			g_Player[0].focus = FALSE;
			g_Player[0].focusTimeCnt = 0;
			g_Player[0].focusEffectSprite = 0;
		}

		//アニメーション用
		g_P_Texture_No = ANIM_FOCUS;
		g_Texture_Divide_X = 1;

		//ボタンの長押し
		if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, BUTTON_B))
		{
			if (g_Player[0].focusTimeCnt != 0 && g_Player[0].focusTimeCnt % 4 == 0)
			{
				g_Player[0].focusEffectSprite++;
			}

			if (g_Player[0].focusTimeCnt >= g_Player[0].focusTimeMax)
			{
				g_Player[0].focus = FALSE;
				g_Player[0].focusTimeCnt = 0;
				g_Player[0].focusEffectSprite = 0;

				g_Player[0].lifes++;
				UpdateLifesUI();

				g_Player[0].energy -= HEAL_ENERGY_COST;
				UpdateEnergyUI();
			}

			g_Player[0].focusTimeCnt++;
		}
		else //ボタンを離した
		{
			g_Player[0].focus = FALSE;
			g_Player[0].focusTimeCnt = 0;
			g_Player[0].focusEffectSprite = 0;
		}
	}
	// ジャンプボタン押した？
	else if ((g_Player[0].focus == FALSE) && (GetKeyboardPress(DIK_A) || IsButtonPressed(0, BUTTON_B)))
	{
		if (g_Player[0].onGround == TRUE && g_Player[0].moving == FALSE)
		{
			g_Player[0].focus = TRUE;
			g_Player[0].focusTimeCnt = 0;
			g_Player[0].focusEffectSprite = 0;
		}
	}
}


//=============================================================================
// SLASH処理
//=============================================================================
void SlashUpdate(void)
{
	if (g_Player[0].slash == TRUE)
	{
		g_Player[0].slashCnt++;


		g_P_Texture_No = ANIM_ATTACK;
		g_Texture_Divide_X = 1;
		
		if (g_Player[0].slashCnt >= g_Player[0].slashColliderFrames)
		{
			g_Player[0].slashCollider = FALSE;

			if (g_Player[0].slashCnt >= g_Player[0].slashFrames)
			{
				g_Player[0].slash = FALSE;
			}
		}
	}
	else if (g_Player[0].slash == FALSE)
	{
		if (GetKeyboardTrigger(DIK_X) || IsButtonTriggered(0, BUTTON_X))
		{
			g_Player[0].slashDistance = PLAYER_SLASH_DISTANCE;

			g_Player[0].slashDir = g_Player[0].dir;
			g_Player[0].slashCnt = 0;
			g_Player[0].slash = TRUE;
			g_Player[0].slashCollider = TRUE;

			PlaySound(SOUND_LABEL_SE_slash);
		}

	}
}


//=============================================================================
// ダッシュ処理
//=============================================================================
void DashUpdate(void)
{
	// ダッシュ処理中？
	if (g_Player[0].dash == TRUE)
	{
		float distancePerFrame = g_Player[0].dashDistance / g_Player[0].dashFrames;
		g_Player[0].pos.x += distancePerFrame;

		g_Player[0].dashCnt++;

		g_P_Texture_No = ANIM_DASH;	//アニメーション用
		g_Texture_Divide_X = 1;		//アニメーション用

		if (g_Player[0].dashCnt >= g_Player[0].dashFrames)
		{
			g_Player[0].dash = FALSE;
		}
	}
	// ダッシュボタン押した？
	else if ((g_Player[0].dash == FALSE) && (GetKeyboardTrigger(DIK_C) || IsButtonTriggered(0, BUTTON_R2)) && g_Player[0].moving == TRUE)
	{
		PlaySound(SOUND_LABEL_SE_Dash);

		if (g_Player[0].dashReload == TRUE)
		{
			g_Player[0].dashDistance = PLAYER_DASH_DISTANCE;

			if (g_Player[0].dir == CHAR_DIR_LEFT)
			{
				g_Player[0].dashDistance = -g_Player[0].dashDistance;
			}
			g_Player[0].dashCnt = 0;
			g_Player[0].dash = TRUE;
			g_Player[0].dashReload = FALSE;
		}
	}
	else if (g_Player[0].dashReload == FALSE)
	{
		if (g_Player[0].onGround) g_Player[0].dashReload = TRUE;
	}
}


//=============================================================================
// アニメーション 処理
//=============================================================================
void AnimationUpdate(void) 
{
	if (g_Player[0].alive == TRUE)
	{
		if (g_Player[0].moving == TRUE && g_Player[0].onGround == TRUE)
		{
			g_P_Texture_No		= ANIM_RUN;
			g_Texture_Divide_X	= 8;
		}
		else if (g_Player[0].onGround == FALSE)
		{

		}
		else
		{
			g_P_Texture_No		= ANIM_IDLE;
			g_Texture_Divide_X	= 5;
		}

		g_Player[0].countAnim += 1.0f;
		if (g_Player[0].countAnim > ANIM_WAIT)
		{
			g_Player[0].countAnim	= 0.0f;
			g_Player[0].patternAnim = (g_Player[0].patternAnim + 1) % g_Texture_Divide_X;	// パターンの切り替え
		}
	}
	else 
	{
		g_P_Texture_No		= ANIM_DEATH;
		g_Texture_Divide_X  = 4;
		g_Player[0].w		= g_Player[0].h;

		
		if (g_Player[0].countAnim > ANIM_WAIT)
		{
			g_Player[0].countAnim = 0.0f;
			// パターンの切り替え
			g_Player[0].patternAnim = (g_Player[0].patternAnim + 1);
		}
		if (g_Player[0].patternAnim < 2)
		{
			g_Player[0].countAnim += 1.0f;
		}
		else if (g_Player[0].onGround && g_Player[0].patternAnim == 2)
		{
			g_Player[0].patternAnim ++;
		}
	}
}



//=============================================================================
// エネミーに攻撃が当たる 処理
//=============================================================================
void EnemyHitted(XMFLOAT3 slashPos, XMFLOAT3 enemyPos)
{
	if (g_Player[0].energy < g_Player[0].energyMax)
	{
		g_Player[0].energy++;
		UpdateEnergyUI();
	}

	//Slash Hit 描画用
	g_SlashHitPos			= slashPos;
	g_SlashHitFramesCurrent = 0;
	g_SlashHitTexPatter		= 0;

	XMFLOAT3 enemyPosition  = enemyPos;

	XMVECTOR Ppos = XMLoadFloat3(&g_Player[0].pos);
	XMVECTOR vec  = (Ppos - XMLoadFloat3(&enemyPosition));			//自分とプレイヤーとの差分を求めて
	float angle   = atan2f(vec.m128_f32[1], vec.m128_f32[0]);		//その差分を使って角度を求めている
	g_SlashHitRot = angle;

	if (g_Player[0].slashDir == SLASH_DIR_DOWN)
	{
		g_Player[0].jumpCnt = 0;
		g_Player[0].jump	= TRUE;
	}
}

//=============================================================================
// エネミーに攻撃が当たる 処理
//=============================================================================
float Lerp(float start, float end, float t) 
{
	return start + t * (end - start);
}


//=============================================================================
// プレイヤーのHPとエネルギーを設定する
//=============================================================================
void SetPlayerStats(int hp, int energy)
{
	g_Player[0].lifes  = hp;
	g_Player[0].energy = energy;
	UpdateLifesUI();
	UpdateEnergyUI();
}
