/********************************************************************
	filename	: 	NetKeyBoardService.h
	author		:	s00194833
	created		:	2011/12/07	
	description	:	网络键盘数据解析
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/12/07 初始版本
*********************************************************************/
#include "SocketService.h"
#include "NetKeyBoardService.h"
#include "IVS_Trace.h"

//数据解析错误码
#define WRONG_CMD_WORD                  699002
#define WRONG_LENGHT                    699003
#define WRONG_CHECK                     699004

//接收缓冲去长度
#define BUFFER_LENGTH                     1024

//大华网络键盘的心跳检测时间间隔
#define DAHUA_KB_BEAT_INTERVAL            23

//网络键盘数据中固定数据
#define NKB_CMD_WORD                      0x98
#define NKB_FIRST_ADDR                    0x90
#define NKB_ADDR_CODE                     0xFF
#define NKB_BEAT_WORD                     0xA1

//网络键盘和校验数据位置及长度
#define NKB_DATA_CHECK_START_POS            15
#define NKB_DATA_CHECK_NUMBER                7

//网络键盘数据长度宏
#define NKB_MAX_BYTE                       256
#define NKB_DATA_BASIC_LENGTH               32

//网络键盘扩展数据位置和长度宏
#define NKB_EXTEND_DEFINE_POS                4
#define NKB_EXTEND_DEFINE_LENGTH             4

//网络键盘数据位置宏
#define NKB_KEY_VALUE_POS                   17
#define NKB_KEY_LABEL                       18
#define NKB_KEY_Reserved_FIRST              19

//键按下和弹出宏
#define NKB_KEY_PRESS                        1
#define NKB_KEY_UP                           0
#define TIME_OUT                             2

#define MAX_CHANNEL_NUMBER                  9999
#define SLEEP_TIME                          1000
//功能控制权限
enum enControlPower
{
    CONTROL_PTZ = 0,
    SETUP_SYSTEM,
    COPY_FILE,
    SENIOR_OPTION,
    MAX_CONTROLPOWER = 4,
    LOGIN_POWER = 0x0F
};

//登录权限标识
enum enPowerLabel
{
    GUEST = 0,
    USER,
    ADMINSTRATOR,
    MAX_POWER,
    NKB_LOGOUT = 0xFF
};

//网络键盘键值
enum enNKBKeyValue
{
    NKB_ZERO = 0,
    NKB_NINE = 9,

    NKB_UP,                         //上
    NKB_DOWN,                       //下
    NKB_LEFT,                       //左
    NKB_RIGHT,                      //右
    NKB_CANCEL,                     //取消
    NKB_OK,                         //确认

    NKB_PTZ_STOP = 0x11,            //停止
    NKB_PTZ_MODE_CHANGE = 0x1A,     //软硬解切换

    NKB_FN_FIRST = 0x1F,            //功能键1
    NKB_FN_SECOND,                  //功能键2

    NKB_TELE = 0x22,                //变倍大
    NKB_WIDE,                       //变倍小
    NKB_APERTURE_CLOSE,             //光圈关
    NKB_APERTURE_OPEN,              //光圈开
    NKB_FOCUS_NEAR,                 //近焦
    NKB_FOCUS_FAR,                  //远焦

    NKB_CHAN_CHANGE = 0x33,         //通道切换

    NKB_ROCKER_LEFT_UP = 0x45 ,
    NKB_ROCKER_LEFT_DOWN,
    NKB_ROCKER_RIGHT_UP,
    NKB_ROCKER_RIGHT_DOWN,

    NKB_ROCKER_UP_ZOOM_IN = 0x70,
    NKB_ROCKER_DOWN_ZOOM_IN,
    NKB_ROCKER_LEFT_ZOOM_IN,
    NKB_ROCKER_RIGHT_ZOOM_IN,
    NKB_ROCKER_LEFT_UP_ZOOM_IN,
    NKB_ROCKER_LEFT_DOWN_ZOOM_IN,
    NKB_ROCKER_RIGHT_UP_ZOOM_IN,
    NKB_ROCKER_RIGHT_DOWN_ZOOM_IN,
    NKB_ROCKER_UP_ZOOM_OUT,
    NKB_ROCKER_DOWN_ZOOM_OUT,
    NKB_ROCKER_LEFT_ZOOM_OUT,
    NKB_ROCKER_RIGHT_ZOOM_OUT,
    NKB_ROCKER_LEFT_UP_ZOOM_OUT,
    NKB_ROCKER_LEFT_DOWN_ZOOM_OUT,
    NKB_ROCKER_RIGHT_UP_ZOOM_OUT,
    NKB_ROCKER_RIGHT_DOWN_ZOOM_OUT,

    NKB_LOGIN = 0xFE
};

// 固定数据(天地伟业)
#define TD_NKB_KEY_PROTOFLAG_FIRST         0xE5
#define TD_NKB_KEY_PROTOFLAG_SECOND        0xBE
#define TD_NKB_KEY_FRAME_HEAD              0xFA
#define TD_NKB_KEY_FRAME_TAIL              0xFD

// 网络键盘数据位置宏(天地伟业)
#define TD_NKB_KEY_RSPCTRL_POS                6
#define TD_NKB_KEY_DATALENGTH_POS             8
#define TD_NKB_KEY_FRAMEHEAD_POS             10
#define TD_NKB_KEY_DATA_POS                  14
#define TD_NKB_KEY_DATA_MIN_LEN              19

// 网络键盘键值(天地伟业)
enum enTDNKBKeyValue
{
    TD_NKB_CANCEL = 0x21,                 // 取消
    TD_NKB_OK = 0x22,                     // 确认
	TD_NKB_PRESET = 0x25,                 // 预置位调用
    TD_NKB_CAM = 0x26,                    // 【MON】+【数字键】+【ENTER】指定输出窗口
    TD_NKB_FN_FIRST = 0x36,               //  按【F1】+数字键+【ENTER】键
    TD_NKB_MON = 0x3d,                    // 【CAM】+【数字键】+【ENTER】指定输入通道

    // 镜头控制
    TD_NKB_APERTURE_PLUS = 0x41,          // 光圈+：41    光圈大
    TD_NKB_APERTURE_MINUS,                // 光圈-：42    光圈小
    TD_NKB_TELE,                          // 变倍+：43    变倍大
    TD_NKB_WIDE,                          // 变倍-：44    变倍小
    TD_NKB_FOCUS_NEAR,                    // 聚焦近：45   聚焦近
    TD_NKB_FOCUS_FAR,                     // 聚焦远：46   聚焦远
    TD_NKB_STOP,                          // 镜头控制停止码：47

    // 云台方向控制
    TD_NKB_PU = 0x48,                     // 前端控制 0x48 
    TD_NKB_MOUSE,                         // 鼠标控制 0x49

	TD_NKB_PRESET_CRUISE_STOP = 0x76,     // 预置位巡航停止
	TD_NKB_PRESET_CRUISE_START = 0x77,    // 预置位巡航调用

    TD_NKB_SHIFT_PGM_TVWALL = 0x7d,       // 电视墙硬解 0x7d
    TD_NKB_SHIFT_PGM_CLIENT,              // 客户端软解 0x7e
    
};

// 云台方向标志(天地伟业)
enum enPTZDirectionFlag
{
    TD_PTZ_RIGHT = 0X01,
    TD_PTZ_LEFT = 0X02,
    TD_PTZ_UP = 0X04,
    TD_PTZ_DOWN = 0X08,
    TD_PTZ_TELE = 0X10,                    // 变倍+：    变倍大
    TD_PTZ_WIDE = 0X20,                    // 变倍-：    变倍小
    TD_PTZ_STOP = 0X80,
};

//与CU交互私有协议约定
enum enCUProtocalValue
{
    PTZ_STOP = 0XBD00,
    PTZ_UP,
    PTZ_DOWN,
    PTZ_LEFT,
    PTZ_RIGHT,
    PTZ_TELE,
    PTZ_WIDE,
    PTZ_APERTURE_CLOSE,
    PTZ_APERTURE_OPEN,
    PTZ_FOCUS_NEAR,
    PTZ_FOCUS_FAR,
    PTZ_LEFT_UP,
    PTZ_LEFT_DOWN,
    PTZ_RIGHT_UP,
    PTZ_RIGHT_DOWN,
    PTZ_VIDEO_CHANGE,
	PTZ_VIDEO_CANCEL,
    
    PTZ_MODE_CHANGE,
    PTZ_OK,
    PTZ_CANCEL,
    PTZ_MON_CHANGE,
    PTZ_CAM_CHANGE,
    PTZ_SCREEN_CHANGE,

	PTZ_PRESET,
	PTZ_PRESET_CRUISE_STOP,
	PTZ_PRESET_CRUISE_START,
};

CNetKeyBoardService* CNetKeyBoardService::m_pNetKeyBoardService = NULL;

CNetKeyBoardService::CNetKeyBoardService()
{
    m_bIsListen = true;
    m_bIsInvalidKey = false;
    m_bTvWallPTZ = false;
    m_bIsTVWall = true;
    m_iWinNum = 0;
    m_iScreenNum = 0;
    m_iCurrentMaxUser = 0;
    m_iVideoLabel = 0;
    m_iVideoNum = 0;
    m_iDecodeNum = 0;
    m_iCameraID = 0;
    m_iWinID = 0;
    m_LastBeatTime = 0;
    m_iOldCallBackKeyValue = 0;
    m_iOldCallBackTeleWideKeyValue = 0;
    m_cOldSpeed = 0;
	m_cOldTeleWideSpeed = 0;
	m_iOldOtherKeyValue = 0;
	m_bTimerReg = false;
	m_bTimerRun = false;
}

CNetKeyBoardService::~CNetKeyBoardService()
{
    m_bIsListen = false;
    m_bIsInvalidKey = false;
    m_bIsTVWall = true;
    m_iScreenNum = 0;
    m_iCurrentMaxUser = 0;
    m_iVideoLabel = 0;
    m_iVideoNum = 0;
    m_iDecodeNum = 0;
    m_iCameraID = 0;
    m_iWinID = 0;

	try
    {
		if (m_bTimerReg)
		{
			(void)m_timer.cancelTimer(&m_KBTigger);
			m_bTimerReg = false;
		}

		if (m_bTimerRun)
		{
			//m_timer.clearTimer();
            m_timer.exit();
			m_bTimerRun = false;
		}
    }
    catch (...)
    {
    }
}

CNetKeyBoardService* CNetKeyBoardService::GetInstance()
{
    if (NULL == m_pNetKeyBoardService)
    {
        NEW_TRYCATCH(m_pNetKeyBoardService,CNetKeyBoardService,NULL)
    }
    return m_pNetKeyBoardService;
}

void CNetKeyBoardService::ReleaseInstance()
{
    DELETEP(m_pNetKeyBoardService);
}

/**************************************************************************
* name       : IniTcpServer
* description: 初始化网络键盘的TCP服务器监听套接字
* input      : NA
* output     : NA
* return     : IVS_SUCCEED - 成功,其他 - 失败
* remark     : NA
**************************************************************************/
int CNetKeyBoardService::IniTcpServer()
{
    IVS_INFO_TRACE("NetKeyBoard ");

    int iRet = IVS_SUCCEED;

    iRet = m_SocketService.CreateSocket();
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard Create Socket Failed!.","Error Code:%d", iRet);
        return iRet;
    }

    iRet = m_SocketService.Listen();
    if (IVS_SUCCEED != iRet)
    {
        m_SocketService.CloseSocket();

        BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard Listen Socket Failed!","Error Code:%d.",iRet);
        return iRet;
    }

    return IVS_SUCCEED;
}

/**************************************************************************
* name       : ListenSocket
* description: 端口监听线程
* input      : lpParameter 回调函数地址
* output     : NA
* return     : IVS_SUCCEED - 成功,其他 - 失败
* remark     : NA
**************************************************************************/
DWORD __stdcall CNetKeyBoardService::ListenSocket(LPVOID lpParameter)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    //检验参数合法性
    CHECK_POINTER(lpParameter,SDK_NULL_CALLBACK);
    
    CNetKeyBoardService *pNetKeyBoardService = NULL;
    pNetKeyBoardService = GetInstance();
    CHECK_POINTER(pNetKeyBoardService,IVS_FAIL);

    DWORD iRet = 0;

    LPCallBack lpCallBack = (LPCallBack)lpParameter;

    char *pszRecieveData = NULL;
    try
    {
        pszRecieveData = new char[BUFFER_LENGTH];
    }
    catch (...)
    {
        pszRecieveData = NULL;
    }
    CHECK_POINTER(pszRecieveData,IVS_FAIL);

    int iRecieveLength = 0;

    //监听端口，第一重循环用于等待客户端连接
    while (pNetKeyBoardService->m_bIsListen)
    {
        iRet = (DWORD)pNetKeyBoardService->m_SocketService.AcceptClientRequst(TIME_OUT);
        if (IVS_SUCCEED != iRet)
        {
            //IVS_LOG(LOG_DEBUG, "Accept Error Code:%d",iRet);
            BP_RUN_LOG_INF("NetKeyBoard Accept", "Error Code:%d", iRet);
        }

        Sleep(SLEEP_TIME);
        //该重循环在连接成功后用于数据的接收和发送
        //当连接断开时会退出该循环
        //当连接出错时也会退出该循环
        while (IVS_SUCCEED == iRet)
        {
            memset(pszRecieveData, 0, BUFFER_LENGTH);

            iRecieveLength = pNetKeyBoardService->m_SocketService.Receive(pszRecieveData, BUFFER_LENGTH,TIME_OUT);
            if (0 == iRecieveLength)
            {
                //IVS_LOG(LOG_ERROR, "NetKeyBoardService Disconnection!");
                BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard NetKeyBoardService Disconnection!","NA");
                break;
            }
            else if (SOCKET_ERROR == iRecieveLength)
            {
                int iError;
                iError = GetLastError();

                //超时认为是客户端未发送数据
                //未超时认为客户端断开连接
                if (WSAETIMEDOUT != iError)
                {
                    //IVS_LOG(LOG_ERROR, "Receive Error Code:%d",iError);
                    BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard Receive ", "Error Code:%d",iError);
                    break;
                }

                time_t tCurTime = pNetKeyBoardService->GetCurTime();

                //BP_DBG_LOG("CNetKeyBoardService::ListenSocket: m_LastBeatTime = %d.", pNetKeyBoardService->m_LastBeatTime);
                //BP_DBG_LOG("CNetKeyBoardService::ListenSocket: tCurTime = %d.", tCurTime);

                if ((tCurTime - pNetKeyBoardService->m_LastBeatTime  >  DAHUA_KB_BEAT_INTERVAL) && (0 != pNetKeyBoardService->m_LastBeatTime))
                {
                    BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard Beat Failed! Disconnected!", "Error Code:%d.",iError);
                    break;
                }
            }
            else
            {   
                //日志中打印接收的数据
                for (int i = 0; i < iRecieveLength; i++)
                {
                    //IVS_LOG(LOG_INFO, "Receive Data %d Is %02X",(i+1),(byte)pszRecieveData[i]);
                    BP_RUN_LOG_INF("NetKeyBoard:", "Receive Data %d Is %02X",(i+1),(byte)pszRecieveData[i]);
                }

                //解析数据
                (void)pNetKeyBoardService->IndentifyAction(pszRecieveData, iRecieveLength, lpCallBack);
            }
        }

        pNetKeyBoardService->m_SocketService.CloseConnectSocket();
    }

    delete[] pszRecieveData;
    pszRecieveData = NULL;

    pNetKeyBoardService->m_SocketService.CloseSocket();
    //pNetKeyBoardService->m_SocketService.FreeSock();
    pNetKeyBoardService->ReleaseInstance();

    //IVS_LOG(LOG_INFO, "Leave %s",__FUNCTION__);
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : IniUdpServer
* description: 初始化网络键盘的UDP服务器监听套接字
* input      : NA
* output     : NA
* return     : IVS_SUCCEED - 成功,其他 - 失败
* remark     : NA
**************************************************************************/
int CNetKeyBoardService::IniUdpServer()
{
    IVS_INFO_TRACE("NetKeyBoard ");

    int iRet = IVS_SUCCEED;

    iRet = m_UDPSocketService.CreateSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard Create UDPSocket Failed!.","Error Code:%d", iRet);
        return iRet;
    }

    iRet = m_UDPSocketService.UDPListen();
    if (IVS_SUCCEED != iRet)
    {
        m_UDPSocketService.CloseSocket();

        BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard Listen UDPSocket Failed!","Error Code:%d.",iRet);
        return iRet;
    }

    return IVS_SUCCEED;
}

//  端口监听线程(UDP)
DWORD __stdcall CNetKeyBoardService::ListenUDPSocket(LPVOID lpParameter)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    //检验参数合法性
    CHECK_POINTER(lpParameter,SDK_NULL_CALLBACK);

    CNetKeyBoardService *pNetKeyBoardService = NULL;
    pNetKeyBoardService = GetInstance();
    CHECK_POINTER(pNetKeyBoardService,IVS_FAIL);

    DWORD iRet = 0;

    LPCallBack lpCallBack = (LPCallBack)lpParameter;

    char *pszRecieveData = NULL;
    try
    {
        pszRecieveData = new char[BUFFER_LENGTH];
    }
    catch (...)
    {
        pszRecieveData = NULL;
    }
    CHECK_POINTER(pszRecieveData,IVS_FAIL);

    int iRecieveLength = 0;

    //监听端口，第一重循环用于等待客户端连接
    while (pNetKeyBoardService->m_bIsListen)
    {
		BP_DBG_LOG("NetKeyBoard To Recv UDP Data.");

        //该重循环在连接成功后用于数据的接收和发送
        //当连接断开时会退出该循环
        //当连接出错时也会退出该循环
        memset(pszRecieveData, 0, BUFFER_LENGTH);

        iRecieveLength = pNetKeyBoardService->m_UDPSocketService.UDPReceive(pszRecieveData, BUFFER_LENGTH, TIME_OUT);
        if (0 == iRecieveLength)
        {
            BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard NetKeyBoardUDPService Disconnection!","NA");
            continue;
        }
        else if (SOCKET_ERROR == iRecieveLength)
        {
            int iError;
            iError = GetLastError();

            //超时认为是客户端未发送数据
            //未超时认为客户端断开连接
            if (WSAETIMEDOUT != iError)
            {
                BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard UDPReceive ", "Error Code:%d",iError);
                continue;
            }

			BP_DBG_LOG("NetKeyBoard Recv UDP Data Timeout.");
        }
        else
        {   
            //日志中打印接收的数据
            for (int i = 0; i < iRecieveLength; i++)
            {
                BP_RUN_LOG_INF("NetKeyBoard:", "Receive Data %d Is %02X",(i+1),(byte)pszRecieveData[i]);
            }

            //解析数据
            (void)pNetKeyBoardService->IndentifyActionUDP(pszRecieveData, iRecieveLength, lpCallBack);
        }
        //pNetKeyBoardService->m_SocketService.CloseConnectSocket();
    }

    delete[] pszRecieveData;
    pszRecieveData = NULL;

    pNetKeyBoardService->m_UDPSocketService.CloseSocket();
    //pNetKeyBoardService->m_SocketService.FreeSock();
    //pNetKeyBoardService->ReleaseInstance();
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : ReleaseListenServer
* description: 释放监听套接字，供启动监听线程失败时使用
* input      : NA
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::ReleaseListenServer()
{
    IVS_INFO_TRACE("NetKeyBoard ");

    m_SocketService.CloseConnectSocket();
    m_UDPSocketService.CloseConnectSocket();

    m_SocketService.CloseSocket();
    m_UDPSocketService.CloseSocket();
}

/**************************************************************************
* name       : ReleaseListenThread
* description: 释放监听线程
* input      : hRleaseThread 监听线程句柄
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::ReleaseListenThread(HANDLE hRleaseThread)
{
    //IVS_LOG(LOG_INFO, "Enter %s",__FUNCTION__);
	IVS_INFO_TRACE("NetKeyBoard ");
	CNetKeyBoardService *pNetKeyBoardService = NULL;
	pNetKeyBoardService = GetInstance();
	CHECK_POINTER_VOID(pNetKeyBoardService);

	pNetKeyBoardService->m_bIsListen = false;
	pNetKeyBoardService->m_SocketService.CloseConnectSocket();
	int iRet = WaitForSingleObject(hRleaseThread,1500);

	//进程未结束，强制杀死进程
	if (WAIT_OBJECT_0 != iRet)
	{
		BP_RUN_LOG_INF("NetKeyBoard:", "kill the hRleaseThread, WaitForSingleObject iRet:%d", iRet);
		DWORD dwExitWord = 0;
		(void)GetExitCodeThread(hRleaseThread,&dwExitWord);
		(void)TerminateThread(hRleaseThread,dwExitWord);

		//强制杀死进程时，释放相应socket通讯资源
		pNetKeyBoardService->m_SocketService.CloseConnectSocket();
	}

	pNetKeyBoardService->m_UDPSocketService.CloseConnectSocket();
	pNetKeyBoardService->ReleaseInstance();

	(void)CloseHandle(hRleaseThread);
	hRleaseThread = NULL;
}
/**************************************************************************
* name       : PraseData
* description: 解析获得数据是否符合协议要求
* input      : pszRecieveData 接收的数据，iLength 接收数据长度
* output     : NA
* return     : IVS_SUCCEED - 成功，其他 - 失败
* remark     : NA
**************************************************************************/
int CNetKeyBoardService::PraseData(char *pszRecieveData, int iLength)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    //检验参数合法性
    CHECK_POINTER(pszRecieveData,IVS_FAIL);

    byte *pbtResultDala = (byte *)pszRecieveData;

    //收到心跳包，记录当前系统时间
    if (NKB_BEAT_WORD == pbtResultDala[0])
    {
        NoteLastBeatTime();
        return WRONG_CMD_WORD;
    }

    //检验命令字、键盘首地址和地址码
    if (NKB_CMD_WORD != pbtResultDala[0]
        || NKB_FIRST_ADDR != pbtResultDala[NKB_DATA_CHECK_START_POS]
        || NKB_ADDR_CODE != pbtResultDala[NKB_DATA_CHECK_START_POS+1])
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Wrong Command Word");
        return WRONG_CMD_WORD;
    }
    //检验获得数据的长度
    int iCheckLength = 0;
    for (int i = 0; i < NKB_EXTEND_DEFINE_LENGTH; i++)
    {
        iCheckLength *= NKB_MAX_BYTE;
        iCheckLength += pbtResultDala[NKB_EXTEND_DEFINE_POS + i];
    }
    if ((iCheckLength + NKB_DATA_BASIC_LENGTH) != (iLength))
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Wrong Data Length");
        return WRONG_LENGHT;
    }
    //检验和校验
    unsigned int bCheck = 0;
    for (int i = 0; i < NKB_DATA_CHECK_NUMBER; i++)
    {
        bCheck += pbtResultDala[NKB_DATA_CHECK_START_POS + i];
    }
    bCheck = bCheck % NKB_MAX_BYTE;
    if (bCheck != pbtResultDala[NKB_DATA_CHECK_START_POS + NKB_DATA_CHECK_NUMBER])
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Wrong Data Check");
        return WRONG_CHECK;
    }

    //收到的是正常数据，那么把这个包也认为是一个心跳包，记录当前时间
    NoteLastBeatTime();

    return IVS_SUCCEED;
}

/**************************************************************************
* name       : IsLogin
* description: 判断是否为登录消息
* input      : m_stNKBData 网络键盘数据
* output     : NA
* return     : true 是登录消息，false 不是登录消息
* remark     : NA
**************************************************************************/
bool CNetKeyBoardService::IsLogin()
{
    if ((NKB_KEY_PRESS == m_stNKBData.btKeyLabel)
        && (m_stNKBData.btReservedSecond < MAX_POWER)
        && ((m_stNKBData.btReservedFirst < MAX_CONTROLPOWER) || (LOGIN_POWER == m_stNKBData.btReservedFirst)))
    {
        return true;
    }

    return false;
}

/**************************************************************************
* name       : IsLogout
* description: 判断是否为注销消息
* input      : m_stNKBData 网络键盘数据
* output     : NA
* return     : true 是注销消息，false 不是注销消息
* remark     : NA
**************************************************************************/
bool CNetKeyBoardService::IsLogout()
{
    if ((NKB_KEY_PRESS == m_stNKBData.btKeyLabel)
        && (NKB_LOGOUT == m_stNKBData.btReservedSecond)
        && ((m_stNKBData.btReservedFirst < MAX_CONTROLPOWER) || (LOGIN_POWER == m_stNKBData.btReservedFirst)))
    {
        return true;
    }

    return false;
}

/**************************************************************************
* name       : LoginSuccess
* description: 网络键盘登录成功数据处理
* input      : pszRecieveData 网络键盘发送的数据流
               iLength 数据流长度
               m_stNKBData 网络键盘数据
* output     : NA
* return     : IVS_SUCCEED 返回数据成功，其他 失败
* remark     : NA
**************************************************************************/
int CNetKeyBoardService::LoginSuccess(char *pszRecieveData, int iLength)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    //检验参数合法性
    CHECK_POINTER(pszRecieveData,IVS_FAIL);

    byte *pbtSend = (byte *)pszRecieveData;

    //向网络键盘返回登录成功信息
    int iSendLength = m_SocketService.Send((char*)pbtSend, iLength,10);

    if (iSendLength != iLength)
    {
        int iRet = 0;
        iRet = GetLastError();
        BP_RUN_LOG_ERR(iRet, "NetKeyBoard Send Error", "Error Code: %d", iRet);
        return iRet;
    }
    //记录当前已登录用户中的最大权限用户
    if (m_stNKBData.btReservedSecond > m_iCurrentMaxUser)
    {
        m_iCurrentMaxUser = m_stNKBData.btReservedSecond;
    }
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : LoginFail
* description: 网络键盘登录失败数据处理
* input      : pszRecieveData 网络键盘发送的数据流
               iLength 数据流长度
               m_stNKBData 网络键盘数据
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::LoginFail(char *pszRecieveData, int iLength)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    CHECK_POINTER_VOID(pszRecieveData);

    byte *pbtSend = (byte *)pszRecieveData;
    pbtSend[NKB_KEY_LABEL] = 0;
    pbtSend[NKB_KEY_Reserved_FIRST + 1] = (byte)m_iCurrentMaxUser;

    unsigned int bCheck = 0;
    for (int i = 0; i < NKB_DATA_CHECK_NUMBER; i++)
    {
        bCheck += pszRecieveData[NKB_DATA_CHECK_START_POS + i];
    }

    bCheck = bCheck % NKB_MAX_BYTE;
    pbtSend[NKB_DATA_CHECK_START_POS + NKB_DATA_CHECK_NUMBER] = (byte)bCheck;

    (void)m_SocketService.Send((char*)pbtSend, iLength,2);
}

/**************************************************************************
* name       : DealLogData
* description: 处理登录相关消息
* input      : pszRecieveData 网络键盘发送的数据流
               iLength 数据流长度
               m_stNKBData 网络键盘数据
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::DisposeLogOnData(char *pszRecieveData, int iLength)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    //登录成功返回内容
    if (IsLogin())
    {
        int iRet = LoginSuccess(pszRecieveData, iLength);
        if (iRet)
        {
            (void)LoginFail(pszRecieveData, iLength);
        }
    }
    //注销不返回
    else if (IsLogout())
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Key Logout Success!");
    }
    //失败返回内容
    else
    {
        (void)LoginFail(pszRecieveData, iLength);
    }

}

/**************************************************************************
* name       : PTZDirectionControl
* description: 网络键盘云台方向控制数据处理
* input      : lpCallBack 回调函数
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::PTZDirectionControl(LPCallBack lpCallBack)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    CHECK_POINTER_VOID(lpCallBack);

    int iCallBackKeyValue = 0;

    if ((NKB_KEY_PRESS == m_stNKBData.btKeyLabel) 
        && ((0 != m_stNKBData.btReservedFirst)||(0 != m_stNKBData.btReservedSecond)))
    {
        switch(m_stNKBData.btKeyValue)
        {
        case NKB_UP:
            {
                iCallBackKeyValue = PTZ_UP;
                break;
            }
        case NKB_DOWN:
            {
                iCallBackKeyValue = PTZ_DOWN;
                break;
            }
        case NKB_LEFT:
            {
                iCallBackKeyValue = PTZ_LEFT;
                break;
            }
        case NKB_RIGHT:
            {
                iCallBackKeyValue = PTZ_RIGHT;
                break;
            }
        case NKB_ROCKER_LEFT_UP:
            {
                iCallBackKeyValue = PTZ_LEFT_UP;
                break;
            }
        case NKB_ROCKER_LEFT_DOWN:
            {
                iCallBackKeyValue = PTZ_LEFT_DOWN;
                break;
            }
        case NKB_ROCKER_RIGHT_UP:
            {
                iCallBackKeyValue = PTZ_RIGHT_UP;
                break;
            }
        case NKB_ROCKER_RIGHT_DOWN:
            {
                iCallBackKeyValue = PTZ_RIGHT_DOWN;
                break;
            }
        default:
            {
                break;
            }
        }
        m_bIsInvalidKey = true;
        BP_RUN_LOG_INF("NetKeyBoard ", "The Pressed Key Value:%d,PTZ Speed1:%d,PTZ Speed2:%d", m_stNKBData.btKeyValue, m_stNKBData.btReservedFirst,m_stNKBData.btReservedSecond);
        DealPTZDirectionCallBack(iCallBackKeyValue, lpCallBack);
    }
    else
    {
        DealPTZDirection(lpCallBack);
    }
}

void CNetKeyBoardService::DealPTZDirectionCallBack(int iCallBackKeyValue, LPCallBack lpCallBack)
{
    //(void)lpCallBack(iCallBackKeyValue,m_stNKBData.btReservedFirst,m_iWinNum,true);
    // 取数值较大的作为云台速度
    char cSpeed = m_stNKBData.btReservedFirst;
    if (m_stNKBData.btReservedFirst < m_stNKBData.btReservedSecond)
    {
        cSpeed = m_stNKBData.btReservedSecond;
    }
    // 速度范围1-10 
    cSpeed = (cSpeed) * 10 / 48;
    if (cSpeed > 10)
    {
        cSpeed = 10;
    }
    if (cSpeed < 1)
    {
        cSpeed = 1;
    }
    (void)lpCallBack(iCallBackKeyValue,cSpeed,0,false);

}

void CNetKeyBoardService::DealPTZDirection(LPCallBack lpCallBack)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    if (m_bIsInvalidKey)
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "The PTZ Stop!")
        (void)lpCallBack(PTZ_STOP, 0,m_iWinNum,false);
        m_bIsInvalidKey = false;
    }
}

/**************************************************************************
* name       : PTZOtherControl
* description: 网络键盘云台光圈、变焦、变倍控制数据处理
* input      : lpCallBack 回调函数
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::PTZOtherControl(LPCallBack lpCallBack)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    CHECK_POINTER_VOID(lpCallBack);

    int iCallBackKeyValue = 0;
    if (NKB_KEY_PRESS == m_stNKBData.btKeyLabel) 
    {
        switch(m_stNKBData.btKeyValue)
        {
        case NKB_TELE:
            {
                iCallBackKeyValue = PTZ_TELE;
                break;
            }
        case NKB_WIDE:
            {
                iCallBackKeyValue = PTZ_WIDE;
                break;
            }
        case NKB_APERTURE_CLOSE:
            {
                iCallBackKeyValue = PTZ_APERTURE_CLOSE;
                break;
            }
        case NKB_APERTURE_OPEN:
            {
                iCallBackKeyValue = PTZ_APERTURE_OPEN;
                break;
            }
        case NKB_FOCUS_NEAR:
            {
                iCallBackKeyValue = PTZ_FOCUS_NEAR;
                break;
            }
        case NKB_FOCUS_FAR:
            {
                iCallBackKeyValue = PTZ_FOCUS_FAR;
                break;
            }
        default:
            {
                break;
            }
        }
        m_bIsInvalidKey = true;
        BP_RUN_LOG_INF("NetKeyBoard ", "The Pressed Key Value:%d", m_stNKBData.btKeyValue);
        (void)lpCallBack(iCallBackKeyValue,0,m_iWinNum,true);
    }
    else
    {
        if (m_bIsInvalidKey)
        {
            BP_RUN_LOG_INF("NetKeyBoard ", "The PTZ Stop!");
            (void)lpCallBack(PTZ_STOP,0,m_iWinNum,true);
            m_bIsInvalidKey = false;
        }
    }

}

/**************************************************************************
* name       : IsDirectionControl
* description: 判断是否是云台方向控制
* input      : NA
* output     : NA
* return     : true 是云台方向控制，false 不是云台方向控制
* remark     : NA
**************************************************************************/
int CNetKeyBoardService::IsDirectionControl()
{
    if (((NKB_UP <= m_stNKBData.btKeyValue) && (NKB_RIGHT >= m_stNKBData.btKeyValue))
        || ((NKB_ROCKER_LEFT_UP <= m_stNKBData.btKeyValue) && (NKB_ROCKER_RIGHT_DOWN >= m_stNKBData.btKeyValue)))
    {
        return true;
    }
    return false;
}

/**************************************************************************
* name       : IndentifyAction
* description: 判别网络键盘按键
* input      : pszRecieveData 接收的数据，iLength 接收数据长度
* output     : NA
* return     : IVS_SUCCEED - 成功，其他 - 失败
* remark     : NA
**************************************************************************/
int CNetKeyBoardService::IndentifyAction(char *pszRecieveData, int iLength, LPCallBack lpCallBack)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    CHECK_POINTER(pszRecieveData,IVS_PARA_INVALID);

    if (NULL == lpCallBack)
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Callback Function Is Null!");
        return SDK_NULL_CALLBACK;
    }

    int iCount = 0;
    iCount = iLength / NKB_DATA_BASIC_LENGTH;

    for (int i = 0; i < iCount; i++)
    {
        DealAction(pszRecieveData, iLength, i, lpCallBack);
    }
    return IVS_SUCCEED;
}

void CNetKeyBoardService::DealAction(char *pszRecieveData, int iLength, int i, LPCallBack lpCallBack)
{
    char* pData = pszRecieveData + NKB_DATA_BASIC_LENGTH * i;

    int iRet;
    iRet = PraseData(pData, NKB_DATA_BASIC_LENGTH);
    if (iRet)
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Prase Data Failed!");
        return; 
    }

    m_stNKBData.btKeyValue   = (byte)pData[NKB_KEY_VALUE_POS];
    m_stNKBData.btKeyLabel   = (byte)pData[NKB_KEY_LABEL];
    m_stNKBData.btReservedFirst  = (byte)pData[NKB_KEY_Reserved_FIRST];
    m_stNKBData.btReservedSecond = (byte)pData[NKB_KEY_Reserved_FIRST + 1];

    //判断是否为登录
    if (NKB_LOGIN == m_stNKBData.btKeyValue)
    {
        DisposeLogOnData(pData, iLength);
    }
    //判断是否为停止
    else if (NKB_PTZ_STOP == m_stNKBData.btKeyValue)
    {
        m_iVideoLabel = 0;
        m_iVideoNum = 0;
        m_iDecodeNum = 0;

        BP_RUN_LOG_INF("NetKeyBoard ", "The PTZ Stop!");
        (void)lpCallBack(PTZ_STOP , 0,0,false);
    }
    //判断是否为方向控制
    else if (IsDirectionControl())
    {
        m_iVideoLabel = 0;
        m_iVideoNum = 0;
        m_iDecodeNum = 0;

        PTZDirectionControl(lpCallBack);
    }
    //判断是否为光圈、变焦、变倍等控制
    else if ((NKB_TELE  <= m_stNKBData.btKeyValue)&& (NKB_FOCUS_FAR >= m_stNKBData.btKeyValue))
    {
        m_iVideoLabel = 0;
        m_iVideoNum = 0;
        m_iDecodeNum = 0;

        PTZOtherControl(lpCallBack);
    }
    else if ((NKB_FN_FIRST == m_stNKBData.btKeyValue)
        || (NKB_FN_SECOND == m_stNKBData.btKeyValue)
        || (NKB_NINE >= m_stNKBData.btKeyValue)
        || (NKB_OK == m_stNKBData.btKeyValue)
        || (NKB_CANCEL == m_stNKBData.btKeyValue))
    {
        PTZVideoExchange(lpCallBack);
    }
    else if ((NKB_KEY_PRESS == m_stNKBData.btKeyLabel) 
        && (NKB_PTZ_MODE_CHANGE == m_stNKBData.btKeyValue))
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Mode Change. m_bIsTVWall = %d.", m_bIsTVWall);

        if (m_bIsTVWall)
        {
            (void)lpCallBack(PTZ_MODE_CHANGE , 0,m_iWinID,false);
        }
        else
        {
            (void)lpCallBack(PTZ_MODE_CHANGE , 1,m_iWinID,true);
        }

        m_bIsTVWall = !m_bIsTVWall;

        m_iVideoLabel = 0;
        m_iVideoNum = 0;
        m_iDecodeNum = 0;

        BP_RUN_LOG_INF("NetKeyBoard ", "Mode Change end. m_bIsTVWall = %d.", m_bIsTVWall);

    }
    //其他操作暂不支持
    else
    {
        DealOtherAction();
    }
}

void CNetKeyBoardService::DealOtherAction()
{
    if (NKB_KEY_PRESS == m_stNKBData.btKeyLabel)
    {
        if (0 != m_iVideoNum) //电视上墙的云台控制
        {
            if (true == m_bTvWallPTZ)
            {
                m_bTvWallPTZ = false;
            } 
            else
            {
                m_bTvWallPTZ = true;
            }

            m_iWinNum = m_iVideoNum;
        }
        else
        {
            m_bTvWallPTZ = false;
        }

        m_iVideoLabel = 0;
        m_iVideoNum = 0;
        m_iDecodeNum = 0;
        BP_RUN_LOG_INF("NetKeyBoard ", "Can't Do This Action");
    }
}

/**************************************************************************
* name       : PTZVideoExchange
* description: 网络键盘视频切换数据处理
* input      : lpCallBack 回调函数
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::PTZVideoExchange(LPCallBack lpCallBack)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    BP_RUN_LOG_INF("NetKeyBoard ", "PTZVideoExchange: Video %d On TVWall %d",m_iVideoNum,m_iDecodeNum);
    BP_RUN_LOG_INF("NetKeyBoard ", "PTZVideoExchange: Is %02X",(byte)m_stNKBData.btKeyValue);

    if (NKB_KEY_PRESS == m_stNKBData.btKeyLabel)
    {
        if (NKB_FN_FIRST == m_stNKBData.btKeyValue)
        {
            m_iVideoLabel = 1;
        }
        else if (NKB_FN_SECOND == m_stNKBData.btKeyValue)
        {
            m_iVideoLabel = 2;
        }
        else if (NKB_NINE >= m_stNKBData.btKeyValue)
        {
			UpdateNum();
        }
        else if ((NKB_OK == m_stNKBData.btKeyValue)
            && ((0 != m_iVideoNum)&&(MAX_CHANNEL_NUMBER >= m_iVideoNum))
            && ((0 != m_iDecodeNum)&&(MAX_CHANNEL_NUMBER >= m_iDecodeNum)))
        {
            BP_RUN_LOG_INF("NetKeyBoard ", "Video %d On TVWall %d",m_iVideoNum,m_iDecodeNum);
            (void)lpCallBack(PTZ_VIDEO_CHANGE,m_iVideoNum,m_iDecodeNum,false);
            m_iVideoLabel = 0;
            m_iVideoNum = 0;
            m_iDecodeNum = 0;
        }
        //切换窗格
        else if (NKB_OK == m_stNKBData.btKeyValue  &&  0 == m_iVideoNum  &&  2 == m_iVideoLabel)
        {
            BP_RUN_LOG_INF("NetKeyBoard ", "PTZVideoExchange: m_iVideoNum = %d, m_iVideoLabel = %d, m_iDecodeNum = %d",m_iVideoNum, m_iVideoLabel, m_iDecodeNum);
            BP_RUN_LOG_INF("NetKeyBoard ", "PTZVideoExchange: OK.");
            (void)lpCallBack(PTZ_MON_CHANGE,m_iDecodeNum,0,m_bIsTVWall);
            m_iVideoLabel = 0;
            m_iVideoNum = 0;
            m_iDecodeNum = 0;
        }
		else if (NKB_CANCEL == m_stNKBData.btKeyValue)
		{
            BP_RUN_LOG_INF("NetKeyBoard ", "PTZVideoExchange: Cancel.");
			PTVideoZCancel(lpCallBack);
			m_iVideoLabel = 0;
			m_iVideoNum = 0;
			m_iDecodeNum = 0;
		}
        else if ((NKB_OK == m_stNKBData.btKeyValue))
        {
            (void)lpCallBack(PTZ_SCREEN_CHANGE,m_iScreenNum,0,false);
        }
		else
		{
			m_iVideoLabel = 0;
			m_iVideoNum = 0;
			m_iDecodeNum = 0;
		}


    }
}


/**************************************************************************
* name       : UpdateNum
* description: 更新通道值
* input      : NA
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::UpdateNum()
{
    IVS_INFO_TRACE("NetKeyBoard m_iDecodeNum %d, m_iVideoLabel %d",m_iDecodeNum, m_iVideoLabel);
	if (1 == m_iVideoLabel)
	{
		m_iVideoNum *= 10;
		m_iVideoNum += m_stNKBData.btKeyValue;
	}
	else if (2 == m_iVideoLabel)
	{
		m_iDecodeNum *= 10;
		m_iDecodeNum += m_stNKBData.btKeyValue;
	}
    else
    {
        m_iVideoNum = 0;
        m_iDecodeNum = 0;
        m_iScreenNum = m_stNKBData.btKeyValue;
    }
}

/**************************************************************************
* name       : PTVideoZCancel
* description: 取消电视上墙
* input      : lpCallBack 回调函数
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::PTVideoZCancel(LPCallBack lpCallBack)
{
	//if ((0 != m_iVideoNum)&&(MAX_CHANNEL_NUMBER >= m_iVideoNum) ||  ((0 != m_iDecodeNum)&&(MAX_CHANNEL_NUMBER >= m_iDecodeNum)))
	//{
        BP_RUN_LOG_INF("NetKeyBoard ", "PTVideoZCancel: Video %d On TVWall %d",m_iVideoNum,m_iDecodeNum);
        (void)lpCallBack(PTZ_CANCEL,0,0,m_bIsTVWall);
	//}
}

// 处理天地伟业(UDP)消息
int CNetKeyBoardService::IndentifyActionUDP(char *pszRecieveData, int iLength, LPCallBack lpCallBack)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    CHECK_POINTER(pszRecieveData,IVS_FAIL);
    // 校验消息合法性
    int iCheckRet = PraseDataUDP(pszRecieveData, iLength);
    if (IVS_SUCCEED != iCheckRet)
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Wrong Command Word");
        return WRONG_CMD_WORD;
    }
    char cCmdType = m_stTDNKBData.btKeyValue;
    // 光圈变倍聚焦
    if (TD_NKB_APERTURE_PLUS <= cCmdType && TD_NKB_STOP >= cCmdType)
    {
        m_iCameraID = 0;
        m_iWinID = 0;
        PTZOtherControlUDP(lpCallBack);
    }
    // 摇杆云台控制
    else if (TD_NKB_PU == cCmdType || TD_NKB_MOUSE == cCmdType)
    {
        m_iCameraID = 0;
        m_iWinID = 0;
        PTZDirectionControlUDP(lpCallBack);
    }
    // 通道切换
    else if (TD_NKB_MON == cCmdType)
    {
        m_iWinID = m_stTDNKBData.btKeyLabel;
        (void)lpCallBack(PTZ_MON_CHANGE,m_iWinID,0,m_bIsTVWall);
    }
    else if (TD_NKB_CAM == cCmdType)
    {
        m_iCameraID = m_stTDNKBData.btKeyLabel * 256 + m_stTDNKBData.btReservedFirst;
        (void)lpCallBack(PTZ_CAM_CHANGE,m_iCameraID,m_iWinID,m_bIsTVWall);
    }
    // 硬解、软解切换
    else if (TD_NKB_SHIFT_PGM_TVWALL == cCmdType)
    {
        m_bIsTVWall = true;
        (void)lpCallBack(PTZ_MODE_CHANGE,1,m_iWinID,m_bIsTVWall);
    }
    else if (TD_NKB_SHIFT_PGM_CLIENT == cCmdType)
    {
        m_bIsTVWall = false;
        (void)lpCallBack(PTZ_MODE_CHANGE,0,m_iWinID,m_bIsTVWall);
    }
    // 选择画面
    else if (TD_NKB_FN_FIRST == cCmdType)
    {
        (void)lpCallBack(PTZ_SCREEN_CHANGE,m_stTDNKBData.btKeyLabel,0,m_bIsTVWall);
    }
	//预置位调用
	else if (TD_NKB_PRESET == cCmdType)
	{
		(void)PresetControlUDP(lpCallBack);
	}
	//预置位巡航调用
	else if (TD_NKB_PRESET_CRUISE_START == cCmdType)
	{
		(void)PresetCruiseControlUDP(lpCallBack);
	}
	//预置位巡航调用停止
	else if (TD_NKB_PRESET_CRUISE_STOP == cCmdType)
	{
		(void)PresetCruiseStopControlUDP(lpCallBack);
	}
    // 确认
    else if (TD_NKB_OK == cCmdType)
    {
        (void)lpCallBack(PTZ_OK,0,0,m_bIsTVWall);
    }
    // 取消
    else if (TD_NKB_CANCEL == cCmdType)
    {
        (void)lpCallBack(PTZ_CANCEL,0,0,m_bIsTVWall);
    }
    return IVS_SUCCEED;
}

// 校验天地伟业(UDP)消息合法性
int CNetKeyBoardService::PraseDataUDP(char *pszRecieveData,int iLength)
{
    CHECK_POINTER(pszRecieveData,IVS_FAIL);
    if (TD_NKB_KEY_DATA_MIN_LEN > iLength)
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Wrong Command Word, iLength:%d",iLength);
        return WRONG_CMD_WORD;
    }
    byte *pbtResultDala = (byte *)pszRecieveData;
    // 检查协议标志 0xE5E5BEBE
    if (TD_NKB_KEY_PROTOFLAG_FIRST != pbtResultDala[0] || TD_NKB_KEY_PROTOFLAG_FIRST != pbtResultDala[1] 
            || TD_NKB_KEY_PROTOFLAG_SECOND != pbtResultDala[2] || TD_NKB_KEY_PROTOFLAG_SECOND != pbtResultDala[3])
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Wrong Command Word");
        return WRONG_CMD_WORD;
    }
    // 检查回应标示，0x0000表示无回应控制操作
    if (0x00 != pbtResultDala[TD_NKB_KEY_RSPCTRL_POS] || 0x00 != pbtResultDala[TD_NKB_KEY_RSPCTRL_POS])
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Wrong Command Word");
        return WRONG_CMD_WORD;
    }
    // 检查帧头帧尾标示，帧头应为0xFAFA；帧尾应为0xFDFD
    if (TD_NKB_KEY_FRAME_HEAD != pbtResultDala[TD_NKB_KEY_FRAMEHEAD_POS] || TD_NKB_KEY_FRAME_HEAD != pbtResultDala[TD_NKB_KEY_FRAMEHEAD_POS + 1]
            || TD_NKB_KEY_FRAME_TAIL != pbtResultDala[iLength - 2] || TD_NKB_KEY_FRAME_TAIL != pbtResultDala[iLength - 1])
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Wrong Command Word");
        return WRONG_CMD_WORD;
    }
    // 检查校验位，从DVR编号到数据内容的异或和
    byte bCheck = 0x00;
    int iCheckLen = pbtResultDala[TD_NKB_KEY_DATALENGTH_POS];
    iCheckLen = iCheckLen * 256 + pbtResultDala[TD_NKB_KEY_DATALENGTH_POS + 1];
	for (int i = 0; i < iCheckLen - 6; i++)
	{
		bCheck ^= pbtResultDala[TD_NKB_KEY_FRAMEHEAD_POS + 2 + i];
	}
    if (bCheck != pbtResultDala[iLength - 4])
    {
        BP_RUN_LOG_INF("NetKeyBoard ", "Wrong Command Word");
        return WRONG_CMD_WORD;
    }
    // 记录报文内容
    memcpy(&m_stTDNKBData, pszRecieveData + TD_NKB_KEY_DATA_POS, iCheckLen - 8);
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : PTZOtherControl
* description: 网络键盘云台光圈、变焦、变倍控制数据处理
* input      : lpCallBack 回调函数
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::PTZOtherControlUDP(LPCallBack lpCallBack)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    CHECK_POINTER_VOID(lpCallBack);

    int iCallBackKeyValue = 0;
    switch(m_stTDNKBData.btKeyValue)
    {
    case TD_NKB_APERTURE_PLUS:
        {
            iCallBackKeyValue = PTZ_APERTURE_OPEN;
            break;
        }
    case TD_NKB_APERTURE_MINUS:
        {
            iCallBackKeyValue = PTZ_APERTURE_CLOSE;
            break;
        }
    case TD_NKB_TELE:
        {
            iCallBackKeyValue = PTZ_TELE;
            break;
        }
    case TD_NKB_WIDE:
        {
            iCallBackKeyValue = PTZ_WIDE;
            break;
        }
    case TD_NKB_FOCUS_NEAR:
        {
            iCallBackKeyValue = PTZ_FOCUS_NEAR;
            break;
        }
    case TD_NKB_FOCUS_FAR:
        {
            iCallBackKeyValue = PTZ_FOCUS_FAR;
            break;
        }
    case TD_NKB_STOP:
        {
            iCallBackKeyValue = PTZ_STOP;
            break;
        }
    default:
        {
			BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard PTZOtherControlUDP Failed!.","The Pressed Key Value:%d", m_stTDNKBData.btKeyValue);
            return;
        }
    }

	if (m_iOldOtherKeyValue == iCallBackKeyValue)
	{
		BP_DBG_LOG("Same Para, Not Need Send To CU.");
		return;
	}

	m_iOldOtherKeyValue = iCallBackKeyValue;

    //m_bIsInvalidKey = true;
    BP_RUN_LOG_INF("NetKeyBoard ", "The Pressed Key Value:%d", m_stTDNKBData.btKeyValue);
    (void)lpCallBack(iCallBackKeyValue,0,m_iWinID,m_bIsTVWall);

	if (PTZ_STOP == iCallBackKeyValue)
	{
		m_iOldOtherKeyValue = 0;
	}

    //else
    //{
    //    if (m_bIsInvalidKey)
    //    {
    //        BP_RUN_LOG_INF("NetKeyBoard ", "The PTZ Stop!");
    //        //(void)lpCallBack(PTZ_STOP, 0,0,false);
    //        if (m_bTvWallPTZ) //电视墙的云台控制
    //        {
    //            (void)lpCallBack(PTZ_STOP,0,m_iWinNum,true);
    //        } 
    //        else
    //        {
    //            (void)lpCallBack(PTZ_STOP, 0,0,false);
    //        }

    //        m_bIsInvalidKey = false;
    //    }
    //}
}

/**************************************************************************
* name       : PTZDirectionControlUDP
* description: 网络键盘云台方向控制数据处理
* input      : lpCallBack 回调函数
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::PTZDirectionControlUDP(LPCallBack lpCallBack)
{
    IVS_INFO_TRACE("NetKeyBoard ");
    CHECK_POINTER_VOID(lpCallBack);
	
	//销毁定时器
	CancelTimer();

    int iCallBackKeyValue = 0;
    int iCallBackTeleWideKeyValue = 0;
    // 右上，右下，右
    if (TD_PTZ_RIGHT & m_stTDNKBData.btKeyLabel)
    {
        if (TD_PTZ_UP & m_stTDNKBData.btKeyLabel)
        {
            iCallBackKeyValue = PTZ_RIGHT_UP;
        }
        else if (TD_PTZ_DOWN & m_stTDNKBData.btKeyLabel)
        {
            iCallBackKeyValue = PTZ_RIGHT_DOWN;
        }
        else
        {
            iCallBackKeyValue = PTZ_RIGHT;
        }
    }
    // 左上，左下，左
    else if (TD_PTZ_LEFT & m_stTDNKBData.btKeyLabel)
    {
        if (TD_PTZ_UP & m_stTDNKBData.btKeyLabel)
        {
            iCallBackKeyValue = PTZ_LEFT_UP;
        }
        else if (TD_PTZ_DOWN & m_stTDNKBData.btKeyLabel)
        {
            iCallBackKeyValue = PTZ_LEFT_DOWN;
        }
        else
        {
            iCallBackKeyValue = PTZ_LEFT;
        }
    }
    // 上，下
    else if (TD_PTZ_UP & m_stTDNKBData.btKeyLabel)
    {
        iCallBackKeyValue = PTZ_UP;
    }
    else if (TD_PTZ_DOWN & m_stTDNKBData.btKeyLabel)
    {
        iCallBackKeyValue = PTZ_DOWN;
    }
    // 停止码
    else if (TD_PTZ_STOP == m_stTDNKBData.btKeyLabel)
    {
        iCallBackKeyValue = PTZ_STOP;
    }

    // 变倍大
    if (TD_PTZ_TELE & m_stTDNKBData.btKeyLabel)
    {
        iCallBackTeleWideKeyValue = PTZ_TELE;
    }
    // 变倍小
    if (TD_PTZ_WIDE & m_stTDNKBData.btKeyLabel)
    {
        iCallBackTeleWideKeyValue = PTZ_WIDE;
    }

	//云台速度
	char cSpeed;
	if (TD_PTZ_TELE & m_stTDNKBData.btKeyLabel  ||  TD_PTZ_WIDE & m_stTDNKBData.btKeyLabel) //变倍速度
	{
		cSpeed = m_stTDNKBData.btReservedThird; //网络键盘传过来的，按照协议应该是0-7级别
		cSpeed +=1;
		/*if (cSpeed > 8)
		{
			cSpeed = 8;
		}
		if (cSpeed < 1)
		{
			cSpeed = 1;
		}*/
	}
	else
	{
		// 取数值较大的作为云台速度
		cSpeed = m_stTDNKBData.btReservedFirst;
		if (m_stTDNKBData.btReservedFirst < m_stTDNKBData.btReservedSecond)
		{
			cSpeed = m_stTDNKBData.btReservedSecond;
		}
		// 速度范围1-48 TODO 待确认
		/*cSpeed = (cSpeed + 1) * 10 / 64;
		if (cSpeed > 10)
		{
			cSpeed = 10;
		}
		if (cSpeed < 1)
		{
			cSpeed = 1;
		}*/
	}

    BP_RUN_LOG_INF("NetKeyBoard ", "The Pressed Key Value:%d,PTZ Speed1:%d,PTZ Speed2:%d, KeyLabel:%d", 
		m_stTDNKBData.btKeyValue, m_stTDNKBData.btReservedFirst,m_stTDNKBData.btReservedSecond, m_stTDNKBData.btKeyLabel);

    //变倍
    while (0 != iCallBackTeleWideKeyValue)
    {
		BP_RUN_LOG_INF("NetKeyBoard ", "TeleWideKeyValue:%d, OldTeleWideKeyValue:%d, TeleWideSpeed:%d, OldTeleWideSpeed:%d, cSpeed:%d",
			iCallBackTeleWideKeyValue, m_iOldCallBackTeleWideKeyValue, m_stTDNKBData.btReservedThird, m_cOldTeleWideSpeed, cSpeed);

        if (0 != m_iOldCallBackTeleWideKeyValue   &&   iCallBackTeleWideKeyValue == m_iOldCallBackTeleWideKeyValue  &&
			cSpeed == m_cOldTeleWideSpeed  &&  0 != m_cOldTeleWideSpeed)
        {
            BP_DBG_LOG("Same Para, Not Need Send.");
            break;
        }

        (void)lpCallBack(iCallBackTeleWideKeyValue,cSpeed,m_iWinID,m_bIsTVWall);
        m_iOldCallBackTeleWideKeyValue = iCallBackTeleWideKeyValue;
		m_cOldTeleWideSpeed = cSpeed;

        break;
    }
    // 屏蔽其余指令
    while (0 != iCallBackKeyValue)
    {
        if (iCallBackKeyValue == m_iOldCallBackKeyValue  &&  cSpeed == m_cOldSpeed  &&
            0 != m_iOldCallBackKeyValue                  &&  0 != m_cOldSpeed)
        {
            BP_DBG_LOG("Same Para, Not Need Send.");
            break;
        }

        (void)lpCallBack(iCallBackKeyValue,cSpeed,m_iWinID,m_bIsTVWall);
        m_iOldCallBackKeyValue = iCallBackKeyValue;
        m_cOldSpeed = cSpeed;

        break;
    }

    if (TD_PTZ_STOP == m_stTDNKBData.btKeyLabel)
    {
        m_iOldCallBackKeyValue = 0;
        m_iOldCallBackTeleWideKeyValue = 0;
        m_cOldSpeed = 0;
		m_cOldTeleWideSpeed = 0;
    }
	else
	{
		//启动定时器
		RegisterTimer(lpCallBack);
	}
}

//取得当前系统时间，以秒为单位
void CNetKeyBoardService::NoteLastBeatTime()
{
    time_t tCurrentTime = 0;
    (void)BP_OS::time(&tCurrentTime);
    m_LastBeatTime = tCurrentTime;
}

time_t CNetKeyBoardService::GetCurTime()
{
    time_t _tt = time(NULL);

    //BP_DBG_LOG("CNetKeyBoardService::GetCurTime: _tt = %d.", _tt);

    return _tt;
}

void CNetKeyBoardService::RegisterTimer(LPCallBack lpCallBack)
{
	//启动定时器
	if (!m_bTimerRun)
	{
		m_timer.init(500);
		(void)m_timer.run();
		m_bTimerRun = true;
	}

	m_KBTigger.SetNetKeyBoardService(this, lpCallBack);
	m_timer.registerTimer(&m_KBTigger, NULL, 1, enRepeated);
	m_bTimerReg = true;
}

void CNetKeyBoardService::CancelTimer()
{
	if(m_bTimerReg)
	{
	    (void)m_timer.cancelTimer(&m_KBTigger);
	    m_bTimerReg = false;
	}
}

//向CU发送停止摇杆的消息
void CNetKeyBoardService::StopPTZDirectionControlUDP(LPCallBack lpCallBack)
{
	IVS_INFO_TRACE("NetKeyBoard ");
	CHECK_POINTER_VOID(lpCallBack);

	(void)lpCallBack(PTZ_STOP,m_cOldSpeed,m_iWinID,m_bIsTVWall);
}

void CKBTrigger::onTrigger(void *pArg, ULONGLONG ullScales, TriggerStyle enStyle)
{
	CHECK_POINTER_VOID(m_lpCallBack);
	CHECK_POINTER_VOID(m_pNetKeyBoardService);
	m_pNetKeyBoardService->StopPTZDirectionControlUDP(m_lpCallBack);
}

/**************************************************************************
* name       : PresetControlUDP
* description: 网络键盘预置位控制数据处理
* input      : lpCallBack 回调函数
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::PresetControlUDP(LPCallBack lpCallBack)
{
	IVS_INFO_TRACE("NetKeyBoard ");
	CHECK_POINTER_VOID(lpCallBack);

	BP_RUN_LOG_INF("NetKeyBoard PresetControlUDP ", "The Pressed Key Value:%d, KeyLabel:%d", 
		m_stTDNKBData.btKeyValue, m_stTDNKBData.btKeyLabel);

	int iCallBackKeyValue = m_stTDNKBData.btKeyLabel;
	if (iCallBackKeyValue < 1  ||  iCallBackKeyValue > 255)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard PresetControlUDP Failed!.","Para Err:%d", iCallBackKeyValue);
		return;
	}

	(void)lpCallBack(PTZ_PRESET, iCallBackKeyValue, 0, m_bIsTVWall);
}

/**************************************************************************
* name       : PresetCruiseControlUDP
* description: 网络键盘预置位巡航调用控制数据处理
* input      : lpCallBack 回调函数
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::PresetCruiseControlUDP(LPCallBack lpCallBack)
{
	IVS_INFO_TRACE("NetKeyBoard ");
	CHECK_POINTER_VOID(lpCallBack);

	BP_RUN_LOG_INF("NetKeyBoard PresetCruiseControlUDP ", "The Pressed Key Value:%d, KeyLabel:%d", 
		m_stTDNKBData.btKeyValue, m_stTDNKBData.btKeyLabel);

	int iCallBackKeyValue = m_stTDNKBData.btKeyLabel;
	if (iCallBackKeyValue < 1  ||  iCallBackKeyValue > 255)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard PresetCruiseControlUDP Failed!.","Para Err:%d", iCallBackKeyValue);
		return;
	}

	(void)lpCallBack(PTZ_PRESET_CRUISE_START, iCallBackKeyValue, 0, m_bIsTVWall);
}

/**************************************************************************
* name       : PresetCruiseStopControlUDP
* description: 网络键盘预置位巡航停止控制数据处理
* input      : lpCallBack 回调函数
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CNetKeyBoardService::PresetCruiseStopControlUDP(LPCallBack lpCallBack)
{
	IVS_INFO_TRACE("NetKeyBoard ");
	CHECK_POINTER_VOID(lpCallBack);

	(void)lpCallBack(PTZ_PRESET_CRUISE_STOP, 0, 0, m_bIsTVWall);
}








