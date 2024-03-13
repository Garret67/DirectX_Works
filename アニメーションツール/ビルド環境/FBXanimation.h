//=============================================================================
//
// ���f���̏��� [FBXmodel.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "FBXmodel.h"

//*********************************************************
// �\����
//*********************************************************
#define HIDDEN_OBJ_NUM		(9)			

struct FBX_ANIMATED_VERTEX
{
	VERTEX_3D*		VertexArray;		//���_���		Position, Normal, Diffuse, TexCoord
	unsigned short	VertexNum;			//���_��
};

// ���f���\����
struct ANIM_MODEL
{
	XMFLOAT3			LclPosition;	//���W
	XMFLOAT3			LclRotation;	//��]
	XMFLOAT3			LclScaling;		//�X�P�[��
};

struct FBX_ANIM_KEYFRAME
{
	ANIM_MODEL*			Modelo;			//OBJ���
	int					modelosNum;		//OBJ��
	float				AnimFramesMax;	//�A�j���[�V�����̃t���[����

};

struct FBX_ANIMATION
{
	char				animationName[256];		//�A�j���[�V�����̖��O

	int					keyFrameNum;			//�L�[�t���[����
	FBX_ANIM_KEYFRAME*	keyFrame;				//�L�[�t���[�����

	float				speed;					//�Đ����x
	BOOL				loop;					//���[�v�t���O

	float				AnimTimeCnt;			//�A�j���[�V�����̎��ԃJ�E���^�[
	float				AnimTransitionFrames;	//���̃A�j���[�V�����ɐ؂�ւ���܂ł̃t���[����
	int 				nextAnimation;			//���[�v���Ȃ��ꍇ�A���̃A�j���[�V�������I������玟�͂ǂ̃A�j���[�V�������Đ�����̂������߂�
};

struct FBX_ANIMATOR
{
	int					animationNum;				//�A�j���[�V������
	FBX_ANIMATION*		animation;					//�A�j���[�V�������

	int					curAnim;					//�Đ����Ă���A�j���[�V����
	int					nextAnim;					//���Đ�����A�j���[�V����

	float				transitionFramesCnt;		//���̃A�j���[�V�����ɐ؂�ւ���܂ł̃t���[���J�E���^�[
	int					hiddenObj[HIDDEN_OBJ_NUM];	//��\���I�u�W�F�N�g
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void LoadAnimationFromFBX(char* FileName, FBX_ANIMATION* Animation, unsigned short ModelNum);

void UnloadFBXanimation(FBX_ANIMATOR* Animator);

void AnimationInterpolation(MODEL_DATA* ResultModel, FBX_ANIMATOR& Animator, int& curAnim, int& nextAnim);

void InitAnimator(FBX_ANIMATOR* animator, int animationNum);


