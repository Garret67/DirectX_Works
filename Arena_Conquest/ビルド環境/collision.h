//=============================================================================
//
// �����蔻�菈�� [collision.h]
// Author : GARCIA DANIEL
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
 enum CollisionDirection {
	None,		//�������Ă��Ȃ�
	FromTop,	//�ォ��
	FromBottom,	//������
	FromLeft,	//������
	FromRight	//�E����
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

 int CheckCollisionAndGetDirection(float boxAX, float boxAY, float boxAWidth, float boxAHeight,
								   float boxBX, float boxBY, float boxBWidth, float boxBHeight);

BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh, XMFLOAT3 ypos, float yw, float yh);
BOOL CollisionBBCenterAndLT(XMFLOAT3 mpos, float mw, float mh, XMFLOAT3 ypos, float yw, float yh);

BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2);


