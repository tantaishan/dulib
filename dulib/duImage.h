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
	virtual UINT GetType() { return DU_RES_IMAGE; }
	virtual BOOL OnCreate(TiXmlElement *pElement);

	virtual BOOL CreateImage(int nWidth, int nHeight);
	virtual BOOL LoadFromFile(LPCTSTR lpszFile);
	virtual BOOL LoadFromMemory(PBYTE pData, int nSize);
	virtual BOOL LoadFromStream(IStream *pStream);

	virtual PBYTE GetBits();
	virtual HBITMAP GetBitmap();
	virtual int GetWidth();
	virtual int GetHeight();
	virtual int GetFrameCount();
	virtual HBITMAP GetFrameBitmap(int nIndex);
	virtual PBYTE GetFrameBits(int nIndex);
	virtual UINT GetFrameDelay(int nIndex);
	
	virtual BOOL BeginHSLConvert();
	virtual BOOL AdjustHSL(int nHua, int nPerSat, int nPerLum);
	virtual void EndHSLConvert();

	virtual LPCTSTR GetValue();
	virtual void SetValue(LPCTSTR strImagePath);
	
	virtual duImage *CreateGrayImage();
	virtual duImage *CreateStretchSmooth(int nWidth, int nHeight);
	virtual BOOL LoadFromHICON(HICON hIcon);
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duImage"); }

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
