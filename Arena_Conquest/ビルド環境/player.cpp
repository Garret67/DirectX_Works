//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "player.h"
#include "input.h"
#include "bg.h"
#include "platforms.h"
#include "interactiveObject.h"
#include "bullet.h"
#include "enemyFlyBulletOne.h"
#include "enemyFlyBounce.h"
#include "enemyKnight.h"
#include "enemyBall.h"
#include "enemyPatrol.h"
#include "enemySlimeKing.h"
#include "enemyBoss.h"
#include "collision.h"
#include "result.h"
#include "screenGameOver.h"
#include "file.h"
#include "fade.h"
#include "sound.h"

#include "d3d9.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(60.0f)		// �L�����T�C�Y
#define TEXTURE_HEIGHT				(81.6f)		// �L�����T�C�Y

#define TEXTURE_PATTERN_DIVIDE_X	(9)			// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)			// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)			// �A�j���[�V�����̐؂�ւ��Wait�l

// �v���C���[�̉�ʓ��z�u���W
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

// �v���C���[��UI
#define PLAYER_HP_MAX				(10)
#define PLAYER_ENERGY_MAX			(8)

// �W�����v����
#define	PLAYER_JUMP_CNT_MAX			(30)		// 30�t���[���Œ��n����
#define	PLAYER_JUMP_Y_MAX			(100.0f)	// �W�����v�̍���

//�_�b�V������
#define	PLAYER_DASH_DISTANCE		(300.0f)	// �_�b�V���̋���

//Slash����
#define	PLAYER_SLASH_DISTANCE		(70.0f)		// Slash�̋���
#define SLASH_TEXTURE_WIDTH			(465 / 4)	// Slash ��
#define SLASH_TEXTURE_HEIGHT		(348 / 4)	// Slash ����

//�G�i�W�[����
#define	BULLET_ENERGY_COST			(2)			// ���@�̏���
#define HEAL_ENERGY_COST			(2)			// �񕜂̏���

//���G���
#define INVINCIBLE_TIME				(80)		//���G����

//�d��
#define GRAVITY_FORCE_PER_FRAME		(11.5f)		//�d��

//SlowMotionTime
#define SLOW_MOTION_TIME			(80)		//�X���[���[�V��������



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void JumpUpdate(void);
void SlashUpdate(void);
void DashUpdate(void);
void DamageUpdate(void);
void InvincibleUpdate(void);
void FocusUpdate(void);

void AnimationUpdate(void);

void CollisionPlatforms(void);
void CollisionSlash(void);

void PlayerInvincible(int invincibleTime);

void DrawPlayerOffset(int no);
void DrawPlayerSlash(void);
void DrawSlashHit(void);
void DrawFocusEffect(void);
void DrawExclamation(void);

void EnemyHitted(XMFLOAT3 slashPos, XMFLOAT3 enemyPos);

float Lerp(float start, float end, float t);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[ANIM_TEX_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[ANIM_TEX_MAX] = {
	
	"data/TEXTURE/Player/Idle.png",
	"data/TEXTURE/Player/Run.png",
	"data/TEXTURE/Player/Jump.png",
	"data/TEXTURE/Player/Fall.png",
	"data/TEXTURE/Player/Attack2.png",
	"data/TEXTURE/Player/Dash.png",
	"data/TEXTURE/Player/DashOffset.png",
	"data/TEXTURE/Player/Hit.png",
	"data/TEXTURE/Player/Focus.png",
	"data/TEXTURE/Player/Dead.png",
	"data/TEXTURE/Effect/Slash.png",
	"data/TEXTURE/Effect/SlashHit.png",
	"data/TEXTURE/Effect/FocusEffect.png",
	"data/TEXTURE/Effect/Exclamation.png",
};


static PLAYER	g_Player[PLAYER_MAX];					// �v���C���[�\����

static int		g_Texture_Divide_X		= 5;			// ���̃A�j���p�^�[���̃e�N�X�`�����������iX)
static int		g_P_Texture_No			= ANIM_IDLE;	// ���̃e�N�X�`��

static XMFLOAT3 g_SlashHitPos;							//SlashHit���W
static float	g_SlashHitRot;							//SlashHit��]
static int		g_SlashHitFrames		= 14;			//SlashHit�t���[��
static int		g_SlashHitFramesCurrent = 16;			//SlashHit�t���[���J�E���g
static int		g_SlashHitTexPatter		= 0;			//SlashHit�e�N�X�`���p�^�[��

DWORD			prevTime = timeGetTime();				// ���[�v�̍ŏ��̎���
static int		g_SlowMotionCnt;						// �X���[���[�V�����J�E���g
BOOL			g_Exclamation;							//�`���[�g���A���̂͂Ă�

static BOOL		g_Load = FALSE;							// ���������s�������̃t���O



//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < ANIM_TEX_MAX; i++)
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


	g_SlowMotionCnt = 0;
	SetFinalScreen(SCREEN_INGAME);
	SetPauseScreen(FALSE);
	SetGameNormalSpeed();

	BG* bg = GetBG();
	// �v���C���[�\���̂̏�����
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].alive	  = TRUE;

		switch (GetMode())
		{
		case MODE_TUTORIAL_1:
			g_Player[i].pos		= XMFLOAT3(SCREEN_CENTER_X, bg->h - 150.0f, 0.0f);
			g_Player[i].lifes	= 7;	//�`���[�g���A���ł̗͑͂�������
			g_Player[i].energy	= 5;	//�`���[�g���A���ł̓G�l���M�[��������
			break;

		case MODE_TUTORIAL_2:
			g_Player[i].pos		= XMFLOAT3(400.0f,			bg->h - 150.0f, 0.0f);
			g_Player[i].lifes	= 7;	//�`���[�g���A���ł̗͑͂�������
			g_Player[i].energy	= 5;	//�`���[�g���A���ł̓G�l���M�[��������;
			break;

		case MODE_COLISEUM:
			g_Player[i].pos		= XMFLOAT3(bg->w / 2,		bg->h - 150.0f, 0.0f);
			g_Player[i].lifes	= PLAYER_HP_MAX;
			g_Player[i].energy	= PLAYER_ENERGY_MAX;
			break;
		}
		
		g_Player[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w	  = TEXTURE_WIDTH;
		g_Player[i].h	  = TEXTURE_HEIGHT;
		g_Player[i].texNo = 0;


		g_Player[i].lifesMax  = PLAYER_HP_MAX;
		g_Player[i].energyMax = PLAYER_ENERGY_MAX;


		//�����̏�����
		g_Player[i].moveSpeed = 6.0f;				// �ړ���
		g_Player[i].dir		  = CHAR_DIR_RIGHT;		// �E����
		g_Player[i].Lastdir	  = CHAR_DIR_RIGHT;		// �E����
		g_Player[i].moving	  = FALSE;				// �ړ����t���O


		//�A�j���[�V����������
		g_Player[i].countAnim   = 0;
		g_Player[i].patternAnim = 0;		//�ŏ��Ƀv���C���[�����Ɍ�����悤��
		g_Player[i].patternAnim = g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X;


		// �W�����v�̏�����
		g_Player[i].jump		   = FALSE;
		g_Player[i].extendJump	   = FALSE;
		g_Player[i].extendJumpCnt = 0;
		g_Player[i].extendJumpMax = 15;
		g_Player[i].jumpCnt	   = 0;
		g_Player[i].jumpYMax	   = PLAYER_JUMP_Y_MAX;

		g_Player[i].onGround	   = FALSE;


		// dash�̏�����
		g_Player[i].dash		 = FALSE;
		g_Player[i].dashReload	 = TRUE;
		g_Player[i].dashFrames   = 15;
		g_Player[i].dashCnt		 = 0;
		g_Player[i].dashDistance = 300.0f;
		g_Player[i].slashDmg	 = 10.0f;

		// Slash�̏�����
		g_Player[i].slash				= FALSE;
		g_Player[i].slashCollider		= FALSE;
		g_Player[i].slashFrames			= 20;
		g_Player[i].slashColliderFrames = 5;
		g_Player[i].slashCnt			= 0;
		g_Player[i].slashDistance		= PLAYER_SLASH_DISTANCE;

		//�_���[�W���󂯂鎞�̏�����
		g_Player[i].damaged			= FALSE;
		g_Player[i].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].dmgTime			= 20;
		g_Player[i].dmgTimeCnt		= 0;

		g_Player[i].playerColor		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


		//���G������
		g_Player[i].invincible		  = FALSE;
		g_Player[i].invincibleTime	  = 0;
		g_Player[i].invincibleTimeCnt = 0;

		//�W��������
		g_Player[i].focus			  = FALSE;
		g_Player[i].focusTimeMax	  = 60;
		g_Player[i].focusTimeCnt	  = 0;
		g_Player[i].focusEffectSprite = 0;



		// ���g�p
		for (int j = 0; j < PLAYER_OFFSET_CNT; j++)
		{
			g_Player[i].offset[j] = g_Player[i].pos;
		}
	}

	g_Exclamation = FALSE;

	InitPlayerUI();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < ANIM_TEX_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	UninitPlayerUI();

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{

	for (int i = 0; i < PLAYER_MAX; i++)
	{

		// �A�j���[�V����  
		AnimationUpdate();
		
		
		// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
		XMFLOAT3 pos_old = g_Player[i].pos;

		// �_�b�V���̉e
		for (int j = PLAYER_OFFSET_CNT - 1; j > 0; j--)
		{
			g_Player[i].offset[j] = g_Player[i].offset[j - 1];
		}
		g_Player[i].offset[0] = pos_old;


		//*****************************************************************************
		//  �L�[����
		//*****************************************************************************
			
		//�|�[�Y Mode
		if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_START))
		{
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
			SetPauseScreen(TRUE);
		}

		//�v���C���[�������Ă���Ԃ̏���
		if(g_Player[i].alive == TRUE)
		{
			//�_�b�V�����Ă�����A���������Ȃ�
			if (g_Player[i].dash == FALSE) 
			{
				g_Player[i].moving = FALSE;


				// �L�[���͂ŏ㉺����
				if (GetKeyboardPress(DIK_DOWN)	  || IsButtonPressed(0, BUTTON_DOWN))		// ��
				{
					g_Player[i].dir = CHAR_DIR_DOWN;
				}
				else if (GetKeyboardPress(DIK_UP) || IsButtonPressed(0, BUTTON_UP))			// ��
				{
					g_Player[i].dir = CHAR_DIR_UP;
				}
				else
				{
					g_Player[i].dir = g_Player[i].Lastdir;
				}

				// �L�[���͂ō��E�ړ� 
				if (GetKeyboardPress(DIK_RIGHT)		|| IsButtonPressed(0, BUTTON_RIGHT))	// �E
				{
					g_Player[i].pos.x += g_Player[i].moveSpeed;
					g_Player[i].dir = CHAR_DIR_RIGHT;
					g_Player[i].Lastdir = CHAR_DIR_RIGHT;
					g_Player[i].moving = TRUE;
				}
				else if (GetKeyboardPress(DIK_LEFT) || IsButtonPressed(0, BUTTON_LEFT))		// ��
				{
					g_Player[i].pos.x -= g_Player[i].moveSpeed;
					g_Player[i].dir = CHAR_DIR_LEFT;
					g_Player[i].Lastdir = CHAR_DIR_LEFT;
					g_Player[i].moving = TRUE;
				}


				// ���@�ݒ�
				if (GetKeyboardTrigger(DIK_F) || IsButtonTriggered(0, BUTTON_R))	// �o���b�g����
				{
					if (g_Player[0].energy >= BULLET_ENERGY_COST) //Bullet cost
					{
						PlaySound(SOUND_LABEL_SE_MagicCast);
						g_Player[0].energy -= BULLET_ENERGY_COST;
						UpdateEnergyUI();
						SetBulletPlayer(g_Player[i].pos, g_Player[i].dir);
					}
				}

	
				FocusUpdate();	// �W��    ����
				JumpUpdate();	// �W�����v����
				SlashUpdate();	// Slash   ����
			}

			DashUpdate();	//�_�b�V������



			// ������Z�[�u����
			/*if (GetKeyboardTrigger(DIK_S))
			{
				SaveData();
			}*/
		}

		//*****************************************************************************
		//  ���
		//*****************************************************************************

		//�d��
		if (g_Player[i].dash == FALSE)
		{
			g_Player[i].pos.y += GRAVITY_FORCE_PER_FRAME;
		}


		DamageUpdate();		//���t������

		InvincibleUpdate(); //���G����

			


		//*****************************************************************************
		//  �����蔻��
		//*****************************************************************************
		CollisionPlatforms();	// PLATFORMS�Ƃ̓����蔻��
		CollisionSlash();		// SLASH�ƃG�l�~�[�̓����蔻��

		// MAP�O�`�F�b�N
		BG* bg = GetBG();

		if (g_Player[i].pos.x < g_Player[i].w * 0.5f)
		{
			g_Player[i].pos.x = g_Player[i].w * 0.5f;
		}

		if (g_Player[i].pos.x > bg->w - g_Player[i].w * 0.5f)
		{
			g_Player[i].pos.x = bg->w - g_Player[i].w * 0.5f;
		}

		if (g_Player[i].pos.y < g_Player[i].h * 0.5f)
		{
			g_Player[i].pos.y = g_Player[i].h * 0.5f;
		}

		if (g_Player[i].pos.y > bg->h - g_Player[i].h * 0.5f)
		{
			g_Player[i].pos.y = bg->h - g_Player[i].h * 0.5f;
		}


		//*****************************************************************************
		//  MAP�̃X�N���[��
		// �v���C���[�̗����ʒu����MAP�̃X�N���[�����W���v�Z����
		//*****************************************************************************
		{
			DWORD currentTime = timeGetTime();						// ���[�v�J�n����
			float deltaTime = (currentTime - prevTime) / 1000.0f;	// deltaTime ��b�P�ʂŌv�Z
			prevTime = currentTime;									// ���̃t���[���̂��߂Ɍ��ݎ�����ۑ�����

			float newCameraX = Lerp(bg->pos.x, g_Player[i].pos.x - PLAYER_DISP_X, deltaTime * 3);
			float newCameraY = Lerp(bg->pos.y, g_Player[i].pos.y - PLAYER_DISP_Y + 200.0f, deltaTime * 3);

			// �J�������[����O��Ȃ��悤�ɐ�����������
			if (newCameraX < 0) newCameraX = 0;
			if (newCameraX > bg->w - SCREEN_WIDTH) newCameraX = bg->w - SCREEN_WIDTH;

			if (newCameraY < 0) newCameraY = 0;
			if (newCameraY > bg->h - SCREEN_HEIGHT) newCameraY = bg->h - SCREEN_HEIGHT;

			bg->pos.x = newCameraX;
			bg->pos.y = newCameraY;

		}
		
		
	}

	UpdatePlayerUI();	//UI����

	//�v���C���[�����񂾂�A�X���[���[�V����
	if (g_Player[0].alive == FALSE)
	{
		g_SlowMotionCnt++;

		if (g_SlowMotionCnt == SLOW_MOTION_TIME)
		{
			SetGameNormalSpeed();
			SetScreenGameOver();
		}
	}


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		{
			// �v���C���[��Slash
			DrawPlayerSlash();
			DrawSlashHit();
			

			// �v���C���[�̕��g��`��
			DrawPlayerOffset(i);
			

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_P_Texture_No]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Player[i].pos.x - bg->pos.x;			// �v���C���[�̕\���ʒuX
			float py = g_Player[i].pos.y - bg->pos.y;			// �v���C���[�̕\���ʒuY
			float pw = g_Player[i].w;							// �v���C���[�̕\����
			float ph = g_Player[i].h;							// �v���C���[�̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / g_Texture_Divide_X;				// �e�N�X�`���̕�
			float th = 1.0f;									// �e�N�X�`���̍���
			float tx = (float)(g_Player[i].patternAnim) * tw;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;									// �e�N�X�`���̍���Y���W

			//�������̒���
			if (g_Player[i].dir == CHAR_DIR_LEFT)
			{
				tx += tw;
				tw *= -1.0f;
			}
			

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_Player[i].playerColor,
				g_Player[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}
	}
	DrawExclamation();		//�͂ĂȂ̕`��

	DrawFocusEffect();		//�񕜂̃G�t�F�N�g�`��

	DrawPlayerUI();			//UI�`��
}


//=============================================================================
// Player�\���̂̐擪�A�h���X���擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}


//=============================================================================
// �v���C���[�̕��g��`��
//=============================================================================
void DrawPlayerOffset(int no)
{
	if (!g_Player[0].dash) return;

	BG* bg = GetBG();
	float alpha = 0.0f;

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ANIM_DASH_OFFSET]);

	for (int j = PLAYER_OFFSET_CNT - 1; j >= 0; j--)
	{
		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Player[no].offset[j].x - bg->pos.x;	// �v���C���[�̕\���ʒuX
		float py = g_Player[no].offset[j].y - bg->pos.y;	// �v���C���[�̕\���ʒuY
		float pw = g_Player[no].w;							// �v���C���[�̕\����
		float ph = g_Player[no].h;							// �v���C���[�̕\������

		// �A�j���[�V�����p
		float tw = 1.0f;									// �e�N�X�`���̕�
		float th = 1.0f;									// �e�N�X�`���̍���
		float tx = 0.0f;									// �e�N�X�`���̍���X���W
		float ty = 0.0f;									// �e�N�X�`���̍���Y���W
		
		


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// �v���C���[��Slash��`��
//=============================================================================
void DrawPlayerSlash(void)
{
	if (!g_Player[0].slashCollider) return;

	BG* bg = GetBG();

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ANIM_SLASH]);

	//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_Player[0].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
	float py = g_Player[0].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
	float pw = SLASH_TEXTURE_WIDTH;				// �v���C���[�̕\����
	float ph = SLASH_TEXTURE_HEIGHT;			// �v���C���[�̕\������

	float rotSlash = 0;

	//SLASH�̌���
	switch (g_Player[0].slashDir)
	{
	case SLASH_DIR_RIGHT:
		px += g_Player[0].slashDistance;
		break;

	case SLASH_DIR_LEFT:
		px -= g_Player[0].slashDistance;
		rotSlash = XM_PI;
		break;

	case SLASH_DIR_DOWN:
		py += g_Player[0].slashDistance;
		rotSlash = XM_PIDIV2;
		break;

	case SLASH_DIR_UP:
		py -= g_Player[0].slashDistance;
		rotSlash = -XM_PIDIV2;
		break;
	}

	float tx = 0.0f;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;	// �e�N�X�`���̍���Y���W
	float tw = 1.0f;	// �e�N�X�`���̕�
	float th = 1.0f;
	



	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, 
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		rotSlash);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
	
}


//=============================================================================
// �v���C���[��Slash HIT��`��
//=============================================================================
void DrawSlashHit(void)
{

	if (g_SlashHitFramesCurrent <= g_SlashHitFrames)
	{
		if (g_SlashHitFramesCurrent != 0 && g_SlashHitFramesCurrent % 5 == 0)
		{
			g_SlashHitTexPatter++;
		}




		BG* bg = GetBG();

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ANIM_SLASHHIT]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_SlashHitPos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
		float py = g_SlashHitPos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
		float pw = 75.0f;						// �v���C���[�̕\����
		float ph = 500.0f;						// �v���C���[�̕\������

		float rotSlashHit = 0;


		float tw = 1.0f / 3.0f;					// �e�N�X�`���̕�
		float th = 1.0f;						// �e�N�X�`���̍���
		float tx = tw * (g_SlashHitTexPatter);	// �e�N�X�`���̍���X���W
		float ty = 0.0f;						// �e�N�X�`���̍���Y���W


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			g_SlashHitRot);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		g_SlashHitFramesCurrent++;
	}
}


//=============================================================================
// �񕜃G�t�F�N�g��`��
//=============================================================================
void DrawFocusEffect(void)
{
	if (g_Player[0].focus == FALSE) return;

	BG* bg = GetBG();

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ANIM_FOCUS_EFFECT]);

	//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_Player[0].pos.x - bg->pos.x;		// �v���C���[�̕\���ʒuX
	float py = g_Player[0].pos.y - bg->pos.y;		// �v���C���[�̕\���ʒuY
	float pw = 100.0f;								// �v���C���[�̕\����
	float ph = 120.0f;								// �v���C���[�̕\������


	float tw = 1.0f / 15.0f ;						// �e�N�X�`���̕�
	float th = 1.0f;								// �e�N�X�`���̍���
	float tx = tw * g_Player[0].focusEffectSprite;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;								// �e�N�X�`���̍���Y���W


	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

}



void DrawExclamation(void)
{
	if (g_Exclamation == TRUE)
	{
		BG* bg = GetBG();

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ANIM_EXCLAMATION]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Player[0].pos.x - bg->pos.x;			// �v���C���[�̕\���ʒuX
		float py = g_Player[0].pos.y - bg->pos.y - 100.0f;	// �v���C���[�̕\���ʒuY
		float pw = 70.0f;									// �v���C���[�̕\����
		float ph = 70.0f;									// �v���C���[�̕\������


		float tw = 1.0f;									// �e�N�X�`���̕�
		float th = 1.0f;									// �e�N�X�`���̍���
		float tx = 0.0f;									// �e�N�X�`���̍���X���W
		float ty = 0.0f;									// �e�N�X�`���̍���Y���W


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
}



//=============================================================================
// �v���C���[�����t����
// �����F���t��������
//=============================================================================
void PlayerDamaged(XMFLOAT3 enemyPos)
{
	g_Player[0].damaged			= TRUE;
	g_Player[0].damageOriginPos = enemyPos;
	g_Player[0].playerColor		= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	g_Player[0].lifes--;
	UpdateLifesUI();
	ResultHit();

	//���񂾂�̐ݒ�
	if (g_Player[0].lifes <= 0)
	{
		g_Player[0].alive			= FALSE;
		g_Player[0].slash			= FALSE;
		g_Player[0].slashCollider	= FALSE;
		g_Player[0].patternAnim		= 0;

		SetSlowMotion();

		if (GetMode() != MODE_COLISEUM)
		{
			SetFade(FADE_OUT, GetMode());
		}
	}
	else
	{
		PlayerInvincible(INVINCIBLE_TIME);
	}

}


//=============================================================================
// ���t������
//=============================================================================
void DamageUpdate(void)
{
	if (g_Player[0].damaged == TRUE)
	{
		//�A�j���[�V�����p
		if (g_Player[0].alive)
		{
			g_P_Texture_No		= ANIM_HIT;
			g_Texture_Divide_X	= 1;
		}
		else
		{
			g_P_Texture_No		= ANIM_DEATH;
			g_Texture_Divide_X	= 4;
		}
		

		//�m�b�N�o�b�N����
		XMVECTOR Ppos = XMLoadFloat3(&g_Player[0].pos);
		XMVECTOR vec = (Ppos - XMLoadFloat3(&g_Player[0].damageOriginPos));		//�����ƃv���C���[�Ƃ̍��������߂�
		float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);					//���̍������g���Ċp�x�����߂Ă���

		if (fabsf(angle) < XM_PIDIV2)	//�_���[�W�������炫���ꍇ	�i������, �E��0�A�と�����A���������j
		{
			angle = -XM_PIDIV4;		//����̊p�x
		}
		else
		{
			angle = -XM_PIDIV4 * 3;	//�E��̊p�x
		}

		float repealDisstance		  = 25.0f;										//�X�s�[�h�͂�����ƒx�����Ă݂�
		float repealDisstancePerFrame = repealDisstance / g_Player[0].dmgTime;

		float repealDistNow	= repealDisstancePerFrame * (g_Player[0].dmgTime - g_Player[0].dmgTimeCnt);


		g_Player[0].pos.x += cosf(angle) * repealDistNow;					//angle�̕����ֈړ�
		g_Player[0].pos.y += sinf(angle) * repealDistNow;					//angle�̕����ֈړ�

		g_Player[0].dmgTimeCnt++;

		if (g_Player[0].dmgTimeCnt >= g_Player[0].dmgTime)
		{
			g_Player[0].damaged		= FALSE;
			g_Player[0].dmgTimeCnt	= 0;
			g_Player[0].playerColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}


//=============================================================================
// �v���C���[�̖��G���
// �����F���G����/�t���[���i60 �� 1�b�j
//=============================================================================
void PlayerInvincible(int invincibleTime)
{
	g_Player[0].invincible		= TRUE;
	g_Player[0].invincibleTime	= invincibleTime;
}

//=============================================================================
// �v���C���[�̖��G����
//=============================================================================
void InvincibleUpdate(void)
{
	if (g_Player[0].invincible == TRUE)
	{
		if (g_Player[0].damaged == FALSE)
		{
			g_Player[0].playerColor.w = 0.7f;
		}

		g_Player[0].invincibleTimeCnt++;

		if (g_Player[0].invincibleTimeCnt >= g_Player[0].invincibleTime)
		{
			g_Player[0].invincible		  = FALSE;
			g_Player[0].invincibleTimeCnt = 0;
			g_Player[0].invincibleTime    = 0;
			g_Player[0].playerColor.w	  = 1.0f;
		}
	}
}


//=============================================================================
// PLATFORM�Ƃ̓����蔻��
//=============================================================================
void CollisionPlatforms(void)
{
	BOOL ans;

	//�`���[�g���A���ɂ����ǂ̓����蔻����s��
	switch (GetMode())
	{
	case MODE_TUTORIAL_1:
		{
			PLATFORM* HouseGround = GetGround();
			ans = CollisionBB(HouseGround[0].pos,   HouseGround[0].w ,  HouseGround[0].h,
							  g_Player[0].pos,		g_Player[0].w,		g_Player[0].h);

			if (ans == TRUE)
			{
				g_Player[0].pos.y	 = HouseGround[0].pos.y - (HouseGround[0].h / 2) - (g_Player[0].h / 2);
				g_Player[0].onGround = TRUE;
				g_Player[0].onAirCnt = 0;
			}

			BREAKABLE_WALL BreakableWall = GetBreakableWall();
			if (BreakableWall.HP > 0)
			{
				//�v���C���[���ǂ𒴂����Ȃ��ׂ̂����蔻��
				ans = CollisionBB(BreakableWall.pos,	BreakableWall.w,	BreakableWall.h,
								  g_Player[0].pos,		g_Player[0].w,		g_Player[0].h);

				if (ans == TRUE)
				{
					g_Player[0].pos.x = BreakableWall.pos.x - (BreakableWall.w / 2) - (g_Player[0].w / 2);
				}

				//�v���C���[�̏�Ƀr�b�N���}�[�N��\���ׂ̂����蔻��
				ans = CollisionBB(BreakableWall.pos,	BreakableWall.w * 3,	BreakableWall.h,
								  g_Player[0].pos,		g_Player[0].w,			g_Player[0].h);

				if (ans == TRUE && g_Exclamation == FALSE)
				{
					g_Exclamation = TRUE;
					PlaySound(SOUND_LABEL_SE_Exclamation);
				}
				else if (ans == FALSE && g_Exclamation == TRUE)
				{
					g_Exclamation = FALSE;
				}
			}
			
		}
		break;

	default:

		{
			PLATFORM* ground = GetGround();

			// groundS�̐��������蔻����s��
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;

				ans = CheckCollisionAndGetDirection(ground[j].pos.x,	ground[j].pos.y,	(ground[j].w),		(ground[j].h),
													g_Player[0].pos.x,  g_Player[0].pos.y,	(g_Player[0].w),	(g_Player[0].h));
				
				// �������Ă���H
				switch (ans)
				{
				case FromTop:
					g_Player[0].pos.y	 = ground[j].pos.y - (ground[j].h / 2) - (g_Player[0].h / 2);
					g_Player[0].onGround = TRUE;
					g_Player[0].onAirCnt = 0;
					break;

				case FromBottom:
					g_Player[0].pos.y = ground[j].pos.y + (ground[j].h / 2) + (g_Player[0].h / 2);
					break;

				case FromLeft:
					g_Player[0].pos.x = ground[j].pos.x - (ground[j].w / 2) - (g_Player[0].w / 2);
					break;

				case FromRight:
					g_Player[0].pos.x = ground[j].pos.x + (ground[j].w / 2) + (g_Player[0].w / 2);
					break;

				default:
					if (g_Player[0].onAirCnt > 10)
					{
						g_Player[0].onGround = FALSE;
					}
					else
					{
						g_Player[0].onAirCnt++;
					}
					break;
				}
			}
		}

		{
			PLATFORM_ANIM* platform = GetPlatforms();

			// PLATFORMS�̐��������蔻����s��
			for (int j = 0; j < PLATFORM_MAX; j++)
			{
				if (platform[j].collider == FALSE) continue;

				float platformH = 50.0f;

				ans = CheckCollisionAndGetDirection(platform[j].pos.x, platform[j].pos.y, (platform[j].w), (platformH),
					g_Player[0].pos.x, g_Player[0].pos.y, (g_Player[0].w), (g_Player[0].h));
				// �������Ă���H

				switch (ans)
				{
				case FromTop:
					g_Player[0].pos.y = platform[j].pos.y - (platformH / 2) - (g_Player[0].h / 2);
					g_Player[0].onGround = TRUE;
					g_Player[0].onAirCnt = 0;
					break;

				case FromBottom:
					g_Player[0].pos.y = platform[j].pos.y + (platformH / 2) + (g_Player[0].h / 2);
					break;

				case FromLeft:
					g_Player[0].pos.x = platform[j].pos.x - (platform[j].w / 2) - (g_Player[0].w / 2);
					break;

				case FromRight:
					g_Player[0].pos.x = platform[j].pos.x + (platform[j].w / 2) + (g_Player[0].w / 2);
					break;

				default:
					if (g_Player[0].onAirCnt > 10)
					{
						g_Player[0].onGround = FALSE;
					}
					else
					{
						g_Player[0].onAirCnt++;
					}
					break;
				}
			}
		}

		{
			PLATFORM* Spikes = GetSpikes();
			for (int i = 0; i < SPIKES_MAX; i++)
			{
				ans = CollisionBB(Spikes[i].pos, Spikes[i].w, Spikes[i].h,
					g_Player[0].pos, g_Player[0].w, g_Player[0].h);

				if (ans == TRUE && g_Player[0].lifes >= 1)
				{
					g_Player[0].lifes = 1;
					PlayerDamaged(Spikes[i].pos);
				}
			}

			
			
		}
		
		break;
	}

	
}


//=============================================================================
// SLASH�ƃG�l�~�[�̓����蔻��
//=============================================================================
void CollisionSlash(void)
{
	if (g_Player[0].slashCollider == TRUE)
	{
		//SLASH�̈ʒu���v�Z����
		float slashW = SLASH_TEXTURE_WIDTH;
		float slashH = SLASH_TEXTURE_HEIGHT;
		XMFLOAT3 slashPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		slashPos.x = g_Player[0].pos.x;
		slashPos.y = g_Player[0].pos.y;

		switch (g_Player[0].slashDir)
		{
		case SLASH_DIR_RIGHT:
			slashPos.x += g_Player[0].slashDistance;
			break;

		case SLASH_DIR_LEFT:
			slashPos.x -= g_Player[0].slashDistance;
			break;

		case SLASH_DIR_DOWN:
			slashPos.y += g_Player[0].slashDistance;
			slashW = SLASH_TEXTURE_HEIGHT;	//90�x��]���Ă���̂ł��Ƃ���ς���
			slashH = SLASH_TEXTURE_WIDTH;
			break;

		case SLASH_DIR_UP:
			slashPos.y -= g_Player[0].slashDistance;
			slashW = SLASH_TEXTURE_HEIGHT;	//90�x��]���Ă���̂ł��Ƃ���ς���
			slashH = SLASH_TEXTURE_WIDTH;
			break;
		}


		//�ǂ̓����蔻��
		if(GetMode() == MODE_TUTORIAL_1)
		{
			BREAKABLE_WALL BreakableWall = GetBreakableWall();

			if (BreakableWall.HP > 0)
			{
				BOOL ans = CollisionBB(slashPos, slashW, slashH,
					BreakableWall.pos, BreakableWall.w, BreakableWall.h);
				// �������Ă���H
				if (ans == TRUE)
				{
					//IteractiveObjHit[j].HP--;
					BreakableWallHit();
					g_SlashHitPos = slashPos;
					g_SlashHitRot = 0.0f;
					g_SlashHitFramesCurrent = 0;
					g_SlashHitTexPatter = 0;
						
				}
			}
				
		}
		

		{
			BOSS Boss = GetBoss();


			// �����Ă�G�l�~�[�Ɠ����蔻�������
			if (Boss.alive == TRUE && (Boss.damaged == FALSE))	//�G�͎���ł��Ȃ����܂������Ă��Ȃ����
			{
				BOOL ans = CollisionBB(slashPos, slashW, slashH,
					Boss.pos, Boss.w, Boss.h);
				// �������Ă���H
				if (ans == TRUE)
				{
					// �����������̏���

					BossDamaged(g_Player[0].slashDmg);

					EnemyHitted(slashPos, Boss.pos);
				}
			}
		}



		{
			SLIME_KING slimeKing = GetSlimeKing();


			// �����Ă�G�l�~�[�Ɠ����蔻�������
			if (slimeKing.alive == TRUE && (slimeKing.damaged == FALSE))	//�G�͎���ł��Ȃ����܂������Ă��Ȃ����
			{
				BOOL ans = CollisionBB(slashPos, slashW, slashH,
					slimeKing.pos, slimeKing.w, slimeKing.h);
				// �������Ă���H
				if (ans == TRUE)
				{
					// �����������̏���

					SlimeKingDamaged(g_Player[0].slashDmg);

					EnemyHitted(slashPos, slimeKing.pos);

				}
			}
			
		}


		{
			ENEMY_FLY_ONE* enemyFlyOne = GetEnemyFlyOne();

			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < ENEMY_FLY_ONE_MAX; j++)
			{
				// �����Ă�G�l�~�[�Ɠ����蔻�������
				if (enemyFlyOne[j].alive == TRUE && (enemyFlyOne[j].state != FLY_ONE_STATE_HIT))	//�G�͎���ł��Ȃ����܂������Ă��Ȃ����
				{
					BOOL ans = CollisionBB(slashPos, slashW, slashH,
						enemyFlyOne[j].pos, enemyFlyOne[j].w, enemyFlyOne[j].h);
					// �������Ă���H
					if (ans == TRUE)
					{
						// �����������̏���

						EnemyFlyOneDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);

						EnemyHitted(slashPos, enemyFlyOne[j].pos);

					}
				}
			}
		}


		{
			ENEMY_FLY_BOUNCE* enemyFlyBounce = GetEnemyFlyBounce();

			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < ENEMY_FLY_BOUNCE_MAX; j++)
			{
				// �����Ă�G�l�~�[�Ɠ����蔻�������
				if (enemyFlyBounce[j].alive == TRUE && enemyFlyBounce[j].damaged == FALSE)	//�G�͎���ł��Ȃ����܂������Ă��Ȃ����
				{
					BOOL ans = CollisionBB(slashPos, slashW, slashH,
						enemyFlyBounce[j].pos, enemyFlyBounce[j].w, enemyFlyBounce[j].h);
					// �������Ă���H
					if (ans == TRUE)
					{
						// �����������̏���

						EnemyFlyBounceDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);

						EnemyHitted(slashPos, enemyFlyBounce[j].pos);

					}
				}
			}
		}



		{
			ENEMY_KNIGHT* enemyKnight = GetEnemyKnight();

			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < ENEMY_KNIGHT_MAX; j++)
			{
				// �����Ă�G�l�~�[�Ɠ����蔻�������
				if ((enemyKnight[j].alive == TRUE) && (enemyKnight[j].state != KNIGHT_STATE_HIT))	//�G�͎���ł��Ȃ����܂������Ă��Ȃ����
				{
					if (g_Player[0].slashCnt == 0)	//SLASH�̍ŏ��̃t���[�������L��
					{
						BOOL ans = CheckCollisionAndGetDirection(enemyKnight[j].pos.x, enemyKnight[j].pos.y, KNIGHT_COLLIDER_WIDTH, enemyKnight[j].h,
																slashPos.x, slashPos.y, slashW, slashH);
						// �������Ă���H

						if (ans > 0)
						{
							g_Player[0].slashCnt = g_Player[0].slashCnt;
						}

						switch (ans)
						{
							case FromTop:
								if (enemyKnight[j].state == KNIGHT_STATE_DEFENCE_UP)
								{
									PlaySound(SOUND_LABEL_SE_shieldGuard); //Defence sound
									//enemyKnight[j].state = KNIGHT_STATE_ATTACK;		//(JP)Attack state�ɕς��@(EN)Change to Attack state
									if (g_Player[0].slashDir == SLASH_DIR_DOWN)
									{
										
										g_Player[0].jumpCnt = 0;
										g_Player[0].jump = TRUE;
									}
								}
								else
								{
									EnemyKnightDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);
									EnemyHitted(slashPos, enemyKnight[j].pos);
								}
								
								break;

							case FromBottom:
								EnemyKnightDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);
								EnemyHitted(slashPos, enemyKnight[j].pos);
								break;

							case FromLeft:

								if (enemyKnight[j].state == KNIGHT_STATE_DEFENCE_FRONT ||
									(enemyKnight[j].state == KNIGHT_STATE_ATTACK && enemyKnight[j].currentSprite < 3))
								{
									PlaySound(SOUND_LABEL_SE_shieldGuard); //Defence sound
									if (enemyKnight[j].state != KNIGHT_STATE_ATTACK)
									{
										enemyKnight[j].state = KNIGHT_STATE_ATTACK;		//(JP)Attack state�ɕς��@(EN)Change to Attack state
										enemyKnight[j].stateTime = 60;
										enemyKnight[j].stateTimeCnt = 0;

										enemyKnight[j].countAnim = 0;
										enemyKnight[j].currentSprite = 0;
									}
								}
								else
								{
									EnemyKnightDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);
									EnemyHitted(slashPos, enemyKnight[j].pos);
								}

								break;

						case FromRight:
							if (enemyKnight[j].state == KNIGHT_STATE_DEFENCE_FRONT ||
								(enemyKnight[j].state == KNIGHT_STATE_ATTACK && enemyKnight[j].currentSprite < 3))
							{
								PlaySound(SOUND_LABEL_SE_shieldGuard); //Defence sound
								if (enemyKnight[j].state != KNIGHT_STATE_ATTACK)
								{
									enemyKnight[j].state = KNIGHT_STATE_ATTACK;		//(JP)Attack state�ɕς��@(EN)Change to Attack state
									enemyKnight[j].stateTime = 60;
									enemyKnight[j].stateTimeCnt = 0;

									enemyKnight[j].countAnim = 0;
									enemyKnight[j].currentSprite = 0;
								}
							}
							else
							{
								EnemyKnightDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);
								EnemyHitted(slashPos, enemyKnight[j].pos);
							}
							break;
						}
					}
				}
			}
		}



		{
			ENEMY_BALL* enemyBall = GetEnemyBall();

			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < ENEMY_BALL_MAX; j++)
			{
				// �����Ă�G�l�~�[�Ɠ����蔻�������
				if (enemyBall[j].alive == TRUE && enemyBall[j].damaged == FALSE)	//�G�͎���ł��Ȃ����܂������Ă��Ȃ����
				{
						

					BOOL ans = CollisionBB(slashPos, slashW, slashH,
						enemyBall[j].pos, enemyBall[j].w, enemyBall[j].h);
					// �������Ă���H
					if (ans == TRUE)
					{
						// �����������̏���
							
						EnemyBallDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);
						enemyBall[j].state = BALL_STATE_SEARCH;
						enemyBall[j].stateTime = 30 + (rand() % 200);
						enemyBall[j].stateTimeCnt = 0;

						EnemyHitted(slashPos, enemyBall[j].pos);

					}
				}
			}
		}
			
		{
			ENEMY_PATROL* enemyPatrol = GetEnemyPatrol();

			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < ENEMY_PATROL_MAX; j++)
			{
				// �����Ă�G�l�~�[�Ɠ����蔻�������
				if (enemyPatrol[j].alive == TRUE && enemyPatrol[j].damaged == FALSE)	//�G�͎���ł��Ȃ����܂������Ă��Ȃ����
				{


					BOOL ans = CollisionBB(slashPos, slashW, slashH,
						enemyPatrol[j].pos, enemyPatrol[j].w, enemyPatrol[j].h);
					// �������Ă���H
					if (ans == TRUE)
					{
						// �����������̏���

						EnemyDamaged(g_Player[0].pos, j, g_Player[0].slashDmg);

						EnemyHitted(slashPos, enemyPatrol[j].pos);

					}
				}
			}
		}
		
	}
}


//=============================================================================
// �W�����v����
//=============================================================================
void JumpUpdate(void)
{
	if (g_Player[0].jump == TRUE)
	{
		if (g_Player[0].damaged == TRUE)
		{
			g_Player[0].jump = FALSE;
			return;
		}

		g_Player[0].pos.y -= GRAVITY_FORCE_PER_FRAME;	//�d�͂𖳌��ɂ���

		float angle = (XM_PI / PLAYER_JUMP_CNT_MAX) * g_Player[0].jumpCnt;		//PI���W�����v�i�K�Ɋ���

		//�W�����v�̍ŏ��̃W�����v
		if (g_Player[0].jumpCnt <= PLAYER_JUMP_CNT_MAX * 0.5f)
		{
			g_P_Texture_No		= ANIM_JUMP;
			g_Texture_Divide_X  = 1;
			float y = GRAVITY_FORCE_PER_FRAME * (sinf(XM_PI + angle) + 1);
			g_Player[0].pos.y -= y;
		}
		else	//�����n�߂�
		{
			float y = GRAVITY_FORCE_PER_FRAME * (sinf(XM_PI + angle) + 1);
			g_Player[0].pos.y += y;
			g_P_Texture_No = ANIM_FALL;
			g_Texture_Divide_X = 1;
		}

		//�W�����v�̉�������
		if ((GetKeyboardPress(DIK_Z) || IsButtonPressed(0, BUTTON_A)) && g_Player[0].extendJump == TRUE)
		{
			g_Player[0].extendJumpCnt++;
			if (g_Player[0].extendJumpCnt >= g_Player[0].extendJumpMax)
			{
				g_Player[0].extendJump	  = FALSE;
				g_Player[0].extendJumpCnt = 0;
			}
		}
		else
		{
			g_Player[0].onGround		= FALSE;
			g_Player[0].extendJump		= FALSE;
			g_Player[0].extendJumpCnt	= 0;
			g_Player[0].jumpCnt++;

		}

		//�W�����v�̏I���
		if (g_Player[0].jumpCnt > PLAYER_JUMP_CNT_MAX)
		{			 
			g_Player[0].jump = FALSE;
			g_Player[0].jumpCnt = 0;
		}

	}
	// �W�����v�{�^���������H
	else if ((g_Player[0].jump == FALSE) && (GetKeyboardPress(DIK_Z) || IsButtonPressed(0, BUTTON_A)) && g_Player[0].onGround == TRUE)
	{
		g_Player[0].jump = TRUE;
		g_Player[0].extendJump = TRUE;
		g_Player[0].onGround = FALSE;
		g_Player[0].jumpCnt = 0;
		
	}
}


//=============================================================================
// �W������
//=============================================================================
void FocusUpdate(void)
{
	if (g_Player[0].energy < HEAL_ENERGY_COST || g_Player[0].lifes >= g_Player[0].lifesMax) return;


	if (g_Player[0].focus == TRUE)
	{
		if (g_Player[0].onGround == FALSE || g_Player[0].moving == TRUE) 
		{
			g_Player[0].focus = FALSE;
			g_Player[0].focusTimeCnt = 0;
			g_Player[0].focusEffectSprite = 0;
		}

		//�A�j���[�V�����p
		g_P_Texture_No = ANIM_FOCUS;
		g_Texture_Divide_X = 1;

		//�{�^���̒�����
		if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, BUTTON_B))
		{
			if (g_Player[0].focusTimeCnt != 0 && g_Player[0].focusTimeCnt % 4 == 0)
			{
				g_Player[0].focusEffectSprite++;
			}

			if (g_Player[0].focusTimeCnt >= g_Player[0].focusTimeMax)
			{
				g_Player[0].focus = FALSE;
				g_Player[0].focusTimeCnt = 0;
				g_Player[0].focusEffectSprite = 0;

				g_Player[0].lifes++;
				UpdateLifesUI();

				g_Player[0].energy -= HEAL_ENERGY_COST;
				UpdateEnergyUI();
			}

			g_Player[0].focusTimeCnt++;
		}
		else //�{�^���𗣂���
		{
			g_Player[0].focus = FALSE;
			g_Player[0].focusTimeCnt = 0;
			g_Player[0].focusEffectSprite = 0;
		}
	}
	// �W�����v�{�^���������H
	else if ((g_Player[0].focus == FALSE) && (GetKeyboardPress(DIK_A) || IsButtonPressed(0, BUTTON_B)))
	{
		if (g_Player[0].onGround == TRUE && g_Player[0].moving == FALSE)
		{
			g_Player[0].focus = TRUE;
			g_Player[0].focusTimeCnt = 0;
			g_Player[0].focusEffectSprite = 0;
		}
	}
}


//=============================================================================
// SLASH����
//=============================================================================
void SlashUpdate(void)
{
	if (g_Player[0].slash == TRUE)
	{
		g_Player[0].slashCnt++;


		g_P_Texture_No = ANIM_ATTACK;
		g_Texture_Divide_X = 1;
		
		if (g_Player[0].slashCnt >= g_Player[0].slashColliderFrames)
		{
			g_Player[0].slashCollider = FALSE;

			if (g_Player[0].slashCnt >= g_Player[0].slashFrames)
			{
				g_Player[0].slash = FALSE;
			}
		}
	}
	else if (g_Player[0].slash == FALSE)
	{
		if (GetKeyboardTrigger(DIK_X) || IsButtonTriggered(0, BUTTON_X))
		{
			g_Player[0].slashDistance = PLAYER_SLASH_DISTANCE;

			g_Player[0].slashDir = g_Player[0].dir;
			g_Player[0].slashCnt = 0;
			g_Player[0].slash = TRUE;
			g_Player[0].slashCollider = TRUE;

			PlaySound(SOUND_LABEL_SE_slash);
		}

	}
}


//=============================================================================
// �_�b�V������
//=============================================================================
void DashUpdate(void)
{
	// �_�b�V���������H
	if (g_Player[0].dash == TRUE)
	{
		float distancePerFrame = g_Player[0].dashDistance / g_Player[0].dashFrames;
		g_Player[0].pos.x += distancePerFrame;

		g_Player[0].dashCnt++;

		g_P_Texture_No = ANIM_DASH;	//�A�j���[�V�����p
		g_Texture_Divide_X = 1;		//�A�j���[�V�����p

		if (g_Player[0].dashCnt >= g_Player[0].dashFrames)
		{
			g_Player[0].dash = FALSE;
		}
	}
	// �_�b�V���{�^���������H
	else if ((g_Player[0].dash == FALSE) && (GetKeyboardTrigger(DIK_C) || IsButtonTriggered(0, BUTTON_R2)) && g_Player[0].moving == TRUE)
	{
		PlaySound(SOUND_LABEL_SE_Dash);

		if (g_Player[0].dashReload == TRUE)
		{
			g_Player[0].dashDistance = PLAYER_DASH_DISTANCE;

			if (g_Player[0].dir == CHAR_DIR_LEFT)
			{
				g_Player[0].dashDistance = -g_Player[0].dashDistance;
			}
			g_Player[0].dashCnt = 0;
			g_Player[0].dash = TRUE;
			g_Player[0].dashReload = FALSE;
		}
	}
	else if (g_Player[0].dashReload == FALSE)
	{
		if (g_Player[0].onGround) g_Player[0].dashReload = TRUE;
	}
}


//=============================================================================
// �A�j���[�V���� ����
//=============================================================================
void AnimationUpdate(void) 
{
	if (g_Player[0].alive == TRUE)
	{
		if (g_Player[0].moving == TRUE && g_Player[0].onGround == TRUE)
		{
			g_P_Texture_No		= ANIM_RUN;
			g_Texture_Divide_X	= 8;
		}
		else if (g_Player[0].onGround == FALSE)
		{

		}
		else
		{
			g_P_Texture_No		= ANIM_IDLE;
			g_Texture_Divide_X	= 5;
		}

		g_Player[0].countAnim += 1.0f;
		if (g_Player[0].countAnim > ANIM_WAIT)
		{
			g_Player[0].countAnim	= 0.0f;
			g_Player[0].patternAnim = (g_Player[0].patternAnim + 1) % g_Texture_Divide_X;	// �p�^�[���̐؂�ւ�
		}
	}
	else 
	{
		g_P_Texture_No		= ANIM_DEATH;
		g_Texture_Divide_X  = 4;
		g_Player[0].w		= g_Player[0].h;

		
		if (g_Player[0].countAnim > ANIM_WAIT)
		{
			g_Player[0].countAnim = 0.0f;
			// �p�^�[���̐؂�ւ�
			g_Player[0].patternAnim = (g_Player[0].patternAnim + 1);
		}
		if (g_Player[0].patternAnim < 2)
		{
			g_Player[0].countAnim += 1.0f;
		}
		else if (g_Player[0].onGround && g_Player[0].patternAnim == 2)
		{
			g_Player[0].patternAnim ++;
		}
	}
}



//=============================================================================
// �G�l�~�[�ɍU���������� ����
//=============================================================================
void EnemyHitted(XMFLOAT3 slashPos, XMFLOAT3 enemyPos)
{
	if (g_Player[0].energy < g_Player[0].energyMax)
	{
		g_Player[0].energy++;
		UpdateEnergyUI();
	}

	//Slash Hit �`��p
	g_SlashHitPos			= slashPos;
	g_SlashHitFramesCurrent = 0;
	g_SlashHitTexPatter		= 0;

	XMFLOAT3 enemyPosition  = enemyPos;

	XMVECTOR Ppos = XMLoadFloat3(&g_Player[0].pos);
	XMVECTOR vec  = (Ppos - XMLoadFloat3(&enemyPosition));			//�����ƃv���C���[�Ƃ̍��������߂�
	float angle   = atan2f(vec.m128_f32[1], vec.m128_f32[0]);		//���̍������g���Ċp�x�����߂Ă���
	g_SlashHitRot = angle;

	if (g_Player[0].slashDir == SLASH_DIR_DOWN)
	{
		g_Player[0].jumpCnt = 0;
		g_Player[0].jump	= TRUE;
	}
}

//=============================================================================
// �G�l�~�[�ɍU���������� ����
//=============================================================================
float Lerp(float start, float end, float t) 
{
	return start + t * (end - start);
}


//=============================================================================
// �v���C���[��HP�ƃG�l���M�[��ݒ肷��
//=============================================================================
void SetPlayerStats(int hp, int energy)
{
	g_Player[0].lifes  = hp;
	g_Player[0].energy = energy;
	UpdateLifesUI();
	UpdateEnergyUI();
}
