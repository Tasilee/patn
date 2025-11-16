// 3DXView.cpp : implementation file
//

//printing has been removed!

#include "stdafx.h"
#include "patn.h"
#include "patndoc.h"
#include "tableview.h"
#include "WriteAVI.h"
#include "3DXView.h"
#include <math.h>
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "KruskalWalice.h"
#include "pccvectorselection.h"
#include "bitms.h"
#include "objectsize.h"
#include "numframes.h"
#include "d3dx.h"
#include "mainframe.h"
#include ".\3dxview.h"
#include "selcolforcolours.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double drawratio = 0;

HENHMETAFILE g_emf; 

//callback functgion for enum display modes
long max_x= 0;
long max_y= 0;
BOOL WINAPI EnumDisplayModesCallback(LPDDSURFACEDESC2 lpDDSurfaceDesc,LPVOID lpContext)
{
	if(max_x*max_y < (long)(lpDDSurfaceDesc->dwWidth*lpDDSurfaceDesc->dwHeight))
	{
		max_x = lpDDSurfaceDesc->dwWidth;
		max_y = lpDDSurfaceDesc->dwHeight;
	}
	return true;
}

BOOL WINAPI EnumSurfacesCallback(LPDDSURFACEDESC2 lpDDSurfaceDesc,LPVOID lpContext)
{
	if(max_x*max_y < (long)(lpDDSurfaceDesc->dwWidth*lpDDSurfaceDesc->dwHeight))
	{
		max_x = lpDDSurfaceDesc->dwWidth;
		max_y = lpDDSurfaceDesc->dwHeight;
	}
	return true;
}
void SrtShl(CArray<double>*  data,CArray<long>* indx)
{
	long i,j,fm,ii;
	long fn = indx->GetSize();
	long n = fn;
	long nloops = (long) (log(fn*1.0)/log(2.0));
	long m = (long) pow(2,nloops-1);
	double tempd;
	long tempi;

	if(fn == 0)
		return;

	for(ii=1;ii<=max(1,nloops);ii++){
		fm = m;
		for(i=1;i<=max(1,n-m);i++){
			if(data->GetAt(i-1) > data->GetAt(i+m-1)){
				tempd = data->GetAt(i+m-1);
				tempi = indx->GetAt(i+m-1);
				data->SetAt(i+m-1,data->GetAt(i-1));
				indx->SetAt(i+m-1,indx->GetAt(i-1));
				if(i <= m){
					data->SetAt(i-1,tempd);
					indx->SetAt(i-1,tempi);
				}else{
					for(j=i-m;j>=1;j-=m){
						if(tempd >= data->GetAt(j-1)) 
							break;
						data->SetAt(j+m-1,data->GetAt(j-1));
						indx->SetAt(j+m-1,indx->GetAt(j-1));
					}
					data->SetAt(j+m-1,tempd);
					indx->SetAt(j+m-1,tempi);
				}
			}
		}
		m = fm/2;
	}
}
void RotateAboutVector(D3DVECTOR * mod,D3DVECTOR * vDir,double fRads)
{
	//build rotation matrix
	D3DMATRIX mat;
    double fCos = cos( fRads );
    double fSin = sin( fRads );
    D3DVECTOR v    = Normalize( *vDir );//*vDir; //

    mat._11 = (float)(( v.x * v.x ) * ( 1.0f - fCos ) + fCos);
    mat._12 = (float)(( v.x * v.y ) * ( 1.0f - fCos ) - (v.z * fSin));
    mat._13 = (float)(( v.x * v.z ) * ( 1.0f - fCos ) + (v.y * fSin));

    mat._21 = (float)(( v.y * v.x ) * ( 1.0f - fCos ) + (v.z * fSin));
    mat._22 = (float)(( v.y * v.y ) * ( 1.0f - fCos ) + fCos );
    mat._23 = (float)(( v.y * v.z ) * ( 1.0f - fCos ) - (v.x * fSin));

    mat._31 = (float)(( v.z * v.x ) * ( 1.0f - fCos ) - (v.y * fSin));
    mat._32 = (float)(( v.z * v.y ) * ( 1.0f - fCos ) + (v.x * fSin));
    mat._33 = (float)(( v.z * v.z ) * ( 1.0f - fCos ) + fCos);

    mat._14 = mat._24 = mat._34 = 0.0f;
    mat._41 = mat._42 = mat._43 = 0.0f;
    mat._44 = 1.0f;

	//multiply mod
	v.x = mod->x;
	v.y = mod->y;
	v.z = mod->z;

	mod->x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + mat._41;
	mod->y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + mat._42;
	mod->z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + mat._43;
}

//-----------------------------------------------------------------------------
// Name: App_InitDeviceObjects()
// Desc: Initialize scene objects. This function is called after all the
//       DirectDraw and Direct3D objects have been initialized. It makes sense
//       to structure code this way, separating the DDraw/D3D initialization
//       code from the app-specific intialization code.
//-----------------------------------------------------------------------------
HRESULT C3DXView::App_InitDeviceObjects( LPDIRECT3DDEVICE7 pd3dDevice )
{
	if(pd3dDevice == NULL )//&& ((CPatnApp*)AfxGetApp())->m_ordtype == 1)
		return E_INVALIDARG;

    // Set the material as yellow. We're setting the ambient color here
    // since this tutorial only uses ambient lighting. For apps that use real
    // lights, the diffuse and specular values should be set. (In addition, the
    // polygons' vertices need normals for true lighting.)
    D3DMATERIAL7 mtrl;
    ZeroMemory( &mtrl, sizeof(mtrl) );
	mtrl.specular.r = mtrl.specular.g = mtrl.specular.b = 1.0f;
    mtrl.diffuse.r = mtrl.diffuse.g = mtrl.diffuse.b = .5f ;
    mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = .5f;
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		pd3dDevice->SetMaterial( &mtrl );
	//}

    // Set the transform matrices. Direct3D uses three independant matrices:
    // the world matrix, the view matrix, and the projection matrix. For
    // convienence, we are first setting up an identity matrix.
    D3DMATRIX mat;
    mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
    mat._12 = mat._13 = mat._14 = mat._41 = 0.0f;
    mat._21 = mat._23 = mat._24 = mat._42 = 0.0f;
    mat._31 = mat._32 = mat._34 = mat._43 = 0.0f;
    
    // The world matrix controls the position and orientation of the polygons
    // in world space. We'll use it later to spin the triangle.
    D3DMATRIX matWorld = mat;
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );
	//}else{
	//	m_matWorld = mat;
	//}

    // The view matrix defines the position and orientation of the camera.
    // Here, we are just moving it back along the z-axis by 10 units.
    D3DMATRIX matView = mat;
    matView._43 = 10.0f;
    //if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		//pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView );
	//}else{
	//	m_matView = mat;
	//}

    // The projection matrix defines how the 3D scene is "projected" onto the
    // 2D render target (the backbuffer surface). Refer to the docs for more
    // info about projection matrices.
    D3DMATRIX matProj = mat;
    matProj._11 =  3.0f;
    matProj._22 =  3.0f;
	matProj._33 = 1.5f;
    matProj._34 =  1.5f;
    matProj._43 = -1.0f;
    matProj._44 =  0.0f;
	
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );
	//}else{
	//	m_matProj = mat;
	//}

    // Enable z-buffering.
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, TRUE );
	//}

    // Set up the light. Note: to be friendly for cards that can do hardware
    // transform-and-lighting (TnL), we should (but we don't) check the caps
    // to see if the device can support directional lights. 
    ZeroMemory( &m_light, sizeof(D3DLIGHT7) );
	m_light.dltType       =  D3DLIGHT_DIRECTIONAL;
    m_light.dcvDiffuse.r  =  0.7f;
    m_light.dcvDiffuse.g  =  0.7f;
    m_light.dcvDiffuse.b  =  0.7f;
	m_light.dcvAmbient.r = .7f;
	m_light.dcvAmbient.g = .7f;
	m_light.dcvAmbient.b = .7f;
	m_light.dcvSpecular.r = 1.0f;
	m_light.dcvSpecular.g = 1.0f;
	m_light.dcvSpecular.b = 1.0f;
    /*m_light.dvDirection.x =  -50.0f;
    m_light.dvDirection.y = 25.0f;
    m_light.dvDirection.z =  250.0f;
	*/
	m_vLight.x = -50.0f;
	m_vLight.y = 25.0f;
	m_vLight.z = 250.0f;
	m_light.dvDirection.x = m_vLight.x;
	m_light.dvDirection.y = m_vLight.y;
	m_light.dvDirection.z = m_vLight.z;
	m_light.dvFalloff = 0;
	m_light.dvRange = 1000.0f;
	m_light.dvAttenuation0 = 1;
	m_light.dvAttenuation1 = 0;
	m_light.dvAttenuation2 = 0;
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		pd3dDevice->SetLight( 0, &m_light );
	    pd3dDevice->LightEnable( 0, TRUE );
//		pd3dDevice->SetRenderState( D3DRENDERSTATE_LIGHTING, TRUE );
		pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, TRUE );

	//}

	if(this->m_viewtype == 2)
	{
		OnSetRotationyz();
	}
	else if(m_viewtype == 3)
	{
		OnSetRotationxz();
	}
	else if(m_viewtype == 1)
	{
		OnSetRotationxy();
	}
	else
	{
	}

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: SetViewMatrix()
// Desc: Given an eye point, a lookat point, and an up vector, this
//       function builds a 4x4 view matrix.
//-----------------------------------------------------------------------------
HRESULT C3DXView::SetViewMatrix( D3DMATRIX& mat, D3DVECTOR& vFrom, D3DVECTOR& vAt,
                       D3DVECTOR& vWorldUp )
{
	if(true){//m_viewtype == 0){
		// Get the z basis vector, which points straight ahead. This is the
		// difference from the eyepoint to the lookat point.
		D3DVECTOR vView = vAt - vFrom;

		
		FLOAT fLength = Magnitude( vView );
		if( fLength < 1e-6f )
			return E_INVALIDARG;

		// Normalize the z basis vector
		vView /= fLength;//lengthen vView

		// Get the dot product, and calculate the projection of the z basis
		// vector onto the up vector. The projection is the y basis vector.
		FLOAT fDotProduct = DotProduct( vWorldUp, vView );

		D3DVECTOR vUp = vWorldUp - fDotProduct * vView;

		// If this vector has near-zero length because the input specified a
		// bogus up vector, let's try a default up vector
		if( 1e-6f > ( fLength = Magnitude( vUp ) ) )
		{
			vUp = D3DVECTOR( 0.0f, 1.0f, 0.0f ) - vView.y * vView;

			// If we still have near-zero length, resort to a different axis.
			if( 1e-6f > ( fLength = Magnitude( vUp ) ) )
			{
				vUp = D3DVECTOR( 0.0f, 0.0f, 1.0f ) - vView.z * vView;

				if( 1e-6f > ( fLength = Magnitude( vUp ) ) )
					return E_INVALIDARG;
			}
		}

		// Normalize the y basis vector
		vUp /= fLength;

		// The x basis vector is found simply with the cross product of the y
		// and z basis vectors
		D3DVECTOR vRight = CrossProduct( vUp, vView );
    
		// Start building the matrix. The first three rows contains the basis
		// vectors used to rotate the view to point at the lookat point
		if(m_viewtype == 0){
			mat._11 = vRight.x;  mat._12 = vUp.x;  mat._13 = vView.x;  mat._14 = 0.0f;
			mat._21 = vRight.y;  mat._22 = vUp.y;  mat._23 = vView.y;  mat._24 = 0.0f;
			mat._31 = vRight.z;  mat._32 = vUp.z;  mat._33 = vView.z;  mat._34 = 0.0f;
		}else
		{
			mat._11 = vRight.x;  mat._12 = vUp.x;  mat._13 = vView.x;  mat._14 = 0.0f;
			mat._21 = vRight.y;  mat._22 = vUp.y;  mat._23 = vView.y;  mat._24 = 0.0f;
			mat._31 = vRight.z;  mat._32 = vUp.z;  mat._33 = vView.z;  mat._34 = 0.0f;
		}

		// Do the translation values (rotations are still about the eyepoint)
		//move vFrom back
		mat._41 = - DotProduct( vFrom, vRight )*2;
		mat._42 = - DotProduct( vFrom, vUp )*2;
		/*if(m_viewtype == 0)*/ mat._43 = - DotProduct( vFrom, vView )+1.0f;
		//else mat._43 = - DotProduct( vFrom, vView )*2 ;
		mat._44 = 1.0f;

		return S_OK;
	}else if(m_viewtype == 1){
		mat._11 = 1;
		mat._21 = 0;
		mat._31 = 0;
		mat._41 = 0;

		mat._12 = 0;
		mat._22 = 1;
		mat._32 = 0;
		mat._42 = 0;

		mat._13 = 0;
		mat._23 = 0;
		mat._33 = 1;
		mat._43 = 2.5;

		mat._14 = 0.0f;
		mat._24 = 0.0f;
		mat._34 = 0.0f;
		mat._44 = 1.0f;	
	}
	return S_OK;
}





//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is used for animating the scene. The
//       device is used for changing various render states, and the timekey is
//       used for timing of the dynamics of the scene.
//-----------------------------------------------------------------------------
HRESULT C3DXView::App_FrameMove( LPDIRECT3DDEVICE7 pd3dDevice, FLOAT fTimeKey )
{
	if(pd3dDevice == NULL){// && ((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		return E_INVALIDARG;
	}
    // Use the above parameters to build a new view matrix and put it into
    // effect.
    //D3DMATRIX matView;
	if(true) { //m_viewtype == 0){
		SetViewMatrix( m_matView, m_vEyePt, m_vLookatPt, m_vUpVec );
	}else if(m_viewtype == 1){
		//m_vEyePt = D3DVECTOR(0,0,-1.5);
		//m_vLookatPt = D3DVECTOR(0,0,0);
		//m_vUpVec = D3DVECTOR(0,0.5,-1.5);
		m_vEyePt = D3DVECTOR( 0, 0, 1.2f);
		m_vLookatPt = D3DVECTOR( 0,0,0);
		m_vUpVec    = D3DVECTOR( 0, 1,0 );
		m_vRight = D3DVECTOR(1,0,0);
		SetViewMatrix( m_matView, m_vEyePt, m_vLookatPt, m_vUpVec );
	}else if(m_viewtype == 2){
		//m_vEyePt = D3DVECTOR(1.5,0,0);
		//m_vLookatPt = D3DVECTOR(0,0,0);
		//m_vUpVec = D3DVECTOR(1.5,0.5,0);
		m_vEyePt = D3DVECTOR( 1.2f, 0, 0);
		m_vLookatPt = D3DVECTOR( 0,0,0);
		m_vUpVec    = D3DVECTOR( 0,1 , 0);
		m_vRight = D3DVECTOR(0,0,1);
		SetViewMatrix( m_matView, m_vEyePt, m_vLookatPt, m_vUpVec );
	}else if(m_viewtype == 3){
		//m_vEyePt = D3DVECTOR(0,1.5,0);
		//m_vLookatPt = D3DVECTOR(0,0,0);
		//m_vUpVec = D3DVECTOR(0,1.5,0.5);
		m_vEyePt = D3DVECTOR( 0, 1.2f, 0);
		m_vLookatPt = D3DVECTOR( 0,0,0);
		m_vUpVec    = D3DVECTOR( 0,0 , 1);
		m_vRight = D3DVECTOR(1,0,0);
		SetViewMatrix( m_matView, m_vEyePt, m_vLookatPt, m_vUpVec );
	}
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &m_matView );

		D3DMATRIX proj;
		
		float n = 0.7f;
		float f = -2000.7f, fov= 90.0f*2*3.14f/180.0f, aspect = 1;

		float yScale = 1/tan(fov/2.0f);
		float xScale = yScale / aspect;

		proj._11 = proj._12 = proj._13 = proj._14 = 0;
		proj._21 = proj._22 = proj._23 = proj._24 = 0;
		proj._31 = proj._32 = proj._33 = proj._34 = 0;
		proj._41 = proj._42 = proj._43 = proj._44 = 0;

		proj._11 = xScale;
		proj._22 = yScale;
		proj._33 = f / (f - n);
		proj._34 = 1;
		proj._43 = -n * f / (f - n);

	//	pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &proj);
	//}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Renders the scene. This tutorial draws a bunch of intersecting
//       triangles that are rotating about the y-axis. Without z-buffering,
//       the polygons could not be drawn correctly (unless the app performed
//       complex polygon-division routines and sorted the polygons in back-to-
//       front order.)
//-----------------------------------------------------------------------------
HRESULT C3DXView::App_Render( LPDIRECT3DDEVICE7 pd3dDevice )
{
	if(pd3dDevice == NULL ){//&& ((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		return E_INVALIDARG;
	}

	if(!m_printimage){
		GetClientRect(m_screenrect);
	}
    // Clear the viewport and z-buffer
	DWORD dwRenderWidth;
	DWORD dwRenderHeight;
	
	HRESULT hr;
	CString text;
	//CFont font;
	HFONT font;
	HFONT oldfont;
//	D3DVERTEX v[3];
	CString str;

	HDC hdc;
	long err;

	BYTE quality = m_pDoc->m_lfCells.lfQuality;
	m_pDoc->m_lfCells.lfQuality |= ANTIALIASED_QUALITY;
	font = CreateFontIndirect(&m_pDoc->m_lfCells);
	m_pDoc->m_lfCells.lfQuality = quality;

    // Begin the scene
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		if( FAILED( pd3dDevice->BeginScene() ) )
			return E_FAIL;
	//}

	{
		//draw info & legend
		if(!m_printimage && !m_saveimage)
			GetClientRect(&g_rcScreenRect );

		D3DVIEWPORT7 vp2 = { 0, 0, g_rcScreenRect.right,g_rcScreenRect.bottom, 0.0f, 1.0f };;
		
		
		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
			hr = g_pd3dDevice->SetViewport( &vp2 );
			if( FAILED( hr ) )
				return hr;
		
			err = pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
				0x00ffffff, 1.0f, 0L );
				
		//}else{
		//	GetDC()->FillSolidRect(0,0,g_rcScreenRect.right,g_rcScreenRect.bottom,0x00111111);
		//	hdc = GetDC()->GetSafeHdc();
		//}

		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1)
			err = g_pd3dDevice->EndScene();	

		//draw object space
		if(!m_printimage  && !m_saveimage)
			GetClientRect(&g_rcScreenRect );

		dwRenderWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
		dwRenderHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;
		if(dwRenderWidth-m_infowidth > dwRenderHeight)
			dwRenderWidth = dwRenderHeight;
		else
			dwRenderHeight = dwRenderWidth-m_infowidth;

		D3DVIEWPORT7 vp;
		vp.dwX = m_infowidth;vp.dwY = 0;vp.dwHeight = dwRenderHeight;
		vp.dvMinZ = .0f;vp.dvMaxZ = 1.0f;
		if(!m_printimage && (!m_saveimage || m_Legend)){
			//vp = { 120, 0, dwRenderWidth, dwRenderHeight, 0.0f, 1.0f };
			vp.dwWidth = dwRenderWidth;
			if(vp.dwWidth > vp.dwHeight){
				//vp.dwY = (vp.dwWidth-vp.dwHeight)/2;
				vp.dwWidth = vp.dwHeight;
			}
		}else{
			//vp = { 120, 0, dwRenderWidth-120, dwRenderHeight, 0.0f, 1.0f };
			vp.dwWidth = dwRenderWidth-m_infowidth;

			if(vp.dwWidth > vp.dwHeight){
			//	vp.dwY = (vp.dwWidth-vp.dwHeight)/2;
				vp.dwWidth = vp.dwHeight;
			}
		}

		

		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
			hr = g_pd3dDevice->SetViewport( &vp );			
			//g_pddsBackBuffer->GetDC(&hdc);
		//}else{
		//	m_gdi_offsetx = m_infowidth;
		//}

		//SetBkMode(hdc,TRANSPARENT);
		hdc = NULL;

		if(m_Symbols || m_Labels){		
			hr = g_pd3dDevice->SetViewport( &vp );			
			g_pddsBackBuffer->GetDC(&hdc);
			SetBkMode(hdc,TRANSPARENT);
		}

		if(!m_legend_instance)
		{
//			g_pd3dDevice->BeginScene();
			DrawObjects(hdc);	
//			g_pd3dDevice->EndScene();
//			g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			//return TRUE;

		}

		if(!m_Symbols && !m_Labels){		
			hr = g_pd3dDevice->SetViewport( &vp );			
			g_pddsBackBuffer->GetDC(&hdc);
			SetBkMode(hdc,TRANSPARENT);
		}


		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		//	hr = g_pd3dDevice->SetViewport( &vp );			
		//	g_pddsBackBuffer->GetDC(&hdc);
		//}

		
 		oldfont = (HFONT) SelectObject(hdc,font);

		SetTextColor(hdc,m_pDoc->m_rgbFontCells);

		if(m_selectedobject != m_lastselectedobject) {
			m_lasttime = ::GetTickCount();
			m_lastselectedobject = m_selectedobject;
		} else if(::GetTickCount() - 500 >= m_lasttime && m_selectedobject >= 0) {
			
			long height = (long)fabs((double)m_pDoc->m_lfCells.lfHeight);

			MoveToEx(hdc,0,dwRenderHeight - height*2 - 10,NULL);
			LineTo(hdc,m_infowidth,dwRenderHeight - height*2 - 10);

			m_pDoc->GetRowLabel(m_selectedobject,text);
			TextOut(hdc,m_gdi_offsetx,dwRenderHeight-height * 2-5,text,text.GetLength());

			if (m_colColourMap >= 0) {
				
				m_pDoc->GetColumnLabel(m_colColourMap,text);

				CString s;
				s.Format("%s: %.4f",text,m_pDoc->GetValueAt(m_colColourMap, m_selectedobject));

				TextOut(hdc,m_gdi_offsetx,dwRenderHeight-height-5,s,s.GetLength());
			}
		}

		if((m_legend_instance || m_saveimage) && !m_printimage )
		{
			DrawLegend(hdc);
		}
		
		if(!m_legend_instance)
		{
			DrawPcc(hdc);

			if (m_Showranges) { 
				DrawRanges(hdc);
			}
			
			DrawCompare(hdc);		

			//text out
	//		g_pd3dDevice->settexture
	//		g_pddsBackBuffer->

			if(!m_printimage && m_settingrotate){
				//pDC->MoveTo(m_oldpoint);
				//pDC->LineTo(m_rotatepoint);

				float tz,ty,tx;
				App_Corner_Reverse(g_pd3dDevice,&tx,&ty,&tz,(float)(m_oldpoint.x-m_infowidth),(float)(m_oldpoint.y),0);
				m_line2[0].x = tx;
				m_line2[0].y = ty;
				m_line2[0].z = tz;

				App_Corner_Reverse(g_pd3dDevice,&tx,&ty,&tz,(float)(m_rotatepoint.x-m_infowidth),(float)(m_rotatepoint.y),0);
				m_line2[1].x = tx;
				m_line2[1].y = ty;
				m_line2[1].z = tz;

				if(m_line2 != NULL)
					g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,m_line2, 2, NULL );

			}
			
			SetTextColor(hdc,m_pDoc->m_rgbFontCells);
			DrawInfoBar(hdc);		
			
			if(m_Legend && !m_printimage){
	//			DrawLegend(hdc);
			}		
			
			
			DrawTextBoxes(hdc);	
		}
		if(oldfont != NULL)
			SelectObject(hdc,oldfont);

		DeleteObject(font);
		
	//	if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
			err = g_pddsBackBuffer->ReleaseDC(hdc);
	//	}

		//draw object space - FOR HIT TESTING
		if(!m_printimage)
			GetClientRect(&g_rcScreenRect );
		dwRenderWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
		dwRenderHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;
		if(dwRenderWidth-m_infowidth > dwRenderHeight)
			dwRenderWidth = dwRenderHeight;
		else
			dwRenderHeight = dwRenderWidth-m_infowidth;
	
	}

	// End the scene.
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
//		pd3dDevice->EndScene();
	//}
	
	
//	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );


    return S_OK;
}

HRESULT C3DXView::CreateEverything( HWND hWnd )
{
	// Cleanup any objects that might've been created before
	if( FAILED( Cleanup3DEnvironment() ) )
		return E_FAIL;
				
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		// Create the D3D environment, at first, trying the HAL
		if( SUCCEEDED( Initialize3DEnvironment( hWnd, NULL,
												&IID_IDirect3DHALDevice ) ) )
			return S_OK;

		// Else, cleanup objects potentially created during the failed
		// initialization attempt.
		Cleanup3DEnvironment();

		if( SUCCEEDED( Initialize3DEnvironment( hWnd, NULL,
												&IID_IDirect3DRGBDevice ) ) )
			return S_OK;
		
		// Else, return failure. This simple tutorial will exit ungracefully.
		return E_FAIL;
	//}else{
	//	Initialize3DEnvironment( hWnd, NULL,&IID_IDirect3DHALDevice );
	//	return S_OK;
	//}
}

//-----------------------------------------------------------------------------
// Name: EnumZBufferCallback()
// Desc: Enumeration function to report valid pixel formats for z-buffers.
//-----------------------------------------------------------------------------
HRESULT WINAPI C3DXView::EnumZBufferCallback( DDPIXELFORMAT* pddpf,
                                           VOID* pddpfDesired )
{
	// For this tutorial, we are only interested in z-buffers, so ignore any
	// other formats (e.g. DDPF_STENCILBUFFER) that get enumerated. An app
	// could also check the depth of the z-buffer (16-bit, etc,) and make a
	// choice based on that, as well. For this tutorial, we'll take the first
	// one we get.
    if( pddpf->dwFlags == DDPF_ZBUFFER )
    {
        memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );

        // Return with D3DENUMRET_CANCEL to end the search.
		return D3DENUMRET_CANCEL;
    }

    // Return with D3DENUMRET_OK to continue the search.
    return D3DENUMRET_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize3DEnvironment()
// Desc: This function initializes all the DirectDraw/Direct3D objects used for
//       3D-rendering. This code is expanded from the Step 1 tutorial, in that
//       it adds a z-buffer.
//-----------------------------------------------------------------------------
HRESULT C3DXView::Initialize3DEnvironment( HWND hWnd, GUID* pDriverGUID,
								 const GUID* pDeviceGUID )
{
	HRESULT hr;

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		// Create the IDirectDraw interface. The first parameter is the GUID,
		// which is allowed to be NULL. If there are more than one DirectDraw
		// drivers on the system, a NULL guid requests the primary driver. For 
		// non-GDI hardware cards like the 3DFX and PowerVR, the guid would need
		// to be explicity specified . (Note: these guids are normally obtained
		// from enumeration, which is convered in a subsequent tutorial.)
		hr = MDirectDrawCreateEx( pDriverGUID, (VOID**)&g_pDD, IID_IDirectDraw7,
								 NULL );
		if(FAILED(hr))
			return hr;

		if(true || max_x == 0)
		{
			g_pDD->EnumDisplayModes(0,0,0,(LPDDENUMMODESCALLBACK2)EnumDisplayModesCallback);
			g_pDD->EnumSurfaces(0,0,0,(LPDDENUMSURFACESCALLBACK7)EnumSurfacesCallback);
		}

		// Set the Windows cooperative level. This is where we tell the system
		// whether wew will be rendering in fullscreen mode or in a window. Note
		// that some hardware (non-GDI) may not be able to render into a window.
		// The flag DDSCL_NORMAL specifies windowed mode. Using fullscreen mode
		// is the topic of a subsequent tutorial. The DDSCL_FPUSETUP flag is a 
		// hint to DirectX to optomize floating points calculations. See the docs
		// for more info on this. Note: this call could fail if another application
		// already controls a fullscreen, exclusive mode.
		hr = g_pDD->SetCooperativeLevel( hWnd, DDSCL_NORMAL );
		if( FAILED( hr ) )
			return hr;

		if(true || max_x == 0)
		{
			g_pDD->EnumDisplayModes((DWORD)g_pDD,0,0,(LPDDENUMMODESCALLBACK2)EnumDisplayModesCallback);
			g_pDD->EnumSurfaces((DWORD)g_pDD,0,0,(LPDDENUMSURFACESCALLBACK7)EnumSurfacesCallback);
		}

		// Initialize a surface description structure for the primary surface. The
		// primary surface represents the entire display, with dimensions and a
		// pixel format of the display. Therefore, none of that information needs
		// to be specified in order to create the primary surface.


		DDSURFACEDESC2 ddsd;
		ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
		ddsd.dwSize         = sizeof(DDSURFACEDESC2);
		ddsd.dwFlags        = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		// Create the primary surface.
		hr = g_pDD->CreateSurface( &ddsd, &g_pddsPrimary, NULL );
		if( FAILED( hr ) )
			return hr;

		g_pDD->EnumSurfaces((DWORD)g_pDD,0,0,(LPDDENUMSURFACESCALLBACK7)EnumSurfacesCallback);

		// Setup a surface description to create a backbuffer. This is an
		// offscreen plain surface with dimensions equal to our window size.
		// The DDSCAPS_3DDEVICE is needed so we can later query this surface
		// for an IDirect3DDevice interface.
		ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

		// Set the dimensions of the backbuffer. Note that if our window changes
		// size, we need to destroy this surface and create a new one.
		GetClientRect(&g_rcScreenRect );
		//if(m_printimage) 
		//	g_rcScreenRect = m_rectDraw;
		ClientToScreen((POINT*)&g_rcScreenRect.left );
		ClientToScreen((POINT*)&g_rcScreenRect.right );
		ddsd.dwWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
		ddsd.dwHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;

		//if(ddsd.dwWidth-120 > ddsd.dwHeight)
			//ddsd.dwWidth = ddsd.dwHeight;
		//else
		//	ddsd.dwHeight = ddsd.dwWidth-120;

		// Create the backbuffer. The most likely reason for failure is running
		// out of video memory. (A more sophisticated app should handle this.)
		hr = g_pDD->CreateSurface( &ddsd, &g_pddsBackBuffer, NULL );
		if( FAILED( hr ) )
			return hr;

		// Note: if using a z-buffer, the zbuffer surface creation would go around
		// here. However, z-buffer usage is the topic of a subsequent tutorial.

		// Create a clipper object which handles all our clipping for cases when
		// our window is partially obscured by other windows. This is not needed
		// for apps running in fullscreen mode.
		LPDIRECTDRAWCLIPPER pcClipper;
		hr = g_pDD->CreateClipper( 0, &pcClipper, NULL );
		if( FAILED( hr ) )
			return hr;

		// Associate the clipper with our window. Note that, afterwards, the
		// clipper is internally referenced by the primary surface, so it is safe
		// to release our local reference to it.
		pcClipper->SetHWnd( 0, hWnd );
		g_pddsPrimary->SetClipper( pcClipper );
		pcClipper->Release();

		// Query DirectDraw for access to Direct3D
		g_pDD->QueryInterface( IID_IDirect3D7, (VOID**)&g_pD3D );
		if( FAILED( hr) )
			return hr;

		// Enumerate the various z-buffer formats, finding a DDPIXELFORMAT
		// to use to create the z-buffer surface.
		DDPIXELFORMAT ddpfZBuffer;
		g_pD3D->EnumZBufferFormats( *pDeviceGUID, 
									EnumZBufferCallback, (VOID*)&ddpfZBuffer );

		// If we found a good zbuffer format, then the dwSize field will be
		// properly set during enumeration. Else, we have a problem and will exit.
		if( sizeof(DDPIXELFORMAT) != ddpfZBuffer.dwSize )
			return E_FAIL;

		// Get z-buffer dimensions from the render target
		// Setup the surface desc for the z-buffer.
		ddsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
		ddsd.dwWidth        = g_rcScreenRect.right - g_rcScreenRect.left;
		ddsd.dwHeight       = g_rcScreenRect.bottom - g_rcScreenRect.top;
		//if(ddsd.dwWidth-120 > ddsd.dwHeight)
		//	ddsd.dwWidth = ddsd.dwHeight;
		//else
		//	ddsd.dwHeight = ddsd.dwWidth-120;
		memcpy( &ddsd.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT) );

		// For hardware devices, the z-buffer should be in video memory. For
		// software devices, create the z-buffer in system memory
		if( IsEqualIID( *pDeviceGUID, IID_IDirect3DHALDevice ) )
			ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		else
			ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

		// Create and attach a z-buffer. Real apps should be able to handle an
		// error here (DDERR_OUTOFVIDEOMEMORY may be encountered). For this 
		// tutorial, though, we are simply going to exit ungracefully.
		if( FAILED( hr = g_pDD->CreateSurface( &ddsd, &g_pddsZBuffer, NULL ) ) )
			return hr;

		// Attach the z-buffer to the back buffer.
		if( FAILED( hr = g_pddsBackBuffer->AddAttachedSurface( g_pddsZBuffer ) ) )
			return hr;

		// Before creating the device, check that we are NOT in a palettized
		// display. That case will cause CreateDevice() to fail, since this simple 
		// tutorial does not bother with palettes.
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		g_pDD->GetDisplayMode( &ddsd );
		if( ddsd.ddpfPixelFormat.dwRGBBitCount <= 8 )
			return DDERR_INVALIDMODE;

		// Create the device. The device is created off of our back buffer, which
		// becomes the render target for the newly created device. Note that the
		// z-buffer must be created BEFORE the device
		if( FAILED( hr = g_pD3D->CreateDevice( *pDeviceGUID, g_pddsBackBuffer,
											   &g_pd3dDevice ) ) )
		{
			// This call could fail for many reasons. The most likely cause is
			// that we specifically requested a hardware device, without knowing
			// whether there is even a 3D card installed in the system. Another
			// possibility is the hardware is incompatible with the current display
			// mode (the correct implementation would use enumeration for this.)
			int i;
			switch(hr){
			case DD_OK:
				i = 0;break;
			case DDERR_ALREADYINITIALIZED:
				i = 0;break;
			case DDERR_BLTFASTCANTCLIP :
				i = 0;break;
			case DDERR_CANNOTATTACHSURFACE :
				i = 0;break;
			case DDERR_CANNOTDETACHSURFACE :
				i = 0;break;
			case DDERR_CANTCREATEDC :
				i = 0;break;
			case DDERR_CANTDUPLICATE :
				i = 0;break;
			case DDERR_CANTLOCKSURFACE :
				i = 0;break;
			case DDERR_CANTPAGELOCK :
				i = 0;break;
			case DDERR_CANTPAGEUNLOCK :
				i = 0;break; 
			case DDERR_CLIPPERISUSINGHWND :
				i = 0;break; 
			case DDERR_COLORKEYNOTSET :
				i = 0;break; 
			case DDERR_CURRENTLYNOTAVAIL :
				i = 0;break; 
			case DDERR_DDSCAPSCOMPLEXREQUIRED :
				i = 0;break; 
			case DDERR_DCALREADYCREATED :
				i = 0;break; 
			case DDERR_DEVICEDOESNTOWNSURFACE :
				i = 0;break; 
			case DDERR_DIRECTDRAWALREADYCREATED :
				i = 0;break; 
			case DDERR_EXCEPTION :
				i = 0;break; 
			case DDERR_EXCLUSIVEMODEALREADYSET :
				i = 0;break; 
			case DDERR_EXPIRED :
				i = 0;break; 
			case DDERR_GENERIC :
				i = 0;break; 
			case DDERR_HEIGHTALIGN :
				i = 0;break; 
			case DDERR_HWNDALREADYSET :
				i = 0;break; 
			case DDERR_HWNDSUBCLASSED :
				i = 0;break; 
			case DDERR_IMPLICITLYCREATED :
				i = 0;break; 
			case DDERR_INCOMPATIBLEPRIMARY :
				i = 0;break; 
			case DDERR_INVALIDCAPS :
				i = 0;break; 
			case DDERR_INVALIDCLIPLIST :
				i = 0;break; 
			case DDERR_INVALIDDIRECTDRAWGUID :
				i = 0;break; 
			case DDERR_INVALIDMODE :
				i = 0;break;
			case DDERR_INVALIDOBJECT :
				i = 0;break;
			case DDERR_INVALIDPARAMS :
				i = 0;break;
			case DDERR_INVALIDPIXELFORMAT :
				i = 0;break;
			case DDERR_INVALIDPOSITION :
				i = 0;break;
			case DDERR_INVALIDRECT :
				i = 0;break;
			case DDERR_INVALIDSTREAM :
				i = 0;break;
			case DDERR_INVALIDSURFACETYPE :
				i = 0;break;
			case DDERR_LOCKEDSURFACES :
				i = 0;break;
			case DDERR_MOREDATA :
				i = 0;break;
			case DDERR_NEWMODE :
				i = 0;break;
			case DDERR_NO3D :
				i = 0;break;
			case DDERR_NOALPHAHW :
				i = 0;break;
			case DDERR_NOBLTHW :
				i = 0;break; 
			case DDERR_NOCLIPLIST :
				i = 0;break; 
			case DDERR_NOCLIPPERATTACHED :
				i = 0;break;
			case DDERR_NOCOLORCONVHW :
				i = 0;break; 
			case DDERR_NOCOLORKEY :
				i = 0;break; 
			case DDERR_NOCOLORKEYHW :
				i = 0;break; 
			case DDERR_NOCOOPERATIVELEVELSET :
				i = 0;break; 
			case DDERR_NODC :
				i = 0;break; 
			case DDERR_NODDROPSHW :
				i = 0;break; 
			case DDERR_NODIRECTDRAWHW :
				i = 0;break; 
			case DDERR_NODIRECTDRAWSUPPORT :
				i = 0;break;
			case DDERR_NODRIVERSUPPORT :
				i = 0;break; 
			case DDERR_NOEMULATION :
				i = 0;break; 
			case DDERR_NOEXCLUSIVEMODE :
				i = 0;break; 
			case DDERR_NOFLIPHW :
				i = 0;break; 
			case DDERR_NOFOCUSWINDOW :
				i = 0;break; 
			case DDERR_NOGDI :
				i = 0;break; 
			case DDERR_NOHWND :
				i = 0;break; 
			case DDERR_NOMIPMAPHW :
				i = 0;break; 
			case DDERR_NOMIRRORHW :
				i = 0;break; 
			case DDERR_NOMONITORINFORMATION :
				i = 0;break; 
			case DDERR_NONONLOCALVIDMEM :
				i = 0;break; 
			case DDERR_NOOPTIMIZEHW :
				i = 0;break; 
			case DDERR_NOOVERLAYDEST :
				i = 0;break; 
			case DDERR_NOOVERLAYHW :
				i = 0;break; 
			case DDERR_NOPALETTEATTACHED :
				i = 0;break; 
			case DDERR_NOPALETTEHW :
				i = 0;break; 
			case DDERR_NORASTEROPHW :
				i = 0;break;
			case DDERR_NOROTATIONHW :
				i = 0;break; 
			case DDERR_NOSTEREOHARDWARE :
				i = 0;break;
			case DDERR_NOSTRETCHHW :
				i = 0;break;
			case DDERR_NOSURFACELEFT :
				i = 0;break;
			case DDERR_NOT4BITCOLOR :
				i = 0;break;
			case DDERR_NOT4BITCOLORINDEX :
				i = 0;break;
			case DDERR_NOT8BITCOLOR :
				i = 0;break; 
			case DDERR_NOTAOVERLAYSURFACE :
				i = 0;break;
			case DDERR_NOTEXTUREHW :
				i = 0;break;
			case DDERR_NOTFLIPPABLE :
				i = 0;break; 
			case DDERR_NOTFOUND :
				i = 0;break;
			case DDERR_NOTINITIALIZED :
				i = 0;break;
			case DDERR_NOTLOADED :
				i = 0;break;
			case DDERR_NOTLOCKED :
				i = 0;break;
			case DDERR_NOTPAGELOCKED :
				i = 0;break;
			case DDERR_NOTPALETTIZED :
				i = 0;break;
			case DDERR_NOVSYNCHW :
				i = 0;break;
			case DDERR_NOZBUFFERHW :
				i = 0;break;
			case DDERR_NOZOVERLAYHW :
				i = 0;break;
			case DDERR_OUTOFCAPS :
				i = 0;break;
			case DDERR_OUTOFMEMORY :
				i = 0;break; 
			case DDERR_OUTOFVIDEOMEMORY :
				i = 0;break;
			case DDERR_OVERLAPPINGRECTS :
				i = 0;break;
			case DDERR_OVERLAYCANTCLIP :
				i = 0;break;
			case DDERR_OVERLAYCOLORKEYONLYONEACTIVE :
				i = 0;break;
			case DDERR_OVERLAYNOTVISIBLE :
				i = 0;break;
			case DDERR_PALETTEBUSY :
				i = 0;break;
			case DDERR_PRIMARYSURFACEALREADYEXISTS :
				i = 0;break;
			case DDERR_REGIONTOOSMALL :
				i = 0;break; 
			case DDERR_SURFACEALREADYATTACHED :
				i = 0;break;
			case DDERR_SURFACEALREADYDEPENDENT :
				i = 0;break;
			case DDERR_SURFACEBUSY :
				i = 0;break;
			case DDERR_SURFACEISOBSCURED :
				i = 0;break;
			case DDERR_SURFACELOST :
				i = 0;break;
			case DDERR_SURFACENOTATTACHED :
				i = 0;break;
			case DDERR_TESTFINISHED :
				i = 0;break;
			case DDERR_TOOBIGHEIGHT :
				i = 0;break;
			case DDERR_TOOBIGSIZE :
				i = 0;break;
			case DDERR_TOOBIGWIDTH :
				i = 0;break;
			case DDERR_UNSUPPORTED :
				i = 0;break;
			case DDERR_UNSUPPORTEDFORMAT :
				i = 0;break;
			case DDERR_UNSUPPORTEDMASK :
				i = 0;break;
			case DDERR_UNSUPPORTEDMODE :
				i = 0;break;
			case DDERR_VERTICALBLANKINPROGRESS :
				i = 0;break;
			case DDERR_VIDEONOTACTIVE :
				i = 0;break; 
			case DDERR_WASSTILLDRAWING :
				i = 0;break;
			case DDERR_WRONGMODE :
				i = 0;break;
			case DDERR_XALIGN :
				i = 0;break;
			default:
				i = 0;break;
			}
			return hr;
		}
		g_pd3dDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS,D3DANTIALIAS_SORTINDEPENDENT );
		g_pd3dDevice->SetRenderState(D3DRENDERSTATE_EDGEANTIALIAS,TRUE);

		// Create the viewport
		DWORD dwRenderWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
		DWORD dwRenderHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;
		//if(dwRenderWidth-120 > dwRenderHeight)
		//	dwRenderWidth = dwRenderHeight;
		//else
		//	dwRenderHeight = dwRenderWidth-120;
		D3DVIEWPORT7 vp = { 0, 0, dwRenderWidth, dwRenderHeight, 0.0f, 1.0f };
		hr = g_pd3dDevice->SetViewport( &vp );
		if( FAILED( hr ) )
			return hr;

		// Finish by setting up our scene
		return App_InitDeviceObjects( g_pd3dDevice );
	//}else{
	//	return App_InitDeviceObjects( g_pd3dDevice );
	//}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Cleanup3DEnvironment()
// Desc: Releases all the resources used by the app. Note the check for
//       reference counts when releasing the D3DDevice and DDraw objects. If
//       these ref counts are non-zero, then something was not cleaned up
//       correctly.
//-----------------------------------------------------------------------------
HRESULT C3DXView::Cleanup3DEnvironment()
{
	// Cleanup any objects created for the scene
	//App_DeleteDeviceObjects( g_pd3dDevice );

//	if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		// Release the DDraw and D3D objects used by the app
		if( g_pddsZBuffer )    g_pddsZBuffer->Release();
		if( g_pD3D )           g_pD3D->Release();
		if( g_pddsBackBuffer ) g_pddsBackBuffer->Release();
		if( g_pddsPrimary )    g_pddsPrimary->Release();

		// Do a safe check for releasing the D3DDEVICE. RefCount should be zero.
		if( g_pd3dDevice )
			if( 0 < g_pd3dDevice->Release() )
				return E_FAIL;

		// Do a safe check for releasing DDRAW. RefCount should be zero.
		if( g_pDD )
			if( 0 < g_pDD->Release() )
				return E_FAIL;
//	}

    g_pddsZBuffer    = NULL;
	g_pd3dDevice     = NULL;
	g_pD3D           = NULL;
	g_pddsBackBuffer = NULL;
	g_pddsPrimary    = NULL;
	g_pDD            = NULL;

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render3DEnvironment()
// Desc: Draws the scene. There are three steps here:
//       (1) Animate the scene
//       (2) Render the scene
//       (3) Show the frame (copy backbuffer contents to the primary).
//-----------------------------------------------------------------------------
HRESULT C3DXView::Render3DEnvironment()
{
	// Call the app specific function to framemove (animate) the scene
	App_FrameMove( g_pd3dDevice, (m_time++)/100.0f );

	//fix light
	m_light.dvDirection.x =  -(m_vRight.x + m_vUpVec.x + m_vEyePt.x)/3.0f;
 	m_light.dvDirection.y = -(m_vRight.y + m_vUpVec.y + m_vEyePt.y)/3.0f;
	m_light.dvDirection.z = -(m_vRight.z + m_vUpVec.z + m_vEyePt.z)/3.0f;

	g_pd3dDevice->LightEnable( 0, FALSE );
	g_pd3dDevice->SetLight( 0, &m_light );
	g_pd3dDevice->LightEnable( 0, TRUE );

    // Call the app specific function to render the scene
    App_Render( g_pd3dDevice  );
    
    // Show the frame on the primary surface. Note: this is the best place to
	// check for "lost" surfaces. Surfaces can be lost if something caused
	// them to temporary lose their video memory. "Lost" surfaces simply
	// need to be restored before continuing.
    if( DDERR_SURFACELOST == ShowFrame() )
		RestoreSurfaces();

	RestoreSurfaces();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ShowFrame()
// Desc: Show the frame on the primary surface
//-----------------------------------------------------------------------------
HRESULT C3DXView::ShowFrame()
{
	if( NULL == g_pddsPrimary )
		return E_FAIL;

    // We are in windowed mode, so perform a blit from the backbuffer to the
	// correct position on the primary surface
	GetClientRect(&g_rcScreenRect );
	ClientToScreen((POINT*)&g_rcScreenRect.left );
	ClientToScreen((POINT*)&g_rcScreenRect.right );

	long height = g_rcScreenRect.bottom - g_rcScreenRect.top;
	long width = g_rcScreenRect.right - g_rcScreenRect.left;
	/*if(width-120 > height){
		g_rcScreenRect.right = g_rcScreenRect.left + height;
	}else{
		g_rcScreenRect.bottom = g_rcScreenRect.top + width-120;
	}
	g_rcScreenRect.left += 120;
	g_rcScreenRect.right += 120;

	
	if(width-120 > height)
		width = height;
	else
		height = width-120;
*/
//	if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		return g_pddsPrimary->Blt( &g_rcScreenRect, g_pddsBackBuffer, 
			NULL, DDBLT_WAIT, NULL );
//	}else{
//		return S_OK;
//	}
}


//-----------------------------------------------------------------------------
// Name: RestoreSurfaces()
// Desc: Checks for lost surfaces and restores them if lost.
//-----------------------------------------------------------------------------
HRESULT C3DXView::RestoreSurfaces()
{
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		// Check/restore the primary surface
		if( g_pddsPrimary )
			if( g_pddsPrimary->IsLost() )
				g_pddsPrimary->Restore();
    
		// Check/restore the back buffer
		if( g_pddsBackBuffer )
			if( g_pddsBackBuffer->IsLost() )
				g_pddsBackBuffer->Restore();

		// Check/restore the z-buffer
		if( g_pddsZBuffer )
			if( g_pddsZBuffer->IsLost() )
				g_pddsZBuffer->Restore();
//	}

    return S_OK;
}





/////////////////////////////////////////////////////////////////////////////
// C3DXView


IMPLEMENT_DYNCREATE(C3DXView, CViewExtension)

C3DXView::C3DXView()
{
	m_firsttime = true;
	m_mst = NULL;
	m_mst_size = 0;
	m_pcc = NULL;
	m_line = NULL;
	m_line2 = NULL;
	m_pcc_size = 0;
	DDHinst = 0;
	m_viewtype = 0;
	m_selectedobject = -1;
	m_selectedgroup = -1;
	m_colColourMap = -1;
}

C3DXView::~C3DXView()
{
	POSITION pos = m_pDoc->GetFirstViewPosition();   
	while (pos != NULL)   
	{
		CView* pView = m_pDoc->GetNextView(pos);   
		if(pView->IsKindOf(RUNTIME_CLASS(C3DXView)) && ((C3DXView*)pView)->m_legend_view==(void*)this)
		{
			((C3DXView*)pView)->m_legend_view = NULL;
			((C3DXView*)pView)->m_Legend = false;
			((C3DXView*)pView)->m_menu.CheckMenuItem(ID__DISPLAYLEGEND,MF_UNCHECKED);					
			break;
		}		
	}

	if(m_legend_view && m_legend_view != (void*)1)
	{
		((CView*)m_legend_view)->GetParentFrame()->CloseWindow();
	}
	


	long i;
	
	for(i=0;i<m_TextBoxes.GetSize();i++){
		delete m_TextBoxes[i];
	}
	
	m_pDoc->m_OrdXVisible = 0;
	if(!m_pDoc->m_LoadingXOrd)
		m_pDoc->mr_ord.bottom = -1;

	if(m_mst != NULL){
		delete [] m_mst;
	}
	if(m_pcc != NULL){
		delete [] m_pcc;
	}
	if(m_line != NULL){
		delete [] m_line;
	}
	if(m_line2 != NULL){
		delete [] m_line2;
	}

	{//dynamically get access to function DirectDrawCreateEx - it is not present
		// in earlier versions of dx - code from msdn
		if(DDHinst != 0)
			FreeLibrary( DDHinst );		
	}
}


BEGIN_MESSAGE_MAP(C3DXView, CViewExtension)
	//{{AFX_MSG_MAP(C3DXView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID__DISPLAYMST, OnDisplaymst)
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID__DISPLAYCENTROIDS, OnDisplaycentroids)
	ON_COMMAND(ID__SHOWGROUPING, OnShowgrouping)
	ON_COMMAND(ID__SHOWRANGES, OnShowranges)
	ON_COMMAND(ID__SHOWSELECTEDONLY, OnShowselectedonly)
	ON_COMMAND(ID__USEUSERGROUPCOLOURS, OnUseusergroupcolours)
	ON_COMMAND(ID__SELECTPCCVECTORS, OnSelectpccvectors)
	ON_COMMAND(ID__RESETROTATION, OnResetRotation)
	ON_UPDATE_COMMAND_UI(ID__RESETROTATION, OnUpdateResetRotation)
	ON_UPDATE_COMMAND_UI(ID__SELECTPCCVECTORS, OnUpdateSelectpccvectors)
	ON_COMMAND(ID__CLOSE, OnClose)
	ON_COMMAND(ID__ADDTEXTBOX, OnAddtextbox)
	ON_COMMAND(ID__DISPLAYLEGEND, OnDisplaylegend)
	ON_COMMAND(ID__USEBLACKWHITESYMBOLS, OnUseblackwhitesymbols)
	ON_UPDATE_COMMAND_UI(ID__USEBLACKWHITESYMBOLS, OnUpdateUseblackwhitesymbols)
	ON_COMMAND(ID__DISPLAYLABELS, OnDisplaylabels)
	ON_COMMAND(ID__FONT, OnFont)
	ON_COMMAND(ID__ROTATE, OnRotate)
	ON_UPDATE_COMMAND_UI(ID__ROTATE, OnUpdateRotate)
	ON_WM_TIMER()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID__MINUSROTATIONSPEED, OnMinusrotationspeed)
	ON_COMMAND(ID__PLUSROTATIONSPEED, OnPlusrotationspeed)
	ON_UPDATE_COMMAND_UI(ID__PRINTPREVIEW, OnUpdatePrintpreview)
	ON_UPDATE_COMMAND_UI(ID__PRINT, OnUpdatePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID__OBJECTSIZE, OnObjectsize)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID__SETROTATION_VIEWYZ, OnSetRotationyz)
	ON_COMMAND(ID__SETROTATION_VIEWXY, OnSetRotationxy)
	ON_COMMAND(ID__SETROTATION_VIEWXZ, OnSetRotationxz)
	ON_COMMAND(ID__ZOOM, OnZoom)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID__SAVEIMAGE, SaveIt)
	ON_COMMAND(ID__SAVEVIDEO, SaveAVI)
	ON_COMMAND(ID__PRINTPREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID__PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_GROUP_ANALYSISGROUPSANDCENTROIDS, OnGroupAnalysisgroupsandcentroids)
	ON_COMMAND(ID_GROUP_APRIORIGROUPSANDCENTROIDS, OnGroupApriorigroupsancentroids)
	ON_COMMAND(ID_SYMBOLSIZE_DECREASEPGDN, OnSymbolsizeDecreasepgdn)
	ON_COMMAND(ID_SYMBOLSIZE_INCREASEPGUP, OnSymbolsizeIncreasepgup)
	ON_UPDATE_COMMAND_UI(ID_GROUP_APRIORIGROUPSANDCENTROIDS, OnUpdateGroupApriorigroupsandcentroids)
	ON_UPDATE_COMMAND_UI(ID__USEUSERGROUPCOLOURS, OnUpdateUseusergroupcolours)
	ON_COMMAND(ID__PICKACOLUMN, OnPickacolumnButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DXView drawing

void C3DXView::OnDraw(CDC* pDC)
{
	//sync settings with legend
	if(m_Legend && m_legend_view && ((int)m_legend_view) != 1)
	{
		POSITION pos = m_pDoc->GetFirstViewPosition();   
		bool found = false;
		while (pos != NULL)   
		{
			if(m_pDoc->GetNextView(pos) == (CView*) m_legend_view)
			{
				found = true;
				break;
			}
		}

		if(found)
		{

			//perform sync
			C3DXView* v = (C3DXView*)m_legend_view;
			bool u = false;
			if(v->m_Grouping != m_Grouping){ u=true; v->m_Grouping = m_Grouping; };

			if(v->m_Centroid != m_Centroid){ u=true; v->m_Centroid = m_Centroid; };
			if(v->m_Usergroupcolours != m_Usergroupcolours){ u=true; v->m_Usergroupcolours = m_Usergroupcolours; };
			if(v->m_SelOnly != m_SelOnly){ u=true; v->m_SelOnly = m_SelOnly; };
			if(v->m_Symbols != m_Symbols){ u=true; v->m_Symbols = m_Symbols; };

					
			if(v->m_menu.GetMenuState(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_BYCOMMAND) != m_menu.GetMenuState(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_BYCOMMAND))
			{ 
				u=true; 
				v->m_menu.CheckMenuItem(
					ID_GROUP_APRIORIGROUPSANDCENTROIDS,
					m_menu.GetMenuState(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_BYCOMMAND)
					);
			};

			if(u)
			{
				v->RedrawWindow();
			}
		}
		else
		{
			m_legend_view = NULL;
		}
	}

	KillTimer(2);
	SetTimer(2,2000,NULL);

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		CRect rect;
		GetWindowRect(rect);

		if(g_avi_count > 0)
		{
			GetClientRect(rect);
			pDC->FillSolidRect(rect,0x00ffffff);
			pDC->TextOut(20,20,"Saving Video...");
			return;
		}

		drawratio = (rect.Width()) / (double) rect.Height();

		if(m_pDoc->m_OrdChange){
			GetClientRect(rect);
			pDC->FillSolidRect(rect,0x00ffffff);
			pDC->TextOut(20,20,"Ordination coordinates have changed.");
			return;
		}

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

		if(m_printimage)
			return;

		if(m_firsttime ){
			g_bReady = TRUE;

			if(FAILED(CreateEverything(GetSafeHwnd())))
			{
				Cleanup3DEnvironment();
				CRect r;
				GetWindowRect(r);
				GetDC()->FillSolidRect(r,0x00ffffff);
				GetDC()->TextOut(30,30,"An error occured. Close and reopen the ordination window.");
				GetDC()->TextOut(30,60,"If you are using more than one monitor keep the ordination window on the one screen.");

				}
			else
			{
				Render3DEnvironment();
			}
			m_firsttime  = false;

			UpdateZOrders();
		}
		//if(g_bReady == TRUE)
		//	ShowFrame();
		
		if(this->g_pd3dDevice == NULL && FAILED(CreateEverything(GetSafeHwnd())))
		{
			Cleanup3DEnvironment();
			CRect r;
				GetClientRect(r);
				pDC->FillSolidRect(r,0x00ffffff);
			
				pDC->TextOut(30,30,"An error occured. Close and reopen the ordination window.");
				pDC->TextOut(30,60,"If you are using more than one monitor keep the ordination window on the one screen.");
		}
		else
			Render3DEnvironment();

		
/*	}else{
		if(m_firsttime ){
			g_bReady = TRUE;

			CreateEverything(GetSafeHwnd());
			Render3DEnvironment();

			m_firsttime  = false;

			UpdateZOrders();
		}
		Render3DEnvironment();
		CRect rect;
		GetClientRect(rect);
		//pDC->FillSolidRect(rect,0x00ffffff);
		//DrawToMetaFile(2,"");
		//*
		//play g_emf
		DrawToMetaFile(0,"");		

		ENHMETAHEADER EnhMetaHdr;
		GetEnhMetaFileHeader(g_emf, sizeof(EnhMetaHdr), &EnhMetaHdr); 

		drawratio = m_drawres.x/(double)m_drawres.y;
		drawratio = EnhMetaHdr.rclBounds.right / (double)EnhMetaHdr.rclBounds.bottom; //preserve meta aspect
		if(rect.Width()/(double)rect.Height() < drawratio){
			rect.bottom = (long)(rect.top + rect.Width()/drawratio);
		}else{
			rect.right = (long)(rect.left + rect.Height()*drawratio);
		}
		PlayEnhMetaFile(pDC->GetSafeHdc(),g_emf,rect);
		DeleteEnhMetaFile(g_emf);//	
	}
*/
	CString str = "";
		
	if(m_viewtype == 1){
		str = "Orthographic View - Axies 1 and 2";
	}else if(m_viewtype == 2){
		str = "Orthographic View - Axies 2 and 3";
	}else if(m_viewtype == 3){
		str = "Orthographic View - Axies 1 and 3";
	}
	pDC->SetTextColor(m_pDoc->m_rgbFontCells);
	pDC->TextOut(120,30,str);
}

/////////////////////////////////////////////////////////////////////////////
// C3DXView diagnostics

#ifdef _DEBUG
void C3DXView::AssertValid() const
{
	CView::AssertValid();
}

void C3DXView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// C3DXView message handlers

int C3DXView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pDoc = (CPatnDoc*) GetDocument();

	m_legend_instance = false;

	POSITION pos = m_pDoc->GetFirstViewPosition();   
	int instance_count = 0;
	while (pos != NULL)   
	{
		CView* pView = m_pDoc->GetNextView(pos);   
		if(pView->IsKindOf(RUNTIME_CLASS(C3DXView)))
		{
			instance_count++;
		}		
	}   
	m_legend_view = NULL;
	if(instance_count % 2 == 0)
	{
		m_legend_instance = true;
		m_legend_view = (void*)1;
	}
	
	
	m_zoom = 0;

	m_legendPos = 0;
	m_legendHeight = 0;
	m_legendRect = CRect(0,0,0,0);
	m_legendX = 0;
	m_legendNumberOfRows = 10000;
	m_legendScale = 1;
	
	m_time = 0;
	g_avi_count = 0;

	//TWO MODES... GDI & DIRECTX7
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){//dynamically get access to function DirectDrawCreateEx - it is not present
		// in earlier versions of dx - code from msdn
		//HINSTANCE            DDHinst = 0; //now global
		//DIRECTDRAWCREATEEX   DirectDrawCreateEx = 0; //now global
		DDHinst = 0;
		MDirectDrawCreateEx = 0;
		// Now we know we are in Windows 9x (or maybe 3.1), so anything's possible.
		// First see if DDRAW.DLL even exists.
		DDHinst = LoadLibrary( "DDRAW.DLL" );
		if( DDHinst == 0 )	{
			FreeLibrary( DDHinst );
			return -1;
		}	
		// Check for DirectX 7 by creating a DDraw7 object
		MDirectDrawCreateEx = (DIRECTDRAWCREATEEX)GetProcAddress( DDHinst,"DirectDrawCreateEx" );
		if( NULL == MDirectDrawCreateEx ){
			FreeLibrary( DDHinst );
			return -1;
		}		
	//}

	long i;	
	g_pDD = NULL;
	g_pddsPrimary = NULL;
	g_pddsBackBuffer = NULL;
	g_pddsZBuffer = NULL;
	g_pD3D = NULL;
	g_pd3dDevice = NULL;

	m_infowidth = 120;

	g_bActive = false;
	g_bReady = false;

	m_glselect = false;
	m_togglerotate = false;
	m_Gstaattrib = 0;
	m_saveimage = false;
	m_printimage = false;
//	m_psd = NULL;
	m_waitforrefresh = false;
	m_settingrotate = false;
	
	
	if(m_pDoc == NULL)
		return -1;
	m_pDoc->m_OrdXVisible = 1;

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

	m_menu.LoadMenu(IDR_MENU_3DVIEW);
	
	//to correctly scale ordination axies
	float max_range = (float)(m_pDoc->m_OrdXmax - m_pDoc->m_OrdXmin);
	if(max_range < m_pDoc->m_OrdYmax - m_pDoc->m_OrdYmin)
		max_range = (float)(m_pDoc->m_OrdYmax - m_pDoc->m_OrdYmin);
	if(max_range < m_pDoc->m_OrdZmax - m_pDoc->m_OrdZmin)
		max_range = (float)(m_pDoc->m_OrdZmax - m_pDoc->m_OrdZmin);
	x_scale = (float)(m_pDoc->m_OrdXmax - m_pDoc->m_OrdXmin) / max_range;
	y_scale = (float)(m_pDoc->m_OrdYmax - m_pDoc->m_OrdYmin) / max_range;
	z_scale = (float)(m_pDoc->m_OrdZmax - m_pDoc->m_OrdZmin) / max_range;
    

	/*CString strError;
	CString patterns = ((CPatnApp*)AfxGetApp())->m_ProgramPath;
	patterns += "patterns\\patterns.dat";
	m_NoSymbols = false;
	if(!m_tex.LoadPatterns(patterns)){
		strError = "Failed to find file ";
		strError += patterns;
		strError += "\nBlack and White Symbols will be unavailable.";
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox(strError);
		m_NoSymbols = true;
	}*/
	m_NoSymbols = false;
	CString patterns;
	HRSRC rsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_TEXT1),"TEXT");
	HGLOBAL h = LoadResource(NULL,rsrc);
	char * c = (char*)h;
	i=0;//loop until "eee" occurs
	while(!(c[i] == 'e' && c[i+1] == 'e' && c[i+2] == 'e')){
		patterns.Insert(i,c[i]);
		i++;
	}
	patterns.Insert(i,'e');
	if(!m_tex.LoadPatterns(0,patterns)){
		m_NoSymbols = true;
	}

	for(i=0;i<m_pDoc->m_PatternObjects.GetSize();i++){
		if(m_pDoc->m_PatternObjects[i] == -1)
			m_pDoc->m_PatternObjects[i] = i;
	}
	CRect rect;
	GetClientRect(rect);
	m_SaveHeight = rect.Height();

	//init m_sphere
	InitSphere();
	InitSphere2();
	InitRing();

	if(m_zoom == 0)
		m_vEyePt = D3DVECTOR( 0, 0, -0.4f);
	else
	m_vEyePt = D3DVECTOR( 0, 0, -0.6f);
    m_vLookatPt = D3DVECTOR( 0,0,0);
    m_vUpVec    = D3DVECTOR( 0, 1.0f,0 );
	m_vRight = D3DVECTOR(1.0f,0,0);

	//vLookatPt = D3DVECTOR(0,0,0);
	//vEyePt = D3DVECTOR(2*sinf(fTimeKey),0,2*cosf(fTimeKey));

	//SetViewMatrix( m_matView, m_vEyePt, m_vLookatPt, m_vUpVec );
	if(m_viewtype == 0){
		SetViewMatrix( m_matView, m_vEyePt, m_vLookatPt, m_vUpVec );
	}else if(m_viewtype == 1){
		SetViewMatrix(m_matView,D3DVECTOR(0,0,-.5),D3DVECTOR(0,0,0),D3DVECTOR(0,.5,0));
	}else if(m_viewtype == 2){
		SetViewMatrix(m_matView,D3DVECTOR(.5,0,0),D3DVECTOR(-.5,0,0),D3DVECTOR(0,.5,0));
	}else if(m_viewtype == 3){
		SetViewMatrix(m_matView,D3DVECTOR(0,.5,0),D3DVECTOR(0,-.5,0),D3DVECTOR(0,0,.5));
	}

	
	m_rotatepoint.x = 100;
	m_rotatepoint.y = 0;
	
	//ordination stress
	//m_stressbox.font.lfHeight = -15;
	//m_stressbox.font.lfWidth = 0;
	//m_stressbox.font.lfEscapement = 0;
	//m_stressbox.font.lfOrientation = 0;
	//m_stressbox.font.lfWeight = 400;//FW_NORMAL;
	//m_stressbox.font.lfItalic = FALSE;
	//m_stressbox.font.lfUnderline = FALSE;
	//m_stressbox.font.lfStrikeOut = FALSE;
	//m_stressbox.font.lfCharSet = 0;//ANSI_CHARSET;
	//m_stressbox.font.lfOutPrecision = 3;//OUT_DEFAULT_PRECIS;
	//m_stressbox.font.lfClipPrecision = 2;//CLIP_DEFAULT_PRECIS;
	//m_stressbox.font.lfQuality = 1;//PROOF_QUALITY;
	//m_stressbox.font.lfPitchAndFamily = 18;//VARIABLE_PITCH | FF_SWISS;
	m_stressbox.font = m_pDoc->m_lfCells;
	strcpy(m_stressbox.font.lfFaceName,m_pDoc->m_lfCells.lfFaceName);
	m_stressbox.colour = 0; //black

	if(m_pDoc->m_OrdStress == m_pDoc->GetMissingValue())
		m_stressbox.contents.Format("");
	else
		m_stressbox.contents.Format("Stress: %.4f",m_pDoc->m_OrdStress);

	long newheight = (long)fabs((double)m_stressbox.font.lfHeight);
	long newlength = (long)(m_stressbox.contents.GetLength() * newheight / 2.0);
		
	m_stressbox.location.top = 0;
	m_stressbox.location.left = m_infowidth;
	m_stressbox.location.bottom = 0 + newheight;
	m_stressbox.location.right = m_infowidth + newlength;
	

	InitRanges();
	InitMst();
	InitPcc();
	InitLine();

	return 0;
}


void C3DXView::OnSize(UINT nType, int cx, int cy) 
{
	if(cy/(double)cx > 0.75)
	{
		cx = (int)(cy/0.75);
	}
	else if(cy/(double)cx < 0.75)
	{
		cy = (int)(0.75*cx);
	}

	CView::OnSize(nType, cx, cy);
	
	// Check to see if we are losing or gaining our window. Set the
	// active flag to match.
    if( SIZE_MAXHIDE==nType || SIZE_MINIMIZED==nType )
        g_bActive = FALSE;
	else g_bActive = TRUE;

    // A new window size will require a new backbuffer size. The
	// easiest way to achieve this is to release and re-create
	// everything. Note: if the window gets too big, we may run out
	// of video memory and need to exit. This simple app exits
	// without displaying error messages, but a real app would behave
	// itself much better.
    if( g_bActive && g_bReady )
	{
		g_bReady = FALSE;

		// Cleanup the environment and recreate everything
		if( FAILED( CreateEverything( this->GetSafeHwnd()) ) )
		{
			//DestroyWindow();
			CRect r;
			GetWindowRect(r);
			GetDC()->FillSolidRect(r,0x00ffffff);
			GetDC()->TextOut(30,30,"An error occured. Close and reopen the ordination window.");
			GetDC()->TextOut(30,60,"If you are using more than one monitor keep the ordination window on the one screen.");

			g_bReady = TRUE;

			return;
		}
		else
		{
			
g_bReady = TRUE;
			RedrawWindow();
		}
		
		
	}
	
}

void C3DXView::OnMove(int x, int y) 
{	

	DWORD dwWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
	DWORD dwHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;

	if(dwWidth > dwHeight)
		dwWidth = dwHeight;
	else
		dwHeight = dwWidth;

    SetRect( &g_rcScreenRect, x, y, x + dwWidth, y + dwHeight );

	Cleanup3DEnvironment();


	RedrawWindow();

	CView::OnMove(x, y);	
}


void C3DXView::OnDestroy() 
{

	Cleanup3DEnvironment();	

	if(m_legend_instance)
	{
				        POSITION pos = m_pDoc->GetFirstViewPosition();   
		int instance_found = 0;
		while (pos != NULL)   
		{
			CView* pView = m_pDoc->GetNextView(pos);   
			if(pView == (CView*)this)
			{				
			}
			else if(pView->IsKindOf(RUNTIME_CLASS(C3DXView)))
			{
				((C3DXView*)pView)->m_legend_view = NULL;
				((C3DXView*)pView)->m_Legend = false;
				//GetParentFrame()->PostMessage(WM_CLOSE);		
				break;
			}		
		}

	}

	CView::OnDestroy();


	
}

void C3DXView::DrawSphere(double x,double y,double z,double red,double green,double blue)
{
	D3DVERTEX sphere[ _NUM_SPHERE_VERTICIES_ ];

	for(long i=0;i< _NUM_SPHERE_VERTICIES_ ;i++){
		sphere[i] = m_sphere[i];

		sphere[i].x += (float)x-.5f;
		sphere[i].y += (float)y-.5f;
		sphere[i].z += (float)z-.5f;
	}

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		// Set the material as yellow. We're setting the ambient color here
		// since this tutorial only uses ambient lighting. For apps that use real
		// lights, the diffuse and specular values should be set. (In addition, the
		// polygons' vertices need normals for true lighting.)
		D3DMATERIAL7  mtrl;
		ZeroMemory( &mtrl, sizeof(mtrl) );
		mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = mtrl.specular.r = mtrl.specular.g = mtrl.specular.b = 1.0f;
		mtrl.ambient.r = mtrl.diffuse.r = (float)red + 0;
		mtrl.ambient.g = mtrl.diffuse.g = (float)green + 0;
		mtrl.ambient.b = mtrl.diffuse.b = (float)blue + 0;
		g_pd3dDevice->SetMaterial( &mtrl );

		// Draw the wall, composed of a D3DVERTEX-type triangle strip
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, D3DFVF_VERTEX,sphere, _NUM_SPHERE_VERTICIES_/2, NULL );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, D3DFVF_VERTEX,sphere+_NUM_SPHERE_VERTICIES_/2, _NUM_SPHERE_VERTICIES_/2, NULL );
	//}else{
		
	//}
}

void setVertex(D3DVERTEX * vout, D3DVECTOR * v1, D3DVECTOR * v2, float tu, float tv)
{
	vout->x = v1->x;
	vout->y = v1->y;
	vout->z = v1->z;
	vout->nx = v2->x;
	vout->ny = v2->y;
	vout->nz = v2->z;
	vout->tu = tu;
	vout->tv = tv;
}

void C3DXView::InitRing() {
	//init m_ring
CPatnApp * app = (CPatnApp*)AfxGetApp();
	if( app == NULL)
		return;
	float ff = (float)app->m_ordobjectsize * 1.2f;

double segments = 18;
int pos = 0;
for(int i=0;i<=segments;i++)
		{
			float x1 = (float)(cos(3.1416/segments*i)*0.5f);
			float y1 = 0.0f;			
			float z1 = (float)(sin(3.1416/segments*i)*0.5f);
			
			m_ring[pos++] = D3DVERTEX( ff * D3DVECTOR(x1,y1-0.2f,z1),Normalize(D3DVECTOR(x1,y1-0.2f,z1)), 0, 0 );	
			m_ring[pos++] = D3DVERTEX( ff * D3DVECTOR(x1,y1+0.2f,z1),Normalize(D3DVECTOR(x1,y1+0.2f,z1)), 0, 0 );	
			
		}	
}

void C3DXView::InitSphere()
{
	//init m_sphere
	CPatnApp * app = (CPatnApp*)AfxGetApp();
	if( app == NULL)
		return;
	float ff = (float)app->m_ordobjectsize;

	m_sphere[0] = D3DVERTEX( ff * D3DVECTOR(0,.5,0),
		Normalize(D3DVECTOR( 0, .5, 0 )), 0, 0 );
	m_sphere[1] = D3DVERTEX( ff * D3DVECTOR(.5,0,.5),
		Normalize(D3DVECTOR( -.5, .5, -.5 )), 0, 0 );
	m_sphere[2] = D3DVERTEX( ff * D3DVECTOR(.5,0,-.5),
		Normalize(D3DVECTOR( -.5, .5, .5 )), 0, 0 );
	m_sphere[3] = D3DVERTEX( ff * D3DVECTOR(-.5,0,-.5),
		Normalize(D3DVECTOR( .5, .5, .5 )), 0, 0 );
	m_sphere[4] = D3DVERTEX( ff * D3DVECTOR(-.5,0,.5),
		Normalize(D3DVECTOR( .5, .5, -.5 )), 0, 0 );
	m_sphere[5] = D3DVERTEX( ff * D3DVECTOR(.5,0,.5),
		Normalize(D3DVECTOR( -.5, .5, -.5 )), 0, 0 );

	m_sphere[6] = D3DVERTEX( ff * D3DVECTOR(0,-.5,0),
		Normalize(D3DVECTOR( 0, -.5, 0 )), 0, 0 );
	m_sphere[7] = D3DVERTEX( ff * D3DVECTOR(.5,0,.5),
		Normalize(D3DVECTOR( -.5, -.5, -.5 )), 0, 0 );
	m_sphere[8] = D3DVERTEX( ff * D3DVECTOR(-.5,0,.5),
		Normalize(D3DVECTOR( .5, -.5, -.5 )), 0, 0 );
	m_sphere[9] = D3DVERTEX( ff * D3DVECTOR(-.5,0,-.5),
		Normalize(D3DVECTOR( .5, -.5, .5 )), 0, 0 );
	m_sphere[10] = D3DVERTEX( ff * D3DVECTOR(.5,0,-.5),
		Normalize(D3DVECTOR( -.5, -.5, .5 )), 0, 0 );
	m_sphere[11] = D3DVERTEX( ff * D3DVECTOR(.5,0,.5),
		Normalize(D3DVECTOR( -.5, -.5, -.5 )), 0, 0 );

}

bool C3DXView::HitSphere(float x,float y,float z,CPoint point)
{
	//triangle hit test on m_sphere struct with center about x,y,z (which are 0->1)
	
	D3DVERTEX v[3];
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		App_Corner(g_pd3dDevice,&v[0].x,&v[0].y,&v[0].z,(m_sphere[0].x+ x-.5f)*x_scale,
			(m_sphere[0].y+ y-.5f)*y_scale,(m_sphere[0].z+ z-.5f)*z_scale);

		App_Corner(g_pd3dDevice,&v[1].x,&v[1].y,&v[1].z,(m_sphere[1].x+ x-.5f)*x_scale,
			(m_sphere[1].y+ y-.5f)*y_scale,(m_sphere[1].z+ z-.5f)*z_scale);
		
		for(long j=2;j< _NUM_SPHERE_VERTICIES_ /2 ;j++){
			App_Corner(g_pd3dDevice,&v[2].x,&v[2].y,&v[2].z,(m_sphere[j].x+ x-.5f)*x_scale,
				(m_sphere[j].y+ y-.5f)*y_scale,(m_sphere[j].z+ z-.5f)*z_scale);
			
			if(v[0].x - v[1].x == 0 || v[2].x-point.x == 0)
				continue;
			double ma = (v[0].y-v[1].y)/(v[0].x-v[1].x);
			double mb = (v[2].y-point.y)/(v[2].x-point.x);
			if(ma - mb == 0)
				continue;
			double tx = (v[2].y-v[0].y-mb*v[2].x+ma*v[0].x)/(ma - mb);
			double ty = ma*(tx-v[0].x) + v[0].y;
			double la = (tx-v[2].x)*(tx-v[2].x) + (ty-v[2].y)*(ty-v[2].y);
			double lb = (point.x-v[2].x)*(point.x-v[2].x) + (point.y-v[2].y)*(point.y-v[2].y);
			if(la > lb &&
				((v[0].x < tx && tx < v[1].x) ||
				(v[1].x < tx && tx < v[0].x)) &&
				((v[2].x < point.x && point.x < tx) ||
				(tx < point.x && point.x < v[2].x))){// && z > hitz){
			//	m_str.Format("p(%d,%d) t1(%d,%d) t2(%d,%d) t3(%d,%d) tx(%.2f)",point.x,point.y,
			//		(long)v[0].x,(long)v[0].y,(long)v[1].x,(long)v[1].y,(long)v[2].x,(long)v[2].y,tx);
				return true;
			}
			v[1].x = v[2].x;
			v[1].y = v[2].y;
			m_hittop++;
		}
		long idx = _NUM_SPHERE_VERTICIES_/2;
		App_Corner(g_pd3dDevice,&v[0].x,&v[0].y,&v[0].z,(m_sphere[idx].x+ x-.5f)*x_scale,
			(m_sphere[idx].y+ y-.5f)*y_scale,(m_sphere[idx].z+ z-.5f)*z_scale);
		App_Corner(g_pd3dDevice,&v[1].x,&v[1].y,&v[1].z,(m_sphere[idx].x+ x-.5f)*x_scale,
			(m_sphere[idx].y+ y-.5f)*y_scale,(m_sphere[idx].z+ z-.5f)*z_scale);
		for(j=idx+2;j< _NUM_SPHERE_VERTICIES_;j++){
			App_Corner(g_pd3dDevice,&v[2].x,&v[2].y,&v[2].z,(m_sphere[j].x+ x-.5f)*x_scale,
				(m_sphere[j].y+ y-.5f)*y_scale,(m_sphere[j].z+ z-.5f)*z_scale);
			if(v[0].x - v[1].x == 0 || v[2].x-point.x == 0)
				continue;
			double ma = (v[0].y-v[1].y)/(v[0].x-v[1].x);
			double mb = (v[2].y-point.y)/(v[2].x-point.x);
			if(ma - mb == 0)
				continue;
			double tx = (v[2].y-v[0].y-mb*v[2].x+ma*v[0].x)/(ma - mb);
			double ty = ma*(tx-v[0].x) + v[0].y;
			double la = (tx-v[2].x)*(tx-v[2].x) + (ty-v[2].y)*(ty-v[2].y);
			double lb = (point.x-v[2].x)*(point.x-v[2].x) + (point.y-v[2].y)*(point.y-v[2].y);
			if(la > lb &&
				((v[0].x < tx && tx < v[1].x) ||
				(v[1].x < tx && tx < v[0].x)) &&
				((v[2].x < point.x && point.x < tx) ||
				(tx < point.x && point.x < v[2].x))){// && z > hitz){
				//m_str.Format("p(%d,%d) t1(%d,%d) t2(%d,%d) t3(%d,%d) tx(%.2f)",point.x,point.y,
				//	(long)v[0].x,(long)v[0].y,(long)v[1].x,(long)v[1].y,(long)v[2].x,(long)v[2].y,tx);

				//hit successful
				return true;
			}
			v[1].x = v[2].x;
			v[1].y = v[2].y;
			m_hitbottom++;
		}			
	//}else{
		//GDIDRAWING
	//}
	return false;
}

void C3DXView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if(m_legend_instance) return;

	m_local = point;
	ScreenToClient(&m_local);

	CMenu* pPopup = m_menu.GetSubMenu(0);
	if(pPopup != NULL){
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y,
		AfxGetMainWnd());
	}
}
	

VOID C3DXView::App_Corner(LPDIRECT3DDEVICE7  pd3dDevice,float *sx,float *sy,float *sz,float x,float y,float z)
{
	// Get the width and height of the viewport. This is needed to scale the
    // transformed vertices to fit the render window.
	if(pd3dDevice == NULL)
		return;

    D3DVIEWPORT7 vp;
	pd3dDevice->GetViewport( &vp );
	DWORD dwClipWidth  = vp.dwWidth/2;
    DWORD dwClipHeight = vp.dwHeight/2;
	CRect rect;
	GetClientRect(rect);
	
    // Get the current matrix set. This is needed for the transformation.
    D3DMATRIX matWorld, matView, matProj;
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		pd3dDevice->GetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
		//pd3dDevice->GetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
		matView = m_matView;
		pd3dDevice->GetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );
	D3DMATRIX matSet = matWorld * matView * matProj;

    // Transform it through the current matrix set
    FLOAT xp = matSet._11*x + matSet._21*y + matSet._31*z + matSet._41;
    FLOAT yp = matSet._12*x + matSet._22*y + matSet._32*z + matSet._42;
    FLOAT zp = matSet._13*x + matSet._23*y + matSet._33*z + matSet._43;
    FLOAT wp = matSet._14*x + matSet._24*y + matSet._34*z + matSet._44;

	if(dwClipWidth != dwClipHeight)
		dwClipWidth = dwClipWidth;

	*sx  = ( 1.0f + (xp/wp) ) * dwClipWidth;
    *sy  = ( 1.0f - (yp/wp) ) * dwClipHeight;
	*sz  = zp / wp * dwClipHeight;	
	
}

VOID C3DXView::App_Corner_Reverse(LPDIRECT3DDEVICE7 pd3dDevice,float *sx,float *sy,float *sz,float x,float y,float z)
{
	// Get the width and height of the viewport. This is needed to scale the
    // transformed vertices to fit the render window.
	if(pd3dDevice == NULL)
		return;

    D3DVIEWPORT7 vp;
	pd3dDevice->GetViewport( &vp );
	DWORD dwClipWidth  = vp.dwWidth/2;
    DWORD dwClipHeight = vp.dwHeight/2;
	CRect rect;
	GetClientRect(rect);
	
    // Get the current matrix set. This is needed for the transformation.
    D3DMATRIX matWorld, matView, matProj;
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		pd3dDevice->GetTransform( D3DTRANSFORMSTATE_WORLD,      &matWorld );
		//pd3dDevice->GetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
		matView = m_matView;
		pd3dDevice->GetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );
	D3DMATRIX matSet = matWorld * matView * matProj;

    if(dwClipWidth != dwClipHeight)
		dwClipWidth = dwClipWidth;
	
	//reverse it
	FLOAT wp = 1;
	FLOAT xp;
	FLOAT zp;
	FLOAT yp;
	
	xp = (float) (x/(double)dwClipWidth - 1)*(wp);
	yp = (float) (1 - y/(double)dwClipHeight)*(wp);
	zp = (float) (z/(double)dwClipHeight)*(wp);


	float determinant = 
   matSet._14 * matSet._23 * matSet._32 * matSet._41-matSet._13 * matSet._24 * matSet._32 * matSet._41-matSet._14 * matSet._22 * matSet._33 * matSet._41+matSet._12 * matSet._24    * matSet._33 * matSet._41+
   matSet._13 * matSet._22 * matSet._34 * matSet._41-matSet._12 * matSet._23 * matSet._34 * matSet._41-matSet._14 * matSet._23 * matSet._31 * matSet._42+matSet._13 * matSet._24    * matSet._31 * matSet._42+
   matSet._14 * matSet._21 * matSet._33 * matSet._42-matSet._11 * matSet._24 * matSet._33 * matSet._42-matSet._13 * matSet._21 * matSet._34 * matSet._42+matSet._11 * matSet._23    * matSet._34 * matSet._42+
   matSet._14 * matSet._22 * matSet._31 * matSet._43-matSet._12 * matSet._24 * matSet._31 * matSet._43-matSet._14 * matSet._21 * matSet._32 * matSet._43+matSet._11 * matSet._24    * matSet._32 * matSet._43+
   matSet._12 * matSet._21 * matSet._34 * matSet._43-matSet._11 * matSet._22 * matSet._34 * matSet._43-matSet._13 * matSet._22 * matSet._31 * matSet._44+matSet._12 * matSet._23    * matSet._31 * matSet._44+
   matSet._13 * matSet._21 * matSet._32 * matSet._44-matSet._11 * matSet._23 * matSet._32 * matSet._44-matSet._12 * matSet._21 * matSet._33 * matSet._44+matSet._11 * matSet._22    * matSet._33 * matSet._44;

	D3DMATRIX m;

	if(determinant != 0)
	{			
		m._11 = matSet._23*matSet._34*matSet._42 - matSet._24*matSet._33*matSet._42 + matSet._24*matSet._32*matSet._43 - matSet._22*matSet._34*matSet._43 - matSet._23*matSet._32*matSet._44 + matSet._22*matSet._33*matSet._44;
		m._12 = matSet._14*matSet._33*matSet._42 - matSet._13*matSet._34*matSet._42 - matSet._14*matSet._32*matSet._43 + matSet._12*matSet._34*matSet._43 + matSet._13*matSet._32*matSet._44 - matSet._12*matSet._33*matSet._44;
		m._13 = matSet._13*matSet._24*matSet._42 - matSet._14*matSet._23*matSet._42 + matSet._14*matSet._22*matSet._43 - matSet._12*matSet._24*matSet._43 - matSet._13*matSet._22*matSet._44 + matSet._12*matSet._23*matSet._44;
		m._14 = matSet._14*matSet._23*matSet._32 - matSet._13*matSet._24*matSet._32 - matSet._14*matSet._22*matSet._33 + matSet._12*matSet._24*matSet._33 + matSet._13*matSet._22*matSet._34 - matSet._12*matSet._23*matSet._34;
		m._21 = matSet._24*matSet._33*matSet._41 - matSet._23*matSet._34*matSet._41 - matSet._24*matSet._31*matSet._43 + matSet._21*matSet._34*matSet._43 + matSet._23*matSet._31*matSet._44 - matSet._21*matSet._33*matSet._44;
		m._22 = matSet._13*matSet._34*matSet._41 - matSet._14*matSet._33*matSet._41 + matSet._14*matSet._31*matSet._43 - matSet._11*matSet._34*matSet._43 - matSet._13*matSet._31*matSet._44 + matSet._11*matSet._33*matSet._44;
		m._23 = matSet._14*matSet._23*matSet._41 - matSet._13*matSet._24*matSet._41 - matSet._14*matSet._21*matSet._43 + matSet._11*matSet._24*matSet._43 + matSet._13*matSet._21*matSet._44 - matSet._11*matSet._23*matSet._44;
		m._24 = matSet._13*matSet._24*matSet._31 - matSet._14*matSet._23*matSet._31 + matSet._14*matSet._21*matSet._33 - matSet._11*matSet._24*matSet._33 - matSet._13*matSet._21*matSet._34 + matSet._11*matSet._23*matSet._34;
		m._31 = matSet._22*matSet._34*matSet._41 - matSet._24*matSet._32*matSet._41 + matSet._24*matSet._31*matSet._42 - matSet._21*matSet._34*matSet._42 - matSet._22*matSet._31*matSet._44 + matSet._21*matSet._32*matSet._44;
		m._32 = matSet._14*matSet._32*matSet._41 - matSet._12*matSet._34*matSet._41 - matSet._14*matSet._31*matSet._42 + matSet._11*matSet._34*matSet._42 + matSet._12*matSet._31*matSet._44 - matSet._11*matSet._32*matSet._44;
		m._33 = matSet._12*matSet._24*matSet._41 - matSet._14*matSet._22*matSet._41 + matSet._14*matSet._21*matSet._42 - matSet._11*matSet._24*matSet._42 - matSet._12*matSet._21*matSet._44 + matSet._11*matSet._22*matSet._44;
		m._34 = matSet._14*matSet._22*matSet._31 - matSet._12*matSet._24*matSet._31 - matSet._14*matSet._21*matSet._32 + matSet._11*matSet._24*matSet._32 + matSet._12*matSet._21*matSet._34 - matSet._11*matSet._22*matSet._34;
		m._41 = matSet._23*matSet._32*matSet._41 - matSet._22*matSet._33*matSet._41 - matSet._23*matSet._31*matSet._42 + matSet._21*matSet._33*matSet._42 + matSet._22*matSet._31*matSet._43 - matSet._21*matSet._32*matSet._43;
		m._42 = matSet._12*matSet._33*matSet._41 - matSet._13*matSet._32*matSet._41 + matSet._13*matSet._31*matSet._42 - matSet._11*matSet._33*matSet._42 - matSet._12*matSet._31*matSet._43 + matSet._11*matSet._32*matSet._43;
		m._43 = matSet._13*matSet._22*matSet._41 - matSet._12*matSet._23*matSet._41 - matSet._13*matSet._21*matSet._42 + matSet._11*matSet._23*matSet._42 + matSet._12*matSet._21*matSet._43 - matSet._11*matSet._22*matSet._43;
		m._44 = matSet._12*matSet._23*matSet._31 - matSet._13*matSet._22*matSet._31 + matSet._13*matSet._21*matSet._32 - matSet._11*matSet._23*matSet._32 - matSet._12*matSet._21*matSet._33 + matSet._11*matSet._22*matSet._33;

		m._11 = m._11 / determinant;
		m._12 = m._12 / determinant;
		m._13 = m._13 / determinant;
		m._14 = m._14 / determinant;
		m._21 = m._21 / determinant;
		m._22 = m._22 / determinant;
		m._23 = m._23 / determinant;
		m._24 = m._24 / determinant;
		m._31 = m._31 / determinant;
		m._32 = m._32 / determinant;
		m._33 = m._33 / determinant;
		m._34 = m._34 / determinant;
		m._41 = m._41 / determinant;
		m._42 = m._42 / determinant;
		m._43 = m._43 / determinant;
		m._44 = m._44 / determinant;
	}

   	*sx = m._11*xp + m._21*yp + m._31*zp + m._41;
    *sy = m._12*xp + m._22*yp + m._32*zp + m._42;
    *sz = m._13*xp + m._23*yp + m._33*zp + m._43;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	 switch(Msg)
    {
        case WM_COMMAND:
            
        break;
        default:
            return FALSE;
    }
    return TRUE;
}


void C3DXView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	
	POSITION pos;
	CView *pView;

	pos = m_pDoc->GetFirstViewPosition();   
	while (pos != NULL) {
		pView = m_pDoc->GetNextView(pos);   
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView)))
			break;
		else
			pView = NULL;
	} 

	CRect rect;
	GetClientRect(rect);

	m_oldpoint = point;

	UINT state = m_menu.GetMenuState(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_BYCOMMAND);

	//check for legend
	if(point.x <= m_legendRect.right && 
		point.x >= m_legendRect.left &&
		point.y >= m_legendRect.top &&
		point.y <= m_legendRect.bottom)
	{
		//is it one by one?
		
		//left one?
		if(point.x <= m_legendRect.left + m_legendHeight/2 + 1)
		{
			m_legendPos -= m_legendNumberOfRows;
			if(m_legendPos < 0)
				m_legendPos = 0;
		}
		else if(point.x >= m_legendRect.right - m_legendHeight/2 -1)
		{
			m_legendPos += m_legendNumberOfRows;
		}
		else //must be MOVE TO POS request
		{
			long sz = (long)(m_legendNumberOfRows / m_legendScale);
			m_legendPos = (m_legendPos / m_legendNumberOfRows) * m_legendNumberOfRows + 
				(long)(m_legendScale * (point.x-m_legendX));
			if(m_legendPos < 0)
				m_legendPos = 0;
		}

		RedrawWindow();
		return;
	}

	if(m_legend_instance) return;

	if(nFlags & MK_SHIFT || m_togglerotate){
		//SetCursor(AfxGetApp()->LoadCursor(IDC_3DROTATE));
	}else{// if(((CPatnApp*)AfxGetApp())->m_ordtype == 1 || (!m_Legend)){
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
			if(m_currentobject < m_pDoc->GetNoOfRows()){
				m_pDoc->GetRowLabel(m_currentobject,m_infobar.name1); 
				//m_infobar.ordin1[0] = m_pDoc->m_aRowXCoords[m_currentobject];
				//m_infobar.ordin1[1] = m_pDoc->m_aRowYCoords[m_currentobject];
				//m_infobar.ordin1[2] = m_pDoc->m_aRowZCoords[m_currentobject];
			}
			if(!m_SelOnly && pView != NULL){
				((CTableView*)pView)->m_pSelectedColumns->RemoveAll();
				((CTableView*)pView)->m_pSelectedRows->RemoveAll();
				((CTableView*)pView)->m_pSelectedRows->SetAt(m_currentobject);
				pos = m_pDoc->GetFirstViewPosition();   
				while (pos != NULL)   
				{
					pView = m_pDoc->GetNextView(pos);   
					if(pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)))
						pView->RedrawWindow();
				}
			}
			rect.right = m_infowidth + rect.Height() + (long)fabs((double)2*m_pDoc->m_lfCells.lfHeight);
			RedrawWindow(rect);
			break;
		case h_centroid:
			//set info bar contents to centroid's info
			//set tableview selection to objects belonging to 
			//	this group
			//change mhint to centroidselection
			m_hint = centroidselection;

			m_infobar.item = group;
			m_infobar.itemNr = 1;
			
			if(state & MF_CHECKED)
			{
				if(m_currentcentroid < m_pDoc->m_AprioriGroups.GetSize()){
					m_infobar.name1.Format("%d",m_currentcentroid+1); 
					m_infobar.ordin1[0] = m_pDoc->m_aAprioriGroupColours[m_currentcentroid].red*(m_pDoc->m_OrdXmax-m_pDoc->m_OrdXmin)+m_pDoc->m_OrdXmin;
				}
			}
			else
			{
				if(m_currentcentroid < m_pDoc->m_Groups.GetSize()){
					m_infobar.name1.Format("%d",m_pDoc->m_Groups.GetAt(m_currentcentroid)); 
					m_infobar.ordin1[0] = m_pDoc->m_aGroupColours[m_currentcentroid].red*(m_pDoc->m_OrdXmax-m_pDoc->m_OrdXmin)+m_pDoc->m_OrdXmin;
					//m_infobar.ordin1[1] = m_pDoc->m_aGroupColours[m_currentcentroid].green*(m_pDoc->m_OrdYmax-m_pDoc->m_OrdYmin)+m_pDoc->m_OrdYmin;
					//m_infobar.ordin1[2] = m_pDoc->m_aGroupColours[m_currentcentroid].blue*(m_pDoc->m_OrdZmax-m_pDoc->m_OrdZmin)+m_pDoc->m_OrdZmin;
					//m_infobar.group1.SetAt(0,m_pDoc->m_Gstastats.GetAt(m_currentcentroid)->GetAt(m_Gstaattrib)->minimum);
					//m_infobar.group1.SetAt(1,m_pDoc->m_Gstastats.GetAt(m_currentcentroid)->GetAt(m_Gstaattrib)->first_quartile);
					//m_infobar.group1.SetAt(2,m_pDoc->m_Gstastats.GetAt(m_currentcentroid)->GetAt(m_Gstaattrib)->median);
					//m_infobar.group1.SetAt(3,m_pDoc->m_Gstastats.GetAt(m_currentcentroid)->GetAt(m_Gstaattrib)->mean);
					//m_infobar.group1.SetAt(4,m_pDoc->m_Gstastats.GetAt(m_currentcentroid)->GetAt(m_Gstaattrib)->third_quartile);
					//m_infobar.group1.SetAt(5,m_pDoc->m_Gstastats.GetAt(m_currentcentroid)->GetAt(m_Gstaattrib)->maximum);
				}
			}
			if(m_Gstaattrib < m_pDoc->GetNoOfColumns())
				m_pDoc->GetColumnLabel(m_Gstaattrib,m_infobar.attrib);

			if(!m_SelOnly && pView != NULL){
				((CTableView*)pView)->m_pSelectedColumns->RemoveAll();
				((CTableView*)pView)->m_pSelectedRows->RemoveAll();
				pos = m_pDoc->GetFirstViewPosition();   
				while (pos != NULL)   
				{
					pView = m_pDoc->GetNextView(pos);   
					if(pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)))
						pView->RedrawWindow();
				}
			}
			rect.right = m_infowidth + rect.Height() + (long)fabs((double)2*m_pDoc->m_lfCells.lfHeight);
			RedrawWindow(rect);
			break;
		case h_space:
			//find closest group and same as centroid
			m_hint = spaceselection;
			if(!m_SelOnly && pView != NULL){
				((CTableView*)pView)->m_pSelectedColumns->RemoveAll();
				((CTableView*)pView)->m_pSelectedRows->RemoveAll();
				while (pos != NULL)   
				{
					CView* pView = m_pDoc->GetNextView(pos);   
					if(pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)))
						pView->RedrawWindow();
				}
			}
			rect.right = m_infowidth + rect.Height() + (long)fabs((double)2*m_pDoc->m_lfCells.lfHeight);
			RedrawWindow(rect);
			break;
		case h_textbox:
			//change mhint to txtselection
			
			m_hint = textboxselection;
			//RedrawWindow();
			break;
		case h_none:
			//do nothing
			break;
		case h_stressbox:
			m_hint = stressboxselection;
			//RedrawWindow();
			break;
		default:
			break;
		}		
	}
//	CView::OnLButtonDown(nFlags, point);
}

void C3DXView::OnMouseMove(UINT nFlags, CPoint point) 
{
	long newlength;
	long newheight;
	long oldobject = m_currentobject;
	long oldcentroid = m_currentcentroid;
	CRect rect,r2;
	GetClientRect(rect);
	
	m_glselect = true;

	hit test;

	if((m_togglerotate || nFlags & MK_SHIFT) && (nFlags & MK_LBUTTON)){
		m_Rotate = false;
		//KillTimer(1);
		m_menu.CheckMenuItem(ID__ROTATE,MF_UNCHECKED);

		double xdiv = (point.x - m_oldpoint.x)/-100.0;
		double ydiv = (point.y - m_oldpoint.y)/100.0;

		//MoveView(xdiv,ydiv);
		m_vEyePt;m_vUpVec;m_vRight;
		
		//find transform from initial pos to current pos
		m_matView;

		//if(m_pDoc->m_Ord3d){
			RotateAboutVector(&m_vEyePt,&m_vUpVec,xdiv);
			RotateAboutVector(&m_vRight,&m_vUpVec,xdiv);
			RotateAboutVector(&m_vLight,&m_vUpVec,-xdiv);

			RotateAboutVector(&m_vEyePt,&m_vRight,ydiv);
			RotateAboutVector(&m_vUpVec,&m_vRight,ydiv);
			RotateAboutVector(&m_vLight,&m_vRight,-ydiv);

			//fix light
			m_light.dvDirection.x = m_vLight.x;
			m_light.dvDirection.y = m_vLight.y;
			m_light.dvDirection.z = m_vLight.z;

			g_pd3dDevice->SetLight( 0, &m_light );
			g_pd3dDevice->LightEnable( 0, TRUE );

			//g_pd3dDevice->SetRenderState( D3DRENDERSTATE_LIGHTING, TRUE );

		g_pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, TRUE );
			UpdateZOrders();
		//}		
		
		m_oldpoint = point;
		
		rect.left += m_infowidth;
		rect.right = m_infowidth + rect.Height() + (long)fabs((double)2*m_pDoc->m_lfCells.lfHeight);
		RedrawWindow(rect);
	}else if((nFlags & MK_SHIFT) && (nFlags & MK_RBUTTON)){
		m_oldpoint;

		m_rotatepoint = point;
		
		RedrawWindow(rect);
	}else{
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
			rect.left += m_infowidth;
			rect.right = m_infowidth + rect.Height() + (long)fabs((double)2*m_pDoc->m_lfCells.lfHeight);
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
			rect.left += m_infowidth;
			rect.right = m_infowidth + rect.Height() + (long)fabs((double)2.0*m_pDoc->m_lfCells.lfHeight);
			RedrawWindow();
			break;
		case textboxselection:
			if(m_oldpoint.x == point.x && m_oldpoint.y == point.y)
			{
				break;
			}
		case textboxmove:
			
			m_hint = textboxmove;
			//update selected text box location
			//newheight = (int)fabs((double)m_TextBoxes[m_currenttextbox]->font.lfHeight);
			//newlength = m_TextBoxes[m_currenttextbox]->contents.GetLength() * newheight;
			if(m_currenttextbox < m_TextBoxes.GetSize() && m_TextBoxes[m_currenttextbox] != NULL){
				rect = m_TextBoxes[m_currenttextbox]->location;
				newheight = rect.Height();
				newlength = rect.Width();

				m_TextBoxes[m_currenttextbox]->location.left = point.x+m_local.x;
				m_TextBoxes[m_currenttextbox]->location.right = point.x + newlength+m_local.x;				
				m_TextBoxes[m_currenttextbox]->location.top = point.y+m_local.y;
				m_TextBoxes[m_currenttextbox]->location.bottom = point.y + newheight+m_local.y;
				
				r2 = m_TextBoxes[m_currenttextbox]->location;

				if(rect.left > r2.left) rect.left = r2.left;
				if(rect.right < r2.right) rect.right = r2.right;
				if(rect.top > r2.top) rect.top = r2.top;
				if(rect.bottom < r2.bottom) rect.bottom = r2.bottom;
				
				RedrawWindow(rect);
			}
			break;
		case stressboxselection:
		case stressboxmove:
			m_hint = stressboxmove;
			//update selected text box location
			//newheight = (int)fabs((double)m_stressbox.font.lfHeight);
			//newlength = m_stressbox.contents.GetLength() * newheight;
			
			rect = m_stressbox.location;
			newheight = rect.Height();
			newlength = rect.Width();

			m_stressbox.location.left = point.x + m_local.x;
			m_stressbox.location.right = point.x + newlength + m_local.x;				
			m_stressbox.location.top = point.y + m_local.y;
			m_stressbox.location.bottom = point.y + newheight + m_local.y;

			r2 = m_stressbox.location;

			if(rect.left > r2.left) rect.left = r2.left;
			if(rect.right < r2.right) rect.right = r2.right;
			if(rect.top > r2.top) rect.top = r2.top;
			if(rect.bottom < r2.bottom) rect.bottom = r2.bottom;
			
			RedrawWindow(rect);
			break;
		case none:
		default:
			test = HitTest(point, true);
			switch(test){
			case h_object:
				break;
			default:

				break;
			}

			RedrawWindow();
			break; 
		}
	}

	//RedrawWindow();
	//CView::OnMouseMove(nFlags, point);

	m_glselect = false;

	//CView::OnMouseMove(nFlags, point);
}

void C3DXView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//Render3DEnvironment();	
	//RedrawWindow();
	//this->OnSetRotationxz();
	CView::OnLButtonDblClk(nFlags, point);
}

void C3DXView::OnDisplaymst() 
{
	if(m_legend_instance) return;
	if(m_Mst){
		m_Mst = false;
		m_menu.CheckMenuItem(ID__DISPLAYMST,MF_UNCHECKED);
	}else{
		m_Mst = true;
		m_menu.CheckMenuItem(ID__DISPLAYMST,MF_CHECKED);
	}
	RedrawWindow();	
}

void C3DXView::InitMst(void)
{

	long i, j;

	if(m_pDoc->m_aMst.GetSize() == 0)
		return;

	if(m_mst != NULL){
		delete [] m_mst;
		m_mst = NULL;
	}
	m_mst = new D3DVERTEX[(m_pDoc->m_aMst.GetSize()-1)*2];
	if(m_mst == NULL){
		m_mst_size = 0;
		return;
	}
	m_mst_size = (m_pDoc->m_aMst.GetSize()-1)*2;

	m_infobar.mst_length = 0;
	long pos = 0;
	for(i=0;i<m_pDoc->m_aMst.GetSize();i++){
		for (j = 0; j < m_pDoc->m_aMst[i]->GetSize(); j++) {
			if (m_pDoc->m_aMst.GetAt(i)->GetAt(j) == 1 &&
				i < m_pDoc->m_aObjectColours.GetSize() &&
				j < m_pDoc->m_aObjectColours.GetSize() &&
				pos < ((m_pDoc->m_aMst.GetSize()-1)*2)) {
				m_mst[pos].x = (float)((m_pDoc->m_aObjectColours[i].red - .5f)*x_scale);
				if(m_pDoc->m_Ord3d >= 2) m_mst[pos].y = (float)((m_pDoc->m_aObjectColours[i].green - .5f)*y_scale);
				if(m_pDoc->m_Ord3d == 3)
					m_mst[pos++].z = (float)((m_pDoc->m_aObjectColours[i].blue - .5f)*z_scale);
				else if(m_pDoc->m_Ord3d == 2)
					m_mst[pos++].z = 0;
				else
				{
					m_mst[pos].z = 0;
					m_mst[pos++].y = 0;
				}

				m_mst[pos].x = (float)((m_pDoc->m_aObjectColours[j].red - .5f)*x_scale);
				if(m_pDoc->m_Ord3d >= 2) m_mst[pos].y = (float)((m_pDoc->m_aObjectColours[j].green - .5f)*y_scale);
				if(m_pDoc->m_Ord3d == 3)
					m_mst[pos++].z = (float)((m_pDoc->m_aObjectColours[j].blue - .5f)*z_scale);
				else if(m_pDoc->m_Ord3d == 2)
					m_mst[pos++].z = 0;
				else
				{
					m_mst[pos].z = 0;
					m_mst[pos++].y = 0;
				}

				if (i < j) {
					m_infobar.mst_length += m_pDoc->m_AssocMatrixRows.m_aMatrix[j-1]->GetAt(i);
				} else {
					m_infobar.mst_length += m_pDoc->m_AssocMatrixRows.m_aMatrix[i-1]->GetAt(j);
				}
			}
		}
	}
}

void C3DXView::DrawMst(void)
{

	D3DMATERIAL7 mtrl;
    ZeroMemory( &mtrl, sizeof(mtrl) );
    mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = 0.0f;
	mtrl.diffuse.r = (float)0;
	mtrl.diffuse.g = (float)0;
	mtrl.diffuse.b = (float)0;
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		g_pd3dDevice->SetMaterial( &mtrl );
		m_gdi_offsetx = 0;
	//}

	if(m_mst == NULL ||
		m_mst_size != (m_pDoc->m_aMst.GetSize()-1)*2 ||
		m_pDoc->m_mst_changed)
	{
		m_pDoc->m_mst_changed = false;
		InitMst();
		InitRanges();
	}

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		if(m_mst != NULL)
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,
				m_mst, (m_pDoc->m_aMst.GetSize()-1)*2, NULL );
	//}else{
		//GDIDRAWING
				

	//}
}
void C3DXView::DrawMst(HDC hdc)
{
	if(m_mst == NULL ||
		m_mst_size != (m_pDoc->m_aMst.GetSize()-1)*2)
		InitMst();

	float tx,ty,tz;
	if(m_mst != NULL){
		//g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,
			//m_mst, (m_pDoc->m_aMst.GetSize()-1)*2, NULL );
		for(long i=0;i<(m_pDoc->m_aMst.GetSize()-1)*2;i+=2){
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,m_mst[i].x,
				m_mst[i].y,m_mst[i].z);
			MoveToEx(hdc,(long)tx+m_infowidth,(long)ty,NULL);
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,m_mst[i+1].x,
				m_mst[i+1].y,m_mst[i+1].z);
			LineTo(hdc,(long)tx+m_infowidth,(long)ty);
		}
	}
}

C3DXView::hit C3DXView::HitTest(CPoint point, bool hover)
{
	POSITION pos;
	CView * pView = NULL;
	pos = m_pDoc->GetFirstViewPosition();
	while(pos != NULL){
		pView = m_pDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView)))
			break;
		else
			pView = NULL;
	}

	long i;
	float x,y,z;

	if(!hover) {
		m_currenttextbox = -1;
	}

	//check if over info bar region (none)
	//if(point.x < 120) return h_none;

	//check if over stress box
	if(!hover && 
		point.x <= m_stressbox.location.right &&
		point.x >= m_stressbox.location.left &&
		point.y <= m_stressbox.location.bottom &&
		point.y >= m_stressbox.location.top){
		m_local.y = m_stressbox.location.top - point.y;
		m_local.x = m_stressbox.location.left - point.x;
		return h_stressbox;
	}

	//check if over txt box
	long lpos = 5000;
	if(!hover){
		for(i=0;i<m_TextBoxes.GetSize() && m_TextBoxes[i] != NULL;i++){
			if(point.x <= m_TextBoxes[i]->location.right &&
				point.x >= m_TextBoxes[i]->location.left &&
				point.y <= m_TextBoxes[i]->location.bottom &&
				point.y >= m_TextBoxes[i]->location.top){
				if(m_currenttextbox == -1 || lpos < m_TextBoxes[i]->location.left){
					m_currenttextbox = i;			
					lpos = m_TextBoxes[i]->location.left;
					break;
				}
			}
		}
		if(m_currenttextbox != -1){
			m_local.y = m_TextBoxes[i]->location.top - point.y;
			m_local.x = m_TextBoxes[i]->location.left - point.x;
			return h_textbox;
		}
	
		m_currentobject = m_currentcentroid = -1;
		m_compareobject = m_comparecentroid = -1;

	
	}
	m_selectedobject = -1;
	m_selectedgroup = -1;

	float nx,ny,nz;
	float hitz = 10000;
	CString str;

	point.x -= m_infowidth;
	
	UINT state = m_menu.GetMenuState(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_BYCOMMAND);
	
	if(!m_Centroid){	
		m_hittop = m_hitbottom = 0;
		for(i=0;i<m_pDoc->m_aObjectColours.GetSize();i++){
			if(!m_SelOnly || (pView != NULL && ((CTableView*)pView)->m_pSelectedRows->IsSelected(i))){
				x = m_pDoc->m_aObjectColours[i].red;
				y = m_pDoc->m_aObjectColours[i].green;
				z = m_pDoc->m_aObjectColours[i].blue;
				if(m_pDoc->m_Ord3d == 2) z = 0.5;
				else if(m_pDoc->m_Ord3d == 1) {z = 0.5; y = 0.5;}

				App_Corner(g_pd3dDevice,&nx,&ny,&nz,(float)((x-.5f)*x_scale),(float)((y-.5f)*y_scale),(float)((z-.5f)*z_scale));
				
				if(m_Symbols && 
					point.x >= nx-8 && point.x <= nx+8 &&
					point.y >= ny-8 && point.y <= ny+8 &&
					nz < hitz){
					if(!hover) m_currentobject = i;
					m_selectedobject = i;
					hitz = nz;
				}else if(!m_Symbols && HitSphere2(x,y,z,point) && nz < hitz){
					//hit successful
					//str = m_str;
					if(!hover) m_currentobject = i;
					m_selectedobject = i;
					hitz = nz;
				}			
			}
		}
		if((m_currentobject != -1 && !hover) || (hover && m_selectedobject != -1)){
			//m_str = str;
			return h_object;
		}
	}else {
		if(state & MF_CHECKED)
		{
			for(i=0;i<m_pDoc->m_aAprioriGroupColours.GetSize();i++){
				x = m_pDoc->m_aAprioriGroupColours[i].red;
				y = m_pDoc->m_aAprioriGroupColours[i].green;
				z = m_pDoc->m_aAprioriGroupColours[i].blue;
				if(m_pDoc->m_Ord3d == 2) z = 0.5;
				else if(m_pDoc->m_Ord3d == 1) {z = 0.5; y = 0.5;}

				App_Corner(g_pd3dDevice,&nx,&ny,&nz,(float)((x-.5f)*x_scale),(float)((y-.5f)*y_scale),(float)((z-.5f)*z_scale));

				if(m_Symbols && 
					point.x >= nx-8 && point.x <= nx+8 &&
					point.y >= ny-8 && point.y <= ny+8 &&
					nz < hitz){
					if(!hover) m_currentobject = i;
					m_selectedgroup = i;
					hitz = nz;
				}else if(!m_Symbols && HitSphere2(x,y,z,point) && nz < hitz){
					//str = m_str;
					if(!hover) m_currentcentroid = i;
					m_selectedgroup = i;
					hitz = nz;
				}
			}
		}
		else
		{
			for(i=0;i<m_pDoc->m_aGroupColours.GetSize();i++){
				x = m_pDoc->m_aGroupColours[i].red;
				y = m_pDoc->m_aGroupColours[i].green;
				z = m_pDoc->m_aGroupColours[i].blue;
				if(m_pDoc->m_Ord3d == 2) z = 0.5;
				else if(m_pDoc->m_Ord3d == 1) {z = 0.5; y = 0.5;}

				App_Corner(g_pd3dDevice,&nx,&ny,&nz,(x-.5f)*x_scale,(y-.5f)*y_scale,(z-.5f)*z_scale);

				if(m_Symbols && 
					point.x >= nx-8 && point.x <= nx+8 &&
					point.y >= ny-8 && point.y <= ny+8 &&
					nz < hitz){
					if(!hover) m_currentobject = i;
					m_selectedgroup = i;
					hitz = nz;
				}else if(!m_Symbols && HitSphere2(x,y,z,point) && nz < hitz){
					//str = m_str;
					if(!hover) m_currentcentroid = i;
					m_selectedgroup = i;
					hitz = nz;
				}
			}
		}
		if(m_currentcentroid != -1){
			//m_str = str;
			return h_centroid;
		}
		
		m_currentobject = -1;
	}

	double tdist,dist = 100;
	
	if( state & MF_CHECKED)
	{
		for (i = 0; i<m_pDoc->m_aAprioriGroupColours.GetSize(); i++) {
			x = m_pDoc->m_aAprioriGroupColours[i].red;
			y = m_pDoc->m_aAprioriGroupColours[i].green;
			z = m_pDoc->m_aAprioriGroupColours[i].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5; y=0.5;}
				
			App_Corner(g_pd3dDevice,&nx,&ny,&nz,(x-.5f)*x_scale,(y-.5f)*y_scale,(z-.5f)*z_scale);

			tdist = (nx - point.x) * (nx - point.x) +
				(ny - point.y) * (ny - point.y);

			if(m_currentcentroid == -1 || tdist < dist){
				if(!hover) m_currentcentroid = i;
				m_selectedgroup = i;
				dist = tdist;
			}		
		}	
	}
	else 
	{
		for (i = 0; i<m_pDoc->m_aGroupColours.GetSize(); i++) {
			x = m_pDoc->m_aGroupColours[i].red;
			y = m_pDoc->m_aGroupColours[i].green;
			z = m_pDoc->m_aGroupColours[i].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5; y=0.5;}
				
			App_Corner(g_pd3dDevice,&nx,&ny,&nz,(x-.5f)*x_scale,(y-.5f)*y_scale,(z-.5f)*z_scale);

			tdist = (nx - point.x) * (nx - point.x) +
				(ny - point.y) * (ny - point.y);

			if(m_selectedgroup == -1 || tdist < dist){
				if(!hover) m_currentcentroid = i;
				m_selectedgroup = i;
				dist = tdist;
			}		
		
		}
	}
	if(!hover && m_currentcentroid != -1){
		return h_centroid;
	}

	return h_space;
}


void C3DXView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	POSITION pos;
	CView * pView = NULL;
	pos = m_pDoc->GetFirstViewPosition();
	while(pos){
		pView = m_pDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView)))
			break;
		else
			pView = NULL;
	}

	CEditText2 edittext;
//	long newlength,newheight;
//	SIZE size;
//	HFONT oldfont,font;

	SetCursor(AfxGetApp()->LoadCursor(IDC_ARROW));
	
	UINT state = m_menu.GetMenuState(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_BYCOMMAND);
	
	long ret;

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
			if(m_compareobject < m_pDoc->GetNoOfRows()){
				m_pDoc->GetRowLabel(m_compareobject,m_infobar.name2);
				//m_infobar.ordin2[0] = m_pDoc->m_aRowXCoords[m_compareobject];
				//m_infobar.ordin2[1] = m_pDoc->m_aRowYCoords[m_compareobject];
				//m_infobar.ordin2[2] = m_pDoc->m_aRowZCoords[m_compareobject];
				if(m_compareobject < m_currentobject && 
					m_currentobject -1 < m_pDoc->m_AssocMatrixRows.m_aMatrix.GetSize() &&
					m_compareobject < m_pDoc->m_AssocMatrixRows.m_aMatrix[m_currentobject-1]->GetSize())
						m_infobar.assoc = m_pDoc->m_AssocMatrixRows.m_aMatrix.GetAt(m_currentobject-1)->GetAt(m_compareobject);
				else if(m_compareobject-1 < m_pDoc->m_AssocMatrixRows.m_aMatrix.GetSize() &&
					m_currentobject < m_pDoc->m_AssocMatrixRows.m_aMatrix[m_compareobject-1]->GetSize())
						m_infobar.assoc = m_pDoc->m_AssocMatrixRows.m_aMatrix.GetAt(m_compareobject-1)->GetAt(m_currentobject);
			}
			if(!m_SelOnly && pView != NULL){
				((CTableView*)pView)->m_pSelectedRows->SetAt(m_compareobject);
				pos = m_pDoc->GetFirstViewPosition();   
				while (pos != NULL)   
				{
					pView = m_pDoc->GetNextView(pos);   
					if(pView->IsKindOf(RUNTIME_CLASS(CColumnTotalsView)))
						pView->RedrawWindow();
				}
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
			if(state & MF_CHECKED)
			{
				if(m_comparecentroid < m_pDoc->m_AprioriGroups.GetSize()){
					m_infobar.name2.Format("%d",m_comparecentroid+1); 
					//m_infobar.ordin2[0] = m_pDoc->m_aGroupColours[m_comparecentroid].red*(m_pDoc->m_OrdXmax-m_pDoc->m_OrdXmin)+m_pDoc->m_OrdXmin;
					//m_infobar.ordin2[1] = m_pDoc->m_aGroupColours[m_comparecentroid].green*(m_pDoc->m_OrdYmax-m_pDoc->m_OrdYmin)+m_pDoc->m_OrdYmin;
					//m_infobar.ordin2[2] = m_pDoc->m_aGroupColours[m_comparecentroid].blue*(m_pDoc->m_OrdZmax-m_pDoc->m_OrdZmin)+m_pDoc->m_OrdZmin;
					//m_infobar.group2.SetAt(0,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->minimum);
					//m_infobar.group2.SetAt(1,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->first_quartile);
					//m_infobar.group2.SetAt(2,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->median);
					//m_infobar.group2.SetAt(3,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->mean);
					//m_infobar.group2.SetAt(4,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->third_quartile);
					//m_infobar.group2.SetAt(5,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->maximum);
				}
			}
			else
			{
				if(m_comparecentroid < m_pDoc->m_Groups.GetSize()){
					m_infobar.name2.Format("%d",m_pDoc->m_Groups.GetAt(m_comparecentroid)); 
					//m_infobar.ordin2[0] = m_pDoc->m_aGroupColours[m_comparecentroid].red*(m_pDoc->m_OrdXmax-m_pDoc->m_OrdXmin)+m_pDoc->m_OrdXmin;
					//m_infobar.ordin2[1] = m_pDoc->m_aGroupColours[m_comparecentroid].green*(m_pDoc->m_OrdYmax-m_pDoc->m_OrdYmin)+m_pDoc->m_OrdYmin;
					//m_infobar.ordin2[2] = m_pDoc->m_aGroupColours[m_comparecentroid].blue*(m_pDoc->m_OrdZmax-m_pDoc->m_OrdZmin)+m_pDoc->m_OrdZmin;
					//m_infobar.group2.SetAt(0,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->minimum);
					//m_infobar.group2.SetAt(1,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->first_quartile);
					//m_infobar.group2.SetAt(2,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->median);
					//m_infobar.group2.SetAt(3,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->mean);
					//m_infobar.group2.SetAt(4,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->third_quartile);
					//m_infobar.group2.SetAt(5,m_pDoc->m_Gstastats.GetAt(m_comparecentroid)->GetAt(m_Gstaattrib)->maximum);
				}
			}

			//calculate KWcomparison here...
			CKruskalWallace kw;
			kw.Create2(m_pDoc,&m_KWOrder,&m_KWValues,m_comparecentroid,m_currentcentroid);
		}
		break;
	case textboxmove:
	case stressboxmove:
		//update txtbox location + more?
		//clear m_hint
		m_hint = none;
		break;
	case textboxselection:
		//activate text box edit facility
		if(m_currenttextbox < m_TextBoxes.GetSize() && m_TextBoxes[m_currenttextbox] != NULL){
			edittext.m_colour = m_TextBoxes[m_currenttextbox]->colour;
			edittext.m_text = m_TextBoxes[m_currenttextbox]->contents;
			memcpy(&edittext.m_font,&(m_TextBoxes[m_currenttextbox]->font),sizeof(LOGFONT));
			strcpy(edittext.m_font.lfFaceName,m_TextBoxes[m_currenttextbox]->font.lfFaceName);
			
			if((ret = edittext.DoModal()) == IDOK){
				m_TextBoxes[m_currenttextbox]->location.right = m_local.x 
					+ edittext.m_text.GetLength() * 10; //Width of 1 char

				m_TextBoxes[m_currenttextbox]->contents = edittext.m_text;
				memcpy(&(m_TextBoxes[m_currenttextbox]->font),&edittext.m_font,sizeof(LOGFONT));
				strcpy(m_TextBoxes[m_currenttextbox]->font.lfFaceName,edittext.m_font.lfFaceName);

//				HDC hdc;
			//	if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
//					g_pddsBackBuffer->GetDC(&hdc);
			///	}else{
			//		hdc = GetDC()->m_hDC;
			//	}

		//		m_TextBoxes[m_currenttextbox]->font.lfQuality |= ANTIALIASED_QUALITY;
		//		font = CreateFontIndirect(&m_TextBoxes[m_currenttextbox]->font);
				
//				oldfont = (HFONT)SelectObject(hdc,font);
				
//				GetTextExtentPoint32(hdc,m_TextBoxes[m_currenttextbox]->contents,
//					m_TextBoxes[m_currenttextbox]->contents.GetLength(),&size);
				
//				SelectObject(hdc,oldfont);

			//	if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
//					g_pddsBackBuffer->ReleaseDC(hdc);
				//}else{
			//		hdc = GetDC()->m_hDC;
			//	}
  
//				newheight = size.cy;
//				newlength = size.cx;
				//newheight = (int)fabs((double)m_TextBoxes[m_currenttextbox]->font.lfHeight);
				//newlength = m_TextBoxes[m_currenttextbox]->contents.GetLength() * newheight;
				
//				m_TextBoxes[m_currenttextbox]->location.right = 
//					m_TextBoxes[m_currenttextbox]->location.left + newlength;				
//				m_TextBoxes[m_currenttextbox]->location.bottom = 
//					m_TextBoxes[m_currenttextbox]->location.top + newheight;

				m_TextBoxes[m_currenttextbox]->colour = edittext.m_colour;

			}else if(edittext.m_delete){
				//delete it
				delete m_TextBoxes[m_currenttextbox];
				m_TextBoxes.RemoveAt(m_currenttextbox);
			}
		}
		m_hint = none;

		break;
	case stressboxselection:
		edittext.m_text = m_stressbox.contents;
		//memcpy(&edittext.m_font,&(m_stressbox.font),sizeof(LOGFONT));
		//strcpy(edittext.m_font.lfFaceName,m_stressbox.font.lfFaceName);
		edittext.m_colour = m_stressbox.colour;

		if(IDOK == edittext.DoModal()){
			m_stressbox.location.right = m_local.x 
				+ edittext.m_text.GetLength() * 10; //Width of 1 char

			m_stressbox.contents = edittext.m_text;
		//	memcpy(&(m_stressbox.font),&edittext.m_font,sizeof(LOGFONT));
		//	strcpy(m_stressbox.font.lfFaceName,edittext.m_font.lfFaceName);

//			HDC hdc;
		//	if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
//				g_pddsBackBuffer->GetDC(&hdc);
			//}else{
		//		hdc = GetDC()->m_hDC;
		//	}

		//	m_stressbox.font.lfQuality |= ANTIALIASED_QUALITY;
		//	font = CreateFontIndirect(&m_stressbox.font);
//			oldfont = (HFONT)SelectObject(hdc,font);
			
//			GetTextExtentPoint32(hdc,m_stressbox.contents,
//				m_stressbox.contents.GetLength(),&size);
			
//			SelectObject(hdc,oldfont);

		//	if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
//				g_pddsBackBuffer->ReleaseDC(hdc);
		//	}else{
		//		hdc = GetDC()->m_hDC;
		//	}
  
//			newheight = size.cy;
//			newlength = size.cx;
			//newheight = (int)fabs((double)m_stressbox.font.lfHeight);
			//newlength = m_stressbox.contents.GetLength() * newheight;
			
//			m_stressbox.location.right = m_stressbox.location.left + newlength;				
//			m_stressbox.location.bottom = m_stressbox.location.top + newheight;

			m_stressbox.colour = edittext.m_colour;

		}else{
			//delete it
			//delete m_TextBoxes[m_currenttextbox];
			//m_TextBoxes.RemoveAt(m_currenttextbox);
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("no");
		}
		m_hint = none;
		break;
	default:
		break;
	}
	RedrawWindow();
	//CView::OnLButtonUp(nFlags, point);	
	//CView::OnLButtonUp(nFlags, point);
}

void C3DXView::InitRanges()
{
	float zmin=-0.5f*z_scale+0.5f,zmax=0.5f*z_scale+0.5f;
	float ymin=-0.5f*y_scale+0.5f,ymax=0.5f*y_scale+0.5f;
	float xmin=-0.5f*x_scale+0.5f,xmax=0.5f*x_scale+0.5f;
	if(m_pDoc->m_Ord3d == 2)
	{
		zmin = zmax = 0.5f;
		if(m_pDoc->m_Ord3d == 1)
		{
			ymin = ymax = 0.5f;
		}
	}

	m_ranges[0].x = xmin-.05f;m_ranges[0].y = -.1f;m_ranges[0].z = zmax;
	m_ranges[1].x = xmax-.05f;m_ranges[1].y = -.1f;m_ranges[1].z = zmax;
	m_ranges[2].x = xmin-.15f;m_ranges[2].y = ymin;m_ranges[2].z = zmax;
	m_ranges[3].x = xmin-.15f;m_ranges[3].y = ymax;m_ranges[3].z = zmax;
	m_ranges[4].x = xmax + .1f;m_ranges[4].y = ymin;m_ranges[4].z = zmax;
	m_ranges[5].x = xmax + .1f;m_ranges[5].y = ymin;m_ranges[5].z = zmin;

	m_ranges[6].x = xmin;m_ranges[6].y = ymax;m_ranges[6].z = zmax;
	m_ranges[7].x = xmin;m_ranges[7].y = ymin;m_ranges[7].z = zmax;
	m_ranges[8].x = xmax;m_ranges[8].y = ymin;m_ranges[8].z = zmax;
	m_ranges[9].x = xmax;m_ranges[9].y = ymin;m_ranges[9].z =zmin;
	
	m_ranges[10].x = xmin;m_ranges[10].y = ymax;m_ranges[10].z = zmax;
	m_ranges[11].x = xmin-.05f;m_ranges[11].y = ymax;m_ranges[11].z = zmax;
	m_ranges[12].x = xmin;m_ranges[12].y = ymin;m_ranges[12].z = zmax;
	m_ranges[13].x = xmin-.05f;m_ranges[13].y = ymin;m_ranges[13].z = zmax;
	m_ranges[14].x = xmin;m_ranges[14].y = ymin;m_ranges[14].z = zmax;
	m_ranges[15].x = xmin;m_ranges[15].y = -.05f;m_ranges[15].z = zmax;
	m_ranges[16].x = xmax;m_ranges[16].y = ymin;m_ranges[16].z = zmax;
	
	m_ranges[17].x = xmax;m_ranges[17].y = -.05f;m_ranges[17].z = zmax;
	m_ranges[18].x = xmax;m_ranges[18].y = ymin;m_ranges[18].z = zmax;
	m_ranges[19].x = xmax + .05f;m_ranges[19].y = ymin;m_ranges[19].z = zmax;
	m_ranges[20].x = xmax;m_ranges[20].y = ymin;m_ranges[20].z = zmin;
	m_ranges[21].x = xmax + .05f;m_ranges[21].y = ymin;m_ranges[21].z = zmin;

	for(long i=0;i<22;i++){
		m_ranges[i].x -= .5f;
		m_ranges[i].y -= .5f;
		m_ranges[i].z -= .5f;
	}
}

void C3DXView::DrawRanges(HDC hdc)
{
	D3DMATERIAL7 mtrl;
    ZeroMemory( &mtrl, sizeof(mtrl) );
    mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = 0.0f;
	mtrl.diffuse.r = 0.0f;
	mtrl.diffuse.g = 0.0f;
	mtrl.diffuse.b = 0.0f;
	

	float zmin=0,zmax=1;
	float ymin=0,ymax=1;
	if(m_pDoc->m_Ord3d == 2)
	{
		zmin = zmax = 0.5f;
		if(m_pDoc->m_Ord3d == 1)
		{
			ymin = ymax = 0.5f;
		}
	}

//	int err; 

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		g_pd3dDevice->SetMaterial( &mtrl );
		// Draw the wall, composed of a D3DVERTEX-type triangle strip
		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
			//err = g_pd3dDevice->BeginScene();
		//}
		if(m_pDoc->m_Ord3d == 3 /*|| !m_viewtype*/){
			g_pd3dDevice->DrawPrimitive( D3DPT_LINESTRIP, D3DFVF_VERTEX,
				m_ranges+6, 4, NULL );
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,
				m_ranges+10, 12, NULL );
		}else{
			g_pd3dDevice->DrawPrimitive( D3DPT_LINESTRIP, D3DFVF_VERTEX,
				m_ranges+6, 4-1, NULL );
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,
				m_ranges+10, 12-4, NULL );
		}
		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		//	err = g_pd3dDevice->EndScene();
		//}
	//}else{
		//GDIDRAWING
	//}

	CString text;
	float tx,ty,tz;
	long width = m_infowidth;

	SetTextColor(hdc,m_pDoc->m_rgbFontCells);
	SetBkMode(hdc,TRANSPARENT);
	//if(m_viewtype != 2){
		text.Format("%.3f",m_pDoc->m_OrdXmin);
		//App_Corner(g_pd3dDevice,&tx,&ty,&tz,-.05f-.5f,-.1f-.5f,1-.5f);		
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,(-.04f-.5f)*x_scale,(-.04f-.5f)*y_scale,(zmax-.5f)*z_scale);		
		TextOut(hdc,(long)tx+width-10,(long)ty,text,text.GetLength());	
		text.Format("%.3f",m_pDoc->m_OrdXmax);
		//App_Corner(g_pd3dDevice,&tx,&ty,&tz,.95f-.5f,-.1f-.5f,1-.5f);
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,(.98f-.5f)*x_scale,(-.04f-.5f)*y_scale,(zmax-.5f)*z_scale);
		TextOut(hdc,(long)tx+width-10,(long)ty,text,text.GetLength());	
	//}
	//if(m_viewtype != 3){		
		text.Format("%.3f",m_pDoc->m_OrdYmin);
		if(m_viewtype == 2){
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(-.04f-.5f),y_scale*(ymin-.5f),z_scale*(zmax-.5f+.04f));
		}else{
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(-.04f-.5f),y_scale*(ymin-.5f),z_scale*(zmax-.5f));
		}
		TextOut(hdc,(long)tx+width-10,(long)ty,text,text.GetLength());	
		text.Format("%.3f",m_pDoc->m_OrdYmax);
		if(m_viewtype == 2){
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(-.04f-.5f),y_scale*(ymax-.5f),z_scale*(zmax-.5f+.04f));
		}else{
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(-.04f-.5f),y_scale*(ymax-.5f),z_scale*(zmax-.5f));
		}		
		TextOut(hdc,(long)tx+width-10,(long)ty,text,text.GetLength());
	//}
	
	//if(m_pDoc->m_Ord3d == 3 && m_viewtype != 1){
		text.Format("%.3f",m_pDoc->m_OrdZmin);
		if(m_viewtype == 2){
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(1.04f-.5f),y_scale*(ymin-.5f-.04f),z_scale*(1-.5f));
		}else{
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(1.04f-.5f),y_scale*(ymin-.5f),z_scale*(1-.5f));
		}

		TextOut(hdc,(long)tx+width-10,(long)ty,text,text.GetLength());	
		text.Format("%.3f",m_pDoc->m_OrdZmax);
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(1.04f-.5f),y_scale*(ymin-.5f),z_scale*(0-.5f));
		if(m_viewtype == 2){
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(1.04f-.5f),y_scale*(ymin-.5f-.04f),z_scale*(0-.5f));
		}else{
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(1.04f-.5f),y_scale*(ymin-.5f),z_scale*(0-.5f));
		}
		TextOut(hdc,(long)tx+width-10,(long)ty,text,text.GetLength());	
	//}
}
void C3DXView::DrawRangesDC(HDC hdc,long pos)
{
	CString text;
	float tx,ty,tz,tz2;

	if(pos == 0){//first instance (behind)
		//lines 
		App_Corner(g_pd3dDevice,&tx,&ty,&tz2,-.5f*x_scale,y_scale*.5f,z_scale*-.5f);
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,.5f*x_scale,y_scale*-.5f,-.5f*z_scale);
		if(tz2 <= tz){
			MoveToEx(hdc,(long)tx+m_infowidth,(long)ty,NULL);
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*.5f,y_scale*-.5f,z_scale*.5f);
			LineTo(hdc,(long)tx+m_infowidth,(long)ty);
		}
		App_Corner(g_pd3dDevice,&tx,&ty,&tz2,x_scale*.5f,y_scale*.5f,z_scale*-.5f);
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*.5f,y_scale*-.5f,z_scale*.5f);
		if(tz2 <= tz){
			MoveToEx(hdc,(long)tx+m_infowidth,(long)ty,NULL);
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*-.5f,y_scale*-.5f,z_scale*.5f);
			LineTo(hdc,(long)tx+m_infowidth,(long)ty);
		}
		App_Corner(g_pd3dDevice,&tx,&ty,&tz2,x_scale*.5f,y_scale*-.5f,z_scale*-.5f);
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*-.5f,y_scale*-.5f,z_scale*.5f);
		if(tz2 <= tz){
			MoveToEx(hdc,(long)tx+m_infowidth,(long)ty,NULL);
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*-.5f,y_scale*.5f,z_scale*.5f);
			LineTo(hdc,(long)tx+m_infowidth,(long)ty);
		}
	}else{//last instance
		
		//lines 
		App_Corner(g_pd3dDevice,&tx,&ty,&tz2,x_scale*-.5f,y_scale*.5f,z_scale*-.5f);
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*.5f,y_scale*-.5f,z_scale*-.5f);
		if(tz2 > tz){
			MoveToEx(hdc,(long)tx+m_infowidth,(long)ty,NULL);
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*.5f,y_scale*-.5f,z_scale*.5f);
			LineTo(hdc,(long)tx+m_infowidth,(long)ty);
		}
		App_Corner(g_pd3dDevice,&tx,&ty,&tz2,x_scale*.5f,y_scale*.5f,z_scale*-.5f);
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*.5f,y_scale*-.5f,z_scale*.5f);
		if(tz2 > tz){
			MoveToEx(hdc,(long)tx+m_infowidth,(long)ty,NULL);
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*-.5f,y_scale*-.5f,z_scale*.5f);
			LineTo(hdc,(long)tx+m_infowidth,(long)ty);
		}
		App_Corner(g_pd3dDevice,&tx,&ty,&tz2,x_scale*.5f,y_scale*-.5f,z_scale*-.5f);
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*-.5f,y_scale*-.5f,z_scale*.5f);
		if(tz2 > tz){
			MoveToEx(hdc,(long)tx+m_infowidth,(long)ty,NULL);
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*-.5f,y_scale*.5f,z_scale*.5f);
			LineTo(hdc,(long)tx+m_infowidth,(long)ty);
		}

		long width = m_infowidth;

		SetTextColor(hdc,m_pDoc->m_rgbFontCells);
		SetBkMode(hdc,TRANSPARENT);
		text.Format("%.3f",m_pDoc->m_OrdXmin);
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(-.05f-.5f),y_scale*(-.1f-.5f),z_scale*(1-.5f));
		TextOut(hdc,(long)tx+width-10,(long)ty,text,text.GetLength());	
		text.Format("%.3f",m_pDoc->m_OrdXmax);
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(.95f-.5f),y_scale*(-.1f-.5f),z_scale*(1-.5f));
		TextOut(hdc,(long)tx+width-10,(long)ty,text,text.GetLength());	

		if(m_pDoc->m_Ord3d >= 2)
		{
			text.Format("%.3f",m_pDoc->m_OrdYmin);
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(-.15f-.5f),y_scale*(0-.5f),z_scale*(1-.5f));
			TextOut(hdc,(long)tx+width-10,(long)ty,text,text.GetLength());	
			text.Format("%.3f",m_pDoc->m_OrdYmax);
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(-.15f-.5f),y_scale*(1-.5f),z_scale*(1-.5f));
			TextOut(hdc,(long)tx+width-10,(long)ty,text,text.GetLength());
		}
        		
		if(m_pDoc->m_Ord3d == 3){
			text.Format("%.3f",m_pDoc->m_OrdZmin);
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(1.1f-.5f),y_scale*(0-.5f),z_scale*(1-.5f));
			TextOut(hdc,(long)tx+width-10,(long)ty,text,text.GetLength());	
			text.Format("%.3f",m_pDoc->m_OrdZmax);
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,x_scale*(1.1f-.5f),y_scale*(0-.5f),z_scale*(0-.5f));
			TextOut(hdc,(long)tx+width-10,(long)ty,text,text.GetLength());	
		}
	}
}

void C3DXView::OnDisplaycentroids() 
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
		if(m_colColourMap != -1) {
			m_colColourMap = -1;
			m_menu.CheckMenuItem(ID__PICKACOLUMN,MF_UNCHECKED);
		}
	}
	m_currenttextbox = -1;
	m_currentobject = m_currentcentroid = -1;
	m_compareobject = m_comparecentroid = -1;
	m_hint = none;

	RedrawWindow();	
}

void C3DXView::OnShowgrouping() 
{
	if(m_legend_instance) return;
	if(m_Grouping){
		m_Grouping = false;
		m_menu.CheckMenuItem(ID__SHOWGROUPING,MF_UNCHECKED);
	}else{
		m_Grouping = true;
		m_Usergroupcolours = false;
		m_menu.CheckMenuItem(ID__SHOWGROUPING,MF_CHECKED);
		m_menu.CheckMenuItem(ID__USEUSERGROUPCOLOURS,MF_UNCHECKED);
		m_colColourMap = -1;
		m_menu.CheckMenuItem(ID__PICKACOLUMN,MF_UNCHECKED);
	}
	RedrawWindow();	
}

void C3DXView::OnShowranges() 
{
	if(m_legend_instance) return;

	if(m_Showranges){
		m_Showranges = false;
		m_menu.CheckMenuItem(ID__SHOWRANGES,MF_UNCHECKED);
	}else{
		m_Showranges = true;
		m_menu.CheckMenuItem(ID__SHOWRANGES,MF_CHECKED);
	}
	RedrawWindow();
	
}

void C3DXView::OnShowselectedonly() 
{
	if(m_legend_instance) return;

	if(m_SelOnly){
		m_SelOnly = false;
		m_menu.CheckMenuItem(ID__SHOWSELECTEDONLY,MF_UNCHECKED);
	}else{
		m_SelOnly = true;
		m_menu.CheckMenuItem(ID__SHOWSELECTEDONLY,MF_CHECKED);
	}

	RedrawWindow();
}

void C3DXView::OnUseusergroupcolours() 
{
	if(m_legend_instance) return;

	if(m_Usergroupcolours){
		m_Usergroupcolours = false;
		m_menu.CheckMenuItem(ID__USEUSERGROUPCOLOURS,MF_UNCHECKED);
	}else{
		m_Usergroupcolours = true;
		m_Grouping = false;
		m_menu.CheckMenuItem(ID__USEUSERGROUPCOLOURS,MF_CHECKED);
		m_menu.CheckMenuItem(ID__SHOWGROUPING,MF_UNCHECKED);
		m_colColourMap = -1;
		m_menu.CheckMenuItem(ID__PICKACOLUMN,MF_UNCHECKED);
	}
	RedrawWindow();
}

void C3DXView::InitLine()
{		

	float Xrange = (float)(m_pDoc->m_OrdXmax-m_pDoc->m_OrdXmin);
	float Yrange = (float)(m_pDoc->m_OrdYmax-m_pDoc->m_OrdYmin);
	float Zrange = (float)(m_pDoc->m_OrdZmax-m_pDoc->m_OrdZmin);
	//works only if min's are negative
	// finds ranged zero of x,y,z
	float Xcentre = (float)(-m_pDoc->m_OrdXmin/Xrange);
	float Ycentre = (float)(-m_pDoc->m_OrdYmin/Yrange);
	float Zcentre = (float)(-m_pDoc->m_OrdZmin/Zrange);

	//draw pcc vectors found in m_PccVectors
//	float xpos,ypos;//,zpos;

	if(m_line != NULL)
		delete [] m_line;
	m_line = new D3DVERTEX[2];
	if(m_line == NULL){		
		return;
	}	

	for(long i=0;i<2;i++){				
		m_line[i].x = 0;
		m_line[i].y = 0;
		m_line[i].z = 0;		
	}

	if(m_line2 != NULL)
		delete [] m_line2;
	m_line2 = new D3DVERTEX[2];
	if(m_line2 == NULL){		
		return;
	}	

}

void C3DXView::InitPcc()
{		
	float Xrange = (float)(m_pDoc->m_OrdXmax-m_pDoc->m_OrdXmin);
	float Yrange = (float)(m_pDoc->m_OrdYmax-m_pDoc->m_OrdYmin);
	float Zrange = (float)(m_pDoc->m_OrdZmax-m_pDoc->m_OrdZmin);
	//works only if min's are negative
	// finds ranged zero of x,y,z
	float Xcentre = (float)(-m_pDoc->m_OrdXmin/Xrange);
	float Ycentre = (float)(-m_pDoc->m_OrdYmin/Yrange);
	float Zcentre = (float)(-m_pDoc->m_OrdZmin/Zrange);

	//draw pcc vectors found in m_PccVectors
	float xpos,ypos,zpos;

	if(m_pcc != NULL)
		delete m_pcc;
	m_pcc = new D3DVERTEX[m_PccVectors.GetSize()*2];
	if(m_pcc == NULL){
		m_pcc_size = 0;
		return;
	}
	m_pcc_size = m_PccVectors.GetSize()*2;
	for(long i=0;i<m_PccVectors.GetSize() &&
		m_pDoc->m_Vectors.GetSize() > m_PccVectors[i];i++){		
		xpos = (float)m_pDoc->m_Vectors.GetAt(m_PccVectors.GetAt(i))->GetAt(0);
		if(m_pDoc->m_Ord3d >= 2) ypos = (float)m_pDoc->m_Vectors.GetAt(m_PccVectors.GetAt(i))->GetAt(1);
		if(m_pDoc->m_Ord3d == 3) zpos = (float)m_pDoc->m_Vectors.GetAt(m_PccVectors.GetAt(i))->GetAt(2);
		else if(m_pDoc->m_Ord3d == 2) zpos = 0;
		else 
		{
			zpos = 0;
			ypos = 0;
		}
		
		m_pcc[2*i].x = xpos/2.0f+Xcentre-.5f;
		m_pcc[2*i+1].x = Xcentre-.5f;

		if(m_pDoc->m_Ord3d >= 2)
		{
			m_pcc[2*i].y = ypos/2.0f+Ycentre-.5f;	
			m_pcc[2*i+1].y = Ycentre-.5f;
		}
		if(m_pDoc->m_Ord3d == 3){ 
			m_pcc[2*i].z = zpos/2.0f+Zcentre-.5f;
			m_pcc[2*i+1].z = Zcentre-.5f;
		}else if(m_pDoc->m_Ord3d == 2){
			m_pcc[2*i].z = zpos;
			m_pcc[2*i+1].z = zpos;
		}else
		{
			m_pcc[2*i].z = zpos;
			m_pcc[2*i+1].z = zpos;
			m_pcc[2*i].y = ypos;
			m_pcc[2*i+1].y = ypos;
		}
	}
}

void C3DXView::DrawPcc(HDC hdc)
{
	D3DMATERIAL7 mtrl;
    ZeroMemory( &mtrl, sizeof(mtrl) );
	mtrl.ambient.r = mtrl.diffuse.r = 1.0f;
	mtrl.ambient.g = mtrl.diffuse.g = 0.0f;
	mtrl.ambient.b = mtrl.diffuse.b = 0.0f;
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		g_pd3dDevice->SetMaterial( &mtrl );
	//}else{

	//}

	// Draw the wall, composed of a D3DVERTEX-type triangle strip
	if(m_PccVectors.GetSize()*2 != m_pcc_size)
		InitPcc();

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		if(m_pcc != NULL)
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,
				m_pcc, m_PccVectors.GetSize()*2, NULL );
	//}else{
		//GDIDRAWING
	//}
	
	SetTextColor(hdc,m_pDoc->m_rgbFontCells);
	SetTextAlign(hdc,TA_TOP);
	SetBkMode(hdc,TRANSPARENT);

	double x,y,z;
	float tx,ty,tz;
	CString text;
	long width = m_infowidth;
	//if(m_saveimage) width = 0;
	//else width = m_infowidth;

	for(long i=0;i<m_PccVectors.GetSize();i++){		
		x = m_pcc[i*2].x;
		y = m_pcc[i*2].y;
		z = m_pcc[i*2].z;	
		
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)x,
			(float)y,(float)z);		

		if(m_PccVectors.GetAt(i) < m_pDoc->GetNoOfColumns())
			m_pDoc->GetColumnLabel(m_PccVectors.GetAt(i),text);

		TextOut(hdc,(long)tx+width,(long)ty,text,text.GetLength());
	}
}
void C3DXView::DrawPcc(HDC hdc,long pos)
{
	SetBkMode(hdc,TRANSPARENT);
	// Draw the wall, composed of a D3DVERTEX-type triangle strip
	if(m_PccVectors.GetSize()*2 != m_pcc_size)
		InitPcc();

	float tx,ty,tz;

	if(m_pcc != NULL && pos == 0){
		for(long i=0;i<m_PccVectors.GetSize()*2;i+=2){
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,m_pcc[i].x,
				m_pcc[i].y,m_pcc[i].z);
			MoveToEx(hdc,(long)tx+m_infowidth,(long)ty,NULL);
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,m_pcc[i+1].x,
				m_pcc[i+1].y,m_pcc[i+1].z);
			LineTo(hdc,(long)tx+m_infowidth,(long)ty);
		}
	
		
SetTextColor(hdc,m_pDoc->m_rgbFontCells);
		SetTextAlign(hdc,TA_TOP);

		double x,y,z;
		float tx,ty,tz;
		CString text;
		long width = m_infowidth;
	
		for(long i=0;i<m_PccVectors.GetSize();i++){		
			x = m_pcc[i*2].x;
			y = m_pcc[i*2].y;
			z = m_pcc[i*2].z;	
			
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)x,
				(float)y,(float)z);		

			if(m_PccVectors.GetAt(i) < m_pDoc->GetNoOfColumns())
				m_pDoc->GetColumnLabel(m_PccVectors.GetAt(i),text);
	
			TextOut(hdc,(long)tx+width,(long)ty,text,text.GetLength());
		}
	}
}


void C3DXView::OnSelectpccvectors() 
{
	if(m_legend_instance) return;

	CPccVectorSelection dlg;
	dlg.m_pDoc = m_pDoc;
	dlg.m_PccVectors = &m_PccVectors;
	dlg.DoModal();
	InitPcc();
	RedrawWindow();
}

void C3DXView::OnResetRotation() 
{
	if(m_legend_instance) return;

	m_viewtype = 0;

	D3DMATRIX mat;
    mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
    mat._12 = mat._13 = mat._14 = mat._41 = 0.0f;
    mat._21 = mat._23 = mat._24 = mat._42 = 0.0f;
    mat._31 = mat._32 = mat._34 = mat._43 = 0.0f;
    
    D3DMATRIX matProj = mat;
    matProj._11 =  3.0f;
    matProj._22 =  3.0f;
	matProj._33 = 1.5f;
    matProj._34 =  1.5f;
    matProj._43 = -1.0f;
    matProj._44 =  0.0f;
//	if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		g_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );
	//}else{
	//	m_matProj = mat;
	//}


	m_vEyePt = D3DVECTOR( 0, 0, -0.6f);
    m_vLookatPt = D3DVECTOR( 0,0,0);
    m_vUpVec    = D3DVECTOR( 0, 1,0 );
	m_vRight = D3DVECTOR(1,0,0);

	CRect rect;
	GetClientRect(rect);
	rect.left += m_infowidth;

	//RedrawWindow(rect);

	UpdateZOrders();

	UpdateWindow();
	RedrawWindow();
}

void C3DXView::OnUpdateResetRotation(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(false);
	if(m_legend_instance) return;

	//pCmdUI->Enable(m_pDoc->m_Ord3d);		
	pCmdUI->Enable(true);		
}

void C3DXView::OnUpdateSelectpccvectors(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(false);	
	if(m_legend_instance) return;
	for(long i=0;i<m_pDoc->GetNoOfColumns();i++){
		if(i < m_pDoc->m_Vectors.GetSize() &&
			m_pDoc->m_Vectors.GetSize() &&
			m_pDoc->m_Vectors[i]->GetAt(0) != ANALYSIS_VALUE){
			pCmdUI->Enable(true);
			return;
		}
	}	
}

void C3DXView::OnClose() 
{
		
	((CFrameWnd*)GetParent())->DestroyWindow();	
}

void C3DXView::OnAddtextbox() 
{
	if(m_legend_instance) return;

	TextBox2 * newtextbox = new TextBox2;
	CEditText2 edittext;

	//newtextbox->font.lfHeight = -14;
	//newtextbox->font.lfWidth = 0;
	//newtextbox->font.lfEscapement = 0;
	//newtextbox->font.lfOrientation = 0;
	//newtextbox->font.lfWeight = FW_NORMAL;
	//newtextbox->font.lfItalic = FALSE;
	//newtextbox->font.lfUnderline = FALSE;
	//newtextbox->font.lfStrikeOut = FALSE;
	//newtextbox->font.lfCharSet = ANSI_CHARSET;
	//newtextbox->font.lfOutPrecision = OUT_DEFAULT_PRECIS;
	//newtextbox->font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	//newtextbox->font.lfQuality = PROOF_QUALITY;
	//newtextbox->font.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	//strcpy(newtextbox->font.lfFaceName,"Times");
	newtextbox->font = m_pDoc->m_lfCells;
	strcpy(newtextbox->font.lfFaceName,m_pDoc->m_lfCells.lfFaceName);

	memcpy(&edittext.m_font,&(newtextbox->font),sizeof(LOGFONT));
	strcpy(edittext.m_font.lfFaceName,newtextbox->font.lfFaceName);

	edittext.m_colour = 0;

	SIZE size;
	HFONT font,oldfont;
	
	if(IDOK == edittext.DoModal()){
		newtextbox->contents = edittext.m_text;			
		memcpy(&(newtextbox->font),&edittext.m_font,sizeof(LOGFONT));
		strcpy(newtextbox->font.lfFaceName,edittext.m_font.lfFaceName);

		long newheight = (long)fabs((double)edittext.m_font.lfHeight);
		long newlength = (long)(edittext.m_text.GetLength() * newheight);

		HDC hdc;
	//	if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
			g_pddsBackBuffer->GetDC(&hdc);
		//}else{
	//		hdc = GetDC()->m_hDC;
	//	}

		newtextbox->font.lfQuality |= ANTIALIASED_QUALITY;
		font = CreateFontIndirect(&newtextbox->font);
		oldfont = (HFONT)SelectObject(hdc,font);
		
		GetTextExtentPoint32(hdc,newtextbox->contents,
			newtextbox->contents.GetLength(),&size);
		
		SelectObject(hdc,oldfont);

		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
			g_pddsBackBuffer->ReleaseDC(hdc);
		//}else{
		//	hdc = GetDC()->m_hDC;
		//}

		newheight = size.cy;
		newlength = size.cx;
		
		newtextbox->location.top = m_local.y;
		newtextbox->location.left = m_local.x;
		newtextbox->location.bottom = m_local.y + newheight;
		newtextbox->location.right = m_local.x + newlength;
		
		newtextbox->colour = edittext.m_colour;
		
		m_TextBoxes.Add(newtextbox);
	}else{
		delete newtextbox;
	}
	RedrawWindow();	
}

void C3DXView::OnDisplaylegend() 
{
	if(m_legend_instance)
	{
	/*			        POSITION pos = m_pDoc->GetFirstViewPosition();   
		int instance_found = 0;
		while (pos != NULL)   
		{
			CView* pView = m_pDoc->GetNextView(pos);   
			if(pView == (CView*)this)
			{				
			}
			else if(pView->IsKindOf(RUNTIME_CLASS(C3DXView)))
			{
				((C3DXView*)pView)->m_legend_view = NULL;
				((C3DXView*)pView)->m_Legend = false;
				GetParentFrame()->PostMessage(WM_CLOSE);		
				break;
			}		
		}

	*/
		GetParentFrame()->PostMessage(WM_CLOSE);		
		return;
	}

	if(m_Legend){

		if(m_legend_view != NULL && m_legend_view != (void*)1)
			((CView*)m_legend_view)->GetParentFrame()->PostMessage(WM_CLOSE);
		m_legend_view = NULL;

		m_Legend = false;
		m_menu.CheckMenuItem(ID__DISPLAYLEGEND,MF_UNCHECKED);
	}else{		
		m_pDoc->OnView3dgraphDirectx70a_forward();
        POSITION pos = m_pDoc->GetFirstViewPosition();   
		int instance_found = 0;
		while (pos != NULL)   
		{
			CView* pView = m_pDoc->GetNextView(pos);   
			if(pView == (CView*)this)
			{				
			}
			else if(pView->IsKindOf(RUNTIME_CLASS(C3DXView)) && ((C3DXView*)pView)->m_legend_view==(void*)1)
			{
				((C3DXView*)pView)->m_legend_view = NULL;
				m_legend_view = (void*) pView;		

				((C3DXView*)m_legend_view)->m_colColourMap = m_colColourMap;
				((C3DXView*)m_legend_view)->m_colColours.SetSize(m_colColours.GetSize());
				for(long q = 0;q < m_colColours.GetSize();q++) {
					((C3DXView*)m_legend_view)->m_colColours.SetAt(q,m_colColours[q]);
				}
				
				break;
			}		
		}
		m_Legend = true;
		m_menu.CheckMenuItem(ID__DISPLAYLEGEND,MF_CHECKED);
		//if(((CPatnApp*)AfxGetApp())->m_ordtype != 1){
		//	if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Objects cannot be selected while\r\nthe legend is displayed\r\nwhen using GDI drawing type.");
		//}
	}
	RedrawWindow();	
}

void C3DXView::DrawLegend(HDC hdc)
{

	POSITION pos;
	CView * pView = NULL;
	pos = m_pDoc->GetFirstViewPosition();
	while(pos){
		pView = m_pDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView)))
			break;
		else
			pView = NULL;
	}
	UINT state = m_menu.GetMenuState(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_BYCOMMAND);

	CRect rect;
	if(!m_saveimage && !m_printimage)
		GetClientRect(rect);
	else
		rect = g_rcScreenRect;

	long i,x,nCols,nRows,nObjects;
	CString text,str;
	
	SIZE size;

	//iterate here to get the text to fit into the area provided
	static long fontsize = 0;
	static long usefontsize = m_pDoc->m_lfCells.lfHeight;
	static CRect windowrect = rect;
	static long lastgroups = m_Centroid;

	long height;

	bool groups = m_Centroid || m_Usergroupcolours || m_Grouping;

	if(groups)
		nObjects = m_pDoc->m_aGroupColours.GetSize();
	else
		nObjects = m_pDoc->m_aObjectColours.GetSize();
	if(groups && state & MF_CHECKED)
		nObjects = m_pDoc->m_aAprioriGroupColours.GetSize()-1;

	if(false){	
		usefontsize = m_pDoc->m_lfCells.lfHeight;
		while(abs(usefontsize) > 1 && false)
		{
			if(m_printimage)
				GetTextExtentPoint32(hdc,"a ssstttrrriiinnnggga ssstttrrriiinnnggg",40,&size);
			else
				GetTextExtentPoint32(hdc,"a ssstttrrriiinnnggga ssstttrrriiinnnggg",30,&size);
	
			if(m_printimage)
				x = 0;
			else if(m_saveimage)
				x = m_infowidth + rect.Height();
			else
				//x = m_infowidth + rect.Height();
				x = 0;

			height = (long)(size.cy*1.5);

			nRows = (long)(floor(1.0*rect.Height()/height)-2);
			nCols = (long)(ceil(nObjects/(double)(nRows)));

			if(nCols == 0)
				return;

			//is the font size too big?  If so, reduce it.
			if(size.cx * nCols > rect.Width() - x)
			{
				if(usefontsize < 1) usefontsize++;
				else usefontsize--;
			}
			else
			{
				break;
			}
		}

		windowrect = rect;
		fontsize = m_pDoc->m_lfCells.lfHeight;
		lastgroups = m_Centroid;
	}

	if(m_printimage)
		GetTextExtentPoint32(hdc,"a ssstttrrriiinnnggga ssstttrrriiinnnggg",40,&size);
	else
		GetTextExtentPoint32(hdc,"a ssstttrrriiinnnggga ssstttrrriiinnnggg",30,&size);
	
	if(m_saveimage)
		x = m_infowidth + rect.Height();
	else
		x = 10;
	height = (long)(size.cy*1.5);
	nRows = (long)(floor(1.0*rect.Height()/height)-2);
	nCols = (long)(ceil(nObjects/(double)(nRows)));

	long row = 0;
	
	if(nRows == 0){
		return;
	}
	nCols = (long)(ceil(nObjects/(double)(nRows)));
	if(m_printimage)
		x = 0;
	else if(m_saveimage)
		x = m_infowidth + rect.Height() + height*2;
	else
		//x = m_infowidth + rect.Height() + height*2;
		x = height*2;

	text = "LEGEND";

	TextOut(hdc,x,height,text,text.GetLength());

	unsigned char r,g,b;
	unsigned long clr;
	CBrush brush,*oldbrush;
	HBRUSH br = NULL;
	long grp;

	CPatnApp * app = (CPatnApp*)AfxGetApp();

	long heightinc = 0;
	i = 0;
	//setup with scrollbar pos
	m_legendNumberOfRows = nRows;
	if(m_legendPos > nObjects)
		m_legendPos = (nCols-1)*nRows;
	if(m_legendNumberOfRows == 0)
		i = 0;
	else	
		i = (long) (m_legendPos/m_legendNumberOfRows)*m_legendNumberOfRows;

	if(i<0)
	{
		i = 0;
		m_legendPos = 0;
	}

	for(;i<nObjects;i++){
		if(!(groups || (!m_SelOnly || (pView != NULL && ((CTableView*)pView)->m_pSelectedRows->IsSelected(i)))))
			continue;
		//1st, draw label
		if(groups)
		{
			if(state & MF_CHECKED && m_pDoc->m_aAprioriGroupColours.GetSize()-1 > i)
			{
                text.Format("Group %d",i+1);//m_pDoc->m_AprioriGroups[i]);
			}
			else
			{
				text.Format("Group %d",m_pDoc->m_Groups[i]);
			}
		}
		else
			m_pDoc->GetRowLabel(i,text);

		CRect rct(CPoint(x+height,height*(2+heightinc)),CSize(size.cx-(int)(height*1.5),size.cy));
		ExtTextOut(hdc,x+height,height*(2+heightinc),ETO_CLIPPED,rct,text,text.GetLength(),NULL);
			
		//2nd, draw object, symbol, with appropriate colour
		if(groups) {
			if(!m_Usergroupcolours){	
				if(state & MF_CHECKED && m_pDoc->m_aAprioriGroupColours.GetSize())
				{
					r = (unsigned char)(m_pDoc->m_aAprioriGroupColours[i].red*255);
					g = (unsigned char)(m_pDoc->m_aAprioriGroupColours[i].green*255);
					b = (unsigned char)(m_pDoc->m_aAprioriGroupColours[i].blue*255);
				}
				else
				{
					r = (unsigned char)(m_pDoc->m_aGroupColours[i].red*255);
					g = (unsigned char)(m_pDoc->m_aGroupColours[i].green*255);
					b = (unsigned char)(m_pDoc->m_aGroupColours[i].blue*255);
				}
				if(m_pDoc->m_Ord3d == 2) b = 128;
				else if(m_pDoc->m_Ord3d == 1) {b = 128; g = 128; }
			}else{
				if(state & MF_CHECKED && i<m_pDoc->m_aGroupColours2.GetSize()){
					r = (unsigned char)(m_pDoc->m_aGroupColours2[i].red*255);
					g = (unsigned char)(m_pDoc->m_aGroupColours2[i].green*255);
					b = (unsigned char)(m_pDoc->m_aGroupColours2[i].blue*255);

				}else if(i<m_pDoc->m_UserGroupColours.GetSize())
				{
					r = (unsigned char)(m_pDoc->m_UserGroupColours[i].red*255);
					g = (unsigned char)(m_pDoc->m_UserGroupColours[i].green*255);
					b = (unsigned char)(m_pDoc->m_UserGroupColours[i].blue*255);
				}
			}
			clr = r+256*(g+256*(b ));

			oldbrush = (CBrush*)SelectObject(hdc,br = CreateSolidBrush(clr));
			
			if(m_Symbols){
				if(state & MF_CHECKED)
				{
					if(i < m_pDoc->m_PatternApriori.GetSize()){
						if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternApriori[i],hdc,x+height/2,height*(2+heightinc),height/20);
						else m_tex.GDIPatterns(m_pDoc->m_PatternApriori[i],hdc,x+height/2,height*(2+heightinc),15*app->m_ordobjectsize);
					}
				}
				else
				{
					if(i < m_pDoc->m_PatternGroups.GetSize()){
						if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternGroups[i],hdc,x+height/2,height*(2+heightinc),height/20);
						else m_tex.GDIPatterns(m_pDoc->m_PatternGroups[i],hdc,x+height/2,height*(2+heightinc),15*app->m_ordobjectsize);
					}
				}
			}else{
				Ellipse(hdc,x+2,height*(2+heightinc)+2,x+height-2,height*(2+heightinc+1)-2);
			}
			if(oldbrush != NULL)
				SelectObject(hdc,oldbrush);

			if(br != NULL)
				DeleteObject(br);
		}else { 
			//if statement for selected objects only display
			if(m_Grouping){
				if(state & MF_CHECKED)
				{
					if(m_pDoc->m_AprioriGroups.GetSize() > i && m_pDoc->m_AprioriGroups[i] > 0){
						if(i < m_pDoc->GetNoOfRows() &&
							m_pDoc->GetRowGroups(i)-1 >= 0 &&
							m_pDoc->GetRowGroups(i)-1 < m_pDoc->m_aAprioriGroupColours.GetSize()){
							r = (unsigned char)(m_pDoc->m_aAprioriGroupColours[m_pDoc->m_AprioriGroups[i]-1].red*255);
							g = (unsigned char)(m_pDoc->m_aAprioriGroupColours[m_pDoc->m_AprioriGroups[i]-1].green*255);
							b = (unsigned char)(m_pDoc->m_aAprioriGroupColours[m_pDoc->m_AprioriGroups[i]-1].blue*255);
						}
					}
				}
				else
				{
					if(m_pDoc->GetRowGroups(i) > 0){
						if(i < m_pDoc->GetNoOfRows() &&
							m_pDoc->GetRowGroups(i)-1 >= 0 &&
							m_pDoc->GetRowGroups(i)-1 < m_pDoc->m_aGroupColours.GetSize()){
							r = (unsigned char)(m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].red*255);
							g = (unsigned char)(m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].green*255);
							b = (unsigned char)(m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].blue*255);
						}
					}
				}
				if(m_pDoc->m_Ord3d == 2) b = 128;
				else if(m_pDoc->m_Ord3d == 1) {b = 128; g = 128;}
			}else if(m_Usergroupcolours){
				if(state & MF_CHECKED)
				{
					if(m_pDoc->m_AprioriGroups.GetSize() > i && m_pDoc->m_AprioriGroups[i] > 0){
						if(i < m_pDoc->GetNoOfRows() &&
							m_pDoc->GetRowGroups(i)-1 >= 0 &&
							m_pDoc->GetRowGroups(i)-1 < m_pDoc->m_aAprioriGroupColours.GetSize()){
							r = (unsigned char)(m_pDoc->m_aGroupColours2[m_pDoc->m_AprioriGroups[i]-1].red*255);
							g = (unsigned char)(m_pDoc->m_aGroupColours2[m_pDoc->m_AprioriGroups[i]-1].green*255);
							b = (unsigned char)(m_pDoc->m_aGroupColours2[m_pDoc->m_AprioriGroups[i]-1].blue*255);
						}
					}
				}else if(i < m_pDoc->GetNoOfRows() && m_pDoc->GetRowGroups(i)-1 > 0 &&
					m_pDoc->GetRowGroups(i)-1 < m_pDoc->m_UserGroupColours.GetSize()){
					r = (unsigned char)(m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].red*255);
					g = (unsigned char)(m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].green*255);
					b = (unsigned char)(m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].blue*255);
				}
			}else{
				if(i < m_pDoc->m_aObjectColours.GetSize()){
					if(m_colColourMap >= 0) {
							r = (unsigned char) ((m_colColours[i] & 0xff0000) >> 16);
							g = (unsigned char) ((m_colColours[i] & 0x00ff00) >> 8);
							b = (unsigned char) (m_colColours[i] & 0x0000ff);
						} else  {
							r = (unsigned char)(m_pDoc->m_aObjectColours[i].red*255);
							g = (unsigned char)(m_pDoc->m_aObjectColours[i].green*255);
							b = (unsigned char)(m_pDoc->m_aObjectColours[i].blue*255);			
						}
				}
				if(m_pDoc->m_Ord3d == 2) b = 128;
				else if(m_pDoc->m_Ord3d == 1) {b = 128; g = 128;}
			}
			clr = r+256*(g+256*(b ));

			oldbrush = (CBrush*)SelectObject(hdc,br = CreateSolidBrush(clr));

			if(m_Symbols && !m_Grouping && i < m_pDoc->m_PatternObjects.GetSize()){
				if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternObjects[i],hdc,x+height/2,height*(2+heightinc),height/20);//45*app->m_ordobjectsize);
				else m_tex.GDIPatterns(m_pDoc->m_PatternObjects[i],hdc,x+height/2,height*(2+heightinc),15*app->m_ordobjectsize);//15*app->m_ordobjectsize);
			}else if(m_Grouping && m_Symbols && i < m_pDoc->GetNoOfRows()){
				if(state & MF_CHECKED  && m_pDoc->m_AprioriGroups.GetSize() > i)
				{
					long j = m_pDoc->m_AprioriGroups[i];
					
					if(j < m_pDoc->m_PatternGroups.GetSize()){
						if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternApriori[j],hdc,x+height/2,height*(2+heightinc),height/20);
						else m_tex.GDIPatterns(m_pDoc->m_PatternApriori[j],hdc,x+height/2,height*(2+heightinc),15*app->m_ordobjectsize);
					}
				}
				else
				{
					grp = m_pDoc->GetRowGroups(i);
					for(long j=0;j<m_pDoc->m_Groups.GetSize();j++){
						if(m_pDoc->m_Groups[j] == grp)
							break;
					}
					if(j < m_pDoc->m_PatternGroups.GetSize()){
						if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternGroups[j],hdc,x+height/2,height*(2+heightinc),height/20);
						else m_tex.GDIPatterns(m_pDoc->m_PatternGroups[j],hdc,x+height/2,height*(2+heightinc),15*app->m_ordobjectsize);
					}
				}
			}else{
				Ellipse(hdc,x+2,height*(2+heightinc)+2,x+height-2,height*(2+heightinc+1)-2);
			}
			if(oldbrush != NULL)
				SelectObject(hdc,oldbrush);
			if(br != NULL)
                DeleteObject(br);

		} // end of selection if statement
		if((heightinc+1)%(nRows) == 0){
			x += size.cx;
			heightinc = -1;
		}
		heightinc++;
	}	

	//draw the scrollbar for the legend
	if(m_saveimage)
		x = m_infowidth + rect.Height() + height*2;
	else
		x = height*2;
	CRect rct(CPoint(x,0),CSize(rect.Width()-x,size.cy));
	

	long w = rct.Width()-height;
	long num = (long)ceil((rect.Width() - x)/(double)size.cx);
	m_legendHeight = height;
	m_legendRect = rct;
	
	m_legendScale = (nCols*nRows) / (double) (m_legendRect.Width() - height);

	long sz =  (long)(nRows / m_legendScale);
	if(sz < height/2)
		sz = height/2;

  	if(rct.Width() > height*2 + sz && !m_saveimage && !m_printimage) 
	{
		Rectangle(hdc,rct.left,rct.top,rct.right,rct.bottom);

		//left triangle (black line)
		if(rct.Width() > height) 
		{
			MoveToEx(hdc,rct.left+1,rct.top+rct.Height()/2,NULL);
			LineTo(hdc,rct.left+height/2,1);
			LineTo(hdc,rct.left+height/2,rct.bottom-1);
			LineTo(hdc,rct.left+1,rct.top+rct.Height()/2);
		}
		if(rct.Width() > height*2) 
		{
			MoveToEx(hdc,rct.right-1,rct.top+rct.Height()/2,NULL);
			LineTo(hdc,rct.right-height/2,1);
			LineTo(hdc,rct.right-height/2,rct.bottom-1);
			LineTo(hdc,rct.right-1,rct.top+rct.Height()/2);
		}

		long pi = m_legendPos / m_legendNumberOfRows;
		if(nCols == 0)
			return;
		long p = (long)(x + height/2 + pi*(m_legendRect.Width() - height)/nCols);

		HGDIOBJ gbrush,obj = SelectObject(hdc, gbrush=GetStockObject(GRAY_BRUSH));
		Rectangle(hdc,p,0,p+sz,size.cy);
		if(obj != NULL)
		{
			SelectObject(hdc,obj);
			DeleteObject(gbrush);
		}

		m_legendX = p;	
	}
}

void C3DXView::OnUseblackwhitesymbols() 
{
	if(m_legend_instance) return;

	if(m_Symbols){
		m_Symbols = false;
		m_menu.CheckMenuItem(ID__USEBLACKWHITESYMBOLS,MF_UNCHECKED);
	}else{
		m_Labels = false;
		m_menu.CheckMenuItem(ID__DISPLAYLABELS,MF_UNCHECKED);
		m_Symbols = true;
		m_menu.CheckMenuItem(ID__USEBLACKWHITESYMBOLS,MF_CHECKED);
		if(m_Usergroupcolours){
			m_Usergroupcolours = false;
			m_menu.CheckMenuItem(ID__USEUSERGROUPCOLOURS,MF_UNCHECKED);
		}
	}	
	RedrawWindow();	
}

void C3DXView::OnUpdateUseblackwhitesymbols(CCmdUI* pCmdUI) 
{

	pCmdUI->Enable(!m_NoSymbols);	
}

void C3DXView::UpdateZOrders()
{
	long i;
	float x,y,z,tx,ty,tz;
	UINT state = m_menu.GetMenuState(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_BYCOMMAND);
	if(state & MF_CHECKED)
	{
		m_groupzvalues.SetSize(m_pDoc->m_aAprioriGroupColours.GetSize());
		m_groupzorder.SetSize(m_pDoc->m_aAprioriGroupColours.GetSize());
		for(i=0;i<m_pDoc->m_aAprioriGroupColours.GetSize();i++){	
			x = m_pDoc->m_aAprioriGroupColours[i].red;
			y = m_pDoc->m_aAprioriGroupColours[i].green;
			z = m_pDoc->m_aAprioriGroupColours[i].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5; y = 0.5;}
			
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,
					(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);
					
			m_groupzvalues.SetAt(i,tz);
			m_groupzorder.SetAt(i,i);
		}
	}
	else
	{
		m_groupzvalues.SetSize(m_pDoc->m_aGroupColours.GetSize());
		m_groupzorder.SetSize(m_pDoc->m_aGroupColours.GetSize());
		for(i=0;i<m_pDoc->m_aGroupColours.GetSize();i++){	
			x = m_pDoc->m_aGroupColours[i].red;
			y = m_pDoc->m_aGroupColours[i].green;
			z = m_pDoc->m_aGroupColours[i].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5; y = 0.5;}
			
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,
					(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);
					
			m_groupzvalues.SetAt(i,tz);
			m_groupzorder.SetAt(i,i);
		}
	}
	SrtShl(&m_groupzvalues,&m_groupzorder);
	m_objectzvalues.SetSize(m_pDoc->m_aObjectColours.GetSize());
	m_objectzorder.SetSize(m_pDoc->m_aObjectColours.GetSize());
	for(i=0;i<m_pDoc->m_aObjectColours.GetSize();i++){
		x = m_pDoc->m_aObjectColours[i].red;
		y = m_pDoc->m_aObjectColours[i].green;
		z = m_pDoc->m_aObjectColours[i].blue;
		if(m_pDoc->m_Ord3d == 2) z = 0.5;
		else if(m_pDoc->m_Ord3d == 1) {z = 0.5; y = 0.5;}

		App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,
					(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);

		m_objectzvalues.SetAt(i,tz);
		m_objectzorder.SetAt(i,i);		 
	} 
	SrtShl(&m_objectzvalues,&m_objectzorder);
}

void C3DXView::OnDisplaylabels() 
{
	if(m_legend_instance) return;

if(m_Labels){
		m_Labels = false;
		m_menu.CheckMenuItem(ID__DISPLAYLABELS,MF_UNCHECKED);
	}else{
		m_Labels = true;
		m_menu.CheckMenuItem(ID__DISPLAYLABELS,MF_CHECKED);
		m_Symbols = false;
		m_menu.CheckMenuItem(ID__USEBLACKWHITESYMBOLS,MF_UNCHECKED);
	}
	
	m_hint = none;
	RedrawWindow();	
}

void C3DXView::OnFont() 
{
	if(m_legend_instance) return;

	LOGFONT lf;
	memcpy(&lf,&(m_pDoc->m_lfCells),sizeof(LOGFONT));
	strcpy(lf.lfFaceName,m_pDoc->m_lfCells.lfFaceName);
	
	CFontDialog dlg(&lf);
	dlg.m_cf.rgbColors = m_pDoc->m_rgbFontCells;

	if ( dlg.DoModal() == IDOK )
	{
		memcpy(&(m_pDoc->m_lfCells),&lf,sizeof(LOGFONT));
		strcpy(m_pDoc->m_lfCells.lfFaceName,lf.lfFaceName);
		m_pDoc->m_rgbFontCells = dlg.m_cf.rgbColors;
		m_pDoc->m_nPointSizeCells = dlg.m_cf.iPointSize;///10; // iPointSize is in 1/10s
		m_pDoc->SetModifiedFlag(TRUE);

		RedrawWindow();
	}	
}

void C3DXView::SaveIt()
{
	if(m_legend_instance) return;

	m_SaveHeight = m_Rect.bottom;
	m_SaveWidth = m_Rect.right;

	CRect rect;
	GetClientRect(rect);

	long selheight,selcolours,selcoloursbits,selcompression;

	CFileDialog* pFileDlg;
	CStdioFile f;
	CFileException fe;
	CString strFileError;
	CString strPathName;
	CString strTitle;

	strTitle = "Save 3D Graph:";
	
	// Get path for the new file
	//pFileDlg = new CFileDialog(FALSE,"bmp","*.bmp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"(height=480 Colour) Bitmap (*.bmp)|*.bmp|(height=960 Colour) Bitmap (*.bmp)|*.bmp|(height=480 BW) Bitmap (*.bmp)|*.bmp|(height=960 BW) Bitmap (*.bmp)|*.bmp|(height=480 Colour) jpeg (*.jpg)|*.jpg|(height=960 Colour) jpeg (*.jpg)|*.jpg|(height=480 BW) jpeg (*.jpg)|*.jpg|(height=960 BW) jpeg (*.jpg)|*.jpg|All files (*.*)|*.*||",NULL);
	pFileDlg = new CFileDialog(FALSE,"bmp","*.bmp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"Bitmap (current height) (*.bmp)|*.bmp|Bitmap (height=480)(*.bmp)|*.bmp|Bitmap (height=960)(*.bmp)|*.bmp|Windows Metafile (*.emf)|*.emf|All files (*.*)|*.*||",NULL);
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	if ( pFileDlg->DoModal() != IDOK ){
		delete pFileDlg;
		return;
	}
	strPathName = pFileDlg->GetPathName();
	
	long fi = pFileDlg->m_ofn.nFilterIndex;
	switch(fi){
	case 2:
		selheight = 480;selcolours = 24;selcoloursbits = 8;selcompression = BI_RGB;
		break;
	case 3:
		selheight = 960;selcolours = 24;selcoloursbits = 8;selcompression = BI_RGB;
		break;
	case 1:
		selheight = rect.Height();selcolours = 24;selcoloursbits = 8;selcompression = BI_RGB;
		break;
	default:
		selheight = 480;selcolours = 24;selcoloursbits = 8;selcompression = BI_RGB;
		break;
	}

	if((selheight % 2) == 1) selheight++;

	delete pFileDlg;

	if(fi == 4){ //metafile
		DrawToMetaFile(true,strPathName);
		return;
	}

	//Create device context
	HDC dc = CreateCompatibleDC(NULL);
	BITMAPINFO info;
	PBYTE bits;

	long nWidth,nObjects;
	if(m_Centroid || m_Usergroupcolours || m_Grouping)
		nObjects = m_pDoc->m_aGroupColours.GetSize();
	else
		nObjects = m_pDoc->m_aObjectColours.GetSize();
	long nRows = (long)floor(selheight/10.0)-1;
	long nCols = (long)ceil(nObjects/(double)nRows);
	if(m_Legend)
		nWidth = 120 + selheight + nCols*150;
	else
		nWidth = 120 + selheight;

	m_saveimage = true;

	m_drawres.x = selheight;
	m_drawres.y = selheight;
	
	double factor = 1;

	while(!RenderBySize((int)(nWidth*factor),(int)(selheight*factor)) && factor > 0.1){
		factor -= 0.1;
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Detail of saved image reduced/ndue to insufficient system memory.");
	}
	
	m_saveimage = false;

	//if(g_pddsBackBuffer == NULL)
	//	return;

	// create and bind bitmap
	memset(&info, 0, sizeof(info));
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = nWidth;
	info.bmiHeader.biHeight = selheight;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = (WORD)selcolours;
	info.bmiHeader.biCompression = selcompression;
	if(selcompression != BI_RGB)
		info.bmiHeader.biSizeImage = 20000;

	HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, (void**)&bits, NULL, 0);
	PIXELFORMATDESCRIPTOR pfd;
	SelectObject(dc, bitmap);

	//set pixel format
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_BITMAP | PFD_DEPTH_DONTCARE;
	if(selcolours == 1) pfd.iPixelType = PFD_TYPE_COLORINDEX ;
	else pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = (BYTE)selcolours;
	//pfd.cRedBits = selcoloursbits;//8;
	//pfd.cGreenBits = selcoloursbits;
	//pfd.cBlueBits = selcoloursbits;
	pfd.cDepthBits = 1;

	int pf = ChoosePixelFormat(dc, &pfd);
	SetPixelFormat(dc, pf, &pfd);

	//create and bind the rendering context
	//HGLRC rc = wglCreateContext(dc);
	//wglMakeCurrent(dc, rc);

	//m_ccfont.FontDestroy(m_Font);
	//delete m_Font;
	//m_Font = m_ccfont.FontCreate(wglGetCurrentDC(), m_Fonttype, 15, 0, m_italic,0);

	//glClearDepth(1.0f);

	//DrawScreen and save to bitmap
	m_Rect.bottom = selheight;
	m_Rect.right = nWidth;

	//m_saveimage = true;
	//DrawScene();
	//m_saveimage = false;

	//glFinish();

	HDC hdc;
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		g_pddsBackBuffer->GetDC(&hdc);
	//}else{
	//	hdc = GetDC()->m_hDC;
	//}
	
	BitBlt(dc,0,0,nWidth,selheight,hdc,0,0,SRCCOPY);

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		g_pddsBackBuffer->ReleaseDC(hdc);
	//}

	SaveDIBitmap(strPathName, &info, bits);

	CreateEverything(GetSafeHwnd());
}

BOOL C3DXView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	((CPatnApp*)AfxGetApp())->SetLandscape(true);

	m_printimage = true;
	
	m_currentpage = 0;
	m_dc = NULL;
	m_bitmap = NULL;

	m_printreduction = false;

	return DoPreparePrinting(pInfo);

}

// for printing (and saving):
//
// 1. Remove action of creating new d3d surfaces and devices.  Do this so that printing is not restricted to the capabilities of the 
//    video hardware.
// 2. To support printing (to the CDC), there are two options:
//    a) Create a global surface for blitting:
//       - create desired surface
//       - render to this surface using existing "app_render()", but supplying the new surface
//       - while rendering, use "app_corner()" to compute relative object positions 
//         (note: z-positions have already been calculated)
//    b) Same as (a), but redraw to the current CDC for each draw.
void C3DXView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	if(m_legend_instance) return;

	CPatnApp * app = (CPatnApp*)AfxGetApp();
	if( app == NULL)
		return;

	HDC hdc;
	static double factor = 1;
	CRect client;
	this->GetClientRect(client);	

	if(m_currentpage != (signed int) pInfo->m_nCurPage){
		//calculate how much larger text must be
		//FILE * fp = fopen("3DXView_info.txt","a+");

		double font_scale = (double)(m_drawres.y /
			(double)((((client.Width()) < client.Height())?(client.Width()):client.Height())));
		long lastinfowidth = m_infowidth;
		long textfontsize = m_stressbox.font.lfHeight;
		long fontsize = m_pDoc->m_lfCells.lfHeight;
		double last_ordobjectsize = app->m_ordobjectsize;
		CArray<long> textboxfontsize;
		textboxfontsize.SetSize(m_TextBoxes.GetSize());
		for(int p=0;p<m_TextBoxes.GetSize();p++)
		{
			textboxfontsize[p] = m_TextBoxes[p]->font.lfHeight;
		}
		CArray<CRect> textboxpos;
		textboxpos.SetSize(m_TextBoxes.GetSize()+1);
		textboxpos[m_TextBoxes.GetSize()] = m_stressbox.location;
		for(p=0;p<m_TextBoxes.GetSize();p++)
		{
			textboxpos[p] = m_TextBoxes[p]->location;
		}

		//fprintf(fp,"scale=%f\nlastinfowidth=%d\ntextfontscale=%d\nfontscale=%d\n",font_scale,lastinfowidth,textfontsize,fontsize);
		
		//fprintf(fp,"m_rectDraw.width=%d, m_rectDraw.height=%d\n",m_rectDraw.Width(),m_rectDraw.Height());

		//create page
		m_currentpage = pInfo->m_nCurPage;
		
		if(pInfo->m_nCurPage == 1){
			m_drawres.x = m_drawres.y = (long)(((m_rectDraw.Width() < m_rectDraw.Height())?m_rectDraw.Width()*factor:m_rectDraw.Height())*factor);
			//fprintf(fp,"**start\n**screen width=%d, height=%d\n",m_rectDraw.Width(),m_rectDraw.Height());
			//fprintf(fp,"**client client.width=%d, client.height=%d\n",client.Width(),client.Height());
			//fprintf(fp,"**scaling x=%d, y=%d, factor=%f\n",m_drawres.x,m_drawres.y,factor);

			//repeatedly reduce the size of image until it is able to be created by the device or it 
			// fails completely.  (reduce by 10% of print device size per iteration, or until it is 10% of original value);
			//Cleanup3DEnvironment();
			m_printimage = true;

			font_scale = (double)(m_drawres.y /
				(double)((((client.Width()) < client.Height())?(client.Width()):client.Height())));
			if(true){
				//m_infowidth = (long) (m_infowidth * font_scale);
				for(int i=0;i<m_TextBoxes.GetSize() && m_TextBoxes[i] != NULL;i++){
					m_TextBoxes[i]->font.lfHeight = (long) (font_scale * m_TextBoxes[i]->font.lfHeight);					
					m_TextBoxes[i]->location.top = (int) (m_TextBoxes[i]->location.top * font_scale);
					m_TextBoxes[i]->location.left =(int)(m_TextBoxes[i]->location.left * font_scale);
				}
				//m_stressbox.location.top *= font_scale;
				//m_stressbox.location.left *= font_scale;
				m_stressbox.font.lfHeight = (long) (m_stressbox.font.lfHeight * font_scale);				
				m_pDoc->m_lfCells.lfHeight = (long) (m_pDoc->m_lfCells.lfHeight* font_scale);
//				app->m_ordobjectsize = (app->m_ordobjectsize * font_scale);
//				m_pDoc->m_3dfont.lfHeight *= 3;
			}

			if(factor == 1)
			{
				while(!RenderBySize(m_drawres.x,m_drawres.y-m_infowidth) && factor > .1){
					factor -= 0.05;
					m_drawres.x = m_drawres.y = (long)(((m_rectDraw.Width() < m_rectDraw.Height())?m_rectDraw.Width()*factor:m_rectDraw.Height())*factor);
				//	fprintf(fp,"**scaling x=%d, y=%d, factor=%f\n",m_drawres.x,m_drawres.y,factor);
					font_scale = (double)(m_drawres.y /
						(double)((((client.Width()) < client.Height())?(client.Width()):client.Height())));
					if(true){
						//m_infowidth = (long) (lastinfowidth * font_scale);
						for(int i=0;i<m_TextBoxes.GetSize() && m_TextBoxes[i] != NULL;i++){
							m_TextBoxes[i]->font.lfHeight = (long) (font_scale * textboxfontsize[i]);					
							m_TextBoxes[i]->location.top = (int)(textboxpos[i].top*font_scale);
							m_TextBoxes[i]->location.left = (int)(textboxpos[i].left*font_scale);
						}
					//	m_stressbox.location.top = textboxpos[m_TextBoxes.GetSize()].top*font_scale;
					//	m_stressbox.location.left = textboxpos[m_TextBoxes.GetSize()].left*font_scale;
						m_stressbox.font.lfHeight = (long) (textfontsize * font_scale);				
						m_pDoc->m_lfCells.lfHeight = (long) (fontsize* font_scale);
	//					app->m_ordobjectsize = (last_ordobjectsize*font_scale);
		//				m_pDoc->m_3dfont.lfHeight *= 3;
					}
				}
			}
		//	fprintf(fp,"**3d graph page 1\nscale=%f\nlastinfowidth=%d\ntextfontscale=%d\nfontscale=%d\n",font_scale,lastinfowidth,m_stressbox.font.lfHeight ,m_pDoc->m_lfCells.lfHeight);
			//Cleanup3DEnvironment();
			RenderBySize(m_drawres.x,m_drawres.y);

			
			//m_drawres.x = m_drawres.y = 800;
			
			//m_drawres.x = m_drawres.y = (long)(((m_rectDraw.Width() < m_rectDraw.Height())?m_rectDraw.Width()*factor:m_rectDraw.Height())*factor);
			//fprintf(fp,"**screen\nwidth=%d, height=%d\n",m_rectDraw.Width(),m_rectDraw.Height());
			//fprintf(fp,"**scaling\nx=%d, y=%d, factor=%f\n",m_drawres.x,m_drawres.y,factor);

			////repeatedly reduce the size of image until it is able to be created by the device or it 
			//// fails completely.  (reduce by 10% of print device size per iteration, or until it is 10% of original value);
			//while(!RenderBySize(m_drawres.x,m_drawres.y-m_infowidth) && factor > .1){
			//	factor -= 0.05;
			//	m_drawres.x = m_drawres.y = (long)(((m_rectDraw.Width() < m_rectDraw.Height())?m_rectDraw.Width()*factor:m_rectDraw.Height())*factor);
			//	fprintf(fp,"**scaling\nx=%d, y=%d, factor=%f\n",m_drawres.x,m_drawres.y,factor);
			//}
			if(factor <= .1){
				//handle partial success
				Cleanup3DEnvironment();
			}				
			if(factor < 1){
				m_printreduction = true;
			//	if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Detail of printed image reduced/ndue to insufficient system memory.");
			}
			m_printimage = false;	
		//	fprintf(fp,"**end\n\n");
		//	fclose(fp);
			
		}else{
			m_printimage = true;
			
			m_drawres.x = (long)(m_rectDraw.Width()*factor);
			m_drawres.y = (long)(m_rectDraw.Height()*factor);

			//repeatedly reduce the size of image until it is able to be created by the device or it 
			// fails completely.  (reduce by 10% of print device size per iteration, or until it is 10% of original value);
			double font_scale = 1.2;//(double)(m_drawres.y/(double)(((client.Width() < client.Height())?client.Width():client.Height()))/4.0);
			m_pDoc->m_lfCells.lfHeight = (long)(m_pDoc->m_lfCells.lfHeight *font_scale);
			//Cleanup3DEnvironment();
			while(!RenderLegend(m_drawres.x,m_drawres.y) && factor > .1){
				factor -= 0.05;
				m_drawres.x = (long)(m_rectDraw.Width()*factor);
				m_drawres.y = (long)(m_rectDraw.Height()*factor);
			}
			//font_scale = (double)(m_drawres.y/(double)(((client.Width() < client.Height())?client.Width():client.Height()))/4.0);
			if(true){
			//	m_infowidth = (long) (m_infowidth * font_scale);
				for(int i=0;i<m_TextBoxes.GetSize() && m_TextBoxes[i] != NULL;i++){
					m_TextBoxes[i]->font.lfHeight = (long) (font_scale * m_TextBoxes[i]->font.lfHeight);					
				}
				m_stressbox.font.lfHeight = (long) (m_stressbox.font.lfHeight * font_scale);				
				m_pDoc->m_lfCells.lfHeight = (long) (m_pDoc->m_lfCells.lfHeight* font_scale);
				app->m_ordobjectsize *= font_scale;
			}
			//Cleanup3DEnvironment();
			RenderLegend(m_drawres.x,m_drawres.y);

			if(factor <= .1){
				//handle partial success
				Cleanup3DEnvironment();
			}				
			if(factor < 1){
				m_printreduction = true;
				//if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Detail of printed image reduced/ndue to insufficient system memory.");
			}
			m_printimage = false;
			//m_pDoc->m_lfCells.lfHeight /=font_scale;
			m_pDoc->m_lfCells.lfHeight = fontsize;
		}
		if(true){
			//m_infowidth /= font_scale;
			m_infowidth = lastinfowidth;
			for(int i=0;i<m_TextBoxes.GetSize();i++){
				//m_TextBoxes[i]->font.lfHeight /= font_scale;
				m_TextBoxes[i]->font.lfHeight = textboxfontsize[i];
			}
			//m_stressbox.font.lfHeight /= font_scale;
			m_stressbox.font.lfHeight = textfontsize;
			//m_pDoc->m_lfCells.lfHeight /=font_scale;
			m_pDoc->m_lfCells.lfHeight = fontsize;				
			//m_pDoc->m_3dfont.lfHeight /= 3;
			app->m_ordobjectsize = last_ordobjectsize;

			m_stressbox.location = textboxpos[m_TextBoxes.GetSize()];
			for(p=0;p<m_TextBoxes.GetSize();p++)
			{
				m_TextBoxes[p]->location = textboxpos[p];
			}
		}
	}		

	if(g_pddsBackBuffer != NULL){
		g_pddsBackBuffer->GetDC(&hdc);

		double diff = (m_drawres.x-m_infowidth)/((double)m_drawres.x);
		if(pInfo->m_nCurPage == 1){
			StretchBlt(pDC->GetSafeHdc(),m_rectDraw.left,m_rectDraw.top,
				m_rectDraw.Width(),(int)(m_rectDraw.Width()*(diff)),
				hdc,0,0,(int)((m_drawres.x)*1.2),(int)(1.2*(m_drawres.y-m_infowidth)),SRCCOPY);
		}else{
			StretchBlt(pDC->GetSafeHdc(),m_rectDraw.left,m_rectDraw.top,
				m_rectDraw.Width(),(int)(m_rectDraw.Height()),
				hdc,0,0,m_drawres.x,m_drawres.y,SRCCOPY);
		}

		g_pddsBackBuffer->ReleaseDC(hdc);
	}
	/*m_printimage = false;
	DrawToMetaFile(false,"");

	if(g_emf == NULL)
		return;

	CRect rect;
	rect = m_rectDraw;

	ENHMETAHEADER EnhMetaHdr;
	GetEnhMetaFileHeader(g_emf, sizeof(EnhMetaHdr), &EnhMetaHdr); 

	//drawratio = m_drawres.x/(double)m_drawres.y;
	drawratio = EnhMetaHdr.rclBounds.right / (double)EnhMetaHdr.rclBounds.bottom; //preserve meta aspect
	if(rect.Width()/(double)rect.Height() < drawratio){
		rect.bottom = (long)(rect.top + rect.Width()/drawratio);
	}else{
		rect.right = (long)(rect.left + rect.Height()*drawratio);
	}

	PlayEnhMetaFile(pDC->GetSafeHdc(),g_emf,rect);
	DeleteEnhMetaFile(g_emf);	*/

	//while(!RenderBySize((int)(nWidth*factor),(int)(selheight*factor)) && factor > 0.1){
	//	factor -= 0.1;
	//	if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Detail of saved image reduced/ndue to insufficient system memory.");
	//}
	
}

void C3DXView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	if(m_legend_instance) return;

//	delete m_psd;
//	m_psd = NULL;

	m_printimage = false;

//	if(m_dc != NULL) DeleteDC(m_dc);
//	if(m_bitmap != NULL) DeleteObject(m_bitmap);

	CView::OnEndPrinting(pDC, pInfo);

	CreateEverything( GetSafeHwnd());

	((CPatnApp*)AfxGetApp())->SetLandscape(false);
	RedrawWindow();

}

void C3DXView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
if(m_legend_instance) return;
	

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
//	
//	m_Hratio = point.x/((double)m_HRes);
//	m_Vratio = point.y/((double)m_VRes);
//
//	//USED 300 'CAUSE THAT'S WHAT IT WAS WRITTEN ON
//	m_rectDraw.left = (long)((rect.left)/((double) point.x)*m_HRes);
//	m_rectDraw.right = (long)((point.x - rect.right)/((double) point.x)*m_HRes);
//	m_rectDraw.top = (long)((rect.top)/((double) point.y)*m_VRes);
//	m_rectDraw.bottom = (long)((point.y - rect.bottom)/((double) point.y)*m_VRes);

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

void C3DXView::DrawInfoBar(HDC hdc)
{

	SetBkMode(hdc,TRANSPARENT);

	long height = (long)fabs((double)m_pDoc->m_lfCells.lfHeight);

	CString text;

	// Bracket begin a path 
 
	if (m_infobar.item == object && m_currentobject != -1) {
		// print information on object one
		text = "1st OBJECT:";
		TextOut(hdc,5,height*1,text,text.GetLength());
		
		text = "Name:";
		TextOut(hdc,5,height*2,text,text.GetLength());
		SetTextColor(hdc,0x000000ff);
		TextOut(hdc,5,height*3,m_infobar.name1,m_infobar.name1.GetLength());
SetTextColor(hdc,m_pDoc->m_rgbFontCells);
		
		if (m_infobar.itemNr == 2) {
			//print information on object two
			text = "2nd OBJECT:";
			TextOut(hdc,5,(long)(height*8.5),text,text.GetLength());

			text = "Name:";
			TextOut(hdc,5,(long)(height*9.5),text,text.GetLength());
			SetTextColor(hdc,0x000000ff);
			TextOut(hdc,5,(long)(height*10.5),m_infobar.name2,m_infobar.name2.GetLength());
SetTextColor(hdc,m_pDoc->m_rgbFontCells);

			// print association measure
			text = "ASSOCIATION:";
			TextOut(hdc,5,height*16,text,text.GetLength());
			SetTextColor(hdc,0x000000ff);
			text.Format("%.5f", m_infobar.assoc);
			TextOut(hdc,5,height*17,text,text.GetLength());		
SetTextColor(hdc,m_pDoc->m_rgbFontCells);

		}
	} else if(m_currentcentroid != -1){
		// print information on group one
		text = "1st GROUP:";
		TextOut(hdc,5,height*1,text,text.GetLength());
		
		text = "Group#:";
		TextOut(hdc,10,height*2,text,text.GetLength());
		SetTextColor(hdc,0x000000ff);
		TextOut(hdc,10,height*3,m_infobar.name1,m_infobar.name1.GetLength());
SetTextColor(hdc,m_pDoc->m_rgbFontCells);
		
		if(m_infobar.itemNr == 2) {
			//print information on object two
			text = "2nd GROUP:";
			TextOut(hdc,5,(long)(height*7.5),text,text.GetLength());
			
			text = "Group#:";
			TextOut(hdc,10,(long)(height*8.5),text,text.GetLength());
			SetTextColor(hdc,0x000000ff);
			TextOut(hdc,10,(long)(height*9.5),m_infobar.name2,m_infobar.name2.GetLength());
SetTextColor(hdc,m_pDoc->m_rgbFontCells);
			
			//Kruskal-Wallis column titles			
			text = "Kruskal-Wallis";
			TextOut(hdc,5,(long)(height*14),text,text.GetLength());
			
			text = "Top 5";
			TextOut(hdc,5,(long)(height*15),text,text.GetLength());
			
			SetTextColor(hdc,0x000000ff);
			long j = m_KWOrder.GetSize();
			for(long i=0;i<5 && i<j && 
				j-1-i >= 0 && j-1-i < m_KWOrder.GetSize() &&
				j-1-i < m_KWValues.GetSize() && 
				m_KWOrder[j-1-i] < m_pDoc->m_KWAttributes.GetSize() &&
				m_pDoc->m_KWAttributes[m_KWOrder[j-1-i]] < m_pDoc->GetNoOfColumns();i++){
				m_pDoc->GetColumnLabel(m_pDoc->m_KWAttributes[m_KWOrder[j-1-i]],text);
				if(m_KWValues[j-1-i] == m_pDoc->GetMissingValue())
					text.Format("identical  %s",text);
				else
					text.Format("%6.4f %s",m_KWValues[j-1-i],text);	
				TextOut(hdc,10,(long)(height*(16.5 + i)),text,text.GetLength());
			}
SetTextColor(hdc,m_pDoc->m_rgbFontCells);
			
		}else{ //single group selected
			//Kruskal-Wallice column titles
			text = "Kruskal-Wallis";
			TextOut(hdc,5,height*14,text,text.GetLength());
			
			text = "Top 5";
			TextOut(hdc,5,height*15,text,text.GetLength());
			
			SetTextColor(hdc,0x000000ff);
			long j = m_pDoc->m_KW.GetSize();
			for(long i=0;i<5 && i < j &&
				j-1-i >=0 && j-1-i < m_pDoc->m_KW.GetSize() &&
				j-1-i < m_pDoc->m_KWValues.GetSize() &&
				m_pDoc->m_KW[j-1-i] < m_pDoc->m_KWAttributes.GetSize() &&
				m_pDoc->m_KWAttributes[m_pDoc->m_KW[j-1-i]] < m_pDoc->GetNoOfColumns();i++){
				m_pDoc->GetColumnLabel(m_pDoc->m_KWAttributes[m_pDoc->m_KW[j-1-i]],text);
				if(m_pDoc->m_KWValues[j-1-i] == m_pDoc->GetMissingValue())
					text.Format("identical  %s",text);
				else
					text.Format("%6.4f %s",m_pDoc->m_KWValues[j-1-i],text);	
				TextOut(hdc,5,(long)(height*(16.5+i)),text,text.GetLength());
			}			
SetTextColor(hdc,m_pDoc->m_rgbFontCells);
		}

		
	}

	// print mst
	if (m_Mst && !m_Centroid) {
		// print association measure
		text = "MST DISTANCE:";
		TextOut(hdc,5,height*19,text,text.GetLength());
		SetTextColor(hdc,0x000000ff);
		text.Format("%.5f", m_infobar.mst_length);
		TextOut(hdc,5,height*20,text,text.GetLength());		
SetTextColor(hdc,m_pDoc->m_rgbFontCells);
	}

	if(this->m_Grouping || this->m_Centroid)
	{
		UINT state = m_menu.GetMenuState(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_BYCOMMAND);
		if(state & MF_CHECKED )//&& m_pDoc->m_AprioriGroups.GetSize() && m_pDoc->m_AprioriGroups[0] > 0)
		{
			text = "A-priori groups";
			TextOut(hdc,5,(long)(height*(16.5+8)),text,text.GetLength());
		}
		else if(!(state & MF_CHECKED))
		{
			text = "PATN groups";
			TextOut(hdc,5,(long)(height*(16.5+8)),text,text.GetLength());
		}
		
	}
}

void C3DXView::DrawTextBoxes(HDC hdc)
{
	//CFont font,*oldfont;
	HFONT font,oldfont;
	//HPEN pen,oldpen;
	long i;

	m_screenrect.right = m_screenrect.bottom + m_infowidth;

	//draw text boxes
	for(i=0;i<m_TextBoxes.GetSize() && m_TextBoxes[i] != NULL;i++){
		m_TextBoxes[i]->font.lfQuality |= ANTIALIASED_QUALITY;
		font = CreateFontIndirect(&m_TextBoxes[i]->font);
		oldfont = (HFONT) SelectObject(hdc,font);
		//pen = CreatePen(PS_SOLID,0,m_TextBoxes[i]->colour);
		//oldpen = (HPEN) SelectObject(hdc,pen);

		SetTextColor(hdc,m_TextBoxes[i]->colour);
		if(m_printimage || m_saveimage){
			m_screenrect.right = m_screenrect.bottom + m_infowidth;
			if(m_TextBoxes[i]->location.left >= m_infowidth){
				TextOut(hdc,m_infowidth+(long)((m_TextBoxes[i]->location.left-m_infowidth)*(m_drawres.x-m_infowidth)/((double)(m_screenrect.right-m_infowidth))),
					(long)(m_TextBoxes[i]->location.top*(g_rcScreenRect.bottom)/((double)m_screenrect.bottom)),
					m_TextBoxes[i]->contents,m_TextBoxes[i]->contents.GetLength());
			}else{
				TextOut(hdc,m_TextBoxes[i]->location.left,
					m_TextBoxes[i]->location.top,
					m_TextBoxes[i]->contents,m_TextBoxes[i]->contents.GetLength());
			}
		}else{
			TextOut(hdc,(int)(m_TextBoxes[i]->location.left),
				(int)(m_TextBoxes[i]->location.top),
				m_TextBoxes[i]->contents,m_TextBoxes[i]->contents.GetLength());

			SIZE sz;
			GetTextExtentPoint32(hdc,m_TextBoxes[i]->contents,m_TextBoxes[i]->contents.GetLength(),&sz);
			m_TextBoxes[i]->location.right = m_TextBoxes[i]->location.left + sz.cx;
			m_TextBoxes[i]->location.bottom = m_TextBoxes[i]->location.top + sz.cy;
		}

		if(oldfont != NULL)
			SelectObject(hdc,oldfont);

		DeleteObject(font);
	}

	m_screenrect.right = m_screenrect.bottom + m_infowidth;

	//draw stress box
	m_stressbox.font.lfQuality |= ANTIALIASED_QUALITY;
	font = CreateFontIndirect(&m_stressbox.font);
	oldfont = (HFONT) SelectObject(hdc,font);

	SetTextAlign(hdc,TA_TOP);

	if(m_pDoc->m_OrdStress != m_pDoc->GetMissingValue())
	{
		SetTextColor(hdc,m_stressbox.colour);
		if(m_printimage || m_saveimage){
			m_screenrect.right = m_screenrect.bottom+ m_infowidth;
			if(m_stressbox.location.left >= m_infowidth){
				TextOut(hdc,m_infowidth+(int)((m_stressbox.location.left-m_infowidth)*(m_drawres.x-m_infowidth)/((double)(m_screenrect.right-m_infowidth))),
					(int)(m_stressbox.location.top*(g_rcScreenRect.bottom)/((double)m_screenrect.bottom)),
					m_stressbox.contents,m_stressbox.contents.GetLength());
			}else{
				TextOut(hdc,m_stressbox.location.left,m_stressbox.location.top,
					m_stressbox.contents,m_stressbox.contents.GetLength());
			}
		}else{
			TextOut(hdc,(int)(m_stressbox.location.left),
				(int)(m_stressbox.location.top),
				m_stressbox.contents,m_stressbox.contents.GetLength());
		}
	}
	if(oldfont != NULL)
		SelectObject(hdc,oldfont);

	DeleteObject(font);

	SetTextColor(hdc,m_pDoc->m_rgbFontCells);
}

void C3DXView::DrawObjects(HDC hdc)
{
	POSITION pos;
	CView * pView = NULL;
	pos = m_pDoc->GetFirstViewPosition();
	while(pos){
		pView = m_pDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView)))
			break;
		else
			pView = NULL;
	}
	double x,y,z;
	float tx,ty,tz;
	CString text;
	double red,green,blue;
	int i,idx,grp,j;
	unsigned long clr;

	HFONT oldfont,font;

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		m_gdi_offsetx = 0;
	//}	

	double xmov = 0;//.025*cos(wAngleY*3.14/180.0);
	double zmov = -8;//.025*sin(wAngleY*3.14/180.0);
	
	int err;

	if(!m_Symbols && !m_Labels){
		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		//	err = g_pddsBackBuffer->ReleaseDC(hdc);
		//}
		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
			err = g_pd3dDevice->BeginScene();
		//}
	}else{

		//g_pddsBackBuffer->GetDC(&hdc);
		SetBkMode(hdc,TRANSPARENT);
		
		BYTE quality = m_pDoc->m_lfCells.lfQuality;
		m_pDoc->m_lfCells.lfQuality |= ANTIALIASED_QUALITY;
		font = CreateFontIndirect(&m_pDoc->m_lfCells);
		m_pDoc->m_lfCells.lfQuality = quality;
		oldfont = (HFONT) SelectObject(hdc,&font);			
	}
	
	CPatnApp * app = (CPatnApp*)AfxGetApp();
	
	UINT state = m_menu.GetMenuState(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_BYCOMMAND);

	if(m_Centroid) { //m_groupzorder
		for(idx=0;idx<m_groupzorder.GetSize();idx++){	
			if(m_groupzorder.GetSize()-1-idx >= 0){
				i = m_groupzorder[m_groupzorder.GetSize()-1-idx];
				if(state & MF_CHECKED)
				{
					if(m_Usergroupcolours && i<m_pDoc->m_aGroupColours2.GetSize())
					{
						red = m_pDoc->m_aGroupColours2[i].red;
						green = m_pDoc->m_aGroupColours2[i].green;
						blue = m_pDoc->m_aGroupColours2[i].blue;
					}else if(i<m_pDoc->m_aAprioriGroupColours.GetSize())
					{
						red = m_pDoc->m_aAprioriGroupColours[i].red;
						green = m_pDoc->m_aAprioriGroupColours[i].green;
						blue = m_pDoc->m_aAprioriGroupColours[i].blue;
					}
					if(m_pDoc->m_Ord3d == 2) blue = .5f;
					else if(m_pDoc->m_Ord3d == 1) {blue = .5f; green = .5f;}
				}else if(!m_Usergroupcolours){
					if(i<m_pDoc->m_aGroupColours.GetSize())
					{
					red = m_pDoc->m_aGroupColours[i].red;
					green = m_pDoc->m_aGroupColours[i].green;
					blue = m_pDoc->m_aGroupColours[i].blue;
					}
					if(m_pDoc->m_Ord3d == 2) blue = .5f;
					else if(m_pDoc->m_Ord3d == 1) {blue = .5f; green = .5f;}
				}else{
					if(i<m_pDoc->m_UserGroupColours.GetSize())
					{
					red = m_pDoc->m_UserGroupColours[i].red;
					green = m_pDoc->m_UserGroupColours[i].green;
					blue = m_pDoc->m_UserGroupColours[i].blue;
					}
				}

				if(state & MF_CHECKED)
				{
					x = m_pDoc->m_aAprioriGroupColours[i].red;
					y = m_pDoc->m_aAprioriGroupColours[i].green;
					z = m_pDoc->m_aAprioriGroupColours[i].blue;
				}
				else
				{
					x = m_pDoc->m_aGroupColours[i].red;
					y = m_pDoc->m_aGroupColours[i].green;
					z = m_pDoc->m_aGroupColours[i].blue;
				}
				if(m_pDoc->m_Ord3d == 2) z = 0.5;
				else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y = 0.5;}
				App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,
						(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);
						
				if(m_Symbols){
					if(state & MF_CHECKED)
					{
						if(i < m_pDoc->m_PatternGroups.GetSize()){
							if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternApriori[i],hdc,(long)tx-5+m_gdi_offsetx+m_infowidth,(long)ty-5,45*app->m_ordobjectsize);
							else m_tex.GDIPatterns(m_pDoc->m_PatternApriori[i],hdc,(long)tx-5+m_gdi_offsetx+m_infowidth,(long)ty-5,15*app->m_ordobjectsize);
						}
					}
					else
					{
						if(i < m_pDoc->m_PatternGroups.GetSize()){
							if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternGroups[i],hdc,(long)tx-5+m_gdi_offsetx+m_infowidth,(long)ty-5,45*app->m_ordobjectsize);
							else m_tex.GDIPatterns(m_pDoc->m_PatternGroups[i],hdc,(long)tx-5+m_gdi_offsetx+m_infowidth,(long)ty-5,15*app->m_ordobjectsize);
						}
					}
				}else if(m_Labels){
					//code to draw centroid labels, see colour retrieval for coords					
					if(state & MF_CHECKED)
					{
						if( i < m_pDoc->m_AprioriGroups.GetSize())
							text.Format("%d",i+1);//m_pDoc->m_Groups.GetAt(i));
					}
					else
					{
						if( i < m_pDoc->m_Groups.GetSize())
							text.Format("%d",m_pDoc->m_Groups.GetAt(i));
					}

					red = (int)(red*255);
					green = (int)(green*255);
					blue = (int)(blue*255);

					clr = (unsigned long)(red + 256*(green+256*blue));
					SetTextColor(hdc,clr);

					//TextOut(hdc,(int)tx+m_gdi_offsetx+m_infowidth,(int)ty+(int)zmov,text,text.GetLength());
					TextOut(hdc,(long)tx+m_gdi_offsetx+m_infowidth,(long)ty+(long)zmov,text,text.GetLength());
					
				}else{
					DrawSphere2(m_gdi_offsetx+(x-0.5)*x_scale+0.5,(y-0.5)*y_scale+0.5,0.5+(z-0.5)*z_scale,red,green,blue,i == m_selectedgroup || i == m_currentcentroid || i == m_comparecentroid
						, false );
				}
				
			}
		}
	} else { //m_objectzorder
		if (m_Mst) { 
			if(!m_Symbols && !m_Labels)
				DrawMst(); 
			else
				DrawMst(hdc);
		}
		for(idx=0;idx<m_objectzorder.GetSize() && m_objectzorder.GetSize()-1-idx >= 0;idx++){
			i = m_objectzorder[m_objectzorder.GetSize()-1-idx];
			//if statement for selected objects only display
			if(i<m_pDoc->GetNoOfRows() && i < m_pDoc->m_aObjectColours.GetSize()){
				if(!m_SelOnly || (pView != NULL && 
					((CTableView*)pView)->m_pSelectedRows->IsSelected(i))){
					if(m_Grouping){
						if(m_pDoc->GetRowGroups(i) > 0 &&
							m_pDoc->GetRowGroups(i)-1 < m_pDoc->m_aGroupColours.GetSize()){
							if(state & MF_CHECKED && m_pDoc->m_AprioriGroups.GetSize() > i && m_pDoc->m_AprioriGroups[i]-1 < m_pDoc->m_aAprioriGroupColours.GetSize() && m_pDoc->m_AprioriGroups[i] > 0)
							{
								red = m_pDoc->m_aAprioriGroupColours[m_pDoc->m_AprioriGroups[i]-1].red;
								green = m_pDoc->m_aAprioriGroupColours[m_pDoc->m_AprioriGroups[i]-1].green;
								blue = m_pDoc->m_aAprioriGroupColours[m_pDoc->m_AprioriGroups[i]-1].blue;
							}
							else
							{
								red = m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].red;
								green = m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].green;
								blue = m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].blue;
							}
						}
						if(m_pDoc->m_Ord3d == 2) blue = .5f;
						else if(m_pDoc->m_Ord3d == 1) {blue = 0.5f; green = 0.5f;}
					}else if(m_Usergroupcolours){
						if(state & MF_CHECKED && i<m_pDoc->m_AprioriGroups.GetSize() &&
							m_pDoc->m_AprioriGroups.GetAt(i) > 1 && m_pDoc->m_AprioriGroups.GetAt(i)-2 < m_pDoc->m_aGroupColours2.GetSize()){
							
							red = m_pDoc->m_aGroupColours2[m_pDoc->m_AprioriGroups.GetAt(i)-2].red;
							green = m_pDoc->m_aGroupColours2[m_pDoc->m_AprioriGroups.GetAt(i)-2].green;
							blue = m_pDoc->m_aGroupColours2[m_pDoc->m_AprioriGroups.GetAt(i)-2].blue;
						}else if(m_pDoc->GetRowGroups(i) > 0 &&
							m_pDoc->GetRowGroups(i)-1 < m_pDoc->m_UserGroupColours.GetSize()){
							red = m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].red;
							green = m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].green;
							blue = m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].blue;
						}
					}else if(i < m_pDoc->m_aObjectColours.GetSize()){
						if(m_colColourMap >= 0) {
							red = ((m_colColours[i] & 0xff0000) >> 16)/255.0f;
							green = ((m_colColours[i] & 0x00ff00) >> 8)/255.0f;
							blue = (m_colColours[i] & 0x0000ff)/255.0f;
						} else  {
							red = m_pDoc->m_aObjectColours[i].red;
							green = m_pDoc->m_aObjectColours[i].green;
							blue = m_pDoc->m_aObjectColours[i].blue;					
							if(m_pDoc->m_Ord3d == 2) blue = .5f;
							else if(m_pDoc->m_Ord3d == 1) {blue = 0.5f; green = 0.5f;}
						}
					}
				
					x = m_pDoc->m_aObjectColours[i].red;
					y = m_pDoc->m_aObjectColours[i].green;
					z = m_pDoc->m_aObjectColours[i].blue;
					if(m_pDoc->m_Ord3d == 2) z = 0.5;
					else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}

					App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,
						(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);

					if(m_Symbols && !m_Grouping){
						if(i < m_pDoc->m_PatternObjects.GetSize()){
							if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternObjects[i],hdc,(long)tx-5+m_infowidth,(long)ty-5,45*app->m_ordobjectsize);
							else m_tex.GDIPatterns(m_pDoc->m_PatternObjects[i],hdc,(long)tx-5+m_gdi_offsetx+m_infowidth,(long)ty-5,15*app->m_ordobjectsize);
						}
					}else if(m_Symbols && m_Grouping){
						if(state & MF_CHECKED && m_pDoc->m_AprioriGroups.GetSize() > i)
						{
							j = m_pDoc->m_AprioriGroups[i]-1;
											
							if(j >= 0 && j < m_pDoc->m_PatternApriori.GetSize()){
								if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternApriori[j],hdc,(long)tx-5+m_infowidth,(long)ty-5,45*app->m_ordobjectsize);
								else m_tex.GDIPatterns(m_pDoc->m_PatternApriori[j],hdc,(long)tx-5+m_gdi_offsetx+m_infowidth,(long)ty-5,15*app->m_ordobjectsize);
							}
						}
						else
						{
							grp = m_pDoc->GetRowGroups(i);
							for(j=0;j<m_pDoc->m_Groups.GetSize();j++){
								if(m_pDoc->m_Groups[j] == grp)
									break;
							}					
							if(j >= 0 && j < m_pDoc->m_PatternGroups.GetSize()){
								if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternGroups[j],hdc,(long)tx-5+m_infowidth,(long)ty-5,45*app->m_ordobjectsize);
								else m_tex.GDIPatterns(m_pDoc->m_PatternGroups[j],hdc,(long)tx-5+m_gdi_offsetx+m_infowidth,(long)ty-5,15*app->m_ordobjectsize);
							}
						}
					}else if(m_Labels){
						red = (int)(red*255);
						green = (int)(green*255);
						blue = (int)(blue*255);

						clr = (unsigned long)(red + 256*(green+256*blue));
						SetTextColor(hdc,clr);

						m_pDoc->GetRowLabel(i,text);
						TextOut(hdc,(long)tx+m_gdi_offsetx+m_infowidth,(long)ty+(long)zmov,text,text.GetLength());
					}else{
						DrawSphere2(m_gdi_offsetx+(x-0.5)*x_scale+0.5,(y-0.5)*y_scale+0.5,(z-0.5)*z_scale+0.5,red,green,blue, i == m_selectedobject || i == m_currentobject || i == m_compareobject
							, (m_pDoc->m_aRowGroups[i] == m_selectedgroup));

						
					}
				} // end of selection if statement
			} // end of for-loop			
		}
	}  // end of if m_Centroid else statement

	if(!m_Symbols && !m_Labels){		
		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
			err = g_pd3dDevice->EndScene();
		//}
		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		////	err = g_pddsBackBuffer->GetDC(&hdc);
		//}
	}else{
		if(oldfont != NULL)
			SelectObject(hdc,oldfont);
		DeleteObject(font);
		
		//err = g_pddsBackBuffer->ReleaseDC(hdc);

	}
}

void C3DXView::DrawCompare(HDC hdc)
{
	float x,y,z;
	float tx,ty,tz;
	CString text;
	long width;

	UINT state = m_menu.GetMenuState(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_BYCOMMAND);

	width = m_infowidth;

	if((m_currentobject >= 0 && m_currentobject < m_pDoc->m_aObjectColours.GetSize() &&
		m_currentobject < m_pDoc->GetNoOfRows()) ||
		m_hint == objectselection || m_hint == objectcompare){

		//highlight single object at 
		m_pDoc->GetRowLabel(m_currentobject,text);
		double xmov = 8;//.025*cos(wAngleY*3.14/180.0);
		double zmov = -8;//.025*sin(wAngleY*3.14/180.0);

		xmov = 0;

		x = m_pDoc->m_aObjectColours[m_currentobject].red;
		y = m_pDoc->m_aObjectColours[m_currentobject].green;
		z = m_pDoc->m_aObjectColours[m_currentobject].blue;
		if(m_pDoc->m_Ord3d == 2) z = 0.5;
		else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
		
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);
		
		TextOut(hdc,(long)tx+(long)xmov+width,(long)ty+(long)zmov,text,text.GetLength());

		if((m_compareobject >= 0) &&
			m_compareobject < m_pDoc->m_aObjectColours.GetSize() &&
			m_compareobject < m_pDoc->GetNoOfRows()){
			//highlight compare object at 
			m_pDoc->GetRowLabel(m_compareobject,text);
			x = m_pDoc->m_aObjectColours[m_compareobject].red;
			y = m_pDoc->m_aObjectColours[m_compareobject].green;
			z = m_pDoc->m_aObjectColours[m_compareobject].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
			
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);
			
			TextOut(hdc,(long)tx+(long)xmov+width,(long)ty+(long)zmov,text,text.GetLength());
		}
	}

	//highlight an centroid
	if((m_currentcentroid >= 0 && m_currentcentroid < m_pDoc->m_aGroupColours.GetSize() &&
		m_currentcentroid < m_pDoc->m_Groups.GetSize()) ||
		m_hint == centroidselection || m_hint == centroidcompare){

		//highlight single centroid at 
		double xmov = 8;//.05*cos(wAngleY*3.14/180.0);
		double zmov = -8;//.05*sin(wAngleY*3.14/180.0);
		
		xmov = 0;
		
		if(state & MF_CHECKED)
		{
			text.Format("%d",m_currentcentroid+1);
			x = m_pDoc->m_aAprioriGroupColours[m_currentcentroid].red;
			y = m_pDoc->m_aAprioriGroupColours[m_currentcentroid].green;
			z = m_pDoc->m_aAprioriGroupColours[m_currentcentroid].blue;
		}
		else
		{
			text.Format("%d",m_pDoc->m_Groups.GetAt(m_currentcentroid));
			x = m_pDoc->m_aGroupColours[m_currentcentroid].red;
			y = m_pDoc->m_aGroupColours[m_currentcentroid].green;
			z = m_pDoc->m_aGroupColours[m_currentcentroid].blue;
		}
		if(m_pDoc->m_Ord3d == 2) z = 0.5;
		else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
		
		App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);
		
		TextOut(hdc,(long)tx+(long)xmov+width,(long)ty+(long)zmov,text,text.GetLength());

		if(!(state & MF_CHECKED) && m_comparecentroid != -1 &&
			m_comparecentroid < m_pDoc->m_aGroupColours.GetSize() &&
			m_comparecentroid < m_pDoc->m_Groups.GetSize()){
			//highlight compare centroid at 
			text.Format("%d",m_comparecentroid+1);
			x = m_pDoc->m_aGroupColours[m_comparecentroid].red;
			y = m_pDoc->m_aGroupColours[m_comparecentroid].green;
			z = m_pDoc->m_aGroupColours[m_comparecentroid].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
			
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);
			
			TextOut(hdc,(long)tx+(long)xmov+width,(long)ty+(long)zmov,text,text.GetLength());
		}

		if((state & MF_CHECKED) && m_comparecentroid != -1 &&
			m_comparecentroid < m_pDoc->m_aAprioriGroupColours.GetSize() &&
			m_comparecentroid < m_pDoc->m_AprioriGroups.GetSize()){
			//highlight compare centroid at 
			text.Format("%d",m_comparecentroid+1);
			x = m_pDoc->m_aAprioriGroupColours[m_comparecentroid].red;
			y = m_pDoc->m_aAprioriGroupColours[m_comparecentroid].green;
			z = m_pDoc->m_aAprioriGroupColours[m_comparecentroid].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
			
			App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);
			
			TextOut(hdc,(long)tx+(long)xmov+width,(long)ty+(long)zmov,text,text.GetLength());

		}
	}

	//draw line for comparison (is needed when m_hint == blahcompare)
	if(m_compareobject != -1 &&
		m_currentobject < m_pDoc->m_aObjectColours.GetSize()){

		x = m_pDoc->m_aObjectColours[m_currentobject].red;
		y = m_pDoc->m_aObjectColours[m_currentobject].green;
		z = m_pDoc->m_aObjectColours[m_currentobject].blue;

		if(m_pDoc->m_Ord3d == 2) z = 0.5;
		else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
		
//		App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);

		//MoveToEx(hdc,(long)tx + width,(long)ty,NULL);
		m_line[0].x = (x-.5f)*x_scale;
		m_line[0].y = (y-.5f)*y_scale;
		m_line[0].z = (z-.5f)*z_scale;
					
		x = m_pDoc->m_aObjectColours[m_compareobject].red;
		y = m_pDoc->m_aObjectColours[m_compareobject].green;
		z = m_pDoc->m_aObjectColours[m_compareobject].blue;

		if(m_pDoc->m_Ord3d == 2) z = 0.5;
		else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
		
		//App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);

		m_line[1].x = (x-.5f)*x_scale;
		m_line[1].y = (y-.5f)*y_scale;
		m_line[1].z = (z-.5f)*z_scale;

		if(m_line != NULL)
		{
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,
				m_line, 2, NULL );
		}
	}else if(m_hint == objectcompare && 
		m_currentobject < m_pDoc->m_aObjectColours.GetSize()){
		x = m_pDoc->m_aObjectColours[m_currentobject].red;
		y = m_pDoc->m_aObjectColours[m_currentobject].green;
		z = m_pDoc->m_aObjectColours[m_currentobject].blue;
		if(m_pDoc->m_Ord3d == 2) z = 0.5;
		else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}

		//App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);

			m_line[0].x = (x-.5f)*x_scale;
			m_line[0].y = (y-.5f)*y_scale;
			m_line[0].z = (z-.5f)*z_scale;

//		MoveToEx(hdc,(long)tx + width,(long)ty,NULL);
		//LineTo(hdc,m_comparepoint.x,m_comparepoint.y);

			App_Corner_Reverse(g_pd3dDevice,&tx,&ty,&tz,(float)(m_comparepoint.x-width),(float)(m_comparepoint.y),0);

			m_line[1].x = (tx);
			m_line[1].y = (ty);
			m_line[1].z = (tz);

		if(m_line != NULL)
		{
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,
				m_line, 2, NULL );
		}


	}
	if(state & MF_CHECKED)
	{
		if(m_comparecentroid != -1 &&
			m_currentcentroid != -1 &&
			m_currentcentroid < m_pDoc->m_aAprioriGroupColours.GetSize() &&
			m_comparecentroid < m_pDoc->m_aAprioriGroupColours.GetSize()){
			x = m_pDoc->m_aAprioriGroupColours[m_currentcentroid].red;
			y = m_pDoc->m_aAprioriGroupColours[m_currentcentroid].green;
			z = m_pDoc->m_aAprioriGroupColours[m_currentcentroid].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
			
//			App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);

			m_line[0].x = (x-.5f)*x_scale;
			m_line[0].y = (y-.5f)*y_scale;
			m_line[0].z = (z-.5f)*z_scale;

//			MoveToEx(hdc,(long)tx + width,(long)ty,NULL);

			x = m_pDoc->m_aAprioriGroupColours[m_comparecentroid].red;
			y = m_pDoc->m_aAprioriGroupColours[m_comparecentroid].green;
			z = m_pDoc->m_aAprioriGroupColours[m_comparecentroid].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
			
			//App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);

			//LineTo(hdc,(long)tx + width,(long)ty);
			m_line[1].x = (x-.5f)*x_scale;
			m_line[1].y = (y-.5f)*y_scale;
			m_line[1].z = (z-.5f)*z_scale;


			if(m_line != NULL)
				g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,
				m_line, 2, NULL );


		}else if (m_hint == centroidcompare && m_currentcentroid < m_pDoc->m_aAprioriGroupColours.GetSize()) {
			x = m_pDoc->m_aAprioriGroupColours[m_currentcentroid].red;
			y = m_pDoc->m_aAprioriGroupColours[m_currentcentroid].green;
			z = m_pDoc->m_aAprioriGroupColours[m_currentcentroid].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
			
//			App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);

			m_line[0].x = (x-.5f)*x_scale;
			m_line[0].y = (y-.5f)*y_scale;
			m_line[0].z = (z-.5f)*z_scale;

			//MoveToEx(hdc,(long)tx + width,(long)ty,NULL);
			//LineTo(hdc,m_comparepoint.x,m_comparepoint.y);

			App_Corner_Reverse(g_pd3dDevice,&tx,&ty,&tz,(float)(m_comparepoint.x-width),(float)(m_comparepoint.y),0);

			m_line[1].x = (tx);
			m_line[1].y = (ty);
			m_line[1].z = (tz);


		if(m_line != NULL)
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,
				m_line, 2, NULL );

		}
	}
	else
	{
		if(m_comparecentroid != -1 &&
			m_currentcentroid != -1 &&
			m_currentcentroid < m_pDoc->m_aGroupColours.GetSize() &&
			m_comparecentroid < m_pDoc->m_aGroupColours.GetSize()){
			x = m_pDoc->m_aGroupColours[m_currentcentroid].red;
			y = m_pDoc->m_aGroupColours[m_currentcentroid].green;
			z = m_pDoc->m_aGroupColours[m_currentcentroid].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
			
			//App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);
		
			m_line[0].x = (x-.5f)*x_scale;
			m_line[0].y = (y-.5f)*y_scale;
			m_line[0].z = (z-.5f)*z_scale;

//			MoveToEx(hdc,(long)tx + width,(long)ty,NULL);

			x = m_pDoc->m_aGroupColours[m_comparecentroid].red;
			y = m_pDoc->m_aGroupColours[m_comparecentroid].green;
			z = m_pDoc->m_aGroupColours[m_comparecentroid].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
			
			//App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);

//			LineTo(hdc,(long)tx + width,(long)ty);

			m_line[1].x = (x-.5f)*x_scale;
			m_line[1].y = (y-.5f)*y_scale;
			m_line[1].z = (z-.5f)*z_scale;


		if(m_line != NULL)
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,
				m_line, 2, NULL );

		
		}else if (m_hint == centroidcompare && m_currentcentroid < m_pDoc->m_aGroupColours.GetSize()) {
			x = m_pDoc->m_aGroupColours[m_currentcentroid].red;
			y = m_pDoc->m_aGroupColours[m_currentcentroid].green;
			z = m_pDoc->m_aGroupColours[m_currentcentroid].blue;
			if(m_pDoc->m_Ord3d == 2) z = 0.5;
			else if(m_pDoc->m_Ord3d == 1) {z = 0.5;y=0.5;}
			
//			App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);

			//MoveToEx(hdc,(long)tx + width,(long)ty,NULL);
			m_line[0].x = (x-.5f)*x_scale;
			m_line[0].y = (y-.5f)*y_scale;
			m_line[0].z = (z-.5f)*z_scale;

			App_Corner_Reverse(g_pd3dDevice,&tx,&ty,&tz,(float)(m_comparepoint.x-width),(float)(m_comparepoint.y),0);

			m_line[1].x = (tx);
			m_line[1].y = (ty);
			m_line[1].z = (tz);
		
			if(m_line != NULL)
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_VERTEX,
				m_line, 2, NULL );

		}
	}
//	if(oldfont != NULL)
//		SelectObject(hdc,oldfont);
//
//	DeleteObject(font);


}

BOOL C3DXView::PreCreateWindow(CREATESTRUCT& cs) 
{
	HICON icon = AfxGetApp()->LoadIcon(IDR_3DVIEW_ICON);

	// Styles required by OpenGL
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC | CS_PARENTDC | CS_SAVEBITS | CS_CLASSDC,
		::LoadCursor(NULL, IDC_ARROW), NULL, icon);

	cs.cy = 1024;
	cs.cx = 960;

	return CView::PreCreateWindow(cs);
}


bool C3DXView::RenderBySize(long x,long y)
{
	GUID* pDriverGUID = NULL;
	const GUID* pDeviceGUID = &IID_IDirect3DRGBDevice;
	
	Cleanup3DEnvironment();

	HRESULT hr;

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		// Create the IDirectDraw interface. The first parameter is the GUID,
		// which is allowed to be NULL. If there are more than one DirectDraw
		// drivers on the system, a NULL guid requests the primary driver. For 
		// non-GDI hardware cards like the 3DFX and PowerVR, the guid would need
		// to be explicity specified . (Note: these guids are normally obtained
		// from enumeration, which is convered in a subsequent tutorial.)
		hr = MDirectDrawCreateEx( pDriverGUID, (VOID**)&g_pDD, IID_IDirectDraw7,
								 NULL );	

		if( FAILED( hr ) )
			return false;//hr;

		if(true || max_x == 0)
		{			
			g_pDD->EnumDisplayModes(0,0,0,(LPDDENUMMODESCALLBACK2)EnumDisplayModesCallback);
			g_pDD->EnumSurfaces(0,0,0,(LPDDENUMSURFACESCALLBACK7)EnumSurfacesCallback);
		}

		// Set the Windows cooperative level. This is where we tell the system
		// whether wew will be rendering in fullscreen mode or in a window. Note
		// that some hardware (non-GDI) may not be able to render into a window.
		// The flag DDSCL_NORMAL specifies windowed mode. Using fullscreen mode
		// is the topic of a subsequent tutorial. The DDSCL_FPUSETUP flag is a 
		// hint to DirectX to optomize floating points calculations. See the docs
		// for more info on this. Note: this call could fail if another application
		// already controls a fullscreen, exclusive mode.
		hr = g_pDD->SetCooperativeLevel( GetSafeHwnd(), DDSCL_NORMAL );
		if( FAILED( hr ) )
			return false;// hr;

		if(true || max_x == 0)
		{
			g_pDD->EnumDisplayModes(0,0,0,(LPDDENUMMODESCALLBACK2)EnumDisplayModesCallback);
		}

		// Initialize a surface description structure for the primary surface. The
		// primary surface represents the entire display, with dimensions and a
		// pixel format of the display. Therefore, none of that information needs
		// to be specified in order to create the primary surface.
		DDSURFACEDESC2 ddsd;
		ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
		ddsd.dwSize         = sizeof(DDSURFACEDESC2);
		//ddsd.dwFlags        = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		// Create the primary surface.
		//hr = g_pDD->CreateSurface( &ddsd, &g_pddsPrimary, NULL );
		//if( FAILED( hr ) )
		//	return false;// hr;

		// Setup a surface description to create a backbuffer. This is an
		// offscreen plain surface with dimensions equal to our window size.
		// The DDSCAPS_3DDEVICE is needed so we can later query this surface
		// for an IDirect3DDevice interface.
		ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT;// | DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY | DDSCAPS_3DDEVICE;

		// Set the dimensions of the backbuffer. Note that if our window changes
		// size, we need to destroy this surface and create a new one.
		//GetClientRect(&g_rcScreenRect );
		g_rcScreenRect.left = g_rcScreenRect.top = 0;
		g_rcScreenRect.right = x;
		g_rcScreenRect.bottom = y;
		//if(m_printimage) 
		//	g_rcScreenRect = m_rectDraw;
		//ClientToScreen((POINT*)&g_rcScreenRect.left );
		//ClientToScreen((POINT*)&g_rcScreenRect.right );
		ddsd.dwWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
		ddsd.dwHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;

		//if(ddsd.dwWidth-120 > ddsd.dwHeight)
			//ddsd.dwWidth = ddsd.dwHeight;
		//else
		//	ddsd.dwHeight = ddsd.dwWidth-120;

		// Create the backbuffer. The most likely reason for failure is running
		// out of video memory. (A more sophisticated app should handle this.)
		hr = g_pDD->CreateSurface( &ddsd, &g_pddsBackBuffer, NULL  );
		if( FAILED( hr ) )
			return false;

		// Note: if using a z-buffer, the zbuffer surface creation would go around
		// here. However, z-buffer usage is the topic of a subsequent tutorial.

		// Create a clipper object which handles all our clipping for cases when
		// our window is partially obscured by other windows. This is not needed
		// for apps running in fullscreen mode.
		//LPDIRECTDRAWCLIPPER pcClipper;
		//hr = g_pDD->CreateClipper( 0, &pcClipper, NULL );
		//if( FAILED( hr ) )
		//	return false;// hr;

		// Associate the clipper with our window. Note that, afterwards, the
		// clipper is internally referenced by the primary surface, so it is safe
		// to release our local reference to it.
		//pcClipper->SetHWnd( 0, GetSafeHwnd() );
		//g_pddsPrimary->SetClipper( pcClipper );
		//pcClipper->Release();

		// Query DirectDraw for access to Direct3D
		g_pDD->QueryInterface( IID_IDirect3D7, (VOID**)&g_pD3D );
		if( FAILED( hr) )
			return false;// hr;

		// Enumerate the various z-buffer formats, finding a DDPIXELFORMAT
		// to use to create the z-buffer surface.
		DDPIXELFORMAT ddpfZBuffer;
		g_pD3D->EnumZBufferFormats( *pDeviceGUID, 
									EnumZBufferCallback, (VOID*)&ddpfZBuffer );

		// If we found a good zbuffer format, then the dwSize field will be
		// properly set during enumeration. Else, we have a problem and will exit.
		if( sizeof(DDPIXELFORMAT) != ddpfZBuffer.dwSize )
			return false;// E_FAIL;

		// Get z-buffer dimensions from the render target
		// Setup the surface desc for the z-buffer.
		ddsd.dwFlags        = DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
		ddsd.dwWidth        = g_rcScreenRect.right - g_rcScreenRect.left;
		ddsd.dwHeight       = g_rcScreenRect.bottom - g_rcScreenRect.top;
		//if(ddsd.dwWidth-120 > ddsd.dwHeight)
		//	ddsd.dwWidth = ddsd.dwHeight;
		//else
		//	ddsd.dwHeight = ddsd.dwWidth-120;
		memcpy( &ddsd.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT) );

		// For hardware devices, the z-buffer should be in video memory. For
		// software devices, create the z-buffer in system memory
		//if( IsEqualIID( *pDeviceGUID, IID_IDirect3DHALDevice ) )
		//	ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		//else
			ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

		// Create and attach a z-buffer. Real apps should be able to handle an
		// error here (DDERR_OUTOFVIDEOMEMORY may be encountered). For this 
		// tutorial, though, we are simply going to exit ungracefully.
		if( FAILED( hr = g_pDD->CreateSurface( &ddsd, &g_pddsZBuffer, NULL ) ) )
			return false;

		// Attach the z-buffer to the back buffer.
		if( FAILED( hr = g_pddsBackBuffer->AddAttachedSurface( g_pddsZBuffer ) ) )
			return false;

		// Before creating the device, check that we are NOT in a palettized
		// display. That case will cause CreateDevice() to fail, since this simple 
		// tutorial does not bother with palettes.
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		g_pDD->GetDisplayMode( &ddsd );
		if( ddsd.ddpfPixelFormat.dwRGBBitCount <= 8 )
			return false;// DDERR_INVALIDMODE;



		// Create the device. The device is created off of our back buffer, which
		// becomes the render target for the newly created device. Note that the
		// z-buffer must be created BEFORE the device

		if(true || max_x == 0)
		{			
			g_pDD->EnumDisplayModes(0,0,0,(LPDDENUMMODESCALLBACK2)EnumDisplayModesCallback);
			g_pDD->EnumSurfaces(0,0,0,(LPDDENUMSURFACESCALLBACK7)EnumSurfacesCallback);
		}

		if( FAILED( hr = g_pD3D->CreateDevice( *pDeviceGUID, g_pddsBackBuffer,
											   &g_pd3dDevice ) ) )
		{
			// This call could fail for many reasons. The most likely cause is
			// that we specifically requested a hardware device, without knowing
			// whether there is even a 3D card installed in the system. Another
			// possibility is the hardware is incompatible with the current display
			// mode (the correct implementation would use enumeration for this.)
			int i;
			switch(hr){
			case DD_OK:
				i = 0;break;
			case DDERR_ALREADYINITIALIZED:
				i = 0;break;
			case DDERR_BLTFASTCANTCLIP :
				i = 0;break;
			case DDERR_CANNOTATTACHSURFACE :
				i = 0;break;
			case DDERR_CANNOTDETACHSURFACE :
				i = 0;break;
			case DDERR_CANTCREATEDC :
				i = 0;break;
			case DDERR_CANTDUPLICATE :
				i = 0;break;
			case DDERR_CANTLOCKSURFACE :
				i = 0;break;
			case DDERR_CANTPAGELOCK :
				i = 0;break;
			case DDERR_CANTPAGEUNLOCK :
				i = 0;break; 
			case DDERR_CLIPPERISUSINGHWND :
				i = 0;break; 
			case DDERR_COLORKEYNOTSET :
				i = 0;break; 
			case DDERR_CURRENTLYNOTAVAIL :
				i = 0;break; 
			case DDERR_DDSCAPSCOMPLEXREQUIRED :
				i = 0;break; 
			case DDERR_DCALREADYCREATED :
				i = 0;break; 
			case DDERR_DEVICEDOESNTOWNSURFACE :
				i = 0;break; 
			case DDERR_DIRECTDRAWALREADYCREATED :
				i = 0;break; 
			case DDERR_EXCEPTION :
				i = 0;break; 
			case DDERR_EXCLUSIVEMODEALREADYSET :
				i = 0;break; 
			case DDERR_EXPIRED :
				i = 0;break; 
			case DDERR_GENERIC :
				i = 0;break; 
			case DDERR_HEIGHTALIGN :
				i = 0;break; 
			case DDERR_HWNDALREADYSET :
				i = 0;break; 
			case DDERR_HWNDSUBCLASSED :
				i = 0;break; 
			case DDERR_IMPLICITLYCREATED :
				i = 0;break; 
			case DDERR_INCOMPATIBLEPRIMARY :
				i = 0;break; 
			case DDERR_INVALIDCAPS :
				i = 0;break; 
			case DDERR_INVALIDCLIPLIST :
				i = 0;break; 
			case DDERR_INVALIDDIRECTDRAWGUID :
				i = 0;break; 
			case DDERR_INVALIDMODE :
				i = 0;break;
			case DDERR_INVALIDOBJECT :
				i = 0;break;
			case DDERR_INVALIDPARAMS :
				i = 0;break;
			case DDERR_INVALIDPIXELFORMAT :
				i = 0;break;
			case DDERR_INVALIDPOSITION :
				i = 0;break;
			case DDERR_INVALIDRECT :
				i = 0;break;
			case DDERR_INVALIDSTREAM :
				i = 0;break;
			case DDERR_INVALIDSURFACETYPE :
				i = 0;break;
			case DDERR_LOCKEDSURFACES :
				i = 0;break;
			case DDERR_MOREDATA :
				i = 0;break;
			case DDERR_NEWMODE :
				i = 0;break;
			case DDERR_NO3D :
				i = 0;break;
			case DDERR_NOALPHAHW :
				i = 0;break;
			case DDERR_NOBLTHW :
				i = 0;break; 
			case DDERR_NOCLIPLIST :
				i = 0;break; 
			case DDERR_NOCLIPPERATTACHED :
				i = 0;break;
			case DDERR_NOCOLORCONVHW :
				i = 0;break; 
			case DDERR_NOCOLORKEY :
				i = 0;break; 
			case DDERR_NOCOLORKEYHW :
				i = 0;break; 
			case DDERR_NOCOOPERATIVELEVELSET :
				i = 0;break; 
			case DDERR_NODC :
				i = 0;break; 
			case DDERR_NODDROPSHW :
				i = 0;break; 
			case DDERR_NODIRECTDRAWHW :
				i = 0;break; 
			case DDERR_NODIRECTDRAWSUPPORT :
				i = 0;break;
			case DDERR_NODRIVERSUPPORT :
				i = 0;break; 
			case DDERR_NOEMULATION :
				i = 0;break; 
			case DDERR_NOEXCLUSIVEMODE :
				i = 0;break; 
			case DDERR_NOFLIPHW :
				i = 0;break; 
			case DDERR_NOFOCUSWINDOW :
				i = 0;break; 
			case DDERR_NOGDI :
				i = 0;break; 
			case DDERR_NOHWND :
				i = 0;break; 
			case DDERR_NOMIPMAPHW :
				i = 0;break; 
			case DDERR_NOMIRRORHW :
				i = 0;break; 
			case DDERR_NOMONITORINFORMATION :
				i = 0;break; 
			case DDERR_NONONLOCALVIDMEM :
				i = 0;break; 
			case DDERR_NOOPTIMIZEHW :
				i = 0;break; 
			case DDERR_NOOVERLAYDEST :
				i = 0;break; 
			case DDERR_NOOVERLAYHW :
				i = 0;break; 
			case DDERR_NOPALETTEATTACHED :
				i = 0;break; 
			case DDERR_NOPALETTEHW :
				i = 0;break; 
			case DDERR_NORASTEROPHW :
				i = 0;break;
			case DDERR_NOROTATIONHW :
				i = 0;break; 
			case DDERR_NOSTEREOHARDWARE :
				i = 0;break;
			case DDERR_NOSTRETCHHW :
				i = 0;break;
			case DDERR_NOSURFACELEFT :
				i = 0;break;
			case DDERR_NOT4BITCOLOR :
				i = 0;break;
			case DDERR_NOT4BITCOLORINDEX :
				i = 0;break;
			case DDERR_NOT8BITCOLOR :
				i = 0;break; 
			case DDERR_NOTAOVERLAYSURFACE :
				i = 0;break;
			case DDERR_NOTEXTUREHW :
				i = 0;break;
			case DDERR_NOTFLIPPABLE :
				i = 0;break; 
			case DDERR_NOTFOUND :
				i = 0;break;
			case DDERR_NOTINITIALIZED :
				i = 0;break;
			case DDERR_NOTLOADED :
				i = 0;break;
			case DDERR_NOTLOCKED :
				i = 0;break;
			case DDERR_NOTPAGELOCKED :
				i = 0;break;
			case DDERR_NOTPALETTIZED :
				i = 0;break;
			case DDERR_NOVSYNCHW :
				i = 0;break;
			case DDERR_NOZBUFFERHW :
				i = 0;break;
			case DDERR_NOZOVERLAYHW :
				i = 0;break;
			case DDERR_OUTOFCAPS :
				i = 0;break;
			case DDERR_OUTOFMEMORY :
				i = 0;break; 
			case DDERR_OUTOFVIDEOMEMORY :
				i = 0;break;
			case DDERR_OVERLAPPINGRECTS :
				i = 0;break;
			case DDERR_OVERLAYCANTCLIP :
				i = 0;break;
			case DDERR_OVERLAYCOLORKEYONLYONEACTIVE :
				i = 0;break;
			case DDERR_OVERLAYNOTVISIBLE :
				i = 0;break;
			case DDERR_PALETTEBUSY :
				i = 0;break;
			case DDERR_PRIMARYSURFACEALREADYEXISTS :
				i = 0;break;
			case DDERR_REGIONTOOSMALL :
				i = 0;break; 
			case DDERR_SURFACEALREADYATTACHED :
				i = 0;break;
			case DDERR_SURFACEALREADYDEPENDENT :
				i = 0;break;
			case DDERR_SURFACEBUSY :
				i = 0;break;
			case DDERR_SURFACEISOBSCURED :
				i = 0;break;
			case DDERR_SURFACELOST :
				i = 0;break;
			case DDERR_SURFACENOTATTACHED :
				i = 0;break;
			case DDERR_TESTFINISHED :
				i = 0;break;
			case DDERR_TOOBIGHEIGHT :
				i = 0;break;
			case DDERR_TOOBIGSIZE :
				i = 0;break;
			case DDERR_TOOBIGWIDTH :
				i = 0;break;
			case DDERR_UNSUPPORTED :
				i = 0;break;
			case DDERR_UNSUPPORTEDFORMAT :
				i = 0;break;
			case DDERR_UNSUPPORTEDMASK :
				i = 0;break;
			case DDERR_UNSUPPORTEDMODE :
				i = 0;break;
			case DDERR_VERTICALBLANKINPROGRESS :
				i = 0;break;
			case DDERR_VIDEONOTACTIVE :
				i = 0;break; 
			case DDERR_WASSTILLDRAWING :
				i = 0;break;
			case DDERR_WRONGMODE :
				i = 0;break;
			case DDERR_XALIGN :
				i = 0;break;
			default:
				i = 0;break;
			}
			return false;
		}
	//}

	//g_pd3dDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS,D3DANTIALIAS_SORTINDEPENDENT );
	g_pd3dDevice->SetRenderState(D3DRENDERSTATE_EDGEANTIALIAS,TRUE);

	// Finish by setting up our scene
	App_InitDeviceObjects( g_pd3dDevice );

	App_FrameMove( g_pd3dDevice, (m_time++)/100.0f);//((FLOAT)clock())/CLOCKS_PER_SEC );

	App_Render(g_pd3dDevice);

	return true;
}

bool C3DXView::RenderLegend(long x,long y)
{
	GUID* pDriverGUID = NULL;
	const GUID* pDeviceGUID = &IID_IDirect3DRGBDevice;
	
	Cleanup3DEnvironment();	
	
	HRESULT hr;

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		// Create the IDirectDraw interface. The first parameter is the GUID,
		hr = MDirectDrawCreateEx( pDriverGUID, (VOID**)&g_pDD, IID_IDirectDraw7,
								 NULL );
		if( FAILED( hr ) )
			return false;// hr;

		hr = g_pDD->SetCooperativeLevel( GetSafeHwnd(), DDSCL_NORMAL );
		if( FAILED( hr ) )
			return false;// hr;

		DDSURFACEDESC2 ddsd;
		ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
		ddsd.dwSize         = sizeof(DDSURFACEDESC2);
		ddsd.dwFlags        = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		// Create the primary surface.
		hr = g_pDD->CreateSurface( &ddsd, &g_pddsPrimary, NULL );
		if( FAILED( hr ) )
			return false;// hr;

		ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

		g_rcScreenRect.left = g_rcScreenRect.top = 0;
		g_rcScreenRect.right = x;
		g_rcScreenRect.bottom = y;
		ddsd.dwWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
		ddsd.dwHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;

		hr = g_pDD->CreateSurface( &ddsd, &g_pddsBackBuffer, NULL );
		if( FAILED( hr ) )
			return false;

		LPDIRECTDRAWCLIPPER pcClipper;
		hr = g_pDD->CreateClipper( 0, &pcClipper, NULL );
		if( FAILED( hr ) )
			return false;// hr;

		pcClipper->SetHWnd( 0, GetSafeHwnd() );
		g_pddsPrimary->SetClipper( pcClipper );
		pcClipper->Release();

		// Query DirectDraw for access to Direct3D
		g_pDD->QueryInterface( IID_IDirect3D7, (VOID**)&g_pD3D );
		if( FAILED( hr) )
			return false;// hr;

		// Enumerate the various z-buffer formats, finding a DDPIXELFORMAT
		// to use to create the z-buffer surface.
		DDPIXELFORMAT ddpfZBuffer;
		g_pD3D->EnumZBufferFormats( *pDeviceGUID, 
									EnumZBufferCallback, (VOID*)&ddpfZBuffer );

		// If we found a good zbuffer format, then the dwSize field will be
		// properly set during enumeration. Else, we have a problem and will exit.
		if( sizeof(DDPIXELFORMAT) != ddpfZBuffer.dwSize )
			return false;// E_FAIL;

		// Get z-buffer dimensions from the render target
		// Setup the surface desc for the z-buffer.
		ddsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
		ddsd.dwWidth        = g_rcScreenRect.right - g_rcScreenRect.left;
		ddsd.dwHeight       = g_rcScreenRect.bottom - g_rcScreenRect.top;
		memcpy( &ddsd.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT) );

		// For hardware devices, the z-buffer should be in video memory. For
		// software devices, create the z-buffer in system memory
		if( IsEqualIID( *pDeviceGUID, IID_IDirect3DHALDevice ) )
			ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
		else
			ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

		if( FAILED( hr = g_pDD->CreateSurface( &ddsd, &g_pddsZBuffer, NULL ) ) )
			return false;

		// Attach the z-buffer to the back buffer.
		if( FAILED( hr = g_pddsBackBuffer->AddAttachedSurface( g_pddsZBuffer ) ) )
			return false;

		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		g_pDD->GetDisplayMode( &ddsd );
		if( ddsd.ddpfPixelFormat.dwRGBBitCount <= 8 )
			return false;// DDERR_INVALIDMODE;

		if( FAILED( hr = g_pD3D->CreateDevice( *pDeviceGUID, g_pddsBackBuffer,
											   &g_pd3dDevice ) ) )
		{
			return false;
		}
		//g_pd3dDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS,D3DANTIALIAS_SORTINDEPENDENT );
		g_pd3dDevice->SetRenderState(D3DRENDERSTATE_EDGEANTIALIAS,TRUE);
	//}

	// Finish by setting up our scene
	App_InitDeviceObjects( g_pd3dDevice );

	App_FrameMove( g_pd3dDevice, (m_time++)/100.0f);//((FLOAT)clock())/CLOCKS_PER_SEC );

	HDC hdc;

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		if( FAILED( g_pd3dDevice->BeginScene() ) )
			return false;
	//}

	D3DVIEWPORT7 vp2 = { 0, 0, g_rcScreenRect.right,g_rcScreenRect.bottom, 0.0f, 1.0f };

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		hr = g_pd3dDevice->SetViewport( &vp2 );
	//}
	
	if( FAILED( hr ) )
		return false;

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
			0x00ffffff, 1.0f, 0L );
	//g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );
	
		g_pddsBackBuffer->GetDC(&hdc);
	//}else{
	//	hdc = GetDC()->m_hDC;
	//}

	SetBkMode(hdc,TRANSPARENT);

	BYTE quality = m_pDoc->m_lfCells.lfQuality;
	m_pDoc->m_lfCells.lfQuality |= ANTIALIASED_QUALITY;
	HFONT oldfont,font = CreateFontIndirect(&m_pDoc->m_lfCells);
	m_pDoc->m_lfCells.lfQuality = quality;

	oldfont = (HFONT) SelectObject(hdc,font);			

	DrawLegend(hdc);

	if(oldfont != NULL)
		SelectObject(hdc,oldfont);

	DeleteObject(font);

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		g_pddsBackBuffer->ReleaseDC(hdc);
		g_pd3dDevice->EndScene();
	//}

	return true;
}



void C3DXView::OnRotate() 
{
	if(m_legend_instance) return;

	if(m_Rotate){
		m_Rotate = false;
		KillTimer(1);
		m_menu.CheckMenuItem(ID__ROTATE,MF_UNCHECKED);
	}else{
		m_Rotate = true;
		SetTimer(1,50,NULL);
		m_menu.CheckMenuItem(ID__ROTATE,MF_CHECKED);
	}	
}

void C3DXView::OnUpdateRotate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(false);
	if(m_legend_instance) return;

	//pCmdUI->Enable(m_pDoc->m_Ord3d);		
	pCmdUI->Enable(true);		
}

void C3DXView::OnTimer(UINT nIDEvent) 
{
	if(m_legend_instance) return;

	if(nIDEvent == 2) {
		RedrawWindow();
	} else {
		KillTimer(1);
		CView::OnTimer(nIDEvent);

		if(m_settingrotate){
			RotateAboutVector(&m_vEyePt,&m_vUpVec,(m_rotatepoint.x-m_oldpoint.x)/-10000.0);
			RotateAboutVector(&m_vRight,&m_vUpVec,(m_rotatepoint.x-m_oldpoint.x)/-10000.0);

			RotateAboutVector(&m_vEyePt,&m_vRight,(m_rotatepoint.y-m_oldpoint.y)/10000.0);
			RotateAboutVector(&m_vUpVec,&m_vRight,(m_rotatepoint.y-m_oldpoint.y)/10000.0);
			
			
		}else{
			RotateAboutVector(&m_vEyePt,&m_vUpVec,m_rotatepoint.x/-10000.0);
			RotateAboutVector(&m_vRight,&m_vUpVec,m_rotatepoint.x/-10000.0);
			RotateAboutVector(&m_vEyePt,&m_vRight,m_rotatepoint.y/10000.0);
			RotateAboutVector(&m_vUpVec,&m_vRight,m_rotatepoint.y/10000.0);;

		}

		UpdateZOrders();

		RedrawWindow();
		
		if(g_avi_count > 0){
			SaveAVI();
			SetTimer(1,1,NULL);
		}else{
			SetTimer(1,50,NULL);
		}
	}
}

void C3DXView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if(m_legend_instance) return;

	if(nFlags & MK_SHIFT){ 
		m_settingrotate = true;
		m_oldpoint = point;
		m_rotatepoint = point;
	}
	
	CView::OnRButtonDown(nFlags, point);
}

void C3DXView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if(m_legend_instance) return;

	if(!m_settingrotate){
		CView::OnRButtonUp(nFlags, point);
	}else{
		m_settingrotate = false;
		m_rotatepoint.x -= m_oldpoint.x;
		m_rotatepoint.y -= m_oldpoint.y;
	}
}

void C3DXView::OnMinusrotationspeed() 
{
	if(m_legend_instance) return;
	if(!m_settingrotate){
		if(m_rotatepoint.x >= 1.5) m_rotatepoint.x = (long)(m_rotatepoint.x / 1.5);
		if(m_rotatepoint.y >= 1.5) m_rotatepoint.y = (long)(m_rotatepoint.y / 1.5);
	}	
}

void C3DXView::OnPlusrotationspeed() 
{
	if(m_legend_instance) return;
	if(!m_settingrotate){
		if(m_rotatepoint.x == (long)(m_rotatepoint.x * 1.5)) m_rotatepoint.x = (long)(m_rotatepoint.x + 1);
		else m_rotatepoint.x = (long)(m_rotatepoint.x * 1.5);
		if(m_rotatepoint.y == (long)(m_rotatepoint.y * 1.5)) m_rotatepoint.y = (long)(m_rotatepoint.y + 1);
		else m_rotatepoint.y = (long)(m_rotatepoint.y * 1.5);
	}
}

bool C3DXView::HitSphere2(float x,float y,float z,CPoint point)
{
	//triangle hit test on m_sphere struct with center about x,y,z (which are 0->1)
	CPatnApp * app = (CPatnApp*)AfxGetApp();
	D3DVERTEX v[3];

//	long idx;

	App_Corner(g_pd3dDevice,&v[0].x,&v[0].y,&v[0].z,(x-.5f)*x_scale,(y-.5f)*y_scale,(z-.5f)*z_scale);

	App_Corner(g_pd3dDevice,&v[1].x,&v[1].y,&v[1].z,
		(x+(float)app->m_ordobjectsize*.5f/1.5f-.5f)*x_scale,
		(y+(float)app->m_ordobjectsize*.5f/1.5f-.5f)*y_scale,
		(z+(float)app->m_ordobjectsize*.5f/1.5f-.5f)*z_scale);

	double radius1,radius2;

	radius1 = sqrt((v[0].x-v[1].x)*(v[0].x-v[1].x) + (v[0].y-v[1].y)*(v[0].y-v[1].y) + (v[0].z-v[1].z)*(v[0].z-v[1].z));
	radius2 = sqrt((v[0].x-point.x)*(v[0].x-point.x) + (v[0].y-point.y)*(v[0].y-point.y));

	if(radius2 <= radius1)
		return true;
	
	return false;
}

void C3DXView::DrawSphere2(double x,double y,double z,double red,double green,double blue,bool selected, bool grouped)
{
	D3DVERTEX sphere[ _NUM_SPHERE_VERTICIES_ ];

//	float xscale, yscale, zscale;

//	D3DVECTOR ne = m_vEyePt; Normalize(m_vEyePt);
//	xscale = ne.x *20 + 1;
//	yscale = ne.y *20 + 1;
//	zscale = ne.z *20 + 1;

	for(long i=0;i< _NUM_SPHERE_VERTICIES_ ;i++){
		sphere[i] = m_sphere[i];

		sphere[i].x += (float)x-.5f;
		sphere[i].y += (float)y-.5f;
		sphere[i].z += (float)z-.5f;
				
		// z - 1.0f so max is 0
		// * 10 to scale back to -30.0f
		// + 0.5f to recentre on 0
		// add to sphere
//		sphere[i].x += (float)x*xscale - xscale + 0.5f;
//		sphere[i].y += (float)y*yscale - yscale + 0.5f;
//		sphere[i].z += (float)z*zscale - zscale + 0.5f;
	}

	// Set the material as yellow. We're setting the ambient color here
    // since this tutorial only uses ambient lighting. For apps that use real
    // s, the diffuse and specular values should be set. (In addition, the
    // polygons' vertices need normals for true lighting.)
    D3DMATERIAL7 mtrl;
    ZeroMemory( &mtrl, sizeof(mtrl) );
    mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = 0.7f;
	mtrl.specular.r = mtrl.specular.g = mtrl.specular.b = 0.5f;
	mtrl.power = 10.0f;
	mtrl.ambient.r = mtrl.diffuse.r = (float)red + 0;
	mtrl.ambient.g = mtrl.diffuse.g = (float)green + 0;
	mtrl.ambient.b = mtrl.diffuse.b = (float)blue + 0;

	if(selected || grouped) {
		mtrl.specular.r = .5f ;
		mtrl.specular.g = .5f ;
		mtrl.specular.b = .5f ;
		mtrl.power = 1.0f;
	} 

	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		g_pd3dDevice->SetMaterial( &mtrl );
//		g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SPECULARMATERIALSOURCE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRENDERSTATE_ANTIALIAS, TRUE);
		g_pd3dDevice->SetRenderState(D3DRENDERSTATE_EDGEANTIALIAS, TRUE);
		g_pd3dDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);
		g_pd3dDevice->SetRenderState(D3DRENDERSTATE_NORMALIZENORMALS, TRUE);

		int segments = _NUM_SPHERE_SEGMENTS_;
		// Draw the wall, composed of a D3DVERTEX-type triangle strip
		//top fan
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, D3DFVF_VERTEX | D3DFVF_NORMAL,
								 sphere, segments*2+2, NULL );
		//bottom fan
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, D3DFVF_VERTEX | D3DFVF_NORMAL,
								 sphere+segments*2+2, segments*2+2, NULL );
		//body strips
		
		for(i=0;i<segments-2;i++){
			g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX | D3DFVF_NORMAL ,
									sphere+segments*4+4 +(segments*2+1)*2*i, (segments*2+1)*2, NULL );
		}

//		D3DVERTEX ring[ 37 ];
//		if(!selected && grouped) {
//			for(long i=0;i<=36;i++){
//				ring[i] = m_ring[i];
//
//				ring[i].x += (float)x-.5f;
//				ring[i].y += (float)y-.5f;
//				ring[i].z += (float)z-.5f;
//			}
//
//			g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX  | D3DFVF_NORMAL, ring, 36, NULL);
//		}
	//}
}



void C3DXView::InitSphere2()
{
	//init m_sphere
	CPatnApp * app = (CPatnApp*)AfxGetApp();
	float ff = (float)app->m_ordobjectsize * 1;
	long pos = 0;
	int i;
	
float segments = _NUM_SPHERE_SEGMENTS_;
	//do top & botton fans
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,.5f,0),Normalize(D3DVECTOR( 0, .5f, 0 )), 0, 0 );	
	for(i=segments*2;i>=0;i--)
	{
		//vertex is 
		float x = (float)(cos(3.1416f/segments*i)*sin(3.1416f/segments)*0.5f);
		float y = (float)(cos(3.1416f/segments)*0.5f);
		float z = (float)(sin(3.1416f/segments*i)*sin(3.1416f/segments)*0.5f);
		m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(x,y,z),Normalize(D3DVECTOR(x,y,z)), 0, 0 );
	}
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,-.5f,0),Normalize(D3DVECTOR( 0, -.5f, 0 )), 0, 0 );	
	for(i=0;i<=segments*2;i++)	
	{
		//vertex is 
		float x = (float)(cos(3.1416/segments*i)*sin(3.1416f/segments)*0.5f);
		float y = (float)(cos(3.1416/segments)*-0.5);
		float z = (float)(sin(3.1416/segments*i)*sin(3.1416f/segments)*0.5f);
		m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(x,y,z),Normalize(D3DVECTOR(x,y,z)), 0, 0 );
	}
	
//(segments-2)*(segments*2+1)*2 + (segments*2+1)*2
	//middle sections 1 to 6
	int s;
	for(s=1;s<segments-1;s++)
	{
		for(i=0;i<=segments*2;i++)
		{
			float x1 = (float)(cos(3.1416/segments*i)*sin(3.1416f/segments*s)*0.5f);
			float y1 = (float)(cos(3.1416/segments*s)*0.5f);			
			float z1 = (float)(sin(3.1416/segments*i)*sin(3.1416f/segments*s)*0.5f);
			
			float x2 = (float)(cos(3.1416/segments*i)*sin(3.1416f/segments*(s+1))*0.5f);
			float y2 = (float)(cos(3.1416/segments*(s+1))*0.5f);
			float z2 = (float)(sin(3.1416/segments*i)*sin(3.1416f/segments*(s+1))*0.5f);

			m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(x2,y2,z2),Normalize(D3DVECTOR(x2,y2,z2)), 0, 0 );			
			m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(x1,y1,z1),Normalize(D3DVECTOR(x1,y1,z1)), 0, 0 );
			
		}	
	}

	/*
	//top fan
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,.5f,0),Normalize(D3DVECTOR( 0, .5f, 0 )), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,.31f,.5f*.74f),Normalize(D3DVECTOR(0,.31f,.5f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.31f*.74f,.31f,.31f*.74f),Normalize(D3DVECTOR(.31f*.74f,.31f,.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.5f*.74f,.31f,0),Normalize(D3DVECTOR(.5f*.74f,.31f,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.31f*.74f,.31f,-.31f*.74f),Normalize(D3DVECTOR(.31f*.74f,.31f,-.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,.31f,-.5f*.74f),Normalize(D3DVECTOR(0,.31f,-.5f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.31f*.74f,.31f,-.31f*.74f),Normalize(D3DVECTOR(-.31f*.74f,.31f,-.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.5f*.74f,.31f,0),Normalize(D3DVECTOR(-.5f*.74f,.31f,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.31f*.74f,.31f,.31f*.74f),Normalize(D3DVECTOR(-.31f*.74f,.31f,.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,.31f,.5f*.74f),Normalize(D3DVECTOR(0,.31f,.5f*.74f)), 0, 0 );
	//bottom fan
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,-.5f,0),Normalize(D3DVECTOR(0,-.5f,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,-.31f,.5f*.74f),Normalize(D3DVECTOR(0,-.31f,.5f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.31f*.74f,-.31f,.31f*.74f),Normalize(D3DVECTOR(-.31f*.74f,-.31f,.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.5f*.74f,-.31f,0),Normalize(D3DVECTOR(-.5f*.74f,-.31f,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.31f*.74f,-.31f,-.31f*.74f),Normalize(D3DVECTOR(-.31f*.74f,-.31f,-.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,-.31f,-.5f*.74f),Normalize(D3DVECTOR(0,-.31f,-.5f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.31f*.74f,-.31f,-.31f*.74f),Normalize(D3DVECTOR(.31f*.74f,-.31f,-.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.5f*.74f,-.31f,0),Normalize(D3DVECTOR(.5f*.74f,-.31f,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.31f*.74f,-.31f,.31f*.74f),Normalize(D3DVECTOR(.31f*.74f,-.31f,.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,-.31f,.5f*.74f),Normalize(D3DVECTOR(0,-.31f,.5f*.74f)), 0, 0 );

	//upper body strip
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,.31f,.5f*.74f),Normalize(D3DVECTOR(0,.31f,.5f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,0,.5f),Normalize(D3DVECTOR(0,0,.5f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.31f*.74f,.31f,.31f*.74f),Normalize(D3DVECTOR(.31f*.74f,.31f,.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.31f,0,.31f),Normalize(D3DVECTOR(.31f,0,.31f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.5f*.74f,.31f,0),Normalize(D3DVECTOR(.5f*.74f,.31f,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.5f,0,0),Normalize(D3DVECTOR(.5f,0,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.31f*.74f,.31f,-.31f*.74f),Normalize(D3DVECTOR(.31f*.74f,.31f,-.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.31f,0,-.31f),Normalize(D3DVECTOR(.31f,0,-.31f)), 0, 0 );
	
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,.31f,-.5f*.74f),Normalize(D3DVECTOR(0,.31f,-.5f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,0,-.5f),Normalize(D3DVECTOR(0,0,-.5f)), 0, 0 );

	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.31f*.74f,.31f,-.31f*.74f),Normalize(D3DVECTOR(-.31f*.74f,.31f,-.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.31f,0,-.31f),Normalize(D3DVECTOR(-.31f,0,-.31f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.5f*.74f,.31f,0),Normalize(D3DVECTOR(-.5f*.74f,.31f,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.5f,0,0),Normalize(D3DVECTOR(-.5f,0,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.31f*.74f,.31f,.31f*.74f),Normalize(D3DVECTOR(-.31f*.74f,.31f,.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.31f,0,.31f),Normalize(D3DVECTOR(-.31f,0,.31f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,.31f,.5f*.74f),Normalize(D3DVECTOR(0,.31f,.5f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,0,.5f),Normalize(D3DVECTOR(0,0,.5f)), 0, 0 );

	//lower body strip
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,0,.5f),Normalize(D3DVECTOR(0,0,.5f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,-.31f,.5f*.74f),Normalize(D3DVECTOR(0,-.31f,.5f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.31f,0,.31f),Normalize(D3DVECTOR(.31f,0,.31f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.31f*.74f,-.31f,.31f*.74f),Normalize(D3DVECTOR(.31f*.74f,-.31f,.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.5f,0,0),Normalize(D3DVECTOR(.5f,0,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.5f*.74f,-.31f,0),Normalize(D3DVECTOR(.5f*.74f,-.31f,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.31f,0,-.31f),Normalize(D3DVECTOR(.31f,0,-.31f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(.31f*.74f,-.31f,-.31f*.74f),Normalize(D3DVECTOR(.31f*.74f,-.31f,-.31f*.74f)), 0, 0 );
	
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,0,-.5f),Normalize(D3DVECTOR(0,0,-.5f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,-.31f,-.5f*.74f),Normalize(D3DVECTOR(0,-.31f,-.5f*.74f)), 0, 0 );

	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.31f,0,-.31f),Normalize(D3DVECTOR(-.31f,0,-.31f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.31f*.74f,-.31f,-.31f*.74f),Normalize(D3DVECTOR(-.31f*.74f,-.31f,-.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.5f,0,0),Normalize(D3DVECTOR(-.5f,0,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.5f*.74f,-.31f,0),Normalize(D3DVECTOR(-.5f*.74f,-.31f,0)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.31f,0,.31f),Normalize(D3DVECTOR(-.31f,0,.31f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(-.31f*.74f,-.31f,.31f*.74f),Normalize(D3DVECTOR(-.31f*.74f,-.31f,.31f*.74f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,0,.5f),Normalize(D3DVECTOR(0,0,.5f)), 0, 0 );
	m_sphere[pos++] = D3DVERTEX( ff * D3DVECTOR(0,-.31f,.5f*.74f),Normalize(D3DVECTOR(0,-.31f,.5f*.74f)), 0, 0 );*/
}


void C3DXView::OnObjectsize()
{
	if(m_legend_instance) return;
	CPatnApp * app = (CPatnApp*)AfxGetApp();	
	CObjectSize dlg;
	dlg.m_objectsize = app->m_ordobjectsize;
	if(dlg.DoModal() == IDOK){
		app->m_ordobjectsize = dlg.m_objectsize;
		InitSphere2();
		InitRing();
		RedrawWindow();
	}
}
void C3DXView::OnPlusobjectsize()
{
	if(m_legend_instance) return;
	CPatnApp * app = (CPatnApp*)AfxGetApp();
	if(app->m_ordobjectsize < 4.994)
		app->m_ordobjectsize += .005;
	InitSphere2();
	InitRing();
	RedrawWindow();

	if(m_legend_view != NULL && m_legend_view != (void*)1) ((CView*)m_legend_view)->RedrawWindow();
}
void C3DXView::OnMinusobjectsize()
{
	if(m_legend_instance) return;
	CPatnApp * app = (CPatnApp*)AfxGetApp();
	if(app->m_ordobjectsize > .006)
		app->m_ordobjectsize -= .005;
	InitSphere2();
	InitRing();
	RedrawWindow();

	if(m_legend_view != NULL && m_legend_view != (void*)1) ((CView*)m_legend_view)->RedrawWindow();
}

void C3DXView::OnUpdatePrintpreview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_legend_instance);
}

void C3DXView::OnUpdatePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_legend_instance);
}

void C3DXView::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_legend_instance);
}

void C3DXView::OnUpdateFilePrintPreview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_legend_instance);
}

void C3DXView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	

	UINT state;

	if(m_legend_instance)
	{
		if(nChar == VK_LEFT)
		{
			m_legendPos -= m_legendNumberOfRows;
			if(m_legendPos < 0)
				m_legendPos = 0;

			RedrawWindow();
		}
		else if(nChar == VK_RIGHT)
		{
			m_legendPos += m_legendNumberOfRows;
			if(m_legendPos < 0)
				m_legendPos = 0;

			RedrawWindow();
		}
		
		return;
	}

	switch(nChar)
	{
			
		case VK_PRIOR:
			OnPlusobjectsize();
			break;		          
		case VK_NEXT:
			OnMinusobjectsize();
			break;
		case VK_ADD:
		case '+':
		case VK_UP:
		case VK_RIGHT:
			OnPlusrotationspeed();
			break;
		case VK_SUBTRACT:				
		case VK_LEFT:
		case VK_DOWN:
		case '_':
		case '-':
			OnMinusrotationspeed();
			break;
		case VK_RETURN:			
		case VK_ESCAPE:
			break;
		case 'z':
		case 'Z':
			OnZoom();
			break;
		case 't':
		case 'T':
			state = m_menu.GetMenuState(ID_GROUP_ANALYSISGROUPSANDCENTROIDS,MF_BYCOMMAND);
			if(state & MF_CHECKED)
				OnGroupApriorigroupsancentroids();
			else
				OnGroupAnalysisgroupsandcentroids();
			RedrawWindow();
			break;
		default:
			break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void C3DXView::DrawToMetaFile(long usefilename,CString filename) //if false, must deletemetafile g_emf after use
{
	POSITION pos;
	CView * pView = NULL;
	pos = m_pDoc->GetFirstViewPosition();
	while(pos){
		pView = m_pDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CTableView)))
			break;
		else
			pView = NULL;
	}

	CDC * pDC = GetDC();
	CRect rr;
	GetClientRect(rr);
	pDC->FillSolidRect(rr,0x00ffffff);
	long iWidthMM = GetDeviceCaps(pDC->GetSafeHdc(), HORZSIZE);
	long iHeightMM = GetDeviceCaps(pDC->GetSafeHdc(), VERTSIZE);
	long iWidthPels = GetDeviceCaps(pDC->GetSafeHdc(), HORZRES);
	long iHeightPels = GetDeviceCaps(pDC->GetSafeHdc(), VERTRES);

	// Use iWidthMM, iWidthPels, iHeightMM, and iHeightPels to determine the
	// number of .01-millimeter units per pixel in the x and y directions.

	long iMMPerPelX = (iWidthMM * 100)/iWidthPels;
	long iMMPerPelY = (iHeightMM * 100)/iHeightPels;

	// Retrieve the coordinates of the client rectangle in pixels.
	// Convert client coordinates to .01-mm units. 
	long width = rr.Width();
	long height = rr.Height();

	rr.left = rr.left * iMMPerPelX;
	rr.top = rr.top * iMMPerPelY;
	rr.right = rr.right * iMMPerPelX;
	rr.bottom = rr.bottom * iMMPerPelY;
	UINT state = m_menu.GetMenuState(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_BYCOMMAND);

	HDC met;
	if(usefilename == 2){
		//no metafile, just draw to pDC
		
		
	}else if(usefilename == 1){
		//met = CreateEnhMetaFile(pDC->GetSafeHdc(),filename,&rr,NULL);
		met = CreateEnhMetaFile(pDC->GetSafeHdc(),filename,NULL,NULL);
	}else{
		//met = CreateEnhMetaFile(pDC->GetSafeHdc(),NULL,&rr,NULL);
		met = CreateEnhMetaFile(pDC->GetSafeHdc(),NULL,NULL,NULL);
	}
	if(usefilename == 2){
		met = pDC->GetSafeHdc();
	}else if(met != NULL){
		pDC = CDC::FromHandle(met);
	}else{
		pDC = GetDC();
		met = pDC->m_hDC;
	}

	{
		//draw a bounding rect for metafile to recognise as the boundary
		if(width-120 < height){
			height = width-120;
		}else{
			width = height+120;
		}
		m_drawres.x = width;
		m_drawres.y = height;
		pDC->SelectStockObject(WHITE_PEN);
		pDC->MoveTo(0,0);
		pDC->LineTo(width,0);
		pDC->LineTo(width,height);
		pDC->SelectStockObject(BLACK_PEN);

		//drawing functions go here
		//draw info & legend
		DrawInfoBar(met);		
		
		if(m_Legend && !m_printimage){
			m_saveimage = true;
			DrawLegend(met);
			m_saveimage = false;
		}

		//attempt to draw axies (also do this at the end)
		if (m_Showranges) { 
			DrawRangesDC(met,0);
		}

		if (m_Mst && !m_Centroid) { 
			DrawMst(met); 
		}

		DrawPcc(met,0);

		SetTextAlign(met,TA_TOP);

		//DrawObjects(hdc);
		{
			double x,y,z;
			float tx,ty,tz;
			CString text;
			double red,green,blue;
			long i,idx,grp,j;
			unsigned long clr;
			//CFont *oldfont,font;
			HFONT oldfont,font;

			double xmov = 0;//.025*cos(wAngleY*3.14/180.0);
			double zmov = -8;//.025*sin(wAngleY*3.14/180.0);
			
			if(!m_Symbols && !m_Labels){			
				//g_pddsBackBuffer->ReleaseDC(hdc);
			}else{
				BYTE quality = m_pDoc->m_lfCells.lfQuality;
				m_pDoc->m_lfCells.lfQuality |= ANTIALIASED_QUALITY;
				font = CreateFontIndirect(&m_pDoc->m_lfCells);
				m_pDoc->m_lfCells.lfQuality = quality;

				oldfont = (HFONT) SelectObject(met,&font);			
			}
			
			CPatnApp * app = (CPatnApp*)AfxGetApp();
			
			pDC->SetBkMode(TRANSPARENT);

			if(m_Centroid) { //m_groupzorder
				for(idx=0;idx<m_groupzorder.GetSize();idx++){	
					if(m_groupzorder.GetSize()-1-idx >= 0){
						i = m_groupzorder[m_groupzorder.GetSize()-1-idx];
						if(i < m_pDoc->m_aGroupColours.GetSize() &&
							i < m_pDoc->m_UserGroupColours.GetSize()){
							if(!m_Usergroupcolours){
								red = m_pDoc->m_aGroupColours[i].red;
								green = m_pDoc->m_aGroupColours[i].green;
								blue = m_pDoc->m_aGroupColours[i].blue;
								if(m_pDoc->m_Ord3d == 2) blue = .5f;
								else if(m_pDoc->m_Ord3d == 1) {blue = 0.5f; green = 0.5f;}
							}else{
								red = m_pDoc->m_UserGroupColours[i].red;
								green = m_pDoc->m_UserGroupColours[i].green;
								blue = m_pDoc->m_UserGroupColours[i].blue;
							}

							x = m_pDoc->m_aGroupColours[i].red;
							y = m_pDoc->m_aGroupColours[i].green;
							z = m_pDoc->m_aGroupColours[i].blue;
							if(m_pDoc->m_Ord3d == 2) z = .5;
							else if(m_pDoc->m_Ord3d == 1) {z=0.5;y=0.5;}

							App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,
									(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);
															
							if(m_Symbols){
								if(i < m_pDoc->m_PatternGroups.GetSize()){
									if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternGroups[i],met,(long)tx-5+m_infowidth,(long)ty-5,45*app->m_ordobjectsize);
									else m_tex.GDIPatterns(m_pDoc->m_PatternGroups[i],met,(long)tx-5+m_infowidth,(long)ty-5,15*app->m_ordobjectsize);
								}
							}else if(m_Labels){
								//code to draw centroid labels, see colour retrieval for coords					
								//m_pDoc->GetRowLabel(i,text);
								if( i < m_pDoc->m_Groups.GetSize())
									text.Format("%d",m_pDoc->m_Groups.GetAt(i));

								red = (long)(red*0xff);
								green = (long)(green*0xff);
								blue = (long)(blue*0xff);
								clr = (unsigned long)(red + 0x100*(green+0x100*blue));
								SetTextColor(met,clr);
								SetBkMode(met,TRANSPARENT);
								//m_pDoc->GetRowLabel(i,text);
								TextOut(met,(long)tx+m_infowidth,(long)ty+(long)zmov,text,text.GetLength());								
							}else{
								//DrawSphere2(x,y,z,red,green,blue);
								//pDC->Ellipse(tx+m_infowidth-tz*15*app->m_ordobjectsize,ty-tz*15*app->m_ordobjectsize,
								//	tx+m_infowidth+tz*15*app->m_ordobjectsize,ty+tz*15*app->m_ordobjectsize);

								CBrush brush;
								red = (long)(red*0xff);
								green = (long)(green*0xff);
								blue = (long)(blue*0xff);
								clr = (unsigned long)(red + 0x100*(green+0x100*blue));
								
								brush.CreateSolidBrush(clr);
								CBrush * oldbrush = pDC->SelectObject(&brush);
								D3DVERTEX v[3];
								App_Corner(g_pd3dDevice,&v[0].x,&v[0].y,&v[0].z,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);
								//App_Corner(g_pd3dDevice,&v[1].x,&v[1].y,&v[1].z,
								//	(float)(x+app->m_ordobjectsize*.5/1.5-.5f),
								//	(float)(y+app->m_ordobjectsize*.5/1.5-.5f),
								//	(float)(z+app->m_ordobjectsize*.5/1.5-.5f));
								double tsr;
								//tsr = sqrt((v[0].x-v[1].x)*(v[0].x-v[1].x) + (v[0].y-v[1].y)*(v[0].y-v[1].y) + (v[0].z-v[1].z)*(v[0].z-v[1].z));
								//tsr = (275-v[0].z) * app->m_ordobjectsize*2.1;
								tsr = (height/2.1-v[0].z) * app->m_ordobjectsize*2.1;
								pDC->Ellipse((long)(tx+m_infowidth-tsr),(long)(ty-tsr),
									(long)(tx+m_infowidth+tsr),(long)(ty+tsr));

								for(long i=0;i<m_PccVectors.GetSize()*2;i+=2){
								//	DrawOverlap(met,0,m_pcc[i].x,m_pcc[i].y,m_pcc[i].z,
								//		m_pcc[i+1].x,m_pcc[i+1].y,m_pcc[i+1].z,
								//		(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale,.2f,height);									
								}								
							}
						}
					}
				}
			} else { //m_objectzorder
				for(idx=0;idx<m_objectzorder.GetSize() && m_objectzorder.GetSize()-1-idx >= 0;idx++){
					i = m_objectzorder[m_objectzorder.GetSize()-1-idx];
					//if statement for selected objects only display
					if(i<m_pDoc->GetNoOfRows() && i < m_pDoc->m_aObjectColours.GetSize()){
						if(!m_SelOnly || (pView != NULL && 
							((CTableView*)pView)->m_pSelectedRows->IsSelected(i))){
							if(m_Grouping){
								if(m_pDoc->GetRowGroups(i) > 0 &&
									m_pDoc->GetRowGroups(i)-1 < m_pDoc->m_aGroupColours.GetSize()){
									red = m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].red;
									green = m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].green;
									blue = m_pDoc->m_aGroupColours[m_pDoc->GetRowGroups(i)-1].blue;
								}
								if(m_pDoc->m_Ord3d == 2) blue = .5;
								else if(m_pDoc->m_Ord3d == 1) {blue=0.5;green=0.5;}
							}else if(m_Usergroupcolours){
								
								if(state & MF_CHECKED && i<m_pDoc->m_AprioriGroups.GetSize() && 
									m_pDoc->m_AprioriGroups.GetAt(i) > 0 && m_pDoc->m_AprioriGroups.GetAt(i) < m_pDoc->m_aGroupColours2.GetSize()){
									
									red = m_pDoc->m_aGroupColours2[m_pDoc->m_AprioriGroups.GetAt(i)-1].red;
									green = m_pDoc->m_aGroupColours2[m_pDoc->m_AprioriGroups.GetAt(i)-1].green;
									blue = m_pDoc->m_aGroupColours2[m_pDoc->m_AprioriGroups.GetAt(i)-1].blue;
								}else if(m_pDoc->GetRowGroups(i) > 0 &&
									m_pDoc->GetRowGroups(i)-1 < m_pDoc->m_UserGroupColours.GetSize()){
									red = m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].red;
									green = m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].green;
									blue = m_pDoc->m_UserGroupColours[m_pDoc->GetRowGroups(i)-1].blue;
								}
							}else if(i < m_pDoc->m_aObjectColours.GetSize()){
								if(m_colColourMap >= 0) {
									red = ((m_colColours[i] & 0xff0000) >> 16)/255.0f;
									green = ((m_colColours[i] & 0x00ff00) >> 8)/255.0f;
									blue = (m_colColours[i] & 0x0000ff)/255.0f;
								} else  {
									red = m_pDoc->m_aObjectColours[i].red;
									green = m_pDoc->m_aObjectColours[i].green;
									blue = m_pDoc->m_aObjectColours[i].blue;					
									if(m_pDoc->m_Ord3d == 2) blue = .5f;
									else if(m_pDoc->m_Ord3d == 1) {blue = 0.5f; green = 0.5f;}
								}
							}
						
							x = m_pDoc->m_aObjectColours[i].red;
							y = m_pDoc->m_aObjectColours[i].green;
							z = m_pDoc->m_aObjectColours[i].blue;
							if(m_pDoc->m_Ord3d == 2) z = .5;
							else if(m_pDoc->m_Ord3d == 1) {z=0.5;y=0.5;}

							App_Corner(g_pd3dDevice,&tx,&ty,&tz,(float)(x-.5f)*x_scale,
								(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);

							if(m_Symbols && !m_Grouping){
								if(i < m_pDoc->m_PatternObjects.GetSize()){
									if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternObjects[i],met,(long)tx-5+m_infowidth,(long)ty-5,45*app->m_ordobjectsize);
									else m_tex.GDIPatterns(m_pDoc->m_PatternObjects[i],met,(long)tx-5+m_infowidth,(long)ty-5,15*app->m_ordobjectsize);
								}
							}else if(m_Symbols && m_Grouping){
								grp = m_pDoc->GetRowGroups(i);
								for(j=0;j<m_pDoc->m_Groups.GetSize();j++){
									if(m_pDoc->m_Groups[j] == grp)
										break;
								}					
								if(j >= 0 && j < m_pDoc->m_PatternGroups.GetSize()){
									if(m_printimage) m_tex.GDIPatterns(m_pDoc->m_PatternGroups[j],met,(long)tx-5+m_infowidth,(long)ty-5,45*app->m_ordobjectsize);
									else m_tex.GDIPatterns(m_pDoc->m_PatternGroups[j],met,(long)tx-5+m_infowidth,(long)ty-5,15*app->m_ordobjectsize);
								}
							}else if(m_Labels){
								red = (long)(red*0xff);
								green = (long)(green*0xff);
								blue = (long)(blue*0xff);
								clr = (unsigned long)(red + 0x100*(green+0x100*blue));
								SetTextColor(met,clr);
								SetBkMode(met,TRANSPARENT);

								m_pDoc->GetRowLabel(i,text);
								TextOut(met,(long)tx+m_infowidth,(long)ty+(long)zmov,text,text.GetLength());
							}else{
								//DrawSphere2(x,y,z,red,green,blue);
								//pDC->Ellipse(tx+m_infowidth-tz*15,ty-tz*15,tx+m_infowidth+tz*15,ty+tz*15);
								CBrush brush;
								red = (long)(red*0xff);
								green = (long)(green*0xff);
								blue = (long)(blue*0xff);
								clr = (unsigned long)(red + 0x100*(green+0x100*blue));
								
								brush.CreateSolidBrush(clr);
								CBrush * oldbrush = pDC->SelectObject(&brush);
								D3DVERTEX v[3];
								App_Corner(g_pd3dDevice,&v[0].x,&v[0].y,&v[0].z,(float)(x-.5f)*x_scale,(float)(y-.5f)*y_scale,(float)(z-.5f)*z_scale);
								//App_Corner(g_pd3dDevice,&v[1].x,&v[1].y,&v[1].z,
								//	(float)(x+app->m_ordobjectsize*.5/1.5-.5f),
								//	(float)(y+app->m_ordobjectsize*.5/1.5-.5f),
								//	(float)(z+app->m_ordobjectsize*.5/1.5-.5f));
								//Un_App_Corner(g_pd3dDevice,&v[1].x,&v[1].y,&v[1].z,
								//	v[0].x+app->m_ordobjectsize,v[0].y,v[0].z);

								double tsr;
								//tsr = sqrt((v[0].x-v[1].x)*(v[0].x-v[1].x) + (v[0].y-v[1].y)*(v[0].y-v[1].y) + (v[0].z-v[1].z)*(v[0].z-v[1].z));
								tsr = (height/2.1-v[0].z) * app->m_ordobjectsize*2.1;
								pDC->Ellipse((long)(tx+m_infowidth-tsr),(long)(ty-tsr),(long)(tx+m_infowidth+tsr),(long)(ty+tsr));

								long i;
								for(i=0;i<m_PccVectors.GetSize()*2;i+=2){
								//	DrawOverlap(met,0,m_pcc[i].x,m_pcc[i].y,m_pcc[i].z,
								//		m_pcc[i+1].x,m_pcc[i+1].y,m_pcc[i+1].z,
								//		(float)(x-.5)*x_scale,(float)(y-.5)*y_scale,(float)(z-.5)*z_scale,.2f,height);									
								}
								for(i=0;m_Mst && i<(m_pDoc->m_aMst.GetSize()-1)*2;i+=2){
								//	DrawOverlap(met,0,m_mst[i].x*x_scale,m_mst[i].y*y_scale,m_mst[i].z*z_scale,
								//		m_mst[i+1].x*x_scale,m_mst[i+1].y*y_scale,m_mst[i+1].z*z_scale,
								//		(float)(x-.5)*x_scale,(float)(y-.5)*y_scale,(float)(z-.5)*z_scale,.02f,height);
								}
								if(oldbrush != NULL){
									pDC->SelectObject(oldbrush);
								}
							}
						} // end of selection if statement
					} // end of for-loop					
				}
			}  // end of if m_Centroid else statement

			if(!m_Symbols && !m_Labels){
				//g_pddsBackBuffer->GetDC(&met);
			}else{
				if(oldfont != NULL)
					SelectObject(met,oldfont);
				DeleteObject(font);
			}
		}
		
	//	DrawPcc(met,1);

		if (m_Showranges) { 
			DrawRangesDC(met,1);
		}
		DrawCompare(met);		

		DrawTextBoxes(met);			
	}
	if(usefilename != 2 && met != NULL){
		g_emf = CloseEnhMetaFile(met);
		if(usefilename){
			DeleteEnhMetaFile(g_emf);
		}
	}
}

void C3DXView::DrawOverlap(HDC hdc,long shape,
						   float c1x,float c1y,float c1z,
						   float c2x,float c2y,float c2z,
						   float sx,float sy,float sz,float sr,long height)
{
	double magc1c2,magsc1,magsc2,theta2,a,mx,my,mz,
		magmc1,magmc2,magsm,magtmc1,magtmc2,magtsm,
		int1x,int1y,int1z,int2x,int2y,int2z,e1x,e1y,e2x,e2y;

	//calc sr
	CPatnApp * app = (CPatnApp*)AfxGetApp();
	D3DVERTEX v[3];
	App_Corner(g_pd3dDevice,&v[0].x,&v[0].y,&v[0].z,sx,sy,sz);
	//App_Corner(g_pd3dDevice,&v[1].x,&v[1].y,&v[1].z,
//		(float)(sx+app->m_ordobjectsize*.5/1.7-.5f),
//		(float)(sy+app->m_ordobjectsize*.5/1.7-.5f),
//		(float)(sz+app->m_ordobjectsize*.5/1.7-.5f));
	double tsr;
//	tsr = sqrt((v[0].x-v[1].x)*(v[0].x-v[1].x) + (v[0].y-v[1].y)*(v[0].y-v[1].y) + (v[0].z-v[1].z)*(v[0].z-v[1].z));
	//tsr = (275-v[0].z) * app->m_ordobjectsize*2.1;
	tsr = (height/2.1-v[0].z) * app->m_ordobjectsize*2.1;
	sr = (float)(app->m_ordobjectsize/3.5);

	magc1c2 = sqrt((c1x-c2x)*(c1x-c2x)+(c1y-c2y)*(c1y-c2y)+(c1z-c2z)*(c1z-c2z));
	magsc1 = sqrt((sx-c1x)*(sx-c1x)+(sy-c1y)*(sy-c1y)+(sz-c1z)*(sz-c1z));
	magsc2 = sqrt((sx-c2x)*(sx-c2x) + (sy-c2y)*(sy-c2y) + (sz-c2z)*(sz-c2z));
	theta2 = (magsc1*magsc1 - magsc2*magsc2 + magc1c2*magc1c2) / (2*magsc1*magc1c2);
	a = theta2 * magsc1;

	mx = (c2x-c1x)/magc1c2 * a + c1x;
	my = (c2y-c1y)/magc1c2 * a + c1y;
	mz = (c2z-c1z)/magc1c2 * a + c1z;

	magmc1 = sqrt((mx-c1x)*(mx-c1x) + (my-c1y)*(my-c1y) + (mz-c1z)*(mz-c1z));
	magmc2 = sqrt((mx-c2x)*(mx-c2x) + (my-c2y)*(my-c2y) + (mz-c2z)*(mz-c2z));
	magsm = sqrt((sx-mx)*(sx-mx) + (sy-my)*(sy-my) + (sz-mz)*(sz-mz));
	
	float tmx,tmy,tmz,tc1x,tc1y,tc1z,tc2x,tc2y,tc2z,tsx,tsy,tsz,tr;
	//build these screen transformed variables
	App_Corner(g_pd3dDevice,&tmx,&tmy,&tmz,(float)mx,(float)mz,(float)my);
	App_Corner(g_pd3dDevice,&tc1x,&tc1y,&tc1z,c1x,c1y,c1z);
	App_Corner(g_pd3dDevice,&tc2x,&tc2y,&tc2z,c2x,c2y,c2z);
	App_Corner(g_pd3dDevice,&tsx,&tsy,&tsz,sx,sy,sz);
	tr = (float)tsr;

	double magtc1c2 = sqrt((tc1x-tc2x)*(tc1x-tc2x)+(tc1y-tc2y)*(tc1y-tc2y));
	double magtsc1 = sqrt((tsx-tc1x)*(tsx-tc1x)+(tsy-tc1y)*(tsy-tc1y));
	double magtsc2 = sqrt((tsx-tc2x)*(tsx-tc2x) + (tsy-tc2y)*(tsy-tc2y));
	theta2 = (magtsc1*magtsc1 - magtsc2*magtsc2 + magtc1c2*magtc1c2) / (2*magtsc1*magtc1c2);
	a = theta2 * magtsc1;
	tmx = (float)((tc2x-tc1x)/magtc1c2 * a + tc1x);
	tmy = (float)((tc2y-tc1y)/magtc1c2 * a + tc1y);
	magtmc1 = sqrt((tmx-tc1x)*(tmx-tc1x)+(tmy-tc1y)*(tmy-tc1y));
	magtmc2 = sqrt((tmx-tc2x)*(tmx-tc2x)+(tmy-tc2y)*(tmy-tc2y));
	magtsm = sqrt((tmx-tsx)*(tmx-tsx)+(tmy-tsy)*(tmy-tsy));
	tr = tr*2;
	a = sqrt(tr*tr-magtsm*magtsm);
	
	e1x = (tc1x-tmx)/magtmc1 * a + tmx;
	e1y = (tc1y-tmy)/magtmc1 * a + tmy;
	e2x = (tc2x-tmx)/magtmc2 * a + tmx;
	e2y = (tc2y-tmy)/magtmc2 * a + tmy;

	a = sqrt(sr*sr - magsm*magsm);
	int1x = (c1x-mx)/magmc1*a + mx;
	int1y = (c1y-my)/magmc1*a+my;
	int1z = (c1z-mz)/magmc1*a+mz;
	int2x = (c2x-mx)/magmc2*a + mx;
	int2y = (c2y-my)/magmc2*a+my;
	int2z = (c2z-mz)/magmc2*a+mz;
	float tint1x,tint1y,tint1z,
		tint2x,tint2y,tint2z;
	App_Corner(g_pd3dDevice,&tint1x,&tint1y,&tint1z,(float)int1x,(float)int1y,(float)int1z);
	App_Corner(g_pd3dDevice,&tint2x,&tint2y,&tint2z,(float)int2x,(float)int2y,(float)int2z);	

	//draw overlap in correct position
	if(tmz > tsz){
		//is behind
		return;
	}else{
		if(magsm > sr){//entirely in front of the object
			MoveToEx(hdc,(long)(e1x+m_infowidth),(long)e1y,NULL);
			LineTo(hdc,(long)(e2x+m_infowidth),(long)e2y);
		}else if(tr > magtsc1 && tr > magtsc2){//both points in the object
			//draw nothing
		}else{//not entirely in front
			if(tint1z < tsz && magsc1 >= sr){
				MoveToEx(hdc,(long)(tint1x+m_infowidth),(long)tint1y,NULL);
				LineTo(hdc,(long)e1x+m_infowidth,(long)e1y);
			}
			if(tint2z < tsz && magsc2 >= sr){
				MoveToEx(hdc,(long)(tint2x+m_infowidth),(long)tint2y,NULL);
				LineTo(hdc,(long)e2x+m_infowidth,(long)e2y);
			}
		}
	}
}

BOOL C3DXView::OnEraseBkgnd(CDC* pDC) 
{

	//return FALSE;

	return CView::OnEraseBkgnd(pDC);
}


void C3DXView::SaveAVI()
{
	if(m_legend_instance) return;

	static long selheight,selcolours,selcoloursbits,selcompression, sNumFrames;

	static double oldx = 0;
	static double oldy = 0;
	static long mrx = m_rotatepoint.x;
	static long mry = m_rotatepoint.y;

	if(g_avi_count == 0){
		m_SaveHeight = m_Rect.bottom;
		m_SaveWidth = m_Rect.right;

		CRect rect;
		GetClientRect(rect);


		CFileDialog* pFileDlg;
		CStdioFile f;
		CFileException fe;
		CString strFileError;
		CString strPathName;
		CString strTitle;

		strTitle = "Save as AVI (one rotation):";
		
		// Get path for the new file
		//pFileDlg = new CFileDialog(FALSE,"bmp","*.bmp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,"(height=480 Colour) Bitmap (*.bmp)|*.bmp|(height=960 Colour) Bitmap (*.bmp)|*.bmp|(height=480 BW) Bitmap (*.bmp)|*.bmp|(height=960 BW) Bitmap (*.bmp)|*.bmp|(height=480 Colour) jpeg (*.jpg)|*.jpg|(height=960 Colour) jpeg (*.jpg)|*.jpg|(height=480 BW) jpeg (*.jpg)|*.jpg|(height=960 BW) jpeg (*.jpg)|*.jpg|All files (*.*)|*.*||",NULL);
		pFileDlg = new CFileDialog(FALSE,"avi","*.avi",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,
			"AVI (height=240) (*.avi)|*.avi|AVI (height=480)(*.avi)|*.avi|AVI (height=960)(*.avi)|*.avi||",NULL);
		pFileDlg->m_ofn.lpstrTitle = strTitle;
		if ( pFileDlg->DoModal() != IDOK ){
			delete pFileDlg;
			return;
		}
		strPathName = pFileDlg->GetPathName();
		
		long fi = pFileDlg->m_ofn.nFilterIndex;
		switch(fi){
		case 2:
			selheight = 480;selcolours = 24;selcoloursbits = 8;selcompression = BI_RGB;
			break;
		case 3:
			selheight = 960;selcolours = 24;selcoloursbits = 8;selcompression = BI_RGB;
			break;
		case 1:
			selheight = 240;selcolours = 24;selcoloursbits = 8;selcompression = BI_RGB;
			break;
		default:
			selheight = 480;selcolours = 24;selcoloursbits = 8;selcompression = BI_RGB;
			break;
		}

		delete pFileDlg;

		//empty file
		FILE * fp = fopen(strPathName,"w");
		if(fp == NULL){
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("invalid file.");
			return;
		}else{
			fclose(fp);
		}


		//Determine number of images
		double radiansx = (m_rotatepoint.x)/-10000.0;
		double radiansy = (m_rotatepoint.y)/-10000.0;

		oldx = m_rotatepoint.x;
		oldy = m_rotatepoint.y;

		long nx = abs((long)(2*g_PI/radiansx));
		long ny = abs((long)(2*g_PI/radiansy));

		//sNumFrames = (nx>ny)?ny:nx;
		sNumFrames = nx;

		//fix up number of frames, ie, no more than 620
		{
			if(sNumFrames > 600 && AfxMessageBox("Would you like the video length reduced to 40 seconds (600 frames) for a single rotation?",MB_YESNO) == IDYES)
			{
				m_rotatepoint.x = (long)(m_rotatepoint.x * nx/600.0);
				m_rotatepoint.y = (long)(m_rotatepoint.y * nx/600.0);

				radiansx = (m_rotatepoint.x)/-10000.0;
				radiansy = (m_rotatepoint.y)/-10000.0;

				nx = abs((long)(2*g_PI/radiansx));
				ny = abs((long)(2*g_PI/radiansy));

				//sNumFrames = (nx>ny)?ny:nx;
				sNumFrames = nx;
			}
			if(sNumFrames < 40)
			{
				m_rotatepoint.x = (long)(m_rotatepoint.x * nx/40.0);
				m_rotatepoint.y = (long)(m_rotatepoint.y * nx/40.0);

				radiansx = (m_rotatepoint.x)/-10000.0;
				radiansy = (m_rotatepoint.y)/-10000.0;

				nx = abs((long)(2*g_PI/radiansx));
				ny = abs((long)(2*g_PI/radiansy));

				//sNumFrames = (nx>ny)?ny:nx;
				sNumFrames = nx;
			}
		}


		/*DON'T ASK FOR USER TO ENTER NUMBER OF FRAMES
		CNumFrames dlg;
		dlg.m_numframes = sNumFrames;
		
		if(dlg.DoModal() == IDCANCEL){
			return;
		}
		sNumFrames = (long)dlg.m_numframes;
		*/


		//Create device context
		HDC dc;
		BITMAPINFO info;
		PBYTE bits;

		long nWidth,nObjects;
		if(m_Centroid || m_Usergroupcolours || m_Grouping)
			nObjects = m_pDoc->m_aGroupColours.GetSize();
		else
			nObjects = m_pDoc->m_aObjectColours.GetSize();
		long nRows = (long)floor(selheight/20.0)-1;
		long nCols = (long)ceil(nObjects/(double)nRows);
		if(selheight %2 == 1) selheight += 1;
		if(m_Legend)
			nWidth = 120 + selheight + 240;//nCols*150			
			//nWidth = selheight * 4/3;
		else
			nWidth = 120 + selheight;
			//nWidth = selheight * 4/3;

		//double factor = 1;
		m_saveimage = true;

		m_drawres.x = selheight*4/3;
		m_drawres.y = selheight;
		
		double factor = 1;

		while(!RenderBySize((int)(nWidth*factor),(int)(selheight*factor)) && factor > 0.1){
			factor -= 0.1;
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Detail of saved image reduced/ndue to insufficient system memory.");
		}
		
		m_saveimage = false;

		LPBITMAPINFOHEADER FAR alpbi;

		// create and bind bitmap
		memset(&info, 0, sizeof(info));
		info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		info.bmiHeader.biWidth = nWidth;
		info.bmiHeader.biHeight = selheight;
		info.bmiHeader.biPlanes = 1;
		info.bmiHeader.biBitCount = 24;
		info.bmiHeader.biCompression = selcompression;
		if(selcompression != BI_RGB)
			info.bmiHeader.biSizeImage = 20000;

		dc = CreateCompatibleDC(NULL);
		
		HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, (void**)&bits, NULL, 0);

		PIXELFORMATDESCRIPTOR pfd;

		HBITMAP oldbitmap = (HBITMAP) SelectObject(dc, bitmap);

		//set pixel format
		memset(&pfd, 0, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_BITMAP | PFD_DEPTH_DONTCARE;
		if(selcolours == 1) pfd.iPixelType = PFD_TYPE_COLORINDEX ;
		else pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cDepthBits = 1;

		long pf = ChoosePixelFormat(dc, &pfd);
		SetPixelFormat(dc, pf, &pfd);

		//DrawScreen and save to bitmap
		m_Rect.bottom = selheight;
		m_Rect.right = nWidth;

		HDC hdc;
		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
			g_pddsBackBuffer->GetDC(&hdc);
		//}else{
		//	hdc = GetDC()->m_hDC;
	//	}
		
		BitBlt(dc,0,0,nWidth,selheight,hdc,0,0,SRCCOPY);

	//	if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		g_pddsBackBuffer->ReleaseDC(hdc);
		//}

		SelectBitmap(dc, oldbitmap);

		alpbi = (LPBITMAPINFOHEADER)GlobalLock(g_wa.MakeDib(bitmap, 24));

		if(!g_wa.BeginNewAVI(strPathName,nWidth,selheight,24,(LPBITMAPINFOHEADER)alpbi)){
			g_wa.EndAVI();
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Cannot create AVI.");

			GlobalFreePtr(alpbi);

			DeleteObject(bitmap);
			DeleteObject(dc) ;

			m_rotatepoint.x = (long)oldx;
			m_rotatepoint.y = (long)oldy;

			this->Cleanup3DEnvironment();
			RedrawWindow();

			if(!m_Rotate) KillTimer(1);
			
			return;
		}			

		//Don't add the first frame.  It appears to be disjointed.
		//g_wa.AddFrame((LPBITMAPINFOHEADER)alpbi);

		GlobalFreePtr(alpbi);

		g_avi_count++;

		DeleteObject(bitmap);
		DeleteObject(dc) ;

		SetTimer(1,1,NULL);
	}else if(g_avi_count < sNumFrames){
		g_avi_count++;
		//Create device context
		HDC dc;
		BITMAPINFO info;
		PBYTE bits;

		long nWidth,nObjects;
		if(m_Centroid || m_Usergroupcolours || m_Grouping)
			nObjects = m_pDoc->m_aGroupColours.GetSize();
		else
			nObjects = m_pDoc->m_aObjectColours.GetSize();
		long nRows = (long)floor(selheight/20.0)-1;
		long nCols = (long)ceil(nObjects/(double)nRows);
		if(m_Legend)
			//nWidth = 120 + selheight + nCols*150;
			nWidth = 360+ selheight;
		else
			nWidth = 120 + selheight;

		m_drawres.x = nWidth;//selheight;
		m_drawres.y = selheight;
		
		//double factor = 1;
		m_saveimage = true;

		m_drawres.x = selheight;
		m_drawres.y = selheight;
		
		double factor = 1;

		while(!RenderBySize((int)(nWidth*factor),(int)(selheight*factor)) && factor > 0.1){
			factor -= 0.1;
			if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Detail of saved image reduced/ndue to insufficient system memory.");
		}
		
		m_saveimage = false;

		LPBITMAPINFOHEADER FAR alpbi;
		// create and bind bitmap
		memset(&info, 0, sizeof(info));
		info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		info.bmiHeader.biWidth = nWidth;
		info.bmiHeader.biHeight = selheight;
		info.bmiHeader.biPlanes = 1;
		info.bmiHeader.biBitCount = 24;
		info.bmiHeader.biCompression = selcompression;
		if(selcompression != BI_RGB)
			info.bmiHeader.biSizeImage = 20000;

		dc = CreateCompatibleDC(NULL);

		HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, (void**)&bits, NULL, 0);

		PIXELFORMATDESCRIPTOR pfd;

		HBITMAP oldbitmap = (HBITMAP) SelectObject(dc, bitmap);

		//set pixel format
		memset(&pfd, 0, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_BITMAP | PFD_DEPTH_DONTCARE;
		if(selcolours == 1) pfd.iPixelType = PFD_TYPE_COLORINDEX ;
		else pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cDepthBits = 1;

		long pf = ChoosePixelFormat(dc, &pfd);
		SetPixelFormat(dc, pf, &pfd);

		//DrawScreen and save to bitmap
		m_Rect.bottom = selheight;
		m_Rect.right = nWidth;

		HDC hdc;
		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
			g_pddsBackBuffer->GetDC(&hdc);
		//}else{
		//	hdc = GetDC()->m_hDC;
	//	}
		
		BitBlt(dc,0,0,nWidth,selheight,hdc,0,0,SRCCOPY);

		//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
			g_pddsBackBuffer->ReleaseDC(hdc);
	//	}

		SelectBitmap(dc, oldbitmap);

		alpbi = (LPBITMAPINFOHEADER)GlobalLock(g_wa.MakeDib(bitmap, 24));

		g_wa.AddFrame((LPBITMAPINFOHEADER)alpbi);

		GlobalFreePtr(alpbi);
		
		DeleteObject(bitmap);
		DeleteObject(dc) ;

		CString txt;
		txt.Format("AVI Frame %d of %d",g_avi_count,sNumFrames);
		((CMainFrame*)AfxGetMainWnd())->m_wndStatusBar.SetWindowText(txt);

	}else{
		g_wa.EndAVI();
		m_rotatepoint.x = (long)oldx;
		m_rotatepoint.y = (long)oldy;

		g_avi_count = 0;
		((CMainFrame*)AfxGetMainWnd())->m_wndStatusBar.SetWindowText("AVI Finished");
		
		if( g_bActive && g_bReady )
		{
			g_bReady = FALSE;

			// Cleanup the environment and recreate everything
			this->Cleanup3DEnvironment();
			if( false && FAILED( CreateEverything( this->GetSafeHwnd()) ) && FAILED( CreateEverything( this->GetSafeHwnd()) ) )
			{
				
				//DestroyWindow();
				CRect r;
				GetWindowRect(r);
				GetDC()->FillSolidRect(r,0x00ffffff);
				GetDC()->TextOut(30,30,"An error occured. Close and reopen the ordination window.");
				GetDC()->TextOut(30,60,"If you are using more than one monitor keep the ordination window on the one screen.");

				g_bReady = TRUE;

				oldx = m_rotatepoint.x;
				oldy = m_rotatepoint.y;

				if(!m_Rotate) KillTimer(1);

				return;
			}
			else
			{				
				g_bReady = TRUE;
				RedrawWindow();
			}
			
			
		}

	}

	
}

void C3DXView::OnSetRotationyz()
{
	if(m_legend_instance) return;

	OnResetRotation();
	m_viewtype = 2;
	D3DMATRIX mat;

    mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
    mat._12 = mat._13 = mat._14 = mat._41 = 0.0f;
    mat._21 = mat._23 = mat._24 = mat._42 = 0.0f;
    mat._31 = mat._32 = mat._34 = mat._43 = 0.0f;
    
    D3DMATRIX matProj = mat;
    
/*	matProj._11 =  2.0f;
	matProj._14 = 1.0f;
    matProj._22 =  2.0f;
	matProj._24 = 1.0f;
	matProj._33 = -2.0f;
    matProj._34 =  -1.0f;
    matProj._44 =  -1.0f;*/
	matProj._33 = .6f;
	matProj._43 = -.80f;
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		g_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );
	//}else{
	//	m_matProj = mat;
	//}

	RedrawWindow();
	UpdateZOrders();
	RedrawWindow();

	float nx,ny,nz;
	App_Corner(g_pd3dDevice,&nx,&ny,&nz,0,0,0);
	App_Corner(g_pd3dDevice,&nx,&ny,&nz,1,1,1);
}
void C3DXView::OnSetRotationxz()
{
	if(m_legend_instance) return;
	OnResetRotation();
	m_viewtype = 3;
	D3DMATRIX mat;

    mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
    mat._12 = mat._13 = mat._14 = mat._41 = 0.0f;
    mat._21 = mat._23 = mat._24 = mat._42 = 0.0f;
    mat._31 = mat._32 = mat._34 = mat._43 = 0.0f;
    
    D3DMATRIX matProj = mat;
    
/*	matProj._11 =  2.0f;
	matProj._14 = 1.0f;
    matProj._22 =  2.0f;
	matProj._24 = 1.0f;
	matProj._33 = -2.0f;
    matProj._34 =  -1.0f;
    matProj._44 =  -1.0f;*/
	matProj._22 = -1.0f;
	matProj._33 = .6f;
	matProj._43 = -.80f;
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		g_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );
//	}else{
	//	m_matProj = mat;
//	}

	RedrawWindow();
	UpdateZOrders();
	RedrawWindow();

	float nx,ny,nz;
	App_Corner(g_pd3dDevice,&nx,&ny,&nz,0,0,0);
	App_Corner(g_pd3dDevice,&nx,&ny,&nz,1,1,1);
}
void C3DXView::OnSetRotationxy()
{
	if(m_legend_instance) return;
	OnResetRotation();
	m_viewtype = 1;

	D3DMATRIX mat;
    mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
    mat._12 = mat._13 = mat._14 = mat._41 = 0.0f;
    mat._21 = mat._23 = mat._24 = mat._42 = 0.0f;
    mat._31 = mat._32 = mat._34 = mat._43 = 0.0f;
    
    D3DMATRIX matProj = mat;
    
/*	matProj._11 =  2.0f;
	matProj._14 = 1.0f;
    matProj._22 =  2.0f;
	matProj._24 = 1.0f;
	matProj._33 = -2.0f;
    matProj._34 =  -1.0f;
    matProj._44 =  -1.0f;*/
	matProj._33 = .6f;
	matProj._43 = -.80f;
	//if(((CPatnApp*)AfxGetApp())->m_ordtype == 1){
		//g_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );
			g_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );
	//}else{
	//	m_matProj = mat;
//	}

	RedrawWindow();
	UpdateZOrders();
	RedrawWindow();

	float nx,ny,nz;
	App_Corner(g_pd3dDevice,&nx,&ny,&nz,0,0,0);
	App_Corner(g_pd3dDevice,&nx,&ny,&nz,1,1,1);
}

void C3DXView::OnGroupAnalysisgroupsandcentroids()
{
	if(m_legend_instance) return;

	//UINT i = m_menu.GetMenuState(ID_GROUP_ANALYSISGROUPSANDCENTROIDS,MF_BYCOMMAND);

	//if(state & MF_CHECKED){
	//	.CheckMenuItem(ID__SHOWSELECTEDONLY,MF_UNCHECKED);
	//}else{
	//	m_SelOnly = true;
	//	m_menu.CheckMenuItem(ID__SHOWSELECTEDONLY,MF_CHECKED);
	//}
	m_menu.CheckMenuItem(ID_GROUP_ANALYSISGROUPSANDCENTROIDS,MF_CHECKED);
	m_menu.CheckMenuItem(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_UNCHECKED);
	m_currentcentroid = -1;
	m_comparecentroid = -1;
		UpdateZOrders();
	RedrawWindow();
}

void C3DXView::OnGroupApriorigroupsancentroids()
{
	if(m_legend_instance) return;
	m_menu.CheckMenuItem(ID_GROUP_ANALYSISGROUPSANDCENTROIDS,MF_UNCHECKED);
	m_menu.CheckMenuItem(ID_GROUP_APRIORIGROUPSANDCENTROIDS,MF_CHECKED);
	m_currentcentroid = -1;
	m_comparecentroid = -1;
		UpdateZOrders();
	RedrawWindow();
}

void C3DXView::OnSymbolsizeDecreasepgdn()
{
	if(m_legend_instance) return;
	
	this->OnMinusobjectsize();
}

void C3DXView::OnSymbolsizeIncreasepgup()
{
	if(m_legend_instance) return;
	this->OnPlusobjectsize();
}

void C3DXView::OnUpdateGroupApriorigroupsandcentroids(CCmdUI *pCmdUI)
{
	if(m_pDoc->m_AprioriGroups.GetSize())
	{
		pCmdUI->Enable();
	}
	else
		pCmdUI->Enable(false);
}

void C3DXView::OnZoom()
{
	if(m_legend_instance) return;
	if(m_zoom){
		m_zoom = 0;		
		m_menu.CheckMenuItem(ID__ZOOM,MF_UNCHECKED);
	}else{
		m_zoom = 1;		
		m_menu.CheckMenuItem(ID__ZOOM,MF_CHECKED);
	}		

	if(m_zoom == 0)
		m_vEyePt = D3DVECTOR( 0, 0, -0.4f);
	else
		m_vEyePt = D3DVECTOR( 0, 0, -0.6f);

	RedrawWindow();
}
void C3DXView::OnUpdateUseusergroupcolours(CCmdUI *pCmdUI)
{
	if(m_Symbols)
	{
		pCmdUI->Enable(false);		
	}
	else
	{
		pCmdUI->Enable(true);		
	}
}

void C3DXView::OnPickacolumn() {
	//get column idx
	CSelColForColours dlg(m_pDoc,m_colColourMap);

	if (dlg.DoModal() == IDCANCEL) {
		return;
	}

	if(m_Grouping) {
		m_Grouping = false;
		m_menu.CheckMenuItem(ID__SHOWGROUPING,MF_UNCHECKED);
	}

	if(m_Usergroupcolours) {
		m_Usergroupcolours = false;	
		m_menu.CheckMenuItem(ID__USEUSERGROUPCOLOURS,MF_UNCHECKED);
	}

	m_menu.CheckMenuItem(ID__PICKACOLUMN,MF_CHECKED);

	long col = dlg.m_sel;
	
	//build colour map
	double min, max;
	min = max = m_pDoc->GetMissingValue();

	long i;
	long rows = (m_pDoc->GetIdxTopExtrinsicRow() == -1)?m_pDoc->GetNoOfRows():m_pDoc->GetIdxTopExtrinsicRow();

	for(i=0;i<rows;i++) {
		if(min == m_pDoc->GetMissingValue() || min > m_pDoc->GetValueAt(col,i)) {
			min = m_pDoc->GetValueAt(col,i);
		}
		if(max == m_pDoc->GetMissingValue() || max < m_pDoc->GetValueAt(col, i)) {
			max = m_pDoc->GetValueAt(col, i);
		}
	}

	int r1 = (dlg.m_colour1 >> 16);
	int g1 = ((dlg.m_colour1 & 0x00ff00) >> 8);
	int b1 = (dlg.m_colour1 & 0x0000ff);

	int r2 = (dlg.m_colour2 >> 16);
	int g2 = ((dlg.m_colour2 & 0x00ff00) >> 8);
	int b2 = (dlg.m_colour2 & 0x0000ff);

	//scale
	m_colColours.SetSize(i);
	for(i=0;i<rows;i++) {
		double v = (m_pDoc->GetValueAt(col, i) - min) / (max - min);
		if(dlg.m_logScale) {
			m_colColours.SetAt(i,getColourFor(log10(v*9 + 1), r1, g1, b1, r2, g2, b2));
		} else {
			m_colColours.SetAt(i,getColourFor(v, r1, g1, b1, r2, g2, b2));
		}
	}

	//apply
	m_colColourMap = col;

	if(m_legend_view != NULL) {
		((C3DXView*)m_legend_view)->m_colColourMap = m_colColourMap;
		((C3DXView*)m_legend_view)->m_colColours.SetSize(m_colColours.GetSize());
		for(long q = 0;q < m_colColours.GetSize();q++) {
			((C3DXView*)m_legend_view)->m_colColours.SetAt(q,m_colColours[q]);
		}
	}

	RedrawWindow();
}

int C3DXView::getColourFor(double d, int r1, int g1, int b1, int r2, int g2, int b2) {
	//range between c1 and c2 for d 0 to 1
	return RGB( (int)(r1 * d + r2 * (1-d)), 
		(int)(g1*d + g2*(1-d)),
		(int)(b1*d + b2*(1-d)));
}

void C3DXView::OnPickacolumnButton()
{
	OnPickacolumn();
}
