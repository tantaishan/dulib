//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duImage.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "duImage.h"
#include "duWindowManager.h"

#define HSLMAX    255
#define RGBMAX    255
#define UNDEFINED    (HSLMAX*2/3)

#define max_t(a,b,c)    ((a > ((b>c)?b:c)) ? a : ((b>c)?b:c))
#define min_t(a,b,c)    ((a < ((b<c)?b:c)) ? a : ((b<c)?b:c))

duImage::duImage() :
	 m_hBitmap(NULL)
	,m_pBits(NULL)
	,m_nWidth(0)
	,m_nHeight(0)
	,m_nFrameCount(0)
	,m_phFrameBitmap(NULL)
	,m_pFrameBits(NULL)
	,m_puFrameDelay(NULL)
	,m_pHSL(NULL)
{
	ZeroMemory(m_strImage, MAX_NAME * sizeof(TCHAR));
}

duImage::~duImage()
{
	//_TRACE(_T("~duImage name[%s]\n"), GetName());
	Destroy();
}

BOOL duImage::OnCreate(TiXmlElement *pElement)
{
	if (pElement == NULL)
		return FALSE;

	ReadXmlAttributeText(pElement, _T("value"), m_strImage, MAX_NAME);

	duWindowManager *pWinManager = GetResManager()->GetWindowManager();
	if (pWinManager)
	{
		IStream *pStream = NULL;
		HANDLE hMem = NULL;
		pWinManager->GetResource(m_strImage, hMem, pStream);
		if (!LoadFromStream(pStream))
		{
			_TRACE(_T("failed to loading image[%s]\n"), m_strImage);
		}
		pWinManager->ReleaseResource(hMem, pStream);
	}
	
	return duResBase::OnCreate(pElement);
}

void duImage::Destroy()
{
	if (m_nFrameCount > 1)
	{
		int i;
		for (i = 0;i < m_nFrameCount; i++)
		{
			HBITMAP hBitmap = m_phFrameBitmap[i];
			//_TRACE(_T("delete bitmap[%x]\n"), hBitmap);
			::DeleteObject(hBitmap);
		}
		
		delete [] m_phFrameBitmap;
		delete [] m_pFrameBits;
		delete [] m_puFrameDelay;
		m_phFrameBitmap = NULL;
		m_pFrameBits = NULL;
		m_puFrameDelay = NULL;
		m_nFrameCount = 0;
		m_hBitmap = NULL;
		m_pBits = NULL;
		m_nWidth = 0;
		m_nHeight = 0;
	}
	else
	{
		if (m_hBitmap)
		{
			::DeleteObject(m_hBitmap);
			m_hBitmap = NULL;
		}
		m_pBits = NULL;
		m_nWidth = 0;
		m_nHeight = 0;
	}
	
	if (m_pHSL)
	{
		delete [] m_pHSL;
		m_pHSL = NULL;
	}
}

BOOL duImage::CreateImage(int nWidth, int nHeight)
{
	if (nWidth <= 0 || nHeight <= 0)
		return FALSE;

	Destroy();

	HDC hMemDC = ::CreateCompatibleDC(NULL);
	HBITMAP hOldBmp = NULL;
	BITMAPINFO bmi;

	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biHeight   = nHeight * -1; // 负值,像素从第一行开始
	bmi.bmiHeader.biWidth    = nWidth;
	bmi.bmiHeader.biPlanes   = 1;

	m_nWidth  = nWidth;
	m_nHeight = nHeight;
	m_hBitmap = ::CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0);

	if (m_hBitmap == NULL || m_pBits == NULL)
	{
		::DeleteDC(hMemDC);
		return FALSE;
	}

	::SelectObject(hMemDC, hOldBmp);
	::DeleteDC(hMemDC);
	return TRUE;
}

BOOL duImage::LoadFromFile(LPCTSTR lpszFile)
{
	if (lpszFile == NULL || *lpszFile == 0)
		return FALSE;

	Destroy();

	HANDLE hFile = ::CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_DELETE|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE || hFile == NULL)
		return FALSE;

	DWORD dwFileSize = ::GetFileSize(hFile, NULL);
	if (dwFileSize > 0)
	{
		HANDLE hGlobalMem = ::GlobalAlloc(GMEM_FIXED, dwFileSize);
		IStream *pStream = NULL;
		::CreateStreamOnHGlobal(hGlobalMem, FALSE, &pStream);
		if (pStream == NULL)
		{
			::CloseHandle(hFile);
			::GlobalFree(hGlobalMem);
			return FALSE;
		}

		BYTE *pGlobalData = (BYTE *)::GlobalLock(hGlobalMem);
		DWORD dwByteRead = 0;
		if (::ReadFile(hFile, pGlobalData, dwFileSize, &dwByteRead, NULL) && LoadFromStream(pStream))
		{
			::CloseHandle(hFile);
			pStream->Release();
			::GlobalUnlock(hGlobalMem);
			::GlobalFree(hGlobalMem);
			return TRUE;
		}

		::CloseHandle(hFile);
		pStream->Release();
		::GlobalUnlock(hGlobalMem);
		::GlobalFree(hGlobalMem);
		return FALSE;
	}

	::CloseHandle(hFile);
	return FALSE;
}

BOOL duImage::LoadFromMemory(PBYTE pData, int nSize)
{
	if (pData == NULL || nSize <= 0)
		return FALSE;

	Destroy();

	HANDLE hGlobalMem = ::GlobalAlloc(GMEM_FIXED, nSize);
	IStream *pStream = NULL;
	::CreateStreamOnHGlobal(hGlobalMem, FALSE, &pStream);
	if (pStream == NULL)
	{
		::GlobalFree(hGlobalMem);
		return FALSE;
	}

	BYTE *pGlobalData = (BYTE *)::GlobalLock(hGlobalMem);
	memcpy(pGlobalData, pData, nSize);
	if (LoadFromStream(pStream))
	{
		pStream->Release();
		::GlobalUnlock(hGlobalMem);
		::GlobalFree(hGlobalMem);
		return TRUE;
	}

	pStream->Release();
	::GlobalUnlock(hGlobalMem);
	::GlobalFree(hGlobalMem);
	return FALSE;
}

BOOL duImage::LoadFromStream(IStream *pStream)
{
	if (pStream == NULL)
		return FALSE;

	Destroy();

	Image *pImage = Image::FromStream(pStream, FALSE);
	if (pImage == NULL)
		return FALSE;

	HDC hMemDC = ::CreateCompatibleDC(NULL);
	m_nWidth  = pImage->GetWidth();
	m_nHeight = pImage->GetHeight();
		
	// Gif
	UINT nDimensionsCount = pImage->GetFrameDimensionsCount();
	GUID *pDimensionIDs = (GUID *)malloc(sizeof(GUID) * nDimensionsCount);
	pImage->GetFrameDimensionsList(pDimensionIDs, nDimensionsCount);
	
	WCHAR strGuid[39];
	StringFromGUID2(pDimensionIDs[0], strGuid, 39);
	
	m_nFrameCount = pImage->GetFrameCount(&pDimensionIDs[0]);
	free(pDimensionIDs);
	
	if (m_nFrameCount > 1)
	{
		UINT nTotalBuffer = pImage->GetPropertyItemSize(PropertyTagFrameDelay);
		PropertyItem *pItem = (PropertyItem*)malloc(nTotalBuffer);
		pImage->GetPropertyItem(PropertyTagFrameDelay, nTotalBuffer, pItem);
		
		m_phFrameBitmap = new HBITMAP[m_nFrameCount];
		m_pFrameBits = new PBYTE[m_nFrameCount];
		m_puFrameDelay = new UINT[m_nFrameCount];
		
		int i;
		GUID guidFrame = FrameDimensionTime;
		for (i = 0;i < m_nFrameCount; i++)
		{
			m_puFrameDelay[i] = ((UINT*)pItem[0].value)[i] * 10;
			pImage->SelectActiveFrame(&guidFrame, i);
			LoadFrame(hMemDC, pImage, m_phFrameBitmap[i], m_pFrameBits[i]);
		}
		
		m_hBitmap = m_phFrameBitmap[0];
		m_pBits = m_pFrameBits[0];
		free(pItem);
		
		//for (i = 0;i < m_nFrameCount; i++)
		//	_TRACE(_T("delay[%d] bitmap[%x] bits[%x]\n"), m_puFrameDelay[i], m_phFrameBitmap[i], m_pFrameBits[i]);
	}
	else
	{
		HBITMAP hOldBmp = NULL;
		BITMAPINFO bmi;

		ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biWidth    = pImage->GetWidth();
		bmi.bmiHeader.biHeight   = pImage->GetHeight() * -1; // 负值,像素从第一行开始
		bmi.bmiHeader.biPlanes   = 1;

		m_hBitmap = ::CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0);

		if (m_hBitmap == NULL || m_pBits == NULL)
		{
			delete pImage;
			::DeleteDC(hMemDC);
			return FALSE;
		}

		hOldBmp = (HBITMAP)::SelectObject(hMemDC, m_hBitmap);
		Graphics gs(hMemDC);
		gs.DrawImage(pImage, 0, 0, m_nWidth, m_nHeight);
		::SelectObject(hMemDC, hOldBmp);
	}
	
	delete pImage;
	::DeleteDC(hMemDC);

	::SetProcessWorkingSetSize(::GetCurrentProcess(), -1, -1);
	return TRUE;
}

BOOL duImage::LoadFromHICON(HICON hIcon)
{
	if (hIcon == NULL)
		return FALSE;

	ICONINFO iif;
	memset(&iif, 0, sizeof(ICONINFO));
	GetIconInfo(hIcon, &iif);

	BITMAP bm;
	memset(&bm, 0, sizeof(BITMAP));
	GetObject(iif.hbmColor, sizeof(BITMAP), &bm);

	CreateImage(bm.bmWidth, bm.bmHeight);
	if (m_hBitmap == NULL)
		return FALSE;

	Bitmap temp(iif.hbmColor, NULL);
	BitmapData lockedBitmapData;
	Rect rc(0, 0, temp.GetWidth(), temp.GetHeight());
	temp.LockBits(&rc, Gdiplus::ImageLockModeRead, temp.GetPixelFormat(), &lockedBitmapData);

	Bitmap image(lockedBitmapData.Width, lockedBitmapData.Height, lockedBitmapData.Stride,
		PixelFormat32bppARGB, reinterpret_cast<BYTE *>(lockedBitmapData.Scan0));

	temp.UnlockBits(&lockedBitmapData);

	HDC hDC = ::CreateCompatibleDC(NULL);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDC, m_hBitmap);
	
	Graphics gs(hDC);
	
	gs.DrawImage(&image, 0, 0, bm.bmWidth, bm.bmHeight);

	::SelectObject(hDC, hOldBitmap);
	::DeleteDC(hDC);

	return TRUE;
}

void duImage::LoadFrame(HDC hDC, Image *pImage, HBITMAP &hBitmap, PBYTE &pBits)
{
	HBITMAP hOldBmp = NULL;
	BITMAPINFO bmi;

	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biWidth    = m_nWidth;
	bmi.bmiHeader.biHeight   = m_nHeight * -1; // 负值,像素从第一行开始
	bmi.bmiHeader.biPlanes   = 1;

	hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void **)&pBits, NULL, 0);
	hOldBmp = (HBITMAP)::SelectObject(hDC, hBitmap);
	Graphics gs(hDC);
	gs.DrawImage(pImage, 0, 0, m_nWidth, m_nHeight);
	::SelectObject(hDC, hOldBmp);
}

PBYTE duImage::GetBits()
{
	return m_pBits;
}

HBITMAP duImage::GetBitmap()
{
	return m_hBitmap;
}

int duImage::GetWidth()
{
	return m_nWidth;
}

int duImage::GetHeight()
{
	return m_nHeight;
}

int duImage::GetFrameCount()
{
	return m_nFrameCount;
}

HBITMAP duImage::GetFrameBitmap(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nFrameCount)
		return m_phFrameBitmap[nIndex];
		
	return NULL;
}

PBYTE duImage::GetFrameBits(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nFrameCount)
		return m_pFrameBits[nIndex];
		
	return NULL;
}

UINT duImage::GetFrameDelay(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nFrameCount)
		return m_puFrameDelay[nIndex];
	
	return 0;
}

BOOL duImage::BeginHSLConvert()
{
	if (m_pBits == NULL)
		return FALSE;
	
	if (m_pHSL)
		return TRUE;
	
	m_pHSL = new COLORHSL[m_nWidth * m_nHeight];
	COLORHSL *pHSL = m_pHSL;
	RGBQUAD *pBits = (RGBQUAD *)m_pBits;
	
	int x,y;
	for (y = 0;y < m_nHeight; y++)
	{
		for (x = 0;x < m_nWidth; x++)
		{
			RGB2HSL(pBits->rgbRed, pBits->rgbGreen, pBits->rgbBlue, &pHSL->hue, &pHSL->saturation, &pHSL->luminance);
			pHSL++;
			pBits++;
		}
	}

	return TRUE;
}

BOOL duImage::AdjustHSL(int nHua, int nPerSat, int nPerLum)
{
	if (m_pHSL == NULL)
		return FALSE;
	
	WORD h, s, l;
	COLORHSL *pHSL = m_pHSL;
	RGBQUAD *pBits = (RGBQUAD *)m_pBits;
	
	int x,y;
	for (y = 0;y < m_nHeight; y++)
	{
		for (x = 0;x < m_nWidth; x++)
		{
			h = pHSL->hue;
			s = pHSL->saturation;
			l = pHSL->luminance;
			
			h += nHua;
			if (nPerSat > 0)
				s = min(s + nPerSat, 255);
			else
				s = max(s + nPerSat, 0);

			if (nPerLum > 0)
				l = min(l + nPerLum, 255);
			else
				l = max(l + nPerLum, 0);

			HSL2RGB(h, s, l, &pBits->rgbRed, &pBits->rgbGreen, &pBits->rgbBlue);
			
			pBits->rgbRed = min(max(pBits->rgbRed, 0), 255);
			pBits->rgbGreen = min(max(pBits->rgbGreen, 0), 255);
			pBits->rgbBlue = min(max(pBits->rgbBlue, 0), 255);

			pHSL++;
			pBits++;
		}
	}

	return TRUE;
}

void duImage::EndHSLConvert()
{
}

inline void duImage::RGB2HSL(BYTE r, BYTE g, BYTE b, BYTE *h, BYTE *s, BYTE *l)
{	
	BYTE cMax, cMin;
	WORD Rdelta, Gdelta, Bdelta;

	cMax = max_t(r, g, b);
	cMin = min_t(r, g, b);
	*l = (((cMax+cMin)*HSLMAX)+RGBMAX) / (2*RGBMAX);

	if (cMax == cMin)
	{
		*s = 0;
		*h = UNDEFINED;
	}
	else
	{
		if (*l <= (HSLMAX/2))
			*s = (((cMax-cMin)*HSLMAX)+((cMax+cMin)/2)) / (cMax+cMin);
		else
			*s = (((cMax-cMin)*HSLMAX)+((2*RGBMAX-cMax-cMin)/2)) / (2*RGBMAX-cMax-cMin);

		Rdelta = (((cMax-r)*(HSLMAX/6))+((cMax-cMin)/2)) / (cMax-cMin);
		Gdelta = (((cMax-g)*(HSLMAX/6))+((cMax-cMin)/2)) / (cMax-cMin);
		Bdelta = (((cMax-b)*(HSLMAX/6))+((cMax-cMin)/2)) / (cMax-cMin);

		if (r == cMax)
			*h = Bdelta - Gdelta;
		else if (g == cMax)
			*h = (HSLMAX/3) + Rdelta - Bdelta;
		else
			*h = ((2*HSLMAX)/3) + Gdelta - Rdelta;

		if (*h < 0)
			*h += HSLMAX;
		if (*h > HSLMAX)
			*h -= HSLMAX;
	}
}

inline void duImage::HSL2RGB(WORD h, WORD s, WORD l, BYTE *r, BYTE *g, BYTE *b)
{
	WORD Magic1, Magic2;
	if (0 == s)
		*r = *g = *b = (l*RGBMAX) / HSLMAX;
	else
	{
		if (l <= (HSLMAX/2))
			Magic2 = (l*(HSLMAX+s)+(HSLMAX/2)) / HSLMAX;
		else
			Magic2 = l + s - ((l*s)+(HSLMAX/2))/HSLMAX;

		Magic1 = 2*l - Magic2;

		*r = (Hue2RGB(Magic1, Magic2, h+(HSLMAX/3))*RGBMAX+(HSLMAX/2)) / HSLMAX;
		*g = (Hue2RGB(Magic1, Magic2, h)*RGBMAX+(HSLMAX/2)) / HSLMAX;
		*b = (Hue2RGB(Magic1, Magic2, h-(HSLMAX/3))*RGBMAX+(HSLMAX/2)) / HSLMAX;
	}
}

inline BYTE duImage::Hue2RGB(WORD n1, WORD n2, WORD hue)
{	
	if (hue < 0)
		hue += HSLMAX;
	if (hue > HSLMAX)
		hue -= HSLMAX;

	if (hue < (HSLMAX/6))
		return (n1 + (((n2-n1)*hue+(HSLMAX/12))/(HSLMAX/6)));
	if (hue < (HSLMAX/2))
		return (n2);
	if (hue < ((HSLMAX*2)/3))
		return (n1 + (((n2-n1)*(((HSLMAX*2)/3)-hue)+(HSLMAX/12))/(HSLMAX/6)));
	else
		return (n1);
}

LPCTSTR duImage::GetValue()
{
	return m_strImage;
}

void duImage::SetValue(LPCTSTR strImagePath)
{
	if(strImagePath)
	{
		_tcsncpy(m_strImage, strImagePath, MAX_NAME);
	}
	else
	{
		ZeroMemory(m_strImage, sizeof(TCHAR));
	}
}


//返回新建灰色图片指针.当此灰色图片不再被使用时,应调用duImage::FinalRelease函数将其销毁.
duImage *duImage::CreateGrayImage()
{
	duResManager *pResManager = GetResManager();
	if (pResManager)
	{
		dustring strTempName = GetTempName();
		duImage *pGrayImage = (duImage *)pResManager->CreateResObj(strTempName.c_str(), DU_RES_IMAGE);
		if (pGrayImage)
		{
			BOOL bRet = pGrayImage->CreateImage(m_nWidth, m_nHeight);
			if(!bRet)
			{
				pGrayImage->FinalRelease();
				return NULL;
			}

			memcpy(pGrayImage->GetBits(), this->GetBits(), m_nWidth * m_nHeight * 4);
		
			BYTE *pBits = pGrayImage->GetBits();
			BYTE gray;

			RGBQUAD *pNewBits = (RGBQUAD *)pBits;

			int x,y;
			for (y = 0;y < m_nHeight; y++)
			{
				for (x = 0;x < m_nWidth; x++)
				{
					gray = RGB2GRAY(pNewBits->rgbRed, pNewBits->rgbGreen, pNewBits->rgbBlue);
					pNewBits->rgbRed = pNewBits->rgbGreen = pNewBits->rgbBlue = gray;
					pNewBits++;
				}
			}

			return pGrayImage;
		}
	}
	return NULL;
}

duImage *duImage::CreateStretchSmooth(int nWidth, int nHeight)
{
	//
	// TODO: 应用于Gif图像
	//
	if (nWidth <= 0 || nHeight <= 0)
		return NULL;

	dustring strTempName = GetTempName();
	duImage *pStretchImage = (duImage *)GetResManager()->CreateResObj(strTempName.c_str(), DU_RES_IMAGE);
	if (pStretchImage == NULL)
		return NULL;

	if (!pStretchImage->CreateImage(nWidth, nHeight))
	{
		pStretchImage->FinalRelease();
		return NULL;
	}

	int nPitchSrc = (((m_nWidth * 32) + 31) / 32) * 4;
	Bitmap bitmap(m_nWidth, m_nHeight, nPitchSrc, PixelFormat32bppARGB, m_pBits);

	HDC hMemDC = ::CreateCompatibleDC(NULL);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, pStretchImage->GetBitmap());

	Graphics gs(hMemDC);
	gs.SetSmoothingMode(SmoothingModeHighQuality);
	gs.DrawImage(&bitmap, 0, 0, nWidth, nHeight);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);

	return pStretchImage;
}
