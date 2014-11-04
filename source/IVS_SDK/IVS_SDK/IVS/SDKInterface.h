/********************************************************************
 filename    :    SDKInterface.h
 author      :    z00201790
 created     :    2012/10/23
 description :    管理用户实例,SDK最大支持64个用户实例。
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/17 初始版本
*********************************************************************/

#ifndef __SDK_INTERFACE_H__
#define __SDK_INTERFACE_H__

#include "vos.h"      // mutex
#include "hwsdk.h"    // type definition
#include "SDKDef.h"
#include "EventMgr.h"
#include "UserMgr.h"

class CUserMgr;
typedef std::map <int, CUserMgr*>        MAP_SESSEIONID_USERMGR;
typedef MAP_SESSEIONID_USERMGR::iterator MAP_SESSEIONID_USERMGR_ITER;

typedef struct {
    int iTimeOut;      // 同步发送消息超时时间
} ST_SDK_DEFAULT_CFG;


class CSDKInterface
{
public:
    CSDKInterface(void);
	~CSDKInterface(void);

	/******************************************************************
	 function   : Init
	 description: 初始化
	 output     : NA
	 return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
	*******************************************************************/
	IVS_INT32 Init();

	/******************************************************************
	 function   : Cleanup
	 description: 清理
	 output     : NA
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
	*******************************************************************/
	IVS_INT32 Cleanup();

	/******************************************************************
	 function   : SetEventCallBack
	 description: 设置SDK的回调事件
	 input      : IVS_INT32 iSessionID              对应的用户管理类的SessionID
	 input      : EventCallBack fEventCallBack      事件回调函数指针
	 input      : void * pUserData                  用户数据
	 output     : NA
	 return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
	*******************************************************************/
	IVS_INT32 SetEventCallBack(IVS_INT32 iSessionID, EventCallBack fEventCallBack = NULL, void *pUserData = NULL);

public:
	/******************************************************************
	 function   : GetLoginRspInfo
	 description: 获取登录信息
	 input      : IVS_INT32 iSessionID              对应的用户管理类的SessionID
	 input      : LOGIN_RSP_INFO * pLoginRspInfo    
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	//IVS_INT32 GetLoginRspInfo(IVS_INT32 iSessionID, LOGIN_RSP_INFO* pLoginRspInfo);

	/******************************************************************
	 function   : GetSessionIDByLoginID
	 description: 根据loginID查询 SessionID
	 input      : const string & strLoginID  // 登录ID
     output     : NA
	 return     : IVS_INT32 SessionID值
	*******************************************************************/
	IVS_INT32 GetSessionIDByLoginID(const string &strLoginID);

	/******************************************************************
	 function   : GetSessoinID
	 description: 获取会话ID
	 output     : NA
	 return     : int 会话ID
	*******************************************************************/
	int GetSessoinID();

	/******************************************************************
	 function   : FreeSessoinID
	 description: 释放SessionID
	 input      : int iSessionID    会话ID
	 output     : NA
	 return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
	*******************************************************************/
	IVS_INT32 FreeSessoinID(int iSessionID);

	/******************************************************************
	 function   : AddSessoinID_SingleSvrProxyToMap
	 description: 插入SessionID到map中
	 input      : int iSessionID                会话ID
	 input      : CUserMgr * pSingleSvrProxy    用户管理对象;
	 output     : NA
	 return     : void
	*******************************************************************/
	void AddSessoinID_SingleSvrProxyToMap(int iSessionID, CUserMgr *pSingleSvrProxy);

	/******************************************************************
	 function   : GetUserMgr
	 description: 根据SessionID获取用户对象
	 input      : int iSessionID        会话ID
	 output     : NA
	 return     : CUserMgr *            用户管理对象;
	*******************************************************************/
	CUserMgr *GetUserMgr(int iSessionID);

    /******************************************************************
     function   : Login
     description: 用户登录
     input      : const IVS_LOGIN_INFO * pLoginReqInfo  登录信息
     input      : IVS_INT32 * pSessionId                回话ID
     output     : NA
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    *******************************************************************/
    IVS_INT32 Login(const IVS_LOGIN_INFO* pLoginReqInfo, IVS_INT32* pSessionId);


	/******************************************************************
     function   : LoginByTicket
     description: 用户单点登录;
     input      : const IVS_LOGIN_INFO_EX * pLoginReqInfoEx  单点登录信息;
     output      : IVS_INT32 * pSessionId                会话ID;
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败;
    *******************************************************************/
    IVS_INT32 LoginByTicket(const IVS_LOGIN_INFO_EX* pLoginReqInfoEx, IVS_INT32* pSessionId);


    /**************************************************************************
    * name       : WinUserLogin
    * description: 用户登录
    * input      : pUserName 用户名
                   pPassword 密码
                   pIP       服务器IP地址
                   iPort     服务器端口
    * output     : NA
    * return     : 成功返回SessionID，即会话ID，SessionID由SDK内部产生，大于0的整数，
                   作为其他接口的入参，用来区分访问的服务器，最多登录128个服务器，失败
                   返回负数错误码
    * remark     : NA
    **************************************************************************/
    IVS_INT32 WinUserLogin(const IVS_LOGIN_INFO* pLoginReqInfo, IVS_INT32* pSessionId);

    // 用户注销
    IVS_INT32 Logout(int iSessionID, bool iSendLogoutCmd = true);

    // 重置已登录用户
    int RemoveUser(const string &strUserName, const string &strIP, const string &strDomainName);

	// 重置已单点登录的用户;
	int RemoveUserByTicketID(const string &strTicketID, const string &strIP, const string &strDomainName);

  	// 释放SDK内部申请的内存
	IVS_INT32 ReleaseBuffer(IVS_CHAR *pBuffer)const;
	
	//通过rtsp句柄获取用户管理类对象
	CUserMgr* GetUserMgrbyRtspHandle(const long iRtspHandle);

    void AddTransIDChannelListRetriveStatusMap(std::string& strTransID, IVS_CHANNLELIST_RETRIVE_STATUS_INFO& channelListRetriveStatus);
    void RemoveChannelListRetriveStatusByTransID(std::string& strTransID);
    IVS_INT32 GetChannelListRetriveStatusByTransID(std::string& strTransID, IVS_CHANNLELIST_RETRIVE_STATUS_INFO& channelListRetriveStatus);
    
private:
    // 初始化BP
    IVS_INT32 BPInit();

    // 清理BP
    IVS_INT32 BPCleanup();
    
    // 加载本地配置参数
    IVS_INT32 LoadLocalConfig();

protected:
	char                    m_SessionIDFlagArray[SDK_CLIENT_TOTAL]; //SessionID标志数组 0表示未分配  1表示已分配
	VOS_Mutex               *m_pMutexArrayLock;                     // m_SessionIDFlagArray锁
	MAP_SESSEIONID_USERMGR  m_mapSessionIDToSingleSvrProxy;         // 存储用户实例
	VOS_Mutex               *m_pMutexMapLock;                       // m_mapSessionIDToSingleSvrProxy锁
	VOS_Mutex				*m_pMutexLoginLock;						//普通登录接口的互斥锁
	VOS_Mutex				*m_pMutexLoginByTktLock;			//单点登录接口的互斥锁
	CUserMgr                m_UserMgrArray[SDK_CLIENT_TOTAL];       // 128个用户实例
	IVS_BOOL                m_bInit;                                // 是否初始化
    IVS_BOOL                m_bBplogInit;                           // 日志初始化结果，初始化日志失败，不用调用BP清理日志方法;


    std::map<std::string, IVS_CHANNLELIST_RETRIVE_STATUS_INFO> m_ChannelListRetriveStatusMap;
    VOS_Mutex               *m_pRetriveStatusMapLock;                       // m_mapSessionIDToSingleSvrProxy锁


private: 
    CEventMgr               m_eventMgr;                             // 事件管理类;
};
#endif  //__SDK_INTERFACE_H__
