//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "title.h"
#include "fightUI.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ

#define TEXTURE_MESSAGE_WIDTH		(500.0f)		// メッセージサイズ
#define TEXTURE_MESSAGE_HEIGHT		(29.0f)			// メッセージサイズ

#define TITLE_PARTICLE_MAX			(50)			//パーティクル数
#define TITLE_PARTICLE_SPAWN_TIME	(15)			//パーティクル寿命

#define MESSAGE_DIVIDE_X			(500)			// メッセージの横分解

#define ANGLE_CHANGE_AMOUNT  (TEXTURE_MESSAGE_WIDTH / 360.0f)	


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetTitlePartible(XMFLOAT3 pos);
void DrawTitleParticles(void);

void DrawMessage(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TITLE_TEX_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TITLE_TEX_MAX] =
{
	"data/TEXTURE/Menu/TitleImg.png",
	"data/TEXTURE/Menu/ArenaConquer.png",
	"data/TEXTURE/Menu/Particle3.png",
	"data/TEXTURE/Menu/TitleMessageKeyboard.png",
};


static BOOL			g_bg_Use;							// TRUE:使っている  FALSE:未使用
static float		g_bg_w, g_h;						// 幅と高さ
static XMFLOAT3		g_bg_Pos;							// ポリゴンの座標
static int			g_bg_TexNo;							// テクスチャ番号

TITLE_PARTICLE		g_TitleParticle[TITLE_PARTICLE_MAX];//パーティクル
int					g_ParticleSpawnTimeCnt;				//パーティクルカウント

BUTTON_MESSAGE		g_Message[MESSAGE_DIVIDE_X];		//「ゲームスタート」メッセージ
float				g_FrameAngleStartTate;				// メッセージの動きタイミング

BOOL				g_TitleInput;						// タイトルインプット

static BOOL			g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{


	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TITLE_TEX_MAX; i++)
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


	// 変数の初期化
	g_bg_Use = TRUE;
	g_bg_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_bg_Pos = XMFLOAT3(g_bg_w / 2, g_h / 2, 0.0f);
	g_bg_TexNo = 0;


	for (int i = 0; i < TITLE_PARTICLE_MAX; i++)
	{

		g_TitleParticle[i].use = FALSE;
		g_TitleParticle[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_TitleParticle[i].w = 10.0f;
		g_TitleParticle[i].h = 10.0f;
		g_TitleParticle[i].texNo = TITLE_TEX_PARTICLES;

		g_TitleParticle[i].moveSpeed = 2.0f;
		g_TitleParticle[i].moveDir = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_TitleParticle[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_TitleParticle[i].lifeTime = 0;
		g_TitleParticle[i].lifeTimeCnt = 0;
		g_TitleParticle[i].alphaAdd = 0.0f;
	}

	g_ParticleSpawnTimeCnt = 0;


	// プレイヤー構造体の初期化
	for (int i = 0; i < MESSAGE_DIVIDE_X; i++)
	{
		g_Message[i].use	= TRUE;
		g_Message[i].pos	= XMFLOAT3((SCREEN_CENTER_X-100.0f) / 1.5f + (1.0f * i), SCREEN_HEIGHT - 50.0f, 0.0f);	// 中心点から表示
		g_Message[i].rot	= XMFLOAT3(0.0f, 0.0f, 0.0f);				// 回転率
		g_Message[i].scl	= XMFLOAT3(1.0f, 1.0f, 1.0f);				// 拡大縮小
		g_Message[i].w		= 1.0f;
		g_Message[i].h		= TEXTURE_MESSAGE_HEIGHT;
		g_Message[i].texNo	= TITLE_TEX_BUTTON_MESSAGE;

		g_Message[i].countAnim   = 0;
		g_Message[i].patternAnim = 0;

	}

	g_FrameAngleStartTate = 0.0f;

	g_TitleInput = TRUE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TITLE_TEX_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{
	float posX = (float)(rand() % SCREEN_WIDTH);
	SetTitlePartible(XMFLOAT3(posX, SCREEN_HEIGHT, 0.0f));

	if (g_TitleInput)
	{
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B) || IsButtonTriggered(0, BUTTON_R))
		{// Enter押したら、ステージを切り替える
			SetFade(FADE_OUT, MODE_TUTORIAL);
			g_TitleInput = FALSE;
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
		}
	}


	for (int i = 0; i < MESSAGE_DIVIDE_X; i++)
	{
		// 生きてるプレイヤーだけ処理をする
		if (g_Message[i].use == TRUE)
		{
			float DrawAngleStart = g_FrameAngleStartTate + (ANGLE_CHANGE_AMOUNT * i);
			float currentRadians = XMConvertToRadians(DrawAngleStart);
			g_Message[i].pos.y = (SCREEN_HEIGHT - 50.0f) + sinf(currentRadians) * 5;
		}
	}

	g_FrameAngleStartTate += ANGLE_CHANGE_AMOUNT;

	if (g_FrameAngleStartTate >= 360.0f)
	{
		g_FrameAngleStartTate -= 360.0f;
	}



#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
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

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	DrawTitleParticles();

	DrawTitleParticles();

	DrawMessage();
}

//===========================
// パーティクル設定
//===========================
void SetTitlePartible(XMFLOAT3 pos)
{
	g_ParticleSpawnTimeCnt++;
	if (g_ParticleSpawnTimeCnt > TITLE_PARTICLE_SPAWN_TIME)
	{
		g_ParticleSpawnTimeCnt = 0;

		for (int i = 0; i < TITLE_PARTICLE_MAX; i++)
		{
			if (g_TitleParticle[i].use == FALSE)
			{
				g_TitleParticle[i].use = TRUE;
				g_TitleParticle[i].pos = pos;
				g_TitleParticle[i].moveSpeed = 0.0f;

				float angle = (float)(rand() % 78);
				angle /= 100;
				angle += XM_PIDIV2 - XM_PIDIV4 / 2;
				angle *= -1;
				g_TitleParticle[i].moveDir.x = cosf(angle);	//angleの方向へ移動
				g_TitleParticle[i].moveDir.y = sinf(angle);	//angleの方向へ移動

				g_TitleParticle[i].lifeTime = TITLE_PARTICLE_MAX;
				g_TitleParticle[i].lifeTimeCnt = 0;

				g_TitleParticle[i].alphaAdd = 0.007f;

				g_TitleParticle[i].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
				break;
			}
		}
	}


	for (int i = 0; i < TITLE_PARTICLE_MAX; i++)
	{
		if (g_TitleParticle[i].use == TRUE)
		{
			g_TitleParticle[i].lifeTimeCnt++;
			g_TitleParticle[i].moveSpeed += 0.02f;
			g_TitleParticle[i].pos.x += g_TitleParticle[i].moveDir.x * g_TitleParticle[i].moveSpeed;
			g_TitleParticle[i].pos.y += g_TitleParticle[i].moveDir.y * g_TitleParticle[i].moveSpeed;
			g_TitleParticle[i].color.x += 0.002f/*1.0f / TITLE_PARTICLE_MAX*/;

			if (g_TitleParticle[i].color.w <= 1.0f)
			{
				g_TitleParticle[i].color.w += g_TitleParticle[i].alphaAdd;
			}

			//画面端の当たり判定
			{
				if (g_TitleParticle[i].pos.x < (-g_TitleParticle[i].w / 2))		// 右
				{
					g_TitleParticle[i].use = FALSE;
				}
				if (g_TitleParticle[i].pos.x > (SCREEN_WIDTH + g_TitleParticle[i].w / 2))	// 左
				{
					g_TitleParticle[i].use = FALSE;
				}
				if (g_TitleParticle[i].pos.y < (-g_TitleParticle[i].h / 2))		// 上
				{
					g_TitleParticle[i].use = FALSE;
				}
			}
		}
	}
}

//===========================
// パーティクル描画処理
//===========================
void DrawTitleParticles(void)
{
	for (int i = 0; i < TITLE_PARTICLE_MAX; i++)
	{
		if (g_TitleParticle[i].use == TRUE)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TitleParticle[i].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_TitleParticle[i].pos.x;	// エネミーの表示位置X
			float py = g_TitleParticle[i].pos.y;	// エネミーの表示位置Y
			float pw = g_TitleParticle[i].w;					// エネミーの表示幅
			float ph = g_TitleParticle[i].h;					// エネミーの表示高さ

			float tw = 1.0f;		// テクスチャの幅
			float th = 1.0f;		// テクスチャの高さ
			float tx = 0.0f;		// テクスチャの左上X座標
			float ty = 0.0f;		// テクスチャの左上Y座標


			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_TitleParticle[i].color);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//===========================
// メッセージ描画処理
//===========================
void DrawMessage(void)
{
	for (int i = 0; i < MESSAGE_DIVIDE_X; i++)
	{
		if (g_Message[i].use == TRUE)		// このプレイヤーが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Message[i].texNo]);

			//ラスターの位置やテクスチャー座標を反映
			float px = g_Message[i].pos.x;	// プレイヤーの表示位置X
			float py = g_Message[i].pos.y;	// プレイヤーの表示位置Y
			float pw = g_Message[i].w;		// プレイヤーの表示幅
			float ph = g_Message[i].h;		// プレイヤーの表示高さ

			float ty = 0.0f;						// テクスチャの左上X座標
			float tx = (1.0f / TEXTURE_MESSAGE_WIDTH) * i;	// テクスチャの左上Y座標
			float th = 1.0f;						// テクスチャの幅
			float tw = (1.0f / TEXTURE_MESSAGE_WIDTH);		// テクスチャの高さ

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}