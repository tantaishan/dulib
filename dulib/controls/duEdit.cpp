//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duEdit.cpp
//
//  History:    Nov-23-2009   Steven Li  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duEdit.h"
#include "duCtrlManager.h"

duEdit::duEdit() :
	 m_fUnicode(FALSE)
	,m_lpszText(NULL)
    ,m_uTextLength(0)
	,m_uBufferSize(0)
	,m_uBufferLimit(0)
	,m_hFont(NULL)
	,m_nXoffset(0)
	,m_nLineHeight(0)
	,m_nCharWidth(0)
	,m_dwStyle(0)
	,m_wflags(0)
	,m_nUndoInsertCount(0)
	,m_uUndoPosition(0)
	,m_lpszUndoText(NULL)
	,m_uUndoBufferSize(0)
	,m_nSelectionStart(0)
	,m_nSelectionEnd(0)
	,m_chPassword('*')
	,m_nLeftMargin(0)
	,m_nRightMargin(0)
	,m_nTextWidth(0)
	,m_nRegionPosX(0)
	,m_nRegionPosY(0)
	,m_pWordBreakProc(NULL)
	,m_nLineCount(0)
	,m_nYOffset(0)
	,m_fCaptureState(FALSE)
	,m_fEnableState(FALSE)
	,m_nLockCount(0)
	,m_nTabsCount(0)
	,m_pTabs(NULL)
	,m_pFirstLineDef(NULL)
	,m_hLoc32W(NULL)
	,m_hLoc32A(NULL)
	,m_nCompositionLen(0)
	,m_nCompositionStart(0)
	,m_fShowCaret(FALSE)
	,m_clrText(RGB(0,0,0))
	,m_lTimerId(0)
	,m_nLeftBorder(0)
	,m_nTopBorder(0)
	,m_nRightBorder(0)
	,m_nBottomBorder(0)
	,m_fMouseIn(FALSE)
	,m_fReadOnly(FALSE)
	,m_fPassword(FALSE)
	,m_fNumber(FALSE)
	,m_fRedraw(TRUE)
{
	m_clrSelect = RGB(51, 153, 255);//GetSysColor(COLOR_HIGHLIGHT);
	m_clrSelectText = RGB(255, 255, 255);

	::SetRectEmpty(&m_rcFormat);
	ZeroMemory(m_szFont, sizeof(TCHAR) * MAX_NAME);

#ifdef _UNICODE
	Init(TRUE);
#else
	Init(FALSE);
#endif //#ifdef _UNICODE
}

duEdit::~duEdit()
{
	Destroy();
	Plugin_UnHookWindowMessage(this);
}

void WINAPI duEdit::RegisterControlProperty()
{
	RegisterProperty(_T("leftborder"),   DU_PROPERTY_LONG, &m_nLeftBorder);
	RegisterProperty(_T("topborder"),    DU_PROPERTY_LONG, &m_nTopBorder);
	RegisterProperty(_T("rightborder"),  DU_PROPERTY_LONG, &m_nRightBorder);
	RegisterProperty(_T("bottomborder"), DU_PROPERTY_LONG, &m_nBottomBorder);

	RegisterProperty(_T("font"), DU_PROPERTY_FONT, m_szFont);
	RegisterProperty(_T("textcolor"), DU_PROPERTY_COLOR, &m_clrText);
	RegisterProperty(_T("selectcolor"), DU_PROPERTY_COLOR, &m_clrSelect);
	RegisterProperty(_T("selecttextcolor"), DU_PROPERTY_COLOR, &m_clrSelectText);

	RegisterProperty(_T("readonly"), DU_PROPERTY_BOOL, &m_fReadOnly);
	RegisterProperty(_T("password"), DU_PROPERTY_BOOL, &m_fPassword);
	RegisterProperty(_T("number"), DU_PROPERTY_BOOL, &m_fNumber);
}

void WINAPI duEdit::OnCreate()
{
	if (m_fReadOnly)
		m_dwStyle |= ES_READONLY;

	if (m_fPassword)
		m_dwStyle |= ES_PASSWORD;

	if (m_fNumber)
		m_dwStyle |= ES_NUMBER;

	SetCached(TRUE);

	m_lTimerId = GetUniTimer();
	
	Plugin_HookWindowMessage(this);

	duFont *pFont = (duFont *)GetResObj(m_szFont, DU_RES_FONT);
	if (pFont)
		m_hFont = pFont->GetHFont();
	else
		m_hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
    
    OnSetFont(m_hFont, FALSE);
    OnEMEmptyUndoBuffer();
    
	duRect rcEdit;
	SetRectEmpty(&rcEdit);
    GetRect(&rcEdit);
	rcEdit.OffsetRect(-rcEdit.left, -rcEdit.top);
    SetRectNP(&rcEdit);
    
    if (GetTextLength() > 0)
	{
		OnEmReplaceSel(FALSE, duPlugin::GetText(), FALSE, FALSE);
		m_nSelectionStart = m_nSelectionEnd = 0;
		EMScrollCaret();
    }
}

void WINAPI duEdit::DrawObject(HDC hDC)
{
	duRect rcEdit;
	rcEdit.SetRectEmpty();
	GetRect(&rcEdit);
	rcEdit.OffsetRect(-rcEdit.left, -rcEdit.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rcEdit, GetState(), NULL, GetAlpha());

	GetRect(&rcEdit);
	HRGN hRgnClip = ::CreateRectRgn(rcEdit.left + m_rcFormat.left, rcEdit.top + m_rcFormat.top, rcEdit.left + m_rcFormat.right, rcEdit.top + m_rcFormat.bottom);
	::SelectClipRgn(hDC, hRgnClip);

	int s = m_nSelectionStart;
	int e = m_nSelectionEnd;
	int ll;
	int x;
	int y;
	LRESULT pos;

	pos = EMPosFromChar(0);
	x = (short)LOWORD(pos);
	y = (short)HIWORD(pos);
	ll = GetEditTextLength();
	s = min(m_nSelectionStart, m_nSelectionEnd);
	e = max(m_nSelectionStart, m_nSelectionEnd);
	s = min(ll, s);
	e = min(ll, e);
	if ((s != e) && IsFocus())
	{
		x += PaintText(hDC, x, y, 0, 0, s, FALSE);
		x += PaintText(hDC, x, y, 0, s, e - s, TRUE);
		x += PaintText(hDC, x, y, 0, e, ll - e, FALSE);
	}
	else
	{
		x += PaintText(hDC, x, y, 0, 0, ll, FALSE);
	}

	::SelectClipRgn(hDC, NULL);
	::DeleteObject(hRgnClip);
}

INT duEdit::PaintText(HDC hDC, INT x, INT y, INT line, INT col, INT count, BOOL rev)
{
	if (count == 0)
		return 0;
	
	LPWSTR text = GetPasswordPointerSL();
	
	if (rev && m_nCompositionLen == 0)
		_TextOutW(hDC, m_hFont, x, y, text + col, count, TRUE);
	else
		_TextOutW(hDC, m_hFont, x, y, text + col, count, FALSE);

	duSize size;
	HFONT hOldFont = (HFONT)::SelectObject(hDC, m_hFont);
	GetTextExtentPoint32W(hDC, text + col, count, &size);
	::SelectObject(hDC, hOldFont);
	
	if (m_dwStyle & ES_PASSWORD)
		HeapFree(GetProcessHeap(), 0, text);

	return size.cx;
}


LRESULT WINAPI duEdit::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MOUSEMOVE)
	{
		if (!Plugin_IsValid(this) || !Plugin_IsVisible(this))
			return 0;

		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
	
		duRect rcEdit;
		Plugin_GetRect(this, &rcEdit);
		if (rcEdit.PtInRect(pt))
		{
			if (!m_fMouseIn)
			{
				m_fMouseIn = TRUE;
				Plugin_SetState(this, DU_STATE_OVER);
				Plugin_Redraw(this, TRUE);
				return 0;
			}
		}
		else
		{
			if (m_fMouseIn)
			{
				m_fMouseIn = FALSE;
				Plugin_SetState(this, DU_STATE_NORMAL);
				Plugin_Redraw(this, TRUE);
			}
		}
	}

	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return 0;
	
	if (pCtrlManager->GetFocusPlugin() != this)
		return 0;

#ifdef _UNICODE
	return EditWndProc_common(hWnd, uMsg, wParam, lParam, TRUE);
#else
	return EditWndProc_common(hWnd, uMsg, wParam, lParam, FALSE);
#endif //#ifdef _UNICODE
}

BOOL WINAPI duEdit::OnSetCursor()
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

void WINAPI duEdit::OnMouseMove(POINT pt)
{
	duRect rcEdit;
	rcEdit.SetRectEmpty();
	GetRect(&rcEdit);
	OnMouseMove(pt.x - rcEdit.left, pt.y - rcEdit.top);
}

void WINAPI duEdit::OnMouseDbClick(POINT pt)
{
	INT s;
	INT e = m_nSelectionEnd;
	INT l;
	INT li;
	INT ll;

	m_fCaptureState = TRUE;
	Plugin_SetCapture(this);

	l = 0;
	li = 0;
	ll = GetEditTextLength();
	s = li + CallWordBreakProc(li, e - li, ll, WB_LEFT);
	e = li + CallWordBreakProc(li, e - li, ll, WB_RIGHT);
	EMSetSel(s, e);
	EMScrollCaret();
	m_nRegionPosX = m_nRegionPosY = 0;
	Plugin_SetTimer(this, m_lTimerId, 100);
}

void WINAPI duEdit::OnMouseLDown(POINT pt)
{
	duRect rcEdit;
	rcEdit.SetRectEmpty();
	GetRect(&rcEdit);
	OnLButtonDown(pt.x - rcEdit.left, pt.y - rcEdit.top);
}

void WINAPI duEdit::OnMouseLUp(POINT pt)
{
	if (m_fCaptureState)
	{
		Plugin_KillTimer(this, m_lTimerId);
		if (Plugin_GetCapture(this) == this)
			Plugin_ReleaseCapture(this);
	}
	m_fCaptureState = FALSE;
}

void WINAPI duEdit::OnMouseRDown(POINT pt)
{
	if (!(m_wflags & EF_FOCUSED))
		::SetFocus(m_hWnd);
	else
	{
		// 当此plugin是focus状态，但是窗口失去焦点再切换回来时，focus不变，不会重新创建显示Caret
		duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
		if (pCtrlManager)
		{
			pCtrlManager->_DestroyCaret();
			pCtrlManager->_CreateCaret(1, m_nLineHeight);
			SetEditCaretPos(m_nSelectionEnd);
			pCtrlManager->_ShowCaret();
		}
	}
}

void WINAPI duEdit::OnMouseRUp(POINT pt)
{
}

void WINAPI duEdit::OnTimer(UINT nEventId)
{
	if (nEventId != m_lTimerId)
		return;
	
	if (m_nRegionPosX < 0)
		MoveBackward(TRUE);
	else if (m_nRegionPosX > 0)
		MoveForward(TRUE);
}

BOOL duEdit::OnEMCanUndo()
{
	return (m_nUndoInsertCount || lstrlenW(m_lpszUndoText));
}

void duEdit::OnEMEmptyUndoBuffer()
{
	m_nUndoInsertCount = 0;
	*m_lpszUndoText = '\0';
}

INT WordBreakProc(LPWSTR s, INT index, INT count, INT action)
{
	INT ret = 0;

	if(!s)
		return 0;

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
			while ((index < count) && (s[index] == ' ')) index++;
		else
		{
			while (s[index] && (s[index] != ' ') && (index < count))
				index++;
			while ((s[index] == ' ') && (index < count)) index++;
		}
		ret = index;
	}
	break;
	
	case WB_ISDELIMITER:
		ret = (s[index] == ' ');
		break;
	default:
		break;
	}
	return ret;
}

INT duEdit::CallWordBreakProc(INT start, INT index, INT count, INT action)
{
	INT ret;

	if (m_pWordBreakProc)
	{
		if(m_fUnicode)
	    {
			EDITWORDBREAKPROCW wbpW = (EDITWORDBREAKPROCW)m_pWordBreakProc;
			ret = wbpW(m_lpszText + start, index, count, action);
	    }
	    else
	    {
			EDITWORDBREAKPROCA wbpA = (EDITWORDBREAKPROCA)m_pWordBreakProc;
			INT countA;
			CHAR *textA;

			countA = WideCharToMultiByte(CP_ACP, 0, m_lpszText + start, count, NULL, 0, NULL, NULL);
			textA = (CHAR *)HeapAlloc(GetProcessHeap(), 0, countA);
			if (textA == NULL)
				return 0;
			WideCharToMultiByte(CP_ACP, 0, m_lpszText + start, count, textA, countA, NULL, NULL);
			ret = wbpA(textA, index, countA, action);
			HeapFree(GetProcessHeap(), 0, textA);
		}
	}
	else
		ret = WordBreakProc(m_lpszText + start, index, count, action);

	return ret;
}

UINT duEdit::GetEditTextLength()
{
    if(m_uTextLength == (UINT)-1)
        m_uTextLength = lstrlenW(m_lpszText);
    return m_uTextLength;
}

LPWSTR duEdit::GetPasswordPointerSL()
{
	if (m_dwStyle & ES_PASSWORD)
	{
		INT len = GetEditTextLength();
		LPWSTR text = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, (len + 1) * sizeof(WCHAR));
        if (text == NULL)
            return NULL;
		text[len] = '\0';
		while(len)
			text[--len] = m_chPassword;
		return text;
	}
	else
		return m_lpszText;
		
	return NULL;
}

void duEdit::CalcLineWidthSL()
{
	SIZE size;
	LPWSTR text;
	HDC hDC;
	HFONT hOlfFont = NULL;
	text = GetPasswordPointerSL();
	hDC = GetDC(m_hWnd);
	if (m_hFont)
		hOlfFont = (HFONT)SelectObject(hDC, m_hFont);

	GetTextExtentPoint32W(hDC, text, lstrlenW(text), &size);

	if (m_hFont)
		SelectObject(hDC, hOlfFont);
	ReleaseDC(m_hWnd, hDC);

	if (m_dwStyle & ES_PASSWORD)
		HeapFree(GetProcessHeap(), 0, text);

	m_nTextWidth = size.cx;
}

int WINAPI duEdit::CharFromPos(int x, int y)
{
	INT index;
	HDC hDC;
	HFONT hOldFont = 0;
	INT x_high = 0, x_low = 0;
	LPWSTR text;
	SIZE size;
	x -= m_rcFormat.left;
	if (!x)
		return m_nXoffset;

	if (!m_nXoffset)
	{
		INT indent = (m_rcFormat.right - m_rcFormat.left) - m_nTextWidth;
		if (m_dwStyle & ES_RIGHT)
			x -= indent;
		else if (m_dwStyle & ES_CENTER)
			x -= indent / 2;
	}

	text = GetPasswordPointerSL();
	hDC = GetDC(m_hWnd);
	if (m_hFont)
		hOldFont = (HFONT)SelectObject(hDC, m_hFont);
	if (x < 0)
	{
		INT low = 0;
		INT high = m_nXoffset;
		while (low < high - 1)
		{
			INT mid = (low + high) / 2;
			GetTextExtentPoint32W( hDC, text + mid, m_nXoffset - mid, &size );
			if (size.cx > -x)
			{
				low = mid;
				x_low = size.cx;
			}
			else
			{
				high = mid;
				x_high = size.cx;
			}
		}
		if (abs(x_high + x) <= abs(x_low + x) + 1)
			index = high;
		else
			index = low;
	}
	else
	{
		INT low = m_nXoffset;
		INT high = GetEditTextLength() + 1;
		while (low < high - 1)
		{
			INT mid = (low + high) / 2;
			GetTextExtentPoint32W( hDC, text + m_nXoffset, mid - m_nXoffset, &size );
			if (size.cx > x)
			{
				high = mid;
				x_high = size.cx;
			}
			else
			{
				low = mid;
				x_low = size.cx;
			}
		}
		if (abs(x_high - x) <= abs(x_low - x) + 1)
			index = high;
		else
		index = low;
	}
	
	if (m_dwStyle & ES_PASSWORD)
		HeapFree(GetProcessHeap(), 0, text);

	if (m_hFont)
		SelectObject(hDC, hOldFont);
	ReleaseDC(m_hWnd, hDC);
	return index;
}

void duEdit::ConfinePoint(LPINT x, LPINT y)
{
	*x = min(max(*x, m_rcFormat.left), m_rcFormat.right - 1);
	*y = min(max(*y, m_rcFormat.top), m_rcFormat.bottom - 1);
}

LRESULT duEdit::EMPosFromChar(INT index)
{
	INT len = GetEditTextLength();
	INT x;
	INT y = 0;
	INT lw = 0;
	INT ll = 0;
	HDC hDC;

	index = min(index, len);
	hDC = GetDC(m_hWnd);

	HFONT hOldFont = (HFONT)::SelectObject(hDC, m_hFont);
	duSize size;
	
	LPWSTR text = GetPasswordPointerSL();
	if (index < m_nXoffset)
	{
		GetTextExtentPoint32W(hDC, text + index, m_nXoffset - index, &size);
		x = -size.cx;
	}
	else
	{
		GetTextExtentPoint32W(hDC, text + m_nXoffset, index - m_nXoffset, &size);
		x = size.cx;
	}
	y = 0;
	if (m_dwStyle & ES_PASSWORD)
		HeapFree(GetProcessHeap(), 0, text);
	
	x += m_rcFormat.left;
	y += m_rcFormat.top;

	::SelectObject(hDC, hOldFont);
	ReleaseDC(m_hWnd, hDC);
	return MAKELONG((INT16)x, (INT16)y);
}

void duEdit::GetLineRect(INT line, INT scol, INT ecol, LPRECT rc)
{
	INT line_index =  0;
	rc->top = m_rcFormat.top;
	rc->bottom = rc->top + m_nLineHeight;
	rc->left = (scol == 0) ? m_rcFormat.left : (short)LOWORD(EMPosFromChar(line_index + scol));
	rc->right = (ecol == -1) ? m_rcFormat.right : (short)LOWORD(EMPosFromChar(line_index + ecol));
}

void duEdit::TextBufferChanged()
{
    m_uTextLength = (UINT)-1;
}

void duEdit::LockBuffer()
{
	if (!m_lpszText)
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
			return;

	    if(textA)
	    {
			HLOCAL hloc32W_new;
			UINT countW_new = MultiByteToWideChar(CP_ACP, 0, textA, -1, NULL, 0);
			if(countW_new > m_uBufferSize + 1)
			{
				UINT alloc_size = ROUND_TO_GROW(countW_new * sizeof(WCHAR));
				hloc32W_new = LocalReAlloc(m_hLoc32W, alloc_size, LMEM_MOVEABLE | LMEM_ZEROINIT);
				if(hloc32W_new)
				{
					m_hLoc32W = hloc32W_new;
					m_uBufferSize = LocalSize(hloc32W_new)/sizeof(WCHAR) - 1;
				}
			}
			m_lpszText = (LPWSTR)LocalLock(m_hLoc32W);
			MultiByteToWideChar(CP_ACP, 0, textA, -1, m_lpszText, m_uBufferSize + 1);
			LocalUnlock(m_hLoc32A);
		}
		else
			m_lpszText = (LPWSTR)LocalLock(m_hLoc32W);
	}
	
    if(m_wflags & EF_APP_HAS_HANDLE)
		TextBufferChanged();
	m_nLockCount++;
}

void duEdit::UnlockBuffer(BOOL force)
{
	if(!IsWindow(m_hWnd))
	    return;

	if (!m_nLockCount)
		return;
	
	if (!m_lpszText)
		return;
	
	if (force || (m_nLockCount == 1))
	{
	    if (m_hLoc32W)
		{
			UINT countA = 0;
			UINT countW = GetEditTextLength() + 1;

			if(m_hLoc32A)
			{
				UINT countA_new = WideCharToMultiByte(CP_ACP, 0, m_lpszText, countW, NULL, 0, NULL, NULL);
				countA = LocalSize(m_hLoc32A);
				if(countA_new > countA)
				{
					HLOCAL hloc32A_new;
					UINT alloc_size = ROUND_TO_GROW(countA_new);
					hloc32A_new = LocalReAlloc(m_hLoc32A, alloc_size, LMEM_MOVEABLE | LMEM_ZEROINIT);
					if(hloc32A_new)
					{
						m_hLoc32A = hloc32A_new;
						countA = LocalSize(hloc32A_new);
					}
				}
				WideCharToMultiByte(CP_ACP, 0, m_lpszText, countW, (LPSTR)LocalLock(m_hLoc32A), countA, NULL, NULL);
				LocalUnlock(m_hLoc32A);
			}

			LocalUnlock(m_hLoc32W);
			m_lpszText = NULL;
		}
	    else
			return;
	}
	m_nLockCount--;
}

BOOL duEdit::MakeFit(UINT size)
{
	HLOCAL hNew32W;

	if (size <= m_uBufferSize)
		return TRUE;

	UnlockBuffer(TRUE);

	if (m_hLoc32W)
	{
	    UINT alloc_size = ROUND_TO_GROW((size + 1) * sizeof(WCHAR));
	    if ((hNew32W = LocalReAlloc(m_hLoc32W, alloc_size, LMEM_MOVEABLE | LMEM_ZEROINIT)))
		{
			m_hLoc32W = hNew32W;
			m_uBufferSize = LocalSize(hNew32W)/sizeof(WCHAR) - 1;
	    }
	}

	LockBuffer();
	if (m_uBufferSize < size)
		return FALSE;

	return TRUE;
}

BOOL duEdit::MakeUndoFit(UINT size)
{
	if (size <= m_uUndoBufferSize)
		return TRUE;

	UINT alloc_size = ROUND_TO_GROW((size + 1) * sizeof(WCHAR));
	if ((m_lpszUndoText = (LPWSTR)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_lpszUndoText, alloc_size)))
	{
		m_uUndoBufferSize = alloc_size/sizeof(WCHAR) - 1;
		return TRUE;
	}

	return FALSE;
}

void duEdit::SLInvalidateText(INT start, INT end)
{
	RECT line_rect;
	RECT rc;

	GetLineRect(0, start, end, &line_rect);
	if (IntersectRect(&rc, &line_rect, &m_rcFormat))
		_RedrawEdit();
}

void duEdit::InvalidateText(INT start, INT end)
{
	if (end == start)
		return;

	if (end == -1)
		end = GetEditTextLength();

	if (end < start)
	{
	    INT tmp = start;
	    start = end;
	    end = tmp;
	}

	SLInvalidateText(start, end);
}

void duEdit::EMSetSel(UINT start, UINT end)
{
	UINT old_start = m_nSelectionStart;
	UINT old_end = m_nSelectionEnd;
	UINT len = GetEditTextLength();

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

	ORDER_UINT(end, old_end);
	ORDER_UINT(start, old_start);
	ORDER_UINT(old_start, old_end);
	ORDER_UINT(start, end);

	//if (end != old_start)
	//{
	//	if (old_start > end )
	//	{
	//		InvalidateText(start, end);
	//		InvalidateText(old_start, old_end);
	//	}
	//	else
	//	{
	//		InvalidateText(start, old_start);
	//		InvalidateText(end, old_end);
	//	}
	//}
	//else 
	//	InvalidateText(start, old_end);

	if(m_wflags & EF_FOCUSED)
		SetEditCaretPos(m_nSelectionEnd);

	_RedrawEdit();
}

void duEdit::SetEditCaretPos(INT pos)
{
	LRESULT res = EMPosFromChar(pos);
	duRect rcEdit;
	rcEdit.SetRectEmpty();
	GetRect(&rcEdit);
	
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	pCtrlManager->_SetCaretPos(rcEdit.left + (short)LOWORD(res), rcEdit.top + (short)HIWORD(res));
}

void duEdit::EMScrollCaret()
{
	INT x;
	INT goal;
	INT format_width;
    
	x = (short)LOWORD(EMPosFromChar(m_nSelectionEnd));
	format_width = m_rcFormat.right - m_rcFormat.left;
	if (x < m_rcFormat.left)
	{
		goal = m_rcFormat.left + format_width / HSCROLL_FRACTION;
		do
		{
			m_nXoffset--;
			x = (short)LOWORD(EMPosFromChar(m_nSelectionEnd));
		} while ((x < goal) && m_nXoffset);
		_RedrawEdit();
	}
	else if (x > m_rcFormat.right)
	{
		INT x_last;
		INT len = GetEditTextLength();
		goal = m_rcFormat.right - format_width / HSCROLL_FRACTION;
		do
		{
			m_nXoffset++;
			x = (short)LOWORD(EMPosFromChar(m_nSelectionEnd));
			x_last = (short)LOWORD(EMPosFromChar(len));
		} while ((x > goal) && (x_last > m_rcFormat.right));
		_RedrawEdit();
	}
	
    if(m_wflags & EF_FOCUSED)
		SetEditCaretPos(m_nSelectionEnd);
}

void duEdit::MoveBackward(BOOL extend)
{
	INT e = m_nSelectionEnd;

	if (e)
		e--;

	EMSetSel(extend ? m_nSelectionStart : e, e);
	EMScrollCaret();
}

void duEdit::MoveEnd(BOOL extend, BOOL ctrl)
{
	INT e = GetEditTextLength();
	EMSetSel(extend ? m_nSelectionStart : e, e);
	EMScrollCaret();
}

void duEdit::MoveForward(BOOL extend)
{
	INT e = m_nSelectionEnd;

	if (m_lpszText[e])
		e++;
	
	EMSetSel(extend ? m_nSelectionStart : e, e);
	EMScrollCaret();
}

void duEdit::MoveHome(BOOL extend, BOOL ctrl)
{
	INT e = 0;
	EMSetSel(extend ? m_nSelectionStart : e, e);
	EMScrollCaret();
}

void duEdit::MoveWordBackward(BOOL extend)
{
	INT s = m_nSelectionStart;
	INT e = m_nSelectionEnd;
	INT l;
	INT ll;
	INT li;

	l = 0;
	ll = GetEditTextLength();
	li = 0;
	if (e - li == 0)
	{
		if (l)
		{
			li = 0;
			e = li + GetEditTextLength();
		}
	}
	else
		e = li + CallWordBreakProc(li, e - li, ll, WB_LEFT);

	if (!extend)
		s = e;
	EMSetSel(s, e);
	EMScrollCaret();
}

void duEdit::MoveWordForward(BOOL extend)
{
	INT s = m_nSelectionStart;
	INT e = m_nSelectionEnd;
	INT l;
	INT ll;
	INT li;

	l = 0;
	ll = GetEditTextLength();
	li = 0;
	if (e - li != ll)
		e = li + CallWordBreakProc(li, e - li + 1, ll, WB_RIGHT);
	
	if (!extend)
		s = e;
	EMSetSel(s, e);
	EMScrollCaret();
}

void duEdit::AdjustFormatRect()
{
	RECT ClientRect;

	m_rcFormat.right = max(m_rcFormat.right, m_rcFormat.left + m_nCharWidth);
	m_rcFormat.bottom = m_rcFormat.top + m_nLineHeight;

	_GetClientRect(m_hWnd, &ClientRect);
	m_rcFormat.bottom = min(m_rcFormat.bottom, ClientRect.bottom);

	m_rcFormat.top += m_nTopBorder;
	m_rcFormat.bottom += m_nTopBorder;
	m_rcFormat.left += m_nLeftBorder;
	m_rcFormat.right -= m_nRightBorder;

	SetEditCaretPos(m_nSelectionEnd);
}

void duEdit::SetRectNP(const RECT *rc)
{
	INT bw, bh;
	CopyRect(&m_rcFormat, rc);
	
	if (m_dwStyle & WS_BORDER) 
	{
		bw = GetSystemMetrics(SM_CXBORDER) + 1;
		bh = GetSystemMetrics(SM_CYBORDER) + 1;
		m_rcFormat.left += bw;
		m_rcFormat.right -= bw;
		if (m_rcFormat.bottom - m_rcFormat.top >= m_nLineHeight + 2 * bh)
		{
		    m_rcFormat.top += bh;
		    m_rcFormat.bottom -= bh;
		}
	}

	m_rcFormat.left += m_nLeftMargin;
	m_rcFormat.right -= m_nRightMargin;
	AdjustFormatRect();
}

LRESULT duEdit::OnEmCharFromPos(INT x, INT y)
{
	POINT pt;
	RECT rc;
	INT index;

	pt.x = x;
	pt.y = y;
	_GetClientRect(m_hWnd, &rc);
	if (!PtInRect(&rc, pt))
		return -1;

	index = CharFromPos(x, y);
	return MAKELONG(index, 0);
}

void duEdit::OnEmReplaceSel(BOOL can_undo, LPCWSTR lpsz_replace, BOOL send_update, BOOL honor_limit)
{
	UINT strl = lstrlenW(lpsz_replace);
	UINT tl = GetEditTextLength();
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
	
	if ((honor_limit) && (size > m_uBufferLimit))
	{
		if (m_uBufferLimit < (tl - (e-s)))
			strl = 0;
		else
			strl = m_uBufferLimit - (tl - (e-s));
	}

	if (!MakeFit(tl - (e - s) + strl))
		return;

	if (e != s)
	{
		bufl = e - s;
		buf = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, (bufl + 1) * sizeof(WCHAR));
		if (!buf)
			return;
		memcpy(buf, m_lpszText + s, bufl * sizeof(WCHAR));
		buf[bufl] = 0;
		lstrcpyW(m_lpszText + s, m_lpszText + e);
		TextBufferChanged();
	}
	if (strl)
	{
		tl = GetEditTextLength();
		for (p = m_lpszText + tl ; p >= m_lpszText + s ; p--)
			p[strl] = p[0];
			
		for (i = 0 , p = m_lpszText + s ; i < strl ; i++)
			p[i] = lpsz_replace[i];
			
		if(m_dwStyle & ES_UPPERCASE)
			CharUpperBuffW(p, strl);
		else if(m_dwStyle & ES_LOWERCASE)
			CharLowerBuffW(p, strl);

		TextBufferChanged();
	}

	INT fw = m_rcFormat.right - m_rcFormat.left;
	CalcLineWidthSL();
	if (honor_limit && !(m_dwStyle & ES_AUTOHSCROLL) && (m_nTextWidth > fw))
	{
		while ((m_nTextWidth > fw) && s + strl >= s)
		{
			lstrcpyW(m_lpszText + s + strl - 1, m_lpszText + s + strl);
			strl--;
			CalcLineWidthSL();
		}
        TextBufferChanged();
	}

	if (e != s)
	{
		if (can_undo)
		{
			utl = lstrlenW(m_lpszUndoText);
			if (!m_nUndoInsertCount && (*m_lpszUndoText && (s == m_uUndoPosition)))
			{
				MakeUndoFit(utl + e - s);
				memcpy(m_lpszUndoText + utl, buf, (e - s)*sizeof(WCHAR));
				(m_lpszUndoText + utl)[e - s] = 0; /* ensure 0 termination */
			}
			else if (!m_nUndoInsertCount && (*m_lpszUndoText && (e == m_uUndoPosition)))
			{
				MakeUndoFit(utl + e - s);
				for (p = m_lpszUndoText + utl ; p >= m_lpszUndoText ; p--)
					p[e - s] = p[0];
				for (i = 0 , p = m_lpszUndoText ; i < e - s ; i++)
					p[i] = buf[i];
				m_uUndoPosition = s;
			}
			else
			{
				MakeUndoFit(e - s);
				memcpy(m_lpszUndoText, buf, (e - s)*sizeof(WCHAR));
				m_lpszUndoText[e - s] = 0;
				m_uUndoPosition = s;
			}
			m_nUndoInsertCount = 0;
		}
		else
			OnEMEmptyUndoBuffer();
	}
	if (strl)
	{
		if (can_undo)
		{
			if ((s == m_uUndoPosition) || ((m_nUndoInsertCount) && (s == m_uUndoPosition + m_nUndoInsertCount)))
				m_nUndoInsertCount += strl;
			else
			{
				m_uUndoPosition = s;
				m_nUndoInsertCount = strl;
				*m_lpszUndoText = '\0';
			}
		}
		else
			OnEMEmptyUndoBuffer();
	}

	if (bufl)
		HeapFree(GetProcessHeap(), 0, buf);

	s += strl;

	EMSetSel(s, s);
	m_wflags |= EF_MODIFIED;
	if (send_update) m_wflags |= EF_UPDATE;
	if (hrgn)
	{
		_RedrawEdit();
		DeleteObject(hrgn);
	}
	else
        _RedrawEdit();

	EMScrollCaret();
	NotifyUser(DUM_EDITCHANGE, NULL, NULL);
}

int duEdit::CalcMinSetMarginSize(HDC dc, INT left, INT right)
{
    WCHAR magic_string[] = {'\'','*','*','\'', 0};
    SIZE sz;

    GetTextExtentPointW(dc, magic_string, sizeof(magic_string)/sizeof(WCHAR) - 1, &sz);
    return sz.cx + left + right;
}

void duEdit::EMSetMargins(INT action, WORD left, WORD right, BOOL repaint)
{
	TEXTMETRICW tm;
	INT default_left_margin  = 0;
	INT default_right_margin = 0;

	if (m_hFont && (left == EC_USEFONTINFO || right == EC_USEFONTINFO))
	{
		HDC hDC = GetDC(m_hWnd);
		HFONT hOldFont = (HFONT)SelectObject(hDC, m_hFont);
		GetTextMetricsW(hDC, &tm);
		
		if (tm.tmPitchAndFamily & ( TMPF_VECTOR | TMPF_TRUETYPE ))
		{
			int min_size;
			RECT rc;
			
			default_left_margin = tm.tmAveCharWidth / 2;
			default_right_margin = tm.tmAveCharWidth / 2;
			min_size = CalcMinSetMarginSize(hDC, default_left_margin, default_right_margin);
			_GetClientRect(m_hWnd, &rc);
			
			if(rc.right - rc.left < min_size)
			{
				default_left_margin = m_nLeftMargin;
				default_right_margin = m_nRightMargin;
			}
		}
		SelectObject(hDC, hOldFont);
		ReleaseDC(m_hWnd, hDC);
	}

	if (action & EC_LEFTMARGIN)
	{
		m_rcFormat.left -= m_nLeftMargin;
		if (left != EC_USEFONTINFO)
			m_nLeftMargin = left;
		else
			m_nLeftMargin = default_left_margin;
		m_rcFormat.left += m_nLeftMargin;
	}

	if (action & EC_RIGHTMARGIN)
	{
		m_rcFormat.right += m_nRightMargin;
		if (right != EC_USEFONTINFO)
 			m_nRightMargin = right;
		else
			m_nRightMargin = default_right_margin;
		m_rcFormat.right -= m_nRightMargin;
	}

	if (action & (EC_LEFTMARGIN | EC_RIGHTMARGIN))
	{
		AdjustFormatRect();
		if (repaint) 
			_RedrawEdit();
	}
}

BOOL duEdit::OnEmUndo()
{
	INT ulength;
	LPWSTR utext;

	if(m_dwStyle & ES_READONLY)
            return TRUE;

	ulength = lstrlenW(m_lpszUndoText);
	utext = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, (ulength + 1) * sizeof(WCHAR));
    if (utext == NULL)
        return FALSE;

	lstrcpyW(utext, m_lpszUndoText);
	EMSetSel(m_uUndoPosition, m_uUndoPosition + m_nUndoInsertCount);
	OnEMEmptyUndoBuffer();
	OnEmReplaceSel(TRUE, utext, TRUE, TRUE);
	EMSetSel(m_uUndoPosition, m_uUndoPosition + m_nUndoInsertCount);
	NotifyUser(DUM_EDITCHANGE, NULL, NULL);
	EMScrollCaret();
	HeapFree(GetProcessHeap(), 0, utext);

	return TRUE;
}

void WINAPI duEdit::OnPaste()
{
	HGLOBAL hsrc;
	LPWSTR src;
	
	if(m_dwStyle & ES_READONLY)
	    return;

	OpenClipboard(m_hWnd);
	if ((hsrc = GetClipboardData(CF_UNICODETEXT)))
	{
		src = (LPWSTR)GlobalLock(hsrc);
		OnEmReplaceSel(TRUE, src, TRUE, TRUE);
		GlobalUnlock(hsrc);
	}
	else if (m_dwStyle & ES_PASSWORD)
            OnEmReplaceSel(TRUE, empty_stringW, TRUE, TRUE);
	
	CloseClipboard();
}

void WINAPI duEdit::OnCopy()
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
	memcpy(dst, m_lpszText + s, len * sizeof(WCHAR));
	dst[len] = 0;
	
	GlobalUnlock(hdst);
	OpenClipboard(m_hWnd);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hdst);
	CloseClipboard();
}

void WINAPI duEdit::OnClear()
{
	if(m_dwStyle & ES_READONLY)
	    return;

	OnEmReplaceSel(TRUE, empty_stringW, TRUE, TRUE);
}

void WINAPI duEdit::OnCut()
{
	OnCopy();
	OnClear();
}

LRESULT duEdit::OnChar(WCHAR c)
{
	BOOL control = GetKeyState(VK_CONTROL) & 0x8000;

	switch (c)
	{
		case VK_BACK:
		{
			if (!(m_dwStyle & ES_READONLY) && !control)
			{
				if (m_nSelectionStart != m_nSelectionEnd)
					OnClear();
				else
				{
					EMSetSel((UINT)-1, 0);
					MoveBackward(TRUE);
					OnClear();
				}
			}
		}
		break;
		
		case 0x03: /* ^C */
		{
			if (!(m_dwStyle & ES_PASSWORD))
				OnCopy();
		}
		break;
		
		case 0x16: /* ^V */
		{
			if (!(m_dwStyle & ES_READONLY))
				OnPaste();
		}
		break;
		
		case 0x18: /* ^X */
		{
			if (!((m_dwStyle & ES_READONLY) || (m_dwStyle & ES_PASSWORD)))
				OnCut();
		}
		break;
		
		case 0x1A: /* ^Z */
		{
			if (!(m_dwStyle & ES_READONLY))
				OnEmUndo();
		}
		break;
		
		default:
		{
			if((m_dwStyle & ES_NUMBER) && !( c >= '0' && c <= '9') && !(c == '-'))
				break;

			if (!(m_dwStyle & ES_READONLY) && (c >= ' ') && (c != 127))
			{
				WCHAR str[2];
				str[0] = c;
				str[1] = '\0';
				OnEmReplaceSel(TRUE, str, TRUE, TRUE);
			}
		}
		break;
	}
    return 1;
}

INT duEdit::OnGetText(INT count, LPWSTR dst, BOOL unicode)
{
    if(!count)
		return 0;

    if(unicode)
    {
		lstrcpynW(dst, m_lpszText, count);
		return lstrlenW(dst);
    }
    else
    {
		LPSTR textA = (LPSTR)dst;
		if (!WideCharToMultiByte(CP_ACP, 0, m_lpszText, -1, textA, count, NULL, NULL))
			textA[count - 1] = 0;
		return strlen(textA);
    }
}

void WINAPI duEdit::OnKeyDown(UINT key, UINT nRepCnt, UINT nFlags)
{
	BOOL shift;
	BOOL control;

	if (GetKeyState(VK_MENU) & 0x8000)
		return;

	shift = GetKeyState(VK_SHIFT) & 0x8000;
	control = GetKeyState(VK_CONTROL) & 0x8000;

	switch (key)
	{
		case VK_LEFT:
		{
			if (control)
				MoveWordBackward(shift);
			else
				MoveBackward(shift);
		}
		break;
			
		case VK_RIGHT:
		{
			if (control)
				MoveWordForward(shift);
			else
				MoveForward(shift);
		}
		break;
		
		case VK_HOME:
			MoveHome(shift, control);
			break;
		
		case VK_END:
			MoveEnd(shift, control);
			break;

		case VK_DELETE:
		{
			if (!(m_dwStyle & ES_READONLY) && !(shift && control)) 
			{
				if (m_nSelectionStart != m_nSelectionEnd) 
				{
					if (shift)
						OnCut();
					else
						OnClear();
				}
				else
				{
					if (shift) 
					{
						EMSetSel((UINT)-1, 0);
						MoveBackward(TRUE);
						OnClear();
					}
					else if (control) 
					{
						EMSetSel((UINT)-1, 0);
						MoveEnd(TRUE, FALSE);
						OnClear();
					}
					else 
					{
						EMSetSel((UINT)-1, 0);
						MoveForward(TRUE);
						OnClear();
					}
				}
			}
		}
		break;
		
		case VK_INSERT:
		{
			if (shift)
			{
				if (!(m_dwStyle & ES_READONLY))
					OnPaste();
			} else if (control)
				OnCopy();
		}
		break;
	}
}

BOOL WINAPI duEdit::OnKillFocus(duPlugin *pNewFocus)
{
	m_wflags &= ~EF_FOCUSED;

	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return FALSE;

	pCtrlManager->_DestroyCaret();
	InvalidateText(m_nSelectionStart, m_nSelectionEnd);
	m_fShowCaret = FALSE;
	return TRUE;
}

LRESULT duEdit::OnLButtonDown(INT x, INT y)
{
	INT e;

	m_fCaptureState = TRUE;
	Plugin_SetCapture(this);
	ConfinePoint(&x, &y);
	e = CharFromPos(x, y);
	EMSetSel(e, e);
	EMScrollCaret();
	m_nRegionPosX = m_nRegionPosY = 0;
	Plugin_SetTimer(this, m_lTimerId, 100);

	if (!(m_wflags & EF_FOCUSED))
		::SetFocus(m_hWnd);
	else
	{
		// 当此plugin是focus状态，但是窗口失去焦点再切换回来时，focus不变，不会重新创建显示Caret
		duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
		if (pCtrlManager)
		{
			pCtrlManager->_DestroyCaret();
			pCtrlManager->_CreateCaret(1, m_nLineHeight);
			SetEditCaretPos(m_nSelectionEnd);
			pCtrlManager->_ShowCaret();
		}
	}

	return 0;
}

LRESULT duEdit::OnMouseMove(INT x, INT y)
{
	INT e;
	INT prex, prey;
	
	if (!m_fCaptureState || Plugin_GetCapture(this) != this)
		return 0;

	prex = x; prey = y;
	ConfinePoint(&x, &y);
	m_nRegionPosX = (prex < x) ? -1 : ((prex > x) ? 1 : 0);
	m_nRegionPosY = (prey < y) ? -1 : ((prey > y) ? 1 : 0);
	e = CharFromPos(x, y);
	EMSetSel(m_nSelectionStart, e);
	SetEditCaretPos(m_nSelectionEnd);
	return 0;
}

BOOL WINAPI duEdit::OnSetFocus(duPlugin *pOldFocus)
{
	m_fShowCaret = TRUE;
	m_wflags |= EF_FOCUSED;

	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return FALSE;

	pCtrlManager->_CreateCaret(1, m_nLineHeight);
	SetEditCaretPos(m_nSelectionEnd);
	pCtrlManager->_ShowCaret();
	return TRUE;
}

void duEdit::OnSetFont(HFONT font, BOOL redraw)
{
	TEXTMETRICW tm;
	HDC hDC;
	HFONT hOldFont = NULL;
	RECT clientRect;

	m_hFont = font;
	hDC = GetDC(m_hWnd);
	if (font)
		hOldFont = (HFONT)SelectObject(hDC, font);
	
	GetTextMetricsW(hDC, &tm);
	m_nLineHeight = tm.tmHeight;
	m_nCharWidth = tm.tmAveCharWidth;
	if (font)
		SelectObject(hDC, hOldFont);
	ReleaseDC(m_hWnd, hDC);

	_GetClientRect(m_hWnd, &clientRect);
	SetRectNP(&clientRect);
	EMSetMargins(EC_LEFTMARGIN | EC_RIGHTMARGIN, EC_USEFONTINFO, EC_USEFONTINFO, FALSE);

    CalcLineWidthSL();

	if (redraw)
		_RedrawEdit();
	
	if (m_wflags & EF_FOCUSED)
	{
		duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
		if (pCtrlManager == NULL)
			return;

		pCtrlManager->_DestroyCaret();
		pCtrlManager->_CreateCaret(1, m_nLineHeight);
		SetEditCaretPos(m_nSelectionEnd);
		pCtrlManager->_ShowCaret();
	}
}

LRESULT duEdit::OnSysKeyDown(INT key, DWORD key_data)
{
	if ((key == VK_BACK) && (key_data & 0x2000))
	{
		if (OnEMCanUndo())
			OnEmUndo();
		return 0;
	} 

	return 0;
}

void WINAPI duEdit::Resize(LPRECT lpRect/*=NULL*/)
{
	duPlugin::Resize(lpRect);
	
	duRect rcEdit;
	SetRectEmpty(&rcEdit);
    GetRect(&rcEdit);
	rcEdit.OffsetRect(-rcEdit.left, -rcEdit.top);
    SetRectNP(&rcEdit);
	
	//Plugin_Redraw(this, TRUE);
}

void duEdit::GetCompositionStr(HIMC hIMC, LPARAM CompFlag)
{
    LONG buflen;
    LPWSTR lpCompStr = NULL;
    LPSTR lpCompStrAttr = NULL;
    DWORD dwBufLenAttr;

    buflen = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, NULL, 0);
    if (buflen < 0)
        return;

    lpCompStr = (LPWSTR)HeapAlloc(GetProcessHeap(),0,buflen + sizeof(WCHAR));
    if (!lpCompStr)
        return;

    if (buflen)
        ImmGetCompositionStringW(hIMC, GCS_COMPSTR, lpCompStr, buflen);
	
    lpCompStr[buflen/sizeof(WCHAR)] = 0;
    if (CompFlag & GCS_COMPATTR)
    {
        dwBufLenAttr = ImmGetCompositionStringW(hIMC, GCS_COMPATTR, NULL, 0);
        if (dwBufLenAttr)
        {
            dwBufLenAttr ++;
            lpCompStrAttr = (LPSTR)HeapAlloc(GetProcessHeap(),0,dwBufLenAttr+1);
            if (!lpCompStrAttr)
            {
                HeapFree(GetProcessHeap(),0,lpCompStr);
                return;
            }
            ImmGetCompositionStringW(hIMC,GCS_COMPATTR, lpCompStrAttr, dwBufLenAttr);
            lpCompStrAttr[dwBufLenAttr] = 0;
        }
        else
            lpCompStrAttr = NULL;
    }

    if (m_nSelectionEnd < m_nCompositionStart)
        m_nCompositionStart = m_nSelectionEnd;

    m_nSelectionStart = m_nCompositionStart;

    if (m_nCompositionLen > 0)
        m_nSelectionEnd = m_nCompositionStart + m_nCompositionLen;
    else
        m_nSelectionEnd = m_nSelectionStart;

    OnEmReplaceSel(FALSE, lpCompStr, TRUE, TRUE);
    m_nCompositionLen = abs(m_nCompositionStart - m_nSelectionEnd);

    m_nSelectionStart = m_nCompositionStart;
    m_nSelectionEnd = m_nSelectionStart + m_nCompositionLen;

    HeapFree(GetProcessHeap(),0,lpCompStrAttr);
    HeapFree(GetProcessHeap(),0,lpCompStr);
}

void duEdit::Init(BOOL fUnicode)
{
	m_fUnicode = fUnicode;
	m_dwStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL;

    m_fEnableState = !(m_dwStyle & WS_DISABLED);
    if (m_dwStyle & ES_NUMBER)
	{
    }
	else if (m_dwStyle & ES_LOWERCASE)
	{
        m_dwStyle &= ~ES_UPPERCASE;
    }

	m_uBufferLimit = BUFLIMIT_INITIAL;
	if (m_dwStyle & ES_PASSWORD)
		m_chPassword = '*';

	UINT alloc_size = ROUND_TO_GROW((m_uBufferSize + 1) * sizeof(WCHAR));
	if(!(m_hLoc32W = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, alloc_size)))
	    goto cleanup;
	
	m_uBufferSize = LocalSize(m_hLoc32W)/sizeof(WCHAR) - 1;
	if (!(m_lpszUndoText = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (m_uBufferSize + 1) * sizeof(WCHAR))))
		goto cleanup;
	
	m_uUndoBufferSize = m_uBufferSize;
	m_nLineCount = 1;
	LockBuffer();

	return;

cleanup:
	HeapFree(GetProcessHeap(), 0, m_pFirstLineDef);
	HeapFree(GetProcessHeap(), 0, m_lpszUndoText);
	if (m_hLoc32W)
		LocalFree(m_hLoc32W);
}



//ResetHFont
void duEdit::ResetHFont()
{
	duFont *pFont = (duFont *)GetResObj(m_szFont, DU_RES_FONT);
	if (pFont)
		m_hFont = pFont->GetHFont();
	else
		m_hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);

}


void duEdit::Destroy()
{
	LINEDEF *pc, *pp;

	if (m_hLoc32W)
	{
		LocalUnlock(m_hLoc32W);
		LocalFree(m_hLoc32W);
		m_hLoc32W = NULL;
	}
	
	if (m_hLoc32A)
	{
		LocalUnlock(m_hLoc32A);
		LocalFree(m_hLoc32A);
		m_hLoc32A = NULL;
	}
	
	pc = m_pFirstLineDef;
	while (pc)
	{
		pp = pc->next;
		HeapFree(GetProcessHeap(), 0, pc);
		pc = pp;
	}

	HeapFree(GetProcessHeap(), 0, m_lpszUndoText);
}

LRESULT WINAPI duEdit::EditWndProc_common(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL unicode)
{
	LRESULT result = 0;
	if (msg != WM_NCDESTROY)
		LockBuffer();

	switch (msg) 
	{
		case WM_IME_CHAR:
		{
			if (!unicode)
			{
				WCHAR charW;
				CHAR  string[2];

				string[0] = wParam >> 8;
				string[1] = wParam & 0xff;
				if (string[0]) 
					MultiByteToWideChar(CP_ACP, 0, string, 2, &charW, 1);
				else 
					MultiByteToWideChar(CP_ACP, 0, &string[1], 1, &charW, 1);
				result = OnChar(charW);
				break;
			}
		}
		break;

		case WM_CHAR:
		{
			WCHAR charW;

			if(unicode)
				charW = wParam;
			else
			{
				CHAR charA = wParam;
				MultiByteToWideChar(CP_ACP, 0, &charA, 1, &charW, 1);
			}

			result = OnChar(charW);
		}
		break;

		case WM_UNICHAR:
		{
			if (unicode)
			{
				if (wParam == UNICODE_NOCHAR)
					return TRUE;
				if (wParam <= 0x000fffff)
				{
					if(wParam > 0xffff) /* convert to surrogates */
					{
						wParam -= 0x10000;
						OnChar((wParam >> 10) + 0xd800);
						OnChar((wParam & 0x03ff) + 0xdc00);
					}
					else 
						OnChar(wParam);
				}
				return 0;
			}
		}
		break;

		case WM_CLEAR:
			OnClear();
			break;

		case WM_COPY:
			if (!m_fPassword)
				OnCopy();
			break;

		case WM_CUT:
			OnCut();
			break;

		case WM_ENABLE:
		{
			m_fEnableState = (BOOL) wParam;
			_RedrawEdit();
		}
		break;

		case WM_GETTEXTLENGTH:
		{
			if (unicode)
				result = GetEditTextLength();
			else 
				result = WideCharToMultiByte(CP_ACP, 0, m_lpszText, GetEditTextLength(), NULL, 0, NULL, NULL);
		}
		break;

		case WM_PASTE:
			OnPaste();
			break;

		case WM_SYSKEYDOWN:
			result = OnSysKeyDown((INT)wParam, (DWORD)lParam);
			break;

		case WM_IME_STARTCOMPOSITION:
		{
			m_nCompositionStart = m_nSelectionEnd;
			m_nCompositionLen = 0;
		}
		break;

		case WM_IME_ENDCOMPOSITION:
		{
			if (m_nCompositionLen > 0)
			{
				OnEmReplaceSel(TRUE, empty_stringW, TRUE, TRUE);
				m_nSelectionEnd = m_nSelectionStart;
				m_nCompositionLen= 0;
			}
		}
		break;


	default:
		break;
	}

	if (IsWindow(hwnd))
		UnlockBuffer(FALSE);

	return result;
}

void duEdit::_GetClientRect(HWND hWnd, LPRECT lpRect)
{
	if (lpRect)
	{
		::SetRectEmpty(lpRect);
		GetRect(lpRect);
		::OffsetRect(lpRect, -lpRect->left, -lpRect->top);
	}
}

void duEdit::_TextOutW(HDC hDC, HFONT hFont, int nXStart, int nYStart, LPCWSTR lpString, int cbString, BOOL fSelect)
{	
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HFONT hOldFont = (HFONT)::SelectObject(hMemDC, hFont);

	duSize size;
	GetTextExtentPoint32W(hMemDC, lpString, cbString, &size);

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
		
		TextOutW(hMemDC, 0, 0, lpString, cbString);

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
		TextOutW(hMemDC, 0, 0, lpString, cbString);

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
}

void duEdit::_RedrawEdit()
{
	if (!m_fRedraw)
		return;

	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	if (m_fShowCaret)
		pCtrlManager->_HideCaret();

	Plugin_Redraw(this, TRUE);

	if (m_fShowCaret)
		pCtrlManager->_ShowCaret();
}

void WINAPI duEdit::SetText(LPCTSTR lpszText)
{
	m_fRedraw = FALSE;

    EMSetSel(0, (UINT)-1);
    if (lpszText) 	
		OnEmReplaceSel(FALSE, lpszText, FALSE, FALSE);
    else 
    {
		static const WCHAR empty_stringW[] = {0};
		OnEmReplaceSel(FALSE, empty_stringW, FALSE, FALSE);
    }
    m_nXoffset = 0;
	m_wflags &= ~EF_MODIFIED;
    EMSetSel(0, 0);
	NotifyUser(DUM_EDITCHANGE, NULL, NULL);
    EMScrollCaret();

	m_fRedraw = TRUE;
	_RedrawEdit();
}

DWORD WINAPI duEdit::GetSel()
{
	UINT s = m_nSelectionStart;
	UINT e = m_nSelectionEnd;

	ORDER_UINT(s, e);
	return MAKELONG(s, e);
}

void WINAPI duEdit::PosFromChar(int nIndex, LPPOINT lppt)
{
	if (lppt == NULL)
		return;
	
	LRESULT lRes = EMPosFromChar(nIndex);
	lppt->x = GET_X_LPARAM(lRes);
	lppt->y = GET_Y_LPARAM(lRes);
}

void WINAPI duEdit::SetLimitText(UINT uMax)
{
    if (uMax == 0xFFFFFFFF)
        m_uBufferLimit = -1;

	if (uMax)
		m_uBufferLimit = min(uMax, BUFLIMIT_SINGLE);
	else
		m_uBufferLimit = BUFLIMIT_SINGLE;
}

void WINAPI duEdit::ReplaceSel(LPCTSTR lpszNewText, BOOL fCanUndo)
{
	OnEmReplaceSel(fCanUndo, lpszNewText, TRUE, TRUE);
}

void WINAPI duEdit::SetSel(int nStartChar, int nEndChar)
{
	EMSetSel(nStartChar, nEndChar);
}


void WINAPI duEdit::SetReadOnly(BOOL fReadOnly)
{
	m_fReadOnly = fReadOnly;
	if (m_fReadOnly)
	{
		m_dwStyle |= ES_READONLY;
	}
	else
	{
		m_dwStyle &= ~ES_READONLY;
	}
}

void WINAPI duEdit::SetPassword(BOOL fPassword)
{
	m_fPassword = fPassword;
	if (m_fPassword)
	{
		m_dwStyle |= ES_PASSWORD;
	}
	else
	{
		m_dwStyle &= ~ES_PASSWORD;
	}
}

void WINAPI duEdit::SetNumber(BOOL fNumber)
{
	m_fNumber = fNumber;
	if (m_fNumber)
	{
		m_dwStyle |= ES_NUMBER;
	}
	else
	{
		m_dwStyle &= ~ES_NUMBER;
	}
}

void WINAPI duEdit::SetFont(LPCTSTR lpszFont)
{
	if (lpszFont)
	{
		_tcsncpy(m_szFont, lpszFont, MAX_NAME);
		
		duFont *pFont = (duFont *)GetResObj(m_szFont, DU_RES_FONT);
		if (pFont)
			OnSetFont(pFont->GetHFont(), TRUE);
	}
	else
		ZeroMemory(m_szFont, sizeof(TCHAR) * MAX_NAME);
}

void WINAPI duEdit::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
}

void WINAPI duEdit::SetSelectTextColor(COLORREF clrSelectText)
{
	m_clrSelectText = clrSelectText;
}

void WINAPI duEdit::SetSelectColor(COLORREF clrSelect)
{
	m_clrSelect = clrSelect;
}

void WINAPI duEdit::SetLeftBorder(int nLeftBorder)
{
	m_nLeftBorder = nLeftBorder;
}

void WINAPI duEdit::SetTopBorder(int nTopBorder)
{
	m_nTopBorder = nTopBorder;
}
void WINAPI duEdit::SetRightBorder(int nRightBorder)
{
	m_nRightBorder = nRightBorder;
}
void WINAPI duEdit::SetBottomBorder(int nBottomBorder)
{
	m_nBottomBorder = nBottomBorder;
}

