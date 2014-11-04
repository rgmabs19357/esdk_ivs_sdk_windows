#include "OCXMotionDetecMgrXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "SDKDef.h"
#include "IVS_Trace.h"

COCXMotionDetecMgrXMLProcess::COCXMotionDetecMgrXMLProcess()
{
}

COCXMotionDetecMgrXMLProcess::~COCXMotionDetecMgrXMLProcess()
{
}


//解析平台移动检测xml
IVS_INT32 COCXMotionDetecMgrXMLProcess::ParseSetMotionDetecXml(IVS_MOTION_DETECTION* pMotionDetection, CXml& xml)
{
	CHECK_POINTER(pMotionDetection, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_INT32 iNum = 0;
	(void)GetMotionDetecAreaInfoNum(xml, iNum);

	if (!xml.FindElemEx("Content/MotionDetectionInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Set Motion Detection Xml", "can not find Element(Content/MotionDetectionInfo) in xml");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* szElemValue = NULL;
	IVS_CHAR szTemp[11] = {0};
	GET_ELEM_VALUE_NUM_FOR_UINT("DetectInterval", szElemValue, pMotionDetection->uiDetectInterval, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("Sensitivity", szElemValue, pMotionDetection->uiSensitivity, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("AutoClearAlarmTime", szElemValue, pMotionDetection->uiAutoClearAlarmTime, xml);
	GET_ELEM_VALUE_CHAR("MotionDetectionEnable", szElemValue, szTemp, 10, xml);
	if (0 == strcmp("ON", szTemp))
	{
		pMotionDetection->bMotionDetectionEnable = 1;
	}
	if (0 == strcmp("OFF", szTemp))
	{
		pMotionDetection->bMotionDetectionEnable = 0;
	}
	if (!xml.FindElem("AreaInfoList"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Set Motion Detection Xml", "can not find Element(AreaInfoList) in xml");
		return IVS_XML_INVALID;
	}
	if (0 == iNum)
	{
		return IVS_SUCCEED;
	}

	if (iNum > IVS_MAX_MOTION_DETECTION_AREA_NUM)
	{
		pMotionDetection->uiRectNum = IVS_MAX_MOTION_DETECTION_AREA_NUM;
	}
	else
	{
		pMotionDetection->uiRectNum = (IVS_UINT32)iNum;
	}
	
	(void)xml.IntoElem();
	if (!xml.FindElem("AreaInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Set Motion Detection Xml", "can not find Element(AreaInfo) in xml");
		return IVS_XML_INVALID;
	}
	
	for(IVS_UINT32 i = 0; i < (pMotionDetection->uiRectNum); i++)
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
		(void)xml.NextElem();
	}

	return IVS_SUCCEED;
}

// 计算响应消息中AreaInfo的条数
IVS_INT32 COCXMotionDetecMgrXMLProcess::GetMotionDetecAreaInfoNum(CXml& xml, IVS_INT32& iNum)
{
	IVS_DEBUG_TRACE("");

	if (!xml.FindElemEx("Content/MotionDetectionInfo/AreaInfoList"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Get Motion Detection AreaInfo Num", "can not find Element Path(Content/MotionDetectionInfo/AreaInfoList) in xml");
		return IVS_SMU_XML_INVALID;
	}

	if (!xml.FindElem("AreaInfo"))// AreaInfo 是O字段，可以没有，如果没有直接返回0；
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

// 拼装平台移动检测响应消息xml
IVS_INT32 COCXMotionDetecMgrXMLProcess::SetMotionDetecGetXML(const IVS_MOTION_DETECTION* pMotionDetec, CXml& xml)
{
	IVS_DEBUG_TRACE("");

	// 开始拼装xml
	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");

	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");
	//xml.OutOfElem();

	(void)xml.AddElem("MotionDetectionInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("DetectInterval");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionDetec->uiDetectInterval)).c_str());
	(void)xml.AddElem("AutoClearAlarmTime");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionDetec->uiAutoClearAlarmTime)).c_str());
	(void)xml.AddElem("Sensitivity");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionDetec->uiSensitivity)).c_str());

	(void)xml.AddElem("MotionDetectionEnable");
	if (pMotionDetec->bMotionDetectionEnable)
	{
		(void)xml.SetElemValue("ON");
	}
	else
	{
		(void)xml.SetElemValue("OFF");
	}

	(void)xml.AddElem("AreaInfoList");
	(void)xml.IntoElem();

	for (IVS_UINT32 i = 0; i < (pMotionDetec->uiRectNum); i++)
	{
		const IVS_RECT_FLOAT& stRect = pMotionDetec->rectDetection[i];
		i == 0?(void)xml.AddChildElem("AreaInfo"):(void)xml.AddElem("AreaInfo");
		(void)xml.IntoElem();
		(void)xml.AddChildElem("X1");
		(void)xml.IntoElem();
		IVS_CHAR cLeft[STR_LEN] = {0};
		//sprintf(cLeft, "%f", stRect.left);
		if (!CToolsHelp::Snprintf(cLeft, STR_LEN, "%f", stRect.left))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR ,"Set Motion Detection Get XML", "copy left failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cLeft);

		(void)xml.AddElem("Y1");
		IVS_CHAR cTop[STR_LEN] = {0};
		//sprintf(cTop, "%f", stRect.top);
		if (!CToolsHelp::Snprintf(cTop, STR_LEN, "%f", stRect.top))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR ,"Set Motion Detection Get XML", "copy top failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cTop);

		(void)xml.AddElem("X2");
		IVS_CHAR cRight[STR_LEN] = {0};
		//sprintf(cRight, "%f", stRect.right);
		if (!CToolsHelp::Snprintf(cRight, STR_LEN, "%f", stRect.right))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR ,"Set Motion Detection Get XML", "copy right failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cRight);

		(void)xml.AddElem("Y2");
		IVS_CHAR cBottom[STR_LEN] = {0};
		//sprintf(cBottom, "%f", stRect.bottom);
		if (!CToolsHelp::Snprintf(cBottom, STR_LEN, "%f", stRect.right))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR ,"Set Motion Detection Get XML", "copy cBottom failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cBottom);
		xml.OutOfElem();
	}

	xml.OutOfElem();
	xml.OutOfElem();
	return IVS_SUCCEED;
}

//解析查询平台运动检测幅度数据xml
IVS_INT32 COCXMotionDetecMgrXMLProcess::ParseGetMotionRangeDataPlatformXml(const IVS_CHAR* pStartTime,const IVS_CHAR* pEndTime,const IVS_MOTION_RANGE_DATA_LIST* pMotionRangeData,IVS_INT32 iRetCode, CXml& xml)
{
	CHECK_POINTER(pStartTime, IVS_PARA_INVALID);
	CHECK_POINTER(pEndTime, IVS_PARA_INVALID);
	CHECK_POINTER(pMotionRangeData, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRetCode).c_str());

	(void)xml.AddElem("PageInfo");
	(void)xml.AddChildElem("RealNum");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionRangeData->uiTotal)).c_str());
	(void)xml.AddElem("FromIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionRangeData->stIndexRange.uiFromIndex)).c_str());
	(void)xml.AddElem("ToIndex");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionRangeData->stIndexRange.uiToIndex)).c_str());
	xml.OutOfElem();
	(void)xml.AddElem("FromTime");
	(void)xml.SetElemValue(pStartTime);
	(void)xml.AddElem("ToTime");
	(void)xml.SetElemValue(pEndTime);
	(void)xml.AddElem("TimeInterval");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionRangeData->uiTimeInterval)).c_str());
	if(pMotionRangeData->uiTotal>0)
	{
		(void)xml.AddElem("MotionList");
		IVS_INT32 iRealNum = static_cast<IVS_INT32>(pMotionRangeData->uiTotal);
		for (IVS_INT32 iIndex = 0;iIndex<iRealNum;iIndex++)
		{
			if(0 == iIndex)
			{
				(void)xml.AddChildElem("MotionData");
				(void)(xml.IntoElem());
			}
			else
			{
				(void)xml.AddElem("MotionData");
			}
			(void)xml.AddChildElem("StartTime");
			(void)(xml.IntoElem());

			IVS_CHAR cStartTime[IVS_TIME_LEN+1] = {0};
			bool bRet = CToolsHelp::Memcpy(cStartTime,IVS_TIME_LEN,pMotionRangeData->stMotionRangeDataInfo[iIndex].StartTime,IVS_TIME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Motion Range Data Platform Parse XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cStartTime);

			(void)xml.AddElem("EndTime");
			IVS_CHAR cEndTime[IVS_TIME_LEN+1];
			memset(cEndTime,0,IVS_TIME_LEN+1);
			bRet = CToolsHelp::Memcpy(cEndTime,IVS_TIME_LEN,pMotionRangeData->stMotionRangeDataInfo[iIndex].EndTime,IVS_TIME_LEN);
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Get Motion Range Data Platform Parse XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			(void)xml.SetElemValue(cEndTime);
			(void)xml.AddElem("Data");
			(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionRangeData->stMotionRangeDataInfo[iIndex].uiMotionData)).c_str());
			xml.OutOfElem();
		}
	}
	xml.OutOfElem();

	return IVS_SUCCEED;
}


