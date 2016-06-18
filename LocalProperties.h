// LocalProperties.h: interface for the CLocalProperties class.
//
//////////////////////////////////////////////////////////////////////

/**************************************************************************
 * class CLocalProperties
 *
 * written by moonknit
 *
 * @Project Name
 * Fish
 *
 * @History
 * created 2005-12-13
 *
 * @Description
 * Fish의 각종 설정값을 관리하기 위한 클래스이다.
 **************************************************************************/

#if !defined(AFX_LOCALPROPERTIES_H__56AE0955_C12F_440B_B0CA_418D9306759E__INCLUDED_)
#define AFX_LOCALPROPERTIES_H__56AE0955_C12F_440B_B0CA_418D9306759E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// OPML Sync
#define FLAG_REG_USERID							0x00000001
#define FLAG_REG_PASSWORD						0x00000002
#define FLAG_REG_AUTOLOGIN						0x00000004
#define FLAG_REG_LASTOPMLTIME					0x00000008
#define FLAG_REG_SAVEPASSWORD					0x00000010
#define FLAG_REG_USEOPMLSYNC					0x00000020

// General1
#define FLAG_REG_USERPATH						0x00000001
#define FLAG_REG_AUTOSTART						0x00000002
#define FLAG_REG_CLOSEBTNEXIT					0x00000004
#define FLAG_REG_IRCID							0x00000008
#define FLAG_REG_SEARCHSAVE						0x00000010
#define FLAG_REG_POSTAUTODELETE					0x00000020

// General2
#define FLAG_REG_DOWNLOADPATH					0x00000002
#define FLAG_REG_PPP							0x00000004
#define FLAG_REG_USERINFORMWND					0x00000008
#define FLAG_REG_STYLE							0x00000010
#define FLAG_REG_NOTUSEP2P						0x00000020
#define FLAG_REG_SRL							0x00000040

// General3
#define FLAG_REG_LISTDESCSUMMARY				0x00000001
#define FLAG_REG_PAPERSTYLE						0x00000002

// Search Properties Flag
#define FLAG_REG_SWREAD							0x00000001
#define FLAG_REG_SWLINK							0x00000002
#define FLAG_REG_SWHIT							0x00000004
#define FLAG_REG_SWFILTER						0x00000008
#define FLAG_REG_SWSCRAP						0x00000010
#define FLAG_REG_SWBASE							0x00000020
#define FLAG_REG_SWREADLIMIT					0x00010000
#define FLAG_REG_SWLINKLIMIT					0x00020000
#define FLAG_REG_SWHITLIMIT						0x00040000
#define FLAG_REG_SWFILTERLIMIT					0x00080000
#define FLAG_REG_SWSCRAPLIMIT					0x00100000


class CLocalProperties  
{
protected:
	void MakeAutoStart();
	void InitProperties();
	void MakeFirstStart();

	// 최초 실행 플래그
	BOOL		bFirst;

	// 기본 사용자 ID 여부
	BOOL		bDefaultUser;

	// 사용자 초기화 여부
	BOOL		bUserInit;

	// GMT 정보
	BOOL		bGmtSet;
	long		bias;

	// Explorer의 즐겨찾기 디렉토리
	CString		sFavoritesDir;

	// 사용자 ID
	CString		sUserID;
	CString		sTempUserID;

	// 사용자 암호 encoded
	CString		sPassword;

	// User Sequence
	int			nUserSeq;

	// IRC ID
	char		szIRCID[20+1];

	// Handler Key
	char		szHandlerKey[20+1];

	// 사용자의 OPML 의 최종 갱신 시간
	CString		sLastOPMLTime;

	// OPML Sync 사용 여부
	BOOL		bUseOPMLSync;
	BOOL		bOPMLConnected;

	// IRC 접속 여부
	BOOL		bIRCConnected;

	// flags
	BOOL		bAutoLogin;
	BOOL		bSavePassword;

	BOOL		bAutoStart;
	BOOL		bCloseBtnExit;

	BOOL		bSearchSave;

	BOOL		bNotUseP2P;

	BOOL		bPostAutoDelete;

	// 검색 결과 허가 개수
	UINT		uSearchResultLimit;

	BOOL		bListDescSummary; // 리스트 보기시 본문 모두 보기

	// Inform Wnd Flag
	BOOL		bUseInformWnd;

	// USER'S GMT INFORMATION (TIME SPAN)
	COleDateTimeSpan	spgmt;

	// POSTS PER PAGE
	int			ppp;

	// 각종 폴더
//	CString		UserPath;		// 사용자 폴더 위치2 - UserPathParent + UserID
	CString		UserPathParent;	// 사용자 폴더 위치
	CString		UserPathTemp;	// 이전 사용자의 사용자 폴더
	CString		SaveDataPath;	// 검색 결과 저장 위치
	CString		TempDataPath;	// 임시 파일 생성 위치 
	CString		PostDataPath;

	CString		StyleFile;		// Style 파일
	CString		PaperStyleFile; // 페이퍼 스타일 파일

	// DWORD FLAG FOR MODIFIED DATA CHECK
	DWORD		dwFlagMain1;
	DWORD		dwFlagMain2;
	DWORD		dwFlagMain3;
	DWORD		dwFlagSearch;
	DWORD		dwFlagOPML;

	// HWNDs
	HWND		hSubscribeWnd;
	HWND		hMainWnd;

	double		fSWRead;
	double		fSWLink;
	double		fSWScrap;
	double		fSWFilter;
	double		fSWHit;
	double		fSWBase;

	double		fSWReadLimit;
	double		fSWLinkLimit;
	double		fSWScrapLimit;
	double		fSWFilterLimit;
	double		fSWHitLimit;
	
	
public:
	// HWNDs method
	void SetSubscribeHwnd(HWND hFlag)	{ hSubscribeWnd = hFlag; }
	HWND GetSubscribeHwnd()				{ return hSubscribeWnd; }
	void SetMainHwnd(HWND hFlag)		{ hMainWnd = hFlag; }
	HWND GetMainHwnd()					{ return hMainWnd; } 

	void MakeRandomIRCID();
	
	int GetUserSeq()					{ return nUserSeq; }
	void SetUserSeq(int seq);

	static ULONGLONG ReCalcDownPath(CString path);

	void CopyTempIDTo()					{ sUserID = sTempUserID; dwFlagOPML |= FLAG_REG_USERID; }
	void SetTempUserID(CString s)		{ sTempUserID = s; }
	CString GetTempUserID()				{ return sTempUserID; }

	void SetUserID(CString s)			{ sUserID = s; dwFlagOPML |= FLAG_REG_USERID; }
	CString GetUserID()					{ return sUserID; }

	bool SetDownloadPath(CString sPath);
	CString GetDownloadPath()			{ return DownloadPath; }
	
	CString GetEncodedPassword()		{ return sPassword; }
	void SetPassword(CString sEncPass)	{ if(sPassword != sEncPass) { sPassword = sEncPass; dwFlagOPML |= FLAG_REG_PASSWORD; } }

	CString GetLastOPMLTime()			{ return sLastOPMLTime; }
	void SetLastOPMLTime(CString sTime) { if(sLastOPMLTime != sTime)	{sLastOPMLTime = sTime; dwFlagOPML |= FLAG_REG_LASTOPMLTIME; } }

	COleDateTimeSpan GetSPGMT()			{ return spgmt; }

	BOOL IsIRCConnected()				{ return bIRCConnected; }
	BOOL IsOPMLConnected()				{ return bOPMLConnected; }
	BOOL CanOPMLUse()					{ return bIRCConnected && bOPMLConnected; }
	void SetIRCConnected(BOOL bFlag)	{ bIRCConnected = bFlag; }
	void SetOPMLConnected(BOOL bFlag)	{ bOPMLConnected = bFlag; }

	BOOL GetAutoLogin()					{ return bAutoLogin; }
	BOOL GetSavePassword()				{ return bSavePassword; }
	BOOL GetUseOPMLSync()				{ return bUseOPMLSync; }
	void SetAutoLogin(BOOL bFlag)		{ if(bFlag != bAutoLogin)		{bAutoLogin = bFlag; dwFlagOPML |= FLAG_REG_AUTOLOGIN; } }
	void SetSavePassword(BOOL bFlag)	{ if(bFlag != bSavePassword)	{bSavePassword = bFlag; dwFlagOPML |= FLAG_REG_SAVEPASSWORD; } }
	void SetUseOPMLSync(BOOL bFlag)		{ if(bFlag != bUseOPMLSync)		{bUseOPMLSync = bFlag; dwFlagOPML |= FLAG_REG_USEOPMLSYNC; } }

	BOOL GetAutoStart()					{ return bAutoStart; }
	BOOL GetCloseBtnExit()				{ return bCloseBtnExit; }
	BOOL GetUseInformWnd()				{ return bUseInformWnd; }
	BOOL GetForceSave()					{ return bSearchSave; }
	CString GetUserPath()				{ return UserPathParent; }
	CString GetPostDataPath()			{ return PostDataPath; }
	CString GetTempDataPath()			{ return TempDataPath; }

	void SetAutoStart(BOOL bFlag)		{ if(bFlag != bAutoStart)		{bAutoStart = bFlag; dwFlagMain1 |= FLAG_REG_AUTOSTART; } }
	void SetCloseBtnExit(BOOL bFlag)	{ if(bFlag != bCloseBtnExit)	{bCloseBtnExit = bFlag; dwFlagMain1 |= FLAG_REG_CLOSEBTNEXIT; } }
	void SetUseInformWnd(BOOL bFlag)	{ if(bFlag != bUseInformWnd)	{bUseInformWnd = bFlag; dwFlagMain1 |= FLAG_REG_USERINFORMWND; } }
	void SetSearchSave(BOOL bFlag);
	void SetUserPath(CString sFlag);

	char* GetIRCID() { return szIRCID; }
	void SetIRCID(char* pid);

	int GetPPP()						{ return ppp; }
	CString GetStyleFile()				{ return StyleFile; }
	void SetPPP(int nFlag)				{ if(nFlag != ppp)				{ ppp = nFlag; dwFlagMain2 |= FLAG_REG_PPP; } }
	void SetStyleFile(CString sFlag)	{ if(sFlag != StyleFile)		{ StyleFile = sFlag; dwFlagMain2 |= FLAG_REG_STYLE; } }

	CString GetPartialDir();

	CString GetFavoritesDir();

	double GetSWRead()					{ return fSWRead; }
	double GetSWLink()					{ return fSWLink; }
	double GetSWHit()					{ return fSWHit; }
	double GetSWScrap()					{ return fSWScrap; }
	double GetSWFilter()				{ return fSWFilter; }
	double GetSWBase()					{ return fSWBase; }
	void SetSWRead(double fFlag)		{ if(fFlag != fSWRead)			{fSWRead = fFlag; dwFlagSearch |= FLAG_REG_SWREAD; } }
	void SetSWLink(double fFlag)		{ if(fFlag != fSWLink)			{fSWLink = fFlag; dwFlagSearch |= FLAG_REG_SWLINK; } }
	void SetSWHit(double fFlag)			{ if(fFlag != fSWHit)			{fSWHit = fFlag; dwFlagSearch |= FLAG_REG_SWHIT; } }
	void SetSWScrap(double fFlag)		{ if(fFlag != fSWScrap)			{fSWScrap = fFlag; dwFlagSearch |= FLAG_REG_SWSCRAP; } }
	void SetSWFilter(double fFlag)		{ if(fFlag != fSWFilter)		{fSWFilter = fFlag; dwFlagSearch |= FLAG_REG_SWFILTER; } }
	void SetSWBase(double fFlag)		{ if(fFlag != fSWBase)			{if(fFlag <= 0) fFlag = 100; fSWBase = fFlag; dwFlagSearch |= FLAG_REG_SWBASE; } }
	double GetSWReadLimit()				{ return fSWReadLimit; }
	double GetSWLinkLimit()				{ return fSWLinkLimit; }
	double GetSWHitLimit()				{ return fSWHitLimit; }
	double GetSWScrapLimit()			{ return fSWScrapLimit; }
	double GetSWFilterLimit()			{ return fSWFilterLimit; }
	void SetSWReadLimit(double fFlag)	{ if(fFlag != fSWReadLimit)		{fSWReadLimit = fFlag; dwFlagSearch |= FLAG_REG_SWREADLIMIT; } }
	void SetSWLinkLimit(double fFlag)	{ if(fFlag != fSWLinkLimit)		{fSWLinkLimit = fFlag; dwFlagSearch |= FLAG_REG_SWLINKLIMIT; } }
	void SetSWHitLimit(double fFlag)	{ if(fFlag != fSWHitLimit)		{fSWHitLimit = fFlag; dwFlagSearch |= FLAG_REG_SWHITLIMIT; } }
	void SetSWScrapLimit(double fFlag)	{ if(fFlag != fSWScrapLimit)	{fSWScrapLimit = fFlag; dwFlagSearch |= FLAG_REG_SWSCRAPLIMIT; } }
	void SetSWFilterLimit(double fFlag)	{ if(fFlag != fSWFilterLimit)	{fSWFilterLimit = fFlag; dwFlagSearch |= FLAG_REG_SWHITLIMIT; } }

	BOOL GetNotUseP2P()					{ return bNotUseP2P; }
	void SetNotUseP2P(BOOL bFlag)		{ if(bFlag != bNotUseP2P)		{bNotUseP2P = bFlag; dwFlagMain2 |= FLAG_REG_NOTUSEP2P; } }

	UINT GetSRL()						{ return uSearchResultLimit; }
	void SetSRL(UINT uFlag)				{ if(uFlag != uSearchResultLimit)	{ uSearchResultLimit = uFlag; dwFlagMain2 |= FLAG_REG_SRL; } }

	BOOL GetAutoDelete()				{ return bPostAutoDelete; }
	void SetAutoDelete(BOOL bFlag)		{ if(bFlag != bPostAutoDelete)		{bPostAutoDelete = bFlag; dwFlagMain1 |= FLAG_REG_POSTAUTODELETE; } }

	BOOL	GetListDescSummary()		{ return bListDescSummary; } 
	void	SetListDescSummary(BOOL bFlag)	{ if(bFlag != bListDescSummary) { bListDescSummary = bFlag; dwFlagMain3 |= FLAG_REG_LISTDESCSUMMARY; } }
	CString GetPaperStyle()				{ return PaperStyleFile; }
	void	SetPaperStyle(CString sFlag)	{ if(sFlag != PaperStyleFile)	{ PaperStyleFile = sFlag; dwFlagMain3 |= FLAG_REG_PAPERSTYLE; } }

	void LoadOPMLInform(CString userid);
	void SaveProperties();
	void LoadProperties();
	CLocalProperties();
	virtual ~CLocalProperties();

	bool		PartialsInDir;  // 멀티 다운로드시 Partial 파일 생성 위치 flag
	CString		DownloadPath;
	CString		RunPath;		// 실행파일 폴더 위치
};

extern void GuidtoIRCID(GUID inGuid, char* pid, int len);

#endif // !defined(AFX_LOCALPROPERTIES_H__56AE0955_C12F_440B_B0CA_418D9306759E__INCLUDED_)
