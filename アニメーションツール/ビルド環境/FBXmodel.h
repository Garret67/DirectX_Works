//=============================================================================
//
// ���f���̏��� [FBXmodel.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*********************************************************
// �\����
//*********************************************************

// �}�e���A���\����

#define FBXMODEL_MAX_MATERIAL		(16)		// �P���f����Max�}�e���A����

struct DX11_FBXMODEL_MATERIAL
{
	MATERIAL					Material;	//�}�e���A���̏��
	ID3D11ShaderResourceView	*Texture;	//�e�N�X�`�����
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
	ID3D11Buffer*	VertexBuffer;		//���_�o�b�t�@�[
	ID3D11Buffer*	IndexBuffer;		//�C���f�N�X�o�b�t�@

	DX11_FBXSUBSET	*SubsetArray;		//�T�u�Z�b�g�̔z��
	unsigned short	SubsetNum;			//�T�u�Z�b�g�̐�
};

struct DX11_MODEL
{
	DX11_BUFFERS*	Buffers;			//�o�b�t�@�̏��
	unsigned short	BuffersNum;			//�o�b�t�@�̐�
};


//*********
//FBX_MODEL
//*********

// �}�e���A���\����
struct FBXMODEL_MATERIAL
{
	//char			Name[256];			//Nombre de la textura 
	MATERIAL		Material;			//�}�e���A���̒l		Ambient, Diffuse, Specular, Emission, Shininess, noTexSampling
	char			TextureName[256];	//�e�N�X�`���̏Z��		Direccion del archivo de la textura 
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
//FBX_DATA
//*********

enum OBJECT_TYPE
{
	TYPE_GEOMETRY,
	TYPE_MODEL,
	TYPE_MATERIAL,
	TYPE_TEXTURE,

	TYPE_ANIM_CURVE,
	TYPE_ANIM_CURVE_NODE,

};


// Geometry �\����
struct FBX_GEOMETRY
{
	XMFLOAT3*		Position;			//���_���W
	XMFLOAT3*		Normal;				//�@��
	XMFLOAT4*		Diffuse;			//�J���[
	XMFLOAT2*		TexCoord;			//UV���W

	unsigned short	VertexNum;			//���_�̐�
	int*			IndexArray;			//�C���f�N�X�̔z��
	unsigned short	FBXIndexNum;		//�C���f�N�X
	unsigned short	RealIndexNum;		//�C���f�N�X�i�l�p���ׂ̈̕ϐ��j�@�@�@Creo que para las caras de 4 lados
	unsigned short* MaterialPerVertex;	//���_�̃}�e���A���@ en el FBX viene por cara, pero yo lo transformo a por vertice
	char			ID[15];				//ID
	char			FatherID[15];		//�e��ID
	int				ObjectType;			//
	int				ModelAttachedIdx;	//�e�̃C���f�N�X
};

// Model �\����
struct FBX_MODEL
{

	XMFLOAT3		LclPosition;		//OBJ�̍��W
	XMFLOAT3		LclRotation;		//OBJ�̉�]
	XMFLOAT3		LclScaling;			//OBJ�̃X�P�[��
	XMFLOAT3		PivotPos;			//�s�{�b�g�̍��W
	char*			LinkedMatID[15];	//���̃I�u�W�F�N�g�������Ă���}�e���A����ID	//cuando vaya a meterlo, ininializarlo con la size de un char[20] y cuando este asinando poner: LinkedMatID[iD_size * Idx] para guardarlo.
	int				MatNumInModel;		//���̃I�u�W�F�N�g�������Ă���}�e���A���̐�
	char			ID[15];				//ID
	char			ObjectName[50];		//�I�u�W�F�N�g�̖��O
	int				ObjectType;			//�I�u�W�F�N�g���
	int				FatherIdx;			//�e�̃C���f�N�X
};


struct FBX_MATERIAL
{
	XMFLOAT4	AmbientColor;
	XMFLOAT4	DiffuseColor;
	XMFLOAT4	SpecularColor;
	XMFLOAT4	EmissiveColor;
	float		Shininess;			//brillo especular
	int			TexSampling;		//�e�N�X�`���L���̃t���O	si esta en 0 NO lee la informacion de una textura y si esta en 1 SI le informacion de ninguna textura
	char		textureName[256];	//�e�N�X�`���̃A�h���X		texture address
	char		ID[15];				//�}�e���A��ID
	int			ObjectType;			//�}�e���A���̎��
};


struct FBX_CONNECTIONS
{
	char	IdFather[15];		//�e��ID
	int		ObjectTypeFather;	//�e�̎��
	char	IdChild[15];		//�q����ID
	int		ObjectTypeChild;	//�q���̎��
};





//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void LoadFBXModel(char* FileName, DX11_MODEL* Model, MODEL_DATA* DX11ModelData);
void UnloadFBXModel (DX11_MODEL* Model, MODEL_DATA* modelData);
void DrawFBXModel	(DX11_MODEL* Model, MODEL_DATA  modelData, XMMATRIX worldMatix);

void GetModelDiffuse(DX11_BUFFERS* Model, XMFLOAT4* diffuse);
void SetModelDiffuse(DX11_BUFFERS* Model, int mno, XMFLOAT4 diffuse);
