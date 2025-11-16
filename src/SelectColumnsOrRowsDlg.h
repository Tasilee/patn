// SelectColumnsOrRowsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectColumnsOrRowsDlg dialog

class CSelectColumnsOrRowsDlg : public CDialog
{
// Construction
public:
	CSelectColumnsOrRowsDlg(CWnd* pParent, CPatnDoc* pDoc);

protected:
	CTableView* m_pView;
	CPatnDoc* m_pDoc;
	CArray<long> m_aSelectedItems;

public:
// Dialog Data
	//{{AFX_DATA(CSelectColumnsOrRowsDlg)
	enum { IDD = IDD_SELECT_COLS_OR_ROWS };
	CListBox	m_lbSelected;
	CListBox	m_lbAll;
	CButton	m_btnRemoveAll;
	CButton	m_btnRemove;
	CButton	m_btnAddAll;
	CButton	m_btnAdd;
	int		m_nDataType;
	//}}AFX_DATA

	enum { columns, rows };


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectColumnsOrRowsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillList();
	void FillWithColumnLabels();
	void FillWithRowLabels();
	void ResetList();

	// Generated message map functions
	//{{AFX_MSG(CSelectColumnsOrRowsDlg)
	afx_msg void OnAdd();
	afx_msg void OnAddAll();
	afx_msg void OnRemove();
	afx_msg void OnRemoveAll();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkAll();
	afx_msg void OnDblclkSelected();
	afx_msg void OnBtnClose();
	afx_msg void OnRadioRows();
	afx_msg void OnRadioColumns();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
