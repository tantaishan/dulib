//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duDrawHelper.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duDrawHelper.h"

BOOL WINAPI DrawNormal(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
								duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nAlpha)
{
	if (pImage == NULL || pImage->GetBitmap() == NULL || pImage->GetBits() == NULL ||
		nWidthDest <= 0 || nHeightDest <= 0)
		return FALSE;

	HDC hSrcDC = ::CreateCompatibleDC(NULL);	
	HBITMAP hSrcOldBmp = (HBITMAP)::SelectObject(hSrcDC, pImage->GetBitmap());

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, nAlpha, AC_SRC_ALPHA};
	::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, 
		hSrcDC, nXOriginSrc, nYOriginSrc, nWidthDest, nHeightDest, bf);

	::SelectObject(hSrcDC, hSrcOldBmp);
	::DeleteDC(hSrcDC);
	
	return TRUE;
}

BOOL WINAPI DrawStretch(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
								 duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, int nAlpha)
{
	if (pImage == NULL || pImage->GetBitmap() == NULL || pImage->GetBits() == NULL ||
		nWidthDest <= 0 || nHeightDest <= 0 ||
		nWidthSrc <= 0 || nHeightSrc <= 0)
		return FALSE;
	
	HDC hSrcDC = ::CreateCompatibleDC(NULL);
	HBITMAP hSrcOldBmp = (HBITMAP)::SelectObject(hSrcDC, pImage->GetBitmap());

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, nAlpha, AC_SRC_ALPHA};
	::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, 
		hSrcDC, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, bf);

	::SelectObject(hSrcDC, hSrcOldBmp);
	::DeleteDC(hSrcDC);

	return TRUE;
}

BOOL WINAPI DrawTile(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
							  duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, int nAlpha)
{
	if (pImage == NULL || pImage->GetBitmap() == NULL || pImage->GetBits() == NULL ||
		nWidthDest <= 0 || nHeightDest <= 0 ||
		nWidthSrc <= 0 || nHeightSrc <= 0)
		return FALSE;

	HDC hSrcDC = ::CreateCompatibleDC(NULL);
	HBITMAP hSrcOldBmp = (HBITMAP)::SelectObject(hSrcDC, pImage->GetBitmap());
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, nAlpha, AC_SRC_ALPHA};

	int x, y;
	x = (nWidthDest / nWidthSrc) + 1;
	y = (nHeightDest / nHeightSrc) + 1;

	duRect rcTileDest;
	rcTileDest.SetRectEmpty();

	int i, j;
	for (i = 0; i < x; i++)
	{
		rcTileDest.left = nXOriginDest + i * nWidthSrc;
		rcTileDest.right = min(rcTileDest.left + nWidthSrc, nXOriginDest + nWidthDest);

		for (j = 0; j < y; j++)
		{
			rcTileDest.top = nYOriginDest + j * nHeightSrc;
			rcTileDest.bottom = min(rcTileDest.top + nHeightSrc, nYOriginDest + nHeightDest);

			::AlphaBlend(hDstDC, rcTileDest.left, rcTileDest.top, rcTileDest.Width(), rcTileDest.Height(), 
				hSrcDC, nXOriginSrc, nYOriginSrc, rcTileDest.Width(), rcTileDest.Height(), bf);
		}
	}

	::SelectObject(hSrcDC, hSrcOldBmp);
	::DeleteDC(hSrcDC);

	return TRUE;
}

BOOL WINAPI DrawVertStretch(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
									 duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, 
									 int nTopPart, int nBottomPart, int nAlpha)
{
	if (pImage == NULL || pImage->GetBitmap() == NULL || pImage->GetBits() == NULL ||
		nWidthDest <= 0 || nHeightDest <= 0 ||
		nWidthSrc <= 0 || nHeightSrc <= 0 ||
		nTopPart < 0 || nBottomPart < 0)
		return FALSE;

	HDC hSrcDC = ::CreateCompatibleDC(NULL);
	HBITMAP hSrcOldBmp = (HBITMAP)::SelectObject(hSrcDC, pImage->GetBitmap());
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, nAlpha, AC_SRC_ALPHA};

	if (nHeightDest <= nTopPart)
	{
		::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, 
			hSrcDC, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightDest, bf);
	}
	else if (nHeightDest < (nTopPart + nBottomPart))
	{
		::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest, nWidthDest, nTopPart, 
			hSrcDC, nXOriginSrc, nYOriginSrc, nWidthSrc, nTopPart, bf);

		::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest + nTopPart, nWidthDest, nHeightDest - nTopPart, 
			hSrcDC, nXOriginSrc, nYOriginSrc + nTopPart, nWidthSrc, nHeightSrc - nTopPart - nBottomPart, bf);
	}
	else
	{
		::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest, nWidthDest, nTopPart, 
			hSrcDC, nXOriginSrc, nYOriginSrc, nWidthSrc, nTopPart, bf);

		::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest + nTopPart, nWidthDest, nHeightDest - nTopPart - nBottomPart, 
			hSrcDC, nXOriginSrc, nYOriginSrc + nTopPart, nWidthSrc, nHeightSrc - nTopPart - nBottomPart, bf);

		::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest + nHeightDest - nBottomPart, nWidthDest, nBottomPart, 
			hSrcDC, nXOriginSrc, nYOriginSrc + nHeightSrc - nBottomPart, nWidthSrc, nBottomPart, bf);
	}

	::SelectObject(hSrcDC, hSrcOldBmp);
	::DeleteDC(hSrcDC);
	return TRUE;
}

BOOL WINAPI DrawHorzStretch(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
									 duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
									 int nLeftPart, int nRightPart, int nAlpha)
{
	if (pImage == NULL || pImage->GetBitmap() == NULL || pImage->GetBits() == NULL ||
		nWidthDest <= 0 || nHeightDest <= 0 ||
		nWidthSrc <= 0 || nHeightSrc <= 0 ||
		nLeftPart < 0 || nRightPart < 0)
		return FALSE;

	HDC hSrcDC = ::CreateCompatibleDC(NULL);
	HBITMAP hSrcOldBmp = (HBITMAP)::SelectObject(hSrcDC, pImage->GetBitmap());
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, nAlpha, AC_SRC_ALPHA};

	if (nWidthDest <= nLeftPart)
	{
		::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, 
			hSrcDC, nXOriginSrc, nYOriginSrc, nWidthDest, nHeightSrc, bf);
	}
	else if (nWidthDest < (nLeftPart + nRightPart))
	{
		::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest, nLeftPart, nHeightDest, 
			hSrcDC, nXOriginSrc, nYOriginSrc, nLeftPart, nHeightSrc, bf);

		::AlphaBlend(hDstDC, nXOriginDest + nLeftPart, nYOriginDest, nWidthDest - nLeftPart, nHeightDest, 
			hSrcDC, nXOriginSrc + nLeftPart, nYOriginSrc, nWidthSrc - nLeftPart - nRightPart, nHeightSrc, bf);
	}
	else
	{
		::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest, nLeftPart, nHeightDest, 
			hSrcDC, nXOriginSrc, nYOriginSrc, nLeftPart, nHeightSrc, bf);

		::AlphaBlend(hDstDC, nXOriginDest + nLeftPart, nYOriginDest, nWidthDest - nLeftPart - nRightPart, nHeightDest, 
			hSrcDC, nXOriginSrc + nLeftPart, nYOriginSrc, nWidthSrc - nLeftPart - nRightPart, nHeightSrc, bf);

		::AlphaBlend(hDstDC, nXOriginDest + nWidthDest - nRightPart, nYOriginDest, nRightPart, nHeightDest, 
			hSrcDC, nXOriginSrc + nWidthSrc - nRightPart, nYOriginSrc, nRightPart, nHeightSrc, bf);
	}

	::SelectObject(hSrcDC, hSrcOldBmp);
	::DeleteDC(hSrcDC);

	return TRUE;
}

BOOL WINAPI DrawAllPartStretch(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
										duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
										int nLeftPart, int nRightPart, int nTopPart, int nBottomPart, int nAlpha)
{
	if (pImage == NULL || pImage->GetBitmap() == NULL || pImage->GetBits() == NULL ||
		nWidthDest <= 0 || nHeightDest <= 0 ||
		nWidthSrc <= 0 || nHeightSrc <= 0 ||
		nLeftPart < 0 || nRightPart < 0 || nTopPart < 0 || nBottomPart < 0)
		return FALSE;
		
	HDC hSrcDC = ::CreateCompatibleDC(NULL);
	HBITMAP hSrcOldBmp = (HBITMAP)::SelectObject(hSrcDC, pImage->GetBitmap());
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, nAlpha, AC_SRC_ALPHA};

	//
	::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest, nLeftPart, nTopPart, 
		hSrcDC, nXOriginSrc, nYOriginSrc, nLeftPart, nTopPart, bf);

	::AlphaBlend(hDstDC, nXOriginDest + nLeftPart, nYOriginDest, nWidthDest - nLeftPart - nRightPart, nTopPart, 
		hSrcDC, nXOriginSrc + nLeftPart, nYOriginSrc, nWidthSrc - nLeftPart - nRightPart, nTopPart, bf);

	::AlphaBlend(hDstDC, nXOriginDest + nWidthDest - nRightPart, nYOriginDest, nRightPart, nTopPart, 
		hSrcDC, nXOriginSrc + nWidthSrc - nRightPart, nYOriginSrc, nRightPart, nTopPart, bf);


	//
	::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest + nTopPart, nLeftPart, nHeightDest - nTopPart - nBottomPart, 
		hSrcDC, nXOriginSrc, nYOriginSrc + nTopPart, nLeftPart, nHeightSrc - nTopPart - nBottomPart, bf);

	::AlphaBlend(hDstDC, nXOriginDest + nLeftPart, nYOriginDest + nTopPart, nWidthDest - nLeftPart - nRightPart, nHeightDest - nTopPart - nBottomPart, 
		hSrcDC, nXOriginSrc + nLeftPart, nYOriginSrc + nTopPart, nWidthSrc - nLeftPart - nRightPart, nHeightSrc - nTopPart - nBottomPart, bf);

	::AlphaBlend(hDstDC, nXOriginDest + nWidthDest - nRightPart, nYOriginDest + nTopPart, nRightPart, nHeightDest - nTopPart - nBottomPart, 
		hSrcDC, nXOriginSrc + nWidthSrc - nRightPart, nYOriginSrc + nTopPart, nRightPart, nHeightSrc - nTopPart - nBottomPart, bf);

	//
	::AlphaBlend(hDstDC, nXOriginDest, nYOriginDest + nHeightDest - nBottomPart, nLeftPart, nBottomPart, 
		hSrcDC, nXOriginSrc, nYOriginSrc + nHeightSrc - nBottomPart, nLeftPart, nBottomPart, bf);

	::AlphaBlend(hDstDC, nXOriginDest + nLeftPart, nYOriginDest + nHeightDest - nBottomPart, nWidthDest - nLeftPart - nRightPart, nBottomPart, 
		hSrcDC, nXOriginSrc + nLeftPart, nYOriginSrc + nHeightSrc - nBottomPart, nWidthSrc - nLeftPart - nRightPart, nBottomPart, bf);

	::AlphaBlend(hDstDC, nXOriginDest + nWidthDest - nRightPart, nYOriginDest + nHeightDest - nBottomPart, nRightPart, nBottomPart, 
		hSrcDC, nXOriginSrc + nWidthSrc - nRightPart, nYOriginSrc + nHeightSrc - nBottomPart, nRightPart, nBottomPart, bf);

	::SelectObject(hSrcDC, hSrcOldBmp);
	::DeleteDC(hSrcDC);

	return TRUE;
}
