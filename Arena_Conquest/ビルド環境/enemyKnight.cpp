//=============================================================================
//
// エネミー処理 [enemyKnight.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemyKnight.h"
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
#define TEXTURE_MAX					(1)			// テクスチャの数

#define TEXTURE_ANIM_SPRITES_MAX	(4)			// 1つのアニメーションはスプライト何枚（X)
#define ANIM_WAIT					(10)		// アニメーションの切り替わるWait値

#define STATE_HIT_TIME				(30)		// 無敵状態時間（ダメージを受けた後）
#define STATE_ATTACK_TIME			(100)		// 攻撃の時間
#define ATTACK_DELAY_TIME			(30)		// 攻撃するまでの時間（構え時間）
#define KNOCKBACK_TIME				(15)		// ノックバック時間
#define DESPAWN_TIME				(180)		// 消えるまでの時間

#define HP_MAX						(50.0f)		// 最大のHP
#define MOVE_SPEED					(2.0f)		// 移動速度

#define FAR_DISTANCE				(500.0f)	// 遠い距離
#define NEAR_DISTANCE				(180.0f)	// 短い距離


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void EnemyKnightAnimationUpdate(void);		//アニメーションの更新処理

void DespawnEnemyKnight(int enemyIndex);	//Knightを未使用にする

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/Skeleton/Skeleton_Anim.png",
};


static BOOL		g_Load = FALSE;							// 初期化を行ったかのフラグ
static ENEMY_KNIGHT	g_EnemyKnight[ENEMY_KNIGHT_MAX];	// エネミー構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemyKnight(void)
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
	for (int i = 0; i < ENEMY_KNIGHT_MAX; i++)
	{
		g_EnemyKnight[i].spawned = FALSE;
		g_EnemyKnight[i].alive = FALSE;
		g_EnemyKnight[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 中心点から表示
		g_EnemyKnight[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyKnight[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_EnemyKnight[i].w = KNIGHT_TEXTURE_WIDTH;
		g_EnemyKnight[i].h = KNIGHT_TEXTURE_HEIGHT;
		g_EnemyKnight[i].texNo = 0;

		g_EnemyKnight[i].currentSprite = 0;
		g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_IDLE;

		g_EnemyKnight[i].moveSpeed = MOVE_SPEED;		// 移動量
		g_EnemyKnight[i].moveDir = KNIGHT_DIR_RIGHT;

		g_EnemyKnight[i].HP = HP_MAX;

		g_EnemyKnight[i].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
		g_EnemyKnight[i].stateTime = 0;
		g_EnemyKnight[i].stateTimeCnt = 0;


		g_EnemyKnight[i].playerDistance = KNIGHT_DISTANCE_FAR;
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemyKnight(void)
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
void UpdateEnemyKnight(void)
{

	for (int i = 0; i < ENEMY_KNIGHT_MAX; i++)
	{
		if (g_EnemyKnight[i].spawned == FALSE) continue;

		// 地形との当たり判定用に座標のバックアップを取っておく
		XMFLOAT3 pos_old = g_EnemyKnight[i].pos;


		// 生きてるエネミーだけ処理をする
		if (g_EnemyKnight[i].alive == TRUE)
		{
			

			//騎士エネミーのパトロール
			{
				
				PLAYER* player = GetPlayer();

				//騎士の向き
				{
					if (g_EnemyKnight[i].pos.x < player[0].pos.x)
					{
						g_EnemyKnight[i].moveDir = KNIGHT_DIR_RIGHT;
						g_EnemyKnight[i].moveSpeed = 1.0f;
					}
					else
					{
						g_EnemyKnight[i].moveDir = KNIGHT_DIR_LEFT;
						g_EnemyKnight[i].moveSpeed = -1.0f;
					}
				}



				int lastPlayerDistance = g_EnemyKnight[i].playerDistance;
				float playerDistance = (g_EnemyKnight[i].pos.x - player[0].pos.x);

				//プレイヤーとの距離
				{
					if (fabsf(playerDistance) > FAR_DISTANCE)
					{
						g_EnemyKnight[i].playerDistance = KNIGHT_DISTANCE_FAR;
					}
					else if (fabsf(playerDistance) < FAR_DISTANCE &&
							 fabsf(playerDistance) > NEAR_DISTANCE)
					{
						g_EnemyKnight[i].playerDistance = KNIGHT_DISTANCE_MIDWAY;
					}
					else if (fabsf(playerDistance) < NEAR_DISTANCE)
					{
						float knightTop = g_EnemyKnight[i].pos.y - (g_EnemyKnight[i].h / 2);

						if (knightTop > player[0].pos.y)
						{
							g_EnemyKnight[i].playerDistance = KNIGHT_DISTANCE_NEAR_UP;
						}
						else
						{
							g_EnemyKnight[i].playerDistance = KNIGHT_DISTANCE_NEAR_FRONT;
						}
					
					}
				}


				//プレイヤーとの距離によって、状態を変える
				{
					if (g_EnemyKnight[i].state == KNIGHT_STATE_CONTROLLER || lastPlayerDistance != g_EnemyKnight[i].playerDistance)
					{
						if(g_EnemyKnight[i].state != KNIGHT_STATE_ATTACK)
						{
							switch (g_EnemyKnight[i].playerDistance)
							{
							case KNIGHT_DISTANCE_FAR:
								g_EnemyKnight[i].state = KNIGHT_STATE_MOVE_FORWARD;
								break;

							case KNIGHT_DISTANCE_MIDWAY:
							{
								int newState = rand() % 5;

								//ランダムで動きを決める
								switch (newState)
								{
								case 0:
								case 1:
								case 2:
									g_EnemyKnight[i].state = KNIGHT_STATE_MOVE_FORWARD;		//間に進む
									break;

								case 3:
									g_EnemyKnight[i].state = KNIGHT_STATE_MOVE_BACKWARD;	//後ろに動く
									break;

								case 4:
									g_EnemyKnight[i].state = KNIGHT_STATE_MOVE_STAY;		//動かない
									break;
								}


								g_EnemyKnight[i].stateTime = 20 + rand() % 60;
							}
							break;

							case KNIGHT_DISTANCE_NEAR_UP:
							case KNIGHT_DISTANCE_NEAR_FRONT:

								int newState = rand() % 3;

								//ランダムで動きを決める
								switch (newState)
								{
								case 0:
								case 1:
									//防御
									if (g_EnemyKnight[i].playerDistance == KNIGHT_DISTANCE_NEAR_FRONT)
									{
										g_EnemyKnight[i].state = KNIGHT_STATE_DEFENCE_FRONT;
									}
									else
									{
										g_EnemyKnight[i].state = KNIGHT_STATE_DEFENCE_UP;
									}
								
									g_EnemyKnight[i].stateTime = 20 + rand() % 150;
									break;

								case 2:
									//攻撃
									g_EnemyKnight[i].state = KNIGHT_STATE_ATTACK;

									g_EnemyKnight[i].stateTime = STATE_ATTACK_TIME;
									g_EnemyKnight[i].stateTimeCnt = 0;
								
									g_EnemyKnight[i].countAnim = 0;
									g_EnemyKnight[i].currentSprite = 0;
									break;
								}
							
							
							
								break;
							}
						}
					}
				}


				//状態の処理
				switch (g_EnemyKnight[i].state)
				{
					//動かない
				case KNIGHT_STATE_MOVE_STAY:
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_IDLE;

					if (g_EnemyKnight[i].stateTimeCnt < g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}
					break;

					//前に進む
				case KNIGHT_STATE_MOVE_FORWARD:
					g_EnemyKnight[i].pos.x += g_EnemyKnight[i].moveSpeed;
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_MOVE;

					if (g_EnemyKnight[i].stateTimeCnt < g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}
					break;

					//後ろに動く
				case KNIGHT_STATE_MOVE_BACKWARD:
					g_EnemyKnight[i].pos.x -= g_EnemyKnight[i].moveSpeed;
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_MOVE;

					if (g_EnemyKnight[i].stateTimeCnt < g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}
					break;

					//前の防御
				case KNIGHT_STATE_DEFENCE_FRONT:
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_DEFENCE_FRONT;

					if (g_EnemyKnight[i].stateTimeCnt < g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}
					break;

					//上の防御
				case KNIGHT_STATE_DEFENCE_UP:
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_DEFENCE_UP;

					if (g_EnemyKnight[i].stateTimeCnt < g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}
					break;

					//攻撃
				case KNIGHT_STATE_ATTACK:
				{
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_ATTACK;
					int attackStateCnt = g_EnemyKnight[i].stateTimeCnt - ATTACK_DELAY_TIME;

					if (attackStateCnt < g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].stateTimeCnt++;
					}
					else
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}
				}
					break;

					//ダメージを受ける
				case KNIGHT_STATE_HIT:
					g_EnemyKnight[i].currentAnim = KNIGHT_ANIM_HIT;

					if (g_EnemyKnight[i].stateTimeCnt < KNOCKBACK_TIME) //はね返す処理は最初の15フレームだけ
					{
						float repealDisstance = 10.0f;
						if (g_EnemyKnight[i].pos.x < player[0].pos.x)
						{
							repealDisstance = -10.0f;
						}

						float repealDisstancePerFrame = repealDisstance / KNOCKBACK_TIME;
						float repealDistNow = repealDisstancePerFrame * (KNOCKBACK_TIME - g_EnemyKnight[i].stateTimeCnt);//SMOOTH
						g_EnemyKnight[i].pos.x += repealDistNow;
					}
					

					g_EnemyKnight[i].stateTimeCnt++;

					if (g_EnemyKnight[i].stateTimeCnt >= g_EnemyKnight[i].stateTime)
					{
						g_EnemyKnight[i].state = KNIGHT_STATE_CONTROLLER;
						g_EnemyKnight[i].stateTimeCnt = 0;
					}

					break;
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
						float enemyCollW = KNIGHT_COLLIDER_WIDTH;
						if (g_EnemyKnight[i].state == KNIGHT_STATE_ATTACK && g_EnemyKnight[i].currentSprite == 2)
						{
							enemyCollW = g_EnemyKnight[i].w;
						}


						BOOL ans = CollisionBB(g_EnemyKnight[i].pos, enemyCollW, g_EnemyKnight[i].h,
							player[j].pos, player[j].w, player[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							PlayerDamaged(g_EnemyKnight[i].pos);
						}
					}
				}
			}
		}
		else
		{
			DespawnEnemyKnight(i);	//Despawn
		}
	
		//重力
		g_EnemyKnight[i].pos.y += 5.0f;



		// Ground との当たり判定
		{
			PLATFORM* ground = GetGround();

			// groundSの数分当たり判定を行う
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;


				BOOL ans = CheckCollisionAndGetDirection(ground[j].pos.x, ground[j].pos.y, (ground[j].w), (ground[j].h),
					g_EnemyKnight[i].pos.x, g_EnemyKnight[i].pos.y, (KNIGHT_COLLIDER_WIDTH), (g_EnemyKnight[i].h));
				// 当たっている？

				switch (ans)
				{
				case FromTop:
					g_EnemyKnight[i].pos.y = ground[j].pos.y - (ground[j].h / 2) - (g_EnemyKnight[i].h / 2);
					break;

				case FromBottom:
					g_EnemyKnight[i].pos.y = ground[j].pos.y + (ground[j].h / 2) + (g_EnemyKnight[i].h / 2);
					break;

				case FromLeft:
					g_EnemyKnight[i].pos.x = ground[j].pos.x - (ground[j].w / 2) - (KNIGHT_COLLIDER_WIDTH / 2);
					break;

				case FromRight:
					g_EnemyKnight[i].pos.x = ground[j].pos.x + (ground[j].w / 2) + (KNIGHT_COLLIDER_WIDTH / 2);
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
					g_EnemyKnight[i].pos.x, g_EnemyKnight[i].pos.y, (KNIGHT_COLLIDER_WIDTH), (g_EnemyKnight[i].h));
				// 当たっている？

				switch (ans)
				{
				case FromTop:
					g_EnemyKnight[i].pos.y = platform[j].pos.y - (platformH / 2) - (g_EnemyKnight[i].h / 2);
					break;

				case FromBottom:
					g_EnemyKnight[i].pos.y = platform[j].pos.y + (platformH / 2) + (g_EnemyKnight[i].h / 2);
					break;

				case FromLeft:
					g_EnemyKnight[i].pos.x = platform[j].pos.x - (platform[j].w / 2) - (KNIGHT_COLLIDER_WIDTH / 2);
					break;

				case FromRight:
					g_EnemyKnight[i].pos.x = platform[j].pos.x + (platform[j].w / 2) + (KNIGHT_COLLIDER_WIDTH / 2);
					break;

				default:

					break;
				}
			}
		}
	}


	// アニメーション
	EnemyKnightAnimationUpdate();


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemyKnight(void)
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

	for (int i = 0; i < ENEMY_KNIGHT_MAX; i++)
	{
		if (g_EnemyKnight[i].spawned == FALSE) continue;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnemyKnight[i].texNo]);

		//エネミーの位置やテクスチャー座標を反映
		float px = g_EnemyKnight[i].pos.x - bg->pos.x;	// エネミーの表示位置X
		float py = g_EnemyKnight[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
		float pw = g_EnemyKnight[i].w;					// エネミーの表示幅
		float ph = g_EnemyKnight[i].h;					// エネミーの表示高さ

		float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// テクスチャの幅
		float th = 1.0f / KNIGHT_ANIM_MAX;				// テクスチャの高さ
		float tx = tw * g_EnemyKnight[i].currentSprite;	// テクスチャの左上X座標
		float ty = th * g_EnemyKnight[i].currentAnim;	// テクスチャの左上Y座標

		if (g_EnemyKnight[i].moveDir == KNIGHT_DIR_LEFT)
		{
			tx += tw;
			tw *= -1.0f;
		}

		ty += 0.005f;
		


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_EnemyKnight[i].enemyColor);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}


//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
ENEMY_KNIGHT* GetEnemyKnight(void)
{
	return &g_EnemyKnight[0];
}

//ダメージを受ける
void EnemyKnightDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage)
{
	g_EnemyKnight[enemyIndex].state = KNIGHT_STATE_HIT;
	g_EnemyKnight[enemyIndex].stateTime = STATE_HIT_TIME;
	g_EnemyKnight[enemyIndex].stateTimeCnt = 0;

	g_EnemyKnight[enemyIndex].HP -= damage;

	PlaySound(SOUND_LABEL_SE_bloodyBlade);

	g_EnemyKnight[enemyIndex].countAnim = 0;
	g_EnemyKnight[enemyIndex].currentSprite = 0;

	if (g_EnemyKnight[enemyIndex].HP <= 0)
	{
		g_EnemyKnight[enemyIndex].alive = FALSE;
		g_EnemyKnight[enemyIndex].currentAnim = KNIGHT_ANIM_DEATH;//Death animation
		RoundKill();
	}
}

//アニメーションの更新処理
void EnemyKnightAnimationUpdate(void)
{
	for (int i = 0; i < ENEMY_KNIGHT_MAX; i++)
	{
		if (g_EnemyKnight[i].spawned == FALSE) continue;

		if (g_EnemyKnight[i].countAnim > ANIM_WAIT)
		{
			g_EnemyKnight[i].countAnim = 0;

			// パターンの切り替え
			if (g_EnemyKnight[i].currentAnim == KNIGHT_ANIM_MOVE || 
				g_EnemyKnight[i].currentAnim == KNIGHT_ANIM_IDLE) //ループアニメーション
			{
				if (g_EnemyKnight[i].state == KNIGHT_STATE_MOVE_FORWARD ||
					g_EnemyKnight[i].state == KNIGHT_STATE_MOVE_STAY)
				{
					g_EnemyKnight[i].currentSprite = (g_EnemyKnight[i].currentSprite + 1) % TEXTURE_ANIM_SPRITES_MAX;
				}
				else if (g_EnemyKnight[i].state == KNIGHT_STATE_MOVE_BACKWARD)
				{
					g_EnemyKnight[i].currentSprite--;
					if (g_EnemyKnight[i].currentSprite == -1) 
					{
						g_EnemyKnight[i].currentSprite = TEXTURE_ANIM_SPRITES_MAX - 1;
					}
				}
				
			}
			else //NO ループアニメーション
			{
				if (g_EnemyKnight[i].currentSprite < TEXTURE_ANIM_SPRITES_MAX - 1)
				{
					g_EnemyKnight[i].currentSprite++;

					if (g_EnemyKnight[i].currentAnim == KNIGHT_ANIM_ATTACK && g_EnemyKnight[i].currentSprite == 2)
					{
						PlaySound(SOUND_LABEL_SE_slash);
					}
				}
				
			}
			
		}

		if (g_EnemyKnight[i].currentAnim == KNIGHT_ANIM_ATTACK)
		{
			int attackStateCnt = g_EnemyKnight[i].stateTimeCnt - ATTACK_DELAY_TIME;

			if (attackStateCnt >= 0)
			{
				g_EnemyKnight[i].countAnim++;
			}
		}
		else 
		{
			g_EnemyKnight[i].countAnim++;
		}
		
		
	}
}

//Knightを出現させる
void SpawnEnemyKnight(XMFLOAT3 spawnPos)
{
	for (int i = 0; i < ENEMY_KNIGHT_MAX; i++)
	{
		if (g_EnemyKnight[i].spawned == FALSE)
		{
			g_EnemyKnight[i].spawned = TRUE;
			g_EnemyKnight[i].alive = TRUE;
			g_EnemyKnight[i].pos = spawnPos;
			break;
		}
	}
}


//Knightを未使用にする
void DespawnEnemyKnight(int enemyIndex)
{
	g_EnemyKnight[enemyIndex].stateTimeCnt++;

	if (g_EnemyKnight[enemyIndex].stateTimeCnt >= DESPAWN_TIME)
	{
		if (g_EnemyKnight[enemyIndex].enemyColor.w > 0)
		{
			g_EnemyKnight[enemyIndex].enemyColor.w -= 0.02f;
			return;
		}

		
		g_EnemyKnight[enemyIndex].spawned = FALSE;
		g_EnemyKnight[enemyIndex].alive = FALSE;
		g_EnemyKnight[enemyIndex].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	// 中心点から表示
		g_EnemyKnight[enemyIndex].w = KNIGHT_TEXTURE_WIDTH;
		g_EnemyKnight[enemyIndex].h = KNIGHT_TEXTURE_HEIGHT;

		g_EnemyKnight[enemyIndex].currentSprite = 0;
		g_EnemyKnight[enemyIndex].currentAnim = KNIGHT_ANIM_IDLE;

		g_EnemyKnight[enemyIndex].moveSpeed = 2.0f;		// 移動量
		g_EnemyKnight[enemyIndex].moveDir = KNIGHT_DIR_RIGHT;

		g_EnemyKnight[enemyIndex].HP = HP_MAX;

		g_EnemyKnight[enemyIndex].enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_EnemyKnight[enemyIndex].state = KNIGHT_STATE_CONTROLLER;
		g_EnemyKnight[enemyIndex].stateTime = 0;
		g_EnemyKnight[enemyIndex].stateTimeCnt = 0;


		g_EnemyKnight[enemyIndex].playerDistance = KNIGHT_DISTANCE_FAR;

	}
}

