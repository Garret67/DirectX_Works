//=============================================================================
//
// �G�l�~�[���� [enemySlimeKing.h]
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
#define SLIME_KING_PARTICLE_MAX	(40)


enum 
{
	SLIME_KING_ANIM_WAKE_UP,
	SLIME_KING_ANIM_PREPARATION,
	SLIME_KING_ANIM_TIRED,
	SLIME_KING_ANIM_DEATH,
	
	SLIME_KING_ANIM_MAX,
};


enum {
	SLIME_KING_DIR_RIGHT,
	SLIME_KING_DIR_LEFT,
};

enum {
	SLIME_KING_ATTACK_HORIZONTAL,
	SLIME_KING_ATTACK_VERTICAL,
};


enum {
	SLIME_KING_STATE_IDLE,
	SLIME_KING_STATE_PREPARATION,
	SLIME_KING_STATE_TIRED,
	SLIME_KING_STATE_RUSH,
};
//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct SLIME_KING
{
	BOOL		spawned;			// true�����Ă���   false:����ł���
	BOOL		alive;			// true�����Ă���   false:����ł���
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	float		w, h;			// ���ƍ���
	int			currentSprite;	// ���̃A�j���[�V�����X�v���C�g
	int			currentAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			countAnim;		// �A�j���[�V�����J�E���g
	int			texNo;			// �e�N�X�`���ԍ�
	float		moveSpeed;		// �ړ����x
	float		moveDir;		// �ړ����x

	float		HP;				// �̗�

	BOOL		damaged;		//�G�l�~�[�����t����
	int			dmgTimeCnt;		

	XMFLOAT4	enemyColor;

	int			attackType;

	int			phase;
	int			portal;
	int			state;
	int			stateTime;
	int			stateTimeCnt;
};


struct SLIME_KING_PARTICLE
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
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSlimeKing(void);
void UninitSlimeKing(void);
void UpdateSlimeKing(void);
void DrawSlimeKing(void);

SLIME_KING GetSlimeKing(void);


void SlimeKingDamaged(float damage);

void SpawnSlimeKing(XMFLOAT3 spawnPos);
