//=============================================================================
//
// �G�l�~�[���� [enemyBoss.h]
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

//�{�X�̃e�N�X�`��
enum
{
	BOSS_TEX_BOSS_SPRITE_SHEET,
	BOSS_TEX_PARTIBLE,
	BOSS_TEX_LASER_SIGN,
	BOSS_TEX_LASER,
	BOSS_TEX_LIGHTNING_BALL,
	BOSS_TEX_BOSS_UI,
	BOSS_TEX_BOSS_UI_HP,

	BOSS_TEX_MAX,
};

//�{�X�̃A�j���[�V����
enum 
{
	BOSS_ANIM_CHASE,
	BOSS_ANIM_SHOOTING,
	BOSS_ANIM_LASER,
	BOSS_ANIM_DEATH1,
	BOSS_ANIM_DEATH2,
	
	BOSS_ANIM_MAX,
};

//�{�X�̏��
enum 
{
	BOSS_STATE_CHASE,
	BOSS_STATE_SHOOTING,
	BOSS_STATE_LASER,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
//�{�X�̍\����
struct BOSS
{
	BOOL		spawned;		// true:�g���Ă���  false:���g�p
	BOOL		alive;			// true�����Ă���   false:����ł���
	BOOL		shoot;			// �d�C�ʂ̔��˃t���O
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	float		w, h;			// ���ƍ���

	int			currentSprite;	// ���̃A�j���[�V�����X�v���C�g
	int			currentAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			countAnim;		// �A�j���[�V�����J�E���g
	int			texNo;			// �e�N�X�`���ԍ�
	XMFLOAT4	enemyColor;		// �F�̐ݒ�

	XMFLOAT3	moveStartPos;	// �ړ��J�n�_
	XMFLOAT3	moveEndPos;		// �ړ��I���_

	float		HP;				// �̗�

	BOOL		damaged;		// �G�l�~�[�����t����
	int			dmgTimeCnt;		// �_���[�W�J�E���g
								   
	int			phase;			// �t�F�[�Y
	int			attacks;		// �U���̐�
	int			attackCnt;		// �U���̃J�E���g
	float		attackTime;		// �U������
	float		attackTimeCnt;	// �U�����ԃJ�E���g

	int			projectileNum;	// �d�C�ʂ̐�

	int			state;			// �A�N�V�������

	XMFLOAT3	attackPos;		// �{�X�̍U���ꏊ

	XMFLOAT4	hpUIColor;		// �{�X�̗̑�UI�F
};

//�{�X�̓d�C�ʂ̍\����
struct BOSS_PROJECTILE
{
	BOOL		use;			// true:�g���Ă���  false:���g�p
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�
	int			currentSprite;	// �A�j���[�V�����p�^�[���i���o�[
	int			countAnim;		// �A�j���[�V�����J�E���g

	float		moveSpeed;		// �ړ����x
	XMFLOAT3	moveDir;		// �ړ�����

	float		angle;			// �G�ɑ΂���d�C�ʂ̉�]�p

	XMFLOAT4	color;			// �F�̐ݒ�
};

//�{�X�̃��[�U�[�̍\����
struct BOSS_LASER
{
	BOOL		use;			// true:�g���Ă���  false:���g�p
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�

	XMFLOAT4	color;			// �F�̐ݒ�

};

//�{�X�̃p�[�e�B�N���̍\����
struct BOSS_PARTICLE
{
	BOOL		use;			// true:�g���Ă���  false:���g�p
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�

	float		moveSpeed;		// �ړ����x
	XMFLOAT3	moveDir;		// �ړ����x

	XMFLOAT4	color;			// �F�̐ݒ�

	int			lifeTime;		//�g������
	int			lifeTimeCnt;	//�g�����ԃJ�E���g
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBoss(void);
void UninitBoss(void);
void UpdateBoss(void);
void DrawBoss(void);

BOSS GetBoss(void);


void BossDamaged(float damage);

void SpawnBoss(XMFLOAT3 spawnPos);
