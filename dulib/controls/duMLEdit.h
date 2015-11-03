//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duMLEdit.h
//
//  History:    Nov-23-2009   Steven Li  Created
//
//--------------------------------------------------------------------------
#pragma once

#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"
#include "duEdit.h"
#include "duScroll.h"

//
//
//
class duMLEdit : public duPlugin
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	
	virtual LRESULT OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnKillFocus(duPlugin *pNewFocus);
	virtual BOOL OnSetFocus(duPlugin *pOldFocus);
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseDbClick(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseRDown(POINT pt);
	virtual void OnMouseRUp(POINT pt);
	virtual void OnTimer(UINT nEventId);
	virtual BOOL OnSetCursor();
	virtual void OnMouseWheel(UINT fwKeys, int zDelta, POINT pt);
	virtual void OnVScroll(ScrollBarAction sbAction, int nPos);
	virtual void OnHScroll(ScrollBarAction sbAction, int nPos);

	virtual void SetText(LPCTSTR lpszText);
	virtual LPCTSTR GetText() { return m_szTextW; }
	virtual void SetSel(UINT start, UINT end, BOOL after_wrap);
	virtual LRESULT GetSel(PUINT start, PUINT end);

public:
	duMLEdit();
	~duMLEdit();
	
public:
	BOOL CanUndo();
	void EmptyUndoBuffer();
	void OnClear();
	void OnCut();

	void BuildLineDefs_ML(INT iStart, INT iEnd, INT delta, HRGN hrgn);
	void MoveDown_ML(BOOL extend);
	void MovePageDown_ML(BOOL extend);
	void MovePageUp_ML(BOOL extend);
	void MoveUp_ML(BOOL extend);

	INT	CallWordBreakProc(INT start, INT index, INT count, INT action);
	INT	CharFromPos(INT x, INT y, LPBOOL after_wrap);
	void ConfinePoint(INT *x, INT *y);
	void GetLineRect(INT line, INT scol, INT ecol, LPRECT rc);

	void LockBuffer();
	BOOL MakeFit(UINT size);
	BOOL MakeUndoFit(UINT size);
	void MoveBackward(BOOL extend);
	void MoveEnd(BOOL extend);
	void MoveForward(BOOL extend);
	void MoveHome(BOOL extend);
	void MoveWordBackward(BOOL extend);
	void MoveWordForward(BOOL extend);
	void PaintLine(HDC hdc, INT line, BOOL rev);
	INT	PaintText(HDC hdc, INT x, INT y, INT line, INT col, INT count, BOOL rev);
	void SetCaretPos(INT pos, BOOL after_wrap);
	void AdjustFormatRect();
	void SetRectNP(LPRECT lprc);
	void UnlockBuffer(BOOL force);
	void UpdateScrollInfo();
	
	LRESULT	CharFromPos(INT x, INT y);

	INT	GetLine(INT line, LPWSTR dst, BOOL unicode);
	//LRESULT	GetSel(PUINT start, PUINT end);
	INT	LineFromChar(INT index);
	INT	LineIndex(INT line);
	INT	LineLength(INT index);
	BOOL LineScroll(INT dx, INT dy);
	BOOL LineScroll_internal(INT dx, INT dy);
	LRESULT	PosFromChar(INT index, BOOL after_wrap);
	void ReplaceSel(BOOL can_undo, LPCWSTR lpsz_replace, BOOL send_update, BOOL honor_limit);
	void ScrollCaret();

	void SetLimitText(INT limit);
	//void SetSel(UINT start, UINT end, BOOL after_wrap);

	void SetWordBreakProc(void *wbp);

	BOOL Undo();
	void OnChar(WCHAR c);
	void OnContextMenu(INT x, INT y);
	void OnCopy();
	INT	OnGetText(INT count, LPWSTR dst, BOOL unicode);
	void OnPaste();
	void OnSetFont(HFONT font);
	void OnSetText(LPCWSTR text, BOOL unicode);
	LRESULT OnStyleChanged(WPARAM which, const STYLESTRUCT *style);
	LRESULT	OnSysKeyDown(INT key, DWORD key_data);

protected:
	void Init(BOOL fUnicode);
	DWORD _TextOutW(HDC hDC, HFONT hFont, int nXStart, int nYStart, LPCWSTR lpString, int cbString, BOOL fSelect);
	DWORD _TabbedTextOutW(HDC hDC, HFONT hFont, int nXStart, int nYStart, LPCWSTR lpString, int cbString, 
		int nTabPositions, CONST LPINT lpnTabStopPositions, int nTabOrigin, BOOL fSelect);
	DWORD _GetTabbedTextExtentW(HDC hDC, LPCWSTR lpString, int nCount, int nTabPositions, CONST LPINT lpnTabStopPositions);
	duRect _GetClientRect();
	BOOL IsHorzScrollBarShow();
	BOOL IsVertScrollBarShow();
	void _RedrawEdit();

protected:
	int m_lTimerId;
	BOOL m_fShowCaret;
	BOOL m_fMouseIn;

public:
	TCHAR m_szVertScroll[MAX_NAME];
	TCHAR m_szHorzScroll[MAX_NAME];
	TCHAR m_szFont[MAX_NAME];
	BOOL m_fVertScroll;
	COLORREF m_clrText;
	COLORREF m_clrSelectText;
	COLORREF m_clrSelect;

protected:
	BOOL m_fUnicode;			/* how the control was created */
	LPWSTR m_szTextW;			/* the actual contents of the control */
	UINT m_nBufferSize;			/* the size of the buffer in characters */
	UINT m_nBufferLimit;		/* the maximum size to which the buffer may grow in characters */
	HFONT m_hFont;				/* NULL means standard system font */
	INT m_nLineHeight;			/* height of a screen line in pixels */
	INT m_nCharWidth;			/* average character width in pixels */
	DWORD m_nStyle;				/* sane version of wnd->dwStyle */
	WORD m_nFlags;				/* flags that are not in es->style or wnd->flags (EF_XXX) */
	INT m_nUndoInsertCount;		/* number of characters inserted in sequence */
	UINT m_nUndoPosition;		/* character index of the insertion and deletion */
	LPWSTR m_szUndoTextW;		/* deleted text */
	UINT m_nUndoBufferSize;		/* size of the deleted text buffer */
	INT m_nSelectionStart;		/* == selection_end if no selection */
	INT m_nSelectionEnd;		/* == current caret position */
	WCHAR m_nPasswordChar;		/* == 0 if no password char, and for multi line controls */
	INT m_nLeftMargin;			/* in pixels */
	INT m_nRightMargin;			/* in pixels */
	duRect m_rcFormat;
	INT m_nTextWidth;			/* width of the widest line in pixels for multi line controls
									and just line width for single line controls	*/
	INT m_nRegionPosX;			/* Position of cursor relative to region: */
	INT m_nRegionPosY;			/* -1: to left, 0: within, 1: to right */
	void *m_pfWordbreakProc;		/* 32-bit word break proc: ANSI or Unicode */
	INT m_nLineCount;			/* number of lines */
	BOOL m_fCaptureState; 		/* flag indicating whether mouse was captured */
	BOOL m_fEnableState;		/* flag keeping the enable state */
	int m_nYOffset;
	int m_nXOffset;
	/*
	 *	only for multi line controls
	 */
	INT m_nLockCount;			/* amount of re-entries in the EditWndProc */
	LINEDEF *m_pFirstLineDef;	/* linked list of (soft) linebreaks */
	HLOCAL m_hLoc32W;			/* our unicode local memory block */
	HLOCAL m_hLoc32A;			/* alias for ANSI control receiving EM_GETHANDLE
									or EM_SETHANDLE */
};
