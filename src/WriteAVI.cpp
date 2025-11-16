// WriteAVI.cpp: implementation of the CWriteAVI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patn.h"
#include "WriteAVI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWriteAVI::CWriteAVI()
{

}

CWriteAVI::~CWriteAVI()
{

}

/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (C) 1992 - 1997 Microsoft Corporation.  All Rights Reserved.
 *
 **************************************************************************/
/****************************************************************************
 *
 *  WRITEAVI.C
 *
 *  Creates the file OUTPUT.AVI, an AVI file consisting of a rotating clock
 *  face.  This program demonstrates using the functions in AVIFILE.DLL
 *  to make writing AVI files simple.
 *
 *  This is a stripped-down example; a real application would have a user
 *  interface and check for errors.
 *
 ***************************************************************************/



//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
// Our movie is 160x120 and 15 frames long
//
#define AVIIF_KEYFRAME	0x00000010L // this frame is a key frame.

#define BUFSIZE 260

#define LPLPBI	LPBITMAPINFOHEADER *

//----------------------------------------------------------------------------
//
// We don't have a window, we just pop up a dialog
// box, write the file, and quit
//
bool CWriteAVI::BeginNewAVI(CString file_name,int width,int height,int bits,LPBITMAPINFOHEADER first_frame)
{
	AVISTREAMINFO strhdr;
	pfile = NULL;

	ps = NULL;
	psCompressed = NULL;


	AVICOMPRESSOPTIONS opts;
	AVICOMPRESSOPTIONS FAR * aopts[1] = {&opts};
	HRESULT hr;
	WORD wVer;

	/* first let's make sure we are running on 1.1 */
	wVer = HIWORD(VideoForWindowsVersion());
	if (wVer < 0x010a){
		 /* oops, we are too old, blow out of here */
		if(!((CPatnApp*)AfxGetApp())->silent_mode) AfxMessageBox("Video for Windows Version too old.");
		return FALSE;
	}

	AVIFileInit();

	//
	// Open the movie file for writing....
	//	
	hr = AVIFileOpen(&pfile,		    // returned file pointer
		       file_name,		            // file name
		       OF_WRITE | OF_CREATE,	    // mode to open file with
		       NULL);			    // use handler determined
						    // from file extension....
	if (hr != AVIERR_OK)
	{
		AfxMessageBox("Cannot write to this file.");
		return false;
	}

	// Fill in the header for the video stream....

	// The video stream will run in 15ths of a second....

	_fmemset(&strhdr, 0, sizeof(strhdr));
	strhdr.fccType                = streamtypeVIDEO;// stream type
	strhdr.fccHandler             = 0;
	strhdr.dwScale                = 1;
	strhdr.dwRate                 = 15;		    // 15 fps
	strhdr.dwSuggestedBufferSize  = first_frame->biSizeImage;
	SetRect(&strhdr.rcFrame, 0, 0,		    // rectangle for stream
	    (int) first_frame->biWidth,
	    (int) first_frame->biHeight);

	// And create the stream;
	hr = AVIFileCreateStream(pfile,		    // file pointer
			         &ps,		    // returned stream pointer
			         &strhdr);	    // s4tream header
	if (hr != AVIERR_OK) {
		AfxMessageBox("Cannot save to this file.");
		return false;
	}

	_fmemset(&opts, 0, sizeof(opts));
	opts.dwQuality = 8500;
	opts.fccType = streamtypeVIDEO;
	opts.dwFlags = 8;
	//opts.fccHandler = mmioFOURCC('i','v','3','1');
//	opts.fccHandler = mmioFOURCC('m','s','v','c');
	opts.fccHandler = 1668707181; //(ms video 1)
	if (!AVISaveOptions(NULL, ICMF_CHOOSE_DATARATE | ICMF_CHOOSE_KEYFRAME , 1, &ps, (LPAVICOMPRESSOPTIONS FAR *) &aopts))
	    return false;

	hr = AVIMakeCompressedStream(&psCompressed, ps, &opts, NULL);
	if(hr == AVIERR_OK) {
		hr = AVIStreamSetFormat(psCompressed, 0,
			       first_frame,	    // stream format
			       first_frame->biSize + 
				   first_frame->biClrUsed*sizeof(RGBQUAD));
	}
	while (hr != AVIERR_OK && AfxMessageBox("Error occured.  Would you like to try another codec.",MB_YESNO) == IDYES) {
		opts.fccHandler = 0;
		opts.dwQuality = 0;
		opts.dwFlags = 0;
		opts.fccType = 0;
		
		ps = NULL;
		// And create the stream;
		hr = AVIFileCreateStream(pfile,		    // file pointer
						&ps,		    // returned stream pointer
						&strhdr);	    // stream header
		if (hr == AVIERR_OK) {
			
			if (!AVISaveOptions(NULL, ICMF_CHOOSE_DATARATE | ICMF_CHOOSE_KEYFRAME , 1, &ps, (LPAVICOMPRESSOPTIONS FAR *) &aopts))
				return false;

			hr = AVIMakeCompressedStream(&psCompressed, ps, &opts, NULL);
			if (hr == AVIERR_OK) {
				hr = AVIStreamSetFormat(psCompressed, 0,
					first_frame,	    // stream format
					first_frame->biSize + 
					first_frame->biClrUsed*sizeof(RGBQUAD));
			}
		}
	}

	if(hr != AVIERR_OK)
	{
		return false;
	}

	m_frame_count = 0;
	

	return true;//AddFrame(first_frame);
}

bool CWriteAVI::AddFrame(LPBITMAPINFOHEADER frame)
{
	//
	// Now write out each video frame, along with a text label.
	// The video frames are 2/3 of a second apart, which is 10
	// in the video time scale and 40 in the text stream's time scale.
	//
	
	HRESULT hr = AVIStreamWrite(psCompressed,	// stream pointer
		m_frame_count,				// time of this frame
		1,				// number to write
		(LPBYTE) frame +		// pointer to data
			frame->biSize +
			frame->biClrUsed * sizeof(RGBQUAD),
			frame->biSizeImage,	// size of this frame
		AVIIF_KEYFRAME,			 // flags....
		NULL,
		NULL);

	if (hr != AVIERR_OK)
		return false;
	
	
	m_frame_count++;

	return true;	
}


bool CWriteAVI::EndAVI()
{
	//
	// Now close the file
	//
	if (ps)
		AVIStreamClose(ps);

	if (psCompressed)
		AVIStreamClose(psCompressed);

	if (pfile)
		AVIFileClose(pfile);

	AVIFileExit();

	return true;
}

HANDLE  CWriteAVI::MakeDib( HBITMAP hbitmap, UINT bits )
{
	HANDLE              hdib ;
	HDC                 hdc ;
	BITMAP              bitmap ;
	UINT                wLineLen ;
	DWORD               dwSize ;
	DWORD               wColSize ;
	LPBITMAPINFOHEADER  lpbi ;
	LPBYTE              lpBits ;
	
	GetObject(hbitmap,sizeof(BITMAP),&bitmap) ;

	//
	// DWORD align the width of the DIB
	// Figure out the size of the colour table
	// Calculate the size of the DIB
	//
	wLineLen = (bitmap.bmWidth*bits+31)/32 * 4;
	wColSize = sizeof(RGBQUAD)*((bits <= 8) ? 1<<bits : 0);
	dwSize = sizeof(BITMAPINFOHEADER) + wColSize +
		(DWORD)(UINT)wLineLen*(DWORD)(UINT)bitmap.bmHeight;

	//
	// Allocate room for a DIB and set the LPBI fields
	//
	hdib = GlobalAlloc(GHND,dwSize);
	if (!hdib)
		return hdib ;

	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib) ;

	lpbi->biSize = sizeof(BITMAPINFOHEADER) ;
	lpbi->biWidth = bitmap.bmWidth ;
	lpbi->biHeight = bitmap.bmHeight ;
	lpbi->biPlanes = 1 ;
	lpbi->biBitCount = (WORD) bits ;
	lpbi->biCompression = BI_RGB ;
	lpbi->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize ;
	lpbi->biXPelsPerMeter = 0 ;
	lpbi->biYPelsPerMeter = 0 ;
	lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;
	lpbi->biClrImportant = 0 ;

	//
	// Get the bits from the bitmap and stuff them after the LPBI
	//
	lpBits = (LPBYTE)(lpbi+1)+wColSize ;

	hdc = CreateCompatibleDC(NULL) ;

	GetDIBits(hdc,hbitmap,0,bitmap.bmHeight,lpBits,(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

	// Fix this if GetDIBits messed it up....
	lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;

	DeleteDC(hdc) ;
	GlobalUnlock(hdib);

	return hdib ;
}