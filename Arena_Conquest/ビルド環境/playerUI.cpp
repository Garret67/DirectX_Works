//=============================================================================
//
// スコア処理 [score.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "playerUI.h"
#include "player.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//#define TEXTURE_MAX					(4)		// テクスチャの数
#define ENERGY_TEXTURE_DIVIDE_X		(9)		// アニメパターンのテクスチャ内分割数（X)
#define ENERGY_TEXTURE_1_DIVISION	(1.0f / ENERGY_TEXTURE_DIVIDE_X)		// アニメパターンのテクスチャ内分割数（X)

#define ROUND_MSG_MAX				(10)

#define ROUND_MSG_SHOW_FRAMES		(120)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void UpdateRoundMsg(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[UI_TEX_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[UI_TEX_MAX] = {
	"data/TEXTURE/Player/HealthUI.png",
	"data/TEXTURE/Player/EnergyUI.png",

	"data/TEXTURE/Effect/RoundMsg/RoundBG3.png",
	"data/TEXTURE/Effect/RoundMsg/Round1.png",
	"data/TEXTURE/Effect/RoundMsg/Round2.png",
	"data/TEXTURE/Effect/RoundMsg/Round3.png",
	"data/TEXTURE/Effect/RoundMsg/Round4.png",
	"data/TEXTURE/Effect/RoundMsg/Round5.png",
	"data/TEXTURE/Effect/RoundMsg/Round6.png",
	"data/TEXTURE/Effect/RoundMsg/Round7.png",
	"data/TEXTURE/Effect/RoundMsg/Round8.png",
	"data/TEXTURE/Effect/RoundMsg/Round9.png",
	"data/TEXTURE/Effect/RoundMsg/Round10.png",
};



static HEALTH_UI		g_HealthUI;
static ENERGY_UI		g_EnergyUI;

static ROUND_MESSAGE	g_RoundMsgBG;
static ROUND_MESSAGE	g_RoundMsg[ROUND_MSG_MAX];

static BOOL				g_ShowRoundMsg;
static int				g_RoundMsgShowFramesCnt;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayerUI(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < UI_TEX_MAX; i++)
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


	PLAYER* player = GetPlayer();

	// HEALTH_UIの初期化
	g_HealthUI.pos		= XMFLOAT3(150.0f, 40.0f, 0.0f);
	g_HealthUI.rot		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_HealthUI.scl		= XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_HealthUI.w		= 30.0f;
	g_HealthUI.h		= 30.0f;
	g_HealthUI.texNo	= UI_TEX_HEALTH;

	g_HealthUI.lifes	= player[0].lifes;
	g_HealthUI.lifesMax = player[0].lifesMax;
	g_HealthUI.active	= TRUE;

	// ENERGY_UIの初期化
	g_EnergyUI.pos		 = XMFLOAT3(70.0f, 70.0f, 0.0f);
	g_EnergyUI.rot		 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_EnergyUI.scl		 = XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_EnergyUI.w		 = 110.0f;
	g_EnergyUI.h		 = 110.0f;
	g_EnergyUI.texNo	 = UI_TEX_ENERGY;
	
	g_EnergyUI.energy	 = player[0].energy;
	g_EnergyUI.energyMax = player[0].energyMax;
	g_EnergyUI.active	 = TRUE;


	g_RoundMsgBG.pos = XMFLOAT3(SCREEN_WIDTH + g_RoundMsgBG.w / 2, SCREEN_CENTER_Y - (SCREEN_CENTER_Y / 2), 0.0f);
	g_RoundMsgBG.w = 600.0f / 2;
	g_RoundMsgBG.h = 150.0f / 2;
	g_RoundMsgBG.texNo = UI_TEX_ROUND_BG;

	float firstRoundMsgPosY = SCREEN_CENTER_Y - (SCREEN_CENTER_Y / 2) - 25.0f + 2.5f;
	for (int i = 0; i < ROUND_MSG_MAX; i++)
	{
		g_RoundMsg[i].w = 600.0f / 2;
		g_RoundMsg[i].h = 100.0f / ROUND_MSG_MAX / 2;
		g_RoundMsg[i].pos = XMFLOAT3(-g_RoundMsg[i].w / 2 - (300 * i), firstRoundMsgPosY + (g_RoundMsg[i].h * i), 0.0f);
		g_RoundMsg[i].texNo = UI_TEX_ROUND_1;
	}

	g_ShowRoundMsg = FALSE;
	
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayerUI(void)
{
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < UI_TEX_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayerUI(void)
{
	UpdateRoundMsg();

#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayerUI(void)
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


	//Draw HP
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_HealthUI.texNo]);

		// 桁数分処理する
		for (int i = 0; i < g_HealthUI.lifesMax; i++)
		{

			// HPの位置やテクスチャー座標を反映
			float px = g_HealthUI.pos.x + (40.0f * i);	// スコアの表示位置X
			float py = g_HealthUI.pos.y;			// スコアの表示位置Y
			float pw = g_HealthUI.w;				// スコアの表示幅
			float ph = g_HealthUI.h;				// スコアの表示高さ


			float tx = 0.0f;			// テクスチャの左上X座標
			float ty = 0.0f;			// テクスチャの左上Y座標
			float tw = 0.5f;		// テクスチャの幅
			float th = 1.0f;		// テクスチャの高さ

			if (i >= g_HealthUI.lifes)
			{
				tx = 0.5f;
			}

			
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}


	//Draw Energy
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnergyUI.texNo]);

		// 桁数分処理する

		// HPの位置やテクスチャー座標を反映
		float px = g_EnergyUI.pos.x;			// スコアの表示位置X
		float py = g_EnergyUI.pos.y;			// スコアの表示位置Y
		float pw = g_EnergyUI.w;				// スコアの表示幅
		float ph = g_EnergyUI.h;				// スコアの表示高さ

		float tx = ENERGY_TEXTURE_1_DIVISION * g_EnergyUI.energy;	// テクスチャの左上X座標
		float ty = 0.0f;											// テクスチャの左上Y座標
		float tw = ENERGY_TEXTURE_1_DIVISION;						// テクスチャの幅
		float th = 1.0f;											// テクスチャの高さ


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
		
	}

	if (!g_ShowRoundMsg)return;
	//ROUND BG の描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_RoundMsgBG.texNo]);

		//ROUND BGの位置やテクスチャー座標を反映
		float px = g_RoundMsgBG.pos.x;		// 表示位置X
		float py = g_RoundMsgBG.pos.y;		// 表示位置Y
		float pw = g_RoundMsgBG.w * 1.2f;	// 表示幅
		float ph = g_RoundMsgBG.h * 1.2f;	// 表示高さ

		float tw = 1.0f;				// テクスチャの幅
		float th = 1.0f;				// テクスチャの高さ
		float tx = 0.0f;				// テクスチャの左上X座標
		float ty = 0.0f;				// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//ROUND MSG の描画
	for (int i = 0; i < ROUND_MSG_MAX; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_RoundMsg[i].texNo]);

		//ROUND MSGの位置やテクスチャー座標を反映
		float px = g_RoundMsg[i].pos.x;	// 表示位置X
		float py = g_RoundMsg[i].pos.y;	// 表示位置Y
		float pw = g_RoundMsg[i].w;					// 表示幅
		float ph = g_RoundMsg[i].h;					// 表示高さ

		float tw = 1.0f;		// テクスチャの幅
		float th = 1.0f / ROUND_MSG_MAX;				// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = th * i;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	
}


//=============================================================================
// スコアを加算する
// 引数:add :追加する点数。マイナスも可能
//=============================================================================
void UpdateLifesUI(void)
{
	PLAYER* player = GetPlayer();
	g_HealthUI.lifes = player[0].lifes;
}

void UpdateEnergyUI(void)
{
	PLAYER* player = GetPlayer();
	g_EnergyUI.energy = player[0].energy;
}

void ShowRoundMsg(int round)
{
	g_ShowRoundMsg = TRUE;

	g_RoundMsgBG.pos = XMFLOAT3(SCREEN_WIDTH + g_RoundMsgBG.w / 2, SCREEN_CENTER_Y - (SCREEN_CENTER_Y / 2), 0.0f);

	float firstRoundMsgPosY = SCREEN_CENTER_Y - (SCREEN_CENTER_Y / 2) - 25.0f + 2.5f;

	for (int i = 0; i < ROUND_MSG_MAX; i++)
	{
		g_RoundMsg[i].pos = XMFLOAT3(-g_RoundMsg[i].w / 2 - (300 * i), firstRoundMsgPosY + (g_RoundMsg[i].h * i), 0.0f);
		g_RoundMsg[i].texNo = round + 2;
	}

}

void UpdateRoundMsg(void)
{
	if (!g_ShowRoundMsg)return;

	if (g_RoundMsgBG.pos.x > SCREEN_CENTER_X)
	{
		g_RoundMsgBG.pos.x -= 10.0f;
	}
	else if(g_RoundMsg[ROUND_MSG_MAX - 1].pos.x < SCREEN_CENTER_X)
	{
		for (int i = 0; i < ROUND_MSG_MAX; i++)
		{
			if (g_RoundMsg[i].pos.x < SCREEN_CENTER_X)
			{
				g_RoundMsg[i].pos.x += 50.0f;
			}
			
		}
	}
	else if (g_RoundMsgShowFramesCnt < ROUND_MSG_SHOW_FRAMES)
	{
		g_RoundMsgShowFramesCnt++;
	}
	else if (g_RoundMsgBG.pos.y > -g_RoundMsgBG.h)
	{
		g_RoundMsgBG.pos.y -= 5.0f;
		for (int i = 0; i < ROUND_MSG_MAX; i++)
		{
			g_RoundMsg[i].pos.y -= 5.0f;
		}
	}
	else 
	{
		g_RoundMsgShowFramesCnt = 0;
		g_ShowRoundMsg = FALSE;
	}
}


