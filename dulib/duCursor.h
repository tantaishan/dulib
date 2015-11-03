//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duCursor.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duResBase.h"
#include "duResManager.h"

#define disp_duCursor_LoadCursor				0x0F30
#define disp_duCursor_hCursor					0x0F31
#define disp_duCursor_value						0x0F32

// duCursor
//
// 
class duCursor : public duResBase
{
public:
	virtual UINT GetType() { return DU_RES_CURSOR; }
	virtual BOOL OnCreate(TiXmlElement *pElement);

	virtual LPCTSTR GetValue() { return m_strCursor; } 
	virtual void SetValue(LPCTSTR lpszCursor);
	virtual HCURSOR GetHCursor() { return m_hCursor; }
	virtual void SetHCursor(HCURSOR hCursor);

	virtual HCURSOR LoadFromFile(LPCTSTR lpszFile);
	virtual HCURSOR LoadFromMemory(PBYTE pData, int nSize);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duCursor"); }

public:
	duCursor();
	~duCursor();
	
protected:
	void Destroy();
	HANDLE CreateTempFile(LPTSTR lpszFilePathBufOut);

protected:
	TCHAR m_strCursor[MAX_NAME];
	HCURSOR m_hCursor;
};
