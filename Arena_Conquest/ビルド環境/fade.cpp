//=============================================================================
//
// フェード処理 [fade.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "sound.h"
#include "bg.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(7)				// テクスチャの数

#define	FADE_RATE					(0.02f)			// フェード係数

#define DOOR_MAX					(4)
#define DOOR_MOVE_SPEED				(4.0f)

#define	QUARTER_SCREEN_X 			(SCREEN_WIDTH / 4)
#define	QUARTER_SCREEN_Y 			(SCREEN_HEIGHT / 4)

#define	TRANSITION_TIME 			(60.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
double easeInOut(double t);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Effect/fade_black.png",
	"data/TEXTURE/Effect/fade_white.png",
	"data/TEXTURE/Effect/MarcoTransition0.png",
	"data/TEXTURE/Effect/MarcoTransition1.png",
	"data/TEXTURE/Effect/MarcoTransition2.png",
	"data/TEXTURE/Effect/MarcoTransition3.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

FADE							g_Fade = FADE_IN;						// フェードの状態
int								g_ModeNext;					// 次のモード
XMFLOAT4						g_Color;					// フェードのカラー（α値）
float							g_fTime;					// フェード時間

DOOR							g_Door[DOOR_MAX];
int								g_DoorMove;
int								g_Transition;

float							g_TransitionTime;
float							g_TransitionTimeCnt;

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFade(void)
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


	// 初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 0.0f, 0.0f, 0.0f };
	g_TexNo = 1;
	g_fTime = 0.02f;

	g_Color = { 0.0, 0.0, 0.0, 1.0 };

	g_Door[0].pos = XMFLOAT3(-QUARTER_SCREEN_X,				  QUARTER_SCREEN_Y,			0.0f);
	g_Door[1].pos = XMFLOAT3(SCREEN_WIDTH + QUARTER_SCREEN_X, QUARTER_SCREEN_Y,			0.0f);
	g_Door[2].pos = XMFLOAT3(-QUARTER_SCREEN_X,				  QUARTER_SCREEN_Y * 3.0f,  0.0f);
	g_Door[3].pos = XMFLOAT3(SCREEN_WIDTH + QUARTER_SCREEN_X, QUARTER_SCREEN_Y * 3.0f,  0.0f);

	for (int i = 0; i < DOOR_MAX; i++)
	{
		g_Door[i].w = SCREEN_WIDTH / 2;
		g_Door[i].h = SCREEN_HEIGHT / 2;
		g_Door[i].texNo = 2 + i;
	}
	g_DoorMove	 = DOOR_HORIZONTAL_MOVE;
	g_Transition = TRANSITION_DOOR_NONE;


	g_TransitionTime = TRANSITION_TIME;
	g_TransitionTimeCnt = 0.0f;

	g_ModeNext = GetMode();


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFade(void)
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
void UpdateFade(void)
{

	if (g_Fade != FADE_NONE)
	{// フェード処理中
		if (g_Fade == FADE_OUT)
		{// フェードアウト処理
			g_Color.w += g_fTime;		// α値を加算して画面を消していく
			float volume = GetVolumeBGM() - g_Color.w;
			if (volume > 0)
			{
				SetVolumeSound(GetCurrentBGM(), volume);
			}
			else 
			{
				SetVolumeSound(GetCurrentBGM(), 0.0f);
			}
			
			
			
			if (g_Color.w >= 1.0f)
			{
				// 鳴っている曲を全部止める
				StopSound();

				// フェードイン処理に切り替え
				g_Color.w = 1.0f;
				SetFade(FADE_IN, g_ModeNext);

				// モードを設定
				SetMode(g_ModeNext);
			}

		}
		else if (g_Fade == FADE_IN)
		{// フェードイン処理
			g_Color.w -= g_fTime;		// α値を減算して画面を浮き上がらせる
			float volume = 1.0f - g_Color.w;

			if (volume < GetVolumeBGM())
			{
				SetVolumeSound(volume);
			}
			else
			{
				SetVolumeSound(GetVolumeBGM());
			}
			
			if (g_Color.w <= 0.0f)
			{
				// フェード処理終了
				g_Color.w = 0.0f;
				SetFade(FADE_NONE, g_ModeNext);
			}

		}
	}



	if (g_Transition != TRANSITION_DOOR_NONE)
	{
		double t = g_TransitionTimeCnt / g_TransitionTime;
		t = easeInOut(t);
		

		if (g_Transition == TRANSITION_DOOR_CLOSE)
		{

			float volume =  1.0f - (g_TransitionTimeCnt / g_TransitionTime);
			if (volume < GetVolumeBGM())
			{
				SetVolumeSound(GetCurrentBGM(), volume);
			}


			if (g_DoorMove == DOOR_HORIZONTAL_MOVE)
			{
				for (int i = 0; i < DOOR_MAX; i++)
				{
					if (g_Door[i].pos.x < QUARTER_SCREEN_X)
					{
						g_Door[i].pos.x = (float)((double)-QUARTER_SCREEN_X + ((double)QUARTER_SCREEN_X - (double)-QUARTER_SCREEN_X) * t);
						//g_Door[i].pos.x += DOOR_MOVE_SPEED;
					}
					else
					{
						g_Door[i].pos.x = (float)((double)(SCREEN_WIDTH + QUARTER_SCREEN_X) + ((double)(QUARTER_SCREEN_X * 3.0f) - (double)(SCREEN_WIDTH + QUARTER_SCREEN_X)) * t);
						//g_Door[i].pos.x -= DOOR_MOVE_SPEED;
					}

				}

				if (g_TransitionTimeCnt == g_TransitionTime)
				{
					// 鳴っている曲を全部止める
					StopSound();

					// フェードイン処理に切り替え
					//g_Color.w = 1.0f;
					SetTransitionDoor(TRANSITION_DOOR_OPEN, g_ModeNext);
					g_DoorMove = (g_DoorMove + 1) % DOOR_MOVE_MAX;

					// モードを設定
					SetMode(g_ModeNext);

					g_TransitionTimeCnt = 0;
				}
			}
			else if (g_DoorMove == DOOR_VERTICAL_MOVE)
			{
				for (int i = 0; i < DOOR_MAX; i++)
				{
					if (g_Door[i].pos.y < SCREEN_CENTER_Y)
					{
						g_Door[i].pos.y = (float)((double)-QUARTER_SCREEN_Y + ((double)QUARTER_SCREEN_Y - (double)-QUARTER_SCREEN_Y) * t);
						//g_Door[i].pos.y += DOOR_MOVE_SPEED;
					}
					else
					{
						g_Door[i].pos.y = (float)((double)(SCREEN_HEIGHT + QUARTER_SCREEN_Y) + ((double)(QUARTER_SCREEN_Y * 3.0f) - (double)(SCREEN_HEIGHT + QUARTER_SCREEN_Y)) * t);
						g_Door[i].pos.y -= DOOR_MOVE_SPEED;
					}
				}

				if (g_TransitionTimeCnt == g_TransitionTime)
				{
					// 鳴っている曲を全部止める
					StopSound();

					// Transition処理を切り替え
					//g_Color.w = 1.0f;
					SetTransitionDoor(TRANSITION_DOOR_OPEN, g_ModeNext);
					g_DoorMove = (g_DoorMove + 1) % DOOR_MOVE_MAX;

					// モードを設定
					SetMode(g_ModeNext);

					g_TransitionTimeCnt = 0;
				}

			}

		}
		else if (g_Transition == TRANSITION_DOOR_OPEN)
		{

			float volume = (g_TransitionTimeCnt / g_TransitionTime);
			if (volume < GetVolumeBGM())
			{
				SetVolumeSound(GetCurrentBGM(), volume);
			}


			if (g_DoorMove == DOOR_HORIZONTAL_MOVE)
			{
				for (int i = 0; i < DOOR_MAX; i++)
				{
					if (g_Door[i].pos.x < SCREEN_CENTER_X)
					{
						g_Door[i].pos.x = (float)((double)QUARTER_SCREEN_X + ((double)-QUARTER_SCREEN_X - (double)QUARTER_SCREEN_X) * t);
						g_Door[i].pos.x -= DOOR_MOVE_SPEED;		// α値を加算して画面を消していく
					}
					else
					{
						g_Door[i].pos.x = (float)((double)(QUARTER_SCREEN_X * 3.0f) + ((double)(SCREEN_WIDTH + QUARTER_SCREEN_X) - (double)(QUARTER_SCREEN_X * 3.0f)) * t);
						g_Door[i].pos.x += DOOR_MOVE_SPEED;
					}
				}

				if (g_TransitionTimeCnt == g_TransitionTime)
				{
					//g_Color.w = 1.0f;
					SetTransitionDoor(TRANSITION_DOOR_NONE, g_ModeNext);
					g_TransitionTimeCnt = 0;
				}
			}
			else if(g_DoorMove == DOOR_VERTICAL_MOVE)
			{
				for (int i = 0; i < DOOR_MAX; i++)
				{
					if (g_Door[i].pos.y < SCREEN_CENTER_Y)
					{
						g_Door[i].pos.y = (float)((double)QUARTER_SCREEN_Y + ((double)(-QUARTER_SCREEN_Y) - (double)QUARTER_SCREEN_Y) * t);
						g_Door[i].pos.y -= DOOR_MOVE_SPEED;		// α値を加算して画面を消していく
					}
					else
					{
						g_Door[i].pos.y = (float)((double)(QUARTER_SCREEN_Y * 3.0f) + ((double)(SCREEN_HEIGHT + QUARTER_SCREEN_Y) - (double)(QUARTER_SCREEN_Y * 3.0f)) * t);
						g_Door[i].pos.y += DOOR_MOVE_SPEED;
					}
				}

				if (g_TransitionTimeCnt == g_TransitionTime)
				{
					//g_Color.w = 1.0f;
					SetTransitionDoor(TRANSITION_DOOR_NONE, g_ModeNext);
					g_TransitionTimeCnt = 0;
				}
			}
		}

		g_TransitionTimeCnt++;
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	// PrintDebugProc("\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFade(void)
{
	// 加算合成に設定
//SetBlendState(BLEND_MODE_ADD);

// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	if (g_Fade != FADE_NONE) 	// フェードしないのなら描画しない
	{

		// FADEを描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			//SetVertex(0.0f, 0.0f, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f);
			SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH / 2, TEXTURE_WIDTH / 2, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f,
				g_Color);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
	
	if (g_Transition != TRANSITION_DOOR_NONE)
	{
		for (int i = 0; i < DOOR_MAX; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Door[i].texNo]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, g_Door[i].pos.x, g_Door[i].pos.y, g_Door[i].w, g_Door[i].h, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// フェードの状態設定
//=============================================================================
void SetFade(FADE fade, int modeNext)
{
	g_Fade = fade;
	if (fade == FADE_OUT)
	{
		g_Color = { 0.0f, 0.0f, 0.0f, 0.0f };
		g_fTime = 0.02f;
	}
	
	g_ModeNext = modeNext;
}

//=============================================================================
// フェードの状態設定
//=============================================================================
void SetFade2(FADE fade, int modeNext, float seconds, XMFLOAT4 color)
{
	g_Fade		= fade;
	g_Color		= color;
	g_fTime		= 1.0f / 60.0f / seconds;
	g_ModeNext  = modeNext;
}


//=============================================================================
// TRANSITIONの状態設定
//=============================================================================
void SetTransitionDoor(int transition, int nextMode)
{
	g_Transition = transition;
	g_ModeNext = nextMode;
}


double easeInOut(double t)
{
	return t < 0.5 ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2;
}