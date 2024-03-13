//=============================================================================
//
// �X�R�A���� [score.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCORE_MAX			(5)		// �X�R�A�̍ő�l
#define SCORE_DIGIT			(5)			// ����

enum
{
	SCORE_TEX_BG,
	SCORE_TEX_SCORE_TITLE,
	SCORE_TEX_RESET,
	SCORE_TEX_CANCEL,
	SCORE_TEX_RESET_WARNING,
	SCORE_TEX_SCORE_NUM,
	SCORE_TEX_NUMBERS,
	SCORE_TEX_STAR,
	SCORE_TEX_STAR_GREY,

	SCORE_TEX_MAX,
};

struct RESET_WARNING_BUTTON
{
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�
	XMFLOAT4	color;			// �{�^���̐F
};

struct SCORE 
{
	int stars;
	int time;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);


SCORE* GetScore(void);
void SetScore(int stars, int time);


