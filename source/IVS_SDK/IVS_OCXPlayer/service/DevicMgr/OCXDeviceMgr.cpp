/*****************************************************************
 filename    :    OCXDeviceMgr.cpp
 author      :    guandiqun
 created     :    2012/11/19
 description :    实现设备;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*****************************************************************/
#include "OCXDeviceMgr.h"
#include "ivs_error.h"
#include "OCXDevicMgrXMLProcess.h"
#include "IVS_SDK.h"
#include "IVSCommon.h"
#include "OCXXmlProcess.h"
#include "IVS_Trace.h"
#include "OCXAlarmMgrXMLProcess.h"

// 新增设备
IVS_INT32 COCXDeviceMgr::AddDevice(IVS_INT32 iSessionID, CXml& xmlReq, CXml& xmlRsp)
{ 
	IVS_DEBUG_TRACE("");

    IVS_UINT32 uiNum = COCXXmlProcess::GetXmlDevinfoNum(xmlReq);
    //查看有多少个info 如果数量为0，那么就返回IVS_SMU_XML_NODE_VALUE_NOT_EXIST
    if (0 == uiNum)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Add Device fail", "iNum == 0");
        return IVS_XML_INVALID;
    }  

    IVS_UINT32 iReqBufferSize = sizeof(IVS_DEVICE_OPER_INFO)*(uiNum);
    IVS_VOID* pReqData = IVS_NEW((IVS_CHAR*&)pReqData, iReqBufferSize); 
    if (NULL == pReqData)
    {
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS_NEW((IVS_CHAR*&)pReqData, iReqBufferSize) fail", "NA");
        return IVS_ALLOC_MEMORY_ERROR;       
    }
    memset(pReqData, 0, iReqBufferSize);

    IVS_CHAR chDomainCode[IVS_DOMAIN_CODE_LEN + 1] ;  
    memset(chDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);

    //解析xml保存到pReqData中
    IVS_INT32 iRet = COCXDeviceXMLProcess::AddDeviceParseXml(xmlReq, pReqData, uiNum, chDomainCode);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Add Device ParseXml fail", "IVS_SUCCEED != iRet");
        IVS_DELETE(pReqData, MUILI);     
        return iRet;
    } 

    IVS_UINT32 iRspBufferSize = sizeof(IVS_DEVICE_OPER_RESULT_LIST) + sizeof(IVS_DEVICE_OPER_RESULT) * ((uiNum-1));  

    IVS_VOID* pRspData = IVS_NEW((IVS_CHAR*&)pRspData, iRspBufferSize);
    if (NULL == pRspData)
    {   
        IVS_DELETE(pReqData, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS_NEW((IVS_CHAR*&)pRspData, iRspBufferSize) fail", "NA");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    memset(pRspData, 0, iRspBufferSize);    
    //转换成IVS_DEVICE_OPER_INFO*
    IVS_DEVICE_OPER_INFO* pDevList = (IVS_DEVICE_OPER_INFO*)pReqData;    
    //转换成pResultList*
    IVS_DEVICE_OPER_RESULT_LIST* pResultList = (IVS_DEVICE_OPER_RESULT_LIST*)pRspData;

    iRet = IVS_SDK_AddDevice(iSessionID, chDomainCode, uiNum, pDevList, pResultList, iRspBufferSize);
    if (IVS_SUCCEED != iRet)
    {  
        BP_RUN_LOG_ERR(iRet, "SDK_AddDevice fail", "NA");
		IVS_DELETE(pReqData, MUILI);  
		IVS_DELETE(pRspData, MUILI); 
		return iRet;
    }      
	//从pRspData中组装xml
	iRet = COCXDeviceXMLProcess::AddDeviceGetXml( xmlRsp, pRspData, uiNum , chDomainCode);    
	IVS_DELETE(pReqData, MUILI);  
	IVS_DELETE(pRspData, MUILI);  
    // TODO: Add your dispatch handler code here

    return iRet;
}

// 查询设备列表（可以设置查询条件）
IVS_INT32 COCXDeviceMgr::QueryDeviceList( IVS_INT32 iSessionID, IVS_ULONG ulDeviceType, CXml& reqXml, CXml& xmlRsp )
{
    IVS_DEBUG_TRACE("");

    // 查询条件个数，无查询条件为0
    IVS_INT32 iFieldNum = COCXXmlProcess::GetFieldNum(reqXml); 

    IVS_VOID *pReqBuffer = NULL;
    //调用通用的方法 会对请求的数量进行判断
    IVS_INT32 iRet = COCXXmlProcess::GetUnifiedFormatQueryParseXML(pReqBuffer, reqXml, iFieldNum);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get UnifiedFormatQuery ParseXML fail", "IVS_SUCCEED != iRet");
        IVS_DELETE(pReqBuffer, MUILI);
        return iRet;
    }

    IVS_QUERY_UNIFIED_FORMAT *pQueryFormat = (IVS_QUERY_UNIFIED_FORMAT*)pReqBuffer;
    IVS_UINT32 uiReqNum = (pQueryFormat->stIndex.uiToIndex - pQueryFormat->stIndex.uiFromIndex) + 1;  

    //定义域，保存域的
    IVS_CHAR chDomainCode[IVS_DOMAIN_CODE_LEN + 1];
    memset(chDomainCode, 0, IVS_DOMAIN_CODE_LEN + 1);
    const IVS_CHAR *pTemp = NULL;
	if (!reqXml.FindElemEx("Content/DomainCode"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "domain code is empty", "NA");
		IVS_DELETE(pReqBuffer, MUILI);
		return IVS_XML_INVALID;
	}
    GET_ELEM_VALUE_CHAR("DomainCode", pTemp, chDomainCode, IVS_DOMAIN_CODE_LEN, reqXml);

    //分配返回来的数据的空间
    IVS_UINT32 uiRspBufferSize = 0;
    IVS_VOID *pRspBuffer = GetListObject((IVS_INT32)ulDeviceType, uiReqNum, uiRspBufferSize);    
    if (NULL == pRspBuffer)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "'new pRspBuffer' fail", "NULL == pRspBuffer");       
        IVS_DELETE(pReqBuffer, MUILI);
        return IVS_ALLOC_MEMORY_ERROR;
    }  

    //获取list
    iRet = IVS_SDK_QueryDeviceList(iSessionID,chDomainCode,ulDeviceType, pQueryFormat, pRspBuffer, uiRspBufferSize);
    //根据iRet组装xml然后返回给cu
    if (IVS_SUCCEED != iRet)
    {        
        BP_RUN_LOG_ERR(iRet, "SDK_QueryDeviceList fail", "NA");
		IVS_DELETE(pRspBuffer, MUILI);
		IVS_DELETE(pReqBuffer, MUILI);
		return iRet;
    }

    iRet = COCXDeviceXMLProcess::QueryDeviceListGetXml(pRspBuffer, chDomainCode, ulDeviceType, xmlRsp); 
    IVS_DELETE(pRspBuffer, MUILI);
    IVS_DELETE(pReqBuffer, MUILI);    

    return iRet;
}

// 获取设备列表(无查询条件设置)
IVS_INT32 COCXDeviceMgr::GetDeviceList( IVS_INT32 iSessionID, 
	IVS_ULONG ulDeviceType, 
	IVS_ULONG ulFromIndex, 
	IVS_ULONG ulToIndex,
	CXml& xmlRsp)
{
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;

	// 分配返回来的数据的空间
	IVS_UINT32 uiReqNum = (ulToIndex - ulFromIndex) + 1; 
	IVS_UINT32 uiRspBufferSize = 0;
	IVS_VOID *pRspBuffer = GetListObject((IVS_INT32)ulDeviceType, uiReqNum, uiRspBufferSize);    
	if (NULL == pRspBuffer)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "'new pRspBuffer' fail", "NULL == pRspBuffer");       
		return IVS_ALLOC_MEMORY_ERROR;
	}

	// 获取list
	IVS_INDEX_RANGE stRange;
	stRange.uiToIndex = ulToIndex;
	stRange.uiFromIndex = ulFromIndex;
	iRet = IVS_SDK_GetDeviceList(iSessionID,(IVS_UINT32)ulDeviceType,&stRange,pRspBuffer,uiRspBufferSize);
	if (IVS_SUCCEED != iRet)
	{        
		 BP_RUN_LOG_ERR(iRet, "SDK_GetDeviceList fail", "NA");
		 IVS_DELETE(pRspBuffer, MUILI);
		 return iRet;
	}
	//根据iRet组装xml然后返回给cu
	iRet = COCXDeviceXMLProcess::GetDeviceListGetXml(pRspBuffer, ulDeviceType, xmlRsp);
	IVS_DELETE(pRspBuffer, MUILI);

	return iRet;
}

//统一分配内存
IVS_VOID* COCXDeviceMgr::GetListObject( IVS_INT32 lDeviceType , IVS_UINT32 uiNum, IVS_UINT32 &uiSizeBuff)
{
	IVS_DEBUG_TRACE("");

    switch(lDeviceType)
    {
    case DEVICE_TYPE_MAIN:
        {            
            uiSizeBuff = sizeof(IVS_DEVICE_BRIEF_INFO_LIST) + (uiNum-1)*sizeof(IVS_DEVICE_BRIEF_INFO);
            break;
        }
    case DEVICE_TYPE_CAMERA:
        {            
            uiSizeBuff = sizeof(IVS_CAMERA_BRIEF_INFO_LIST) + (uiNum-1)*sizeof(IVS_CAMERA_BRIEF_INFO);
            break;
        }
    case DEVICE_TYPE_VOICE:
        {            
            uiSizeBuff = sizeof(IVS_AUDIO_BRIEF_INFO_LIST) + (uiNum-1)*sizeof(IVS_AUDIO_BRIEF_INFO);
            break;
        }
    case DEVICE_TYPE_ALARM:
	case DEVICE_TYPE_EXTEND_ALARM:
        {            
            uiSizeBuff = sizeof(IVS_ALARM_BRIEF_INFO_LIST) + (uiNum-1)*sizeof(IVS_ALARM_BRIEF_INFO);
            break;
        }
    case DEVICE_TYPE_SERIAL:
    default:
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get ListObject fail", "NA");
        return NULL;
    }

    IVS_CHAR *pBuff = IVS_NEW(pBuff, uiSizeBuff);
    if (NULL != pBuff)
    {
        memset(pBuff, 0, uiSizeBuff);
    }

    return (IVS_VOID*)pBuff;
}

// 获取域路由
IVS_VOID COCXDeviceMgr::GetDomainRoute(IVS_INT32 iSessionID, CString &strResult)
{
	IVS_DEBUG_TRACE("");

    IVS_UINT32 uiMaxNum = IVS_MAX_DOMAIN_ROUTE_NUM * 8;
    IVS_UINT32 uiBufferSize = sizeof(IVS_DOMAIN_ROUTE_LIST) + sizeof(IVS_DOMAIN_ROUTE)*(uiMaxNum - 1);
    IVS_VOID *pReqRoute = IVS_NEW((IVS_CHAR*&)pReqRoute, uiBufferSize);
    if (NULL == pReqRoute)
    {
        COCXXmlProcess::GetErrString(strResult, IVS_ALLOC_MEMORY_ERROR);
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "'new pReqRoute' fail", "NULL == pReqRoute");
        return;
    }
	memset(pReqRoute, 0, uiBufferSize);
    IVS_DOMAIN_ROUTE_LIST *pList = reinterpret_cast<IVS_DOMAIN_ROUTE_LIST *>(pReqRoute);

    IVS_INT32 iRet = IVS_SDK_GetDomainRoute(iSessionID, pList, uiBufferSize);
    if (IVS_SUCCEED != iRet)
    {
        COCXXmlProcess::GetErrString(strResult, iRet);
        BP_RUN_LOG_ERR(iRet, "SDK_GetDomainRoute fail", "NULL == pReqRoute");
        IVS_DELETE(pReqRoute, MUILI);
        return ;
    }

    CXml xmlRsp;
    iRet = COCXDeviceXMLProcess::GetDomainRouteGetXML(pList, xmlRsp);
    if (IVS_SUCCEED != iRet)
    {
        COCXXmlProcess::GetErrString(strResult, iRet);
        BP_RUN_LOG_ERR(iRet, "Get DomainRoute GetXML fail", "IVS_SUCCEED != iRet");        
    }
    else
    {
        IVS_UINT32 uiLen = 0;
        strResult = xmlRsp.GetXMLStream(uiLen);
    }
    IVS_DELETE(pReqRoute, MUILI);

	return;
}

// 查询主设备详细信息
IVS_VOID COCXDeviceMgr::GetDeviceInfo( IVS_INT32 iSessionID, CXml& xmlReq, CString &strResult )
{
    IVS_DEBUG_TRACE("");

    IVS_CHAR szDomainCode[IVS_DEV_CODE_LEN+1];
    memset(szDomainCode, 0, IVS_DEV_CODE_LEN+1);
    IVS_CHAR szDevCode[IVS_DEV_CODE_LEN+1];
    memset(szDevCode, 0, IVS_DEV_CODE_LEN+1);

    // 解析xml
    IVS_INT32 iRet = COCXDeviceXMLProcess::GetDeviceInfoParseXML(szDomainCode, szDevCode, xmlReq);
    BP_RUN_LOG_INF("NA", "Get DeviceInfo ParseXML iRet=%d", iRet);
    if (IVS_SUCCEED == iRet)
    {
        IVS_DEVICE_INFO stDeviceInfo;
        memset(&stDeviceInfo, 0, sizeof(stDeviceInfo));
        // 调用SDK接口
        iRet = IVS_SDK_GetDeviceInfo(iSessionID, szDomainCode, szDevCode, &stDeviceInfo);
        BP_RUN_LOG_INF("NA", "SDK_GetDeviceInfo iRet=%d", iRet);
        if (IVS_SUCCEED == iRet)
        {
            CXml xmlRsp;
            iRet = COCXDeviceXMLProcess::GetDeviceInfoXML(&stDeviceInfo, xmlRsp);  
            IVS_UINT32 iLen = 0;
            strResult = xmlRsp.GetXMLStream(iLen);
        }        
    }

    if (IVS_SUCCEED != iRet)
    {
        COCXXmlProcess::GetErrString(strResult, iRet);  
    }         

	return;
}

// 查询NVR列表
IVS_VOID COCXDeviceMgr::GetNVRList(IVS_INT32 iSessionID, CXml& xmlReq, CString &strResult)
{
	IVS_DEBUG_TRACE("");

    IVS_CHAR cDomainCode[IVS_DOMAIN_LEN + 1];
    memset(cDomainCode,0,IVS_DOMAIN_LEN + 1);
    IVS_INDEX_RANGE stIndex;
    memset(&stIndex,0,sizeof(IVS_INDEX_RANGE));
    IVS_UINT32 uiServerType = 0;
    if(IVS_SUCCEED != COCXDeviceXMLProcess::ParseGetNVRListReq(xmlReq,cDomainCode,stIndex,uiServerType))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse GetNVRListReq fail", "ErrorCode =IVS_XML_INVALID");
        COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
        return ;
    }

    //为结果LIST分配内存
    IVS_UINT32 uiBufSize = sizeof(IVS_DEVICE_BRIEF_INFO_LIST) + (stIndex.uiToIndex-stIndex.uiFromIndex) * sizeof(IVS_DEVICE_BRIEF_INFO);
    IVS_VOID* pNVRListBuf = IVS_NEW((IVS_CHAR*&)pNVRListBuf, uiBufSize);
    if (NULL == pNVRListBuf)
    {       
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "'new pNVRListBuf' fail", "ErrorCode =IVS_ALLOC_MEMORY_ERROR");
        COCXXmlProcess::GetErrString(strResult, IVS_ALLOC_MEMORY_ERROR);
        return ;
    }
    memset(pNVRListBuf, 0, uiBufSize);

    IVS_DEVICE_BRIEF_INFO_LIST* pNVRList = reinterpret_cast<IVS_DEVICE_BRIEF_INFO_LIST*>(pNVRListBuf);
    IVS_INT32 iRet = IVS_SDK_GetNVRList(iSessionID,cDomainCode,uiServerType,&stIndex,pNVRList,uiBufSize);
    if (iRet!= IVS_SUCCEED)
    {
        IVS_DELETE(pNVRListBuf, MUILI);
        BP_RUN_LOG_ERR(iRet, "SDK_GetNVRList fail", "iRet!= IVS_SUCCEED");
        COCXXmlProcess::GetErrString(strResult, iRet);
        return;
    }

    CXml xmlRsp;
    //拼装响应消息结构体为XML，
    if (IVS_SUCCEED != COCXDeviceXMLProcess::GetNVRListRsp(pNVRList,iRet,xmlRsp))
    {
        IVS_DELETE(pNVRListBuf, MUILI);
        BP_RUN_LOG_ERR(iRet, "IVS_SUCCEED != GetNVR ListRsp(pNVRList,iRet,xmlRsp)", "NA");
        COCXXmlProcess::GetErrString(strResult, iRet);
        return;
    }
    IVS_UINT32 uiXmlLen = 0;
    IVS_DELETE(pNVRListBuf, MUILI);
    strResult = xmlRsp.GetXMLStream(uiXmlLen);

	return;
}

// 根据主设备查询子设备列表
IVS_VOID COCXDeviceMgr::GetChannelList( IVS_INT32 iSessionID, const IVS_CHAR *pDevCode,  CString &strResult)
{
	if (NULL == pDevCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "pDevCode fail", "GetChannelList fail");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}

    IVS_DEBUG_TRACE("");    
	
    IVS_UINT32 uiBuffSize = sizeof(IVS_DEV_CHANNEL_BRIEF_INFO_LIST) + sizeof(IVS_DEV_CHANNEL_BRIEF_INFO)*(IVS_MAX_DEVICE_CHANNEL_NUM - 1);
    IVS_VOID *pBuffer = IVS_NEW((IVS_CHAR* &)pBuffer, uiBuffSize);
    if (NULL == pBuffer)
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "'new pBuffer' fail", "NULL == pBuffer");
        COCXXmlProcess::GetErrString(strResult, IVS_ALLOC_MEMORY_ERROR);
        return;
    }
    memset(pBuffer, 0, uiBuffSize);
	IVS_DEV_CHANNEL_BRIEF_INFO_LIST* pList = reinterpret_cast<IVS_DEV_CHANNEL_BRIEF_INFO_LIST*>(pBuffer);
    pList->stIndexRange.uiFromIndex = 1;
    pList->stIndexRange.uiToIndex = IVS_MAX_DEVICE_CHANNEL_NUM;
	IVS_CHAR chDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN + 1] = {0};

	(void)COCXXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomainCode);

    IVS_INT32 iRet = IVS_SDK_GetChannelList(iSessionID, pDevCode, pList, uiBuffSize);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "SDK_GetChannelList fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		IVS_DELETE(pBuffer, MUILI);
		return;
    }

	CXml xmlRsp;
	iRet = COCXDeviceXMLProcess::GetChannelListGetXML(pList, xmlRsp, chDomainCode);
    IVS_DELETE(pBuffer, MUILI);
    if (IVS_SUCCEED == iRet)
    {
        IVS_UINT32 uiLen = 0;
        strResult = xmlRsp.GetXMLStream(uiLen);
    } 
    else
    {
        BP_RUN_LOG_ERR(iRet, "Get ChannelList GetXML fail", "iRet = %d", iRet);
        COCXXmlProcess::GetErrString(strResult, iRet);
    }

	return;
}

// 修改主设备名称
IVS_INT32 COCXDeviceMgr::SetMainDeviceName(IVS_INT32 iSessionID,const IVS_CHAR *pDevCode,const IVS_CHAR *pNewDevName)
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNewDevName, IVS_OPERATE_MEMORY_ERROR);

	IVS_DEBUG_TRACE("");
	
	IVS_INT32 iRet = IVS_SDK_SetMainDeviceName(iSessionID,pDevCode,pNewDevName);

	return iRet;
}

// 修改摄像机名称
IVS_INT32 COCXDeviceMgr::SetCameraName(IVS_INT32 iSessionID,const IVS_CHAR *pCameraCode,const IVS_CHAR *pNewCameraName)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNewCameraName, IVS_OPERATE_MEMORY_ERROR);

	IVS_DEBUG_TRACE("");
	
	IVS_INT32 iRet = IVS_SDK_SetCameraName(iSessionID,pCameraCode,pNewCameraName);

	return iRet;
}

// 删除设备
IVS_VOID COCXDeviceMgr::DeleteDevice(IVS_INT32 iSessionID, CString &strResult, LPCTSTR pReqXml)
{
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "NULL == pReqGetList", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_OPERATE_MEMORY_ERROR);
		return;
	}	
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "false==xmlReq.Parse(pReqXml)", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}

	IVS_UINT32 uiNum = (IVS_UINT32)COCXDeviceXMLProcess::GetDevCodeNum(xmlReq);
	if (0 == uiNum)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetDevCodeNum's method return 0", "iNum=0");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}

	IVS_UINT32 uiReqBufferSize = sizeof(IVS_DEVICE_CODE) * uiNum;

	//分配内存
	IVS_VOID *pReqBuff = IVS_NEW((IVS_CHAR* &)pReqBuff, uiReqBufferSize);
	if (NULL == pReqBuff)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "'new pReqBuff' fail", "alloc memory error");
		COCXXmlProcess::GetErrString(strResult, IVS_ALLOC_MEMORY_ERROR);
		return;
	}	
	memset(pReqBuff, 0, uiReqBufferSize);

	//分配空间
	IVS_CHAR chDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_DEVICE_CODE* pDevCode = reinterpret_cast<IVS_DEVICE_CODE*>(pReqBuff);
	IVS_INT32 iRet = COCXDeviceXMLProcess::DeleteDevicePraseXML(pDevCode, chDomainCode, uiNum, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "DeleteDevice PraseXML fail", "IVS_SUCCEED != iRet");
		COCXXmlProcess::GetErrString(strResult, iRet);
		IVS_DELETE(pReqBuff, MUILI);
		return ; 
	}

	//计算buffersize
	IVS_UINT32 uiBufSize = ((uiNum - 1) * sizeof(IVS_RESULT_INFO)) + sizeof(IVS_RESULT_LIST);

	IVS_VOID *pBuffer = IVS_NEW((IVS_CHAR* &)pBuffer, uiBufSize);
	memset(pBuffer, 0, uiBufSize);
	IVS_RESULT_LIST* pReqList = reinterpret_cast<IVS_RESULT_LIST*>(pBuffer);
	iRet = IVS_SDK_DeleteDevice(iSessionID, chDomainCode, uiNum, pDevCode, pReqList, uiBufSize);
	if (IVS_SUCCEED == iRet)
	{		
		CXml xmlRsp;
		//从pRspData中组装xml
		iRet = COCXDeviceXMLProcess::DeleteDeviceGetXml(pReqList, xmlRsp, chDomainCode);
		if (IVS_SUCCEED == iRet)
		{
			IVS_UINT32 iLen = 0;
			strResult = xmlRsp.GetXMLStream(iLen);//lint !e64
		}
		else
		{
			COCXXmlProcess::GetErrString(strResult, iRet);
		}
	}	
	else
	{
		BP_RUN_LOG_ERR(iRet, "SDK_DeleteDevice fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
	}
	IVS_DELETE(pReqBuff, MUILI);
	IVS_DELETE(pBuffer, MUILI);

	return;
}

// 重启前端设备
IVS_INT32 COCXDeviceMgr::RestartDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDevCode, IVS_UINT32 uiRebootType)
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_SDK_RebootDevice(iSessionID, pDevCode, uiRebootType);

	return iRet;
}

// 启动搜索前端设备
IVS_INT32 COCXDeviceMgr::StartSearchDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pSearchInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_SDK_StartSearchDevice(iSessionID, pDomainCode, pSearchInfo);

	return iRet;
}

// 停止搜索前端设备
IVS_INT32 COCXDeviceMgr::StopSearchDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pSearchInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_SDK_StopSearchDevice(iSessionID, pDomainCode, pSearchInfo);

	return iRet;
}

// 验证前端设备
IVS_VOID COCXDeviceMgr::VerifyDevice(IVS_INT32 iSessionID, const IVS_CHAR* pDomainCode, const IVS_CHAR* pVerifyInfo, CString &strResult)
{
	if (NULL == pDomainCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "pDomainCode = NULL", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
		return;
	}
	if (NULL == pVerifyInfo)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "pVerifyInfo = NULL", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRspInfo = NULL;
	IVS_INT32 iRet = IVS_SDK_VerifyDevice(iSessionID, pDomainCode, pVerifyInfo, &pRspInfo);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "VerifyDevice SDK Interface fail", "NA");
		if (NULL != pRspInfo)
		{
			(void)IVS_SDK_ReleaseBuffer(pRspInfo);
		}
		COCXXmlProcess::GetErrString(strResult, iRet);
		return; 
	}
	// 拼接Resultcode放在报文前面
	iRet = COCXAlarmMgrXMLProcess::AddResultCodeUp(pRspInfo, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddResultCode To RspXML fail", "NA");
		(void)IVS_SDK_ReleaseBuffer(pRspInfo);
		COCXXmlProcess::GetErrString(strResult, iRet);
		return; 
	}
	(void)IVS_SDK_ReleaseBuffer(pRspInfo);

	return;
}

// 查询前端日志
IVS_VOID COCXDeviceMgr::GetDevLog(IVS_INT32 iSessionID, const IVS_CHAR* pQueryXml, CString &strResult)
{
	if (NULL == pQueryXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "pQueryXml = NULL", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return;
	}
	IVS_DEBUG_TRACE("");

	IVS_CHAR* pRspInfo = NULL;
	IVS_INT32 iRet = IVS_SDK_GetDevLog(iSessionID, pQueryXml, &pRspInfo);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get DevLog SDK Interface fail", "NA");
		if (NULL != pRspInfo)
		{
			(void)IVS_SDK_ReleaseBuffer(pRspInfo);
		}
		COCXXmlProcess::GetErrString(strResult, iRet);
		return; 
	}
	// 拼接Resultcode放在报文前面
	iRet = COCXAlarmMgrXMLProcess::AddResultCodeUp(pRspInfo, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddResultCode To RspXML fail", "NA");
		(void)IVS_SDK_ReleaseBuffer(pRspInfo);
		COCXXmlProcess::GetErrString(strResult, iRet);
		return; 
	}
	(void)IVS_SDK_ReleaseBuffer(pRspInfo);
	
	return;
}

// 查询摄像机码流能力
IVS_VOID COCXDeviceMgr::GetDeviceCapability(IVS_INT32 iSessionID, IVS_UINT32 uiCapabilityType, const IVS_CHAR* pDevCode, CString &strResult)
{
	if (NULL == pDevCode)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "pDevCode = NULL", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_OPERATE_MEMORY_ERROR);
		return;
	}
	IVS_DEBUG_TRACE("");

	IVS_CHAR *pRspXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetDeviceCapability(iSessionID, uiCapabilityType, pDevCode, &pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS_SDK_GetDeviceCapability fail", "NA");
		if (NULL != pRspXml)
		{
			(void)IVS_SDK_ReleaseBuffer(pRspXml);
		}
		COCXXmlProcess::GetErrString(strResult, iRet);
		return;
	}
	// 拼接Resultcode放在报文前面
	iRet = COCXAlarmMgrXMLProcess::AddResultCodeUp(pRspXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "AddResultCode To RspXML fail", "NA");
		(void)IVS_SDK_ReleaseBuffer(pRspXml);
		COCXXmlProcess::GetErrString(strResult, iRet);
		return; 
	}
	(void)IVS_SDK_ReleaseBuffer(pRspXml);

	return;
}

// 设置前端设备密码
IVS_INT32 COCXDeviceMgr::SetDevPWD(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml)
{
	CHECK_POINTER(pReqXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "xmlReq.Parse(pReqXml) fail", "pReqXml fail");
		return IVS_PARA_INVALID;
	}
	IVS_CHAR cDevCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR cDevUserName[IVS_NAME_LEN + 1] = {0};
	IVS_CHAR cDevPWD[IVS_PWD_LEN + 1] = {0};
	IVS_CHAR cDevRegPWD[IVS_PWD_LEN + 1] = {0};
	IVS_INT32 iRet = COCXDeviceXMLProcess::SetDevPWDParseXML(cDevCode, cDevUserName, cDevPWD, cDevRegPWD, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set DevPWD ParseXML fail", "NA");
		return iRet;
	}
	iRet = IVS_SDK_SetDevPWD(iSessionID, cDevCode, cDevUserName, cDevPWD, cDevRegPWD);

	return iRet;
}

// 刷新摄像机列表，同步下级域摄像机列表
IVS_INT32 COCXDeviceMgr::RefreshCameraList(IVS_INT32 iSessionID)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_SDK_RefreshCameraList(iSessionID);

    return iRet;
}

//获取前端Onvif设备编码参数XML
IVS_INT32 COCXDeviceMgr::GetCameraEncodeCapabilities(IVS_STREAM_INFO_LIST *pStreamInfoList,IVS_INT32 iRetCord,CXml &xml)
{
	CHECK_POINTER(pStreamInfoList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	bool bRet = false;

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRetCord).c_str());
	(void)xml.AddElem("StreamInfoList");
	for (int iIndex = 0;iIndex<pStreamInfoList->iRealNum;iIndex++)
	{
		IVS_ST_STREAM_INFO &stStreamInfo = pStreamInfoList->stStreamInfo[iIndex];
		iIndex == 0 ? (void)xml.AddChildElem("StreamInfo"):(void)xml.AddElem("StreamInfo");
		(void)(xml.IntoElem());

		(void)xml.AddChildElem("StreamType");
		(void)(xml.IntoElem());

		IVS_CHAR cStreamType[IVS_FILE_NAME_LEN+1];
		memset(cStreamType,0,IVS_FILE_NAME_LEN+1);
		bRet = CToolsHelp::Memcpy(cStreamType,IVS_FILE_NAME_LEN,stStreamInfo.cStreamType,IVS_FILE_NAME_LEN);
		if(false == bRet)
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Camera Encode Capabilities List Get XML", "Memcpy error.");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cStreamType);

		(void)xml.AddElem("EncodeModeList");
		for (int iEncodeModeIndex = 0;iEncodeModeIndex < stStreamInfo.iRealNum;iEncodeModeIndex++)
		{
			IVS_ENCODEMODE_INFO &stEncodeModeInfo = stStreamInfo.stEncodeMode[iEncodeModeIndex];
			iEncodeModeIndex == 0 ? (void)xml.AddChildElem("EncodeModeInfo"):(void)xml.AddElem("EncodeModeInfo");
			(void)(xml.IntoElem());
			(void)xml.AddChildElem("EncodeMode");
			(void)(xml.IntoElem());
			IVS_CHAR cEncodeMode[IVS_DEV_MODEL_LEN+1];
			memset(cEncodeMode,0,IVS_DEV_MODEL_LEN+1);
			bRet = CToolsHelp::Memcpy(cEncodeMode,IVS_DEV_MODEL_LEN,stEncodeModeInfo.cEncodeMode,IVS_DEV_MODEL_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Camera Encode Capabilities List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cEncodeMode);

			(void)xml.AddElem("ResolutionList");
			for (int iResolutionIndex = 0;iResolutionIndex < stEncodeModeInfo.iRealNum;iResolutionIndex++)
			{
				IVS_RESOLUTION_INFO &stResolutionInfo = stEncodeModeInfo.stResolution[iResolutionIndex];
				iResolutionIndex == 0 ? (void)xml.AddChildElem("ResolutionInfo"):(void)xml.AddElem("ResolutionInfo");
				(void)(xml.IntoElem());

				(void)xml.AddChildElem("Resolution");
				(void)(xml.IntoElem());
				IVS_CHAR cResolution[IVS_FILE_NAME_LEN+1];
				memset(cResolution,0,IVS_FILE_NAME_LEN+1);
				bRet = CToolsHelp::Memcpy(cResolution,IVS_FILE_NAME_LEN,stResolutionInfo.cResolution,IVS_FILE_NAME_LEN);
				if(false == bRet)
				{
					BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Camera Encode Capabilities List Get XML", "Memcpy error.");
					return IVS_ALLOC_MEMORY_ERROR;
				}
				(void)xml.SetElemValue(cResolution);

				(void)xml.AddElem("PType");
				(void)xml.AddChildElem("FrameList");
				(void)(xml.IntoElem());
				IVS_CHAR cPFrameList[IVS_MAX_NAME_LIST_LEN+1];
				memset(cPFrameList,0,IVS_MAX_NAME_LIST_LEN+1);
				bRet = CToolsHelp::Memcpy(cPFrameList,IVS_MAX_NAME_LIST_LEN,stResolutionInfo.cPFrameList,IVS_MAX_NAME_LIST_LEN);
				if(false == bRet)
				{
					BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Camera Encode Capabilities List Get XML", "Memcpy error.");
					return IVS_ALLOC_MEMORY_ERROR;
				}
				(void)xml.SetElemValue(cPFrameList);
				xml.OutOfElem();

				(void)xml.AddElem("NType");
				(void)xml.AddChildElem("FrameList");
				(void)(xml.IntoElem());
				IVS_CHAR cNFrameList[IVS_MAX_NAME_LIST_LEN+1];
				memset(cNFrameList,0,IVS_MAX_NAME_LIST_LEN+1);
				bRet = CToolsHelp::Memcpy(cNFrameList,IVS_MAX_NAME_LIST_LEN,stResolutionInfo.cNFrameList,IVS_MAX_NAME_LIST_LEN);
				if(false == bRet)
				{
					BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Camera Encode Capabilities List Get XML", "Memcpy error.");
					return IVS_ALLOC_MEMORY_ERROR;
				}
				(void)xml.SetElemValue(cNFrameList);
				xml.OutOfElem();

				xml.OutOfElem();
			}
			xml.OutOfElem();

			(void)xml.AddElem("ProfileList");
			IVS_CHAR cProfileList[IVS_MAX_NAME_LIST_LEN+1];
			memset(cProfileList,0,IVS_MAX_NAME_LIST_LEN+1);
			bRet = CToolsHelp::Memcpy(cProfileList,IVS_MAX_NAME_LIST_LEN,stEncodeModeInfo.cProfileList,IVS_MAX_NAME_LIST_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Camera Encode Capabilities List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cProfileList);

			(void)xml.AddElem("IFrameInfo");
			(void)xml.AddChildElem("PType");
			(void)(xml.IntoElem());
			(void)xml.AddChildElem("MinIFrameInterval");
			(void)(xml.IntoElem());
			(void)xml.SetElemValue(CToolsHelp::Int2Str(stEncodeModeInfo.stIFrameInfo.iPMinIFrameInterval).c_str());
			(void)xml.AddElem("MaxIFrameInterval");
			(void)xml.SetElemValue(CToolsHelp::Int2Str(stEncodeModeInfo.stIFrameInfo.iPMaxIFrameInterval).c_str());
			xml.OutOfElem();
			(void)xml.AddElem("NType");
			(void)xml.AddChildElem("MinIFrameInterval");
			(void)(xml.IntoElem());
			(void)xml.SetElemValue(CToolsHelp::Int2Str(stEncodeModeInfo.stIFrameInfo.iNMinIFrameInterval).c_str());
			(void)xml.AddElem("MaxIFrameInterval");
			(void)xml.SetElemValue(CToolsHelp::Int2Str(stEncodeModeInfo.stIFrameInfo.iNMaxIFrameInterval).c_str());
			xml.OutOfElem();
			xml.OutOfElem();

			(void)xml.AddElem("BitRateList");
			IVS_CHAR cBitRateList[IVS_MAX_NAME_LIST_LEN+1];
			memset(cBitRateList,0,IVS_MAX_NAME_LIST_LEN+1);
			bRet = CToolsHelp::Memcpy(cBitRateList,IVS_MAX_NAME_LIST_LEN,stEncodeModeInfo.cBitRateList,IVS_MAX_NAME_LIST_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Camera Encode Capabilities List Get XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cBitRateList);

			xml.OutOfElem();
		}

		xml.OutOfElem();
		xml.OutOfElem();
	}
	xml.OutOfElem();
	return IVS_SUCCEED;
}


IVS_INT32 COCXDeviceMgr::GetVideoDiagnose(IVS_INT32 iSessionID,
	const IVS_CHAR* pDomainCode,
	const IVS_CHAR* pCameraCode,
	IVS_CHAR** pRspXml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDomainCode, IVS_XML_INVALID);
	CHECK_POINTER(pCameraCode, IVS_XML_INVALID);
	CHECK_POINTER(pRspXml, IVS_XML_INVALID);
	// 调用SDK接口
	IVS_INT32 iRet = IVS_SDK_GetVideoDiagnose(iSessionID, pDomainCode, pCameraCode, pRspXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetVideoDiagnose", "SDK Interface IVS_SDK_GetVideoDiagnose fail");
	}
	return iRet;
}

IVS_INT32 COCXDeviceMgr::SetVideoDiagnose(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	// 调用SDK接口
	IVS_INT32 iRet = IVS_SDK_SetVideoDiagnose(iSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SetVideoDiagnose", "SDK Interface IVS_SDK_SetVideoDiagnose fail");
	}
	return iRet;
}