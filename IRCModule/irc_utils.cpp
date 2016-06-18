#include "stdafx.h"
#include "irc_defines.h"
#include "irc_utils.h"
#include "../fish_def.h"
#include <MMSystem.h>

// ���� ����ŭ Byte�� ��ȯ�Ѵ�.
int getString(char *lpDestPtr, const int destLimit,
		const char *lpSrcePtr, const int srceLimit, BOOL bFirst)
{
	char *pIdxDest;
	char *pIdxSrce;
	int realLimit = min( srceLimit, destLimit );
	int i=0;

	pIdxDest = lpDestPtr;
	pIdxSrce = (char *)lpSrcePtr;

	if(!bFirst)
	{
		if(*(pIdxSrce)!=DELIMITER) return -1;
		else pIdxSrce++;
	}

	while(i<realLimit 
		&& *(pIdxSrce)!=NULL
		&& *(pIdxSrce)!=DELIMITER 
		&& *(pIdxSrce)!=CR && *(pIdxSrce)!=LF)
	{
		*(pIdxDest) = *(pIdxSrce);
		pIdxDest++;
		pIdxSrce++;
		i++;
	}
	*(pIdxDest) = EOS;
	return( i );	
}

// ������ ���� ��ȿ�� Data�� ��ġ(Byte ��)�� ��ȯ�Ѵ�.
int findNext(const char *lpSrcePtr, const int srceLimit)
{
	char *pIdxSrce;
	int i=0;

	pIdxSrce = (char *)lpSrcePtr;

	while(1)
	{
		if(i>=srceLimit) break;
		if(*(pIdxSrce)==NULL) break;
		if(*(pIdxSrce)!=DELIMITER 
			&& *(pIdxSrce)!=CR 
			&& *(pIdxSrce)!=LF)
		{
			break;
		}
		pIdxSrce++;
		i++;
	}
	return( i );	
}


bool IsEmpty( char* str )
{
	if( str == NULL ) return true;
	if( strlen(str)==0 ) return true;
	if( strcmp( str, "" )==0 )
		return true;
	return false;
}//end of IsEmpty()

// strcmp()
bool isEqual( char* str1, char* str2 )
{
	if( str1 == NULL && str2==NULL )
		return false;

	if( strcmp( str1, str2 )==0 )
		return true;
	else
		return false;
}//end of isEqual()

// strncmp()  
bool isEqualN( char* str1, char* str2, int size )
{
	if( str1 == NULL &&  str2==NULL )
		return false;
	if( strncmp( str1, str2, size )== 0 )
		return true;
	else return false;
}//end of isEqualN()
bool isTrue( char* str )
{
	if( IsEmpty(str) ) return false;
	if( isEqual( str, "1" ) ) return true;
	else return false;
}//end of isTrue()

bool isTrue( CString str )
{
	if( str == _T("1") ) return true;
	else return false;
	
}//end of isTrue()

// ���� ����ϴ� �Լ� nType�� ���� �ٸ� ���带 ����Ѵ�.
void SoundWav( int nType )
{
	CFileFind find;

	switch ( nType ) 
	{
	case 1:
		// �⺻ ����
		if ( find.FindFile( _T(".\\wav\\default.wav") , NULL ) )
			sndPlaySound( _T(".\\Wav\\default.wav"), SND_ASYNC);
		break;
	case 2:
		// ���ε� �Ϸ�
		if ( find.FindFile( _T(".\\wav\\up.wav") , NULL ) )
			sndPlaySound( _T(".\\Wav\\up.wav"), SND_ASYNC);
		break;
	case 3:
		// �ٿ�ε� �Ϸ�
		if ( find.FindFile( _T(".\\wav\\down.wav") , NULL ) )
			sndPlaySound( _T(".\\Wav\\down.wav"), SND_ASYNC);
		break;
	case 4:
		// ���� �Ϸ�
		if ( find.FindFile( _T(".\\wav\\memo.wav") , NULL ) )
			sndPlaySound( _T(".\\Wav\\memo.wav"), SND_ASYNC);
		break;
	default:
		break;
	}
	find.Close();
}



/*
** NAME    **: encprytPassword 
** PARAM   **: char* dest, const char* src
** RETURN  **: boolean
** DESC    **: ������Ʈ���� ��ϵ� ������� ��й�ȣ�� encrpyt
** by bemlove at 2002-12-13, VER : 1.0, COMMENTS: ORG 
*/
bool encryptPassword( char* dest, char* src )
{
	if( IsEmpty( src ) ) return false;
	char szDest[30] = {0};
	char szSrc[30] = {0};
	memcpy( szSrc, src, strlen(src) );
	int len=0;
	len = strlen( szSrc );
	int i;
	for( i=0; i<len; i++ )
	{
		szDest[i] = szSrc[i] ^ ENCRYPT_KEY;
	}
	szDest[i] = 0L;

	memcpy( dest, szDest, len );
	return true;
}//end of encryptPassword()

/*
** NAME    **: decprytPassword 
** PARAM   **: char* dest, const char* src
** RETURN  **: boolean
** DESC    **: ������Ʈ���� ��ϵ� ��й�ȣ�� decrypt.
** by bemlove at 2002-12-13, VER : 1.0, COMMENTS: ORG 
*/
bool decryptPassword( char* dest, char* src )
{
	if( IsEmpty( src ) ) return false;
	char szDest[30] = {0};
	char szSrc[30] = {0};
	memcpy( szSrc, src, strlen(src) );
	int len=0;
	len = strlen( szSrc );
	int i;
	for( i=0; i<len; i++ )
	{
		szDest[i] = szSrc[i] ^ ENCRYPT_KEY;
	}
	szDest[i] = 0L;

	memcpy( dest, szDest, len );
	return true;
}//end of decryptPassword()

// ���ڿ��� �빮�ڷ� ��ȯ
bool toUppercase( char* str )
{
	if( IsEmpty( str ) ) return false;
	int len = strlen(str);
	for( int i=0; i<len ;i++)
	{
		if( str[i] >= 97 && str[i] <= 122 )
		{
			str[i] -= 32;
		}
	}
	return true;
}//end of toUppercase()


// ���ڿ��� �ҹ��ڷ� ��ȯ
bool toLowercase( char* str )
{
	if( IsEmpty( str ) ) return false;
	int len = strlen(str);
	for( int i=0; i<len ;i++)
	{
		if( str[i] >= 65 && str[i] <= 90 )
		{
			str[i] += 32;
		}
	}
	return true;
}// end of toLowercase()


// ��ҹ��ڿ� ������� ������������ Ȯ��
bool isEqualNocase( char * str1, char* str2 )
{
	if( IsEmpty( str1 ) || IsEmpty( str2 ) ) return false;

	char szStr1[MAX_BUFF+1]  = {0}, szStr2[MAX_BUFF+2] = {0};

	sprintf( szStr1, "%s", str1 );
	sprintf( szStr2, "%s", str2 );
	toLowercase( szStr1 );
	toLowercase( szStr2 );

	if( isEqual( szStr1, szStr2 ) ) 
		return true;

	return false;
}//end of isEqualNocase()

