//=============================================================================
//
// �G�l�~�[���� [enemyKnight.h]
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
#define KNIGHT_TEXTURE_WIDTH		(132.0f * 2.5f)				// �L�����T�C�Y
#define KNIGHT_TEXTURE_HEIGHT		(60.0f  * 2.5f)				// �L�����T�C�Y

#define KNIGHT_COLLIDER_WIDTH		(KNIGHT_TEXTURE_WIDTH / 3)	// �R���C�_�[�̉���

#define ENEMY_KNIGHT_MAX			(10)						// �G�l�~�[��Max�l��

//KNIGHT�A�j���[�V����
enum 
{
	KNIGHT_ANIM_IDLE,
	KNIGHT_ANIM_MOVE,
	KNIGHT_ANIM_ATTACK,
	KNIGHT_ANIM_DEFENCE_FRONT,
	KNIGHT_ANIM_DEFENCE_UP,
	KNIGHT_ANIM_HIT,
	KNIGHT_ANIM_DEATH,

	KNIGHT_ANIM_MAX,
};

//KNIGHT����
enum {
	KNIGHT_DIR_RIGHT,
	KNIGHT_DIR_LEFT,
};

//KNIGHT���
enum {
	KNIGHT_STATE_CONTROLLER,
	KNIGHT_STATE_MOVE_STAY,
	KNIGHT_STATE_MOVE_FORWARD,
	KNIGHT_STATE_MOVE_BACKWARD,
	KNIGHT_STATE_ATTACK,
	KNIGHT_STATE_DEFENCE_FRONT,
	KNIGHT_STATE_DEFENCE_UP,
	KNIGHT_STATE_HIT,
};

//KNIGHT�ƃv���C���[�Ԃ̋���
enum {
	KNIGHT_DISTANCE_FAR,
	KNIGHT_DISTANCE_MIDWAY,
	KNIGHT_DISTANCE_NEAR_FRONT,
	KNIGHT_DISTANCE_NEAR_UP,
};
//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct ENEMY_KNIGHT
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
	float		moveDir;		// �ړ����x

	float		HP;				// �̗�

	XMFLOAT4	enemyColor;

	int			state;
	int			stateTime;
	int			stateTimeCnt;

	int			playerDistance;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyKnight(void);
void UninitEnemyKnight(void);
void UpdateEnemyKnight(void);
void DrawEnemyKnight(void);

ENEMY_KNIGHT* GetEnemyKnight(void);

void EnemyKnightDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage);

void SpawnEnemyKnight(XMFLOAT3 spawnPos);
