//=============================================================================
//
// �؏��� [tree.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "modelBinaryFBX.h"
#include "gameEnvironment.h"
#include "modelBinaryFBX.h"
#include "player.h"
#include "enemy.h"



//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX				(2)				// �e�N�X�`���̐�

#define	TREE_WIDTH				(50.0f)			// ���_�T�C�Y
#define	TREE_HEIGHT				(80.0f)			// ���_�T�C�Y

#define	MAX_GROUND				(4)				// ���ő吔

#define	MODEL_FIGHT_ENVIRONMENT	"data\\MODEL\\Environment\\Fight_Environment_Model.bin"

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct ENVIRONMENT_MODEL
{
	XMFLOAT4X4	mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;				// ���f���̈ʒu
	XMFLOAT3	rot;				// ���f���̌���(��])
	XMFLOAT3	scl;				// ���f���̑傫��(�X�P�[��)

	BOOL		use;
	BOOL		load;
	DX11_MODEL	model;		// ���f�����
	MODEL_DATA	modelData;	// ���f�����

	//FBX_ANIMATOR		animator;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexTree(void);
HRESULT MakeVertexGround(void);

void SetTree(XMFLOAT3 pos, float fWidth, float fHeight);
void SetGround(XMFLOAT3 pos, float fWidth, float fHeight);

void TreeRelocation(BOOL Xmov, BOOL positiveMov, float limitPos);

void SetGameTree(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_TreeVertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11Buffer					*g_GroundVertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static BILLBOARD			g_Tree[MAX_TREE];	// �؃��[�N
static BOOL					g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static BILLBOARD			g_Ground[MAX_GROUND];	// �؃��[�N
static ENVIRONMENT_MODEL	g_FightEnvironment;

static int					g_TexNo;			// �e�N�X�`���ԍ�
static int					g_TexAnim;			// �e�N�X�`���A�j���p

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/Environment/tree1.png",
	"data/TEXTURE/Environment/GroundGame.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGameEnvironment(void)
{
	MakeVertexTree();
	MakeVertexGround();

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

	g_TexNo = g_TexAnim = 0;

	// �؃��[�N�̏�����
	for (int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		ZeroMemory(&g_Tree[nCntTree].material, sizeof(g_Tree[nCntTree].material));
		g_Tree[nCntTree].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Tree[nCntTree].pos		= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Tree[nCntTree].fWidth		= TREE_WIDTH;
		g_Tree[nCntTree].fHeight	= TREE_HEIGHT;
		g_Tree[nCntTree].use		= FALSE;
	}

	// �����[�N�̏�����
	for (int i = 0; i < MAX_GROUND; i++)
	{
		ZeroMemory(&g_Ground[i].material, sizeof(g_Ground[i].material));
		g_Ground[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Ground[i].fWidth	= GROUND_WIDTH;
		g_Ground[i].fHeight = GROUND_HEIGHT;
		g_Ground[i].pos		= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ground[i].rot		= XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Ground[i].use = FALSE;
	}


	switch (GetMode())
	{
	case MODE_GAME:
		// �؂̐ݒ�
		SetGameTree();

		SetGround(XMFLOAT3(0.0f,		 0.0f, 0.0f),			GROUND_WIDTH, GROUND_WIDTH);
		SetGround(XMFLOAT3(0.0f,		 0.0f, -GROUND_WIDTH),	GROUND_WIDTH, GROUND_WIDTH);
		SetGround(XMFLOAT3(GROUND_WIDTH, 0.0f, 0.0f),			GROUND_WIDTH, GROUND_WIDTH);
		SetGround(XMFLOAT3(GROUND_WIDTH, 0.0f, -GROUND_WIDTH),	GROUND_WIDTH, GROUND_WIDTH);

		break;

	case MODE_FIGHT:

		InitModelBinaryFBX(MODEL_FIGHT_ENVIRONMENT, &g_FightEnvironment.model, &g_FightEnvironment.modelData);
		g_FightEnvironment.load = TRUE;

		g_FightEnvironment.pos = XMFLOAT3(0.0f, 0.0f, 20.0f);
		g_FightEnvironment.rot = XMFLOAT3(0.0f, -XM_PIDIV2, 0.0f);
		g_FightEnvironment.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_FightEnvironment.use = TRUE;			// TRUE:�����Ă�

		// �؂̐ݒ�
		SetTree(XMFLOAT3(-143.0f, 0.0f,-159.0f), 100.0f, 100.0f);
		SetTree(XMFLOAT3(-216.0f, 0.0f, -81.0f), 100.0f, 100.0f);
		SetTree(XMFLOAT3(-151.0f, 0.0f, -28.0f), 100.0f, 100.0f);
		SetTree(XMFLOAT3(-177.0f, 0.0f,  62.0f), 100.0f, 100.0f);
		SetTree(XMFLOAT3(-201.0f, 0.0f, 151.0f), 100.0f, 100.0f);
		SetTree(XMFLOAT3(-110.0f, 0.0f, 158.0f), 100.0f, 100.0f);
		SetTree(XMFLOAT3(-128.0f, 0.0f, 250.0f), 100.0f, 100.0f);
		SetTree(XMFLOAT3( -52.0f, 0.0f, 256.0f), 100.0f, 100.0f);

		break;

	default:
		break;
	}




	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGameEnvironment(void)
{
	for(int i = 0; i < TEXTURE_MAX; i++)
	{
		if(g_Texture[i] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	if(g_TreeVertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_TreeVertexBuffer->Release();
		g_TreeVertexBuffer = NULL;
	}

	if(g_GroundVertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_GroundVertexBuffer->Release();
		g_GroundVertexBuffer = NULL;
	}

	switch (GetMode())
	{
	case MODE_GAME:

		break;

	case MODE_FIGHT:
		if (g_FightEnvironment.load == TRUE)
		{
			UninitModelBinaryFBX(&g_FightEnvironment.model, &g_FightEnvironment.modelData);
			g_FightEnvironment.load = FALSE;
		}
		break;

	default:
		break;
	}


}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGameEnvironment(void)
{
	if (GetMode() == MODE_GAME)
	{
		PLAYER* player = GetPlayer();

		for (int i = 0; i < MAX_GROUND; i++)
		{
			float playerDistanceX = g_Ground[i].pos.x - player[0].pos.x;
			float playerDistanceZ = g_Ground[i].pos.z - player[0].pos.z;

			if (playerDistanceX < -GROUND_WIDTH)//�E�ɓ���
			{
				TreeRelocation (TRUE, TRUE, g_Ground[i].pos.x + (GROUND_WIDTH * 0.5f));
				EnemyRelocation(TRUE, TRUE, g_Ground[i].pos.x + (GROUND_WIDTH * 0.5f));

				g_Ground[i].pos.x += GROUND_WIDTH * 2;
			}
			else if (playerDistanceX > GROUND_WIDTH)//���ɓ���
			{
				TreeRelocation (TRUE, FALSE, g_Ground[i].pos.x - (GROUND_WIDTH * 0.5f));
				EnemyRelocation(TRUE, FALSE, g_Ground[i].pos.x - (GROUND_WIDTH * 0.5f));

				g_Ground[i].pos.x -= GROUND_WIDTH * 2;
			}

			if (playerDistanceZ < -GROUND_HEIGHT)//��ɓ���
			{
				TreeRelocation (FALSE, TRUE, g_Ground[i].pos.z + (GROUND_HEIGHT * 0.5f));
				EnemyRelocation(FALSE, TRUE, g_Ground[i].pos.z + (GROUND_HEIGHT * 0.5f));

				g_Ground[i].pos.z += GROUND_HEIGHT * 2;
			}
			else if (playerDistanceZ > GROUND_HEIGHT)//���ɓ���
			{
				TreeRelocation (FALSE, FALSE, g_Ground[i].pos.z - (GROUND_HEIGHT * 0.5f));
				EnemyRelocation(FALSE, FALSE, g_Ground[i].pos.z - (GROUND_HEIGHT * 0.5f));

				g_Ground[i].pos.z -= GROUND_WIDTH * 2;
			}
		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGameEnvironment(void)
{
	// ���e�X�g�ݒ�
	if (g_bAlpaTest == TRUE)
	{
		// ���e�X�g��L����
		SetAlphaTestEnable(TRUE);
	}

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_TreeVertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_TREE; i++)
	{
		if (g_Tree[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// �֐��g������
			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;


			// �����s��i�����s��j��]�u�s�񂳂��ċt�s�������Ă��(����)
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
			mtxScl = XMMatrixScaling(g_Tree[i].fWidth, g_Tree[i].fHeight, g_Tree[i].fWidth);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Tree[i].pos.x, g_Tree[i].pos.y, g_Tree[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Tree[i].material);

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	switch (GetMode())
	{
	case MODE_GAME:

		// ���_�o�b�t�@�ݒ�
		stride = sizeof(VERTEX_3D);
		offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_GroundVertexBuffer, &stride, &offset);

		for (int i = 0; i < MAX_GROUND; i++)
		{
			if (g_Ground[i].use)
			{
				// ���[���h�}�g���b�N�X�̏�����
				mtxWorld = XMMatrixIdentity();

				// �X�P�[���𔽉f
				mtxScl = XMMatrixScaling(g_Ground[i].fWidth, g_Ground[i].fHeight, g_Ground[i].fWidth);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// ��]�𔽉f
				mtxRot = XMMatrixRotationRollPitchYaw(g_Ground[i].rot.x, g_Ground[i].rot.y, g_Ground[i].rot.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

				// �ړ��𔽉f
				mtxTranslate = XMMatrixTranslation(g_Ground[i].pos.x, g_Ground[i].pos.y, g_Ground[i].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				// ���[���h�}�g���b�N�X�̐ݒ�
				SetWorldMatrix(&mtxWorld);


				// �}�e���A���ݒ�
				SetMaterial(g_Ground[i].material);

				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

				// �|���S���̕`��
				GetDeviceContext()->Draw(4, 0);
			}
		}

		break;

	case MODE_FIGHT:
		//*************
		// MODEL_FIGHT_ENVIRONMENT�̕`��
		//*************

		// �J�����O����
		SetCullingMode(CULL_MODE_BACK);

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_FightEnvironment.scl.x, g_FightEnvironment.scl.y, g_FightEnvironment.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_FightEnvironment.rot.x, g_FightEnvironment.rot.y + XM_PI, g_FightEnvironment.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_FightEnvironment.pos.x, g_FightEnvironment.pos.y, g_FightEnvironment.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_FightEnvironment.mtxWorld, mtxWorld);


		// ���f���`��
		//DrawModel(&g_FightEnvironment.model);
		DrawModelBinaryFBX(&g_FightEnvironment.model, g_FightEnvironment.modelData, mtxWorld);


		break;

	default:
		break;
	}


	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// �؂̒��_���̍쐬
//=============================================================================
HRESULT MakeVertexTree(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_TreeVertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_TreeVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-1.0f / 2.0f, 1.0f, 0.0f);
	vertex[1].Position = XMFLOAT3(1.0f / 2.0f, 1.0f, 0.0f);
	vertex[2].Position = XMFLOAT3(-1.0f / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(1.0f / 2.0f, 0.0f, 0.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_TreeVertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �O���E���h�̒��_���̍쐬
//=============================================================================
HRESULT MakeVertexGround(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_GroundVertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_GroundVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-1.0f / 2.0f, 0.0f,  1.0f / 2.0f);
	vertex[1].Position = XMFLOAT3( 1.0f / 2.0f, 0.0f,  1.0f / 2.0f);
	vertex[2].Position = XMFLOAT3(-1.0f / 2.0f, 0.0f, -1.0f / 2.0f);
	vertex[3].Position = XMFLOAT3( 1.0f / 2.0f, 0.0f, -1.0f / 2.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_GroundVertexBuffer, 0);

	return S_OK;
}


//=============================================================================
// �؂̃p�����[�^���Z�b�g
//=============================================================================
void SetTree(XMFLOAT3 pos, float fWidth, float fHeight)
{
	for(int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		if(!g_Tree[nCntTree].use)
		{
			g_Tree[nCntTree].pos = pos;
			g_Tree[nCntTree].fWidth = fWidth;
			g_Tree[nCntTree].fHeight = fHeight;
			g_Tree[nCntTree].use = TRUE;

			break;
		}
	}
}


//=============================================================================
// �O���E���h�̃p�����[�^���Z�b�g
//=============================================================================
void SetGround(XMFLOAT3 pos, float fWidth, float fHeight)
{

	for (int i = 0; i < MAX_TREE; i++)
	{
		if (!g_Ground[i].use)
		{
			g_Ground[i].pos = pos;
			g_Ground[i].fWidth = fWidth;
			g_Ground[i].fHeight = fHeight;
			g_Ground[i].use = TRUE;

			break;
		}
	}
}

//=============================================================================
// �؂̍Ĕz�u
//=============================================================================
void TreeRelocation(BOOL Xmov, BOOL positiveMov, float limitPos)
{
	if (Xmov)
	{
		if (positiveMov)
		{
			for (int i = 0; i < MAX_TREE; i++)
			{
				if (g_Tree[i].pos.x < limitPos)
				{
					g_Tree[i].pos.x += GROUND_WIDTH * 2;
				}
			}
		}
		else
		{
			for (int i = 0; i < MAX_TREE; i++)
			{
				if (g_Tree[i].pos.x > limitPos)
				{
					g_Tree[i].pos.x -= GROUND_WIDTH * 2;
				}
			}
		}

	}
	else
	{
		if (positiveMov)
		{
			for (int i = 0; i < MAX_TREE; i++)
			{
				if (g_Tree[i].pos.z < limitPos)
				{
					g_Tree[i].pos.z += GROUND_HEIGHT * 2;
				}
			}
		}
		else
		{
			for (int i = 0; i < MAX_TREE; i++)
			{
				if (g_Tree[i].pos.z > limitPos)
				{
					g_Tree[i].pos.z -= GROUND_HEIGHT * 2;
				}
			}
		}

	}
}

BILLBOARD* GetTree(void)
{
	return &g_Tree[0];
}

// GAME MODE�̖؂̐ݒ�
void SetGameTree(void)
{
	//CENTER
	SetTree(XMFLOAT3( 207.0f, 0.0f,  221.0f), 100.0f, 100.0f); //1
	SetTree(XMFLOAT3( 198.0f, 0.0f,  465.0f), 100.0f, 100.0f); //2
	SetTree(XMFLOAT3( 474.0f, 0.0f,  221.0f), 100.0f, 100.0f); //3
	SetTree(XMFLOAT3( 475.0f, 0.0f,  473.0f), 100.0f, 100.0f); //4
	SetTree(XMFLOAT3(-449.0f, 0.0f, -136.0f), 100.0f, 100.0f); //5
	SetTree(XMFLOAT3(-207.0f, 0.0f, -132.0f), 100.0f, 100.0f); //6
	SetTree(XMFLOAT3(-110.0f, 0.0f,  465.0f), 100.0f, 100.0f); //7
	SetTree(XMFLOAT3(-101.0f, 0.0f,  157.0f), 100.0f, 100.0f); //8
	SetTree(XMFLOAT3(-340.0f, 0.0f,  487.0f), 100.0f, 100.0f); //9
	SetTree(XMFLOAT3(-354.0f, 0.0f,  179.0f), 100.0f, 100.0f); //10
	SetTree(XMFLOAT3( 193.0f, 0.0f,  -34.0f), 100.0f, 100.0f); //11
	SetTree(XMFLOAT3( 488.0f, 0.0f,    3.0f), 100.0f, 100.0f); //12
	SetTree(XMFLOAT3( 211.0f, 0.0f, -242.0f), 100.0f, 100.0f); //13
	SetTree(XMFLOAT3( 468.0f, 0.0f, -328.0f), 100.0f, 100.0f); //14
	SetTree(XMFLOAT3(  70.0f, 0.0f, -370.0f), 100.0f, 100.0f); //15
	SetTree(XMFLOAT3(-294.0f, 0.0f,   52.0f), 100.0f, 100.0f); //16
	SetTree(XMFLOAT3(-213.0f, 0.0f, -375.0f), 100.0f, 100.0f); //17
	SetTree(XMFLOAT3(-385.0f, 0.0f, -322.0f), 100.0f, 100.0f); //18
	SetTree(XMFLOAT3(  -8.0f, 0.0f, -184.0f), 100.0f, 100.0f); //19

	//DOWN
	SetTree(XMFLOAT3( 207.0f, 0.0f,  221.0f - GROUND_WIDTH), 100.0f, 100.0f); //1
	SetTree(XMFLOAT3( 198.0f, 0.0f,  465.0f - GROUND_WIDTH), 100.0f, 100.0f); //2
	SetTree(XMFLOAT3( 474.0f, 0.0f,  221.0f - GROUND_WIDTH), 100.0f, 100.0f); //3
	SetTree(XMFLOAT3( 475.0f, 0.0f,  473.0f - GROUND_WIDTH), 100.0f, 100.0f); //4
	SetTree(XMFLOAT3(-449.0f, 0.0f, -136.0f - GROUND_WIDTH), 100.0f, 100.0f); //5
	SetTree(XMFLOAT3(-207.0f, 0.0f, -132.0f - GROUND_WIDTH), 100.0f, 100.0f); //6
	SetTree(XMFLOAT3(-110.0f, 0.0f,  465.0f - GROUND_WIDTH), 100.0f, 100.0f); //7
	SetTree(XMFLOAT3(-101.0f, 0.0f,  157.0f - GROUND_WIDTH), 100.0f, 100.0f); //8
	SetTree(XMFLOAT3(-340.0f, 0.0f,  487.0f - GROUND_WIDTH), 100.0f, 100.0f); //9
	SetTree(XMFLOAT3(-354.0f, 0.0f,  179.0f - GROUND_WIDTH), 100.0f, 100.0f); //10
	SetTree(XMFLOAT3( 193.0f, 0.0f,  -34.0f - GROUND_WIDTH), 100.0f, 100.0f); //11
	SetTree(XMFLOAT3( 488.0f, 0.0f,    3.0f - GROUND_WIDTH), 100.0f, 100.0f); //12
	SetTree(XMFLOAT3( 211.0f, 0.0f, -242.0f - GROUND_WIDTH), 100.0f, 100.0f); //13
	SetTree(XMFLOAT3( 468.0f, 0.0f, -328.0f - GROUND_WIDTH), 100.0f, 100.0f); //14
	SetTree(XMFLOAT3(  70.0f, 0.0f, -370.0f - GROUND_WIDTH), 100.0f, 100.0f); //15
	SetTree(XMFLOAT3(-294.0f, 0.0f,   52.0f - GROUND_WIDTH), 100.0f, 100.0f); //16
	SetTree(XMFLOAT3(-213.0f, 0.0f, -375.0f - GROUND_WIDTH), 100.0f, 100.0f); //17
	SetTree(XMFLOAT3(-385.0f, 0.0f, -322.0f - GROUND_WIDTH), 100.0f, 100.0f); //18
	SetTree(XMFLOAT3(  -8.0f, 0.0f, -184.0f - GROUND_WIDTH), 100.0f, 100.0f); //19

	//RIGHT
	SetTree(XMFLOAT3(GROUND_WIDTH + 207.0f, 0.0f,  221.0f), 100.0f, 100.0f); //1
	SetTree(XMFLOAT3(GROUND_WIDTH + 198.0f, 0.0f,  465.0f), 100.0f, 100.0f); //2
	SetTree(XMFLOAT3(GROUND_WIDTH + 474.0f, 0.0f,  221.0f), 100.0f, 100.0f); //3
	SetTree(XMFLOAT3(GROUND_WIDTH + 475.0f, 0.0f,  473.0f), 100.0f, 100.0f); //4
	SetTree(XMFLOAT3(GROUND_WIDTH - 449.0f, 0.0f, -136.0f), 100.0f, 100.0f); //5
	SetTree(XMFLOAT3(GROUND_WIDTH - 207.0f, 0.0f, -132.0f), 100.0f, 100.0f); //6
	SetTree(XMFLOAT3(GROUND_WIDTH - 110.0f, 0.0f,  465.0f), 100.0f, 100.0f); //7
	SetTree(XMFLOAT3(GROUND_WIDTH - 101.0f, 0.0f,  157.0f), 100.0f, 100.0f); //8
	SetTree(XMFLOAT3(GROUND_WIDTH - 340.0f, 0.0f,  487.0f), 100.0f, 100.0f); //9
	SetTree(XMFLOAT3(GROUND_WIDTH - 354.0f, 0.0f,  179.0f), 100.0f, 100.0f); //10
	SetTree(XMFLOAT3(GROUND_WIDTH + 193.0f, 0.0f,  -34.0f), 100.0f, 100.0f); //11
	SetTree(XMFLOAT3(GROUND_WIDTH + 488.0f, 0.0f,    3.0f), 100.0f, 100.0f); //12
	SetTree(XMFLOAT3(GROUND_WIDTH + 211.0f, 0.0f, -242.0f), 100.0f, 100.0f); //13
	SetTree(XMFLOAT3(GROUND_WIDTH + 468.0f, 0.0f, -328.0f), 100.0f, 100.0f); //14
	SetTree(XMFLOAT3(GROUND_WIDTH +  70.0f, 0.0f, -370.0f), 100.0f, 100.0f); //15
	SetTree(XMFLOAT3(GROUND_WIDTH - 294.0f, 0.0f,   52.0f), 100.0f, 100.0f); //16
	SetTree(XMFLOAT3(GROUND_WIDTH - 213.0f, 0.0f, -375.0f), 100.0f, 100.0f); //17
	SetTree(XMFLOAT3(GROUND_WIDTH - 385.0f, 0.0f, -322.0f), 100.0f, 100.0f); //18
	SetTree(XMFLOAT3(GROUND_WIDTH -   8.0f, 0.0f, -184.0f), 100.0f, 100.0f); //19

	//DOWN RIGHT
	SetTree(XMFLOAT3(GROUND_WIDTH + 207.0f, 0.0f,  221.0f - GROUND_WIDTH), 100.0f, 100.0f); //1
	SetTree(XMFLOAT3(GROUND_WIDTH + 198.0f, 0.0f,  465.0f - GROUND_WIDTH), 100.0f, 100.0f); //2
	SetTree(XMFLOAT3(GROUND_WIDTH + 474.0f, 0.0f,  221.0f - GROUND_WIDTH), 100.0f, 100.0f); //3
	SetTree(XMFLOAT3(GROUND_WIDTH + 475.0f, 0.0f,  473.0f - GROUND_WIDTH), 100.0f, 100.0f); //4
	SetTree(XMFLOAT3(GROUND_WIDTH - 449.0f, 0.0f, -136.0f - GROUND_WIDTH), 100.0f, 100.0f); //5
	SetTree(XMFLOAT3(GROUND_WIDTH - 207.0f, 0.0f, -132.0f - GROUND_WIDTH), 100.0f, 100.0f); //6
	SetTree(XMFLOAT3(GROUND_WIDTH - 110.0f, 0.0f,  465.0f - GROUND_WIDTH), 100.0f, 100.0f); //7
	SetTree(XMFLOAT3(GROUND_WIDTH - 101.0f, 0.0f,  157.0f - GROUND_WIDTH), 100.0f, 100.0f); //8
	SetTree(XMFLOAT3(GROUND_WIDTH - 340.0f, 0.0f,  487.0f - GROUND_WIDTH), 100.0f, 100.0f); //9
	SetTree(XMFLOAT3(GROUND_WIDTH - 354.0f, 0.0f,  179.0f - GROUND_WIDTH), 100.0f, 100.0f); //10
	SetTree(XMFLOAT3(GROUND_WIDTH + 193.0f, 0.0f,  -34.0f - GROUND_WIDTH), 100.0f, 100.0f); //11
	SetTree(XMFLOAT3(GROUND_WIDTH + 488.0f, 0.0f,    3.0f - GROUND_WIDTH), 100.0f, 100.0f); //12
	SetTree(XMFLOAT3(GROUND_WIDTH + 211.0f, 0.0f, -242.0f - GROUND_WIDTH), 100.0f, 100.0f); //13
	SetTree(XMFLOAT3(GROUND_WIDTH + 468.0f, 0.0f, -328.0f - GROUND_WIDTH), 100.0f, 100.0f); //14
	SetTree(XMFLOAT3(GROUND_WIDTH +  70.0f, 0.0f, -370.0f - GROUND_WIDTH), 100.0f, 100.0f); //15
	SetTree(XMFLOAT3(GROUND_WIDTH - 294.0f, 0.0f,   52.0f - GROUND_WIDTH), 100.0f, 100.0f); //16
	SetTree(XMFLOAT3(GROUND_WIDTH - 213.0f, 0.0f, -375.0f - GROUND_WIDTH), 100.0f, 100.0f); //17
	SetTree(XMFLOAT3(GROUND_WIDTH - 385.0f, 0.0f, -322.0f - GROUND_WIDTH), 100.0f, 100.0f); //18
	SetTree(XMFLOAT3(GROUND_WIDTH -   8.0f, 0.0f, -184.0f - GROUND_WIDTH), 100.0f, 100.0f); //19

}