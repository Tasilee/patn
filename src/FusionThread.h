#if !defined(AFX_FUSIONTHREAD_H__9ABA9422_71C2_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_FUSIONTHREAD_H__9ABA9422_71C2_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FusionThread.h : header file
//




class CAssocMatrix;

struct NN
{
	long 	key;
	long 	keyNN;
	double	nLevel;
	long 	nNbrInGroup;
};
typedef struct NN NN;

typedef CArray<double> CADouble;
typedef CArray<CADouble*> CMatrix;

/////////////////////////////////////////////////////////////////////////////
// CFusionThread thread

class CFusionThread : public CWinThread
{
public:
	DECLARE_DYNAMIC(CFusionThread)
	CFusionThread();           // protected constructor used by dynamic creation

// Attributes
public:
	CPatnDoc * m_pDoc;
	HANDLE m_hEventKillFusionThread;
	HWND m_hWnd;

		enum
	{
		NearestNeighbour,
		FurthestNeighbour,
		FlexibleWPGMA,
		FlexibleUPGMA,
	};


	long m_type;
	bool m_rows;
	bool * m_value;
	long m_Step;

	CDialog * m_dlg;

public:
	double	m_nAlpha1;
	double	m_nAlpha2;
	double	m_nBeta;
	double	m_nGamma;
	double* m_pCutOff;	// TODO: temp code to facilitate groups line in dendrogram

protected:
	long		m_nFuseTechnique;
	long		m_nNoOfGroups;
	CMatrix* m_pMatrix, m_Matrix;
	CFuseTable* m_pFuseTable;
	double	m_nOMB;
	CMap<long,long,NN*,NN*> m_mapNN;
	CArray<long> *m_pGroups;

public:
	BOOL Attach(CMatrix* pMatrix,
	            CFuseTable* pFuseTable,
	            long nFuseTechnique,
				double* pCutOff,
	            long nNoOfGroups,
	            double nBeta,
				CArray<long>* pGroups);
	BOOL	Fuse();
	BOOL	SetFuseTechnique(long nFuseTechnique);
	BOOL	SetNoOfGroups(long nNoOfGroups);
	void AssignGroups(void);

	void WriteFuseTableToASCII(CPatnDoc * m_pDoc,long nType);

protected:
	void	Initialize();
	bool	CreateNNMap();
	NN*		GetMinNN();
	long		FindNNFor(long key);
	double	GetAssocAt(long i,long j);
	void	SetAssocAt(long i, long j, double nLevel);
	long		Full(void);
	void	AllocateGroups(long idx,long group);

public:
	bool Go();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFusionThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFusionThread();

	// Generated message map functions
	//{{AFX_MSG(CFusionThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FUSIONTHREAD_H__9ABA9422_71C2_11D4_A3D4_525405F5DADA__INCLUDED_)

