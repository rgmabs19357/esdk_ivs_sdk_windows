/*****************************************************************
 filename    :    OCXDeviceMgr.h
 author      :    guandiqun
 created     :    2012/11/19
 description :    实现设备;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*****************************************************************/
#ifndef __OCX_DEVICE_MGR_H__
#define __OCX_DEVICE_MGR_H__

#include <afx.h>
#include "SDKDef.h"


class COCXDeviceMgr
{
public:
    ~COCXDeviceMgr();

	// 新增设备
    static IVS_INT32 AddDevice(IVS_INT32 iSessionID, CXml& xmlReq, CXml& xmlRsp);

	// 查询设备列表（可以设置查询条件）
    static IVS_INT32 QueryDeviceList( IVS_INT32 iSessionID, 
                                    IVS_ULONG ulDeviceType, 
                                    CXml& xmlReq, 
                                    CXml& xmlRsp);

	// 获取设备列表(无查询条件设置)
	static IVS_INT32 GetDeviceList( IVS_INT32 iSessionID, 
									IVS_ULONG ulDeviceType, 
									IVS_ULONG ulFromIndex, 
									IVS_ULONG ulToIndex,
									CXml& xmlRsp);

	// 获取域路由
    static IVS_VOID GetDomainRoute(IVS_INT32 iSessionID,                                    
                                    CString &strResult);

	// 查询主设备详细信息
    static IVS_VOID GetDeviceInfo(IVS_INT32 iSessionID,  
                                  CXml& xmlReq,
                                  CString &strResult);
	// 查询NVR列表
    static IVS_VOID GetNVRList(IVS_INT32 iSessionID,  
                               CXml& xmlReq,
                               CString &strResult);
    // 根据主设备查询子设备列表
    static IVS_VOID GetChannelList(IVS_INT32 iSessionID, 
                                   const IVS_CHAR *pDevCode,
                                   CString &strResult);

	// 修改主设备名称
	static IVS_INT32 SetMainDeviceName(IVS_INT32 iSessionID,
									const IVS_CHAR *pDevCode,
									const IVS_CHAR *pNewDevName);

	// 修改摄像机名称
	static IVS_INT32 SetCameraName(IVS_INT32 iSessionID,
									const IVS_CHAR *pCameraCode,
									const IVS_CHAR *pNewCameraName);
    // 删除设备
	static IVS_VOID DeleteDevice(IVS_INT32 iSessionID, CString &strResult, LPCTSTR pReqXml);

	// 重启前端设备
	static IVS_INT32 RestartDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDevCode, IVS_UINT32 uiRebootType);

	// 启动搜索前端设备
	static IVS_INT32 StartSearchDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo);

	// 停止搜索前端设备
	static IVS_INT32 StopSearchDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo);

	// 验证前端设备
	static IVS_VOID VerifyDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pVerifyInfo, CString &strResult);

	// 查询前端日志
	static IVS_VOID GetDevLog(IVS_INT32 iSessionID, const IVS_CHAR* pQueryXml, CString &strResult);

	// 查询摄像机码流能力
	static IVS_VOID GetDeviceCapability(IVS_INT32 iSessionID, IVS_UINT32 uiCapabilityType, const IVS_CHAR* pDevCode, CString &strResult);

	// 设置前端设备密码
	static IVS_INT32 SetDevPWD(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml);

    // 刷新摄像机列表
    static IVS_INT32 RefreshCameraList(IVS_INT32 iSessionID);

    // 查询Onvif设备从前端获取参数参数分为
    static IVS_INT32 GetCameraEncodeCapabilities(IVS_STREAM_INFO_LIST *pStreamInfoList,IVS_INT32 iRetCord,CXml &xml);

	// 查询单台VCN的视频质量诊断计划
	static IVS_INT32 GetVideoDiagnose(IVS_INT32 iSessionID,
		const IVS_CHAR* pDomainCode,
		const IVS_CHAR* pCameraCode,
		IVS_CHAR** pRspXml);

	// 设置单台VCN的视频质量诊断计划
	static IVS_INT32 SetVideoDiagnose(IVS_INT32 iSessionID, 
		const IVS_CHAR* pReqXml);

protected:
private:
    COCXDeviceMgr();
	// 分配内存
    static IVS_VOID* GetListObject( IVS_INT32 lDeviceType , IVS_UINT32 uiNum, IVS_UINT32 &uiSizeBuff);

};



#endif
