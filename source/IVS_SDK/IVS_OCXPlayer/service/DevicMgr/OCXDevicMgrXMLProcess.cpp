/*****************************************************************
 filename    :    OCXDevicMgrXMLProcess.cpp
 author      :    guandiqun
 created     :    2012/11/19
 description :    实现设备;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*****************************************************************/
#include "OCXDevicMgrXMLProcess.h"
#include "bp_log.h"
#include "ToolsHelp.h"
#include "DevEnumXML.h"
#include "OCXXmlProcess.h"
#include "IVS_Trace.h"

// 添加主设备的解析xml
IVS_INT32 COCXDeviceXMLProcess::AddDeviceParseXml( CXml &reqXml, IVS_VOID* pReqBuffer, const IVS_UINT32 &uiNum, IVS_CHAR* pDomain)
{
    CHECK_POINTER(pReqBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomain, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    //查找是否有Content/DevList元素
    if (!reqXml.FindElemEx("Content/DomainCode"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "FALSE == reqXml.FindElemEx('Content/DomainCode'')", "NA");
        return IVS_XML_INVALID;
    }       
    const IVS_CHAR* pTemp = NULL;
    GET_ELEM_VALUE_CHAR("DomainCode", pTemp, pDomain, IVS_DOMAIN_CODE_LEN, reqXml);
    reqXml.OutOfElem();

    if (!reqXml.FindElemEx("Content/DevList"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "FALSE == reqXml.FindElemEx('Content/DevList'')", "NA");
        return IVS_XML_INVALID;
    }

    IVS_DEVICE_OPER_INFO* pDevList = (IVS_DEVICE_OPER_INFO*)pReqBuffer;

    if (!reqXml.FindElem("DevInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == reqXml.FindElem(DevInfo)", "NA");
        return IVS_XML_INVALID;
    }

    IVS_UINT32 ui = 0;
    IVS_CHAR chTemp[128];
    do 
    {
        if (!reqXml.FindElemValue("DevInfo") && !reqXml.NextElem())
        {
            break;
        }
        reqXml.IntoElem();  

        IVS_DEVICE_OPER_INFO &stDevInfo = pDevList[ui];
        //添加序号
        GET_ELEM_VALUE_NUM_FOR_UINT("Sequence", pTemp, stDevInfo.uiSequence, reqXml);  
        //DomainCode
        //主设备编码
        GET_ELEM_VALUE_CHAR("DevCode", pTemp, stDevInfo.stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN, reqXml);
        //主设备设备名称
        GET_ELEM_VALUE_CHAR("DevName", pTemp, stDevInfo.stDeviceCfg.stDeviceInfo.cName, IVS_DEVICE_NAME_LEN, reqXml);
        //主设备协议类型
        GET_ELEM_VALUE_CHAR("DevProtocolType", pTemp, stDevInfo.stDeviceCfg.cProtocolType,IVS_MAX_PROTOCOLTYPE_NAME_LEN, reqXml);  
		//认证类型
	    GET_ELEM_VALUE_NUM_FOR_UINT("DevLoginType", pTemp, stDevInfo.stDeviceCfg.iLoginType, reqXml); 

        //主设备提供商类型
		GET_ELEM_VALUE_CHAR("DevVendorType", pTemp, stDevInfo.stDeviceCfg.stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, reqXml);
        //主设备形态类型  MAXLEN    ========================================
        memset(chTemp, 0, 128);
        GET_ELEM_VALUE_CHAR("DevFormType", pTemp, chTemp, 128, reqXml);
        stDevInfo.stDeviceCfg.stDeviceInfo.uiType = static_cast<IVS_UINT32>(CDevEnumXML::GetDevTypeEnum(chTemp));
        //主设备型号
        GET_ELEM_VALUE_CHAR("DevModelType", pTemp, stDevInfo.stDeviceCfg.stDeviceInfo.cModel, IVS_DEV_MODEL_LEN , reqXml);
        //设备告警使能 MAXLEN   ====================================
		if (reqXml.FindElem("DevAlarmEnable"))
		{
			GET_ELEM_VALUE_NUM("DevAlarmEnable", pTemp, stDevInfo.stDeviceCfg.bEnableAlarm, reqXml);
		}
		else
		{
			stDevInfo.stDeviceCfg.bEnableAlarm = 1;
		}

        //视频制式 MAXLEN    ========================================
        memset(chTemp, 0, 128);
        GET_ELEM_VALUE_CHAR("VideoFormat", pTemp, chTemp, 128,reqXml); 
        stDevInfo.stDeviceCfg.uiVideoFormat = static_cast<IVS_UINT32>(CDevEnumXML::GetVideoFormatEnum(chTemp));
        //是否直连优先
		GET_ELEM_VALUE_NUM_FOR_UINT("DirectConnectFirst", pTemp, stDevInfo.stDeviceCfg.uiDirectConnectFirst, reqXml); 
         
        //直连最大数目
		GET_ELEM_VALUE_NUM_FOR_UINT("MaxDirectConnectNum", pTemp, stDevInfo.stDeviceCfg.uiMaxDirectConnectNum, reqXml);

        //设备所属NVR编码
        GET_ELEM_VALUE_CHAR("NVRCode", pTemp,  stDevInfo.stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN, reqXml);
        //所属TAS编码      
        GET_ELEM_VALUE_CHAR("DCGCode", pTemp, stDevInfo.stDeviceCfg.cTasCode, IVS_TAS_CODE_LEN, reqXml);
		// DCGIP 
		GET_ELEM_VALUE_CHAR("DCGIP", pTemp, stDevInfo.stDeviceCfg.stTasIP.cIP, IVS_IP_LEN, reqXml);
        //IP类型
		GET_ELEM_VALUE_NUM_FOR_UINT("DevIPType", pTemp, stDevInfo.stDeviceCfg.stDeviceInfo.stIP.uiIPType, reqXml);
        
        //设备所在IP地址
        GET_ELEM_VALUE_CHAR("DevIP", pTemp, stDevInfo.stDeviceCfg.stDeviceInfo.stIP.cIP, IVS_IP_LEN, reqXml);
		//设备登录端口
		GET_ELEM_VALUE_NUM_FOR_UINT("DevPort", pTemp, stDevInfo.stDeviceCfg.stDeviceInfo.uiPort, reqXml);

		//设备SN号
		GET_ELEM_VALUE_CHAR("DevSN", pTemp, stDevInfo.stDeviceCfg.stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN, reqXml);
        //设备登录用户名
        GET_ELEM_VALUE_CHAR("DevUser", pTemp, stDevInfo.stDeviceCfg.cDevUser, IVS_NAME_LEN, reqXml); 
        //设备登录密码
        GET_ELEM_VALUE_CHAR("DevPWD", pTemp, stDevInfo.stDeviceCfg.cDevPasswd, IVS_PWD_LEN, reqXml);
        //设备向平台密码
        GET_ELEM_VALUE_CHAR("DevRegPWD", pTemp, stDevInfo.stDeviceCfg.cDevRegPasswd, IVS_PWD_LEN, reqXml);   
        //是否支持集群调度
		GET_ELEM_VALUE_NUM("BlanceSupport", pTemp, stDevInfo.stDeviceCfg.bEnableSchedule, reqXml);  

        reqXml.OutOfElem();

        ui++;
    } while (reqXml.NextElem() && ui < uiNum);  

    return IVS_SUCCEED;
}

// 新增设备拼接XML
IVS_INT32 COCXDeviceXMLProcess::AddDeviceGetXml( CXml &rspXml, IVS_VOID* pRspData, const IVS_UINT32 &uiLen, const IVS_CHAR *pDomainCode)
{
    CHECK_POINTER(pRspData, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    IVS_DEVICE_OPER_RESULT_LIST *pResultList = (IVS_DEVICE_OPER_RESULT_LIST*)pRspData; 

	(void)rspXml.AddDeclaration("1.0","UTF-8","");
    rspXml.AddElem("Content");
    rspXml.AddChildElem("ResultCode");
    rspXml.IntoElem();
    rspXml.SetElemValue("0");
    rspXml.AddElem("DevList");
    for (IVS_UINT32 uiIndex = 0 ; uiIndex < uiLen; uiIndex++)
    {
        IVS_DEVICE_OPER_RESULT &stQueryDeviceInfo = pResultList->stCodeInfo[uiIndex];

        if (0 == uiIndex)
        {
            rspXml.AddChildElem("DevInfo");            
        }
        else
        {
            rspXml.AddElem("DevInfo");
        }
        rspXml.IntoElem();

        rspXml.AddChildElem("Sequence");
        rspXml.IntoElem();
        rspXml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)stQueryDeviceInfo.uiSequence).c_str());

        //主设备编码
        rspXml.AddElem("DevCode"); 
		std::string strBigDevCode;
		IVS_CHAR cCode[IVS_DEV_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cCode, IVS_DEV_CODE_LEN, stQueryDeviceInfo.cCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cCode, IVS_DEV_CODE_LEN, stQueryDeviceInfo.cCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		strBigDevCode.append(cCode).append("#").append(pDomainCode);
        rspXml.SetElemValue(strBigDevCode.c_str());

        //NVR编码
        rspXml.AddElem("NVRCode");
		IVS_CHAR cNVRCode[IVS_NVR_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cNVRCode, IVS_NVR_CODE_LEN, stQueryDeviceInfo.cNvrCode, IVS_NVR_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "stQueryDeviceInfo.cNvrCode memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        rspXml.SetElemValue(cNVRCode);

        rspXml.AddElem("Result");
        rspXml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)stQueryDeviceInfo.uiResult).c_str());

        rspXml.OutOfElem();
    }

    return IVS_SUCCEED;
}

// 获取主设备列表拼接XML
IVS_INT32 COCXDeviceXMLProcess::GetMainDeviceListGetXml( CXml &rspXml, IVS_VOID* pRspData ,IVS_VOID* pDomain)
{
    CHECK_POINTER(pRspData, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomain, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    IVS_DEVICE_BRIEF_INFO_LIST *pRspDeviceList = (IVS_DEVICE_BRIEF_INFO_LIST*)pRspData; 
    IVS_CHAR *pchDomain  =( IVS_CHAR * )(pDomain);

	(void)rspXml.AddDeclaration("1.0","UTF-8","");
    rspXml.AddElem("Content");
    rspXml.AddChildElem("ResultCode");
    rspXml.IntoElem();
    rspXml.SetElemValue("0");
    rspXml.AddElem("PageInfo");   
    rspXml.AddChildElem("RealNum");
    rspXml.IntoElem();
    rspXml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRspDeviceList->uiTotal).c_str());

    rspXml.AddElem("FromIndex");
    rspXml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRspDeviceList->stIndexRange.uiFromIndex).c_str());

    rspXml.AddElem("ToIndex");
    rspXml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pRspDeviceList->stIndexRange.uiToIndex).c_str());

    rspXml.OutOfElem();

    rspXml.AddElem("DevList");

	IVS_UINT32 iNum = 0;
	if (pRspDeviceList->uiTotal >= pRspDeviceList->stIndexRange.uiToIndex)
	{
		iNum = (pRspDeviceList->stIndexRange.uiToIndex - pRspDeviceList->stIndexRange.uiFromIndex) + 1;
	}
	else
	{
		iNum = (pRspDeviceList->uiTotal - pRspDeviceList->stIndexRange.uiFromIndex) + 1;
	}
    IVS_UINT32 i= 0;
    const IVS_UINT32 uiMaxElemLen = 256;
    IVS_CHAR chTemp[uiMaxElemLen];

    while (i < iNum)
    {
        i == 0 ? rspXml.AddChildElem("DevInfo"):rspXml.AddElem("DevInfo");
        rspXml.IntoElem();
        IVS_DEVICE_BRIEF_INFO &devInfo = pRspDeviceList->stDeviceBriefInfo[i];

        //主设备编码
        rspXml.AddChildElem("DevCode");
        rspXml.IntoElem();

		std::string strBigCode;
		IVS_CHAR cCode[IVS_DEV_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cCode, IVS_DEV_CODE_LEN, devInfo.stDeviceInfo.cCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cCode, IVS_DEV_CODE_LEN, devInfo.stDeviceInfo.cCode, IVS_DEV_CODE_LEN) fail", "devInfo.stDeviceInfo.cName copy fail");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		strBigCode.append(cCode).append("#").append(pchDomain);
		rspXml.SetElemValue(strBigCode.c_str());   
        //主设备设备名称
        rspXml.AddElem("DevName");
        memset(chTemp, 0, uiMaxElemLen);
	    if (!CToolsHelp::Memcpy(chTemp, uiMaxElemLen, devInfo.stDeviceInfo.cName, IVS_DEVICE_NAME_LEN))
	    {
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiMaxElemLen, devInfo.stDeviceInfo.cName, IVS_DEVICE_NAME_LEN) fail", "devInfo.stDeviceInfo.cName copy fail");
			return IVS_ALLOC_MEMORY_ERROR;
	    }
        rspXml.SetElemValue(chTemp);
        //主设备协议类型
        rspXml.AddElem("DevProtocolType"); 
		IVS_CHAR cDevProtocolType[IVS_MAX_PROTOCOLTYPE_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDevProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN, devInfo.cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN, devInfo.cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN) fail", "devInfo.stDeviceInfo.cName copy fail");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		rspXml.SetElemValue(cDevProtocolType);
        //认证类型
        rspXml.AddElem("DevLoginType");
        rspXml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)devInfo.iLoginType).c_str());
        //主设备提供商类型
        rspXml.AddElem("DevVendorType");
		IVS_CHAR cDevVendorType[IVS_MAX_VENDOR_TYPE_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDevVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, devInfo.stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, devInfo.stDeviceInfo.cVendorType) fail", "devInfo.stDeviceInfo.cName copy fail");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		rspXml.SetElemValue(cDevVendorType);
        //主设备形态类型 ======================================
        rspXml.AddElem("DevFormType");
        rspXml.SetElemValue(CDevEnumXML::GetDevType((IVS_INT32)devInfo.stDeviceInfo.uiType));
        //主设备型号
        rspXml.AddElem("DevModelType");
        memset(chTemp, 0, uiMaxElemLen);
		if (!CToolsHelp::Memcpy(chTemp, uiMaxElemLen, devInfo.stDeviceInfo.cModel, IVS_DEV_MODEL_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "devInfo.stDeviceInfo.cModel memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        rspXml.SetElemValue(chTemp); 
		//设备SN号(移动模拟前端为IMEI号)
		rspXml.AddElem("DevSN");
		IVS_CHAR cDevSN[IVS_DEV_SERIALNO_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDevSN, IVS_DEV_SERIALNO_LEN, devInfo.stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevSN, IVS_DEV_SERIALNO_LEN, devInfo.stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		rspXml.SetElemValue(cDevSN);
        //IP类型
        rspXml.AddElem("DevIPType");
        rspXml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)devInfo.stDeviceInfo.stIP.uiIPType).c_str());
        //设备所在IP地址
        rspXml.AddElem("DevIP");
        memset(chTemp, 0, uiMaxElemLen);
		if (!CToolsHelp::Memcpy(chTemp, uiMaxElemLen, devInfo.stDeviceInfo.stIP.cIP, IVS_IP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "devInfo.stDeviceInfo.stIP.cIP memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        rspXml.SetElemValue(chTemp);     
        //是否在线    ==============================
        rspXml.AddElem("IsOnline");       
        rspXml.SetElemValue(CDevEnumXML::GetCommStatusType((IVS_INT32)devInfo.uiStatus));
        //是否支持集群调度 
        rspXml.AddElem("BlanceSupport");
        rspXml.SetElemValue(CToolsHelp::Int2Str(devInfo.bEnableSchedule).c_str());

        rspXml.OutOfElem();
        i++;

    }
    rspXml.OutOfElem();
    rspXml.OutOfElem();  

    return IVS_SUCCEED;
}

// 解析查询主设备详细信息请求xml
IVS_INT32 COCXDeviceXMLProcess::GetDeviceInfoParseXML(const IVS_CHAR* pDomainCode,const IVS_CHAR* pDevCode, CXml& xml)
{
    CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDevCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    if (!xml.FindElem("Content"))
    {
        return IVS_XML_INVALID;
    }
    (void)xml.IntoElem();

    const char* szElemValue = NULL;
    GET_ELEM_VALUE_CHAR("DomainCode", szElemValue, pDomainCode, IVS_DEV_CODE_LEN, xml);
    GET_ELEM_VALUE_CHAR("DevCode", szElemValue, pDevCode, IVS_DEV_CODE_LEN, xml);

    xml.OutOfElem();

    return IVS_SUCCEED;
}

// 查询主设备详细信息响应xml
IVS_INT32 COCXDeviceXMLProcess::GetDeviceInfoXML(const IVS_DEVICE_INFO* pDeviceInfo, CXml& xml)
{
    CHECK_POINTER(pDeviceInfo,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue("0");
    (void)xml.AddElem("DevInfo");
    (void)xml.AddChildElem("DevCode");
    (void)xml.IntoElem();
	IVS_CHAR cCode[IVS_DEV_CODE_LEN+1] = {0};
	(void)CToolsHelp::Memcpy(cCode, IVS_DEV_CODE_LEN, pDeviceInfo->stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN);
    (void)xml.SetElemValue(cCode);
    (void)xml.AddElem("DevName");
	IVS_CHAR cName[IVS_DEVICE_NAME_LEN+1] = {0};
	(void)CToolsHelp::Memcpy(cName, IVS_DEVICE_NAME_LEN, pDeviceInfo->stDeviceCfg.stDeviceInfo.cName, IVS_DEVICE_NAME_LEN);
    (void)xml.SetElemValue(cName);
	(void)xml.AddElem("DevProtocolType");
	IVS_CHAR cProtocolType[IVS_MAX_PROTOCOLTYPE_NAME_LEN+1] = {0};
	(void)CToolsHelp::Memcpy(cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN, pDeviceInfo->stDeviceCfg.cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN);
	(void)xml.SetElemValue(cProtocolType);
    (void)xml.AddElem("DevLoginType");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((int)pDeviceInfo->stDeviceCfg.iLoginType).c_str());
	(void)xml.AddElem("DevVendorType");
	IVS_CHAR cVendorType[IVS_MAX_VENDOR_TYPE_NAME_LEN+1] = {0};
	(void)CToolsHelp::Memcpy(cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, pDeviceInfo->stDeviceCfg.stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN);
    (void)xml.SetElemValue(cVendorType);
	(void)xml.AddElem("DevFormType");
    (void)xml.SetElemValue(CDevEnumXML::GetDevType((IVS_INT32)pDeviceInfo->stDeviceCfg.stDeviceInfo.uiType));
    (void)xml.AddElem("DevModelType");
	IVS_CHAR cModel[IVS_DEV_MODEL_LEN+1] = {0};
	(void)CToolsHelp::Memcpy(cModel, IVS_DEV_MODEL_LEN, pDeviceInfo->stDeviceCfg.stDeviceInfo.cModel, IVS_DEV_MODEL_LEN);
    (void)xml.SetElemValue(cModel);
	(void)xml.AddElem("DevAlarmEnable");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pDeviceInfo->stDeviceCfg.bEnableAlarm).c_str());
    (void)xml.AddElem("VideoFormat");
    (void)xml.SetElemValue(CDevEnumXML::GetVideoFormatType((IVS_INT32) pDeviceInfo->stDeviceCfg.uiVideoFormat));
    (void)xml.AddElem("DirectConnectFirst");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((int)pDeviceInfo->stDeviceCfg.uiDirectConnectFirst).c_str());
    (void)xml.AddElem("NVRCode");
	IVS_CHAR cNvrCode[IVS_NVR_CODE_LEN+1] = {0};
	(void)CToolsHelp::Memcpy(cNvrCode, IVS_NVR_CODE_LEN, pDeviceInfo->stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN);
	(void)xml.SetElemValue(cNvrCode);
	(void)xml.AddElem("DCGCode");
	IVS_CHAR cTasCode[IVS_TAS_CODE_LEN+1] = {0};
	(void)CToolsHelp::Memcpy(cTasCode, IVS_TAS_CODE_LEN, pDeviceInfo->stDeviceCfg.cTasCode, IVS_TAS_CODE_LEN);
	(void)xml.SetElemValue(cTasCode);
    (void)xml.AddElem("DevUser");
	IVS_CHAR cDevUser[IVS_NAME_LEN+1] = {0};
	(void)CToolsHelp::Memcpy(cDevUser, IVS_NAME_LEN, pDeviceInfo->stDeviceCfg.cDevUser, IVS_NAME_LEN);
    (void)xml.SetElemValue(cDevUser);
    (void)xml.AddElem("DevPWD");
	IVS_CHAR cDevPasswd[IVS_TAS_CODE_LEN+1] = {0};
	(void)CToolsHelp::Memcpy(cDevPasswd, IVS_PWD_LEN, pDeviceInfo->stDeviceCfg.cDevPasswd, IVS_PWD_LEN);
    (void)xml.SetElemValue(cDevPasswd);
    (void)xml.AddElem("DevRegPWD");
	IVS_CHAR cDevRegPasswd[IVS_PWD_LEN+1] = {0};
	(void)CToolsHelp::Memcpy(cDevRegPasswd, IVS_PWD_LEN, pDeviceInfo->stDeviceCfg.cDevRegPasswd, IVS_PWD_LEN);
    (void)xml.SetElemValue(cDevRegPasswd);
    (void)xml.AddElem("BlanceSupport");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(pDeviceInfo->stDeviceCfg.bEnableSchedule).c_str());
    (void)xml.AddElem("MaxVideoNum");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((int)pDeviceInfo->stChannelInfo.uiMaxVideoNum).c_str());
    (void)xml.AddElem("MaxAudioInNum");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((int)pDeviceInfo->stChannelInfo.uiMaxAudioInNum).c_str());
    (void)xml.AddElem("MaxAudioOutNum");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((int)pDeviceInfo->stChannelInfo.uiMaxAudioOutNum).c_str());
    (void)xml.AddElem("MaxAlarmInNum");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((int)pDeviceInfo->stChannelInfo.uiMaxAlarmInNum).c_str());
    (void)xml.AddElem("MaxAlarmOutNum");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((int)pDeviceInfo->stChannelInfo.uiMaxAlarmOutNum).c_str());
    (void)xml.AddElem("MaxSerialNum");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((int)pDeviceInfo->stChannelInfo.uiMaxSerialNum).c_str());

    xml.OutOfElem();
    xml.OutOfElem();

    return IVS_SUCCEED;
}

// 查询NVR列表解析XML
IVS_INT32 COCXDeviceXMLProcess::ParseGetNVRListReq(CXml& xml,IVS_CHAR* cDomainCode,IVS_INDEX_RANGE &sIndex,IVS_UINT32 &uiServerType)
{
	CHECK_POINTER(cDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    if (!xml.FindElemEx("Content/PageInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(Content/PageInfo) is fail", "NA");
        return IVS_XML_INVALID;
    }   

    const char* pTemp = NULL;
    (void)xml.IntoElem();
    GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex", pTemp, sIndex.uiFromIndex, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex", pTemp, sIndex.uiToIndex, xml);
    xml.OutOfElem();

    if (!xml.FindElemEx("Content/NVRInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(Content/PageInfo) is fail", "NA");
        return IVS_XML_INVALID;
    }   

    xml.IntoElem();
    GET_ELEM_VALUE_NUM_FOR_UINT("NVRType",pTemp, uiServerType, xml);
    GET_ELEM_VALUE_CHAR("DomainCode",pTemp, cDomainCode, IVS_DOMAIN_CODE_LEN, xml);
    xml.OutOfElem();

    return IVS_SUCCEED;
}

// 查询NVR列表拼接XML
IVS_INT32 COCXDeviceXMLProcess::GetNVRListRsp(IVS_DEVICE_BRIEF_INFO_LIST *pNVRList,IVS_INT32 iRetCode,CXml& xml)
{
    if (NULL == pNVRList)
    {
        return IVS_OPERATE_MEMORY_ERROR;
    }
	IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");	
    (void)xml.AddChildElem("ResultCode");
    (void)(xml.IntoElem());
    (void)xml.SetElemValue(CToolsHelp::Int2Str(iRetCode).c_str());

    (void)xml.AddElem("PageInfo");
    (void)xml.AddChildElem("Total");
    (void)(xml.IntoElem());
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pNVRList->uiTotal).c_str());
    (void)xml.AddElem("FromIndex");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pNVRList->stIndexRange.uiFromIndex).c_str());
    (void)xml.AddElem("ToIndex");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pNVRList->stIndexRange.uiToIndex).c_str());
    xml.OutOfElem();

    (void)xml.AddElem("NVRList");
    if(pNVRList->uiTotal>0)
    {
        IVS_UINT32 iRealNum = pNVRList->stIndexRange.uiToIndex - pNVRList->stIndexRange.uiFromIndex ;
        iRealNum += 1;

        for (IVS_UINT32 iIndex = 0;iIndex<iRealNum;iIndex++)
        {
            IVS_DEVICE_BRIEF_INFO &stDeviceBriefInfo = pNVRList->stDeviceBriefInfo[iIndex];
            iIndex == 0 ? (void)xml.AddChildElem("NVRInfo"):(void)xml.AddElem("NVRInfo");
            (void)(xml.IntoElem());
            (void)xml.AddChildElem("DomainCode");
            (void)(xml.IntoElem());

			IVS_CHAR cDomainCode[IVS_DOMAIN_CODE_LEN+1];
			memset(cDomainCode,0,IVS_DOMAIN_CODE_LEN+1);
			if (!CToolsHelp::Memcpy(cDomainCode, IVS_DOMAIN_CODE_LEN, stDeviceBriefInfo.stDeviceInfo.cDomainCode, IVS_DOMAIN_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "stDeviceBriefInfo.stDeviceInfo.cDomainCode memcpy fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cDomainCode);

            (void)xml.AddElem("NVRCode");
			IVS_CHAR cCode[IVS_DEV_CODE_LEN+1];
			memset(cCode,0,IVS_DEV_CODE_LEN+1);
			if (!CToolsHelp::Memcpy(cCode, IVS_DEV_CODE_LEN, stDeviceBriefInfo.stDeviceInfo.cCode, IVS_DEV_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "stDeviceBriefInfo.stDeviceInfo.cCode memcpy fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cCode);

            (void)xml.AddElem("NVRName");
			IVS_CHAR cName[IVS_DEVICE_NAME_LEN+1];
			memset(cName,0,IVS_DEVICE_NAME_LEN+1);
			if (!CToolsHelp::Memcpy(cName, IVS_DEVICE_NAME_LEN, stDeviceBriefInfo.stDeviceInfo.cName, IVS_DEVICE_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "stDeviceBriefInfo.stDeviceInfo.cName memcpy fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cName);

            (void)xml.AddElem("NVRType");
            (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)stDeviceBriefInfo.stDeviceInfo.uiType).c_str());
            (void)xml.AddElem("NVRModel");
			IVS_CHAR cModel[IVS_DEV_MODEL_LEN+1];
			memset(cModel,0,IVS_DEV_MODEL_LEN+1);
			if (!CToolsHelp::Memcpy(cModel, IVS_DEV_MODEL_LEN, stDeviceBriefInfo.stDeviceInfo.cModel, IVS_DEV_MODEL_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "stDeviceBriefInfo.stDeviceInfo.cModel memcpy fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cModel);

            (void)xml.AddElem("NVRVendor");
			IVS_CHAR cNVRVendor[IVS_MAX_VENDOR_TYPE_NAME_LEN + 1] = {0};
			if (!CToolsHelp::Memcpy(cNVRVendor, IVS_MAX_VENDOR_TYPE_NAME_LEN, stDeviceBriefInfo.stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cNVRVendor, IVS_MAX_VENDOR_TYPE_NAME_LEN, stDeviceBriefInfo.stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
            (void)xml.SetElemValue(cNVRVendor);
            (void)xml.AddElem("NVRIp");
			IVS_CHAR cIP[IVS_IP_LEN+1];
			memset(cIP,0,IVS_IP_LEN+1);
			if (!CToolsHelp::Memcpy(cIP, IVS_IP_LEN, stDeviceBriefInfo.stDeviceInfo.stIP.cIP, IVS_IP_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "stDeviceBriefInfo.stDeviceInfo.stIP.cIP memcpy fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cIP);

			(void)xml.AddElem("NVRPort");
			(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)stDeviceBriefInfo.stDeviceInfo.uiPort).c_str());

            (void)xml.AddElem("NVROnline");
            IVS_UINT32 iStatus = stDeviceBriefInfo.uiStatus;
            if (1 == iStatus)
            {
                (void)xml.SetElemValue("ON");
            }
            else
            {
                (void)xml.SetElemValue("OFF");
            }
            xml.OutOfElem();
        }
    }
    xml.OutOfElem();

    return IVS_SUCCEED;
}

// 删除设备解析XML
IVS_INT32 COCXDeviceXMLProcess::DeleteDevicePraseXML(IVS_DEVICE_CODE* pDevCode, IVS_CHAR *pDomainCode, const IVS_UINT32& uiDeviceNum, CXml& xmlReq)
{
    CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    if (!xmlReq.FindElemEx("Content/DevList"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElemEx(Content/DevList) is fail", "NA");
        return IVS_XML_INVALID;
    }

    const IVS_UINT32 MAXELEMLEN = IVS_DEV_CODE_LEN + IVS_DOMAIN_CODE_LEN +2; 
	IVS_CHAR chTemp[MAXELEMLEN + 1] = {0};
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_UINT32 iIndex = 0;
	
	do 
	{
		if (!xmlReq.FindElemValue("DevCode"))
		{
			break;
		}
		memset(chTemp , 0, MAXELEMLEN + 1);
		memset(chDevCode , 0, IVS_DEV_CODE_LEN + 1);
		memset(chTemp , 0, IVS_DOMAIN_CODE_LEN+1);
		if (!CToolsHelp::Strncpy(chTemp, MAXELEMLEN + 1, xmlReq.GetElemValue(), MAXELEMLEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Strncpy(chTemp, MAXELEMLEN + 1, xmlReq.GetElemValue(), MAXELEMLEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}

		IVS_INT32 iRet = COCXXmlProcess::ParseDevCode(chTemp, chDevCode, chDomaCode);
		if (IVS_SUCCEED == iRet)
		{
			if (!CToolsHelp::Memcpy(pDevCode[iIndex].cDevCode, IVS_DEV_CODE_LEN, chDevCode, IVS_DEV_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pDevCode[iIndex].cDevCode, IVS_DEV_CODE_LEN, chDevCode, IVS_DEV_CODE_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
		}
		else
		{
			if (!CToolsHelp::Memcpy(pDevCode[iIndex].cDevCode, IVS_DOMAIN_CODE_LEN, chTemp, IVS_DOMAIN_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pDevCode[iIndex].cDevCode, IVS_DOMAIN_CODE_LEN, chTemp, IVS_DOMAIN_CODE_LEN fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
		}
		
		++iIndex;
	} while (xmlReq.NextElem() && iIndex < uiDeviceNum);
    if (!CToolsHelp::Memcpy(pDomainCode, IVS_DOMAIN_CODE_LEN, chDomaCode, IVS_DOMAIN_CODE_LEN))
    {
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pDomainCode, IVS_DOMAIN_CODE_LEN, chDomaCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
    }

    return IVS_SUCCEED;
}

// 删除设备拼接XML
IVS_INT32 COCXDeviceXMLProcess::DeleteDeviceGetXml(IVS_RESULT_LIST* pReqList, CXml& xmlRsp, const IVS_CHAR *pDomainCode)
{
    CHECK_POINTER(pReqList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    (void)xmlRsp.AddDeclaration("1.0","UTF-8","");
    (void)xmlRsp.AddElem("Content");
    (void)xmlRsp.IntoElem();
    (void)xmlRsp.AddChildElem("ResultCode");
    (void)xmlRsp.IntoElem();
    (void)xmlRsp.SetElemValue("0");
    (void)xmlRsp.AddElem("DevList");
    (void)xmlRsp.IntoElem();

    IVS_UINT32 uiNum = pReqList->uiNum;

    for (IVS_UINT32 uiIndex = 0; uiIndex < uiNum; uiIndex++)
    {
        IVS_RESULT_INFO &stInfo = pReqList->stInfo[uiIndex];
        if (0 == uiIndex)
        {
            xmlRsp.AddChildElem("DevInfo");
        }
        else
        {
            xmlRsp.AddElem("DevInfo");
        }
       
        (void)xmlRsp.IntoElem();
        //设备编码
        (void)xmlRsp.AddChildElem("DevCode");
		(void)xmlRsp.IntoElem();
		std::string strBigDevCode;
		IVS_CHAR cCode[IVS_DEV_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cCode, IVS_DEV_CODE_LEN, stInfo.cCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cCode, IVS_DEV_CODE_LEN, stInfo.cCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		strBigDevCode.append(cCode).append("#").append(pDomainCode);
        (void)xmlRsp.SetElemValue(strBigDevCode.c_str());
        //删除结果
        (void)xmlRsp.AddElem("Result");
        (void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(stInfo.iResult).c_str());
        xmlRsp.OutOfElem();

    }
	xmlRsp.GetRootPos();

    return IVS_SUCCEED;
}

IVS_INT32 COCXDeviceXMLProcess::QueryDeviceListGetXml( IVS_VOID* pRspData ,IVS_VOID* pDomain ,IVS_UINT32 iDevType, CXml &xmlRsp )
{
    IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_SUCCEED;

    if (NULL == pDomain || NULL == pRspData)
    {
        return IVS_OPERATE_MEMORY_ERROR;
    }    

    switch (iDevType)
    {
    case DEVICE_TYPE_MAIN:
        iRet = GetMainDeviceListGetXml(xmlRsp, pRspData, pDomain);
        break;
    default: 
		iRet = IVS_FAIL;
		break;
    }

    return iRet;
}

// 查询设备列表拼接XML
IVS_INT32 COCXDeviceXMLProcess::GetDeviceListGetXml(IVS_VOID* pRspData ,
	IVS_UINT32 iDevType, 
	CXml &xmlRsp)
{
	CHECK_POINTER(pRspData, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;;
	switch (iDevType)
	{
	case DEVICE_TYPE_CAMERA:
		{
			IVS_CAMERA_BRIEF_INFO_LIST *pRspDeviceList = (IVS_CAMERA_BRIEF_INFO_LIST*)pRspData; 
			iRet = GetCamerasGetXml(pRspDeviceList,xmlRsp);
		}
		break;
	case DEVICE_TYPE_VOICE:
		{
			IVS_AUDIO_BRIEF_INFO_LIST *pRspDeviceList = (IVS_AUDIO_BRIEF_INFO_LIST*)pRspData; 
			iRet = GetAudiosGetXml(pRspDeviceList,xmlRsp);
		}
		break;
	case DEVICE_TYPE_ALARM:
	case DEVICE_TYPE_EXTEND_ALARM:
		{
			IVS_ALARM_BRIEF_INFO_LIST *pRspDeviceList = (IVS_ALARM_BRIEF_INFO_LIST*)pRspData; 
			iRet = GetAlarmsGetXml(pRspDeviceList,xmlRsp);
		}
		break;
	default:
		{
			iRet = IVS_OPERATE_MEMORY_ERROR;
			BP_RUN_LOG_ERR(iRet, "Get DeviceList GetXml fail", "iDevType Error");    
		}
		break;
	}

	return iRet;

}

// 输入获取摄像机设备列表拼接xml
IVS_INT32 COCXDeviceXMLProcess::GetAlarmsGetXml( IVS_ALARM_BRIEF_INFO_LIST* pRspDeviceList,
	CXml &rspXml)
{
	CHECK_POINTER(pRspDeviceList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)rspXml.AddDeclaration("1.0","UTF-8","");
	(void)rspXml.AddElem("Content");
	(void)rspXml.AddChildElem("ResultCode");
	(void)rspXml.IntoElem();
	(void)rspXml.SetElemValue(CToolsHelp::Int2Str(IVS_SUCCEED).c_str());

	(void)rspXml.AddElem("PageInfo");
	(void)rspXml.IntoElem();

	(void)rspXml.AddChildElem("RealNum");
	(void)rspXml.IntoElem();
	(void)rspXml.SetElemValue(CToolsHelp::Int2Str((int)(pRspDeviceList->uiTotal)).c_str());

	(void)rspXml.AddElem("FromIndex");
	(void)rspXml.SetElemValue(CToolsHelp::Int2Str((int)(pRspDeviceList->stIndexRange.uiFromIndex)).c_str());

	(void)rspXml.AddElem("ToIndex");
	(void)rspXml.SetElemValue(CToolsHelp::Int2Str((int)(pRspDeviceList->stIndexRange.uiToIndex)).c_str());

	rspXml.OutOfElem();	

   if (pRspDeviceList->stIndexRange.uiFromIndex > pRspDeviceList->uiTotal)
   {
       return IVS_SUCCEED;
   }

	IVS_UINT32 iNum = pRspDeviceList->uiTotal;
	// 添加判断
	if (pRspDeviceList->uiTotal >= pRspDeviceList->stIndexRange.uiToIndex)
	{
		iNum = (pRspDeviceList->stIndexRange.uiToIndex - pRspDeviceList->stIndexRange.uiFromIndex) + 1;
	}
	else
	{
		iNum = (pRspDeviceList->uiTotal - pRspDeviceList->stIndexRange.uiFromIndex) + 1;
	}

	if (0 == pRspDeviceList->uiTotal)
	{
		iNum = 0;
	}
	else
	{
		(void)rspXml.AddElem("AlarmDevList");
	}

	IVS_UINT32 i= 0;
	while (i < iNum)
	{
		i == 0 ? rspXml.AddChildElem("AlarmDevInfo"):rspXml.AddElem("AlarmDevInfo");
		(void)rspXml.IntoElem();

		IVS_ALARM_BRIEF_INFO &devInfo = pRspDeviceList->stAlarmBriefInfo[i];
		//告警设备组编码
		(void)rspXml.AddChildElem("GroupID");
		(void)rspXml.IntoElem();
		IVS_CHAR cGroupID[IVS_DEVICE_GROUP_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cGroupID, IVS_DEVICE_GROUP_LEN, devInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cGroupID, IVS_DEVICE_GROUP_LEN, devInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cGroupID);
		//(void)rspXml.SetElemValue(CToolsHelp::Int2Str((int)(devInfo.uiDevGroupID)).c_str());
		//告警编码
		(void)rspXml.AddElem("DevCode");
		IVS_CHAR cDevCode[IVS_DEV_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDevCode, IVS_DEV_CODE_LEN, devInfo.cAlarmCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevCode, IVS_DEV_CODE_LEN, devInfo.cAlarmCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cDevCode);
		//域编码
		(void)rspXml.AddElem("DomainCode");
		IVS_CHAR cDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDomainCode, IVS_DOMAIN_CODE_LEN, devInfo.cDomainCode, IVS_DOMAIN_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDomainCode, IVS_DOMAIN_CODE_LEN, devInfo.cDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cDomainCode);
		//告警设备名称
		(void)rspXml.AddElem("DevName");
		IVS_CHAR cDevName[IVS_ALARM_IN_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDevName, IVS_ALARM_IN_NAME_LEN, devInfo.cAlarmName, IVS_ALARM_IN_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevName, IVS_ALARM_IN_NAME_LEN, devInfo.cAlarmName, IVS_ALARM_IN_NAME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cDevName);
		//通道类型
		(void)rspXml.AddElem("ChannelType");
		if(0 == devInfo.uiChannelType)
		{
			(void)rspXml.SetElemValue("02");
		}
		else
		{
			(void)rspXml.SetElemValue("03");
		}
		(void)rspXml.AddElem("ParentCode");
		IVS_CHAR cParentCode[IVS_DEV_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cParentCode, IVS_DEV_CODE_LEN, devInfo.cParentCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cParentCode, IVS_DEV_CODE_LEN, devInfo.cParentCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cParentCode);

		(void)rspXml.AddElem("IsExDomian");
		(void)rspXml.SetElemValue(CToolsHelp::Int2Str(devInfo.bIsExDomain).c_str());
		rspXml.OutOfElem();
		i++;

	}
	if (iNum != 0)
	{ 
		rspXml.OutOfElem();
	}

	rspXml.OutOfElem();  

	return IVS_SUCCEED;
}

IVS_INT32 COCXDeviceXMLProcess::GetAudiosGetXml( IVS_AUDIO_BRIEF_INFO_LIST* pRspDeviceList,
	CXml &rspXml)
{
	CHECK_POINTER(pRspDeviceList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)rspXml.AddDeclaration("1.0","UTF-8","");
	(void)rspXml.AddElem("Content");
	(void)rspXml.AddChildElem("ResultCode");
	(void)rspXml.IntoElem();
	(void)rspXml.SetElemValue(CToolsHelp::Int2Str(IVS_SUCCEED).c_str());

	(void)rspXml.AddElem("PageInfo");
	(void)rspXml.IntoElem();

	(void)rspXml.AddChildElem("RealNum");
	(void)rspXml.IntoElem();
	(void)rspXml.SetElemValue(CToolsHelp::Int2Str((int)(pRspDeviceList->uiTotal)).c_str());

	(void)rspXml.AddElem("FromIndex");
	(void)rspXml.SetElemValue(CToolsHelp::Int2Str((int)(pRspDeviceList->stIndexRange.uiFromIndex)).c_str());

	(void)rspXml.AddElem("ToIndex");
	(void)rspXml.SetElemValue(CToolsHelp::Int2Str((int)(pRspDeviceList->stIndexRange.uiToIndex)).c_str());

	rspXml.OutOfElem();	

	IVS_UINT32 iNum = pRspDeviceList->uiTotal;
	// 添加判断
	if (pRspDeviceList->uiTotal >= pRspDeviceList->stIndexRange.uiToIndex)
	{
		iNum = (pRspDeviceList->stIndexRange.uiToIndex - pRspDeviceList->stIndexRange.uiFromIndex) + 1;
	}
	else
	{
		iNum = (pRspDeviceList->uiTotal - pRspDeviceList->stIndexRange.uiFromIndex) + 1;
	}

	if (0 == pRspDeviceList->uiTotal)
	{
		iNum = 0;
	}
	else
	{
		(void)rspXml.AddElem("VoiceChanList");
	}

	IVS_UINT32 i= 0;
	while (i < iNum)
	{
		i == 0 ? rspXml.AddChildElem("VoiceChanInfo"):rspXml.AddElem("VoiceChanInfo");
		(void)rspXml.IntoElem();

		IVS_AUDIO_BRIEF_INFO &devInfo = pRspDeviceList->stAudioBriefInfo[i];
		//设备组编码
		(void)rspXml.AddChildElem("GroupID");
		(void)rspXml.IntoElem();
		(void)rspXml.SetElemValue(devInfo.cDevGroupCode);
		//语音通道编码
		(void)rspXml.AddElem("VoiceCode");
		(void)rspXml.SetElemValue(devInfo.cAudioCode);
		//语音通道名称
		(void)rspXml.AddElem("VoiceName");
		(void)rspXml.SetElemValue(devInfo.cAudioName);
		//通道类型
		(void)rspXml.AddElem("ChannelType");
		if(0 == devInfo.uiChannelType)
		{
			(void)rspXml.SetElemValue("04");
		}
		else
		{
			(void)rspXml.SetElemValue("05");
		}
		rspXml.OutOfElem();
		i++;

	}
	if (iNum != 0)
	{ 
		rspXml.OutOfElem();
	}

	rspXml.OutOfElem();  

	return IVS_SUCCEED;
}

// 获取摄像机设备列表拼接xml
IVS_INT32 COCXDeviceXMLProcess::GetCamerasGetXml( IVS_CAMERA_BRIEF_INFO_LIST* pRspDeviceList,
	CXml &rspXml)
{
	CHECK_POINTER(pRspDeviceList, IVS_OPERATE_MEMORY_ERROR); 
	IVS_DEBUG_TRACE("");

	(void)rspXml.AddDeclaration("1.0","UTF-8","");
	(void)rspXml.AddElem("Content");
	(void)rspXml.AddChildElem("ResultCode");
	(void)rspXml.IntoElem();
	(void)rspXml.SetElemValue(CToolsHelp::Int2Str(IVS_SUCCEED).c_str());

	(void)rspXml.AddElem("PageInfo");
	(void)rspXml.IntoElem();

	(void)rspXml.AddChildElem("RealNum");
	(void)rspXml.IntoElem();
	(void)rspXml.SetElemValue(CToolsHelp::Int2Str((int)(pRspDeviceList->uiTotal)).c_str());

	(void)rspXml.AddElem("FromIndex");
	(void)rspXml.SetElemValue(CToolsHelp::Int2Str((int)(pRspDeviceList->stIndexRange.uiFromIndex)).c_str());

	(void)rspXml.AddElem("ToIndex");
	(void)rspXml.SetElemValue(CToolsHelp::Int2Str((int)(pRspDeviceList->stIndexRange.uiToIndex)).c_str());

	rspXml.OutOfElem();

    if (pRspDeviceList->stIndexRange.uiFromIndex > pRspDeviceList->uiTotal)
    {
        return IVS_SUCCEED;
    }


	IVS_UINT32 iNum = pRspDeviceList->uiTotal;
	// 添加判断
	if (pRspDeviceList->uiTotal >= pRspDeviceList->stIndexRange.uiToIndex)
	{
		iNum = (pRspDeviceList->stIndexRange.uiToIndex - pRspDeviceList->stIndexRange.uiFromIndex) + 1;
	}
	else
	{
		iNum = (pRspDeviceList->uiTotal - pRspDeviceList->stIndexRange.uiFromIndex) + 1;
	}

	if (0 == pRspDeviceList->uiTotal)
	{
		iNum = 0;
	}
	else
	{
		(void)rspXml.AddElem("VideoInChanList");
	}

	IVS_UINT32 i= 0;
	while (i < iNum)
	{
		i == 0 ? rspXml.AddChildElem("VideoInChanInfo"):rspXml.AddElem("VideoInChanInfo");
		(void)rspXml.IntoElem();
		IVS_CAMERA_BRIEF_INFO &devInfo = pRspDeviceList->stCameraBriefInfo[i];
		//设备组编码
		(void)rspXml.AddChildElem("GroupID");
		(void)rspXml.IntoElem();
		IVS_CHAR cGroupID[IVS_DEVICE_GROUP_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cGroupID, IVS_DEVICE_GROUP_LEN, devInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cGroupID, IVS_DEVICE_GROUP_LEN, devInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cGroupID);
		//摄像机编码
		(void)rspXml.AddElem("CameraCode");
		IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cCameraCode, IVS_DEV_CODE_LEN, devInfo.cCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cCameraCode, IVS_DEV_CODE_LEN, devInfo.cCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cCameraCode);

		//域编码
		(void)rspXml.AddElem("DomainCode");
		IVS_CHAR cDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDomainCode, IVS_DOMAIN_CODE_LEN, devInfo.cDomainCode, IVS_DOMAIN_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDomainCode, IVS_DOMAIN_CODE_LEN, devInfo.cDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cDomainCode);

		//摄像机类型
		(void)rspXml.AddElem("CameraType");
		(void)rspXml.SetElemValue(CDevEnumXML::GetCameraTypeEnum(static_cast<IVS_INT32>(devInfo.uiType)));

		//摄像机名称
		(void)rspXml.AddElem("CameraName");
		IVS_CHAR cCameraName[IVS_CAMERA_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cCameraName, IVS_CAMERA_NAME_LEN, devInfo.cName, IVS_CAMERA_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cCameraName, IVS_CAMERA_NAME_LEN, devInfo.cName, IVS_CAMERA_NAME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cCameraName);
		//父设备编码
		(void)rspXml.AddElem("ParentCode"); 
		IVS_CHAR cParentCode[IVS_DEV_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cParentCode, IVS_DEV_CODE_LEN, devInfo.cParentCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cParentCode, IVS_DEV_CODE_LEN, devInfo.cParentCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cParentCode);
		//主设备型号
		(void)rspXml.AddElem("DevModelType");    
		IVS_CHAR cDevModelType[IVS_DEV_MODEL_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDevModelType, IVS_DEV_MODEL_LEN, devInfo.cDevModelType, IVS_DEV_MODEL_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevModelType, IVS_DEV_MODEL_LEN, devInfo.cDevModelType, IVS_DEV_MODEL_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cDevModelType);
		//主设备提供商类型
		(void)rspXml.AddElem("DevVendorType");
		IVS_CHAR cDevVendorType[IVS_MAX_VENDOR_TYPE_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDevVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, devInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, devInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cDevVendorType);
		//主设备形态类型
		(void)rspXml.AddElem("DevFormType");
		(void)rspXml.SetElemValue(CDevEnumXML::GetDevType(static_cast<IVS_INT32>(devInfo.uiDevFormType)));

		//摄像机位置
		(void)rspXml.AddElem("CameraLocation");
		IVS_CHAR cCameraLocation[IVS_DESCRIBE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cCameraLocation, IVS_DESCRIBE_LEN, devInfo.cCameraLocation, IVS_DESCRIBE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cCameraLocation, IVS_DESCRIBE_LEN, devInfo.cCameraLocation, IVS_DESCRIBE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cCameraLocation);
		//摄像机扩展状态
		(void)rspXml.AddElem("ExtendStatus");
		(void)rspXml.SetElemValue(CToolsHelp::Int2Str((int)(devInfo.uiCameraStatus)).c_str());
		//是否在线
		(void)rspXml.AddElem("IsOnline");
		(void)rspXml.SetElemValue(CDevEnumXML::GetCommStatusType(static_cast<IVS_INT32>(devInfo.uiStatus)));
		//网络类型
		(void)rspXml.AddElem("NetType");
		(void)rspXml.SetElemValue(CDevEnumXML::GetNetType(static_cast<IVS_INT32>(devInfo.uiNetType)));

		//是否支持智能分析
		(void)rspXml.AddElem("IsIntelligent");
		(void)rspXml.SetElemValue(CDevEnumXML::GetCommIsOrNotType(devInfo.bSupportIntelligent));

		//随路音频是否启用
		(void)rspXml.AddElem("VoiceInEnable");
		(void)rspXml.SetElemValue(CDevEnumXML::GetVoiceInEnable(devInfo.bEnableVoice));

		//NVR编码
		(void)rspXml.AddElem("NVRCode");
		IVS_CHAR cNVRCode[IVS_NVR_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cNVRCode, IVS_NVR_CODE_LEN, devInfo.cNvrCode, IVS_NVR_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cNVRCode, IVS_NVR_CODE_LEN, devInfo.cNvrCode, IVS_NVR_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cNVRCode);  

		//设备在线IP地址
		(void)rspXml.AddElem("DevIP");
		IVS_CHAR cDevIP[IVS_IP_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDevIP, IVS_IP_LEN, devInfo.cDevIp, IVS_IP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevIP, IVS_IP_LEN, devInfo.cDevIp, IVS_IP_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cDevIP);

		//DevCreateTime
		(void)rspXml.AddElem("DevCreateTime");
		IVS_CHAR cDevCreateTime[IVS_TIME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cDevCreateTime, IVS_TIME_LEN, devInfo.cDevCreateTime, IVS_TIME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevCreateTime, IVS_TIME_LEN, devInfo.cDevCreateTime, IVS_TIME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)rspXml.SetElemValue(cDevCreateTime);

		(void)rspXml.AddElem("IsExDomian");
		(void)rspXml.SetElemValue(CToolsHelp::Int2Str(devInfo.bIsExDomain).c_str());

		rspXml.OutOfElem();
		i++;

	}
	if (iNum != 0)
	{ 
		rspXml.OutOfElem();
	}

	rspXml.OutOfElem();  

	return IVS_SUCCEED;
}

// 设备上报拼接XML
IVS_INT32 COCXDeviceXMLProcess::DeviceStateGetXML(const IVS_DEVICE_STATE* pstDeviceState, CXml& xml)
{
    CHECK_POINTER(pstDeviceState,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("DevInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("DevCode");
    (void)xml.IntoElem();
	IVS_CHAR cCode[IVS_DEV_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cCode, IVS_DEV_CODE_LEN, pstDeviceState->cCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cCode, IVS_DEV_CODE_LEN, pstDeviceState->cCode, IVS_DEV_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    (void)xml.SetElemValue(cCode);
    (void)xml.AddElem("Status");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pstDeviceState->uiStatus).c_str());
    (void)xml.AddElem("IPType");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pstDeviceState->stIP.uiIPType).c_str());
    (void)xml.AddElem("DevIP");
    (void)xml.SetElemValue(pstDeviceState->stIP.cIP);
    (void)xml.AddElem("DevPort");
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pstDeviceState->uiPort).c_str());
    xml.OutOfElem();
    xml.OutOfElem();

    return IVS_SUCCEED;
}

// 计算DevCode个数
IVS_INT32 COCXDeviceXMLProcess::GetDevCodeNum(CXml &xml)
{
    IVS_DEBUG_TRACE("");
    IVS_INT32 iNum = 0;	

    if (xml.FindElemEx("Content/DevList"))
    {        
        do 
        {
            if (!xml.FindElemValue("DevCode"))
            {
                break;
            }
            iNum++;
        } while (xml.NextElem());            

    }
    xml.OutOfElem();
    xml.OutOfElem();

    return iNum;	
}

// 获取域路由拼接XML
IVS_INT32 COCXDeviceXMLProcess::GetDomainRouteGetXML(IVS_DOMAIN_ROUTE_LIST *pList, CXml &xmlRsp )
{
    CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    IVS_UINT32 uiLen = pList->uiTotal;
    const IVS_UINT32 uiMaxElemLen = 150;
    IVS_CHAR chTemp[uiMaxElemLen];
    (void)xmlRsp.AddDeclaration("1.0","UTF-8","");
    (void)xmlRsp.AddElem("Content");
    (void)xmlRsp.AddChildElem("ResultCode");
    (void)xmlRsp.IntoElem();
    (void)xmlRsp.SetElemValue("0");
    (void)xmlRsp.AddElem("DomainRoutes");
    
    for (IVS_UINT32 uiIndex = 0; uiIndex < uiLen; uiIndex++)
    {
        if (0 == uiIndex)
        {
            xmlRsp.AddChildElem("DomainRoute");
        }
        else
        {
            xmlRsp.AddElem("DomainRoute");
        }
        xmlRsp.IntoElem();
        IVS_DOMAIN_ROUTE &stRoute = pList->stDomainRoute[uiIndex];

        xmlRsp.AddChildElem("DomainCode");
        xmlRsp.IntoElem();
        memset(chTemp, 0, uiMaxElemLen);
		if (!CToolsHelp::Memcpy(chTemp, uiMaxElemLen, stRoute.cDomainCode, IVS_DOMAIN_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiMaxElemLen, stRoute.cDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        xmlRsp.SetElemValue(chTemp);

        xmlRsp.AddElem("DomainName");
        memset(chTemp, 0, uiMaxElemLen);
		if (!CToolsHelp::Memcpy(chTemp, uiMaxElemLen, stRoute.cDomainName, IVS_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiMaxElemLen, stRoute.cDomainName, IVS_NAME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        xmlRsp.SetElemValue(chTemp);

		xmlRsp.AddElem("DomainIp");
		memset(chTemp, 0, uiMaxElemLen);
		if (!CToolsHelp::Memcpy(chTemp, uiMaxElemLen, stRoute.stIP.cIP, IVS_IP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiMaxElemLen, stRoute.stIP.cIP, IVS_IP_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        xmlRsp.SetElemValue(chTemp);

        xmlRsp.AddElem("DomainPort");  
        xmlRsp.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)stRoute.uiPort).c_str());

        xmlRsp.AddElem("DomainType");
        xmlRsp.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)stRoute.uiDomainType).c_str());

        xmlRsp.AddElem("IsProxy");
        xmlRsp.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)stRoute.bIsAgent).c_str());

        xmlRsp.AddElem("SuperDomain");
        memset(chTemp, 0, uiMaxElemLen);
		if (!CToolsHelp::Memcpy(chTemp, uiMaxElemLen, stRoute.cSuperDomain, IVS_DOMAIN_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(chTemp, uiMaxElemLen, stRoute.cSuperDomain, IVS_DOMAIN_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        xmlRsp.SetElemValue(chTemp);

		xmlRsp.AddElem("IsLocalDomain");
		xmlRsp.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)stRoute.bIsLocalDomain).c_str());

        xmlRsp.OutOfElem();
    }

    xmlRsp.IntoElem();
  
    return IVS_SUCCEED;
}

// 根据主设备查询子设备列表拼接XML
IVS_INT32 COCXDeviceXMLProcess::GetChannelListGetXML( IVS_DEV_CHANNEL_BRIEF_INFO_LIST *pList, CXml &xmlRsp, const IVS_CHAR *pDomainCode)
{
    CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    IVS_UINT32 uiLen = pList->uiTotal;
    if (uiLen > IVS_MAX_DEVICE_CHANNEL_NUM)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get ChannelList GetXML fail", "uiLen=%u", uiLen);
        return IVS_OPERATE_MEMORY_ERROR;
    }
    (void)xmlRsp.AddDeclaration("1.0","UTF-8","");
    (void)xmlRsp.AddElem("Content");
	(void)xmlRsp.AddChildElem("ResultCode");
	(void)xmlRsp.IntoElem();
	(void)xmlRsp.SetElemValue("0");
    (void)xmlRsp.AddElem("ChannelList");
    (void)xmlRsp.IntoElem();    

    IVS_CHAR chChannelType[3];
    for (IVS_UINT32 uiIndex = 0; uiIndex < uiLen; uiIndex++)
    {
        if (0 == uiIndex)
        {
            (void)xmlRsp.AddChildElem("ChannelInfo");
        }
        else
        {
            (void)xmlRsp.AddElem("ChannelInfo");
        }
        (void)xmlRsp.IntoElem();
        IVS_DEV_CHANNEL_BRIEF_INFO &stChannel = pList->stDevChannelBriefInfo[uiIndex];
        //通道编码
        (void)xmlRsp.AddChildElem("ChannelCode");
        (void)xmlRsp.IntoElem();
		std::string strBigChannelCode;
		IVS_CHAR cChannelCode[IVS_DEV_CODE_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cChannelCode, IVS_DEV_CODE_LEN, stChannel.cChannelCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cChannelCode, IVS_DEV_CODE_LEN, stChannel.cChannelCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		strBigChannelCode.append(cChannelCode).append("#").append(pDomainCode);
        (void)xmlRsp.SetElemValue(strBigChannelCode.c_str());
        //通道类型
        memset(chChannelType, 0, 3);
        CDevEnumXML::GetChannelType((IVS_INT32)stChannel.uiChannelType, chChannelType);
        (void)xmlRsp.AddElem("ChannelType");
        (void)xmlRsp.SetElemValue(chChannelType);

		(void)xmlRsp.AddElem("ChannelName");
		IVS_CHAR cChannelName[IVS_CHANNEL_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cChannelName, IVS_CHANNEL_NAME_LEN, stChannel.cChannelName, IVS_CHANNEL_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cChannelName, IVS_CHANNEL_NAME_LEN, stChannel.cChannelName, IVS_CHANNEL_NAME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xmlRsp.SetElemValue(cChannelName);

        xmlRsp.OutOfElem();
    }

    xmlRsp.OutOfElem();

    return IVS_SUCCEED;
}

// 设置前端设备密码解析XML
IVS_INT32 COCXDeviceXMLProcess::SetDevPWDParseXML(IVS_CHAR* pDevCode, IVS_CHAR* pDevUserName, IVS_CHAR* pDevPWD, IVS_CHAR* pDevRegPWD, CXml &xml)
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevUserName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevRegPWD, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(Content) fail", "NA");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	const IVS_CHAR* pTemp = NULL;
	GET_ELEM_VALUE_CHAR("DevCode", pTemp, pDevCode, IVS_DEV_CODE_LEN, xml);
	GET_ELEM_VALUE_CHAR("DevUser", pTemp, pDevUserName, IVS_NAME_LEN, xml);
	GET_ELEM_VALUE_CHAR("DevPWD", pTemp, pDevPWD, IVS_PWD_LEN, xml);
	GET_ELEM_VALUE_CHAR("DevRegPWD", pTemp, pDevRegPWD, IVS_PWD_LEN, xml);
	xml.OutOfElem();

	return IVS_SUCCEED;
}
