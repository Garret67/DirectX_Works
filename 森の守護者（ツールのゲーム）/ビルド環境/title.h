//=============================================================================
//
// �^�C�g����ʏ��� [title.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"


//*****************************************************************************
// �}�N����`
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
// �\���̒�`
//*****************************************************************************

struct TITLE_PARTICLE
{
	BOOL		use;			// true�����Ă���   false:����ł���
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�

	float		moveSpeed;		// �ړ����x
	XMFLOAT3	moveDir;		// �ړ����x

	XMFLOAT4	color;

	int			lifeTime;
	int			lifeTimeCnt;
	float		alphaAdd;
};


struct BUTTON_MESSAGE
{
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���
	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;			// �e�N�X�`���ԍ�

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTitle(void);
void UninitTitle(void);
void UpdateTitle(void);
void DrawTitle(void);

XMFLOAT3 GetMainMenuButtonPos(void);

void BackToMainMenu(void);
