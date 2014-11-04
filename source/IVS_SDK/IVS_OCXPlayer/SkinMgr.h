/*****************************************************************
 filename    :    SkinMgr.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    皮肤管理类
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#pragma once

#include "WindowSkinXML.h"

#define BUFSIZE 260  //路径最大长度

enum ImageTyp
{
    Image_NORMAL = 0,
    Image_DISABLED,
    Image_MOUSEOVER,
    Image_PRESSED,
    Image_SELECTED
};

typedef std::tr1::shared_ptr<Gdiplus::Image> ptrImage;
typedef std::vector<ptrImage> ptrVector;
typedef std::map<std::string, ptrVector> ptrImageMap;
typedef std::pair<std::string, ptrVector> ptrImagePair;

#define SkinMgr CSkinMgr::Instance()

class CSkinMgr
{
private:
	CWindowSkinXML m_SkinXML;

    ptrImageMap m_MapBarPngPicture;
    ptrImageMap m_MapBarBMPicture;

	std::string m_strCurrentPath;
	CSkinMgr(void);                 //lint !e1704
public:
	static CSkinMgr& Instance();
	~CSkinMgr(void);

	IVS_ULONG Init();

    void LoadButtonPNGPicture(const std::string &strButtonName,
                              std::string &strPathFocus, 
                              std::string &strPathGray, 
                              std::string &strPathNormal, 
                              std::string &strPathClicked );

    IVS_ULONG SetSkin();
	IVS_ULONG SetSkinType( IVS_INT32 iSkinID);
	IVS_INT32 GetSkinType() const {return m_iSkinID;}

	IVS_ULONG ToolBarGetPNGButtonByName(
		const std::string &strButtonName,
		std::string &strHoverPath,
		std::string &strClickedPath,
		std::string &strGrayPath,
		std::string &strNormalPath
	) const;

	IVS_ULONG ToolBarGetBmpButtonByName( 
		const std::string &strButtonName, 
		std::string &strHoverPath, 
		std::string &strGrayPath, 
		std::string &strNormalPath ) const;

	IVS_ULONG ToolBarGetPNGButtonByName(
		const std::string &strButtonName,
		TCHAR szcImageList[TWS_BUTTON_NUM][BUFSIZE]
		) const;

	IVS_ULONG ToolBarGetBackGroundPngPath(std::string &strBackGroundPath);

	IVS_ULONG TitleBarGetBackGroundPngPath(std::string &strBackGroundPath);
	/******************************************************************
	function : StatusBarGetBackGroundPngPath
	description: 获取状态Bar的背景图片
	input : std::string & strOnlinePath
	input : std::string & strOfflinePath
	output : NA
	return : IVS_ULONG
	*******************************************************************/
	IVS_ULONG StatusBarGetBackGroundPngPath(std::string &strOnlinePath,std::string &strOfflinePath);

	IVS_ULONG TranslucentDlgGetBackGroundPngPath(std::string &strBackGroundPath);
	
	IVS_ULONG TitleBarGetPNGButtonByName( 
		const std::string &strButtonName, 
		std::string &strHoverPath, 
		std::string &strClickedPath, 
		std::string &strGrayPath, 
		std::string &strNormalPath ) const;

	IVS_ULONG TitleBarGetPNGButtonByName( 
		const std::string &strButtonName, 
		TCHAR szcImageList[TWS_BUTTON_NUM][BUFSIZE] 
	) const;

	IVS_ULONG DigtalPtzButtonByName( 
		const std::string &strButtonName, 
		std::string &strHoverPath, 
		std::string &strClickedPath, 
		std::string &strGrayPath, 
		std::string &strNormalPath ) const;

	IVS_ULONG DigtalPtzGetPNGButtonByName( 
		const std::string &strButtonName, 
		TCHAR szcImageList[TWS_BUTTON_NUM][BUFSIZE] 
	) const;

	IVS_ULONG ToolBarPopMenuHead(std::string &strMenuHeadPath);
	IVS_ULONG ToolBarPopMenuTail(std::string &strMenuTailPath);
	IVS_ULONG ToolBarPopMenuMidl(std::string &strMenuMidlPath);
	IVS_ULONG ToolBarPopMenuHover(std::string &strMenuHoverPath);

	IVS_ULONG VideoPaneVideoBackGround(std::string &strBackGroundPath);
	IVS_ULONG VideoPaneAlarmBackGround(std::string &strBackGroundPath);

    IVS_ULONG LoadToolBarPngPicture()const;

	IVS_ULONG TurnCruisePaneBackGroudPngPath(std::string &strBackGroundPath);
	IVS_ULONG InstantReplayBackGroudPngPath(std::string &strBackGroundPath);
private:
    Gdiplus::Image *m_imagePaneBKG;
    Gdiplus::Image *m_imagePaneALARM;
	Gdiplus::Image *m_imageBarBKG;
	Gdiplus::Image *m_imageTitleBarBKG;
	Gdiplus::Image *m_imageTVWallOnlineStatusBarBKG;//电视墙墙通道在线背景
	Gdiplus::Image *m_imageTVWallOfflineStatusBarBKG;//电视墙通道离线背景

	Gdiplus::Image *m_imageWhitePaneBKG;
	Gdiplus::Image *m_imageWhitePaneALARM;
	Gdiplus::Image *m_imageWhiteBarBKG;
	Gdiplus::Image *m_imageWhiteTitleBarBKG;
	Gdiplus::Image *m_imageWhiteTVWallOnlineStatusBarBKG;//电视墙墙通道在线背景
	Gdiplus::Image *m_imageWhiteTVWallOfflineStatusBarBKG;//电视墙通道离线背景

	IVS_INT32 m_iSkinID;
public:
	Gdiplus::Image *GetImagePaneBKG() const { if (m_iSkinID == 1)
		return m_imagePaneBKG;
	else
		return m_imageWhitePaneBKG;}   //lint !e1763
	Gdiplus::Image *GetImagePaneALARM() const { if (m_iSkinID == 1)
		return m_imagePaneALARM;
	else
		return m_imageWhitePaneALARM;} //lint !e1763
	Gdiplus::Image *GetImageBarBKG() const { if (m_iSkinID == 1)
		return m_imageBarBKG;
	else return m_imageWhiteBarBKG;
	} //lint !e1763
	Gdiplus::Image *GetImageTitleBarBKG() const { if (m_iSkinID == 1)
		return m_imageTitleBarBKG;
	else return m_imageWhiteTitleBarBKG;} //lint !e1763
	Gdiplus::Image *GetImageTVWallOnlineStatusBarBKG() const { if(m_iSkinID == 1)return m_imageTVWallOnlineStatusBarBKG;
	else return m_imageWhiteTVWallOnlineStatusBarBKG;} //lint !e1763
	Gdiplus::Image *GetImageTVWallOfflineStatusBarBKG() const { if(m_iSkinID == 1)return m_imageTVWallOfflineStatusBarBKG;
	else return m_imageWhiteTVWallOfflineStatusBarBKG;} //lint !e1763
};

