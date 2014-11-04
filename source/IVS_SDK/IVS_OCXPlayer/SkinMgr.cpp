#include "stdafx.h"
#include "SkinMgr.h"
#include "TranslucentUtility.h"


CSkinMgr::CSkinMgr(void)
    :m_strCurrentPath("")
    ,m_imagePaneBKG(nullptr)
	,m_imagePaneALARM(nullptr)
	,m_imageBarBKG(nullptr)
	,m_imageTitleBarBKG(nullptr)
	,m_imageTVWallOnlineStatusBarBKG(nullptr)
	,m_imageTVWallOfflineStatusBarBKG(nullptr)
	,m_imageWhitePaneBKG(nullptr)
    ,m_imageWhitePaneALARM(nullptr)
    ,m_imageWhiteBarBKG(nullptr)
    ,m_imageWhiteTitleBarBKG(nullptr)
    ,m_imageWhiteTVWallOnlineStatusBarBKG(nullptr)
    ,m_imageWhiteTVWallOfflineStatusBarBKG(nullptr)
    ,m_iSkinID(1)
{
    m_MapBarPngPicture.clear();
    m_MapBarBMPicture.clear();
}

CSkinMgr::~CSkinMgr(void)
{
    try
    {
        m_MapBarPngPicture.clear();
        m_MapBarBMPicture.clear();
		delete m_imagePaneBKG;
		m_imagePaneBKG = NULL;
		delete m_imagePaneALARM;
		m_imagePaneALARM = NULL;
		delete m_imageBarBKG;
		m_imageBarBKG = NULL;
		delete m_imageTitleBarBKG;
		m_imageTitleBarBKG = NULL;
		delete m_imageTVWallOnlineStatusBarBKG;
		m_imageTVWallOnlineStatusBarBKG = NULL;
		delete m_imageTVWallOfflineStatusBarBKG;
		m_imageTVWallOfflineStatusBarBKG = NULL;
		delete m_imageWhitePaneBKG;
		m_imageWhitePaneBKG = NULL;
		delete m_imageWhitePaneALARM;
		m_imageWhitePaneALARM = NULL;
		delete m_imageWhiteBarBKG;
		m_imageWhiteBarBKG = NULL;
		delete m_imageWhiteTitleBarBKG;
		m_imageWhiteTitleBarBKG = NULL;
		delete m_imageWhiteTVWallOnlineStatusBarBKG;
		m_imageWhiteTVWallOnlineStatusBarBKG = NULL;
		delete m_imageWhiteTVWallOfflineStatusBarBKG;
		m_imageWhiteTVWallOfflineStatusBarBKG = NULL;
    }
    catch (...)
    {
        //log
    }
} //lint !e1740

CSkinMgr& CSkinMgr::Instance()
{
    static CSkinMgr SkinInstance;
    return SkinInstance;
}

IVS_ULONG CSkinMgr::Init()
{
    TCHAR Buffer[BUFSIZE];
    memset(Buffer,0x0,BUFSIZE);
    if(0 ==GetModuleFileName(AfxGetInstanceHandle(),Buffer,BUFSIZE))
    {
        BP_RUN_LOG_ERR(IVS_OCX_RET_SKIN_INIT_ERROR,_T("IVS_OCX::"),_T("Get Module File Name.Failed"));
    }
    m_strCurrentPath.clear();
    (void)m_strCurrentPath.append(Buffer);
    std::string strDevCode = m_strCurrentPath.substr(0,m_strCurrentPath.rfind(_T("\\")));
    m_strCurrentPath = strDevCode;
    if(!m_SkinXML.InitXML(m_strCurrentPath))
    {
        BP_RUN_LOG_ERR(IVS_OCX_RET_SKIN_INIT_ERROR,"IVS_OCX::","SkinMgr InitXML Failed");
        return IVS_OCX_RET_SKIN_INIT_ERROR;
    }

    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::ToolBarGetPNGButtonByName( 
    const std::string &strButtonName, 
    std::string &strHoverPath, 
    std::string &strClickedPath, 
    std::string &strGrayPath, 
    std::string &strNormalPath ) const
{
    if (strButtonName.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
        return IVS_PARA_INVALID;
    }	
    strHoverPath.clear();
    strClickedPath.clear();
    strGrayPath.clear();
    strNormalPath.clear();

    TiXmlNode * pButtonNode = m_SkinXML.ToolBarGetButtonElementByName(strButtonName,"PNG");
    CHECK_POINTER(pButtonNode,IVS_FAIL);

    (void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode ,_T("Hover"),strHoverPath) ;
    (void)strHoverPath.insert(0,m_strCurrentPath);

    (void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode,_T("Clicked"),strClickedPath);
    (void)strClickedPath.insert(0,m_strCurrentPath);

    (void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode,_T("Gray"),strGrayPath);
    (void)strGrayPath.insert(0,m_strCurrentPath);

    (void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode,_T("Normal"),strNormalPath);
    (void)strNormalPath.insert(0,m_strCurrentPath);

    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::ToolBarGetBmpButtonByName( 
    const std::string &strButtonName, 
    std::string &strHoverPath, 
    std::string &strGrayPath, 
    std::string &strNormalPath ) const
{
    if (strButtonName.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
        return IVS_PARA_INVALID;
    }	
    strHoverPath.clear();
    strGrayPath.clear();
    strNormalPath.clear();

    TiXmlNode * pButtonNode = m_SkinXML.ToolBarGetButtonElementByName(strButtonName,_T("BMP"));

    (void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode ,_T("Hover"),strHoverPath) ;
    (void)strHoverPath.insert(0,m_strCurrentPath);

    (void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode,_T("Gray"),strGrayPath);
    (void)strGrayPath.insert(0,m_strCurrentPath);

    (void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode,_T("Normal"),strNormalPath);
    (void)strNormalPath.insert(0,m_strCurrentPath);

    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::TitleBarGetPNGButtonByName( 
    const std::string &strButtonName, 
    std::string &strHoverPath, 
    std::string &strClickedPath, 
    std::string &strGrayPath, 
    std::string &strNormalPath ) const
{
    if (strButtonName.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
        return IVS_PARA_INVALID;
    }
    strHoverPath.clear();
    strClickedPath.clear();
    strGrayPath.clear();
    strNormalPath.clear();

    TiXmlNode * pButtonNode = m_SkinXML.TitleBarGetButtonElementByName(strButtonName,"PNG");

    (void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode ,_T("Hover"),strHoverPath) ;
    (void)strHoverPath.insert(0,m_strCurrentPath);

    (void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode,_T("Clicked"),strClickedPath);
    (void)strClickedPath.insert(0,m_strCurrentPath);

    (void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode,_T("Gray"),strGrayPath);
    (void)strGrayPath.insert(0,m_strCurrentPath);

    (void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode,_T("Normal"),strNormalPath);
    (void)strNormalPath.insert(0,m_strCurrentPath);

    return IVS_SUCCEED;
}
IVS_ULONG CSkinMgr::DigtalPtzButtonByName( 
	const std::string &strButtonName, 
	std::string &strHoverPath, 
	std::string &strClickedPath, 
	std::string &strGrayPath, 
	std::string &strNormalPath ) const
{
	if (strButtonName.empty())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
		return IVS_PARA_INVALID;
	}
	strHoverPath.clear();
	strClickedPath.clear();
	strGrayPath.clear();
	strNormalPath.clear();

	TiXmlNode * pButtonNode = m_SkinXML.DigtalPtzGetButtonElementByName(strButtonName,"PNG");

	(void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode ,_T("Hover"),strHoverPath) ;
	(void)strHoverPath.insert(0,m_strCurrentPath);

	(void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode,_T("Clicked"),strClickedPath);
	(void)strClickedPath.insert(0,m_strCurrentPath);

	(void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode,_T("Gray"),strGrayPath);
	(void)strGrayPath.insert(0,m_strCurrentPath);

	(void)m_SkinXML.GetPathofButtonPictureStatus(pButtonNode,_T("Normal"),strNormalPath);
	(void)strNormalPath.insert(0,m_strCurrentPath);

	return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::DigtalPtzGetPNGButtonByName( 
	const std::string &strButtonName, 
	TCHAR szcImageList[TWS_BUTTON_NUM][BUFSIZE] ) const
{
	if (strButtonName.empty())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
		return IVS_PARA_INVALID;
	}

	std::string strHoverPath;
	std::string strClickedPath;
	std::string strGrayPath;
	std::string strNormalPath;

	strHoverPath.clear();
	strClickedPath.clear();
	strGrayPath.clear();
	strNormalPath.clear();

	(void)DigtalPtzButtonByName(strButtonName,strHoverPath,strClickedPath,strGrayPath,strNormalPath);

	if(!CToolsHelp::Strncpy(szcImageList[TWS_NORMAL],   MAX_PATH,strNormalPath.c_str(), strNormalPath.length()))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
	}
	if(!CToolsHelp::Strncpy(szcImageList[TWS_DISABLED], MAX_PATH,strGrayPath.c_str(),   strGrayPath.length()))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
	}
	if(!CToolsHelp::Strncpy(szcImageList[TWS_MOUSEOVER],MAX_PATH,strHoverPath.c_str(),  strHoverPath.length()))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
	}
	if(!CToolsHelp::Strncpy(szcImageList[TWS_PRESSED],  MAX_PATH,strClickedPath.c_str(),strClickedPath.length()))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
	}
	if(!CToolsHelp::Strncpy(szcImageList[TWS_SELECTED], MAX_PATH,strClickedPath.c_str(),strClickedPath.length()))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
	}

	return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::ToolBarGetPNGButtonByName( 
    const std::string &strButtonName, 
    TCHAR szcImageList[TWS_BUTTON_NUM][BUFSIZE] ) const
{
    if (strButtonName.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
        return IVS_PARA_INVALID;
    }

    std::string strHoverPath;
    std::string strClickedPath;
    std::string strGrayPath;
    std::string strNormalPath;

    (void)ToolBarGetPNGButtonByName(strButtonName,strHoverPath,strClickedPath,strGrayPath,strNormalPath);

    if(!CToolsHelp::Strncpy(szcImageList[TWS_NORMAL],   MAX_PATH , strNormalPath.c_str(),   strNormalPath.length()))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
    }
    if(!CToolsHelp::Strncpy(szcImageList[TWS_DISABLED], MAX_PATH , strGrayPath.c_str(),     strGrayPath.length()))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
    }
    if(!CToolsHelp::Strncpy(szcImageList[TWS_MOUSEOVER],MAX_PATH , strHoverPath.c_str(),    strHoverPath.length()))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
    }
    if(!CToolsHelp::Strncpy(szcImageList[TWS_PRESSED],  MAX_PATH , strClickedPath.c_str(),  strClickedPath.length()))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
    }
    if(!CToolsHelp::Strncpy(szcImageList[TWS_SELECTED], MAX_PATH , strClickedPath.c_str(),  strClickedPath.length()))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
    }

    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::TitleBarGetPNGButtonByName( 
    const std::string &strButtonName, 
    TCHAR szcImageList[TWS_BUTTON_NUM][BUFSIZE] ) const
{
    if (strButtonName.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
        return IVS_PARA_INVALID;
    }

    std::string strHoverPath;
    std::string strClickedPath;
    std::string strGrayPath;
    std::string strNormalPath;

    (void)TitleBarGetPNGButtonByName(strButtonName,strHoverPath,strClickedPath,strGrayPath,strNormalPath);

    if(!CToolsHelp::Strncpy(szcImageList[TWS_NORMAL],   MAX_PATH,strNormalPath.c_str(), strNormalPath.length()))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
    }
    if(!CToolsHelp::Strncpy(szcImageList[TWS_DISABLED], MAX_PATH,strGrayPath.c_str(),   strGrayPath.length()))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
    }
    if(!CToolsHelp::Strncpy(szcImageList[TWS_MOUSEOVER],MAX_PATH,strHoverPath.c_str(),  strHoverPath.length()))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
    }
    if(!CToolsHelp::Strncpy(szcImageList[TWS_PRESSED],  MAX_PATH,strClickedPath.c_str(),strClickedPath.length()))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
    }
    if(!CToolsHelp::Strncpy(szcImageList[TWS_SELECTED], MAX_PATH,strClickedPath.c_str(),strClickedPath.length()))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
    }

    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::SetSkin()
{
	//ÒÀ´Î¶ÁÈëºÚÆ¤·ôºÍ°×Æ¤·ôµÄÎÄ¼þ
	m_SkinXML.SetSkin(1);
	// ±³¾°Í¼Æ¬
	std::string strPicturePath;
	if(IVS_SUCCEED != VideoPaneVideoBackGround(strPicturePath))return IVS_FAIL;
	WCHAR* wstr = CTranslucentUtility::ttowc(strPicturePath.c_str());
	m_imagePaneBKG   = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
	// ±³¾°¸æ¾¯Í¼Æ¬
	strPicturePath.clear();
	if(IVS_SUCCEED != VideoPaneAlarmBackGround(strPicturePath))return IVS_FAIL;
	wstr = CTranslucentUtility::ttowc(strPicturePath.c_str());
	m_imagePaneALARM   = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
	//Bar ToolbarÍ¼Æ¬
	strPicturePath.clear();
	if(IVS_SUCCEED != ToolBarGetBackGroundPngPath(strPicturePath))return IVS_FAIL;
	wstr = CTranslucentUtility::ttowc(strPicturePath.c_str());
	m_imageBarBKG   = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
	//TitleBar ±³¾°Í¼Æ¬
	strPicturePath.clear();
	if(IVS_SUCCEED != TitleBarGetBackGroundPngPath(strPicturePath))return IVS_FAIL;
	wstr = CTranslucentUtility::ttowc(strPicturePath.c_str());
	m_imageTitleBarBKG   = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
	//µçÊÓÇ½Í¨µÀÔÚÏßºÍÀëÏß±³¾°Í¼Æ¬
	std::string strTVWallOnlinePath = "";
	std::string strTVWallOfflinePath = "";
	if(IVS_SUCCEED != StatusBarGetBackGroundPngPath(strTVWallOnlinePath,strTVWallOfflinePath)) 
	{	
		return IVS_FAIL;
	}
	wstr = CTranslucentUtility::ttowc(strTVWallOnlinePath.c_str());
	m_imageTVWallOnlineStatusBarBKG   = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
    wstr = CTranslucentUtility::ttowc(strTVWallOfflinePath.c_str());
    m_imageTVWallOfflineStatusBarBKG = Gdiplus::Image::FromFile(wstr);
    if (NULL != wstr)
    {
        delete []wstr;
        wstr = NULL;
    }
	
	//¶ÁÈë°×Æ¤·ôÎÄ¼þ
	m_SkinXML.SetSkin(2);
	// ±³¾°Í¼Æ¬
	strPicturePath.clear();
	if(IVS_SUCCEED != VideoPaneVideoBackGround(strPicturePath))return IVS_FAIL;
	wstr = CTranslucentUtility::ttowc(strPicturePath.c_str());
	m_imageWhitePaneBKG   = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
	//TitleBar ±³¾°Í¼Æ¬
	strPicturePath.clear();
	if(IVS_SUCCEED != TitleBarGetBackGroundPngPath(strPicturePath))return IVS_FAIL;
	wstr = CTranslucentUtility::ttowc(strPicturePath.c_str());
	m_imageWhiteTitleBarBKG   = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}

	// ±³¾°¸æ¾¯Í¼Æ¬
	strPicturePath.clear();
	if(IVS_SUCCEED != VideoPaneAlarmBackGround(strPicturePath))return IVS_FAIL;
	wstr = CTranslucentUtility::ttowc(strPicturePath.c_str());
	m_imageWhitePaneALARM   = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}

	//Bar ToolbarÍ¼Æ¬
	strPicturePath.clear();
	if(IVS_SUCCEED != ToolBarGetBackGroundPngPath(strPicturePath))return IVS_FAIL;
	wstr = CTranslucentUtility::ttowc(strPicturePath.c_str());
	m_imageWhiteBarBKG   = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}

	//TitleBar ±³¾°Í¼Æ¬
	strPicturePath.clear();
	if(IVS_SUCCEED != TitleBarGetBackGroundPngPath(strPicturePath))return IVS_FAIL;
	wstr = CTranslucentUtility::ttowc(strPicturePath.c_str());
	m_imageWhiteTitleBarBKG   = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}

	//µçÊÓÇ½Í¨µÀÔÚÏßºÍÀëÏß±³¾°Í¼Æ¬
	strTVWallOnlinePath = "";
	strTVWallOfflinePath = "";
	if(IVS_SUCCEED != StatusBarGetBackGroundPngPath(strTVWallOnlinePath,strTVWallOfflinePath)) 
	{	
		return IVS_FAIL;
	}
	wstr = CTranslucentUtility::ttowc(strTVWallOnlinePath.c_str());
	m_imageWhiteTVWallOnlineStatusBarBKG   = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
	wstr = CTranslucentUtility::ttowc(strTVWallOfflinePath.c_str());
	m_imageWhiteTVWallOfflineStatusBarBKG   = Gdiplus::Image::FromFile(wstr);
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}
    
    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::LoadToolBarPngPicture()const
{
    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::ToolBarGetBackGroundPngPath( std::string &strBackGroundPath )
{
    strBackGroundPath.clear();
    (void)m_SkinXML.ToolBarGetBackGround(strBackGroundPath);
    if (strBackGroundPath.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
        return IVS_FAIL;
    }

    (void)strBackGroundPath.insert(0,m_strCurrentPath);
    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::StatusBarGetBackGroundPngPath(std::string &strOnlinePath,std::string &strOfflinePath)
{
	strOnlinePath.clear();
	(void)m_SkinXML.StatusBarGetBackGround(strOnlinePath,strOfflinePath);
	if (strOnlinePath.empty())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("Get Online Path Failed."),_T("Reason:SkinXML Get Online PNG Path Failed."));
		return IVS_FAIL;
	}
	if (strOfflinePath.empty())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("Get Offline Path Failed."),_T("Reason:SkinXML Get Offline PNG Path Failed."));
		return IVS_FAIL;
	}

	(void)strOnlinePath.insert(0,m_strCurrentPath);
	(void)strOfflinePath.insert(0,m_strCurrentPath);
	return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::TitleBarGetBackGroundPngPath( std::string &strBackGroundPath )
{
    strBackGroundPath.clear();
   (void)m_SkinXML.TitleBarGetBackGround(strBackGroundPath);
    if (strBackGroundPath.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
        return IVS_PARA_INVALID;
    }

    (void)strBackGroundPath.insert(0,m_strCurrentPath);
    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::TurnCruisePaneBackGroudPngPath(std::string &strBackGroundPath)
{
	strBackGroundPath.clear();
	(void)m_SkinXML.TitleBarGetBackGround(strBackGroundPath);
	if (strBackGroundPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
		return IVS_PARA_INVALID;
	}

	(void)strBackGroundPath.insert(0,m_strCurrentPath);
	return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::ToolBarPopMenuHead(std::string &strMenuHeadPath)
{
    strMenuHeadPath.clear();
    (void)m_SkinXML.ToolBarPopMenuHead(strMenuHeadPath);
    if (strMenuHeadPath.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
        return IVS_FAIL;
    }
    (void)strMenuHeadPath.insert(0,m_strCurrentPath);
    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::ToolBarPopMenuTail(std::string &strMenuTailPath)
{
    strMenuTailPath.clear();
    (void)m_SkinXML.ToolBarPopMenuTail(strMenuTailPath);
    if (strMenuTailPath.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
        return IVS_FAIL;
    }
    (void)strMenuTailPath.insert(0,m_strCurrentPath);
    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::ToolBarPopMenuMidl(std::string &strMenuMidlPath)
{
    strMenuMidlPath.clear();
    (void)m_SkinXML.ToolBarPopMenuMidl(strMenuMidlPath);
    if (strMenuMidlPath.empty())
    {
        return IVS_FAIL;
    }
    (void)strMenuMidlPath.insert(0,m_strCurrentPath);
    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::ToolBarPopMenuHover(std::string &strMenuHoverPath)
{
    strMenuHoverPath.clear();
    (void)m_SkinXML.ToolBarPopMenuHover(strMenuHoverPath);
    if (strMenuHoverPath.empty())
    {
        return IVS_FAIL;
    }
    (void)strMenuHoverPath.insert(0,m_strCurrentPath);
    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::VideoPaneVideoBackGround( std::string &strBackGroundPath )
{
    strBackGroundPath.clear();
    (void)m_SkinXML.VideoPaneVideoBackGround(strBackGroundPath);
    if (strBackGroundPath.empty())
    {
        return IVS_FAIL;
    }
    (void)strBackGroundPath.insert(0,m_strCurrentPath);
    return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::VideoPaneAlarmBackGround( std::string &strBackGroundPath )
{
    strBackGroundPath.clear();
    (void)m_SkinXML.VideoPaneAlarmBackGround(strBackGroundPath);
    if (strBackGroundPath.empty())
    {
        return IVS_FAIL;
    }
    (void)strBackGroundPath.insert(0,m_strCurrentPath);
    return IVS_SUCCEED;
}

void CSkinMgr::LoadButtonPNGPicture(const std::string &strButtonName,
                                    std::string &strPathFocus, 
                                    std::string &strPathGray, 
                                    std::string &strPathNormal, 
                                    std::string &strPathClicked )
{
    strPathFocus.insert(0,m_strCurrentPath);
    strPathGray.insert(0,m_strCurrentPath);
    strPathNormal.insert(0,m_strCurrentPath);
    strPathClicked.insert(0,m_strCurrentPath);
	WCHAR* wstr = CTranslucentUtility::ttowc(strPathFocus.c_str());
    ptrImage pImage_Focus   (new Gdiplus::Image(wstr));
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}

	wstr = CTranslucentUtility::ttowc(strPathGray.c_str());
    ptrImage pImage_Gray    (new Gdiplus::Image(wstr));
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}

	wstr = CTranslucentUtility::ttowc(strPathNormal.c_str());
    ptrImage pImage_Normal  (new Gdiplus::Image(wstr));
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}

	wstr = CTranslucentUtility::ttowc(strPathClicked.c_str());
    ptrImage pImage_Clicked (new Gdiplus::Image(wstr));
	if (NULL != wstr)
	{
		delete []wstr;
		wstr = NULL;
	}

    std::vector<ptrImage> vectorImage;
	vectorImage.clear();
    vectorImage.push_back(pImage_Normal);
    vectorImage.push_back(pImage_Gray);
    vectorImage.push_back(pImage_Focus);
    vectorImage.push_back(pImage_Clicked);

    (void)m_MapBarPngPicture.insert(ptrImagePair(strButtonName,vectorImage));
}

IVS_ULONG CSkinMgr::TranslucentDlgGetBackGroundPngPath(std::string &strBackGroundPath)
{
	strBackGroundPath.clear();
	(void)m_SkinXML.TranslucentDialogBackGround(strBackGroundPath);
	if (strBackGroundPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
		return IVS_PARA_INVALID;
	}

	(void)strBackGroundPath.insert(0,m_strCurrentPath);
	return IVS_SUCCEED;
}


IVS_ULONG CSkinMgr::InstantReplayBackGroudPngPath(std::string &strBackGroundPath)
{
	strBackGroundPath.clear();
	(void)m_SkinXML.InstantReplayBackGround(strBackGroundPath);
	if (strBackGroundPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,_T("IVS_OCX::"),_T("Skin Manage Failed"));
		return IVS_PARA_INVALID;
	}

	(void)strBackGroundPath.insert(0,m_strCurrentPath);
	return IVS_SUCCEED;
}

IVS_ULONG CSkinMgr::SetSkinType(IVS_INT32 iSkinID)
{
	m_iSkinID = iSkinID;
	m_SkinXML.SetSkin(iSkinID);
	return IVS_SUCCEED;
}
