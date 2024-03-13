//=============================================================================
//
// リザルト画面処理 [result.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

enum {
	RESULT_TEX_BG,
	RESULT_TEX_NUMBER,
	RESULT_TEX_STAR,
	RESULT_TEX_STARGREY,
	RESULT_TEX_RESULT_TITLE,
	RESULT_TEX_RETRY_BUTTON,
	RESULT_TEX_MAINMENU_BUTTON,

	RESULT_TEX_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct RESULT_BUTTON
{
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号
	XMFLOAT4	color;			// ボタンの色
};

struct STAR
{
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号
	float		scl;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitResult(void);
void UninitResult(void);
void UpdateResult(void);
void DrawResult(void);

void SetResultScreen(void);
void ResultHit(void);
