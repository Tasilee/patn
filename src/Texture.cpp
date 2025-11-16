// Texture.cpp: implementation of the CTexture class.
//
//////////////////////////////////////////////////////////////////////
/* textures.* is written to provide a quick & easy method of 
** loading & displaying textures.  Intended to be used in a 
** 3d or 2d environment.  Scaling & moving textures is up to 
** the calling function.  
**
** All textures draw from x=0to9, y=0to9, z=1 (only 3d);
** 
** Texture file format:
** Each individual pattern to be loaded begins with an 's' & ends with an 'e'
**		
**		s
**		...texture data...
**		e
**	
** texture data may contain any number of lines, triangles, squares 
** and circles.  They may be filled or unfilled.  Each individual 
** primitive is on a seperate line.
**
**		Lines:
**			...
**			0,x1,y1,x2,y2
**			...
**		where	point (x1,y1) is the first point (0to9)
**				point (x2,y2) is the second point (0to9)
** 
**		Triangles:
**			...
**			1,fill,x1,y1,x2,y2,x3,y3
**			...
**		where	'fill' is 0 or 1 (unfilled or filled)
**				point (x1,y1) is the first point (0to9)
**				point (x2,y2) is the second point (0to9)
**				point (x3,y3) is the third point (0to9)
**
**		Square:
**			...
**			2,fill,x1,y1,x2,y2,x3,y3,x4,y4
**			...
**		where	'fill' is 0 or 1 (unfilled or filled)
**				point (x1,y1) is the first point (0to9)
**				point (x2,y2) is the second point (0to9)
**				point (x3,y3) is the third point (0to9)
**				point (x4,y4) is the fourth point (0to9)
** 
**		Circle:
**			...
**			3,fill,x1,y1,radius
**			...
**		where	'fill' is 0 or 1 (unfilled or filled)
**				point (x1,y1) is the centre (0to9)
**				'radius' is the radius of the circle (0to9)
**
**
*/

#include "stdafx.h"
#include "patn.h"
#include "Texture.h"
//#include <gl\gl.h>
//#include <gl\glu.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexture::CTexture()
{
	m_TooManyObjects = false;
}

CTexture::~CTexture()
{
	for(int i=0;i<m_Patterns.GetSize();i++){
		for(int j=0;j<m_Patterns[i]->GetSize();j++)
			delete m_Patterns[i]->GetAt(j);
		delete m_Patterns[i];
	}

}

int CTexture::LoadPatterns(int,CString filename)
{
	CStdioFile io;
	CString strRow;
	Primitive * tempPrimitive;
	Pattern * tempPattern;
	int end;
	int error;
	int value,i;
/*
	if ( !io.Open(filename,CFile::modeRead) )
	{
		//AfxGetMainWnd()->MessageBox("Could not open file","Read external data file",
		//		MB_ICONSTOP | MB_OK );
		return 0;
	}
*/
	int pos = 0;
	int pos2 = filename.Find("\n",0);
	while ((strRow = filename.Mid(pos,pos2-1)).GetLength())//io.ReadString(strRow) )
	{
		pos = pos2+1; pos2 = filename.Find("\n",pos);

		//find start of next pattern
		if(strRow[0] == 's' || strRow[0] == 'S'){
			tempPattern = new Pattern;			
			//find primitives until end of pattern			
			while((end = (strRow = filename.Mid(pos,pos2-1)).GetLength()) && strRow[0] != 'e' && strRow[0] != 'E'){
				pos = pos2+1; pos2 = filename.Find("\n",pos);
				tempPrimitive = new Primitive;
				//determine type
				switch(strRow[0]){
				case '0': //line
					tempPrimitive->type = (Shape) 0;
					error = sscanf(strRow,"0,%d,%d,%d,%d",&tempPrimitive->data.line.x1,
						&tempPrimitive->data.line.y1,&tempPrimitive->data.line.x2,
						&tempPrimitive->data.line.y2);
					value = tempPrimitive->data.line.x1;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.line.x2;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.line.y1;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.line.y2;
					if(value > 11 || value < 0) error = 0;
					break;
				case '1': //triangle
					tempPrimitive->type = (Shape)1;
					error = sscanf(strRow,"1,%d,%d,%d,%d,%d,%d,%d",&tempPrimitive->data.triangle.fill,
						&tempPrimitive->data.triangle.x1,&tempPrimitive->data.triangle.y1,
						&tempPrimitive->data.triangle.x2,&tempPrimitive->data.triangle.y2,
						&tempPrimitive->data.triangle.x3,&tempPrimitive->data.triangle.y3);
					value = tempPrimitive->data.triangle.fill;
					if(value > 1 || value < 0) error = 0;
					value = tempPrimitive->data.triangle.x1;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.triangle.x2;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.triangle.x3;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.triangle.y1;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.triangle.y2;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.triangle.y3;
					if(value > 11 || value < 0) error = 0;
					break;
				case '2': //square
					tempPrimitive->type =(Shape) 2;
					error = sscanf(strRow,"2,%d,%d,%d,%d,%d,%d,%d,%d,%d",&tempPrimitive->data.square.fill,
						&tempPrimitive->data.square.x1,&tempPrimitive->data.square.y1,
						&tempPrimitive->data.square.x2,&tempPrimitive->data.square.y2,
						&tempPrimitive->data.square.x3,&tempPrimitive->data.square.y3,
						&tempPrimitive->data.square.x4,&tempPrimitive->data.square.y4);
					value = tempPrimitive->data.square.fill;
					if(value > 1 || value < 0) error = 0;
					value = tempPrimitive->data.square.x1;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.square.x2;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.square.x3;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.square.x4;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.square.y1;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.square.y2;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.square.y3;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.square.y4;
					if(value > 11 || value < 0) error = 0;
					break;
				case '3': //circle
					tempPrimitive->type = (Shape)3;
					error = sscanf(strRow,"3,%d,%d,%d,%d",&tempPrimitive->data.circle.fill,
						&tempPrimitive->data.triangle.x1,&tempPrimitive->data.triangle.y1,
						&tempPrimitive->data.circle.radius);
					value = tempPrimitive->data.circle.fill;
					if(value > 1 || value < 0) error = 0;
					value = tempPrimitive->data.circle.x1;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.circle.y1;
					if(value > 11 || value < 0) error = 0;
					value = tempPrimitive->data.circle.radius;
					if(value > 11 || value < 0) error = 0;
					break;
				}
				if(error != EOF && error != 0) tempPattern->Add(tempPrimitive);
				else delete tempPrimitive;
			}
			//check not at end of file
			if(end){ m_Patterns.Add(tempPattern);
			}else{
				for(i=0;i<tempPattern->GetSize();i++)
					delete tempPattern->GetAt(i);
				delete tempPattern;
			}
		}
	}

	return GetNoOfPatterns();
}

bool CTexture::DrawPattern2D(int idx)
{
/*	if(idx >= GetNoOfPatterns()){
		if(!m_TooManyObjects)
			AfxGetMainWnd()->MessageBox("Exceeded total number of unique symbols.\nAdditional objects will be displayed with the\nlast object",
			"Too Many Objects/Centroids",MB_ICONSTOP | MB_OK );
		m_TooManyObjects = true;
		idx = GetNoOfPatterns() - 1;
	}

	glPushMatrix();
	glTranslated(-5,-5,0);
	glColor3f(0,0,0);
	glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);  // filled mode 
	glEdgeFlag(GL_TRUE);

	double cosine,sine;

	for(int i=0;i<m_Patterns[idx]->GetSize();i++){
		//determine type & draw it
		switch(m_Patterns[idx]->GetAt(i)->type){
		case 0: //line
			glBegin(GL_LINES);
			glVertex2i(-m_Patterns[idx]->GetAt(i)->data.line.x1,
				m_Patterns[idx]->GetAt(i)->data.line.y1);
			glVertex2i(-m_Patterns[idx]->GetAt(i)->data.line.x2,
				m_Patterns[idx]->GetAt(i)->data.line.y2);
			glEnd();			
			break;
		case 1: //triangle
			if(m_Patterns[idx]->GetAt(i)->data.triangle.fill){
				glBegin(GL_TRIANGLES);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.triangle.x1,
					m_Patterns[idx]->GetAt(i)->data.triangle.y1);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.triangle.x2,
					m_Patterns[idx]->GetAt(i)->data.triangle.y2);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.triangle.x3,
					m_Patterns[idx]->GetAt(i)->data.triangle.y3);
				glEnd();
			}else{
				glEdgeFlag(GL_TRUE);
				glColor3f(0,0,0);
				glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);  // filled mode 
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.triangle.x1,
					m_Patterns[idx]->GetAt(i)->data.triangle.y1);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.triangle.x2,
					m_Patterns[idx]->GetAt(i)->data.triangle.y2);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.triangle.x3,
					m_Patterns[idx]->GetAt(i)->data.triangle.y3);
				glEnd();
				glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);// filled mode 
				glEdgeFlag(GL_FALSE);
				glColor3f(1,1,1);
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.triangle.x1,
					m_Patterns[idx]->GetAt(i)->data.triangle.y1);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.triangle.x2,
					m_Patterns[idx]->GetAt(i)->data.triangle.y2);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.triangle.x3,
					m_Patterns[idx]->GetAt(i)->data.triangle.y3);
				
				glEnd();
			}
			break;
		case 2: //square
			if(m_Patterns[idx]->GetAt(i)->data.square.fill){
				glBegin(GL_QUADS);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.square.x1,
					m_Patterns[idx]->GetAt(i)->data.square.y1);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.square.x2,
					m_Patterns[idx]->GetAt(i)->data.square.y2);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.square.x3,
					m_Patterns[idx]->GetAt(i)->data.square.y3);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.square.x4,
					m_Patterns[idx]->GetAt(i)->data.square.y4);
				glEnd();
			}else{
				glColor3f(0,0,0);
				glBegin(GL_LINE_LOOP);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.square.x1,
					m_Patterns[idx]->GetAt(i)->data.square.y1);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.square.x2,
					m_Patterns[idx]->GetAt(i)->data.square.y2);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.square.x3,
					m_Patterns[idx]->GetAt(i)->data.square.y3);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.square.x4,
					m_Patterns[idx]->GetAt(i)->data.square.y4);
				glEnd();
				glColor3f(1,1,1);
				glBegin(GL_QUADS);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.square.x1,
					m_Patterns[idx]->GetAt(i)->data.square.y1);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.square.x2,
					m_Patterns[idx]->GetAt(i)->data.square.y2);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.square.x3,
					m_Patterns[idx]->GetAt(i)->data.square.y3);
				glVertex2i(-m_Patterns[idx]->GetAt(i)->data.square.x4,
					m_Patterns[idx]->GetAt(i)->data.square.y4);
				glEnd();
				
			}
			break;
		case 3: //circle
			glPushMatrix();
			glTranslatef(-1.0f*m_Patterns[idx]->GetAt(i)->data.circle.x1,
					1.0f*m_Patterns[idx]->GetAt(i)->data.circle.y1,0);
			if(m_Patterns[idx]->GetAt(i)->data.circle.fill){
				glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);  // filled mode 
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				for(int j=0; j<15; j++) { 
					cosine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						cos(j * 2 * 3.14/15.0); 
					sine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						sin(j * 2 * 3.14/15.0); 
					glVertex2d(cosine, sine); 
				} 
				glEnd(); 
			}else{
				glPolygonMode(GL_FRONT_AND_BACK , GL_LINE);  // Unfilled mode 
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				for(int j=0; j<15; j++) { 
					cosine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						cos(j * 2 * 3.14/15.0); 
					sine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						sin(j * 2 * 3.14/15.0); 
					glVertex2d(cosine, sine); 
				} 
				glEnd(); 
				//reset so nothing else is messed up
				glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
			}
			glPopMatrix();
			break;
		}
	}
	glPopMatrix();
	*/
	return true;

}

bool CTexture::DrawPattern3D(int idx,bool flip)
{
	/*
	if(idx >= GetNoOfPatterns()){
		if(!m_TooManyObjects)
			AfxGetMainWnd()->MessageBox("Exceeded total number of unique symbols.\nAdditional objects will be displayed with the\nlast object",
			"Too Many Objects/Centroids",MB_ICONSTOP | MB_OK );
		m_TooManyObjects = true;
		idx = GetNoOfPatterns() - 1;
	}

	if(idx < 0) return false;

	glPushMatrix();
	glTranslated(-5,-5,-1);
	glColor3f(0,0,0);
	glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);  // filled mode 
	glEdgeFlag(GL_TRUE);
	
	int r;
	if(flip){
		//flip about symetry & move back over centre
		r = 1;
		//glTranslated(10,0,0);
	}else{
		//nothing
		r = -1;
		glTranslated(10,0,0);
	}

	double cosine,sine;

	glColor3f(0,0,0);
	double dist;
	for(int i=0;i<m_Patterns[idx]->GetSize();i++){
		dist = 0.001*i;
		//determine type & draw it
		switch(m_Patterns[idx]->GetAt(i)->type){
		case 0: //line
			glBegin(GL_LINES);
			glVertex3i(m_Patterns[idx]->GetAt(i)->data.line.x1*r,
				m_Patterns[idx]->GetAt(i)->data.line.y1,0);
			glVertex3i(m_Patterns[idx]->GetAt(i)->data.line.x2*r,
				m_Patterns[idx]->GetAt(i)->data.line.y2,0);
			glEnd();			
			break;
		case 1: //triangle
			if(m_Patterns[idx]->GetAt(i)->data.triangle.fill){
				glBegin(GL_TRIANGLES);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x1*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y1,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x2*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y2,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x3*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y3,0);
				glEnd();
			}else{
				//glEdgeFlag(GL_TRUE);
				glColor3f(0,0,0);
				glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);  // filled mode 
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x1*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y1,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x2*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y2,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x3*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y3,0);
				glEnd();
				glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);// filled mode 
				//glEdgeFlag(GL_FALSE);
				glColor3f(1,1,1);
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x1*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y1,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x2*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y2,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x3*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y3,0);
				glEnd();
			}
			break;
		case 2: //square
			if(m_Patterns[idx]->GetAt(i)->data.square.fill){
				glBegin(GL_QUADS);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x1*r,
					m_Patterns[idx]->GetAt(i)->data.square.y1,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x2*r,
					m_Patterns[idx]->GetAt(i)->data.square.y2,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x3*r,
					m_Patterns[idx]->GetAt(i)->data.square.y3,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x4*r,
					m_Patterns[idx]->GetAt(i)->data.square.y4,0);
				glEnd();
			}else{
				//glEdgeFlag(GL_TRUE);
				glColor3f(0,0,0);
				glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);  // filled mode 
				glBegin(GL_LINE_LOOP);  // Use polygon to approximate a circle 
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x1*r,
					m_Patterns[idx]->GetAt(i)->data.square.y1,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x2*r,
					m_Patterns[idx]->GetAt(i)->data.square.y2,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x3*r,
					m_Patterns[idx]->GetAt(i)->data.square.y3,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x4*r,
					m_Patterns[idx]->GetAt(i)->data.square.y4,0);
				glEnd();
				glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);// filled mode 
				//glEdgeFlag(GL_FALSE);
				glColor3f(1,1,1);
				glBegin(GL_QUADS);  // Use polygon to approximate a circle 
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x1*r,
					m_Patterns[idx]->GetAt(i)->data.square.y1,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x2*r,
					m_Patterns[idx]->GetAt(i)->data.square.y2,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x3*r,
					m_Patterns[idx]->GetAt(i)->data.square.y3,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x4*r,
					m_Patterns[idx]->GetAt(i)->data.square.y4,0);
				glEnd();
			}
			break;
		case 3: //circle
			glPushMatrix();
			glTranslatef(1.0f*m_Patterns[idx]->GetAt(i)->data.circle.x1*r,
					1.0f*m_Patterns[idx]->GetAt(i)->data.circle.y1,0);
			if(m_Patterns[idx]->GetAt(i)->data.circle.fill){
				//glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);  // filled mode 
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				for(int j=0; j<15; j++) { 
					cosine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						cos(j * 2 * 3.14/15.0); 
					sine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						sin(j * 2 * 3.14/15.0); 
					glVertex3d(cosine*r, sine,0); 
				} 
				glEnd(); 
			}else{
				//glEdgeFlag(GL_TRUE);
				glColor3f(0,0,0);
				glPolygonMode(GL_FRONT_AND_BACK , GL_LINE);  // Unfilled mode 
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				for(int j=0; j<15; j++) { 
					cosine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						cos(j * 2 * 3.14/15.0); 
					sine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						sin(j * 2 * 3.14/15.0); 
					glVertex3d(cosine*r, sine,0); 
				} 
				glEnd(); 
				glColor3f(1,1,1);
				//glEdgeFlag(GL_FALSE);
				//reset so nothing else is messed up
				//glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);  // filled mode 
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				for(j=0; j<15; j++) { 
					cosine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						cos(j * 2 * 3.14/15.0); 
					sine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						sin(j * 2 * 3.14/15.0); 
					glVertex3d(cosine, sine,0); 
				} 
				glEnd();
			}
			glPopMatrix();
			break;
		}
	}/*
int r;
	if(flip){
		//flip about symetry & move back over centre
		r = -1;
		glTranslated(10,0,0);
	}else{
		//nothing
		r = 1;
	}

	double cosine,sine;

	glColor3f(0,0,0);
	double dist;
	for(int i=0;i<m_Patterns[idx]->GetSize();i++){
		dist = 0.001*i;
		//determine type & draw it
		switch(m_Patterns[idx]->GetAt(i)->type){
		case 0: //line
			glBegin(GL_LINES);
			glVertex3i(m_Patterns[idx]->GetAt(i)->data.line.x1*r,
				m_Patterns[idx]->GetAt(i)->data.line.y1,0);
			glVertex3i(m_Patterns[idx]->GetAt(i)->data.line.x2*r,
				m_Patterns[idx]->GetAt(i)->data.line.y2,0);
			glEnd();			
			break;
		case 1: //triangle
			if(m_Patterns[idx]->GetAt(i)->data.triangle.fill){
				glBegin(GL_TRIANGLES);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x1*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y1,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x2*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y2,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x3*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y3,0);
				glEnd();
			}else{
				glEdgeFlag(GL_TRUE);
				glColor3f(0,0,0);
				glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);  // filled mode 
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x1*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y1,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x2*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y2,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x3*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y3,0);
				glEnd();
				glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);// filled mode 
				glEdgeFlag(GL_FALSE);
				glColor3f(1,1,1);
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x1*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y1,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x2*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y2,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.triangle.x3*r,
					m_Patterns[idx]->GetAt(i)->data.triangle.y3,0);
				glEnd();
			}
			break;
		case 2: //square
			if(m_Patterns[idx]->GetAt(i)->data.square.fill){
				glBegin(GL_QUADS);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x1*r,
					m_Patterns[idx]->GetAt(i)->data.square.y1,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x2*r,
					m_Patterns[idx]->GetAt(i)->data.square.y2,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x3*r,
					m_Patterns[idx]->GetAt(i)->data.square.y3,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x4*r,
					m_Patterns[idx]->GetAt(i)->data.square.y4,0);
				glEnd();
			}else{
				glEdgeFlag(GL_TRUE);
				glColor3f(0,0,0);
				glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);  // filled mode 
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x1*r,
					m_Patterns[idx]->GetAt(i)->data.square.y1,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x2*r,
					m_Patterns[idx]->GetAt(i)->data.square.y2,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x3*r,
					m_Patterns[idx]->GetAt(i)->data.square.y3,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x4*r,
					m_Patterns[idx]->GetAt(i)->data.square.y4,0);
				glEnd();
				glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);// filled mode 
				glEdgeFlag(GL_FALSE);
				glColor3f(1,1,1);
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x1*r,
					m_Patterns[idx]->GetAt(i)->data.square.y1,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x2*r,
					m_Patterns[idx]->GetAt(i)->data.square.y2,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x3*r,
					m_Patterns[idx]->GetAt(i)->data.square.y3,0);
				glVertex3i(m_Patterns[idx]->GetAt(i)->data.square.x4*r,
					m_Patterns[idx]->GetAt(i)->data.square.y4,0);
				glEnd();
			}
			break;
		case 3: //circle
			glPushMatrix();
			glTranslatef(1.0f*m_Patterns[idx]->GetAt(i)->data.circle.x1*r,
					1.0f*m_Patterns[idx]->GetAt(i)->data.circle.y1,0);
			if(m_Patterns[idx]->GetAt(i)->data.circle.fill){
				glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);  // filled mode 
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				for(int j=0; j<15; j++) { 
					cosine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						cos(j * 2 * 3.14/15.0); 
					sine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						sin(j * 2 * 3.14/15.0); 
					glVertex3d(cosine*r, sine,0); 
				} 
				glEnd(); 
			}else{
				glEdgeFlag(GL_TRUE);
				glColor3f(0,0,0);
				glPolygonMode(GL_FRONT_AND_BACK , GL_LINE);  // Unfilled mode 
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				for(int j=0; j<15; j++) { 
					cosine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						cos(j * 2 * 3.14/15.0); 
					sine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						sin(j * 2 * 3.14/15.0); 
					glVertex3d(cosine*r, sine,0); 
				} 
				glEnd(); 
				glColor3f(1,1,1);
				glEdgeFlag(GL_FALSE);
				//reset so nothing else is messed up
				glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);  // filled mode 
				glBegin(GL_POLYGON);  // Use polygon to approximate a circle 
				for(j=0; j<15; j++) { 
					cosine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						cos(j * 2 * 3.14/15.0); 
					sine = m_Patterns[idx]->GetAt(i)->data.circle.radius * 
						sin(j * 2 * 3.14/15.0); 
					glVertex3d(cosine, sine,0); 
				} 
				glEnd();
			}
			glPopMatrix();
			break;
		}
	}*//*
	glPopMatrix();
	*/
	return true;
}


bool CTexture::GDIPatterns(int idx,CDC *dc,int x,int y)
{
	if(idx >= GetNoOfPatterns()){
		if(!m_TooManyObjects)
			AfxGetMainWnd()->MessageBox("Exceeded total number of unique symbols.\nAdditional objects will be displayed with the\nlast object",
			"Too Many Objects/Centroids",MB_ICONSTOP | MB_OK );
		m_TooManyObjects = true;
		idx = GetNoOfPatterns() - 1;
	}

	if(idx < 0) return false;

	CPoint point[4];

	int radius;
	CRect rect;

	CBrush black_brush,white_brush;
	white_brush.CreateSolidBrush(RGB(1, 1, 1));
	black_brush.CreateSolidBrush(RGB(0, 0, 0));

	int x1,x2,y1,y2;

	y += 10;
	int last = dc->SetROP2(R2_BLACK);

	double dist;
	for(int i=m_Patterns[idx]->GetSize()-1;i>=0;i--){
		dc->SetROP2(R2_BLACK);
		dist = 0.001*i;
		//determine type & draw it
		switch(m_Patterns[idx]->GetAt(i)->type){
		case 0: //line
			x1 = m_Patterns[idx]->GetAt(i)->data.line.x1;
			x2 = m_Patterns[idx]->GetAt(i)->data.line.x2;
			y1 = m_Patterns[idx]->GetAt(i)->data.line.y1;
			y2 = m_Patterns[idx]->GetAt(i)->data.line.y2;			
			dc->MoveTo(x1+x,-y1+y);
			dc->LineTo(x2+x,-y2+y);
			break;
		case 1: //triangle
			point[0].x = m_Patterns[idx]->GetAt(i)->data.triangle.x1+x;
			point[0].y = -m_Patterns[idx]->GetAt(i)->data.triangle.y1+y;
			point[1].x = m_Patterns[idx]->GetAt(i)->data.triangle.x2+x;
			point[1].y = -m_Patterns[idx]->GetAt(i)->data.triangle.y2+y;
			point[2].x = m_Patterns[idx]->GetAt(i)->data.triangle.x3+x;
			point[2].y = -m_Patterns[idx]->GetAt(i)->data.triangle.y3+y;
			if(m_Patterns[idx]->GetAt(i)->data.triangle.fill){
				dc->Polygon(point,3);
			}else{
				dc->SetROP2(R2_WHITE);
				dc->Polygon(point,3);					
				dc->SetROP2(R2_BLACK);
				dc->MoveTo(point[0].x,point[0].y);
				dc->LineTo(point[1].x,point[1].y);
				dc->LineTo(point[2].x,point[2].y);
				dc->LineTo(point[0].x,point[0].y);
			}
			break;
		case 2: //square
			point[0].x = m_Patterns[idx]->GetAt(i)->data.square.x1+x;
			point[0].y = -m_Patterns[idx]->GetAt(i)->data.square.y1+y;
			point[1].x = m_Patterns[idx]->GetAt(i)->data.square.x2+x;
			point[1].y = -m_Patterns[idx]->GetAt(i)->data.square.y2+y;
			point[2].x = m_Patterns[idx]->GetAt(i)->data.square.x3+x;
			point[2].y = -m_Patterns[idx]->GetAt(i)->data.square.y3+y;
			point[3].x = m_Patterns[idx]->GetAt(i)->data.square.x4+x;
			point[3].y = -m_Patterns[idx]->GetAt(i)->data.square.y4+y;

			if(m_Patterns[idx]->GetAt(i)->data.square.fill){
				dc->Polygon(point,4);
			}else{
				dc->SetROP2(R2_WHITE);
				dc->Polygon(point,4);
				dc->SetROP2(R2_BLACK);
				dc->MoveTo(point[0].x,point[0].y);
				dc->LineTo(point[1].x,point[1].y);
				dc->LineTo(point[2].x,point[2].y);
				dc->LineTo(point[3].x,point[3].y);				
				dc->LineTo(point[0].x,point[0].y);			
			}
			break;
		case 3: //circle
			radius = m_Patterns[idx]->GetAt(i)->data.circle.radius;
			x1 = m_Patterns[idx]->GetAt(i)->data.circle.x1+x;
			y1 = m_Patterns[idx]->GetAt(i)->data.circle.y1+y-10;
			
			if(m_Patterns[idx]->GetAt(i)->data.circle.fill){
				dc->SetROP2(R2_BLACK);
				dc->Ellipse(x1-radius,y1-radius,x1+radius,y1+radius);	
			}else{
				dc->SetROP2(R2_BLACK);
				dc->Ellipse(x1-radius,y1-radius,x1+radius,y1+radius);
				dc->SetROP2(R2_WHITE);
				dc->Ellipse(x1-radius+1,y1-radius+1,x1+radius-1,y1+radius-1);
			}
			break;
		}
	}
	dc->SetROP2(last);
	return true;
}


bool CTexture::GDIPatterns(int idx,HDC hdc,int x,int y,double multiplier)
{
	if(idx >= GetNoOfPatterns()){
		if(!m_TooManyObjects)
			AfxGetMainWnd()->MessageBox("Exceeded total number of unique symbols.\nAdditional objects will be displayed with the\nlast object",
			"Too Many Objects/Centroids",MB_ICONSTOP | MB_OK );
		m_TooManyObjects = true;
		idx = GetNoOfPatterns() - 1;
	}

	if(idx < 0) return false;

	CPoint point[4];

	int radius;
	CRect rect;

	CBrush black_brush,white_brush;
	white_brush.CreateSolidBrush(RGB(1, 1, 1));
	black_brush.CreateSolidBrush(RGB(0, 0, 0));

	int x1,x2,y1,y2;

	y += (int)(10*multiplier-1*multiplier);
	x -= (int)(3*multiplier);

	int last = SetROP2(hdc,R2_BLACK);

	double dist;
	for(int i=m_Patterns[idx]->GetSize()-1;i>=0;i--){
		SetROP2(hdc,R2_BLACK);
		dist = 0.001*i;
		//determine type & draw it
		switch(m_Patterns[idx]->GetAt(i)->type){
		case 0: //line
			x1 = (int)(m_Patterns[idx]->GetAt(i)->data.line.x1*multiplier);
			x2 = (int)(m_Patterns[idx]->GetAt(i)->data.line.x2*multiplier);
			y1 = (int)(m_Patterns[idx]->GetAt(i)->data.line.y1*multiplier);
			y2 = (int)(m_Patterns[idx]->GetAt(i)->data.line.y2*multiplier);			
			MoveToEx(hdc,x1+x,-y1+y,NULL);
			LineTo(hdc,x2+x,-y2+y);
			break;
		case 1: //triangle
			point[0].x = (int)(m_Patterns[idx]->GetAt(i)->data.triangle.x1*multiplier+x);
			point[0].y = (int)(-m_Patterns[idx]->GetAt(i)->data.triangle.y1*multiplier+y);
			point[1].x = (int)(m_Patterns[idx]->GetAt(i)->data.triangle.x2*multiplier+x);
			point[1].y = (int)(-m_Patterns[idx]->GetAt(i)->data.triangle.y2*multiplier+y);
			point[2].x = (int)(m_Patterns[idx]->GetAt(i)->data.triangle.x3*multiplier+x);
			point[2].y = (int)(-m_Patterns[idx]->GetAt(i)->data.triangle.y3*multiplier+y);
			if(m_Patterns[idx]->GetAt(i)->data.triangle.fill){
				Polygon(hdc,point,3);
			}else{
				SetROP2(hdc,R2_WHITE);
				Polygon(hdc,point,3);					
				SetROP2(hdc,R2_BLACK);
				MoveToEx(hdc,point[0].x,point[0].y,NULL);
				LineTo(hdc,point[1].x,point[1].y);
				LineTo(hdc,point[2].x,point[2].y);
				LineTo(hdc,point[0].x,point[0].y);
			}
			break;
		case 2: //square
			point[0].x = (int)(m_Patterns[idx]->GetAt(i)->data.square.x1*multiplier+x);
			point[0].y = (int)(-m_Patterns[idx]->GetAt(i)->data.square.y1*multiplier+y);
			point[1].x = (int)(m_Patterns[idx]->GetAt(i)->data.square.x2*multiplier+x);
			point[1].y = (int)(-m_Patterns[idx]->GetAt(i)->data.square.y2*multiplier+y);
			point[2].x = (int)(m_Patterns[idx]->GetAt(i)->data.square.x3*multiplier+x);
			point[2].y = (int)(-m_Patterns[idx]->GetAt(i)->data.square.y3*multiplier+y);
			point[3].x = (int)(m_Patterns[idx]->GetAt(i)->data.square.x4*multiplier+x);
			point[3].y = (int)(-m_Patterns[idx]->GetAt(i)->data.square.y4*multiplier+y);

			if(m_Patterns[idx]->GetAt(i)->data.square.fill){
				Polygon(hdc,point,4);
			}else{
				SetROP2(hdc,R2_WHITE);
				Polygon(hdc,point,4);
				SetROP2(hdc,R2_BLACK);
				MoveToEx(hdc,point[0].x,point[0].y,NULL);
				LineTo(hdc,point[1].x,point[1].y);
				LineTo(hdc,point[2].x,point[2].y);
				LineTo(hdc,point[3].x,point[3].y);				
				LineTo(hdc,point[0].x,point[0].y);			
			}
			break;
		case 3: //circle
			y -= (int)(9*multiplier);
			radius = (int)(m_Patterns[idx]->GetAt(i)->data.circle.radius*multiplier);
			x1 = (int)(m_Patterns[idx]->GetAt(i)->data.circle.x1*multiplier+x);
			y1 = (int)(m_Patterns[idx]->GetAt(i)->data.circle.y1*multiplier+y);
			
			if(m_Patterns[idx]->GetAt(i)->data.circle.fill){
				SetROP2(hdc,R2_BLACK);
				Ellipse(hdc,x1-radius,y1-radius,x1+radius,y1+radius);	
			}else{
				SetROP2(hdc,R2_BLACK);
				Ellipse(hdc,x1-radius,y1-radius,x1+radius,y1+radius);
				SetROP2(hdc,R2_WHITE);
				Ellipse(hdc,x1-radius+1,y1-radius+1,x1+radius-1,y1-1+radius);
			}
			break;
		}
	}

	SetROP2(hdc,last);
	return true;
}


