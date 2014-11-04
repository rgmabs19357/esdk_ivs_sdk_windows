/********************************************************************
filename    :    Favorite.h
author      :    
created     :    2012/11/26
description :    设备收藏和场景实现
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/13 初始版本
*********************************************************************/
#ifndef _IVS_FAVORITE_H_
#define _IVS_FAVORITE_H_

#include "hwsdk.h"
#include "SDKDef.h"

USING_NAMESPACE_BP


class CFavorite
{
public:
	CFavorite(void);
	virtual ~CFavorite(void);
	static const IVS_CHAR* NewGUID();
    
    /******************************************************************
     function   : AddFavoriteDevice
     description: 添加设备收藏
     input      : const IVS_CHAR * pDeviceFavoriteInfo
     input      : std::string & strDeviceGUID
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 AddFavoriteDevice(const IVS_CHAR* pDeviceFavoriteInfo, std::string& strDeviceGUID);
    
    /******************************************************************
     function   : MoveFavoriteDevice
     description: 移动设备收藏
     input      : const IVS_CHAR * pDeviceFavoriteInfo
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 MoveFavoriteDevice(const IVS_CHAR* pDeviceFavoriteInfo);
    
    /******************************************************************
     function   : DelFavoriteDevice
     description: 删除设备收藏
     input      : const IVS_CHAR * pDeviceFavoriteInfo
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 DelFavoriteDevice(const IVS_CHAR* pDeviceFavoriteInfo);
    
    /******************************************************************
     function   : DelFavoriteFolder
     description: 删除收藏夹
     input      : const IVS_CHAR * pDeviceFavoriteInfo
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 DelFavoriteFolder(const IVS_CHAR* pDeviceFavoriteInfo);
    
    /******************************************************************
     function   : AddFavoriteFolder
     description: 新增收藏夹
     input      : const IVS_CHAR * pDeviceFavoriteInfo
     input      : std::string & strFolderGUID
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 AddFavoriteFolder(const IVS_CHAR* pDeviceFavoriteInfo, std::string& strFolderGUID);
    
    /******************************************************************
     function   : ModFavoriteFolder
     description: 修改收藏夹
     input      : const IVS_CHAR * pDeviceFavoriteInfo
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 ModFavoriteFolder(const IVS_CHAR* pDeviceFavoriteInfo);
    
    /******************************************************************
     function   : MoveFavoriteFolder
     description: 移动收藏夹
     input      : const IVS_CHAR * pDeviceFavoriteInfo
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 MoveFavoriteFolder(const IVS_CHAR* pDeviceFavoriteInfo);
    
    /******************************************************************
     function   : GetFavoriteDevice
     description: 查询设备收藏
     input      : CXml & xmlRsp
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetFavoriteDevice(CXml& xmlRsp);
    
    /******************************************************************
     function   : AddFavoriteDeviceRspXML
     description: 拼装收藏设备成功响应xml
     input      : const std::string & strDeviceGUID
     input      : CXml & xml
     output     : NA
     return     : void
    *******************************************************************/
    static void AddFavoriteDeviceRspXML(const std::string& strDeviceGUID, CXml& xml);
    
    /******************************************************************
     function   : AddFavoriteFolderRspXML
     description: 拼装添加收藏夹成功响应xml
     input      : const std::string & strFolderGUID
     input      : CXml & xml
     output     : NA
     return     : void
    *******************************************************************/
    static void AddFavoriteFolderRspXML(const std::string& strFolderGUID, CXml& xml);

	/******************************************************************
	 function   : AddScenarioFavorite
	 description: 新增场景
	 input      : const IVS_CHAR * pScenarioXml
	 input      : const std::string & strID
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 AddScenarioFavorite(const IVS_CHAR* pScenarioXml, const std::string& strID);

	/******************************************************************
	 function   : ModScenarioFavorite
	 description: 修改场景
	 input      : const IVS_CHAR * pScenarioXml
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 ModScenarioFavorite(const IVS_CHAR* pScenarioXml);

	//修改电视墙场景
	static IVS_INT32 SetTVwallScenarioFavorite(const IVS_CHAR* pScenarioXml);
	
	/******************************************************************
	 function   : DelScenarioFavorite
	 description: 删除场景
	 input      : const IVS_CHAR * pScenarioID
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 DelScenarioFavorite(const IVS_CHAR* pScenarioID);
	
	/******************************************************************
	 function   : GetScenarioFavoriteList
	 description: 获取场景列表
	 input      : std::string & strScenarioInfoList
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 GetScenarioFavoriteList(std::string& strScenarioInfoList);
	
	/******************************************************************
	 function   : GetScenarioFavorite
	 description: 获取场景详情
	 input      : const IVS_CHAR * pScenarioID
	 input      : std::string & strScenarioInfo
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 GetScenarioFavorite(const IVS_CHAR* pScenarioID, std::string& strScenarioInfo);

	/******************************************************************
	 function   : GetScenarioFavorite
	 description: 获取电视墙场景详情
	 input      : const IVS_CHAR * pScenarioID
	 input      : std::string & strScenarioInfo
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 GetTVWallScenarioFavorite(const IVS_CHAR* pScenarioID, std::string& strScenarioInfo);
	
	/******************************************************************
	 function   : GetTempScenario
	 description: 获取临时场景
	 input      : void
	 output     : NA
	 return     : std::string
	*******************************************************************/
	static std::string GetTempScenario(const IVS_CHAR* pScenarioID);

	//获取电视墙临时场景
	static std::string GetTVWallTempScenario(const std::string& strScenarioID);

	// 拼接新增场景响应
	static void BulidAddScenarioInfoResult(IVS_INT32 iResultCode, const std::string& strScenarioID, std::string& strResult);
	// 拼接获取场景响应
	static void BulidGetScenarioInfoResult(IVS_INT32 iResultCode, const std::string& strScenarioInfo, std::string& strResult);
	// 拼接获取场景列表响应
	static void BulidGetScenarioInfoListResult(IVS_INT32 iResultCode, const std::string& strScenarioInfoList, std::string& strResult);

	/******************************************************************
	 function   : AddScenarioFavorite
	 description: 新增轮巡资源
	 input      : const IVS_CHAR * pSwitchInfo
	 input      : const std::string & strSwitchID
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 AddCameraSwitch(const IVS_CHAR* pSwitchInfo, const std::string& strSwitchID);
	
	/******************************************************************
	 function   : DelCameraSwitch
	 description: 删除轮巡资源
	 input      : const IVS_CHAR * pScenarioID
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 DelCameraSwitch(const IVS_CHAR* pCameraSwitchID);

    /******************************************************************
	 function   : ModCameraSwitch
	 description: 删除轮巡资源
	 input      : const IVS_CHAR * pScenarioID
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 ModCameraSwitch(const IVS_CHAR* pSwitchInfo);
	
	/******************************************************************
	 function   : GetCameraSwitchInfoList
	 description: 获取轮巡资源列表
	 input      : std::string & strCameraSwitchInfoList
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 GetCameraSwitchInfoList(std::string& strCameraSwitchInfoList);
	
	/******************************************************************
	 function   : GetCameraSwitchInfo
	 description: 获取轮巡资源详情
	 input      : const IVS_CHAR * pSwitchInfo
	 input      : std::string & strCameraSwitchInfo
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	static IVS_INT32 GetCameraSwitchInfo(const IVS_CHAR* pCameraSwitchID, std::string& strCameraSwitchInfo);

	// 拼接新增轮巡资源响应
	static void BulidAddCameraSwitchResult(IVS_INT32 iResultCode, const std::string& strCameraSwitchID, std::string& strResult);
	// 拼接查询轮巡资源详情响应
	static void BulidGetCameraSwitchInfoResult(IVS_INT32 iResultCode, const std::string& strCameraSwitchInfo, std::string& strResult);
	// 拼接获取轮巡资源列表响应
	static void BulidGetCameraSwitchListResult(IVS_INT32 iResultCode, const std::string& strCameraSwitchInfoList, std::string& strResult);

	// 添加轮巡计划
	static IVS_INT32 AddCameraSwitchPlan(const IVS_CHAR* pSwitchPlanInfo);

	// 删除轮巡计划
	static IVS_INT32 DelCameraSwitchPlan(const IVS_CHAR* pCameraSwitchID);

	// 获取轮巡计划
	static IVS_INT32 GetCameraSwitchPlan(const IVS_CHAR* pCameraSwitchID, std::string& strCameraSwitchPlan);

	// 获取轮巡计划列表
	static IVS_INT32 GetCameraSwitchPlanList(std::string& strCameraSwitchPlanList);

	// 添加电视墙轮训计划
	static IVS_INT32 AddCameraSwitchPlanTVWall(const IVS_CHAR* pSwitchPlanInfo);

	// 删除电视墙轮巡计划
	static IVS_INT32 DelCameraSwitchPlanTVWall(ULONG ulTVWallID,const IVS_CHAR* pCameraSwitchID);
	
	// 获取电视墙轮巡计划
	static IVS_INT32 GetCameraSwitchPlanTVWall(ULONG ulTVWallID, const IVS_CHAR* pCameraSwitchID, std::string& strTVWallCameraSwitchPlan);

	// 获取电视墙轮巡计划列表
	static IVS_INT32 GetCameraSwitchPlanListTVWall(std::string& strTVWallCameraSwitchPlanList);

	// 获取xml中的轮巡资源ID
	static IVS_INT32 GetCameraSwitchID(const IVS_CHAR* pSwitchPlanInfo, std::string &pCameraSwitchID);
private:
    static IVS_INT32 AddFavoriteDeviceParseXML(const IVS_CHAR* pDeviceFavoriteInfo, IVS_CHAR* pszParentFolderID, IVS_CHAR* pszDeviceCode);
    static IVS_INT32 MoveFavoriteDeviceParseXML(const IVS_CHAR* pDeviceFavoriteInfo, IVS_CHAR* pszParentFolderID, IVS_CHAR* pszDeviceFavoriteID);
    static IVS_INT32 DelFavoriteDeviceParseXML(const IVS_CHAR* pDeviceFavoriteInfo, IVS_CHAR* pszDeviceFavoriteID);
    static IVS_INT32 DelFavoriteFolderParseXML(const IVS_CHAR* pDeviceFavoriteInfo, IVS_CHAR* pszFolderID);
    static IVS_INT32 AddFavoriteFolderParseXML(const IVS_CHAR* pDeviceFavoriteInfo, IVS_CHAR* pszParentFolderID, IVS_CHAR* pszFolderName);
    static IVS_INT32 ModFavoriteFolderParseXML(const IVS_CHAR* pFavoriteFolder, IVS_CHAR* pszFolderID, IVS_CHAR* pszFolderName);
    static IVS_INT32 MoveFavoriteFolderParseXML(const IVS_CHAR* pFavoriteFolder, IVS_CHAR* pszParentFolderID, IVS_CHAR* pszFolderID);
};

#endif //_IVS_FAVORITE_H_
