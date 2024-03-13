//=============================================================================
//
// ファイル処理 [file.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "FBXanimation.h"
#include "FBXmodel.h"
#include "player.h"


/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/
void SaveModel(MODEL_DATA modelData, FBX_ANIMATOR Animator, int activeAnimNum);
void LoadModel(char* FileName, MODEL_DATA* modelData, FBX_ANIMATOR* Animator);


