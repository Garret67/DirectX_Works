//=============================================================================
//
// ���U���g��ʏ��� [screenGameOver.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "screenGameOver.h"
#include "input.h"
#include "score.h"
#include "fade.h"
#include "sound.h"
#include "spawnController.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
//#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
//#define TEXTURE_MAX					(5)				// �e�N�X�`���̐�

#define TEXTURE_BUTTON_WIDTH		(1000.0f / 3)	
#define TEXTURE_BUTTON_HEIGHT		(230.0f / 3)	

#define GAMEOVER_BUTTON_MAX			(3)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

void DrawScreenGameOverButtons(void);

void OnClickGOButton(int button);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[GAMEOVER_TEX_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[GAMEOVER_TEX_MAX] = {
	"data/TEXTURE/Menu/UIbgH.png",
	"data/TEXTURE/Menu/MenuButtons/GameOverTitle.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/GORetryRound.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/GORetryColiseum.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/GOBackToMenu.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/GORetryRoundMessage.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/GORetryColiseumMessage.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/GOBackToMenuMessage.png",
	"data/TEXTURE/Player/DeadGameOver.png",
};


static float			g_GameOverScreenAlpha;

static GAMEOVER_BUTTON	g_GameOverButton[GAMEOVER_BUTTON_MAX];

static int				g_SelectedButton;

BOOL					g_GameOverInput;

static BOOL				g_Load = FALSE;



//=============================================================================
// ����������
//=============================================================================
HRESULT InitScreenGameOver(void)
{
	switch (GetLanguage())
	{
	case LANG_JPN:
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/Japanese/GORetryRound.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/Japanese/GORetryColiseum.png";
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/Japanese/GOBackToMenu.png";
		g_TexturName[5] = "data/TEXTURE/Menu/MenuButtons/Japanese/GORetryRoundMessage.png";
		g_TexturName[6] = "data/TEXTURE/Menu/MenuButtons/Japanese/GORetryColiseumMessage.png";
		g_TexturName[7] = "data/TEXTURE/Menu/MenuButtons/Japanese/GOBackToMenuMessage.png";
		break;

	case LANG_USA:
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/English/GORetryRound.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/English/GORetryColiseum.png";
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/English/GOBackToMenu.png";
		g_TexturName[5] = "data/TEXTURE/Menu/MenuButtons/English/GORetryRoundMessage.png";
		g_TexturName[6] = "data/TEXTURE/Menu/MenuButtons/English/GORetryColiseumMessage.png";
		g_TexturName[7] = "data/TEXTURE/Menu/MenuButtons/English/GOBackToMenuMessage.png";
		break;

	case LANG_ESP:
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/Spanish/GORetryRound.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/Spanish/GORetryColiseum.png";
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/Spanish/GOBackToMenu.png";
		g_TexturName[5] = "data/TEXTURE/Menu/MenuButtons/Spanish/GORetryRoundMessage.png";
		g_TexturName[6] = "data/TEXTURE/Menu/MenuButtons/Spanish/GORetryColiseumMessage.png";
		g_TexturName[7] = "data/TEXTURE/Menu/MenuButtons/Spanish/GOBackToMenuMessage.png";
		break;
	}


	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < GAMEOVER_TEX_MAX; i++)
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


	g_GameOverScreenAlpha = 0.0f;

	g_SelectedButton = 1;


	for (int i = 0; i < GAMEOVER_BUTTON_MAX; i++)
	{
		g_GameOverButton[i].pos = XMFLOAT3(SCREEN_CENTER_X - 175.0f + (i * 175.0f), (SCREEN_CENTER_Y / 2) - 75.0f, 0.0f);
		g_GameOverButton[i].w = TEXTURE_BUTTON_WIDTH;
		g_GameOverButton[i].h = TEXTURE_BUTTON_HEIGHT;
		g_GameOverButton[i].texNo = GAMEOVER_TEX_RETRY_ROUND_BUTTON + i;
		g_GameOverButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	g_GameOverButton[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);


	g_GameOverInput = FALSE;


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitScreenGameOver(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < GAMEOVER_TEX_MAX; i++)
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
void UpdateScreenGameOver(void)
{

	if (g_GameOverScreenAlpha < 1.0f)
	{
		g_GameOverScreenAlpha += 0.005f;

		if (g_GameOverScreenAlpha >= 1.0f)
		{
			g_GameOverInput = TRUE;
		}
	}




	if (g_GameOverInput)
	{
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
			g_GameOverInput = FALSE;
			OnClickGOButton(g_SelectedButton);
		}


		if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_RIGHT) || GetKeyboardTrigger(DIK_D))
		{
			PlaySound(SOUND_LABEL_SE_ChangeButton);

			g_GameOverButton[g_SelectedButton].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_SelectedButton = (g_SelectedButton + 1) % GAMEOVER_BUTTON_MAX;
			g_GameOverButton[g_SelectedButton].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		else if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_LEFT) || GetKeyboardTrigger(DIK_A))
		{
			PlaySound(SOUND_LABEL_SE_ChangeButton);

			g_GameOverButton[g_SelectedButton].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_SelectedButton = (--g_SelectedButton < 0) ? (GAMEOVER_BUTTON_MAX - 1) : g_SelectedButton;
			g_GameOverButton[g_SelectedButton].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		}
	}
	


#ifdef _DEBUG	// �f�o�b�O����\������

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawScreenGameOver(void)
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

	// GAMEOVER�̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GAMEOVER_TEX_BG]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2), 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_GameOverScreenAlpha));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// GAMEOVER�̃^�C�g����`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GAMEOVER_TEX_GAMEOVER_TITLE]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y - (SCREEN_HEIGHT / 4), (1000.0f / 2), (100.0f / 2), 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_GameOverScreenAlpha));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// Player DEAD ��`��
	{
		//// �e�N�X�`���ݒ�
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GAMEOVER_TEX_DEAD_PLAYER]);

		//// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//SetSpriteColor(g_VertexBuffer, SCREEN_CENTER_X - 200.0f, SCREEN_CENTER_Y - 50.0f, 75.0f * 2, 50.0f * 2, 0.0f, 0.0f, 1.0f, 1.0f,
		//	XMFLOAT4(1.0f, 1.0f, 1.0f, g_GameOverScreenAlpha));

		//// �|���S���`��
		//GetDeviceContext()->Draw(4, 0);
	}

	// Button message ��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GAMEOVER_TEX_RETRY_ROUND_MESSAGE + g_SelectedButton]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2), 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_GameOverScreenAlpha));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	DrawScreenGameOverButtons();
}

void DrawScreenGameOverButtons(void)
{
	for (int i = 0; i < GAMEOVER_BUTTON_MAX; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_GameOverButton[i].texNo]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_GameOverButton[i].pos.x;			// �{�^���̕\���ʒuX
		float py = g_GameOverButton[i].pos.y + SCREEN_CENTER_Y;			// �{�^���̕\���ʒuY
		float pw = g_GameOverButton[i].w;		// �{�^���̕\����
		float ph = g_GameOverButton[i].h;		// �{�^���̕\������


		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(g_GameOverButton[i].color.x, g_GameOverButton[i].color.y, g_GameOverButton[i].color.z, g_GameOverScreenAlpha));
		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}


void SetScreenGameOver(void)
{
	SetFinalScreen(SCREEN_GAMEOVER);
}


void OnClickGOButton(int button)
{
	switch (button)
	{
	case BUTTON_RETRY_ROUND:
		SetRetryRound(GetRound());
		//SetFade(FADE_OUT, MODE_COLISEUM);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_COLISEUM);
		break;

	case BUTTON_RETRY_COLISEUM:
		SetRetryRound(0);
		//SetFade(FADE_OUT, MODE_COLISEUM);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_COLISEUM);
		break;

	case BUTTON_BACK_TO_MENU:
		//SetRetryRound(0);
		//SetFade(FADE_OUT, MODE_TITLE);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_MENU);
		break;
	}
}