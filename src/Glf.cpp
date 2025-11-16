// Glf.cpp: implementation of the CGlf class.
//
//////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdafx.h"
#include <gl\glu.h>
#include <gl\gl.h>
#include "patn.h"
#include "Glf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/* Main variables */

float SymbolDist = 0.2f;  /* Distance between symbols (Variable constant) */
float SymbolDepth = 0.2f; /* Symbol Depth in 3D space (Variable constant) */
float SpaceSize = 2.0f;   /* Space size (Variable constant)               */

/* Array of font pointers, up to MAX_FONTS fonts can be loaded at once */
/* if (fonts[i] == NULL) then this font is not present in memory */
struct glf_font *fonts[MAX_FONTS];
int curfont; /* Index of current font pointer */
char ap = GLF_CENTER; /* Anchor point */
char console_msg = GLF_NO;
char texturing = GLF_NO;
char contouring = GLF_NO;
struct color contouring_color;

/* Console mode variables */
int conWidth, conHeight; /* Console width and height */
int conx = 0, cony = 0;  /* Console current X and Y */
//char *conData;           /* Console data */
int conFont;             /* Console font */
char conCursor = GLF_NO; /* Console cursor Enabled|Disabled */
int conCursorBlink;          /* Console cursor blink rate */
int conCursorCount;          /* Console cursor blink counter */
char conCursorMode = GLF_NO; /* Console Cursor mode (on/off screen) */

bool blending;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlf::CGlf()
{
  int i;

  for (i=0; i<MAX_FONTS; i++) m_fonts[i] = NULL;
  curfont = -1;
  console_msg = GLF_NO;
  ap = GLF_CENTER; /* Set anchor point to center of each symbol */
  texturing = GLF_NO;  /* By default texturing is NOT Enabled */
  contouring = GLF_NO; /* By default contouring is NOT Enabled */
  memset(&contouring_color, 0, sizeof(struct color));
  conData = NULL;
  glfSetConsoleParam(40, 20);
  glfConsoleClear();
  glfEnable(GLF_CONSOLE_CURSOR);
  glfSetCursorBlinkRate(10);

}

CGlf::~CGlf()
{

}


void CGlf::glfInit()
{
}

int LittleEndian()
{
/* This function check that this machine is little endian
   Return value: 1 - little endian
                 0 - big endian
*/
  int endianTester = 1;
  char *endianChar = 0;

  endianChar = (char *)&endianTester;
  if (*endianChar) return 1;
  return 0;
}

int ReadFont(char *font_name, struct glf_font *glff)
{
/* 
   This function read font file and store information in memory
   Return: GLF_OK - if all OK
   Return: GLF_ERROR - if any error 
*/

  FILE *fontf;
  char buffer[64];
  int i, j;
  unsigned char temp, code, verts, fcets, lns;
  float tempfx, tempfy;
  unsigned char *tp;
  int LEndian; /* True if little endian machine */

  fontf = fopen(font_name, "rb");
  if (fontf == NULL) return GLF_ERROR;

  fread(buffer, 3, 1, fontf);
  buffer[3] = 0;
  if (strcmp(buffer, "GLF"))
  {
    if (console_msg) printf("Error reading font file: incorrect file format\n");
    return GLF_ERROR;
  }

  /* Check for machine */
  LEndian = LittleEndian();

  fread(glff->font_name, 96, 1, fontf);
  glff->font_name[96] = 0;

  fread(&glff->sym_total, 1, 1, fontf); /* Read total symbols in font */

  for (i=0; i<MAX_FONTS; i++) glff->symbols[i] = NULL;

  for (i=0; i<28; i++) fread(&temp, 1, 1, fontf);  /* Read unused data */

  /* Now start to read font data */
  
  for (i=0; i<glff->sym_total; i++)
  {
    fread(&code, 1, 1, fontf);  /* Read symbol code   */
    fread(&verts, 1, 1, fontf); /* Read vertexs count */
    fread(&fcets, 1, 1, fontf); /* Read facets count  */
    fread(&lns, 1, 1, fontf);   /* Read lines count   */

    if (glff->symbols[code] != NULL) 
    {
      if (console_msg) printf("Error reading font file: encountered symbols in font\n");
      return GLF_ERROR;
    }

    glff->symbols[code] = (struct one_symbol *)malloc(sizeof(struct one_symbol));
    glff->symbols[code]->vdata = (float *)malloc(8*verts);
    glff->symbols[code]->fdata = (unsigned char *)malloc(3*fcets);
    glff->symbols[code]->ldata = (unsigned char *)malloc(lns);

    glff->symbols[code]->vertexs = verts;
    glff->symbols[code]->facets = fcets;
    glff->symbols[code]->lines = lns;

    /* Read vertexs data */
    glff->symbols[code]->leftx = 10;
    glff->symbols[code]->rightx = -10;
    glff->symbols[code]->topy = 10;
    glff->symbols[code]->bottomy = -10;

    for (j=0; j<verts; j++)
    {
      fread(&tempfx, 4, 1, fontf);
      fread(&tempfy, 4, 1, fontf);
          
      /* If machine is bigendian -> swap low and high words in
       tempfx and tempfy */
      if (!LEndian) 
      {
        tp = (unsigned char *)&tempfx;
        temp = tp[0]; tp[0] = tp[3]; tp[3] = temp;
        temp = tp[1]; tp[1] = tp[2]; tp[2] = temp;
        tp = (unsigned char *)&tempfy;
        temp = tp[0]; tp[0] = tp[3]; tp[3] = temp;
        temp = tp[1]; tp[1] = tp[2]; tp[2] = temp;
      }
      glff->symbols[code]->vdata[j*2] = tempfx;
      glff->symbols[code]->vdata[j*2+1] = tempfy;

      if (tempfx < glff->symbols[code]->leftx) glff->symbols[code]->leftx = tempfx;
      if (tempfx > glff->symbols[code]->rightx) glff->symbols[code]->rightx = tempfx;
      if (tempfy < glff->symbols[code]->topy) glff->symbols[code]->topy = tempfy;
      if (tempfy > glff->symbols[code]->bottomy) glff->symbols[code]->bottomy = tempfy;
    }
    for (j=0; j<fcets; j++)
      fread(&glff->symbols[code]->fdata[j*3], 3, 1, fontf);
    for (j=0; j<lns; j++)
       fread(&glff->symbols[code]->ldata[j], 1, 1, fontf);
  }
  fclose(fontf);
  return GLF_OK;
}

int CGlf::glfLoadFont(char *font_name)
{
/* 
  Function loads font to memory from file
  Return value: GLF_ERROR  - if error
                >=0 - returned font descriptor (load success)
*/

  int i;
  char flag; /* Temporary flag */

  /* First we find free font descriptor */
  flag = 0; /* Descriptor not found yet */
  for (i=0; i<MAX_FONTS; i++)
    if (m_fonts[i] == NULL)
    {
      /* Initialize this font */
      m_fonts[i] = (struct glf_font *)malloc(sizeof(struct glf_font));
      flag = 1;
      break;
    }

  if (!flag) return GLF_ERROR; /* Free font not found */
  if (ReadFont(font_name, m_fonts[i]) == GLF_OK) 
  {
    curfont = i; /* Set curfont to just loaded font */
    return i;
  }
  
  if (m_fonts[i] != NULL)
  {
    free(m_fonts[i]);
    m_fonts[i] = NULL;
  }
  return GLF_ERROR;
}

int CGlf::glfUnloadFont()
{
/* 
  Unload current font from memory
  Return value: GLF_OK  - if all OK
                GLF_ERROR -  if error
*/
  int i;
  
  if ((curfont<0) || (m_fonts[curfont] == NULL)) return GLF_ERROR;
  
  for (i=0; i<MAX_FONTS; i++)
  {
    if (m_fonts[curfont]->symbols[i] != NULL)
    {
      free(m_fonts[curfont]->symbols[i]->vdata);
      free(m_fonts[curfont]->symbols[i]->fdata);
      free(m_fonts[curfont]->symbols[i]->ldata);
      free(m_fonts[curfont]->symbols[i]);
    }
  }
  free(m_fonts[curfont]);
  m_fonts[curfont] = NULL;
  if (conData) free(conData);
  conData = NULL;

  curfont = -1;
  return GLF_OK;
}

/* Unload font by font descriptor */
int CGlf::glfUnloadFontD(int font_descriptor)
{
  int temp;

  if ((font_descriptor<0) || (m_fonts[font_descriptor] == NULL)) return GLF_ERROR;

  temp = curfont;
  curfont = font_descriptor;
  glfUnloadFont();
  if (temp != font_descriptor) curfont = temp;
  else curfont = -1;
  return GLF_OK;
}

//void CGlf::glfDrawWiredSymbol(char s)
void glfDrawWiredSymbol(char s)
{
  int i, cur_line;
  float *tvp; /* temporary vertex pointer */
  float x, y;
  
  if ((curfont < 0) || (fonts[curfont] == NULL)) return;
  if (fonts[curfont]->symbols[s] == NULL) return;

  glBegin(GL_LINE_LOOP);
  tvp = fonts[curfont]->symbols[s]->vdata;
  cur_line = 0;
  for (i=0; i<fonts[curfont]->symbols[s]->vertexs; i++)
  {
    x = *tvp;
    tvp++;
    y = *tvp;
    tvp++;
    anchor_proc(x, y, ap);
    glVertex2f(x, y);
    if (fonts[curfont]->symbols[s]->ldata[cur_line] == i)
    {
      glEnd();
      cur_line++;
      if (cur_line < fonts[curfont]->symbols[s]->lines) glBegin(GL_LINE_LOOP);
      else break; /* No more lines */
    }
  }
}

/* Draw wired symbol by font_descriptor */
void CGlf::glfDrawWiredSymbolF(int font_descriptor, char s)
{
//  int temp;

  //temp = curfont;
  //curfont = font_descriptor;
  curfont = 0;
  fonts[0] = m_fonts[font_descriptor];
  glfDrawWiredSymbol(s);
  //curfont = temp;
}

void DrawString(CString s, pfn funct)
{
  int i;
  float sda, sdb;

  if (!strcmp(s, "")) return;
  if (curfont == -1) return;

  glPushMatrix();
  for (i=0; i<(int)s.GetLength(); i++)
  {
	  if (s[i] != ' ') (*funct)(s[i]);
    if ((fonts[curfont]->symbols[s[i]] == NULL) || (s[i] == ' '))
      glTranslatef(SpaceSize, 0, 0);
    else
    {
      if (i < ((int)s.GetLength()-1))
      {
        if (s[i+1] == ' ') glTranslatef(SymbolDist, 0, 0);
        else
        {
          if (fonts[curfont]->symbols[s[i+1]] == NULL) continue;
          sda = fonts[curfont]->symbols[s[i]]->rightx;
          sdb = -fonts[curfont]->symbols[s[i+1]]->leftx;
          glTranslatef(sda+sdb+SymbolDist, 0, 0);
        }
      }
    }
  }
  glPopMatrix();
}


void CGlf::glfDrawWiredString(CString s)
{
	DrawString(s, glfDrawWiredSymbol);
}

/* Draw wired string by font_descriptor */
void CGlf::glfDrawWiredStringF(int font_descriptor, CString s)
{
//  int temp;

//  temp = curfont;
//  curfont = font_descriptor;
    curfont = 0;
	fonts[0] = m_fonts[font_descriptor];
	DrawString(s, &glfDrawWiredSymbol);
//  curfont = temp;
}

//void CGlf::glfDrawSolidSymbol(char s)
void glfDrawSolidSymbol(char s)
{
  unsigned char *b; /* Face pointer   */
  float *vp;        /* Vertex pointer */
  int i, j;
  float x, y;
  float temp_color[4];

  if ((curfont<0) || (fonts[curfont] == NULL)) return;
  
  if (fonts[curfont]->symbols[s] == NULL) return;

  b = fonts[curfont]->symbols[s]->fdata;
  vp = fonts[curfont]->symbols[s]->vdata;
  
  if (blending) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  }
  glBegin(GL_TRIANGLES); 
  for (i=0; i<fonts[curfont]->symbols[s]->facets; i++)
  {
      for (j=0; j<3; j++)
      {
        x = vp[*b*2];
        y = vp[*b*2+1];
        anchor_proc(x, y, ap);
        if (texturing == GLF_YES) glTexCoord2f((x+1)/2, (y+1)/2);
        glVertex2f(x, y);
        b++;
      }
  }
  glEnd();
  if (blending) {
  glDisable(GL_BLEND);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH);
  }

  /* Draw contour, if enabled */
  if (contouring == GLF_YES) 
  {
    glGetFloatv(GL_CURRENT_COLOR, temp_color);
    glColor4f(contouring_color.r, contouring_color.g, contouring_color.b, contouring_color.a);
    glfDrawWiredSymbol(s);
    glColor4fv(temp_color);
  }
}

/* Draw solid symbol by font_descriptor */
void CGlf::glfDrawSolidSymbolF(int font_descriptor, char s)
{
  int temp;

  temp = curfont;
  curfont = font_descriptor;
  glfDrawSolidSymbol(s);
  curfont = temp;
}

void CGlf::glfDrawSolidString(CString s)
{
  blending = m_blending;

  DrawString(s, glfDrawSolidSymbol);
}

/* Draw solid string by font_descriptor */
void CGlf::glfDrawSolidStringF(int font_descriptor, CString s)
{
//  int temp;

  //temp = curfont;
  //curfont = font_descriptor;
	curfont = 0;
	fonts[0] = m_fonts[font_descriptor];

	DrawString(s, &glfDrawSolidSymbol);
  //curfont = temp;
}

/* ------------ 3D Wired text drawing ---------------------- */

//void CGlf::glfDraw3DWiredSymbol(char s)
void glfDraw3DWiredSymbol(char s)
{
  int i, cur_line;
  float *tvp; /* temp vertex pointer */
  float x, y;
  
  if ((curfont<0) || (fonts[curfont] == NULL)) return;
  if (fonts[curfont]->symbols[(int)s] == NULL) return;

  glBegin(GL_LINE_LOOP);
  tvp = fonts[curfont]->symbols[s]->vdata;
  cur_line = 0;
  for (i=0; i<fonts[curfont]->symbols[s]->vertexs; i++)
  {
    x = *tvp;
    tvp++;
    y = *tvp;
    tvp++;
    anchor_proc(x, y, ap);
    glVertex3f(x, y, 1);
    if (fonts[curfont]->symbols[s]->ldata[cur_line] == i)
    {
      glEnd();
      cur_line++;
      if (cur_line < fonts[curfont]->symbols[s]->lines) glBegin(GL_LINE_LOOP);
      else break; /* No more lines */
    }
  }

  glBegin(GL_LINE_LOOP);
  tvp = fonts[curfont]->symbols[s]->vdata;
  cur_line = 0;
  for (i=0; i<fonts[curfont]->symbols[s]->vertexs; i++)
  {
    x = *tvp;
    tvp++;
    y = *tvp;
    tvp++;
    anchor_proc(x, y, ap);
    glVertex3f(x, y, 1+SymbolDepth);
    if (fonts[curfont]->symbols[s]->ldata[cur_line] == i)
    {
      glEnd();
      cur_line++;
      if (cur_line < fonts[curfont]->symbols[s]->lines) glBegin(GL_LINE_LOOP);
      else break; /* No more lines */
    }
  }

  glBegin(GL_LINES);
  tvp = fonts[curfont]->symbols[s]->vdata;
  for (i=0; i<fonts[curfont]->symbols[s]->vertexs; i++)
  {
    x = *tvp;
    tvp++;
    y = *tvp;
    tvp++;
    anchor_proc(x, y, ap);
    glVertex3f(x, y, 1);
    glVertex3f(x, y, 1+SymbolDepth);
  }
  glEnd();
}

/* Draw 3D wired symbol by font_descriptor */
void CGlf::glfDraw3DWiredSymbolF(int font_descriptor, char s)
{
  int temp;

  temp = curfont;
  curfont = font_descriptor;
  glfDraw3DWiredSymbol(s);
  curfont = temp;
}

void CGlf::glfDraw3DWiredString(CString s)
{
  DrawString(s, glfDraw3DWiredSymbol);
}

/* Draw 3D wired string by font_descriptor */
void CGlf::glfDraw3DWiredStringF(int font_descriptor, CString s)
{
//  int temp;

  //temp = curfont;
  //curfont = font_descriptor;
    curfont = 0;
	fonts[0] = m_fonts[font_descriptor];

	DrawString(s, glfDraw3DWiredSymbol);
  //curfont = temp;
}

/* ------------ 3D Solid text drawing ---------------------- */

//void CGlf::glfDraw3DSolidSymbol(char s)
void glfDraw3DSolidSymbol(char s)
{
  int i, j, cur_line, flag;
  float x, y, bx, by;
  unsigned char *b; /* Face pointer   */
  float *vp;        /* Vertex pointer */
  float *tvp;       /* temp vertex pointer */
  float temp_color[4];
  GLboolean light_temp;
  
  if ((curfont<0) || (fonts[curfont] == NULL)) return;
  if (fonts[curfont]->symbols[(int)s] == NULL) return;

  b = fonts[curfont]->symbols[s]->fdata;
  vp = fonts[curfont]->symbols[s]->vdata;

  glBegin(GL_TRIANGLES);   
  glNormal3f(0, 0, 1);
  for (i=0; i<fonts[curfont]->symbols[s]->facets; i++)
  {
    b += 2;
    for (j=0; j<3; j++)
    {
      x = vp[*b*2];
      y = vp[*b*2+1];
      anchor_proc(x, y, ap);
      glVertex3f(x, y, 1+SymbolDepth);
      b--;
    }
    b += 4;
  }
  glEnd();

  b = fonts[curfont]->symbols[s]->fdata;
  vp = fonts[curfont]->symbols[s]->vdata;
  
  glBegin(GL_TRIANGLES);   
  glNormal3f(0, 0, -1);
  for (i=0; i<fonts[curfont]->symbols[s]->facets; i++)
  {
      for (j=0; j<3; j++)
      {
        x = vp[*b*2];
        y = vp[*b*2+1];
        anchor_proc(x, y, ap);
        glVertex3f(x, y, 1);
        b++;
      }
  }
  glEnd();

  flag = 0;
  glBegin(GL_QUAD_STRIP);
  tvp = fonts[curfont]->symbols[s]->vdata;
  cur_line = 0;
  for (i=0; i<fonts[curfont]->symbols[s]->vertexs; i++)
  {
    x = *tvp;
    tvp++;
    y = *tvp;
    tvp++;
    if (!flag) 
    {
      bx = x;
      by = y;
      flag = 1;
    }
    anchor_proc(x, y, ap);
    glNormal3f(x, y, 0);
    glVertex3f(x, y, 1);
    glVertex3f(x, y, 1+SymbolDepth);
    if (fonts[curfont]->symbols[s]->ldata[cur_line] == i)
    {
      anchor_proc(bx, by, ap);
      glVertex3f(bx, by, 1);
      glVertex3f(bx, by, 1+SymbolDepth);
      flag = 0;
      glEnd();
      cur_line++;
      if (cur_line < fonts[curfont]->symbols[s]->lines) glBegin(GL_QUAD_STRIP);
      else break; /* No more lines */
    }
  }

  /* Draw contour, if enabled */
  if (contouring == GLF_YES) 
  {
    glGetBooleanv(GL_LIGHTING, &light_temp);
    glDisable(GL_LIGHTING);
    glGetFloatv(GL_CURRENT_COLOR, temp_color);
    glColor4f(contouring_color.r, contouring_color.g, contouring_color.b, contouring_color.a);
    glfDraw3DWiredSymbol(s);
    glColor4fv(temp_color);
    if (light_temp) glEnable(GL_LIGHTING);
  }
}

/* Draw 3D solid symbol by font_descriptor */
void CGlf::glfDraw3DSolidSymbolF(int font_descriptor, char s)
{
  int temp;

  temp = curfont;
  curfont = font_descriptor;
  glfDraw3DSolidSymbol(s);
  curfont = temp;
}

void CGlf::glfDraw3DSolidString(CString s)
{
  DrawString(s, glfDraw3DSolidSymbol);
}

/* Draw 3D solid string by font_descriptor */
void CGlf::glfDraw3DSolidStringF(int font_descriptor, CString s)
{
  int temp;

  temp = curfont;
  curfont = font_descriptor;
  DrawString(s, &glfDraw3DSolidSymbol);
  curfont = temp;
}

/* Get the size a string will have on screen */
void CGlf::glfGetStringBoundsF(int fd, CString s, float *minx, float *miny, float *maxx, float *maxy)
{
  struct glf_font *font;
  int i;
  float sda, sdb, cw = 0, minxx = 10;
  float top = 10, bottom = -10;

  if (fd < 0 || fd > (MAX_FONTS-1)) return;
  font = m_fonts[fd];

  if (font == NULL) return;

  if (font->symbols[s[0]])
    minxx = font->symbols[s[0]]->leftx;
  else
    minxx = 0.0;

  for (i=0; i<(int)s.GetLength(); i++)
  {
    if ((font->symbols[s[i]] == NULL) || (s[i] == ' '))
      cw += SpaceSize;
    else
    {
      sdb = -font->symbols[(int)s[i]]->leftx;
      sda = font->symbols[(int)s[i]]->rightx;

      cw += sda+sdb+SymbolDist;

      /* Update top/bottom bounds */
      if (font->symbols[(int)s[i]]->bottomy > bottom)
        bottom = font->symbols[(int)s[i]]->bottomy;

      if (font->symbols[(int)s[i]]->topy < top)
        top = font->symbols[(int)s[i]]->topy;
    }
  }

  cw += minxx;

  if ((maxx) && (maxy))
  {
    *maxx = cw;
    *maxy = bottom;
    anchor_proc(maxx, maxy, ap);
  }

  if ((minx) && (miny))
  {
    *minx = minxx;
    *miny = top;
    anchor_proc(maxx, maxy, ap);
  }
}

void CGlf::glfGetStringBounds(CString s, float *minx, float *miny, float *maxx, float *maxy)
{
  glfGetStringBoundsF(curfont, s, minx, miny, maxx, maxy);
}

void CGlf::glfSetSymbolSpace(float sp)
{
  SymbolDist = sp;
}

float CGlf::glfGetSymbolSpace()
{
  return SymbolDist;
}

void CGlf::glfSetSpaceSize(float sp)
{
  SpaceSize = sp;
}

float CGlf::glfGetSpaceSize()
{
  return SpaceSize;
}

void CGlf::glfSetSymbolDepth(float dpth)
{
  SymbolDepth = dpth;
}

float CGlf::glfGetSymbolDepth()
{
  return SymbolDepth;
}

int CGlf::glfSetCurrentFont(int Font_Descriptor)
{
  if ((Font_Descriptor < 0) || (m_fonts[Font_Descriptor] == NULL)) return GLF_ERROR;
  
  curfont = Font_Descriptor;
  return GLF_OK;
}

int CGlf::glfGetCurrentFont()
{
  return curfont;
}

void CGlf::glfSetAnchorPoint(int anchp)
{
  if ((anchp >= GLF_LEFT_UP) && (anchp <= GLF_RIGHT_DOWN))
    ap = anchp;
}

void CGlf::glfSetContourColor(float r, float g, float b, float a)
{
  contouring_color.r = r;
  contouring_color.g = g;
  contouring_color.b = b;
  contouring_color.a = a;
}

void CGlf::glfEnable(int what)
{
  switch (what) 
  {
    case GLF_CONSOLE_MESSAGES: console_msg = GLF_YES; break;
    case GLF_TEXTURING: texturing = GLF_YES; break;
    case GLF_CONSOLE_CURSOR: conCursor = GLF_YES; break;
    case GLF_CONTOURING: contouring = GLF_YES; break;
  }
}

void CGlf::glfDisable(int what)
{
  switch (what) 
  {
    case GLF_CONSOLE_MESSAGES: console_msg = GLF_NO; break;
    case GLF_TEXTURING: texturing = GLF_NO; break;
    case GLF_CONSOLE_CURSOR: conCursor = GLF_NO; break;
    case GLF_CONTOURING: contouring = GLF_NO; break;
  }
}

/* ---------------- Console functions ---------------------- */

void CGlf::glfSetConsoleParam(int width, int height)
{
  if (conData) free(conData);

  conWidth = width;
  conHeight = height;
  conData = (char *)malloc(width*height);
  glfConsoleClear();
}

int CGlf::glfSetConsoleFont(int Font_Descriptor)
{
  if ((Font_Descriptor < 0) || (m_fonts[Font_Descriptor] == NULL)) return GLF_ERROR;
  
  conFont = Font_Descriptor;
  return GLF_OK;
}

void CGlf::glfConsoleClear()
{
  memset(conData, 0, conWidth*conHeight);
  conx = 0;
  cony = 0;
}

void CGlf::glfPrint(CString s, int lenght)
{
  int i;

  for (i=0; i<lenght; i++)
  {
    if (s[i] > 31)
    {
      conData[cony*conWidth+conx] = s[i];
      conx++;
    } else
    if (s[i] == '\n') conx = conWidth;
    if (conx >= conWidth)
    {
      conx = 0;
      cony++;
      if (cony >= conHeight)
      {
        /* Shift all console contents up */
        memcpy(conData, &conData[conWidth], conWidth*(conHeight-1));
        /* Fill bottom line by spaces */
        memset(&conData[conWidth*(conHeight-1)], 0, conWidth);
        cony = conHeight-1;
      }
    }
  }
}

void CGlf::glfPrintString(CString s)
{
  glfPrint(s, strlen(s));
}

void CGlf::glfPrintChar(char s)
{
  glfPrint(&s, 1);
}

void CGlf::glfConsoleDraw()
{
  int i, j;
  char s[512];

  for (i=0; i<conHeight; i++)
  {
    memcpy(s, &conData[i*conWidth], conWidth);
    s[conWidth] = 0;
    if ((conCursor == GLF_YES) && (i == cony))
    {
      conCursorCount--;
      if (conCursorCount < 0)
      {
        conCursorCount = conCursorBlink;
        if (conCursorMode == GLF_YES) conCursorMode = GLF_NO;
        else conCursorMode = GLF_YES;
      }

      if (conCursorMode == GLF_YES)
        for (j=0; j<conWidth; j++)
        {
        if (!s[j])
          {
           s[j] = '_'; /* Draw cursor */
           s[j+1] = 0;
           break;
          }
        }
    }
    glfDrawSolidStringF(conFont, s);
    glTranslatef(0, -2, 0);
  }
}

void CGlf::glfSetCursorBlinkRate(int Rate)
{
  if (Rate > 0)
  {
    conCursorBlink = Rate;
    conCursorCount = Rate;
    conCursorMode = GLF_YES;
  }
}

