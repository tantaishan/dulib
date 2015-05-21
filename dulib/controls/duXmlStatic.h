//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duXmlStatic.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

enum XMLTEXTNODETYPE
{
	NORMALTEXT = 1,
	FORMATTEXT = 2,
	BRTEXT = 3,
	TYPECOUNT = 4
};

typedef struct _tagXMLTEXTHEAD
{
	XMLTEXTNODETYPE enType;
	dustring strText;
	SIZE size;
}XMLTEXTHEAD;

typedef struct _tagXMLNORMALTEXT
{
	XMLTEXTHEAD head;
}XMLNORMALTEXT;

typedef struct _tagXMLBRTEXT
{
	XMLTEXTHEAD head;
}XMLBRTEXT;

typedef struct _tagXMLFORMATTEXT
{
	XMLTEXTHEAD head;
	dustring strFont;
	COLORREF clrText;
	dustring strHref;
}XMLFORMATTEXT;

typedef struct _tagXMLSTATICLINESEGMENT
{
	XMLTEXTHEAD *pTextHead;
	LPCTSTR lpString;
	int nStringWidth;
	int nStringPosInTextHead;
	int nWidth;
	int nHeight;
}XMLSTATICLINESEGMENT;

typedef struct _tagXMLSTATICLINE
{
	int nWidth;
	int nHeight;
	vector<XMLSTATICLINESEGMENT *> vtLineSegment;
}XMLSTATICLINE;


// duXmlStatic
// 
// 
class duXmlStatic : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseMove(POINT pt);
	virtual void WINAPI SetText(LPCTSTR lpszText);
	virtual BOOL WINAPI OnSetCursor();
	virtual void WINAPI Resize(LPRECT lpRect/*=NULL*/);
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duXmlStatic"); }	

public:
	duXmlStatic();
	virtual ~duXmlStatic();
	
protected:
	void ParseTextParam();
	void Destroy();
	XMLTEXTHEAD *ParseFormatText(TiXmlElement *pElement);
	XMLTEXTHEAD *ParseNormalText(TiXmlText *pXmlText);
	XMLTEXTHEAD *ParseBrText(TiXmlElement *pElement);
	dustring GetElementText(TiXmlElement *pElement);
	void DrawXmlText(HDC hDC);
	void PaintSegMent(HDC hDC, XMLSTATICLINESEGMENT *pSegment, duRect &rcSegMent);
	void MeasureString(XMLTEXTHEAD *pTextHead);
	XMLSTATICLINESEGMENT *MeasureStringBreakPos(XMLTEXTHEAD *pTextHead, int nStartPos, int nCurWidth, int nCtrlWidth);
	HFONT GetXmlTextHeadFont(XMLTEXTHEAD *pTextHead);
	BOOL IsHrefSegment(XMLSTATICLINESEGMENT *pSegment);
	
#ifdef _DEBUG
	void PrintText();
#endif

protected:
	int m_nParagraphSpace;
	TCHAR m_szDefaultFont[MAX_NAME];
	COLORREF m_clrDefault;
	vector<XMLTEXTHEAD *> m_vtXmlText;
	vector<XMLSTATICLINE *> m_vtLine;
	XMLSTATICLINESEGMENT *m_pHotSegMent;
};
