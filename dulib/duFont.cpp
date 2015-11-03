//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duFont.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "duFont.h"

duFont::duFont() :
	 m_nHeight(12)
	,m_nWeight(FW_NORMAL)
	,m_fItalic(FALSE)
	,m_fUnderline(FALSE)
	,m_fStrikeOut(FALSE)
	,m_hFont(NULL)
{
	ZeroMemory(m_szFace, MAX_NAME * sizeof(TCHAR));
}

duFont::~duFont()
{
	SAFE_DELETE_OBJECT(m_hFont);
}

BOOL duFont::OnCreate(TiXmlElement *pElement)
{
	if (pElement == NULL)
		return FALSE;

	m_nHeight = ReadXmlAttributeInt(pElement, _T("height"));
	m_nWeight = ReadXmlAttributeInt(pElement, _T("weight"));
	ReadXmlAttributeBOOL(pElement, _T("italic"), &m_fItalic);
	ReadXmlAttributeBOOL(pElement, _T("underline"), &m_fUnderline);
	ReadXmlAttributeBOOL(pElement, _T("strikeout"), &m_fStrikeOut);
	ReadXmlAttributeText(pElement, _T("fontface"), m_szFace, MAX_NAME);

	CreateFont();
	return TRUE;
}

void duFont::SetFontFace(LPCTSTR lpszFace)
{
	if (lpszFace)
		_tcsncpy(m_szFace, lpszFace, MAX_NAME);
	else
		ZeroMemory(m_szFace, MAX_NAME * sizeof(TCHAR));
}

HFONT duFont::CreateFont()
{
	SAFE_DELETE_OBJECT(m_hFont);

	m_hFont = ::CreateFont(-m_nHeight, NULL, NULL, NULL, m_nWeight, m_fItalic, m_fUnderline, m_fStrikeOut,
		GB2312_CHARSET, OUT_STRING_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, 0x22, m_szFace);

	return m_hFont;
}
