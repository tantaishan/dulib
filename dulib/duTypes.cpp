// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
#pragma once

#include "duTypes.h"

// CSize
duSize::duSize() throw()
	{ /* random filled */ }
duSize::duSize(int initCX, int initCY) throw()
	{ cx = initCX; cy = initCY; }
duSize::duSize(SIZE initSize) throw()
	{ *(SIZE*)this = initSize; }
duSize::duSize(POINT initPt) throw()
	{ *(POINT*)this = initPt; }
duSize::duSize(DWORD dwSize) throw()
	{
		cx = (short)LOWORD(dwSize);
		cy = (short)HIWORD(dwSize);
	}
BOOL duSize::operator==(SIZE size) const throw()
	{ return (cx == size.cx && cy == size.cy); }
BOOL duSize::operator!=(SIZE size) const throw()
	{ return (cx != size.cx || cy != size.cy); }
void duSize::operator+=(SIZE size) throw()
	{ cx += size.cx; cy += size.cy; }
void duSize::operator-=(SIZE size) throw()
	{ cx -= size.cx; cy -= size.cy; }
void duSize::SetSize(int CX, int CY) throw()
	{ cx = CX; cy = CY; }	
duSize duSize::operator+(SIZE size) const throw()
	{ return duSize(cx + size.cx, cy + size.cy); }
duSize duSize::operator-(SIZE size) const throw()
	{ return duSize(cx - size.cx, cy - size.cy); }
duSize duSize::operator-() const throw()
	{ return duSize(-cx, -cy); }
duPoint duSize::operator+(POINT point) const throw()
	{ return duPoint(cx + point.x, cy + point.y); }
duPoint duSize::operator-(POINT point) const throw()
	{ return duPoint(cx - point.x, cy - point.y); }
duRect duSize::operator+(const RECT* lpRect) const throw()
	{ return duRect(lpRect) + *this; }
duRect duSize::operator-(const RECT* lpRect) const throw()
	{ return duRect(lpRect) - *this; }

// CPoint
duPoint::duPoint() throw()
	{ /* random filled */ }
duPoint::duPoint(int initX, int initY) throw()
	{ x = initX; y = initY; }
duPoint::duPoint(POINT initPt) throw()
	{ *(POINT*)this = initPt; }
duPoint::duPoint(SIZE initSize) throw()
	{ *(SIZE*)this = initSize; }
duPoint::duPoint(LPARAM dwPoint) throw()
	{
		x = (short)LOWORD(dwPoint);
		y = (short)HIWORD(dwPoint);
	}
void duPoint::Offset(int xOffset, int yOffset) throw()
	{ x += xOffset; y += yOffset; }
void duPoint::Offset(POINT point) throw()
	{ x += point.x; y += point.y; }
void duPoint::Offset(SIZE size) throw()
	{ x += size.cx; y += size.cy; }
void duPoint::SetPoint(int X, int Y) throw()
	{ x = X; y = Y; }
BOOL duPoint::operator==(POINT point) const throw()
	{ return (x == point.x && y == point.y); }
BOOL duPoint::operator!=(POINT point) const throw()
	{ return (x != point.x || y != point.y); }
void duPoint::operator+=(SIZE size) throw()
	{ x += size.cx; y += size.cy; }
void duPoint::operator-=(SIZE size) throw()
	{ x -= size.cx; y -= size.cy; }
void duPoint::operator+=(POINT point) throw()
	{ x += point.x; y += point.y; }
void duPoint::operator-=(POINT point) throw()
	{ x -= point.x; y -= point.y; }
duPoint duPoint::operator+(SIZE size) const throw()
	{ return duPoint(x + size.cx, y + size.cy); }
duPoint duPoint::operator-(SIZE size) const throw()
	{ return duPoint(x - size.cx, y - size.cy); }
duPoint duPoint::operator-() const throw()
	{ return duPoint(-x, -y); }
duPoint duPoint::operator+(POINT point) const throw()
	{ return duPoint(x + point.x, y + point.y); }
duSize duPoint::operator-(POINT point) const throw()
	{ return duSize(x - point.x, y - point.y); }
duRect duPoint::operator+(const RECT* lpRect) const throw()
	{ return duRect(lpRect) + *this; }
duRect duPoint::operator-(const RECT* lpRect) const throw()
	{ return duRect(lpRect) - *this; }

// CRect
duRect::duRect() throw()
	{ /* random filled */ }
duRect::duRect(int l, int t, int r, int b) throw()
{
	left = l; top = t; right = r; bottom = b; 
}

duRect::duRect(const RECT& srcRect) throw()
{
	left = srcRect.left;
	top = srcRect.top;
	right = srcRect.right;
	bottom = srcRect.bottom;
}

duRect::duRect(LPCRECT lpSrcRect) throw()
{ 
	left = lpSrcRect->left;
	top = lpSrcRect->top;
	right = lpSrcRect->right;
	bottom = lpSrcRect->bottom;
}

duRect::duRect(POINT point, SIZE size) throw()
	{ right = (left = point.x) + size.cx; bottom = (top = point.y) + size.cy; }
duRect::duRect(POINT topLeft, POINT bottomRight) throw()
	{ left = topLeft.x; top = topLeft.y;
		right = bottomRight.x; bottom = bottomRight.y; }
int duRect::Width() const throw()
	{ return right - left; }
int duRect::Height() const throw()
	{ return bottom - top; }
duSize duRect::Size() const throw()
	{ return duSize(right - left, bottom - top); }
duPoint& duRect::TopLeft() throw()
	{ return *((duPoint*)this); }
duPoint& duRect::BottomRight() throw()
	{ return *((duPoint*)this+1); }
const duPoint& duRect::TopLeft() const throw()
	{ return *((duPoint*)this); }
const duPoint& duRect::BottomRight() const throw()
	{ return *((duPoint*)this+1); }
duPoint duRect::CenterPoint() const throw()
	{ return duPoint((left+right)/2, (top+bottom)/2); }
void duRect::SwapLeftRight() throw()
	{ SwapLeftRight(LPRECT(this)); }
void duRect::SwapLeftRight(LPRECT lpRect) throw()
	{ LONG temp = lpRect->left; lpRect->left = lpRect->right; lpRect->right = temp; }
duRect::operator LPRECT() throw()
	{ return this; }
duRect::operator LPCRECT() const throw()
	{ return this; }
BOOL duRect::IsRectEmpty() const throw()
	{ return ::IsRectEmpty(this); }
BOOL duRect::IsRectNull() const throw()
	{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }
BOOL duRect::PtInRect(POINT point) const throw()
{
	return ((point.y >= top)  && (point.y < bottom) && (point.x >= left) && (point.x < right));
		//return ::PtInRect(this, point); 
}

void duRect::SetRect(int x1, int y1, int x2, int y2) throw()
{ 
	left = x1;
	top = y1;
	right = x2;
	bottom = y2;
}

void duRect::SetRect(POINT topLeft, POINT bottomRight) throw()
	{ ::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y); }
void duRect::SetRectEmpty() throw()
{
	left = top = right = bottom = 0;
}

void duRect::CopyRect(LPCRECT lpSrcRect) throw()
	{ ::CopyRect(this, lpSrcRect); }
BOOL duRect::EqualRect(LPCRECT lpRect) const throw()
	{ return ::EqualRect(this, lpRect); }
void duRect::InflateRect(int x, int y) throw()
	{ ::InflateRect(this, x, y); }
void duRect::InflateRect(SIZE size) throw()
	{ ::InflateRect(this, size.cx, size.cy); }
void duRect::DeflateRect(int x, int y) throw()
	{ ::InflateRect(this, -x, -y); }
void duRect::DeflateRect(SIZE size) throw()
	{ ::InflateRect(this, -size.cx, -size.cy); }
void duRect::OffsetRect(int x, int y) throw()
{
    left   += x;
    right  += x;
    bottom += y;
    top    += y;
}

void duRect::OffsetRect(POINT point) throw()
	{ ::OffsetRect(this, point.x, point.y); }
void duRect::OffsetRect(SIZE size) throw()
	{ ::OffsetRect(this, size.cx, size.cy); }
void duRect::MoveToY(int y) throw()
	{ bottom = Height() + y; top = y; }
void duRect::MoveToX(int x) throw()
	{ right = Width() + x; left = x; }
void duRect::MoveToXY(int x, int y) throw()
	{ MoveToX(x); MoveToY(y); }
void duRect::MoveToXY(POINT pt) throw()
	{ MoveToX(pt.x); MoveToY(pt.y); }

BOOL duRect::IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2) throw()
{
    left  = max(lpRect1->left, lpRect2->left);
    right = min(lpRect1->right, lpRect2->right);

    if (left < right) 
	{
        top = max(lpRect1->top, lpRect2->top);
        bottom = min(lpRect1->bottom, lpRect2->bottom);

        if (top < bottom)
            return TRUE;
    }
	
	left = top = right = bottom = 0;
    return FALSE;
}

BOOL duRect::UnionRect(LPCRECT lpRect1, LPCRECT lpRect2) throw()
	{ return ::UnionRect(this, lpRect1, lpRect2); }
void duRect::operator=(const RECT& srcRect) throw()
	{ ::CopyRect(this, &srcRect); }
BOOL duRect::operator==(const RECT& rect) const throw()
	{ return ::EqualRect(this, &rect); }
BOOL duRect::operator!=(const RECT& rect) const throw()
	{ return !::EqualRect(this, &rect); }
void duRect::operator+=(POINT point) throw()
	{ ::OffsetRect(this, point.x, point.y); }
void duRect::operator+=(SIZE size) throw()
	{ ::OffsetRect(this, size.cx, size.cy); }
void duRect::operator+=(LPCRECT lpRect) throw()
	{ InflateRect(lpRect); }
void duRect::operator-=(POINT point) throw()
	{ ::OffsetRect(this, -point.x, -point.y); }
void duRect::operator-=(SIZE size) throw()
	{ ::OffsetRect(this, -size.cx, -size.cy); }
void duRect::operator-=(LPCRECT lpRect) throw()
	{ DeflateRect(lpRect); }
void duRect::operator&=(const RECT& rect) throw()
	{ ::IntersectRect(this, this, &rect); }
void duRect::operator|=(const RECT& rect) throw()
	{ ::UnionRect(this, this, &rect); }
duRect duRect::operator+(POINT pt) const throw()
	{ duRect rect(*this); ::OffsetRect(&rect, pt.x, pt.y); return rect; }
duRect duRect::operator-(POINT pt) const throw()
	{ duRect rect(*this); ::OffsetRect(&rect, -pt.x, -pt.y); return rect; }
duRect duRect::operator+(SIZE size) const throw()
	{ duRect rect(*this); ::OffsetRect(&rect, size.cx, size.cy); return rect; }
duRect duRect::operator-(SIZE size) const throw()
	{ duRect rect(*this); ::OffsetRect(&rect, -size.cx, -size.cy); return rect; }
duRect duRect::operator+(LPCRECT lpRect) const throw()
	{ duRect rect(this); rect.InflateRect(lpRect); return rect; }
duRect duRect::operator-(LPCRECT lpRect) const throw()
	{ duRect rect(this); rect.DeflateRect(lpRect); return rect; }
duRect duRect::operator&(const RECT& rect2) const throw()
	{ duRect rect; ::IntersectRect(&rect, this, &rect2);
		return rect; }
duRect duRect::operator|(const RECT& rect2) const throw()
	{ duRect rect; ::UnionRect(&rect, this, &rect2);
		return rect; }
BOOL duRect::SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2) throw()
	{ return ::SubtractRect(this, lpRectSrc1, lpRectSrc2); }

void duRect::NormalizeRect() throw()
	{
		int nTemp;
		if (left > right)
		{
			nTemp = left;
			left = right;
			right = nTemp;
		}
		if (top > bottom)
		{
			nTemp = top;
			top = bottom;
			bottom = nTemp;
		}
	}

void duRect::InflateRect(LPCRECT lpRect) throw()
	{
		left -= lpRect->left;		top -= lpRect->top;
		right += lpRect->right;		bottom += lpRect->bottom;
	}

void duRect::InflateRect(int l, int t, int r, int b) throw()
	{
		left -= l;			top -= t;
		right += r;			bottom += b;
	}

void duRect::DeflateRect(LPCRECT lpRect) throw()
	{
		left += lpRect->left;	top += lpRect->top;
		right -= lpRect->right;	bottom -= lpRect->bottom;
	}

void duRect::DeflateRect(int l, int t, int r, int b) throw()
	{
		left += l;		top += t;
		right -= r;		bottom -= b;
	}

duRect duRect::MulDiv(int nMultiplier, int nDivisor) const throw()
	{
		return duRect(
			::MulDiv(left, nMultiplier, nDivisor),
			::MulDiv(top, nMultiplier, nDivisor),
			::MulDiv(right, nMultiplier, nDivisor),
			::MulDiv(bottom, nMultiplier, nDivisor));
	}

