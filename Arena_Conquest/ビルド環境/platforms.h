//=============================================================================
//
// �G�l�~�[���� [platforms.h]
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
#define GROUND_MAX				(10)	// GROUND��Max��
#define PLATFORM_MAX			(5)		// PLATFORMS��Max��
#define SPIKES_MAX				(5)		// SPIKES��Max��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct PLATFORM
{
	BOOL		use;			// �L��?
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�

};

struct PLATFORM_ANIM
{
	BOOL		use;
	BOOL		collider;
	XMFLOAT3	pos;			// �|���S���̍��W
	float		movePosHeight;	// �|���S���̂�������Y���W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�
	int			currentSprite;			// �e�N�X�`���ԍ�
	int			countAnim;			// �e�N�X�`���ԍ�
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlatforms(void);
void UninitPlatforms(void);
void UpdatePlatforms(void);
void DrawPlatforms(void);


PLATFORM* GetGround(void);
PLATFORM* GetSpikes(void);
PLATFORM_ANIM* GetPlatforms(void);

void SetPlatform(XMFLOAT3 FinalPos);
void ResetPlatform(void);

