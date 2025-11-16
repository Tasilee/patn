// logconst.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogConstDlg dialog

class CLogConstDlg : public CDialog
{
// Construction
public:
	CLogConstDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLogConstDlg)
	enum { IDD = IDD_LOG_CONSTANT };
	double	m_nLogConstant;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CLogConstDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
