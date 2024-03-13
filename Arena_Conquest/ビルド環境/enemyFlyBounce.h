//=============================================================================
//
// �G�l�~�[���� [enemyFlyBounce.h]
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
#define ENEMY_FLY_BOUNCE_MAX		(10)		// �G�l�~�[��Max�l��

enum 
{
	FLY_BOUNCE_ANIM_MOVE,
	FLY_BOUNCE_ANIM_HIT,
	FLY_BOUNCE_ANIM_DIE,

	FLY_BOUNCE_ANIM_MAX,
};


enum {
	FLY_BOUNCE_DIR_RIGHT,
	FLY_BOUNCE_DIR_LEFT,
};


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct ENEMY_FLY_BOUNCE
{
	BOOL		spawned;		// true:�g���Ă���  false:���g�p
	BOOL		alive;			// true�����Ă���   false:����ł���

	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	float		w, h;			// ���ƍ���
	int			currentSprite;	// ���̃A�j���[�V�����X�v���C�g
	int			currentAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			countAnim;		// �A�j���[�V�����J�E���g
	int			texNo;			// �e�N�X�`���ԍ�
	XMFLOAT3	moveSpeed;		// �ړ����x
	float		moveDir;		// �ړ����x

	float		HP;				// �̗�

	BOOL		damaged;		//�G�l�~�[Bounce�����t����
	XMFLOAT3	damageOriginPos;//�ǂ�����_���[�W���󂯂�
	int			dmgTimeCnt;		//�_���[�W���ԃJ�E���g

	XMFLOAT4	enemyColor;		//�F�̐ݒ�

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyFlyBounce(void);
void UninitEnemyFlyBounce(void);
void UpdateEnemyFlyBounce(void);
void DrawEnemyFlyBounce(void);

ENEMY_FLY_BOUNCE* GetEnemyFlyBounce(void);


void EnemyFlyBounceDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage);


void SpawnEnemyFlyBounce(XMFLOAT3 spawnPos);