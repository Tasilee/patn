// PageRowOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageRowOptions dialog

class CPageRowOptions : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageRowOptions)

// Construction
public:
	CPageRowOptions();
	~CPageRowOptions();
	CPatnDoc* m_pDoc;
	long m_pMaxGroupNumber;
	long m_strGroup;
// Dialog Data
	//{{AFX_DATA(CPageRowOptions)
	enum { IDD = IDD_PAGE_ROW_OPTIONS };
	CEdit	m_editGroup;
	CButton	m_checkHide;
	CEdit	m_editLabel;
	CString	m_strLabel;
	int		m_nHide;
	//}}AFX_DATA
 	BOOL	m_bMultiple;
	BOOL	m_bChangedLabel;
	BOOL	m_bChangedHidden;
	BOOL	m_bChangedGroup;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageRowOptions)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageRowOptions)
	afx_msg void OnCheckHide();
	afx_msg void OnChangeEditLabel();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditGroup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
