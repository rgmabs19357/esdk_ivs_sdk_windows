/********************************************************************
filename : DevicMgrXMLProcess.h
author : gwx151720
created : 2012/12/03
description : 
copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history : 2012/12/03 初始版本
*********************************************************************/

#ifndef __DEVICEMGR_XML_PROCESS_H__
#define __DEVICEMGR_XML_PROCESS_H__

#include "SDKDef.h"

class CDeviceMgrXMLProcess
{
public:
    /******************************************************************
    function : AddDeviceInfoGetXML
    description: 集群下增加设备组装xml
    input : const IVS_CHAR * pLoginID:登录ID
    input : const IVS_CHAR * pDomainCode:域编码
    input : IVS_UINT32 uiDeviceNum:设备个数
    input : const IVS_DEVICE_OPER_INFO * pDevList:设备信息
    input : CXml & xml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/    
    static IVS_INT32 AddDeviceClusterGetXML( 
        const IVS_CHAR* pDomainCode, 
        IVS_UINT32 uiDeviceNum, 
        const IVS_DEVICE_OPER_INFO* pDevList,
		std::vector<std::string> &devPWDList,
		std::vector<std::string> &devRegPWDList,
        CXml &xml);

    /******************************************************************
    function : AddDeviceInfoPraseXML
    description: 增加设备解析xml
    input : CXml & xml
    input : IVS_DEVICE_OPER_RESULT_LIST * pResultList
    input : const IVS_INT32 & iLen
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/    
    static IVS_INT32 AddDeviceClusterPraseXML(CXml &xml,
        IVS_DEVICE_OPER_RESULT_LIST* pResultList, 
        const IVS_INT32 &iLen);
    
    /******************************************************************
    function : GetDeviceListNum
    description: 获取摄像机列表数目
    input : CXml & xml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/    
    static IVS_INT32 GetDeviceListNum(CXml &xml ,IVS_UINT32 &uiTotal);

    /******************************************************************
    function : GetMainDeviceListParseXML
    description: 获取主设备列表解析XML
    input : IVS_DEVICE_BRIEF_INFO_LIST * pDevInfoList
    input : IVS_UINT32 uiBufferSize
    input : CXml & Rspxml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/    
    static IVS_INT32 GetMainDeviceListParseXML(IVS_DEVICE_BRIEF_INFO_LIST* pDevInfoList, 
        IVS_UINT32 uiBufferSize,
        CXml &Rspxml);

    /******************************************************************
    function : DeleteDeviceGetXML
    description: 删除设备拼接XML
    input : const IVS_CHAR * pLoginID
    input : const IVS_UINT32 & uiDeviceNum
    input : const IVS_DEVICE_CODE * pDevCodeList
    input : CXml & xmlReq
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/    
    static IVS_INT32 DeleteDeviceGetXML(
        const IVS_UINT32& uiDeviceNum,
        const IVS_DEVICE_CODE* pDevCodeList,
        CXml& xmlReq,
        const IVS_CHAR*  pDomainCode);
    /******************************************************************
    function : DeleteDeviceParseXML
    description: 删除设备
    input : IVS_RESULT_LIST * pResultList
    input : IVS_UINT32 uiResultBufferSize
    input : CXml & xmlRes
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/    
    static IVS_INT32 DeleteDeviceParseXML(IVS_RESULT_LIST* pResultList,
        IVS_UINT32 uiResultBufferSize,
        CXml& xmlRes);

   /******************************************************************
   function : GetDeviceGroupXML
   description: 构造查询设备组列表请求xml
   input : const IVS_CHAR * cLoginID
   input : const IVS_CHAR * pDomainCode
   input : const IVS_UINT32 uiGroupID
   input : const IVS_INDEX_RANGE * pIndexRange
   input : CXml & xml
   output : NA
   return : 成功返回0，失败返回负数错误码
   *******************************************************************/   
   static IVS_INT32 GetDeviceGroupXML(IVS_INT32 iType,
        const IVS_CHAR* pDomainCode,
        const IVS_CHAR* pDevGroupCode,
        CXml& xml);

   /******************************************************************
   function : GetDeviceGroupParseXML
   description: 解析查询设备组列表响应xml
   input : IVS_DEV_GROUP_LIST * pDeviceGroupList
   input : const IVS_UINT32 uiBufferSize
   input : CXml & xml
   output : NA
   return : 成功返回0，失败返回负数错误码
   *******************************************************************/   
   static IVS_INT32 GetDeviceGroupParseXML(IVS_UINT32 &uiSize, IVS_DEVICE_GROUP_LIST* pDeviceGroupList, 
       const IVS_UINT32 uiBufferSize, 
       CXml& xml);

   /******************************************************************
   function : DeleteDeviceGroupGetXML
   description: 构造删除设备组请求xml
   input : const IVS_CHAR * cLoginID
   input : IVS_UINT32 uiGroupNum
   input : const IVS_DEVICE_GROUP * pDeviceGroupList
   input : CXml & xmlReq
   output : NA
   return : 成功返回0，失败返回负数错误码
   *******************************************************************/   
   static IVS_INT32  DeleteDeviceGroupGetXML(
                                             const IVS_CHAR* pDomainCode,
                                             IVS_UINT32 uiDevGroupID,
                                             CXml& xmlReq);

   /******************************************************************
   function : GetDeviceInfoParseXML
   description: 获取设备详细信息解析XML
   input : IVS_DEVICE_INFO * pDeviceInfo
   input : CXml & xml
   output : NA
   return : 成功返回0，失败返回负数错误码
   *******************************************************************/   
   static IVS_INT32 GetDeviceInfoParseXML(IVS_DEVICE_CFG* pDeviceInfo, CXml& xml);

    /******************************************************************
    function : GetDeviceInfoXML
    description: 查询设备详细信息拼接XML
    input : const IVS_CHAR * cLoginID
    input : const IVS_CHAR * pDomainCode
    input : const IVS_CHAR * pDevCode
    input : CXml & xml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/    
    static IVS_INT32 GetDeviceInfoXML(
        const IVS_CHAR* pDomainCode,
        const IVS_CHAR* pDevCode,
        CXml& xml);

    /******************************************************************
    function : ModifyDeviceGroupGetXML
    description: 修改设备组(名称)拼接XML
    input : const IVS_CHAR * pLoginID
    input : const IVS_DEV_GROUP * pDevGroupInfo
    input : CXml & xml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/    
    static IVS_INT32 ModifyDeviceGroupGetXML(
                                             const IVS_CHAR* pDomainCode,
                                             IVS_UINT32 uiDevGroupID,
                                             const IVS_CHAR* pNewDevGroupName ,
                                             CXml &xml);

    /******************************************************************
    function : MoveDeviceGroupGetXML
    description: 移动设备组拼接XML
    input : const IVS_CHAR * pLoginID
    input : const IVS_CHAR * pTargetDomainCode
    input : IVS_UINT32 uiTargetDevGroupID
    input : const IVS_DEV_GROUP_LIST * pDeviceGroupList
    input : CXml & xml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/    
    static IVS_INT32 MoveDeviceGroupGetXML(
                                            const IVS_CHAR* pDomainCode,
                                            IVS_UINT32 uiNewParentDevGroupID,
                                            IVS_UINT32 uiDevGroupID,
                                            CXml& xml);

    /******************************************************************
    function : AddDeviceGroupGetXML
    description: 新增设备组拼接XML
    input : const IVS_CHAR * cLoginID
    input : const IVS_DEV_GROUP * pGroupInfo
    input : CXml & xml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/    
    static IVS_INT32 AddDeviceGroupGetXML(
        const IVS_DEV_GROUP* pGroupInfo,
        CXml &xml);
	/******************************************************************
    function : AddDeviceGroupParseXML
    description: 新增设备组解析XML
    input : CXml & xml
    input : IVS_UINT32 * uiDevGroupID
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/    
    static IVS_INT32 AddDeviceGroupParseXML(CXml &xml, 
        IVS_UINT32* uiDevGroupID);

    /******************************************************************
    function : DeleteDeviceFromGroupReqGetXML
    description: 删除设备组的摄像机拼接XML
    input : const IVS_CHAR * pLoginID
    input : const IVS_DEV_GROUP_CAMERA_LIST * pDeviceGroupList
    input : CXml & xml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/    
    static IVS_INT32 DeleteDeviceFromGroupGetXML(
                                                 const IVS_CHAR* pTargetDomainCode,
                                                 IVS_UINT32 uiDeviceNum,
                                                 const IVS_DEV_GROUP_CAMERA* pDeviceGroupList,
                                                 CXml& xml);

    /*****************************************************************
         function   : AddDeviceToGroupGetXML
         description: 新增设备下摄像机拼接XML
         output     : const IVS_CHAR * pLoginID
         output     : const IVS_CHAR * pTargetDomainCode
         output     : IVS_UINT32 uiTargetDevGroupID
         output     : IVS_UINT32 uiDeviceNum
         output     : const IVS_DEV_GROUP_CAMERA * pDeviceGroupList
         output     : CXml & xml
         return     : 
    *****************************************************************/
    static IVS_INT32 AddDeviceToGroupGetXML(
                                            const IVS_CHAR* pTargetDomainCode,
                                            IVS_UINT32 uiTargetDevGroupID,
                                            IVS_UINT32 uiDeviceNum,
                                            const IVS_DEV_GROUP_CAMERA* pDeviceGroupList,
                                            CXml& xml);

    /******************************************************************
    function : GetNVRListReqXML
    description: 获取网元列表参数拼接XML
    input : const IVS_CHAR * pLoginID
    input : const IVS_CHAR * pDomainCode
    input : const IVS_INDEX_RANGE * pIndex
    input : IVS_UINT32 uiServerType
    input : CXml & xml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
    static IVS_INT32 GetNVRListReqXML(
        const IVS_CHAR* pDomainCode,
        const IVS_INDEX_RANGE* pIndex,
        IVS_UINT32 uiServerType,
        CXml &xml);

    /******************************************************************
    function : PraseNVRListRsp
    description: 获取网元列表解析XML
    input : CXml & xml
    input : IVS_DEVICE_BRIEF_INFO_LIST * pNvrList
    input : IVS_UINT32 uiBufferSize
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
    static IVS_INT32 ParseNVRListRsp(CXml& xml,
        IVS_DEVICE_BRIEF_INFO_LIST* pNvrList,
        IVS_UINT32 uiBufferSize);


    /******************************************************************
    function : GetCameraStreamParaGetXML
    description: 获取摄像机码流参数拼接XML
    input : const IVS_CHAR * pLoginID
    input : const IVS_CHAR * pDomainCode
    input : const IVS_CHAR * pCode
    input : CXml & xml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
    static IVS_INT32 GetCameraStreamParaGetXML( 
        const IVS_CHAR* pDomainCode, 
        const IVS_CHAR* pCode, 
        CXml &xml);

    /******************************************************************
    function : GetDeviceState
    description: 获取设备状态
    input : CXml & xml
    input : IVS_DEVICE_STATE & stDeviceState
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
    static IVS_INT32 GetDeviceState(CXml& xml, 
        IVS_DEVICE_STATE& stDeviceState);     

    /*****************************************************************
         function   : GetChannelListGetXML
         description: 根据主设备查询子设备拼接XML
         output     : const IVS_CHAR * pLoginID
         output     : const IVS_CHAR * pDomainCode
         output     : const IVS_CHAR * pCode
         output     : CXml & xml
         return     : 
    *****************************************************************/
    static IVS_INT32 GetChannelListGetXML(
                                          const IVS_CHAR* pDomainCode, 
                                          const IVS_CHAR* pCode, 
                                          CXml &xml );

    /*****************************************************************
         function   : GetChannelListParseXML
         description: 根据主设备查询子设备解析XML
         output     : CXml & xml
         output     : IVS_DEV_CHANNEL_BRIEF_INFO_LIST * pChannelList
         return     : 
    *****************************************************************/
    static IVS_INT32 GetChannelListParseXML(CXml &xml ,
                                            IVS_DEV_CHANNEL_BRIEF_INFO_LIST* pChannelList);

	 /******************************************************************
    function : SetMainDeviceNameGetXML
    description: 修改主设备名称拼接XML
    input : const IVS_CHAR * pLoginID
    input : const IVS_CHAR * pDevCode
    input : const IVS_CHAR * pNewDevName
    input : CXml & xml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
	static IVS_INT32 SetMainDeviceNameGetXML(
											 const IVS_CHAR* pDomainCode,
											 const IVS_CHAR* pDevCode,
											 const IVS_CHAR* pNewDevName,
											 CXml &xml);

	/******************************************************************
	function : GetDomainList
	description: "取得用户拥有设备的域列表"请求xml
	author: fengjiaji f00152768
	input : const IVS_UINT32 &uiUserID
	output : CXml &xml
	return : 成功返回0，失败返回负数错误码
	*******************************************************************/
	static IVS_INT32 GetDomainListGetXml(
		                           const IVS_UINT32 &uiUserID,
								   CXml &xml);

	/******************************************************************
	function : GetDeviceListGetXml
	description:获取设备列表时组装请求xml
	author: fengjiaji f00152768
	input : const IVS_CHAR * pLoginID
	input : const IVS_CHAR * pDomainCode
	input : const IVS_UINT32 & uiDevType
	input : const IVS_INDEX_RANGE & stRange
	output : CXml & xml
	return : 成功返回0，失败返回负数错误码
	*******************************************************************/
	static IVS_INT32 GetDeviceListGetXml(IVS_INT32 iType,
									const IVS_CHAR* pDomainCode,
									const IVS_UINT32 &uiDevType,
									const IVS_INDEX_RANGE &stRange,
									CXml &xml);

	/******************************************************************
	function : GetDomainListParseXml
	description:解析 "取得用户拥有设备的域列表"的响应
	author: fengjiaji f00152768
	input : CXml & xml
	output : std::list<std::string> & dmList
	return : 成功返回0，失败返回负数错误码
	*******************************************************************/
	static IVS_INT32 GetDomainListParseXml(CXml &xml,std::vector<std::string> &dmList);

	/******************************************************************
	function : GetCamerasParseXml
	description: 获取摄像机列表后的响应xml解析
	author: fengjiaji f00152768
	input : const IVS_CHAR * pDomainCode
	input : CXml & xml
	output : std::vector<IVS_CAMERA_BRIEF_INFO> * pCameraVec
	output : IVS_UINT32 & uiSize
	output : IVS_INDEX_RANGE & outRange
	return : 成功返回0，失败返回负数错误码
	*******************************************************************/
	static IVS_INT32 GetCamerasParseXml(IVS_INT32 iType, const IVS_CHAR* pDomainCode,
										CXml &xml,
										std::vector<IVS_CAMERA_BRIEF_INFO>* pCameraVec,
										IVS_UINT32 &uiSize,
										IVS_INDEX_RANGE& outRange);

    static IVS_INT32 ParseVideoChannelList(const IVS_CHAR* pDomainCode, 
                                       const IVS_CHAR* pData, 
                                       const IVS_INT32 iExDomainFlag,
                                       IVS_PAGE_INFO& pageInfo, 
                                       std::vector<IVS_CAMERA_BRIEF_INFO>& videoChannelVector);

    static IVS_INT32 ParseAlarmChannelList(const IVS_CHAR* pDomainCode, 
                                    const IVS_CHAR* pData, 
                                    const IVS_INT32 iExDomainFlag,
                                    IVS_PAGE_INFO& pageInfo, 
                                    std::vector<IVS_ALARM_BRIEF_INFO>& alarmChannelVector);

	/******************************************************************
	function : GetAlarmsParseXml
	description: 获取告警列表后的响应xml解析
	author: fengjiaji f00152768
	input : const IVS_CHAR * pDomainCode
	input : CXml & xml
	output : std::vector<IVS_ALARM_BRIEF_INFO> * pAlarmVec
	output : IVS_UINT32 & uiSize
	output : IVS_INDEX_RANGE & outRange
	return : 成功返回0，失败返回错误码
	*******************************************************************/
	static IVS_INT32 GetAlarmsParseXml(const IVS_CHAR* pDomainCode,
										CXml &xml,
										std::vector<IVS_ALARM_BRIEF_INFO>* pAlarmVec,
										IVS_UINT32 &uiSize,
										IVS_INDEX_RANGE& outRange);

	/******************************************************************
	function : GetAudiosParseXml
	description: 获取语音设备列表后的响应xml解析
	author: fengjiaji f00152768
	input : const IVS_CHAR * pDomainCode
	input : CXml & xml
	input : std::vector<IVS_AUDIO_BRIEF_INFO> * pAudioVec
	input : IVS_UINT32 & uiSize
	input : IVS_INDEX_RANGE & outRange
	output : NA
	return : 成功返回0，失败返回负数错误码
	*******************************************************************/
	static IVS_INT32 GetAudiosParseXml(const IVS_CHAR* pDomainCode,
										CXml &xml,
										std::vector<IVS_AUDIO_BRIEF_INFO>* pAudioVec,
										IVS_UINT32 &uiSize,
										IVS_INDEX_RANGE& outRange);

	/******************************************************************
    function : SetMainDeviceNameGetXML
    description: 修改摄像机名称拼接XML
    input : const IVS_CHAR * pLoginID
    input : const IVS_CHAR * pCameraCode
    input : const IVS_CHAR * pNewDevName
    input : CXml & xml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
	static IVS_INT32 SetCameraNameGetXML(
										 const IVS_CHAR* pDomainCode,
										 const IVS_CHAR* pCameraCode,
										 const IVS_CHAR* pNewCameraName,
										 CXml &xml);


	/******************************************************************
    function : RestartDeviceGetXML
    description: 重启前端设备拼接XML
    input : const IVS_CHAR * pLoginID
    input : const IVS_CHAR * pDomainCode
    input : const IVS_CHAR * pDeviceCode
    input : CXml & xml
    output : NA
    return : 成功返回0，失败返回负数错误码
    *******************************************************************/
	static IVS_INT32 RestartDeviceGetXML(
										 const IVS_CHAR* pDomainCode,
										 const IVS_CHAR* pDeviceCode,
										 IVS_UINT32 uiRebootType,
										 CXml &xml);

	/***********************************************************************************
    * function: GetRecordTaskGetXML
	* description: 拼装查询当前用户的手动录像任务请求的xml
	* input      : pCameraCode 摄像机编码
	* output     : xml  平台手动抓拍的xml
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 GetRecordTaskGetXML(const IVS_UINT32 uiUserId,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pCameraCode,CXml &xml);

	/***********************************************************************************
    * function:    SearchDeviceParseXML
	* description: 启动搜索前端设备解析XML
	* input      : xml  启动搜索前端设备XML
	* output     : NA
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 SearchDeviceParseXML(CXml &xml, const IVS_CHAR* pAddress);

	/***********************************************************************************
    * function:    StartSearchDeviceParseXML
	* description: 启动搜索前端设备解析XML
	* input      : xml  启动搜索前端设备XML
	* output     : NA
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 ModifyServerCode(CXml &xml, const std::string& strServerCode, const IVS_CHAR* pAddress);

	/***********************************************************************************
    * function:    FindIdlestNvrParseXML
	* description: 启动搜索前端设备解析XML
	* input      : xml  启动搜索前端设备XML
	* output     : NA
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 FindIdlestNvrParseXML(std::string& strServerCode, CXml &xml);

	/***********************************************************************************
    * function:    SetDevGroupRemarkGetXML
	* description: 设置设备组备注信息拼接XML
	* input      : pDomainCode：域编码
				   uiDevGroupID：设备组ID
				   pDevGroupRemark：备注信息
				   xml：拼接XML
	* output     : NA
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 SetDevGroupRemarkGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiDevGroupID, const IVS_CHAR* pDevGroupRemark, CXml &xml);

	/***********************************************************************************
    * function:    GetDevGroupRemarkGetXML
	* description: 查询设备组备注信息拼接XML
	* input      : pDomainCode：域编码
				   uiDevGroupID：设备组ID
				   xml：拼接XML
	* output     : NA
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 GetDevGroupRemarkGetXML(const IVS_CHAR* pDomainCode, IVS_UINT32 uiDevGroupID, CXml &xml);

	/***********************************************************************************
    * function:    GetDevGroupRemarkParseXML
	* description: 查询设备组备注信息拼接XML
	* input      : pDevGroupRemark：备注信息
				   xml：拼接XML
				   uiRemarkLen: 备注字符长度（最大324字节）
	* output     : NA
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 GetDevGroupRemarkParseXML(IVS_CHAR* pDevGroupRemark, CXml &xml, IVS_UINT32 uiRemarkLen);

	/***********************************************************************************
    * function:    GetDeviceCapabilityGetXML
	* description: 查询摄像机码流能力拼接XML
	* input      : pCameraCode：摄像机编码
				   pDomainCode：域编码
				   xml        : 拼接XML
	* output     : NA
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 GetDeviceCapabilityGetXML(const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode, CXml &xml);

	/***********************************************************************************
    * function:    ImportDevCfgFileGetXML
	* description: 导入前端配置文件
	* input      : pDevCode：   设备编码
				   pDomainCode：域编码
				   pCFGFile   ：配置文件全路径
				   xml        : 拼接XML
	* output     : NA
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 ImportDevCfgFileGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, const IVS_CHAR* pCFGFile, CXml &xml);

	/***********************************************************************************
    * function:    ExportDevCfgFileGetXML
	* description: 设置前端设备密码
	* input      : pDevCode：   设备编码
				   pDomainCode：域编码
				   pDevUserName:设备登录用户名
				   strDevPWD  : 设备登录密码
				   xml        : 拼接XML
	* output     : NA
	* return     : 成功返回0，失败返回错误码;
    ***********************************************************************************/
	static IVS_INT32 SetDevPWDGetXML(const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevCode, const IVS_CHAR* pDevUserName, const std::string &strDevPWD, const std::string &strDevRegPWD, CXml &xml);

	// 堆叠新增设备拼接XML
	static IVS_INT32 AddDevicePileGetXML(const IVS_CHAR* pDomainCode, const IVS_DEVICE_OPER_INFO* pDevList, const std::string &strDevPWD, const std::string &strDevRegPWD, IVS_UINT32 uiNum, CXml &xml);

	static IVS_INT32 AddDevicePileParseXML(IVS_DEVICE_OPER_RESULT_LIST* pResultList, IVS_UINT32 uiNum, CXml &xml);

    // 刷新设备列表
    static IVS_INT32 RefreshCameraListXML(IVS_INT32 iSessionID, CXml &xml);

    //解析返回的Onvif设备编码参数XML
    static IVS_INT32 ParseCameraEncodeCapabilities(CXml& xml,
	    IVS_STREAM_INFO_LIST* pStreameList);

	/******************************************************************
   function		: GetVideoDiagnoseGetXML
   description: 构造查询单台VCN的视频质量诊断计划请求xml
   input			: pDomainCode
   input			: pCameraCode
   output			: xml
   return			: 成功返回0，失败返回负数错误码
   *******************************************************************/   
	static IVS_INT32 GetVideoDiagnoseGetXML(const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pCameraCode,
        CXml& xml);

    ~CDeviceMgrXMLProcess();
protected:
private:
    CDeviceMgrXMLProcess();
    

};


#endif
