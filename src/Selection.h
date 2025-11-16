// Selection.h : header file
//

#ifndef SELECTION_H
#define SELECTION_H

/////////////////////////////////////////////////////////////////////////////
// CSelection
class CTableView;

class CSelection 
// abstract class - serves as base for derived classes which store idxs of
// selected rows/columns in CTableView
{
public:
	void Shift(long nCount);
	CSelection(CTableView* pTableView);
	~CSelection();
	DECLARE_DYNAMIC( CSelection )

public:
	CTableView* m_pView;

protected:
	void	Initialize();	// sets all values to empty map state

public:
	CMap<long,long,long,long> m_map;
	long		m_idxMin;		// Lowest idx value stored in map
	long 	m_idxMax;		// Highest idx value stored in map
	long		m_idxAnchor;	// Starting idx of current selection operation
							// Note: this is not necessarily the same as the lowest idx,
							// eg, if selection operation goes in descending idx direction
							// or if current selection operation is part of multi-selection.
	long		m_idxActiveEnd;	// Termination idx of current active selection
	POSITION m_pos;

public:
	// NOTE: Avoid using SetAt() & RemoveAt() directly, as they don't update
	// m_idxAnchor & m_idxActiveEnd
	virtual void	StartActiveSelection(long idx, BOOL bRemove = TRUE) = 0;
	virtual void	UpdateActiveSelection(long idx, BOOL bRemove = TRUE) = 0;
	void	SetAt(long idx);
	void	RemoveAt(long idx);
	void	RemoveAll();

public:
	long		GetIdxAnchor()		{ return m_idxAnchor; }
	long		GetIdxActiveEnd()	{ return m_idxActiveEnd; }
	long 	GetMinValue()		{ return m_idxMin; }
	long 	GetMaxValue()		{ return m_idxMax; }
	BOOL	IsEmpty()			{ return m_map.IsEmpty(); }
	long		GetCount()			{ return (long)m_map.GetCount(); }
	BOOL	IsSelected(long idx);
	BOOL	IsContiguous();
	void	GetItems(CArray<int>* pArray, BOOL bSort = TRUE);
	long		GetFirst();
	long		GetNext();
};
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CColumnSelection

class CColumnSelection : public CSelection
// stores idx of selected columns in CTableView
{
public:
	CColumnSelection(CTableView* pTableView);
	DECLARE_DYNAMIC( CColumnSelection )

// overides of pure virtual functions in base class
public:
	// NOTE: Avoid using SetAt() & RemoveAt() directly, as they don't update
	// m_idxAnchor & m_idxActiveEnd
	void	StartActiveSelection(long idx, BOOL bRemove = TRUE);
	void	UpdateActiveSelection(long idx, BOOL bRemove = TRUE);
	void	SetAt(long idx);
	void	RemoveAt(long idx);
	void	RemoveAll();
	CColumnSelection * Copy();
};
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CRowSelection

class CRowSelection : public CSelection
// stores idx of selected columns in CTableView
{
public:
	CRowSelection(CTableView* pTableView);
	DECLARE_DYNAMIC( CRowSelection )

// overides of pure virtual functions in base class
public:
	// NOTE: Avoid using SetAt() & RemoveAt() directly, as they don't update
	// m_idxAnchor & m_idxActiveEnd
	void	StartActiveSelection(long idx, BOOL bRemove = TRUE);
	void	UpdateActiveSelection(long idx, BOOL bRemove = TRUE);
	void	SetAt(long idx);
	void	RemoveAt(long idx);
	void	RemoveAll();
	CRowSelection * Copy();
};


/////////////////////////////////////////////////////////////////////////////
// inline functions

inline BOOL CSelection::IsSelected(long idx)
{
	long nValue;

	return ( m_map.Lookup(idx,nValue) ? TRUE : FALSE );
}
/////////////////////////////////////////////////////////////////////////////


typedef struct {
	CColumnSelection * c;
	CRowSelection * r;
	CString name;
} Sel;

#endif // SELECTION_H
