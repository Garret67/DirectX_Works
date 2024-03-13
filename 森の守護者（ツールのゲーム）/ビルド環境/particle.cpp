//=============================================================================
//
// �p�[�e�B�N������ [particle.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "light.h"
#include "particle.h"
#include "modelBinaryFBX.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX				(2)			// �e�N�X�`���̐�

#define	PARTICLE_SIZE			(9.0f)		// ���_�T�C�Y

#define	VALUE_MOVE_PARTICLE		(5.0f)		// �ړ����x

#define	MAX_PARTICLE			(512)		// �p�[�e�B�N���ő吔
#define	PARTICLE_HEATH_MAX		(30)		// �p�[�e�B�N���ő吔
#define	PARTICLE_HEATH_TIME_MAX (120)		// �p�[�e�B�N���ő吔
#define	PARTICLE_RATE_HEATH		(5)			// �p�[�e�B�N���ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;		// �ʒu
	XMFLOAT3		rot;		// ��]
	XMFLOAT3		scale;		// �X�P�[��
	XMFLOAT3		move;		// �ړ���
	MATERIAL		material;	// �}�e���A��
	int				nLife;		// ����
	float			angle;		// HP�p�[�e�B�N���̉��p�x
	BOOL			use;		// �g�p���Ă��邩�ǂ���

	int				type;		// �g�p���Ă��邩�ǂ���
	int				texNo;		// �e�N�X�`���ԍ�


} PARTICLE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticle(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static PARTICLE					g_aParticle[MAX_PARTICLE];	// �p�[�e�B�N�����[�N
static XMFLOAT3					g_posBase;					// �r���{�[�h�����ʒu
static float					g_fWidthBase = 2.0f;		// ��̕�
static float					g_fHeightBase = 10.0f;		// ��̍���
static float					g_roty = 0.0f;				// �ړ�����
static float					g_spd = 0.0f;				// �ړ��X�s�[�h

//CURE PARTICLE
static BOOL						g_CureParticles;			// 
static int						g_ParticleRateCnt;			// �Đ��p�x
static int						g_ParticleTimeCnt;
static int						g_ParticleCnt;

static XMFLOAT3					g_ParticlePos;

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/Menu/star.png",
	"data/TEXTURE/Menu/star_06.png",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticle(void)
{
	// ���_���̍쐬
	MakeVertexParticle();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// �p�[�e�B�N�����[�N�̏�����
	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		g_aParticle[nCntParticle].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_aParticle[nCntParticle].material, sizeof(g_aParticle[nCntParticle].material));
		g_aParticle[nCntParticle].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aParticle[nCntParticle].nLife = 0;
		g_aParticle[nCntParticle].use = FALSE;
	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitParticle(void)
{
	if (g_Load == FALSE) return;

	//�e�N�X�`���̉��
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateParticle(void)
{
	//�p�[�e�B�N���̍X�V
	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(g_aParticle[nCntParticle].use)
		{// �g�p��

			switch (g_aParticle[nCntParticle].type)
			{
			case PARTICLE_HIT:

				g_aParticle[nCntParticle].pos.x += g_aParticle[nCntParticle].move.x;
				g_aParticle[nCntParticle].pos.z += g_aParticle[nCntParticle].move.z;

				g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y;
				if (g_aParticle[nCntParticle].pos.y <= PARTICLE_SIZE / 2)
				{// ���n����
					g_aParticle[nCntParticle].pos.y = PARTICLE_SIZE / 2;
					g_aParticle[nCntParticle].move.y = -g_aParticle[nCntParticle].move.y * 0.75f;
				}

				g_aParticle[nCntParticle].move.x += (0.0f - g_aParticle[nCntParticle].move.x) * 0.015f;
				g_aParticle[nCntParticle].move.y -= 0.05f;
				g_aParticle[nCntParticle].move.z += (0.0f - g_aParticle[nCntParticle].move.z) * 0.015f;


				g_aParticle[nCntParticle].nLife--;
				if (g_aParticle[nCntParticle].nLife <= 0)
				{
					g_aParticle[nCntParticle].use = FALSE;
				}
				else
				{
					if (g_aParticle[nCntParticle].nLife <= 20)
					{
						g_aParticle[nCntParticle].material.Diffuse.x = 0.8f - (float)(20 - g_aParticle[nCntParticle].nLife) / 20 * 0.8f;
						g_aParticle[nCntParticle].material.Diffuse.y = 0.7f - (float)(20 - g_aParticle[nCntParticle].nLife) / 20 * 0.7f;
						g_aParticle[nCntParticle].material.Diffuse.z = 0.2f - (float)(20 - g_aParticle[nCntParticle].nLife) / 20 * 0.2f;
					}

					if (g_aParticle[nCntParticle].nLife <= 20)
					{
						// ���l�ݒ�
						g_aParticle[nCntParticle].material.Diffuse.w -= 0.05f;
						if (g_aParticle[nCntParticle].material.Diffuse.w < 0.0f)
						{
							g_aParticle[nCntParticle].material.Diffuse.w = 0.0f;
						}
					}
				}

				g_aParticle[nCntParticle].rot.z -= 0.05f;

				break;

			case PARTICLE_HEATH:
				g_aParticle[nCntParticle].angle += g_aParticle[nCntParticle].move.x/*0.05f*/;
				if (g_aParticle[nCntParticle].angle > XM_2PI)
				{
					g_aParticle[nCntParticle].angle = 0.0f;
				}

				g_aParticle[nCntParticle].pos.x = g_ParticlePos.x + sinf(g_aParticle[nCntParticle].angle) * 15.0f;
				g_aParticle[nCntParticle].pos.z = g_ParticlePos.z + cosf(g_aParticle[nCntParticle].angle) * 15.0f;

				g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y;


				g_aParticle[nCntParticle].nLife--;
				if (g_aParticle[nCntParticle].nLife <= 0)
				{
					g_aParticle[nCntParticle].use = FALSE;
					g_ParticleCnt--;
				}
				else if(g_aParticle[nCntParticle].nLife <= 30)
				{
					
					// ���l�ݒ�
					g_aParticle[nCntParticle].material.Diffuse.w = (float)g_aParticle[nCntParticle].nLife / 30.0f;
					
				}

				break;

			default:
				break;
			}
		}
	}

	


	if (g_CureParticles)
	{

		if (g_ParticleTimeCnt++ > PARTICLE_HEATH_TIME_MAX)
		{
			g_CureParticles = FALSE;
			g_ParticleTimeCnt = 0;
		}


		g_ParticleRateCnt++;
		// �p�[�e�B�N������
		if (g_ParticleCnt < PARTICLE_HEATH_MAX && g_ParticleRateCnt >= PARTICLE_RATE_HEATH)
		{
			g_ParticleRateCnt = 0;
			g_ParticleCnt++;

			XMFLOAT3 pos;
			XMFLOAT3 scl;
			XMFLOAT3 move;
			float fAngle, fLength;
			int nLife;
			float fSize;
			float alpha;

			pos = g_ParticlePos;
			scl = XMFLOAT3(0.2f, 0.2f, 0.2f);


			fAngle = (float)(rand() % 628 - 314) / 100.0f;
			fLength = rand() % (int)(g_fWidthBase * 200) / 100.0f - g_fWidthBase;

			move.x = (float)((rand() % 6) / 100.0f + 0.02f);;
	

			move.y = rand() % 20 / 100.0f + 0.2f;


			move.z = cosf(fAngle) * fLength;
			move.z /= 30.0f;

			nLife = rand() % 100 + 70;

			fSize = (float)((rand() % 200) / 100.0f + 0.5f);

			alpha = (rand() % 50 + 50) / 100.0f;

			// �r���{�[�h�̐ݒ�
			SetParticle(pos, move, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), fSize, nLife, fAngle, PARTICLE_HEATH);
		}
	}


	#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_O))
	{
		// �p�[�e�B�N������
		//SetHitParticle(XMFLOAT3(0.0f, 15.0f, 0.0f), FALSE);
		g_CureParticles = TRUE;
	}
	if (GetKeyboardTrigger(DIK_I))
	{
		// �p�[�e�B�N������
		//SetHitParticle(XMFLOAT3(0.0f, 15.0f, 0.0f), TRUE);
		g_CureParticles = FALSE;
	}
	#endif // DEBUG

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawParticle(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���C�e�B���O�𖳌���
	SetLightEnable(FALSE);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ADD);

	// Z��r����
	SetDepthEnable(FALSE);

	// �t�H�O����
	SetFogEnable(FALSE);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(g_aParticle[nCntParticle].use)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_aParticle[nCntParticle].texNo]);

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			// ������������������
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_aParticle[nCntParticle].scale.x, g_aParticle[nCntParticle].scale.y, g_aParticle[nCntParticle].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_aParticle[nCntParticle].pos.x, g_aParticle[nCntParticle].pos.y, g_aParticle[nCntParticle].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_aParticle[nCntParticle].material);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r�L��
	SetDepthEnable(TRUE);

	// �t�H�O�L��
	SetFogEnable( GetFogEnable() );

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE / 2, PARTICLE_SIZE / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE / 2, PARTICLE_SIZE / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE / 2, -PARTICLE_SIZE / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE / 2, -PARTICLE_SIZE / 2, 0.0f);

		// �@���̐ݒ�
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// ���ˌ��̐ݒ�
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// �}�e���A���J���[�̐ݒ�
//=============================================================================
void SetColorParticle(int nIdxParticle, XMFLOAT4 col)
{
	g_aParticle[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float Size, int nLife, float angle, int type)
{
	int nIdxParticle = -1;

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(!g_aParticle[nCntParticle].use)
		{
			g_aParticle[nCntParticle].pos = pos;
			g_aParticle[nCntParticle].rot   = { 0.0f, 0.0f, 0.0f };
			g_aParticle[nCntParticle].scale = { Size, Size, Size };
			g_aParticle[nCntParticle].move = move;
			g_aParticle[nCntParticle].material.Diffuse = col;
			g_aParticle[nCntParticle].nLife = nLife;
			g_aParticle[nCntParticle].use = TRUE;
			g_aParticle[nCntParticle].angle = angle;

			g_aParticle[nCntParticle].type = type;
			g_aParticle[nCntParticle].texNo = type;

			nIdxParticle = nCntParticle;

			break;
		}
	}

	return nIdxParticle;
}

//=============================================================================
// �U���̃p�[�e�B�N���̔�������
//=============================================================================
void SetHitParticle(XMFLOAT3 position, BOOL crit)
{
	for (int i = 0; i < 6; i++)
	{
		XMFLOAT3 pos;
		XMFLOAT3 move;
		float fAngle, fLength;
		int nLife;
		float fSize;

		pos = position;
		pos.y = 10.0f;

		fAngle = (float)(rand() % 628 - 314) / 100.0f;
		fLength = rand() % (int)(100) / 100.0f + 0.3f;
		move.x = sinf(fAngle) * fLength;
		move.y = rand() % 20 / 10.0f /*+ g_fHeightBase*/;
		move.z = cosf(fAngle) * fLength;

		nLife = rand() % 30 + 50;

		fSize = (float)(rand() % 10 - 5);

		fSize = 1 + (fSize / 10);

		XMFLOAT4 color = crit ? XMFLOAT4(1.0f, 0.1f, 0.1f, 1.0f) : XMFLOAT4(0.8f, 0.7f, 0.2f, 1.0f);
		
		// �r���{�[�h�̐ݒ�
		SetParticle(pos, move, color, fSize, nLife, 0.0f, PARTICLE_HIT);
	}

}

void SetHitParticle(XMFLOAT3 position)
{
	g_ParticlePos	= position;
	g_CureParticles = TRUE;
}