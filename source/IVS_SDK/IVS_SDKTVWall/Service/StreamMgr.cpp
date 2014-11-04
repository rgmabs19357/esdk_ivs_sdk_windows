#include "StdAfx.h"
#include "StreamMgr.h"

//Default constructor
CStreamMgr::CStreamMgr() : m_pRemoteDecoderMgr(NULL)
{
}

//Destructor
CStreamMgr::~CStreamMgr()
{
    m_pRemoteDecoderMgr = NULL; //远程解码器
}

IVS_INT32 CStreamMgr::InitStreamMgr
(   IN PCUMW_CALLBACK_NOTIFY   pFunNotifyCallBack,
    IN void*                   pCallBackUserData)
{
    //设置通知回调函数状态值
    m_objStatusNotifyCallBack.SetStatusValue(pFunNotifyCallBack, pCallBackUserData);

    IVS_RUN_LOG_INF("Init Stream Manager Success.");
    return IVS_SUCCEED;
}

IVS_INT32 CStreamMgr::ReleaseStreamMgr()
{
    IVS_RUN_LOG_INF("Release Stream Manager.");

    return IVS_SUCCEED;
}

IVS_INT32 CStreamMgr::InitBusiness
(   IN CBusiness& objBusiness)
{
    const unsigned long ulBusinessId = objBusiness.GetBusinessId();

    IVS_DBG_LOG("[ BusinessID = %lu ] Init Business In Stream Manager Begin.", ulBusinessId);

    long lResult = IVS_SUCCEED;

    //上墙业务
    if (objBusiness.IsRemoteBusiness()) //lint !e1055
    {
        //上墙业务
        if (NULL == m_pRemoteDecoderMgr)
        {
            IVS_RUN_LOG_ERR(
                    "[ BusinessID = %lu ] Init Remote Business In Stream Manager Failed. Remote Decoder Manager Has Stopped.",
                    ulBusinessId);
            return IVS_TVWALL_TV_WALL_SRC_STOPED;
        }

        lResult = m_pRemoteDecoderMgr->InitBusiness(objBusiness);
        if (IVS_SUCCEED != lResult)
        {
            IVS_RUN_LOG_ERR( "[ BusinessID = %lu ] Init Remote Business In Stream Manager Failed.", ulBusinessId);
            return lResult;
        }
    }

    IVS_DBG_LOG("[ BusinessID = %lu ] Init Business In Stream Manager Success.", ulBusinessId);
    return IVS_SUCCEED;
}

IVS_INT32 CStreamMgr::StartBusiness
(   IN CBusiness& objBusiness)
{
    const unsigned long ulBusinessId = objBusiness.GetBusinessId();

    IVS_DBG_LOG("[ BusinessID = %lu ] Start Business In Stream Manager Begin.", ulBusinessId);

    long lResult = IVS_SUCCEED;

    //上墙业务
    if (objBusiness.IsRemoteBusiness())
    {
        if (NULL == m_pRemoteDecoderMgr)
        {
            IVS_RUN_LOG_ERR(
                    "[ BusinessID = %lu ] Start Remote Business In Stream Manager Failed. Remote Decoder Manager Has Stopped.",
                    ulBusinessId);
            return IVS_TVWALL_TV_WALL_SRC_STOPED;
        }

        //if (objBusiness.IsRemoteDirectConnect()) //lint !e1055
        {
            //直连上墙时直接使用服务端IP和端口信息
            objBusiness.m_stMediaDistributePortInfo = objBusiness.m_stServerPortInfo;
            if(!CToolsHelp::Strncpy(objBusiness.m_szMediaDistributeIp, sizeof(objBusiness.m_szMediaDistributeIp), objBusiness.m_szServerIp,
                    sizeof(objBusiness.m_szMediaDistributeIp) - 1))
			{
				IVS_RUN_LOG_ERR("Start Business Copy MediaDistributeIp error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
        }
        /*else
        {
            //非直连上墙时直接使用本地IP
            const char* pszLocalIp = CStatusSingleton::Instance().GetLocalIp();
            strncpy(objBusiness.m_szMediaDistributeIp, pszLocalIp, sizeof(objBusiness.m_szMediaDistributeIp) - 1);
        }*/

        lResult = m_pRemoteDecoderMgr->StartBusiness(objBusiness);
        if (IVS_SUCCEED != lResult)
        {
            IVS_RUN_LOG_ERR( "[ BusinessID = %lu ] Start Remote Business In Stream Manager Failed.", ulBusinessId);
            return lResult;
        }

        //直连上墙时，直接返回
        if (objBusiness.IsRemoteDirectConnect())
        {
            IVS_RUN_LOG_INF(
                    "[ BusinessID = %lu ] Start Remote Business In Stream Manager Success. Direct Connect To Video Source.",
                    ulBusinessId);
            return IVS_SUCCEED;
        }
    }

    IVS_DBG_LOG("[ BusinessID = %lu ] Start Business In Stream Manager Success.", ulBusinessId);
    return IVS_SUCCEED;
}

IVS_INT32 CStreamMgr::ControlBusiness
(   IO CBusiness&                  objBusiness,
    IN CUMW::BUSINESS_CTRL_PARA&   stCtrlPara)
{
    const unsigned long ulBusinessId = objBusiness.GetBusinessId();

    IVS_DBG_LOG("[ BusinessID = %lu ] Control Business In Stream Manager Begin.", ulBusinessId);

    long lResult = IVS_SUCCEED;

    //上墙业务只有音频控制
    if ((CUMW::BUSINESS_CTRL_CODE_E_LOCAL_AUDIO == stCtrlPara.enCtrlCode)
        && objBusiness.IsRemoteBusiness())
    {
        if (NULL == m_pRemoteDecoderMgr)
        {
            IVS_RUN_LOG_ERR(
                    "[ BusinessID = %lu ] Control Remote Business In Stream Manager Failed. Remote Decoder Manager Has Stopped.",
                    ulBusinessId);
            return IVS_TVWALL_TV_WALL_SRC_STOPED;
        }

        lResult = m_pRemoteDecoderMgr->ControlBusiness(objBusiness, stCtrlPara);
    }

    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "[ BusinessID = %lu ] Control Business In Stream Manager Failed.", ulBusinessId);
        return lResult;
    }

    IVS_DBG_LOG("[ BusinessID = %lu ] Control Business In Stream Manager Success.", ulBusinessId);
    return IVS_SUCCEED;
}

IVS_INT32 CStreamMgr::StopBusiness
(   IN const CBusiness& objBusiness)
{
    const unsigned long ulBusinessId = objBusiness.GetBusinessId();

    IVS_DBG_LOG("[ BusinessID = %lu ] Stop Business In Stream Manager Begin.", ulBusinessId);

    long lResult = IVS_SUCCEED;

    //上墙业务
    if (objBusiness.IsRemoteBusiness())
    {
        if (NULL != m_pRemoteDecoderMgr)
        {
            lResult = m_pRemoteDecoderMgr->StopBusiness(objBusiness);
        }
    }

    IVS_DBG_LOG("[ BusinessID = %lu ] Stop Business In Stream Manager End.", ulBusinessId);
    return lResult;
}

IVS_INT32 __stdcall CStreamMgr::HandleNotifyCallBack
(   IN CUMW_NOTIFY_INFO* pstNotifyInfo,
    IN void*             pUserData)
{
    if (NULL == pstNotifyInfo)
    {
        IVS_RUN_LOG_INF("Stream Manager Handle Notify Call Back Failed. Notify Info is Emtpy.");
        return IVS_FAIL;
    }

    IVS_DBG_LOG("Stream Manager Handle Notify Call Back Begin. NotifyType = 0x%04X.",
            pstNotifyInfo->ulNotifyType);

    //参数指针安全转换
    CStreamMgr* pThis = VOS_CAST((CStreamMgr*)pUserData);
    if (NULL == pThis)
    {
        IVS_RUN_LOG_ERR( "Stream Manager Handle Notify Call Back Failed. "
                "User Data(%#p) Can't Convert to Stream Manager Object.", pUserData);
        return IVS_FAIL;
    }

    //直接透传通知回调函数
    long lResult = pThis->m_objStatusNotifyCallBack.NotifyCallBack(*pstNotifyInfo);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "Stream Manager Handle Notify Call Back Failed. Return Error(0x%04X).", lResult);
        return IVS_FAIL;
    }

    IVS_DBG_LOG("Stream Manager Handle Notify Call Back Success. NotifyType = 0x%04X.",
            pstNotifyInfo->ulNotifyType);
    return IVS_SUCCEED;
}

IVS_INT32 CStreamMgr::StartRemoteDecoderMgr()
{
    long lResult = IVS_SUCCEED;

    IVS_DBG_LOG("Start Remote Decoder Manager Begin.");

    if (NULL != m_pRemoteDecoderMgr)
    {
        IVS_RUN_LOG_ERR( "Create Remote Decoder Manager Failed On Start Remote Decoder Manager. Already Running.");
        return IVS_TVWALL_TV_WALL_SRC_RUNING;
    }

    if (NULL == VOS_NEW(m_pRemoteDecoderMgr))
    {
        IVS_RUN_LOG_ERR( "Create Remote Decoder Manager Failed On Start Remote Decoder Manager.");
        return IVS_ALLOC_MEMORY_ERROR;
    }

    //初始化RemoteDecoderMgr实例
    lResult = m_pRemoteDecoderMgr->InitRemoteDecoderMgr(HandleNotifyCallBack, this);
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "Init Remote Decoder Manager Failed On Start Remote Decoder Manager.");
        return lResult;
    }

    //启动RemoteDecoderMgr实例
    lResult = m_pRemoteDecoderMgr->StartRemoteDecoderMgr();
    if (IVS_SUCCEED != lResult)
    {
        IVS_RUN_LOG_ERR( "Start Remote Decoder Manager Failed. Result = 0x%04X.", lResult);
        return lResult;
    }

    IVS_DBG_LOG("Start Remote Decoder Manager Success.");
    return IVS_SUCCEED;
}

IVS_INT32 CStreamMgr::StopRemoteDecoderMgr()
{
    IVS_DBG_LOG("Stop Remote Decoder Manager Begin.");

    long lResult = IVS_SUCCEED;

    if (NULL != m_pRemoteDecoderMgr)
    {
        lResult = m_pRemoteDecoderMgr->StopRemoteDecoderMgr();

        VOS_DELETE(m_pRemoteDecoderMgr);
    }

    IVS_DBG_LOG("Stop Remote Decoder Manager End. Result = 0x%04X.", lResult);
    return lResult;
}
