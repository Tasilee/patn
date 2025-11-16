// nonzeroc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNonZeroConstantDlg dialog

class CNonZeroConstantDlg : public CDialog
{
// Construction
public:
	CNonZeroConstantDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNonZeroConstantDlg)
	enum { IDD = IDD_NONZEROS_CONSTANT };
	double	m_nNonZeroConstant;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CNonZeroConstantDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
