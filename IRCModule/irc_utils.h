#ifndef _IRC_UTILS_H
#define _IRC_UTILS_H
#include "../Util/md5e.h"

int getString(char *lpDestPtr, const int destLimit,
		const char *lpSrcePtr, const int srceLimit, BOOL bFirst=FALSE);
int findNext(const char *lpSrcePtr, const int srceLimit);

// added by moonknit 2005-05-24
// copied from old util.h of Enppy 2.0
bool IsEmpty(char* str);
bool isEqual( char* str1, char* str2 );
bool isEqualN( char* str1, char* str2, int size );

bool isTrue( char* str );
bool isTrue( CString str );

bool toUppercase( char* str );
bool toLowercase( char* str );
bool isEqualNocase( char* str1, char* str2 );

bool encryptPassword( char* dest, char* src );
bool decryptPassword( char* dest, char* src );

bool encryptUID( char* uid, CTime time,  /*[out]*/DWORD & key);


#endif