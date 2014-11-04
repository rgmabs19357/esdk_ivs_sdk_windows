#include "MotionDetecMgrXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "IVS_Trace.h"
#include "bp_os.h"


CMotionDetecMgrXMLProcess::CMotionDetecMgrXMLProcess()
{
}

CMotionDetecMgrXMLProcess::~CMotionDetecMgrXMLProcess()
{
}


// 拼装查询运动检测请求xml
IVS_INT32 CMotionDetecMgrXMLProcess::GetMotionDetecXML(const IVS_CHAR* pLoginId, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode, CXml& xml)
{
	CHECK_POINTER(pLoginId, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("LoginInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("LoginID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pLoginId);
	xml.OutOfElem();

	(void)xml.AddElem("DomainCode");
	(void)xml.SetElemValue(pDomainCode);

	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(pCameraCode);

	xml.OutOfElem();
	xml.OutOfElem();
	return IVS_SUCCEED;
}

// 计算响应消息中AreaInfo的条数
IVS_INT32 CMotionDetecMgrXMLProcess::GetMotionDetecAreaInfoNum(CXml& xml, IVS_INT32& iNum)
{
	IVS_DEBUG_TRACE("");

	if (!xml.FindElemEx("Content/MotionDetectionInfo/AreaInfoList"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "CMotionDetecMgrXMLProcess::GetMotionDetecAreaInfoNum", "xml.FindElemEx(Content/MotionDetectionInfo/AreaInfoList)");
		return IVS_SMU_XML_INVALID;
	}

	// AreaInfo 是O字段，可以没有，如果没有直接返回0;
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

// 解析查询运动检测响应消息xml
IVS_INT32 CMotionDetecMgrXMLProcess::ParseMotionDetecXML(CXml& xml, IVS_MOTION_DETECTION* pMotionDetection)
{
	CHECK_POINTER(pMotionDetection, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	if (!xml.FindElemEx("Content/MotionDetectionInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Motion Detection XML", "Find Element Path(Content/MotionDetectionInfo) failed");
		return IVS_XML_INVALID;
	}
	
	const IVS_CHAR* szElemValue = NULL;
	IVS_CHAR szTemp[EABLE_LENGTH+1] = {0};
	GET_ELEM_VALUE_NUM_FOR_UINT("DetectInterval", szElemValue, pMotionDetection->uiDetectInterval, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("Sensitivity", szElemValue, pMotionDetection->uiSensitivity, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("AutoClearAlarmTime", szElemValue, pMotionDetection->uiAutoClearAlarmTime, xml);
	GET_ELEM_VALUE_CHAR("MotionDetectionEnable", szElemValue, szTemp, EABLE_LENGTH, xml);
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
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Motion Detection XML", "Find Element(AreaInfoList) failed");
		return IVS_XML_INVALID;
	}
	xml.IntoElem();
	IVS_INT32 iNum = 0;
	(void)CMotionDetecMgrXMLProcess::GetMotionDetecAreaInfoNum(xml, iNum);
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

	if (xml.FindElem("AreaInfo"))
	{
		for(IVS_UINT32 i = 0; i < (pMotionDetection->uiRectNum); i++)
		{
			if (!xml.FindElemValue("AreaInfo"))
			{
				break;
			}
			IVS_RECT_FLOAT& stRect = pMotionDetection->rectDetection[i];
			GET_ELEM_VALUE_NUM_FOR_FLOAT("X1", szElemValue, stRect.left, xml);
			GET_ELEM_VALUE_NUM_FOR_FLOAT("Y1", szElemValue, stRect.top, xml);
			GET_ELEM_VALUE_NUM_FOR_FLOAT("X2", szElemValue, stRect.right, xml);
			GET_ELEM_VALUE_NUM_FOR_FLOAT("Y2", szElemValue, stRect.bottom, xml);
			xml.OutOfElem();
			(void)xml.NextElem();
		}
	}
	return IVS_SUCCEED;
	
}

// 拼装修改运动检测请求xml
IVS_INT32 CMotionDetecMgrXMLProcess::SetMotionDetecGetXML(const IVS_CHAR* pLoginId, const IVS_CHAR* pCameraCode, const IVS_CHAR* pDomainCode, const IVS_MOTION_DETECTION* pMotionDetection, CXml& xml)
{
	CHECK_POINTER(pLoginId, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pMotionDetection, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);

	IVS_DEBUG_TRACE("");
	std::string strMotionDetectionInfo = "MotionDetectionInfo";
	// 拼装xml
	(void)xml.AddDeclaration("1.0", "UTF-8", "");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(pCameraCode);

	(void)xml.AddElem(strMotionDetectionInfo.c_str());
	(void)xml.IntoElem();
	(void)xml.AddChildElem("DetectInterval");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionDetection->uiDetectInterval)).c_str());
	(void)xml.AddElem("AutoClearAlarmTime");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionDetection->uiAutoClearAlarmTime)).c_str());
	(void)xml.AddElem("Sensitivity");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pMotionDetection->uiSensitivity)).c_str());
	(void)xml.AddElem("MotionDetectionEnable");
	if (pMotionDetection->bMotionDetectionEnable == ENABLEMOTIONDETECTION)
	{
		(void)xml.SetElemValue("ON");
	}
	else
	{
		(void)xml.SetElemValue("OFF");
	}

	(void)xml.AddElem("AreaInfoList");
	(void)xml.IntoElem();

	for (IVS_UINT32 i = 0; i < (pMotionDetection->uiRectNum); i++)
	{
		const IVS_RECT_FLOAT& stRect = pMotionDetection->rectDetection[i];
		i == 0?(void)xml.AddChildElem("AreaInfo"):(void)xml.AddElem("AreaInfo");
		(void)xml.IntoElem();
		(void)xml.AddChildElem("X1");
		(void)xml.IntoElem();
		IVS_CHAR cLeft[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cLeft, STR_LEN, "%f", stRect.left))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR ,"CMotionDetecMgrXMLProcess::SetMotionDetecGetXML", "Snprintf cLeft failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cLeft);

		(void)xml.AddElem("Y1");
		IVS_CHAR cTop[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cTop, STR_LEN, "%f", stRect.top))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR ,"Set Motion Detection Get XML", "Snprintf cTop failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cTop);

		(void)xml.AddElem("X2");
		IVS_CHAR cRight[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cRight, STR_LEN, "%f", stRect.right))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR ,"Set Motion Detection Get XML", "Snprintf cRight failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cRight);

		(void)xml.AddElem("Y2");
		IVS_CHAR cBottom[STR_LEN] = {0};
		if (!CToolsHelp::Snprintf(cBottom, STR_LEN, "%f", stRect.bottom))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR ,"Set Motion Detection Get XML", "Snprintf cBottom failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		(void)xml.SetElemValue(cBottom);
		xml.OutOfElem();
	}

	xml.OutOfElem();
	xml.OutOfElem();
	return IVS_SUCCEED;
}

//拼装查询移动侦测幅度数据的请求xml
IVS_INT32 CMotionDetecMgrXMLProcess::GetMotionRangeDataPlatformReqXML(const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode,const IVS_TIME_SPAN* pTimeSpan,
	const IVS_INDEX_RANGE* pIndexRange,CXml& xml)
{
	CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pIndexRange,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pTimeSpan,IVS_OPERATE_MEMORY_ERROR);

	IVS_DEBUG_TRACE("");
	std::string strQueryType = "EXACT";
	std::string strFromIndex = "FromIndex";
	std::string strToIndex = "ToIndex";

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");	
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(pDomainCode);
	(void)xml.AddElem("PageInfo");	
	(void)xml.AddChildElem(strFromIndex.c_str());
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pIndexRange->uiFromIndex)).c_str());
	(void)xml.AddElem(strToIndex.c_str());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(pIndexRange->uiToIndex)).c_str());
	
	(void)xml.AddElem("QueryCond");	
	(void)xml.AddChildElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("CameraCode");
	(void)xml.AddElem("Value");	
	(void)xml.SetElemValue(pCameraCode);
	(void)xml.AddElem("QueryType");	
	(void)xml.SetElemValue(strQueryType.c_str());
	xml.OutOfElem();
	(void)xml.AddElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("FromTime");
	(void)xml.AddElem("Value");	
	(void)xml.SetElemValue(pTimeSpan->cStart);
	(void)xml.AddElem("QueryType");	
	(void)xml.SetElemValue(strQueryType.c_str());
	xml.OutOfElem();

	(void)xml.AddElem("QueryField");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Field");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("ToTime");
	(void)xml.AddElem("Value");	
	(void)xml.SetElemValue(pTimeSpan->cEnd);
	(void)xml.AddElem("QueryType");	
	(void)xml.SetElemValue(strQueryType.c_str());
	xml.OutOfElem();

	xml.OutOfElem();

	return IVS_SUCCEED;
}

//解析查询移动侦测幅度数据的响应xml
IVS_INT32 CMotionDetecMgrXMLProcess::ParseGetMotionRangeDataPlatformRspXML(CXml &RspXml, IVS_MOTION_RANGE_DATA_LIST* pMotionRangeData,IVS_UINT32 uiBufferSize)
{
	IVS_DEBUG_TRACE("");

	const char* szElemValue = NULL;
	if(RspXml.FindElemEx("Content/PageInfo"))
	{
		(void)RspXml.IntoElem();
		GET_ELEM_VALUE_NUM_FOR_UINT("RealNum", szElemValue,pMotionRangeData->uiTotal , RspXml);
		GET_ELEM_VALUE_NUM_FOR_UINT("FromIndex", szElemValue,pMotionRangeData->stIndexRange.uiFromIndex, RspXml);
		GET_ELEM_VALUE_NUM_FOR_UINT("ToIndex", szElemValue, pMotionRangeData->stIndexRange.uiToIndex, RspXml);
		RspXml.OutOfElem();
	}    

	if(!RspXml.FindElemEx("Content/FromTime"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Get Motion Range Data Platform Rsp XML", "xml.FindElem(Content/FromTime) is fail");
		return IVS_XML_INVALID;
	}  
	GET_ELEM_VALUE_CHAR("FromTime",szElemValue,pMotionRangeData->stTime.cStart,IVS_TIME_LEN,RspXml);
	if(!RspXml.FindElemEx("Content/ToTime"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Get Motion Range Data Platform Rsp XML", "xml.FindElem(Content/ToTime) is fail");
		return IVS_XML_INVALID;
	}  
	GET_ELEM_VALUE_CHAR("ToTime",szElemValue,pMotionRangeData->stTime.cEnd,IVS_TIME_LEN,RspXml);
	if(!RspXml.FindElemEx("Content/TimeInterval"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Get Motion Range Data Platform Rsp XML", "xml.FindElem(Content/TimeInterval) is fail");
		return IVS_XML_INVALID;
	}  
	GET_ELEM_VALUE_NUM_FOR_UINT("TimeInterval", szElemValue, pMotionRangeData->uiTimeInterval,RspXml);

	IVS_INT32 iSize = 0;
	if(RspXml.FindElemEx("Content/MotionList/MotionData"))
	{
		do{
			IVS_MOTION_RANGE_DATA_INFO stMotionRangeDataInfo;
			memset(&stMotionRangeDataInfo, 0, sizeof(IVS_MOTION_RANGE_DATA_INFO));

			(void)RspXml.IntoElem();
		    GET_ELEM_VALUE_NUM_FOR_UINT64("Data", szElemValue, stMotionRangeDataInfo.uiMotionData, RspXml);
			GET_ELEM_VALUE_CHAR("StartTime",szElemValue,  stMotionRangeDataInfo.StartTime,IVS_TIME_LEN,RspXml);
			GET_ELEM_VALUE_CHAR("EndTime",szElemValue,  stMotionRangeDataInfo.EndTime,IVS_TIME_LEN,RspXml);
			RspXml.OutOfElem();
			
			bool bRet = CToolsHelp::Memcpy(&(pMotionRangeData->stMotionRangeDataInfo[iSize]), sizeof(IVS_MOTION_RANGE_DATA_INFO), &stMotionRangeDataInfo, sizeof(IVS_MOTION_RANGE_DATA_INFO));
			if(false == bRet)
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"Parse Result XML", "Memcpy error.");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			iSize++;
		}while (RspXml.NextElem());
	} 

	//添加尺寸校验

    IVS_UINT32 unItemSize =(IVS_UINT32)((iSize-1 > 0) ? (iSize-1) : 0);

	IVS_UINT32 uiSize = sizeof(IVS_MOTION_RANGE_DATA_LIST) +  unItemSize * sizeof(IVS_MOTION_RANGE_DATA_INFO);
	if (uiSize>uiBufferSize)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Get Motion Range Data Platform Rsp XML is failed","Response Result is larger than uiBufferSize");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return IVS_SUCCEED;
}

