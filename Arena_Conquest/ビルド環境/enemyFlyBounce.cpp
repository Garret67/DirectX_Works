//=============================================================================
//
// �G�l�~�[���� [enemyFlyBounce.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemyFlyBounce.h"
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
#define TEXTURE_WIDTH				(100.0f)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(100.0f)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_ANIM_SPRITES_MAX	(10)		// 1�̃A�j���[�V�����̓X�v���C�g�����iX)
#define TEXTURE_ANIM_MAX			(3)		// �A�j���[�V�����͂�����			  �iY)
#define ANIM_WAIT					(7)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define DAMAGE_TIME					(30)
#define KNOCKBACK_TIME				(15)
#define DESPAWN_TIME				(180)
#define HP_MAX						(20.0f)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void EnemyFlyBounceAnimationUpdate(void);

void DespawnEnemyFlyBounce(int enemyIndex);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/Wispy/Wispy_anim.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static ENEMY_FLY_BOUNCE	g_EnemyFlyBounce[ENEMY_FLY_BOUNCE_MAX];		// �G�l�~�[�\����



//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemyFlyBounce(void)
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
	for (int i = 0; i < ENEMY_FLY_BOUNCE_MAX; i++)
	{	
		g_EnemyFlyBounce[i].spawned = FALSE;
		g_EnemyFlyBounce[i].alive = FALSE;
		g_EnemyFlyBounce[i].pos = XMFLOAT3(SCREEN_CENTER_X, 1465.0f, 0.0f);	// ���S�_����\��
		g_EnemyFlyBounce[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyFlyBounce[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_EnemyFlyBounce[i].w = TEXTURE_WIDTH;
		g_EnemyFlyBounce[i].h = TEXTURE_HEIGHT;
		g_EnemyFlyBounce[i].texNo = 0;

		g_EnemyFlyBounce[i].currentSprite = 0;
		g_EnemyFlyBounce[i].currentAnim = FLY_BOUNCE_ANIM_MOVE;

		g_EnemyFlyBounce[i].moveSpeed = XMFLOAT3(3.0f + (i * 1.0f), -3.0f + (i * 1.0f), 1.0f);		// �ړ���
		g_EnemyFlyBounce[i].moveDir = FLY_BOUNCE_DIR_RIGHT;

		g_EnemyFlyBounce[i].HP = HP_MAX;

		g_EnemyFlyBounce[i].damaged = FALSE;
		g_EnemyFlyBounce[i].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyFlyBounce[i].dmgTimeCnt = 0;

		g_EnemyFlyBounce[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemyFlyBounce(void)
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
void UpdateEnemyFlyBounce(void)
{

	for (int i = 0; i < ENEMY_FLY_BOUNCE_MAX; i++)
	{
		if (g_EnemyFlyBounce[i].spawned == FALSE) continue;
		// �����Ă�G�l�~�[��������������
		if (g_EnemyFlyBounce[i].alive == TRUE)
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_EnemyFlyBounce[i].pos;

			//�G�l�~�[�̃p�g���[��
			{
				g_EnemyFlyBounce[i].pos.x += g_EnemyFlyBounce[i].moveSpeed.x;
				g_EnemyFlyBounce[i].pos.y += g_EnemyFlyBounce[i].moveSpeed.y;
				
			}


			


			//�G�l�~�[�����t����
			{
				if (g_EnemyFlyBounce[i].damaged == TRUE)
				{

					if (g_EnemyFlyBounce[i].dmgTimeCnt < KNOCKBACK_TIME)
					{
						XMVECTOR epos = XMLoadFloat3(&g_EnemyFlyBounce[i].pos);
						XMVECTOR vec = (epos - XMLoadFloat3(&g_EnemyFlyBounce[i].damageOriginPos));			//�����ƃv���C���[�Ƃ̍��������߂�
						float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);		//���̍������g���Ċp�x�����߂Ă���
						float repealDisstance = 10.0f;										//�X�s�[�h�͂�����ƒx�����Ă݂�
						float repealDisstancePerFrame = repealDisstance / KNOCKBACK_TIME;

						//SMOOTH
						float repealDistNow = repealDisstancePerFrame * (KNOCKBACK_TIME - g_EnemyFlyBounce[i].dmgTimeCnt);



						g_EnemyFlyBounce[i].pos.x += cosf(angle) * repealDistNow;					//angle�̕����ֈړ�
						g_EnemyFlyBounce[i].pos.y += sinf(angle) * repealDistNow;					//angle�̕����ֈړ�
					}


					g_EnemyFlyBounce[i].dmgTimeCnt++;

					if (g_EnemyFlyBounce[i].dmgTimeCnt >= DAMAGE_TIME)
					{
						g_EnemyFlyBounce[i].damaged = FALSE;
						g_EnemyFlyBounce[i].dmgTimeCnt = 0;
					}
				}


			}



			// PLAYER�Ƃ̓����蔻��
			{
				PLAYER* player = GetPlayer();

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (player[j].alive == TRUE && player[j].invincible == FALSE)
					{
						BOOL ans = CollisionBB(g_EnemyFlyBounce[i].pos, g_EnemyFlyBounce[i].w, g_EnemyFlyBounce[i].h,
							player[j].pos, player[j].w, player[j].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							PlayerDamaged(g_EnemyFlyBounce[i].pos);
						}
					}
				}
			}


			

			//��ʒ[�̓����蔻��
			{
				BG* bg = GetBG();
				if (g_EnemyFlyBounce[i].pos.x < (g_EnemyFlyBounce[i].w / 2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
				{
					g_EnemyFlyBounce[i].moveSpeed.x *= -1;
					g_EnemyFlyBounce[i].pos.x = g_EnemyFlyBounce[i].w / 2;
				}
				if (g_EnemyFlyBounce[i].pos.x > (bg->w - g_EnemyFlyBounce[i].w / 2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
				{
					g_EnemyFlyBounce[i].moveSpeed.x *= -1;
					g_EnemyFlyBounce[i].pos.x = bg->w - (g_EnemyFlyBounce[i].w / 2);
				}

				if (g_EnemyFlyBounce[i].pos.y < (g_EnemyFlyBounce[i].h / 2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
				{
					g_EnemyFlyBounce[i].moveSpeed.y *= -1;
					g_EnemyFlyBounce[i].pos.y = (g_EnemyFlyBounce[i].h / 2);
				}
				if (g_EnemyFlyBounce[i].pos.y > (bg->h - g_EnemyFlyBounce[i].h / 2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
				{
					g_EnemyFlyBounce[i].moveSpeed.y *= -1;
					g_EnemyFlyBounce[i].pos.y = bg->h - (g_EnemyFlyBounce[i].h / 2);
				}
			}
			

			//FLY_BOUNCE�̌���
			if (g_EnemyFlyBounce[i].pos.x < pos_old.x)
			{
				g_EnemyFlyBounce[i].moveDir = FLY_BOUNCE_DIR_LEFT;
			}
			else if (g_EnemyFlyBounce[i].pos.x > pos_old.x)
			{
				g_EnemyFlyBounce[i].moveDir = FLY_BOUNCE_DIR_RIGHT;
			}
		}
		else
		{
			//�d��
			g_EnemyFlyBounce[i].pos.y += 5.0f;

			DespawnEnemyFlyBounce(i);
		}
		
		// ground �Ƃ̓����蔻��
		{
			PLATFORM* ground = GetGround();

			// groundS�̐��������蔻����s��
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;

				BOOL ans = CheckCollisionAndGetDirection(ground[j].pos.x, ground[j].pos.y, (ground[j].w), (ground[j].h),
					g_EnemyFlyBounce[i].pos.x, g_EnemyFlyBounce[i].pos.y, (g_EnemyFlyBounce[i].w), (g_EnemyFlyBounce[i].h));
				// �������Ă���H

				switch (ans)
				{
				case FromTop:
					g_EnemyFlyBounce[i].pos.y = ground[j].pos.y - (ground[j].h / 2) - (g_EnemyFlyBounce[i].h / 2);
					g_EnemyFlyBounce[i].moveSpeed.y *= -1;
					break;

				case FromBottom:
					g_EnemyFlyBounce[i].pos.y = ground[j].pos.y + (ground[j].h / 2) + (g_EnemyFlyBounce[i].h / 2);
					g_EnemyFlyBounce[i].moveSpeed.y *= -1;
					break;

				case FromLeft:
					g_EnemyFlyBounce[i].pos.x = ground[j].pos.x - (ground[j].w / 2) - (g_EnemyFlyBounce[i].w / 2);
					g_EnemyFlyBounce[i].moveSpeed.x *= -1;
					break;

				case FromRight:
					g_EnemyFlyBounce[i].pos.x = ground[j].pos.x + (ground[j].w / 2) + (g_EnemyFlyBounce[i].w / 2);
					g_EnemyFlyBounce[i].moveSpeed.x *= -1;
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

				BOOL ans = CheckCollisionAndGetDirection(platform[j].pos.x, platform[j].pos.y, (platform[j].w), (platformH),
					g_EnemyFlyBounce[i].pos.x, g_EnemyFlyBounce[i].pos.y, (g_EnemyFlyBounce[i].w), (g_EnemyFlyBounce[i].h));
				// �������Ă���H

				switch (ans)
				{
				case FromTop:
					g_EnemyFlyBounce[i].pos.y = platform[j].pos.y - (platformH / 2) - (g_EnemyFlyBounce[i].h / 2);
					g_EnemyFlyBounce[i].moveSpeed.y *= -1;
					break;

				case FromBottom:
					g_EnemyFlyBounce[i].pos.y = platform[j].pos.y + (platformH / 2) + (g_EnemyFlyBounce[i].h / 2);
					g_EnemyFlyBounce[i].moveSpeed.y *= -1;
					break;

				case FromLeft:
					g_EnemyFlyBounce[i].pos.x = platform[j].pos.x - (platform[j].w / 2) - (g_EnemyFlyBounce[i].w / 2);
					g_EnemyFlyBounce[i].moveSpeed.x *= -1;
					break;

				case FromRight:
					g_EnemyFlyBounce[i].pos.x = platform[j].pos.x + (platform[j].w / 2) + (g_EnemyFlyBounce[i].w / 2);
					g_EnemyFlyBounce[i].moveSpeed.x *= -1;
					break;

				default:

					break;
				}


			}
		}

	}


	// �A�j���[�V����
	EnemyFlyBounceAnimationUpdate();


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemyFlyBounce(void)
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

	for (int i = 0; i < ENEMY_FLY_BOUNCE_MAX; i++)
	{
		if (g_EnemyFlyBounce[i].spawned == TRUE)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnemyFlyBounce[i].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_EnemyFlyBounce[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_EnemyFlyBounce[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_EnemyFlyBounce[i].w;					// �G�l�~�[�̕\����
			float ph = g_EnemyFlyBounce[i].h;					// �G�l�~�[�̕\������

			float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_ANIM_MAX;				// �e�N�X�`���̍���
			float tx = tw * g_EnemyFlyBounce[i].currentSprite;	// �e�N�X�`���̍���X���W
			float ty = th * g_EnemyFlyBounce[i].currentAnim;	// �e�N�X�`���̍���Y���W

			if (g_EnemyFlyBounce[i].moveDir == FLY_BOUNCE_DIR_RIGHT)
			{
				tx += tw;
				tw *= -1.0f;
			}

			ty += 0.01f;



			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_EnemyFlyBounce[i].enemyColor);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

		
	}

}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY_FLY_BOUNCE* GetEnemyFlyBounce(void)
{
	return &g_EnemyFlyBounce[0];
}


void EnemyFlyBounceDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage)
{
	g_EnemyFlyBounce[enemyIndex].damaged = TRUE;
	g_EnemyFlyBounce[enemyIndex].damageOriginPos = playerPos;
	g_EnemyFlyBounce[enemyIndex].HP -= damage;
	PlaySound(SOUND_LABEL_SE_bloodyBlade);
	g_EnemyFlyBounce[enemyIndex].countAnim = ANIM_WAIT + 1;
	g_EnemyFlyBounce[enemyIndex].currentSprite = 0;

	if (g_EnemyFlyBounce[enemyIndex].HP <= 0)
	{
		g_EnemyFlyBounce[enemyIndex].alive = FALSE;
		RoundKill();
	}
}

void EnemyFlyBounceAnimationUpdate(void)
{
	for (int i = 0; i < ENEMY_FLY_BOUNCE_MAX; i++)
	{
		if (g_EnemyFlyBounce[i].spawned == FALSE) continue;

		if (g_EnemyFlyBounce[i].alive == FALSE)
		{
			g_EnemyFlyBounce[i].currentAnim = FLY_BOUNCE_ANIM_DIE;
		}
		else if (g_EnemyFlyBounce[i].damaged == TRUE)
		{
			g_EnemyFlyBounce[i].currentAnim = FLY_BOUNCE_ANIM_HIT;
		}
		else
		{
			g_EnemyFlyBounce[i].currentAnim = FLY_BOUNCE_ANIM_MOVE;
		}

		g_EnemyFlyBounce[i].countAnim ++;


		if (g_EnemyFlyBounce[i].countAnim > ANIM_WAIT)
		{
			g_EnemyFlyBounce[i].countAnim = 0;

			// �p�^�[���̐؂�ւ�
			if (g_EnemyFlyBounce[i].currentAnim == FLY_BOUNCE_ANIM_MOVE) //���[�v�A�j���[�V����
			{
				g_EnemyFlyBounce[i].currentSprite = (g_EnemyFlyBounce[i].currentSprite + 1) % TEXTURE_ANIM_SPRITES_MAX;
			}
			else //NO ���[�v�A�j���[�V����
			{
				if (g_EnemyFlyBounce[i].currentSprite < TEXTURE_ANIM_SPRITES_MAX - 1)
				{
					g_EnemyFlyBounce[i].currentSprite++;
				}
				
			}
			
		}
	}
}

void SpawnEnemyFlyBounce(XMFLOAT3 spawnPos)
{
	for (int i = 0; i < ENEMY_FLY_BOUNCE_MAX; i++)
	{
		if (g_EnemyFlyBounce[i].spawned == FALSE)
		{
			g_EnemyFlyBounce[i].spawned = TRUE;
			g_EnemyFlyBounce[i].alive = TRUE;
			g_EnemyFlyBounce[i].pos = spawnPos;

			int randMove = rand() % 4;	//�ŏ��̓������������_���ɂ���
			switch (randMove)
			{
			case 0:
				break;

			case 1:
				g_EnemyFlyBounce[i].moveSpeed.x *= -1;
				break;

			case 2:
				g_EnemyFlyBounce[i].moveSpeed.y *= -1;
				break;

			case 3:
				g_EnemyFlyBounce[i].moveSpeed.x *= -1;
				g_EnemyFlyBounce[i].moveSpeed.y *= -1;
				break;
			}
			break;
		}
	}
}



void DespawnEnemyFlyBounce(int enemyIndex)
{
	g_EnemyFlyBounce[enemyIndex].dmgTimeCnt++;

	if (g_EnemyFlyBounce[enemyIndex].dmgTimeCnt >= DESPAWN_TIME)
	{
		if (g_EnemyFlyBounce[enemyIndex].enemyColor.w > 0)
		{
			g_EnemyFlyBounce[enemyIndex].enemyColor.w -= 0.02f;
			return;
		}

		g_EnemyFlyBounce[enemyIndex].spawned = FALSE;
		g_EnemyFlyBounce[enemyIndex].alive = FALSE;
		g_EnemyFlyBounce[enemyIndex].pos = XMFLOAT3(SCREEN_CENTER_X, 1465.0f, 0.0f);	// ���S�_����\��

		g_EnemyFlyBounce[enemyIndex].currentSprite = 0;
		g_EnemyFlyBounce[enemyIndex].currentAnim = FLY_BOUNCE_ANIM_MOVE;

		g_EnemyFlyBounce[enemyIndex].moveSpeed = XMFLOAT3(3.0f, -3.0f, 1.0f);		// �ړ���
		g_EnemyFlyBounce[enemyIndex].moveDir = FLY_BOUNCE_DIR_RIGHT;

		g_EnemyFlyBounce[enemyIndex].HP = HP_MAX;

		g_EnemyFlyBounce[enemyIndex].damaged = FALSE;
		g_EnemyFlyBounce[enemyIndex].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyFlyBounce[enemyIndex].dmgTimeCnt = 0;

		g_EnemyFlyBounce[enemyIndex].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	}
}
