//=============================================================================
//
// �^�C�g����ʏ��� [options.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "options.h"
#include "sound.h"
#include "menu.h"
#include "pauseMenu.h"
#include "input.h"
#include "fade.h"
#include "file.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define TEXTURE_WIDTH				(SCREEN_WIDTH / 2)	// �w�i�T�C�Y
//#define TEXTURE_HEIGHT			(SCREEN_HEIGHT / 2)	// 
#define TEXTURE_MAX					(17)				// �e�N�X�`���̐�

#define TEXTURE_BUTTON_WIDTH		(500.0f / 3)		// 
#define TEXTURE_BUTTON_HEIGHT		(115.0f / 3)		// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void OnClickOptionsButton(int button);

void DrawOptionsButtons(void);
void DrawSoundSettings(void);
void DrawControls(void);
void DrawLanguageSettings(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {

	"data/TEXTURE/Menu/MenuButtons/Japanese/SoundSettings.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/ControlsButton.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Language.png",

	"data/TEXTURE/Menu/MenuButtons/Japanese/SEVolumeButton.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/BGMVolumeButton.png",

	"data/TEXTURE/Menu/MenuButtons/ControllerButton.png",
	"data/TEXTURE/Menu/MenuButtons/KeyBoardButton.png",

	"data/TEXTURE/Menu/MenuButtons/SliderLine.png",
	"data/TEXTURE/Menu/MenuButtons/SliderMark.png",

	"data/TEXTURE/Menu/MenuButtons/Japanese/Controls.png",

	"data/TEXTURE/Menu/MenuButtons/Japanese/Options_Title.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/SoundSettingsButton_Title.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Controls_Title.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Language_Title.png",

	"data/TEXTURE/Menu/MenuButtons/JapaneseButton.png",
	"data/TEXTURE/Menu/MenuButtons/EnglishButton.png",
	"data/TEXTURE/Menu/MenuButtons/SpanishButton.png",
};


static OPTIONS_BUTTON	g_Options_Button	 [OPTIONS_BUTTON_MAX];
static OPTIONS_BUTTON	g_SoundSetting_Button[SOUND_SETTINGS_BUTTON_MAX];
static OPTIONS_BUTTON	g_Controls_Button	 [CONTROLS_SETTINGS_BUTTON_MAX];
static OPTIONS_BUTTON	g_Language_Button	 [LANGUAGE_SETTINGS_BUTTON_MAX];

static float			XPosSliderBGM;
static float			XPosSliderSE;

static XMFLOAT3			g_ButtonsPivot;
static int				buttonSelected;
static int				optionTitleTex;

static int				g_OptionsScreen;

static BOOL				g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitOptions(void)
{
	switch (GetLanguage())
	{
	case LANG_JPN:
		g_TexturName[0]  = "data/TEXTURE/Menu/MenuButtons/Japanese/SoundSettings.png";
		g_TexturName[1]  = "data/TEXTURE/Menu/MenuButtons/Japanese/ControlsButton.png";
		g_TexturName[2]  = "data/TEXTURE/Menu/MenuButtons/Japanese/Language.png";
		g_TexturName[3]  = "data/TEXTURE/Menu/MenuButtons/Japanese/SEVolumeButton.png";
		g_TexturName[4]  = "data/TEXTURE/Menu/MenuButtons/Japanese/BGMVolumeButton.png";		
		g_TexturName[10] = "data/TEXTURE/Menu/MenuButtons/Japanese/Options_Title.png";
		g_TexturName[11] = "data/TEXTURE/Menu/MenuButtons/Japanese/SoundSettings_Title.png";
		g_TexturName[12] = "data/TEXTURE/Menu/MenuButtons/Japanese/Controls_Title.png";
		g_TexturName[13] = "data/TEXTURE/Menu/MenuButtons/Japanese/Language_Title.png";

		if (GetMode() == MODE_MENU)
		{
			g_TexturName[9] = "data/TEXTURE/Menu/MenuButtons/Japanese/Controls.png";
		}
		else
		{
			g_TexturName[9] = "data/TEXTURE/Menu/MenuButtons/Japanese/ControlsPauseMenu.png";
		}
		break;

	case LANG_USA:
		g_TexturName[0]  = "data/TEXTURE/Menu/MenuButtons/English/SoundSettings.png";
		g_TexturName[1]  = "data/TEXTURE/Menu/MenuButtons/English/ControlsButton.png";
		g_TexturName[2]  = "data/TEXTURE/Menu/MenuButtons/English/Language.png";
		g_TexturName[3]  = "data/TEXTURE/Menu/MenuButtons/English/SEVolumeButton.png";
		g_TexturName[4]  = "data/TEXTURE/Menu/MenuButtons/English/BGMVolumeButton.png";
		g_TexturName[10] = "data/TEXTURE/Menu/MenuButtons/English/Options_Title.png";
		g_TexturName[11] = "data/TEXTURE/Menu/MenuButtons/English/SoundSettings_Title.png";
		g_TexturName[12] = "data/TEXTURE/Menu/MenuButtons/English/Controls_Title.png";
		g_TexturName[13] = "data/TEXTURE/Menu/MenuButtons/English/Language_Title.png";

		if (GetMode() == MODE_MENU)
		{
			g_TexturName[9] = "data/TEXTURE/Menu/MenuButtons/English/Controls.png";
		}
		else
		{
			g_TexturName[9] = "data/TEXTURE/Menu/MenuButtons/English/ControlsPauseMenu.png";
		}
		break;

	case LANG_ESP:
		g_TexturName[0]  = "data/TEXTURE/Menu/MenuButtons/Spanish/SoundSettings.png";
		g_TexturName[1]  = "data/TEXTURE/Menu/MenuButtons/Spanish/ControlsButton.png";
		g_TexturName[2]  = "data/TEXTURE/Menu/MenuButtons/Spanish/Language.png";
		g_TexturName[3]  = "data/TEXTURE/Menu/MenuButtons/Spanish/SEVolumeButton.png";
		g_TexturName[4]  = "data/TEXTURE/Menu/MenuButtons/Spanish/BGMVolumeButton.png";
		g_TexturName[10] = "data/TEXTURE/Menu/MenuButtons/Spanish/Options_Title.png";
		g_TexturName[11] = "data/TEXTURE/Menu/MenuButtons/Spanish/SoundSettings_Title.png";
		g_TexturName[12] = "data/TEXTURE/Menu/MenuButtons/Spanish/Controls_Title.png";
		g_TexturName[13] = "data/TEXTURE/Menu/MenuButtons/Spanish/Language_Title.png";

		if (GetMode() == MODE_MENU)
		{
			g_TexturName[9] = "data/TEXTURE/Menu/MenuButtons/Spanish/Controls.png";
		}
		else
		{
			g_TexturName[9] = "data/TEXTURE/Menu/MenuButtons/Spanish/ControlsPauseMenu.png";
		}
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
	if (GetMode() == MODE_MENU)
	{
		g_ButtonsPivot = GetMainMenuButtonPos();
	}
	else
	{
		g_ButtonsPivot = GetPauseMenuButtonPos();
	}
	


	for (int i = 0; i < OPTIONS_BUTTON_MAX; i++)
	{
		g_Options_Button[i].use	= TRUE;
		g_Options_Button[i].pos	= XMFLOAT3(g_ButtonsPivot.x, g_ButtonsPivot.y + (i * 80.0f), 0.0f);	// ���S�_����\��
		g_Options_Button[i].rot	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Options_Button[i].scl	= XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Options_Button[i].w		= TEXTURE_BUTTON_WIDTH;
		g_Options_Button[i].h		= TEXTURE_BUTTON_HEIGHT;
		g_Options_Button[i].texNo = i;
		g_Options_Button[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	g_Options_Button[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);


	for (int i = 0; i < SOUND_SETTINGS_BUTTON_MAX; i++)
	{
		g_SoundSetting_Button[i].use = TRUE;
		g_SoundSetting_Button[i].pos = XMFLOAT3(g_ButtonsPivot.x - 100, g_ButtonsPivot.y + (i * 80.0f), 0.0f);	// ���S�_����\��
		g_SoundSetting_Button[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_SoundSetting_Button[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_SoundSetting_Button[i].w = TEXTURE_BUTTON_WIDTH;
		g_SoundSetting_Button[i].h = TEXTURE_BUTTON_HEIGHT;
		g_SoundSetting_Button[i].texNo = 3 + i;
		g_SoundSetting_Button[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	g_SoundSetting_Button[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	XPosSliderBGM = g_SoundSetting_Button[0].pos.x + 200 - g_SoundSetting_Button[0].w / 2;
	XPosSliderBGM += (GetVolumeBGM() / 0.2f) * (g_SoundSetting_Button[0].w / 10);

	XPosSliderSE = g_SoundSetting_Button[1].pos.x + 200 - g_SoundSetting_Button[1].w / 2;
	XPosSliderSE += (GetVolumeSE() / 0.2f) * (g_SoundSetting_Button[0].w / 10);




	for (int i = 0; i < CONTROLS_SETTINGS_BUTTON_MAX; i++)
	{
		g_Controls_Button[i].use = TRUE;
		g_Controls_Button[i].pos = XMFLOAT3(g_ButtonsPivot.x, g_ButtonsPivot.y + (i * 80.0f), 0.0f);	// ���S�_����\��
		g_Controls_Button[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Controls_Button[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Controls_Button[i].w = TEXTURE_BUTTON_WIDTH;
		g_Controls_Button[i].h = TEXTURE_BUTTON_HEIGHT;
		g_Controls_Button[i].texNo = 5 + i;
		g_Controls_Button[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	g_Controls_Button[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);


	for (int i = 0; i < LANGUAGE_SETTINGS_BUTTON_MAX; i++)
	{
		g_Language_Button[i].use = TRUE;
		g_Language_Button[i].pos = XMFLOAT3(g_ButtonsPivot.x, g_ButtonsPivot.y + (i * 80.0f), 0.0f);	// ���S�_����\��
		g_Language_Button[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Language_Button[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Language_Button[i].w = TEXTURE_BUTTON_WIDTH;
		g_Language_Button[i].h = TEXTURE_BUTTON_HEIGHT;
		g_Language_Button[i].texNo = 14 + i;
		g_Language_Button[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	g_Language_Button[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);


	g_OptionsScreen = OPTION_SCREEN_OPTIONS;
	buttonSelected = BUTTON_SOUND_SETTINGS;
	optionTitleTex = 10;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitOptions(void)
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
void UpdateOptions(void)
{

	OPTIONS_BUTTON* CurrentButtons = &g_Options_Button[0];
	int ButtonsMax = 0;
	BOOL ButtonsMovement = FALSE;

	switch (g_OptionsScreen)
	{
	case OPTION_SCREEN_OPTIONS:
		CurrentButtons	= &g_Options_Button[0];
		ButtonsMax		= OPTIONS_BUTTON_MAX;
		ButtonsMovement = TRUE;
		break;

	case OPTION_SCREEN_SOUND_SETTINGS:
		CurrentButtons	= &g_SoundSetting_Button[0];
		ButtonsMax		= SOUND_SETTINGS_BUTTON_MAX;
		ButtonsMovement = TRUE;
		break;

	/*case OPTION_SCREEN_CONTROLS:
		CurrentButtons = &g_Controls_Button[0];
		ButtonsMax = CONTROLS_SETTINGS_BUTTON_MAX;
		ButtonsMovement = TRUE;
		break;*/

		case OPTION_SCREEN_LANGUAGE:
		CurrentButtons	= &g_Language_Button[0];
		ButtonsMax		= LANGUAGE_SETTINGS_BUTTON_MAX;
		ButtonsMovement = TRUE;
		break;

	default:

		break;
	}

	if (ButtonsMovement == TRUE)
	{
		// �{�^���̑I��
		if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN) || GetKeyboardTrigger(DIK_S))
		{
			CurrentButtons[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			buttonSelected = (buttonSelected + 1) % ButtonsMax;
			CurrentButtons[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}
		else if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP) || GetKeyboardTrigger(DIK_W))
		{
			CurrentButtons[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			buttonSelected = (--buttonSelected < 0) ? (ButtonsMax - 1) : buttonSelected;
			CurrentButtons[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}
	}
	

	if (g_OptionsScreen == OPTION_SCREEN_SOUND_SETTINGS)
	{
		
		if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_RIGHT) || GetKeyboardTrigger(DIK_D))
		{
			float XPosSliderSEMax = g_SoundSetting_Button[0].pos.x + 200 + g_SoundSetting_Button[0].w / 2;
			PlaySound(SOUND_LABEL_SE_ChangeButton);

			if (buttonSelected == BUTTON_SE_VOLUME)
			{
				XPosSliderSE += g_SoundSetting_Button[0].w / 10;
				ChangeSEVolume(0.2f);

				if (XPosSliderSE > XPosSliderSEMax)
				{
					XPosSliderSE = XPosSliderSEMax;
					SetSEVolume(2.0f);
				}
			}
			else if (buttonSelected == BUTTON_BGM_VOLUME)
			{
				XPosSliderBGM += g_SoundSetting_Button[0].w / 10;
				ChangeBGMVolume(0.2f);

				if (XPosSliderBGM > XPosSliderSEMax)
				{
					XPosSliderBGM = XPosSliderSEMax;
					SetBGMVolume(2.0f);
				}
			}
			
		}
		else if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_LEFT) || GetKeyboardTrigger(DIK_A))
		{
			float XPosSliderSEMin = g_SoundSetting_Button[0].pos.x + 200 - g_SoundSetting_Button[0].w / 2;
			PlaySound(SOUND_LABEL_SE_ChangeButton);

			if (buttonSelected == BUTTON_SE_VOLUME)
			{
				XPosSliderSE -= g_SoundSetting_Button[0].w / 10;
				ChangeSEVolume(-0.2f);

				if (XPosSliderSE < XPosSliderSEMin)
				{
					XPosSliderSE = XPosSliderSEMin;
					SetSEVolume(0.0f);
				}
			}
			else if (buttonSelected == BUTTON_BGM_VOLUME)
			{
				XPosSliderBGM -= g_SoundSetting_Button[0].w / 10;
				ChangeBGMVolume(-0.2f);

				if (XPosSliderBGM < XPosSliderSEMin)
				{
					XPosSliderBGM = XPosSliderSEMin;
					SetBGMVolume(0.0f);
				}
			}
		}
	}
	




	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
	{
		PlaySound(SOUND_LABEL_SE_PulseButtonAccept);

		if (g_OptionsScreen == OPTION_SCREEN_OPTIONS)
		{
			{//(JP)�{�^�������Z�b�g����		(EN)Reset the selected button to the first one
				g_Options_Button[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				g_Options_Button[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
				
			}

			OnClickOptionsButton(buttonSelected);
			buttonSelected = 0;
			
		}
		else if (g_OptionsScreen == OPTION_SCREEN_LANGUAGE)
		{
			{//(JP)�{�^�������Z�b�g����		(EN)Reset the selected button to the first one
				g_Language_Button[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				g_Language_Button[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			}

			OnClickOptionsButton(buttonSelected);
			buttonSelected = 0;
		}
		
		
	}



	if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_B))
	{
		PlaySound(SOUND_LABEL_SE_PulseButtonBack);

		if (ButtonsMovement == TRUE)	
		{//(JP)�{�^�������Z�b�g����		(EN)Reset the selected button to the first one
			CurrentButtons[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			CurrentButtons[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			buttonSelected = 0;
		}

		if (g_OptionsScreen == OPTION_SCREEN_OPTIONS)
		{
			if (GetMode() == MODE_MENU)
			{
				BackToMainMenu();
			}
			else
			{
				BackToPauseMenu();
			}
			
		}
		else
		{
			if (g_OptionsScreen == OPTION_SCREEN_SOUND_SETTINGS)
				SaveData();		//���ʐݒ���Z�[�u���邽��

			g_OptionsScreen = OPTION_SCREEN_OPTIONS;
			optionTitleTex = 10;
		}
	}


#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawOptions(void)
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


	//Options Title  
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[optionTitleTex]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_ButtonsPivot.x, g_ButtonsPivot.y - 70.0f, TEXTURE_BUTTON_WIDTH, TEXTURE_BUTTON_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


	}

	switch (g_OptionsScreen)
	{
	case OPTION_SCREEN_OPTIONS:
		DrawOptionsButtons();
		break;

	case OPTION_SCREEN_SOUND_SETTINGS:
		DrawSoundSettings();
		break;

	case OPTION_SCREEN_CONTROLS:
		DrawControls();
		break;

	case OPTION_SCREEN_LANGUAGE:
		DrawLanguageSettings();
		break;
	}
	

}


void DrawOptionsButtons(void)
{

	// �{�^����`��
	for (int i = 0; i < OPTIONS_BUTTON_MAX; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Options_Button[i].texNo]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Options_Button[i].pos.x;			// �{�^���̕\���ʒuX
		float py = g_Options_Button[i].pos.y;			// �{�^���̕\���ʒuY
		float pw = TEXTURE_BUTTON_WIDTH;		// �{�^���̕\����
		float ph = TEXTURE_BUTTON_HEIGHT;		// �{�^���̕\������


		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_Options_Button[i].color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}



void DrawSoundSettings(void)
{
	// ���ʐݒ�̃{�^����`��
	for (int i = 0; i < SOUND_SETTINGS_BUTTON_MAX; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_SoundSetting_Button[i].texNo]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_SoundSetting_Button[i].pos.x;			// �{�^���̕\���ʒuX
		float py = g_SoundSetting_Button[i].pos.y;			// �{�^���̕\���ʒuY
		float pw = TEXTURE_BUTTON_WIDTH;		// �{�^���̕\����
		float ph = TEXTURE_BUTTON_HEIGHT;		// �{�^���̕\������


		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_SoundSetting_Button[i].color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);



		//Slider
		{
			{//Slider Line
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_SoundSetting_Button[i].pos.x + 200.0f;			// �{�^���̕\���ʒuX
				float py = g_SoundSetting_Button[i].pos.y;			// �{�^���̕\���ʒuY
				float pw = TEXTURE_BUTTON_WIDTH;		// �{�^���̕\����
				float ph = TEXTURE_BUTTON_HEIGHT;		// �{�^���̕\������


				float tx = 0.0f;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W
				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;


				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

			
		}
	}

	{//Slider Mark SE
				// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = XPosSliderSE;		// �{�^���̕\���ʒuX
		float py = g_SoundSetting_Button[0].pos.y;				// �{�^���̕\���ʒuY
		float pw = 12.0f;										// �{�^���̕\����
		float ph = 25.0f;										// �{�^���̕\������


		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	{//Slider Mark BGM
				// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = XPosSliderBGM;		// �{�^���̕\���ʒuX
		float py = g_SoundSetting_Button[1].pos.y;				// �{�^���̕\���ʒuY
		float pw = 12.0f;										// �{�^���̕\����
		float ph = 25.0f;										// �{�^���̕\������


		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	
	

}


void DrawControls(void)
{

	// ����ݒ�̃{�^����`��

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

	//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
	float pw = SCREEN_HEIGHT / 2;		// �{�^���̕\����
	float ph = SCREEN_HEIGHT / 2;		// �{�^���̕\������
	float px = g_ButtonsPivot.x;			// �{�^���̕\���ʒuX
	float py = g_ButtonsPivot.y + (ph / 2) - 30.0f;			// �{�^���̕\���ʒuY
	if (GetMode() != MODE_MENU)
	{
		pw = SCREEN_WIDTH / 2;		// �{�^���̕\����
		ph = (SCREEN_HEIGHT / 2) - 50.0f;
		py -= 80.0f;
	}



	float tx = 0.0f;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;	// �e�N�X�`���̍���Y���W
	float tw = 1.0f;	// �e�N�X�`���̕�
	float th = 1.0f;


	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}


void DrawLanguageSettings(void)
{

	// ���ʐݒ�̃{�^����`��
	for (int i = 0; i < LANGUAGE_SETTINGS_BUTTON_MAX; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Language_Button[i].texNo]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Language_Button[i].pos.x;			// �{�^���̕\���ʒuX
		float py = g_Language_Button[i].pos.y;			// �{�^���̕\���ʒuY
		float pw = TEXTURE_BUTTON_WIDTH;		// �{�^���̕\����
		float ph = TEXTURE_BUTTON_HEIGHT;		// �{�^���̕\������


		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_Language_Button[i].color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}



//=============================================================================
// OnClick Button ��`��
//=============================================================================
void OnClickOptionsButton(int button)
{
	if (g_OptionsScreen == OPTION_SCREEN_OPTIONS)
	{
		switch (button)
		{
		case BUTTON_SOUND_SETTINGS:
			g_OptionsScreen = OPTION_SCREEN_SOUND_SETTINGS;
			optionTitleTex = 11;

			XPosSliderBGM = g_SoundSetting_Button[0].pos.x + 200 - g_SoundSetting_Button[0].w / 2;
			XPosSliderBGM += (GetVolumeBGM() / 0.2f) * (g_SoundSetting_Button[0].w / 10);

			XPosSliderSE = g_SoundSetting_Button[1].pos.x + 200 - g_SoundSetting_Button[1].w / 2;
			XPosSliderSE += (GetVolumeSE() / 0.2f) * (g_SoundSetting_Button[0].w / 10);
			//���ʐݒ�
			//���邳�ݒ�
			break;

		case BUTTON_CONTROLS:
			g_OptionsScreen = OPTION_SCREEN_CONTROLS;
			optionTitleTex = 12;
			break;

		case BUTTON_LANGUAGE:
			g_OptionsScreen = OPTION_SCREEN_LANGUAGE;
			optionTitleTex = 13;
			//����ݒ�
			break;

		}
	}
	else if (g_OptionsScreen == OPTION_SCREEN_LANGUAGE)
	{
		switch (button)
		{
		case BUTTON_JAPANESE:
			SetLanguage(LANG_JPN);
			break;

		case BUTTON_ENGLISH:
			SetLanguage(LANG_USA);
			break;

		case BUTTON_SPANISH:
			SetLanguage(LANG_ESP);
			break;
		}

		SetPauseScreen(FALSE);
		//SetFade(FADE_OUT, GetMode());
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, GetMode());

		SaveData();
	}
	
}
