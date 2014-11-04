/*****************************************************************
 filename    :    OCXDeviceGroupMgrXMLProcess.h
 author      :    guandiqun
 created     :    2012/11/19
 description :    实现设备组XML的拼装和解析;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*****************************************************************/
#ifndef __OCX_DEVICE_GROUP_PROCESS_H__
#define __OCX_DEVICE_GROUP_PROCESS_H__

#pragma once
#include <afx.h>
#include "SDKDef.h"
#include "OCXXmlProcess.h"

class COCXDeviceGroupMgrXMLProcess
{
public:
    ~COCXDeviceGroupMgrXMLProcess();

    // 查询设备组info的数量
	static IVS_UINT32 GetGroupInfoNum(CXml &xmlTemp);

	// 添加设备组拼接XML
	static IVS_INT32 AddDeviceGroupGetXML(const IVS_UINT32& uiDevGroupID,
		CXml &xml);

	// 新增设备组解析XML
	static IVS_INT32 AddDeviceGroupParseXML(IVS_DEV_GROUP* pGroup,
		CXml &xml);

	// 删除设备组解析XML
	static IVS_INT32 DeleteDeviceGroupParseXML(IVS_DEV_GROUP &stDevGroup,
		CXml &xml);

	// 移动设备组解析XML
	static IVS_INT32 MoveDeviceGroupParseXML(IVS_DEV_GROUP &stDevGroup,
		CXml& xml);

	// 查询设备组拼接XML
    static IVS_INT32 GetDeviceGroupGetXML(IVS_DEVICE_GROUP_LIST* pDeviceGroupList, IVS_UINT32 uiBufferSize, CXml& xml);

	// 查询设备组解析XML
    static IVS_INT32 GetDeviceGroupParseXML(IVS_CHAR* pDomainCode, IVS_CHAR* pDevGroupCode, CXml& xmlReq);

    // 修改设备组名称解析XML
	static IVS_INT32 ModifyDeviceGroupNamePraseXML(IVS_DEV_GROUP* pDevGroupInfo,
		CXml &xml);

	// 新增设备组摄像机解析XML
	static IVS_INT32 AddDeviceToGroupPraseXML(IVS_INT32 iSeesionID, 
		IVS_CHAR* pTargetDomainCode,
		IVS_UINT32 uiDeviceNum,
		IVS_DEV_GROUP_CAMERA* pGroupCameraList,
		IVS_UINT32& uiTargetDevGroupID,
		CXml& xmlReq);
	// 删除设备组摄像机解析XML
	static IVS_INT32 DeleteDeviceFromGroupPraseXML(IVS_INT32 iSeesionID, 
													IVS_CHAR* pTargetDomainCode,
													IVS_UINT32 uiDeviceNum,
													IVS_DEV_GROUP_CAMERA* pGroupCameraList,
													CXml& xmlReq);

	// 设置设备组备注信息解析XML
	static IVS_INT32 SetDevGroupRemarkParseXML(IVS_CHAR* pDomainCode, IVS_UINT32 &uiDevGroupID, IVS_CHAR* pDevGroupRemark, CXml &xml);

	// 查询设备组备注信息解析XML
	static IVS_INT32 GetDevGroupRemarkParseXML(IVS_CHAR* pDomainCode, IVS_UINT32 &uiDevGroupID, CXml &xml);

	// 查询设备组备注信息拼接XML
	static IVS_INT32 GetDevGroupRemarkGetXML(const IVS_CHAR* pDevGroupRemark, CString& strResult);

private:
    COCXDeviceGroupMgrXMLProcess();
    
};



#endif
