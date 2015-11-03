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
	virtual UINT GetType() { return DU_RES_STYLEGROUP; }
	virtual BOOL OnCreate(TiXmlElement *pElement);
	virtual void Draw(HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha);
	virtual void DrawNotText(HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha);
	virtual void DrawOnlyText(HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha);
	virtual void GrayDrawByStyle(HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha);
	virtual int GetStyleCount();
	virtual void AddStyle(duStyleBase *pStyleBase);
	virtual BOOL RemoveStyle(int nIndex);
	virtual duStyleBase *GetStyle(int nIndex);
	virtual void CalcTextRect(UINT uState, LPCTSTR lpszText, LPSIZE lpOutSize);
	virtual BOOL MoveUp(int nIndex);
	virtual BOOL MoveDown(int nIndex);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duStyleGroup"); }

protected:
	void Destroy();

public:
	duStyleGroup();
	~duStyleGroup();

private:
	vector<duStyleBase *> m_vtStylelist;
};

duStyleBase *CreateChildStyle(LPCTSTR lpszType);

