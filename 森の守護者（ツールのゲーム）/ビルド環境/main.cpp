//=============================================================================
//
// ���C������ [main.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "sound.h"
#include "time.h"

#include "modelBinaryFBX.h"
#include "player.h"
#include "enemy.h"
#include "game.h"
#include "fade.h"
#include "title.h"
#include "tutorial.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"				// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"�e�\��"	// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void CheckHit(void);


//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

int g_Mode = MODE_TITLE;					// �N�����̉�ʂ�ݒ�

BOOL g_Gamepad = FALSE;						// �Q�[���p�b�h�̐ڑ����

//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	
	// �����_���̏���������
	srand((unsigned)time(NULL));

	// ���Ԍv���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// �E�B���h�E�̍����W
		CW_USEDEFAULT,																		// �E�B���h�E�̏���W
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,
		NULL,
		hInstance,
		NULL);

	// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	BOOL mode = TRUE;

	//int id = MessageBox(NULL, "Window���[�h�Ńv���C���܂����H", "�N�����[�h", MB_YESNOCANCEL | MB_ICONQUESTION);
	//switch (id)
	//{
	//case IDYES:		// Yes�Ȃ�Window���[�h�ŋN��
	//	mode = TRUE;
	//	break;
	//case IDNO:		// No�Ȃ�t���X�N���[�����[�h�ŋN��
	//	mode = FALSE;
	//	break;
	//case IDCANCEL:	// CANCEL�Ȃ�I��
	//default:
	//	return -1;
	//	break;
	//}

	// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	if (FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	//case WM_KEYDOWN:
	//	switch(wParam)
	//	{
	//	case VK_ESCAPE:
	//		DestroyWindow(hWnd);
	//		break;
	//	}
	//	break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);
	InitLight();

	InitCamera();
	SetViewPort(TYPE_FULL_SCREEN);

	InitInput(hInstance, hWnd);		// ���͏����̏�����
	InitSound(hWnd);				// �T�E���h�̏�����

	SetLightEnable(TRUE);
	SetCullingMode(CULL_MODE_BACK);	// �w�ʃ|���S�����J�����O

	InitFade();						// �t�F�[�h�̏�����

	SetMode(g_Mode);				// �ŏ��̃��[�h���Z�b�g// ������SetMode�̂܂܂ŁI



	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	SetMode(MODE_MAX);	// �I���̃��[�h���Z�b�g

	UninitFade();		// �t�F�[�h�̏I������
	UninitSound();		// �T�E���h�I������
	UninitCamera();		// �J�����̏I������
	UninitInput();		//���͂̏I������
	UninitRenderer();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	UpdateInput();	// ���͂̍X�V����
	UpdateLight();	// ���C�g�̍X�V����
	UpdateCamera();	// �J�����X�V


	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̍X�V
		UpdateTitle();
		break;

	case MODE_GAME:			// �Q�[����ʂ̍X�V
		UpdateGame();
		break;

	case MODE_FIGHT:			// �ΐ��ʂ̍X�V
		UpdateGame();
		break;


	case MODE_TUTORIAL:		// ���U���g��ʂ̍X�V
		UpdateTutorial();
		break;
	}


	UpdateFade();			// �t�F�[�h�����̍X�V
}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	Clear();	// �o�b�N�o�b�t�@�N���A
	SetCamera();

	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̕`��
		SetViewPort(TYPE_FULL_SCREEN);

		// 2D�̕���`�悷�鏈��
		SetDepthEnable(FALSE);		// Z��r�Ȃ�
		SetLightEnable(FALSE);		// ���C�e�B���O�𖳌�

		DrawTitle();

		SetLightEnable(TRUE);		// ���C�e�B���O��L����
		SetDepthEnable(TRUE);		// Z��r����

		break;

	case MODE_GAME:			// �Q�[����ʂ̕`��
		DrawGame();
		break;

	case MODE_FIGHT:		// �ΐ��ʂ̕`��
		DrawGame();
		break;

	case MODE_TUTORIAL:		// ���U���g��ʂ̕`��
		SetViewPort(TYPE_FULL_SCREEN);

		// 2D�̕���`�悷�鏈��
		SetDepthEnable(FALSE);		// Z��r�Ȃ�
		SetLightEnable(FALSE);		// ���C�e�B���O�𖳌�

		DrawTutorial();

		SetLightEnable(TRUE);		// ���C�e�B���O��L����
		SetDepthEnable(TRUE);		// Z��r����

		break;
	}



	{	// �t�F�[�h����
		SetViewPort(TYPE_FULL_SCREEN);

		// 2D�̕���`�悷�鏈��
		SetDepthEnable(FALSE);		// Z��r�Ȃ�
		SetLightEnable(FALSE);		// ���C�e�B���O�𖳌�

		DrawFade();					// �t�F�[�h�`��

		SetLightEnable(TRUE);		// ���C�e�B���O��L����
		SetDepthEnable(TRUE);		// Z��r����
	}


#ifdef _DEBUG
	DrawDebugProc();	// �f�o�b�O�\��
#endif

	Present();	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
}

//�}�E�X��X���W���擾����
long GetMousePosX(void)
{
	return g_MouseX;
}

//�}�E�X��Y���W���擾����
long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif


//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int mode)
{
	// ���[�h��ς���O�ɑS������������������Ⴄ

	UninitTitle();	// �^�C�g����ʂ̏I������
	UninitGame();	// �Q�[����ʂ̏I������
	UninitTutorial();	// ���U���g��ʂ̏I������

	g_Mode = mode;	// ���̃��[�h���Z�b�g���Ă���

	switch (g_Mode)
	{
	case MODE_TITLE:
		InitTitle();		// �^�C�g����ʂ̏�����

		SetCurrentBGM(SOUND_LABEL_BGM_EpicTitleBGM);
		break;

	case MODE_GAME:
		UninitCamera();		// �J�����������ŏ��������Ă������ɂ���
		InitCamera();

		InitGame();			// �Q�[����ʂ̏�����

		SetCurrentBGM(SOUND_LABEL_BGM_EpicFight);
		break;

	case MODE_FIGHT:
		UninitCamera();		// �J�����������ŏ��������Ă������ɂ���
		InitCamera();

		InitGame();			// �Q�[����ʂ̏�����

		SetCurrentBGM(SOUND_LABEL_BGM_EpicFight);
		break;


	case MODE_TUTORIAL:
		InitTutorial();		// ���U���g��ʂ̏�����

		SetCurrentBGM(SOUND_LABEL_BGM_EpicTitleBGM);
		break;

		// �Q�[���I�����̏���
	case MODE_MAX:
		UninitEnemy();		// �G�l�~�[�̏I������
		UninitPlayer();		// �v���C���[�̏I������
		break;
	}
}

//=============================================================================
// ���[�h�̎擾
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}

//�Q�[���p�b�h�̐ڑ�����t���O��TRUE�ɂ���
void GamepadConnected(void)
{
	g_Gamepad = TRUE;
}

//�Q�[���p�b�h�̐ڑ���Ԃ��擾����
BOOL CheckGamepadConnection(void)
{
	return g_Gamepad;
}
