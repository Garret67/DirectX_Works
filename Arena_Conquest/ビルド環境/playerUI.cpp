//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "playerUI.h"
#include "player.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//#define TEXTURE_MAX					(4)		// �e�N�X�`���̐�
#define ENERGY_TEXTURE_DIVIDE_X		(9)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define ENERGY_TEXTURE_1_DIVISION	(1.0f / ENERGY_TEXTURE_DIVIDE_X)		// �A�j���p�^�[���̃e�N�X�`�����������iX)

#define ROUND_MSG_MAX				(10)

#define ROUND_MSG_SHOW_FRAMES		(120)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void UpdateRoundMsg(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[UI_TEX_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[UI_TEX_MAX] = {
	"data/TEXTURE/Player/HealthUI.png",
	"data/TEXTURE/Player/EnergyUI.png",

	"data/TEXTURE/Effect/RoundMsg/RoundBG3.png",
	"data/TEXTURE/Effect/RoundMsg/Round1.png",
	"data/TEXTURE/Effect/RoundMsg/Round2.png",
	"data/TEXTURE/Effect/RoundMsg/Round3.png",
	"data/TEXTURE/Effect/RoundMsg/Round4.png",
	"data/TEXTURE/Effect/RoundMsg/Round5.png",
	"data/TEXTURE/Effect/RoundMsg/Round6.png",
	"data/TEXTURE/Effect/RoundMsg/Round7.png",
	"data/TEXTURE/Effect/RoundMsg/Round8.png",
	"data/TEXTURE/Effect/RoundMsg/Round9.png",
	"data/TEXTURE/Effect/RoundMsg/Round10.png",
};



static HEALTH_UI		g_HealthUI;
static ENERGY_UI		g_EnergyUI;

static ROUND_MESSAGE	g_RoundMsgBG;
static ROUND_MESSAGE	g_RoundMsg[ROUND_MSG_MAX];

static BOOL				g_ShowRoundMsg;
static int				g_RoundMsgShowFramesCnt;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayerUI(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < UI_TEX_MAX; i++)
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


	PLAYER* player = GetPlayer();

	// HEALTH_UI�̏�����
	g_HealthUI.pos		= XMFLOAT3(150.0f, 40.0f, 0.0f);
	g_HealthUI.rot		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_HealthUI.scl		= XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_HealthUI.w		= 30.0f;
	g_HealthUI.h		= 30.0f;
	g_HealthUI.texNo	= UI_TEX_HEALTH;

	g_HealthUI.lifes	= player[0].lifes;
	g_HealthUI.lifesMax = player[0].lifesMax;
	g_HealthUI.active	= TRUE;

	// ENERGY_UI�̏�����
	g_EnergyUI.pos		 = XMFLOAT3(70.0f, 70.0f, 0.0f);
	g_EnergyUI.rot		 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_EnergyUI.scl		 = XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_EnergyUI.w		 = 110.0f;
	g_EnergyUI.h		 = 110.0f;
	g_EnergyUI.texNo	 = UI_TEX_ENERGY;
	
	g_EnergyUI.energy	 = player[0].energy;
	g_EnergyUI.energyMax = player[0].energyMax;
	g_EnergyUI.active	 = TRUE;


	g_RoundMsgBG.pos = XMFLOAT3(SCREEN_WIDTH + g_RoundMsgBG.w / 2, SCREEN_CENTER_Y - (SCREEN_CENTER_Y / 2), 0.0f);
	g_RoundMsgBG.w = 600.0f / 2;
	g_RoundMsgBG.h = 150.0f / 2;
	g_RoundMsgBG.texNo = UI_TEX_ROUND_BG;

	float firstRoundMsgPosY = SCREEN_CENTER_Y - (SCREEN_CENTER_Y / 2) - 25.0f + 2.5f;
	for (int i = 0; i < ROUND_MSG_MAX; i++)
	{
		g_RoundMsg[i].w = 600.0f / 2;
		g_RoundMsg[i].h = 100.0f / ROUND_MSG_MAX / 2;
		g_RoundMsg[i].pos = XMFLOAT3(-g_RoundMsg[i].w / 2 - (300 * i), firstRoundMsgPosY + (g_RoundMsg[i].h * i), 0.0f);
		g_RoundMsg[i].texNo = UI_TEX_ROUND_1;
	}

	g_ShowRoundMsg = FALSE;
	
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayerUI(void)
{
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < UI_TEX_MAX; i++)
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
void UpdatePlayerUI(void)
{
	UpdateRoundMsg();

#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayerUI(void)
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


	//Draw HP
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_HealthUI.texNo]);

		// ��������������
		for (int i = 0; i < g_HealthUI.lifesMax; i++)
		{

			// HP�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_HealthUI.pos.x + (40.0f * i);	// �X�R�A�̕\���ʒuX
			float py = g_HealthUI.pos.y;			// �X�R�A�̕\���ʒuY
			float pw = g_HealthUI.w;				// �X�R�A�̕\����
			float ph = g_HealthUI.h;				// �X�R�A�̕\������


			float tx = 0.0f;			// �e�N�X�`���̍���X���W
			float ty = 0.0f;			// �e�N�X�`���̍���Y���W
			float tw = 0.5f;		// �e�N�X�`���̕�
			float th = 1.0f;		// �e�N�X�`���̍���

			if (i >= g_HealthUI.lifes)
			{
				tx = 0.5f;
			}

			
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}


	//Draw Energy
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnergyUI.texNo]);

		// ��������������

		// HP�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_EnergyUI.pos.x;			// �X�R�A�̕\���ʒuX
		float py = g_EnergyUI.pos.y;			// �X�R�A�̕\���ʒuY
		float pw = g_EnergyUI.w;				// �X�R�A�̕\����
		float ph = g_EnergyUI.h;				// �X�R�A�̕\������

		float tx = ENERGY_TEXTURE_1_DIVISION * g_EnergyUI.energy;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;											// �e�N�X�`���̍���Y���W
		float tw = ENERGY_TEXTURE_1_DIVISION;						// �e�N�X�`���̕�
		float th = 1.0f;											// �e�N�X�`���̍���


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
		
	}

	if (!g_ShowRoundMsg)return;
	//ROUND BG �̕`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_RoundMsgBG.texNo]);

		//ROUND BG�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_RoundMsgBG.pos.x;		// �\���ʒuX
		float py = g_RoundMsgBG.pos.y;		// �\���ʒuY
		float pw = g_RoundMsgBG.w * 1.2f;	// �\����
		float ph = g_RoundMsgBG.h * 1.2f;	// �\������

		float tw = 1.0f;				// �e�N�X�`���̕�
		float th = 1.0f;				// �e�N�X�`���̍���
		float tx = 0.0f;				// �e�N�X�`���̍���X���W
		float ty = 0.0f;				// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//ROUND MSG �̕`��
	for (int i = 0; i < ROUND_MSG_MAX; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_RoundMsg[i].texNo]);

		//ROUND MSG�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_RoundMsg[i].pos.x;	// �\���ʒuX
		float py = g_RoundMsg[i].pos.y;	// �\���ʒuY
		float pw = g_RoundMsg[i].w;					// �\����
		float ph = g_RoundMsg[i].h;					// �\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f / ROUND_MSG_MAX;				// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = th * i;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	
}


//=============================================================================
// �X�R�A�����Z����
// ����:add :�ǉ�����_���B�}�C�i�X���\
//=============================================================================
void UpdateLifesUI(void)
{
	PLAYER* player = GetPlayer();
	g_HealthUI.lifes = player[0].lifes;
}

void UpdateEnergyUI(void)
{
	PLAYER* player = GetPlayer();
	g_EnergyUI.energy = player[0].energy;
}

void ShowRoundMsg(int round)
{
	g_ShowRoundMsg = TRUE;

	g_RoundMsgBG.pos = XMFLOAT3(SCREEN_WIDTH + g_RoundMsgBG.w / 2, SCREEN_CENTER_Y - (SCREEN_CENTER_Y / 2), 0.0f);

	float firstRoundMsgPosY = SCREEN_CENTER_Y - (SCREEN_CENTER_Y / 2) - 25.0f + 2.5f;

	for (int i = 0; i < ROUND_MSG_MAX; i++)
	{
		g_RoundMsg[i].pos = XMFLOAT3(-g_RoundMsg[i].w / 2 - (300 * i), firstRoundMsgPosY + (g_RoundMsg[i].h * i), 0.0f);
		g_RoundMsg[i].texNo = round + 2;
	}

}

void UpdateRoundMsg(void)
{
	if (!g_ShowRoundMsg)return;

	if (g_RoundMsgBG.pos.x > SCREEN_CENTER_X)
	{
		g_RoundMsgBG.pos.x -= 10.0f;
	}
	else if(g_RoundMsg[ROUND_MSG_MAX - 1].pos.x < SCREEN_CENTER_X)
	{
		for (int i = 0; i < ROUND_MSG_MAX; i++)
		{
			if (g_RoundMsg[i].pos.x < SCREEN_CENTER_X)
			{
				g_RoundMsg[i].pos.x += 50.0f;
			}
			
		}
	}
	else if (g_RoundMsgShowFramesCnt < ROUND_MSG_SHOW_FRAMES)
	{
		g_RoundMsgShowFramesCnt++;
	}
	else if (g_RoundMsgBG.pos.y > -g_RoundMsgBG.h)
	{
		g_RoundMsgBG.pos.y -= 5.0f;
		for (int i = 0; i < ROUND_MSG_MAX; i++)
		{
			g_RoundMsg[i].pos.y -= 5.0f;
		}
	}
	else 
	{
		g_RoundMsgShowFramesCnt = 0;
		g_ShowRoundMsg = FALSE;
	}
}


