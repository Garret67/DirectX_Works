//=============================================================================
//
// �^�C�g����ʏ��� [options.h]
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

#define MENU_BUTTON_MAX		(4)

enum
{
	BUTTON_SOUND_SETTINGS,
	BUTTON_CONTROLS,
	BUTTON_LANGUAGE,

	OPTIONS_BUTTON_MAX,
};

enum
{
	BUTTON_SE_VOLUME,
	BUTTON_BGM_VOLUME,

	SOUND_SETTINGS_BUTTON_MAX,
};

enum
{
	BUTTON_KEYBOARD,
	BUTTON_CONTROLLER,

	CONTROLS_SETTINGS_BUTTON_MAX,
};


enum
{
	BUTTON_JAPANESE,
	BUTTON_ENGLISH,
	BUTTON_SPANISH,

	LANGUAGE_SETTINGS_BUTTON_MAX,
};

enum
{
	OPTION_SCREEN_OPTIONS,
	OPTION_SCREEN_SOUND_SETTINGS,
	OPTION_SCREEN_CONTROLS,
	OPTION_SCREEN_LANGUAGE,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct OPTIONS_BUTTON 
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
HRESULT InitOptions(void);
void UninitOptions(void);
void UpdateOptions(void);
void DrawOptions(void);


