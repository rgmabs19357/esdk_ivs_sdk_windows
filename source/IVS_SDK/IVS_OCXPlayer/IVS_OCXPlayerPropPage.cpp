// IVS_OCXPlayerPropPage.cpp : Implementation of the CIVS_OCXPlayerPropPage property page class.

#include "stdafx.h"
#include "IVS_OCXPlayerPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CIVS_OCXPlayerPropPage, COlePropertyPage)

// Message map

BEGIN_MESSAGE_MAP(CIVS_OCXPlayerPropPage, COlePropertyPage) 
END_MESSAGE_MAP()  //lint !e751  MFC自动生成的的函数

// Initialize class factory and guid

//IMPLEMENT_OLECREATE_EX(CIVS_OCXPlayerPropPage, "IVS_OCXPLAYER.IVS_OCXPlayerPropPage.1",
//	0xeffff423, 0x1304, 0x4401, 0x95, 0x7b, 0xce, 0x24, 0x93, 0x84, 0xb3, 0x6c)

IMPLEMENT_OLECREATE_EX(CIVS_OCXPlayerPropPage, "IVS_OCXPLAYER.IVS_OCXPlayerPropPage.1",
	0x25a412e7, 0xcc35, 0x4334, 0xb2, 0xdd, 0x08, 0x60, 0x03, 0x4e, 0x65, 0xfb)

// CIVS_OCXPlayerPropPage::CIVS_OCXPlayerPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CIVS_OCXPlayerPropPage

BOOL CIVS_OCXPlayerPropPage::CIVS_OCXPlayerPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_IVS_OCXPLAYER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}

// CIVS_OCXPlayerPropPage::CIVS_OCXPlayerPropPage - Constructor

CIVS_OCXPlayerPropPage::CIVS_OCXPlayerPropPage() :
	COlePropertyPage(IDD, IDS_IVS_OCXPLAYER_PPG_CAPTION)
{
}

// CIVS_OCXPlayerPropPage::DoDataExchange - Moves data between page and properties

void CIVS_OCXPlayerPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}

// CIVS_OCXPlayerPropPage message handlers
