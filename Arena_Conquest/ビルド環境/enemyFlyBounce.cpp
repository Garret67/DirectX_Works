//=============================================================================
//
// エネミー処理 [enemyFlyBounce.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemyFlyBounce.h"
#include "spawnController.h"
#include "bg.h"
#include "platforms.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100.0f)	// キャラサイズ
#define TEXTURE_HEIGHT				(100.0f)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_ANIM_SPRITES_MAX	(10)		// 1つのアニメーションはスプライト何枚（X)
#define TEXTURE_ANIM_MAX			(3)		// アニメーションはいくつ			  （Y)
#define ANIM_WAIT					(7)		// アニメーションの切り替わるWait値

#define DAMAGE_TIME					(30)
#define KNOCKBACK_TIME				(15)
#define DESPAWN_TIME				(180)
#define HP_MAX						(20.0f)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void EnemyFlyBounceAnimationUpdate(void);

void DespawnEnemyFlyBounce(int enemyIndex);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/Wispy/Wispy_anim.png",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static ENEMY_FLY_BOUNCE	g_EnemyFlyBounce[ENEMY_FLY_BOUNCE_MAX];		// エネミー構造体



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemyFlyBounce(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	
	// エネミー構造体の初期化
	for (int i = 0; i < ENEMY_FLY_BOUNCE_MAX; i++)
	{	
		g_EnemyFlyBounce[i].spawned = FALSE;
		g_EnemyFlyBounce[i].alive = FALSE;
		g_EnemyFlyBounce[i].pos = XMFLOAT3(SCREEN_CENTER_X, 1465.0f, 0.0f);	// 中心点から表示
		g_EnemyFlyBounce[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyFlyBounce[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_EnemyFlyBounce[i].w = TEXTURE_WIDTH;
		g_EnemyFlyBounce[i].h = TEXTURE_HEIGHT;
		g_EnemyFlyBounce[i].texNo = 0;

		g_EnemyFlyBounce[i].currentSprite = 0;
		g_EnemyFlyBounce[i].currentAnim = FLY_BOUNCE_ANIM_MOVE;

		g_EnemyFlyBounce[i].moveSpeed = XMFLOAT3(3.0f + (i * 1.0f), -3.0f + (i * 1.0f), 1.0f);		// 移動量
		g_EnemyFlyBounce[i].moveDir = FLY_BOUNCE_DIR_RIGHT;

		g_EnemyFlyBounce[i].HP = HP_MAX;

		g_EnemyFlyBounce[i].damaged = FALSE;
		g_EnemyFlyBounce[i].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyFlyBounce[i].dmgTimeCnt = 0;

		g_EnemyFlyBounce[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemyFlyBounce(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemyFlyBounce(void)
{

	for (int i = 0; i < ENEMY_FLY_BOUNCE_MAX; i++)
	{
		if (g_EnemyFlyBounce[i].spawned == FALSE) continue;
		// 生きてるエネミーだけ処理をする
		if (g_EnemyFlyBounce[i].alive == TRUE)
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_EnemyFlyBounce[i].pos;

			//エネミーのパトロール
			{
				g_EnemyFlyBounce[i].pos.x += g_EnemyFlyBounce[i].moveSpeed.x;
				g_EnemyFlyBounce[i].pos.y += g_EnemyFlyBounce[i].moveSpeed.y;
				
			}


			


			//エネミーが傷付いた
			{
				if (g_EnemyFlyBounce[i].damaged == TRUE)
				{

					if (g_EnemyFlyBounce[i].dmgTimeCnt < KNOCKBACK_TIME)
					{
						XMVECTOR epos = XMLoadFloat3(&g_EnemyFlyBounce[i].pos);
						XMVECTOR vec = (epos - XMLoadFloat3(&g_EnemyFlyBounce[i].damageOriginPos));			//自分とプレイヤーとの差分を求めて
						float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);		//その差分を使って角度を求めている
						float repealDisstance = 10.0f;										//スピードはちょっと遅くしてみる
						float repealDisstancePerFrame = repealDisstance / KNOCKBACK_TIME;

						//SMOOTH
						float repealDistNow = repealDisstancePerFrame * (KNOCKBACK_TIME - g_EnemyFlyBounce[i].dmgTimeCnt);



						g_EnemyFlyBounce[i].pos.x += cosf(angle) * repealDistNow;					//angleの方向へ移動
						g_EnemyFlyBounce[i].pos.y += sinf(angle) * repealDistNow;					//angleの方向へ移動
					}


					g_EnemyFlyBounce[i].dmgTimeCnt++;

					if (g_EnemyFlyBounce[i].dmgTimeCnt >= DAMAGE_TIME)
					{
						g_EnemyFlyBounce[i].damaged = FALSE;
						g_EnemyFlyBounce[i].dmgTimeCnt = 0;
					}
				}


			}



			// PLAYERとの当たり判定
			{
				PLAYER* player = GetPlayer();

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (player[j].alive == TRUE && player[j].invincible == FALSE)
					{
						BOOL ans = CollisionBB(g_EnemyFlyBounce[i].pos, g_EnemyFlyBounce[i].w, g_EnemyFlyBounce[i].h,
							player[j].pos, player[j].w, player[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							PlayerDamaged(g_EnemyFlyBounce[i].pos);
						}
					}
				}
			}


			

			//画面端の当たり判定
			{
				BG* bg = GetBG();
				if (g_EnemyFlyBounce[i].pos.x < (g_EnemyFlyBounce[i].w / 2))		// 自分の大きさを考慮して画面外か判定している
				{
					g_EnemyFlyBounce[i].moveSpeed.x *= -1;
					g_EnemyFlyBounce[i].pos.x = g_EnemyFlyBounce[i].w / 2;
				}
				if (g_EnemyFlyBounce[i].pos.x > (bg->w - g_EnemyFlyBounce[i].w / 2))	// 自分の大きさを考慮して画面外か判定している
				{
					g_EnemyFlyBounce[i].moveSpeed.x *= -1;
					g_EnemyFlyBounce[i].pos.x = bg->w - (g_EnemyFlyBounce[i].w / 2);
				}

				if (g_EnemyFlyBounce[i].pos.y < (g_EnemyFlyBounce[i].h / 2))		// 自分の大きさを考慮して画面外か判定している
				{
					g_EnemyFlyBounce[i].moveSpeed.y *= -1;
					g_EnemyFlyBounce[i].pos.y = (g_EnemyFlyBounce[i].h / 2);
				}
				if (g_EnemyFlyBounce[i].pos.y > (bg->h - g_EnemyFlyBounce[i].h / 2))	// 自分の大きさを考慮して画面外か判定している
				{
					g_EnemyFlyBounce[i].moveSpeed.y *= -1;
					g_EnemyFlyBounce[i].pos.y = bg->h - (g_EnemyFlyBounce[i].h / 2);
				}
			}
			

			//FLY_BOUNCEの向き
			if (g_EnemyFlyBounce[i].pos.x < pos_old.x)
			{
				g_EnemyFlyBounce[i].moveDir = FLY_BOUNCE_DIR_LEFT;
			}
			else if (g_EnemyFlyBounce[i].pos.x > pos_old.x)
			{
				g_EnemyFlyBounce[i].moveDir = FLY_BOUNCE_DIR_RIGHT;
			}
		}
		else
		{
			//重力
			g_EnemyFlyBounce[i].pos.y += 5.0f;

			DespawnEnemyFlyBounce(i);
		}
		
		// ground との当たり判定
		{
			PLATFORM* ground = GetGround();

			// groundSの数分当たり判定を行う
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;

				BOOL ans = CheckCollisionAndGetDirection(ground[j].pos.x, ground[j].pos.y, (ground[j].w), (ground[j].h),
					g_EnemyFlyBounce[i].pos.x, g_EnemyFlyBounce[i].pos.y, (g_EnemyFlyBounce[i].w), (g_EnemyFlyBounce[i].h));
				// 当たっている？

				switch (ans)
				{
				case FromTop:
					g_EnemyFlyBounce[i].pos.y = ground[j].pos.y - (ground[j].h / 2) - (g_EnemyFlyBounce[i].h / 2);
					g_EnemyFlyBounce[i].moveSpeed.y *= -1;
					break;

				case FromBottom:
					g_EnemyFlyBounce[i].pos.y = ground[j].pos.y + (ground[j].h / 2) + (g_EnemyFlyBounce[i].h / 2);
					g_EnemyFlyBounce[i].moveSpeed.y *= -1;
					break;

				case FromLeft:
					g_EnemyFlyBounce[i].pos.x = ground[j].pos.x - (ground[j].w / 2) - (g_EnemyFlyBounce[i].w / 2);
					g_EnemyFlyBounce[i].moveSpeed.x *= -1;
					break;

				case FromRight:
					g_EnemyFlyBounce[i].pos.x = ground[j].pos.x + (ground[j].w / 2) + (g_EnemyFlyBounce[i].w / 2);
					g_EnemyFlyBounce[i].moveSpeed.x *= -1;
					break;

				default:

					break;
				}


			}
		}


		// PLATFORM との当たり判定
		{
			PLATFORM_ANIM* platform = GetPlatforms();

			// platformSの数分当たり判定を行う
			for (int j = 0; j < PLATFORM_MAX; j++)
			{
				if (platform[j].collider == FALSE) continue;

				float platformH = 50.0f;

				BOOL ans = CheckCollisionAndGetDirection(platform[j].pos.x, platform[j].pos.y, (platform[j].w), (platformH),
					g_EnemyFlyBounce[i].pos.x, g_EnemyFlyBounce[i].pos.y, (g_EnemyFlyBounce[i].w), (g_EnemyFlyBounce[i].h));
				// 当たっている？

				switch (ans)
				{
				case FromTop:
					g_EnemyFlyBounce[i].pos.y = platform[j].pos.y - (platformH / 2) - (g_EnemyFlyBounce[i].h / 2);
					g_EnemyFlyBounce[i].moveSpeed.y *= -1;
					break;

				case FromBottom:
					g_EnemyFlyBounce[i].pos.y = platform[j].pos.y + (platformH / 2) + (g_EnemyFlyBounce[i].h / 2);
					g_EnemyFlyBounce[i].moveSpeed.y *= -1;
					break;

				case FromLeft:
					g_EnemyFlyBounce[i].pos.x = platform[j].pos.x - (platform[j].w / 2) - (g_EnemyFlyBounce[i].w / 2);
					g_EnemyFlyBounce[i].moveSpeed.x *= -1;
					break;

				case FromRight:
					g_EnemyFlyBounce[i].pos.x = platform[j].pos.x + (platform[j].w / 2) + (g_EnemyFlyBounce[i].w / 2);
					g_EnemyFlyBounce[i].moveSpeed.x *= -1;
					break;

				default:

					break;
				}


			}
		}

	}


	// アニメーション
	EnemyFlyBounceAnimationUpdate();


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemyFlyBounce(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();

	for (int i = 0; i < ENEMY_FLY_BOUNCE_MAX; i++)
	{
		if (g_EnemyFlyBounce[i].spawned == TRUE)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnemyFlyBounce[i].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_EnemyFlyBounce[i].pos.x - bg->pos.x;	// エネミーの表示位置X
			float py = g_EnemyFlyBounce[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
			float pw = g_EnemyFlyBounce[i].w;					// エネミーの表示幅
			float ph = g_EnemyFlyBounce[i].h;					// エネミーの表示高さ

			float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// テクスチャの幅
			float th = 1.0f / TEXTURE_ANIM_MAX;				// テクスチャの高さ
			float tx = tw * g_EnemyFlyBounce[i].currentSprite;	// テクスチャの左上X座標
			float ty = th * g_EnemyFlyBounce[i].currentAnim;	// テクスチャの左上Y座標

			if (g_EnemyFlyBounce[i].moveDir == FLY_BOUNCE_DIR_RIGHT)
			{
				tx += tw;
				tw *= -1.0f;
			}

			ty += 0.01f;



			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_EnemyFlyBounce[i].enemyColor);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		
	}

}


//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
ENEMY_FLY_BOUNCE* GetEnemyFlyBounce(void)
{
	return &g_EnemyFlyBounce[0];
}


void EnemyFlyBounceDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage)
{
	g_EnemyFlyBounce[enemyIndex].damaged = TRUE;
	g_EnemyFlyBounce[enemyIndex].damageOriginPos = playerPos;
	g_EnemyFlyBounce[enemyIndex].HP -= damage;
	PlaySound(SOUND_LABEL_SE_bloodyBlade);
	g_EnemyFlyBounce[enemyIndex].countAnim = ANIM_WAIT + 1;
	g_EnemyFlyBounce[enemyIndex].currentSprite = 0;

	if (g_EnemyFlyBounce[enemyIndex].HP <= 0)
	{
		g_EnemyFlyBounce[enemyIndex].alive = FALSE;
		RoundKill();
	}
}

void EnemyFlyBounceAnimationUpdate(void)
{
	for (int i = 0; i < ENEMY_FLY_BOUNCE_MAX; i++)
	{
		if (g_EnemyFlyBounce[i].spawned == FALSE) continue;

		if (g_EnemyFlyBounce[i].alive == FALSE)
		{
			g_EnemyFlyBounce[i].currentAnim = FLY_BOUNCE_ANIM_DIE;
		}
		else if (g_EnemyFlyBounce[i].damaged == TRUE)
		{
			g_EnemyFlyBounce[i].currentAnim = FLY_BOUNCE_ANIM_HIT;
		}
		else
		{
			g_EnemyFlyBounce[i].currentAnim = FLY_BOUNCE_ANIM_MOVE;
		}

		g_EnemyFlyBounce[i].countAnim ++;


		if (g_EnemyFlyBounce[i].countAnim > ANIM_WAIT)
		{
			g_EnemyFlyBounce[i].countAnim = 0;

			// パターンの切り替え
			if (g_EnemyFlyBounce[i].currentAnim == FLY_BOUNCE_ANIM_MOVE) //ループアニメーション
			{
				g_EnemyFlyBounce[i].currentSprite = (g_EnemyFlyBounce[i].currentSprite + 1) % TEXTURE_ANIM_SPRITES_MAX;
			}
			else //NO ループアニメーション
			{
				if (g_EnemyFlyBounce[i].currentSprite < TEXTURE_ANIM_SPRITES_MAX - 1)
				{
					g_EnemyFlyBounce[i].currentSprite++;
				}
				
			}
			
		}
	}
}

void SpawnEnemyFlyBounce(XMFLOAT3 spawnPos)
{
	for (int i = 0; i < ENEMY_FLY_BOUNCE_MAX; i++)
	{
		if (g_EnemyFlyBounce[i].spawned == FALSE)
		{
			g_EnemyFlyBounce[i].spawned = TRUE;
			g_EnemyFlyBounce[i].alive = TRUE;
			g_EnemyFlyBounce[i].pos = spawnPos;

			int randMove = rand() % 4;	//最初の動きををランダムにする
			switch (randMove)
			{
			case 0:
				break;

			case 1:
				g_EnemyFlyBounce[i].moveSpeed.x *= -1;
				break;

			case 2:
				g_EnemyFlyBounce[i].moveSpeed.y *= -1;
				break;

			case 3:
				g_EnemyFlyBounce[i].moveSpeed.x *= -1;
				g_EnemyFlyBounce[i].moveSpeed.y *= -1;
				break;
			}
			break;
		}
	}
}



void DespawnEnemyFlyBounce(int enemyIndex)
{
	g_EnemyFlyBounce[enemyIndex].dmgTimeCnt++;

	if (g_EnemyFlyBounce[enemyIndex].dmgTimeCnt >= DESPAWN_TIME)
	{
		if (g_EnemyFlyBounce[enemyIndex].enemyColor.w > 0)
		{
			g_EnemyFlyBounce[enemyIndex].enemyColor.w -= 0.02f;
			return;
		}

		g_EnemyFlyBounce[enemyIndex].spawned = FALSE;
		g_EnemyFlyBounce[enemyIndex].alive = FALSE;
		g_EnemyFlyBounce[enemyIndex].pos = XMFLOAT3(SCREEN_CENTER_X, 1465.0f, 0.0f);	// 中心点から表示

		g_EnemyFlyBounce[enemyIndex].currentSprite = 0;
		g_EnemyFlyBounce[enemyIndex].currentAnim = FLY_BOUNCE_ANIM_MOVE;

		g_EnemyFlyBounce[enemyIndex].moveSpeed = XMFLOAT3(3.0f, -3.0f, 1.0f);		// 移動量
		g_EnemyFlyBounce[enemyIndex].moveDir = FLY_BOUNCE_DIR_RIGHT;

		g_EnemyFlyBounce[enemyIndex].HP = HP_MAX;

		g_EnemyFlyBounce[enemyIndex].damaged = FALSE;
		g_EnemyFlyBounce[enemyIndex].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyFlyBounce[enemyIndex].dmgTimeCnt = 0;

		g_EnemyFlyBounce[enemyIndex].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	}
}
