//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duEdit.h
//
//  History:    Jan-05-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#pragma once
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>

//
// internal 
//
#define DUMI_RESIZE				0x0F01 // wParam = duPlugin *
#define DUMI_SHOWCONTROL		0x0F02 // wParam = duPlugin *
#define DUMI_MOUSEMOVE			0x0F03
#define DUMI_MOUSEIN			0x0F04
#define DUMI_MOUSELEAVE			0x0F05
#define DUMI_MOUSELDOWN			0x0F06
#define DUMI_MOUSELUP			0x0F07
#define DUMI_MOUSERDOWN			0x0F08
#define DUMI_MOUSERUP			0x0F09
#define DUMI_MOUSEHOVER			0x0F10
#define DUMI_MOUSEDBCLICK		0x0F11
#define DUMI_MOUSEWHEEL			0x0F12
#define DUMI_CAPTURECHANGED		0x0F13
#define DUMI_TIMER				0x0F14
#define DUMI_KILLFOCUS			0x0F15
#define DUMI_SETFOCUS			0x0F16
#define DUMI_PAINT				0x0F17
#define DUMI_CREATE				0x0F18
#define DUMI_KEYDOWN			0x0F19
#define DUMI_KEYUP				0x0F1A
#define DUMI_VSCROLL   			0x0F1B
#define DUMI_HSCROLL   			0x0F1C

//
// notify
//
#define DUM_RESIZE					0x0001
#define DUM_SHOWCONTROL				0x0002
#define DUM_MOUSEIN					0x0003
#define DUM_MOUSELEAVE				0x0004
#define DUM_MOUSELDOWN				0x0005
#define DUM_MOUSELUP				0x0006
#define DUM_MOUSERDOWN				0x0007
#define DUM_MOUSERUP				0x0008
#define DUM_MOUSEHOVER				0x0009
#define DUM_MOUSEDBCLICK			0x000A
#define DUM_KILLFOCUS				0x000B
#define DUM_SETFOCUS				0x000C

#define DUM_BTNCLICK    			0x000D
#define DUM_BTNDBCLICK				0x000E

#define DUM_CKCLICK					0x000F
#define DUM_COMBOSELCHANGE			0x0010
#define DUM_IMPEXPAND				0x0011
#define DUM_LBSELCHANGED			0x0012
#define DUM_LBHOVER					0x0013
#define DUM_LBRBUTTONDOWN			0x0014
#define DUM_LBLBUTTONDBCLICK		0x0015
#define DUM_LBEXSELCHANGED			0x0016
#define DUM_LBEXDBCLICK				0x0017
#define DUM_LBEXHOVER				0x0018
#define DUM_LBEXCONTENTMENU			0x0019
#define DUM_MENUPOPUP				0x001A
#define DUM_RADIOCLICK				0x001B
#define DUM_VSCROLL					0x001C
#define DUM_HSCROLL					0x001D
#define DUM_TBBSELECTED				0x001E
#define DUM_SLIDERCHANGED			0x0020
#define DUM_LKCLICK					0x0021
#define DUM_GIFCLICK				0x0022
#define DUM_SPINCHANGE				0x0023
#define DUM_SPLITTERMOVED			0x0024
#define DUM_SCROLLCONTAINTERSIZE	0x0025
#define DUM_EDITCHANGE				0x0026
#define DUM_XMLSTATICHREFCLICK	    0x0027
#define DUM_TREEITEMCLICK           0x0028
#define DUM_TREEITEMDBCLICK         0x0029
#define DUM_TREEITEMRBUTTONCLICK    0x002A
#define DUM_TREEITEMHOVER			0x002B
#define DUM_PAGECHANGED				0x002C
#define DUM_HEADERSORT				0x002D
#define DUM_HEADERCKCLICK			0x002E
#define DUM_TREELISTITEMCHECKED		0x002F
#define DUM_MENUITEMINIT			0x0030
#define DUM_MENUCLICK				0x0031
#define DUM_MENUSIZE				0x0032
#define DUM_MENUINIT				0x0033
#define DUM_REMOTECTRLSCROLL		0x0034

typedef struct _tagMessageStruct
{
	TCHAR szMessage[128];
	SHORT wParamType;
	SHORT lParamType;
}MessageStruct;


#define WM_DIRECTUI_NOTIFY WM_USER + 0x188

typedef struct duNotify
{
	UINT    uMsg;
	WPARAM  wParam;
	LPARAM  lParam;
}duNotify;
