/********************************************************************
filename	: 	IVSSDK.cpp
author		:	z00201790
created		:	2012/10/23	
description	:	定义 DLL 应用程序的导出函数;
copyright	:	Copyright (C) 2011-2015
history		:	2012/10/23 初始版本;
*********************************************************************/

#ifndef __USER_MGR_H__
#define __USER_MGR_H__

#include "UserInfoMgr.h"
#include "RoleMgr.h"
#include "UserGroupMgr.h"
#include "RecordPlanMgr.h"
#include "RecordBookmark.h"
#include "RealPlayBookmark.h"
#include "RecordList.h"
#include "DeviceMgr.h"
#include "AlarmMgr.h"
#include "AlarmAreaMgr.h"
#include "AlarmHelpMgr.h"
#include "AlarmLevelMgr.h"
#include "DeviceGroupMgr.h"
#include "DeviceParaConfig.h"
#include "RealPlayMgr.h"
#include "RecordService.h"
#include "PlaybackMgr.h"
#include "LogMgr.h"
#include "DeviceParaConfig.h"
#include "LoginMgr.h"
#include "SDKDef.h"
#include "IVS_SDKTVWall.h"
#include "AlarmLinkageMgr.h"
#include "IntelligenceAnalysisMgr.h"
#include "LinkRouteMgr.h"
#include "DomainRouteMgr.h"
#include "PtzControl.h"
#include "UserDataMgr.h"
#include "MotionDetection.h"
#include "AutoResume.h"
#include "CloseLinkCount.h"
#include "ReLink.h"
#include "TVWallMgr.h"
#include "RecordBackupMgr.h"
#include "CapturePicture.h"
#include "ClientNetworkMgr.h"
#include "AudioMgr.h"
#include "DownloadManger.h"
#include "TelepresenceMgr.h"
#include "ReportMgr.h"
#include "UpgradeMgr.h"

#define XML XmlProcessMgr::GetInstance()

typedef enum
{
	TYPE_NSS_XML = 0,
	TYPE_NSS_NOXML
}NSS_NEED_XML;

class CSendNssMsgInfo
{
private:
	unsigned short m_usReqID;               //发送消息NSS头
	unsigned int m_uiNeedXml;               //是否需要返回XML
	int m_iNetElemType;                    //需要发送的网元
    unsigned int m_iReqLength;             //请求字串长度，仅用于发送结构体；其余不需要
    char* m_pReqData;                      //请求数据指针 
	std::string m_strDomainCode;           //域编码
	std::string m_strCameraCode;           //摄像机编码 
	std::string m_strReqData;              //请求XML字串指针

public:
	CSendNssMsgInfo()
		: m_usReqID(0)
		, m_uiNeedXml(0)
		, m_iNetElemType(0)
        , m_iReqLength(0)
        , m_pReqData(NULL)
	{}

	unsigned short GetReqID()const { return m_usReqID; };
	void SetReqID(unsigned short usReqID){ m_usReqID = usReqID; };
	unsigned int GetNeedXml()const { return m_uiNeedXml; };
	void SetNeedXml(unsigned int uiNeedXml){ m_uiNeedXml = uiNeedXml; };
	int GetNetElemType()const { return m_iNetElemType; };
	void SetNetElemType(int usNetElemType){ m_iNetElemType = usNetElemType; };
    unsigned int GetReqLength() const { return m_iReqLength; };
    void SetReqLength(unsigned int uiReqLength) { m_iReqLength = uiReqLength; };
    char* GetReqDataPointer() { return m_pReqData; };
    void SetReqDataPointer(char*& pReqData) { m_pReqData = pReqData; };

	std::string& GetDomainCode(){ return m_strDomainCode; };//lint !e1036 !e1536
	void SetDomainCode(const std::string& usDomainCode){ m_strDomainCode = usDomainCode; };
	std::string& GetCameraCode(){ return m_strCameraCode; };//lint !e1036 !e1536
	void SetCameraCode(const std::string& usCameraCode){ m_strCameraCode = usCameraCode; };
	std::string& GetReqData(){ return m_strReqData; };//lint !e1036 !e1536
	void SetReqData(const std::string& strReqData){ m_strReqData = strReqData; };
	
};

typedef enum EM_ADD_LINK_STATUS
{
	TYPE_ADD_CLOSECOUNT = 0,
	TYPE_ADD_RELINK
}ADD_LINK_STATUS;

class CUserMgr
{
public:
	CUserMgr(void);
	~CUserMgr(void);

    /////////////////////////////提供管理类对象对外暴露的接口begin///////////////////////////
    CUpgradeMgr& GetUpgradeMgr(){return m_UpgradeMgr;}; //lint !e1536

    // 获取登录管理类对象;
    CLoginMgr& GetLoginMgr(){ return m_loginObj; };//lint !e1536

    // 获取用户信息管理类对象;
    CUserInfoMgr& GetUserInfoMgr(){ return m_UserInfoMgr; };//lint !e1536

    // 获取用户组管理类对象;
    CUserGroupMgr& GetUserGroupMgr(){ return m_UserGroupMgr; };//lint !e1536

    // 获取角色管理类对象;
    CRoleMgr& GetRoleMgr(){ return m_RoleMgr; };//lint !e1536

    // 获取录像计划管理类对象;
    CRecordPlanMgr& GetRecordPlanMgr(){ return m_RecordPlanMgr; };//lint !e1536

    // 获取录像书签管理类对象;
    CRecordBookmark& GetRecordBookmark(){ return m_RecordBookMark; };//lint !e1536

	 // 获取实况书签管理类对象;
    CRealPlayBookmark& GetRealPlayBookmark(){ return m_RealPlayBookMark; };//lint !e1536

    // 获取录像列表查询管理类对象;
    CRecordList& GetRecordList(){ return m_RecordList; };//lint !e1536

    // 获取设备组管理类对象;
    CDeviceGroupMgr& GetDeviceGroupMgr(){ return m_DeviceGroupMgr; };//lint !e1536

    // 获取设备参数配置类对象;
    CDeviceParaConfig& GetDeviceParaConfig(){ return m_DeviceParaConfig; };//lint !e1536

    // 获取告警管理类对象;
    CAlarmMgr& GetAlarmMgr(){ return m_AlarmMgr; };//lint !e1536

    // 获取告警防区管理类对象;
    CAlarmAreaMgr& GetAlarmAreaMgr(){ return m_AlarmAreaMgr; };//lint !e1536

    // 获取告警帮助管理类对象;
    CAlarmHelpMgr& GetAlarHelpMgr(){ return m_AlarmHelpMgr; };//lint !e1536

    // 获取告警级别管理类对象;
    CAlarmLevelMgr& GetAlarmLevelMgr(){ return m_AlarmLevelMgr; };//lint !e1536

    // 获取实况管理类对象;
    CRealPlayMgr& GetRealPlayMgr() { return m_RealPlayMgr;}; //lint !e1536

    // 获取录像业务服务管理类对象;
    CRecordService& GetRecordService(){ return m_RecordService; };//lint !e1536

    // 获取回放管理类对象;
    CPlaybackMgr& GetPlaybackMgr(){ return m_PlaybackMgr; };//lint !e1536

    // 获取操作日志管理类对象;
    CLogMgr& GetLogMgr(){ return m_LogMgr; };//lint !e1536

    // 获取告警管理类对象;
    CAlarmLinkageMgr& GetAlarmLinkageMgr(){ return m_AlarmLinkageMgr; }; //lint !e1536

    // 获取智能分析规则管理类对象;
    CIntelligenceAnalysisMgr& GetIntelligenceAnalysisMgr(){ return m_IntelligenceAnalysisMgr; };//lint !e1536

    // 获取域路由管理类对象;
    CDomainRouteMgr& GetDomainRouteMgr() { return m_DomainRouteMgr;}; //lint !e1536

    // 获取云台管理类对象;
    CPtzControl& GetPtzControl(){ return m_PtzControl; };//lint !e1536

    // 获取用户数据类对象;
    CUserDataMgr& GetUserDataMgr(){ return m_UserDataMgr; };//lint !e1536

    // 移动侦测管理类对象;
    CMotionDetection& GetMotionDetection(){ return m_MotionDetection; };//lint !e1536

    // 获取设备管理类对象;
    CDeviceMgr& GetDeviceMgr() { return m_DeviceMgr; };  //lint !e1536

    // 获取自动恢复类对象;
    CAutoResume& GetAutoResume() { return m_AutoResume;}; //lint !e1536
	
	// 获取电视墙管理类对象;
	CTVWallMgr& GetTVWallMgr() { return m_TVWallMgr;}; //lint !e1536

	// 获取录像备份管理类对象
	CRecordBackupMgr& GetRecordBackupMgr() {return m_RecordBackupMgr;}; //lint !e1536

	// 获取图像抓拍管理类对象;
	CCapturePicture& GetCapturePicture(){ return m_CapturePicture;};//lint !e1536

	// 获取客户端网管类对象;
	CClientNetworkMgr& GetClientNetworkMgr(){ return m_ClientNetworkMgr;};//lint !e1536

    // 获取音频管理类对象;
    CAudioMgr& GetAudioMgr() { return m_AudioMgr;}; //lint !e1536

	CDownloadMgr& GetDownLoadMgr() { return m_DownLoadMgr; };//lint !e1536

	// 获取智真管理类对象
	CTelepresenceMgr& GetTelepresenceMgr() { return m_TPMgr; };//lint !e1536

    // 获取报表管理类对象
    CReportMgr& GetReportMgr(){ return m_ReportMgr; };//lint !e1536
	
    /////////////////////////////提供管理类对象对外暴露的接口end///////////////////////////

    // 设置SDKInterface单实例对象;
	void SetSingleSvrProxy(void *pSingleSvrProxy);

    void* GetSingleSvrProxy(){return m_pSingleSvrProxy; };

    // 设置该UserMgr绑定的SessionID号;
	void SetSessionID(int iSessionID){ m_iSessionID = iSessionID; };

    // 获取该UserMgr对应的SessionID
	int GetSessionID() const{ return m_iSessionID; };

    // 获取SDK保存的用户信息，用于其它业务
    CUserInfo* GetUserInfoEx();

    //获取loginId
    char* GetLoginId();
    
    // 获取UserID
    IVS_UINT32 GetUserID() const;

    // 用户登录
    IVS_INT32 Login(const IVS_LOGIN_INFO* pLoginReqInfo);

	// 用户单点登录;
	IVS_INT32 LoginByTicket(const IVS_LOGIN_INFO_EX* pLoginReqInfoEx);

    // 用户注销
    IVS_INT32 Logout(bool bSendLogoutCmd);

    // 设置事件回调函数;
    void SetEventCallBack( EventCallBack pEventCallBack, void *pUserData)
    {
        m_fnEventCallBack = pEventCallBack;
        m_pUserParam = pUserData;
    }

    //获取异常数据
    void* GetEventUserData()
    {
        return m_pUserParam;
    }

    // 获取事件回调函数
    EventCallBack GetEventCallBack() const { return m_fnEventCallBack;}

    // 获取登陆到smu时的linkId
    void GetSMULinkID(std::string& strSmuLinkID);

	//modify by zwx211831增加返回码
	// 获取指定域的smulinkId
	IVS_INT32 GetSMULinkID(std::string& strSmuLinkID, const std::string& strDomainCode);

    // 获取登陆时与omu建立连接的linkId
    void GetOMULinkID(std::string& strOmuLinkID);

    // 获取本域LinkID（消息发送到本域SMU和OMU用到）
    IVS_INT32 GetLocalDomainLinkID(IVS_INT32 iNetElementType, std::string& strLocalDomainLink);

    // 获取与scu建立的连接linkId
    void GetSCULinkID(const std::string& strIP, const std::string& strPort, std::string& strScuLinkID) const;

    // 设置用户本域编码
    void SetDomainCode(const std::string &strDomainCode){ m_strDomainCode = strDomainCode; };

    // 获取用户本域编码
    void GetDomainCode(std::string &strDomainCode)const { strDomainCode = m_strDomainCode; };

    // 根据Rtsp句柄获取播放句柄
    IVS_ULONG GetPlayHandlebyRtspHandle(int iRtspHandle);

    // 根据播放通道号获取播放句柄
    IVS_ULONG GetPlayHandlebyPlayChannel(unsigned long ulPlayChannel);

    // 根据NetSource的通道号获取播放句柄
    IVS_ULONG GetPlayHandlebyNet(unsigned long ulNetChannel);

	// 获取发现前端设备事务ID
	void GetDevTransID(std::string &strTransID)const { strTransID = m_strTransID; };

	// 设置发现前端设备事务ID
	void SetDevTransID(const std::string& strTransID){ m_strTransID = strTransID; };

	// 获取发现前端设备事务ID
	void GetDevServerCode(std::string &strServerCode)const { strServerCode = m_strServerCode; };

	// 设置发现前端设备事务ID
	void SetDevServerCode(const std::string& strServerCode){ m_strServerCode = strServerCode; };
	 /******************************************************************
     function   : GetSameSuperDomainCode
     description: 获取最近的父域节点
     input      : strDomainCode1 域Code1
     input      : strDomainCode2 域Code2
     output     : strSuperDomainCode 父域信息
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    *******************************************************************/
	IVS_INT32 GetSameSuperDomainCode(const std::string& strDomainCode1,const std::string& strDomainCode2,
			std::string& strSuperDomainCode);

	 /******************************************************************
     function   : IsInDomainTree
     description: 判断是否在这个域及下级域
     input      : strDomainCode 域Code
     output     : 
     return     : bool true成功 false失败
    *******************************************************************/
	bool IsInDomainTree(const std::string& strDomainCode);
    
	/******************************************************************
     function   : BuildSMUCmd
     description: 发送NSS请求给登陆的SMU,连接建立成功通知用户
     input      : unsigned short usReqID NSS消息类型
     input      : const char * pData     请求数据
     output     : NA
     return     : BP::CCmd* 成功返回CCmd，失败返回NULL
    *******************************************************************/
    BP::CCmd* BuildSMUCmd(unsigned short usReqID,const char* pData);

     /******************************************************************
     function   : SendCmd
     description: 向指定服务器发送命令;
     input      : pNssMsgInfo     指令结构体
	 input      : uiTimeout       超时时间
	 input      : bTimeout        是否需要设置超时时间(0(默认):不需要设置;1:需要设置)
     output     : strNSSRsp       消息响应指令
	 output     : iNeedRedirect   是否需要重定向
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    *******************************************************************/
    IVS_INT32 SendCmd(CSendNssMsgInfo& sendNssMsgInfo,std::string &strNSSRsp,IVS_INT32& iNeedRedirect, uint32_t uiTimeout = 35, IVS_BOOL bTimeout = 0);
    IVS_INT32 ProcrssEnableLinkNode(CLinkInfo& oLinkIdInfo);
	 /******************************************************************
     function   : SendRedirectCmd
     description: 发起重定向服务
     input      : pNssMsgInfo     指令结构体
     output     : strNSSRsp       消息响应指令
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    *******************************************************************/
	IVS_INT32 SendRedirectServe(CSendNssMsgInfo& sendNssMsgInfo,std::string& strNSSRsp);

	/******************************************************************
	 function   : SendSynCmd
	 description: 发送同步命令,构造并发送Cmd
	 input      : const SEND_NSS_MSG_INFO * pNssMsgInfo 发送消息结构体
	 input      : const std::string & strLinkID         指定发送的连接
	 input      : uiTimeout       超时时间
	 input      : bTimeout        是否需要设置超时时间(0(默认):不需要设置;1:需要设置)
	 output     : std::string & strNSSRsp               返回响应
	 return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
	*******************************************************************/
	IVS_INT32 SendSynCmd(CSendNssMsgInfo& sendNssMsgInfo, const std::string& strLinkID,std::string& strNSSRsp, uint32_t uiTimeout = 35, IVS_BOOL bTimeout = 0);

    // 异步发送消息
    IVS_INT32 SendAsynMsg(const IVS_CHAR* pTargetDomainCode, 
        IVS_UINT32 uiReqID, 
        const std::string& strTransID,
        CXml* pSendXm);
	/******************************************************************
	 function   : GetCameraBriefInfobyCode
	 description: 通过摄像机Code获取摄像机具体信息,默认传入的查询条件strCameraCode是不带域编码
	 input      : const std::string & strCameraCode         摄像机编码
	 input      : bool bHasDomainCode                       指定摄像机编码中是否存在域编码
	 output     : IVS_CAMERA_BRIEF_INFO & stCameraBriefInfo 摄像机信息
	 return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
	*******************************************************************/
	IVS_INT32 GetCameraBriefInfobyCode(const std::string& strCameraCode, IVS_CAMERA_BRIEF_INFO& stCameraBriefInfo, bool bHasDomainCode = false);
	
	/******************************************************************
	 function   : GetCameraCodebyCode
	 description: 在摄像机列表中通过不带域的摄像机编码，获取带域的摄像机编码
	 input      : const std::string & strCameraCode         摄像机编码
	 output     : std::string & strCameraCodeWithDomainCode 拼装了域编码的摄像机编码
	 return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
	*******************************************************************/
	IVS_INT32 GetCameraCodebyCode(const std::string& strCameraCode, std::string& strCameraCodeWithDomainCode);

    /******************************************************************
     function   : LinkOpenSuccessNotify
     description: 连接建立通知
     input      : const std::string & strLinkId 连接ID
     output     : NA
     return     : void
    *******************************************************************/
    void LinkOpenSuccessNotify(const std::string &strLinkId);

	/******************************************************************
	 function   : AddLinkIdToBusinessResumeList
	 description: 增加LinkID到链表中;
	 input      : const std::string & strLinkID     LinkID
	 input      : int iNetElemType                  网元类型
	 input      : const std::string & strIP         IP地址
	 input      : int iPort                         端口
	 input      : int iAddType                      增加类型
	 output     : NA
	 return     : void
	*******************************************************************/
	void AddLinkIdToBusinessResumeList(const std::string& strLinkID,int iNetElemType,const std::string& strIP,int iPort,int iAddType);

	/******************************************************************
	 function   : RemoveLinkIDfromResumeList
	 description: 从链表中移除LinkID
	 input      : const std::string & strLinkID LinkID
	 input      : int iAddType                  增加类型
	 output     : NA
	 return     : void
	*******************************************************************/
	// void RemoveLinkIDfromResumeList(const std::string& strLinkID,int iNetElemType,const std::string& strIP,int iPort,int iAddType);

    /******************************************************************
     function   : GetServiceTypebyPlayHandle
     description: 根据播放句柄获取服务类型
     input      : IVS_ULONG ulPlayHandle            播放句柄
     output     : IVS_SERVICE_TYPE & enServiceType  业务类型
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    *******************************************************************/
    IVS_INT32 GetServiceTypebyPlayHandle(IVS_ULONG ulPlayHandle, IVS_SERVICE_TYPE& enServiceType);

	/******************************************************************
     function   : SetPlaySecretKeybyRtspHandle
     description: 根据获取的RTSP Announce消息，获取视频解密密钥
     input      : int iRtspHandle    rtsp句柄
				  char* pSdp         SDP消息
 
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    *******************************************************************/
	IVS_INT32 SetPlaySecretKeybyRtspHandle(int iRtspHandle,const char* pSdp);

	/**************************************************************************
	* function   : GetVideoNumAndResource
	* description: 查询MU的资源预算是否达到上限及当前摄像机视频流是否被引用
	* input      : pCameraCode       摄像机编码
	* output     : bReachLimit       是否达到上限：0-未达到 1-已达到
				   bWhetherCited     是否被引用：0-没有引用 1-有被引用
	* return     : 成功返回0；失败返回负数错误码
	* remark     : NA
	**************************************************************************/
	IVS_INT32 GetVideoNumAndResource(const IVS_CHAR* pCameraCode,IVS_BOOL* bReachLimit,
									 IVS_BOOL* bWhetherCited);

    /******************************************************************
     function   : NotifyStreamBufferState
     description: 通知缓冲区状态事件
     input      : IVS_ULONG ulPlayHandle    播放句柄
     input      : IVS_INT32 iBufferState    缓冲区状态
     output     : NA
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败 
    *******************************************************************/
    IVS_INT32 NotifyStreamBufferState(IVS_ULONG ulPlayHandle, IVS_INT32 iBufferState);

	IVS_VOID StopNotifyBufEvent(IVS_ULONG ulPlayHandle);

	/******************************************************************
	 function   : RecollectCameraDomainCodes2Map
	 description: 将域编码list表根据域编码筛选分类（已考虑代理情况）;
	 input      : IVS_UINT32 uiCameraNum
	 input      : const IVS_DEVICE_CODE * pCameraCodeList
	 output     : DeviceCodeMap & deviceCodeMap
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 RecollectCameraDomainCodes2Map(IVS_UINT32 uiCameraNum,const IVS_DEVICE_CODE* pCameraCodeList, DeviceCodeMap& deviceCodeMap);

	/*****************************************************************
	     function   : BuildCmd
	     description: 获取通道号
	     input     : CSendNssMsgInfo & sendNssMsgInfo
	     return     : 
	*****************************************************************/
	CCmd *BuildCmd(CSendNssMsgInfo& sendNssMsgInfo);
private:
	/******************************************************************
     function   : BuildLinkAndDoBusiness
     description: 建立连接并且是发起业务
     input      : const std::string & strDomainCode     域编码
     input      : const std::string & strNvrCode        NVR编码
     output     : IVS_INT32 &iRetCode                   NSS消息返回码
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    *******************************************************************/
    IVS_INT32 BuildLinkAndDoBusiness(const std::string& strDomainCode, const std::string& strNvrCode,CSendNssMsgInfo& sendNssMsgInfo, std::string &strNSSRsp) ;

    /******************************************************************
     function   : SendCmdbyDomainNvrCode
     description: 根据域编码和NVRCode查找LinkRouteMgr找SCU的LinkID
     input      : const std::string & strDomainCode 域编码
     input      : const std::string & strNvrCode    NVR编码
     output     : IVS_INT32 &iRetCode               NSS消息返回码
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    *******************************************************************/
	IVS_INT32 SendCmdbyDomainNvrCode(const std::string& strDomainCode, const std::string& strNvrCode,CSendNssMsgInfo& sendNssMsgInfo, std::string &strNSSRsp) ;

    /******************************************************************
     function   : SendCmdbyReDirect
     description: 发重定向请求;
     input      : const IVS_DOMAIN_ROUTE & stDomainRoute    域信息
     input      : pNssMsgInfo                               请求指令结构体
     output     : IVS_INT32 &iRetCode                       NSS消息返回码
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    *******************************************************************/
    IVS_INT32 SendCmdbyReDirect(const IVS_DOMAIN_ROUTE& stDomainRoute,CSendNssMsgInfo& sendNssMsgInfo,std::string &strNSSRsp);

	/******************************************************************
	 function   : StartResumeTimer
	 description: 启动自动恢复定时器
	 output     : NA
	 return     : void
	*******************************************************************/
	void StartResumeTimer();

	/******************************************************************
	 function   : StopResumeTimer
	 description: 停止定时器
	 output     : NA
	 return     : void
	*******************************************************************/
	void StopResumeTimer();

    /******************************************************************
     function   : PrintLog
     description: 循环打印日志，因BP打印的单条日志不能超过2048
     input      : const std::string& strLog
     output     : NA
     return     : void
    *******************************************************************/
    void PrintLog(const std::string& strLog) const;

public:
    // 设置水印数据信息
    void SetWaterMarkInfo(IVS_ULONG ulPlayHandle, ST_WATERMARK_ALARM_INFO& stWaterMarkInfo);

    // 获取水印数据信息
    void GetWaterMarkInfo(IVS_ULONG ulPlayHandle, ST_WATERMARK_ALARM_INFO &stWaterMarkInfo);

    void RemoveWaterMarkInfoFromMap(IVS_ULONG ulPlayHandle);

    bool GetRelinkFlag(){return m_bRelink;};

	/******************************************************************
     function		: SetReceiveOMUAlarm
     description	: 设置是否接收OMU设备类告警
     input				: bReceive true为接收，false为不接收
     output			: NA
     return			: NA
    *******************************************************************/
	void SetReceiveOMUAlarm(IVS_BOOL bReceive);

	/******************************************************************
     function		: GetReceiveOMUAlarm
     description	: 获取是否接收OMU设备类告警的标识
     input				: NA
     output			: NA
     return			: IVS_BOOL 是否接收OMU设备类告警的标识
    *******************************************************************/
	IVS_BOOL GetReceiveOMUAlarm(void) {return m_bReceiveOMUAlarm;}

	/******************************************************************
     function		: GetSSOResumeStartTime
     description	: 获取重连起始时间
     input				: NA
     output			: NA
     return			: time_t
    *******************************************************************/
	time_t GetResumeStartTime(void) const {return m_ResumeStartTime;}

	/******************************************************************
     function		: UpdateSSOResumeStartTime
     description	: 更新重连起始时间
     input				: NA
     output			: NA
     return			: time_t
    *******************************************************************/
	void UpdateResumeStartTime(const time_t &ltime);

private:
	CLoginMgr           m_loginObj;             // 登录管理
	CUserInfoMgr        m_UserInfoMgr;          // 用户信息管理类;
	CUserGroupMgr       m_UserGroupMgr;         // 用户组管理类;
	CRoleMgr            m_RoleMgr;              // 角色管理类;
	CRecordPlanMgr      m_RecordPlanMgr;        // 录像计划管理类;
	CRecordBookmark     m_RecordBookMark;       // 录像书签管理类;
	CRealPlayBookmark   m_RealPlayBookMark;     // 实况书签管理类
	CRecordList         m_RecordList;           // 录像列表类;
	CDeviceGroupMgr     m_DeviceGroupMgr;       // 设备组管理类
	CDeviceParaConfig   m_DeviceParaConfig;     // 设备参数的管理
	CAlarmMgr           m_AlarmMgr;             // 告警管理类
	CAlarmAreaMgr       m_AlarmAreaMgr;         // 告警防区管理类
	CAlarmHelpMgr       m_AlarmHelpMgr;         // 告警帮助管理类
	CAlarmLevelMgr      m_AlarmLevelMgr;        // 告警级别管理类
    CRealPlayMgr        m_RealPlayMgr;          // 实况
	CRecordService      m_RecordService;        // 录像管理类
	CPlaybackMgr        m_PlaybackMgr;          // 回放管理类
	CLogMgr             m_LogMgr;               // 查询操作日志
	CAlarmLinkageMgr    m_AlarmLinkageMgr;      // 告警联动策略
    CIntelligenceAnalysisMgr    m_IntelligenceAnalysisMgr; // 查询已设智能分析规则摄像机列表
	CDomainRouteMgr     m_DomainRouteMgr;       // 域路由管理类
    CPtzControl         m_PtzControl;           // 云镜控制类
    CUserDataMgr        m_UserDataMgr;          // 数据上传下载
	CMotionDetection    m_MotionDetection;      // 运动检测
    CDeviceMgr          m_DeviceMgr;            // 设备管理类
    CAutoResume         m_AutoResume;	        // 自动恢复类
	CTVWallMgr			m_TVWallMgr;			// 电视墙管理类
	CRecordBackupMgr    m_RecordBackupMgr;     // 录像备份管理类
	CCapturePicture     m_CapturePicture;       // 抓拍控制类
	CClientNetworkMgr   m_ClientNetworkMgr;     // 客户端网管类
    CAudioMgr           m_AudioMgr;             // 语音对讲广播管理类
	CDownloadMgr        m_DownLoadMgr;       
	CTelepresenceMgr    m_TPMgr;				//智真对接管理类
    CReportMgr          m_ReportMgr;            // 报表管理类
    CUpgradeMgr       m_UpgradeMgr;         // 升级类
private:
	int                 m_iSessionID;           // 用户及登录相关信息
    std::string         m_strDomainCode;        // 域名Code
    void*               m_pUserParam;           // 由回调函数传入的用户参数;
	void*               m_pSingleSvrProxy;      // 保存SDKInterface指针
	CNVSTimer           m_ResumeTimerInst;      // 自动恢复定时器
	bool                m_bAutoResume;          // 自动恢复开关
	CNVSTimer           m_CloseLinkIDInst;      // 断连的linkID的计数定时器
	CCloseLinkCount     m_CloseLinkIDCount;     // 计数的统计类
	bool                m_bCloseLinkIDCount;    // 计数统计的开关
	CNVSTimer           m_RelinkInst;           // 重新建立连接的计数器
	CReLink             m_Relink;               // 重新建连接的类
	bool                m_bRelink;              // 重连开关;
	IVS_BOOL m_bReceiveOMUAlarm;		//是否接收OMU设备类告警，0为不接收，非0为接收
    EventCallBack       m_fnEventCallBack;      // 事件回调函数;
	std::string         m_strTransID;           // 获取到的事务ID
	std::string         m_strServerCode;        // 获取到的ServerCode
	time_t					m_ResumeStartTime;		//单点登录开始时间

    MAP_WATERMARK_ALARM_INFO m_mapWaterMarkInfo;// 水印数据信息与窗格句柄对应表 
    VOS_Mutex*          m_pWaterMarkInfoMutex;  // 水印数据信息锁
};
#endif
