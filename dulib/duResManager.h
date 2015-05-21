//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duResManager.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duResBase.h"
#include "duStyle.h"

#define disp_duResManager_AddResObj				0x0F01
#define disp_duResManager_DeleteResObj			0x0F02
#define disp_duResManager_GetResObj				0x0F03
#define disp_duResManager_CreateResObj			0x0F04
#define disp_duResManager_CreateStyle			0x0F05
#define disp_duResManager_GetResObjCount      	0x0F06
#define disp_duResManager_GetResObjByIndex    	0x0F07

typedef map<dustring, duResBase *> map_ResBase;

class duWindowManager;
class duStyleGroup;
// duReManager
//
//
class duResManager : public duObject
{
public:
	virtual BOOL WINAPI AddResObj(duResBase *pNewRes);
	virtual BOOL WINAPI DeleteResObj(LPCTSTR lpszResName, UINT uResType);
	
	virtual duResBase *WINAPI GetResObj(LPCTSTR lpszResName, UINT uResType);
	virtual duResBase *WINAPI CreateResObj(LPCTSTR lpszResName, UINT uResType);
	virtual duStyleBase *WINAPI CreateStyle(UINT uStyleType);
	virtual int WINAPI GetResObjCount(UINT uResType);
	virtual duResBase *WINAPI GetResObjByIndex(int iIndex, UINT uResType);
	virtual BOOL WINAPI ResRename(LPCTSTR lpszResName, UINT uResType, LPCTSTR lpszResNewName);
	virtual BOOL WINAPI EraseResObjFromMap(LPCTSTR lpszResName, UINT uResType);
	virtual duStyleBase *WINAPI CloneChildStyle(duStyleBase *pStyleBase);
	virtual duStyleGroup *WINAPI CloneStyle(duStyleGroup *pStyleGroup, LPCTSTR lpszResName);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duResManager"); }

public:
	duResManager();
	~duResManager();
	
public:
	BOOL OnCreate(TiXmlElement *pElement);
	duWindowManager *GetWindowManager() { return m_pWindowManager; }
	void SetWindowManager(duWindowManager *pWindowManager) { m_pWindowManager = pWindowManager; }
	
protected:
	void Init();
	void Destroy();
	BOOL LoadResObj(TiXmlElement *pElement, UINT uResType);

protected:
	map_ResBase *m_pMapResBase[5];
	duWindowManager *m_pWindowManager;
};
