//=============================================================================
//
// モデル処理 [player.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER				(1)			// プレイヤーの数

#define	MODEL_ANIMATION_MAX	(20)				// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct FBXPLAYER
{
	XMFLOAT3		pos;		// ポリゴンの位置
	XMFLOAT3		rot;		// ポリゴンの向き(回転)
	XMFLOAT3		scl;		// ポリゴンの大きさ(スケール)

	XMFLOAT4X4		mtxWorld;	// ワールドマトリックス

	bool			load;

	DX11_MODEL		model;		// モデル情報
	MODEL_DATA		modelData;
	FBX_ANIMATOR	Animator;

	float			spd;		// 移動スピード
	float			dir;		// 向き

	bool			use;

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

FBXPLAYER* GetPlayer(void);

void LoadModelButton(WCHAR* text);
void LoadAnimationButton(WCHAR* animText, WCHAR* transitionFramesText, WCHAR* nextAnimationtext, int animNum, int activeAnimNum);

BOOL LoopButton(int AnimNum);
void PlayButton(int AnimNum);
void SpeedButton(int AnimNum, float speed);

void DeleteAnimation(int animNum, int g_ActiveAnimNum);

void SaveModelAnimation(int activeAnimNum);
void LoadModelDataBinary(void);


int GetCurAnimation(void);

void UpdateAnimationZone(int showingAnim, int activeAnimNum, BOOL& loop, float& speed, WCHAR* animText, WCHAR* transitionFramesText, WCHAR* nextAnimationtext);

void HideObjButton(int ButtonIdx, WCHAR* text, int sumando);

void TransitionFramesButton(int AnimNum, WCHAR* text, float sumando);
void NextAnimationButton(int AnimNum, int ActiveAnimNum, WCHAR* text, int sumando);