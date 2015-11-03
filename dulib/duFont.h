//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duFont.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duResBase.h"
#include "duResManager.h"

#define disp_duFont_height						0x0F10
#define disp_duFont_weight						0x0F11
#define disp_duFont_italic						0x0F12
#define disp_duFont_underline					0x0F13
#define disp_duFont_strikeout					0x0F14
#define disp_duFont_font						0x0F15
#define disp_duFont_hFont						0x0F16
#define disp_duFont_CreateFont					0x0F17

// duFont
//
// ×ÖÌå(Font)

class duFont : public duResBase
{
public:
	virtual UINT GetType() { return DU_RES_FONT; }
	virtual BOOL OnCreate(TiXmlElement *pElement);
	
	virtual int GetHeight() { return m_nHeight; }
	virtual void SetHeight(int nHeight) { m_nHeight = nHeight; }
	virtual int GetWeight() { return m_nWeight; }
	virtual void SetWeight(int nWeight) { m_nWeight = nWeight; }
	virtual BOOL IsItalic() { return m_fItalic; }
	virtual void SetItalic(BOOL fItalic) { m_fItalic = fItalic; }
	virtual BOOL IsUnderline() { return m_fUnderline; } 
	virtual void SetUnderline(BOOL fUnderline) { m_fUnderline = fUnderline; }
	virtual BOOL IsStrikeOut() { return m_fStrikeOut; }
	virtual void SetStrikeOut(BOOL fStrikeOut) { m_fStrikeOut = fStrikeOut; }
	virtual LPCTSTR GetFontFace() { return m_szFace; }
	virtual void SetFontFace(LPCTSTR lpszFace);
	virtual HFONT GetHFont() { return m_hFont; }
	virtual void SetHFont(HFONT hFont) { m_hFont = hFont; }
	
	virtual HFONT CreateFont();

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duFont"); }

public:
	duFont();
	~duFont();

protected:
	int	   m_nHeight;
	int	   m_nWeight;
	BOOL   m_fItalic;
	BOOL   m_fUnderline;
	BOOL   m_fStrikeOut;
	TCHAR  m_szFace[MAX_NAME];
	HFONT  m_hFont;
};
