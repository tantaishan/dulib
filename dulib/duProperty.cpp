//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duProperty.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duProperty.h"

duPropertyGroup::duPropertyGroup(TiXmlElement *pElement) :
		m_pXmlElement(pElement)
{
	m_duPropertyList.clear();
}

duPropertyGroup::~duPropertyGroup()
{
	duPropertyList::iterator iter;
	for (iter = m_duPropertyList.begin(); iter != m_duPropertyList.end(); iter++)
	{
		duProperty *p = *iter;

		SAFE_DELETE_ARRAY(p->duPropertyName);
		SAFE_DELETE(p);
	}
	m_duPropertyList.clear();
}

/*++ duPropertyGroup::AddProperty

Routine Description:

	添加属性.

Arguments:

	strName    - 属性名称.
	vt         - 属性类型.
	pProperty  - 属性变量.

Return Value:
	
	如果成功,返回真(TRUE).如果假,返回假(FALSE).

Revision History:

	Jan-17-95 Davepl  Created

--*/
BOOL duPropertyGroup::AddProperty(const TCHAR *strName, duVARIANT_TYPE vt, void *pProperty)
{
	if (strName == NULL || pProperty == NULL) return FALSE;

	duProperty *pro = new duProperty;

	size_t len = _tcslen(strName);
	pro->duPropertyName = new TCHAR[len+1];
	_tcsncpy(pro->duPropertyName, strName, len+1);
	pro->duVariant.vt = vt;
	switch(vt)
	{
	case DU_PROPERTY_RGN:
	case DU_PROPERTY_FONT:
	case DU_PROPERTY_CURSOR:
	case DU_PROPERTY_TEXT:
	case DU_PROPERTY_CONTROL:
	case DU_PROPERTY_IMAGE:
	case DU_PROPERTY_STYLEGROUP:
	case DU_PROPERTY_STRING:
	case DU_PROPERTY_DUWINDOW:
		pro->duVariant.ptcString = (TCHAR *)pProperty;
		break;

	case DU_PROPERTY_LONG:
		pro->duVariant.l = (long *)pProperty;
		break;

	case DU_PROPERTY_FLOAT:
		pro->duVariant.f	= (float *)pProperty;
		break;

	case DU_PROPERTY_DOUBLE:
		pro->duVariant.d = (double *)pProperty;
		break;

	case DU_PROPERTY_COLOR:
		pro->duVariant.clr = (COLORREF *)pProperty;
		break;

	case DU_PROPERTY_BOOL:
		pro->duVariant.b = (BOOL *)pProperty;
		break;
	default:
		delete pro;
		return FALSE;
	}
	
	m_duPropertyList.push_back(pro);
	
	return TRUE;
}

/*++ duPropertyGroup::AccessXml

Routine Description:

	读取XML属性到变量.

Arguments:

Return Value:
	
	如果成功,返回真(TRUE).如果失败,返回假(FALSE).

Revision History:

	Jan-17-95 Davepl  Created

--*/
BOOL duPropertyGroup::AccessXml()
{
	if (m_pXmlElement == NULL) return FALSE;

	duPropertyList::iterator iter;
	for (iter = m_duPropertyList.begin(); iter != m_duPropertyList.end(); iter++)
	{
		duProperty *pro = *iter;
		dustring val = ReadXmlAttributeText(m_pXmlElement, pro->duPropertyName);
		if (val.empty()) continue;
		switch(pro->duVariant.vt)
		{
		case DU_PROPERTY_RGN:
		case DU_PROPERTY_FONT:
		case DU_PROPERTY_CURSOR:
		case DU_PROPERTY_TEXT:
		case DU_PROPERTY_CONTROL:
		case DU_PROPERTY_IMAGE:
		case DU_PROPERTY_STYLEGROUP:
		case DU_PROPERTY_STRING:
		case DU_PROPERTY_DUWINDOW:
			{
				_tcsncpy( pro->duVariant.ptcString, val.c_str(), val.length()+1);
			}
			break;

		case DU_PROPERTY_LONG:
				*(pro->duVariant.l) = _tstoi(val.c_str());
			break;

		case DU_PROPERTY_FLOAT:
				*(pro->duVariant.f)	= (float)_tstof(val.c_str());
			break;

		case DU_PROPERTY_DOUBLE:
				*(pro->duVariant.d) = _tstof(val.c_str());
			break;

		case DU_PROPERTY_COLOR:
			{
				int r=0,g=0,b=0;
				_stscanf(val.c_str(), _T("RGB(%d,%d,%d)"), &r, &g, &b);
				*(pro->duVariant.clr) = RGB(r,g,b);
			}
			break;
		case DU_PROPERTY_BOOL:
			{
				if (_T("true") == val) *(pro->duVariant.b) = TRUE;
				else if (_T("false") == val) *(pro->duVariant.b) = FALSE;
			}
			break;
		}
	}

	return TRUE;
}

/*++ duPropertyGroup::GetPropertyCount

Routine Description:

	获得属性个数.

Arguments:

Return Value:
	
	返回属性个数

Revision History:

	Jan-17-95 Davepl  Created

--*/
int duPropertyGroup::GetPropertyCount()
{
	return (int)m_duPropertyList.size();
}

/*++ duPropertyGroup::GetPropertyName

Routine Description:

	获得属性名称.

Arguments:

	nNameIndex        - 属性索引

Return Value:
	
	如果成功,属性名称字符串.如果失败,返回空(NULL).

Revision History:

	Jan-17-95 Davepl  Created

--*/
const TCHAR * duPropertyGroup::GetPropertyName(int nNameIndex)
{
	int propCount = GetPropertyCount();
	if (nNameIndex >= 0 && nNameIndex < propCount)
	{
		duPropertyList::iterator Iter = m_duPropertyList.begin();
		int i = 0;
		for (; Iter != m_duPropertyList.end(); Iter++)
		{
			if (i == nNameIndex)
			{
				duProperty *pProp = *Iter;
				if (pProp)
				{
					return pProp->duPropertyName;
				}
			}
			i++;
		}
	}

	return NULL;
}
