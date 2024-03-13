//=============================================================================
//
// フェード処理 [fade.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************

// フェードの状態
typedef enum
{
	FADE_NONE = 0,		// 何もない状態
	FADE_IN,			// フェードイン処理
	FADE_OUT,			// フェードアウト処理
	FADE_MAX
} FADE;


enum
{
	TRANSITION_DOOR_NONE,
	TRANSITION_DOOR_OPEN,
	TRANSITION_DOOR_CLOSE,
};


enum 
{
	DOOR_HORIZONTAL_MOVE,
	DOOR_VERTICAL_MOVE,
	DOOR_MOVE_MAX,
};


struct DOOR
{
	XMFLOAT3	pos;		// ポリゴンの座標
	float		w, h;		// 幅と高さ
	int			texNo;		// 使用しているテクスチャ番号
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitFade(void);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);

void SetFade(FADE fade, int modeNext);
void SetFade2(FADE fade, int modeNext, float seconds, XMFLOAT4 color);

void SetTransitionDoor(int transition, int nextMode);
