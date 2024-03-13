//=============================================================================
//
// �G�l�~�[���� [enemyFlyBulletOne.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemyFlyBulletOne.h"
#include "spawnController.h"
#include "bg.h"
#include "bullet.h"
#include "platforms.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define FLY_ONE_TEXTURE_WIDTH		(30 * 4)	// �L�����T�C�Y
#define FLY_ONE_TEXTURE_HEIGHT		(16 * 4)	// 

#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_ANIM_SPRITES_MAX	(8)		// 1�̃A�j���[�V�����̓X�v���C�g�����iX)
#define TEXTURE_ANIM_MAX			(4)		// �A�j���[�V�����͂�����			  �iY)
#define ANIM_WAIT					(5)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define STATE_HIT_TIME				(30)
#define NEXT_ATTACK_TIME			(180)
#define ATTACK_DELAY_TIME			(30)
#define KNOCKBACK_TIME				(15)
#define DESPAWN_TIME				(180)
#define HP_MAX						(50.0f)
#define FLY_ONE_BULLET_SPEED		(5.0f)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void EnemyFlyOneAnimationUpdate(void);

void DespawnEnemyFlyOne(int enemyIndex);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/Flying eye/FlyingEye_Anim.png",
};


static BOOL		g_Load = FALSE;							// ���������s�������̃t���O

static ENEMY_FLY_ONE	g_EnemyFlyOne[ENEMY_FLY_ONE_MAX];	// �G�l�~�[�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemyFlyOne(void)
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
	for (int i = 0; i < ENEMY_FLY_ONE_MAX; i++)
	{
		g_EnemyFlyOne[i].spawned = FALSE;
		g_EnemyFlyOne[i].alive = FALSE;
		g_EnemyFlyOne[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// ���S�_����\��
		g_EnemyFlyOne[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyFlyOne[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_EnemyFlyOne[i].w = FLY_ONE_TEXTURE_WIDTH;
		g_EnemyFlyOne[i].h = FLY_ONE_TEXTURE_HEIGHT;
		g_EnemyFlyOne[i].texNo = 0;

		g_EnemyFlyOne[i].currentSprite = 0;
		g_EnemyFlyOne[i].currentSpriteMax = TEXTURE_ANIM_SPRITES_MAX;
		g_EnemyFlyOne[i].currentAnim = FLY_ONE_ANIM_MOVE;

		g_EnemyFlyOne[i].moveSpeed = 3.0f;		// �ړ���
		g_EnemyFlyOne[i].moveDir = FLY_ONE_DIR_RIGHT;

		g_EnemyFlyOne[i].WayPointPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_EnemyFlyOne[i].HP = HP_MAX;

		g_EnemyFlyOne[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_EnemyFlyOne[i].state = FLY_ONE_STATE_CONTROLLER;
		g_EnemyFlyOne[i].stateTime = 0;
		g_EnemyFlyOne[i].stateTimeCnt = 0;

		g_EnemyFlyOne[i].attackTimeCnt = 0;

		g_EnemyFlyOne[i].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemyFlyOne(void)
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
void UpdateEnemyFlyOne(void)
{

	for (int i = 0; i < ENEMY_FLY_ONE_MAX; i++)
	{
		if (g_EnemyFlyOne[i].spawned == FALSE) continue;

		// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
		XMFLOAT3 pos_old = g_EnemyFlyOne[i].pos;


		// �����Ă�G�l�~�[��������������
		if (g_EnemyFlyOne[i].alive == TRUE)
		{
			PLAYER* player = GetPlayer();

			//FLY_ONE�G�l�~�[�̃p�g���[��
			{
				float playerDistance = (g_EnemyFlyOne[i].pos.x - player[0].pos.x);

				//�v���C���[�Ƃ̋���
				//{
				//	if (fabsf(playerDistance) < 300.0f)
				//	{
				//		//MOVE
				//		g_EnemyFlyOne[i].state = FLY_ONE_STATE_MOVE;
				//	}
				//}

				if (g_EnemyFlyOne[i].state != FLY_ONE_STATE_ATTACK)
				{
					g_EnemyFlyOne[i].attackTimeCnt++;

					if (g_EnemyFlyOne[i].attackTimeCnt >= NEXT_ATTACK_TIME)
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_ATTACK;
						g_EnemyFlyOne[i].stateTime = 40;
						g_EnemyFlyOne[i].stateTimeCnt = 0;
						g_EnemyFlyOne[i].currentSprite = 0;
						g_EnemyFlyOne[i].currentSpriteMax = 2;

						g_EnemyFlyOne[i].attackTimeCnt = 0;
					}
				}
				


				//STATE CONTROLLER
				if (g_EnemyFlyOne[i].state == FLY_ONE_STATE_CONTROLLER)
				{
					//Decide next movePoint
					int newState = rand() % 3;

					if (newState == 2)
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_MOVE;
						g_EnemyFlyOne[i].stateTime = 300;

						g_EnemyFlyOne[i].currentSpriteMax = 8;

						BOOL IncorrectWayPoint = TRUE;

						while (IncorrectWayPoint)
						{
							BG* bg = GetBG();

							float PosXMax = bg->w - g_EnemyFlyOne[i].w;
							float PosYMax = bg->h - 300;

							g_EnemyFlyOne[i].WayPointPos.x = (rand() % (int)PosXMax) + (g_EnemyFlyOne[i].w / 2);
							g_EnemyFlyOne[i].WayPointPos.y = (rand() % (int)PosYMax) + (g_EnemyFlyOne[i].h / 2);


							//platform�̓����蔻��
							PLATFORM* platform = GetGround();
							for (int j = 0; j < GROUND_MAX; j++)
							{
								BOOL ans = CollisionBB(g_EnemyFlyOne[i].WayPointPos, g_EnemyFlyOne[i].w, g_EnemyFlyOne[i].h,
									platform[j].pos, platform[j].w, platform[j].h);

								// �������Ă���H
								if (ans == FALSE)
								{
									// �����������̏���
									IncorrectWayPoint = FALSE;
								}
								
							}
						}
						
					}
					else
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_STAY;
						g_EnemyFlyOne[i].stateTime = 20 + rand() % 60;
						g_EnemyFlyOne[i].currentSpriteMax = 8;
					}
				}


				//��Ԃ̏���
				switch (g_EnemyFlyOne[i].state)
				{
				case FLY_ONE_STATE_STAY:
					g_EnemyFlyOne[i].currentAnim = FLY_ONE_ANIM_MOVE;

					if (g_EnemyFlyOne[i].stateTimeCnt < g_EnemyFlyOne[i].stateTime)
					{
						g_EnemyFlyOne[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_CONTROLLER;
						g_EnemyFlyOne[i].stateTimeCnt = 0;
					}
					break;

				case FLY_ONE_STATE_MOVE:
				{
					g_EnemyFlyOne[i].currentAnim = FLY_ONE_ANIM_MOVE;



					XMVECTOR epos = XMLoadFloat3(&g_EnemyFlyOne[i].pos);
					XMVECTOR vec =  XMLoadFloat3(&g_EnemyFlyOne[i].WayPointPos) - epos;	//�����ƃv���C���[�Ƃ̍��������߂�

					float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);		//���̍������g���Ċp�x�����߂Ă���

					g_EnemyFlyOne[i].pos.x += cosf(angle) * g_EnemyFlyOne[i].moveSpeed;				//angle�̕����ֈړ�
					g_EnemyFlyOne[i].pos.y += sinf(angle) * g_EnemyFlyOne[i].moveSpeed;				//angle�̕����ֈړ�


					XMVECTOR newVec = XMLoadFloat3(&g_EnemyFlyOne[i].WayPointPos) - XMLoadFloat3(&g_EnemyFlyOne[i].pos);

					float vecDist = 0.0f;
					XMStoreFloat(&vecDist, XMVector3LengthSq(newVec));

					if (vecDist < 50.0f)
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_CONTROLLER;
						g_EnemyFlyOne[i].stateTimeCnt = 0;
					}
					else if (g_EnemyFlyOne[i].stateTimeCnt < g_EnemyFlyOne[i].stateTime)
					{
						g_EnemyFlyOne[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_CONTROLLER;
						g_EnemyFlyOne[i].stateTimeCnt = 0;
					}

				}
					break;

				case FLY_ONE_STATE_ATTACK: 
				{
					g_EnemyFlyOne[i].currentAnim = FLY_ONE_ANIM_ATTACK;
					int attackStateCnt = g_EnemyFlyOne[i].stateTimeCnt - ATTACK_DELAY_TIME;

					if (attackStateCnt == 0)
					{
						XMVECTOR epos = XMLoadFloat3(&g_EnemyFlyOne[i].pos);
						XMVECTOR vec = XMLoadFloat3(&player->pos) - epos;		//�����ƃv���C���[�Ƃ̍��������߂�
						float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);						//���̍������g���Ċp�x�����߂Ă���

						XMFLOAT3 moveDir = XMFLOAT3(0.0f, 0.0f, 0.0f);
						moveDir.x = cosf(angle) * FLY_ONE_BULLET_SPEED;					//angle�̕����ֈړ�
						moveDir.y = sinf(angle) * FLY_ONE_BULLET_SPEED;					//angle�̕����ֈړ�

						SetBulletEnemy(g_EnemyFlyOne[i].pos, angle - (XM_PIDIV2), moveDir);
					}

					if (attackStateCnt < g_EnemyFlyOne[i].stateTime)
					{
						g_EnemyFlyOne[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_CONTROLLER;
						g_EnemyFlyOne[i].stateTimeCnt = 0;
					}
				}
					break;

				case FLY_ONE_STATE_HIT:
					g_EnemyFlyOne[i].currentAnim = FLY_ONE_ANIM_HIT;

					if (g_EnemyFlyOne[i].stateTimeCnt < KNOCKBACK_TIME) //�͂˕Ԃ������͍ŏ���15�t���[������
					{
						XMVECTOR epos = XMLoadFloat3(&g_EnemyFlyOne[i].pos);
						XMVECTOR vec = (epos - XMLoadFloat3(&g_EnemyFlyOne[i].damageOriginPos));			//�����ƃv���C���[�Ƃ̍��������߂�
						float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);		//���̍������g���Ċp�x�����߂Ă���
						float repealDisstance = 10.0f;										//�X�s�[�h�͂�����ƒx�����Ă݂�
						float repealDisstancePerFrame = repealDisstance / KNOCKBACK_TIME;

						//SMOOTH
						float repealDistNow = repealDisstancePerFrame * (KNOCKBACK_TIME - g_EnemyFlyOne[i].stateTimeCnt);

						g_EnemyFlyOne[i].pos.x += cosf(angle) * repealDistNow;					//angle�̕����ֈړ�
						g_EnemyFlyOne[i].pos.y += sinf(angle) * repealDistNow;					//angle�̕����ֈړ�
					}
					

					g_EnemyFlyOne[i].stateTimeCnt++;

					if (g_EnemyFlyOne[i].stateTimeCnt >= g_EnemyFlyOne[i].stateTime)
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_CONTROLLER;
						g_EnemyFlyOne[i].stateTimeCnt = 0;

						//g_EnemyFlyOne[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
					}

					break;
				}

			}

			

			//FLY_ONE�̃v���C���[����
			{
				if (g_EnemyFlyOne[i].pos.x < player[0].pos.x)
				{
					g_EnemyFlyOne[i].moveDir = FLY_ONE_DIR_RIGHT;
				}
				else
				{
					g_EnemyFlyOne[i].moveDir = FLY_ONE_DIR_LEFT;
				}
			}
			//FLY_ONE�̓����̌���
			if (g_EnemyFlyOne[i].pos.x < pos_old.x)
			{
				g_EnemyFlyOne[i].moveDir = FLY_ONE_DIR_LEFT;
			}
			else if (g_EnemyFlyOne[i].pos.x > pos_old.x)
			{
				g_EnemyFlyOne[i].moveDir = FLY_ONE_DIR_RIGHT;
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
						BOOL ans = CollisionBB(g_EnemyFlyOne[i].pos, g_EnemyFlyOne[i].w , g_EnemyFlyOne[i].h,
							player[j].pos, player[j].w, player[j].h);

						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							PlayerDamaged(g_EnemyFlyOne[i].pos);
						}
					}
				}
			}
		}
		else
		{
			//�d��
			g_EnemyFlyOne[i].pos.y += 5.0f;

			DespawnEnemyFlyOne(i);	//Despawn
		}



		// Ground �Ƃ̓����蔻��
		{
			PLATFORM* ground = GetGround();

			// groundS�̐��������蔻����s��
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;


				BOOL ans = CheckCollisionAndGetDirection(ground[j].pos.x, ground[j].pos.y, (ground[j].w), (ground[j].h),
					g_EnemyFlyOne[i].pos.x, g_EnemyFlyOne[i].pos.y, (g_EnemyFlyOne[i].w), (g_EnemyFlyOne[i].h));
				// �������Ă���H

				switch (ans)
				{
				case FromTop:
					g_EnemyFlyOne[i].pos.y = ground[j].pos.y - (ground[j].h / 2) - (g_EnemyFlyOne[i].h / 2);
					break;

				case FromBottom:
					g_EnemyFlyOne[i].pos.y = ground[j].pos.y + (ground[j].h / 2) + (g_EnemyFlyOne[i].h / 2);
					break;

				case FromLeft:
					g_EnemyFlyOne[i].pos.x = ground[j].pos.x - (ground[j].w / 2) - (g_EnemyFlyOne[i].w / 2);
					break;

				case FromRight:
					g_EnemyFlyOne[i].pos.x = ground[j].pos.x + (ground[j].w / 2) + (g_EnemyFlyOne[i].w / 2);
					break;

				default:

					break;
				}


			}
		}


		// PLATFORMS�Ƃ̓����蔻��
		{
			PLATFORM_ANIM* platform = GetPlatforms();

			for (int j = 0; j < PLATFORM_MAX; j++)
			{
				if (platform[j].collider == FALSE) continue;

				float platformH = 50.0f;

				BOOL ans = CheckCollisionAndGetDirection(platform[j].pos.x, platform[j].pos.y, (platform[j].w), (platformH),
					g_EnemyFlyOne[i].pos.x, g_EnemyFlyOne[i].pos.y, (g_EnemyFlyOne[i].w), (g_EnemyFlyOne[i].h));
				// �������Ă���H

				switch (ans)
				{
				case FromTop:
					g_EnemyFlyOne[i].pos.y = platform[j].pos.y - (platformH / 2) - (g_EnemyFlyOne[i].h / 2);
					break;

				case FromBottom:
					g_EnemyFlyOne[i].pos.y = platform[j].pos.y + (platformH / 2) + (g_EnemyFlyOne[i].h / 2);
					break;

				case FromLeft:
					g_EnemyFlyOne[i].pos.x = platform[j].pos.x - (platform[j].w / 2) - (g_EnemyFlyOne[i].w / 2);
					break;

				case FromRight:
					g_EnemyFlyOne[i].pos.x = platform[j].pos.x + (platform[j].w / 2) + (g_EnemyFlyOne[i].w / 2);
					break;

				default:

					break;
				}


			}
		}


		//��ʒ[�̓����蔻��
		{
			BG* bg = GetBG();
			if (g_EnemyFlyOne[i].pos.x < (g_EnemyFlyOne[i].w / 2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_EnemyFlyOne[i].pos.x = g_EnemyFlyOne[i].w / 2;
			}
			if (g_EnemyFlyOne[i].pos.x > (bg->w - g_EnemyFlyOne[i].w / 2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_EnemyFlyOne[i].pos.x = bg->w - (g_EnemyFlyOne[i].w / 2);
			}

			if (g_EnemyFlyOne[i].pos.y < (g_EnemyFlyOne[i].h / 2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_EnemyFlyOne[i].pos.y = (g_EnemyFlyOne[i].h / 2);
			}
			if (g_EnemyFlyOne[i].pos.y > (bg->h - g_EnemyFlyOne[i].h / 2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_EnemyFlyOne[i].pos.y = bg->h - (g_EnemyFlyOne[i].h / 2);
			}
		}
		

	}


	// �A�j���[�V����
	EnemyFlyOneAnimationUpdate();


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemyFlyOne(void)
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

	for (int i = 0; i < ENEMY_FLY_ONE_MAX; i++)
	{
		if (g_EnemyFlyOne[i].spawned == FALSE) continue;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnemyFlyOne[i].texNo]);

		//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_EnemyFlyOne[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
		float py = g_EnemyFlyOne[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
		float pw = g_EnemyFlyOne[i].w;					// �G�l�~�[�̕\����
		float ph = g_EnemyFlyOne[i].h;					// �G�l�~�[�̕\������

		float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// �e�N�X�`���̕�
		float th = 1.0f / FLY_ONE_ANIM_MAX;				// �e�N�X�`���̍���
		float tx = tw * g_EnemyFlyOne[i].currentSprite;	// �e�N�X�`���̍���X���W
		float ty = th * g_EnemyFlyOne[i].currentAnim;	// �e�N�X�`���̍���Y���W

		if (g_EnemyFlyOne[i].moveDir == FLY_ONE_DIR_LEFT)
		{
			tx += tw;
			tw *= -1.0f;
		}

		ty += 0.005f;
		


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_EnemyFlyOne[i].enemyColor);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY_FLY_ONE* GetEnemyFlyOne(void)
{
	return &g_EnemyFlyOne[0];
}


void EnemyFlyOneDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage)
{
	g_EnemyFlyOne[enemyIndex].state = FLY_ONE_STATE_HIT;
	g_EnemyFlyOne[enemyIndex].stateTime = STATE_HIT_TIME;
	g_EnemyFlyOne[enemyIndex].stateTimeCnt = 0;
	g_EnemyFlyOne[enemyIndex].damageOriginPos = playerPos;

	g_EnemyFlyOne[enemyIndex].HP -= damage;

	g_EnemyFlyOne[enemyIndex].countAnim = 0;
	g_EnemyFlyOne[enemyIndex].currentSprite = 0;
	g_EnemyFlyOne[enemyIndex].currentSpriteMax = 4;

	PlaySound(SOUND_LABEL_SE_bloodyBlade);

	if (g_EnemyFlyOne[enemyIndex].HP <= 0)
	{
		g_EnemyFlyOne[enemyIndex].alive = FALSE;
		RoundKill();
		//Death animation
		g_EnemyFlyOne[enemyIndex].currentAnim = FLY_ONE_ANIM_DEATH;
	}
}

void EnemyFlyOneAnimationUpdate(void)
{
	for (int i = 0; i < ENEMY_FLY_ONE_MAX; i++)
	{
		if (g_EnemyFlyOne[i].spawned == FALSE) continue;

		if (g_EnemyFlyOne[i].currentAnim == FLY_ONE_ANIM_ATTACK)
		{
			int attackStateCnt = g_EnemyFlyOne[i].stateTimeCnt - ATTACK_DELAY_TIME;

			if (attackStateCnt >= 0)
			{
				g_EnemyFlyOne[i].countAnim++;
			}
		}
		else
		{
			g_EnemyFlyOne[i].countAnim++;
		}


		if (g_EnemyFlyOne[i].countAnim >= ANIM_WAIT)
		{
			g_EnemyFlyOne[i].countAnim = 0;

			// �p�^�[���̐؂�ւ�
			if (g_EnemyFlyOne[i].currentAnim == FLY_ONE_ANIM_MOVE) //���[�v�A�j���[�V����
			{
				g_EnemyFlyOne[i].currentSprite = (g_EnemyFlyOne[i].currentSprite + 1) % g_EnemyFlyOne[i].currentSpriteMax;
			}
			else //NO ���[�v�A�j���[�V����
			{
				if (g_EnemyFlyOne[i].currentSprite < g_EnemyFlyOne[i].currentSpriteMax - 1)
				{
					g_EnemyFlyOne[i].currentSprite++;
				}
				
			}
			
		}

		
		
		
	}
}


void SpawnEnemyFlyOne(XMFLOAT3 spawnPos)
{
	for (int i = 0; i < ENEMY_FLY_ONE_MAX; i++)
	{
		if (g_EnemyFlyOne[i].spawned == FALSE)
		{
			g_EnemyFlyOne[i].spawned = TRUE;
			g_EnemyFlyOne[i].alive = TRUE;
			g_EnemyFlyOne[i].pos = spawnPos;
			break;
		}
	}
}



void DespawnEnemyFlyOne(int enemyIndex)
{
	g_EnemyFlyOne[enemyIndex].stateTimeCnt++;

	if (g_EnemyFlyOne[enemyIndex].stateTimeCnt >= DESPAWN_TIME)
	{
		if (g_EnemyFlyOne[enemyIndex].enemyColor.w > 0)
		{
			g_EnemyFlyOne[enemyIndex].enemyColor.w -= 0.02f;
			return;
		}

		
		g_EnemyFlyOne[enemyIndex].spawned = FALSE;
		g_EnemyFlyOne[enemyIndex].alive = FALSE;
		g_EnemyFlyOne[enemyIndex].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);		// ���S�_����\��
		g_EnemyFlyOne[enemyIndex].w = FLY_ONE_TEXTURE_WIDTH;
		g_EnemyFlyOne[enemyIndex].h = FLY_ONE_TEXTURE_HEIGHT;

		g_EnemyFlyOne[enemyIndex].currentSprite = 0;
		g_EnemyFlyOne[enemyIndex].currentAnim = FLY_ONE_ANIM_MOVE;

		g_EnemyFlyOne[enemyIndex].moveSpeed = 2.0f;		// �ړ���
		g_EnemyFlyOne[enemyIndex].moveDir = FLY_ONE_DIR_RIGHT;

		g_EnemyFlyOne[enemyIndex].HP = HP_MAX;

		g_EnemyFlyOne[enemyIndex].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_EnemyFlyOne[enemyIndex].state = FLY_ONE_STATE_CONTROLLER;
		g_EnemyFlyOne[enemyIndex].stateTime = 0;
		g_EnemyFlyOne[enemyIndex].stateTimeCnt = 0;

		g_EnemyFlyOne[enemyIndex].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}
}
