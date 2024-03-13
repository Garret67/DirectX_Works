//=============================================================================
//
// �o���b�g���� [bullet.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "bullet.h"

#include "enemyPatrol.h"
#include "enemyFlyBulletOne.h"
#include "enemyFlyBounce.h"
#include "enemyBall.h"
#include "enemyKnight.h"
#include "enemySlimeKing.h"
#include "enemyBoss.h"

#include "collision.h"
#include "score.h"
#include "bg.h"
#include "player.h"
#include "platforms.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(70.0f / 2)		// �e�̕�
#define TEXTURE_HEIGHT				(277.0f / 2)	// �e�̍���

#define TEXTURE_PATTERN_DIVIDE_X	(8)				// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)				// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)				// �A�j���[�V�����̐؂�ւ��Wait�l

#define BULLET_DAMAGE				(10.0f)			// �e�̃_���[�W
#define BULLET_SPEED				(15.0f)			// �e�̈ړ��X�s�[�h

#define EXPLOSION_WIDTH				(100.0f)		// �e�̔����̕�
#define EXPLOSION_HEIGHT			(100.0f)		// �e�̔����̍���

#define BULLET_MAX					(10)			// �e��Max��
#define EXPLOSION_MAX				(10)			// �e��Max��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DrawBulletExplosion(void);			//�e�̔����A�j���[�V�����`��
void ExplosionAnimationUpdate(void);	//�e�̔����A�j���[�V��������

void SetExplosion(XMFLOAT3 pos);		//�e�̐ݒ�

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[BULLET_TEX_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[BULLET_TEX_MAX] =
{
	"data/TEXTURE/Effect/FireBullet.png",
	"data/TEXTURE/Effect/ColliderImage.png",
	"data/TEXTURE/Effect/BulletExplosionG.png",
};

static BULLET		g_Bullet[BULLET_MAX];		// �o���b�g�\����
static EXPLOSION	g_Explosion[EXPLOSION_MAX];	// �o���b�g�̔����\����

static BOOL			g_Load = FALSE;				// ���������s�������̃t���O


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < BULLET_TEX_MAX; i++)
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


	// �o���b�g�\���̂̏�����
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use			= FALSE;
		g_Bullet[i].w			= TEXTURE_WIDTH;
		g_Bullet[i].h			= TEXTURE_HEIGHT;
		g_Bullet[i].pos			= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].rot			= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].texNo		= BULLET_TEX_FIRE_BULLET;

		g_Bullet[i].countAnim	= 0;
		g_Bullet[i].patternAnim = 0;

		g_Bullet[i].move		= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].color		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}


	// EXPLOSION�\���̂̏�����
	for (int i = 0; i < EXPLOSION_MAX; i++)
	{
		g_Explosion[i].use			 = FALSE;
		g_Explosion[i].pos			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Explosion[i].w			 = EXPLOSION_WIDTH;
		g_Explosion[i].h			 = EXPLOSION_HEIGHT;
		g_Explosion[i].texNo		 = BULLET_TEX_BULLET_EXPLOSION;

		g_Explosion[i].countAnim	 = 0;
		g_Explosion[i].currentSprite = 0;

		g_Explosion[i].color		 = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBullet(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < BULLET_TEX_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBullet(void)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)	continue;	// ���̃o���b�g���g���Ă���H
		
		// �A�j���[�V����  
		g_Bullet[i].countAnim++;
		if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
		{
			// �p�^�[���̐؂�ւ�
			g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
		}


		// �o���b�g�̈ړ�����
		XMVECTOR pos  = XMLoadFloat3(&g_Bullet[i].pos);
		XMVECTOR move = XMLoadFloat3(&g_Bullet[i].move);
		pos += move;
		XMStoreFloat3(&g_Bullet[i].pos, pos);


		// ��ʊO�܂Ői�񂾁H
		BG* bg = GetBG();
		if (g_Bullet[i].pos.x < (-g_Bullet[i].w/2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
		{
			g_Bullet[i].use = FALSE;
		}
		if (g_Bullet[i].pos.x > (bg->w + g_Bullet[i].w/2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
		{
			g_Bullet[i].use = FALSE;
		}


		//�����蔻��̍��W�A�����ƕ�
		XMFLOAT3 BulletPos = XMFLOAT3(/* X */ g_Bullet[i].pos.x + (cosf(g_Bullet[i].rot.z + (XM_PIDIV2)) * (g_Bullet[i].h / 2 - g_Bullet[i].w / 2)),
									  /* Y */ g_Bullet[i].pos.y + (sinf(g_Bullet[i].rot.z + (XM_PIDIV2)) * (g_Bullet[i].h / 2 - g_Bullet[i].w / 2)),
									  /* Z */ 0.0f);

		//�����蔻��̂��߂̕��ƍ���
		float BulletWH = g_Bullet[i].w;	


		// Ground �Ƃ̓����蔻��
		{
			PLATFORM* ground = GetGround();

			// groundS�̐��������蔻����s��
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;

				BOOL ans = CollisionBB(	BulletPos,		BulletWH,		BulletWH,
										ground[j].pos,	ground[j].w,	ground[j].h);

				// �������Ă���H
				if (ans == TRUE)
				{
					// �����������̏���
					SetExplosion(BulletPos);					//�e�̔����ݒ�
					g_Bullet[i].use = FALSE;					//�e�𖢎g�p�ɂ���
					PlaySound(SOUND_LABEL_SE_MagicExplosion);	//��
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

				BOOL ans = CollisionBB(	BulletPos,			BulletWH,			BulletWH,
										platform[j].pos,	(platform[j].w),	platformH);

				// �������Ă���H
				if (ans == TRUE)
				{
					// �����������̏���
					SetExplosion(BulletPos);					//�e�̔����ݒ�
					g_Bullet[i].use = FALSE;					//�e�𖢎g�p�ɂ���
					PlaySound(SOUND_LABEL_SE_MagicExplosion);	//��
				}
			}
		}




		// �v���C���[�Ƃ̓����蔻�菈��
		{
			if (g_Bullet[i].color.z == 0.0f)
			{

				PLAYER* player = GetPlayer();

				// �v���C���[�̐��������蔻����s��
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					// �����Ă�v���C���[�Ɠ����蔻�������
					if (player[j].alive == TRUE && player[j].invincible == FALSE)
					{
						BOOL ans = CollisionBB(	BulletPos,		BulletWH,		BulletWH,
												player[j].pos,	player[j].w,	player[j].h);

						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							PlayerDamaged(BulletPos);					//�v���C���[�̃_���[�W
							SetExplosion(player[j].pos);				//�e�̔����ݒ�
							g_Bullet[i].use = FALSE;					//�e�𖢎g�p�ɂ���
							PlaySound(SOUND_LABEL_SE_MagicExplosion);	//��
						}
					}
				}
			}
		}


		// �G�l�~�[�̓����蔻�菈��
		{
			if (g_Bullet[i].color.z == 0.0f) continue;	//�G�l�~�[�̒e��������G�l�~�[�Ƃ̓����蔻������Ȃ�


			BOSS Boss = GetBoss();
	
			// �����Ă�G�l�~�[�Ɠ����蔻�������
			if (Boss.alive == TRUE)
			{
				BOOL ans = CollisionBB(	BulletPos,	BulletWH,	BulletWH,
										Boss.pos,	Boss.w,		Boss.h);

				// �������Ă���H
				if (ans == TRUE)
				{
					// �����������̏���
					BossDamaged(BULLET_DAMAGE);					//�G�l�~�[�̃_���[�W
					SetExplosion(Boss.pos);						//�e�̔����ݒ�
					g_Bullet[i].use = FALSE;					//�e�𖢎g�p�ɂ���
					PlaySound(SOUND_LABEL_SE_MagicExplosion);	//��

				}
			}
				



			SLIME_KING slimeKing = GetSlimeKing();

			// �����Ă�G�l�~�[�Ɠ����蔻�������
			if (slimeKing.alive == TRUE)
			{
				BOOL ans = CollisionBB(	BulletPos,		BulletWH,		BulletWH,
										slimeKing.pos,	slimeKing.w,	slimeKing.h);

				// �������Ă���H
				if (ans == TRUE)
				{
					// �����������̏���
					SlimeKingDamaged(BULLET_DAMAGE);			//�G�l�~�[�̃_���[�W
					SetExplosion(slimeKing.pos);				//�e�̔����ݒ�
					g_Bullet[i].use = FALSE;					//�e�𖢎g�p�ɂ���
					PlaySound(SOUND_LABEL_SE_MagicExplosion);	//��
				}
			}
				


			ENEMY_PATROL* enemyPatrol = GetEnemyPatrol();

			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < ENEMY_PATROL_MAX; j++)
			{
				// �����Ă�ƃ_���[�W��Ԃ���Ȃ��G�l�~�[�Ɠ����蔻�������
				if (enemyPatrol[j].alive == TRUE && enemyPatrol[j].damaged == FALSE)
				{
					BOOL ans = CollisionBB(	BulletPos,			BulletWH,			BulletWH,
											enemyPatrol[j].pos, enemyPatrol[j].w,	enemyPatrol[j].h);

					// �������Ă���H
					if (ans == TRUE)
					{
						// �����������̏���
						EnemyDamaged(g_Bullet[i].pos, j, BULLET_DAMAGE);	//�G�l�~�[�̃_���[�W
						SetExplosion(enemyPatrol[j].pos);					//�e�̔����ݒ�
							
					}
				}
			}


			ENEMY_BALL* enemyBall = GetEnemyBall();

			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < ENEMY_BALL_MAX; j++)
			{
				// �����Ă�ƃ_���[�W��Ԃ���Ȃ��G�l�~�[�Ɠ����蔻�������
				if (enemyBall[j].alive == TRUE && enemyBall[j].damaged == FALSE)
				{
					BOOL ans = CollisionBB(	BulletPos,			BulletWH,			BulletWH,
											enemyBall[j].pos,	enemyBall[j].w,		enemyBall[j].h);

					// �������Ă���H
					if (ans == TRUE)
					{
						// �����������̏���
						EnemyBallDamaged(g_Bullet[i].pos, j, BULLET_DAMAGE);	//�G�l�~�[�̃_���[�W
						SetExplosion(enemyBall[j].pos);							//�e�̔����ݒ�
					}
				}
			}



			ENEMY_FLY_BOUNCE* enemyFlyBounce = GetEnemyFlyBounce();

			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < ENEMY_FLY_BOUNCE_MAX; j++)
			{
				// �����Ă�ƃ_���[�W��Ԃ���Ȃ��G�l�~�[�Ɠ����蔻�������
				if (enemyFlyBounce[j].alive == TRUE && enemyFlyBounce[j].damaged == FALSE)
				{
					BOOL ans = CollisionBB(	BulletPos,				BulletWH,				BulletWH,
											enemyFlyBounce[j].pos,	enemyFlyBounce[j].w,	enemyFlyBounce[j].h);

					// �������Ă���H
					if (ans == TRUE)
					{
						// �����������̏���
						EnemyFlyBounceDamaged(g_Bullet[i].pos, j, BULLET_DAMAGE);	//�G�l�~�[�̃_���[�W
						SetExplosion(enemyFlyBounce[j].pos);						//�e�̔����ݒ�
					}
				}
			}




			ENEMY_FLY_ONE* enemyFlyOne = GetEnemyFlyOne();

			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < ENEMY_FLY_ONE_MAX; j++)
			{
				// �����Ă�ƃ_���[�W��Ԃ���Ȃ��G�l�~�[�Ɠ����蔻�������
				if (enemyFlyOne[j].alive == TRUE && (enemyFlyOne[j].state != FLY_ONE_STATE_HIT))
				{
					BOOL ans = CollisionBB(	BulletPos,				BulletWH,			BulletWH,
											enemyFlyOne[j].pos,		enemyFlyOne[j].w,	enemyFlyOne[j].h);
					// �������Ă���H
					if (ans == TRUE)
					{
						// �����������̏���
						EnemyFlyOneDamaged(g_Bullet[i].pos, j, BULLET_DAMAGE);	//�G�l�~�[�̃_���[�W
						SetExplosion(enemyFlyOne[j].pos);						//�e�̔����ݒ�
					}
				}
			}



			ENEMY_KNIGHT* enemyKnight = GetEnemyKnight();

			// �G�l�~�[�̐��������蔻����s��
			for (int j = 0; j < ENEMY_KNIGHT_MAX; j++)
			{
				// �����Ă�ƃ_���[�W��Ԃ���Ȃ��G�l�~�[�Ɠ����蔻�������
				if (enemyKnight[j].alive == TRUE && (enemyKnight[j].state != KNIGHT_STATE_HIT))
				{
					BOOL ans = CollisionBB(	BulletPos,				BulletWH,					BulletWH,
											enemyKnight[j].pos,		KNIGHT_COLLIDER_WIDTH,		enemyKnight[j].h);
					// �������Ă���H
					if (ans == TRUE)
					{
						// �����������̏���
						EnemyKnightDamaged(g_Bullet[i].pos, j, BULLET_DAMAGE);	//�G�l�~�[�̃_���[�W
						SetExplosion(enemyKnight[j].pos);						//�e�̔����ݒ�

					}
				}
			}
		}
		
	}

	ExplosionAnimationUpdate();	//�e�̔����A�j���[�V��������
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBullet(void)
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

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE) continue;	// ���̃o���b�g���g���Ă���H
		
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

		//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Bullet[i].pos.x - bg->pos.x;	// �o���b�g�̕\���ʒuX
		float py = g_Bullet[i].pos.y - bg->pos.y;	// �o���b�g�̕\���ʒuY
		float pw = g_Bullet[i].w;					// �o���b�g�̕\����
		float ph = g_Bullet[i].h;					// �o���b�g�̕\������

		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;										// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;										// �e�N�X�`���̍���
		float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
		float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, 
			px, py, pw, ph, 
			tx, ty, tw, th,
			g_Bullet[i].color,
			g_Bullet[i].rot.z);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


#ifdef _DEBUG	// �f�o�b�O����\������


		XMFLOAT3 BulletPos = XMFLOAT3(	/* X */ g_Bullet[i].pos.x + (cosf(g_Bullet[i].rot.z + (XM_PIDIV2)) * (g_Bullet[i].h / 2 - g_Bullet[i].w / 2)),
										/* Y */ g_Bullet[i].pos.y + (sinf(g_Bullet[i].rot.z + (XM_PIDIV2)) * (g_Bullet[i].h / 2 - g_Bullet[i].w / 2)),
										/* Z */ 0.0f);

		float BulletWH = g_Bullet[i].w;	//�����蔻��̂��߂̕��ƍ���
			
		BG* bg = GetBG();
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BULLET_TEX_COLLIDER]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer,
			BulletPos.x - bg->pos.x, BulletPos.y - bg->pos.y, BulletWH, BulletWH,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

#endif
		
	}

	DrawBulletExplosion();

}


//=============================================================================
// �e�̔����̕`�揈��
//=============================================================================
void DrawBulletExplosion(void)
{
	BG* bg = GetBG();

	for (int i = 0; i < EXPLOSION_MAX; i++)
	{
		if (g_Explosion[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Explosion[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Explosion[i].pos.x - bg->pos.x;	// �o���b�g�̕\���ʒuX
			float py = g_Explosion[i].pos.y - bg->pos.y;	// �o���b�g�̕\���ʒuY
			float pw = g_Explosion[i].w;		// �o���b�g�̕\����
			float ph = g_Explosion[i].h;		// �o���b�g�̕\������

			float tw = 1.0f / 4;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = g_Explosion[i].currentSprite * tw;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				g_Bullet[i].color);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾�B
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_Bullet[0];
}


//=============================================================================
// �v���C���[�̒e�̔��ːݒ�
// ����: pos (�e�̍��W), direction(�v���C���[�̌���)
//=============================================================================
void SetBulletPlayer(XMFLOAT3 pos, int direction)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Bullet[i].use   = TRUE;								// �g�p��Ԃ֕ύX����
			g_Bullet[i].pos   = pos;								// ���W���Z�b�g
			g_Bullet[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);	// �F�̐ݒ�

			//����������
			switch (direction)
			{
			case CHAR_DIR_LEFT:
				g_Bullet[i].move = XMFLOAT3(-BULLET_SPEED, 0.0f, 0.0f);
				g_Bullet[i].rot  = XMFLOAT3(0.0f, 0.0f, XM_PIDIV2);
				break;

			case CHAR_DIR_RIGHT:
				g_Bullet[i].move = XMFLOAT3(BULLET_SPEED, 0.0f, 0.0f);
				g_Bullet[i].rot  = XMFLOAT3(0.0f, 0.0f, -XM_PIDIV2);
				break;

			case CHAR_DIR_UP:
				g_Bullet[i].move = XMFLOAT3(0.0f, -BULLET_SPEED, 0.0f);
				g_Bullet[i].rot  = XMFLOAT3(0.0f, 0.0f, XM_PI);
				break;

			case CHAR_DIR_DOWN:
				g_Bullet[i].move = XMFLOAT3(0.0f, BULLET_SPEED, 0.0f);
				g_Bullet[i].rot  = XMFLOAT3(0.0f, 0.0f, 0.0f);
				break;
			}

			return;			// 1���Z�b�g�����̂ŏI������
		}
	}
}

//=============================================================================
// �G�l�~�[�̒e�̔��ːݒ�
// ����: pos (�e�̍��W), rotation (�e�̉�]), MoveDir(�ړ���)
//=============================================================================
void SetBulletEnemy(XMFLOAT3 pos, float rotation, XMFLOAT3 MoveDir)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Bullet[i].use   = TRUE;								// �g�p��Ԃ֕ύX����
			g_Bullet[i].pos   = pos;								// ���W���Z�b�g
			g_Bullet[i].move  = MoveDir;							// ����������
			g_Bullet[i].rot   = XMFLOAT3(0.0f, 0.0f, rotation);		// ��]
			g_Bullet[i].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);	// �F�̐ݒ�

			return;							// 1���Z�b�g�����̂ŏI������
		}
	}
}

//=============================================================================
// �e�̔����ݒ�
//=============================================================================
void SetExplosion(XMFLOAT3 pos)
{
	for (int i = 0; i < EXPLOSION_MAX; i++)
	{
		if (g_Explosion[i].use == FALSE)
		{
			g_Explosion[i].use = TRUE;
			g_Explosion[i].pos = pos;
			return;
		}
	}
	
}

//=============================================================================
// �e�̔����̏���
//=============================================================================
void ExplosionAnimationUpdate(void)
{
	for (int i = 0; i < EXPLOSION_MAX; i++)
	{
		if (g_Explosion[i].use == TRUE)
		{
			if (g_Explosion[i].countAnim > ANIM_WAIT)
			{
				//�A�j���[�V�������I������珉��������
				if (g_Explosion[i].currentSprite == 3)
				{
					g_Explosion[i].use				= FALSE;
					g_Explosion[i].pos				= XMFLOAT3(0.0f, 0.0f, 0.0f);
					g_Explosion[i].currentSprite	= 0;
					g_Explosion[i].countAnim		= 0;
				}

				g_Explosion[i].currentSprite++;
				g_Explosion[i].countAnim = 0;
			}
			g_Explosion[i].countAnim++;

		}
	}

}