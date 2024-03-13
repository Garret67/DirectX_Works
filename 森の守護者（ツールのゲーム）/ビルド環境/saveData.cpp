//=============================================================================
//
// ファイル処理 [file.cpp]
// Author : GARCIA DANIEL
//
//=============================================================================

/******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "SaveData.h"
#include "sound.h"


/*******************************************************************************
* マクロ定義
*******************************************************************************/



/*******************************************************************************
* 構造体定義
*******************************************************************************/



/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/



/*******************************************************************************
マクロ定義
*******************************************************************************/



/*******************************************************************************
* グローバル変数
*******************************************************************************/
SAVE_PLAYER_DATA	saveData;		// セーブデータ作成場所


/*******************************************************************************
関数名:	void SaveDataPlayer( void )
引数:	void
戻り値:	void
説明:	セーブデータを作成し、ファイルへ出力する
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



	// セーブデータのチェックサムを求める
	{
		char *adr = (char *)&saveData;	// デーブデータの先頭アドレスをadrに入れる
		int  sum = 0;					// 計算するチェックサム

		saveData.sum = 0;				// セーブデータ側のチェックサムを０でクリアしていく

		for (int i = 0; i < sizeof(SAVE_PLAYER_DATA); i++)
		{
			sum += adr[i];
		}

		saveData.sum = sum;
	}



	// SAVEDATA構造体ごと全部をファイルに出力する
	FILE *fp;

	printf("\nセーブ開始・・・");
	fp = fopen("data/SaveData/playerData.bin", "wb");			// ファイルをバイナリ書き込みモードでOpenする

	if (fp != NULL)								// ファイルがあれば書き込み、無ければ無視
	{	
		fwrite(&saveData, 1, sizeof(SAVE_PLAYER_DATA), fp);	// 指定したアドレスから指定したバイト数分ファイルへ書き込む
		fclose(fp);								// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}

}


/*******************************************************************************
関数名:	void LoadData( void )
引数:	void
戻り値:	void
説明:	セーブデータをファイルから読み込む
*******************************************************************************/
void LoadDataPlayer(void)
{
	// ファイルからセーブデータを読み込む
	FILE* fp;

	printf("\nロード開始・・・");
	fp = fopen("savedata.bin", "rb");	// ファイルをバイナリ読み込みモードでOpenする

	if (fp != NULL)						// ファイルがあれば書き込み、無ければ無視
	{
		fread(&saveData, 1, sizeof(SAVE_PLAYER_DATA), fp);	// 指定したアドレスへ指定したバイト数分ファイルから読み込む
		fclose(fp);								// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}


	// セーブデータのチェックサムが合っているか調べる
	{
		char *adr = (char *)&saveData;	// デーブデータの先頭アドレスをadrに入れる
		int  sum = 0;				// 計算するチェックサム
		int  org = saveData.sum;		// セーブデータ内のチェックサム

		saveData.sum = 0;				// セーブデータ側のチェックサムを０でクリアしていく

		for (int i = 0; i < sizeof(SAVE_PLAYER_DATA); i++)
		{
			sum += adr[i];
		}

		// 元々のチェックサムと再計算したチェックサムが同じか調べている
		if (sum != org)
		{
			// データが改ざんされている！
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



