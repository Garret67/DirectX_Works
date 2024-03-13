//=============================================================================
//
// �v���C���[���� [player.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"
#include "playerUI.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define PLAYER_MAX			(1)		// �v���C���[��Max�l��

#define	PLAYER_OFFSET_CNT	(10)	// �_�b�V���̉e��

//�v���C���[�̌���
enum
{
	CHAR_DIR_UP,
	CHAR_DIR_RIGHT,
	CHAR_DIR_DOWN,
	CHAR_DIR_LEFT,

	CHAR_DIR_MAX
};

//�X���b�V���̌���
enum
{
	SLASH_DIR_UP,
	SLASH_DIR_RIGHT,
	SLASH_DIR_DOWN,
	SLASH_DIR_LEFT,

	SLASH_DIR_MAX
};

//�v���C���[�̂̃A�j���[�V����
enum
{
	ANIM_IDLE,
	ANIM_RUN,
	ANIM_JUMP,
	ANIM_FALL,
	ANIM_ATTACK,
	ANIM_DASH,
	ANIM_DASH_OFFSET,
	ANIM_HIT,
	ANIM_FOCUS,
	ANIM_DEATH,
	ANIM_SLASH,
	ANIM_SLASHHIT,
	ANIM_FOCUS_EFFECT,
	ANIM_EXCLAMATION,

	ANIM_TEX_MAX
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct PLAYER
{
	XMFLOAT3	pos;				// �|���S���̍��W
	XMFLOAT3	rot;				// �|���S���̉�]��
	BOOL		alive;				// true:�g���Ă���  false:���g�p
	float		w, h;				// ���ƍ���
	float		countAnim;			// �A�j���[�V�����J�E���g
	int			patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;				// �e�N�X�`���ԍ�

	int			lifes;				// ���̗̑�
	int			lifesMax;			// �̗�Max
	int			energy;				// ���̃G�l���M�[
	int			energyMax;			// �G�l���M�[Max
	
	int			dir;				// �����i0:�� 1:�E 2:�� 3:���j
	int			Lastdir;			// �����i0:�� 1:�E 2:�� 3:���j
	BOOL		moving;				// �ړ����t���O

	BOOL		dash;				// �_�b�V�����t���O
	BOOL		dashReload;			// 
	int			dashFrames;			// �_�b�V���̃t���[����
	int			dashCnt;			// ���̃_�b�V���̃t���[����
	float		dashDistance;		// �_�b�V���̋���

	float		moveSpeed;			// �ړ����x
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// �c���|���S���̍��W

	BOOL		jump;				// �W�����v�t���O
	int			jumpCnt;			// �W�����v���̃J�E���g
	float		jumpYMax;			// �W�����v�̃t���[����
	BOOL		extendJump;			// �W�����v�̉���
	int			extendJumpCnt;		// ���̃W�����v�̉����̃t���[����
	int			extendJumpMax;		// �W�����v�̉����̃t���[����
	

	
	BOOL		slash;				// Slash���t���O
	int			slashFrames;		// Slash�̃t���[����
	BOOL		slashCollider;		// Slash�����蔻��L��
	int			slashColliderFrames;// Slash�̃t���[����
	int			slashCnt;			// ����Slash�̃t���[����
	float		slashDistance;		// Slash�̋���
	int			slashDir;			// Slash�̋���
	float		slashDmg;			// Slash�_���[�W

	BOOL		onGround;			// �n�ʂɐG��Ă���
	int			onAirCnt;			// �n�ʂɐG�ꂸ�Ɉ��t���[�����󒆂ɂ���΁A���ł���
	float		gravityVelocity;	

	BOOL		damaged;			//�G�l�~�[�����t����
	XMFLOAT3	damageOriginPos;	//�ǂ�����_���[�W���󂯂�
	int			dmgTime;
	int			dmgTimeCnt;

	BOOL		invincible;			//���G���
	int			invincibleTime;		//���G�̃t���[����
	int			invincibleTimeCnt;	//���̖��G�̃t���[����

	BOOL		focus;				//�W�����
	int			focusTimeMax;		//�W���̃t���[����
	int			focusTimeCnt;		//���̏W���̃t���[����
	int			focusEffectSprite;	//���̏W���G�t�F�N�g�̃X�v���C�g��


	XMFLOAT4	playerColor;

};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);

void PlayerDamaged(XMFLOAT3 enemyPos);

void SetPlayerStats(int hp, int energy);