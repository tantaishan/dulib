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
#include "duObject.h"
#include "duPlugin.h"
#include "duProperty.h"
#include "duStyleGroup.h"
#include "duZorderList.h"
#include "duRedrawLayer.h"
#include "ScriptObject.h"

#include "controls/AxControl.h"
#include "controls/duAnimation.h"
#include "controls/duButton.h"
#include "controls/duCheckBox.h"
#include "controls/duComboBox.h"
#include "controls/duEdit.h"
#include "controls/duFlash.h"
#include "controls/duFlashEventSink.h"
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
	duWindowManager *WINAPI LoadStyleA(LPCSTR lpszXml);
	duWindowManager *WINAPI LoadStyleW(LPCWSTR lpszXml);
	duWindowManager *WINAPI LoadStyleZipA(LPCSTR lpszZip, LPCSTR lpszZipPassword);
	duWindowManager *WINAPI LoadStyleZipW(LPCWSTR lpszZip, LPCSTR lpszZipPassword);
	duWindowManager *WINAPI LoadResourceStyleZipW(LPCWSTR lpszZipData,ULONG nResZipSize, LPCSTR lpszZipPassword);

	duWindowManager *WINAPI LoadStyleZipMemory(BYTE *pData, int nSize);
	duWindowManager *WINAPI LoadStyleBduA(LPCWSTR lpszZip);
	duWindowManager *WINAPI LoadStyleBduW(LPCWSTR lpszZip);
	duWindowManager *WINAPI LoadStyleBduMemory(BYTE *pData, int nSize);
	BOOL WINAPI ReleaseStyle(duWindowManager *pWinManager);
	BOOL WINAPI WindowManager_Attach(duWindowManager *pWinManager, HWND hWnd, LPCWSTR lpWindowName);
	BOOL WINAPI MatchString(LPCTSTR lpszPat, LPCTSTR lpszStr);
	BOOL WINAPI Plugin_Redraw(duPlugin *pPlugin, BOOL fNotify);
	BOOL WINAPI Plugin_IsVisible(duPlugin *pPlugin);
	void WINAPI Plugin_SetVisible(duPlugin *pPlugin, BOOL fVisible);
	BOOL WINAPI Plugin_IsValid(duPlugin *pPlugin);
	duPlugin *WINAPI Plugin_Clone(duPlugin *pTemplate, duPlugin *pParent, duPlugin *pPrevious, UINT nSuffix);
	BOOL WINAPI Plugin_Delete(duPlugin *pPlugin);
	BOOL WINAPI Plugin_GetVisibleRect(duPlugin *pPlugin, LPRECT lpRect);
	void WINAPI Plugin_GetRect(duPlugin *pPlugin, LPRECT lpRect);
	void WINAPI Plugin_Resize(duPlugin *pPlugin, LPRECT lpRect);
	BOOL WINAPI Plugin_MoveUp(duPlugin *pPlugin);
	BOOL WINAPI Plugin_MoveDown(duPlugin *pPlugin);
	void WINAPI DrawByStyle(duPlugin *pPlugin, LPCTSTR lpszStyle, HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha);
	duPlugin *WINAPI GetPluginByName(HWND hWnd, LPCTSTR lpszName);
	duPlugin *WINAPI GetFocusPlugin(HWND hWnd);
	void WINAPI SetFocusPlugin(HWND hWnd, duPlugin *pPlugin);
	int WINAPI ReadXmlAttributeText2(void *pEle, const TCHAR *keyName, TCHAR *pVal, int maxlen);
	int WINAPI ReadXmlAttributeInt(void *pEle, const TCHAR *keyName);
	float WINAPI ReadXmlAttributeFloat(void *pEle, const TCHAR *keyName);
	double WINAPI ReadXmlAttributeDouble(void *pEle, const TCHAR *keyName);
	COLORREF WINAPI ReadXmlAttributeColor(void *pEle, const TCHAR *keyName);
	void WINAPI ReadXmlAttributeBOOL(void *pEle, const TCHAR *keyName, BOOL *pbResult);
	void WINAPI ReadXmlAttributeSize(void *pEle, const TCHAR *keyName, int *cx, int *cy);
	UINT WINAPI ReadXmlAttributeState(void *pEle, const TCHAR *keyName);
	void WINAPI ReadXmlAttributePoint(void *pEle, const TCHAR *keyName, POINT *pPoint);
	void WINAPI WriteXmlAttributeText(void *pEle, const TCHAR *keyName, const TCHAR *Val);
	void WINAPI WriteXmlAttributeInt(void *pEle, const TCHAR *keyName, int Val);
	void WINAPI WriteXmlAttributeBOOL(void *pEle, const TCHAR *keyName, BOOL Val);
	void WINAPI WriteXmlAttributeFloat(void *pEle, const TCHAR *keyName, float Val);
	void WINAPI WriteXmlAttributeDouble(void *pEle, const TCHAR *keyName, double Val);
	void WINAPI WriteXmlAttributeColor(void *pEle, const TCHAR *keyName, COLORREF clr);
	void WINAPI WriteXmlAttributeState(void *pEle, const TCHAR *keyName, int nState);
	void WINAPI WriteXmlAttributePoint(void *pEle, const TCHAR *keyName, POINT pPoint);
	void *WINAPI XmlFirstChild(void *pEle, const TCHAR *eleName);
	void *WINAPI XmlNextSibling(void *pEle, const TCHAR *eleName);
	void *WINAPI XmlCreateElement(const TCHAR *eleName);
	void WINAPI XmlDeleteElement(void *pEle);
	void *WINAPI XmlLinkEndChild(void *pParent, void *pInsert);
	void WINAPI XmlDeleteChild(void *pParent, void *pChild);
	void *WINAPI XmlClone(void *pEle);
	const TCHAR *WINAPI XmlElementValue(void *pEle);
	void *WINAPI XmlCloneOne(void *pEle);

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

