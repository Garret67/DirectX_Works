//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "result.h"
#include "input.h"
#include "score.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
//#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
//#define TEXTURE_MAX					(5)				// テクスチャの数

#define TEXTURE_BUTTON_WIDTH		(1000.0f / 3)	
#define TEXTURE_BUTTON_HEIGHT		(114.0f / 3)	

#define TEXTURE_NUM_WIDTH			(40.0f)	
#define TEXTURE_NUM_HEIGHT			(80.0f)

#define RESULT_BUTTON_MAX			(2)
#define STARS_MAX					(5)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawNumbers(XMFLOAT3 pos);
void DrawResultButtons(void);

void StarsAnimation(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[RESULT_TEX_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[RESULT_TEX_MAX] = {
	"data/TEXTURE/Menu/UIbgH.png",
	"data/TEXTURE/Menu/Numbers2.png",
	"data/TEXTURE/Player/Star.png",
	"data/TEXTURE/Player/StarGrey.png",

	"data/TEXTURE/Menu/MenuButtons/Japanese/ResultTitle.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Retry.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/ReturnToMenu.png",
};


//static BOOL			g_Use;						// TRUE:使っている  FALSE:未使用
//static float		g_w, g_h;					// 幅と高さ
//static XMFLOAT3		g_Pos;						// ポリゴンの座標
//static int			g_TexNo;					// テクスチャ番号

static XMFLOAT3			g_ResultScreenPos;

static DWORD			g_StartTime;
static DWORD			g_FinalTime;
//static DWORD			g_DSecs;
static DWORD			g_Secs;
static DWORD			g_Mins;

static int				g_HitsReceived;
static int				g_Stars;

static RESULT_BUTTON	g_ResultButton[RESULT_BUTTON_MAX];
static STAR				g_GreyStars[STARS_MAX];
static STAR				g_GoldStars[STARS_MAX];

static BOOL				g_StarsAnim;
static BOOL				g_DrawGoldStars;
static int				g_StarsDisplayed;

BOOL					g_ResultInput;

static BOOL				g_Load = FALSE;



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
{
	switch (GetLanguage())
	{
	case LANG_JPN:
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/Japanese/ResultTitle.png";
		g_TexturName[5] = "data/TEXTURE/Menu/MenuButtons/Japanese/Retry.png";
		g_TexturName[6] = "data/TEXTURE/Menu/MenuButtons/Japanese/ReturnToMenu.png";
		break;

	case LANG_USA:
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/English/ResultTitle.png";
		g_TexturName[5] = "data/TEXTURE/Menu/MenuButtons/English/Retry.png";
		g_TexturName[6] = "data/TEXTURE/Menu/MenuButtons/English/ReturnToMenu.png";
		break;

	case LANG_ESP:
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/Spanish/ResultTitle.png";
		g_TexturName[5] = "data/TEXTURE/Menu/MenuButtons/Spanish/Retry.png";
		g_TexturName[6] = "data/TEXTURE/Menu/MenuButtons/Spanish/ReturnToMenu.png";
		break;
	}


	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < RESULT_TEX_MAX; i++)
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


	g_ResultScreenPos = XMFLOAT3(SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 4, 0.0f);

	g_StartTime = timeGetTime();
	g_FinalTime = 0;

	//g_DSecs = 0;
	g_Secs  = 0;
	g_Mins  = 0;

	g_HitsReceived = 0;
	g_Stars = 0;
	g_StarsAnim = FALSE;
	g_DrawGoldStars = FALSE;
	g_StarsDisplayed = 0;

	for (int i = 0; i < RESULT_BUTTON_MAX; i++)
	{
		g_ResultButton[i].pos = XMFLOAT3(SCREEN_CENTER_X - (TEXTURE_BUTTON_WIDTH / 2) + (i * TEXTURE_BUTTON_WIDTH), /*SCREEN_CENTER_Y + */(SCREEN_CENTER_Y / 2) - 75.0f, 0.0f);
		g_ResultButton[i].w = TEXTURE_BUTTON_WIDTH;
		g_ResultButton[i].h = TEXTURE_BUTTON_HEIGHT;
		g_ResultButton[i].texNo = RESULT_TEX_RETRY_BUTTON + i;
		g_ResultButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	g_ResultButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	for (int i = 0; i < STARS_MAX; i++)
	{
		g_GreyStars[i].pos = XMFLOAT3(SCREEN_CENTER_X - 200.0f + (i * 100), -80.0f, 0.0f);
		g_GreyStars[i].scl = 1.0f;
		g_GreyStars[i].w = 100.0f;
		g_GreyStars[i].h = 100.0f;
		g_GreyStars[i].texNo = RESULT_TEX_STARGREY;
	}

	for (int i = 0; i < STARS_MAX; i++)
	{
		g_GoldStars[i].pos = XMFLOAT3(SCREEN_CENTER_X - 200.0f + (i * 100), -80.0f, 0.0f);
		g_GoldStars[i].scl = 3.0f;
		g_GoldStars[i].w = 100.0f;
		g_GoldStars[i].h = 100.0f;
		g_GoldStars[i].texNo = RESULT_TEX_STAR;
	}

	g_ResultInput = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < RESULT_TEX_MAX; i++)
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
void UpdateResult(void)
{
	if (g_ResultScreenPos.y < SCREEN_CENTER_Y)
	{
		g_ResultScreenPos.y += 5.0f;

		if (g_ResultScreenPos.y >= SCREEN_CENTER_Y)
		{
			g_StarsAnim = TRUE;
			g_DrawGoldStars = TRUE;
			g_ResultInput = TRUE;
		}
	}




	if (g_ResultInput)
	{
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
			g_ResultInput = FALSE;

			if (g_ResultButton[0].color.x == 0.0f)
			{
				//SetFade(FADE_OUT, GetMode());
				SetTransitionDoor(TRANSITION_DOOR_CLOSE, GetMode());
				//SetFinalScreen(SCREEN_INGAME);
			}
			else
			{
				//SetFade(FADE_OUT, MODE_TITLE);
				SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_MENU);
				//SetFinalScreen(SCREEN_INGAME);
			}
		}


		if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_RIGHT) || GetKeyboardTrigger(DIK_D))
		{
			PlaySound(SOUND_LABEL_SE_ChangeButton);
			g_ResultButton[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_ResultButton[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		else if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_LEFT) || GetKeyboardTrigger(DIK_A))
		{
			PlaySound(SOUND_LABEL_SE_ChangeButton);
			g_ResultButton[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_ResultButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		}
	}
	


	StarsAnimation();


#ifdef _DEBUG	// デバッグ情報を表示する
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
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

	// リザルトの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[RESULT_TEX_BG]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_ResultScreenPos.x, g_ResultScreenPos.y, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2), 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// リザルトのタイトルを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[RESULT_TEX_RESULT_TITLE]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, SCREEN_CENTER_X, g_ResultScreenPos.y - (SCREEN_HEIGHT / 4), (1000.0f / 2), (100.0f / 2), 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// リザルトのグレーほしを描画
	{
		for (int i = 0; i < STARS_MAX; i++)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[RESULT_TEX_STARGREY]);
		

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_GreyStars[i].pos.x;			// ボタンの表示位置X
			float py = g_GreyStars[i].pos.y + g_ResultScreenPos.y;			// ボタンの表示位置Y
			float pw = g_GreyStars[i].w;		// ボタンの表示幅
			float ph = g_GreyStars[i].h;		// ボタンの表示高さ


			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標
			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// リザルトのほしを描画
	{
		for (int i = 0; i < STARS_MAX; i++)
		{
			if (!g_DrawGoldStars) break;
			if (i > g_StarsDisplayed) break;

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[RESULT_TEX_STAR]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_GoldStars[i].pos.x;			// ボタンの表示位置X
			float py = g_GoldStars[i].pos.y + g_ResultScreenPos.y;			// ボタンの表示位置Y
			float pw = g_GoldStars[i].w * g_GoldStars[i].scl;		// ボタンの表示幅
			float ph = g_GoldStars[i].h * g_GoldStars[i].scl;		// ボタンの表示高さ


			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標
			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}


	// スコア表示
	{
		DrawNumbers(XMFLOAT3(SCREEN_CENTER_X, 30.0f, 0.0f));
	}


	DrawResultButtons();
}

void DrawResultButtons(void)
{
	for (int i = 0; i < RESULT_BUTTON_MAX; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ResultButton[i].texNo]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_ResultButton[i].pos.x;			// ボタンの表示位置X
		float py = g_ResultButton[i].pos.y + g_ResultScreenPos.y;			// ボタンの表示位置Y
		float pw = g_ResultButton[i].w;		// ボタンの表示幅
		float ph = g_ResultButton[i].h;		// ボタンの表示高さ


		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_ResultButton[i].color);
		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}



void DrawNumbers(XMFLOAT3 pos)
{
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[RESULT_TEX_NUMBER]);

	// 桁数分処理する
	int number = (int)g_FinalTime;

	for (int i = 0; i < 5; i++)
	{
		// 今回表示する桁の数字
		float x = (float)(number % 10);

		if (i == 2) x = 10.0f; //「：」を書く時
		

		// スコアの位置やテクスチャー座標を反映
		float pw = TEXTURE_NUM_WIDTH;			// スコアの表示幅
		float ph = TEXTURE_NUM_HEIGHT;			// スコアの表示高さ
		float px = (pos.x + TEXTURE_NUM_WIDTH * 2) - (i * pw) ;	// スコアの表示位置X
		float py = pos.y + g_ResultScreenPos.y;			// スコアの表示位置Y

		float tw = 1.0f / 11;		// テクスチャの幅
		float th = 1.0f;			// テクスチャの高さ
		float tx = x * tw;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// 次の桁へ
		number /= 10;
	}
}



void SetResultScreen(void)
{
	g_FinalTime = timeGetTime() - g_StartTime;

	//g_DSecs = (g_FinalTime / 10);
	g_Secs =  (g_FinalTime / 1000) % 60;
	g_Mins =  (g_FinalTime / 1000) / 60;

	g_FinalTime = g_Mins * 1000 + g_Secs;

	if (g_HitsReceived <= 10)
	{
		g_Stars = 5;
	}
	else if (g_HitsReceived <= 20)
	{
		g_Stars = 4;
	}
	else if (g_HitsReceived <= 30)
	{
		g_Stars = 3;
	}
	else if (g_HitsReceived <= 40)
	{
		g_Stars = 2;
	}
	else if (g_HitsReceived >= 50)
	{
		g_Stars = 1;
	}


	SetScore(g_Stars, (int)g_FinalTime);
	

	SetFinalScreen(SCREEN_RESULT);
}


void ResultHit(void)
{
	g_HitsReceived++;
}


void StarsAnimation(void)
{
	if (!g_StarsAnim) return;		//スターアニメーションのフラグ

	if (g_GoldStars[g_StarsDisplayed].scl > 1.0f)
	{
		g_GoldStars[g_StarsDisplayed].scl -= 0.05f;
	}
	else
	{
		g_GoldStars[g_StarsDisplayed].scl = 1.0f;
		g_StarsDisplayed++;

		if (g_StarsDisplayed == g_Stars)
		{
			g_StarsDisplayed--;
			g_StarsAnim = FALSE;
		}
	}
}