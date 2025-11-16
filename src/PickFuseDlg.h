// PickFuseDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPickFuseTableDlg dialog

class CPickFuseTableDlg : public CDialog
{
// Construction
public:
	CPickFuseTableDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPickFuseTableDlg)
	enum { IDD = IDD_PICK_FUSE_TABLE };
	int		m_nDataType;
	//}}AFX_DATA
	enum { COLUMNS, ROWS };


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickFuseTableDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPickFuseTableDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
