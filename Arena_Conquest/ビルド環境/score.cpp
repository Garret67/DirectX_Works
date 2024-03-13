//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "score.h"
#include "sprite.h"
#include "sound.h"
#include "fade.h"

#include "menu.h"
#include "file.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH / 2)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT / 2)	// 

#define STARS_MAX					(5)		// STARS�̐�

#define TEXTURE_BUTTON_WIDTH		(500.0f / 3)		//
#define TEXTURE_BUTTON_HEIGHT		(115.0f / 3)		// 

#define RESET_WARNING_BUTTON_MAX	(2)

#define TEXTURE_NUM_WIDTH			(15.0f)	
#define TEXTURE_NUM_HEIGHT			(30.0f)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DrawScoresStars(void);
void DrawScoresNumbers(void);

void ResetScores(void);
void DrawResetScoreWarning(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[SCORE_TEX_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[SCORE_TEX_MAX] = {
	"data/TEXTURE/Menu/UIbgH.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Scores_Title.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Reset.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/Cancel.png",
	"data/TEXTURE/Menu/MenuButtons/Japanese/ResetScoreWarning.png",
	"data/TEXTURE/Menu/MenuButtons/Records_Num.png",
	"data/TEXTURE/Menu/Numbers2.png",
	"data/TEXTURE/Player/Star.png",
	"data/TEXTURE/Player/StarGrey.png",
};

static RESET_WARNING_BUTTON g_ResetWarningButton[RESET_WARNING_BUTTON_MAX];
static SCORE				g_Score[SCORE_MAX];					// �X�R�A

static BOOL					g_WarningMessage;

static XMFLOAT3				g_ButtonsPivot;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitScore(void)
{
	switch (GetLanguage())
	{
	case LANG_JPN:
		g_TexturName[1] = "data/TEXTURE/Menu/MenuButtons/Japanese/Scores_Title.png";
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/Japanese/Reset.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/Japanese/Cancel.png";
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/Japanese/ResetScoreWarning.png";
		break;

	case LANG_USA:
		g_TexturName[1] = "data/TEXTURE/Menu/MenuButtons/English/Scores_Title.png";
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/English/Reset.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/English/Cancel.png";
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/English/ResetScoreWarning.png";
		break;

	case LANG_ESP:
		g_TexturName[1] = "data/TEXTURE/Menu/MenuButtons/Spanish/Scores_Title.png";
		g_TexturName[2] = "data/TEXTURE/Menu/MenuButtons/Spanish/Reset.png";
		g_TexturName[3] = "data/TEXTURE/Menu/MenuButtons/Spanish/Cancel.png";
		g_TexturName[4] = "data/TEXTURE/Menu/MenuButtons/Spanish/ResetScoreWarning.png";
		break;
	}


	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < SCORE_TEX_MAX; i++)
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


	g_ButtonsPivot = GetMainMenuButtonPos();

	if (g_Score[0].stars == NULL && g_Score[0].time == NULL)
	{
		for (int i = 0; i < SCORE_MAX; i++)
		{
			g_Score[i].stars = 0;
			g_Score[i].time = 0;
		}
	}

	for (int i = 0; i < RESET_WARNING_BUTTON_MAX; i++)
	{
		g_ResetWarningButton[i].pos = XMFLOAT3(SCREEN_CENTER_X - (TEXTURE_BUTTON_WIDTH / 2) + (i * TEXTURE_BUTTON_WIDTH), SCREEN_CENTER_Y + (SCREEN_CENTER_Y / 2) - 75.0f, 0.0f);
		g_ResetWarningButton[i].w = TEXTURE_BUTTON_WIDTH;
		g_ResetWarningButton[i].h = TEXTURE_BUTTON_HEIGHT;
		g_ResetWarningButton[i].texNo = SCORE_TEX_RESET + i;
		g_ResetWarningButton[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	g_ResetWarningButton[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);


	g_WarningMessage = FALSE;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitScore(void)
{
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < SCORE_TEX_MAX; i++)
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
void UpdateScore(void)
{
	if (g_WarningMessage)
	{
		if (GetKeyboardTrigger(DIK_RIGHT) || IsButtonTriggered(0, BUTTON_RIGHT))
		{
			PlaySound(SOUND_LABEL_SE_ChangeButton);
			g_ResetWarningButton[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_ResetWarningButton[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		else if (GetKeyboardTrigger(DIK_LEFT) || IsButtonTriggered(0, BUTTON_LEFT))
		{
			PlaySound(SOUND_LABEL_SE_ChangeButton);
			g_ResetWarningButton[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_ResetWarningButton[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		}
	}



	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_A))
	{
		PlaySound(SOUND_LABEL_SE_PulseButtonAccept);

		if (g_WarningMessage) 
		{

			if (g_ResetWarningButton[0].color.x == 0.0f)
			{
				ResetScores();
				SaveData();
				//SetFade(FADE_OUT, MODE_TITLE);
				SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_MENU);
			}
			else
			{
				g_WarningMessage = FALSE;
			}
		}
		else
		{
			g_WarningMessage = TRUE;
		}
	}


	if (GetKeyboardTrigger(DIK_ESCAPE) || IsButtonTriggered(0, BUTTON_B))
	{
		PlaySound(SOUND_LABEL_SE_PulseButtonBack);

		if (g_WarningMessage)
		{
			g_WarningMessage = FALSE;
		}
		else
		{
			BackToMainMenu();
		}
	}


#ifdef _DEBUG	// �f�o�b�O����\������
	
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawScore(void)
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


	//Score Title  
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_SCORE_TITLE]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_ButtonsPivot.x, g_ButtonsPivot.y - 70.0f, TEXTURE_BUTTON_WIDTH, TEXTURE_BUTTON_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	//Reset Score Button  
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_RESET]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, g_ButtonsPivot.x, g_ButtonsPivot.y + 275.0f, TEXTURE_BUTTON_WIDTH, TEXTURE_BUTTON_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	//Score num  
	{
		for (int i = 0; i < SCORE_MAX; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_SCORE_NUM]);

		
			float pw = 30.0f;							// �{�^���̕\����
			float ph = 30.0f;							// �{�^���̕\������
			float px = g_ButtonsPivot.x - 140.0f;	// �{�^���̕\���ʒuX
			float py = g_ButtonsPivot.y + (i * 25 * 2);	// �{�^���̕\���ʒuY


			float tw = 1.0f / SCORE_MAX;	// �e�N�X�`���̕�
			float th = 1.0f;
			float tx = i * tw;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W


			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
	

	
	DrawScoresStars();
	DrawScoresNumbers();


	if (g_WarningMessage) 
	{
		DrawResetScoreWarning();
	}
}



void DrawScoresStars(void)
{
	for(int i = 0; i < SCORE_MAX; i++)
	{
		for (int j = 0; j < STARS_MAX; j++)
		{
			if (g_Score[i].stars > j)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_STAR]);
			}
			else
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_STAR_GREY]);
			}


			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float pw = 25.0f;							// �{�^���̕\����
			float ph = 25.0f;							// �{�^���̕\������
			float px = g_ButtonsPivot.x - 80.0f + (j * pw);	// �{�^���̕\���ʒuX
			float py = g_ButtonsPivot.y + (i * ph * 2);	// �{�^���̕\���ʒuY


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


void DrawScoresNumbers(void)
{
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_NUMBERS]);

	for (int i = 0; i < SCORE_MAX; i++)
	{
		// ��������������
		int number = g_Score[i].time;

		for (int j = 0; j < 5; j++)
		{
			// ����\�����錅�̐���
			float x = (float)(number % 10);

			if (j == 2) x = 10.0f; //�u�F�v��������


			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float pw = TEXTURE_NUM_WIDTH;			// �X�R�A�̕\����
			float ph = TEXTURE_NUM_HEIGHT;			// �X�R�A�̕\������
			float px = g_ButtonsPivot.x + 140.0f - (j * pw);	// �X�R�A�̕\���ʒuX
			float py = g_ButtonsPivot.y + (i * 25 * 2);;			// �X�R�A�̕\���ʒuY

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
}


void DrawResetScoreWarning(void)
{
	//Message BG  
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_BG]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//Message
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[SCORE_TEX_RESET_WARNING]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	//�{�^���̕`��
	{
		for (int i = 0; i < RESET_WARNING_BUTTON_MAX; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ResetWarningButton[i].texNo]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_ResetWarningButton[i].pos.x;			// �{�^���̕\���ʒuX
			float py = g_ResetWarningButton[i].pos.y;			// �{�^���̕\���ʒuY
			float pw = g_ResetWarningButton[i].w;		// �{�^���̕\����
			float ph = g_ResetWarningButton[i].h;		// �{�^���̕\������


			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W
			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;


			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_ResetWarningButton[i].color);
			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


SCORE* GetScore(void)
{
	return &g_Score[0];
}


void SetScore(int stars, int time)
{
	for (int i = 0; i < SCORE_MAX; i++)
	{
		if (stars > g_Score[i].stars)	//����̃X�^�[�̐�������������ۑ�����
		{
			for (int j = SCORE_MAX-1; j > i; j--)
			{
				g_Score[j].stars = g_Score[j - 1].stars;
				g_Score[j].time = g_Score[j - 1].time;
			}
			g_Score[i].stars = stars;
			g_Score[i].time = time;

			if (GetRetryRound() == 0) SaveData();

			return;
		}
		else if(stars == g_Score[i].stars)	//�X�^�[�̐�������
		{
			if (time <= g_Score[i].time)		//����̎��Ԃ�����������ۑ�����
			{
				for (int j = SCORE_MAX - 1; j > i; j--)
				{
					g_Score[j].stars = g_Score[j - 1].stars;
					g_Score[j].time = g_Score[j - 1].time;
				}
				g_Score[i].stars = stars;
				g_Score[i].time = time;

				if (GetRetryRound() == 0) SaveData();

				return;
			}
		}
	}
}


void ResetScores(void)
{
	for (int i = 0; i < SCORE_MAX; i++)
	{
		g_Score[i].stars = 0;
		g_Score[i].time = 0;
	}
}