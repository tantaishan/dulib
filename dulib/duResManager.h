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
class duResManager
{
public:
	virtual BOOL AddResObj(duResBase *pNewRes);
	virtual BOOL DeleteResObj(LPCTSTR lpszResName, UINT uResType);
	
	virtual duResBase *GetResObj(LPCTSTR lpszResName, UINT uResType);
	virtual duResBase *CreateResObj(LPCTSTR lpszResName, UINT uResType);
	virtual duStyleBase *CreateStyle(UINT uStyleType);
	virtual int GetResObjCount(UINT uResType);
	virtual duResBase *GetResObjByIndex(int iIndex, UINT uResType);
	virtual BOOL ResRename(LPCTSTR lpszResName, UINT uResType, LPCTSTR lpszResNewName);
	virtual BOOL EraseResObjFromMap(LPCTSTR lpszResName, UINT uResType);
	virtual duStyleBase *CloneChildStyle(duStyleBase *pStyleBase);
	virtual duStyleGroup *CloneStyle(duStyleGroup *pStyleGroup, LPCTSTR lpszResName);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duResManager"); }

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
