// WriteAVI.h: interface for the CWriteAVI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WRITEAVI_H__1738654B_7A9A_465A_A9F6_556E8FC49537__INCLUDED_)
#define AFX_WRITEAVI_H__1738654B_7A9A_465A_A9F6_556E8FC49537__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define  INC_OLE2
#include <windows.h>
#include <windowsx.h>
#include <memory.h>
#include <mmsystem.h>
#include <vfw.h>

class CWriteAVI  
{
public:
	CWriteAVI();
	virtual ~CWriteAVI();

	int m_frame_count;
	PAVISTREAM ps, psCompressed;
	PAVIFILE pfile;


	bool BeginNewAVI(CString file_name,int width,int height,int bits,LPBITMAPINFOHEADER first_frame);
	bool AddFrame(LPBITMAPINFOHEADER frame);
	bool EndAVI();
	HANDLE MakeDib( HBITMAP hbitmap, UINT bits );

};

#endif // !defined(AFX_WRITEAVI_H__1738654B_7A9A_465A_A9F6_556E8FC49537__INCLUDED_)
