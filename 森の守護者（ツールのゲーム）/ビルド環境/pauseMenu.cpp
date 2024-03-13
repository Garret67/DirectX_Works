//=============================================================================
//
// タイトル画面処理 [pauseMenu.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "pauseMenu.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "game.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define TEXTURE_MAX		(4)

#define OPTIONS_SPACE	(50.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void OnClickButtonPauseMenu(int button);

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct MENU_SPRITE
{
	BOOL		use;		// TRUE:使っている  FALSE:未使用
	float		w, h;		// 幅と高さ
	XMFLOAT3	pos;		// ポリゴンの座標
	int			texNo;		// テクスチャ番号
};


struct PAUSE_MENU
{
	MENU_SPRITE leftBG;
	MENU_SPRITE pauseMenuTitleBG;
	//MENU_SPRITE optionBG;
};

struct MENU_OPTION
{
	BOOL		use;		// TRUE:使っている  FALSE:未使用
	float		w, h;		// 幅と高さ
	XMFLOAT3	pos;		// ポリゴンの座標
	char		text[256];
};


enum MENU_OPTIONS {
	OP_CONTINUE,
	OP_OPTIONS,
	OP_SAVE,
	OP_BACK_TO_TITLE,

	MAX_OPTIONS,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = 
{
	"data/TEXTURE/Menu/fade_white.png",	
	"data/TEXTURE/Menu/UIbgH.png",	
	"data/TEXTURE/Menu/UIbgV.png",	
	"data/TEXTURE/Menu/OptionsBG.png",	

};

static PAUSE_MENU	g_PauseMenu;
static MENU_OPTION	g_MenuOptions[MAX_OPTIONS];
static int 			g_SelectedOption;

static BOOL			g_PauseMenuMoving = TRUE;
static XMFLOAT4		g_ScreenColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f );


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPauseMenu(void)
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
	g_PauseMenu.leftBG.use		= TRUE;
	g_PauseMenu.leftBG.w		= 300.0f;
	g_PauseMenu.leftBG.h		= SCREEN_HEIGHT;
	g_PauseMenu.leftBG.pos		= XMFLOAT3(g_PauseMenu.leftBG.w / 2, SCREEN_CENTER_Y, 0.0f);
	g_PauseMenu.leftBG.texNo	= 2;

	g_PauseMenu.pauseMenuTitleBG.use	= TRUE;
	g_PauseMenu.pauseMenuTitleBG.pos	= XMFLOAT3(0.0f, 75.0f, 0.0f);
	g_PauseMenu.pauseMenuTitleBG.w		= 250.0f;
	g_PauseMenu.pauseMenuTitleBG.h		= 50.0f;
	g_PauseMenu.pauseMenuTitleBG.texNo	= 1;

	for (int i = 0; i < MAX_OPTIONS; i++)
	{
		g_MenuOptions[i].use	= TRUE;
		g_MenuOptions[i].pos	= XMFLOAT3(0.0f, 150.f + OPTIONS_SPACE * i, 0.0f);
		g_MenuOptions[i].w		= g_PauseMenu.leftBG.w;
		g_MenuOptions[i].h		= 30.0f;
	}
	strcpy_s(g_MenuOptions[0].text, 256, "再開");
	strcpy_s(g_MenuOptions[1].text, 256, "音量");
	strcpy_s(g_MenuOptions[2].text, 256, "セーブ");
	strcpy_s(g_MenuOptions[3].text, 256, "タイトルに戻る");


	g_SelectedOption = OP_CONTINUE;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPauseMenu(void)
{

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

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePauseMenu(void)
{
	if (GetPauseMode() == PAUSE_MODE_MENU)
	{
		if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_JS_DOWN) || IsButtonTriggered(0, BUTTON_ARROW_DOWN))
		{
			g_SelectedOption = (g_SelectedOption + 1) % MAX_OPTIONS;		//選択しているボタンを更新
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}
		else if (GetKeyboardTrigger(DIK_UP) || GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_JS_UP) || IsButtonTriggered(0, BUTTON_ARROW_UP))
		{
			g_SelectedOption = g_SelectedOption ? --g_SelectedOption : MAX_OPTIONS - 1;	//選択しているボタンを
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}

		//ボタンの動作を行う
		if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger( DIK_SPACE) || IsButtonTriggered(0, BUTTON_B) || IsButtonTriggered(0, BUTTON_R))
		{
			OnClickButtonPauseMenu(g_SelectedOption);
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
		}

		//ゲームに戻る
		if (GetKeyboardTrigger(DIK_ESCAPE) || GetKeyboardTrigger(DIK_A))
		{
			PlaySound(SOUND_LABEL_SE_PulseButtonBack);
		}
	}


	if (g_PauseMenuMoving)
	{

		//左右の動き
		{
			static float pointerMoveCnt;
			float pointerMoveMax = 60;

			float angle = (XM_2PI / pointerMoveMax) * pointerMoveCnt;
			float x = cosf(angle);

			g_PauseMenu.leftBG.pos.x = g_PauseMenu.leftBG.w/2 * -x;

			g_ScreenColor.w = (-x + 1) / 3;

			if (GetPauseMode() == PAUSE_MODE_MENU)
			{
				pointerMoveCnt++;
				if (pointerMoveCnt >= pointerMoveMax/2) g_PauseMenuMoving = FALSE;
			}
			else
			{
				pointerMoveCnt--;
				if (pointerMoveCnt <= 0) g_PauseMenuMoving = FALSE;

			}
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
	if (g_PauseMenuMoving || GetPauseMode() == PAUSE_MODE_MENU)
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
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, g_ScreenColor);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		//*******************
		// leftBGを描画
		//*******************
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_PauseMenu.leftBG.texNo]);

		//背景の位置やテクスチャー座標を反映
		float px = g_PauseMenu.leftBG.pos.x;	// 表示位置X
		float py = g_PauseMenu.leftBG.pos.y;	// 表示位置Y
		float pw = g_PauseMenu.leftBG.w;		// 表示幅
		float ph = g_PauseMenu.leftBG.h;		// 表示高さ

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th/*, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)*/);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		//*******************
		// pauseMenuTitleBGを描画
		//*******************
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_PauseMenu.pauseMenuTitleBG.texNo]);

		//位置座標を反映
		px = g_PauseMenu.pauseMenuTitleBG.pos.x;	// 表示位置X
		py = g_PauseMenu.pauseMenuTitleBG.pos.y;	// 表示位置Y
		pw = g_PauseMenu.pauseMenuTitleBG.w;		// 表示幅
		ph = g_PauseMenu.pauseMenuTitleBG.h;		// 表示高さ

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		px += g_PauseMenu.leftBG.pos.x;
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th/*, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)*/);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//SetFont(L"Showcard Gothic", 35.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
		//DrawStringRect(L"PAUSE", D2D1::RectF(px - pw / 2, py - ph / 2, px + pw / 2, py + ph / 2), D2D1_DRAW_TEXT_OPTIONS_NONE);
		DrawTextRect(FormatToString("PAUSE"), 35, XMFLOAT4(0, 0, 0, 1),
			px - pw / 2, py - ph / 2, px + pw / 2, py + ph / 2, TEXT_ANCHOR_CENTER_CENTER, "HG創英角ﾎﾟｯﾌﾟ体");



		//*******************
		// optionBGを描画
		//*******************
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		//位置座標を反映
		px = g_MenuOptions[g_SelectedOption].pos.x;	// 表示位置X
		py = g_MenuOptions[g_SelectedOption].pos.y;	// 表示位置Y
		pw = g_MenuOptions[g_SelectedOption].w;		// 表示幅
		ph = g_MenuOptions[g_SelectedOption].h;		// 表示高さ

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		px += g_PauseMenu.leftBG.pos.x;
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		for (int i = 0; i < MAX_OPTIONS; i++)
		{
			DrawTextRect(FormatToString(g_MenuOptions[i].text), 20, XMFLOAT4(0, 0, 0, 1),
				px - pw, g_MenuOptions[i].pos.y - ph, px + pw, g_MenuOptions[i].pos.y + ph, TEXT_ANCHOR_CENTER_CENTER, "HG創英角ﾎﾟｯﾌﾟ体");

		}


	}

}




//=============================================================================
// OnClick Button を描画
//=============================================================================
void OnClickButtonPauseMenu(int button)
{
	switch (button)
	{
	case OP_CONTINUE:
		SetPauseMode(PAUSE_MODE_OFF);
		break;

	case OP_OPTIONS:

		break;

	case OP_SAVE:

		break;


	case OP_BACK_TO_TITLE:
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_TITLE);
		break;

	default:
		break;
	}
}




void MoveMenu(void)
{
	g_PauseMenuMoving = TRUE;
}


