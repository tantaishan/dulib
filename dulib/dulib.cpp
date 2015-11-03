// dulib.cpp : Defines the entry point for the DLL application.
//
#include "duUtility.h"
#include "dulib.h"
#include "duWindow.h"
#include "duWindowManager.h"
#include "duCtrlManager.h"

GdiplusStartupInput g_gdiplusStartupInput;
ULONG_PTR g_gdiplusToken;
set<duPlugin *> *g_setValid;
HINSTANCE g_hInstance;
ATOM RegisterMenuClass();

extern "C"
{

	duWindowManager *LoadStyleZipMemory(BYTE *pData, int nSize)
	{
		if (!pData || nSize <= 0)
			return NULL;

		::CoInitialize(NULL);

		RegisterMenuClass();

		void *p = NULL;
		duWindowManager *pWinManager = new duWindowManager;
		if (pWinManager)
		{
			if (pWinManager->OpenSkinMemory(pData, nSize))
				return pWinManager;
		}

		return pWinManager;
	}

	duWindowManager *LoadStyleZipW(LPCWSTR lpszZip, LPCSTR lpszZipPassword)
	{
		if (!lpszZip || !*lpszZip)
			return NULL;

		::CoInitialize(NULL);

		RegisterMenuClass();

		duWindowManager *pWinManager = new duWindowManager;
		if (pWinManager)
		{
			if (pWinManager->OpenSkinZip(lpszZip, lpszZipPassword))
				return pWinManager;
		}

		return pWinManager;
	}

	duWindowManager *LoadResourceStyleZipW(LPCWSTR lpszZipData,ULONG nResZipSize, LPCSTR lpszZipPassword)
	{
		::CoInitialize(NULL);

		RegisterMenuClass();

		duWindowManager *pWinManager = new duWindowManager;
		if (pWinManager)
		{
			if (pWinManager->OpenSkinZip(lpszZipData, lpszZipPassword))
				return pWinManager;
		}

		return pWinManager;
	}

	duWindowManager* LoadStyleZipA(LPCSTR lpszZip, LPCSTR lpszZipPassword)
	{
		if (!lpszZip || !*lpszZip)
			return NULL;

		RegisterMenuClass();

		int len = (int)strlen(lpszZip);
		wchar_t *lpwZip = new wchar_t[len+1];
		ZeroMemory(lpwZip, (len+1) * sizeof(wchar_t));

		::MultiByteToWideChar(CP_ACP, NULL, lpszZip, len, lpwZip, len+1);
		duWindowManager *pWinManager = LoadStyleZipW(lpwZip, lpszZipPassword);
		delete [] lpwZip;
		return pWinManager;
	}


	duWindowManager *LoadStyleW(const wchar_t *lpszXml)
	{
		if (!lpszXml || !*lpszXml)
			return NULL;

		::CoInitialize(NULL);

		RegisterMenuClass();

		duWindowManager *pWinManager = new duWindowManager;
		if (pWinManager)
		{
			if (pWinManager->OpenSkin(lpszXml))
				return pWinManager;
		}

		return pWinManager;
	}

	duWindowManager *LoadStyleA(const char *lpszXml)
	{
		if (!lpszXml || !*lpszXml)
			return NULL;

		RegisterMenuClass();

		int len = (int)strlen(lpszXml);
		wchar_t *lpwXml = new wchar_t[len+1];
		ZeroMemory(lpwXml, (len+1) * sizeof(wchar_t));
		::MultiByteToWideChar(CP_ACP, NULL, lpszXml, len, lpwXml, len+1);
		duWindowManager *pWinManager = LoadStyleW(lpwXml);
		delete [] lpwXml;
		return pWinManager;
	}

	void Dulib_Init()
	{
		g_hInstance = GetModuleHandle(NULL);

		InitCommonControls();
		g_setValid = new set<duPlugin *>;
		g_setValid->clear();
		GdiplusStartup(&g_gdiplusToken, &g_gdiplusStartupInput, NULL);
	}

	void Dulib_Release()
	{
		if (g_setValid)
		{
			g_setValid->clear();
			delete g_setValid;
			g_setValid = NULL;
		}

		GdiplusShutdown(g_gdiplusToken);
	}

	BOOL WindowManager_Attach(duWindowManager *pWinManager, HWND hWnd, LPCTSTR lpWindowName)
	{
		if (pWinManager == NULL || !::IsWindow(hWnd) || lpWindowName == NULL)
			return FALSE;

		return pWinManager->Attach(hWnd, lpWindowName);
	}

	BOOL ReleaseStyle(duWindowManager *pWinManager)
	{
		if (pWinManager == NULL)
			return FALSE;

		duWindowManager *pManager = (duWindowManager *)pWinManager;
		pManager->FinalRelease();
		return TRUE;
	}

	BOOL MatchString(LPCTSTR lpszPat, LPCTSTR lpszStr)
	{
		if (lpszPat == NULL || lpszStr == NULL)
			return FALSE;

		while(*lpszStr && *lpszPat) 
		{
			if(*lpszPat == '?')
			{
				if(MatchString(lpszPat+1, lpszStr)) 
					return TRUE;

				lpszStr++;
				lpszPat++;
			}
			else if(*lpszPat == '*')
			{
				while(*lpszPat == '*' || *lpszPat == '?') 
					lpszPat++;

				if(!*lpszPat)
					return TRUE;

				while(*lpszStr)
				{
					if(MatchString(lpszPat, lpszStr))
						return TRUE;

					lpszStr++;
				}
				return FALSE;
			}
			else
			{
				if(*lpszPat != *lpszStr)
					return FALSE;

				lpszStr++;
				lpszPat++;
			}
		}
		if(*lpszStr!=0)
			return FALSE;

		while(*lpszPat == '*' || *lpszPat == '?') 
			lpszPat++;

		return !*lpszPat;
}

}// extern "C"
