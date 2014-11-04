#include "stdafx.h"
#include "OCXIAXMLProcess.h"
#include "IVS_Trace.h"
#include "ToolsHelp.h"


#define TEMP_BUF_MAX_LENGTH 800	// 临时缓存最大长度


#define CHECK_SUCCESS(s)\
	do { if(!(s)) return IVS_FAIL;} while(0)

// 分割字符串时判断指针是否为空
#define CHECK_STRTOK_POINTER(pPointer, PointerName) \
	if ( NULL == ( pPointer ) )						\
{													\
	BP_DBG_LOG("Point " #PointerName " is null.");	\
	return IVS_FAIL;								\
}

#define SET_ELEMENT_ULONG(_num, _buf, _xml)				\
	do {(void)_ultoa_s(_num, _buf, sizeof(_buf), 10);	\
		CHECK_SUCCESS(_xml.SetElemValue(_buf));			\
	} while(0)

#define SET_ELEMENT_STRING(_buf, _xml)\
	do { CHECK_SUCCESS(_xml.SetElemValue(_buf)); } while(0)


// 获取UINT64类型的字段
#define  GET_ELEMENT_UINT64(val, element, _xml, _bfind)	\
	do { const char *data = NULL;						\
		if (_xml.FindElem(element)						\
			&& (NULL != (data = _xml.GetElemValue())))	\
			{val = _strtoui64(data, NULL, 10);			\
			_bfind = true;}								\
	} while (0)

// 获取String类型的字段
#define GET_ELEMENT_STRING(_DestBuf, _MaxLenth, element, _xml, _bfind)				\
	do { const char *_data;															\
		if (_xml.FindElem(element)													\
			&& (NULL != (_data = _xml.GetElemValue())))								\
			{(void)CToolsHelp::Memcpy(_DestBuf, _MaxLenth, _data, strlen(_data));	\
			_bfind = true;}															\
	} while (0)

// 获取ULONG类型的字段
#define  GET_ELEMENT_ULONG(val, element, _xml, _bfind)	\
	do { const char *data = NULL;						\
		if (_xml.FindElem(element)						\
			&& (NULL != (data = _xml.GetElemValue())))	\
			{val = strtoul(data, NULL, 10);				\
			_bfind = true;}								\
	} while (0)




CIAXMLProcess::CIAXMLProcess()
{

}
	
CIAXMLProcess::~CIAXMLProcess()
{

}

/*********************** 请求轨迹报文样例 *********************** 
<MESSAGE>
	<CV_CONTENT>
		<REQ_INFO>
			<RequestID>9999</RequestID>
			<CameraID>918200000000101</CameraID>
			<RuleType>1</RuleType>			RuleType  是 规则类型，1是行为分析 3是人脸识别
			<QueryType>1</QueryType>		QueryType 是 请求类型，0表示实况，1表示录像
			<StartTime>20120524173445</StartTime>
			<StopTime>20120524173456</StopTime>
		</REQ_INFO>
	</CV_CONTENT>
</MESSAGE>
**************************************************************/

// 构建智能分析请求轨迹报文
IVS_INT32 CIAXMLProcess::GetAnalyzeResultBuildXML(const IA_REQ_PARAM &IAReqParam, CXml &ReqXML)
{
	char val[65] = {0};

	CHECK_SUCCESS(ReqXML.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(ReqXML.AddElem("MESSAGE"));
	CHECK_SUCCESS(ReqXML.AddChildElem("CV_CONTENT"));
	CHECK_SUCCESS(ReqXML.IntoElem());
	CHECK_SUCCESS(ReqXML.AddChildElem("REQ_INFO"));
	CHECK_SUCCESS(ReqXML.IntoElem());

	CHECK_SUCCESS(ReqXML.AddChildElem("RequestID"));
	CHECK_SUCCESS(ReqXML.IntoElem());
 	SET_ELEMENT_ULONG(IAReqParam.ulRequestID, val, ReqXML);

//     CHECK_SUCCESS(ReqXML.AddElem("CUDevCode"));
//     SET_ELEMENT_STRING(IAReqParam.CUCode, ReqXML);

	CHECK_SUCCESS(ReqXML.AddElem("CameraID"));
	SET_ELEMENT_STRING(IAReqParam.CameraID, ReqXML);

	CHECK_SUCCESS(ReqXML.AddElem("RuleType"));
	SET_ELEMENT_ULONG(IAReqParam.ulRuleType, val, ReqXML);

	CHECK_SUCCESS(ReqXML.AddElem("QueryType"));
	SET_ELEMENT_ULONG(IAReqParam.ulQueryType, val, ReqXML);

	CHECK_SUCCESS(ReqXML.AddElem("StartTime"));
	SET_ELEMENT_STRING(IAReqParam.StartTime, ReqXML);

	CHECK_SUCCESS(ReqXML.AddElem("StopTime"));
	SET_ELEMENT_STRING(IAReqParam.StopTime, ReqXML);

	ReqXML.OutOfElem();
	ReqXML.OutOfElem();
	ReqXML.OutOfElem();

	return IVS_SUCCEED;
}


/*********************** 请求轨迹报文样例 *********************** 
<MESSAGE><CV_CONTENT>
	<RESULT_CODE>1200</RESULT_CODE>
	<LastRecordStopTime>20120615091744</LastRecordStopTime>
</CV_CONTENT></MESSAGE>
**************************************************************/

// 解析智能分析请求轨迹报文
IVS_INT32 CIAXMLProcess::GetAnalyzeResultPraseXML(LPCTSTR pRspXML, IVS_ULONG &ulResultCode, IA_REQ_PARAM &IAReqParam)
{
	CHECK_POINTER(pRspXML, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("RspXML: %s",pRspXML);
	CXml xml;
	if (!xml.Parse((char *)pRspXML))
	{
		return IVS_FAIL;
	}
	bool bFind = false;
	CHECK_SUCCESS(xml.FindElem("MESSAGE"));
	CHECK_SUCCESS(xml.IntoElem());
	CHECK_SUCCESS(xml.FindElem("CV_CONTENT"));
	CHECK_SUCCESS(xml.IntoElem());
	
	GET_ELEMENT_ULONG(ulResultCode, "RESULT_CODE", xml, bFind);
	CHECK_SUCCESS(bFind);

	if (IA_QUERY_TYPE_RECORD == IAReqParam.ulQueryType)
	{
		bFind = false;
		GET_ELEMENT_STRING(IAReqParam.StartTime, IVS_TIME_LEN, "LastRecordStopTime", xml, bFind);
		CHECK_SUCCESS(bFind);
	}
	xml.OutOfElem();
	xml.OutOfElem();
	return IVS_SUCCEED;
}


/*********************** 轨迹推送报文样例 *********************** 
"<XMLLayoutMessage>\
<LayoutVersion>ver4.0</LayoutVersion>\
<RequestID>9999</RequestID>\
<DeviceID>918200000000101</DeviceID>\
<AlarmID>0</AlarmID>\
<FrameID>1339723003680</FrameID>\
<FrameSize>352,288</FrameSize>\
<LayoutElementList>\
<LayoutElement><Layer>ROI</Layer><Color>255,0,0</Color><ElementType>Polyline</ElementType><ThePoints><NumberOfPoints>11</NumberOfPoints><ElementPoints>125,204;127,204;130,216;132,220;132,220;133,222;134,222;138,222;141,222;144,222;147,223</ElementPoints><PercentPoint>0</PercentPoint></ThePoints></LayoutElement>\
<LayoutElement><Layer>Alarmed</Layer><Color>255,255,0</Color><ElementType>Polyline</ElementType><ThePoints><NumberOfPoints>5</NumberOfPoints><ElementPoints>118,194;132,194;132,212;118,212;118,194</ElementPoints></ThePoints></LayoutElement>\
<LayoutElement><Layer>Alarmed</Layer><Color>0,0,255</Color><ElementType>Text</ElementType><TheText><Scale>1</Scale><Text>AAAAA</Text><PointToDraw>118,194</PointToDraw></TheText></LayoutElement>\
</LayoutElementList>\
</XMLLayoutMessage> "
**************************************************************
<MESSAGE>
	<CV_CONTENT>
		<DeviceID>99000000000000000101#0123456789</DeviceID>
		<VideoType>0</VideoType>
		<RuleType>1</RuleType>
		<VideoTime>20130220123918.245</VideoTime>
		<FrameID>1361363958245</FrameID>
		<LayoutInfo>
			<![CDATA[<?xml version="1.0" encoding="UTF-8"?>
			<XMLLayoutMessage>
				<LayoutVersion>ver4.0</LayoutVersion>
				<DeviceID>99000000000000000101#0123456789</DeviceID>
				<AlarmID></AlarmID>
				<FrameID>2569488060</FrameID>
				<FrameSize>640,368</FrameSize>
				<LayoutElementList>
				<LayoutElement>
					<Layer>ROI</Layer><Color>255,0,255</Color><ElementType>Polyline</ElementType><ThePoints><NumberOfPoints>5</NumberOfPoints><ElementPoints>56,30;606,30;606,345;56,345;56,30</ElementPoints></ThePoints>
				</LayoutElement>
				</LayoutElementList>
			</XMLLayoutMessage>
			]]>
		</LayoutInfo>
		<RequestID>1</RequestID>
		<CUDevCodeList>
			<CUDevCode>1</CUDevCode>
		</CUDevCodeList>
		<AlarmID />
	</CV_CONTENT>
</MESSAGE>
**************************************************************/
// 解析智能分析轨迹推送报文
IVS_INT32 CIAXMLProcess::TrackInfoContentPraseXML(LPCTSTR pTrackXml, IA_LAYOUT_MESSAGE &pIALayoutMessage, IA_REQ_PARAM &IAReqParameters)
{
	CHECK_POINTER(pTrackXml, IVS_OPERATE_MEMORY_ERROR);

	IVS_DEBUG_TRACE("TrackXml: %s",pTrackXml);

	memset(&pIALayoutMessage, 0, sizeof(IA_LAYOUT_MESSAGE));
	CXml xmlMessage;
	if (!xmlMessage.Parse((char *)pTrackXml))
	{
		return IVS_FAIL;
	}

	CHECK_SUCCESS(xmlMessage.FindElem("MESSAGE"));
	CHECK_SUCCESS(xmlMessage.IntoElem());
	CHECK_SUCCESS(xmlMessage.FindElem("CV_CONTENT"));
	CHECK_SUCCESS(xmlMessage.IntoElem());

	// 解析DeviceID字段
	bool bFind = false;
	GET_ELEMENT_STRING(IAReqParameters.CameraID, IVS_DEV_CODE_LEN + 1, "DeviceID", xmlMessage, bFind);
	CHECK_SUCCESS(bFind);

	// 解析VideoType字段
	bFind = false;
	GET_ELEMENT_ULONG(IAReqParameters.ulQueryType, "VideoType", xmlMessage, bFind);
	CHECK_SUCCESS(bFind);

	// 解析RuleType字段
	bFind = false;
	GET_ELEMENT_ULONG(IAReqParameters.ulRuleType, "RuleType", xmlMessage, bFind);
	CHECK_SUCCESS(bFind);

	// 解析FrameID字段
	bFind = false;
	GET_ELEMENT_UINT64(pIALayoutMessage.ulFrameID, "FrameID", xmlMessage, bFind);
	CHECK_SUCCESS(bFind);

	const char *pLayoutInfo;	
	CHECK_SUCCESS(xmlMessage.FindElem("LayoutInfo"));
	CHECK_SUCCESS(NULL != (pLayoutInfo = xmlMessage.GetElemValue()));			

	CXml xml;
	if (!xml.Parse((char *)pLayoutInfo))
	{
		return IVS_FAIL;
	}


	CHECK_SUCCESS(xml.FindElem("XMLLayoutMessage"));
	CHECK_SUCCESS(xml.IntoElem());

	char tempBuf[TEMP_BUF_MAX_LENGTH] = {0};


	// 解析FrameSize字段
	bFind = false;
	GET_ELEMENT_STRING(tempBuf, TEMP_BUF_MAX_LENGTH, "FrameSize", xml, bFind);
	CHECK_SUCCESS(bFind);
	CHECK_SUCCESS(IVS_SUCCEED == PraseFrameSize(tempBuf, pIALayoutMessage));


	CHECK_SUCCESS(xml.FindElem("LayoutElementList"));
	CHECK_SUCCESS(xml.IntoElem());

	ULONG ulElementNum = 0;
	do
	{
		if (!ulElementNum)
		{
			CHECK_SUCCESS(xml.FindElem("LayoutElement"));
		}
		CHECK_SUCCESS(xml.IntoElem());

		// 解析Layer字段
		bFind = false;
		memset(tempBuf, 0, TEMP_BUF_MAX_LENGTH);
		GET_ELEMENT_STRING(tempBuf, TEMP_BUF_MAX_LENGTH, "Layer", xml, bFind);
		CHECK_SUCCESS(bFind);
		if (!strcmp("Alarmed", tempBuf))
		{
			pIALayoutMessage.LayoutElement[ulElementNum].ulLayer = IA_LAYER_TYPE_ALARM;
		}
		if (!strcmp("ROI", tempBuf))
		{
			pIALayoutMessage.LayoutElement[ulElementNum].ulLayer = IA_LAYER_TYPE_ROI;
		}

		// 解析Color字段
		bFind = false;
		memset(tempBuf, 0, TEMP_BUF_MAX_LENGTH);
		GET_ELEMENT_STRING(tempBuf, TEMP_BUF_MAX_LENGTH, "Color", xml, bFind);
		CHECK_SUCCESS(bFind);
		CHECK_SUCCESS(IVS_SUCCEED == PraseColor(tempBuf, pIALayoutMessage.LayoutElement[ulElementNum].ulColor));

		// 解析ElementType字段
		bFind = false;
		memset(tempBuf, 0, TEMP_BUF_MAX_LENGTH);
		GET_ELEMENT_STRING(tempBuf, TEMP_BUF_MAX_LENGTH, "ElementType", xml, bFind);
		CHECK_SUCCESS(bFind);
		if (!strcmp("Text", tempBuf))
		{
			pIALayoutMessage.LayoutElement[ulElementNum].ulElementType = IA_ELEMENT_TYPE_TEXT;

			// 类型为Text时，解析TheText字段
			CHECK_SUCCESS(xml.FindElem("TheText"));
			CHECK_SUCCESS(xml.IntoElem());

			bFind = false;
			char *text = pIALayoutMessage.LayoutElement[ulElementNum].AlarmText.TextContent;
			GET_ELEMENT_STRING(text, IA_TEXT_LENGTH_MAX, "Text", xml, bFind);
			CHECK_SUCCESS(bFind);
			pIALayoutMessage.LayoutElement[ulElementNum].AlarmText.ulTextLenth = strlen(text);

			bFind = false;
			GET_ELEMENT_ULONG(pIALayoutMessage.LayoutElement[ulElementNum].AlarmText.ulTextScale, "Scale", xml, bFind);
			CHECK_SUCCESS(bFind);

			bFind = false;
			GET_ELEMENT_STRING(tempBuf, TEMP_BUF_MAX_LENGTH, "PointToDraw", xml, bFind);
			CHECK_SUCCESS(bFind);
			CHECK_SUCCESS(IVS_SUCCEED == PrasePoint(tempBuf, pIALayoutMessage.LayoutElement[ulElementNum].AlarmText.TextPoint));

			GET_ELEMENT_ULONG(pIALayoutMessage.LayoutElement[ulElementNum].AlarmText.ulIsPercent, "PercentPoint", xml, bFind);
			BP_RUN_LOG_INF("Prase Track", "Is Percent: %lu", pIALayoutMessage.LayoutElement[ulElementNum].AlarmPolyline.ulIsPercent);

			xml.OutOfElem();
		}
		if (!strcmp("Polyline", tempBuf))
		{
			pIALayoutMessage.LayoutElement[ulElementNum].ulElementType = IA_ELEMENT_TYPE_POLYLINE;

			// 类型为Polyline时，解析ThePoints字段
			CHECK_SUCCESS(xml.FindElem("ThePoints"));
			CHECK_SUCCESS(xml.IntoElem());

			bFind = false;
			ULONG ulPointsNum = 0;
			GET_ELEMENT_ULONG(ulPointsNum, "NumberOfPoints", xml, bFind);
			CHECK_SUCCESS(bFind);
			pIALayoutMessage.LayoutElement[ulElementNum].AlarmPolyline.ulPointsNum = ulPointsNum;
	
			GET_ELEMENT_ULONG(pIALayoutMessage.LayoutElement[ulElementNum].AlarmPolyline.ulIsPercent, "PercentPoint", xml, bFind);
			BP_RUN_LOG_INF("Prase Track", "Is Percent: %lu", pIALayoutMessage.LayoutElement[ulElementNum].AlarmPolyline.ulIsPercent);

			bFind = false;
			ULONG ulListPointsNum = 0;
			memset(tempBuf, 0, TEMP_BUF_MAX_LENGTH);
			GET_ELEMENT_STRING(tempBuf, TEMP_BUF_MAX_LENGTH, "ElementPoints", xml, bFind);
			CHECK_SUCCESS(bFind);
			CHECK_SUCCESS(IVS_SUCCEED == PrasePointList(tempBuf, pIALayoutMessage.LayoutElement[ulElementNum].AlarmPolyline.Points, ulListPointsNum));

            // add by wanglei 00165153:2013.11.6 拌线需要增加方向
            GET_ELEMENT_ULONG(pIALayoutMessage.LayoutElement[ulElementNum].AlarmPolyline.ulDirection, "TripWireDirection", xml, bFind);
            // end add

			xml.OutOfElem();

			// 判断解析点列表获得的点数 与 传入的点数是否一致
			CHECK_SUCCESS(ulPointsNum == ulListPointsNum);


		}

		ulElementNum++;
		xml.OutOfElem();//LayoutElement

	}while (xml.NextElem());
	pIALayoutMessage.ulLayoutElementNum = ulElementNum;

	xml.OutOfElem();//LayoutElementList
	xml.OutOfElem();//XMLLayoutMessage

	if (IA_QUERY_TYPE_RECORD == IAReqParameters.ulQueryType)
	{
		// 解析RequestID字段
		bFind = false;
		GET_ELEMENT_ULONG(IAReqParameters.ulRequestID, "RequestID", xmlMessage, bFind);
		CHECK_SUCCESS(bFind);
	}


	xmlMessage.OutOfElem();//CV_CONTENT
	xmlMessage.OutOfElem();//MESSAGE

	return IVS_SUCCEED;
}

// 解析分辨率长宽字段
IVS_INT32 CIAXMLProcess::PraseFrameSize(char *tempBuf, IA_LAYOUT_MESSAGE &pIALayoutMessage) 
{
	CHECK_POINTER(tempBuf, IVS_FAIL);

	char *outer_ptr = NULL;  
	char *token = NULL;

	CHECK_STRTOK_POINTER(token = strtok_s(tempBuf, ",", &outer_ptr), ulFrameWidth);
	pIALayoutMessage.ulFrameWidth = strtoul(token, NULL, 10);
	CHECK_STRTOK_POINTER(outer_ptr, ulFrameHeight);
	pIALayoutMessage.ulFrameHeight = strtoul(outer_ptr, NULL, 10);
	return IVS_SUCCEED;
}

// 解析颜色字段
IVS_INT32 CIAXMLProcess::PraseColor(char *tempBuf, COLORREF &ulColor)
{
	CHECK_POINTER(tempBuf, IVS_FAIL);

	char *token = NULL;
	char *outer_ptr = NULL;

	CHECK_STRTOK_POINTER(token = strtok_s(tempBuf, ",", &outer_ptr), Red);
	unsigned char cRed = (unsigned char)atoi(token);
	CHECK_STRTOK_POINTER(token = strtok_s(NULL, ",", &outer_ptr), Green);
	unsigned char cGreen = (unsigned char)atoi(token);
	CHECK_STRTOK_POINTER(outer_ptr, Blue);
	unsigned char cBlue = (unsigned char)atoi(outer_ptr);
	ulColor = RGB(cRed, cGreen, cBlue);

	return IVS_SUCCEED;
}

// 解析点坐标字段
IVS_INT32 CIAXMLProcess::PrasePoint(char *tempBuf, POINT &point) 
{
	CHECK_POINTER(tempBuf, IVS_FAIL);

	char *token = NULL;
	char *outer_ptr = NULL;

	CHECK_STRTOK_POINTER(token = strtok_s(tempBuf, ",", &outer_ptr), point.x);
	point.x = atol(token);
	CHECK_STRTOK_POINTER(outer_ptr, point.y);
	point.y = atol(outer_ptr);
	return IVS_SUCCEED;
}

// 解析点列表字段
IVS_INT32 CIAXMLProcess::PrasePointList(char *tempBuf, POINT *PointList, ULONG &ulListPointsNum) 
{
	CHECK_POINTER(tempBuf, IVS_FAIL);
	CHECK_POINTER(PointList, IVS_FAIL);

	char *outer_ptr = NULL;
	char *PointToken = NULL;
	char *buf = tempBuf;
	ULONG   tot = 0;

	while((PointToken = strtok_s(buf, ";", &outer_ptr)) != NULL) 
	{  
		buf = PointToken;  
		CHECK_SUCCESS(IVS_SUCCEED == CIAXMLProcess::PrasePoint(buf, PointList[tot]));
		tot++;
		buf = NULL;   
	}  

	ulListPointsNum = tot;

	return IVS_SUCCEED;
}

// 构建停止智能分析请求轨迹报文
IVS_INT32 CIAXMLProcess::StopAnalyzeResultBuildXML(IVS_ULONG ulRequestID, IVS_ULONG ulRuleType, IVS_ULONG ulQueryType, const IVS_CHAR *CameraID, CXml &ReqXML)
{
	CHECK_POINTER(CameraID, IVS_FAIL);
	char val[65] = {0};

	CHECK_SUCCESS(ReqXML.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(ReqXML.AddElem("MESSAGE"));
	CHECK_SUCCESS(ReqXML.AddChildElem("CV_CONTENT"));
	CHECK_SUCCESS(ReqXML.IntoElem());
	CHECK_SUCCESS(ReqXML.AddChildElem("REQ_INFO"));
	CHECK_SUCCESS(ReqXML.IntoElem());

	CHECK_SUCCESS(ReqXML.AddChildElem("CameraID"));
	CHECK_SUCCESS(ReqXML.IntoElem());
	SET_ELEMENT_STRING(CameraID, ReqXML);
	
	CHECK_SUCCESS(ReqXML.AddElem("RuleType"));
	SET_ELEMENT_ULONG(ulRuleType, val, ReqXML);

	CHECK_SUCCESS(ReqXML.AddElem("VideoType"));
	SET_ELEMENT_ULONG(ulQueryType, val, ReqXML);
	
	if (ulRequestID)
	{
		CHECK_SUCCESS(ReqXML.AddElem("RequestID"));
		SET_ELEMENT_ULONG(ulRequestID, val, ReqXML);
	}

	ReqXML.OutOfElem();
	ReqXML.OutOfElem();
	ReqXML.OutOfElem();

	return IVS_SUCCEED;

}

// 解析停止智能分析请求轨迹报文
IVS_INT32 CIAXMLProcess::StopAnalyzeResultPraseXML(LPCTSTR pRspXML, IVS_ULONG &ulResultCode)
{
	CHECK_POINTER(pRspXML, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("RspXML: %s",pRspXML);
	CXml xml;
	if (!xml.Parse((char *)pRspXML))
	{
		return IVS_FAIL;
	}
	CHECK_SUCCESS(xml.FindElem("MESSAGE"));
	CHECK_SUCCESS(xml.IntoElem());
	CHECK_SUCCESS(xml.FindElem("CV_CONTENT"));
	CHECK_SUCCESS(xml.IntoElem());

	bool bFind = false;
	GET_ELEMENT_ULONG(ulResultCode, "RESULT_CODE", xml, bFind);
	CHECK_SUCCESS(bFind);

	xml.OutOfElem();
	xml.OutOfElem();
	return IVS_SUCCEED;
}
