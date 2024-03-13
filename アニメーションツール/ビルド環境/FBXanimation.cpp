//=============================================================================
//
// アニメーションの処理 [FBXanimation.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include "FBXmodel.h"
#include "FBXanimation.h"
#include "camera.h"
#include "fileSaveLoad.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VALUE_MOVE_MODEL	(0.50f)					// 移動速度
#define	RATE_MOVE_MODEL		(0.20f)					// 移動慣性係数
#define	VALUE_ROTATE_MODEL	(XM_PI * 0.05f)			// 回転速度
#define	RATE_ROTATE_MODEL	(0.20f)					// 回転慣性係数
#define	SCALE_MODEL			(10.0f)					// 回転慣性係数

#define FBX_TIME_UNIT		(46186158000)			// FBXの時間単位（FbxTime）は1秒の1/46186158000です。	the time unit in FBX (FbxTime) is 1/46186158000 of one second


//*****************************************************************************
// 構造体定義
//*****************************************************************************

//キーフレームの値
struct FBX_ANIM_CURVE
{
	float*	KeyTime;				//時間（秒）　　time in seconds
	int		KeyTimeNum;				//時間の値の数　number of keys

	float*	KeyValueFloat;			//キーの値　　　key value
	int		KeyValueFloatNum;		//キーの値の数　number of keys

	char	ID[15];					//ID
	int		ObjectType;				//種類
	char	IDmodelConected[15];	//親モデル
	int		KeyValueType;			//値

};

// Model 構造体 
struct FBX_ANIM_MODEL
{
	XMFLOAT3		LclPosition;	//座標
	XMFLOAT3		LclRotation;	//回転
	XMFLOAT3		LclScaling;		//スケール
	XMFLOAT3		PivotPos;		//ピボット座標
	
	char			ID[15];			//ID
	int				ObjectType;		//種類
};

//キーフレームの接続ノード
struct FBX_ANIM_CURVE_NODE
{
	char			ID[15];					//ID
	char			IDmodelConected[15];	//接続モデル
	int				ObjectType;				//種類
};

//キーフレーム
struct KEYTIME_POS_ROT_SCL
{
	XMFLOAT3	Translation;	//座標
	XMFLOAT3	Rotation;		//回転
	XMFLOAT3	Scaling;		//スケール
	
	float		keyTime;		//時間
};

//キーフレーム値の種類
enum ANIM_CURVE_NODE_TYPE
{
	TYPE_T_X = 10, 	//Transform X...
	TYPE_T_Y,
	TYPE_T_Z,

	TYPE_R_X,		//Rotation X...
	TYPE_R_Y,
	TYPE_R_Z,

	TYPE_S_X,		//Scale X...
	TYPE_S_Y,
	TYPE_S_Z,

	TYPE_TRANSFORM,	//Transform component
	TYPE_ROTATION,	//Rotation  component
	TYPE_SCALE,		//Scale     component

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

void BubbleSort(float arr[], int n);


//=============================================================================
// 終了処理
//=============================================================================
void UnloadFBXanimation(FBX_ANIMATOR* Animator)
{
	//ANIMATOR
	{
		if (Animator->animationNum > 0 && Animator->animationNum <= 20)
		{
			for (int i = 0; i < Animator->animationNum; i++)
			{
				if (Animator->animation[i].keyFrameNum > 0 && Animator->animation[i].keyFrameNum < 500 && Animator->animation[i].keyFrame)
				{
					for (int j = 0; j < Animator->animation[i].keyFrameNum; j++)
					{
						if (Animator->animation[i].keyFrame[j].Modelo) delete[] Animator->animation[i].keyFrame[j].Modelo;
					}

					delete[] Animator->animation[i].keyFrame;
				}
			}

			if (Animator->animation) delete[] Animator->animation;
		}
	}
}



//=============================================================================
// アニメーションを読込
//=============================================================================
void LoadAnimationFromFBX(char* FileName, FBX_ANIMATION* Animation, unsigned short ModelNum)
{
	int	AnimCurveNum		= 0;		//AnimCurves     の数
	int	AnimCurveNODENum	= 0;		//AnimCurveNODEs の数
	int	AnimModelNum		= 0;		//AnimModels     の数

	int	AnimConnectionNum	= 0;		//AnimConnectionsの数



	FBX_ANIM_CURVE*			AnimCurves	= NULL;		//アニメーションの時間と値
	FBX_ANIM_CURVE_NODE*	AnimCurveNODEs = NULL;	//AnimCurvesとModelsの接続
	FBX_ANIM_MODEL*			AnimModels	= NULL;		//AnimCurvesの値はここにセーブされる

	FBX_CONNECTIONS*		AnimConnections = NULL; //全部の接続




	
	char str[256];		//ファイルを読むための変数		//varibles para leer el documento


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

				//アニメーションの時間と値　変数初期化
				if (strcmp(str, "\"AnimationCurve\"") == 0)
				{
					fscanf(file, "%*s%*s%s", str);
					AnimCurveNum = atoi(str);

					AnimCurves = new FBX_ANIM_CURVE[AnimCurveNum];

					for (int i = 0; i < AnimCurveNum; i++)
					{
						AnimCurves[i].ObjectType = OBJECT_TYPE::TYPE_ANIM_CURVE;
					}
				}
				//接続関係を初期化
				else if (strcmp(str, "\"AnimationCurveNode\"") == 0)
				{
					fscanf(file, "%*s%*s%s", str);
					AnimCurveNODENum = atoi(str);

					AnimCurveNODEs = new FBX_ANIM_CURVE_NODE[AnimCurveNODENum];

					//初期化
					for (int i = 0; i < AnimCurveNODENum; i++)
					{
						AnimCurveNODEs[i].ObjectType = OBJECT_TYPE::TYPE_MODEL;
					}

				}
				//モデル
				else if (strcmp(str, "\"Model\"") == 0)
				{
					fscanf(file, "%*s%*s%s", str);
					AnimModelNum = atoi(str);

					AnimModels = new FBX_ANIM_MODEL[AnimModelNum];

					//初期化
					for (int i = 0; i < AnimModelNum; i++)
					{
						AnimModels[i].ObjectType = OBJECT_TYPE::TYPE_MODEL;
					}

				}

			}
		}


		//オブジェクトのプロパティを検索する		//Busqueda de las propiedades del objeto 
		if (strcmp(str, "Objects:") == 0)
		{
			int curAnimCurve		= -1; 
			int curAnimCurveNODE	= -1;
			int curAnimModel		= -1;


			//FBXの接続の欄まで検索する
			while (strcmp(str, "Connections:") != 0)  //Poner una cosa como esta → fscanf(file, "%*c%*c%d", &coordNum);
			{
				//fscanf(file, "%s", str);
				fscanf(file, "%255s", str); //limita la lectura a 255 caracteres

				//AnimationCurveの情報（キーフレームの情報）
				if (strcmp(str, "AnimationCurve:") == 0)//Se pone o no?
				{
					curAnimCurve++;

					fscanf(file, "%13s", str); //consigue el id con la coma al final

					strcpy(AnimCurves[curAnimCurve].ID, str);
				}
				//キーフレームの時間を取得
				if (strcmp(str, "KeyTime:") == 0)
				{
					
					fscanf(file, "%*c%*c%d", &AnimCurves[curAnimCurve].KeyTimeNum);//salta dos caracteres y lee el siguiente int

					AnimCurves[curAnimCurve].KeyTime = new float[AnimCurves[curAnimCurve].KeyTimeNum];


					char curChar;				// ファイルを調べて、各文字を頂点に追加する文字列。　 //character que va a ir recorriendo el file y añadiendo cada letra al string de los vertices
					char appendstr[2];			// vertexPosStrとcurCharの間のstrcatを作成する文字列。//string para poder hacer el strcat entre vertexPosStr y curChar
					int Idx = 0;				// 次の座標が格納されるインデックスを保持する配列　　 //array que guarda el indice donde se guardara la proxima coordenada

					long long InitialFBXkeyTime;

					str[0] = '\0';
					appendstr[1] = '\0';

					fscanf(file, "%*s%*s%c", &curChar);		//se coge la cadena en la que estan las coordenadas de los vertices

					do
					{
						curChar = fgetc(file);	// igual que → //fscanf(file, "%c", curChar);

						switch (curChar)
						{
						case ',':
						case '}':

							if (Idx == 0)
							{
								InitialFBXkeyTime = strtoll(str, NULL, 10);
							}
							long long FBXkeyTime;

							FBXkeyTime = strtoll(str, NULL, 10);

							FBXkeyTime -= InitialFBXkeyTime;

							float keyTime;
							keyTime = (float)FBXkeyTime / FBX_TIME_UNIT;

							AnimCurves[curAnimCurve].KeyTime[Idx] = keyTime;



							Idx++;	//Suma uno a posArrayIdx en caso de que posArrayaxis sea 0 (falso), lo que significa que ha vuelto al eje x

							str[0] = '\0'; //vaciar cadena
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
				//キーフレームの値を取得
				if (strcmp(str, "KeyValueFloat:") == 0)
				{

					fscanf(file, "%*c%*c%d", &AnimCurves[curAnimCurve].KeyValueFloatNum);//salta dos caracteres y lee el siguiente int

					AnimCurves[curAnimCurve].KeyValueFloat = new float[AnimCurves[curAnimCurve].KeyValueFloatNum];


					char curChar;				//character que va a ir recorriendo el file y añadiendo cada letra al string de los vertices
					char appendstr[2];			//string para poder hacer el strcat entre vertexPosStr y curChar
					int Idx = 0;				//array que guarda el indice donde se guardara la proxima coordenada

					str[0] = '\0';
					appendstr[1] = '\0';

					fscanf(file, "%*s%*s%c", &curChar);		//se coge la cadena en la que estan las coordenadas de los vertices

					do
					{
						curChar = fgetc(file);	// igual que → //fscanf(file, "%c", curChar);

						switch (curChar)
						{
						case ',':
						case '}':


							AnimCurves[curAnimCurve].KeyValueFloat[Idx] = strtof(str, NULL);


							Idx++;	//Suma uno a posArrayIdx en caso de que posArrayaxis sea 0 (falso), lo que significa que ha vuelto al eje x

							str[0] = '\0'; //vaciar cadena
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
				//キーフレームの接続ノード
				else if (strcmp(str, "AnimationCurveNode:") == 0)
				{
					curAnimCurveNODE++;

					fscanf(file, "%13s", str); //consigue el id con la coma al final

					strcpy(AnimCurveNODEs[curAnimCurveNODE].ID, str);
				}
				//キーフレームの接続モデル
				else if (strcmp(str, "Model:") == 0)
				{
					curAnimModel++;

					fscanf(file, "%13s", str); //consigue el id con la coma al final

					strcpy(AnimModels[curAnimModel].ID, str);
				}




			}
		}
		//接続欄
		if (strcmp(str, "Connections:") == 0)
		{
			long posicionInicial = ftell(file);		//Connectionsが始まる所 
			int curConnections = -1;					//Geometry actual


			//Connectionsの数を数える
			while (strcmp(str, "}") != 0)
			{
				fscanf(file, "%255s", str); //limita la lectura a 255 caracteres

				if (strcmp(str, "C:") == 0)
				{
					AnimConnectionNum++;
				}

			}

			AnimConnections = new FBX_CONNECTIONS[AnimConnectionNum];

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
						AnimConnections[curConnections].ObjectTypeChild = OBJECT_TYPE::TYPE_MODEL;
					}
					else if (strcmp(objType, "AnimCurveNode") == 0)
					{
						AnimConnections[curConnections].ObjectTypeChild = OBJECT_TYPE::TYPE_ANIM_CURVE_NODE;
					}
					else if (strcmp(objType, "AnimCurve") == 0)
					{
						AnimConnections[curConnections].ObjectTypeChild = OBJECT_TYPE::TYPE_ANIM_CURVE;
					}


					fscanf(file, "%255s", str); //limita la lectura a 255 caracteres

					objType = str;
					objTypeFinal = 0;

					objTypeFinal = strchr(objType, ':');	//Returns a pointer to the first occurrence of character in the C string str.

					if (objTypeFinal == NULL)	//Hay veces que los nombres no están unidos, por lo que al buscar la proxima str coge algo que no queremos y al buscar ':' sale NULL. Esto es una contramedida para eso
					{
						while (objTypeFinal == NULL)
						{
							fscanf(file, "%255s", objType); //limita la lectura a 255 caracteres
							objTypeFinal = strchr(objType, ':');	//Returns a pointer to the first occurrence of character in the C string str.
						}
					}

					*objTypeFinal = '\0';
					//親の種類を設定
					if (strcmp(objType, "Model") == 0)
					{
						AnimConnections[curConnections].ObjectTypeFather = OBJECT_TYPE::TYPE_MODEL;
					}
					else if (strcmp(objType, "AnimCurveNode") == 0)
					{
						//fscanf(file, "%*c%*c%c", &objType);//salta dos caracteres y lee el siguiente int

						switch (str[15])
						{
						case 'T':
							AnimConnections[curConnections].ObjectTypeFather = TYPE_TRANSFORM;
							break;

						case 'R':
							AnimConnections[curConnections].ObjectTypeFather = TYPE_ROTATION;
							break;

						case 'S':
							AnimConnections[curConnections].ObjectTypeFather = TYPE_SCALE;
							break;
						}
					}

				}
				//子供と親のIDを取得する
				else if (strcmp(str, "C:") == 0)
				{
					if (curConnections == -1) curConnections = 0;	//hay veces que el fseek no devuelve justo a la primera linea y hace que no se cuente la primera vez el curConnections
					fscanf(file, "%*c%*c%*c%*c%*c%*c%13s", str);	//salta dos caracteres y lee el siguiente int
					strcpy(AnimConnections[curConnections].IdChild, str);

					fscanf(file, "%*c%13s", str);
					strcpy(AnimConnections[curConnections].IdFather, str);

				}
				//種類を設定（X軸）
				else if (strcmp(str, "\"d|X\"") == 0)
				{
					switch (AnimConnections[curConnections].ObjectTypeFather)	//[curConnections - 1] porque lo estoy buscando en el proximo loop para que no afectase a otros
					{
					case TYPE_TRANSFORM:
						AnimConnections[curConnections].ObjectTypeFather = TYPE_T_X;
						break;

					case TYPE_ROTATION:
						AnimConnections[curConnections].ObjectTypeFather = TYPE_R_X;
						break;

					case TYPE_SCALE:
						AnimConnections[curConnections].ObjectTypeFather = TYPE_S_X;
						break;
					}
				}
				//種類を設定（Y軸）
				else if (strcmp(str, "\"d|Y\"") == 0)
				{
					switch (AnimConnections[curConnections].ObjectTypeFather)	//[curConnections - 1] porque lo estoy buscando en el proximo loop para que no afectase a otros
					{
					case TYPE_TRANSFORM:
						AnimConnections[curConnections].ObjectTypeFather = TYPE_T_Y;
						break;

					case TYPE_ROTATION:
						AnimConnections[curConnections].ObjectTypeFather = TYPE_R_Y;
						break;

					case TYPE_SCALE:
						AnimConnections[curConnections].ObjectTypeFather = TYPE_S_Y;
						break;
					}
				}
				//種類を設定（Z軸）
				else if (strcmp(str, "\"d|Z\"") == 0)
				{
					switch (AnimConnections[curConnections].ObjectTypeFather)	//[curConnections - 1] porque lo estoy buscando en el proximo loop para que no afectase a otros
					{
					case TYPE_TRANSFORM:
						AnimConnections[curConnections].ObjectTypeFather = TYPE_T_Z;
						break;

					case TYPE_ROTATION:
						AnimConnections[curConnections].ObjectTypeFather = TYPE_R_Z;
						break;

					case TYPE_SCALE:
						AnimConnections[curConnections].ObjectTypeFather = TYPE_S_Z;
						break;
					}
				}



			}
		}

	}
	fclose(file);

	//アニメーションファイルがモデルファイルと異なります　→　無効
	if (AnimModelNum != ModelNum) return;
	

	//接続
	for (int i = 0; i < AnimConnectionNum; i++)
	{
		int childIdx = -1;
		int fatherIdx = -1;

		switch (AnimConnections[i].ObjectTypeChild)
		{

		case TYPE_ANIM_CURVE_NODE:
			for (int j = 0; j < AnimCurveNODENum; j++)
			{
				if (strcmp(AnimCurveNODEs[j].ID, AnimConnections[i].IdChild) == 0)
				{
					childIdx = j;
					break;
				}
			}
			for (int j = 0; j < AnimModelNum; j++)
			{
				if (strcmp(AnimModels[j].ID, AnimConnections[i].IdFather) == 0)
				{
					fatherIdx = j;
					break;
				}
			}


			if (childIdx != -1 && fatherIdx != -1)
			{
				strcpy(AnimCurveNODEs[childIdx].IDmodelConected, AnimModels[fatherIdx].ID);
				
			}

			break;


		case TYPE_ANIM_CURVE:
			if (AnimConnections[i].ObjectTypeFather < TYPE_T_X) break;

			for (int j = 0; j < AnimCurveNum; j++)
			{
				if (strcmp(AnimCurves[j].ID, AnimConnections[i].IdChild) == 0)
				{
					childIdx = j;
					break;
				}
			}
			for (int j = 0; j < AnimCurveNODENum; j++)
			{
				if (strcmp(AnimCurveNODEs[j].ID, AnimConnections[i].IdFather) == 0)
				{
					fatherIdx = j;
					break;
				}
			}


			if (childIdx != -1 && fatherIdx != -1)
			{
				strcpy(AnimCurves[childIdx].IDmodelConected, AnimCurveNODEs[fatherIdx].IDmodelConected);
				AnimCurves[childIdx].KeyValueType = AnimConnections[i].ObjectTypeFather;
			}

			break;



		}

	}

	//**************************
	// キーフレームの数を調べる		BUSQUEDA DE CUANTOS KEY FRAMES DIFERENTES HAY
	//**************************
	float GlobalKeyTimes[100];
	int   GlobalKeyTimesNum = 0;
	BOOL  KeyFind = FALSE;


	for (int i = 0; i < AnimCurveNum; i++)
	{
		for (int j = 0; j < AnimCurves[i].KeyTimeNum; j++)
		{

			KeyFind = FALSE;

			for (int k = 0; k < GlobalKeyTimesNum; k++)
			{
				if (GlobalKeyTimes[k] == AnimCurves[i].KeyTime[j])
				{
					KeyFind = TRUE;
					break;
				}
			}

			if (KeyFind == FALSE)
			{
				GlobalKeyTimes[GlobalKeyTimesNum] = AnimCurves[i].KeyTime[j];
				GlobalKeyTimesNum++;
			}

		}
	}
	//キーを順番に並べ替える
	BubbleSort(GlobalKeyTimes, GlobalKeyTimesNum);

	Animation->keyFrame = new FBX_ANIM_KEYFRAME[GlobalKeyTimesNum];
	Animation->keyFrameNum = GlobalKeyTimesNum;


	for (int i = 0; i < GlobalKeyTimesNum; i++)
	{
		//一時変数を初期化
		MODEL_DATA KTmodelData;

		KTmodelData.TransformNum = ModelNum;

		KTmodelData.Transform = new TRANSFORM_DATA[KTmodelData.TransformNum];
		
		for (int j = 0; j < KTmodelData.TransformNum; j++)
		{
			KTmodelData.Transform[j].LclPosition	= XMFLOAT3(0.0f, 0.0f, 0.0f);
			KTmodelData.Transform[j].LclRotation	= XMFLOAT3(0.0f, 0.0f, 0.0f);
			KTmodelData.Transform[j].LclScaling		= XMFLOAT3(1.0f, 1.0f, 1.0f);
		}


		//すべての値を一時変数に渡す
		for (int j = 0; j < AnimCurveNum; j++)
		{
			for (int k = 0; k < AnimCurves[j].KeyTimeNum; k++)
			{
				if (AnimCurves[j].KeyTime[k] == GlobalKeyTimes[i] ||		//Los tiempos son iguales 
					(AnimCurves[j].KeyTime[k] > GlobalKeyTimes[i] && k == 0) ||	//tiempos diferentes pero es la primera clave por lo que se queda con el valor de la primera clave
					(k == AnimCurves[j].KeyTimeNum - 1 && AnimCurves[j].KeyTime[k] < GlobalKeyTimes[i]))	//llega a la ultima clave pero sigue siendo mas pequeño el tiempo que el buscado (se queda con el valor de la ultima clave)
				{
					int modelIdx = -1;
					for (int m = 0; m < KTmodelData.TransformNum; m++)
					{
						if (strcmp(AnimCurves[j].IDmodelConected, AnimModels[m].ID) == 0)
						{
							modelIdx = m;
							break;
						}
					}

					if (modelIdx < 0) break;	//Los Visibility creo que son asi

					//種類によって
					switch (AnimCurves[j].KeyValueType)
					{
					case TYPE_T_X:
						KTmodelData.Transform[modelIdx].LclPosition.x = AnimCurves[j].KeyValueFloat[k];
						KTmodelData.Transform[modelIdx].LclPosition.x *= SCALE_MODEL;
							break;

					case TYPE_T_Y:
						KTmodelData.Transform[modelIdx].LclPosition.y = AnimCurves[j].KeyValueFloat[k];
						KTmodelData.Transform[modelIdx].LclPosition.y *= SCALE_MODEL;
						break;

					case TYPE_T_Z:
						KTmodelData.Transform[modelIdx].LclPosition.z = AnimCurves[j].KeyValueFloat[k];
						KTmodelData.Transform[modelIdx].LclPosition.z *= SCALE_MODEL;
						break;

					case TYPE_R_X:
						KTmodelData.Transform[modelIdx].LclRotation.x = AnimCurves[j].KeyValueFloat[k];
						KTmodelData.Transform[modelIdx].LclRotation.x = XMConvertToRadians(KTmodelData.Transform[modelIdx].LclRotation.x);
						break;

					case TYPE_R_Y:
						KTmodelData.Transform[modelIdx].LclRotation.y = AnimCurves[j].KeyValueFloat[k];
						if (AnimCurves[j].KeyValueFloat[k] == 110.7682f)
						{
							AnimCurves[j].KeyValueFloat[k] = AnimCurves[j].KeyValueFloat[k];
						}
						KTmodelData.Transform[modelIdx].LclRotation.y = XMConvertToRadians(KTmodelData.Transform[modelIdx].LclRotation.y);
						break;

					case TYPE_R_Z:
						KTmodelData.Transform[modelIdx].LclRotation.z = AnimCurves[j].KeyValueFloat[k];
						KTmodelData.Transform[modelIdx].LclRotation.z = XMConvertToRadians(KTmodelData.Transform[modelIdx].LclRotation.z);
						break;

					case TYPE_S_X:
						KTmodelData.Transform[modelIdx].LclScaling.x = AnimCurves[j].KeyValueFloat[k];
						break;

					case TYPE_S_Y:
						KTmodelData.Transform[modelIdx].LclScaling.y = AnimCurves[j].KeyValueFloat[k];
						break;

					case TYPE_S_Z:
						KTmodelData.Transform[modelIdx].LclScaling.z = AnimCurves[j].KeyValueFloat[k];
						break;



					}

					break;
				}
				else if(AnimCurves[j].KeyTime[k] > GlobalKeyTimes[i])
				{
					//INTERPOLATION
					double pendiente;
					if (AnimCurves[j].KeyTime[k] != AnimCurves[j].KeyTime[k - 1])
					{
						pendiente = (AnimCurves[j].KeyValueFloat[k] - AnimCurves[j].KeyValueFloat[k - 1]) / (AnimCurves[j].KeyTime[k] - AnimCurves[j].KeyTime[k - 1]);
					}
					else {
						pendiente = 0.0;  
					}

					double ordenadaAlOrigen = AnimCurves[j].KeyValueFloat[k - 1] - pendiente * AnimCurves[j].KeyTime[k - 1];
					double interpolation = pendiente * GlobalKeyTimes[i] + ordenadaAlOrigen;

					int modelIdx = 0;
					BOOL conected = FALSE;
					//どのモデルに繋がっているか探す
					for (int m = 0; m < KTmodelData.TransformNum; m++)
					{
						if (strcmp(AnimCurves[j].IDmodelConected, AnimModels[m].ID) == 0)
						{
							modelIdx = m;
							conected = TRUE;
							break;
						}
					}

					if (modelIdx < 0 || conected == FALSE) break;	//Visibilityノードであればスキップする　//Los Visibility creo que son asi


					switch (AnimCurves[j].KeyValueType)
					{
					case TYPE_T_X:
						KTmodelData.Transform[modelIdx].LclPosition.x = (float)interpolation;
						KTmodelData.Transform[modelIdx].LclPosition.x *= SCALE_MODEL;
						break;

					case TYPE_T_Y:
						KTmodelData.Transform[modelIdx].LclPosition.y = (float)interpolation;
						KTmodelData.Transform[modelIdx].LclPosition.y *= SCALE_MODEL;
						break;

					case TYPE_T_Z:
						KTmodelData.Transform[modelIdx].LclPosition.z = (float)interpolation;
						KTmodelData.Transform[modelIdx].LclPosition.z *= SCALE_MODEL;
						break;

					case TYPE_R_X:
						KTmodelData.Transform[modelIdx].LclRotation.x = (float)interpolation;
						KTmodelData.Transform[modelIdx].LclRotation.x = XMConvertToRadians(KTmodelData.Transform[modelIdx].LclRotation.x);

						break;

					case TYPE_R_Y:
						KTmodelData.Transform[modelIdx].LclRotation.y = (float)interpolation;
						KTmodelData.Transform[modelIdx].LclRotation.y = XMConvertToRadians(KTmodelData.Transform[modelIdx].LclRotation.y);

						break;

					case TYPE_R_Z:
						KTmodelData.Transform[modelIdx].LclRotation.z = (float)interpolation;
						KTmodelData.Transform[modelIdx].LclRotation.z = XMConvertToRadians(KTmodelData.Transform[modelIdx].LclRotation.z);

						break;

					case TYPE_S_X:
						KTmodelData.Transform[modelIdx].LclScaling.x = (float)interpolation;
						break;

					case TYPE_S_Y:
						KTmodelData.Transform[modelIdx].LclScaling.y = (float)interpolation;
						break;

					case TYPE_S_Z:
						KTmodelData.Transform[modelIdx].LclScaling.z = (float)interpolation;
						break;
					}


					break;
				}
			}
		}
			
		//***************************
		
		//QUATERNION 変換

		//for (int j = 0; j < KTmodelData.modelPartsDataNum; j++)
		//{
		//	XMVECTOR quaternionVector = XMQuaternionRotationRollPitchYaw(KTmodelData.modelPartsData[j].LclRotation.x, 
		//																 KTmodelData.modelPartsData[j].LclRotation.y, 
		//																 KTmodelData.modelPartsData[j].LclRotation.z);

		//	quaternionVector = XMQuaternionNormalize(quaternionVector);

		//	XMStoreFloat4(&KTmodelData.modelPartsData[j].LclRotation, quaternionVector);// Convertir el quaternion a XMFLOAT4
		//}




	//**************************
	// 各グローバルキーフレームに対して1つのfbx_animated_vertexを作成する。	CREAR UN FBX_ANIMATED_VERTEX POR CADA GLOBAL KEY FRAME
	//**************************

	//全部のアニメーション　モデルの初期化

		Animation->keyFrame[i].modelosNum = KTmodelData.TransformNum;
		Animation->keyFrame[i].Modelo = new ANIM_MODEL[KTmodelData.TransformNum];

		for (int j = 0; j < KTmodelData.TransformNum; j++)
		{
			Animation->keyFrame[i].Modelo[j].LclPosition	= KTmodelData.Transform[j].LclPosition;
			Animation->keyFrame[i].Modelo[j].LclRotation	= KTmodelData.Transform[j].LclRotation;
			Animation->keyFrame[i].Modelo[j].LclScaling		= KTmodelData.Transform[j].LclScaling;
		}

		if (i < GlobalKeyTimesNum - 1)
		{
			Animation->keyFrame[i].AnimFramesMax = (GlobalKeyTimes[i + 1] - GlobalKeyTimes[i]) * 60.0f;
		}
		else
		{
			Animation->keyFrame[i].AnimFramesMax = 0.0f;
		}

		if (KTmodelData.Transform)	delete[] KTmodelData.Transform;

	}

	Animation->AnimTimeCnt			= 0.0f;
	Animation->AnimTransitionFrames = 10.0f;
	Animation->nextAnimation		= 0;
	Animation->loop					= TRUE;
	Animation->speed				= 1.0f;


	for (int i = 0; i < AnimCurveNum; i++)
	{
		delete[] AnimCurves[i].KeyTime;
		delete[] AnimCurves[i].KeyValueFloat;
	}
	delete[] AnimCurves;
	delete[] AnimCurveNODEs;
	delete[] AnimModels;
	delete[] AnimConnections;

}


//アニメーションの再生を線形補間で行う
void AnimationInterpolation(MODEL_DATA* ResultModel, FBX_ANIMATOR& Animator, int& curAnim, int& nextAnim)
{
	//アニメーションがない場合、戻る
	if (curAnim < 0) return;
	if (Animator.animation[curAnim].AnimTimeCnt < 0|| Animator.animation[curAnim].AnimTimeCnt > 100) return;

	float& curAnimFramesCnt  = Animator.animation[curAnim].AnimTimeCnt;//アニメーションの現在のフレーム（時間）

	int CurKF = (int)curAnimFramesCnt;		//今のキーフレーム　//current Key frame

	float curAnimSpeed = Animator.animation[curAnim].speed;		//アニメーションの速度

	float  curAnimFramesMax = Animator.animation[curAnim].keyFrame[CurKF].AnimFramesMax;	//アニメーションの最終フレーム（時間）
	curAnimFramesMax /= curAnimSpeed;	//最終フレームをアニメーションの速度に合わせる

	//現在再生しているアニメーションを更新する
	for (int i = 0; i < Animator.animation[curAnim].keyFrame[CurKF].modelosNum; i++)
	{
		// 今のキーフレームと次のキーフレームをXMVECTORに変換する　// Convertir las representaciones iniciales y finales a XMVECTOR
		XMVECTOR nowPos  = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF].Modelo[i].LclPosition);
		XMVECTOR nowRot  = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF].Modelo[i].LclRotation);
		XMVECTOR nowScl  = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF].Modelo[i].LclScaling);

		XMVECTOR NextPos = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF + 1].Modelo[i].LclPosition);
		XMVECTOR NextRot = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF + 1].Modelo[i].LclRotation);
		XMVECTOR NextScl = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF + 1].Modelo[i].LclScaling);


		float nowTime = curAnimFramesCnt - CurKF;	// 時間部分である少数を取り出している

		//線形補間で次のフレームを計算する
		XMVECTOR resultPos = XMVectorLerp		(nowPos, NextPos, nowTime);
		XMVECTOR resultRot = XMVectorLerp		(nowRot, NextRot, nowTime);
		XMVECTOR resultScl = XMVectorLerp		(nowScl, NextScl, nowTime);

		//結果をセーブする
		XMStoreFloat3(&ResultModel->Transform[i].LclPosition, resultPos);
		XMStoreFloat3(&ResultModel->Transform[i].LclRotation, resultRot);
		XMStoreFloat3(&ResultModel->Transform[i].LclScaling, resultScl);


		{//非表示オブジェクト用
			for (int j = 0; j < HIDDEN_OBJ_NUM; j++)
			{
				if (Animator.hiddenObj[j] == i)
				{
					//非表示する部分のスケールを0にする（親子関係で、子供も見えなくなる）
					ResultModel->Transform[i].LclScaling = XMFLOAT3(0.0f, 0.0f, 0.0f);
					break;
				}
			}
		}

	}

	
	curAnimFramesCnt += 1.0f / curAnimFramesMax;	// 時間を進めている


	if ((int)curAnimFramesCnt >= Animator.animation[curAnim].keyFrameNum - 1)	// アニメーションが終わった？
	{

		//ループしない場合
		if (Animator.animation[curAnim].loop == FALSE)
		{
			if (curAnim != Animator.animation[curAnim].nextAnimation)
			{
				nextAnim = Animator.animation[curAnim].nextAnimation;		//次のアニメーションに変更する
			}
			curAnimFramesCnt -= 1.0f / curAnimFramesMax;
		}
		else
		{
			curAnimFramesCnt -= Animator.animation[curAnim].keyFrameNum - 1;		// ０番目にリセットしつつも小数部分を引き継いでいる
		}
	}
	

	//アニメーションブレンディングしていたら、次のアニメーションも更新する
	if (nextAnim >= 0)
	{
		XMVECTOR nextAnimPos;
		XMVECTOR nextAnimRos;
		XMVECTOR nextAnimScl;

		float& transitionFramesCnt = Animator.transitionFramesCnt;		//ブレンディングの現在のフレームカウンター（時間）
		float curtransitionFramesMax = Animator.animation[curAnim].AnimTransitionFrames;	//次のアニメーションに切り替えるまでのフレーム数


		float& nextAnimFramesCnt = Animator.animation[nextAnim].AnimTimeCnt;	//次のアニメーションの時間カウンター


		int NextAnimCurKF; //次のアニメーションの現在のキーフレーム	//current Key frame
		NextAnimCurKF = (int)nextAnimFramesCnt;

		float nextAnimSpeed = Animator.animation[nextAnim].speed;	//次のアニメーションの速度
		float nextAnimFramesMax = Animator.animation[nextAnim].keyFrame[NextAnimCurKF].AnimFramesMax;	//次のアニメーションの最終フレーム（時間）
		nextAnimFramesMax /= nextAnimSpeed;

		//次のアニメーションも更新する
		for (int i = 0; i < Animator.animation[nextAnim].keyFrame[NextAnimCurKF].modelosNum; i++)
		{
			// 今のキーフレームと次のキーフレームをXMVECTORに変換する	// Convertir las representaciones iniciales y finales a XMVECTOR
			XMVECTOR nowPos  = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF].Modelo[i].LclPosition);
			XMVECTOR nowRot  = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF].Modelo[i].LclRotation);
			XMVECTOR nowScl  = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF].Modelo[i].LclScaling);

			XMVECTOR NextPos = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF + 1].Modelo[i].LclPosition);
			XMVECTOR NextRot = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF + 1].Modelo[i].LclRotation);
			XMVECTOR NextScl = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF + 1].Modelo[i].LclScaling);


			float nowTime = nextAnimFramesCnt - NextAnimCurKF;	// 時間部分である少数を取り出している

			//線形補間で次のフレームを計算する
			nextAnimPos = XMVectorLerp		(nowPos, NextPos, nowTime);
			nextAnimRos = XMVectorLerp		(nowRot, NextRot, nowTime);
			nextAnimScl = XMVectorLerp		(nowScl, NextScl, nowTime);



			//**************************
			//TRANSITION INTERPOLATION
			// 	// アニメーションブレンディング
			//**************************

			// 今再生しているアニメーションの値をXMVECTORに変換する
			XMVECTOR nowAnimPos = XMLoadFloat3(&ResultModel->Transform[i].LclPosition);		// XMVECTORへ変換
			XMVECTOR nowAnimRot = XMLoadFloat3(&ResultModel->Transform[i].LclRotation);		// XMVECTORへ変換
			XMVECTOR nowAnimScl = XMLoadFloat3(&ResultModel->Transform[i].LclScaling);		// XMVECTORへ変換


			//両方のアニメーションの差を計算する
			XMVECTOR DiferencePos = (nextAnimPos) - nowAnimPos;	// XYZ移動量を計算している
			XMVECTOR DiferenceRot = (nextAnimRos) - nowAnimRot;	// XYZ回転量を計算している
			XMVECTOR DiferenceScl = (nextAnimScl) - nowAnimScl;	// XYZ拡大率を計算している

			float transitionTime = transitionFramesCnt / curtransitionFramesMax;	// 時間部分である少数を取り出している


			DiferencePos *= transitionTime;								// 現在の移動量を計算している
			DiferenceRot *= transitionTime;								// 現在の回転量を計算している
			DiferenceScl *= transitionTime;								// 現在の拡大率を計算している

			//ブレンディングの結果をセーブする
			XMStoreFloat3(&ResultModel->Transform[i].LclPosition,	nowAnimPos + DiferencePos);
			XMStoreFloat3(&ResultModel->Transform[i].LclRotation,		nowAnimRot + DiferenceRot);
			XMStoreFloat3(&ResultModel->Transform[i].LclScaling,		nowAnimScl + DiferenceScl);


			{//非表示オブジェクト用
				for (int j = 0; j < HIDDEN_OBJ_NUM; j++)
				{
					if (Animator.hiddenObj[j] == i)
					{
						//非表示する部分のスケールを0にする（親子関係で、子供も見えなくなる）
						ResultModel->Transform[i].LclScaling = XMFLOAT3(0.0f, 0.0f, 0.0f);
						break;
					}
				}
			}

		}

		// 次のアニメーションの時間経過処理をする
		if (Animator.animation[nextAnim].loop == TRUE) nextAnimFramesCnt += 1.0f / nextAnimFramesMax;	// 時間を進めている

		if ((int)nextAnimFramesCnt >= Animator.animation[nextAnim].keyFrameNum - 1)			// 登録テーブル最後まで移動したか？
		{
			nextAnimFramesCnt -= Animator.animation[nextAnim].keyFrameNum - 1;				// ０番目にリセットしつつも小数部分を引き継いでいる
		}



		transitionFramesCnt++;

		if (transitionFramesCnt >= curtransitionFramesMax)
		{
			curAnimFramesCnt = 0.0f;

			curAnim = nextAnim;
			nextAnim = -1;

			Animator.transitionFramesCnt = 0.0f;
		}

	}
}


//アニメーターの初期化
void InitAnimator(FBX_ANIMATOR* animator, int animationNum)
{
	animator->transitionFramesCnt = 0.0f;
	animator->animationNum = animationNum;

	animator->animation = new FBX_ANIMATION[animationNum];
	for (int i = 0; i < animationNum; i++)
	{
		strcpy(animator->animation[i].animationName, "");
		animator->animation[i].keyFrameNum = 0;
	}
	
	animator->curAnim   = -1;
	animator->nextAnim  = -1;

	for (int i = 0; i < HIDDEN_OBJ_NUM; i++)
	{
		animator->hiddenObj[i] = -1;
	}

}

//要素を順番に並べ替える
void BubbleSort(float arr[], int n) 
{
	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				// 要素の順序が正しくない場合は入れ替える
				float temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}