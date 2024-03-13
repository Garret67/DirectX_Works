//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "interface.h"
#include "input.h"

#include "write.h"
#include "FBXmodel.h"
#include "FBXanimation.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define BUTTON_WIDTH			(100.0f)
#define BUTTON_HEIGHT			(30.0f)
#define HIDE_OBJ_BUTTON_WIDTH	(120.0f)

#define ANIMATION_ZONE_HEIGHT	(100.0f)

#define OFFSET					(SCREEN_WIDTH / 2.0f)

#define HIDE_BUTTON_MAX			(9)

#define MARGIN_HORIZONTAL		(30.0f)

#define ZONES_FRAME_WIDTH		(OFFSET - MARGIN_HORIZONTAL * 2 + 10.0f)

#define SAVED_MESSAGE_TIME		(240.0f)

#define COLOR_WHITE				(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
#define COLOR_GREY				(XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f))
#define COLOR_DARK_GREY			(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f))
#define COLOR_BLACK				(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
#define COLOR_YELLOW			(XMFLOAT4(0.9f, 0.9f, 0.0f, 1.0f))
#define COLOR_DARK_YELLOW		(XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f))
#define COLOR_RED				(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f))
#define COLOR_DARK_RED			(XMFLOAT4(0.7f, 0.0f, 0.0f, 1.0f))
#define COLOR_GREEN				(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f))
#define COLOR_DARK_GREEN		(XMFLOAT4(0.0f, 0.7f, 0.0f, 1.0f))
#define COLOR_CYAN				(XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f))



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DrawModelZone(void);			//���f���]�[���̕`�揈��
void DrawHideObjZone(void);			//��\���{�^���]�[���̕`�揈��
void DrawAnimationZone(void);		//�A�j���[�V�����]�[���̕`��
void DrawAddAnimationZone(void);	//�ǉ����폜�A�j���[�V�����]�[���̕`�揈��
void SaveLoadButton(void);			//�Z�[�u�ƃ��[�h�{�^���̕`�揈��

void UpdateSpeedText(WCHAR* text, float speed);		//�A�j���[�V�������x�̕�������X�V

BOOL CollisionMouse(XMFLOAT3 buttonPos, float ButtonW, float ButtonH, float mouseX, float mouseY);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[INTERFACE_TEX_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[INTERFACE_TEX_MAX] =
{
	"data/TEXTURE/fade_white.png",
	"data/TEXTURE/TitleBG.png",
	"data/TEXTURE/ovalo.png",
};


static BOOL				g_bg_Use;							// TRUE:�g���Ă���  FALSE:���g�p
static float			g_bg_w, g_bg_h;						// ���ƍ���
static XMFLOAT3			g_bg_Pos;							// �|���S���̍��W
static int				g_bg_TexNo;							// �e�N�X�`���ԍ�

BUTTON					g_ModelSelectButton;				//���f����ǂݍ��݃{�^��
HIDE_OBJ_BUTTON			g_HideObjButton[HIDE_BUTTON_MAX];	//��\���{�^��

ANIMATION_ZONE			g_AnimationZone;							//�A�j���[�V������
ADD_DEL_ZONE			g_AddAnimationZone;							//�A�j���[�V�����ǉ���
BUTTON					g_AnimIndexButtons[MODEL_ANIMATION_MAX];	//�A�j���[�V�����i���o�[�I���{�^��

BUTTON					g_SaveButton;					//�Z�[�u�{�^��
BUTTON					g_LoadButton;					//���[�h�{�^��

static int				g_ActiveAnimNum;				//animaciones que se ven a la vez
static int				g_ShowingAnim;					//���ݕ\������Ă���A�j���[�V����

static float			g_SavedMessageTimeCnt;			//�Z�[�u�������b�Z�[�W���ԃJ�E���^�[

static BOOL				g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitInterface(void)
{
	InitDirectWrite();
	//SetFont(L"Showcard Gothic", 30.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
	SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);


	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < INTERFACE_TEX_MAX; i++)
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


	g_ActiveAnimNum = 0;
	g_ShowingAnim = 0;

	g_SavedMessageTimeCnt = SAVED_MESSAGE_TIME;

	// �ϐ��̏�����
	g_bg_Use	= TRUE;
	g_bg_w		= SCREEN_CENTER_X;
	g_bg_h		= SCREEN_HEIGHT;
	g_bg_Pos	= XMFLOAT3(g_bg_w, 0.0f, 0.0f);
	g_bg_TexNo	= 0;

	//���f���I���̏�����
	g_ModelSelectButton.pos			= XMFLOAT3(OFFSET + MARGIN_HORIZONTAL, 50.0f, 0.0f);	// ���S�_����\��
	g_ModelSelectButton.w			= BUTTON_WIDTH;
	g_ModelSelectButton.h			= BUTTON_HEIGHT;
	g_ModelSelectButton.texNo		= 0;
	g_ModelSelectButton.selected	= FALSE;
	g_ModelSelectButton.color		= COLOR_WHITE;



	//�Z�[�u�{�^��
	g_SaveButton.pos		= XMFLOAT3(SCREEN_WIDTH - BUTTON_WIDTH - 100.0f, SCREEN_HEIGHT- BUTTON_HEIGHT - 20.0f, g_AddAnimationZone.pos.z);
	g_SaveButton.w			= BUTTON_WIDTH;
	g_SaveButton.h			= BUTTON_HEIGHT;
	g_SaveButton.texNo		= 0;
	g_SaveButton.selected	= FALSE;
	g_SaveButton.color		= COLOR_WHITE;
	wcscpy_s(g_SaveButton.text, 256, L"Save");


	//���[�h�{�^��
	g_LoadButton.pos = XMFLOAT3(OFFSET + 100.0f, SCREEN_HEIGHT - BUTTON_HEIGHT - 20.0f, g_AddAnimationZone.pos.z);
	g_LoadButton.w = BUTTON_WIDTH;
	g_LoadButton.h = BUTTON_HEIGHT;
	g_LoadButton.texNo = 0;
	g_LoadButton.selected = FALSE;
	g_LoadButton.color = COLOR_WHITE;
	wcscpy_s(g_LoadButton.text, 256, L"Load");


	//ANIMATION SELECT BUTTONS
	for (int i = 0; i < MODEL_ANIMATION_MAX; i++)
	{
		g_AnimIndexButtons[i].pos	= XMFLOAT3(g_ModelSelectButton.pos.x + 40 * (i % 10), SCREEN_CENTER_Y + (25.0f * (int)(i / 10)) + 10.0f, 0.0f);
		g_AnimIndexButtons[i].w		= 30.0f;
		g_AnimIndexButtons[i].h		= 20.0f;
		g_AnimIndexButtons[i].texNo	= 0;
		g_AnimIndexButtons[i].selected = FALSE;
		g_AnimIndexButtons[i].color	= COLOR_WHITE;
		//wcscpy_s(g_AnimIndexButtons.text, 256, L"Save");;

		WCHAR animationNum[3];
		swprintf_s(animationNum, L"%d", i + 1);

		wcscpy(g_AnimIndexButtons[i].text, animationNum);

	}
		
	//�A�j���[�V������ǉ����폜����]�[���̏�����
	g_AddAnimationZone.pos = XMFLOAT3(g_AnimIndexButtons[0].pos.x + 200.0f, g_AnimIndexButtons[0].pos.y - 25.0f, 0.0f);
	g_AddAnimationZone.w = BUTTON_WIDTH;
	g_AddAnimationZone.h = 20;
	g_AddAnimationZone.texNo = 0;
	for (int i = 0; i < 2; i++)
	{
		g_AddAnimationZone.button[i].pos = XMFLOAT3(g_AddAnimationZone.pos.x + (BUTTON_WIDTH + 10) * i, g_AddAnimationZone.pos.y, g_AddAnimationZone.pos.z);
		g_AddAnimationZone.button[i].w = BUTTON_WIDTH;
		g_AddAnimationZone.button[i].h = 20;
		g_AddAnimationZone.button[i].texNo = 0;
		g_AddAnimationZone.button[i].selected = FALSE;
		g_AddAnimationZone.button[i].color = COLOR_WHITE;

		switch (i)
		{
		case 0:
			wcscpy_s(g_AddAnimationZone.button[i].text, 256, L"Add Animation");
			break;

		case 1:
			wcscpy_s(g_AddAnimationZone.button[i].text, 256, L"Delete Animation");
			break;

		default:
			wcscpy_s(g_AddAnimationZone.button[i].text, 256, L"Undefined");
			break;
		}

	}



	//�A�j���[�V�����̃]�[���̏�����
	g_AnimationZone.use		= FALSE;
	g_AnimationZone.pos		= XMFLOAT3(OFFSET + MARGIN_HORIZONTAL, SCREEN_CENTER_Y + 100, 0.0f);
	g_AnimationZone.w		= OFFSET - MARGIN_HORIZONTAL * 2;
	g_AnimationZone.h		= ANIMATION_ZONE_HEIGHT;
	g_AnimationZone.texNo	= 1;
	g_AnimationZone.play	= FALSE;
	g_AnimationZone.loop	= TRUE;
	g_AnimationZone.speed	= 1.0f;

	g_AnimationZone.AnimSelectButton.pos	 = g_AnimationZone.pos;
	g_AnimationZone.AnimSelectButton.w		 = BUTTON_WIDTH;
	g_AnimationZone.AnimSelectButton.h		 = BUTTON_HEIGHT;
	g_AnimationZone.AnimSelectButton.texNo	 = 0;
	g_AnimationZone.AnimSelectButton.selected = FALSE;
	g_AnimationZone.AnimSelectButton.color	 = COLOR_WHITE;

	for (int j = 0; j < ANIMATION_ZONE_BUTTON_MAX; j++)
	{	
		g_AnimationZone.button[j].pos.x		= g_AnimationZone.pos.x + (g_AnimationZone.w / 3) * j;
		g_AnimationZone.button[j].pos.y		= g_AnimationZone.pos.y + 35.0f;
		g_AnimationZone.button[j].w			= BUTTON_WIDTH;
		g_AnimationZone.button[j].pos.z		= 0.0f;
		g_AnimationZone.button[j].h			= BUTTON_HEIGHT;
		g_AnimationZone.button[j].texNo		= 0;
		g_AnimationZone.button[j].selected	= FALSE;
		g_AnimationZone.button[j].color		= COLOR_WHITE;

		switch (j)
		{
		case BUTTON_PLAY:
			wcscpy_s(g_AnimationZone.button[j].text, 256, L"Play");
			break;

		case BUTTON_LOOP:
			wcscpy_s(g_AnimationZone.button[j].text, 256, L"Loop");
			break;

		case BUTTON_SPEED:
			wcscpy_s(g_AnimationZone.button[j].text, 256, L"Speed: 1.0");
			g_AnimationZone.button[j].w -= BUTTON_HEIGHT / 2;		////�|�Ɓ{�{�^���̃X�y�[�X�����
			break;

		case BUTTON_SPEED_PLUS:
			wcscpy_s(g_AnimationZone.button[j].text, 256, L"+");
			g_AnimationZone.button[BUTTON_SPEED_PLUS].h = BUTTON_HEIGHT / 2;
			g_AnimationZone.button[BUTTON_SPEED_PLUS].w = g_AnimationZone.button[BUTTON_SPEED_PLUS].h;
			g_AnimationZone.button[BUTTON_SPEED_PLUS].pos = XMFLOAT3(g_AnimationZone.button[BUTTON_SPEED].pos.x + BUTTON_WIDTH - BUTTON_HEIGHT / 2,
				g_AnimationZone.button[BUTTON_SPEED].pos.y,
				g_AnimationZone.button[BUTTON_SPEED].pos.z);
			break;
		case BUTTON_SPEED_MINUS:
			wcscpy_s(g_AnimationZone.button[j].text, 256, L"-");
			g_AnimationZone.button[BUTTON_SPEED_MINUS].h = g_AnimationZone.button[BUTTON_SPEED_PLUS].h;
			g_AnimationZone.button[BUTTON_SPEED_MINUS].w = g_AnimationZone.button[BUTTON_SPEED_PLUS].h;
			g_AnimationZone.button[BUTTON_SPEED_MINUS].pos = XMFLOAT3(g_AnimationZone.button[BUTTON_SPEED].pos.x + BUTTON_WIDTH - BUTTON_HEIGHT / 2,
				g_AnimationZone.button[BUTTON_SPEED].pos.y + g_AnimationZone.button[BUTTON_SPEED_MINUS].h,
				g_AnimationZone.button[BUTTON_SPEED].pos.z);
			break;


		case BUTTON_TRANSITION_FRAMES:
			wcscpy_s(g_AnimationZone.button[j].text, 256, L"Transition frames: ??");
			g_AnimationZone.button[j].w -= BUTTON_HEIGHT / 2;		////�|�Ɓ{�{�^���̃X�y�[�X�����
			g_AnimationZone.button[j].pos.x = g_AnimationZone.pos.x;
			g_AnimationZone.button[j].pos.y = g_AnimationZone.pos.y + 70.0f;
			break;

		case BUTTON_TRANSITION_FRAMES_PLUS:
			wcscpy_s(g_AnimationZone.button[j].text, 256, L"+");
			g_AnimationZone.button[j].h = BUTTON_HEIGHT / 2;
			g_AnimationZone.button[j].w = g_AnimationZone.button[j].h;
			g_AnimationZone.button[j].pos = XMFLOAT3(g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].pos.x + g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].w,
				g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].pos.y,
				g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].pos.z);
			break;
		case BUTTON_TRANSITION_FRAMES_MINUS:
			wcscpy_s(g_AnimationZone.button[j].text, 256, L"-");
			g_AnimationZone.button[j].h = BUTTON_HEIGHT / 2;
			g_AnimationZone.button[j].w = g_AnimationZone.button[j].h;
			g_AnimationZone.button[j].pos = XMFLOAT3(g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].pos.x + g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].w,
				g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].pos.y + g_AnimationZone.button[j].h,
				g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].pos.z);

			break;


		case BUTTON_NEXT_ANIM:
			wcscpy_s(g_AnimationZone.button[j].text, 256, L"Next animation: Undefined");
			g_AnimationZone.button[j].w = 280.0f;
			g_AnimationZone.button[j].pos.x = g_AnimationZone.pos.x + 120.0f ;
			g_AnimationZone.button[j].pos.y = g_AnimationZone.pos.y + 70.0f;
			break;

		case BUTTON_NEXT_ANIM_PLUS:
			wcscpy_s(g_AnimationZone.button[j].text, 256, L"+");
			g_AnimationZone.button[j].h = BUTTON_HEIGHT / 2;
			g_AnimationZone.button[j].w = g_AnimationZone.button[j].h;
			g_AnimationZone.button[j].pos = XMFLOAT3(g_AnimationZone.button[BUTTON_NEXT_ANIM].pos.x + g_AnimationZone.button[BUTTON_NEXT_ANIM].w,
				g_AnimationZone.button[BUTTON_NEXT_ANIM].pos.y,
				g_AnimationZone.button[BUTTON_NEXT_ANIM].pos.z);
			break;
		case BUTTON_NEXT_ANIM_MINUS:
			wcscpy_s(g_AnimationZone.button[j].text, 256, L"-");
			g_AnimationZone.button[j].h = BUTTON_HEIGHT / 2;
			g_AnimationZone.button[j].w = g_AnimationZone.button[j].h;
			g_AnimationZone.button[j].pos = XMFLOAT3(g_AnimationZone.button[BUTTON_NEXT_ANIM].pos.x + g_AnimationZone.button[BUTTON_NEXT_ANIM].w,
				g_AnimationZone.button[BUTTON_NEXT_ANIM].pos.y + g_AnimationZone.button[j].h,
				g_AnimationZone.button[BUTTON_NEXT_ANIM].pos.z);

			break;

		default:
			wcscpy_s(g_AnimationZone.button[j].text, 256, L"Undefined");
			break;
		}
	}




	//�A�j���[�V�����̃]�[���̏�����
	for (int i = 0; i < HIDE_BUTTON_MAX; i++)
	{
		g_HideObjButton[i].pos.x = g_ModelSelectButton.pos.x + ((OFFSET - MARGIN_HORIZONTAL * 2) / (3)) * (i % 3);
		g_HideObjButton[i].pos.y = g_ModelSelectButton.pos.y + 35 * (i / 3) + 80;
		g_HideObjButton[i].pos.z = 0.0f;
		g_HideObjButton[i].w	 = HIDE_OBJ_BUTTON_WIDTH;
		g_HideObjButton[i].h	 = BUTTON_HEIGHT;
		g_HideObjButton[i].texNo = 0;
		g_HideObjButton[i].color = COLOR_WHITE;
		wcscpy_s(g_HideObjButton[i].text, 256, L"No obj");

		for (int j = 0; j < 2; j++)
		{																														//�|�Ɓ{�{�^��������
			g_HideObjButton[i].button[j].pos.x		= g_HideObjButton[i].pos.x + HIDE_OBJ_BUTTON_WIDTH;
			g_HideObjButton[i].button[j].pos.y		= g_HideObjButton[i].pos.y;
			g_HideObjButton[i].button[j].pos.z		= 0.0f;
			g_HideObjButton[i].button[j].w			= BUTTON_HEIGHT / 2;
			g_HideObjButton[i].button[j].h			= BUTTON_HEIGHT / 2;
			g_HideObjButton[i].button[j].texNo		= 0;
			g_HideObjButton[i].button[j].selected	= FALSE;
			g_HideObjButton[i].button[j].color		= COLOR_WHITE;

			switch (j)
			{

			case 0:
				wcscpy_s(g_HideObjButton[i].button[j].text, 256, L"+");

				break;
			case 1:
				wcscpy_s(g_HideObjButton[i].button[j].text, 256, L"-");
				g_HideObjButton[i].button[j].pos.y += BUTTON_HEIGHT / 2;

				break;

			default:
				wcscpy_s(g_HideObjButton[i].button[j].text, 256, L"Undefined");
				break;
			}
		}


	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitInterface(void)
{
	UninitDirectWrite();

	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < INTERFACE_TEX_MAX; i++)
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
void UpdateInterface(void)
{
	float MouseX = (float)GetMousePosX();
	float MouseY = (float)GetMousePosY();

	//���f���I���̍X�V����
	{
		g_ModelSelectButton.selected = CollisionMouse(g_ModelSelectButton.pos, g_ModelSelectButton.w, g_ModelSelectButton.h, MouseX, MouseY);

		if (g_ModelSelectButton.selected)
		{
			g_ModelSelectButton.color = COLOR_GREY;
			if (IsMouseLeftTriggered())
			{
				LoadModelButton(g_ModelSelectButton.text);
			}
		}
		else
		{
			g_ModelSelectButton.color = COLOR_WHITE;
		}
	}

	//HIDE OBJECT �̍X�V����
	{
		for (int i = 0; i < HIDE_BUTTON_MAX; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				g_HideObjButton[i].button[j].selected = CollisionMouse(g_HideObjButton[i].button[j].pos, g_HideObjButton[i].button[j].w, g_HideObjButton[i].button[j].h, MouseX, MouseY);

				if (g_HideObjButton[i].button[j].selected)
				{
					g_HideObjButton[i].button[j].color = COLOR_GREY;

					if (IsMouseLeftTriggered() && wcscmp(g_AnimationZone.AnimSelectButton.text, L"") != 0 &&
					wcscmp(g_AnimationZone.AnimSelectButton.text, L"Invalid Model File") != 0 &&
					wcscmp(g_AnimationZone.AnimSelectButton.text, L"Model File not found") != 0)
					{
						switch (j)
						{
						case 0:
							HideObjButton(i, g_HideObjButton[i].text, 1);
							break;

						case 1:
							HideObjButton(i, g_HideObjButton[i].text, -1);
							break;

						default:
							break;
						}

					}
				}
				else
				{
					g_HideObjButton[i].button[j].color = COLOR_WHITE;
				}
			}
		}
	}



	//�A�j���[�V������ǉ�����]�[���̍X�V����
	for (int j = 0; j < 2; j++)	//�ǉ��ƍ폜�{�^�����m�F���邽�߂�2�񃋁[�v
	{
		//�A�j���[�V�����I���̍X�V����
		g_AddAnimationZone.button[j].selected = CollisionMouse(g_AddAnimationZone.button[j].pos, g_AddAnimationZone.button[j].w, g_AddAnimationZone.button[j].h, MouseX, MouseY);


		if (g_AddAnimationZone.button[j].selected)
		{
			g_AddAnimationZone.button[j].color = COLOR_GREY;
			if (IsMouseLeftTriggered())
			{
				switch (j)
				{
				case BUTTON_ADD_ANIMATION:	//�A�j���[�V������ǉ�����{�^��
					if (g_ActiveAnimNum < MODEL_ANIMATION_MAX)
					{
						g_ActiveAnimNum++;
						g_ShowingAnim = g_ActiveAnimNum - 1;
						UpdateAnimationZone(g_ShowingAnim, g_ActiveAnimNum, g_AnimationZone.loop, g_AnimationZone.speed, g_AnimationZone.AnimSelectButton.text, g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].text, g_AnimationZone.button[BUTTON_NEXT_ANIM].text);
						UpdateSpeedText(g_AnimationZone.button[BUTTON_SPEED].text, g_AnimationZone.speed);
					}

					break;

				case BUTTON_DELETE_ANIMATION:	//�A�j���[�V�������폜�{�^��
					if (g_ActiveAnimNum > 0)
					{

						DeleteAnimation(g_ShowingAnim, g_ActiveAnimNum);
						mbstowcs(g_AnimationZone.AnimSelectButton.text, "", 256);
						g_ActiveAnimNum--;

						g_ShowingAnim = 0;
						UpdateAnimationZone(g_ShowingAnim, g_ActiveAnimNum, g_AnimationZone.loop, g_AnimationZone.speed, g_AnimationZone.AnimSelectButton.text, g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].text, g_AnimationZone.button[BUTTON_NEXT_ANIM].text);
						UpdateSpeedText(g_AnimationZone.button[BUTTON_SPEED].text, g_AnimationZone.speed);
					}

					break;
				default:
					break;
				}
			}
		}
		else
		{
			g_AddAnimationZone.button[j].color = COLOR_WHITE;
		}

	}


	//�A�j���[�V�����I���{�^���̍X�V����
	for (int i = 0; i < g_ActiveAnimNum; i++)
	{
		g_AnimIndexButtons[i].selected = CollisionMouse(g_AnimIndexButtons[i].pos, g_AnimIndexButtons[i].w, g_AnimIndexButtons[i].h, MouseX, MouseY);

		//�I����Ԃɂ���ĐF�𕪂��܂�
		if (g_AnimIndexButtons[i].selected)
		{
			g_AnimIndexButtons[i].color = (i != g_ShowingAnim) ? COLOR_GREY : COLOR_DARK_YELLOW;
			if (IsMouseLeftTriggered())
			{
				g_ShowingAnim = i;
				UpdateAnimationZone(g_ShowingAnim, g_ActiveAnimNum, g_AnimationZone.loop, g_AnimationZone.speed, g_AnimationZone.AnimSelectButton.text, g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].text, g_AnimationZone.button[BUTTON_NEXT_ANIM].text);
				UpdateSpeedText(g_AnimationZone.button[BUTTON_SPEED].text, g_AnimationZone.speed);
			}
		}
		else
		{
			g_AnimIndexButtons[i].color = (i != g_ShowingAnim) ? COLOR_WHITE : COLOR_YELLOW;

		}

	}

	
	//�Z�[�u�{�^���̍X�V����
	{
		g_SaveButton.selected = CollisionMouse(g_SaveButton.pos, g_SaveButton.w, g_SaveButton.h, MouseX, MouseY);

		if (g_SaveButton.selected)
		{
			g_SaveButton.color = COLOR_GREY;
			if (IsMouseLeftTriggered())
			{
				SaveModelAnimation(g_ActiveAnimNum);
				g_SavedMessageTimeCnt = 0.0f;
			}
		}
		else
		{
			g_SaveButton.color = COLOR_WHITE;
		}
	}


	//���[�h�{�^���̍X�V����

	{
		g_LoadButton.selected = CollisionMouse(g_LoadButton.pos, g_LoadButton.w, g_LoadButton.h, MouseX, MouseY);

		if (g_LoadButton.selected)
		{
			g_LoadButton.color = COLOR_GREY;
			if (IsMouseLeftTriggered())
			{
				LoadModelDataBinary();
			}
		}
		else
		{
			g_LoadButton.color = COLOR_WHITE;
		}
	}



	//�A�j���[�V�����]�[���̍X�V����
	if(g_ActiveAnimNum > 0)
	{
		//�A�j���[�V�����I���̍X�V����
		g_AnimationZone.AnimSelectButton.selected = CollisionMouse(g_AnimationZone.AnimSelectButton.pos, g_AnimationZone.AnimSelectButton.w, g_AnimationZone.AnimSelectButton.h, MouseX, MouseY);

		if (g_AnimationZone.AnimSelectButton.selected)
		{
			g_AnimationZone.AnimSelectButton.color = COLOR_GREY;
			if (IsMouseLeftTriggered())
			{
				DeleteAnimation(g_ShowingAnim, g_ShowingAnim + 1); //Como no quiero que se reorganicen las animaciones le paso "g_ShowingAnim + 1"
				LoadAnimationButton(g_AnimationZone.AnimSelectButton.text, g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].text, g_AnimationZone.button[BUTTON_NEXT_ANIM].text, g_ShowingAnim, g_ActiveAnimNum);
				UpdateSpeedText(g_AnimationZone.button[BUTTON_SPEED].text, g_AnimationZone.speed);
				g_AnimationZone.AnimSelectButton.color = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
			}
		}
		else
		{
			g_AnimationZone.AnimSelectButton.color = COLOR_WHITE;
		}

		for (int j = 0; j < ANIMATION_ZONE_BUTTON_MAX; j++)
		{
			//�A�j���[�V�����I���̍X�V����
			g_AnimationZone.button[j].selected = CollisionMouse(g_AnimationZone.button[j].pos, g_AnimationZone.button[j].w, g_AnimationZone.button[j].h, MouseX, MouseY);

			
			if (g_AnimationZone.button[j].selected)
			{
				switch (j)
				{
				case BUTTON_PLAY:
					g_AnimationZone.button[j].color = (g_ShowingAnim != GetCurAnimation()) ? COLOR_DARK_RED : COLOR_DARK_GREEN;
					break;

				case BUTTON_LOOP:
					g_AnimationZone.button[j].color = g_AnimationZone.loop ? COLOR_DARK_GREEN : COLOR_DARK_RED;
					break;

				case BUTTON_SPEED:
				case BUTTON_TRANSITION_FRAMES:
				case BUTTON_NEXT_ANIM:
					break;

				default:
					g_AnimationZone.button[j].color = COLOR_GREY;
					break;
				}

				if (IsMouseLeftTriggered())
				{
					if (wcscmp(g_AnimationZone.AnimSelectButton.text, L"") != 0 &&
						wcscmp(g_AnimationZone.AnimSelectButton.text, L"Invalid Animation File") != 0 &&
						wcscmp(g_AnimationZone.AnimSelectButton.text, L"Animation File not found") != 0)
					{
						switch (j)
						{
						case BUTTON_PLAY:
							PlayButton(g_ShowingAnim);
							//g_AnimationZone.play = !g_AnimationZone.play;

							break;

						case BUTTON_LOOP:
							g_AnimationZone.button[j].color = LoopButton(g_ShowingAnim) ? COLOR_GREEN : COLOR_RED;
							g_AnimationZone.loop = !g_AnimationZone.loop;
							break;

						case BUTTON_SPEED_PLUS:
							if (g_AnimationZone.speed < 1.95)
							{
								g_AnimationZone.speed += 0.1f;
								SpeedButton(g_ShowingAnim, g_AnimationZone.speed);
								UpdateSpeedText(g_AnimationZone.button[BUTTON_SPEED].text, g_AnimationZone.speed);
							}
							break;

						case BUTTON_SPEED_MINUS:
							if (g_AnimationZone.speed > 0.15)
							{
								g_AnimationZone.speed -= 0.1f;
								SpeedButton(g_ShowingAnim, g_AnimationZone.speed);
								UpdateSpeedText(g_AnimationZone.button[BUTTON_SPEED].text, g_AnimationZone.speed);
							}
							break;

						case BUTTON_TRANSITION_FRAMES_PLUS:
							TransitionFramesButton(g_ShowingAnim, g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].text, 1);
							break;

						case BUTTON_TRANSITION_FRAMES_MINUS:
							TransitionFramesButton(g_ShowingAnim, g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].text, -1);
							break;


						case BUTTON_NEXT_ANIM_PLUS:
							NextAnimationButton(g_ShowingAnim, g_ActiveAnimNum, g_AnimationZone.button[BUTTON_NEXT_ANIM].text, 1);
							break;

						case BUTTON_NEXT_ANIM_MINUS:
							NextAnimationButton(g_ShowingAnim, g_ActiveAnimNum, g_AnimationZone.button[BUTTON_NEXT_ANIM].text, -1);
							break;


						default:
							break;
						}

					}
				}
			}
			else
			{
				switch (j)
				{
				case BUTTON_PLAY:
					g_AnimationZone.button[j].color = (g_ShowingAnim != GetCurAnimation()) ? COLOR_RED : COLOR_GREEN;
					break;

				case BUTTON_LOOP:
					g_AnimationZone.button[j].color = g_AnimationZone.loop ? COLOR_GREEN : COLOR_RED;
					break;

				case BUTTON_SPEED:
					break;

				default:
					g_AnimationZone.button[j].color = COLOR_WHITE;
					break;
				}

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
void DrawInterface(void)
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
	material.Diffuse = COLOR_WHITE;
	SetMaterial(material);

	// BACKGROUND��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_bg_TexNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, g_bg_Pos.x, g_bg_Pos.y, g_bg_w, g_bg_h, 0.0f, 0.0f, 1.0f, 1.0f, COLOR_DARK_GREY);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	

	DrawModelZone();
	DrawHideObjZone();
	DrawAnimationZone();
	DrawAddAnimationZone();
	SaveLoadButton();

	SetFont(L"Showcard Gothic", 13.0f, D2D1::ColorF(D2D1::ColorF::White), ALIGNMENT_TOP_CENTER);
	DrawStringRect(L"Camera movement: WASD QE", D2D1::RectF(0.0f, 20.0f, SCREEN_CENTER_X, 100.0f), D2D1_DRAW_TEXT_OPTIONS_NONE);

}

//���f���]�[���̕`�揈��
void DrawModelZone(void)
{
	//********
	// MODEL TITLE
	//********
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INTERFACE_TEX_TITLE_BG]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_ModelSelectButton.pos.x;	// �v���C���[�̕\���ʒuX
	float py = g_ModelSelectButton.pos.y - 25.0f;	// �v���C���[�̕\���ʒuY
	float pw = 90;		// �v���C���[�̕\����
	float ph = 20;		// �v���C���[�̕\������

	float ty = 0.0f;						// �e�N�X�`���̍���X���W
	float tx = 0.0f;	// �e�N�X�`���̍���Y���W
	float th = 1.0f;						// �e�N�X�`���̕�
	float tw = 1.0f;		// �e�N�X�`���̍���

	//SetSpriteLTColor(g_VertexBuffer, px - 5, py - 5, pw + 5, ph + 10, tx, ty, tw, th, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	//GetDeviceContext()->Draw(4, 0);	// �|���S���`��

	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, COLOR_WHITE);
	GetDeviceContext()->Draw(4, 0);	// �|���S���`��


	px += 10.0f;

	//SetTextAlignment(ALIGNMENT_CENTER_CENTER);//SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
	SetFont(L"Showcard Gothic", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_LEFT);
	DrawStringRect(L"MODEL", D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);
	SetFont(L"Bernard MT", 15.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);


	//********
	// MODEL ZONE FRAME
	//********
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INTERFACE_TEX_RECTANGLE]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	 px = g_ModelSelectButton.pos.x - 5.0f;	// �v���C���[�̕\���ʒuX
	 py = g_ModelSelectButton.pos.y - 5.0f;	// �v���C���[�̕\���ʒuY
	 pw = ZONES_FRAME_WIDTH;				// �v���C���[�̕\����
	 ph = BUTTON_HEIGHT + 10.0f;			// �v���C���[�̕\������

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//XMFLOAT4 color = g_ModelSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, COLOR_BLACK);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);



	//********
	// SELECT MODEL BUTTON
	//********

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ModelSelectButton.texNo]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	px = g_ModelSelectButton.pos.x;	// �v���C���[�̕\���ʒuX
	py = g_ModelSelectButton.pos.y;	// �v���C���[�̕\���ʒuY
	pw = g_ModelSelectButton.w;		// �v���C���[�̕\����
	ph = g_ModelSelectButton.h;		// �v���C���[�̕\������


	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//XMFLOAT4 color = g_ModelSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, g_ModelSelectButton.color);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	SetTextAlignment(ALIGNMENT_CENTER_CENTER);//SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
	DrawStringRect(L"Load Model", D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);
	SetFont(L"Bernard MT", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);

	//******************

	px += pw;
	pw = SCREEN_CENTER_X - (MARGIN_HORIZONTAL * 2) - pw;

	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, COLOR_CYAN);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

			
	SetTextAlignment(ALIGNMENT_CENTER_LEFT);//SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_LEFT);
	DrawStringRect(g_ModelSelectButton.text, D2D1::RectF(px+10.0f, py, px+10.0f + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);
	
}

//��\���{�^���]�[���̕`�揈��
void DrawHideObjZone(void)
{
	//********
	// HIDE_OBJ TITLE
	//********
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INTERFACE_TEX_TITLE_BG]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_HideObjButton[0].pos.x;	// �v���C���[�̕\���ʒuX
	float py = g_HideObjButton[0].pos.y - 25.0f;	// �v���C���[�̕\���ʒuY
	float pw = 140;			// �v���C���[�̕\����
	float ph = 20;			// �v���C���[�̕\������

	float ty = 0.0f;	// �e�N�X�`���̍���X���W
	float tx = 0.0f;	// �e�N�X�`���̍���Y���W
	float th = 1.0f;	// �e�N�X�`���̕�
	float tw = 1.0f;	// �e�N�X�`���̍���

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//XMFLOAT4 color = g_ModelSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, COLOR_WHITE);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	px += 10.0f;

	//SetTextAlignment(ALIGNMENT_CENTER_CENTER);//SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
	SetFont(L"Showcard Gothic", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_LEFT);
	DrawStringRect(L"HIDE OBJECTS", D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);
	SetFont(L"Bernard MT", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);

	

	//********
	// HIDE_OBJ ZONE FRAME
	//********
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INTERFACE_TEX_RECTANGLE]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	px = g_HideObjButton[0].pos.x - 5.0f;			// �v���C���[�̕\���ʒuX
	py = g_HideObjButton[0].pos.y - 5.0f;	// �v���C���[�̕\���ʒuY
	pw = ZONES_FRAME_WIDTH;					// �v���C���[�̕\����
	ph = 110.0f;								// �v���C���[�̕\������


	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//XMFLOAT4 color = g_ModelSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, COLOR_BLACK);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	//********
	// HIDE_OBJ BUTTONS
	//********
	for (int i = 0; i < HIDE_BUTTON_MAX; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_HideObjButton[i].texNo]);

		//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
		px = g_HideObjButton[i].pos.x;	// �v���C���[�̕\���ʒuX
		py = g_HideObjButton[i].pos.y;	// �v���C���[�̕\���ʒuY
		pw = g_HideObjButton[i].w;		// �v���C���[�̕\����
		ph = g_HideObjButton[i].h;		// �v���C���[�̕\������

		ty = 0.0f;						// �e�N�X�`���̍���X���W
		tx = 0.0f;	// �e�N�X�`���̍���Y���W
		th = 1.0f;						// �e�N�X�`���̕�
		tw = 1.0f;		// �e�N�X�`���̍���

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//XMFLOAT4 color = g_HideObjButton[i].selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, g_HideObjButton[i].color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		SetTextAlignment(ALIGNMENT_CENTER_CENTER);//SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
		DrawStringRect(g_HideObjButton[i].text/*L"No obj"*/, D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);


		for (int j = 0; j < 2; j++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_HideObjButton[i].button[j].texNo]);

			//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_HideObjButton[i].button[j].pos.x;	// �v���C���[�̕\���ʒuX
			float py = g_HideObjButton[i].button[j].pos.y;	// �v���C���[�̕\���ʒuY
			float pw = g_HideObjButton[i].button[j].w;		// �v���C���[�̕\����
			float ph = g_HideObjButton[i].button[j].h;		// �v���C���[�̕\������

			float ty = 0.0f;						// �e�N�X�`���̍���X���W
			float tx = 0.0f;	// �e�N�X�`���̍���Y���W
			float th = 1.0f;						// �e�N�X�`���̕�
			float tw = 1.0f;		// �e�N�X�`���̍���

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			//XMFLOAT4 color = g_HideObjButton[i].selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, g_HideObjButton[i].button[j].color);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			SetTextAlignment(ALIGNMENT_CENTER_CENTER);//SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
			DrawStringRect(g_HideObjButton[i].button[j].text, D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);

		}
	}


}

//�A�j���[�V�����]�[���̕`��
void DrawAnimationZone(void)
{
	if (g_ActiveAnimNum <= 0) return;
	//********
	// �A�j���[�V���� TITLE
	//********
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INTERFACE_TEX_TITLE_BG]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_AnimationZone.pos.x;	// �v���C���[�̕\���ʒuX
	float py = g_AnimationZone.pos.y - 25.0f;	// �v���C���[�̕\���ʒuY
	float pw = 120;			// �v���C���[�̕\����
	float ph = 20;			// �v���C���[�̕\������

	float ty = 0.0f;	// �e�N�X�`���̍���X���W
	float tx = 0.0f;	// �e�N�X�`���̍���Y���W
	float th = 1.0f;	// �e�N�X�`���̕�
	float tw = 1.0f;	// �e�N�X�`���̍���

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//XMFLOAT4 color = g_ModelSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, COLOR_WHITE);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	px += 10.0f;

	//SetTextAlignment(ALIGNMENT_CENTER_CENTER);//SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
	SetFont(L"Showcard Gothic", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_LEFT);
	DrawStringRect(L"ANIMATION", D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);
	SetFont(L"Bernard MT", 15.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);

	//******************

	//********
	// �A�j���[�V���� ZONE FRAME
	//********
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INTERFACE_TEX_RECTANGLE]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	 px = g_AnimationZone.pos.x - 5.0f;	// �v���C���[�̕\���ʒuX
	 py = g_AnimationZone.pos.y - 5.0f;	// �v���C���[�̕\���ʒuY
	 pw = ZONES_FRAME_WIDTH;			// �v���C���[�̕\����
	 ph = 110.0f;			// �v���C���[�̕\������
	
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, COLOR_BLACK);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	//***
	//�A�j���[�V�����I���{�^��
	//***

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_AnimationZone.AnimSelectButton.texNo]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	px = g_AnimationZone.AnimSelectButton.pos.x;	// �v���C���[�̕\���ʒuX
	py = g_AnimationZone.AnimSelectButton.pos.y;	// �v���C���[�̕\���ʒuY
	pw = g_AnimationZone.AnimSelectButton.w;		// �v���C���[�̕\����
	ph = g_AnimationZone.AnimSelectButton.h;		// �v���C���[�̕\������

	ty = 0.0f;	// �e�N�X�`���̍���X���W
	tx = 0.0f;	// �e�N�X�`���̍���Y���W
	th = 1.0f;	// �e�N�X�`���̕�
	tw = 1.0f;	// �e�N�X�`���̍���

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//XMFLOAT4 color = g_AnimationZone.AnimSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, g_AnimationZone.AnimSelectButton.color);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	SetTextAlignment(ALIGNMENT_CENTER_CENTER);//SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
	DrawStringRect(L"Load Animation", D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);
	SetFont(L"Bernard MT", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);

	//***�A�j���[�V�������O��***

	px += pw;
	pw = SCREEN_CENTER_X - (MARGIN_HORIZONTAL * 2) - pw;

	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, COLOR_CYAN);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	//WCHAR* WCtext = new WCHAR[256];
	//mbstowcs(WCtext, g_AnimationZone.AnimSelectButton.text, 1);


	SetTextAlignment(ALIGNMENT_CENTER_LEFT);//SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_LEFT);
	DrawStringRect(g_AnimationZone.AnimSelectButton.text, D2D1::RectF(px + 10.0f, py, px + 10.0f + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);

	//delete[] WCtext;
	SetFont(L"Bernard MT", 18.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);

	for (int j = 0; j < ANIMATION_ZONE_BUTTON_MAX; j++)
	{
		//***�A�j���[�V�����I���{�^��***

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_AnimationZone.button[j].texNo]);

		//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_AnimationZone.button[j].pos.x;	// �v���C���[�̕\���ʒuX
		float py = g_AnimationZone.button[j].pos.y;	// �v���C���[�̕\���ʒuY
		float pw = g_AnimationZone.button[j].w;		// �v���C���[�̕\����
		float ph = g_AnimationZone.button[j].h;		// �v���C���[�̕\������

		float ty = 0.0f;						// �e�N�X�`���̍���X���W
		float tx = 0.0f;	// �e�N�X�`���̍���Y���W
		float th = 1.0f;						// �e�N�X�`���̕�
		float tw = 1.0f;		// �e�N�X�`���̍���

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//XMFLOAT4 color = g_AnimationZone.button[j].selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, g_AnimationZone.button[j].color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		if(j == 5)	SetFont(L"Bernard MT", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);

		DrawStringRect(g_AnimationZone.button[j].text, D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);
	}
	

}

//�ǉ����폜�A�j���[�V�����]�[���̕`�揈��
void DrawAddAnimationZone(void)
{
	//********
	// �A�j���[�V������ǉ�/�폜 TITLE
	//********
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INTERFACE_TEX_TITLE_BG]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_AnimIndexButtons[0].pos.x;	// �v���C���[�̕\���ʒuX
	float py = g_AnimIndexButtons[0].pos.y - 25.0f;	// �v���C���[�̕\���ʒuY
	float pw = 175;			// �v���C���[�̕\����
	float ph = 20;			// �v���C���[�̕\������

	float ty = 0.0f;	// �e�N�X�`���̍���X���W
	float tx = 0.0f;	// �e�N�X�`���̍���Y���W
	float th = 1.0f;	// �e�N�X�`���̕�
	float tw = 1.0f;	// �e�N�X�`���̍���

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//XMFLOAT4 color = g_ModelSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, COLOR_WHITE);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	px += 10.0f;

	//SetTextAlignment(ALIGNMENT_CENTER_CENTER);//SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
	SetFont(L"Showcard Gothic", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_LEFT);
	DrawStringRect(L"ANIMATION SELECT", D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);
	SetFont(L"Bernard MT", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);

	//******************

	//********
	// �A�j���[�V������ǉ�/�폜 FRAME
	//********
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INTERFACE_TEX_RECTANGLE]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	px = g_AnimIndexButtons[0].pos.x - 5.0f;	// �v���C���[�̕\���ʒuX
	py = g_AnimIndexButtons[0].pos.y - 5.0f;	// �v���C���[�̕\���ʒuY
	pw = ZONES_FRAME_WIDTH;			// �v���C���[�̕\����
	ph = 55;			// �v���C���[�̕\������

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//XMFLOAT4 color = g_ModelSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, COLOR_BLACK);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	//***
	//�A�j���[�V������ǉ�/�폜�@�{�^��
	//***

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_AddAnimationZone.texNo]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	px = g_AddAnimationZone.pos.x;	// �v���C���[�̕\���ʒuX
	py = g_AddAnimationZone.pos.y;	// �v���C���[�̕\���ʒuY
	pw = g_AddAnimationZone.w;		// �v���C���[�̕\����
	ph = g_AddAnimationZone.h;		// �v���C���[�̕\������

	ty = 0.0f;	// �e�N�X�`���̍���X���W
	tx = 0.0f;	// �e�N�X�`���̍���Y���W
	th = 1.0f;	// �e�N�X�`���̕�
	tw = 1.0f;	// �e�N�X�`���̍���

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, COLOR_CYAN);

	// �|���S���`��
	//GetDeviceContext()->Draw(4, 0);


	for (int j = 0; j < 2; j++)
	{
		//***�A�j���[�V�����I���{�^��***

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_AddAnimationZone.button[j].texNo]);

		//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
		px = g_AddAnimationZone.button[j].pos.x;	// �v���C���[�̕\���ʒuX
		py = g_AddAnimationZone.button[j].pos.y;	// �v���C���[�̕\���ʒuY
		pw = g_AddAnimationZone.button[j].w;		// �v���C���[�̕\����
		ph = g_AddAnimationZone.button[j].h;		// �v���C���[�̕\������

		ty = 0.0f;						// �e�N�X�`���̍���X���W
		tx = 0.0f;	// �e�N�X�`���̍���Y���W
		th = 1.0f;						// �e�N�X�`���̕�
		tw = 1.0f;		// �e�N�X�`���̍���

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//XMFLOAT4 color = g_AnimationZone.button[j].selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, g_AddAnimationZone.button[j].color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		SetTextAlignment(ALIGNMENT_CENTER_CENTER);
		DrawStringRect(g_AddAnimationZone.button[j].text, D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);
	}

	//********
	// �A�j���[�V�����I���{�^���̍Đ����Ă���{�^��
	//********

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INTERFACE_TEX_OVALO]);

	int playingAnimIdx = GetCurAnimation();

	if (playingAnimIdx >= 0)
	{
		px = g_AnimIndexButtons[playingAnimIdx].pos.x - 3.0f;	// �v���C���[�̕\���ʒuX
		py = g_AnimIndexButtons[playingAnimIdx].pos.y - 3.0f;	// �v���C���[�̕\���ʒuY
		pw = g_AnimIndexButtons[playingAnimIdx].w + 6.0f;			// �v���C���[�̕\����
		ph = g_AnimIndexButtons[playingAnimIdx].h + 6.0f;			// �v���C���[�̕\������

		ty = 0.0f;	// �e�N�X�`���̍���X���W
		tx = 0.0f;	// �e�N�X�`���̍���Y���W
		th = 1.0f;	// �e�N�X�`���̕�
		tw = 1.0f;	// �e�N�X�`���̍���

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//XMFLOAT4 color = g_ModelSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, COLOR_GREEN);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	//********
	// �A�j���[�V�����I���{�^��
	//********
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INTERFACE_TEX_OVALO]);

	for (int i = 0; i < g_ActiveAnimNum; i++)
	{
		//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
		px = g_AnimIndexButtons[i].pos.x;	// �v���C���[�̕\���ʒuX
		py = g_AnimIndexButtons[i].pos.y;	// �v���C���[�̕\���ʒuY
		pw = g_AnimIndexButtons[i].w;			// �v���C���[�̕\����
		ph = g_AnimIndexButtons[i].h;			// �v���C���[�̕\������

		ty = 0.0f;	// �e�N�X�`���̍���X���W
		tx = 0.0f;	// �e�N�X�`���̍���Y���W
		th = 1.0f;	// �e�N�X�`���̕�
		tw = 1.0f;	// �e�N�X�`���̍���

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//XMFLOAT4 color = g_ModelSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, g_AnimIndexButtons[i].color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		SetTextAlignment(ALIGNMENT_CENTER_CENTER);//SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
		DrawStringRect(g_AnimIndexButtons[i].text, D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);

	}



	
}

//�Z�[�u�ƃ��[�h�{�^���̕`�揈��
void SaveLoadButton(void)
{
	//******************
	// SAVE BUTTON
	//*********************
	
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_SaveButton.texNo]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = g_SaveButton.pos.x;	// �v���C���[�̕\���ʒuX
	float py = g_SaveButton.pos.y;	// �v���C���[�̕\���ʒuY
	float pw = g_SaveButton.w;		// �v���C���[�̕\����
	float ph = g_SaveButton.h;		// �v���C���[�̕\������

	float ty = 0.0f;						// �e�N�X�`���̍���X���W
	float tx = 0.0f;	// �e�N�X�`���̍���Y���W
	float th = 1.0f;						// �e�N�X�`���̕�
	float tw = 1.0f;		// �e�N�X�`���̍���

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//XMFLOAT4 color = g_ModelSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, g_SaveButton.color);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	SetTextAlignment(ALIGNMENT_CENTER_CENTER);
	SetFont(L"Bernard MT", 20.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
	DrawStringRect(g_SaveButton.text, D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);



	//******************
	// LOAD BUTTON
	//*********************
	 
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_LoadButton.texNo]);

	//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
	px = g_LoadButton.pos.x;	// �v���C���[�̕\���ʒuX
	py = g_LoadButton.pos.y;	// �v���C���[�̕\���ʒuY
	pw = g_LoadButton.w;		// �v���C���[�̕\����
	ph = g_LoadButton.h;		// �v���C���[�̕\������

	ty = 0.0f;						// �e�N�X�`���̍���X���W
	tx = 0.0f;	// �e�N�X�`���̍���Y���W
	th = 1.0f;						// �e�N�X�`���̕�
	tw = 1.0f;		// �e�N�X�`���̍���

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//XMFLOAT4 color = g_ModelSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, g_LoadButton.color);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	//SetTextAlignment(ALIGNMENT_CENTER_CENTER);//SetFont(L"Arial", 13.0f, D2D1::ColorF(D2D1::ColorF::Black), ALIGNMENT_CENTER_CENTER);
	DrawStringRect(g_LoadButton.text, D2D1::RectF(px, py, px + pw, py + ph), D2D1_DRAW_TEXT_OPTIONS_NONE);


	//******************
	// SAVED MESAGGE
	//*********************
	 
	if (g_SavedMessageTimeCnt < SAVED_MESSAGE_TIME)
	{
		FLOAT alpha = 1.0f;
		if (g_SavedMessageTimeCnt > (SAVED_MESSAGE_TIME * 0.5f)) alpha = (SAVED_MESSAGE_TIME - g_SavedMessageTimeCnt) / (SAVED_MESSAGE_TIME * 0.5f);
		g_SavedMessageTimeCnt++;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INTERFACE_TEX_RECTANGLE]);

		//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
		px = SCREEN_CENTER_X;	// �v���C���[�̕\���ʒuX
		py = SCREEN_CENTER_Y;	// �v���C���[�̕\���ʒuY
		pw = 300.0f;		// �v���C���[�̕\����
		ph = 120.0f;		// �v���C���[�̕\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//XMFLOAT4 color = g_ModelSelectButton.selected ? XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f) : XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, XMFLOAT4(0.0f, 1.0f, 0.0f, alpha));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		SetFont(L"Showcard Gothic", 80.0f, D3DCOLORVALUE{FLOAT(0.0f),FLOAT(0.0f),FLOAT(0.0f),alpha }, ALIGNMENT_CENTER_CENTER);
		DrawStringRect(L"Saved", D2D1::RectF(SCREEN_CENTER_X - 150, SCREEN_CENTER_Y - 60, SCREEN_CENTER_X + 150, SCREEN_CENTER_Y + 60), D2D1_DRAW_TEXT_OPTIONS_NONE);

	}

}


//�A�j���[�V�������x�̕�������X�V
void UpdateSpeedText(WCHAR* text, float speed)
{
	const WCHAR* prefix = L"Speed: ";

	if (speed < -1) speed = 1.0f;
	// Convertir el float a una cadena con un solo digito decimal
	WCHAR speedStr[20]; // Ajusta el tamano segun tus necesidades
	swprintf_s(speedStr, L"%.1f", speed);

	// Crear una cadena final
	WCHAR resultado[50]; // Ajusta el tamano segun tus necesidades
	wcscpy(resultado, prefix);
	wcscat(resultado, speedStr);

	wcscpy(text, resultado);

}


//=============================================================================
// BB�ɂ�铖���蔻�菈��
// ��]�͍l�����Ȃ�
// �߂�l�F�������Ă���true
//=============================================================================
BOOL CollisionMouse(XMFLOAT3 buttonPos, float ButtonW, float ButtonH, float mouseX, float mouseY)
{
	BOOL ans = FALSE;	// �O����Z�b�g���Ă���

	// �o�E���f�B���O�{�b�N�X(BB)�̏���
	if ((mouseX > buttonPos.x ) &&
		(mouseX < buttonPos.x + ButtonW) &&
		(mouseY > buttonPos.y) &&
		(mouseY < buttonPos.y + ButtonH))
	{
		// �����������̏���
		ans = TRUE;
	}

	return ans;
}

//���f�����[�h����ۂɐV�����C���^�[�t�F�[�X���X�V���܂�
void UpdateNewModelInterface(int AnimationNum, char* modelName)
{
	g_ActiveAnimNum = AnimationNum;
	mbstowcs(g_ModelSelectButton.text, modelName, 256);

	for (int i = 0; i < HIDE_BUTTON_MAX; i++)
	{
		HideObjButton(i, g_HideObjButton[i].text, 0);
	}

	UpdateAnimationZone(g_ShowingAnim, g_ActiveAnimNum, g_AnimationZone.loop, g_AnimationZone.speed, g_AnimationZone.AnimSelectButton.text, g_AnimationZone.button[BUTTON_TRANSITION_FRAMES].text, g_AnimationZone.button[BUTTON_NEXT_ANIM].text);
	UpdateSpeedText(g_AnimationZone.button[BUTTON_SPEED].text, g_AnimationZone.speed);

}