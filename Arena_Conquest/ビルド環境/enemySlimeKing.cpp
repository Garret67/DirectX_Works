//=============================================================================
//
// エネミー処理 [enemySlimeKing.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#include "enemySlimeKing.h"
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
#define TEXTURE_HEIGHT				(60.0f)	// 
#define TEXTURE_MAX					(4)		// テクスチャの数

#define TEXTURE_ANIM_SPRITES_MAX	(5)		// 1つのアニメーションはスプライト何枚（X)
//#define TEXTURE_ANIM_MAX			(3)		// アニメーションはいくつ			  （Y)
//#define ANIM_WAIT					(20)		// アニメーションの切り替わるWait値

#define DAMAGE_TIME					(30)
#define KNOCKBACK_TIME				(15)
#define DESPAWN_TIME				(120)
#define HP_MAX						(300.0f)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SlimeKingAnimationUpdate(void);

void DespawnSlimeKing(void);

void SetSlimeKingPartible(XMFLOAT3 pos);

void DrawSlimeKingUI(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Enemy/SlimeKing/SlimeKing_anim.png",
	"data/TEXTURE/Enemy/SlimeKing/SlimeKing_UI.png",
	"data/TEXTURE/Effect/Particle.png",
	"data/TEXTURE/Effect/fade_white.png",
};

static SLIME_KING			g_SlimeKing;		// エネミー構造体
static SLIME_KING_PARTICLE	g_Particle[SLIME_KING_PARTICLE_MAX];		// エネミー構造体

int animWait;
XMFLOAT4 KingSlimeHP_UI_Color;

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSlimeKing(void)
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

	BG* bg = GetBG();
	// エネミー構造体の初期化
	
	g_SlimeKing.spawned = FALSE;
	g_SlimeKing.alive = FALSE;
	g_SlimeKing.w = TEXTURE_WIDTH;
	g_SlimeKing.h = TEXTURE_HEIGHT;
	g_SlimeKing.pos = XMFLOAT3((bg->w / 2), bg->h - g_SlimeKing.h / 2 - 100.0f, 0.0f);
	g_SlimeKing.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_SlimeKing.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_SlimeKing.texNo = 0;

	g_SlimeKing.currentSprite = 0;
	g_SlimeKing.currentAnim = SLIME_KING_ANIM_WAKE_UP;

	g_SlimeKing.moveSpeed = 2.0f;		// 移動量
	g_SlimeKing.moveDir = SLIME_KING_DIR_LEFT;

	g_SlimeKing.HP = HP_MAX;

	g_SlimeKing.damaged = FALSE;
	g_SlimeKing.dmgTimeCnt = 0;

	g_SlimeKing.enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	g_SlimeKing.attackType = SLIME_KING_ATTACK_HORIZONTAL;

	g_SlimeKing.phase = 1;
	g_SlimeKing.portal = 0;
	g_SlimeKing.state = SLIME_KING_STATE_IDLE;
	g_SlimeKing.stateTime = 180;


	for (int i = 0; i < SLIME_KING_PARTICLE_MAX; i++)
	{
		
		g_Particle[i].use = FALSE;
		g_Particle[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Particle[i].w = 50.0f;
		g_Particle[i].h = 50.0f;
		g_Particle[i].texNo = 2;

		g_Particle[i].moveSpeed = 10.0f;
		g_Particle[i].moveDir = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Particle[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Particle[i].lifeTime = 0;
		g_Particle[i].lifeTimeCnt = 0;
	}
	

	animWait = 20;

	KingSlimeHP_UI_Color = XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSlimeKing(void)
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
void UpdateSlimeKing(void)
{
	if (g_SlimeKing.spawned == FALSE) return;
	// 生きてるエネミーだけ処理をする
	if (g_SlimeKing.alive == TRUE)
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
					float slimeW;
					float slimeH;

					if (g_SlimeKing.state == SLIME_KING_STATE_RUSH ||
						g_SlimeKing.state == SLIME_KING_STATE_PREPARATION)
					{
						slimeW = g_SlimeKing.w * 0.5f;
						slimeH = g_SlimeKing.h * 0.7f;
					}
					else
					{
						slimeW = g_SlimeKing.w;
						slimeH = g_SlimeKing.h * 0.5f;
					}

					BOOL ans = CollisionBB(g_SlimeKing.pos, slimeW, slimeH,
						player[j].pos, player[j].w, player[j].h);
					// 当たっている？
					if (ans == TRUE)
					{
						// 当たった時の処理
						PlayerDamaged(g_SlimeKing.pos);
						//Retroceso
					}
				}
			}
		}

		//SLIME KINGエネミーのパトロール
		{


			switch (g_SlimeKing.state)
			{
			case SLIME_KING_STATE_IDLE:
				g_SlimeKing.stateTimeCnt++;
				if (g_SlimeKing.stateTimeCnt > 120)
				{
					g_SlimeKing.state = SLIME_KING_STATE_PREPARATION;
					g_SlimeKing.stateTimeCnt = 0;
					g_SlimeKing.countAnim = 0;
					g_SlimeKing.currentSprite = 0;
				}
				break;

			case SLIME_KING_STATE_PREPARATION:


				g_SlimeKing.moveDir = SLIME_KING_DIR_LEFT;
				g_SlimeKing.moveSpeed = 0.3f;

				g_SlimeKing.pos.x += g_SlimeKing.moveSpeed;
				g_SlimeKing.stateTimeCnt++;

				if (g_SlimeKing.stateTimeCnt > 120)
				{
					g_SlimeKing.state = SLIME_KING_STATE_RUSH;
					g_SlimeKing.stateTimeCnt = 0;
				}
				break;

			case SLIME_KING_STATE_RUSH:

				if (g_SlimeKing.attackType == SLIME_KING_ATTACK_HORIZONTAL)
				{
					g_SlimeKing.pos.x -= g_SlimeKing.moveSpeed;
				}
				else if (g_SlimeKing.attackType == SLIME_KING_ATTACK_VERTICAL)
				{
					g_SlimeKing.pos.y += g_SlimeKing.moveSpeed;
				}



				//速度調整
				if (g_SlimeKing.portal > 3)
				{
					BG* bg = GetBG();
					if (g_SlimeKing.pos.x < bg->w / 2)
					{
						if ((g_SlimeKing.moveSpeed) > 0.0f)
						{
							g_SlimeKing.moveSpeed -= 0.1f * g_SlimeKing.stateTimeCnt;
							g_SlimeKing.stateTimeCnt++;
						}
						else
						{
							g_SlimeKing.moveSpeed = 0.0f;
							g_SlimeKing.state = SLIME_KING_STATE_TIRED;
							g_SlimeKing.stateTimeCnt = 0;
							g_SlimeKing.portal = 0;
							g_SlimeKing.countAnim = 0;
							g_SlimeKing.currentSprite = 0;
						}
					}

				}
				else
				{
					if (fabs(g_SlimeKing.moveSpeed) < 15.0f)
					{
						g_SlimeKing.moveSpeed += 0.1f * g_SlimeKing.stateTimeCnt;
						g_SlimeKing.stateTimeCnt++;
					}
					else
					{
						g_SlimeKing.stateTimeCnt = 0;
					}
				}



				break;

			case SLIME_KING_STATE_TIRED:
				g_SlimeKing.stateTimeCnt++;
				if (g_SlimeKing.stateTimeCnt > 120)
				{
					g_SlimeKing.state = SLIME_KING_STATE_IDLE;
					g_SlimeKing.stateTimeCnt = 0;
					g_SlimeKing.countAnim = 0;
					g_SlimeKing.currentSprite = 0;
				}
				break;
			}

		}



		//エネミーが傷付いた
		{
			if (g_SlimeKing.damaged == TRUE)
			{
				g_SlimeKing.dmgTimeCnt++;

				if (g_SlimeKing.dmgTimeCnt >= DAMAGE_TIME)
				{
					g_SlimeKing.damaged = FALSE;
					g_SlimeKing.dmgTimeCnt = 0;

					//g_SlimeKing.enemyColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
				}
			}


		}

		//重力
		//g_SlimeKing.pos.y += 8.0f;




		//画面端の当たり判定
		if (g_SlimeKing.state == SLIME_KING_STATE_RUSH)
		{// 自分の大きさを考慮して画面外か判定している
			BG* bg = GetBG();
			PLAYER* player = GetPlayer();

			if (g_SlimeKing.pos.x < (-g_SlimeKing.w) ||				// 左
				g_SlimeKing.pos.y >(bg->h + g_SlimeKing.h))			// 下
			{

				switch (g_SlimeKing.phase)
				{
				case 1:
					g_SlimeKing.attackType = SLIME_KING_ATTACK_HORIZONTAL;
					g_SlimeKing.pos.x = bg->w + g_SlimeKing.w / 2;
					g_SlimeKing.pos.y = bg->h - g_SlimeKing.h / 2 - 100.0f;

					g_SlimeKing.portal++;
					break;

				case 2:
					if (g_SlimeKing.portal >= 3)
					{
						g_SlimeKing.attackType = SLIME_KING_ATTACK_HORIZONTAL;
						g_SlimeKing.pos.x = bg->w + g_SlimeKing.w / 2;
						g_SlimeKing.pos.y = bg->h - g_SlimeKing.h / 2 - 100.0f;
					}
					else
					{
						g_SlimeKing.attackType = SLIME_KING_ATTACK_VERTICAL;
						g_SlimeKing.pos.y = -500.0f;
						g_SlimeKing.pos.x = player[0].pos.x;
					}

					g_SlimeKing.portal++;
					break;

				case 3:
					if (g_SlimeKing.portal >= 3)
					{
						g_SlimeKing.pos.x = bg->w + g_SlimeKing.w / 2;
						g_SlimeKing.pos.y = bg->h - g_SlimeKing.h / 2 - 100.0f;
						g_SlimeKing.attackType = SLIME_KING_ATTACK_HORIZONTAL;
					}
					else
					{
						int nextAttack = rand() % 2;

						if (nextAttack == 0)
						{
							g_SlimeKing.pos.x = bg->w + g_SlimeKing.w / 2;
							g_SlimeKing.pos.y = bg->h - g_SlimeKing.h / 2 - 100.0f;
							g_SlimeKing.attackType = SLIME_KING_ATTACK_HORIZONTAL;
						}
						else if (nextAttack == 1)
						{
							g_SlimeKing.pos.y = -500.0f;
							g_SlimeKing.pos.x = player[0].pos.x;
							g_SlimeKing.attackType = SLIME_KING_ATTACK_VERTICAL;
						}
					}

					g_SlimeKing.portal++;
					break;
				}
			}


		}
	}
	else
	{
		DespawnSlimeKing();
	}
	

	// アニメーション
	SlimeKingAnimationUpdate();


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSlimeKing(void)
{
	if (g_SlimeKing.spawned == FALSE) return;

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
	if (g_SlimeKing.alive == FALSE)
	{
		for (int i = 0; i < SLIME_KING_PARTICLE_MAX; i++)
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
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_SlimeKing.texNo]);

	//エネミーの位置やテクスチャー座標を反映
	float px = g_SlimeKing.pos.x - bg->pos.x;	// エネミーの表示位置X
	float py = g_SlimeKing.pos.y - bg->pos.y;	// エネミーの表示位置Y
	float pw = g_SlimeKing.w;					// エネミーの表示幅
	float ph = g_SlimeKing.h;					// エネミーの表示高さ

	float tw = 1.0f / TEXTURE_ANIM_SPRITES_MAX;		// テクスチャの幅
	float th = 1.0f / SLIME_KING_ANIM_MAX;				// テクスチャの高さ
	float tx = tw * g_SlimeKing.currentSprite;	// テクスチャの左上X座標
	float ty = th * g_SlimeKing.currentAnim;	// テクスチャの左上Y座標

	if (g_SlimeKing.moveDir == SLIME_KING_DIR_RIGHT)
	{
		tx += tw;
		tw *= -1.0f;
	}

	ty += 0.01f;
		


	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					g_SlimeKing.enemyColor);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
	

	DrawSlimeKingUI();

}


//=============================================================================
// Enemy構造体の先頭アドレスを取得
//=============================================================================
SLIME_KING GetSlimeKing(void)
{
	return g_SlimeKing;
}


void DrawSlimeKingUI(void)
{
	BG* bg = GetBG();

	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		//エネミーの位置やテクスチャー座標を反映
		float px = SCREEN_WIDTH- 40.0f;	// エネミーの表示位置X
		float py = 35.0f;		// エネミーの表示位置Y
		float pw = 500.0f;				// エネミーの表示幅
		float ph = 50.0f;				// エネミーの表示高さ

		float tw = 1.0f;				// テクスチャの幅
		float th = 1.0f;				// テクスチャの高さ
		float tx = 0.0f;				// テクスチャの左上X座標
		float ty = 0.0f;				// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteRightTop(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		//エネミーの位置やテクスチャー座標を反映
		float px = SCREEN_WIDTH - 47.0f;	// エネミーの表示位置X
		float py = 42;		// エネミーの表示位置Y
		float pw = 480.0f / HP_MAX * g_SlimeKing.HP;				// エネミーの表示幅
		float ph = 10.0f;				// エネミーの表示高さ

		float tw = 1.0f;				// テクスチャの幅
		float th = 1.0f;				// テクスチャの高さ
		float tx = 0.0f;				// テクスチャの左上X座標
		float ty = 0.0f;				// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteRTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, KingSlimeHP_UI_Color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}


void SlimeKingDamaged(float damage)
{
	g_SlimeKing.damaged = TRUE;
	//g_SlimeKing.enemyColor = XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f);
	g_SlimeKing.HP -= damage;
	PlaySound(SOUND_LABEL_SE_bloodyBlade);
	g_SlimeKing.w += 10;
	g_SlimeKing.h += 6;
	g_SlimeKing.enemyColor.y -= 0.03f;
	g_SlimeKing.enemyColor.z -= 0.03f;

	float phaseHP = HP_MAX / 3;

	if (g_SlimeKing.HP <= 0)
	{
		g_SlimeKing.alive = FALSE;
		//g_SlimeKing.enemyColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		//RoundKill();
		//Death animation
	}
	else if (g_SlimeKing.HP <= phaseHP)
	{
		g_SlimeKing.phase = 3;
		KingSlimeHP_UI_Color = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	}
	else if (g_SlimeKing.HP <= phaseHP * 2)
	{
		g_SlimeKing.phase = 2;
		KingSlimeHP_UI_Color = XMFLOAT4(0.8f, 0.8f, 0.0f, 1.0f);
	}

}

void SlimeKingAnimationUpdate(void)
{
	if (g_SlimeKing.alive == TRUE)
	{
		if (g_SlimeKing.state == SLIME_KING_STATE_IDLE)
		{
			g_SlimeKing.currentAnim = SLIME_KING_ANIM_WAKE_UP;
		}
		else if (g_SlimeKing.state == SLIME_KING_STATE_PREPARATION)
		{
			g_SlimeKing.currentAnim = SLIME_KING_ANIM_PREPARATION;
		}
		else if (g_SlimeKing.state == SLIME_KING_STATE_TIRED)
		{
			g_SlimeKing.currentAnim = SLIME_KING_ANIM_TIRED;
		}
	}
	else
	{
		g_SlimeKing.currentAnim = SLIME_KING_ANIM_DEATH;
		animWait = 5;
	}
	

	g_SlimeKing.countAnim ++;


	if (g_SlimeKing.countAnim > animWait)
	{
		g_SlimeKing.countAnim = 0;

		// パターンの切り替え
		if (g_SlimeKing.alive) //NO ループアニメーション
		{
			if (g_SlimeKing.currentSprite < TEXTURE_ANIM_SPRITES_MAX - 1)
			{
				g_SlimeKing.currentSprite++;
			}
		}
		else //ループアニメーション
		{
			g_SlimeKing.currentSprite = (g_SlimeKing.currentSprite + 1) % TEXTURE_ANIM_SPRITES_MAX;
		}
	}
	
}



void SpawnSlimeKing(XMFLOAT3 spawnPos)
{
	g_SlimeKing.spawned = TRUE;
	g_SlimeKing.alive = TRUE;
	g_SlimeKing.pos = spawnPos;
}



void DespawnSlimeKing(void)
{
	g_SlimeKing.dmgTimeCnt++;
	SetSlimeKingPartible(g_SlimeKing.pos);

	if (g_SlimeKing.dmgTimeCnt >= DESPAWN_TIME)
	{
		if (g_SlimeKing.w > 10)
		{
			g_SlimeKing.w -= 1.0f;
			g_SlimeKing.h -= 0.6f;
			return;
		}

		g_SlimeKing.spawned = FALSE;
		g_SlimeKing.alive = FALSE;
		g_SlimeKing.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_SlimeKing.w = TEXTURE_WIDTH;
		g_SlimeKing.h = TEXTURE_HEIGHT;

		g_SlimeKing.currentSprite = 0;
		g_SlimeKing.currentAnim = SLIME_KING_ANIM_WAKE_UP;

		g_SlimeKing.moveSpeed = 2.0f;		// 移動量
		g_SlimeKing.moveDir = SLIME_KING_DIR_LEFT;

		g_SlimeKing.HP = HP_MAX;

		g_SlimeKing.damaged = FALSE;
		g_SlimeKing.dmgTimeCnt = 0;

		g_SlimeKing.enemyColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_SlimeKing.attackType = SLIME_KING_ATTACK_HORIZONTAL;

		g_SlimeKing.phase = 1;
		g_SlimeKing.portal = 0;
		g_SlimeKing.state = SLIME_KING_STATE_IDLE;
		g_SlimeKing.stateTime = 180;

		RoundKill();
	}
}


void SetSlimeKingPartible(XMFLOAT3 pos)
{
	for (int i = 0; i < SLIME_KING_PARTICLE_MAX; i++)
	{
		if (g_Particle[i].use == FALSE)
		{
			g_Particle[i].use = TRUE;
			g_Particle[i].pos = pos;

			float angle = (float)(rand() % 628) / 100;
			g_Particle[i].moveDir.x = cosf(angle);	//angleの方向へ移動
			g_Particle[i].moveDir.y = sinf(angle);	//angleの方向へ移動

			g_Particle[i].lifeTime = SLIME_KING_PARTICLE_MAX;
			g_Particle[i].lifeTimeCnt = 0;

			g_Particle[i].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		}		
	}

	for (int i = 0; i < SLIME_KING_PARTICLE_MAX; i++)
	{
		if (g_Particle[i].use == TRUE)
		{
			g_Particle[i].lifeTimeCnt++;
			g_Particle[i].pos.x += g_Particle[i].moveDir.x * g_Particle[i].moveSpeed;
			g_Particle[i].pos.y += g_Particle[i].moveDir.y * g_Particle[i].moveSpeed;
			g_Particle[i].color.w -= 1.0f/ SLIME_KING_PARTICLE_MAX;
			g_Particle[i].color.z += 1.0f/ SLIME_KING_PARTICLE_MAX;

			if (g_Particle[i].lifeTimeCnt == g_Particle[i].lifeTime)
			{
				g_Particle[i].use = FALSE;
			}
		}

	}
}
