//=============================================================================
//
// リザルト画面処理 [screenGameOver.h]
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
	GAMEOVER_TEX_BG,
	GAMEOVER_TEX_GAMEOVER_TITLE,
	GAMEOVER_TEX_RETRY_ROUND_BUTTON,
	GAMEOVER_TEX_RETRY_COLISEUM_BUTTON,
	GAMEOVER_TEX_MAINMENU_BUTTON,
	GAMEOVER_TEX_RETRY_ROUND_MESSAGE,
	GAMEOVER_TEX_RETRY_COLISEUM_MESSAGE,
	GAMEOVER_TEX_MAINMENU_MESSAGE,
	GAMEOVER_TEX_DEAD_PLAYER,

	GAMEOVER_TEX_MAX,
};

enum {
	BUTTON_RETRY_ROUND,
	BUTTON_RETRY_COLISEUM,
	BUTTON_BACK_TO_MENU,

	GAMEOVER_BUTTON_MAX,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct GAMEOVER_BUTTON
{
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号
	XMFLOAT4	color;			// ボタンの色
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitScreenGameOver(void);
void UninitScreenGameOver(void);
void UpdateScreenGameOver(void);
void DrawScreenGameOver(void);

void SetScreenGameOver(void);
