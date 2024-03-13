//=============================================================================
//
// リザルト画面処理 [screenGameOver.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "screenGameOver.h"
#include "input.h"
#include "score.h"
#include "fade.h"
#include "sound.h"
#include "spawnController.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
//#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
//#define TEXTURE_MAX					(5)				// テクスチャの数

#define TEXTURE_BUTTON_WIDTH		(1000.0f / 3)	
#define TEXTURE_BUTTON_HEIGHT		(230.0f / 3)	

#define GAMEOVER_BUTTON_MAX			(3)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

void DrawScreenGameOverButtons(void);

void OnClickGOButton(int button);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[GAMEOVER_TEX_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[GAMEOVER_TEX_MAX] = {
	"data/TEXTURE/Menu/UIbgH.png",
	"data/TEXTURE/Menu/MenuButtons/GameOverTitle.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/GORetryRound.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/GORetryColiseum.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/GOBackToMenu.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/GORetryRoundMessage.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/GORetryColiseumMessage.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/GOBackToMenuMessage.png",
	"data/TEXTURE/Player/DeadGameOver.png",
};


static float			g_GameOverScreenAlpha;

static GAMEOVER_BUTTON	g_GameOverButton[GAMEOVER_BUTTON_MAX];

static int				g_SelectedButton;

BOOL					g_GameOverInput;

static BOOL				g_Load = FALSE;



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScreenGameOver(void)
{
	switch (GetLanguage())
	{
	case LANG_JPN:
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/Japanese/GORetryRound.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/Japanese/GORetryColiseum.png";
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/Japanese/GOBackToMenu.png";
		g_TexturName[5] = "data/TEXTURE/Menu/MenuButtons/Japanese/GORetryRoundMessage.png";
		g_TexturName[6] = "data/TEXTURE/Menu/MenuButtons/Japanese/GORetryColiseumMessage.png";
		g_TexturName[7] = "data/TEXTURE/Menu/MenuButtons/Japanese/GOBackToMenuMessage.png";
		break;

	case LANG_USA:
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/English/GORetryRound.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/English/GORetryColiseum.png";
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/English/GOBackToMenu.png";
		g_TexturName[5] = "data/TEXTURE/Menu/MenuButtons/English/GORetryRoundMessage.png";
		g_TexturName[6] = "data/TEXTURE/Menu/MenuButtons/English/GORetryColiseumMessage.png";
		g_TexturName[7] = "data/TEXTURE/Menu/MenuButtons/English/GOBackToMenuMessage.png";
		break;

	case LANG_ESP:
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/Spanish/GORetryRound.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/Spanish/GORetryColiseum.png";
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/Spanish/GOBackToMenu.png";
		g_TexturName[5] = "data/TEXTURE/Menu/MenuButtons/Spanish/GORetryRoundMessage.png";
		g_TexturName[6] = "data/TEXTURE/Menu/MenuButtons/Spanish/GORetryColiseumMessage.png";
		g_TexturName[7] = "data/TEXTURE/Menu/MenuButtons/Spanish/GOBackToMenuMessage.png";
		break;
	}


	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < GAMEOVER_TEX_MAX; i++)
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


	g_GameOverScreenAlpha = 0.0f;

	g_SelectedButton = 1;


	for (int i = 0; i < GAMEOVER_BUTTON_MAX; i++)
	{
		g_GameOverButton[i].pos = XMFLOAT3(SCREEN_CENTER_X - 175.0f + (i * 175.0f), (SCREEN_CENTER_Y / 2) - 75.0f, 0.0f);
		g_GameOverButton[i].w = TEXTURE_BUTTON_WIDTH;
		g_GameOverButton[i].h = TEXTURE_BUTTON_HEIGHT;
		g_GameOverButton[i].texNo = GAMEOVER_TEX_RETRY_ROUND_BUTTON + i;
		g_GameOverButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	g_GameOverButton[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);


	g_GameOverInput = FALSE;


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitScreenGameOver(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < GAMEOVER_TEX_MAX; i++)
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
void UpdateScreenGameOver(void)
{

	if (g_GameOverScreenAlpha < 1.0f)
	{
		g_GameOverScreenAlpha += 0.005f;

		if (g_GameOverScreenAlpha >= 1.0f)
		{
			g_GameOverInput = TRUE;
		}
	}




	if (g_GameOverInput)
	{
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
			g_GameOverInput = FALSE;
			OnClickGOButton(g_SelectedButton);
		}


		if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_RIGHT) || GetKeyboardTrigger(DIK_D))
		{
			PlaySound(SOUND_LABEL_SE_ChangeButton);

			g_GameOverButton[g_SelectedButton].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_SelectedButton = (g_SelectedButton + 1) % GAMEOVER_BUTTON_MAX;
			g_GameOverButton[g_SelectedButton].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		else if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_LEFT) || GetKeyboardTrigger(DIK_A))
		{
			PlaySound(SOUND_LABEL_SE_ChangeButton);

			g_GameOverButton[g_SelectedButton].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_SelectedButton = (--g_SelectedButton < 0) ? (GAMEOVER_BUTTON_MAX - 1) : g_SelectedButton;
			g_GameOverButton[g_SelectedButton].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		}
	}
	


#ifdef _DEBUG	// デバッグ情報を表示する

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScreenGameOver(void)
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

	// GAMEOVERの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GAMEOVER_TEX_BG]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2), 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_GameOverScreenAlpha));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// GAMEOVERのタイトルを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GAMEOVER_TEX_GAMEOVER_TITLE]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y - (SCREEN_HEIGHT / 4), (1000.0f / 2), (100.0f / 2), 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_GameOverScreenAlpha));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// Player DEAD を描画
	{
		//// テクスチャ設定
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GAMEOVER_TEX_DEAD_PLAYER]);

		//// １枚のポリゴンの頂点とテクスチャ座標を設定
		//SetSpriteColor(g_VertexBuffer, SCREEN_CENTER_X - 200.0f, SCREEN_CENTER_Y - 50.0f, 75.0f * 2, 50.0f * 2, 0.0f, 0.0f, 1.0f, 1.0f,
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, g_GameOverScreenAlpha));

		//// ポリゴン描画
		//GetDeviceContext()->Draw(4, 0);
	}

	// Button message を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GAMEOVER_TEX_RETRY_ROUND_MESSAGE + g_SelectedButton]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2), 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_GameOverScreenAlpha));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	DrawScreenGameOverButtons();
}

void DrawScreenGameOverButtons(void)
{
	for (int i = 0; i < GAMEOVER_BUTTON_MAX; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_GameOverButton[i].texNo]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_GameOverButton[i].pos.x;			// ボタンの表示位置X
		float py = g_GameOverButton[i].pos.y + SCREEN_CENTER_Y;			// ボタンの表示位置Y
		float pw = g_GameOverButton[i].w;		// ボタンの表示幅
		float ph = g_GameOverButton[i].h;		// ボタンの表示高さ


		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(g_GameOverButton[i].color.x, g_GameOverButton[i].color.y, g_GameOverButton[i].color.z, g_GameOverScreenAlpha));
		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}


void SetScreenGameOver(void)
{
	SetFinalScreen(SCREEN_GAMEOVER);
}


void OnClickGOButton(int button)
{
	switch (button)
	{
	case BUTTON_RETRY_ROUND:
		SetRetryRound(GetRound());
		//SetFade(FADE_OUT, MODE_COLISEUM);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_COLISEUM);
		break;

	case BUTTON_RETRY_COLISEUM:
		SetRetryRound(0);
		//SetFade(FADE_OUT, MODE_COLISEUM);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_COLISEUM);
		break;

	case BUTTON_BACK_TO_MENU:
		//SetRetryRound(0);
		//SetFade(FADE_OUT, MODE_TITLE);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_MENU);
		break;
	}
}