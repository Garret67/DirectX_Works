//=============================================================================
//
// �t�@�C������ [file.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "FBXanimation.h"
#include "FBXmodel.h"
#include "player.h"


/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/
void SaveModel(MODEL_DATA modelData, FBX_ANIMATOR Animator, int activeAnimNum);
void LoadModel(char* FileName, MODEL_DATA* modelData, FBX_ANIMATOR* Animator);


