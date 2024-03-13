//=============================================================================
//
// �t�@�C������ [file.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================

/******************************************************************************
* �C���N���[�h�t�@�C��
*******************************************************************************/
#include "SaveData.h"
#include "sound.h"


/*******************************************************************************
* �}�N����`
*******************************************************************************/



/*******************************************************************************
* �\���̒�`
*******************************************************************************/



/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/



/*******************************************************************************
�}�N����`
*******************************************************************************/



/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
SAVE_PLAYER_DATA	saveData;		// �Z�[�u�f�[�^�쐬�ꏊ


/*******************************************************************************
�֐���:	void SaveDataPlayer( void )
����:	void
�߂�l:	void
����:	�Z�[�u�f�[�^���쐬���A�t�@�C���֏o�͂���
*******************************************************************************/
void SaveDataPlayer(void)
{
	
	PLAYER* player = GetPlayer();

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		saveData.playerData[i].pos		= player[i].pos;
		saveData.playerData[i].rot		= player[i].rot;

		saveData.playerData[i].HPmax	= player[i].HPmax;
		saveData.playerData[i].HPcur	= player[i].HPcur;
		saveData.playerData[i].agility	= player[i].agility;
		saveData.playerData[i].attack	= player[i].attack;
		saveData.playerData[i].deffense = player[i].deffense;
	}



	// �Z�[�u�f�[�^�̃`�F�b�N�T�������߂�
	{
		char *adr = (char *)&saveData;	// �f�[�u�f�[�^�̐擪�A�h���X��adr�ɓ����
		int  sum = 0;					// �v�Z����`�F�b�N�T��

		saveData.sum = 0;				// �Z�[�u�f�[�^���̃`�F�b�N�T�����O�ŃN���A���Ă���

		for (int i = 0; i < sizeof(SAVE_PLAYER_DATA); i++)
		{
			sum += adr[i];
		}

		saveData.sum = sum;
	}



	// SAVEDATA�\���̂��ƑS�����t�@�C���ɏo�͂���
	FILE *fp;

	printf("\n�Z�[�u�J�n�E�E�E");
	fp = fopen("data/SaveData/playerData.bin", "wb");			// �t�@�C�����o�C�i���������݃��[�h��Open����

	if (fp != NULL)								// �t�@�C��������Ώ������݁A������Ζ���
	{	
		fwrite(&saveData, 1, sizeof(SAVE_PLAYER_DATA), fp);	// �w�肵���A�h���X����w�肵���o�C�g�����t�@�C���֏�������
		fclose(fp);								// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");
	}

}


/*******************************************************************************
�֐���:	void LoadData( void )
����:	void
�߂�l:	void
����:	�Z�[�u�f�[�^���t�@�C������ǂݍ���
*******************************************************************************/
void LoadDataPlayer(void)
{
	// �t�@�C������Z�[�u�f�[�^��ǂݍ���
	FILE* fp;

	printf("\n���[�h�J�n�E�E�E");
	fp = fopen("savedata.bin", "rb");	// �t�@�C�����o�C�i���ǂݍ��݃��[�h��Open����

	if (fp != NULL)						// �t�@�C��������Ώ������݁A������Ζ���
	{
		fread(&saveData, 1, sizeof(SAVE_PLAYER_DATA), fp);	// �w�肵���A�h���X�֎w�肵���o�C�g�����t�@�C������ǂݍ���
		fclose(fp);								// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");
	}


	// �Z�[�u�f�[�^�̃`�F�b�N�T���������Ă��邩���ׂ�
	{
		char *adr = (char *)&saveData;	// �f�[�u�f�[�^�̐擪�A�h���X��adr�ɓ����
		int  sum = 0;				// �v�Z����`�F�b�N�T��
		int  org = saveData.sum;		// �Z�[�u�f�[�^���̃`�F�b�N�T��

		saveData.sum = 0;				// �Z�[�u�f�[�^���̃`�F�b�N�T�����O�ŃN���A���Ă���

		for (int i = 0; i < sizeof(SAVE_PLAYER_DATA); i++)
		{
			sum += adr[i];
		}

		// ���X�̃`�F�b�N�T���ƍČv�Z�����`�F�b�N�T�������������ׂĂ���
		if (sum != org)
		{
			// �f�[�^�������񂳂�Ă���I
			return;
		}
	}

	PLAYER* player = GetPlayer();

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		player[i].pos		= saveData.playerData[i].pos;
		player[i].rot		= saveData.playerData[i].rot;

		player[i].HPmax		= saveData.playerData[i].HPmax;
		player[i].HPcur		= saveData.playerData[i].HPcur;
		player[i].agility	= saveData.playerData[i].agility;
		player[i].attack	= saveData.playerData[i].attack;
		player[i].deffense	= saveData.playerData[i].deffense;
	}


}



