#include "UdpConnection.h"
#include "ivs_log.h"

CUdpConnection::CUdpConnection(void)
{
}


CUdpConnection::~CUdpConnection(void)
{
}

int CUdpConnection::Bind(const char* pszLocalIP, unsigned int uiLocalPort)
{
    int iRet = CConnection::Bind(pszLocalIP, uiLocalPort);
    if (IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Bind", "Call base class failed.");
        return IVS_FAIL;
    }

    m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(m_socket == INVALID_SOCKET)
    {
        IVS_LOG(IVS_LOG_ERR, "Bind", "Create udp socket failed, error:%d",  WSAGetLastError());
        return IVS_FAIL;
    }

    sockaddr_in _sin;
    ZeroMemory(&_sin,sizeof(sockaddr_in));
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons((u_short)m_uiLocalPort);
    _sin.sin_addr.s_addr = inet_addr(m_strLocalIP.c_str());

    if(SOCKET_ERROR == ::bind(m_socket,(sockaddr*)&_sin,sizeof(_sin)))
    {
        IVS_LOG(IVS_LOG_ERR, "Bind", "Bind udp socket failed, error:%d",  WSAGetLastError());
        return IVS_FAIL;
    }

    return IVS_SUCCEED;
}

int CUdpConnection::Connect(const char* pszServerIP, unsigned int uiServerPort)
{
    int iRet = CConnection::Connect(pszServerIP, uiServerPort);
    if (IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Connect", "Call base class failed.");
        return IVS_FAIL;
    }

    sockaddr_in _sin; 
    ZeroMemory(&_sin,sizeof(sockaddr_in));
    _sin.sin_family = AF_INET;
    _sin.sin_addr.s_addr = inet_addr(m_strServerIP.c_str());
    _sin.sin_port = htons((unsigned short)m_uiServerPort);
#if 0
	//只接收该端口和IP的数据
	if (SOCKET_ERROR == ::connect(m_socket, (SOCKADDR*) &_sin, sizeof(_sin) ))
	{
		IVS_LOG(IVS_LOG_ERR, "Connect", "Connect udp socket failed, error:%d",  WSAGetLastError());
		return IVS_PLAYER_RET_CONNECT_SOCKET_ERROR;
	}
#endif 

    return IVS_SUCCEED;
}

int CUdpConnection::RecvData(char* pData, unsigned int uiDataLen)
{
    if(NULL == pData)
    {
        IVS_LOG(IVS_LOG_ERR, "RecvData", "Input Param error, pData is NULL");
        return -1; // 与系统的recvfrom的错误返回值保持统一;
    }
    sockaddr_in addr;
    ZeroMemory(&addr,sizeof(sockaddr_in));
    int nLen = sizeof(addr);
    
    int iRet = Select(READ);
    if (iRet <= 0)
    {
       // IVS_LOG(IVS_LOG_ERR, "RecvData", "Select read mode error, error code:%d", WSAGetLastError());
    }
    else
    {
        iRet = ::recvfrom(m_socket, pData, (int)uiDataLen, 0, (sockaddr*)&addr,&nLen);
    }
    return iRet;
}

int CUdpConnection::SendData(const char* pData, unsigned int uiDataLen)
{
    if(NULL == pData)
    {
        IVS_LOG(IVS_LOG_ERR, "SendData", "Input Param error, pData is NULL");
        return -1;  // 与系统的sendto的错误返回值保持统一;
    }
    
    sockaddr_in addr;
    ZeroMemory(&addr,sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port  = htons((u_short)m_uiServerPort);
    addr.sin_addr.S_un.S_addr = inet_addr(m_strServerIP.c_str());
    
	int iRet = ::sendto(m_socket, pData, (int)uiDataLen, 0, (sockaddr*)&addr, sizeof(addr));
    return iRet;
}

int CUdpConnection::SendMulitCastData(const char* pData, unsigned int uiDataLen, const sockaddr_in& sockAddr)
{
	if(NULL == pData)
	{
		IVS_LOG(IVS_LOG_ERR, "SendData", "Input Param error, pData is NULL");
		return -1;  // 与系统的sendto的错误返回值保持统一;
	}

	int iRet = ::sendto(m_socket, pData, (int)uiDataLen, 0, (sockaddr*)&sockAddr, sizeof(sockAddr));
	return iRet;
}
