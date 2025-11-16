// Glf.h: interface for the CGlf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLF_H__B6EF4321_8199_11D4_A580_94D3D7107536__INCLUDED_)
#define AFX_GLF_H__B6EF4321_8199_11D4_A580_94D3D7107536__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* CONSTANTS */

typedef void (*pfn)(char);

void glfDrawWiredSymbol(char s);    /* Draw wired symbol    */
void glfDrawSolidSymbol(char s);    /* Draw solid symbol    */
void glfDraw3DSolidSymbol(char s);  /* Draw 3D solid symbol */
void glfDraw3DWiredSymbol(char s);  /* Draw 3D wired symbol */

#define GLF_ERROR           -1
#define GLF_OK               0
#define GLF_YES              1
#define GLF_NO               2

#define GLF_CONSOLE_MESSAGES 10
#define GLF_TEXTURING        11
#define GLF_CONTOURING       12

#define GLF_LEFT_UP          20
#define GLF_LEFT_CENTER      21
#define GLF_LEFT_DOWN        22
#define GLF_CENTER_UP        23
#define GLF_CENTER_CENTER    24
#define GLF_CENTER_DOWN      25
#define GLF_RIGHT_UP         26
#define GLF_RIGHT_CENTER     27
#define GLF_RIGHT_DOWN       28
#define GLF_CENTER           24

	/* Defines */

#define anchor_proc(x, y, ap) { \
    float *xx, *yy; \
    xx = (float *)&x; \
    yy = (float *)&y; \
    switch (ap) \
    { \
      case GLF_CENTER: break; \
      case GLF_LEFT_UP: *xx += 1; *yy -= 1; break; \
      case GLF_LEFT_CENTER: *xx += 1; break; \
      case GLF_LEFT_DOWN: *xx += 1; *yy += 1; break; \
      case GLF_CENTER_UP: *yy -= 1; break; \
      case GLF_CENTER_DOWN: *yy += 1; break; \
    } \
}

#define MAX_FONTS 256

/* Color structure */
struct color
{
  float r, g, b, a;
};

/* One symbol of font */
struct one_symbol
{
  unsigned char vertexs; /* Number of vertexs         */
  unsigned char facets;  /* Number of facets          */
  unsigned char lines;   /* Number of lines in symbol */

  float *vdata;          /* Pointer to Vertex data    */
  unsigned char *fdata;  /* Pointer to Facets data    */
  unsigned char *ldata;  /* Pointer to Line data      */

  float leftx;           /* Smaller x coordinate      */
  float rightx;          /* Right x coordinate        */
  float topy;            /* Top y coordinate          */
  float bottomy;         /* Bottom y coordinate       */
};

/* Font structure */
struct glf_font
{
  char font_name [97];
  unsigned char sym_total;          /* Total symbols in font */
  struct one_symbol *symbols[256];  /* Pointers to symbols   */
};


/* Console constants */
#define GLF_CONSOLE_CURSOR   30

class CGlf  
{
public:
	CGlf();
	virtual ~CGlf();

	bool m_blending;

	struct glf_font *m_fonts[MAX_FONTS];
	char *conData;           /* Console data */

	/* Library initialization (must be called before any usage of library) */
	void glfInit(); 

	/* Font loading/unloading functions */
	int  glfLoadFont(char *font_name);        /* Load font to memory*/
	int  glfUnloadFont();                     /* Unload font from memory */
	int  glfUnloadFontD(int font_descriptor); /* Unload font by font_descriptor */

	/* Text drawing functions */
	void glfDrawWiredString(CString s);   /* Draw wired string    */
	void glfDrawSolidString(CString s);   /* Draw wired string    */
	void glfDraw3DWiredString(CString s); /* Draw 3D wired string */
	void glfDraw3DSolidString(CString s); /* Draw 3D solid string */

	/* Text drawing functions by specified font */
	void glfDrawWiredSymbolF(int font_descriptor, char s);
	void glfDrawWiredStringF(int font_descriptor, CString s);
	void glfDrawSolidSymbolF(int font_descriptor, char s);
	void glfDrawSolidStringF(int font_descriptor, CString s);
	void glfDraw3DWiredSymbolF(int font_descriptor, char s);
	void glfDraw3DWiredStringF(int font_descriptor, CString s);
	void glfDraw3DSolidSymbolF(int font_descriptor, char s);
	void glfDraw3DSolidStringF(int font_descriptor, CString s);

	/* Text control functions */
	void glfGetStringBoundsF(int fd, CString s, float *minx, float *miny, float *maxx, float *maxy);
	void glfGetStringBounds(CString s, float *minx, float *miny, float *maxx, float *maxy);

	void  glfSetSymbolSpace(float sp);   /* Set space between symbols */
	float glfGetSymbolSpace();           /* Get space between symbols */
	void  glfSetSpaceSize(float sp);     /* Set space size            */
	float glfGetSpaceSize();             /* Get current space size    */

	void  glfSetSymbolDepth(float dpth); /* Set depth of 3D symbol */
	float glfGetSymbolDepth();           /* Get depth of 3D symbol */
	int   glfSetCurrentFont(int Font_Descriptor); /* Set current font */
	int   glfGetCurrentFont();                    /* Get current font descriptor */

	void  glfSetAnchorPoint(int anchp); /* Set symbol anchor point */
	void glfSetContourColor(float r, float g, float b, float a); /* Contour color */

	/* Enable/Disable GLF features */
	void glfEnable(int what); /* Enable GLF feature 'what' */
	void glfDisable(int what); /* Disable GLF feature 'what' */

	/* Console functions */
	void glfSetConsoleParam(int width, int height);
	int  glfSetConsoleFont(int Font_Descriptor);
	void glfConsoleClear();
	void glfPrint(CString s, int lenght);
	void glfPrintString(CString s);
	void glfPrintChar(char s);

	void glfConsoleDraw();
	void glfSetCursorBlinkRate(int Rate);

};

#endif // !defined(AFX_GLF_H__B6EF4321_8199_11D4_A580_94D3D7107536__INCLUDED_)
