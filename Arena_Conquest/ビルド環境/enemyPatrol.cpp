//=============================================================================
//
// �G�l�~�[���� [enemyPatrol.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemyPatrol.h"
#include "spawnController.h"
#include "bg.h"
#include "platforms.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(30 * 4)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(16 * 4)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_ANIM_SPRITES_MAX	(4)		// 1�̃A�j���[�V�����̓X�v���C�g�����iX)
#define TEXTURE_ANIM_MAX			(3)		// �A�j���[�V�����͂�����			  �iY)
#define ANIM_WAIT					(10)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define DAMAGE_TIME					(30)
#define KNOCKBACK_TIME				(15)
#define DESPAWN_TIME				(180)
#define HP_MAX						(20.0f)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void EnemyAnimationUpdate(void);

void DespawnEnemyPatrol(int enemyIndex);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/Slime/Slime_aprite_sheet.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static ENEMY_PATROL	g_EnemyPatrol[ENEMY_PATROL_MAX];		// �G�l�~�[�\����
static int g_ScenePatrolEnemies;



//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemyPatrol(void)
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

	switch (GetMode())
	{
	case MODE_TUTORIAL_1:
		g_ScenePatrolEnemies = 0;

		for (int i = 0; i < ENEMY_PATROL_MAX; i++)
		{
			if (i < g_ScenePatrolEnemies)
			{
				//NO ENEMY
			}
			else
			{
				g_EnemyPatrol[i].spawned = FALSE;
				g_EnemyPatrol[i].alive   = FALSE;
			}

		}
		break;

	case MODE_TUTORIAL_2:
		g_ScenePatrolEnemies = 4;

		g_EnemyPatrol[0].pos = XMFLOAT3(1500.0f, 1465.0f, 0.0f);	// ���S�_����\��
		g_EnemyPatrol[1].pos = XMFLOAT3(1800.0f, 1465.0f, 0.0f);	// ���S�_����\��
		g_EnemyPatrol[2].pos = XMFLOAT3(2930.0f, 1365.0f, 0.0f);	// ���S�_����\��
		g_EnemyPatrol[3].pos = XMFLOAT3(5400.0f, 1465.0f, 0.0f);	// ���S�_����\��

		for (int i = 0; i < ENEMY_PATROL_MAX; i++)
		{
			if (i < g_ScenePatrolEnemies)
			{
				g_EnemyPatrol[i].spawned = TRUE;
				g_EnemyPatrol[i].alive = TRUE;
				//g_EnemyPatrol[i].pos = XMFLOAT3(200.0f + i * 600.0f, 1080.0f - 200.0f, 0.0f);	// ���S�_����\��
				g_EnemyPatrol[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
				g_EnemyPatrol[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
				g_EnemyPatrol[i].w = TEXTURE_WIDTH;
				g_EnemyPatrol[i].h = TEXTURE_HEIGHT;
				g_EnemyPatrol[i].texNo = 0;

				g_EnemyPatrol[i].currentSprite = 0;
				g_EnemyPatrol[i].currentAnim   = SLIME_ANIM_MOVE;

				g_EnemyPatrol[i].moveSpeed = 2.0f;		// �ړ���
				g_EnemyPatrol[i].moveDir = SLIME_DIR_RIGHT;
				g_EnemyPatrol[i].moveTimeCnt = 0;

				g_EnemyPatrol[i].HP = HP_MAX;

				g_EnemyPatrol[i].damaged = FALSE;
				g_EnemyPatrol[i].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
				g_EnemyPatrol[i].dmgTimeCnt = 0;

				g_EnemyPatrol[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				g_EnemyPatrol[i].platformIndex = 99;
				g_EnemyPatrol[i].InGround = TRUE;
			}
			else 
			{
				g_EnemyPatrol[i].spawned = FALSE;
				g_EnemyPatrol[i].alive = FALSE;
			}
			
		}
		g_EnemyPatrol[2].HP = 10.0f;

		break;

	case MODE_COLISEUM:

		for (int i = 0; i < ENEMY_PATROL_MAX; i++)
		{
			g_EnemyPatrol[i].spawned = FALSE;
			g_EnemyPatrol[i].alive = FALSE;
			g_EnemyPatrol[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// ���S�_����\��
			g_EnemyPatrol[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_EnemyPatrol[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_EnemyPatrol[i].w = TEXTURE_WIDTH;
			g_EnemyPatrol[i].h = TEXTURE_HEIGHT;
			g_EnemyPatrol[i].texNo = 0;

			g_EnemyPatrol[i].currentSprite = 0;
			g_EnemyPatrol[i].currentAnim = SLIME_ANIM_MOVE;

			g_EnemyPatrol[i].moveSpeed = 2.0f + (i * 0.5f);		// �ړ���
			g_EnemyPatrol[i].moveDir = SLIME_DIR_RIGHT;

			g_EnemyPatrol[i].HP = HP_MAX;

			g_EnemyPatrol[i].damaged = FALSE;
			g_EnemyPatrol[i].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_EnemyPatrol[i].dmgTimeCnt = 0;

			g_EnemyPatrol[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			g_EnemyPatrol[i].platformIndex = 99;

		}

		break;
	}
	// �G�l�~�[�\���̂̏�����
	


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemyPatrol(void)
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
void UpdateEnemyPatrol(void)
{

	for (int i = 0; i < ENEMY_PATROL_MAX; i++)
	{
		g_EnemyPatrol[i].moveTimeCnt++;


		// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
		XMFLOAT3 pos_old = g_EnemyPatrol[i].pos;


		// �����Ă�G�l�~�[��������������
		if (g_EnemyPatrol[i].alive == TRUE)
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
						BOOL ans = CollisionBB(g_EnemyPatrol[i].pos, g_EnemyPatrol[i].w, g_EnemyPatrol[i].h,
											   player[j].pos,		 player[j].w,		 player[j].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							PlayerDamaged(g_EnemyPatrol[i].pos);
						}
					}
				}
			}

			//�G�l�~�[�̃p�g���[��
			{
				if (g_EnemyPatrol[i].platformIndex != 99)
				{
					g_EnemyPatrol[i].pos.x += g_EnemyPatrol[i].moveSpeed;

					float platformLeftSide;
					float platformRightSide;

					if (g_EnemyPatrol[i].InGround == TRUE)
					{
						PLATFORM* ground = GetGround();
						platformLeftSide = ground[g_EnemyPatrol[i].platformIndex].pos.x - (ground[g_EnemyPatrol[i].platformIndex].w / 2) + g_EnemyPatrol[i].w / 2;
						platformRightSide = ground[g_EnemyPatrol[i].platformIndex].pos.x + (ground[g_EnemyPatrol[i].platformIndex].w / 2) - g_EnemyPatrol[i].w / 2;
					}
					else
					{
						PLATFORM_ANIM* platform = GetPlatforms();
						platformLeftSide = platform[g_EnemyPatrol[i].platformIndex].pos.x - (platform[g_EnemyPatrol[i].platformIndex].w / 2) + g_EnemyPatrol[i].w / 2;
						platformRightSide = platform[g_EnemyPatrol[i].platformIndex].pos.x + (platform[g_EnemyPatrol[i].platformIndex].w / 2) - g_EnemyPatrol[i].w / 2;
					}
					

					if (g_EnemyPatrol[i].pos.x < platformLeftSide ||
						g_EnemyPatrol[i].pos.x > platformRightSide)
					{
						if (g_EnemyPatrol[i].moveTimeCnt > 30)
						{
							g_EnemyPatrol[i].moveSpeed *= -1;
							g_EnemyPatrol[i].moveTimeCnt = 0;
						}
						
					}
				}
			}


			//�G�l�~�[�����t����
			{
				if (g_EnemyPatrol[i].damaged == TRUE)
				{
					if (g_EnemyPatrol[i].dmgTimeCnt < KNOCKBACK_TIME)	//�͂˕Ԃ������͍ŏ���15�t���[������
					{
						g_EnemyPatrol[i].platformIndex = 99;	//�G�l�~�[�����������Ȃ��悤��platformIndex�����Z�b�g����

						XMVECTOR epos = XMLoadFloat3(&g_EnemyPatrol[i].pos);
						XMVECTOR vec = (epos - XMLoadFloat3(&g_EnemyPatrol[i].damageOriginPos));			//�����ƃv���C���[�Ƃ̍��������߂�
						float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);		//���̍������g���Ċp�x�����߂Ă���
						float repealDisstance = 20.0f;										//�X�s�[�h�͂�����ƒx�����Ă݂�
						float repealDisstancePerFrame = repealDisstance / KNOCKBACK_TIME;

						//SMOOTH
						float repealDistNow = repealDisstancePerFrame * (KNOCKBACK_TIME - g_EnemyPatrol[i].dmgTimeCnt);



						g_EnemyPatrol[i].pos.x += cosf(angle) * repealDistNow;					//angle�̕����ֈړ�
						g_EnemyPatrol[i].pos.y += sinf(angle) * repealDistNow;					//angle�̕����ֈړ�
					}
					

					g_EnemyPatrol[i].dmgTimeCnt++;

					if (g_EnemyPatrol[i].dmgTimeCnt >= DAMAGE_TIME)
					{
						g_EnemyPatrol[i].damaged = FALSE;
						g_EnemyPatrol[i].dmgTimeCnt = 0;

						g_EnemyPatrol[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


					}
				}
			}
			


			//SLIME�̌���
			if (g_EnemyPatrol[i].pos.x < pos_old.x)
			{
				g_EnemyPatrol[i].moveDir = SLIME_DIR_LEFT;
			}
			else if (g_EnemyPatrol[i].pos.x > pos_old.x)
			{
				g_EnemyPatrol[i].moveDir = SLIME_DIR_RIGHT;
			}
		}
		else
		{
			if (g_EnemyPatrol[i].spawned == TRUE)
			{
				DespawnEnemyPatrol(i);
			}
		}
			

		

		//�d��
		g_EnemyPatrol[i].pos.y += 5.0f;

		




		// Ground �Ƃ̓����蔻��
		{
			PLATFORM* ground = GetGround();

			// groundS�̐��������蔻����s��
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;

				BOOL ans = CheckCollisionAndGetDirection(ground[j].pos.x, ground[j].pos.y, (ground[j].w), (ground[j].h),
					g_EnemyPatrol[i].pos.x, g_EnemyPatrol[i].pos.y, (g_EnemyPatrol[i].w), (g_EnemyPatrol[i].h));
				// �������Ă���H

				switch (ans)
				{
				case FromTop:
					g_EnemyPatrol[i].platformIndex = j;
					g_EnemyPatrol[i].InGround = TRUE;
					g_EnemyPatrol[i].pos.y = ground[j].pos.y - (ground[j].h / 2) - (g_EnemyPatrol[i].h / 2);
					break;

				case FromBottom:
					g_EnemyPatrol[i].pos.y = ground[j].pos.y + (ground[j].h / 2) + (g_EnemyPatrol[i].h / 2);
					break;

				case FromLeft:
					g_EnemyPatrol[i].pos.x = ground[j].pos.x - (ground[j].w / 2) - (g_EnemyPatrol[i].w / 2);
					g_EnemyPatrol[i].moveSpeed *= -1;
					g_EnemyPatrol[i].moveTimeCnt = 0;
					break;

				case FromRight:
						g_EnemyPatrol[i].pos.x = ground[j].pos.x + (ground[j].w / 2) + (g_EnemyPatrol[i].w / 2);
						g_EnemyPatrol[i].moveSpeed *= -1;
						g_EnemyPatrol[i].moveTimeCnt = 0;
					break;

				default:
							
					break;
				}

					
			}
		}



		// PLATFORMS�Ƃ̓����蔻��
		{
			PLATFORM_ANIM* platform = GetPlatforms();

			// PLATFORMS�̐��������蔻����s��
			for (int j = 0; j < PLATFORM_MAX; j++)
			{
				if (platform[j].collider == FALSE) continue;

				float platformH = 50.0f;

				BOOL ans = CheckCollisionAndGetDirection(platform[j].pos.x, platform[j].pos.y, (platform[j].w), (platformH),
					g_EnemyPatrol[i].pos.x, g_EnemyPatrol[i].pos.y, (g_EnemyPatrol[i].w), (g_EnemyPatrol[i].h));
				// �������Ă���H

				switch (ans)
				{
				case FromTop:
					g_EnemyPatrol[i].platformIndex = j;
					g_EnemyPatrol[i].InGround = FALSE;
					g_EnemyPatrol[i].pos.y = platform[j].pos.y - (platformH / 2) - (g_EnemyPatrol[i].h / 2);
					break;

				case FromBottom:
					g_EnemyPatrol[i].pos.y = platform[j].pos.y + (platformH / 2) + (g_EnemyPatrol[i].h / 2);
					break;

				case FromLeft:
					g_EnemyPatrol[i].pos.x = platform[j].pos.x - (platform[j].w / 2) - (g_EnemyPatrol[i].w / 2);
					g_EnemyPatrol[i].moveSpeed *= -1;
					break;

				case FromRight:
					g_EnemyPatrol[i].pos.x = platform[j].pos.x + (platform[j].w / 2) + (g_EnemyPatrol[i].w / 2);
					g_EnemyPatrol[i].moveSpeed *= -1;
					break;

				default:

					break;
				}
			}
		}


	}


	// �A�j���[�V����
	EnemyAnimationUpdate();


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemyPatrol(void)
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

	for (int i = 0; i < ENEMY_PATROL_MAX; i++)
	{
		if (g_EnemyPatrol[i].spawned == TRUE)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnemyPatrol[i].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_EnemyPatrol[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_EnemyPatrol[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_EnemyPatrol[i].w;					// �G�l�~�[�̕\����
			float ph = g_EnemyPatrol[i].h;					// �G�l�~�[�̕\������

			float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_ANIM_MAX;				// �e�N�X�`���̍���
			float tx = tw * g_EnemyPatrol[i].currentSprite;	// �e�N�X�`���̍���X���W
			float ty = th * g_EnemyPatrol[i].currentAnim;	// �e�N�X�`���̍���Y���W

			if (g_EnemyPatrol[i].moveDir == SLIME_DIR_RIGHT)
			{
				tx += tw;
				tw *= -1.0f;
			}

			ty += 0.01f;



			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_EnemyPatrol[i].enemyColor);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

		
	}

}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY_PATROL* GetEnemyPatrol(void)
{
	return &g_EnemyPatrol[0];
}


void EnemyDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage)
{
	g_EnemyPatrol[enemyIndex].damaged = TRUE;
	g_EnemyPatrol[enemyIndex].damageOriginPos = playerPos;
	g_EnemyPatrol[enemyIndex].enemyColor = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	g_EnemyPatrol[enemyIndex].HP -= damage;
	PlaySound(SOUND_LABEL_SE_bloodyBlade);
	g_EnemyPatrol[enemyIndex].countAnim = ANIM_WAIT + 1;
	g_EnemyPatrol[enemyIndex].currentSprite = 0;

	if (g_EnemyPatrol[enemyIndex].HP <= 0)
	{
		g_EnemyPatrol[enemyIndex].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_EnemyPatrol[enemyIndex].alive = FALSE;
		RoundKill();
		//Death animation
		
	}
}

void EnemyAnimationUpdate(void)
{
	for (int i = 0; i < ENEMY_PATROL_MAX; i++)
	{
		if (g_EnemyPatrol[i].alive == FALSE)
		{
			g_EnemyPatrol[i].currentAnim = SLIME_ANIM_DIE;
		}
		else if (g_EnemyPatrol[i].damaged == TRUE)
		{
			g_EnemyPatrol[i].currentAnim = SLIME_ANIM_HIT;
		}
		else
		{
			g_EnemyPatrol[i].currentAnim = SLIME_ANIM_MOVE;
		}

		g_EnemyPatrol[i].countAnim ++;


		if (g_EnemyPatrol[i].countAnim > ANIM_WAIT)
		{
			g_EnemyPatrol[i].countAnim = 0;

			// �p�^�[���̐؂�ւ�
			if (g_EnemyPatrol[i].currentAnim == SLIME_ANIM_MOVE) //���[�v�A�j���[�V����
			{
				g_EnemyPatrol[i].currentSprite = (g_EnemyPatrol[i].currentSprite + 1) % TEXTURE_ANIM_SPRITES_MAX;
			}
			else //NO ���[�v�A�j���[�V����
			{
				if (g_EnemyPatrol[i].currentSprite < TEXTURE_ANIM_SPRITES_MAX - 1)
				{
					g_EnemyPatrol[i].currentSprite++;
				}
				
			}
			
		}
	}
}


void SpawnEnemyPatrol(XMFLOAT3 spawnPos)
{
	for (int i = 0; i < ENEMY_PATROL_MAX; i++)
	{
		if (g_EnemyPatrol[i].spawned == FALSE)
		{
			g_EnemyPatrol[i].spawned	= TRUE;
			g_EnemyPatrol[i].alive		= TRUE;
			g_EnemyPatrol[i].pos		= spawnPos;

			PLAYER* player = GetPlayer();
			if (g_EnemyPatrol[i].pos.x > player[0].pos.x)
			{
				g_EnemyPatrol[i].moveSpeed *= -1;
			}
		
			break;
		}
	}
}


void DespawnEnemyPatrol(int enemyIndex)
{
	g_EnemyPatrol[enemyIndex].dmgTimeCnt++;

	if (g_EnemyPatrol[enemyIndex].dmgTimeCnt >= DESPAWN_TIME)
	{
		if(g_EnemyPatrol[enemyIndex].enemyColor.w > 0)
		{
			g_EnemyPatrol[enemyIndex].enemyColor.w -= 0.02f;
			return;
		}

		g_EnemyPatrol[enemyIndex].spawned = FALSE;
		g_EnemyPatrol[enemyIndex].alive = FALSE;
		g_EnemyPatrol[enemyIndex].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// ���S�_����\��

		g_EnemyPatrol[enemyIndex].currentSprite = 0;
		g_EnemyPatrol[enemyIndex].currentAnim = SLIME_ANIM_MOVE;

		g_EnemyPatrol[enemyIndex].moveSpeed = 2.0f;		// �ړ���
		g_EnemyPatrol[enemyIndex].moveDir = SLIME_DIR_RIGHT;

		g_EnemyPatrol[enemyIndex].HP = HP_MAX;

		g_EnemyPatrol[enemyIndex].damaged = FALSE;
		g_EnemyPatrol[enemyIndex].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyPatrol[enemyIndex].dmgTimeCnt = 0;

		g_EnemyPatrol[enemyIndex].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_EnemyPatrol[enemyIndex].platformIndex = 99;
	}
}