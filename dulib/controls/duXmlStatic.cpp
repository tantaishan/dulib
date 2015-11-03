//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duXmlStatic.cpp
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "duXmlStatic.h"
#include "duDrawHelper.h"
#include "duCtrlManager.h"

#define XMLSTATIC_TEXT_DEFAULTSAPCE  4
const TCHAR g_emptyString[] = _T("");

duXmlStatic::duXmlStatic() :
	 m_nParagraphSpace(0)
	,m_clrDefault(RGB(0,0,0))
	,m_pHotSegMent(NULL)
{
	ZeroMemory(m_szDefaultFont, MAX_NAME * sizeof(TCHAR));
	m_vtXmlText.clear();
	m_vtLine.clear();
}

duXmlStatic::~duXmlStatic()
{
	Destroy();
}

void duXmlStatic::RegisterControlProperty()
{
}

void duXmlStatic::OnCreate()
{
	ParseTextParam();
//#ifdef _DEBUG
//	PrintText();
//#endif
}

void duXmlStatic::DrawObject(HDC hDC)
{
	duRect rectStatic;
	Plugin_GetRect(this, &rectStatic);
	rectStatic.OffsetRect(-rectStatic.left, -rectStatic.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rectStatic, GetState(), NULL, GetAlpha());
		
	DrawXmlText(hDC);
}

void duXmlStatic::DrawXmlText(HDC hDC)
{
	if (m_vtXmlText.size() <= 0)
		return;
	
	duRect rectStatic;
	Plugin_GetRect(this, &rectStatic);
	rectStatic.OffsetRect(-rectStatic.left, -rectStatic.top);
	
	duRect rcLine;
	int nSegMentLeft;
	duRect rcSegMent;
	
	int nLineCount = (int)m_vtLine.size();
	int i;
	int nCurTop = 0;
	int nCurTextIndex = 0;
	for (i = 0;i < nLineCount; i++)
	{
		XMLSTATICLINE *pLine = m_vtLine[i];
		rcLine.SetRectEmpty();
		rcLine.right = rectStatic.Width();
		rcLine.top = nCurTop;
		rcLine.bottom = nCurTop + pLine->nHeight;
		
		int j;
		nSegMentLeft = 0;
		for (j = 0;j < (int)pLine->vtLineSegment.size(); j++)
		{
			XMLSTATICLINESEGMENT *pSegment = pLine->vtLineSegment[j];
			rcSegMent.SetRectEmpty();
			
			rcSegMent.left = nSegMentLeft;
			rcSegMent.right = nSegMentLeft + pSegment->nWidth; 
			rcSegMent.bottom = rcLine.bottom;
			rcSegMent.top = rcSegMent.bottom - pSegment->nHeight;
			
			PaintSegMent(hDC, pSegment, rcSegMent);
			
			nSegMentLeft = rcSegMent.right;
		}
		
		nCurTop += (pLine->nHeight + m_nParagraphSpace);
	}
}

void duXmlStatic::PaintSegMent(HDC hDC, XMLSTATICLINESEGMENT *pSegment, duRect &rcSegMent)
{
	XMLTEXTHEAD *pTextHead = pSegment->pTextHead;
	HFONT hFont = GetXmlTextHeadFont(pTextHead);
	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
	
	LPTSTR lpString = new TCHAR[pSegment->nStringWidth + 1];
	memset(lpString, 0, sizeof(TCHAR) * (pSegment->nStringWidth + 1));
	_tcsncpy(lpString, pSegment->lpString, pSegment->nStringWidth);

	if (pTextHead->enType == NORMALTEXT)
	{
		duFont *pFont = (duFont *)GetResObj(m_szDefaultFont, DU_RES_FONT);
		DrawText32Bpp(hDC, pFont, m_clrDefault, lpString, pSegment->nStringWidth, &rcSegMent, DT_LEFT|DT_TOP|DT_SINGLELINE, GetAlpha());
	}
	else if (pTextHead->enType == FORMATTEXT)
	{
		XMLFORMATTEXT *pFormatText = (XMLFORMATTEXT *)pTextHead;
		if (m_pHotSegMent && 
			(m_pHotSegMent->pTextHead == pSegment->pTextHead) && 
			IsHrefSegment(m_pHotSegMent))
		{
			duFont *pFont = (duFont *)GetResObj(pFormatText->strFont.c_str(), DU_RES_FONT);
			duFont *pFontUnderline = new duFont;
			pFontUnderline->SetHeight(pFont->GetHeight());
			pFontUnderline->SetWeight(pFont->GetWeight());
			pFontUnderline->SetItalic(pFont->IsItalic());
			pFontUnderline->SetUnderline(TRUE);
			pFontUnderline->SetStrikeOut(pFont->IsStrikeOut());
			pFontUnderline->SetFontFace(pFont->GetFontFace());
			pFontUnderline->CreateFont();
			
			DrawText32Bpp(hDC, pFontUnderline, pFormatText->clrText, lpString, pSegment->nStringWidth, &rcSegMent, DT_LEFT|DT_TOP|DT_SINGLELINE, GetAlpha());
			delete pFontUnderline;
		}
		else
		{
			duFont *pFont = (duFont *)GetResObj(pFormatText->strFont.c_str(), DU_RES_FONT);
			DrawText32Bpp(hDC, pFont, pFormatText->clrText, lpString, pSegment->nStringWidth, &rcSegMent, DT_LEFT|DT_TOP|DT_SINGLELINE, GetAlpha());
		}
	}
	
	::SelectObject(hDC, hOldFont);
	delete [] lpString;
}

void duXmlStatic::OnMouseMove(POINT pt)
{
	duRect rectStatic;
	Plugin_GetRect(this, &rectStatic);
	
	POINT ptStatic(pt);
	ptStatic.x -= rectStatic.left;
	ptStatic.y -= rectStatic.top;
	rectStatic.OffsetRect(-rectStatic.left, -rectStatic.top);
	duRect rcLine;
	duRect rcSegMent;
	int nSegMentLeft = 0;

	int nLineCount = (int)m_vtLine.size();
	int i;
	int nCurTop = 0;
	int nCurTextIndex = 0;
	for (i = 0;i < nLineCount; i++)
	{
		XMLSTATICLINE *pLine = m_vtLine[i];
		rcLine.SetRectEmpty();
		rcLine.right = rectStatic.Width();
		rcLine.top = nCurTop;
		rcLine.bottom = nCurTop + pLine->nHeight;
		
		int j;
		nSegMentLeft = 0;
		for (j = 0;j < (int)pLine->vtLineSegment.size(); j++)
		{
			XMLSTATICLINESEGMENT *pSegment = pLine->vtLineSegment[j];
			rcSegMent.SetRectEmpty();
			
			rcSegMent.left = nSegMentLeft;
			rcSegMent.right = nSegMentLeft + pSegment->nWidth; 
			rcSegMent.bottom = rcLine.bottom;
			rcSegMent.top = rcSegMent.bottom - pSegment->nHeight;
			
			if (rcSegMent.PtInRect(ptStatic))
			{
				if (m_pHotSegMent != pSegment)
				{
					BOOL fNeedRedraw = FALSE;
					if (IsHrefSegment(m_pHotSegMent) || IsHrefSegment(pSegment))
						fNeedRedraw = TRUE;
					
					m_pHotSegMent = pSegment;
					if (fNeedRedraw)
						Plugin_Redraw(this, TRUE);
				}
				return;
			}
			
			nSegMentLeft = rcSegMent.right;
		}
		
		nCurTop += (pLine->nHeight + m_nParagraphSpace);
	}
	
	if (IsHrefSegment(m_pHotSegMent))
	{
		m_pHotSegMent = NULL;
		Plugin_Redraw(this, TRUE);
	}
	else
		m_pHotSegMent = NULL;
}

BOOL duXmlStatic::OnSetCursor()
{
	if (IsHrefSegment(m_pHotSegMent))
		return duPlugin::OnSetCursor();
	
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	return TRUE;
}

void duXmlStatic::OnMouseLeave(POINT pt)
{
	if (IsHrefSegment(m_pHotSegMent))
	{
		m_pHotSegMent = NULL;
		Plugin_Redraw(this, TRUE);
	}

	m_pHotSegMent = NULL;
}

void duXmlStatic::OnMouseLDown(POINT pt)
{
	if (IsHrefSegment(m_pHotSegMent))
	{
		_TRACE(_T("IsHrefSegment true\n"));
		XMLTEXTHEAD *pTextHead = m_pHotSegMent->pTextHead;
		if (pTextHead->enType == NORMALTEXT)
			return;
		
		XMLFORMATTEXT *pFormatText = (XMLFORMATTEXT *)pTextHead;
		if (!pFormatText->strHref.empty())
		{
			NotifyUser(DUM_XMLSTATICHREFCLICK, (WPARAM)pFormatText->strHref.c_str(), NULL);
		}

		Plugin_Redraw(this, TRUE);
	}
	else
	{
		_TRACE(_T("IsHrefSegment false\n"));
	}
}

void duXmlStatic::OnMouseLUp(POINT pt)
{
}

void duXmlStatic::SetText(LPCTSTR lpszText)
{
	duPlugin::SetText(lpszText);
	ParseTextParam();
}

void duXmlStatic::Resize(LPRECT lpRect/*=NULL*/)
{
	duPlugin::Resize(lpRect);
	ParseTextParam();
}

void duXmlStatic::ParseTextParam()
{
	LPCTSTR lpszText = GetText();
	if (lpszText == NULL || *lpszText == 0)
		return;
	
	TiXmlDocument *pXmlDoc = new TiXmlDocument;
	pXmlDoc->Parse(lpszText, NULL);
	
	TiXmlElement *pTextElement = pXmlDoc->FirstChildElement(_T("text"));
	if (pTextElement == NULL)
	{
		delete pXmlDoc;
		return;
	}
	
	Destroy();
	
	m_nParagraphSpace = ReadXmlAttributeInt(pTextElement, _T("ParagraphSpace"));
	ReadXmlAttributeText(pTextElement, _T("Font"), m_szDefaultFont, MAX_NAME);
	m_clrDefault = ReadXmlAttributeColor(pTextElement, _T("Color"));

	TiXmlNode *pChildNode = pTextElement->FirstChild();
	while (pChildNode)
	{
		if (pChildNode->Type() == TiXmlNode::ELEMENT)
		{
			TiXmlElement *pThis = pChildNode->ToElement();
			if (lstrcmpi(pThis->Value(), _T("br")) == 0)
			{
				XMLTEXTHEAD *pBrText = ParseBrText(pThis);
				if (pBrText)
					m_vtXmlText.push_back(pBrText);
			}
			else if (lstrcmpi(pThis->Value(), _T("text")) == 0)
			{
				XMLTEXTHEAD *pFormatText = ParseFormatText(pThis);
				if (pFormatText)
					m_vtXmlText.push_back(pFormatText);
			}
		}
		else if (pChildNode->Type() == TiXmlNode::TEXT)
		{
			TiXmlText *pThis = pChildNode->ToText();
			XMLTEXTHEAD *pNormalText = ParseNormalText(pThis);
			if (pNormalText)
				m_vtXmlText.push_back(pNormalText);
		}
		
		pChildNode = pChildNode->NextSibling();
	}
	
	//////////////////////////////////////////////////////////////////
	duRect rectStatic;
	Plugin_GetRect(this, &rectStatic);
	rectStatic.OffsetRect(-rectStatic.left, -rectStatic.top);
	
	int nTextCount = (int)m_vtXmlText.size();
	int i;
	int nCurrentLineWidth = 0;
	int nMaxHeight = 0;
	
	XMLSTATICLINE *pThisLine = NULL;
	for (i = 0;i < nTextCount; i++)
	{
		XMLTEXTHEAD *pTextHead = m_vtXmlText[i];
		MeasureString(pTextHead);
		
		if (pTextHead->size.cy > nMaxHeight)
			nMaxHeight = pTextHead->size.cy;

		if (pTextHead->enType == BRTEXT)
		{
			XMLSTATICLINESEGMENT *pLineSegment = new XMLSTATICLINESEGMENT;
			pLineSegment->pTextHead = pTextHead;
			pLineSegment->lpString = g_emptyString;
			pLineSegment->nStringWidth = 0;
			pLineSegment->nStringPosInTextHead = 0;
			pLineSegment->nWidth = pTextHead->size.cx;
			pLineSegment->nHeight = pTextHead->size.cy;

			if (pThisLine == NULL)
			{
				pThisLine = new XMLSTATICLINE;
				pThisLine->nWidth = 0;
				pThisLine->nHeight = 0;
				pThisLine->vtLineSegment.clear();
				m_vtLine.push_back(pThisLine);
			}

			pThisLine->nWidth = nCurrentLineWidth;
			pThisLine->nHeight = nMaxHeight;
			pThisLine->vtLineSegment.push_back(pLineSegment);

			pThisLine = NULL;
			nCurrentLineWidth = 0;
		}
		else
		{
			nCurrentLineWidth += pTextHead->size.cx;
			if (nCurrentLineWidth > rectStatic.Width())
			{
				XMLSTATICLINESEGMENT *pLineSegment = MeasureStringBreakPos(pTextHead, 0, nCurrentLineWidth - pTextHead->size.cx, rectStatic.Width());
				if (pLineSegment == NULL)
					_TRACE(_T("Error: pLineSegment is NULL\n"));
				
				if (pThisLine == NULL)
				{
					pThisLine = new XMLSTATICLINE;
					pThisLine->nWidth = 0;
					pThisLine->nHeight = 0;
					pThisLine->vtLineSegment.clear();
					m_vtLine.push_back(pThisLine);
				}

				pThisLine->nWidth = nCurrentLineWidth - pTextHead->size.cx + pLineSegment->nWidth;
				pThisLine->nHeight = nMaxHeight;
				pThisLine->vtLineSegment.push_back(pLineSegment);
				
				nMaxHeight = 0;
				
				int nRemainWidth = nCurrentLineWidth - pThisLine->nWidth;
				XMLSTATICLINESEGMENT *pLineSegment2 = pLineSegment;
				while (nRemainWidth > rectStatic.Width())
				{
					pLineSegment2 = MeasureStringBreakPos(pTextHead, pLineSegment2->nStringPosInTextHead + pLineSegment2->nStringWidth, 0, rectStatic.Width());
					if (pLineSegment2 == NULL)
						_TRACE(_T("Error: pLineSegment2 is NULL\n"));

					pThisLine = new XMLSTATICLINE;
					pThisLine->nWidth = pLineSegment2->nWidth;
					pThisLine->nHeight = pLineSegment2->nHeight;
					pThisLine->vtLineSegment.push_back(pLineSegment2);
					m_vtLine.push_back(pThisLine);

					nRemainWidth -= pLineSegment2->nWidth;
				}
				
				if (pLineSegment2->nStringPosInTextHead + pLineSegment2->nStringWidth < pLineSegment2->pTextHead->strText.length())
				{
					pThisLine = new XMLSTATICLINE;
					XMLSTATICLINESEGMENT *pLineSegment3 = new XMLSTATICLINESEGMENT;
					pLineSegment3->pTextHead = pTextHead;
					pLineSegment3->lpString = pLineSegment2->lpString + pLineSegment2->nStringWidth;
					pLineSegment3->nStringPosInTextHead = pLineSegment2->nStringPosInTextHead + pLineSegment2->nStringWidth ;
					pLineSegment3->nStringWidth = pLineSegment2->pTextHead->strText.length() - pLineSegment3->nStringPosInTextHead;
					pLineSegment3->nWidth = nRemainWidth;
					pLineSegment3->nHeight = pTextHead->size.cy;
					pThisLine->vtLineSegment.push_back(pLineSegment3);
					pThisLine->nWidth = 0;
					pThisLine->nHeight = 0;
					
					nMaxHeight = pTextHead->size.cy;
					m_vtLine.push_back(pThisLine);
				}

				nCurrentLineWidth = nRemainWidth;
			}
			else
			{
				XMLSTATICLINESEGMENT *pLineSegment = new XMLSTATICLINESEGMENT;
				pLineSegment->pTextHead = pTextHead;
				pLineSegment->lpString = pTextHead->strText.c_str();
				pLineSegment->nStringWidth = pTextHead->strText.length();
				pLineSegment->nStringPosInTextHead = 0;
				pLineSegment->nWidth = pTextHead->size.cx;
				pLineSegment->nHeight = pTextHead->size.cy;
				
				if (pThisLine == NULL)
				{
					pThisLine = new XMLSTATICLINE;
					pThisLine->nWidth = 0;
					pThisLine->nHeight = 0;
					m_vtLine.push_back(pThisLine);
				}
				
				pThisLine->vtLineSegment.push_back(pLineSegment);
			}
		}
	}
	
	if (pThisLine->nHeight == 0)
		pThisLine->nHeight = nMaxHeight;

	delete pXmlDoc;
}

void duXmlStatic::Destroy()
{
	m_nParagraphSpace = 0;
	
	int i;
	for (i = 0;i < (int)m_vtXmlText.size(); i++)
	{
		XMLTEXTHEAD *p = m_vtXmlText[i];
		if (p)
			delete p;
	}
	m_vtXmlText.clear();
	
	for (i = 0;i < (int)m_vtLine.size(); i++)
	{
		XMLSTATICLINE *p = m_vtLine[i];
		if (p)
		{
			int j;
			for (j = 0;j < (int)p->vtLineSegment.size(); j++)
			{
				XMLSTATICLINESEGMENT *pSegment = p->vtLineSegment[j];
				delete pSegment;
			}
			p->vtLineSegment.clear();
			delete p;
		}
	}
	m_vtLine.clear();
	m_pHotSegMent = NULL;
}

XMLTEXTHEAD *duXmlStatic::ParseFormatText(TiXmlElement *pElement)
{
	if (pElement == NULL)
		return NULL;
	
	XMLFORMATTEXT *pFormatText = new XMLFORMATTEXT;
	pFormatText->head.enType = FORMATTEXT;
	pFormatText->head.strText = GetElementText(pElement);
	pFormatText->head.size.cx = pFormatText->head.size.cy = 0;
	if (pFormatText->head.strText.empty())
	{
		delete pFormatText;
		return NULL;
	}
	
	pFormatText->strFont = ReadXmlAttributeText(pElement, _T("Font"));
	pFormatText->clrText = ReadXmlAttributeColor(pElement, _T("Color"));
	pFormatText->strHref = ReadXmlAttributeText(pElement, _T("href"));
	
	return (XMLTEXTHEAD *)pFormatText;
}

XMLTEXTHEAD *duXmlStatic::ParseNormalText(TiXmlText *pXmlText)
{
	if (pXmlText == NULL)
		return NULL;
	
	XMLNORMALTEXT *pNormalText = new XMLNORMALTEXT;
	pNormalText->head.enType = NORMALTEXT;
	pNormalText->head.strText = pXmlText->Value();
	pNormalText->head.size.cx = pNormalText->head.size.cy = 0;
	
	if (pNormalText->head.strText.empty())
	{
		delete pNormalText;
		return NULL;
	}
	
	return (XMLTEXTHEAD *)pNormalText;
}

XMLTEXTHEAD *duXmlStatic::ParseBrText(TiXmlElement *pElement)
{
	if (pElement == NULL)
		return NULL;

	XMLBRTEXT *pBrText = new XMLBRTEXT;
	pBrText->head.enType = BRTEXT;
	pBrText->head.strText = _T("");
	pBrText->head.size.cx = pBrText->head.size.cy = 0;

	return (XMLTEXTHEAD *)pBrText;
}

dustring duXmlStatic::GetElementText(TiXmlElement *pElement)
{
	dustring strText = _T("");
	TiXmlNode *pChildNode = pElement->FirstChild();
	while (pChildNode)
	{
		if (pChildNode->Type() == TiXmlNode::TEXT)
		{
			TiXmlText *pXmlText = pChildNode->ToText();
			strText += pXmlText->Value();
		}
		
		pChildNode = pChildNode->NextSibling();
	}
	
	return strText;
}

#ifdef _DEBUG
void duXmlStatic::PrintText()
{
	_TRACE(_T("Print XML TEXT ATTRIBUTE\n"));
	int i;
	for (i = 0;i < (int)m_vtXmlText.size(); i++)
	{
		XMLTEXTHEAD *p = m_vtXmlText[i];
		if (p->enType == NORMALTEXT)
		{
			_TRACE(_T("NORMALTEXT [%s] SIZE{%d,%d}\n"), p->strText.c_str(), p->size.cx, p->size.cy);
		}
		else if (p->enType == FORMATTEXT)
		{
			XMLFORMATTEXT *pFormatText = (XMLFORMATTEXT *)p;
			_TRACE(_T("FORMATTEXT [%s] SIZE{%d,%d} FONT{%s} COLOR{0x00%x} HREF{%s}\n"), p->strText.c_str(), p->size.cx, p->size.cy,
				pFormatText->strFont.c_str(), pFormatText->clrText, pFormatText->strHref.c_str());
		}
		else if (p->enType == BRTEXT)
		{
			_TRACE(_T("BR\n"));	
		}
		
	}
	
	_TRACE(_T("Print LINE ATTRIBUTE\n"));
	for (i = 0;i < (int)m_vtLine.size(); i++)
	{
		XMLSTATICLINE *p = m_vtLine[i];
		_TRACE(_T("LINE[%d] WIDTH[%d] HEIGHT[%d] SEGMENT[%d]\n"), i, p->nWidth, p->nHeight, (int)p->vtLineSegment.size());
		int j;
		for (j = 0;j < (int)p->vtLineSegment.size(); j++)
		{
			XMLSTATICLINESEGMENT *pSegment = p->vtLineSegment[j];
			LPTSTR lpString = new TCHAR[pSegment->nStringWidth + 1];
			memset(lpString, 0, sizeof(TCHAR) * (pSegment->nStringWidth + 1));
			_tcsncpy(lpString, pSegment->lpString, pSegment->nStringWidth);
			
			
			_TRACE(_T("\tSTRING{%s} STRINGLENGTH{%d} STRINGINDEX{%d} WIDTH{%d} HEIGHT{%d}\n"), lpString, 
				pSegment->nStringWidth, pSegment->nStringPosInTextHead, pSegment->nWidth, pSegment->nHeight);
				
			delete lpString;
		}
	}
}
#endif

void duXmlStatic::MeasureString(XMLTEXTHEAD *pTextHead)
{
	HFONT hFont = GetXmlTextHeadFont(pTextHead);
	HDC hDC = ::GetDC(NULL);
	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
	duRect rcText;
	rcText.SetRectEmpty();
	
	if (pTextHead->enType == BRTEXT)
	{
		::DrawText(hDC, _T("1"), 1, &rcText, DT_CALCRECT|DT_SINGLELINE);
		pTextHead->size.cx = 0;
		pTextHead->size.cy = rcText.Height();
	}
	else
	{
		::DrawText(hDC, pTextHead->strText.c_str(), pTextHead->strText.length(), &rcText, DT_CALCRECT|DT_SINGLELINE);
		pTextHead->size.cx = rcText.Width();
		pTextHead->size.cy = rcText.Height();
	}

	::SelectObject(hDC, hOldFont);
	::ReleaseDC(NULL, hDC);
}

XMLSTATICLINESEGMENT *duXmlStatic::MeasureStringBreakPos(XMLTEXTHEAD *pTextHead, int nStartPos, int nCurWidth, int nCtrlWidth)
{
	XMLSTATICLINESEGMENT *pLineSegment = new XMLSTATICLINESEGMENT;;
	LPCTSTR lpString = pTextHead->strText.c_str();
	lpString += nStartPos;
	
	HFONT hFont = GetXmlTextHeadFont(pTextHead);
	HDC hDC = ::GetDC(NULL);
	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
	duRect rcText;
	
	int nStringWidth = 0;
	int nTotalWidth = nCurWidth;
	pLineSegment->pTextHead = pTextHead;
	pLineSegment->nStringPosInTextHead = nStartPos;
	
	rcText.SetRectEmpty();
	while(nTotalWidth <= nCtrlWidth)
	{	
		pLineSegment->lpString = lpString;
		pLineSegment->nStringWidth = nStringWidth;
		pLineSegment->nWidth = rcText.Width();
		pLineSegment->nHeight = rcText.Height();
		
		nStringWidth++;
		rcText.SetRectEmpty();
		::DrawText(hDC, lpString, nStringWidth, &rcText, DT_CALCRECT|DT_SINGLELINE);
		
		if (nCurWidth + rcText.Width() > nCtrlWidth)
			break;
		
		nTotalWidth = nCurWidth + rcText.Width();
	}
	
	::SelectObject(hDC, hOldFont);
	::ReleaseDC(NULL, hDC);
	
	return pLineSegment;
}

HFONT duXmlStatic::GetXmlTextHeadFont(XMLTEXTHEAD *pTextHead)
{
	duFont *font = NULL;
	HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	if (pTextHead->enType == NORMALTEXT || pTextHead->enType == BRTEXT)
	{
		font = (duFont *)GetResObj(m_szDefaultFont, DU_RES_FONT);
	}
	else if (pTextHead->enType == FORMATTEXT)
	{
		XMLFORMATTEXT *pFormatText = (XMLFORMATTEXT *)pTextHead;
		font = (duFont *)GetResObj(pFormatText->strFont.c_str(), DU_RES_FONT);
	}
	
	if (font)
		hFont = font->GetHFont();
		
	return hFont;
}

BOOL duXmlStatic::IsHrefSegment(XMLSTATICLINESEGMENT *pSegment)
{
	if (pSegment == NULL || pSegment->pTextHead == NULL)
		return FALSE;
	
	XMLTEXTHEAD *pTextHead = pSegment->pTextHead;
	if (pTextHead->enType == NORMALTEXT)
		return FALSE;
		
	XMLFORMATTEXT *pFormatText = (XMLFORMATTEXT *)pTextHead;
	return pFormatText->strHref.empty() ? FALSE : TRUE;
}
