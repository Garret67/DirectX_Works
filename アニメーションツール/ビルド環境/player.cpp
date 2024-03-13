//=============================================================================
//
// モデル処理 [player.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "FBXmodel.h"
#include "FBXanimation.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"
#include "fileSaveLoad.h"
#include "interface.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define	VALUE_MOVE			(2.0f)							// 移動量


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static FBXPLAYER			g_fbxPlayer;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{

	InitAnimator(&g_fbxPlayer.Animator, MODEL_ANIMATION_MAX);
	
	g_fbxPlayer.pos = { 0.0f, 0.0f, 0.0f };
	g_fbxPlayer.rot = { 0.0f, 0.0f, 0.0f };
	g_fbxPlayer.scl = { 1.0f, 1.0f, 1.0f };

	g_fbxPlayer.spd  = 0.0f;			// 移動スピードクリア

	g_fbxPlayer.use = true;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (g_fbxPlayer.load)
	{
		UnloadFBXModel(&g_fbxPlayer.model, &g_fbxPlayer.modelData);
		UnloadFBXanimation(&g_fbxPlayer.Animator);
		g_fbxPlayer.load = false;
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	if(g_fbxPlayer.Animator.curAnim != -1)
	{
		AnimationInterpolation(&g_fbxPlayer.modelData, g_fbxPlayer.Animator, g_fbxPlayer.Animator.curAnim, g_fbxPlayer.Animator.nextAnim);
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	//PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_fbxPlayer.pos.x, g_fbxPlayer.pos.y, g_fbxPlayer.pos.z);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	if (!g_fbxPlayer.load)return;
	// カリング無効
	//SetCullingMode(CULL_MODE_NONE);
	SetCullingMode(CULL_MODE_BACK);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_fbxPlayer.scl.x, g_fbxPlayer.scl.y, g_fbxPlayer.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_fbxPlayer.rot.x, g_fbxPlayer.rot.y + XM_PI, g_fbxPlayer.rot.z);
	//mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_fbxPlayer.pos.x, g_fbxPlayer.pos.y, g_fbxPlayer.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	//SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_fbxPlayer.mtxWorld, mtxWorld);

	//SetAlphaTestEnable(TRUE);
	//SetLightEnable(FALSE);
	
	// モデル描画
	DrawFBXModel(&g_fbxPlayer.model, g_fbxPlayer.modelData, mtxWorld);

	SetAlphaTestEnable(FALSE);
	SetLightEnable(TRUE);

	FLOAT left = 0.0f + 480;
	FLOAT top = 400.0f;
	FLOAT right = 300.0f + 480;
	FLOAT bottom = 400.0f;


	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
FBXPLAYER *GetPlayer(void)
{
	return &g_fbxPlayer;
}

//ロードモデルのボタン
void LoadModelButton(WCHAR* modelText)
{
	//コンソールウィンドウを取得
	HWND hwndOwner = GetConsoleWindow();
	char filePath[256] = {};

	//ファイルエクスプローラーを開いて、ファイルパスを取得する
	if (OpenFileAndGetPath(hwndOwner, filePath))
	{
		if (strcmp(filePath, "") == 0)
		{
			mbstowcs(modelText, "Invalid Model File", 256);
			return;
		}

		//再初期化
		UninitPlayer();
		InitPlayer();

		//モデルを読み込む
		LoadFBXModel(filePath, &g_fbxPlayer.model, &g_fbxPlayer.modelData);
		g_fbxPlayer.load = TRUE;

		//違うファイルだったら
		if (g_fbxPlayer.modelData.MeshNum <= 0 || g_fbxPlayer.modelData.MeshNum > 1000)
		{
			mbstowcs(modelText, "Invalid Model File", 256);
			return;
		}


		//strcpy(path, FileName);
		char* adr = filePath;
		char* ans = adr;
		while (1)
		{
			adr = strstr(adr, "\\");		//strstr devuelve un puntero a la primera aparicion de _SubString en _string
			if (adr == NULL) break;
			else ans = adr;
			adr++;
		}
		if (filePath != ans) ans++;

		mbstowcs(modelText, ans, 256);
		strcpy(g_fbxPlayer.modelData.ModelName, ans);
	}
	else
	{
		mbstowcs(modelText, "Model File not found", 256);
	}

}


//ロードアニメーションのボタン
void LoadAnimationButton(WCHAR* animText, WCHAR* transitionFramesText, WCHAR* nextAnimationtext, int animNum, int activeAnimNum)
{
	//コンソールウィンドウを取得
	HWND hwndOwner = GetConsoleWindow();
	char filePath[256] = {};

	//ファイルエクスプローラーを開いて、ファイルパスを取得する
	if (OpenFileAndGetPath(hwndOwner, filePath))
	{
		if (strcmp(filePath, "") == 0)
		{
			mbstowcs(animText, "Invalid Animation File", 256);
			return;
		}
		LoadAnimationFromFBX(filePath, &g_fbxPlayer.Animator.animation[animNum], g_fbxPlayer.modelData.TransformNum);

		if (g_fbxPlayer.Animator.animation[animNum].keyFrameNum <= 0 || g_fbxPlayer.Animator.animation[animNum].keyFrameNum > 100)
		{
			mbstowcs(animText, "Invalid Animation File", 256);
			return;
		}


		char* adr = filePath;
		char* ans = adr;
		while (1)
		{
			adr = strstr(adr, "\\");		//strstr devuelve un puntero a la primera aparicion de _SubString en _string
			if (adr == NULL) break;
			else ans = adr;
			adr++;
		}
		if (filePath != ans) ans++;	


		mbstowcs(animText, ans, 256);
		strncpy(g_fbxPlayer.Animator.animation[animNum].animationName, ans, 255);
		TransitionFramesButton(animNum, transitionFramesText, 0);
		NextAnimationButton(animNum, activeAnimNum, nextAnimationtext, 0);

		g_fbxPlayer.Animator.curAnim = animNum;
	}
	else
	{
		mbstowcs(animText, "Animation File not found", 256);
	}
	

}



//再生/停止ボタン
void PlayButton(int AnimNum)
{
	if (g_fbxPlayer.Animator.curAnim < 0)
	{
		g_fbxPlayer.Animator.curAnim = AnimNum;
	}
	else if (g_fbxPlayer.Animator.curAnim == AnimNum)
	{
		g_fbxPlayer.Animator.curAnim = -1;
	}
	else
	{
		g_fbxPlayer.Animator.nextAnim = AnimNum;
	}

}

//アニメーションループボタン
BOOL LoopButton(int AnimNum)
{
	g_fbxPlayer.Animator.animation[AnimNum].loop = !g_fbxPlayer.Animator.animation[AnimNum].loop;

	return g_fbxPlayer.Animator.animation[AnimNum].loop;
}

//アニメーション速度ボタン
void SpeedButton(int AnimNum, float speed)
{
	if(g_fbxPlayer.Animator.animation[AnimNum].speed > 0.05f)
	{
		g_fbxPlayer.Animator.animation[AnimNum].speed = speed;
	}
}

//アニメーションブレンディングボタン
void TransitionFramesButton(int AnimNum, WCHAR *text, float sumando)
{
	float& frames = g_fbxPlayer.Animator.animation[AnimNum].AnimTransitionFrames;

	if (sumando > 0) frames = frames < 60 ? frames + sumando : frames;
	if (sumando < 0) frames = frames > 0  ? frames + sumando : frames;

	if (frames > 1 && frames < 60)
	{

		const WCHAR* prefix = L"Transition frames: ";

		// Convertir el float a una cadena con un solo digito decimal
		WCHAR speedStr[25]; // Ajusta el tamano segun tus necesidades
		swprintf_s(speedStr, L"%d", (int)frames);

		// Crear una cadena final
		WCHAR resultado[50]; // Ajusta el tamano segun tus necesidades
		wcscpy(resultado, prefix);
		wcscat(resultado, speedStr);

		wcscpy(text, resultado);
	}
	//else
	//{
	//	wcscpy(text, L"Transition frames: ??");
	//}
}

//次のアニメーションのボタン
void NextAnimationButton(int AnimNum, int ActiveAnimNum, WCHAR* text, int sumando)
{
	int& animIdx = g_fbxPlayer.Animator.animation[AnimNum].nextAnimation;

	BOOL invalidAnimation = TRUE;
	int loopCnt = 0;

	while (invalidAnimation)
	{

		if (sumando > 0) animIdx = (animIdx + sumando) % ActiveAnimNum;

		if (sumando < 0) animIdx = animIdx <= 0 ? (ActiveAnimNum - 1) : --animIdx;


		if (strcmp(g_fbxPlayer.Animator.animation[animIdx].animationName, "") != 0 &&
			strcmp(g_fbxPlayer.Animator.animation[animIdx].animationName, "Invalid Animation File") != 0 &&
			strcmp(g_fbxPlayer.Animator.animation[animIdx].animationName, "Animation File not found") != 0)
		{
			invalidAnimation = FALSE;	//Valid animation
		}
		else
		{
			if (sumando == 0)
			{
				animIdx = (animIdx + 1) % ActiveAnimNum;
			}
		}

		if (loopCnt >= ActiveAnimNum) invalidAnimation = FALSE;	//No valid animation
		loopCnt++;
	}

	if (animIdx >= 0 && animIdx <= (ActiveAnimNum - 1))
	{
		//in case there is no valid animation
		if (strcmp(g_fbxPlayer.Animator.animation[animIdx].animationName, "") == 0 ||
			strcmp(g_fbxPlayer.Animator.animation[animIdx].animationName, "Invalid Animation File") == 0 ||
			strcmp(g_fbxPlayer.Animator.animation[animIdx].animationName, "Animation File not found") == 0)
		{
			mbstowcs(text, "Next animation: Undefined!", 256);
		}
		else 
		{
			char* prefix = "Next animation: ";

			char result[256];
			strcpy(result, prefix);
			strcat(result, g_fbxPlayer.Animator.animation[animIdx].animationName);

			mbstowcs(text, result, 256);
		}
	}
}


//アニメーションを削除
void DeleteAnimation(int showingAnim, int g_ActiveAnimNum)
{
	//ANIMARION KEY FRAMES
	{
		if (g_fbxPlayer.Animator.animation[showingAnim].keyFrameNum > 0 && g_fbxPlayer.Animator.animation[showingAnim].keyFrameNum < 100)
		{
			for (int i = 0; i < g_fbxPlayer.Animator.animation[showingAnim].keyFrameNum; i++)
			{
				if (g_fbxPlayer.Animator.animation[showingAnim].keyFrame[i].Modelo) delete[] g_fbxPlayer.Animator.animation[showingAnim].keyFrame[i].Modelo;
			}
			if (g_fbxPlayer.Animator.animation[showingAnim].keyFrame) delete[] g_fbxPlayer.Animator.animation[showingAnim].keyFrame;
			g_fbxPlayer.Animator.animation[showingAnim].keyFrameNum = 0;
			strcpy(g_fbxPlayer.Animator.animation[showingAnim].animationName, "");
		}

		g_fbxPlayer.Animator.animation[showingAnim].AnimTimeCnt = 0.0f;
		g_fbxPlayer.Animator.animation[showingAnim].AnimTransitionFrames = 10.0f;
		g_fbxPlayer.Animator.animation[showingAnim].nextAnimation = 0;
		g_fbxPlayer.Animator.animation[showingAnim].loop = TRUE;
		g_fbxPlayer.Animator.animation[showingAnim].speed = -1.0f;

	}

	//アニメーションを置き換える
	if (showingAnim != g_ActiveAnimNum - 1)
	{
		for (int i = showingAnim; i < (g_ActiveAnimNum - 1)/* - showingAnim*/; i++)
		{
			g_fbxPlayer.Animator.animation[i] = g_fbxPlayer.Animator.animation[i + 1];
		}

		g_fbxPlayer.Animator.animation[g_ActiveAnimNum - 1].keyFrameNum = 0;

		g_fbxPlayer.Animator.animation[g_ActiveAnimNum - 1].AnimTimeCnt = 0.0f;
		g_fbxPlayer.Animator.animation[g_ActiveAnimNum - 1].AnimTransitionFrames = 30.0f;
		g_fbxPlayer.Animator.animation[g_ActiveAnimNum - 1].nextAnimation = 0;
		g_fbxPlayer.Animator.animation[g_ActiveAnimNum - 1].loop = TRUE;
		g_fbxPlayer.Animator.animation[g_ActiveAnimNum - 1].speed = 1.0f;

		strcpy(g_fbxPlayer.Animator.animation[g_ActiveAnimNum - 1].animationName, "");
		g_fbxPlayer.Animator.animation[g_ActiveAnimNum - 1].keyFrameNum = 0;
	}

	if (g_fbxPlayer.Animator.curAnim == showingAnim ||
		g_fbxPlayer.Animator.curAnim == g_ActiveAnimNum - 1)
	{
		g_fbxPlayer.Animator.curAnim = -1;
	}
}

//モデルをバイナリファイルとして出力
void SaveModelAnimation(int activeAnimNum)
{
	SaveModel(g_fbxPlayer.modelData, g_fbxPlayer.Animator, activeAnimNum);
}

//現在のアニメーションを取得
int GetCurAnimation(void)
{
	return g_fbxPlayer.Animator.curAnim;
}


//アニメーション欄を更新
void UpdateAnimationZone(int showingAnim, int activeAnimNum, BOOL& loop, float& speed, WCHAR* animText, WCHAR* transitionFramesText, WCHAR* nextAnimationtext)
{
	if (g_fbxPlayer.Animator.animation[showingAnim].speed > 0.01f && g_fbxPlayer.Animator.animation[showingAnim].speed < 2.05f &&
		g_fbxPlayer.Animator.animation[showingAnim].nextAnimation >= 0 && g_fbxPlayer.Animator.animation[showingAnim].nextAnimation < activeAnimNum)
	{
		loop = g_fbxPlayer.Animator.animation[showingAnim].loop;
		speed = g_fbxPlayer.Animator.animation[showingAnim].speed;
		mbstowcs(animText, g_fbxPlayer.Animator.animation[showingAnim].animationName, 256);
		TransitionFramesButton(showingAnim, transitionFramesText, 0);
		NextAnimationButton(showingAnim, activeAnimNum, nextAnimationtext, 0);
	}
	else 
	{
		loop = TRUE;
		speed = 1.0f;
		mbstowcs(animText, "", 256);
		mbstowcs(transitionFramesText, "Transition frames: ??", 256);
		g_fbxPlayer.Animator.animation[showingAnim].nextAnimation = 0;
		mbstowcs(nextAnimationtext,	   "Next animation: Undefined", 256);
	}
}

//オブジェクト非表示ボタン
void HideObjButton(int ButtonIdx, WCHAR* text, int sumando)
{
	int& hiddenObjIdx = g_fbxPlayer.Animator.hiddenObj[ButtonIdx];

	//if (sumando > 0) hiddenObjIdx = (hiddenObjIdx + sumando) % (g_fbxPlayer.modelData.ModelsNum + 1);
	if (sumando > 0) hiddenObjIdx = hiddenObjIdx >= (g_fbxPlayer.modelData.TransformNum) - 1 ? -1 : ++hiddenObjIdx;
	if (sumando < 0) hiddenObjIdx = hiddenObjIdx < 0 ? (g_fbxPlayer.modelData.TransformNum - 1) : --hiddenObjIdx;

	if (g_fbxPlayer.Animator.hiddenObj[ButtonIdx] == -1)
	{
		wcscpy_s(text, 256, L"No obj");
	}
	else
	{
		mbstowcs(text, g_fbxPlayer.modelData.Transform[g_fbxPlayer.Animator.hiddenObj[ButtonIdx]].ObjectName, 256);
	}
}



//セーブされたモデルをロードする
void LoadModelDataBinary(void)
{
	//コンソールウィンドウを取得
	HWND hwndOwner = GetConsoleWindow();
	char filePath[256] = {};

	//ファイルエクスプローラーを開いて、ファイルパスを取得する
	if (OpenFileAndGetPath(hwndOwner, filePath))
	{
		if (strcmp(filePath, "") == 0)
		{
			//mbstowcs(modelText, "Invalid Model File", 256);
			return;
		}

		UninitPlayer();
		InitPlayer();
		
	
		LoadModel(filePath, &g_fbxPlayer.modelData, &g_fbxPlayer.Animator);

		if (g_fbxPlayer.modelData.MeshNum < 0 || g_fbxPlayer.modelData.MeshNum > 1000) return;

		g_fbxPlayer.load = TRUE;


		g_fbxPlayer.model.BuffersNum = g_fbxPlayer.modelData.MeshNum;
		g_fbxPlayer.model.Buffers = new DX11_BUFFERS[g_fbxPlayer.model.BuffersNum];

		for (int j = 0; j < g_fbxPlayer.modelData.MeshNum; j++)
		{
			{
				D3D11_BUFFER_DESC bd;
				ZeroMemory(&bd, sizeof(bd));
				bd.Usage = D3D11_USAGE_DEFAULT;
				bd.ByteWidth = sizeof(VERTEX_3D) * g_fbxPlayer.modelData.Mesh[j].VertexNum;
				bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bd.CPUAccessFlags = 0;

				D3D11_SUBRESOURCE_DATA sd;
				ZeroMemory(&sd, sizeof(sd));
				sd.pSysMem = g_fbxPlayer.modelData.Mesh[j].VertexArray;

				GetDevice()->CreateBuffer(&bd, &sd, &g_fbxPlayer.model.Buffers[j].VertexBuffer);
			}


			// インデックスバッファ生成
			{
				D3D11_BUFFER_DESC bd;
				ZeroMemory(&bd, sizeof(bd));
				bd.Usage = D3D11_USAGE_DEFAULT;
				bd.ByteWidth = sizeof(unsigned short) * g_fbxPlayer.modelData.Mesh[j].IndexNum;
				bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
				bd.CPUAccessFlags = 0;

				D3D11_SUBRESOURCE_DATA sd;
				ZeroMemory(&sd, sizeof(sd));
				sd.pSysMem = g_fbxPlayer.modelData.Mesh[j].IndexArray;

				GetDevice()->CreateBuffer(&bd, &sd, &g_fbxPlayer.model.Buffers[j].IndexBuffer);
			}

			// サブセット設定
			{
				g_fbxPlayer.model.Buffers[j].SubsetArray = new DX11_FBXSUBSET[g_fbxPlayer.modelData.Mesh[j].SubsetNum];
				g_fbxPlayer.model.Buffers[j].SubsetNum = g_fbxPlayer.modelData.Mesh[j].SubsetNum;
				g_fbxPlayer.model.Buffers[j].SubsetArray[0].Material.Texture = NULL;

				for (unsigned short i = 0; i < g_fbxPlayer.modelData.Mesh[j].SubsetNum; i++)
				{
					g_fbxPlayer.model.Buffers[j].SubsetArray[i].StartIndex = g_fbxPlayer.modelData.Mesh[j].SubsetArray[i].StartIndex;
					g_fbxPlayer.model.Buffers[j].SubsetArray[i].IndexNum = g_fbxPlayer.modelData.Mesh[j].SubsetArray[i].IndexNum;

					g_fbxPlayer.model.Buffers[j].SubsetArray[i].Material.Material = g_fbxPlayer.modelData.Mesh[j].SubsetArray[i].Material.Material;

					g_fbxPlayer.model.Buffers[j].SubsetArray[i].Material.Texture = NULL;
					D3DX11CreateShaderResourceViewFromFile(GetDevice(),
						g_fbxPlayer.modelData.Mesh[j].SubsetArray[i].Material.TextureName,
						NULL,
						NULL,
						&g_fbxPlayer.model.Buffers[j].SubsetArray[i].Material.Texture,
						NULL);
				}
			}

		}


		UpdateNewModelInterface(g_fbxPlayer.Animator.animationNum, g_fbxPlayer.modelData.ModelName);
	}
	else
	{
		//mbstowcs(modelText, "File not found", 256);
	}

}
