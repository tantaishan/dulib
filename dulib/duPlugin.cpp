//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duPlugin.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "duPlugin.h"
#include "duStyle.h"
#include "duWindowManager.h"
#include "duResManager.h"
#include "duHwndObj.h"

extern set<duPlugin *> *g_setValid;

duPlugin *CreateDirectUIControl(LPCTSTR lpszType);

duPlugin::duPlugin() :
	 m_fVisible(TRUE)
	,m_fFocus(FALSE)
	,m_fDisable(FALSE)
	,m_fCached(FALSE)
	,m_fOpaque(FALSE)
	,m_hWnd(NULL)
	,m_uZorder(0)
	,m_uState(DU_STATE_NORMAL)
	,m_pProperty(NULL)
	,m_nLeft(0)
	,m_nTop(0)
	,m_nRight(0)
	,m_nBottom(0)
	,m_nHorzanchor(0)
	,m_nVertanchor(0)
	,m_nWidth(0)
	,m_nHeight(0)
	,m_pParent(NULL)
	,m_pFirstChild(NULL)
	,m_pLastChild(NULL)
	,m_pNextSibling(NULL)
	,m_pPreSibling(NULL)
	,m_lRef(0)
	,m_strText(_T(""))
	,m_lParam(0)
	,m_nParam64(0)
	,m_nAlpha(255)
	,m_hBitmap(NULL)
	,m_nTabOrder(0)
{
	m_rect.SetRectEmpty();
	ZeroMemory(m_strType, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_strName, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_strStyle, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_strCursor, MAX_NAME * sizeof(TCHAR));
}

duPlugin::~duPlugin()
{
	SAFE_DELETE(m_pProperty);
	if (m_hBitmap)
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}

	Plugin_DeleteValid(this);
}

void duPlugin::SetName(LPCTSTR lpszName)
{
	if (lpszName)
		_tcsncpy(m_strName, lpszName, MAX_NAME);
	else
		ZeroMemory(m_strName, MAX_NAME * sizeof(TCHAR));
}

void duPlugin::SetStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_strStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_strStyle, MAX_NAME * sizeof(TCHAR));
}

void duPlugin::SetText(LPCTSTR lpszText)
{
	if (lpszText)
		m_strText = lpszText;
	else
		m_strText = _T("");
}

void duPlugin::SetCursor(LPCTSTR lpszCursor)
{
	if (lpszCursor)
		_tcsncpy(m_strCursor, lpszCursor, MAX_NAME);
	else
		ZeroMemory(m_strCursor, MAX_NAME * sizeof(TCHAR));
}

void duPlugin::SetRect(LPRECT lpRect)
{
	if (lpRect)
		::CopyRect(&m_rect, lpRect);
}

void duPlugin::SetCachedBitmap(HBITMAP hBitmap)
{
	if (m_hBitmap)
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
	
	m_hBitmap = hBitmap;
}

int duPlugin::GetPropertyCount()
{
	if (m_pProperty == NULL)
		return 0;

	return m_pProperty->GetPropertyCount();
}

LPCTSTR duPlugin::GetPropertyName(int nNameIndex)
{
	if (m_pProperty == NULL)
		return NULL;

	return m_pProperty->GetPropertyName(nNameIndex);
}

int duPlugin::GetChildCount()
{
	int c = 0;
	duPlugin *pTemp = m_pFirstChild;
	while(pTemp) {
		++c;
		pTemp = pTemp->GetNextSibling();
	}

	return c;
}

BOOL duPlugin::RegisterProperty(LPCTSTR lpszPropName, duVARIANT_TYPE vt, void *duVariant)
{
	if (m_pProperty)
		return m_pProperty->AddProperty(lpszPropName, vt, duVariant);
	return FALSE;
}

void duPlugin::Resize(LPRECT lpRect/* = NULL */)
{
	duPlugin *pParent = GetParent();
	if (pParent)
	{
		RECT rectParent;
		if (lpRect)
			CopyRect(&rectParent, lpRect);
		else
			pParent->GetRect(&rectParent);

		MakeRectExt(m_nLeft, m_nTop, m_nRight, m_nBottom, m_nHorzanchor, m_nVertanchor, 
			m_nWidth, m_nHeight, &rectParent, &m_rect);
	}
	else
	{
		duRect rcClient;
		::GetClientRect(m_hWnd, &rcClient);
		::CopyRect(&m_rect, &rcClient);
	}

	duPlugin *pChild = GetFirstChild();
	while (pChild)
	{
		pChild->Resize(NULL);
		pChild = pChild->GetNextSibling();
	}
}

BOOL duPlugin::OnSetCursor()
{
	LPCTSTR lpszCurName = GetCursor();
	duCursor *pCursor = (duCursor *)GetResObj(lpszCurName, DU_RES_CURSOR);
	if (pCursor == NULL)
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		return TRUE;
	}

	HCURSOR hCursor = pCursor->GetHCursor();
	if (hCursor == NULL) 
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		return TRUE;
	}
	::SetCursor(hCursor);
	return TRUE;
}

duResBase *duPlugin::GetResObj(LPCTSTR lpszResName, UINT uResType)
{
	duResManager *pResManager = GetResManager(m_hWnd);
	if (pResManager)
		return pResManager->GetResObj(lpszResName, uResType);
	
	return NULL;
}

duPlugin *duPlugin::GetPluginByName(LPCTSTR lpszName)
{
	if (lpszName == NULL)
		return NULL;
	
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager)
		return pCtrlManager->GetPluginByName(lpszName);
	
	return NULL;
}

BOOL duPlugin::OnCreate(TiXmlElement *pElement, duCtrlManager *pCtrlManager)
{
	if (pElement == NULL || pCtrlManager == NULL)
		return FALSE;

	m_hWnd = pCtrlManager->GetHwnd();
	_tcsncpy(m_strType, pElement->Value(), MAX_NAME);

	ReadXmlAttributeText(pElement, _T("name"), m_strName, MAX_NAME);
	ReadXmlAttributeText(pElement, _T("style"), m_strStyle, MAX_NAME);
	ReadXmlAttributeText(pElement, _T("cursor"), m_strCursor, MAX_NAME);
	m_strText = ReadXmlAttributeText(pElement, _T("text"));

	ReadXmlAttributeBOOL(pElement, _T("visible"), &m_fVisible);
	ReadXmlAttributeBOOL(pElement, _T("cached"), &m_fCached);
	ReadXmlAttributeBOOL(pElement, _T("opaque"), &m_fOpaque);
	
	m_uZorder = ReadXmlAttributeInt(pElement, _T("zorder"));
	m_nLeft = ReadXmlAttributeInt(pElement, _T("left"));
	if (m_nLeft == 0)
	{
		TCHAR strLeft[MAX_NAME];
		ZeroMemory(strLeft, MAX_NAME * sizeof(TCHAR));
		ReadXmlAttributeText(pElement, _T("left"), strLeft, MAX_NAME);
		if (lstrcmpi(strLeft, _T("center")) == 0)
			m_nLeft = DU_HCENTER;
	}
	
	TCHAR strAlpha[MAX_NAME];
	ZeroMemory(strAlpha, MAX_NAME * sizeof(TCHAR));
	ReadXmlAttributeText(pElement, _T("alpha"), strAlpha, MAX_NAME);
	if (_tcslen(strAlpha) != 0)
		m_nAlpha = _tstoi(strAlpha);
	
	m_nTop = ReadXmlAttributeInt(pElement, _T("top"));
	if (m_nTop == 0)
	{
		TCHAR strTop[MAX_NAME];
		ZeroMemory(strTop, MAX_NAME * sizeof(TCHAR));
		ReadXmlAttributeText(pElement, _T("top"), strTop, MAX_NAME);
		if (lstrcmpi(strTop, _T("center")) == 0)
			m_nTop = DU_VCENTER;
	}

	m_nRight = ReadXmlAttributeInt(pElement, _T("right"));
	m_nBottom = ReadXmlAttributeInt(pElement, _T("bottom"));
	m_nHorzanchor = ReadXmlAttributeInt(pElement, _T("horzanchor"));
	m_nVertanchor = ReadXmlAttributeInt(pElement, _T("vertanchor"));

	if (m_nHorzanchor == 1) // percent
	{
		TCHAR strWidth[MAX_NAME];
		ZeroMemory(strWidth, MAX_NAME * sizeof(TCHAR));
		ReadXmlAttributeText(pElement, _T("width"), strWidth, MAX_NAME);

		_stscanf(strWidth, _T("%d%%"), &m_nWidth);
	}
	else
	{
		m_nWidth = ReadXmlAttributeInt(pElement, _T("width"));
	}
	
	if (m_nVertanchor == 1)
	{
		TCHAR strHeight[MAX_NAME];
		ZeroMemory(strHeight, MAX_NAME * sizeof(TCHAR));
		ReadXmlAttributeText(pElement, _T("height"), strHeight, MAX_NAME);

		_stscanf(strHeight, _T("%d%%"), &m_nHeight);
	}
	else
	{
		m_nHeight = ReadXmlAttributeInt(pElement, _T("height"));
	}

	m_nTabOrder = ReadXmlAttributeInt(pElement, _T("taborder"));

	CallProperty(pElement);

	if (m_pParent)
	{
		Resize(NULL);
		if (GetZorder() == 0)
			SetZorder(m_pParent->GetZorder() + 1);
	}
	else
	{
		duRect rectT(0, 0, m_nWidth, m_nHeight);
		SetRect(&rectT);
	}
	
	Plugin_AddValid(this);
	//pCtrlManager->AddName(this);
	
	TiXmlElement *pChildElement = pElement->FirstChildElement();
	while (pChildElement)
	{
		CreateChildPlugin(pChildElement, pCtrlManager);
		pChildElement = pChildElement->NextSiblingElement();
	}

	return TRUE;
}

BOOL duPlugin::CreateChildPlugin(TiXmlElement *pElement, duCtrlManager *pCtrlManager)
{
	if (pElement == NULL || pCtrlManager == NULL)
		return FALSE;

	duPlugin *pNewPlugin = CreateDirectUIControl(pElement->Value());
	if (pNewPlugin == NULL)
		return FALSE;

	duWindowManager *pWinManager = pCtrlManager->GetWindowManager();
	LPCTSTR lpszTypeName = pNewPlugin->GetTypeInfoName();

	Plugin_SetParent(pNewPlugin, this, NULL);
	pNewPlugin->OnCreate(pElement, pCtrlManager);

	return TRUE;
}

BOOL duPlugin::CallProperty(TiXmlElement *pElement)
{
	if (pElement == NULL)
		return FALSE;

	m_pProperty = new duPropertyGroup(pElement);
	if (m_pProperty)
	{
		this->RegisterControlProperty();
		m_pProperty->AccessXml();
		return TRUE;
	}
	
	return FALSE;
}

TiXmlElement *duPlugin::GetXmlElement()
{
	if (m_pProperty)
		return m_pProperty->GetXML();
	return NULL;
}

HBITMAP duPlugin::GetBkHBitmap()
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return NULL;
	
	return pCtrlManager->GetBkHBitmap(this);
}

HBITMAP duPlugin::GetSnapshot()
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return NULL;
	
	return pCtrlManager->GetSnapshot(this);
}

extern "C"
{

duPlugin *Plugin_SetParent(duPlugin *pPlugin, duPlugin *pParent, LPCTSTR lpszPre)
{
	if (pPlugin == NULL)
		return NULL;
	
	duPlugin *pOldParent = pPlugin->GetParent();
	if (pOldParent)
	{
		if (pParent == NULL || !Plugin_IsValid(pParent) || !Plugin_IsValid(pPlugin))
			return NULL;
		
		//
		// 从pOldParent移除
		//
		duPlugin *pOldPre = pPlugin->GetPreSibling();
		duPlugin *pOldNext = pPlugin->GetNextSibling();
			
		pPlugin->SetPreSibling(NULL);
		pPlugin->SetNextSibling(NULL);
			
		if (pOldPre)
			pOldPre->SetNextSibling(pOldNext);
				
		if (pOldNext)
			pOldNext->SetPreSibling(pOldPre);
			
		if (pOldParent->GetFirstChild() == pPlugin)
			pOldParent->SetFirstChild(pOldNext);
			
		if (pOldParent->GetLastChild() == pPlugin)
			pOldParent->SetLastChild(pOldPre);

		//
		// 更改父控件
		//			
		pPlugin->SetParent(pParent);
		duPlugin *pPre = pPlugin->GetPluginByName(lpszPre);
		if (pPre == NULL || !Plugin_IsValid(pPre))
		{
			duPlugin *pNewNext = pParent->GetFirstChild();
			pPlugin->SetNextSibling(pNewNext);
			if (pNewNext)
				pNewNext->SetPreSibling(pPlugin);
			
			pParent->SetFirstChild(pPlugin);
			if (pParent->GetLastChild() == NULL)
				pParent->SetLastChild(pPlugin);
		}
		else
		{
			duPlugin *pNewNext = pPre->GetNextSibling();
			pPre->SetNextSibling(pPlugin);
			if (pNewNext)
				pNewNext->SetPreSibling(pPlugin);
			
			pPlugin->SetPreSibling(pPre);
			pPlugin->SetNextSibling(pNewNext);
			
			if (pParent->GetLastChild() == pPre)
				pParent->SetLastChild(pPlugin);
		}
	}
	else
	{
		if (pParent == NULL)
			return NULL;
		
		duPlugin *pFirstSibling = pParent->GetFirstChild();
		duPlugin *pLastSibling  = pParent->GetLastChild();

		pPlugin->SetPreSibling(NULL);
		pPlugin->SetNextSibling(NULL);
		pPlugin->SetFirstChild(NULL);
		pPlugin->SetLastChild(NULL);
		pPlugin->SetParent(pParent);

		if (pFirstSibling)
		{
			pParent->SetLastChild(pPlugin);
			pLastSibling->SetNextSibling(pPlugin);
			pPlugin->SetPreSibling(pLastSibling);
		}
		else
		{
			pParent->SetFirstChild(pPlugin);
			pParent->SetLastChild(pPlugin);
		}
	}
	
	return pOldParent;
}

BOOL Plugin_IsValid(duPlugin *pPlugin)
{
	if (pPlugin == NULL)
		return FALSE;
	
	set<duPlugin *>::const_iterator iter = g_setValid->find(pPlugin);
	if (iter != g_setValid->end())
		return TRUE;

	return FALSE;
}

void Plugin_AddValid(duPlugin *pPlugin)
{
	if (pPlugin == NULL)
		return;

	g_setValid->insert(pPlugin);
}

void Plugin_DeleteValid(duPlugin *pPlugin)
{
	if (pPlugin == NULL)
		return;

	g_setValid->erase(pPlugin);
}

BOOL Plugin_IsVisible(duPlugin *pPlugin)
{
	if (pPlugin->IsVisible())
	{
		duPlugin *pParent = pPlugin->GetParent();
		while (pParent)
		{
			if (!pParent->IsVisible())
				return FALSE;

			pParent = pParent->GetParent();
		}

		return TRUE;
	}
	
	return FALSE;
}

BOOL Plugin_GetVisibleRect(duPlugin *pPlugin, LPRECT lpRect)
{
	duRect rectParent;
	
	if (lpRect == NULL || !Plugin_IsValid(pPlugin))
		return FALSE;
	
	::SetRectEmpty(lpRect);
	pPlugin->GetRect(lpRect);
	duPlugin *pParent = pPlugin->GetParent();
	while (pParent)
	{
		rectParent.SetRectEmpty();
		pParent->GetRect(&rectParent);
		::IntersectRect(lpRect, lpRect, &rectParent);

		pParent = pParent->GetParent();
	}
	
	return TRUE;
}

BOOL Plugin_Redraw(duPlugin *pPlugin, BOOL fNotify)
{
	if (pPlugin == NULL)
		return FALSE;
	
	duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
	if (pCtrlManager == NULL)
		return FALSE;
	
	if (fNotify)
		pPlugin->OnRedraw();
	
	return pCtrlManager->RedrawPlugin(pPlugin);
}

BOOL Plugin_SetCached(duPlugin *pPlugin, BOOL fCached)
{
	if (!Plugin_IsValid(pPlugin) || fCached == pPlugin->IsCached())
		return FALSE;
		
	if (fCached)
	{
		if (pPlugin->GetCachedBitmap() != NULL)
		{
			_TRACE(_T("err[%s]cached bitmap is not null\n"), pPlugin->GetName());
			return FALSE;
		}
		
		duRect rcPlugin;
		rcPlugin.SetRectEmpty();
		pPlugin->GetRect(&rcPlugin);
		BITMAPINFO bmInfo;
		PBYTE pBits = NULL;
		
		HDC hPluginDC = ::CreateCompatibleDC(NULL);
		InitBitmapInfo(&bmInfo, rcPlugin.Width(), rcPlugin.Height());
		HBITMAP hPluginBitmap = ::CreateDIBSection(hPluginDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
		HBITMAP hOldPluginBitmap = (HBITMAP)::SelectObject(hPluginDC, hPluginBitmap);

		pPlugin->DrawObject(hPluginDC);

		::SelectObject(hPluginDC, hPluginBitmap);
		::DeleteDC(hPluginDC);
		
		pPlugin->SetCachedBitmap(hPluginBitmap);
	}
	else
	{
		pPlugin->SetCachedBitmap(NULL);
	}
	
	pPlugin->SetCached(fCached);
	return TRUE;
}

BOOL Plugin_GetType(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount)
{
	if (!Plugin_IsValid(pPlugin) || lpString == NULL || nMaxCount <= 0)
		return FALSE;
	
	LPCTSTR lpszType = pPlugin->GetType();
	int nLen = lstrlen(lpszType);
	_tcsncpy(lpString, lpszType, min(nMaxCount, nLen));
	return TRUE;
}

BOOL Plugin_GetName(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount)
{
	if (!Plugin_IsValid(pPlugin) || lpString == NULL || nMaxCount <= 0)
		return FALSE;
	
	LPCTSTR lpszName = pPlugin->GetName();
	int nLen = lstrlen(lpszName);
	_tcsncpy(lpString, lpszName, min(nMaxCount, nLen));
	return TRUE;
}

BOOL Plugin_GetStyle(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount)
{
	if (!Plugin_IsValid(pPlugin) || lpString == NULL || nMaxCount <= 0)
		return FALSE;
		
	LPCTSTR lpszStyle = pPlugin->GetStyle();
	int nLen = lstrlen(lpszStyle);
	_tcsncpy(lpString, lpszStyle, min(nMaxCount, nLen));
	return TRUE;
}

BOOL Plugin_GetCursor(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount)
{
	if (!Plugin_IsValid(pPlugin) || lpString == NULL || nMaxCount <= 0)
		return FALSE;
	
	LPCTSTR lpszCursor = pPlugin->GetCursor();
	int nLen = lstrlen(lpszCursor);
	_tcsncpy(lpString, lpszCursor, min(nMaxCount, nLen));
	return TRUE;
}

int Plugin_GetTextLength(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin))
		return 0;
	
	return pPlugin->GetTextLength();
}

BOOL Plugin_GetText(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount)
{
	if (!Plugin_IsValid(pPlugin) || lpString == NULL || nMaxCount <= 0)
		return FALSE;
	
	LPCTSTR lpszText = pPlugin->GetText();
	int nLen = pPlugin->GetTextLength();
	_tcsncpy(lpString, lpszText, min(nMaxCount, nLen));
	return TRUE;
}

void Plugin_GetRect(duPlugin *pPlugin, LPRECT lpRect)
{
	//if (!Plugin_IsValid(pPlugin) || lpRect == NULL)
	//	return;
	
	lpRect->left = lpRect->top = lpRect->right = lpRect->bottom = 0;
	pPlugin->GetRect(lpRect);
}

void Plugin_SetFocus(duPlugin *pPlugin, BOOL fFocus)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
	if (pCtrlManager == NULL)
		return;
	
	pCtrlManager->SetFocusPlugin(pPlugin, fFocus);
}

void Plugin_SetHwndVisible(duHwndObj *pHwndObj)
{
	HWND hWnd = pHwndObj->GetAttach();
	BOOL fVisible = Plugin_IsVisible(pHwndObj);
	fVisible ? ShowWindow(hWnd, SW_SHOW) : ShowWindow(hWnd, SW_HIDE);		
}

void Plugin_SetHwndObjVisible(duPlugin *pParent, BOOL fVisible)
{	
	deque<duPlugin *> dqChild;
	dqChild.clear();
	dqChild.push_back(pParent);

	while (!dqChild.empty())
	{
		duPlugin *pTop = dqChild.at(0); 
		dqChild.pop_front();

		LPCTSTR lpszChildType = pTop->GetType();
		if (lstrcmpi(lpszChildType, _T("hwndobj")) == 0 || lstrcmpi(lpszChildType, _T("flash")) == 0)
		{
			duHwndObj *pHwndObj = (duHwndObj *)pTop;
			HWND hWnd = pHwndObj->GetAttach();
			if (::IsWindow(hWnd))
				Plugin_SetHwndVisible(pHwndObj);
		}

		duPlugin *pTemp = pTop->GetFirstChild();
		while(pTemp) 
		{			
			dqChild.push_back(pTemp);
			pTemp = pTemp->GetNextSibling();
		}
	}
	dqChild.clear();
}

void Plugin_SetVisible(duPlugin *pPlugin, BOOL fVisible)
{
	if (!Plugin_IsValid(pPlugin))
		return;

	if (pPlugin->IsVisible() == fVisible)
		return;

	duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
	if (pCtrlManager == NULL)
		return;

	pPlugin->SetVisible(fVisible);
	Plugin_SetHwndObjVisible(pPlugin, fVisible);
	
	duPlugin *pParent = pPlugin->GetParent();
	pCtrlManager->DispatchMsg(pParent, DUMI_SHOWCONTROL, (WPARAM)pPlugin, (LPARAM)fVisible);
	pPlugin->NotifyUser(DUM_SHOWCONTROL, (WPARAM)fVisible, NULL);

	if (Plugin_IsValid(pCtrlManager->m_pFocus) && !Plugin_IsVisible(pCtrlManager->m_pFocus))
	{
		pCtrlManager->m_pFocus->SetFocus(FALSE);
		duRect rcFocus;
		Plugin_GetRect(pCtrlManager->m_pFocus, &rcFocus);
		POINT pt = {rcFocus.left - 1, rcFocus.top - 1};
		pCtrlManager->m_pFocus->OnMouseLeave(pt);
		pCtrlManager->m_pFocus = NULL;
	}

	if (Plugin_IsValid(pCtrlManager->m_pHot) && !Plugin_IsVisible(pCtrlManager->m_pHot))
	{
		if (pCtrlManager->m_pHot != pCtrlManager->m_pFocus)
		{
			duRect rcHot;
			Plugin_GetRect(pCtrlManager->m_pHot, &rcHot);
			POINT pt = {rcHot.left - 1, rcHot.top - 1};
			pCtrlManager->m_pHot->OnMouseLeave(pt);
		}
		pCtrlManager->m_pHot = NULL;
	}
	if (Plugin_IsValid(pCtrlManager->m_pCapture) && !Plugin_IsVisible(pCtrlManager->m_pCapture))
		pCtrlManager->ReleaseCapture(NULL);
}

void Plugin_Resize(duPlugin *pPlugin, LPRECT lpRect)
{
	if (!Plugin_IsValid(pPlugin))
		return;
		
	duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
	if (pCtrlManager == NULL)
		return;
	
	duRect rcOldPlugin;
	duRect rcNewPlugin;
	
	rcOldPlugin.SetRectEmpty();
	rcNewPlugin.SetRectEmpty();
	
	pPlugin->GetRect(&rcOldPlugin);
	pPlugin->Resize(lpRect);
	pPlugin->GetRect(&rcNewPlugin);
	
	
	if (rcOldPlugin != rcNewPlugin)
	{
		duPlugin *pParent = pPlugin->GetParent();
		pCtrlManager->DispatchMsg(pParent, DUMI_RESIZE, (WPARAM)pPlugin, NULL);
	}
}

UINT duPlugin::GetState()
{
	BOOL fChecked = IS_STATE_CHECKED(m_uState);
	BOOL fDisable = IsDisable();
	BOOL fFocus = IsFocus();
	
	if (fChecked)
	{
		if (fDisable)
			return DU_STATE_CHECKEDDISABLE;
		else
		{
			if (fFocus && m_uState == DU_STATE_CHECKED)
				return DU_STATE_CHECKEDFOCUS;
			else
				return m_uState;
		}
	}
	else
	{
		if (fDisable)
			return DU_STATE_DISABLE;
		else
		{
			if (fFocus && m_uState == DU_STATE_NORMAL)
				return DU_STATE_FOCUS;
			else
				return m_uState;
		}
	}

	return DU_STATE_NORMAL;
}

LRESULT duPlugin::NotifyUser(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Plugin_IsValid(this))
		return 0;

	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return 0;

	//pCtrlManager->CallScript(this, uMsg, wParam, lParam);

	duNotify notify;
	notify.uMsg = uMsg;
	notify.wParam = wParam;
	notify.lParam = lParam;
	return ::SendMessage(m_hWnd, WM_DIRECTUI_NOTIFY, (WPARAM)this, (LPARAM)&notify);
}

UINT Plugin_GetState(duPlugin *pPlugin)
{
	return pPlugin->GetState();
}

void Plugin_SetState(duPlugin *pPlugin, UINT uState)
{
	pPlugin->SetState(uState);
}

BOOL Plugin_SetTimer(duPlugin *pPlugin, UINT uTimerId, UINT uElapse)
{
	if (!Plugin_IsValid(pPlugin))
		return FALSE;
	
	duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
	if (pCtrlManager == NULL)
		return FALSE;
		
	return pCtrlManager->SetTimer(pPlugin, uTimerId, uElapse);
}

BOOL Plugin_KillTimer(duPlugin *pPlugin, UINT uTimerId)
{
	if (!Plugin_IsValid(pPlugin))
		return FALSE;
		
	duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
	if (pCtrlManager == NULL)
		return FALSE;
		
	return pCtrlManager->KillTimer(pPlugin, uTimerId);
}

duPlugin *Plugin_SetCapture(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin))
		return NULL;
	
	duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
	if (pCtrlManager == NULL)
		return FALSE;
		
	return pCtrlManager->SetCapture(pPlugin);
}

BOOL Plugin_ReleaseCapture(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin))
		return FALSE;
		
	duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
	if (pCtrlManager == NULL)
		return FALSE;
		
	return pCtrlManager->ReleaseCapture(pPlugin);
}

void DrawByStyle(duPlugin *pPlugin, LPCTSTR lpszStyle, HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha)
{
	if (!Plugin_IsValid(pPlugin) || lpszStyle == NULL || hDC == NULL || lpDstRect == NULL)
		return;
		
	duStyleGroup *pStyleGroup = (duStyleGroup *)pPlugin->GetResObj(lpszStyle, DU_RES_STYLEGROUP);
	if (pStyleGroup == NULL)
		return;
	
	pStyleGroup->Draw(hDC, lpDstRect, uState, lpszText, nAlpha);
}

BOOL Plugin_HookWindowMessage(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin))
		return FALSE;
	
	duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
	if (pCtrlManager == NULL)
		return FALSE;
		
	return pCtrlManager->HookWindowMessage(pPlugin);
}

BOOL Plugin_UnHookWindowMessage(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin))
		return FALSE;
	
	duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
	if (pCtrlManager == NULL)
		return FALSE;
		
	return pCtrlManager->UnHookWindowMessage(pPlugin);
}

duPlugin *Plugin_GetCapture(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin))
		return FALSE;
	
	duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
	if (pCtrlManager == NULL)
		return FALSE;
		
	return pCtrlManager->GetCapture();
}

BOOL Plugin_Delete(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin))
		return FALSE;

	duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
	if (pCtrlManager == NULL)
		return FALSE;

	return pCtrlManager->DeletePlugin(pPlugin);
}

duPlugin *Plugin_Clone(duPlugin *pTemplate, duPlugin *pParent, duPlugin *pPrevious, UINT nSuffix)
{
	if (!Plugin_IsValid(pTemplate) || !Plugin_IsValid(pParent))
		return NULL;
	
	if (pPrevious && !Plugin_IsValid(pPrevious))
			return NULL;

	duCtrlManager *pCtrlManager = GetCtrlManager(pParent->GetHwnd());
	if (pCtrlManager == NULL)
		return NULL;

	TCHAR szName[MAX_NAME];
	ZeroMemory(szName, MAX_NAME * sizeof(TCHAR));
	_stprintf(szName, _T("%s%d"), pTemplate->GetName(), nSuffix);
	if (pCtrlManager->GetPluginByName(szName))
		return NULL;

	return pCtrlManager->Clone(pTemplate, pParent, pPrevious, nSuffix);
}

BOOL Plugin_SetDisable(duPlugin *pPlugin, BOOL fDisable)
{
	if (!Plugin_IsValid(pPlugin))
		return FALSE;
	
	BOOL fOldDisable = pPlugin->IsDisable();
	if (fOldDisable == fDisable)
		return FALSE;

	pPlugin->SetDisable(fDisable);
	if (fDisable)
	{
		duCtrlManager *pCtrlManager = GetCtrlManager(pPlugin->GetHwnd());
		if (pCtrlManager == NULL)
			return fDisable;
	
		if (pCtrlManager->m_pHot == pPlugin)
		{
			duRect rcPlugin;
			Plugin_GetRect(pPlugin, &rcPlugin);
			
			POINT ptLeave = {rcPlugin.left - 1, rcPlugin.top - 1};
			pPlugin->OnMouseLeave(ptLeave);
		}
	}

	return fOldDisable;
}
UINT64 Plugin_GetParam64(duPlugin *pPlugin)
{
	return pPlugin->GetParam64();
}

void Plugin_SetParam64(duPlugin *pPlugin, UINT64 nlParam)
{
	pPlugin->SetParam64(nlParam);
}

void Plugin_SetTabOrder(duPlugin *pPlugin, int nTabOrder)
{
	if (nTabOrder < 0)
		nTabOrder = 0;

	pPlugin->SetTabOrder(nTabOrder);
}

int Plugin_GetTabOrder(duPlugin *pPlugin)
{
	return pPlugin->GetTabOrder();
}

}
