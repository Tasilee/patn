// DataTransformDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDataTransformDlg dialog

class CDataTransformDlg : public CDialog
{
// Construction
public:
	CDataTransformDlg(CPatnDoc* pDoc, CWnd* pParent = NULL);

	enum { columns, rows };
	
protected:
	CTableView* m_pView;
	CPatnDoc* m_pDoc;
	CArray<long> m_aSelectedItems;

// Dialog Data
	//{{AFX_DATA(CDataTransformDlg)
	enum { IDD = IDD_DATA_TRANSFORM };
	CListBox	m_lbSelected;
	CListBox	m_lbAll;
	CListBox	m_lbOptions;
	int	m_nDataType;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void FillList();
	void FillWithColumnLabels();
	void FillWithRowLabels();
	void ResetList();
	
	// Generated message map functions
	//{{AFX_MSG(CDataTransformDlg)
	afx_msg void OnColumns();
	afx_msg void OnRows();
	afx_msg void OnAdd();
	afx_msg void OnAddAll();
	afx_msg void OnRemove();
	afx_msg void OnRemoveAll();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkAll();
	afx_msg void OnDblclkSelected();
	afx_msg void OnClose();
	afx_msg void OnRun();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
