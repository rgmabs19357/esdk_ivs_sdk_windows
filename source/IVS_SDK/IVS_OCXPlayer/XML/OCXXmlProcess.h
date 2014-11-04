/********************************************************************
	filename	: 	OCXXMLProcess.h
	author		:	z00193167
	created		:	2012/12/03	
	description	:	XML公用定义
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/03 初始版本
*********************************************************************/
#ifndef __OCX_XML_PROCESS_H__
#define __OCX_XML_PROCESS_H__

#include <afx.h>
#include "QueryFiled.h"
#include "Utility.h"
#include "ToolsHelp.h"
#include "SDKDef.h"


#define QUERYFILED CQueryFiled::GetInstance()


/* 
*  获取结点信息-字符
*  elem        待查找的元素，如"DEV_INFO"
*  szXMLValue  存放结点元素的指针
*  NodeInfo    缓存结点元素的值
*  iMaxLen     缓存区的最大值
*  xml         待解析XML
*/
#define GET_ELEM_VALUE_CHAR(elem, szXMLValue, NodeInfo, iMaxLen, xml)                  \
	szXMLValue = NULL      ;                                                            \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
	{                                                                                  \
	(IVS_VOID)CToolsHelp::Memcpy((char *)NodeInfo, iMaxLen, szXMLValue, strlen(szXMLValue));  \
	}                                                                                  \
}

/* 
*  获取结点信息-数值
*  elem        待查找的元素，如"DEV_INFO"
*  szXMLValue  存放结点元素的指针
*  NodeInfo    缓存结点元素的值
*  xml         待解析XML
*/
#define GET_ELEM_VALUE_NUM(elem, szXMLValue, NodeInfo, xml)                            \
	NodeInfo = 0 ;                                                                   \
	if (xml.FindElem(elem))                                                                \
{			                                                                            \
	NodeInfo = 0;                                                                                \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
   {                                                                                   \
   NodeInfo = atoi(szXMLValue);                                                    \
   }                                                                                   \
}

#define GET_ELEM_VALUE_NUM_FOR_UINT(elem, szXMLValue, NodeInfo, xml)                                 \
	NodeInfo = 0;                                                                                        \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
   {                                                                                   \
   NodeInfo = static_cast<IVS_UINT32>(atoi(szXMLValue));                                                    \
   }                                                                                   \
}

#define GET_ELEM_VALUE_FLOAT(elem, szXMLValue, NodeInfo, xml)                                 \
    NodeInfo = 0;                                                                                        \
    if (xml.FindElem(elem))                                                                \
{																	                   \
    szXMLValue = xml.GetElemValue();                                                   \
    if (NULL != szXMLValue)                                                            \
   {                                                                                   \
   NodeInfo = static_cast<IVS_FLOAT>(atof(szXMLValue));                                                    \
   }                                                                                   \
}

#define GET_ELEM_VALUE_DOUBLE(elem, szXMLValue, NodeInfo, xml)                                 \
	NodeInfo = 0;                                                                                        \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
   {                                                                                   \
   NodeInfo = static_cast<IVS_DOUBLE>(atof(szXMLValue));                                                    \
   }                                                                                   \
}


class COCXXmlProcess
{
public:
	/******************************************************************
	function : GetVideoNumAndResourceXML
	description: 查询MU的资源预算是否达到上限及当前摄像机视频流是否被引用
	input :  iRet ResultCode
			 bReachLimit 是否达到上限
	         bWhetherCited 是否被引用
	output : strOutXml 返回XML
	return : IVS_INT32
	*******************************************************************/
	static IVS_INT32 GetVideoNumAndResourceXML(IVS_INT32 iRet,IVS_BOOL bReachLimit,IVS_BOOL bWhetherCited, std::string& strOutXml);

	/******************************************************************
	function : GetUnifiedFormatQueryParseXML
	description: 通用的查询xml，在调用完成之后，要删除掉pReqBuffer(IVS_DELETE(pReqBuffer, MUTII))
	input :  CXml & xmlReq
	         IVS_INT32 iFiedNum
	output : IVS_VOID *  & pReqBuffer
	return : IVS_INT32
	*******************************************************************/
	static IVS_INT32 GetUnifiedFormatQueryParseXML(IVS_VOID* &pReqBuffer,CXml& xmlReq,IVS_INT32 iFiedNum);

    /******************************************************************
	function : GetUnifiedFormatQueryParseXML
	description: 通用的查询xml，在调用完成之后，要删除掉pReqBuffer(IVS_DELETE(pReqBuffer, MUTII))
	input :  CXml & xmlReq
	         IVS_INT32 iFiedNum
	output : IVS_VOID *  & pReqBuffer
    output : IVS_CHAR *  & pDomainCode 域编码
	return : IVS_INT32
	*******************************************************************/
	static IVS_INT32 GetUnifiedFormatQueryParseXMLEx(IVS_VOID* &pReqBuffer,CXml& xmlReq,IVS_INT32 iFiedNum,IVS_CHAR* pDomainCode);

	
	/*****************************************************************
	     function   : GetAlarmErrString
	     description: 查询告警级别列表返回的xml
		 intput     : IVS_INT32 iRet
	     output     : CString & strResult  返回的报文
	     return     : NA
	*****************************************************************/
	static IVS_VOID GetAlarmErrString( CString &strResult, IVS_INT32 iRet );

	/*****************************************************************
	     function   : AlloceQueryStructMemory
	     description: 分配通用格式内存大小
	     output     : IVS_VOID *  & pReqBuffer 申请内存的指针
	     output     : IVS_INT32 iFieldNum      申请内存块得个数
	     return     : 成功返回0，失败返回响应的错误码
	*****************************************************************/
	static IVS_INT32 AlloceQueryStructMemory( IVS_VOID* &pReqBuffer, IVS_INT32 iFieldNum );

	/*****************************************************************
	     function   : GetIsUp
	     description: 判断是否为升序
	     intput     : const IVS_CHAR * pQueryUp  需要比较的字符串
	     return     : 是 返回1  否 返回0
	*****************************************************************/
	static IVS_BOOL GetIsUp(const IVS_CHAR* pQueryUp);

	/*****************************************************************
	     function   : GetQueryType
	     description: 判断是否为精确
	     output     : const IVS_CHAR * pQueryType  需要比较的字符串
	     return     : 是 返回1  否 返回0
	*****************************************************************/
	static IVS_BOOL GetQueryType(const IVS_CHAR* pQueryType);
	
	/*****************************************************************
	     function   : GetErrString
	     description: 拼装错误码的xml
	     output     : CString & strResult 返回给CU的报文
	     input      : IVS_INT32 iRet  错误码
	     return     : NA
	*****************************************************************/
	static IVS_VOID GetErrString( CString &strResult, IVS_INT32 iRet );

	/*****************************************************************
	     function   : GetErrorResultXML
	     description: 拼装错误码的xml
	     input      : IVS_INT32 iRetCord  错误码
	     output     : CXml & xml 拼装后的xml
	     return     : 
	*****************************************************************/
	static IVS_VOID GetErrorResultXML(IVS_INT32 iRetCord, CXml &xml);

	/*****************************************************************
	     function   : GetFieldNum
	     description: 获取查询字段的个数
	     input      : CXml & xmlReq  所要查找的报文
	     return     : 查询到的个数
	*****************************************************************/
	static IVS_INT32 GetFieldNum(CXml &xmlReq);

	/*****************************************************************
	     function   : GetResultXML
	     description: 拼装返回码到xml
	     input      : IVS_INT32 & iRet 返回码
	     input      : const IVS_CHAR * pRspXml
	     output     : CString & strResult 拼装后的xml
	     return     : 成功返回0，失败返回错误码
	*****************************************************************/
	static IVS_INT32 GetResultXML(IVS_INT32& iRet, const IVS_CHAR* pRspXml, CString& strResult);

	/*****************************************************************
	     function   : GetElemCount
	     description: 获取指定路径（含节点名称）节点数
	     input      : const IVS_CHAR * pElemPath 节点路径
		 input      :  CXml & xml                xml报文
	     output     : IVS_UINT32 & uiNum         节点个数
	     return     : 成功返回0，失败返回1
	*****************************************************************/
	static IVS_UINT32 GetElemCount(const IVS_CHAR* pElemPath,IVS_UINT32 &uiNum, CXml &xml);
	
	/*****************************************************************
	     function   : ParseDevCode
	     description: 解析CU传入带域的CODE
	     input      : const IVS_CHAR * pDevBigCode  CU传入带域的CODE
	     output     : IVS_CHAR * pDevCode           解析后的设备编码
	     output     : IVS_CHAR * pDomainCode        解析后的域编码
	     return     : 成功返回0，失败返回相应的错误码
	*****************************************************************/
	static IVS_INT32 ParseDevCode(const IVS_CHAR *pDevBigCode, IVS_CHAR *pDevCode, IVS_CHAR* pDomainCode );
    
	/******************************************************************
    function : GetXmlDevinfoNum
    description: 查询设备参数的个数
    input : CXml & xml
    return : 返回节点的个数
    *******************************************************************/
    static IVS_UINT32 GetXmlDevinfoNum(CXml &xml);

	/*****************************************************************
	     function   : AddResultCodeUnderContent
	     description: 在content节点下面加一个resultcode节点
	     input      : iResultCode  结果码
	                  strInXml 输入XML
	     output     : strOutXml 输出拼装后的
	     return     : 成功返回0，失败返回相应的错误码
	*****************************************************************/
	static IVS_INT32 AddResultCodeUnderContent(IVS_INT32 iResultCode, const std::string& strInXml, std::string& strOutXml);

	// 转换int类型到BOOL
	static bool TransIntToBool(unsigned int iRet);

	// 转换0,1到字串ON,OFF
	static std::string TransIntToOnOff(unsigned int iRet);

	// 转换ON,OFF到0,1的
	static unsigned int TransOnOffToInt(const std::string &str);

	// 字符串转换到long
	static long TransStrToLong(const std::string& str);

	// 字符串0,1到数字0,1的转换
	static  int TransStrToInt(const std::string& str);

	// 定义字符数组到字串的转换，主要用于数据结构中字符数字的转换
	static void SetElemValueStr(const IVS_CHAR* pXmlValue,IVS_UINT32 ulLength,CXml &xml);

	// 定义将十进制的数字转换为二进制字串
	static std::string TransDecToBinStr(unsigned int ulValue);

	//拼装OCX返回事件的XML
	static void GetEventXML(CXml &xml,const std::string &strEvent,ULONG ulWndID,const std::string &strStatus = "");

    static void GetTalkbackXML(CString &strResult, IVS_INT32 iRet, IVS_ULONG ulHandle);

	static int BuildShareCatalogResultXml(IVS_INT32 iRet, const IVS_CHAR* pRspXml, CString& strResult);
};

#endif // __OCX_XML_PROCESS_H__
