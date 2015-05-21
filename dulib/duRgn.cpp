//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duRgn.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duRgn.h"
#include "duStyleGroup.h"
#include "duImage.h"

duRgn::duRgn() :
	 m_nRoundX(0)
	,m_nRoundY(0)
	,m_hRgn(NULL)
	,m_nRgnWidth(0)
	,m_nRgnHeight(0)
{
	ZeroMemory(m_szType, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szStyle, MAX_NAME * sizeof(TCHAR));
}

duRgn::~duRgn()
{
	SAFE_DELETE_OBJECT(m_hRgn);
}

BOOL WINAPI duRgn::OnCreate(TiXmlElement *pElement)
{
	m_nRoundX = ReadXmlAttributeInt(pElement, _T("roundx"));
	m_nRoundY = ReadXmlAttributeInt(pElement, _T("roundy"));
	ReadXmlAttributeText(pElement, _T("type"), m_szType, MAX_NAME);
	ReadXmlAttributeText(pElement, _T("style"), m_szStyle, MAX_NAME);

	return duResBase::OnCreate(pElement);
}

void WINAPI duRgn::SetRgnType(LPCTSTR lpszType)
{
	if (lpszType)
		_tcsncpy(m_szType, lpszType, MAX_NAME);
	else
		ZeroMemory(m_szType, MAX_NAME * sizeof(TCHAR));
}

void WINAPI duRgn::SetStyle(LPCTSTR lpszStyle)
{
	if(lpszStyle)
		_tcsncpy(m_szStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szType, MAX_NAME * sizeof(TCHAR));

}

HRGN WINAPI duRgn::GetHRgn(int nWidth, int nHeight)
{
	duRect rectNew;
	rectNew.SetRect(0, 0, nWidth+1, nHeight+1);

	HRGN hRgnLeftBottom = NULL;
	HRGN hRgnRightBottom = NULL;

	if (lstrcmpi(m_szType, _T("roundrect")) == 0)
		return CreateRoundRectRgn(rectNew.left, rectNew.top, rectNew.right, rectNew.bottom, m_nRoundX, m_nRoundY);
	else if (lstrcmpi(m_szType, _T("halfroundrect")) == 0)
	{
		HRGN hRgn = CreateRoundRectRgn(rectNew.left, rectNew.top, rectNew.right, rectNew.bottom, m_nRoundX, m_nRoundY);
		hRgnLeftBottom = CreateRectRgn(rectNew.left, rectNew.bottom - m_nRoundY, rectNew.left + m_nRoundX, rectNew.bottom);
		
		if (hRgn && hRgnLeftBottom)
		{
			int nRegion = CombineRgn(hRgn, hRgn, hRgnLeftBottom, RGN_OR);
			if (nRegion == NULLREGION || ERROR)
			{
				SAFE_DELETE_OBJECT(hRgnLeftBottom);
				return NULL;
			}

			hRgnRightBottom = CreateRectRgn(rectNew.right - m_nRoundX, rectNew.bottom - m_nRoundY,  rectNew.right, rectNew.bottom);
			if (hRgnRightBottom)
			{
				nRegion = CombineRgn(hRgn, hRgn, hRgnRightBottom, RGN_OR);
				if (nRegion == NULLREGION || ERROR)
				{
					SAFE_DELETE_OBJECT(hRgnLeftBottom);
					SAFE_DELETE_OBJECT(hRgnRightBottom);
					return NULL;
				}
				
				m_hRgn = hRgn;
			}
		}
	}
	else if (lstrcmpi(m_szType, _T("style")) == 0)
		return GetStyleHRgn(nWidth, nHeight);

	SAFE_DELETE_OBJECT(hRgnLeftBottom);
	SAFE_DELETE_OBJECT(hRgnRightBottom);

	return m_hRgn;
}

HRGN WINAPI duRgn::GetStyleHRgn(int nWidth, int nHeight)
{
	duStyleGroup *pStyleGroup = (duStyleGroup *)m_pResManager->GetResObj(m_szStyle, DU_RES_STYLEGROUP);
	if (pStyleGroup == NULL)
		return NULL;
	
	duImageStyle *pImageStyle = NULL;
	int i;
	int nStyleCount = pStyleGroup->GetStyleCount();
	for (i = 0;i < nStyleCount; i++)
	{
		duStyleBase *pStyleBase = pStyleGroup->GetStyle(i);
		if ((pStyleBase) && (pStyleBase->GetState() & DU_STATE_NORMAL) && (pStyleBase->GetType() == STYLE_IMAGE))
		{
			pImageStyle = (duImageStyle *)pStyleBase;
			break;
		}
	}
	
	if (pImageStyle == NULL)
		return NULL;
	
	HRGN hRgn = NULL;
	LPCTSTR lpszPaintMode = pImageStyle->GetPaintMode();
	if (lstrcmpi(lpszPaintMode, _T("normal")) == 0)
		hRgn = GetNormalStyleHRgn(pImageStyle, nWidth, nHeight);
	else if (lstrcmpi(lpszPaintMode, _T("stretch")) == 0)
		hRgn = GetStretchStyleHRgn(pImageStyle, nWidth, nHeight);
	else if (lstrcmpi(lpszPaintMode, _T("stretchvert")) == 0)
		hRgn = GetStretchVertStyleHRgn(pImageStyle, nWidth, nHeight);
	else if (lstrcmpi(lpszPaintMode, _T("stretchhorz")) == 0)
		hRgn = GetStretchHorzStyleHRgn(pImageStyle, nWidth, nHeight);
	else if (lstrcmpi(lpszPaintMode, _T("stretchpartall")) == 0)
		hRgn = GetStretchPartAllStyleHRgn(pImageStyle, nWidth, nHeight);
	
	return hRgn;
}

HRGN WINAPI duRgn::GetNormalStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight)
{
GetNormalStyleHRgn_start:
	if (m_hRgn == NULL)
	{
		LPCTSTR lpszPic = pStyle->GetPicFile();
		duImage *pImage = (duImage *)m_pResManager->GetResObj(lpszPic, DU_RES_IMAGE);
		if (pImage == NULL)
			return NULL;

		RGBQUAD *pRgnQuad = (RGBQUAD *)pImage->GetBits();
		if (pRgnQuad == NULL)
			return NULL;
		
		m_nRgnWidth = pImage->GetWidth();
		m_nRgnHeight = pImage->GetHeight();
		
		HRGN hRgn = ::CreateRectRgn(0, 0, m_nRgnWidth, m_nRgnHeight);
		int x,y;
		for (y = 0;y < m_nRgnHeight; y++)
		{
			for (x = 0;x < m_nRgnWidth; x++)
			{
				if (pRgnQuad->rgbReserved == 0)
				{
					HRGN hRgnTemp = ::CreateRectRgn(x, y, x+1, y+1);
					::CombineRgn(hRgn, hRgn, hRgnTemp, RGN_XOR);
					::DeleteObject(hRgnTemp);
				}
				pRgnQuad++;
			}
		}
		
		m_hRgn = hRgn;
		return hRgn;
	}
	else
	{
		HRGN hRgn = ::CreateRectRgn(0, 0, nWidth, nHeight);
		if (::CombineRgn(hRgn, m_hRgn, NULL, RGN_AND) == ERROR)
		{
			m_hRgn = NULL;
			goto GetNormalStyleHRgn_start;
		}

		m_hRgn = hRgn;
		return hRgn;
	}

	return NULL;
}

HRGN WINAPI duRgn::GetStretchStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight)
{
	if (m_hRgn == NULL)
		GetNormalStyleHRgn(pStyle, nWidth, nHeight);
	
	if (m_hRgn == NULL)
		return NULL;

	DWORD dwSize = ::GetRegionData(m_hRgn, 0, NULL);
	BYTE *pBuffer = new BYTE[dwSize];
	ZeroMemory(pBuffer, dwSize);
	RGNDATA *pRgnData = (RGNDATA *)pBuffer;
	pRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
	pRgnData->rdh.iType = RDH_RECTANGLES;
	::GetRegionData(m_hRgn, dwSize, pRgnData);
	
	XFORM xForm;
	xForm.eM11 = nWidth / (float)m_nRgnWidth;
	xForm.eM12 = 0.0;
	xForm.eM21 = 0.0;
	xForm.eM22 = nHeight / (float)m_nRgnHeight;
	xForm.eDx = 0.0;
	xForm.eDy = 0.0;

	HRGN hRgn = ::ExtCreateRegion(&xForm, dwSize, pRgnData);	
	delete [] pBuffer;
	return hRgn;
}

HRGN WINAPI duRgn::GetStretchVertStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight)
{
	if (m_hRgn == NULL)
		GetNormalStyleHRgn(pStyle, nWidth, nHeight);
	
	if (m_hRgn == NULL)
		return NULL;
	
	DWORD dwSize = ::GetRegionData(m_hRgn, 0, NULL);
	char *pBuffer = new char[dwSize];
	ZeroMemory(pBuffer, dwSize);
	RGNDATA *pRgnData = (RGNDATA *)pBuffer;
	pRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
	pRgnData->rdh.iType = RDH_RECTANGLES;
	
	::GetRegionData(m_hRgn, dwSize, pRgnData);

	int nTopPart = pStyle->GetTopPart();
	int nBottomPart = pStyle->GetBottomPart();
	float yRatio = (float)(nHeight - nTopPart - nBottomPart)/ (float)(m_nRgnHeight - nTopPart - nBottomPart);
	
	LPRECT lpRect = (LPRECT)pRgnData->Buffer;
	int i;
	for (i = 0;i < (int)pRgnData->rdh.nCount; i++)
	{		
		int nRgnTop = lpRect->top;
		int nRgnBottom = lpRect->bottom;
		
		if ((nRgnTop > nTopPart) && (nRgnTop < (m_nRgnHeight - nBottomPart)))
			lpRect->top = nTopPart + (int)(yRatio * (float)(nRgnTop - nTopPart));
		else if (nRgnTop >= (m_nRgnHeight - nBottomPart))
			lpRect->top = nHeight - (m_nRgnHeight - nRgnTop);
		
		if ((nRgnBottom > nTopPart) && (nRgnBottom < (m_nRgnHeight - nBottomPart)))
			lpRect->bottom = nTopPart + (int)(yRatio * (float)(nRgnBottom - nTopPart));
		else if (nRgnBottom >= (m_nRgnHeight - nBottomPart))
			lpRect->bottom = nHeight - (m_nRgnHeight - nRgnBottom);

		lpRect++;
	}
	
	HRGN hRgn = ::ExtCreateRegion(NULL, dwSize, pRgnData);
	
	delete [] pBuffer;
	return hRgn;
}

HRGN WINAPI duRgn::GetStretchHorzStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight)
{
	if (m_hRgn == NULL)
		GetNormalStyleHRgn(pStyle, nWidth, nHeight);
	
	if (m_hRgn == NULL)
		return NULL;
	
	DWORD dwSize = ::GetRegionData(m_hRgn, 0, NULL);
	char *pBuffer = new char[dwSize];
	ZeroMemory(pBuffer, dwSize);
	RGNDATA *pRgnData = (RGNDATA *)pBuffer;
	pRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
	pRgnData->rdh.iType = RDH_RECTANGLES;
	
	::GetRegionData(m_hRgn, dwSize, pRgnData);

	int nLeftPart = pStyle->GetLeftPart();
	int nRightPart = pStyle->GetRightPart();
	
	float xRatio = (float)(nWidth - nLeftPart - nRightPart)/ (float)(m_nRgnWidth - nLeftPart - nRightPart);

	LPRECT lpRect = (LPRECT)pRgnData->Buffer;
	int i;
	for (i = 0;i < (int)pRgnData->rdh.nCount; i++)
	{		
		int nRgnLeft = lpRect->left;
		int nRgnRight = lpRect->right;
		
		if ((nRgnLeft > nLeftPart) && (nRgnLeft < (m_nRgnWidth - nRightPart)))
			lpRect->left = nLeftPart + (int)(xRatio * (float)(nRgnLeft - nLeftPart));
		else if (nRgnLeft >= (m_nRgnWidth - nRightPart))
			lpRect->left = nWidth - (m_nRgnWidth - nRgnLeft);
		
		if ((nRgnRight > nLeftPart) && (nRgnRight < (m_nRgnWidth - nRightPart)))
			lpRect->right = nLeftPart + (int)(xRatio * (float)(nRgnRight - nLeftPart));
		else if (nRgnRight >= (m_nRgnWidth - nRightPart))
			lpRect->right = nWidth - (m_nRgnWidth - nRgnRight);

		lpRect++;
	}
	
	HRGN hRgn = ::ExtCreateRegion(NULL, dwSize, pRgnData);
	
	delete [] pBuffer;
	return hRgn;
}

HRGN WINAPI duRgn::GetStretchPartAllStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight)
{
	if (m_hRgn == NULL)
		GetNormalStyleHRgn(pStyle, nWidth, nHeight);
	
	if (m_hRgn == NULL)
		return NULL;
	
	DWORD dwSize = ::GetRegionData(m_hRgn, 0, NULL);
	char *pBuffer = new char[dwSize];
	ZeroMemory(pBuffer, dwSize);
	RGNDATA *pRgnData = (RGNDATA *)pBuffer;
	pRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
	pRgnData->rdh.iType = RDH_RECTANGLES;
	
	::GetRegionData(m_hRgn, dwSize, pRgnData);

	int nLeftPart = pStyle->GetLeftPart();
	int nRightPart = pStyle->GetRightPart();
	int nTopPart = pStyle->GetTopPart();
	int nBottomPart = pStyle->GetBottomPart();
	
	float xRatio = (float)(nWidth - nLeftPart - nRightPart)/ (float)(m_nRgnWidth - nLeftPart - nRightPart);
	float yRatio = (float)(nHeight - nTopPart - nBottomPart)/ (float)(m_nRgnHeight - nTopPart - nBottomPart);
	
	LPRECT lpRect = (LPRECT)pRgnData->Buffer;
	int i;
	for (i = 0;i < (int)pRgnData->rdh.nCount; i++)
	{		
		int nRgnLeft = lpRect->left;
		int nRgnRight = lpRect->right;
		int nRgnTop = lpRect->top;
		int nRgnBottom = lpRect->bottom;
		
		if ((nRgnLeft > nLeftPart) && (nRgnLeft < (m_nRgnWidth - nRightPart)))
			lpRect->left = nLeftPart + (int)(xRatio * (float)(nRgnLeft - nLeftPart));
		else if (nRgnLeft >= (m_nRgnWidth - nRightPart))
			lpRect->left = nWidth - (m_nRgnWidth - nRgnLeft);
		
		if ((nRgnRight > nLeftPart) && (nRgnRight < (m_nRgnWidth - nRightPart)))
			lpRect->right = nLeftPart + (int)(xRatio * (float)(nRgnRight - nLeftPart));
		else if (nRgnRight >= (m_nRgnWidth - nRightPart))
			lpRect->right = nWidth - (m_nRgnWidth - nRgnRight);
			
		if ((nRgnTop > nTopPart) && (nRgnTop < (m_nRgnHeight - nBottomPart)))
			lpRect->top = nTopPart + (int)(yRatio * (float)(nRgnTop - nTopPart));
		else if (nRgnTop >= (m_nRgnHeight - nBottomPart))
			lpRect->top = nHeight - (m_nRgnHeight - nRgnTop);
		
		if ((nRgnBottom > nTopPart) && (nRgnBottom < (m_nRgnHeight - nBottomPart)))
			lpRect->bottom = nTopPart + (int)(yRatio * (float)(nRgnBottom - nTopPart));
		else if (nRgnBottom >= (m_nRgnHeight - nBottomPart))
			lpRect->bottom = nHeight - (m_nRgnHeight - nRgnBottom);

		lpRect++;
	}
	
	HRGN hRgn = ::ExtCreateRegion(NULL, dwSize, pRgnData);
	
	delete [] pBuffer;
	return hRgn;
}
