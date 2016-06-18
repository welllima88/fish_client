#if !defined(AFX_GNULOCAL_H__14B5ACE9_0402_4CD9_A995_E140E876549F__INCLUDED_)
#define AFX_GNULOCAL_H__14B5ACE9_0402_4CD9_A995_E140E876549F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Packet.h"
#include <vector>


class CGnuControl;

class CGnuLocal : public CAsyncSocket
{
public:
	CGnuLocal(CGnuControl*);
	virtual ~CGnuLocal();

	void Init();
	void SendPing();

	std::vector<LanNode> m_HostList;


	//{{AFX_VIRTUAL(CGnuLocal)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGnuLocal)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG


protected:
	CAsyncSocket m_OutSock;

	int  m_Broadcasted;

	CGnuControl*  m_pComm;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GNULOCAL_H__14B5ACE9_0402_4CD9_A995_E140E876549F__INCLUDED_)
