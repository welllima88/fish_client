#if !defined(AFX_GNUWORDHASH_H__25474FC8_F0CB_44E6_9EC5_E401C7A02C09__INCLUDED_)
#define AFX_GNUWORDHASH_H__25474FC8_F0CB_44E6_9EC5_E401C7A02C09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define TABLE_BITS		16
#define TABLE_INFINITY   2


class CGnuShare;
class CGnuNode;
class CGnucleusDoc;


class CGnuWordHash  
{
public:
	CGnuWordHash(CGnuShare* pShare);
	virtual ~CGnuWordHash();

	void ClearLocalTable();
	void ResetTable(CGnuNode* ResetNode);
	void ApplyPatch(CGnuNode* pNode, int EntryBits);
	
	void InsertFilename(std::basic_string<char> Name, int Index);
	void InsertHash(int Index);

	void BreakupName(std::basic_string<char> Name, std::vector< std::basic_string<char> > &Keywords);

	void AddWord(std::vector< std::basic_string<char> > &Keywords, std::basic_string<char> Word);

	void LookupQuery(std::basic_string<char> Query, std::list<UINT> &Indexes, std::list<CGnuNode*> &RemoteNodes,QueryComp SID);
	void LookupHash(std::basic_string<char> Sha1Hash, std::list<UINT> &Indexes, std::list<CGnuNode*> &RemoteNodes);

	bool IntersectIndexes(std::list<UINT> &Index, std::vector<UINT> &CompIndex);
	bool IntersectNodes(std::list<CGnuNode*> &Nodes, std::list<CGnuNode*> &CompNodes);

	UINT Hash(std::basic_string<char> x, byte bits);
	UINT HashFast(UINT64 x, byte bits);


	WordKey m_HashTable[1 << TABLE_BITS];

	char    m_PatchTable[1 << TABLE_BITS];

	UINT m_TableSize;
	UINT m_HashedWords;
	UINT m_LargestRehash;
	UINT m_UniqueSlots;
	UINT m_RemoteSlots;

	CGnuShare*    m_pShare;
	CGnucleusDoc* m_pDoc;
};


#endif // !defined(AFX_GNUWORDHASH_H__25474FC8_F0CB_44E6_9EC5_E401C7A02C09__INCLUDED_)
