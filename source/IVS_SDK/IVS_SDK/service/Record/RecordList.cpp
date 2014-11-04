#include "RecordList.h"
#include "UserMgr.h"
#include "nss_mt.h"
#include "NSSOperator.h"
#include "IVS_Trace.h"
#include "ToolHelp.h"
#include "RecordQueryInCluster.h"
#define MAX_RECORD_DATA_NUM 25       //最大缓存摄像机数目
#define MAX_PAPER_SUPPORT 8196       //最大支持单次查询的数目
#define SEARCH_RECORD_STEP_NUM 500   //每次查询备份录像数目

CRecordList::CRecordList(void)
{
    m_pUserMgr = NULL;
}

CRecordList::~CRecordList(void)
{
    m_pUserMgr = NULL;
    try
    {
        ClearUpList();
    }
    catch (...)
    {}
}

void CRecordList::SetUserMgr( CUserMgr *pUserMgr)
{
    m_pUserMgr = pUserMgr;
}

void CRecordList::InsertRecordInfoToList(IVS_RECORD_DATA_PLACE* pRecordData)
{
    if (NULL == pRecordData)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Insert Record List", "Record Value is NULL");
        return;
    }

    //删除相同摄像机Code的信息
    RECORD_DATA_PLACE_List_ITER beginIter = m_RecordInfoList.begin();
    RECORD_DATA_PLACE_List_ITER endIter = m_RecordInfoList.end();
    IVS_RECORD_DATA_PLACE* pRecordTmp = NULL;
    for (; beginIter != endIter;)
    {
        pRecordTmp = *beginIter;
        if (NULL == pRecordTmp)
        {
			beginIter++;
            continue;
        }

        //如果存在相同摄像头的信息
        if (0 == strcmp(pRecordData->cCameraCode, pRecordTmp->cCameraCode))
        {
            IVS_DELETE(pRecordTmp, MUILI);
            m_RecordInfoList.erase(beginIter++);
            continue;
        }

        beginIter++;
    }

    //获取当前list的大小
    unsigned int uiListSize = m_RecordInfoList.size();

    //如果已经达到缓存上限，则回收最早的缓存信息
    if (MAX_RECORD_DATA_NUM <= uiListSize)
    {
        pRecordTmp = NULL;
        pRecordTmp = m_RecordInfoList.front();
        IVS_DELETE(pRecordTmp, MUILI);
        m_RecordInfoList.pop_front();
    }

    //将新加入的信息加入List尾部
    m_RecordInfoList.push_back(pRecordData);
}

//回收List资源
void CRecordList::ClearUpList()
{
    //删除相同摄像机Code的信息
    RECORD_DATA_PLACE_List_ITER beginIter = m_RecordInfoList.begin();
    RECORD_DATA_PLACE_List_ITER endIter = m_RecordInfoList.end();

    for (; beginIter != endIter;)
    {
        IVS_DELETE(*beginIter);
        m_RecordInfoList.erase(beginIter++);
    }

    m_RecordInfoList.clear();
}

IVS_INT32 CRecordList::GetRecordNVRInfo(const IVS_CHAR* pCameraCode, const IVS_INT32 iStarTime,
                                        const IVS_INT32 iEndTime,
                                        IVS_CHAR* pNVR)
{
    IVS_DEBUG_TRACE("");
    if ((NULL == pCameraCode) || (NULL == pNVR))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Query NVR Info", "Point is NULL");
        return IVS_PARA_INVALID;
    }

    //删除相同摄像机Code的信息
    RECORD_DATA_PLACE_List_ITER beginIter = m_RecordInfoList.begin();
    RECORD_DATA_PLACE_List_ITER endIter = m_RecordInfoList.end();

    BP_DBG_LOG("m_RecordInfoList size = %d.", m_RecordInfoList.size());
    IVS_RECORD_DATA_PLACE* pRecordTmp = NULL;
    bool bRet = false;
    for (; beginIter != endIter; beginIter++)
    {
        pRecordTmp = *beginIter;
        if (NULL == pRecordTmp)
        {
            continue;
        }

        //插入的时候已经保证只有一个CameraCode相同的数据
        if (0 != strcmp(pCameraCode, pRecordTmp->cCameraCode))
        {
            continue;
        }

        unsigned int uiTotal = pRecordTmp->uiTotal;
        for (unsigned int i = 0; i < uiTotal; i++)
        {
            if ((pRecordTmp->stRecordData[i].iStartTime <= iStarTime)
                && (iEndTime <= pRecordTmp->stRecordData[i].iEndTime))
            {
                bRet = CToolsHelp::Memcpy(pNVR, IVS_NVR_CODE_LEN,
                                          pRecordTmp->stRecordData[i].cNVRCode, IVS_NVR_CODE_LEN);
                BP_DBG_LOG("Get Nvr Code = %s.", pNVR);
                return bRet ? IVS_SUCCEED : IVS_FAIL;
            }
        }
    }

    return IVS_FAIL;
} //lint !e818

// 查询录像列表
IVS_INT32 CRecordList::QueryRecordList(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,
                                       IVS_RECORD_INFO_LIST* pRecordInfoList, IVS_UINT32 uiBufferSize)
{
    CHECK_POINTER(pUnifiedQuery, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pRecordInfoList, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");
    CXml xmlReq;
    IVS_INT32 iRet = IVS_FAIL;

    //摄像机编号去掉域编号处理
    IVS_CHAR chDevCode[IVS_DEV_CODE_LEN + 1] = {0};
    IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
    for (int i = 0; i < pUnifiedQuery->iFieldNum; i++)
    {
        if (QUERY_CAMERA_CODE == pUnifiedQuery->stQueryField[i].eFieID)
        {
            (IVS_VOID) CXmlProcess::ParseDevCode(pUnifiedQuery->stQueryField[i].cValue, chDevCode, chDomaCode);
            break;
        }
    }

    std::string strRecordMethod;
    for (int i = 0; i < pUnifiedQuery->iFieldNum; i++)
    {
        strRecordMethod = pUnifiedQuery->stQueryField[i].cValue;

        //录像方式为备份服务器的录像检索
        if ((QUERY_RECORD_METHOD == pUnifiedQuery->stQueryField[i].eFieID) && ("2" == strRecordMethod))
        {
            iRet = QueryRecordListByBackupServer(pUnifiedQuery, chDevCode, chDomaCode, pRecordInfoList, uiBufferSize);
            break;
        }

        if ((QUERY_RECORD_METHOD == pUnifiedQuery->stQueryField[i].eFieID) && ("2" != strRecordMethod))
        {

            // 如果是容灾录像检索，将xml中的域编码置为登陆域的域编码 add by lilongxin
            if ("3" == strRecordMethod)
            {
                std::string strDomainCode;
				m_pUserMgr->GetDomainCode(strDomainCode);
				//拼装XML查询请求
				iRet = CXmlProcess::GetUnifiedFormatQueryXML(pUnifiedQuery, xmlReq, strDomainCode.c_str());
            }
			else
			{
				//拼装XML查询请求
				iRet = CXmlProcess::GetUnifiedFormatQueryXML(pUnifiedQuery, xmlReq, chDomaCode);
			}
            if (IVS_SUCCEED != iRet)
            {
                BP_RUN_LOG_ERR(iRet, "Get request xml failed", "failed");
                return iRet;
            }
			//RequestSource消息请求来源:0-用户 SDK固定拼个0
            std::string sRequestSource = "RequestSource";
			iRet = CXmlProcess::AddQueryFieldForUnifiedFormatQueryXML(sRequestSource.c_str(), CToolsHelp::Int2Str(0).c_str(), xmlReq);
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "Add QueryField For Unified Format Query XML failed", "failed");
				return iRet;
			}

            unsigned int xmlLen = 0;
            const IVS_CHAR * pQureyReq = xmlReq.GetXMLStream(xmlLen);
            CHECK_POINTER(pQureyReq, IVS_OPERATE_MEMORY_ERROR);
            CSendNssMsgInfo sendNssMsgInfo;
            sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
            sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
            sendNssMsgInfo.SetReqID(NSS_GET_RECORD_LIST_REQ);
            sendNssMsgInfo.SetReqData(pQureyReq);

            // 如果是容灾录像检索，则发送消息到本域 add by lilongxin
            if ("3" != strRecordMethod)
            {
                sendNssMsgInfo.SetCameraCode(chDevCode);
                sendNssMsgInfo.SetDomainCode(chDomaCode);
            }

            iRet = IVS_FAIL;
            std::string strpRsp;
            IVS_INT32 iNeedRedirect = IVS_FAIL;

            // 发送操作失败，直接返回
            iRet = m_pUserMgr->SendCmd(sendNssMsgInfo, strpRsp, iNeedRedirect);
            if (IVS_SUCCEED == iNeedRedirect)
            {
                iRet = QueryInCluster(sendNssMsgInfo,pUnifiedQuery, strpRsp);
            }

            // NSS消息返回码错误，不解析数据，直接返回错误码
            if (IVS_SUCCEED != iRet)
            {
                BP_RUN_LOG_ERR(iRet, "Query Record List", "SendCmd operation succeed,rspCode = %d", iRet);
                return iRet;
            }

            //分配本地保存录像信息内存
            IVS_UINT32 uiTotal = (pUnifiedQuery->stIndex).uiToIndex - (pUnifiedQuery->stIndex).uiFromIndex;
            if (MAX_PAPER_SUPPORT <= uiTotal)
            {
                BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Record Too Much", "Query Record number is %d",
                               uiTotal);
                return IVS_ALLOC_MEMORY_ERROR;
            }

            IVS_UINT32 uiBufSize = sizeof(IVS_RECORD_DATA_PLACE) + sizeof(RECORD_DATA_PLACE) * uiTotal;
            char * pRecordDataPlaceBuf = IVS_NEW(pRecordDataPlaceBuf, uiBufSize);
            if (NULL == pRecordDataPlaceBuf)
            {
                BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Record List is failed", "Alloc Memory Error");
                return IVS_ALLOC_MEMORY_ERROR;
            }

            IVS_RECORD_DATA_PLACE* pRecordDataPlace = (IVS_RECORD_DATA_PLACE*)pRecordDataPlaceBuf; //lint !e826
            //解析数据
            CXml xmlRsp;
            (void)xmlRsp.Parse(strpRsp.c_str());
            IVS_UINT32 uiRecordNum = 0;

            //解析查询请求XML
            if (IVS_SUCCEED
                != CRecordXMLProcess::ParseGetRecordList(xmlRsp, pRecordInfoList,pRecordDataPlace, uiBufferSize, uiRecordNum, 0, 0))
            {
                BP_RUN_LOG_ERR(IVS_XML_INVALID, "Query Record List is failed", "failed");
                return IVS_XML_INVALID;
            }
			InsertRecordInfoToList(pRecordDataPlace);
            break;
        }
    }

    
    return iRet;
}

// 从备份服务器查询录像列表
IVS_INT32 CRecordList::QueryRecordListByBackupServer(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,
                                                     const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode,
                                                     IVS_RECORD_INFO_LIST* pRecordInfoList,
                                                     IVS_UINT32 uiBufferSize)
{
    IVS_DEBUG_TRACE("");
	CHECK_POINTER(pUnifiedQuery, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRecordInfoList, IVS_OPERATE_MEMORY_ERROR);

    unsigned int uiXmlLen = 0;
    IVS_INT32 iRet = IVS_FAIL;

	std::string strNVRCode = "NVRCode";
	std::string strRequestSource = "RequestSource";
	std::string strCameraDomain = "CameraDomain";

	/**************************************
	**第一次：查询记录的总条数
	***************************************/
	IVS_UINT32 uiFromIndex = pUnifiedQuery->stIndex.uiFromIndex;
	IVS_UINT32 uiToIndex   = pUnifiedQuery->stIndex.uiToIndex;

	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_RECORD_LIST_REQ);

	//拼装XML查询请求
	CXml recordXmlReq;
	iRet = CXmlProcess::GetUnifiedFormatQueryXML(pUnifiedQuery, recordXmlReq, CToolsHelp::Int2Str(0).c_str());
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get request xml failed", "failed");
		return iRet;
	}
	//增加CamerDomain节点，备份录像检索必须带这个节点
	CXmlProcess::AddQueryFieldForUnifiedFormatQueryXML(strCameraDomain.c_str(), pDomainCode, recordXmlReq);

	//获取登录域编码
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	std::string strLocalDomainCode;
	m_pUserMgr->GetDomainCode(strLocalDomainCode);

	iRet = CXmlProcess::AddQueryFieldForUnifiedFormatQueryXML(strNVRCode.c_str(), strLocalDomainCode.c_str(), recordXmlReq);
	//RequestSource消息请求来源:0-用户 SDK固定拼个0
	iRet += CXmlProcess::AddQueryFieldForUnifiedFormatQueryXML(strRequestSource.c_str(), CToolsHelp::Int2Str(0).c_str(), recordXmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add QueryField For Unified Format Query XML failed", "failed");
		return iRet;
	}

	//修改请求的分页信息字段	
	if (recordXmlReq.FindElemEx("Content/PageInfo"))
	{
		recordXmlReq.IntoElem();
		if (recordXmlReq.FindElem("FromIndex"))
		{
			recordXmlReq.ModifyElemValue(CToolsHelp::Int2Str((int)uiFromIndex).c_str());
		}
		if (recordXmlReq.FindElem("ToIndex"))
		{
			recordXmlReq.ModifyElemValue(CToolsHelp::Int2Str((int)uiToIndex).c_str());
		}
		recordXmlReq.OutOfElem();
	}
	const IVS_CHAR * pXml = recordXmlReq.GetXMLStream(uiXmlLen);
	if (NULL == pXml)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "pRecordNum", "failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	sendNssMsgInfo.SetReqData(pXml);
	//向登录域发送录像备份检索
	sendNssMsgInfo.SetDomainCode(strLocalDomainCode);

	//录像记录总条数
	int iRecordRealNum = 0;

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo, strpRsp, iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Query Record List By Backup Server:Query Record",
			           "SendCmd operation fail,rspCode = %d", iRet);
		return iRet;
	}
	else
	{
		CXml xmlRsp;
		if(xmlRsp.Parse(strpRsp.c_str()))
		{
			//获取记录条数
			xmlRsp.FindElemEx("Content/PageInfo/RealNum");
			iRecordRealNum = CToolsHelp::StrToInt(xmlRsp.GetElemValue());
			pRecordInfoList->uiTotal = (unsigned int)iRecordRealNum;

			//如果记录为0,直接返回
			if (0 == iRecordRealNum)
			{
				BP_DBG_LOG("Query Record List By Backup Server: RecordNum[%d]", iRecordRealNum);
				return IVS_SUCCEED;
			}
		}
	}

	//查询1-500，如果返回的RealNum大于500则继续查询，直到小于500
	int iFromIndex = 1;
	int iToIndex = SEARCH_RECORD_STEP_NUM;	

	//每次500条,需要读取的次数
	unsigned int uiReadCount = pRecordInfoList->uiTotal / SEARCH_RECORD_STEP_NUM; 

	unsigned int uiRecordNum = 0;
	int iRemainNum = 0;
	unsigned int uiCount = 0;

	do{
		//修改请求的分页信息字段	
		if (recordXmlReq.FindElemEx("Content/PageInfo"))
		{
			recordXmlReq.IntoElem();
			if (recordXmlReq.FindElem("FromIndex"))
			{
				recordXmlReq.ModifyElemValue(CToolsHelp::Int2Str(iFromIndex).c_str());
			}
			if (recordXmlReq.FindElem("ToIndex"))
			{
				recordXmlReq.ModifyElemValue(CToolsHelp::Int2Str(iToIndex).c_str());
			}
			recordXmlReq.OutOfElem();
		}

		const IVS_CHAR * pRecordXml = recordXmlReq.GetXMLStream(uiXmlLen);
		if (NULL == pRecordXml)
		{
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "pRecordNum", "failed");
			return IVS_OPERATE_MEMORY_ERROR;
		}

		sendNssMsgInfo.SetReqData(pRecordXml);
		//向登录域发送录像备份检索
		sendNssMsgInfo.SetDomainCode(strLocalDomainCode);

		//iRet = IVS_FAIL;
		std::string strRsp;
		//IVS_INT32 iNeedRedirect = IVS_FAIL;
		iRet = m_pUserMgr->SendCmd(sendNssMsgInfo, strRsp, iNeedRedirect);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Query Record List By Backup Server:Query Record",
				"SendCmd operation fail,rspCode = %d", iRet);
			return iRet;
		}
		else
		{
			CXml xmlRsp;
			if(xmlRsp.Parse(strRsp.c_str()))
			{
				//修改MBUDomain
				xmlRsp.GetRootPos();
				(void)xmlRsp.FindElem("Content");	
				(void)xmlRsp.IntoElem();
				(void)xmlRsp.FindElem("RecordDataList");
				(void)xmlRsp.IntoElem();
				(void)xmlRsp.FindElem("RecordDataInfo");
				do 
				{
					(void)xmlRsp.IntoElem();

					if (!xmlRsp.FindElem("MBUDomain"))
					{
						BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlRsp.FindElem(\"MBUDomain\")", "NA");
						return IVS_XML_INVALID;
					}

					xmlRsp.ModifyElemValue(strLocalDomainCode.c_str());
					xmlRsp.OutOfElem();
				} while (xmlRsp.NextElem());

				//查看是否修改正确
				const char* pTmp = xmlRsp.GetXMLStream(uiXmlLen);
				if (NULL != pTmp)
				{
					BP_DBG_LOG("Query Record List By Backup Server: RspXml:[%s]", pTmp);
				}
	
				//解析XML,将数据导入到RecordInfoList中
				uiCount++;

				if (IVS_SUCCEED != CRecordXMLProcess::ParseGetRecordList(xmlRsp, pRecordInfoList, NULL, 
																		 uiBufferSize, uiRecordNum, 
																		(IVS_UINT32)iFromIndex, 
																		(IVS_UINT32)iToIndex))
				{
					BP_RUN_LOG_ERR(IVS_XML_INVALID, "Query Record List is failed", "failed");
					break;
				}

				if (uiCount <= uiReadCount)
				{
					iFromIndex += SEARCH_RECORD_STEP_NUM;
					iToIndex += SEARCH_RECORD_STEP_NUM;
					if (iToIndex >= iRecordRealNum)
					{
						iToIndex = iRecordRealNum;
					}
				}

				// 剩余录像条数
				iRemainNum = pRecordInfoList->uiTotal - uiCount * SEARCH_RECORD_STEP_NUM;  //lint !e713
			}
		}
	}while(iRemainNum > 0);

	pRecordInfoList->stIndexRange.uiFromIndex = 1;
	pRecordInfoList->stIndexRange.uiToIndex = uiRecordNum;

	return IVS_SUCCEED;
}

// 根据每个MBU检索的结果数和分页请求索引号计算出目标的每个MBU轨迹的结果索引
IVS_INT32 CRecordList::ComputeIndexOfRecordList(const IVS_UINT32 uiFromIndex, const IVS_UINT32 uiToIndex,
                                                IVS_MBU_CHANGE_INFO_LIST* pMBUInfoList)const
{
    CHECK_POINTER(pMBUInfoList, IVS_OPERATE_MEMORY_ERROR);
    if (uiFromIndex > uiToIndex)
    {
        return IVS_PARA_INVALID;
    }

    IVS_UINT32 uiTotalNum = 0;
    IVS_UINT32 uiIndexNum = 0;
    for (unsigned int i = 0; i < pMBUInfoList->uiTotal; i++)
    {
        uiTotalNum += pMBUInfoList->stMBUChangeInfo[i].uiRecordNum;
    }

    if ((uiTotalNum < uiToIndex) && (uiFromIndex > 1))
    {
        return IVS_PARA_INVALID;
    }

    if ((uiTotalNum <= uiToIndex) && (uiFromIndex == 1))
    {
        for (unsigned int i = 0; i < pMBUInfoList->uiTotal; i++)
        {
            pMBUInfoList->stMBUChangeInfo[i].uiFromIndex = 1;
            pMBUInfoList->stMBUChangeInfo[i].uiToIndex = pMBUInfoList->stMBUChangeInfo[i].uiRecordNum;
        }
    }

    if ((uiTotalNum > uiToIndex) && (uiFromIndex == 1))
    {
        for (unsigned int i = 0; i < pMBUInfoList->uiTotal; i++)
        {
            uiIndexNum += pMBUInfoList->stMBUChangeInfo[i].uiRecordNum;
            if (uiIndexNum <= uiToIndex)
            {
                pMBUInfoList->stMBUChangeInfo[i].uiFromIndex = 1;
                pMBUInfoList->stMBUChangeInfo[i].uiToIndex = pMBUInfoList->stMBUChangeInfo[i].uiRecordNum;
                if (uiIndexNum == uiToIndex)
                {
                    break;
                }
            }

            if (uiIndexNum > uiToIndex)
            {
                pMBUInfoList->stMBUChangeInfo[i].uiFromIndex = 1;
                pMBUInfoList->stMBUChangeInfo[i].uiToIndex = pMBUInfoList->stMBUChangeInfo[i].uiRecordNum
                                                             - (uiIndexNum - uiToIndex);
                break;
            }
        }
    }

    if ((uiTotalNum == uiToIndex) && (uiFromIndex > 1))
    {
        IVS_UINT32 uiTempNum = 0;
        for (unsigned int i = 0; i < pMBUInfoList->uiTotal; i++)
        {
            uiTempNum   = pMBUInfoList->stMBUChangeInfo[i].uiRecordNum;
            uiIndexNum += uiTempNum;
            if (uiIndexNum < uiFromIndex)
            {
                pMBUInfoList->stMBUChangeInfo[i].uiFromIndex = 0;
                pMBUInfoList->stMBUChangeInfo[i].uiToIndex = 0;
            }
            else
            {
                if (uiIndexNum - uiTempNum >= uiFromIndex)
                {
                    pMBUInfoList->stMBUChangeInfo[i].uiFromIndex = 1;
                    pMBUInfoList->stMBUChangeInfo[i].uiToIndex = uiTempNum;
                }
                else
                {
                    pMBUInfoList->stMBUChangeInfo[i].uiFromIndex = uiTempNum - (uiIndexNum - uiFromIndex);
                    pMBUInfoList->stMBUChangeInfo[i].uiToIndex = uiTempNum;
                }
            }

            if (uiIndexNum == uiTotalNum)
            {
                break;
            }
        }
    }

    if ((uiTotalNum > uiToIndex) && (uiFromIndex > 1))
    {
        IVS_UINT32 uiTempNum = 0;
        for (unsigned int i = 0; i < pMBUInfoList->uiTotal; i++)
        {
            uiTempNum   = pMBUInfoList->stMBUChangeInfo[i].uiRecordNum;
            uiIndexNum += uiTempNum;
            if ((uiIndexNum < uiFromIndex) || (uiIndexNum - uiTempNum > uiToIndex))
            {
                pMBUInfoList->stMBUChangeInfo[i].uiFromIndex = 0;
                pMBUInfoList->stMBUChangeInfo[i].uiToIndex = 0;
            }
            else
            {
                if ((uiIndexNum - uiTempNum >= uiFromIndex) && (uiIndexNum <= uiToIndex))
                {
                    pMBUInfoList->stMBUChangeInfo[i].uiFromIndex = 1;
                    pMBUInfoList->stMBUChangeInfo[i].uiToIndex = uiTempNum;
                }
                else if ((uiIndexNum - uiTempNum < uiFromIndex) && (uiIndexNum >= uiFromIndex))
                {
                    pMBUInfoList->stMBUChangeInfo[i].uiFromIndex = uiTempNum - (uiIndexNum - uiFromIndex);
                    pMBUInfoList->stMBUChangeInfo[i].uiToIndex = uiTempNum;
                }
                else
                {
                    pMBUInfoList->stMBUChangeInfo[i].uiFromIndex = 1;
                    pMBUInfoList->stMBUChangeInfo[i].uiToIndex = uiTempNum - (uiIndexNum - uiToIndex);
                }
            }
        }
    }

    return IVS_SUCCEED;
}

// 从备份服务器查询录像列表
IVS_INT32 CRecordList::QueryMBUChangeHistoryList(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,
                                                 const IVS_CHAR* pCameraCode,
                                                 const IVS_CHAR* pDomainCode,
                                                 IVS_MBU_CHANGE_INFO_LIST* pMBUInfoList,
												 IVS_INT32 iToIndex,
												 IVS_UINT32* uiDiffTotalNum)
{
	CHECK_POINTER(pUnifiedQuery, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pMBUInfoList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(uiDiffTotalNum, IVS_OPERATE_MEMORY_ERROR);

    CXml MBUXmlReq;

    (void)MBUXmlReq.AddDeclaration("1.0", "UTF-8", "");
    (void)MBUXmlReq.AddElem("Content");
    (void)MBUXmlReq.AddChildElem("DomainCode");
    (void)MBUXmlReq.IntoElem();
    (void)MBUXmlReq.SetElemValue(pDomainCode);
    (void)MBUXmlReq.AddElem("CameraCode");
    (void)MBUXmlReq.SetElemValue(pCameraCode);
    for (int i = 0; i < pUnifiedQuery->iFieldNum; i++)
    {
        if (QUERY_OPERATION_START_TIME == pUnifiedQuery->stQueryField[i].eFieID)
        {
            (void)MBUXmlReq.AddElem("StartTime");
            (void)MBUXmlReq.SetElemValue(pUnifiedQuery->stQueryField[i].cValue);
            break;
        }
    }

    (void)MBUXmlReq.AddElem("PageInfo");
    (void)MBUXmlReq.AddChildElem("FromIndex");
    (void)MBUXmlReq.IntoElem();
    (void)MBUXmlReq.SetElemValue(CToolsHelp::Int2Str(1).c_str());
    (void)MBUXmlReq.AddElem("ToIndex");
    (void)MBUXmlReq.SetElemValue(CToolsHelp::Int2Str(iToIndex).c_str());
    MBUXmlReq.OutOfElem();
    MBUXmlReq.OutOfElem();

    unsigned int xmlLen = 0;
    const IVS_CHAR * pMBUReqPlan = MBUXmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pMBUReqPlan, IVS_OPERATE_MEMORY_ERROR);
    CSendNssMsgInfo sendMBUNssMsgInfo;
    sendMBUNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendMBUNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendMBUNssMsgInfo.SetReqID(NSS_GET_MBU_CHANGE_HISTORY_REQ);
    sendMBUNssMsgInfo.SetReqData(pMBUReqPlan);
    sendMBUNssMsgInfo.SetCameraCode(pCameraCode);

	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	std::string strLocalDomainCode;
	m_pUserMgr->GetDomainCode(strLocalDomainCode);
    sendMBUNssMsgInfo.SetDomainCode(strLocalDomainCode.c_str());

    std::string strMBUInfoRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;

    // 发送操作失败，直接返回
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_INT32 iRet = m_pUserMgr->SendCmd(sendMBUNssMsgInfo, strMBUInfoRsp, iNeedRedirect);
    if (IVS_SUCCEED == iNeedRedirect)
    {
        iRet = m_pUserMgr->SendRedirectServe(sendMBUNssMsgInfo, strMBUInfoRsp);
    }

    // NSS消息返回码错误，不解析数据，直接返回错误码
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Query MBU Change History List", "SendCmd operation succeed,rspCode = %d", iRet);
        return iRet;
    }

    //解析数据
    CXml MBUXmlRsp;
    (void)MBUXmlRsp.Parse(strMBUInfoRsp.c_str());

    //解析查询请求XML
    if (IVS_SUCCEED != ParseGetMBUChangeHistoryList(MBUXmlRsp, pMBUInfoList,uiDiffTotalNum))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Query MBU Change History List",
                       "Query Record List is failed =%d", iRet);
        return IVS_XML_INVALID;
    }

    return IVS_SUCCEED;
}

// 获取录像列表
IVS_INT32 CRecordList::GetRecordList(const IVS_CHAR* pCameraCode, IVS_INT32 iRecordMethod,
                                     const IVS_TIME_SPAN* pTimeSpan,
                                     const IVS_INDEX_RANGE* pIndexRange, IVS_RECORD_INFO_LIST* pRecordList,
                                     IVS_UINT32 uiBufSize)                                    //lint !e818 //不需要
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pTimeSpan, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pIndexRange, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRecordList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN + 1];
	memset(chDevCode, 0, IVS_DEV_CODE_LEN + 1);
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN + 1];
	memset(chDomaCode, 0, IVS_DOMAIN_CODE_LEN + 1);
	(IVS_VOID) CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	IVS_INT32 iRet = IVS_FAIL;
	if(2 == iRecordMethod)
	{
		IVS_VOID *pReqBuffer = NULL;
		pReqBuffer = IVS_NEW((IVS_CHAR* &)pReqBuffer, sizeof(IVS_QUERY_UNIFIED_FORMAT) + 7*sizeof(IVS_QUERY_FIELD));
		if (NULL == pReqBuffer)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Allocate Query Struct Memory", "new memory failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		memset(pReqBuffer, 0, sizeof(IVS_QUERY_UNIFIED_FORMAT) + 7*sizeof(IVS_QUERY_FIELD));
		IVS_QUERY_UNIFIED_FORMAT *pUnifiedQuery = (IVS_QUERY_UNIFIED_FORMAT*)pReqBuffer;
		pUnifiedQuery->iFieldNum = 8;
		pUnifiedQuery->stIndex.uiFromIndex = pIndexRange->uiFromIndex;
		pUnifiedQuery->stIndex.uiToIndex = pIndexRange->uiToIndex;
		pUnifiedQuery->stOrderCond.bEnableOrder = FALSE;
		pUnifiedQuery->stQueryField[0].bExactQuery = 1;
		pUnifiedQuery->stQueryField[0].eFieID = QUERY_CAMERA_CODE;
		if (!CToolsHelp::Memcpy(pUnifiedQuery->stQueryField[0].cValue, strlen(pCameraCode),pCameraCode, strlen(pCameraCode)))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Record List By Backup Server", "Memcpy pCameraCode failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		pUnifiedQuery->stQueryField[1].bExactQuery = 1; //lint !e415
		pUnifiedQuery->stQueryField[1].eFieID = QUERY_RECORD_QUERY_TYPE;//lint !e415
		std::string strQueryType = "0";
		if (!CToolsHelp::Memcpy(pUnifiedQuery->stQueryField[1].cValue, 1,strQueryType.c_str(), 1))//lint !e416
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Record List By Backup Server", "Memcpy strQueryType failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		pUnifiedQuery->stQueryField[2].bExactQuery = 1;//lint !e415 !e416
		pUnifiedQuery->stQueryField[2].eFieID = QUERY_RECORD_METHOD;//lint !e415 !e416
		std::string strRecordMethod = CToolsHelp::Int2Str(iRecordMethod);
		if (!CToolsHelp::Memcpy(pUnifiedQuery->stQueryField[2].cValue, 1,strRecordMethod.c_str(), 1))//lint !e416
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,  "Get Record List By Backup Server", "Memcpy strRecordMethod failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		pUnifiedQuery->stQueryField[3].bExactQuery = 1;//lint !e415 !e416
		pUnifiedQuery->stQueryField[3].eFieID = QUERY_RECORD_TYPE;//lint !e415 !e416
		std::string strQueryRecordType = "010";
		if (!CToolsHelp::Memcpy(pUnifiedQuery->stQueryField[3].cValue, 3,strQueryRecordType.c_str(), 3))//lint !e416
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,  "Get Record List By Backup Server", "Memcpy strQueryRecordType failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		pUnifiedQuery->stQueryField[4].bExactQuery = 1;//lint !e415 !e416
		pUnifiedQuery->stQueryField[4].eFieID = QUERY_OPERATION_START_TIME;//lint !e415 !e416
		if (!CToolsHelp::Memcpy(pUnifiedQuery->stQueryField[4].cValue, IVS_TIME_LEN,pTimeSpan->cStart, IVS_TIME_LEN))//lint !e416
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,  "Get Record List By Backup Server", "Memcpy pTimeSpan->cStart failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		pUnifiedQuery->stQueryField[5].bExactQuery = 1;//lint !e415 !e416
		pUnifiedQuery->stQueryField[5].eFieID = QUERY_OPERATION_END_TIME;//lint !e415 !e416
		if (!CToolsHelp::Memcpy(pUnifiedQuery->stQueryField[5].cValue, IVS_TIME_LEN,pTimeSpan->cEnd, IVS_TIME_LEN))//lint !e416
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,  "Get Record List By Backup Server", "Memcpy pTimeSpan->cEnd failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		pUnifiedQuery->stQueryField[6].bExactQuery = 1;//lint !e415 !e416
		pUnifiedQuery->stQueryField[6].eFieID = QUERY_OPERATOR_ID;//lint !e415 !e416
		std::string strOperatorID = CToolsHelp::Int2Str(static_cast<IVS_INT32>(m_pUserMgr->GetUserID()));
		if (!CToolsHelp::Memcpy(pUnifiedQuery->stQueryField[6].cValue, 7,strOperatorID.c_str(), 7))//lint !e416
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,  "Get Record List By Backup Server", "Memcpy strOperatorID failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		pUnifiedQuery->stQueryField[7].bExactQuery = 1;//lint !e415 !e416
		pUnifiedQuery->stQueryField[7].eFieID = QUERY_RECORD_USER_DOMAIN;//lint !e415 !e416
		std::string strOperatorDomain = "";
		m_pUserMgr->GetDomainCode(strOperatorDomain);
		if (!CToolsHelp::Memcpy(pUnifiedQuery->stQueryField[7].cValue, IVS_DOMAIN_CODE_LEN,strOperatorDomain.c_str(), IVS_DOMAIN_CODE_LEN))//lint !e416
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,  "Get Record List By Backup Server", "Memcpy strOperatorDomain failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}

		iRet = this->QueryRecordListByBackupServer(pUnifiedQuery, chDevCode, chDomaCode, pRecordList, uiBufSize);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Query RecordList By Backup Server is failed", "failed");
			return iRet;
		}
		return iRet;
	}

	CXml xmlReq;
    //拼装XML查询请求
    iRet = CRecordXMLProcess::GetRecordListReqXML(chDevCode, chDomaCode,
                                                  iRecordMethod, pTimeSpan, pIndexRange, xmlReq);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Record List Req XML is failed", "failed");
        return iRet;
    }
	IVS_UINT32 uiOperatorID = m_pUserMgr->GetUserID();
	iRet = CXmlProcess::AddQueryFieldForUnifiedFormatQueryXML("OperatorID", CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiOperatorID)).c_str(), xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add QueryField For Unified Format Query XML failed", "failed");
		return iRet;
	}

    unsigned int xmlLen = 0;
    const IVS_CHAR * pReqPlan = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_GET_RECORD_LIST_REQ);
    sendNssMsgInfo.SetReqData(pReqPlan);

    //摄像机编号去掉域编号处理
    (IVS_VOID) CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
    sendNssMsgInfo.SetCameraCode(chDevCode);
    sendNssMsgInfo.SetDomainCode(chDomaCode);

    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;

    // 发送操作失败，直接返回
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo, strpRsp, iNeedRedirect);
    if (IVS_SUCCEED == iNeedRedirect)
    {
        iRet = m_pUserMgr->SendRedirectServe(sendNssMsgInfo, strpRsp);
    }

    // NSS消息返回码错误，不解析数据，直接返回错误码
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Record List", "SendCmd operation succeed,rspCode = %d", iRet);
        return iRet;
    }

    //解析数据
    CXml xmlRsp;
    (void)xmlRsp.Parse(strpRsp.c_str());
    IVS_UINT32 uiRecordNum = 0;

    //解析查询请求XML
    if (IVS_SUCCEED != CRecordXMLProcess::ParseGetRecordList(xmlRsp, pRecordList,NULL, uiBufSize, uiRecordNum, 0, 0))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Get Record List is failed", "failed");
        return IVS_XML_INVALID;
    }

    return iRet;
}

// 查询录像状态
IVS_INT32 CRecordList::GetRecordStatus(const IVS_CHAR* pCameraCode, IVS_UINT32 uiRecordMethod,
                                       IVS_UINT32* pRecordStatus) const                                                      //lint !e818 返回的不需要
{
    CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    CXml xmlReq;

    //拼装XML查询请求
    IVS_INT32 iRet = IVS_FAIL;
    iRet = CRecordXMLProcess::GetRecordStatusReqXML(pCameraCode,
                                                    uiRecordMethod, xmlReq);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Record Status:GetRecordStatusReqXML is failed", "failed");
        return iRet;
    }

    unsigned int xmlLen = 0;
    const IVS_CHAR * pRecordStatusReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pRecordStatusReq, IVS_OPERATE_MEMORY_ERROR);

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	
	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_RECORD_STATE_REQ);
	sendNssMsgInfo.SetReqData(pRecordStatusReq); 
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chDomaCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SMU_LINKID_NOT_EXIST == iRet)
	{
	    BP_RUN_LOG_ERR(IVS_SMU_LINKID_NOT_EXIST, "Get Record Status:This Camera is not Registered Or Existed", "failed");
	    return IVS_SMU_LINKID_NOT_EXIST;
	}
	if (IVS_SUCCEED != iRet)
	{
	    BP_RUN_LOG_ERR(iRet, "Get Record Status Failed", "failed");
	    return iRet;
	}

    CXml xmlRsp;
    xmlRsp.Parse(strpRsp.c_str());

    //解析查询请求XML
    if (IVS_SUCCEED != CRecordXMLProcess::ParseRecordStatus(xmlRsp, pRecordStatus))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Record Status:Parse Record Status is failed", "failed");
        return IVS_XML_INVALID;
    }
    return iRet;
}

//lint !e818

//集群下的录像检索
int CRecordList::QueryInCluster(CSendNssMsgInfo& sendNssMsgInfo,const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,std::string& strpRsp)
{
    //根据域编码查找目标域和代理域映射表，查找代理域
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	//CHECK_POINTER(pIndex, IVS_OPERATE_MEMORY_ERROR);
   // CHECK_POINTER(pTimeSpan, IVS_OPERATE_MEMORY_ERROR);
    std::string strSendDomainCode;
    std::string strDomainCode = sendNssMsgInfo.GetDomainCode();
    bool bRet = m_pUserMgr->GetDomainRouteMgr().FindProxyDomainCode(strDomainCode, strSendDomainCode);

    // 如果未找到代理域，使用目标域代替;
    if (!bRet)
    {
        strSendDomainCode = strDomainCode;
    }

    IVS_INT32 iRet = IVS_FAIL;
    IVS_DOMAIN_ROUTE stDomainRoute;
    memset(&stDomainRoute, 0, sizeof(IVS_DOMAIN_ROUTE));

    //获取域路由信息
    iRet = m_pUserMgr->GetDomainRouteMgr().GetDomainRoutebyCode(strSendDomainCode, stDomainRoute);
    if (IVS_SUCCEED != iRet)
    {
        return iRet;
    }

    IVS_DOMAIN_TYPE enDomainType = static_cast <IVS_DOMAIN_TYPE>(stDomainRoute.uiDomainType);

    //非集群模式
    if (DOMAIN_TYPE_CLUSTER != enDomainType)
    {
        iRet = m_pUserMgr->SendRedirectServe(sendNssMsgInfo, strpRsp);
    }
    //集群模式
    else
    {
        CRecordQueryInCluster cQueryInCluster;
        cQueryInCluster.SetUserMgr(m_pUserMgr);
        //集群模式下的录像检索
        iRet = cQueryInCluster.GetRecordInCluster(sendNssMsgInfo, pUnifiedQuery, strpRsp);
    }

    return iRet;
}

// 锁定录像(平台/备份服务器)
IVS_INT32 CRecordList::LockRecord(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode, const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,
                                  const IVS_RECORD_LOCK_INFO* pRecordLockInfo) const
{
    CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pRecordLockInfo, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	//参数校验，此处注意不能对pNVRCode验空，因为平台锁定录像的SDK接口参数无pNVRCode，传的为NULL
    IVS_DEBUG_TRACE("");

    CXml xmlReq;

    //拼装XML查询请求
    IVS_INT32 iRet = IVS_FAIL;
    iRet = CRecordXMLProcess::LockRecordReqXML(iRecordMethod,pDomainCode,pNVRCode, pCameraCode, pRecordLockInfo, xmlReq);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Lock Record:Lock Record Req XML is failed", "failed");
        return iRet;
    }

    IVS_UINT32 xmlLen = 0;
    const IVS_CHAR* pGetXml = xmlReq.GetXMLStream(xmlLen);
    if (NULL == pGetXml)
    {
        return IVS_XML_INVALID;
    }

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
    (IVS_VOID) CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_ADD_RECORD_LOCK_REQ);
    sendNssMsgInfo.SetReqData(pGetXml);
    sendNssMsgInfo.SetCameraCode(chDevCode);
	if(RECORD_METHOD_MBU == iRecordMethod)
	{
		sendNssMsgInfo.SetDomainCode(pDomainCode);
	}
	else
	{
		sendNssMsgInfo.SetDomainCode(chDomaCode);
	}

    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo, strpRsp, iNeedRedirect);
    return iRet;
}

// 解锁录像(平台/备份服务器)
IVS_INT32 CRecordList::UnLockRecord(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode, const IVS_CHAR* pCameraCode,
                                    const IVS_RECORD_LOCK_INFO* pRecordLockInfo) const
{
	//参数校验，此处注意不能对pNVRCode验空，因为平台解锁录像的SDK接口参数无pNVRCode，传的为NULL
    CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pRecordLockInfo, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    CXml xmlReq;

    //拼装XML查询请求
    IVS_INT32 iRet = IVS_FAIL;
    iRet = CRecordXMLProcess::UnLockRecordReqXML(iRecordMethod,pDomainCode,pNVRCode, pCameraCode, pRecordLockInfo, xmlReq);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "UnLock Record:UnLock Record Req XML is failed", "failed");
        return iRet;
    }

    IVS_UINT32 xmlLen = 0;
    const IVS_CHAR* pGetXml = xmlReq.GetXMLStream(xmlLen);
    if (NULL == pGetXml)
    {
        return IVS_XML_INVALID;
    }


    IVS_CHAR chDevCode[IVS_DEV_CODE_LEN + 1];
    IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN + 1];
    (IVS_VOID) CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_DEL_RECORD_LOCK_REQ);
    sendNssMsgInfo.SetReqData(pGetXml);
    sendNssMsgInfo.SetCameraCode(chDevCode);
	if(RECORD_METHOD_MBU == iRecordMethod)
	{
		sendNssMsgInfo.SetDomainCode(pDomainCode);
	}
	else
	{
		sendNssMsgInfo.SetDomainCode(chDomaCode);
	}
	IVS_INT32 iNeedRedirect = IVS_FAIL;
    std::string strpRsp;
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo, strpRsp, iNeedRedirect);
    return iRet;
}

// 修改录像锁定信息
IVS_INT32 CRecordList::ModLockRecord(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode, const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,
	const IVS_RECORD_LOCK_INFO* pRecordLockInfo) const
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRecordLockInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;

	//拼装XML查询请求
	IVS_INT32 iRet = IVS_FAIL;
	iRet = CRecordXMLProcess::ModLockRecordReqXML(iRecordMethod,pDomainCode,pNVRCode, pCameraCode, pRecordLockInfo, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Lock Record:Mod Lock Record Req XML is failed", "failed");
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pGetXml = xmlReq.GetXMLStream(xmlLen);

    if (NULL == pGetXml)
    {
        return IVS_XML_INVALID;
    }

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	(IVS_VOID) CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_MOD_RECORD_LOCK_REQ);
	sendNssMsgInfo.SetReqData(pGetXml);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo, strpRsp, iNeedRedirect);
	return iRet;
}

//MBU服务器变更记录的响应解析XML
IVS_INT32 CRecordList::ParseGetMBUChangeHistoryList(CXml& xml,IVS_MBU_CHANGE_INFO_LIST* pMBUChangeInfoList,IVS_UINT32* uiDiffTotalNum)
{
	CHECK_POINTER(pMBUChangeInfoList, IVS_OPERATE_MEMORY_ERROR); 
	IVS_DEBUG_TRACE("");
	int iRet = IVS_FAIL;

	if (!xml.FindElemEx("Content"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "xml.FindElem(Content) is fail");
		return iRet;
	}
	if (!xml.FindElemEx("Content/PageInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Result XML", "xml.FindElem(Content/PageInfo) is fail");
		return IVS_XML_INVALID;
	}   

	const char* pTemp = NULL;
	GET_ELEM_VALUE_NUM("FromIndex", pTemp, pMBUChangeInfoList->stIndexRange.uiFromIndex, xml);//lint !e732
	GET_ELEM_VALUE_NUM("ToIndex", pTemp, pMBUChangeInfoList->stIndexRange.uiToIndex, xml);//lint !e732
	GET_ELEM_VALUE_NUM("RealNum",pTemp, pMBUChangeInfoList->uiTotal, xml);//lint !e734 !e732
	if (0 == pMBUChangeInfoList->uiTotal)
	{
		return IVS_SUCCEED;
	}

	IVS_INT32 iRealNum = 0;
	const char* szElemValue = NULL;
	m_MBUChangeInfoMap.clear();
	if (xml.FindElemEx("Content/MBUChangeList/MBUChangeInfo"))
	{
		do
		{
			IVS_MBU_CHANGE_INFO sMBUChangeInfo;
			memset(&sMBUChangeInfo, 0, sizeof(IVS_MBU_CHANGE_INFO));
			bool bRet = false;
			(void)xml.IntoElem();
			char cMBUCode[IVS_DOMAIN_CODE_LEN+1]={0};
			memset(cMBUCode,0x0,IVS_DOMAIN_CODE_LEN+1);
			char cMBUDomainCode[IVS_DOMAIN_CODE_LEN+1]={0};
			memset(cMBUDomainCode,0x0,IVS_DOMAIN_CODE_LEN+1);
			GET_ELEM_VALUE_CHAR("MBUCode",szElemValue,  cMBUCode,IVS_DOMAIN_CODE_LEN,xml);
			GET_ELEM_VALUE_CHAR("MBUDomainCode",szElemValue,  cMBUDomainCode,IVS_DOMAIN_CODE_LEN,xml);

			bRet = CToolsHelp::Memcpy(sMBUChangeInfo.cMBUCode,IVS_DOMAIN_CODE_LEN, cMBUCode, IVS_DOMAIN_CODE_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Manual Record State XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			bRet = CToolsHelp::Memcpy(sMBUChangeInfo.cMBUDomainCode,IVS_DOMAIN_CODE_LEN, cMBUDomainCode, IVS_DOMAIN_CODE_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Manual Record State XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			xml.OutOfElem();

			MBU_CHANGE_INFO_LIST_MAP_ITER mapIter;
			MBU_CHANGE_INFO_LIST_MAP_ITER mapIterEnd = m_MBUChangeInfoMap.end();
			mapIter = m_MBUChangeInfoMap.find(sMBUChangeInfo.cMBUCode);
			if (mapIter != mapIterEnd)
			{
				//说明Map里面已近存在了一个相同key值的MBUCode,即pMBUChangeInfoList中已经保存过此MBU的编码信息，此时不做任何操作。
			}
			else
			{
				std::string strMBUCode = sMBUChangeInfo.cMBUCode;
				std::string strMBUDomainCode = sMBUChangeInfo.cMBUDomainCode;
				//更新Map表
				(IVS_VOID)m_MBUChangeInfoMap.insert(std::make_pair(strMBUCode, strMBUDomainCode));

				bRet = CToolsHelp::Memcpy(&(pMBUChangeInfoList->stMBUChangeInfo[iRealNum]), sizeof(IVS_MBU_CHANGE_INFO), &sMBUChangeInfo, sizeof(IVS_MBU_CHANGE_INFO));
				if(false == bRet)
				{
					BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Result XML", "Memcpy error.");
					return IVS_ALLOC_MEMORY_ERROR;
				}
				iRealNum++;
			}
		}while(xml.NextElem());
	}
	*uiDiffTotalNum = static_cast<IVS_UINT32>(iRealNum);

	return IVS_SUCCEED;
}

