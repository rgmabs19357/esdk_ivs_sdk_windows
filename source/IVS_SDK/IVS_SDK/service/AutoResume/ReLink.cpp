#include "ReLink.h"
#include "IVSCommon.h"
#include "bp_log.h"
#include "bp_def.h"
#include "UserMgr.h"
#include "SDKDef.h"
#include "EventCallBackJob.h"

CReLink::CReLink(void)
	: m_pUserMgr(NULL)
{
	m_pMutexLock = VOS_CreateMutex();

}

CReLink::~CReLink(void)
{
	try
	{
		ClearBusinessList();
		VOS_DestroyMutex(m_pMutexLock);
		m_pMutexLock = NULL;
		m_pUserMgr = NULL;
	}
	catch(...)
	{}
}

void CReLink::SetUserMgr(CUserMgr *pUserMgr)
{
	m_pUserMgr = pUserMgr;
}



void CReLink::onTrigger( void* /*pArg*/, ULONGLONG /*ullScales*/, TriggerStyle /*style*/ )
{
    CHECK_POINTER_VOID(m_pUserMgr);
    RELINK_LIST reLinkFail;

    (void)VOS_MutexLock(m_pMutexLock);

    RELINK_LIST reLinkSrc(m_listRelink);
    m_listRelink.clear();

    (void)VOS_MutexUnlock(m_pMutexLock);

    CRelinkInfo* pRelink = NULL;
    
    do 
    {
        if (!m_pUserMgr->GetRelinkFlag())
        {
            break;
        }

        if (reLinkSrc.empty())
        {
            break;
        }

        pRelink =  (CRelinkInfo*)reLinkSrc.front();
        reLinkSrc.pop_front();
   
        if (NULL == pRelink)
        {
            continue;
        }
        if (!pRelink->GetIsConnected())
        {
            //如果没有连上就重连
            int iRet = IVS_FAIL;
            // 重连OMU
            if (NET_ELE_OMU_NSS == pRelink->GetNetElemType())
            {
                iRet = m_pUserMgr->GetLoginMgr().ReLoginOmu(pRelink->GetIP(),pRelink->GetPort());
                if (IVS_SUCCEED == iRet)
                {
					//modify by zwx211831, 释放开辟的堆空间，避免内存泄露。Date：20140521
                    IVS_DELETE(pRelink);
                    continue;
                }
                else
                {
                    reLinkFail.push_back(pRelink);
                    // 重连失败，需要删除BP底层重连列表
                    (void)CNetFactory::Instance().DeleteClientLinkByAddr(pRelink->GetIP().c_str(), pRelink->GetPort(), pRelink->GetNetElemType());

                    BP_RUN_LOG_ERR(iRet, "resume on trigger,", "relink failed, target ip:[%s:%d], element type:[%d].", 
                        pRelink->GetIP().c_str(), pRelink->GetPort(), pRelink->GetNetElemType());
                }
            }
            else
            {
                // 如果IP对应的域在域列表缓存中不存在，则无须再重连， 场景: 某域被修改IP, CU未重新登录
                IVS_DOMAIN_ROUTE stDomainRoute;
                memset(&stDomainRoute, 0, sizeof(stDomainRoute));
                iRet = m_pUserMgr->GetDomainRouteMgr().GetDomainRouteByIP(pRelink->GetIP(), stDomainRoute);
                if (IVS_SUCCEED != iRet)
                {
                    IVS_CHAR pszDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
                    strncpy(pszDomainCode, stDomainRoute.cDomainCode, IVS_DOMAIN_CODE_LEN);
                    BP_RUN_LOG_INF("resume on trigger,", "domain:[%s-%s:%u] is not exist in domain route cache, no need to continue relinking", 
                        pszDomainCode, pRelink->GetIP().c_str(), pRelink->GetPort());
                    IVS_DELETE(pRelink);
                    continue;
                }

                long startTime = clock();
                iRet = m_pUserMgr->GetLoginMgr().ReLoginSMU(pRelink->GetIP(), pRelink->GetPort());
                long finishDet = clock() - startTime;

                if (IVS_SUCCEED == iRet || IVS_PASSWORD_ERROR == iRet 

                    || IVS_SMU_USER_PWD_NEED_MOD == iRet)
                {
                    BP_RUN_LOG_INF("resume on trigger,", "relink success, target ip:[%s:%d], element type:[%d], process interval:[%d ms].", 
                        pRelink->GetIP().c_str(), pRelink->GetPort(), pRelink->GetNetElemType(), finishDet);

                    // 不是本域的话，就不抛各种消息了
					if (!IsLocalDomain(pRelink))
					{
						BP_RUN_LOG_INF("not local domain", "no need report to cu");
						IVS_DELETE(pRelink);
						continue;
					}

					//如果重连成功就将用户的重连起始时间设为0
					m_pUserMgr->UpdateResumeStartTime((time_t)0);

                    IVS_DELETE(pRelink);
                    EventCallBack fnCallBack = m_pUserMgr->GetEventCallBack();
                    if (NULL == fnCallBack)
                    {
                        continue;
                    }
                    if (IVS_SUCCEED == iRet)
                    {
                        // 抛一个重连成功事件，并停止重连
                        fnCallBack(IVS_EVENT_AUTO_CONNECT_SUCCEED, NULL, 1, m_pUserMgr->GetEventUserData());
                        BP_RUN_LOG_INF("report to cu", "IVS_EVENT_AUTO_CONNECT_SUCCEED");
                    }
                    else if (IVS_PASSWORD_ERROR == iRet)
                    {
                        // 抛一个重连失败事件(场景：用户密码被修改；密码过期，SMU重启)，并停止重连
                        fnCallBack(IVS_EVENT_LOGIN_FAILED, NULL, 1, m_pUserMgr->GetEventUserData());
                        BP_RUN_LOG_INF("report to cu", "IVS_EVENT_LOGIN_FAILED");
                    }
                    // TODO 确认是否改为上抛密码过期事件
                    else
                    {
                        // 抛一个重连成功事件，并停止重连
                        fnCallBack(IVS_EVENT_AUTO_CONNECT_SUCCEED, NULL, 1, m_pUserMgr->GetEventUserData());
                        BP_RUN_LOG_INF("report to cu", "IVS_EVENT_AUTO_CONNECT_SUCCEED");
                    }
                    continue;
                }
                else
                {
					//add by zwx211831,Date:20140707,
					//单点登录 并且为本域，如果是SMU重启或者重连10分钟后仍然失败，则上抛10301事件，业务层需要调用logout后再次登录
					if (SSO_LOGIN == m_pUserMgr->GetLoginMgr().GetLoginType() && IsLocalDomain(pRelink))
					{
						time_t lResumeTimes = ::time(NULL) - m_pUserMgr->GetResumeStartTime();
						if (IVS_SMU_HAS_RESTARED == iRet || lResumeTimes >= SSO_RESUME_MAX_SEC)
						{
							if (IVS_SMU_HAS_RESTARED == iRet)
							{
								BP_RUN_LOG_INF("SMU has restared, SSO resume failed", "resume times: %d second", lResumeTimes);
							}
							if (lResumeTimes >= SSO_RESUME_MAX_SEC)
							{
								BP_RUN_LOG_INF("Try resume more than ten minute", "SSO resume times: %d second", lResumeTimes);
							}

							// 重连失败，需要删除BP底层重连列表，此时不需要添加到重连链表中。先删除链接后上报事件，防止业务层重连太快。
							(void)CNetFactory::Instance().DeleteClientLinkByAddr(pRelink->GetIP().c_str(), pRelink->GetPort(), pRelink->GetNetElemType());
							BP_RUN_LOG_ERR(iRet, "resume on trigger,", "relink failed, target ip:[%s:%d], element type:[%d], process interval:[%d ms].", 
								pRelink->GetIP().c_str(), pRelink->GetPort(), pRelink->GetNetElemType(), finishDet);

							IVS_DELETE(pRelink);

							//上报重连失败事件
							CEventCallBackJob::instance().PostEvent(m_pUserMgr->GetSessionID(), IVS_EVENT_LOGIN_FAILED, "", 1);

							//重连失败，将用户的重连起始时间设为0
							m_pUserMgr->UpdateResumeStartTime((time_t)0);

							continue;
						}
					}
					//end
                    reLinkFail.push_back(pRelink);

                    // 重连失败，需要删除BP底层重连列表
                    (void)CNetFactory::Instance().DeleteClientLinkByAddr(pRelink->GetIP().c_str(), pRelink->GetPort(), pRelink->GetNetElemType());
                    BP_RUN_LOG_ERR(iRet, "resume on trigger,", "relink failed, target ip:[%s:%d], element type:[%d], process interval:[%d ms].", 
                        pRelink->GetIP().c_str(), pRelink->GetPort(), pRelink->GetNetElemType(), finishDet);
                }
            }

        }
    } while (true);

    RELINK_LIST_ITER itorTempItem = reLinkFail.begin();
    for (; reLinkFail.end() != itorTempItem; itorTempItem++)
    {
        (void)AddLinkIdToBusinessList((*itorTempItem)->GetLinkID(), (*itorTempItem)->GetNetElemType(), (*itorTempItem)->GetIP(), (*itorTempItem)->GetPort());
        IVS_DELETE(*itorTempItem);
    }

}


int CReLink::AddLinkIdToBusinessList(const std::string& strLinkID,int iNetElemType,const std::string& strIP,int iPort)
{

	bool bRet = false;

    CLockGuard oReLinklock(m_pMutexLock);

    //(void)VOS_MutexLock(m_pMutexLock);

	CRelinkInfo* pRelink = NULL;
	RELINK_LIST_ITER InfoIter = m_listRelink.begin();
	for (; InfoIter != m_listRelink.end(); InfoIter++)
	{
		//如果找到
		pRelink = dynamic_cast<CRelinkInfo*>(*InfoIter);
        if (NULL == pRelink)
        {
            continue;
        }
		//CHECK_POINTER(pRelink,IVS_SDK_RET_INIT_RTSPCLIENT_ERROR);
		if (0 == pRelink->GetIP().compare(strIP) && pRelink->GetPort() == iPort)
		{
			bRet = true;
			pRelink->SetIsConnected(false);
			break;
		}
	}

	if (bRet)
	{
		//找到了
		BP_RUN_LOG_INF("AddLinkId","find linkID OK");

        //(void)VOS_MutexUnlock(m_pMutexLock);
		return IVS_FAIL;
	}

	//没找到第一次就插入
	CRelinkInfo* pNewRelink = IVS_NEW(pRelink);
    pNewRelink->Init(false,iNetElemType,iPort,strLinkID,strIP);

	m_listRelink.push_back(pNewRelink);

    BP_DBG_LOG("relink list update, add resume item, element type:[%d], target ip:[%s:%u].", iNetElemType, strIP.c_str(), iPort);
    
    //(void)VOS_MutexUnlock(m_pMutexLock);

	return IVS_SUCCEED;
}

//int CReLink::RemoveLinkIdFromBusinessList(const std::string& strLinkID)
//{
//
//
//	BP_RUN_LOG_INF("RemoveLinkId","%s",strLinkID.c_str());
//	bool bRet = false;
//	// CLockGuard lock(m_pMutexLock);
//    (void)VOS_MutexLock(m_pMutexLock);
//
//	CRelinkInfo* pRelink = NULL;
//	RELINK_LIST_ITER InfoIter = m_listRelink.begin();
//	RELINK_LIST_ITER InfoIterEnd = m_listRelink.end();
//	for (; InfoIter != InfoIterEnd; InfoIter++)
//	{
//		//如果找到
//		pRelink = (CRelinkInfo*)(*InfoIter);
//        if (NULL == pRelink)
//        {
//            continue;
//        }
//		//CHECK_POINTER(pRelink,IVS_SDK_RET_INIT_RTSPCLIENT_ERROR);
//		if (0 == pRelink->GetLinkID().compare(strLinkID))
//		{
//			bRet = true;
//
//			//如果找到就删除
//			m_listRelink.erase(InfoIter);
//			IVS_DELETE(pRelink);
//			break;
//		}
//	}
//
//    (void)VOS_MutexUnlock(m_pMutexLock);
//
//	if (bRet)
//	{
//		//找到了
//		BP_RUN_LOG_INF("RemoveLinkId","find linkID ok");
//		return IVS_SUCCEED;
//	}
//	else
//	{
//		BP_RUN_LOG_INF("RemoveLinkId","find linkID failed");
//		return IVS_FAIL;
//	}
//}

void CReLink::ClearBusinessList()
{
    CRelinkInfo* pRelink = NULL;
    (void)VOS_MutexLock(m_pMutexLock);

    RELINK_LIST_ITER InfoIter = m_listRelink.begin();
    RELINK_LIST_ITER InfoIterEnd = m_listRelink.end();
    for (; InfoIter != InfoIterEnd;)
    {
        pRelink = (CRelinkInfo*)(*InfoIter);
        if (NULL == pRelink)
        {
            InfoIter++;
            continue;
        }
        IVS_DELETE(pRelink);
        m_listRelink.erase(InfoIter++);
    }

    (void)VOS_MutexUnlock(m_pMutexLock);
}


//判断断线重连的域是否为本域
bool CReLink::IsLocalDomain(CRelinkInfo* pRelink)
{
	CHECK_POINTER(m_pUserMgr, false);
	CHECK_POINTER(pRelink, false);

	CUserInfo* pUserInfo = m_pUserMgr->GetLoginMgr().GetUserInfo();
	if (NULL != pUserInfo)
	{
		std::string strSerIP = pUserInfo->GetServerIP();
		if (0 != strSerIP.compare(pRelink->GetIP()))
		{
			return false;
		}

		return true;
	}

	return false;
}