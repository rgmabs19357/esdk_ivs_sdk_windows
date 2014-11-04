/********************************************************************
filename 	: GlobalIAMessageMgr.h
author   	: z00227377
created 	: 2013/01/23
description : 智能分析消息全局管理
copyright 	: Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history 	: 2013/01/23 初始版本
*********************************************************************/

#ifndef __IVS_OCX_IA_MESSAGE_MGR_H__
#define __IVS_OCX_IA_MESSAGE_MGR_H__

#include "Lock.h"
#include "SDKDef.h"


class CVideoPane;			//lint !e763
class CGlobalIAMessageMgr;	//lint !e763

#define IA_INVALID_REQUEST_ID	0
#define TRANS_ID_LENTH			32

#define IA_QUERY_TYPE_REALTIME	0
#define IA_QUERY_TYPE_RECORD	1

#define IA_RULE_TYPE_ANALYSE	1
#define IA_RULE_TYPE_RECOGNIZE	3

#define TREAD_WAIT_MSECS		10000
#define TRACK_MESSAGE_NUM_MIN	200

typedef struct
{
	IVS_ULONG ulRequestID;					// Panel识别号
//  IVS_CHAR  CUCode[65];
	IVS_ULONG ulRuleType;					// 规则类型，1是行为分析 3是人脸识别 4前端智能分析 5人脸抓拍
	IVS_ULONG ulQueryType;					// 请求类型，0表示实况，1表示录像
	IVS_CHAR  CameraID[IVS_DEV_CODE_LEN + 1];	// 摄像机编号
	IVS_CHAR  StartTime[IVS_TIME_LEN + 1];		// 轨迹叠加开始时间, 格式为yyyyMMddHHmmss
	IVS_CHAR  StopTime[IVS_TIME_LEN + 1];		// 轨迹叠加结束时间, 格式为yyyyMMddHHmmss
}IA_REQ_PARAM;

typedef struct
{
	IVS_CHAR Year[5];
	IVS_CHAR Month[3];
	IVS_CHAR Day[3];
	IVS_CHAR Hour[3];
	IVS_CHAR Min[3];
	IVS_CHAR Sec[3];
}TM;


typedef std::list<CVideoPane*> PANEL_LIST;
typedef PANEL_LIST::iterator   PANEL_ITER;




/************************************************************************/
/*                            CIAObject                                 */
/*                 与RequestID对应的Panel列表及轨迹请求参数                  */
/************************************************************************/

class CIAObject
{
	friend CGlobalIAMessageMgr;
public:
	CIAObject(IVS_ULONG	ulRuleType, IVS_ULONG ulQueryType);
	~CIAObject();
	IVS_INT32 InsertPanel(CVideoPane *pPanel);
	IVS_INT32 DeletePanel(const CVideoPane *pPanel);
	IVS_INT32 SetCameraID(const TCHAR *cCamID);
	bool CheckPanelListNull()const { return m_VideoPaneList.empty(); }
	IVS_INT32 GetPanelNum();

private:
	CIAObject();

private:
	IVS_ULONG	m_ulRuleType;
	IVS_ULONG	m_ulQueryType;
	IVS_CHAR	m_CameraID[IVS_DEV_CODE_LEN + 1];
	PANEL_LIST	m_VideoPaneList;
};

/************************************************************************/
/*                           CRecordPanel                               */
/*                   记录录像轨迹叠加Panel及轨迹请求参数                      */
/************************************************************************/

class CRecordPanel
{
	friend CGlobalIAMessageMgr;
public:
	CRecordPanel(IVS_INT32 iSessionID, const IA_REQ_PARAM &ReqParam, CVideoPane *pVideoPane);
	~CRecordPanel();
	IVS_INT32 SetTransID(const IVS_CHAR *cTransID);

private:
	CRecordPanel();
private:
	IVS_INT32		m_iSessionID;	
	IA_REQ_PARAM	m_ReqParam;
	IVS_CHAR		m_cTransID[TRANS_ID_LENTH + 1];
	CVideoPane		*m_pVideoPane;
};


/************************************************************************/
/*                        CGlobalIAMessageMgr                           */
/*                    管理轨迹消息与Panel之间的对应关系                       */
/************************************************************************/

class CGlobalIAMessageMgr
{
public:
	static CGlobalIAMessageMgr& Instance();
	~CGlobalIAMessageMgr();
	
	IVS_ULONG StartOverlay(IVS_INT32 iSessionID, CVideoPane *pVideoPane, IVS_ULONG ulRuleType);


	IVS_INT32 StopOverlay(IVS_INT32 iSessionID, const CVideoPane *pVideoPane, IVS_ULONG &ulRequestID);
	IVS_INT32 IAPushAlarmLocus(const IVS_CHAR *pIAMessage);

private:
	CGlobalIAMessageMgr();
	IVS_ULONG AllocRequestID();
	IVS_INT32 GetReqParam(const CVideoPane *pVideoPane, IVS_ULONG ulRuleType, IA_REQ_PARAM &IAReqParam)const;
	IVS_ULONG TryRealStreamShare(CVideoPane *pVideoPane, const IA_REQ_PARAM &IAReqParam);
	IVS_ULONG StartRealtimeOverlay(CVideoPane *pVideoPane, IVS_INT32 iSessionID, IA_REQ_PARAM &IAReqParam);
	IVS_ULONG StartRecordOverlay(CVideoPane *pVideoPane, IVS_INT32 iSessionID, IA_REQ_PARAM &IAReqParam);

	IVS_INT32 SendMessageReqXML(IVS_INT32 iSessionID, IA_REQ_PARAM &IAReqParam, IVS_CHAR *pTransID)const;
	IVS_INT32 SendMessageStopXML(IVS_INT32 iSessionID, IVS_ULONG ulRequestID, IVS_ULONG ulRuleType, IVS_ULONG ulQueryType, const IVS_CHAR *CameraID, IVS_CHAR *pTransID)const;
	IVS_INT32 InsertIAObject(CVideoPane *pVideoPane, IA_REQ_PARAM &IAReqParam);
	IVS_VOID DeletePanelInMap(IVS_INT32 iSessionID, IVS_ULONG ulRequestID, const CVideoPane *pVideoPane); 
	IVS_VOID DeletePanelInRecordList(IVS_INT32 iSessionID, const CVideoPane *pVideoPane) ;
	IVS_INT32 InsertTrackInfo(const IA_REQ_PARAM &IAReqParameters, const IA_LAYOUT_MESSAGE &IALayoutMessage);
	static IVS_VOID CalculateStopTime(IA_REQ_PARAM &IAReqParam);
	static  IVS_UINT32 __stdcall ThreadProc(void *pParameter);
	


private:

	typedef std::map<IVS_ULONG, CIAObject*>	IAMESSAGE_MAP;
	typedef IAMESSAGE_MAP::iterator			IAMESSAGE_MAP_ITER;

	typedef std::list<CRecordPanel*>		RECORD_PANEL_LIST;
	typedef RECORD_PANEL_LIST::iterator		RECORD_PANEL_LIST_ITER;


	IVS_ULONG  	m_ulRequestID;
	bool		m_bInited;
	bool		m_bThreadRun;

	IA_REQ_PARAM		m_IAReqParam;
	IAMESSAGE_MAP		m_IAMessageMap;
	RECORD_PANEL_LIST	m_RecordPanelList;
	
	HANDLE				m_ThreadHandle;
	CIVSMutex			m_MutexMap;
	CIVSMutex			m_MutexList;

};




/************************************************************************/
/*                           COCXListObject                             */
/*                    对应同一个消息的OCXPlayerCtrl列表                     */
/************************************************************************/
class CIAMessageOCXMgr;

typedef std::list<CIVS_OCXPlayerCtrl*>	OCX_PLAYER_CTRL_LIST;
typedef OCX_PLAYER_CTRL_LIST::iterator	OCX_PLAYER_CTRL_LIST_ITER;

class COCXListObject
{
	friend CIAMessageOCXMgr;
public:
	COCXListObject();
	~COCXListObject();
	IVS_INT32 InsertOCXPlayerCtrl(CIVS_OCXPlayerCtrl *pPanel);
	IVS_INT32 DeleteOCXPlayerCtrl(const CIVS_OCXPlayerCtrl *pPanel);
	bool CheckOCXPlayerCtrlListNull()const { return m_OCXPlayerCtrlList.empty(); }

private:
	OCX_PLAYER_CTRL_LIST	m_OCXPlayerCtrlList;
};

/************************************************************************/
/*                           CIAEventRouter                             */
/*                    管理智能分析消息与OCX之间的对应关系                     */
/************************************************************************/

class CIAEventRouter
{
public:
	~CIAEventRouter();
	static CIAEventRouter &instance();

private:
	CIAEventRouter();

	typedef std::map<IVS_ULONG, COCXListObject*>	IAMESSAGE_OCX_MAP;
	typedef IAMESSAGE_OCX_MAP::iterator				IAMESSAGE_OCX_MAP_ITER;

private:

	IAMESSAGE_OCX_MAP m_IAMessageOCXMap;

};









#endif // __IVS_OCX_IA_MESSAGE_MGR_H__
