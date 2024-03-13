//=============================================================================
//
// スコア処理 [score.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "score.h"
#include "sprite.h"
#include "sound.h"
#include "fade.h"

#include "menu.h"
#include "file.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH / 2)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT / 2)	// 

#define STARS_MAX					(5)		// STARSの数

#define TEXTURE_BUTTON_WIDTH		(500.0f / 3)		//
#define TEXTURE_BUTTON_HEIGHT		(115.0f / 3)		// 

#define RESET_WARNING_BUTTON_MAX	(2)

#define TEXTURE_NUM_WIDTH			(15.0f)	
#define TEXTURE_NUM_HEIGHT			(30.0f)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawScoresStars(void);
void DrawScoresNumbers(void);

void ResetScores(void);
void DrawResetScoreWarning(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[SCORE_TEX_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[SCORE_TEX_MAX] = {
	"data/TEXTURE/Menu/UIbgH.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Scores_Title.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Reset.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Cancel.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/ResetScoreWarning.png",
	"data/TEXTURE/Menu/MenuButtons/Records_Num.png",
	"data/TEXTURE/Menu/Numbers2.png",
	"data/TEXTURE/Player/Star.png",
	"data/TEXTURE/Player/StarGrey.png",
};

static RESET_WARNING_BUTTON g_ResetWarningButton[RESET_WARNING_BUTTON_MAX];
static SCORE				g_Score[SCORE_MAX];					// スコア

static BOOL					g_WarningMessage;

static XMFLOAT3				g_ButtonsPivot;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
{
	switch (GetLanguage())
	{
	case LANG_JPN:
		g_TexturName[1] = "data/TEXTURE/Menu/MenuButtons/Japanese/Scores_Title.png";
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/Japanese/Reset.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/Japanese/Cancel.png";
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/Japanese/ResetScoreWarning.png";
		break;

	case LANG_USA:
		g_TexturName[1] = "data/TEXTURE/Menu/MenuButtons/English/Scores_Title.png";
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/English/Reset.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/English/Cancel.png";
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/English/ResetScoreWarning.png";
		break;

	case LANG_ESP:
		g_TexturName[1] = "data/TEXTURE/Menu/MenuButtons/Spanish/Scores_Title.png";
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/Spanish/Reset.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/Spanish/Cancel.png";
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/Spanish/ResetScoreWarning.png";
		break;
	}


	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < SCORE_TEX_MAX; i++)
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


	g_ButtonsPivot = GetMainMenuButtonPos();

	if (g_Score[0].stars == NULL && g_Score[0].time == NULL)
	{
		for (int i = 0; i < SCORE_MAX; i++)
		{
			g_Score[i].stars = 0;
			g_Score[i].time = 0;
		}
	}

	for (int i = 0; i < RESET_WARNING_BUTTON_MAX; i++)
	{
		g_ResetWarningButton[i].pos = XMFLOAT3(SCREEN_CENTER_X - (TEXTURE_BUTTON_WIDTH / 2) + (i * TEXTURE_BUTTON_WIDTH), SCREEN_CENTER_Y + (SCREEN_CENTER_Y / 2) - 75.0f, 0.0f);
		g_ResetWarningButton[i].w = TEXTURE_BUTTON_WIDTH;
		g_ResetWarningButton[i].h = TEXTURE_BUTTON_HEIGHT;
		g_ResetWarningButton[i].texNo = SCORE_TEX_RESET + i;
		g_ResetWarningButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	g_ResetWarningButton[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);


	g_WarningMessage = FALSE;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitScore(void)
{
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < SCORE_TEX_MAX; i++)
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
void UpdateScore(void)
{
	if (g_WarningMessage)
	{
		if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_RIGHT))
		{
			PlaySound(SOUND_LABEL_SE_ChangeButton);
			g_ResetWarningButton[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_ResetWarningButton[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		else if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_LEFT))
		{
			PlaySound(SOUND_LABEL_SE_ChangeButton);
			g_ResetWarningButton[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_ResetWarningButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		}
	}



	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
	{
		PlaySound(SOUND_LABEL_SE_PulseButtonAccept);

		if (g_WarningMessage) 
		{

			if (g_ResetWarningButton[0].color.x == 0.0f)
			{
				ResetScores();
				SaveData();
				//SetFade(FADE_OUT, MODE_TITLE);
				SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_MENU);
			}
			else
			{
				g_WarningMessage = FALSE;
			}
		}
		else
		{
			g_WarningMessage = TRUE;
		}
	}


	if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_B))
	{
		PlaySound(SOUND_LABEL_SE_PulseButtonBack);

		if (g_WarningMessage)
		{
			g_WarningMessage = FALSE;
		}
		else
		{
			BackToMainMenu();
		}
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScore(void)
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


	//Score Title  
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_SCORE_TITLE]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_ButtonsPivot.x, g_ButtonsPivot.y - 70.0f, TEXTURE_BUTTON_WIDTH, TEXTURE_BUTTON_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	//Reset Score Button  
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_RESET]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, g_ButtonsPivot.x, g_ButtonsPivot.y + 275.0f, TEXTURE_BUTTON_WIDTH, TEXTURE_BUTTON_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	//Score num  
	{
		for (int i = 0; i < SCORE_MAX; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_SCORE_NUM]);

		
			float pw = 30.0f;							// ボタンの表示幅
			float ph = 30.0f;							// ボタンの表示高さ
			float px = g_ButtonsPivot.x - 140.0f;	// ボタンの表示位置X
			float py = g_ButtonsPivot.y + (i * 25 * 2);	// ボタンの表示位置Y


			float tw = 1.0f / SCORE_MAX;	// テクスチャの幅
			float th = 1.0f;
			float tx = i * tw;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標


			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
	

	
	DrawScoresStars();
	DrawScoresNumbers();


	if (g_WarningMessage) 
	{
		DrawResetScoreWarning();
	}
}



void DrawScoresStars(void)
{
	for(int i = 0; i < SCORE_MAX; i++)
	{
		for (int j = 0; j < STARS_MAX; j++)
		{
			if (g_Score[i].stars > j)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_STAR]);
			}
			else
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_STAR_GREY]);
			}


			//プレイヤーの位置やテクスチャー座標を反映
			float pw = 25.0f;							// ボタンの表示幅
			float ph = 25.0f;							// ボタンの表示高さ
			float px = g_ButtonsPivot.x - 80.0f + (j * pw);	// ボタンの表示位置X
			float py = g_ButtonsPivot.y + (i * ph * 2);	// ボタンの表示位置Y


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
}


void DrawScoresNumbers(void)
{
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_NUMBERS]);

	for (int i = 0; i < SCORE_MAX; i++)
	{
		// 桁数分処理する
		int number = g_Score[i].time;

		for (int j = 0; j < 5; j++)
		{
			// 今回表示する桁の数字
			float x = (float)(number % 10);

			if (j == 2) x = 10.0f; //「：」を書く時


			// スコアの位置やテクスチャー座標を反映
			float pw = TEXTURE_NUM_WIDTH;			// スコアの表示幅
			float ph = TEXTURE_NUM_HEIGHT;			// スコアの表示高さ
			float px = g_ButtonsPivot.x + 140.0f - (j * pw);	// スコアの表示位置X
			float py = g_ButtonsPivot.y + (i * 25 * 2);;			// スコアの表示位置Y

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
}


void DrawResetScoreWarning(void)
{
	//Message BG  
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_BG]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//Message
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_RESET_WARNING]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	//ボタンの描画
	{
		for (int i = 0; i < RESET_WARNING_BUTTON_MAX; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ResetWarningButton[i].texNo]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_ResetWarningButton[i].pos.x;			// ボタンの表示位置X
			float py = g_ResetWarningButton[i].pos.y;			// ボタンの表示位置Y
			float pw = g_ResetWarningButton[i].w;		// ボタンの表示幅
			float ph = g_ResetWarningButton[i].h;		// ボタンの表示高さ


			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標
			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;


			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_ResetWarningButton[i].color);
			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


SCORE* GetScore(void)
{
	return &g_Score[0];
}


void SetScore(int stars, int time)
{
	for (int i = 0; i < SCORE_MAX; i++)
	{
		if (stars > g_Score[i].stars)	//今回のスターの数が多かったら保存する
		{
			for (int j = SCORE_MAX-1; j > i; j--)
			{
				g_Score[j].stars = g_Score[j - 1].stars;
				g_Score[j].time = g_Score[j - 1].time;
			}
			g_Score[i].stars = stars;
			g_Score[i].time = time;

			if (GetRetryRound() == 0) SaveData();

			return;
		}
		else if(stars == g_Score[i].stars)	//スターの数が同じ
		{
			if (time <= g_Score[i].time)		//今回の時間が速かったら保存する
			{
				for (int j = SCORE_MAX - 1; j > i; j--)
				{
					g_Score[j].stars = g_Score[j - 1].stars;
					g_Score[j].time = g_Score[j - 1].time;
				}
				g_Score[i].stars = stars;
				g_Score[i].time = time;

				if (GetRetryRound() == 0) SaveData();

				return;
			}
		}
	}
}


void ResetScores(void)
{
	for (int i = 0; i < SCORE_MAX; i++)
	{
		g_Score[i].stars = 0;
		g_Score[i].time = 0;
	}
}