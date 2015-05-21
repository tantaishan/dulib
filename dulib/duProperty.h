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
#pragma once
#include "duUtility.h"
#include <list>
#include <tchar.h>
using namespace std;

typedef list<duProperty *> duPropertyList;

// duPropertyGroup
//
// 控件属性集合

class duPropertyGroup
{
public:
	duPropertyGroup(TiXmlElement *pElement);
	virtual ~duPropertyGroup();

public:
	BOOL SetXML(TiXmlElement *pElement) { m_pXmlElement = pElement; }
	TiXmlElement *GetXML() { return m_pXmlElement; }

	BOOL AddProperty(const TCHAR *strName, duVARIANT_TYPE vt, void *pProperty);
	BOOL AccessXml();

	int GetPropertyCount();
	const TCHAR * GetPropertyName(int nNameIndex);

private:
	duPropertyList m_duPropertyList;
	TiXmlElement *m_pXmlElement;
};
