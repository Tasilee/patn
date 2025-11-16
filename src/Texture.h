// Texture.h: interface for the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURE_H__8F00FD6A_8BE7_11D4_A3D5_525405F5DADA__INCLUDED_)
#define AFX_TEXTURE_H__8F00FD6A_8BE7_11D4_A3D5_525405F5DADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CTexture  
{
	enum Shape {line,triangle,square,circle};

	typedef struct Line{
		int x1,y1,x2,y2;
	}Line;

	typedef struct Triangle{
		int fill;
		int x1,y1,x2,y2,x3,y3;
	}Triangle;

	typedef struct Square{
		int fill;
		int x1,y1,x2,y2,x3,y3,x4,y4;
	}Square;

	typedef struct Circle{
		int fill;
		int x1,y1,radius;
	}Circle;

	typedef struct Primitive{
		Shape type;
		union{
			Line line;
			Triangle triangle;
			Square square;
			Circle circle;
		} data;
	}Primitive;

	typedef CArray<Primitive*> Pattern;

public:
	CTexture();
	virtual ~CTexture();

protected:
	CArray<Pattern*> m_Patterns;
	bool m_TooManyObjects;

public:
	// loads patterns from filename & returns total number of symbols actually loaded
	int LoadPatterns(int,CString filename);

	// returns sizeof m_Patterns
	int GetNoOfPatterns(){return (int)m_Patterns.GetSize();};

	// draw the pattern found at idx & returns if successful
	bool DrawPattern2D(int idx);
	bool DrawPattern3D(int idx,bool flip);

	bool GDIPatterns(int idx,CDC *dc,int x,int y);
	bool GDIPatterns(int idx,HDC hdc,int x,int y,double multiplier);

};

#endif // !defined(AFX_TEXTURE_H__8F00FD6A_8BE7_11D4_A3D5_525405F5DADA__INCLUDED_)




