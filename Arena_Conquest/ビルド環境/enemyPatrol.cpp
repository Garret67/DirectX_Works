//=============================================================================
//
// エネミー処理 [enemyPatrol.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemyPatrol.h"
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
#define TEXTURE_WIDTH				(30 * 4)	// キャラサイズ
#define TEXTURE_HEIGHT				(16 * 4)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_ANIM_SPRITES_MAX	(4)		// 1つのアニメーションはスプライト何枚（X)
#define TEXTURE_ANIM_MAX			(3)		// アニメーションはいくつ			  （Y)
#define ANIM_WAIT					(10)		// アニメーションの切り替わるWait値

#define DAMAGE_TIME					(30)
#define KNOCKBACK_TIME				(15)
#define DESPAWN_TIME				(180)
#define HP_MAX						(20.0f)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void EnemyAnimationUpdate(void);

void DespawnEnemyPatrol(int enemyIndex);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/Slime/Slime_aprite_sheet.png",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static ENEMY_PATROL	g_EnemyPatrol[ENEMY_PATROL_MAX];		// エネミー構造体
static int g_ScenePatrolEnemies;



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemyPatrol(void)
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

	switch (GetMode())
	{
	case MODE_TUTORIAL_1:
		g_ScenePatrolEnemies = 0;

		for (int i = 0; i < ENEMY_PATROL_MAX; i++)
		{
			if (i < g_ScenePatrolEnemies)
			{
				//NO ENEMY
			}
			else
			{
				g_EnemyPatrol[i].spawned = FALSE;
				g_EnemyPatrol[i].alive   = FALSE;
			}

		}
		break;

	case MODE_TUTORIAL_2:
		g_ScenePatrolEnemies = 4;

		g_EnemyPatrol[0].pos = XMFLOAT3(1500.0f, 1465.0f, 0.0f);	// 中心点から表示
		g_EnemyPatrol[1].pos = XMFLOAT3(1800.0f, 1465.0f, 0.0f);	// 中心点から表示
		g_EnemyPatrol[2].pos = XMFLOAT3(2930.0f, 1365.0f, 0.0f);	// 中心点から表示
		g_EnemyPatrol[3].pos = XMFLOAT3(5400.0f, 1465.0f, 0.0f);	// 中心点から表示

		for (int i = 0; i < ENEMY_PATROL_MAX; i++)
		{
			if (i < g_ScenePatrolEnemies)
			{
				g_EnemyPatrol[i].spawned = TRUE;
				g_EnemyPatrol[i].alive = TRUE;
				//g_EnemyPatrol[i].pos = XMFLOAT3(200.0f + i * 600.0f, 1080.0f - 200.0f, 0.0f);	// 中心点から表示
				g_EnemyPatrol[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
				g_EnemyPatrol[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
				g_EnemyPatrol[i].w = TEXTURE_WIDTH;
				g_EnemyPatrol[i].h = TEXTURE_HEIGHT;
				g_EnemyPatrol[i].texNo = 0;

				g_EnemyPatrol[i].currentSprite = 0;
				g_EnemyPatrol[i].currentAnim   = SLIME_ANIM_MOVE;

				g_EnemyPatrol[i].moveSpeed = 2.0f;		// 移動量
				g_EnemyPatrol[i].moveDir = SLIME_DIR_RIGHT;
				g_EnemyPatrol[i].moveTimeCnt = 0;

				g_EnemyPatrol[i].HP = HP_MAX;

				g_EnemyPatrol[i].damaged = FALSE;
				g_EnemyPatrol[i].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
				g_EnemyPatrol[i].dmgTimeCnt = 0;

				g_EnemyPatrol[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				g_EnemyPatrol[i].platformIndex = 99;
				g_EnemyPatrol[i].InGround = TRUE;
			}
			else 
			{
				g_EnemyPatrol[i].spawned = FALSE;
				g_EnemyPatrol[i].alive = FALSE;
			}
			
		}
		g_EnemyPatrol[2].HP = 10.0f;

		break;

	case MODE_COLISEUM:

		for (int i = 0; i < ENEMY_PATROL_MAX; i++)
		{
			g_EnemyPatrol[i].spawned = FALSE;
			g_EnemyPatrol[i].alive = FALSE;
			g_EnemyPatrol[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 中心点から表示
			g_EnemyPatrol[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_EnemyPatrol[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_EnemyPatrol[i].w = TEXTURE_WIDTH;
			g_EnemyPatrol[i].h = TEXTURE_HEIGHT;
			g_EnemyPatrol[i].texNo = 0;

			g_EnemyPatrol[i].currentSprite = 0;
			g_EnemyPatrol[i].currentAnim = SLIME_ANIM_MOVE;

			g_EnemyPatrol[i].moveSpeed = 2.0f + (i * 0.5f);		// 移動量
			g_EnemyPatrol[i].moveDir = SLIME_DIR_RIGHT;

			g_EnemyPatrol[i].HP = HP_MAX;

			g_EnemyPatrol[i].damaged = FALSE;
			g_EnemyPatrol[i].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_EnemyPatrol[i].dmgTimeCnt = 0;

			g_EnemyPatrol[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			g_EnemyPatrol[i].platformIndex = 99;

		}

		break;
	}
	// エネミー構造体の初期化
	


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemyPatrol(void)
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
void UpdateEnemyPatrol(void)
{

	for (int i = 0; i < ENEMY_PATROL_MAX; i++)
	{
		g_EnemyPatrol[i].moveTimeCnt++;


		// 地形との当たり判定用に座標のバックアップを取っておく
		XMFLOAT3 pos_old = g_EnemyPatrol[i].pos;


		// 生きてるエネミーだけ処理をする
		if (g_EnemyPatrol[i].alive == TRUE)
		{
			// PLAYERとの当たり判定
			{
				PLAYER* player = GetPlayer();

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (player[j].alive == TRUE && player[j].invincible == FALSE)
					{
						BOOL ans = CollisionBB(g_EnemyPatrol[i].pos, g_EnemyPatrol[i].w, g_EnemyPatrol[i].h,
											   player[j].pos,		 player[j].w,		 player[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							PlayerDamaged(g_EnemyPatrol[i].pos);
						}
					}
				}
			}

			//エネミーのパトロール
			{
				if (g_EnemyPatrol[i].platformIndex != 99)
				{
					g_EnemyPatrol[i].pos.x += g_EnemyPatrol[i].moveSpeed;

					float platformLeftSide;
					float platformRightSide;

					if (g_EnemyPatrol[i].InGround == TRUE)
					{
						PLATFORM* ground = GetGround();
						platformLeftSide = ground[g_EnemyPatrol[i].platformIndex].pos.x - (ground[g_EnemyPatrol[i].platformIndex].w / 2) + g_EnemyPatrol[i].w / 2;
						platformRightSide = ground[g_EnemyPatrol[i].platformIndex].pos.x + (ground[g_EnemyPatrol[i].platformIndex].w / 2) - g_EnemyPatrol[i].w / 2;
					}
					else
					{
						PLATFORM_ANIM* platform = GetPlatforms();
						platformLeftSide = platform[g_EnemyPatrol[i].platformIndex].pos.x - (platform[g_EnemyPatrol[i].platformIndex].w / 2) + g_EnemyPatrol[i].w / 2;
						platformRightSide = platform[g_EnemyPatrol[i].platformIndex].pos.x + (platform[g_EnemyPatrol[i].platformIndex].w / 2) - g_EnemyPatrol[i].w / 2;
					}
					

					if (g_EnemyPatrol[i].pos.x < platformLeftSide ||
						g_EnemyPatrol[i].pos.x > platformRightSide)
					{
						if (g_EnemyPatrol[i].moveTimeCnt > 30)
						{
							g_EnemyPatrol[i].moveSpeed *= -1;
							g_EnemyPatrol[i].moveTimeCnt = 0;
						}
						
					}
				}
			}


			//エネミーが傷付いた
			{
				if (g_EnemyPatrol[i].damaged == TRUE)
				{
					if (g_EnemyPatrol[i].dmgTimeCnt < KNOCKBACK_TIME)	//はね返す処理は最初の15フレームだけ
					{
						g_EnemyPatrol[i].platformIndex = 99;	//エネミーが混乱させないようにplatformIndexをリセットする

						XMVECTOR epos = XMLoadFloat3(&g_EnemyPatrol[i].pos);
						XMVECTOR vec = (epos - XMLoadFloat3(&g_EnemyPatrol[i].damageOriginPos));			//自分とプレイヤーとの差分を求めて
						float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);		//その差分を使って角度を求めている
						float repealDisstance = 20.0f;										//スピードはちょっと遅くしてみる
						float repealDisstancePerFrame = repealDisstance / KNOCKBACK_TIME;

						//SMOOTH
						float repealDistNow = repealDisstancePerFrame * (KNOCKBACK_TIME - g_EnemyPatrol[i].dmgTimeCnt);



						g_EnemyPatrol[i].pos.x += cosf(angle) * repealDistNow;					//angleの方向へ移動
						g_EnemyPatrol[i].pos.y += sinf(angle) * repealDistNow;					//angleの方向へ移動
					}
					

					g_EnemyPatrol[i].dmgTimeCnt++;

					if (g_EnemyPatrol[i].dmgTimeCnt >= DAMAGE_TIME)
					{
						g_EnemyPatrol[i].damaged = FALSE;
						g_EnemyPatrol[i].dmgTimeCnt = 0;

						g_EnemyPatrol[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


					}
				}
			}
			


			//SLIMEの向き
			if (g_EnemyPatrol[i].pos.x < pos_old.x)
			{
				g_EnemyPatrol[i].moveDir = SLIME_DIR_LEFT;
			}
			else if (g_EnemyPatrol[i].pos.x > pos_old.x)
			{
				g_EnemyPatrol[i].moveDir = SLIME_DIR_RIGHT;
			}
		}
		else
		{
			if (g_EnemyPatrol[i].spawned == TRUE)
			{
				DespawnEnemyPatrol(i);
			}
		}
			

		

		//重力
		g_EnemyPatrol[i].pos.y += 5.0f;

		




		// Ground との当たり判定
		{
			PLATFORM* ground = GetGround();

			// groundSの数分当たり判定を行う
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;

				BOOL ans = CheckCollisionAndGetDirection(ground[j].pos.x, ground[j].pos.y, (ground[j].w), (ground[j].h),
					g_EnemyPatrol[i].pos.x, g_EnemyPatrol[i].pos.y, (g_EnemyPatrol[i].w), (g_EnemyPatrol[i].h));
				// 当たっている？

				switch (ans)
				{
				case FromTop:
					g_EnemyPatrol[i].platformIndex = j;
					g_EnemyPatrol[i].InGround = TRUE;
					g_EnemyPatrol[i].pos.y = ground[j].pos.y - (ground[j].h / 2) - (g_EnemyPatrol[i].h / 2);
					break;

				case FromBottom:
					g_EnemyPatrol[i].pos.y = ground[j].pos.y + (ground[j].h / 2) + (g_EnemyPatrol[i].h / 2);
					break;

				case FromLeft:
					g_EnemyPatrol[i].pos.x = ground[j].pos.x - (ground[j].w / 2) - (g_EnemyPatrol[i].w / 2);
					g_EnemyPatrol[i].moveSpeed *= -1;
					g_EnemyPatrol[i].moveTimeCnt = 0;
					break;

				case FromRight:
						g_EnemyPatrol[i].pos.x = ground[j].pos.x + (ground[j].w / 2) + (g_EnemyPatrol[i].w / 2);
						g_EnemyPatrol[i].moveSpeed *= -1;
						g_EnemyPatrol[i].moveTimeCnt = 0;
					break;

				default:
							
					break;
				}

					
			}
		}



		// PLATFORMSとの当たり判定
		{
			PLATFORM_ANIM* platform = GetPlatforms();

			// PLATFORMSの数分当たり判定を行う
			for (int j = 0; j < PLATFORM_MAX; j++)
			{
				if (platform[j].collider == FALSE) continue;

				float platformH = 50.0f;

				BOOL ans = CheckCollisionAndGetDirection(platform[j].pos.x, platform[j].pos.y, (platform[j].w), (platformH),
					g_EnemyPatrol[i].pos.x, g_EnemyPatrol[i].pos.y, (g_EnemyPatrol[i].w), (g_EnemyPatrol[i].h));
				// 当たっている？

				switch (ans)
				{
				case FromTop:
					g_EnemyPatrol[i].platformIndex = j;
					g_EnemyPatrol[i].InGround = FALSE;
					g_EnemyPatrol[i].pos.y = platform[j].pos.y - (platformH / 2) - (g_EnemyPatrol[i].h / 2);
					break;

				case FromBottom:
					g_EnemyPatrol[i].pos.y = platform[j].pos.y + (platformH / 2) + (g_EnemyPatrol[i].h / 2);
					break;

				case FromLeft:
					g_EnemyPatrol[i].pos.x = platform[j].pos.x - (platform[j].w / 2) - (g_EnemyPatrol[i].w / 2);
					g_EnemyPatrol[i].moveSpeed *= -1;
					break;

				case FromRight:
					g_EnemyPatrol[i].pos.x = platform[j].pos.x + (platform[j].w / 2) + (g_EnemyPatrol[i].w / 2);
					g_EnemyPatrol[i].moveSpeed *= -1;
					break;

				default:

					break;
				}
			}
		}


	}


	// アニメーション
	EnemyAnimationUpdate();


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemyPatrol(void)
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

	for (int i = 0; i < ENEMY_PATROL_MAX; i++)
	{
		if (g_EnemyPatrol[i].spawned == TRUE)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnemyPatrol[i].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_EnemyPatrol[i].pos.x - bg->pos.x;	// エネミーの表示位置X
			float py = g_EnemyPatrol[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
			float pw = g_EnemyPatrol[i].w;					// エネミーの表示幅
			float ph = g_EnemyPatrol[i].h;					// エネミーの表示高さ

			float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// テクスチャの幅
			float th = 1.0f / TEXTURE_ANIM_MAX;				// テクスチャの高さ
			float tx = tw * g_EnemyPatrol[i].currentSprite;	// テクスチャの左上X座標
			float ty = th * g_EnemyPatrol[i].currentAnim;	// テクスチャの左上Y座標

			if (g_EnemyPatrol[i].moveDir == SLIME_DIR_RIGHT)
			{
				tx += tw;
				tw *= -1.0f;
			}

			ty += 0.01f;



			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_EnemyPatrol[i].enemyColor);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		
	}

}


//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
ENEMY_PATROL* GetEnemyPatrol(void)
{
	return &g_EnemyPatrol[0];
}


void EnemyDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage)
{
	g_EnemyPatrol[enemyIndex].damaged = TRUE;
	g_EnemyPatrol[enemyIndex].damageOriginPos = playerPos;
	g_EnemyPatrol[enemyIndex].enemyColor = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	g_EnemyPatrol[enemyIndex].HP -= damage;
	PlaySound(SOUND_LABEL_SE_bloodyBlade);
	g_EnemyPatrol[enemyIndex].countAnim = ANIM_WAIT + 1;
	g_EnemyPatrol[enemyIndex].currentSprite = 0;

	if (g_EnemyPatrol[enemyIndex].HP <= 0)
	{
		g_EnemyPatrol[enemyIndex].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_EnemyPatrol[enemyIndex].alive = FALSE;
		RoundKill();
		//Death animation
		
	}
}

void EnemyAnimationUpdate(void)
{
	for (int i = 0; i < ENEMY_PATROL_MAX; i++)
	{
		if (g_EnemyPatrol[i].alive == FALSE)
		{
			g_EnemyPatrol[i].currentAnim = SLIME_ANIM_DIE;
		}
		else if (g_EnemyPatrol[i].damaged == TRUE)
		{
			g_EnemyPatrol[i].currentAnim = SLIME_ANIM_HIT;
		}
		else
		{
			g_EnemyPatrol[i].currentAnim = SLIME_ANIM_MOVE;
		}

		g_EnemyPatrol[i].countAnim ++;


		if (g_EnemyPatrol[i].countAnim > ANIM_WAIT)
		{
			g_EnemyPatrol[i].countAnim = 0;

			// パターンの切り替え
			if (g_EnemyPatrol[i].currentAnim == SLIME_ANIM_MOVE) //ループアニメーション
			{
				g_EnemyPatrol[i].currentSprite = (g_EnemyPatrol[i].currentSprite + 1) % TEXTURE_ANIM_SPRITES_MAX;
			}
			else //NO ループアニメーション
			{
				if (g_EnemyPatrol[i].currentSprite < TEXTURE_ANIM_SPRITES_MAX - 1)
				{
					g_EnemyPatrol[i].currentSprite++;
				}
				
			}
			
		}
	}
}


void SpawnEnemyPatrol(XMFLOAT3 spawnPos)
{
	for (int i = 0; i < ENEMY_PATROL_MAX; i++)
	{
		if (g_EnemyPatrol[i].spawned == FALSE)
		{
			g_EnemyPatrol[i].spawned	= TRUE;
			g_EnemyPatrol[i].alive		= TRUE;
			g_EnemyPatrol[i].pos		= spawnPos;

			PLAYER* player = GetPlayer();
			if (g_EnemyPatrol[i].pos.x > player[0].pos.x)
			{
				g_EnemyPatrol[i].moveSpeed *= -1;
			}
		
			break;
		}
	}
}


void DespawnEnemyPatrol(int enemyIndex)
{
	g_EnemyPatrol[enemyIndex].dmgTimeCnt++;

	if (g_EnemyPatrol[enemyIndex].dmgTimeCnt >= DESPAWN_TIME)
	{
		if(g_EnemyPatrol[enemyIndex].enemyColor.w > 0)
		{
			g_EnemyPatrol[enemyIndex].enemyColor.w -= 0.02f;
			return;
		}

		g_EnemyPatrol[enemyIndex].spawned = FALSE;
		g_EnemyPatrol[enemyIndex].alive = FALSE;
		g_EnemyPatrol[enemyIndex].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 中心点から表示

		g_EnemyPatrol[enemyIndex].currentSprite = 0;
		g_EnemyPatrol[enemyIndex].currentAnim = SLIME_ANIM_MOVE;

		g_EnemyPatrol[enemyIndex].moveSpeed = 2.0f;		// 移動量
		g_EnemyPatrol[enemyIndex].moveDir = SLIME_DIR_RIGHT;

		g_EnemyPatrol[enemyIndex].HP = HP_MAX;

		g_EnemyPatrol[enemyIndex].damaged = FALSE;
		g_EnemyPatrol[enemyIndex].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyPatrol[enemyIndex].dmgTimeCnt = 0;

		g_EnemyPatrol[enemyIndex].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_EnemyPatrol[enemyIndex].platformIndex = 99;
	}
}