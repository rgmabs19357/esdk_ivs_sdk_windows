/********************************************************************
 filename    :    SDKDecoderMgr.h
 author      :    g00209332
 created     :    2013/1/18
 description :    SDK解码器管理类,单独线程管理与解码器的连接
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/1/18 初始版本
*********************************************************************/
#pragma once
#include "SDKDef.h"
#include "vos.h"
#include "hwsdk.h"
#include "IVS_SDKTVWall.h"

typedef struct 
{
	IVS_CHAR szDecoderID[IVS_TVWALL_DECODER_CODE_LEN];	//解码器ID
	IVS_CHAR szDecoderIP[IVS_IP_LEN];					//解码器IP
	IVS_USHORT usDecoderPort;							//解码器端口
	IVS_CHAR szUserName[IVS_NAME_LEN];					//解码器登陆名
	IVS_CHAR szPWD[IVS_PWD_LEN];						//登陆密码
	IVS_ULONG ulIdentifyID;								//登陆成功后的ID
	IVS_BOOL bOnline;									//是否在线
}SDK_DECODER_INFO;


enum LINK_MODE
{
	LINK_MODE_AUTO = 0,		// 自动
	LINK_MODE_MANUAL = 1,	// 手动
	LINK_MODE_MIX = 2		// 混合
};

class CSDKDecoderMgr;
typedef std::list<CSDKDecoderMgr*> SDK_DECODER_MGR_LIST;				//解码器管理实例列表 

class CSDKDecoderMgr
{
	typedef std::map<std::string, SDK_DECODER_INFO*> SDK_DECODER_INFO_MAP;	//解码器信息MAP
public:
	CSDKDecoderMgr(void);
	~CSDKDecoderMgr(void);
	// 初始化解码器SDK
	IVS_INT32 InitSDK(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData);
	// 释放解码器SDK
	IVS_INT32 CleanupSDK();
	//// 设置解码器信息
	//IVS_INT32 SetDecoderInfo(const SDK_DECODER_INFO* pDecoderInfo, IVS_UINT32 uiDecoderNum);
protected:
	// 线程调用函数
	static void invoke(void *argc);
	// SDK回调函数
	static IVS_LONG __stdcall SDKEventCallBack(void *argc);
	// 登陆解码器
	IVS_VOID  LoginToDecoder();
	// 获取解码器注册信息
	IVS_INT32 GetDecoderRegInfo(IVS_ULONG /*ulID*/, const std::string& /*strDecoderID*/, 
								const std::string& /*strDecoderIP*/, 
								CUMW_NOTIFY_REMOTE_DECODER_STATUS* pRegInfo) const;
private:
	// 是否包含该登录信息解码器
	IVS_BOOL IsIncludeID(IVS_ULONG ulIdentifyID);
	// 增加一个解码器管理对象
	static IVS_VOID AddDecMgrObj(CSDKDecoderMgr* pDecoder);
	// 移除一个解码器管理对象
	static IVS_VOID RemoveDecMgrObj(const CSDKDecoderMgr* pDecoder);
	// 断连通知
	IVS_VOID NotifyDisConnect(IVS_ULONG ulIdentifyID);
	// 移除解码器信息
	IVS_VOID RemoveDecoderInfo();
	PCUMW_CALLBACK_NOTIFY m_pCallBackFun;	// 回调函数
	void* m_pUserData;						// 用户数据
	SDK_DECODER_INFO_MAP m_decoderInfoMap;	// 已配置的解码器信息
	VOS_Mutex* m_pDecoderInfoMapMutex;		// 解码器信息Map锁
	VOS_Thread* m_pVosThread;				// 线程对象
	IVS_BOOL m_bThreadExit;					// 线程退出
	time_t m_lLastLoginTime;    		    // 上次登录时间
	IVS_BOOL m_bInit;						// 是否初始化了
	static SDK_DECODER_MGR_LIST m_DecMgrObjList;// 解码器管理实例列表。SDK回调函数不能确定哪个实例，只能用静态变量保存
};
