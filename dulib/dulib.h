//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       dulib.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once

#include "duTypes.h"
#include "duCtrlManager.h"
#include "duResManager.h"
#include "duWindowManager.h"
#include "duStyle.h"
#include "duFont.h"
#include "duCursor.h"
#include "duMessage.h"
#include "duPlugin.h"
#include "duProperty.h"
#include "duStyleGroup.h"
#include "duZorderList.h"
#include "duRedrawLayer.h"

#include "controls/duAnimation.h"
#include "controls/duButton.h"
#include "controls/duCheckBox.h"
#include "controls/duComboBox.h"
#include "controls/duEdit.h"
#include "controls/duGifCtrl.h"
#include "controls/duGridLayout.h"
#include "controls/duHeaderCtrl.h"
#include "controls/duHwndObj.h"
#include "controls/duIMPanelNode.h"
#include "controls/duLayout.h"
#include "controls/duLEDTimer.h"
#include "controls/duLink.h"
#include "controls/duListBox.h"
#include "controls/duListBoxEx.h"
#include "controls/duListCtrl.h"
#include "controls/duMenu.h"
#include "controls/duMenuBar.h"
#include "controls/duMenuItem.h"
#include "controls/duMLEdit.h"
#include "controls/duOutlookBar.h"
#include "controls/duPage.h"
#include "controls/duPie.h"
#include "controls/duProgress.h"
#include "controls/duRadio.h"
#include "controls/duRemoteCtrl.h"
#include "controls/duReportCell.h"
#include "controls/duReportColumn.h"
#include "controls/duReportHeader.h"
#include "controls/duReportLine.h"
#include "controls/duReportView.h"
#include "controls/duScroll.h"
#include "controls/duScrollContainer.h"
#include "controls/duScrollView.h"
#include "controls/duSlider.h"
#include "controls/duSpin.h"
#include "controls/duSplitter.h"
#include "controls/duStatic.h"
#include "controls/duStaticEx.h"
#include "controls/duTabButton.h"
#include "controls/duTreeCtrl.h"
#include "controls/duTreeListCtrl.h"
#include "controls/duTreeListItem.h"
#include "controls/duWindow.h"

extern "C"
{
	duWindowManager* LoadStyleA(LPCSTR lpszXml);
	duWindowManager* LoadStyleW(LPCWSTR lpszXml);
	duWindowManager* LoadStyleZipA(LPCSTR lpszZip, LPCSTR lpszZipPassword);
	duWindowManager* LoadStyleZipW(LPCWSTR lpszZip, LPCSTR lpszZipPassword);
	duWindowManager* LoadResourceStyleZipW(LPCWSTR lpszZipData, ULONG nResZipSize, LPCSTR lpszZipPassword);
					 
	duWindowManager* LoadStyleZipMemory(BYTE *pData, int nSize);
	//duWindowManager* LoadStyleBduA(LPCWSTR lpszZip);
	//duWindowManager*LoadStyleBduW(LPCWSTR lpszZip);
	//duWindowManager*LoadStyleBduMemory(BYTE *pData, int nSize);

	void Dulib_Init();
	void Dulib_Release();

	BOOL ReleaseStyle(duWindowManager *pWinManager);
	BOOL WindowManager_Attach(duWindowManager *pWinManager, HWND hWnd, LPCWSTR lpWindowName);
	BOOL MatchString(LPCTSTR lpszPat, LPCTSTR lpszStr);
	BOOL Plugin_Redraw(duPlugin *pPlugin, BOOL fNotify);
	BOOL Plugin_IsVisible(duPlugin *pPlugin);
	void Plugin_SetVisible(duPlugin *pPlugin, BOOL fVisible);
	BOOL Plugin_IsValid(duPlugin *pPlugin);
	duPlugin *Plugin_Clone(duPlugin *pTemplate, duPlugin *pParent, duPlugin *pPrevious, UINT nSuffix);
	BOOL Plugin_Delete(duPlugin *pPlugin);
	BOOL Plugin_GetVisibleRect(duPlugin *pPlugin, LPRECT lpRect);
	void Plugin_GetRect(duPlugin *pPlugin, LPRECT lpRect);
	void Plugin_Resize(duPlugin *pPlugin, LPRECT lpRect);
	BOOL Plugin_MoveUp(duPlugin *pPlugin);
	BOOL Plugin_MoveDown(duPlugin *pPlugin);
	void DrawByStyle(duPlugin *pPlugin, LPCTSTR lpszStyle, HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha);
	duPlugin* GetPluginByName(HWND hWnd, LPCTSTR lpszName);
	duPlugin* GetFocusPlugin(HWND hWnd);
	void SetFocusPlugin(HWND hWnd, duPlugin *pPlugin);
	int ReadXmlAttributeText2(void *pEle, const TCHAR *keyName, TCHAR *pVal, int maxlen);
	int ReadXmlAttributeInt(void *pEle, const TCHAR *keyName);
	float ReadXmlAttributeFloat(void *pEle, const TCHAR *keyName);
	double ReadXmlAttributeDouble(void *pEle, const TCHAR *keyName);
	COLORREF ReadXmlAttributeColor(void *pEle, const TCHAR *keyName);
	void ReadXmlAttributeBOOL(void *pEle, const TCHAR *keyName, BOOL *pbResult);
	void ReadXmlAttributeSize(void *pEle, const TCHAR *keyName, int *cx, int *cy);
	UINT ReadXmlAttributeState(void *pEle, const TCHAR *keyName);
	void ReadXmlAttributePoint(void *pEle, const TCHAR *keyName, POINT *pPoint);
	void WriteXmlAttributeText(void *pEle, const TCHAR *keyName, const TCHAR *Val);
	void WriteXmlAttributeInt(void *pEle, const TCHAR *keyName, int Val);
	void WriteXmlAttributeBOOL(void *pEle, const TCHAR *keyName, BOOL Val);
	void WriteXmlAttributeFloat(void *pEle, const TCHAR *keyName, float Val);
	void WriteXmlAttributeDouble(void *pEle, const TCHAR *keyName, double Val);
	void WriteXmlAttributeColor(void *pEle, const TCHAR *keyName, COLORREF clr);
	void WriteXmlAttributeState(void *pEle, const TCHAR *keyName, int nState);
	void WriteXmlAttributePoint(void *pEle, const TCHAR *keyName, POINT pPoint);
	void* XmlFirstChild(void *pEle, const TCHAR *eleName);
	void* XmlNextSibling(void *pEle, const TCHAR *eleName);
	void* XmlCreateElement(const TCHAR *eleName);
	void XmlDeleteElement(void *pEle);
	void* XmlLinkEndChild(void *pParent, void *pInsert);
	void XmlDeleteChild(void *pParent, void *pChild);
	void* XmlClone(void *pEle);
	const TCHAR* XmlElementValue(void *pEle);
	void* XmlCloneOne(void *pEle);

};


//////////////////////////////////////////////////////////////////////////
// Message Map
template<class T>
struct _du_MESSAGE_MAP
{
	typedef LRESULT (T::*TMFP)(duPlugin *pPlugin, WPARAM wParam, LPARAM lParam);

	const TCHAR *szName;
	UINT uMsg;
	TMFP func;
};

#define duBEGIN_MESSAGE_MAP(dispClass, __wParam, __lParam) \
	typedef dispClass _du_disp_classype; \
	duPlugin *__pPlugin = (duPlugin *)__wParam; \
	duNotify *__pNotify = (duNotify *)__lParam; \
	static _du_MESSAGE_MAP<_du_disp_classype> __du_message_map[] = \
{ \

#define duON_MESSAGE(name, msg, func) \
{ name, msg, (LRESULT (_du_disp_classype::*)(duPlugin *pPlugin, WPARAM, LPARAM))func}, \

//#define duON_MESSAGE(name, msg, func) \
//{ _T(#name), msg, (LRESULT (_du_disp_classype::*)(duPlugin *pPlugin, WPARAM, LPARAM))func}, \

#define duEND_MESSAGE_MAP() \
{ NULL, NULL, NULL } \
}; \
	LRESULT lResult = 0; \
	_du_MESSAGE_MAP<_du_disp_classype> *pMessageMap = __du_message_map; \
	while ( pMessageMap->szName ) { \
	if ( (MatchString(pMessageMap->szName, __pPlugin->GetName())) && (pMessageMap->uMsg == __pNotify->uMsg) )  { \
	lResult = (this->*pMessageMap->func)(__pPlugin, __pNotify->wParam, __pNotify->lParam); \
	return lResult; \
	} \
	pMessageMap++; \
	} \
	return lResult;

