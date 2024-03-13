//=============================================================================
//
// モデルの処理 [FBXmodel.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*********************************************************
// 構造体
//*********************************************************

// マテリアル構造体

#define FBXMODEL_MAX_MATERIAL		(16)		// １モデルのMaxマテリアル数

struct DX11_FBXMODEL_MATERIAL
{
	MATERIAL					Material;	//マテリアルの情報
	ID3D11ShaderResourceView	*Texture;	//テクスチャ情報
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
	ID3D11Buffer*	VertexBuffer;		//頂点バッファー
	ID3D11Buffer*	IndexBuffer;		//インデクスバッファ

	DX11_FBXSUBSET	*SubsetArray;		//サブセットの配列
	unsigned short	SubsetNum;			//サブセットの数
};

struct DX11_MODEL
{
	DX11_BUFFERS*	Buffers;			//バッファの情報
	unsigned short	BuffersNum;			//バッファの数
};


//*********
//FBX_MODEL
//*********

// マテリアル構造体
struct FBXMODEL_MATERIAL
{
	//char			Name[256];			//Nombre de la textura 
	MATERIAL		Material;			//マテリアルの値		Ambient, Diffuse, Specular, Emission, Shininess, noTexSampling
	char			TextureName[256];	//テクスチャの住所		Direccion del archivo de la textura 
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


// Geometry 構造体
struct FBX_GEOMETRY
{
	XMFLOAT3*		Position;			//頂点座標
	XMFLOAT3*		Normal;				//法線
	XMFLOAT4*		Diffuse;			//カラー
	XMFLOAT2*		TexCoord;			//UV座標

	unsigned short	VertexNum;			//頂点の数
	int*			IndexArray;			//インデクスの配列
	unsigned short	FBXIndexNum;		//インデクス
	unsigned short	RealIndexNum;		//インデクス（四角化の為の変数）　　　Creo que para las caras de 4 lados
	unsigned short* MaterialPerVertex;	//頂点のマテリアル　 en el FBX viene por cara, pero yo lo transformo a por vertice
	char			ID[15];				//ID
	char			FatherID[15];		//親のID
	int				ObjectType;			//
	int				ModelAttachedIdx;	//親のインデクス
};

// Model 構造体
struct FBX_MODEL
{

	XMFLOAT3		LclPosition;		//OBJの座標
	XMFLOAT3		LclRotation;		//OBJの回転
	XMFLOAT3		LclScaling;			//OBJのスケール
	XMFLOAT3		PivotPos;			//ピボットの座標
	char*			LinkedMatID[15];	//このオブジェクトが持っているマテリアルのID	//cuando vaya a meterlo, ininializarlo con la size de un char[20] y cuando este asinando poner: LinkedMatID[iD_size * Idx] para guardarlo.
	int				MatNumInModel;		//このオブジェクトが持っているマテリアルの数
	char			ID[15];				//ID
	char			ObjectName[50];		//オブジェクトの名前
	int				ObjectType;			//オブジェクト種類
	int				FatherIdx;			//親のインデクス
};


struct FBX_MATERIAL
{
	XMFLOAT4	AmbientColor;
	XMFLOAT4	DiffuseColor;
	XMFLOAT4	SpecularColor;
	XMFLOAT4	EmissiveColor;
	float		Shininess;			//brillo especular
	int			TexSampling;		//テクスチャ有無のフラグ	si esta en 0 NO lee la informacion de una textura y si esta en 1 SI le informacion de ninguna textura
	char		textureName[256];	//テクスチャのアドレス		texture address
	char		ID[15];				//マテリアルID
	int			ObjectType;			//マテリアルの種類
};


struct FBX_CONNECTIONS
{
	char	IdFather[15];		//親のID
	int		ObjectTypeFather;	//親の種類
	char	IdChild[15];		//子供のID
	int		ObjectTypeChild;	//子供の種類
};





//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadFBXModel(char* FileName, DX11_MODEL* Model, MODEL_DATA* DX11ModelData);
void UnloadFBXModel (DX11_MODEL* Model, MODEL_DATA* modelData);
void DrawFBXModel	(DX11_MODEL* Model, MODEL_DATA  modelData, XMMATRIX worldMatix);

void GetModelDiffuse(DX11_BUFFERS* Model, XMFLOAT4* diffuse);
void SetModelDiffuse(DX11_BUFFERS* Model, int mno, XMFLOAT4 diffuse);
