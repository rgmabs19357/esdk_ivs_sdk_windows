#ifndef __LOCAL_CONFIG_H__
#define __LOCAL_CONFIG_H__

#include "SDKTimer.h"
#include "io.h"
#include <fstream>
#include <ImageHlp.h>
#pragma comment (lib, "ImageHlp.lib")
#include "SDKDef.h"

#define SOFTWARE_NAME "IVS"
#define LOCAL_CONFING_FILE_NAME "LocalConfig.xml"
#define DEVICE_KEYWORD_FILE_NAME "DeviceKeyWord.xml"
#define CUSTOM_LAYOUT_CONFIG_FILE_NAME "CustomLayoutConfig.xml"
#define FAVORITE_CONFIG_FILE_NAME "FavoriteConfig.xml"
#define SCENARIO_FAVORITE_FILE_NAME "ScenarioFavorite.xml"
#define TVWALL_SCENARIO_FAVORITE_FILE_NAME "TVWallScenarioFavorite.xml"
#define CAMERA_SWITCH_FILE_NAME "CameraSwitch.xml"
#define CAMERA_ALIAS_FILE_NAME "CameraAlias.xml"
#define DEFAULT_LOCALCONFIG_PATH "\\defaultConfig\\LocalConfig.xml"
#define CAMERRA_SWITCH_PLAN_FINE_NAME "CameraSwitchPlan.xml"
#define CAMERRA_SWITCH_PLAN_TVWALL_FILE_NAME "TVWallCameraSwitchPlan.xml"
#define I18N_DIR "\\i18n\\"
#define NET_KEYBOARD_INFO_FILE_NAME "NetKeyBoardInfo.xml"
#define BROADCAST_FILELIST_FILE_NAME "BroadcastFileList.xml"
#define BROADCAST_CAMERALIST_CAMERA_CODE "BroadcastCameraList.xml"

#define DIR_SEPARATOR "\\"
#define DEVCODE_ATTRIBUTE "devcode"
#define LAYOUT_ATTRIBUTE "layoutID"

#define CONNECT_MOD_NODE "ConnectMod"

#define DEVICE_KEYWORDD_NODE "DeviceKeyWord"
#define DEVICE_ALIAS_NODE "CameraAlias"

#define CUSTOM_LAYOUT_NODE "CustomLayout"

#define SCENARIOLIST_NODE "Content/ScenarioList"

#define CAMERASWITCHLIST_NODE "Content/CameraSwitchList"

#define  CAMERASWITCHLPLANIST_NODE "Content/CameraSwitchPlanList"
//#define SCENARIO_INFO_NODE "ScenarioInfo"

// 各项配置的节点（只用于存XML）
#define ALARM_CONFIG_ELEMENT    "LocalConfig/AlarmConfigInfo"
#define RECORD_CONFIG_ELEMENT   "LocalConfig/RecordConfigInfo"
#define CAPTURE_CONFIG_ELEMENT  "LocalConfig/CaptureConfigInfo"
#define NETWORK_CONFIG_ELEMENT  "LocalConfig/NetWorkConfig"
#define PTZ_CONFIG_ELEMENT  "LocalConfig/PTZConfig"
#define RECORD_SEARCH_INFO_ELEMENT  "LocalConfig/RecordSearchInfo"
#define CONNECT_MOD_LIST_ELEMENT     "LocalConfig/ConnectModList"
#define DEVICE_KEYWORD_ELEMENT     "LocalConfig/DeviceKeyWordList"
#define DEVICE_CAMERAALIAS_ELEMENT     "LocalConfig/CameraAliasList"
#define CUSTOM_LAYOUT_LIST_ELEMENT     "CustomLayoutConfig/CustomLayoutList"
#define NET_KEYBORAD_LIST_ELEMENT     "Content/NetKeyBoardInfoList"
#define BROADCAST_FILELIST_ELEMENT    "Content/BroadcastFileList"
#define BROADCAST_CAMERALIST_ELEMENT    "Content/BroadcastList"

// DeviceKeyWord.xml的模板
#define DEVICE_KEYWORD_XML_TEMPLATE "<?xml version='1.0' encoding='utf-8' ?>                  \
<LocalConfig>                                             \
<DeviceKeyWordList></DeviceKeyWordList>                   \
</LocalConfig>"

// LocalConfig.xml的模板
#define LOCAL_CONFIG_XML_TEMPLATE "<?xml version='1.0' encoding='utf-8' ?>                  \
<LocalConfig>                                             \
<AlarmConfigInfo></AlarmConfigInfo>                       \
<RecordConfigInfo></RecordConfigInfo>                     \
<CaptureConfigInfo></CaptureConfigInfo>                   \
<NetWorkConfig></NetWorkConfig>                           \
<PTZConfig></PTZConfig>                                   \
<ConnectModList></ConnectModList>                                 \
<ScenarioRecovery>0</ScenarioRecovery>\
</LocalConfig>"

// CustomLayoutConfig.xml的模板
#define CUSTOM_LAYOUT_CONFIG_XML_TEMPLATE "<?xml version='1.0' encoding='utf-8' ?>                  \
<CustomLayoutConfig>                                             \
<CustomLayoutList></CustomLayoutList>                            \
</CustomLayoutConfig>"

// FavoriteConfig.xml的模板
#define FAVORITE_CONFIG_XML_TEMPLATE "<?xml version='1.0' encoding='utf-8' ?>                  \
<Content>                                             \
<FavoriteFolderList></FavoriteFolderList>                       \
<DeviceFavoriteList></DeviceFavoriteList>                     \
</Content>"

// ScenarioFavorite.xml的模板
#define SCENARIO_FAVORITE_XML_TEMPLATE "<?xml version='1.0' encoding='utf-8' ?>                  \
<Content>                                             \
<ScenarioList></ScenarioList>                       \
</Content>"

// CameraSwitch.xml的模板
#define CAMERA_SWITCH_XML_TEMPLATE "<?xml version='1.0' encoding='utf-8' ?>                  \
<Content>                                             \
<CameraSwitchList></CameraSwitchList>                       \
</Content>"

// CameraAlias.xml的模板
#define CAMERA_ALIAS_XML_TEMPLATE "<?xml version='1.0' encoding='utf-8' ?>                  \
<LocalConfig>                                             \
<CameraAliasList></CameraAliasList>                       \
</LocalConfig>"

// CameraSwitchPlan.xml
#define CAMERA_SWITCH_PLAN_XML_TEMPLATE "<?xml version='1.0' encoding='utf-8' ?>                  \
<Content>                                             \
<CameraSwitchPlanList></CameraSwitchPlanList>                       \
</Content>"

// CameraSwitchPlanTVWall.xml
#define CAMERA_SWITCH_PLAN_TVWALL_XML_TEMPLATE "<?xml version='1.0' encoding='utf-8' ?>                  \
<Content>                                             \
<CameraSwitchPlanList></CameraSwitchPlanList>                       \
</Content>"

#define NET_KEYBOARD_INFO_XML_TEMPLATE "<?xml version='1.0' encoding='utf-8' ?>                  \
<Content>                                             \
<NetboardCameraList></NetboardCameraList>                       \
</Content>"

// BroadcastFileList.xml的模板
#define BROADCAST_FILELIST_XML_TEMPLATE "<?xml version='1.0' encoding='utf-8' ?>                  \
<Content>                                             \
<BroadcastFileList></BroadcastFileList>                       \
</Content>"

// BroadcastCameraList.xml的模板
#define BROADCAST_CAMERALIST_XML_TEMPLATE "<?xml version='1.0' encoding='utf-8' ?>                  \
<Content>                                             \
<BroadcastList></BroadcastList>                       \
</Content>"



class CLocalConfig : public ITrigger
{
public:
	CLocalConfig();
	~CLocalConfig();
	/**********************************************************************
	* name			: SetCDATA2XML
	* description	: 在一个xml的节点中插入一个CDATA数据,
					  文件默认路径在app data/ivs/ip/username/
	* input			: pFileName 文件名，默认路径app data/ivs/ip/username/
					  pElement  节点名称
					  pValue    节点值
	* output     	: NA
	* return     	: true - 设置成功 false - 设置失败
	* remark     	: NA   
	*********************************************************************/
	bool SetCDATA2XML(const char* pFileName, const char* pElement, const char* pValue) const;

	/**********************************************************************
	* name			: SetCDATA2XML
	* description	: 获取一个CDATA数据,文件默认路径在app data/ivs/ip/username/
	* input			: pFileName 文件名，默认路径app data/ivs/ip/username/
					  pElement  节点名称
	* output     	: NA
	* return     	: std::string - 节点中的数据
	* remark     	: NA   
	*********************************************************************/
	std::string GetCDATA2XML(const char* pFileName, const char* pElement) const;

	/**********************************************************************
	* name			: MakeSureLocalConfigDirExist
	* description	: 检查本地配置的文件夹，默认路径是app data/ivs/ip/username/
	* input			: NA
	* output     	: NA
	* return     	: std::string - 文件夹的目录
	* remark     	: NA   
	*********************************************************************/
	std::string MakeSureLocalConfigDirExist() const;

	/**********************************************************************
	* name			: MakeSureLocalConfigFileExist
	* description	: 检查本地文件是否存在，如果不存在就根据模板生成一个,
					  本地文件存放在app data/ivs/ip/username/localconfig.xml
	* input			: pFileName 文件名，默认路径app data/ivs/ip/username/
	* output     	: NA
	* return     	: std::string - 文件夹的路径
	* remark     	: NA   
	*********************************************************************/
	std::string MakeSureLocalConfigFileExist(const char* pFileName) const;

	/**********************************************************************
	* name			: CheckLocalConfigFileExist
	* description	: 检查本地文件是否存在，不存在不创建，返回空路径
	* input			: pFileName 文件名，默认路径app data/ivs/ip/username/
	* output     	: NA
	* return     	: std::string - 文件夹的路径
	* remark     	: NA   
	*********************************************************************/
	std::string CheckLocalConfigFileExist(const char* pFileName) const;

	/**********************************************************************
	* name			: CreateLocalConfigXML
	* description	: 创建localConfig.xml的模板
	* input			: pFilePath 文件路径
	* output     	: NA
	* return     	: bool - 是否创建成功
	* remark     	: NA   
	*********************************************************************/
	bool CreateLocalConfigXML(const char* pFilePath) const;

	/**********************************************************************
	* name			: CreateDeviceKeyWordXML
	* description	: 创建DeviceKeyWord.xml的模板
	* input			: pFilePath 文件路径
	* output     	: NA
	* return     	: bool - 是否创建成功
	* remark     	: NA   
	*********************************************************************/
	bool CreateDeviceKeyWordXML(const char* pFilePath) const;

	/**********************************************************************
	* name			: CreateCustomLayoutXML
	* description	: 创建CustomLayoutConfig.xml的模板
	* input			: pFilePath 文件路径
	* output     	: NA
	* return     	: bool - 是否创建成功
	* remark     	: NA   
	*********************************************************************/
	bool CreateCustomLayoutXML(const char* pFilePath) const;

	/**********************************************************************
	* name			: CreateFavoriteConfigXML
	* description	: 创建FavoriteConfig.xml的模板
	* input			: pFilePath 文件路径
	* output     	: NA
	* return     	: bool - 是否创建成功
	* remark     	: NA   
	*********************************************************************/
	bool CreateFavoriteConfigXML(const char* pFilePath) const;

	/**********************************************************************
	* name			: CreateScenarioFavoriteXML
	* description	: 创建ScenarioFavorite.xml的模板
	* input			: pFilePath 文件路径
	* output     	: NA
	* return     	: bool - 是否创建成功
	* remark     	: NA   
	*********************************************************************/
	bool CreateScenarioFavoriteXML(const char* pFilePath) const;

	/**********************************************************************
	* name			: CreateScenarioFavoriteXML
	* description	: 创建TVWallScenarioFavorite.xml的模板
	* input			: pFilePath 文件路径
	* output     	: NA
	* return     	: bool - 是否创建成功
	* remark     	: NA   
	*********************************************************************/
	bool CreateTVWallScenarioFavoriteXML(const char* pFilePath) const;

	/**********************************************************************
	* name			: CreateCameraSwitchXML
	* description	: 创建CameraSwitch.xml的模板
	* input			: pFilePath 文件路径
	* output     	: NA
	* return     	: bool - 是否创建成功
	* remark     	: NA   
	*********************************************************************/
	bool CreateCameraSwitchXML(const char* pFilePath) const;


	/**********************************************************************
	* name			: CreateCameraAliasXML
	* description	: 创建CameraAlias.xml的模板
	* input			: pFilePath 文件路径
	* output     	: NA
	* return     	: bool - 是否创建成功
	* remark     	: NA   
	*********************************************************************/
	bool CreateCameraAliasXML(const char* pFilePath) const;

	/*****************************************************************
	     function   : CreateCameraSwitchPlanXML
	     description: 创建CameraSwitchPlan.xml的模板
	     input      : pFilePath 文件路径
	     return     : bool - 是否创建成功
	*****************************************************************/
	bool CreateCameraSwitchPlanXML(const char* pFilePath) const;
		
	/*****************************************************************
	     function   : CreateCameraSwitchPlanXMLTVWall
	     description: 创建TVWallCameraSwitchPlan.xml的模板
	     input      : pFilePath 文件路径
	     return     : bool - 是否创建成功
	*****************************************************************/
	bool CreateCameraSwitchPlanXMLTVWall(const char* pFilePath) const;

	/*****************************************************************
	     function   : CreateCameraSwitchPlanXML
	     description: 创建CameraSwitchPlan.xml的模板
	     input      : pFilePath 文件路径
	     return     : bool - 是否创建成功
	*****************************************************************/
	bool CreateNetKeyBoardInfoXML(const char* pFilePath) const;

	/*****************************************************************
	     function   : CreateBroadcastFileListXML
	     description: 创建CameraSwitchPlan.xml的模板
	     input      : pFilePath 文件路径
	     return     : bool - 是否创建成功
	*****************************************************************/
	bool CreateBroadcastFileListXML(const char* pFilePath) const;

	/*****************************************************************
	     function   : CreateBroadcastCameraListXML
	     description: 创建BroadcastCameraList.xml的模板
	     input      : pFilePath 文件路径
	     return     : bool - 是否创建成功
	*****************************************************************/
	bool CreateBroadcastCameraListXML(const char* pFilePath) const;


	/**********************************************************************
	* name			: ParseLayoutXML2List
	* description	: 将布局数据的XML转换成可绘图的点
	* input			: layoutXML 布局的xml信息
					  liRectList xml信息转换出的数据
	* output     	: NA
	* return     	: int - 通用响应码
	* remark     	: NA   
	*********************************************************************/
	IVS_INT32 ParseLayoutXML2List(const char* layoutXML, std::list<CRect> &liRectList, IVS_UINT32& xNum, IVS_UINT32& yNum) const;
	
	/**********************************************************************
	* name			: GetLayoutXML
	* description	: 读取本地文件，获取指定自定义布局的XML
	* input			: ulLayoutID 布局ID
	* output     	: NA
	* return     	: std::string - 布局xml
	* remark     	: NA   
	*********************************************************************/
    IVS_UINT32 GetLayoutXML(unsigned long ulLayoutID, std::string & strlayoutXML) const;

	/**********************************************************************
	* name			: BackUserData
	* description	: 备份本地文件
	* input			: ulLayoutID 布局ID
	* output     	: NA
	* return     	: 错误码
	* remark     	: NA   
	*********************************************************************/
	IVS_INT32 BackUserData(void);

	/**************************************************************************
	* name       : GetConfigDir
	* description: 获取config目录
	* input      : dir		输出目录的存放
	* return     : 成功返回翻译后的整形数
	* remark     : 其他模块请不要调用
    **************************************************************************/
	static IVS_INT32 GetConfigDir(std::string& dir);

	// 保存登陆名
	static void SetLoginName(const std::string& strLoginName);
	// 保存IP
	static void SetIP(const std::string& strIP);

	/***********************************************************************************
    * name       : CompressionDir
    * description: 压缩一个文件夹下的所有文件到zip,不包括子目录
    * input      : zipName:压缩包的名字
				   strInPath: 文件夹路径
				   strOutPath:输出zip文件路径
    * output     : 
    * return     : 返回成功或者失败
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 CompressionDir(const std::string& zipFileName, const std::string& strInPath, const std::string& strOutPath);

	/***********************************************************************************
    * name       : CompressionDir
    * description: 压缩一个文件夹下的所有文件到zip,不包括子目录
    * input      : zipFileName:压缩包的名字
				   zipFile:压缩包的路径
				   strOutPath:文件解压路径
    * output     : 
    * return     : 返回成功或者失败
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 DecompressionDir(const std::string& zipFileName, const std::string& zipFilePath, const std::string& strOutPath);

	/***********************************************************************************
    * name       : StartUserData
    * description: 压缩一个文件夹下的所有文件到zip,不包括子目录
    * input      : iSessionID sessionid
				   strLocalLoginName 用户名
	               strLocalstrIP   ip
    * output     : 
    * return     : 返回成功或者失败
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 StartUserData(IVS_INT32 iSessionID, std::string &strLocalLoginName, std::string &strLocalstrIP);

	/***********************************************************************************
    * name       : StopLocalConfigTimer
    * description: 压缩一个文件夹下的所有文件到zip,不包括子目录
    * input      : NA
    * output     : 
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	void StopLocalConfigTimer(void);
	
	/***********************************************************************************
    * name       : onTrigger
    * description: 定时器的回调函数
    * input      : pArg 附加参数
				   ullScales 定时器时长
	               style   定时器类型
    * output     : 
    * return     : NA
    * remark     : NA
    ***********************************************************************************/
	virtual void onTrigger(void* pArg, ULONGLONG ullScales, TriggerStyle style);

private:
	/**********************************************************************
	* name			: CreateXML
	* description	: 在指定路径，创建模板XML
	* input			: pFilePath 文件路径
					  pStream xml模板信息
	* output     	: NA
	* return     	: bool - 是否创建模板成功
	* remark     	: NA   
	*********************************************************************/
	bool CreateXML(const char* pFilePath, const char* pStream) const;

	/**********************************************************************
	* name			: StartLocalConfigTimer
	* description	: 启动定时器
	* input			: NA
	* output     	: NA
	* return     	: NA
	* remark     	: NA   
	*********************************************************************/
	void StartLocalConfigTimer();
public:
	static std::string m_strLocalLoginName;  
	static std::string m_strLocalstrIP;
    std::string m_strFileNamePath;

	CNVSTimer m_LocalConfigTimerInst; 	     // 用户数据备份定时器
	IVS_INT32 m_iSessionID;
	bool m_bBackUserData;
	VOS_Mutex* m_pMutexLocalConfigFileLock;  // 检查本地文件是否存在相关数据锁	

};

#endif
