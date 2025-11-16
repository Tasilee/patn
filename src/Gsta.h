// Gsta.h: interface for the CGsta class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GSTA_H__217005D3_5C56_11D4_82AD_00105A6871C5__INCLUDED_)
#define AFX_GSTA_H__217005D3_5C56_11D4_82AD_00105A6871C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGsta  
{
public:
	CGsta(CPatnDoc *pDoc);
	virtual ~CGsta();

	CPatnDoc *m_pDoc;
    double Get_First_Quartile(CArray<double> *values);
	double Get_Maximum(CArray<double> *values);
	double Get_Mimimum(CArray<double> *values);
	double Get_Median(CArray<double> *values);
	double Get_Mean(CArray<double> *values);
	double Get_Third_Quartile(CArray<double> *values);

	bool SaveGsta(CArray<dblStat*> * in = NULL);
	void SaveColumnGsta(CPatnDoc *pDoc);
};

#endif // !defined(AFX_GSTA_H__217005D3_5C56_11D4_82AD_00105A6871C5__INCLUDED_)
