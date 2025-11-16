// PageNewTableOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageNewTableOptions dialog

class CPageNewTableOptions : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageNewTableOptions)

// Construction
public:
	CPageNewTableOptions();
	~CPageNewTableOptions();

public:
// Dialog Data
	//{{AFX_DATA(CPageNewTableOptions)
	enum { IDD = IDD_PAGE_NEW_TABLE_OPTIONS };
	CEdit	m_editColumnWidth;
	CSpinButtonCtrl	m_spinDecimalPrecision;
	CEdit	m_MissingValue;
	int		m_nNoOfColumns;
	int		m_nNoOfRows;
	int		m_nWhichInitialValues;
	int		m_nWhichColumnWidth;
	int		m_nDecimalPrecision;
	double	m_nColumnWidth;
	double	m_nMissingValue;
	int		m_random_seed;
	//}}AFX_DATA
	enum { zero,missingValue,random }; // initial values for cells
	enum { standard,custom }; // initial column width

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageNewTableOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageNewTableOptions)
	afx_msg void OnRadioStandard();
	afx_msg void OnRadioCustom();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditMissingValue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
