#include "fish_struct.h"

#define NORMAL		0
#define SEARCH		1

#define HM_NOTREVERSE	0x00000001
#define HM_PAPERSTYLE	0x00000002
#define HM_DESCSUMMARY	0x00000004
//#define HM_STYLEMASK	0x0000000f

#define HM_LOADLOCAL	0x00000010
#define HM_LOADSTRING	0x00000030
#define HM_LOADWEB		0x00000070
#define HM_LOADONLYCNT	0x000000f0
#define HM_LOADMASK		0x000000f0

class CHtmlMaker
{
public:

	CHtmlMaker() {}
	~CHtmlMaker() {}
	
	static BOOL		MakeHtml_Xslt(SmartPtr<POST_LIST>& postlist, CString& htmlstream, CString xsltfile = _T(""), DWORD flag = HM_LOADLOCAL);
	static BOOL		MakeHtml_Xslt(POSTITEM& post, CString& htmlstream, CString xsltfile = _T(""), DWORD flag = HM_LOADLOCAL);

	BOOL			MakeHtml(SmartPtr<POST_LIST> &postlist, CString xsltfile = _T(""), DWORD flag = HM_LOADLOCAL);
	BOOL			MakeHtml(POSTITEM& post, CString xsltfile = _T(""), DWORD flag = HM_LOADLOCAL);
	CString			GetHtmlStream()	{ return m_strHtml; }
	void			Init() { m_strHtml.Empty(); }
	BOOL			Save(CString filepath);

protected:
	
	CString			m_strHtml;	
};


class CTimeTick
{
private:
    LARGE_INTEGER iPerfFreq;
    LARGE_INTEGER iPerfStart;
    LARGE_INTEGER iPerfEnd;

public:
    CTimeTick()
    {
        QueryPerformanceFrequency(&iPerfFreq);
    }

    ~CTimeTick()
    {
    }

    void SetStart()
    {
        QueryPerformanceCounter(&iPerfStart);  // start point		
    }

    double SetEnd()
    {
		QueryPerformanceCounter(&iPerfEnd);    // end point
		QueryPerformanceFrequency(&iPerfFreq);
        return  ((double)(iPerfEnd.QuadPart - iPerfStart.QuadPart))/(double)iPerfFreq.QuadPart;
    }
};