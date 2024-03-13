//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"

#include "gameEnvironment.h"
#include "write.h"
#include "fightUI.h"
#include "pauseMenu.h"

#include "modelBinaryFBX.h"
#include "player.h"
#include "enemy.h"
#include "particle.h"
#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static int 	g_PauseMode;	// �|�[�YON/OFF




//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	InitGameEnvironment();	//���̏�����

	InitPlayer();			// �v���C���[�̏�����
	InitEnemy();			// �G�l�~�[�̏�����
	InitParticle();			// �p�[�e�B�N���̏�����

	InitDirectWrite();
	InitFightUI();			// �X�R�A�̏�����
	InitPauseMenu();		// �X�R�A�̏�����

	g_PauseMode = PAUSE_MODE_OFF;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{
	UninitParticle();		// �p�[�e�B�N���̏I������
	UninitEnemy();			// �G�l�~�[�̏I������
	UninitPlayer();			// �v���C���[�̏I������

	UninitPauseMenu();
	UninitFightUI();		// �X�R�A�̏I������
	InitDirectWrite();

	UninitGameEnvironment(); //���̏I������
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
	if ((GetKeyboardTrigger(DIK_P) || IsButtonTriggered(0, BUTTON_START)) && g_PauseMode != PAUSE_MODE_SCREEN)
	{
		g_PauseMode = g_PauseMode ? FALSE : PAUSE_MODE_MENU;
		MoveMenu();
		PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
	}


	if(g_PauseMode == PAUSE_MODE_SCREEN)return;

	if (g_PauseMode == PAUSE_MODE_MENU)
	{
		UpdatePauseMenu();		// ���j���[�̍X�V����
		return;
	}
		
		

	UpdateGameEnvironment();	//���̏I������

	UpdatePlayer();				// �v���C���[�̍X�V����
	UpdateEnemy();				// �G�l�~�[�̍X�V����
	UpdateParticle();			// �p�[�e�B�N���̍X�V����
	CheckHit();					// �����蔻�菈��
	UpdateFightUI();				// �X�R�A�̍X�V����
	UpdatePauseMenu();			// PAUSE MENU�̍X�V����

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame3D(void)
{
	// 3D�̕���`�悷�鏈��
	DrawGameEnvironment();	//���̕`�揈��

	DrawEnemy();			// �G�l�~�[�̕`�揈��
	DrawPlayer();			// �v���C���[�̕`�揈��
	DrawParticle();			// �p�[�e�B�N���̕`�揈��
}

void DrawGame2D(void)
{
	// 2D�̕���`�悷�鏈��
	SetDepthEnable(FALSE);	// Z��r�Ȃ�
	SetLightEnable(FALSE);	// ���C�e�B���O�𖳌�

	DrawFightUI();			// �X�R�A�̕`�揈��
	DrawPauseMenu();			// �X�R�A�̕`�揈��

	SetLightEnable(TRUE);	// ���C�e�B���O��L����
	SetDepthEnable(TRUE);	// Z��r����

}


void DrawGame(void)
{
	XMFLOAT3 pos;

	switch (GetMode())
	{
	case MODE_GAME:
		// �v���C���[���_
		pos = GetPlayer()->pos;
		pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
		SetCameraAT(pos);
		SetCamera();
		break;

	case MODE_FIGHT:
		// �v���C���[���_
		pos = XMFLOAT3(0.0f, 0.0f, /*-2*/0.0f);
		SetCameraAT(pos);
		SetCamera();

		break;

	default:
		break;
	}


	SetViewPort(TYPE_FULL_SCREEN);
	DrawGame3D();
	DrawGame2D();

}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// �G�l�~�[�̃|�C���^�[��������
	PLAYER *player = GetPlayer();	// �v���C���[�̃|�C���^�[��������
	BILLBOARD *tree = GetTree();	// �e�̃|�C���^�[��������

	// �G�ƃv���C���[�L����
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//�G�̗L���t���O���`�F�b�N����
		if (enemy[i].use == FALSE)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
		{
			g_PauseMode = PAUSE_MODE_SCREEN;
			SaveEnemyMapInfo(i);
			SavePlayerGameInfo();

			SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_FIGHT);
		}
	}


	if (GetMode() == MODE_GAME)
	{
		for (int i = 0; i < MAX_TREE; i++)
		{
			if (tree[i].use == FALSE)
				continue;


			XMFLOAT3 treePos = tree[i].pos;
			treePos.x -= 8.0f;
			treePos.z -= 8.0f;

			BOOL ans = TRUE;
			while (ans == TRUE)
			{
				ans = CollisionBC(player[0].pos, treePos, player[0].size, 15);

				// �������Ă���H
				if (ans == TRUE)
				{
					XMVECTOR currentEnemy = XMLoadFloat3(&tree[i].pos);                         //���̃G�l�~�[�̍��W
					XMVECTOR vectorToEnemy = XMLoadFloat3(&player[0].pos) - currentEnemy;       //2�ڂ̃G�l�~�[����1�ڂ̃G�l�~�[�܂ł�Vector

					float angle = atan2f(vectorToEnemy.m128_f32[1], vectorToEnemy.m128_f32[0]);

					float XMove = cosf(angle);
					float YMove = sinf(angle);

					player[0].pos.x += XMove;
					player[0].pos.z += YMove;
				}
			}
		}
	}
}

//=============================================================================
// �|�[�Y���[�h���擾
//=============================================================================
int GetPauseMode(void)
{
	return g_PauseMode;
}

//=============================================================================
// �|�[�Y���[�h��ݒ�
//=============================================================================
void SetPauseMode(int pauseMode)
{
	g_PauseMode = pauseMode;
}