// pickam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPickAssocMatrixDlg dialog

class CPickAssocMatrixDlg : public CDialog
{
// Construction
public:
	CPickAssocMatrixDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPickAssocMatrixDlg)
	enum { IDD = IDD_PICK_ASSOC_MATRIX };
	int		m_nDataType;
	//}}AFX_DATA
	enum { COLUMNS, ROWS };


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickAssocMatrixDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPickAssocMatrixDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
