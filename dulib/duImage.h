//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duImage.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duResBase.h"
#include "duResManager.h"

typedef struct tagCOLORHSL
{
	BYTE hue;
	BYTE saturation;
	BYTE luminance;
}COLORHSL;


#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)
// duImage
//
//
class duImage : public duResBase
{
public:
	virtual UINT WINAPI GetType() { return DU_RES_IMAGE; }
	virtual BOOL WINAPI OnCreate(TiXmlElement *pElement);

	virtual BOOL WINAPI CreateImage(int nWidth, int nHeight);
	virtual BOOL WINAPI LoadFromFile(LPCTSTR lpszFile);
	virtual BOOL WINAPI LoadFromMemory(PBYTE pData, int nSize);
	virtual BOOL WINAPI LoadFromStream(IStream *pStream);

	virtual PBYTE WINAPI GetBits();
	virtual HBITMAP WINAPI GetBitmap();
	virtual int WINAPI GetWidth();
	virtual int WINAPI GetHeight();
	virtual int WINAPI GetFrameCount();
	virtual HBITMAP WINAPI GetFrameBitmap(int nIndex);
	virtual PBYTE WINAPI GetFrameBits(int nIndex);
	virtual UINT WINAPI GetFrameDelay(int nIndex);
	
	virtual BOOL WINAPI BeginHSLConvert();
	virtual BOOL WINAPI AdjustHSL(int nHua, int nPerSat, int nPerLum);
	virtual void WINAPI EndHSLConvert();

	virtual LPCTSTR WINAPI GetValue();
	virtual void WINAPI SetValue(LPCTSTR strImagePath);
	
	virtual duImage *WINAPI CreateGrayImage();
	virtual duImage *WINAPI CreateStretchSmooth(int nWidth, int nHeight);
	virtual BOOL WINAPI LoadFromHICON(HICON hIcon);
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duImage"); }

public:
	duImage();
	~duImage();
	
protected:
	void Destroy();
	void LoadFrame(HDC hDC, Image *pImage, HBITMAP &hBitmap, PBYTE &pBits);
	inline void RGB2HSL(BYTE r, BYTE g, BYTE b, BYTE *h, BYTE *s, BYTE *l);
	inline void HSL2RGB(WORD h, WORD s, WORD l, BYTE *r, BYTE *g, BYTE *b);
	inline BYTE Hue2RGB(WORD n1, WORD n2, WORD hue);
	
protected:
	HBITMAP m_hBitmap;
	PBYTE m_pBits;
	int m_nWidth;
	int m_nHeight;
	TCHAR m_strImage[MAX_NAME];
	int m_nFrameCount;
	HBITMAP *m_phFrameBitmap;
	PBYTE *m_pFrameBits;
	UINT *m_puFrameDelay;
	COLORHSL *m_pHSL;
};
