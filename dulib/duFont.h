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
	virtual UINT WINAPI GetType() { return DU_RES_FONT; }
	virtual BOOL WINAPI OnCreate(TiXmlElement *pElement);
	
	virtual int WINAPI GetHeight() { return m_nHeight; }
	virtual void WINAPI SetHeight(int nHeight) { m_nHeight = nHeight; }
	virtual int WINAPI GetWeight() { return m_nWeight; }
	virtual void WINAPI SetWeight(int nWeight) { m_nWeight = nWeight; }
	virtual BOOL WINAPI IsItalic() { return m_fItalic; }
	virtual void WINAPI SetItalic(BOOL fItalic) { m_fItalic = fItalic; }
	virtual BOOL WINAPI IsUnderline() { return m_fUnderline; } 
	virtual void WINAPI SetUnderline(BOOL fUnderline) { m_fUnderline = fUnderline; }
	virtual BOOL WINAPI IsStrikeOut() { return m_fStrikeOut; }
	virtual void WINAPI SetStrikeOut(BOOL fStrikeOut) { m_fStrikeOut = fStrikeOut; }
	virtual LPCTSTR WINAPI GetFontFace() { return m_szFace; }
	virtual void WINAPI SetFontFace(LPCTSTR lpszFace);
	virtual HFONT WINAPI GetHFont() { return m_hFont; }
	virtual void WINAPI SetHFont(HFONT hFont) { m_hFont = hFont; }
	
	virtual HFONT WINAPI CreateFont();

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duFont"); }

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
