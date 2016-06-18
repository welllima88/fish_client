// FishLinkManager.cpp : implementation file
//

#include "stdafx.h"
#include "fish.h"
#include "fish_common.h"
#include "FishLinkManager.h"
#include "Dlg_MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STR_FLM_INFORM_LINK				_T("FISH와 FSH 링크가 연결되어 있지 않습니다.") \
										_T(" 지금 FISH를 fish 링크와 연결하시겠습니까(권장)?")

/////////////////////////////////////////////////////////////////////////////
// CFishLinkManager
CFishLinkManager* CFishLinkManager::_instance   =   NULL;

CFishLinkManager::CFishLinkManager()
{

}

CFishLinkManager::~CFishLinkManager()
{
}


BEGIN_MESSAGE_MAP(CFishLinkManager, CWnd)
	//{{AFX_MSG_MAP(CFishLinkManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFishLinkManager message handlers
bool    CFishLinkManager::HaveFishRegAccess()
{
	CRegKey regkey;
	DWORD dwRegResult = regkey.Create(HKEY_CLASSES_ROOT, _T("fish\\shell\\open\\command"));
	regkey.Close();
	return (dwRegResult == ERROR_SUCCESS);
}

bool    CFishLinkManager::Ask4RegFix(bool checkOnly, bool dontAsk)
{
	// Barry - Make backup first
	if (!checkOnly)
		BackupReg();

	// check registry if fish links is assigned to emule
	CRegKey regkey;
	if (regkey.Create(HKEY_CLASSES_ROOT, _T("fish\\shell\\open\\command")) == ERROR_SUCCESS)
	{
		TCHAR rbuffer[500];
		ULONG maxsize = ARRSIZE(rbuffer);
		regkey.QueryValue(NULL, rbuffer, &maxsize);

		TCHAR modbuffer[490];
#ifdef _UNICODE
		_tcscpy(modbuffer, __wargv[0]);
#else
		_tcscpy(modbuffer, __argv[0]);
#endif
		CString strCanonFileName = modbuffer;
		strCanonFileName.Replace(_T("%"), _T("%%"));

		TCHAR regbuffer[520];
		_sntprintf(regbuffer, ARRSIZE(regbuffer), _T("\"%s\" \"%%1\""), strCanonFileName);
		if (_tcscmp(rbuffer, regbuffer) != 0)
		{
			if (checkOnly)
				return true;
//			if (dontAsk || (AfxMessageBox(STR_FLM_INFORM_LINK , MB_ICONQUESTION|MB_YESNO) == IDYES))
            if (dontAsk || (FishMessageBox(STR_FLM_INFORM_LINK , FMB_ICONQUESTION | FMB_YESNO) == IDYES))
			{
				regkey.SetValue( regbuffer, (LPCTSTR) NULL);	
				
				regkey.Create(HKEY_CLASSES_ROOT, _T("fish\\DefaultIcon"));
				regkey.SetValue( modbuffer, (LPCTSTR) NULL );

				regkey.Create(HKEY_CLASSES_ROOT, _T("fish"));
				regkey.SetValue( _T("URL: fish Protocol"), (LPCTSTR) NULL);
				regkey.SetValue( _T(""), _T("URL Protocol") );

				regkey.Open(HKEY_CLASSES_ROOT, _T("fish"));
				regkey.RecurseDeleteKey(_T("ddexec"));
				regkey.RecurseDeleteKey(_T("ddeexec"));
			}
		}
		regkey.Close();
	}
	return false;
}

void    CFishLinkManager::BackupReg(void)
{
	// Look for pre-existing old fish links
	CRegKey regkey;
	if (regkey.Create(HKEY_CLASSES_ROOT, _T("fish\\shell\\open\\command")) == ERROR_SUCCESS)
	{
		TCHAR rbuffer[500];
		ULONG maxsize = ARRSIZE(rbuffer);
		// Is it ok to write new values
		if ((regkey.QueryValue(_T("OldDefault"), rbuffer, &maxsize) != ERROR_SUCCESS) || (maxsize == 0))
		{
			maxsize = ARRSIZE(rbuffer);
			if ( regkey.QueryValue(NULL, rbuffer, &maxsize) == ERROR_SUCCESS )
				regkey.SetValue( rbuffer, _T("OldDefault") );

			regkey.Create(HKEY_CLASSES_ROOT, _T("fish\\DefaultIcon"));
			maxsize = ARRSIZE(rbuffer);
			if (regkey.QueryValue(NULL, rbuffer, &maxsize) == ERROR_SUCCESS)
				regkey.SetValue( rbuffer, _T("OldIcon") );
		}
		regkey.Close();
	}
}

void    CFishLinkManager::RevertReg(void)
{
	// restore previous fish links before being assigned to emule
	CRegKey regkey;
	if (regkey.Create(HKEY_CLASSES_ROOT, _T("fish\\shell\\open\\command")) == ERROR_SUCCESS)
	{
		TCHAR rbuffer[500];
		ULONG maxsize = ARRSIZE(rbuffer);
		if (regkey.QueryValue(_T("OldDefault"), rbuffer, &maxsize) == ERROR_SUCCESS)
		{
			regkey.SetValue( rbuffer, (LPCTSTR) NULL );
			regkey.DeleteValue(_T("OldDefault"));
			regkey.Create(HKEY_CLASSES_ROOT, _T("fish\\DefaultIcon"));
			maxsize = ARRSIZE(rbuffer);
			if (regkey.QueryValue(_T("OldIcon"), rbuffer, &maxsize) == ERROR_SUCCESS)
			{
				regkey.SetValue( rbuffer, (LPCTSTR) NULL );
				regkey.DeleteValue(_T("OldIcon"));
			}
		}
		regkey.Close();
	}
}