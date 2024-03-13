//=============================================================================
//
// �G�l�~�[���� [enemyFlyBulletOne.h]
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

#define ENEMY_FLY_ONE_MAX		(10)		// �G�l�~�[��Max�l��

enum 
{
	FLY_ONE_ANIM_MOVE,
	FLY_ONE_ANIM_ATTACK,
	FLY_ONE_ANIM_HIT,
	FLY_ONE_ANIM_DEATH,
	
	FLY_ONE_ANIM_MAX,
};


enum {
	FLY_ONE_DIR_RIGHT,
	FLY_ONE_DIR_LEFT,
};

enum {
	FLY_ONE_STATE_CONTROLLER,
	FLY_ONE_STATE_STAY,
	FLY_ONE_STATE_MOVE,
	FLY_ONE_STATE_ATTACK,
	FLY_ONE_STATE_HIT,
};


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct ENEMY_FLY_ONE
{
	BOOL		spawned;		// true:�g���Ă���  false:���g�p
	BOOL		alive;			// true:�����Ă���  false:����ł���
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	float		w, h;			// ���ƍ���
	int			currentSprite;	// ���̃A�j���[�V�����X�v���C�g
	int			currentSpriteMax;	// ���̃A�j���[�V�����X�v���C�g
	int			currentAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			countAnim;		// �A�j���[�V�����J�E���g
	int			texNo;			// �e�N�X�`���ԍ�
	float		moveSpeed;		// �ړ����x
	float		moveDir;		// �ړ�����
	XMFLOAT3	WayPointPos;	// �ړ�����

	float		HP;				// �̗�

	XMFLOAT4	enemyColor;

	int			state;
	int			stateTime;
	int			stateTimeCnt;

	int			attackTimeCnt;

	XMFLOAT3	damageOriginPos;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyFlyOne(void);
void UninitEnemyFlyOne(void);
void UpdateEnemyFlyOne(void);
void DrawEnemyFlyOne(void);

ENEMY_FLY_ONE* GetEnemyFlyOne(void);

void EnemyFlyOneDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage);

void SpawnEnemyFlyOne(XMFLOAT3 spawnPos);
