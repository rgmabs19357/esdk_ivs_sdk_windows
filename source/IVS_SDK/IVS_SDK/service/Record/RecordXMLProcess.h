/********************************************************************
	filename	: 	RecordXMLProcess.h
	author		:	z00193167
	created		:	2012/12/03	
	description	:	录像XML
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/03 初始版本
*********************************************************************/

#ifndef __RECORD_XML_PROCESS_H__
#define __RECORD_XML_PROCESS_H__

#include "XmlProcess.h"

typedef std::vector<IVS_CAMERA_PLAN_SET_INFO> PLAN_INFO_VEC;

class CRecordXMLProcess
{
//private:
//	CRecordXMLProcess();
//public:
//	~CRecordXMLProcess();

public:
	
	/***********************************************************************************
    * name       : GetRecordSegmentXML
    * description: 各时间段内记录条数统计消息，用于录像分页检索
    * input      : pTimeSpan 查找时间区间
	* input      : iHoursPerSeg 每个时间分段的小时数，目前暂定为6
    * output     : xml 组装XML
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetRecordSegmentXML(const IVS_TIME_SPAN* pTimeSpan,const std::string& strCamerCode,IVS_INT32 iHoursPerSeg,CXml &xml);

	/***********************************************************************************
    * name       : GetCamerTraceXML
    * description: 查询摄像机轨迹XML
    * input      : pTimeSpan 查找时间区间
	* input      : strCamerCode 摄像机编码
    * output     : xml 组装XML
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetCamerTraceXML(const IVS_TIME_SPAN* pTimeSpan,const std::string& strCamerCode,CXml &xml);
	/***********************************************************************************
    * name       : ParseCamerTraceInfo
    * description: 解析录像轨迹结果，获取摄像机所在的NVR信息
    * input      : xml:定义XML
    * output     : nvrList NVR信息
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 ParseCamerTraceInfo(CXml& xml,std::list<std::string>& nvrList);

	/***********************************************************************************
    * name       : AddRecordSpaceGetXML
    * description: 增加录像空间
    * input      : pRecordSpace:录像空间结构体; pCameraCode:摄像机编码; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AddRecordSpaceGetXML(const IVS_RECORD_POLICY_TIME* pRecordSpace,const IVS_CHAR* pCameraCode,CXml &xml);
	/***********************************************************************************
    * name       : GetRecordSpaceGetXML
    * description: 查询告警类型列表拼接xml
    * input      : pRecordSpaceInfo:录像空间结构体; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetRecordSpaceGetXML(const IVS_RECORD_POLICY_TIME* pRecordSpaceInfo,CXml &xml);

	/***********************************************************************************
    * name       : GetRecordPolicyGetXML
    * description: 查询录像策略拼接xml
    * input      : IVS_RECORD_POLICY_TIME:录像空间结构体; xml:定义XML
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 GetRecordPolicyGetXML(CXml &xml,const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode);

    /********************************************************************
	name : GetRecordPolicyParseRSP
	author : w00206574
	created : 2012/12/13
	description : 获取录像策略的响应XML
	copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history : 2012/12/13 初始版本
	*********************************************************************/
	static IVS_INT32 GetRecordPolicyParseRSP(IVS_RECORD_POLICY_TIME* pResultList, CXml& xml);

	/********************************************************************
	name : SetRecordPolicyGetXML
	author : w00206574
	created : 2012/12/13
	description : 设置录像策略拼接xml
	copyright : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history : 2012/12/13 初始版本
	*********************************************************************/
	static IVS_INT32 SetRecordPolicyGetXML(const IVS_RECORD_POLICY_TIME* pRecordPolicy ,const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode,CXml&xml);

	/***********************************************************************************
    * name       : AddRecordSpacePraseXML
    * description: 增加录像空间
    * input      : xml:定义XML;pRecordSpaceInfo:录像空间结构体
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	static IVS_INT32 AddRecordSpacePraseXML(CXml xml,const IVS_RECORD_POLICY_TIME* pRecordSpaceInfo);

	/********************************************************************
	* name       : ParseGetRecordList
	* description: 录像检索的响应解析XML
	* input      : xml:响应结果xml;pRecordInfoList:录像计划列表结构体对象; uiBufferSize:响应结果分配空间大小
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/

	static IVS_INT32 ParseGetRecordList(CXml& xml,IVS_RECORD_INFO_LIST* pRecordInfoList,IVS_RECORD_DATA_PLACE* pRecordData,IVS_UINT32 uiBufferSize,
		    IVS_UINT32& uiRecordNum,IVS_UINT32 uiFromIndex,IVS_UINT32 uiToIndex);


	/********************************************************************
	* name       : AddOrModRecordPlanGetXML
	* description: 添加/修改录像计划的请求XML
	* input      : uiDeviceNum:摄像机数量；pDeviceList：摄像机编号； pRecordPlan:录像计划结构体; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
    static IVS_INT32 AddOrModRecordPlanGetXML(const std::string& strSendDomainCode,DeviceCodeList& devList,
	                                          const IVS_RECORD_PLAN* pRecordPlan, CXml &xml);

    /********************************************************************
	* name       : AddOrModExDomainRecordPlanGetXML
	* description: 添加/修改录像计划的请求XML
	* input      : uiDeviceNum:摄像机数量；pDeviceList：摄像机编号； pRecordPlan:录像计划结构体; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
    static IVS_INT32 AddOrModExDomainRecordPlanGetXML(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
        const IVS_RECORD_PLAN* pRecordPlan, CXml &xml);

	/********************************************************************
	* name       : DelRecordPlanGetXML
	* description: 删除录像计划的请求XML
	* input      : uiDeviceNum:摄像机数量；pDeviceList：摄像机编号； uiRecordMethod:录像计划方式; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 DelRecordPlanGetXML(const std::string& strSendDomainCode,DeviceCodeList& devList,
		IVS_UINT32 uiRecordMethod, CXml &xml);

    /********************************************************************
	* name       : DelExDomainRecordPlanXML
	* description: 删除录像计划的请求XML
	* input      : uiDeviceNum:摄像机数量；pDeviceList：摄像机编号； uiRecordMethod:录像计划方式; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
    static IVS_INT32 DelExDomainRecordPlanXML(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,
        IVS_UINT32 uiRecordMethod, CXml &xml);
	/********************************************************************
	* name       : GetRecordPlanGetXML
	* description: 查询录像计划的请求XML
	* input      : pCameraCode：摄像机编号； uiRecordMethod:录像计划方式; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 GetRecordPlanGetXML(const char* pCameraCode,IVS_UINT32 uiRecordMethod, CXml &xml);

	/********************************************************************
	* name       : RecordPlanParseRSP
	* description:  添加/修改/删除录像计划的响应XML
	* input      : pResultList：列表查询结果结构体； uiResultBufferSize:返回结果Buffer大小; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 RecordPlanParseRSP(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,IVS_RESULT_LIST* pResultList,IVS_UINT32 uiResultBufferSize, CXml& xml,IVS_UINT32& uiRecordNum);
	/********************************************************************
	* name       : GetRecordPlanParseRSP
	* description: 查询录像计划的请求XML
	* input      : pRecordPlan:录像计划结构体; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
    static IVS_INT32 GetRecordPlanParseRSP(IVS_RECORD_PLAN* pRecordPlan, CXml& xml);
	/********************************************************************
	* name       : GetRecordListReqXML
	* description: 录像检索的请求XML
	* input      : pCameraCode:摄像机编号；pDomainCode：域编号； iRecordMethod:录像计划方式;
	               pTimeSpan，时间片段结构；pIndexRange，分页信息； xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 GetRecordListReqXML(const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode,IVS_INT32 iRecordMethod,const IVS_TIME_SPAN* pTimeSpan,
		const IVS_INDEX_RANGE* pIndexRange,CXml& xml);
	/********************************************************************
	* name       : GetRecordStatusReqXML
	* description: 查询录像状态的请求XML
	* input      : pCameraCode：摄像机编号； iRecordMethod:录像计划方式; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 GetRecordStatusReqXML(const IVS_CHAR* pCameraCode,IVS_UINT32 iRecordMethod,CXml& xml);
	/********************************************************************
	* name       : ParseRecordStatus
	* description: 查询录像状态的响应解析XML
	* input      : ulStatus:录像状态; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 ParseRecordStatus(CXml& xml,IVS_UINT32* ulStatus);
	/********************************************************************
	* name       : StartRemoteRecordReqXML
	* description: 开始远程录像回放的请求XML
	* input      : pCameraCode：摄像机编号； iRecordType:录像方式;iRecordTime:录像时间; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 StartRemoteRecordReqXML(const IVS_UINT32 uiUserId,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 iRecordType,IVS_UINT32 iRecordTime,CXml& xml);
	/********************************************************************
	* name       : StopRemoteRecordReqXML
	* description: 停止远程录像回放的请求XML
	* input      : pCameraCode：摄像机编号； iRecordType:录像方式; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 StopRemoteRecordReqXML(const IVS_UINT32 uiUserId,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 iRecordType,CXml& xml);

	/********************************************************************
	* name       : AddBookmarkGetXML
	* description: 添加录像书签的请求XML
	* input      : iRecordMethod：录像方式；
				   uiUserId：用户ID;
				   pUserDomainCode:用户所属域；
				   cUserName：用户姓名;
				   pDomainCode:摄像机所属域编号；
				   pNVRCode：NVR编号；
				   pCameraCode：摄像机编号；
				   pBookmarkName：录像书签名称；
				   pBookmarkTime：录像书签时间；
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
    static IVS_INT32 AddBookmarkGetXML(const IVS_INT32 iRecordMethod,const IVS_UINT32 uiUserId,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* cUserName,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,
		const IVS_CHAR* pCameraCode,const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime, CXml &xml);
	/********************************************************************
	* name       : ModifyBookmarkGetXML
	* description: 修改录像书签的请求XML
	* input      : uiUserId：用户ID;cUserName:用户姓名;pCameraCode:摄像机编号；pBookmarkName：书签名称； uiBookmarkID:录像书签ID; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 ModifyBookmarkGetXML(const IVS_INT32 iRecordMethod,const IVS_UINT32 uiUserId,const IVS_CHAR* cUserName,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,
		const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName, CXml &xml);
	/********************************************************************
	* name       : DeleteBookmarkGetXML
	* description: 删除录像书签的请求XML
	* input      : pCameraCode:摄像机编号； uiBookmarkID:录像书签ID; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 DeleteBookmarkGetXML(const IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,CXml &xml);
	/********************************************************************
	* name       : ParseGetBookmarkList
	* description: 解析查询平台书签的响应XML
	* input      : pRecordBookmarkInfoList：录像书签信息列表； strDomainCode:域编码; xml:定义XML；uiBufferSize:响应结果分配空间大小
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 ParseGetBookmarkList(CXml& xml,IVS_RECORD_BOOKMARK_INFO_LIST* pRecordBookmarkInfoList,const std::string& /*strDomainCode*/,IVS_UINT32 uiBufferSize);

	/********************************************************************
	* name       : ParseGetBookmarkListForBackup
	* description: 解析查询备份服务器书签的响应XML
	* input      : pRecordBookmarkInfoList：录像书签信息列表； strDomainCode:域编码; xml:定义XML；uiBufferSize:响应结果分配空间大小
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 ParseGetBookmarkListForBackup(CXml& xml,IVS_RECORD_BOOKMARK_INFO_LIST* pRecordBookmarkInfoList,const std::string& /*strDomainCode*/,IVS_UINT32 uiBufferSize,
		 IVS_UINT32& uiRecordNum,IVS_UINT32 uiFromIndex,IVS_UINT32 uiToIndex);

	/********************************************************************
	* name       : ParseManualRecordStateXML
	* description: 手动录像状态的报文解析XML
	* input      : pManualRecordStatusNotify:手动录像状态信息结构体; xml:定义XML
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 ParseManualRecordStateXML(CXml& xml,IVS_MANUAL_RECORD_STATUS_NOTIFY* pManualRecordStatusNotify);

	/********************************************************************
	* name       : LockRecordReqXML
	* description: 锁定录像的请求XML
	* input      : xml:响应结果xml;
	               iRecordMethod:录像方式；
				   pCameraCode：摄像机编号；
				   pRecordLockInfo：录像锁定结构体信息
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 LockRecordReqXML(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_RECORD_LOCK_INFO* pRecordLockInfo, CXml &xml);

	/********************************************************************
	* name       : UnLockRecordReqXML
	* description: 解锁录像的请求XML
	* input      : xml:响应结果xml;
	               iRecordMethod:录像方式；
				   pCameraCode：摄像机编号；
				   pRecordLockInfo：录像锁定结构体信息
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 UnLockRecordReqXML(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_RECORD_LOCK_INFO* pRecordLockInfo, CXml &xml);

	/********************************************************************
	* name       : ModLockRecordReqXML
	* description: 修改录像锁定信息的请求XML
	* input      : xml:响应结果xml;
	               iRecordMethod:录像方式；
				   pCameraCode：摄像机编号；
				   pRecordLockInfo：录像锁定结构体信息
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_INT32 ModLockRecordReqXML(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_RECORD_LOCK_INFO* pRecordLockInfo, CXml &xml);

    /******************************************************************
    function : GetCameraPlanInfoGetXML
    description: 查询摄像机计划设置信息请求xml
    input : IVS_UINT32 uiPlanType
    input : const IVS_CHAR * pDomainCode
    input : CXml & xml
    output : NA
    return : IVS_VOID
    *******************************************************************/
    static IVS_VOID GetCameraPlanInfoGetXML(IVS_UINT32 uiPlanType, const IVS_CHAR* pDomainCode, CXml &xml);

    /******************************************************************
    function : GetCameraPlanInfoParseXML
    description: 查询摄像机计划设置信息解析响应xml
    input : CXml & xml
    input : PLAN_INFO_VEC * cameraPlanInfoList
    output : NA
    return : IVS_INT32
    *******************************************************************/
    static IVS_INT32 GetCameraPlanInfoParseXML(CXml &xml, PLAN_INFO_VEC &cameraPlanInfoList);

    static IVS_VOID GetCameraPlanInfoGetXml(const std::map<std::string, PLAN_INFO_VEC> &planMap, CXml &xml);

	private:
	/********************************************************************
	* name       : GetCameraDomainCodeByCameraCode
	* description: 获得摄像机编号中的域编号
	* input      : uiDeviceNum：摄像机数量；pDeviceList:带域编号的摄像机编号；CameraCode[64]:摄像机编号;
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	static IVS_CHAR* GetCameraDomainCodeByCameraCode(IVS_UINT32 uiDeviceNum,const IVS_DEVICE_CODE* pDeviceList,const IVS_CHAR CameraCode[64]);

};
#endif // __RECORD_XML_PROCESS_H__
