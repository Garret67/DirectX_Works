//=============================================================================
//
// BackGround���� [bg.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "bg.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH * 5)	// �w�i�� �@(TUTORIAL_2)
#define TEXTURE_HEIGHT				(1600)				// �w�i���� (TUTORIAL_2)
#define TEXTURE_MAX					(7)					// �e�N�X�`���̐�

#define BG_SCROLL_SPEED				(0.1f)				//Background�̈ړ���


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Stage/BG/Background.png",
	"data/TEXTURE/Stage/BG/background_layer_1_Large.png",
	"data/TEXTURE/Stage/BG/background_layer_2_Large.png",
	"data/TEXTURE/Stage/BG/background_layer_3_Large.png",

	"data/TEXTURE/Stage/Assets/Exit_anim.png",

	"data/TEXTURE/Stage/BG/ArenaBGSky.png",
	"data/TEXTURE/Stage/BG/ArenaBG.png",
};


static BG	g_BG;							//�V�[���̑傫��
static BG	g_BGmove[BACKGROUNDS_MAX];		//�ړ�����BackGround

static BOOL	g_Load = FALSE;					// ���������s�������̃t���O


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBG(void)
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


	// �ϐ��̏�����
	switch (GetMode())
	{
	case MODE_TUTORIAL_1:
		
		//Background
		g_BG.w		= SCREEN_WIDTH;
		g_BG.h		= SCREEN_HEIGHT;
		g_BG.pos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_BG.texNo	= 0;

		g_BG.scrl	= 0.0f;
		break;

	case MODE_TUTORIAL_2:
		
		//�V�[���̑傫��
		g_BG.w		= TEXTURE_WIDTH;
		g_BG.h		= TEXTURE_HEIGHT;
		g_BG.pos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_BG.texNo	= 0;
		g_BG.scrl	= 0.0f;
		
		//�\������BackGround
		for (int i = 0; i < BACKGROUNDS_MAX; i++)
		{
			g_BGmove[i].w		= TEXTURE_WIDTH;
			g_BGmove[i].h		= TEXTURE_HEIGHT;
			g_BGmove[i].pos		= XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_BGmove[i].texNo	= 1 + i;
			g_BGmove[i].scrl	= BG_SCROLL_SPEED + (BG_SCROLL_SPEED * i);
		}
		break;

	case MODE_COLISEUM:

		//�V�[���̑傫��
		g_BG.w		= SCREEN_WIDTH + SCREEN_WIDTH / 2;
		g_BG.h		= SCREEN_HEIGHT;
		g_BG.pos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_BG.texNo	= 0;
		g_BG.scrl	= 0.0f;

		//�V�[���̑傫��
		for (int i = 0; i < BACKGROUNDS_MAX; i++)
		{
			g_BGmove[i].w		= g_BG.w;
			g_BGmove[i].h		= g_BG.h;
			g_BGmove[i].pos		= XMFLOAT3(0.0f, g_BG.h - g_BGmove[i].h, 0.0f);
			g_BGmove[i].texNo	= 4 + i;
			g_BGmove[i].scrl	= BG_SCROLL_SPEED + (BG_SCROLL_SPEED * i);
		}

		break;
	}
	


	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// �I������
//=============================================================================
void UninitBG(void)
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
void UpdateBG(void)
{


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBG(void)
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


	{//BG
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_BG.texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer,
			g_BG.pos.x, g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

		
	{// �w�i��`��
		for (int i = 0; i < BACKGROUNDS_MAX; i++)
		{
			if (GetMode() == MODE_TUTORIAL_1) break;

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_BGmove[i].texNo]);

			float px = g_BGmove[i].pos.x - (g_BG.pos.x * g_BGmove[i].scrl);	// �G�l�~�[�̕\���ʒuX
			float py = g_BGmove[i].pos.y - g_BG.pos.y;						// �G�l�~�[�̕\���ʒuY
			float pw = g_BGmove[i].w;										// �G�l�~�[�̕\����
			float ph = g_BGmove[i].h;										// �G�l�~�[�̕\������

			//PLATFORM�̃e�N�X�`���[���W�𔽉f
			float tx = 0.0f;					// �e�N�X�`���̍���X���W
			float ty = 0.0f;					// �e�N�X�`���̍���Y���W
			float tw = 1.3f;					// �e�N�X�`���̕�
			float th = 1.0f;					// �e�N�X�`���̍���

			if (GetMode() == MODE_TUTORIAL_2) tw = pw / SCREEN_WIDTH;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}		
	}
}


//=============================================================================
// �V�[���̑傫���\���̂̐擪�A�h���X���擾
//=============================================================================
BG* GetBG(void)
{
	return &g_BG;
}


//=============================================================================
// BG�\���̂̐擪�A�h���X���擾
//=============================================================================
BG* GetBGmove(void)
{
	return &g_BGmove[0];
}
