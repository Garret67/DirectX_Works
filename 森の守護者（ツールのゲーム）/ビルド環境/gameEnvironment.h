//=============================================================================
//
// 木処理 [tree.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#define	GROUND_WIDTH			(1000.0f)			// 頂点サイズ
#define	GROUND_HEIGHT			(1000.0f)			// 頂点サイズ

#define	MAX_TREE				(80)			// 木最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************

typedef struct
{
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	rot;			// 回転

	float		fWidth;			// 幅
	float		fHeight;		// 高さ

	MATERIAL	material;		// マテリアル
	int			nIdxShadow;		// 影ID
	BOOL		use;			// 使用しているかどうか

} BILLBOARD;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGameEnvironment(void);
void UninitGameEnvironment(void);
void UpdateGameEnvironment(void);
void DrawGameEnvironment(void);

BILLBOARD* GetTree(void);
