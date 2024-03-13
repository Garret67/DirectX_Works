//=============================================================================
//
// ���f������ [player.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"

#include "modelBinaryFBX.h"
#include "player.h"
#include "debugproc.h"
#include "fade.h"
#include "enemy.h"
#include "fightUI.h"
#include "sound.h"
#include "fightUI.h"
#include "particle.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_WOODCUTTER	"data\\MODEL\\Woodcutter_Model_Data.bin"	// �ǂݍ��ރ��f����
#define	MODEL_GNOME			"data\\MODEL\\Gnome_Model_Data.bin"			// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(2.0f)		// �ړ���
#define FIGHT_SPEED			(1.5f)		// �U���ړ����x

#define LERP_GAME_FACTOR	(0.3f)		// LERP���x
#define LERP_FIGHT_FACTOR	(0.1f)		// ��]LERP�̑��x

#define SLOW_OUT_DISTANCE	(25.0f)		// SLOW OUT���n�܂鋗��
#define SLOW_IN_FRAMES		(30.0f)		// SLOW IN��������t���[����

#define DISTANCE_BTW_ENEMY_ROWS (50.0f)	//�G��Ԃ̋���// distance Between Enemy Rows

#define OBJ_IDX_AXE (7)					//���̃C���f�b�N�X

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void LoadPlayerGameInfo(void);
void LoadPlayerFightInfo(void);

void UpdatePlayerGameMode(void);
void UpdatePlayerFightMode(void);

void CheckFightTurn(int idx);

void SelectTargetEnemy(int& targetIdx);
void SelectTargetPlayer(int& targetIdx);

void UpdatePMovement(float moveX, float moveZ, float moveVectorMagnitude, int Idx, float speedMultiplier, float slowInFrames, float slowOutDistance);
void PlayerAttack(int Idx, int& attackedIdx, BOOL& attack);

void PlayerPunchAttack(int Idx, int attackedIdx, BOOL& critFail, BOOL& enemyHitted, BOOL& attacked);
void PlayerAxeAttack(int Idx, int attackedIdx, BOOL& enemyHitted, BOOL& attacked);


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PLAYER_MAP_INFO
{
	XMFLOAT3		pos[MAX_PLAYER];		// ���f���̈ʒu
	XMFLOAT3		rot[MAX_PLAYER];		// ���f���̌���(��])
	BOOL			use[MAX_PLAYER];		// �g�p�t���O

	int				HPmax	[MAX_PLAYER];	// �ő�HP
	int				HPcur	[MAX_PLAYER];	// ���݂�HP
	int				agility	[MAX_PLAYER];	// ���x
	int				attack	[MAX_PLAYER];	// �U���l
	int				deffense[MAX_PLAYER];	// �h��l

	BOOL			saved;		// �g�p�t���O
};

//�U�����
enum ATTACK_STATUS
{
	ATTACK_NONE,
	ATTACK_PUNCH,
	ATTACK_AXE,
};



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER			g_Player[MAX_PLAYER];			// �v���C���[

static PLAYER_MAP_INFO	g_PlayerMapInfo;				// �t�@�C�g�ɓ��鎞�ׂ̈̃v���C���[���

static XMFLOAT3			g_FightPos[MAX_PLAYER] =		// �o�g���̑ҋ@���W
{
	XMFLOAT3(  0.0f, 0.0f, -70.0f),
	XMFLOAT3( 60.0f, 0.0f, -70.0f),
	XMFLOAT3(-60.0f, 0.0f, -70.0f),
};

static char* g_PlayerModelFile[MAX_PLAYER] =			// ���f���t�@�C��
{
	MODEL_WOODCUTTER,
	MODEL_GNOME,
	MODEL_GNOME,
};

static BOOL selectionEnemy;		// �ΏۃG�l�~�[��I��ł�����
static BOOL selectionPlayer;	// �Ώۃv���C���[��I��ł�����

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	//���[�h�ɂ���ď���������
	switch (GetMode())
	{
	case MODE_GAME:
		InitModelBinaryFBX(MODEL_WOODCUTTER, &g_Player[0].model, &g_Player[0].modelData, &g_Player[0].animator);
		g_Player[0].load = TRUE;

		if (g_PlayerMapInfo.saved)
		{
			LoadPlayerGameInfo();
		}
		else
		{
			g_Player[0].pos			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Player[0].rot			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Player[0].scl			 = XMFLOAT3(1.0f, 1.0f, 1.0f);
									 
			g_Player[0].use			 = TRUE;			// TRUE:�����Ă�
			g_Player[0].size		 = PLAYER_SIZE;		// �����蔻��̑傫��
									 
			for (int i = 0; i < MAX_PLAYER; i++)
			{
				g_Player[i].HPmax		 = 20;
				g_Player[i].HPcur		 = 20;
				g_Player[i].agility		 = 12;
				g_Player[i].attack		 = 10;
				g_Player[i].deffense	 = 10;
				g_Player[i].myTurn		 = FALSE;
				g_Player[i].turnFinished = FALSE;
			}
		}

		break;

	case MODE_FIGHT:
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			InitModelBinaryFBX(g_PlayerModelFile[i], &g_Player[i].model, &g_Player[i].modelData, &g_Player[i].animator);
			g_Player[i].load = TRUE;

			g_Player[i].pos			 = g_FightPos[i];
			g_Player[i].rot			 = XMFLOAT3(0.0f, XM_PI, 0.0f);
			g_Player[i].scl			 = XMFLOAT3(1.0f, 1.0f, 1.0f);

			g_Player[i].use			 = TRUE;			// TRUE:�����Ă�
			g_Player[i].size		 = PLAYER_SIZE;		// �����蔻��̑傫��

			g_Player[i].myTurn		 = FALSE;
			g_Player[i].turnFinished = FALSE;

			g_Player[i].patrolDest	 = g_Player[i].pos;
			g_Player[i].patrolFrames = 0;

		}
		LoadPlayerFightInfo();

		break;

	default:
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			g_Player[i].pos			 = XMFLOAT3(-10.0f, 0.0f, -50.0f);
			g_Player[i].rot			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Player[i].scl			 = XMFLOAT3(1.0f, 1.0f, 1.0f);

			g_Player[i].use			 = TRUE;			// TRUE:�����Ă�
			g_Player[i].size		 = PLAYER_SIZE;	// �����蔻��̑傫��

			g_Player[i].HPmax		 = 20;
			g_Player[i].HPcur		 = 20;
			g_Player[i].agility		 = 4;
			g_Player[i].attack		 = 10;
			g_Player[i].deffense	 = 10;
			g_Player[i].myTurn		 = FALSE;
			g_Player[i].turnFinished = FALSE;

			g_Player[i].patrolDest	 = g_Player[i].pos;
			g_Player[i].patrolFrames = 0;

		}

		break;
	}

	

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	//�N�����|��Ă�����AHP�P�ɂ���
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].HPcur <= 0)
		{
			g_Player[i].HPcur = 1;
			g_Player[i].dead = FALSE;
		}
	}

	//MODE���o�鎞�Ƀv���C���[�����Z�[�u����
	if (GetMode() == MODE_GAME) SavePlayerGameInfo();		
	if (GetMode() == MODE_FIGHT) SavePlayerFightInfo();	

	//�v���C���[���f���̃������[�������
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].load == TRUE)
		{
			UninitModelBinaryFBX(&g_Player[i].model, &g_Player[i].modelData, &g_Player[i].animator);
			g_Player[i].load = FALSE;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	switch (GetMode())
	{
	case MODE_GAME:
		UpdatePlayerGameMode();
		break;

	case MODE_FIGHT:
		UpdatePlayerFightMode();
		break;

	default:

		break;
	}

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].load && g_Player[i].use)
		{
			if (g_Player[i].animator.curAnim != -1 && g_Player[i].animator.animationNum > 0)
			{
				UpdateModelBinaryFBX(&g_Player[i].modelData, g_Player[i].animator);
			}
		}
	}

#ifdef _DEBUG
	// �f�o�b�O�\��
		
	//�v���C���[�̍��W
	PrintDebugProc("Player X:%f Y:%f Z:% N:%f\n", g_Player[0].pos.x, g_Player[0].pos.y, g_Player[0].pos.z);
	//�v���C���[�̉�]
	PrintDebugProc("Rotation Y: %f \n", XMConvertToDegrees(g_Player[0].rot.y));
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_BACK);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	//*************
	// �v���C���[�̕`��
	//*************
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].load == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Player[i].scl.x, g_Player[i].scl.y, g_Player[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);

		// ���f���`��
		//DrawModel(&g_Player[i].model);
		DrawModelBinaryFBX(&g_Player[i].model, g_Player[i].modelData, mtxWorld);


		//*************
		// �e�̕`��
		//*************

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x /*+ XM_PIDIV4*/, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Player[i].scl.x, 0.01f, g_Player[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, 0.01f, g_Player[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);


		//�}�e���A����Diffuse��ς���
		XMFLOAT4* MaterialDiffuse = new XMFLOAT4[100];
		int matIdx = 0;

		for (int z = 0; z < g_Player[i].model.BuffersNum; z++)
		{
			for (int j = 0; j < g_Player[i].model.Buffers[z].SubsetNum; j++)
			{
				MaterialDiffuse[matIdx] = g_Player[i].model.Buffers[z].SubsetArray[j].Material.Material.Diffuse;
				g_Player[i].model.Buffers[z].SubsetArray[j].Material.Material.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.2f);
				matIdx++;
			}
		}
		

		// �e�`��
		DrawModelBinaryFBX(&g_Player[i].model, g_Player[i].modelData, mtxWorld);

		//�}�e���A����Diffuse��߂�
		matIdx = 0;
		for (int z = 0; z < g_Player[i].model.BuffersNum; z++)
		{
			for (int j = 0; j < g_Player[i].model.Buffers[z].SubsetNum; j++)
			{
				g_Player[i].model.Buffers[z].SubsetArray[j].Material.Material.Diffuse = MaterialDiffuse[matIdx];
				matIdx++;
			}
		}
		//�������[���
		delete[] MaterialDiffuse;

	}


	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player[0];
}


//=============================================================================
// �v���C���[��GAME�����Z�[�u����
//=============================================================================
void SavePlayerGameInfo(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		g_PlayerMapInfo.pos[i]		= g_Player[i].pos;
		g_PlayerMapInfo.rot[i]		= g_Player[i].rot;
		g_PlayerMapInfo.use[i]		= g_Player[i].use;
		g_PlayerMapInfo.HPmax[i]	= g_Player[i].HPmax;
		g_PlayerMapInfo.HPcur[i]	= g_Player[i].HPcur;
		g_PlayerMapInfo.agility[i]	= g_Player[i].agility;
		g_PlayerMapInfo.attack[i]	= g_Player[i].attack;
		g_PlayerMapInfo.deffense[i] = g_Player[i].deffense;
	}

	g_PlayerMapInfo.saved = TRUE;
}

//=============================================================================
// �v���C���[��FIGHT�����Z�[�u����
//=============================================================================
void SavePlayerFightInfo(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		g_PlayerMapInfo.HPmax[i]	= g_Player[i].HPmax;
		g_PlayerMapInfo.HPcur[i]	= g_Player[i].HPcur;
		g_PlayerMapInfo.agility[i]	= g_Player[i].agility;
		g_PlayerMapInfo.attack[i]	= g_Player[i].attack;
		g_PlayerMapInfo.deffense[i] = g_Player[i].deffense;
	}

	g_PlayerMapInfo.saved = TRUE;
}


//=============================================================================
// �v���C���[��GAME�����擾
//=============================================================================
void LoadPlayerGameInfo(void)
{
	if (g_PlayerMapInfo.saved)
	{
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			g_Player[i].pos		 = g_PlayerMapInfo.pos[i];
			g_Player[i].rot		 = g_PlayerMapInfo.rot[i];
			g_Player[i].use		 = g_PlayerMapInfo.use[i];
			g_Player[i].HPmax	 = g_PlayerMapInfo.HPmax[i];
			g_Player[i].HPcur	 = g_PlayerMapInfo.HPcur[i];
			g_Player[i].agility  = g_PlayerMapInfo.agility[i];
			g_Player[i].attack	 = g_PlayerMapInfo.attack[i];
			g_Player[i].deffense = g_PlayerMapInfo.deffense[i];
		}

		g_PlayerMapInfo.saved = FALSE;
	}
}

//=============================================================================
// �v���C���[��FIGHT�����擾
//=============================================================================
void LoadPlayerFightInfo(void)
{
	if (g_PlayerMapInfo.saved)
	{
		for (int i = 0; i < MAX_PLAYER; i++)
		{
			g_Player[i].HPmax	 = g_PlayerMapInfo.HPmax[i];
			g_Player[i].HPcur	 = g_PlayerMapInfo.HPcur[i];
			g_Player[i].agility  = g_PlayerMapInfo.agility[i];
			g_Player[i].attack	 = g_PlayerMapInfo.attack[i];
			g_Player[i].deffense = g_PlayerMapInfo.deffense[i];
		}

		g_PlayerMapInfo.saved = FALSE;
	}
}


//=============================================================================
// �v���C���[��GAME Update����
//=============================================================================
void UpdatePlayerGameMode(void)
{
	XMFLOAT3 lastPos = g_Player[0].pos;

	CAMERA* cam = GetCamera();

	// �ړ�����
	float moveX = 0.0f;
	float moveZ = 0.0f;

	//�L�[�{�[�h�ړ�����
	if (GetKeyboardPress(DIK_LEFT)	) moveX = -1.0f;
	if (GetKeyboardPress(DIK_RIGHT)	) moveX =  1.0f;
	if (GetKeyboardPress(DIK_UP)	) moveZ =  1.0f;
	if (GetKeyboardPress(DIK_DOWN)	) moveZ = -1.0f;

	//�ړ���
	float moveMagnitude = sqrtf(moveX * moveX + moveZ * moveZ);
	//�ړ��ʐ��K��
	if (moveMagnitude > 1.0f)
	{
		moveX /= moveMagnitude;
		moveZ /= moveMagnitude;
	}

	//�Q�[���p�b�h�ړ�����
	if (GetJoystickLevelX(0)) moveX =  GetJoystickLevelX(0);
	if (GetJoystickLevelY(0)) moveZ = -GetJoystickLevelY(0);

	//�ړ���
	moveMagnitude = sqrtf(moveX * moveX + moveZ * moveZ);
	//�ړ��ʐ��K��
	if (moveMagnitude > 1.0f)
	{
		moveX /= moveMagnitude;
		moveZ /= moveMagnitude;
	}

	//�ړ�����
	if (moveX != 0.0f || moveZ != 0.0f)
	{
		// ���K�������ړ��x�N�g���̒������͂���i�ړ��ʁj
		moveMagnitude = sqrtf(moveX * moveX + moveZ * moveZ);

		// ���ۂ̈ړ����x��������
		float speed = VALUE_MOVE * moveMagnitude;

		// ���x�ɂ���ăA�j���[�^�[�̑��x�𒲐�
		g_Player[0].animator.animation[P_ANIM_WALK].speed = speed;


		// ������8�����ɐ�������
		float moveAngle = atan2f(moveZ, moveX);
		float roundedAngle = roundf(moveAngle / XM_PIDIV4) * XM_PIDIV4;
		float adjustedAngle = roundedAngle + XM_PIDIV4;

		// �ړ���X������Z�������v�Z����
		float rotatedMoveX = cosf(adjustedAngle);
		float rotatedMoveZ = sinf(adjustedAngle);

		// �X�s�[�h��K�p���A�L�����N�^�[�̈ʒu���X�V����
		g_Player[0].pos.x += rotatedMoveX * speed;
		g_Player[0].pos.z += rotatedMoveZ * speed;

		// ���`��ԁilerp�j�ŉ�]�����炩��
		float avanceX = lastPos.x - g_Player[0].pos.x;
		float avanceZ = lastPos.z - g_Player[0].pos.z;
		float targetRotation = atan2f(avanceX, avanceZ);

		float angleDifference = targetRotation - g_Player[0].rot.y;		// 2�̕����Ԃ̍ŒZ�p�x���v�Z����  (ES)Calcular el angulo mas corto entre las dos direcciones

		angleDifference = NormalizeAngle(angleDifference);		// �p�x���𐳋K������	(ES) Normalizar la diferencia angular


		g_Player[0].rot.y = lerp(g_Player[0].rot.y, g_Player[0].rot.y + angleDifference, LERP_GAME_FACTOR);	//���`���

		//�����̃A�j���[�V�����ɕς��
		if (g_Player[0].animator.curAnim != P_ANIM_WALK) g_Player[0].animator.nextAnim = P_ANIM_WALK;

	}
	else
	{
		//��]���傫���Ȃ肷���Ȃ��悤��
		if (g_Player[0].rot.y > XM_2PI) g_Player[0].rot.y -= XM_2PI;
		else if (g_Player[0].rot.y < -XM_2PI) g_Player[0].rot.y += XM_2PI;

		//���A�j���[�V�����ɕς��
		if (g_Player[0].animator.curAnim != P_ANIM_IDLE) g_Player[0].animator.nextAnim = P_ANIM_IDLE;
	}
}

//=============================================================================
// �^�[�������߂܂�
//=============================================================================
void CheckFightTurn(int idx)
{
	ENEMY* enemy = GetEnemy();

	//���݂̃^�[���̃`�F�b�N
	for (int j = 0; j < MAX_PLAYER; j++)
	{
		if (g_Player[j].myTurn) return;
	}
	for (int j = 0; j < MAX_ENEMY; j++)
	{
		if (enemy[j].myTurn) return;
	}

	//�N�̔Ԃł��Ȃ��̂ŁA���ɃA�N�V��������l�����߂�B
	int Idx		   = 0;
	int maxAgility = 0;
	BOOL enemyTurn = FALSE;

	//�܂��A�N�V�������Ă��Ȃ��v���C���[�̑������m�F����
	int deadPlayer = 0;
	for (int j = 0; j < MAX_PLAYER; j++)
	{
		if (g_Player[j].dead) deadPlayer++;
		if (!g_Player[j].use || !g_Player[j].load || g_Player[j].dead) continue;
		if (!g_Player[j].turnFinished && g_Player[j].agility > maxAgility)
		{
			Idx = j;
			maxAgility = g_Player[j].agility;
		}
	}
	if (deadPlayer == MAX_PLAYER)//�v���C���[�S���|�ꂽ��A�^�C�g����ʂ�
	{
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_TITLE);
		return;
	}

	//�܂��A�N�V�������Ă��Ȃ��G�l���M�[�̑������m�F����
	int deadEnemy = 0;
	for (int j = 0; j < MAX_ENEMY; j++)
	{
		if (enemy[j].dead) deadEnemy++;
		if (!enemy[j].use || !enemy[j].load || enemy[j].dead) continue;
		if (!enemy[j].turnFinished && enemy[j].agility > maxAgility)
		{
			Idx = j;
			maxAgility = enemy[j].agility;
			enemyTurn = TRUE;
		}
	}
	if (deadEnemy == GetSpawnedEnemies())//�G�l�~�[�S���|�ꂽ��A�Q�[����ʂ�
	{
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_GAME);
		return;
	}

	//���ʔ��\�i�N��1�ԑ����j
	if (maxAgility == 0)	//���E���h���I����āA�݂�Ȃ̃^�[�������Z�b�g����
	{
		for (int j = 0; j < MAX_PLAYER; j++)
		{
			if (!g_Player[j].use || !g_Player[j].load) continue;
			g_Player[j].turnFinished = FALSE;
		}
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			if (!enemy[j].use || !enemy[j].load) continue;
			enemy[j].turnFinished = FALSE;
		}
	}
	else if (enemyTurn)//�G�l�~�[�̃^�[��
	{
		enemy[Idx].myTurn = TRUE;
	}
	else //�v���C���[�̃^�[��
	{
		g_Player[Idx].myTurn = TRUE;
		SetActionButton(g_Player[Idx].pos, TRUE);
	}
}


//***********************************
// �v���C���[�̃o�g�����[�h���X�V
//***********************************
void UpdatePlayerFightMode(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes

			{//�N���̃^�[��

				//if (CheckOthersTurnP(i)) continue;
				CheckFightTurn(i);
			}

			//�N�̔Ԃł��Ȃ��i���̃G�l�~�[�ȊO�j
			if (g_Player[i].myTurn)	//���̃G�l�~�[�̃^�[���ł��Ȃ���
			{
				ENEMY* enemy = GetEnemy();

				static int attack;
				static int targetIdx;
				PrintDebugProc("targetIdx: %d \n", targetIdx);

				//�U���̏���
				if (attack != ATTACK_NONE)
				{
					PlayerAttack(i, targetIdx, attack);
					return;
				}

				//ACTION�{�^��
				if (!selectionEnemy && !GetActionMenuUse())
				{
					//ACTION�I��
					if (GetKeyboardTrigger(DIK_D) || IsButtonTriggered(0, BUTTON_B))	//�ʏ�U��
					{
						selectionEnemy = TRUE;
						SetButtonSelected(BUTTON_RIGHT_ATTACK);
						PlaySound(SOUND_LABEL_SE_ChangeButton);
						break;
					}
					else if (GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_A))//���̂��i�ɂ���j
					{
						SetActionsMenu(TRUE, BUTTON_DOWN_OTHER);
						SetButtonSelected(BUTTON_DOWN_OTHER);
						PlaySound(SOUND_LABEL_SE_ChangeButton);
						break;
					}
					else if (GetKeyboardTrigger(DIK_A) || IsButtonTriggered(0, BUTTON_X))//�X�y�V�����i����U���j
					{
						SetActionsMenu(TRUE, BUTTON_LEFT_SPECIAL);
						SetButtonSelected(BUTTON_LEFT_SPECIAL);
						PlaySound(SOUND_LABEL_SE_ChangeButton);
						break;
					}
					else if (GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_Y))//�A�C�e���i�񕜖�j
					{
						SetActionsMenu(TRUE, BUTTON_UP_ITEM);
						SetButtonSelected(BUTTON_UP_ITEM);
						PlaySound(SOUND_LABEL_SE_ChangeButton);
						break;
					}
				}

				//�I������ACTION����������
				if (selectionEnemy || GetActionMenuUse())
				{
					if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_A))
					{
						selectionEnemy = FALSE;
						selectionPlayer = FALSE;
						SetSelectionPointer(enemy[targetIdx].pos, FALSE);	// Reset Pointer
						SetActionsMenu(FALSE, ACTION_BUTTONS_OFF);
						SetButtonSelected(0);

						PlaySound(SOUND_LABEL_SE_PulseButtonBack);
					}
				}


				//�^�[�Q�b�g��I�ԏ���
				if (selectionEnemy)
				{
					SelectTargetEnemy(targetIdx);	//�G�l�~�[��I�ԏ���

					if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B))
					{
						//����U��
						if (GetActionMenuUse())
						{
							switch (GetSelecteOption())
							{
							case 0://AXE
								attack = ATTACK_AXE;
								break;

							default:
								break;
							}

						}
						//�ʏ�U��
						else
						{
							attack = ATTACK_PUNCH;
						}
						selectionEnemy = FALSE;

						SetSelectionPointer(enemy[targetIdx].pos, FALSE);	// Reset Pointer
						SetButtonSelected(0);
						SetActionButton(g_Player[0].pos, FALSE);			// Reset Action Button
						SetActionsMenu(FALSE, ACTION_BUTTONS_OFF);

						PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
					}
				}
				else if (selectionPlayer)
				{
					SelectTargetPlayer(targetIdx);//�v���C���[��I�ԏ���

					if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B))
					{
						//�@�񕜌���
						switch (GetSelecteOption())
						{
						case IT_HEAL_POTION_SMALL:
							g_Player[targetIdx].HPcur += 5;
							break;

						case IT_HEAL_POTION_MID:
							g_Player[targetIdx].HPcur += 10;
							break;

						case IT_HEAL_POTION_BIG:
							g_Player[targetIdx].HPcur += 20;
							break;

						default:
							break;
						}

						if (g_Player[targetIdx].HPcur > g_Player[targetIdx].HPmax) g_Player[targetIdx].HPcur = g_Player[targetIdx].HPmax;
						SetHitParticle(g_Player[targetIdx].pos);

						selectionPlayer = FALSE;
						g_Player[i].myTurn = FALSE;				//�^�[��	�t���O
						g_Player[i].turnFinished = TRUE;		//���̃��E���h�Ŋ���ACTION���s�����@�t���O

						SetSelectionPointer(g_Player[targetIdx].pos, FALSE);	// Reset Pointer
						SetButtonSelected(0);
						SetActionButton(g_Player[0].pos, FALSE);			// Reset Action Button
						SetActionsMenu(FALSE, ACTION_BUTTONS_OFF);

						PlaySound(SOUND_LABEL_SE_Heal);
						break;
					}

				}

			
#ifdef _DEBUG
				//�v���C���[�^�[�����΂�
				if (GetKeyboardTrigger(DIK_G))
				{
					g_Player[i].myTurn = FALSE;
					g_Player[i].turnFinished = TRUE;
					SetActionButton(g_Player[i].pos, FALSE);
				}
#endif // DEBUG


			}
		}
	}
}


//******************************************
// �G�l���M�[�^�[�Q�b�g��I��
//******************************************
void SelectTargetEnemy(int& targetIdx)
{
	ENEMY* enemy = GetEnemy();

	if (!GetSelectionPointerState())
	{
		for (int e = 0; e < MAX_ENEMY + 1; e++)//�ŏ��̃^�[�Q�b�g
		{
			targetIdx = e;
			if (enemy[e].use && !enemy[e].dead) break;
		}
		SetSelectionPointer(enemy[targetIdx].pos, TRUE);
	}


	BOOL invalidTarget = TRUE;

	if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_JS_RIGHT) || IsButtonTriggered(0, BUTTON_ARROW_RIGHT))
	{
		while (invalidTarget)
		{
			targetIdx = targetIdx <= 0 ? GetSpawnedEnemies() - 1 : --targetIdx;

			if (enemy[targetIdx].use && !enemy[targetIdx].dead) invalidTarget = FALSE;
		}
		SetSelectionPointer(enemy[targetIdx].pos, TRUE);
	}
	else if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_JS_LEFT) || IsButtonTriggered(0, BUTTON_ARROW_LEFT))
	{
		while (invalidTarget)
		{
			targetIdx = (targetIdx + 1) % GetSpawnedEnemies();

			if (enemy[targetIdx].use && !enemy[targetIdx].dead) invalidTarget = FALSE;
		}
		SetSelectionPointer(enemy[targetIdx].pos, TRUE);
	}
	else if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_JS_DOWN) || IsButtonTriggered(0, BUTTON_ARROW_DOWN))
	{
		while (invalidTarget)
		{
			for (int i = 0; i < 3; i++)
			{
				targetIdx = targetIdx <= 0 ? GetSpawnedEnemies() - 1 : --targetIdx;
			}

			if (enemy[targetIdx].use && !enemy[targetIdx].dead) invalidTarget = FALSE;
		}
		SetSelectionPointer(enemy[targetIdx].pos, TRUE);
	}
	else if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_JS_UP) || IsButtonTriggered(0, BUTTON_ARROW_UP))
	{
		while (invalidTarget)
		{
			for (int i = 0; i < 3; i++)
			{
				targetIdx = (targetIdx + 1) % GetSpawnedEnemies();
			}

			if (enemy[targetIdx].use && !enemy[targetIdx].dead) invalidTarget = FALSE;
		}
		SetSelectionPointer(enemy[targetIdx].pos, TRUE);
	}
}

//******************************************
// �v���C���[�^�[�Q�b�g��I��
//******************************************
void SelectTargetPlayer(int& targetIdx)
{
	if (!GetSelectionPointerState())
	{
		for (int p = 0; p < MAX_PLAYER + 1; p++)//�ŏ��̃^�[�Q�b�g
		{
			targetIdx = p;
			if (g_Player[p].use && !g_Player[p].dead) break;
		}
		SetSelectionPointer(g_Player[targetIdx].pos, TRUE);
	}

	BOOL invalidTarget = TRUE;

	if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_JS_LEFT) || IsButtonTriggered(0, BUTTON_ARROW_LEFT))
	{
		while (invalidTarget)
		{
			targetIdx = targetIdx <= 0 ? MAX_PLAYER - 1 : --targetIdx;

			if (g_Player[targetIdx].use && !g_Player[targetIdx].dead) invalidTarget = FALSE;
		}
		SetSelectionPointer(g_Player[targetIdx].pos, TRUE);
	}
	else if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_JS_RIGHT) || IsButtonTriggered(0, BUTTON_ARROW_RIGHT))
	{
		while (invalidTarget)
		{
			targetIdx = (targetIdx + 1) % MAX_PLAYER;

			if (g_Player[targetIdx].use && !g_Player[targetIdx].dead) invalidTarget = FALSE;
		}
		SetSelectionPointer(g_Player[targetIdx].pos, TRUE);
	}
}


//******************************************
// �v���C���[�̍U��
//******************************************
void PlayerAttack(int Idx, int& attackedIdx, BOOL& attack)
{
	ENEMY* enemy = GetEnemy();
	static float slowIn;		//������ Slow In
	static float slowOut;		//������ Slow Out
	static BOOL enemyHitted;	//�G�l�~�[�ɍU������
	static BOOL critFail;		//�N���e�B�J�������s����

	// �ړ�����			
	float moveX = g_Player[Idx].patrolDest.x - g_Player[Idx].pos.x;
	float moveZ = g_Player[Idx].patrolDest.z - g_Player[Idx].pos.z;


	// �ړ��ʂ��v�Z����		// Calcular la magnitud del movimiento total
	float moveVectorMagnitude = sqrtf(moveX * moveX + moveZ * moveZ);

	if (moveVectorMagnitude < 3.0f)
	{
		static BOOL attacked;

		//�p�g���[������܂ł̎���
		if (g_Player[Idx].patrolDest.z == enemy[attackedIdx].pos.z - DISTANCE_BTW_ENEMY_ROWS)
		{
			if (attacked)	//���ɍU��������ԂȂ�A�ҋ@�̏ꏊ�ɖ߂�
			{
				{
					g_Player[Idx].patrolDest = g_FightPos[Idx];
					slowIn = SLOW_IN_FRAMES;
					slowOut = SLOW_OUT_DISTANCE;
					g_Player[Idx].animator.hiddenObj[0] = OBJ_IDX_AXE;	//�����\��
				}


			}
			else//�܂��U�����Ă��Ȃ�
			{
				if (attack == ATTACK_PUNCH)
				{
					PlayerPunchAttack(Idx, attackedIdx, critFail, enemyHitted, attacked);
				}
				else if (attack == ATTACK_AXE)
				{
					PlayerAxeAttack(Idx, attackedIdx, enemyHitted, attacked);
				}
			}
		}
		else  //�p�g���[���J�n
		{
			if (attacked)	//���ɍU������
			{
				//5
				attack = ATTACK_NONE;					//�U�����	
				attacked = FALSE;						//���ɍU������	�t���O
				critFail = FALSE;
				enemyHitted = FALSE;					//�G�l�~�[���_���[�W���󂯂��t���O
				g_Player[Idx].myTurn = FALSE;			//�^�[��		�t���O
				g_Player[Idx].turnFinished = TRUE;		//���̃��E���h�Ŋ���ACTION���s�����@�t���O
				g_Player[Idx].animator.nextAnim = P_ANIM_IDLE;
				g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].nextAnimation = P_ANIM_IDLE;

				attackedIdx = 0;
				
			}
			else
			{
				//1
				g_Player[Idx].patrolDest = enemy[attackedIdx].pos;
				g_Player[Idx].patrolDest.z -= DISTANCE_BTW_ENEMY_ROWS;
				slowIn = SLOW_IN_FRAMES;
				slowOut = SLOW_OUT_DISTANCE;
				
			}
		}

	}
	else
	{
		UpdatePMovement(moveX, moveZ, moveVectorMagnitude, Idx, FIGHT_SPEED, slowIn, slowOut);
	}

}



//******************************************
// �v���C���[�̃p���`�U��
//******************************************
void PlayerPunchAttack(int Idx, int attackedIdx, BOOL &critFail, BOOL& enemyHitted, BOOL& attacked)
{
	ENEMY* enemy = GetEnemy();

	if (g_Player[Idx].animator.curAnim != P_ANIM_ATTACK_PUNCH && g_Player[Idx].animator.nextAnim != P_ANIM_ATTACK_PUNCH && g_Player[Idx].animator.curAnim != P_ANIM_ATTACK_CRIT)//�U���A�j���[�V�������n�܂��Ă��Ȃ� 
	{
		g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].AnimTimeCnt = 0.0f;
		g_Player[Idx].animator.nextAnim = P_ANIM_ATTACK_PUNCH;
	}
	else//�U���A�j���[�V������
	{
		//�N���e�B�J���\���Ԕ͈�	//Critical chance range
		if (g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].AnimTimeCnt > g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].keyFrameNum - 2.0f &&
			g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].AnimTimeCnt < g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].keyFrameNum - 1.05f)//�U���A�j���[�V�������I��낤�Ƃ��Ă��鎞
		{

			SetActionIndicator(g_Player[Idx].pos, TRUE);	//��ʂɃN���e�B�J���`�����X��\��	//critical chance indicator on screen

			if ((GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B)) && !critFail)
			{
				g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].nextAnimation = P_ANIM_ATTACK_CRIT;
			}
		}
		//Enemy�ɓ���������
		if (g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].AnimTimeCnt > g_Player[Idx].animator.animation[P_ANIM_ATTACK_PUNCH].keyFrameNum - 1.05f)//�U���A�j���[�V�������I��낤�Ƃ��Ă��鎞
		{
			//�G�l�~�[���_���[�W���󂯂�	//Enemy damaged
			if (!enemyHitted)
			{
				EnemyDamaged(g_Player[Idx].attack, attackedIdx);
				SetDamageUI(enemy[attackedIdx].pos, g_Player[Idx].attack, TRUE, FALSE);
				enemyHitted = TRUE;
				SetActionIndicator(g_Player[Idx].pos, FALSE);
				SetHitParticle(enemy[attackedIdx].pos, FALSE);
				PlaySound(SOUND_LABEL_SE_Punch);
			}
		}
		else if (enemyHitted == TRUE && g_Player[Idx].animator.animation[P_ANIM_ATTACK_CRIT].AnimTimeCnt < 1.0f)
		{
			enemyHitted = FALSE;
		}
		else if (g_Player[Idx].animator.animation[P_ANIM_ATTACK_CRIT].AnimTimeCnt > g_Player[Idx].animator.animation[P_ANIM_ATTACK_CRIT].keyFrameNum - 7.5f)//�U���A�j���[�V�������I��낤�Ƃ��Ă��鎞
		{
			//Enemy damaged
			if (!enemyHitted)
			{
				EnemyDamaged((int)(g_Player[Idx].attack * 0.5f), attackedIdx);
				SetDamageUI(enemy[attackedIdx].pos, g_Player[Idx].attack + (int)(g_Player[Idx].attack * 0.5f), TRUE, TRUE);

				enemyHitted = TRUE;
				SetActionIndicator(g_Player[Idx].pos, FALSE);

				SetHitParticle(enemy[attackedIdx].pos, TRUE);

				PlaySound(SOUND_LABEL_SE_Punch);
			}
		}
		if (g_Player[Idx].animator.curAnim == P_ANIM_IDLE)
		{
			attacked = TRUE;
		}

		if ((GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B)) && !critFail)
		{
			critFail = TRUE;
		}
	}
			
}


//******************************************
// �v���C���[�̕��U��
//******************************************
void PlayerAxeAttack(int Idx, int attackedIdx, BOOL& enemyHitted, BOOL& attacked)
{
	ENEMY* enemy = GetEnemy();

	if (g_Player[Idx].animator.curAnim != P_ANIM_ATTACK_AXE && g_Player[Idx].animator.nextAnim != P_ANIM_ATTACK_AXE && g_Player[Idx].animator.curAnim != P_ANIM_ATTACK_CRIT)//�U���A�j���[�V�������n�܂��Ă��Ȃ� 
	{
		g_Player[Idx].animator.animation[P_ANIM_ATTACK_AXE].AnimTimeCnt = 0.0f;
		g_Player[Idx].animator.nextAnim = P_ANIM_ATTACK_AXE;
		g_Player[Idx].animator.hiddenObj[0] = -1;
	}
	else//�U���A�j���[�V������
	{
		//Enemy�ɓ���������
		if (g_Player[Idx].animator.animation[P_ANIM_ATTACK_AXE].AnimTimeCnt > g_Player[Idx].animator.animation[P_ANIM_ATTACK_AXE].keyFrameNum - 6.05f)//�U���A�j���[�V�������I��낤�Ƃ��Ă��鎞
		{
			//�G�l�~�[���_���[�W���󂯂�	//Enemy damaged
			if (!enemyHitted)
			{
				EnemyDamaged(g_Player[Idx].attack * 2, attackedIdx);
				SetDamageUI(enemy[attackedIdx].pos, g_Player[Idx].attack * 2, TRUE, FALSE);
				enemyHitted = TRUE;
				SetActionIndicator(g_Player[Idx].pos, FALSE);
				SetHitParticle(enemy[attackedIdx].pos, TRUE);
				SetHitParticle(enemy[attackedIdx].pos, TRUE);
				PlaySound(SOUND_LABEL_SE_Punch);
			}
		}
		if (g_Player[Idx].animator.curAnim == P_ANIM_IDLE)
		{
			attacked = TRUE;
		}
	}

}




//******************************************
// �v���C���[�ړ��@�X�V����
//******************************************
void UpdatePMovement(float moveX, float moveZ, float moveVectorMagnitude, int Idx, float speedMultiplier, float slowInFrames, float slowOutDistance)
{
	// ���K��
	if (moveVectorMagnitude > 1.0f)
	{
		moveX /= moveVectorMagnitude;
		moveZ /= moveVectorMagnitude;
	}

	//Speed �v�Z
	float speed = 1.0f;

	//Move Angle
	float moveAngle = atan2f(moveX, moveZ) + XM_PI;

	if (moveVectorMagnitude < slowOutDistance)	//Slow out
	{
		speed = moveVectorMagnitude / slowOutDistance;
		if (GetMode() == MODE_FIGHT) moveAngle = XM_PI;
	}
	else if (g_Player[Idx].patrolFrames < slowInFrames)	//Slow in
	{
		g_Player[Idx].patrolFrames++;
		speed = g_Player[Idx].patrolFrames / slowInFrames;
	}


	g_Player[Idx].pos.x += moveX * speed * speedMultiplier;
	g_Player[Idx].pos.z += moveZ * speed * speedMultiplier;


	float angleDifference = moveAngle - g_Player[Idx].rot.y;		// 2�̕����Ԃ̍ŒZ�p�x���v�Z����  (ES)Calcular el angulo mas corto entre las dos direcciones
	angleDifference = NormalizeAngle(angleDifference);		// �p�x���𐳋K������	(ES) Normalizar la diferencia angular
	g_Player[Idx].rot.y = lerp(g_Player[Idx].rot.y, g_Player[Idx].rot.y + angleDifference, LERP_FIGHT_FACTOR);	//���`���



	g_Player[Idx].animator.animation[P_ANIM_WALK].speed = speed * speedMultiplier * 1.5f;
	if (g_Player[Idx].animator.curAnim != P_ANIM_WALK) g_Player[Idx].animator.nextAnim = P_ANIM_WALK;

}


//******************************************
// �v���C���[���_���[�W���󂯂�
//******************************************
void PlayerDamaged(int damage, int playerIdx)
{
	if (damage == 0)
	{
		//block anim
		g_Player[playerIdx].animator.nextAnim = P_ANIM_BLOCK;
		return;
	}
	g_Player[playerIdx].HPcur -= damage;

	if (g_Player[playerIdx].HPcur <= 0)
	{
		//g_Enemy[enemyIdx].use = FALSE;
		g_Player[playerIdx].HPcur = 0;
		g_Player[playerIdx].dead = TRUE;
		g_Player[playerIdx].animator.nextAnim = P_ANIM_DEATH;
	}
	else
	{
		g_Player[playerIdx].animator.nextAnim = P_ANIM_HIT;
	}
}


//******************************************
// �G�l�~�[�^�[�Q�b�g�I�����[�h��TRUE�ɂ���
//******************************************
void SetSelectionEnemy(void)
{
	selectionEnemy = TRUE;
}

//******************************************
// �v���C���[�^�[�Q�b�g�I�����[�h��TRUE�ɂ���
//******************************************
void SetSelectionPlayer(void)
{
	selectionPlayer = TRUE;
}