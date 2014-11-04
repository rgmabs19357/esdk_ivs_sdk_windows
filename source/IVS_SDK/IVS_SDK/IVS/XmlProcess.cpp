#include "XmlProcess.h"
#include "ToolsHelp.h"
#include "ParaCheck.h"
#include "NSSOperator.h"
#include "nss_mt.h"
#include "UserMgr.h"

// 通用查询转换成XML
IVS_INT32 CXmlProcess::GetUnifiedFormatQueryXML(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, CXml &xml,
	const IVS_CHAR* pDomainCode/* = NULL*/,const IVS_CHAR* pCamerCode)
{
	// pDomainCode不要事先判空，作为默认参数
	CHECK_POINTER(pUnifiedQuery, IVS_OPERATE_MEMORY_ERROR);  

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");	
	if (NULL != pDomainCode)
	{
		(void)xml.AddChildElem("DomainCode");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(pDomainCode);
		(void)xml.AddElem("PageInfo");
	}
	else
	{
		(void)xml.AddChildElem("PageInfo");
	}
	
	(void)xml.IntoElem();
	(void)xml.AddChildElem("FromIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>((pUnifiedQuery->stIndex).uiFromIndex)).c_str());
	(void)xml.AddElem("ToIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>((pUnifiedQuery->stIndex).uiToIndex)).c_str());

	IVS_UINT32 iFieldNum = static_cast<IVS_UINT32>(pUnifiedQuery->iFieldNum);	

	if (iFieldNum > 0) //如果是0的话，这个QueryCond节点都不要了
	{
		IVS_CHAR szValue[IVS_QUERY_VALUE_LEN+1]={0};
		(void)xml.AddElem("QueryCond");	
		for (unsigned int i=0;i < iFieldNum;i++)
		{	
			if (0 == i)
			{
				(void)xml.AddChildElem("QueryField");
			}
			else
			{
				(void)xml.AddElem("QueryField");	
			}

			(void)xml.IntoElem();
			(void)xml.AddChildElem("Field");
			(void)xml.IntoElem();

			std::string  iField = QUERYFILED.GetValue(pUnifiedQuery->stQueryField[i].eFieID);
			(void)xml.SetElemValue(iField.c_str());
			(void)xml.AddElem("Value");
			if (QUERY_CAMERA_CODE == pUnifiedQuery->stQueryField[i].eFieID || QUERY_OPERATION_OBJECT_CODE == pUnifiedQuery->stQueryField[i].eFieID)
			{
				if (NULL != pCamerCode)
				{
					(void)xml.SetElemValue(pCamerCode);
				}
				else
				{
					IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1];
					IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
					(IVS_VOID)CXmlProcess::ParseDevCode(pUnifiedQuery->stQueryField[i].cValue, chDevCode, chDomaCode);
					(void)xml.SetElemValue(chDevCode);
				}
			}
			else
			{
				if (!CToolsHelp::Memcpy(szValue, IVS_QUERY_VALUE_LEN, pUnifiedQuery->stQueryField[i].cValue, IVS_QUERY_VALUE_LEN))
				{
					BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Get Unified Format Query XML", "Memcpy szValue failed");
					return IVS_ALLOC_MEMORY_ERROR;
				}
				(void)xml.SetElemValue(szValue);
			}

			//注意如果这里的值不是0/1，直接返回参数错误的错误码
			CHECK_IVSBOOL(pUnifiedQuery->stQueryField[i].bExactQuery);
			
			(void)xml.AddElem("QueryType");
			if (MATCHED_SUCCEED == pUnifiedQuery->stQueryField[i].bExactQuery)
			{
				(void)xml.SetElemValue("EXACT");
			}
			else
			{
				(void)xml.SetElemValue("INEXACT");
			}
			xml.OutOfElem();
		}
		xml.OutOfElem();

		//注意如果这里的值不是0/1，直接返回参数错误的错误码
		CHECK_IVSBOOL(pUnifiedQuery->stOrderCond.bEnableOrder);

		if (pUnifiedQuery->stOrderCond.bEnableOrder)
		{
			(void)xml.AddElem("OrderCond");
			(void)xml.AddChildElem("OrderField");
			(void)xml.IntoElem();	
			std::string iOrderField = QUERYFILED.GetValue(pUnifiedQuery->stOrderCond.eFieID);
			(void)xml.SetElemValue(iOrderField.c_str());

			//注意如果这里的值不是0/1，直接返回参数错误的错误码
			CHECK_IVSBOOL(pUnifiedQuery->stOrderCond.bUp);

			(void)xml.AddElem("Order");
			if (MATCHED_SUCCEED==pUnifiedQuery->stOrderCond.bUp)
			{
				(void)xml.SetElemValue("UP");
			}
			else
			{
				(void)xml.SetElemValue("DOWN");
			}
			xml.OutOfElem();
		}
	}
	xml.OutOfElem();

	return IVS_SUCCEED;
}

IVS_INT32 CXmlProcess::PrintfXML2File(const IVS_CHAR* FileName,const IVS_CHAR* XMLContent)
{
	CHECK_POINTER(FileName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(XMLContent, IVS_OPERATE_MEMORY_ERROR);

	FILE *streamW;
	streamW = fopen(FileName,"w+");
	//int iLen = sizeof(XMLContent);
	if(NULL == streamW)
	{
		return IVS_OPEN_FILE_ERROR;
	}
	fprintf(streamW,"%s",XMLContent);

	fclose(streamW);
	return IVS_SUCCEED;
}

IVS_INT32 CXmlProcess::GetCommBuildRet( CXml &xmlReq, CXml &xmlRsp, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const std::string& strLinkID)
{
    IVS_UINT32 xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	BP_RUN_LOG_INF("COCXXmlProcess::GetCommBuildRet", "send XML = %s", pReq);
    //创建要发送的CMD，拼装了NSS消息头
    CCmd* pCmd = CNSSOperator::instance().BuildSMUCmd(msgType, pReq, strLinkID);
    CHECK_POINTER(pCmd, IVS_SDK_RET_BP_CMD_REQ_INVALID);

    //发送消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);

    //返回响应消息
    IVS_INT32 iRet = IVS_SUCCEED;
    const IVS_CHAR* pRsp = NULL;
    if (BUILDRET == iType)
    {
        iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
    }
    else
    {
        pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);        
    }
	BP_RUN_LOG_INF("GetCommBuildRet", "revice XML = %s", pRsp);

    HW_DELETE(pCmdRsp);
    if (BUILSTRING == iType)
    {
        if (NULL == pRsp)
        {
            BP_RUN_LOG_ERR(iRet,"NULL == pRsp", "iRet = %d", iRet);
            return iRet;
        }

        if(iRet == IVS_SUCCEED)
        { 
            iRet = IVS_XML_INVALID;
            if(xmlRsp.Parse(pRsp))
            {
                iRet = IVS_SUCCEED;
            }
            IVS_DELETE(pRsp, MUILI);
        }
    }
    BP_RUN_LOG_INF("GetCommBuildRet", "iType = %d,iRet = %d", iType, iRet);
	return iRet;
}


IVS_INT32 CXmlProcess::GetCommBuildRet( CXml &xml, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const std::string& strLinkID)
{
    IVS_UINT32 xmlLen = 0;
    const IVS_CHAR* pReq = xml.GetXMLStream(xmlLen);
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	BP_RUN_LOG_INF("COCXXmlProcess::GetCommBuildRet", "send XML = %s", pReq);
    //创建要发送的CMD，拼装了NSS消息头
    CCmd* pCmd = CNSSOperator::instance().BuildSMUCmd(msgType, pReq, strLinkID);
    CHECK_POINTER(pCmd, IVS_SDK_RET_BP_CMD_REQ_INVALID);

    //发送消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);

    //返回响应消息
    IVS_INT32 iRet = IVS_SUCCEED;
    const IVS_CHAR* pRsp = NULL;
    if (BUILDRET == iType)
    {
        iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
    }
    else
    {
        pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);        
    }
	BP_RUN_LOG_INF("GetCommBuildRet", "revice XML = %s", pRsp);

    HW_DELETE(pCmdRsp);
    if (BUILSTRING == iType)
    {
        if (NULL == pRsp)
        {
            BP_RUN_LOG_ERR(iRet,"NULL == pRsp", "iRet = %d", iRet);
            return iRet;
        }

        if(iRet == IVS_SUCCEED)
        { 
            iRet = IVS_XML_INVALID;
            xml.RemoveElem();
            if(xml.Parse(pRsp))
            {
                iRet = IVS_SUCCEED;
            }
            IVS_DELETE(pRsp, MUILI);
        }
    }
    BP_RUN_LOG_INF("GetCommBuildRet", "iType = %d,iRet = %d", iType, iRet);
	return iRet;
}

IVS_INT32 CXmlProcess::ParseDevCodeList(std::list<std::string>& lDevCodeList,std::string& strCamerCode,IVS_CHAR* pDomainCode )
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	memset(pDomainCode, 0, IVS_DOMAIN_CODE_LEN+1);
	std::string strDevBigCode;
	while (!lDevCodeList.empty())
	{
		strDevBigCode = lDevCodeList.front();
		lDevCodeList.pop_front();
		if (IVS_DEV_CODE_LEN < strDevBigCode.size())
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "ParseDevCode", "IVS_DEV_CODE_LEN != strlen(pDevBigCode)");
			continue;
		}

		std::string strDevCode = strDevBigCode.substr(0,strDevBigCode.rfind("#"));
		std::string strDomainCode = strDevBigCode.substr(strDevBigCode.rfind("#") + 1, strlen(strDevBigCode.c_str()));

		strCamerCode.append(strDevCode);
		if (!lDevCodeList.empty())
		{
			strCamerCode.append(",");
		}
		if(!CToolsHelp::Strncpy(pDomainCode, IVS_DOMAIN_CODE_LEN+1, strDomainCode.c_str(), IVS_DOMAIN_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "parse dev code", "Copy pDomainCode failed");
			continue;
		}
	}
	return IVS_SUCCEED;
}



//解析的Devcode
IVS_INT32 CXmlProcess::ParseDevCode(const IVS_CHAR *pDevBigCode, IVS_CHAR *pDevCode, IVS_CHAR* pDomainCode )
{
    CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDevBigCode, IVS_OPERATE_MEMORY_ERROR);  

    if (IVS_DEV_CODE_LEN < strlen(pDevBigCode))
    {
        BP_RUN_LOG_ERR(IVS_CODE_INVALID, "ParseDevCode", "IVS_DEV_CODE_LEN != strlen(pDevBigCode)");
        return IVS_CODE_INVALID;
    }
    memset(pDevCode, 0, IVS_SDK_DEV_CODE_LEN+1);
    memset(pDomainCode, 0, IVS_DOMAIN_CODE_LEN+1);

	// 避免常量测试出错
	std::string strDevBigCode = pDevBigCode;
    std::string strDevCode;
    std::string strDomainCode;
    if (std::string::npos == strDevBigCode.rfind("#"))
    {
        strDevCode = strDevBigCode;
    }
    else
    {
        strDevCode = strDevBigCode.substr(0,strDevBigCode.rfind("#"));
        strDomainCode = strDevBigCode.substr(strDevBigCode.rfind("#") + 1, strlen(strDevBigCode.c_str()));
    }
	
	if(!CToolsHelp::Strncpy(pDevCode, IVS_SDK_DEV_CODE_LEN+1, strDevCode.c_str(), IVS_SDK_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "parse dev code", "Copy pDevCode failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	if(!CToolsHelp::Strncpy(pDomainCode, IVS_DOMAIN_CODE_LEN+1, strDomainCode.c_str(), IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "parse dev code", "Copy pDomainCode failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	
	//BP_RUN_LOG_INF("ParseDevCode", "big camera code[%s], dev code[%s], domain code[%s]", pDevBigCode, pDevCode, pDomainCode);
    return IVS_SUCCEED;
}

bool CXmlProcess::TransIntToBool(unsigned int iRet)
{
	if(0 == iRet)
		return false;
	else
		return true;
}

std::string CXmlProcess::TransIntToOnOff(unsigned int iRet)
{
	if(OFF_LINE == iRet)
		return "OFF";
	else if(ON_LINE == iRet)
		return "ON";
	else
		return "";
}

long CXmlProcess::TransStrToLong(const std::string& str)
{
	long lRet = 0;
	if (CToolsHelp::IsNum(str))
	{
		(void)sscanf_s(str.c_str(), STR_FORMAT_L, &lRet);
		return lRet;
	}
	else
		return lRet;
}

unsigned int CXmlProcess::TransOnOffToInt(const std::string &str)
{
	unsigned int iRet = OFF_LINE;
	if (str.empty())
	{
		iRet = OFF_LINE;
	}
	else
	{
		if ((MATCHED == str.compare("ON"))||(MATCHED == str.compare("on")))
		{
			iRet = ON_LINE;
		}
		else if((MATCHED == str.compare("OFF"))||(MATCHED == str.compare("off")))
		{
			iRet = OFF_LINE;
		}
		else
			iRet = OFF_LINE;
	}
	return iRet;
}

 int CXmlProcess::TransStrToInt(const std::string& str)
{
	int iRet = 0;
	if (CToolsHelp::IsNum(str))
	{
		(void)sscanf_s(str.c_str(), STR_FORMAT_D, &iRet);
		return iRet;
	}
	else
		return iRet;
}

 void CXmlProcess::SetElemValueStr(const IVS_CHAR* pXmlValue,IVS_UINT32 ulLength,CXml &xml)
 {
	 CHECK_POINTER_VOID(pXmlValue);
	 IVS_CHAR* pXmlNewValue = IVS_NEW((IVS_CHAR* &)pXmlNewValue,ulLength+1);
	 CHECK_POINTER_VOID(pXmlNewValue);
	 memset(pXmlNewValue,0,ulLength+1);
	 if (!CToolsHelp::Memcpy(pXmlNewValue, ulLength+1, pXmlValue, ulLength))
	 {
		 BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy fail", "NA");
		 IVS_DELETE(pXmlNewValue,MUILI);
		 return;
	 }
	 (void)xml.SetElemValue(pXmlNewValue);
	 IVS_DELETE(pXmlNewValue,MUILI);
 }

 unsigned int CXmlProcess::TransBinStrToDec(const std::string& str)
 {
	 unsigned int ulResult = 0;
	 int iTemp = '0';
	 int strlength = static_cast<int>(strlen(str.c_str())-1);
	 for (int i=strlength;i>=0;i--)
	 {
		 iTemp = static_cast<int>(str[strlength - i] - '0');//lint !e732
		 ulResult+=(static_cast<unsigned int>(pow(2.0,static_cast<double>(i))*iTemp));
	 }
	 return ulResult;
 }

 // 修改AlarmInCode
 IVS_INT32 CXmlProcess::SetAlarmInCodeValue(const IVS_CHAR* pPath, CXml& xml)
 {
	 CHECK_POINTER(pPath, IVS_OPERATE_MEMORY_ERROR);

	 if (!xml.FindElemEx(pPath))
	 {
		 BP_RUN_LOG_ERR(IVS_XML_INVALID, "SetAlarmInCodeValue", "xml.FindElemEx(pPath)");
		 return IVS_XML_INVALID;
	 }

	 const IVS_CHAR* pElemValue = xml.GetElemValue();
	 IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
	 IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	 IVS_INT32 iRet = CXmlProcess::ParseDevCode(pElemValue, szAlarmInCode, szDomainCode);
	 xml.ModifyElemValue(szAlarmInCode);
	 return iRet;
 }

 //通用的查询请求
 IVS_INT32 CXmlProcess::GetCommConfigGetXML(const IVS_CHAR* pDomainCode,const IVS_CHAR* pDevCode, CXml& xml, enConfigType enType)
 {
	 CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
	 CHECK_POINTER(pDevCode,IVS_OPERATE_MEMORY_ERROR);

	 (void)xml.AddDeclaration("1.0","UTF-8","");
	 (void)xml.AddElem("Content");

	 (void)xml.AddChildElem("DomainCode");
	 (void)xml.IntoElem();
	 (void)xml.SetElemValue(pDomainCode);
	 if (SDKCAMERA == enType)
	 {
		 (void)xml.AddElem("CameraCode");
	 }
	 else if (SDKDEVICE == enType)
	 {
		 (void)xml.AddElem("DevCode");
	 }
	 else if (SDKALARMINCODE == enType)
	 {
		 (void)xml.AddElem("AlarmInCode");
	 }
	 else if (SDKALARMOUTCODE == enType)
	 {
		 (void)xml.AddElem("AlarmOutCode");
	 }
	 else if (SDKSERIAL == enType)
	 {
		 (void)xml.AddElem("SerialCode");
	 }
	 else if (SDKVOICE == enType)
	 {
		 (void)xml.AddElem("VoiceCode");
	 }


	 (void)xml.SetElemValue(pDevCode);
	 xml.OutOfElem();

	 return IVS_SUCCEED;
 }

 // 实现外域发送消息公共方法
 IVS_INT32 CXmlProcess::GetCommSendMgr(CXml &xml, CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const IVS_CHAR* pDomainCode)
 {
	 CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	 CHECK_POINTER(pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	 IVS_UINT32 xmlLen = 0;
	 const IVS_CHAR* pReq = xml.GetXMLStream(xmlLen);
	 CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	 BP_RUN_LOG_INF("Get CommSendMgr", "send XML = %s", pReq);

	 // 构造带域的请求消息，并发送
	 CSendNssMsgInfo sendNssMsgInfo;
	 sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	 sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	 sendNssMsgInfo.SetReqID(msgType);
	 sendNssMsgInfo.SetReqData(pReq);	
	 sendNssMsgInfo.SetDomainCode(pDomainCode);
	 std::string strpRsp;
	 IVS_INT32 iNeedRedirect = IVS_FAIL;
	 IVS_INT32 iRet = pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
     if (IVS_SUCCEED != iRet)
     {
         BP_RUN_LOG_ERR(iRet,"Get Comm Send Mgr", "fail");
         return iRet;
     }


	 // 返回相应消息
	 if (BUILSTRING == iType)
	 {
		 if (strpRsp.empty())
		 {
			 BP_RUN_LOG_ERR(iRet,"NULL == strpRsp", "iRet = %d", iRet);
			 return iRet;
		 }
		 if(iRet == IVS_SUCCEED)
		 { 
			 iRet = IVS_XML_INVALID;
			 xml.RemoveElem();
			 if(xml.Parse(strpRsp.c_str()))
			 {
				 iRet = IVS_SUCCEED;
			 }
		 }
		 BP_RUN_LOG_INF("Get CommSendMgr", "revice XML = %s", strpRsp.c_str());
	 }

	 return iRet;
 }

 // 实现外域发送消息公共方法 -- 解决内存泄露
 IVS_INT32 CXmlProcess::GetCommSendMgr(CXml &xml, CXml &OutXml,CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const IVS_CHAR* pDomainCode)
 {
	 CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	 CHECK_POINTER(pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	 IVS_UINT32 xmlLen = 0;
	 const IVS_CHAR* pReq = xml.GetXMLStream(xmlLen);
	 CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	 BP_RUN_LOG_INF("Get CommSendMgr", "send XML = %s", pReq);

	 // 构造带域的请求消息，并发送
	 CSendNssMsgInfo sendNssMsgInfo;
	 sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	 sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	 sendNssMsgInfo.SetReqID(msgType);
	 sendNssMsgInfo.SetReqData(pReq);	
	 sendNssMsgInfo.SetDomainCode(pDomainCode);
	 std::string strpRsp;
	 IVS_INT32 iNeedRedirect = IVS_FAIL;
	 IVS_INT32 iRet = pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	 // 返回相应消息
	 if (BUILSTRING == iType)
	 {
		 if (strpRsp.empty())
		 {
			 BP_RUN_LOG_ERR(iRet,"NULL == strpRsp", "iRet = %d", iRet);
			 return iRet;
		 }
		 if(iRet == IVS_SUCCEED)
		 { 
			 iRet = IVS_XML_INVALID;
			 OutXml.RemoveElem();
			 if(OutXml.Parse(strpRsp.c_str()))
			 {
				 iRet = IVS_SUCCEED;
			 }
		 }
		 BP_RUN_LOG_INF("Get CommSendMgr", "revice XML = %s", strpRsp.c_str());
	 }

	 return iRet;
 }

 // 实现外域发送消息公共方法(支持重定向)
 IVS_INT32 CXmlProcess::GetCommSendMgrForRedirect(CXml &xml, CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode)
 {
	 CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	 CHECK_POINTER(pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	 IVS_UINT32 xmlLen = 0;
	 const IVS_CHAR* pReq = xml.GetXMLStream(xmlLen);
	 CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	 BP_RUN_LOG_INF("Get CommSendMgr For Redirect", "send XML = %s", pReq);

	 // 构造带域的请求消息，并发送
	 CSendNssMsgInfo sendNssMsgInfo;
	 sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	 sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	 sendNssMsgInfo.SetReqID(msgType);
	 sendNssMsgInfo.SetReqData(pReq);
	 sendNssMsgInfo.SetCameraCode(pCameraCode);
	 sendNssMsgInfo.SetDomainCode(pDomainCode);
	 std::string strpRsp;
	 IVS_INT32 iNeedRedirect = IVS_FAIL;
	 IVS_INT32 iRet = pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

     if (IVS_SUCCEED != iRet)
     {
          BP_RUN_LOG_ERR(iRet,"Get CommSendMgr For Redirect", "seng cmd fail");
          return iRet;
     }

	 if(NSS_GET_VOICE_INFO_REQ == msgType || NSS_GET_CAMERA_CFG_REQ == msgType)
	 {
		 if (IVS_SUCCEED == iNeedRedirect)
		 {
			 iRet = pUserMgr->SendRedirectServe(sendNssMsgInfo,strpRsp);
		 }
	 }

	 // 返回相应消息
	 if (BUILSTRING == iType)
	 {
		 if (strpRsp.empty())
		 {
			 BP_RUN_LOG_ERR(iRet,"NULL == strpRsp", "iRet = %d", iRet);
			 return iRet;
		 }
		 if(iRet == IVS_SUCCEED)
		 { 
			 iRet = IVS_XML_INVALID;
			 xml.RemoveElem();
			 if(xml.Parse(strpRsp.c_str()))
			 {
				 iRet = IVS_SUCCEED;
			 }
		 }
		 BP_RUN_LOG_INF("Get CommSendMgr For Redirect", "revice XML = %s", strpRsp.c_str());
	 }

	 return iRet;
 }

 // 实现外域发送消息公共方法(支持重定向) -- 解决内存泄露
 IVS_INT32 CXmlProcess::GetCommSendMgrForRedirect(CXml &reqXml, CXml &rspXml,CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode)
 {
	 CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	 CHECK_POINTER(pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	 IVS_UINT32 xmlLen = 0;
	 const IVS_CHAR* pReq = reqXml.GetXMLStream(xmlLen);
	 CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	 BP_RUN_LOG_INF("Get CommSendMgr For Redirect", "send XML = %s", pReq);

	 // 构造带域的请求消息，并发送
	 CSendNssMsgInfo sendNssMsgInfo;
	 sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	 sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	 sendNssMsgInfo.SetReqID(msgType);
	 sendNssMsgInfo.SetReqData(pReq);
	 sendNssMsgInfo.SetCameraCode(pCameraCode);
	 sendNssMsgInfo.SetDomainCode(pDomainCode);
	 std::string strpRsp;
	 IVS_INT32 iNeedRedirect = IVS_FAIL;
	 IVS_INT32 iRet = pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	 if(NSS_GET_VOICE_INFO_REQ == msgType || NSS_GET_CAMERA_CFG_REQ == msgType)
	 {
		 if (IVS_SUCCEED == iNeedRedirect)
		 {
			 iRet = pUserMgr->SendRedirectServe(sendNssMsgInfo,strpRsp);
		 }
	 }

	 // 返回相应消息
	 if (BUILSTRING == iType)
	 {
		 if (strpRsp.empty())
		 {
			 BP_RUN_LOG_ERR(iRet,"NULL == strpRsp", "iRet = %d", iRet);
			 return iRet;
		 }
		 if(iRet == IVS_SUCCEED)
		 { 
			 iRet = IVS_XML_INVALID;
			 rspXml.RemoveElem();
			 if(rspXml.Parse(strpRsp.c_str()))
			 {
				 iRet = IVS_SUCCEED;
			 }
		 }
		 BP_RUN_LOG_INF("Get CommSendMgr For Redirect", "revice XML = %s", strpRsp.c_str());
	 }

	 return iRet;
 }

 // 通用请求XML增加QueryField字段
 IVS_INT32 CXmlProcess::AddQueryFieldForUnifiedFormatQueryXML(const IVS_CHAR* pField,const IVS_CHAR* pValue, CXml& xmlReq)
 {
	 CHECK_POINTER(pField, IVS_OPERATE_MEMORY_ERROR);
	 CHECK_POINTER(pValue, IVS_OPERATE_MEMORY_ERROR);

	 std::string sQueryType = "EXACT";
	 if (xmlReq.FindElemEx("Content/PageInfo/QueryCond/QueryField"))
	 {
		 xmlReq.OutOfElem();
		 (void)xmlReq.AddElem("QueryField");
		 (void)xmlReq.AddChildElem("Field");
		 (void)xmlReq.IntoElem();
		 (void)xmlReq.SetElemValue(pField);
		 (void)xmlReq.AddElem("Value");
		 (void)xmlReq.SetElemValue(pValue); 
		 (void)xmlReq.AddElem("QueryType");
		 (void)xmlReq.SetElemValue(sQueryType.c_str());
		 xmlReq.OutOfElem();
	 }
	 return IVS_SUCCEED;
 }

