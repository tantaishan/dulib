//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duStyleGroup.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "duStyleGroup.h"
#include "duWindowManager.h"

duStyleGroup::duStyleGroup()
{
	m_vtStylelist.clear();
}

duStyleGroup::~duStyleGroup()
{
	Destroy();
}

void duStyleGroup::Destroy()
{
	vector<duStyleBase *>::iterator iterStyleBase = m_vtStylelist.begin();
	for (; iterStyleBase != m_vtStylelist.end(); iterStyleBase++)
	{
		duStyleBase *pStyleBase = *iterStyleBase;
		if (pStyleBase)
		{
			pStyleBase->FinalRelease();
			pStyleBase = NULL;
		}
	}
	m_vtStylelist.clear();
}

BOOL duStyleGroup::OnCreate(TiXmlElement *pElement)
{
	if (!duResBase::OnCreate(pElement))
		return FALSE;
	
	TiXmlElement *pChildElement = pElement->FirstChildElement();
	for (; pChildElement; pChildElement = pChildElement->NextSiblingElement())
	{
		LPCTSTR lpszType = pChildElement->Value();
		duStyleBase *pNewStyle = CreateChildStyle(lpszType);
		if (pNewStyle)
		{
			pNewStyle->SetResManager(GetResManager());
			duWindowManager *pWinManager = GetResManager()->GetWindowManager();

			AddStyle(pNewStyle);
			pNewStyle->OnCreate(pChildElement);
		}
	}

	return TRUE;
}

void duStyleGroup::Draw(HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha)
{
	int nStyleCount = GetStyleCount();
	int i;
	for (i = 0; i < nStyleCount; i++)
	{
		duStyleBase *pStyle = m_vtStylelist[i];
		if (pStyle && (pStyle->GetState() & uState)) 
			pStyle->Draw(hDC, lpDstRect, lpszText, nAlpha);
	}
}

void duStyleGroup::DrawNotText(HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha)
{
	int nStyleCount = GetStyleCount();
	int i;
	for (i = 0; i < nStyleCount; i++)
	{
		duStyleBase *pStyle = m_vtStylelist[i];
		if (pStyle && (pStyle->GetState() & uState)) 
		{
			if (pStyle->GetType() != STYLE_TEXT)
				pStyle->Draw(hDC, lpDstRect, lpszText, nAlpha);
		}
	}
}

void duStyleGroup::DrawOnlyText(HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha)
{
	int nStyleCount = GetStyleCount();
	int i;
	for (i = 0; i < nStyleCount; i++)
	{
		duStyleBase *pStyle = m_vtStylelist[i];
		if (pStyle && (pStyle->GetState() & uState)) 
		{
			if (pStyle->GetType() == STYLE_TEXT)
				pStyle->Draw(hDC, lpDstRect, lpszText, nAlpha);
		}
	}
}

void duStyleGroup::GrayDrawByStyle(HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha)
{
	BYTE r,g,b,gray;
	int styleCount = GetStyleCount();
	UINT nStyleType = 0;
	COLORREF orgColor, grayColor;

	int i = 0;
	for (i = 0;i < styleCount; i++)
	{
		duStyleBase *pStyle = m_vtStylelist[i];

		if ( (pStyle == NULL) || (!(pStyle->GetState() & uState)) )
			continue;

		nStyleType = pStyle->GetType();
		if (nStyleType == STYLE_TEXT)
		{
			duTextStyle *pTextStyle = (duTextStyle *)pStyle;

			orgColor  = pTextStyle->GetTextColor();
			r         = GetRValue(orgColor);
			g         = GetGValue(orgColor);
			b         = GetBValue(orgColor);
			gray      = RGB2GRAY(r,g,b);
			grayColor = RGB(gray, gray, gray);

			pTextStyle->SetTextColor(grayColor);
			pTextStyle->Draw(hDC, lpDstRect, lpszText, nAlpha);
			pTextStyle->SetTextColor(orgColor);
		}
		else if (nStyleType == STYLE_RECT)
		{
			duRectStyle *pRectStyle = (duRectStyle *)pStyle;

			COLORREF fillColor, fillGrayColor, borderColor, borderGrayColor;
			if ( pRectStyle->IsFillRect() )
			{
				fillColor           = pRectStyle->GetFillColor();
				r                   = GetRValue(fillColor);
				g                   = GetGValue(fillColor);
				b                   = GetBValue(fillColor);
				gray                = RGB2GRAY(r, g, b);
				fillGrayColor       = RGB(gray, gray, gray);

				pRectStyle->SetFillColor(fillGrayColor);

				if ( pRectStyle->IsDrawBorder() )
				{
					borderColor     = pRectStyle->GetBorderColor();
					r               = GetRValue(borderColor);
					g               = GetGValue(borderColor);
					b               = GetBValue(borderColor);
					gray            = RGB2GRAY(r, g, b);
					borderGrayColor = RGB(gray, gray, gray);

					pRectStyle->SetBorderColor(borderGrayColor);
				}
				pRectStyle->Draw(hDC, lpDstRect, lpszText, nAlpha);

				pRectStyle->SetFillColor(fillColor);

				if ( pRectStyle->IsDrawBorder() )
				{
					pRectStyle->SetBorderColor(borderColor);
				}
			}
			else
			{
				if ( pRectStyle->IsDrawBorder() )
				{
					borderColor     = pRectStyle->GetBorderColor();
					r               = GetRValue(borderColor);
					g               = GetGValue(borderColor);
					b               = GetBValue(borderColor);
					gray            = RGB2GRAY(r, g, b);
					borderGrayColor = RGB(gray, gray, gray);

					pRectStyle->SetBorderColor(borderGrayColor);				
					
					pRectStyle->Draw(hDC, lpDstRect, lpszText, nAlpha);
					pRectStyle->SetBorderColor(borderColor);
				}
			}
		}
		else if (nStyleType == STYLE_IMAGE)
		{
			duImageStyle *pImageStyle = (duImageStyle *)pStyle;
			duResManager *pResManager = GetResManager();
			if (pResManager)
			{
				dustring picfile  = pImageStyle->GetPicFile();
				duImage *pImage = (duImage *)pResManager->GetResObj(picfile.c_str(), DU_RES_IMAGE);
				if (pImage)
				{
					duImage *pGrayImage = pImage->CreateGrayImage();
					if (pGrayImage)
					{
						pResManager->AddResObj(pGrayImage);
						LPCTSTR pNewPicFile = pGrayImage->GetName();
						pImageStyle->SetPicFile(pNewPicFile);
						pImageStyle->Draw(hDC, lpDstRect, lpszText, nAlpha);
						pImageStyle->SetPicFile(picfile.c_str());
						pResManager->DeleteResObj(pNewPicFile, DU_RES_IMAGE);
					}
				}				
			}
		}
		else if (nStyleType == STYLE_LINE)
		{
			duLineStyle *pLineStyle = (duLineStyle *)pStyle;
			COLORREF clrLineColor, clrGrayColor;
			clrLineColor = pLineStyle->GetLineColor();
			BYTE r, g, b, gray;
			r                   = GetRValue(clrLineColor);
			g                   = GetGValue(clrLineColor);
			b                   = GetBValue(clrLineColor);
			gray                = RGB2GRAY(r, g, b);
			clrGrayColor = RGB(gray, gray, gray);
			
			pLineStyle->SetLineColor(clrGrayColor);
			pLineStyle->Draw(hDC, lpDstRect, lpszText, nAlpha);
			pLineStyle->SetLineColor(clrLineColor);
		}
	}
}

int duStyleGroup::GetStyleCount()
{
	return (int)m_vtStylelist.size();
}

duStyleBase *duStyleGroup::GetStyle(int nIndex)
{
	int nStyleCount = GetStyleCount();
	if (nIndex < 0 || nIndex >= nStyleCount)
		return NULL;

	return m_vtStylelist[nIndex];
}

void duStyleGroup::AddStyle(duStyleBase *pStyleBase)
{
	m_vtStylelist.push_back(pStyleBase);
}

void duStyleGroup::CalcTextRect(UINT uState, LPCTSTR lpszText, LPSIZE lpOutSize)
{
	if (lpOutSize == NULL || lpszText == NULL)
		return;

	lpOutSize->cx = lpOutSize->cy = 0;
	int nStyleCount = GetStyleCount();
	int i = 0;
	for (i = 0; i < nStyleCount; i++)
	{
		duStyleBase *pStyleBase = GetStyle(i);
		if (pStyleBase && (pStyleBase->GetType() == STYLE_TEXT) && (pStyleBase->GetState() & uState))
		{
			duTextStyle *pTextStyle = (duTextStyle *)pStyleBase;
			pTextStyle->CalcTextRect(lpszText, lpOutSize);
			break;
		}
	}
}

BOOL duStyleGroup::RemoveStyle(int nIndex)
{
	int nStyleCount = GetStyleCount();
	if (nIndex < 0 || nIndex >= nStyleCount)
		return FALSE;

	int i = 0;
	vector<duStyleBase *>::iterator iterStyle = m_vtStylelist.begin();
	for (; iterStyle != m_vtStylelist.end(); iterStyle++)
	{
		if (i != nIndex)
		{
			i++;
			continue;
		}
		
		duStyleBase *pStyleBase = *iterStyle;
		if (pStyleBase)
		{
			m_vtStylelist.erase(iterStyle);
			delete pStyleBase;
			pStyleBase = NULL;
			return TRUE;
		}
		i++;
	}

	return FALSE;
}


BOOL duStyleGroup::MoveUp(int nIndex)
{
	int nStyleCount = GetStyleCount();
	if (nIndex >= nStyleCount || nIndex <= 0)
		return FALSE;

	duStyleBase *pStyleBase = m_vtStylelist[nIndex - 1];
	m_vtStylelist[nIndex - 1] = m_vtStylelist[nIndex];
	m_vtStylelist[nIndex] = pStyleBase;
	return TRUE;
}

BOOL duStyleGroup::MoveDown(int nIndex)
{
	int nStyleCount = GetStyleCount();
	if (nIndex >= nStyleCount || nIndex < 0)
		return FALSE;

	duStyleBase *pStyleBase = m_vtStylelist[nIndex + 1];
	m_vtStylelist[nIndex + 1] = m_vtStylelist[nIndex];
	m_vtStylelist[nIndex] = pStyleBase;
	return TRUE;
}

duStyleBase *CreateChildStyle(LPCTSTR lpszType)
{
	if (lpszType == NULL)
		return NULL;
	
	duStyleBase *pStyle = NULL;
	if (lstrcmpi(lpszType, _T("image")) == 0)
		pStyle = new duImageStyle;
	else if (lstrcmpi(lpszType, _T("text")) == 0)
		pStyle = new duTextStyle;
	else if (lstrcmpi(lpszType, _T("rect")) == 0)
		pStyle = new duRectStyle;
	else if (lstrcmpi(lpszType, _T("line")) == 0)
		pStyle = new duLineStyle;
		
	return pStyle;
}

