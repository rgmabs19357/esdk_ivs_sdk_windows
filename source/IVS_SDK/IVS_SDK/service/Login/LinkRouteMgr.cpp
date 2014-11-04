/********************************************************************
 filename    :    LinkRouteMgr.cpp
 author      :    s00191067
 created     :    2012/11/26
 description :    网络Link路由管理类;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/26 初始版本
*********************************************************************/
#include "LinkRouteMgr.h"
#include "IVSCommon.h"
#include "bp_log.h"
#include "LockGuard.h"
#include "IVS_Trace.h"
using namespace BP;

CLinkRouteMgr::CLinkRouteMgr(void)
{
    m_pMutexLinkRouteMap = VOS_CreateMutex();
    m_LinkRouteMap.clear();
}

CLinkRouteMgr::~CLinkRouteMgr(void)
{
    try
    {
        // 释放map中List内存，清理map
        ClearAllLink();
        // 销毁锁
        if (NULL != m_pMutexLinkRouteMap)
        {
            VOS_DestroyMutex(m_pMutexLinkRouteMap);
            m_pMutexLinkRouteMap = NULL;
        }
    }
    catch (...)
    {
    }
}

// 添加一条连接信息
int CLinkRouteMgr::AddLinkInfo(int iSessionID, const CLinkInfo& oLinkIDInfo)
{
    IVS_DEBUG_TRACE("iSessionID:%d", iSessionID);
    CLinkInfo* poLinkIdInfo = IVS_NEW(poLinkIdInfo);
    if (NULL == poLinkIdInfo)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Add LinkInfo failed", "alloc Memory for pstLinkIdInfo error");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    // 拷贝Link内容
	*poLinkIdInfo = oLinkIDInfo;
	LINKID_INFO_LIST* pLinkInfoList = NULL;
	CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
	LINK_ROUTE_MAP_ITER linkRouteMapIter = m_LinkRouteMap.find(iSessionID);

    // map中没有iSessionID对应的List，创建List并添加LinkInfo
	if (linkRouteMapIter == m_LinkRouteMap.end())
	{
		pLinkInfoList = IVS_NEW(pLinkInfoList);
        if (NULL == pLinkInfoList)
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Add LinkInfo failed", "alloc Memory for pLinkInfoList error");
            IVS_DELETE(poLinkIdInfo);
            return IVS_ALLOC_MEMORY_ERROR;
        }
		pLinkInfoList->push_back(poLinkIdInfo);
		(void)m_LinkRouteMap.insert(make_pair(iSessionID, pLinkInfoList));
		return IVS_SUCCEED;
	}

    // map中存在iSessionID对应的List，直接向List添加
	pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteMapIter->second); //lint !e612
    if (NULL == pLinkInfoList)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Add LinkInfo failed", "pLinkInfoList is null");
        IVS_DELETE(poLinkIdInfo);
        return IVS_OPERATE_MEMORY_ERROR;
    }

	pLinkInfoList->push_back(poLinkIdInfo);
	return IVS_SUCCEED;
}

// 更新LinkInfo, 如有不存在，需要新加；如果已经存在，则更新状态
int CLinkRouteMgr::UpdateLinkInfo(int iSessionID, const CLinkInfo& oLinkIDInfo, bool bInsertOnly)
{

    CLinkInfo* poLinkIdInfo = IVS_NEW(poLinkIdInfo);
    if (NULL == poLinkIdInfo)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Add LinkInfo failed", "alloc Memory for pstLinkIdInfo error");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    LINKID_INFO_LIST* pLinkInfoList = NULL;
   	*poLinkIdInfo = oLinkIDInfo;
    CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
    LINK_ROUTE_MAP_ITER linkRouteMapIter = m_LinkRouteMap.find(iSessionID);
    // map中没有iSessionID对应的List，创建List并添加LinkInfo
    if (linkRouteMapIter == m_LinkRouteMap.end())
    {
        pLinkInfoList = IVS_NEW(pLinkInfoList);
        if (NULL == pLinkInfoList)
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Add LinkInfo failed", "alloc Memory for pLinkInfoList error");
            IVS_DELETE(poLinkIdInfo);
            return IVS_ALLOC_MEMORY_ERROR;
        }
        pLinkInfoList->push_back(poLinkIdInfo);
        (void)m_LinkRouteMap.insert(make_pair(iSessionID, pLinkInfoList));
        return IVS_SUCCEED;
    }

    // map中存在iSessionID对应的List，直接向List添加
    pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteMapIter->second); //lint !e612
    if (NULL == pLinkInfoList)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Add LinkInfo failed", "pLinkInfoList is null");
        IVS_DELETE(poLinkIdInfo);
        return IVS_OPERATE_MEMORY_ERROR;
    }

    LINKID_INFO_LIST_ITER itor = pLinkInfoList->begin();
	LINKID_INFO_LIST_ITER itorEnd = pLinkInfoList->end();
    for (; itorEnd != itor; itor++)
    {
        if (0 ==  strncmp(oLinkIDInfo.GetStrDomainCode().c_str(), (*itor)->GetStrDomainCode().c_str(), IVS_DOMAIN_CODE_LEN) 
            && (*itor)->GetNetElemType() == oLinkIDInfo.GetNetElemType())
        {
            // 如果cache中存在域编码相同，但IP不一样的item，则需要请求刷新状态（close or enable）
            if (bInsertOnly && 0 == strncmp(oLinkIDInfo.GetStrDomainIP().c_str(), (*itor)->GetStrDomainIP().c_str(), IVS_IP_LEN))
            {
                // 当前只允许插入，不允许被更新
				//add by zwx211831 释放内存，避免内存泄露。Date： 20140520
				IVS_DELETE(poLinkIdInfo);
                return IVS_SUCCEED;
            }
            
            (*itor)->Init(poLinkIdInfo->GetNetElemType(), oLinkIDInfo.GetLinkStatus(), 
                oLinkIDInfo.GetStrLinkId(), oLinkIDInfo.GetStrDomainCode(), oLinkIDInfo.GetStrDomainIP(), oLinkIDInfo.GetStrNVRCode());
            IVS_DELETE(poLinkIdInfo);
            break;
        }
    }

    if (pLinkInfoList->end() == itor)
    {
        pLinkInfoList->push_back(poLinkIdInfo);
    }
    return IVS_SUCCEED;
}


// 删除指定的一条连接
int CLinkRouteMgr::RemoveLinkInfo(int iSessionID, const std::string& strLinkId)
{
    IVS_DEBUG_TRACE("iSessionID:%d, LinkId:%s", iSessionID, strLinkId.c_str());
	CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
	LINK_ROUTE_MAP_ITER linkRouteMapIter = m_LinkRouteMap.find(iSessionID);
	if (linkRouteMapIter == m_LinkRouteMap.end())
	{
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Remove LinkInfo failed", "can't find LINKID_INFO_LIST by iSessionID");
		return IVS_FAIL;
	}

	LINKID_INFO_LIST* pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteMapIter->second); //lint !e612
	if (NULL == pLinkInfoList)
	{
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Remove LinkInfo failed", "pLinkInfoList is null");
        return IVS_FAIL;
	}

	LINKID_INFO_LIST_ITER linkInfoListIter = pLinkInfoList->begin();
    LINKID_INFO_LIST_ITER linkInfoListIterEnd = pLinkInfoList->end();
	for (; linkInfoListIter != linkInfoListIterEnd;)
	{
        CLinkInfo* poLinkInfo = *linkInfoListIter;
        if (NULL == poLinkInfo)
        {
            linkInfoListIter++;
            continue;
        }

		if (0 == strLinkId.compare(poLinkInfo->GetStrLinkId()))
		{
			CNetFactory::Instance().DeleteClientLink(poLinkInfo->GetStrLinkId(), poLinkInfo->GetNetElemType());
			IVS_DELETE(poLinkInfo);
			pLinkInfoList->erase(linkInfoListIter++);
            continue;
		}
         linkInfoListIter++; //lint !e539
	}
    return IVS_SUCCEED;
}



// 删除指定用户的所有连接
void CLinkRouteMgr::DeleteLinkList(int iSessionID)
{
    IVS_DEBUG_TRACE("iSessionID:%d", iSessionID);
	CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
	LINK_ROUTE_MAP_ITER linkRouteMapIter = m_LinkRouteMap.find(iSessionID);
	if (linkRouteMapIter == m_LinkRouteMap.end())
	{
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Delete LinkList failed", "can't find LINKID_INFO_LIST by iSessionID");
		return;
	}
	LINKID_INFO_LIST* pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteMapIter->second); //lint !e612
    if (NULL == pLinkInfoList)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Delete LinkList failed", "pLinkInfoList is null");
        return;
    }
	// 释放list中的所有linkinfo内存
	LINKID_INFO_LIST_ITER linkInfoListIter = pLinkInfoList->begin();
    LINKID_INFO_LIST_ITER linkInfoListIterEnd = pLinkInfoList->end();
	for (; linkInfoListIter != linkInfoListIterEnd; linkInfoListIter++)
	{
		CLinkInfo* poLinkInfoTmp = *linkInfoListIter;
        if (NULL != poLinkInfoTmp)
        {
            CNetFactory::Instance().DeleteClientLink(poLinkInfoTmp->GetStrLinkId(), poLinkInfoTmp->GetNetElemType());
            IVS_DELETE(poLinkInfoTmp);
        }
	}
    pLinkInfoList->clear();
	// 释放list
	IVS_DELETE(pLinkInfoList);
	// map中删除list
	m_LinkRouteMap.erase(linkRouteMapIter);
}

// 根据连接查找iSessionID
int CLinkRouteMgr::FindSessionIDByLinkID(const std::string& strLinkId)
{
    IVS_DEBUG_TRACE("strLinkId:%s", strLinkId.c_str());
	if (strLinkId.empty())
	{
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Find SessionID By LinkID failed", "strLinkId is empty");
		return SDK_SESSIONID_INIT;
	}
	CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
    LINK_ROUTE_MAP_ITER linkRouteMapIter = m_LinkRouteMap.begin();
    LINK_ROUTE_MAP_ITER linkRouteMapIterEnd = m_LinkRouteMap.end();
	for (; linkRouteMapIter != linkRouteMapIterEnd; linkRouteMapIter++)
	{
		LINKID_INFO_LIST* pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteMapIter->second); //lint !e612
		if (NULL == pLinkInfoList)
		{
			continue;
		}
		LINKID_INFO_LIST_ITER linkInfoListIter = pLinkInfoList->begin();
        LINKID_INFO_LIST_ITER linkInfoListIterEnd = pLinkInfoList->end();
		for (; linkInfoListIter != linkInfoListIterEnd; linkInfoListIter++)
		{
            CLinkInfo* poLinkIdInfo = *linkInfoListIter;
            if (NULL == poLinkIdInfo)
            {
                continue;
            }
			if (0 == strLinkId.compare(poLinkIdInfo->GetStrLinkId()))
			{
                int iSessionID = linkRouteMapIter->first;
				return iSessionID;
			}
		}
	}
    BP_RUN_LOG_ERR(IVS_FAIL, "Find SessionID By LinkID failed", "can't Find SessionID By LinkID");
	return SDK_SESSIONID_INIT;
}

// 查找连接详情
bool CLinkRouteMgr::FindLinkIDInfo(const std::string& strLinkId, CLinkInfo &oLinkIDInfo)
{
    IVS_DEBUG_TRACE("strLinkId:%s", strLinkId.c_str());
	if (strLinkId.empty())
	{
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Find LinkID Info failed", "strLinkId is empty");
		return false;
	}
	CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
	LINK_ROUTE_MAP_ITER linkIter = m_LinkRouteMap.begin();
    LINK_ROUTE_MAP_ITER linkRouteMapIterEnd = m_LinkRouteMap.end();
	for (; linkIter != linkRouteMapIterEnd; linkIter++)
	{
		LINKID_INFO_LIST* pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkIter->second); //lint !e612
		if (NULL == pLinkInfoList)
		{
			continue;
		}
		LINKID_INFO_LIST_ITER linkInfoListIter = pLinkInfoList->begin();
        LINKID_INFO_LIST_ITER linkInfoListIterEnd = pLinkInfoList->end();
		for (; linkInfoListIter != linkInfoListIterEnd; linkInfoListIter++)
		{
            CLinkInfo* poLinkIdInfo = *linkInfoListIter;
            if (NULL == poLinkIdInfo)
            {
                continue;
            }

			if (0 == strLinkId.compare(poLinkIdInfo->GetStrLinkId()))
			{
				oLinkIDInfo = *poLinkIdInfo;
				return true;
			}
		}
	}
    BP_RUN_LOG_ERR(IVS_FAIL,"Find LinkID Info failed", "can't find stLinkIDInfo");
	return false;
}

// 根据linkid 设置link状态
void CLinkRouteMgr::SetLinkIDStatus(const std::string& strLinkId, int iLinkStatus)
{
    IVS_DEBUG_TRACE("strLinkId:%s, iLinkStatus:%d", strLinkId.c_str(), iLinkStatus);
	if (strLinkId.empty() || iLinkStatus < LINKID_STATUS_OPENNING || iLinkStatus > LINKID_STATUS_CLOSED)
	{
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Set LinkID Status failed", "strLinkId is empty or iLinkStatus error");
		return;
	}
	CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
	LINK_ROUTE_MAP_ITER linkRouteIterTemp = m_LinkRouteMap.begin();
    LINK_ROUTE_MAP_ITER linkRouteMapIterEnd = m_LinkRouteMap.end();
	for (; linkRouteIterTemp != linkRouteMapIterEnd; linkRouteIterTemp++)
	{
		LINKID_INFO_LIST* pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteIterTemp->second); //lint !e612
		if (NULL == pLinkInfoList)
		{
			continue;
		}
		LINKID_INFO_LIST_ITER linkInfoListIter = pLinkInfoList->begin();
        LINKID_INFO_LIST_ITER linkInfoListIterEnd = pLinkInfoList->end();
		for (; linkInfoListIter != linkInfoListIterEnd; linkInfoListIter++)
		{
            CLinkInfo* poLinkIdInfo = *linkInfoListIter;
            if (NULL == poLinkIdInfo)
            {
                continue;
            }
			if (0 == strLinkId.compare(poLinkIdInfo->GetStrLinkId()))
			{
				poLinkIdInfo->SetLinkStatus(iLinkStatus);
				return;
			}
		}
	}
    BP_RUN_LOG_ERR(IVS_FAIL,"Set LinkID Status failed", "can't find stLinkIDInfo");
}

// 更新linkID,linkID状态(重登录成功后，需更新LinkInfo的LinkID以及iLinkStatus)
void CLinkRouteMgr::SetLinkIDByDomainCode(int iSessionID, int iNetElemType, const std::string& strDomainCode, const std::string& strLinkID, int iLinkStatus)
{
    IVS_DEBUG_TRACE("iSessionID:%d, strDomainCode:%s, strLinkID:%s, iLinkStatus:%d", iSessionID, strDomainCode.c_str(), strLinkID.c_str(), iLinkStatus);
    if (iLinkStatus < LINKID_STATUS_OPENNING || iLinkStatus > LINKID_STATUS_CLOSED)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Set SmuLinkID By DomainCode failed", "iLinkStatus error");
        return;
    }
    if (strDomainCode.empty() || strLinkID.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Set SmuLinkID By DomainCode failed", "strDomainCode or strLinkId is empty");
        return;
    }
    CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
    LINK_ROUTE_MAP_ITER linkRouteMapIter = m_LinkRouteMap.find(iSessionID);
    if (linkRouteMapIter == m_LinkRouteMap.end())
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"Set SmuLinkID By DomainCode failed", "can't find linkRoute in m_LinkRouteMap");
        return;
    }
    LINKID_INFO_LIST* pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteMapIter->second); //lint !e612
    if (NULL == pLinkInfoList)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"Set SmuLinkID By DomainCode failed", "pLinkInfoList is null");
        return;
    }
    LINKID_INFO_LIST_ITER linkInfoListIter = pLinkInfoList->begin();
    LINKID_INFO_LIST_ITER linkInfoListIterEnd = pLinkInfoList->end();
    for (; linkInfoListIter != linkInfoListIterEnd; linkInfoListIter++)
    {
        CLinkInfo* poLinkIdInfo = *linkInfoListIter;
        if (NULL == poLinkIdInfo)
        {
            continue;
        }
        if (iNetElemType == poLinkIdInfo->GetNetElemType() 
            && 0 == poLinkIdInfo->GetStrDomainCode().compare(strDomainCode))
        {
            poLinkIdInfo->SetStrLinkId(strLinkID);
            poLinkIdInfo->SetLinkStatus(iLinkStatus);
            return;
        }
    }
}

// 更新本域CODE为strDomainCode(用户登陆时会添加一个域CODE为初始化DOMAIN_CODE_INIT的LinkID)
void CLinkRouteMgr::UpdateLocalDomainCode(int iSessionID, const std::string& strDomainCode)
{
    IVS_DEBUG_TRACE("iSessionID:%d, strDomainCode:%s", iSessionID, strDomainCode.c_str());
    if (strDomainCode.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Update Local DomainCode failed", "strLinkId is empty");
        return;
    }
	CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
    LINK_ROUTE_MAP_ITER linkRouteMapIter = m_LinkRouteMap.find(iSessionID);
    if (linkRouteMapIter == m_LinkRouteMap.end())
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"Update Local DomainCode failed", "can't find linkRoute , m_LinkRouteMap size:%d", m_LinkRouteMap.size());
        return;
    }
    LINKID_INFO_LIST* pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteMapIter->second); //lint !e612
    if (NULL == pLinkInfoList)
    {
		BP_RUN_LOG_ERR(IVS_FAIL,"Update Local DomainCode failed", "pLinkInfoList is null");
        return;
    }
    LINKID_INFO_LIST_ITER linkInfoListIter = pLinkInfoList->begin();
    LINKID_INFO_LIST_ITER linkInfoListIterEnd = pLinkInfoList->end();
    for (; linkInfoListIter != linkInfoListIterEnd; linkInfoListIter++)
    {
        CLinkInfo* poLinkIdInfo = *linkInfoListIter;
        if (NULL == poLinkIdInfo)
        {
            continue;
        }
        if ( 0 == poLinkIdInfo->GetStrDomainCode().compare(LOCAL_DOMAIN_CODE_INIT))
        {
            poLinkIdInfo->SetStrDomainCode(strDomainCode);
            return;
        }
    }
    BP_RUN_LOG_INF("Update Local DomainCode failed", "pLinkInfoList size:%d", pLinkInfoList->size());
}

//根据iSessionID，域编码查找SMU的LinkIDInfo信息
bool CLinkRouteMgr::GetSmuLinkIDInfoByDomainCode(int iSessionID, const std::string &strDomaiCode, CLinkInfo &oLinkIDInfo)
{
    return GetLinkIDInfoByDomainCode(iSessionID, NET_ELE_SMU_NSS, strDomaiCode, oLinkIDInfo);
}

// 根据iSessionId，域编码和NVRCode 查询linkIDInfo
bool CLinkRouteMgr::GetScuLinkByDomainCodeAndNVRCode(int iSessionID, const std::string &strDomaiCode, const std::string &strNVRCode, CLinkInfo &oLinkIDInfo)
{
    IVS_DEBUG_TRACE("iSessionID：%d, strDomaiCode:%s, strNVRCode:%s", iSessionID, strDomaiCode.c_str(), strNVRCode.c_str());
    if (strDomaiCode.empty() || strNVRCode.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get ScuLink By DomainCodeAndNVRCode failed", "strLinkId or strNVRCode is empty");
        return false;
    }
    // 加锁，根据iSessionID查找用户的LinkList
	CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
    LINK_ROUTE_MAP_ITER linkRouteMapIter = m_LinkRouteMap.find(iSessionID);
    if (linkRouteMapIter == m_LinkRouteMap.end())
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Get ScuLink By DomainCodeAndNVRCode failed", "can't find linkRoute in m_LinkRouteMap");
        return false;
    }

    LINKID_INFO_LIST* pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteMapIter->second); //lint !e612
    if (NULL == pLinkInfoList)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Get ScuLink By DomainCodeAndNVRCode failed", "pLinkInfoList is null");
        return false;
    }

    // 根据iNetElemType、strDomaiCode查找LinkInfo
    LINKID_INFO_LIST_ITER linkInfoListIter = pLinkInfoList->begin();
    LINKID_INFO_LIST_ITER linkInfoListIterEnd = pLinkInfoList->end();
    for (; linkInfoListIter != linkInfoListIterEnd; linkInfoListIter++)
    {
        CLinkInfo* poLinkIdInfo = *linkInfoListIter;
        if (NULL == poLinkIdInfo)
        {
            continue;
        }

        if (NET_ELE_SCU_NSS == poLinkIdInfo->GetNetElemType() 
            && 0 == strDomaiCode.compare(poLinkIdInfo->GetStrDomainCode()) 
            && 0 == strNVRCode.compare(poLinkIdInfo->GetStrNVRCode()) )
        {
            oLinkIDInfo = *poLinkIdInfo;
            return true;
        }
    }
    BP_RUN_LOG_ERR(IVS_FAIL,"Get ScuLink By DomainCodeAndNVRCode failed", "can't find ScuLinkID");
    return false;
}

// 根据域编码查询连接详情
bool CLinkRouteMgr::GetLinkIDInfoByDomainCode(int iSessionID, int iNetElemType, const std::string &strDomaiCode, CLinkInfo &oLinkIDInfo)
{
    IVS_DEBUG_TRACE("iSessionID:%d, iNetElemType: %d, strDomaiCode:%s", iSessionID, iNetElemType, strDomaiCode.c_str());
    if (strDomaiCode.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"Get LinkIDInfo By DomainCode failed", "strLinkId is empty");
        return false;
    }

    // 加锁，根据iSessionID查找用户的LinkList
	CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
    LINK_ROUTE_MAP_ITER linkRouteMapIter = m_LinkRouteMap.find(iSessionID);
    if (linkRouteMapIter == m_LinkRouteMap.end())
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Get LinkIDInfo By DomainCode failed", "can't find linkRoute in m_LinkRouteMap");
        return false;
    }

    LINKID_INFO_LIST* pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteMapIter->second); //lint !e612
    if (NULL == pLinkInfoList)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Get LinkIDInfo By DomainCode failed", "pLinkInfoList is null");
        return false;
    }

    // 根据iNetElemType、strDomaiCode查找LinkInfo
    LINKID_INFO_LIST_ITER linkInfoListIter = pLinkInfoList->begin();
    LINKID_INFO_LIST_ITER linkInfoListIterEnd = pLinkInfoList->end();
    for (; linkInfoListIter != linkInfoListIterEnd; linkInfoListIter++)
    {
        CLinkInfo* poLinkIdInfo = *linkInfoListIter;
        if (NULL == poLinkIdInfo)
        {
            continue;
        }

        if (iNetElemType == poLinkIdInfo->GetNetElemType() && 0 == strDomaiCode.compare(poLinkIdInfo->GetStrDomainCode()) )
        {
			oLinkIDInfo = *poLinkIdInfo;
            return true;
        }
    }
    BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Get LinkIDInfo By DomainCode failed", "can't find pstLinkIdInfo in pLinkInfoList");
    return false;
}


//判断域编码是否有效
IVS_INT32 CLinkRouteMgr::DecideDomainCode(int iSessionID, int iNetElemType, const std::string &strDomaiCode)
{
	IVS_DEBUG_TRACE("iSessionID：%d, iNetElemType: %d, strDomaiCode:%s", iSessionID, iNetElemType, strDomaiCode.c_str());
	if (strDomaiCode.empty())
	{
		BP_RUN_LOG_ERR(IVS_CODE_INVALID, "DomainCode is empty", "NA");
		return IVS_CODE_INVALID;
	}

	// 加锁，根据iSessionID查找用户的LinkList
	CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
	LINK_ROUTE_MAP_ITER linkRouteMapIter = m_LinkRouteMap.find(iSessionID);
	if (linkRouteMapIter == m_LinkRouteMap.end())
	{
		BP_RUN_LOG_ERR(IVS_SDK_RET_INVALID_SESSION_ID,"Decide DomainCode failed", "SessionID is Invalid");
		return IVS_SDK_RET_INVALID_SESSION_ID;
	}

	LINKID_INFO_LIST* pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteMapIter->second); //lint !e612
	if (NULL == pLinkInfoList)
	{
		BP_RUN_LOG_ERR(IVS_SMU_USER_OFF_LINE,"Decide DomainCode failed", "pLinkInfoList is null");
		return IVS_SMU_USER_OFF_LINE;
	}

	// 根据iNetElemType、strDomaiCode查找LinkInfo
	LINKID_INFO_LIST_ITER linkInfoListIter = pLinkInfoList->begin();
	LINKID_INFO_LIST_ITER linkInfoListIterEnd = pLinkInfoList->end();
	for (; linkInfoListIter != linkInfoListIterEnd; linkInfoListIter++)
	{
		CLinkInfo* poLinkIdInfo = *linkInfoListIter;
		if (NULL == poLinkIdInfo)
		{
			continue;
		}

		if (iNetElemType == poLinkIdInfo->GetNetElemType() && 0 == strDomaiCode.compare(poLinkIdInfo->GetStrDomainCode()) )
		{
			return IVS_SUCCEED;
		}
	}
	BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"DomainCode not Exist", "NA");
	return IVS_SDK_ERR_DOMAINCODE_NOT_EXIST;
}

// 删除除指定类型，域编码的其他连接
void CLinkRouteMgr::DeleteLinkExceptLocalDomain(int iSessionID, int iNetElemType, const std::string &strDomaiCode)
{
    IVS_DEBUG_TRACE("iSessionID:%d, iNetElemType:%d, strDomaiCode:%d", iSessionID, iNetElemType, strDomaiCode.c_str());
    CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
    LINK_ROUTE_MAP_ITER linkRouteMapIter = m_LinkRouteMap.find(iSessionID);
    if (linkRouteMapIter == m_LinkRouteMap.end())
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Delete LinkList ExceptLocalDomain failed", "can't find LINKID_INFO_LIST by iSessionID");
        return;
    }
    LINKID_INFO_LIST* pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteMapIter->second); //lint !e612
    if (NULL == pLinkInfoList)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"Delete LinkList ExceptLocalDomain failed", "pLinkInfoList is null");
        return;
    }
    // 释放list中的所有linkinfo内存
    LINKID_INFO_LIST_ITER linkInfoListIter = pLinkInfoList->begin();
    LINKID_INFO_LIST_ITER linkInfoListIterEnd = pLinkInfoList->end();
    for (; linkInfoListIter != linkInfoListIterEnd;)
    {
        CLinkInfo* poLinkInfo = *linkInfoListIter;
        if (NULL == poLinkInfo)
        {
            linkInfoListIter++;
            continue;
        }
        // 删除外域连接,本域SCU连接
        if (0 != strDomaiCode.compare(poLinkInfo->GetStrDomainCode()) || NET_ELE_SCU_NSS == poLinkInfo->GetNetElemType())
        {
            CNetFactory::Instance().DeleteClientLink(poLinkInfo->GetStrLinkId(), poLinkInfo->GetNetElemType());
            IVS_DELETE(poLinkInfo);
            pLinkInfoList->erase(linkInfoListIter++);
            continue;
        }
        // OMU连接，仅删除TCP重连
        if (NET_ELE_OMU_NSS == poLinkInfo->GetNetElemType())
        {
            CNetFactory::Instance().DeleteClientLink(poLinkInfo->GetStrLinkId(), poLinkInfo->GetNetElemType());
        }
        linkInfoListIter++; //lint !e539
    }
}

// 释放所有List中内存，释放List，清除map
void CLinkRouteMgr::ClearAllLink()
{
    // 加锁
	CLockGuard cLinkRouteMapLock(m_pMutexLinkRouteMap);
	LINK_ROUTE_MAP_ITER linkRouteIter = m_LinkRouteMap.begin();
	LINK_ROUTE_MAP_ITER linkRouteMapIterEnd = m_LinkRouteMap.end();
	for (;linkRouteIter != linkRouteMapIterEnd; linkRouteIter++)
	{
	    LINKID_INFO_LIST* pLinkInfoList = dynamic_cast<LINKID_INFO_LIST*>(linkRouteIter->second); //lint !e612
	    if (NULL == pLinkInfoList)
	    {
		    continue;
	    }
	    // 释放list中的所有linkinfo内存
	    LINKID_INFO_LIST_ITER linkInfoListIter = pLinkInfoList->begin();
	    LINKID_INFO_LIST_ITER linkInfoListIterEnd = pLinkInfoList->end();
	    for (; linkInfoListIter != linkInfoListIterEnd; linkInfoListIter++)
	    {
            CLinkInfo* poLinkInfoTmp = *linkInfoListIter;
            if (NULL != poLinkInfoTmp)
            {
                CNetFactory::Instance().DeleteClientLink(poLinkInfoTmp->GetStrLinkId(), poLinkInfoTmp->GetNetElemType());
                IVS_DELETE(poLinkInfoTmp);
            }
	    }
	    pLinkInfoList->clear();
	    // 释放list
	    IVS_DELETE(pLinkInfoList);
	}
    // 清除map中所有list
    m_LinkRouteMap.clear();
}

