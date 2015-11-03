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
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include "duMessage.h"

#define MAX_NAME 128

// 控件状态
// 
#define DU_STATE_NORMAL			    0x0001
#define DU_STATE_OVER				0x0002
#define DU_STATE_PRESS			    0x0004
#define DU_STATE_FOCUS			    0x0008
#define DU_STATE_DISABLE			0x0010
#define DU_STATE_NORMAL_ALL		    0x001F

#define DU_STATE_CHECKED			0x0100
#define DU_STATE_CHECKEDOVER		0x0400
#define DU_STATE_CHECKEDPRESS		0x0800
#define DU_STATE_CHECKEDFOCUS		0x1000
#define DU_STATE_CHECKEDDISABLE	    0x2000
#define DU_STATE_CHECKED_ALL		0x3D00

#define DU_STATE_ALL				0x3D1F

#define IS_STATE_CHECKED(x) ((x) & 0x0000FF00)

// 属性类型
// 
#define DU_PROPERTY_LONG       0x01
#define DU_PROPERTY_FLOAT      0x02
#define DU_PROPERTY_DOUBLE     0x03
#define DU_PROPERTY_COLOR      0x04
#define DU_PROPERTY_STRING     0x05
#define DU_PROPERTY_BOOL       0x06
#define DU_PROPERTY_RECT       0x07
#define DU_PROPERTY_TEXT       0x08
#define DU_PROPERTY_CONTROL    0x09
#define DU_PROPERTY_STYLEGROUP 0x10
#define DU_PROPERTY_CURSOR     0x11
#define DU_PROPERTY_IMAGE      0x12
#define DU_PROPERTY_FONT       0x13
#define DU_PROPERTY_RGN        0x14
#define DU_PROPERTY_DUWINDOW   0x15

// 动画类型
//
//#define DU_ANIMATE_NONE                  0x00
//#define DU_ANIMATE_SLIDE_UP_BOTTOM       0x01
//#define DU_ANIMATE_SLIDE_BOTTOM_UP       0x02
//#define DU_ANIMATE_SLIDE_LEFT_RIGHT      0x03
//#define DU_ANIMATE_SLIDE_RIGHT_LEFT      0x04
//#define DU_ANIMATE_ALPHA                 0x05

// 样式类型
// 
#define STYLE_UNKNOWN	0
#define STYLE_IMAGE		1
#define STYLE_TEXT		2
#define STYLE_RECT		3
#define STYLE_LINE		4

// 资源类型
// 
#define DU_RES_UNKNOWN			 0
#define DU_RES_IMAGE			 1
#define DU_RES_FONT				 2
#define DU_RES_CURSOR			 3
#define DU_RES_RGN				 4
#define DU_RES_STYLEGROUP		 5

// 居中位置值
// 
#define DU_HCENTER   0x0FFFFFFF
#define DU_VCENTER   0x0FFFFFFF

// 图片样式画图类型
//
#define DU_PAINTMODE_UNKNOWN			0
#define DU_PAINTMODE_NORMAL				1
#define DU_PAINTMODE_TILE				2
#define DU_PAINTMODE_STRETCH			3
#define DU_PAINTMODE_STRETCHVERT		4
#define DU_PAINTMODE_STRETCHHORZ		5
#define DU_PAINTMODE_STRETCHPARTALL		6


// 文本样式对齐方式
//
#define DU_TEXTALIGN_UNKNOWN			0
#define DU_TEXTALIGN_TOP				1
#define DU_TEXTALIGN_BOTTOM				2
#define DU_TEXTALIGN_LEFT				3
#define DU_TEXTALIGN_RIGHT				4
#define DU_TEXTALIGN_CENTER				5

// 样式边框类型
//
#define DU_RECTBORDER_UNKNOWN			0
#define DU_RECTBORDER_SOLID				1
#define DU_RECTBORDER_DASH				2
#define DU_RECTBORDER_DOT				3
#define DU_RECTBORDER_DASHDOT			4
#define DU_RECTBORDER_DASHNOTDOT		5


// 控件属性结构体
// 
typedef unsigned short duVARIANT_TYPE;
struct duVARIANT 
{
	duVARIANT_TYPE vt;
	union {
		long     *l;
		float    *f;
		double   *d;
		BOOL	 *b;
		TCHAR	 *ptcString;
		COLORREF *clr;
	};
};
typedef struct duVARIANT duVARIANT;

struct tagPROPERTY
{
	TCHAR *duPropertyName;
	duVARIANT duVariant;
};
typedef struct tagPROPERTY duProperty;


/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

class duSize;
class duPoint;
class duRect;

/////////////////////////////////////////////////////////////////////////////
// CSize - An extent, similar to Windows SIZE structure.

class duSize : public tagSIZE
{
public:

// Constructors
	// construct an uninitialized size
	duSize() throw();
	// create from two integers
	duSize(int initCX, int initCY) throw();
	// create from another size
	duSize(SIZE initSize) throw();
	// create from a point
	duSize(POINT initPt) throw();
	// create from a DWORD: cx = LOWORD(dw) cy = HIWORD(dw)
	duSize(DWORD dwSize) throw();

// Operations
	BOOL operator==(SIZE size) const throw();
	BOOL operator!=(SIZE size) const throw();
	void operator+=(SIZE size) throw();
	void operator-=(SIZE size) throw();
	void SetSize(int CX, int CY) throw();

// Operators returning CSize values
	duSize operator+(SIZE size) const throw();
	duSize operator-(SIZE size) const throw();
	duSize operator-() const throw();

// Operators returning CPoint values
	duPoint operator+(POINT point) const throw();
	duPoint operator-(POINT point) const throw();

// Operators returning CRect values
	duRect operator+(const RECT* lpRect) const throw();
	duRect operator-(const RECT* lpRect) const throw();
};

/////////////////////////////////////////////////////////////////////////////
// CPoint - A 2-D point, similar to Windows POINT structure.

class duPoint : public tagPOINT
{
public:
// Constructors

	// create an uninitialized point
	duPoint() throw();
	// create from two integers
	duPoint(int initX, int initY) throw();
	// create from another point
	duPoint(POINT initPt) throw();
	// create from a size
	duPoint(SIZE initSize) throw();
	// create from an LPARAM: x = LOWORD(dw) y = HIWORD(dw)
	duPoint(LPARAM dwPoint) throw();


// Operations

// translate the point
	void Offset(int xOffset, int yOffset) throw();
	void Offset(POINT point) throw();
	void Offset(SIZE size) throw();
	void SetPoint(int X, int Y) throw();

	BOOL operator==(POINT point) const throw();
	BOOL operator!=(POINT point) const throw();
	void operator+=(SIZE size) throw();
	void operator-=(SIZE size) throw();
	void operator+=(POINT point) throw();
	void operator-=(POINT point) throw();

// Operators returning CPoint values
	duPoint operator+(SIZE size) const throw();
	duPoint operator-(SIZE size) const throw();
	duPoint operator-() const throw();
	duPoint operator+(POINT point) const throw();

// Operators returning CSize values
	duSize operator-(POINT point) const throw();

// Operators returning CRect values
	duRect operator+(const RECT* lpRect) const throw();
	duRect operator-(const RECT* lpRect) const throw();
};

/////////////////////////////////////////////////////////////////////////////
// CRect - A 2-D rectangle, similar to Windows RECT structure.

class duRect : public tagRECT
{
// Constructors
public:
	// uninitialized rectangle
	duRect() throw();
	// from left, top, right, and bottom
	duRect(int l, int t, int r, int b) throw();
	// copy constructor
	duRect(const RECT& srcRect) throw();
	// from a pointer to another rect
	duRect(LPCRECT lpSrcRect) throw();
	// from a point and size
	duRect(POINT point, SIZE size) throw();
	// from two points
	duRect(POINT topLeft, POINT bottomRight) throw();

// Attributes (in addition to RECT members)

	// retrieves the width
	int Width() const throw();
	// returns the height
	int Height() const throw();
	// returns the size
	duSize Size() const throw();
	// reference to the top-left point
	duPoint& TopLeft() throw();
	// reference to the bottom-right point
	duPoint& BottomRight() throw();
	// const reference to the top-left point
	const duPoint& TopLeft() const throw();
	// const reference to the bottom-right point
	const duPoint& BottomRight() const throw();
	// the geometric center point of the rectangle
	duPoint CenterPoint() const throw();
	// swap the left and right
	void SwapLeftRight() throw();
	static void SwapLeftRight(LPRECT lpRect) throw();

	// convert between CRect and LPRECT/LPCRECT (no need for &)
	operator LPRECT() throw();
	operator LPCRECT() const throw();

	// returns TRUE if rectangle has no area
	BOOL IsRectEmpty() const throw();
	// returns TRUE if rectangle is at (0,0) and has no area
	BOOL IsRectNull() const throw();
	// returns TRUE if point is within rectangle
	BOOL PtInRect(POINT point) const throw();

// Operations

	// set rectangle from left, top, right, and bottom
	void SetRect(int x1, int y1, int x2, int y2) throw();
	void SetRect(POINT topLeft, POINT bottomRight) throw();
	// empty the rectangle
	void SetRectEmpty() throw();
	// copy from another rectangle
	void CopyRect(LPCRECT lpSrcRect) throw();
	// TRUE if exactly the same as another rectangle
	BOOL EqualRect(LPCRECT lpRect) const throw();

	// Inflate rectangle's width and height by
	// x units to the left and right ends of the rectangle
	// and y units to the top and bottom.
	void InflateRect(int x, int y) throw();
	// Inflate rectangle's width and height by
	// size.cx units to the left and right ends of the rectangle
	// and size.cy units to the top and bottom.
	void InflateRect(SIZE size) throw();
	// Inflate rectangle's width and height by moving individual sides.
	// Left side is moved to the left, right side is moved to the right,
	// top is moved up and bottom is moved down.
	void InflateRect(LPCRECT lpRect) throw();
	void InflateRect(int l, int t, int r, int b) throw();

	// deflate the rectangle's width and height without
	// moving its top or left
	void DeflateRect(int x, int y) throw();
	void DeflateRect(SIZE size) throw();
	void DeflateRect(LPCRECT lpRect) throw();
	void DeflateRect(int l, int t, int r, int b) throw();

	// translate the rectangle by moving its top and left
	void OffsetRect(int x, int y) throw();
	void OffsetRect(SIZE size) throw();
	void OffsetRect(POINT point) throw();
	void NormalizeRect() throw();

	// absolute position of rectangle
	void MoveToY(int y) throw();
	void MoveToX(int x) throw();
	void MoveToXY(int x, int y) throw();
	void MoveToXY(POINT point) throw();

	// set this rectangle to intersection of two others
	BOOL IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2) throw();

	// set this rectangle to bounding union of two others
	BOOL UnionRect(LPCRECT lpRect1, LPCRECT lpRect2) throw();

	// set this rectangle to minimum of two others
	BOOL SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2) throw();

// Additional Operations
	void operator=(const RECT& srcRect) throw();
	BOOL operator==(const RECT& rect) const throw();
	BOOL operator!=(const RECT& rect) const throw();
	void operator+=(POINT point) throw();
	void operator+=(SIZE size) throw();
	void operator+=(LPCRECT lpRect) throw();
	void operator-=(POINT point) throw();
	void operator-=(SIZE size) throw();
	void operator-=(LPCRECT lpRect) throw();
	void operator&=(const RECT& rect) throw();
	void operator|=(const RECT& rect) throw();

// Operators returning CRect values
	duRect operator+(POINT point) const throw();
	duRect operator-(POINT point) const throw();
	duRect operator+(LPCRECT lpRect) const throw();
	duRect operator+(SIZE size) const throw();
	duRect operator-(SIZE size) const throw();
	duRect operator-(LPCRECT lpRect) const throw();
	duRect operator&(const RECT& rect2) const throw();
	duRect operator|(const RECT& rect2) const throw();
	duRect MulDiv(int nMultiplier, int nDivisor) const throw();
};


