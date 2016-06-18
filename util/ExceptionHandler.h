#ifndef __MSJEXHND_H__
#define __MSJEXHND_H__
#include <imagehlp.h>
// #include "Defines.h"

class MSJExceptionHandler
{
	public:
	
	MSJExceptionHandler( );
	~MSJExceptionHandler( );

	void SetLogFileName( PTSTR pszLogFileName ); // file name..
	void GameMessage(const char *cstr, ...);
	void SockMessage(const char *cstr, ...);
	void SetSystemInfo(char* videocard, char* cpu, char* os, ULONG memory, ULONG vmemory);
	char* GetGameMessage() { return m_strGameExBuffer; };
	void GenerateExceptionReport(char* error);

	
	static char m_strVideo[256];
	static char m_strCpu[256];
	static char m_strOs[256];
	static ULONG m_ulMemory;
	static ULONG m_ulVMemory;
	
	static char m_strGameExBuffer[1024];
	static char m_strSockExBuffer[1024];

	private: 

	// entry point where control comes on an unhandled exception
	static LONG WINAPI MSJUnhandledExceptionFilter(
								PEXCEPTION_POINTERS pExceptionInfo );

	// where report info is extracted and generated	
	static void GenerateExceptionReport( PEXCEPTION_POINTERS pExceptionInfo );

	// Helper functions
	static LPTSTR GetExceptionString( DWORD dwCode );
	static BOOL GetLogicalAddress( 	PVOID addr, PTSTR szModule, DWORD len,
									DWORD& section, DWORD& offset );
	static void IntelStackWalk( PCONTEXT pContext );
	#if 1
	static void ImagehlpStackWalk( PCONTEXT pContext );
	#endif
	static int __cdecl _tprintf(const TCHAR * format, ...);

	#if 1
	static BOOL InitImagehlpFunctions( void );
	#endif
		
	// Variables used by the class
	static TCHAR m_szLogFileName[MAX_PATH];
	static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
	static HANDLE m_hReportFile;

	static char* m_strDebug; 

	#if 1
	// Make typedefs for some IMAGEHLP.DLL functions so that we can use them
	// with GetProcAddress
	typedef BOOL (__stdcall * SYMINITIALIZEPROC)( HANDLE, LPSTR, BOOL );
	typedef BOOL (__stdcall *SYMCLEANUPPROC)( HANDLE );

	typedef BOOL (__stdcall * STACKWALKPROC)
					( DWORD, HANDLE, HANDLE, LPSTACKFRAME, LPVOID,
					PREAD_PROCESS_MEMORY_ROUTINE,PFUNCTION_TABLE_ACCESS_ROUTINE,
					PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE );
	
	typedef LPVOID (__stdcall *SYMFUNCTIONTABLEACCESSPROC)( HANDLE, DWORD );

	typedef DWORD (__stdcall *SYMGETMODULEBASEPROC)( HANDLE, DWORD );

	typedef BOOL (__stdcall *SYMGETSYMFROMADDRPROC)
								( HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL );

	static SYMINITIALIZEPROC _SymInitialize;
	static SYMCLEANUPPROC _SymCleanup;
	static STACKWALKPROC _StackWalk;
	static SYMFUNCTIONTABLEACCESSPROC _SymFunctionTableAccess;
	static SYMGETMODULEBASEPROC _SymGetModuleBase;
	static SYMGETSYMFROMADDRPROC _SymGetSymFromAddr;

	#endif
};

extern MSJExceptionHandler* g_MSJExceptionHandler;	//  global instance of class

#endif
