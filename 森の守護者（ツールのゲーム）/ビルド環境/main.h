//=============================================================================
//
// ���C������ [main.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf ��warning�h�~
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>


// �{���̓w�b�_�ɏ����Ȃ������ǂ�
using namespace DirectX;


#define DIRECTINPUT_VERSION 0x0800		// �x���Ώ�
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)


//*****************************************************************************
// ���C�u�����̃����N
//*****************************************************************************
#pragma comment (lib, "d3d11.lib")		
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")	
#pragma comment (lib, "d3dx9.lib")	
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCREEN_WIDTH	(1280)			// �E�C���h�E�̕�		1920�~1080	1280�~720	960�~540
#define SCREEN_HEIGHT	( 720)			// �E�C���h�E�̍���
#define SCREEN_CENTER_X	(SCREEN_WIDTH  / 2)	// �E�C���h�E�̒��S�w���W
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)	// �E�C���h�E�̒��S�x���W

//#define	MAP_W			(1280.0f)
//#define	MAP_H			(1280.0f)
//#define	MAP_TOP			(MAP_H/2)
//#define	MAP_DOWN		(-MAP_H/2)
//#define	MAP_LEFT		(-MAP_W/2)
//#define	MAP_RIGHT		(MAP_W/2)


enum
{
	MODE_TITLE = 0,			// �^�C�g�����
	MODE_TUTORIAL,			// �Q�[���������
	MODE_GAME,				// �Q�[�����
	MODE_FIGHT,				// �t�@�C�g���
	//MODE_RESULT,			// ���U���g���
	MODE_MAX
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
long GetMousePosX(void);
long GetMousePosY(void);
char* GetDebugStr(void);

void SetMode(int mode);
int GetMode(void);

void GamepadConnected(void);
BOOL CheckGamepadConnection(void);

