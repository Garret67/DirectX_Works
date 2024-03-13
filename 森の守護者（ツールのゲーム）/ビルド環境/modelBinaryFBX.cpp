//=============================================================================
//
// ���f���̏��� [modelBinaryFBX.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include "modelBinaryFBX.h"
#include "camera.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

void LoadModelAnimData(char* FileName, MODEL_DATA* modelData, FBX_ANIMATOR* Animator);
void LoadModelData(char* FileName, MODEL_DATA* modelData);

void AnimationInterpolation(MODEL_DATA* ResultModel, FBX_ANIMATOR& Animator, int& curAnim, int& nextAnim);

//=============================================================================
// ����������
// �A�j���[�V���������郂�f��
//=============================================================================
void InitModelBinaryFBX(char* FileName, DX11_MODEL* Model, MODEL_DATA* modelData, FBX_ANIMATOR* Animator)
{
	LoadModelAnimData(FileName, modelData, Animator);
	
	Model->BuffersNum = modelData->MeshNum;
	Model->Buffers = new DX11_BUFFERS[Model->BuffersNum];

	for (int j = 0; j < modelData->MeshNum; j++)
	{
		// ���_�o�b�t�@����
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * modelData->Mesh[j].VertexNum;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  // Permite la escritura desde la CPU

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = modelData->Mesh[j].VertexArray;

			GetDevice()->CreateBuffer(&bd, &sd, &Model->Buffers[j].VertexBuffer);
		}


		// �C���f�b�N�X�o�b�t�@����
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned short) * modelData->Mesh[j].IndexNum;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = modelData->Mesh[j].IndexArray;

			GetDevice()->CreateBuffer(&bd, &sd, &Model->Buffers[j].IndexBuffer);
		}

		// �T�u�Z�b�g�ݒ�
		{
			Model->Buffers[j].SubsetArray = new DX11_FBXSUBSET[modelData->Mesh[j].SubsetNum];
			Model->Buffers[j].SubsetNum = modelData->Mesh[j].SubsetNum;
			Model->Buffers[j].SubsetArray[0].Material.Texture = NULL;

			for (unsigned short i = 0; i < modelData->Mesh[j].SubsetNum; i++)
			{
				Model->Buffers[j].SubsetArray[i].StartIndex = modelData->Mesh[j].SubsetArray[i].StartIndex;
				Model->Buffers[j].SubsetArray[i].IndexNum = modelData->Mesh[j].SubsetArray[i].IndexNum;

				Model->Buffers[j].SubsetArray[i].Material.Material = modelData->Mesh[j].SubsetArray[i].Material.Material;

				Model->Buffers[j].SubsetArray[i].Material.Texture = NULL;
				D3DX11CreateShaderResourceViewFromFile(GetDevice(),
					modelData->Mesh[j].SubsetArray[i].Material.TextureName,
					NULL,
					NULL,
					&Model->Buffers[j].SubsetArray[i].Material.Texture,
					NULL);
			}
		}

	}



}

//=============================================================================
// ����������
// �A�j���[�V�������Ȃ����f��
//=============================================================================
void InitModelBinaryFBX(char* FileName, DX11_MODEL* Model, MODEL_DATA* modelData)
{
	LoadModelData(FileName, modelData);

	Model->BuffersNum = modelData->MeshNum;
	Model->Buffers = new DX11_BUFFERS[Model->BuffersNum];

	for (int j = 0; j < modelData->MeshNum; j++)
	{
		// ���_�o�b�t�@����
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * modelData->Mesh[j].VertexNum;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  // Permite la escritura desde la CPU

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = modelData->Mesh[j].VertexArray;

			GetDevice()->CreateBuffer(&bd, &sd, &Model->Buffers[j].VertexBuffer);
		}


		// �C���f�b�N�X�o�b�t�@����
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned short) * modelData->Mesh[j].IndexNum;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = modelData->Mesh[j].IndexArray;

			GetDevice()->CreateBuffer(&bd, &sd, &Model->Buffers[j].IndexBuffer);
		}

		// �T�u�Z�b�g�ݒ�
		{
			Model->Buffers[j].SubsetArray = new DX11_FBXSUBSET[modelData->Mesh[j].SubsetNum];
			Model->Buffers[j].SubsetNum = modelData->Mesh[j].SubsetNum;
			Model->Buffers[j].SubsetArray[0].Material.Texture = NULL;

			for (unsigned short i = 0; i < modelData->Mesh[j].SubsetNum; i++)
			{
				Model->Buffers[j].SubsetArray[i].StartIndex = modelData->Mesh[j].SubsetArray[i].StartIndex;
				Model->Buffers[j].SubsetArray[i].IndexNum = modelData->Mesh[j].SubsetArray[i].IndexNum;

				Model->Buffers[j].SubsetArray[i].Material.Material = modelData->Mesh[j].SubsetArray[i].Material.Material;

				Model->Buffers[j].SubsetArray[i].Material.Texture = NULL;
				D3DX11CreateShaderResourceViewFromFile(GetDevice(),
					modelData->Mesh[j].SubsetArray[i].Material.TextureName,
					NULL,
					NULL,
					&Model->Buffers[j].SubsetArray[i].Material.Texture,
					NULL);
			}
		}

	}



}


//=============================================================================
// �A�j���[�V���������郂�f���̏I������
//=============================================================================
void UninitModelBinaryFBX(DX11_MODEL* Model, MODEL_DATA* modelData, FBX_ANIMATOR* Animator)
{

	//MODEL
	{
		for (int i = 0; i < Model->BuffersNum; i++)
		{
			for (unsigned short j = 0; j < Model->Buffers[i].SubsetNum; j++)
			{
				if (Model->Buffers[i].SubsetArray[j].Material.Texture)
				{
					Model->Buffers[i].SubsetArray[j].Material.Texture->Release();
					Model->Buffers[i].SubsetArray[j].Material.Texture = NULL;
				}
			}


			if (Model->Buffers[i].VertexBuffer)	Model->Buffers[i].VertexBuffer->Release();
			if (Model->Buffers[i].IndexBuffer)	Model->Buffers[i].IndexBuffer->Release();
			if (Model->Buffers[i].SubsetArray)	delete[] Model->Buffers[i].SubsetArray;

		}
		delete[] Model->Buffers;

	}

	//MODEL DATA
	{
		for (int j = 0; j < modelData->MeshNum; j++)
		{
			delete[] modelData->Mesh[j].VertexArray;
			delete[] modelData->Mesh[j].IndexArray;
			delete[] modelData->Mesh[j].SubsetArray;
		}
		delete[] modelData->Mesh;

		delete[] modelData->Transform;
	}


	//ANIMATOR
	{
		if (Animator->animationNum > 0)
		{
			for (int i = 0; i < Animator->animationNum; i++)
			{
				if (Animator->animation[i].keyFrameNum > 0 && Animator->animation[i].keyFrame)
				{
					for (int j = 0; j < Animator->animation[i].keyFrameNum; j++)
					{
						if (Animator->animation[i].keyFrame[j].Modelo) delete[] Animator->animation[i].keyFrame[j].Modelo;
					}

					delete[] Animator->animation[i].keyFrame;
				}

			}

			if (Animator->animation) delete[] Animator->animation;
		}

	}

}

//=============================================================================
// �A�j���[�V�������Ȃ����f���̏I������
//=============================================================================
void UninitModelBinaryFBX(DX11_MODEL* Model, MODEL_DATA* modelData)
{
	//MODEL
	{
		for (int i = 0; i < Model->BuffersNum; i++)
		{
			for (unsigned short j = 0; j < Model->Buffers[i].SubsetNum; j++)
			{
				if (Model->Buffers[i].SubsetArray[j].Material.Texture)
				{
					Model->Buffers[i].SubsetArray[j].Material.Texture->Release();
					Model->Buffers[i].SubsetArray[j].Material.Texture = NULL;
				}
			}


			if (Model->Buffers[i].VertexBuffer)	Model->Buffers[i].VertexBuffer->Release();
			if (Model->Buffers[i].IndexBuffer)	Model->Buffers[i].IndexBuffer->Release();
			if (Model->Buffers[i].SubsetArray)	delete[] Model->Buffers[i].SubsetArray;

		}
		delete[] Model->Buffers;

	}

	//MODEL DATA
	{
		for (int j = 0; j < modelData->MeshNum; j++)
		{
			delete[] modelData->Mesh[j].VertexArray;
			delete[] modelData->Mesh[j].IndexArray;
			delete[] modelData->Mesh[j].SubsetArray;
		}
		delete[] modelData->Mesh;

		delete[] modelData->Transform;
	}


}


//******************************************
// ���f���X�V����
//******************************************
void UpdateModelBinaryFBX(MODEL_DATA* ResultModel, FBX_ANIMATOR& Animator)
{

	AnimationInterpolation(ResultModel, Animator, Animator.curAnim, Animator.nextAnim);


}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawModelBinaryFBX(DX11_MODEL* Model, MODEL_DATA  modelData, XMMATRIX worldMatix)
{

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;

	int ModelsDiference = (modelData.TransformNum - modelData.MeshNum);	//�`�悵�Ȃ����f���ׁ̈@//For the empty models

	for (int j = 0; j < modelData.TransformNum; j++)
	{

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(modelData.Transform[j].LclScaling.x, modelData.Transform[j].LclScaling.y, modelData.Transform[j].LclScaling.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(modelData.Transform[j].LclRotation.x, modelData.Transform[j].LclRotation.y, modelData.Transform[j].LclRotation.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(modelData.Transform[j].LclPosition.x, modelData.Transform[j].LclPosition.y, modelData.Transform[j].LclPosition.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// �q����������e�ƌ�������
		if (modelData.Transform[j].FatherIdx >= 0)
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&modelData.Transform[modelData.Transform[j].FatherIdx].mtxWorld));
		}

		//�}�g���N�X���Z�[�u����
		XMStoreFloat4x4(&modelData.Transform[j].mtxWorld, mtxWorld);

		int drawIdx = j - ModelsDiference;	//empty objects

		if (drawIdx < 0) continue;


		//World matrix�ƌ�������
		mtxWorld = XMMatrixMultiply(mtxWorld, worldMatix);


		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);


		//************************

		GetDeviceContext()->IASetVertexBuffers(0, 1, &Model->Buffers[drawIdx].VertexBuffer, &stride, &offset);

		// �C���f�b�N�X�o�b�t�@�ݒ�
		GetDeviceContext()->IASetIndexBuffer(Model->Buffers[drawIdx].IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// �v���~�e�B�u�g�|���W�ݒ�
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (unsigned short i = 0; i < Model->Buffers[drawIdx].SubsetNum; i++)
		{
			// �}�e���A���ݒ�
			SetMaterial(Model->Buffers[drawIdx].SubsetArray[i].Material.Material);

			// �e�N�X�`���ݒ�
			if (Model->Buffers[drawIdx].SubsetArray[i].Material.Material.noTexSampling == 0)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &Model->Buffers[drawIdx].SubsetArray[i].Material.Texture);
			}

			// �|���S���`��
			GetDeviceContext()->DrawIndexed(Model->Buffers[drawIdx].SubsetArray[i].IndexNum, Model->Buffers[drawIdx].SubsetArray[i].StartIndex, 0);

		}

	}
}




//*****************************
// ���[�h���f���֐�
// �A�j���[�V���������郂�f��
//*****************************
void LoadModelAnimData(char* FileName, MODEL_DATA* modelData, FBX_ANIMATOR* Animator)
{
	// �t�@�C������Z�[�u�f�[�^��ǂݍ���
	FILE* fp;

	printf("\n���[�h�J�n�E�E�E");
	fp = fopen(FileName, "rb");	// �t�@�C�����o�C�i���ǂݍ��݃��[�h��Open����

	if (fp != NULL)		// �t�@�C��������Ώ������݁A������Ζ���
	{
		//Model data
		fread(&modelData->ModelName, sizeof(char), 256, fp);
		fread(&modelData->MeshNum, sizeof(unsigned short), 1, fp);
		modelData->Mesh = new MESH_DATA[modelData->MeshNum];

		//���b�V���f�[�^
		for (int i = 0; i < modelData->MeshNum; i++)
		{
			//���_�o�b�t�@�[
			fread(&modelData->Mesh[i].VertexNum, sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].VertexArray = new VERTEX_3D[modelData->Mesh[i].VertexNum];
			for (int j = 0; j < modelData->Mesh[i].VertexNum; j++)
			{
				fread(&modelData->Mesh[i].VertexArray[j], sizeof(VERTEX_3D), 1, fp);
			}

			//�C���f�N�X�o�b�t�@�[
			fread(&modelData->Mesh[i].IndexNum, sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].IndexArray = new unsigned short[modelData->Mesh[i].IndexNum];
			for (int j = 0; j < modelData->Mesh[i].IndexNum; j++)
			{
				fread(&modelData->Mesh[i].IndexArray[j], sizeof(unsigned short), 1, fp);
			}

			//�T�u�Z�b�g�f�[�^�i�}�e���A�����j
			fread(&modelData->Mesh[i].SubsetNum, sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].SubsetArray = new FBXSUBSET[modelData->Mesh[i].SubsetNum];
			for (int j = 0; j < modelData->Mesh[i].SubsetNum; j++)
			{
				fread(&modelData->Mesh[i].SubsetArray[j], sizeof(FBXSUBSET), 1, fp);
			}

		}

		//�g�����X�t�H�[�����
		fread(&modelData->TransformNum, 1, sizeof(unsigned short), fp);
		modelData->Transform = new TRANSFORM_DATA[modelData->TransformNum];

		for (int i = 0; i < modelData->TransformNum; i++)
		{
			fread(&modelData->Transform[i].ObjectName, sizeof(char), 50, fp);
			fread(&modelData->Transform[i].FatherIdx, sizeof(int), 1, fp);

			modelData->Transform[i].LclPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
			modelData->Transform[i].LclRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
			modelData->Transform[i].LclScaling = XMFLOAT3(1.0f, 1.0f, 1.0f);
		}




		//�A�j���[�V�������	//Animator
		fread(&Animator->animationNum, 1, sizeof(int), fp);
		Animator->animation = new FBX_ANIMATION[Animator->animationNum];

		for (int i = 0; i < Animator->animationNum; i++)
		{
			fread(&Animator->animation[i].keyFrameNum, sizeof(int), 1, fp);
			fread(&Animator->animation[i].AnimTimeCnt, sizeof(float), 1, fp);
			fread(&Animator->animation[i].AnimTransitionFrames, sizeof(float), 1, fp);
			fread(&Animator->animation[i].loop, sizeof(BOOL), 1, fp);
			fread(&Animator->animation[i].nextAnimation, sizeof(int), 1, fp);
			fread(&Animator->animation[i].speed, sizeof(float), 1, fp);
			fread(&Animator->animation[i].animationName, sizeof(char), 256, fp);

			Animator->animation[i].keyFrame = new FBX_ANIM_KEYFRAME[Animator->animation[i].keyFrameNum];
			//�L�[�t���[�����
			for (int j = 0; j < Animator->animation[i].keyFrameNum; j++)
			{
				fread(&Animator->animation[i].keyFrame[j].modelosNum, sizeof(int), 1, fp);
				fread(&Animator->animation[i].keyFrame[j].AnimFramesMax, sizeof(float), 1, fp);

				Animator->animation[i].keyFrame[j].Modelo = new ANIM_MODEL[Animator->animation[i].keyFrame[j].modelosNum];

				//�I�u�W�F�N�g�̃g�����X�t�H�[�����
				for (int z = 0; z < Animator->animation[i].keyFrame[j].modelosNum; z++)
				{
					fread(&Animator->animation[i].keyFrame[j].Modelo[z].LclPosition, sizeof(XMFLOAT3), 1, fp);
					fread(&Animator->animation[i].keyFrame[j].Modelo[z].LclRotation, sizeof(XMFLOAT4), 1, fp);
					fread(&Animator->animation[i].keyFrame[j].Modelo[z].LclScaling, sizeof(XMFLOAT3), 1, fp);
				}
			}
		}

		//�A�j���[�V�����Ǘ��ϐ�
		fread(&Animator->curAnim, sizeof(int), 1, fp);
		fread(&Animator->nextAnim, sizeof(int), 1, fp);
		fread(&Animator->transitionFramesCnt, sizeof(float), 1, fp);
		fread(&Animator->hiddenObj, sizeof(int), HIDDEN_OBJ_NUM, fp);

		fclose(fp);								// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");
	}

}


//*****************************
// ���[�h���f���֐�
// �A�j���[�V�������Ȃ����f��
//*****************************
void LoadModelData(char* FileName, MODEL_DATA* modelData)
{
	// �t�@�C������Z�[�u�f�[�^��ǂݍ���
	FILE* fp;

	printf("\n���[�h�J�n�E�E�E");
	fp = fopen(FileName, "rb");	// �t�@�C�����o�C�i���ǂݍ��݃��[�h��Open����

	if (fp != NULL)						// �t�@�C��������Ώ������݁A������Ζ���
	{
		//Model data
		fread(&modelData->ModelName, sizeof(char), 256, fp);
		fread(&modelData->MeshNum, sizeof(unsigned short), 1, fp);
		modelData->Mesh = new MESH_DATA[modelData->MeshNum];

		//���b�V���f�[�^
		for (int i = 0; i < modelData->MeshNum; i++)
		{
			//���_�o�b�t�@�[
			fread(&modelData->Mesh[i].VertexNum, sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].VertexArray = new VERTEX_3D[modelData->Mesh[i].VertexNum];
			for (int j = 0; j < modelData->Mesh[i].VertexNum; j++)
			{
				fread(&modelData->Mesh[i].VertexArray[j], sizeof(VERTEX_3D), 1, fp);
			}

			//�C���f�N�X�o�b�t�@�[
			fread(&modelData->Mesh[i].IndexNum, sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].IndexArray = new unsigned short[modelData->Mesh[i].IndexNum];
			for (int j = 0; j < modelData->Mesh[i].IndexNum; j++)
			{
				fread(&modelData->Mesh[i].IndexArray[j], sizeof(unsigned short), 1, fp);
			}

			//�T�u�Z�b�g�f�[�^�i�}�e���A�����j
			fread(&modelData->Mesh[i].SubsetNum, sizeof(unsigned short), 1, fp);
			modelData->Mesh[i].SubsetArray = new FBXSUBSET[modelData->Mesh[i].SubsetNum];
			for (int j = 0; j < modelData->Mesh[i].SubsetNum; j++)
			{
				fread(&modelData->Mesh[i].SubsetArray[j], sizeof(FBXSUBSET), 1, fp);
			}

		}

		//�g�����X�t�H�[�����
		fread(&modelData->TransformNum, 1, sizeof(unsigned short), fp);
		modelData->Transform = new TRANSFORM_DATA[modelData->TransformNum];

		for (int i = 0; i < modelData->TransformNum; i++)
		{
			fread(&modelData->Transform[i].ObjectName, sizeof(char), 50, fp);
			fread(&modelData->Transform[i].FatherIdx, sizeof(int), 1, fp);

			modelData->Transform[i].LclPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
			modelData->Transform[i].LclRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
			modelData->Transform[i].LclScaling = XMFLOAT3(1.0f, 1.0f, 1.0f);
		}


		fclose(fp);								// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");
	}

}



//******************************************
// �A�j���[�V�����̍X�V����
// �����F�u�����f�B���O�A��\���A
//******************************************
void AnimationInterpolation(MODEL_DATA* ResultModel, FBX_ANIMATOR& Animator, int& curAnim, int& nextAnim)
{
	//�A�j���[�V�������Ȃ��ꍇ�A�߂�
	if (curAnim < 0) return;	
	if (Animator.animation[curAnim].AnimTimeCnt < 0 || Animator.animation[curAnim].AnimTimeCnt > 100) return;

	float& curAnimFramesCnt = Animator.animation[curAnim].AnimTimeCnt;	//�A�j���[�V�����̌��݂̃t���[���i���ԁj

	int CurKF = (int)curAnimFramesCnt; //���̃L�[�t���[���@//current Key frame

	float curAnimSpeed = Animator.animation[curAnim].speed;	//�A�j���[�V�����̑��x

	float  curAnimFramesMax = Animator.animation[curAnim].keyFrame[CurKF].AnimFramesMax;	//�A�j���[�V�����̍ŏI�t���[���i���ԁj
	curAnimFramesMax /= curAnimSpeed;	//�ŏI�t���[�����A�j���[�V�����̑��x�ɍ��킹��

	//���ݍĐ����Ă���A�j���[�V�������X�V����
	for (int i = 0; i < Animator.animation[curAnim].keyFrame[CurKF].modelosNum; i++)
	{
		// ���̃L�[�t���[���Ǝ��̃L�[�t���[����XMVECTOR�ɕϊ�����@// Convertir las representaciones iniciales y finales a XMVECTOR
		XMVECTOR nowPos = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF].Modelo[i].LclPosition);
		XMVECTOR nowRot = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF].Modelo[i].LclRotation);
		XMVECTOR nowScl = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF].Modelo[i].LclScaling);

		XMVECTOR NextPos = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF + 1].Modelo[i].LclPosition);
		XMVECTOR NextRot = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF + 1].Modelo[i].LclRotation);
		XMVECTOR NextScl = XMLoadFloat3(&Animator.animation[curAnim].keyFrame[CurKF + 1].Modelo[i].LclScaling);


		float nowTime = curAnimFramesCnt - CurKF;	// ���ԕ����ł��鏭�������o���Ă���

		//���`��ԂŎ��̃t���[�����v�Z����
		XMVECTOR resultPos = XMVectorLerp(nowPos, NextPos, nowTime);
		XMVECTOR resultRot = XMVectorLerp(nowRot, NextRot, nowTime);
		XMVECTOR resultScl = XMVectorLerp(nowScl, NextScl, nowTime);

		//���ʂ��Z�[�u����
		XMStoreFloat3(&ResultModel->Transform[i].LclPosition, resultPos);
		XMStoreFloat3(&ResultModel->Transform[i].LclRotation, resultRot);
		XMStoreFloat3(&ResultModel->Transform[i].LclScaling, resultScl);


		{//��\���I�u�W�F�N�g�p
			for (int j = 0; j < HIDDEN_OBJ_NUM; j++)
			{
				if (Animator.hiddenObj[j] == i)
				{
					//��\�����镔���̃X�P�[����0�ɂ���i�e�q�֌W�ŁA�q���������Ȃ��Ȃ�j
					ResultModel->Transform[i].LclScaling = XMFLOAT3(0.0f, 0.0f, 0.0f);
					break;
				}
			}
		}

	}


	curAnimFramesCnt += 1.0f / curAnimFramesMax;	// ���Ԃ�i�߂Ă���


	if ((int)curAnimFramesCnt >= Animator.animation[curAnim].keyFrameNum - 1)	// �A�j���[�V�������I������H
	{

		//���[�v���Ȃ��ꍇ
		if (Animator.animation[curAnim].loop == FALSE)
		{
			if (curAnim != Animator.animation[curAnim].nextAnimation)
			{
				nextAnim = Animator.animation[curAnim].nextAnimation;	//���̃A�j���[�V�����ɕύX����
			}
			curAnimFramesCnt -= 1.0f / curAnimFramesMax;
		}
		else //���[�v����ꍇ
		{
			curAnimFramesCnt -= Animator.animation[curAnim].keyFrameNum - 1;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
		}
	}


	//�A�j���[�V�����u�����f�B���O���Ă�����A���̃A�j���[�V�������X�V����
	if (nextAnim >= 0)
	{
		XMVECTOR nextAnimPos;
		XMVECTOR nextAnimRos;
		XMVECTOR nextAnimScl;

		float& transitionFramesCnt = Animator.transitionFramesCnt;		//�u�����f�B���O�̌��݂̃t���[���J�E���^�[�i���ԁj
		float curtransitionFramesMax = Animator.animation[curAnim].AnimTransitionFrames;	//���̃A�j���[�V�����ɐ؂�ւ���܂ł̃t���[����


		float& nextAnimFramesCnt = Animator.animation[nextAnim].AnimTimeCnt;	//���̃A�j���[�V�����̎��ԃJ�E���^�[


		int NextAnimCurKF; //���̃A�j���[�V�����̌��݂̃L�[�t���[��	//current Key frame
		NextAnimCurKF = (int)nextAnimFramesCnt;

		float nextAnimSpeed = Animator.animation[nextAnim].speed;	//���̃A�j���[�V�����̑��x
		float nextAnimFramesMax = Animator.animation[nextAnim].keyFrame[NextAnimCurKF].AnimFramesMax;	//���̃A�j���[�V�����̍ŏI�t���[���i���ԁj
		nextAnimFramesMax /= nextAnimSpeed;

		//���̃A�j���[�V�������X�V����
		for (int i = 0; i < Animator.animation[nextAnim].keyFrame[NextAnimCurKF].modelosNum; i++)
		{
			// ���̃L�[�t���[���Ǝ��̃L�[�t���[����XMVECTOR�ɕϊ�����	// Convertir las representaciones iniciales y finales a XMVECTOR
			XMVECTOR nowPos = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF].Modelo[i].LclPosition);
			XMVECTOR nowRot = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF].Modelo[i].LclRotation);
			XMVECTOR nowScl = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF].Modelo[i].LclScaling);

			XMVECTOR NextPos = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF + 1].Modelo[i].LclPosition);
			XMVECTOR NextRot = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF + 1].Modelo[i].LclRotation);
			XMVECTOR NextScl = XMLoadFloat3(&Animator.animation[nextAnim].keyFrame[NextAnimCurKF + 1].Modelo[i].LclScaling);


			float nowTime = nextAnimFramesCnt - NextAnimCurKF;	// ���ԕ����ł��鏭�������o���Ă���

			//���`��ԂŎ��̃t���[�����v�Z����
			nextAnimPos = XMVectorLerp(nowPos, NextPos, nowTime);
			nextAnimRos = XMVectorLerp(nowRot, NextRot, nowTime);
			nextAnimScl = XMVectorLerp(nowScl, NextScl, nowTime);



			//**************************
			// TRANSITION INTERPOLATION
			// �A�j���[�V�����u�����f�B���O
			//**************************

			// ���Đ����Ă���A�j���[�V�����̒l��XMVECTOR�ɕϊ�����
			XMVECTOR nowAnimPos = XMLoadFloat3(&ResultModel->Transform[i].LclPosition);		// XMVECTOR�֕ϊ�
			XMVECTOR nowAnimRot = XMLoadFloat3(&ResultModel->Transform[i].LclRotation);		// XMVECTOR�֕ϊ�
			XMVECTOR nowAnimScl = XMLoadFloat3(&ResultModel->Transform[i].LclScaling);		// XMVECTOR�֕ϊ�


			//�����̃A�j���[�V�����̍����v�Z����
			XMVECTOR DiferencePos = (nextAnimPos)-nowAnimPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			XMVECTOR DiferenceRot = (nextAnimRos)-nowAnimRot;	// XYZ��]�ʂ��v�Z���Ă���
			XMVECTOR DiferenceScl = (nextAnimScl)-nowAnimScl;	// XYZ�g�嗦���v�Z���Ă���

			float transitionTime = transitionFramesCnt / curtransitionFramesMax;	// ���ԕ����ł��鏭�������o���Ă���


			DiferencePos *= transitionTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
			DiferenceRot *= transitionTime;								// ���݂̉�]�ʂ��v�Z���Ă���
			DiferenceScl *= transitionTime;								// ���݂̊g�嗦���v�Z���Ă���

			//�u�����f�B���O�̌��ʂ��Z�[�u����
			XMStoreFloat3(&ResultModel->Transform[i].LclPosition, nowAnimPos + DiferencePos);
			XMStoreFloat3(&ResultModel->Transform[i].LclRotation, nowAnimRot + DiferenceRot);
			XMStoreFloat3(&ResultModel->Transform[i].LclScaling, nowAnimScl + DiferenceScl);


			{//��\���I�u�W�F�N�g�p
				for (int j = 0; j < HIDDEN_OBJ_NUM; j++)
				{
					if (Animator.hiddenObj[j] == i)
					{
						//��\�����镔���̃X�P�[����0�ɂ���i�e�q�֌W�ŁA�q���������Ȃ��Ȃ�j
						ResultModel->Transform[i].LclScaling = XMFLOAT3(0.0f, 0.0f, 0.0f);
						break;
					}
				}
			}

		}

		// ���̃A�j���[�V�����̎��Ԍo�ߏ���������
		if (Animator.animation[nextAnim].loop == TRUE) nextAnimFramesCnt += 1.0f / nextAnimFramesMax;	// ���Ԃ�i�߂Ă���

		if ((int)nextAnimFramesCnt >= Animator.animation[nextAnim].keyFrameNum - 1)			// �A�j���[�V�������I������H
		{
			nextAnimFramesCnt -= Animator.animation[nextAnim].keyFrameNum - 1;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
		}



		transitionFramesCnt++;

		//�u�����f�B���O�̏I������
		if (transitionFramesCnt >= curtransitionFramesMax)
		{
			curAnimFramesCnt = 0.0f;
			curAnim = nextAnim;
			nextAnim = -1;

			Animator.transitionFramesCnt = 0.0f;
		}

	}
}


// �p�x���𐳋K������
float NormalizeAngle(float angle)
{
	while (angle < -XM_PI)
	{
		angle += XM_2PI;
	}

	while (angle > XM_PI)
	{
		angle -= XM_2PI;
	}

	return angle;
}


//���`���
float lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

//=============================================================================
// �x�W�F�Ȑ��֐�
// �߂�l�Ft�ɂ���āA����OBJ�̍��W�iVector3�j
//=============================================================================
XMFLOAT3 BezierFunction(float t, XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3)
{
	XMVECTOR P0 = XMLoadFloat3(&p0);
	XMVECTOR P1 = XMLoadFloat3(&p1);
	XMVECTOR P2 = XMLoadFloat3(&p2);
	XMVECTOR P3 = XMLoadFloat3(&p3);



	XMVECTOR p =
		powf((1 - t), 3) * P0 +
		3 * powf((1 - t), 2) * t * P1 +
		3 * (1 - t) * powf(t, 2) * P2 +
		powf(t, 3) * P3;

	XMFLOAT3 ans;
	XMStoreFloat3(&ans, p);

	return ans;
}
