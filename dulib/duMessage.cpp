//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duMessage.h
//
//  History:    Jan-05-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duMessage.h"

MessageStruct g_messageStruct[] = 
{
	{_T(""), VT_EMPTY, VT_EMPTY},
	{_T("OnSize"), VT_EMPTY, VT_EMPTY}, 					//#define DUM_RESIZE			0x0001
	{_T("ShowControl"), VT_I4, VT_EMPTY}, 					//#define DUM_SHOWCONTROL		0x0002
	{_T("MouseIn"), VT_I4, VT_I4},							//#define DUM_MOUSEIN			0x0003
	{_T("MouseLeave"), VT_EMPTY, VT_EMPTY},					//#define DUM_MOUSELEAVE		0x0004
	{_T("MouseLDown"), VT_I4, VT_I4},						//#define DUM_MOUSELDOWN		0x0005
	{_T("MouseLUp"), VT_I4, VT_I4},							//#define DUM_MOUSELUP			0x0006
	{_T("MouseRDown"), VT_I4, VT_I4},						//#define DUM_MOUSERDOWN		0x0007
	{_T("MouseRUp"), VT_I4, VT_I4},							//#define DUM_MOUSERUP			0x0008
	{_T("MouseHOver"), VT_I4, VT_I4},						//#define DUM_MOUSEHOVER		0x0009
	{_T("MouseDBClick"), VT_I4, VT_I4},						//#define DUM_MOUSEDBCLICK		0x000A
	{_T("KillFocus"), VT_DISPATCH, VT_EMPTY},				//#define DUM_KILLFOCUS			0x000B
	{_T("SetFocus"), VT_DISPATCH, VT_EMPTY},				//#define DUM_SETFOCUS			0x000C
	{_T("BtnClick"), VT_EMPTY, VT_EMPTY},					//#define DUM_BTNCLICK    		0x000D
	{_T("BtnDbClick"), VT_EMPTY, VT_EMPTY},					//#define DUM_BTNDBCLICK		0x000E
	{_T("CkClick"), VT_I4, VT_EMPTY},						//#define DUM_CKCLICK			0x000F
	{_T("ComboSelChange"), VT_I4, VT_EMPTY},				//#define DUM_COMBOSELCHANGE	0x0010
	{_T("ImpExpand"), VT_I4, VT_EMPTY},						//#define DUM_IMPEXPAND			0x0011
	{_T("LbSelChanged"), VT_I4, VT_I4},						//#define DUM_LBSELCHANGED		0x0012
	{_T("LbHover"), VT_I4, VT_I4},							//#define DUM_LBHOVER			0x0013
	{_T("LbRButtonDown"), VT_I4, VT_I4},					//#define DUM_LBRBUTTONDOWN		0x0014
	{_T("LbLButtonDbClick"), VT_I4, VT_I4},					//#define DUM_LBLBUTTONDBCLICK	0x0015
	{_T("LbExSelChanged"), VT_DISPATCH, VT_DISPATCH},		//#define DUM_LBEXSELCHANGED	0x0016
	{_T("LbExDbClick"), VT_DISPATCH, VT_EMPTY},				//#define DUM_LBEXDBCLICK		0x0017
	{_T("LbExHover"), VT_DISPATCH, VT_EMPTY},				//#define DUM_LBEXHOVER			0x0018
	{_T("LbExContentMenu"), VT_DISPATCH, VT_EMPTY},			//#define DUM_LBEXCONTENTMENU	0x0019
	{_T("MenuPopup"), VT_DISPATCH, VT_EMPTY},				//#define DUM_MENUPOPUP			0x001A
	{_T("RadioClick"), VT_I4, VT_EMPTY},					//#define DUM_RADIOCLICK		0x001B
	{_T("VScroll"), VT_I4, VT_I4},							//#define DUM_VSCROLL			0x001C
	{_T("HScroll"), VT_I4, VT_I4},							//#define DUM_HSCROLL			0x001D
	{_T("TbbSelected"), VT_I4, VT_EMPTY},					//#define DUM_TBBSELECTED		0x001E
	{_T("SliderChanged"), VT_I4, VT_I4},					//#define DUM_SLIDERCHANGED		0x0020
	{_T("LkClick"), VT_EMPTY, VT_EMPTY},					//#define DUM_LKCLICK			0x0021
	{_T("GifClick"), VT_EMPTY, VT_EMPTY},					//#define DUM_GIFCLICK			0x0022
	{_T("SpinChange"), VT_I4, VT_EMPTY},					//#define DUM_SPINCHANGE		0x0023
	{_T("SplitterMoved"), VT_EMPTY, VT_EMPTY},				//#define DUM_SPLITTERMOVED		0x0024
	{_T("ScrollContainerSize"), VT_I4, VT_I4},				//#define DUM_SCROLLCONTAINTERSIZE		0x0025
	{_T("EditChange"), VT_EMPTY, VT_EMPTY},					//#define DUM_EDITCHANGE				0x0026
	{_T("XmlStaticHrefClick"), VT_EMPTY, VT_EMPTY},         //#define DUM_XMLSTATICHREFCLICK				0x0027
	{_T("TreeItemClick"), VT_I4, VT_I4},					//#define DUM_TREEITEMCLICK           0x0028
	{_T("TreeItemDBClick"), VT_I4, VT_EMPTY},				//#define DUM_TREEITEMDBCLICK         0x0029
	{_T("TreeItemRBtnClick"), VT_I4, VT_I4},				//#define DUM_TREEITEMRBUTTONCLICK    0x002A
	{_T("TreeItemHover"), VT_I4, VT_I4},					//#define DUM_TREEITEMHOVER			0x002B
	{_T("PageChanged"), VT_I4, VT_I4},						//#define DUM_PAGECHANGED				0x002C
	{_T("HeaderSort"), VT_I4, VT_I4},						//#define DUM_HEADERSORT				0x002D
	{_T("HeaderCkClick"), VT_I4, VT_EMPTY},					//#define DUM_HEADERCKCLICK			0x002E
	{_T("TreeListItemChecked"), VT_I4, VT_I4},				//#define DUM_TREELISTITEMCHECKED		0x002F
	{_T("MenuItemInit"), VT_EMPTY, VT_EMPTY},				//#define DUM_MENUITEMINIT			0x0030
	{_T("MenuClick"), VT_EMPTY, VT_EMPTY},					//#define DUM_MENUCLICK				0x0031
	{_T("MenuSize"), VT_EMPTY, VT_EMPTY},					//#define DUM_MENUSIZE				0x0032
	{_T("MenuInit"), VT_EMPTY, VT_EMPTY},					//#define DUM_MENUINIT				0x0033};
};
