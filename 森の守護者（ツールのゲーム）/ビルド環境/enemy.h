//=============================================================================
//
// �G�l�~�[���f������ [enemy.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_ENEMY		(20)					// �G�l�~�[�̐�

#define	ENEMY_SIZE		(8.0f)				// �����蔻��̑傫��

enum SQUELETON_ANIM
{
	SQUELETON_IDLE,
	SQUELETON_WALK,
	SQUELETON_JUMP,
	SQUELETON_HIT,
	SQUELETON_DEATH,
	SQUELETON_ATTACK_PUNCH,

	SQUELETON_ANIM_MAX,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY
{
	XMFLOAT4X4		mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3		pos;				// ���f���̈ʒu
	XMFLOAT3		rot;				// ���f���̌���(��])
	XMFLOAT3		scl;				// ���f���̑傫��(�X�P�[��)

	BOOL			use;
	BOOL			load;
	DX11_MODEL		model;		// ���f�����
	MODEL_DATA		modelData;
	FBX_ANIMATOR	animator;

	float			spd;				// �ړ��X�s�[�h
	float			size;				// �����蔻��̑傫��

	int				HP;
	int				agility;
	int				agilityCnt;
	int				attack;
	int				deffense;
	BOOL			myTurn;			//���̃G�l�~�[�̃^�[��
	BOOL			turnFinished;	//���̃G�l�~�[�̃^�[�����I�����
	BOOL			dead;			

	//int				shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	XMFLOAT3		patrolDest;			//�ړI�n
	float			patrolFrames;		//�ړ��J�n����̃t���[����	
	float			patrolWaitFrames;		//�ړ��J�n����̃t���[����	

	DISSOLVE		dissolve;
	float			shadowAlpha;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
int GetSpawnedEnemies(void);

void SaveEnemyMapInfo(int enemyIdx);

void EnemyDamaged(int damage, int enemyIdx);

void EnemyRelocation(BOOL Xmov, BOOL positiveMov, float limitPos);