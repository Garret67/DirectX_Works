//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


enum PARTICLE_TYPE
{
	PARTICLE_HIT,
	PARTICLE_HEATH,
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float Size, int nLife, float angle, int type);
void SetColorParticle(int nIdxParticle, XMFLOAT4 col);

void SetHitParticle(XMFLOAT3 position, BOOL crit);
void SetHitParticle(XMFLOAT3 position);
