//=============================================================================
//
// モデルの処理 [modelBinaryFBX.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include "modelBinaryFBX.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

void LoadModelAnimData(char* FileName, MODEL_DATA* modelData, FBX_ANIMATOR* Animator);
void LoadModelData(char* FileName, MODEL_DATA* modelData);

void AnimationInterpolation(MODEL_DATA* ResultModel, FBX_ANIMATOR& Animator, int& curAnim, int& nextAnim);

//=============================================================================
// 初期化処理
// アニメーションがあるモデル
//=============================================================================
void InitModelBinaryFBX(char* FileName, DX11_MODEL* Model, MODEL_DATA* modelData, FBX_ANIMATOR* Animator)
{
	LoadModelAnimData(FileName, modelData, Animator);
	
	Model->BuffersNum = modelData->MeshNum;
	Model->Buffers = new DX11_BUFFERS[Model->BuffersNum];

	for (int j = 0; j < modelData->MeshNum; j++)
	{
		// 頂点バッファ生成
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * modelData->Mesh[j].VertexNum;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  // Permite la escritura desde la CPU

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = modelData->Mesh[j].VertexArray;

			GetDevice()->CreateBuffer(&bd, &sd, &Model->Buffers[j].VertexBuffer);
		}


		// インデックスバッファ生成
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned short) * modelData->Mesh[j].IndexNum;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = modelData->Mesh[j].IndexArray;

			GetDevice()->CreateBuffer(&bd, &sd, &Model->Buffers[j].IndexBuffer);
		}

		// サブセット設定
		{
			Model->Buffers[j].SubsetArray = new DX11_FBXSUBSET[modelData->Mesh[j].SubsetNum];
			Model->Buffers[j].SubsetNum = modelData->Mesh[j].SubsetNum;
			Model->Buffers[j].SubsetArray[0].Material.Texture = NULL;

			for (unsigned short i = 0; i < modelData->Mesh[j].SubsetNum; i++)
			{
				Model->Buffers[j].SubsetArray[i].StartIndex = modelData->Mesh[j].SubsetArray[i].StartIndex;
				Model->Buffers[j].SubsetArray[i].IndexNum = modelData->Mesh[j].SubsetArray[i].IndexNum;

				Model->Buffers[j].SubsetArray[i].Material.Material = modelData->Mesh[j].SubsetArray[i].Material.Material;

				Model->Buffers[j].SubsetArray[i].Material.Texture = NULL;
				D3DX11CreateShaderResourceViewFromFile(GetDevice(),
					modelData->Mesh[j].SubsetArray[i].Material.TextureName,
					NULL,
					NULL,
					&Model->Buffers[j].SubsetArray[i].Material.Texture,
					NULL);
			}
		}

	}



}

//=============================================================================
// 初期化処理
// アニメーションがないモデル
//=============================================================================
void InitModelBinaryFBX(char* FileName, DX11_MODEL* Model, MODEL_DATA* modelData)
{
	LoadModelData(FileName, modelData);

	Model->BuffersNum = modelData->MeshNum;
	Model->Buffers = new DX11_BUFFERS[Model->BuffersNum];

	for (int j = 0; j < modelData->MeshNum; j++)
	{
		// 頂点バッファ生成
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * modelData->Mesh[j].VertexNum;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  // Permite la escritura desde la CPU

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = modelData->Mesh[j].VertexArray;

			GetDevice()->CreateBuffer(&bd, &sd, &Model->Buffers[j].VertexBuffer);
		}


		// インデックスバッファ生成
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned short) * modelData->Mesh[j].IndexNum;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = modelData->Mesh[j].IndexArray;

			GetDevice()->CreateBuffer(&bd, &sd, &Model->Buffers[j].IndexBuffer);
		}

		// サブセット設定
		{
			Model->Buffers[j].SubsetArray = new DX11_FBXSUBSET[modelData->Mesh[j].SubsetNum];
			Model->Buffers[j].SubsetNum = modelData->Mesh[j].SubsetNum;
			Model->Buffers[j].SubsetArray[0].Material.Texture = NULL;

			for (unsigned short i = 0; i < modelData->Mesh[j].SubsetNum; i++)
			{
				Model->Buffers[j].SubsetArray[i].StartIndex = modelData->Mesh[j].SubsetArray[i].StartIndex;
				Model->Buffers[j].SubsetArray[i].IndexNum = modelData->Mesh[j].SubsetArray[i].IndexNum;

				Model->Buffers[j].SubsetArray[i].Material.Material = modelData->Mesh[j].SubsetArray[i].Material.Material;

				Model->Buffers[j].SubsetArray[i].Material.Texture = NULL;
				D3DX11CreateShaderResourceViewFromFile(GetDevice(),
					modelData->Mesh[j].SubsetArray[i].Material.TextureName,
					NULL,
					NULL,
					&Model->Buffers[j].SubsetArray[i].Material.Texture,
					NULL);
			}
		}

	}



}


//=============================================================================
// アニメーションがあるモデルの終了処理
//=============================================================================
void UninitModelBinaryFBX(DX11_MODEL* Model, MODEL_DATA* modelData, FBX_ANIMATOR* Animator)
{

	//MODEL
	{
		for (int i = 0; i < Model->BuffersNum; i++)
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

	}

	//MODEL DATA
	{
		for (int j = 0; j < modelData->MeshNum; j++)
		{
			delete[] modelData->Mesh[j].VertexArray;
			delete[] modelData->Mesh[j].IndexArray;
			delete[] modelData->Mesh[j].SubsetArray;
		}
		delete[] modelData->Mesh;

		delete[] modelData->Transform;
	}


	//ANIMATOR
	{
		if (Animator->animationNum > 0)
		{
			for (int i = 0; i < Animator->animationNum; i++)
			{
				if (Animator->animation[i].keyFrameNum > 0 && Animator->animation[i].keyFrame)
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
// アニメーションがないモデルの終了処理
//=============================================================================
void UninitModelBinaryFBX(DX11_MODEL* Model, MODEL_DATA* modelData)
{
	//MODEL
	{
		for (int i = 0; i < Model->BuffersNum; i++)
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

	}

	//MODEL DATA
	{
		for (int j = 0; j < modelData->MeshNum; j++)
		{
			delete[] modelData->Mesh[j].VertexArray;
			delete[] modelData->Mesh[j].IndexArray;
			delete[] modelData->Mesh[j].SubsetArray;
		}
		delete[] modelData->Mesh;

		delete[] modelData->Transform;
	}


}


//******************************************
// モデル更新処理
//******************************************
void UpdateModelBinaryFBX(MODEL_DATA* ResultModel, FBX_ANIMATOR& Animator)
{

	AnimationInterpolation(ResultModel, Animator, Animator.curAnim, Animator.nextAnim);


}


//=============================================================================
// 描画処理
//=============================================================================
void DrawModelBinaryFBX(DX11_MODEL* Model, MODEL_DATA  modelData, XMMATRIX worldMatix)
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
		if (modelData.Transform[j].FatherIdx >= 0)
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&modelData.Transform[modelData.Transform[j].FatherIdx].mtxWorld));
		}

		//マトリクスをセーブする
		XMStoreFloat4x4(&modelData.Transform[j].mtxWorld, mtxWorld);

		int drawIdx = j - ModelsDiference;	//empty objects

		if (drawIdx < 0) continue;


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




//*****************************
// ロードモデル関数
// アニメーションがあるモデル
//*****************************
void LoadModelAnimData(char* FileName, MODEL_DATA* modelData, FBX_ANIMATOR* Animator)
{
	// ファイルからセーブデータを読み込む
	FILE* fp;

	printf("\nロード開始・・・");
	fp = fopen(FileName, "rb");	// ファイルをバイナリ読み込みモードでOpenする

	if (fp != NULL)		// ファイルがあれば書き込み、無ければ無視
	{
		//Model data
		fread(&modelData->ModelName, sizeof(char), 256, fp);
		fread(&modelData->MeshNum, sizeof(unsigned short), 1, fp);
		modelData->Mesh = new MESH_DATA[modelData->MeshNum];

		//メッシュデータ
		for (int i = 0; i < modelData->MeshNum; i++)
		{
			//頂点バッファー
			fread(&modelData->Mesh[i].VertexNum, sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].VertexArray = new VERTEX_3D[modelData->Mesh[i].VertexNum];
			for (int j = 0; j < modelData->Mesh[i].VertexNum; j++)
			{
				fread(&modelData->Mesh[i].VertexArray[j], sizeof(VERTEX_3D), 1, fp);
			}

			//インデクスバッファー
			fread(&modelData->Mesh[i].IndexNum, sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].IndexArray = new unsigned short[modelData->Mesh[i].IndexNum];
			for (int j = 0; j < modelData->Mesh[i].IndexNum; j++)
			{
				fread(&modelData->Mesh[i].IndexArray[j], sizeof(unsigned short), 1, fp);
			}

			//サブセットデータ（マテリアル情報）
			fread(&modelData->Mesh[i].SubsetNum, sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].SubsetArray = new FBXSUBSET[modelData->Mesh[i].SubsetNum];
			for (int j = 0; j < modelData->Mesh[i].SubsetNum; j++)
			{
				fread(&modelData->Mesh[i].SubsetArray[j], sizeof(FBXSUBSET), 1, fp);
			}

		}

		//トランスフォーム情報
		fread(&modelData->TransformNum, 1, sizeof(unsigned short), fp);
		modelData->Transform = new TRANSFORM_DATA[modelData->TransformNum];

		for (int i = 0; i < modelData->TransformNum; i++)
		{
			fread(&modelData->Transform[i].ObjectName, sizeof(char), 50, fp);
			fread(&modelData->Transform[i].FatherIdx, sizeof(int), 1, fp);

			modelData->Transform[i].LclPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
			modelData->Transform[i].LclRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
			modelData->Transform[i].LclScaling = XMFLOAT3(1.0f, 1.0f, 1.0f);
		}




		//アニメーション情報	//Animator
		fread(&Animator->animationNum, 1, sizeof(int), fp);
		Animator->animation = new FBX_ANIMATION[Animator->animationNum];

		for (int i = 0; i < Animator->animationNum; i++)
		{
			fread(&Animator->animation[i].keyFrameNum, sizeof(int), 1, fp);
			fread(&Animator->animation[i].AnimTimeCnt, sizeof(float), 1, fp);
			fread(&Animator->animation[i].AnimTransitionFrames, sizeof(float), 1, fp);
			fread(&Animator->animation[i].loop, sizeof(BOOL), 1, fp);
			fread(&Animator->animation[i].nextAnimation, sizeof(int), 1, fp);
			fread(&Animator->animation[i].speed, sizeof(float), 1, fp);
			fread(&Animator->animation[i].animationName, sizeof(char), 256, fp);

			Animator->animation[i].keyFrame = new FBX_ANIM_KEYFRAME[Animator->animation[i].keyFrameNum];
			//キーフレーム情報
			for (int j = 0; j < Animator->animation[i].keyFrameNum; j++)
			{
				fread(&Animator->animation[i].keyFrame[j].modelosNum, sizeof(int), 1, fp);
				fread(&Animator->animation[i].keyFrame[j].AnimFramesMax, sizeof(float), 1, fp);

				Animator->animation[i].keyFrame[j].Modelo = new ANIM_MODEL[Animator->animation[i].keyFrame[j].modelosNum];

				//オブジェクトのトランスフォーム情報
				for (int z = 0; z < Animator->animation[i].keyFrame[j].modelosNum; z++)
				{
					fread(&Animator->animation[i].keyFrame[j].Modelo[z].LclPosition, sizeof(XMFLOAT3), 1, fp);
					fread(&Animator->animation[i].keyFrame[j].Modelo[z].LclRotation, sizeof(XMFLOAT4), 1, fp);
					fread(&Animator->animation[i].keyFrame[j].Modelo[z].LclScaling, sizeof(XMFLOAT3), 1, fp);
				}
			}
		}

		//アニメーション管理変数
		fread(&Animator->curAnim, sizeof(int), 1, fp);
		fread(&Animator->nextAnim, sizeof(int), 1, fp);
		fread(&Animator->transitionFramesCnt, sizeof(float), 1, fp);
		fread(&Animator->hiddenObj, sizeof(int), HIDDEN_OBJ_NUM, fp);

		fclose(fp);								// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}

}


//*****************************
// ロードモデル関数
// アニメーションがないモデル
//*****************************
void LoadModelData(char* FileName, MODEL_DATA* modelData)
{
	// ファイルからセーブデータを読み込む
	FILE* fp;

	printf("\nロード開始・・・");
	fp = fopen(FileName, "rb");	// ファイルをバイナリ読み込みモードでOpenする

	if (fp != NULL)						// ファイルがあれば書き込み、無ければ無視
	{
		//Model data
		fread(&modelData->ModelName, sizeof(char), 256, fp);
		fread(&modelData->MeshNum, sizeof(unsigned short), 1, fp);
		modelData->Mesh = new MESH_DATA[modelData->MeshNum];

		//メッシュデータ
		for (int i = 0; i < modelData->MeshNum; i++)
		{
			//頂点バッファー
			fread(&modelData->Mesh[i].VertexNum, sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].VertexArray = new VERTEX_3D[modelData->Mesh[i].VertexNum];
			for (int j = 0; j < modelData->Mesh[i].VertexNum; j++)
			{
				fread(&modelData->Mesh[i].VertexArray[j], sizeof(VERTEX_3D), 1, fp);
			}

			//インデクスバッファー
			fread(&modelData->Mesh[i].IndexNum, sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].IndexArray = new unsigned short[modelData->Mesh[i].IndexNum];
			for (int j = 0; j < modelData->Mesh[i].IndexNum; j++)
			{
				fread(&modelData->Mesh[i].IndexArray[j], sizeof(unsigned short), 1, fp);
			}

			//サブセットデータ（マテリアル情報）
			fread(&modelData->Mesh[i].SubsetNum, sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].SubsetArray = new FBXSUBSET[modelData->Mesh[i].SubsetNum];
			for (int j = 0; j < modelData->Mesh[i].SubsetNum; j++)
			{
				fread(&modelData->Mesh[i].SubsetArray[j], sizeof(FBXSUBSET), 1, fp);
			}

		}

		//トランスフォーム情報
		fread(&modelData->TransformNum, 1, sizeof(unsigned short), fp);
		modelData->Transform = new TRANSFORM_DATA[modelData->TransformNum];

		for (int i = 0; i < modelData->TransformNum; i++)
		{
			fread(&modelData->Transform[i].ObjectName, sizeof(char), 50, fp);
			fread(&modelData->Transform[i].FatherIdx, sizeof(int), 1, fp);

			modelData->Transform[i].LclPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
			modelData->Transform[i].LclRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
			modelData->Transform[i].LclScaling = XMFLOAT3(1.0f, 1.0f, 1.0f);
		}


		fclose(fp);								// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}

}



//******************************************
// アニメーションの更新処理
// 説明：ブレンディング、非表示、
//******************************************
void AnimationInterpolation(MODEL_DATA* ResultModel, FBX_ANIMATOR& Animator, int& curAnim, int& nextAnim)
{
	//アニメーションがない場合、戻る
	if (curAnim < 0) return;	
	if (Animator.animation[curAnim].AnimTimeCnt < 0 || Animator.animation[curAnim].AnimTimeCnt > 100) return;

	float& curAnimFramesCnt = Animator.animation[curAnim].AnimTimeCnt;	//アニメーションの現在のフレーム（時間）

	int CurKF = (int)curAnimFramesCnt; //今のキーフレーム　//current Key frame

	float curAnimSpeed = Animator.animation[curAnim].speed;	//アニメーションの速度

	float  curAnimFramesMax = Animator.animation[curAnim].keyFrame[CurKF].AnimFramesMax;	//アニメーションの最終フレーム（時間）
	curAnimFramesMax /= curAnimSpeed;	//最終フレームをアニメーションの速度に合わせる

	//現在再生しているアニメーションを更新する
	for (int i = 0; i < Animator.animation[curAnim].keyFrame[CurKF].modelosNum; i++)
	{
		// 今のキーフレームと次のキーフレームをXMVECTORに変換する　// Convertir las representaciones iniciales y finales a XMVECTOR
		XMVECTOR nowPos = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF].Modelo[i].LclPosition);
		XMVECTOR nowRot = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF].Modelo[i].LclRotation);
		XMVECTOR nowScl = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF].Modelo[i].LclScaling);

		XMVECTOR NextPos = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF + 1].Modelo[i].LclPosition);
		XMVECTOR NextRot = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF + 1].Modelo[i].LclRotation);
		XMVECTOR NextScl = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF + 1].Modelo[i].LclScaling);


		float nowTime = curAnimFramesCnt - CurKF;	// 時間部分である少数を取り出している

		//線形補間で次のフレームを計算する
		XMVECTOR resultPos = XMVectorLerp(nowPos, NextPos, nowTime);
		XMVECTOR resultRot = XMVectorLerp(nowRot, NextRot, nowTime);
		XMVECTOR resultScl = XMVectorLerp(nowScl, NextScl, nowTime);

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
				nextAnim = Animator.animation[curAnim].nextAnimation;	//次のアニメーションに変更する
			}
			curAnimFramesCnt -= 1.0f / curAnimFramesMax;
		}
		else //ループする場合
		{
			curAnimFramesCnt -= Animator.animation[curAnim].keyFrameNum - 1;				// ０番目にリセットしつつも小数部分を引き継いでいる
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
			XMVECTOR nowPos = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF].Modelo[i].LclPosition);
			XMVECTOR nowRot = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF].Modelo[i].LclRotation);
			XMVECTOR nowScl = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF].Modelo[i].LclScaling);

			XMVECTOR NextPos = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF + 1].Modelo[i].LclPosition);
			XMVECTOR NextRot = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF + 1].Modelo[i].LclRotation);
			XMVECTOR NextScl = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF + 1].Modelo[i].LclScaling);


			float nowTime = nextAnimFramesCnt - NextAnimCurKF;	// 時間部分である少数を取り出している

			//線形補間で次のフレームを計算する
			nextAnimPos = XMVectorLerp(nowPos, NextPos, nowTime);
			nextAnimRos = XMVectorLerp(nowRot, NextRot, nowTime);
			nextAnimScl = XMVectorLerp(nowScl, NextScl, nowTime);



			//**************************
			// TRANSITION INTERPOLATION
			// アニメーションブレンディング
			//**************************

			// 今再生しているアニメーションの値をXMVECTORに変換する
			XMVECTOR nowAnimPos = XMLoadFloat3(&ResultModel->Transform[i].LclPosition);		// XMVECTORへ変換
			XMVECTOR nowAnimRot = XMLoadFloat3(&ResultModel->Transform[i].LclRotation);		// XMVECTORへ変換
			XMVECTOR nowAnimScl = XMLoadFloat3(&ResultModel->Transform[i].LclScaling);		// XMVECTORへ変換


			//両方のアニメーションの差を計算する
			XMVECTOR DiferencePos = (nextAnimPos)-nowAnimPos;	// XYZ移動量を計算している
			XMVECTOR DiferenceRot = (nextAnimRos)-nowAnimRot;	// XYZ回転量を計算している
			XMVECTOR DiferenceScl = (nextAnimScl)-nowAnimScl;	// XYZ拡大率を計算している

			float transitionTime = transitionFramesCnt / curtransitionFramesMax;	// 時間部分である少数を取り出している


			DiferencePos *= transitionTime;								// 現在の移動量を計算している
			DiferenceRot *= transitionTime;								// 現在の回転量を計算している
			DiferenceScl *= transitionTime;								// 現在の拡大率を計算している

			//ブレンディングの結果をセーブする
			XMStoreFloat3(&ResultModel->Transform[i].LclPosition, nowAnimPos + DiferencePos);
			XMStoreFloat3(&ResultModel->Transform[i].LclRotation, nowAnimRot + DiferenceRot);
			XMStoreFloat3(&ResultModel->Transform[i].LclScaling, nowAnimScl + DiferenceScl);


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

		if ((int)nextAnimFramesCnt >= Animator.animation[nextAnim].keyFrameNum - 1)			// アニメーションが終わった？
		{
			nextAnimFramesCnt -= Animator.animation[nextAnim].keyFrameNum - 1;				// ０番目にリセットしつつも小数部分を引き継いでいる
		}



		transitionFramesCnt++;

		//ブレンディングの終了処理
		if (transitionFramesCnt >= curtransitionFramesMax)
		{
			curAnimFramesCnt = 0.0f;
			curAnim = nextAnim;
			nextAnim = -1;

			Animator.transitionFramesCnt = 0.0f;
		}

	}
}


// 角度差を正規化する
float NormalizeAngle(float angle)
{
	while (angle < -XM_PI)
	{
		angle += XM_2PI;
	}

	while (angle > XM_PI)
	{
		angle -= XM_2PI;
	}

	return angle;
}


//線形補間
float lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

//=============================================================================
// ベジェ曲線関数
// 戻り値：tによって、そのOBJの座標（Vector3）
//=============================================================================
XMFLOAT3 BezierFunction(float t, XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3)
{
	XMVECTOR P0 = XMLoadFloat3(&p0);
	XMVECTOR P1 = XMLoadFloat3(&p1);
	XMVECTOR P2 = XMLoadFloat3(&p2);
	XMVECTOR P3 = XMLoadFloat3(&p3);



	XMVECTOR p =
		powf((1 - t), 3) * P0 +
		3 * powf((1 - t), 2) * t * P1 +
		3 * (1 - t) * powf(t, 2) * P2 +
		powf(t, 3) * P3;

	XMFLOAT3 ans;
	XMStoreFloat3(&ans, p);

	return ans;
}
