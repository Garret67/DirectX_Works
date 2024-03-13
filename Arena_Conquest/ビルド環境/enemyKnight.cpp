//=============================================================================
//
// �G�l�~�[���� [enemyKnight.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemyKnight.h"
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
#define TEXTURE_MAX					(1)			// �e�N�X�`���̐�

#define TEXTURE_ANIM_SPRITES_MAX	(4)			// 1�̃A�j���[�V�����̓X�v���C�g�����iX)
#define ANIM_WAIT					(10)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define STATE_HIT_TIME				(30)		// ���G��Ԏ��ԁi�_���[�W���󂯂���j
#define STATE_ATTACK_TIME			(100)		// �U���̎���
#define ATTACK_DELAY_TIME			(30)		// �U������܂ł̎��ԁi�\�����ԁj
#define KNOCKBACK_TIME				(15)		// �m�b�N�o�b�N����
#define DESPAWN_TIME				(180)		// ������܂ł̎���

#define HP_MAX						(50.0f)		// �ő��HP
#define MOVE_SPEED					(2.0f)		// �ړ����x

#define FAR_DISTANCE				(500.0f)	// ��������
#define NEAR_DISTANCE				(180.0f)	// �Z������


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void EnemyKnightAnimationUpdate(void);		//�A�j���[�V�����̍X�V����

void DespawnEnemyKnight(int enemyIndex);	//Knight�𖢎g�p�ɂ���

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/Skeleton/Skeleton_Anim.png",
};


static BOOL		g_Load = FALSE;							// ���������s�������̃t���O
static ENEMY_KNIGHT	g_EnemyKnight[ENEMY_KNIGHT_MAX];	// �G�l�~�[�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemyKnight(void)
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
	for (int i = 0; i < ENEMY_KNIGHT_MAX; i++)
	{
		g_EnemyKnight[i].spawned = FALSE;
		g_EnemyKnight[i].alive = FALSE;
		g_EnemyKnight[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// ���S�_����\��
		g_EnemyKnight[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyKnight[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_EnemyKnight[i].w = KNIGHT_TEXTURE_WIDTH;
		g_EnemyKnight[i].h = KNIGHT_TEXTURE_HEIGHT;
		g_EnemyKnight[i].texNo = 0;

		g_EnemyKnight[i].currentSprite = 0;
		g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_IDLE;

		g_EnemyKnight[i].moveSpeed = MOVE_SPEED;		// �ړ���
		g_EnemyKnight[i].moveDir = KNIGHT_DIR_RIGHT;

		g_EnemyKnight[i].HP = HP_MAX;

		g_EnemyKnight[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
		g_EnemyKnight[i].stateTime = 0;
		g_EnemyKnight[i].stateTimeCnt = 0;


		g_EnemyKnight[i].playerDistance = KNIGHT_DISTANCE_FAR;
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemyKnight(void)
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
void UpdateEnemyKnight(void)
{

	for (int i = 0; i < ENEMY_KNIGHT_MAX; i++)
	{
		if (g_EnemyKnight[i].spawned == FALSE) continue;

		// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
		XMFLOAT3 pos_old = g_EnemyKnight[i].pos;


		// �����Ă�G�l�~�[��������������
		if (g_EnemyKnight[i].alive == TRUE)
		{
			

			//�R�m�G�l�~�[�̃p�g���[��
			{
				
				PLAYER* player = GetPlayer();

				//�R�m�̌���
				{
					if (g_EnemyKnight[i].pos.x < player[0].pos.x)
					{
						g_EnemyKnight[i].moveDir = KNIGHT_DIR_RIGHT;
						g_EnemyKnight[i].moveSpeed = 1.0f;
					}
					else
					{
						g_EnemyKnight[i].moveDir = KNIGHT_DIR_LEFT;
						g_EnemyKnight[i].moveSpeed = -1.0f;
					}
				}



				int lastPlayerDistance = g_EnemyKnight[i].playerDistance;
				float playerDistance = (g_EnemyKnight[i].pos.x - player[0].pos.x);

				//�v���C���[�Ƃ̋���
				{
					if (fabsf(playerDistance) > FAR_DISTANCE)
					{
						g_EnemyKnight[i].playerDistance = KNIGHT_DISTANCE_FAR;
					}
					else if (fabsf(playerDistance) < FAR_DISTANCE &&
							 fabsf(playerDistance) > NEAR_DISTANCE)
					{
						g_EnemyKnight[i].playerDistance = KNIGHT_DISTANCE_MIDWAY;
					}
					else if (fabsf(playerDistance) < NEAR_DISTANCE)
					{
						float knightTop = g_EnemyKnight[i].pos.y - (g_EnemyKnight[i].h / 2);

						if (knightTop > player[0].pos.y)
						{
							g_EnemyKnight[i].playerDistance = KNIGHT_DISTANCE_NEAR_UP;
						}
						else
						{
							g_EnemyKnight[i].playerDistance = KNIGHT_DISTANCE_NEAR_FRONT;
						}
					
					}
				}


				//�v���C���[�Ƃ̋����ɂ���āA��Ԃ�ς���
				{
					if (g_EnemyKnight[i].state == KNIGHT_STATE_CONTROLLER || lastPlayerDistance != g_EnemyKnight[i].playerDistance)
					{
						if(g_EnemyKnight[i].state != KNIGHT_STATE_ATTACK)
						{
							switch (g_EnemyKnight[i].playerDistance)
							{
							case KNIGHT_DISTANCE_FAR:
								g_EnemyKnight[i].state = KNIGHT_STATE_MOVE_FORWARD;
								break;

							case KNIGHT_DISTANCE_MIDWAY:
							{
								int newState = rand() % 5;

								//�����_���œ��������߂�
								switch (newState)
								{
								case 0:
								case 1:
								case 2:
									g_EnemyKnight[i].state = KNIGHT_STATE_MOVE_FORWARD;		//�Ԃɐi��
									break;

								case 3:
									g_EnemyKnight[i].state = KNIGHT_STATE_MOVE_BACKWARD;	//���ɓ���
									break;

								case 4:
									g_EnemyKnight[i].state = KNIGHT_STATE_MOVE_STAY;		//�����Ȃ�
									break;
								}


								g_EnemyKnight[i].stateTime = 20 + rand() % 60;
							}
							break;

							case KNIGHT_DISTANCE_NEAR_UP:
							case KNIGHT_DISTANCE_NEAR_FRONT:

								int newState = rand() % 3;

								//�����_���œ��������߂�
								switch (newState)
								{
								case 0:
								case 1:
									//�h��
									if (g_EnemyKnight[i].playerDistance == KNIGHT_DISTANCE_NEAR_FRONT)
									{
										g_EnemyKnight[i].state = KNIGHT_STATE_DEFENCE_FRONT;
									}
									else
									{
										g_EnemyKnight[i].state = KNIGHT_STATE_DEFENCE_UP;
									}
								
									g_EnemyKnight[i].stateTime = 20 + rand() % 150;
									break;

								case 2:
									//�U��
									g_EnemyKnight[i].state = KNIGHT_STATE_ATTACK;

									g_EnemyKnight[i].stateTime = STATE_ATTACK_TIME;
									g_EnemyKnight[i].stateTimeCnt = 0;
								
									g_EnemyKnight[i].countAnim = 0;
									g_EnemyKnight[i].currentSprite = 0;
									break;
								}
							
							
							
								break;
							}
						}
					}
				}


				//��Ԃ̏���
				switch (g_EnemyKnight[i].state)
				{
					//�����Ȃ�
				case KNIGHT_STATE_MOVE_STAY:
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_IDLE;

					if (g_EnemyKnight[i].stateTimeCnt < g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}
					break;

					//�O�ɐi��
				case KNIGHT_STATE_MOVE_FORWARD:
					g_EnemyKnight[i].pos.x += g_EnemyKnight[i].moveSpeed;
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_MOVE;

					if (g_EnemyKnight[i].stateTimeCnt < g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}
					break;

					//���ɓ���
				case KNIGHT_STATE_MOVE_BACKWARD:
					g_EnemyKnight[i].pos.x -= g_EnemyKnight[i].moveSpeed;
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_MOVE;

					if (g_EnemyKnight[i].stateTimeCnt < g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}
					break;

					//�O�̖h��
				case KNIGHT_STATE_DEFENCE_FRONT:
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_DEFENCE_FRONT;

					if (g_EnemyKnight[i].stateTimeCnt < g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}
					break;

					//��̖h��
				case KNIGHT_STATE_DEFENCE_UP:
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_DEFENCE_UP;

					if (g_EnemyKnight[i].stateTimeCnt < g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}
					break;

					//�U��
				case KNIGHT_STATE_ATTACK:
				{
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_ATTACK;
					int attackStateCnt = g_EnemyKnight[i].stateTimeCnt - ATTACK_DELAY_TIME;

					if (attackStateCnt < g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}
				}
					break;

					//�_���[�W���󂯂�
				case KNIGHT_STATE_HIT:
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_HIT;

					if (g_EnemyKnight[i].stateTimeCnt < KNOCKBACK_TIME) //�͂˕Ԃ������͍ŏ���15�t���[������
					{
						float repealDisstance = 10.0f;
						if (g_EnemyKnight[i].pos.x < player[0].pos.x)
						{
							repealDisstance = -10.0f;
						}

						float repealDisstancePerFrame = repealDisstance / KNOCKBACK_TIME;
						float repealDistNow = repealDisstancePerFrame * (KNOCKBACK_TIME - g_EnemyKnight[i].stateTimeCnt);//SMOOTH
						g_EnemyKnight[i].pos.x += repealDistNow;
					}
					

					g_EnemyKnight[i].stateTimeCnt++;

					if (g_EnemyKnight[i].stateTimeCnt >= g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}

					break;
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
						float enemyCollW = KNIGHT_COLLIDER_WIDTH;
						if (g_EnemyKnight[i].state == KNIGHT_STATE_ATTACK && g_EnemyKnight[i].currentSprite == 2)
						{
							enemyCollW = g_EnemyKnight[i].w;
						}


						BOOL ans = CollisionBB(g_EnemyKnight[i].pos, enemyCollW, g_EnemyKnight[i].h,
							player[j].pos, player[j].w, player[j].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							PlayerDamaged(g_EnemyKnight[i].pos);
						}
					}
				}
			}
		}
		else
		{
			DespawnEnemyKnight(i);	//Despawn
		}
	
		//�d��
		g_EnemyKnight[i].pos.y += 5.0f;



		// Ground �Ƃ̓����蔻��
		{
			PLATFORM* ground = GetGround();

			// groundS�̐��������蔻����s��
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;


				BOOL ans = CheckCollisionAndGetDirection(ground[j].pos.x, ground[j].pos.y, (ground[j].w), (ground[j].h),
					g_EnemyKnight[i].pos.x, g_EnemyKnight[i].pos.y, (KNIGHT_COLLIDER_WIDTH), (g_EnemyKnight[i].h));
				// �������Ă���H

				switch (ans)
				{
				case FromTop:
					g_EnemyKnight[i].pos.y = ground[j].pos.y - (ground[j].h / 2) - (g_EnemyKnight[i].h / 2);
					break;

				case FromBottom:
					g_EnemyKnight[i].pos.y = ground[j].pos.y + (ground[j].h / 2) + (g_EnemyKnight[i].h / 2);
					break;

				case FromLeft:
					g_EnemyKnight[i].pos.x = ground[j].pos.x - (ground[j].w / 2) - (KNIGHT_COLLIDER_WIDTH / 2);
					break;

				case FromRight:
					g_EnemyKnight[i].pos.x = ground[j].pos.x + (ground[j].w / 2) + (KNIGHT_COLLIDER_WIDTH / 2);
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
					g_EnemyKnight[i].pos.x, g_EnemyKnight[i].pos.y, (KNIGHT_COLLIDER_WIDTH), (g_EnemyKnight[i].h));
				// �������Ă���H

				switch (ans)
				{
				case FromTop:
					g_EnemyKnight[i].pos.y = platform[j].pos.y - (platformH / 2) - (g_EnemyKnight[i].h / 2);
					break;

				case FromBottom:
					g_EnemyKnight[i].pos.y = platform[j].pos.y + (platformH / 2) + (g_EnemyKnight[i].h / 2);
					break;

				case FromLeft:
					g_EnemyKnight[i].pos.x = platform[j].pos.x - (platform[j].w / 2) - (KNIGHT_COLLIDER_WIDTH / 2);
					break;

				case FromRight:
					g_EnemyKnight[i].pos.x = platform[j].pos.x + (platform[j].w / 2) + (KNIGHT_COLLIDER_WIDTH / 2);
					break;

				default:

					break;
				}
			}
		}
	}


	// �A�j���[�V����
	EnemyKnightAnimationUpdate();


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemyKnight(void)
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

	for (int i = 0; i < ENEMY_KNIGHT_MAX; i++)
	{
		if (g_EnemyKnight[i].spawned == FALSE) continue;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnemyKnight[i].texNo]);

		//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_EnemyKnight[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
		float py = g_EnemyKnight[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
		float pw = g_EnemyKnight[i].w;					// �G�l�~�[�̕\����
		float ph = g_EnemyKnight[i].h;					// �G�l�~�[�̕\������

		float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// �e�N�X�`���̕�
		float th = 1.0f / KNIGHT_ANIM_MAX;				// �e�N�X�`���̍���
		float tx = tw * g_EnemyKnight[i].currentSprite;	// �e�N�X�`���̍���X���W
		float ty = th * g_EnemyKnight[i].currentAnim;	// �e�N�X�`���̍���Y���W

		if (g_EnemyKnight[i].moveDir == KNIGHT_DIR_LEFT)
		{
			tx += tw;
			tw *= -1.0f;
		}

		ty += 0.005f;
		


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_EnemyKnight[i].enemyColor);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY_KNIGHT* GetEnemyKnight(void)
{
	return &g_EnemyKnight[0];
}

//�_���[�W���󂯂�
void EnemyKnightDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage)
{
	g_EnemyKnight[enemyIndex].state = KNIGHT_STATE_HIT;
	g_EnemyKnight[enemyIndex].stateTime = STATE_HIT_TIME;
	g_EnemyKnight[enemyIndex].stateTimeCnt = 0;

	g_EnemyKnight[enemyIndex].HP -= damage;

	PlaySound(SOUND_LABEL_SE_bloodyBlade);

	g_EnemyKnight[enemyIndex].countAnim = 0;
	g_EnemyKnight[enemyIndex].currentSprite = 0;

	if (g_EnemyKnight[enemyIndex].HP <= 0)
	{
		g_EnemyKnight[enemyIndex].alive = FALSE;
		g_EnemyKnight[enemyIndex].currentAnim = KNIGHT_ANIM_DEATH;//Death animation
		RoundKill();
	}
}

//�A�j���[�V�����̍X�V����
void EnemyKnightAnimationUpdate(void)
{
	for (int i = 0; i < ENEMY_KNIGHT_MAX; i++)
	{
		if (g_EnemyKnight[i].spawned == FALSE) continue;

		if (g_EnemyKnight[i].countAnim > ANIM_WAIT)
		{
			g_EnemyKnight[i].countAnim = 0;

			// �p�^�[���̐؂�ւ�
			if (g_EnemyKnight[i].currentAnim == KNIGHT_ANIM_MOVE || 
				g_EnemyKnight[i].currentAnim == KNIGHT_ANIM_IDLE) //���[�v�A�j���[�V����
			{
				if (g_EnemyKnight[i].state == KNIGHT_STATE_MOVE_FORWARD ||
					g_EnemyKnight[i].state == KNIGHT_STATE_MOVE_STAY)
				{
					g_EnemyKnight[i].currentSprite = (g_EnemyKnight[i].currentSprite + 1) % TEXTURE_ANIM_SPRITES_MAX;
				}
				else if (g_EnemyKnight[i].state == KNIGHT_STATE_MOVE_BACKWARD)
				{
					g_EnemyKnight[i].currentSprite--;
					if (g_EnemyKnight[i].currentSprite == -1) 
					{
						g_EnemyKnight[i].currentSprite = TEXTURE_ANIM_SPRITES_MAX - 1;
					}
				}
				
			}
			else //NO ���[�v�A�j���[�V����
			{
				if (g_EnemyKnight[i].currentSprite < TEXTURE_ANIM_SPRITES_MAX - 1)
				{
					g_EnemyKnight[i].currentSprite++;

					if (g_EnemyKnight[i].currentAnim == KNIGHT_ANIM_ATTACK && g_EnemyKnight[i].currentSprite == 2)
					{
						PlaySound(SOUND_LABEL_SE_slash);
					}
				}
				
			}
			
		}

		if (g_EnemyKnight[i].currentAnim == KNIGHT_ANIM_ATTACK)
		{
			int attackStateCnt = g_EnemyKnight[i].stateTimeCnt - ATTACK_DELAY_TIME;

			if (attackStateCnt >= 0)
			{
				g_EnemyKnight[i].countAnim++;
			}
		}
		else 
		{
			g_EnemyKnight[i].countAnim++;
		}
		
		
	}
}

//Knight���o��������
void SpawnEnemyKnight(XMFLOAT3 spawnPos)
{
	for (int i = 0; i < ENEMY_KNIGHT_MAX; i++)
	{
		if (g_EnemyKnight[i].spawned == FALSE)
		{
			g_EnemyKnight[i].spawned = TRUE;
			g_EnemyKnight[i].alive = TRUE;
			g_EnemyKnight[i].pos = spawnPos;
			break;
		}
	}
}


//Knight�𖢎g�p�ɂ���
void DespawnEnemyKnight(int enemyIndex)
{
	g_EnemyKnight[enemyIndex].stateTimeCnt++;

	if (g_EnemyKnight[enemyIndex].stateTimeCnt >= DESPAWN_TIME)
	{
		if (g_EnemyKnight[enemyIndex].enemyColor.w > 0)
		{
			g_EnemyKnight[enemyIndex].enemyColor.w -= 0.02f;
			return;
		}

		
		g_EnemyKnight[enemyIndex].spawned = FALSE;
		g_EnemyKnight[enemyIndex].alive = FALSE;
		g_EnemyKnight[enemyIndex].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// ���S�_����\��
		g_EnemyKnight[enemyIndex].w = KNIGHT_TEXTURE_WIDTH;
		g_EnemyKnight[enemyIndex].h = KNIGHT_TEXTURE_HEIGHT;

		g_EnemyKnight[enemyIndex].currentSprite = 0;
		g_EnemyKnight[enemyIndex].currentAnim = KNIGHT_ANIM_IDLE;

		g_EnemyKnight[enemyIndex].moveSpeed = 2.0f;		// �ړ���
		g_EnemyKnight[enemyIndex].moveDir = KNIGHT_DIR_RIGHT;

		g_EnemyKnight[enemyIndex].HP = HP_MAX;

		g_EnemyKnight[enemyIndex].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_EnemyKnight[enemyIndex].state = KNIGHT_STATE_CONTROLLER;
		g_EnemyKnight[enemyIndex].stateTime = 0;
		g_EnemyKnight[enemyIndex].stateTimeCnt = 0;


		g_EnemyKnight[enemyIndex].playerDistance = KNIGHT_DISTANCE_FAR;

	}
}

