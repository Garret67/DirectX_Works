//=============================================================================
//
// �G�l�~�[���� [enemySlimeKing.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemySlimeKing.h"
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
#define TEXTURE_HEIGHT				(60.0f)	// 
#define TEXTURE_MAX					(4)		// �e�N�X�`���̐�

#define TEXTURE_ANIM_SPRITES_MAX	(5)		// 1�̃A�j���[�V�����̓X�v���C�g�����iX)
//#define TEXTURE_ANIM_MAX			(3)		// �A�j���[�V�����͂�����			  �iY)
//#define ANIM_WAIT					(20)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define DAMAGE_TIME					(30)
#define KNOCKBACK_TIME				(15)
#define DESPAWN_TIME				(120)
#define HP_MAX						(300.0f)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SlimeKingAnimationUpdate(void);

void DespawnSlimeKing(void);

void SetSlimeKingPartible(XMFLOAT3 pos);

void DrawSlimeKingUI(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/SlimeKing/SlimeKing_anim.png",
	"data/TEXTURE/Enemy/SlimeKing/SlimeKing_UI.png",
	"data/TEXTURE/Effect/Particle.png",
	"data/TEXTURE/Effect/fade_white.png",
};

static SLIME_KING			g_SlimeKing;		// �G�l�~�[�\����
static SLIME_KING_PARTICLE	g_Particle[SLIME_KING_PARTICLE_MAX];		// �G�l�~�[�\����

int animWait;
XMFLOAT4 KingSlimeHP_UI_Color;

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O


//=============================================================================
// ����������
//=============================================================================
HRESULT InitSlimeKing(void)
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

	BG* bg = GetBG();
	// �G�l�~�[�\���̂̏�����
	
	g_SlimeKing.spawned = FALSE;
	g_SlimeKing.alive = FALSE;
	g_SlimeKing.w = TEXTURE_WIDTH;
	g_SlimeKing.h = TEXTURE_HEIGHT;
	g_SlimeKing.pos = XMFLOAT3((bg->w / 2), bg->h - g_SlimeKing.h / 2 - 100.0f, 0.0f);
	g_SlimeKing.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_SlimeKing.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_SlimeKing.texNo = 0;

	g_SlimeKing.currentSprite = 0;
	g_SlimeKing.currentAnim = SLIME_KING_ANIM_WAKE_UP;

	g_SlimeKing.moveSpeed = 2.0f;		// �ړ���
	g_SlimeKing.moveDir = SLIME_KING_DIR_LEFT;

	g_SlimeKing.HP = HP_MAX;

	g_SlimeKing.damaged = FALSE;
	g_SlimeKing.dmgTimeCnt = 0;

	g_SlimeKing.enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	g_SlimeKing.attackType = SLIME_KING_ATTACK_HORIZONTAL;

	g_SlimeKing.phase = 1;
	g_SlimeKing.portal = 0;
	g_SlimeKing.state = SLIME_KING_STATE_IDLE;
	g_SlimeKing.stateTime = 180;


	for (int i = 0; i < SLIME_KING_PARTICLE_MAX; i++)
	{
		
		g_Particle[i].use = FALSE;
		g_Particle[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Particle[i].w = 50.0f;
		g_Particle[i].h = 50.0f;
		g_Particle[i].texNo = 2;

		g_Particle[i].moveSpeed = 10.0f;
		g_Particle[i].moveDir = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Particle[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Particle[i].lifeTime = 0;
		g_Particle[i].lifeTimeCnt = 0;
	}
	

	animWait = 20;

	KingSlimeHP_UI_Color = XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSlimeKing(void)
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
void UpdateSlimeKing(void)
{
	if (g_SlimeKing.spawned == FALSE) return;
	// �����Ă�G�l�~�[��������������
	if (g_SlimeKing.alive == TRUE)
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
					float slimeW;
					float slimeH;

					if (g_SlimeKing.state == SLIME_KING_STATE_RUSH ||
						g_SlimeKing.state == SLIME_KING_STATE_PREPARATION)
					{
						slimeW = g_SlimeKing.w * 0.5f;
						slimeH = g_SlimeKing.h * 0.7f;
					}
					else
					{
						slimeW = g_SlimeKing.w;
						slimeH = g_SlimeKing.h * 0.5f;
					}

					BOOL ans = CollisionBB(g_SlimeKing.pos, slimeW, slimeH,
						player[j].pos, player[j].w, player[j].h);
					// �������Ă���H
					if (ans == TRUE)
					{
						// �����������̏���
						PlayerDamaged(g_SlimeKing.pos);
						//Retroceso
					}
				}
			}
		}

		//SLIME KING�G�l�~�[�̃p�g���[��
		{


			switch (g_SlimeKing.state)
			{
			case SLIME_KING_STATE_IDLE:
				g_SlimeKing.stateTimeCnt++;
				if (g_SlimeKing.stateTimeCnt > 120)
				{
					g_SlimeKing.state = SLIME_KING_STATE_PREPARATION;
					g_SlimeKing.stateTimeCnt = 0;
					g_SlimeKing.countAnim = 0;
					g_SlimeKing.currentSprite = 0;
				}
				break;

			case SLIME_KING_STATE_PREPARATION:


				g_SlimeKing.moveDir = SLIME_KING_DIR_LEFT;
				g_SlimeKing.moveSpeed = 0.3f;

				g_SlimeKing.pos.x += g_SlimeKing.moveSpeed;
				g_SlimeKing.stateTimeCnt++;

				if (g_SlimeKing.stateTimeCnt > 120)
				{
					g_SlimeKing.state = SLIME_KING_STATE_RUSH;
					g_SlimeKing.stateTimeCnt = 0;
				}
				break;

			case SLIME_KING_STATE_RUSH:

				if (g_SlimeKing.attackType == SLIME_KING_ATTACK_HORIZONTAL)
				{
					g_SlimeKing.pos.x -= g_SlimeKing.moveSpeed;
				}
				else if (g_SlimeKing.attackType == SLIME_KING_ATTACK_VERTICAL)
				{
					g_SlimeKing.pos.y += g_SlimeKing.moveSpeed;
				}



				//���x����
				if (g_SlimeKing.portal > 3)
				{
					BG* bg = GetBG();
					if (g_SlimeKing.pos.x < bg->w / 2)
					{
						if ((g_SlimeKing.moveSpeed) > 0.0f)
						{
							g_SlimeKing.moveSpeed -= 0.1f * g_SlimeKing.stateTimeCnt;
							g_SlimeKing.stateTimeCnt++;
						}
						else
						{
							g_SlimeKing.moveSpeed = 0.0f;
							g_SlimeKing.state = SLIME_KING_STATE_TIRED;
							g_SlimeKing.stateTimeCnt = 0;
							g_SlimeKing.portal = 0;
							g_SlimeKing.countAnim = 0;
							g_SlimeKing.currentSprite = 0;
						}
					}

				}
				else
				{
					if (fabs(g_SlimeKing.moveSpeed) < 15.0f)
					{
						g_SlimeKing.moveSpeed += 0.1f * g_SlimeKing.stateTimeCnt;
						g_SlimeKing.stateTimeCnt++;
					}
					else
					{
						g_SlimeKing.stateTimeCnt = 0;
					}
				}



				break;

			case SLIME_KING_STATE_TIRED:
				g_SlimeKing.stateTimeCnt++;
				if (g_SlimeKing.stateTimeCnt > 120)
				{
					g_SlimeKing.state = SLIME_KING_STATE_IDLE;
					g_SlimeKing.stateTimeCnt = 0;
					g_SlimeKing.countAnim = 0;
					g_SlimeKing.currentSprite = 0;
				}
				break;
			}

		}



		//�G�l�~�[�����t����
		{
			if (g_SlimeKing.damaged == TRUE)
			{
				g_SlimeKing.dmgTimeCnt++;

				if (g_SlimeKing.dmgTimeCnt >= DAMAGE_TIME)
				{
					g_SlimeKing.damaged = FALSE;
					g_SlimeKing.dmgTimeCnt = 0;

					//g_SlimeKing.enemyColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
				}
			}


		}

		//�d��
		//g_SlimeKing.pos.y += 8.0f;




		//��ʒ[�̓����蔻��
		if (g_SlimeKing.state == SLIME_KING_STATE_RUSH)
		{// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			BG* bg = GetBG();
			PLAYER* player = GetPlayer();

			if (g_SlimeKing.pos.x < (-g_SlimeKing.w) ||				// ��
				g_SlimeKing.pos.y >(bg->h + g_SlimeKing.h))			// ��
			{

				switch (g_SlimeKing.phase)
				{
				case 1:
					g_SlimeKing.attackType = SLIME_KING_ATTACK_HORIZONTAL;
					g_SlimeKing.pos.x = bg->w + g_SlimeKing.w / 2;
					g_SlimeKing.pos.y = bg->h - g_SlimeKing.h / 2 - 100.0f;

					g_SlimeKing.portal++;
					break;

				case 2:
					if (g_SlimeKing.portal >= 3)
					{
						g_SlimeKing.attackType = SLIME_KING_ATTACK_HORIZONTAL;
						g_SlimeKing.pos.x = bg->w + g_SlimeKing.w / 2;
						g_SlimeKing.pos.y = bg->h - g_SlimeKing.h / 2 - 100.0f;
					}
					else
					{
						g_SlimeKing.attackType = SLIME_KING_ATTACK_VERTICAL;
						g_SlimeKing.pos.y = -500.0f;
						g_SlimeKing.pos.x = player[0].pos.x;
					}

					g_SlimeKing.portal++;
					break;

				case 3:
					if (g_SlimeKing.portal >= 3)
					{
						g_SlimeKing.pos.x = bg->w + g_SlimeKing.w / 2;
						g_SlimeKing.pos.y = bg->h - g_SlimeKing.h / 2 - 100.0f;
						g_SlimeKing.attackType = SLIME_KING_ATTACK_HORIZONTAL;
					}
					else
					{
						int nextAttack = rand() % 2;

						if (nextAttack == 0)
						{
							g_SlimeKing.pos.x = bg->w + g_SlimeKing.w / 2;
							g_SlimeKing.pos.y = bg->h - g_SlimeKing.h / 2 - 100.0f;
							g_SlimeKing.attackType = SLIME_KING_ATTACK_HORIZONTAL;
						}
						else if (nextAttack == 1)
						{
							g_SlimeKing.pos.y = -500.0f;
							g_SlimeKing.pos.x = player[0].pos.x;
							g_SlimeKing.attackType = SLIME_KING_ATTACK_VERTICAL;
						}
					}

					g_SlimeKing.portal++;
					break;
				}
			}


		}
	}
	else
	{
		DespawnSlimeKing();
	}
	

	// �A�j���[�V����
	SlimeKingAnimationUpdate();


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSlimeKing(void)
{
	if (g_SlimeKing.spawned == FALSE) return;

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

	//Particles
	if (g_SlimeKing.alive == FALSE)
	{
		for (int i = 0; i < SLIME_KING_PARTICLE_MAX; i++)
		{
			if (g_Particle[i].use == TRUE)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Particle[i].texNo]);

				//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Particle[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
				float py = g_Particle[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
				float pw = g_Particle[i].w;					// �G�l�~�[�̕\����
				float ph = g_Particle[i].h;					// �G�l�~�[�̕\������

				float tw = 1.0f;		// �e�N�X�`���̕�
				float th = 1.0f;		// �e�N�X�`���̍���
				float tx = 0.0f;		// �e�N�X�`���̍���X���W
				float ty = 0.0f;		// �e�N�X�`���̍���Y���W


				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					g_Particle[i].color);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
		}

	}



	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_SlimeKing.texNo]);

	//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_SlimeKing.pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
	float py = g_SlimeKing.pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
	float pw = g_SlimeKing.w;					// �G�l�~�[�̕\����
	float ph = g_SlimeKing.h;					// �G�l�~�[�̕\������

	float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// �e�N�X�`���̕�
	float th = 1.0f / SLIME_KING_ANIM_MAX;				// �e�N�X�`���̍���
	float tx = tw * g_SlimeKing.currentSprite;	// �e�N�X�`���̍���X���W
	float ty = th * g_SlimeKing.currentAnim;	// �e�N�X�`���̍���Y���W

	if (g_SlimeKing.moveDir == SLIME_KING_DIR_RIGHT)
	{
		tx += tw;
		tw *= -1.0f;
	}

	ty += 0.01f;
		


	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					g_SlimeKing.enemyColor);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
	

	DrawSlimeKingUI();

}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
SLIME_KING GetSlimeKing(void)
{
	return g_SlimeKing;
}


void DrawSlimeKingUI(void)
{
	BG* bg = GetBG();

	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = SCREEN_WIDTH- 40.0f;	// �G�l�~�[�̕\���ʒuX
		float py = 35.0f;		// �G�l�~�[�̕\���ʒuY
		float pw = 500.0f;				// �G�l�~�[�̕\����
		float ph = 50.0f;				// �G�l�~�[�̕\������

		float tw = 1.0f;				// �e�N�X�`���̕�
		float th = 1.0f;				// �e�N�X�`���̍���
		float tx = 0.0f;				// �e�N�X�`���̍���X���W
		float ty = 0.0f;				// �e�N�X�`���̍���Y���W


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteRightTop(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = SCREEN_WIDTH - 47.0f;	// �G�l�~�[�̕\���ʒuX
		float py = 42;		// �G�l�~�[�̕\���ʒuY
		float pw = 480.0f / HP_MAX * g_SlimeKing.HP;				// �G�l�~�[�̕\����
		float ph = 10.0f;				// �G�l�~�[�̕\������

		float tw = 1.0f;				// �e�N�X�`���̕�
		float th = 1.0f;				// �e�N�X�`���̍���
		float tx = 0.0f;				// �e�N�X�`���̍���X���W
		float ty = 0.0f;				// �e�N�X�`���̍���Y���W


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteRTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, KingSlimeHP_UI_Color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}


void SlimeKingDamaged(float damage)
{
	g_SlimeKing.damaged = TRUE;
	//g_SlimeKing.enemyColor = XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f);
	g_SlimeKing.HP -= damage;
	PlaySound(SOUND_LABEL_SE_bloodyBlade);
	g_SlimeKing.w += 10;
	g_SlimeKing.h += 6;
	g_SlimeKing.enemyColor.y -= 0.03f;
	g_SlimeKing.enemyColor.z -= 0.03f;

	float phaseHP = HP_MAX / 3;

	if (g_SlimeKing.HP <= 0)
	{
		g_SlimeKing.alive = FALSE;
		//g_SlimeKing.enemyColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		//RoundKill();
		//Death animation
	}
	else if (g_SlimeKing.HP <= phaseHP)
	{
		g_SlimeKing.phase = 3;
		KingSlimeHP_UI_Color = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	}
	else if (g_SlimeKing.HP <= phaseHP * 2)
	{
		g_SlimeKing.phase = 2;
		KingSlimeHP_UI_Color = XMFLOAT4(0.8f, 0.8f, 0.0f, 1.0f);
	}

}

void SlimeKingAnimationUpdate(void)
{
	if (g_SlimeKing.alive == TRUE)
	{
		if (g_SlimeKing.state == SLIME_KING_STATE_IDLE)
		{
			g_SlimeKing.currentAnim = SLIME_KING_ANIM_WAKE_UP;
		}
		else if (g_SlimeKing.state == SLIME_KING_STATE_PREPARATION)
		{
			g_SlimeKing.currentAnim = SLIME_KING_ANIM_PREPARATION;
		}
		else if (g_SlimeKing.state == SLIME_KING_STATE_TIRED)
		{
			g_SlimeKing.currentAnim = SLIME_KING_ANIM_TIRED;
		}
	}
	else
	{
		g_SlimeKing.currentAnim = SLIME_KING_ANIM_DEATH;
		animWait = 5;
	}
	

	g_SlimeKing.countAnim ++;


	if (g_SlimeKing.countAnim > animWait)
	{
		g_SlimeKing.countAnim = 0;

		// �p�^�[���̐؂�ւ�
		if (g_SlimeKing.alive) //NO ���[�v�A�j���[�V����
		{
			if (g_SlimeKing.currentSprite < TEXTURE_ANIM_SPRITES_MAX - 1)
			{
				g_SlimeKing.currentSprite++;
			}
		}
		else //���[�v�A�j���[�V����
		{
			g_SlimeKing.currentSprite = (g_SlimeKing.currentSprite + 1) % TEXTURE_ANIM_SPRITES_MAX;
		}
	}
	
}



void SpawnSlimeKing(XMFLOAT3 spawnPos)
{
	g_SlimeKing.spawned = TRUE;
	g_SlimeKing.alive = TRUE;
	g_SlimeKing.pos = spawnPos;
}



void DespawnSlimeKing(void)
{
	g_SlimeKing.dmgTimeCnt++;
	SetSlimeKingPartible(g_SlimeKing.pos);

	if (g_SlimeKing.dmgTimeCnt >= DESPAWN_TIME)
	{
		if (g_SlimeKing.w > 10)
		{
			g_SlimeKing.w -= 1.0f;
			g_SlimeKing.h -= 0.6f;
			return;
		}

		g_SlimeKing.spawned = FALSE;
		g_SlimeKing.alive = FALSE;
		g_SlimeKing.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_SlimeKing.w = TEXTURE_WIDTH;
		g_SlimeKing.h = TEXTURE_HEIGHT;

		g_SlimeKing.currentSprite = 0;
		g_SlimeKing.currentAnim = SLIME_KING_ANIM_WAKE_UP;

		g_SlimeKing.moveSpeed = 2.0f;		// �ړ���
		g_SlimeKing.moveDir = SLIME_KING_DIR_LEFT;

		g_SlimeKing.HP = HP_MAX;

		g_SlimeKing.damaged = FALSE;
		g_SlimeKing.dmgTimeCnt = 0;

		g_SlimeKing.enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_SlimeKing.attackType = SLIME_KING_ATTACK_HORIZONTAL;

		g_SlimeKing.phase = 1;
		g_SlimeKing.portal = 0;
		g_SlimeKing.state = SLIME_KING_STATE_IDLE;
		g_SlimeKing.stateTime = 180;

		RoundKill();
	}
}


void SetSlimeKingPartible(XMFLOAT3 pos)
{
	for (int i = 0; i < SLIME_KING_PARTICLE_MAX; i++)
	{
		if (g_Particle[i].use == FALSE)
		{
			g_Particle[i].use = TRUE;
			g_Particle[i].pos = pos;

			float angle = (float)(rand() % 628) / 100;
			g_Particle[i].moveDir.x = cosf(angle);	//angle�̕����ֈړ�
			g_Particle[i].moveDir.y = sinf(angle);	//angle�̕����ֈړ�

			g_Particle[i].lifeTime = SLIME_KING_PARTICLE_MAX;
			g_Particle[i].lifeTimeCnt = 0;

			g_Particle[i].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		}		
	}

	for (int i = 0; i < SLIME_KING_PARTICLE_MAX; i++)
	{
		if (g_Particle[i].use == TRUE)
		{
			g_Particle[i].lifeTimeCnt++;
			g_Particle[i].pos.x += g_Particle[i].moveDir.x * g_Particle[i].moveSpeed;
			g_Particle[i].pos.y += g_Particle[i].moveDir.y * g_Particle[i].moveSpeed;
			g_Particle[i].color.w -= 1.0f/ SLIME_KING_PARTICLE_MAX;
			g_Particle[i].color.z += 1.0f/ SLIME_KING_PARTICLE_MAX;

			if (g_Particle[i].lifeTimeCnt == g_Particle[i].lifeTime)
			{
				g_Particle[i].use = FALSE;
			}
		}

	}
}
