//=============================================================================
//
// タイトル画面処理 [options.h]
// Author : GARCIA DANIEL
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

#define MENU_BUTTON_MAX		(4)

enum
{
	BUTTON_SOUND_SETTINGS,
	BUTTON_CONTROLS,
	BUTTON_LANGUAGE,

	OPTIONS_BUTTON_MAX,
};

enum
{
	BUTTON_SE_VOLUME,
	BUTTON_BGM_VOLUME,

	SOUND_SETTINGS_BUTTON_MAX,
};

enum
{
	BUTTON_KEYBOARD,
	BUTTON_CONTROLLER,

	CONTROLS_SETTINGS_BUTTON_MAX,
};


enum
{
	BUTTON_JAPANESE,
	BUTTON_ENGLISH,
	BUTTON_SPANISH,

	LANGUAGE_SETTINGS_BUTTON_MAX,
};

enum
{
	OPTION_SCREEN_OPTIONS,
	OPTION_SCREEN_SOUND_SETTINGS,
	OPTION_SCREEN_CONTROLS,
	OPTION_SCREEN_LANGUAGE,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct OPTIONS_BUTTON 
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
HRESULT InitOptions(void);
void UninitOptions(void);
void UpdateOptions(void);
void DrawOptions(void);


