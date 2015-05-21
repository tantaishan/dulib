//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       InvokeVTable.c
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include <windows.h>

DWORD WINAPI InvokeVTableFunc(void *pThis, FARPROC funcAddr, DWORD *pParam, int nParamCount)
{
	DWORD _result = 0;
	switch (nParamCount)
	{
	case 0:
		_result = funcAddr(pThis);
		break;
	case 1:
		_result = funcAddr(pThis, pParam[0]);
		break;
	case 2:
		_result =  funcAddr(pThis, pParam[0],pParam[1]);
		break;
	case 3:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2]);
		break;
	case 4:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2],pParam[3]);
		break;
	case 5:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2],pParam[3],pParam[4]);
		break;
	case 6:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2],pParam[3],pParam[4],pParam[5]);
		break;
	case 7:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2],pParam[3],pParam[4],pParam[5],pParam[6]);
		break;
	case 8:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2],pParam[3],pParam[4],pParam[5],pParam[6],pParam[7]);
		break;
	case 9:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2],pParam[3],pParam[4],pParam[5],pParam[6],pParam[7],pParam[8]);
		break;
	case 10:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2],pParam[3],pParam[4],pParam[5],pParam[6],pParam[7],pParam[8],pParam[9]);
		break;
	case 11:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2],pParam[3],pParam[4],pParam[5],pParam[6],pParam[7],pParam[8],pParam[9],pParam[10]);
		break;
	case 12:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2],pParam[3],pParam[4],pParam[5],pParam[6],pParam[7],pParam[8],pParam[9],pParam[10],pParam[11]);
		break;
	case 13:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2],pParam[3],pParam[4],pParam[5],pParam[6],pParam[7],pParam[8],pParam[9],pParam[10],pParam[11],pParam[12]);
		break;
	case 14:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2],pParam[3],pParam[4],pParam[5],pParam[6],pParam[7],pParam[8],pParam[9],pParam[10],pParam[11],pParam[12],pParam[13]);
		break;
	case 15:
		_result =  funcAddr(pThis, pParam[0],pParam[1],pParam[2],pParam[3],pParam[4],pParam[5],pParam[6],pParam[7],pParam[8],pParam[9],pParam[10],pParam[11],pParam[12],pParam[13],pParam[14]);
		break;

	default:
		_result =  -1;
		break;
	}
	return _result;
}