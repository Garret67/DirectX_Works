//=============================================================================
//
// �G�l�~�[���� [enemyBall.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemyBall.h"

#include "fade.h"
#include "collision.h"
#include "sound.h"

#include "spawnController.h"
#include "bg.h"
#include "platforms.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(30 * 4)	// �G�l�~�[�T�C�Y
#define TEXTURE_HEIGHT				(16 * 4)	// 
#define TEXTURE_MAX					(1)			// �e�N�X�`���̐�

#define TEXTURE_ANIM_SPRITES_MAX	(4)			// 1�̃A�j���[�V�����̓X�v���C�g�����iX)
#define ANIM_WAIT					(10)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define DAMAGE_TIME					(30)		// �_���[�W��Ԃ̎���
#define KNOCKBACK_TIME				(15)		// �m�b�N�o�b�N�̎���
#define DESPAWN_TIME				(180)		// �E����邩�������܂ł̎���
												   
#define HP_MAX						(40.0f)		// �̗�
												   
#define SEARCH_STATE_SPEED			(0.5f)		// SEARCH��Ԃ̑��x
#define RUSH_STATE_SPEED			(8.0f)		// �ˌ���Ԃ̑��x

#define STATE_TIME_FIRST_SEARCH		(180)
#define STATE_TIME_JUMP				(25)

#define BALL_GRAVITY_FORCE				(8.0f)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void EnemyBallAnimationUpdate(void);
void BallJumpUpdate(void);

void DespawnEnemyBall(int enemyIndex);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/Ball/Bola_anim.png",
};

static ENEMY_BALL	g_EnemyBall[ENEMY_BALL_MAX];	// �G�l�~�[�\����

static BOOL			g_Load = FALSE;					// ���������s�������̃t���O


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemyBall(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
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

	
	// �G�l�~�[�\���̂̏�����
	for (int i = 0; i < ENEMY_BALL_MAX; i++)
	{	
		g_EnemyBall[i].spawned	= FALSE;
		g_EnemyBall[i].alive	= FALSE;
		g_EnemyBall[i].pos		= XMFLOAT3(1400.0f, 1000.0f, 0.0f);
		g_EnemyBall[i].w		= TEXTURE_WIDTH;
		g_EnemyBall[i].h		= TEXTURE_HEIGHT;
		g_EnemyBall[i].texNo	= 0;

		g_EnemyBall[i].currentSprite	= 0;
		g_EnemyBall[i].currentAnim		= BALL_ANIM_MOVE;

		g_EnemyBall[i].moveSpeed		= SEARCH_STATE_SPEED;
		g_EnemyBall[i].moveDir			= BALL_DIR_RIGHT;

		g_EnemyBall[i].HP				= HP_MAX;

		g_EnemyBall[i].damaged			= FALSE;
		g_EnemyBall[i].damageOriginPos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyBall[i].dmgTimeCnt		= 0;

		g_EnemyBall[i].enemyColor		= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

		g_EnemyBall[i].state			= BALL_STATE_SEARCH;
		g_EnemyBall[i].stateTime		= STATE_TIME_FIRST_SEARCH;
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemyBall(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
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
void UpdateEnemyBall(void)
{

	for (int i = 0; i < ENEMY_BALL_MAX; i++)
	{
		if (g_EnemyBall[i].spawned == FALSE) continue;
	

		// �����Ă�G�l�~�[��������������
		if (g_EnemyBall[i].alive == TRUE)
		{
			// PLAYER�Ƃ̓����蔻��
			{
				PLAYER* player = GetPlayer();

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (player[j].alive == TRUE && player[j].invincible == FALSE)
					{
						BOOL ans = CollisionBB(g_EnemyBall[i].pos,	g_EnemyBall[i].w,	g_EnemyBall[i].h,
											   player[j].pos,		player[j].w,		player[j].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							PlayerDamaged(g_EnemyBall[i].pos);

							//�W�����v����
							if (g_EnemyBall[i].state == BALL_STATE_RUSH)
							{
								g_EnemyBall[i].state = BALL_STATE_JUMP;
								g_EnemyBall[i].stateTime = STATE_TIME_JUMP;
							}
						}
					}
				}
			}

			//�ۂ��G�l�~�[�̃p�g���[��
			{
				//�v���C���[��T���Ă�����
				if (g_EnemyBall[i].state == BALL_STATE_SEARCH)
				{
					if (g_EnemyBall[i].stateTimeCnt < g_EnemyBall[i].stateTime)
					{

						PLAYER* player = GetPlayer();

						//�v���C���[�Ɍ������Č��ɓ���
						if (g_EnemyBall[i].pos.x < player[0].pos.x)
						{
							g_EnemyBall[i].moveDir   = BALL_DIR_RIGHT;
							g_EnemyBall[i].moveSpeed = -SEARCH_STATE_SPEED;
						}
						else
						{
							g_EnemyBall[i].moveDir   = BALL_DIR_LEFT;
							g_EnemyBall[i].moveSpeed = SEARCH_STATE_SPEED;
						}

						g_EnemyBall[i].pos.x += g_EnemyBall[i].moveSpeed;	//�v���C���[�̔��Α��ɓ����i�\���Ă���j

						g_EnemyBall[i].stateTimeCnt++;

						
					}
					else	//�T����Ԃ��I���
					{
						g_EnemyBall[i].state		= BALL_STATE_RUSH;		//�ˌ���Ԃɕς��
						g_EnemyBall[i].stateTimeCnt = 0;					//�A�N�V������Ԏ��Ԃ����Z�b�g
						g_EnemyBall[i].moveSpeed   *= -1;					//���x���v���C���[�̕��ɕς��
					}
					
					
				}

				//�ˌ����
				if (g_EnemyBall[i].state == BALL_STATE_RUSH)
				{
					g_EnemyBall[i].pos.x += g_EnemyBall[i].moveSpeed;

					if (fabs(g_EnemyBall[i].moveSpeed) < RUSH_STATE_SPEED)
					{
						g_EnemyBall[i].moveSpeed += g_EnemyBall[i].moveSpeed;
					}
				}

				BallJumpUpdate();
				
			}
		}
		else
		{
			DespawnEnemyBall(i);
		}
			

		//�G�l�~�[�����t����
		{
			if (g_EnemyBall[i].damaged == TRUE)
			{
				//�m�b�N�o�b�N
				if (g_EnemyBall[i].dmgTimeCnt < KNOCKBACK_TIME)	//�͂˕Ԃ������͍ŏ���15�t���[������
				{
					XMVECTOR epos = XMLoadFloat3(&g_EnemyBall[i].pos);
					XMVECTOR vec  = (epos - XMLoadFloat3(&g_EnemyBall[i].damageOriginPos));		//�����ƃ_���[�W�̂Ƃ���Ƃ̍��������߂�
					float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);						//���̍������g���Ċp�x�����߂Ă���
					float repealDisstance = 15.0f;												//�X�s�[�h�͂�����ƒx�����Ă݂�
					float repealDisstancePerFrame = repealDisstance / KNOCKBACK_TIME;

					//SMOOTH
					float repealDistNow = repealDisstancePerFrame * (KNOCKBACK_TIME - g_EnemyBall[i].dmgTimeCnt);

					g_EnemyBall[i].pos.x += cosf(angle) * repealDistNow;					//angle�̕����ֈړ�
					g_EnemyBall[i].pos.y += sinf(angle) * repealDistNow;					//angle�̕����ֈړ�
				}
		

				g_EnemyBall[i].dmgTimeCnt++;

				//�_���[�W���I�������
				if (g_EnemyBall[i].dmgTimeCnt >= DAMAGE_TIME)
				{
					g_EnemyBall[i].damaged	  = FALSE;
					g_EnemyBall[i].dmgTimeCnt = 0;
					
					g_EnemyBall[i].enemyColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
					
				}
			}


		}

		//�d��
		g_EnemyBall[i].pos.y += BALL_GRAVITY_FORCE;




		// Ground�Ƃ̓����蔻��
		{
			PLATFORM* ground = GetGround();

			// groundS�̐��������蔻����s��
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;

				BOOL ans = CheckCollisionAndGetDirection(ground[j].pos.x,		ground[j].pos.y,		(ground[j].w),		(ground[j].h),
														 g_EnemyBall[i].pos.x,	g_EnemyBall[i].pos.y,	(g_EnemyBall[i].w), (g_EnemyBall[i].h));
				// �������Ă���H

				switch (ans)
				{
				case FromTop:
					g_EnemyBall[i].pos.y = ground[j].pos.y - (ground[j].h / 2) - (g_EnemyBall[i].h / 2);
					break;

				case FromBottom:
					g_EnemyBall[i].pos.y = ground[j].pos.y + (ground[j].h / 2) + (g_EnemyBall[i].h / 2);
					break;

				case FromLeft:
					g_EnemyBall[i].pos.x = ground[j].pos.x - (ground[j].w / 2) - (g_EnemyBall[i].w / 2);

					//���ɂԂ�������JUMP��Ԃɕς��
					if (g_EnemyBall[i].pos.y > (ground[j].pos.y - ground[j].h / 2) && g_EnemyBall[i].pos.y < (ground[j].pos.y + ground[j].h / 2))
					{
						if (g_EnemyBall[i].state == BALL_STATE_RUSH)
						{
							g_EnemyBall[i].state	 = BALL_STATE_JUMP;
							g_EnemyBall[i].stateTime = STATE_TIME_JUMP;
						}
					}
					break;

				case FromRight:
					g_EnemyBall[i].pos.x = ground[j].pos.x + (ground[j].w / 2) + (g_EnemyBall[i].w / 2);

					//���ɂԂ�������JUMP��Ԃɕς��
					if (g_EnemyBall[i].pos.y > (ground[j].pos.y - ground[j].h / 2) && g_EnemyBall[i].pos.y < (ground[j].pos.y + ground[j].h / 2))
					{
						if (g_EnemyBall[i].state == BALL_STATE_RUSH)
						{
							g_EnemyBall[i].state	 = BALL_STATE_JUMP;
							g_EnemyBall[i].stateTime = STATE_TIME_JUMP;
						}
					}
					break;

				default:
							
					break;
				}

					
			}
		}


		// PLATFORM �Ƃ̓����蔻��
		{
			PLATFORM_ANIM* platform = GetPlatforms();

			// platformS�̐��������蔻����s��
			for (int j = 0; j < PLATFORM_MAX; j++)
			{
				if (platform[j].collider == FALSE) continue;

				float platformH = 50.0f;

				BOOL ans = CheckCollisionAndGetDirection(platform[j].pos.x,		platform[j].pos.y,		(platform[j].w),	(platformH),
														 g_EnemyBall[i].pos.x,	g_EnemyBall[i].pos.y,	(g_EnemyBall[i].w), (g_EnemyBall[i].h));

				// �������Ă���H
				switch (ans)
				{
				case FromTop:
					g_EnemyBall[i].pos.y = platform[j].pos.y - (platformH / 2) - (g_EnemyBall[i].h / 2);
					break;

				case FromBottom:
					g_EnemyBall[i].pos.y = platform[j].pos.y + (platformH / 2) + (g_EnemyBall[i].h / 2);
					break;

				case FromLeft:
					g_EnemyBall[i].pos.x = platform[j].pos.x - (platform[j].w / 2) - (g_EnemyBall[i].w / 2);

					//���ɂԂ�������JUMP��Ԃɕς��
					if (g_EnemyBall[i].pos.y > (platform[j].pos.y - platformH / 2) && g_EnemyBall[i].pos.y < (platform[j].pos.y + platformH / 2))
					{
						if (g_EnemyBall[i].state == BALL_STATE_RUSH)
						{
							g_EnemyBall[i].state	 = BALL_STATE_JUMP;
							g_EnemyBall[i].stateTime = STATE_TIME_JUMP;
						}
					}
					break;

				case FromRight:
					g_EnemyBall[i].pos.x = platform[j].pos.x + (platform[j].w / 2) + (g_EnemyBall[i].w / 2);

					//���ɂԂ�������JUMP��Ԃɕς��
					if (g_EnemyBall[i].pos.y > (platform[j].pos.y - platformH / 2) && g_EnemyBall[i].pos.y < (platform[j].pos.y + platformH / 2))
					{
						if (g_EnemyBall[i].state == BALL_STATE_RUSH)
						{
							g_EnemyBall[i].state	 = BALL_STATE_JUMP;
							g_EnemyBall[i].stateTime = STATE_TIME_JUMP;
						}
					}
					break;

				default:

					break;
				}


			}
		}



	}


	// �A�j���[�V����
	EnemyBallAnimationUpdate();


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemyBall(void)
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

	for (int i = 0; i < ENEMY_BALL_MAX; i++)
	{
		if (g_EnemyBall[i].spawned == FALSE) continue;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnemyBall[i].texNo]);

		//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_EnemyBall[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
		float py = g_EnemyBall[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
		float pw = g_EnemyBall[i].w;					// �G�l�~�[�̕\����
		float ph = g_EnemyBall[i].h;					// �G�l�~�[�̕\������

		float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// �e�N�X�`���̕�
		float th = 1.0f / BALL_ANIM_MAX;				// �e�N�X�`���̍���
		float tx = tw * g_EnemyBall[i].currentSprite;	// �e�N�X�`���̍���X���W
		float ty = th * g_EnemyBall[i].currentAnim;		// �e�N�X�`���̍���Y���W

		//�E�Ɍ�������e�N�X�`���𔽓]����
		if (g_EnemyBall[i].moveDir == BALL_DIR_RIGHT)
		{
			tx += tw;
			tw *= -1.0f;
		}

		ty += 0.01f;	//�X�v���C�g�̏�ɕςȐ�������Ȃ��悤�Ƀe�N�X�`����������Ƃ��炷
		


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					   g_EnemyBall[i].enemyColor);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}


//=============================================================================
// EnemyBall�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY_BALL* GetEnemyBall(void)
{
	return &g_EnemyBall[0];
}

//=============================================================================
// EnemyBall���_���[�W���󂯂�
// ����: playerPos (�v���C���[�̍��W), enemyIndex (�G�l�~�[�̃C���f�N�X), damage (�_���[�W��)
//=============================================================================
void EnemyBallDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage)
{
	//�_���[�W�e��
	g_EnemyBall[enemyIndex].damaged			= TRUE;
	g_EnemyBall[enemyIndex].damageOriginPos = playerPos;
	g_EnemyBall[enemyIndex].HP				-= damage;
	g_EnemyBall[enemyIndex].enemyColor		= XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f);

	//�A�j���[�V����
	g_EnemyBall[enemyIndex].countAnim		= ANIM_WAIT + 1;
	g_EnemyBall[enemyIndex].currentSprite	= 0;

	PlaySound(SOUND_LABEL_SE_bloodyBlade);

	//���񂾂�
	if (g_EnemyBall[enemyIndex].HP <= 0)
	{
		g_EnemyBall[enemyIndex].alive		= FALSE;
		g_EnemyBall[enemyIndex].enemyColor  = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		RoundKill();
	}
}


//=============================================================================
// EnemyBall�A�j���[�V��������
//=============================================================================
void EnemyBallAnimationUpdate(void)
{
	for (int i = 0; i < ENEMY_BALL_MAX; i++)
	{
		//�A�j���[�V�����̔��f
		if (g_EnemyBall[i].alive == FALSE)
		{
			g_EnemyBall[i].currentAnim = BALL_ANIM_DIE;
		}
		else if (g_EnemyBall[i].damaged == TRUE)
		{
			g_EnemyBall[i].currentAnim = BALL_ANIM_HIT;
		}
		else if (g_EnemyBall[i].state == BALL_STATE_SEARCH)
		{
			g_EnemyBall[i].currentAnim = BALL_ANIM_SEARCH;
		}
		else
		{
			g_EnemyBall[i].currentAnim = BALL_ANIM_MOVE;
		}

		g_EnemyBall[i].countAnim ++;


		if (g_EnemyBall[i].countAnim > ANIM_WAIT)
		{
			g_EnemyBall[i].countAnim = 0;

			//���[�v�A�j���[�V����
			if (g_EnemyBall[i].currentAnim == BALL_ANIM_MOVE) 
			{
				g_EnemyBall[i].currentSprite = (g_EnemyBall[i].currentSprite + 1) % TEXTURE_ANIM_SPRITES_MAX;
			}
			else //NO ���[�v�A�j���[�V����
			{
				if (g_EnemyBall[i].currentSprite < TEXTURE_ANIM_SPRITES_MAX - 1)
				{
					g_EnemyBall[i].currentSprite++;
				}
				
			}
			
		}
	}
}


//=============================================================================
// ���˂鏈��
//=============================================================================
void BallJumpUpdate(void)
{
	for (int i = 0; i < ENEMY_BALL_MAX; i++)
	{
		if (g_EnemyBall[i].spawned == FALSE) continue;

		if (g_EnemyBall[i].state == BALL_STATE_JUMP)
		{

			float angle;		//���̍������g���Ċp�x�����߂Ă���

			if (g_EnemyBall[i].moveDir == BALL_DIR_LEFT)
			{
				angle = -XM_PIDIV4;		//����̊p�x
			}
			else
			{
				angle = -XM_PIDIV4 * 3;	//�E��̊p�x
			}


			float repealDisstance			= 10.0f;										//�X�s�[�h�͂�����ƒx�����Ă݂�
			float repealDisstancePerFrame	= repealDisstance / g_EnemyBall[i].stateTime;

			float repealDistNow				= repealDisstancePerFrame * (g_EnemyBall[i].stateTime - g_EnemyBall[i].stateTimeCnt);


			g_EnemyBall[i].pos.x += cosf(angle) * repealDistNow;					//angle�̕����ֈړ�
			g_EnemyBall[i].pos.y += sinf(angle) * repealDistNow;					//angle�̕����ֈړ�

			g_EnemyBall[i].stateTimeCnt++;

			//�I�����SEARCH��Ԃɕς��
			if (g_EnemyBall[i].stateTimeCnt >= g_EnemyBall[i].stateTime)
			{
				g_EnemyBall[i].state		= BALL_STATE_SEARCH;
				g_EnemyBall[i].stateTime	= 30 + (rand() % 200);
				g_EnemyBall[i].stateTimeCnt = 0;
			}
		}
	}
	
}

//=============================================================================
// EnemyBall���o��������
// ����: spawnPos(�o���������W)
//=============================================================================
void SpawnEnemyBall(XMFLOAT3 spawnPos)
{
	for (int i = 0; i < ENEMY_BALL_MAX; i++)
	{
		if (g_EnemyBall[i].spawned == FALSE)
		{
			g_EnemyBall[i].spawned	= TRUE;
			g_EnemyBall[i].alive	= TRUE;
			g_EnemyBall[i].pos		= spawnPos;
			break;
		}
	}
}


//=============================================================================
// EnemyBall�𖢎g�p�ɂ���
//=============================================================================
void DespawnEnemyBall(int enemyIndex)
{
	g_EnemyBall[enemyIndex].dmgTimeCnt++;

	if (g_EnemyBall[enemyIndex].dmgTimeCnt >= DESPAWN_TIME)
	{
		//������Ƃ������ɂ���
		if (g_EnemyBall[enemyIndex].enemyColor.w > 0)
		{
			g_EnemyBall[enemyIndex].enemyColor.w -= 0.02f;
			return;
		}

		//�����������烊�Z�b�g����
		g_EnemyBall[enemyIndex].spawned			= FALSE;
		g_EnemyBall[enemyIndex].alive			= FALSE;
		g_EnemyBall[enemyIndex].pos				= XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_EnemyBall[enemyIndex].currentSprite	= 0;
		g_EnemyBall[enemyIndex].currentAnim		= BALL_ANIM_MOVE;

		g_EnemyBall[enemyIndex].moveSpeed		= SEARCH_STATE_SPEED;
		g_EnemyBall[enemyIndex].moveDir			= BALL_DIR_RIGHT;

		g_EnemyBall[enemyIndex].HP				= HP_MAX;

		g_EnemyBall[enemyIndex].damaged			= FALSE;
		g_EnemyBall[enemyIndex].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyBall[enemyIndex].dmgTimeCnt		= 0;

		g_EnemyBall[enemyIndex].enemyColor		= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

		g_EnemyBall[enemyIndex].state			= BALL_STATE_SEARCH;
		g_EnemyBall[enemyIndex].stateTime		= STATE_TIME_FIRST_SEARCH;
	}
}
