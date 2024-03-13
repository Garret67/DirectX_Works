//=============================================================================
//
// タイトル画面処理 [pauseMenu.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************

enum
{
	PAUSE_BUTTON_NEW_GAME,
	PAUSE_BUTTON_OPTIONS,
	PAUSE_BUTTON_QUIT_GAME,

	PAUSE_BUTTON_MAX
};

enum
{
	PAUSE_SCREEN_MAIN,
	PAUSE_SCREEN_OPTIONS,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PAUSE_MENU_BUTTON 
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号

	XMFLOAT4	color;			// ボタンの色
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPauseMenu(void);
void UninitPauseMenu(void);
void UpdatePauseMenu(void);
void DrawPauseMenu(void);

XMFLOAT3 GetPauseMenuButtonPos(void);

void BackToPauseMenu(void);