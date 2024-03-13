//=============================================================================
//
// BackGround処理 [bg.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "bg.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH * 5)	// 背景幅 　(TUTORIAL_2)
#define TEXTURE_HEIGHT				(1600)				// 背景高さ (TUTORIAL_2)
#define TEXTURE_MAX					(7)					// テクスチャの数

#define BG_SCROLL_SPEED				(0.1f)				//Backgroundの移動量


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Stage/BG/Background.png",
	"data/TEXTURE/Stage/BG/background_layer_1_Large.png",
	"data/TEXTURE/Stage/BG/background_layer_2_Large.png",
	"data/TEXTURE/Stage/BG/background_layer_3_Large.png",

	"data/TEXTURE/Stage/Assets/Exit_anim.png",

	"data/TEXTURE/Stage/BG/ArenaBGSky.png",
	"data/TEXTURE/Stage/BG/ArenaBG.png",
};


static BG	g_BG;							//シーンの大きさ
static BG	g_BGmove[BACKGROUNDS_MAX];		//移動するBackGround

static BOOL	g_Load = FALSE;					// 初期化を行ったかのフラグ


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBG(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
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
	switch (GetMode())
	{
	case MODE_TUTORIAL_1:
		
		//Background
		g_BG.w		= SCREEN_WIDTH;
		g_BG.h		= SCREEN_HEIGHT;
		g_BG.pos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_BG.texNo	= 0;

		g_BG.scrl	= 0.0f;
		break;

	case MODE_TUTORIAL_2:
		
		//シーンの大きさ
		g_BG.w		= TEXTURE_WIDTH;
		g_BG.h		= TEXTURE_HEIGHT;
		g_BG.pos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_BG.texNo	= 0;
		g_BG.scrl	= 0.0f;
		
		//表示するBackGround
		for (int i = 0; i < BACKGROUNDS_MAX; i++)
		{
			g_BGmove[i].w		= TEXTURE_WIDTH;
			g_BGmove[i].h		= TEXTURE_HEIGHT;
			g_BGmove[i].pos		= XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_BGmove[i].texNo	= 1 + i;
			g_BGmove[i].scrl	= BG_SCROLL_SPEED + (BG_SCROLL_SPEED * i);
		}
		break;

	case MODE_COLISEUM:

		//シーンの大きさ
		g_BG.w		= SCREEN_WIDTH + SCREEN_WIDTH / 2;
		g_BG.h		= SCREEN_HEIGHT;
		g_BG.pos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_BG.texNo	= 0;
		g_BG.scrl	= 0.0f;

		//シーンの大きさ
		for (int i = 0; i < BACKGROUNDS_MAX; i++)
		{
			g_BGmove[i].w		= g_BG.w;
			g_BGmove[i].h		= g_BG.h;
			g_BGmove[i].pos		= XMFLOAT3(0.0f, g_BG.h - g_BGmove[i].h, 0.0f);
			g_BGmove[i].texNo	= 4 + i;
			g_BGmove[i].scrl	= BG_SCROLL_SPEED + (BG_SCROLL_SPEED * i);
		}

		break;
	}
	


	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBG(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
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
void UpdateBG(void)
{


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBG(void)
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


	{//BG
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_BG.texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer,
			g_BG.pos.x, g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

		
	{// 背景を描画
		for (int i = 0; i < BACKGROUNDS_MAX; i++)
		{
			if (GetMode() == MODE_TUTORIAL_1) break;

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_BGmove[i].texNo]);

			float px = g_BGmove[i].pos.x - (g_BG.pos.x * g_BGmove[i].scrl);	// エネミーの表示位置X
			float py = g_BGmove[i].pos.y - g_BG.pos.y;						// エネミーの表示位置Y
			float pw = g_BGmove[i].w;										// エネミーの表示幅
			float ph = g_BGmove[i].h;										// エネミーの表示高さ

			//PLATFORMのテクスチャー座標を反映
			float tx = 0.0f;					// テクスチャの左上X座標
			float ty = 0.0f;					// テクスチャの左上Y座標
			float tw = 1.3f;					// テクスチャの幅
			float th = 1.0f;					// テクスチャの高さ

			if (GetMode() == MODE_TUTORIAL_2) tw = pw / SCREEN_WIDTH;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}		
	}
}


//=============================================================================
// シーンの大きさ構造体の先頭アドレスを取得
//=============================================================================
BG* GetBG(void)
{
	return &g_BG;
}


//=============================================================================
// BG構造体の先頭アドレスを取得
//=============================================================================
BG* GetBGmove(void)
{
	return &g_BGmove[0];
}
