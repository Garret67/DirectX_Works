//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "modelBinaryFBX.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "fade.h"
#include "player.h"
#include "fightUI.h"
#include "gameEnvironment.h"
#include "collision.h"
#include "sound.h"
#include "particle.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define	MODEL_ENEMY			"data/MODEL/enemy.obj"						// �ǂݍ��ރ��f����
#define	MODEL_SQUELETON		"data\\MODEL\\Squeleton_ModelData.bin"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(1.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��

#define SLOW_OUT_DISTANCE	(25.0f)						// SLOW OUT���n�܂鋗��
#define SLOW_IN_FRAMES		(30.0f)						// SLOW IN��������t���[����

#define PATROL_WAIT_FRAMES	(120.0f)					// SLOW IN��������t���[����
#define LERP_ROT_FACTOR		(0.1f)						// ���`��Ԃ̑���

#define FIGHT_POS_MAX		(5)							// �G�l�~�[�̗����ꏊ

#define FIGHT_SPEED			(1.5f)						// �G�l�~�[��FIGHT�ړ����x
#define GAME_SPEED			(0.5f)						// �G�l�~�[��GAME �ړ����x


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void LoadEnemyMapInfo(void);
void UpdateEnemyGameMode(void);
void UpdateEnemyFightMode(void);

void UpdateMovement(float moveX, float moveZ, float moveVectorMagnitude, int Idx, float speedMultiplier, float slowInFrames, float slowOutDistance);
BOOL Jumping(int Idx, int attackedIdx, XMFLOAT3 initPos, XMFLOAT3 finalPos);
void EnemyAttack(int Idx);
void EnemyAttack2(int Idx);

void EnemyCollider(int idx);

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY_MAP_INFO
{
	XMFLOAT3		pos;		// ���f���̈ʒu
	XMFLOAT3		rot;		// ���f���̌���(��])
	BOOL			use;		// �g�p�t���O
	int				spawnedEnemies;

	XMFLOAT3		patrolDest;	//�ړI�n

	BOOL			saved;		// �g�p�t���O
};


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[

static ENEMY_MAP_INFO	g_EnemyMapInfo[MAX_ENEMY];		// �G�l�~�[�̃Z�[�u�f�[�^

static XMFLOAT3			g_FightPos[FIGHT_POS_MAX] =		//�o�g���̍��W
{
	XMFLOAT3( 70.0f, 0.0f,  60.0f),
	XMFLOAT3(  0.0f, 0.0f,  60.0f),
	XMFLOAT3(-70.0f, 0.0f,  60.0f),
	XMFLOAT3( 35.0f, 0.0f, 110.0f),
	XMFLOAT3(-35.0f, 0.0f, 110.0f),
};

static int g_SpawnedEnemies = 0;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��

		g_Enemy[i].HP			= 25;
		g_Enemy[i].agility		= 10;
		g_Enemy[i].agilityCnt	= g_Enemy[i].agility;
		g_Enemy[i].attack		= 8;
		g_Enemy[i].deffense		= 10;
		g_Enemy[i].myTurn		= FALSE;
		g_Enemy[i].turnFinished = FALSE;
		g_Enemy[i].dead			= FALSE;

		g_Enemy[i].use = FALSE;			// TRUE:�����Ă�

		g_Enemy[i].patrolDest = g_Enemy[i].pos;
		g_Enemy[i].patrolFrames = 0;
		g_Enemy[i].patrolWaitFrames = 0;

		g_Enemy[i].shadowAlpha = 0.2f;
	}

	//���[�h�ɂ���ď���������
	switch (GetMode())
	{
	case MODE_GAME:
		g_SpawnedEnemies = MAX_ENEMY;

		for (int i = 0; i < g_SpawnedEnemies; i++)
		{
			InitModelBinaryFBX(MODEL_SQUELETON, &g_Enemy[i].model, &g_Enemy[i].modelData, &g_Enemy[i].animator);
			g_Enemy[i].load = TRUE;

			if(i < 5)		g_Enemy[i].pos = XMFLOAT3( GROUND_WIDTH , 0.0f, -400 + (i * 200.0f));
			else if(i < 10)	g_Enemy[i].pos = XMFLOAT3(-GROUND_WIDTH , 0.0f, -400 + ((i-5) * 200.0f));
			else if(i < 15)	g_Enemy[i].pos = XMFLOAT3(-400.0f + ((i-10) * 200.0f), 0.0f,  GROUND_HEIGHT);
			else if(i < 20)	g_Enemy[i].pos = XMFLOAT3(-400.0f + ((i-15) * 200.0f), 0.0f, -GROUND_HEIGHT);

			g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Enemy[i].patrolDest = g_Enemy[i].pos;
			g_Enemy[i].use = TRUE;

			g_Enemy[i].dissolve.Enable = FALSE;
			g_Enemy[i].dissolve.threshold = 0.0f;
			g_Enemy[i].shadowAlpha = 0.2f;
		}
		LoadEnemyMapInfo();

		break;

	case MODE_FIGHT:
		g_SpawnedEnemies = (rand() % 5) + 1;
		//g_SpawnedEnemies = 3;

		for (int i = 0; i < g_SpawnedEnemies; i++)
		{
			InitModelBinaryFBX(MODEL_SQUELETON, &g_Enemy[i].model, &g_Enemy[i].modelData, &g_Enemy[i].animator);
			g_Enemy[i].load = TRUE;

			g_Enemy[i].pos = g_FightPos[i];
			g_Enemy[i].rot = XMFLOAT3(0.0f, XM_PI, 0.0f);
			g_Enemy[i].patrolDest = g_Enemy[i].pos;
			g_Enemy[i].animator.animation[SQUELETON_JUMP].speed = 1.5f;
			g_Enemy[i].use = TRUE;

			g_Enemy[i].dissolve.Enable = TRUE;
			g_Enemy[i].dissolve.threshold = 0.0f;
			g_Enemy[i].shadowAlpha = 0.2f;

		}

		break;

	default:
		g_SpawnedEnemies = MAX_ENEMY;

		for (int i = 0; i < g_SpawnedEnemies; i++)
		{
			InitModelBinaryFBX(MODEL_SQUELETON, &g_Enemy[i].model, &g_Enemy[i].modelData, &g_Enemy[i].animator);
			g_Enemy[i].load = TRUE;

			g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, 0.0f, 20.0f);
			g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Enemy[i].patrolDest = g_Enemy[i].pos;
			g_Enemy[i].use = TRUE;

			g_Enemy[i].dissolve.Enable = FALSE;
			g_Enemy[i].dissolve.threshold = 0.0f;
			g_Enemy[i].shadowAlpha = 0.2f;
			break;
		}
	}
	

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UninitModelBinaryFBX(&g_Enemy[i].model, &g_Enemy[i].modelData, &g_Enemy[i].animator);
			g_Enemy[i].load = FALSE;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	switch (GetMode())
	{
	case MODE_GAME:
		UpdateEnemyGameMode();
		break;

	case MODE_FIGHT:
		UpdateEnemyFightMode();
		break;

	default:
		break;
	}


	//***********
	// �A�j���[�V����
	//***********
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use && g_Enemy[i].load)		// ���̃G�l�~�[���g���Ă���H
		{
			//�A�j���[�V��������
			if (g_Enemy[i].animator.curAnim != -1 && g_Enemy[i].animator.animationNum > 0)
			{
				UpdateModelBinaryFBX(&g_Enemy[i].modelData, g_Enemy[i].animator);
			}
		}
	}


#ifdef _DEBUG


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_BACK);


	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (!g_Enemy[i].use  || !g_Enemy[i].load) continue;

		SetDissolve(&g_Enemy[i].dissolve);
		SetAlphaTestEnable(g_Enemy[i].dissolve.Enable);

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y/* + XM_PI*/, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// ���f���`��
		//DrawModel(&g_Enemy[i].model);
		DrawModelBinaryFBX(&g_Enemy[i].model, g_Enemy[i].modelData, mtxWorld);

		SetDissolveEnable(FALSE);
		SetAlphaTestEnable(FALSE);


		//*************
		// �e�̕`��
		//*************


		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, 0.01f, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y/* + XM_PI*/, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, 0.01f, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		XMFLOAT4* MaterialDiffuse = new XMFLOAT4[100];
		int matIdx = 0;

		for (int z = 0; z < g_Enemy[i].model.BuffersNum; z++)
		{
			for (int j = 0; j < g_Enemy[i].model.Buffers[z].SubsetNum; j++)
			{
				MaterialDiffuse[matIdx] = g_Enemy[i].model.Buffers[z].SubsetArray[j].Material.Material.Diffuse;
				g_Enemy[i].model.Buffers[z].SubsetArray[j].Material.Material.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, g_Enemy[i].shadowAlpha);
				matIdx++;
			}
		}

		// �e�`��
		DrawModelBinaryFBX(&g_Enemy[i].model, g_Enemy[i].modelData, mtxWorld);


		matIdx = 0;
		for (int z = 0; z < g_Enemy[i].model.BuffersNum; z++)
		{
			for (int j = 0; j < g_Enemy[i].model.Buffers[z].SubsetNum; j++)
			{
				g_Enemy[i].model.Buffers[z].SubsetArray[j].Material.Material.Diffuse = MaterialDiffuse[matIdx];
				matIdx++;
			}
		}


		delete[] MaterialDiffuse;


	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

//=============================================================================
// �Z�[�u�G�l�~�[�f�[�^
//=============================================================================
void SaveEnemyMapInfo(int enemyIdx)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		g_EnemyMapInfo[i].pos		 = g_Enemy[i].pos;
		g_EnemyMapInfo[i].rot		 = g_Enemy[i].rot;
		g_EnemyMapInfo[i].use		 = g_Enemy[i].use;
		g_EnemyMapInfo[i].patrolDest = g_Enemy[i].patrolDest;
		g_EnemyMapInfo[i].saved		 = TRUE;
	}

	g_EnemyMapInfo[enemyIdx].pos.z -= GROUND_HEIGHT;
}

//=============================================================================
// ���[�h�G�l�~�[�f�[�^
//=============================================================================
void LoadEnemyMapInfo(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_EnemyMapInfo[i].saved)
		{
			g_Enemy[i].pos		  = g_EnemyMapInfo[i].pos;
			g_Enemy[i].rot		  = g_EnemyMapInfo[i].rot;
			g_Enemy[i].use		  = g_EnemyMapInfo[i].use;
			g_Enemy[i].patrolDest = g_EnemyMapInfo[i].patrolDest;

			g_EnemyMapInfo[i].saved = FALSE;
		}
	}
}

//=============================================================================
// �X�V�Q�[�����[�h�G�l�~�[
//=============================================================================
void UpdateEnemyGameMode(void)
{
	for (int i = 0; i < g_SpawnedEnemies; i++)
	{
		if (g_Enemy[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes
				// �ړ�����			
			float moveX = g_Enemy[i].patrolDest.x - g_Enemy[i].pos.x;
			float moveZ = g_Enemy[i].patrolDest.z - g_Enemy[i].pos.z;


			// Calcular la magnitud del movimiento total
			float moveVectorMagnitude = sqrtf(moveX * moveX + moveZ * moveZ);

			if (moveVectorMagnitude < 2)
			{
				//�p�g���[������܂ł̎���
				if (g_Enemy[i].patrolFrames < SLOW_IN_FRAMES + g_Enemy[i].patrolWaitFrames)
				{
					g_Enemy[i].patrolFrames++;
				}
				else  //�p�g���[���J�n
				{
					PLAYER* player = GetPlayer();
					g_Enemy[i].patrolDest.x = (float)(rand() % 2000 - 1000) + player->pos.x;
					g_Enemy[i].patrolDest.z = (float)(rand() % 2000 - 1000) + player->pos.z;

					g_Enemy[i].patrolWaitFrames = (float)(rand() % 300 + 60);
					g_Enemy[i].patrolFrames = 0;
				}

				if (g_Enemy[i].animator.curAnim != SQUELETON_IDLE) g_Enemy[i].animator.nextAnim = SQUELETON_IDLE;

			}
			else
			{
				UpdateMovement(moveX, moveZ, moveVectorMagnitude, i, GAME_SPEED, SLOW_IN_FRAMES, 7.0f);
			}


			EnemyCollider(i);
		}
	}




}

//=============================================================================
// �X�V�o�g�����[�h�G�l�~�[
//=============================================================================
void UpdateEnemyFightMode(void)
{
	for (int i = 0; i < g_SpawnedEnemies; i++)
	{
		if (g_Enemy[i].use && g_Enemy[i].load)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes
			//�N�̔Ԃł��Ȃ��i���̃G�l�~�[�ȊO�j
			if (g_Enemy[i].myTurn)	//���̃G�l�~�[�̃^�[���ł��Ȃ���
			{
				EnemyAttack(i);		//�����U��

				//EnemyAttack2(i);	//�W�����v�U��
			}

			//�G�l�~�[���|��Ă���i�����鏈���j
			if (g_Enemy[i].dead)
			{
				if (g_Enemy[i].animator.animation[SQUELETON_DEATH].AnimTimeCnt > g_Enemy[i].animator.animation[SQUELETON_DEATH].keyFrameNum - 1.5f)
				{
					g_Enemy[i].dissolve.threshold += 0.01f;

					//�e����������
					if (g_Enemy[i].dissolve.threshold <= 0.2f)	g_Enemy[i].shadowAlpha = 0.2f - g_Enemy[i].dissolve.threshold;

					//�G�l�~�[������
					if (g_Enemy[i].dissolve.threshold >= 1.0f) g_Enemy[i].use = FALSE;

				}
			}

		}
	}




}

//=============================================================================
// �X�V�G�l�~�[�̓���
//=============================================================================
void UpdateMovement(float moveX, float moveZ, float moveVectorMagnitude, int Idx, float speedMultiplier, float slowInFrames, float slowOutDistance)
{
	
	// ���v�̑傫����1���傫���ꍇ�A���K������B// Si la magnitud total es mayor que 1, normalizar
	if (moveVectorMagnitude > 1.0f)
	{
		moveX /= moveVectorMagnitude;
		moveZ /= moveVectorMagnitude;
	}

	//Speed �v�Z
	float speed = 1.0f;

	//Move Angle
	float moveAngle = atan2f(moveX, moveZ);

	if (moveVectorMagnitude < slowOutDistance)	//Slow out
	{
		speed = moveVectorMagnitude / slowOutDistance;
		if (GetMode() == MODE_FIGHT) moveAngle = XM_PI;
	}
	else if (g_Enemy[Idx].patrolFrames < slowInFrames)	//Slow in
	{
		g_Enemy[Idx].patrolFrames++;
		speed = g_Enemy[Idx].patrolFrames / slowInFrames;
	}
		

	g_Enemy[Idx].pos.x += moveX * speed * speedMultiplier;
	g_Enemy[Idx].pos.z += moveZ * speed * speedMultiplier;


	float angleDifference = moveAngle - g_Enemy[Idx].rot.y;		// 2�̕����Ԃ̍ŒZ�p�x���v�Z����  (ES)Calcular el angulo mas corto entre las dos direcciones
	angleDifference = NormalizeAngle(angleDifference);		// �p�x���𐳋K������	(ES) Normalizar la diferencia angular
	g_Enemy[Idx].rot.y = lerp(g_Enemy[Idx].rot.y, g_Enemy[Idx].rot.y + angleDifference, LERP_ROT_FACTOR);	//���`���



	g_Enemy[Idx].animator.animation[SQUELETON_WALK].speed = speed * speedMultiplier * 1.5f;
	if (g_Enemy[Idx].animator.curAnim != SQUELETON_WALK) g_Enemy[Idx].animator.nextAnim = SQUELETON_WALK;
}



//=============================================================================
// �G�l�~�[�̍U���i�����j
//=============================================================================
void EnemyAttack(int Idx)
{
	PLAYER* player = GetPlayer();
	static float slowIn;
	static float slowOut;
	static int attackedIdx;
	static BOOL playerHitted;

	// �ړ�����			
	float moveX = g_Enemy[Idx].patrolDest.x - g_Enemy[Idx].pos.x;
	float moveZ = g_Enemy[Idx].patrolDest.z - g_Enemy[Idx].pos.z;


	// �ړ��ʂ��v�Z����@�@// Calcular la magnitud del movimiento total
	float moveVectorMagnitude = sqrtf(moveX * moveX + moveZ * moveZ);

	if (moveVectorMagnitude < 3.0f)
	{
		static BOOL attacked;

		//�p�g���[������܂ł̎���
		if (g_Enemy[Idx].patrolDest.z == player[attackedIdx].pos.z + 50.0f)
		{
			if (attacked)	//���ɍU������
			{
				if (g_FightPos[Idx].z > g_FightPos->z + 10.0f)	//�G�l�~�[�����̗�ɖ߂肽��������
				{
					g_Enemy[Idx].patrolDest = g_FightPos[Idx];
					g_Enemy[Idx].patrolDest.z -= 60.0f;
					slowIn = SLOW_IN_FRAMES;
					slowOut = g_FightPos[Idx].z == g_FightPos->z ? SLOW_OUT_DISTANCE : 1.0f;	//�擪�̗�̃G�l�~�[��������ASLOW OUT����;
				}
				else//�G�l�~�[���擪�̗�ɖ߂肽��������
				{
					g_Enemy[Idx].patrolDest = g_FightPos[Idx];
					slowIn = SLOW_IN_FRAMES;
					slowOut = SLOW_OUT_DISTANCE;
				}
				

			}
			else//�܂��U�����Ă��Ȃ�
			{
				if (g_Enemy[Idx].animator.curAnim != SQUELETON_ATTACK_PUNCH && g_Enemy[Idx].animator.nextAnim != SQUELETON_ATTACK_PUNCH)//�U���A�j���[�V�������n�܂��Ă��Ȃ� 
				{
					g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].AnimTimeCnt = 0.0f;
					g_Enemy[Idx].animator.nextAnim = SQUELETON_ATTACK_PUNCH;
				}
				else//�U���A�j���[�V������
				{
					static BOOL attackBlocked;
					static BOOL attackBlockedFailed;
					//
					if (g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].AnimTimeCnt > g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].keyFrameNum - 5.0f &&
						g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].AnimTimeCnt < g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].keyFrameNum - 1.05f)//�U���A�j���[�V�������I��낤�Ƃ��Ă��鎞
					{
						//�h�� �`�����X�@defence chance indicator on screen
						SetActionIndicator(player[attackedIdx].pos, TRUE);
						if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B))
						{
							attackBlocked = TRUE;
						}

					}
					else if (g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].AnimTimeCnt > g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].keyFrameNum - 1.05f)//�U���A�j���[�V�������I��낤�Ƃ��Ă��鎞
					{
						if (!playerHitted)
						{
							//�h��	attack blocked
							if (attackBlocked && !attackBlockedFailed)
							{
								PlayerDamaged(0, attackedIdx);						//�h��	attack blocked
								SetDamageUI(player[attackedIdx].pos, 0, TRUE, FALSE);
								PlaySound(SOUND_LABEL_SE_Slap);
							}
							//�U�����������
							else
							{
								PlayerDamaged(g_Enemy[Idx].attack, attackedIdx);	//�U�����������
								SetDamageUI(player[attackedIdx].pos, g_Enemy[Idx].attack, TRUE, FALSE);
								SetHitParticle(player[attackedIdx].pos, FALSE);
								PlaySound(SOUND_LABEL_SE_Punch);
							}

							SetActionIndicator(player[attackedIdx].pos, FALSE);

							playerHitted = TRUE;
							attackBlocked = FALSE;
							attackBlockedFailed = FALSE;
						}
					}
					if (g_Enemy[Idx].animator.curAnim == SQUELETON_IDLE)
					{
						attacked = TRUE;
					}

					if ((GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B)) && !attackBlockedFailed && !attackBlocked)
					{
						attackBlockedFailed = TRUE;
					}

				}
			}
		}
		else  //�p�g���[���J�n
		{
			if (attacked)	//���ɍU������
			{
				if (g_Enemy[Idx].patrolDest.z != g_FightPos[Idx].z)
				{//4444
					g_Enemy[Idx].patrolDest = g_FightPos[Idx];
					slowIn = 1.0f;
					slowOut = SLOW_OUT_DISTANCE;
				}
				else
				{//5555
					attacked = FALSE;
					playerHitted = FALSE;
					g_Enemy[Idx].myTurn = FALSE;
					g_Enemy[Idx].turnFinished = TRUE;
					g_Enemy[Idx].animator.nextAnim = SQUELETON_IDLE;
				}
			}
			else
			{
				if (g_Enemy[Idx].pos.z > g_FightPos->z + 10.0f)	//�G�l�~�[�����̗�ɂ�����A������Ɛi��ł���v���C���[�̏��ɍs��
				{
					g_Enemy[Idx].patrolDest = g_FightPos[Idx];
					g_Enemy[Idx].patrolDest.z -= 60.0f;
					slowIn = SLOW_IN_FRAMES;
					slowOut = 1.0f;
				}
				else
				{
					BOOL invalidPlayer = TRUE;
					while (invalidPlayer)
					{
						attackedIdx = rand() % MAX_PLAYER;
						if (player[attackedIdx].use && player[attackedIdx].load && !player[attackedIdx].dead) invalidPlayer = FALSE;
					}

					g_Enemy[Idx].patrolDest = player[attackedIdx].pos;
					g_Enemy[Idx].patrolDest.z += 50.0f;
					slowIn = g_FightPos[Idx].z == g_FightPos->z ? SLOW_IN_FRAMES : 1.0f;	//�擪�̗�̃G�l�~�[��������ASLOW IN����
					slowOut = SLOW_OUT_DISTANCE;
				}
			}
		}

	}
	else
	{
		UpdateMovement(moveX, moveZ, moveVectorMagnitude, Idx, FIGHT_SPEED, slowIn, slowOut);
	}

}


//=============================================================================
// �G�l�~�[�̍U���i�W�����v�j
//=============================================================================
void EnemyAttack2(int Idx)
{
	PLAYER* player = GetPlayer();

	static BOOL attacked;
	static BOOL jumped;
	static int attackedIdx;
	static BOOL startJump;

	if (jumped/*g_Enemy[Idx].pos.z - 5 <= player[attackedIdx].pos.z + 50.0f*/) //�v���C���[�̑O�ɂ���
	{
		if (attacked)	//���ɍU������
		{
			if (Jumping(Idx, attackedIdx, player[attackedIdx].pos, g_FightPos[Idx]))
			{//333
				attacked = FALSE;
				jumped = FALSE;
				startJump = FALSE;
				g_Enemy[Idx].myTurn = FALSE;
				g_Enemy[Idx].turnFinished = TRUE;
				g_Enemy[Idx].animator.nextAnim = SQUELETON_IDLE;
			}
		}
		else//�܂��U�����Ă��Ȃ�
		{//222
			if (g_Enemy[Idx].animator.curAnim != SQUELETON_ATTACK_PUNCH && g_Enemy[Idx].animator.nextAnim != SQUELETON_ATTACK_PUNCH)//�U���A�j���[�V�������n�܂��Ă��Ȃ� 
			{
				g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].AnimTimeCnt = 0.0f;
				g_Enemy[Idx].animator.nextAnim = SQUELETON_ATTACK_PUNCH;
			}
			else//�U���A�j���[�V������
			{
				if (g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].AnimTimeCnt > g_Enemy[Idx].animator.animation[SQUELETON_ATTACK_PUNCH].keyFrameNum - 1.05f)//�U���A�j���[�V�������I��낤�Ƃ��Ă��鎞
				{
					//player[attackedIdx].animator.nextAnim = P_ANIM_HIT;
					PlayerDamaged(g_Enemy[Idx].attack, attackedIdx);
					SetDamageUI(player[attackedIdx].pos, g_Enemy[Idx].attack, TRUE, FALSE);
					SetHitParticle(player[attackedIdx].pos, FALSE);
					attacked = TRUE;
				}
				//if (g_Enemy[Idx].animator.curAnim == SQUELETON_IDLE)
				//{
				//	attacked = TRUE;
				//}
			}

		}
	}
	else  //�p�g���[���J�n
	{
		if (attacked)	//���ɍU������
		{
			attacked = FALSE;
			//g_Enemy[Idx].attacking = FALSE;
			g_Enemy[Idx].animator.nextAnim = SQUELETON_IDLE;
		}
		else
		{//111
			if (!startJump)
			{
				BOOL invalidPlayer = TRUE;
				while (invalidPlayer)
				{
					attackedIdx = rand() % MAX_PLAYER;
					if (player[attackedIdx].use && player[attackedIdx].load && !player[attackedIdx].dead) invalidPlayer = FALSE;
				}
				startJump = TRUE;
			}

			jumped = Jumping(Idx, attackedIdx, g_FightPos[Idx], player[attackedIdx].pos);
		}
	}
}


//=============================================================================
// �X�V�G�l�~�[�̃W�����v
//=============================================================================
BOOL Jumping(int Idx, int attackedIdx, XMFLOAT3 initPos, XMFLOAT3 finalPos)
{
	//PLAYER* player = GetPlayer();

	if (g_Enemy[Idx].animator.curAnim != SQUELETON_JUMP && g_Enemy[Idx].animator.nextAnim != SQUELETON_JUMP)
	{
		g_Enemy[Idx].animator.nextAnim = SQUELETON_JUMP;
		g_Enemy[Idx].animator.animation[SQUELETON_JUMP].AnimTimeCnt = 0.0f;
	}

	float t = g_Enemy[Idx].animator.animation[SQUELETON_JUMP].AnimTimeCnt / (g_Enemy[Idx].animator.animation[SQUELETON_JUMP].keyFrameNum - 1);

	XMFLOAT3 p0 = initPos;
	if (p0.z < 0)p0.z += 50.0f;
	XMFLOAT3 p1 = p0;
	XMFLOAT3 p2 = finalPos;
	if(p2.z < 0)p2.z += 50.0f;
	XMFLOAT3 p3 = p2;

	float moveX = p3.x - p0.x;
	float moveZ = p3.z - p0.z;
	float moveVectorDistance = sqrtf(moveX * moveX + moveZ * moveZ);

	p1.y += moveVectorDistance * 0.3f;
	p2.y += moveVectorDistance * 0.3f;

	if (t >= 0.97f)
	{
		t = 1;
		//return TRUE;//�W�������ς�
	}
	g_Enemy[Idx].pos = BezierFunction(t, p0, p1, p2, p3);

	return t >= 1 ? TRUE : FALSE; //�W��������
}

//**************************
// ���������G�l�~�[���擾
//**************************
int GetSpawnedEnemies(void)
{
	return g_SpawnedEnemies;
}

//**************************
// �G�l�~�[���U�������炤
//**************************
void EnemyDamaged(int damage, int enemyIdx)
{
	if (g_Enemy[enemyIdx].HP <= 0)return;

	g_Enemy[enemyIdx].HP -= damage;

	if (g_Enemy[enemyIdx].HP <= 0)
	{
		g_Enemy[enemyIdx].dead = TRUE;
		g_Enemy[enemyIdx].animator.nextAnim = SQUELETON_DEATH;
		if (g_Enemy[enemyIdx].animator.curAnim == SQUELETON_HIT) g_Enemy[enemyIdx].animator.animation[SQUELETON_HIT].nextAnimation = SQUELETON_DEATH;
	}
	else
	{
		if (g_Enemy[enemyIdx].animator.curAnim == SQUELETON_HIT || g_Enemy[enemyIdx].animator.nextAnim == SQUELETON_HIT)
		{
			g_Enemy[enemyIdx].animator.animation[SQUELETON_HIT].AnimTimeCnt = 0;
		}
		else
		{
			g_Enemy[enemyIdx].animator.nextAnim = SQUELETON_HIT;
		}
	}
}

//**************************
// �G�l�~�[�̍Ĕz�u
//**************************
void EnemyRelocation(BOOL Xmov, BOOL positiveMov, float limitPos)
{
	//���ړ�
	if (Xmov)
	{
		//�E�ړ�
		if (positiveMov)
		{
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].pos.x < limitPos)
				{
					g_Enemy[i].pos.x		+= GROUND_WIDTH * 2;
					g_Enemy[i].patrolDest.x += GROUND_WIDTH * 2;
				}
			}
		}
		//���ړ�
		else
		{
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].pos.x > limitPos)
				{
					g_Enemy[i].pos.x		-= GROUND_WIDTH * 2;
					g_Enemy[i].patrolDest.x -= GROUND_WIDTH * 2;
				}
			}
		}

	}
	//�c�ړ�
	else
	{
		//��ړ�
		if (positiveMov)
		{
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].pos.z < limitPos)
				{
					g_Enemy[i].pos.z		+= GROUND_HEIGHT * 2;
					g_Enemy[i].patrolDest.z += GROUND_HEIGHT * 2;
				}
			}
		}
		//���ړ�
		else
		{
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				if (g_Enemy[i].pos.z > limitPos)
				{
					g_Enemy[i].pos.z		-= GROUND_HEIGHT * 2;
					g_Enemy[i].patrolDest.z -= GROUND_HEIGHT * 2;
				}
			}
		}

	}
}

//****************************
//�@�G�l���M�[�̓����蔻��
//****************************
void EnemyCollider(int idx)
{
	//�G�l�~�[���m�̓����蔻��
	for (int e = 0; e < MAX_ENEMY; e++)
	{
		if ((e == idx) || (g_Enemy[e].use == FALSE))
		{
			continue;
		}

		BOOL ans = TRUE;
		while (ans == TRUE)
		{
			ans = CollisionBC(g_Enemy[idx].pos, g_Enemy[e].pos, g_Enemy[idx].size,
				g_Enemy[e].size);

			// �������Ă���H
			if (ans == TRUE)
			{
				XMVECTOR currentEnemy = XMLoadFloat3(&g_Enemy[e].pos);                         //���̃G�l�~�[�̍��W
				XMVECTOR vectorToEnemy = XMLoadFloat3(&g_Enemy[idx].pos) - currentEnemy;       //2�ڂ̃G�l�~�[����1�ڂ̃G�l�~�[�܂ł�Vector

				float angle = atan2f(vectorToEnemy.m128_f32[1], vectorToEnemy.m128_f32[0]);

				float XMove = cosf(angle);
				float YMove = sinf(angle);

				g_Enemy[idx].pos.x += XMove;
				g_Enemy[idx].pos.z += YMove;
			}
		}
	}
}