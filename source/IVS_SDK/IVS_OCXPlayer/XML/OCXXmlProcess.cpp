#include "OCXXmlProcess.h"
#include "ToolsHelp.h"
#include "IVS_Trace.h"

IVS_INT32 COCXXmlProcess::GetVideoNumAndResourceXML(IVS_INT32 iRet,IVS_BOOL bReachLimit,IVS_BOOL bWhetherCited, std::string& strOutXml)
{
	 IVS_DEBUG_TRACE("");
	 CXml xmlRsp;
	 (void)xmlRsp.AddDeclaration("1.0","UTF-8","");
	 (void)xmlRsp.AddElem("Content");
	 (void)xmlRsp.IntoElem();
	 (void)xmlRsp.AddChildElem("ResultCode");
	 (void)xmlRsp.IntoElem();
	 (void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());

	 (void)xmlRsp.AddElem("ReachLimit");
	 (void)xmlRsp.IntoElem();
	 (void)xmlRsp.SetElemValue(CToolsHelp::Int2Str((int)bReachLimit).c_str());

	 (void)xmlRsp.AddElem("WhetherCited");
	 (void)xmlRsp.IntoElem();
	 (void)xmlRsp.SetElemValue(CToolsHelp::Int2Str((int)bWhetherCited).c_str());

	 xmlRsp.OutOfElem();
	 IVS_UINT32 iLen = 0;
	 const char* pXml = xmlRsp.GetXMLStream(iLen);
	 if (NULL == pXml)
	 {
		 return IVS_OPERATE_MEMORY_ERROR;
	 }
	 strOutXml = pXml;
	 return IVS_SUCCEED;
}

IVS_INT32 COCXXmlProcess::GetUnifiedFormatQueryParseXML(IVS_VOID* &pReqBuffer,CXml& xmlReq,IVS_INT32 iFiedNum)
{
    IVS_DEBUG_TRACE("");
	// 分配空间 失败返回IVS_ALLOC_MEMORY_ERROR
	if (IVS_SUCCEED != AlloceQueryStructMemory(pReqBuffer, iFiedNum))
	{
		return IVS_ALLOC_MEMORY_ERROR;
	}    

	// 查找pageinfo 这个是must字段
	if (!xmlReq.FindElemEx("Content/PageInfo"))
	{
		IVS_DELETE(pReqBuffer, MUILI);
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Unified Format Query Parse XML", "can not find Element Path(Content/PageInfo)");
		return IVS_XML_INVALID;
	}
	IVS_QUERY_UNIFIED_FORMAT *pQueryUnifiedFormat = (IVS_QUERY_UNIFIED_FORMAT*)pReqBuffer;
	const IVS_CHAR* szElemValue = NULL;
	IVS_INT32 iFromTemp = 0;
	IVS_INT32 iToTemp = 0;

	// 获取请求的数量完成  从第几条记录开始 v    //截止第几条记
	GET_ELEM_VALUE_NUM("FromIndex", szElemValue, iFromTemp, xmlReq);
	GET_ELEM_VALUE_NUM("ToIndex", szElemValue, iToTemp, xmlReq);
	// sdk在这要对值进行判断
	if (iFromTemp > iToTemp || iFromTemp <= 0 || iToTemp <= 0)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Unified Format Query Parse XML", "FromIndex[%d] and ToIndex[%d] failed", iFromTemp, iToTemp);
		IVS_DELETE(pReqBuffer, MUILI);
		return IVS_XML_INVALID;
	}

	pQueryUnifiedFormat->stIndex.uiFromIndex = static_cast<IVS_UINT32>(iFromTemp);
	pQueryUnifiedFormat->stIndex.uiToIndex = static_cast<IVS_UINT32>(iToTemp);   
	// 获取请求的数量完成

	// 这个是查询条件的个数
	pQueryUnifiedFormat->iFieldNum = iFiedNum;

	// 查找查询条件元素 这个是O，不是must
	if (xmlReq.FindElem("QueryCond"))
	{
		xmlReq.IntoElem();

		if (xmlReq.FindElem("QueryField"))
		{        
			IVS_CHAR cTemp[IVS_DESCRIBE_LEN+1];
			IVS_INT32 iIndex = 0;
			IVS_QUERY_FIELD stQueryField;
			do 
			{
				if (!xmlReq.FindElemValue("QueryField"))
				{
					break;
				}
				xmlReq.IntoElem();
				szElemValue = NULL;

				memset(&stQueryField, 0, sizeof(IVS_QUERY_FIELD));

				// 对于字段的值，不知道具体的大小 现在目前先定IVS_DESCRIBE_LEN
				// 查询字段
				memset(cTemp, 0, IVS_DESCRIBE_LEN+1);
				GET_ELEM_VALUE_CHAR("Field", szElemValue, cTemp, IVS_DESCRIBE_LEN, xmlReq);
				stQueryField.eFieID = QUERYFILED.GetTypeID(cTemp);
				// 查询字段的值
				GET_ELEM_VALUE_CHAR("Value", szElemValue,stQueryField.cValue, IVS_QUERY_VALUE_LEN, xmlReq);
				// 查询类型
				memset(cTemp, 0, IVS_DESCRIBE_LEN);
				GET_ELEM_VALUE_CHAR("QueryType", szElemValue, cTemp, IVS_DEV_CODE_LEN, xmlReq);
				stQueryField.bExactQuery = GetQueryType(cTemp);
				xmlReq.OutOfElem();

				// (void)sdk_memcpy(&(pQueryUnifiedFormat->stQueryField[iIndex]),sizeof(IVS_QUERY_FIELD),&stQueryField,sizeof(IVS_QUERY_FIELD));
				if (!CToolsHelp::Memcpy(&(pQueryUnifiedFormat->stQueryField[iIndex]),sizeof(IVS_QUERY_FIELD),&stQueryField,sizeof(IVS_QUERY_FIELD)))
				{
					IVS_DELETE(pReqBuffer, MUILI);
					BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR ,"Get Unified Format Query Parse XML", "Memcpy(QueryField) failed");
					return IVS_ALLOC_MEMORY_ERROR;
				}
				iIndex++;

			} while (xmlReq.NextElem());          	
			xmlReq.OutOfElem();
			// 查找OrderCond字段，如果没有找到，不要返回，这是个o字段
			if (xmlReq.FindElem("OrderCond"))
			{
				// 默认的是排序的
				pQueryUnifiedFormat->stOrderCond.bEnableOrder = MATCHED_SUCCEED;
				xmlReq.IntoElem();

				// 排序字段
				memset(cTemp,0,IVS_DESCRIBE_LEN);
				GET_ELEM_VALUE_CHAR("OrderField", szElemValue,cTemp, IVS_DESCRIBE_LEN, xmlReq);
				pQueryUnifiedFormat->stOrderCond.eFieID = QUERYFILED.GetTypeID(cTemp);
				// 升序还是降序
				memset(cTemp, 0, IVS_DESCRIBE_LEN);
				GET_ELEM_VALUE_CHAR("Order", szElemValue, cTemp, IVS_DESCRIBE_LEN, xmlReq);
				pQueryUnifiedFormat->stOrderCond.bUp = GetIsUp(cTemp);
				xmlReq.OutOfElem();               
			}          
		} 
	}
	return IVS_SUCCEED;
}

IVS_INT32 COCXXmlProcess::GetUnifiedFormatQueryParseXMLEx(IVS_VOID* &pReqBuffer,CXml& xmlReq,IVS_INT32 iFiedNum,IVS_CHAR* pDomainCode)
{
    IVS_DEBUG_TRACE("");
    // 分配空间 失败返回IVS_ALLOC_MEMORY_ERROR
    if (IVS_SUCCEED != AlloceQueryStructMemory(pReqBuffer, iFiedNum))
    {
        return IVS_ALLOC_MEMORY_ERROR;
    }    

    // 查找pageinfo 这个是must字段
    if (!xmlReq.FindElemEx("Content/PageInfo"))
    {
        IVS_DELETE(pReqBuffer, MUILI);
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Unified Format Query Parse XML", "can not find Element Path(Content/PageInfo)");
        return IVS_XML_INVALID;
    }
    IVS_QUERY_UNIFIED_FORMAT *pQueryUnifiedFormat = (IVS_QUERY_UNIFIED_FORMAT*)pReqBuffer;
    const IVS_CHAR* szElemValue = NULL;
    IVS_INT32 iFromTemp = 0;
    IVS_INT32 iToTemp = 0;

    // 获取请求的数量完成  从第几条记录开始 v    //截止第几条记
    GET_ELEM_VALUE_NUM("FromIndex", szElemValue, iFromTemp, xmlReq);
    GET_ELEM_VALUE_NUM("ToIndex", szElemValue, iToTemp, xmlReq);
    // sdk在这要对值进行判断
    if (iFromTemp > iToTemp || iFromTemp <= 0 || iToTemp <= 0)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Unified Format Query Parse XML", "FromIndex[%d] and ToIndex[%d] failed", iFromTemp, iToTemp);
        IVS_DELETE(pReqBuffer, MUILI);
        return IVS_XML_INVALID;
    }

    pQueryUnifiedFormat->stIndex.uiFromIndex = static_cast<IVS_UINT32>(iFromTemp);
    pQueryUnifiedFormat->stIndex.uiToIndex = static_cast<IVS_UINT32>(iToTemp);   
    // 获取请求的数量完成

    // 这个是查询条件的个数
    pQueryUnifiedFormat->iFieldNum = iFiedNum;

    if(xmlReq.FindElemEx("Content/DomainCode"))
    {
        GET_ELEM_VALUE_CHAR("DomainCode", szElemValue, pDomainCode, IVS_DOMAIN_CODE_LEN, xmlReq);
    }

    // 查找查询条件元素 这个是O，不是must
    if (xmlReq.FindElemEx("Content/PageInfo/QueryCond"))
    {
       // xmlReq.IntoElem();

        if (xmlReq.FindElem("QueryField"))
        {        
            IVS_CHAR cTemp[IVS_DESCRIBE_LEN+1];
            IVS_INT32 iIndex = 0;
            IVS_QUERY_FIELD stQueryField;
            do 
            {
                if (!xmlReq.FindElemValue("QueryField"))
                {
                    break;
                }
                xmlReq.IntoElem();
                szElemValue = NULL;

                memset(&stQueryField, 0, sizeof(IVS_QUERY_FIELD));

                // 对于字段的值，不知道具体的大小 现在目前先定IVS_DESCRIBE_LEN
                // 查询字段
                memset(cTemp, 0, IVS_DESCRIBE_LEN);
                GET_ELEM_VALUE_CHAR("Field", szElemValue, cTemp, IVS_DESCRIBE_LEN, xmlReq);
                stQueryField.eFieID = QUERYFILED.GetTypeID(cTemp);
                // 查询字段的值
                GET_ELEM_VALUE_CHAR("Value", szElemValue,stQueryField.cValue, IVS_QUERY_VALUE_LEN, xmlReq);
                // 查询类型
                memset(cTemp, 0, IVS_DESCRIBE_LEN);
                GET_ELEM_VALUE_CHAR("QueryType", szElemValue, cTemp, IVS_DEV_CODE_LEN, xmlReq);
                stQueryField.bExactQuery = GetQueryType(cTemp);
                xmlReq.OutOfElem();
                if (!CToolsHelp::Memcpy(&(pQueryUnifiedFormat->stQueryField[iIndex]),sizeof(IVS_QUERY_FIELD),&stQueryField,sizeof(IVS_QUERY_FIELD)))
                {
                    IVS_DELETE(pReqBuffer, MUILI);
                    BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR ,"Get Unified Format Query Parse XML", "Memcpy(QueryField) failed");
                    return IVS_ALLOC_MEMORY_ERROR;
                }
                iIndex++;

            } while (xmlReq.NextElem());          	
            xmlReq.OutOfElem();
            if (xmlReq.FindElem("OrderCond"))
            {
                // 默认的是排序的
                pQueryUnifiedFormat->stOrderCond.bEnableOrder = MATCHED_SUCCEED;
                xmlReq.IntoElem();
                // 排序字段
                memset(cTemp,0,IVS_DESCRIBE_LEN);
                GET_ELEM_VALUE_CHAR("OrderField", szElemValue,cTemp, IVS_DESCRIBE_LEN, xmlReq);
                pQueryUnifiedFormat->stOrderCond.eFieID = QUERYFILED.GetTypeID(cTemp);
                // 升序还是降序
                memset(cTemp, 0, IVS_DESCRIBE_LEN);
                GET_ELEM_VALUE_CHAR("Order", szElemValue, cTemp, IVS_DESCRIBE_LEN, xmlReq);
                pQueryUnifiedFormat->stOrderCond.bUp = GetIsUp(cTemp);
                xmlReq.OutOfElem();               
            }          
        } 
    }
    return IVS_SUCCEED;
}

// 分配通用格式内存大小
IVS_INT32 COCXXmlProcess::AlloceQueryStructMemory( IVS_VOID* &pReqBuffer, IVS_INT32 iFieldNum )
{
	IVS_DEBUG_TRACE("Field Num = %d", iFieldNum);
	// 分配空间，这里不是0的情况下就多分配了一个结构体大小
	IVS_UINT32 iFieldBufferSize = 0;
	if (iFieldNum == 0)
	{
		iFieldBufferSize = sizeof(IVS_QUERY_UNIFIED_FORMAT);
	}
	else
	{
		iFieldBufferSize = sizeof(IVS_QUERY_UNIFIED_FORMAT);
		iFieldBufferSize += sizeof(IVS_QUERY_FIELD)*((IVS_UINT32)(iFieldNum - 1));
	}

	pReqBuffer = IVS_NEW((IVS_CHAR* &)pReqBuffer, iFieldBufferSize);
	
	if (NULL == pReqBuffer)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Alloce Query Struct Memory", "new memory failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(pReqBuffer, 0, iFieldBufferSize);
	return IVS_SUCCEED;
}

// 查询告警级别列表返回的xml
IVS_VOID COCXXmlProcess::GetAlarmErrString( CString &strResult, IVS_INT32 iRet )
{
	IVS_DEBUG_TRACE("");
	CXml xmlRsp;

	(void)xmlRsp.AddDeclaration("1.0","UTF-8","");

	(void)xmlRsp.AddElem("Content");
	(void)xmlRsp.IntoElem();
	(void)xmlRsp.AddChildElem("AlarmLevelList");
	(void)xmlRsp.IntoElem();

	(void)xmlRsp.AddElem("ResultCode");
	(void)xmlRsp.IntoElem();
	(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	xmlRsp.OutOfElem();
	IVS_UINT32 iLen = 0;
	const char* pXml = xmlRsp.GetXMLStream(iLen);
	CHECK_POINTER_VOID(pXml);
	strResult = pXml;
}

// 判断是否为精确
IVS_BOOL COCXXmlProcess::GetQueryType(const IVS_CHAR* pQueryType)
{
	if (NULL==pQueryType)
	{
		return MATCHED_FAIL;
	}
	else if (0==strcmp(pQueryType ,"EXACT")||0==strcmp(pQueryType ,"exact"))
	{
		return MATCHED_SUCCEED;
	}
	else
	{
		return MATCHED_FAIL;
	}
}

// 判断是否为升序
IVS_BOOL COCXXmlProcess::GetIsUp(const IVS_CHAR* pQueryUp)
{
	if (NULL==pQueryUp)
	{
		return MATCHED_FAIL;
	}
	else if ((0==strcmp(pQueryUp,"UP"))||(0==strcmp(pQueryUp,"up")))
	{
		return MATCHED_SUCCEED;
	}
	else
	{
		return MATCHED_FAIL;
	}
}

// 拼装错误码的xml
IVS_VOID COCXXmlProcess::GetErrString( CString &strResult, IVS_INT32 iRet )
{
    IVS_DEBUG_TRACE("");
	CXml xmlRsp;
	if (IVS_SUCCEED != iRet)
	{
        BP_RUN_LOG_ERR(iRet, " COCXXmlProcess::GetErrString", "IVS_SUCCEED != iRet");
		(void)xmlRsp.AddDeclaration("1.0","UTF-8","");

		(void)xmlRsp.AddElem("Content");
		(void)xmlRsp.AddChildElem("ResultCode");
		(void)xmlRsp.IntoElem();
		(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
		xmlRsp.OutOfElem();
		IVS_UINT32 uiLen = 0;
		const char* pXml = xmlRsp.GetXMLStream(uiLen);
		CHECK_POINTER_VOID(pXml);
		strResult = pXml;
	}
}

// 获取查询字段的个数
IVS_INT32 COCXXmlProcess::GetFieldNum(CXml &xmlReq)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iNum = 0;	
	// 看是否有QueryField
	if (xmlReq.FindElemEx("Content/PageInfo/QueryCond"))
	{
		do 
		{
			if (!xmlReq.FindElemValue("QueryField"))
			{
				break;
			}
			iNum++;
		} while (xmlReq.NextElem());           
	}

	return iNum;	
}

void COCXXmlProcess::GetErrorResultXML(IVS_INT32 iRetCord, CXml &xml)
{
	(void)xml.AddDeclaration("1.0","UTF-8","");

	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRetCord).c_str());
	xml.OutOfElem();
}

// 获取指定路径（含节点名称）节点数
IVS_UINT32 COCXXmlProcess::GetElemCount(const IVS_CHAR* pElemPath,IVS_UINT32 &uiNum, CXml &xml)
{
	IVS_DEBUG_TRACE("");
	uiNum = 0;
	if(!xml.FindElemEx(pElemPath))
	{
		return IVS_FAIL;
	}
	//xml.OutOfElem();
	//do 
	//{
	//	++uiNum;
	//}while (xml.NextElem());
	if (xml.FindElemEx(pElemPath))
	{ 
		do 
		{
			uiNum++;
		} while (xml.NextElem());
	}
	return IVS_SUCCEED;
}

// 拼装返回码到xml
IVS_INT32 COCXXmlProcess::GetResultXML(IVS_INT32& iRet, const IVS_CHAR* pRspXml, CString& strResult)
{
	IVS_DEBUG_TRACE("");

	CHECK_POINTER(pRspXml,iRet);
	CXml xmlRsp;
	if (!xmlRsp.Parse(pRspXml))
	{
		BP_RUN_LOG_ERR(iRet, "Get Result XML", "xmlRsp.Parse(pRspXml)");
		return iRet;
	}

	if (!xmlRsp.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Get Result XML", "xmlRsp.FindElem(Content)");
		return IVS_SMU_XML_INVALID;
	}
	if (xmlRsp.IntoElem())
	{
		(void)xmlRsp.AddElem("ResultCode");
	}
	else
	{
		(void)xmlRsp.AddChildElem("ResultCode");
	}
	(void)xmlRsp.IntoElem();
	(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());

	IVS_UINT32 xmlLen = 0;
	const char* pXml = xmlRsp.GetXMLStream(xmlLen);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
	strResult = pXml;

	return IVS_SUCCEED;

}

// 查询设备参数的个数
IVS_UINT32 COCXXmlProcess::GetXmlDevinfoNum( CXml &xml )
{
    IVS_DEBUG_TRACE("");

    IVS_UINT32 uiNum = 0;
    if (xml.FindElemEx("Content/DevList"))
    {
        do 
        {
            if (!xml.FindElemValue("DevInfo"))
            {
                break;
            }
            uiNum++;
        } while (xml.NextElem());

    }   

    xml.OutOfElem();
    xml.OutOfElem(); 

    return uiNum;
}

// 在Content下面加一个resultCode节点
IVS_INT32 COCXXmlProcess::AddResultCodeUnderContent(IVS_INT32 iResultCode, const std::string& strInXml, std::string& strOutXml)
{
	// 构造resultcode的节点. TODO new加上异常处理
	TiXmlElement* pResultCodeElem = NULL;
	try
	{
		pResultCodeElem = new TiXmlElement("ResultCode");
	}
	catch(...)
	{
		BP_RUN_LOG_INF("new TiXmlElement(LoginInfo) is error", "NA");
		pResultCodeElem = NULL;
		return IVS_OPERATE_MEMORY_ERROR;
	}
	TiXmlText* pResultCodeXmlText = NULL;
	try
	{
		pResultCodeXmlText = new TiXmlText(CToolsHelp::Int2Str(iResultCode).c_str());
	}
	catch(...)
	{
		BP_RUN_LOG_INF("new TiXmlText(pLoginID) is error", "NA");
		IVS_DELETE(pResultCodeElem);
		pResultCodeXmlText = NULL;
		return IVS_OPERATE_MEMORY_ERROR;
	}
	pResultCodeElem->LinkEndChild(pResultCodeXmlText);
	TiXmlDocument XmlDoc;
	XmlDoc.Parse(strInXml.c_str(), NULL, TIXML_ENCODING_LEGACY);
	if (0 != XmlDoc.Error())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "parse xml failed", "NA");
		IVS_DELETE(pResultCodeElem);
		return IVS_FAIL;//lint !e429
	}
 	// 查找content节点
 	TiXmlElement* pContentElem = CToolsHelp::FindElementByPath(&XmlDoc,"Content");
	if (NULL == pContentElem)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "content elem not find", "NA");
		IVS_DELETE(pResultCodeElem);
		return IVS_FAIL;//lint !e429
	}

 	// 如果没有子节点 直接返回错
	TiXmlElement* pContentFirstChild = pContentElem->FirstChildElement();
	if (NULL == pContentFirstChild)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "first elem not find", "NA");
		IVS_DELETE(pResultCodeElem);
		return IVS_FAIL;//lint !e429
	}

 	// 在找到的第一个节点之前插入resultcode
	if (NULL == pContentElem->InsertBeforeChild(pContentFirstChild, *pResultCodeElem))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "InsertBeforeChild error", "NA");
		IVS_DELETE(pResultCodeElem);
		return IVS_FAIL;//lint !e429
	}
	// 将拼装好的xml打到Cxml对象里面，保持外部一致
	TiXmlPrinter xmlPrinter;
	xmlPrinter.SetStreamPrinting();		

	if (!XmlDoc.Accept(&xmlPrinter))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "print error", "NA");
		IVS_DELETE(pResultCodeElem);
		return IVS_FAIL;//lint !e429
	}
	strOutXml = xmlPrinter.CStr();
	IVS_DELETE(pResultCodeElem);
	return IVS_SUCCEED;//lint !e429
}

// 解析的Devcode
IVS_INT32 COCXXmlProcess::ParseDevCode(const IVS_CHAR *pDevBigCode, IVS_CHAR *pDevCode, IVS_CHAR* pDomainCode )
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevBigCode, IVS_OPERATE_MEMORY_ERROR);  

	const IVS_INT32 IVS_SDK_DEV_CODE_LEN = 32;
	if (IVS_DEV_CODE_LEN < strlen(pDevBigCode))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Parse Device Code", "IVS_DEV_CODE_LEN != strlen(pDevBigCode)");
		return IVS_PARA_INVALID;
	}

	memset(pDevCode, 0, IVS_SDK_DEV_CODE_LEN+1);
	memset(pDomainCode, 0, IVS_DOMAIN_CODE_LEN+1);

	// 避免常量测试出错
	std::string strDevBigCode = pDevBigCode;
	std::string strDevCode = strDevBigCode.substr(0,strDevBigCode.rfind("#"));
	std::string strDomainCode = strDevBigCode.substr(strDevBigCode.rfind("#") + 1, strlen(strDevBigCode.c_str()));

	if (!CToolsHelp::Strncpy(pDevCode, IVS_SDK_DEV_CODE_LEN+1, strDevCode.c_str(), IVS_SDK_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR ,"Parse Device Code", "Strncpy(DevCode) failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	if (!CToolsHelp::Strncpy(pDomainCode, IVS_DOMAIN_CODE_LEN+1, strDomainCode.c_str(), IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR ,"Parse Device Code", "Strncpy(DomainCode) failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	return IVS_SUCCEED;
}

// 转换int类型到BOOL
bool COCXXmlProcess::TransIntToBool(unsigned int iRet)
{
	if(0 == iRet)
		return false;
	else
		return true;
}

// 转换0,1到字串ON,OFF
std::string COCXXmlProcess::TransIntToOnOff(unsigned int iRet)
{
	if(OFF_LINE == iRet)
		return "OFF";
	else if(ON_LINE == iRet)
		return "ON";
	else
		return "";
}

// 字符串转换到long
long COCXXmlProcess::TransStrToLong(const std::string& str)
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

// 转换ON,OFF到0,1的
unsigned int COCXXmlProcess::TransOnOffToInt(const std::string &str)
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

// 字符串0,1到数字0,1的转换
int COCXXmlProcess::TransStrToInt(const std::string& str)
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

// 定义字符数组到字串的转换，主要用于数据结构中字符数字的转换
void COCXXmlProcess::SetElemValueStr(const IVS_CHAR* pXmlValue,IVS_UINT32 ulLength,CXml &xml)
{
	CHECK_POINTER_VOID(pXmlValue);
	IVS_CHAR* pXmlNewValue = IVS_NEW((IVS_CHAR* &)pXmlNewValue,ulLength+1);
	CHECK_POINTER_VOID(pXmlNewValue);
	memset(pXmlNewValue,0,ulLength+1);
	(void)CToolsHelp::Memcpy(pXmlNewValue, ulLength+1, pXmlValue, ulLength);
	(void)xml.SetElemValue(pXmlNewValue);
	IVS_DELETE(pXmlNewValue,MUILI);
}

// 定义将十进制的数字转换为二进制字串
std::string COCXXmlProcess::TransDecToBinStr(unsigned int ulValue)
{
	CString str;
	char ch;
	int bits = 0;
	frexp((float)ulValue,&bits);
	str.Empty();
	for (int i=0;i<bits;i++)
	{
		if (1==((ulValue>>((bits-1)-i))&0x01))
		{
			ch = '1';
		}
		else
		{
			ch = '0';
		}
		str += ch;
	}

	std::string strtemp(str.GetBuffer());  //lint !e1036没有风险，不需要
	str.ReleaseBuffer();
	return strtemp;//lint !e1036没有风险，不需要
}

//拼装OCX返回事件的XML
void COCXXmlProcess::GetEventXML(CXml &xml,const std::string &strEvent,ULONG ulWndID,const std::string &strStatus)
{
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("MsgType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(strEvent.c_str());
	(void)xml.AddElem("WndID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(ulWndID)).c_str());
	if (strStatus.size()>0)
	{
		(void)xml.AddElem("Status");
		(void)xml.SetElemValue(strStatus.c_str());
	}
	xml.OutOfElem();
}

void COCXXmlProcess::GetTalkbackXML(CString &strResult, IVS_INT32 iRet, IVS_ULONG ulHandle)
{
    IVS_DEBUG_TRACE("");
    CXml xmlRsp;

    BP_RUN_LOG_ERR(iRet, " COCXXmlProcess::GetTalkbackXML", "IVS_SUCCEED != iRet");
    (void)xmlRsp.AddDeclaration("1.0","UTF-8","");

    (void)xmlRsp.AddElem("Content");

    (void)xmlRsp.AddChildElem("ResultCode");
    (void)xmlRsp.IntoElem();
    (void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());

    (void)xmlRsp.AddElem("TalkbackHandle");
    (void)xmlRsp.IntoElem();
    (void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(ulHandle).c_str());//lint !e713

    xmlRsp.OutOfElem();
    IVS_UINT32 uiLen = 0;
    const char* pXml = xmlRsp.GetXMLStream(uiLen);
    CHECK_POINTER_VOID(pXml);
    strResult = pXml;
}

// 拼装返回码到xml
IVS_INT32 COCXXmlProcess::BuildShareCatalogResultXml(IVS_INT32 iRet, const IVS_CHAR* pRspXml, CString& strResult)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
	CXml xmlRsp;
	if (!xmlRsp.Parse(pRspXml))
	{
		BP_RUN_LOG_ERR(iRet, "Build Share Catalog Result XML Fail, ", "xmlRsp.Parse(pRspXml)");
		return IVS_XML_INVALID;
	}

	if (!xmlRsp.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Build Share Catalog Result XML Fail, ", "xmlRsp.FindElem(Content)");
		return IVS_XML_INVALID;
	}

	if (xmlRsp.IntoElem())
	{
		(void)xmlRsp.AddElem("ResultCode");
	}
	else
	{
		(void)xmlRsp.AddChildElem("ResultCode");
	}
	(void)xmlRsp.IntoElem();
	(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());

	//将DevCode和DevDomainCode合并到DevCode中
	if (xmlRsp.FindElemEx("Content/DevList"))
	{
		do 
		{
			const char* pDevCode = NULL;
			const char* pDevDomainCode = NULL;
			if (xmlRsp.FindElemValue("DevInfo"))
			{
				(void)xmlRsp.IntoElem();

				std::string strDevCode;
				if (xmlRsp.FindElem("DevDomainCode"))
				{
					pDevDomainCode = xmlRsp.GetElemValue();
				}

				if (xmlRsp.FindElem("DevCode"))
				{
					pDevCode = xmlRsp.GetElemValue();
				}

				if ((NULL == pDevDomainCode) || (NULL == pDevCode))
				{
					BP_RUN_LOG_ERR(IVS_XML_INVALID, "Build Share Catalog Result XML Fail, ", "DevCode or DevDomainCode Node is No Exist.");
					return IVS_XML_INVALID;
				}

				strDevCode.append(pDevCode).append("#").append(pDevDomainCode);	
				xmlRsp.ModifyElemValue(strDevCode.c_str());
			}
			xmlRsp.OutOfElem();

		} while (xmlRsp.NextElem());
	}   

	IVS_UINT32 xmlLen = 0;
	const char* pXml = xmlRsp.GetXMLStream(xmlLen);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
	strResult = pXml;

	return IVS_SUCCEED;
}

