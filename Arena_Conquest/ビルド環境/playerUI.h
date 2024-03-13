//=============================================================================
//
// �X�R�A���� [score.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************

enum {
	UI_TEX_HEALTH,
	UI_TEX_ENERGY,
	UI_TEX_ROUND_BG,
	UI_TEX_ROUND_1,
	UI_TEX_ROUND_2,
	UI_TEX_ROUND_3,
	UI_TEX_ROUND_4,
	UI_TEX_ROUND_5,
	UI_TEX_ROUND_6,
	UI_TEX_ROUND_7,
	UI_TEX_ROUND_8,
	UI_TEX_ROUND_9,
	UI_TEX_ROUND_10,

	UI_TEX_MAX,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct HEALTH_UI
{
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	float		w, h;			// ���ƍ���
	int			texNo;

	int			lifes;			// ���`��ԗp
	int			lifesMax;		// �s���f�[�^�̃e�[�u���ԍ�
	BOOL		active;
};


struct ENERGY_UI
{
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	float		w, h;			// ���ƍ���
	int			texNo;

	int			energy;			// ���`��ԗp
	int			energyMax;		// �s���f�[�^�̃e�[�u���ԍ�
	BOOL		active;
};


struct ROUND_MESSAGE
{
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayerUI(void);
void UninitPlayerUI(void);
void UpdatePlayerUI(void);
void DrawPlayerUI(void);

void UpdateLifesUI(void);
void UpdateEnergyUI(void);

void ShowRoundMsg(int round);
