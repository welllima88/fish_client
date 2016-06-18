// FISH.h : main header file for the FISH application
//

#if !defined(AFX_FISH_H__7E0F3674_DA40_4BC2_8C5D_FD778507919B__INCLUDED_)
#define AFX_FISH_H__7E0F3674_DA40_4BC2_8C5D_FD778507919B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CLocalProperties;
class CGnuDoc;
class CRSSDB;
class CRSSCurrentList;
class CSearchManager;
class COPMLSock;
class CCustomOccManager;

/////////////////////////////////////////////////////////////////////////////
// CFISHApp:
// See FISH.cpp for the implementation of this class
//
class CFISHApp : public CWinApp
{
public:
	// data sending
	tagCOPYDATASTRUCT	sendstruct;
	CString*			pendinglink;
	// --

	// Check Already Running
	bool				m_bRunAleady;
	HWND				m_hmaininst;
	HANDLE				m_hMutexOneInstance;
	// --
	SmartPtr<CLocalProperties>	m_spLP;
	SmartPtr<CGnuDoc>			m_spGD;
	SmartPtr<CRSSDB>			m_spRD;
	SmartPtr<CRSSCurrentList>	m_spCL;
	SmartPtr<CSearchManager>	m_spSM;
	SmartPtr<COPMLSock>			m_spOS;

	class CImpIDispatch* m_pDispOM;

public:
	int		m_ntotalpostcnt;
	int		m_nnetpostcnt;
	int		m_nnetusercnt;

	HWND GetMainInstHWND()	{ return m_hmaininst; }
	BOOL IsClosing();
	BOOL IsExiting();
	void SetClosing(BOOL bclose);
	void SetExit(BOOL bexit) { m_bExit = bexit; }
    void OnFocusOnAddrCombo();
	CFISHApp();
	~CFISHApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFISHApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CFISHApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
//	BOOL	m_bATLInited;
	BOOL	m_bClosing;
	BOOL	m_bExit;
	CCustomOccManager *pMgr;

private:
	// Check Already Running
	static BOOL CALLBACK SearchFishWindow(HWND hWnd, LPARAM lParam);
	void SendArgumentToAnother();
	bool CheckRun();
protected:
	BOOL ProcessCommandline(CCommandLineInfo& cmdInfo);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FISH_H__7E0F3674_DA40_4BC2_8C5D_FD778507919B__INCLUDED_)
extern CFISHApp theApp;