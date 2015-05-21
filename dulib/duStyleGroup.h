//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duStyleGroup.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duResBase.h"
#include "duResManager.h"

#define disp_duStyleGroup_AddStyle				0x0F50
#define disp_duStyleGroup_GetStyleCount			0x0F51
#define disp_duStyleGroup_GetStyle				0x0F52

// duStyleGroup
//
//
class duStyleGroup : public duResBase
{
public:
	virtual UINT WINAPI GetType() { return DU_RES_STYLEGROUP; }
	virtual BOOL WINAPI OnCreate(TiXmlElement *pElement);
	virtual void WINAPI Draw(HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha);
	virtual void WINAPI DrawNotText(HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha);
	virtual void WINAPI DrawOnlyText(HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha);
	virtual void WINAPI GrayDrawByStyle(HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha);
	virtual int WINAPI GetStyleCount();
	virtual void WINAPI AddStyle(duStyleBase *pStyleBase);
	virtual BOOL WINAPI RemoveStyle(int nIndex);
	virtual duStyleBase *WINAPI GetStyle(int nIndex);
	virtual void WINAPI CalcTextRect(UINT uState, LPCTSTR lpszText, LPSIZE lpOutSize);
	virtual BOOL WINAPI MoveUp(int nIndex);
	virtual BOOL WINAPI MoveDown(int nIndex);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duStyleGroup"); }

protected:
	void Destroy();

public:
	duStyleGroup();
	~duStyleGroup();

private:
	vector<duStyleBase *> m_vtStylelist;
};

duStyleBase *WINAPI CreateChildStyle(LPCTSTR lpszType);

