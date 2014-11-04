#include "PtzControlXMLProcess.h"
#include "ToolsHelp.h"
#include "IVS_Trace.h"

const IVS_INT32 iptzCount = 38;

const IVS_CHAR *g_szPTZ_Code[iptzCount] = {
	"",
	"PTZ_STOP",
	"PTZ_UP",
	"PTZ_DOWN",
	"PTZ_LEFT",
	"PTZ_UP_LEFT",
	"PTZ_DOWN_LEFT",
	"PTZ_RIGHT",
	"PTZ_UP_RIGHT",
	"PTZ_DOWN_RIGHT",
	"PTZ_AUTO",
	"PTZ_PREFAB_BIT_RUN",
	"PTZ_CRUISE_RUN",
	"PTZ_CRUISE_STOP",
	"PTZ_MODE_CRUISE_RUN",
	"PTZ_MODE_CRUISE_STOP",
	"PTZ_MENU_OPEN",
	"PTZ_MENU_EXIT",
	"PTZ_MENU_ENTER",
	"PTZ_FLIP",
	"PTZ_START",
	// 镜头控制，包括光圈、缩放、聚焦
	"PTZ_LENS_APERTURE_OPEN",
	"PTZ_LENS_APERTURE_CLOSE",
	"PTZ_LENS_ZOOM_IN",
	"PTZ_LENS_ZOOM_OUT",
	"PTZ_LENS_FOCAL_NEAT",
	"PTZ_LENS_FOCAL_FAR",
	"PTZ_AUX_OPEN",
	"PTZ_AUX_STOP",
	// 模式
	"MODE_SET_START",
	"MODE_SET_STOP",
	// 增强操作
	"PTZ_FAST_LOCATE",
	"PTZ_HORIZONTAL_SCAN",
	"PTZ_VERTICAL_SCAN",
	"PTZ_LOCK",
	"PTZ_UNLOCK",
	"PTZ_ABSOLUTE_MOVE",
	"PTZ_DIRECTION_MOVE"
	//end
};

//???????xml
IVS_INT32 PtzControlProcess::GetPtzPositionGetXML(const IVS_CHAR* pCameraCode, CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	//????????????
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	//???????xml
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDomaCode);
	(void)xml.AddElem("CameraInfo");
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	xml.OutOfElem();

	return IVS_SUCCEED;
}

//???????xml
IVS_INT32 PtzControlProcess::GetPtzPositionParseXML(const std::string& strRsp, IVS_PTZ_ABSPOSITION* pPTZPosInfo)
{
	CHECK_POINTER(pPTZPosInfo, IVS_OPERATE_MEMORY_ERROR);
	memset(pPTZPosInfo, 0, sizeof(IVS_PTZ_ABSPOSITION));

	CXml xml;
	CHECK_SUCCESS_RET(xml.Parse(strRsp.c_str()), IVS_SMU_XML_INVALID);

	//???????
	CHECK_SUCCESS_RET(xml.FindElemEx("Content/CameraCode"), IVS_SMU_XML_INVALID);
	const char *pCameraCode = xml.GetElemValue();
	if (NULL != pCameraCode)
	{
		(void)CToolsHelp::Memcpy(pPTZPosInfo->cCameraCode, IVS_DEV_CODE_LEN, pCameraCode, strlen(pCameraCode));
	}

	//????AttriValue,????CXml::GetElemValue????
	const char *AttriValue = NULL;

	//????????
	CHECK_SUCCESS_RET(xml.FindElemEx("Content/PTZPosition"), IVS_SMU_XML_INVALID);
	xml.IntoElem();
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("X", AttriValue, pPTZPosInfo->stPTZPosition.dX, xml);
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("Y", AttriValue, pPTZPosInfo->stPTZPosition.dY, xml);
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("Z", AttriValue, pPTZPosInfo->stPTZPosition.dZ, xml);
	xml.OutOfElem();

	//??????????
	CHECK_SUCCESS_RET(xml.FindElemEx("Content/PTZRange"), IVS_SMU_XML_INVALID);
	xml.IntoElem();
	//???????????????
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("XMax", AttriValue, pPTZPosInfo->stPTZRange.dXMax, xml);
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("XMin", AttriValue, pPTZPosInfo->stPTZRange.dXMin, xml);
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("YMax", AttriValue, pPTZPosInfo->stPTZRange.dYMax, xml);
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("YMin", AttriValue, pPTZPosInfo->stPTZRange.dYMin, xml);
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("ZMax", AttriValue, pPTZPosInfo->stPTZRange.dZMax, xml);
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("ZMin", AttriValue, pPTZPosInfo->stPTZRange.dZMin, xml);
	//??????????
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("VxMax", AttriValue, pPTZPosInfo->stPTZRange.dVXMax, xml);
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("VxMin", AttriValue, pPTZPosInfo->stPTZRange.dVXMin, xml);
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("VyMax", AttriValue, pPTZPosInfo->stPTZRange.dVYMax, xml);
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("VyMin", AttriValue, pPTZPosInfo->stPTZRange.dVYMin, xml);
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("VzMax", AttriValue, pPTZPosInfo->stPTZRange.dVZMax, xml);
	GET_ELEM_VALUE_NUM_FOR_DOUBLE("VzMin", AttriValue, pPTZPosInfo->stPTZRange.dVZMin, xml);
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// ???????xml
IVS_INT32 PtzControlProcess::PtzControlGetXML(const IVS_CHAR* pCameraCode,IVS_INT32 iControlCode,const IVS_CHAR* pControlPara1,const IVS_CHAR* pControlPara2,CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pControlPara1, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pControlPara2, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	if (iControlCode >= iptzCount||iControlCode<0)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"PtzControlProcess::PtzControlGetXML","ControlCode is invalid");
		return IVS_PARA_INVALID;
	}

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDomaCode);
	(void)xml.AddElem("PTZ");
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("OPCode");
	(void)xml.SetElemValue(g_szPTZ_Code[iControlCode]);
	(void)xml.AddElem("Param1");
	(void)xml.SetElemValue(pControlPara1);
	(void)xml.AddElem("Param2");
	(void)xml.SetElemValue(pControlPara2);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

//解析云镜控制的xml
IVS_INT32 PtzControlProcess::PtzControlParseXML(const std::string& strRsp, IVS_PTZ_CONTROL_INFO* pPtzControlInfo)
{
	CHECK_POINTER(pPtzControlInfo, IVS_OPERATE_MEMORY_ERROR);
	memset(pPtzControlInfo, 0, sizeof(IVS_PTZ_CONTROL_INFO));
	CXml xml;
	CHECK_SUCCESS_RET(xml.Parse(strRsp.c_str()), IVS_SMU_XML_INVALID);

	CHECK_SUCCESS_RET(xml.FindElemEx("Content/LockStatus"), IVS_SMU_XML_INVALID);
	const char* pLockStatus = xml.GetElemValue();
	CHECK_POINTER(pLockStatus, IVS_SMU_XML_INVALID);
	pPtzControlInfo->uiLockStatus = static_cast<IVS_UINT32>(atoi(pLockStatus));

	if (xml.FindElemEx("Content/LockUserInfo"))
	{
		xml.IntoElem();
		const char* AttriValue = NULL;
		GET_ELEM_VALUE_NUM("UserID", AttriValue, pPtzControlInfo->uiUserID, xml);
		GET_ELEM_VALUE_CHAR("UserName", AttriValue, pPtzControlInfo->cUserName, IVS_NAME_LEN, xml);
		GET_ELEM_VALUE_CHAR("LoginIP", AttriValue, pPtzControlInfo->stIP.cIP, IVS_IP_LEN, xml);
		GET_ELEM_VALUE_NUM("ReleaseTimeRemain", AttriValue, pPtzControlInfo->uiReleaseTimeRemain, xml);
	}

	return IVS_SUCCEED;
}

// 拼装增加预置位的xml
IVS_INT32 PtzControlProcess::AddPTZPresetGetXML(const IVS_CHAR* pCameraCode, const IVS_CHAR* pPresetName,CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pPresetName, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chAddPTZDomainCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chAddPTZDomainCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chAddPTZDomainCode);
	(void)xml.AddElem("PresetPTZ");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("Name");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pPresetName);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 解析增加预置位的xml
IVS_INT32 PtzControlProcess::AddPTZPresetParseXML(const std::string& strRsp, IVS_UINT32* uiPresetIndex)
{
	CHECK_POINTER(uiPresetIndex, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strRsp.c_str()))
	{
		BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
		return iRet;
	}
	if (!xml.FindElemEx("Content/PresetPTZ/Index"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "cannot find index", "NA");
		return iRet;
	}
	const char* pPresetIndex = xml.GetElemValue();
	CHECK_POINTER(pPresetIndex, IVS_XML_INVALID);
	*uiPresetIndex = static_cast<IVS_UINT32>(atoi(pPresetIndex));
	return IVS_SUCCEED;
}

// 拼装删除预置位的xml
IVS_INT32 PtzControlProcess::DelPTZPresetGetXML(const IVS_CHAR* pCameraCode, IVS_UINT32 uiPresetIndex, CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDelPTZDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDelPTZDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDelPTZDomaCode);
	(void)xml.AddElem("PresetPTZ");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("Index");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)uiPresetIndex).c_str());
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 拼装修改预置位的xml
IVS_INT32 PtzControlProcess::ModPTZPresetGetXML(const IVS_CHAR* pCameraCode, const IVS_PTZ_PRESET* pPTZPreset, CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pPTZPreset, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chModPTZDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chModPTZDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chModPTZDomaCode);
	(void)xml.AddElem("PresetPTZ");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("Index");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pPTZPreset->uiPresetIndex)).c_str());
	(void)xml.AddElem("Name");
	(void)xml.IntoElem();
	IVS_CHAR szTemp[IVS_PRESET_NAME_LEN + 1] = {0};
	if (!CToolsHelp::Memcpy(szTemp, IVS_PRESET_NAME_LEN + 1, pPTZPreset->cPresetName, IVS_PRESET_NAME_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy strDomainCode failed", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szTemp);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 拼装获取预置位的xml
IVS_INT32 PtzControlProcess::GetPTZPresetListGetXML(const IVS_CHAR* pCameraCode, CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chGetPTZDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chGetPTZDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chGetPTZDomaCode);
	(void)xml.AddElem("PresetPTZ");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 解析获取预置位的xml
IVS_INT32 PtzControlProcess::GetPTZPresetListParseXML(const std::string& strRsp, IVS_PTZ_PRESET* pPTZPresetList, IVS_UINT32 /*uiBufferSize*/, IVS_UINT32* uiPTZPresetNum)
{
	CHECK_POINTER(pPTZPresetList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(uiPTZPresetNum, IVS_OPERATE_MEMORY_ERROR);

	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strRsp.c_str()))
	{
		BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
		return iRet;
	}
	if (!xml.FindElemEx("Content/PresetPTZList"))
	{
		iRet = IVS_SUCCEED;
		*uiPTZPresetNum = 0;
		BP_RUN_LOG_INF("preset list is empty","NA");
		return iRet;
	}

	unsigned int i = 0;
	do 
	{
		if (!xml.FindElemValue("PresetPTZ"))
		{
			break;
		}

		IVS_PTZ_PRESET& stPTZPreset = *(pPTZPresetList + i);
		(void)xml.IntoElem();
		if (xml.FindElem("Index"))
		{
			const char* pPresetIndex = xml.GetElemValue();
			if (NULL != pPresetIndex)
			{
				stPTZPreset.uiPresetIndex = static_cast<IVS_UINT32>(atoi(pPresetIndex));
			}
		}

		if (xml.FindElem("Name"))
		{
            const IVS_CHAR* pName = xml.GetElemValue();
            if (NULL != pName)
            {
			    (void)CToolsHelp::Memcpy(stPTZPreset.cPresetName, IVS_PRESET_NAME_LEN, pName, strlen(pName));
            }
        }
		xml.OutOfElem();
		i++;
	} while (xml.NextElem() && i < IVS_MAX_PRESET_NUM);
	*uiPTZPresetNum = i;
	return IVS_SUCCEED;
}

// 拼装设置看守位xml
IVS_INT32 PtzControlProcess::SetGuardPosGetXML(const IVS_CHAR* pCameraCode, const IVS_GUARD_POS_INFO* pGuardPosInfo, CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pGuardPosInfo, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chSetGuardDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chSetGuardDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chSetGuardDomaCode);
	(void)xml.AddElem("GuardPositionPara");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("Enabled");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pGuardPosInfo->bEnableGuardPosition)).c_str());
	(void)xml.AddElem("WaitTime");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pGuardPosInfo->uiWaitTime)).c_str());
	(void)xml.AddElem("PresetIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pGuardPosInfo->uiPresetIndex)).c_str());
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}


// 拼装删除看守位xml
IVS_INT32 PtzControlProcess::DelGuardPosGetXML(const IVS_CHAR* pCameraCode, IVS_UINT32 uiPresetIndex, CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDelGuardDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};

	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDelGuardDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDelGuardDomaCode);
	(void)xml.AddElem("GuardPositionPara");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("PresetIndex");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)uiPresetIndex).c_str());
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}


// 拼装获取看守位xml
IVS_INT32 PtzControlProcess::GetGuardPosGetXML(const IVS_CHAR* pCameraCode, CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chGetGuardDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chGetGuardDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chGetGuardDomaCode);
	(void)xml.AddElem("GuardPositionPara");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 解析获取看守位xml
IVS_INT32 PtzControlProcess::GetGuardPosParseXML(const std::string& strRsp, IVS_GUARD_POS_INFO* pGuardPosInfo)
{
	CHECK_POINTER(pGuardPosInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strRsp.c_str()))
	{
		BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
		return iRet;
	}
	if (!xml.FindElemEx("Content/GuardPositionPara/Enabled"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "cannot find Enabled", "NA");
		return iRet;
	}
	const char* pbEnableGuardPosition = xml.GetElemValue();
	CHECK_POINTER(pbEnableGuardPosition, IVS_XML_INVALID);
	pGuardPosInfo->bEnableGuardPosition = atoi(pbEnableGuardPosition);

	if (!xml.FindElemEx("Content/GuardPositionPara/WaitTime"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "cannot find Enabled", "NA");
		return iRet;
	}
	const char* pWaitTime = xml.GetElemValue();
	CHECK_POINTER(pWaitTime, IVS_XML_INVALID);
	pGuardPosInfo->uiWaitTime = static_cast<IVS_UINT32>(atoi(pWaitTime));

	if (!xml.FindElemEx("Content/GuardPositionPara/PresetIndex"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "cannot find Enabled", "NA");
		return iRet;
	}
	const char* pPresetIndex = xml.GetElemValue();
	CHECK_POINTER(pPresetIndex, IVS_XML_INVALID);
	pGuardPosInfo->uiPresetIndex = static_cast<IVS_UINT32>(atoi(pPresetIndex));
	return IVS_SUCCEED;
}

// 拼装增加巡航轨迹xml
IVS_INT32 PtzControlProcess::AddCruiseTrackGetXML(const IVS_CHAR* pCameraCode, const IVS_CRUISE_TRACK* pCuriseTrackInfo, CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCuriseTrackInfo, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chAddCruiseDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chAddCruiseDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chAddCruiseDomaCode);
	(void)xml.AddElem("CruiseTrack");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("Name");
	(void)xml.IntoElem();
	IVS_CHAR szTemp[IVS_CURISE_TRACK_NAME_LEN + 1] = {0};
	(void)CToolsHelp::Memcpy(szTemp, IVS_CURISE_TRACK_NAME_LEN + 1, pCuriseTrackInfo->cTrackName, IVS_CURISE_TRACK_NAME_LEN);
	(void)xml.SetElemValue(szTemp);
	(void)xml.AddElem("CruiseType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pCuriseTrackInfo->uiCruiseType)).c_str());
	(void)xml.AddElem("CruisePointList");
	(void)xml.IntoElem();

	for (unsigned int i=0; i < pCuriseTrackInfo->uiTrackPointNum; i++)
	{
		const IVS_CRUISE_TRACK_POINT& stCruiseTrackPoint = pCuriseTrackInfo->stTrackPoint[i];
		i==0?(void)xml.AddChildElem("CruisePoint"):(void)xml.AddElem("CruisePoint");	
		(void)xml.IntoElem();
		(void)xml.AddChildElem("PresetIndex");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTrackPoint.uiPresetIndex).c_str());
		(void)xml.AddElem("DwellTime");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTrackPoint.uiDwellTime).c_str());
		(void)xml.AddElem("Speed");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTrackPoint.uiSpeed).c_str());
		xml.OutOfElem();
	}

	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 解析增加巡航轨迹xml
IVS_INT32 PtzControlProcess::AddCruiseTrackGetParseXML(const std::string& strRsp, IVS_UINT32* uiTrackIndex)
{
	CHECK_POINTER(uiTrackIndex, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strRsp.c_str()))
	{
		BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
		return iRet;
	}
	if (!xml.FindElemEx("Content/CruisePoint/CruiseNO"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "cannot find CruiseNO", "NA");
		return iRet;
	}
	const char* pTrackIndex = xml.GetElemValue();
	CHECK_POINTER(pTrackIndex, IVS_XML_INVALID);
	*uiTrackIndex = static_cast<IVS_UINT32>(atoi(pTrackIndex));
	return IVS_SUCCEED;
}

// 拼装删除巡航轨迹xml
IVS_INT32 PtzControlProcess::DelCruiseTrackGetXML(const IVS_CHAR* pCameraCode, IVS_UINT32 uiTrackIndex, IVS_UINT32 uiCruiseType, CXml &xml)
{
	
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDelCruiseDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDelCruiseDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDelCruiseDomaCode);
	(void)xml.AddElem("CruiseTrack");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("CruiseNO");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)uiTrackIndex).c_str());
	(void)xml.AddElem("CruiseType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)uiCruiseType).c_str());
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 拼装修改巡航轨迹xml
IVS_INT32 PtzControlProcess::ModCruiseTrackGetXML(const IVS_CHAR* pCameraCode, const IVS_CRUISE_TRACK* pCuriseTrackInfo, CXml &xml)
{
	
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCuriseTrackInfo, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chModCruiseDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chModCruiseDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chModCruiseDomaCode);
	(void)xml.AddElem("CruiseTrack");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("CruiseNO");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pCuriseTrackInfo->uiCruiseNO)).c_str());
	(void)xml.AddElem("Name");
	(void)xml.IntoElem();
	IVS_CHAR szTemp[IVS_CURISE_TRACK_NAME_LEN + 1] = {0};
	(void)CToolsHelp::Memcpy(szTemp, IVS_CURISE_TRACK_NAME_LEN + 1, pCuriseTrackInfo->cTrackName, IVS_CURISE_TRACK_NAME_LEN);
	(void)xml.SetElemValue(szTemp);
	(void)xml.AddElem("CruiseType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(pCuriseTrackInfo->uiCruiseType)).c_str());
	(void)xml.AddElem("CruisePointList");
	(void)xml.IntoElem();

	for (unsigned int i=0; i < pCuriseTrackInfo->uiTrackPointNum; i++)
	{
		const IVS_CRUISE_TRACK_POINT& stCruiseTrackPoint = pCuriseTrackInfo->stTrackPoint[i];
		i==0?(void)xml.AddChildElem("CruisePoint"):(void)xml.AddElem("CruisePoint");	
		(void)xml.IntoElem();
		(void)xml.AddChildElem("PresetIndex");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTrackPoint.uiPresetIndex).c_str());
		(void)xml.AddElem("DwellTime");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTrackPoint.uiDwellTime).c_str());
		(void)xml.AddElem("Speed");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTrackPoint.uiSpeed).c_str());
		xml.OutOfElem();
	}

	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 拼装获取巡航轨迹列表xml
IVS_INT32 PtzControlProcess::GetCruiseTrackListGetXML(const IVS_CHAR* pCameraCode, CXml &xml)
{
	
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chGetCruiseDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chGetCruiseDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chGetCruiseDomaCode);
	(void)xml.AddElem("CruiseTrack");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 解析获取巡航轨迹列表xml
IVS_INT32 PtzControlProcess::GetCruiseTrackListParseXML(const std::string& strRsp, IVS_CRUISE_TRACK* pCruiseTrackList, IVS_UINT32 /*uiBufferSize*/, IVS_UINT32* uiCruiseTrackNum)
{
	CHECK_POINTER(pCruiseTrackList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(uiCruiseTrackNum, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strRsp.c_str()))
	{
		BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
		return iRet;
	}
	if (!xml.FindElemEx("Content/CruiseTrackList"))
	{
		iRet = IVS_SUCCEED;
		*uiCruiseTrackNum = 0;
		BP_RUN_LOG_INF("preset list is empty","NA");
		return iRet;
	}

	unsigned int i = 0;
	do 
	{
		if (!xml.FindElemValue("CruiseTrack"))
		{
			break;
		}

		IVS_CRUISE_TRACK& stCruiseTrack = *(pCruiseTrackList + i);
		(void)xml.IntoElem();
		if (xml.FindElem("CruiseNO"))
		{
			const char* pCruiseNO = xml.GetElemValue();
			if (NULL != pCruiseNO)
			{
				stCruiseTrack.uiCruiseNO = static_cast<IVS_UINT32>(atoi(pCruiseNO));
			}
		}

		if (xml.FindElem("Name"))
		{
            const IVS_CHAR* pName = xml.GetElemValue();
            if (NULL != pName)
            {
               (void)CToolsHelp::Memcpy(stCruiseTrack.cTrackName, IVS_PRESET_NAME_LEN, pName, strlen(pName));
            }
	
		}

		if (xml.FindElem("CruiseType"))
		{
			const char* pCruiseType = xml.GetElemValue();
			if (NULL != pCruiseType)
			{
				stCruiseTrack.uiCruiseType = static_cast<IVS_UINT32>(atoi(pCruiseType));
			}
		}
		xml.OutOfElem();
		i++;
	} while (xml.NextElem() && i < IVS_MAX_CURISE_TRACK_NUM);
	*uiCruiseTrackNum = i;
	return IVS_SUCCEED;
}

// 拼装获取巡航轨迹详情xml
IVS_INT32 PtzControlProcess::GetCruiseTrackGetXML(const IVS_CHAR* pCameraCode, IVS_INT32 uiTrackIndex, CXml &xml)
{
	
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chGetCruiseTDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chGetCruiseTDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chGetCruiseTDomaCode);
	(void)xml.AddElem("CruiseTrack");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("CruiseNO");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)uiTrackIndex).c_str());
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 解析获取巡航轨迹详情xml
IVS_INT32 PtzControlProcess::GetCruiseTrackParseXML(const std::string& strRsp, IVS_CRUISE_TRACK* pCruiseTrackInfo)
{
	CHECK_POINTER(pCruiseTrackInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	CXml xml;
	if (!xml.Parse(strRsp.c_str()))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
		return iRet;
	}

	if (!xml.FindElemEx("Content/CruiseTrack/CruiseNO"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CruiseNO", "NA");
		return iRet;
	}
	const char* pCruiseNO = xml.GetElemValue();
	CHECK_POINTER(pCruiseNO, IVS_XML_INVALID);
	pCruiseTrackInfo->uiCruiseNO = static_cast<IVS_UINT32>(atoi(pCruiseNO));

	if (!xml.FindElemEx("Content/CruiseTrack/Name"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find Name", "NA");
		return iRet;
	}
    const IVS_CHAR* pName = xml.GetElemValue();
    if (NULL != pName)
    {
        (void)CToolsHelp::Memcpy(pCruiseTrackInfo->cTrackName, IVS_CURISE_TRACK_NAME_LEN, pName, strlen(pName));

    }
	
	if (!xml.FindElemEx("Content/CruiseTrack/CruiseType"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CruiseType", "NA");
		return iRet;
	}
	const char* pCruiseType = xml.GetElemValue();
	CHECK_POINTER(pCruiseType, IVS_XML_INVALID);
	pCruiseTrackInfo->uiCruiseType = static_cast<IVS_UINT32>(atoi(pCruiseType));

	if (!xml.FindElemEx("Content/CruiseTrack/CruisePointList"))
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "can not find CruisePointList", "NA");
		return iRet;
	}

	unsigned int i = 0;
	do 
	{
		if (!xml.FindElemValue("CruisePoint"))
		{
			break;
		}

		IVS_CRUISE_TRACK_POINT& stCuriseTrackPoint = pCruiseTrackInfo->stTrackPoint[i];
		(void)xml.IntoElem();
		if (xml.FindElem("PresetIndex"))
		{
			const char* pPresetIndex = xml.GetElemValue();
			if (NULL != pPresetIndex)
			{
				stCuriseTrackPoint.uiPresetIndex = static_cast<IVS_UINT32>(atoi(pPresetIndex));
			}
		}

		if (xml.FindElem("DwellTime"))
		{
			const char* pDwellTime = xml.GetElemValue();
			if (NULL != pDwellTime)
			{
				stCuriseTrackPoint.uiDwellTime = static_cast<IVS_UINT32>(atoi(pDwellTime));
			}
		}

		if (xml.FindElem("Speed"))
		{
			const char* pSpeed = xml.GetElemValue();
			if (NULL != pSpeed)
			{
				stCuriseTrackPoint.uiSpeed = static_cast<IVS_UINT32>(atoi(pSpeed));
			}
		}

		xml.OutOfElem();
		i++;
	} while (xml.NextElem() && i < IVS_MAX_CURISE_TRACK_POINT);
	
	pCruiseTrackInfo->uiTrackPointNum = i;
	return IVS_SUCCEED;
}

// 拼装设置巡航计划xml
IVS_INT32 PtzControlProcess::SetCruisePlanGetXML(const IVS_CHAR* pCameraCode, const IVS_CRUISE_PLAN* pCruisePlan, CXml &xml)
{
	
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chSetCruisePlanDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chSetCruisePlanDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chSetCruisePlanDomaCode);
	(void)xml.AddElem("CruisePlanInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	(void)xml.AddElem("PlanType");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pCruisePlan->uiPlanType).c_str());
	(void)xml.AddElem("IsEnable");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((int)pCruisePlan->bEnable).c_str());
	(void)xml.AddElem("PlanList");
	(void)xml.IntoElem();

	for (unsigned int i=0; i < pCruisePlan->uiPlanInfoNum; i++)
	{
		const IVS_CRUISE_PLAN_INFO& stCruisePlanInfo = pCruisePlan->stPlanInfo[i];
		i==0?(void)xml.AddChildElem("PlanInfo"):(void)xml.AddElem("PlanInfo");	
		(void)xml.IntoElem();
		(void)xml.AddChildElem("DayType");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruisePlanInfo.uiDayType).c_str());
		(void)xml.AddElem("TimeInfoNum");
		(void)xml.IntoElem();
		(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruisePlanInfo.uiTimeSpanInfoNum).c_str());
		(void)xml.AddElem("TimeList");
		(void)xml.IntoElem();
		for (unsigned int j = 0; j < stCruisePlanInfo.uiTimeSpanInfoNum; j++)
		{
			const IVS_CRUISE_TIME_SPAN& stCruiseTimeInfo = stCruisePlanInfo.stTimeSpanInfo[j];
			j==0?(void)xml.AddChildElem("TimeInfo"):(void)xml.AddElem("TimeInfo");	
			(void)xml.IntoElem();
			(void)xml.AddChildElem("StartTime");
			(void)xml.IntoElem();
			IVS_CHAR szTemp[IVS_TIME_LEN + 1] = {0};
			(void)CToolsHelp::Memcpy(szTemp, IVS_TIME_LEN + 1, stCruiseTimeInfo.cStart, IVS_TIME_LEN);
			(void)xml.SetElemValue(szTemp);
			(void)xml.AddElem("EndTime");
			(void)xml.IntoElem();
			(void)CToolsHelp::Memcpy(szTemp, IVS_TIME_LEN + 1, stCruiseTimeInfo.cEnd, IVS_TIME_LEN);
			(void)xml.SetElemValue(szTemp);
			(void)xml.AddElem("TrackInfo");
			(void)xml.IntoElem();
			(void)xml.AddChildElem("TrackType");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTimeInfo.uiCruiseType).c_str());
			(void)xml.AddElem("TrackIndex");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(CToolsHelp::Int2Str((int)stCruiseTimeInfo.uiCruiseNO).c_str());
			xml.OutOfElem();
			xml.OutOfElem();
		}
		xml.OutOfElem();
		xml.OutOfElem();
	}
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 拼装获取巡航计划xml
IVS_INT32 PtzControlProcess::GetCruisePlanGetXML(const IVS_CHAR* pCameraCode, CXml &xml)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chGetCruisePlanDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chGetCruisePlanDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chGetCruisePlanDomaCode);
	(void)xml.AddElem("CruisePlanInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode);
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

IVS_INT32 PtzControlProcess::GetCruisePlanParseXML(const std::string& strRsp, IVS_CRUISE_PLAN* pCruisePlan)
{
	CHECK_POINTER(pCruisePlan, IVS_OPERATE_MEMORY_ERROR);
	CXml xml;
	CHECK_SUCCESS_RET(xml.Parse(strRsp.c_str()), IVS_FAIL);

	CHECK_SUCCESS_RET(xml.FindElemEx("Content/CruisePlanInfo/IsEnable"), IVS_SMU_XML_INVALID);
	const char* pEnable = xml.GetElemValue();
	CHECK_POINTER(pEnable, IVS_XML_INVALID);
	pCruisePlan->bEnable = static_cast<IVS_INT32>(atoi(pEnable));
	CHECK_SUCCESS_RET(xml.FindElemEx("Content/CruisePlanInfo/PlanType"), IVS_SMU_XML_INVALID);
	const char* pPlanType = xml.GetElemValue();
	CHECK_POINTER(pPlanType, IVS_XML_INVALID);
	pCruisePlan->uiPlanType = static_cast<IVS_UINT32>(atoi(pPlanType));
	CHECK_SUCCESS_RET(xml.FindElemEx("Content/CruisePlanInfo/PlanList"), IVS_SMU_XML_INVALID);
	IVS_UINT32 i = 0;
	do 
	{
		if (!xml.FindElemValue("PlanInfo"))
		{
			break;
		}

		IVS_CRUISE_PLAN_INFO& stCruisePlanInfo = pCruisePlan->stPlanInfo[i];
		(void)xml.IntoElem();
		if (xml.FindElem("DayType"))
		{
			const char* pDayType = xml.GetElemValue();
			if (NULL != pDayType)
			{
				stCruisePlanInfo.uiDayType = static_cast<IVS_UINT32>(atoi(pDayType));
			}
		}

		if (xml.FindElem("TimeInfoNum"))
		{
			const char* pTimeSpanInfoNum = xml.GetElemValue();
			if (NULL != pTimeSpanInfoNum)
			{
				stCruisePlanInfo.uiTimeSpanInfoNum = static_cast<IVS_UINT32>(atoi(pTimeSpanInfoNum));
			}
		}

		CHECK_SUCCESS_RET(xml.FindElem("TimeList"), IVS_SMU_XML_INVALID);
		(void)xml.IntoElem();
		IVS_UINT32 j = 0;
		do 
		{
			CHECK_SUCCESS_RET(xml.FindElemValue("TimeInfo"), IVS_SMU_XML_INVALID);
			(void)xml.IntoElem(); 
			IVS_CRUISE_TIME_SPAN& stCuriseTimeSpanInfo = stCruisePlanInfo.stTimeSpanInfo[j];
			if (xml.FindElem("StartTime"))
			{
                const IVS_CHAR* pStartTime = xml.GetElemValue();
                if (NULL != pStartTime)
                {
                    (void)CToolsHelp::Memcpy(stCuriseTimeSpanInfo.cStart, IVS_TIME_LEN, pStartTime, strlen(pStartTime));
                }
				
			}
			//(void)xml.IntoElem();
			if (xml.FindElem("EndTime"))
			{
                const IVS_CHAR* pEndTime = xml.GetElemValue();
                if (NULL != pEndTime)
                {
                    (void)CToolsHelp::Memcpy(stCuriseTimeSpanInfo.cEnd, IVS_TIME_LEN, pEndTime, strlen(pEndTime));
                }
				
			}

			CHECK_SUCCESS_RET(xml.FindElem("TrackInfo"), IVS_SMU_XML_INVALID);
			(void)xml.IntoElem();
			if (xml.FindElem("TrackType"))
			{
				const char* pCruiseType = xml.GetElemValue();
				if (NULL != pCruiseType)
				{
					stCuriseTimeSpanInfo.uiCruiseType = static_cast<IVS_UINT32>(atoi(pCruiseType));
				}
			}
			if (xml.FindElem("TrackIndex"))
			{
				const char* pCruiseNO = xml.GetElemValue();
				if (NULL != pCruiseNO)
				{
					stCuriseTimeSpanInfo.uiCruiseNO = static_cast<IVS_UINT32>(atoi(pCruiseNO));
				}
			}
			xml.OutOfElem();
			xml.OutOfElem();
			j++;
		} while (xml.NextElem() && j < IVS_MAX_TIMESPAN_NUM);
		stCruisePlanInfo.uiTimeSpanInfoNum = j;

		xml.OutOfElem();
		xml.OutOfElem();
		i++;
	} while (xml.NextElem() && i < IVS_MAX_PLAN_INFO_NUM);
	pCruisePlan->uiPlanInfoNum = i;

	return IVS_SUCCEED;
}
