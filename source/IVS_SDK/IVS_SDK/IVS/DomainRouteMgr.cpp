/********************************************************************
 filename    :    DomainRouteMgr.cpp
 author      :    x00192614
 created     :    2013/01/17
 description :    域路由管理
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/17 初始版本
 *********************************************************************/
#include "DomainRouteMgr.h"
#include "NSSOperator.h"
#include "nss_mt.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "XmlProcess.h"
#include "ToolsHelp.h"
#include "UserMgr.h"
#include "IVS_Trace.h"
#include "devEnum.h"
CDomainRouteMgr::CDomainRouteMgr(void)
    : m_pUserMgr(NULL)
{}

CDomainRouteMgr::~CDomainRouteMgr(void)
{
    m_pUserMgr = NULL;
}

IVS_INT32 CDomainRouteMgr::GetDomainRoute()
{
    BP_RUN_LOG_INF("Get DomainRoute", "Enter");
    IVS_INT32 iRet = IVS_FAIL;
    CXml xmlRsp;
    iRet = SendCmdAndGetInfo(xmlRsp);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get DomainRoute", "Get DomainRoute Info Error");
        return iRet;
    }

    DOMAIN_CODE_LIST stCodeInfoList;
	iRet = GetDomainRouteToListParseXML(stCodeInfoList, xmlRsp);
    if (IVS_SUCCEED == iRet)
    {
		ClearDomainRouteMap();
        DOMAIN_CODE_LIST_ITER iterBegin = stCodeInfoList.begin();
        DOMAIN_CODE_LIST_ITER iterEnd = stCodeInfoList.end();
//         DOMAIN_CODE_LIST_MAP_ITER mapIter;
//         DOMAIN_CODE_LIST_MAP_ITER mapIterEnd = m_domainCodeMap.end();
        char szDomainCode[IVS_DOMAIN_CODE_LEN + 1];
        unsigned int uiCopyLen = IVS_DOMAIN_CODE_LEN;
        IVS_DOMAIN_ROUTE* pstDomainRout = NULL;
        for (; iterBegin != iterEnd; iterBegin++)
        {
            pstDomainRout = *iterBegin;
            if (NULL == pstDomainRout)
            {
                continue;
            }
            //确保数组最后一位为“/0”
            memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
            (void)CToolsHelp::Memcpy(szDomainCode, uiCopyLen, pstDomainRout->cDomainCode, uiCopyLen);
            //插入Map表
            (IVS_VOID)m_domainCodeMap.insert(std::make_pair(szDomainCode, pstDomainRout));
        }
        stCodeInfoList.clear();
    }
    m_proxyDomainCodeMap.clear(); // 清除代理域map
    BP_RUN_LOG_INF("Get DomainRoute", "Leave");
    return iRet;
}

IVS_INT32 CDomainRouteMgr::SendCmdAndGetInfo(CXml& xmlRsp) const
{
    IVS_INT32 iRet = IVS_FAIL;
    CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
    //创建要发送的CMD，拼装了NSS消息头
    CCmd* pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_GET_DOMAIN_ROUTE_REQ, NULL, strSMULinkID);

    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

    //发送消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);

    const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
    HW_DELETE(pCmdRsp);
    CHECK_POINTER(pRsp, iRet);
    if (IVS_SUCCEED != iRet)
    {
		 BP_RUN_LOG_INF("Send Command get return value", " Error");
		 IVS_DELETE(pRsp, MUILI); //lint !e605
		 return IVS_FAIL;
	}
	iRet = IVS_XML_INVALID;
    BP_RUN_LOG_INF("Get domain route ", "pRsp:%s", pRsp);
    if (xmlRsp.Parse(pRsp))
    {
		iRet = IVS_SUCCEED;
    }
    IVS_DELETE(pRsp, MUILI); //lint !e605
    return iRet;
}

//域路由
IVS_INT32 CDomainRouteMgr::GetDomainRoute(IVS_DOMAIN_ROUTE_LIST* pDomainRouteList, IVS_UINT32 uiBufferSize )
{
    BP_RUN_LOG_INF("Get DomainRoute", "Enter");
    CHECK_POINTER(pDomainRouteList, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    //如果超出最大的数量则返回错误,一个系统最多128*8个域
	//IVS_UINT32 uiMapLen = m_domainCodeMap.size();
	if ((IVS_MAX_DOMAIN_ROUTE_NUM * 8) < m_domainCodeMap.size())
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get DomainRoute", "Request Buffer Size Not Enough, DomainCount[%d].", m_domainCodeMap.size());
		return IVS_OPERATE_MEMORY_ERROR;
	}

    CXml xmlRsp;
    IVS_INT32 iRet = SendCmdAndGetInfo(xmlRsp);
    if (IVS_SUCCEED == iRet)
    {
        iRet = GeDomainRouteParseXML(pDomainRouteList, xmlRsp);
    }
    m_proxyDomainCodeMap.clear(); // 清除代理域map


    IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
    for (unsigned int i = 0;  i < pDomainRouteList->uiTotal; i++)
    {
        memset(szDomainCode, 0, sizeof(szDomainCode));
        strncpy(szDomainCode, pDomainRouteList->stDomainRoute[i].cDomainCode, IVS_DOMAIN_CODE_LEN);
        if (pDomainRouteList->stDomainRoute[i].bIsLocalDomain)
        {
            BP_DBG_LOG("get domain list, Local domain:[%s-%s:%u] is online.", szDomainCode,
                pDomainRouteList->stDomainRoute[i].stIP.cIP, pDomainRouteList->stDomainRoute[i].uiPort);
            continue;
        }
        CLinkInfo oLinkIDInfoTemp;

        if (DEV_STATUS_ONLINE == pDomainRouteList->stDomainRoute[i].uiStatus)
        {
            oLinkIDInfoTemp.Init(NET_ELE_SMU_NSS, LINKID_STATUS_ENABLED, "", szDomainCode, pDomainRouteList->stDomainRoute[i].stIP.cIP, "");

  
            (void)CLinkRouteMgr::instance().UpdateLinkInfo(m_pUserMgr->GetSessionID(), oLinkIDInfoTemp, true);

            BP_DBG_LOG("get domain list, domain:[%s-%s:%u] is online.", szDomainCode,
                pDomainRouteList->stDomainRoute[i].stIP.cIP, pDomainRouteList->stDomainRoute[i].uiPort);
        }
        else
        {
            oLinkIDInfoTemp.Init(NET_ELE_SMU_NSS, LINKID_STATUS_CLOSED, "", szDomainCode,  pDomainRouteList->stDomainRoute[i].stIP.cIP, "");
            (void)CLinkRouteMgr::instance().UpdateLinkInfo(m_pUserMgr->GetSessionID(), oLinkIDInfoTemp, true);

            // 当前域不可用，添加到重连列表中(键值为 IP + Port)
            m_pUserMgr->AddLinkIdToBusinessResumeList("", NET_ELE_SMU_NSS, pDomainRouteList->stDomainRoute[i].stIP.cIP, 
                pDomainRouteList->stDomainRoute[i].uiPort, TYPE_ADD_RELINK);

            BP_RUN_LOG_INF("get domain list,", "domain:[%s-%s:%u] is offline, have been push into resume list.", szDomainCode,
                pDomainRouteList->stDomainRoute[i].stIP.cIP, pDomainRouteList->stDomainRoute[i].uiPort);

        }
        
    }

    BP_RUN_LOG_INF("CDeviceMgr::GetDomainRoute", "Leave");
    return iRet;
}//lint !e529 !e830

// 解析域路由到List
IVS_INT32 CDomainRouteMgr::GetDomainRouteToListParseXML(DOMAIN_CODE_LIST& stCodeInfoList, CXml &xmlRsp)
{
    BP_RUN_LOG_INF("Parse XML to List", "Enter");

    if (!xmlRsp.FindElemEx("Content/DomainRoutes"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse XML to List",
                       "no 'Content/DomainRoutes' elemenet");
        return IVS_XML_INVALID;
    }

    if (!xmlRsp.FindElem("DomainRoute"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse XML to List",
                       "no 'DevInfo' elemenet");
        return IVS_XML_INVALID;
    }

    const IVS_CHAR *pSzElement = NULL;
    IVS_UINT32 uiSTLen = sizeof(IVS_DOMAIN_ROUTE);
    const int tempLength = 10;
    IVS_CHAR tempChar[tempLength]={0};
    do
    {
        if (!xmlRsp.FindElemValue("DomainRoute"))
        {
            break;
        }

        xmlRsp.IntoElem();

        IVS_DOMAIN_ROUTE *pDomain_route = IVS_NEW(pDomain_route);

        if (NULL == pDomain_route)
        {
            return IVS_FAIL;
        }

        memset(pDomain_route, 0, uiSTLen);

        //域编码
        GET_ELEM_VALUE_CHAR("DomainCode", pSzElement, pDomain_route->cDomainCode, IVS_DOMAIN_CODE_LEN, xmlRsp);

        //域名称
        GET_ELEM_VALUE_CHAR("DomainName", pSzElement, pDomain_route->cDomainName, IVS_NAME_LEN, xmlRsp);

        //域IP
        GET_ELEM_VALUE_CHAR("DomainIP", pSzElement, pDomain_route->stIP.cIP, IVS_IP_LEN, xmlRsp);

        //域端口
        //GET_ELEM_VALUE_NUM_FOR_UINT("DomainPort", pSzElement, pDomain_route->uiPort, xmlRsp);

        //域模式
        GET_ELEM_VALUE_NUM_FOR_UINT("DomainType", pSzElement, pDomain_route->uiDomainType, xmlRsp);

        //是否代理
        GET_ELEM_VALUE_NUM("IsProxy", pSzElement, pDomain_route->bIsAgent, xmlRsp);

        //代理网段
        //GET_ELEM_VALUE_NUM("ProxySegment", pSzElement, stDomainRoute.bIsAgent, xmlRsp);

        //上级域编码
        GET_ELEM_VALUE_CHAR("SuperDomain", pSzElement, pDomain_route->cSuperDomain, IVS_DOMAIN_CODE_LEN, xmlRsp);

        GET_ELEM_VALUE_CHAR("IsOnline", pSzElement, tempChar, sizeof(tempChar) - 1, xmlRsp);

        pDomain_route->uiStatus = CDevEnum::GetDevStatusEnum(tempChar);

        //是否本域
        GET_ELEM_VALUE_NUM("IsLocalDomain", pSzElement, pDomain_route->bIsLocalDomain, xmlRsp);


        pDomain_route->uiPort = DEFAULT_SMU_PORT;
        
        stCodeInfoList.push_back(pDomain_route);
        xmlRsp.OutOfElem();
    } while (xmlRsp.NextElem());

    BP_RUN_LOG_INF("Parse XML to List", "Leave");
    return IVS_SUCCEED;
} //lint !e1762

// 解析获取域路由响应
IVS_INT32 CDomainRouteMgr::GeDomainRouteParseXML( IVS_DOMAIN_ROUTE_LIST* pDomainRouteList, CXml &xmlRsp)
{
    BP_RUN_LOG_INF("Get Parse Domain Route XML Info", "Enter");
    CHECK_POINTER(pDomainRouteList, IVS_OPERATE_MEMORY_ERROR);

    DOMAIN_CODE_LIST stCodeInfoList;
    IVS_INT32 iRet = IVS_FAIL;

    iRet = GetDomainRouteToListParseXML(stCodeInfoList, xmlRsp);
    if (IVS_SUCCEED == iRet)
    {
        pDomainRouteList->uiTotal = stCodeInfoList.size();
        DOMAIN_CODE_LIST_ITER iterBegin = stCodeInfoList.begin();
        DOMAIN_CODE_LIST_ITER iterEnd = stCodeInfoList.end();
        DOMAIN_CODE_LIST_MAP_ITER mapIter;
        DOMAIN_CODE_LIST_MAP_ITER mapIterEnd = m_domainCodeMap.end();
        IVS_UINT32 uiSTLen = sizeof(IVS_DOMAIN_ROUTE);
        IVS_UINT32 uiIndex = 0;
        char szDomainCode[IVS_DOMAIN_CODE_LEN + 1];
        unsigned int uiCopyLen = IVS_DOMAIN_CODE_LEN;
        for (; iterBegin != iterEnd; iterBegin++)
        {
            if (NULL == *iterBegin)
            {
                continue;
            }

            IVS_DOMAIN_ROUTE &stDomainRouteTmp = pDomainRouteList->stDomainRoute[uiIndex];
            memset(&stDomainRouteTmp, 0, sizeof(stDomainRouteTmp));

            //赋值设备信息到pDomainRouteList

            if (!CToolsHelp::Memcpy(&stDomainRouteTmp, uiSTLen, *iterBegin, uiSTLen))
            {
                BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Parse Domain Route XML Info", "DomainRoute Memcpy error.");
                return IVS_ALLOC_MEMORY_ERROR;
            }

            memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
            strncpy(szDomainCode, (*iterBegin)->cDomainCode, IVS_DOMAIN_CODE_LEN);

            mapIter = m_domainCodeMap.find(szDomainCode);
            if (mapIter != mapIterEnd)
            {
                //说明Map里面已近存在了一个相同key值的对象，需要删除否则内存泄漏
                IVS_DELETE(mapIter->second); //lint !e1025 !e1514 !e64
                m_domainCodeMap.erase(mapIter);
            }

            //更新Map表
            (IVS_VOID)m_domainCodeMap.insert(std::make_pair(szDomainCode, *iterBegin));
            uiIndex++;
        }

        stCodeInfoList.clear();
    }
    else
    {
        pDomainRouteList->uiTotal = 0;
    }

    // TODO for 通用请求
    BP_RUN_LOG_INF("Get Parse Domain Route XML Info", "Leave");
    return iRet;
}

//查询外部域列表
IVS_INT32 CDomainRouteMgr::GetExDomainList(const IVS_CHAR* pQueryInfo,IVS_CHAR** pRspXml)
{
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	if (NULL == pQueryInfo)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get ExDomain List", "Query Info is NULL");
		return IVS_FAIL;
	}
    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	//创建要发送的CMD，拼装了NSS消息头
	CCmd* pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_GET_EXDOMAIN_LIST_REQ, pQueryInfo,strSMULinkID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
	//发送消息
	IVS_INT32 iRet = IVS_FAIL;
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	*pRspXml = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);
	return iRet;
}//lint !e1762

//目录共享
IVS_INT32 CDomainRouteMgr::SetShareCatalog(const IVS_CHAR* pShareInfo)
{
	if (NULL == pShareInfo)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Share Cata log", "Cmd is NULL");
		return IVS_FAIL;
	}
	std::string strShareCatalog(pShareInfo);
	std::string strSendCmd;

	CXml xml;
	if (!xml.Parse(pShareInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pShareInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	//if (!xml.FindElemEx("Content/ExDomainInfo/ExDomainCode"))
	//{
	//	BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/ExDomainInfo) fail", "NA");
	//	return IVS_XML_INVALID;
	//}
	//const IVS_CHAR* pExDomainCode = xml.GetElemValue();

	IVS_INT32 iRet = SplitDevDomainCode(strShareCatalog,strSendCmd);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Share Cata log", "XML illegal ");
		return iRet;
	}

	// 构造带域的请求消息，并发送
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_SHARE_CATALOG_REQ);
	sendNssMsgInfo.SetReqData(strSendCmd.c_str());	
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	return iRet;
}//lint !e1762


IVS_INT32 CDomainRouteMgr::GetShareCatalogList(const IVS_CHAR* pQueryInfo, IVS_CHAR** pRspXml)
{
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	if (NULL == pQueryInfo)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Share Cata log List", "Query Info is NULL");
		return IVS_FAIL;
	}
    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	CCmd* pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_GET_SHARE_CATALOG_LIST_REQ, pQueryInfo, strSMULinkID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
	//发送消息
	IVS_INT32 iRet = IVS_FAIL;
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	const char* pBefore =  CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	if (IVS_SUCCEED != iRet || NULL == pBefore)
	{
		HW_DELETE(pCmdRsp);
		BP_RUN_LOG_ERR(iRet, "Share Cata log list", "XML illegal ");
		return iRet;
	}
	HW_DELETE(pCmdRsp);
	std::string strBefore = pBefore;
	IVS_DELETE(pBefore, MUILI);
	//std::string strBefore("<Content><ShareDevList><ShareDevInfo><OriginDevCode>123456</OriginDevCode><OriginDomainCode>0123456789</OriginDomainCode><LocalDevCode>0123#001</LocalDevCode></ShareDevInfo></ShareDevList></Content>");
	//std::string strBefore("<OriginDevCode>00000010000000000301</OriginDevCode><OriginDomainCode>001</OriginDomainCode><Latitude>0</Latitude><OriginDevCode>00000010000000000201#001</OriginDevCode><OriginDomainCode>001</OriginDomainCode><LocalDevCode /><Latitude>0</Latitude><OriginDevCode>00000010000000000101#001</OriginDevCode><OriginDomainCode>001</OriginDomainCode><OriginDevCode>1</OriginDevCode><OriginDomainCode /><LocalDevCode />");
	std::string strAfter;
	iRet = ComplicationDevDomainCode(strBefore,strAfter);

	CXml xml;
	std::string strDomainCode;
	m_pUserMgr->GetDomainCode(strDomainCode);
	std::string strLocalDevCode;
	if (!xml.Parse(strAfter.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "xml.Parse(strAfter) fail", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	if (!xml.FindElemEx("Content/ShareDevList"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_DEV_REQ_XML_INVALID, "xml.FindElemEx(Content/ShareDevList) fail", "NA");
		return IVS_SMU_DEV_REQ_XML_INVALID;
	}
	if (!xml.FindElem("ShareDevInfo"))
	{
		IVS_UINT32 uiXmlLen = 0;
		const IVS_CHAR* pRsp = xml.GetXMLStream(uiXmlLen);
        if (NULL == pRsp)
        {
            return IVS_SMU_DEV_REQ_XML_INVALID;
        }

		IVS_CHAR *cTmp = IVS_NEW(cTmp,strlen(pRsp) + 1);
		if (NULL == cTmp)
		{
			return IVS_ALLOC_MEMORY_ERROR;
		}
		memset(cTmp, 0, strlen(pRsp) + 1);
		memcpy(cTmp, pRsp, strlen(pRsp));
		*pRspXml = cTmp;
		return IVS_SUCCEED;
	}
	do 
	{
		(void)xml.IntoElem();
		if (!xml.FindElem("LocalDevCode"))
		{
			BP_RUN_LOG_ERR(IVS_SMU_DEV_REQ_XML_INVALID, "xml.FindElem(LocalDevCode) fail", "NA");
			return IVS_SMU_DEV_REQ_XML_INVALID;
		}
        const IVS_CHAR* pLocalDevCode =  xml.GetElemValue();
        if (NULL != pLocalDevCode)
        {
            strLocalDevCode = pLocalDevCode;
        }

		if (0 != strcmp(strLocalDevCode.c_str(), ""))
		{
			strLocalDevCode.append("#");
			strLocalDevCode.append(strDomainCode);
			xml.ModifyElemValue(strLocalDevCode.c_str());
		}
		xml.OutOfElem();
	} while (xml.NextElem());

	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pRsq = xml.GetXMLStream(uiLen);

    if (NULL == pRsq)
    {
        return IVS_SMU_DEV_REQ_XML_INVALID;
    }

	IVS_CHAR *cTmp = IVS_NEW(cTmp,strlen(pRsq) + 1);
	if (NULL == cTmp)
	{
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(cTmp, 0, strlen(pRsq) + 1);
	memcpy(cTmp, pRsq, strlen(pRsq));
	*pRspXml = cTmp;

	return iRet;
}//lint !e1762


// 修改域路由
IVS_INT32 CDomainRouteMgr::ModifyDomainRoute(IVS_DOMAIN_ROUTE* pDomainRoute)
{
    if (NULL == pDomainRoute)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Modify Domain Route", "DomainRoute is NULL");
        return IVS_FAIL;
    }

    DOMAIN_CODE_LIST_MAP_ITER mapIter;
    char szDomainCode[IVS_DOMAIN_CODE_LEN + 1];
    unsigned int uiCopyLen = IVS_DOMAIN_CODE_LEN;

    memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
    if (!CToolsHelp::Memcpy(szDomainCode, uiCopyLen, pDomainRoute->cDomainCode, uiCopyLen))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Modify Domain Route", "DomainCode Memcpy error.");
        return IVS_ALLOC_MEMORY_ERROR;
    }

    mapIter = m_domainCodeMap.find(szDomainCode);

    //Map中存在相同Key的域路由信息
    if (mapIter != m_domainCodeMap.end())
    {
        IVS_DELETE(mapIter->second); //lint !e1025 !e1514 !e64
        m_domainCodeMap.erase(mapIter);
    }

    m_domainCodeMap.insert(std::make_pair(szDomainCode, pDomainRoute)); //lint !e534
    return IVS_SUCCEED;
}

IVS_INT32 CDomainRouteMgr::ModifyProxyDomainRoute(IVS_DOMAIN_ROUTE* pDomainRoute)
{
    if (NULL == pDomainRoute)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Modify Proxy Domain Route", "DomainRoute is NULL");
        return IVS_FAIL;
    }

    PROXY_DOMAIN_CODEL_MAP_ITER mapIter;
    DOMAIN_CODE_LIST_MAP_ITER damainCodeIter;
    DOMAIN_CODE_LIST_MAP_ITER damainCodeEndIter = m_domainCodeMap.end();
    char szDomainCode[IVS_DOMAIN_CODE_LEN + 1];
    unsigned int uiCopyLen = IVS_DOMAIN_CODE_LEN;
    IVS_DOMAIN_ROUTE* pProxyDomainRoute = NULL;
    std::string strFatherDomainCode;
    unsigned int uiMapSize = m_domainCodeMap.size();

    //定义循环次数限制，防止死循环发生
    unsigned int loopNum = 0;

    memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
    if (!CToolsHelp::Memcpy(szDomainCode, uiCopyLen, pDomainRoute->cDomainCode, uiCopyLen))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Modify Proxy Domain Route", "DomainCode Memcpy error.");
        return IVS_ALLOC_MEMORY_ERROR;
    }

    mapIter = m_proxyDomainCodeMap.find(szDomainCode);

    //Map中存在相同Key的域路由信息
    if (mapIter != m_proxyDomainCodeMap.end())
    {
        IVS_DELETE(mapIter->second); //lint !e1025 !e1514 !e64
        m_proxyDomainCodeMap.erase(mapIter);
    }

    do
    {
        loopNum++;

        //找到第一个代理域则退出
        if (TRUE == pDomainRoute->bIsAgent)
        {
            pProxyDomainRoute = pDomainRoute; //lint !e611
            break;
        }

        memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
        if (!CToolsHelp::Memcpy(szDomainCode, uiCopyLen, pDomainRoute->cSuperDomain, uiCopyLen))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Modify Proxy Domain Route", "DomainCode Memcpy error.");
            return IVS_ALLOC_MEMORY_ERROR;
        }

        //获取该域的父域Code
        strFatherDomainCode = szDomainCode;

        //查找父域在Map表中的信息
        damainCodeIter = m_domainCodeMap.find(strFatherDomainCode);

        //如果该域不存在父域信息，说明已经是本域，即根节点
        if (damainCodeIter == damainCodeEndIter)
        {
            break;
        }

        pDomainRoute = dynamic_cast <IVS_DOMAIN_ROUTE*>(damainCodeIter->second); //lint !e611
    } while (loopNum < uiMapSize);

    if (NULL != pProxyDomainRoute)
    {
        memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);

		if (NULL != pDomainRoute)
		{
			if (!CToolsHelp::Memcpy(szDomainCode, uiCopyLen, pDomainRoute->cDomainCode, uiCopyLen))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Modify Proxy Domain Route", "DomainCode Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
		}
        m_proxyDomainCodeMap.insert(std::make_pair(szDomainCode, pProxyDomainRoute)); //lint !e534
    }

    return IVS_SUCCEED;
}

// 删除域路由
IVS_VOID CDomainRouteMgr::ClearDomainRouteMap()
{
    //删除域路由
    DOMAIN_CODE_LIST_MAP_ITER domain_map_iter = m_domainCodeMap.begin();
    DOMAIN_CODE_LIST_MAP_ITER domain_map_iter_end = m_domainCodeMap.end();

    while (domain_map_iter != domain_map_iter_end)
    {
        IVS_DOMAIN_ROUTE* pRoute = dynamic_cast <IVS_DOMAIN_ROUTE*>(domain_map_iter->second); //lint !e611
        IVS_DELETE(pRoute);
        m_domainCodeMap.erase(domain_map_iter++);
    }

    m_domainCodeMap.clear();
}

//删除代理域路由
IVS_VOID CDomainRouteMgr::ClearProxyDomainRouteMap()
{
    //删除代理域路由
    PROXY_DOMAIN_CODEL_MAP_ITER proxy_domain_map_iter = m_proxyDomainCodeMap.begin();
    PROXY_DOMAIN_CODEL_MAP_ITER proxy_domain_map_iter_end = m_proxyDomainCodeMap.end();

    while (proxy_domain_map_iter != proxy_domain_map_iter_end)
    {
        IVS_DOMAIN_ROUTE* pProxyRoute = dynamic_cast <IVS_DOMAIN_ROUTE*>(proxy_domain_map_iter->second); //lint !e611
        IVS_DELETE(pProxyRoute);
        m_proxyDomainCodeMap.erase(proxy_domain_map_iter++);
    }

    m_proxyDomainCodeMap.clear();
}

//根据map填充结构体
IVS_INT32 CDomainRouteMgr::GetDomainRouteStruct( IVS_DOMAIN_ROUTE_LIST* pDomainRouteList, IVS_UINT32 uiReqNum )
{
    BP_RUN_LOG_INF("Get Domain Route Info ", "Enter");

    IVS_UINT32 uiLen = m_domainCodeMap.size();

    //若map里面没有值，或者请求的大于最大值，则返回，告诉上层，要重新获取
    if ((0 == uiLen) || (uiLen > IVS_MAX_DOMAIN_ROUTE_NUM))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Get Domain Route Info", "Can not find Domain Route Info");
        ClearDomainRouteMap();
        return IVS_FAIL;
    }

    DOMAIN_CODE_LIST_MAP_ITER domainInter = m_domainCodeMap.begin();
    DOMAIN_CODE_LIST_MAP_ITER domainInterEnd = m_domainCodeMap.end();
    IVS_UINT32 uiStLen = sizeof(IVS_DOMAIN_ROUTE);

    //这里要跟请求的数量进行比较
    for (IVS_UINT32 uiIndex = 0; uiIndex < uiReqNum && domainInter != domainInterEnd; uiIndex++, domainInter++)
    {
        IVS_DOMAIN_ROUTE &stRoute = pDomainRouteList->stDomainRoute[uiIndex];
        if (NULL != domainInter->second)
        {
            memcpy(&stRoute, domainInter->second, uiStLen);
        }
    }

    pDomainRouteList->uiTotal = uiLen;
    //pDomainRouteList->stIndexRange.uiFromIndex = 1;
    //pDomainRouteList->stIndexRange.uiToIndex = uiLen;
    BP_RUN_LOG_INF("Get Domain Route Info", "Leave");
    return IVS_SUCCEED;
}

// 设置用户对象指针
void CDomainRouteMgr::SetUserMgr(CUserMgr *pUserMgr)
{
    m_pUserMgr = pUserMgr;
}


// 根据域编码查询代理域
bool CDomainRouteMgr::FindProxyDomainCode(const std::string& strDestDomainCode, std::string& strProxyDomainCode)
{
    IVS_DEBUG_TRACE("strDestDomainCode: %s",strDestDomainCode.c_str());
    DOMAIN_CODE_LIST_MAP_ITER doMainCodeIter = m_domainCodeMap.find(strDestDomainCode);
    DOMAIN_CODE_LIST_MAP_ITER doMainCodeIterEnd = m_domainCodeMap.end();

    if (doMainCodeIter == doMainCodeIterEnd)
    {
        BP_RUN_LOG_INF("Find Proxy Domain Route Info", "Can not find DomainRoute Info");
        return false;
    }

    //查找当前域编码是否存在代理域
    PROXY_DOMAIN_CODEL_MAP_ITER proxydoMainCodeIter = m_proxyDomainCodeMap.find(strDestDomainCode);
    if ((proxydoMainCodeIter != m_proxyDomainCodeMap.end()) && (NULL != proxydoMainCodeIter->second))
    {
        BP_RUN_LOG_INF("Find Proxy Domain Route Info", "m_proxyDomainCodeMap size:%d",m_proxyDomainCodeMap.size());
        IVS_DOMAIN_ROUTE* pProxyDomainRouteTemp = dynamic_cast <IVS_DOMAIN_ROUTE*>(proxydoMainCodeIter->second);     //lint !e611
        if (NULL == pProxyDomainRouteTemp)
        {
            BP_RUN_LOG_INF("Find Proxy Domain Route Info", "pProxyDomainRoute is null"); // 正常逻辑不会进此分支
            return false;
        }
        char cDomainCodeTmp[IVS_DOMAIN_CODE_LEN + 1] = {0};
        (void)CToolsHelp::Memcpy(cDomainCodeTmp, IVS_DOMAIN_CODE_LEN, pProxyDomainRouteTemp->cDomainCode,IVS_DOMAIN_CODE_LEN); //lint !e1013
        strProxyDomainCode = cDomainCodeTmp;
        BP_RUN_LOG_INF("Find Proxy Domain Route Info", "m_proxyDomainCodeMap exist strProxyDomainCode:%s",strProxyDomainCode.c_str());
        return true;
    }

    //如果不存在域编码，查找当前域的代理域，并将代理域加入Map表中
    IVS_DOMAIN_ROUTE* pProxyDomainRoute = NULL;
    IVS_DOMAIN_ROUTE* pDomainRoute = NULL;
    std::string strFatherDomainCode;
    unsigned int uiMapSize = m_domainCodeMap.size();
    unsigned int loopNum = 0; //定义循环次数限制，防止死循环发生
    char szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
    unsigned int uiCopyLen = IVS_DOMAIN_CODE_LEN;

    //查找到该域的第一个代理域
    pDomainRoute = dynamic_cast <IVS_DOMAIN_ROUTE*>(doMainCodeIter->second);     //lint !e611
    //循环查找代理域信息
    do
    {
        if (NULL == pDomainRoute)
        {
            break;
        }

        loopNum++;

        //找到第一个代理域则退出
        if (TRUE == pDomainRoute->bIsAgent)
        {
            pProxyDomainRoute = pDomainRoute;
            break;
        }

        memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
        if (!CToolsHelp::Memcpy(szDomainCode, uiCopyLen, pDomainRoute->cSuperDomain, uiCopyLen))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Find Proxy Domain Route Info", "DomainCode Memcpy error.");
            return false;
        }

        //获取该域的父域Code
        strFatherDomainCode = szDomainCode;

        //查找父域在Map表中的信息
        doMainCodeIter = m_domainCodeMap.find(strFatherDomainCode);

        //如果该域不存在父域信息，说明已经是本域，即根节点
        if (doMainCodeIter == doMainCodeIterEnd)
        {
            break;
        }

        pDomainRoute = dynamic_cast <IVS_DOMAIN_ROUTE*>(doMainCodeIter->second);     //lint !e611
    } while (loopNum < uiMapSize);

    if ((NULL != pProxyDomainRoute) && (NULL != pDomainRoute))
    {
        memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN);
        if (!CToolsHelp::Memcpy(szDomainCode, uiCopyLen, pProxyDomainRoute->cDomainCode, uiCopyLen))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Find Proxy Domain Route Info", "DomainCode Memcpy error.");
            return false;
        }

        strProxyDomainCode = szDomainCode;
        m_proxyDomainCodeMap.insert(std::make_pair(strDestDomainCode, pProxyDomainRoute));     //lint !e534
        BP_RUN_LOG_INF("Find Proxy Domain Route Info", "m_proxyDomainCodeMap insert strDestDomainCode:%s,strProxyDomainCode:%s",strDestDomainCode.c_str(),strProxyDomainCode.c_str());
        return true; // 添加进代理域MAP，也视为查询成功
    }
    BP_RUN_LOG_INF("Find Proxy Domain Route Info", "Can not find DomainRoute Info");
    return false;
}

// 获取域信息
IVS_INT32 CDomainRouteMgr::GetDomainRoutebyCode(const std::string& strDomainCode, IVS_DOMAIN_ROUTE& stDomainRoute)
{
    DOMAIN_CODE_LIST_MAP_ITER doMainIter = m_domainCodeMap.find(strDomainCode);

    if ((doMainIter != m_domainCodeMap.end()) && (NULL != doMainIter->second))
    {
        if (!CToolsHelp::Memcpy(&stDomainRoute, sizeof(IVS_DOMAIN_ROUTE), doMainIter->second, sizeof(IVS_DOMAIN_ROUTE)))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Domain Route Info By Code", "DomainRoute Memcpy error.");
            return IVS_ALLOC_MEMORY_ERROR;
        }

        return IVS_SUCCEED;
    }

    BP_RUN_LOG_ERR(IVS_FAIL, "Get Domain Route Info By Code","Can not find Domain Route");
    return IVS_FAIL;
}
IVS_INT32 CDomainRouteMgr::GetDomainRouteByIP(const std::string& strIP, IVS_DOMAIN_ROUTE& stDomainRoute)
{
    DOMAIN_CODE_LIST_MAP_ITER doMainIter = m_domainCodeMap.begin();
	DOMAIN_CODE_LIST_MAP_ITER doMainIterEnd = m_domainCodeMap.end();
    for (; doMainIterEnd != doMainIter; doMainIter++)
    {
        if (0 == strIP.compare(doMainIter->second->stIP.cIP))
        {
            stDomainRoute = *doMainIter->second;
            return IVS_SUCCEED;
        }
        
    }

    return IVS_FAIL;
}

// 查找登录域的域编码（上级域编码为空即为登录的域） add by xiongfeng 192614
IVS_INT32 CDomainRouteMgr::GetLocalDomainCode(std::string &strLocalDomainCode)
{
    DOMAIN_CODE_LIST_MAP_ITER iter = m_domainCodeMap.begin();
    DOMAIN_CODE_LIST_MAP_ITER iterEnd = m_domainCodeMap.end();
    char szDomainCode[IVS_DOMAIN_CODE_LEN + 1];
    unsigned int uiCopyLen = IVS_DOMAIN_CODE_LEN;
    std::string strDomainCodeTemp;

    for (; iter != iterEnd; iter++)
    {
        IVS_DOMAIN_ROUTE* pDomainRoute = dynamic_cast <IVS_DOMAIN_ROUTE*>(iter->second); //lint !e611
        if (NULL == pDomainRoute)
        {
            continue;
        }
        //char cSupDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
        //(void)CToolsHelp::Memcpy(cSupDomainCode, IVS_DOMAIN_CODE_LEN, pDomainRoute->cSuperDomain,IVS_DOMAIN_CODE_LEN);
        //strDomainCodeTemp = cSupDomainCode;
        // 本域为1，不是本域为0
        if (!pDomainRoute->bIsLocalDomain)
        {
            continue;
        }

        // 上级域为空的域为本域
        memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
        if (!CToolsHelp::Memcpy(szDomainCode, uiCopyLen, pDomainRoute->cDomainCode, uiCopyLen))
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get local Domain Route", "DomainRoute Memcpy error.");
            return IVS_ALLOC_MEMORY_ERROR;
        }

        strLocalDomainCode = szDomainCode;
        return IVS_SUCCEED;
    }

    BP_RUN_LOG_ERR(IVS_FAIL, "Get local Domain Route", "can't find LocalDomainCode");
    return IVS_FAIL;
}

IVS_INT32 CDomainRouteMgr::GetSameSuperDomainCode(const std::string& strDomainCode1,
	const std::string& strDomainCode2, std::string& strSuperDomainCode)
{
	DOMAIN_CODE_LIST_MAP_ITER iter = m_domainCodeMap.begin();
	DOMAIN_CODE_LIST_MAP_ITER iterEnd = m_domainCodeMap.end();
	if (iter == iterEnd)
	{
		BP_RUN_LOG_INF("Get Same Super Domain Info", "No Domain Info");
		return IVS_FAIL;
	}
	IVS_INT32 iRet = IVS_FAIL;
	SUPER_DOMAIN_CODE_LIST superDomainCodeList1;	//域1的所有上级域List
	SUPER_DOMAIN_CODE_LIST superDomainCodeList2;	//域2的所有上级域List
	iRet = GetSuperDomainCodeList(superDomainCodeList1,strDomainCode1);
	iRet += GetSuperDomainCodeList(superDomainCodeList2,strDomainCode2);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_INF("Get Same Super Domain Info", "Find Super Domain Info Error");
		return IVS_FAIL;
	}
	//如果其中一个没有
	if (superDomainCodeList1.size() >= superDomainCodeList2.size())
	{
		iRet = FindSameSuperDomainCode(superDomainCodeList1,superDomainCodeList2,strSuperDomainCode);
	}
	else
	{
		iRet = FindSameSuperDomainCode(superDomainCodeList2,superDomainCodeList1,strSuperDomainCode);
	}
	return iRet;
}

bool CDomainRouteMgr::IsInDomainTree(const std::string& strDomainCode)
{
	CHECK_POINTER(m_pUserMgr, false);
	bool bOut = false;
	std::string strFindDomain = strDomainCode;
	std::string localDomainCode;
	IVS_DOMAIN_ROUTE* pDomainRoute = NULL;
	m_pUserMgr->GetDomainCode(localDomainCode);
	for (unsigned int loopNum = 0;loopNum < m_domainCodeMap.size();loopNum++)
	{
		if (0 == strFindDomain.compare(localDomainCode))
		{
			bOut = true;
			break;
		}
		// 找上级域编码
		// 一直向上找，如果最顶级不是localdomain就返回true
		DOMAIN_CODE_LIST_MAP_ITER iter = m_domainCodeMap.find(strDomainCode);
		DOMAIN_CODE_LIST_MAP_ITER iterEnd = m_domainCodeMap.end();
		if (iter != iterEnd)
		{
			char szDomainCode[IVS_DOMAIN_CODE_LEN + 1];
			//取出查找的域信息
			pDomainRoute = dynamic_cast <IVS_DOMAIN_ROUTE*>(iter->second);//lint !e611
			if (NULL == pDomainRoute)
			{
				continue;
			}
			//拷贝出父域信息
			memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
			if (!CToolsHelp::Memcpy(szDomainCode, IVS_DOMAIN_CODE_LEN, pDomainRoute->cSuperDomain, IVS_DOMAIN_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Super Domain Route Info", "DomainCode Memcpy error.");
				continue;
			}
			//获取该域的父域Code
			strFindDomain = szDomainCode;
			continue;
		}
		break;
	}

	return bOut;
}

IVS_INT32 CDomainRouteMgr::GetSuperDomainCodeList(SUPER_DOMAIN_CODE_LIST& superDomainCodeList, const std::string& strDomainCode)
{
	DOMAIN_CODE_LIST_MAP_ITER iter = m_domainCodeMap.find(strDomainCode);
	DOMAIN_CODE_LIST_MAP_ITER iterEnd = m_domainCodeMap.end();
	if (iter == iterEnd)
	{
		BP_RUN_LOG_INF("Get Super Domain Info", "No Find Domain Info");
		return IVS_FAIL;
	}
	std::string strFatherDomainCode;
	char szDomainCode[IVS_DOMAIN_CODE_LEN + 1];
	unsigned int uiCopyLen = IVS_DOMAIN_CODE_LEN;
	unsigned int uiMapSize = m_domainCodeMap.size();
	unsigned int loopNum = 0; //定义循环次数限制，防止死循环发生
	IVS_DOMAIN_ROUTE* pDomainRoute = NULL;
	//先将自己所在的域信息加入到List（解决如果当前域就是最上级域的问题）
	superDomainCodeList.push_back(strDomainCode);
	//递归查找某个域的所有上级域信息
	do 
	{
		//取出查找的域信息
		pDomainRoute = dynamic_cast <IVS_DOMAIN_ROUTE*>(iter->second);//lint !e611
		if (NULL == pDomainRoute)
		{
			return IVS_FAIL;
		}
		//拷贝出父域信息
		memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
		if (!CToolsHelp::Memcpy(szDomainCode, uiCopyLen, pDomainRoute->cSuperDomain, uiCopyLen))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Super Domain Route Info", "DomainCode Memcpy error.");
			return false;
		}
		//获取该域的父域Code
		strFatherDomainCode = szDomainCode;
		//查找父域在Map表中的信息
		iter = m_domainCodeMap.find(strFatherDomainCode);
		if (iter == iterEnd)
		{
			break;
		}
		//如果父域信息在Map表中可以找到，将父域信息加入List
		superDomainCodeList.push_back(strFatherDomainCode);
	} while (loopNum < uiMapSize);

	return IVS_SUCCEED;
}

IVS_INT32 CDomainRouteMgr::FindSameSuperDomainCode(SUPER_DOMAIN_CODE_LIST& superDomainCodeList1,
	SUPER_DOMAIN_CODE_LIST& superDomainCodeList2, std::string& strSuperDomainCode)
{
	unsigned int  iSizeTolist1 = superDomainCodeList1.size();
	unsigned int  iSizeTolist2 = superDomainCodeList2.size();
	if (iSizeTolist1 < iSizeTolist2)
	{
		BP_RUN_LOG_INF("Find Same Super Domain Code", "Use Function Error");
		return IVS_FAIL;
	}
	//所有list的迭代器初始化
	SUPER_DOMAIN_CODE_LIST_ITER list1Iter = superDomainCodeList1.begin();
	SUPER_DOMAIN_CODE_LIST_ITER list1IterEnd = superDomainCodeList1.end();
	SUPER_DOMAIN_CODE_LIST_ITER list2Iter = superDomainCodeList2.begin();
	SUPER_DOMAIN_CODE_LIST_ITER list2IterEnd = superDomainCodeList2.end();
	//list的大小差异
	unsigned int  iDifSize = iSizeTolist1 - iSizeTolist2;
	//首先查找list1比list2多出的部分
	for (unsigned int i = 0; i < iDifSize ; i++)
	{
		//如果父域相同
		if (0 == strcmp((*list1Iter).c_str(),(*list2Iter).c_str()))
		{
			strSuperDomainCode = *list1Iter;
			return IVS_SUCCEED;
		}
		list1Iter++;
	}
	//剩余相等部分
	for (;list1Iter != list1IterEnd && list2Iter != list2IterEnd ;)
	{
		//如果父域相同
		if (0 == strcmp((*list1Iter).c_str(),(*list2Iter).c_str()))
		{
			strSuperDomainCode = *list1Iter;
			return IVS_SUCCEED;
		}
		list1Iter++;
		list2Iter++;
	}
	return IVS_FAIL;
}//lint !e1762

IVS_INT32 CDomainRouteMgr::GetAllDomainCode(DOMAIN_CODE_VEC& stDomainCodeVec )
{
	DOMAIN_CODE_LIST_MAP_ITER iter = m_domainCodeMap.begin();
	DOMAIN_CODE_LIST_MAP_ITER iterEnd = m_domainCodeMap.end();
	IVS_DOMAIN_ROUTE* pDomainRoute = NULL;
    CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	for (;iter != iterEnd ; iter++)
	{
		pDomainRoute = dynamic_cast <IVS_DOMAIN_ROUTE*>(iter->second);//lint !e611
		if (NULL == pDomainRoute)
		{
			continue;
		}
        memset(szDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
        if (CToolsHelp::Memcpy(szDomainCode, IVS_DOMAIN_CODE_LEN, pDomainRoute->cDomainCode, IVS_DOMAIN_CODE_LEN))
        {
            stDomainCodeVec.push_back(szDomainCode);
        }
	}
	return IVS_SUCCEED;
}

IVS_INT32 CDomainRouteMgr::SplitDevDomainCode(std::string& strBefore, std::string& strAfter)
{
	//找到第一个OriginDevCode
	std::string::size_type pStartPos = strBefore.find("<OriginDevCode>");
	bool bHasFind = false;
	while(std::string::npos != pStartPos)
	{
		bHasFind = true;
		std::string::size_type pEndPos = strBefore.find("</OriginDevCode>");
		if (std::string::npos == pEndPos)
		{
			return IVS_FAIL;
		}
		unsigned int uiStartIdex = pStartPos + strlen("<OriginDevCode>");
		unsigned int uiEndIdex = pEndPos + strlen("</OriginDevCode>");
		unsigned int uiSize = strBefore.size();
		// 获取到DevCode
		std::string strOriginDevCode = strBefore.substr(uiStartIdex, (uiEndIdex - uiStartIdex) - strlen("</OriginDevCode>"));
		strAfter.append(strBefore.substr(0,pStartPos)) ;//拷贝到<OriginDevCode>处为止
		std::string strShareInfoSec = strBefore.substr(uiEndIdex ,uiSize - uiEndIdex);
		std::string strCameraCode;
		std::string strDomainCode;
		IVS_INT32 iRet = CToolsHelp::SplitCameraDomainCode(strOriginDevCode, strCameraCode, strDomainCode);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Do Get Url RealPlay", "CToolsHelp::SplitCameraCode failed, RetCode=%d.", iRet);
			return iRet;
		}
		strAfter.append("<OriginDevCode>");
		strAfter.append(strCameraCode);
		strAfter.append("</OriginDevCode>");
		strAfter.append("<OriginDomainCode>");
		strAfter.append(strDomainCode);
		strAfter.append("</OriginDomainCode>");
		strBefore = strShareInfoSec;
		pStartPos = strBefore.find("<OriginDevCode>");
		//如果不存在了
		if (std::string::npos == pStartPos)
		{
			strAfter.append(strShareInfoSec);
		}
	}
	if(!bHasFind)
	{
		strAfter.append(strBefore);
	}
	return IVS_SUCCEED;
}//lint !e1762

//合并设备编码以及域编码
IVS_INT32 CDomainRouteMgr::ComplicationDevDomainCode(std::string& strBefore, std::string& strAfter)
{
	//找到第一个OriginDevCode
	std::string::size_type pStartDevPos = strBefore.find("<OriginDevCode>");
	std::string::size_type pStartDomainPos = strBefore.find("<OriginDomainCode>");
	bool bHasFind = false;
	while(std::string::npos != pStartDevPos && std::string::npos != pStartDomainPos)
	{
		bHasFind = true;
		//查找设备编码
		std::string::size_type pEndDevPos = strBefore.find("</OriginDevCode>");
		if (std::string::npos == pEndDevPos)
		{
			return IVS_FAIL;
		}
		unsigned int uiStartIdex = pStartDevPos + strlen("<OriginDevCode>");
		unsigned int uiEndIdex = pEndDevPos + strlen("</OriginDevCode>");
		unsigned int uiSize = strBefore.size();
		// 获取到DevCode
		std::string strOriginDevCode = strBefore.substr(uiStartIdex, (uiEndIdex - uiStartIdex) - strlen("</OriginDevCode>"));

		//查找域编码
		std::string::size_type pEndDomainPos = strBefore.find("</OriginDomainCode>");
		if (std::string::npos == pEndDomainPos)
		{
			//strAfter.append(strBefore);
			return IVS_SUCCEED;
		}
		//查找域编码不存在的节点
		std::string::size_type pNoDomainPos = strBefore.find("<OriginDomainCode/>");
		if (std::string::npos != pNoDomainPos)
		{
			if (pNoDomainPos < pEndDomainPos)
			{
				unsigned int uiBegin = pNoDomainPos + strlen("<OriginDomainCode/>");
				strAfter.append(strBefore.substr(0,uiBegin));
				strBefore = strBefore.substr(uiBegin,uiSize - uiBegin);
				pStartDevPos = strBefore.find("<OriginDevCode>");
				pStartDomainPos = strBefore.find("<OriginDomainCode>");
				if (std::string::npos == pStartDevPos || std::string::npos == pStartDomainPos)
				{
					strAfter.append(strBefore);
				}
				continue;
			}
		}
		unsigned int uiStartDomainIdex = pStartDomainPos + strlen("<OriginDomainCode>");
		unsigned int uiEndDomainIdex = pEndDomainPos + strlen("</OriginDomainCode>");
		std::string strOriginDomainCode = strBefore.substr(uiStartDomainIdex, (uiEndDomainIdex - uiStartDomainIdex) - strlen("</OriginDomainCode>"));
		strAfter.append(strBefore.substr(0,pStartDevPos)) ;//拷贝到<OriginDevCode>处为止
		//从域编码处开始认为第二个
		std::string strShareInfoSec = strBefore.substr(uiEndDomainIdex ,uiSize - uiEndDomainIdex);
		strAfter.append("<OriginDevCode>");
		strAfter.append(strOriginDevCode);
		//如果域编码不为空才添加域信息
		if (!strOriginDomainCode.empty())
		{
			strAfter.append("#");
			strAfter.append(strOriginDomainCode);
		}
		strAfter.append("</OriginDevCode>");
		strAfter.append("<OriginDomainCode>");
		strAfter.append(strOriginDomainCode);
		strAfter.append("</OriginDomainCode>");
		strBefore = strShareInfoSec;
		pStartDevPos = strBefore.find("<OriginDevCode>");
		pStartDomainPos = strBefore.find("<OriginDomainCode>");
		//如果不存在了
		if (std::string::npos == pStartDevPos || std::string::npos == pStartDomainPos)
		{
			strAfter.append(strShareInfoSec);
		}
	}
	if(!bHasFind)
	{
		strAfter.append(strBefore);
	}
	return IVS_SUCCEED;
}//lint !e1762
