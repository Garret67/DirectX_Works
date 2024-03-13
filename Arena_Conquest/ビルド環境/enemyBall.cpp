//=============================================================================
//
// エネミー処理 [enemyBall.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemyBall.h"

#include "fade.h"
#include "collision.h"
#include "sound.h"

#include "spawnController.h"
#include "bg.h"
#include "platforms.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(30 * 4)	// エネミーサイズ
#define TEXTURE_HEIGHT				(16 * 4)	// 
#define TEXTURE_MAX					(1)			// テクスチャの数

#define TEXTURE_ANIM_SPRITES_MAX	(4)			// 1つのアニメーションはスプライト何枚（X)
#define ANIM_WAIT					(10)		// アニメーションの切り替わるWait値

#define DAMAGE_TIME					(30)		// ダメージ状態の時間
#define KNOCKBACK_TIME				(15)		// ノックバックの時間
#define DESPAWN_TIME				(180)		// 殺されるから消えるまでの時間
												   
#define HP_MAX						(40.0f)		// 体力
												   
#define SEARCH_STATE_SPEED			(0.5f)		// SEARCH状態の速度
#define RUSH_STATE_SPEED			(8.0f)		// 突撃状態の速度

#define STATE_TIME_FIRST_SEARCH		(180)
#define STATE_TIME_JUMP				(25)

#define BALL_GRAVITY_FORCE				(8.0f)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void EnemyBallAnimationUpdate(void);
void BallJumpUpdate(void);

void DespawnEnemyBall(int enemyIndex);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/Ball/Bola_anim.png",
};

static ENEMY_BALL	g_EnemyBall[ENEMY_BALL_MAX];	// エネミー構造体

static BOOL			g_Load = FALSE;					// 初期化を行ったかのフラグ


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemyBall(void)
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
	for (int i = 0; i < ENEMY_BALL_MAX; i++)
	{	
		g_EnemyBall[i].spawned	= FALSE;
		g_EnemyBall[i].alive	= FALSE;
		g_EnemyBall[i].pos		= XMFLOAT3(1400.0f, 1000.0f, 0.0f);
		g_EnemyBall[i].w		= TEXTURE_WIDTH;
		g_EnemyBall[i].h		= TEXTURE_HEIGHT;
		g_EnemyBall[i].texNo	= 0;

		g_EnemyBall[i].currentSprite	= 0;
		g_EnemyBall[i].currentAnim		= BALL_ANIM_MOVE;

		g_EnemyBall[i].moveSpeed		= SEARCH_STATE_SPEED;
		g_EnemyBall[i].moveDir			= BALL_DIR_RIGHT;

		g_EnemyBall[i].HP				= HP_MAX;

		g_EnemyBall[i].damaged			= FALSE;
		g_EnemyBall[i].damageOriginPos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyBall[i].dmgTimeCnt		= 0;

		g_EnemyBall[i].enemyColor		= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

		g_EnemyBall[i].state			= BALL_STATE_SEARCH;
		g_EnemyBall[i].stateTime		= STATE_TIME_FIRST_SEARCH;
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemyBall(void)
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
void UpdateEnemyBall(void)
{

	for (int i = 0; i < ENEMY_BALL_MAX; i++)
	{
		if (g_EnemyBall[i].spawned == FALSE) continue;
	

		// 生きてるエネミーだけ処理をする
		if (g_EnemyBall[i].alive == TRUE)
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
						BOOL ans = CollisionBB(g_EnemyBall[i].pos,	g_EnemyBall[i].w,	g_EnemyBall[i].h,
											   player[j].pos,		player[j].w,		player[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							PlayerDamaged(g_EnemyBall[i].pos);

							//ジャンプする
							if (g_EnemyBall[i].state == BALL_STATE_RUSH)
							{
								g_EnemyBall[i].state = BALL_STATE_JUMP;
								g_EnemyBall[i].stateTime = STATE_TIME_JUMP;
							}
						}
					}
				}
			}

			//丸いエネミーのパトロール
			{
				//プレイヤーを探している状態
				if (g_EnemyBall[i].state == BALL_STATE_SEARCH)
				{
					if (g_EnemyBall[i].stateTimeCnt < g_EnemyBall[i].stateTime)
					{

						PLAYER* player = GetPlayer();

						//プレイヤーに向かって後ろに動く
						if (g_EnemyBall[i].pos.x < player[0].pos.x)
						{
							g_EnemyBall[i].moveDir   = BALL_DIR_RIGHT;
							g_EnemyBall[i].moveSpeed = -SEARCH_STATE_SPEED;
						}
						else
						{
							g_EnemyBall[i].moveDir   = BALL_DIR_LEFT;
							g_EnemyBall[i].moveSpeed = SEARCH_STATE_SPEED;
						}

						g_EnemyBall[i].pos.x += g_EnemyBall[i].moveSpeed;	//プレイヤーの反対側に動く（構っている）

						g_EnemyBall[i].stateTimeCnt++;

						
					}
					else	//探す状態が終わる
					{
						g_EnemyBall[i].state		= BALL_STATE_RUSH;		//突撃状態に変わる
						g_EnemyBall[i].stateTimeCnt = 0;					//アクション状態時間をリセット
						g_EnemyBall[i].moveSpeed   *= -1;					//速度をプレイヤーの方に変わる
					}
					
					
				}

				//突撃状態
				if (g_EnemyBall[i].state == BALL_STATE_RUSH)
				{
					g_EnemyBall[i].pos.x += g_EnemyBall[i].moveSpeed;

					if (fabs(g_EnemyBall[i].moveSpeed) < RUSH_STATE_SPEED)
					{
						g_EnemyBall[i].moveSpeed += g_EnemyBall[i].moveSpeed;
					}
				}

				BallJumpUpdate();
				
			}
		}
		else
		{
			DespawnEnemyBall(i);
		}
			

		//エネミーが傷付いた
		{
			if (g_EnemyBall[i].damaged == TRUE)
			{
				//ノックバック
				if (g_EnemyBall[i].dmgTimeCnt < KNOCKBACK_TIME)	//はね返す処理は最初の15フレームだけ
				{
					XMVECTOR epos = XMLoadFloat3(&g_EnemyBall[i].pos);
					XMVECTOR vec  = (epos - XMLoadFloat3(&g_EnemyBall[i].damageOriginPos));		//自分とダメージのところとの差分を求めて
					float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);						//その差分を使って角度を求めている
					float repealDisstance = 15.0f;												//スピードはちょっと遅くしてみる
					float repealDisstancePerFrame = repealDisstance / KNOCKBACK_TIME;

					//SMOOTH
					float repealDistNow = repealDisstancePerFrame * (KNOCKBACK_TIME - g_EnemyBall[i].dmgTimeCnt);

					g_EnemyBall[i].pos.x += cosf(angle) * repealDistNow;					//angleの方向へ移動
					g_EnemyBall[i].pos.y += sinf(angle) * repealDistNow;					//angleの方向へ移動
				}
		

				g_EnemyBall[i].dmgTimeCnt++;

				//ダメージが終わったら
				if (g_EnemyBall[i].dmgTimeCnt >= DAMAGE_TIME)
				{
					g_EnemyBall[i].damaged	  = FALSE;
					g_EnemyBall[i].dmgTimeCnt = 0;
					
					g_EnemyBall[i].enemyColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
					
				}
			}


		}

		//重力
		g_EnemyBall[i].pos.y += BALL_GRAVITY_FORCE;




		// Groundとの当たり判定
		{
			PLATFORM* ground = GetGround();

			// groundSの数分当たり判定を行う
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;

				BOOL ans = CheckCollisionAndGetDirection(ground[j].pos.x,		ground[j].pos.y,		(ground[j].w),		(ground[j].h),
														 g_EnemyBall[i].pos.x,	g_EnemyBall[i].pos.y,	(g_EnemyBall[i].w), (g_EnemyBall[i].h));
				// 当たっている？

				switch (ans)
				{
				case FromTop:
					g_EnemyBall[i].pos.y = ground[j].pos.y - (ground[j].h / 2) - (g_EnemyBall[i].h / 2);
					break;

				case FromBottom:
					g_EnemyBall[i].pos.y = ground[j].pos.y + (ground[j].h / 2) + (g_EnemyBall[i].h / 2);
					break;

				case FromLeft:
					g_EnemyBall[i].pos.x = ground[j].pos.x - (ground[j].w / 2) - (g_EnemyBall[i].w / 2);

					//横にぶつかったらJUMP状態に変わる
					if (g_EnemyBall[i].pos.y > (ground[j].pos.y - ground[j].h / 2) && g_EnemyBall[i].pos.y < (ground[j].pos.y + ground[j].h / 2))
					{
						if (g_EnemyBall[i].state == BALL_STATE_RUSH)
						{
							g_EnemyBall[i].state	 = BALL_STATE_JUMP;
							g_EnemyBall[i].stateTime = STATE_TIME_JUMP;
						}
					}
					break;

				case FromRight:
					g_EnemyBall[i].pos.x = ground[j].pos.x + (ground[j].w / 2) + (g_EnemyBall[i].w / 2);

					//横にぶつかったらJUMP状態に変わる
					if (g_EnemyBall[i].pos.y > (ground[j].pos.y - ground[j].h / 2) && g_EnemyBall[i].pos.y < (ground[j].pos.y + ground[j].h / 2))
					{
						if (g_EnemyBall[i].state == BALL_STATE_RUSH)
						{
							g_EnemyBall[i].state	 = BALL_STATE_JUMP;
							g_EnemyBall[i].stateTime = STATE_TIME_JUMP;
						}
					}
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

				BOOL ans = CheckCollisionAndGetDirection(platform[j].pos.x,		platform[j].pos.y,		(platform[j].w),	(platformH),
														 g_EnemyBall[i].pos.x,	g_EnemyBall[i].pos.y,	(g_EnemyBall[i].w), (g_EnemyBall[i].h));

				// 当たっている？
				switch (ans)
				{
				case FromTop:
					g_EnemyBall[i].pos.y = platform[j].pos.y - (platformH / 2) - (g_EnemyBall[i].h / 2);
					break;

				case FromBottom:
					g_EnemyBall[i].pos.y = platform[j].pos.y + (platformH / 2) + (g_EnemyBall[i].h / 2);
					break;

				case FromLeft:
					g_EnemyBall[i].pos.x = platform[j].pos.x - (platform[j].w / 2) - (g_EnemyBall[i].w / 2);

					//横にぶつかったらJUMP状態に変わる
					if (g_EnemyBall[i].pos.y > (platform[j].pos.y - platformH / 2) && g_EnemyBall[i].pos.y < (platform[j].pos.y + platformH / 2))
					{
						if (g_EnemyBall[i].state == BALL_STATE_RUSH)
						{
							g_EnemyBall[i].state	 = BALL_STATE_JUMP;
							g_EnemyBall[i].stateTime = STATE_TIME_JUMP;
						}
					}
					break;

				case FromRight:
					g_EnemyBall[i].pos.x = platform[j].pos.x + (platform[j].w / 2) + (g_EnemyBall[i].w / 2);

					//横にぶつかったらJUMP状態に変わる
					if (g_EnemyBall[i].pos.y > (platform[j].pos.y - platformH / 2) && g_EnemyBall[i].pos.y < (platform[j].pos.y + platformH / 2))
					{
						if (g_EnemyBall[i].state == BALL_STATE_RUSH)
						{
							g_EnemyBall[i].state	 = BALL_STATE_JUMP;
							g_EnemyBall[i].stateTime = STATE_TIME_JUMP;
						}
					}
					break;

				default:

					break;
				}


			}
		}



	}


	// アニメーション
	EnemyBallAnimationUpdate();


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemyBall(void)
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

	for (int i = 0; i < ENEMY_BALL_MAX; i++)
	{
		if (g_EnemyBall[i].spawned == FALSE) continue;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_EnemyBall[i].texNo]);

		//エネミーの位置やテクスチャー座標を反映
		float px = g_EnemyBall[i].pos.x - bg->pos.x;	// エネミーの表示位置X
		float py = g_EnemyBall[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
		float pw = g_EnemyBall[i].w;					// エネミーの表示幅
		float ph = g_EnemyBall[i].h;					// エネミーの表示高さ

		float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// テクスチャの幅
		float th = 1.0f / BALL_ANIM_MAX;				// テクスチャの高さ
		float tx = tw * g_EnemyBall[i].currentSprite;	// テクスチャの左上X座標
		float ty = th * g_EnemyBall[i].currentAnim;		// テクスチャの左上Y座標

		//右に向いたらテクスチャを反転する
		if (g_EnemyBall[i].moveDir == BALL_DIR_RIGHT)
		{
			tx += tw;
			tw *= -1.0f;
		}

		ty += 0.01f;	//スプライトの上に変な線が現れないようにテクスチャをちょっとすらす
		


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					   g_EnemyBall[i].enemyColor);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}


//=============================================================================
// EnemyBall構造体の先頭アドレスを取得
//=============================================================================
ENEMY_BALL* GetEnemyBall(void)
{
	return &g_EnemyBall[0];
}

//=============================================================================
// EnemyBallがダメージを受けた
// 引数: playerPos (プレイヤーの座標), enemyIndex (エネミーのインデクス), damage (ダメージ量)
//=============================================================================
void EnemyBallDamaged(XMFLOAT3 playerPos, int enemyIndex, float damage)
{
	//ダメージ影響
	g_EnemyBall[enemyIndex].damaged			= TRUE;
	g_EnemyBall[enemyIndex].damageOriginPos = playerPos;
	g_EnemyBall[enemyIndex].HP				-= damage;
	g_EnemyBall[enemyIndex].enemyColor		= XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f);

	//アニメーション
	g_EnemyBall[enemyIndex].countAnim		= ANIM_WAIT + 1;
	g_EnemyBall[enemyIndex].currentSprite	= 0;

	PlaySound(SOUND_LABEL_SE_bloodyBlade);

	//死んだら
	if (g_EnemyBall[enemyIndex].HP <= 0)
	{
		g_EnemyBall[enemyIndex].alive		= FALSE;
		g_EnemyBall[enemyIndex].enemyColor  = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		RoundKill();
	}
}


//=============================================================================
// EnemyBallアニメーション処理
//=============================================================================
void EnemyBallAnimationUpdate(void)
{
	for (int i = 0; i < ENEMY_BALL_MAX; i++)
	{
		//アニメーションの判断
		if (g_EnemyBall[i].alive == FALSE)
		{
			g_EnemyBall[i].currentAnim = BALL_ANIM_DIE;
		}
		else if (g_EnemyBall[i].damaged == TRUE)
		{
			g_EnemyBall[i].currentAnim = BALL_ANIM_HIT;
		}
		else if (g_EnemyBall[i].state == BALL_STATE_SEARCH)
		{
			g_EnemyBall[i].currentAnim = BALL_ANIM_SEARCH;
		}
		else
		{
			g_EnemyBall[i].currentAnim = BALL_ANIM_MOVE;
		}

		g_EnemyBall[i].countAnim ++;


		if (g_EnemyBall[i].countAnim > ANIM_WAIT)
		{
			g_EnemyBall[i].countAnim = 0;

			//ループアニメーション
			if (g_EnemyBall[i].currentAnim == BALL_ANIM_MOVE) 
			{
				g_EnemyBall[i].currentSprite = (g_EnemyBall[i].currentSprite + 1) % TEXTURE_ANIM_SPRITES_MAX;
			}
			else //NO ループアニメーション
			{
				if (g_EnemyBall[i].currentSprite < TEXTURE_ANIM_SPRITES_MAX - 1)
				{
					g_EnemyBall[i].currentSprite++;
				}
				
			}
			
		}
	}
}


//=============================================================================
// 跳ねる処理
//=============================================================================
void BallJumpUpdate(void)
{
	for (int i = 0; i < ENEMY_BALL_MAX; i++)
	{
		if (g_EnemyBall[i].spawned == FALSE) continue;

		if (g_EnemyBall[i].state == BALL_STATE_JUMP)
		{

			float angle;		//その差分を使って角度を求めている

			if (g_EnemyBall[i].moveDir == BALL_DIR_LEFT)
			{
				angle = -XM_PIDIV4;		//左上の角度
			}
			else
			{
				angle = -XM_PIDIV4 * 3;	//右上の角度
			}


			float repealDisstance			= 10.0f;										//スピードはちょっと遅くしてみる
			float repealDisstancePerFrame	= repealDisstance / g_EnemyBall[i].stateTime;

			float repealDistNow				= repealDisstancePerFrame * (g_EnemyBall[i].stateTime - g_EnemyBall[i].stateTimeCnt);


			g_EnemyBall[i].pos.x += cosf(angle) * repealDistNow;					//angleの方向へ移動
			g_EnemyBall[i].pos.y += sinf(angle) * repealDistNow;					//angleの方向へ移動

			g_EnemyBall[i].stateTimeCnt++;

			//終わったSEARCH状態に変わる
			if (g_EnemyBall[i].stateTimeCnt >= g_EnemyBall[i].stateTime)
			{
				g_EnemyBall[i].state		= BALL_STATE_SEARCH;
				g_EnemyBall[i].stateTime	= 30 + (rand() % 200);
				g_EnemyBall[i].stateTimeCnt = 0;
			}
		}
	}
	
}

//=============================================================================
// EnemyBallを出現させる
// 引数: spawnPos(出現される座標)
//=============================================================================
void SpawnEnemyBall(XMFLOAT3 spawnPos)
{
	for (int i = 0; i < ENEMY_BALL_MAX; i++)
	{
		if (g_EnemyBall[i].spawned == FALSE)
		{
			g_EnemyBall[i].spawned	= TRUE;
			g_EnemyBall[i].alive	= TRUE;
			g_EnemyBall[i].pos		= spawnPos;
			break;
		}
	}
}


//=============================================================================
// EnemyBallを未使用にする
//=============================================================================
void DespawnEnemyBall(int enemyIndex)
{
	g_EnemyBall[enemyIndex].dmgTimeCnt++;

	if (g_EnemyBall[enemyIndex].dmgTimeCnt >= DESPAWN_TIME)
	{
		//ちょっとずつ透明にする
		if (g_EnemyBall[enemyIndex].enemyColor.w > 0)
		{
			g_EnemyBall[enemyIndex].enemyColor.w -= 0.02f;
			return;
		}

		//透明だったらリセットする
		g_EnemyBall[enemyIndex].spawned			= FALSE;
		g_EnemyBall[enemyIndex].alive			= FALSE;
		g_EnemyBall[enemyIndex].pos				= XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_EnemyBall[enemyIndex].currentSprite	= 0;
		g_EnemyBall[enemyIndex].currentAnim		= BALL_ANIM_MOVE;

		g_EnemyBall[enemyIndex].moveSpeed		= SEARCH_STATE_SPEED;
		g_EnemyBall[enemyIndex].moveDir			= BALL_DIR_RIGHT;

		g_EnemyBall[enemyIndex].HP				= HP_MAX;

		g_EnemyBall[enemyIndex].damaged			= FALSE;
		g_EnemyBall[enemyIndex].damageOriginPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyBall[enemyIndex].dmgTimeCnt		= 0;

		g_EnemyBall[enemyIndex].enemyColor		= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

		g_EnemyBall[enemyIndex].state			= BALL_STATE_SEARCH;
		g_EnemyBall[enemyIndex].stateTime		= STATE_TIME_FIRST_SEARCH;
	}
}
