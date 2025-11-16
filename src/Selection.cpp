// Selection.cpp : implementation file
//

#include "stdafx.h"
#include "PatnDoc.h"
#include "Selection.h"
#include "TableView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelection
IMPLEMENT_DYNAMIC( CSelection, CObject )

CSelection::CSelection(CTableView* pTableView)
{
	ASSERT(pTableView->IsKindOf(RUNTIME_CLASS(CTableView)));
	m_pView = pTableView;

	Initialize();
}

CSelection::~CSelection()
{
	m_map.RemoveAll();
}

void CSelection::Initialize()
{
	m_idxMin = -1;
	m_idxMax = -1;
	m_idxAnchor = -1;
	m_idxActiveEnd = -1;
}

void CSelection::SetAt(long idx)
{
	m_map.SetAt(idx, idx);
	if ( m_map.GetCount() == 1 )		// First element
	{
		m_idxMin = idx;
		m_idxMax = idx;
	}
	else
	{
		m_idxMin = min(m_idxMin, idx);
		m_idxMax = max(m_idxMax, idx);
	}
}

void CSelection::RemoveAt(long idx)
{
	long nValue;
	long i;

	m_map.RemoveKey(idx);

	// if the idx to be removed is the minimum in the selection,
	// then find a new minimum before removing the idx from the map
	if ( idx == m_idxMin )
	{
		i = idx;
		while ( i < m_idxMax )
		{
			if ( m_map.Lookup(i,nValue) )
				break;

			i++;
		}
		m_idxMin = i;
	}
	// if the idx to be removed is the maximum in the selection,
	// then find a new maximum before removing the idx from the map
	else if ( idx == m_idxMax )
	{
		i = idx;
		while ( i > m_idxMin )
		{
			if ( m_map.Lookup(i,nValue) )
				break;

			i--;
		}
		m_idxMax = i;
	}
	
	if ( m_map.IsEmpty() )
		Initialize();
}

void CSelection::RemoveAll()
{
	m_map.RemoveAll();
	Initialize();
}

BOOL CSelection::IsContiguous()
{
	if ( m_map.GetCount() != (long)(m_idxMax - m_idxMin + 1) )
		return FALSE;
	else
		return TRUE;
}

void CSelection::GetItems(CArray<int>* pArray, BOOL bSort /*= TRUE*/)
// returns all selected items in an array, optionally sorted
{
	POSITION pos;
	long nCount;
	long i;
	long idx;
	long nValue;

	// make sure the array is empty
	pArray->RemoveAll();

	if ( (nCount = m_map.GetCount()) == 0 )
		return;

	pArray->SetSize(nCount);
	i = 0;
	pos = m_map.GetStartPosition();
	while ( pos != NULL )
	{
		m_map.GetNextAssoc(pos,idx,nValue);
		pArray->SetAt(i,idx);
		i++;
	}

	if ( bSort )
		::sort(pArray);
}

long CSelection::GetFirst()
{
	long idx;
	long nValue;

	if ( (m_pos = m_map.GetStartPosition()) == NULL )
		return -1;

	m_map.GetNextAssoc(m_pos,idx,nValue);
	return idx;
}

long CSelection::GetNext()
{
	long idx;
	long nValue;

	if ( m_pos == NULL )
		return -1;

	m_map.GetNextAssoc(m_pos,idx,nValue);
	return idx;
}

void CSelection::Shift(long nCount)
{
	long idx;
	long nValue;
	POSITION pos;

	pos = m_map.GetStartPosition();
	while ( pos != NULL )
	{
		m_map.GetNextAssoc(pos,idx,nValue);
		RemoveAt(idx);
		idx += nCount;
		SetAt(idx);
	}
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CColumnSelection
IMPLEMENT_DYNAMIC( CColumnSelection, CSelection )

CColumnSelection::CColumnSelection(CTableView* pTableView)
: CSelection(pTableView)
{
}

CColumnSelection * CColumnSelection::Copy() {
	CColumnSelection * c = new CColumnSelection(this->m_pView);
	c->m_idxActiveEnd = m_idxActiveEnd;
	c->m_idxAnchor = m_idxAnchor;
	c->m_idxMax = m_idxMax;
	c->m_idxMin = m_idxMin;
	c->m_pos = m_pos;
	c->m_pView = m_pView;
	
	long idx;
	long nValue;
	POSITION pos;

	pos = m_map.GetStartPosition();
	while ( pos != NULL )
	{
		m_map.GetNextAssoc(pos,idx,nValue);
		c->m_map.SetAt(idx, nValue);
	}

	return c;
}
void CColumnSelection::StartActiveSelection(long idx, BOOL bRemove /*= TRUE*/)
{
	if ( bRemove )
		RemoveAll();

	SetAt(idx);

	m_idxActiveEnd = m_idxAnchor = idx;
}

void CColumnSelection::UpdateActiveSelection(long idx, BOOL bRemove /*= TRUE*/)
{
	long i;
	long idxLower, idxUpper;
	POSITION pos;
	long nValue;

	if ( m_map.IsEmpty() )
	{
		StartActiveSelection(idx,bRemove);
		return;
	}

	if ( !IsSelected(m_idxAnchor) || !IsSelected(m_idxActiveEnd) )
	{
		StartActiveSelection(idx,bRemove);
		return;
	}

	// remove all idxs outside of active selection?
	if ( bRemove )
	{
		idxLower = min(m_idxAnchor, m_idxActiveEnd);
		idxUpper = max(m_idxAnchor, m_idxActiveEnd);
		pos = m_map.GetStartPosition();
		while ( pos != NULL )
		{
			m_map.GetNextAssoc(pos,i,nValue);
			if ( i < idxLower || i > idxUpper )
				RemoveAt(i);
		}
	}

	if ( idx == m_idxActiveEnd ) // no change to active selection
		return;

	if ( idx > m_idxActiveEnd )
	{
		if ( m_idxAnchor <= m_idxActiveEnd )	// expanding selection in pos direction
		{
			for ( i = m_idxActiveEnd + 1; i <= idx; i++ )
				SetAt(i);
		}
		else // either shrinking selection or flipping selection to pos side of m_idxAnchor
		{
			idxUpper = min(idx, m_idxAnchor);
			for ( i = m_idxActiveEnd; i < idxUpper; i++ )
				RemoveAt(i);

			if ( idx > m_idxAnchor )
				for ( i = m_idxAnchor + 1; i <= idx; i++ )
				SetAt(i);
		}
	}
	else // idx < m_idxActiveEnd
	{
		if ( m_idxAnchor >= m_idxActiveEnd )	// expanding selection in neg direction
		{
			for ( i = m_idxActiveEnd - 1; i >= idx; i-- )
				SetAt(i);
		}
		else // either shrinking selection or flipping selection to neg side of m_idxAnchor
		{
			idxLower = max(idx, m_idxAnchor);
			for ( i = m_idxActiveEnd; i > idxLower; i-- )
				RemoveAt(i);

			if ( idx < m_idxAnchor )
				for ( i = m_idxAnchor - 1; i >= idx; i-- )
				SetAt(i);
		}
	}
	
	m_idxActiveEnd = idx;
}

void CColumnSelection::SetAt(long idx)
{
	long i;

	if ( IsSelected(idx) )
		return;
	
	if ( (i = m_pView->GetColumn(idx)) != -1 )
		m_pView->HighlightColumn(i);

	CSelection::SetAt(idx);
}

void CColumnSelection::RemoveAt(long idx)
{
	long i;

	if ( !IsSelected(idx) )
		return;

	if ( (i = m_pView->GetColumn(idx)) != -1 )
		m_pView->UnHighlightColumn(i);

	CSelection::RemoveAt(idx);
}

void CColumnSelection::RemoveAll()
{
	long i;
	long nCount;

	if ( IsEmpty() )
		return;

	nCount = m_pView->m_aColumns.GetSize();
	for ( i = 0; i < nCount; i++ )
	{
		if ( IsSelected(m_pView->m_aColumns[i]->m_idx) )
			m_pView->UnHighlightColumn(i);
	}

	CSelection::RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CRowSelection
IMPLEMENT_DYNAMIC( CRowSelection, CSelection )

CRowSelection::CRowSelection(CTableView* pTableView)
: CSelection(pTableView)
{
}

void CRowSelection::StartActiveSelection(long idx, BOOL bRemove /*= TRUE*/)
{
	if ( bRemove )
		RemoveAll();

	SetAt(idx);

	m_idxActiveEnd = m_idxAnchor = idx;
}

void CRowSelection::UpdateActiveSelection(long idx, BOOL bRemove /*= TRUE*/)
{
	long i;
	long idxLower, idxUpper;
	POSITION pos;
	long nValue;

	if ( m_map.IsEmpty() )
	{
		StartActiveSelection(idx,bRemove);
		return;
	}

	if ( !IsSelected(m_idxAnchor) || !IsSelected(m_idxActiveEnd) )
	{
		StartActiveSelection(idx,bRemove);
		return;
	}

	// remove all idxs outside of active selection?
	if ( bRemove )
	{
		idxLower = min(m_idxAnchor, m_idxActiveEnd);
		idxUpper = max(m_idxAnchor, m_idxActiveEnd);
		pos = m_map.GetStartPosition();
		while ( pos != NULL )
		{
			m_map.GetNextAssoc(pos,i,nValue);
			if ( i < idxLower || i > idxUpper )
				RemoveAt(i);
		}
	}

	if ( idx == m_idxActiveEnd ) // no change to active selection
		return;

	if ( idx > m_idxActiveEnd )
	{
		if ( m_idxAnchor <= m_idxActiveEnd )	// expanding selection in pos direction
		{
			for ( i = m_idxActiveEnd + 1; i <= idx; i++ )
				SetAt(i);
		}
		else // either shrinking selection or flipping selection to pos side of m_idxAnchor
		{
			idxUpper = min(idx, m_idxAnchor);
			for ( i = m_idxActiveEnd; i < idxUpper; i++ )
				RemoveAt(i);

			if ( idx > m_idxAnchor )
				for ( i = m_idxAnchor + 1; i <= idx; i++ )
				SetAt(i);
		}
	}
	else // idx < m_idxActiveEnd
	{
		if ( m_idxAnchor >= m_idxActiveEnd )	// expanding selection in neg direction
		{
			for ( i = m_idxActiveEnd - 1; i >= idx; i-- )
				SetAt(i);
		}
		else // either shrinking selection or flipping selection to neg side of m_idxAnchor
		{
			idxLower = max(idx, m_idxAnchor);
			for ( i = m_idxActiveEnd; i > idxLower; i-- )
				RemoveAt(i);

			if ( idx < m_idxAnchor )
				for ( i = m_idxAnchor - 1; i >= idx; i-- )
				SetAt(i);
		}
	}
	
	m_idxActiveEnd = idx;
}

void CRowSelection::SetAt(long idx)
{
	long i;

	if ( IsSelected(idx) )
		return;
	
	if ( (i = m_pView->GetRow(idx)) != -1 )
		m_pView->HighlightRow(i);

	CSelection::SetAt(idx);
}

void CRowSelection::RemoveAt(long idx)
{
	long i;

	if ( !IsSelected(idx) )
		return;

	if ( (i = m_pView->GetRow(idx)) != -1 )
		m_pView->UnHighlightRow(i);

	CSelection::RemoveAt(idx);
}

void CRowSelection::RemoveAll()
{
	long i;
	long nCount;

	if ( IsEmpty() )
		return;

	nCount = m_pView->m_aRows.GetSize();
	for ( i = 0; i < nCount; i++ )
	{
		if ( IsSelected(m_pView->m_aRows[i]->m_idx) )
			m_pView->UnHighlightRow(i);
	}

	CSelection::RemoveAll();
}

CRowSelection * CRowSelection::Copy() {
	CRowSelection * c = new CRowSelection(this->m_pView);
	c->m_idxActiveEnd = m_idxActiveEnd;
	c->m_idxAnchor = m_idxAnchor;
	c->m_idxMax = m_idxMax;
	c->m_idxMin = m_idxMin;
	c->m_pos = m_pos;
	c->m_pView = m_pView;
	
	long idx;
	long nValue;
	POSITION pos;

	pos = m_map.GetStartPosition();
	while ( pos != NULL )
	{
		m_map.GetNextAssoc(pos,idx,nValue);
		c->m_map.SetAt(idx, nValue);
	}

	return c;
}
/////////////////////////////////////////////////////////////////////////////
