//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "sprite.h"
#include "camera.h"
#include "input.h"
#include "fade.h"

#include "fightUI.h"
#include "write.h"

#include "modelBinaryFBX.h"
#include "player.h"
#include "sound.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define BUTTON_WIDTH		(121.5f)	// �{�^�����T�C�Y
#define BUTTON_HEIGHT		(25.0f)		// �{�^���c�T�C�Y

#define TEXTURE_MAX			(16)		// �e�N�X�`���̐�

#define ACTBUTTON_X_OFFSET	(120.0f)	// �A�N�V�����{�^���̉��I�t�Z�b�g
#define ACTBUTTON_Y_OFFSET	(90.0f)		// �A�N�V�����{�^���̏c�I�t�Z�b�g

#define OPTIONS_SPACE		(30.0f)		// �ݒ�{�^���̊Ԃ̃X�y�[�X

#define OPTIONS_TOP_MARGIN	(75.0f)		// �ݒ�{�^���̃g�b�v�X�y�[�X

#define PLAYER_UI_WIDTH		(250.0f)	// �v���C���[��UI���T�C�Y
#define PLAYER_UI_HEIGHT	(75.0f)		// �v���C���[��UI�c�T�C�Y

#define ACTION_MENU_WIDTH	(300.0f)	 // ACTION MENU�̉��T�C�Y
#define ACTION_MENU_HEIGHT	(200.0f)	 // ACTION MENU�̏c�T�C�Y

#define ACTION_MENU_TITLE_WIDTH	 (250.0f) // ACTION MENU TITLE �̉��T�C�Y
#define ACTION_MENU_TITLE_HEIGHT (50.0f)  // ACTION MENU TITLE �̏c�T�C�Y

#define OPTIONS_HEIGHT		(30.0f)		// �ݒ�̏c�T�C�Y
#define HP_BAR_HEIGHT		(15.0f)		// HP�o�[�̏c�T�C�Y

#define ACTION_IND_SIZE		(70.0f)		// ACTION_INDICATOR�̉摜�̃T�C�Y
#define SELECT_POINTER_SIZE	(50.0f)		// �^�[�Q�b�g���w���摜�̃T�C�Y
#define DAMAGE_SIZE			(100.0f)	// �_���[�W�̉摜�̃T�C�Y

#define DAMAGE_IMG_INCREASE_TIME (11)	// �_���[�W�̉摜���g�厞��
#define DAMAGE_IMG_DECREASE_TIME (100)	// �_���[�W�̉摜�̏k�����n�߂鎞��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
XMFLOAT3 ConvertirCoordenadas3Da2D(XMFLOAT3 pos3D);

void DrawActionMenu(void);

void OnClickButtonActionMenu(void);


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct SPRITE
{
	BOOL		use;		// TRUE:�g���Ă���  FALSE:���g�p
	float		w, h;		// ���ƍ���
	XMFLOAT3	pos;		// �|���S���̍��W
	int			texNo;		// �e�N�X�`���ԍ�

};

struct PLAYER_UI
{
	BOOL		use;		// TRUE:�g���Ă���  FALSE:���g�p
	float		w, h;		// ���ƍ���
	XMFLOAT3	pos;		// �|���S���̍��W
	int			texNo;		// �e�N�X�`���ԍ�

	SPRITE		Img;		//�v���C���[�̉摜
	SPRITE		HPbar;		//HP�o�[

	WCHAR		Name[256];	//�v���C���[��
	WCHAR		HP[256];	//HP�̕���
	WCHAR		HPnum[256];	//HP�̐���
};

struct ACTION_MENU
{
	BOOL	use;
	BOOL	moving;			//�����t���O
	SPRITE	leftBG;			//���o�b�N�O���E���h�摜
	SPRITE	MenuTitleBG;	//���j���[�^�C�g���摜
};

struct ACTION_MENU_OPTION
{
	BOOL		use;		// TRUE:�g���Ă���  FALSE:���g�p
	float		w, h;		// ���ƍ���
	XMFLOAT3	pos;		// �|���S���̍��W
	WCHAR		text[256];	// �A�N�V��������
};





//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/number16x32.png",			//0

	"data/TEXTURE/UIfight/Keyboard_Item.png",		//1
	"data/TEXTURE/UIfight/Keyboard_Other.png",		//2
	"data/TEXTURE/UIfight/Keyboard_Attack.png",		//3
	"data/TEXTURE/UIfight/Keyboard_Special.png",	//4
	"data/TEXTURE/UIfight/pointer.png",				//5

	"data/TEXTURE/Menu/fade_black.png",				//6
	"data/TEXTURE/Menu/fade_white.png",				//7

	"data/TEXTURE/UIfight/woodcutterImage2.png",	//8
	"data/TEXTURE/UIfight/GnomeImage3.png",			//9

	"data/TEXTURE/UIfight/DamageImage.png",			//10
	"data/TEXTURE/UIfight/ActionIndicator.png",		//11

	"data/TEXTURE/Menu/fade_white.png",				//12
	"data/TEXTURE/Menu/UIbgH.png",					//13
	"data/TEXTURE/Menu/UIbgV.png",					//14
	"data/TEXTURE/Menu/OptionsBG.png",				//15

};

static WCHAR* g_PlayerNames[MAX_PLAYER] = 
{
	L"�X�̎���",			
	L"�m�[��",			
	L"�m�[�}",			
};



static SPRITE		g_ActionButton;				//�A�N�V�����I�����̉摜
static int			g_ActionButtonSelected;		//�ǂ̃{�^����I������

static SPRITE		g_SelectionPointer;			//�^�[�Q�b�g���w���摜�i��̉摜�j

static SPRITE		g_ActionIndicator;			//�A�N�V�����\���Ԃ������摜�i�r�b�N���}�[�N�j

static SPRITE		g_Damage;					//�_���[�W�̉摜
static int			g_DamageNum;				//�_���[�W�l
static int			g_DamageTime;				//�_���[�W��\�����鎞�Ԃ̃J�E���^�[
static BOOL			g_CriticalDamage;			//�N���e�B�J���_���[�W�������t���O

static PLAYER_UI	g_PlayerUI[MAX_PLAYER];		//�v���C���[��UI

static ACTION_MENU	g_ActionMenu;				//�A�N�V�������j���[
static int			g_ActionMenuType;			//�A�N�V�������j���[�̃^�C�v

static ACTION_MENU_OPTION	g_SpecialsOptions[SP_MAX_OPTIONS];	//�X�y�V�����̑I����
static ACTION_MENU_OPTION	g_ItemsOptions	 [IT_MAX_OPTIONS];	//�A�C�e���̑I����
static ACTION_MENU_OPTION	g_OtherOptions	 [OT_MAX_OPTIONS];	//���̑��̑I����

static ACTION_MENU_OPTION*  g_ActualOptions;	//���̃A�N�V�������j���[
static int					g_ActualMaxOptions;	//���̃A�N�V�������j���[�̑I�����̐�

static int 			g_SelectedOption;			//�I�������A�N�V����

static char*		g_Font;						//�����t�H���g�i�����g�p�j

static BOOL			g_inputAllowed;				//�C���v�b�g�\�t���O

static BOOL			g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitFightUI(void)
{
	//SetFont(L"Showcard Gothic", 30.0f, D2D1::ColorF(D2D1::ColorF::White), ALIGNMENT_CENTER_CENTER);
	//g_Font = "Showcard Gothic";
	g_Font = "Yu Gothic UI";

	if (CheckGamepadConnection())
	{
		g_TexturName[1] = "data/TEXTURE/UIfight/Controller_Item.png";
		g_TexturName[2] = "data/TEXTURE/UIfight/Controller_Other.png";
		g_TexturName[3] = "data/TEXTURE/UIfight/Controller_Attack.png";
		g_TexturName[4] = "data/TEXTURE/UIfight/Controller_Special.png";
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



	// ACTION BUTTON�̏�����
	g_ActionButton.use		= FALSE;
	g_ActionButton.w		= BUTTON_WIDTH;
	g_ActionButton.h		= BUTTON_HEIGHT;
	g_ActionButton.pos		= { 0.0f, 0.0f, 0.0f };
	g_ActionButton.texNo	= 1;
	


	// SELECTION POINTER �̏�����
	g_SelectionPointer.use	 = FALSE;
	g_SelectionPointer.w	 = SELECT_POINTER_SIZE;
	g_SelectionPointer.h	 = SELECT_POINTER_SIZE;
	g_SelectionPointer.pos	 = { 0.0f, 0.0f, 0.0f };
	g_SelectionPointer.texNo = 5;


	// ACTION INDICATOR �̏�����
	g_ActionIndicator.use	= FALSE;
	g_ActionIndicator.w		= ACTION_IND_SIZE;
	g_ActionIndicator.h		= ACTION_IND_SIZE;
	g_ActionIndicator.pos	= { 0.0f, 0.0f, 0.0f };
	g_ActionIndicator.texNo = 11;


	// DAMAGE �̏�����
	g_Damage.use	= FALSE;
	g_Damage.w		= DAMAGE_SIZE;
	g_Damage.h		= DAMAGE_SIZE;
	g_Damage.pos	= { 0.0f, 0.0f, 0.0f };
	g_Damage.texNo	= 10;

	g_DamageNum		= 0;
	g_DamageTime	= 0;


	for (int i = 0; i < MAX_PLAYER; i++)
	{
		g_PlayerUI[i].use	= TRUE;
		g_PlayerUI[i].w		= PLAYER_UI_WIDTH;
		g_PlayerUI[i].h		= PLAYER_UI_HEIGHT;
		g_PlayerUI[0].pos	= { SCREEN_WIDTH - (g_PlayerUI[i].w / 2 + 10.0f) - (g_PlayerUI[i].w + 10.0f) * 1, SCREEN_HEIGHT - g_PlayerUI[i].h / 2 - 10.0f, 0.0f };
		g_PlayerUI[1].pos	= { SCREEN_WIDTH - (g_PlayerUI[i].w / 2 + 10.0f) - (g_PlayerUI[i].w + 10.0f) * 0, SCREEN_HEIGHT - g_PlayerUI[i].h / 2 - 10.0f, 0.0f };
		g_PlayerUI[2].pos	= { SCREEN_WIDTH - (g_PlayerUI[i].w / 2 + 10.0f) - (g_PlayerUI[i].w + 10.0f) * 2, SCREEN_HEIGHT - g_PlayerUI[i].h / 2 - 10.0f, 0.0f };
		g_PlayerUI[i].texNo = 6;

		g_PlayerUI[0].Img.texNo = 8;
		g_PlayerUI[1].Img.texNo = 9;
		g_PlayerUI[2].Img.texNo = 9;
		g_PlayerUI[i].Img.h		= g_PlayerUI[i].h - 20.0f;
		g_PlayerUI[i].Img.w		= g_PlayerUI[i].Img.h + 10.0f;
		g_PlayerUI[i].Img.pos	= g_PlayerUI[i].pos;
		g_PlayerUI[i].Img.pos.x += -g_PlayerUI[i].w / 2 + g_PlayerUI[i].Img.w / 2 + 10.0f;

		g_PlayerUI[i].HPbar.texNo = 7;
		g_PlayerUI[i].HPbar.pos	  = g_PlayerUI[i].pos;
		g_PlayerUI[i].HPbar.pos.x += -g_PlayerUI[i].w / 2 + g_PlayerUI[i].Img.w + 20.0f;
		g_PlayerUI[i].HPbar.pos.y += g_PlayerUI[i].h / 2 - 10.0f;
		g_PlayerUI[i].HPbar.h	  = HP_BAR_HEIGHT;
		g_PlayerUI[i].HPbar.w	  = g_PlayerUI[i].w - g_PlayerUI[i].Img.w - 30.0f;

	}


	//ACTION MENU
	g_ActionMenu.use			= FALSE;
	g_ActionMenu.moving			= FALSE;

	g_ActionMenu.leftBG.use		= TRUE;
	g_ActionMenu.leftBG.w		= ACTION_MENU_WIDTH;
	g_ActionMenu.leftBG.h		= ACTION_MENU_HEIGHT;
	g_ActionMenu.leftBG.pos		= XMFLOAT3(g_ActionMenu.leftBG.w / 2, g_ActionMenu.leftBG.h / 2, 0.0f);
	g_ActionMenu.leftBG.texNo	= 14;

	g_ActionMenu.MenuTitleBG.use = TRUE;
	g_ActionMenu.MenuTitleBG.pos = XMFLOAT3(0.0f, 75.0f, 0.0f);
	g_ActionMenu.MenuTitleBG.w   = ACTION_MENU_TITLE_WIDTH;
	g_ActionMenu.MenuTitleBG.h   = ACTION_MENU_TITLE_HEIGHT;
	g_ActionMenu.MenuTitleBG.texNo = 13;


	for (int i = 0; i < SP_MAX_OPTIONS; i++)
	{
		g_SpecialsOptions[i].use = TRUE;
		g_SpecialsOptions[i].pos = XMFLOAT3(0.0f, OPTIONS_TOP_MARGIN + OPTIONS_SPACE * i, 0.0f);
		g_SpecialsOptions[i].w   = g_ActionMenu.leftBG.w;
		g_SpecialsOptions[i].h   = OPTIONS_HEIGHT;
	}
	wcscpy_s(g_SpecialsOptions[0].text, 256, L"����U��");



	for (int i = 0; i < IT_MAX_OPTIONS; i++)
	{
		g_ItemsOptions[i].use = TRUE;
		g_ItemsOptions[i].pos = XMFLOAT3(0.0f, OPTIONS_TOP_MARGIN + OPTIONS_SPACE * i, 0.0f);
		g_ItemsOptions[i].w	  = g_ActionMenu.leftBG.w;
		g_ItemsOptions[i].h   = OPTIONS_HEIGHT;
	}
	wcscpy_s(g_ItemsOptions[0].text, 256, L"�����ӂ��₭�@��");
	wcscpy_s(g_ItemsOptions[1].text, 256, L"�����ӂ��₭�@��");
	wcscpy_s(g_ItemsOptions[2].text, 256, L"�����ӂ��₭�@��");
	//wcscpy_s(g_ItemsOptions[1].text, 256, L"�������₭");


	for (int i = 0; i < OT_MAX_OPTIONS; i++)
	{
		g_OtherOptions[i].use = TRUE;
		g_OtherOptions[i].pos = XMFLOAT3(0.0f, OPTIONS_TOP_MARGIN + OPTIONS_SPACE * i, 0.0f);
		g_OtherOptions[i].w   = g_ActionMenu.leftBG.w;
		g_OtherOptions[i].h   = OPTIONS_HEIGHT;
	}
	wcscpy_s(g_OtherOptions[0].text, 256, L"�ɂ���");


	g_SelectedOption = 0;

	g_ActualOptions = &g_OtherOptions[0];
	g_ActualMaxOptions = OT_MAX_OPTIONS;

	g_inputAllowed = TRUE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitFightUI(void)
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
void UpdateFightUI(void)
{
	if (g_ActionMenu.use && g_inputAllowed)
	{
		if (GetKeyboardTrigger(DIK_DOWN) || IsButtonTriggered(0, BUTTON_JS_DOWN) || IsButtonTriggered(0, BUTTON_ARROW_DOWN))
		{
			g_SelectedOption = (g_SelectedOption + 1) % g_ActualMaxOptions;		//�I�����Ă���{�^�����X�V
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}
		else if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_JS_UP) || IsButtonTriggered(0, BUTTON_ARROW_UP))
		{
			g_SelectedOption = g_SelectedOption ? --g_SelectedOption : g_ActualMaxOptions - 1;	//�I�����Ă���{�^����
			PlaySound(SOUND_LABEL_SE_ChangeButton);
		}

		//�{�^���̓�����s��
		if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_B))
		{
			OnClickButtonActionMenu();
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
		}
	}


	if (g_ActionMenu.moving)
	{

		//���E�̓���
		{
			static float pointerMoveCnt;
			float pointerMoveMax = 60;

			float angle = (XM_2PI / pointerMoveMax) * pointerMoveCnt;
			float x = cosf(angle);

			g_ActionMenu.leftBG.pos.x = g_ActionMenu.leftBG.w / 2 * -x;


			if (g_ActionMenu.use)
			{
				pointerMoveCnt++;
				if (pointerMoveCnt >= pointerMoveMax / 2)
				{
					g_ActionMenu.moving = FALSE;
				}
			}
			else
			{
				pointerMoveCnt--;
				if (pointerMoveCnt <= 0) g_ActionMenu.moving = FALSE;

			}
		}
	}



#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawFightUI(void)
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

	//ACTION BUTTON�̕`�揈��
	if(g_ActionButton.use)
	{
		for (int i = 0; i < 4; i++)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ActionButton.texNo + i]);

			
			// ����\�����錅�̐���

			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_ActionButton.pos.x;	// �X�R�A�̕\���ʒuX
			float py = g_ActionButton.pos.y;			// �X�R�A�̕\���ʒuY
			float pw = g_ActionButton.w;				// �X�R�A�̕\����
			float ph = g_ActionButton.h;				// �X�R�A�̕\������

			if (i == 0) py -= ACTBUTTON_Y_OFFSET;
			if (i == 1) py += ACTBUTTON_Y_OFFSET;
			if (i == 2) px += ACTBUTTON_X_OFFSET;
			if (i == 3) px -= ACTBUTTON_X_OFFSET;


			float tx = 0.0f;		// �e�N�X�`���̍���X���W
			float ty = 0.0f;		// �e�N�X�`���̍���Y���W
			float tw = 1.0f;		// �e�N�X�`���̕�
			float th = 1.0f;		// �e�N�X�`���̍���

			XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

			//�{�^�����I������Ă���ꍇ�A���̃{�^���𔼓����ɂ���B
			if (g_ActionButtonSelected != 0 && (g_ActionButtonSelected - 1) != i) color.w = 0.2f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, color);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}
	}


	//SELECTION POINTER �̕`�揈��
	if (g_SelectionPointer.use)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_SelectionPointer.texNo]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_SelectionPointer.pos.x;	// �X�R�A�̕\���ʒuX
		float py = g_SelectionPointer.pos.y;			// �X�R�A�̕\���ʒuY
		float pw = g_SelectionPointer.w;				// �X�R�A�̕\����
		float ph = g_SelectionPointer.h;				// �X�R�A�̕\������

		//���E�̓���
		{
			static float pointerMoveCnt;
			float pointerMoveMax = 60;

			float angle = (XM_2PI / pointerMoveMax) * pointerMoveCnt;
			float x = 10.0f * cosf(angle);

			px += x;

			pointerMoveCnt++;

			if (pointerMoveCnt >= pointerMoveMax) pointerMoveCnt = 0;

		}

		float tx = 0.0f;		// �e�N�X�`���̍���X���W
		float ty = 0.0f;		// �e�N�X�`���̍���Y���W
		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//ACTION INDICATOR �̕`�揈��
	if (g_ActionIndicator.use)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ActionIndicator.texNo]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_ActionIndicator.pos.x;	// �X�R�A�̕\���ʒuX
		float py = g_ActionIndicator.pos.y;			// �X�R�A�̕\���ʒuY
		float pw = g_ActionIndicator.w;				// �X�R�A�̕\����
		float ph = g_ActionIndicator.h;				// �X�R�A�̕\������


		float tx = 0.0f;		// �e�N�X�`���̍���X���W
		float ty = 0.0f;		// �e�N�X�`���̍���Y���W
		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	//DAMAGE �̕`�揈��
	if (g_Damage.use)
	{

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Damage.texNo]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Damage.pos.x;	// �X�R�A�̕\���ʒuX
		float py = g_Damage.pos.y;	// �X�R�A�̕\���ʒuY
		float pw = g_Damage.w;		// �X�R�A�̕\����
		float ph = g_Damage.h;		// �X�R�A�̕\������

		//�g��k���̓���
		if(g_DamageTime <= DAMAGE_IMG_INCREASE_TIME)
		{
			float pointerMoveMax = 30;	//�摜�̍ő�X�P�[��

			float angle = (XM_2PI / pointerMoveMax) * g_DamageTime;
			float scale = (cosf(angle - XM_PIDIV2) );
			
			pw *= scale;
			ph *= scale;
		}
		else if (g_DamageTime < DAMAGE_IMG_DECREASE_TIME)
		{
			pw *= 0.8f;
			ph *= 0.8f;
		}
		g_DamageTime++;


		float tx = 0.0f;		// �e�N�X�`���̍���X���W
		float ty = 0.0f;		// �e�N�X�`���̍���Y���W
		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		XMFLOAT4 color;
		if (g_CriticalDamage)color = XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);
		else				 color = XMFLOAT4(0.8f, 0.8f, 0.1f, 1.0f);
		
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		SetFont(L"Showcard Gothic", 30.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
		WCHAR HPstring[5]; 
		swprintf_s(HPstring, L"%d", g_DamageNum);
		DrawStringRect(HPstring, D2D1::RectF(px - pw + 2, py - ph + 2, px + pw + 2, py + ph + 2), D2D1_DRAW_TEXT_OPTIONS_NONE);
		DrawStringRect(HPstring, D2D1::RectF(px - pw - 2, py - ph - 2, px + pw - 2, py + ph - 2), D2D1_DRAW_TEXT_OPTIONS_NONE);
		////////////DrawTextRect(FormatToString("%d", g_DamageNum), 30, XMFLOAT4(0, 0, 0, 1),
		////////////	px - pw - 2, py - ph - 2, px + pw - 2, py + ph - 2, TEXT_ANCHOR_CENTER_CENTER, g_Font);


		if(g_CriticalDamage)SetFont(L"Showcard Gothic", 30.0f, D2D1::ColorF(D2D1::ColorF::Red), ALIGNMENT_CENTER_CENTER);
		else				SetFont(L"Showcard Gothic", 30.0f, D2D1::ColorF(D2D1::ColorF::White), ALIGNMENT_CENTER_CENTER);
		DrawStringRect(HPstring, D2D1::RectF(px - pw, py - ph, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);

		////////////if (g_CriticalDamage)DrawTextRect(FormatToString("%d", g_DamageNum), 30, XMFLOAT4(1, 0, 0, 1),
		////////////	px - pw + 2, py - ph + 2, px + pw + 2, py + ph + 2, TEXT_ANCHOR_CENTER_CENTER, g_Font);
		////////////else				 DrawTextRect(FormatToString("%d", g_DamageNum), 30, XMFLOAT4(1, 1, 1, 1),
		////////////	px - pw + 2, py - ph + 2, px + pw + 2, py + ph + 2, TEXT_ANCHOR_CENTER_CENTER, g_Font);


		if (g_DamageTime >= DAMAGE_IMG_DECREASE_TIME)
		{
			g_DamageTime	 = 0;
			g_Damage.use	 = FALSE;
			g_CriticalDamage = FALSE;
		}

	}


	//PLAYER UI �̕`�揈��
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_PlayerUI[i].use)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_PlayerUI[i].texNo]);

			// �ʒu��e�N�X�`���[���W�𔽉f
			float px = g_PlayerUI[i].pos.x;		// �X�R�A�̕\���ʒuX
			float py = g_PlayerUI[i].pos.y;		// �X�R�A�̕\���ʒuY
			float pw = g_PlayerUI[i].w;			// �X�R�A�̕\����
			float ph = g_PlayerUI[i].h;			// �X�R�A�̕\������

			float tx = 0.0f;		// �e�N�X�`���̍���X���W
			float ty = 0.0f;		// �e�N�X�`���̍���Y���W
			float tw = 1.0f;		// �e�N�X�`���̕�
			float th = 1.0f;		// �e�N�X�`���̍���

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.7f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);



			//PLAYER IMAGE
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_PlayerUI[i].Img.texNo]);

			px = g_PlayerUI[i].Img.pos.x;		// �X�R�A�̕\���ʒuX
			py = g_PlayerUI[i].Img.pos.y;		// �X�R�A�̕\���ʒuY
			pw = g_PlayerUI[i].Img.w;			// �X�R�A�̕\����
			ph = g_PlayerUI[i].Img.h;			// �X�R�A�̕\������

			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);
			GetDeviceContext()->Draw(4, 0);


			//PLAYER HP BAR
			PLAYER* player = GetPlayer();
			//WHITE
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_PlayerUI[i].HPbar.texNo]);

			px = g_PlayerUI[i].HPbar.pos.x;		// �X�R�A�̕\���ʒuX
			py = g_PlayerUI[i].HPbar.pos.y;		// �X�R�A�̕\���ʒuY
			pw = g_PlayerUI[i].HPbar.w;			// �X�R�A�̕\����
			ph = g_PlayerUI[i].HPbar.h;			// �X�R�A�̕\������

			SetSpriteLBColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
			GetDeviceContext()->Draw(4, 0);

			//LETERS
			SetFont(L"Showcard Gothic", 15.0f, D2D1::ColorF(D2D1::ColorF::White), ALIGNMENT_TOP_LEFT);
			DrawStringRect(g_PlayerNames[i], D2D1::RectF(px, py - g_PlayerUI[i].h + g_PlayerUI[i].HPbar.h, px + pw, py+100), D2D1_DRAW_TEXT_OPTIONS_NONE);
			//////////DrawTextRect(FormatToString(g_PlayerNames[i]), 15, XMFLOAT4(1, 1, 1, 1),
			//////////	px, py - g_PlayerUI[i].h + g_PlayerUI[i].HPbar.h, px + pw, py + 100, TEXT_ANCHOR_TOP_LEFT, g_Font);

			SetFont(L"Showcard Gothic", 15.0f, D2D1::ColorF(D2D1::ColorF::White), ALIGNMENT_CENTER_LEFT);
			DrawStringRect(L"HP", D2D1::RectF(px, g_PlayerUI[i].pos.y, px + pw, g_PlayerUI[i].pos.y), D2D1_DRAW_TEXT_OPTIONS_NONE);
			//////////DrawTextRect(FormatToString("HP"), 15, XMFLOAT4(1, 1, 1, 1),
			//////////	px, g_PlayerUI[i].pos.y + 100, px + pw, g_PlayerUI[i].pos.y - 100, TEXT_ANCHOR_CENTER_LEFT, g_Font);


			SetFont(L"Showcard Gothic", 20.0f, D2D1::ColorF(D2D1::ColorF::White), ALIGNMENT_CENTER_RIGHT);
			WCHAR HPstring[25]; // Ajusta el tamano segun tus necesidades
			swprintf_s(HPstring, L"%d%c%d", player[i].HPcur, '/', player[i].HPmax);

			DrawStringRect(HPstring, D2D1::RectF(px, g_PlayerUI[i].pos.y, px + pw, g_PlayerUI[i].pos.y), D2D1_DRAW_TEXT_OPTIONS_NONE);

			//////////DrawTextRect(FormatToString("%d%c%d", player[i].HPcur, '/', player[i].HPmax), 15, XMFLOAT4(1, 1, 1, 1),
			//////////	px, g_PlayerUI[i].pos.y + 100, px + pw, g_PlayerUI[i].pos.y - 100, TEXT_ANCHOR_CENTER_RIGHT, g_Font);


			//BLACK
			px += 1.0f;		// �X�R�A�̕\���ʒuX
			py -= 1.0f;		// �X�R�A�̕\���ʒuY
			pw -= 2.0f;		// �X�R�A�̕\����
			ph -= 2.0f;		// �X�R�A�̕\������

			SetSpriteLBColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
			GetDeviceContext()->Draw(4, 0);

			//BLUE
			pw = (pw / player[i].HPmax) * player[i].HPcur;			// �X�R�A�̕\����

			SetSpriteLBColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, XMFLOAT4(0.3f, 0.3f, 1.0f, 1.0f));
			GetDeviceContext()->Draw(4, 0);
		}

	}


	DrawActionMenu();

}

void DrawActionMenu(void)
{
	if (g_ActionMenu.moving || g_ActionMenu.use)
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


		////Black screen  
		//{
		//	// �e�N�X�`���ݒ�
		//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		//	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//	SetSpriteLTColor(g_VertexBuffer, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, g_ScreenColor);

		//	// �|���S���`��
		//	GetDeviceContext()->Draw(4, 0);
		//}

		//*******************
		// leftBG��`��
		//*******************
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ActionMenu.leftBG.texNo]);

		//�w�i�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_ActionMenu.leftBG.pos.x;	// �\���ʒuX
		float py = g_ActionMenu.leftBG.pos.y;	// �\���ʒuY
		float pw = g_ActionMenu.leftBG.w;		// �\����
		float ph = g_ActionMenu.leftBG.h;		// �\������

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
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ActionMenu.MenuTitleBG.texNo]);

		////�ʒu���W�𔽉f
		//px = g_ActionMenu.MenuTitleBG.pos.x;	// �\���ʒuX
		//py = g_ActionMenu.MenuTitleBG.pos.y;	// �\���ʒuY
		//pw = g_ActionMenu.MenuTitleBG.w;		// �\����
		//ph = g_ActionMenu.MenuTitleBG.h;		// �\������

		//// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//px += g_ActionMenu.leftBG.pos.x;
		//SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th/*, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)*/);

		//// �|���S���`��
		//GetDeviceContext()->Draw(4, 0);

		//SetFont(L"Showcard Gothic", 35.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
		//DrawStringRect(L"PAUSE", D2D1::RectF(px - pw / 2, py - ph / 2, px + pw / 2, py + ph / 2), D2D1_DRAW_TEXT_OPTIONS_NONE);


		//*******************
		// optionBG��`��
		//*******************
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[15]);

		//�ʒu���W�𔽉f
		px = g_ActualOptions[g_SelectedOption].pos.x;	// �\���ʒuX
		py = g_ActualOptions[g_SelectedOption].pos.y;	// �\���ʒuY
		pw = g_ActualOptions[g_SelectedOption].w;		// �\����
		ph = g_ActualOptions[g_SelectedOption].h;		// �\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		px += g_ActionMenu.leftBG.pos.x;
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		SetFont(L"HG�n�p�p�߯�ߑ�", 20.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);

		for (int i = 0; i < g_ActualMaxOptions; i++)
		{
			DrawStringRect(g_ActualOptions[i].text, D2D1::RectF(px - pw, g_ActualOptions[i].pos.y - ph, px + pw, g_ActualOptions[i].pos.y + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);

		}

		////////for (int i = 0; i < g_ActualMaxOptions; i++)
		////////{
		////////	DrawTextRect(FormatToString(g_ActualOptions[i].text), 20, XMFLOAT4(0, 0, 0, 1),
		////////		px - pw, g_ActualOptions[i].pos.y - ph, px + pw, g_ActualOptions[i].pos.y + ph, TEXT_ANCHOR_CENTER_CENTER, "HG�n�p�p�߯�ߑ�");
		////////}

	}
}

BOOL GetActionMenuUse(void)
{
	return g_ActionMenu.use;
}

void SetActionsMenu(BOOL flag, int OptionType)
{
	if (!flag && g_ActionMenu.use) g_ActionMenu.use = flag;
	else if(!flag && !g_ActionMenu.use) return;

	g_ActionMenu.use = flag;
	g_ActionMenu.moving = TRUE;
	g_inputAllowed = flag ? TRUE : FALSE;

	switch (OptionType)
	{
	case BUTTON_UP_ITEM:
		g_ActualOptions		= &g_ItemsOptions[0];
		g_ActualMaxOptions	= IT_MAX_OPTIONS;
		g_SelectedOption	= 0;
		g_ActionMenuType	= BUTTON_UP_ITEM;
		break;

	case BUTTON_DOWN_OTHER:
		g_ActualOptions		= &g_OtherOptions[0];
		g_ActualMaxOptions	= OT_MAX_OPTIONS;
		g_SelectedOption	= 0;
		g_ActionMenuType	= BUTTON_DOWN_OTHER;
		break;

	case BUTTON_LEFT_SPECIAL:
		g_ActualOptions		= &g_SpecialsOptions[0];
		g_ActualMaxOptions	= SP_MAX_OPTIONS;
		g_SelectedOption	= 0;
		g_ActionMenuType	= BUTTON_LEFT_SPECIAL;
		break;


	default:
		break;
	}
}

void SetActionButton(XMFLOAT3 playerPos, BOOL flag)
{
	XMFLOAT3 pos = playerPos;
	pos.y += 15.0f;
	g_ActionButton.pos = ConvertirCoordenadas3Da2D(pos);
	g_ActionButton.use = flag;
}


void SetSelectionPointer(XMFLOAT3 targetPos, BOOL flag)
{
	XMFLOAT3 pos = targetPos;
	pos.y += 15.0f;
	g_SelectionPointer.pos = ConvertirCoordenadas3Da2D(pos);
	g_SelectionPointer.pos.x -= 80.0f;
	g_SelectionPointer.use = flag;
}

//�^�[�Q�b�g���w���摜���\������Ă��邩�ǂ����A���ׂ�֐�
BOOL GetSelectionPointerState(void)
{
	return g_SelectionPointer.use;
}


void SetActionIndicator(XMFLOAT3 targetPos, BOOL flag)
{
	XMFLOAT3 pos = targetPos;
	pos.y += 15.0f;
	g_ActionIndicator.pos = ConvertirCoordenadas3Da2D(pos);
	g_ActionIndicator.pos.x -= 0.0f;
	g_ActionIndicator.pos.y -= 100.0f;
	g_ActionIndicator.use = flag;
}


void SetDamageUI(XMFLOAT3 targetPos, int damage, BOOL flag, BOOL critical)
{
	XMFLOAT3 pos = targetPos;
	pos.y += 15.0f;
	g_Damage.pos	= ConvertirCoordenadas3Da2D(pos);
	g_Damage.pos.x += 80.0f;

	g_DamageNum		= damage;
	g_Damage.use	= flag;

	g_DamageTime = 0;
	g_CriticalDamage = critical;
}


void SetButtonSelected(int buttonSelected)
{
	g_ActionButtonSelected = buttonSelected;
}


// 3D���W���X�N���[�����W�ɕϊ�����֐�
XMFLOAT3 ConvertirCoordenadas3Da2D(XMFLOAT3 pos3D)
{
	CAMERA* camera = GetCamera();
	
	XMMATRIX mtxView		= XMLoadFloat4x4(&camera->mtxView);
	XMMATRIX mtxProjection	= XMLoadFloat4x4(&camera->mtxProjection);


	XMVECTOR worldPosition = XMVectorSet(pos3D.x, pos3D.y, pos3D.z, 1.0f);
	XMVECTOR screenPosition = XMVector3TransformCoord(worldPosition, mtxView);
	screenPosition = XMVector3TransformCoord(screenPosition, mtxProjection);

	// ���K��
	screenPosition = screenPosition / screenPosition.m128_f32[3];

	// �s�N�Z���ɕϊ�
	float screenX = 0.5f * (screenPosition.m128_f32[0] + 1.0f) * SCREEN_WIDTH;
	float screenY = 0.5f * (1.0f - screenPosition.m128_f32[1]) * SCREEN_HEIGHT;

	return XMFLOAT3(screenX, screenY, 0.0f);
}


//=============================================================================
// OnClick Button ��`��
//=============================================================================
void OnClickButtonActionMenu(void)
{
	g_inputAllowed = FALSE; 

	switch (g_ActionMenuType)
	{
	case BUTTON_UP_ITEM:
		SetSelectionPlayer();
		break;

	case BUTTON_DOWN_OTHER:
		SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_GAME);
		SetSelectionPointer(XMFLOAT3(0.0f, 0.0f, 0.0f), FALSE);	// Reset Pointer
		SetButtonSelected(0);
		SetActionButton(XMFLOAT3(0.0f, 0.0f, 0.0f), FALSE);			// Reset Action Button
		SetActionsMenu(FALSE, ACTION_BUTTONS_OFF);

		break;

	case BUTTON_LEFT_SPECIAL:
		SetSelectionEnemy();
		break;


	default:
		break;
	}
}

int GetSelecteOption(void)
{
	return g_SelectedOption;
}