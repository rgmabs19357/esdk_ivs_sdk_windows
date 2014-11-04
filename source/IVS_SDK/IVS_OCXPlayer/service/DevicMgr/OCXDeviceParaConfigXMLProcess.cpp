/*****************************************************************
 filename    :    OCXDeviceParaConfigXMLProcess.cpp
 author      :    guandiqun
 created     :    2012/11/19
 description :    实现设备;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*****************************************************************/
#include "OCXDeviceParaConfigXMLProcess.h"
#include "bp_log.h"
#include "ToolsHelp.h"
#include "DevEnumXML.h"
#include "OCXXmlProcess.h"
#include "IVSCommon.h"
#include "SDKDef.h"
#include "IVS_Trace.h"

#define STR_LEN 20

COCXDeviceParaConfigXMLProcess::COCXDeviceParaConfigXMLProcess()
{

}

COCXDeviceParaConfigXMLProcess::~COCXDeviceParaConfigXMLProcess()
{

}

// 各请求名的表
COCXDeviceParaConfigXMLProcess::REQUEST_NAME COCXDeviceParaConfigXMLProcess::m_reqNames[] =
{
	{ CONFIG_DEVICE_CFG,	reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetMainDevConfigParseXml),        reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetMainDevConfigGetXML)},            
	{ CONFIG_DEVICE_NET_CFG,   reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetDevNetCfgParseXML),     reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetDevNetCfgGetXML)},
	{ CONFIG_CAMERA_CFG,     reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetCameraCfgParseXML),      reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetCameraCfgGetXML)},
	{ CONFIG_CAMERA_STREAM_CFG,   reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetCameraStreamCfgParseXML),     reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetCameraStreamCfgGetXML)},
	{ CONFIG_CAMERA_DISPLAY_CFG,    reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetCameraDisplayCfgParseXml),     reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetCameraDisplayCfgGetXml)},
	{ CONFIG_ALARM_IN_CFG,   reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetAlarmInCfgParseXML),           reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetAlarmInCfgGetXML)},
	{ CONFIG_ALARM_OUT_CFG,    reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetAlarmOutCfgParseXML),          reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetALarmOutCfgGetXML)},
	{ CONFIG_CAMERA_OSD_CFG,     reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetCameraOsgCfgParseXML),         reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetOSDConfigGetXML)},
	{ CONFIG_CAMERA_VIDEO_HIDE_ALARM_CFG,    reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetVideHideAlarmParseXML),        reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetVideoHideAlarmGetXML)},
	{ CONFIG_CAMERA_IMAGING_CFG,      reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetCameraImgCfgParseXML),         reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetCameraImgCfgGetXML)},
	{ CONFIG_CAMERA_VIDEO_MASK_CFG,     reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetVideoMaskConfigParseXml),      reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetVideoMaskConfigGetXml)},
	{ CONFIG_DEVICE_PTZ_CFG,     reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetCameraPTZCfgParseXml),         reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetPTZCfgGetXML)},
	{ CONFIG_CAMERA_AUDIO_CFG,     reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetAudioCfgParseXml),       reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetAudioCfgGetXML)},
	{ CONFIG_CAMERA_MOTION_DETECTION_CFG,    reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetMotionDetecParseXML),      reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetMotioinDetecGetXML)},
	{ CONFIG_DEVICE_TIME_CFG,    reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetDevTimeCfgParseXML),       reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetDevTimeCfgGetXML)},
	{ CONFIG_SERIAL_CHANNEL_CFG,    reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetSerialChannelCfgParseXML),     reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetSerialChannelCfgGetXML)},
	{ CONFIG_CAMERA_EXTEND_CFG,     reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetCameraExCfgParseXML),       reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetCameraExCfgGetXML)},
	{ CONFIG_CAMERA_SNAPSHOT_CFG,    reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetCameraSnapshotCfgParseXML),    reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetCameraSnapshotCfgGetXML)},
	{ CONFIG_CAMERA_RECORD_STREAM_CFG,    reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetCameraRecordStreamCfgParseXML),   reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetCameraRecordStreamCfgGetXML)},
	{ CONFIG_CAMERA_FEC_CFG,     reinterpret_cast<FnSetDeviceParaParseXML>(&COCXDeviceParaConfigXMLProcess::SetCameraFECCfgParseXML),      reinterpret_cast<FnGetDeviceParaGetXML>(&COCXDeviceParaConfigXMLProcess::GetCameraFECCfgGetXML)}
}; 


//根据REQ_ID获取处理函数指针
COCXDeviceParaConfigXMLProcess::FnSetDeviceParaParseXML COCXDeviceParaConfigXMLProcess::GetFnSetDevicePara(uint32_t enReqID)const
{
	FnSetDeviceParaParseXML fn = NULL;
	int iSize = sizeof(m_reqNames) / sizeof(REQUEST_NAME);

	for ( int i = 0; i < iSize; i++ )
	{
		if ( m_reqNames[i].reqID == enReqID )
		{
			fn = m_reqNames[i].fnSetDevicePara;
			break;
		}
	}
	return fn;
}


//根据REQ_ID获取处理函数指针
COCXDeviceParaConfigXMLProcess::FnGetDeviceParaGetXML COCXDeviceParaConfigXMLProcess::GetFnGetDevicePara(uint32_t enReqID)const
{
	FnGetDeviceParaGetXML fn = NULL;
	int iSize = sizeof(m_reqNames) / sizeof(REQUEST_NAME);

	for ( int i = 0; i < iSize; i++ )
	{
		if ( m_reqNames[i].reqID == enReqID )
		{
			fn = m_reqNames[i].fnGetDevicePara;
			break;
		}
	}
	return fn;
}

// 设置设备参数统一解析XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetDeviceConfigParseXml(IVS_VOID* &pBuffer, IVS_UINT32 uiConfigType, CXml &xml)
{
	IVS_DEBUG_TRACE("");

	FnSetDeviceParaParseXML fnSetProcess = GetFnSetDevicePara(uiConfigType);
	if (NULL == fnSetProcess)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "no process fn","iRet = %d", IVS_FAIL);
		return IVS_FAIL;
	}
	return (this->*fnSetProcess)(pBuffer, xml);
}//lint !e1762

// 查询设备参数统一拼接XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetDeviceConfigGetXml(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, IVS_UINT32 uiConfigType, CXml &xml)
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	FnGetDeviceParaGetXML fnGetProcess = GetFnGetDevicePara(uiConfigType);
	if (NULL == fnGetProcess)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "no process fn","iRet = %d", IVS_FAIL);
		return IVS_FAIL;
	}
	return (this->*fnGetProcess)(pBuffer, pDevCode, xml);
}//lint !e1762



// 设置主设备基本参数解析XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetMainDevConfigParseXml(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

	IVS_DEVICE_CFG *pDevCfg = reinterpret_cast<IVS_DEVICE_CFG*>(pBuffer);
    if (!xml.FindElemEx("Content/DevInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/DevInfo)", "NA");
        return IVS_XML_INVALID;
    }
    const IVS_INT32 maxLen = 120;
	IVS_CHAR chTemp[maxLen] = {0};
    const IVS_CHAR* szElemValue = NULL;
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
    //主设备编码
    GET_ELEM_VALUE_CHAR("DevCode", szElemValue, chTemp, maxLen, xml);
	IVS_INT32 iRet = COCXXmlProcess::ParseDevCode(chTemp, chDevCode, chDomaCode);
	if (IVS_SUCCEED == iRet)
	{
		if (!CToolsHelp::Memcpy(pDevCfg->stDeviceInfo.cCode, IVS_DEV_CODE_LEN, chDevCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "chDevCode memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
	}
	else
	{
		if (!CToolsHelp::Memcpy(pDevCfg->stDeviceInfo.cCode, IVS_DEV_CODE_LEN, chTemp, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "chTemp memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
	}
    //主设备设备名称
    GET_ELEM_VALUE_CHAR("DevName", szElemValue, pDevCfg->stDeviceInfo.cName, IVS_DEVICE_NAME_LEN, xml);
    //主设备协议类型
    GET_ELEM_VALUE_CHAR("DevProtocolType", szElemValue, pDevCfg->cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN, xml);
    //认证类型
    GET_ELEM_VALUE_NUM_FOR_UINT("DevLoginType", szElemValue, pDevCfg->iLoginType, xml);
    //主设备提供商类型
    GET_ELEM_VALUE_CHAR("DevVendorType", szElemValue, pDevCfg->stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, xml);
    //主设备形态类型
    memset(chTemp, 0, maxLen);
    GET_ELEM_VALUE_CHAR("DevFormType", szElemValue, chTemp, maxLen, xml);
    pDevCfg->stDeviceInfo.uiType = static_cast<IVS_UINT32>(CDevEnumXML::GetDevTypeEnum(chTemp));
    //主设备型号
    GET_ELEM_VALUE_CHAR("DevModelType", szElemValue, pDevCfg->stDeviceInfo.cModel, IVS_DEV_MODEL_LEN, xml);
	//主设备SN号
	GET_ELEM_VALUE_CHAR("DevSN", szElemValue, pDevCfg->stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN, xml);
    //设备告警使能
	GET_ELEM_VALUE_NUM("DevAlarmEnable", szElemValue, pDevCfg->bEnableAlarm, xml);

    //视频制式
    memset(chTemp, 0, maxLen);
    GET_ELEM_VALUE_CHAR("VideoFormat", szElemValue, chTemp, maxLen, xml);
    pDevCfg->uiVideoFormat = (IVS_UINT32)CDevEnumXML::GetVideoFormatEnum(chTemp);
    //是否直连优先
    GET_ELEM_VALUE_NUM_FOR_UINT("DirectConnectFirst", szElemValue, pDevCfg->uiDirectConnectFirst, xml);
    //设备所属NVR编码
    GET_ELEM_VALUE_CHAR("NVRCode", szElemValue, pDevCfg->cNvrCode, IVS_DEV_CODE_LEN, xml);
    //设备所属TAS编码
    GET_ELEM_VALUE_CHAR("DCGCode", szElemValue, pDevCfg->cTasCode, IVS_DEV_CODE_LEN, xml);
    //设备登录用户名
    GET_ELEM_VALUE_CHAR("DevUser", szElemValue, pDevCfg->cDevUser, IVS_NAME_LEN, xml);
    //设备登录密码
    GET_ELEM_VALUE_CHAR("DevPWD", szElemValue, pDevCfg->cDevPasswd, IVS_PWD_LEN, xml);
    //设备向平台密码
    GET_ELEM_VALUE_CHAR("DevRegPWD", szElemValue, pDevCfg->cDevRegPasswd, IVS_PWD_LEN, xml);
    //是否支持集群调度
    GET_ELEM_VALUE_NUM("BlanceSupport", szElemValue, pDevCfg->bEnableSchedule, xml);   

    xml.OutOfElem();
    xml.OutOfElem();  

    return IVS_SUCCEED;
}

// 查询主设备基本参数拼接XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetMainDevConfigGetXML(IVS_VOID* &pBuffer, const IVS_CHAR *pDevCode, CXml& xml)const
{
    CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_DEVICE_CFG *pMainDevCfg = reinterpret_cast<IVS_DEVICE_CFG *>(pBuffer);
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = COCXXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Parse DevCode fail", "(ParseDevCode)iRet=%d", iRet);
		return iRet;
	}

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");
	(void)xml.AddElem("DevInfo");
	(void)xml.AddChildElem("DevCode");
	(void)xml.IntoElem();

	std::string strBigCode;
	IVS_CHAR cCode[IVS_DEV_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cCode, IVS_DEV_CODE_LEN, pMainDevCfg->stDeviceInfo.cCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cCode, IVS_DEV_CODE_LEN, pMainDevCfg->stDeviceInfo.cCode, IVS_DEV_CODE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	strBigCode.append(cCode).append("#").append(chDomaCode);
	(void)xml.SetElemValue(strBigCode.c_str());
	(void)xml.AddElem("DevName");
	IVS_CHAR cName[IVS_DEVICE_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cName, IVS_DEVICE_NAME_LEN, pMainDevCfg->stDeviceInfo.cName, IVS_DEVICE_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pMainDevCfg->stDeviceInfo.cName memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cName);
	(void)xml.AddElem("DevProtocolType");
	IVS_CHAR cDevProtocolType[IVS_MAX_PROTOCOLTYPE_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cDevProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN, pMainDevCfg->cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN, pMainDevCfg->cProtocolType, IVS_MAX_PROTOCOLTYPE_NAME_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cDevProtocolType);
	(void)xml.AddElem("DevLoginType");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pMainDevCfg->iLoginType).c_str());
	(void)xml.AddElem("DevVendorType");
	IVS_CHAR cDevVendorType[IVS_MAX_VENDOR_TYPE_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cDevVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, pMainDevCfg->stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, pMainDevCfg->stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cDevVendorType);
	(void)xml.AddElem("DevFormType");
	(void)xml.SetElemValue(CDevEnumXML::GetDevType((IVS_INT32)pMainDevCfg->stDeviceInfo.uiType));
	(void)xml.AddElem("DevModelType");
	IVS_CHAR cModel[IVS_DEV_MODEL_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cModel, IVS_DEV_MODEL_LEN, pMainDevCfg->stDeviceInfo.cModel, IVS_DEV_MODEL_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pMainDevCfg->stDeviceInfo.cModel memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cModel);
	(void)xml.AddElem("DevSN");
	IVS_CHAR cDevSN[IVS_DEV_SERIALNO_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cDevSN, IVS_DEV_SERIALNO_LEN, pMainDevCfg->stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pMainDevCfg->stDeviceInfo.cSerialNumber memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cDevSN);
	(void)xml.AddElem("DevAlarmEnable");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((pMainDevCfg->bEnableAlarm)).c_str());
	(void)xml.AddElem("VideoFormat");
	(void)xml.SetElemValue(CDevEnumXML::GetVideoFormatType((IVS_INT32) pMainDevCfg->uiVideoFormat));
	(void)xml.AddElem("DirectConnectFirst");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pMainDevCfg->uiDirectConnectFirst).c_str());
	(void)xml.AddElem("NVRCode");
	IVS_CHAR cNvrCode[IVS_DEV_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cNvrCode, IVS_DEV_CODE_LEN, pMainDevCfg->cNvrCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pMainDevCfg->cNvrCode memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cNvrCode);
	(void)xml.AddElem("DCGCode");
	IVS_CHAR cTasCode[IVS_DEV_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cTasCode, IVS_DEV_CODE_LEN, pMainDevCfg->cTasCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pMainDevCfg->cTasCode memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cTasCode);
	(void)xml.AddElem("DevUser");
	IVS_CHAR cDevUser[IVS_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cDevUser, IVS_NAME_LEN, pMainDevCfg->cDevUser, IVS_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pMainDevCfg->cDevUser memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cDevUser);
	(void)xml.AddElem("DevPWD");
	IVS_CHAR cDevPasswd[IVS_PWD_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cDevPasswd, IVS_PWD_LEN, pMainDevCfg->cDevPasswd, IVS_PWD_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pMainDevCfg->cDevPasswd memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cDevPasswd);
	(void)xml.AddElem("DevRegPWD");
	IVS_CHAR cDevRegPasswd[IVS_PWD_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cDevRegPasswd, IVS_PWD_LEN, pMainDevCfg->cDevRegPasswd, IVS_PWD_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pMainDevCfg->cDevRegPasswd memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cDevRegPasswd);
	(void)xml.AddElem("BlanceSupport");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pMainDevCfg->bEnableSchedule).c_str());
	(void)xml.AddElem("MaxVideoNum");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pMainDevCfg->stDeviceChannel.uiMaxVideoNum).c_str());
	(void)xml.AddElem("MaxAudioInNum");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pMainDevCfg->stDeviceChannel.uiMaxAudioInNum).c_str());
	(void)xml.AddElem("MaxAudioOutNum");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pMainDevCfg->stDeviceChannel.uiMaxAudioOutNum).c_str());
	(void)xml.AddElem("MaxAlarmInNum");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pMainDevCfg->stDeviceChannel.uiMaxAlarmInNum).c_str());
	(void)xml.AddElem("MaxAlarmOutNum");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pMainDevCfg->stDeviceChannel.uiMaxAlarmOutNum).c_str());
	(void)xml.AddElem("MaxSerialNum");
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pMainDevCfg->stDeviceChannel.uiMaxSerialNum).c_str());
	// modify by limingjie lWX175483 2013-7-30 增加一个字段 DTS2013072504738
	(void)xml.AddElem("ParentCode");
	IVS_CHAR cParentCode[IVS_DEV_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cParentCode, IVS_DEV_CODE_LEN, pMainDevCfg->stDeviceInfo.cParentCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pMainDevCfg->cParentCode memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cParentCode);
	xml.OutOfElem();
	xml.OutOfElem();

    return IVS_SUCCEED;
}

// 查询摄像机码流参数拼接XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetCameraStreamCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_STREAM_CFG* pCameraStrCfg = reinterpret_cast<IVS_CAMERA_STREAM_CFG *>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");
	(void)xml.AddElem("VideoInChanInfo");
	(void)xml.IntoElem();


	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDevCode);

	(void)xml.AddElem("StreamNum");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraStrCfg->uiStreamInfoNum)).c_str());

	(void)xml.AddElem("StreamList");
	(void)xml.IntoElem();
	IVS_UINT32 i = 0;
	while(i < pCameraStrCfg->uiStreamInfoNum)
	{
		i == 0 ? (void)xml.AddChildElem("StreamInfo"):(void)xml.AddElem("StreamInfo");
		(void)xml.IntoElem();
		(void)xml.AddChildElem("StreamType");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraStrCfg->stStreamInfo[i].uiStreamType)).c_str());

		(void)xml.AddElem("EncodeMode");
		(void)xml.IntoElem();
		const IVS_CHAR* pEncodeType = CDevEnumXML::GetEnCodeModeCH(static_cast<IVS_INT32>(pCameraStrCfg->stStreamInfo[i].uiEncodeType));
		(void)xml.SetElemValue(pEncodeType);

		(void)xml.AddElem("PicQuality");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraStrCfg->stStreamInfo[i].uiPicQuality)).c_str());

		(void)xml.AddElem("Resolution");
		(void)xml.IntoElem();
		IVS_CHAR cResolution[IVS_RESOLUTION_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cResolution, IVS_RESOLUTION_LEN, pCameraStrCfg->stStreamInfo[i].cResolution, IVS_RESOLUTION_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cResolution, IVS_RESOLUTION_LEN, pCameraStrCfg->stStreamInfo->cResolution, IVS_RESOLUTION_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		//const IVS_CHAR* pResolution = CDevEnumXML::GetResolutionCH(static_cast<IVS_INT32>(pCameraStrCfg->stStreamInfo[i].uiResolution));
		(void)xml.SetElemValue(cResolution);

		(void)xml.AddElem("BitRate");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraStrCfg->stStreamInfo[i].uiBitRate)).c_str());

		(void)xml.AddElem("BitRateType");
		(void)xml.IntoElem();
		const IVS_CHAR* pBitRateType = CDevEnumXML::GetBitRateTypeCH(static_cast<IVS_INT32>(pCameraStrCfg->stStreamInfo[i].uiBitRateType));
		(void)xml.SetElemValue(pBitRateType);

		(void)xml.AddElem("FrameRate");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraStrCfg->stStreamInfo[i].uiFrameRate)).c_str());

		(void)xml.AddElem("Profile");
		(void)xml.IntoElem();
		const IVS_CHAR* pProfile = NULL;
		if (1 == pCameraStrCfg->stStreamInfo[i].uiEncodeType)
		{
			pProfile = CDevEnumXML::GetH264ProfileCH(static_cast<IVS_INT32>(pCameraStrCfg->stStreamInfo[i].uiProfile));
		}
		else if (2 == pCameraStrCfg->stStreamInfo[i].uiEncodeType)
		{
			pProfile = CDevEnumXML::GetMjpegProfileCH(static_cast<IVS_INT32>(pCameraStrCfg->stStreamInfo[i].uiProfile));
		}
		else
		{
			pProfile = "";
		}
		(void)xml.SetElemValue(pProfile);

		(void)xml.AddElem("IframeInterval");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraStrCfg->stStreamInfo[i].uiFrameInterval)).c_str());

		(void)xml.AddElem("TransProtocol");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraStrCfg->stStreamInfo[i].uiTransProtocol)).c_str());

		(void)xml.AddElem("PKTProtocol");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraStrCfg->stStreamInfo[i].uiPktProtocol)).c_str());

		i++;
		xml.OutOfElem();
	}
	xml.OutOfElem();
	
	return IVS_SUCCEED;
}

// 修改摄像机码流参数解析XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetCameraStreamCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_STREAM_CFG* pCameraStrCfg = reinterpret_cast<IVS_CAMERA_STREAM_CFG *>(pBuffer);
	if (!xml.FindElemEx("Content/VideoInChanInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/VideoInChanInfo) fail", "NA");
		return IVS_XML_INVALID;
	}

	const char* pTemp = NULL;
	IVS_INT32 iTemp = 0;
	GET_ELEM_VALUE_NUM("StreamNum", pTemp, iTemp, xml);
	pCameraStrCfg->uiStreamInfoNum = static_cast<IVS_UINT32>(iTemp);

	if (!xml.FindElem("StreamList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "no element StreamList", "NA");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();

	if (!xml.FindElem("StreamInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "no element StreamInfo", "NA");
		return IVS_XML_INVALID;
	}

	IVS_INT32 i = 0;
	const IVS_INT32 maxLen = 120;
	do 
	{
		(void)xml.IntoElem();
		GET_ELEM_VALUE_NUM("StreamType", pTemp, iTemp, xml);
		pCameraStrCfg->stStreamInfo[i].uiStreamType = static_cast<IVS_UINT32>(iTemp);

		IVS_CHAR cEncodeMode[maxLen] = {0};
		GET_ELEM_VALUE_CHAR("EncodeMode", pTemp, cEncodeMode, maxLen, xml);
		pCameraStrCfg->stStreamInfo[i].uiEncodeType = static_cast<IVS_UINT32>(CDevEnumXML::GetEnCodeModeEnum(cEncodeMode));

		GET_ELEM_VALUE_NUM("PicQuality", pTemp, iTemp, xml);
		pCameraStrCfg->stStreamInfo[i].uiPicQuality = static_cast<IVS_UINT32>(iTemp);

		//IVS_CHAR cResolution[maxLen] = {0};
		GET_ELEM_VALUE_CHAR("Resolution", pTemp, pCameraStrCfg->stStreamInfo[i].cResolution, IVS_RESOLUTION_LEN, xml);
		//pCameraStrCfg->stStreamInfo[i].uiResolution = static_cast<IVS_UINT32>(CDevEnumXML::GetResolutionEnum(cResolution));

		GET_ELEM_VALUE_NUM("BitRate", pTemp, iTemp, xml);
		pCameraStrCfg->stStreamInfo[i].uiBitRate = static_cast<IVS_UINT32>(iTemp);

		IVS_CHAR cBitRateType[maxLen] = {0};
		GET_ELEM_VALUE_CHAR("BitRateType", pTemp, cBitRateType, maxLen, xml);
		pCameraStrCfg->stStreamInfo[i].uiBitRateType = static_cast<IVS_UINT32>(CDevEnumXML::GetBitRateTypeEnum(cBitRateType));

		GET_ELEM_VALUE_NUM("FrameRate", pTemp, iTemp, xml);
		pCameraStrCfg->stStreamInfo[i].uiFrameRate = static_cast<IVS_UINT32>(iTemp);

		IVS_CHAR cProfile[maxLen] = {0};
		GET_ELEM_VALUE_CHAR("Profile", pTemp, cProfile, maxLen, xml);
		if (1 == pCameraStrCfg->stStreamInfo[i].uiEncodeType)
		{
			pCameraStrCfg->stStreamInfo[i].uiProfile = static_cast<IVS_UINT32>(CDevEnumXML::GetH264ProfileEnum(cProfile));
		}
		if (2 == pCameraStrCfg->stStreamInfo[i].uiEncodeType)
		{
			pCameraStrCfg->stStreamInfo[i].uiProfile = static_cast<IVS_UINT32>(CDevEnumXML::GetMjpegProfileEnum(cProfile));
		}
		

		GET_ELEM_VALUE_NUM("IframeInterval", pTemp, iTemp, xml);
		pCameraStrCfg->stStreamInfo[i].uiFrameInterval = static_cast<IVS_UINT32>(iTemp);

		GET_ELEM_VALUE_NUM("TransProtocol", pTemp, iTemp, xml);
		pCameraStrCfg->stStreamInfo[i].uiTransProtocol = static_cast<IVS_UINT32>(iTemp);

		GET_ELEM_VALUE_NUM("PKTProtocol", pTemp, iTemp, xml);
		pCameraStrCfg->stStreamInfo[i].uiPktProtocol = static_cast<IVS_UINT32>(iTemp);

		i++;
		xml.OutOfElem();
	} while (xml.NextElem());
	xml.OutOfElem();

	if (i >= IVS_MAX_STREAM_NUM)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Set CameraStreamCfg ParseXML fail", "i >= IVS_MAX_STREAM_NUM");
		return IVS_XML_INVALID;
	}

	return IVS_SUCCEED;
}

// 查询摄像机基本参数解析xml
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetCameraCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_CFG* pCameraCfg = reinterpret_cast<IVS_CAMERA_CFG *>(pBuffer);
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = COCXXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Parse DevCode fail", "(ParseDevCode)iRet=%d", iRet);
		return iRet;
	}

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("VideoInChanInfo");
	(void)xml.IntoElem();
	//增加CameraCode
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDevCode);

	//增加ParentCode
	(void)xml.AddElem("ParentCode");
	(void)xml.IntoElem();
	IVS_CHAR cCode[IVS_DEV_CODE_LEN + 1];
	memset(cCode,0,IVS_DEV_CODE_LEN + 1);
	if (!CToolsHelp::Memcpy(cCode, IVS_DEV_CODE_LEN, pCameraCfg->cParentCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pCameraCfg->cParentCode memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cCode);

	//增加VideoInIndex
	(void)xml.AddElem("VideoInIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraCfg->uiVideoChannel)).c_str());

	//增加CameraType
	(void)xml.AddElem("CameraType");
	(void)xml.IntoElem();
	switch (pCameraCfg->uiCameraType)
	{
	case CAMERA_FIX:
		{
			(void)xml.SetElemValue("FIX");
			break;
		}
	case CAMERA_PTZ:
		{
			(void)xml.SetElemValue("PTZ");
			break;
		}
	case CAMERA_DOME:
		{
			(void)xml.SetElemValue("DOME");
			break;
		}
	case CAMERA_FIX_DOME:
		{
			(void)xml.SetElemValue("FIX_DOME");
			break;
		}
	default:
		{
			(void)xml.SetElemValue("FIX");
			break;
		}
	}

	//增加IsBroadcast
	(void)xml.AddElem("IsBroadcast");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pCameraCfg->bSupportBroadcast).c_str());

	//增加BroadcastIPType
	/*
	组播地址IP类型
	0：IPV4
	1：IPV6
	*/
	(void)xml.AddElem("BroadcastIPType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraCfg->stBroadcast.stIP.uiIPType)).c_str());

	//增加BroadcastIP
	(void)xml.AddElem("BroadcastIP");
	(void)xml.IntoElem();
	IVS_CHAR cBroadcastIP[IVS_IP_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cBroadcastIP, IVS_IP_LEN, pCameraCfg->stBroadcast.stIP.cIP, IVS_IP_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cBroadcastIP, IVS_IP_LEN, pCameraCfg->stBroadcast.stIP.cIP, IVS_IP_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cBroadcastIP);

	//增加BroadcastPort
	(void)xml.AddElem("BroadcastPort");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraCfg->stBroadcast.uiPort)).c_str());

	//增加BroadcastTTL
	(void)xml.AddElem("BroadcastTTL");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraCfg->stBroadcast.uiTTL)).c_str());

	//增加Longitude
	(void)xml.AddElem("Longitude");
	(void)xml.IntoElem();
	IVS_CHAR cLongitude[STR_LEN+1] = {0};
	if (!CToolsHelp::Snprintf(cLongitude, STR_LEN, "%.12f", pCameraCfg->stPosition.dLongitude))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cLongitude, STR_LEN, '%.12f', pCameraCfg->stPosition.dLongitude) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cLongitude);
	//(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraCfg->stPosition.uiLongitude)).c_str());

	//增加Latitude
	(void)xml.AddElem("Latitude");
	(void)xml.IntoElem();
	IVS_CHAR cLatitude[STR_LEN+1] = {0};
	if (!CToolsHelp::Snprintf(cLatitude, STR_LEN, "%.12f", pCameraCfg->stPosition.dLatitude))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cLatitude, STR_LEN, '%.12f', pCameraCfg->stPosition.dLatitude) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cLatitude);
	//(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraCfg->stPosition.uiLatitude)).c_str());

	//增加CameraLocation
	(void)xml.AddElem("CameraLocation");
	(void)xml.IntoElem();
	IVS_CHAR cCameraLocation[IVS_DESCRIBE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cCameraLocation, IVS_DESCRIBE_LEN, pCameraCfg->stPosition.cCameraLocation, IVS_DESCRIBE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cCameraLocation, IVS_DESCRIBE_LEN, pCameraCfg->stPosition.cCameraLocation, IVS_DESCRIBE_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cCameraLocation);

	//增加Height
	(void)xml.AddElem("Height");
	(void)xml.IntoElem();
	IVS_CHAR cHeight[STR_LEN] = {0};
	if (!CToolsHelp::Snprintf(cHeight, STR_LEN, "%.2f", pCameraCfg->stPosition.dHeight))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cHeight, STR_LEN, '%.2f', pCameraCfg->stPosition.dHeight) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cHeight);

	//增加CameraName
	(void)xml.AddElem("CameraName");
	(void)xml.IntoElem();
	IVS_CHAR cCameraName[IVS_CAMERA_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cCameraName, IVS_CAMERA_NAME_LEN, pCameraCfg->cCameraName, IVS_CAMERA_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memcpy(cCameraName, IVS_CAMERA_NAME_LEN, pCameraCfg->cCameraName, IVS_CAMERA_NAME_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cCameraName);

	//增加VoiceInEnable
	(void)xml.AddElem("VoiceInEnable");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pCameraCfg->bEnableVoice?"1":"0");

	// VoiceInCode
	(void)xml.AddElem("VoiceOutCode");
	(void)xml.IntoElem();
	std::string strVoiceOutCode;
	IVS_CHAR cVoiceOutCode[IVS_DEV_CODE_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cVoiceOutCode, IVS_DEV_CODE_LEN, pCameraCfg->cVoiceOutCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cVoiceInCode, IVS_DEV_CODE_LEN, pCameraCfg->cVoiceInCode, cVoiceInCode) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	strVoiceOutCode.append(cVoiceOutCode);
	strVoiceOutCode.append("#");
	strVoiceOutCode.append(chDomaCode);
	(void)xml.SetElemValue(strVoiceOutCode.c_str());

	xml.OutOfElem();

	return IVS_SUCCEED;
}

//"查询摄像机显示参数" 响应处理 由结构体生成xml
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetCameraDisplayCfgGetXml(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_DISPLAY_CFG* pCameraDisplayCfg = reinterpret_cast<IVS_CAMERA_DISPLAY_CFG*>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	//增加CameraCode
	(void)xml.AddElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDevCode);

	//增加ImagingInfo
	(void)xml.AddElem("ImagingInfo");
	(void)xml.IntoElem();
	{
		//增加BasicInfo
		(void)xml.AddChildElem("BasicInfo");
		(void)xml.IntoElem();
		{
			//增加Contrast
			(void)xml.AddChildElem("Contrast");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraDisplayCfg->uiContrast)).c_str());

			//增加Bright
			(void)xml.AddElem("Bright");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraDisplayCfg->uiBrightness)).c_str());

			//增加Hue
			(void)xml.AddElem("Hue");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraDisplayCfg->uiHue)).c_str());

			//增加Saturation
			(void)xml.AddElem("Saturation");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraDisplayCfg->uiSaturation)).c_str());
			xml.OutOfElem();
		}
		xml.OutOfElem();
	}
	xml.OutOfElem();

	return IVS_SUCCEED;
}

//查询摄像机云台参数结构体解析成xml
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetPTZCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_PTZ_CFG* pCameraPTZCfg = reinterpret_cast<IVS_PTZ_CFG*>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	//增加PTZInfo
	(void)xml.AddElem("PTZInfo");
	(void)xml.IntoElem();
	{
		//增加DecoderType
		(void)xml.AddChildElem("DecoderType");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraPTZCfg->uiDecoderType)).c_str());
		
		//增加DecoderAddress
		(void)xml.AddElem("DecoderAddress");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraPTZCfg->uiDecoderAddress)).c_str());

		//PosNegDirectControl
		(void)xml.AddElem("PosNegDirectControl");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(pCameraPTZCfg->bPosNegDirectControl?"BACK":"FORWARD");

		//SerialCode
		(void)xml.AddElem("SerialCode");
		(void)xml.IntoElem();
		IVS_CHAR cSerialCode[IVS_DEV_SERIALNO_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cSerialCode, IVS_DEV_SERIALNO_LEN, pCameraPTZCfg->cSerialCode, IVS_DEV_SERIALNO_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cSerialCode, IVS_DEV_SERIALNO_LEN, pCameraPTZCfg->cSerialCode, IVS_DEV_SERIALNO_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cSerialCode);
		xml.OutOfElem();
	}
	xml.OutOfElem();

	return IVS_SUCCEED;
}

//查询语音子设备参数，结构体解析成xml
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetAudioCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_AUDIO_CFG* pAudioCfg = reinterpret_cast<IVS_AUDIO_CFG*>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	//增加VoiceChanInfo
	(void)xml.AddElem("VoiceChanInfo");
	(void)xml.IntoElem();
	
	//VoiceName
	(void)xml.AddChildElem("VoiceName");
	(void)xml.IntoElem();
	IVS_CHAR cVoiceName[IVS_AUDIO_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cVoiceName, IVS_AUDIO_NAME_LEN, pAudioCfg->cAudioName, IVS_AUDIO_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cVoiceName, IVS_AUDIO_NAME_LEN, pAudioCfg->cAudioName, IVS_AUDIO_NAME_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cVoiceName);

	//ChannelType
	(void)xml.AddElem("ChannelType");
	(void)xml.IntoElem();
	switch (pAudioCfg->uiChannelType)
	{
	case 0:
		{
			(void)xml.SetElemValue("INPUT");
			break;
		}
	case 1:
		{
			(void)xml.SetElemValue("OUTPUT");
			break;
		}
	default:
		{
			IVS_INT32 iRet = IVS_XML_INVALID;
			BP_RUN_LOG_ERR(iRet, "Get AudioCfg GetXML fail","Error ChannelType");
			break;
		}

	}
		
	//VoiceIndex
	(void)xml.AddElem("VoiceIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAudioCfg->uiAudioIndex)).c_str());

	//VoiceParam
	(void)xml.AddElem("VoiceParam");
	(void)xml.IntoElem();
		
	//Pitch
	(void)xml.AddChildElem("Pitch");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAudioCfg->stAudioParam.uiPitch)).c_str());

	//Volume
	(void)xml.AddElem("Volume");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAudioCfg->stAudioParam.uiVolume)).c_str());

	//InputType
	(void)xml.AddElem("InputType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAudioCfg->stAudioParam.uiInputType)).c_str());

	//DataFormat
	(void)xml.AddElem("DataFormat");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CDevEnumXML::GetDataFormatCHAR(static_cast<IVS_INT32>(pAudioCfg->stAudioParam.uiDataFormat)));

	//SampleIngrate
	(void)xml.AddElem("SampleIngrate");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAudioCfg->stAudioParam.uiSampleRate)).c_str());

	//Channel
	(void)xml.AddElem("Channel");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAudioCfg->stAudioParam.uiChannel)).c_str());

	//Gain
	(void)xml.AddElem("Gain");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAudioCfg->stAudioParam.uiGain)).c_str());

	//SampleBits
	(void)xml.AddElem("SampleBits");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAudioCfg->stAudioParam.uiSampleBits)).c_str());

	xml.OutOfElem();
				
	xml.OutOfElem();
	
	xml.OutOfElem();

	return IVS_SUCCEED;
}

//设置摄像机基本参数解析xml,生成IVS_CAMERA_CFG
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetCameraCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_CFG* pCameraCfg = reinterpret_cast<IVS_CAMERA_CFG *>(pBuffer);
	if (!xml.FindElemEx("Content/VideoInChanInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/VideoInChanInfo) fail", "NA");
		return IVS_XML_INVALID;
	}

	(void)xml.IntoElem();

	const IVS_CHAR* szElemValue = NULL;
	IVS_INT32 iTemp = 0;
	//摄像机编码
	GET_ELEM_VALUE_CHAR("CameraCode", szElemValue, pCameraCfg->cCameraCode, IVS_DEV_CODE_LEN, xml);
	//摄像机类型
	const int tempLegth = 10;
	IVS_CHAR tempChar[tempLegth]={0};
	GET_ELEM_VALUE_CHAR("CameraType",szElemValue,tempChar,tempLegth,xml);
	tempChar[tempLegth-1]=0;
	if(0 == strcmp(tempChar,"FIX"))//FIX，默认为FIX
	{
		pCameraCfg->uiCameraType = CAMERA_FIX;
	}
	else if(0 == strcmp(tempChar,"PTZ"))
	{
		pCameraCfg->uiCameraType = CAMERA_PTZ;
	}
	else if(0 == strcmp(tempChar,"DOME"))
	{
		pCameraCfg->uiCameraType = CAMERA_DOME;
	}
	else if(0 == strcmp(tempChar,"FIX_DOME"))
	{
		pCameraCfg->uiCameraType = CAMERA_FIX_DOME;
	}
	else
	{
		IVS_INT32 iRet = IVS_PARA_INVALID;
		BP_RUN_LOG_ERR(iRet, "Set CameraBriefInfoCfg ParseXML fail","Invalid CameraType");
		return iRet;
	}

	//是否支持组播IsBroadcast
	GET_ELEM_VALUE_NUM("IsBroadcast", szElemValue, iTemp, xml);
	pCameraCfg->bSupportBroadcast = static_cast<IVS_INT32>(iTemp);
	//组播地址IP类型
	GET_ELEM_VALUE_NUM("BroadcastIPType", szElemValue, iTemp, xml);
	pCameraCfg->stBroadcast.stIP.uiIPType = static_cast<IVS_UINT32>(iTemp);
	//组播地址BroadcastIP
	GET_ELEM_VALUE_CHAR("BroadcastIP", szElemValue, pCameraCfg->stBroadcast.stIP.cIP, IVS_IP_LEN, xml);
	//组播端口BroadcastPort
	GET_ELEM_VALUE_NUM("BroadcastPort", szElemValue, iTemp, xml);
	pCameraCfg->stBroadcast.uiPort = static_cast<IVS_UINT32>(iTemp);
	//组播TTL BroadcastTTL
	GET_ELEM_VALUE_NUM("BroadcastTTL", szElemValue, iTemp, xml);
	pCameraCfg->stBroadcast.uiTTL = static_cast<IVS_UINT32>(iTemp);
	//经度Longitude
	GET_ELEM_VALUE_DOUBLE("Longitude", szElemValue, pCameraCfg->stPosition.dLongitude, xml);

	/*GET_ELEM_VALUE_NUM("Longitude", szElemValue, iTemp, xml);
	pCameraCfg->stPosition.uiLongitude = static_cast<IVS_UINT32>(iTemp);*/
	//纬度Latitude
	GET_ELEM_VALUE_DOUBLE("Latitude", szElemValue, pCameraCfg->stPosition.dLatitude, xml);
	/*GET_ELEM_VALUE_NUM("Latitude", szElemValue, iTemp, xml);
	pCameraCfg->stPosition.uiLatitude = static_cast<IVS_UINT32>(iTemp);*/
	//摄像机安装位置CameraLocation
	GET_ELEM_VALUE_CHAR("CameraLocation", szElemValue, pCameraCfg->stPosition.cCameraLocation, IVS_DESCRIBE_LEN, xml);
	//高度Height
	GET_ELEM_VALUE_DOUBLE("Height", szElemValue, pCameraCfg->stPosition.dHeight, xml);

	//摄像机名称CameraName
	GET_ELEM_VALUE_CHAR("CameraName", szElemValue, pCameraCfg->cCameraName, IVS_CAMERA_NAME_LEN, xml);
	//随路音频是否启用VoiceInEnable on =1;off = 0
	const int LENGTH =3;
	IVS_CHAR tempChar2[LENGTH]={0};
	GET_ELEM_VALUE_CHAR("VoiceInEnable", szElemValue, tempChar2, LENGTH, xml);
	tempChar2[LENGTH-1]=0;
	if(0 == strcmp(tempChar2,"1"))
	{
		pCameraCfg->bEnableVoice = 1;
	}
	else if(0 == strcmp(tempChar2,"0"))
	{
		pCameraCfg->bEnableVoice = 0;
	}
	else
	{
		IVS_INT32 iRet = IVS_PARA_INVALID;
		BP_RUN_LOG_ERR(iRet, "Set CameraBriefInfoCfg ParseXML fail","Invalid VoiceInEnable");
		return iRet;
	}
	GET_ELEM_VALUE_CHAR("VoiceOutCode", szElemValue, pCameraCfg->cVoiceOutCode, IVS_DEV_CODE_LEN, xml);

	return IVS_SUCCEED;
}

//设置摄像机显示参数，解析xml,生成结构体
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetCameraDisplayCfgParseXml(IVS_VOID*& pBuffer, CXml& xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_DISPLAY_CFG* pCameraDisplayCfg = reinterpret_cast<IVS_CAMERA_DISPLAY_CFG*>(pBuffer);
	if (!xml.FindElemEx("Content/ImagingInfo/BasicInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/ImagingInfo) fail", "NA");
		return IVS_XML_INVALID;
	}

	(void)xml.IntoElem();
	const IVS_CHAR* szElemValue = NULL;
	//亮度
	GET_ELEM_VALUE_NUM_FOR_UINT("Bright", szElemValue, pCameraDisplayCfg->uiBrightness, xml);
	//对比度
	GET_ELEM_VALUE_NUM_FOR_UINT("Contrast", szElemValue, pCameraDisplayCfg->uiContrast, xml);
	//色度
	GET_ELEM_VALUE_NUM_FOR_UINT("Hue", szElemValue, pCameraDisplayCfg->uiHue, xml);
	//饱和度
	GET_ELEM_VALUE_NUM_FOR_UINT("Saturation", szElemValue, pCameraDisplayCfg->uiSaturation, xml);

	return IVS_SUCCEED;
}

//设置云台参数，输入设置的xml,输出云台参数结构
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetCameraPTZCfgParseXml(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);

	IVS_DEBUG_TRACE("");
	IVS_PTZ_CFG* pCameraStrCfg = (IVS_PTZ_CFG*)(pBuffer);
	if (!xml.FindElemEx("Content/PTZInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/PTZInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	
	//进入PTZInfo节点
	(void)xml.IntoElem();
	const IVS_CHAR* szElemValue = NULL;

	//IVS_INT32 iTemp = 0;
	//云台解码器类型DecoderType
	GET_ELEM_VALUE_NUM_FOR_UINT("DecoderType", szElemValue, pCameraStrCfg->uiDecoderType, xml);

	//云台解码器地址DecoderAddress
	GET_ELEM_VALUE_NUM_FOR_UINT("DecoderAddress", szElemValue, pCameraStrCfg->uiDecoderAddress, xml);

	//云台正反向控制PosNegDirectControl，取值范围：0-正向 1-反向
	const int LENGTH =10;
	IVS_CHAR tempChar[LENGTH]={0};
	GET_ELEM_VALUE_CHAR("PosNegDirectControl", szElemValue, tempChar, LENGTH, xml);
	tempChar[LENGTH-1] = 0;
	if(0 == strcmp(tempChar,"FORWARD"))
	{
		pCameraStrCfg->bPosNegDirectControl = 0;
	}
	else if(0 == strcmp(tempChar,"BACK"))
	{
		pCameraStrCfg->bPosNegDirectControl = 1;
	}
	else
	{
		IVS_INT32 iRet = IVS_PARA_INVALID;
		BP_RUN_LOG_ERR(iRet, "Set CameraPTZCfg ParseXml fail","Invalid PosNegDirectControl");
		return iRet;
	}

	//云台串口编码SerialCode
	GET_ELEM_VALUE_CHAR("SerialCode", szElemValue, pCameraStrCfg->cSerialCode, IVS_DEV_SERIALNO_LEN, xml);

	//其他云台参数结构体默认值
	memset(pCameraStrCfg->cReserve,0,32);

	return IVS_SUCCEED;
}

//设置语音子设备参数，输入设置的xml,输出语音参数结构
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetAudioCfgParseXml(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_AUDIO_CFG* pAudioCfg = reinterpret_cast<IVS_AUDIO_CFG*>(pBuffer);
	IVS_INT32 iRet = IVS_FAIL;
	if (!xml.FindElemEx("Content/VoiceChanInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/VoiceChanInfo) fail", "NA");
		return IVS_XML_INVALID;
	}

	//进入VoiceChanInfo节点
	(void)xml.IntoElem();

	//处理解析时使用的一些临时的变量
	const IVS_CHAR* szElemValue = NULL;

	//VoiceName
	GET_ELEM_VALUE_CHAR("VoiceName", szElemValue, pAudioCfg->cAudioName, IVS_AUDIO_NAME_LEN, xml);

	//VoiceParam
	if (!xml.FindElemEx("Content/VoiceChanInfo/VoiceParam"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/VoiceChanInfo/VoiceParam) fail", "NA");
		return iRet;
	}
	//进入VoiceParam节点
	(void)xml.IntoElem();
	{
		//Pitch
		GET_ELEM_VALUE_NUM_FOR_UINT("Pitch", szElemValue,pAudioCfg->stAudioParam.uiPitch, xml);

		//Volume
		GET_ELEM_VALUE_NUM_FOR_UINT("Volume", szElemValue,pAudioCfg->stAudioParam.uiVolume, xml);

		//InputType
		GET_ELEM_VALUE_NUM_FOR_UINT("InputType", szElemValue,pAudioCfg->stAudioParam.uiInputType, xml);

		//DataFormat G711A G711U G726 AAC
		const int tempLength = 10;
		IVS_CHAR tempChar[tempLength] = {0};
		GET_ELEM_VALUE_CHAR("DataFormat",szElemValue,tempChar,tempLength,xml);
		pAudioCfg->stAudioParam.uiDataFormat = static_cast<IVS_UINT32>(CDevEnumXML::GetDataFormatINT(tempChar));

		//SampleIngrate
		GET_ELEM_VALUE_NUM_FOR_UINT("SampleIngrate", szElemValue,pAudioCfg->stAudioParam.uiSampleRate, xml);

		//Channel
		GET_ELEM_VALUE_NUM_FOR_UINT("Channel", szElemValue,pAudioCfg->stAudioParam.uiChannel, xml);

		//Gain
		GET_ELEM_VALUE_NUM_FOR_UINT("Gain", szElemValue,pAudioCfg->stAudioParam.uiGain, xml);

		//SampleBits
		GET_ELEM_VALUE_NUM_FOR_UINT("SampleBits", szElemValue,pAudioCfg->stAudioParam.uiSampleBits, xml);
	}

	//其他参数结构体默认值
	memset(pAudioCfg->stAudioParam.cReserve,0,32);
	memset(pAudioCfg->cReserve,0,32);

	return IVS_SUCCEED;
}

// 解析DevCode
IVS_INT32 COCXDeviceParaConfigXMLProcess::ParseDevCode(const IVS_CHAR *pDevBigCode, IVS_CHAR *pDevCode, IVS_CHAR* pDomainCode )const
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevBigCode, IVS_OPERATE_MEMORY_ERROR);  
	IVS_DEBUG_TRACE("");

	if (IVS_DEV_CODE_LEN < strlen(pDevBigCode))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Parse DevCode fail", "IVS_DEV_CODE_LEN != strlen(pDevBigCode)");
		return IVS_PARA_INVALID;
	}

	// 避免常量测试出错
	std::string strDevBigCode = pDevBigCode;
	std::string strDevCode = strDevBigCode.substr(0,strDevBigCode.rfind("#"));
	std::string strDomainCode = strDevBigCode.substr(strDevBigCode.rfind("#") + 1, strlen(strDevBigCode.c_str()));

	if (!CToolsHelp::Strncpy(pDevCode, IVS_SDK_DEV_CODE_LEN+1, strDevCode.c_str(), IVS_SDK_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "strDevCode.c_str() strncpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	if (!CToolsHelp::Strncpy(pDomainCode, IVS_DOMAIN_CODE_LEN+1, strDomainCode.c_str(), IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "strDomainCode.c_str() strncpy fail", "NA");
	}

	return IVS_SUCCEED;
}

// 修改告警输入子设备
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetAlarmInCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_ALARM_IN_CFG* pAlarmInCfg = reinterpret_cast<IVS_ALARM_IN_CFG*>(pBuffer);
	if (!xml.FindElemEx("Content/AlarmInChanInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Set AlarmInCfg ParseXML fail", "xml.FindElemEx(Content/AlarmInChanInfo) fail");
		return IVS_XML_INVALID;
	}
	const IVS_INT32 iMaxLen = 10;
	(void)xml.IntoElem();
	const char* pTemp = NULL;
	IVS_INT32 iTemp = 0;
	GET_ELEM_VALUE_CHAR("AlarmInName", pTemp, pAlarmInCfg->cAlarmInName, IVS_ALARM_IN_NAME_LEN, xml);
	IVS_CHAR cSwitchType[iMaxLen] = {0};
	GET_ELEM_VALUE_CHAR("SwitchType", pTemp, cSwitchType, iMaxLen, xml);
	pAlarmInCfg->uiSwitchType = static_cast<IVS_UINT32>(CDevEnumXML::GetCommStatusBool(cSwitchType));
	GET_ELEM_VALUE_NUM("AutoClearAlarmTime", pTemp, iTemp, xml);
	pAlarmInCfg->uiAutoClearAlarmTime = static_cast<IVS_UINT32>(iTemp);

	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询告警输入子设备
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetAlarmInCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_ALARM_IN_CFG* pAlarmInCfg = reinterpret_cast<IVS_ALARM_IN_CFG*>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("AlarmInChanInfo");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("AlarmInName");
	(void)xml.IntoElem();
	IVS_CHAR cAlarmInName[IVS_ALARM_IN_NAME_LEN + 1];
	memset(cAlarmInName, 0x0, IVS_ALARM_IN_NAME_LEN + 1);
	if (!CToolsHelp::Memcpy(cAlarmInName, IVS_ALARM_IN_NAME_LEN, pAlarmInCfg->cAlarmInName, IVS_ALARM_IN_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pAlarmInCfg->cAlarmInName memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cAlarmInName);

	(void)xml.AddElem("AlarmInChanIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmInCfg->uiAlarmInChanIndex)).c_str());

	(void)xml.AddElem("SwitchType");
	(void)xml.IntoElem();
	const IVS_CHAR* pSwitchType = CDevEnumXML::GetCommStatusType(static_cast<IVS_INT32>(pAlarmInCfg->uiSwitchType));
	(void)xml.SetElemValue(pSwitchType);

	(void)xml.AddElem("AutoClearAlarmTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmInCfg->uiAutoClearAlarmTime)).c_str());

	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 修改告警输出子设备
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetAlarmOutCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_ALARM_OUT_CFG* pAlarmOutCfg = reinterpret_cast<IVS_ALARM_OUT_CFG*>(pBuffer);
	if (!xml.FindElemEx("Content/AlarmOutChanInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/AlarmOutChanInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	const IVS_INT32 iMaxLen = 10;
	(void)xml.IntoElem();
	const char* pTemp = NULL;
	IVS_INT32 iTemp = 0;
	GET_ELEM_VALUE_CHAR("AlarmOutName", pTemp, pAlarmOutCfg->cAlarmOutName, IVS_ALARM_OUT_NAME_LEN, xml);
	IVS_CHAR cSwitchType[iMaxLen] = {0};
	GET_ELEM_VALUE_CHAR("SwitchType", pTemp, cSwitchType, iMaxLen, xml);
	pAlarmOutCfg->uiSwitchType = static_cast<IVS_UINT32>(CDevEnumXML::GetCommStatusBool(cSwitchType));
	GET_ELEM_VALUE_NUM("HoldTime", pTemp, iTemp, xml);
	pAlarmOutCfg->uiHoldTime = static_cast<IVS_UINT32>(iTemp);

	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询告警输出子设备拼接XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetALarmOutCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_ALARM_OUT_CFG* pAlarmOutCfg = reinterpret_cast<IVS_ALARM_OUT_CFG*>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("AlarmOutChanInfo");
	(void)xml.IntoElem();
	
	(void)xml.AddChildElem("AlarmOutName");
	(void)xml.IntoElem();
	IVS_CHAR cAlarmOutCfg[IVS_ALARM_OUT_NAME_LEN + 1];
	memset(cAlarmOutCfg, 0x0, IVS_ALARM_OUT_NAME_LEN + 1);
	if (!CToolsHelp::Memcpy(cAlarmOutCfg, IVS_ALARM_OUT_NAME_LEN, pAlarmOutCfg->cAlarmOutName, IVS_ALARM_OUT_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pAlarmOutCfg->cAlarmOutName memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cAlarmOutCfg);

	(void)xml.AddElem("AlarmOutChanIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmOutCfg->uiAlarmOutChanIndex)).c_str());

	(void)xml.AddElem("SwitchType");
	(void)xml.IntoElem();
	const IVS_CHAR* pSwitchType = CDevEnumXML::GetCommStatusType(static_cast<IVS_INT32>(pAlarmOutCfg->uiSwitchType));
	(void)xml.SetElemValue(pSwitchType);

	(void)xml.AddElem("HoldTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pAlarmOutCfg->uiHoldTime)).c_str());
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询OSD参数拼接XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetOSDConfigGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_OSD_CFG *pOSDConfig = reinterpret_cast<IVS_CAMERA_OSD_CFG *>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");
	(void)xml.AddElem("OSDParaInfo");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("OSDTimeInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("OSDTimeEnable");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(COCXXmlProcess::TransIntToOnOff(static_cast<IVS_UINT32>(pOSDConfig->stOSDTime.bEnableOSDTime)).c_str());

	(void)xml.AddElem("TimeFormatType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pOSDConfig->stOSDTime.uiTimeFormat)).c_str());

	(void)xml.AddElem("TimeX");
	(void)xml.IntoElem();
	IVS_CHAR cTimeX[STR_LEN] = {0};
	if (!CToolsHelp::Snprintf(cTimeX, STR_LEN, "%f", pOSDConfig->stOSDTime.fTimeX))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cTimeX, STR_LEN, '%f', pCameraOsgCfg->stOSDTime.fTimeX) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cTimeX);

	(void)xml.AddElem("TimeY");
	(void)xml.IntoElem();
	IVS_CHAR cTimeY[STR_LEN] = {0};
	if (!CToolsHelp::Snprintf(cTimeY, STR_LEN, "%f", pOSDConfig->stOSDTime.fTimeY))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cTimeY, STR_LEN, '%f', pCameraOsgCfg->stOSDTime.fTimeY) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cTimeY);

	xml.OutOfElem();

	(void)xml.AddElem("OSDNameInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("OSDNameEnable");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(COCXXmlProcess::TransIntToOnOff(static_cast<IVS_UINT32>(pOSDConfig->stOSDName.bEnableOSDName)).c_str());

	(void)xml.AddElem("OSDNameText");
	(void)xml.IntoElem();
	IVS_CHAR cOSDNameText[IVS_MAX_OSD_TEXT_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cOSDNameText, IVS_MAX_OSD_TEXT_LEN, pOSDConfig->stOSDName.cOSDNameText, IVS_MAX_OSD_TEXT_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cOSDNameText, IVS_MAX_OSD_TEXT_LEN, pOSDConfig->stOSDName.cOSDNameText, IVS_MAX_OSD_TEXT_LEN) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cOSDNameText);

	(void)xml.AddElem("TextX");
	(void)xml.IntoElem();
	IVS_CHAR cLeft[STR_LEN] = {0};
	if (!CToolsHelp::Snprintf(cLeft, STR_LEN, "%f", pOSDConfig->stOSDName.rectText.left))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cLeft, STR_LEN, '%f', pOSDConfig->stOSDName.rectText.left) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cLeft);

	(void)xml.AddElem("TextY");
	(void)xml.IntoElem();
	IVS_CHAR cTop[STR_LEN] = {0};
	if (!CToolsHelp::Snprintf(cTop, STR_LEN, "%f", pOSDConfig->stOSDName.rectText.top))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cTop, STR_LEN, '%f', pOSDConfig->stOSDName.rectText.top) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cTop);

	(void)xml.AddElem("SwitchEnable");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pOSDConfig->stOSDName.bEnableSwitch).c_str());

	(void)xml.AddElem("SwitchInterval");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pOSDConfig->stOSDName.uiSwitchInterval)).c_str());

	(void)xml.AddElem("TextBlinkEnable");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pOSDConfig->stOSDName.bEnableTextBlink).c_str());

	(void)xml.AddElem("TextTranslucentEnable");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pOSDConfig->stOSDName.bEnableTextTranslucent).c_str());

	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 拼装隐私保护xml
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetVideoMaskConfigGetXml(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	
	IVS_VIDEO_MASK* pVideoMask = reinterpret_cast<IVS_VIDEO_MASK*>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("VideoMaskInfo");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("VideoMaskEnable");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CDevEnumXML::GetCommStatusType(pVideoMask->bEnableVideoMask));
	(void)xml.AddElem("AreaInfoList");
	(void)xml.IntoElem();
	IVS_UINT32 iCount = pVideoMask->uiAreaNum;
	for (IVS_UINT32 index = 0; index < iCount; index ++)
	{
		if (0 == index)
		{
			(void)xml.AddChildElem("AreaInfo");
		}
		else
		{
			(void)xml.AddElem("AreaInfo");
		}

		(void)xml.IntoElem();
		(void)xml.AddChildElem("X1");
		(void)xml.IntoElem();
		IVS_CHAR cLeft[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cLeft, STR_LEN, "%f", pVideoMask->rectArea[index].left))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cLeft, STR_LEN, '%f', pVideoMask->rectArea[index].left) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cLeft);

		(void)xml.AddElem("Y1");
		IVS_CHAR cTop[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cTop, STR_LEN, "%f", pVideoMask->rectArea[index].top))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cTop, STR_LEN, '%f', pVideoMask->rectArea[index].top) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cTop);

		(void)xml.AddElem("X2");
		IVS_CHAR cRight[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cRight, STR_LEN, "%f", pVideoMask->rectArea[index].right))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cRight, STR_LEN, '%f', pVideoMask->rectArea[index].right) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cRight);

		(void)xml.AddElem("Y2");
		IVS_CHAR cBottom[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cBottom, STR_LEN, "%f", pVideoMask->rectArea[index].bottom))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cBottom, STR_LEN, '%f', pVideoMask->rectArea[index].bottom) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cBottom);
	    
		xml.OutOfElem();
	}

	return IVS_SUCCEED;
}

// 解析修改隐私保护xml
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetVideoMaskConfigParseXml(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_VIDEO_MASK* pVideoMask = reinterpret_cast<IVS_VIDEO_MASK*>(pBuffer);
	if (!xml.FindElemEx("Content/VideoMaskInfo"))
	{
		return IVS_XML_INVALID;
	}
	const IVS_INT32 maxLen = 120;
	IVS_CHAR chTemp[maxLen] = {0};
	const char* szElemValue = NULL;
	IVS_UINT32 index = 0;

	GET_ELEM_VALUE_CHAR("VideoMaskEnable", szElemValue, chTemp, maxLen, xml);
	pVideoMask->bEnableVideoMask = CDevEnumXML::GetCommStatusBool(chTemp);

	if (!xml.FindElem("AreaInfoList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(StreamList) fail", "NA");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();

	if (!xml.FindElem("AreaInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(StreamList) fail", "NA");
		pVideoMask->uiAreaNum = 0;
	} 
	else
	{
		do 
		{
			(void)xml.IntoElem();

			GET_ELEM_VALUE_FLOAT("X1", szElemValue, pVideoMask->rectArea[index].left, xml);

			GET_ELEM_VALUE_FLOAT("Y1", szElemValue, pVideoMask->rectArea[index].top, xml);

			GET_ELEM_VALUE_FLOAT("X2", szElemValue, pVideoMask->rectArea[index].right, xml);

			GET_ELEM_VALUE_FLOAT("Y2", szElemValue, pVideoMask->rectArea[index].bottom, xml);

			xml.OutOfElem();
			index++;

		} while (xml.NextElem() && index < IVS_MAX_VIDEO_MASK_AREA_NUM);

		pVideoMask->uiAreaNum = index;
	}
	
	xml.GetRootPos();

	return IVS_SUCCEED;
}

// 修改OSD参数
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetCameraOsgCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_OSD_CFG* pCameraOsgCfg = reinterpret_cast<IVS_CAMERA_OSD_CFG*>(pBuffer);
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "xml.FindElemEx(Content/OSDParaInfo)", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	(void)xml.IntoElem();
	if (!xml.FindElem("OSDParaInfo"))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "xml.FindElemEx(Content/OSDParaInfo)", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	(void)xml.IntoElem();

	const IVS_INT32 iMaxLen = 10;
	const char* pTemp = NULL;
	IVS_INT32 iTemp = 0;

	if (!xml.FindElem("OSDTimeInfo"))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "xml.FindElem(OSDTimeInfo)", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	(void)xml.IntoElem();
	{
		IVS_CHAR cOSDTimeEnable[iMaxLen] = {0};
		GET_ELEM_VALUE_CHAR("OSDTimeEnable", pTemp, cOSDTimeEnable, iMaxLen, xml);
		pCameraOsgCfg->stOSDTime.bEnableOSDTime = CDevEnumXML::GetCommStatusBool(cOSDTimeEnable);
		GET_ELEM_VALUE_NUM("TimeFormatType", pTemp, iTemp, xml);
		pCameraOsgCfg->stOSDTime.uiTimeFormat = static_cast<IVS_UINT32>(iTemp);
		GET_ELEM_VALUE_FLOAT("TimeX", pTemp, pCameraOsgCfg->stOSDTime.fTimeX, xml);
		GET_ELEM_VALUE_FLOAT("TimeY", pTemp, pCameraOsgCfg->stOSDTime.fTimeY, xml);
	}
	xml.OutOfElem();

	if (!xml.FindElem("OSDNameInfo"))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "xml.FindElem(OSDNameInfo)", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	(void)xml.IntoElem();
	{
		IVS_CHAR cOSDNameEnable[iMaxLen] = {0};
		GET_ELEM_VALUE_CHAR("OSDNameEnable", pTemp, cOSDNameEnable, iMaxLen, xml);
		pCameraOsgCfg->stOSDName.bEnableOSDName = CDevEnumXML::GetCommStatusBool(cOSDNameEnable);
		GET_ELEM_VALUE_CHAR("OSDNameText", pTemp, pCameraOsgCfg->stOSDName.cOSDNameText, IVS_MAX_OSD_TEXT_LEN, xml);
		GET_ELEM_VALUE_FLOAT("TextX", pTemp, pCameraOsgCfg->stOSDName.rectText.left, xml);
		GET_ELEM_VALUE_FLOAT("TextY", pTemp, pCameraOsgCfg->stOSDName.rectText.right, xml);
		GET_ELEM_VALUE_NUM("SwitchEnable", pTemp, pCameraOsgCfg->stOSDName.bEnableSwitch, xml);
		GET_ELEM_VALUE_NUM("SwitchInterval", pTemp, iTemp, xml);
		pCameraOsgCfg->stOSDName.uiSwitchInterval = static_cast<IVS_UINT32>(iTemp);
		GET_ELEM_VALUE_NUM("TextBlinkEnable", pTemp, pCameraOsgCfg->stOSDName.bEnableTextBlink, xml);
		GET_ELEM_VALUE_NUM("TextTranslucentEnable", pTemp, pCameraOsgCfg->stOSDName.bEnableTextTranslucent, xml);
	}
	xml.OutOfElem();

	return IVS_SUCCEED;
}
   
// 修改图像遮挡
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetVideHideAlarmParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_VIDEO_HIDE_ALARM* pVideoHideAlarm = reinterpret_cast<IVS_VIDEO_HIDE_ALARM*>(pBuffer);
	if (!xml.FindElemEx("Content/VideoHideInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/VideoHideInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	const IVS_UINT32 uiMaxLen = 10;
	const char* pTemp = NULL;
	IVS_INT32 iTemp = 0;
	(void)xml.IntoElem();
	
	GET_ELEM_VALUE_NUM("Sensitivity", pTemp, iTemp, xml);
	pVideoHideAlarm->uiSensitivity = static_cast<IVS_UINT32>(iTemp);
	GET_ELEM_VALUE_NUM("AutoClearAlarmTime", pTemp, iTemp, xml);
	pVideoHideAlarm->uiAutoClearAlarmTime = static_cast<IVS_UINT32>(iTemp);
	IVS_CHAR cVideoHideEnable[uiMaxLen] = {0};
	GET_ELEM_VALUE_CHAR("VideoHideEnable", pTemp, cVideoHideEnable, uiMaxLen, xml);
	pVideoHideAlarm->bEnableVideoHide = CDevEnumXML::GetCommStatusBool(cVideoHideEnable);
	if (!xml.FindElem("GuardPlan"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(GuardPlan) fail", "NA");
		return IVS_XML_INVALID;
	}
	IVS_UINT32 iGuardTime = 0;
	(void)xml.IntoElem();
	{
		if (!xml.FindElem("GuardTime"))
		{
			BP_DBG_LOG("xml.FindElem(GuardTime) fail", "NA");
		}

		else
		{
			do 
			{
				(void)xml.IntoElem();
				{
					GET_ELEM_VALUE_CHAR("StartTime", pTemp, pVideoHideAlarm->stGuardTime[iGuardTime].cStart, IVS_TIME_LEN, xml);
					GET_ELEM_VALUE_CHAR("EndTime", pTemp, pVideoHideAlarm->stGuardTime[iGuardTime].cEnd, IVS_TIME_LEN, xml);
				}
				xml.OutOfElem();
				iGuardTime++;
			} while (xml.NextElem() && iGuardTime < IVS_MAX_VIDEO_HIDE_GUARD_TIME_NUM);
			xml.OutOfElem();
		}
	}

	pVideoHideAlarm->uiGuardTimeNum = iGuardTime;

	if (!xml.FindElem("AreaInfoList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(AreaInfoList)", "NA");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	IVS_UINT32 iAreaInfo = 0;
	if (!xml.FindElem("AreaInfo"))
	{
		BP_RUN_LOG_INF("xml.FindElem(AreaInfo) fail", "NA");
	}
	else
	{
		do 
		{
			(void)xml.IntoElem();
			{
				GET_ELEM_VALUE_FLOAT("X1", pTemp, pVideoHideAlarm->rectArea[iAreaInfo].left, xml);
				GET_ELEM_VALUE_FLOAT("Y1", pTemp, pVideoHideAlarm->rectArea[iAreaInfo].top, xml);
				GET_ELEM_VALUE_FLOAT("X2", pTemp, pVideoHideAlarm->rectArea[iAreaInfo].right, xml);
				GET_ELEM_VALUE_FLOAT("Y2", pTemp, pVideoHideAlarm->rectArea[iAreaInfo].bottom, xml);
			}
			xml.OutOfElem();
			iAreaInfo++;
		} while (xml.NextElem() && iAreaInfo < IVS_MAX_VIDEO_HIDE_AREA_NUM);
		xml.OutOfElem();
	}

	pVideoHideAlarm->uiAreaNum = iAreaInfo;
	
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询图像遮挡拼接XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetVideoHideAlarmGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_VIDEO_HIDE_ALARM* pVideoHideAlarm = reinterpret_cast<IVS_VIDEO_HIDE_ALARM*>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("VideoHideInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Sensitivity");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pVideoHideAlarm->uiSensitivity)).c_str());

	(void)xml.AddElem("AutoClearAlarmTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pVideoHideAlarm->uiAutoClearAlarmTime)).c_str());

	(void)xml.AddElem("VideoHideEnable");
	(void)xml.IntoElem();
	const IVS_CHAR* pVideoHideEnable = CDevEnumXML::GetCommStatusType(pVideoHideAlarm->bEnableVideoHide);
	(void)xml.SetElemValue(pVideoHideEnable);

	(void)xml.AddElem("GuardPlan");
	(void)xml.IntoElem();
	for (IVS_UINT32 i = 0;i < pVideoHideAlarm->uiGuardTimeNum;i++)
	{
		i == 0 ? (void)xml.AddChildElem("GuardTime"):(void)xml.AddElem("GuardTime");
		(void)xml.IntoElem();
		(void)xml.AddChildElem("StartTime");
		(void)xml.IntoElem();
		IVS_CHAR cStartTime[IVS_TIME_LEN + 1];
		memset(cStartTime, 0x0, IVS_TIME_LEN + 1);
		if (!CToolsHelp::Memcpy(cStartTime, IVS_TIME_LEN, pVideoHideAlarm->stGuardTime[i].cStart, IVS_TIME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pVideoHideAlarm->stGuardTime[i].cStart memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cStartTime);

		(void)xml.AddElem("EndTime");
		(void)xml.IntoElem();
		IVS_CHAR cEndTime[IVS_TIME_LEN + 1];
		memset(cEndTime, 0x0, IVS_TIME_LEN + 1);
		if (!CToolsHelp::Memcpy(cEndTime, IVS_TIME_LEN, pVideoHideAlarm->stGuardTime[i].cEnd, IVS_TIME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pVideoHideAlarm->stGuardTime[i].cEnd memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cEndTime);
		xml.OutOfElem();
		
	}
	if (0 != pVideoHideAlarm->uiGuardTimeNum)
	{
		xml.OutOfElem();
	}


	(void)xml.AddElem("AreaInfoList");
	(void)xml.IntoElem();
	for (IVS_UINT32 j = 0;j < pVideoHideAlarm->uiAreaNum;j++)
	{
		j == 0 ? (void)xml.AddChildElem("AreaInfo"):(void)xml.AddElem("AreaInfo");
		(void)xml.IntoElem();
		(void)xml.AddChildElem("X1");
		(void)xml.IntoElem();
		IVS_CHAR cLeft[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cLeft, STR_LEN, "%f", pVideoHideAlarm->rectArea[j].left))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cLeft, STR_LEN, '%f', pVideoHideAlarm->rectArea[j].left) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cLeft);

		(void)xml.AddElem("Y1");
		(void)xml.IntoElem();
		IVS_CHAR cTop[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cTop, STR_LEN, "%f", pVideoHideAlarm->rectArea[j].top))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cTop, STR_LEN, '%f', pVideoHideAlarm->rectArea[j].top) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cTop);

		(void)xml.AddElem("X2");
		(void)xml.IntoElem();
		IVS_CHAR cRight[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cRight, STR_LEN, "%f", pVideoHideAlarm->rectArea[j].right))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cRight, STR_LEN, '%f', pVideoHideAlarm->rectArea[j].right) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cRight);

		(void)xml.AddElem("Y2");
		(void)xml.IntoElem();
		IVS_CHAR cBottom[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cBottom, STR_LEN, "%f", pVideoHideAlarm->rectArea[j].bottom))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cBottom, STR_LEN, '%f', pVideoHideAlarm->rectArea[j].bottom) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cBottom);
		xml.OutOfElem();
	}
	if (0 != pVideoHideAlarm->uiAreaNum)
	{
		xml.OutOfElem();
	}

	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 计算响应消息中AreaInfo的条数
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetMotionDetecAreaInfoNum(CXml& xml, IVS_INT32& iNum)const
{
	IVS_DEBUG_TRACE("");

	if (!xml.FindElemEx("Content/MotionDetectionInfo/AreaInfoList"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "xml.FindElemEx(Content/MotionDetectionInfo/AreaInfoList)", "NA");
		return IVS_SMU_XML_INVALID;
	}

	// AreaInfo 是O字段，可以没有，如果没有直接返回0；
	if (!xml.FindElem("AreaInfo"))
	{
		return iNum;
	}

	do 
	{
		if (!xml.FindElemValue("AreaInfo"))
		{
			break;
		}
		iNum++;
	} while (xml.NextElem());

	return iNum;
}

// 设置前段移动监测
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetMotionDetecParseXML(IVS_VOID*& pBuffer, CXml& xml)const
{
	CHECK_POINTER(pBuffer, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	 
	IVS_MOTION_DETECTION* pMotionDetection = reinterpret_cast<IVS_MOTION_DETECTION*>(pBuffer);
	IVS_INT32 iNum = 0;
	(void)GetMotionDetecAreaInfoNum(xml, iNum);

	if (!xml.FindElemEx("Content/MotionDetectionInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/MotionDetectionInfo)", "NA");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* szElemValue = NULL;
	IVS_CHAR szTemp[ENABLE_lENGTH] = {0};
	GET_ELEM_VALUE_NUM_FOR_UINT("DetectInterval", szElemValue, pMotionDetection->uiDetectInterval, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("Sensitivity", szElemValue, pMotionDetection->uiSensitivity, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("AutoClearAlarmTime", szElemValue, pMotionDetection->uiAutoClearAlarmTime, xml);
	GET_ELEM_VALUE_CHAR("MotionDetectionEnable", szElemValue, szTemp, ENABLE_lENGTH, xml);
	if (0 == strcmp("ON", szTemp))
	{
		pMotionDetection->bMotionDetectionEnable = 1;
	}
	if (0 == strcmp("OFF", szTemp))
	{
		pMotionDetection->bMotionDetectionEnable = 0;
	}
	memset(szTemp, 0x0, ENABLE_lENGTH);
	GET_ELEM_VALUE_CHAR("PlatformMotionDetection", szElemValue, szTemp, ENABLE_lENGTH, xml);
	if (0 == strcmp("ON", szTemp))
	{
		pMotionDetection->bPlatformMotionDetection = 1;
	}
	if (0 == strcmp("OFF", szTemp))
	{
		pMotionDetection->bPlatformMotionDetection = 0;
	}

	if (!xml.FindElem("AreaInfoList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(AreaInfoList)", "NA");
		return IVS_XML_INVALID;
	}
	if (0 == iNum)
	{
		return IVS_SUCCEED;
	}
	if (iNum > IVS_MAX_MOTION_DETECTION_AREA_NUM)
	{
		BP_DBG_LOG("iNum < IVS_MAX_MOTION_DETECTION_AREA_NUM", "NA");
		pMotionDetection->uiRectNum = IVS_MAX_MOTION_DETECTION_AREA_NUM;
		iNum = IVS_MAX_MOTION_DETECTION_AREA_NUM;
	}
	else
	{
		pMotionDetection->uiRectNum = static_cast<IVS_UINT32>(iNum);
	}

	(void)xml.IntoElem();
	if (!xml.FindElem("AreaInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(AreaInfo)", "NA");
		return IVS_XML_INVALID;
	}
	IVS_INT32 i = 0;
	while(i < iNum)
	{
		if (!xml.FindElemValue("AreaInfo"))
		{
			break;
		}
		(void)xml.IntoElem();
		IVS_RECT_FLOAT& stRect = pMotionDetection->rectDetection[i];
		GET_ELEM_VALUE_FLOAT("X1", szElemValue, stRect.left, xml);
		GET_ELEM_VALUE_FLOAT("Y1", szElemValue, stRect.top, xml);
		GET_ELEM_VALUE_FLOAT("X2", szElemValue, stRect.right, xml);
		GET_ELEM_VALUE_FLOAT("Y2", szElemValue, stRect.bottom, xml);
		xml.OutOfElem();
		i++;
		(void)xml.NextElem();
	}

	return IVS_SUCCEED;
}

// 查询前端移动监测拼装响应xml
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetMotioinDetecGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	
	IVS_MOTION_DETECTION* pMotionDetec = reinterpret_cast<IVS_MOTION_DETECTION*>(pBuffer);
	// 开始拼装xml
	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(IVS_SUCCEED).c_str());

	(void)xml.AddElem("MotionDetectionInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("DetectInterval");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionDetec->uiDetectInterval)).c_str());

	(void)xml.AddElem("Sensitivity");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionDetec->uiSensitivity)).c_str());

	(void)xml.AddElem("AutoClearAlarmTime");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionDetec->uiAutoClearAlarmTime)).c_str());

	(void)xml.AddElem("MotionDetectionEnable");
 	if (1 == pMotionDetec->bMotionDetectionEnable)
 	{
 		(void)xml.SetElemValue("ON");
 	}
 	if (0 == pMotionDetec->bMotionDetectionEnable)
 	{
 		(void)xml.SetElemValue("OFF");
 	}

	(void)xml.AddElem("PlatformMotionDetection");
	if (1 == pMotionDetec->bPlatformMotionDetection)
	{
		(void)xml.SetElemValue("ON");
	}
	if (0 == pMotionDetec->bPlatformMotionDetection)
	{
		(void)xml.SetElemValue("OFF");
	}

	(void)xml.AddElem("AreaInfoList");
	(void)xml.IntoElem();
	IVS_UINT32 i = 0;
	while(i < pMotionDetec->uiRectNum)
	{

		const IVS_RECT_FLOAT& stRect = pMotionDetec->rectDetection[i];

		i == 0?(void)xml.AddChildElem("AreaInfo"):(void)xml.AddElem("AreaInfo");
		(void)xml.IntoElem();
		(void)xml.AddChildElem("X1");
		(void)xml.IntoElem();
		IVS_CHAR cLeft[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cLeft, STR_LEN, "%f", stRect.left))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cLeft, STR_LEN, '%f', stRect.left) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cLeft);

		(void)xml.AddElem("Y1");
		IVS_CHAR cTop[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cTop, STR_LEN, "%f", stRect.top))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cTop, STR_LEN,'%f', stRect.top) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cTop);

		(void)xml.AddElem("X2");
		IVS_CHAR cRight[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cRight, STR_LEN, "%f", stRect.right))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cRight, STR_LEN, '%f', stRect.right) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cRight);

		(void)xml.AddElem("Y2");
		IVS_CHAR cBottom[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cBottom, STR_LEN, "%f", stRect.bottom))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cBottom, STR_LEN, '%f', stRect.bottom) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cBottom);
		xml.OutOfElem();

		i++;
	}
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 修改主设备网络参数解析XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetDevNetCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");

	IVS_DEVICE_NET_CFG* pDevNetCfg = reinterpret_cast<IVS_DEVICE_NET_CFG*>(pBuffer);
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(Content) fail", "NA");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	if (!xml.FindElem("NetInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(NetInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	const char* pTemp = NULL;
	(void)xml.IntoElem();
	{
		GET_ELEM_VALUE_NUM_FOR_UINT("DevIPType", pTemp, pDevNetCfg->stPUIp.uiIPType, xml);
		GET_ELEM_VALUE_CHAR("DevIP", pTemp, pDevNetCfg->stPUIp.cIP, IVS_IP_LEN, xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("DevPort", pTemp, pDevNetCfg->uiPUPort, xml);
		GET_ELEM_VALUE_CHAR("DevGW", pTemp, pDevNetCfg->stPUGateway.cIP, IVS_IP_LEN, xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("MaxMTU", pTemp, pDevNetCfg->uiMaxMtu, xml);
		GET_ELEM_VALUE_CHAR("SubNetMask", pTemp, pDevNetCfg->stSubNetMask.cIP, IVS_IP_LEN, xml);
	}
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询主设备网络参数拼接XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetDevNetCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_DEVICE_NET_CFG* pDevNetCfg = reinterpret_cast<IVS_DEVICE_NET_CFG*>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("NetInfo");
	(void)xml.IntoElem();
	{
		(void)xml.AddChildElem("DevIPType");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevNetCfg->stPUIp.uiIPType)).c_str());

		(void)xml.AddElem("DevIP");
		(void)xml.IntoElem();
		IVS_CHAR cDevIp[IVS_IP_LEN + 1];
		memset(cDevIp, 0x0, IVS_IP_LEN + 1);
		if (!CToolsHelp::Memcpy(cDevIp, IVS_IP_LEN, pDevNetCfg->stPUIp.cIP, IVS_IP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevNetCfg->stPUIp.cIP memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cDevIp);

		(void)xml.AddElem("DevPort");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevNetCfg->uiPUPort)).c_str());

		(void)xml.AddElem("DevGW");
		(void)xml.IntoElem();
		IVS_CHAR cDevGw[IVS_IP_LEN + 1];
		memset(cDevGw, 0x0, IVS_IP_LEN + 1);
		if (!CToolsHelp::Memcpy(cDevGw, IVS_IP_LEN, pDevNetCfg->stPUGateway.cIP, IVS_IP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevNetCfg->stPUGateway.cIP memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cDevGw);

		(void)xml.AddElem("MaxMTU");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevNetCfg->uiMaxMtu)).c_str());

		(void)xml.AddElem("SubNetMask");
		(void)xml.IntoElem();
		IVS_CHAR cSubNetMask[IVS_IP_LEN + 1];
		memset(cSubNetMask, 0x0, IVS_IP_LEN + 1);
		if (!CToolsHelp::Memcpy(cSubNetMask, IVS_IP_LEN, pDevNetCfg->stSubNetMask.cIP, IVS_IP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevNetCfg->stSubNetMask.cIP memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cSubNetMask);
	}
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}


// 修改主设备时间参数解析XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetDevTimeCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_DEVICE_TIME_CFG* pDevTimeCfg = reinterpret_cast<IVS_DEVICE_TIME_CFG*>(pBuffer);
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(Content)", "NA");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	if (!xml.FindElem("TimeInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(TimeInfo)", "NA");
		return IVS_XML_INVALID;
	}
	const char* pTemp = NULL;
	(void)xml.IntoElem();
	{
		IVS_CHAR cTimeType[DEV_TIME_CFG_LEN] = {0};
		GET_ELEM_VALUE_CHAR("TimeType", pTemp, cTimeType, DEV_TIME_CFG_LEN, xml);
		if (0 == strcmp(cTimeType,"NTP"))
		{
			pDevTimeCfg->bEnableNtp = 1;
		}
		else if (0 == strcmp(cTimeType, "MANUAL"))
		{
			pDevTimeCfg->bEnableNtp = 0;
		}
		else
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "TimeType fail", "NA");
			return IVS_XML_INVALID;
		}

		IVS_CHAR cNtpType[DEV_TIME_CFG_LEN] = {0};
		GET_ELEM_VALUE_CHAR("NTPType", pTemp, cNtpType, DEV_TIME_CFG_LEN, xml);
		if (0 == strcmp(cNtpType, "DHCP"))
		{
			pDevTimeCfg->stNTP.uiNtpType = 0;
		}
		else if (0 == strcmp(cNtpType, "MANUAL"))
		{
			pDevTimeCfg->stNTP.uiNtpType = 1;
		}
		else
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "NTPType fail", "NA");
			return IVS_XML_INVALID;
		}

		GET_ELEM_VALUE_NUM_FOR_UINT("NTPIPType", pTemp, pDevTimeCfg->stNTP.stNtpServerIP.uiIPType, xml);
		GET_ELEM_VALUE_CHAR("NTPServerIP", pTemp, pDevTimeCfg->stNTP.stNtpServerIP.cIP, IVS_IP_LEN, xml);
		GET_ELEM_VALUE_CHAR("NTPDns", pTemp, pDevTimeCfg->stNTP.stDnsIP.cIP, IVS_IP_LEN, xml);
		GET_ELEM_VALUE_NUM("DevTimeZone", pTemp, pDevTimeCfg->iTimezone, xml);

		IVS_CHAR cDevDtsType[DEV_TIME_CFG_LEN] = {0};
		GET_ELEM_VALUE_CHAR("DevDSTType", pTemp, cDevDtsType, DEV_TIME_CFG_LEN, xml);
		pDevTimeCfg->bEnableDst = CDevEnumXML::GetCommStatusBool(cDevDtsType);
	}	
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询主设备时间参数拼接XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetDevTimeCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_DEVICE_TIME_CFG* pDevTimeCfg = reinterpret_cast<IVS_DEVICE_TIME_CFG*>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("TimeInfo");
	(void)xml.IntoElem();
	{
		(void)xml.AddChildElem("TimeType");
		(void)xml.IntoElem();
		if (0 == pDevTimeCfg->bEnableNtp)
		{
			(void)xml.SetElemValue("MANUAL");
		}
		else
		{
			(void)xml.SetElemValue("NTP");
		}

		(void)xml.AddElem("NTPType");
		(void)xml.IntoElem();
		if (0 == pDevTimeCfg->stNTP.uiNtpType)
		{
			(void)xml.SetElemValue("DHCP");
		}
		else
		{
			(void)xml.SetElemValue("MANUAL");
		}

		(void)xml.AddElem("NTPIPType");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pDevTimeCfg->stNTP.stNtpServerIP.uiIPType)).c_str());

		(void)xml.AddElem("NTPServerIP");
		(void)xml.IntoElem();
		IVS_CHAR cNtpServerIp[IVS_IP_LEN + 1];
		memset(cNtpServerIp, 0x0, IVS_IP_LEN + 1);
		if (!CToolsHelp::Memcpy(cNtpServerIp, IVS_IP_LEN, pDevTimeCfg->stNTP.stNtpServerIP.cIP, IVS_IP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevTimeCfg->stNTP.stNtpServerIP.cIP memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cNtpServerIp);

		(void)xml.AddElem("NTPDns");
		(void)xml.IntoElem();
		IVS_CHAR cNtpDns[IVS_IP_LEN + 1];
		memset(cNtpDns, 0x0, IVS_IP_LEN + 1);
		if (!CToolsHelp::Memcpy(cNtpDns, IVS_IP_LEN, pDevTimeCfg->stNTP.stDnsIP.cIP, IVS_IP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevTimeCfg->stNTP.stDnsIP.cIP memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cNtpDns);

		(void)xml.AddElem("DevTimeZone");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str(pDevTimeCfg->iTimezone).c_str());

		/*(void)xml.AddElem("DevTime");
		(void)xml.IntoElem();
		IVS_CHAR cDevTime[IVS_TIME_LEN + 1];
		memset(cDevTime, 0x0, IVS_TIME_LEN + 1);
		if (!CToolsHelp::Memcpy(cDevTime, IVS_TIME_LEN, pDevTimeCfg->cDevTime, IVS_TIME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pDevTimeCfg->cDevTime memcpy fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cDevTime);*/

		(void)xml.AddElem("DevDSTType");
		(void)xml.IntoElem();
		if (0 == pDevTimeCfg->bEnableDst)
		{
			(void)xml.SetElemValue("OFF");
		}
		else
		{
			(void)xml.SetElemValue("ON");
		}
	}
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 修改主设备时间参数解析XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetSerialChannelCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_SERIAL_CHANNEL_CFG* pSerialChannelCfg = reinterpret_cast<IVS_SERIAL_CHANNEL_CFG*>(pBuffer);
	if (!xml.FindElemEx("Content/SerialChanInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/TimeInfo)", "NA");
		return IVS_XML_INVALID;
	}
	const char* pTemp = NULL;
	(void)xml.IntoElem();
	GET_ELEM_VALUE_CHAR("SerialName", pTemp, pSerialChannelCfg->cSerialName, IVS_SERIAL_NAME_LEN, xml);
	IVS_CHAR cSerialType[10] = {0};
	//GET_ELEM_VALUE_NUM_FOR_UINT("", pTemp, pSerialChannelCfg->uiSerialType, xml);
	GET_ELEM_VALUE_CHAR("SerialType", pTemp, cSerialType, 10, xml);
	pSerialChannelCfg->uiSerialType = static_cast<IVS_UINT32> (CDevEnumXML::GetSerialTypeEnum(cSerialType));

	GET_ELEM_VALUE_NUM_FOR_UINT("BaudRate", pTemp, pSerialChannelCfg->uiBaudRate, xml);

	GET_ELEM_VALUE_NUM_FOR_UINT("DataBit", pTemp, pSerialChannelCfg->uiDataBit, xml);

	GET_ELEM_VALUE_NUM_FOR_UINT("Parity", pTemp, pSerialChannelCfg->uiParity, xml);

	GET_ELEM_VALUE_NUM_FOR_UINT("StopBit", pTemp, pSerialChannelCfg->uiStopBit, xml);

	GET_ELEM_VALUE_NUM_FOR_UINT("FlowControl", pTemp, pSerialChannelCfg->uiFlowControl, xml);

	return IVS_SUCCEED;
}

// 查询串口子设备详细信息拼接XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetSerialChannelCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_SERIAL_CHANNEL_CFG* pSerialChannelCfg = reinterpret_cast<IVS_SERIAL_CHANNEL_CFG*>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("SerialChanInfo");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("SerialName");
	(void)xml.IntoElem();
	IVS_CHAR cSerialName[IVS_SERIAL_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(cSerialName, IVS_SERIAL_NAME_LEN, pSerialChannelCfg->cSerialName, IVS_SERIAL_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "pSerialChannelCfg->cSerialName memcpy fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cSerialName);

	(void)xml.AddElem("SerialIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pSerialChannelCfg->uiSerialIndex)).c_str());

	(void)xml.AddElem("SerialType");
	(void)xml.IntoElem();
	const IVS_CHAR *pSerialType = CDevEnumXML::GetSerialTypeCH(static_cast<IVS_INT32>(pSerialChannelCfg->uiSerialType));
	(void)xml.SetElemValue(pSerialType);

	(void)xml.AddElem("BaudRate");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pSerialChannelCfg->uiBaudRate)).c_str());

	(void)xml.AddElem("DataBit");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pSerialChannelCfg->uiDataBit)).c_str());

	(void)xml.AddElem("Parity");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pSerialChannelCfg->uiParity)).c_str());

	(void)xml.AddElem("StopBit");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pSerialChannelCfg->uiStopBit)).c_str());

	(void)xml.AddElem("FlowControl");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pSerialChannelCfg->uiFlowControl)).c_str());
	
	xml.GetRootPos();

	return IVS_SUCCEED;
}

#define CAMERA_IMGAE_LEN 20
// 设置摄像机扩展图像参数解析XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetCameraImgCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_IMAGING_CFG* pCameraImgCfg = reinterpret_cast<IVS_CAMERA_IMAGING_CFG*>(pBuffer);
	if (!xml.FindElemEx("Content/ImagingInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx('Content/ImagingInfo') fail", "NA");
		return IVS_XML_INVALID;
	}

	const char* pTemp = NULL;
	// 背光补偿信息
	if (xml.FindElem("BacklightInfo"))
	{
		(void)xml.IntoElem();
		// 背光补偿模式
		IVS_CHAR cBacklightMode[CAMERA_IMGAE_LEN] = {0};
		GET_ELEM_VALUE_CHAR("BacklightMode", pTemp, cBacklightMode, CAMERA_IMGAE_LEN, xml);
		pCameraImgCfg->stBacklight.uiBacklightMode = static_cast<IVS_UINT32>(CDevEnumXML::GetCommStatusBool(cBacklightMode));

		// 背光补偿等级
		GET_ELEM_VALUE_NUM_FOR_UINT("BacklightLevel", pTemp, pCameraImgCfg->stBacklight.uiBacklightLevel, xml);
		xml.OutOfElem();
	}
	// 曝光参数信息
	if (xml.FindElem("ExposureInfo"))
	{
		(void)xml.IntoElem();
		// 曝光模式
		IVS_CHAR cExposureMode[CAMERA_IMGAE_LEN] = {0};
		GET_ELEM_VALUE_CHAR("ExposureMode", pTemp, cExposureMode, CAMERA_IMGAE_LEN, xml);
		pCameraImgCfg->stExposure.uiExposureMode = static_cast<IVS_UINT32>(CDevEnumXML::GetCommMode(cExposureMode));

		// 曝光优先
		IVS_CHAR cExposurePriority[CAMERA_IMGAE_LEN] = {0};
		GET_ELEM_VALUE_CHAR("ExposurePriority", pTemp, cExposurePriority, CAMERA_IMGAE_LEN, xml);
		pCameraImgCfg->stExposure.uiExposurePriority = static_cast<IVS_UINT32>(CDevEnumXML::GetExposurePriority(cExposurePriority));

		// 曝光窗口信息
		if (!xml.FindElem("AreaInfo"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem('AreaInfo')", "NA");
			return IVS_XML_INVALID;
		}
		(void)xml.IntoElem();
		GET_ELEM_VALUE_FLOAT("X1", pTemp, pCameraImgCfg->stExposure.rectWindow.left, xml);
		GET_ELEM_VALUE_FLOAT("Y1", pTemp, pCameraImgCfg->stExposure.rectWindow.top, xml);
		GET_ELEM_VALUE_FLOAT("X2", pTemp, pCameraImgCfg->stExposure.rectWindow.right, xml);
		GET_ELEM_VALUE_FLOAT("Y2", pTemp, pCameraImgCfg->stExposure.rectWindow.bottom, xml);
		xml.OutOfElem();

		// 曝光时间
		GET_ELEM_VALUE_NUM_FOR_UINT("ExposureTime", pTemp, pCameraImgCfg->stExposure.uiExposureTime, xml);
		// 曝光增益
		GET_ELEM_VALUE_NUM_FOR_UINT("ExposureGain", pTemp, pCameraImgCfg->stExposure.uiExposureGain, xml);
		// 光圈
		GET_ELEM_VALUE_NUM_FOR_UINT("ExposureIris", pTemp, pCameraImgCfg->stExposure.uiExposureIris, xml);
		xml.OutOfElem();
	}
	// 变焦参数信息
	if (xml.FindElem("FocusInfo"))
	{
		(void)xml.IntoElem();
		// 变焦模式
		IVS_CHAR cFocusMode[CAMERA_IMGAE_LEN] = {0};
		GET_ELEM_VALUE_CHAR("FocusMode", pTemp, cFocusMode, CAMERA_IMGAE_LEN, xml);
		pCameraImgCfg->stFocus.uiFocusMode = static_cast<IVS_UINT32>(CDevEnumXML::GetCommMode(cFocusMode));

		// 变焦速度
		GET_ELEM_VALUE_NUM_FOR_UINT("FocusSpeed", pTemp, pCameraImgCfg->stFocus.uiFocusSpeed, xml);
		// 最小变焦范围
		GET_ELEM_VALUE_NUM_FOR_UINT("NearLimit", pTemp, pCameraImgCfg->stFocus.uiNearLimit, xml);
		// 最大变焦范围
		GET_ELEM_VALUE_NUM_FOR_UINT("FarLimit", pTemp, pCameraImgCfg->stFocus.uiFarLimit, xml);
		xml.OutOfElem();
	}
	// 红外过滤
	if (xml.FindElem("IrCutFilter"))
	{
		IVS_CHAR cIrCutFilter[CAMERA_IMGAE_LEN] = {0};
		GET_ELEM_VALUE_CHAR("IrCutFilter", pTemp, cIrCutFilter, CAMERA_IMGAE_LEN, xml);
		pCameraImgCfg->uiIrCutFilter = static_cast<IVS_UINT32>(CDevEnumXML::GetCommExImgMode(cIrCutFilter));
	}
	// 宽动态信息
	if (xml.FindElem("WideDynamic"))
	{
		(void)xml.IntoElem();
		// 宽动态模式
		IVS_CHAR cWideDynamicMode[CAMERA_IMGAE_LEN] = {0};
		GET_ELEM_VALUE_CHAR("WideDynamicMode", pTemp, cWideDynamicMode, CAMERA_IMGAE_LEN, xml);
		pCameraImgCfg->stWideDynamic.uiWideDynamicMode = static_cast<IVS_UINT32>(CDevEnumXML::GetCommStatusBool(cWideDynamicMode));
		
		// 宽动态等级
		GET_ELEM_VALUE_NUM_FOR_UINT("WideDynamicLevel", pTemp, pCameraImgCfg->stWideDynamic.uiWideDynamicLevel, xml);
		xml.OutOfElem();
	}
	// 锐度
	if (xml.FindElem("Sharpness"))
	{
		GET_ELEM_VALUE_NUM_FOR_UINT("Sharpness", pTemp, pCameraImgCfg->uiSharpness, xml);
	}
	// 白平衡参数
	if (xml.FindElem("WhiteBalanceInfo"))
	{
		(void)xml.IntoElem();
		// 白平衡模式
		IVS_CHAR cWhiteBalanceMode[CAMERA_IMGAE_LEN] = {0};
		GET_ELEM_VALUE_CHAR("WhiteBalanceMode", pTemp, cWhiteBalanceMode, CAMERA_IMGAE_LEN, xml);
		pCameraImgCfg->stWhiteBalance.uiWhiteBalanceMode = static_cast<IVS_UINT32>(CDevEnumXML::GetCommMode(cWhiteBalanceMode));

		// 红色增益
		GET_ELEM_VALUE_NUM_FOR_UINT("CrGain", pTemp, pCameraImgCfg->stWhiteBalance.uiCrGain, xml);

		// 蓝色增益
		GET_ELEM_VALUE_NUM_FOR_UINT("CbGain", pTemp, pCameraImgCfg->stWhiteBalance.uiCbGain, xml);
		xml.OutOfElem();
	}
	// 防抖动信息
	if (xml.FindElem("StabilizationInfo"))
	{
		(void)xml.IntoElem();
		// 防抖动模式
		IVS_CHAR cStabilizationMode[CAMERA_IMGAE_LEN] = {0};
		GET_ELEM_VALUE_CHAR("StabilizationMode", pTemp, cStabilizationMode, CAMERA_IMGAE_LEN, xml);
		pCameraImgCfg->stStabilization.uiStabilizationMode = static_cast<IVS_UINT32>(CDevEnumXML::GetCommExImgMode(cStabilizationMode));

		// 防抖动等级
		GET_ELEM_VALUE_NUM_FOR_UINT("StabilizationLevel", pTemp, pCameraImgCfg->stStabilization.uiStabilizationLevel, xml);
		xml.OutOfElem();
	}

	return IVS_SUCCEED;
}

// 查询摄像机扩展图像参数拼接XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetCameraImgCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_IMAGING_CFG* pCameraImgCfg = reinterpret_cast<IVS_CAMERA_IMAGING_CFG*>(pBuffer);
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();

	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(pDevCode);

	// 图像参数信息
	(void)xml.AddElem("ImagingInfo");
	(void)xml.IntoElem();

	// 背光补偿信息
	(void)xml.AddChildElem("BacklightInfo");
	(void)xml.IntoElem();
	// 背光补偿模式
	(void)xml.AddChildElem("BacklightMode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CDevEnumXML::GetCommStatusType(static_cast<IVS_INT32>(pCameraImgCfg->stBacklight.uiBacklightMode)));
	// 背光补偿等级
	(void)xml.AddElem("BacklightLevel");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraImgCfg->stBacklight.uiBacklightLevel)).c_str());
	xml.OutOfElem();

	// 曝光参数信息
	(void)xml.AddElem("ExposureInfo");
	(void)xml.IntoElem();
	// 曝光模式
	(void)xml.AddChildElem("ExposureMode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CDevEnumXML::GetCommModeStr(static_cast<IVS_INT32>(pCameraImgCfg->stExposure.uiExposureMode)));
	// 曝光优先
	(void)xml.AddElem("ExposurePriority");
	(void)xml.SetElemValue(CDevEnumXML::GetExposurePriorityStr(static_cast<IVS_INT32>(pCameraImgCfg->stExposure.uiExposurePriority)));
	// 曝光窗口信息
	(void)xml.AddElem("AreaInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("X1");
	(void)xml.IntoElem();
	IVS_CHAR cLeft[STR_LEN] = {0};
	if (!CToolsHelp::Snprintf(cLeft, STR_LEN, "%f", pCameraImgCfg->stExposure.rectWindow.left))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cLeft, STR_LEN, %f, pCameraImgCfg->stExposure.rectWindow.left) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cLeft);

	(void)xml.AddElem("Y1");
	(void)xml.IntoElem();
	IVS_CHAR cTop[STR_LEN] = {0};
	if (!CToolsHelp::Snprintf(cTop, STR_LEN, "%f", pCameraImgCfg->stExposure.rectWindow.top))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cTop, STR_LEN, %f, pCameraImgCfg->stExposure.rectWindow.top) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cTop);

	(void)xml.AddElem("X2");
	(void)xml.IntoElem();
	IVS_CHAR cRight[STR_LEN] = {0};
	if (!CToolsHelp::Snprintf(cRight, STR_LEN, "%f", pCameraImgCfg->stExposure.rectWindow.right))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cRight, STR_LEN, %f, pCameraImgCfg->stExposure.rectWindow.right) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cRight);

	(void)xml.AddElem("Y2");
	(void)xml.IntoElem();
	IVS_CHAR cBottom[STR_LEN] = {0};
	if (!CToolsHelp::Snprintf(cBottom, STR_LEN, "%f", pCameraImgCfg->stExposure.rectWindow.bottom))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Snprintf(cBottom, STR_LEN, %f, pCameraImgCfg->stExposure.rectWindow.bottom) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(cBottom);
	xml.OutOfElem();
	// 曝光时间
	(void)xml.AddElem("ExposureTime");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraImgCfg->stExposure.uiExposureTime)).c_str());
	// 曝光增益
	(void)xml.AddElem("ExposureGain");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraImgCfg->stExposure.uiExposureGain)).c_str());
	// 光圈
	(void)xml.AddElem("ExposureIris");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraImgCfg->stExposure.uiExposureIris)).c_str());
	xml.OutOfElem();

	// 变焦参数信息
	(void)xml.AddElem("FocusInfo");
	(void)xml.IntoElem();
	// 变焦模式
	(void)xml.AddChildElem("FocusMode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CDevEnumXML::GetCommModeStr(static_cast<IVS_INT32>(pCameraImgCfg->stFocus.uiFocusMode)));
	// 变焦速度
	(void)xml.AddElem("FocusSpeed");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraImgCfg->stFocus.uiFocusSpeed)).c_str());
	// 最小变焦范围
	(void)xml.AddElem("NearLimit");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraImgCfg->stFocus.uiNearLimit)).c_str());
	// 最大变焦范围
	(void)xml.AddElem("FarLimit");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraImgCfg->stFocus.uiFarLimit)).c_str());
	xml.OutOfElem();

	// 红外过滤
	(void)xml.AddElem("IrCutFilter");
	(void)xml.SetElemValue(CDevEnumXML::GetCommExImgModeStr(static_cast<IVS_INT32>(pCameraImgCfg->uiIrCutFilter)));

	// 宽动态信息
	(void)xml.AddElem("WideDynamic");
	(void)xml.IntoElem();
	// 宽动态模式
	(void)xml.AddChildElem("WideDynamicMode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CDevEnumXML::GetCommStatusType(static_cast<IVS_INT32>(pCameraImgCfg->stWideDynamic.uiWideDynamicMode)));
	// 宽动态等级
	(void)xml.AddElem("WideDynamicLevel");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraImgCfg->stWideDynamic.uiWideDynamicLevel)).c_str());
	xml.OutOfElem();

	// 锐度
	(void)xml.AddElem("Sharpness");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraImgCfg->uiSharpness)).c_str());

	// 白平衡参数
	(void)xml.AddElem("WhiteBalanceInfo");
	(void)xml.IntoElem();
	// 白平衡模式
	(void)xml.AddChildElem("WhiteBalanceMode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CDevEnumXML::GetCommModeStr(static_cast<IVS_INT32>(pCameraImgCfg->stWhiteBalance.uiWhiteBalanceMode)));
	// 红色增益
	(void)xml.AddElem("CrGain");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraImgCfg->stWhiteBalance.uiCrGain)).c_str());
	// 蓝色增益
	(void)xml.AddElem("CbGain");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraImgCfg->stWhiteBalance.uiCbGain)).c_str());
	xml.OutOfElem();

	// 防抖动信息
	(void)xml.AddElem("StabilizationInfo");
	(void)xml.IntoElem();
	// 防抖动模式
	(void)xml.AddChildElem("StabilizationMode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CDevEnumXML::GetCommExImgModeStr(static_cast<IVS_INT32>(pCameraImgCfg->stStabilization.uiStabilizationMode)));
	// 防抖动等级
	(void)xml.AddElem("StabilizationLevel");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraImgCfg->stStabilization.uiStabilizationLevel)).c_str());
	xml.OutOfElem();

	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 设置摄像机扩展参数解析XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetCameraExCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_EXTEND_CFG* pCameraExCfg = reinterpret_cast<IVS_CAMERA_EXTEND_CFG*>(pBuffer); //lint !e818
	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Set CameraExCfg ParseXML fail", "xml.FindElem(Content) fail");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	if (!xml.FindElem("CameraExtendedInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Set CameraExCfg ParseXML fail", "xml.FindElem(CameraExtendedInfo) fail");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	const char* pTemp = NULL;
	GET_ELEM_VALUE_NUM("WatermarkEnable", pTemp, pCameraExCfg->bEnableWatermark, xml);

	GET_ELEM_VALUE_NUM("BandwidthAdaptive", pTemp, pCameraExCfg->bEnableBandwidthAdaptive, xml);

	GET_ELEM_VALUE_NUM("Cache", pTemp, pCameraExCfg->bEnableCache, xml);

	if (!xml.FindElem("MediaSecurityInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "MediaSecurityInfo fail", "xml.FindElem(MediaSecurityInfo)");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	GET_ELEM_VALUE_NUM("MediaSecurityEnable", pTemp, pCameraExCfg->stMediaSecurityCfg.bEnableMediaSecurity, xml);

	GET_ELEM_VALUE_NUM_FOR_UINT("MediaSecurityAlgorithm", pTemp, pCameraExCfg->stMediaSecurityCfg.uiMediaSecurityAlgorithm, xml);

	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询摄像机扩展参数拼接XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetCameraExCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_EXTEND_CFG* pCameraExCfg = reinterpret_cast<IVS_CAMERA_EXTEND_CFG*>(pBuffer);

	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");
	(void)xml.AddElem("CameraExtendedInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("WatermarkEnable");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pCameraExCfg->bEnableWatermark).c_str());

	(void)xml.AddElem("BandwidthAdaptive");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pCameraExCfg->bEnableBandwidthAdaptive).c_str());

	(void)xml.AddElem("Cache");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pCameraExCfg->bEnableCache).c_str());

	(void)xml.AddElem("MediaSecurityInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("MediaSecurityEnable");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(pCameraExCfg->stMediaSecurityCfg.bEnableMediaSecurity).c_str());

	(void)xml.AddElem("MediaSecurityAlgorithm");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraExCfg->stMediaSecurityCfg.uiMediaSecurityAlgorithm)).c_str());
	xml.OutOfElem();

	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 摄像机抓拍参数设置XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetCameraSnapshotCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_SNAPSHOT_CFG* pCameraSnapshotCfg = reinterpret_cast<IVS_CAMERA_SNAPSHOT_CFG*>(pBuffer);
	if (!xml.FindElemEx("Content/SnapInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Set CameraExCfg ParseXML fail", "xml.FindElem(CameraExtendedInfo) fail");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	const char* pTemp = NULL;

	GET_ELEM_VALUE_NUM_FOR_UINT("SnapCount", pTemp, pCameraSnapshotCfg->uiSnapCount, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("SnapInterval", pTemp, pCameraSnapshotCfg->uiSnapInterval, xml);

	xml.GetRootPos();
	return IVS_SUCCEED;
}

// 查询摄像机抓拍参数XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetCameraSnapshotCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_SNAPSHOT_CFG* pCameraSnapshotCfg = reinterpret_cast<IVS_CAMERA_SNAPSHOT_CFG*>(pBuffer);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDevCode);

	(void)xml.AddElem("SnapInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("SnapCount");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraSnapshotCfg->uiSnapCount)).c_str());

	(void)xml.AddElem("SnapInterval");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraSnapshotCfg->uiSnapInterval)).c_str());

	xml.GetRootPos();

	return IVS_SUCCEED;
}

// 摄像机前端录像码流参数设置XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetCameraRecordStreamCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_UINT32 uiStreamType = 0;
	IVS_CAMERA_RECORD_STREAM_CFG* pCameraRecordStream = (IVS_CAMERA_RECORD_STREAM_CFG*)(pBuffer);
	if (!xml.FindElemEx("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Set CameraRecordStreamCfg ParseXML fail", "xml.FindElem(Content) fail");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	const char* pTemp = NULL;

	GET_ELEM_VALUE_NUM_FOR_UINT("RecordStreamID", pTemp, uiStreamType, xml);
	pCameraRecordStream->uiStreamType = uiStreamType;

	xml.GetRootPos();
	return IVS_SUCCEED;
}

// 查询摄像机前端录像码流参数XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetCameraRecordStreamCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_RECORD_STREAM_CFG* pCameraRecordStream = (IVS_CAMERA_RECORD_STREAM_CFG*)(pBuffer);
	IVS_UINT32 uiStreamType = pCameraRecordStream->uiStreamType; 

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("RecordStreamID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiStreamType)).c_str());

	xml.GetRootPos();

	return IVS_SUCCEED;
}


// 设置摄像机FEC参数设置XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::SetCameraFECCfgParseXML(IVS_VOID*& pBuffer, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// IVS_UINT32 uiStreamType = 0;
	IVS_CAMERA_FEC_CFG* pCameraFEC = (IVS_CAMERA_FEC_CFG*)(pBuffer);
	if (!xml.FindElemEx("Content/FECInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Set CameraFECCfg ParseXML fail", "xml.FindElem(FECInfo) fail");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	const char* pTemp = NULL;

	IVS_CHAR cReliableTransmission[STR_LEN] = {0};
	GET_ELEM_VALUE_CHAR("ReliableTransmission", pTemp, cReliableTransmission, STR_LEN, xml);
	pCameraFEC->uiReliableTransmission = static_cast<IVS_UINT32>(CDevEnumXML::GetReliableTransTypeINT(cReliableTransmission));
	GET_ELEM_VALUE_NUM_FOR_UINT("DataDropPercent", pTemp, pCameraFEC->uiDataDropPercent, xml);

	xml.GetRootPos();
	return IVS_SUCCEED;

}

// 查询摄像机FEC参数XML
IVS_INT32 COCXDeviceParaConfigXMLProcess::GetCameraFECCfgGetXML(IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, CXml &xml)const
{
	CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CAMERA_FEC_CFG* pCameraFEC = (IVS_CAMERA_FEC_CFG*)(pBuffer);

	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");
	(void)xml.AddElem("FECInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ReliableTransmission");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CDevEnumXML::GetReliableTransTypeCHAR(static_cast<IVS_INT32>(pCameraFEC->uiReliableTransmission)));
	 
	if (RELIABLE_TRANSMISSION_FEC == pCameraFEC->uiReliableTransmission)
	{
	 	(void)xml.AddElem("DataDropPercent");
	 	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pCameraFEC->uiDataDropPercent)).c_str());
	}

	return IVS_SUCCEED;
}

