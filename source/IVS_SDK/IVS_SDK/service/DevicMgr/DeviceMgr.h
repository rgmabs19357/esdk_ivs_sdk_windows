/********************************************************************
	filename	: 	DeviceMgr.h
	author		:	gwx151720
	created		:	2012/11/01	
	description	:	设备组管理类，提供给cu使用
	copyright	:	Copyright (C) 2012
	history		:	2012/11/01
*********************************************************************/
#ifndef _DEVICE_MGR_H_
#define _DEVICE_MGR_H_

#include "ivs_error.h"
#include "hwsdk.h"
#include "vos.h"
#include "DeviceGroupMgr.h"
#include "ivs_xml.h"
#include "SDKDef.h"
#include "SDKSecurityClient.h"
// 设备基本信息链表
typedef std::list<IVS_CAMERA_BRIEF_INFO> CAMERA_BRIEF_INFO_LIST;
typedef CAMERA_BRIEF_INFO_LIST::iterator CAMERA_BRIEF_INFO_LIST_ITER;

typedef std::vector<std::string> DOMAIN_VEC;
typedef std::vector<IVS_CAMERA_BRIEF_INFO> CAMERA_BRIEF_INFO_VEC;
typedef std::vector<IVS_ALARM_BRIEF_INFO> ALARM_BRIEF_INFO_VEC;
typedef std::vector<IVS_AUDIO_BRIEF_INFO> AUDIO_BRIEF_INFO_VEC;
// NVR信息链表
typedef std::list<IVS_DEVICE_BRIEF_INFO*> DEVICE_BRIEF_INFO_LIST;
typedef DEVICE_BRIEF_INFO_LIST::iterator DEVICE_BRIEF_INFO_LIST_ITER;

// 域编码为主键的设备基本信息链表的map定义
typedef std::map<std::string, DEVICE_BRIEF_INFO_LIST*> DEVICE_BRIEF_INFO_LIST_MAP;
typedef DEVICE_BRIEF_INFO_LIST_MAP::iterator DEVICE_BRIEF_INFO_LIST_MAP_ITER;

class CRecordTaskCameraInfo
{
private:
	unsigned int uiRecordMethod;               //是否需要返回XML
	std::string m_strDomainCode;           //域编码
	std::string m_strCameraCode;           //摄像机编码 
public:
	CRecordTaskCameraInfo()
		:  uiRecordMethod(0)
	{}
	unsigned int GetRecordMethod()const { return uiRecordMethod; };
	void SetRecordMethod(unsigned int m_uiRecordMethod)const{ m_uiRecordMethod = uiRecordMethod; };//lint !e550

	std::string& GetDomainCode(){ return m_strDomainCode; };//lint !e1036 !e1536
	void SetDomainCode(const std::string& usDomainCode){ m_strDomainCode = usDomainCode; };
	std::string& GetCameraCode(){ return m_strCameraCode; };//lint !e1036 !e1536
	void SetCameraCode(const std::string& usCameraCode){ m_strCameraCode = usCameraCode; };
};

// 域编码为主键的当前用户的手动录像任务信息链表的map定义
typedef std::map<std::string,CRecordTaskCameraInfo*> DEVICE_RECORD_TASK_CAMERA_INFO_MAP;
typedef DEVICE_RECORD_TASK_CAMERA_INFO_MAP::iterator DEVICE_RECORD_TASK_CAMERA_INFO_MAP_ITER;


//class CUserMgr;
class CDeviceMgr
{
public:
	CDeviceMgr();
	~CDeviceMgr();

public:
	void SetUserMgr(CUserMgr *pUserMgr);
    void Clear();

	/*****************************************************************
     function   : AddDevice
     description: 增加设备
	 input      : pDomainCode:域编码; uiDeviceNum:设备个数; pDevList:设备信息结构体; pResultList:设备编码列表; uiResultBufferSize:分配内存大小;
     output     : NA
     return     : 成功返回0，失败返回负数错误码
   ***************************************************************/

	IVS_INT32 AddDevice(const IVS_CHAR* pDomainCode, 
                        IVS_UINT32 uiDeviceNum, 
                        const IVS_DEVICE_OPER_INFO* pDevList, 
                        IVS_DEVICE_OPER_RESULT_LIST* pResultList, 
                        IVS_UINT32 uiResultBufferSize)const;

	/*****************************************************************
	     function   : DeleteDevice
	     description: 删除设备
	     input      : pDomainCode:域编码
	                  uiDeviceNum:设备个数
	                  pDevCodeList:设备编码列表
	                  pResultList:设备批量操作结果列表
	                  uiResultBufferSize:分配内存大小
		 output     : NA
	     return     : 成功返回0，失败返回负数错误码
	*****************************************************************/
	IVS_INT32 DeleteDevice(const IVS_CHAR*  pDomainCode,
                            IVS_UINT32 uiDeviceNum , 
                            const IVS_DEVICE_CODE* pDevCodeList , 
                            IVS_RESULT_LIST* pResultList ,
                            IVS_UINT32 uiResultBufferSize)const;

    /*****************************************************************
         function   : QueryDeviceList
         description: 查询设备列表
         input      : uiDeviceType:设备类型
                      pDomainCode:域编码
                      pQueryFormat:查询通用格式
                      pBuffer:分配内存指针
                      uiBufferSize:分配内存大小
		 output     : NA
         return     : 成功返回0，失败返回负数错误码
    *****************************************************************/
    IVS_INT32 QueryDeviceList(IVS_UINT32 uiDeviceType,
							const IVS_CHAR* pDomainCode,
                            const IVS_QUERY_UNIFIED_FORMAT *pQueryFormat,
                            IVS_VOID* pBuffer, 
                            IVS_UINT32 uiBufferSize);

	
	/******************************************************************
	function : GetDeviceList
	description: 获取设备列表
	input : uiDeviceType:设备类型
	input : pIndexRange:分页信息
	input : uiBufferSize:分配内存大小
	input : pDeviceList:分配内存指针
	output : NA
	return : 成功返回0，失败返回负数错误码
	*******************************************************************/
	IVS_INT32 GetDeviceList(IVS_UINT32 uiDeviceType,
		const IVS_INDEX_RANGE* pIndexRange,
		IVS_VOID* pDeviceList,
		IVS_UINT32 uiBufferSize);

	/******************************************************************
	function : GetDomainDeviceList
	description: 获取指定域设备列表
	input : pDomainCode:设备所在的域编码
	input : uiDeviceType:设备类型
	input : pIndexRange:分页信息
	input : uiBufferSize:分配内存大小
	input : pDeviceList:分配内存指针
	output : NA
	return : 成功返回0，失败返回负数错误码
	*******************************************************************/
	IVS_INT32 GetDomainDeviceList(const IVS_CHAR* pDomainCode,
		IVS_UINT32 uiDeviceType,
		const IVS_INDEX_RANGE* pIndexRange,
		IVS_VOID* pDeviceList,
		IVS_UINT32 uiBufferSize);

	/******************************************************************
	function : GetCameras
	description: 获取摄像机
	input : const IVS_INDEX_RANGE * pQueryRange:分页信息
	input : IVS_UINT32 uiBufferSize：分配内存大小
	input : IVS_CAMERA_BRIEF_INFO_LIST * pList	：视频子设备列表
	output ：NA 
	return : 成功返回0，失败返回负数错误码
	*******************************************************************/
	IVS_INT32 GetCameras(const IVS_INDEX_RANGE* pQueryRange,
		                IVS_CAMERA_BRIEF_INFO_LIST* pList,
						IVS_UINT32 uiBufferSize);

    // 查询摄像机列表
    IVS_INT32 GetVideoChannelList(const IVS_INDEX_RANGE* pQueryRange,
                       IVS_CAMERA_BRIEF_INFO_LIST* pList,
                       IVS_UINT32 uiBufferSize);

	// 查询指定域的摄像机列表
	IVS_INT32 GetDomainVideoChannelList(const IVS_CHAR* pDomainCode,
		const IVS_INDEX_RANGE* pQueryRange,
		IVS_CAMERA_BRIEF_INFO_LIST* pList,
		IVS_UINT32 uiBufferSize);

    // 查询告警设备列表
    IVS_INT32 GetAlarmChannelList(const IVS_INDEX_RANGE* pQueryRange,
                        IVS_ALARM_BRIEF_INFO_LIST* pList,
                        IVS_UINT32 uiBufferSize,
                        IVS_UINT32 uiAlarmType);

	// 查询指定域的告警设备列表
	IVS_INT32 GetDomainAlarmChannelList(const IVS_CHAR* pDomainCode,
		const IVS_INDEX_RANGE* pQueryRange,
		IVS_ALARM_BRIEF_INFO_LIST* pList,
		IVS_UINT32 uiBufferSize,
		IVS_UINT32 uiAlarmType);

    IVS_INT32 GetVideoChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_CAMERA_BRIEF_INFO_LIST* pList);
	IVS_INT32 GetVideoChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_CAMERA_BRIEF_INFO_LIST* pList, CAMERA_BRIEF_INFO_VEC &vecCameraDev);

    IVS_INT32 GetAlarmChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_ALARM_BRIEF_INFO_LIST* pList);
	IVS_INT32 GetAlarmChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_ALARM_BRIEF_INFO_LIST* pList, ALARM_BRIEF_INFO_VEC &vecAlarmDev);

	IVS_INT32 ParseChannelListRsp(const IVS_CHAR* pData, IVS_UINT32 uiChannelType, const IVS_INT32 iExDomainFlag, 
		const IVS_CHAR* pTargetDomainCode, IVS_PAGE_INFO& pageInfo, IVS_UINT32& uiItemSize, IVS_INT32 iGetChannelType);

    IVS_INT32 ProcessGetUserChannelListRsp(IVS_CHANNLELIST_RETRIVE_STATUS_INFO& channelListRetriveStatus, /*std::string& strTransID,*/ const IVS_CHAR* pData);

    void SetChannelListRetriveStatusValue(IVS_UINT32 uiChannelType, 
                    const IVS_CHAR* pTargetDomainCode, 
                    IVS_INT32 iExDomainFlag, 
                    IVS_UINT32 uiRealNum,
                    IVS_INT32 emThreadStatus,
					IVS_INT32 iGetChannelType);

	/******************************************************************
	function : GetDeviceListByPage
	description: 通过分页查找设备列表
	input : const IVS_CHAR * pDomainCode：域编码
	input : const IVS_INDEX_RANGE & stRange：分页信息
	input: const IVS_UINT32 &uiDevType：设备类型
	output : IVS_UINT32 & uiSize：设备个数
	output : IVS_INDEX_RANGE & outRange：分页信息
	return : 成功返回0，失败返回负数错误码
	*******************************************************************/
	IVS_INT32 GetDeviceListByPage(const IVS_CHAR* pDomainCode,
		const IVS_INDEX_RANGE &stRange,
		const IVS_UINT32 &uiDevType,
		IVS_UINT32& uiSize,
		IVS_INDEX_RANGE& outRange);

	/******************************************************************
	function : GetAudios
	description: 获取语音设备列表
	input : const IVS_INDEX_RANGE * pQueryRange：分页信息
	input : IVS_AUDIO_BRIEF_INFO_LIST * pList：语音设备列表信息
	input : IVS_UINT32 uiBufferSize：分配内存大小
	output ：NA
	return : 成功返回0，失败返回负数错误码
	*******************************************************************/
	IVS_INT32 GetAudios(const IVS_INDEX_RANGE* pQueryRange,
						IVS_AUDIO_BRIEF_INFO_LIST* pList,
						IVS_UINT32 uiBufferSize);
	/******************************************************************
	function : GetAlarms
	description: 获取告警设备列表
	author: fengjiaji f00152768
	input : const IVS_INDEX_RANGE * pQueryRange：分页信息
	input : IVS_UINT32 uiBufferSize：分配内存大小
	output : IVS_ALARM_BRIEF_INFO_LIST * pList：告警设备列表信息
	return : 成功返回0，失败返回负数错误码
	*******************************************************************/
	//IVS_INT32 GetAlarms(const IVS_INDEX_RANGE* pQueryRange,
	//					IVS_ALARM_BRIEF_INFO_LIST* pList,
	//					IVS_UINT32 uiBufferSize);
						

    /*****************************************************************
         function   : GetMainDeviceList
         description: 获取主设备列表
         input     : const IVS_CHAR * pDomainCode：域编码
         input     : const IVS_QUERY_UNIFIED_FORMAT * pQueryFormat：查询通用格式
         input     : IVS_DEVICE_BRIEF_INFO_LIST * pBuffer：设备详细信息列表
         input     : IVS_UINT32 uiBufferSize：内存大小
		 output    : NA
         return    : 成功返回0，失败返回负数错误码
    *****************************************************************/
    IVS_INT32 GetMainDeviceList(const IVS_CHAR* pDomainCode,
								const IVS_QUERY_UNIFIED_FORMAT *pQueryFormat,
                                IVS_DEVICE_BRIEF_INFO_LIST* pBuffer, 
                                IVS_UINT32 uiBufferSize);

    /*****************************************************************
         function   : GetDeviceInfo
         description: 查询主设备详细信息
         input     : const IVS_CHAR * pDomainCode：域编码
         input     : const IVS_CHAR * pDevCode：设备编码
         input     : IVS_DEVICE_INFO * pDeviceInfo：主设备详细信息
		 output    ：NA
         return    : 成功返回0，失败返回负数错误码
    *****************************************************************/
    IVS_INT32 GetDeviceInfo(const IVS_CHAR* pDomainCode,
                            const IVS_CHAR* pDevCode, 
                            const IVS_DEVICE_INFO* pDeviceInfo)const;

    /*****************************************************************
         function   : GetChannelList
         description: 根据主设备查询子设备列表
         input     : const IVS_CHAR * pDevCode
         input     : IVS_DEV_CHANNEL_BRIEF_INFO_LIST * pChannelList
         input     : IVS_UINT32 uiBufferSize
		 output    ：NA
         return    : 成功返回0，失败返回负数错误码
    *****************************************************************/
    IVS_INT32 GetChannelList(const IVS_CHAR* pDevCode,
                             IVS_DEV_CHANNEL_BRIEF_INFO_LIST* pChannelList, 
                             IVS_UINT32 uiBufferSize)const;

	/*****************************************************************
	     function   : SetMainDeviceName
	     description: 修改主设备名称
	     input      : const IVS_CHAR * pDevCode：设备编码
	     input      : const IVS_CHAR * pNewDevName：新设备名称
		 output     ：NA
	     return     : 成功返回0，失败返回负数错误码
	*****************************************************************/
	IVS_INT32 SetMainDeviceName(const IVS_CHAR* pDevCode,const IVS_CHAR* pNewDevName)const;

	/*****************************************************************
	     function   : SetCameraName
	     description: 修改摄像机名称
	     input      : const IVS_CHAR * pCameraCode：摄像机编码
	     input      : const IVS_CHAR * pNewCameraName：新设备名称
		 output     ：NA
	     return     : 成功返回0，失败返回负数错误码
	*****************************************************************/
	IVS_INT32 SetCameraName(const IVS_CHAR* pCameraCode,const IVS_CHAR* pNewCameraName)const;

    /******************************************************************
     function   : GetCameraBriefInfobyCode
     description: 根据摄像机编号获取摄像机简要信息;
     input      : const std::string & strCameraCode         摄像机编码
				  bHasDomainCode 查询的摄像机编码是否有域编码，默认没有
     output     : IVS_CAMERA_BRIEF_INFO& stCameraBriefInfo  摄像机简要信息
     return     : 成功返回0，失败返回负数错误码
    *******************************************************************/
    IVS_INT32 GetCameraBriefInfobyCode(const std::string& strCameraCode, IVS_CAMERA_BRIEF_INFO& stCameraBriefInfo, bool bHasDomainCode = false);

	/*****************************************************************
	     function   : GetNVRList
	     description: 查询网元列表
	     input      : const IVS_CHAR * pDomainCode：域编码
	     input      : IVS_UINT32 uiServerType：服务器类型
	     input      : const IVS_INDEX_RANGE * pIndex：分页信息
	     input      : IVS_DEVICE_BRIEF_INFO_LIST * pNvrList：设备信息列表
	     input      : IVS_UINT32 uiBufferSize：申请内存大小
		 output     ：NA
	     return     : 成功返回0，失败返回负数错误码 
	*****************************************************************/
	IVS_INT32 GetNVRList(const IVS_CHAR* pDomainCode,IVS_UINT32 uiServerType,const IVS_INDEX_RANGE* pIndex,IVS_DEVICE_BRIEF_INFO_LIST* pNvrList,
		IVS_UINT32 uiBufferSize);

	 /******************************************************************
     function   : GetFirstNVR
     description: 根据域编码找到第一个NVR，并将其移到列表尾部;
     input      : const std::string & strDomainCode            域编码;
     input      : IVS_DEVICE_BASIC_INFO & stDeviceBasicInfo    NVR信息;
     output     : NA
     return     : 成功返回0，失败返回负数错误码 
    *******************************************************************/
    IVS_INT32 GetFirstNVR(const std::string& strDomainCode, IVS_DEVICE_BASIC_INFO& stDeviceBasicInfo);

    /******************************************************************
     function   : GetNVRSize
     description: 根据域编码获取list长度;
     input      : const std::string & strDomainCode
     output     : NA
     return     : 成功返回0，失败返回负数错误码 ;
    *******************************************************************/
    IVS_UINT32 GetNVRSize(const std::string& strDomainCode);

	/*****************************************************************
	     function   : GetNVRInfoByDomainCodeAndNVRCode
	     description: 根据域编码和NVRCode获取NVR信息
	     input      : const std::string & strDomainCode：域编码
	     input      : const std::string & strNVRCode：NVR编码
	     input      : IVS_DEVICE_BASIC_INFO & stDeviceBasicInfo：通用设备信息
		 output     : NA
	     return     : 成功返回0，失败返回负数错误码 ;
	*****************************************************************/
	IVS_INT32 GetNVRInfoByDomainCodeAndNVRCode(const std::string& strDomainCode,const std::string& strNVRCode,IVS_DEVICE_BASIC_INFO& stDeviceBasicInfo);

	//获取所有的NVR消息,接口给内部登录使用，调用一次查询结果保存到Map表中
	/*****************************************************************
	     function   : GetAllNVRInfo
	     description: 获取所有的NVR消息
	     input      : const IVS_CHAR * pDomainCode：域编码
	     input      : IVS_UINT32 uiServerType：服务器类型
		 output     : NA
	     return     : 成功返回0，失败返回负数错误码 ;
	*****************************************************************/
	IVS_INT32 GetAllNVRInfo(const IVS_CHAR* pDomainCode,IVS_UINT32 uiServerType);

	/*****************************************************************
	     function   : DeleteNVRListByDomainCode
	     description: 根据域编码干掉对应的列表
	     input      : const std::string & strDomainCode：域编码
		 output     : NA
		 return     : 成功返回0，失败返回负数错误码 ;
	*****************************************************************/
	IVS_INT32 DeleteNVRListByDomainCode(const std::string& strDomainCode);

	/*****************************************************************
	     function   : ModifyDevStatusByDomainCode
	     description: 设置状态
	     input      : const std::string & strDomainCode：域编码
	     input      : const std::string & strCameraCode：设备编码
	     input      : const IVS_UINT32 & uiStatus：状态
	     input      : const std::string & strNVRCode：网元编码
		 output     : NA
		 return     : 成功返回0，失败返回负数错误码 ;
	*****************************************************************/
	IVS_INT32 ModifyDevStatusByDomainCode(const std::string& /*strDomainCode*/, const std::string& strCameraCode, const IVS_UINT32& uiStatus, const std::string& strNVRCode);

	/*****************************************************************
	     function   : RestartDevice
	     description: 重启前端设备
	     input      : const IVS_CHAR * pDeviceCode：设备编码
					  uiRebootType  重启类型：0-重启 1-恢复默认配置并重启
		 output     : NA
		 return     : 成功返回0，失败返回负数错误码 ;
	*****************************************************************/
	IVS_INT32 RestartDevice(const IVS_CHAR* pDeviceCode, IVS_UINT32 uiRebootType)const;

	/*****************************************************************
	     function   : StartSearchDevice
	     description: 启动搜索前端设备
	     input      : const IVS_CHAR* pDomainCode 目标域编码
						const IVS_CHAR * pSearchInfo 设备搜索信息XML
		 output     : NA
		 return     : 成功返回0，失败返回负数错误码 ;
	*****************************************************************/
	IVS_INT32 StartSearchDevice(const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo);

	/*****************************************************************
	     function   : StopSearchDevice
	     description: 停止搜索前端设备
	     input      : const IVS_CHAR* pDomainCode 目标域编码
					    const IVS_CHAR * pSearchInfo 设备搜索信息XML
		 output     : NA
		 return     : 成功返回0，失败返回负数错误码 ;
	*****************************************************************/
	IVS_INT32 StopSearchDevice(const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo)const;

	/*****************************************************************
	     function   : VerifyDevice
	     description: 验证前端设备
	     input      : const IVS_CHAR* pDomainCode 目标域编码
						const IVS_CHAR * pVerifyInfo 设备验证信息XML
		 output     : IVS_CHAR** pRspXml           验证结果XML 
		 return     : 成功返回0，失败返回负数错误码 ;
	*****************************************************************/
	IVS_INT32 VerifyDevice(const IVS_CHAR* pDomainCode, const IVS_CHAR* pVerifyInfo, IVS_CHAR** pRspXml)const;


	/*****************************************************************
	     function   : GetDevLog
	     description: 查询前端日志
	     input      : const IVS_CHAR * pQueryXml   查询条件XML
		 output     : IVS_CHAR** pRspXml           验证结果XML 
		 return     : 成功返回0，失败返回负数错误码 ;
	*****************************************************************/
	IVS_INT32 GetDevLog(const IVS_CHAR* pQueryXml, IVS_CHAR** pRspXml)const;

	/***********************************************************************************
    * name       : GetRecordTask
    * description: 查询当前用户的手动录像任务
    * input      : pRspXml:当前用户的手动录像任务信息
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 GetRecordTask(const IVS_UINT32 uiUserId,CDeviceMgr &m_DeviceMgr,IVS_CHAR** pRspXml);

	/******************************************************************
    function : GetDeviceCapability
    description: 查询摄像机码流能力
    input  : uiCapabilityType  设置类型：IVS_DEVICE_CAPABILITY_TYPE的DEVICE_ENCODE_CAPABILITY
    input  : pDevCode          设备编码#域编码
	output : pRspXml           查询XML
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
	IVS_INT32 GetDeviceCapability(IVS_UINT32 uiCapabilityType, const IVS_CHAR* pDevCode, IVS_CHAR** pRspXml)const;

	// 获取摄像机预览图接口
	IVS_INT32 GetCameraThumbnail(const IVS_CHAR* pCameraCode, IVS_CHAR* pThumbnailURL,IVS_UINT32 uiBufferSize)const;

	/******************************************************************
    function : SetDevPWD
    description: 设置前端设备密码
    input  : const IVS_CHAR* pDevCode：            设备编码
    input  : const IVS_CHAR* pDevUserName          设备登录用户名
	input  : const IVS_CHAR* pDevPWD               设备登录密码
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
	IVS_INT32 SetDevPWD(const IVS_CHAR* pDevCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD);

	/******************************************************************
    function : ImportDevCfgFile
    description: 导入前端配置文件
    input  : const IVS_CHAR* pDevCode：            设备编码
    input  : const IVS_CHAR* pCFGFile              配置文件
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
	IVS_INT32 ImportDevCfgFile(const IVS_CHAR* pDevCode, const IVS_CHAR* pCFGFile);

	/******************************************************************
    function : ExportDevCfgFile
    description: 导出前端配置文件
    input  : const IVS_CHAR* pDevCode：                设备编码
    input  : const IVS_CHAR* pCFGFilePath              配置文件全路径
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
	IVS_INT32 ExportDevCfgFile(const IVS_CHAR* pDevCode, const IVS_CHAR* pCFGFilePath);

	/******************************************************************
    function : ExGetDeviceStatus
    description: 设备状态查询
	input : pReqXml,请求XML
	output : pRspXml 响应XML
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
	IVS_INT32 ExGetDeviceStatus(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml)const;

	/******************************************************************
    function : ExGetDeviceInfo
    description: 设备信息查询
	input : pReqXml,请求XML
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
	IVS_INT32 ExGetDeviceInfo(const IVS_CHAR* pReqXml)const;

	/******************************************************************
    function : GetCameraEncodeCapabilities
    description: Onvif设备从前端获取能力参数范围
	input : pCameraCode,查询请求的摄像机编码
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
	IVS_INT32 GetDeviceEncodeCapability(const IVS_CHAR* pCameraCode,
		IVS_STREAM_INFO_LIST* pStreamInfoList)const;

    /******************************************************************
    function : GetCameraList
    description: 刷新摄像机列表，向SMU下发同步下级域摄像机列表
	input : iSessionID
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
    IVS_INT32 RefreshCameraList(IVS_INT32 iSessionID);
private:
 
    /******************************************************************
    function : CheckBufferSize
    description: 检查分配空间是否相等
    input : const IVS_UINT32 & uiDevType:设备类型
    input : const IVS_UINT32 & iNum:设备个数
	input : const IVS_UINT32 & uiBufferSize:申请内存大小
    output : NA
    return : IVS_INT32
    *******************************************************************/
    IVS_INT32 CheckBufferSize(const IVS_UINT32 &uiDevType, const IVS_UINT32 &iNum, const IVS_UINT32& uiBufferSize)const;

	/******************************************************************
    function : GetCamerasInfo
    description: 获取摄像机列表结构体赋值
    input : IVS_UINT32 uiSize:循环次数
    input : IVS_CAMERA_BRIEF_INFO_LIST* pList:视频子设备列表
	input : const IVS_INDEX_RANGE* pQueryRange:分页信息
    output : NA
    return : IVS_INT32
    *******************************************************************/
	// IVS_INT32 GetCamerasInfo(IVS_UINT32 uiSize, IVS_CAMERA_BRIEF_INFO_LIST* pList, const IVS_INDEX_RANGE* pQueryRange);

	/******************************************************************
    function : GetAudiosInfo
    description: 获取语音列表结构体赋值
    input : IVS_UINT32 uiSize:循环次数
    input : IVS_AUDIO_BRIEF_INFO_LIST* pList:语音设备列表信息
	input : const IVS_INDEX_RANGE* pQueryRange:分页信息
    output : NA
    return : IVS_INT32
    *******************************************************************/
	IVS_INT32 GetAudiosInfo(IVS_UINT32 uiSize, IVS_AUDIO_BRIEF_INFO_LIST* pList, const IVS_INDEX_RANGE* pQueryRange);

	/******************************************************************
    function : GetAudiosInfo
    description: 获取告警设备列表结构体赋值
    input : IVS_UINT32 uiSize:循环次数
    input : IVS_ALARM_BRIEF_INFO_LIST* pList:告警设备列表信息
	input : const IVS_INDEX_RANGE* pQueryRange:分页信息
    output : NA
    return : IVS_INT32
    *******************************************************************/
	IVS_INT32 GetAlarmsInfo(IVS_UINT32 uiSize, IVS_ALARM_BRIEF_INFO_LIST* pList, const IVS_INDEX_RANGE* pQueryRange);

	/******************************************************************
    function : GetBasicInfoByNVRCode
    description: 根据NVRCode去遍历DEVICE_BRIEF_INFO_LIST查到IVS_DEVICE_BASIC_INFO
    input : IVS_DEVICE_BRIEF_INFO*& pDeviceBriefInfo:查询主设备列表信息
    input : DEVICE_BRIEF_INFO_LIST* pDeviceBriefInfoList:查询主设备列表信息列表
	input : IVS_DEVICE_BASIC_INFO& stDeviceBasicInfo:通用设备信息
	input : DEVICE_BRIEF_INFO_LIST_ITER InfoIter：查询主设备列表信息列表 迭代器
	input : const std::string& strNVRCode：NVRCode
    output : NA
    return : IVS_INT32
    *******************************************************************/
	IVS_INT32 GetBasicInfoByNVRCode(IVS_DEVICE_BRIEF_INFO*& pDeviceBriefInfo, DEVICE_BRIEF_INFO_LIST* pDeviceBriefInfoList, IVS_DEVICE_BASIC_INFO& stDeviceBasicInfo, DEVICE_BRIEF_INFO_LIST_ITER InfoIter, const std::string& strNVRCode)const;

	// 堆叠下新增设备
	IVS_INT32 AddDevicePile(const IVS_CHAR* pDomainCode, 
		IVS_UINT32 uiDeviceNum, 
		const IVS_DEVICE_OPER_INFO* pDevList, 
		IVS_DEVICE_OPER_RESULT_LIST* pResultList)const;

	// 集群下新增设备
	IVS_INT32 AddDeviceCluster(const IVS_CHAR* pDomainCode, IVS_UINT32 uiDeviceNum, const IVS_DEVICE_OPER_INFO* pDevList, IVS_DEVICE_OPER_RESULT_LIST* pResultList)const;

	// 堆叠下设置设备密码
	IVS_INT32 SetDevPWDPile(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD)const;

	// 集群下设置设备密码
	IVS_INT32 SetDevPWDCluster(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD)const;

	// 集群下设置设备密码密钥交换消息发送
	IVS_INT32 SetDevPWDClusterMsg(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD, NSS_MSG_TYPE_E msgType, const char* pszDestModule, const char * pszDeviceCode = "")const;

	// 验证前端设备消息
	IVS_INT32 VerifyDeviceMsg(const IVS_CHAR* pDomainCode, const IVS_CHAR* pServerCode, const IVS_CHAR* pVerifyInfo, IVS_CHAR** pRspXml, enum BP::DOMAIN_TYPE enDomainType = BP::DOMAIN_NONE)const;

public:

	/******************************************************************
	function : GetDomainList
	description: 获取域列表
	author: fengjiaji f00152768
	input : NA
	output : DOMAIN_VEC& DmList
	return : IVS_INT32
	*******************************************************************/
	IVS_INT32 GetDomainListByUserID(DOMAIN_VEC& dmList)const;

	/******************************************************************
	function			: GetVideoDiagnose
	description	: 视频质量诊断计划查询
	author			: zwx211831
	input				: pDomainCode 域编码
	input				: pCameraCode 摄像机编码
	output			: pRspXml 单台VCN的视频质量诊断计划xml
	return				: IVS_INT32
	*******************************************************************/
	IVS_INT32 GetVideoDiagnose(const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pCameraCode,
		IVS_CHAR** pRspXml)const;

	/******************************************************************
	function			: SetVideoDiagnose
	description	: 视频质量诊断计划配置
	author			: zwx211831
	input				: pReqXml 单台VCN的视频质量诊断计划xml
	output			: NA
	return				: IVS_INT32
	*******************************************************************/
	IVS_INT32 SetVideoDiagnose(const IVS_CHAR* pReqXml)const;


	//摄像机列表相关成员变量	
	DOMAIN_VEC m_vecCameraDomain;//摄像机域列表

private:
	CUserMgr   *m_pUserMgr;
	IVS_BOOL   bIsOutAlarm; // 0 不是外域告警设备 1是外域告警设备   

	//存放查询NVR/TAS设备列表的Map
	DEVICE_BRIEF_INFO_LIST_MAP m_DeviceInfoListMap;
	VOS_Mutex* m_pMutexNVRLock;

	//存放查询手动录像任务信息的Map
	DEVICE_RECORD_TASK_CAMERA_INFO_MAP m_DeviceRecordTaskCameraInfoMap;
	VOS_Mutex* m_pMutexRecordTaskLock;
	enum THREAD_STATUS
	{
		eM_Thread_Ready=0,//线程准备
		eM_Thread_Init,//线程初始化
		eM_Thread_ExceptionRet,//线程已异常返回
		eM_Thread_DataIsReving,//线程正在接收数据
		eM_Thread_DataIsReady//线程全部接收完毕数据
	};

    enum THREAD_RETRIVE_STATUS
    {
        eM_Thread_Retrive_Status_Init,           // 初始化
        eM_Thread_Retrive_Status_Retriving, // 获取中
        eM_Thread_Retrive_Status_Ready,      // 就绪
        eM_Thread_Retrive_Excepting,            // 出现异常
    };

	//获取设备列表类型
	enum GET_CHANNEL_TYPE
	{
		eM_Get_Device_All,									// 获取所有设备列表
		eM_Get_Device_ByDomainCode,			// 获取指定域编码设备列表
	}; 


	// 摄像机列表相关成员变量	
	CAMERA_BRIEF_INFO_VEC m_vecAllCameraDev;
    IVS_UINT32 m_uiVideoChannelTotalNum;
	VOS_Mutex* m_pMutexCameraLock;//摄像机列表相关数据锁	
    THREAD_RETRIVE_STATUS m_VideoChannelRetriveThreadStatus;                         // 线程状态
    std::list<IVS_CHANNLELIST_RETRIVE_STATUS_INFO> m_VideoChannelRetriveStatusInfoList;    // 记录每个域响应状态
    VOS_Mutex* m_pVideoRetriveStatusInfoListMutex;

	CAMERA_BRIEF_INFO_VEC m_vecAllCamDevByDom;		//指定域编码的所有摄像机列表
	IVS_UINT32 m_uiVideoChannelTotalNumByDom;			//指定域编码的摄像机列表总数

    // 告警设备列表相关成员变量
    ALARM_BRIEF_INFO_VEC m_vecAllAlarmDev;
    IVS_UINT32 m_uiAlarmChannelTotalNum;
    VOS_Mutex* m_pMutexAlarmLock;//摄像机列表相关数据锁	
    THREAD_RETRIVE_STATUS m_AlarmChannelRetriveThreadStatus;                         // 线程状态
    std::list<IVS_CHANNLELIST_RETRIVE_STATUS_INFO> m_AlarmChannelRetriveStatusInfoList;    // 记录每个域响应状态
    VOS_Mutex* m_pAlarmRetriveStatusInfoListMutex;

	ALARM_BRIEF_INFO_VEC m_vecAllAlarmDevByDom;		//指定域编码的所有告警设备列表
	IVS_UINT32 m_uiAlarmChannelTotalNumByDom;			//指定域编码的设备列表总数
    


	//VOS_Thread* m_pGetCamerasThread; //获取摄像机列表线程指针	
	//static long __stdcall GetCamerasThreadEntry(void* pArg);// 获取摄像机列表线程入口
	//THREAD_STATUS mGetCamerasThreadStatus;//GetCamerasThreadEntry线程状态
	//IVS_INT32 iThreadGetCameraRet;//GetCamerasThreadEntry线程返回值

	////告警设备列表相关成员变量
	//DOMAIN_VEC m_vecAlarmDomain;//告警设备域列表
	//ALARM_BRIEF_INFO_VEC m_vecAllAlarmDev;
	//VOS_Mutex* m_pMutexAlarmLock;//告警设备列表相关数据锁	
	//VOS_Thread* m_pGetAlarmsThread; //获取告警列表线程指针	
	//static long __stdcall GetAlarmsThreadEntry(void* pArg);// 获取告警设备列表线程入口
	//THREAD_STATUS mGetAlarmsThreadStatus;//GetAlarmsThreadEntry线程状态
	//IVS_INT32 iThreadGetAlarmRet;//GetAlarmsThreadEntry线程返回值

	////语音设备列表相关成员变量
	//DOMAIN_VEC m_vecAudioDomain;//语音设备域列表
	//AUDIO_BRIEF_INFO_VEC m_vecAllAudioDev;
	//VOS_Mutex* m_pMutexAudioLock;//语音设备列表相关数据锁	
	//VOS_Thread* m_pGetAudiosThread; //获取设备列表线程指针	
	//static long __stdcall GetAudiosThreadEntry(void* pArg);// 获取语音设备列表线程入口
	//THREAD_STATUS mGetAudiosThreadStatus;//GetAudiosThreadEntry线程状态
	//IVS_INT32 iThreadGetAudioRet;//GetAudiosThreadEntry线程返回值

	//static bool m_bSendLocalCmdFail;      //发送本域请求消息失败
	//static bool m_bSendExdomainCmdFail;   //发送外域请求消息失败
};

#endif
