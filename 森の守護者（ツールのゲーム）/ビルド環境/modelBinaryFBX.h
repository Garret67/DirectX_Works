//=============================================================================
//
// ���f���̏��� [modelBinaryFBX.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*********************************************************
// �\����
//*********************************************************
#define HIDDEN_OBJ_NUM		(9)			//��\���o����I�u�W�F�N�g�̐�

//**********
//MODEL �`��p DATA
//**********
struct DX11_FBXMODEL_MATERIAL
{
	MATERIAL					Material;	//�}�e���A���̏��
	ID3D11ShaderResourceView*	Texture;	//�e�N�X�`�����
};

// �`��T�u�Z�b�g�\����
struct DX11_FBXSUBSET
{
	unsigned short			StartIndex;		//�T�u�Z�b�g�̊J�n�C���f�N�X
	unsigned short			IndexNum;		//�T�u�Z�b�g�̒��_�̐�
	DX11_FBXMODEL_MATERIAL	Material;		//�T�u�Z�b�g�̃}�e���A��
};

struct DX11_BUFFERS
{
	ID3D11Buffer*	VertexBuffer;			//���_�o�b�t�@�[
	ID3D11Buffer*	IndexBuffer;			//�C���f�N�X�o�b�t�@

	DX11_FBXSUBSET* SubsetArray;			//�T�u�Z�b�g�̔z��
	unsigned short	SubsetNum;				//�T�u�Z�b�g�̐�
};

struct DX11_MODEL
{
	DX11_BUFFERS*	Buffers;				//�o�b�t�@�̏��
	unsigned short	BuffersNum;				//�o�b�t�@�̐�
};


//*********
//FBX_MODEL DATA
//*********

// �}�e���A���\����
struct FBXMODEL_MATERIAL
{
	//char			Name[256];			//Nombre de la textura 
	MATERIAL		Material;			//�}�e���A���̒l	//Ambient, Diffuse, Specular, Emission, Shininess, noTexSampling
	char			TextureName[256];	//�e�N�X�`���̏Z��	//Direccion del archivo de la textura 
};

// �`��T�u�Z�b�g�\����
struct FBXSUBSET
{
	unsigned short		StartIndex;		//�T�u�Z�b�g�̊J�n�C���f�N�X
	unsigned short		IndexNum;		//�T�u�Z�b�g�̒��_�̐�
	FBXMODEL_MATERIAL	Material;		//�T�u�Z�b�g�̃}�e���A��
};

// ���f���\����
struct MESH_DATA
{
	VERTEX_3D*		VertexArray;		//���_�̔z��	//Position, Normal, Diffuse, TexCoord
	unsigned short	VertexNum;			//���_�̐�

	unsigned short* IndexArray;			//�C���f�b�N�X�̔z��
	unsigned short	IndexNum;			//�C���f�b�N�X�̐�

	FBXSUBSET*		SubsetArray;		//�T�u�Z�b�g�̔z��
	unsigned short	SubsetNum;			//�T�u�Z�b�g�̐�
};

struct TRANSFORM_DATA
{
	XMFLOAT4X4		mtxWorld;			// OBJ�̃��[���h�}�g���b�N�X

	XMFLOAT3		LclPosition;		// OBJ�̍��W
	XMFLOAT3		LclRotation;		// OBJ�̉�]
	XMFLOAT3		LclScaling;			// OBJ�̃X�P�[��
	char			ObjectName[50];		// OBJ�̖��O
	int				FatherIdx;			// OBJ�̐e�̃C���f�N�X
};


struct MODEL_DATA
{
	char			 ModelName[256];	//���f���̖��O

	MESH_DATA*		 Mesh;				//���b�V�����
	unsigned short	 MeshNum;			//���b�V����

	TRANSFORM_DATA*  Transform;			//�g�����X�t�H�[�����
	unsigned short   TransformNum;		//�g�����X�t�H�[����

};


//*********
// ANIMATION
//*********

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

void InitModelBinaryFBX(char* FileName, DX11_MODEL* Model, MODEL_DATA* modelData, FBX_ANIMATOR* Animator);
void InitModelBinaryFBX(char* FileName, DX11_MODEL* Model, MODEL_DATA* modelData);

void UninitModelBinaryFBX(DX11_MODEL* Model, MODEL_DATA* modelData, FBX_ANIMATOR* Animator);
void UninitModelBinaryFBX(DX11_MODEL* Model, MODEL_DATA* modelData);

void UpdateModelBinaryFBX(MODEL_DATA* ResultModel, FBX_ANIMATOR& Animator);

void DrawModelBinaryFBX(DX11_MODEL* Model, MODEL_DATA  modelData, XMMATRIX worldMatix);

float NormalizeAngle(float angle);
float lerp(float a, float b, float t);
XMFLOAT3 BezierFunction(float t, XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3);