//=============================================================================
//
// �^�C�g����ʏ��� [pauseMenu.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "pauseMenu.h"
#include "options.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "spawnController.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH / 2)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT / 2)	// 
#define TEXTURE_MAX					(11)				// �e�N�X�`���̐�

#define TEXTURE_BUTTON_WIDTH		(1000.0f / 3)		// ���S�T�C�Y
#define TEXTURE_BUTTON_HEIGHT		(114.0f / 3)		// 

#define TEXTURE_NUM_WIDTH			(15.0f)	
#define TEXTURE_NUM_HEIGHT			(30.0f)



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DrawMenuButtonPauseMenu(void);
void OnClickButtonPauseMenu(int button);

void UpdateDebugMenu(void);
void DrawDebugMenuButton(void);
void DrawDebugOptionsNumbers(void);

void OnClickButtonPauseMenu(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = 
{
	"data/TEXTURE/Menu/UIbgH.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Resume.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Options.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/ReturnToMenu.png",

	"data/TEXTURE/Menu/MenuButtons/PauseTitle.png",

	"data/TEXTURE/Effect/fade_black.png",

	"data/TEXTURE/Menu/Numbers2.png",
	"data/TEXTURE/Menu/MenuButtons/DebugButtonRound.png",
	"data/TEXTURE/Menu/MenuButtons/DebugButtonHP.png",
	"data/TEXTURE/Menu/MenuButtons/DebugButtonEnergy.png",
	"data/TEXTURE/Menu/MenuButtons/DebugButtonConfirm.png",
};


static BOOL						g_bg_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_bg_w, g_bg_h;					// ���ƍ���
static XMFLOAT3					g_bg_Pos;						// �|���S���̍��W
static int						g_bg_TexNo;						// �e�N�X�`���ԍ�


static PAUSE_MENU_BUTTON		menuButton[PAUSE_BUTTON_MAX];
static PAUSE_MENU_BUTTON		debugMenuButton[DEBUG_BUTTON_MAX];

static int						buttonSelected;

static int						g_PauseScreen;

BOOL							g_PauseInput;

static int						g_Debug_HP, g_Debug_Energy, g_Debug_Round;

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPauseMenu(void)
{
	switch (GetLanguage())
	{
	case LANG_JPN:
		g_TexturName[1] = "data/TEXTURE/Menu/MenuButtons/Japanese/Resume.png";
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/Japanese/Options.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/Japanese/ReturnToMenu.png";
		break;

	case LANG_USA:
		g_TexturName[1] = "data/TEXTURE/Menu/MenuButtons/English/Resume.png";
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/English/Options.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/English/ReturnToMenu.png";
		break;

	case LANG_ESP:
		g_TexturName[1] = "data/TEXTURE/Menu/MenuButtons/Spanish/Resume.png";
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/Spanish/Options.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/Spanish/ReturnToMenu.png";
		break;
	}


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
	g_bg_Use   = TRUE;
	g_bg_w     = TEXTURE_WIDTH;
	g_bg_h	   = TEXTURE_HEIGHT;
	g_bg_Pos   = XMFLOAT3(SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);
	g_bg_TexNo = 0;

	for (int i = 0; i < PAUSE_BUTTON_MAX; i++)
	{
		menuButton[i].use	= TRUE;
		menuButton[i].pos	= XMFLOAT3((SCREEN_CENTER_X), (SCREEN_CENTER_Y - 50.0f) + (i * 60.0f), 0.0f);	// ���S�_����\��
		menuButton[i].rot	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		menuButton[i].scl	= XMFLOAT3(1.0f, 1.0f, 1.0f);
		menuButton[i].w		= TEXTURE_BUTTON_WIDTH;
		menuButton[i].h		= TEXTURE_BUTTON_HEIGHT;
		menuButton[i].texNo = 1 + i;
		menuButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	menuButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);


	for (int i = 0; i < DEBUG_BUTTON_MAX; i++)
	{
		debugMenuButton[i].use = TRUE;
		debugMenuButton[i].pos = XMFLOAT3((SCREEN_CENTER_X - 50.0f), (SCREEN_CENTER_Y - 75.0f) + (i * 60.0f), 0.0f);	// ���S�_����\��
		debugMenuButton[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		debugMenuButton[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		debugMenuButton[i].w = TEXTURE_BUTTON_WIDTH;
		debugMenuButton[i].h = TEXTURE_BUTTON_HEIGHT;
		debugMenuButton[i].texNo = 7 + i;
		debugMenuButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	debugMenuButton[3].pos.x = SCREEN_CENTER_X;
	debugMenuButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);


	g_PauseScreen = PAUSE_SCREEN_MAIN;
	buttonSelected = 0;

	g_PauseInput = TRUE;

	g_Debug_Round	= 1;
	g_Debug_HP		= 9;
	g_Debug_Energy	= 8;

	InitOptions();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPauseMenu(void)
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

	UninitOptions();

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePauseMenu(void)
{
	if (g_PauseScreen == PAUSE_SCREEN_OPTIONS)
	{
		UpdateOptions();
		return;
	}
	else if (g_PauseScreen == PAUSE_SCREEN_DEBUG_MENU)
	{
		UpdateDebugMenu();
		return;
	}


	if (g_PauseInput)
	{
		//Pause Mode
		if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_B))
		{
			SetPauseScreen(FALSE);
			PlaySound(SOUND_LABEL_SE_PulseButtonBack);
		}

		//�{�^������������
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			OnClickButtonPauseMenu(buttonSelected);
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
		}


		// �{�^���̑I��
		if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN) || GetKeyboardTrigger(DIK_S))
		{
			menuButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			buttonSelected = (buttonSelected + 1) % PAUSE_BUTTON_MAX;
			menuButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}
		else if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP) || GetKeyboardTrigger(DIK_W))
		{
			menuButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			buttonSelected = (--buttonSelected < 0) ? (PAUSE_BUTTON_MAX - 1) : buttonSelected;
			menuButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}


		//Debug menu
		if (GetKeyboardPress(DIK_D) && GetKeyboardPress(DIK_G))
		{
			g_Debug_Round  = GetRound();
			buttonSelected = 0;
			g_PauseScreen  = PAUSE_SCREEN_DEBUG_MENU;
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// Pause�̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_bg_TexNo]);

		//Pause�̔w�i�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_bg_Pos.x;	// �\���ʒuX
		float py = g_bg_Pos.y;	// �\���ʒuY
		float pw = g_bg_w;		// �\����
		float ph = g_bg_h;		// �\������


		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	//Pause Title  
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_bg_Pos.x, g_bg_Pos.y - g_bg_h / 2 + 12.0f, TEXTURE_BUTTON_WIDTH, TEXTURE_BUTTON_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	if (g_PauseScreen == PAUSE_SCREEN_OPTIONS)
	{
		DrawOptions();
		return;
	}
	else if (g_PauseScreen == PAUSE_SCREEN_DEBUG_MENU)
	{
		DrawDebugMenuButton();
		DrawDebugOptionsNumbers();
		return;
	}

	DrawMenuButtonPauseMenu();

}


//=============================================================================
// Menu Button ��`��
//=============================================================================
void DrawMenuButtonPauseMenu(void)
{

	for (int i = 0; i < PAUSE_BUTTON_MAX; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[menuButton[i].texNo]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = menuButton[i].pos.x;			// �{�^���̕\���ʒuX
		float py = menuButton[i].pos.y;			// �{�^���̕\���ʒuY
		float pw = TEXTURE_BUTTON_WIDTH;		// �{�^���̕\����
		float ph = TEXTURE_BUTTON_HEIGHT;		// �{�^���̕\������


		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			menuButton[i].color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	

}



//=============================================================================
// OnClick Button ��`��
//=============================================================================
void OnClickButtonPauseMenu(int button)
{
	switch (button)
	{
	case PAUSE_BUTTON_NEW_GAME:
		SetPauseScreen(FALSE);
		break;

	case PAUSE_BUTTON_OPTIONS:
		g_PauseScreen = PAUSE_SCREEN_OPTIONS;
		//���ʐݒ�
		//���邳�ݒ�
		//����ݒ�
		break;

	case PAUSE_BUTTON_QUIT_GAME:
		//SetFade(FADE_OUT, MODE_TITLE);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_MENU);
		g_PauseInput = FALSE;
		//SetPauseScreen(FALSE);
		break;
	}
}


XMFLOAT3 GetPauseMenuButtonPos(void)
{
	return menuButton[0].pos;
}


void BackToPauseMenu(void)
{
	g_PauseScreen = PAUSE_SCREEN_MAIN;
}



void UpdateDebugMenu(void)
{

	if (buttonSelected == DEBUG_BUTTON_CONFIRM && (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A)))
	{
		OnClickButtonPauseMenu();
	}


	//Get out Debug Mode
	if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_B))
	{
		g_PauseScreen = PAUSE_SCREEN_MAIN;
		buttonSelected = 0;
		PlaySound(SOUND_LABEL_SE_PulseButtonBack);
	}


	// �{�^���̑I�����㉺
	if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN) || GetKeyboardTrigger(DIK_S))
	{
		debugMenuButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		buttonSelected = (buttonSelected + 1) % DEBUG_BUTTON_MAX;
		debugMenuButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		PlaySound(SOUND_LABEL_SE_ChangeButton);
	}
	else if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP) || GetKeyboardTrigger(DIK_W))
	{
		debugMenuButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		buttonSelected = (--buttonSelected < 0) ? (DEBUG_BUTTON_MAX - 1) : buttonSelected;
		debugMenuButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		PlaySound(SOUND_LABEL_SE_ChangeButton);
	}

	// ���l�̑I�������E
	if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_RIGHT) || GetKeyboardTrigger(DIK_D))
	{
		switch (buttonSelected)
		{
		case DEBUG_BUTTON_ROUND:
			if (g_Debug_Round < 8)
			{
				g_Debug_Round++;
			}
			break;

		case DEBUG_BUTTON_HP:
			if (g_Debug_HP < 9)
			{
				g_Debug_HP++;
			}
			break;

		case DEBUG_BUTTON_ENERGY:
			if (g_Debug_Energy < 8)
			{
				g_Debug_Energy++;
			}
			break;
		}

	}
	else if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_LEFT) || GetKeyboardTrigger(DIK_A))
	{

		switch (buttonSelected)
		{
		case DEBUG_BUTTON_ROUND:
			if (g_Debug_Round > 1)
			{
				g_Debug_Round--;
			}
			break;

		case DEBUG_BUTTON_HP:
			if (g_Debug_HP > 0)
			{
				g_Debug_HP--;
			}
			break;

		case DEBUG_BUTTON_ENERGY:
			if (g_Debug_Energy > 0)
			{
				g_Debug_Energy--;
			}
			break;
		}
	}
}



//=============================================================================
// Debug Menu Button ��`��
//=============================================================================
void DrawDebugMenuButton(void)
{

	for (int i = 0; i < DEBUG_BUTTON_MAX; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[debugMenuButton[i].texNo]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = debugMenuButton[i].pos.x;			// �{�^���̕\���ʒuX
		float py = debugMenuButton[i].pos.y;			// �{�^���̕\���ʒuY
		float pw = TEXTURE_BUTTON_WIDTH;		// �{�^���̕\����
		float ph = TEXTURE_BUTTON_HEIGHT;		// �{�^���̕\������


		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			debugMenuButton[i].color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


}

void DrawDebugOptionsNumbers(void)
{
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

	for (int i = 0; i < DEBUG_BUTTON_MAX - 1; i++)
	{
		// ��������������
		int number;

		if (i == 0)
		{
			number = g_Debug_Round;
		}
		else if (i == 1)
		{
			number = g_Debug_HP;
		}
		else
		{
			number = g_Debug_Energy;
		}

		// ����\�����錅�̐���
		float x = (float)(number % 10);


		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float pw = TEXTURE_NUM_WIDTH * 1.5f;			// �X�R�A�̕\����
		float ph = TEXTURE_NUM_HEIGHT * 1.5f;			// �X�R�A�̕\������
		float px = debugMenuButton[i].pos.x + 100.0f;	// �X�R�A�̕\���ʒuX
		float py = debugMenuButton[i].pos.y;			// �X�R�A�̕\���ʒuY

		float tw = 1.0f / 11.0f;		// �e�N�X�`���̕�
		float th = 1.0f;			// �e�N�X�`���̍���
		float tx = x * tw;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// ���̌���
		number /= 10;
		
	}
}



//=============================================================================
// OnClick Debug Confirm Button ��`��
//=============================================================================
void OnClickButtonPauseMenu(void)
{
	if (g_Debug_Round != GetRound())
	{
		SetRetryRound(g_Debug_Round);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_COLISEUM);
	}
	else
	{
		SetPlayerStats(g_Debug_HP, g_Debug_Energy);
	}
}