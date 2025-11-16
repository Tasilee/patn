// exponent.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExponentDlg dialog

class CExponentDlg : public CDialog
{
// Construction
public:
	CExponentDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CExponentDlg)
	enum { IDD = IDD_EXPONENT };
	double	m_nExponent;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CExponentDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
