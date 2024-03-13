//=============================================================================
//
// �G�l�~�[���� [enemyBall.h]
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
#define ENEMY_BALL_MAX			(10)		// �G�l�~�[��Max��

enum 
{
	BALL_ANIM_MOVE,
	BALL_ANIM_HIT,
	BALL_ANIM_DIE,
	BALL_ANIM_SEARCH,

	BALL_ANIM_MAX,
};


enum {
	BALL_DIR_RIGHT,
	BALL_DIR_LEFT,
};


enum {
	BALL_STATE_SEARCH,
	BALL_STATE_RUSH,
	BALL_STATE_JUMP,
};
//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct ENEMY_BALL
{
	BOOL		spawned;		// true:�g���Ă���  false:���g�p
	BOOL		alive;			// true�����Ă���   false:����ł���
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			currentSprite;	// ���̃A�j���[�V�����X�v���C�g
	int			currentAnim;	// ���̃A�j���[�V�����p�^�[��
	int			countAnim;		// �A�j���[�V�����J�E���g
	int			texNo;			// �e�N�X�`���ԍ�
	float		moveSpeed;		// �ړ����x
	int			moveDir;		// �G�l�~�[�̌���

	float		HP;				// �̗�

	BOOL		damaged;		//�G�l�~�[�����t�����t���O
	XMFLOAT3	damageOriginPos;//�ǂ�����_���[�W���󂯂�
	int			dmgTimeCnt;		//�_���[�W�J�E���g

	XMFLOAT4	enemyColor;		//�F�̐ݒ�

	int			state;			//�A�N�V�������
	int			stateTime;		//�A�N�V������Ԏ���
	int			stateTimeCnt;	//�A�N�V������Ԏ��ԃJ�E���g
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyBall(void);
void UninitEnemyBall(void);
void UpdateEnemyBall(void);
void DrawEnemyBall(void);


ENEMY_BALL* GetEnemyBall(void);


void EnemyBallDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage);

void SpawnEnemyBall(XMFLOAT3 spawnPos);
