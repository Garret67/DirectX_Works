//=============================================================================
//
// タイトル画面処理 [title.h]
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
	TITLE_TEX_BG,
	TITLE_TEX_TITLE_NAME,
	TITLE_TEX_PARTICLES,
	TITLE_TEX_BUTTON_MESSAGE,

	TITLE_TEX_MAX,
};


//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct TITLE_PARTICLE
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


struct BUTTON_MESSAGE
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTitle(void);
void UninitTitle(void);
void UpdateTitle(void);
void DrawTitle(void);

XMFLOAT3 GetMainMenuButtonPos(void);

void BackToMainMenu(void);
