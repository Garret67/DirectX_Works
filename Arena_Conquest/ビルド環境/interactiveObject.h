//=============================================================================
//
// �G�l�~�[���� [interactiveObject.h]
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
#define WOODENSIGN_MAX				(5)		// PLATFORMS��Max��
#define INTERACTIVEOBJ_HIT_HOUSE_MAX	(1)		// PLATFORMS��Max��

enum
{
	TEX_SIGN,
	TEX_DOOR_ROCKS,
	TEX_EXIT_HOUSE,
	TEX_MESSAGE_EXAMINE,
	TEX_MESSAGE_EXIT,
	TEX_EXPLAIN_SIGN,
	TEX_EXPLAIN_ATTACKROCK,
	TEX_EXPLAIN_RUN,
	TEX_EXPLAIN_BULLET,
	TEX_EXPLAIN_DASH,
	TEX_EXPLAIN_FOCUS,
	TEX_EXPLAIN_JUMP,
	TEX_EXPLAIN_ENERGY,

	TEX_MAX,
	
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct INTERACTIVEOBJ
{
	BOOL		active;			// �L��
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�
	int			texAnimSpritesMax;	// �g�p���Ă���e�N�X�`���ԍ�

	int			currentSprite;
	int			countAnim;
	int			animWait;

	int			messageTexNo;
};


struct BREAKABLE_WALL
{
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�
	int			messageTexNo;

	int			HP;
	
	BOOL		invincible;			//���G���
	int			invincibleTime;		//���G�̃t���[����
	int			invincibleTimeCnt;	//���̖��G�̃t���[����
};


struct MESSAGE
{
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�

	BOOL		isActive;
};




//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitInteractiveObj(void);
void UninitInteractiveObj(void);
void UpdateInteractiveObj(void);
void DrawInteractiveObj(void);

BREAKABLE_WALL GetBreakableWall(void);

void BreakableWallHit(void);




