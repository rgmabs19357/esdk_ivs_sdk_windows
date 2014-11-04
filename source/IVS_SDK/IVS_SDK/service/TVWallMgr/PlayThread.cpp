#include "PlayThread.h"
#include "bp_def.h"
#include "bp_log.h"
#include "IVSCommon.h"
#include "TVWallMgr.h"
#include "ToolsHelp.h"
#include "IVS_Trace.h"

CPlayThread::CPlayThread(void) : m_pChannel(NULL)
								, m_pTVWallMgr(NULL)
								, m_pVosThread(NULL)
{
    memset(m_szCameraCode, 0, IVS_DEV_CODE_LEN+1);
    memset(&m_stTVWallParam, 0, sizeof(m_stTVWallParam));
}

CPlayThread::~CPlayThread(void)
{
    m_pVosThread = NULL;
    m_pChannel = NULL;
	m_pTVWallMgr = NULL;
}

IVS_INT32 CPlayThread::run()
{
    errno = 0;
    if (VOS_OK != VOS_CreateThread((VOS_THREAD_FUNC)invoke, (void *)this, 
                                    &m_pVosThread, VOS_DEFAULT_STACK_SIZE))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Create Play thread ", "failed. error(%d):%s", errno, strerror(errno));
        return IVS_FAIL;
    };
    return IVS_SUCCEED;
}

void CPlayThread::SetCameraCode(const IVS_CHAR* pCameraCode)
{
    if(!CToolsHelp::Strncpy(m_szCameraCode, sizeof(m_szCameraCode), pCameraCode, strlen(pCameraCode)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Set Camera Code", "Strcpy failed!");
	}
}

void CPlayThread::SetDecoderChannel(CDecoderChannel* pCh)
{
    m_pChannel = pCh;
}

void CPlayThread::SetTVWallMgr(CTVWallMgr* pTVWallMgr)
{
	m_pTVWallMgr = pTVWallMgr;
}

void CPlayThread::SetTVWallParam(const IVS_TVWALL_PARAM* pTVWallParam)
{
    if(!CToolsHelp::Memcpy(&m_stTVWallParam, sizeof(IVS_TVWALL_PARAM), pTVWallParam, sizeof(IVS_TVWALL_PARAM)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Set TVWall Param", "Memcpy failed!");
	}
}

void CPlayThread::invoke(void *argc)
{
    IVS_DEBUG_TRACE("");
    CPlayThread* pPlay = (CPlayThread*)argc;
	CHECK_POINTER_VOID(pPlay);
	CHECK_POINTER_VOID(pPlay->m_pTVWallMgr);
    IVS_INT32 iRet = pPlay->StartPlay();
    if(IVS_SUCCEED!=iRet)
    {//播放失败，通知上层
        BP_RUN_LOG_ERR(iRet, "Async Start Play", "Failed! Camera=%s, DecoderID=%s, Channel=%u", 
                        pPlay->m_szCameraCode, pPlay->m_stTVWallParam.cDecoderCode, pPlay->m_stTVWallParam.uiChannel);
        CUMW_NOTIFY_ASYNC_RET_EX stAsyncRet = {0};
        stAsyncRet.iRetCode = iRet;
        if(!CToolsHelp::Strncpy(stAsyncRet.szCameraCode, sizeof(stAsyncRet.szCameraCode), pPlay->m_szCameraCode, strlen(pPlay->m_szCameraCode)))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Async Start Play", "Copy CameraCode Failed");
		}
        if(!CToolsHelp::Strncpy(stAsyncRet.szDecoderID, sizeof(stAsyncRet.szDecoderID), pPlay->m_stTVWallParam.cDecoderCode, strlen(pPlay->m_stTVWallParam.cDecoderCode)))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Async Start Play", "Copy DecoderID Failed");
		}
        stAsyncRet.uiChannel = pPlay->m_stTVWallParam.uiChannel;
        pPlay->m_pTVWallMgr->NotifyAsyncReturn(stAsyncRet);
    }
    VOS_pthread_exit(NULL);
    //释放内存
    if(NULL!=pPlay->m_pVosThread)
    {
        VOS_free(pPlay->m_pVosThread);
        pPlay->m_pVosThread = NULL;
    }
    //删除自己
    IVS_DELETE(pPlay);
    pPlay = NULL;
}

CRealPlayThread::CRealPlayThread(void)
{
    memset(&m_stRealPlayParam, 0, sizeof(IVS_REALPLAY_PARAM));
}

CRealPlayThread::~CRealPlayThread(void)
{

}

void CRealPlayThread::SetRealPlayParam(const IVS_REALPLAY_PARAM* pRealPlayParam)
{
    if(!CToolsHelp::Memcpy(&m_stRealPlayParam, sizeof(IVS_REALPLAY_PARAM), pRealPlayParam, sizeof(IVS_REALPLAY_PARAM)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Set RealPlay Param", "Memcpy failed!");
	}
}

IVS_INT32 CRealPlayThread::StartPlay()
{
    CHECK_POINTER(m_pChannel, IVS_OPERATE_MEMORY_ERROR);
    IVS_INT32 iRet = m_pChannel->StartRealPlay(m_szCameraCode, m_stRealPlayParam, m_stTVWallParam);
    return iRet;
}
