//=============================================================================
//
// サウンド処理 [sound.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_EpicTitleBGM,		// 
	SOUND_LABEL_BGM_EpicFight,			// 

	SOUND_LABEL_SE_ChangeButton,		// 
	SOUND_LABEL_SE_PulseButtonAccept,	// 
	SOUND_LABEL_SE_PulseButtonBack,		// 

	SOUND_LABEL_SE_Punch,			//
	SOUND_LABEL_SE_Slap,			//
	SOUND_LABEL_SE_Heal,			//
	SOUND_LABEL_SE_ChangePage,		//
	SOUND_LABEL_SE_Exclamation,		//

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);

void PlaySound(int label);

//void StopSound(int label);
void StopSound(void);

void SetVolumeSound(float volume);
void SetVolumeSound(int label, float volume);

void SetFrequencyRatio(float pitch);
void SetFrequencyRatio(int label, float pitch);

void SetOutputMatrixSound(int label, float left, float right);

void ChangeBGMVolume(float volumeChange);
void ChangeSEVolume(float volumeChange);

void SetBGMVolume(float volume);
void SetSEVolume(float volume);

float GetVolumeBGM(void);
float GetVolumeSE(void);

void SetCurrentBGM(int currentBGM);
int GetCurrentBGM(void);