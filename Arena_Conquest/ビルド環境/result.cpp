//=============================================================================
//
// ���U���g��ʏ��� [result.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "result.h"
#include "input.h"
#include "score.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
//#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
//#define TEXTURE_MAX					(5)				// �e�N�X�`���̐�

#define TEXTURE_BUTTON_WIDTH		(1000.0f / 3)	
#define TEXTURE_BUTTON_HEIGHT		(114.0f / 3)	

#define TEXTURE_NUM_WIDTH			(40.0f)	
#define TEXTURE_NUM_HEIGHT			(80.0f)

#define RESULT_BUTTON_MAX			(2)
#define STARS_MAX					(5)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DrawNumbers(XMFLOAT3 pos);
void DrawResultButtons(void);

void StarsAnimation(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[RESULT_TEX_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[RESULT_TEX_MAX] = {
	"data/TEXTURE/Menu/UIbgH.png",
	"data/TEXTURE/Menu/Numbers2.png",
	"data/TEXTURE/Player/Star.png",
	"data/TEXTURE/Player/StarGrey.png",

	"data/TEXTURE/Menu/MenuButtons/Japanese/ResultTitle.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Retry.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/ReturnToMenu.png",
};


//static BOOL			g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
//static float		g_w, g_h;					// ���ƍ���
//static XMFLOAT3		g_Pos;						// �|���S���̍��W
//static int			g_TexNo;					// �e�N�X�`���ԍ�

static XMFLOAT3			g_ResultScreenPos;

static DWORD			g_StartTime;
static DWORD			g_FinalTime;
//static DWORD			g_DSecs;
static DWORD			g_Secs;
static DWORD			g_Mins;

static int				g_HitsReceived;
static int				g_Stars;

static RESULT_BUTTON	g_ResultButton[RESULT_BUTTON_MAX];
static STAR				g_GreyStars[STARS_MAX];
static STAR				g_GoldStars[STARS_MAX];

static BOOL				g_StarsAnim;
static BOOL				g_DrawGoldStars;
static int				g_StarsDisplayed;

BOOL					g_ResultInput;

static BOOL				g_Load = FALSE;



//=============================================================================
// ����������
//=============================================================================
HRESULT InitResult(void)
{
	switch (GetLanguage())
	{
	case LANG_JPN:
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/Japanese/ResultTitle.png";
		g_TexturName[5] = "data/TEXTURE/Menu/MenuButtons/Japanese/Retry.png";
		g_TexturName[6] = "data/TEXTURE/Menu/MenuButtons/Japanese/ReturnToMenu.png";
		break;

	case LANG_USA:
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/English/ResultTitle.png";
		g_TexturName[5] = "data/TEXTURE/Menu/MenuButtons/English/Retry.png";
		g_TexturName[6] = "data/TEXTURE/Menu/MenuButtons/English/ReturnToMenu.png";
		break;

	case LANG_ESP:
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/Spanish/ResultTitle.png";
		g_TexturName[5] = "data/TEXTURE/Menu/MenuButtons/Spanish/Retry.png";
		g_TexturName[6] = "data/TEXTURE/Menu/MenuButtons/Spanish/ReturnToMenu.png";
		break;
	}


	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < RESULT_TEX_MAX; i++)
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


	g_ResultScreenPos = XMFLOAT3(SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 4, 0.0f);

	g_StartTime = timeGetTime();
	g_FinalTime = 0;

	//g_DSecs = 0;
	g_Secs  = 0;
	g_Mins  = 0;

	g_HitsReceived = 0;
	g_Stars = 0;
	g_StarsAnim = FALSE;
	g_DrawGoldStars = FALSE;
	g_StarsDisplayed = 0;

	for (int i = 0; i < RESULT_BUTTON_MAX; i++)
	{
		g_ResultButton[i].pos = XMFLOAT3(SCREEN_CENTER_X - (TEXTURE_BUTTON_WIDTH / 2) + (i * TEXTURE_BUTTON_WIDTH), /*SCREEN_CENTER_Y + */(SCREEN_CENTER_Y / 2) - 75.0f, 0.0f);
		g_ResultButton[i].w = TEXTURE_BUTTON_WIDTH;
		g_ResultButton[i].h = TEXTURE_BUTTON_HEIGHT;
		g_ResultButton[i].texNo = RESULT_TEX_RETRY_BUTTON + i;
		g_ResultButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	g_ResultButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	for (int i = 0; i < STARS_MAX; i++)
	{
		g_GreyStars[i].pos = XMFLOAT3(SCREEN_CENTER_X - 200.0f + (i * 100), -80.0f, 0.0f);
		g_GreyStars[i].scl = 1.0f;
		g_GreyStars[i].w = 100.0f;
		g_GreyStars[i].h = 100.0f;
		g_GreyStars[i].texNo = RESULT_TEX_STARGREY;
	}

	for (int i = 0; i < STARS_MAX; i++)
	{
		g_GoldStars[i].pos = XMFLOAT3(SCREEN_CENTER_X - 200.0f + (i * 100), -80.0f, 0.0f);
		g_GoldStars[i].scl = 3.0f;
		g_GoldStars[i].w = 100.0f;
		g_GoldStars[i].h = 100.0f;
		g_GoldStars[i].texNo = RESULT_TEX_STAR;
	}

	g_ResultInput = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitResult(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < RESULT_TEX_MAX; i++)
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
void UpdateResult(void)
{
	if (g_ResultScreenPos.y < SCREEN_CENTER_Y)
	{
		g_ResultScreenPos.y += 5.0f;

		if (g_ResultScreenPos.y >= SCREEN_CENTER_Y)
		{
			g_StarsAnim = TRUE;
			g_DrawGoldStars = TRUE;
			g_ResultInput = TRUE;
		}
	}




	if (g_ResultInput)
	{
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
		{
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
			g_ResultInput = FALSE;

			if (g_ResultButton[0].color.x == 0.0f)
			{
				//SetFade(FADE_OUT, GetMode());
				SetTransitionDoor(TRANSITION_DOOR_CLOSE, GetMode());
				//SetFinalScreen(SCREEN_INGAME);
			}
			else
			{
				//SetFade(FADE_OUT, MODE_TITLE);
				SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_MENU);
				//SetFinalScreen(SCREEN_INGAME);
			}
		}


		if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_RIGHT) || GetKeyboardTrigger(DIK_D))
		{
			PlaySound(SOUND_LABEL_SE_ChangeButton);
			g_ResultButton[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_ResultButton[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		else if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_LEFT) || GetKeyboardTrigger(DIK_A))
		{
			PlaySound(SOUND_LABEL_SE_ChangeButton);
			g_ResultButton[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_ResultButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		}
	}
	


	StarsAnimation();


#ifdef _DEBUG	// �f�o�b�O����\������
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawResult(void)
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

	// ���U���g�̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[RESULT_TEX_BG]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_ResultScreenPos.x, g_ResultScreenPos.y, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2), 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// ���U���g�̃^�C�g����`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[RESULT_TEX_RESULT_TITLE]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, SCREEN_CENTER_X, g_ResultScreenPos.y - (SCREEN_HEIGHT / 4), (1000.0f / 2), (100.0f / 2), 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// ���U���g�̃O���[�ق���`��
	{
		for (int i = 0; i < STARS_MAX; i++)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[RESULT_TEX_STARGREY]);
		

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_GreyStars[i].pos.x;			// �{�^���̕\���ʒuX
			float py = g_GreyStars[i].pos.y + g_ResultScreenPos.y;			// �{�^���̕\���ʒuY
			float pw = g_GreyStars[i].w;		// �{�^���̕\����
			float ph = g_GreyStars[i].h;		// �{�^���̕\������


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

	// ���U���g�̂ق���`��
	{
		for (int i = 0; i < STARS_MAX; i++)
		{
			if (!g_DrawGoldStars) break;
			if (i > g_StarsDisplayed) break;

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[RESULT_TEX_STAR]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_GoldStars[i].pos.x;			// �{�^���̕\���ʒuX
			float py = g_GoldStars[i].pos.y + g_ResultScreenPos.y;			// �{�^���̕\���ʒuY
			float pw = g_GoldStars[i].w * g_GoldStars[i].scl;		// �{�^���̕\����
			float ph = g_GoldStars[i].h * g_GoldStars[i].scl;		// �{�^���̕\������


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


	// �X�R�A�\��
	{
		DrawNumbers(XMFLOAT3(SCREEN_CENTER_X, 30.0f, 0.0f));
	}


	DrawResultButtons();
}

void DrawResultButtons(void)
{
	for (int i = 0; i < RESULT_BUTTON_MAX; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ResultButton[i].texNo]);

		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_ResultButton[i].pos.x;			// �{�^���̕\���ʒuX
		float py = g_ResultButton[i].pos.y + g_ResultScreenPos.y;			// �{�^���̕\���ʒuY
		float pw = g_ResultButton[i].w;		// �{�^���̕\����
		float ph = g_ResultButton[i].h;		// �{�^���̕\������


		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W
		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_ResultButton[i].color);
		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}



void DrawNumbers(XMFLOAT3 pos)
{
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[RESULT_TEX_NUMBER]);

	// ��������������
	int number = (int)g_FinalTime;

	for (int i = 0; i < 5; i++)
	{
		// ����\�����錅�̐���
		float x = (float)(number % 10);

		if (i == 2) x = 10.0f; //�u�F�v��������
		

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float pw = TEXTURE_NUM_WIDTH;			// �X�R�A�̕\����
		float ph = TEXTURE_NUM_HEIGHT;			// �X�R�A�̕\������
		float px = (pos.x + TEXTURE_NUM_WIDTH * 2) - (i * pw) ;	// �X�R�A�̕\���ʒuX
		float py = pos.y + g_ResultScreenPos.y;			// �X�R�A�̕\���ʒuY

		float tw = 1.0f / 11;		// �e�N�X�`���̕�
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



void SetResultScreen(void)
{
	g_FinalTime = timeGetTime() - g_StartTime;

	//g_DSecs = (g_FinalTime / 10);
	g_Secs =  (g_FinalTime / 1000) % 60;
	g_Mins =  (g_FinalTime / 1000) / 60;

	g_FinalTime = g_Mins * 1000 + g_Secs;

	if (g_HitsReceived <= 10)
	{
		g_Stars = 5;
	}
	else if (g_HitsReceived <= 20)
	{
		g_Stars = 4;
	}
	else if (g_HitsReceived <= 30)
	{
		g_Stars = 3;
	}
	else if (g_HitsReceived <= 40)
	{
		g_Stars = 2;
	}
	else if (g_HitsReceived >= 50)
	{
		g_Stars = 1;
	}


	SetScore(g_Stars, (int)g_FinalTime);
	

	SetFinalScreen(SCREEN_RESULT);
}


void ResultHit(void)
{
	g_HitsReceived++;
}


void StarsAnimation(void)
{
	if (!g_StarsAnim) return;		//�X�^�[�A�j���[�V�����̃t���O

	if (g_GoldStars[g_StarsDisplayed].scl > 1.0f)
	{
		g_GoldStars[g_StarsDisplayed].scl -= 0.05f;
	}
	else
	{
		g_GoldStars[g_StarsDisplayed].scl = 1.0f;
		g_StarsDisplayed++;

		if (g_StarsDisplayed == g_Stars)
		{
			g_StarsDisplayed--;
			g_StarsAnim = FALSE;
		}
	}
}