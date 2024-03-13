//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "bullet.h"

#include "enemyPatrol.h"
#include "enemyFlyBulletOne.h"
#include "enemyFlyBounce.h"
#include "enemyBall.h"
#include "enemyKnight.h"
#include "enemySlimeKing.h"
#include "enemyBoss.h"

#include "collision.h"
#include "score.h"
#include "bg.h"
#include "player.h"
#include "platforms.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(70.0f / 2)		// 弾の幅
#define TEXTURE_HEIGHT				(277.0f / 2)	// 弾の高さ

#define TEXTURE_PATTERN_DIVIDE_X	(8)				// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)				// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)				// アニメーションの切り替わるWait値

#define BULLET_DAMAGE				(10.0f)			// 弾のダメージ
#define BULLET_SPEED				(15.0f)			// 弾の移動スピード

#define EXPLOSION_WIDTH				(100.0f)		// 弾の爆発の幅
#define EXPLOSION_HEIGHT			(100.0f)		// 弾の爆発の高さ

#define BULLET_MAX					(10)			// 弾のMax数
#define EXPLOSION_MAX				(10)			// 弾のMax数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawBulletExplosion(void);			//弾の爆発アニメーション描画
void ExplosionAnimationUpdate(void);	//弾の爆発アニメーション処理

void SetExplosion(XMFLOAT3 pos);		//弾の設定

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[BULLET_TEX_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[BULLET_TEX_MAX] =
{
	"data/TEXTURE/Effect/FireBullet.png",
	"data/TEXTURE/Effect/ColliderImage.png",
	"data/TEXTURE/Effect/BulletExplosionG.png",
};

static BULLET		g_Bullet[BULLET_MAX];		// バレット構造体
static EXPLOSION	g_Explosion[EXPLOSION_MAX];	// バレットの爆発構造体

static BOOL			g_Load = FALSE;				// 初期化を行ったかのフラグ


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < BULLET_TEX_MAX; i++)
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


	// バレット構造体の初期化
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use			= FALSE;
		g_Bullet[i].w			= TEXTURE_WIDTH;
		g_Bullet[i].h			= TEXTURE_HEIGHT;
		g_Bullet[i].pos			= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].rot			= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].texNo		= BULLET_TEX_FIRE_BULLET;

		g_Bullet[i].countAnim	= 0;
		g_Bullet[i].patternAnim = 0;

		g_Bullet[i].move		= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].color		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}


	// EXPLOSION構造体の初期化
	for (int i = 0; i < EXPLOSION_MAX; i++)
	{
		g_Explosion[i].use			 = FALSE;
		g_Explosion[i].pos			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Explosion[i].w			 = EXPLOSION_WIDTH;
		g_Explosion[i].h			 = EXPLOSION_HEIGHT;
		g_Explosion[i].texNo		 = BULLET_TEX_BULLET_EXPLOSION;

		g_Explosion[i].countAnim	 = 0;
		g_Explosion[i].currentSprite = 0;

		g_Explosion[i].color		 = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < BULLET_TEX_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBullet(void)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)	continue;	// このバレットが使われている？
		
		// アニメーション  
		g_Bullet[i].countAnim++;
		if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
		{
			// パターンの切り替え
			g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
		}


		// バレットの移動処理
		XMVECTOR pos  = XMLoadFloat3(&g_Bullet[i].pos);
		XMVECTOR move = XMLoadFloat3(&g_Bullet[i].move);
		pos += move;
		XMStoreFloat3(&g_Bullet[i].pos, pos);


		// 画面外まで進んだ？
		BG* bg = GetBG();
		if (g_Bullet[i].pos.x < (-g_Bullet[i].w/2))		// 自分の大きさを考慮して画面外か判定している
		{
			g_Bullet[i].use = FALSE;
		}
		if (g_Bullet[i].pos.x > (bg->w + g_Bullet[i].w/2))	// 自分の大きさを考慮して画面外か判定している
		{
			g_Bullet[i].use = FALSE;
		}


		//当たり判定の座標、高さと幅
		XMFLOAT3 BulletPos = XMFLOAT3(/* X */ g_Bullet[i].pos.x + (cosf(g_Bullet[i].rot.z + (XM_PIDIV2)) * (g_Bullet[i].h / 2 - g_Bullet[i].w / 2)),
									  /* Y */ g_Bullet[i].pos.y + (sinf(g_Bullet[i].rot.z + (XM_PIDIV2)) * (g_Bullet[i].h / 2 - g_Bullet[i].w / 2)),
									  /* Z */ 0.0f);

		//当たり判定のための幅と高さ
		float BulletWH = g_Bullet[i].w;	


		// Ground との当たり判定
		{
			PLATFORM* ground = GetGround();

			// groundSの数分当たり判定を行う
			for (int j = 0; j < GROUND_MAX; j++)
			{
				if (ground[j].use == FALSE) break;

				BOOL ans = CollisionBB(	BulletPos,		BulletWH,		BulletWH,
										ground[j].pos,	ground[j].w,	ground[j].h);

				// 当たっている？
				if (ans == TRUE)
				{
					// 当たった時の処理
					SetExplosion(BulletPos);					//弾の爆発設定
					g_Bullet[i].use = FALSE;					//弾を未使用にする
					PlaySound(SOUND_LABEL_SE_MagicExplosion);	//音
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

				BOOL ans = CollisionBB(	BulletPos,			BulletWH,			BulletWH,
										platform[j].pos,	(platform[j].w),	platformH);

				// 当たっている？
				if (ans == TRUE)
				{
					// 当たった時の処理
					SetExplosion(BulletPos);					//弾の爆発設定
					g_Bullet[i].use = FALSE;					//弾を未使用にする
					PlaySound(SOUND_LABEL_SE_MagicExplosion);	//音
				}
			}
		}




		// プレイヤーとの当たり判定処理
		{
			if (g_Bullet[i].color.z == 0.0f)
			{

				PLAYER* player = GetPlayer();

				// プレイヤーの数分当たり判定を行う
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					// 生きてるプレイヤーと当たり判定をする
					if (player[j].alive == TRUE && player[j].invincible == FALSE)
					{
						BOOL ans = CollisionBB(	BulletPos,		BulletWH,		BulletWH,
												player[j].pos,	player[j].w,	player[j].h);

						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							PlayerDamaged(BulletPos);					//プレイヤーのダメージ
							SetExplosion(player[j].pos);				//弾の爆発設定
							g_Bullet[i].use = FALSE;					//弾を未使用にする
							PlaySound(SOUND_LABEL_SE_MagicExplosion);	//音
						}
					}
				}
			}
		}


		// エネミーの当たり判定処理
		{
			if (g_Bullet[i].color.z == 0.0f) continue;	//エネミーの弾だったらエネミーとの当たり判定をしない


			BOSS Boss = GetBoss();
	
			// 生きてるエネミーと当たり判定をする
			if (Boss.alive == TRUE)
			{
				BOOL ans = CollisionBB(	BulletPos,	BulletWH,	BulletWH,
										Boss.pos,	Boss.w,		Boss.h);

				// 当たっている？
				if (ans == TRUE)
				{
					// 当たった時の処理
					BossDamaged(BULLET_DAMAGE);					//エネミーのダメージ
					SetExplosion(Boss.pos);						//弾の爆発設定
					g_Bullet[i].use = FALSE;					//弾を未使用にする
					PlaySound(SOUND_LABEL_SE_MagicExplosion);	//音

				}
			}
				



			SLIME_KING slimeKing = GetSlimeKing();

			// 生きてるエネミーと当たり判定をする
			if (slimeKing.alive == TRUE)
			{
				BOOL ans = CollisionBB(	BulletPos,		BulletWH,		BulletWH,
										slimeKing.pos,	slimeKing.w,	slimeKing.h);

				// 当たっている？
				if (ans == TRUE)
				{
					// 当たった時の処理
					SlimeKingDamaged(BULLET_DAMAGE);			//エネミーのダメージ
					SetExplosion(slimeKing.pos);				//弾の爆発設定
					g_Bullet[i].use = FALSE;					//弾を未使用にする
					PlaySound(SOUND_LABEL_SE_MagicExplosion);	//音
				}
			}
				


			ENEMY_PATROL* enemyPatrol = GetEnemyPatrol();

			// エネミーの数分当たり判定を行う
			for (int j = 0; j < ENEMY_PATROL_MAX; j++)
			{
				// 生きてるとダメージ状態じゃないエネミーと当たり判定をする
				if (enemyPatrol[j].alive == TRUE && enemyPatrol[j].damaged == FALSE)
				{
					BOOL ans = CollisionBB(	BulletPos,			BulletWH,			BulletWH,
											enemyPatrol[j].pos, enemyPatrol[j].w,	enemyPatrol[j].h);

					// 当たっている？
					if (ans == TRUE)
					{
						// 当たった時の処理
						EnemyDamaged(g_Bullet[i].pos, j, BULLET_DAMAGE);	//エネミーのダメージ
						SetExplosion(enemyPatrol[j].pos);					//弾の爆発設定
							
					}
				}
			}


			ENEMY_BALL* enemyBall = GetEnemyBall();

			// エネミーの数分当たり判定を行う
			for (int j = 0; j < ENEMY_BALL_MAX; j++)
			{
				// 生きてるとダメージ状態じゃないエネミーと当たり判定をする
				if (enemyBall[j].alive == TRUE && enemyBall[j].damaged == FALSE)
				{
					BOOL ans = CollisionBB(	BulletPos,			BulletWH,			BulletWH,
											enemyBall[j].pos,	enemyBall[j].w,		enemyBall[j].h);

					// 当たっている？
					if (ans == TRUE)
					{
						// 当たった時の処理
						EnemyBallDamaged(g_Bullet[i].pos, j, BULLET_DAMAGE);	//エネミーのダメージ
						SetExplosion(enemyBall[j].pos);							//弾の爆発設定
					}
				}
			}



			ENEMY_FLY_BOUNCE* enemyFlyBounce = GetEnemyFlyBounce();

			// エネミーの数分当たり判定を行う
			for (int j = 0; j < ENEMY_FLY_BOUNCE_MAX; j++)
			{
				// 生きてるとダメージ状態じゃないエネミーと当たり判定をする
				if (enemyFlyBounce[j].alive == TRUE && enemyFlyBounce[j].damaged == FALSE)
				{
					BOOL ans = CollisionBB(	BulletPos,				BulletWH,				BulletWH,
											enemyFlyBounce[j].pos,	enemyFlyBounce[j].w,	enemyFlyBounce[j].h);

					// 当たっている？
					if (ans == TRUE)
					{
						// 当たった時の処理
						EnemyFlyBounceDamaged(g_Bullet[i].pos, j, BULLET_DAMAGE);	//エネミーのダメージ
						SetExplosion(enemyFlyBounce[j].pos);						//弾の爆発設定
					}
				}
			}




			ENEMY_FLY_ONE* enemyFlyOne = GetEnemyFlyOne();

			// エネミーの数分当たり判定を行う
			for (int j = 0; j < ENEMY_FLY_ONE_MAX; j++)
			{
				// 生きてるとダメージ状態じゃないエネミーと当たり判定をする
				if (enemyFlyOne[j].alive == TRUE && (enemyFlyOne[j].state != FLY_ONE_STATE_HIT))
				{
					BOOL ans = CollisionBB(	BulletPos,				BulletWH,			BulletWH,
											enemyFlyOne[j].pos,		enemyFlyOne[j].w,	enemyFlyOne[j].h);
					// 当たっている？
					if (ans == TRUE)
					{
						// 当たった時の処理
						EnemyFlyOneDamaged(g_Bullet[i].pos, j, BULLET_DAMAGE);	//エネミーのダメージ
						SetExplosion(enemyFlyOne[j].pos);						//弾の爆発設定
					}
				}
			}



			ENEMY_KNIGHT* enemyKnight = GetEnemyKnight();

			// エネミーの数分当たり判定を行う
			for (int j = 0; j < ENEMY_KNIGHT_MAX; j++)
			{
				// 生きてるとダメージ状態じゃないエネミーと当たり判定をする
				if (enemyKnight[j].alive == TRUE && (enemyKnight[j].state != KNIGHT_STATE_HIT))
				{
					BOOL ans = CollisionBB(	BulletPos,				BulletWH,					BulletWH,
											enemyKnight[j].pos,		KNIGHT_COLLIDER_WIDTH,		enemyKnight[j].h);
					// 当たっている？
					if (ans == TRUE)
					{
						// 当たった時の処理
						EnemyKnightDamaged(g_Bullet[i].pos, j, BULLET_DAMAGE);	//エネミーのダメージ
						SetExplosion(enemyKnight[j].pos);						//弾の爆発設定

					}
				}
			}
		}
		
	}

	ExplosionAnimationUpdate();	//弾の爆発アニメーション処理
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
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

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE) continue;	// このバレットが使われている？
		
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

		//バレットの位置やテクスチャー座標を反映
		float px = g_Bullet[i].pos.x - bg->pos.x;	// バレットの表示位置X
		float py = g_Bullet[i].pos.y - bg->pos.y;	// バレットの表示位置Y
		float pw = g_Bullet[i].w;					// バレットの表示幅
		float ph = g_Bullet[i].h;					// バレットの表示高さ

		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;										// テクスチャの幅
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;										// テクスチャの高さ
		float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
		float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, 
			px, py, pw, ph, 
			tx, ty, tw, th,
			g_Bullet[i].color,
			g_Bullet[i].rot.z);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


#ifdef _DEBUG	// デバッグ情報を表示する


		XMFLOAT3 BulletPos = XMFLOAT3(	/* X */ g_Bullet[i].pos.x + (cosf(g_Bullet[i].rot.z + (XM_PIDIV2)) * (g_Bullet[i].h / 2 - g_Bullet[i].w / 2)),
										/* Y */ g_Bullet[i].pos.y + (sinf(g_Bullet[i].rot.z + (XM_PIDIV2)) * (g_Bullet[i].h / 2 - g_Bullet[i].w / 2)),
										/* Z */ 0.0f);

		float BulletWH = g_Bullet[i].w;	//当たり判定のための幅と高さ
			
		BG* bg = GetBG();
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BULLET_TEX_COLLIDER]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer,
			BulletPos.x - bg->pos.x, BulletPos.y - bg->pos.y, BulletWH, BulletWH,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

#endif
		
	}

	DrawBulletExplosion();

}


//=============================================================================
// 弾の爆発の描画処理
//=============================================================================
void DrawBulletExplosion(void)
{
	BG* bg = GetBG();

	for (int i = 0; i < EXPLOSION_MAX; i++)
	{
		if (g_Explosion[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Explosion[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Explosion[i].pos.x - bg->pos.x;	// バレットの表示位置X
			float py = g_Explosion[i].pos.y - bg->pos.y;	// バレットの表示位置Y
			float pw = g_Explosion[i].w;		// バレットの表示幅
			float ph = g_Explosion[i].h;		// バレットの表示高さ

			float tw = 1.0f / 4;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = g_Explosion[i].currentSprite * tw;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				g_Bullet[i].color);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// バレット構造体の先頭アドレスを取得。
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_Bullet[0];
}


//=============================================================================
// プレイヤーの弾の発射設定
// 引数: pos (弾の座標), direction(プレイヤーの向き)
//=============================================================================
void SetBulletPlayer(XMFLOAT3 pos, int direction)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Bullet[i].use   = TRUE;								// 使用状態へ変更する
			g_Bullet[i].pos   = pos;								// 座標をセット
			g_Bullet[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);	// 色の設定

			//向かう方向
			switch (direction)
			{
			case CHAR_DIR_LEFT:
				g_Bullet[i].move = XMFLOAT3(-BULLET_SPEED, 0.0f, 0.0f);
				g_Bullet[i].rot  = XMFLOAT3(0.0f, 0.0f, XM_PIDIV2);
				break;

			case CHAR_DIR_RIGHT:
				g_Bullet[i].move = XMFLOAT3(BULLET_SPEED, 0.0f, 0.0f);
				g_Bullet[i].rot  = XMFLOAT3(0.0f, 0.0f, -XM_PIDIV2);
				break;

			case CHAR_DIR_UP:
				g_Bullet[i].move = XMFLOAT3(0.0f, -BULLET_SPEED, 0.0f);
				g_Bullet[i].rot  = XMFLOAT3(0.0f, 0.0f, XM_PI);
				break;

			case CHAR_DIR_DOWN:
				g_Bullet[i].move = XMFLOAT3(0.0f, BULLET_SPEED, 0.0f);
				g_Bullet[i].rot  = XMFLOAT3(0.0f, 0.0f, 0.0f);
				break;
			}

			return;			// 1発セットしたので終了する
		}
	}
}

//=============================================================================
// エネミーの弾の発射設定
// 引数: pos (弾の座標), rotation (弾の回転), MoveDir(移動量)
//=============================================================================
void SetBulletEnemy(XMFLOAT3 pos, float rotation, XMFLOAT3 MoveDir)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Bullet[i].use   = TRUE;								// 使用状態へ変更する
			g_Bullet[i].pos   = pos;								// 座標をセット
			g_Bullet[i].move  = MoveDir;							// 向かう方向
			g_Bullet[i].rot   = XMFLOAT3(0.0f, 0.0f, rotation);		// 回転
			g_Bullet[i].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);	// 色の設定

			return;							// 1発セットしたので終了する
		}
	}
}

//=============================================================================
// 弾の爆発設定
//=============================================================================
void SetExplosion(XMFLOAT3 pos)
{
	for (int i = 0; i < EXPLOSION_MAX; i++)
	{
		if (g_Explosion[i].use == FALSE)
		{
			g_Explosion[i].use = TRUE;
			g_Explosion[i].pos = pos;
			return;
		}
	}
	
}

//=============================================================================
// 弾の爆発の処理
//=============================================================================
void ExplosionAnimationUpdate(void)
{
	for (int i = 0; i < EXPLOSION_MAX; i++)
	{
		if (g_Explosion[i].use == TRUE)
		{
			if (g_Explosion[i].countAnim > ANIM_WAIT)
			{
				//アニメーションが終わったら初期化する
				if (g_Explosion[i].currentSprite == 3)
				{
					g_Explosion[i].use				= FALSE;
					g_Explosion[i].pos				= XMFLOAT3(0.0f, 0.0f, 0.0f);
					g_Explosion[i].currentSprite	= 0;
					g_Explosion[i].countAnim		= 0;
				}

				g_Explosion[i].currentSprite++;
				g_Explosion[i].countAnim = 0;
			}
			g_Explosion[i].countAnim++;

		}
	}

}