//=============================================================================
//
// エネミー処理 [platforms.h]
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
#define GROUND_MAX				(10)	// GROUNDのMax数
#define PLATFORM_MAX			(5)		// PLATFORMSのMax数
#define SPIKES_MAX				(5)		// SPIKESのMax数


//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct PLATFORM
{
	BOOL		use;			// 有効?
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号

};

struct PLATFORM_ANIM
{
	BOOL		use;
	BOOL		collider;
	XMFLOAT3	pos;			// ポリゴンの座標
	float		movePosHeight;	// ポリゴンのさいごのY座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号
	int			currentSprite;			// テクスチャ番号
	int			countAnim;			// テクスチャ番号
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlatforms(void);
void UninitPlatforms(void);
void UpdatePlatforms(void);
void DrawPlatforms(void);


PLATFORM* GetGround(void);
PLATFORM* GetSpikes(void);
PLATFORM_ANIM* GetPlatforms(void);

void SetPlatform(XMFLOAT3 FinalPos);
void ResetPlatform(void);

