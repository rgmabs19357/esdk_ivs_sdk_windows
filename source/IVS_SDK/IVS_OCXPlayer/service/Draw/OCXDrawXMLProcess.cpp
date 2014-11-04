#include "stdafx.h"
#include "ToolsHelp.h"
#include "OCXDrawXMLProcess.h"
#include "IVS_Trace.h"



#define CHECK_SUCCESS(s)\
	do { if(!(s)) return IVS_FAIL;} while(0)

#define SET_ELEMENT_LONG(_num, _buf, _xml)		\
	do {(void)_itoa_s((int)_num, _buf, 10);		\
		CHECK_SUCCESS(_xml.SetElemValue(_buf)); \
	} while(0)

#define SET_ELEMENT_STRING(_buf, _xml)\
	do { CHECK_SUCCESS(_xml.SetElemValue(_buf)); } while(0)


#define  GET_ELEMENT_ULONG(val, element, _xml)			\
	do {const char *data = NULL;						\
		if (_xml.FindElem(#element)						\
			&& (NULL != (data = _xml.GetElemValue())))	\
			val = (ULONG)strtol(data, NULL, 10);		\
	} while (0)

#define  GET_ELEMENT_BOOL(val, element, _xml)		\
	do {const char *data = NULL;					\
	if (_xml.FindElem(#element)						\
		&& (NULL != (data = _xml.GetElemValue())))	\
		val = (BOOL)strtol(data, NULL, 10);			\
	} while (0)


#define  GET_ELEMENT_UCHAR(val, element, _xml)		\
	do { const char *data = NULL;					\
		if (_xml.FindElem(#element)					\
		&& (NULL != (data = _xml.GetElemValue())))	\
		val = (UCHAR)strtol(data, NULL, 10);		\
	} while (0)

#define GET_ELEMENT_STRING(_DestBuf, _MaxLenth, element, _xml)				\
	do { const char *data;													\
		if (_xml.FindElem(#element)											\
		&& (NULL != (data = _xml.GetElemValue())))							\
		(void)CToolsHelp::Memcpy(_DestBuf, (_MaxLenth), data, strlen(data));\
	} while (0)




COCXDrawXMLProcess::COCXDrawXMLProcess()
{
}

COCXDrawXMLProcess::~COCXDrawXMLProcess()
{
}


// 解析设置图形的XML
IVS_INT32 COCXDrawXMLProcess::SetGraphicsPraseXML(IA_GRAPHICS *pGraphicsArr, ULONG &ulNum, LPCTSTR pGraphicsXml, const SIZE &szGraphic, BOOL bPersent)
{
	CHECK_POINTER(pGraphicsArr, IVS_FAIL);
	CHECK_POINTER(pGraphicsXml, IVS_OPERATE_MEMORY_ERROR);

	IVS_DEBUG_TRACE("GraphicsXml: %s, GraphicWidth: %d, GraphicHeight: %d",pGraphicsXml, szGraphic.cx, szGraphic.cy);

	CXml xml;
	if (!xml.Parse(pGraphicsXml))
	{
		return IVS_FAIL;
	}

	CHECK_SUCCESS(xml.FindElem("Content"));
	CHECK_SUCCESS(xml.IntoElem());
	CHECK_SUCCESS(xml.FindElem("Graphics"));
	CHECK_SUCCESS(xml.IntoElem());

	const char *value = NULL;

	CHECK_SUCCESS(xml.FindElem("GraphicNum") && (NULL != (value = xml.GetElemValue())));
	ulNum = (ULONG)atoi(value);

	// 若设置的图形数大于图形数组长度，截断到最大画图个数
	if (ulNum > GRAPHICS_NUM_MAX)
	{
		BP_RUN_LOG_INF("COCXDrawXMLProcess::SetGraphicsPraseXML", "GraphicNum Truncated to GRAPHICS_NUM_MAX");
		ulNum = GRAPHICS_NUM_MAX;
	}

	CHECK_SUCCESS(xml.FindElem("GraphicList"));
	CHECK_SUCCESS(xml.IntoElem());

	for (ULONG i = 0; i < ulNum; i++)
	{
		// 给图形可选字段赋默认值
		memset(&pGraphicsArr[i], 0, sizeof(IA_GRAPHICS));
		pGraphicsArr[i].ulLineWidth = 3; // 默认基本线宽为3
		pGraphicsArr[i].bAllowDelete = TRUE; // 允许删除
		pGraphicsArr[i].bAllowModifySize = TRUE; // 允许修改矩形大小

		if (0 == i)
		{
			CHECK_SUCCESS(xml.FindElem("GraphicInfo"));
		} else {
			CHECK_SUCCESS(xml.NextElem());
		}
		CHECK_SUCCESS(xml.IntoElem());
		CHECK_SUCCESS(xml.FindElem("GraphicType") && (NULL != (value = xml.GetElemValue())));
		pGraphicsArr[i].ulGraphicType = (ULONG)atoi(value);

		GET_ELEMENT_ULONG(pGraphicsArr[i].ulGraphicGroup, GraphicGroup, xml);
		GET_ELEMENT_STRING(pGraphicsArr[i].GraphicGroupName, CHARACTER_NUM_MAX, GraphicGroupName, xml);

		if (xml.FindElem("GraphicBackgroundColor"))
		{
			CHECK_SUCCESS(xml.IntoElem());
			GET_ELEMENT_UCHAR(pGraphicsArr[i].BgColor.Alpha, Opacity, xml);
			GET_ELEMENT_UCHAR(pGraphicsArr[i].BgColor.Red, Red, xml);
			GET_ELEMENT_UCHAR(pGraphicsArr[i].BgColor.Green, Green, xml);
			GET_ELEMENT_UCHAR(pGraphicsArr[i].BgColor.Blue, Blue, xml);
			xml.OutOfElem();
		}

		if (xml.FindElem("GraphicLineColor"))
		{
			CHECK_SUCCESS(xml.IntoElem());
			GET_ELEMENT_UCHAR(pGraphicsArr[i].LineColor.Alpha, Opacity, xml);
			GET_ELEMENT_UCHAR(pGraphicsArr[i].LineColor.Red, Red, xml);
			GET_ELEMENT_UCHAR(pGraphicsArr[i].LineColor.Green, Green, xml);
			GET_ELEMENT_UCHAR(pGraphicsArr[i].LineColor.Blue, Blue, xml);
			xml.OutOfElem();
		}

		GET_ELEMENT_ULONG(pGraphicsArr[i].ulLineWidth, GraphicLineWidth, xml);

		CHECK_SUCCESS(xml.FindElem("PointNum") && (NULL != (value = xml.GetElemValue())));
		pGraphicsArr[i].ulPointsNum = (ULONG)atoi(value);

		CHECK_SUCCESS(xml.FindElem("PointList"));
		CHECK_SUCCESS(xml.IntoElem());

		for (ULONG j = 0; j < pGraphicsArr[i].ulPointsNum; j++)
		{
			if (0 == j)
			{
				CHECK_SUCCESS(xml.FindElem("Point"));
			} else {
				CHECK_SUCCESS(xml.NextElem());
			}
			CHECK_SUCCESS(xml.IntoElem());

			if ((0 == szGraphic.cx) || (0 == szGraphic.cy))
			{
				if (bPersent)
				{
					CHECK_SUCCESS(xml.FindElem("X") && (NULL != (value = xml.GetElemValue())));
					pGraphicsArr[i].IAPoints[j].x = (LONG)(atof(value) * FLOAT_TO_LONG_COEFF);
					CHECK_SUCCESS(xml.FindElem("Y") && (NULL != (value = xml.GetElemValue())));
					pGraphicsArr[i].IAPoints[j].y = (LONG)(atof(value) * FLOAT_TO_LONG_COEFF);
				} else {
					CHECK_SUCCESS(xml.FindElem("X") && (NULL != (value = xml.GetElemValue())));
					pGraphicsArr[i].IAPoints[j].x = (LONG)atoi(value);
					CHECK_SUCCESS(xml.FindElem("Y") && (NULL != (value = xml.GetElemValue())));
					pGraphicsArr[i].IAPoints[j].y = (LONG)atoi(value);
				}
			}
			else
			{
				CHECK_SUCCESS(xml.FindElem("X") && (NULL != (value = xml.GetElemValue())));
				pGraphicsArr[i].IAPoints[j].x = (LONG)(atof(value) * szGraphic.cx + 0.5);
				CHECK_SUCCESS(xml.FindElem("Y") && (NULL != (value = xml.GetElemValue())));
				pGraphicsArr[i].IAPoints[j].y = (LONG)(atof(value) * szGraphic.cy + 0.5);
			}

			xml.OutOfElem();
		}
		xml.OutOfElem();

		GET_ELEMENT_ULONG(pGraphicsArr[i].NamePosition, NamePosition, xml);
		GET_ELEMENT_BOOL(pGraphicsArr[i].bAllowDelete, DeleteByMouse, xml);
		GET_ELEMENT_BOOL(pGraphicsArr[i].bAllowAddNewPoint, AddNewPoint, xml);
		GET_ELEMENT_BOOL(pGraphicsArr[i].bAllowModifySize, AllowModifySize, xml);
		GET_ELEMENT_ULONG(pGraphicsArr[i].ulArrowType, ArrowType, xml);
		GET_ELEMENT_BOOL(pGraphicsArr[i].bKeepHorizon, KeepHorizon, xml);

		xml.OutOfElem();
	}
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 构建获取图形的XML
IVS_INT32 COCXDrawXMLProcess::GetGraphicsGetXML(const IA_GRAPHICS *pGraphicsArr, IVS_INT32 iResultCode, ULONG ulNum, CXml& xmlGraphics, const SIZE &szGraphic)
{
	CHECK_POINTER(pGraphicsArr, IVS_FAIL);
	IVS_DEBUG_TRACE("iResultCode: %d, ulNum：%d, GraphicWidth: %d, GraphicHeight: %d", iResultCode, ulNum, szGraphic.cx, szGraphic.cy);

	char val[64] = {0};

	CHECK_SUCCESS(xmlGraphics.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(xmlGraphics.AddElem("Content"));
	CHECK_SUCCESS(xmlGraphics.AddChildElem("ResultCode"));
	CHECK_SUCCESS(xmlGraphics.IntoElem());
	SET_ELEMENT_LONG(iResultCode, val, xmlGraphics);

	CHECK_SUCCESS(xmlGraphics.AddElem("Graphics"));
	CHECK_SUCCESS(xmlGraphics.AddChildElem("GraphicNum"));
	CHECK_SUCCESS(xmlGraphics.IntoElem());
	SET_ELEMENT_LONG(ulNum, val, xmlGraphics);
	if (0 == ulNum)
	{
		xmlGraphics.OutOfElem();
		xmlGraphics.OutOfElem();
		return IVS_SUCCEED;
	}

	CHECK_SUCCESS(xmlGraphics.AddElem("GraphicList"));
	for (ULONG i = 0; i < ulNum; i++)
	{
		if (i == 0)
		{
			CHECK_SUCCESS(xmlGraphics.AddChildElem("GraphicInfo"));
			CHECK_SUCCESS(xmlGraphics.IntoElem());
		}
		else
		{
			CHECK_SUCCESS(xmlGraphics.AddElem("GraphicInfo"));
		}

		CHECK_SUCCESS(xmlGraphics.AddChildElem("GraphicType"));
		CHECK_SUCCESS(xmlGraphics.IntoElem());
		SET_ELEMENT_LONG(pGraphicsArr[i].ulGraphicType, val, xmlGraphics);

		CHECK_SUCCESS(xmlGraphics.AddElem("GraphicGroup"));
		SET_ELEMENT_LONG(pGraphicsArr[i].ulGraphicGroup, val, xmlGraphics);

		CHECK_SUCCESS(xmlGraphics.AddElem("GraphicGroupName"));
		SET_ELEMENT_STRING(pGraphicsArr[i].GraphicGroupName, xmlGraphics);

		CHECK_SUCCESS(xmlGraphics.AddElem("GraphicBackgroundColor"));
		CHECK_SUCCESS(xmlGraphics.AddChildElem("Opacity"));
		CHECK_SUCCESS(xmlGraphics.IntoElem());
		SET_ELEMENT_LONG(pGraphicsArr[i].BgColor.Alpha, val, xmlGraphics);
		CHECK_SUCCESS(xmlGraphics.AddElem("Red"));
		SET_ELEMENT_LONG(pGraphicsArr[i].BgColor.Red, val, xmlGraphics);
		CHECK_SUCCESS(xmlGraphics.AddElem("Green"));
		SET_ELEMENT_LONG(pGraphicsArr[i].BgColor.Green, val, xmlGraphics);
		CHECK_SUCCESS(xmlGraphics.AddElem("Blue"));
		SET_ELEMENT_LONG(pGraphicsArr[i].BgColor.Blue, val, xmlGraphics);
		xmlGraphics.OutOfElem();

		CHECK_SUCCESS(xmlGraphics.AddElem("GraphicLineColor"));
		CHECK_SUCCESS(xmlGraphics.AddChildElem("Opacity"));
		CHECK_SUCCESS(xmlGraphics.IntoElem());
		SET_ELEMENT_LONG(pGraphicsArr[i].LineColor.Alpha, val, xmlGraphics);
		CHECK_SUCCESS(xmlGraphics.AddElem("Red"));
		SET_ELEMENT_LONG(pGraphicsArr[i].LineColor.Red, val, xmlGraphics);
		CHECK_SUCCESS(xmlGraphics.AddElem("Green"));
		SET_ELEMENT_LONG(pGraphicsArr[i].LineColor.Green, val, xmlGraphics);
		CHECK_SUCCESS(xmlGraphics.AddElem("Blue"));
		SET_ELEMENT_LONG(pGraphicsArr[i].LineColor.Blue, val, xmlGraphics);
		xmlGraphics.OutOfElem();

		CHECK_SUCCESS(xmlGraphics.AddElem("GraphicLineWidth"));
		SET_ELEMENT_LONG(pGraphicsArr[i].ulLineWidth, val, xmlGraphics);

		CHECK_SUCCESS(xmlGraphics.AddElem("PointNum"));
		SET_ELEMENT_LONG(pGraphicsArr[i].ulPointsNum, val, xmlGraphics);

		CHECK_SUCCESS(xmlGraphics.AddElem("PointList"));
		for (ULONG j = 0; j < pGraphicsArr[i].ulPointsNum; j++)
		{
			if (0 == j)
			{
				CHECK_SUCCESS(xmlGraphics.AddChildElem("Point"));
				CHECK_SUCCESS(xmlGraphics.IntoElem());
			}
			else
			{
				CHECK_SUCCESS(xmlGraphics.AddElem("Point"));
			}

			CHECK_SUCCESS(xmlGraphics.AddChildElem("X"));
			CHECK_SUCCESS(xmlGraphics.IntoElem());
			if ((0 == szGraphic.cx) || (0 == szGraphic.cy))
			{			
				SET_ELEMENT_LONG(pGraphicsArr[i].IAPoints[j].x, val, xmlGraphics);
				CHECK_SUCCESS(xmlGraphics.AddElem("Y"));
				SET_ELEMENT_LONG(pGraphicsArr[i].IAPoints[j].y, val, xmlGraphics);
			}
			else
			{
				char cPointValue[10];
				memset(cPointValue, 0, sizeof(cPointValue));
				double dPointValue = (double)pGraphicsArr[i].IAPoints[j].x / szGraphic.cx;

				(void)_snprintf_s(cPointValue, sizeof(cPointValue) - 1, "%f", dPointValue);
                cPointValue[8] = '\0';
				CHECK_SUCCESS(xmlGraphics.SetElemValue(cPointValue)); 

				CHECK_SUCCESS(xmlGraphics.AddElem("Y"));
				memset(cPointValue, 0, sizeof(cPointValue));
				dPointValue = (double)pGraphicsArr[i].IAPoints[j].y / szGraphic.cy;
				(void)_snprintf_s(cPointValue, sizeof(cPointValue) - 1, "%f", dPointValue);
                cPointValue[8] = '\0';
                CHECK_SUCCESS(xmlGraphics.SetElemValue(cPointValue)); 

			}
			xmlGraphics.OutOfElem();
		}
		xmlGraphics.OutOfElem();

		CHECK_SUCCESS(xmlGraphics.AddElem("NamePosition"));
		SET_ELEMENT_LONG(pGraphicsArr[i].NamePosition, val, xmlGraphics);

		CHECK_SUCCESS(xmlGraphics.AddElem("DeleteByMouse"));
		SET_ELEMENT_LONG(pGraphicsArr[i].bAllowDelete, val, xmlGraphics);

		CHECK_SUCCESS(xmlGraphics.AddElem("AddNewPoint"));
		SET_ELEMENT_LONG(pGraphicsArr[i].bAllowAddNewPoint, val, xmlGraphics);

		CHECK_SUCCESS(xmlGraphics.AddElem("AllowModifySize"));
		SET_ELEMENT_LONG(pGraphicsArr[i].bAllowModifySize, val, xmlGraphics);
	
		CHECK_SUCCESS(xmlGraphics.AddElem("ArrowType"));
		SET_ELEMENT_LONG(pGraphicsArr[i].ulArrowType, val, xmlGraphics);

		CHECK_SUCCESS(xmlGraphics.AddElem("KeepHorizon"));
		SET_ELEMENT_LONG(pGraphicsArr[i].bKeepHorizon, val, xmlGraphics);

		xmlGraphics.OutOfElem();
	}
	xmlGraphics.OutOfElem();

	xmlGraphics.OutOfElem();
	return IVS_SUCCEED;

}

// 构建获取图形失败的XML
IVS_INT32 COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_INT32 iResultCode, CXml& xmlGraphics)
{
	char val[64] = {0};
	IVS_DEBUG_TRACE("iResultCode: %d", iResultCode);

	CHECK_SUCCESS(xmlGraphics.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(xmlGraphics.AddElem("Content"));
	CHECK_SUCCESS(xmlGraphics.AddChildElem("ResultCode"));
	CHECK_SUCCESS(xmlGraphics.IntoElem());
	SET_ELEMENT_LONG(iResultCode, val, xmlGraphics);
	xmlGraphics.OutOfElem();

	return IVS_SUCCEED;

}

// 解析画图前图形属性设置XML
IVS_INT32 COCXDrawXMLProcess::DrawGraphicsPraseXML(IA_GRAPHICS &GraphicAttribute, ULONG &ulGroupDrawMaxNum, LPCTSTR pGraphicsXml)
{
	CHECK_POINTER(pGraphicsXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("GroupDrawMaxNum: %lu, GraphicsXml: %s", ulGroupDrawMaxNum, pGraphicsXml);

	CXml xml;
	if (!xml.Parse(pGraphicsXml))
	{
		return IVS_FAIL;
	}

	CHECK_SUCCESS(xml.FindElem("Content"));
	CHECK_SUCCESS(xml.IntoElem());
	CHECK_SUCCESS(xml.FindElem("Graphics"));
	CHECK_SUCCESS(xml.IntoElem());

	const char *value = NULL;

	CHECK_SUCCESS(xml.FindElem("GraphicNum") && (NULL != (value = xml.GetElemValue())));
	ulGroupDrawMaxNum = (ULONG)atoi(value);

	// 若设置的图形数大于图形数组长度，截断到最大画图个数
	if (ulGroupDrawMaxNum > GRAPHICS_NUM_MAX)
	{
		BP_RUN_LOG_INF("COCXDrawXMLProcess::DrawGraphicsPraseXML", "GroupDrawMaxNum Truncated to GRAPHICS_NUM_MAX");
		ulGroupDrawMaxNum = GRAPHICS_NUM_MAX;
	}

	CHECK_SUCCESS(xml.FindElem("GraphicList"));
	CHECK_SUCCESS(xml.IntoElem());

	// 给图形可选字段赋默认值
	memset(&GraphicAttribute, 0, sizeof(IA_GRAPHICS));
	GraphicAttribute.ulLineWidth = 3; // 默认基本线宽为3
	GraphicAttribute.bAllowDelete = TRUE; // 允许删除
	GraphicAttribute.bAllowModifySize = TRUE; // 允许修改矩形大小

	CHECK_SUCCESS(xml.FindElem("GraphicInfo"));
	CHECK_SUCCESS(xml.IntoElem());
	CHECK_SUCCESS(xml.FindElem("GraphicType") && (NULL != (value = xml.GetElemValue())));
	GraphicAttribute.ulGraphicType = (ULONG)atoi(value);

	GET_ELEMENT_ULONG(GraphicAttribute.ulGraphicGroup, GraphicGroup, xml);
	GET_ELEMENT_STRING(GraphicAttribute.GraphicGroupName, CHARACTER_NUM_MAX, GraphicGroupName, xml);

	if (xml.FindElem("GraphicBackgroundColor"))
	{
		CHECK_SUCCESS(xml.IntoElem());
		GET_ELEMENT_UCHAR(GraphicAttribute.BgColor.Alpha, Opacity, xml);
		GET_ELEMENT_UCHAR(GraphicAttribute.BgColor.Red, Red, xml);
		GET_ELEMENT_UCHAR(GraphicAttribute.BgColor.Green, Green, xml);
		GET_ELEMENT_UCHAR(GraphicAttribute.BgColor.Blue, Blue, xml);
		xml.OutOfElem();
	}

	if (xml.FindElem("GraphicLineColor"))
	{
		CHECK_SUCCESS(xml.IntoElem());
		GET_ELEMENT_UCHAR(GraphicAttribute.LineColor.Alpha, Opacity, xml);
		GET_ELEMENT_UCHAR(GraphicAttribute.LineColor.Red, Red, xml);
		GET_ELEMENT_UCHAR(GraphicAttribute.LineColor.Green, Green, xml);
		GET_ELEMENT_UCHAR(GraphicAttribute.LineColor.Blue, Blue, xml);
		xml.OutOfElem();
	}

	GET_ELEMENT_ULONG(GraphicAttribute.ulLineWidth, GraphicLineWidth, xml);

	GET_ELEMENT_ULONG(GraphicAttribute.NamePosition, NamePosition, xml);
	GET_ELEMENT_BOOL(GraphicAttribute.bAllowDelete, DeleteByMouse, xml);
	GET_ELEMENT_BOOL(GraphicAttribute.bAllowAddNewPoint, AddNewPoint, xml);
	GET_ELEMENT_BOOL(GraphicAttribute.bAllowModifySize, AllowModifySize, xml);
	GET_ELEMENT_ULONG(GraphicAttribute.ulArrowType, ArrowType, xml);
	GET_ELEMENT_BOOL(GraphicAttribute.bKeepHorizon, KeepHorizon, xml);

	xml.OutOfElem();
	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;

}

// 构建选中图形消息XML
IVS_INT32 COCXDrawXMLProcess::SelectGraphicGetXML(CString &xmlToCU, IVS_ULONG ulGraphicGroup)
{
	IVS_DEBUG_TRACE("GraphicGroup: %lu", ulGraphicGroup);
	CXml xmlGraphics;
	char val[64] = {0};
	unsigned int uiLen = 0;

	CHECK_SUCCESS(xmlGraphics.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(xmlGraphics.AddElem("Content"));
	CHECK_SUCCESS(xmlGraphics.AddChildElem("GraphicGroup"));
	CHECK_SUCCESS(xmlGraphics.IntoElem());
	SET_ELEMENT_LONG(ulGraphicGroup, val, xmlGraphics);
	xmlGraphics.OutOfElem();
	xmlToCU = xmlGraphics.GetXMLStream(uiLen);

	return IVS_SUCCEED;
}

// 构建去选中图形消息XML
IVS_INT32 COCXDrawXMLProcess::UnSelectGraphicGetXML(CString &xmlToCU)
{
	IVS_DEBUG_TRACE("");
	CXml xmlGraphics;
	unsigned int uiLen = 0;

	CHECK_SUCCESS(xmlGraphics.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(xmlGraphics.AddElem("Content"));
	xmlToCU = xmlGraphics.GetXMLStream(uiLen);

	return IVS_SUCCEED;
}
