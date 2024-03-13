//=============================================================================
//
// �^�C�g����ʏ��� [interface.h]
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
//#define ANIMATION_ZONE_BUTTON_MAX (6)
enum
{
	INTERFACE_TEX_RECTANGLE,
	INTERFACE_TEX_TITLE_BG,
	INTERFACE_TEX_OVALO,
	
	INTERFACE_TEX_MAX,
};


enum
{
	BUTTON_PLAY,
	BUTTON_LOOP,
	
	BUTTON_SPEED,
	BUTTON_SPEED_PLUS,
	BUTTON_SPEED_MINUS,

	BUTTON_TRANSITION_FRAMES,
	BUTTON_TRANSITION_FRAMES_PLUS,
	BUTTON_TRANSITION_FRAMES_MINUS,

	BUTTON_NEXT_ANIM,
	BUTTON_NEXT_ANIM_PLUS,
	BUTTON_NEXT_ANIM_MINUS,

	ANIMATION_ZONE_BUTTON_MAX,
};

enum
{
	BUTTON_ADD_ANIMATION,
	BUTTON_DELETE_ANIMATION,

};


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct BUTTON
{
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�
	BOOL		selected;

	WCHAR		text[256];
	XMFLOAT4	color;

};

struct HIDE_OBJ_BUTTON
{
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�

	WCHAR		text[256];
	XMFLOAT4	color;

	BUTTON		button[2];
};


struct ANIMATION_ZONE
{
	BOOL		use;			// true:�g���Ă���  false:���g�p
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�

	BOOL		play;
	BOOL		loop;
	float		speed;

	BUTTON		AnimSelectButton;
	BUTTON		button[ANIMATION_ZONE_BUTTON_MAX];
};


struct ADD_DEL_ZONE
{
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�

	BUTTON		button[2];
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitInterface(void);
void UninitInterface(void);
void UpdateInterface(void);
void DrawInterface(void);

void UpdateNewModelInterface(int AnimationNum, char* modelName);