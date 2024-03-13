//=============================================================================
//
// �o���b�g���� [bullet.h]
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

enum
{
	BULLET_TEX_FIRE_BULLET,
	BULLET_TEX_COLLIDER,
	BULLET_TEX_BULLET_EXPLOSION,

	BULLET_TEX_MAX,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

// �e�̍\����
struct BULLET
{
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���
	XMFLOAT3	pos;			// �o���b�g�̍��W
	XMFLOAT3	rot;			// �o���b�g�̉�]��
	XMFLOAT3	move;			// �o���b�g�̈ړ���
	int			countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;			// ���Ԗڂ̃e�N�X�`���[���g�p����̂�

	XMFLOAT4	color;			// ���Ԗڂ̃e�N�X�`���[���g�p����̂�

};

// �e�̔����\����
struct EXPLOSION
{
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���
	XMFLOAT3	pos;			// �o���b�g�̍��W
	int			countAnim;		// �A�j���[�V�����J�E���g
	int			currentSprite;	// ���̃X�v���C�g
	int			texNo;			// ���Ԗڂ̃e�N�X�`���[���g�p����̂�

	XMFLOAT4	color;			// ���Ԗڂ̃e�N�X�`���[���g�p����̂�

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET *GetBullet(void);		//�e���擾����

void SetBulletPlayer(XMFLOAT3 pos, int direction);						//�v���C���[�̒e�ݒ�
void SetBulletEnemy(XMFLOAT3 pos, float rotation, XMFLOAT3 MoveDir);	//�G�l�~�[�@�̒e�ݒ�

