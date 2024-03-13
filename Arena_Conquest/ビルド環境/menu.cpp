//=============================================================================
//
// タイトル画面処理 [menu.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "menu.h"
#include "options.h"
#include "score.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(16)				// テクスチャの数

#define TEXTURE_BUTTON_WIDTH		(1000.0f / 3)		// ロゴサイズ
#define TEXTURE_BUTTON_HEIGHT		(115.0f / 3)		// 

#define PARTICLE_SPAWN_TIME			(15)

#define MENU_PARTICLE_MAX			(50)

#define TEXTURE_NUM_WIDTH			(15.0f)	
#define TEXTURE_NUM_HEIGHT			(30.0f)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawMenuButton(void);
void DrawMenuControlButtons(void);

void OnClickButton(int button);

void SetMenuPartible(XMFLOAT3 pos);
void DrawMenuParticles(void);

void UpdateColiseumOptions(void);
void OnClickButtonColiseumOptions(void);
void DrawColiseumOptions(void);
void DrawColiseumOptionsNumbers(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Menu/MenuColiseum2.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Coliseum.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Tutorial.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Scores.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Options.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Exit.png",
	"data/TEXTURE/Menu/UIbgV.png",
	"data/TEXTURE/Menu/MenuButtons/ArenaConquer.png",

	"data/TEXTURE/Menu/MenuButtons/Japanese/AcceptButton.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/BackButton.png",

	"data/TEXTURE/Menu/MenuButtons/MenuControlButtons/ButtonEnter.png",
	"data/TEXTURE/Menu/MenuButtons/MenuControlButtons/ButtonEsc.png",

	"data/TEXTURE/Effect/Particle.png",

	"data/TEXTURE/Menu/Numbers2.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/ColiseumOptionFromStart.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/ColiseumOptionSelectRound.png",
};


static BOOL				g_bg_Use;						// TRUE:使っている  FALSE:未使用
static float			g_bg_w, g_h;					// 幅と高さ
static XMFLOAT3			g_bg_Pos;						// ポリゴンの座標
static int				g_bg_TexNo;					// テクスチャ番号


static MENU_BUTTON		menuButton[BUTTON_MAX];
static MENU_BUTTON		menuColiseumButton[BUTTON_COLISEUM_MAX];
static int				buttonSelected;
static int				g_StartRound;

MENU_PARTICLE			g_MenuParticle[MENU_PARTICLE_MAX];
int						g_MenuParticleSpawnTimeCnt;

static int				g_MenuScreen;

BOOL					g_MenuInput;

static BOOL				g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMenu(void)
{
	switch (GetLanguage())
	{
	case LANG_JPN:
		g_TexturName[1]  = "data/TEXTURE/Menu/MenuButtons/Japanese/Coliseum.png";
		g_TexturName[2]  = "data/TEXTURE/Menu/MenuButtons/Japanese/Tutorial.png";
		g_TexturName[3]  = "data/TEXTURE/Menu/MenuButtons/Japanese/Scores.png",
		g_TexturName[4]  = "data/TEXTURE/Menu/MenuButtons/Japanese/Options.png";
		g_TexturName[5]  = "data/TEXTURE/Menu/MenuButtons/Japanese/Exit.png";
		g_TexturName[8]  = "data/TEXTURE/Menu/MenuButtons/Japanese/AcceptButton.png";
		g_TexturName[9]  = "data/TEXTURE/Menu/MenuButtons/Japanese/BackButton.png";
		g_TexturName[14] = "data/TEXTURE/Menu/MenuButtons/Japanese/ColiseumOptionFromStart.png";
		g_TexturName[15] = "data/TEXTURE/Menu/MenuButtons/Japanese/ColiseumOptionSelectRound.png";

		break;

	case LANG_USA:
		g_TexturName[1]  = "data/TEXTURE/Menu/MenuButtons/English/Coliseum.png";
		g_TexturName[2]  = "data/TEXTURE/Menu/MenuButtons/English/Tutorial.png";
		g_TexturName[3]  = "data/TEXTURE/Menu/MenuButtons/English/Scores.png",
		g_TexturName[4]  = "data/TEXTURE/Menu/MenuButtons/English/Options.png";
		g_TexturName[5]  = "data/TEXTURE/Menu/MenuButtons/English/Exit.png";
		g_TexturName[8]  = "data/TEXTURE/Menu/MenuButtons/English/AcceptButton.png";
		g_TexturName[9]  = "data/TEXTURE/Menu/MenuButtons/English/BackButton.png";
		g_TexturName[14] = "data/TEXTURE/Menu/MenuButtons/English/ColiseumOptionFromStart.png";
		g_TexturName[15] = "data/TEXTURE/Menu/MenuButtons/English/ColiseumOptionSelectRound.png";
		break;

	case LANG_ESP:
		g_TexturName[1]  = "data/TEXTURE/Menu/MenuButtons/Spanish/Coliseum.png";
		g_TexturName[2]  = "data/TEXTURE/Menu/MenuButtons/Spanish/Tutorial.png";
		g_TexturName[3]  = "data/TEXTURE/Menu/MenuButtons/Spanish/Scores.png",
		g_TexturName[4]  = "data/TEXTURE/Menu/MenuButtons/Spanish/Options.png";
		g_TexturName[5]  = "data/TEXTURE/Menu/MenuButtons/Spanish/Exit.png";
		g_TexturName[8]  = "data/TEXTURE/Menu/MenuButtons/Spanish/AcceptButton.png";
		g_TexturName[9]  = "data/TEXTURE/Menu/MenuButtons/Spanish/BackButton.png";
		g_TexturName[14] = "data/TEXTURE/Menu/MenuButtons/Spanish/ColiseumOptionFromStart.png";
		g_TexturName[15] = "data/TEXTURE/Menu/MenuButtons/Spanish/ColiseumOptionSelectRound.png";
		break;
	}

	if (CheckGamepadConnection() == TRUE)
	{
		g_TexturName[10] = "data/TEXTURE/Menu/MenuButtons/MenuControlButtons/ButtonA.png";
		g_TexturName[11] = "data/TEXTURE/Menu/MenuButtons/MenuControlButtons/ButtonB.png";
	}
	else
	{
		g_TexturName[10] = "data/TEXTURE/Menu/MenuButtons/MenuControlButtons/ButtonEnter.png";
		g_TexturName[11] = "data/TEXTURE/Menu/MenuButtons/MenuControlButtons/ButtonEsc.png";
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
	g_h		   = TEXTURE_HEIGHT;
	g_bg_Pos   = XMFLOAT3(g_bg_w/2, g_h/2, 0.0f);
	g_bg_TexNo = 0;

	for (int i = 0; i < BUTTON_MAX; i++)
	{
		menuButton[i].use	= TRUE;
		menuButton[i].pos	= XMFLOAT3((SCREEN_CENTER_X / 2) - 50.0f, 300.0f + (i * 70.0f), 0.0f);	// 中心点から表示
		menuButton[i].rot	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		menuButton[i].scl	= XMFLOAT3(1.0f, 1.0f, 1.0f);
		menuButton[i].w		= TEXTURE_BUTTON_WIDTH;
		menuButton[i].h		= TEXTURE_BUTTON_HEIGHT;
		menuButton[i].texNo = 1 + i;
		menuButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	menuButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	
	
	for (int i = 0; i < BUTTON_COLISEUM_MAX; i++)
	{
		menuColiseumButton[i].use	= TRUE;
		menuColiseumButton[i].pos	= XMFLOAT3((SCREEN_CENTER_X / 2) - 50.0f, 300.0f + (i * 70.0f), 0.0f);	// 中心点から表示
		menuColiseumButton[i].rot	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		menuColiseumButton[i].scl	= XMFLOAT3(1.0f, 1.0f, 1.0f);
		menuColiseumButton[i].w		= TEXTURE_BUTTON_WIDTH;
		menuColiseumButton[i].h		= TEXTURE_BUTTON_HEIGHT;
		menuColiseumButton[i].texNo = 14 + i;
		menuColiseumButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	menuColiseumButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	buttonSelected = 0;

	g_StartRound = 1;


	for (int i = 0; i < MENU_PARTICLE_MAX; i++)
	{

		g_MenuParticle[i].use = FALSE;
		g_MenuParticle[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_MenuParticle[i].w = 10.0f;
		g_MenuParticle[i].h = 10.0f;
		g_MenuParticle[i].texNo = 12;

		g_MenuParticle[i].moveSpeed = 2.0f;
		g_MenuParticle[i].moveDir = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_MenuParticle[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_MenuParticle[i].lifeTime = 0;
		g_MenuParticle[i].lifeTimeCnt = 0;
		g_MenuParticle[i].alphaAdd = 0.0f;
	}

	g_MenuScreen = SCREEN_MAIN_MENU;

	g_MenuInput = TRUE;

	InitOptions();
	InitScore();
	SetRetryRound(0);
	SetPauseScreen(FALSE);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMenu(void)
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
void UpdateMenu(void)
{

	float posX = (float)(rand() % SCREEN_WIDTH);
	SetMenuPartible(XMFLOAT3(posX, SCREEN_HEIGHT, 0.0f));

	if (g_MenuScreen == SCREEN_OPTIONS)
	{
		UpdateOptions();
		return;
	}
	else if (g_MenuScreen == SCREEN_SCORES)
	{
		UpdateScore();
		return;
	}
	else if (g_MenuScreen == SCREEN_COLISEUM)
	{
		UpdateColiseumOptions();
		return;
	}

	if (g_MenuInput)
	{
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{// Enter押したら、ステージを切り替える
			OnClickButton(buttonSelected);
			buttonSelected = 0;
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
		}

		//Pause Mode
		if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_B))
		{
			SetFade(FADE_OUT, MODE_TITLE);
			g_MenuInput = FALSE;
			PlaySound(SOUND_LABEL_SE_PulseButtonBack);
		}


		// ボタンの選択
		if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN) || GetKeyboardTrigger(DIK_S))
		{
			menuButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			buttonSelected = (buttonSelected + 1) % BUTTON_MAX;
			menuButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}
		else if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP) || GetKeyboardTrigger(DIK_W))
		{
			menuButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			buttonSelected = (--buttonSelected < 0) ? (BUTTON_MAX - 1) : buttonSelected;
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
void DrawMenu(void)
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

	DrawMenuParticles();

	 /*Buttons BG*/
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, (SCREEN_CENTER_X / 2) - 50.0f, SCREEN_CENTER_Y, SCREEN_CENTER_X - 200, TEXTURE_HEIGHT - 100, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//Game Title  
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, (SCREEN_CENTER_X / 2) - 50.0f, 150.0f, 600.0f * 0.6f, 125.0f * 0.6f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	DrawMenuControlButtons();

	if (g_MenuScreen == SCREEN_OPTIONS)
	{
		DrawOptions();
		return;
	}
	else if (g_MenuScreen == SCREEN_SCORES)
	{
		DrawScore();
		return;
	}
	else if (g_MenuScreen == SCREEN_COLISEUM)
	{
		DrawColiseumOptions();
		DrawColiseumOptionsNumbers();
		return;
	}

	DrawMenuButton();
	
}

//=============================================================================
// Menu Control Button を描画
//=============================================================================
void DrawMenuControlButtons(void)
{

	for (int i = 0; i < 2; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8+i]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = SCREEN_WIDTH - 90.0f;			// ボタンの表示位置X
		float py = SCREEN_HEIGHT - 175.0f + (i * 50.0f);			// ボタンの表示位置Y
		float pw = 500.0f / 3;		// ボタンの表示幅
		float ph = TEXTURE_BUTTON_HEIGHT;		// ボタンの表示高さ


		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10 + i]);

		//プレイヤーの位置やテクスチャー座標を反映
		px = SCREEN_WIDTH - 160.0f;			// ボタンの表示位置X
		pw = 285.0f / 3;		// ボタンの表示幅


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


}


//=============================================================================
// Menu Button を描画
//=============================================================================
void DrawMenuButton(void)
{
	if (g_MenuScreen != SCREEN_MAIN_MENU) return;

	for (int i = 0; i < BUTTON_MAX; i++)
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
// OnClick Button 
// 引数：選択しているボタン
//=============================================================================
void OnClickButton(int button)
{
	switch (button)
	{
	case BUTTON_COLISEUM:
		
		//SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_COLISEUM);
		//g_MenuInput = FALSE;
		g_MenuScreen = SCREEN_COLISEUM;
		break;

	case BUTTON_TUTORIAL:
		//SetFade(FADE_OUT, MODE_TUTORIAL_1);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_TUTORIAL_1);
		g_MenuInput = FALSE;
		break;

	case BUTTON_SCORES:
		g_MenuScreen = SCREEN_SCORES;
		menuButton[BUTTON_SCORES].color   = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		menuButton[BUTTON_COLISEUM].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		break;

	case BUTTON_OPTIONS:
		g_MenuScreen = SCREEN_OPTIONS;

		menuButton[BUTTON_OPTIONS].color  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		menuButton[BUTTON_COLISEUM].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		//音量設定
		//明るさ設定
		//言語設定
		break;

	case BUTTON_QUIT_GAME:
		QuitGame();
		break;
	}
}


//=============================================================================
// メインメニューボタンの取得
//=============================================================================
XMFLOAT3 GetMainMenuButtonPos(void)
{
	return menuButton[0].pos;
}


//=============================================================================
// メインメニューに戻る
//=============================================================================
void BackToMainMenu(void)
{
	g_MenuScreen = SCREEN_MAIN_MENU;
}

//=============================================================================
// メインメニューパーティクルの設定
//=============================================================================
void SetMenuPartible(XMFLOAT3 pos)
{
	g_MenuParticleSpawnTimeCnt++;
	if (g_MenuParticleSpawnTimeCnt > PARTICLE_SPAWN_TIME)
	{
		g_MenuParticleSpawnTimeCnt = 0;

		for (int i = 0; i < MENU_PARTICLE_MAX; i++)
		{
			if (g_MenuParticle[i].use == FALSE)
			{
				g_MenuParticle[i].use = TRUE;
				g_MenuParticle[i].pos = pos;
				g_MenuParticle[i].moveSpeed = 0.0f;

				float angle = (float)(rand() % 78);
				angle /= 100;
				angle += XM_PIDIV2 - XM_PIDIV4 / 2;
				angle *= -1;
				g_MenuParticle[i].moveDir.x = cosf(angle);	//angleの方向へ移動
				g_MenuParticle[i].moveDir.y = sinf(angle);	//angleの方向へ移動

				g_MenuParticle[i].lifeTime = MENU_PARTICLE_MAX;
				g_MenuParticle[i].lifeTimeCnt = 0;

				g_MenuParticle[i].alphaAdd = 0.007f;

				g_MenuParticle[i].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
				break;
			}
		}
	}
	

	for (int i = 0; i < MENU_PARTICLE_MAX; i++)
	{
		if (g_MenuParticle[i].use == TRUE)
		{
			g_MenuParticle[i].lifeTimeCnt++;
			g_MenuParticle[i].moveSpeed += 0.02f;
			g_MenuParticle[i].pos.x += g_MenuParticle[i].moveDir.x * g_MenuParticle[i].moveSpeed;
			g_MenuParticle[i].pos.y += g_MenuParticle[i].moveDir.y * g_MenuParticle[i].moveSpeed;
			g_MenuParticle[i].color.y += 0.002f/*1.0f / TITLE_PARTICLE_MAX*/;

			if (g_MenuParticle[i].color.w <= 1.0f)
			{
				g_MenuParticle[i].color.w += g_MenuParticle[i].alphaAdd;
			}

			//画面端の当たり判定
			{
				if (g_MenuParticle[i].pos.x < (-g_MenuParticle[i].w / 2))		// 右
				{
					g_MenuParticle[i].use = FALSE;
				}
				if (g_MenuParticle[i].pos.x > (SCREEN_WIDTH + g_MenuParticle[i].w / 2))	// 左
				{
					g_MenuParticle[i].use = FALSE;
				}
				if (g_MenuParticle[i].pos.y < (-g_MenuParticle[i].h / 2))		// 上
				{
					g_MenuParticle[i].use = FALSE;
				}
			}
		}
	}
}

//=============================================================================
// メインメニューのパーティクル描画
//=============================================================================
void DrawMenuParticles(void)
{
	for (int i = 0; i < MENU_PARTICLE_MAX; i++)
	{
		if (g_MenuParticle[i].use == TRUE)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_MenuParticle[i].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_MenuParticle[i].pos.x;	// エネミーの表示位置X
			float py = g_MenuParticle[i].pos.y;	// エネミーの表示位置Y
			float pw = g_MenuParticle[i].w;					// エネミーの表示幅
			float ph = g_MenuParticle[i].h;					// エネミーの表示高さ

			float tw = 1.0f;		// テクスチャの幅
			float th = 1.0f;		// テクスチャの高さ
			float tx = 0.0f;		// テクスチャの左上X座標
			float ty = 0.0f;		// テクスチャの左上Y座標


			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_MenuParticle[i].color);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// コロシアムボタンのInput処理
//=============================================================================
void UpdateColiseumOptions(void)
{
	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
	{
		OnClickButtonColiseumOptions();
		PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
	}


	//Get out Debug Mode
	if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_B))
	{
		g_MenuScreen = SCREEN_MAIN_MENU;
		buttonSelected = 0;
		menuColiseumButton[BUTTON_COLISEUM_FROM_START].color	 = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		menuColiseumButton[BUTTON_COLISEUM_ROUND_SELECTOR].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		PlaySound(SOUND_LABEL_SE_PulseButtonBack);
	}


	// ボタンの選択→上下
	if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN) || GetKeyboardTrigger(DIK_S))
	{
		menuColiseumButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		buttonSelected = (buttonSelected + 1) % BUTTON_COLISEUM_MAX;
		menuColiseumButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		PlaySound(SOUND_LABEL_SE_ChangeButton);
	}
	else if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP) || GetKeyboardTrigger(DIK_W))
	{
		menuColiseumButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		buttonSelected = (--buttonSelected < 0) ? (BUTTON_COLISEUM_MAX - 1) : buttonSelected;
		menuColiseumButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		PlaySound(SOUND_LABEL_SE_ChangeButton);
	}

	// 数値の選択→左右
	if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_RIGHT) || GetKeyboardTrigger(DIK_D))
	{
		if (buttonSelected == BUTTON_COLISEUM_ROUND_SELECTOR)
		{
			if (g_StartRound < 8)
			{
				g_StartRound++;
				PlaySound(SOUND_LABEL_SE_ChangeButton);
			}
		}
	}
	else if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_LEFT) || GetKeyboardTrigger(DIK_A))
	{
		if (buttonSelected == BUTTON_COLISEUM_ROUND_SELECTOR)
		{
			if (g_StartRound > 1)
			{
				g_StartRound--;
				PlaySound(SOUND_LABEL_SE_ChangeButton);
			}
		}
	}
}


//=============================================================================
// コロシアムボタンのOnClick Button 
// 引数：選択しているボタン
//=============================================================================
void OnClickButtonColiseumOptions(void)
{
	switch (buttonSelected)
	{
	case BUTTON_COLISEUM_FROM_START:
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_COLISEUM);
		g_MenuInput = FALSE;		
		break;

	case BUTTON_COLISEUM_ROUND_SELECTOR:
		SetRetryRound(g_StartRound);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_COLISEUM);
		g_MenuInput = FALSE;
		break;
	}
}


//=============================================================================
// コロシアムのボタンの描画
//=============================================================================
void DrawColiseumOptions(void)
{
	for (int i = 0; i < BUTTON_COLISEUM_MAX; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[menuColiseumButton[i].texNo]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = menuColiseumButton[i].pos.x;			// ボタンの表示位置X
		float py = menuColiseumButton[i].pos.y;			// ボタンの表示位置Y
		float pw = TEXTURE_BUTTON_WIDTH;		// ボタンの表示幅
		float ph = TEXTURE_BUTTON_HEIGHT;		// ボタンの表示高さ


		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			menuColiseumButton[i].color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// コロシアムボタンの数字描画
//=============================================================================
void DrawColiseumOptionsNumbers(void)
{
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);

	
	// 桁数分処理する
	int number = g_StartRound;

	// 今回表示する桁の数字
	float x = (float)(number % 10);


	// スコアの位置やテクスチャー座標を反映
	float pw = TEXTURE_NUM_WIDTH * 1.5f;			// スコアの表示幅
	float ph = TEXTURE_NUM_HEIGHT * 1.5f;			// スコアの表示高さ
	float px = menuColiseumButton[1].pos.x + 93.0f;	// スコアの表示位置X
	float py = menuColiseumButton[1].pos.y;			// スコアの表示位置Y

	float tw = 1.0f / 11.0f;		// テクスチャの幅
	float th = 1.0f;			// テクスチャの高さ
	float tx = x * tw;			// テクスチャの左上X座標
	float ty = 0.0f;			// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		menuColiseumButton[1].color);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// 次の桁へ
	number /= 10;
}

