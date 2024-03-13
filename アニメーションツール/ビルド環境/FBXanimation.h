//=============================================================================
//
// モデルの処理 [FBXmodel.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "FBXmodel.h"

//*********************************************************
// 構造体
//*********************************************************
#define HIDDEN_OBJ_NUM		(9)			

struct FBX_ANIMATED_VERTEX
{
	VERTEX_3D*		VertexArray;		//頂点情報		Position, Normal, Diffuse, TexCoord
	unsigned short	VertexNum;			//頂点数
};

// モデル構造体
struct ANIM_MODEL
{
	XMFLOAT3			LclPosition;	//座標
	XMFLOAT3			LclRotation;	//回転
	XMFLOAT3			LclScaling;		//スケール
};

struct FBX_ANIM_KEYFRAME
{
	ANIM_MODEL*			Modelo;			//OBJ情報
	int					modelosNum;		//OBJ数
	float				AnimFramesMax;	//アニメーションのフレーム数

};

struct FBX_ANIMATION
{
	char				animationName[256];		//アニメーションの名前

	int					keyFrameNum;			//キーフレーム数
	FBX_ANIM_KEYFRAME*	keyFrame;				//キーフレーム情報

	float				speed;					//再生速度
	BOOL				loop;					//ループフラグ

	float				AnimTimeCnt;			//アニメーションの時間カウンター
	float				AnimTransitionFrames;	//次のアニメーションに切り替えるまでのフレーム数
	int 				nextAnimation;			//ループしない場合、このアニメーションが終わったら次はどのアニメーションを再生するのかを決める
};

struct FBX_ANIMATOR
{
	int					animationNum;				//アニメーション数
	FBX_ANIMATION*		animation;					//アニメーション情報

	int					curAnim;					//再生しているアニメーション
	int					nextAnim;					//次再生するアニメーション

	float				transitionFramesCnt;		//次のアニメーションに切り替えるまでのフレームカウンター
	int					hiddenObj[HIDDEN_OBJ_NUM];	//非表示オブジェクト
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadAnimationFromFBX(char* FileName, FBX_ANIMATION* Animation, unsigned short ModelNum);

void UnloadFBXanimation(FBX_ANIMATOR* Animator);

void AnimationInterpolation(MODEL_DATA* ResultModel, FBX_ANIMATOR& Animator, int& curAnim, int& nextAnim);

void InitAnimator(FBX_ANIMATOR* animator, int animationNum);


