//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "title.h"
#include "fightUI.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y

#define TEXTURE_MESSAGE_WIDTH		(500.0f)		// ���b�Z�[�W�T�C�Y
#define TEXTURE_MESSAGE_HEIGHT		(29.0f)			// ���b�Z�[�W�T�C�Y

#define TITLE_PARTICLE_MAX			(50)			//�p�[�e�B�N����
#define TITLE_PARTICLE_SPAWN_TIME	(15)			//�p�[�e�B�N������

#define MESSAGE_DIVIDE_X			(500)			// ���b�Z�[�W�̉�����

#define ANGLE_CHANGE_AMOUNT  (TEXTURE_MESSAGE_WIDTH / 360.0f)	


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetTitlePartible(XMFLOAT3 pos);
void DrawTitleParticles(void);

void DrawMessage(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TITLE_TEX_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TITLE_TEX_MAX] =
{
	"data/TEXTURE/Menu/TitleImg.png",
	"data/TEXTURE/Menu/ArenaConquer.png",
	"data/TEXTURE/Menu/Particle3.png",
	"data/TEXTURE/Menu/TitleMessageKeyboard.png",
};


static BOOL			g_bg_Use;							// TRUE:�g���Ă���  FALSE:���g�p
static float		g_bg_w, g_h;						// ���ƍ���
static XMFLOAT3		g_bg_Pos;							// �|���S���̍��W
static int			g_bg_TexNo;							// �e�N�X�`���ԍ�

TITLE_PARTICLE		g_TitleParticle[TITLE_PARTICLE_MAX];//�p�[�e�B�N��
int					g_ParticleSpawnTimeCnt;				//�p�[�e�B�N���J�E���g

BUTTON_MESSAGE		g_Message[MESSAGE_DIVIDE_X];		//�u�Q�[���X�^�[�g�v���b�Z�[�W
float				g_FrameAngleStartTate;				// ���b�Z�[�W�̓����^�C�~���O

BOOL				g_TitleInput;						// �^�C�g���C���v�b�g

static BOOL			g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
{


	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TITLE_TEX_MAX; i++)
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
	g_bg_Use = TRUE;
	g_bg_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_bg_Pos = XMFLOAT3(g_bg_w / 2, g_h / 2, 0.0f);
	g_bg_TexNo = 0;


	for (int i = 0; i < TITLE_PARTICLE_MAX; i++)
	{

		g_TitleParticle[i].use = FALSE;
		g_TitleParticle[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_TitleParticle[i].w = 10.0f;
		g_TitleParticle[i].h = 10.0f;
		g_TitleParticle[i].texNo = TITLE_TEX_PARTICLES;

		g_TitleParticle[i].moveSpeed = 2.0f;
		g_TitleParticle[i].moveDir = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_TitleParticle[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_TitleParticle[i].lifeTime = 0;
		g_TitleParticle[i].lifeTimeCnt = 0;
		g_TitleParticle[i].alphaAdd = 0.0f;
	}

	g_ParticleSpawnTimeCnt = 0;


	// �v���C���[�\���̂̏�����
	for (int i = 0; i < MESSAGE_DIVIDE_X; i++)
	{
		g_Message[i].use	= TRUE;
		g_Message[i].pos	= XMFLOAT3((SCREEN_CENTER_X-100.0f) / 1.5f + (1.0f * i), SCREEN_HEIGHT - 50.0f, 0.0f);	// ���S�_����\��
		g_Message[i].rot	= XMFLOAT3(0.0f, 0.0f, 0.0f);				// ��]��
		g_Message[i].scl	= XMFLOAT3(1.0f, 1.0f, 1.0f);				// �g��k��
		g_Message[i].w		= 1.0f;
		g_Message[i].h		= TEXTURE_MESSAGE_HEIGHT;
		g_Message[i].texNo	= TITLE_TEX_BUTTON_MESSAGE;

		g_Message[i].countAnim   = 0;
		g_Message[i].patternAnim = 0;

	}

	g_FrameAngleStartTate = 0.0f;

	g_TitleInput = TRUE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TITLE_TEX_MAX; i++)
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
void UpdateTitle(void)
{
	float posX = (float)(rand() % SCREEN_WIDTH);
	SetTitlePartible(XMFLOAT3(posX, SCREEN_HEIGHT, 0.0f));

	if (g_TitleInput)
	{
		if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B) || IsButtonTriggered(0, BUTTON_R))
		{// Enter��������A�X�e�[�W��؂�ւ���
			SetFade(FADE_OUT, MODE_TUTORIAL);
			g_TitleInput = FALSE;
			PlaySound(SOUND_LABEL_SE_PulseButtonAccept);
		}
	}


	for (int i = 0; i < MESSAGE_DIVIDE_X; i++)
	{
		// �����Ă�v���C���[��������������
		if (g_Message[i].use == TRUE)
		{
			float DrawAngleStart = g_FrameAngleStartTate + (ANGLE_CHANGE_AMOUNT * i);
			float currentRadians = XMConvertToRadians(DrawAngleStart);
			g_Message[i].pos.y = (SCREEN_HEIGHT - 50.0f) + sinf(currentRadians) * 5;
		}
	}

	g_FrameAngleStartTate += ANGLE_CHANGE_AMOUNT;

	if (g_FrameAngleStartTate >= 360.0f)
	{
		g_FrameAngleStartTate -= 360.0f;
	}



#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	DrawTitleParticles();

	DrawTitleParticles();

	DrawMessage();
}

//===========================
// �p�[�e�B�N���ݒ�
//===========================
void SetTitlePartible(XMFLOAT3 pos)
{
	g_ParticleSpawnTimeCnt++;
	if (g_ParticleSpawnTimeCnt > TITLE_PARTICLE_SPAWN_TIME)
	{
		g_ParticleSpawnTimeCnt = 0;

		for (int i = 0; i < TITLE_PARTICLE_MAX; i++)
		{
			if (g_TitleParticle[i].use == FALSE)
			{
				g_TitleParticle[i].use = TRUE;
				g_TitleParticle[i].pos = pos;
				g_TitleParticle[i].moveSpeed = 0.0f;

				float angle = (float)(rand() % 78);
				angle /= 100;
				angle += XM_PIDIV2 - XM_PIDIV4 / 2;
				angle *= -1;
				g_TitleParticle[i].moveDir.x = cosf(angle);	//angle�̕����ֈړ�
				g_TitleParticle[i].moveDir.y = sinf(angle);	//angle�̕����ֈړ�

				g_TitleParticle[i].lifeTime = TITLE_PARTICLE_MAX;
				g_TitleParticle[i].lifeTimeCnt = 0;

				g_TitleParticle[i].alphaAdd = 0.007f;

				g_TitleParticle[i].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
				break;
			}
		}
	}


	for (int i = 0; i < TITLE_PARTICLE_MAX; i++)
	{
		if (g_TitleParticle[i].use == TRUE)
		{
			g_TitleParticle[i].lifeTimeCnt++;
			g_TitleParticle[i].moveSpeed += 0.02f;
			g_TitleParticle[i].pos.x += g_TitleParticle[i].moveDir.x * g_TitleParticle[i].moveSpeed;
			g_TitleParticle[i].pos.y += g_TitleParticle[i].moveDir.y * g_TitleParticle[i].moveSpeed;
			g_TitleParticle[i].color.x += 0.002f/*1.0f / TITLE_PARTICLE_MAX*/;

			if (g_TitleParticle[i].color.w <= 1.0f)
			{
				g_TitleParticle[i].color.w += g_TitleParticle[i].alphaAdd;
			}

			//��ʒ[�̓����蔻��
			{
				if (g_TitleParticle[i].pos.x < (-g_TitleParticle[i].w / 2))		// �E
				{
					g_TitleParticle[i].use = FALSE;
				}
				if (g_TitleParticle[i].pos.x > (SCREEN_WIDTH + g_TitleParticle[i].w / 2))	// ��
				{
					g_TitleParticle[i].use = FALSE;
				}
				if (g_TitleParticle[i].pos.y < (-g_TitleParticle[i].h / 2))		// ��
				{
					g_TitleParticle[i].use = FALSE;
				}
			}
		}
	}
}

//===========================
// �p�[�e�B�N���`�揈��
//===========================
void DrawTitleParticles(void)
{
	for (int i = 0; i < TITLE_PARTICLE_MAX; i++)
	{
		if (g_TitleParticle[i].use == TRUE)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TitleParticle[i].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_TitleParticle[i].pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_TitleParticle[i].pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_TitleParticle[i].w;					// �G�l�~�[�̕\����
			float ph = g_TitleParticle[i].h;					// �G�l�~�[�̕\������

			float tw = 1.0f;		// �e�N�X�`���̕�
			float th = 1.0f;		// �e�N�X�`���̍���
			float tx = 0.0f;		// �e�N�X�`���̍���X���W
			float ty = 0.0f;		// �e�N�X�`���̍���Y���W


			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_TitleParticle[i].color);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//===========================
// ���b�Z�[�W�`�揈��
//===========================
void DrawMessage(void)
{
	for (int i = 0; i < MESSAGE_DIVIDE_X; i++)
	{
		if (g_Message[i].use == TRUE)		// ���̃v���C���[���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Message[i].texNo]);

			//���X�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Message[i].pos.x;	// �v���C���[�̕\���ʒuX
			float py = g_Message[i].pos.y;	// �v���C���[�̕\���ʒuY
			float pw = g_Message[i].w;		// �v���C���[�̕\����
			float ph = g_Message[i].h;		// �v���C���[�̕\������

			float ty = 0.0f;						// �e�N�X�`���̍���X���W
			float tx = (1.0f / TEXTURE_MESSAGE_WIDTH) * i;	// �e�N�X�`���̍���Y���W
			float th = 1.0f;						// �e�N�X�`���̕�
			float tw = (1.0f / TEXTURE_MESSAGE_WIDTH);		// �e�N�X�`���̍���

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}