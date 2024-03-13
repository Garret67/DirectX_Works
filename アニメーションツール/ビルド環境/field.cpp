//=============================================================================
//
// �t�B�[���h�\������ [field.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "field.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX		(1)						// �e�N�X�`���̐�

#define MAX_POLYGON		(6)						// �L���[�u�P������̖ʐ�

#define	VALUE_MOVE		(5.0f)					// �ړ���
#define	VALUE_ROTATE	(XM_PI * 0.02f)			// ��]��

#define	SIZE_WH			(1000.0f)				// �n�ʂ̃T�C�Y

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexField(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_���
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static FIELD						g_Field;				// �|���S���f�[�^
static int							g_TexNo;				// �e�N�X�`���ԍ�

static char* g_TextureName[] = {
	"data/TEXTURE/Grid2.png",
};

// ���_�z��
static VERTEX_3D g_VertexArray[4 * MAX_POLYGON] = {
	{XMFLOAT3(-SIZE_WH, 0.0f,  SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(  0.0f,   0.0f)},
	{XMFLOAT3( SIZE_WH, 0.0f,  SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(  0.0f, 151.0f)},
	{XMFLOAT3(-SIZE_WH, 0.0f, -SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(151.0f,   0.0f)},
	{XMFLOAT3( SIZE_WH, 0.0f, -SIZE_WH), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(151.0f, 151.0f)},

};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitField(void)
{
	// ���_�o�b�t�@�̍쐬
	MakeVertexField();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	g_Field.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Field.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitField(void)
{
	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// �e�N�X�`���̉��
	for (int i = 0; i < TEXTURE_MAX; i++)
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
void UpdateField(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawField(void)
{
	SetCullingMode(CULL_MODE_NONE);
	// ���e�X�g��L����
	SetAlphaTestEnable(TRUE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);


	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Field.scl.x, g_Field.scl.y, g_Field.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Field.rot.x, g_Field.rot.y, g_Field.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Field.pos.x, g_Field.pos.y, g_Field.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Field.mtxWorld, mtxWorld);


	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);		// 4���_����0�Ԗڂ̒��_�ԍ�����`��


	// ���e�X�g��L����
	SetAlphaTestEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(TRUE);

	SetCullingMode(CULL_MODE_BACK);
}


HRESULT MakeVertexField(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4 * MAX_POLYGON;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// �O���[�o���̒��_�z�񂩂璸�_�o�b�t�@������
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = g_VertexArray;

	GetDevice()->CreateBuffer(&bd, &sd, &g_VertexBuffer);

	return S_OK;
}