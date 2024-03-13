//=============================================================================
//
// ���f������ [player.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once
//#include "modelBinaryFBX.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PLAYER		(3)					// �v���C���[�̐�

#define	PLAYER_SIZE		(8.0f)				// �����蔻��̑傫��

enum PLAYER_ANIM 
{
	P_ANIM_IDLE,
	P_ANIM_WALK,
	P_ANIM_HIT,
	P_ANIM_DEATH,
	P_ANIM_BLOCK,
	P_ANIM_ATTACK_PUNCH,
	P_ANIM_ATTACK_CRIT,
	P_ANIM_ATTACK_AXE,

	P_ANIM_MAX,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PLAYER
{
	XMFLOAT4X4		mtxWorld;	// ���[���h�}�g���b�N�X
	XMFLOAT3		pos;		// ���f���̈ʒu
	XMFLOAT3		rot;		// ���f���̌���(��])
	XMFLOAT3		scl;		// ���f���̑傫��(�X�P�[��)
	
	BOOL			load;		// ���f�����[�h�t���O
	DX11_MODEL		model;		// ���f���o�b�t�@�[���
	MODEL_DATA		modelData;	// ���f���̃��b�V���E�g�����X�t�H�[�����
	FBX_ANIMATOR	animator;	// �A�j���[�^�[

	BOOL			use;		// �g�p�t���O
	float			size;		// �����蔻��̑傫��
	
	int				HPmax;			// �ő�HP
	int				HPcur;			// ���݂�HP
	int				agility;		// ����
	int				attack;			// �U��
	int				deffense;		// �h��
	BOOL			myTurn;			// ���̃v���C���[�̃^�[��
	BOOL			turnFinished;	// ���̃v���C���[�̃^�[�����I�����
	BOOL			dead;			// �|��Ă�����
									   
	XMFLOAT3		patrolDest;		// �ړI�n
	float			patrolFrames;	// �ړ��J�n����̃t���[����	

};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

void SavePlayerGameInfo(void);
void SavePlayerFightInfo(void);

void SetSelectionEnemy(void);
void SetSelectionPlayer(void);


void PlayerDamaged(int damage, int playerIdx);