//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duResManager.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duResManager.h"
#include "duImage.h"
#include "duFont.h"
#include "duCursor.h"
#include "duRgn.h"
#include "duStyleGroup.h"
#include "duWindowManager.h"

duResManager::duResManager()
{
	Init();
}

duResManager::~duResManager()
{
	Destroy();
}

void duResManager::Init()
{
	int i = 0;
	for (i = 0; i < 5; i++)
		m_pMapResBase[i] = new map_ResBase;
}

void duResManager::Destroy()
{
	int i;
	for (i = 0; i < 5; i++)
	{
		map_ResBase *pMapResBase = m_pMapResBase[i];
		if (pMapResBase == NULL)
			continue;
		
		map_ResBase::iterator iterResBase = pMapResBase->begin();
		for (; iterResBase != pMapResBase->end(); iterResBase++)
		{
			duResBase *pResObj = iterResBase->second;
			if (pResObj)
			{
				pResObj->FinalRelease();
				pResObj = NULL;
			}
		}
		pMapResBase->clear();
		delete pMapResBase;
		m_pMapResBase[i] = NULL;
	}
}

BOOL duResManager::OnCreate(TiXmlElement *pElement)
{
	if (pElement == NULL)
		return NULL;
	
	TiXmlHandle xmlHandler(pElement);
	TiXmlElement *pChildElement = NULL;
	
	pChildElement = xmlHandler.FirstChildElement(_T("dsfont")).Element(); 
	LoadResObj(pChildElement, DU_RES_FONT);

	pChildElement = xmlHandler.FirstChildElement(_T("dscursor")).Element();
	LoadResObj(pChildElement, DU_RES_CURSOR);

	pChildElement = xmlHandler.FirstChildElement(_T("dsimage")).Element();
	LoadResObj(pChildElement, DU_RES_IMAGE);

	pChildElement = xmlHandler.FirstChildElement(_T("dsstyle")).Element();
	LoadResObj(pChildElement, DU_RES_STYLEGROUP);

	pChildElement = xmlHandler.FirstChildElement(_T("dsrgn")).Element(); 
	LoadResObj(pChildElement, DU_RES_RGN);

	return TRUE;
}

BOOL duResManager::LoadResObj(TiXmlElement *pElement, UINT uResType)
{ 
	if (pElement == NULL)
		return FALSE;

	duResBase *pResObj = NULL;
	TCHAR szName[MAX_NAME];
	TiXmlElement *pTemp = pElement->FirstChildElement();
	while (pTemp)
	{
		ZeroMemory(szName, MAX_NAME * sizeof(TCHAR));
		ReadXmlAttributeText(pTemp, _T("name"), szName, MAX_NAME);
		pResObj = CreateResObj(szName, uResType);
		if (pResObj)
		{
			pResObj->OnCreate(pTemp);
			AddResObj(pResObj);
		}

		pTemp = pTemp->NextSiblingElement();
	}

	return TRUE;
}

BOOL WINAPI duResManager::AddResObj(duResBase *pNewResBase)
{
	if (pNewResBase == NULL)
		return FALSE;

	LPCTSTR lpszName = pNewResBase->GetName();
	UINT uResType = pNewResBase->GetType();

	if (GetResObj(lpszName, uResType))
	{
		_TRACE(_T("AddResObj [%s] failed\n"), lpszName);
		return FALSE;
	}

	map_ResBase *pMapResBase = m_pMapResBase[uResType-1];
	if (pMapResBase == NULL)
		return FALSE;
	
	dustring strName = lpszName;
	pMapResBase->insert(pair<dustring, duResBase *>(strName, pNewResBase));
	return TRUE;
}

duResBase *WINAPI duResManager::GetResObj(LPCTSTR lpszResName, UINT uResType)
{
	if (lpszResName == NULL)
		return NULL;

	dustring strName = lpszResName;
	duResBase *pResObj = NULL;
	map_ResBase *pMapResBase = m_pMapResBase[uResType-1];
	if (pMapResBase)
	{
		map_ResBase::iterator iterResBase = pMapResBase->find(strName);
		if (iterResBase != pMapResBase->end())
			pResObj = iterResBase->second;
	}

	return pResObj;
}

BOOL WINAPI duResManager::DeleteResObj(LPCTSTR lpszResName, UINT uResType)
{
	if (lpszResName == NULL)
		return FALSE;

	dustring strName = lpszResName;
	duResBase *pResObj = NULL;
	map_ResBase *pMapResBase = m_pMapResBase[uResType-1];
	if (pMapResBase)
	{
		map_ResBase::iterator iterResBase = pMapResBase->find(strName);
		if (iterResBase != pMapResBase->end())
		{
			duResBase *pResBase = iterResBase->second;
			delete pResBase;
			pMapResBase->erase(iterResBase);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL WINAPI duResManager::EraseResObjFromMap(LPCTSTR lpszResName, UINT uResType)
{
	if (lpszResName == NULL)
		return FALSE;

	dustring strName = lpszResName;
	duResBase *pResObj = NULL;
	map_ResBase *pMapResBase = m_pMapResBase[uResType-1];
	if (pMapResBase)
	{
		map_ResBase::iterator iterResBase = pMapResBase->find(strName);
		if (iterResBase != pMapResBase->end())
		{
			duResBase *pResBase = iterResBase->second;
			pMapResBase->erase(iterResBase);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL WINAPI duResManager::ResRename(LPCTSTR lpszResName, UINT uResType, LPCTSTR lpszResNewName)
{
	if (lpszResName == NULL || lpszResNewName == NULL)
		return FALSE;

	dustring strName = lpszResName;
	dustring strNewName = lpszResNewName;
	duResBase *pResObj = NULL;

	map_ResBase * pMapResBase = m_pMapResBase[uResType - 1];
	if (pMapResBase)
	{
		map_ResBase::iterator iterNewResBase = pMapResBase->find(strNewName);
		if (iterNewResBase != pMapResBase->end())
			return FALSE;

		map_ResBase::iterator iterResBase = pMapResBase->find(strName);
		if (iterResBase != pMapResBase->end())
		{
			duResBase *pResBase = iterResBase->second;
			pResBase->SetName(lpszResNewName);
			pMapResBase->erase(iterResBase);
			AddResObj(pResBase);
			return TRUE;
		}
	}
	return FALSE;
}

duResBase *WINAPI duResManager::CreateResObj(LPCTSTR lpszResName, UINT uResType)
{
	if (lpszResName == NULL)
		return FALSE;

	duResBase *pResObj = NULL;
	switch(uResType)
	{
	case DU_RES_FONT:
		pResObj = new duFont;
		break;
	case DU_RES_CURSOR:
		pResObj = new duCursor;
		break;
	case DU_RES_IMAGE:
		pResObj = new duImage;
		break;
	case DU_RES_RGN:
		pResObj = new duRgn;
		break;
	case DU_RES_STYLEGROUP:
		pResObj = new duStyleGroup;
		break;

	default:
		break;
	}

	if (pResObj)
	{
		pResObj->SetTypeInfo(m_pWindowManager->GetTypeInfoByName(pResObj->GetTypeInfoName()));
		pResObj->SetResManager(this);
		pResObj->SetName(lpszResName);
	}

	return pResObj;
}

duStyleBase *WINAPI duResManager::CreateStyle(UINT uStyleType)
{
	duStyleBase *pStyleBase = NULL;
	switch (uStyleType)
	{
	case STYLE_IMAGE:
		pStyleBase = new duImageStyle();
		break;
	case STYLE_TEXT:
		pStyleBase = new duTextStyle();
		break;
	case STYLE_RECT:
		pStyleBase = new duRectStyle();
		break;
	case STYLE_LINE:
		pStyleBase = new duLineStyle();
		break;
	default:
		break;
	}
	
	if (pStyleBase)
	{
		pStyleBase->SetTypeInfo(m_pWindowManager->GetTypeInfoByName(pStyleBase->GetTypeInfoName()));
		pStyleBase->SetResManager(this);
	}

	return pStyleBase;
}

int WINAPI duResManager::GetResObjCount(UINT uResType)
{
	map_ResBase *pMapResBase = m_pMapResBase[uResType-1];
	if (pMapResBase)
		return (int)pMapResBase->size();

	return 0;
}

duResBase *WINAPI duResManager::GetResObjByIndex(int iIndex, UINT uResType)
{
	map_ResBase *pMapResBase = m_pMapResBase[uResType-1];
	if (pMapResBase)
	{
		int i = 0;
		map_ResBase::iterator iter = pMapResBase->begin();
		for (; iter != pMapResBase->end(); iter++)
		{
			if (i == iIndex)
				return iter->second;
			i++;
		}
	}

	return NULL;
}

duStyleBase *WINAPI duResManager::CloneChildStyle(duStyleBase *pStyleBase)
{
	if (pStyleBase == NULL)
		return NULL;

	UINT uType = pStyleBase->GetType();

	duStyleBase *pNewStyle = CreateStyle(uType);
	pNewStyle->SetState(pStyleBase->GetState());
	pNewStyle->SetLeft(pStyleBase->GetLeft());
	pNewStyle->SetTop(pStyleBase->GetTop());
	pNewStyle->SetRight(pStyleBase->GetRight());
	pNewStyle->SetBottom(pStyleBase->GetBottom());
	pNewStyle->SetWidth(pStyleBase->GetWidth());
	pNewStyle->SetHeight(pStyleBase->GetHeight());
	
	if (uType == STYLE_IMAGE)
	{
		duImageStyle *pImageStyle = (duImageStyle *)pStyleBase;
		duImageStyle *pNewImageStyle = (duImageStyle *)pNewStyle;

		pNewImageStyle->SetSelectLeft(pImageStyle->GetSelectLeft());
		pNewImageStyle->SetSelectTop(pImageStyle->GetSelectTop());
		pNewImageStyle->SetSelectWidth(pImageStyle->GetSelectWidth());
		pNewImageStyle->SetSelectHeight(pImageStyle->GetSelectHeight());

		pNewImageStyle->SetLeftPart(pImageStyle->GetLeftPart());
		pNewImageStyle->SetTopPart(pImageStyle->GetTopPart());
		pNewImageStyle->SetRightPart(pImageStyle->GetRightPart());
		pNewImageStyle->SetBottomPart(pImageStyle->GetBottomPart());
		pNewImageStyle->SetPicFile(pImageStyle->GetPicFile());
		pNewImageStyle->SetPaintMode(pImageStyle->GetPaintMode());

		return pNewImageStyle;
	}
	else if (uType == STYLE_TEXT)
	{
		duTextStyle *pTextStyle = (duTextStyle *)pStyleBase;
		duTextStyle *pNewTextStyle = (duTextStyle *)pNewStyle;

		pNewTextStyle->SetFont(pTextStyle->GetFont());
		pNewTextStyle->SetHAlign(pTextStyle->GetHAlign());
		pNewTextStyle->SetVAlign(pTextStyle->GetVAlign());
		pNewTextStyle->SetTextColor(pTextStyle->GetTextColor());
		pNewTextStyle->SetSingleLine(pTextStyle->IsSingleLine());

		return pNewTextStyle;
	}
	else if (uType == STYLE_RECT)
	{
		duRectStyle *pRectStyle = (duRectStyle *)pStyleBase;
		duRectStyle *pNewRectStyle = (duRectStyle *)pNewStyle;

		pNewRectStyle->SetFillRect(pRectStyle->IsFillRect());
		pNewRectStyle->SetFillColor(pRectStyle->GetFillColor());

		pNewRectStyle->SetDrawBorder(pRectStyle->IsDrawBorder());
		pNewRectStyle->SetBorderColor(pRectStyle->GetBorderColor());
		pNewRectStyle->SetBorderWidth(pRectStyle->GetBorderWidth());
		pNewRectStyle->SetBorderType(pRectStyle->GetBorderType());

		return pNewRectStyle;
	}
	else if (uType == STYLE_LINE)
	{
		duLineStyle *pLineStyle = (duLineStyle *)pStyleBase;
		duLineStyle *pNewLineStyle = (duLineStyle *)pNewStyle;

		pNewLineStyle->SetLineColor(pLineStyle->GetLineColor());
		pNewLineStyle->SetLineType(pLineStyle->GetLineType());
		pNewLineStyle->SetLineWidth(pLineStyle->GetLineWidth());
		POINT ptStart;
		pLineStyle->GetStartPoint(&ptStart);
		pNewLineStyle->SetStartPoint(&ptStart);
		POINT ptEnd;
		pLineStyle->GetEndPoint(&ptEnd);
		pNewLineStyle->SetEndPoint(&ptEnd);

		return pNewLineStyle;
	}

	pNewStyle->FinalRelease();
	return NULL;
}

duStyleGroup *WINAPI duResManager::CloneStyle(duStyleGroup *pStyleGroup, LPCTSTR lpszResName)
{
	if (lpszResName == NULL || *lpszResName == NULL)
		return NULL;

	duStyleGroup *pNewStyleGroup = (duStyleGroup *)CreateResObj(lpszResName, DU_RES_STYLEGROUP);
	if (pNewStyleGroup == NULL)
		return NULL;

	int nStyleCount = pStyleGroup->GetStyleCount();
	int i;
	for (i = 0;i < nStyleCount; i++)
	{
		duStyleBase *pStyleBase = pStyleGroup->GetStyle(i);
		if (pStyleBase)
		{
			duStyleBase *pNewStyleBase = CloneChildStyle(pStyleBase);
			if (pNewStyleBase)
				pNewStyleGroup->AddStyle(pNewStyleBase);
		}
	}

	return pNewStyleGroup;
}
