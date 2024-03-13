//=============================================================================
//
// ���U���g��ʏ��� [result.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
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
// �\���̒�`
//*****************************************************************************
struct RESULT_BUTTON
{
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�
	XMFLOAT4	color;			// �{�^���̐F
};

struct STAR
{
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�
	float		scl;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitResult(void);
void UninitResult(void);
void UpdateResult(void);
void DrawResult(void);

void SetResultScreen(void);
void ResultHit(void);
