//=============================================================================
//
// モデルの処理 [FBXmodel.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include "FBXmodel.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VALUE_MOVE_MODEL	(0.50f)					// 移動速度
#define	RATE_MOVE_MODEL		(0.20f)					// 移動慣性係数
#define	VALUE_ROTATE_MODEL	(XM_PI * 0.05f)			// 回転速度
#define	RATE_ROTATE_MODEL	(0.20f)					// 回転慣性係数
#define	SCALE_MODEL			(10.0f)					// 回転慣性係数

//*****************************************************************************
// グローバル変数
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct FBX_TEXTURE
{
	char	FileName[256];	//テクスチャのアドレス	texture address
	char	ID[15];			//ID
	int		ObjectType;		//オブジェクトの種類
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadFBX(char* FileName, MODEL_DATA* modelData);

//=============================================================================
// 初期化処理
//=============================================================================
void LoadFBXModel(char* FileName, DX11_MODEL* Model, MODEL_DATA* ModelData)
{

	LoadFBX(FileName, ModelData);
	
	if (ModelData->MeshNum <= 0 || ModelData->MeshNum > 1000) return;	//Invalid file

	Model->BuffersNum = ModelData->MeshNum;
	Model->Buffers = new DX11_BUFFERS[Model->BuffersNum];

	for (int j = 0; j < ModelData->MeshNum; j++)
	{
		// 頂点バッファ生成
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(VERTEX_3D) * ModelData->Mesh[j].VertexNum;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = ModelData->Mesh[j].VertexArray;

			GetDevice()->CreateBuffer(&bd, &sd, &Model->Buffers[j].VertexBuffer);
		}


		// インデックスバッファ生成
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned short) * ModelData->Mesh[j].IndexNum;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = ModelData->Mesh[j].IndexArray;

			GetDevice()->CreateBuffer(&bd, &sd, &Model->Buffers[j].IndexBuffer);
		}

		// サブセット設定
		{
			Model->Buffers[j].SubsetArray	= new DX11_FBXSUBSET[ModelData->Mesh[j].SubsetNum];
			Model->Buffers[j].SubsetNum		= ModelData->Mesh[j].SubsetNum;
			Model->Buffers[j].SubsetArray[0].Material.Texture = NULL;

			for (unsigned short i = 0; i < ModelData->Mesh[j].SubsetNum; i++)
			{
				Model->Buffers[j].SubsetArray[i].StartIndex			= ModelData->Mesh[j].SubsetArray[i].StartIndex;
				Model->Buffers[j].SubsetArray[i].IndexNum			= ModelData->Mesh[j].SubsetArray[i].IndexNum;
				Model->Buffers[j].SubsetArray[i].Material.Material	= ModelData->Mesh[j].SubsetArray[i].Material.Material;
				Model->Buffers[j].SubsetArray[i].Material.Texture	= NULL;

				D3DX11CreateShaderResourceViewFromFile(GetDevice(),
					ModelData->Mesh[j].SubsetArray[i].Material.TextureName,
					NULL,
					NULL,
					&Model->Buffers[j].SubsetArray[i].Material.Texture,
					NULL);
			}
		}

	}

}


//=============================================================================
// 終了処理
//=============================================================================
void UnloadFBXModel(DX11_MODEL* Model, MODEL_DATA* modelData)
{
	//MODEL unload
	for(int i = 0; i < Model->BuffersNum; i++)
	{
		for (unsigned short j = 0; j < Model->Buffers[i].SubsetNum; j++)
		{
			if (Model->Buffers[i].SubsetArray[j].Material.Texture)
			{
				Model->Buffers[i].SubsetArray[j].Material.Texture->Release();
				Model->Buffers[i].SubsetArray[j].Material.Texture = NULL;
			}
		}


		if (Model->Buffers[i].VertexBuffer)	Model->Buffers[i].VertexBuffer->Release();
		if (Model->Buffers[i].IndexBuffer)	Model->Buffers[i].IndexBuffer->Release();
		if (Model->Buffers[i].SubsetArray)	delete[] Model->Buffers[i].SubsetArray;

	}
	delete[] Model->Buffers;


	//MODEL DATA unload
	{
		for (int j = 0; j < modelData->MeshNum ; j++)
		{
			delete[] modelData->Mesh[j].VertexArray;
			delete[] modelData->Mesh[j].IndexArray;
			delete[] modelData->Mesh[j].SubsetArray;
		}
		delete[] modelData->Mesh;

		delete[] modelData->Transform;

	}

}


//=============================================================================
// 描画処理
//=============================================================================
void DrawFBXModel(DX11_MODEL* Model, MODEL_DATA  modelData, XMMATRIX worldMatix)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;
	
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;

	int ModelsDiference = (modelData.TransformNum - modelData.MeshNum);	//描画しないモデルの為　//For the empty models

	for (int j = 0; j < modelData.TransformNum; j++)
	{

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(modelData.Transform[j].LclScaling.x, modelData.Transform[j].LclScaling.y, modelData.Transform[j].LclScaling.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(modelData.Transform[j].LclRotation.x, modelData.Transform[j].LclRotation.y, modelData.Transform[j].LclRotation.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(modelData.Transform[j].LclPosition.x, modelData.Transform[j].LclPosition.y, modelData.Transform[j].LclPosition.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// 子供だったら親と結合する
		if (modelData.Transform[j].FatherIdx >= 0 && modelData.Transform[j].FatherIdx < modelData.TransformNum)
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&modelData.Transform[modelData.Transform[j].FatherIdx].mtxWorld));	
		}

		//マトリクスをセーブする
		XMStoreFloat4x4(&modelData.Transform[j].mtxWorld, mtxWorld);

		int drawIdx = j - ModelsDiference;	//描画するためのインデックス

		if (drawIdx < 0) continue;	//空のモデルの場合は描画しない


		//World matrixと結合する
		mtxWorld = XMMatrixMultiply(mtxWorld, worldMatix);


		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);


		//************************

		GetDeviceContext()->IASetVertexBuffers(0, 1, &Model->Buffers[drawIdx].VertexBuffer, &stride, &offset);

		// インデックスバッファ設定
		GetDeviceContext()->IASetIndexBuffer(Model->Buffers[drawIdx].IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// プリミティブトポロジ設定
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (unsigned short i = 0; i < Model->Buffers[drawIdx].SubsetNum; i++)
		{
			// マテリアル設定
			SetMaterial(Model->Buffers[drawIdx].SubsetArray[i].Material.Material);

			// テクスチャ設定
			if (Model->Buffers[drawIdx].SubsetArray[i].Material.Material.noTexSampling == 0)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &Model->Buffers[drawIdx].SubsetArray[i].Material.Texture);
			}

			// ポリゴン描画
			GetDeviceContext()->DrawIndexed(Model->Buffers[drawIdx].SubsetArray[i].IndexNum, Model->Buffers[drawIdx].SubsetArray[i].StartIndex, 0);

		}

	}
}



//=============================================================================
// モデル読込
//=============================================================================
void LoadFBX(char* FileName, MODEL_DATA* modelData)
{
	int	GeometryNum = 0;
	int	MaterialNum = 0;
	int	ModelNum	= 0;
	int	TextureNum	= 0;

	int	ConnectionNum = 0;

	FBX_GEOMETRY* Geometrys = NULL;	//	ジオメトリ	Aqui se guardan todos los geometry
	FBX_MATERIAL* Materials = NULL;	//	マテリアル	Aqui se guardan todos los materials
	FBX_MODEL*	  Models	= NULL;	//	モデル		Aqui se guardan todos los model
	FBX_TEXTURE*  Textures	= NULL; //	テクスチャ	Aqui se guardan todos las texturas

	FBX_CONNECTIONS* Connections = NULL; //接続		Aqui se guardan todos laa conexiones





	XMFLOAT3*	   VertexPos		= NULL;			//頂点座標
	unsigned short positionNum		= 1;			//頂点の数				Numero de VertexPos

	int*		   FBXVertexIndex	= NULL;			//インデックス
	unsigned short FBXVertexIdxNum	= 1;			//インデックスの数		Numero de VertexIndex


	XMFLOAT2*	   UVcoordArray		= NULL;			//UV座標
	unsigned short UVNum			= 1;			//UVの数				Numero de UVs	Cada vez que hay una (vt) suma 1. ej:vt 0.375000 0.000000

	int*		   UVIndexArray		= NULL;			//UVインデックス
	unsigned short UVIndexNum		= 1;			//UVインデックスの数	Numero de index de UVs



	char str[256];	//ドキュメントを読むための変数	//varibles para leer el documento


	FILE* file;
	file = fopen(FileName, "rt");  //rt → open for reading (default), text mode (default)
	if (file == NULL)
	{
		printf("エラー:LoadModel %s \n", FileName);
		return;
	}



	//要素数カウント
	while (TRUE)
	{	
		fscanf(file, "%s", str);	//info url: https://www.tutorialspoint.com/c_standard_library/c_function_fscanf.htm


		if (feof(file) != 0)		//info url: https://www.tutorialspoint.com/c_standard_library/c_function_feof.htm
			break;


		//FBXの定義欄。変数の初期化
		if (strcmp(str, "Definitions:") == 0)
		{
			while (strcmp(str, "Objects:") != 0)
			{
				fscanf(file, "%s", str);

				//Geometryの初期化
				if (strcmp(str, "\"Geometry\"") == 0)
				{
					fscanf(file, "%*s%*s%s", str);
					GeometryNum = atoi(str);

					Geometrys = new FBX_GEOMETRY[GeometryNum];

					for (int i = 0; i < GeometryNum; i++)
					{
						Geometrys[i].ObjectType = OBJECT_TYPE::TYPE_GEOMETRY;
						Geometrys[i].ModelAttachedIdx = -1;
					}
					Geometrys->TexCoord = NULL;		//fbxにUV座標がない場合		hecho para que en el caso de que el fbx no tenga coodenadas poder saberlo. Se comprueba abajo en un if si es igual a NULL

				}
				//Materialの初期化
				else if (strcmp(str, "\"Material\"") == 0)
				{
					fscanf(file, "%*s%*s%s", str);
					MaterialNum = atoi(str);

					Materials = new FBX_MATERIAL[MaterialNum];
					ZeroMemory(Materials, sizeof(FBX_MATERIAL) * MaterialNum);


					for (int i = 0; i < MaterialNum; i++)
					{
						Materials[i].ObjectType = OBJECT_TYPE::TYPE_MATERIAL;
					}
				}
				//Modelの初期化
				else if (strcmp(str, "\"Model\"") == 0)
				{
					fscanf(file, "%*s%*s%s", str);
					ModelNum = atoi(str);

					Models = new FBX_MODEL[ModelNum];

					//初期化
					for (int i = 0; i < ModelNum; i++)
					{
						Models[i].ObjectType = OBJECT_TYPE::TYPE_MODEL;
						Models[i].LclPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
						Models[i].LclRotation	 = XMFLOAT3(0.0f, 0.0f, 0.0f);
						Models[i].LclScaling	 = XMFLOAT3(1.0f, 1.0f, 1.0f);
						Models[i].PivotPos		 = XMFLOAT3(0.0f, 0.0f, 0.0f);
						Models[i].FatherIdx		 = -1;
					}

				}
				//Textureの初期化
				else if (strcmp(str, "\"Texture\"") == 0)
				{
					fscanf(file, "%*s%*s%s", str);
					TextureNum = atoi(str);

					Textures = new FBX_TEXTURE[TextureNum];

					//初期化
					for (int i = 0; i < TextureNum; i++)
					{
						Textures[i].ObjectType = OBJECT_TYPE::TYPE_TEXTURE;
					}

				}
			}
		}


		//オブジェクトのプロパティを検索する		Busqueda de las propiedades del objeto 
		if (strcmp(str, "Objects:") == 0)
		{
			unsigned short curGeometry = -1; //現在のジオメトリ		Geometry actual
			unsigned short curModel	   = -1; //現在のモデル　		Model	actual
			unsigned short curMaterial = -1; //現在のマテリアル		Material actual
			unsigned short curTexture  = -1; //現在のテクスチャ		Textura  actual

			//FBXの接続の欄まで検索する
			while (strcmp(str, "Connections:") != 0)  
			{

				fscanf(file, "%255s", str); //読み取りは255文字まで		limita la lectura a 255 caracteres

				//ジオメトリ情報（この中頂点情報が含まれる）
				if (strcmp(str, "Geometry:") == 0)
				{
					curGeometry++;

					fscanf(file, "%13s", str); //idを取得する			consigue el id con la coma al final

					strcpy(Geometrys[curGeometry].ID, str);
				}
				//頂点座標
				if (strcmp(str, "Vertices:") == 0)
				{
					int coordNum;						//頂点座標の数	numero de coordenadas de vertices
					fscanf(file, "%*c%*c%d", &coordNum);//2文字を読み飛ばし、次のintを読む。salta dos caracteres y lee el siguiente int

					positionNum = coordNum / 3;		//オブジェクトの頂点数（三角形の場合のみ）numero de vertices del objeto (solo para triangulos)
					VertexPos = new XMFLOAT3[positionNum];


					char curChar;				//ファイルを読み、各文字を頂点の文字列に追加する。	character que va a ir recorriendo el file y añadiendo cada letra al string de los vertices
					char appendstr[2];			//vertexPosStrとcurCharの間のstrcatを作る文字列。	string para poder hacer el strcat entre vertexPosStr y curChar
					int Idx = 0;				//次の座標が格納されるインデックスを格納する配列	array que guarda el indice donde se guardara la proxima coordenada
					int Axis = 0;				//次の座標（x、y、z）が格納される軸を格納する配列。	array que guarda en que eje se guardara la proxima coordenada (x, y, z)

					str[0] = '\0';
					appendstr[1] = '\0';

					fscanf(file, "%*s%*s%c", &curChar);		//頂点の座標がある文字列を取る。	se coge la cadena en la que estan las coordenadas de los vertices

					do
					{
						curChar = fgetc(file);	// 似たようなもの → //fscanf(file, "%c", curChar);

						switch (curChar)
						{
						case ',':
						case '}':

							if		(Axis == 0) VertexPos[Idx].x = strtof(str, NULL) * SCALE_MODEL;
							else if (Axis == 1) VertexPos[Idx].y = strtof(str, NULL) * SCALE_MODEL;
							else if (Axis == 2) VertexPos[Idx].z = strtof(str, NULL) * SCALE_MODEL;

							Axis = (Axis + 1) % 3;		//次の軸			Suma uno para el siguiente eje x, y, z
							Idx = Axis ? Idx : ++Idx;	//次のインデクス	Suma uno a posArrayIdx en caso de que posArrayaxis sea 0 (falso), lo que significa que ha vuelto al eje x

							str[0] = '\0'; 
							break;

						case ' ':
						case '\n':
						case '\r':	//retorno de carro
						case '\t':	//tabulacion
							//No hacer nada / 何もしない
							break;

						default:
							appendstr[0] = curChar;
							strcat(str, appendstr);
							break;
						}

					} while (curChar != '}');


				}
				//頂点インデクス
				else if (strcmp(str, "PolygonVertexIndex:") == 0)
				{
					fscanf(file, "%*c%*c%hd", &Geometrys[curGeometry].FBXIndexNum);

					Geometrys[curGeometry].VertexNum = Geometrys[curGeometry].FBXIndexNum;
					Geometrys[curGeometry].Position = new XMFLOAT3[Geometrys[curGeometry].FBXIndexNum];

					Geometrys[curGeometry].FBXIndexNum = Geometrys[curGeometry].FBXIndexNum;
					Geometrys[curGeometry].IndexArray = new int[Geometrys[curGeometry].FBXIndexNum];


					char curChar = NULL;
					int  Idx = 0;
					str[0] = '\0';
					char appendstr[2];			
					appendstr[1] = '\0';

					fscanf(file, "%*s%*s%c", &curChar);		


					//四角対応のため、頂点数え
					unsigned short CurFBXVertexCnt = 0;		//ポリゴンの頂点数
					unsigned short CurRealindexCnt = 0;		//GeometryのIndex数

					do
					{
						curChar = fgetc(file);	// igual que → //fscanf(file, "%c", curChar);

						switch (curChar)
						{
						case ',':
						case '}':

							if (atoi(str) < 0)
							{
								CurRealindexCnt++;
								CurFBXVertexCnt++;

								Geometrys[curGeometry].IndexArray[Idx] = atoi(str);

								int curIdx = Geometrys[curGeometry].IndexArray[Idx];
								curIdx = (curIdx * -1) - 1;		// it’s because the original index is XOR’ed with -1. So for example the index 3 becomes -4
								Geometrys[curGeometry].Position[Idx] = VertexPos[curIdx];

								if (CurFBXVertexCnt == 4)
								{
									CurRealindexCnt += 2;
									CurFBXVertexCnt = 0;
								}

							}
							else
							{
								CurRealindexCnt++;
								CurFBXVertexCnt++;

								Geometrys[curGeometry].IndexArray[Idx] = atoi(str);

								Geometrys[curGeometry].Position[Idx] = VertexPos[Geometrys[curGeometry].IndexArray[Idx]];
							}

							Idx++;	

							str[0] = '\0';
							break;

						case ' ':
						case '\n':
						case '\r'://retorno de carro
						case '\t'://tabulacion
							//No hacer nada / 何もしない
							break;

						default:
							appendstr[0] = curChar;
							strcat(str, appendstr);
							break;
						}

					} while (curChar != '}');

					Geometrys[curGeometry].RealIndexNum = CurRealindexCnt;

					delete[] VertexPos;
				}
				//頂点の法線
				else if (strcmp(str, "Normals:") == 0)
				{
					int normalCoordNum;						//法線の座標数（法線ごとに 3　x, y, z）numero de coord de normales (3 por cada normal x, y, z)

					fscanf(file, "%*c%*c%d", &normalCoordNum);

					int normalNum = normalCoordNum / 3;		//オブジェクトの法線の数	numero de normales del objeto 
					Geometrys[curGeometry].Normal = new XMFLOAT3[normalNum];	//初期化		//inicializa todos las normales (sin valores) (pointer)

					char curChar;				   //ファイルを読み、各文字を頂点の文字列に追加する。	
					char appendstr[2];			   //vertexPosStrとcurCharの間のstrcatを作る文字列。	
					int Idx = 0;				   //次の座標が格納されるインデックスを格納する配列	
					int Axis = 0;				   //次の座標（x、y、z）が格納される軸を格納する配列。

					str[0] = '\0';
					appendstr[1] = '\0';

					fscanf(file, "%*s%*s%c", &curChar);		//頂点の座標がある文字列を取る。	se coge la cadena en la que estan las coordenadas de los vertices

					do
					{
						curChar = fgetc(file);	// 似たようなもの → //fscanf(file, "%c", curChar);

						switch (curChar)
						{
						case ',':
						case '}':

							if		(Axis == 0) Geometrys[curGeometry].Normal[Idx].x = strtof(str, NULL);
							else if (Axis == 1) Geometrys[curGeometry].Normal[Idx].y = strtof(str, NULL);
							else if (Axis == 2) Geometrys[curGeometry].Normal[Idx].z = strtof(str, NULL);

							Axis = (Axis + 1) % 3;		//次の軸			Suma uno para el siguiente eje x, y, z
							Idx = Axis ? Idx : ++Idx;	//次のインデクス	Suma uno a posArrayIdx en caso de que posArrayaxis sea 0 (falso), lo que significa que ha vuelto al eje x

							str[0] = '\0'; 
							break;

						case ' ':
						case '\n':
						case '\r'://retorno de carro
						case '\t'://tabulacion
							//No hacer nada / 何もしない
							break;

						default:
							appendstr[0] = curChar;
							strcat(str, appendstr);
							break;
						}

					} while (curChar != '}');

				}
				//UV情報
				else if (strcmp(str, "UV:") == 0)
				{
					int UVCoordNum;						//UVの座標数（UVごとに 3　x, y, z)		numero de UV (2 por cada UV x, y)
					fscanf(file, "%*c%*c%d", &UVCoordNum);

					UVNum = UVCoordNum / 2;		//UVの座標数	numero de UV del objeto 

					UVcoordArray = new XMFLOAT2[UVNum];

					char curChar;		//	//ファイルを読み、各文字を頂点の文字列に追加する。		character que va a ir recorriendo el file y añadiendo cada letra al string de las normales
					char appendstr[2];	//	//vertexPosStrとcurCharの間のstrcatを作る文字列。		string para poder hacer el strcat entre normalCoorStr y curChar
					int Idx = 0;		//	//次の座標が格納されるインデックスを格納する配列		array que guarda el indice donde se guardara la proxima coordenada
					int Axis = 0;		//	//次の座標（x、y、z）が格納される軸を格納する配列。		array que guarda en que eje se guardara la proxima coordenada (x, y, z)

					str[0] = '\0';
					appendstr[1] = '\0';

					fscanf(file, "%*s%*s%c", &curChar);		//座標がある文字列を取る。se coge la cadena en la que estan las coordenadas de los vertices

					do
					{
						curChar = fgetc(file);	// igual que → //fscanf(file, "%c", curChar);

						switch (curChar)
						{
						case ',':
						case '}':

							if		(Axis == 0) UVcoordArray[Idx].x = strtof(str, NULL);
							else if (Axis == 1) UVcoordArray[Idx].y = 1.0f - strtof(str, NULL);	//El documento da las coordenadas como si el origen fuese abajo a la izq, por lo que se hace esta operacion para ponerlo arriba a la izq

							Axis = (Axis + 1) % 2;		//次の軸			Suma uno para el siguiente eje x, y, z
							Idx = Axis ? Idx : ++Idx;	//次のインデクス	Suma uno a posArrayIdx en caso de que posArrayaxis sea 0 (falso), lo que significa que ha vuelto al eje x

							str[0] = '\0'; 
							break;

						case ' ':
						case '\n':
						case '\r'://retorno de carro
						case '\t'://tabulacion
							//No hacer nada / 何もしない
							break;

						default:
							appendstr[0] = curChar;
							strcat(str, appendstr);
							break;
						}

					} while (curChar != '}');

				}
				//UVインデクス
				else if (strcmp(str, "UVIndex:") == 0)
				{

					fscanf(file, "%*c%*c%hd", &UVIndexNum);

					UVIndexArray = new int[UVIndexNum];	//初期化	inicializa todos los indices (sin valores) (pointer)

					char curChar;		//ファイルを読み、各文字を頂点の文字列に追加する。	character que va a ir recorriendo el file y añadiendo cada letra al string de las normales
					char appendstr[2];	//vertexPosStrとcurCharの間のstrcatを作る文字列。	string para poder hacer el strcat entre str y curChar
					int Idx = 0;		//次の座標が格納されるインデックスを格納する配列	array que guarda el indice de la variable a la que guardar
										  
					str[0] = '\0';
					appendstr[1] = '\0';

					fscanf(file, "%*s%*s%c", &curChar);		//座標がある文字列を取る。　se coge la cadena en la que estan las coordenadas de los vertices

					do
					{
						curChar = fgetc(file);	// 似たようなもの → //fscanf(file, "%c", curChar);

						switch (curChar)
						{
						case ',':
						case '}':

							UVIndexArray[Idx] = atoi(str);

							Idx++;

							str[0] = '\0'; 
							break;

						case ' ':
						case '\n':
						case '\r'://retorno de carro
						case '\t'://tabulacion
							//No hacer nada / 何もしない
							break;

						default:
							appendstr[0] = curChar;
							strcat(str, appendstr);
							break;
						}

					} while (curChar != '}');

					//すべてのuvをそれぞれの場所（uvとuvインデックス）に配置し、Diffuseを割り当てる。	Colocar todos los uvs en sus lugares(uv y el indice de los uvs) y asignar Diffuse
					Geometrys[curGeometry].TexCoord = new XMFLOAT2[UVIndexNum];
					Geometrys[curGeometry].Diffuse  = new XMFLOAT4[UVIndexNum]; 

					for (int i = 0; i < UVIndexNum; i++)
					{
						Geometrys[curGeometry].TexCoord[i] = UVcoordArray[UVIndexArray[i]];

						//Difusse
						Geometrys[curGeometry].Diffuse[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					}

					delete[] UVcoordArray;
					delete[] UVIndexArray;
				}
				//マテリアルの情報
				else if (strcmp(str, "Materials:") == 0)
				{
					unsigned short LinkedMatPerPolygonNum;						
					fscanf(file, "%*c%*c%hd", &LinkedMatPerPolygonNum);//salta dos caracteres y lee el siguiente int

					Geometrys[curGeometry].MaterialPerVertex = new unsigned short[Geometrys[curGeometry].FBXIndexNum];

					if (LinkedMatPerPolygonNum > 1)		//もし1つのマテリアルしかない場合、全面に同じマテリアルを配置する代わりに、情報にゼロを入れるだけだ		este if es necesario porque en caso de que solo sea un material, solo pone un cero en la info en vez de poner ese mismo material por todas las caras
					{
						char curChar;				//ファイルを読み、各文字を頂点の文字列に追加する。		character que va a ir recorriendo el file y añadiendo cada letra al string de los vertices
						char appendstr[2];			//vertexPosStrとcurCharの間のstrcatを作る文字列。		string para poder hacer el strcat entre vertexPosStr y curChar
						int  Idx = -1;				//次の座標が格納されるインデックスを格納する配列		array que guarda el indice donde se guardara la proxima coordenada

						str[0] = '\0';
						appendstr[1] = '\0';

						fscanf(file, "%*s%*s%c", &curChar);		//座標がある文字列を取る。　se coge la cadena en la que estan las coordenadas de los vertices

						do
						{
							curChar = fgetc(file);	// 似たようなもの → //fscanf(file, "%c", curChar);

							switch (curChar)
							{
							case ',':
							case '}':

								do 
								{
									Idx++;
									Geometrys[curGeometry].MaterialPerVertex[Idx] = (unsigned short)atoi(str);
									
								} while (Geometrys[curGeometry].IndexArray[Idx] >= 0);


								str[0] = '\0'; 
								break;

							case ' ':
							case '\n':
							case '\r'://retorno de carro
							case '\t'://tabulacion
								//No hacer nada / 何もしない
								break;

							default:
								appendstr[0] = curChar;
								strcat(str, appendstr);
								break;
							}

						} while (curChar != '}');

					}
					else
					{
						for (int i = 0; i < Geometrys[curGeometry].FBXIndexNum; i++)
						{
							Geometrys[curGeometry].MaterialPerVertex[i] = 0;
						}
					}

				}
				//モデルの情報（オブジェクトのトランスフォーム情報も含まれる）
				else if (strcmp(str, "Model:") == 0)
				{
					curModel++;

					fscanf(file, "%13s", str); //IDを取得　　　consigue el id con la coma al final

					strcpy(Models[curModel].ID, str);

					fscanf(file, "%*c%*c%*c%*c%*c%*c%*c%*c%*c%*c%s", str); //名前取得	consigue el nombre con la coma al final
					char* quotePos = strchr(str, '\"');
					quotePos[0] = '\0';

					strcpy(Models[curModel].ObjectName, str);

				}
				//モデルの座標
				else if (strcmp(str, "\"Lcl") == 0)
				{
					fscanf(file, "%s", str);
					if (strcmp(str, "Translation\",") == 0 ||
						strcmp(str, "Scaling\",")     == 0)
					{
						char dataType = str[0];
						XMFLOAT3 modelsData;

						fscanf(file, "%*s%*s%*s%*c%*c%*c%*c%*c%s", &str);

						char* token = strtok(str, ",");

						modelsData.x  = strtof(token, NULL);
						if(dataType == 'T') modelsData.x *= SCALE_MODEL;
						token   = strtok(NULL, ",");

						modelsData.y  = strtof(token, NULL);
						if (dataType == 'T') modelsData.y *= SCALE_MODEL;
						token   = strtok(NULL, ",");

						modelsData.z  = strtof(token, NULL);
						if (dataType == 'T') modelsData.z *= SCALE_MODEL;



						switch (dataType)
						{
						case 'T':
							Models[curModel].LclPosition = modelsData;
							break;

						case 'S':
							Models[curModel].LclScaling = modelsData;
							break;

						}
					}
					//モデルの回転
					else if(strcmp(str, "Rotation\",") == 0)
					{
						char dataType = str[0];
						XMFLOAT3 modelsData;

						fscanf(file, "%*s%*s%*s%*c%*c%*c%*c%*c%s", &str);

						char* token = strtok(str, ",");

						modelsData.x = strtof(token, NULL);
						modelsData.x *= SCALE_MODEL;
						token = strtok(NULL, ",");

						modelsData.y = strtof(token, NULL);
						modelsData.y *= SCALE_MODEL;
						token = strtok(NULL, ",");

						modelsData.z = strtof(token, NULL);
						modelsData.z *= SCALE_MODEL;

						Models[curModel].LclRotation = modelsData;

					}

				}
				//モデルの回転の原点
				else if (strcmp(str, "\"RotationPivot\",") == 0)
				{

					fscanf(file, "%*s%*s%*c%*c%*c%s", &str);//salta dos caracteres y lee el siguiente int

					char* token = strtok(str, ",");
					
					Models[curModel].PivotPos.x = strtof(token, NULL);
					Models[curModel].PivotPos.x *= SCALE_MODEL;
					token = strtok(NULL, ",");

					Models[curModel].PivotPos.y = strtof(token, NULL);
					Models[curModel].PivotPos.y *= SCALE_MODEL;
					token = strtok(NULL, ",");

					Models[curModel].PivotPos.z = strtof(token, NULL);
					Models[curModel].PivotPos.z *= SCALE_MODEL;

				}

				//マテリアルの情報
				else if (strcmp(str, "Material:") == 0)
				{
					curMaterial++;

					fscanf(file, "%13s", str); //idを取得　	consigue el id con la coma al final

					strcpy(Materials[curMaterial].ID, str);

				}
				else if (strcmp(str, "\"AmbientColor\",")  == 0 || 
						 strcmp(str, "\"DiffuseColor\",")  == 0 ||
						 strcmp(str, "\"SpecularColor\",") == 0 || 
						 strcmp(str, "\"Emissive\",")	   == 0 )
				{

					char dataType = str[1];	//最初の文字で、どの変数に代入すべきかを知る。coge la primera letra para despues saber a que variable adjudicarla
					XMFLOAT4 matData;

					if (dataType == 'E')fscanf(file, "%*s%*s%*c%*c%*c%*c%s", str);
					else				fscanf(file, "%*s%*s%*c%*c%*c%*c%*c%s", str);

					char* token = strtok(str, ",");

					matData.x = strtof(token, NULL);
					token = strtok(NULL, ",");

					matData.y = strtof(token, NULL);
					token = strtok(NULL, ",");

					matData.z = strtof(token, NULL);

					matData.w = 1.0f;

					switch (dataType)
					{
					case 'A':
						Materials[curMaterial].AmbientColor = matData;
						break;

					case 'D':
						Materials[curMaterial].DiffuseColor = matData;
						break;

					case 'S':
						Materials[curMaterial].SpecularColor = matData;
						break;

					case 'E':
						Materials[curMaterial].EmissiveColor = matData;

						if(Materials[curMaterial].EmissiveColor.x == 0 &&
						   Materials[curMaterial].EmissiveColor.y == 0 &&
						   Materials[curMaterial].EmissiveColor.z == 0)
						{
							Materials[curMaterial].EmissiveColor.w = 0;
						}
						break;
					}

				}
				else if (strcmp(str, "\"Shininess\",") == 0)
				{
					fscanf(file, "%*s%*s%*c%*c%*c%*c%f", &Materials[curMaterial].Shininess);

				}


				//テクスチャの情報
				else if (strcmp(str, "Texture:") == 0)
				{
					curTexture++;

					fscanf(file, "%13s", str); //idを取得	consigue el id con la coma al final

					strcpy(Textures[curTexture].ID, str);
				}
				//テクスチャのアドレス
				else if (strcmp(str, "FileName:") == 0)
				{
					//テクスチャのルートを取得
					char path1[256];
					char path2[256];
					strcpy(path1, FileName);
					fgets(path2, 255, file);
					
					char* fileName2 = strrchr(path2, '/');
					if (!fileName2) {
						fileName2 = strrchr(path2, '\\');
					}
					if (fileName2) {
						fileName2++; 
					}

					
					char* fileRute = strstr(path1, "data");

					char* fileName11 = strrchr(fileRute, '\\');
					if(fileName11 == NULL)fileName11 = strrchr(path1, '/');
					
					if (fileName11) {
						fileName11++; 
						*fileName11 = '\0'; 
					}

					
					strcat(fileRute, fileName2);
					
					char* quotePos = strchr(fileRute, '\"');
					quotePos[0] = '\0';
					strcpy(Textures[curTexture].FileName, fileRute);
				}
				else if (strcmp(str, "AnimationCurve:") == 0)
				{
					//アニメーションファイル、無効
					return;
				}


			}
		}
		//接続欄
		if (strcmp(str, "Connections:") == 0)
		{
			long posicionInicial = ftell(file);		//Connectionsが始まる所 
			int curConnections = -1;				//Geometry actual


			//Connectionsの数を数える
			while (strcmp(str, "}") != 0)
			{
				fscanf(file, "%255s", str); 

				if (strcmp(str, "C:") == 0)
				{
					ConnectionNum++;
				}

			}

			Connections = new FBX_CONNECTIONS[ConnectionNum];

			fseek(file, posicionInicial, SEEK_SET);		//Connectionsが始まる所まで戻ります
			strcpy(str, "");

			//Connectionsの内容を取得
			while (strcmp(str, "}") != 0)
			{

				fscanf(file, "%255s", str); //limita la lectura a 255 caracteres

				if (str[0] == ';')
				{
					curConnections++;

					char* objType = str;	
					char* objTypeFinal = 0;

					objType++;

					objTypeFinal = strchr(objType, ':');	//Returns a pointer to the first occurrence of character in the C string str.
					*objTypeFinal = '\0';

					//子供の種類を設定
					if (strcmp(objType, "Model") == 0)
					{
						Connections[curConnections].ObjectTypeChild = OBJECT_TYPE::TYPE_MODEL;
					}
					else if (strcmp(objType, "Geometry") == 0)
					{
						Connections[curConnections].ObjectTypeChild = OBJECT_TYPE::TYPE_GEOMETRY;
					}
					else if (strcmp(objType, "Material") == 0)
					{
						Connections[curConnections].ObjectTypeChild = OBJECT_TYPE::TYPE_MATERIAL;
					}
					else if (strcmp(objType, "Texture") == 0)
					{
						Connections[curConnections].ObjectTypeChild = OBJECT_TYPE::TYPE_TEXTURE;
					}

					fscanf(file, "%255s", str); //limita la lectura a 255 caracteres
					
					//親の種類を設定

					objType = str;
					objTypeFinal = 0;

					objTypeFinal = strchr(objType, ':');	//C の文字列 str に最初に現れる文字へのポインタを返します。	Returns a pointer to the first occurrence of character in the C string str.

					if (objTypeFinal == NULL)	//名前が連結されていない場合があるので、次の文字列を検索するときに、こちらが望まないものを拾ってしまったり、':'を検索するときにNULLを返してしまったりする。	Hay veces que los nombres no están unidos, por lo que al buscar la proxima str coge algo que no queremos y al buscar ':' sale NULL. Esto es una contramedida para eso
					{
						while (objTypeFinal == NULL)
						{
							fscanf(file, "%255s", objType); 
							objTypeFinal = strchr(objType, ':');	
						}
					}

					*objTypeFinal = '\0';

					if (strcmp(objType, "Model") == 0)
					{
						Connections[curConnections].ObjectTypeFather = OBJECT_TYPE::TYPE_MODEL;
					}
					else if (strcmp(objType, "Material") == 0)
					{
						Connections[curConnections].ObjectTypeFather = OBJECT_TYPE::TYPE_MATERIAL;
					}

				}
				//子供と親のIDを取得する
				else if (strcmp(str, "C:") == 0)
				{
					if (curConnections == -1) curConnections = 0;
					fscanf(file, "%*c%*c%*c%*c%*c%*c%13s", str);
					strcpy(Connections[curConnections].IdChild, str);

					fscanf(file, "%*c%13s", str);
					strcpy(Connections[curConnections].IdFather, str);

				}
			}
		}
	}
	fclose(file);

	if (ModelNum == 0) return;	//Unvalid File

	//モデルのTRANSFORMをセーブする
	modelData->TransformNum = ModelNum;
	modelData->Transform = new TRANSFORM_DATA[modelData->TransformNum];

	for (int i = 0; i < ModelNum; i++)
	{
		modelData->Transform[i].LclPosition   = Models[i].LclPosition;
		modelData->Transform[i].LclRotation	   = Models[i].LclRotation;
		modelData->Transform[i].LclScaling	   = Models[i].LclScaling;

		ZeroMemory(modelData->Transform[i].ObjectName, sizeof(char) * 50);
		strcpy(modelData->Transform[i].ObjectName, Models[i].ObjectName);
	}


	//接続を行う
	for (int i = 0; i < ConnectionNum; i++)
	{
		XMVECTOR zeroVector = XMVECTOR{ 0, 0, 0, 0 };

		int childIdx = -1;
		int fatherIdx = -1;

		switch (Connections[i].ObjectTypeChild)
		{
		case TYPE_MODEL:
			for (int j = 0; j < ModelNum; j++)
			{
				if (strcmp(Models[j].ID, Connections[i].IdChild) == 0)
				{
					childIdx = j;
					break;
				}
			}
			for (int j = 0; j < ModelNum; j++)
			{
				if (strcmp(Models[j].ID, Connections[i].IdFather) == 0)
				{
					fatherIdx = j;
					break;
				}
			}


			if (childIdx != -1 && fatherIdx != -1)
			{
				Models[childIdx].FatherIdx = fatherIdx;
				modelData->Transform[childIdx].FatherIdx = fatherIdx;

			}


			break;

		case TYPE_GEOMETRY:
			for (int j = 0; j < GeometryNum; j++)
			{
				if (strcmp(Geometrys[j].ID, Connections[i].IdChild) == 0)
				{
					childIdx = j;
					break;
				}
			}
			for (int j = 0; j < ModelNum; j++)
			{
				if (strcmp(Models[j].ID, Connections[i].IdFather) == 0)
				{
					fatherIdx = j;
					break;
				}
			}

			if (childIdx != -1 && fatherIdx != -1)
			{
				strcpy(Geometrys[childIdx].FatherID, Models[fatherIdx].ID);
				Geometrys[childIdx].ModelAttachedIdx = fatherIdx;



				//モデル内のマテリアルの数を初期化する	inicializar el numero de materiales del Model
				Models[fatherIdx].MatNumInModel = 0;

				for (int j = 0; j < Geometrys[childIdx].FBXIndexNum; j++)
				{
					if (Geometrys[childIdx].MaterialPerVertex[j] > Models[fatherIdx].MatNumInModel)
					{
						Models[fatherIdx].MatNumInModel = Geometrys[childIdx].MaterialPerVertex[j];
					}
				}
				Models[fatherIdx].MatNumInModel += 1;	//MaterialPerVertexは0から始まるインデックスなので、1が追加される。	Se suma 1 porque MaterialPerVertex son indices que empiezan desde 0

				for (int z = 0; z < Models[fatherIdx].MatNumInModel; z++)
				{
					Models[fatherIdx].LinkedMatID[z] = new char[15];	
					ZeroMemory(Models[fatherIdx].LinkedMatID[z], sizeof(char) * 15);

				}
			}



			break;


		case TYPE_MATERIAL:
			for (int j = 0; j < MaterialNum; j++)
			{
				if (strcmp(Materials[j].ID, Connections[i].IdChild) == 0)
				{
					childIdx = j;
					break;
				}
			}
			for (int j = 0; j < ModelNum; j++)
			{
				if (strcmp(Models[j].ID, Connections[i].IdFather) == 0)
				{
					fatherIdx = j;
					break;
				}
			}


			if (childIdx != -1 && fatherIdx != -1)
			{
				int curMaterialIdx = 0;
							//WARNING Puede sobrepasar la memoria que se ha asegurado al inicializar Models[fatherIdx].LinkedMatID
				while (1)	//Mientras no encuentre un espacio libre para el proximo iD
				{
					if (strcmp(Models[fatherIdx].LinkedMatID[curMaterialIdx], "") == 0)
					{
						strcpy(Models[fatherIdx].LinkedMatID[curMaterialIdx], Materials[childIdx].ID);
						break;
					}
					curMaterialIdx++;
				}
			}



			break;


		case TYPE_TEXTURE:
			for (int j = 0; j < TextureNum; j++)
			{
				if (strcmp(Textures[j].ID, Connections[i].IdChild) == 0)
				{
					childIdx = j;
					break;
				}
			}
			for (int j = 0; j < MaterialNum; j++)
			{
				if (strcmp(Materials[j].ID, Connections[i].IdFather) == 0)
				{
					fatherIdx = j;
					break;
				}
			}

			if (childIdx != -1 && fatherIdx != -1)
			{
				strcpy(Materials[fatherIdx].textureName, Textures[childIdx].FileName);
				Materials[fatherIdx].TexSampling = 1;	//このマテリアルがテクスチャを使います（フラグ）
			}


			break;

		}

	}

	//fbx内にUVがない場合		En el caso de que no haya UVs dentro del fbx
	if (Geometrys->TexCoord == NULL)
	{
		for (int j = 0; j < GeometryNum; j++)
		{
			Geometrys[j].TexCoord = new XMFLOAT2[Geometrys[j].FBXIndexNum];
			Geometrys[j].Diffuse = new XMFLOAT4[Geometrys[j].FBXIndexNum];

			for (int i = 0; i < Geometrys[j].FBXIndexNum; i++)
			{
				Geometrys[j].TexCoord[i] = XMFLOAT2(0.0f, 0.0f);
				Geometrys[j].Diffuse[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}

		}

	}


	//モデルのメッシュ情報
	modelData->MeshNum = GeometryNum;
	modelData->Mesh = new MESH_DATA[modelData->MeshNum];

	int DX11ModelIdx = 0;

	for (int j = 0; j < ModelNum; j++)
	{

		//頂点&インデックスの初期化　//Vertex and Index Array
		modelData->Mesh[DX11ModelIdx].VertexNum = 0;
		modelData->Mesh[DX11ModelIdx].IndexNum = 0;

		for (int g = 0; g < GeometryNum; g++)
		{
			if (j == Geometrys[g].ModelAttachedIdx)
			{
				modelData->Mesh[DX11ModelIdx].VertexNum += Geometrys[g].FBXIndexNum;
				modelData->Mesh[DX11ModelIdx].IndexNum  += Geometrys[g].RealIndexNum;
			}
		}
		if (modelData->Mesh[DX11ModelIdx].VertexNum == 0) continue;	//Empty model
		modelData->Mesh[DX11ModelIdx].VertexArray = new VERTEX_3D[modelData->Mesh[DX11ModelIdx].VertexNum];
		modelData->Mesh[DX11ModelIdx].IndexArray = new unsigned short[modelData->Mesh[DX11ModelIdx].IndexNum];


		//サブセットの初期化　//Subset Array
		modelData->Mesh[DX11ModelIdx].SubsetArray = new FBXSUBSET[Models[j].MatNumInModel];
		modelData->Mesh[DX11ModelIdx].SubsetNum = (unsigned short)Models[j].MatNumInModel;

		for (int i = 0; i < modelData->Mesh[DX11ModelIdx].SubsetNum; i++)
		{
			ZeroMemory(&modelData->Mesh[DX11ModelIdx].SubsetArray[i].Material.TextureName, sizeof(char) * 256);
			
			for (int m = 0; m < MaterialNum; m++)
			{
				if (strcmp(Models[j].LinkedMatID[i], Materials[m].ID) == 0)
				{
					modelData->Mesh[DX11ModelIdx].SubsetArray[i].Material.Material.Ambient			= Materials[m].AmbientColor;
					modelData->Mesh[DX11ModelIdx].SubsetArray[i].Material.Material.Diffuse			= Materials[m].DiffuseColor;
					modelData->Mesh[DX11ModelIdx].SubsetArray[i].Material.Material.Specular			= Materials[m].SpecularColor;
					modelData->Mesh[DX11ModelIdx].SubsetArray[i].Material.Material.Emission			= Materials[m].EmissiveColor;
					modelData->Mesh[DX11ModelIdx].SubsetArray[i].Material.Material.Shininess		= Materials[m].Shininess;
					modelData->Mesh[DX11ModelIdx].SubsetArray[i].Material.Material.noTexSampling	= Materials[m].TexSampling ? 0 : 1;

					if (Materials[m].textureName != NULL && strcmp(Materials[m].textureName, "") != 0)
					{
						strcpy(modelData->Mesh[DX11ModelIdx].SubsetArray[i].Material.TextureName, Materials[m].textureName);
					}
					break;
				}
			}

		}





		//メッシュデータをマテリアル（サブセット）によって整理します

		unsigned short indexCnt = 0;
		unsigned short SubsetIdx = 0;

		unsigned short FBXvertexCnt = 0;

		for (SubsetIdx = 0; SubsetIdx < Models[j].MatNumInModel; SubsetIdx++)		//buscar los vertices con los materiales de este modelo
		{

			if (indexCnt != 0)
			{
				modelData->Mesh[DX11ModelIdx].SubsetArray[SubsetIdx - 1].IndexNum = indexCnt - modelData->Mesh[DX11ModelIdx].SubsetArray[SubsetIdx - 1].StartIndex;
			}

			modelData->Mesh[DX11ModelIdx].SubsetArray[SubsetIdx].StartIndex = indexCnt;



			for (int MatNum = 0; MatNum < MaterialNum; MatNum++)
			{
				if (strcmp(Materials[MatNum].ID, Models[j].LinkedMatID[SubsetIdx]) != 0) continue;
				
				for (int h = 0; h < GeometryNum; h++)
				{
					if (strcmp(Models[j].ID, Geometrys[h].FatherID) != 0) continue;
					
					for (int vetMat = 0; vetMat < Geometrys[h].FBXIndexNum; vetMat++)
					{
						if (Geometrys[h].MaterialPerVertex[vetMat] == SubsetIdx) //si el material adjudicado a ese vertice es igual al material que se esta mirando en el padre
						{
							modelData->Mesh[DX11ModelIdx].VertexArray[FBXvertexCnt].Position	= Geometrys[h].Position[vetMat];
											  
							modelData->Mesh[DX11ModelIdx].VertexArray[FBXvertexCnt].Normal		= Geometrys[h].Normal[vetMat];
											  
							modelData->Mesh[DX11ModelIdx].VertexArray[FBXvertexCnt].TexCoord	= Geometrys[h].TexCoord[vetMat];
											  
							modelData->Mesh[DX11ModelIdx].VertexArray[FBXvertexCnt].Diffuse	= Geometrys[h].Diffuse[vetMat];
											  
							modelData->Mesh[DX11ModelIdx].IndexArray[indexCnt] = FBXvertexCnt;

							indexCnt++;

							FBXvertexCnt++;

						}
					}
					
				}
				
			}


		}

		if (SubsetIdx != 0)	//el ultimo index num
		{
			modelData->Mesh[DX11ModelIdx].SubsetArray[SubsetIdx - 1].IndexNum = indexCnt - modelData->Mesh[DX11ModelIdx].SubsetArray[SubsetIdx - 1].StartIndex;
		}


		DX11ModelIdx++;
	}

	//メモリー解放
	delete[] Geometrys;
	delete[] Materials;
	delete[] Models;
	delete[] Textures;
	delete[] Connections;

}



// モデルの全マテリアルのディフューズを取得する。Max16個分にしてある
void GetModelDiffuse(DX11_BUFFERS* Model, XMFLOAT4* diffuse)
{
	int max = (Model->SubsetNum < FBXMODEL_MAX_MATERIAL) ? Model->SubsetNum : FBXMODEL_MAX_MATERIAL;

	for (unsigned short i = 0; i < max; i++)
	{
		// ディフューズ設定
		diffuse[i] = Model->SubsetArray[i].Material.Material.Diffuse;
	}
}


// モデルの指定マテリアルのディフューズをセットする。
void SetModelDiffuse(DX11_BUFFERS* Model, int mno, XMFLOAT4 diffuse)
{
	// ディフューズ設定
	Model->SubsetArray[mno].Material.Material.Diffuse = diffuse;
}

