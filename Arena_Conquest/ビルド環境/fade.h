//=============================================================================
//
// �t�F�[�h���� [fade.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************

// �t�F�[�h�̏��
typedef enum
{
	FADE_NONE = 0,		// �����Ȃ����
	FADE_IN,			// �t�F�[�h�C������
	FADE_OUT,			// �t�F�[�h�A�E�g����
	FADE_MAX
} FADE;


enum
{
	TRANSITION_DOOR_NONE,
	TRANSITION_DOOR_OPEN,
	TRANSITION_DOOR_CLOSE,
};


enum 
{
	DOOR_HORIZONTAL_MOVE,
	DOOR_VERTICAL_MOVE,
	DOOR_MOVE_MAX,
};


struct DOOR
{
	XMFLOAT3	pos;		// �|���S���̍��W
	float		w, h;		// ���ƍ���
	int			texNo;		// �g�p���Ă���e�N�X�`���ԍ�
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitFade(void);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);

void SetFade(FADE fade, int modeNext);
void SetFade2(FADE fade, int modeNext, float seconds, XMFLOAT4 color);

void SetTransitionDoor(int transition, int nextMode);
