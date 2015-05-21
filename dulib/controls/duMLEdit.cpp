//--------------------------------------------------------------------------
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duEdit.cpp
//
//  History:    Nov-23-2009   Steven Li  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duMLEdit.h"
#include "duCtrlManager.h"
#include "duEdit.h"

INT CALLBACK WordBreakProc(LPWSTR s, INT index, INT count, INT action)
{
	INT ret = 0;
	if(!s) return 0;

	switch (action) 
	{
	case WB_LEFT:
		{
			if (!count)
				break;

			if (index)
				index--;

			if (s[index] == ' ') 
			{
				while (index && (s[index] == ' '))
					index--;

				if (index) 
				{
					while (index && (s[index] != ' '))
						index--;

					if (s[index] == ' ')
						index++;
				}
			} 
			else 
			{
				while (index && (s[index] != ' '))
					index--;

				if (s[index] == ' ')
					index++;
			}
			ret = index;
		}
		break;

	case WB_RIGHT:
		{
			if (!count)
				break;

			if (index)
				index--;

			if (s[index] == ' ')
			{
				while ((index < count) && (s[index] == ' ')) 
					index++;
			}
			else 
			{
				while (s[index] && (s[index] != ' ') && (index < count))
					index++;

				while ((s[index] == ' ') && (index < count)) 
					index++;
			}
			ret = index;
		}
		break;

	case WB_ISDELIMITER:
		{
			ret = (s[index] == ' ');
		}
		break;

	default:
		break;
	}

	return ret;
}

wstring Replace(const wstring& orignStr, const wstring& oldStr, const wstring& newStr)
{
	size_t pos = 0;
	wstring tempStr = orignStr;
	int newStrLen = newStr.length();
	int oldStrLen = oldStr.length(); 
	while(1)
	{
		pos = tempStr.find(oldStr, pos);
		if (pos == wstring::npos)
			break;
		tempStr.replace(pos, oldStrLen, newStr);
		pos += newStrLen;
	}

	return tempStr;   
}

duMLEdit::duMLEdit()
{
	m_fUnicode = FALSE;
	m_szTextW = NULL;
	m_nBufferSize = 0;
	m_nBufferLimit = 0;
	m_hFont = NULL;
	m_nLineHeight = 0;
	m_nCharWidth = 0;
	m_nStyle = 0;
	m_nFlags = 0;
	m_nUndoInsertCount = 0;
	m_nUndoPosition = 0;
	m_szUndoTextW = NULL;
	m_nUndoBufferSize = 0;
	m_nSelectionStart = 0;
	m_nSelectionEnd = 0;
	m_nPasswordChar = '*';
	m_nLeftMargin = 0;
	m_nRightMargin = 0;
	m_rcFormat.SetRectEmpty();
	m_nTextWidth = 0;
	m_nRegionPosX = 0;
	m_nRegionPosY = 0;
	m_pfWordbreakProc = NULL;
	m_nLineCount = 0;
	m_fCaptureState = FALSE;
	m_fEnableState = FALSE;
	m_nLockCount = 0;
	m_pFirstLineDef = NULL;
	m_hLoc32W = NULL;
	m_hLoc32A = NULL;
	m_nXOffset = 0;
	m_nYOffset = 0;

	m_lTimerId = 0;
	m_fShowCaret = FALSE;
	m_fMouseIn = FALSE;
	m_fVertScroll = TRUE;

	ZeroMemory(m_szVertScroll, sizeof(TCHAR) * MAX_NAME);
	ZeroMemory(m_szHorzScroll, sizeof(TCHAR) * MAX_NAME);
	ZeroMemory(m_szFont, sizeof(TCHAR) * MAX_NAME);

	m_clrText = RGB(0,0,0);
	m_clrSelect = RGB(51, 153, 255);//GetSysColor(COLOR_HIGHLIGHT);
	m_clrSelectText = RGB(255, 255, 255);
}


duMLEdit::~duMLEdit()
{
	LINEDEF *pc, *pp;
	if (m_hLoc32W)
	{
		while (LocalUnlock(m_hLoc32W));
		LocalFree(m_hLoc32W);
	}

	if (m_hLoc32A)
	{
		while (LocalUnlock(m_hLoc32A)) ;
		LocalFree(m_hLoc32A);
	}

	pc = m_pFirstLineDef;
	while (pc)
	{
		pp = pc->next;
		HeapFree(GetProcessHeap(), 0, pc);
		pc = pp;
	}

	Plugin_UnHookWindowMessage(this);
}

void duMLEdit::Init(BOOL fUnicode)
{
	m_fUnicode = fUnicode;
	m_nStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOVSCROLL;

	m_fEnableState = !(m_nStyle & WS_DISABLED);
	if (m_nStyle & ES_NUMBER)
	{
	}
	else if (m_nStyle & ES_LOWERCASE)
	{
		m_nStyle &= ~ES_UPPERCASE;
	}

	m_nBufferLimit = BUFLIMIT_INITIAL;
	if (m_nStyle & ES_PASSWORD)
		m_nPasswordChar = '*';

	UINT alloc_size = ROUND_TO_GROW((m_nBufferSize + 1) * sizeof(WCHAR));
	if(!(m_hLoc32W = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, alloc_size)))
		goto cleanup;

	m_nBufferSize = LocalSize(m_hLoc32W)/sizeof(WCHAR) - 1;
	if (!(m_szUndoTextW = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (m_nBufferSize + 1) * sizeof(WCHAR))))
		goto cleanup;

	m_nUndoBufferSize = m_nBufferSize;
	m_nLineCount = 1;
	LockBuffer();

	return;

cleanup:
	HeapFree(GetProcessHeap(), 0, m_pFirstLineDef);
	HeapFree(GetProcessHeap(), 0, m_szUndoTextW);
	if (m_hLoc32W)
		LocalFree(m_hLoc32W);
}

void WINAPI duMLEdit::RegisterControlProperty()
{
	RegisterProperty(_T("vertscrollbar"),  DU_PROPERTY_STRING, m_szVertScroll);
	RegisterProperty(_T("horzscrollbar"),  DU_PROPERTY_STRING, m_szHorzScroll);
	RegisterProperty(_T("font"), DU_PROPERTY_FONT, m_szFont);
	RegisterProperty(_T("textcolor"), DU_PROPERTY_COLOR, &m_clrText);
	RegisterProperty(_T("selectcolor"), DU_PROPERTY_COLOR, &m_clrSelect);
	RegisterProperty(_T("selecttextcolor"), DU_PROPERTY_COLOR, &m_clrSelectText);
}

void WINAPI duMLEdit::OnCreate()
{
	Init(TRUE);

#ifdef UNICODE
 	m_fUnicode = TRUE;
#else
	m_fUnicode = FALSE;
#endif

 	m_nStyle = ES_MULTILINE |WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOVSCROLL;
	m_fEnableState = !(m_nStyle & WS_DISABLED);

    if (m_nStyle & ES_NUMBER) {
        ; /* do not override the ES_NUMBER */
    }  
	else if (m_nStyle & ES_LOWERCASE)
		m_nStyle &= ~ES_UPPERCASE;

	if (m_nStyle & ES_MULTILINE) 
	{
		m_nBufferLimit = BUFLIMIT_MULTI;
		if (IsVertScrollBarShow())
			m_nStyle |= ES_AUTOVSCROLL;

		if (IsHorzScrollBarShow())
			m_nStyle |= ES_AUTOHSCROLL;

		m_nStyle &= ~ES_PASSWORD;
		if ((m_nStyle & ES_CENTER) || (m_nStyle & ES_RIGHT))
		{
			if (m_nStyle & ES_RIGHT)
				m_nStyle &= ~ES_CENTER;

			m_nStyle &= ~WS_HSCROLL;
			m_nStyle &= ~ES_AUTOHSCROLL;
		}
	}
	else
	{
		m_nBufferLimit = BUFLIMIT_SINGLE;
		if ((m_nStyle & ES_RIGHT) && (m_nStyle & ES_CENTER))
			m_nStyle &= ~ES_CENTER;
		m_nStyle &= ~WS_HSCROLL;
		m_nStyle &= ~WS_VSCROLL;
		if (m_nStyle & ES_PASSWORD)
			m_nPasswordChar = '*';
	}

	UINT alloc_size = ROUND_TO_GROW((m_nBufferSize + 1) * sizeof(WCHAR));
	m_hLoc32W = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, alloc_size);
	m_nBufferSize = LocalSize(m_hLoc32W)/sizeof(WCHAR) - 1;
	m_szUndoTextW = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (m_nBufferSize + 1) * sizeof(WCHAR));
	m_nUndoBufferSize = m_nBufferSize;

	if (m_nStyle & ES_MULTILINE)
		m_pFirstLineDef = (LINEDEF *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LINEDEF));

	m_nLineCount = 1;

	//
	// 设置控件文本
	//
	m_lTimerId = GetUniTimer();

	duFont *pFont = (duFont *)GetResObj(m_szFont, DU_RES_FONT);
	if (pFont)
		m_hFont = pFont->GetHFont();
	else
		m_hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);

	OnSetFont(m_hFont);
	EmptyUndoBuffer();

	duRect rcEdit;
	SetRectEmpty(&rcEdit);
	GetRect(&rcEdit);
	rcEdit.OffsetRect(-rcEdit.left, -rcEdit.top);
	SetRectNP(&rcEdit);

	Plugin_HookWindowMessage(this);

	UpdateScrollInfo();
}

void WINAPI duMLEdit::DrawObject(HDC hDC)
{
	INT i;
	HFONT old_font = 0;
	duRect rc;
	duRect rcClient;
	duRect rcLine;
	duRect rcRgn;
	duRect rcEdit;
	BOOL rev = m_fEnableState && ((m_nFlags & EF_FOCUSED) || (m_nStyle & ES_NOHIDESEL));

	Plugin_GetRect(this, &rcEdit);
	rcEdit.OffsetRect(-rcEdit.left, -rcEdit.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rcEdit, GetState(), NULL, GetAlpha());

	if (wcslen(m_szTextW) <= 0)
		return;

	if (m_nStyle & ES_MULTILINE)
	{
		INT vlc = (m_rcFormat.bottom - m_rcFormat.top) / m_nLineHeight;
		for (i = m_nYOffset ; i <= min(m_nYOffset + vlc, m_nYOffset + m_nLineCount - 1) ; i++)
		{
			GetLineRect(i, 0, -1, &rcLine);
			PaintLine(hDC, i, rev);
		}
	}
}

void duMLEdit::PaintLine(HDC dc, INT line, BOOL rev)
{
	INT s = m_nSelectionStart;
	INT e = m_nSelectionEnd;
	INT li;
	INT ll;
	INT x;
	INT y;
	LRESULT pos;

	if (m_nStyle & ES_MULTILINE) 
	{
		INT vlc = (m_rcFormat.bottom - m_rcFormat.top) / m_nLineHeight;
		if ((line < m_nYOffset) || (line > m_nYOffset + vlc) || (line >= m_nLineCount))
			return;
	}
	else if (line)
		return;

	pos = PosFromChar(LineIndex(line), FALSE);
	x = (short)LOWORD(pos);
	y = (short)HIWORD(pos);
	li = LineIndex(line);
	ll = LineLength(li);
	s = min(m_nSelectionStart, m_nSelectionEnd);
	e = max(m_nSelectionStart, m_nSelectionEnd);
	s = min(li + ll, max(li, s));
	e = min(li + ll, max(li, e));
	if (rev && (s != e) && ((m_nFlags & EF_FOCUSED) || (m_nStyle & ES_NOHIDESEL)))
	{
		x += PaintText(dc, x, y, line, 0, s - li, FALSE);
		x += PaintText(dc, x, y, line, s - li, e - s, TRUE);
		x += PaintText(dc, x, y, line, e - li, li + ll - e, FALSE);
	}
	else
		x += PaintText(dc, x, y, line, 0, ll, FALSE);
}

INT duMLEdit::PaintText(HDC dc, INT x, INT y, INT line, INT col, INT count, BOOL rev)
{
	COLORREF BkColor;
	COLORREF TextColor;
	HFONT hUnderline = 0;
	HFONT old_font = 0;
	INT ret;
	INT li;
	INT BkMode;

	if (!count)
		return 0;

	BkMode = GetBkMode(dc);
	BkColor = GetBkColor(dc);
	TextColor = GetTextColor(dc);
	li = LineIndex(line);
	if (m_nStyle & ES_MULTILINE)
		ret = _TextOutW(dc, m_hFont, x, y, m_szTextW + li + col, count, rev);

	return ret;
}

LRESULT WINAPI duMLEdit::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//if (uMsg == WM_MOUSEMOVE)
	//{
	//	if (!Plugin_IsValid(this) || !Plugin_IsVisible(this))
	//		return 0;

	//	POINT pt;
	//	pt.x = GET_X_LPARAM(lParam);
	//	pt.y = GET_Y_LPARAM(lParam);

	//	CRect rcEdit;
	//	Plugin_GetRect(this, &rcEdit);
	//	if (rcEdit.PtInRect(pt))
	//	{
	//		if (!m_fMouseIn)
	//		{
	//			m_fMouseIn = TRUE;
	//			Plugin_SetState(this, DU_STATE_OVER);
	//			Plugin_Redraw(this, TRUE);
	//			return 0;
	//		}
	//	}
	//	else
	//	{
	//		if (m_fMouseIn)
	//		{
	//			m_fMouseIn = FALSE;
	//			Plugin_SetState(this, DU_STATE_NORMAL);
	//			Plugin_Redraw(this, TRUE);
	//		}
	//	}
	//}

	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return 0;

	if (pCtrlManager->GetFocusPlugin() != this)
		return 0;

	LockBuffer();
	switch(uMsg)
	{
		case WM_CHAR:
			{
				WCHAR charW;
				charW = wParam;
				OnChar(charW);
				_RedrawEdit();
				break;
			}

		default:
			break;
	}
	
	UnlockBuffer(FALSE);
	return 0;
}

void WINAPI duMLEdit::OnVScroll(ScrollBarAction sbAction, int nPos)
{
	switch (sbAction)
	{
	case SCROLL_NO_WHERE:
		m_nYOffset = 0;
		break;

	case SCROLL_UP:	
	case SCROLL_DOWN:
	case SCROLL_PAGEDOWN:
	case SCROLL_PAGEUP:
	case SCROLL_TRACK:
	case SCROLL_WHEEL_UP:
	case SCROLL_WHEEL_DOWN:
		{
			if (m_nYOffset != nPos)
			{
				m_nYOffset = nPos;
				Plugin_Redraw(this, TRUE);
			}
		}
		break;

	default:
		break;
	}
}

void WINAPI duMLEdit::OnHScroll(ScrollBarAction sbAction, int nPos)
{
	switch (sbAction)
	{
	case SCROLL_NO_WHERE:
		m_nXOffset = 0;
		break;

	case SCROLL_UP:
	case SCROLL_DOWN:
	case SCROLL_TRACK:
	case SCROLL_PAGEUP:
	case SCROLL_PAGEDOWN:
	case SCROLL_WHEEL_UP:
	case SCROLL_WHEEL_DOWN:
		{
			if (m_nXOffset != nPos)
			{
				m_nXOffset = nPos;
				Plugin_Redraw(this, TRUE);
			}
		}
		break;

	default:
		break;
	}
}

void WINAPI duMLEdit::OnKeyDown(UINT key, UINT nRepCnt, UINT nFlags)
{
	BOOL shift;
	BOOL control;

	if (GetKeyState(VK_MENU) & 0x8000)
		return;

	shift = GetKeyState(VK_SHIFT) & 0x8000;
	control = GetKeyState(VK_CONTROL) & 0x8000;

	switch (key) 
	{
	case VK_F4:
	case VK_UP:
		if (key == VK_F4)
			break;

	case VK_LEFT:
		if ((m_nStyle & ES_MULTILINE) && (key == VK_UP))
		{
			MoveUp_ML(shift);
			_RedrawEdit();
		}
		else
		{
			if (control)
			{
				MoveWordBackward(shift);
				_RedrawEdit();
			}
			else
			{
				MoveBackward(shift);
				_RedrawEdit();
			}
		}
		break;
	case VK_DOWN:
	case VK_RIGHT:
		if ((m_nStyle & ES_MULTILINE) && (key == VK_DOWN))
		{
			MoveDown_ML(shift);
			_RedrawEdit();
		}
		else if (control)
		{
			MoveWordForward(shift);
			_RedrawEdit();
		}
		else
		{
			MoveForward(shift);
			_RedrawEdit();
		}
		break;
	case VK_HOME:
		{
			if (control)
			{
				SetSel(0, 0, FALSE);
				ScrollCaret();
			}
			else
				MoveHome(shift);
			
			_RedrawEdit();
		}
		break;
	case VK_END:
		{
			if (control && m_szTextW)
			{
				INT len = lstrlenW(m_szTextW);
				SetSel(len, len, FALSE);
				ScrollCaret();
			}
			else
				MoveEnd(shift);

			_RedrawEdit();
		}
		break;
	case VK_PRIOR:
		{
			MovePageUp_ML(shift);
			_RedrawEdit();
		}
		break;
	case VK_NEXT:
		{
			MovePageDown_ML(shift);
			_RedrawEdit();
		}
		break;
	case VK_DELETE:
		if (!(m_nStyle & ES_READONLY) && !(shift && control)) 
		{
			if (m_nSelectionStart != m_nSelectionEnd) 
			{
				if (shift)
				{
					OnCut();
					_RedrawEdit();
				}
				else
				{
					OnClear();
					_RedrawEdit();
				}
			} 
			else 
			{
				if (shift) 
				{
					/* delete character left of caret */
					SetSel((UINT)-1, 0, FALSE);
					MoveBackward(TRUE);
					OnClear();
					_RedrawEdit();
				}
				else if (control) 
				{
					/* delete to end of line */
					SetSel((UINT)-1, 0, FALSE);
					MoveEnd(TRUE);
					OnClear();
					_RedrawEdit();
				} 
				else 
				{
					/* delete character right of caret */
					SetSel((UINT)-1, 0, FALSE);
					MoveForward(TRUE);
					OnClear();
					_RedrawEdit();
				}
			}
		}
		break;
	case VK_INSERT:
		if (shift)
		{
			if (!(m_nStyle & ES_READONLY))
			{
				OnPaste();
				_RedrawEdit();
			}
		} 
		else if (control)
		{
			OnCopy();
			_RedrawEdit();
		}
		break;
	case VK_RETURN:
		/* If the edit doesn't want the return send a message to the default object */
		if(!(m_nStyle & ES_WANTRETURN))
		{
			HWND hwndParent = ::GetParent(m_hWnd);
			DWORD dw = SendMessageW(hwndParent, DM_GETDEFID, 0, 0);
			if (HIWORD(dw) == DC_HASDEFID)
			{
				//SendMessageW(hwndParent, WM_COMMAND, MAKEWPARAM(LOWORD(dw), BN_CLICKED), (LPARAM)GetDlgItem(hwndParent, LOWORD(dw)));
			}
		}
		break;
	}
}

BOOL WINAPI duMLEdit::OnKillFocus(duPlugin *pNewFocus)
{
	m_nFlags &= ~EF_FOCUSED;

	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return FALSE;

	pCtrlManager->_DestroyCaret();
	
	//
	//
	//
	m_fShowCaret = FALSE;
	return FALSE;
}

BOOL WINAPI duMLEdit::OnSetFocus(duPlugin *pOldFocus)
{
	m_fShowCaret = TRUE;
	m_nFlags |= EF_FOCUSED;

	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return FALSE;

	pCtrlManager->_CreateCaret(1, m_nLineHeight);
	SetCaretPos(m_nSelectionEnd, m_nFlags & EF_AFTER_WRAP);
	pCtrlManager->_ShowCaret();
	return TRUE;
}

void WINAPI duMLEdit::OnMouseMove(POINT pt)
{
	duRect rcEdit;
	rcEdit.SetRectEmpty();
	GetRect(&rcEdit);

	if (!m_fCaptureState || Plugin_GetCapture(this) != this)
		return;

	INT x,y;
	x = pt.x - rcEdit.left;
	y = pt.y - rcEdit.top;

	LONG prex, prey;
	prex = x;
	prey = y;

	ConfinePoint(&x, &y);

	BOOL after_wrap = FALSE;
	INT e = CharFromPos(x, y, &after_wrap);
	SetSel(m_nSelectionStart, e, after_wrap);
	SetCaretPos(m_nSelectionEnd,m_nFlags & EF_AFTER_WRAP);
	Plugin_Redraw(this, TRUE);
}

void WINAPI duMLEdit::OnMouseDbClick(POINT pt)
{
	//INT s;
	//INT e = m_nSelectionEnd;
	//INT l;
	//INT li;
	//INT ll;

	//m_fCaptureState = TRUE;
	//Plugin_SetCapture(this);

	//l = 0;
	//li = 0;
	//ll = GetTextLength();
	//s = li + CallWordBreakProc(li, e - li, ll, WB_LEFT);
	//e = li + CallWordBreakProc(li, e - li, ll, WB_RIGHT);
	//SetSel(s, e, m_nFlags & EF_AFTER_WRAP);
	//ScrollCaret();
	//m_nRegionPosX = m_nRegionPosY = 0;
	//Plugin_SetTimer(this, m_lTimerId, 100);
	//Plugin_Redraw(this, TRUE);
}

void WINAPI duMLEdit::OnMouseLDown(POINT pt)
{
	duRect rcEdit;
	rcEdit.SetRectEmpty();
	GetRect(&rcEdit);

	INT x,y;
	x = pt.x - rcEdit.left;
	y = pt.y - rcEdit.top;

	m_fCaptureState = TRUE;
	Plugin_SetCapture(this);

	BOOL after_wrap = FALSE;
	ConfinePoint(&x, &y);
	INT e = CharFromPos(x, y, &after_wrap);
	SetSel(e, e, FALSE);
	ScrollCaret();
	m_nRegionPosX = m_nRegionPosY = 0;
	Plugin_SetTimer(this, m_lTimerId, 100);

	if (!(m_nFlags & EF_FOCUSED))
		::SetFocus(m_hWnd);
	else
	{
		// 当此plugin是focus状态，但是窗口失去焦点再切换回来时，focus不变，不会重新创建显示Caret
		duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
		if (pCtrlManager)
		{
			pCtrlManager->_DestroyCaret();
			pCtrlManager->_CreateCaret(1, m_nLineHeight);
			SetCaretPos(m_nSelectionEnd, m_nFlags & EF_AFTER_WRAP);
			pCtrlManager->_ShowCaret();
		}
	}

	Plugin_Redraw(this, TRUE);
}

void WINAPI duMLEdit::OnMouseLUp(POINT pt)
{
	if (m_fCaptureState)
	{
		Plugin_KillTimer(this, m_lTimerId);
		if (Plugin_GetCapture(this) == this)
			Plugin_ReleaseCapture(this);
	}
	m_fCaptureState = FALSE;
	Plugin_Redraw(this, TRUE);
}

void WINAPI duMLEdit::OnMouseRDown(POINT pt)
{
	if (!(m_nFlags & EF_FOCUSED))
		::SetFocus(m_hWnd);
	else
	{
		// 当此plugin是focus状态，但是窗口失去焦点再切换回来时，focus不变，不会重新创建显示Caret
		duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
		if (pCtrlManager)
		{
			pCtrlManager->_DestroyCaret();
			pCtrlManager->_CreateCaret(1, m_nLineHeight);
			SetCaretPos(m_nSelectionEnd, m_nFlags & EF_AFTER_WRAP);
			pCtrlManager->_ShowCaret();
		}
	}
}

void WINAPI duMLEdit::OnMouseRUp(POINT pt)
{
}

void WINAPI duMLEdit::OnTimer(UINT nEventId)
{
	if (nEventId != m_lTimerId)
		return;

	if (m_nRegionPosX < 0)
		MoveBackward(TRUE);
	else if (m_nRegionPosX > 0)
		MoveForward(TRUE);
}

void WINAPI duMLEdit::OnMouseWheel(UINT fwKeys, int zDelta, POINT pt)
{
	duScroll *pVertScroll = (duScroll *)GetPluginByName(m_szVertScroll);
	if (pVertScroll)
		pVertScroll->OnMouseWheel(fwKeys, zDelta, pt);
}

BOOL WINAPI duMLEdit::OnSetCursor()
{
	duPoint pt;
	::GetCursorPos(&pt);

	duRect rcWindow;
	::GetWindowRect(m_hWnd, &rcWindow);

	pt.Offset(-rcWindow.left, -rcWindow.top);

	duRect rcEdit;
	Plugin_GetRect(this, &rcEdit);
	pt.Offset(-rcEdit.left, -rcEdit.top);

	if (::PtInRect(&m_rcFormat, pt))
	{
		::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
		return TRUE;
	}

	return (BOOL)::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}

BOOL duMLEdit::CanUndo()
{
	return (m_nUndoInsertCount || lstrlenW(m_szUndoTextW));
}

void duMLEdit::EmptyUndoBuffer()
{
	m_nUndoInsertCount = 0;
	*m_szUndoTextW = '\0';
}

void duMLEdit::OnClear()
{
	/* Protect read-only edit control from modification */
	if(m_nStyle & ES_READONLY)
	    return;

	ReplaceSel(TRUE, empty_stringW, TRUE, TRUE);
}

void duMLEdit::OnCut()
{
	OnCopy();
	OnClear();
}

void duMLEdit::BuildLineDefs_ML(INT istart, INT iend, INT delta, HRGN hrgn)
{
	HDC dc;
	HFONT old_font = 0;
	LPWSTR current_position, cp;
	INT fw;
	LINEDEF *current_line;
	LINEDEF *previous_line;
	LINEDEF *start_line;
	INT line_index = 0, nstart_line = 0, nstart_index = 0;
	INT line_count = m_nLineCount;
	INT orig_net_length;
	RECT rc;

	if (istart == iend && delta == 0)
		return;

	dc = GetDC(m_hWnd);
	if (m_hFont)
		old_font = (HFONT)SelectObject(dc, m_hFont);

	previous_line = NULL;
	current_line = m_pFirstLineDef;

	/* Find starting line. istart must lie inside an existing line or
	 * at the end of buffer */
	do 
	{
		if (istart < current_line->index + current_line->length || current_line->ending == END_0)
			break;

		previous_line = current_line;
		current_line = current_line->next;
		line_index++;
	} while (current_line);

	if (!current_line) /* Error occurred start is not inside previous buffer */
	{
		//FIXME(" modification occurred outside buffer\n");
		ReleaseDC(m_hWnd, dc);
		return;
	}

	/* Remember start of modifications in order to calculate update region */
	nstart_line = line_index;
	nstart_index = current_line->index;

	/* We must start to reformat from the previous line since the modifications
	 * may have caused the line to wrap upwards. */
	if (!(m_nStyle & ES_AUTOHSCROLL) && line_index > 0)
	{
		line_index--;
		current_line = previous_line;
	}
	start_line = current_line;

	fw = m_rcFormat.right - m_rcFormat.left;
	current_position = m_szTextW + current_line->index;
	do 
	{
		if (current_line != start_line)
		{
			if (!current_line || current_line->index + delta > current_position - m_szTextW)
			{
				/* The buffer has been expanded, create a new line and
				   insert it into the link list */
				LINEDEF *new_line = (LINEDEF *)HeapAlloc(GetProcessHeap(), 0, sizeof(LINEDEF));
				new_line->next = previous_line->next;
				previous_line->next = new_line;
				current_line = new_line;
				m_nLineCount++;
			}
			else if (current_line->index + delta < current_position - m_szTextW)
			{
				/* The previous line merged with this line so we delete this extra entry */
				previous_line->next = current_line->next;
				HeapFree(GetProcessHeap(), 0, current_line);
				current_line = previous_line->next;
				m_nLineCount--;
				continue;
			}
			else /* current_line->index + delta == current_position */
			{
				if (current_position - m_szTextW > iend)
					break; /* We reached end of line modifications */
				/* else recalulate this line */
			}
		}

		current_line->index = current_position - m_szTextW;
		orig_net_length = current_line->net_length;

		/* Find end of line */
		cp = current_position;
		while (*cp)
		{
			if (*cp == '\n') 
				break;

			if ((*cp == '\r') && (*(cp + 1) == '\n'))
				break;
			cp++;
		}

		/* Mark type of line termination */
		if (!(*cp)) 
		{
			current_line->ending = END_0;
			current_line->net_length = lstrlenW(current_position);
		}
		else if ((cp > current_position) && (*(cp - 1) == '\r')) 
		{
			current_line->ending = END_SOFT;
			current_line->net_length = cp - current_position - 1;
		}
		else if (*cp == '\n') 
		{
			current_line->ending = END_RICH;
			current_line->net_length = cp - current_position;
		}
		else 
		{
			current_line->ending = END_HARD;
			current_line->net_length = cp - current_position;
		}

		/* Calculate line width */
		current_line->width = (INT)LOWORD(_GetTabbedTextExtentW(dc, current_position, current_line->net_length, 0, NULL));

		/* FIXME: check here for lines that are too wide even in AUTOHSCROLL (> 32767 ???) */
		if (!(m_nStyle & ES_AUTOHSCROLL)) 
		{
		   if (current_line->width > fw) 
		   {
				INT next = 0;
				INT prev;
				do
				{
					prev = next;
					next = CallWordBreakProc(current_position - m_szTextW, prev + 1, current_line->net_length, WB_RIGHT);
					current_line->width = (INT)LOWORD(_GetTabbedTextExtentW(dc, current_position, next, 0, NULL));
				} while (current_line->width <= fw);

				if (!prev) 
				{ 
					/* Didn't find a line break so force a break */
					next = 0;
					do 
					{
						prev = next;
						next++;
						current_line->width = (INT)LOWORD(_GetTabbedTextExtentW(dc, current_position, next, 0, NULL));
					} while (current_line->width <= fw);

					if (!prev)
						prev = 1;
				}

				/* If the first line we are calculating, wrapped before istart, we must
				 * adjust istart in order for this to be reflected in the update region. */
				if (current_line->index == nstart_index && istart > current_line->index + prev)
					istart = current_line->index + prev;

				/* else if we are updating the previous line before the first line we
				* are re-calculating and it expanded */
				else if (current_line == start_line && current_line->index != nstart_index && orig_net_length < prev)
				{
					/* Line expanded due to an upwards line wrap so we must partially include
					 * previous line in update region */
					nstart_line = line_index;
					nstart_index = current_line->index;
					istart = current_line->index + orig_net_length;
				}

				current_line->net_length = prev;
				current_line->ending = END_WRAP;
				current_line->width = (INT)LOWORD(_GetTabbedTextExtentW(dc, current_position, current_line->net_length, 0, NULL));
		    }
		    else if (orig_net_length <  current_line->net_length  && current_line == start_line && current_line->index != nstart_index) 
			{
				/* The previous line expanded but it's still not as wide as the client rect */
				/* The expansion is due to an upwards line wrap so we must partially include
					it in the update region */
				nstart_line = line_index;
				nstart_index = current_line->index;
				istart = current_line->index + orig_net_length;
		    }
		}

		/* Adjust length to include line termination */
		switch (current_line->ending) 
		{
		case END_SOFT:
			current_line->length = current_line->net_length + 3;
			break;

		case END_RICH:
			current_line->length = current_line->net_length + 1;
			break;

		case END_HARD:
			current_line->length = current_line->net_length + 2;
			break;

		case END_WRAP:
		case END_0:
			current_line->length = current_line->net_length;
			break;
		}

		m_nTextWidth = max(m_nTextWidth, current_line->width);
		current_position += current_line->length;
		previous_line = current_line;
		current_line = current_line->next;
		line_index++;
	} while (previous_line->ending != END_0);

	/* Finish adjusting line indexes by delta or remove hanging lines */
	if (previous_line->ending == END_0)
	{
		LINEDEF *pnext = NULL;

		previous_line->next = NULL;
		while (current_line)
		{
			pnext = current_line->next;
			HeapFree(GetProcessHeap(), 0, current_line);
			current_line = pnext;
			m_nLineCount--;
		}
	}
	else if (delta != 0)
	{
		while (current_line)
		{
			current_line->index += delta;
			current_line = current_line->next;
		}
	}

	/* Calculate rest of modification rectangle */
	if (hrgn)
	{
		HRGN tmphrgn;
	   /*
		* We calculate two rectangles. One for the first line which may have
		* an indent with respect to the format rect. The other is a format-width
		* rectangle that spans the rest of the lines that changed or moved.
		*/
		rc.top = m_rcFormat.top + nstart_line * m_nLineHeight - (m_nYOffset * m_nLineHeight); /* Adjust for vertical scrollbar */
		rc.bottom = rc.top + m_nLineHeight;
		if ((m_nStyle & ES_CENTER) || (m_nStyle & ES_RIGHT))
			rc.left = m_rcFormat.left;
		else
			rc.left = m_rcFormat.left + (INT)LOWORD(_GetTabbedTextExtentW(dc, m_szTextW + nstart_index, istart - nstart_index, 0, NULL)) - m_nXOffset; /* Adjust for horz scroll */

		rc.right = m_rcFormat.right;
		SetRectRgn(hrgn, rc.left, rc.top, rc.right, rc.bottom);

		rc.top = rc.bottom;
		rc.left = m_rcFormat.left;
		rc.right = m_rcFormat.right;
	   /*
		* If lines were added or removed we must re-paint the remainder of the
	    * lines since the remaining lines were either shifted up or down.
		*/
		if (line_count < m_nLineCount) /* We added lines */
			rc.bottom = m_nLineCount * m_nLineHeight;
		else if (line_count > m_nLineCount) /* We removed lines */
			rc.bottom = line_count * m_nLineHeight;
		else
			rc.bottom = line_index * m_nLineHeight;
		rc.bottom += m_rcFormat.top;
		rc.bottom -= (m_nYOffset * m_nLineHeight); /* Adjust for vertical scrollbar */
		tmphrgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
		CombineRgn(hrgn, hrgn, tmphrgn, RGN_OR);
		DeleteObject(tmphrgn);
	}

	if (m_hFont)
		SelectObject(dc, old_font);

	ReleaseDC(m_hWnd, dc);
}

INT duMLEdit::CallWordBreakProc(INT start, INT index, INT count, INT action)
{
    INT ret;
	if (m_pfWordbreakProc)
	{
		if(m_fUnicode)
	    {
			EDITWORDBREAKPROCW wbpW = (EDITWORDBREAKPROCW)m_pfWordbreakProc;
			ret = wbpW(m_szTextW + start, index, count, action);
	    }
	    else
	    {
			EDITWORDBREAKPROCA wbpA = (EDITWORDBREAKPROCA)m_pfWordbreakProc;
			INT countA;
			CHAR *textA;

			countA = WideCharToMultiByte(CP_ACP, 0, m_szTextW + start, count, NULL, 0, NULL, NULL);
			textA = (CHAR *)HeapAlloc(GetProcessHeap(), 0, countA);
			WideCharToMultiByte(CP_ACP, 0, m_szTextW + start, count, textA, countA, NULL, NULL);
			ret = wbpA(textA, index, countA, action);
			HeapFree(GetProcessHeap(), 0, textA);
	    }
	}
	else
		ret = WordBreakProc(m_szTextW + start, index, count, action);

    return ret;
}

INT duMLEdit::CharFromPos(INT x, INT y, LPBOOL after_wrap)
{
	INT index;
	HDC dc;
	HFONT old_font = 0;
	INT x_high = 0, x_low = 0;

	if (m_nStyle & ES_MULTILINE) 
	{
		INT line = (y - m_rcFormat.top) / m_nLineHeight + m_nYOffset;
		INT line_index = 0;
		LINEDEF *line_def = m_pFirstLineDef;
		INT low, high;

		while ((line > 0) && line_def->next) 
		{
			line_index += line_def->length;
			line_def = line_def->next;
			line--;
		}

		x += m_nXOffset - m_rcFormat.left;
		if (m_nStyle & ES_RIGHT)
			x -= (m_rcFormat.right - m_rcFormat.left) - line_def->width;
		else if (m_nStyle & ES_CENTER)
			x -= ((m_rcFormat.right - m_rcFormat.left) - line_def->width) / 2;

		if (x >= line_def->width) 
		{
			if (after_wrap)
				*after_wrap = (line_def->ending == END_WRAP);
			return line_index + line_def->net_length;
		}

		if (x <= 0) 
		{
			if (after_wrap)
				*after_wrap = FALSE;
			return line_index;
		}

		dc = GetDC(m_hWnd);
		if (m_hFont)
			old_font = (HFONT)SelectObject(dc, m_hFont);

		low = line_index;
		high = line_index + line_def->net_length + 1;
		while (low < high - 1)
		{
			INT mid = (low + high) / 2;
			INT x_now = LOWORD(_GetTabbedTextExtentW(dc, m_szTextW + line_index, mid - line_index, 0, NULL));
			if (x_now > x) 
			{
				high = mid;
				x_high = x_now;
			}
			else 
			{
				low = mid;
				x_low = x_now;
			}
		}

		if (abs(x_high - x) + 1 <= abs(x_low - x))
			index = high;
		else
			index = low;

		if (after_wrap)
			*after_wrap = ((index == line_index + line_def->net_length) && (line_def->ending == END_WRAP));
	} 

	if (m_hFont)
		SelectObject(dc, old_font);

	ReleaseDC(m_hWnd, dc);
	return index;
}

void duMLEdit::ConfinePoint(INT *x, INT *y)
{
	*x = min(max(*x, m_rcFormat.left), m_rcFormat.right - 1);
	*y = min(max(*y, m_rcFormat.top), m_rcFormat.bottom - 1);
}

void duMLEdit::GetLineRect(INT line, INT scol, INT ecol, LPRECT rc)
{
	INT line_index =  LineIndex(line);

	if (m_nStyle & ES_MULTILINE)
		rc->top = m_rcFormat.top + (line - m_nYOffset) * m_nLineHeight;

	rc->bottom = rc->top + m_nLineHeight;
	rc->left = (scol == 0) ? m_rcFormat.left : (short)LOWORD(PosFromChar(line_index + scol, TRUE));
	rc->right = (ecol == -1) ? m_rcFormat.right : (short)LOWORD(PosFromChar(line_index + ecol, TRUE));
}

void duMLEdit::LockBuffer()
{
	if (!m_szTextW) 
	{
	    CHAR *textA = NULL;
	    UINT countA = 0;

	    if(m_hLoc32W)
	    {
			if(m_hLoc32A)
			{
				textA = (CHAR *)LocalLock(m_hLoc32A);
				countA = strlen(textA) + 1;
			}
	    }
	    else 
		{
			return;
	    }

	    if(textA)
	    {
			HLOCAL hloc32W_new;
			UINT countW_new = MultiByteToWideChar(CP_ACP, 0, textA, countA, NULL, 0);
			if(countW_new > m_nBufferSize + 1)
			{
				UINT alloc_size = ROUND_TO_GROW(countW_new * sizeof(WCHAR));
				hloc32W_new = LocalReAlloc(m_hLoc32W, alloc_size, LMEM_MOVEABLE | LMEM_ZEROINIT);
				if(hloc32W_new)
				{
					m_hLoc32W = hloc32W_new;
					m_nBufferSize = LocalSize(hloc32W_new)/sizeof(WCHAR) - 1;
				}
			}
	    }

	    /*TRACE("Locking 32-bit UNICODE buffer\n");*/
	    m_szTextW = (LPWSTR)LocalLock(m_hLoc32W);

	    if(textA)
	    {
			MultiByteToWideChar(CP_ACP, 0, textA, countA, m_szTextW, m_nBufferSize + 1);
		    LocalUnlock(m_hLoc32A);
	    }
	}
	m_nLockCount++;
}

BOOL duMLEdit::MakeFit(UINT size)
{
	HLOCAL hNew32W;

	if (size <= m_nBufferSize)
		return TRUE;

	int nOldBufferSize = m_nBufferSize + 1;
	TCHAR *lpTemp = new TCHAR[nOldBufferSize];
	memcpy(lpTemp, m_szTextW, nOldBufferSize * sizeof(TCHAR));

	/* Force edit to unlock it's buffer. text now NULL */
	UnlockBuffer(TRUE);

	if (m_hLoc32W)
	{
	    UINT alloc_size = ROUND_TO_GROW((size + 1) * sizeof(WCHAR));
	    if ((hNew32W = LocalReAlloc(m_hLoc32W, alloc_size, LMEM_MOVEABLE | LMEM_ZEROINIT))) 
		{
			m_hLoc32W = hNew32W;
			m_nBufferSize = LocalSize(hNew32W)/sizeof(WCHAR) - 1;
	    }
	}

	LockBuffer();

	memcpy(m_szTextW, lpTemp, nOldBufferSize * sizeof(TCHAR));
	delete [] lpTemp;

	if (m_nBufferSize < size)
	{
		//EDIT_NOTIFY_PARENT(EN_ERRSPACE);
		return FALSE;
	}
	else 
	{
		return TRUE;
	}
}

BOOL duMLEdit::MakeUndoFit(UINT size)
{
	UINT alloc_size;

	if (size <= m_nUndoBufferSize)
		return TRUE;

	alloc_size = ROUND_TO_GROW((size + 1) * sizeof(WCHAR));
	if ((m_szUndoTextW = (LPWSTR)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_szUndoTextW, alloc_size))) 
	{
		m_nUndoBufferSize = alloc_size/sizeof(WCHAR) - 1;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void duMLEdit::MoveBackward(BOOL extend)
{
	INT e = m_nSelectionEnd;

	if (e)
	{
		e--;
		if ((m_nStyle & ES_MULTILINE) && e && (m_szTextW[e - 1] == '\r') && (m_szTextW[e] == '\n')) 
		{
			e--;
			if (e && (m_szTextW[e - 1] == '\r'))
				e--;
		}
	}

	SetSel(extend ? m_nSelectionStart : e, e, FALSE);
	ScrollCaret();
}

void duMLEdit::MoveDown_ML(BOOL extend)
{
	INT s = m_nSelectionStart;
	INT e = m_nSelectionEnd;
	BOOL after_wrap = (m_nFlags & EF_AFTER_WRAP);
	LRESULT pos = PosFromChar(e, after_wrap);
	INT x = (short)LOWORD(pos);
	INT y = (short)HIWORD(pos);

	e = CharFromPos(x, y + m_nLineHeight, &after_wrap);
	if (!extend)
		s = e;

	SetSel(s, e, after_wrap);
	ScrollCaret();
}

void duMLEdit::MoveEnd(BOOL extend)
{
	BOOL after_wrap = FALSE;
	INT e;

	/* Pass a high value in x to make sure of receiving the end of the line */
	if (m_nStyle & ES_MULTILINE)
		e = CharFromPos(0x3fffffff, HIWORD(PosFromChar(m_nSelectionEnd, m_nFlags & EF_AFTER_WRAP)), &after_wrap);
	else
		e = lstrlenW(m_szTextW);

	SetSel(extend ? m_nSelectionStart : e, e, after_wrap);
	ScrollCaret();
}

void duMLEdit::MoveForward(BOOL extend)
{
	INT e = m_nSelectionEnd;

	if (m_szTextW[e])
	{
		e++;
		if ((m_nStyle & ES_MULTILINE) && (m_szTextW[e - 1] == '\r')) 
		{
			if (m_szTextW[e] == '\n')
				e++;
			else if ((m_szTextW[e] == '\r') && (m_szTextW[e + 1] == '\n'))
				e += 2;
		}
	}

	SetSel(extend ? m_nSelectionStart : e, e, FALSE);
	ScrollCaret();
}

void duMLEdit::MoveHome(BOOL extend)
{
	INT e;

	/* Pass the x_offset in x to make sure of receiving the first position of the line */
	if (m_nStyle & ES_MULTILINE)
		e = CharFromPos(-m_nXOffset, HIWORD(PosFromChar(m_nSelectionEnd, m_nFlags & EF_AFTER_WRAP)), NULL);
	else
		e = 0;

	SetSel(extend ? m_nSelectionStart : e, e, FALSE);
	ScrollCaret();
}

void duMLEdit::MovePageDown_ML(BOOL extend)
{
	INT s = m_nSelectionStart;
	INT e = m_nSelectionEnd;
	BOOL after_wrap = (m_nFlags & EF_AFTER_WRAP);
	LRESULT pos = PosFromChar(e, after_wrap);
	INT x = (short)LOWORD(pos);
	INT y = (short)HIWORD(pos);

	e = CharFromPos(x, y + (m_rcFormat.bottom - m_rcFormat.top), &after_wrap);
	if (!extend)
		s = e;

	SetSel(s, e, after_wrap);
	ScrollCaret();
}

void duMLEdit::MovePageUp_ML(BOOL extend)
{
	INT s = m_nSelectionStart;
	INT e = m_nSelectionEnd;
	BOOL after_wrap = (m_nFlags & EF_AFTER_WRAP);
	LRESULT pos = PosFromChar(e, after_wrap);
	INT x = (short)LOWORD(pos);
	INT y = (short)HIWORD(pos);

	e = CharFromPos(x, y - (m_rcFormat.bottom - m_rcFormat.top), &after_wrap);
	if (!extend)
		s = e;

	SetSel(s, e, after_wrap);
	ScrollCaret();
}

void duMLEdit::MoveUp_ML(BOOL extend)
{
	INT s = m_nSelectionStart;
	INT e = m_nSelectionEnd;
	BOOL after_wrap = (m_nFlags & EF_AFTER_WRAP);
	LRESULT pos = PosFromChar(e, after_wrap);
	INT x = (short)LOWORD(pos);
	INT y = (short)HIWORD(pos);

	e = CharFromPos(x, y - m_nLineHeight, &after_wrap);
	if (!extend)
		s = e;

	SetSel(s, e, after_wrap);
	ScrollCaret();
}

void duMLEdit::MoveWordBackward(BOOL extend)
{
	INT s = m_nSelectionStart;
	INT e = m_nSelectionEnd;
	INT l;
	INT ll;
	INT li;

	l = LineFromChar(e);
	ll = LineLength(e);
	li = LineIndex(l);
	if (e - li == 0) 
	{
		if (l) 
		{
			li = LineIndex(l - 1);
			e = li + LineLength(li);
		}
	} 
	else 
	{
		e = li + (INT)CallWordBreakProc(li, e - li, ll, WB_LEFT);
	}

	if (!extend)
		s = e;

	SetSel(s, e, FALSE);
	ScrollCaret();
}

void duMLEdit::MoveWordForward(BOOL extend)
{
	INT s = m_nSelectionStart;
	INT e = m_nSelectionEnd;
	INT l;
	INT ll;
	INT li;

	l = LineFromChar(e);
	ll = LineLength(e);
	li = LineIndex(l);
	if (e - li == ll) 
	{
		if ((m_nStyle & ES_MULTILINE) && (l != m_nLineCount - 1))
			e = LineIndex(l + 1);
	} 
	else 
	{
		e = li + CallWordBreakProc(li, e - li + 1, ll, WB_RIGHT);
	}

	if (!extend)
		s = e;

	SetSel(s, e, FALSE);
	ScrollCaret();
}

void duMLEdit::SetCaretPos(INT pos, BOOL after_wrap)
{
	LRESULT res = PosFromChar(pos, after_wrap);
	
	duRect rcEdit;
	rcEdit.SetRectEmpty();
	GetRect(&rcEdit);

	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	duRect rcClient = _GetClientRect();
	POINT pt;
	pt.x = min(rcClient.Width(), (short)LOWORD(res));
	pt.y = min(rcClient.Height(), (short)HIWORD(res));

	pCtrlManager->_SetCaretPos(rcEdit.left + pt.x, rcEdit.top + pt.y);
}

void duMLEdit::AdjustFormatRect()
{
	duRect ClientRect;
	
	m_rcFormat.right = max(m_rcFormat.right, m_rcFormat.left + m_nCharWidth);
	if (m_nStyle & ES_MULTILINE)
	{
	    INT fw, vlc, max_x_offset, max_y_offset;

	    vlc = (m_rcFormat.bottom - m_rcFormat.top) / m_nLineHeight;
	    m_rcFormat.bottom = m_rcFormat.top + max(1, vlc) * m_nLineHeight;

	    /* correct x_offset */
	    fw = m_rcFormat.right - m_rcFormat.left;
	    max_x_offset = m_nTextWidth - fw;
	    if(max_x_offset < 0) 
			max_x_offset = 0;

	    if(m_nXOffset > max_x_offset)
			m_nXOffset = max_x_offset;

	    /* correct y_offset */
	    max_y_offset = m_nLineCount - vlc;
	    if(max_y_offset < 0) 
			max_y_offset = 0;

	    if(m_nYOffset > max_y_offset)
			m_nYOffset = max_y_offset;

	    /* force scroll info update */
	    UpdateScrollInfo();
	}

	/* Always stay within the client area */
	ClientRect = _GetClientRect();
	m_rcFormat.bottom = min(m_rcFormat.bottom, ClientRect.bottom);

	if ((m_nStyle & ES_MULTILINE) && !(m_nStyle & ES_AUTOHSCROLL))
		BuildLineDefs_ML(0, lstrlenW(m_szTextW), 0, NULL);
	
	SetCaretPos(m_nSelectionEnd, m_nFlags & EF_AFTER_WRAP);
}

void duMLEdit::SetRectNP(LPRECT rc)
{	
	CopyRect(&m_rcFormat, rc);
	
	//
	// 默认Format + 2个像素
	//
	m_rcFormat.top += 2;
	m_rcFormat.left += 2;
	m_rcFormat.right -= 15;
	m_rcFormat.bottom -= 2;
	AdjustFormatRect();
}

void duMLEdit::UnlockBuffer(BOOL force)
{
	if (!IsWindow(m_hWnd))
	    return;

	if (!m_nLockCount)
		return;

	if (!m_szTextW)
		return;

	if (force || (m_nLockCount == 1)) 
	{
	    if (m_hLoc32W) 
		{
			CHAR *textA = NULL;
			UINT countA = 0;
			UINT countW = lstrlenW(m_szTextW) + 1;

			if (m_hLoc32A)
			{
				UINT countA_new = WideCharToMultiByte(CP_ACP, 0, m_szTextW, countW, NULL, 0, NULL, NULL);
				countA = LocalSize(m_hLoc32A);
				if (countA_new > countA)
				{
					HLOCAL hloc32A_new;
					UINT alloc_size = ROUND_TO_GROW(countA_new);
					hloc32A_new = LocalReAlloc(m_hLoc32A, alloc_size, LMEM_MOVEABLE | LMEM_ZEROINIT);
					if (hloc32A_new)
					{
						m_hLoc32A = hloc32A_new;
						countA = LocalSize(hloc32A_new);
					}
				}
				textA = (CHAR *)LocalLock(m_hLoc32A);
			}

			if (textA)
			{
				WideCharToMultiByte(CP_ACP, 0, m_szTextW, countW, textA, countA, NULL, NULL);
				LocalUnlock(m_hLoc32A);
			}

			LocalUnlock(m_hLoc32W);
			m_szTextW = NULL;
	    }
	    else 
		{
			return;
	    }
	}

	m_nLockCount--;
}

void duMLEdit::UpdateScrollInfo()
{
    if (IsVertScrollBarShow() && !(m_nFlags & EF_VSCROLL_TRACK))
    {		
		duScroll *pVertScroll = (duScroll *)GetPluginByName(m_szVertScroll);
		if (pVertScroll)
		{
			pVertScroll->SetMaxPos(m_nLineCount);
			pVertScroll->SetPage((m_rcFormat.bottom - m_rcFormat.top) / m_nLineHeight);
			pVertScroll->SetPos(m_nYOffset);
		}
    }

    if (IsHorzScrollBarShow() && !(m_nFlags & EF_HSCROLL_TRACK))
    {
		duScroll *pHorzScroll = (duScroll *)GetPluginByName(m_szHorzScroll);
		if (pHorzScroll)
		{
			pHorzScroll->SetMaxPos(m_nTextWidth);
			pHorzScroll->SetPage(m_rcFormat.Width());
			pHorzScroll->SetPos(m_nXOffset);
		}
    }
}

LRESULT duMLEdit::CharFromPos(INT x, INT y)
{
	POINT pt;
	duRect rc;
	INT index;

	pt.x = x;
	pt.y = y;
	rc = _GetClientRect();
	if (!PtInRect(&rc, pt))
		return -1;

	index = CharFromPos(x, y, NULL);
	return MAKELONG(index, LineFromChar(index));
}

INT duMLEdit::GetLine(INT line, LPWSTR dst, BOOL unicode)
{
	LPWSTR src;
	INT line_len, dst_len;
	INT i;

	if (m_nStyle & ES_MULTILINE) 
	{
		if (line >= m_nLineCount)
			return 0;
	} 
	else
		line = 0;

	i = LineIndex(line);
	src = m_szTextW + i;
	line_len = LineLength(i);
	dst_len = *(WORD *)dst;
	if(unicode)
	{
	    if(dst_len <= line_len)
	    {
			memcpy(dst, src, dst_len * sizeof(WCHAR));
			return dst_len;
	    }
	    else /* Append 0 if enough space */
	    {
			memcpy(dst, src, line_len * sizeof(WCHAR));
			dst[line_len] = 0;
			return line_len;
	    }
	}
	else
	{
	    INT ret = WideCharToMultiByte(CP_ACP, 0, src, line_len, (LPSTR)dst, dst_len, NULL, NULL);

	    if(!ret && line_len) /* Insufficient buffer size */
			return dst_len;

	    if(ret < dst_len) /* Append 0 if enough space */
			((LPSTR)dst)[ret] = 0;

	    return ret;
	}
}

LRESULT duMLEdit::GetSel(PUINT start, PUINT end)
{
	UINT s = m_nSelectionStart;
	UINT e = m_nSelectionEnd;

	ORDER_UINT(s, e);
	if (start)
		*start = s;

	if (end)
		*end = e;

	return MAKELONG(s, e);
}

INT duMLEdit::LineFromChar(INT index)
{
	INT line;
	LINEDEF *line_def;

	if (!(m_nStyle & ES_MULTILINE))
		return 0;

	if (index > (INT)lstrlenW(m_szTextW))
		return m_nLineCount - 1;

	if (index == -1)
		index = min(m_nSelectionStart, m_nSelectionEnd);

	line = 0;
	line_def = m_pFirstLineDef;
	index -= line_def->length;
	while ((index >= 0) && line_def->next) 
	{
		line++;
		line_def = line_def->next;
		index -= line_def->length;
	}
	return line;
}

INT duMLEdit::LineIndex(INT line)
{
	INT line_index;
	LINEDEF *line_def;

	if (!(m_nStyle & ES_MULTILINE))
		return 0;

	if (line >= m_nLineCount)
		return -1;

	line_index = 0;
	line_def = m_pFirstLineDef;
	if (line == -1) 
	{
		INT index = m_nSelectionEnd - line_def->length;
		while ((index >= 0) && line_def->next) 
		{
			line_index += line_def->length;
			line_def = line_def->next;
			index -= line_def->length;
		}
	} 
	else 
	{
		while (line > 0) 
		{
			line_index += line_def->length;
			line_def = line_def->next;
			line--;
		}
	}

	return line_index;
}

INT duMLEdit::LineLength(INT index)
{
	LINEDEF *line_def;

	if (!(m_nStyle & ES_MULTILINE))
		return lstrlenW(m_szTextW);

	if (index == -1) 
	{
		/* get the number of remaining non-selected chars of selected lines */
		INT32 l; /* line number */
		INT32 li; /* index of first char in line */
		INT32 count;
		l = LineFromChar(m_nSelectionStart);
		/* # chars before start of selection area */
		count = m_nSelectionStart - LineIndex(l);
		l = LineFromChar(m_nSelectionEnd);
		/* # chars after end of selection */
		li = LineIndex(l);
		count += li + LineLength(li) - m_nSelectionEnd;
		return count;
	}

	line_def = m_pFirstLineDef;
	index -= line_def->length;
	while ((index >= 0) && line_def->next) 
	{
		line_def = line_def->next;
		index -= line_def->length;
	}
	return line_def->net_length;
}

BOOL duMLEdit::LineScroll(INT dx, INT dy)
{
	if (!(m_nStyle & ES_MULTILINE))
		return FALSE;

	dx *= m_nCharWidth;
	return LineScroll_internal(dx, dy);
}

BOOL duMLEdit::LineScroll_internal(INT dx, INT dy)
{
	INT nyoff;
	INT x_offset_in_pixels;
	INT lines_per_page = (m_rcFormat.bottom - m_rcFormat.top) / m_nLineHeight;

	if (m_nStyle & ES_MULTILINE)
	    x_offset_in_pixels = m_nXOffset;

	if (-dx > x_offset_in_pixels)
		dx = -x_offset_in_pixels;

	if (dx > m_nTextWidth - x_offset_in_pixels)
		dx = m_nTextWidth - x_offset_in_pixels;

	nyoff = max(0, m_nYOffset + dy);
	if (nyoff >= m_nLineCount - lines_per_page)
		nyoff = max(0, m_nLineCount - lines_per_page);

	dy = (m_nYOffset - nyoff) * m_nLineHeight;
	if (dx || dy) 
	{
		RECT rc1;
		RECT rc;

		m_nYOffset = nyoff;
		if(m_nStyle & ES_MULTILINE)
		    m_nXOffset += dx;

		rc1 = _GetClientRect();
		IntersectRect(&rc, &rc1, &m_rcFormat);
		UpdateScrollInfo();
	}

	//if (dx && !(m_nFlags & EF_HSCROLL_TRACK))
		//EDIT_NOTIFY_PARENT(EN_HSCROLL);

	//if (dy && !(m_nFlags & EF_VSCROLL_TRACK))
	//	EDIT_NOTIFY_PARENT(EN_VSCROLL);

	return TRUE;
}

LRESULT duMLEdit::PosFromChar(INT index, BOOL after_wrap)
{
	INT len = lstrlenW(m_szTextW);
	INT l;
	INT li;
	INT x;
	INT y = 0;
	INT w;
	INT lw = 0;
	INT ll = 0;
	HDC dc;
	HFONT old_font = 0;
	LINEDEF *line_def;

	index = min(index, len);
	dc = GetDC(m_hWnd);
	if (m_hFont)
		old_font = (HFONT)SelectObject(dc, m_hFont);

	if (m_nStyle & ES_MULTILINE) 
	{
		l = LineFromChar(index);
		y = (l - m_nYOffset) * m_nLineHeight;
		li = LineIndex(l);
		if (after_wrap && (li == index) && l) 
		{
			INT l2 = l - 1;
			line_def = m_pFirstLineDef;
			while (l2) 
			{
				line_def = line_def->next;
				l2--;
			}
			if (line_def->ending == END_WRAP) 
			{
				l--;
				y -= m_nLineHeight;
				li = LineIndex(l);
			}
		}

		line_def = m_pFirstLineDef;
		while (line_def->index != li)
			line_def = line_def->next;

		ll = line_def->net_length;
		lw = line_def->width;

		w = m_rcFormat.right - m_rcFormat.left;
		if (m_nStyle & ES_RIGHT)
		{
			x = LOWORD(_GetTabbedTextExtentW(dc, m_szTextW + li + (index - li), ll - (index - li),
				0, NULL)) - m_nXOffset;
			x = w - x;
		}
		else if (m_nStyle & ES_CENTER)
		{
			x = LOWORD(_GetTabbedTextExtentW(dc, m_szTextW + li, index - li,
				0, NULL)) - m_nXOffset;
			x += (w - lw) / 2;
		}
		else /* ES_LEFT */
		{
		    x = LOWORD(_GetTabbedTextExtentW(dc, m_szTextW + li, index - li,
				0, NULL)) - m_nXOffset;
		}
	} 

	x += m_rcFormat.left;
	y += m_rcFormat.top;

	if (m_hFont)
		SelectObject(dc, old_font);

	ReleaseDC(m_hWnd, dc);
	return MAKELONG((INT16)x, (INT16)y);
}

void duMLEdit::ReplaceSel(BOOL can_undo, LPCWSTR lpsz_replace, BOOL send_update, BOOL honor_limit)
{
	UINT strl = lstrlenW(lpsz_replace);
	UINT tl = lstrlenW(m_szTextW);
	UINT utl;
	UINT s;
	UINT e;
	UINT i;
	UINT size;
	LPWSTR p;
	HRGN hrgn = 0;
	LPWSTR buf = NULL;
	UINT bufl = 0;

	s = m_nSelectionStart;
	e = m_nSelectionEnd;

	if ((s == e) && !strl)
		return;

	ORDER_UINT(s, e);

	size = tl - (e - s) + strl;
	if (!size)
		m_nTextWidth = 0;

	/* Issue the EN_MAXTEXT notification and continue with replacing text
	 * such that buffer limit is honored. */
	if ((honor_limit) && (m_nBufferLimit > 0) && (size > m_nBufferLimit)) 
	{
		//EDIT_NOTIFY_PARENT(EN_MAXTEXT);
		strl = m_nBufferLimit - (tl - (e-s));
	}

	//
	// 动态调整文本内存块大小
	//
	if (!MakeFit(tl - (e - s) + strl))
		return;

	if (e != s) 
	{
		/* there is something to be deleted */
		bufl = e - s;
		buf = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, (bufl + 1) * sizeof(WCHAR));
		if (!buf) return;
		memcpy(buf, m_szTextW + s, bufl * sizeof(WCHAR));
		buf[bufl] = 0; /* ensure 0 termination */
		/* now delete */
		lstrcpyW(m_szTextW + s, m_szTextW + e);
	}

	if (strl) 
	{
		/* there is an insertion */
		tl = lstrlenW(m_szTextW);
		for (p = m_szTextW + tl ; p >= m_szTextW + s ; p--)
			p[strl] = p[0];
		for (i = 0 , p = m_szTextW + s ; i < strl ; i++)
			p[i] = lpsz_replace[i];
		if(m_nStyle & ES_UPPERCASE)
			CharUpperBuffW(p, strl);
		else if(m_nStyle & ES_LOWERCASE)
			CharLowerBuffW(p, strl);
	}

	if (m_nStyle & ES_MULTILINE)
	{
		INT st = min(m_nSelectionStart, m_nSelectionEnd);
		INT vlc = (m_rcFormat.bottom - m_rcFormat.top) / m_nLineHeight;

		hrgn = CreateRectRgn(0, 0, 0, 0);
		BuildLineDefs_ML(st, st + strl,
				strl - abs(m_nSelectionEnd - m_nSelectionStart), hrgn);
		/* if text is too long undo all changes */
		if (honor_limit && !(m_nStyle & ES_AUTOVSCROLL) && (m_nLineCount > vlc)) {
			if (strl)
				lstrcpyW(m_szTextW + e, m_szTextW + e + strl);
			if (e != s)
				for (i = 0 , p = m_szTextW ; i < e - s ; i++)
					p[i + s] = buf[i];
			BuildLineDefs_ML(s, e, 
				abs(m_nSelectionEnd - m_nSelectionStart) - strl, hrgn);
			strl = 0;
			e = s;
			hrgn = CreateRectRgn(0, 0, 0, 0);
			//EDIT_NOTIFY_PARENT(EN_MAXTEXT);
		}
	}
	
	if (e != s) 
	{
		if (can_undo) 
		{
			utl = lstrlenW(m_szUndoTextW);
			if (!m_nUndoInsertCount && (*m_szUndoTextW && (s == m_nUndoPosition))) 
			{
				/* undo-buffer is extended to the right */
				MakeUndoFit(utl + e - s);
				memcpy(m_szUndoTextW + utl, buf, (e - s)*sizeof(WCHAR));
				(m_szUndoTextW + utl)[e - s] = 0; /* ensure 0 termination */
			} 
			else if (!m_nUndoInsertCount && (*m_szUndoTextW && (e == m_nUndoPosition))) 
			{
				/* undo-buffer is extended to the left */
				MakeUndoFit(utl + e - s);
				for (p = m_szUndoTextW + utl ; p >= m_szUndoTextW ; p--)
					p[e - s] = p[0];
				for (i = 0 , p = m_szUndoTextW ; i < e - s ; i++)
					p[i] = buf[i];
				m_nUndoPosition = s;
			} 
			else 
			{
				/* new undo-buffer */
				MakeUndoFit(e - s);
				memcpy(m_szUndoTextW, buf, (e - s)*sizeof(WCHAR));
				m_szUndoTextW[e - s] = 0; /* ensure 0 termination */
				m_nUndoPosition = s;
			}
			/* any deletion makes the old insertion-undo invalid */
			m_nUndoInsertCount = 0;
		} 
		else
			EmptyUndoBuffer();
	}

	if (strl) 
	{
		if (can_undo) 
		{
			if ((s == m_nUndoPosition) || ((m_nUndoInsertCount) && (s == m_nUndoPosition + m_nUndoInsertCount)))
				/*
				 * insertion is new and at delete position or
				 * an extension to either left or right
				 */
				m_nUndoInsertCount += strl;
			else 
			{
				/* new insertion undo */
				m_nUndoPosition = s;
				m_nUndoInsertCount = strl;
				/* new insertion makes old delete-buffer invalid */
				*m_szUndoTextW = '\0';
			}
		} 
		else
			EmptyUndoBuffer();
	}

	if (bufl)
		HeapFree(GetProcessHeap(), 0, buf);
 
	s += strl;

	/* If text has been deleted and we're right or center aligned then scroll rightward */
	if (m_nStyle & (ES_RIGHT | ES_CENTER))
	{
		INT delta = strl - abs(m_nSelectionEnd - m_nSelectionStart);

		if (delta < 0 && m_nXOffset)
		{
			if (abs(delta) > m_nXOffset)
				m_nXOffset = 0;
			else
				m_nXOffset += delta;
		}
	}

	SetSel(s, s, FALSE);
	m_nFlags |= EF_MODIFIED;
	if (send_update) m_nFlags |= EF_UPDATE;
	if (hrgn)
	{
		//_RedrawEdit();
		DeleteObject(hrgn);
	}
	//else
		//_RedrawEdit();

	ScrollCaret();
	/* force scroll info update */
	UpdateScrollInfo();

	if (send_update || (m_nFlags & EF_UPDATE))
	{
	    m_nFlags &= ~EF_UPDATE;
	    //EDIT_NOTIFY_PARENT(EN_CHANGE);
	}
}

void duMLEdit::ScrollCaret()
{
	if (m_nStyle & ES_MULTILINE)
	{
		INT l;
		INT li;
		INT vlc;
		INT ww;
		INT cw = m_nCharWidth;
		INT x;
		INT dy = 0;
		INT dx = 0;

		l = LineFromChar(m_nSelectionEnd);
		li = LineIndex(l);
		x = (short)LOWORD(PosFromChar(m_nSelectionEnd, m_nFlags & EF_AFTER_WRAP));
		vlc = (m_rcFormat.bottom - m_rcFormat.top) / m_nLineHeight;

		if (l >= m_nYOffset + vlc)
			dy = l - vlc + 1 - m_nYOffset;

		if (l < m_nYOffset)
			dy = l - m_nYOffset;
		ww = m_rcFormat.right - m_rcFormat.left;
		if (x < m_rcFormat.left)
			dx = x - m_rcFormat.left - ww / HSCROLL_FRACTION / cw * cw;

		if (x > m_rcFormat.right)
			dx = x - m_rcFormat.left - (HSCROLL_FRACTION - 1) * ww / HSCROLL_FRACTION / cw * cw;

		if (dy || dx || (m_nYOffset && (m_nLineCount - m_nYOffset < vlc)))
		{
		    /* check if we are going to move too far */
		    if(m_nXOffset + dx + ww > m_nTextWidth)
				dx = m_nTextWidth - ww - m_nXOffset;

		    if(dx || dy || (m_nYOffset && (m_nLineCount - m_nYOffset < vlc)))
				LineScroll_internal(dx, dy);
		}
	} 
	else 
	{
		INT x;
		INT goal;
		INT format_width;

		x = (short)LOWORD(PosFromChar(m_nSelectionEnd, FALSE));
		format_width = m_rcFormat.right - m_rcFormat.left;

		if (x < m_rcFormat.left) 
		{
			goal = m_rcFormat.left + format_width / HSCROLL_FRACTION;
			do {
				m_nXOffset--;
				x = (short)LOWORD(PosFromChar(m_nSelectionEnd, FALSE));
			} while ((x < goal) && m_nXOffset);
			//_RedrawEdit();
		} 
		else if (x > m_rcFormat.right) 
		{
			INT x_last;
			INT len = lstrlenW(m_szTextW);
			goal = m_rcFormat.right - format_width / HSCROLL_FRACTION;
			do {
				m_nXOffset++;
				x = (short)LOWORD(PosFromChar(m_nSelectionEnd, FALSE));
				x_last = (short)LOWORD(PosFromChar(len, FALSE));
			} while ((x > goal) && (x_last > m_rcFormat.right));
			
			//_RedrawEdit();
		}
	}

    if(m_nFlags & EF_FOCUSED)
		SetCaretPos(m_nSelectionEnd, m_nFlags & EF_AFTER_WRAP);
}

void duMLEdit::SetLimitText(INT limit)
{
    if (limit == 0xFFFFFFFF)
        m_nBufferLimit = -1;
    else if (m_nStyle & ES_MULTILINE)
	{
		if (limit)
			m_nBufferLimit = min(limit, BUFLIMIT_MULTI);
		else
			m_nBufferLimit = BUFLIMIT_MULTI;
	} 
	else 
	{
		if (limit)
			m_nBufferLimit = min(limit, BUFLIMIT_SINGLE);
		else
			m_nBufferLimit = BUFLIMIT_SINGLE;
	}
}

void duMLEdit::SetSel(UINT start, UINT end, BOOL after_wrap)
{
	UINT old_start = m_nSelectionStart;
	UINT old_end = m_nSelectionEnd;
	UINT len = lstrlenW(m_szTextW);

	if (start == (UINT)-1) 
	{
		start = m_nSelectionEnd;
		end = m_nSelectionEnd;
	} 
	else 
	{
		start = min(start, len);
		end = min(end, len);
	}
	m_nSelectionStart = start;
	m_nSelectionEnd = end;
	if (after_wrap)
		m_nFlags |= EF_AFTER_WRAP;
	else
		m_nFlags &= ~EF_AFTER_WRAP;

	ORDER_UINT(end, old_end);
	ORDER_UINT(start, old_start);
	ORDER_UINT(old_start, old_end);
	ORDER_UINT(start, end);

	//_RedrawEdit();
}

void duMLEdit::SetWordBreakProc(void *wbp)
{
	if (m_pfWordbreakProc == wbp)
		return;

	m_pfWordbreakProc = wbp;
	if ((m_nStyle & ES_MULTILINE) && !(m_nStyle & ES_AUTOHSCROLL)) 
	{
		BuildLineDefs_ML(0, lstrlenW(m_szTextW), 0, NULL);
		//_RedrawEdit();
	}
}

BOOL duMLEdit::Undo()
{
	INT ulength;
	LPWSTR utext;

	if (m_nStyle & ES_READONLY)
		return !(m_nStyle & ES_MULTILINE);

	ulength = lstrlenW(m_szUndoTextW);
	utext = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, (ulength + 1) * sizeof(WCHAR));
	lstrcpyW(utext, m_szUndoTextW);

	SetSel(m_nUndoPosition, m_nUndoPosition + m_nUndoInsertCount, FALSE);
	EmptyUndoBuffer();
	ReplaceSel(TRUE, utext, TRUE, TRUE);
	SetSel(m_nUndoPosition, m_nUndoPosition + m_nUndoInsertCount, FALSE);

	/* send the notification after the selection start and end are set */
	//EDIT_NOTIFY_PARENT(EN_CHANGE);

	ScrollCaret();
	HeapFree(GetProcessHeap(), 0, utext);

	return TRUE;
}

void duMLEdit::OnChar(WCHAR c)
{
	BOOL control = GetKeyState(VK_CONTROL) & 0x8000;

	switch (c) 
	{
	case '\r':
		if(!(m_nStyle & ES_MULTILINE) && !(m_nStyle & ES_WANTRETURN))
			break;

	case '\n':
		if (m_nStyle & ES_MULTILINE) 
		{
			if (m_nStyle & ES_READONLY) 
			{
				MoveHome(FALSE);
				MoveDown_ML(FALSE);
			} 
			else 
			{
				static const WCHAR cr_lfW[] = {'\r','\n',0};
				ReplaceSel(TRUE, cr_lfW, TRUE, TRUE);
			}
		}
		break;

	case '\t':
		if ((m_nStyle & ES_MULTILINE) && !(m_nStyle & ES_READONLY))
		{
			static const WCHAR tabW[] = {'\t',0};
			ReplaceSel(TRUE, tabW, TRUE, TRUE);
		}
		break;

	case VK_BACK:
		if (!(m_nStyle & ES_READONLY) && !control)
		{
			if (m_nSelectionStart != m_nSelectionEnd)
				OnClear();
			else 
			{
				SetSel((UINT)-1, 0, FALSE);
				MoveBackward(TRUE);
				OnClear();
			}
		}
		break;

	case 0x03: /* ^C */
		OnCopy();
		break;

	case 0x16: /* ^V */
	    if (!(m_nStyle & ES_READONLY))
			OnPaste();
		break;

	case 0x18: /* ^X */
		if (!(m_nStyle & ES_READONLY))
		{
			OnCopy();
			OnClear();
		}
		break;

	case 0x01: /* ^A */
		{
			SetSel(0, -1, FALSE);
			MoveEnd(TRUE);
		}
		break;

	default:
		//
		// 可以增加功能,只能输入特定字符函数 SetCharSet
		//
		if((m_nStyle & ES_NUMBER) && !( c >= '0' && c <= '9'))
			break;

		if (!(m_nStyle & ES_READONLY) && (c >= ' ') && (c != 127))
		{
			WCHAR str[2];
 			str[0] = c;
 			str[1] = '\0';
 			ReplaceSel(TRUE, str, TRUE, TRUE);
 		}
		break;
	}
}

void duMLEdit::OnContextMenu(INT x, INT y)
{
	
}

void duMLEdit::OnCopy()
{
	INT s = min(m_nSelectionStart, m_nSelectionEnd);
	INT e = max(m_nSelectionStart, m_nSelectionEnd);
	HGLOBAL hdst;
	LPWSTR dst;
	DWORD len;

	if (e == s)
		return;

	len = e - s;
	hdst = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (len + 1) * sizeof(WCHAR));
	dst = (LPWSTR)GlobalLock(hdst);
	memcpy(dst, m_szTextW + s, len * sizeof(WCHAR));
	dst[len] = 0; /* ensure 0 termination */
	GlobalUnlock(hdst);
	OpenClipboard(m_hWnd);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hdst);
	CloseClipboard();
}

INT duMLEdit::OnGetText(INT count, LPWSTR dst, BOOL unicode)
{
    if(!count) 
		return 0;

    if(unicode)
    {
		lstrcpynW(dst, m_szTextW, count);
		return lstrlenW(dst);
    }
    else
    {
		LPSTR textA = (LPSTR)dst;
		if (!WideCharToMultiByte(CP_ACP, 0, m_szTextW, -1, textA, count, NULL, NULL))
            textA[count - 1] = 0; /* ensure 0 termination */
		return strlen(textA);
    }
}

void duMLEdit::OnPaste()
{
	HGLOBAL hsrc;
	LPWSTR src;

	if(m_nStyle & ES_READONLY)
	    return;

	OpenClipboard(m_hWnd);
	if ((hsrc = GetClipboardData(CF_UNICODETEXT)))
	{
		src = (LPWSTR)GlobalLock(hsrc);
		ReplaceSel(TRUE, src, TRUE, TRUE);
		GlobalUnlock(hsrc);
	}
	CloseClipboard();
}

void duMLEdit::OnSetFont(HFONT font)
{
	TEXTMETRICW tm;
	HDC dc;
	HFONT old_font = 0;
	duRect clientRect;

	m_hFont = font;
	dc = GetDC(m_hWnd);
	if (font)
		old_font = (HFONT)SelectObject(dc, font);
	GetTextMetricsW(dc, &tm);
	m_nLineHeight = tm.tmHeight;
	m_nCharWidth = tm.tmAveCharWidth;
	if (font)
		SelectObject(dc, old_font);
	ReleaseDC(m_hWnd, dc);
	
	clientRect = _GetClientRect();
	SetRectNP(&clientRect);
	
	if (m_nStyle & ES_MULTILINE)
		BuildLineDefs_ML(0, lstrlenW(m_szTextW), 0, NULL);

	if (m_nFlags & EF_FOCUSED) 
	{
		DestroyCaret();
		CreateCaret(m_hWnd, 0, 2, m_nLineHeight);
		SetCaretPos(m_nSelectionEnd, m_nFlags & EF_AFTER_WRAP);
		ShowCaret(m_hWnd);
	}
}

void duMLEdit::OnSetText(LPCWSTR text, BOOL unicode)
{
    if (!unicode && text)
    {
		LPCSTR textA = (LPCSTR)text;
		INT countW = MultiByteToWideChar(CP_ACP, 0, textA, -1, NULL, 0);
        LPWSTR textW = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, countW * sizeof(WCHAR));
		if (textW)
			MultiByteToWideChar(CP_ACP, 0, textA, -1, textW, countW);
		text = textW;
    }

    SetSel(0, (UINT)-1, FALSE);
    if (text)
	{
		ReplaceSel(FALSE, text, FALSE, FALSE);
		if(!unicode)
			HeapFree(GetProcessHeap(), 0, (LPWSTR)text);
    } 
    else 
    {
		static const WCHAR empty_stringW[] = {0};
		ReplaceSel(FALSE, empty_stringW, FALSE, FALSE);
    }
    m_nXOffset = 0;
    m_nFlags &= ~EF_MODIFIED;
    SetSel(0, 0, FALSE);

  //  if( !((m_nStyle & ES_MULTILINE)))
  //  {
       // EDIT_NOTIFY_PARENT(EN_UPDATE);
      //  EDIT_NOTIFY_PARENT(EN_CHANGE);
 //   }
    ScrollCaret();
    UpdateScrollInfo();    
}

//void duMLEdit::OnSize(UINT action, INT width, INT height)
//{
//	if ((action == SIZE_MAXIMIZED) || (action == SIZE_RESTORED))
//	{
//		RECT rc;
//		::SetRect(&rc, 0, 0, width, height);
//		SetRectNP(&rc);
//		UpdateText(NULL, TRUE);
//	}
//}

LRESULT duMLEdit::OnStyleChanged (WPARAM which, const STYLESTRUCT *style)
{
	if (GWL_STYLE == which)
	{
		DWORD style_change_mask;
		DWORD new_style;

		style_change_mask = ES_UPPERCASE | ES_LOWERCASE | ES_NUMBER;
		if (m_nStyle & ES_MULTILINE)
			style_change_mask |= ES_WANTRETURN;

		new_style = style->styleNew & style_change_mask;

		if (new_style & ES_NUMBER) 
		{
		}
		else if (new_style & ES_LOWERCASE)
		{
			new_style &= ~ES_UPPERCASE;
		}

		m_nStyle = (m_nStyle & ~style_change_mask) | new_style;
	}

	return 0;
}

LRESULT duMLEdit::OnSysKeyDown(INT key, DWORD key_data)
{
	if ((key == VK_BACK) && (key_data & 0x2000)) {
		if (CanUndo())
			Undo();
		return 0;
	}
	return DefWindowProcW(m_hWnd, WM_SYSKEYDOWN, (WPARAM)key, (LPARAM)key_data);
}

DWORD duMLEdit::_TextOutW(HDC hDC, HFONT hFont, int nXStart, int nYStart, LPCWSTR lpString, int cbString, BOOL fSelect)
{	
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HFONT hOldFont = (HFONT)::SelectObject(hMemDC, hFont);

	duSize size;
	DWORD dwSize = _GetTabbedTextExtentW(hMemDC, lpString, cbString, 0, 0);
	size.cx = (INT)(SHORT)LOWORD(dwSize);
	size.cy = (INT)(SHORT)HIWORD(dwSize);

	WCHAR *lpTemp = new WCHAR[cbString * 5];
	memset(lpTemp, 0, sizeof(WCHAR) * cbString * 5);
	wcsncpy(lpTemp, lpString, cbString);

	wstring strTmp = lpTemp;
	wstring strNew = Replace(strTmp, _T("\t"), _T("    "));

	BITMAPINFO bmInfo;
	RGBQUAD *pBits = NULL;
	InitBitmapInfo(&bmInfo, size.cx, size.cy);

	HBITMAP hBmp = ::CreateDIBSection(hDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBmp);

	int i;
	int nLoop = size.cx * size.cy;
	if (fSelect)
	{
		::SetBkMode(hMemDC, OPAQUE);
		::SetBkColor(hMemDC, m_clrSelect);
		::SetTextColor(hMemDC, m_clrSelectText);

		TextOutW(hMemDC, 0, 0, strNew.c_str(), strNew.length());

		for (i = 0;i < nLoop; i++)
		{
			pBits->rgbReserved = 255;	
			pBits++;
		}
	}
	else
	{
		memset(pBits, 0xFF, nLoop * 4);

		::SetBkMode(hMemDC, TRANSPARENT);
		::SetTextColor(hMemDC, m_clrText);
		TextOutW(hMemDC, 0, 0, strNew.c_str(), strNew.length());

		for (i = 0;i < nLoop; i++)
		{
			if (*(int *)pBits == 0xFFFFFFFF)
				*(int *)pBits = 0;
			else
				pBits->rgbReserved = 255;

			pBits++;
		}
	}

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	::AlphaBlend(hDC, nXStart, nYStart, size.cx, size.cy, hMemDC, 0, 0, size.cx, size.cy, bf);

	::SelectObject(hMemDC, hOldBitmap);
	::SelectObject(hMemDC, hOldFont);
	::DeleteObject(hBmp);
	::DeleteDC(hMemDC);

	delete [] lpTemp;

	return size.cx;
}

DWORD duMLEdit::_TabbedTextOutW(HDC hDC, HFONT hFont, int nXStart, int nYStart, LPCWSTR lpString, int cbString, 
							  int nTabPositions, CONST LPINT lpnTabStopPositions, int nTabOrigin, BOOL fSelect)
{
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HFONT hOldFont = (HFONT)::SelectObject(hMemDC, hFont);

	duSize size;
	DWORD dwTextSize = GetTabbedTextExtentW(hMemDC, lpString, cbString, nTabPositions, lpnTabStopPositions);
	size.cx = (INT)(SHORT)LOWORD(dwTextSize);
	size.cy = (INT)(SHORT)HIWORD(dwTextSize);

	BITMAPINFO bmInfo;
	RGBQUAD *pBits = NULL;
	InitBitmapInfo(&bmInfo, size.cx, size.cy);

	HBITMAP hBmp = ::CreateDIBSection(hDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBmp);

	int i;
	int nLoop = size.cx * size.cy;
	if (fSelect)
	{
		::SetBkMode(hMemDC, OPAQUE);
		::SetBkColor(hMemDC, m_clrSelect);
		::SetTextColor(hMemDC, m_clrSelectText);

		TabbedTextOutW(hMemDC, 0, 0, lpString, cbString, nTabPositions, lpnTabStopPositions, nTabOrigin);

		for (i = 0;i < nLoop; i++)
		{
			pBits->rgbReserved = 255;	
			pBits++;
		}
	}
	else
	{
		memset(pBits, 0xFF, nLoop * 4);

		::SetBkMode(hMemDC, TRANSPARENT);
		::SetTextColor(hMemDC, m_clrText);
		TabbedTextOutW(hMemDC, 0, 0, lpString, cbString, nTabPositions, lpnTabStopPositions, nTabOrigin);

		for (i = 0;i < nLoop; i++)
		{
			if (*(int *)pBits == 0xFFFFFFFF)
				*(int *)pBits = 0;
			else
				pBits->rgbReserved = 255;

			pBits++;
		}
	}

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	::AlphaBlend(hDC, nXStart, nYStart, size.cx, size.cy, hMemDC, 0, 0, size.cx, size.cy, bf);

	::SelectObject(hMemDC, hOldBitmap);
	::SelectObject(hMemDC, hOldFont);
	::DeleteObject(hBmp);
	::DeleteDC(hMemDC);

	return size.cx;
}

DWORD duMLEdit::_GetTabbedTextExtentW(HDC hDC, LPCWSTR lpString, int nCount, int nTabPositions, CONST LPINT lpnTabStopPositions)
{
	if (hDC == NULL || lpString == NULL || nCount <= 0)
		return 0;

	WCHAR *lpTemp = new WCHAR[nCount * 5];
	memset(lpTemp, 0, sizeof(WCHAR) * nCount * 5);
	wcsncpy(lpTemp, lpString, nCount);

	wstring strTmp = lpTemp;
	wstring strNew = Replace(strTmp, _T("\t"), _T("    "));

	duSize size;
	size.cx = size.cy = 0;
	GetTextExtentPoint32W(hDC, strNew.c_str(), strNew.length(), &size);

	delete [] lpTemp;

	//_TRACE(_T("strOld : %s, strNew : %s size{%d,%d}\n"), strTmp.c_str(), strNew.c_str(), size.cx, size.cy);

	return MAKELONG(size.cx, size.cy);
}

BOOL duMLEdit::IsHorzScrollBarShow()
{
	duScroll *pHorzSB = (duScroll *)GetPluginByName(m_szHorzScroll);
	if (Plugin_IsValid(pHorzSB))
		return pHorzSB->IsVisible();
	
	return FALSE;
}

BOOL duMLEdit::IsVertScrollBarShow()
{
	duScroll *pVertSB = (duScroll *)GetPluginByName(m_szVertScroll);
	if (Plugin_IsValid(pVertSB))
		return pVertSB->IsVisible();

	return FALSE;
}

duRect duMLEdit::_GetClientRect()
{
	duScroll *pVScroll = (duScroll *)GetPluginByName(m_szVertScroll);
	duScroll *pHScroll = (duScroll *)GetPluginByName(m_szHorzScroll);

	if (pVScroll == NULL || pHScroll == NULL)
		return duRect(0,0,0,0);

	duRect rcScrollView;
	GetRect(&rcScrollView);
	rcScrollView.OffsetRect(-rcScrollView.left, -rcScrollView.top);

	duRect rcHScroll;
	pHScroll->GetRect(&rcHScroll);
	duRect rcVScroll;
	pVScroll->GetRect(&rcVScroll);

	if (pHScroll->IsVisible())
		rcScrollView.bottom = max(0, rcScrollView.bottom - rcHScroll.Height());

	if (pVScroll->IsVisible())
		rcScrollView.right = max(0, rcScrollView.right - rcVScroll.Width());

	return rcScrollView;
}

void duMLEdit::_RedrawEdit()
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	if (m_fShowCaret)
		pCtrlManager->_HideCaret();

	Plugin_Redraw(this, TRUE);

	if (m_fShowCaret)
		pCtrlManager->_ShowCaret();
}

void WINAPI duMLEdit::SetText(LPCTSTR lpszText)
{
    SetSel(0, (UINT)-1, FALSE);
    if (lpszText)
		ReplaceSel(FALSE, lpszText, FALSE, FALSE);
    else 
    {
		static const WCHAR empty_stringW[] = {0};
		ReplaceSel(FALSE, empty_stringW, FALSE, FALSE);
    }

	UpdateScrollInfo();

	//m_nYOffset = m_nXOffset = 0;
	Plugin_Redraw(this, TRUE);
}
