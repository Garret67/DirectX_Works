//=============================================================================
//
// エネミー処理 [enemyFlyBulletOne.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemyFlyBulletOne.h"
#include "spawnController.h"
#include "bg.h"
#include "bullet.h"
#include "platforms.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FLY_ONE_TEXTURE_WIDTH		(30 * 4)	// キャラサイズ
#define FLY_ONE_TEXTURE_HEIGHT		(16 * 4)	// 

#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_ANIM_SPRITES_MAX	(8)		// 1つのアニメーションはスプライト何枚（X)
#define TEXTURE_ANIM_MAX			(4)		// アニメーションはいくつ			  （Y)
#define ANIM_WAIT					(5)		// アニメーションの切り替わるWait値

#define STATE_HIT_TIME				(30)
#define NEXT_ATTACK_TIME			(180)
#define ATTACK_DELAY_TIME			(30)
#define KNOCKBACK_TIME				(15)
#define DESPAWN_TIME				(180)
#define HP_MAX						(50.0f)
#define FLY_ONE_BULLET_SPEED		(5.0f)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void EnemyFlyOneAnimationUpdate(void);

void DespawnEnemyFlyOne(int enemyIndex);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/Flying eye/FlyingEye_Anim.png",
};


static BOOL		g_Load = FALSE;							// 初期化を行ったかのフラグ

static ENEMY_FLY_ONE	g_EnemyFlyOne[ENEMY_FLY_ONE_MAX];	// エネミー構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemyFlyOne(void)
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
	for (int i = 0; i < ENEMY_FLY_ONE_MAX; i++)
	{
		g_EnemyFlyOne[i].spawned = FALSE;
		g_EnemyFlyOne[i].alive = FALSE;
		g_EnemyFlyOne[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 中心点から表示
		g_EnemyFlyOne[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyFlyOne[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_EnemyFlyOne[i].w = FLY_ONE_TEXTURE_WIDTH;
		g_EnemyFlyOne[i].h = FLY_ONE_TEXTURE_HEIGHT;
		g_EnemyFlyOne[i].texNo = 0;

		g_EnemyFlyOne[i].currentSprite = 0;
		g_EnemyFlyOne[i].currentSpriteMax = TEXTURE_ANIM_SPRITES_MAX;
		g_EnemyFlyOne[i].currentAnim = FLY_ONE_ANIM_MOVE;

		g_EnemyFlyOne[i].moveSpeed = 3.0f;		// 移動量
		g_EnemyFlyOne[i].moveDir = FLY_ONE_DIR_RIGHT;

		g_EnemyFlyOne[i].WayPointPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_EnemyFlyOne[i].HP = HP_MAX;

		g_EnemyFlyOne[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_EnemyFlyOne[i].state = FLY_ONE_STATE_CONTROLLER;
		g_EnemyFlyOne[i].stateTime = 0;
		g_EnemyFlyOne[i].stateTimeCnt = 0;

		g_EnemyFlyOne[i].attackTimeCnt = 0;

		g_EnemyFlyOne[i].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemyFlyOne(void)
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
void UpdateEnemyFlyOne(void)
{

	for (int i = 0; i < ENEMY_FLY_ONE_MAX; i++)
	{
		if (g_EnemyFlyOne[i].spawned == FALSE) continue;

		// 地形との当たり判定用に座標のバックアップを取っておく
		XMFLOAT3 pos_old = g_EnemyFlyOne[i].pos;


		// 生きてるエネミーだけ処理をする
		if (g_EnemyFlyOne[i].alive == TRUE)
		{
			PLAYER* player = GetPlayer();

			//FLY_ONEエネミーのパトロール
			{
				float playerDistance = (g_EnemyFlyOne[i].pos.x - player[0].pos.x);

				//プレイヤーとの距離
				//{
				//	if (fabsf(playerDistance) < 300.0f)
				//	{
				//		//MOVE
				//		g_EnemyFlyOne[i].state = FLY_ONE_STATE_MOVE;
				//	}
				//}

				if (g_EnemyFlyOne[i].state != FLY_ONE_STATE_ATTACK)
				{
					g_EnemyFlyOne[i].attackTimeCnt++;

					if (g_EnemyFlyOne[i].attackTimeCnt >= NEXT_ATTACK_TIME)
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_ATTACK;
						g_EnemyFlyOne[i].stateTime = 40;
						g_EnemyFlyOne[i].stateTimeCnt = 0;
						g_EnemyFlyOne[i].currentSprite = 0;
						g_EnemyFlyOne[i].currentSpriteMax = 2;

						g_EnemyFlyOne[i].attackTimeCnt = 0;
					}
				}
				


				//STATE CONTROLLER
				if (g_EnemyFlyOne[i].state == FLY_ONE_STATE_CONTROLLER)
				{
					//Decide next movePoint
					int newState = rand() % 3;

					if (newState == 2)
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_MOVE;
						g_EnemyFlyOne[i].stateTime = 300;

						g_EnemyFlyOne[i].currentSpriteMax = 8;

						BOOL IncorrectWayPoint = TRUE;

						while (IncorrectWayPoint)
						{
							BG* bg = GetBG();

							float PosXMax = bg->w - g_EnemyFlyOne[i].w;
							float PosYMax = bg->h - 300;

							g_EnemyFlyOne[i].WayPointPos.x = (rand() % (int)PosXMax) + (g_EnemyFlyOne[i].w / 2);
							g_EnemyFlyOne[i].WayPointPos.y = (rand() % (int)PosYMax) + (g_EnemyFlyOne[i].h / 2);


							//platformの当たり判定
							PLATFORM* platform = GetGround();
							for (int j = 0; j < GROUND_MAX; j++)
							{
								BOOL ans = CollisionBB(g_EnemyFlyOne[i].WayPointPos, g_EnemyFlyOne[i].w, g_EnemyFlyOne[i].h,
									platform[j].pos, platform[j].w, platform[j].h);

								// 当たっている？
								if (ans == FALSE)
								{
									// 当たった時の処理
									IncorrectWayPoint = FALSE;
								}
								
							}
						}
						
					}
					else
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_STAY;
						g_EnemyFlyOne[i].stateTime = 20 + rand() % 60;
						g_EnemyFlyOne[i].currentSpriteMax = 8;
					}
				}


				//状態の処理
				switch (g_EnemyFlyOne[i].state)
				{
				case FLY_ONE_STATE_STAY:
					g_EnemyFlyOne[i].currentAnim = FLY_ONE_ANIM_MOVE;

					if (g_EnemyFlyOne[i].stateTimeCnt < g_EnemyFlyOne[i].stateTime)
					{
						g_EnemyFlyOne[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_CONTROLLER;
						g_EnemyFlyOne[i].stateTimeCnt = 0;
					}
					break;

				case FLY_ONE_STATE_MOVE:
				{
					g_EnemyFlyOne[i].currentAnim = FLY_ONE_ANIM_MOVE;



					XMVECTOR epos = XMLoadFloat3(&g_EnemyFlyOne[i].pos);
					XMVECTOR vec =  XMLoadFloat3(&g_EnemyFlyOne[i].WayPointPos) - epos;	//自分とプレイヤーとの差分を求めて

					float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);		//その差分を使って角度を求めている

					g_EnemyFlyOne[i].pos.x += cosf(angle) * g_EnemyFlyOne[i].moveSpeed;				//angleの方向へ移動
					g_EnemyFlyOne[i].pos.y += sinf(angle) * g_EnemyFlyOne[i].moveSpeed;				//angleの方向へ移動


					XMVECTOR newVec = XMLoadFloat3(&g_EnemyFlyOne[i].WayPointPos) - XMLoadFloat3(&g_EnemyFlyOne[i].pos);

					float vecDist = 0.0f;
					XMStoreFloat(&vecDist, XMVector3LengthSq(newVec));

					if (vecDist < 50.0f)
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_CONTROLLER;
						g_EnemyFlyOne[i].stateTimeCnt = 0;
					}
					else if (g_EnemyFlyOne[i].stateTimeCnt < g_EnemyFlyOne[i].stateTime)
					{
						g_EnemyFlyOne[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_CONTROLLER;
						g_EnemyFlyOne[i].stateTimeCnt = 0;
					}

				}
					break;

				case FLY_ONE_STATE_ATTACK: 
				{
					g_EnemyFlyOne[i].currentAnim = FLY_ONE_ANIM_ATTACK;
					int attackStateCnt = g_EnemyFlyOne[i].stateTimeCnt - ATTACK_DELAY_TIME;

					if (attackStateCnt == 0)
					{
						XMVECTOR epos = XMLoadFloat3(&g_EnemyFlyOne[i].pos);
						XMVECTOR vec = XMLoadFloat3(&player->pos) - epos;		//自分とプレイヤーとの差分を求めて
						float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);						//その差分を使って角度を求めている

						XMFLOAT3 moveDir = XMFLOAT3(0.0f, 0.0f, 0.0f);
						moveDir.x = cosf(angle) * FLY_ONE_BULLET_SPEED;					//angleの方向へ移動
						moveDir.y = sinf(angle) * FLY_ONE_BULLET_SPEED;					//angleの方向へ移動

						SetBulletEnemy(g_EnemyFlyOne[i].pos, angle - (XM_PIDIV2), moveDir);
					}

					if (attackStateCnt < g_EnemyFlyOne[i].stateTime)
					{
						g_EnemyFlyOne[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_CONTROLLER;
						g_EnemyFlyOne[i].stateTimeCnt = 0;
					}
				}
					break;

				case FLY_ONE_STATE_HIT:
					g_EnemyFlyOne[i].currentAnim = FLY_ONE_ANIM_HIT;

					if (g_EnemyFlyOne[i].stateTimeCnt < KNOCKBACK_TIME) //はね返す処理は最初の15フレームだけ
					{
						XMVECTOR epos = XMLoadFloat3(&g_EnemyFlyOne[i].pos);
						XMVECTOR vec = (epos - XMLoadFloat3(&g_EnemyFlyOne[i].damageOriginPos));			//自分とプレイヤーとの差分を求めて
						float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);		//その差分を使って角度を求めている
						float repealDisstance = 10.0f;										//スピードはちょっと遅くしてみる
						float repealDisstancePerFrame = repealDisstance / KNOCKBACK_TIME;

						//SMOOTH
						float repealDistNow = repealDisstancePerFrame * (KNOCKBACK_TIME - g_EnemyFlyOne[i].stateTimeCnt);

						g_EnemyFlyOne[i].pos.x += cosf(angle) * repealDistNow;					//angleの方向へ移動
						g_EnemyFlyOne[i].pos.y += sinf(angle) * repealDistNow;					//angleの方向へ移動
					}
					

					g_EnemyFlyOne[i].stateTimeCnt++;

					if (g_EnemyFlyOne[i].stateTimeCnt >= g_EnemyFlyOne[i].stateTime)
					{
						g_EnemyFlyOne[i].state = FLY_ONE_STATE_CONTROLLER;
						g_EnemyFlyOne[i].stateTimeCnt = 0;

						//g_EnemyFlyOne[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
					}

					break;
				}

			}

			

			//FLY_ONEのプレイヤー向き
			{
				if (g_EnemyFlyOne[i].pos.x < player[0].pos.x)
				{
					g_EnemyFlyOne[i].moveDir = FLY_ONE_DIR_RIGHT;
				}
				else
				{
					g_EnemyFlyOne[i].moveDir = FLY_ONE_DIR_LEFT;
				}
			}
			//FLY_ONEの動きの向き
			if (g_EnemyFlyOne[i].pos.x < pos_old.x)
			{
				g_EnemyFlyOne[i].moveDir = FLY_ONE_DIR_LEFT;
			}
			else if (g_EnemyFlyOne[i].pos.x > pos_old.x)
			{
				g_EnemyFlyOne[i].moveDir = FLY_ONE_DIR_RIGHT;
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
						BOOL ans = CollisionBB(g_EnemyFlyOne[i].pos, g_EnemyFlyOne[i].w , g_EnemyFlyOne[i].h,
							player[j].pos, player[j].w, player[j].h);

						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							PlayerDamaged(g_EnemyFlyOne[i].pos);
						}
					}
				}
			}
		}
		else
		{
			//重力
			g_EnemyFlyOne[i].pos.y += 5.0f;

			DespawnEnemyFlyOne(i);	//Despawn
		}



		// Ground との当たり判定
		{
			PLATFORM* ground = GetGround();

			// groundSの数分当たり判定を行う
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;


				BOOL ans = CheckCollisionAndGetDirection(ground[j].pos.x, ground[j].pos.y, (ground[j].w), (ground[j].h),
					g_EnemyFlyOne[i].pos.x, g_EnemyFlyOne[i].pos.y, (g_EnemyFlyOne[i].w), (g_EnemyFlyOne[i].h));
				// 当たっている？

				switch (ans)
				{
				case FromTop:
					g_EnemyFlyOne[i].pos.y = ground[j].pos.y - (ground[j].h / 2) - (g_EnemyFlyOne[i].h / 2);
					break;

				case FromBottom:
					g_EnemyFlyOne[i].pos.y = ground[j].pos.y + (ground[j].h / 2) + (g_EnemyFlyOne[i].h / 2);
					break;

				case FromLeft:
					g_EnemyFlyOne[i].pos.x = ground[j].pos.x - (ground[j].w / 2) - (g_EnemyFlyOne[i].w / 2);
					break;

				case FromRight:
					g_EnemyFlyOne[i].pos.x = ground[j].pos.x + (ground[j].w / 2) + (g_EnemyFlyOne[i].w / 2);
					break;

				default:

					break;
				}


			}
		}


		// PLATFORMSとの当たり判定
		{
			PLATFORM_ANIM* platform = GetPlatforms();

			for (int j = 0; j < PLATFORM_MAX; j++)
			{
				if (platform[j].collider == FALSE) continue;

				float platformH = 50.0f;

				BOOL ans = CheckCollisionAndGetDirection(platform[j].pos.x, platform[j].pos.y, (platform[j].w), (platformH),
					g_EnemyFlyOne[i].pos.x, g_EnemyFlyOne[i].pos.y, (g_EnemyFlyOne[i].w), (g_EnemyFlyOne[i].h));
				// 当たっている？

				switch (ans)
				{
				case FromTop:
					g_EnemyFlyOne[i].pos.y = platform[j].pos.y - (platformH / 2) - (g_EnemyFlyOne[i].h / 2);
					break;

				case FromBottom:
					g_EnemyFlyOne[i].pos.y = platform[j].pos.y + (platformH / 2) + (g_EnemyFlyOne[i].h / 2);
					break;

				case FromLeft:
					g_EnemyFlyOne[i].pos.x = platform[j].pos.x - (platform[j].w / 2) - (g_EnemyFlyOne[i].w / 2);
					break;

				case FromRight:
					g_EnemyFlyOne[i].pos.x = platform[j].pos.x + (platform[j].w / 2) + (g_EnemyFlyOne[i].w / 2);
					break;

				default:

					break;
				}


			}
		}


		//画面端の当たり判定
		{
			BG* bg = GetBG();
			if (g_EnemyFlyOne[i].pos.x < (g_EnemyFlyOne[i].w / 2))		// 自分の大きさを考慮して画面外か判定している
			{
				g_EnemyFlyOne[i].pos.x = g_EnemyFlyOne[i].w / 2;
			}
			if (g_EnemyFlyOne[i].pos.x > (bg->w - g_EnemyFlyOne[i].w / 2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_EnemyFlyOne[i].pos.x = bg->w - (g_EnemyFlyOne[i].w / 2);
			}

			if (g_EnemyFlyOne[i].pos.y < (g_EnemyFlyOne[i].h / 2))		// 自分の大きさを考慮して画面外か判定している
			{
				g_EnemyFlyOne[i].pos.y = (g_EnemyFlyOne[i].h / 2);
			}
			if (g_EnemyFlyOne[i].pos.y > (bg->h - g_EnemyFlyOne[i].h / 2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_EnemyFlyOne[i].pos.y = bg->h - (g_EnemyFlyOne[i].h / 2);
			}
		}
		

	}


	// アニメーション
	EnemyFlyOneAnimationUpdate();


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemyFlyOne(void)
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

	for (int i = 0; i < ENEMY_FLY_ONE_MAX; i++)
	{
		if (g_EnemyFlyOne[i].spawned == FALSE) continue;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnemyFlyOne[i].texNo]);

		//エネミーの位置やテクスチャー座標を反映
		float px = g_EnemyFlyOne[i].pos.x - bg->pos.x;	// エネミーの表示位置X
		float py = g_EnemyFlyOne[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
		float pw = g_EnemyFlyOne[i].w;					// エネミーの表示幅
		float ph = g_EnemyFlyOne[i].h;					// エネミーの表示高さ

		float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// テクスチャの幅
		float th = 1.0f / FLY_ONE_ANIM_MAX;				// テクスチャの高さ
		float tx = tw * g_EnemyFlyOne[i].currentSprite;	// テクスチャの左上X座標
		float ty = th * g_EnemyFlyOne[i].currentAnim;	// テクスチャの左上Y座標

		if (g_EnemyFlyOne[i].moveDir == FLY_ONE_DIR_LEFT)
		{
			tx += tw;
			tw *= -1.0f;
		}

		ty += 0.005f;
		


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_EnemyFlyOne[i].enemyColor);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}


//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
ENEMY_FLY_ONE* GetEnemyFlyOne(void)
{
	return &g_EnemyFlyOne[0];
}


void EnemyFlyOneDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage)
{
	g_EnemyFlyOne[enemyIndex].state = FLY_ONE_STATE_HIT;
	g_EnemyFlyOne[enemyIndex].stateTime = STATE_HIT_TIME;
	g_EnemyFlyOne[enemyIndex].stateTimeCnt = 0;
	g_EnemyFlyOne[enemyIndex].damageOriginPos = playerPos;

	g_EnemyFlyOne[enemyIndex].HP -= damage;

	g_EnemyFlyOne[enemyIndex].countAnim = 0;
	g_EnemyFlyOne[enemyIndex].currentSprite = 0;
	g_EnemyFlyOne[enemyIndex].currentSpriteMax = 4;

	PlaySound(SOUND_LABEL_SE_bloodyBlade);

	if (g_EnemyFlyOne[enemyIndex].HP <= 0)
	{
		g_EnemyFlyOne[enemyIndex].alive = FALSE;
		RoundKill();
		//Death animation
		g_EnemyFlyOne[enemyIndex].currentAnim = FLY_ONE_ANIM_DEATH;
	}
}

void EnemyFlyOneAnimationUpdate(void)
{
	for (int i = 0; i < ENEMY_FLY_ONE_MAX; i++)
	{
		if (g_EnemyFlyOne[i].spawned == FALSE) continue;

		if (g_EnemyFlyOne[i].currentAnim == FLY_ONE_ANIM_ATTACK)
		{
			int attackStateCnt = g_EnemyFlyOne[i].stateTimeCnt - ATTACK_DELAY_TIME;

			if (attackStateCnt >= 0)
			{
				g_EnemyFlyOne[i].countAnim++;
			}
		}
		else
		{
			g_EnemyFlyOne[i].countAnim++;
		}


		if (g_EnemyFlyOne[i].countAnim >= ANIM_WAIT)
		{
			g_EnemyFlyOne[i].countAnim = 0;

			// パターンの切り替え
			if (g_EnemyFlyOne[i].currentAnim == FLY_ONE_ANIM_MOVE) //ループアニメーション
			{
				g_EnemyFlyOne[i].currentSprite = (g_EnemyFlyOne[i].currentSprite + 1) % g_EnemyFlyOne[i].currentSpriteMax;
			}
			else //NO ループアニメーション
			{
				if (g_EnemyFlyOne[i].currentSprite < g_EnemyFlyOne[i].currentSpriteMax - 1)
				{
					g_EnemyFlyOne[i].currentSprite++;
				}
				
			}
			
		}

		
		
		
	}
}


void SpawnEnemyFlyOne(XMFLOAT3 spawnPos)
{
	for (int i = 0; i < ENEMY_FLY_ONE_MAX; i++)
	{
		if (g_EnemyFlyOne[i].spawned == FALSE)
		{
			g_EnemyFlyOne[i].spawned = TRUE;
			g_EnemyFlyOne[i].alive = TRUE;
			g_EnemyFlyOne[i].pos = spawnPos;
			break;
		}
	}
}



void DespawnEnemyFlyOne(int enemyIndex)
{
	g_EnemyFlyOne[enemyIndex].stateTimeCnt++;

	if (g_EnemyFlyOne[enemyIndex].stateTimeCnt >= DESPAWN_TIME)
	{
		if (g_EnemyFlyOne[enemyIndex].enemyColor.w > 0)
		{
			g_EnemyFlyOne[enemyIndex].enemyColor.w -= 0.02f;
			return;
		}

		
		g_EnemyFlyOne[enemyIndex].spawned = FALSE;
		g_EnemyFlyOne[enemyIndex].alive = FALSE;
		g_EnemyFlyOne[enemyIndex].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);		// 中心点から表示
		g_EnemyFlyOne[enemyIndex].w = FLY_ONE_TEXTURE_WIDTH;
		g_EnemyFlyOne[enemyIndex].h = FLY_ONE_TEXTURE_HEIGHT;

		g_EnemyFlyOne[enemyIndex].currentSprite = 0;
		g_EnemyFlyOne[enemyIndex].currentAnim = FLY_ONE_ANIM_MOVE;

		g_EnemyFlyOne[enemyIndex].moveSpeed = 2.0f;		// 移動量
		g_EnemyFlyOne[enemyIndex].moveDir = FLY_ONE_DIR_RIGHT;

		g_EnemyFlyOne[enemyIndex].HP = HP_MAX;

		g_EnemyFlyOne[enemyIndex].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_EnemyFlyOne[enemyIndex].state = FLY_ONE_STATE_CONTROLLER;
		g_EnemyFlyOne[enemyIndex].stateTime = 0;
		g_EnemyFlyOne[enemyIndex].stateTimeCnt = 0;

		g_EnemyFlyOne[enemyIndex].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}
}
