/********************************************************************
filename    : RecordQueryInCluster.h
author      : c00206592
created     : 2013/02/01
description : 集群模式下的录像检索（SMU异常）
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2013/02/01 初始版本
*********************************************************************/

#ifndef __RECORD_QUERY_IN_CLUSTER_H__
#define __RECORD_QUERY_IN_CLUSTER_H__
#include "UserMgr.h"

typedef std::list<std::string> NVR_LIST;
typedef NVR_LIST::iterator NVR_LIST_ITER;

//key:时间区间，value:所在区间包含的录像数目;记录录像记录的总和
typedef std::map<time_t,int> TIME_SPAN_ALL_NUM_MAP;
typedef TIME_SPAN_ALL_NUM_MAP::iterator TIME_SPAN_ALL_NUM_MAP_ITER;

//按时间大小排序某个摄像头的所有录像信息
typedef std::map<time_t,RET_RECORD_INFO*> RET_RECORD_INFO_MAP;
typedef RET_RECORD_INFO_MAP::iterator RET_RECORD_INFO_MAP_ITER;

class CRecordData
{
public:
	time_t  m_start;
	time_t  m_end;
	std::string m_strRecordInfo;
};
typedef std::map<time_t,CRecordData> REACORD_INFO_MAP;
typedef REACORD_INFO_MAP::iterator REACORD_INFO_MAP_ITER;


typedef std::map<std::string,std::string> REACORD_INFO_STR_MAP;
typedef REACORD_INFO_STR_MAP::iterator REACORD_INFO_STR_MAP_ITER;

class CRecordQueryInCluster
{
public:
	CRecordQueryInCluster();
	~CRecordQueryInCluster();
public:
	void SetUserMgr(CUserMgr *pUserMgr);

	void SetIndexRange(const IVS_INDEX_RANGE* pIndexRange);

	/***********************************************************************************
    * name       : GetRecordInCluster
    * description: 查询录像列表（集群模式下SMU工作不正常时候）
	* input      : pNssMsgInfo     指令结构体
	* input      : pTimeSpan       查询的时间区间
	* output     : pRecordInfoList 查询结果list
	* return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    ***********************************************************************************/
	IVS_INT32 GetRecordInCluster(CSendNssMsgInfo& sendNssMsgInfo,const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,std::string& strSendXml);

protected:

private:
	/***********************************************************************************
    * name       : GetTimeSpan
    * description: 获取查询的时间区间
	* input      : pUnifiedQuery 统一查询条件
	* output     : stTimeSpan 查询时间区间
	* return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    ***********************************************************************************/
	IVS_INT32 GetTimeSpan(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,IVS_TIME_SPAN& stTimeSpan);

	/***********************************************************************************
    * name       : UpDateQueryIndex
    * description: 更新查询区间
	* input      : pUnifiedQuery 统一查询条件
	* output     : stTimeSpan 查询时间区间
	* return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    ***********************************************************************************/
	IVS_INT32 UpDateQueryIndex(std::string& strQueryXml);

private:
	/***********************************************************************************
    * name       : GetCamerTraceInfo
    * description: 向SCU发起获得摄像机轨迹信息（集群模式下SMU工作不正常时候）
	* input      : pNssMsgInfo     指令结构体
	* output     : NA
	* return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    ***********************************************************************************/
	IVS_INT32 GetCamerTraceInfo(CSendNssMsgInfo& sendNssMsgInfo);

	/***********************************************************************************
    * name       : GetRecordSegment
    * description: 各时间段内记录条数统计消息，用于录像分页检索（集群模式下SMU工作不正常时候）
	* input      : pNssMsgInfo     指令结构体
	* input      : pUnifiedQuery   统一查询
	* output     : NA
	* return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    ***********************************************************************************/
	IVS_INT32 GetRecordSegment(CSendNssMsgInfo& sendNssMsgInfo,const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery);
	/***********************************************************************************
    * name       : ParseRecordSegmentInfo
    * description: 解析各时间段内记录条数统计消息，用于录像分页检索（集群模式下SMU工作不正常时候）
	* input      : xml    
	* output     : timeSpanMap
	* return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    ***********************************************************************************/
	IVS_INT32 ParseRecordSegmentInfo(CXml &xml);
	/***********************************************************************************
    * name       : GetRecordListInfo
    * description: 检索录像数据（集群模式下SMU工作不正常时候）
	* input      : sendNssMsgInfo     
	* output     : pRecordInfoList
	* return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    ***********************************************************************************/
	IVS_INT32 SendRecordListInfo(CSendNssMsgInfo& sendNssMsgInfo,std::string& strSendXml);



	IVS_INT32 GetRecordListRSP(const std::string& strXml,std::string& strSendXml);
	IVS_INT32 SortAndSetRecordInfo(const std::string& strRecordInfo);
//	IVS_INT32 GetTimeFromRecordInfo(const std::string& strRecordInfo,time_t& startTime, time_t& endTime);
	IVS_INT32 GetTimeFromRecordInfo(const std::string& strRecordInfo,std::string& strKeyValue);
	IVS_INT32 DeleteOverlapTimeSpan();
	IVS_INT32 SetTimeToRecordInfo(std::string& strRecordInfo,time_t& start,time_t& end);
	IVS_INT32 AssembleTimeSpanXml(std::string& strXml);
	IVS_INT32 AssembleDomainCodeXml(std::string& strXml,std::string& strDoaminCode);
	std::string AssembleRspXml();


private:
	unsigned int m_uiFromIndex; // 开始索引
	unsigned int m_uiToIndex;   // 结束索引
	unsigned int m_uiFromIndexReMove; //开始索引处省略的记录数
	unsigned int m_uiToIndexReMove; //结束索引处省略的记录数
	CUserMgr *m_pUserMgr;
	NVR_LIST m_nvrList;
	TIME_SPAN_ALL_NUM_MAP m_timeSpanAllNumMap;
	RET_RECORD_INFO_MAP m_retRecordInfoMap;
	time_t m_StarQueryTime; //最终查询开始时间点
	time_t m_EndQueryTime;  //最终查询结束时间点
	unsigned int uiRealNum; //总的符合条件的记录数
	std::string m_strCamerCode;
	std::string m_strRspXml;
	REACORD_INFO_MAP m_ReacordInfoMap;

	REACORD_INFO_STR_MAP m_ReacordInfoStrMap;
};

#endif//__RECORD_QUERY_IN_CLUSTER_H__
