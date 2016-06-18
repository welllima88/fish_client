// FISHDoc.cpp : implementation of the CFISHDoc class
//

#include "stdafx.h"
#include "FISH.h"

#include "FISHDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFISHDoc

IMPLEMENT_DYNCREATE(CFISHDoc, CDocument)

BEGIN_MESSAGE_MAP(CFISHDoc, CDocument)
	//{{AFX_MSG_MAP(CFISHDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFISHDoc construction/destruction

CFISHDoc::CFISHDoc()
{
	TRACE(_T("CFISHDoc Destroy\r\n"));
}

CFISHDoc::~CFISHDoc()
{
}

BOOL CFISHDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CFISHDoc serialization

void CFISHDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFISHDoc diagnostics

#ifdef _DEBUG
void CFISHDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFISHDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFISHDoc commands
