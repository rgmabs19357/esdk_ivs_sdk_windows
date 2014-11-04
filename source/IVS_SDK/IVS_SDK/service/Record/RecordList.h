/********************************************************************
	filename	: 	CRecordList.h
	author		:	z00201790
	created		:	2012/11/12	
	description	:	定义录像列表查询管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/11/12 初始版本;
*********************************************************************/

#ifndef __RECORD_LIST_H__
#define __RECORD_LIST_H__

#include "hwsdk.h"
#include "ivs_error.h"
#include "XmlProcess.h"


typedef std::list<IVS_RECORD_DATA_PLACE*> RECORD_DATA_PLACE_List;
typedef RECORD_DATA_PLACE_List::iterator RECORD_DATA_PLACE_List_ITER;

//MBU变更记录信息map
typedef std::map<std::string, std::string> MBU_CHANGE_INFO_LIST_MAP;
typedef MBU_CHANGE_INFO_LIST_MAP::iterator MBU_CHANGE_INFO_LIST_MAP_ITER;

class CUserMgr;
class CSendNssMsgInfo;
class CRecordList
{
public:
	CRecordList(void);
	~CRecordList(void);

public:
	void SetUserMgr(CUserMgr *pUserMgr);

public:

	/***********************************************************************************
    * name       : QueryRecordList
    * description: 查询录像列表
	* input      : pUnifiedQuery，查询条件；pRecordInfoList，查询录像结果；uiBufferSize，缓存大小
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 QueryRecordList(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, 
		IVS_RECORD_INFO_LIST* pDeviceList,IVS_UINT32 uiBufferSize);

	/***********************************************************************************
    * name       : QueryRecordListByBackupServer
    * description: 查询录像列表:从备份服务器
	* input      : pUnifiedQuery，查询条件；pRecordInfoList，查询录像结果；uiBufferSize，缓存大小
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 QueryRecordListByBackupServer(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode, 
		IVS_RECORD_INFO_LIST* pDeviceList,IVS_UINT32 uiBufferSize);

	/***********************************************************************************
    * name       : QueryMBUChangeHistoryList
    * description: 查询MBU变化列表
	* input      : pUnifiedQuery，查询条件；pMBUInfoList，查询MBU变化列表
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 QueryMBUChangeHistoryList(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,const IVS_CHAR* pCameraCode,
		const IVS_CHAR* pDomainCode,IVS_MBU_CHANGE_INFO_LIST* pMBUInfoList,IVS_INT32 iToIndex,IVS_UINT32* uiDiffTotalNum);
	/***********************************************************************************
    * name       : ComputeIndexOfRecordList
    * description: 根据每个MBU检索的结果数和分页请求索引号计算出目标的每个MBU轨迹的结果索引
	* input      : uiFromIndex，开始索引；uiToIndex,结束索引;pMBUInfoList，查询MBU变化列表
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 ComputeIndexOfRecordList(const IVS_UINT32 uiFromIndex,const IVS_UINT32 uiToIndex,IVS_MBU_CHANGE_INFO_LIST* pMBUInfoList)const;

	/***********************************************************************************
    * name       : GetRecordList
    * description: 获取录像列表
	* input      : pCameraCode，摄像机列表；iRecordMethod，录像方式；pTimeSpan，查询时间段；pIndexRange，分页信息；pRecordList，查询录像结果；uiBufSize，缓存大小
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 GetRecordList(const IVS_CHAR* pCameraCode,IVS_INT32 iRecordMethod,const IVS_TIME_SPAN* pTimeSpan,
		const IVS_INDEX_RANGE* pIndexRange,IVS_RECORD_INFO_LIST* pRecordList,IVS_UINT32 uiBufSize);

	/***********************************************************************************
    * name       : GetRecordStatus
    * description: 查询录像状态
    * input      : pCameraCode:摄像机列表; uiRecordMethod:录像方式; pRecordStatus:录像状态
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 GetRecordStatus(const IVS_CHAR* pCameraCode,IVS_UINT32 uiRecordMethod,IVS_UINT32* pRecordStatus)const;

	/********************************************************************
	* name       : ParseGetMBUChangeHistoryList
	* description: MBU服务器变更记录的响应解析XML
	* input      : xml:响应结果xml;pMBUChangeInfoList:MBU服务器变更记录对象
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* remark     : NA
	*********************************************************************/
	IVS_INT32 ParseGetMBUChangeHistoryList(CXml& xml,IVS_MBU_CHANGE_INFO_LIST* pMBUChangeInfoList,IVS_UINT32* uiDiffTotalNum);

	/***********************************************************************************
    * name       : GetRecordNVRInfo
    * description: 获取摄像机在该时间段内所处的NVR信息
    * input      : pCameraCode:摄像机ID; pStart:开始时间;pEnd：结束时间
    * output     : pNVR：NVR信息
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 GetRecordNVRInfo(const IVS_CHAR* pCameraCode,const IVS_INT32 iStarTime,const IVS_INT32 iEndTime, IVS_CHAR* pNVR);


	/***********************************************************************************
    * name       : GetRecordTask
    * description: 查询当前用户的手动录像任务
    * input      : pRspXml:当前用户的手动录像任务信息
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 GetRecordTask(IVS_CHAR** pRspXml);


	/***********************************************************************************
    * name       : LockRecord
    * description: 锁定录像
    * input      : pCameraCode:摄像机编码;  iRecordMethod:录像方式
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 LockRecord(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_RECORD_LOCK_INFO* pRecordLockInfo)const;

	/***********************************************************************************
    * name       : UnLockRecord
    * description: 解锁录像
    * input      : pCameraCode:摄像机编码;  iRecordMethod:录像方式
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 UnLockRecord(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_RECORD_LOCK_INFO* pRecordLockInfo)const;

	/***********************************************************************************
    * name       : ModLockRecord
    * description: 修改录像锁定信息
    * input      : pCameraCode:摄像机编码;  iRecordMethod:录像方式
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 ModLockRecord(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_RECORD_LOCK_INFO* pRecordLockInfo)const;

private:
	//缓存录像数据，最多缓存25个摄像机的信息，多出则删除最早的数据
	void InsertRecordInfoToList(IVS_RECORD_DATA_PLACE* pRecordData);
	//清除录像数据
	void ClearUpList();
	//集群下的录像检索
	//int QueryInCluster(CSendNssMsgInfo& sendNssMsgInfo,const IVS_INDEX_RANGE* pIndex,const IVS_TIME_SPAN* pTimeSpan ,std::string& strpRsp);
	int QueryInCluster(CSendNssMsgInfo& sendNssMsgInfo,const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,std::string& strpRsp);

private:
	//集群模式下的的录像检索所需的函数申明


private:
	CUserMgr *m_pUserMgr;

	RECORD_DATA_PLACE_List m_RecordInfoList;		//本地Session保存录像信息，主要用于集群模式下

	MBU_CHANGE_INFO_LIST_MAP m_MBUChangeInfoMap;   //MBU变更记录信息 
};

#endif
