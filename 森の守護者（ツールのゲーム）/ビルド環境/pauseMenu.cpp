//=============================================================================
//
// �^�C�g����ʏ��� [pauseMenu.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "pauseMenu.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "game.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define TEXTURE_MAX		(4)

#define OPTIONS_SPACE	(50.0f)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void OnClickButtonPauseMenu(int button);

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct MENU_SPRITE
{
	BOOL		use;		// TRUE:�g���Ă���  FALSE:���g�p
	float		w, h;		// ���ƍ���
	XMFLOAT3	pos;		// �|���S���̍��W
	int			texNo;		// �e�N�X�`���ԍ�
};


struct PAUSE_MENU
{
	MENU_SPRITE leftBG;
	MENU_SPRITE pauseMenuTitleBG;
	//MENU_SPRITE optionBG;
};

struct MENU_OPTION
{
	BOOL		use;		// TRUE:�g���Ă���  FALSE:���g�p
	float		w, h;		// ���ƍ���
	XMFLOAT3	pos;		// �|���S���̍��W
	char		text[256];
};


enum MENU_OPTIONS {
	OP_CONTINUE,
	OP_OPTIONS,
	OP_SAVE,
	OP_BACK_TO_TITLE,

	MAX_OPTIONS,
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = 
{
	"data/TEXTURE/Menu/fade_white.png",	
	"data/TEXTURE/Menu/UIbgH.png",	
	"data/TEXTURE/Menu/UIbgV.png",	
	"data/TEXTURE/Menu/OptionsBG.png",	

};

static PAUSE_MENU	g_PauseMenu;
static MENU_OPTION	g_MenuOptions[MAX_OPTIONS];
static int 			g_SelectedOption;

static BOOL			g_PauseMenuMoving = TRUE;
static XMFLOAT4		g_ScreenColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f );


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPauseMenu(void)
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
	g_PauseMenu.leftBG.use		= TRUE;
	g_PauseMenu.leftBG.w		= 300.0f;
	g_PauseMenu.leftBG.h		= SCREEN_HEIGHT;
	g_PauseMenu.leftBG.pos		= XMFLOAT3(g_PauseMenu.leftBG.w / 2, SCREEN_CENTER_Y, 0.0f);
	g_PauseMenu.leftBG.texNo	= 2;

	g_PauseMenu.pauseMenuTitleBG.use	= TRUE;
	g_PauseMenu.pauseMenuTitleBG.pos	= XMFLOAT3(0.0f, 75.0f, 0.0f);
	g_PauseMenu.pauseMenuTitleBG.w		= 250.0f;
	g_PauseMenu.pauseMenuTitleBG.h		= 50.0f;
	g_PauseMenu.pauseMenuTitleBG.texNo	= 1;

	for (int i = 0; i < MAX_OPTIONS; i++)
	{
		g_MenuOptions[i].use	= TRUE;
		g_MenuOptions[i].pos	= XMFLOAT3(0.0f, 150.f + OPTIONS_SPACE * i, 0.0f);
		g_MenuOptions[i].w		= g_PauseMenu.leftBG.w;
		g_MenuOptions[i].h		= 30.0f;
	}
	strcpy_s(g_MenuOptions[0].text, 256, "�ĊJ");
	strcpy_s(g_MenuOptions[1].text, 256, "����");
	strcpy_s(g_MenuOptions[2].text, 256, "�Z�[�u");
	strcpy_s(g_MenuOptions[3].text, 256, "�^�C�g���ɖ߂�");


	g_SelectedOption = OP_CONTINUE;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPauseMenu(void)
{

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

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePauseMenu(void)
{
	if (GetPauseMode() == PAUSE_MODE_MENU)
	{
		if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_JS_DOWN) || IsButtonTriggered(0, BUTTON_ARROW_DOWN))
		{
			g_SelectedOption = (g_SelectedOption + 1) % MAX_OPTIONS;		//�I�����Ă���{�^�����X�V
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}
		else if (GetKeyboardTrigger(DIK_UP) || GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_JS_UP) || IsButtonTriggered(0, BUTTON_ARROW_UP))
		{
			g_SelectedOption = g_SelectedOption ? --g_SelectedOption : MAX_OPTIONS - 1;	//�I�����Ă���{�^����
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}

		//�{�^���̓�����s��
		if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger( DIK_SPACE) || IsButtonTriggered(0, BUTTON_B) || IsButtonTriggered(0, BUTTON_R))
		{
			OnClickButtonPauseMenu(g_SelectedOption);
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
		}

		//�Q�[���ɖ߂�
		if (GetKeyboardTrigger(DIK_ESCAPE) || GetKeyboardTrigger(DIK_A))
		{
			PlaySound(SOUND_LABEL_SE_PulseButtonBack);
		}
	}


	if (g_PauseMenuMoving)
	{

		//���E�̓���
		{
			static float pointerMoveCnt;
			float pointerMoveMax = 60;

			float angle = (XM_2PI / pointerMoveMax) * pointerMoveCnt;
			float x = cosf(angle);

			g_PauseMenu.leftBG.pos.x = g_PauseMenu.leftBG.w/2 * -x;

			g_ScreenColor.w = (-x + 1) / 3;

			if (GetPauseMode() == PAUSE_MODE_MENU)
			{
				pointerMoveCnt++;
				if (pointerMoveCnt >= pointerMoveMax/2) g_PauseMenuMoving = FALSE;
			}
			else
			{
				pointerMoveCnt--;
				if (pointerMoveCnt <= 0) g_PauseMenuMoving = FALSE;

			}
		}
	}




#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPauseMenu(void)
{
	if (g_PauseMenuMoving || GetPauseMode() == PAUSE_MODE_MENU)
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


		//Black screen  
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, g_ScreenColor);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

		//*******************
		// leftBG��`��
		//*******************
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_PauseMenu.leftBG.texNo]);

		//�w�i�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_PauseMenu.leftBG.pos.x;	// �\���ʒuX
		float py = g_PauseMenu.leftBG.pos.y;	// �\���ʒuY
		float pw = g_PauseMenu.leftBG.w;		// �\����
		float ph = g_PauseMenu.leftBG.h;		// �\������

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th/*, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)*/);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		//*******************
		// pauseMenuTitleBG��`��
		//*******************
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_PauseMenu.pauseMenuTitleBG.texNo]);

		//�ʒu���W�𔽉f
		px = g_PauseMenu.pauseMenuTitleBG.pos.x;	// �\���ʒuX
		py = g_PauseMenu.pauseMenuTitleBG.pos.y;	// �\���ʒuY
		pw = g_PauseMenu.pauseMenuTitleBG.w;		// �\����
		ph = g_PauseMenu.pauseMenuTitleBG.h;		// �\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		px += g_PauseMenu.leftBG.pos.x;
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th/*, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)*/);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//SetFont(L"Showcard Gothic", 35.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
		//DrawStringRect(L"PAUSE", D2D1::RectF(px - pw / 2, py - ph / 2, px + pw / 2, py + ph / 2), D2D1_DRAW_TEXT_OPTIONS_NONE);
		DrawTextRect(FormatToString("PAUSE"), 35, XMFLOAT4(0, 0, 0, 1),
			px - pw / 2, py - ph / 2, px + pw / 2, py + ph / 2, TEXT_ANCHOR_CENTER_CENTER, "HG�n�p�p�߯�ߑ�");



		//*******************
		// optionBG��`��
		//*******************
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		//�ʒu���W�𔽉f
		px = g_MenuOptions[g_SelectedOption].pos.x;	// �\���ʒuX
		py = g_MenuOptions[g_SelectedOption].pos.y;	// �\���ʒuY
		pw = g_MenuOptions[g_SelectedOption].w;		// �\����
		ph = g_MenuOptions[g_SelectedOption].h;		// �\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		px += g_PauseMenu.leftBG.pos.x;
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		for (int i = 0; i < MAX_OPTIONS; i++)
		{
			DrawTextRect(FormatToString(g_MenuOptions[i].text), 20, XMFLOAT4(0, 0, 0, 1),
				px - pw, g_MenuOptions[i].pos.y - ph, px + pw, g_MenuOptions[i].pos.y + ph, TEXT_ANCHOR_CENTER_CENTER, "HG�n�p�p�߯�ߑ�");

		}


	}

}




//=============================================================================
// OnClick Button ��`��
//=============================================================================
void OnClickButtonPauseMenu(int button)
{
	switch (button)
	{
	case OP_CONTINUE:
		SetPauseMode(PAUSE_MODE_OFF);
		break;

	case OP_OPTIONS:

		break;

	case OP_SAVE:

		break;


	case OP_BACK_TO_TITLE:
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_TITLE);
		break;

	default:
		break;
	}
}




void MoveMenu(void)
{
	g_PauseMenuMoving = TRUE;
}


