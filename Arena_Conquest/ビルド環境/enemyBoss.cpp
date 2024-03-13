//=============================================================================
//
// �G�l�~�[���� [enemyBoss.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemyBoss.h"
#include "spawnController.h"
#include "bg.h"
#include "platforms.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "sound.h"
#include "platforms.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100.0f)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(120.0f)	// �L�����T�C�Y

#define PARTICLE_SIZE				(50.0f)		// �p�[�e�B�N���̑傫��

#define TEXTURE_ANIM_SPRITES_MAX	(5)			// 1�̃A�j���[�V�����̓X�v���C�g�����iX)
#define ANIM_WAIT					(4)			// �A�j���[�V�����̐؂�ւ��Wait�l

#define DAMAGE_TIME					(30)		// �_���[�W��Ԃ̎���
#define DESPAWN_TIME				(180)		// �E����邩�������܂ł̎���
#define HP_MAX						(300.0f)	// �̗�

#define BOSS_PROJECTILE_MAX			(7)			// �d�C�ʂ�Max��
#define BOSS_PARTICLE_MAX			(40)		// �p�[�e�B�N����Max��

#define THIN_LASER					(5.0f)		// ���[�U�[���ׂ����̕�
#define THICK_LASER					(100.0f)	// ���[�U�[���������̕�

#define BOSS_PROJECTILE_SPEED		(10.0f)		// �d�C�ʂ̑��x

//PHASE 1
#define PHASE_1_ATTACK_DURATION_CHASE		(120)	//�U������
#define PHASE_1_ATTACK_DURATION_SHOTS		(40)	//�U������
#define PHASE_1_ATTACK_DURATION_LASER		(100)	//�U������
#define PHASE_1_NUMBER_OF_ATTACKS_CHASE		(3)		//�U����
#define PHASE_1_NUMBER_OF_ATTACKS_SHOTS		(3)		//�U����
#define PHASE_1_NUMBER_OF_ATTACKS_LASER		(3)		//�U����

//PHASE 2
#define PHASE_2_ATTACK_DURATION_CHASE		(90)	//�U������
#define PHASE_2_ATTACK_DURATION_SHOTS		(30)	//�U������
#define PHASE_2_ATTACK_DURATION_LASER		(80)	//�U������
#define PHASE_2_NUMBER_OF_ATTACKS_CHASE		(5)		//�U����
#define PHASE_2_NUMBER_OF_ATTACKS_SHOTS		(5)		//�U����
#define PHASE_2_NUMBER_OF_ATTACKS_LASER		(5)		//�U����

//PHASE 3
#define PHASE_3_ATTACK_DURATION_CHASE		(60)	//�U������
#define PHASE_3_ATTACK_DURATION_SHOTS		(20)	//�U������
#define PHASE_3_ATTACK_DURATION_LASER		(60)	//�U������
#define PHASE_3_NUMBER_OF_ATTACKS_CHASE		(7)		//�U����
#define PHASE_3_NUMBER_OF_ATTACKS_SHOTS		(7)		//�U����
#define PHASE_3_NUMBER_OF_ATTACKS_LASER		(7)		//�U����


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DespawnBoss(void);

void SetBossPartible(XMFLOAT3 pos);

double easeInOutQuad(double t);

void SetBossProjectile(void);
void UpdateBossProjectile(void);
void DrawBossProjectiles(void);

void DrawBossLaser(void);
void DrawBossUI(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[BOSS_TEX_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[BOSS_TEX_MAX] = 
{
	"data/TEXTURE/Enemy/Boss/BossAnim.png",
	"data/TEXTURE/Effect/Particle.png",
	"data/TEXTURE/Effect/LaserSign2.png",
	"data/TEXTURE/Effect/Laser.png",
	"data/TEXTURE/Effect/lightningBall.png",
	"data/TEXTURE/Enemy/Boss/BossUI.png",
	"data/TEXTURE/Effect/fade_white.png",
};

static BOSS				g_Boss;								// �{�X			�\����
static BOSS_LASER		g_Laser;							// ���[�U�[		�\����
static BOSS_PROJECTILE	g_Projectile[BOSS_PROJECTILE_MAX];	// �d�C��		�\����
static BOSS_PARTICLE	g_Particle[BOSS_PARTICLE_MAX];		// �p�[�e�B�N��	�\����


static BOOL		g_Load = FALSE;	// ���������s�������̃t���O


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBoss(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < BOSS_TEX_MAX; i++)
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
	{
		g_Boss.spawned	= FALSE;
		g_Boss.alive	= FALSE;
		g_Boss.shoot	= FALSE;
		g_Boss.w		= TEXTURE_WIDTH;
		g_Boss.h		= TEXTURE_HEIGHT;
		g_Boss.pos		= XMFLOAT3((bg->w / 2), bg->h - g_Boss.h / 2 - 100.0f, 0.0f);
		g_Boss.rot		= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss.scl		= XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Boss.texNo	= BOSS_TEX_BOSS_SPRITE_SHEET;

		g_Boss.currentSprite = 0;
		g_Boss.countAnim	 = 0;
		g_Boss.currentAnim   = BOSS_ANIM_CHASE;

		g_Boss.moveStartPos  = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss.moveEndPos	 = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Boss.HP			 = HP_MAX;

		g_Boss.damaged		 = FALSE;
		g_Boss.dmgTimeCnt	 = 0;

		g_Boss.enemyColor	 = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Boss.phase		 = 1;
		g_Boss.attacks		 = 0;
		g_Boss.attackCnt	 = 0;
		g_Boss.attackTime	 = PHASE_1_ATTACK_DURATION_LASER;
		g_Boss.attackTimeCnt = 0.0f;

		g_Boss.projectileNum = PHASE_1_NUMBER_OF_ATTACKS_SHOTS;

		g_Boss.state		 = BOSS_STATE_CHASE;

		g_Boss.attackPos	 = XMFLOAT3(bg->w / 2, bg->h / 2, 0.0f);

		g_Boss.hpUIColor	 = XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f);
	}

	//�d�C�ʂ̏�����
	for (int i = 0; i < BOSS_PROJECTILE_MAX; i++)
	{

		g_Projectile[i].use			  = FALSE;
		g_Projectile[i].pos			  = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Projectile[i].w			  = 80.0f;
		g_Projectile[i].h			  = 80.0f;
		g_Projectile[i].texNo		  = BOSS_TEX_LIGHTNING_BALL;
		g_Projectile[i].countAnim	  = 0;
		g_Projectile[i].currentSprite = 0;

		g_Projectile[i].moveSpeed	  = BOSS_PROJECTILE_SPEED;
		g_Projectile[i].moveDir		  = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Projectile[i].color		  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_Projectile[i].angle		  = 0.0f;

	}


	//���[�U�[�̏�����
	g_Laser.use		= FALSE;
	g_Laser.pos		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Laser.w		= THICK_LASER;
	g_Laser.h		= bg->h;
	g_Laser.texNo	= BOSS_TEX_LASER_SIGN;
	g_Laser.color	= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//�p�[�e�B�N���̏�����
	for (int i = 0; i < BOSS_PARTICLE_MAX; i++)
	{
		g_Particle[i].use	= FALSE;
		g_Particle[i].pos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Particle[i].w		= PARTICLE_SIZE;
		g_Particle[i].h		= PARTICLE_SIZE;
		g_Particle[i].texNo = BOSS_TEX_PARTIBLE;

		g_Particle[i].moveSpeed   = BOSS_PROJECTILE_SPEED;
		g_Particle[i].moveDir	  = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Particle[i].color		  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Particle[i].lifeTime	  = 0;
		g_Particle[i].lifeTimeCnt = 0;
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBoss(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < BOSS_TEX_MAX; i++)
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
void UpdateBoss(void)
{
	
	UpdateBossProjectile();


	if (g_Boss.spawned == FALSE) return;
	// �����Ă�G�l�~�[��������������
	if (g_Boss.alive == TRUE)
	{
		PLAYER* player = GetPlayer();
		// PLAYER�Ƃ̓����蔻��
		{
			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < PLAYER_MAX; j++)
			{
				// �����Ă�G�l�~�[�Ɠ����蔻�������
				if (player[j].alive == TRUE && player[j].invincible == FALSE)
				{
					BOOL ans;

					//BOSS �̂����蔻��
					{
						ans = CollisionBB(g_Boss.pos,		g_Boss.w,		g_Boss.h,
										  player[j].pos,	player[j].w,	player[j].h);

						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							PlayerDamaged(g_Boss.pos);
							//Retroceso
						}
					}
					
					//Laser �̂����蔻��
					{
						if (g_Laser.texNo == BOSS_TEX_LASER)
						{
							ans = CollisionBB(g_Laser.pos,		g_Laser.w,		g_Laser.h,
											  player[j].pos,	player[j].w,	player[j].h);

							if (ans == TRUE)
							{
								// �����������̏���
								PlayerDamaged(g_Boss.pos);
								PlaySound(SOUND_LABEL_SE_ElectricShock);
							}
						}
					}
					
					
					//PROJECTILE �̂����蔻��
					{
						for (int i = 0; i < g_Boss.projectileNum; i++)
						{
							if (g_Projectile[i].use == TRUE)
							{
								ans = CollisionBB(g_Projectile[i].pos,  g_Projectile[i].w / 2,  g_Projectile[i].h / 2,
												  player[j].pos,		player[j].w,			player[j].h);

								if (ans == TRUE)
								{
									// �����������̏���
									PlayerDamaged(g_Boss.pos);
									PlaySound(SOUND_LABEL_SE_ElectricShock);
								}
							}
						}
					}
				}
			}
		}

		//�G�l�~�[�̏�ԍX�V
		{
			switch (g_Boss.state)
			{
			case BOSS_STATE_CHASE:
			{
				//CHASE��Ԃ��n�߂�Ɠd�C�ʂ��o�Ă���
				{
					//�d�C�ʐ��̐ݒ�
					switch (g_Boss.phase)
					{
					case 1:
						g_Boss.projectileNum = PHASE_1_NUMBER_OF_ATTACKS_SHOTS;
						break;

					case 2:
						g_Boss.projectileNum = PHASE_2_NUMBER_OF_ATTACKS_SHOTS;
						break;

					case 3:
						g_Boss.projectileNum = PHASE_3_NUMBER_OF_ATTACKS_SHOTS;
						break;
					}

					//�d�C�ʂ̐ݒ�
					if (g_Projectile[g_Boss.projectileNum - 1].use == FALSE)
					{
						SetBossProjectile();
						break;
					}
				}

				//CHASE�̊J�n�_�ƏI���_�̐ݒ�
				if (g_Boss.attackTimeCnt == 0)			//CHASE SET
				{
					g_Boss.moveStartPos = g_Boss.pos;
					g_Boss.moveEndPos   = player[0].pos;
				}
				
				//�ړ�����
				double t = g_Boss.attackTimeCnt / g_Boss.attackTime;
				t = easeInOutQuad(t);

				g_Boss.pos.x = (float)((double)g_Boss.moveStartPos.x + ((double)g_Boss.moveEndPos.x - (double)g_Boss.moveStartPos.x) * t);
				g_Boss.pos.y = (float)((double)g_Boss.moveStartPos.y + ((double)g_Boss.moveEndPos.y - (double)g_Boss.moveStartPos.y) * t);

				g_Boss.attackTimeCnt += 1.0f;
				

				//�I���_�ɂ�����A���Z�b�g
				if (t >= 1)
				{
					g_Boss.attackTimeCnt = 0;
					g_Boss.attackCnt++;

					//CHASE���I�������ASHOOTING��Ԃ�������
					if (g_Boss.attackCnt >= g_Boss.attacks)
					{
						g_Boss.state		= BOSS_STATE_SHOOTING;
						g_Boss.attackTime	= 120;
						g_Boss.attackCnt	= 0;

						g_Boss.moveStartPos = g_Boss.pos;
						g_Boss.moveEndPos   = g_Boss.attackPos;
					}	
				}
				
			}
				break;

			case BOSS_STATE_SHOOTING:
			{
				//�ŏ��ɍU���ꏊ�܂ōs���܂�
				if (g_Boss.shoot == FALSE)
				{
					//�ړ�����
					double t = g_Boss.attackTimeCnt / g_Boss.attackTime;
					
					t = easeInOutQuad(t);

					g_Boss.pos.x = (float)((double)g_Boss.moveStartPos.x + ((double)g_Boss.moveEndPos.x - (double)g_Boss.moveStartPos.x) * t);
					g_Boss.pos.y = (float)((double)g_Boss.moveStartPos.y + ((double)g_Boss.moveEndPos.y - (double)g_Boss.moveStartPos.y) * t);

					g_Boss.attackTimeCnt += 1.0f;

					//�U���ꏊ�ɂ�����
					if (g_Boss.attackTimeCnt >= g_Boss.attackTime)
					{
						//���ːݒ�
						g_Boss.shoot		 = TRUE;
						g_Boss.currentAnim   = BOSS_ANIM_SHOOTING;
						g_Boss.attackTimeCnt = 0.0f;

						//���ˑ��x�̐ݒ�
						switch (g_Boss.phase)
						{
						case 1:
							g_Boss.attackTime = PHASE_1_ATTACK_DURATION_SHOTS;
							break;

						case 2:
							g_Boss.attackTime = PHASE_2_ATTACK_DURATION_SHOTS;
							break;

						case 3:
							g_Boss.attackTime = PHASE_3_ATTACK_DURATION_SHOTS;
							break;
						}
					}
					break;
					
				}
				
				
				//�d�C�ʂ��Ȃ��Ȃ�����ALASER��Ԃ�ݒ肷��
				if (g_Projectile[g_Boss.projectileNum - 1].use == FALSE)
				{
					//�U�����x�ƍU���񐔂�ݒ�
					switch (g_Boss.phase)
					{
					case 1:
						g_Boss.attackTime = PHASE_1_ATTACK_DURATION_LASER;
						g_Boss.attacks	  = PHASE_1_NUMBER_OF_ATTACKS_LASER;
						break;

					case 2:
						g_Boss.attackTime = PHASE_2_ATTACK_DURATION_LASER;
						g_Boss.attacks    = PHASE_2_NUMBER_OF_ATTACKS_LASER;
						break;

					case 3:
						g_Boss.attackTime = PHASE_3_ATTACK_DURATION_LASER;
						g_Boss.attacks    = PHASE_3_NUMBER_OF_ATTACKS_LASER;
						break;
					}

					//LASER��Ԃ̐ݒ�
					g_Boss.state		 = BOSS_STATE_LASER;
					g_Boss.currentAnim	 = BOSS_ANIM_LASER;
					g_Boss.attackTimeCnt = 0;
					g_Boss.shoot		 = FALSE;
					g_Laser.use			 = TRUE;
				}
			}
				break;

			case BOSS_STATE_LASER:
			{
				//���U�[�̐ݒ�
				if (g_Boss.attackTimeCnt == 0)
				{
					//���U�[��X���W���v���C���[�̂̈ʒu�ɐݒ�
					g_Laser.pos = XMFLOAT3(player[0].pos.x, SCREEN_CENTER_Y, 0.0f);
					PlaySound(SOUND_LABEL_SE_ChargeBeam);

					//�ŏ��̍U���Ɠ����Ƀv���b�g�t�H�[�����W�J���܂�
					if (g_Boss.attackCnt == 0)
					{
						BG* bg = GetBG();
						SetPlatform(XMFLOAT3(bg->w / 2 - 250.0f, SCREEN_CENTER_Y + 80.0f, 0.0f));
						SetPlatform(XMFLOAT3(bg->w / 2 + 250.0f, SCREEN_CENTER_Y + 80.0f, 0.0f));
					}
				}

				//���U�[�̃|���S���̕��𒲐����鏈��
				{
					double t = g_Boss.attackTimeCnt / g_Boss.attackTime;

					t = easeInOutQuad(t);

					g_Laser.w = (float)((double)THIN_LASER + ((double)THICK_LASER - (double)THIN_LASER) * t);

					if (g_Laser.texNo == BOSS_TEX_LASER_SIGN)g_Boss.attackTimeCnt += 1.0f;
					if (g_Laser.texNo == BOSS_TEX_LASER)	 g_Boss.attackTimeCnt -= 1.0f;
				}
					
				//���U�[�ƃ��U�[�̒����̐؂�ւ�
				if (g_Boss.attackTimeCnt == g_Boss.attackTime)
				{
					if (g_Laser.texNo == BOSS_TEX_LASER_SIGN)
					{
						g_Laser.texNo = BOSS_TEX_LASER;						
						PlaySound(SOUND_LABEL_SE_LaserSound);	//Laser sound
					}
				}
				else if (g_Boss.attackTimeCnt == 0)
				{
					if (g_Boss.attackCnt < g_Boss.attacks)
					{
						g_Laser.texNo = BOSS_TEX_LASER_SIGN;
						g_Boss.attackCnt++;
					}
					else 
					{
						//�S���̃��U�[��ł�����ACHASE�̐ݒ�����܂�
						switch (g_Boss.phase)
						{
						case 1:
							g_Boss.attackTime = PHASE_1_ATTACK_DURATION_CHASE;
							g_Boss.attacks = PHASE_1_NUMBER_OF_ATTACKS_CHASE;
							break;

						case 2:
							g_Boss.attackTime = PHASE_2_ATTACK_DURATION_CHASE;
							g_Boss.attacks = PHASE_2_NUMBER_OF_ATTACKS_CHASE;
							break;

						case 3:
							g_Boss.attackTime = PHASE_3_ATTACK_DURATION_CHASE;
							g_Boss.attacks = PHASE_3_NUMBER_OF_ATTACKS_CHASE;
							break;
						}

						g_Laser.use = FALSE;
						g_Laser.texNo = BOSS_TEX_LASER_SIGN;
						g_Boss.state = BOSS_STATE_CHASE;
						g_Boss.currentAnim = BOSS_ANIM_CHASE;
						g_Boss.attackCnt = 0;
						ResetPlatform();
					}
				}
			}
				break;
			}

		}



		//�G�l�~�[�����t����
		
		if (g_Boss.damaged == TRUE)
		{
			g_Boss.dmgTimeCnt++;

			if (g_Boss.dmgTimeCnt >= DAMAGE_TIME)
			{
				g_Boss.damaged    = FALSE;
				g_Boss.dmgTimeCnt = 0;
			}
		}
		


	}
	else
	{
		DespawnBoss();
	}
	


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBoss(void)
{
	if (g_Boss.spawned == FALSE) return;

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
	if (g_Boss.alive == FALSE)
	{
		for (int i = 0; i < BOSS_PARTICLE_MAX; i++)
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
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Boss.texNo]);

	//�X�v���C�g�̊Ǘ�����
	if (g_Boss.alive == FALSE)
	{
		g_Boss.countAnim++;
		if (g_Boss.countAnim == ANIM_WAIT)
		{
			g_Boss.countAnim = 0;

			if (g_Boss.currentAnim == BOSS_ANIM_DEATH1)
			{
				g_Boss.currentAnim = BOSS_ANIM_DEATH2;
			}
			else {
				g_Boss.currentAnim = BOSS_ANIM_DEATH1;
			}
		}
	}
	

	//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_Boss.pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
	float py = g_Boss.pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
	float pw = g_Boss.w;					// �G�l�~�[�̕\����
	float ph = g_Boss.h;					// �G�l�~�[�̕\������

	float tw = 1.0f;						// �e�N�X�`���̕�
	float th = 1.0f / BOSS_ANIM_MAX;		// �e�N�X�`���̍���
	float tx = tw * g_Boss.currentSprite;	// �e�N�X�`���̍���X���W
	float ty = th * g_Boss.currentAnim;		// �e�N�X�`���̍���Y���W


	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					g_Boss.enemyColor);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	if (g_Boss.alive == TRUE)
	{
		DrawBossProjectiles();

		DrawBossLaser();
	}
	

	DrawBossUI();
}


//=============================================================================
// BOSS�̗̑�UI�̕`�揈��
//=============================================================================
void DrawBossUI(void)
{
	BG* bg = GetBG();

	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BOSS_TEX_BOSS_UI]);

		//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = SCREEN_WIDTH - 40.0f;	// �G�l�~�[�̕\���ʒuX
		float py = 35.0f;					// �G�l�~�[�̕\���ʒuY
		float pw = 500.0f;					// �G�l�~�[�̕\����
		float ph = 50.0f;					// �G�l�~�[�̕\������

		float tw = 1.0f;					// �e�N�X�`���̕�
		float th = 1.0f;					// �e�N�X�`���̍���
		float tx = 0.0f;					// �e�N�X�`���̍���X���W
		float ty = 0.0f;					// �e�N�X�`���̍���Y���W


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteRightTop(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BOSS_TEX_BOSS_UI_HP]);

		//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = SCREEN_WIDTH - 47.0f;			// �G�l�~�[�̕\���ʒuX
		float py = 42;								// �G�l�~�[�̕\���ʒuY
		float pw = 480.0f / HP_MAX * g_Boss.HP;		// �G�l�~�[�̕\����
		float ph = 10.0f;							// �G�l�~�[�̕\������

		float tw = 1.0f;							// �e�N�X�`���̕�
		float th = 1.0f;							// �e�N�X�`���̍���
		float tx = 0.0f;							// �e�N�X�`���̍���X���W
		float ty = 0.0f;							// �e�N�X�`���̍���Y���W


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteRTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, g_Boss.hpUIColor);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
BOSS GetBoss(void)
{
	return g_Boss;
}


//=============================================================================
// BOSS�̃_���[�W�ݒ�
// ����: damage (BOSS���󂯂�_���[�W)
//=============================================================================
void BossDamaged(float damage)
{
	g_Boss.damaged = TRUE;
	g_Boss.HP -= damage;
	PlaySound(SOUND_LABEL_SE_bloodyBlade);

	float phaseHP = HP_MAX / 3;

	if (g_Boss.HP <= 0)
	{
		g_Boss.alive = FALSE;
	}
	else if (g_Boss.HP <= phaseHP)
	{
		g_Boss.phase	 = 3;
		g_Boss.hpUIColor = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	}
	else if (g_Boss.HP <= phaseHP * 2)
	{
		g_Boss.phase	 = 2;
		g_Boss.hpUIColor = XMFLOAT4(0.8f, 0.8f, 0.0f, 1.0f);
	}
}


//=============================================================================
// BOSS�̏o��
// ����: damage (BOSS���󂯂�_���[�W)
//=============================================================================
void SpawnBoss(XMFLOAT3 spawnPos)
{
	g_Boss.spawned	= TRUE;
	g_Boss.alive	= TRUE;
	g_Boss.pos		= spawnPos;
}


//=============================================================================
// BOSS�̏��ŏ���
//=============================================================================
void DespawnBoss(void)
{
	g_Boss.dmgTimeCnt++;
	SetBossPartible(g_Boss.pos);

	if (g_Boss.dmgTimeCnt >= DESPAWN_TIME)
	{
		if (g_Boss.w > 10)
		{
			g_Boss.w -= 1.0f;
			g_Boss.h -= 1.2f;
			return;
		}

		g_Boss.spawned		 = FALSE;
		g_Boss.alive		 = FALSE;
		g_Boss.pos			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss.w			 = TEXTURE_WIDTH;
		g_Boss.h			 = TEXTURE_HEIGHT;

		g_Boss.currentSprite = 0;
		g_Boss.currentAnim	 = BOSS_ANIM_CHASE;

		g_Boss.HP			 = HP_MAX;
		g_Boss.damaged		 = FALSE;
		g_Boss.dmgTimeCnt	 = 0;
							 
		g_Boss.enemyColor	 = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
							 
		g_Boss.phase		 = 1;
		g_Boss.attackCnt	 = 0;
		g_Boss.state		 = BOSS_STATE_CHASE;

		RoundKill();
	}
}

//=============================================================================
// �p�[�e�B�N���̐ݒ�
//=============================================================================
void SetBossPartible(XMFLOAT3 pos)
{
	for (int i = 0; i < BOSS_PARTICLE_MAX; i++)
	{
		if (g_Particle[i].use == FALSE)
		{
			g_Particle[i].use	= TRUE;
			g_Particle[i].pos	= pos;
			g_Particle[i].texNo = BOSS_TEX_PARTIBLE;

			float angle = (float)(rand() % 628) / 100;
			g_Particle[i].moveDir.x   = cosf(angle);	//angle�̕����ֈړ�
			g_Particle[i].moveDir.y   = sinf(angle);	//angle�̕����ֈړ�

			g_Particle[i].lifeTime	  = BOSS_PARTICLE_MAX;
			g_Particle[i].lifeTimeCnt = 0;

			g_Particle[i].color		  = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		}		
	}

	for (int i = 0; i < BOSS_PARTICLE_MAX; i++)
	{
		if (g_Particle[i].use == TRUE)
		{
			g_Particle[i].lifeTimeCnt++;

			g_Particle[i].pos.x	  += g_Particle[i].moveDir.x * g_Particle[i].moveSpeed;
			g_Particle[i].pos.y	  += g_Particle[i].moveDir.y * g_Particle[i].moveSpeed;

			g_Particle[i].color.w -= 1.0f/ BOSS_PARTICLE_MAX;
			g_Particle[i].color.z += 1.0f/ BOSS_PARTICLE_MAX;

			if (g_Particle[i].lifeTimeCnt == g_Particle[i].lifeTime)
			{
				g_Particle[i].use = FALSE;
			}
		}

	}
}

//=============================================================================
// �X���[�C���E�X���[�A�E�g
// ����: t (���`��Ԃ�t�ϐ�)
//=============================================================================
double easeInOutQuad(double t) 
{
	return t < 0.5 ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2;
}

//=============================================================================
// �d�C�ʂ̐ݒ�
//=============================================================================
void SetBossProjectile(void)
{
	for (int i = 0; i < g_Boss.projectileNum; i++)
	{
		if (g_Projectile[i].use == FALSE)
		{
			if (i == 0) 
			{
				g_Projectile[i].use   = TRUE;
				g_Projectile[i].angle = 0.0f;
				return;
			}
			else
			{
				if (g_Projectile[0].angle > (XM_2PI / g_Boss.projectileNum) * i)
				{
					g_Projectile[i].use   = TRUE;
					g_Projectile[i].angle = 0.0f;
					return;
				}
			}
			
		}
	}
}

//=============================================================================
// �d�C�ʂ̏���
//=============================================================================
void UpdateBossProjectile(void)
{
	if (g_Boss.shoot == TRUE)//SHOOT ���
	{
		if (g_Boss.attackTimeCnt < g_Boss.attackTime)
		{
			g_Boss.attackTimeCnt++;
			
		}

		for (int i = 0; i < g_Boss.projectileNum; i++)
		{
			if (g_Projectile[i].use == FALSE) continue;
			
			if (g_Projectile[i].moveDir.x == 0.0f)		//Set projectile direction
			{
				if (g_Boss.attackTimeCnt >= g_Boss.attackTime)
				{
					g_Boss.attackTimeCnt = 0;

					PLAYER* player = GetPlayer();

					XMVECTOR BossPos = XMLoadFloat3(&g_Projectile[i].pos);
					XMVECTOR vec = (XMLoadFloat3(&player[0].pos) - BossPos);

					float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);

					g_Projectile[i].moveDir.x = cosf(angle);
					g_Projectile[i].moveDir.y = sinf(angle);

					PlaySound(SOUND_LABEL_SE_MagicCast);
				}
				
			}
			else										//Update Projectile position
			{
				g_Projectile[i].pos.x += g_Projectile[i].moveDir.x * g_Projectile[i].moveSpeed;
				g_Projectile[i].pos.y += g_Projectile[i].moveDir.y * g_Projectile[i].moveSpeed;


				//��ʒ[�̓����蔻��
				{
					BG* bg = GetBG();
					if (g_Projectile[i].pos.x < (-g_Projectile[i].w) ||			//��
						g_Projectile[i].pos.x >(bg->w + g_Projectile[i].w) ||		//�E
						g_Projectile[i].pos.y < (-g_Projectile[i].h) ||			//��
						g_Projectile[i].pos.y >(bg->h + g_Projectile[i].h))		//��
					{
						g_Projectile[i].use		= FALSE;
						g_Projectile[i].moveDir = XMFLOAT3(0.0f, 0.0f, 0.0f);
					}
				}
			}
		}
	}
	else//CHASE ���
	{
		for (int i = 0; i < g_Boss.projectileNum; i++)
		{
			if (g_Projectile[i].use == TRUE)
			{
				g_Projectile[i].pos.x = g_Boss.pos.x + cosf(g_Projectile[i].angle) * 100.0f;
				g_Projectile[i].pos.y = g_Boss.pos.y + sinf(g_Projectile[i].angle) * 100.0f;

				g_Projectile[i].angle += 0.05f;
			}
		}
	}
	

	
}

//=============================================================================
// �d�C�ʂ̕`��
//=============================================================================
void DrawBossProjectiles(void)
{
	BG* bg = GetBG();

	for (int i = 0; i < g_Boss.projectileNum; i++)
	{
		if (g_Projectile[i].use == TRUE)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Projectile[i].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Projectile[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_Projectile[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_Projectile[i].w;					// �G�l�~�[�̕\����
			float ph = g_Projectile[i].h;					// �G�l�~�[�̕\������

			float tw = 1.0f / 4;		// �e�N�X�`���̕�
			float th = 1.0f;		// �e�N�X�`���̍���
			float tx = tw * g_Projectile[i].currentSprite;		// �e�N�X�`���̍���X���W
			float ty = 0.0f;		// �e�N�X�`���̍���Y���W


			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_Projectile[i].color);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);


			g_Projectile[i].countAnim++;
			if (g_Projectile[i].countAnim >= ANIM_WAIT)
			{
				g_Projectile[i].countAnim = 0;
				g_Projectile[i].currentSprite = (g_Projectile[i].currentSprite + 1) % 4;
			}
		}
	}
	
}


//=============================================================================
// ���U�[�̕`��
//=============================================================================
void DrawBossLaser(void)
{
	BG* bg = GetBG();

	if (g_Laser.use == TRUE)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Laser.texNo]);

		//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Laser.pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
		float py = g_Laser.pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
		float pw = g_Laser.w;					// �G�l�~�[�̕\����
		float ph = g_Laser.h;					// �G�l�~�[�̕\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���
		float tx = 0.0f;		// �e�N�X�`���̍���X���W
		float ty = 0.0f;		// �e�N�X�`���̍���Y���W


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_Laser.color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}