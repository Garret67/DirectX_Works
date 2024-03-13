//=============================================================================
//
// モデルの処理 [modelBinaryFBX.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*********************************************************
// 構造体
//*********************************************************
#define HIDDEN_OBJ_NUM		(9)			//非表示出来るオブジェクトの数

//**********
//MODEL 描画用 DATA
//**********
struct DX11_FBXMODEL_MATERIAL
{
	MATERIAL					Material;	//マテリアルの情報
	ID3D11ShaderResourceView*	Texture;	//テクスチャ情報
};

// 描画サブセット構造体
struct DX11_FBXSUBSET
{
	unsigned short			StartIndex;		//サブセットの開始インデクス
	unsigned short			IndexNum;		//サブセットの頂点の数
	DX11_FBXMODEL_MATERIAL	Material;		//サブセットのマテリアル
};

struct DX11_BUFFERS
{
	ID3D11Buffer*	VertexBuffer;			//頂点バッファー
	ID3D11Buffer*	IndexBuffer;			//インデクスバッファ

	DX11_FBXSUBSET* SubsetArray;			//サブセットの配列
	unsigned short	SubsetNum;				//サブセットの数
};

struct DX11_MODEL
{
	DX11_BUFFERS*	Buffers;				//バッファの情報
	unsigned short	BuffersNum;				//バッファの数
};


//*********
//FBX_MODEL DATA
//*********

// マテリアル構造体
struct FBXMODEL_MATERIAL
{
	//char			Name[256];			//Nombre de la textura 
	MATERIAL		Material;			//マテリアルの値	//Ambient, Diffuse, Specular, Emission, Shininess, noTexSampling
	char			TextureName[256];	//テクスチャの住所	//Direccion del archivo de la textura 
};

// 描画サブセット構造体
struct FBXSUBSET
{
	unsigned short		StartIndex;		//サブセットの開始インデクス
	unsigned short		IndexNum;		//サブセットの頂点の数
	FBXMODEL_MATERIAL	Material;		//サブセットのマテリアル
};

// モデル構造体
struct MESH_DATA
{
	VERTEX_3D*		VertexArray;		//頂点の配列	//Position, Normal, Diffuse, TexCoord
	unsigned short	VertexNum;			//頂点の数

	unsigned short* IndexArray;			//インデックスの配列
	unsigned short	IndexNum;			//インデックスの数

	FBXSUBSET*		SubsetArray;		//サブセットの配列
	unsigned short	SubsetNum;			//サブセットの数
};

struct TRANSFORM_DATA
{
	XMFLOAT4X4		mtxWorld;			// OBJのワールドマトリックス

	XMFLOAT3		LclPosition;		// OBJの座標
	XMFLOAT3		LclRotation;		// OBJの回転
	XMFLOAT3		LclScaling;			// OBJのスケール
	char			ObjectName[50];		// OBJの名前
	int				FatherIdx;			// OBJの親のインデクス
};


struct MODEL_DATA
{
	char			 ModelName[256];	//モデルの名前

	MESH_DATA*		 Mesh;				//メッシュ情報
	unsigned short	 MeshNum;			//メッシュ数

	TRANSFORM_DATA*  Transform;			//トランスフォーム情報
	unsigned short   TransformNum;		//トランスフォーム数

};


//*********
// ANIMATION
//*********

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

void InitModelBinaryFBX(char* FileName, DX11_MODEL* Model, MODEL_DATA* modelData, FBX_ANIMATOR* Animator);
void InitModelBinaryFBX(char* FileName, DX11_MODEL* Model, MODEL_DATA* modelData);

void UninitModelBinaryFBX(DX11_MODEL* Model, MODEL_DATA* modelData, FBX_ANIMATOR* Animator);
void UninitModelBinaryFBX(DX11_MODEL* Model, MODEL_DATA* modelData);

void UpdateModelBinaryFBX(MODEL_DATA* ResultModel, FBX_ANIMATOR& Animator);

void DrawModelBinaryFBX(DX11_MODEL* Model, MODEL_DATA  modelData, XMMATRIX worldMatix);

float NormalizeAngle(float angle);
float lerp(float a, float b, float t);
XMFLOAT3 BezierFunction(float t, XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3);