//=============================================================================
//
// タイトル画面処理 [menu.h]
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


enum
{
	BUTTON_COLISEUM,
	BUTTON_TUTORIAL,
	BUTTON_SCORES,
	BUTTON_OPTIONS,
	BUTTON_QUIT_GAME,

	BUTTON_MAX
};

enum
{
	BUTTON_COLISEUM_FROM_START,
	BUTTON_COLISEUM_ROUND_SELECTOR,

	BUTTON_COLISEUM_MAX
};

enum
{
	SCREEN_MAIN_MENU,
	SCREEN_COLISEUM,
	SCREEN_OPTIONS,
	SCREEN_SCORES,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct MENU_BUTTON 
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

struct MENU_PARTICLE
{
	BOOL		use;			// true生きている   false:死んでいる
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号

	float		moveSpeed;		// 移動速度
	XMFLOAT3	moveDir;		// 移動速度

	XMFLOAT4	color;

	int			lifeTime;
	int			lifeTimeCnt;
	float		alphaAdd;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMenu(void);
void UninitMenu(void);
void UpdateMenu(void);
void DrawMenu(void);

XMFLOAT3 GetMainMenuButtonPos(void);

void BackToMainMenu(void);
