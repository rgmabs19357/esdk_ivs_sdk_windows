#include "AlarmLinkageXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "UserMgr.h"
#include "IVS_Trace.h"
CAlarmLinkagrXMLProcess::CAlarmLinkagrXMLProcess()
{
}


CAlarmLinkagrXMLProcess::~CAlarmLinkagrXMLProcess()
{
}


// 联动策略拼装xml，（包含联动策略ID）(查询联动策略详情和删除联动策略用到)
IVS_INT32 CAlarmLinkagrXMLProcess::GetAndDelAlarmLinkageGetXML(const IVS_CHAR* pReqXml, CXml& xml, std::string& strDomainCode)
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;//修复内存泄露问题（得到最终结果前，不直接操作入参xml）
	CXml xmlModified;
	// 找到AlarmInCode节点，拆分取出域编码，修改AlarmInCode节点值
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	if (!xmlReq.FindElemEx("Content/Linkage/AlarmInCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(\"Content/Linkage/AlarmInCode\") fail", "NA");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* pAlarmInCode = xmlReq.GetElemValue();
	IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pAlarmInCode, szAlarmInCode, szDomainCode);
	strDomainCode = szDomainCode;
	xmlReq.ModifyElemValue(szAlarmInCode);
	// Content节点下增加DomainCode节点
	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_FAIL);
	iRet = AddDomainCode4XML(szDomainCode, "Content", "DomainCode", pReq, xmlModified);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddDomainCode4XML fail", "NA");
		return iRet;
	}
	// AlarmInType节点下添加DevDomainCode节点
	pReq = xmlModified.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_FAIL);
	iRet = AddDomainCode4XML(szDomainCode, "Content/Linkage/AlarmInType", "DevDomainCode", pReq, xml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddDomainCode4XML fail", "NA");
		return iRet;
	}
	return iRet;
}

// 查询告警联动列表，修改响应消息中的AlarmInCode
IVS_INT32 CAlarmLinkagrXMLProcess::GetAlarmInCode(const IVS_CHAR* pReqXML, IVS_CHAR*& pRspXml)
{
	CHECK_POINTER(pReqXML, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	// 在请求xml中找到AlarmInCode，解析出域编码
	CXml xml;
	if (!xml.Parse(pReqXML))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pReqXML)", "NA");
		return IVS_XML_INVALID;
	}
	if (!xml.FindElemEx("Content/AlarmInCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/AlarmInCode)", "NA");
		return IVS_XML_INVALID;
	}

	const IVS_CHAR* pAlarmInCode = xml.GetElemValue();
	IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};	
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pAlarmInCode, szAlarmInCode, szDomainCode);

	// 处理响应消息中AlarmInCode，加上#和域编码
	CXml xmlRsp;
	if (!xmlRsp.Parse(pRspXml))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.Parse(pRspXml)", "NA");
		return IVS_SMU_XML_INVALID;
	}

	if (!xmlRsp.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.FindElemEx(Content/LinkageList)", "NA");
		return IVS_SMU_XML_INVALID;
	}
	(void)xmlRsp.IntoElem();
	if (!xmlRsp.FindElem("LinkageList"))
	{
		BP_RUN_LOG_INF("Get NA Info", "NA");
		return iRet;
	}
	(void)xmlRsp.IntoElem();
	if (!xmlRsp.FindElem("Linkage"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.FindElemEx(Linkage)", "NA");
		return IVS_SMU_XML_INVALID;
	}
	for(;;)
	{
		(void)xmlRsp.IntoElem();
		if (!xmlRsp.FindElem("DevDomainCode"))
		{
			BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.FindElemEx(DevDomainCode)", "NA");
			return IVS_SMU_XML_INVALID;
		}
		std::string strDomainCode;
        const IVS_CHAR* pDomainCode = xmlRsp.GetElemValue();
        if (NULL != pDomainCode)
        {
            strDomainCode = pDomainCode;
        }

		if (!xmlRsp.FindElem("AlarmInCode"))
		{
			BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.FindElemEx(AlarmInCode)", "NA");
			return IVS_SMU_XML_INVALID;
		}
		std::string strAlarmInCode;
        pAlarmInCode = xmlRsp.GetElemValue();
        if (NULL != pAlarmInCode)
        {
            strAlarmInCode = pAlarmInCode;
        }
		if (!strAlarmInCode.empty())
		{
			strAlarmInCode.append("#");
			strAlarmInCode.append(strDomainCode);
			xmlRsp.ModifyElemValue(strAlarmInCode.c_str());
		}	
		xmlRsp.OutOfElem();
		if (!xmlRsp.NextElem())
		{
			break;
		}
	}	

	IVS_UINT32 uiXmlLen = 0;
	const IVS_CHAR* pRsp = xmlRsp.GetXMLStream(uiXmlLen);
	IVS_DELETE(pRspXml, MUILI);
    CHECK_POINTER(pRsp,IVS_SMU_XML_INVALID);//需判断pRsp是否为NULL，再调用strlen
	IVS_CHAR* pResultXml = IVS_NEW(pResultXml, strlen(pRsp) + 1);
	memset(pResultXml, 0x0, strlen(pRsp) + 1);
	if (!CToolsHelp::Memcpy(pResultXml, strlen(pRsp) + 1, pRsp, strlen(pRsp) + 1))
	{
		IVS_DELETE(pResultXml, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy pRsp to pResultXml fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	pRspXml = pResultXml;
	return iRet;
}

// 修改请求消息中的AlarmInCode，DevCode(去掉#域编码)，并且新增DomainCode和DevDomainCode节点(新增，修改联动策略)
IVS_INT32 CAlarmLinkagrXMLProcess::SetAlarmInCodeValue(const IVS_CHAR* pReqXml, CXml& xml, std::string& strDomainCode)
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);	
	IVS_DEBUG_TRACE("");
	IVS_UINT32 xmlLen = 0;
	CXml xmlReq;//修复内存泄露问题（得到最终结果前，不直接操作入参xml）
	CXml xmlModified;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml)", "NA");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	// 修改AlarmInCode， 获取消息目标域编码
	if (!xmlReq.FindElemEx("Content/Linkage/AlarmInCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(Content/Linkage/AlarmInCode)", "NA");
		return IVS_XML_INVALID;
	}

	const IVS_CHAR* pElemValue = xmlReq.GetElemValue();	
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pElemValue, szAlarmInCode, szDomainCode);
	if (NULL != pElemValue)
	{
		xmlReq.ModifyElemValue(szAlarmInCode);
	}
	// 节点AlarmInCode下添加DevDomainCode节点
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_FAIL);
	iRet = AddDomainCode4XML(szDomainCode, "Content/Linkage/AlarmInType", "DevDomainCode", pReq, xmlModified);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddDomainCode4XML fail", "NA");
		return iRet;
	}
	strDomainCode = szDomainCode;

	// 修改联动设备的DevCode 拆分DevCode，添加DevDomainCode节点
	if (!xmlModified.FindElemEx("Content/Linkage/ActionList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlModified.FindElemEx(Content/Linkage/ActionList)", "NA");
		return IVS_XML_INVALID;
	}
	if (!xmlModified.FindElem("Action"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlModified.FindElemEx(DevInfo)", "NA");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* szElemValue = NULL;
	IVS_CHAR szDevCode[IVS_DEV_CODE_LEN + 1] = {0};
	do 
	{
		(void)xmlModified.IntoElem();
		if (!xmlModified.FindElem("DevList"))
		{
			xmlModified.OutOfElem();
			continue;
		}
		(void)xmlModified.IntoElem();
		if (!xmlModified.FindElem("DevInfo"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlModified.FindElemEx(DevInfo)", "NA");
			return IVS_XML_INVALID;
		}
		do 
		{
			(void)xmlModified.IntoElem();
			GET_ELEM_VALUE_CHAR("DevCode", szElemValue, szDevCode, IVS_DEV_CODE_LEN, xmlModified);
			iRet = CXmlProcess::ParseDevCode(szDevCode, szAlarmInCode, szDomainCode);
			xmlModified.ModifyElemValue(szAlarmInCode);
			if (!xmlModified.FindElem("Param"))
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlModified.FindElem(\"Param\") fail", "NA");
				return IVS_XML_INVALID;
			}
			std::string strParam = xmlModified.GetElemValue();
			// 删除Param节点，新增加DevDomainCode和Param节点(使DevdomainCode在Param之前)
			(void)xmlModified.RemoveElem();
			// 判断联动设备域编码是否为本域域编码(联动设备域编码和消息目标域比较)
			(void)xmlModified.AddElem("DevDomainCode");
			if (0 == strcmp(strDomainCode.c_str(), szDomainCode))
			{				
				(void)xmlModified.SetElemValue("");
			}
			else
			{
				(void)xmlModified.SetElemValue(szDomainCode);
			}
			(void)xmlModified.AddElem("Param");
			(void)xmlModified.SetElemValue(strParam.c_str());

			xmlModified.OutOfElem();
		} while (xmlModified.NextElem());

		xmlModified.OutOfElem();
		xmlModified.OutOfElem();
	} while (xmlModified.NextElem());

	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SetDevDomainCode4Xml fail", "NA");
		return iRet;
	}
	// Content节点下添加DomainCode节点
	pReq = xmlModified.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_FAIL);
	iRet = AddDomainCode4XML(strDomainCode.c_str(), "Content", "DomainCode", pReq, xml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddDomainCode4XML fail", "NA");
		return iRet;
	}
	return iRet;
}

// 查询告警联动策略列表拼装请求xml，获取域编码
IVS_INT32 CAlarmLinkagrXMLProcess::GetAlarmLinkageListGetXml(const IVS_CHAR* pReqXml, CXml& xml, std::string& strDomainCode)
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");
	CXml xmlReq;//修复内存泄露问题（得到最终结果前，不直接操作入参xml）
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pReqXml)", "NA");
		return IVS_XML_INVALID;
	}
	if (!xmlReq.FindElemEx("Content/AlarmInCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetAlarmLinkageListGetXml", "xmlReq.FindElemEx(pPath)");
		return IVS_XML_INVALID;
	}
	// 找到AlarmInCode，拆分并修改AlarmInCode节点的值
	const IVS_CHAR* pElemValue = xmlReq.GetElemValue();
	IVS_CHAR szAlarmInCode[IVS_ALARM_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pElemValue, szAlarmInCode, szDomainCode);
	strDomainCode = szDomainCode;
	xmlReq.ModifyElemValue(szAlarmInCode);
	// 增加DevDomainCode节点
	(void)xmlReq.AddElem("DevDomainCode");
	(void)xmlReq.SetElemValue(szDomainCode);
	// Content下添加DomainCode节点
	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_FAIL);
	iRet = AddDomainCode4XML(szDomainCode, "Content", "DomainCode", pReq, xml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddDomainCode4XML Add DomainCode under Content fail", "NA");
		return iRet;
	}
	return iRet;
}
// 在xml中插入节点
IVS_INT32 CAlarmLinkagrXMLProcess::AddDomainCode4XML(const char* pDomainCode, const IVS_CHAR* pDomainCodePath, const IVS_CHAR* pValueName, const IVS_CHAR* pReqXml, CXml& xml)
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_INT32 iRet = IVS_SUCCEED;
	// 构造LoginInfo的节点
	TiXmlElement* pDomainCodeElem = NULL;
	try
	{
		pDomainCodeElem = new TiXmlElement(pValueName);
	}
	catch(...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "new TiXmlElement(LoginInfo) is error", "NA");
		pDomainCodeElem = NULL;
		return IVS_ALLOC_MEMORY_ERROR;
	}

	TiXmlText* pXmlText = NULL;
	try
	{
		pXmlText = new TiXmlText(pDomainCode);
	}
	catch(...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "new TiXmlText(pLoginID) is error", "NA");
		IVS_DELETE(pDomainCodeElem, 0);
		pXmlText = NULL;
		return IVS_ALLOC_MEMORY_ERROR;
	}
	pDomainCodeElem->LinkEndChild(pXmlText);
	// 加载XML
	TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	if (NULL == pXmlDoc)
	{
		BP_RUN_LOG_INF("NULL == pXmlDoc", "NA");
		pXmlDoc = NULL;
		IVS_DELETE(pDomainCodeElem, 0);
		return IVS_FAIL; //lint !e429 pXmlText在释放pDomainCodeElem时一起释放
	}
	//CHECK_POINTER(pXmlDoc, IVS_FAIL);
	(void)(pXmlDoc->Parse(pReqXml, NULL, TIXML_ENCODING_LEGACY));
	if (0 != pXmlDoc->Error())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "parse xml failed", "NA");
		IVS_DELETE(pXmlDoc);
		IVS_DELETE(pDomainCodeElem, 0);
		return IVS_FAIL; //lint !e429
	}

	do 
	{		
		TiXmlElement* pRootElem = pXmlDoc->RootElement();
		if (strcmp(pDomainCodePath, "Content") == 0)
		{
			// 查找content节点，如果根节点就是content就不要找了，下面那个方法有缺陷
			//TiXmlElement* pRootElem = pXmlDoc->RootElement();
			if (NULL == pRootElem)
			{
				BP_RUN_LOG_INF("NULL == pRootElem", "NA");
				iRet = IVS_FAIL;
				break;
			}
			TiXmlElement* pContent = NULL;
			if (strcmp(pRootElem->Value(), "Content") == 0)
			{
				pContent = pXmlDoc->RootElement();
			}
			else
			{
				pContent = CToolsHelp::FindElementByPath(pXmlDoc->RootElement()->Parent(),"Content");
			}
			if (NULL == pContent)
			{
				BP_RUN_LOG_INF("NULL == pContent", "NA");
				iRet = IVS_FAIL;
				break;
			}

			// 如果没有子节点 直接返回错
			TiXmlElement* pContentFirstChild = pContent->FirstChildElement();
			if (NULL == pContentFirstChild)
			{
				BP_RUN_LOG_INF("NULL == pContentFirstChild", "NA");
				iRet = IVS_FAIL;
				break;
			}
			// 在找到的第一个节点之前插入logininfo
			if (NULL == pContent->InsertBeforeChild(pContentFirstChild, *pDomainCodeElem))
			{
				BP_RUN_LOG_ERR(IVS_FAIL, "InsertBeforeChild error", "NA");
				iRet = IVS_FAIL;
				break;
			}
		}
		else
		{
			TiXmlElement* pElem = NULL;
			pElem = CToolsHelp::FindElementByPath(pXmlDoc->RootElement()->Parent(),pDomainCodePath);
			if ((NULL == pElem) || (NULL == pElem->Parent()->ToElement()->InsertAfterChild(pElem, *pDomainCodeElem)))
			{
				BP_RUN_LOG_ERR(IVS_FAIL, "InsertAfterChild  or FindElementByPath error", "NA");
				iRet = IVS_FAIL;
				break;
			}
		}
	} while (false); //lint !e717
	

	// 将拼装好的xml打到Cxml对象里面，保持外部一致
	TiXmlPrinter xmlPrinter;
	xmlPrinter.SetStreamPrinting();		

	if (!pXmlDoc->Accept(&xmlPrinter))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "print error", "NA");
		IVS_DELETE(pXmlDoc);
		IVS_DELETE(pDomainCodeElem, 0);
		return IVS_FAIL;//lint !e429
	}

	(void)xml.Parse(xmlPrinter.CStr());

	IVS_DELETE(pXmlDoc);
	IVS_DELETE(pDomainCodeElem, 0);

	return iRet;//lint !e429
}//lint !e429

//
IVS_INT32 CAlarmLinkagrXMLProcess::AppendDevCode(std::string& strNSSRsq, const IVS_CHAR* pPath, CXml& xml, const std::string& strDomainCode)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pPath, IVS_PARA_INVALID);

	if (!xml.Parse(strNSSRsq.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.Parse(strNSSRsq.c_str())", "NA");
		return IVS_SMU_XML_INVALID;
	}
	if (!xml.FindElemEx(pPath))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.FindElemEx(pPath)", "NA");
		return IVS_SMU_XML_INVALID;
	}
	if (!xml.FindElem("Action"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.FindElem(Action)", "NA");
		return IVS_SMU_XML_INVALID;
	}

	const IVS_CHAR* szElemValue = NULL;
	IVS_CHAR szDevCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	std::string strBigCode;
	do 
	{
		(void)xml.IntoElem();
		// DevList是O字段，可以没有，但是Action是M字段，找不到DevList，循环还需要继续
		if (!xml.FindElem("DevList"))
		{
			xml.OutOfElem();
			continue;
		}
		(void)xml.IntoElem();
		if (!xml.FindElem("DevInfo"))
		{
			BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.FindElemEx(DevInfo) fail", "NA");
			return IVS_SMU_XML_INVALID;
		}
		do 
		{
			(void)xml.IntoElem();
			memset(szDomainCode,0,IVS_DOMAIN_CODE_LEN+1);
			GET_ELEM_VALUE_CHAR("DevDomainCode", szElemValue, szDomainCode, IVS_DOMAIN_CODE_LEN, xml);
			if (0 == strcmp(szDomainCode, ""))
			{
				(void)CToolsHelp::Memcpy(szDomainCode, IVS_DOMAIN_CODE_LEN, strDomainCode.c_str(), strDomainCode.size());
			}
			GET_ELEM_VALUE_CHAR("DevCode", szElemValue, szDevCode, IVS_DEV_CODE_LEN, xml);
			strBigCode.assign(szDevCode);
			strBigCode.append("#").append(szDomainCode);
			xml.ModifyElemValue(strBigCode.c_str());
			xml.OutOfElem();
		} while (xml.NextElem());

		xml.OutOfElem();
		xml.OutOfElem();
	} while (xml.NextElem());
	return IVS_SUCCEED;
}//lint !e1764

