/********************************************************************
	filename	: 	DeviceGroupMgr.h 
	author		:	gwx151720
	created		:	2012/11/01	
	description	:	设备组管理类，提供给cu使用
	copyright	:	Copyright (C) 2012
	history		:	2012/11/01
*********************************************************************/
#ifndef __DEVICE_RROUP_Mgr_H__
#define __DEVICE_RROUP_Mgr_H__

#include "hwsdk.h"
#include "SDKDef.h"

#define SENDRET 0
#define SENDSTR 1
//2012-11-7 2012-11-9
class CUserMgr;
class CDeviceGroupMgr
{
public:
	CDeviceGroupMgr(void);
	~CDeviceGroupMgr(void);

public:
	void SetUserMgr(CUserMgr *pUserMgr);

	/*************************************************************************
	* name       : AddDeviceGroup
	* description: 新增设备组
	* input      : pGroupInfo:设备组结构体; uiDevGroupID:设备组ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-06
	*************************************************************************/
    IVS_INT32 AddDeviceGroup(const IVS_DEV_GROUP* pDevGroupInfo,IVS_UINT32* uiDevGroupID)const;	

	/*************************************************************************
	* name       : DeleteDeviceFromGroup
	* description: 删除设备组的摄像机
	* input      : pTargetDomainCode:域编码; uiDeviceNum:设备个数; pGroupCameraList:设备组摄像机结构体
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-06
	*************************************************************************/
    IVS_INT32 DeleteDeviceFromGroup(const IVS_CHAR* pTargetDomainCode,
                                    IVS_UINT32 uiDeviceNum,
                                    const IVS_DEV_GROUP_CAMERA* pGroupCameraList)const;

	/*************************************************************************
	* name       : AddDeviceToGroup
	* description: 增加设备组的摄像机
	* input      : pTargetDomainCode:域编码; uiTargetDevGroupID:设备组ID; uiDeviceNum:设备个数; pGroupCameraList:设备组摄像机结构体
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-06
	*************************************************************************/
    IVS_INT32 AddDeviceToGroup(const IVS_CHAR* pTargetDomainCode,
                               IVS_UINT32 uiTargetDevGroupID,
                               IVS_UINT32 uiDeviceNum,
                               const IVS_DEV_GROUP_CAMERA* pGroupCameraList)const;
    
	/*************************************************************************
	* name       : GetDeviceGroup
	* description: 查询设备组列表	
	* input      : pDomainCode:域编码; uiDevGroupID:设备组ID; pIndexRange:通用查询范围结构体;pDeviceGroupList:设备组列表结构体;uiBufferSize:申请内存大小;
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-06
	*************************************************************************/	
    IVS_INT32 GetDeviceGroup(const IVS_CHAR* pDomainCode,
                             const IVS_CHAR* pDevGroupCode,
                             IVS_DEVICE_GROUP_LIST* pDeviceGroupList,
                             IVS_UINT32 uiBufferSize);
 	
 	/*************************************************************************
	* name       : DeleteDeviceGroup
	* description: 删除设备组
	* input      : pDomainCode:域编码
				   uiDevGroupID:设备组ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-06
	*************************************************************************/
    IVS_INT32 DeleteDeviceGroup(const IVS_CHAR* pDomainCode,
                                IVS_UINT32 uiDevGroupID)const;

	/*************************************************************************
	* name       : MoveDeviceGroup
	* description: 移动设备组
	* input      : pDomainCode:域编码
				   uiDevGroupID:设备组ID
				   uiNewParentDevGroupID:新父设备组ID
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-06
	*************************************************************************/
    IVS_INT32 MoveDeviceGroup(const IVS_CHAR* pDomainCode,
                              IVS_UINT32 uiNewParentDevGroupID,
                              IVS_UINT32 uiDevGroupID)const;

	/*************************************************************************
	* name       : ModifyDeviceGroupName
	* description: 修改设备组名称
	* input      : pDomainCode:域编码
				   uiDevGroupID:设备组ID
				   pNewDevGroupName:新设备组名称
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-06
	*************************************************************************/
    IVS_INT32 ModifyDeviceGroupName(const IVS_CHAR* pDomainCode,
                                IVS_UINT32 uiDevGroupID,
                                const IVS_CHAR* pNewDevGroupName)const;	
	/*************************************************************************
	* name       : SetDevGroupRemark
	* description: 设置设备组备注信息
	* input      : pDomainCode:域编码
				   uiDevGroupID:设备组ID
				   pDevGroupRemark:设备组备注信息
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	*************************************************************************/
	IVS_INT32 SetDevGroupRemark(const IVS_CHAR* pDomainCode, IVS_UINT32 uiDevGroupID, const IVS_CHAR* pDevGroupRemark)const;

	/*************************************************************************
	* name       : GetDevGroupRemark
	* description: 查询设备组备注信息
	* input      : pDomainCode:域编码
				   uiDevGroupID:设备组ID
				   pDevGroupRemark:设备组备注信息
				   uiRemarkLen:备注字符长度（最大324字节）
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	*************************************************************************/
	IVS_INT32 GetDevGroupRemark(const IVS_CHAR* pDomainCode, IVS_UINT32 uiDevGroupID, IVS_CHAR* pDevGroupRemark, IVS_UINT32 uiRemarkLen)const;

private:
	CUserMgr *m_pUserMgr;

};

#endif
