// PageColumnOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageColumnOptions dialog

class CPageColumnOptions : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageColumnOptions)

// Construction
public:
	CPageColumnOptions();
	~CPageColumnOptions();

	enum { standard,custom,sizetofit }; // column width
public:
	CPatnDoc* m_pDoc;
	long m_pMaxGroupNumber;
	long m_strGroup;

// Dialog Data
	//{{AFX_DATA(CPageColumnOptions)
	enum { IDD = IDD_PAGE_COLUMN_OPTIONS };
	CEdit	m_editGroup;
	CSpinButtonCtrl	m_spinDecimalPrecision;
	CEdit	m_editLabel;
	CEdit	m_editDecimalPrecision;
	CButton	m_checkHide;
	CEdit	m_editColumnWidth;
	int		m_nWhichColumnWidth;
	double	m_nColumnWidth;
	int		m_nHide;
	CString	m_strLabel;
	int		m_nDecimalPrecision;
	//}}AFX_DATA
	BOOL	m_bMultiple;
	BOOL	m_bChangedLabel;
	BOOL	m_bChangedHidden;
	BOOL	m_bChangedColumnWidth;
	BOOL	m_bChangedDecimalPrecision;
	BOOL	m_bChangedGroup;
//troy's global

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageColumnOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageColumnOptions)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioCustom();
	afx_msg void OnRadioStandard();
	afx_msg void OnRadioSizetofit();
	afx_msg void OnCheckHide();
	afx_msg void OnChangeEditDecimalPrecision();
	afx_msg void OnChangeEditLabel();
	afx_msg void OnChangeEditGroup();
	afx_msg void OnUpdateEditGroup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
