#if !defined(AFX_GNUROUTING_H__2DFCB340_8BEE_11D4_ACF2_00A0CC533D52__INCLUDED_)
#define AFX_GNUROUTING_H__2DFCB340_8BEE_11D4_ACF2_00A0CC533D52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define TABLE_SIZE			130		// Size of each hash table
#define MAX_REHASH			2		// How many rehashes we try before we consider the current
									// table too full, flush the old table, and swap tables.

#include <vector> 

using namespace std;

class CGnuNode;

struct key_Value
{
	GUID Guid;
	CGnuNode* Origin;
};

// Add[Modify] at 2003-03-31 오후 9:14:41 by bemlove 
struct search_Value
{
	char uid[20+1];
	char sword[100+1];

	search_Value( void )
	{
		uid[0]		= 0L;
		sword[0]	= 0L;
	}
	void setValue( char* id, char* word )
	{
		sprintf( uid, "%s", id );
		sprintf( sword, "%s", word );
	}

	bool operator==(search_Value & val )
	{
		if( (strcmp(uid, val.uid )==0) &&
			(strcmp(sword, val.sword)==0 ) )
			return true;
		return false;
	}
};

#define MAX_SEARCH_VALUE	100
class CGnuRouting  
{
public:
	CGnuRouting();
	virtual ~CGnuRouting();

	void Insert(GUID*, CGnuNode*);
	key_Value* FindValue(GUID*);

	int m_nHashEntries;

	CTime	  RefreshTime;
	CTimeSpan HashTimeSpan;


private:
	DWORD CreateKey(GUID*);
	bool  CompareGuid(GUID*, GUID*);
	void  ClearTable(int which);
	
	std::vector<key_Value> m_Table[2][TABLE_SIZE];

	CCriticalSection m_AccessTable;

	int m_nCurrent, 
		m_nOld;

// Add[Modify] at 2003-03-31 오후 9:14:45 by bemlove 
	search_Value	m_searchValue[MAX_SEARCH_VALUE];
	UINT			m_nCurrentPos;
	bool			m_bCycled;

public:
	void			InsertValue( char* id, char* word );
	bool			FindSearchValue( char* id, char* word );
};

#endif // !defined(AFX_GNUROUTING_H__2DFCB340_8BEE_11D4_ACF2_00A0CC533D52__INCLUDED_)
