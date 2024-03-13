//=============================================================================
//
// スコア処理 [score.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************

enum {
	UI_TEX_HEALTH,
	UI_TEX_ENERGY,
	UI_TEX_ROUND_BG,
	UI_TEX_ROUND_1,
	UI_TEX_ROUND_2,
	UI_TEX_ROUND_3,
	UI_TEX_ROUND_4,
	UI_TEX_ROUND_5,
	UI_TEX_ROUND_6,
	UI_TEX_ROUND_7,
	UI_TEX_ROUND_8,
	UI_TEX_ROUND_9,
	UI_TEX_ROUND_10,

	UI_TEX_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct HEALTH_UI
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	float		w, h;			// 幅と高さ
	int			texNo;

	int			lifes;			// 線形補間用
	int			lifesMax;		// 行動データのテーブル番号
	BOOL		active;
};


struct ENERGY_UI
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	float		w, h;			// 幅と高さ
	int			texNo;

	int			energy;			// 線形補間用
	int			energyMax;		// 行動データのテーブル番号
	BOOL		active;
};


struct ROUND_MESSAGE
{
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayerUI(void);
void UninitPlayerUI(void);
void UpdatePlayerUI(void);
void DrawPlayerUI(void);

void UpdateLifesUI(void);
void UpdateEnergyUI(void);

void ShowRoundMsg(int round);
