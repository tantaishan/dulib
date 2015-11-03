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

BOOL DrawNormal(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
	duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nAlpha);

BOOL DrawStretch(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
	duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, int nAlpha);

BOOL DrawTile(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
	duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, int nAlpha);

BOOL DrawVertStretch(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
	duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, int nTopPart, int nBottomPart, int nAlpha);

BOOL DrawHorzStretch(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
	duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, int nLeftPart, int nRightPart, int nAlpha);
									 
BOOL DrawAllPartStretch(HDC hDstDC, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
	duImage *pImage, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, 
	int nLeftPart, int nRightPart, int nTopPart, int nBottomPart, int nAlpha);
