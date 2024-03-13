//=============================================================================
//
// �^�C�g����ʏ��� [menu.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "menu.h"
#include "options.h"
#include "score.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(16)				// �e�N�X�`���̐�

#define TEXTURE_BUTTON_WIDTH		(1000.0f / 3)		// ���S�T�C�Y
#define TEXTURE_BUTTON_HEIGHT		(115.0f / 3)		// 

#define PARTICLE_SPAWN_TIME			(15)

#define MENU_PARTICLE_MAX			(50)

#define TEXTURE_NUM_WIDTH			(15.0f)	
#define TEXTURE_NUM_HEIGHT			(30.0f)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DrawMenuButton(void);
void DrawMenuControlButtons(void);

void OnClickButton(int button);

void SetMenuPartible(XMFLOAT3 pos);
void DrawMenuParticles(void);

void UpdateColiseumOptions(void);
void OnClickButtonColiseumOptions(void);
void DrawColiseumOptions(void);
void DrawColiseumOptionsNumbers(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Menu/MenuColiseum2.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Coliseum.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Tutorial.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Scores.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Options.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Exit.png",
	"data/TEXTURE/Menu/UIbgV.png",
	"data/TEXTURE/Menu/MenuButtons/ArenaConquer.png",

	"data/TEXTURE/Menu/MenuButtons/Japanese/AcceptButton.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/BackButton.png",

	"data/TEXTURE/Menu/MenuButtons/MenuControlButtons/ButtonEnter.png",
	"data/TEXTURE/Menu/MenuButtons/MenuControlButtons/ButtonEsc.png",

	"data/TEXTURE/Effect/Particle.png",

	"data/TEXTURE/Menu/Numbers2.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/ColiseumOptionFromStart.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/ColiseumOptionSelectRound.png",
};


static BOOL				g_bg_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float			g_bg_w, g_h;					// ���ƍ���
static XMFLOAT3			g_bg_Pos;						// �|���S���̍��W
static int				g_bg_TexNo;					// �e�N�X�`���ԍ�


static MENU_BUTTON		menuButton[BUTTON_MAX];
static MENU_BUTTON		menuColiseumButton[BUTTON_COLISEUM_MAX];
static int				buttonSelected;
static int				g_StartRound;

MENU_PARTICLE			g_MenuParticle[MENU_PARTICLE_MAX];
int						g_MenuParticleSpawnTimeCnt;

static int				g_MenuScreen;

BOOL					g_MenuInput;

static BOOL				g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitMenu(void)
{
	switch (GetLanguage())
	{
	case LANG_JPN:
		g_TexturName[1]  = "data/TEXTURE/Menu/MenuButtons/Japanese/Coliseum.png";
		g_TexturName[2]  = "data/TEXTURE/Menu/MenuButtons/Japanese/Tutorial.png";
		g_TexturName[3]  = "data/TEXTURE/Menu/MenuButtons/Japanese/Scores.png",
		g_TexturName[4]  = "data/TEXTURE/Menu/MenuButtons/Japanese/Options.png";
		g_TexturName[5]  = "data/TEXTURE/Menu/MenuButtons/Japanese/Exit.png";
		g_TexturName[8]  = "data/TEXTURE/Menu/MenuButtons/Japanese/AcceptButton.png";
		g_TexturName[9]  = "data/TEXTURE/Menu/MenuButtons/Japanese/BackButton.png";
		g_TexturName[14] = "data/TEXTURE/Menu/MenuButtons/Japanese/ColiseumOptionFromStart.png";
		g_TexturName[15] = "data/TEXTURE/Menu/MenuButtons/Japanese/ColiseumOptionSelectRound.png";

		break;

	case LANG_USA:
		g_TexturName[1]  = "data/TEXTURE/Menu/MenuButtons/English/Coliseum.png";
		g_TexturName[2]  = "data/TEXTURE/Menu/MenuButtons/English/Tutorial.png";
		g_TexturName[3]  = "data/TEXTURE/Menu/MenuButtons/English/Scores.png",
		g_TexturName[4]  = "data/TEXTURE/Menu/MenuButtons/English/Options.png";
		g_TexturName[5]  = "data/TEXTURE/Menu/MenuButtons/English/Exit.png";
		g_TexturName[8]  = "data/TEXTURE/Menu/MenuButtons/English/AcceptButton.png";
		g_TexturName[9]  = "data/TEXTURE/Menu/MenuButtons/English/BackButton.png";
		g_TexturName[14] = "data/TEXTURE/Menu/MenuButtons/English/ColiseumOptionFromStart.png";
		g_TexturName[15] = "data/TEXTURE/Menu/MenuButtons/English/ColiseumOptionSelectRound.png";
		break;

	case LANG_ESP:
		g_TexturName[1]  = "data/TEXTURE/Menu/MenuButtons/Spanish/Coliseum.png";
		g_TexturName[2]  = "data/TEXTURE/Menu/MenuButtons/Spanish/Tutorial.png";
		g_TexturName[3]  = "data/TEXTURE/Menu/MenuButtons/Spanish/Scores.png",
		g_TexturName[4]  = "data/TEXTURE/Menu/MenuButtons/Spanish/Options.png";
		g_TexturName[5]  = "data/TEXTURE/Menu/MenuButtons/Spanish/Exit.png";
		g_TexturName[8]  = "data/TEXTURE/Menu/MenuButtons/Spanish/AcceptButton.png";
		g_TexturName[9]  = "data/TEXTURE/Menu/MenuButtons/Spanish/BackButton.png";
		g_TexturName[14] = "data/TEXTURE/Menu/MenuButtons/Spanish/ColiseumOptionFromStart.png";
		g_TexturName[15] = "data/TEXTURE/Menu/MenuButtons/Spanish/ColiseumOptionSelectRound.png";
		break;
	}

	if (CheckGamepadConnection() == TRUE)
	{
		g_TexturName[10] = "data/TEXTURE/Menu/MenuButtons/MenuControlButtons/ButtonA.png";
		g_TexturName[11] = "data/TEXTURE/Menu/MenuButtons/MenuControlButtons/ButtonB.png";
	}
	else
	{
		g_TexturName[10] = "data/TEXTURE/Menu/MenuButtons/MenuControlButtons/ButtonEnter.png";
		g_TexturName[11] = "data/TEXTURE/Menu/MenuButtons/MenuControlButtons/ButtonEsc.png";
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
	g_h		   = TEXTURE_HEIGHT;
	g_bg_Pos   = XMFLOAT3(g_bg_w/2, g_h/2, 0.0f);
	g_bg_TexNo = 0;

	for (int i = 0; i < BUTTON_MAX; i++)
	{
		menuButton[i].use	= TRUE;
		menuButton[i].pos	= XMFLOAT3((SCREEN_CENTER_X / 2) - 50.0f, 300.0f + (i * 70.0f), 0.0f);	// ���S�_����\��
		menuButton[i].rot	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		menuButton[i].scl	= XMFLOAT3(1.0f, 1.0f, 1.0f);
		menuButton[i].w		= TEXTURE_BUTTON_WIDTH;
		menuButton[i].h		= TEXTURE_BUTTON_HEIGHT;
		menuButton[i].texNo = 1 + i;
		menuButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	menuButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	
	
	for (int i = 0; i < BUTTON_COLISEUM_MAX; i++)
	{
		menuColiseumButton[i].use	= TRUE;
		menuColiseumButton[i].pos	= XMFLOAT3((SCREEN_CENTER_X / 2) - 50.0f, 300.0f + (i * 70.0f), 0.0f);	// ���S�_����\��
		menuColiseumButton[i].rot	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		menuColiseumButton[i].scl	= XMFLOAT3(1.0f, 1.0f, 1.0f);
		menuColiseumButton[i].w		= TEXTURE_BUTTON_WIDTH;
		menuColiseumButton[i].h		= TEXTURE_BUTTON_HEIGHT;
		menuColiseumButton[i].texNo = 14 + i;
		menuColiseumButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	menuColiseumButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	buttonSelected = 0;

	g_StartRound = 1;


	for (int i = 0; i < MENU_PARTICLE_MAX; i++)
	{

		g_MenuParticle[i].use = FALSE;
		g_MenuParticle[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_MenuParticle[i].w = 10.0f;
		g_MenuParticle[i].h = 10.0f;
		g_MenuParticle[i].texNo = 12;

		g_MenuParticle[i].moveSpeed = 2.0f;
		g_MenuParticle[i].moveDir = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_MenuParticle[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_MenuParticle[i].lifeTime = 0;
		g_MenuParticle[i].lifeTimeCnt = 0;
		g_MenuParticle[i].alphaAdd = 0.0f;
	}

	g_MenuScreen = SCREEN_MAIN_MENU;

	g_MenuInput = TRUE;

	InitOptions();
	InitScore();
	SetRetryRound(0);
	SetPauseScreen(FALSE);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMenu(void)
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
void UpdateMenu(void)
{

	float posX = (float)(rand() % SCREEN_WIDTH);
	SetMenuPartible(XMFLOAT3(posX, SCREEN_HEIGHT, 0.0f));

	if (g_MenuScreen == SCREEN_OPTIONS)
	{
		UpdateOptions();
		return;
	}
	else if (g_MenuScreen == SCREEN_SCORES)
	{
		UpdateScore();
		return;
	}
	else if (g_MenuScreen == SCREEN_COLISEUM)
	{
		UpdateColiseumOptions();
		return;
	}

	if (g_MenuInput)
	{
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{// Enter��������A�X�e�[�W��؂�ւ���
			OnClickButton(buttonSelected);
			buttonSelected = 0;
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
		}

		//Pause Mode
		if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_B))
		{
			SetFade(FADE_OUT, MODE_TITLE);
			g_MenuInput = FALSE;
			PlaySound(SOUND_LABEL_SE_PulseButtonBack);
		}


		// �{�^���̑I��
		if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN) || GetKeyboardTrigger(DIK_S))
		{
			menuButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			buttonSelected = (buttonSelected + 1) % BUTTON_MAX;
			menuButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}
		else if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP) || GetKeyboardTrigger(DIK_W))
		{
			menuButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			buttonSelected = (--buttonSelected < 0) ? (BUTTON_MAX - 1) : buttonSelected;
			menuButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}
	}
	


#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMenu(void)
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

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	DrawMenuParticles();

	 /*Buttons BG*/
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, (SCREEN_CENTER_X / 2) - 50.0f, SCREEN_CENTER_Y, SCREEN_CENTER_X - 200, TEXTURE_HEIGHT - 100, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//Game Title  
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, (SCREEN_CENTER_X / 2) - 50.0f, 150.0f, 600.0f * 0.6f, 125.0f * 0.6f, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	DrawMenuControlButtons();

	if (g_MenuScreen == SCREEN_OPTIONS)
	{
		DrawOptions();
		return;
	}
	else if (g_MenuScreen == SCREEN_SCORES)
	{
		DrawScore();
		return;
	}
	else if (g_MenuScreen == SCREEN_COLISEUM)
	{
		DrawColiseumOptions();
		DrawColiseumOptionsNumbers();
		return;
	}

	DrawMenuButton();
	
}

//=============================================================================
// Menu Control Button ��`��
//=============================================================================
void DrawMenuControlButtons(void)
{

	for (int i = 0; i < 2; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8+i]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = SCREEN_WIDTH - 90.0f;			// �{�^���̕\���ʒuX
		float py = SCREEN_HEIGHT - 175.0f + (i * 50.0f);			// �{�^���̕\���ʒuY
		float pw = 500.0f / 3;		// �{�^���̕\����
		float ph = TEXTURE_BUTTON_HEIGHT;		// �{�^���̕\������


		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10 + i]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		px = SCREEN_WIDTH - 160.0f;			// �{�^���̕\���ʒuX
		pw = 285.0f / 3;		// �{�^���̕\����


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


}


//=============================================================================
// Menu Button ��`��
//=============================================================================
void DrawMenuButton(void)
{
	if (g_MenuScreen != SCREEN_MAIN_MENU) return;

	for (int i = 0; i < BUTTON_MAX; i++)
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
// OnClick Button 
// �����F�I�����Ă���{�^��
//=============================================================================
void OnClickButton(int button)
{
	switch (button)
	{
	case BUTTON_COLISEUM:
		
		//SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_COLISEUM);
		//g_MenuInput = FALSE;
		g_MenuScreen = SCREEN_COLISEUM;
		break;

	case BUTTON_TUTORIAL:
		//SetFade(FADE_OUT, MODE_TUTORIAL_1);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_TUTORIAL_1);
		g_MenuInput = FALSE;
		break;

	case BUTTON_SCORES:
		g_MenuScreen = SCREEN_SCORES;
		menuButton[BUTTON_SCORES].color   = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		menuButton[BUTTON_COLISEUM].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		break;

	case BUTTON_OPTIONS:
		g_MenuScreen = SCREEN_OPTIONS;

		menuButton[BUTTON_OPTIONS].color  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		menuButton[BUTTON_COLISEUM].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		//���ʐݒ�
		//���邳�ݒ�
		//����ݒ�
		break;

	case BUTTON_QUIT_GAME:
		QuitGame();
		break;
	}
}


//=============================================================================
// ���C�����j���[�{�^���̎擾
//=============================================================================
XMFLOAT3 GetMainMenuButtonPos(void)
{
	return menuButton[0].pos;
}


//=============================================================================
// ���C�����j���[�ɖ߂�
//=============================================================================
void BackToMainMenu(void)
{
	g_MenuScreen = SCREEN_MAIN_MENU;
}

//=============================================================================
// ���C�����j���[�p�[�e�B�N���̐ݒ�
//=============================================================================
void SetMenuPartible(XMFLOAT3 pos)
{
	g_MenuParticleSpawnTimeCnt++;
	if (g_MenuParticleSpawnTimeCnt > PARTICLE_SPAWN_TIME)
	{
		g_MenuParticleSpawnTimeCnt = 0;

		for (int i = 0; i < MENU_PARTICLE_MAX; i++)
		{
			if (g_MenuParticle[i].use == FALSE)
			{
				g_MenuParticle[i].use = TRUE;
				g_MenuParticle[i].pos = pos;
				g_MenuParticle[i].moveSpeed = 0.0f;

				float angle = (float)(rand() % 78);
				angle /= 100;
				angle += XM_PIDIV2 - XM_PIDIV4 / 2;
				angle *= -1;
				g_MenuParticle[i].moveDir.x = cosf(angle);	//angle�̕����ֈړ�
				g_MenuParticle[i].moveDir.y = sinf(angle);	//angle�̕����ֈړ�

				g_MenuParticle[i].lifeTime = MENU_PARTICLE_MAX;
				g_MenuParticle[i].lifeTimeCnt = 0;

				g_MenuParticle[i].alphaAdd = 0.007f;

				g_MenuParticle[i].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
				break;
			}
		}
	}
	

	for (int i = 0; i < MENU_PARTICLE_MAX; i++)
	{
		if (g_MenuParticle[i].use == TRUE)
		{
			g_MenuParticle[i].lifeTimeCnt++;
			g_MenuParticle[i].moveSpeed += 0.02f;
			g_MenuParticle[i].pos.x += g_MenuParticle[i].moveDir.x * g_MenuParticle[i].moveSpeed;
			g_MenuParticle[i].pos.y += g_MenuParticle[i].moveDir.y * g_MenuParticle[i].moveSpeed;
			g_MenuParticle[i].color.y += 0.002f/*1.0f / TITLE_PARTICLE_MAX*/;

			if (g_MenuParticle[i].color.w <= 1.0f)
			{
				g_MenuParticle[i].color.w += g_MenuParticle[i].alphaAdd;
			}

			//��ʒ[�̓����蔻��
			{
				if (g_MenuParticle[i].pos.x < (-g_MenuParticle[i].w / 2))		// �E
				{
					g_MenuParticle[i].use = FALSE;
				}
				if (g_MenuParticle[i].pos.x > (SCREEN_WIDTH + g_MenuParticle[i].w / 2))	// ��
				{
					g_MenuParticle[i].use = FALSE;
				}
				if (g_MenuParticle[i].pos.y < (-g_MenuParticle[i].h / 2))		// ��
				{
					g_MenuParticle[i].use = FALSE;
				}
			}
		}
	}
}

//=============================================================================
// ���C�����j���[�̃p�[�e�B�N���`��
//=============================================================================
void DrawMenuParticles(void)
{
	for (int i = 0; i < MENU_PARTICLE_MAX; i++)
	{
		if (g_MenuParticle[i].use == TRUE)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_MenuParticle[i].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_MenuParticle[i].pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_MenuParticle[i].pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_MenuParticle[i].w;					// �G�l�~�[�̕\����
			float ph = g_MenuParticle[i].h;					// �G�l�~�[�̕\������

			float tw = 1.0f;		// �e�N�X�`���̕�
			float th = 1.0f;		// �e�N�X�`���̍���
			float tx = 0.0f;		// �e�N�X�`���̍���X���W
			float ty = 0.0f;		// �e�N�X�`���̍���Y���W


			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_MenuParticle[i].color);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �R���V�A���{�^����Input����
//=============================================================================
void UpdateColiseumOptions(void)
{
	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
	{
		OnClickButtonColiseumOptions();
		PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
	}


	//Get out Debug Mode
	if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_B))
	{
		g_MenuScreen = SCREEN_MAIN_MENU;
		buttonSelected = 0;
		menuColiseumButton[BUTTON_COLISEUM_FROM_START].color	 = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		menuColiseumButton[BUTTON_COLISEUM_ROUND_SELECTOR].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		PlaySound(SOUND_LABEL_SE_PulseButtonBack);
	}


	// �{�^���̑I�����㉺
	if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_DOWN) || GetKeyboardTrigger(DIK_S))
	{
		menuColiseumButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		buttonSelected = (buttonSelected + 1) % BUTTON_COLISEUM_MAX;
		menuColiseumButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		PlaySound(SOUND_LABEL_SE_ChangeButton);
	}
	else if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP) || GetKeyboardTrigger(DIK_W))
	{
		menuColiseumButton[buttonSelected].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		buttonSelected = (--buttonSelected < 0) ? (BUTTON_COLISEUM_MAX - 1) : buttonSelected;
		menuColiseumButton[buttonSelected].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		PlaySound(SOUND_LABEL_SE_ChangeButton);
	}

	// ���l�̑I�������E
	if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_RIGHT) || GetKeyboardTrigger(DIK_D))
	{
		if (buttonSelected == BUTTON_COLISEUM_ROUND_SELECTOR)
		{
			if (g_StartRound < 8)
			{
				g_StartRound++;
				PlaySound(SOUND_LABEL_SE_ChangeButton);
			}
		}
	}
	else if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_LEFT) || GetKeyboardTrigger(DIK_A))
	{
		if (buttonSelected == BUTTON_COLISEUM_ROUND_SELECTOR)
		{
			if (g_StartRound > 1)
			{
				g_StartRound--;
				PlaySound(SOUND_LABEL_SE_ChangeButton);
			}
		}
	}
}


//=============================================================================
// �R���V�A���{�^����OnClick Button 
// �����F�I�����Ă���{�^��
//=============================================================================
void OnClickButtonColiseumOptions(void)
{
	switch (buttonSelected)
	{
	case BUTTON_COLISEUM_FROM_START:
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_COLISEUM);
		g_MenuInput = FALSE;		
		break;

	case BUTTON_COLISEUM_ROUND_SELECTOR:
		SetRetryRound(g_StartRound);
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_COLISEUM);
		g_MenuInput = FALSE;
		break;
	}
}


//=============================================================================
// �R���V�A���̃{�^���̕`��
//=============================================================================
void DrawColiseumOptions(void)
{
	for (int i = 0; i < BUTTON_COLISEUM_MAX; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[menuColiseumButton[i].texNo]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = menuColiseumButton[i].pos.x;			// �{�^���̕\���ʒuX
		float py = menuColiseumButton[i].pos.y;			// �{�^���̕\���ʒuY
		float pw = TEXTURE_BUTTON_WIDTH;		// �{�^���̕\����
		float ph = TEXTURE_BUTTON_HEIGHT;		// �{�^���̕\������


		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			menuColiseumButton[i].color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// �R���V�A���{�^���̐����`��
//=============================================================================
void DrawColiseumOptionsNumbers(void)
{
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);

	
	// ��������������
	int number = g_StartRound;

	// ����\�����錅�̐���
	float x = (float)(number % 10);


	// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
	float pw = TEXTURE_NUM_WIDTH * 1.5f;			// �X�R�A�̕\����
	float ph = TEXTURE_NUM_HEIGHT * 1.5f;			// �X�R�A�̕\������
	float px = menuColiseumButton[1].pos.x + 93.0f;	// �X�R�A�̕\���ʒuX
	float py = menuColiseumButton[1].pos.y;			// �X�R�A�̕\���ʒuY

	float tw = 1.0f / 11.0f;		// �e�N�X�`���̕�
	float th = 1.0f;			// �e�N�X�`���̍���
	float tx = x * tw;			// �e�N�X�`���̍���X���W
	float ty = 0.0f;			// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		menuColiseumButton[1].color);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// ���̌���
	number /= 10;
}

