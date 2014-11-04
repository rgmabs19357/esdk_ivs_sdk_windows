// IVS_OCXPlayer.cpp : Implementation of CIVS_OCXPlayerApp and DLL registration.

#include "stdafx.h"
#include "IVS_OCXPlayer.h"
#include <comcat.h>
#include <objsafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CIVS_OCXPlayerApp theApp;
//lint -e526
const GUID CDECL _tlid = { 0x72B4D993, 0x872D, 0x422B, { 0xAF, 0xAB, 0xBE, 0x38, 0x92, 0x55, 0xCF, 0x81 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;
const CATID CLSID_SafeItem ={0xB548F3C7,0x2135,0x4242,{0x92,0x0B,0xA7,0xBD,0xEE,0x6D,0x2B,0xA3}};
// CIVS_OCXPlayerApp::InitInstance - DLL initialization

BOOL CIVS_OCXPlayerApp::InitInstance()
{
	return COleControlModule::InitInstance();
}

// CIVS_OCXPlayerApp::ExitInstance - DLL termination

int CIVS_OCXPlayerApp::ExitInstance()
{
    return COleControlModule::ExitInstance();
}
//系统文件不修改
HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription)//lint !e715
{
    USES_CONVERSION;
    
    ICatRegister* pcr = NULL;
    HRESULT hr = S_OK ;
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
                          NULL, 
                          CLSCTX_INPROC_SERVER, 
                          IID_ICatRegister, 
                          (void**)&pcr);
    // Make sure the HKCR\Component Categories\{..catid...} key is registered.
    if (FAILED(hr))
    {
        return hr;
    }
    // Make sure the HKCR/Component Categories/{..catid}    
    // // key is registered.    
    CATEGORYINFO catinfo;
    catinfo.catid = catid;
    catinfo.lcid = 0x0409 ;     // english    

    // Make sure the provided description is not too long，
    // Only copy the first 127 characters if it is.
    int len = static_cast<int>(wcslen(catDescription)); 

    if (len > 127)
    {
        len = 127;
    }

    wcsncpy_s(catinfo.szDescription, catDescription, len);
    catinfo.szDescription[len] = '\0';
    hr = pcr->RegisterCategories(1, &catinfo);
    pcr->Release();
    return hr;
}//lint !e1746

// HRESULT RegisterCLSIDInCategory -  Register your component categories information
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
    // Register your component categories information.
    ICatRegister* pcr = NULL;
    HRESULT hr = S_OK ;   
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
    NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr); 
    if (SUCCEEDED(hr)) 
    { 
        // Register this category as being "implemented" by the class.
        CATID rgcatid[1];
        rgcatid[0] = catid;
        hr = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
    }

    if (pcr != NULL)
    {
        pcr->Release();
    }
    return hr;
} //lint !e1746  MFC自动生成的的函数

// HRESULT UnRegisterCLSIDInCategory -  Remove entries from the registry
HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
    ICatRegister *pcr = NULL ; 
    HRESULT hr = S_OK ;   
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,   
        NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);  

    if (SUCCEEDED(hr))
    {
        // Unregister this category as being "implemented" by the class.
        CATID rgcatid[1];
        rgcatid[0] = catid;
        hr = pcr->UnRegisterClassImplCategories(clsid, 1, rgcatid);
    }

    if (pcr != NULL)
    {
        pcr->Release();
    }

    return hr;
} //lint !e1746  MFC自动生成的的函数
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    HRESULT hr;

    AFX_MANAGE_STATE(_afxModuleAddrThis);

    if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
    {
        return ResultFromScode(SELFREG_E_TYPELIB);
    }

    if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
    {
        return ResultFromScode(SELFREG_E_CLASS);
    }
    hr = CreateComponentCategory(CATID_SafeForInitializing, L"Controls safely initializable from persistent data!");  //lint !e1776  MFC自动生成的的函数
    if (FAILED(hr))
    {
        return hr;
    }
    hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing); 
    if (FAILED(hr))      
    {
        return hr;   
    }
    hr = CreateComponentCategory(CATID_SafeForScripting, L"Controls safely  scriptable!");   //lint !e1776  MFC自动生成的的函数
    if (FAILED(hr))
    {
        return hr;   
    }
    hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);  
    if (FAILED(hr))     
    {
        return hr;    
    }
    return NOERROR;
}


// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    HRESULT hr;    
    AFX_MANAGE_STATE(_afxModuleAddrThis);   
    // Remove entries from the registry. 
    hr=UnRegisterCLSIDInCategory(CLSID_SafeItem,CATID_SafeForInitializing); 
    if (FAILED(hr)) 
    {
        return hr;
    }

    hr=UnRegisterCLSIDInCategory(CLSID_SafeItem,CATID_SafeForScripting);
    if (FAILED(hr))
    {      
        return hr; 
    }
    if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
    {
        return ResultFromScode(SELFREG_E_TYPELIB);
    }
    if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
    { 
        return ResultFromScode(SELFREG_E_CLASS);
    }
    return NOERROR;
}

