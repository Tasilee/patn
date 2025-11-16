// Scat.cpp : implementation file
//
// 3DView.cpp : implementation file
//

#include "stdafx.h"
#include "patn.h"
#include "Patndoc.h"
#include "edittext.h"
#include "texture.h"
#include <gl\gl.h>

#include <gl\glu.h>
#include <math.h>
#include "CFont.h"
#include "scat.h"
#include "bitms.h"
#include "tableview.h"
#include "attributeselection.h"
#include "pccvectorselection.h"
//#include "kwoptions.h"
#include "KruskalWalice.h"
//#include "Glf.h"

/////////////////////////////////////////////////////////////////////////////
// CScat

IMPLEMENT_DYNCREATE(CScat, CViewExtension)

CScat::CScat()
{
}

CScat::~CScat()
{
	long i;
	
	for(i=0;i<m_TextBoxes.GetSize();i++){

		delete m_TextBoxes[i];
	}
	
	m_pDoc->m_OrdVisible = 0;

}

BEGIN_MESSAGE_MAP(CScat, CViewExtension)
	//{{AFX_MSG_MAP(CScat)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID__ADDTEXTBOX, OnAddtextbox)
	ON_COMMAND(ID__DISPLAYCENTROIDS, OnDisplaycentroids)
	ON_COMMAND(ID__DISPLAYLABELS, OnDisplaylabels)
	ON_COMMAND(ID__DISPLAYMST, OnDisplaymst)
	ON_COMMAND(ID__SHOWAXIES, OnShowaxies)
	ON_COMMAND(ID__SHOWGROUPING, OnShowgrouping)
	ON_COMMAND(ID__SHOWSELECTEDONLY, OnShowselectedonly)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_COMMAND(ID__USEUSERGROUPCOLOURS, OnUseusergroupcolours)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID__SHOWRANGES, OnShowranges)
	ON_COMMAND(ID__USEBLACKWHITESYMBOLS, OnUseblackwhitesymbols)
	ON_UPDATE_COMMAND_UI(ID__USEBLACKWHITESYMBOLS, OnUpdateUseblackwhitesymbols)
	ON_COMMAND(ID__SELECTGROUPSTATISTICSATTRIBUTE, OnSelectgroupstatisticsattribute)
	ON_COMMAND(ID__SELECTPCCVECTORS, OnSelectpccvectors)
	ON_UPDATE_COMMAND_UI(ID__DISPLAYCENTROIDS, OnUpdateDisplaycentroids)
	ON_UPDATE_COMMAND_UI(ID__SHOWGROUPING, OnUpdateShowgrouping)
	ON_WM_MOVE()
	ON_UPDATE_COMMAND_UI(ID__KRUSKALWALLICEOPTIONS, OnUpdateKruskalWalliceOptions)
	ON_COMMAND(ID__KRUSKALWALLICEOPTIONS, OnKruskalWalliceOptions)
	ON_UPDATE_COMMAND_UI(ID__SELECTPCCVECTORS, OnUpdateSelectpccvectors)
	ON_COMMAND(ID__DISPLAYLEGEND, OnDisplayLegend)
	ON_UPDATE_COMMAND_UI(ID__USEUSERGROUPCOLOURS, OnUpdateUseusergroupcolours)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID__SAVEIMAGE, SaveIt)
	ON_COMMAND(ID__PRINTPREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID__PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScat drawing

void CScat::OnDraw(CDC* pDC)
{
	m_Fonttype = "Times";
	m_italic = 1;
	long labelSize = 15;

	//Check for updates
	if(m_pDoc->m_OrdUpdate)
	{
		m_pDoc->m_OrdUpdate = 0;
		//redraw all windows
		POSITION pos = m_pDoc->GetFirstViewPosition();   
		while (pos != NULL)   
		{
			CView* pView = m_pDoc->GetNextView(pos);   
			pView->RedrawWindow();
		} 
	}

	double width, height;

	static IN_OnDraw = FALSE;
	if(IN_OnDraw) return;
	IN_OnDraw = TRUE;

	if(m_saveimage){
		SaveIt();
		return;
	}
	

	width = m_Rect.right - 120;
	height = m_Rect.bottom;


	CalcWindowRect(m_Rect);

	if(m_hRC == NULL)
		pDC->TextOut(0,15,"Invalid Rendering Context!",26);
	else
		{
		// Make the rendering context current

		wglMakeCurrent(m_hDC,m_hRC);
		glClearColor(1,0,0,1);
	
		// Clear the window with current clearing color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(m_Font == NULL)
//			m_Font = m_ccfont.FontCreate(wglGetCurrentDC(), m_Fonttype, labelSize, 0, m_italic,0);
			m_Font = m_ccfont.FontCreate(wglGetCurrentDC(), m_pDoc->m_lfCells);

		if(m_pDoc->m_OrdChange){
			//function to check if data has change needs to be run;
		}else{
			DrawScene();
		}

		// Swap our scene to the front
		SwapBuffers(m_hDC);

		// Allow other rendering contexts to co-exist
		wglMakeCurrent(NULL,NULL);
	}

	IN_OnDraw = FALSE;

	m_waitforrefresh = false;
}


/////////////////////////////////////////////////////////////////////////////
// CScat diagnostics

#ifdef _DEBUG
void CScat::AssertValid() const
{
	CView::AssertValid();
}

void CScat::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CScat message handlers

int CScat::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{	

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Gstaattrib = 0;
	m_Font = NULL;

	wAngleY = 0.0f;

	m_saveimage = false;
	m_printimage = false;
	m_psd = NULL;
	m_waitforrefresh = false;

	whiteLight[0] = 0.45f;
	whiteLight[1] = 0.45f;
	whiteLight[2] = 0.45f;
	whiteLight[3] = 1.0f;
	sourceLight[0] = 0.25f;
	sourceLight[1] = 0.25f;
	sourceLight[2] = 0.25f;
	sourceLight[3] = 1.0f;
	lightPos[0] = -50.0f;
	lightPos[1] = 25.0f;
	lightPos[2] = 250.0f;
	lightPos[3] = 0.0f;
	
	m_pDoc = GetDocument();
	m_pDoc->m_OrdVisible = 1;

	//init variables for operation
	m_Rotate = false;
	m_Labels = false;
	m_Legend = false;
	m_Mst = false;
	m_Axies = false;
	m_Grouping = false;
	m_Usergroupcolours = false;
	m_SelOnly = false;
	m_Centroid = false;
	m_Showranges = false;
	m_Symbols = false;
	m_RotationSpeed = 0.2;
	m_currenttextbox = -1;
	m_currentobject = m_currentcentroid = -1;
	m_compareobject = m_comparecentroid = -1;

	m_infobar.ordin1.SetSize(3);
	m_infobar.ordin2.SetSize(3);
	m_infobar.group1.SetSize(6);
	m_infobar.group2.SetSize(6);

	//initialize menu
	m_menu.LoadMenu(IDR_MENU_SCAT);
	
/*	CString strError;
	CString patterns = ((CPatnApp*)AfxGetApp())->m_ProgramPath;
	patterns += "patterns\\patterns.dat";
	m_NoSymbols = false;
	if(!m_tex.LoadPatterns(patterns)){
		strError = "Failed to find file ";
		strError += patterns;
		strError += "\nBlack and White Symbols will be unavailable.";
		if(((CPatnApp*)AfxGetApp())->m_fileerrormsgs) AfxMessageBox(strError);
		m_NoSymbols = true;
	}*/
	m_NoSymbols = false;
	CString patterns;
	HRSRC rsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_TEXT1),"TEXT");
	HGLOBAL h = LoadResource(NULL,rsrc);
	char * c = (char*)h;
	long i=0;//loop until "eee" occurs
	while(!(c[i] == 'e' && c[i+1] == 'e' && c[i+2] == 'e')){
		patterns.Insert(i,c[i]);
		i++;
	}
	patterns.Insert(i,'e');
	if(!m_tex.LoadPatterns(0,patterns)){
		m_NoSymbols = true;
	}

	i;
	for(i=0;i<m_pDoc->m_PatternObjects.GetSize();i++){
		if(m_pDoc->m_PatternObjects[i] == -1)
			m_pDoc->m_PatternObjects[i] = i;
	}

	return 0;
}

void CScat::OnDestroy() 
{
	HGLRC   hrc;
	CPatnDoc* pDoc = GetDocument();

	hrc = ::wglGetCurrentContext();

	::wglMakeCurrent(NULL,  NULL);

//	pDoc->m_pExists = 0;
	m_ccfont.FontDestroy(m_Font);

	CView::OnDestroy();
	
}

void CScat::DrawScene()
{
	//if (m_saveimage || m_printimage) m_glf.m_blending = false;
	//else m_glf.m_blending = true;

	InitObjSpace();
	DrawObjSpace();
	InitTextSpace();
	DrawTextSpace();
	if((m_infobar.item == object && m_currentobject != -1) ||
		m_currentcentroid != -1){
		InitInfoSpace();
		DrawInfoSpace();
	}
	if(!m_printimage && m_Legend){
		InitLegendSpace();
		DrawLegendSpace();
	}
}

void CScat::InitObjSpace()
{
	long width, height;
	float faspect;

	if(m_printimage)
		width = m_Rect.right - 360;
	else
		width = m_Rect.right - 120;
	height = m_Rect.bottom;
	faspect = (float) width / height;

	m_dist = height;

	if(m_printimage)
		glViewport(360,0,(long)m_dist,(long)m_dist);
	else
		glViewport(120,0,(long)m_dist,(long)m_dist);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(-0.5,0.5,-0.5,0.5,1,4);
	glTranslatef(0,0,0);
 	glGetDoublev(GL_PROJECTION_MATRIX, m_ProjMatrix);

	if(m_Rotate){
		wAngleY += (float)m_RotationSpeed;
	}

	glTranslatef(0,0,-2);
	glRotatef(wAngleY, 0.0f, 1.0f, 0.0f);
	glTranslatef(0,0,2);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//set center of space so objects draw 0to1 on x&y&z
	glTranslatef(-0.5,-0.5,-2.5);

	glEnable(GL_DEPTH_TEST);	// Hidden surface removal

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Setup and enable light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,whiteLight);
	glLightfv(GL_LIGHT0,GL_AMBIENT,sourceLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,sourceLight);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	
	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClearDepth(1.0f);
}


void CScat::InitInfoSpace()
{
	long width, height;

	if(m_printimage)
		width = 360;
	else
		width = 120;
	height = m_Rect.bottom;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CScat::InitLegendSpace()
{
	long width, height;

	//2 cases:
	// a) group symbols in legend
	// - true iff m_Centroid || m_Usergroupcolours || m_Grouping
	// b) object symbols in legend
	// - true else

	long nObjects;

	if(m_Centroid || m_Usergroupcolours || m_Grouping)
		nObjects = m_pDoc->m_aGroupColours.GetSize();
	else
		nObjects = m_pDoc->m_aObjectColours.GetSize();

	height = m_Rect.bottom;
	width = (long)(m_Rect.right - m_dist-120);

	long p = 1;
	if(m_printimage)
		p = 3;
			
	glViewport((long)((m_dist+120)*p), 0, width*p, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void CScat::InitTextSpace()
{
	if(m_printimage)
		glViewport(360,0,m_Rect.right-360,m_Rect.bottom);
	else
		glViewport(120, 0, m_Rect.right-120, m_Rect.bottom);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(m_printimage)
		gluOrtho2D(0, m_Rect.right-360, 0, m_Rect.bottom);
	else
		gluOrtho2D(0, m_Rect.right-120, 0, m_Rect.bottom);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CScat::DrawObjSpace()
{
//	GLFONT * Font;
	CString label;
	CString m_Fonttype;
	long i,grp,j;
	double x, y, z;
	GLUquadricObj *object;
	GLUquadricObj *centroid;
	GLUquadricObj *arrow;
	static GLfloat  wAngleX = 1.0f;
	static GLfloat  wAngleZ = 5.0f;

//	m_labelsize = 12;
//	Font = m_ccfont.FontCreate(wglGetCurrentDC(), "Times", 12, 0, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	object = gluNewQuadric();
	centroid = gluNewQuadric();
	arrow = gluNewQuadric();

	glPushMatrix();
	if(m_Centroid) {
		for(i=0;i<m_pDoc->m_aGroupColours.GetSize();i++){	
			if(!m_Usergroupcolours)
				glColor3f(m_pDoc->m_aGroupColours[i].red,
					m_pDoc->m_aGroupColours[i].green,
					m_pDoc->m_aGroupColours[i].blue);
			else 
				glColor3f(m_pDoc->m_UserGroupColours[i].red,
					m_pDoc->m_UserGroupColours[i].green,
					m_pDoc->m_UserGroupColours[i].blue);
			
			if(m_Symbols){
				glPushMatrix();
				glColor3f(0.0,0.0,0.0);
				glTranslated(m_pDoc->m_aGroupColours[i].red,
					m_pDoc->m_aGroupColours[i].green,
					1);
				glScaled(.007,.007,.007);
				if((long)wAngleY%360 > 97 && 
					(long)wAngleY%360 < 278){
					glRotated(-wAngleY,0,1,0);
					m_tex.DrawPattern3D(m_pDoc->m_PatternGroups[i],true);
				}else{
					glRotated(-wAngleY-180,0,1,0);
					m_tex.DrawPattern3D(m_pDoc->m_PatternGroups[i],false);
				}
				glPopMatrix();
			}else if(m_Labels){
				x = m_pDoc->m_aGroupColours[i].red;
				y = m_pDoc->m_aGroupColours[i].green;
				z = m_pDoc->m_aGroupColours[i].blue;
				//code to draw centroid labels, see colour retrieval for coords					
				m_pDoc->GetRowLabel(i,label);
				label.Format("%d",m_pDoc->m_Groups.GetAt(i));
				glRasterPos3d(x, y, 1);
				m_ccfont.FontPuts(m_Font, label);
			}else{
				x = m_pDoc->m_aGroupColours[i].red;
				y = m_pDoc->m_aGroupColours[i].green;
				z = m_pDoc->m_aGroupColours[i].blue;
				//code to draw centroid points, see colour retrieval for coords
				glPushMatrix();
				
				glTranslated(x, y, 1);
				gluCylinder(centroid, 0.025, 0.0, 0.05, 10, 10);
				
				glPopMatrix();	
			}
		}
	} else { 
		for(i=0;i<m_pDoc->m_aObjectColours.GetSize();i++){
			//if statement for selected objects only display
			if(!m_SelOnly || m_pDoc->m_RowSelection->IsSelected(i)){
				if(m_Grouping)
					glColor3f(m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].red,
						m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].green,
						m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].blue);
				else if(m_Usergroupcolours)
					glColor3f(m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].red,
						m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].green,
						m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].blue);
				else
					glColor3f(m_pDoc->m_aObjectColours[i].red,
						m_pDoc->m_aObjectColours[i].green, m_pDoc->m_aObjectColours[i].blue);	

				if(m_Symbols && !m_Grouping){
					glPushMatrix();
					glColor3f(0.0,0.0,0.0);
					glTranslated(m_pDoc->m_aObjectColours[i].red,
						m_pDoc->m_aObjectColours[i].green,
						1);
					glScaled(.007,.007,.007);
					//to fix 'grey' problem
					if((long)wAngleY%360 > 97 && 
						(long)wAngleY%360 < 278){
						glRotated(-wAngleY,0,1,0);
						m_tex.DrawPattern3D(m_pDoc->m_PatternObjects[i],true);
					}else{
						glRotated(-wAngleY-180,0,1,0);
						m_tex.DrawPattern3D(m_pDoc->m_PatternObjects[i],false);
					}
					glPopMatrix();
				}else if(m_Symbols && m_Grouping){
					glPushMatrix();
					glColor3f(0.0,0.0,0.0);
					glTranslated(m_pDoc->m_aObjectColours[i].red,
						m_pDoc->m_aObjectColours[i].green,
						1);
					glScaled(.007,.007,.007);
					grp = m_pDoc->GetRowGroups(i);
					for(j=0;j<m_pDoc->m_Groups.GetSize();j++){
						if(m_pDoc->m_Groups[j] == grp)
							break;
					}
					//to fix 'grey' problem
					if((long)wAngleY%360 > 97 && 
						(long)wAngleY%360 < 278){
						glRotated(-wAngleY,0,1,0);
						m_tex.DrawPattern3D(m_pDoc->m_PatternGroups[j],true);
					}else{
						glRotated(-wAngleY-180,0,1,0);
						glScaled(.7,.7,.7);
						m_tex.DrawPattern3D(m_pDoc->m_PatternGroups[j],false);
						glScaled(10/7.0,10/7.0,10/7.0);
					}					
					glPopMatrix();
				}else if(m_Labels){ 
					x = m_pDoc->m_aObjectColours[i].red;
					y = m_pDoc->m_aObjectColours[i].green;
					z = m_pDoc->m_aObjectColours[i].blue;
					//draw label for i
					m_pDoc->GetRowLabel(i,label);
					glRasterPos3d(x,y,1);
					m_ccfont.FontPuts(m_Font, label);
				}else{
					x = m_pDoc->m_aObjectColours[i].red;
					y = m_pDoc->m_aObjectColours[i].green;
					z = m_pDoc->m_aObjectColours[i].blue;
					//Draw object for i
					glPushMatrix();
					glTranslated(x,y,1);
					gluSphere(object,0.025,10,10);
					glPopMatrix();
				}
			} // end of selection if statement
		} // end of for-loop
		if (m_Mst) { DrawMst(); }
	}  // end of if m_Centroid else statement

	float Xrange = (float)(m_pDoc->m_OrdXmax-m_pDoc->m_OrdXmin);
	float Yrange = (float)(m_pDoc->m_OrdYmax-m_pDoc->m_OrdYmin);
	float Zrange = (float)(m_pDoc->m_OrdZmax-m_pDoc->m_OrdZmin);
	//works only if min's are negative
	// finds ranged zero of x,y,z
	float Xcentre = (float)(-m_pDoc->m_OrdXmin/Xrange);
	float Ycentre = (float)(-m_pDoc->m_OrdYmin/Yrange);
	float Zcentre = (float)(-m_pDoc->m_OrdZmin/Zrange);

	//call to draw axes
	if(m_Axies){ // need to change variable name to m_axes
		glLineWidth(1.5);

		glColor3d(0.0, 0.0, 0.0);
		glBegin(GL_LINES);
			// x axis
			glVertex3f(Xcentre, Ycentre,1);
			glVertex3f(Xcentre+0.2f,Ycentre,1);
			// y axis
			glVertex3f(Xcentre, Ycentre, 1);
			glVertex3f(Xcentre, Ycentre+0.2f, 1);
			// z axis
			glVertex3f(Xcentre, Ycentre, 1);
			glVertex3f(Xcentre, Ycentre, 1);
		glEnd();
		glLineWidth(1.0);
	}
	glPopMatrix();

	if(m_Showranges){
		glColor3f(0.0,0.0,0.0);
		//draw range labels
		label.Format("%.3f",m_pDoc->m_OrdXmin);
		glRasterPos3f(-.05f,-.1f,1);
		m_ccfont.FontPuts(m_Font, label);
		label.Format("%.3f",m_pDoc->m_OrdXmax);
		glRasterPos3f(.95f,-.1f,1);
		m_ccfont.FontPuts(m_Font, label);
		label.Format("%.3f",m_pDoc->m_OrdYmin);
		glRasterPos3f(-.15f,0,1);
		m_ccfont.FontPuts(m_Font, label);
		label.Format("%.3f",m_pDoc->m_OrdYmax);
		glRasterPos3f(-.15f,1,1);
		m_ccfont.FontPuts(m_Font, label);
		//label.Format("%.3f",m_pDoc->m_OrdZmin);
		//glRasterPos3f(1.1f,0,1);
		//m_ccfont.FontPuts(m_Font, label);
		//label.Format("%.3f",m_pDoc->m_OrdZmax);
		//glRasterPos3f(1.1f,0,0);
		//m_ccfont.FontPuts(m_Font, label);

		//draw box & scales
		//
		//draw boundaries
		glPushMatrix();
		glColor3f(0,0,0);
		glBegin(GL_LINE_STRIP);
		glVertex3i(0,1,1);
		glVertex3i(0,0,1);
		glVertex3i(1,0,1);
		//glVertex3i(1,0,0);
		glEnd();

		//draw idx's
		glBegin(GL_LINES);
		glVertex3f(0,1,1);
		glVertex3f(-.05f,1,1);
		glVertex3f(0,0,1);
		glVertex3f(-.05f,0,1);
		glVertex3f(0,0,1);
		glVertex3f(0,-0.05f,1);
		glVertex3f(1,0,1);
		glVertex3f(1,-0.05f,1);
		glVertex3f(1,0,1);
		glVertex3f(1.05f,0,1);
		glVertex3f(1,0,0);
		glVertex3f(1.05f,0,0);
		glEnd();
		glPopMatrix();
	}

	glPushMatrix();
	//highlight an object
	glColor3f(1.0,0,0);
	if((m_currentobject >= 0 && m_currentobject < m_pDoc->m_aObjectColours.GetSize()) ||
		m_hint == objectselection || m_hint == objectcompare){

		//highlight single object at 
		// m_pDoc->m_aObjectColours[m_currentobject]
		m_pDoc->GetRowLabel(m_currentobject,label);
		glRasterPos3f(m_pDoc->m_aObjectColours[m_currentobject].red, 
			m_pDoc->m_aObjectColours[m_currentobject].green,
			1);
		m_ccfont.FontPuts(m_Font, label);

		if((m_compareobject >= 0) &&
			m_compareobject < m_pDoc->m_aObjectColours.GetSize()){
			//highlight compare object at 
			// m_pDoc->m_aObjectColours[m_compareobject]
			m_pDoc->GetRowLabel(m_compareobject,label);
			glRasterPos3f(m_pDoc->m_aObjectColours[m_compareobject].red, 
				m_pDoc->m_aObjectColours[m_compareobject].green,
				1);
			m_ccfont.FontPuts(m_Font, label);
		}
	}

	//highlight an centroid
	glColor3f(1.0,0,0);
	if((m_currentcentroid >= 0 && m_currentcentroid < m_pDoc->m_aGroupColours.GetSize()) ||
		m_hint == centroidselection || m_hint == centroidcompare){

		//highlight single centroid at 
		// m_pDoc->m_aGroupColour[m_currentcentroid]
		// label = ?
		label.Format("%d",m_pDoc->m_Groups.GetAt(m_currentcentroid));
		glRasterPos3f(m_pDoc->m_aGroupColours[m_currentcentroid].red, 
			m_pDoc->m_aGroupColours[m_currentcentroid].green,
			1);
		m_ccfont.FontPuts(m_Font, label);

		if(m_comparecentroid != -1){
			//highlight compare centroid at 
			// m_pDoc->m_aGroupColours[m_comparecentroid]
			label.Format("%d",m_pDoc->m_Groups.GetAt(m_comparecentroid));
			glRasterPos3f(m_pDoc->m_aGroupColours[m_comparecentroid].red, 
				m_pDoc->m_aGroupColours[m_comparecentroid].green,
				1);
			m_ccfont.FontPuts(m_Font, label);
	
		}
	}
	glPopMatrix();

	//draw pcc vectors found in m_PccVectors
	double xpos,ypos,zpos;
	glPushMatrix();
	for(i=0;i<m_PccVectors.GetSize();i++){
		glPushMatrix();
		glBegin(GL_LINES);
		glColor3f(1,0,0);
		xpos = m_pDoc->m_Vectors.GetAt(m_PccVectors.GetAt(i))->GetAt(0);
		ypos = m_pDoc->m_Vectors.GetAt(m_PccVectors.GetAt(i))->GetAt(1);
		zpos = m_pDoc->m_Vectors.GetAt(m_PccVectors.GetAt(i))->GetAt(2);
		glVertex3d(xpos/2.0+Xcentre,ypos/2.0+Ycentre,zpos/2.1);
		glVertex3d(Xcentre,Ycentre,1);
		glEnd();
		glPopMatrix();
		glPushMatrix();
		glColor3f(0,0,0);
		m_pDoc->GetColumnLabel(m_PccVectors.GetAt(i),label);
		glRasterPos3d(xpos/2.0+Xcentre,ypos/2.0+Ycentre,zpos/2.1);
		m_ccfont.FontPuts(m_Font, label);

		glPopMatrix();
		//if extrinsic, underline label
		/*** TAKE OUT BECAUSE NOT ALWAYS IN CORRECT POSITION
		if(m_pDoc->GetIdxLeftExtrinsicColumn() != -1 &&
			m_PccVectors.GetAt(i) >= m_pDoc->GetIdxLeftExtrinsicColumn()){
			glPushMatrix();
			glTranslated(xpos+Xcentre,ypos+Ycentre,zpos+Zcentre);
			glRotatef(-wAngleY,0,1,0);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(xpos+Xcentre + label.GetLength()/50,
				0,0);
			glEnd();
			glPopMatrix();
		}
		***/
	}
	
	glPopMatrix();
//	m_ccfont.FontDestroy(Font);

	gluDeleteQuadric(object);
	gluDeleteQuadric(centroid);
	gluDeleteQuadric(arrow);
}


void CScat::DrawInfoSpace()
{
	long i,j;
	CString text,str;
	long height = m_Rect.bottom-10;

	glColor3f(0.0, 0.0, 0.0);
	glPushMatrix();
	if(m_printimage){
		glScalef(3,3,1);
		height = 480;
	}

	//stress value
	text.Format("Stress: %4.4f",m_pDoc->m_OrdStress);
	height -= 10;
	glRasterPos2i(5, height);
	m_ccfont.FontPuts(m_Font, text);
	height -= 20;

	if (m_infobar.item == object && m_currentobject != -1) {
		// print information on object one
		text = "1st OBJECT:";
		glRasterPos2i(5, height - 10);
		m_ccfont.FontPuts(m_Font, text);
		text = "Name:";
		glRasterPos2i(10, height - 20);
		m_ccfont.FontPuts(m_Font, text);
		glColor3f(1.0, 0.0, 0.0);
		glRasterPos2i(10, height - 30);
		m_ccfont.FontPuts(m_Font, m_infobar.name1);
		glColor3f(0,0,0);
		if (m_infobar.itemNr == 2) {
			//print information on object two
			text = "2nd OBJECT:";
			glRasterPos2i(5, height - 85);
			m_ccfont.FontPuts(m_Font, text);
			text = "Name:";
			glRasterPos2i(10, height - 95);
			m_ccfont.FontPuts(m_Font, text);
			glColor3f(1.0, 0.0, 0.0);
			glRasterPos2i(25, height - 105);
			m_ccfont.FontPuts(m_Font, m_infobar.name2);
			glColor3f(0.0, 0.0, 0.0);
			// print association measure
			text = "ASSOCIATION:";
			glRasterPos2i(5, height - 160);
			m_ccfont.FontPuts(m_Font, text);
			text.Format("%.5f", m_infobar.assoc);
			glRasterPos2i(25, height - 170);
			m_ccfont.FontPuts(m_Font, text);
		}
	} else if(m_currentcentroid != -1){
		// print information on group one
		text = "1st GROUP:";
		glRasterPos2i(5, height - 10);
		m_ccfont.FontPuts(m_Font, text);

		text = "Group#:";
		
		glRasterPos2i(10, height - 20);
		m_ccfont.FontPuts(m_Font, text);
		glColor3f(1,0,0);
		glRasterPos2i(50, height - 20);
		m_ccfont.FontPuts(m_Font, m_infobar.name1);
		glColor3f(0,0,0);
    
		if (m_infobar.itemNr == 2) {
			//print information on object two
			text = "2nd GROUP:";
			glRasterPos2i(5, height - 75);
			m_ccfont.FontPuts(m_Font, text);

			text = "Group#:";

			glRasterPos2i(10, height - 85);
			m_ccfont.FontPuts(m_Font, text);
			glColor3f(1,0,0);
			glRasterPos2i(50, height - 85);
			m_ccfont.FontPuts(m_Font, m_infobar.name2);
			glColor3f(0,0,0);

			//Kruskal-Wallis column titles
			
			text = "Kruskal-Wallis";
			glRasterPos2i(5, height - 140);
			m_ccfont.FontPuts(m_Font, text);
			text = "Top 5";
			glRasterPos2i(5, height - 150);
			m_ccfont.FontPuts(m_Font, text);

			j = m_KWOrder.GetSize();
			//for(i=0;i<m_pDoc->m_KWNumVisible && i<j;i++){
			for(i=0;i<5 && i<j;i++){
				m_pDoc->GetColumnLabel(m_pDoc->m_KWAttributes[m_KWOrder[j-1-i]],str);
				text.Format("%.4f  %s",m_KWValues[j-1-i],str);
				glColor3f(1,0,0);
				glRasterPos2i(10, height - 165 - 10*i);
				m_ccfont.FontPuts(m_Font, text);
				glColor3f(0,0,0);
			}
			
		}else{ //single group selected
			//Kruskal-Wallice column titles
			text = "Kruskal-Wallis";
			glRasterPos2i(5, height - 140);
			m_ccfont.FontPuts(m_Font, text);
			text = "Top 5";
			glRasterPos2i(5, height - 150);
			m_ccfont.FontPuts(m_Font, text);

			j = m_pDoc->m_KW.GetSize();
			for(i=0;i<5 && i < j;i++){
				m_pDoc->GetColumnLabel(m_pDoc->m_KWAttributes[m_pDoc->m_KW[j-1-i]],str);
				text.Format("%.4f  %s",m_pDoc->m_KWValues[j-1-i],str);
				glColor3f(1,0,0);
				glRasterPos2i(10, height - 165 - 10*i);
				m_ccfont.FontPuts(m_Font, text);
				glColor3f(0,0,0);
			}			
		}
	}
	glPopMatrix();
//	m_ccfont.FontDestroy(m_Font);
}

void CScat::DrawLegendSpace()
{
	long i,x,y,nCols,nRows,nObjects;
	CString text,str;
	long height = m_Rect.bottom;

	GLUquadricObj *object = gluNewQuadric();

	glColor3f(0.0, 0.0, 0.0);
	glPushMatrix();

	bool groups = m_Centroid || m_Usergroupcolours || m_Grouping;

	if(groups)
		nObjects = m_pDoc->m_aGroupColours.GetSize();
	else
		nObjects = m_pDoc->m_aObjectColours.GetSize();
	
	nRows = (long)floor(height/20.0)-1;
	nCols = (long)ceil(nObjects/(double)nRows);
	y = 40;
	x = 5;

	long grp,j;
	
	glColor3f(0,0,1);
	text = "LEGEND";
	glRasterPos2i(x, height-15);
	m_ccfont.FontPuts(m_Font, text);
			
	glColor3f(0,0,0);
	
	for(i=0;i<nObjects;i++){
		if(!(groups || (!m_SelOnly || m_pDoc->m_RowSelection->IsSelected(i))))
			continue;
		//1st, draw label
		if(groups)
			text.Format("Group %d",m_pDoc->m_Groups[i]);
		else
			m_pDoc->GetRowLabel(i,text);
		glColor3f(0,0,0);
		glRasterPos2i(x, height-y);
		m_ccfont.FontPuts(m_Font, text);
			
		//2nd, draw object, symbol, with appropriate colour
		glPushMatrix();
		glTranslated(x+70,height-y+5,0.0);
		if(groups) {
			if(!m_Usergroupcolours)
				glColor3f(m_pDoc->m_aGroupColours[i].red,
					m_pDoc->m_aGroupColours[i].green,
					m_pDoc->m_aGroupColours[i].blue);
			else 
				glColor3f(m_pDoc->m_UserGroupColours[i].red,
					m_pDoc->m_UserGroupColours[i].green,
					m_pDoc->m_UserGroupColours[i].blue);
			
			if(m_Symbols){
				glPushMatrix();
				glTranslated(-10,0,0);
				glColor3f(0.0,0.0,0.0);
				glScaled(1.5,1.5,1.5);
				glRotated(180,0,1,0);
				m_tex.DrawPattern2D(m_pDoc->m_PatternGroups[i]);
				glPopMatrix();
			}else{
				//code to draw centroid points, see colour retrieval for coords
				glPushMatrix();
				gluDisk(object,0,5,7,1);				
				glPopMatrix();	
			}
		}else { 
			//if statement for selected objects only display
			if(m_Grouping)
				glColor3f(m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].red,
					m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].green,
					m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].blue);
			else if(m_Usergroupcolours)
				glColor3f(m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].red,
					m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].green,
					m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].blue);
			else
				glColor3f(m_pDoc->m_aObjectColours[i].red,
					m_pDoc->m_aObjectColours[i].green, m_pDoc->m_aObjectColours[i].blue);	

			if(m_Symbols && !m_Grouping){
				glPushMatrix();
				glColor3f(0.0,0.0,0.0);
				//glTranslated(m_pDoc->m_aObjectColours[i].red,
				//	m_pDoc->m_aObjectColours[i].green,
				//	m_pDoc->m_aObjectColours[i].blue);
				glTranslated(-10,0,0);
				glScaled(1.5,1.5,1.5);
				glRotated(180,0,1,0);
				m_tex.DrawPattern2D(m_pDoc->m_PatternObjects[i]);
				glPopMatrix();
			}else if(m_Grouping && m_Symbols){
				glPushMatrix();
				glTranslated(-10,0,0);
				glColor3f(0.0,0.0,0.0);
				glTranslated(m_pDoc->m_aObjectColours[i].red,
					m_pDoc->m_aObjectColours[i].green,
					m_pDoc->m_aObjectColours[i].blue);
				glScaled(1.5,1.5,1.5);
				grp = m_pDoc->GetRowGroups(i);
				for(j=0;j<m_pDoc->m_Groups.GetSize();j++){
					if(m_pDoc->m_Groups[j] == grp)
						break;
				}
				glRotated(180,0,1,0);
				m_tex.DrawPattern2D(m_pDoc->m_PatternGroups[j]);
				glPopMatrix();
			}else{
				gluDisk(object,0,5,7,1);
			}
		} // end of selection if statement
		glPopMatrix();
	
		if(y/20 == nRows){
			y = 40;
			x += 100;
		}else{
			y += 20;
		}
	}	
	
	glPopMatrix();

	gluDeleteQuadric(object);

}

void CScat::DrawTextSpace()
{
	long i;
	double factor = 40.0/15.0;
	if(!m_printimage)
		factor = 1;

	for(i=0;i<m_TextBoxes.GetSize();i++){
		if(i != m_currenttextbox || m_currenttextbox == -1){
			glColor3f(0.0, 0.0, 0.0);
			glRasterPos3d((double) (m_TextBoxes[i]->location.left)*(m_Rect.right-360)/(m_SaveHeight),
				(double)(m_SaveHeight-(m_TextBoxes[i]->location.bottom))*m_Rect.bottom/m_SaveHeight+15/2.0, 0.0);
			m_ccfont.FontPuts(m_Font, m_TextBoxes[i]->contents);
		}
	}

	//highlight a textbox
	if((m_currenttextbox >= 0 && m_currenttextbox < m_TextBoxes.GetSize()) || 
		m_hint == textboxselection || m_hint == textboxmove){
		glColor3f(1.0, 0.0, 0.0);
		glRasterPos3d((double) (m_TextBoxes[m_currenttextbox]->location.left)*(m_Rect.right-360)/(m_SaveHeight),
				(double)(m_SaveHeight-(m_TextBoxes[m_currenttextbox]->location.bottom))*m_Rect.bottom/m_SaveHeight+15/2.0, 0.0);
		m_ccfont.FontPuts(m_Font, m_TextBoxes[m_currenttextbox]->contents);
	}		

	double x,y,z,x1,y1,z1,x2,y2;
	double xx,yy,zz,xx1,yy1,zz1,xx2,yy2;

if(m_compareobject != -1){
		x = m_pDoc->m_aObjectColours[m_currentobject].red;
		y = m_pDoc->m_aObjectColours[m_currentobject].green;
		z = 1;
		x1 = x;
		y1 = y;
		z1 = z;
		z1 = (z-.5)*cos(0.0) + 
			(.5-x)*sin(0.0) + 0.5;
		y2 = (0.5 - (y1-.5)/(z1-2.5));
		x1 = (x-.5)*cos(0.0) + 
			(z-.5)*sin(0.0) + 0.5;
		x2 = (0.5 - (x1-.5)/(z1-2.5));

		xx = m_pDoc->m_aObjectColours[m_compareobject].red;
		yy = m_pDoc->m_aObjectColours[m_compareobject].green;
		zz = 1;
		xx1 = xx;
		yy1 = yy;
		zz1 = zz;
		zz1 = (zz-.5)*cos(0.0) + 
			(.5-xx)*sin(0.0) + 0.5;
		yy2 = (0.5 - (yy1-.5)/(zz1-2.5));
		xx1 = (xx-.5)*cos(0.0) + 
			(zz-.5)*sin(0.0) + 0.5;
		xx2 = (0.5 - (xx1-.5)/(zz1-2.5));

		//draw line from pDoc->m_aObjectColour[m_currentobject]
		// to m_local (CPoint)
		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_LINES);
			glVertex2d(x2*m_dist,y2*m_dist);
			glVertex2d(xx2*m_dist,yy2*m_dist);
		glEnd();
	}else if(m_hint == objectcompare){
		x = m_pDoc->m_aObjectColours[m_currentobject].red;
		y = m_pDoc->m_aObjectColours[m_currentobject].green;
		z = 1;
		x1 = x;
		y1 = y;
		z1 = z;
		z1 = (z-.5)*cos(0.0) + 
			(.5-x)*sin(0.0) + 0.5;
		y2 = (0.5 - (y1-.5)/(z1-2.5));
		x1 = (x-.5)*cos(0.0) + 
			(z-.5)*sin(0.0) + 0.5;
		x2 = (0.5 - (x1-.5)/(z1-2.5));

		//draw line from pDoc->m_aObjectColour[m_currentobject]
		// to m_local (CPoint)
		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_LINES);
			glVertex2d(x2*m_dist,y2*m_dist);
			if(m_printimage)
				glVertex2d((double)(m_comparepoint.x - 360) ,
					(double)(m_Rect.bottom - m_comparepoint.y));
			else
				glVertex2d((double)(m_comparepoint.x - 120) ,
					(double)(m_Rect.bottom - m_comparepoint.y));
		glEnd();
	}
	if(m_comparecentroid != -1){
		x = m_pDoc->m_aGroupColours[m_currentcentroid].red;
		y = m_pDoc->m_aGroupColours[m_currentcentroid].green;
		z = 1;
		x1 = x;
		y1 = y;
		z1 = z;
		z1 = (z-.5)*cos(0.0) + 
			(.5-x)*sin(0.0) + 0.5;
		y2 = (0.5 - (y1-.5)/(z1-2.5));
		x1 = (x-.5)*cos(0.0) + 
			(z-.5)*sin(0.0) + 0.5;
		x2 = (0.5 - (x1-.5)/(z1-2.5));

		xx = m_pDoc->m_aGroupColours[m_comparecentroid].red;
		yy = m_pDoc->m_aGroupColours[m_comparecentroid].green;
		zz = 1;
		xx1 = xx;
		yy1 = yy;
		zz1 = zz;
		zz1 = (zz-.5)*cos(0.0) + 
			(.5-xx)*sin(0.0) + 0.5;
		yy2 = (0.5 - (yy1-.5)/(zz1-2.5));
		xx1 = (xx-.5)*cos(0.0) + 
			(zz-.5)*sin(0.0) + 0.5;
		xx2 = (0.5 - (xx1-.5)/(zz1-2.5));

		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_LINES);
			glVertex2d(x2*m_dist,y2*m_dist);
			glVertex2d(xx2*m_dist,yy2*m_dist);
			
		glEnd();
	}else if (m_hint == centroidcompare) {
		x = m_pDoc->m_aGroupColours[m_currentcentroid].red;
		y = m_pDoc->m_aGroupColours[m_currentcentroid].green;
		z = 1;
		x1 = x;
		y1 = y;
		z1 = z;
		z1 = (z-.5)*cos(0.0) + 
			(.5-x)*sin(0.0) + 0.5;
		y2 = (0.5 - (y1-.5)/(z1-2.5));
		x1 = (x-.5)*cos(0.0) + 
			(z-.5)*sin(0.0) + 0.5;
		x2 = (0.5 - (x1-.5)/(z1-2.5));

		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_LINES);
			glVertex2d(x2*m_dist,y2*m_dist);
			if(m_printimage)
				glVertex2d((double)(m_comparepoint.x - 360) ,
					(double)(m_Rect.bottom - m_comparepoint.y));
			else
				glVertex2d((double)(m_comparepoint.x - 120) ,
					(double)(m_Rect.bottom - m_comparepoint.y));
		glEnd();
	}

	//draw printing/saving region identifier (black square)
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_LINE_LOOP);
	glVertex2d(m_dist/100.0,m_Rect.bottom/2 - m_dist/2 +m_dist/100.0);
	glVertex2d(m_dist/100.0,m_Rect.bottom/2 + m_dist/2 -m_dist/100.0);
	glVertex2d(m_dist-m_dist/100.0,m_Rect.bottom/2 + m_dist/2 - m_dist/100.0);
	glVertex2d(m_dist-m_dist/100.0,m_Rect.bottom/2 - m_dist/2 + m_dist/100.0);
	glEnd();
}

void CScat::DrawMst(void)
{
	long i, j;
	glColor3f(0.0, 0.0, 0.0);  // or do we want the mst in a different colour from the axes?
	for(i=0;i<m_pDoc->m_aMst.GetSize();i++){
		for (j = 0; j < m_pDoc->m_aMst.GetSize(); j++) {
			if (m_pDoc->m_aMst.GetAt(i)->GetAt(j) == 1) {
				glBegin(GL_LINES);
					glVertex3f(m_pDoc->m_aObjectColours[i].red, m_pDoc->m_aObjectColours[i].green,
						1);
					glVertex3f(m_pDoc->m_aObjectColours[j].red, m_pDoc->m_aObjectColours[j].green,
						1);
				glEnd();
			}
		}
	}
}


void CScat::OnTimer(UINT nIDEvent) 
{
	KillTimer(1);
	CView::OnTimer(nIDEvent);
	RedrawWindow();
	SetTimer(1,50,NULL);
}


void CScat::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	m_local = point;
	ScreenToClient(&m_local);

	CMenu* pPopup = m_menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y,
		AfxGetMainWnd());
}


void CScat::OnAddtextbox() 
{
	TextBox * newtextbox = new TextBox;
	CEditText edittext;

	edittext.m_Fontsize = 15;

	if(IDOK == edittext.DoModal()){
		newtextbox->contents = edittext.m_text;
		newtextbox->size = edittext.m_Fontsize;
		long newlength = edittext.m_text.GetLength() * edittext.m_Fontsize;
		long newheight = edittext.m_Fontsize;
		newtextbox->location.top = m_local.y + newheight;
		newtextbox->location.left = m_local.x - 120;
		newtextbox->location.bottom = m_local.y;
		newtextbox->location.right = m_local.x + newlength;
								
		m_TextBoxes.Add(newtextbox);
	}else{
		delete newtextbox;
	}
	RedrawWindow();
}


void CScat::OnDisplaycentroids() 
{
	if(m_Centroid){
		m_Centroid = false;
		m_menu.CheckMenuItem(ID__DISPLAYCENTROIDS,MF_UNCHECKED);
	}else{
		m_Centroid = true;	
		m_menu.CheckMenuItem(ID__DISPLAYCENTROIDS,MF_CHECKED);
		if(m_Grouping){
			m_Grouping = false;
			m_menu.CheckMenuItem(ID__SHOWGROUPING,MF_UNCHECKED);
		}
	}
	m_currenttextbox = -1;
	m_currentobject = m_currentcentroid = -1;
	m_compareobject = m_comparecentroid = -1;
	m_hint = none;

	RedrawWindow();
}


void CScat::OnDisplaylabels() 
{
	if(m_Labels){
		m_Labels = false;
		m_menu.CheckMenuItem(ID__DISPLAYLABELS,MF_UNCHECKED);
	}else{
		m_Labels = true;
		m_menu.CheckMenuItem(ID__DISPLAYLABELS,MF_CHECKED);
	}
	m_currenttextbox = -1;
	m_currentobject = m_currentcentroid = -1;
	m_compareobject = m_comparecentroid = -1;
	m_hint = none;
	RedrawWindow();
}


void CScat::OnDisplaymst() 
{
	if(m_Mst){
		m_Mst = false;
		m_menu.CheckMenuItem(ID__DISPLAYMST,MF_UNCHECKED);
	}else{
		m_Mst = true;
		m_menu.CheckMenuItem(ID__DISPLAYMST,MF_CHECKED);
	}
	RedrawWindow();
}


void CScat::OnShowaxies() 
{
	if(m_Axies){
		m_Axies = false;
		m_menu.CheckMenuItem(ID__SHOWAXIES,MF_UNCHECKED);
	}else{
		m_Axies = true;
		m_menu.CheckMenuItem(ID__SHOWAXIES,MF_CHECKED);
	}
	RedrawWindow();
}


void CScat::OnShowgrouping() 
{
	if(m_Grouping){
		m_Grouping = false;
		m_menu.CheckMenuItem(ID__SHOWGROUPING,MF_UNCHECKED);
	}else{
		m_Grouping = true;
		m_Usergroupcolours = false;
		m_menu.CheckMenuItem(ID__SHOWGROUPING,MF_CHECKED);
		m_menu.CheckMenuItem(ID__USEUSERGROUPCOLOURS,MF_UNCHECKED);
	}
	RedrawWindow();
}


void CScat::OnShowselectedonly() 
{
	if(m_SelOnly){
		m_SelOnly = false;
		m_menu.CheckMenuItem(ID__SHOWSELECTEDONLY,MF_UNCHECKED);
	}else{
		m_SelOnly = true;
		m_menu.CheckMenuItem(ID__SHOWSELECTEDONLY,MF_CHECKED);
	}
	m_currenttextbox = -1;
	m_currentobject = m_currentcentroid = -1;
	m_compareobject = m_comparecentroid = -1;
	m_hint = none;

	RedrawWindow();
}


CScat::hit CScat::HitTest(CPoint point)
{
	long i;
	double x, z;

	//check if over info bar region (none)
	if(point.x < 120) return h_none;

	//check if over txt box
	long charsize;
	for(i=0;i<m_TextBoxes.GetSize();i++){
		charsize = m_TextBoxes[i]->size;
		if(point.x-120 <= m_TextBoxes[i]->location.right-charsize/2 &&
			point.x-120 >= m_TextBoxes[i]->location.left-charsize/2 &&
			point.y >= m_TextBoxes[i]->location.bottom-charsize/2 &&
			point.y <= m_TextBoxes[i]->location.top-charsize/2){
			m_currenttextbox = i;
			m_currentobject = m_currentcentroid = -1;
			m_compareobject = m_comparecentroid = -1;
			//RedrawWindow();
			return h_textbox;
		}
	}

	long width, height;

	width = m_Rect.right-120;
	height = m_Rect.bottom;
	double radius;
	radius = 0.018;
	double y,x1,y1,z1,x2,y2;
	//,z2;

	if(!m_Centroid){
	
		for(i=0;i<m_pDoc->m_aObjectColours.GetSize();i++){
			if(!m_SelOnly || m_pDoc->m_RowSelection->IsSelected(i)){
				x = m_pDoc->m_aObjectColours[i].red;
				y = m_pDoc->m_aObjectColours[i].green;
				z = 1;
				x1 = x;
				y1 = y;
				z1 = z;
				z1 = (z-.5)*cos(0.0) + 
					(.5-x)*sin(0.0) + 0.5;
				y2 = (0.5 - (y1-.5)/(z1-2.5));
				x1 = (x-.5)*cos(0.0) + 
					(z-.5)*sin(0.0) + 0.5;
				x2 = (0.5 - (x1-.5)/(z1-2.5));

				if(((point.x-120)/(double)(m_dist)) <= x2+radius &&
					((point.x-120)/(double)(m_dist)) >= x2-radius &&
					(1-(point.y)/(double)m_dist) <= y2+radius &&
					(1-(point.y)/(double)m_dist) >= y2-radius){
					m_currenttextbox = m_currentcentroid = -1;
					m_compareobject = m_comparecentroid = -1;
					m_currentobject = i;  
					return h_object;
				}
			}
		}
	}else{
		for(i=0;i<m_pDoc->m_aGroupColours.GetSize();i++){
			x = m_pDoc->m_aGroupColours[i].red;
			y = m_pDoc->m_aGroupColours[i].green;
			z = 1;
			x1 = x;
			y1 = y;
			z1 = z;
			z1 = (z-.5)*cos(0.0) + 
				(.5-x)*sin(0.0) + 0.5;
			y2 = (0.5 - (y1-.5)/(z1-2.5));
			x1 = (x-.5)*cos(0.0) + 
				(z-.5)*sin(0.0) + 0.5;
			x2 = (0.5 - (x1-.5)/(z1-2.5));

			if(((point.x-120)/(double)(m_dist)) <= x2+radius &&
				((point.x-120)/(double)(m_dist)) >= x2-radius &&
				(1-(point.y)/(double)m_dist) <= y2+radius &&
				(1-(point.y)/(double)m_dist) >= y2-radius){
				m_currenttextbox = m_currentobject = -1;
				m_compareobject = m_comparecentroid = -1;
				m_currentcentroid = i;
				return h_centroid;
			}
		}
	}

	double t_x,t_y;
	if(true){
		t_x = (point.x - 120) / (double) m_dist;
		t_y = 1-((point.y) / (double) m_dist);

		long minidxx = 0;
		long minidxy = i = 0;
		x = m_pDoc->m_aGroupColours[i].red;
		y = m_pDoc->m_aGroupColours[i].green;
		z = 1;
		x1 = x;
		y1 = y;
		z1 = z;
		z1 = (z-.5)*cos(0.0) + 
			(.5-x)*sin(0.0) + 0.5;
		y2 = (0.5 - (y1-.5)/(z1-2.5));
		x1 = (x-.5)*cos(0.0) + 
			(z-.5)*sin(0.0) + 0.5;
		x2 = (0.5 - (x1-.5)/(z1-2.5));

		double miny = y2;
		double minx = x2;

		for (i = 1; i<m_pDoc->m_aGroupColours.GetSize(); i++) {
			x = m_pDoc->m_aGroupColours[i].red;
			y = m_pDoc->m_aGroupColours[i].green;
			z = 1;
			x1 = x;
			y1 = y;
			z1 = z;
			z1 = (z-.5)*cos(0.0) + 
				(.5-x)*sin(0.0) + 0.5;
			y2 = (0.5 - (y1-.5)/(z1-2.5));
			x1 = (x-.5)*cos(0.0) + 
				(z-.5)*sin(0.0) + 0.5;
			x2 = (0.5 - (x1-.5)/(z1-2.5));

			if ((t_x-x2)*(t_x-x2) < (t_x-minx)*(t_x-minx)){
				minx = x2;
				minidxx = i;
			}
			if ((t_y-y2)*(t_y-y2) < (t_y-miny)*(t_y-miny)){
				miny = y2;
				minidxy = i;
			}
		}
		m_currenttextbox = m_currentobject = -1;
		m_compareobject = m_comparecentroid = -1;
		if (!(minidxy == minidxx)) {
			x = m_pDoc->m_aGroupColours[minidxx].red;
			y = m_pDoc->m_aGroupColours[minidxx].green;
			z = 1;
			x1 = x;
			y1 = y;
			z1 = z;
			z1 = (z-.5)*cos(0.0) + 
				(.5-x)*sin(0.0) + 0.5;
			y2 = (0.5 - (y1-.5)/(z1-2.5));
	
			double temp1 = (t_x-minx)*(t_x-minx) + (t_y-y2)*(t_y-y2);   // using group minx

			x = m_pDoc->m_aGroupColours[minidxy].red;
			y = m_pDoc->m_aGroupColours[minidxy].green;
			z = 1;
			x1 = x;
			y1 = y;
			z1 = z;
			z1 = (z-.5)*cos(0.0) + 
				(.5-x)*sin(0.0) + 0.5;
			x1 = (x-.5)*cos(0.0) + 
				(z-.5)*sin(0.0) + 0.5;
			x2 = (0.5 - (x1-.5)/(z1-2.5));
			double temp2 = (t_x-x2)*(t_x-x2) + (t_y-miny)*(t_y-miny);   // using group miny
			
			if(temp1 < temp2){
				m_currentcentroid = minidxx;
				return h_centroid;
			}else{
				m_currentcentroid = minidxy;
				return h_centroid;
			}
		} else { 
			m_currentcentroid = minidxx;
			return h_centroid;
		}
	}		
	//else space
	return h_space;
}


void CScat::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);


	double width, height;

	if(cy == 0)
		cy = 1;

	CRect rect;
    GetClientRect(&rect);

    glViewport(0, 0, rect.right, rect.bottom);

	m_Rect.left = 0;
	m_Rect.top = 0;
	m_Rect.right = cx;
	m_Rect.bottom = cy;
	width = m_Rect.right - 120;
	height = m_Rect.bottom;


}

void CScat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	POSITION pos;
	//perform hit test
	hit test = HitTest(point);
	switch(test){
	case h_object:
		//set info bar contents to object's info
		//set tableview selection to this object
		//change mhint to objectselection
		m_hint = objectselection;

		m_infobar.item = object;
		m_infobar.itemNr = 1;
		m_pDoc->GetRowLabel(m_currentobject,m_infobar.name1); 
		m_infobar.ordin1[0] = m_pDoc->m_aRowXCoords[m_currentobject];
		m_infobar.ordin1[1] = m_pDoc->m_aRowYCoords[m_currentobject];
		m_pDoc->m_ColSelection->RemoveAll();
		m_pDoc->m_RowSelection->RemoveAll();
		m_pDoc->m_RowSelection->SetAt(m_currentobject);
		pos = m_pDoc->GetFirstViewPosition();   
		while (pos != NULL)   
		{
			CView* pView = m_pDoc->GetNextView(pos);   
			if(pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)))
				pView->RedrawWindow();
		}
		break;
	case h_centroid:
		//set info bar contents to centroid's info
		//set tableview selection to objects belonging to 
		//	this group
		//change mhint to centroidselection
		m_hint = centroidselection;

		m_infobar.item = group;
		m_infobar.itemNr = 1;
		m_infobar.name1.Format("%d",m_pDoc->m_Groups.GetAt(m_currentcentroid)); 
		m_infobar.ordin1[0] = m_pDoc->m_aGroupColours[m_currentcentroid].red*(m_pDoc->m_OrdXmax-m_pDoc->m_OrdXmin)+m_pDoc->m_OrdXmin;
		m_infobar.ordin1[1] = m_pDoc->m_aGroupColours[m_currentcentroid].green*(m_pDoc->m_OrdYmax-m_pDoc->m_OrdYmin)+m_pDoc->m_OrdYmin;
		m_infobar.group1.SetAt(0,m_pDoc->m_Gstastats.GetAt(m_currentcentroid)->GetAt(m_Gstaattrib)->minimum);
		m_infobar.group1.SetAt(1,m_pDoc->m_Gstastats.GetAt(m_currentcentroid)->GetAt(m_Gstaattrib)->first_quartile);
		m_infobar.group1.SetAt(2,m_pDoc->m_Gstastats.GetAt(m_currentcentroid)->GetAt(m_Gstaattrib)->median);
		m_infobar.group1.SetAt(3,m_pDoc->m_Gstastats.GetAt(m_currentcentroid)->GetAt(m_Gstaattrib)->mean);
		m_infobar.group1.SetAt(4,m_pDoc->m_Gstastats.GetAt(m_currentcentroid)->GetAt(m_Gstaattrib)->third_quartile);
		m_infobar.group1.SetAt(5,m_pDoc->m_Gstastats.GetAt(m_currentcentroid)->GetAt(m_Gstaattrib)->maximum);
		m_pDoc->GetColumnLabel(m_Gstaattrib,m_infobar.attrib);

		m_pDoc->m_ColSelection->RemoveAll();
		m_pDoc->m_RowSelection->RemoveAll();
		pos = m_pDoc->GetFirstViewPosition();   
		while (pos != NULL)   
		{
			CView* pView = m_pDoc->GetNextView(pos);   
			if(pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)))
				pView->RedrawWindow();
		}
		break;
	case h_space:
		//find closest group and same as centroid
		m_hint = spaceselection;

		m_pDoc->m_ColSelection->RemoveAll();
		m_pDoc->m_RowSelection->RemoveAll();
		pos = m_pDoc->GetFirstViewPosition();   
		while (pos != NULL)   
		{
			CView* pView = m_pDoc->GetNextView(pos);   
			if(pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)))
				pView->RedrawWindow();
		}
		break;
	case h_textbox:
		//change mhint to txtselection
		m_hint = textboxselection;
		break;
	case h_none:
		//do nothing
	default:
		break;
	}
	RedrawWindow();
	CView::OnLButtonDown(nFlags, point);
}

void CScat::OnLButtonUp(UINT nFlags, CPoint point) 
{
	POSITION pos;
	CEditText edittext;
	//do mhint check
	switch(m_hint){
	case objectselection:
	case centroidselection:
	case spaceselection:
		//clear m_hint
		m_hint = none;
		break;
	case objectcompare:
		//hit test on location for comparison info(done in mousemove)
		//clear m_hint
		m_hint = none;

		if(m_compareobject != -1){
			m_infobar.item = object;
			m_infobar.itemNr = 2;
			m_pDoc->GetRowLabel(m_compareobject,m_infobar.name2);
			m_infobar.ordin2[0] = m_pDoc->m_aRowXCoords[m_compareobject];
			m_infobar.ordin2[1] = m_pDoc->m_aRowYCoords[m_compareobject];
			if(m_compareobject < m_currentobject)
				m_infobar.assoc = 
					m_pDoc->m_AssocMatrixRows.m_aMatrix.GetAt(m_currentobject-1)->GetAt(m_compareobject);
			else
				m_infobar.assoc = 
					m_pDoc->m_AssocMatrixRows.m_aMatrix.GetAt(m_compareobject-1)->GetAt(m_currentobject);
			m_pDoc->m_RowSelection->SetAt(m_compareobject);
			pos = m_pDoc->GetFirstViewPosition();   
			while (pos != NULL)   
			{
				CView* pView = m_pDoc->GetNextView(pos);   
				if(pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)))
					pView->RedrawWindow();
			}
		}
		break;
	case centroidcompare:
		//hit test on location for comparison info(done in mousemove)
		//clear m_hint
		m_hint = none;

		if(m_comparecentroid != -1){
			m_infobar.item = group;
			m_infobar.itemNr = 2;
			m_infobar.name2.Format("%d",m_pDoc->m_Groups.GetAt(m_comparecentroid)); 
			m_infobar.ordin2[0] = m_pDoc->m_aGroupColours[m_comparecentroid].red*(m_pDoc->m_OrdXmax-m_pDoc->m_OrdXmin)+m_pDoc->m_OrdXmin;
			m_infobar.ordin2[1] = m_pDoc->m_aGroupColours[m_comparecentroid].green*(m_pDoc->m_OrdYmax-m_pDoc->m_OrdYmin)+m_pDoc->m_OrdYmin;
			m_infobar.group2.SetAt(0,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->minimum);
			m_infobar.group2.SetAt(1,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->first_quartile);
			m_infobar.group2.SetAt(2,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->median);
			m_infobar.group2.SetAt(3,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->mean);
			m_infobar.group2.SetAt(4,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->third_quartile);
			m_infobar.group2.SetAt(5,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->maximum);

			//calculate KWcomparison here...
			CKruskalWallace kw;
			kw.Create2(m_pDoc,&m_KWOrder,&m_KWValues,m_comparecentroid,m_currentcentroid);
		}
		break;
	case textboxmove:
		//update txtbox location + more?
		//clear m_hint
		m_hint = none;
		break;
	case textboxselection:
		//activate text box edit facility
		edittext.m_text = m_TextBoxes[m_currenttextbox]->contents;
		edittext.m_Fontsize = m_TextBoxes[m_currenttextbox]->size;
		if(IDOK == edittext.DoModal()){
			m_TextBoxes[m_currenttextbox]->location.right = m_local.x 
				+ edittext.m_text.GetLength() * 10; //Width of 1 char

			m_TextBoxes[m_currenttextbox]->contents = edittext.m_text;
			m_TextBoxes[m_currenttextbox]->size = edittext.m_Fontsize;

		}else{
			//delete it
			delete m_TextBoxes[m_currenttextbox];
			m_TextBoxes.RemoveAt(m_currenttextbox);
		}
		m_hint = none;

		break;
	default:
		break;
	}
	RedrawWindow();
	CView::OnLButtonUp(nFlags, point);
}

void CScat::OnMouseMove(UINT nFlags, CPoint point) 
{
	long newlength;
	long newheight;
	long charlength;
	long oldobject = m_currentobject;
	long oldcentroid = m_currentcentroid;

	hit test;

	//perform mhint check
	switch(m_hint){
	case objectselection:
	case objectcompare:
		//line to point for object comparison
		m_comparepoint.x = point.x;
		m_comparepoint.y = point.y;
	
		
		m_hint = objectcompare;
		
		//m_hint = objectcompare;
		//do hit test.
		test = HitTest(point);
		switch(test){
		case h_object:
			//update info bar with object & comparison data
			if(oldobject != m_currentobject){
				m_compareobject = m_currentobject;
			}else{
				m_currentcentroid = -1;
				m_currenttextbox = -1;
				m_compareobject = -1;
				m_comparecentroid = -1;
			}
			break;
		default:
			m_currentcentroid = -1;
			m_currenttextbox = -1;
			m_compareobject = -1;
			m_comparecentroid = -1;
			break;
		}
		m_currentobject = oldobject;

		//set mhint dragging
		
		RedrawWindow();
		break;
	case centroidselection:
	case centroidcompare:
		//draw line to point for object comparison
		m_comparepoint.x = point.x;
		m_comparepoint.y = point.y;
	
		
		//m_hint = objectcompare;
		//do hit test.
		test = HitTest(point);
		switch(test){
		case h_centroid:
			//update info bar with object & comparison data
			if(oldcentroid != m_currentcentroid){
				m_comparecentroid = m_currentcentroid;
			}else{
				m_currentobject = -1;
				m_currenttextbox = -1;
				m_compareobject = -1;
				m_comparecentroid = -1;
			}
			break;
		default:
			m_currentobject = -1;
			m_currenttextbox = -1;
			m_compareobject = -1;
			m_comparecentroid = -1;
			break;
		}
		
		
		m_hint = centroidcompare;

		m_currentcentroid = oldcentroid;
		RedrawWindow();
		break;
	case none:
		break;
	case textboxselection:
	case textboxmove:
		m_hint = textboxmove;
		//update selected text box location
		charlength = m_TextBoxes[m_currenttextbox]->size;
		newlength = m_TextBoxes[m_currenttextbox]->contents.GetLength() * charlength;
		newheight = m_TextBoxes[m_currenttextbox]->size;
		if(point.x > 120 || point.x < m_Rect.bottom+120 ||
			point.y > newheight){
			if(point.x > 120 && point.x < m_Rect.bottom+120){
				m_TextBoxes[m_currenttextbox]->location.left = point.x - 120;
				m_TextBoxes[m_currenttextbox]->location.right = point.x - 120 + newlength;
			}else if(point.x > 120){
				m_TextBoxes[m_currenttextbox]->location.left = m_Rect.bottom;
				m_TextBoxes[m_currenttextbox]->location.right = m_Rect.bottom + newlength;
			}else if(point.x < m_Rect.bottom+120){
				m_TextBoxes[m_currenttextbox]->location.left = 0;
				m_TextBoxes[m_currenttextbox]->location.right = newlength;
			}
			if(point.y > newheight){
				m_TextBoxes[m_currenttextbox]->location.top = point.y + newheight;
				m_TextBoxes[m_currenttextbox]->location.bottom = point.y;
			}
			RedrawWindow();
		}

		break;
	default:
		break; 
	}


	CView::OnMouseMove(nFlags, point);
}

BOOL CScat::DestroyWindow() 
{
	::ReleaseDC(m_hWnd,m_hDC);
	
	return CView::DestroyWindow();
}

void CScat::OnInitialUpdate() 
{
	long nPixelFormat;		// Pixel Format

	CView::OnInitialUpdate();

	// Select the pixel format we would "like"
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,								// Version of this structure	
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,				// Double buffered mode
		PFD_TYPE_RGBA,					// RGBA Color mode
		16,								// Want 24bit color 
		0,0,0,0,0,0,					// Not used to select mode
		0,0,							// Not used to select mode
		0,0,0,0,0,						// Not used to select mode
		0,								// Size of depth buffer
		0,								// Not used to select mode
		0,								// Not used to select mode
		PFD_OVERLAY_PLANE
,					// Draw in main plane
		0,								// Not used to select mode
		0,0,0 };						// Not used to select mode

	m_hDC = ::GetDC(m_hWnd);
	
	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);

	
	// If we can't select a pixel format, all is lost
	if(nPixelFormat == 0)
		{
		MessageBox("Could not select a compatible pixel format!",NULL,MB_OK | MB_ICONSTOP);
		return;
		}

	// Set the pixel format for the device context
	if(!SetPixelFormat(m_hDC, nPixelFormat, &pfd))
		{
		MessageBox("Could not set a valid pixel format!",NULL,MB_OK | MB_ICONSTOP);
		return;
		}

	// Create the palette if needed
	InitializePalette(m_hDC);

	// Create the rendering context
	m_hRC = wglCreateContext(m_hDC);

	if(m_hRC == NULL)
		{
		MessageBox("Could not create rendering context!",NULL,MB_OK | MB_ICONSTOP);
		return;
		}

	// Setup and initialize
	wglMakeCurrent(m_hDC,m_hRC);
	SetupRC(m_hDC);

	// Deselect the rendering context	
	wglMakeCurrent(NULL,NULL);	
}

void CScat::SetupRC(HDC hDC)
{
	GLfloat  whiteLight[] = { 0.45f, 0.45f, 0.45f, 1.0f };
	GLfloat  sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
	GLfloat	 lightPos[] = { -50.f, 25.0f, 250.0f, 0.0f };

	glEnable(GL_DEPTH_TEST);	// Hidden surface removal

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Setup and enable light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,whiteLight);
	glLightfv(GL_LIGHT0,GL_AMBIENT,sourceLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,sourceLight);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	
	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(0.1f);
}

// Initializes the CPalette object
void CScat::InitializePalette(HDC hDC)
	{
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE *pPal;			// Pointer to memory for logical palette
	long nPixelFormat;			// Pixel format index
	long nColors;				// Number of entries in palette
	long i;						// Counting variable
	BYTE RedRange,GreenRange,BlueRange;
									// Range for each color entry (7,7,and 3)

	// Get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(hDC);
	DescribePixelFormat(hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if(!(pfd.dwFlags & PFD_NEED_PALETTE))
		return;

	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;	

	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +nColors*sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

	// Build mask of all 1's.  This creates a number represented by having
	// the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
	// pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) -1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) -1;

	// Loop through all the palette entries
	for(i = 0; i < nColors; i++)
		{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double) pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char) NULL;
		}

		
	// Create the palette
	m_GLPalette.CreatePalette(pPal);

	// Go ahead and select and realize the palette for this device context
	SelectPalette(hDC,(HPALETTE)m_GLPalette,FALSE);
	RealizePalette(hDC);

	// Free the memory used for the logical palette structure
	free(pPal);
}

BOOL CScat::OnQueryNewPalette() 
{
	// If the palette was created.
	if((HPALETTE)m_GLPalette)
		{
		long nRet;

		// Selects the palette into the current device context
		VERIFY(SelectPalette(m_hDC, (HPALETTE)m_GLPalette, FALSE));

		// Map entries from the currently selected palette to
		// the system palette.  The return value is the number 
		// of palette entries modified.
		nRet = RealizePalette(m_hDC);

		// Repaint, forces remap of palette in current window
		InvalidateRect(NULL,FALSE);

		return nRet;
		}

	return CView::OnQueryNewPalette();
	}


void CScat::OnPaletteChanged(CWnd* pFocusWnd) 
{
	if(((HPALETTE)m_GLPalette != NULL))// && (pFocusWnd != this))
		{
		// Select the palette into the device context
		VERIFY(SelectPalette(m_hDC,(HPALETTE)m_GLPalette,FALSE));

		// Map entries to system palette
		VERIFY(RealizePalette(m_hDC) != GDI_ERROR);
				
		// Remap the current colors to the newly realized palette
		UpdateColors(m_hDC);

		return;
		}

	CView::OnPaletteChanged(pFocusWnd);
}

BOOL CScat::PreCreateWindow(CREATESTRUCT& cs) 
{
	CView::PreCreateWindow(cs);

	// Styles required by OpenGL
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC | CS_PARENTDC | CS_SAVEBITS | CS_CLASSDC,
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);

	cs.cy = 1024;
	cs.cx = 960;

	//return CView::PreCreateWindow(cs);
	return true;
}

BOOL CScat::OnEraseBkgnd(CDC* pDC) 
{
	if(m_hRC != NULL)
		// Do not take the time to erase the background between renderings
		return FALSE;

	return CView::OnEraseBkgnd(pDC);
}

void CScat::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	Invalidate(FALSE); // Don't need to erase, double buffered

}

void CScat::OnUseusergroupcolours() 
{
	if(m_Usergroupcolours){
		m_Usergroupcolours = false;
		m_menu.CheckMenuItem(ID__USEUSERGROUPCOLOURS,MF_UNCHECKED);
	}else{
		m_Usergroupcolours = true;
		m_Grouping = false;
		m_menu.CheckMenuItem(ID__USEUSERGROUPCOLOURS,MF_CHECKED);
		m_menu.CheckMenuItem(ID__SHOWGROUPING,MF_UNCHECKED);
	}
	RedrawWindow();	
}


void CScat::OnSaveasimage() 
{
	m_saveimage = true;
	m_SaveHeight = m_Rect.bottom;
	m_SaveWidth = m_Rect.right;
	RedrawWindow();
	m_saveimage = false;
	RedrawWindow();

	
}

void CScat::SaveIt()
{
	m_SaveHeight = m_Rect.bottom;
	m_SaveWidth = m_Rect.right;

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;

	strTitle = "Save 3D Graph:";
	
	// Get path for the new file
	pFileDlg = new CFileDialog(FALSE,"bmp","*.bmp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Bitmap (*.bmp)|*.bmp|All files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
	delete pFileDlg;

	//Create device context
	HDC dc = CreateCompatibleDC(NULL);
	BITMAPINFO info;
	PBYTE bits;

	long nWidth,nObjects;
	if(m_Centroid || m_Usergroupcolours || m_Grouping)
		nObjects = m_pDoc->m_aGroupColours.GetSize();
	else
		nObjects = m_pDoc->m_aObjectColours.GetSize();
	long nRows = (long)floor(480/20.0)-1;
	long nCols = (long)ceil(nObjects/(double)nRows);
	if(m_Legend)
		nWidth = 600 + nCols*100;
	else
		nWidth = 600;


	// create and bind bitmap
	memset(&info, 0, sizeof(info));
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = nWidth;
	info.bmiHeader.biHeight = 480;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;

	HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, (void**)&bits, NULL, 0);
	PIXELFORMATDESCRIPTOR pfd;
	SelectObject(dc, bitmap);

	//set pixel format
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cDepthBits = 16;

	long pf = ChoosePixelFormat(dc, &pfd);
	SetPixelFormat(dc, pf, &pfd);

	//create and bind the rendering context
	HGLRC rc = wglCreateContext(dc);
	wglMakeCurrent(dc, rc);

	m_ccfont.FontDestroy(m_Font);
	//m_Font = m_ccfont.FontCreate(wglGetCurrentDC(), m_Fonttype, 15, 0, m_italic,0);
	m_Font = m_ccfont.FontCreate(wglGetCurrentDC(), m_pDoc->m_lfCells);

	glClearDepth(1.0f);

	//DrawScreen and save to bitmap
	m_Rect.bottom = 480;
	m_Rect.right = nWidth;

	m_saveimage = true;
	DrawScene();
	m_saveimage = false;

	glFinish();

	SaveDIBitmap(strPathName, &info, bits);

	//Destroy all memory device storage
	wglDeleteContext(rc);
	DeleteObject(bitmap);
	DeleteDC(dc);

	m_ccfont.FontDestroy(m_Font);
	m_Font = NULL;
	
	CRect rect;
	GetWindowRect(rect);
	m_Rect.left = 0;
	m_Rect.top = 0;
	m_Rect.right = rect.Width();
	m_Rect.bottom = rect.Height();
	RedrawWindow();
}

BOOL CScat::OnPreparePrinting(CPrintInfo* pInfo) 
{
	if(m_psd != NULL) delete m_psd;	m_psd = NULL;
	CPageSetupDialog *d = new CPageSetupDialog(PSD_DISABLEORIENTATION);
//	if(d->DoModal() == IDCANCEL){delete d; return false;}
	m_psd = d;

	long ret = AfxMessageBox("Print information space?",MB_YESNO);
	if(ret == IDYES) m_showinfo = true;
	else if(ret == IDNO) m_showinfo = false;
	else return false;
	
	m_currentpage = 0;
	m_dc = NULL;
	m_bitmap = NULL;


	return DoPreparePrinting(pInfo);
}

void CScat::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	m_SaveHeight = m_Rect.bottom;
	m_SaveWidth = m_Rect.right;
			
	//3* the res of a saved image.  Greatly improves font.
	if(pInfo->m_nCurPage == 1){
		m_Rect.bottom = 1440;
		m_Rect.right = 1800;
	}else{
		m_Rect.right = 1800;
		m_Rect.bottom = 1800 * m_rectDraw.Height() / m_rectDraw.Width();
	}

	if(m_currentpage != (signed long) pInfo->m_nCurPage){
		//create page
		m_currentpage = pInfo->m_nCurPage;

		//Create device context
		if(m_dc != NULL) DeleteDC(m_dc);
		m_dc = CreateCompatibleDC(NULL);
		
		// create and bind bitmap
		memset(&m_info, 0, sizeof(m_info));
		m_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		
		m_info.bmiHeader.biWidth = m_Rect.right;
		m_info.bmiHeader.biHeight = m_Rect.bottom;
		m_info.bmiHeader.biPlanes = 1;
		m_info.bmiHeader.biBitCount = 24;
		m_info.bmiHeader.biCompression = BI_RGB;
		if(m_bitmap != NULL) DeleteObject(m_bitmap);
		m_bitmap = CreateDIBSection(m_dc, &m_info, DIB_RGB_COLORS, (void**)&m_bits, NULL, 0);
		SelectObject(m_dc, m_bitmap);
		//set pixel format
		memset(&m_pfd, 0, sizeof(m_pfd));
		m_pfd.nSize = sizeof(m_pfd);
		m_pfd.nVersion = 1;
		m_pfd.dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL;
		m_pfd.iPixelType = PFD_TYPE_RGBA;
		m_pfd.cColorBits = 24;
		m_pfd.cRedBits = 8;
		m_pfd.cGreenBits = 8;
		m_pfd.cBlueBits = 8;
		m_pfd.cDepthBits = 16;
		long pf = ChoosePixelFormat(m_dc, &m_pfd);
		SetPixelFormat(m_dc, pf, &m_pfd);
		//create and bind the rendering context
		HGLRC rc = wglCreateContext(m_dc);
		wglMakeCurrent(m_dc, rc);

		m_ccfont.FontDestroy(m_Font);
		//m_Font = m_ccfont.FontCreate(wglGetCurrentDC(), m_Fonttype, 40, 0, m_italic,0);
		m_Font = m_ccfont.FontCreate(wglGetCurrentDC(), m_pDoc->m_lfCells);

		glClearDepth(1.0f);
		long idx;
		if(pInfo->m_nCurPage == 1){
			m_saveimage = true;
			m_printimage = true;
			DrawScene();
			m_printimage = false;
			m_saveimage = false;
		}else{
			m_saveimage = true;
			m_printimage = true;
			InitObjSpace();
			if((idx = (long)((pInfo->m_nCurPage - 2)*((m_rectDraw.Width())/450/m_HScale*(((m_rectDraw.Height()-5)/60/m_VScale)-1)))) == 0){
				PrintPatternTable((long)((m_rectDraw.Height()-5)/60/m_VScale),(long)((m_rectDraw.Width())/450/m_HScale),0);
			}else{
				PrintPatternTable((long)((m_rectDraw.Height()-5)/60/m_VScale),(long)((m_rectDraw.Width())/450/m_HScale),idx-1);
			}
			m_printimage = false;
			m_saveimage = false;
		}
		glFinish();
		//end of unique page

		//oops, forgot this before... (~200k in size)
		wglDeleteContext (rc); 

		m_ccfont.FontDestroy(m_Font);
		m_Font = NULL;

	}

	if(pInfo->m_nCurPage != 1){
		StretchDIBits(pDC->GetSafeHdc(),m_rectDraw.left,
			m_rectDraw.top,m_rectDraw.Width(),
			m_rectDraw.Height(),0,0,
			m_Rect.right,m_Rect.bottom,
			m_bits,&m_info, DIB_RGB_COLORS,SRCCOPY);
	}else if(m_showinfo){
		StretchDIBits(pDC->GetSafeHdc(), m_rectDraw.left,
			m_rectDraw.top,m_rectDraw.Width(),
			(long)(m_rectDraw.Width()*4/5.0), 0, 0, 
			m_Rect.right, m_Rect.bottom,
			m_bits, &m_info, DIB_RGB_COLORS, SRCCOPY);   
	}else{
		StretchDIBits(pDC->GetSafeHdc(), m_rectDraw.left,
			m_rectDraw.top,m_rectDraw.Width(),
			m_rectDraw.Width(), 360, 0, 
			m_Rect.right-360, m_Rect.bottom,
			m_bits, &m_info, DIB_RGB_COLORS, SRCCOPY);   
	}
	
	//this may be needed, but printpreview appears to be 
	//fixed as only window available
	//wglDeleteContext(rc);
	
	m_Rect.bottom = (long)m_SaveHeight;
	m_Rect.right = (long)m_SaveWidth;
}

void CScat::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	delete m_psd;
	m_psd = NULL;

	if(m_dc != NULL) DeleteDC(m_dc);
	if(m_bitmap != NULL) DeleteObject(m_bitmap);

	CView::OnEndPrinting(pDC, pInfo);
}

void CScat::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
//	CRect rect,temp;
//	CPoint point;
//	m_psd->GetMargins(&rect,&temp);
//	point.x = m_psd->GetPaperSize().cx;
//	point.y = m_psd->GetPaperSize().cy;

	//////////////////////////////////////////////////////////////
	//device dependant info for regions & size & aspect retaining
	//////////////////////////////////////////////////////////////
	m_HRes = pDC->GetDeviceCaps(HORZRES);
	m_VRes = pDC->GetDeviceCaps(VERTRES);
	//below to determine xscale
	m_HPixelsPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
	m_HScale = m_HPixelsPerInch / 300.0;
	//below to determine yscale
	m_VPixelsPerInch = pDC->GetDeviceCaps(LOGPIXELSY);
	m_VScale = m_VPixelsPerInch / 300.0;
	
//	m_Hratio = point.x/((double)m_HRes);
//	m_Vratio = point.y/((double)m_VRes);

	//USED 300 'CAUSE THAT'S WHAT IT WAS WRITTEN ON
//	m_rectDraw.left = (rect.left)/((double) point.x)*m_HRes;
//	m_rectDraw.right = (point.x - rect.right)/((double) point.x)*m_HRes;
//	m_rectDraw.top = (rect.top)/((double) point.y)*m_VRes;
//	m_rectDraw.bottom = (point.y - rect.bottom)/((double) point.y)*m_VRes;
	m_rectDraw.top = m_rectDraw.left = 0;
	m_rectDraw.right = m_HRes;
	m_rectDraw.bottom = m_VRes;

	long nRows;
	if((nRows = m_pDoc->GetIdxTopExtrinsicRow()) == -1)
		nRows = m_pDoc->GetNoOfRows();

	long r = (long)((m_rectDraw.Height()-5)/60/m_VScale - 1);
	long c = (long)(m_rectDraw.Width()/450/m_HScale);
	long idx = nRows / r / c;
	pInfo->SetMaxPage(2 + idx);
}

void CScat::OnKillFocus(CWnd* pNewWnd) 
{
	CView::OnKillFocus(pNewWnd);
	
	if(m_Rotate) KillTimer(1);
	
}

void CScat::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	
	if(m_Rotate) SetTimer(1,50,NULL);
}

void CScat::OnShowranges() 
{
	if(m_Showranges){
		m_Showranges = false;
		m_menu.CheckMenuItem(ID__SHOWRANGES,MF_UNCHECKED);
	}else{
		m_Showranges = true;
		m_menu.CheckMenuItem(ID__SHOWRANGES,MF_CHECKED);
	}
	RedrawWindow();

}

void CScat::OnUseblackwhitesymbols() 
{
	if(m_Symbols){
		m_Symbols = false;
		m_menu.CheckMenuItem(ID__USEBLACKWHITESYMBOLS,MF_UNCHECKED);
	}else{
		m_Symbols = true;
		m_menu.CheckMenuItem(ID__USEBLACKWHITESYMBOLS,MF_CHECKED);
		if(m_Usergroupcolours){
			m_Usergroupcolours = false;
			m_menu.CheckMenuItem(ID__USEUSERGROUPCOLOURS,MF_UNCHECKED);
		}
	}	
	RedrawWindow();
	
}

void CScat::OnUpdateUseblackwhitesymbols(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_NoSymbols);
}

void CScat::OnSelectgroupstatisticsattribute() 
{
	CAttributeSelection dlg;
	dlg.m_pDoc = m_pDoc;
	dlg.m_attrib = m_Gstaattrib;
	dlg.DoModal();
	m_Gstaattrib = dlg.m_attrib;
	m_pDoc->GetColumnLabel(m_Gstaattrib,m_infobar.attrib);
	RedrawWindow();
}


void CScat::OnSelectpccvectors() 
{
	CPccVectorSelection dlg;
	dlg.m_pDoc = m_pDoc;
	dlg.m_PccVectors = &m_PccVectors;
	dlg.DoModal();
	RedrawWindow();
}

void CScat::OnUpdateDisplaycentroids(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_pDoc->RowGroupsEmpty());	
}

void CScat::OnUpdateShowgrouping(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_pDoc->RowGroupsEmpty() && !m_Centroid);	
}

void CScat::PrintPatternTable(long nRows,long nCols,long firstIdx)
{
	long width, height;

	nRows--;

	width = (long)(m_rectDraw.Width()/m_HScale);
	height = (long)(m_rectDraw.Height()/m_VScale);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CString FontType;
	CString text;

	glColor3f(0.0, 0.0, 0.0);
	glPushMatrix();
	if(m_printimage){
		glScalef(3,3,1);
		height = height/3;
	}

	long i,j;
	long x,y,z;
	long idx = firstIdx-1;
	x = y = z = 0;
	CString label;
	
	glColor3d(0,0,0);

	glLineWidth(3.0);
	glTranslated(5,-5,0);

	for(i=0;i<nCols;i++){
		//y = m_Rect.bottom-5;
		y = height-5;
		y -= 20;
		for(j=0;j<nRows;j++){
			//draw object & label
			idx++;
			glColor3d(0,0,0);
			if((m_Grouping || m_Centroid || m_Usergroupcolours) && idx < m_pDoc->m_Groups.GetSize() ){
				//draw for number of selected centroids
				if(m_Symbols){
					glPushMatrix();
					glTranslated(x-10,y,z);
					glRotatef(180,0,1,0);
					glScalef(1.5,1.5,1.5);
					m_tex.DrawPattern2D(m_pDoc->m_PatternGroups[idx]);
					glPopMatrix();
				}else{
					if(m_Usergroupcolours)
						glColor3d(m_pDoc->m_UserGroupColours[idx].red,
							m_pDoc->m_UserGroupColours[idx].green,
							m_pDoc->m_UserGroupColours[idx].blue);
					else
						glColor3d(m_pDoc->m_aGroupColours[idx].red,
							m_pDoc->m_aGroupColours[idx].green,
							m_pDoc->m_aGroupColours[idx].blue);
					glBegin(GL_QUADS);
					glVertex2d(x,y-5);
					glVertex2d(x,y+5);
					glVertex2d(x+10,y+5);
					glVertex2d(x+10,y-5);
					glEnd();
				}
				glColor3d(0,0,0);
				label.Format("Group %d",m_pDoc->m_Groups.GetAt(idx));
				glPushMatrix();
				glRasterPos3d(x+20,y,z);
				m_ccfont.FontPuts(m_Font,label);
				glPopMatrix();
				y -= 20;
			}else if(!(m_Grouping || m_Centroid || m_Usergroupcolours) && idx < m_pDoc->GetNoOfRows()){
				//draw for number of selected objects
				if(m_Symbols){
					glPushMatrix();
					glTranslated(x-10,y,z);
					glRotatef(180,0,1,0);
					glScalef(1.5,1.5,1.5);
					m_tex.DrawPattern2D(m_pDoc->m_PatternObjects[idx]);
					glPopMatrix();
				}else{
					glColor3d(m_pDoc->m_aObjectColours[idx].red,
						m_pDoc->m_aObjectColours[idx].green,
						m_pDoc->m_aObjectColours[idx].blue);
					glBegin(GL_QUADS);
					glVertex2d(x,y-5);
					glVertex2d(x,y+5);
					glVertex2d(x+10,y+5);
					glVertex2d(x+10,y-5);
					glEnd();
				}
				glColor3d(0,0,0);
				m_pDoc->GetRowLabel(idx,label);
				glPushMatrix();
				glRasterPos3d(x+20,y,z);
				m_ccfont.FontPuts(m_Font,label);
				glPopMatrix();
				y -= 20;
			}		
		}
		x += 150;	
	}
	glPopMatrix();	
	glLineWidth(1.0);
}

void CScat::OnMove(int x, int y) 
{
	if(y == 0)
		y = 1;

	CView::OnMove(x, y);
	
	m_Rect.left = 0;
	m_Rect.top = 0;
	m_Rect.right = x;
	m_Rect.bottom = y;
	
}


void CScat::OnUpdateKruskalWalliceOptions(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pDoc->m_KW.GetSize());	
}

void CScat::OnKruskalWalliceOptions()
{
	//CKWOptions kwo(m_pDoc);
	//kwo.DoModal();
}

void CScat::OnUpdateSelectpccvectors(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(false);	
	for(long i=0;i<m_pDoc->GetNoOfColumns();i++){
		if(m_pDoc->m_Vectors[i]->GetAt(0) != ANALYSIS_VALUE){
			pCmdUI->Enable(true);
			return;
		}
	}
	
}

void CScat::OnDisplayLegend()
{
	if(m_Legend){
		m_Legend = false;
		m_menu.CheckMenuItem(ID__DISPLAYLEGEND,MF_UNCHECKED);
	}else{
		m_Legend = true;
		m_menu.CheckMenuItem(ID__DISPLAYLEGEND,MF_CHECKED);
	}
	RedrawWindow();
}

void CScat::OnUpdateUseusergroupcolours(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_Symbols);	
}
