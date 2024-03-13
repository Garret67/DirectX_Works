//=============================================================================
//
// �G�l�~�[���� [spawnController.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "spawnController.h"

#include "enemyFlyBulletOne.h"
#include "enemyFlyBounce.h"
#include "enemyKnight.h"
#include "enemyBall.h"
#include "enemyPatrol.h"
#include "enemySlimeKing.h"
#include "enemyBoss.h"

#include "bg.h"
#include "platforms.h"
#include "player.h"
#include "sound.h"
#include "result.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//#define TEXTURE_MAX					(4)		// �e�N�X�`���̐�

#define TEXTURE_ANIM_SPRITES_MAX	(3)		// 1�̃A�j���[�V�����̓X�v���C�g�����iX)
//#define TEXTURE_ANIM_MAX			(3)		// �A�j���[�V�����͂�����			  �iY)
#define ANIM_WAIT					(30)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define ROUND_WAIT_TIME				(600)

#define TEX_ROUND_DIV_X				(9)		//Round texture division in X axis


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetSpawnElevator(int spawnEnemyIndex);
void SpawnElevatorUpdate(void);

void DrawRoundUI(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[SPAWN_TEX_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[SPAWN_TEX_MAX] = {
	"data/TEXTURE/Enemy/Spawn/elevator.png",
	"data/TEXTURE/Enemy/Spawn/EnemyBox.png",
	"data/TEXTURE/Player/RoundUI.png",
};


static BOOL				g_Load = FALSE;			// ���������s�������̃t���O


static int				g_Round;
static int				g_RoundKills;
static int				g_RoundEnemies;
static int				g_RoundWaitTime;
static int				g_RoundWaitTimeCnt;

static int				g_EnemiesSpawned;

static SPAWNER			g_Elevator[SPAWNER_MAX];
static SPAWNER			g_EnemyBox[SPAWNER_MAX];

static SPAWN_ENEMY		g_enemySpawner[SPAWN_ENEMY_MAX];
static SPAWN_PLATFORM	g_PlatformSpawner[SPAWN_PLATFORM_MAX];

float ElevatorInitialHeight;
float EnemyBoxInitialHeight;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSpawnController(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < SPAWN_TEX_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	for (int i = 0; i < SPAWNER_MAX; i++)
	{
		g_Elevator[i].use = FALSE;
		g_Elevator[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Elevator[i].movePosHeight = 0.0f;
		g_Elevator[i].w = 100.0f;
		g_Elevator[i].h = 700.0f;
		g_Elevator[i].currentSprite = 0;
		g_Elevator[i].countAnim = 0;
		g_Elevator[i].texNo = SPAWN_TEX_ELEVATOR;
		g_Elevator[i].spawnEnemyIndex = -1;


		g_EnemyBox[i].use = FALSE;
		g_EnemyBox[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyBox[i].movePosHeight = 0.0f;
		g_EnemyBox[i].w = 125.0f; 
		g_EnemyBox[i].h = 215.0f; 
		g_EnemyBox[i].currentSprite = 0;
		g_EnemyBox[i].countAnim = -60;
		g_EnemyBox[i].texNo = SPAWN_TEX_ENEMY_BOX;
		g_EnemyBox[i].spawnEnemyIndex = -1;
	}
	BG* bg = GetBG();

	ElevatorInitialHeight = bg->h + (g_Elevator[0].h / 2);
	EnemyBoxInitialHeight = bg->h + (g_EnemyBox[0].h / 2);


	for (int i = 0; i < SPAWN_ENEMY_MAX; i++)
	{
		g_enemySpawner[i].spawned = FALSE;
	}

	g_Round				= 0;
	g_RoundKills		= 0;
	g_RoundEnemies		= 0;
	g_RoundWaitTime		= 0;
	g_RoundWaitTimeCnt	= 0;

	if (GetRetryRound() > 0)
	{
		g_Round = GetRetryRound() - 1;
	}



	//					    Spawned	  Round   KillCondition						SpawnPos
	g_PlatformSpawner[0]  = {FALSE,		1,			5,			XMFLOAT3(500.0f,			bg->h - 500.0f, 0.0f)};
	g_PlatformSpawner[1]  = {FALSE,		1,			5,			XMFLOAT3(bg->w - 500.0f,	bg->h - 500.0f, 0.0f)};
						  
	g_PlatformSpawner[2]  = {FALSE,		3,			0,			XMFLOAT3(SCREEN_WIDTH / 2,	bg->h - 300.0f, 0.0f)};
	g_PlatformSpawner[3]  = {FALSE,		3,			0,			XMFLOAT3(SCREEN_WIDTH,		bg->h - 300.0f, 0.0f)};
						  
	g_PlatformSpawner[4]  = {FALSE,		5,			1,			XMFLOAT3(500.0f,			bg->h - 500.0f, 0.0f)};
	g_PlatformSpawner[5]  = {FALSE,		5,			1,			XMFLOAT3(bg->w - 500.0f,	bg->h - 500.0f, 0.0f)};
						  
	g_PlatformSpawner[6]  = {FALSE,		6,			3,			XMFLOAT3(500.0f,			bg->h - 300.0f, 0.0f)};
	g_PlatformSpawner[7]  = {FALSE,		6,			3,			XMFLOAT3(bg->w - 500.0f,	bg->h - 300.0f, 0.0f)};
						  
	g_PlatformSpawner[8]  = {FALSE,		7,			0,			XMFLOAT3(bg->w / 2,			bg->h - 300.0f, 0.0f)};
	g_PlatformSpawner[9]  = {FALSE,		7,			0,			XMFLOAT3(bg->w - 500.0f,	bg->h - 450.0f, 0.0f)};
	g_PlatformSpawner[10] = {FALSE,		7,			0,			XMFLOAT3(500.0f,			bg->h - 450.0f, 0.0f)};



	
	//					 Spawned  Round      Enemy		  KillCondition				SpawnPos
	g_enemySpawner[0]  = {FALSE,    1,   SPAWN_PATROL,		   0,			XMFLOAT3(bg->w / 2,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[1]  = {FALSE,    1,   SPAWN_PATROL,		   1,			XMFLOAT3(500.0f,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[2]  = {FALSE,    1,   SPAWN_PATROL,		   1,			XMFLOAT3(bg->w - 500.0f, bg->h - 150.0f, 0.0f)},
	g_enemySpawner[3]  = {FALSE,    1,   SPAWN_BALL,		   3,			XMFLOAT3(bg->w - 500.0f, bg->h - 150.0f, 0.0f)},
	g_enemySpawner[4]  = {FALSE,    1,   SPAWN_BALL,		   3,			XMFLOAT3(500.0f,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[5]  = {FALSE,    1,   SPAWN_PATROL,		   5,			XMFLOAT3(bg->w - 300.0f, bg->h - 150.0f, 0.0f)},
	g_enemySpawner[6]  = {FALSE,    1,   SPAWN_PATROL,		   5,			XMFLOAT3(300.0f,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[7]  = {FALSE,    1,   SPAWN_BALL,		   5,			XMFLOAT3(bg->w - 500.0f, bg->h - 575.0f, 0.0f)},
	g_enemySpawner[8]  = {FALSE,    1,   SPAWN_BALL,		   5,			XMFLOAT3(500.0f,		 bg->h - 575.0f, 0.0f)},

	g_enemySpawner[9]  = {FALSE,    2,   SPAWN_FLY_BOUNCE, 	   0,			XMFLOAT3(bg->w / 2,		 bg->h - 575.0f, 0.0f)},
	g_enemySpawner[10] = {FALSE,    2,   SPAWN_FLY_BOUNCE, 	   0,			XMFLOAT3(bg->w - 500.0f, bg->h - 575.0f, 0.0f)},
	g_enemySpawner[11] = {FALSE,    2,   SPAWN_FLY_BOUNCE, 	   0,			XMFLOAT3(500.0f,		 bg->h - 575.0f, 0.0f)},
	g_enemySpawner[12] = {FALSE,    2,   SPAWN_FLY_BOUNCE, 	   2,			XMFLOAT3(bg->w - 500.0f, bg->h - 575.0f, 0.0f)},
	g_enemySpawner[13] = {FALSE,    2,   SPAWN_FLY_BOUNCE, 	   2,			XMFLOAT3(500.0f,		 bg->h - 575.0f, 0.0f)},
	g_enemySpawner[14] = {FALSE,    2,   SPAWN_BALL,		   2,			XMFLOAT3(bg->w - 500.0f, bg->h - 150.0f, 0.0f)},
	g_enemySpawner[15] = {FALSE,    2,   SPAWN_BALL,		   2,			XMFLOAT3(500.0f,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[16] = {FALSE,    2,   SPAWN_PATROL,		   3,			XMFLOAT3(300.0f,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[17] = {FALSE,    2,   SPAWN_PATROL,		   4,			XMFLOAT3(bg->w - 300.0f, bg->h - 150.0f, 0.0f)},

	g_enemySpawner[18] = {FALSE,    3,   SPAWN_FLY_ONE, 	   0,			XMFLOAT3(500.0f,		 bg->h - 575.0f, 0.0f)},
	g_enemySpawner[19] = {FALSE,    3,   SPAWN_FLY_ONE, 	   0,			XMFLOAT3(bg->w - 500.0f, bg->h - 575.0f, 0.0f)},
	g_enemySpawner[20] = {FALSE,    3,   SPAWN_FLY_ONE, 	   2,			XMFLOAT3(500.0f,		 bg->h - 575.0f, 0.0f)},
	g_enemySpawner[21] = {FALSE,    3,   SPAWN_FLY_ONE, 	   2,			XMFLOAT3(bg->w - 500.0f, bg->h - 575.0f, 0.0f)},
	g_enemySpawner[22] = {FALSE,    3,   SPAWN_FLY_BOUNCE,     2,			XMFLOAT3(bg->w / 2,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[23] = {FALSE,    3,   SPAWN_FLY_BOUNCE, 	   2,			XMFLOAT3(bg->w - 300.0f, bg->h - 150.0f, 0.0f)},
	g_enemySpawner[24] = {FALSE,    3,   SPAWN_FLY_BOUNCE, 	   2,			XMFLOAT3(250.0f,		 bg->h - 150.0f, 0.0f)},

	g_enemySpawner[25] = {FALSE,    4,   SPAWN_SLIME_KING, 	   0,			XMFLOAT3(bg->w / 2,		 bg->h - 150.0f, 0.0f)},

	g_enemySpawner[26] = {FALSE,    5,   SPAWN_KNIGHT, 		   0,			XMFLOAT3(bg->w / 2,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[27] = {FALSE,    5,   SPAWN_KNIGHT, 		   1,			XMFLOAT3(bg->w / 2,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[28] = {FALSE,    5,   SPAWN_BALL,		   1,			XMFLOAT3(bg->w - 500.0f, bg->h - 575.0f, 0.0f)},
	g_enemySpawner[29] = {FALSE,    5,   SPAWN_BALL,		   1,			XMFLOAT3(500.0f,		 bg->h - 575.0f, 0.0f)},

	g_enemySpawner[30] = {FALSE,    6,   SPAWN_KNIGHT, 		   0,			XMFLOAT3(500.0f,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[31] = {FALSE,    6,   SPAWN_KNIGHT, 		   0,			XMFLOAT3(bg->w - 500.0f, bg->h - 150.0f, 0.0f)},
	g_enemySpawner[32] = {FALSE,    6,   SPAWN_KNIGHT, 		   2,			XMFLOAT3(bg->w / 2,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[33] = {FALSE,    6,   SPAWN_FLY_ONE, 	   2,			XMFLOAT3(500.0f,		 bg->h - 575.0f, 0.0f)},

	g_enemySpawner[34] = {FALSE,    7,   SPAWN_FLY_ONE, 	   0,			XMFLOAT3(500.0f,		 bg->h - 575.0f, 0.0f)},
	g_enemySpawner[35] = {FALSE,    7,   SPAWN_KNIGHT, 		   0,			XMFLOAT3(bg->w / 2,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[36] = {FALSE,    7,   SPAWN_BALL,		   0,			XMFLOAT3(300.0f,		 bg->h - 150.0f, 0.0f)},
	g_enemySpawner[37] = {FALSE,    7,   SPAWN_PATROL,		   0,			XMFLOAT3(bg->w / 2,		 bg->h - 375.0f, 0.0f)},
	g_enemySpawner[38] = {FALSE,    7,   SPAWN_PATROL,		   0,			XMFLOAT3(bg->w - 500.0f, bg->h - 525.0f, 0.0f)},
	g_enemySpawner[39] = {FALSE,    7,   SPAWN_PATROL,		   0,			XMFLOAT3(500.0f,		 bg->h - 525.0f, 0.0f)},

	g_enemySpawner[40] = {FALSE,    8,   SPAWN_BOSS,		   0,			XMFLOAT3(bg->w / 2,		 bg->h - 150.0f, 0.0f)},




	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSpawnController(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < SPAWN_TEX_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSpawnController(void)
{
	if (g_RoundKills == g_RoundEnemies)
	{
		g_Round++;
		g_RoundKills   = 0;
		g_RoundEnemies = 0;

		for (int i = 0; i < SPAWN_ENEMY_MAX; i++)
		{
			if (g_enemySpawner[i].round == g_Round)
			{
				g_RoundEnemies++;
			}
			else if (g_enemySpawner[i].round == 0 || g_enemySpawner[i].round > g_Round)	//���E���h���ݒ肳��Ă��Ȃ��A�߂��Ă���
			{
				break;
			}
		}
		
		g_RoundWaitTime = ROUND_WAIT_TIME;
		g_RoundWaitTimeCnt = 0;


		if (g_RoundEnemies == 0)
		{
			//End game
			g_RoundKills = -1;	//���[�v�����Ȃ��悤��

			SetSlowMotion();
		}
	}

	//Result Screen timer
	if (g_RoundEnemies == 0)
	{
		g_RoundWaitTimeCnt++;

		if (g_RoundWaitTimeCnt == 60)
		{
			SetResultScreen();
			SetGameNormalSpeed();
		}
		return;
	}


	if (g_RoundWaitTimeCnt < g_RoundWaitTime)
	{
		g_RoundWaitTimeCnt++;

		if (g_RoundWaitTimeCnt == 60)
		{
			ResetPlatform();
		}
		else if (g_RoundWaitTimeCnt == 200)
		{
			//���E���h���b�Z�[�W��\��
			ShowRoundMsg(g_Round);		
			
			//PLATFORM����\��
			for (int i = 0; i < SPAWN_PLATFORM_MAX; i++)
			{
				if (g_PlatformSpawner[i].round == g_Round && g_PlatformSpawner[i].killCondition == g_RoundKills)
				{
					SetPlatform(g_PlatformSpawner[i].spawnPos);
					g_PlatformSpawner[i].spawned = TRUE; 
				}
				else if (g_PlatformSpawner[i].round == 0 || g_PlatformSpawner[i].round > g_Round)	//���E���h���ݒ肳��Ă��Ȃ��A�߂��Ă���
				{
					break;
				}
			}
		}
		return;
	}


	//PLATFORM SPAWN
	for (int i = 0; i < SPAWN_PLATFORM_MAX; i++)
	{
		if (g_PlatformSpawner[i].spawned == TRUE) continue;

		if (g_PlatformSpawner[i].round == g_Round && g_PlatformSpawner[i].killCondition == g_RoundKills)
		{
			SetPlatform(g_PlatformSpawner[i].spawnPos);
			g_PlatformSpawner[i].spawned = TRUE;
		}
		else if (g_PlatformSpawner[i].round == 0 || g_PlatformSpawner[i].round > g_Round)	//���E���h���ݒ肳��Ă��Ȃ��A�߂��Ă���
		{
			break;
		}
	}

	//ENEMY SPAWN
	for (int i = 0; i < SPAWN_ENEMY_MAX; i++)
	{
		if (g_enemySpawner[i].spawned == TRUE) continue;

		if (g_enemySpawner[i].round == g_Round && g_enemySpawner[i].killCondition <= g_RoundKills)
		{
			if (g_enemySpawner[i].spawned == FALSE)
			{
				g_enemySpawner[i].spawned = TRUE;
				SetSpawnElevator(i);
			}
			else if (g_enemySpawner[i].round == 0 || g_enemySpawner[i].round > g_Round)
			{
				break;
			}
		}
		
	}

	SpawnElevatorUpdate();

#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSpawnController(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();

	//�G���x�[�^�[�̕`��
	for (int i = 0; i < SPAWNER_MAX; i++)
	{
		if (g_Elevator[i].use == FALSE) continue;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Elevator[i].texNo]);

		//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Elevator[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
		float py = g_Elevator[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
		float pw = g_Elevator[i].w;					// �G�l�~�[�̕\����
		float ph = g_Elevator[i].h;					// �G�l�~�[�̕\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;				// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		//ty += 0.01f;
		

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	//�G���x�[�^�[�̕`��
	for (int i = 0; i < SPAWNER_MAX; i++)
	{
		if (g_EnemyBox[i].use == FALSE) continue;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnemyBox[i].texNo]);

		//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_EnemyBox[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
		float py = g_EnemyBox[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
		float pw = g_EnemyBox[i].w;					// �G�l�~�[�̕\����
		float ph = g_EnemyBox[i].h;					// �G�l�~�[�̕\������

		float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// �e�N�X�`���̕�
		float th = 1.0f;				// �e�N�X�`���̍���
		float tx = tw * g_EnemyBox[i].currentSprite;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		//ty += 0.01f;


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	DrawRoundUI();

}

void RoundKill(void)
{
	g_RoundKills++;
}


void DrawRoundUI(void)
{
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SPAWN_TEX_ROUND_UI]);

	//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
	float pw = 104.0f * 0.7f;					// �G�l�~�[�̕\����
	float ph = 140.0f * 0.7f;					// �G�l�~�[�̕\������
	float px = SCREEN_CENTER_X;	// �G�l�~�[�̕\���ʒuX
	float py = ph / 2;	// �G�l�~�[�̕\���ʒuY

	float tw = 1.0f / TEX_ROUND_DIV_X;		// �e�N�X�`���̕�
	float th = 1.0f;				// �e�N�X�`���̍���
	float tx = tw * g_Round;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;	// �e�N�X�`���̍���Y���W

	if (g_Round > 8)
	{
		tx = tw * 8;
	}


	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}


void SetSpawnElevator(int spawnEnemyIndex)
{
	for (int i = 0; i < SPAWNER_MAX; i++)
	{
		if (g_Elevator[i].use == FALSE)
		{
			g_Elevator[i].spawnEnemyIndex = spawnEnemyIndex;
			g_Elevator[i].movePosHeight = g_enemySpawner[spawnEnemyIndex].spawnPos.y + 50.0f - g_EnemyBox[i].h + (g_Elevator[i].h / 2);
			g_Elevator[i].pos = XMFLOAT3(g_enemySpawner[spawnEnemyIndex].spawnPos.x, ElevatorInitialHeight, 0.0f);
			g_Elevator[i].use = TRUE;

			g_EnemyBox[i].spawnEnemyIndex = spawnEnemyIndex;
			g_EnemyBox[i].movePosHeight = g_enemySpawner[spawnEnemyIndex].spawnPos.y + 50.0f - g_EnemyBox[i].h / 2;
			g_EnemyBox[i].pos = XMFLOAT3(g_enemySpawner[spawnEnemyIndex].spawnPos.x, EnemyBoxInitialHeight, 0.0f);
			g_EnemyBox[i].use = TRUE;
			break;
		}
	}
}


void SpawnElevatorUpdate(void)
{
	for (int i = 0; i < SPAWNER_MAX; i++)
	{
		if (g_Elevator[i].use == FALSE) continue;
		
		//Elevator going up
		if (g_Elevator[i].pos.y > g_Elevator[i].movePosHeight && g_EnemyBox[i].currentSprite < 2)
		{
			g_Elevator[i].pos.y -= 20.0f;
		}
		//EnemyBox going up
		else if (g_EnemyBox[i].pos.y > g_EnemyBox[i].movePosHeight && g_EnemyBox[i].currentSprite < 2)
		{
			g_EnemyBox[i].pos.y -= 10.0f;
		}
		//Elevator open door and spawn enemy
		else if (g_Elevator[i].pos.y <= g_Elevator[i].movePosHeight /*&& g_EnemyBox[i].currentSprite < 2*/ && g_EnemyBox[i].countAnim < 60)
		{
			if (g_EnemyBox[i].countAnim >= ANIM_WAIT && g_EnemyBox[i].currentSprite < 2)
			{
				g_EnemyBox[i].currentSprite++;
				g_EnemyBox[i].countAnim = 0;

				if (g_EnemyBox[i].currentSprite == 2)
				{
					// spawn
					switch (g_enemySpawner[g_EnemyBox[i].spawnEnemyIndex].enemy)
					{
					case SPAWN_PATROL:
						SpawnEnemyPatrol(g_enemySpawner[g_EnemyBox[i].spawnEnemyIndex].spawnPos);
						break;

					case SPAWN_BALL:
						SpawnEnemyBall(g_enemySpawner[g_EnemyBox[i].spawnEnemyIndex].spawnPos);
						break;

					case SPAWN_KNIGHT:
						SpawnEnemyKnight(g_enemySpawner[g_EnemyBox[i].spawnEnemyIndex].spawnPos);
						break;

					case SPAWN_FLY_BOUNCE:
						SpawnEnemyFlyBounce(g_enemySpawner[g_EnemyBox[i].spawnEnemyIndex].spawnPos);
						break;
					
					case SPAWN_FLY_ONE:
						SpawnEnemyFlyOne(g_enemySpawner[g_EnemyBox[i].spawnEnemyIndex].spawnPos);
						break;

					case SPAWN_SLIME_KING:
						SpawnSlimeKing(g_enemySpawner[g_EnemyBox[i].spawnEnemyIndex].spawnPos);
						break;

					case SPAWN_BOSS:
						SpawnBoss(g_enemySpawner[g_EnemyBox[i].spawnEnemyIndex].spawnPos);
						break;
					}
				}
			}

			g_EnemyBox[i].countAnim++;
		}
		//EnemyBox going down
		else if (g_EnemyBox[i].pos.y < EnemyBoxInitialHeight && g_EnemyBox[i].currentSprite == 2)
		{
			g_EnemyBox[i].pos.y += 10.0f;
		}
		//elvator going down
		else if (g_Elevator[i].pos.y < ElevatorInitialHeight && g_EnemyBox[i].currentSprite == 2)
		{
			g_Elevator[i].pos.y += 20.0f;
		}
		//Reset 
		else 
		{
			g_Elevator[i].spawnEnemyIndex = -1;
			g_Elevator[i].movePosHeight = 0;
			g_Elevator[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Elevator[i].use = FALSE;
			g_Elevator[i].currentSprite = 0;
			g_Elevator[i].countAnim = -60;

			g_EnemyBox[i].spawnEnemyIndex = -1;
			g_EnemyBox[i].movePosHeight = 0;
			g_EnemyBox[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_EnemyBox[i].use = FALSE;
			g_EnemyBox[i].currentSprite = 0;
			g_EnemyBox[i].countAnim = -60;
		}
		
	}
}

int GetRound(void)
{
	return g_Round;
}