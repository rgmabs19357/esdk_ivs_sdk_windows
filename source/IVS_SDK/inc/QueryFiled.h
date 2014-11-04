/********************************************************************
	filename	: 	CQueryFiled.h
	author		:	z00201790
	created		:	2012/11/20	
	description	:	定义通用查询的ID个字段映射;
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2012/11/20 初始版本;
*********************************************************************/

#ifndef __QUERY_FILED_H__
#define __QUERY_FILED_H__

#include "hwsdk.h"
#include "ivs_error.h"
#include "vos.h"
#include "SDKDef.h"

//加锁需要实现linux跨平台
typedef std::map<std::string,IVS_QUERY_FIELD_TYPE> MAP_FILED_VALUE_TO_ID;
typedef MAP_FILED_VALUE_TO_ID::iterator MAP_FILED_VALUE_TO_ID_ITER;

class CQueryFiled
{
public:
	CQueryFiled(void):bIsInit(false)
	{
		m_pMutexLock = VOS_CreateMutex();
        if (NULL != m_pMutexLock)
        {
            Init();
        }
	}
	~CQueryFiled(void)
	{
		try
		{
			VOS_DestroyMutex(m_pMutexLock);

			m_mapFiledTypeValue2ID.clear();
		}
		catch(...)
		{

		}
	}

	static CQueryFiled& GetInstance()
	{
		static CQueryFiled inst;
		return inst;
	}

	/*************************************************************************
	* name       : Init
	* description: Map表初始化
	* input      : NA
	* output     : NA
	* return     : NA
	*************************************************************************/
	void Init()
	{
		if (!bIsInit)
		{
			(void)VOS_MutexLock(m_pMutexLock);
			//在这里添加新项
            (void)m_mapFiledTypeValue2ID.insert(std::make_pair("UserID", QUERY_USER_ID));
            (void)m_mapFiledTypeValue2ID.insert(std::make_pair("LoginName", QUERY_LOGIN_NAME));
            (void)m_mapFiledTypeValue2ID.insert(std::make_pair("Status", QUERY_USER_STATUS));
            (void)m_mapFiledTypeValue2ID.insert(std::make_pair("UserName", QUERY_USER_NAME));

			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("Account", QUERY_OPERATION_ACCOUNT));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("Result", QUERY_OPERATION_RESULT));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("FromTime", QUERY_OPERATION_START_TIME));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("ToTime", QUERY_OPERATION_END_TIME));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("OperationCode", QUERY_OPERATION_CODE));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("OperationObjectCode", QUERY_OPERATION_OBJECT_CODE));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("occur_time",QUERY_TIME));
			
            (void)m_mapFiledTypeValue2ID.insert(std::make_pair("NVRCode", QUERY_DEV_NVR_CODE)); 

            (void)m_mapFiledTypeValue2ID.insert(std::make_pair("DevName", QUERY_DEV_DEV_NAME)); 
            (void)m_mapFiledTypeValue2ID.insert(std::make_pair("DevType", QUERY_DEV_TYPE)); 
            (void)m_mapFiledTypeValue2ID.insert(std::make_pair("VendorType", QUERY_DEV_VENDOR_TYPE)); 
            (void)m_mapFiledTypeValue2ID.insert(std::make_pair("DevIP", QUERY_DEV_IP)); 
            (void)m_mapFiledTypeValue2ID.insert(std::make_pair("IsOnline", QUERY_DEV_ONLINE));    
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("DevSN", QUERY_DEV_SN)); 
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("DevModelType", QUERY_DEV_MODEL_TYPE)); 
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("Category", QUERY_ALARM_CATEGORY));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("AlarmType", QUERY_ALARM_TYPE));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("AlarmInType", QUERY_ALARM_IN_TYPE));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("AlarmLevelID", QUERY_ALARM_LEVEL_ID));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("UserID", QUERY_USER_ID));

            (void)m_mapFiledTypeValue2ID.insert(std::make_pair("DevGroupCode", QUERY_DEV_GROUP_CODE));
            (void)m_mapFiledTypeValue2ID.insert(std::make_pair("DomainCode",QUERY_DEV_DOMAIN_CODE));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("CameraCode",QUERY_CAMERA_CODE));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("QueryType",QUERY_RECORD_QUERY_TYPE));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("RecordMethod",QUERY_RECORD_METHOD));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("RecordType",QUERY_RECORD_TYPE));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("BookmarkName",QUERY_RECORD_BOOKMARK_NAME));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("PresetName",QUERY_RECORD_PTZ_PRESET_NAME));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("LockDesc",QUERY_RECORD_LOCK_DESC));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("LockOperator",QUERY_RECORD_LOCK_OPERATOR_TYPE));

			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("AlarmInCode",QUERY_ALARM_IN_CODES));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("AlarmType", QUERY_ALARM_TYPE));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("Category", QUERY_ALARM_CATEGORY));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("AlarmLevelValueMin",QUERY_ALARM_LEVEL_MIN));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("AlarmLevelValueMax",QUERY_ALARM_LEVEL_MAX));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("AlarmStatus",QUERY_ALARM_STATUS));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("ConfirmOperator",QUERY_ALARM_CONFIRM_OPERATOR));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("AlarmDesc",QUERY_ALARM_DESC));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("AlarmTypeList", QUERY_ALARM_TYPE_LIST));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("OperatorID", QUERY_OPERATOR_ID));

			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("AlarmLevelValue", QUERY_ALARM_LEVEL_VALUE));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("AlarmInName", QUERY_ALARM_IN_NAME));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("LocationInfo", QUERY_ALARM_LOCATION_INFO));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("NodeCode", QUERY_NODE_CODE));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("OccurFirTimeStart", QUERY_ALARM_LEVEL_VALUE));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("OccurFirTimeEnd", QUERY_ALARM_LEVEL_VALUE));

			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("WatermarkAlarmStatus", QUERY_WATERMARK_ALARM_STATUS));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("UserNameType", QUERY_USER_NAME_TYPE));
			
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("Filter",QUERY_ALARM_TYPE_FILTER));
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("OperatorDomain", QUERY_RECORD_USER_DOMAIN));
			//add c00206592 添加水印告警查询用户所在域编码
			(void)m_mapFiledTypeValue2ID.insert(std::make_pair("UserIDDomainCode", QUERY_WATERMARK_USERID_DOMAIN));
			//end add

			(void)VOS_MutexUnlock(m_pMutexLock);
			bIsInit = true;
		}
	}

	/*************************************************************************
	* name       : GetTypeID
	* description: 根据Field字串获取到对应的ID
	* input      : strValue 查询条件字串
	* output     : 返回定义的通用查询类型
	* return     : 成功返回定义的类型，失败0
	*************************************************************************/
	IVS_UINT32 GetTypeID(std::string strValue)
	{
		MAP_FILED_VALUE_TO_ID_ITER IpIter;
		IVS_QUERY_FIELD_TYPE eType = static_cast<IVS_QUERY_FIELD_TYPE>(0);
		(void)VOS_MutexLock(m_pMutexLock);
		IpIter = m_mapFiledTypeValue2ID.begin();

		IpIter = m_mapFiledTypeValue2ID.find(strValue);
		if (IpIter!=m_mapFiledTypeValue2ID.end())
		{
			//找到了
			eType = static_cast<IVS_QUERY_FIELD_TYPE>(IpIter->second);
		}
		else
		{
			//没找到
			eType = QUERY_INVALID;
		}
		(void)VOS_MutexUnlock(m_pMutexLock);

		return static_cast<IVS_UINT32>(eType);
	}

	/*************************************************************************
	* name       : GetValue
	* description: 获取录像列表
	* input      : eType 通用查询类型
	* output     : 查询类型对应的字串
	* return     : 成功返回对应的字串，失败返回空字串
	*************************************************************************/

	std::string GetValue(IVS_UINT32 eType)
	{
		std::string strValue;
		MAP_FILED_VALUE_TO_ID_ITER IpIter;
		IVS_QUERY_FIELD_TYPE eTypeOld = QUERY_USER_NAME;
		(void)VOS_MutexLock(m_pMutexLock);

		//查找空闲对象;
		for (IpIter = m_mapFiledTypeValue2ID.begin(); IpIter != m_mapFiledTypeValue2ID.end(); IpIter++)
		{
			eTypeOld = IpIter->second;
			if (static_cast<IVS_QUERY_FIELD_TYPE>(eType)==eTypeOld)
			{
				strValue = static_cast<std::string>(IpIter->first);
				break;
			}
		}
		(void)VOS_MutexUnlock(m_pMutexLock);

		return strValue;
	}

	//FileID字段到值的绑定
	MAP_FILED_VALUE_TO_ID m_mapFiledTypeValue2ID;

private:
	IVS_BOOL bIsInit;
	VOS_Mutex* m_pMutexLock;
};

#endif
