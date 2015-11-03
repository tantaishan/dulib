//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duComboBox.cpp
//
//  History:    Nov-17-2009   Steven Li  Created
//
//--------------------------------------------------------------------------
#include "duComboBox.h"
#include "duCtrlManager.h"

#define LISTBOX_DEFAULT_VALUE	0xEFFFFFFF

LPCTSTR lpszComboxClassName = _T("DUCOMBOXLISTBOXWND");
ATOM RegisterComboListClass();
void TrackPopup(duComboBox *pComboBox, HWND hwndOwner, HWND hwndListBox);
extern "C" BOOL WindowManager_Attach(duWindowManager *pWinManager, HWND hWnd, LPCTSTR lpWindowName);

duComboBox::duComboBox() :
	 m_nWindowHeight(0)
	,m_hWndListBox(NULL)
	,m_fEnd(FALSE)
	,m_nCurSel(-1)
{
	ZeroMemory(m_szListBox, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szWindow, MAX_NAME * sizeof(TCHAR));
	m_vtItem.clear();
}

duComboBox::~duComboBox()
{
	int nItemCount = (int)m_vtItem.size();
	int i;
	ListBoxItem *pItem = NULL;
	for (i = 0; i < nItemCount; i++)
	{
		pItem = m_vtItem[i];
		SAFE_DELETE(pItem);
	}
	m_vtItem.clear();
}

void duComboBox::Destroy()
{
	int nItemCount = (int)m_vtItem.size();
	int i;
	ListBoxItem *pItem = NULL;
	for (i = 0; i < nItemCount; i++)
	{
		pItem = m_vtItem[i];
		SAFE_DELETE(pItem);
	}
	m_vtItem.clear();

	//DeleteAll之后将edit上的文本和当前选择项置为-1

	SetText(_T(""));
	m_nCurSel = -1;
}

void duComboBox::RegisterControlProperty()
{
	duEdit::RegisterControlProperty();
	
	RegisterProperty(_T("window"),   DU_PROPERTY_STRING, m_szWindow);
	RegisterProperty(_T("windowheight"),   DU_PROPERTY_LONG, &m_nWindowHeight);
	RegisterProperty(_T("listbox"),   DU_PROPERTY_STRING, m_szListBox);
}

void duComboBox::OnCreate()
{
	duEdit::OnCreate();

	//int i;
	//for (i = 0;i < 20;i++)
	//{
	//	TCHAR szBuf[128];
	//	_stprintf(szBuf, _T("Item %d"), i);
	//	InsertItem(-1, szBuf, _T("listbox_icon"));
	//}

	//SetItemIconX(7, 10);
}

void duComboBox::OnMouseLDown(POINT pt)
{
	duPoint ptCombo(pt);
	
	duRect rcCombobox;
	Plugin_GetRect(this, &rcCombobox);
	ptCombo.Offset(-rcCombobox.left, -rcCombobox.top);
	
	if (!m_fReadOnly && ::PtInRect(&m_rcFormat, ptCombo))
		duEdit::OnMouseLDown(pt);
	else
	{
		Plugin_SetState(this, DU_STATE_PRESS);
		Plugin_Redraw(this, TRUE);

		RegisterComboListClass();

		ptCombo.x = rcCombobox.left;
		ptCombo.y = rcCombobox.bottom;
		::ClientToScreen(m_hWnd, &ptCombo);

		HWND hwndDesktop = ::GetDesktopWindow();
		int nWindowHeight = CalcListBoxHeight();
		m_hWndListBox = ::CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST, lpszComboxClassName, _T(""),
			WS_CHILD, ptCombo.x, ptCombo.y, rcCombobox.Width(), nWindowHeight, hwndDesktop, (HMENU)1001,
			(HINSTANCE)::GetModuleHandle(NULL), (LPVOID)this);

		if (m_hWndListBox == NULL)
			return;

		if (nWindowHeight <= 6)
			::SetWindowPos(m_hWndListBox, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
		else
		{
			DWORD dwStartTick =::GetTickCount();
			int nWinHeight = 0;
			int nAnimateCount = (nWindowHeight / 5) + 1;
			int i;
			for (i = 0;i < 6; i++)
			{
				nWinHeight += nAnimateCount;
				if (nWinHeight >= nWindowHeight)
					nWinHeight = nWindowHeight;
					
				if (::GetTickCount() - dwStartTick > 200)
				{
					::SetWindowPos(m_hWndListBox, HWND_TOPMOST, 0, 0, rcCombobox.Width(), nWindowHeight, SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOMOVE);
					break;
				}
				
				::SetWindowPos(m_hWndListBox, HWND_TOPMOST, 0, 0, rcCombobox.Width(), nWinHeight, SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOMOVE);
				::RedrawWindow(m_hWndListBox, NULL, NULL, RDW_UPDATENOW|RDW_INTERNALPAINT);
				::Sleep(40);
			}
		}
		
		HWND hWndOwner = m_hWnd;
		while (::GetWindowLong(hWndOwner, GWL_STYLE) & WS_CHILD)
			hWndOwner = ::GetParent(hWndOwner);
		

		TrackPopup(this, hWndOwner, m_hWndListBox);
		SetEnd(FALSE);
		m_hWndListBox = NULL;

		Plugin_SetState(this, DU_STATE_NORMAL);
		Plugin_Redraw(this, TRUE);
	}
}

BOOL duComboBox::OnSetFocus(duPlugin *pOldFocus)
{
	if (m_fReadOnly)
		return duPlugin::OnSetFocus(pOldFocus);
	
	return duEdit::OnSetFocus(pOldFocus);
}

void duComboBox::OnKeyDown(UINT key, UINT nRepCnt, UINT nFlags)
{
	int nItemCount = GetItemCount();
	if (key == VK_UP)
	{
		if (nItemCount == 0)
			return;

		int nSel = GetCurSel();
		nSel--;
		if (nSel < 0)
			nSel = 0;
		if (nSel >= nItemCount)
			nSel = nItemCount - 1;

		SetCurSel(nSel);
		Plugin_Redraw(this, TRUE);
	}
	else if (key == VK_DOWN)
	{
		int nSel = GetCurSel();
		nSel++;
		if (nSel >= nItemCount)
			nSel = nItemCount - 1;
		if (nSel < 0)
			nSel = 0;

		SetCurSel(nSel);
		Plugin_Redraw(this, TRUE);
	}
	else
		duEdit::OnKeyDown(key, nRepCnt, nFlags);
}

BOOL duComboBox::OnSetCursor()
{
	if (m_fReadOnly)
		return duPlugin::OnSetCursor();

	return duEdit::OnSetCursor();
}

void duComboBox::OnMouseDbClick(POINT pt)
{
	if (m_fReadOnly)
		return;

	return duEdit::OnMouseDbClick(pt);
}

BOOL duComboBox::InsertItem(int nIndex, LPCTSTR lpszText, LPCTSTR lpszIcon)
{
	int nItemCount = GetItemCount();
	if (nIndex == -1)
		nIndex = nItemCount;
		
	if (nIndex < 0 || nIndex > nItemCount)
		return FALSE;
	
	ListBoxItem *pItem = new ListBoxItem;
	pItem->uIconX = LISTBOX_DEFAULT_VALUE;
	pItem->uIconY = LISTBOX_DEFAULT_VALUE;
	pItem->uIconWidth = LISTBOX_DEFAULT_VALUE;
	pItem->uIconHeight = LISTBOX_DEFAULT_VALUE;
	pItem->lParam = NULL;
	pItem->fDisable = FALSE;
	if (lpszText)
		pItem->szText = lpszText;
	else
		pItem->szText = _T("");
		
	if (lpszIcon)
		pItem->szIcon = lpszIcon;
	else
		pItem->szIcon = _T("");
	
	vector<ListBoxItem *>::iterator iterInsert = m_vtItem.begin() + nIndex;
	m_vtItem.insert(iterInsert, pItem);
	return TRUE;
}

BOOL duComboBox::DeleteItem(int nIndex)
{
	int nItemCount = GetItemCount();
	if (nIndex < 0 || nIndex >= nItemCount)
		return FALSE;
	
	vector<ListBoxItem *>::iterator iterDelete = m_vtItem.begin() + nIndex;
	ListBoxItem *pItem = *iterDelete;
	SAFE_DELETE(pItem);
	m_vtItem.erase(iterDelete);
	return TRUE;
}

void duComboBox::DeleteAll()
{
	Destroy();
}

LPCTSTR duComboBox::GetItemText(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return NULL;
		
	return pItem->szText.c_str();
}

BOOL duComboBox::SetItemText(int nIndex, LPCTSTR lpszText)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	if (lpszText)
		pItem->szText = lpszText;
	else
		pItem->szText = _T("");
	return TRUE;
}

LPCTSTR duComboBox::GetItemIcon(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return NULL;
		
	return pItem->szIcon.c_str();
}

BOOL duComboBox::SetItemIcon(int nIndex, LPCTSTR lpszIcon)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	if (lpszIcon)
		pItem->szIcon = lpszIcon;
	else
		pItem->szIcon = _T("");
	return TRUE;
}

UINT duComboBox::GetItemIconX(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return LISTBOX_DEFAULT_VALUE;
		
	return pItem->uIconX;
}

BOOL duComboBox::SetItemIconX(int nIndex, UINT uIconX)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	pItem->uIconX = uIconX;
	return TRUE;
}

UINT duComboBox::GetItemIconY(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return LISTBOX_DEFAULT_VALUE;
		
	return pItem->uIconY;
}

BOOL duComboBox::SetItemIconY(int nIndex, UINT uIconY)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	pItem->uIconY = uIconY;
	return TRUE;
}

UINT duComboBox::GetItemIconWidth(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return LISTBOX_DEFAULT_VALUE;
		
	return pItem->uIconWidth;
}

BOOL duComboBox::SetItemIconWidth(int nIndex, UINT uIconWidth)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	pItem->uIconWidth = uIconWidth;
	return TRUE;
}

UINT duComboBox::GetItemIconHeight(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return LISTBOX_DEFAULT_VALUE;
		
	return pItem->uIconHeight;
}

BOOL duComboBox::SetItemIconHeight(int nIndex, UINT uIconHeight)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	pItem->uIconHeight = uIconHeight;
	return TRUE;
}

int duComboBox::GetCurSel()
{
	return m_nCurSel;
}

void duComboBox::SetCurSel(int nIndex)
{
	int nItemCount = (int)m_vtItem.size();
	if (nIndex < -1 || nIndex >= nItemCount)
		return;

	if (m_nCurSel != nIndex)
	{
		if (nIndex == -1)
			SetText(_T(""));
		else
			SetText(GetItemText(nIndex));
			
		m_nCurSel = nIndex;
	}
}

INT_PTR duComboBox::GetItemData(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem)
		return pItem->lParam;

	return NULL;
}

void duComboBox::SetItemData(int nIndex, INT_PTR pData)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem)
		pItem->lParam = pData;
}

ListBoxItem *duComboBox::GetItem(int nIndex)
{
	int nItemCount = GetItemCount();
	if (nIndex < 0 || nIndex >= nItemCount)
		return NULL;
	
	return m_vtItem[nIndex];
}

int duComboBox::AddItem(LPCTSTR lpszText, LPCTSTR lpszIcon)
{
	if (InsertItem(-1, lpszText, lpszIcon))
		return GetItemCount();
	
	return -1;
}

int duComboBox::FindItem(LPCTSTR lpszText)
{
	if (lpszText == NULL)
		return -1;

	int nCount = GetItemCount();
	int i;
	for (i = 0;i < nCount; i++)
	{
		ListBoxItem *pItem = m_vtItem[i];
		if (pItem && pItem->szText == lpszText)
			return i;
	}

	return -1;
}

int duComboBox::SelectItem(LPCTSTR lpszText)
{
	if (lpszText == NULL)
		return -1;

	int nFind = FindItem(lpszText);
	if (nFind != -1)
		SetCurSel(nFind);

	return nFind;
}

int duComboBox::CalcListBoxHeight()
{
	int nHeight = m_nWindowHeight;
	if (m_vtItem.size() == 0)
		return nHeight;

	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return nHeight;

	duWindowManager *pWinManager = pCtrlManager->GetWindowManager();
	if (pWinManager == NULL)
		return nHeight;

	TiXmlElement *pWindowElement = pWinManager->GetWindowXmlElement(m_szWindow);
	if (pWindowElement == NULL)
		return nHeight;

	TiXmlElement *pListBoxElement = pWindowElement->FirstChildElement(_T("listbox"));
	if (pListBoxElement == NULL)
		return nHeight;
	int nItemHeight = ReadXmlAttributeInt(pListBoxElement, _T("itemheight"));
	int nListBoxTop = ReadXmlAttributeInt(pListBoxElement, _T("top"));
	int nListBoxBottom = ReadXmlAttributeInt(pListBoxElement, _T("bottom"));
	nHeight = m_vtItem.size() * nItemHeight  + nListBoxTop  + nListBoxBottom;
	return (nHeight > m_nWindowHeight) ? m_nWindowHeight : nHeight;
}

BOOL duComboBox::IsItemDisable(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	return pItem->fDisable;
}

BOOL duComboBox::SetItemDisable(int nIndex, BOOL fDisable)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
	
	pItem->fDisable = fDisable;
	return TRUE;
}

void TrackPopup(duComboBox *pComboBox, HWND hwndOwner, HWND hwndListBox)
{
	POINT pt;
	MSG  msg;
	HWND hwndActive = NULL;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (pComboBox->IsEnd())
			break;

		hwndActive = GetActiveWindow();
		if (hwndActive != hwndOwner)
			break;

		switch (msg.message) {

		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
			pt.x = (short)LOWORD(msg.lParam);
			pt.y = (short)HIWORD(msg.lParam);
			MapWindowPoints(msg.hwnd, hwndListBox, &pt, 1);
			msg.lParam = MAKELPARAM(pt.x, pt.y);
			msg.hwnd = hwndListBox;
			break;

		case WM_MOUSEWHEEL:
			msg.hwnd = hwndListBox;
			break;

		case WM_NCMOUSEMOVE:
			msg.hwnd = hwndListBox;
			break;

		case WM_NCLBUTTONDOWN:
		case WM_NCLBUTTONUP:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCRBUTTONDOWN:
		case WM_NCRBUTTONUP:
		case WM_NCRBUTTONDBLCLK:
		case WM_NCMBUTTONDOWN:
		case WM_NCMBUTTONUP:
		case WM_NCMBUTTONDBLCLK:
			::DestroyWindow(hwndListBox);
			return;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_DEADCHAR:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_SYSCHAR:
		case WM_SYSDEADCHAR:
			msg.hwnd = hwndListBox;
			break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (pComboBox->IsEnd())
			break;

		hwndActive = GetActiveWindow();
		if (hwndActive != hwndOwner) 
			break;
	}

	::DestroyWindow(hwndListBox);

	if (msg.message == WM_QUIT) {
		PostQuitMessage((int)msg.wParam);
	}
}

ATOM RegisterComboListClass()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ComboListBox_WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= (HINSTANCE)::GetModuleHandle(NULL);
	wcex.hIcon			= NULL;
	wcex.hCursor		= ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= lpszComboxClassName;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

static LRESULT ComboListBox_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	duComboBox *pComboBox = NULL;
	
	if (uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT lpCS = (LPCREATESTRUCT)lParam;
		pComboBox = (duComboBox *)lpCS->lpCreateParams;
		if (!Plugin_IsValid(pComboBox))
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pComboBox);
	}
	
	pComboBox = (duComboBox *)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (!pComboBox || !Plugin_IsValid(pComboBox))
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	
	switch (uMsg)
	{
		case WM_CREATE:
		{
			duCtrlManager *pCtrlManager = GetCtrlManager(pComboBox->GetHwnd());
			if (pCtrlManager == NULL)
				break;
				
			duWindowManager *pWinManager = pCtrlManager->GetWindowManager();
			if (pWinManager == NULL)
				break;
			
			WindowManager_Attach(pWinManager, hWnd, pComboBox->m_szWindow);
			duCtrlManager *pNewCtrlManager = GetCtrlManager(hWnd);
			if (pNewCtrlManager == NULL)
				return -1;

			duWindow *pWindow = (duWindow *)pNewCtrlManager->GetPluginByName(pComboBox->m_szWindow);
			if (pWindow == NULL)
				return -1;

			duRect rcWindow;
			::GetWindowRect(hWnd, &rcWindow);
			pWindow->SetPosWidth(rcWindow.Width());
			pWindow->SetPosHeight(rcWindow.Height());
			pWindow->Resize(NULL);
			
			duListBox *pListBox = (duListBox *)pNewCtrlManager->GetPluginByName(pComboBox->m_szListBox);
			if (!Plugin_IsValid(pListBox))
				return -1;
			
			pListBox->SetComboBoxListBox(TRUE);
			vector<ListBoxItem *> &vtListBoxItem = pListBox->GetItemVector();

			int nItemCount = (int)pComboBox->GetItemCount();
			int i;
			ListBoxItem *pItem = NULL;
			for (i = 0; i < nItemCount; i++)
			{
				pItem = pComboBox->GetItem(i);
				ListBoxItem *pNewItem = new ListBoxItem;
				
				if (pItem->uIconX == LISTBOX_DEFAULT_VALUE)
					pNewItem->uIconX = pListBox->m_nItemIconX;
				else
					pNewItem->uIconX = pItem->uIconX;
			
				if (pItem->uIconY == LISTBOX_DEFAULT_VALUE)
					pNewItem->uIconY = pListBox->m_nItemIconY;
				else
					pNewItem->uIconY = pItem->uIconY;

				if (pItem->uIconWidth == LISTBOX_DEFAULT_VALUE)
					pNewItem->uIconWidth = pListBox->m_nItemIconWidth;
				else
					pNewItem->uIconWidth = pItem->uIconWidth;

				if (pItem->uIconHeight == LISTBOX_DEFAULT_VALUE)
					pNewItem->uIconHeight = pListBox->m_nItemIconHeight;
				else
					pNewItem->uIconHeight = pItem->uIconHeight;
				
				pNewItem->szIcon = pItem->szIcon;
				pNewItem->szText = pItem->szText;
				pNewItem->fDisable = pItem->fDisable;
				
				vtListBoxItem.push_back(pNewItem);
			}
			
			Plugin_SetFocus(pListBox, TRUE);
		}
		break;

		case WM_DIRECTUI_NOTIFY:
		{
			duPlugin *pPlugin = (duPlugin *)wParam;
			duNotify *pNotify = (duNotify *)lParam;

			if ((lstrcmpi(pPlugin->GetName(), pComboBox->m_szListBox) == 0) && (pNotify->uMsg == DUM_LBSELCHANGED))
			{
				duCtrlManager *pCtrlManager = GetCtrlManager(hWnd);
				if (pCtrlManager == NULL)
					return 0;

				duListBox *pListBox = (duListBox *)pCtrlManager->GetPluginByName(pComboBox->m_szListBox);
				if (pListBox == NULL)
					return 0;

				LPCTSTR lpszText = pListBox->GetItemText(pNotify->lParam);
				pComboBox->m_nCurSel = (int)pNotify->lParam;
				pComboBox->SetText(lpszText);
				pComboBox->SetEnd(TRUE);

				pComboBox->NotifyUser(DUM_COMBOSELCHANGE, pNotify->lParam, NULL);
			}
		}
		break;

		case WM_LBUTTONDOWN:
		{
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);

			duCtrlManager *pCtrlManager = GetCtrlManager(hWnd);
			if (pCtrlManager == NULL)
				return 0;

			duListBox *pListBox = (duListBox *)pCtrlManager->GetPluginByName(pComboBox->m_szListBox);
			if (pListBox == NULL)
				return 0;

			duScroll *pScroll = pListBox->GetVertScrollBar();
			if (pScroll == NULL)
			{
				pComboBox->SetEnd(TRUE);
				return 0;
			}

			duRect rcScroll;
			Plugin_GetRect(pScroll, &rcScroll);
			if (!rcScroll.PtInRect(pt))
				pComboBox->SetEnd(TRUE);
		}
		break;

		default:
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	
	return 0;
}
