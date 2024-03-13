//=============================================================================
//
// �G�l�~�[���� [enemyPatrol.h]
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
#define ENEMY_PATROL_MAX		(10)		// �G�l�~�[��Max�l��

enum 
{
	SLIME_ANIM_MOVE,
	SLIME_ANIM_HIT,
	SLIME_ANIM_DIE,

	SLIME_ANIM_MAX,
};


enum {
	SLIME_DIR_RIGHT,
	SLIME_DIR_LEFT,
};
//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct ENEMY_PATROL
{
	BOOL		spawned;		// true:�g���Ă���  false:���g�p
	BOOL		alive;			// true:�����Ă���  false:����ł���
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	float		w, h;			// ���ƍ���
	int			currentSprite;	// ���̃A�j���[�V�����X�v���C�g
	int			currentAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			countAnim;		// �A�j���[�V�����J�E���g
	int			texNo;			// �e�N�X�`���ԍ�
	float		moveSpeed;		// �ړ����x
	float		moveDir;		// �ړ�����
	int			moveTimeCnt;	// �ړ�����

	float		HP;				// �̗�

	BOOL		damaged;		//�G�l�~�[�����t����
	XMFLOAT3	damageOriginPos;//�ǂ�����_���[�W���󂯂�
	int			dmgTimeCnt;		

	XMFLOAT4	enemyColor;

	int			platformIndex;	//�������Ă��鏰
	BOOL		InGround;		//�������Ă��鏰
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyPatrol(void);
void UninitEnemyPatrol(void);
void UpdateEnemyPatrol(void);
void DrawEnemyPatrol(void);

ENEMY_PATROL* GetEnemyPatrol(void);


void EnemyDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage);

void SpawnEnemyPatrol(XMFLOAT3 spawnPos);
