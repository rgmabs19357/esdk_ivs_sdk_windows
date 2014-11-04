/********************************************************************
	filename	: 	SocketService.h
	author		:	s00194833
	created		:	2011/12/07	
	description	:	Socket通讯类
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/12/07 初始版本
*********************************************************************/
#include "SocketService.h"
#include "KBInternalDef.h"
#include "bp_log.h"

#define LISTEN_PORT_NUMBER 5

bool CSocketService::m_bInitLabel = false;
int CSocketService::m_iInUse = 0;

CSocketService::CSocketService()
{
    m_hSocket = INVALID_SOCKET;
    m_hConnectSocket = INVALID_SOCKET;

    m_ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    m_ServerAddr.sin_family = AF_INET;
    m_ServerAddr.sin_port = htons(0);
    memset(m_ServerAddr.sin_zero, 0, sizeof(m_ServerAddr.sin_zero));

    m_ClientAddr.sin_addr.S_un.S_addr = inet_addr("");
    m_ClientAddr.sin_family = AF_INET;
    m_ClientAddr.sin_port = htons(0);
    memset(m_ClientAddr.sin_zero, 0, sizeof(m_ClientAddr.sin_zero));
};

CSocketService::~CSocketService()
{
    m_hConnectSocket = INVALID_SOCKET;
    m_hSocket = INVALID_SOCKET;
};

/**************************************************************************
* name       : InitSock
* description: 初始化Socket通讯库
* input      : NA
* output     : NA
* return     : IVS_SUCCEED 成功，其他 失败
* remark     : NA
**************************************************************************/
int CSocketService::InitSock()
{
    //如果通讯库没有打开执行以下操作
    //已经打开直接返回
    if (!m_bInitLabel)
    {
        WORD wVersionRequested;
        WSADATA wsaData;
        int iRet = 0;

        wVersionRequested = MAKEWORD(2,2);

        //打开socket通讯库
        iRet = WSAStartup(wVersionRequested,&wsaData);
        if (0 != iRet)
        {
            return iRet;
        }

        //检查版本号是否为1.1
        if ((2 != LOBYTE(wsaData.wVersion))||(2 != HIBYTE(wsaData.wVersion)))
        {
            FreeSock();
            return IVS_FAIL;
        }
        m_bInitLabel = true;
        m_iInUse++;
    }
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : FreeSock
* description: 释放Socket通讯库资源
* input      : NA
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CSocketService::FreeSock()
{
    m_iInUse--;
    //无人使用时，关闭通讯库
    if (0 == m_iInUse)
    {
        (void)WSACleanup();
        m_bInitLabel = false;
    }
}

/**************************************************************************
* name       : CreateSocket
* description: 创建套字节
* input      : iAF地址族 只支持AF_INET
               iType Socket类型 支持SOCK_STREAM、SOCK_DGRAM
               iRrotocol 协议方法 IPPROTO_TCP、IPPROTO_UDP
* output     : NA
* return     : IVS_SUCCEED 成功，其他 失败
* remark     : NA
**************************************************************************/
int CSocketService::CreateSocket(int iAF,int iType,int iProtocol)
{
    if(m_hSocket != INVALID_SOCKET)
    {
        BP_RUN_LOG_INF("NetKeyBoard Needn't to Create Socket", "NA");
        return IVS_SUCCEED;
    }

    m_hSocket = socket(iAF,iType,iProtocol);

    if (INVALID_SOCKET == m_hSocket)
    {
        int iRet;
        iRet = WSAGetLastError();
        BP_RUN_LOG_INF("NetKeyBoard Create Socket failed", "iRet:%d", iRet);
        return iRet;
    }

	//解决关闭端口监听后不能立即又使用这个端口的问题
	long lReuseAddrFlag = 1;
	if (setsockopt((SOCKET)m_hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&lReuseAddrFlag,
		sizeof(lReuseAddrFlag)) < 0)
	{	
		int iRet;
		iRet = WSAGetLastError();
		BP_RUN_LOG_INF("setsockopt client socket failed", "error(%d)", iRet);
		(void)CloseSocket();
		return iRet;
	}

    return IVS_SUCCEED;
}


/**************************************************************************
* name       : SetServerAddr
* description: 设置服务器IP和端口号
* input      : iPort 服务器端口号
               pszIP 服务器IP
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CSocketService::SetServerAddr(u_short iPort,const char *pszIP)
{
    if (NULL != pszIP)
    {
        m_ServerAddr.sin_addr.S_un.S_addr = inet_addr(pszIP);
    }
    m_ServerAddr.sin_port = htons(iPort);
}

int CSocketService::Connect(int iTimeout)
{
    if( INVALID_SOCKET == m_hSocket)
    {
        return SDK_SOCKET_INVALID;
    }

    //设置非阻塞方式连接
    u_long ulNonBlock = 1;
    int iRet = 0;
    iRet = ioctlsocket(m_hSocket,FIONBIO,(u_long*)&ulNonBlock);//lint !e569
    if(SOCKET_ERROR == iRet)
    {
        iRet = WSAGetLastError();
        return iRet;
    }

    iRet = connect(m_hSocket,(sockaddr *)&m_ServerAddr,sizeof(sockaddr));
    //连接成功
    if(0 == iRet)
    {
        return IVS_SUCCEED;
    }
    else if(SOCKET_ERROR == iRet)
    {
        iRet = WSAGetLastError();
        if(WSAEWOULDBLOCK == iRet)//暂时阻塞
        {
            timeval timeout;
            timeout.tv_sec = iTimeout;
            timeout.tv_usec = 0;

            fd_set wfd;
            FD_ZERO(&wfd);
            FD_SET(m_hSocket,&wfd);

            int nfds = select(1,(fd_set*)0,&wfd,(fd_set*)0,&timeout);
            //select 错误
            if(nfds < 0)
            {
                iRet = WSAGetLastError();
                return iRet;
            }
            //超时
            else if(nfds ==0)
            {
                return SDK_SOCKET_TIMEOUT;
            }
            //已经返回
            else
            {
                int iErr = -1;
                int len = sizeof(int);

                //获取socket信息错误
                iRet = getsockopt(m_hSocket, SOL_SOCKET, SO_ERROR, (char*)&iErr, &len);
                if ( iRet< 0) 
                { 
                    iRet = WSAGetLastError();
                    return iRet;
                }
                //连接发生错误
                if(0 != iErr)
                {
                    return IVS_FAIL;
                }
            }
        }
        else//其他错误
        {
            return iRet;
        }
    }

    return IVS_SUCCEED;
}
/**************************************************************************
* name       : Listen
* description: 监听端口
* input      : NA
* output     : NA
* return     : IVS_SUCCEED 成功，其他 失败
* remark     : NA
**************************************************************************/
int CSocketService::Listen()
{
    if (INVALID_SOCKET == m_hSocket)
    {
        return SDK_SOCKET_INVALID;
    }

    int iRet;
    iRet = bind(m_hSocket,(sockaddr*)&m_ServerAddr,sizeof(sockaddr));
    if (SOCKET_ERROR == iRet)
    {
        iRet = WSAGetLastError();
        return iRet;
    }

    iRet = listen(m_hSocket,LISTEN_PORT_NUMBER);
    if (SOCKET_ERROR == iRet)
    {
        iRet = WSAGetLastError();
        return iRet;
    }
    return IVS_SUCCEED;
}

// UDP监听端口
int CSocketService::UDPListen()
{
    if (INVALID_SOCKET == m_hSocket)
    {
        return SDK_SOCKET_INVALID;
    }

    int iRet;
    iRet = bind(m_hSocket,(sockaddr*)&m_ServerAddr,sizeof(sockaddr));
    if (SOCKET_ERROR == iRet)
    {
        iRet = WSAGetLastError();
        return iRet;
    }
    return IVS_SUCCEED;
}

/**************************************************************************
* name       : AcceptClientRequst
* description: 等待客户请求
* input      : iTimeout 连接超时时间
* output     : NA
* return     : IVS_SUCCEED 成功，其他 失败
* remark     : NA
**************************************************************************/
int CSocketService::AcceptClientRequst(int iTimeout)
{
    if (INVALID_SOCKET == m_hSocket)
    {
        return SDK_SOCKET_INVALID;
    }

    timeval timeout;
    timeout.tv_sec = iTimeout;
    timeout.tv_usec = 0;

    fd_set rfd;
    FD_ZERO(&rfd);
    FD_SET(m_hSocket,&rfd);

    int nfds = select(1,&rfd,(fd_set*)0,(fd_set*)0,&timeout);

    int iRet = 0;
    //只处理有响应的
    if(nfds > 0)
    {
        FD_CLR(m_hSocket,&rfd);
        int iAddrLen = sizeof(sockaddr);
        m_hConnectSocket = accept(m_hSocket,(sockaddr*)&m_ClientAddr,&iAddrLen);
        if(INVALID_SOCKET == m_hConnectSocket)
        {
            iRet = WSAGetLastError();
            return iRet;
        }
    }
    //超时了
    else if(0 == nfds)
    {
        return SDK_SOCKET_TIMEOUT;
    }
    //出错了
    else
    {
        iRet = WSAGetLastError();
        return iRet;
    }

    return IVS_SUCCEED;
}

/**************************************************************************
* name       : Send
* description: 发送数据 
* input      : pszSendData 发送数据缓冲区
               iLength 发送数据长度
               iTimeout 超时时间
* output     : NA
* return     : totalSendBytes  已发送的数据长度
* remark     : NA
**************************************************************************/
int CSocketService::Send(const char *pszSendData,int iLength,int iTimeout)
{
    if (NULL == pszSendData)
    {
        return IVS_FAIL;
    }

    //变为秒
    iTimeout *= 1000;

    int iRet = 0;;
    iRet = setsockopt(m_hConnectSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&iTimeout,sizeof(iTimeout));
    if (SOCKET_ERROR == iRet)
    {
        iRet = WSAGetLastError();
        return iRet;
    }

    int totalBytes = iLength;
    int totalSendBytes = 0;
    int sendBytes = 0;

    //发送数据保证数据可以全部发送
    while(totalSendBytes<totalBytes)
    {
        sendBytes = send(m_hConnectSocket,pszSendData+totalSendBytes,totalBytes-totalSendBytes,0);
        if(sendBytes<=0)
        {
            break;
        }
        totalSendBytes+=sendBytes;
    }

    return totalSendBytes;
}

/**************************************************************************
* name       : Receive
* description: 接收数据
* input      : iTimeout 超时时间
* output     : pszRecieveData 接收数据缓冲区
               iLength 接收数据缓冲区长度
* return     : iReceiveLength 实际接收数据长度
* remark     : NA
**************************************************************************/
int CSocketService::Receive(char *pszReceiveData,u_int iLength,int iTimeout)
{
    if (NULL == pszReceiveData)
    {
        return IVS_FAIL;
    }
 
    //变为秒
    iTimeout *= 1000;//变为秒

    int iRet = 0;
    iRet = setsockopt(m_hConnectSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&iTimeout,sizeof(iTimeout));
    if (SOCKET_ERROR == iRet)
    {
        iRet = WSAGetLastError();
        return iRet;
    }

    int iReceiveLength =0 ;
    iReceiveLength = recv(m_hConnectSocket,pszReceiveData,(int)iLength,0);
    return iReceiveLength;
}

// UDP接受数据
int CSocketService::UDPReceive(char *pszReceiveData,u_int iLength, int iTimeout)
{
    if (NULL == pszReceiveData)
    {
		BP_RUN_LOG_ERR(IVS_FAIL, "NetKeyBoard Para Is NULL!","NA");
        return IVS_FAIL;
    }
    int iRet = 0;
    iRet = setsockopt(m_hSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&iTimeout,sizeof(iTimeout));
    if (SOCKET_ERROR == iRet)
    {
        iRet = WSAGetLastError();
		BP_RUN_LOG_ERR(iRet, "NetKeyBoard Setsockopt failed!","NA");
        return iRet;
    }
    int iReceiveLength =0 ;
    int	len = sizeof(SOCKADDR);
    iReceiveLength = recvfrom(m_hSocket,pszReceiveData,(int)iLength,0,(SOCKADDR*)&m_ClientAddr,&len);
    return iReceiveLength;
}
/**************************************************************************
* name       : CloseConnectSocket
* description: 关闭连接套字节
* input      : NA
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CSocketService::CloseConnectSocket()
{
    if (INVALID_SOCKET != m_hConnectSocket)
    {
		for (int i = 0; i < 15; i++) //关闭不成功的话，重复去关
		{
			int iRet = closesocket(m_hConnectSocket);
			if (SOCKET_ERROR != iRet) //关闭成功，不成功的话必然返回SOCKET_ERROR
			{
				m_hConnectSocket = INVALID_SOCKET;
				break;
			}

			iRet = WSAGetLastError();
			BP_RUN_LOG_ERR(iRet, "NetKeyBoard CloseConnectSocket failed!","NA");
			Sleep(100);
		}
    }
}

/**************************************************************************
* name       : CloseSocket
* description: 关闭套字节
* input      : NA
* output     : NA
* return     : NA
* remark     : NA
**************************************************************************/
void CSocketService::CloseSocket()
{
    if (INVALID_SOCKET != m_hSocket)
    {
		for (int i = 0; i < 15; i++)
		{
			int iRet = closesocket(m_hSocket);
			if (SOCKET_ERROR != iRet)
			{
				m_hSocket = INVALID_SOCKET;
				break;
			}

			iRet = WSAGetLastError();
			BP_RUN_LOG_ERR(iRet, "NetKeyBoard CloseSocket failed!","NA");
			Sleep(100);
		}
    }
}
