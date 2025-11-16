// CFont.cpp: implementation of the CCFont class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "stdafx.h"
#include "patn.h"
#include <gl\gl.h>
#include <gl\glu.h>
#include "CFont.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LOGFONT lfDlg = {0,0,700,0,0,0,0,0,0,0,0,0,0,"times"};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCFont::CCFont()
{
	m_OldFont = NULL;

}

CCFont::~CCFont()
{

}

//TRY TO DESTROY A FONT BEFORE RUNNING THIS FUNCTION
//GLFONT *CCFont::FontCreate (HDC        hdc,        /* Device context */
  //                  const char *typeface,  /* Font Specification */
    //                int        height,     /* Font height (size in pixels) */
      //              int        weight,     /* Weight of font (bold, etc) */
        //            DWORD      italic,     /* Text is italic */
		//			int		   angle)	   /* Angle in 0.1-degrees*/
GLFONT *CCFont::FontCreate(HDC hdc,LOGFONT f)
{
   GLFONT *font;  /* Font data pointer */
   HFONT fontid;  /*Windows font ID */
//   int charset;   /* Character set code */

   /* Allocate memory */
   if ((font = (GLFONT *) calloc(1, sizeof(GLFONT))) == (GLFONT *)0) {
      return (GLFONT *) 0;
   }

   /* Allocate display lists */
   if ((font->base = glGenLists(256)) == 0) {
      free(font);
      return 0;
   }

   /* Select a character set 
   if (stricmp(typeface, "symbol") == 0) {
      charset = SYMBOL_CHARSET;
   } else {
      charset = ANSI_CHARSET;
   }
   stricmp(typeface,"times");*/

   /* Load the font */

   //fontid = CreateFont(height, 0,angle, angle, FW_NORMAL, FALSE, FALSE, 0,
     //                  charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
       //               PROOF_QUALITY, FIXED_PITCH | FF_SWISS, typeface);
   fontid = CreateFont(f.lfHeight,f.lfWidth,f.lfEscapement,f.lfOrientation,
	   f.lfWeight,f.lfItalic,f.lfUnderline,f.lfStrikeOut,f.lfCharSet,
	   f.lfOutPrecision,f.lfClipPrecision,f.lfQuality,f.lfPitchAndFamily,
	   f.lfFaceName);
   	   
   //fontid = CreateFont(0, 0, 700, 0, 0, 0, 0, 0,
     //                  0, 0, CLIP_LH_ANGLES ,
       //               0, 0, "aerial");
   
   //fontid = CreateFont(0, 0, 900, 0, 0, 0, 0, 0,
     //                  0, 0, 0,
       //               0, 0, typeface);
  // fontid = CreateFontIndirect(&lfDlg);
   
   m_OldFont = (CFont*)SelectObject(hdc, fontid);
   m_Hdc = hdc;

   /* Create bitmaps for each characcter */
   wglUseFontBitmaps(hdc, 0, 256, font->base);

   /* Get the width and height information for each character */
   GetCharWidth(hdc, 0, 255, font->widths);
//   font->height = height;

   return font;
}

void CCFont::FontDestroy(GLFONT *font)
{
   if (font == (GLFONT *) 0) { return; }
   glDeleteLists(font->base, 256);
   free(font);
   SelectObject(m_Hdc,m_OldFont);
   m_OldFont = NULL;
}


void CCFont::FontPuts(GLFONT *font, const char *s)
{
   if (font == (GLFONT *) 0 || s == NULL) { return; }
   glPushAttrib(GL_LIST_BIT);
   glListBase(font->base);
   glCallLists(strlen(s), GL_UNSIGNED_BYTE, s);
//glBitmap(50,50,0,0,0,0,(unsigned char*)((font->base[61])));
   glPopAttrib();
}

void CCFont::FontPuts2(GLFONT *font, const char *s, double x, double y)
{
   GLdouble output[16];

   if (font == (GLFONT *) 0 || s == NULL) { return; }
   glPushAttrib(GL_LIST_BIT);
   glListBase(font->base);
   glGetDoublev(GL_TEXTURE_MATRIX, output);
   glLoadMatrixd(output);
   glPushMatrix();
   glTranslated(x,y,0.0);
   glRotated(90,0.0,0.0,1.0);
   glCallLists(strlen(s), GL_UNSIGNED_BYTE, s);
   glPopMatrix();
   glPopAttrib();
}

void CCFont::FontPrintf(GLFONT *font, int align, const char *format)
{
   va_list ap;  /* Arguement Pointer */
   unsigned char s[1024]; /* Output String */
   unsigned char *ptr;  /* Pointer into string */
   int width;   /* Width of string in pixels */

   if (font == (GLFONT *) 0 || format == (char *) 0) { return; }

   /* Format the string */
   //va_start(ap, format);
   vsprintf((char *)s, format, ap);
   //va_end(ap);

   /* Figure out the width of the string in pixels */
   for (ptr = s,width = 0; *ptr; ptr++) {
      width += font->widths[*ptr];
   }

   /* Adjust the bitmap position as need to justify the text */
   if (align < 0) {
      glBitmap(0, 0, 0, 0, -width*1.0f, 0, NULL);
   } else if (align == 0) {
      glBitmap(0, 0, 0, 0, -width / 2.0f, 0, NULL);
   }

   /* Draw the string */
   FontPuts(font, (const char *) s);
}
