#if !defined(AFX_ODBCCONNECTION_H__7AD33663_7877_11D4_82C4_00105A67A94F__INCLUDED_)
#define AFX_ODBCCONNECTION_H__7AD33663_7877_11D4_82C4_00105A67A94F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ODBCConnection.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CODBCConnection dialog

class CODBCConnection : public CDialog
{
// Construction
public:
	CODBCConnection(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CODBCConnection)
	enum { IDD = IDD_ODBC_CON_VALUES };
	CEdit	m_disable;
	CString	m_col_label_field;
	CString	m_data_field;
	CString	m_dsn;
	CString	m_row_label_field;
	CString	m_table_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CODBCConnection)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CODBCConnection)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ODBCCONNECTION_H__7AD33663_7877_11D4_82C4_00105A67A94F__INCLUDED_)
