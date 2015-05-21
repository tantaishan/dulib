//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duDrawHelper.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duResBase.h"
#include "duImage.h"

BOOL WINAPI DrawNormal(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
	duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nAlpha);

BOOL WINAPI DrawStretch(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
	duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, int nAlpha);

BOOL WINAPI DrawTile(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
	duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, int nAlpha);

BOOL WINAPI DrawVertStretch(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
	duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, int nTopPart, int nBottomPart, int nAlpha);

BOOL WINAPI DrawHorzStretch(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
	duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, int nLeftPart, int nRightPart, int nAlpha);
									 
BOOL WINAPI DrawAllPartStretch(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
	duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, 
	int nLeftPart, int nRightPart, int nTopPart, int nBottomPart, int nAlpha);
