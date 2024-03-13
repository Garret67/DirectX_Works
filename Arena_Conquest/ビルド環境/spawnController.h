//=============================================================================
//
// �G�l�~�[���� [spawnController.h]
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
#define SPAWN_ENEMY_MAX		(45)
#define SPAWN_PLATFORM_MAX	(15)
#define SPAWNER_MAX			(8)	

// �X�|�[���̎��
enum 
{
	SPAWN_PATROL,
	SPAWN_BALL,
	SPAWN_KNIGHT,
	SPAWN_FLY_BOUNCE,
	SPAWN_FLY_ONE,
	SPAWN_SLIME_KING,
	SPAWN_BOSS,
};

// �X�|�[���摜
enum 
{
	SPAWN_TEX_ELEVATOR,
	SPAWN_TEX_ENEMY_BOX,
	SPAWN_TEX_ROUND_UI,

	SPAWN_TEX_MAX,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct SPAWN_ENEMY
{
	BOOL		spawned;
	int			round;
	int			enemy;
	int			killCondition;
	XMFLOAT3	spawnPos;
};


struct SPAWN_PLATFORM
{
	BOOL		spawned;
	int			round;
	int			killCondition;
	XMFLOAT3	spawnPos;
};


struct SPAWNER
{
	BOOL		use;			// true:�g���Ă���  false:���g�p
	XMFLOAT3	pos;			// �|���S���̍��W
	float		movePosHeight;	// �|���S���̂�������Y���W
	float		w, h;			// ���ƍ���
	int			currentSprite;	// ���̃A�j���[�V�����X�v���C�g
	int			countAnim;		// �A�j���[�V�����J�E���g
	int			texNo;			// �e�N�X�`���ԍ�

	int			spawnEnemyIndex;
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSpawnController(void);
void UninitSpawnController(void);
void UpdateSpawnController(void);
void DrawSpawnController(void);


void RoundKill(void);
int GetRound(void);