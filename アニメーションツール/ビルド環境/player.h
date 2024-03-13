//=============================================================================
//
// ���f������ [player.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PLAYER				(1)			// �v���C���[�̐�

#define	MODEL_ANIMATION_MAX	(20)				// �����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct FBXPLAYER
{
	XMFLOAT3		pos;		// �|���S���̈ʒu
	XMFLOAT3		rot;		// �|���S���̌���(��])
	XMFLOAT3		scl;		// �|���S���̑傫��(�X�P�[��)

	XMFLOAT4X4		mtxWorld;	// ���[���h�}�g���b�N�X

	bool			load;

	DX11_MODEL		model;		// ���f�����
	MODEL_DATA		modelData;
	FBX_ANIMATOR	Animator;

	float			spd;		// �ړ��X�s�[�h
	float			dir;		// ����

	bool			use;

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

FBXPLAYER* GetPlayer(void);

void LoadModelButton(WCHAR* text);
void LoadAnimationButton(WCHAR* animText, WCHAR* transitionFramesText, WCHAR* nextAnimationtext, int animNum, int activeAnimNum);

BOOL LoopButton(int AnimNum);
void PlayButton(int AnimNum);
void SpeedButton(int AnimNum, float speed);

void DeleteAnimation(int animNum, int g_ActiveAnimNum);

void SaveModelAnimation(int activeAnimNum);
void LoadModelDataBinary(void);


int GetCurAnimation(void);

void UpdateAnimationZone(int showingAnim, int activeAnimNum, BOOL& loop, float& speed, WCHAR* animText, WCHAR* transitionFramesText, WCHAR* nextAnimationtext);

void HideObjButton(int ButtonIdx, WCHAR* text, int sumando);

void TransitionFramesButton(int AnimNum, WCHAR* text, float sumando);
void NextAnimationButton(int AnimNum, int ActiveAnimNum, WCHAR* text, int sumando);