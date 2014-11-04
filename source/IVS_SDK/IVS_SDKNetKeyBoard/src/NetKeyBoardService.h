/********************************************************************
	filename	: 	NetKeyBoardService.h
	author		:	s00194833
	created		:	2011/12/07	
	description	:	网络键盘数据解析
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/12/07 初始版本
*********************************************************************/
#ifndef NET_KEYBOARD_H
#define NET_KEYBOARD_H

/*!< Socket头文件，线程启动socket通讯使用*/
#include "SocketService.h"
/*!< 公共宏、结构体头文件,回调函数使用*/
#include "KBInternalDef.h"
#include "bp_os.h"
#include "NVSTimer.h"


#ifdef _UT_TEST_
#define  protected public
#define  private public
#endif

//网络键盘传入的数据
typedef struct stNKBData
{
    unsigned char btKeyValue;                       //键值
    unsigned char btKeyLabel;                       //标识
    unsigned char btReservedFirst;                  //保留1
    unsigned char btReservedSecond;                 //保留2
    stNKBData()
    {
        btKeyValue = 0;
        btKeyLabel = 0;
        btReservedFirst = 0;
        btReservedSecond = 0;
    }
    ~stNKBData()
    {
        btKeyValue = 0;
        btKeyLabel = 0;
        btReservedFirst = 0;
        btReservedSecond = 0;
    }
}ST_NKB_DATA;

//网络键盘传入的数据(天地伟业)
typedef struct stTDNKBData
{
    unsigned char btKeyValue;                       //键值
    unsigned char btKeyLabel;                       //标识
    unsigned char btReservedFirst;                  //保留1(摇杆:控制方式位+方向命令字+上下速度+左右速度+变倍速度)
    unsigned char btReservedSecond;                 //保留2
    unsigned char btReservedThird;                  //保留3
    stTDNKBData()
    {
        btKeyValue = 0;
        btKeyLabel = 0;
        btReservedFirst = 0;
        btReservedSecond = 0;
        btReservedThird = 0;
    }
    ~stTDNKBData()
    {
        btKeyValue = 0;
        btKeyLabel = 0;
        btReservedFirst = 0;
        btReservedSecond = 0;
        btReservedThird = 0;
    }
}ST_TD_NKB_DATA;

class CNetKeyBoardService;
class CKBTrigger : public ITrigger
{
public:
	CKBTrigger(void){m_lpCallBack = NULL; m_pNetKeyBoardService = NULL;};
	~CKBTrigger(void){};
	virtual void onTrigger(void *pArg, ULONGLONG ullScales, TriggerStyle enStyle);
	//设置电视墙管理对象
	void SetNetKeyBoardService(CNetKeyBoardService* pNetKeyBoardService, LPCallBack lpCallBack)
	{
		m_lpCallBack = lpCallBack;
		m_pNetKeyBoardService = pNetKeyBoardService;
	};
private:
	LPCallBack m_lpCallBack;                          //回调函数指针
	CNetKeyBoardService* m_pNetKeyBoardService;		//网络键盘对象
};






class CNetKeyBoardService
{
public:
    static CNetKeyBoardService* GetInstance();
    static void ReleaseInstance();

    /**************************************************************************
    * name       : ListenSocket
    * description: 端口监听线程
    * input      : lpParameter 回调函数地址
    * output     : NA
    * return     : IVS_SUCCEED - 成功，其他 - 失败
    * remark     : NA
    **************************************************************************/
    static DWORD __stdcall ListenSocket(LPVOID lpParameter);

    /**************************************************************************
    * name       : ListenSocket
    * description: 端口监听线程
    * input      : lpParameter 回调函数地址
    * output     : NA
    * return     : IVS_SUCCEED - 成功，其他 - 失败
    * remark     : NA
    **************************************************************************/
    static DWORD __stdcall ListenUDPSocket(LPVOID lpParameter);

    /**************************************************************************
    * name       : ReleaseListenThread
    * description: 释放监听线程
    * input      : hRleaseThread 监听线程句柄
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    static void ReleaseListenThread(HANDLE hRleaseThread);

    /**************************************************************************
    * name       : IniTcpServer
    * description: 初始化网络键盘的TCP服务器监听套接字
    * input      : NA
    * output     : NA
    * return     : IVS_SUCCEED - 成功,其他 - 失败
    * remark     : NA
    **************************************************************************/
    int IniTcpServer();

    /**************************************************************************
    * name       : IniUdpServer
    * description: 初始化网络键盘的UDP服务器监听套接字
    * input      : NA
    * output     : NA
    * return     : IVS_SUCCEED - 成功,其他 - 失败
    * remark     : NA
    **************************************************************************/
    int IniUdpServer();

    /**************************************************************************
    * name       : ReleaseListenServer
    * description: 释放监听套接字，供启动监听线程失败时使用
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void ReleaseListenServer();

	//向CU发送停止摇杆的消息
	void StopPTZDirectionControlUDP(LPCallBack lpCallBack);

private:

    CNetKeyBoardService();

    ~CNetKeyBoardService();

    /**************************************************************************
    * name       : IndentifyAction
    * description: 判别网络键盘按键
    * input      : pszRecieveData 接收的数据
                   iLength 接收数据长度
                   lpCallBack 回调函数
    * output     : NA
    * return     : IVS_SUCCEED - 成功，其他 - 失败
    * remark     : NA
    **************************************************************************/
    int IndentifyAction(char *pszRecieveData,int iLength,LPCallBack lpCallBack);

    /**************************************************************************
    * name       : PraseData
    * description: 解析获得数据是否符合协议要求
    * input      : pszRecieveData 接收的数据，iLength 接收数据长度
    * output     : NA
    * return     : IVS_SUCCEED - 成功，其他 - 失败
    * remark     : NA
    **************************************************************************/
    int PraseData(char *pszRecieveData,int iLength);

    /**************************************************************************
    * name       : DealLogData
    * description: 处理登录相关消息
    * input      : pszRecieveData 网络键盘发送的数据流
                   iLength 数据流长度
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void DisposeLogOnData(char *pszRecieveData, int iLength);

    /**************************************************************************
    * name       : IsLogin
    * description: 判断是否为登录消息
    * input      : NA
    * output     : NA
    * return     : true 是登录消息，false 不是登录消息
    * remark     : NA
    **************************************************************************/
    bool IsLogin();

    /**************************************************************************
    * name       : IsLogout
    * description: 判断是否为注销消息
    * input      : NA
    * output     : NA
    * return     : true 是注销消息，false 不是注销消息
    * remark     : NA
    **************************************************************************/
    bool IsLogout();

    /**************************************************************************
    * name       : LoginSuccess
    * description: 网络键盘登录成功数据处理
    * input      : pszRecieveData 网络键盘发送的数据流
                   iLength 数据流长度
    * output     : NA
    * return     : IVS_SUCCEED 返回数据成功，其他 失败
    * remark     : NA
    **************************************************************************/
    int LoginSuccess(char *pszRecieveData, int iLength);

    /**************************************************************************
    * name       : LoginFail
    * description: 网络键盘登录失败数据处理
    * input      : pszRecieveData 网络键盘发送的数据流
                   iLength 数据流长度
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void LoginFail(char *pszRecieveData, int iLength);

    /**************************************************************************
    * name       : IsDirectionControl
    * description: 判断是否是云台方向控制
    * input      : NA
    * output     : NA
    * return     : true 是云台方向控制，false 不是云台方向控制
    * remark     : NA
    **************************************************************************/
    int IsDirectionControl();
    /**************************************************************************
    * name       : PTZDirectionControl
    * description: 网络键盘云台方向控制数据处理
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void PTZDirectionControl(LPCallBack lpCallBack);

    /**************************************************************************
    * name       : PTZOtherControl
    * description: 网络键盘云台光圈、变焦、变倍控制数据处理
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void PTZOtherControl(LPCallBack lpCallBack);

    /**************************************************************************
    * name       : PTZVideoExchange
    * description: 网络键盘视频切换数据处理
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void PTZVideoExchange(LPCallBack lpCallBack);

	/**************************************************************************
    * name       : UpdateNum
    * description: 更新通道值
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
	void UpdateNum();

	/**************************************************************************
    * name       : PTVideoZCancel
    * description: 取消电视上墙
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
	void PTVideoZCancel(LPCallBack lpCallBack);

    void DealAction(char *pszRecieveData, int iLength, int i, LPCallBack lpCallBack);

    void DealOtherAction();

    void DealPTZDirection(LPCallBack lpCallBack);

    void DealPTZDirectionCallBack(int iCallBackKeyValue, LPCallBack lpCallBack);

    /************************天地伟业协议报文处理*****************************/
    /**************************************************************************
    * name       : IndentifyActionUDP
    * description: 判别网络键盘按键
    * input      : pszRecieveData 接收的数据
                   iLength 接收数据长度
                   lpCallBack 回调函数
    * output     : NA
    * return     : IVS_SUCCEED - 成功，其他 - 失败
    * remark     : NA
    **************************************************************************/
    int IndentifyActionUDP(char *pszRecieveData,int iLength,LPCallBack lpCallBack);

    /**************************************************************************
    * name       : PraseDataUDP
    * description: 解析获得数据是否符合协议要求
    * input      : pszRecieveData 接收的数据，iLength 接收数据长度
    * output     : NA
    * return     : IVS_SUCCEED - 成功，其他 - 失败
    * remark     : NA
    **************************************************************************/
    int PraseDataUDP(char *pszRecieveData,int iLength);

    /**************************************************************************
    * name       : PTZOtherControlUDP
    * description: 网络键盘云台光圈、变焦、变倍控制数据处理
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void PTZOtherControlUDP(LPCallBack lpCallBack);

    /**************************************************************************
    * name       : PTZDirectionControlUDP
    * description: 网络键盘云台方向控制数据处理
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void PTZDirectionControlUDP(LPCallBack lpCallBack);

    /**************************************************************************
    * name       : PTZVideoExchangeUDP
    * description: 网络键盘视频切换数据处理
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void PTZVideoExchangeUDP(LPCallBack lpCallBack);

	/**************************************************************************
    * name       : PTVideoZCancelUDP
    * description: 取消电视上墙
    * input      : lpCallBack 回调函数
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
	void PTVideoZCancelUDP(LPCallBack lpCallBack);

	/**************************************************************************
	* name       : PresetControlUDP
	* description: 网络键盘预置位控制数据处理
	* input      : lpCallBack 回调函数
	* output     : NA
	* return     : NA
	* remark     : NA
	**************************************************************************/
	void PresetControlUDP(LPCallBack lpCallBack);

	/**************************************************************************
	* name       : PresetCruiseControlUDP
	* description: 网络键盘预置位巡航调用控制数据处理
	* input      : lpCallBack 回调函数
	* output     : NA
	* return     : NA
	* remark     : NA
	**************************************************************************/
	void PresetCruiseControlUDP(LPCallBack lpCallBack);

	/**************************************************************************
	* name       : PresetCruiseStopControlUDP
	* description: 网络键盘预置位巡航停止控制数据处理
	* input      : lpCallBack 回调函数
	* output     : NA
	* return     : NA
	* remark     : NA
	**************************************************************************/
	void PresetCruiseStopControlUDP(LPCallBack lpCallBack);


    void NoteLastBeatTime();
    time_t GetCurTime();

	void RegisterTimer(LPCallBack lpCallBack);
	void CancelTimer();

public:
    CSocketService m_SocketService;             //网络键盘监听Socket操作对象
    CSocketService m_UDPSocketService;          //网络键盘监听Socket操作对象(天地伟业为UDP协议) 

	CNVSTimer m_timer;                          //网络键盘摇杆定时器
	CKBTrigger m_KBTigger;                      //网络键盘摇杆触发器
	bool m_bTimerReg;                           //定时器是否已经注册了
	bool m_bTimerRun;                           //定时器是否在运行
	
private:
    ST_NKB_DATA m_stNKBData;                    //网络键盘的按键信息数据
    bool m_bIsListen;                           //监听标志位
    bool m_bIsInvalidKey;                       //判断是否为有效按键，用于停止时
    int m_iCurrentMaxUser;                      //当前登录的最大权限用户，在网络键盘登录失败时使用
    int m_iVideoLabel;                          //视频切换标志位
    int m_iVideoNum;                            //视频通道号
    int m_iDecodeNum;                           //解码通道号
    bool m_bTvWallPTZ;                          //电视上墙的网络键盘云台控制
    int m_iWinNum;                              //电视上墙的窗格号
    int m_iScreenNum;                           //屏幕号，电视墙号

    ST_TD_NKB_DATA m_stTDNKBData;               //网络键盘的按键信息数据(天地伟业)
    bool m_bIsTVWall;                           //是否为硬解
    int m_iCameraID;                            //摄像机编号，视频通道号
    int m_iWinID;                               //窗格ID，通道号

    static CNetKeyBoardService *m_pNetKeyBoardService; //网络键盘静态实例

    time_t m_LastBeatTime;                      //记录下最后一次心跳包的时间

    int m_iOldCallBackKeyValue;                 //上一次网络键盘发的云台方向命令
    int m_iOldCallBackTeleWideKeyValue;         //上一次网络键盘发的云台变倍命令
    char m_cOldSpeed;                           //上一次云台方向速度
	char m_cOldTeleWideSpeed;                   //上一次云台变倍速度

	int m_iOldOtherKeyValue;                    //上一次网络键盘云台光圈、变焦、变倍控制数据等命令
};
#endif //NET_KEYBOARD_H
