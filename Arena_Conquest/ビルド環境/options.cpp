//=============================================================================
//
// タイトル画面処理 [options.cpp]
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
// マクロ定義
//*****************************************************************************
//#define TEXTURE_WIDTH				(SCREEN_WIDTH / 2)	// 背景サイズ
//#define TEXTURE_HEIGHT			(SCREEN_HEIGHT / 2)	// 
#define TEXTURE_MAX					(17)				// テクスチャの数

#define TEXTURE_BUTTON_WIDTH		(500.0f / 3)		// 
#define TEXTURE_BUTTON_HEIGHT		(115.0f / 3)		// 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void OnClickOptionsButton(int button);

void DrawOptionsButtons(void);
void DrawSoundSettings(void);
void DrawControls(void);
void DrawLanguageSettings(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

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
// 初期化処理
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

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
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
		g_Options_Button[i].pos	= XMFLOAT3(g_ButtonsPivot.x, g_ButtonsPivot.y + (i * 80.0f), 0.0f);	// 中心点から表示
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
		g_SoundSetting_Button[i].pos = XMFLOAT3(g_ButtonsPivot.x - 100, g_ButtonsPivot.y + (i * 80.0f), 0.0f);	// 中心点から表示
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
		g_Controls_Button[i].pos = XMFLOAT3(g_ButtonsPivot.x, g_ButtonsPivot.y + (i * 80.0f), 0.0f);	// 中心点から表示
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
		g_Language_Button[i].pos = XMFLOAT3(g_ButtonsPivot.x, g_ButtonsPivot.y + (i * 80.0f), 0.0f);	// 中心点から表示
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
// 終了処理
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
// 更新処理
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
		// ボタンの選択
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
			{//(JP)ボタンをリセットする		(EN)Reset the selected button to the first one
				g_Options_Button[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				g_Options_Button[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
				
			}

			OnClickOptionsButton(buttonSelected);
			buttonSelected = 0;
			
		}
		else if (g_OptionsScreen == OPTION_SCREEN_LANGUAGE)
		{
			{//(JP)ボタンをリセットする		(EN)Reset the selected button to the first one
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
		{//(JP)ボタンをリセットする		(EN)Reset the selected button to the first one
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
				SaveData();		//音量設定をセーブするため

			g_OptionsScreen = OPTION_SCREEN_OPTIONS;
			optionTitleTex = 10;
		}
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawOptions(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	//Options Title  
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[optionTitleTex]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_ButtonsPivot.x, g_ButtonsPivot.y - 70.0f, TEXTURE_BUTTON_WIDTH, TEXTURE_BUTTON_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
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

	// ボタンを描画
	for (int i = 0; i < OPTIONS_BUTTON_MAX; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Options_Button[i].texNo]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_Options_Button[i].pos.x;			// ボタンの表示位置X
		float py = g_Options_Button[i].pos.y;			// ボタンの表示位置Y
		float pw = TEXTURE_BUTTON_WIDTH;		// ボタンの表示幅
		float ph = TEXTURE_BUTTON_HEIGHT;		// ボタンの表示高さ


		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_Options_Button[i].color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}



void DrawSoundSettings(void)
{
	// 音量設定のボタンを描画
	for (int i = 0; i < SOUND_SETTINGS_BUTTON_MAX; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_SoundSetting_Button[i].texNo]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_SoundSetting_Button[i].pos.x;			// ボタンの表示位置X
		float py = g_SoundSetting_Button[i].pos.y;			// ボタンの表示位置Y
		float pw = TEXTURE_BUTTON_WIDTH;		// ボタンの表示幅
		float ph = TEXTURE_BUTTON_HEIGHT;		// ボタンの表示高さ


		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_SoundSetting_Button[i].color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);



		//Slider
		{
			{//Slider Line
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

				//プレイヤーの位置やテクスチャー座標を反映
				float px = g_SoundSetting_Button[i].pos.x + 200.0f;			// ボタンの表示位置X
				float py = g_SoundSetting_Button[i].pos.y;			// ボタンの表示位置Y
				float pw = TEXTURE_BUTTON_WIDTH;		// ボタンの表示幅
				float ph = TEXTURE_BUTTON_HEIGHT;		// ボタンの表示高さ


				float tx = 0.0f;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標
				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;


				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			
		}
	}

	{//Slider Mark SE
				// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = XPosSliderSE;		// ボタンの表示位置X
		float py = g_SoundSetting_Button[0].pos.y;				// ボタンの表示位置Y
		float pw = 12.0f;										// ボタンの表示幅
		float ph = 25.0f;										// ボタンの表示高さ


		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	{//Slider Mark BGM
				// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = XPosSliderBGM;		// ボタンの表示位置X
		float py = g_SoundSetting_Button[1].pos.y;				// ボタンの表示位置Y
		float pw = 12.0f;										// ボタンの表示幅
		float ph = 25.0f;										// ボタンの表示高さ


		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	
	

}


void DrawControls(void)
{

	// 操作設定のボタンを描画

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

	//プレイヤーの位置やテクスチャー座標を反映
	float pw = SCREEN_HEIGHT / 2;		// ボタンの表示幅
	float ph = SCREEN_HEIGHT / 2;		// ボタンの表示高さ
	float px = g_ButtonsPivot.x;			// ボタンの表示位置X
	float py = g_ButtonsPivot.y + (ph / 2) - 30.0f;			// ボタンの表示位置Y
	if (GetMode() != MODE_MENU)
	{
		pw = SCREEN_WIDTH / 2;		// ボタンの表示幅
		ph = (SCREEN_HEIGHT / 2) - 50.0f;
		py -= 80.0f;
	}



	float tx = 0.0f;	// テクスチャの左上X座標
	float ty = 0.0f;	// テクスチャの左上Y座標
	float tw = 1.0f;	// テクスチャの幅
	float th = 1.0f;


	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}


void DrawLanguageSettings(void)
{

	// 音量設定のボタンを描画
	for (int i = 0; i < LANGUAGE_SETTINGS_BUTTON_MAX; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Language_Button[i].texNo]);

		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_Language_Button[i].pos.x;			// ボタンの表示位置X
		float py = g_Language_Button[i].pos.y;			// ボタンの表示位置Y
		float pw = TEXTURE_BUTTON_WIDTH;		// ボタンの表示幅
		float ph = TEXTURE_BUTTON_HEIGHT;		// ボタンの表示高さ


		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標
		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_Language_Button[i].color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}



//=============================================================================
// OnClick Button を描画
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
			//音量設定
			//明るさ設定
			break;

		case BUTTON_CONTROLS:
			g_OptionsScreen = OPTION_SCREEN_CONTROLS;
			optionTitleTex = 12;
			break;

		case BUTTON_LANGUAGE:
			g_OptionsScreen = OPTION_SCREEN_LANGUAGE;
			optionTitleTex = 13;
			//言語設定
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
