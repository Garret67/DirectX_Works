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
#define SCORE_MAX			(99999)		// �X�R�A�̍ő�l
#define SCORE_DIGIT			(5)			// ����

enum ACTION_BUTTONS
{
	ACTION_BUTTONS_OFF,

	BUTTON_UP_ITEM = 1,
	BUTTON_DOWN_OTHER,
	BUTTON_RIGHT_ATTACK,
	BUTTON_LEFT_SPECIAL,

};


enum SPECIAL_OPTIONS
{
	SP_STRONG_ATTACK,

	SP_MAX_OPTIONS,
};

enum ITEM_OPTIONS
{
	IT_HEAL_POTION_SMALL,
	IT_HEAL_POTION_MID,
	IT_HEAL_POTION_BIG,
	//IT_RESURRECTION_POTION,

	IT_MAX_OPTIONS,
};

enum OTHER_OPTIONS
{
	OT_ESCAPE,

	OT_MAX_OPTIONS,
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitFightUI(void);
void UninitFightUI(void);
void UpdateFightUI(void);
void DrawFightUI(void);

//void AddScore(int add);
//int GetScore(void);

void SetActionButton(XMFLOAT3 playerPos, BOOL flag);
void SetSelectionPointer(XMFLOAT3 targetPos, BOOL flag);
BOOL GetSelectionPointerState(void);

void SetButtonSelected(int buttonSelected);

void SetActionIndicator(XMFLOAT3 targetPos, BOOL flag);

void SetDamageUI(XMFLOAT3 targetPos, int damage, BOOL flag, BOOL critical);

void SetActionsMenu(BOOL flag, int OptionType);
BOOL GetActionMenuUse(void);
BOOL GetActionButtonUse(void);

int GetSelecteOption(void);