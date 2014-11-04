/*****************************************************************
 filename    :    OCXDevicMgrXMLProcess.h
 author      :    guandiqun
 created     :    2012/11/19
 description :    实现设备;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*****************************************************************/
#ifndef __OCX_DEVICE_MGR_XML_PROCESS_H__
#define __OCX_DEVICE_MGR_XML_PROCESS_H__
//#include <afx.h>
#include "SDKDef.h"
//#include <wtypes.h>
class COCXDeviceXMLProcess
{
public:
    ~COCXDeviceXMLProcess();

    
    // 添加主设备的解析xml
    static IVS_INT32 AddDeviceParseXml(CXml &reqXml, IVS_VOID* PVoid, 
        const IVS_UINT32 &uiNum, 
        IVS_CHAR* pSequence);
    
    // 新增设备拼接XML
    static IVS_INT32 AddDeviceGetXml(CXml &rspXml,
        IVS_VOID *pRspData, 
        const IVS_UINT32 &uiLen, const IVS_CHAR *pDomainCode);

    // 获取主设备列表拼接XML
    static IVS_INT32 GetMainDeviceListGetXml( 
        CXml &rspXml, 
        IVS_VOID* pRspData ,
        IVS_VOID* pDomain);
    
	// 获取摄像机设备列表拼接xml
	static IVS_INT32 GetCamerasGetXml( IVS_CAMERA_BRIEF_INFO_LIST* pRspDeviceList,
		CXml &rspXml);

	// 输入获取摄像机设备列表拼接xml
	static IVS_INT32 GetAlarmsGetXml( IVS_ALARM_BRIEF_INFO_LIST* pRspDeviceList,
		CXml &rspXml);

	// 输入获取音频设备设备列表拼接XML
	static IVS_INT32 GetAudiosGetXml( IVS_AUDIO_BRIEF_INFO_LIST* pRspDeviceList,
		CXml &rspXml);
    
    // 查询设备列表拼接XML
    static IVS_INT32 QueryDeviceListGetXml(IVS_VOID* pRspData ,
        IVS_VOID* pDomain ,
        IVS_UINT32 iDevType, 
        CXml &xmlRsp);

	// 获取设备列表拼接XML
	static IVS_INT32 GetDeviceListGetXml(IVS_VOID* pRspData ,
		IVS_UINT32 iDevType, 
		CXml &xmlRsp);
      
	// 查询NVR列表解析XML
    static IVS_INT32 ParseGetNVRListReq(CXml& xml,IVS_CHAR* cDomainCode,
        IVS_INDEX_RANGE &sIndex,
        IVS_UINT32 &uiServerType);

    // 查询NVR列表拼接XML
    static IVS_INT32 GetNVRListRsp(IVS_DEVICE_BRIEF_INFO_LIST *pNVRList,
        IVS_INT32 iRetCode,
        CXml& xml);

    // 删除设备解析XML
    static IVS_INT32 DeleteDevicePraseXML(IVS_DEVICE_CODE* pQueryList, 
	   IVS_CHAR *pDomainCode,
       const IVS_UINT32& uiDeviceNum,
       CXml& xmlReq);
    
    // 删除设备拼接XML
    static IVS_INT32 DeleteDeviceGetXml(IVS_RESULT_LIST* pResList,
        CXml& xmlRsp, const IVS_CHAR *pDomainCode);
    
	// 解析查询主设备详细信息请求xml
    static IVS_INT32 GetDeviceInfoParseXML(const IVS_CHAR* pDomainCode,
        const IVS_CHAR* pDevCode,
        CXml& xml);
     
    // 查询主设备详细信息响应xml
    static IVS_INT32 GetDeviceInfoXML(const IVS_DEVICE_INFO* pDeviceInfo,
        CXml& xml);     
    
	// 设备上报拼接XML
    static IVS_INT32 DeviceStateGetXML(const IVS_DEVICE_STATE* pstDeviceState, CXml& xml);

	// 计算DevCode个数
    static IVS_INT32 GetDevCodeNum(CXml &xml);
       
	// 获取域路由拼接XML
    static IVS_INT32 GetDomainRouteGetXML(IVS_DOMAIN_ROUTE_LIST *pList, 
        CXml &xmlRsp);

	// 根据主设备查询子设备列表拼接XML
    static IVS_INT32 GetChannelListGetXML(IVS_DEV_CHANNEL_BRIEF_INFO_LIST *pList, CXml &xmlRsp, const IVS_CHAR *pDomainCode);

	// 设置前端设备密码解析XML
	static IVS_INT32 SetDevPWDParseXML(IVS_CHAR* pDevCode, IVS_CHAR* pDevUserName, IVS_CHAR* pDevPWD, IVS_CHAR* pDevRegPWD, CXml &xml);

protected:
private:
    COCXDeviceXMLProcess();
    
};

#endif
