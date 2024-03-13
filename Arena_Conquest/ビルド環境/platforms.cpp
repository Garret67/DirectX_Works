//=============================================================================
//
// �G�l�~�[���� [platforms.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "platforms.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(300)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(100)	// 
#define TEXTURE_MAX					(3)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(2)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define PLATFORM_SPRITE_MAX			(7)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void PlatformAnim(int PlatformIndex);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = 
{
	"data/TEXTURE/Stage/Assets/oak_Ground.png",
	"data/TEXTURE/Stage/Assets/Spikes.png",
	"data/TEXTURE/Stage/Assets/PlatformAnim.png",
};


static PLATFORM			g_Ground	[GROUND_MAX];		// PLATFORM�\����
static PLATFORM			g_Spikes	[SPIKES_MAX];		// SPIKES�\����
static PLATFORM_ANIM	g_Platform [PLATFORM_MAX];		// PLATFORM�\����

static float			g_PlatformInitYPos;


static BOOL				g_Load = FALSE;					// ���������s�������̃t���O


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlatforms(void)
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


	for (int i = 0; i < GROUND_MAX; i++)
	{
		g_Ground[i].use = FALSE;
	}
	for (int i = 0; i < SPIKES_MAX; i++)
	{
		g_Spikes[i].use = FALSE;
	}
	for (int i = 0; i < PLATFORM_MAX; i++)
	{
		g_Platform[i].use = FALSE;
		//g_Platform[i].pos = XMFLOAT3(0.0f, g_PlatformInitYPos, 0.0f);
	}
	

	BG* bg = GetBG();
	switch (GetMode())
	{
	case MODE_TUTORIAL_1:
		g_Ground[0].use	= TRUE;
		g_Ground[0].pos	= XMFLOAT3(640.0f, 670.0f, 0.0f);	// ���S�_����\��
		g_Ground[0].rot	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ground[0].scl	= XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Ground[0].w		= 1280.0f;
		g_Ground[0].h		= 100.0f;
		g_Ground[0].texNo = 0;
		break;
	case MODE_TUTORIAL_2:
		
		// Platform�\���̂̏�����
		g_Ground[0].pos = XMFLOAT3(1125.0f, 1450.0f, 0.0f);
		g_Ground[1].pos = XMFLOAT3(2350.0f, 1450.0f, 0.0f);
		g_Ground[2].pos = XMFLOAT3(2930.0f, 1450.0f, 0.0f);
		for (int i = 0; i < 3; i++)
		{
			g_Ground[i].use = TRUE;
			g_Ground[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Ground[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Ground[i].w = TEXTURE_WIDTH;
			g_Ground[i].h = TEXTURE_HEIGHT;
			g_Ground[i].texNo = 0;
		}

		g_Ground[3].use = TRUE;
		g_Ground[3].pos = XMFLOAT3(1250.0f, 1550.0f, 0.0f);
		g_Ground[3].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ground[3].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Ground[3].w = 2500.0f;
		g_Ground[3].h = TEXTURE_HEIGHT;
		g_Ground[3].texNo = 0;

		g_Ground[4].use = TRUE;
		g_Ground[4].pos = XMFLOAT3(3195.0f, 1550.0f, 0.0f);
		g_Ground[4].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ground[4].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Ground[4].w = 830.0f;
		g_Ground[4].h = TEXTURE_HEIGHT;
		g_Ground[4].texNo = 0;

		g_Ground[5].use = TRUE;
		g_Ground[5].pos = XMFLOAT3(5310.0f, 1550.0f, 0.0f);
		g_Ground[5].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ground[5].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Ground[5].w = 2230.0f;
		g_Ground[5].h = TEXTURE_HEIGHT;
		g_Ground[5].texNo = 0;

		// Platform Half �\���̂̏�����
		g_Ground[6].use = TRUE;
		g_Ground[6].pos = XMFLOAT3(3510.0f, 1280.0f, 0.0f);
		g_Ground[6].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ground[6].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Ground[6].w = 200.0f;
		g_Ground[6].h = 50.0f;
		g_Ground[6].texNo = 0;

		g_Ground[7].use = TRUE;
		g_Ground[7].pos = XMFLOAT3(4040.0f, 1280.0f, 0.0f);
		g_Ground[7].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ground[7].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Ground[7].w = 300.0f;
		g_Ground[7].h = 50.0f;
		g_Ground[7].texNo = 0;

		g_Ground[8].use = TRUE;
		g_Ground[8].pos = XMFLOAT3(3530.0f, 1080.0f, 0.0f);
		g_Ground[8].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ground[8].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Ground[8].w = 500.0f;
		g_Ground[8].h = 50.0f;
		g_Ground[8].texNo = 0;

		//SPIKES
		g_Spikes[0].use = TRUE;
		g_Spikes[0].pos = XMFLOAT3(2640.0f, 1562.0f, 0.0f);
		g_Spikes[0].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Spikes[0].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Spikes[0].w = 300.0f;
		g_Spikes[0].h = 72.0f;
		g_Spikes[0].texNo = 1;

		g_Spikes[1].use = TRUE;
		g_Spikes[1].pos = XMFLOAT3(3900.0f, 1562.0f, 0.0f);
		g_Spikes[1].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Spikes[1].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Spikes[1].w = 600.0f;
		g_Spikes[1].h = 72.0f;
		g_Spikes[1].texNo = 1;

		break;
		
	case MODE_COLISEUM:
		g_Ground[0].use = TRUE;
		g_Ground[0].pos = XMFLOAT3(SCREEN_WIDTH - SCREEN_WIDTH /4, bg->h - 50.0f, 0.0f);	// ���S�_����\��
		g_Ground[0].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ground[0].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Ground[0].w = SCREEN_WIDTH + SCREEN_WIDTH / 2;
		g_Ground[0].h = 100.0f;
		g_Ground[0].texNo = 0;


		g_Ground[1].use = TRUE;
		g_Ground[1].pos = XMFLOAT3(-50.0f, bg->h / 2, 0.0f);
		g_Ground[1].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ground[1].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Ground[1].w = 100.0f;
		g_Ground[1].h = bg->h;
		g_Ground[1].texNo = 0;

		g_Ground[2].use = TRUE;
		g_Ground[2].pos = XMFLOAT3(bg->w + 50.0f, bg->h / 2, 0.0f);
		g_Ground[2].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ground[2].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Ground[2].w = 100.0f;
		g_Ground[2].h = bg->h;
		g_Ground[2].texNo = 0;


		g_PlatformInitYPos = bg->h + 200.0f;

		for (int i = 0; i < PLATFORM_MAX; i++)
		{
			
			g_Platform[i].use			= FALSE;
			g_Platform[i].collider		= FALSE;
			g_Platform[i].pos			= XMFLOAT3(0.0f, g_PlatformInitYPos, 0.0f);
			g_Platform[i].movePosHeight	= 0.0f;
			g_Platform[i].w				= 200.0f;
			g_Platform[i].h				= 200.0f;
			g_Platform[i].texNo			= 2;
			g_Platform[i].currentSprite	= 0;
			g_Platform[i].countAnim		= 0;
		}
		

		break;
	}
	

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlatforms(void)
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
void UpdatePlatforms(void)
{

	for (int i = 0; i < PLATFORM_MAX; i++)
	{
		if (g_Platform[i].use == TRUE)
		{
			if (g_Platform[i].pos.y > g_Platform[i].movePosHeight)
			{
				g_Platform[i].pos.y -= 10.0f;
			}
			else if (g_Platform[i].currentSprite < PLATFORM_SPRITE_MAX - 1)
			{
				PlatformAnim(i);
			}
		}
		else 
		{
			if (g_Platform[i].currentSprite > 0)
			{
				PlatformAnim(i);
			}
			else if (g_Platform[i].pos.y < g_PlatformInitYPos)
			{
				g_Platform[i].pos.y += 5.0f;
			}
		}
		
		
	}


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlatforms(void)
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

	
	BG* bg = GetBG();


	//PLATFORM
	if (GetMode() == MODE_COLISEUM)
	{
		for (int i = 0; i < PLATFORM_MAX; i++)
		{

			if (g_Platform[i].use == FALSE && g_Platform[i].pos.y >= g_PlatformInitYPos) continue;

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Platform[i].texNo]);

			//PLATFORM�̃|���S�����W�𔽉f
			float px = g_Platform[i].pos.x - bg->pos.x;	// �\���ʒuX
			float py = g_Platform[i].pos.y - bg->pos.y;	// �\���ʒuY
			float pw = g_Platform[i].w;					// �\����
			float ph = g_Platform[i].h;					// �\������

			//PLATFORM�̃e�N�X�`���[���W�𔽉f
			float tw = 1.0f / PLATFORM_SPRITE_MAX;		// �e�N�X�`���̕�
			float th = 1.0f;							// �e�N�X�`���̍���
			float tx = tw * g_Platform[i].currentSprite;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;							// �e�N�X�`���̍���Y���W



			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
	


	//Spikes 
	for (int i = 0; i < SPIKES_MAX; i++)
	{
		if (g_Spikes[i].use == FALSE) break;
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Spikes[i].texNo]);

		//PLATFORM�̃|���S�����W�𔽉f
		float px = g_Spikes[i].pos.x - bg->pos.x;	// �\���ʒuX
		float py = g_Spikes[i].pos.y - bg->pos.y;	// �\���ʒuY
		float pw = g_Spikes[i].w;					// �\����
		float ph = g_Spikes[i].h;					// �\������

		//PLATFORM�̃e�N�X�`���[���W�𔽉f
		float tx = 0.0f;								// �e�N�X�`���̍���X���W
		float ty = 0.0f;								// �e�N�X�`���̍���Y���W
		float tw = pw / 250.0f;		// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	//Ground
	for (int i = 0; i < GROUND_MAX; i++)
	{
		if (g_Ground[i].use == FALSE) break;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Ground[i].texNo]);

		//PLATFORM�̃|���S�����W�𔽉f
		float px = g_Ground[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
		float py = g_Ground[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
		float pw = g_Ground[i].w;					// �G�l�~�[�̕\����
		float ph = g_Ground[i].h;					// �G�l�~�[�̕\������

		//PLATFORM�̃e�N�X�`���[���W�𔽉f
		float tx = 0.0f;								// �e�N�X�`���̍���X���W
		float ty = 0.0f;								// �e�N�X�`���̍���Y���W
		float tw = g_Ground[i].w / TEXTURE_WIDTH;		// �e�N�X�`���̕�
		float th = g_Ground[i].h / TEXTURE_HEIGHT;	// �e�N�X�`���̍���


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			g_Ground[i].rot.z);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}


//=============================================================================
// Enemy�\���̂̐擪�A�h���X���擾
//=============================================================================
PLATFORM* GetGround(void)
{
	return &g_Ground[0];
}

//�X�p�C�N���擾
PLATFORM* GetSpikes(void)
{
	return &g_Spikes[0];
}

//PLATFORMS���擾
PLATFORM_ANIM* GetPlatforms(void)
{
	return &g_Platform[0];
}

////PLATFORMS�̃A�j���[�V��������
void PlatformAnim(int PlatformIndex)
{
	g_Platform[PlatformIndex].countAnim++;

	if (g_Platform[PlatformIndex].countAnim > ANIM_WAIT)
	{
		g_Platform[PlatformIndex].countAnim = 0;

		if (g_Platform[PlatformIndex].use == TRUE)
		{
			g_Platform[PlatformIndex].currentSprite++;
			if (g_Platform[PlatformIndex].currentSprite == PLATFORM_SPRITE_MAX - 1)
				g_Platform[PlatformIndex].collider = TRUE;
		}
		else
		{
			g_Platform[PlatformIndex].currentSprite--;
		}
	}
}

//PLATFORMS��ݒ肷��
void SetPlatform(XMFLOAT3 FinalPos)
{
	for (int i = 0; i < PLATFORM_MAX; i++)
	{
		if (g_Platform[i].use == FALSE && g_Platform[i].pos.y >= g_PlatformInitYPos)
		{
			g_Platform[i].use			= TRUE;
			g_Platform[i].pos			= XMFLOAT3(FinalPos.x, g_PlatformInitYPos, 0.0f);
			g_Platform[i].movePosHeight = FinalPos.y;
			break;
		}
	}
}

//PLATFORM�����Z�b�g
void ResetPlatform(void)
{
	for (int i = 0; i < PLATFORM_MAX; i++)
	{
		g_Platform[i].use = FALSE;
		g_Platform[i].collider = FALSE;
		g_Platform[i].movePosHeight = 0.0f;
		g_Platform[i].countAnim = 0;
	}
}