//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "light.h"
#include "particle.h"
#include "modelBinaryFBX.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX				(2)			// テクスチャの数

#define	PARTICLE_SIZE			(9.0f)		// 頂点サイズ

#define	VALUE_MOVE_PARTICLE		(5.0f)		// 移動速度

#define	MAX_PARTICLE			(512)		// パーティクル最大数
#define	PARTICLE_HEATH_MAX		(30)		// パーティクル最大数
#define	PARTICLE_HEATH_TIME_MAX (120)		// パーティクル最大数
#define	PARTICLE_RATE_HEATH		(5)			// パーティクル最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;		// 位置
	XMFLOAT3		rot;		// 回転
	XMFLOAT3		scale;		// スケール
	XMFLOAT3		move;		// 移動量
	MATERIAL		material;	// マテリアル
	int				nLife;		// 寿命
	float			angle;		// HPパーティクルの回り角度
	BOOL			use;		// 使用しているかどうか

	int				type;		// 使用しているかどうか
	int				texNo;		// テクスチャ番号


} PARTICLE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticle(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static PARTICLE					g_aParticle[MAX_PARTICLE];	// パーティクルワーク
static XMFLOAT3					g_posBase;					// ビルボード発生位置
static float					g_fWidthBase = 2.0f;		// 基準の幅
static float					g_fHeightBase = 10.0f;		// 基準の高さ
static float					g_roty = 0.0f;				// 移動方向
static float					g_spd = 0.0f;				// 移動スピード

//CURE PARTICLE
static BOOL						g_CureParticles;			// 
static int						g_ParticleRateCnt;			// 再生頻度
static int						g_ParticleTimeCnt;
static int						g_ParticleCnt;

static XMFLOAT3					g_ParticlePos;

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/Menu/star.png",
	"data/TEXTURE/Menu/star_06.png",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticle(void)
{
	// 頂点情報の作成
	MakeVertexParticle();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// パーティクルワークの初期化
	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		g_aParticle[nCntParticle].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_aParticle[nCntParticle].material, sizeof(g_aParticle[nCntParticle].material));
		g_aParticle[nCntParticle].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aParticle[nCntParticle].nLife = 0;
		g_aParticle[nCntParticle].use = FALSE;
	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticle(void)
{
	if (g_Load == FALSE) return;

	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticle(void)
{
	//パーティクルの更新
	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(g_aParticle[nCntParticle].use)
		{// 使用中

			switch (g_aParticle[nCntParticle].type)
			{
			case PARTICLE_HIT:

				g_aParticle[nCntParticle].pos.x += g_aParticle[nCntParticle].move.x;
				g_aParticle[nCntParticle].pos.z += g_aParticle[nCntParticle].move.z;

				g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y;
				if (g_aParticle[nCntParticle].pos.y <= PARTICLE_SIZE / 2)
				{// 着地した
					g_aParticle[nCntParticle].pos.y = PARTICLE_SIZE / 2;
					g_aParticle[nCntParticle].move.y = -g_aParticle[nCntParticle].move.y * 0.75f;
				}

				g_aParticle[nCntParticle].move.x += (0.0f - g_aParticle[nCntParticle].move.x) * 0.015f;
				g_aParticle[nCntParticle].move.y -= 0.05f;
				g_aParticle[nCntParticle].move.z += (0.0f - g_aParticle[nCntParticle].move.z) * 0.015f;


				g_aParticle[nCntParticle].nLife--;
				if (g_aParticle[nCntParticle].nLife <= 0)
				{
					g_aParticle[nCntParticle].use = FALSE;
				}
				else
				{
					if (g_aParticle[nCntParticle].nLife <= 20)
					{
						g_aParticle[nCntParticle].material.Diffuse.x = 0.8f - (float)(20 - g_aParticle[nCntParticle].nLife) / 20 * 0.8f;
						g_aParticle[nCntParticle].material.Diffuse.y = 0.7f - (float)(20 - g_aParticle[nCntParticle].nLife) / 20 * 0.7f;
						g_aParticle[nCntParticle].material.Diffuse.z = 0.2f - (float)(20 - g_aParticle[nCntParticle].nLife) / 20 * 0.2f;
					}

					if (g_aParticle[nCntParticle].nLife <= 20)
					{
						// α値設定
						g_aParticle[nCntParticle].material.Diffuse.w -= 0.05f;
						if (g_aParticle[nCntParticle].material.Diffuse.w < 0.0f)
						{
							g_aParticle[nCntParticle].material.Diffuse.w = 0.0f;
						}
					}
				}

				g_aParticle[nCntParticle].rot.z -= 0.05f;

				break;

			case PARTICLE_HEATH:
				g_aParticle[nCntParticle].angle += g_aParticle[nCntParticle].move.x/*0.05f*/;
				if (g_aParticle[nCntParticle].angle > XM_2PI)
				{
					g_aParticle[nCntParticle].angle = 0.0f;
				}

				g_aParticle[nCntParticle].pos.x = g_ParticlePos.x + sinf(g_aParticle[nCntParticle].angle) * 15.0f;
				g_aParticle[nCntParticle].pos.z = g_ParticlePos.z + cosf(g_aParticle[nCntParticle].angle) * 15.0f;

				g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y;


				g_aParticle[nCntParticle].nLife--;
				if (g_aParticle[nCntParticle].nLife <= 0)
				{
					g_aParticle[nCntParticle].use = FALSE;
					g_ParticleCnt--;
				}
				else if(g_aParticle[nCntParticle].nLife <= 30)
				{
					
					// α値設定
					g_aParticle[nCntParticle].material.Diffuse.w = (float)g_aParticle[nCntParticle].nLife / 30.0f;
					
				}

				break;

			default:
				break;
			}
		}
	}

	


	if (g_CureParticles)
	{

		if (g_ParticleTimeCnt++ > PARTICLE_HEATH_TIME_MAX)
		{
			g_CureParticles = FALSE;
			g_ParticleTimeCnt = 0;
		}


		g_ParticleRateCnt++;
		// パーティクル発生
		if (g_ParticleCnt < PARTICLE_HEATH_MAX && g_ParticleRateCnt >= PARTICLE_RATE_HEATH)
		{
			g_ParticleRateCnt = 0;
			g_ParticleCnt++;

			XMFLOAT3 pos;
			XMFLOAT3 scl;
			XMFLOAT3 move;
			float fAngle, fLength;
			int nLife;
			float fSize;
			float alpha;

			pos = g_ParticlePos;
			scl = XMFLOAT3(0.2f, 0.2f, 0.2f);


			fAngle = (float)(rand() % 628 - 314) / 100.0f;
			fLength = rand() % (int)(g_fWidthBase * 200) / 100.0f - g_fWidthBase;

			move.x = (float)((rand() % 6) / 100.0f + 0.02f);;
	

			move.y = rand() % 20 / 100.0f + 0.2f;


			move.z = cosf(fAngle) * fLength;
			move.z /= 30.0f;

			nLife = rand() % 100 + 70;

			fSize = (float)((rand() % 200) / 100.0f + 0.5f);

			alpha = (rand() % 50 + 50) / 100.0f;

			// ビルボードの設定
			SetParticle(pos, move, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), fSize, nLife, fAngle, PARTICLE_HEATH);
		}
	}


	#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_O))
	{
		// パーティクル発生
		//SetHitParticle(XMFLOAT3(0.0f, 15.0f, 0.0f), FALSE);
		g_CureParticles = TRUE;
	}
	if (GetKeyboardTrigger(DIK_I))
	{
		// パーティクル発生
		//SetHitParticle(XMFLOAT3(0.0f, 15.0f, 0.0f), TRUE);
		g_CureParticles = FALSE;
	}
	#endif // DEBUG

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticle(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ライティングを無効に
	SetLightEnable(FALSE);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(FALSE);

	// フォグ無効
	SetFogEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(g_aParticle[nCntParticle].use)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_aParticle[nCntParticle].texNo]);

			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			// 処理が速いしお勧め
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// スケールを反映
			mtxScl = XMMatrixScaling(g_aParticle[nCntParticle].scale.x, g_aParticle[nCntParticle].scale.y, g_aParticle[nCntParticle].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_aParticle[nCntParticle].pos.x, g_aParticle[nCntParticle].pos.y, g_aParticle[nCntParticle].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_aParticle[nCntParticle].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(TRUE);

	// フォグ有効
	SetFogEnable( GetFogEnable() );

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE / 2, PARTICLE_SIZE / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE / 2, PARTICLE_SIZE / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE / 2, -PARTICLE_SIZE / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE / 2, -PARTICLE_SIZE / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorParticle(int nIdxParticle, XMFLOAT4 col)
{
	g_aParticle[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理
//=============================================================================
int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float Size, int nLife, float angle, int type)
{
	int nIdxParticle = -1;

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(!g_aParticle[nCntParticle].use)
		{
			g_aParticle[nCntParticle].pos = pos;
			g_aParticle[nCntParticle].rot   = { 0.0f, 0.0f, 0.0f };
			g_aParticle[nCntParticle].scale = { Size, Size, Size };
			g_aParticle[nCntParticle].move = move;
			g_aParticle[nCntParticle].material.Diffuse = col;
			g_aParticle[nCntParticle].nLife = nLife;
			g_aParticle[nCntParticle].use = TRUE;
			g_aParticle[nCntParticle].angle = angle;

			g_aParticle[nCntParticle].type = type;
			g_aParticle[nCntParticle].texNo = type;

			nIdxParticle = nCntParticle;

			break;
		}
	}

	return nIdxParticle;
}

//=============================================================================
// 攻撃のパーティクルの発生処理
//=============================================================================
void SetHitParticle(XMFLOAT3 position, BOOL crit)
{
	for (int i = 0; i < 6; i++)
	{
		XMFLOAT3 pos;
		XMFLOAT3 move;
		float fAngle, fLength;
		int nLife;
		float fSize;

		pos = position;
		pos.y = 10.0f;

		fAngle = (float)(rand() % 628 - 314) / 100.0f;
		fLength = rand() % (int)(100) / 100.0f + 0.3f;
		move.x = sinf(fAngle) * fLength;
		move.y = rand() % 20 / 10.0f /*+ g_fHeightBase*/;
		move.z = cosf(fAngle) * fLength;

		nLife = rand() % 30 + 50;

		fSize = (float)(rand() % 10 - 5);

		fSize = 1 + (fSize / 10);

		XMFLOAT4 color = crit ? XMFLOAT4(1.0f, 0.1f, 0.1f, 1.0f) : XMFLOAT4(0.8f, 0.7f, 0.2f, 1.0f);
		
		// ビルボードの設定
		SetParticle(pos, move, color, fSize, nLife, 0.0f, PARTICLE_HIT);
	}

}

void SetHitParticle(XMFLOAT3 position)
{
	g_ParticlePos	= position;
	g_CureParticles = TRUE;
}