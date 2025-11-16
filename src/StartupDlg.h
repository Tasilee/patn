// StartupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStartupDlg dialog

class CStartupDlg : public CDialog
{
// Construction
public:
	CStartupDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStartupDlg)
	enum { IDD = IDD_STARTUP };
	CButton	m_grp;
	CButton	m_btnOpen;
	CButton	m_btnNew;
	CButton	m_btnImport;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStartupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStartupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnOpen();
	afx_msg void OnBtnNew();
	afx_msg void OnBtnImport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
