//=============================================================================
//
// �؏��� [tree.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#define	GROUND_WIDTH			(1000.0f)			// ���_�T�C�Y
#define	GROUND_HEIGHT			(1000.0f)			// ���_�T�C�Y

#define	MAX_TREE				(80)			// �؍ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

typedef struct
{
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	rot;			// ��]

	float		fWidth;			// ��
	float		fHeight;		// ����

	MATERIAL	material;		// �}�e���A��
	int			nIdxShadow;		// �eID
	BOOL		use;			// �g�p���Ă��邩�ǂ���

} BILLBOARD;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitGameEnvironment(void);
void UninitGameEnvironment(void);
void UpdateGameEnvironment(void);
void DrawGameEnvironment(void);

BILLBOARD* GetTree(void);
