//=============================================================================
//
// �^�C�g����ʏ��� [pauseMenu.h]
// Author : 
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
	PAUSE_BUTTON_NEW_GAME,
	PAUSE_BUTTON_OPTIONS,
	PAUSE_BUTTON_QUIT_GAME,

	PAUSE_BUTTON_MAX
};

enum
{
	PAUSE_SCREEN_MAIN,
	PAUSE_SCREEN_OPTIONS,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PAUSE_MENU_BUTTON 
{
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���
	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;			// �e�N�X�`���ԍ�

	XMFLOAT4	color;			// �{�^���̐F
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPauseMenu(void);
void UninitPauseMenu(void);
void UpdatePauseMenu(void);
void DrawPauseMenu(void);

XMFLOAT3 GetPauseMenuButtonPos(void);

void BackToPauseMenu(void);