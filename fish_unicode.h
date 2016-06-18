#ifndef _TSTRING_
#define _TSTRING_
#include <xstring>
#include <tchar.h>
#define tstring basic_string<TCHAR>         // UNICODE STL CLASS for string 
#endif

#ifndef WCCREAT
#ifdef _UNICODE
#define WCREATE(X, Y)							_wcreat(X, Y)
#else
#define WCREATE(X, Y)							_creat(X, Y)
#endif
#endif // WCCREAT
