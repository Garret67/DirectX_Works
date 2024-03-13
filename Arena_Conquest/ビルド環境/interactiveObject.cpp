//=============================================================================
//
// �G�l�~�[���� [interactiveObject.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "interactiveObject.h"
#include "bg.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "sound.h"
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void UpdateRockWall(void);
void UpdateWoodenSign(void);
void UpdateExitHouse(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEX_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEX_MAX] = {
	"data/TEXTURE/Stage/Assets/WoodSign.png",
	"data/TEXTURE/Stage/Assets/DoorRocks.png",
	"data/TEXTURE/Stage/Assets/Exit_anim.png",
	"data/TEXTURE/Stage/PopUp/Japanese/AvisoInvestigar.png",
	"data/TEXTURE/Stage/PopUp/Japanese/AvisoExit.png",
	"data/TEXTURE/Stage/CartelMessages/Japanese/cartelDelCartel2.png",
	"data/TEXTURE/Stage/CartelMessages/Japanese/cartelAttackRockMessage.png",
	"data/TEXTURE/Stage/CartelMessages/Japanese/cartelRunMessage2.png",
	"data/TEXTURE/Stage/CartelMessages/Japanese/cartelBulletMessage2.png",
	"data/TEXTURE/Stage/CartelMessages/Japanese/cartelDashMessage2.png",
	"data/TEXTURE/Stage/CartelMessages/Japanese/cartelFocusMessage2.png",
	"data/TEXTURE/Stage/CartelMessages/Japanese/cartelJumpMessage2.png",
	"data/TEXTURE/Stage/CartelMessages/Japanese/cartelEnergyMessage.png",
};


static BOOL				g_Load = FALSE;					// ���������s�������̃t���O
static INTERACTIVEOBJ	g_WoodenSign[WOODENSIGN_MAX];	// �Ŕ@�@�@Wooden Sign �\����
static INTERACTIVEOBJ	g_ExitHouse;					// �o���Ŕ@Exit House
static BREAKABLE_WALL   g_breakableWall;				// �ǁi�󂹂�j
static MESSAGE			g_ExamineMessage;				// �u���ׂ�v���b�Z�[�W
static MESSAGE			g_TextMessage;					// �������b�Z�[�W

//=============================================================================
// ����������
//=============================================================================
HRESULT InitInteractiveObj(void)
{
	//�Q�[���p�b�h�̃e�N�X�`��
	if (CheckGamepadConnection() == TRUE)
	{
		//����ݒ�
		switch (GetLanguage())
		{
		case LANG_JPN:
			g_TexturName[3]  = "data/TEXTURE/Stage/PopUp/Japanese/AvisoInvestigar.png";
			g_TexturName[4]  = "data/TEXTURE/Stage/PopUp/Japanese/AvisoExit.png";
			g_TexturName[5]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/Japanese/cartelDelCartel2.png";
			g_TexturName[6]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/Japanese/cartelAttackRockMessage.png";
			g_TexturName[7]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/Japanese/cartelRunMessage2.png";
			g_TexturName[8]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/Japanese/cartelBulletMessage2.png";
			g_TexturName[9]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/Japanese/cartelDashMessage2.png";
			g_TexturName[10] = "data/TEXTURE/Stage/CartelMessages/Gamepad/Japanese/cartelFocusMessage2.png";
			g_TexturName[11] = "data/TEXTURE/Stage/CartelMessages/Gamepad/Japanese/cartelJumpMessage2.png";
			g_TexturName[12] = "data/TEXTURE/Stage/CartelMessages/Gamepad/Japanese/cartelEnergyMessage.png";
			break;

		case LANG_USA:
			g_TexturName[3]  = "data/TEXTURE/Stage/PopUp/English/AvisoInvestigar.png";
			g_TexturName[4]  = "data/TEXTURE/Stage/PopUp/English/AvisoExit.png";
			g_TexturName[5]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/English/cartelDelCartel2.png";
			g_TexturName[6]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/English/cartelAttackRockMessage.png";
			g_TexturName[7]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/English/cartelRunMessage2.png";
			g_TexturName[8]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/English/cartelBulletMessage2.png";
			g_TexturName[9]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/English/cartelDashMessage2.png";
			g_TexturName[10] = "data/TEXTURE/Stage/CartelMessages/Gamepad/English/cartelFocusMessage2.png";
			g_TexturName[11] = "data/TEXTURE/Stage/CartelMessages/Gamepad/English/cartelJumpMessage2.png";
			g_TexturName[12] = "data/TEXTURE/Stage/CartelMessages/Gamepad/English/cartelEnergyMessage.png";
			break;

		case LANG_ESP:
			g_TexturName[3]  = "data/TEXTURE/Stage/PopUp/Spanish/AvisoInvestigar.png";
			g_TexturName[4]  = "data/TEXTURE/Stage/PopUp/Spanish/AvisoExit.png";
			g_TexturName[5]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/Spanish/cartelDelCartel2.png";
			g_TexturName[6]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/Spanish/cartelAttackRockMessage.png";
			g_TexturName[7]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/Spanish/cartelRunMessage2.png";
			g_TexturName[8]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/Spanish/cartelBulletMessage2.png";
			g_TexturName[9]  = "data/TEXTURE/Stage/CartelMessages/Gamepad/Spanish/cartelDashMessage2.png";
			g_TexturName[10] = "data/TEXTURE/Stage/CartelMessages/Gamepad/Spanish/cartelFocusMessage2.png";
			g_TexturName[11] = "data/TEXTURE/Stage/CartelMessages/Gamepad/Spanish/cartelJumpMessage2.png";
			g_TexturName[12] = "data/TEXTURE/Stage/CartelMessages/Gamepad/Spanish/cartelEnergyMessage.png";
			break;
		}
	}
	//�L�[�{�[�h�̃e�N�X�`���̃e�N�X�`��
	else
	{	//����ݒ�
		switch (GetLanguage())
		{
		case LANG_JPN:
			g_TexturName[3]  = "data/TEXTURE/Stage/PopUp/Japanese/AvisoInvestigar.png";
			g_TexturName[4]  = "data/TEXTURE/Stage/PopUp/Japanese/AvisoExit.png";
			g_TexturName[5]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/Japanese/cartelDelCartel2.png";
			g_TexturName[6]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/Japanese/cartelAttackRockMessage.png";
			g_TexturName[7]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/Japanese/cartelRunMessage2.png";
			g_TexturName[8]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/Japanese/cartelBulletMessage2.png";
			g_TexturName[9]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/Japanese/cartelDashMessage2.png";
			g_TexturName[10] = "data/TEXTURE/Stage/CartelMessages/Keyboard/Japanese/cartelFocusMessage2.png";
			g_TexturName[11] = "data/TEXTURE/Stage/CartelMessages/Keyboard/Japanese/cartelJumpMessage2.png";
			g_TexturName[12] = "data/TEXTURE/Stage/CartelMessages/Keyboard/Japanese/cartelEnergyMessage.png";
			break;

		case LANG_USA:
			g_TexturName[3]  = "data/TEXTURE/Stage/PopUp/English/AvisoInvestigar.png";
			g_TexturName[4]  = "data/TEXTURE/Stage/PopUp/English/AvisoExit.png";
			g_TexturName[5]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/English/cartelDelCartel2.png";
			g_TexturName[6]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/English/cartelAttackRockMessage.png";
			g_TexturName[7]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/English/cartelRunMessage2.png";
			g_TexturName[8]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/English/cartelBulletMessage2.png";
			g_TexturName[9]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/English/cartelDashMessage2.png";
			g_TexturName[10] = "data/TEXTURE/Stage/CartelMessages/Keyboard/English/cartelFocusMessage2.png";
			g_TexturName[11] = "data/TEXTURE/Stage/CartelMessages/Keyboard/English/cartelJumpMessage2.png";
			g_TexturName[12] = "data/TEXTURE/Stage/CartelMessages/Keyboard/English/cartelEnergyMessage.png";
			break;

		case LANG_ESP:
			g_TexturName[3]  = "data/TEXTURE/Stage/PopUp/Spanish/AvisoInvestigar.png";
			g_TexturName[4]  = "data/TEXTURE/Stage/PopUp/Spanish/AvisoExit.png";
			g_TexturName[5]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/Spanish/cartelDelCartel2.png";
			g_TexturName[6]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/Spanish/cartelAttackRockMessage.png";
			g_TexturName[7]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/Spanish/cartelRunMessage2.png";
			g_TexturName[8]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/Spanish/cartelBulletMessage2.png";
			g_TexturName[9]  = "data/TEXTURE/Stage/CartelMessages/Keyboard/Spanish/cartelDashMessage2.png";
			g_TexturName[10] = "data/TEXTURE/Stage/CartelMessages/Keyboard/Spanish/cartelFocusMessage2.png";
			g_TexturName[11] = "data/TEXTURE/Stage/CartelMessages/Keyboard/Spanish/cartelJumpMessage2.png";
			g_TexturName[12] = "data/TEXTURE/Stage/CartelMessages/Keyboard/Spanish/cartelEnergyMessage.png";
			break;
		}
	}
	




	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEX_MAX; i++)
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


	
	g_ExamineMessage.pos		= XMFLOAT3(0.0f, 0.0f, 0.0f);	// ���S�_����\��
	g_ExamineMessage.rot		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_ExamineMessage.scl		= XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_ExamineMessage.w			= 130.0f;
	g_ExamineMessage.h			= 90.0f;
	g_ExamineMessage.texNo		= TEX_MESSAGE_EXAMINE;
	g_ExamineMessage.isActive	= FALSE;


	g_TextMessage.pos			= XMFLOAT3(SCREEN_CENTER_X, SCREEN_CENTER_Y - 50.0f, 0.0f);	// ���S�_����\��
	g_TextMessage.rot			= XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_TextMessage.scl			= XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_TextMessage.w				= 448.0f;
	g_TextMessage.h				= 210.0f;
	g_TextMessage.texNo			= TEX_EXPLAIN_RUN;
	g_TextMessage.isActive		= FALSE;



	for (int i = 0; i < WOODENSIGN_MAX; i++)
	{
		g_WoodenSign[i].active				= FALSE;
		g_WoodenSign[i].texAnimSpritesMax	= 1;
		g_WoodenSign[i].currentSprite		= 0;
		g_WoodenSign[i].countAnim			= 0;
		g_WoodenSign[i].animWait			= 0;
	}
	switch (GetMode())
	{
	case MODE_TUTORIAL_1:
	{
		//�Ŕ�����
		g_WoodenSign[0].active			= TRUE;
		g_WoodenSign[0].pos				= XMFLOAT3(300.0f, 565.0f, 0.0f);	// ���S�_����\��
		g_WoodenSign[0].w				= 120.0f;
		g_WoodenSign[0].h				= 110.0f;
		g_WoodenSign[0].texNo			= TEX_SIGN;
		g_WoodenSign[0].messageTexNo	= TEX_EXPLAIN_RUN;

		//�Ŕ�����
		g_WoodenSign[1].active			= TRUE;
		g_WoodenSign[1].pos				= XMFLOAT3(962.0f, 565.0f, 0.0f);	// ���S�_����\��
		g_WoodenSign[1].w				= 120.0f;
		g_WoodenSign[1].h				= 110.0f;
		g_WoodenSign[1].texNo			= TEX_SIGN;
		g_WoodenSign[1].messageTexNo	= TEX_EXPLAIN_ATTACKROCK;

		//�Ŕ���������
		g_WoodenSign[2].active			= FALSE;
		g_WoodenSign[2].pos				= XMFLOAT3(SCREEN_CENTER_X, SCREEN_CENTER_Y - 50.0f, 0.0f);	// ���S�_����\��
		g_WoodenSign[2].w				= 448.0f;
		g_WoodenSign[2].h				= 210.0f;
		g_WoodenSign[2].texNo			= TEX_EXPLAIN_SIGN;
		g_WoodenSign[2].messageTexNo	= TEX_EXPLAIN_SIGN;


		//DoorRocks������
		g_breakableWall.pos				= XMFLOAT3(1257.5f, 425, 0.0f);	// ���S�_����\��
		g_breakableWall.w				= 45.0f;
		g_breakableWall.h				= 390.0f;
		g_breakableWall.texNo			= TEX_DOOR_ROCKS;
		g_breakableWall.messageTexNo	= TEX_EXPLAIN_ATTACKROCK;
		
		g_breakableWall.HP	  = 3;

		g_breakableWall.invincible		  = FALSE;
		g_breakableWall.invincibleTime	  = 30;
		g_breakableWall.invincibleTimeCnt = 0;
	}
		break;
	case MODE_TUTORIAL_2:

		g_WoodenSign[0].pos = XMFLOAT3(640.0f, 1445.0f, 0.0f);	// ���S�_����\��
		g_WoodenSign[0].texNo = TEX_SIGN;
		g_WoodenSign[0].messageTexNo = TEX_EXPLAIN_JUMP;

		g_WoodenSign[1].pos = XMFLOAT3(1125.0f, 1345.0f, 0.0f);	// ���S�_����\��
		g_WoodenSign[1].texNo = TEX_SIGN;
		g_WoodenSign[1].messageTexNo = TEX_EXPLAIN_ENERGY;

		g_WoodenSign[2].pos = XMFLOAT3(2360.0f, 1345.0f, 0.0f);	// ���S�_����\��
		g_WoodenSign[2].texNo = TEX_SIGN;
		g_WoodenSign[2].messageTexNo = TEX_EXPLAIN_BULLET;

		g_WoodenSign[3].pos = XMFLOAT3(3505.0f, 1200.0f, 0.0f);	// ���S�_����\��
		g_WoodenSign[3].texNo = TEX_SIGN;
		g_WoodenSign[3].messageTexNo = TEX_EXPLAIN_DASH;

		g_WoodenSign[4].pos = XMFLOAT3(4464.0f, 1445.0f, 0.0f);	// ���S�_����\��
		g_WoodenSign[4].texNo = TEX_SIGN;
		g_WoodenSign[4].messageTexNo = TEX_EXPLAIN_FOCUS;

		for (int i = 0; i < WOODENSIGN_MAX; i++)
		{
			//sign������
			g_WoodenSign[i].active			= TRUE;
			g_WoodenSign[i].w				= 120.0f;
			g_WoodenSign[i].h				= 110.0f;
		}

		g_ExitHouse.active = TRUE;
		g_ExitHouse.pos = XMFLOAT3(6125.0f, 1265.0f, 0.0f);
		g_ExitHouse.w = 128.0f * 4;
		g_ExitHouse.h = 118.0f * 4;
		g_ExitHouse.texNo = TEX_EXIT_HOUSE;
		g_ExitHouse.texAnimSpritesMax = 6;

		g_ExitHouse.currentSprite = 0;
		g_ExitHouse.countAnim = 0;
		g_ExitHouse.animWait = 10;

		break;

		
	}
	

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitInteractiveObj(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEX_MAX; i++)
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
void UpdateInteractiveObj(void)
{
	g_ExamineMessage.isActive = FALSE;
	//g_TextMessage.isActive = FALSE;


	switch (GetMode())
	{
	case MODE_TUTORIAL_1:
	
		UpdateRockWall();	//�Εǂ̏���
		UpdateWoodenSign();	//�Ŕ̏���
		break;

	case MODE_TUTORIAL_2:

		UpdateWoodenSign();	//�Ŕ̏���
		if (g_TextMessage.isActive == FALSE)
		{
			UpdateExitHouse();
		}
		
		break;
	}



#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawInteractiveObj(void)
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


	//Interactive Obj 
	for (int i = 0; i < WOODENSIGN_MAX; i++)
	{
		if (g_WoodenSign[i].active == FALSE) break;
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_WoodenSign[i].texNo]);

		//PLATFORM�̃|���S�����W�𔽉f
		float px = g_WoodenSign[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
		float py = g_WoodenSign[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
		float pw = g_WoodenSign[i].w;					// �G�l�~�[�̕\����
		float ph = g_WoodenSign[i].h;					// �G�l�~�[�̕\������

		//PLATFORM�̃e�N�X�`���[���W�𔽉f
		float tx = 0.0f;								// �e�N�X�`���̍���X���W
		float ty = 0.0f;								// �e�N�X�`���̍���Y���W
		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// Exit House
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ExitHouse.texNo]);

		float px = g_ExitHouse.pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
		float py = g_ExitHouse.pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
		float pw = g_ExitHouse.w;					// �G�l�~�[�̕\����
		float ph = g_ExitHouse.h;					// �G�l�~�[�̕\������

		//DECORATION�̃e�N�X�`���[���W�𔽉f
		float tw = 1.0f / g_ExitHouse.texAnimSpritesMax;		// �e�N�X�`���̕�
		float th = 1.0f;					// �e�N�X�`���̍���
		float tx = tw * g_ExitHouse.currentSprite;					// �e�N�X�`���̍���X���W
		float ty = 0.0f;					// �e�N�X�`���̍���Y���W

		ty += 0.01f;

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}



	{// Examine Message
		if (g_ExamineMessage.isActive)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ExamineMessage.texNo]);

			//PLATFORM�̃|���S�����W�𔽉f
			float px = g_ExamineMessage.pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_ExamineMessage.pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_ExamineMessage.w;					// �G�l�~�[�̕\����
			float ph = g_ExamineMessage.h;					// �G�l�~�[�̕\������

			//PLATFORM�̃e�N�X�`���[���W�𔽉f
			float tx = 0.0f;								// �e�N�X�`���̍���X���W
			float ty = 0.0f;								// �e�N�X�`���̍���Y���W
			float tw = 1.0f;		// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���


			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
			
	}

	{// Text Message
		if (g_TextMessage.isActive)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TextMessage.texNo]);

			//PLATFORM�̃|���S�����W�𔽉f
			float px = g_TextMessage.pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_TextMessage.pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_TextMessage.w;					// �G�l�~�[�̕\����
			float ph = g_TextMessage.h;					// �G�l�~�[�̕\������

			//PLATFORM�̃e�N�X�`���[���W�𔽉f
			float tx = 0.0f;								// �e�N�X�`���̍���X���W
			float ty = 0.0f;								// �e�N�X�`���̍���Y���W
			float tw = 1.0f;		// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���


			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

	}

	//�`���[�g���A���P�̉�ʂȂ�A�ǂ�`��
	if (GetMode() == MODE_TUTORIAL_1)
	{
		{// Breakable Wall

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_breakableWall.texNo]);

			//PLATFORM�̃|���S�����W�𔽉f
			float px = g_breakableWall.pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_breakableWall.pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_breakableWall.w;					// �G�l�~�[�̕\����
			float ph = g_breakableWall.h;					// �G�l�~�[�̕\������

			//PLATFORM�̃e�N�X�`���[���W�𔽉f
			float tx = 0.0f;								// �e�N�X�`���̍���X���W
			float ty = 0.0f;								// �e�N�X�`���̍���Y���W
			float tw = 1.0f;		// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���


			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//�󂹂��r���擾
BREAKABLE_WALL GetBreakableWall(void)
{
	return g_breakableWall;
}

//�󂹂�ǂ��q�b�g����
void BreakableWallHit(void)
{
	if (g_breakableWall.invincible == FALSE)
	{
		g_breakableWall.HP--;
		g_breakableWall.invincible = TRUE;
		//Animation wall
		//Sound Wall
		PlaySound(SOUND_LABEL_SE_impactRock);
		if (g_breakableWall.HP <= 0)
		{
			PlaySound(SOUND_LABEL_SE_stonesFalling);
			SetFade2(FADE_OUT, MODE_TUTORIAL_2, 2.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f));
		}
	}
	
}

//�Εǂ̍X�V����
void UpdateRockWall(void)
{
	//�Εǂ̖��G���
	if (g_breakableWall.invincible == TRUE)
	{
		g_breakableWall.invincibleTimeCnt++;

		if (g_breakableWall.invincibleTimeCnt >= g_breakableWall.invincibleTime)
		{
			g_breakableWall.invincible = FALSE;
			g_breakableWall.invincibleTimeCnt = 0;

		}
	}
	//�u���ׂ�v���b�Z�[�W��\������ׂ̓����蔻��
	PLAYER* player = GetPlayer();
	XMFLOAT3 WallPos = XMFLOAT3(g_breakableWall.pos.x + 100.0f, g_breakableWall.pos.y, 0.0f);
	BOOL ans = CollisionBB(WallPos, g_breakableWall.w, g_breakableWall.h,
		player[0].pos, player[0].w, player[0].h);
	// �������Ă���H
	if (ans == TRUE)
	{
		// �����������̏���
		if (g_TextMessage.isActive == FALSE)
		{
			g_ExamineMessage.isActive = TRUE;
			g_ExamineMessage.pos = XMFLOAT3(g_breakableWall.pos.x + 80.0f,
				g_breakableWall.pos.y,
				0.0f);
		}

		if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP))
		{
			if (g_TextMessage.isActive == FALSE)
			{
				g_ExamineMessage.isActive = FALSE;
				g_TextMessage.isActive = TRUE;
				g_TextMessage.texNo = g_breakableWall.messageTexNo;
			}
			else
			{
				g_TextMessage.isActive = FALSE;
			}
		}

	}
}

//�Ŕ̍X�V����
void UpdateWoodenSign(void)
{
	BOOL ans = FALSE;

	//�u�ǂށv���b�Z�[�W��\������ׂ̓����蔻��

	for (int i = 0; i < WOODENSIGN_MAX; i++)
	{
		if (g_WoodenSign->active == FALSE) break;
		if (GetMode() == MODE_TUTORIAL_1 && i == 2) continue;

		PLAYER* player = GetPlayer();


		ans = CollisionBB(g_WoodenSign[i].pos, g_WoodenSign[i].w, g_WoodenSign[i].h,
			player[0].pos, player[0].w, player[0].h);

		// �������Ă���H
		if (ans == TRUE)
		{
			// �����������̏���
			if (g_TextMessage.isActive == FALSE)
			{
				g_ExamineMessage.isActive = TRUE;
				g_ExamineMessage.texNo = TEX_MESSAGE_EXAMINE;
				g_ExamineMessage.pos = XMFLOAT3(g_WoodenSign[i].pos.x,
					g_WoodenSign[i].pos.y - (g_WoodenSign[i].h / 2) - (g_ExamineMessage.h / 2),
					0.0f);
				if (GetMode() == MODE_TUTORIAL_1)
				{
					g_WoodenSign[2].active = TRUE;
				}
			}



			if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP))
			{
				if (g_TextMessage.isActive == FALSE)
				{
					PlaySound(SOUND_LABEL_SE_Wood);

					g_ExamineMessage.isActive = FALSE;
					g_TextMessage.isActive = TRUE;
					g_TextMessage.pos = XMFLOAT3(g_WoodenSign[i].pos.x,
						g_WoodenSign[i].pos.y - g_WoodenSign[i].h * 0.5f - g_TextMessage.h * 0.5f,
						0.0f);
					g_TextMessage.texNo = g_WoodenSign[i].messageTexNo;

					if (GetMode() == MODE_TUTORIAL_1)
					{
						g_WoodenSign[2].active = FALSE;
					}
				}
				else
				{
					g_TextMessage.isActive = FALSE;
				}
			}
			break;
		}
	}
	if (ans == FALSE)
	{
		g_TextMessage.isActive = FALSE;

		if (GetMode() == MODE_TUTORIAL_1)
		{
			g_WoodenSign[2].active = FALSE;
		}
	}
}

//�o���Ŕ��X�V����
void UpdateExitHouse(void)
{

	PLAYER* player = GetPlayer();

	XMFLOAT3 ExitHousePos = XMFLOAT3(g_ExitHouse.pos.x, g_ExitHouse.pos.y + (g_ExitHouse.h / 2), 0.0f);
	float ExitHouseW = player[0].w;
	float ExitHouseH = player[0].h;

	BOOL ans = CollisionBB(ExitHousePos, ExitHouseW, ExitHouseH,
		player[0].pos, player[0].w, player[0].h);
	// �������Ă���H
	if (ans == TRUE)
	{
		// �����������̏���
		if (g_TextMessage.isActive == FALSE)
		{
			g_ExamineMessage.isActive = TRUE;
			g_ExamineMessage.texNo = TEX_MESSAGE_EXIT;
			g_ExamineMessage.pos = XMFLOAT3(ExitHousePos.x,
				ExitHousePos.y - (ExitHouseH) - (g_ExamineMessage.h),
				0.0f);
		}



		if (GetKeyboardTrigger(DIK_UP) || IsButtonTriggered(0, BUTTON_UP))
		{
			//SetFade(FADE_OUT, MODE_TITLE);
			SetTransitionDoor(TRANSITION_DOOR_CLOSE, MODE_MENU);

			PlaySound(SOUND_LABEL_SE_Wood);
		}
		
	}
	else 
	{
		g_TextMessage.isActive = FALSE;
	}





	//�A�j���[�V�����p
	g_ExitHouse.countAnim++;

	if (g_ExitHouse.countAnim >= g_ExitHouse.animWait)
	{
		g_ExitHouse.countAnim = 0;
		g_ExitHouse.currentSprite = (g_ExitHouse.currentSprite + 1) % g_ExitHouse.texAnimSpritesMax;
	}
	
}