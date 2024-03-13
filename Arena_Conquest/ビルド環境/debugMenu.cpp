//=============================================================================
//
// タイトル画面処理 [debugMenu.cpp]
// Author : 
//
//=============================================================================
#include "debugMenu.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH / 2)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT / 2)	// 
#define TEXTURE_MAX					(6)				// テクスチャの数

#define TEXTURE_BUTTON_WIDTH		(1000.0f / 3)		// ロゴサイズ
#define TEXTURE_BUTTON_HEIGHT		(114.0f / 3)		// 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawMenuButtonPauseMenu(void);
void OnClickButtonPauseMenu(int button);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = 
{
	"data/TEXTURE/Menu/UIbgH.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Resume.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Options.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/ReturnToMenu.png",

	"data/TEXTURE/Menu/MenuButtons/PauseTitle.png",

	"data/TEXTURE/Effect/fade_black.png",
};


static BOOL						g_bg_Use;						// TRUE:使っている  FALSE:未使用
static float					g_bg_w, g_bg_h;					// 幅と高さ
static XMFLOAT3					g_bg_Pos;						// ポリゴンの座標
static int						g_bg_TexNo;					// テクスチャ番号


static PAUSE_MENU_BUTTON		menuButton[PAUSE_BUTTON_MAX];

static int						buttonSelected;

static int						g_PauseScreen;

BOOL							g_PauseInput;

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPauseMenu(void)
{
	switch (GetLanguage())
	{
	case LANG_JPN:
		g_TexturName[1] = "data/TEXTURE/Menu/MenuButtons/Japanese/Resume.png";
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/Japanese/Options.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/Japanese/ReturnToMenu.png";
		break;

	case LANG_USA:
		g_TexturName[1] = "data/TEXTURE/Menu/MenuButtons/English/Resume.png";
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/English/Options.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/English/ReturnToMenu.png";
		break;

	case LANG_ESP:
		g_TexturName[1] = "data/TEXTURE/Menu/MenuButtons/Spanish/Resume.png";
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/Spanish/Options.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/Spanish/ReturnToMenu.png";
		break;
	}


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
	g_bg_Use   = TRUE;
	g_bg_w     = TEXTURE_WIDTH;
	g_bg_h	   = TEXTURE_HEIGHT;
	g_bg_Pos   = XMFLOAT3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);
	g_bg_TexNo = 0;

	for (int i = 0; i < PAUSE_BUTTON_MAX; i++)
	{
		menuButton[i].use	= TRUE;
		menuButton[i].pos	= XMFLOAT3((SCREEN_CENTER_X), (SCREEN_CENTER_Y - 50.0f) + (i * 60.0f), 0.0f);	// 中心点から表示
		menuButton[i].rot	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		menuButton[i].scl	= XMFLOAT3(1.0f, 1.0f, 1.0f);
		menuButton[i].w		= TEXTURE_BUTTON_WIDTH;
		menuButton[i].h		= TEXTURE_BUTTON_HEIGHT;
		menuButton[i].texNo = 1 + i;
		menuButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	menuButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);


	g_PauseScreen = PAUSE_SCREEN_MAIN;
	buttonSelected = 0;

	g_PauseInput = TRUE;

	InitOptions();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPauseMenu(void)
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

	UninitOptions();

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePauseMenu(void)
{
	if (g_PauseScreen == PAUSE_SCREEN_OPTIONS)
	{
		UpdateOptions();
		return;
	}


	if (g_PauseInput)
	{
		//Pause Mode
		if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_B))
		{
			SetPauseScreen(FALSE);
			PlaySound(SOUND_LABEL_SE_PulseButtonBack);
		}

		//ボタンを押したら
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			OnClickButtonPauseMenu(buttonSelected);
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
		}


		// ボタンの選択
		if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN) || GetKeyboardTrigger(DIK_S))
		{
			menuButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			buttonSelected = (buttonSelected + 1) % PAUSE_BUTTON_MAX;
			menuButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}
		else if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP) || GetKeyboardTrigger(DIK_W))
		{
			menuButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			buttonSelected = (--buttonSelected < 0) ? (PAUSE_BUTTON_MAX - 1) : buttonSelected;
			menuButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}
	}
	


#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPauseMenu(void)
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


	//Black screen  
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// Pauseの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_bg_TexNo]);

		//Pauseの背景の位置やテクスチャー座標を反映
		float px = g_bg_Pos.x;	// 表示位置X
		float py = g_bg_Pos.y;	// 表示位置Y
		float pw = g_bg_w;		// 表示幅
		float ph = g_bg_h;		// 表示高さ


		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	//Pause Title  
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_bg_Pos.x, g_bg_Pos.y - g_bg_h / 2 + 12.0f, TEXTURE_BUTTON_WIDTH, TEXTURE_BUTTON_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	if (g_PauseScreen == PAUSE_SCREEN_OPTIONS)
	{
		DrawOptions();
		return;
	}

	DrawMenuButtonPauseMenu();

}


//=============================================================================
// Menu Button を描画
//=============================================================================
void DrawMenuButtonPauseMenu(void)
{

	for (int i = 0; i < PAUSE_BUTTON_MAX; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[menuButton[i].texNo]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = menuButton[i].pos.x;			// ボタンの表示位置X
		float py = menuButton[i].pos.y;			// ボタンの表示位置Y
		float pw = TEXTURE_BUTTON_WIDTH;		// ボタンの表示幅
		float ph = TEXTURE_BUTTON_HEIGHT;		// ボタンの表示高さ


		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			menuButton[i].color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	

}



//=============================================================================
// OnClick Button を描画
//=============================================================================
void OnClickButtonPauseMenu(int button)
{
	switch (button)
	{
	case PAUSE_BUTTON_NEW_GAME:
		SetPauseScreen(FALSE);
		break;

	case PAUSE_BUTTON_OPTIONS:
		g_PauseScreen = PAUSE_SCREEN_OPTIONS;
		//音量設定
		//明るさ設定
		//言語設定
		break;

	case PAUSE_BUTTON_QUIT_GAME:
		//SetFade(FADE_OUT, MODE_TITLE);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_MENU);
		g_PauseInput = FALSE;
		//SetPauseScreen(FALSE);
		break;
	}
}


XMFLOAT3 GetPauseMenuButtonPos(void)
{
	return menuButton[0].pos;
}


void BackToPauseMenu(void)
{
	g_PauseScreen = PAUSE_SCREEN_MAIN;
}