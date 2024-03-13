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
#define SCORE_MAX			(5)		// スコアの最大値
#define SCORE_DIGIT			(5)			// 桁数

enum
{
	SCORE_TEX_BG,
	SCORE_TEX_SCORE_TITLE,
	SCORE_TEX_RESET,
	SCORE_TEX_CANCEL,
	SCORE_TEX_RESET_WARNING,
	SCORE_TEX_SCORE_NUM,
	SCORE_TEX_NUMBERS,
	SCORE_TEX_STAR,
	SCORE_TEX_STAR_GREY,

	SCORE_TEX_MAX,
};

struct RESET_WARNING_BUTTON
{
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号
	XMFLOAT4	color;			// ボタンの色
};

struct SCORE 
{
	int stars;
	int time;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);


SCORE* GetScore(void);
void SetScore(int stars, int time);


