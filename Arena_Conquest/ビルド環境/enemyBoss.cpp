//=============================================================================
//
// エネミー処理 [enemyBoss.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemyBoss.h"
#include "spawnController.h"
#include "bg.h"
#include "platforms.h"
#include "player.h"
#include "fade.h"
#include "collision.h"
#include "sound.h"
#include "platforms.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100.0f)	// キャラサイズ
#define TEXTURE_HEIGHT				(120.0f)	// キャラサイズ

#define PARTICLE_SIZE				(50.0f)		// パーティクルの大きさ

#define TEXTURE_ANIM_SPRITES_MAX	(5)			// 1つのアニメーションはスプライト何枚（X)
#define ANIM_WAIT					(4)			// アニメーションの切り替わるWait値

#define DAMAGE_TIME					(30)		// ダメージ状態の時間
#define DESPAWN_TIME				(180)		// 殺されるから消えるまでの時間
#define HP_MAX						(300.0f)	// 体力

#define BOSS_PROJECTILE_MAX			(7)			// 電気玉のMax数
#define BOSS_PARTICLE_MAX			(40)		// パーティクルのMax数

#define THIN_LASER					(5.0f)		// レーザーが細い時の幅
#define THICK_LASER					(100.0f)	// レーザーが太い時の幅

#define BOSS_PROJECTILE_SPEED		(10.0f)		// 電気玉の速度

//PHASE 1
#define PHASE_1_ATTACK_DURATION_CHASE		(120)	//攻撃時間
#define PHASE_1_ATTACK_DURATION_SHOTS		(40)	//攻撃時間
#define PHASE_1_ATTACK_DURATION_LASER		(100)	//攻撃時間
#define PHASE_1_NUMBER_OF_ATTACKS_CHASE		(3)		//攻撃回数
#define PHASE_1_NUMBER_OF_ATTACKS_SHOTS		(3)		//攻撃回数
#define PHASE_1_NUMBER_OF_ATTACKS_LASER		(3)		//攻撃回数

//PHASE 2
#define PHASE_2_ATTACK_DURATION_CHASE		(90)	//攻撃時間
#define PHASE_2_ATTACK_DURATION_SHOTS		(30)	//攻撃時間
#define PHASE_2_ATTACK_DURATION_LASER		(80)	//攻撃時間
#define PHASE_2_NUMBER_OF_ATTACKS_CHASE		(5)		//攻撃回数
#define PHASE_2_NUMBER_OF_ATTACKS_SHOTS		(5)		//攻撃回数
#define PHASE_2_NUMBER_OF_ATTACKS_LASER		(5)		//攻撃回数

//PHASE 3
#define PHASE_3_ATTACK_DURATION_CHASE		(60)	//攻撃時間
#define PHASE_3_ATTACK_DURATION_SHOTS		(20)	//攻撃時間
#define PHASE_3_ATTACK_DURATION_LASER		(60)	//攻撃時間
#define PHASE_3_NUMBER_OF_ATTACKS_CHASE		(7)		//攻撃回数
#define PHASE_3_NUMBER_OF_ATTACKS_SHOTS		(7)		//攻撃回数
#define PHASE_3_NUMBER_OF_ATTACKS_LASER		(7)		//攻撃回数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DespawnBoss(void);

void SetBossPartible(XMFLOAT3 pos);

double easeInOutQuad(double t);

void SetBossProjectile(void);
void UpdateBossProjectile(void);
void DrawBossProjectiles(void);

void DrawBossLaser(void);
void DrawBossUI(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[BOSS_TEX_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[BOSS_TEX_MAX] = 
{
	"data/TEXTURE/Enemy/Boss/BossAnim.png",
	"data/TEXTURE/Effect/Particle.png",
	"data/TEXTURE/Effect/LaserSign2.png",
	"data/TEXTURE/Effect/Laser.png",
	"data/TEXTURE/Effect/lightningBall.png",
	"data/TEXTURE/Enemy/Boss/BossUI.png",
	"data/TEXTURE/Effect/fade_white.png",
};

static BOSS				g_Boss;								// ボス			構造体
static BOSS_LASER		g_Laser;							// レーザー		構造体
static BOSS_PROJECTILE	g_Projectile[BOSS_PROJECTILE_MAX];	// 電気玉		構造体
static BOSS_PARTICLE	g_Particle[BOSS_PARTICLE_MAX];		// パーティクル	構造体


static BOOL		g_Load = FALSE;	// 初期化を行ったかのフラグ


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBoss(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < BOSS_TEX_MAX; i++)
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

	BG* bg = GetBG();
	// エネミー構造体の初期化
	{
		g_Boss.spawned	= FALSE;
		g_Boss.alive	= FALSE;
		g_Boss.shoot	= FALSE;
		g_Boss.w		= TEXTURE_WIDTH;
		g_Boss.h		= TEXTURE_HEIGHT;
		g_Boss.pos		= XMFLOAT3((bg->w / 2), bg->h - g_Boss.h / 2 - 100.0f, 0.0f);
		g_Boss.rot		= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss.scl		= XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Boss.texNo	= BOSS_TEX_BOSS_SPRITE_SHEET;

		g_Boss.currentSprite = 0;
		g_Boss.countAnim	 = 0;
		g_Boss.currentAnim   = BOSS_ANIM_CHASE;

		g_Boss.moveStartPos  = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss.moveEndPos	 = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Boss.HP			 = HP_MAX;

		g_Boss.damaged		 = FALSE;
		g_Boss.dmgTimeCnt	 = 0;

		g_Boss.enemyColor	 = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Boss.phase		 = 1;
		g_Boss.attacks		 = 0;
		g_Boss.attackCnt	 = 0;
		g_Boss.attackTime	 = PHASE_1_ATTACK_DURATION_LASER;
		g_Boss.attackTimeCnt = 0.0f;

		g_Boss.projectileNum = PHASE_1_NUMBER_OF_ATTACKS_SHOTS;

		g_Boss.state		 = BOSS_STATE_CHASE;

		g_Boss.attackPos	 = XMFLOAT3(bg->w / 2, bg->h / 2, 0.0f);

		g_Boss.hpUIColor	 = XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f);
	}

	//電気玉の初期化
	for (int i = 0; i < BOSS_PROJECTILE_MAX; i++)
	{

		g_Projectile[i].use			  = FALSE;
		g_Projectile[i].pos			  = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Projectile[i].w			  = 80.0f;
		g_Projectile[i].h			  = 80.0f;
		g_Projectile[i].texNo		  = BOSS_TEX_LIGHTNING_BALL;
		g_Projectile[i].countAnim	  = 0;
		g_Projectile[i].currentSprite = 0;

		g_Projectile[i].moveSpeed	  = BOSS_PROJECTILE_SPEED;
		g_Projectile[i].moveDir		  = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Projectile[i].color		  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_Projectile[i].angle		  = 0.0f;

	}


	//レーザーの初期化
	g_Laser.use		= FALSE;
	g_Laser.pos		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Laser.w		= THICK_LASER;
	g_Laser.h		= bg->h;
	g_Laser.texNo	= BOSS_TEX_LASER_SIGN;
	g_Laser.color	= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//パーティクルの初期化
	for (int i = 0; i < BOSS_PARTICLE_MAX; i++)
	{
		g_Particle[i].use	= FALSE;
		g_Particle[i].pos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Particle[i].w		= PARTICLE_SIZE;
		g_Particle[i].h		= PARTICLE_SIZE;
		g_Particle[i].texNo = BOSS_TEX_PARTIBLE;

		g_Particle[i].moveSpeed   = BOSS_PROJECTILE_SPEED;
		g_Particle[i].moveDir	  = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Particle[i].color		  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Particle[i].lifeTime	  = 0;
		g_Particle[i].lifeTimeCnt = 0;
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBoss(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < BOSS_TEX_MAX; i++)
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
void UpdateBoss(void)
{
	
	UpdateBossProjectile();


	if (g_Boss.spawned == FALSE) return;
	// 生きてるエネミーだけ処理をする
	if (g_Boss.alive == TRUE)
	{
		PLAYER* player = GetPlayer();
		// PLAYERとの当たり判定
		{
			// エネミーの数分当たり判定を行う
			for (int j = 0; j < PLAYER_MAX; j++)
			{
				// 生きてるエネミーと当たり判定をする
				if (player[j].alive == TRUE && player[j].invincible == FALSE)
				{
					BOOL ans;

					//BOSS のあたり判定
					{
						ans = CollisionBB(g_Boss.pos,		g_Boss.w,		g_Boss.h,
										  player[j].pos,	player[j].w,	player[j].h);

						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							PlayerDamaged(g_Boss.pos);
							//Retroceso
						}
					}
					
					//Laser のあたり判定
					{
						if (g_Laser.texNo == BOSS_TEX_LASER)
						{
							ans = CollisionBB(g_Laser.pos,		g_Laser.w,		g_Laser.h,
											  player[j].pos,	player[j].w,	player[j].h);

							if (ans == TRUE)
							{
								// 当たった時の処理
								PlayerDamaged(g_Boss.pos);
								PlaySound(SOUND_LABEL_SE_ElectricShock);
							}
						}
					}
					
					
					//PROJECTILE のあたり判定
					{
						for (int i = 0; i < g_Boss.projectileNum; i++)
						{
							if (g_Projectile[i].use == TRUE)
							{
								ans = CollisionBB(g_Projectile[i].pos,  g_Projectile[i].w / 2,  g_Projectile[i].h / 2,
												  player[j].pos,		player[j].w,			player[j].h);

								if (ans == TRUE)
								{
									// 当たった時の処理
									PlayerDamaged(g_Boss.pos);
									PlaySound(SOUND_LABEL_SE_ElectricShock);
								}
							}
						}
					}
				}
			}
		}

		//エネミーの状態更新
		{
			switch (g_Boss.state)
			{
			case BOSS_STATE_CHASE:
			{
				//CHASE状態が始めると電気玉が出てくる
				{
					//電気玉数の設定
					switch (g_Boss.phase)
					{
					case 1:
						g_Boss.projectileNum = PHASE_1_NUMBER_OF_ATTACKS_SHOTS;
						break;

					case 2:
						g_Boss.projectileNum = PHASE_2_NUMBER_OF_ATTACKS_SHOTS;
						break;

					case 3:
						g_Boss.projectileNum = PHASE_3_NUMBER_OF_ATTACKS_SHOTS;
						break;
					}

					//電気玉の設定
					if (g_Projectile[g_Boss.projectileNum - 1].use == FALSE)
					{
						SetBossProjectile();
						break;
					}
				}

				//CHASEの開始点と終了点の設定
				if (g_Boss.attackTimeCnt == 0)			//CHASE SET
				{
					g_Boss.moveStartPos = g_Boss.pos;
					g_Boss.moveEndPos   = player[0].pos;
				}
				
				//移動処理
				double t = g_Boss.attackTimeCnt / g_Boss.attackTime;
				t = easeInOutQuad(t);

				g_Boss.pos.x = (float)((double)g_Boss.moveStartPos.x + ((double)g_Boss.moveEndPos.x - (double)g_Boss.moveStartPos.x) * t);
				g_Boss.pos.y = (float)((double)g_Boss.moveStartPos.y + ((double)g_Boss.moveEndPos.y - (double)g_Boss.moveStartPos.y) * t);

				g_Boss.attackTimeCnt += 1.0f;
				

				//終了点についたら、リセット
				if (t >= 1)
				{
					g_Boss.attackTimeCnt = 0;
					g_Boss.attackCnt++;

					//CHASEが終わったら、SHOOTING状態を初期化
					if (g_Boss.attackCnt >= g_Boss.attacks)
					{
						g_Boss.state		= BOSS_STATE_SHOOTING;
						g_Boss.attackTime	= 120;
						g_Boss.attackCnt	= 0;

						g_Boss.moveStartPos = g_Boss.pos;
						g_Boss.moveEndPos   = g_Boss.attackPos;
					}	
				}
				
			}
				break;

			case BOSS_STATE_SHOOTING:
			{
				//最初に攻撃場所まで行きます
				if (g_Boss.shoot == FALSE)
				{
					//移動処理
					double t = g_Boss.attackTimeCnt / g_Boss.attackTime;
					
					t = easeInOutQuad(t);

					g_Boss.pos.x = (float)((double)g_Boss.moveStartPos.x + ((double)g_Boss.moveEndPos.x - (double)g_Boss.moveStartPos.x) * t);
					g_Boss.pos.y = (float)((double)g_Boss.moveStartPos.y + ((double)g_Boss.moveEndPos.y - (double)g_Boss.moveStartPos.y) * t);

					g_Boss.attackTimeCnt += 1.0f;

					//攻撃場所についたら
					if (g_Boss.attackTimeCnt >= g_Boss.attackTime)
					{
						//発射設定
						g_Boss.shoot		 = TRUE;
						g_Boss.currentAnim   = BOSS_ANIM_SHOOTING;
						g_Boss.attackTimeCnt = 0.0f;

						//発射速度の設定
						switch (g_Boss.phase)
						{
						case 1:
							g_Boss.attackTime = PHASE_1_ATTACK_DURATION_SHOTS;
							break;

						case 2:
							g_Boss.attackTime = PHASE_2_ATTACK_DURATION_SHOTS;
							break;

						case 3:
							g_Boss.attackTime = PHASE_3_ATTACK_DURATION_SHOTS;
							break;
						}
					}
					break;
					
				}
				
				
				//電気玉がなくなったら、LASER状態を設定する
				if (g_Projectile[g_Boss.projectileNum - 1].use == FALSE)
				{
					//攻撃速度と攻撃回数を設定
					switch (g_Boss.phase)
					{
					case 1:
						g_Boss.attackTime = PHASE_1_ATTACK_DURATION_LASER;
						g_Boss.attacks	  = PHASE_1_NUMBER_OF_ATTACKS_LASER;
						break;

					case 2:
						g_Boss.attackTime = PHASE_2_ATTACK_DURATION_LASER;
						g_Boss.attacks    = PHASE_2_NUMBER_OF_ATTACKS_LASER;
						break;

					case 3:
						g_Boss.attackTime = PHASE_3_ATTACK_DURATION_LASER;
						g_Boss.attacks    = PHASE_3_NUMBER_OF_ATTACKS_LASER;
						break;
					}

					//LASER状態の設定
					g_Boss.state		 = BOSS_STATE_LASER;
					g_Boss.currentAnim	 = BOSS_ANIM_LASER;
					g_Boss.attackTimeCnt = 0;
					g_Boss.shoot		 = FALSE;
					g_Laser.use			 = TRUE;
				}
			}
				break;

			case BOSS_STATE_LASER:
			{
				//レザーの設定
				if (g_Boss.attackTimeCnt == 0)
				{
					//レザーのX座標をプレイヤーのの位置に設定
					g_Laser.pos = XMFLOAT3(player[0].pos.x, SCREEN_CENTER_Y, 0.0f);
					PlaySound(SOUND_LABEL_SE_ChargeBeam);

					//最初の攻撃と同時にプラットフォームが展開します
					if (g_Boss.attackCnt == 0)
					{
						BG* bg = GetBG();
						SetPlatform(XMFLOAT3(bg->w / 2 - 250.0f, SCREEN_CENTER_Y + 80.0f, 0.0f));
						SetPlatform(XMFLOAT3(bg->w / 2 + 250.0f, SCREEN_CENTER_Y + 80.0f, 0.0f));
					}
				}

				//レザーのポリゴンの幅を調整する処理
				{
					double t = g_Boss.attackTimeCnt / g_Boss.attackTime;

					t = easeInOutQuad(t);

					g_Laser.w = (float)((double)THIN_LASER + ((double)THICK_LASER - (double)THIN_LASER) * t);

					if (g_Laser.texNo == BOSS_TEX_LASER_SIGN)g_Boss.attackTimeCnt += 1.0f;
					if (g_Laser.texNo == BOSS_TEX_LASER)	 g_Boss.attackTimeCnt -= 1.0f;
				}
					
				//レザーとレザーの兆しの切り替え
				if (g_Boss.attackTimeCnt == g_Boss.attackTime)
				{
					if (g_Laser.texNo == BOSS_TEX_LASER_SIGN)
					{
						g_Laser.texNo = BOSS_TEX_LASER;						
						PlaySound(SOUND_LABEL_SE_LaserSound);	//Laser sound
					}
				}
				else if (g_Boss.attackTimeCnt == 0)
				{
					if (g_Boss.attackCnt < g_Boss.attacks)
					{
						g_Laser.texNo = BOSS_TEX_LASER_SIGN;
						g_Boss.attackCnt++;
					}
					else 
					{
						//全部のレザーを打ったら、CHASEの設定をします
						switch (g_Boss.phase)
						{
						case 1:
							g_Boss.attackTime = PHASE_1_ATTACK_DURATION_CHASE;
							g_Boss.attacks = PHASE_1_NUMBER_OF_ATTACKS_CHASE;
							break;

						case 2:
							g_Boss.attackTime = PHASE_2_ATTACK_DURATION_CHASE;
							g_Boss.attacks = PHASE_2_NUMBER_OF_ATTACKS_CHASE;
							break;

						case 3:
							g_Boss.attackTime = PHASE_3_ATTACK_DURATION_CHASE;
							g_Boss.attacks = PHASE_3_NUMBER_OF_ATTACKS_CHASE;
							break;
						}

						g_Laser.use = FALSE;
						g_Laser.texNo = BOSS_TEX_LASER_SIGN;
						g_Boss.state = BOSS_STATE_CHASE;
						g_Boss.currentAnim = BOSS_ANIM_CHASE;
						g_Boss.attackCnt = 0;
						ResetPlatform();
					}
				}
			}
				break;
			}

		}



		//エネミーが傷付いた
		
		if (g_Boss.damaged == TRUE)
		{
			g_Boss.dmgTimeCnt++;

			if (g_Boss.dmgTimeCnt >= DAMAGE_TIME)
			{
				g_Boss.damaged    = FALSE;
				g_Boss.dmgTimeCnt = 0;
			}
		}
		


	}
	else
	{
		DespawnBoss();
	}
	


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBoss(void)
{
	if (g_Boss.spawned == FALSE) return;

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

	//Particles
	if (g_Boss.alive == FALSE)
	{
		for (int i = 0; i < BOSS_PARTICLE_MAX; i++)
		{
			if (g_Particle[i].use == TRUE)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Particle[i].texNo]);

				//エネミーの位置やテクスチャー座標を反映
				float px = g_Particle[i].pos.x - bg->pos.x;	// エネミーの表示位置X
				float py = g_Particle[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
				float pw = g_Particle[i].w;					// エネミーの表示幅
				float ph = g_Particle[i].h;					// エネミーの表示高さ

				float tw = 1.0f;		// テクスチャの幅
				float th = 1.0f;		// テクスチャの高さ
				float tx = 0.0f;		// テクスチャの左上X座標
				float ty = 0.0f;		// テクスチャの左上Y座標


				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					g_Particle[i].color);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}



	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Boss.texNo]);

	//スプライトの管理処理
	if (g_Boss.alive == FALSE)
	{
		g_Boss.countAnim++;
		if (g_Boss.countAnim == ANIM_WAIT)
		{
			g_Boss.countAnim = 0;

			if (g_Boss.currentAnim == BOSS_ANIM_DEATH1)
			{
				g_Boss.currentAnim = BOSS_ANIM_DEATH2;
			}
			else {
				g_Boss.currentAnim = BOSS_ANIM_DEATH1;
			}
		}
	}
	

	//エネミーの位置やテクスチャー座標を反映
	float px = g_Boss.pos.x - bg->pos.x;	// エネミーの表示位置X
	float py = g_Boss.pos.y - bg->pos.y;	// エネミーの表示位置Y
	float pw = g_Boss.w;					// エネミーの表示幅
	float ph = g_Boss.h;					// エネミーの表示高さ

	float tw = 1.0f;						// テクスチャの幅
	float th = 1.0f / BOSS_ANIM_MAX;		// テクスチャの高さ
	float tx = tw * g_Boss.currentSprite;	// テクスチャの左上X座標
	float ty = th * g_Boss.currentAnim;		// テクスチャの左上Y座標


	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					g_Boss.enemyColor);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	if (g_Boss.alive == TRUE)
	{
		DrawBossProjectiles();

		DrawBossLaser();
	}
	

	DrawBossUI();
}


//=============================================================================
// BOSSの体力UIの描画処理
//=============================================================================
void DrawBossUI(void)
{
	BG* bg = GetBG();

	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BOSS_TEX_BOSS_UI]);

		//エネミーの位置やテクスチャー座標を反映
		float px = SCREEN_WIDTH - 40.0f;	// エネミーの表示位置X
		float py = 35.0f;					// エネミーの表示位置Y
		float pw = 500.0f;					// エネミーの表示幅
		float ph = 50.0f;					// エネミーの表示高さ

		float tw = 1.0f;					// テクスチャの幅
		float th = 1.0f;					// テクスチャの高さ
		float tx = 0.0f;					// テクスチャの左上X座標
		float ty = 0.0f;					// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteRightTop(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[BOSS_TEX_BOSS_UI_HP]);

		//エネミーの位置やテクスチャー座標を反映
		float px = SCREEN_WIDTH - 47.0f;			// エネミーの表示位置X
		float py = 42;								// エネミーの表示位置Y
		float pw = 480.0f / HP_MAX * g_Boss.HP;		// エネミーの表示幅
		float ph = 10.0f;							// エネミーの表示高さ

		float tw = 1.0f;							// テクスチャの幅
		float th = 1.0f;							// テクスチャの高さ
		float tx = 0.0f;							// テクスチャの左上X座標
		float ty = 0.0f;							// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteRTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, g_Boss.hpUIColor);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
BOSS GetBoss(void)
{
	return g_Boss;
}


//=============================================================================
// BOSSのダメージ設定
// 引数: damage (BOSSが受けるダメージ)
//=============================================================================
void BossDamaged(float damage)
{
	g_Boss.damaged = TRUE;
	g_Boss.HP -= damage;
	PlaySound(SOUND_LABEL_SE_bloodyBlade);

	float phaseHP = HP_MAX / 3;

	if (g_Boss.HP <= 0)
	{
		g_Boss.alive = FALSE;
	}
	else if (g_Boss.HP <= phaseHP)
	{
		g_Boss.phase	 = 3;
		g_Boss.hpUIColor = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	}
	else if (g_Boss.HP <= phaseHP * 2)
	{
		g_Boss.phase	 = 2;
		g_Boss.hpUIColor = XMFLOAT4(0.8f, 0.8f, 0.0f, 1.0f);
	}
}


//=============================================================================
// BOSSの出現
// 引数: damage (BOSSが受けるダメージ)
//=============================================================================
void SpawnBoss(XMFLOAT3 spawnPos)
{
	g_Boss.spawned	= TRUE;
	g_Boss.alive	= TRUE;
	g_Boss.pos		= spawnPos;
}


//=============================================================================
// BOSSの消滅処理
//=============================================================================
void DespawnBoss(void)
{
	g_Boss.dmgTimeCnt++;
	SetBossPartible(g_Boss.pos);

	if (g_Boss.dmgTimeCnt >= DESPAWN_TIME)
	{
		if (g_Boss.w > 10)
		{
			g_Boss.w -= 1.0f;
			g_Boss.h -= 1.2f;
			return;
		}

		g_Boss.spawned		 = FALSE;
		g_Boss.alive		 = FALSE;
		g_Boss.pos			 = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss.w			 = TEXTURE_WIDTH;
		g_Boss.h			 = TEXTURE_HEIGHT;

		g_Boss.currentSprite = 0;
		g_Boss.currentAnim	 = BOSS_ANIM_CHASE;

		g_Boss.HP			 = HP_MAX;
		g_Boss.damaged		 = FALSE;
		g_Boss.dmgTimeCnt	 = 0;
							 
		g_Boss.enemyColor	 = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
							 
		g_Boss.phase		 = 1;
		g_Boss.attackCnt	 = 0;
		g_Boss.state		 = BOSS_STATE_CHASE;

		RoundKill();
	}
}

//=============================================================================
// パーティクルの設定
//=============================================================================
void SetBossPartible(XMFLOAT3 pos)
{
	for (int i = 0; i < BOSS_PARTICLE_MAX; i++)
	{
		if (g_Particle[i].use == FALSE)
		{
			g_Particle[i].use	= TRUE;
			g_Particle[i].pos	= pos;
			g_Particle[i].texNo = BOSS_TEX_PARTIBLE;

			float angle = (float)(rand() % 628) / 100;
			g_Particle[i].moveDir.x   = cosf(angle);	//angleの方向へ移動
			g_Particle[i].moveDir.y   = sinf(angle);	//angleの方向へ移動

			g_Particle[i].lifeTime	  = BOSS_PARTICLE_MAX;
			g_Particle[i].lifeTimeCnt = 0;

			g_Particle[i].color		  = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		}		
	}

	for (int i = 0; i < BOSS_PARTICLE_MAX; i++)
	{
		if (g_Particle[i].use == TRUE)
		{
			g_Particle[i].lifeTimeCnt++;

			g_Particle[i].pos.x	  += g_Particle[i].moveDir.x * g_Particle[i].moveSpeed;
			g_Particle[i].pos.y	  += g_Particle[i].moveDir.y * g_Particle[i].moveSpeed;

			g_Particle[i].color.w -= 1.0f/ BOSS_PARTICLE_MAX;
			g_Particle[i].color.z += 1.0f/ BOSS_PARTICLE_MAX;

			if (g_Particle[i].lifeTimeCnt == g_Particle[i].lifeTime)
			{
				g_Particle[i].use = FALSE;
			}
		}

	}
}

//=============================================================================
// スローイン・スローアウト
// 引数: t (線形補間のt変数)
//=============================================================================
double easeInOutQuad(double t) 
{
	return t < 0.5 ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2;
}

//=============================================================================
// 電気玉の設定
//=============================================================================
void SetBossProjectile(void)
{
	for (int i = 0; i < g_Boss.projectileNum; i++)
	{
		if (g_Projectile[i].use == FALSE)
		{
			if (i == 0) 
			{
				g_Projectile[i].use   = TRUE;
				g_Projectile[i].angle = 0.0f;
				return;
			}
			else
			{
				if (g_Projectile[0].angle > (XM_2PI / g_Boss.projectileNum) * i)
				{
					g_Projectile[i].use   = TRUE;
					g_Projectile[i].angle = 0.0f;
					return;
				}
			}
			
		}
	}
}

//=============================================================================
// 電気玉の処理
//=============================================================================
void UpdateBossProjectile(void)
{
	if (g_Boss.shoot == TRUE)//SHOOT 状態
	{
		if (g_Boss.attackTimeCnt < g_Boss.attackTime)
		{
			g_Boss.attackTimeCnt++;
			
		}

		for (int i = 0; i < g_Boss.projectileNum; i++)
		{
			if (g_Projectile[i].use == FALSE) continue;
			
			if (g_Projectile[i].moveDir.x == 0.0f)		//Set projectile direction
			{
				if (g_Boss.attackTimeCnt >= g_Boss.attackTime)
				{
					g_Boss.attackTimeCnt = 0;

					PLAYER* player = GetPlayer();

					XMVECTOR BossPos = XMLoadFloat3(&g_Projectile[i].pos);
					XMVECTOR vec = (XMLoadFloat3(&player[0].pos) - BossPos);

					float angle = atan2f(vec.m128_f32[1], vec.m128_f32[0]);

					g_Projectile[i].moveDir.x = cosf(angle);
					g_Projectile[i].moveDir.y = sinf(angle);

					PlaySound(SOUND_LABEL_SE_MagicCast);
				}
				
			}
			else										//Update Projectile position
			{
				g_Projectile[i].pos.x += g_Projectile[i].moveDir.x * g_Projectile[i].moveSpeed;
				g_Projectile[i].pos.y += g_Projectile[i].moveDir.y * g_Projectile[i].moveSpeed;


				//画面端の当たり判定
				{
					BG* bg = GetBG();
					if (g_Projectile[i].pos.x < (-g_Projectile[i].w) ||			//左
						g_Projectile[i].pos.x >(bg->w + g_Projectile[i].w) ||		//右
						g_Projectile[i].pos.y < (-g_Projectile[i].h) ||			//上
						g_Projectile[i].pos.y >(bg->h + g_Projectile[i].h))		//下
					{
						g_Projectile[i].use		= FALSE;
						g_Projectile[i].moveDir = XMFLOAT3(0.0f, 0.0f, 0.0f);
					}
				}
			}
		}
	}
	else//CHASE 状態
	{
		for (int i = 0; i < g_Boss.projectileNum; i++)
		{
			if (g_Projectile[i].use == TRUE)
			{
				g_Projectile[i].pos.x = g_Boss.pos.x + cosf(g_Projectile[i].angle) * 100.0f;
				g_Projectile[i].pos.y = g_Boss.pos.y + sinf(g_Projectile[i].angle) * 100.0f;

				g_Projectile[i].angle += 0.05f;
			}
		}
	}
	

	
}

//=============================================================================
// 電気玉の描画
//=============================================================================
void DrawBossProjectiles(void)
{
	BG* bg = GetBG();

	for (int i = 0; i < g_Boss.projectileNum; i++)
	{
		if (g_Projectile[i].use == TRUE)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Projectile[i].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_Projectile[i].pos.x - bg->pos.x;	// エネミーの表示位置X
			float py = g_Projectile[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
			float pw = g_Projectile[i].w;					// エネミーの表示幅
			float ph = g_Projectile[i].h;					// エネミーの表示高さ

			float tw = 1.0f / 4;		// テクスチャの幅
			float th = 1.0f;		// テクスチャの高さ
			float tx = tw * g_Projectile[i].currentSprite;		// テクスチャの左上X座標
			float ty = 0.0f;		// テクスチャの左上Y座標


			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				g_Projectile[i].color);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);


			g_Projectile[i].countAnim++;
			if (g_Projectile[i].countAnim >= ANIM_WAIT)
			{
				g_Projectile[i].countAnim = 0;
				g_Projectile[i].currentSprite = (g_Projectile[i].currentSprite + 1) % 4;
			}
		}
	}
	
}


//=============================================================================
// レザーの描画
//=============================================================================
void DrawBossLaser(void)
{
	BG* bg = GetBG();

	if (g_Laser.use == TRUE)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Laser.texNo]);

		//エネミーの位置やテクスチャー座標を反映
		float px = g_Laser.pos.x - bg->pos.x;	// エネミーの表示位置X
		float py = g_Laser.pos.y - bg->pos.y;	// エネミーの表示位置Y
		float pw = g_Laser.w;					// エネミーの表示幅
		float ph = g_Laser.h;					// エネミーの表示高さ

		float tw = 1.0f;		// テクスチャの幅
		float th = 1.0f;		// テクスチャの高さ
		float tx = 0.0f;		// テクスチャの左上X座標
		float ty = 0.0f;		// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			g_Laser.color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}