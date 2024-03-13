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
	SOUND_LABEL_BGM_EpicTitleBGM,	
	SOUND_LABEL_BGM_EpicFight,		

	SOUND_LABEL_SE_impactRock,
	SOUND_LABEL_SE_stonesFalling,

	SOUND_LABEL_SE_slash,	
	SOUND_LABEL_SE_bloodyBlade,	
	SOUND_LABEL_SE_Dash,		
	SOUND_LABEL_SE_MagicCast,		
	SOUND_LABEL_SE_MagicExplosion,	

	SOUND_LABEL_SE_shieldGuard,		

	SOUND_LABEL_SE_ChangeButton,	
	SOUND_LABEL_SE_PulseButtonAccept,	
	SOUND_LABEL_SE_PulseButtonBack,	

	SOUND_LABEL_SE_Exclamation,		
	SOUND_LABEL_SE_Wood,

	SOUND_LABEL_SE_ElectricShock,
	SOUND_LABEL_SE_ChargeBeam,
	SOUND_LABEL_SE_LaserSound,

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);

void PlaySound(int label);

void StopSound(int label);
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