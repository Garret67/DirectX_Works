//=============================================================================
//
// �^�C�g����ʏ��� [menu.h]
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
	BUTTON_COLISEUM,
	BUTTON_TUTORIAL,
	BUTTON_SCORES,
	BUTTON_OPTIONS,
	BUTTON_QUIT_GAME,

	BUTTON_MAX
};

enum
{
	BUTTON_COLISEUM_FROM_START,
	BUTTON_COLISEUM_ROUND_SELECTOR,

	BUTTON_COLISEUM_MAX
};

enum
{
	SCREEN_MAIN_MENU,
	SCREEN_COLISEUM,
	SCREEN_OPTIONS,
	SCREEN_SCORES,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct MENU_BUTTON 
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

struct MENU_PARTICLE
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


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMenu(void);
void UninitMenu(void);
void UpdateMenu(void);
void DrawMenu(void);

XMFLOAT3 GetMainMenuButtonPos(void);

void BackToMainMenu(void);
