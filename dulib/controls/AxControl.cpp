#include "stdafx.h"
#include "AxControl.h"
#include <crtdbg.h>


class CControlContainer : public IOleClientSite,
						  public IOleInPlaceSite
{
public:
	CControlContainer(HWND hWnd)
	{
		m_hWnd = hWnd;
		m_refCnt = 1;
	}
	
	~CControlContainer()
	{
		_ASSERT(m_refCnt == 0);
	}
	
public:
	HWND m_hWnd;
	ULONG m_refCnt;
	
private:
	CControlContainer() {}
	
public:
	// IUnknown
	virtual ULONG WINAPI AddRef() 
	{ 
		return ++m_refCnt; 
	}
	
	virtual ULONG WINAPI Release()
	{
		if (--m_refCnt == 0)
		{
			delete this;
			return 0;
		}
		return m_refCnt;
	}
	
	virtual HRESULT WINAPI QueryInterface(REFIID riid, void **ppvObject)
	{
		*ppvObject = NULL;
		if ( IID_IUnknown == riid)
			*ppvObject = (IUnknown*)(IOleInPlaceSite*)this;
		else if (IID_IOleClientSite == riid)
			*ppvObject = (IOleClientSite*)this;
		else if (IID_IOleWindow == riid)
			*ppvObject = (IOleWindow*)this;
		else if (IID_IOleInPlaceSite == riid)
			*ppvObject = (IOleInPlaceSite*)this;
		else
			return E_NOINTERFACE;

		AddRef();
		return S_OK;
	}
	
	// IOleControlSite
	virtual HRESULT WINAPI SaveObject() { return E_NOTIMPL; }
	virtual HRESULT WINAPI GetMoniker(DWORD,DWORD,IMoniker**) { return E_NOTIMPL; }
	virtual HRESULT WINAPI GetContainer(IOleContainer **ppContainer) { return E_NOINTERFACE; }
	virtual HRESULT WINAPI ShowObject()	{ return S_OK; }
	virtual HRESULT WINAPI OnShowWindow(BOOL bShow)	{ return S_OK; }
	virtual HRESULT WINAPI RequestNewObjectLayout()	{ return E_NOTIMPL; }
	
	// IOleWindow
	virtual HRESULT WINAPI GetWindow(HWND * pHwnd) { *pHwnd = m_hWnd; return S_OK; }
	virtual HRESULT WINAPI ContextSensitiveHelp(BOOL bEnterMode) { return S_OK; }
	
	//IOleInPlaceSite
	virtual HRESULT WINAPI CanInPlaceActivate() { return S_OK; }
	virtual HRESULT WINAPI OnInPlaceActivate() { return S_OK; }
	virtual HRESULT WINAPI OnUIActivate() { return S_OK; }
	virtual HRESULT WINAPI GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo) { return E_NOTIMPL; }
	virtual HRESULT WINAPI Scroll(SIZE scrollSize) { return S_OK; }
	virtual HRESULT WINAPI OnUIDeactivate(BOOL bUndoable) { return S_OK; }
	virtual HRESULT WINAPI OnInPlaceDeactivate() { return S_OK; }
	virtual HRESULT WINAPI DiscardUndoState() { return S_OK; }
	virtual HRESULT WINAPI DeactivateAndUndo() { return S_OK; }
	virtual HRESULT WINAPI OnPosRectChange(LPCRECT lprcPosRect) { return S_OK; }
};


HRESULT WINAPI CreateAxControl(HWND hWnd, const wchar_t *ProgId, IUnknown **ppControl, IUnknown **ppContainer)
{
	HRESULT hr;
	CLSID cls;
	IOleObject * pObject = NULL;
	CControlContainer * pContainer = NULL;
	hr = CLSIDFromProgID(ProgId,&cls);
	if (FAILED(hr)) 
		goto exit1;
	
	hr = CoCreateInstance(cls,NULL,CLSCTX_INPROC_SERVER|CLSCTX_INPROC_HANDLER|CLSCTX_LOCAL_SERVER,IID_IOleObject,(void**)&pObject);
	if (FAILED(hr)) 
		goto exit1;

	pContainer = new CControlContainer(hWnd);
	hr = pObject->SetClientSite(pContainer);
	if (FAILED(hr)) 
		goto exit1;

	hr = pObject->DoVerb(OLEIVERB_SHOW, 0, pContainer, 0, hWnd, 0);
	if (FAILED(hr)) 
		goto exit1;

    pObject->QueryInterface(IID_IUnknown, (void**)ppControl);
	pContainer->QueryInterface(IID_IUnknown, (void**)ppContainer);
	hr = S_OK;
	
exit1:
	if (pObject) 
		pObject->Release();
	if (pContainer) 
		pContainer->Release();
	
	return hr;
}
