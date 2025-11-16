// CFont.h: interface for the CCFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CFONT_H__2A4B4F42_6637_11D4_A3D4_525405F5DADA__INCLUDED_)
#define AFX_CFONT_H__2A4B4F42_6637_11D4_A3D4_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <gl\glu.h>
 
typedef struct {
	GLuint base;         /* display list number of first character */
	int    widths[256];  /* Width of each character in pixels */
	int    height;       /* Height of character */
} GLFONT;

 
class CCFont  
{
public:
	CCFont();
	virtual ~CCFont();

	CFont * m_OldFont;
	HDC m_Hdc;

	//GLFONT *FontCreate(HDC hdc, const char *typeface, int height,
      //                       int weight, DWORD italic,int angle);
	GLFONT * FontCreate(HDC hdc,LOGFONT font);
	void FontDestroy(GLFONT *font);
	void FontPrintf(GLFONT *font, int align, const char *format);
	void FontPuts(GLFONT *font, const char *s);
	void FontPuts2(GLFONT *font, const char *s, double x, double y);

};

#endif // !defined(AFX_CFONT_H__2A4B4F42_6637_11D4_A3D4_525405F5DADA__INCLUDED_)
