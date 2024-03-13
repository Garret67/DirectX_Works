//=============================================================================
//
// ファイル処理 [file.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================

/******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "fileSaveLoad.h"


/*******************************************************************************
//	セーブデータをバイナリファイルとして作成し、ファイルへ出力する
*******************************************************************************/
void SaveModel(MODEL_DATA modelData, FBX_ANIMATOR Animator, int activeAnimNum)
{

	// SAVEDATA構造体ごと全部をファイルに出力する
	FILE *fp;

	printf("\nセーブ開始・・・");
	fp = fopen("AAA_New_ModelDataSaved.bin", "wb");			// ファイルをバイナリ書き込みモードでOpenする

	if (fp != NULL)								// ファイルがあれば書き込み、無ければ無視
	{	
		//Model data
		fwrite(&modelData.ModelName,	sizeof(char),		  256,	fp);
		fwrite(&modelData.MeshNum,		sizeof(unsigned short),	1,	fp);

		for (int i = 0; i < modelData.MeshNum; i++)
		{
			//fwrite(&modelData.modelParts[i].ModelPartName,	sizeof(char), 256, fp);

			fwrite(&modelData.Mesh[i].VertexNum,	sizeof(unsigned short), 1, fp);
			for (int j = 0; j < modelData.Mesh[i].VertexNum; j++)
			{
				fwrite(&modelData.Mesh[i].VertexArray[j], sizeof(VERTEX_3D), 1, fp);
			}

			fwrite(&modelData.Mesh[i].IndexNum,		sizeof(unsigned short), 1, fp);
			for (int j = 0; j < modelData.Mesh[i].IndexNum; j++)
			{
				fwrite(&modelData.Mesh[i].IndexArray[j], sizeof(unsigned short), 1, fp);
			}

			fwrite(&modelData.Mesh[i].SubsetNum,	sizeof(unsigned short), 1, fp);
			for (int j = 0; j < modelData.Mesh[i].SubsetNum; j++)
			{
				fwrite(&modelData.Mesh[i].SubsetArray[j], sizeof(FBXSUBSET), 1, fp);
			}

		}


		fwrite(&modelData.TransformNum,	1, sizeof(unsigned short), fp);

		for (int i = 0; i < modelData.TransformNum; i++)
		{
			fwrite(&modelData.Transform[i].ObjectName, sizeof(char),  50, fp);
			fwrite(&modelData.Transform[i].FatherIdx,	sizeof(int),	1, fp);

			//fwrite(&modelData.modelPartsData[i].LclPosition,	sizeof(XMFLOAT3),	1, fp);
			//fwrite(&modelData.modelPartsData[i].LclRotation,	sizeof(XMFLOAT3),	1, fp);
			//fwrite(&modelData.modelPartsData[i].LclScaling,		sizeof(XMFLOAT3),	1, fp);
		}


		//Animator
		fwrite(&activeAnimNum, 1, sizeof(int), fp);		//Animator.animationNum

		for (int i = 0; i < activeAnimNum; i++)			//Animator.animationNum
		{
			fwrite(&Animator.animation[i].keyFrameNum,				sizeof(int),	1, fp);
			fwrite(&Animator.animation[i].AnimTimeCnt,				sizeof(float),	1, fp);
			fwrite(&Animator.animation[i].AnimTransitionFrames,		sizeof(float),	1, fp);
			fwrite(&Animator.animation[i].loop,						sizeof(BOOL),	1, fp);
			fwrite(&Animator.animation[i].nextAnimation,			sizeof(int),	1, fp);
			fwrite(&Animator.animation[i].speed,					sizeof(float),	1, fp);
			fwrite(&Animator.animation[i].animationName,			sizeof(char),	256, fp);


			for (int j = 0; j < Animator.animation[i].keyFrameNum; j++)
			{
				fwrite(&Animator.animation[i].keyFrame[j].modelosNum,		sizeof(int),	1, fp);
				fwrite(&Animator.animation[i].keyFrame[j].AnimFramesMax,	sizeof(float),	1, fp);

				for (int z = 0; z < Animator.animation[i].keyFrame[j].modelosNum; z++)
				{
					fwrite(&Animator.animation[i].keyFrame[j].Modelo[z].LclPosition,	sizeof(XMFLOAT3), 1, fp);
					fwrite(&Animator.animation[i].keyFrame[j].Modelo[z].LclRotation,	sizeof(XMFLOAT4), 1, fp);
					fwrite(&Animator.animation[i].keyFrame[j].Modelo[z].LclScaling,		sizeof(XMFLOAT3), 1, fp);
				}
			}
		}

		fwrite(&Animator.curAnim,				sizeof(int),	1, 	fp);
		fwrite(&Animator.nextAnim,				sizeof(int),	1, 	fp);
		fwrite(&Animator.transitionFramesCnt,	sizeof(float),	1, 	fp);
		
		fwrite(&Animator.hiddenObj,				sizeof(int),	HIDDEN_OBJ_NUM, fp);


		fclose(fp);								// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}

}


/*******************************************************************************
//　セーブデータをファイルから読み込む
*******************************************************************************/
void LoadModel(char* FileName, MODEL_DATA* modelData, FBX_ANIMATOR* Animator)
{

	// ファイルからセーブデータを読み込む
	FILE* fp;

	printf("\nロード開始・・・");
	fp = fopen(FileName, "rb");	// ファイルをバイナリ読み込みモードでOpenする

	if (fp != NULL)						// ファイルがあれば書き込み、無ければ無視
	{
		//Model data
		fread(&modelData->ModelName,	sizeof(char),		  256,	fp);
		fread(&modelData->MeshNum,		sizeof(unsigned short),	1,	fp);
		if (modelData->MeshNum <= 0 || modelData->MeshNum > 1000) return;
		modelData->Mesh = new MESH_DATA[modelData->MeshNum];

		for (int i = 0; i < modelData->MeshNum; i++)
		{
			//fread(&modelData->modelParts[i].ModelPartName,	sizeof(char), 256, fp);

			fread(&modelData->Mesh[i].VertexNum,	sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].VertexArray =		new VERTEX_3D[modelData->Mesh[i].VertexNum];
			for (int j = 0; j < modelData->Mesh[i].VertexNum; j++)
			{
				fread(&modelData->Mesh[i].VertexArray[j], sizeof(VERTEX_3D), 1, fp);
			}
			

			fread(&modelData->Mesh[i].IndexNum,		sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].IndexArray =			new unsigned short[modelData->Mesh[i].IndexNum];
			for (int j = 0; j < modelData->Mesh[i].IndexNum; j++)
			{
				fread(&modelData->Mesh[i].IndexArray[j], sizeof(unsigned short), 1, fp);
			}

			fread(&modelData->Mesh[i].SubsetNum,	sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].SubsetArray =		new FBXSUBSET[modelData->Mesh[i].SubsetNum];
			for (int j = 0; j < modelData->Mesh[i].SubsetNum; j++)
			{
				fread(&modelData->Mesh[i].SubsetArray[j], sizeof(FBXSUBSET), 1, fp);
			}

		}


		fread(&modelData->TransformNum,	1, sizeof(unsigned short), fp);
		modelData->Transform = new TRANSFORM_DATA[modelData->TransformNum];

		for (int i = 0; i < modelData->TransformNum; i++)
		{
			fread(&modelData->Transform[i].ObjectName, sizeof(char),  50, fp);
			fread(&modelData->Transform[i].FatherIdx,	sizeof(int),	1, fp);

			modelData->Transform[i].LclPosition	= XMFLOAT3(0.0f, 0.0f, 0.0f);
			modelData->Transform[i].LclRotation	= XMFLOAT3(0.0f, 0.0f, 0.0f);
			modelData->Transform[i].LclScaling	= XMFLOAT3(1.0f, 1.0f, 1.0f);
		}




		//Animator
		fread(&Animator->animationNum, 1, sizeof(int), fp);
		Animator->animation = new FBX_ANIMATION[Animator->animationNum];

		for (int i = 0; i < Animator->animationNum; i++)
		{
			fread(&Animator->animation[i].keyFrameNum,			sizeof(int),	1, fp);
			fread(&Animator->animation[i].AnimTimeCnt,			sizeof(float),	1, fp);
			fread(&Animator->animation[i].AnimTransitionFrames,	sizeof(float),	1, fp);
			fread(&Animator->animation[i].loop,					sizeof(BOOL),	1, fp);
			fread(&Animator->animation[i].nextAnimation,		sizeof(int),	1, fp);
			fread(&Animator->animation[i].speed,				sizeof(float),	1, fp);
			fread(&Animator->animation[i].animationName,		sizeof(char), 256, fp);

			Animator->animation[i].keyFrame = new FBX_ANIM_KEYFRAME[Animator->animation[i].keyFrameNum];

			for (int j = 0; j < Animator->animation[i].keyFrameNum; j++)
			{
				fread(&Animator->animation[i].keyFrame[j].modelosNum,		sizeof(int),	1, fp);
				fread(&Animator->animation[i].keyFrame[j].AnimFramesMax,	sizeof(float),	1, fp);

				Animator->animation[i].keyFrame[j].Modelo = new ANIM_MODEL[Animator->animation[i].keyFrame[j].modelosNum];

				for (int z = 0; z < Animator->animation[i].keyFrame[j].modelosNum; z++)
				{
					fread(&Animator->animation[i].keyFrame[j].Modelo[z].LclPosition,	sizeof(XMFLOAT3), 1, fp);
					fread(&Animator->animation[i].keyFrame[j].Modelo[z].LclRotation,	sizeof(XMFLOAT4), 1, fp);
					fread(&Animator->animation[i].keyFrame[j].Modelo[z].LclScaling,		sizeof(XMFLOAT3), 1, fp);
				}
			}
		}

		fread(&Animator->curAnim,				sizeof(int),	1, 	fp);
		fread(&Animator->nextAnim,				sizeof(int),	1, 	fp);
		fread(&Animator->transitionFramesCnt,	sizeof(float),	1, 	fp);
		fread(&Animator->hiddenObj,				sizeof(int), HIDDEN_OBJ_NUM, fp);

		fclose(fp);								// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}

}

