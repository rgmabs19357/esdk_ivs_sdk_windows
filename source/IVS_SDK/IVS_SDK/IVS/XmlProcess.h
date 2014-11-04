/********************************************************************
	filename	: 	XMLProcess.h
	author		:	z00193167
	created		:	2012/12/03	
	description	:	XML公用定义
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/03 初始版本
*********************************************************************/

#ifndef __XML_PROCESS_H__
#define __XML_PROCESS_H__

#include "IVSCommon.h"
#include "ToolsHelp.h"
#include "QueryFiled.h"
#include "SDKDef.h"
#include "bp_os.h"

#define QUERYFILED CQueryFiled::GetInstance()

#define MATCHED_SUCCEED 1
#define MATCHED_FAIL 0
#define ON_LINE 1
#define OFF_LINE 0
#define MAXLEN 64
#define MATCHED 0

//sdk内部域编码
#define IVS_SDK_DEV_CODE_LEN 32

//统一使用
#define BUILDRET   1
#define BUILSTRING 2

//编码类型
enum enConfigType
{
	SDKCAMERA = 1, //CameraCode
	SDKDEVICE,  //DevCode
	SDKALARMINCODE, //AlarmInCode
	SDKALARMOUTCODE, //AlarmOutCode
	SDKSERIAL, //SerialCode
	SDKVOICE //VoiceCode
};

#define GET_ELEM_VALUE_POINTER(elem, pXMLValue, xml)  \
    pXMLValue = NULL;                             \
    if (xml.FindElem(elem))                        \
{                                                                  \
    pXMLValue = xml.GetElemValue();    \
}

/* 
*  获取结点信息-字符
*  elem        待查找的元素，如"DEV_INFO"
*  szXMLValue  存放结点元素的指针
*  NodeInfo    缓存结点元素的值
*  iMaxLen     缓存区的最大值
*  xml         待解析XML
*/
#define GET_ELEM_VALUE_CHAR(elem, szXMLValue, NodeInfo, iMaxLen, xml)                  \
    szXMLValue = NULL;                                                                  \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
	{                                                                                  \
	(void)CToolsHelp::Memcpy(NodeInfo, iMaxLen, szXMLValue, strlen(szXMLValue));  \
	}                                                                                  \
}

/* 
*  获取结点信息-数值
*  elem        待查找的元素，如"DEV_INFO"
*  szXMLValue  存放结点元素的指针
*  NodeInfo    缓存结点元素的值
*  xml         待解析XML
*/
#define GET_ELEM_VALUE_NUM(elem, szXMLValue, NodeInfo, xml)                                 \
	NodeInfo = 0;                                                                                        \
	if (xml.FindElem(elem))                                                                \
{																	                   \
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

#define GET_ELEM_VALUE_NUM_FOR_UINT64(elem, szXMLValue, NodeInfo, xml)                                 \
	NodeInfo = 0;                                                                                        \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
   {                                                                                   \
   NodeInfo = static_cast<IVS_UINT64>(BP_OS::strtoll(szXMLValue,NULL,10));                                             \
   }                                                                                   \
}

#define GET_ELEM_VALUE_NUM_FOR_FLOAT(elem, szXMLValue, NodeInfo, xml)                                 \
	NodeInfo = 0;                                                                                        \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
   {                                                                                   \
   NodeInfo = static_cast<IVS_FLOAT>(atof(szXMLValue));                                                    \
   }                                                                                   \
}

#define GET_ELEM_VALUE_NUM_FOR_DOUBLE(elem, szXMLValue, NodeInfo, xml)                                 \
	NodeInfo = 0;                                                                                        \
	if (xml.FindElem(elem))                                                                \
{																	                   \
	szXMLValue = xml.GetElemValue();                                                   \
	if (NULL != szXMLValue)                                                            \
	{                                                                                   \
	NodeInfo = static_cast<IVS_DOUBLE>(atof(szXMLValue));                                                    \
	}                                                                                   \
}


// 判断指针是否为空 
#define CHECK_IVSBOOL( retValue )    \
	if (IVS_SUCCEED != CParaCheck::CheckIsIvsBool(retValue))                    \
{                                            \
	return IVS_PARA_INVALID;                         \
}                                            \

class CUserMgr;//lint !e763
class CXmlProcess
{
public:
	/**************************************************************************
    * name       : GetUnifiedFormatQueryXML
    * description: 通用查询转换成XML
    * input      : pUnifiedQuery   通用分页查询结构体
                   xml  输出的Cxml类
    * output     : NA
    * return     : 通用返回
    * remark     : NA
    **************************************************************************/
	static IVS_INT32 GetUnifiedFormatQueryXML(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery, CXml &xml, 
													const IVS_CHAR* pDomainCode = NULL,const IVS_CHAR* pCamerCode = NULL);
	static IVS_INT32 PrintfXML2File(const IVS_CHAR* FileName,const IVS_CHAR* XMLContent);

    static IVS_INT32 GetCommBuildRet(CXml &xml, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const std::string& strLinkID);
	static IVS_INT32 GetCommBuildRet(CXml &xmlReq, CXml &xmlRsp, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const std::string& strLinkID);

    static IVS_INT32 ParseDevCode(const IVS_CHAR *pDevBigCode, IVS_CHAR *pDevCode,IVS_CHAR* pDomainCode );

	//只支持同一个域下的设备编码获取
	static IVS_INT32 ParseDevCodeList(std::list<std::string>& lDevCodeList,std::string& strCamerCode,IVS_CHAR* pDomainCode );

	//转换int类型到BOOL
	static bool TransIntToBool(unsigned int iRet);

	//转换0,1到字串ON,OFF
	static std::string TransIntToOnOff(unsigned int iRet);

	//转换ON,OFF到0,1的
	static unsigned int TransOnOffToInt(const std::string &str);

	//字符串转换到long
	static long TransStrToLong(const std::string& str);

	//字符串0,1到数字0,1的转换
	static  int TransStrToInt(const std::string& str);

	//定义字符数组到字串的转换，主要用于数据结构中字符数字的转换//
	static void SetElemValueStr(const IVS_CHAR* pXmlValue,IVS_UINT32 ulLength,CXml &xml);

	//二进制字串转换到十进制数字
	static unsigned int TransBinStrToDec(const std::string& str);

	// 修改AlarmInCode
	static IVS_INT32  SetAlarmInCodeValue(const IVS_CHAR* pPath, CXml& xml);

	//通用的查询请求
	static IVS_INT32 GetCommConfigGetXML(const IVS_CHAR* pDomainCode,const IVS_CHAR* pDevCode, CXml& xml, enConfigType enType);

	// 实现外域发送消息公共方法
	static IVS_INT32 GetCommSendMgr(CXml &xml, CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const IVS_CHAR* pDomainCode);

	// 实现外域发送消息公共方法 -- 解决内存泄露
	static IVS_INT32 GetCommSendMgr(CXml &xml, CXml &OutXml,CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const IVS_CHAR* pDomainCode);

	// 实现外域发送消息公共方法(支持重定向)
	static IVS_INT32 GetCommSendMgrForRedirect(CXml &xml, CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType,  const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode);

	// 实现外域发送消息公共方法(支持重定向)---- 解决内存泄露
	static IVS_INT32 GetCommSendMgrForRedirect(CXml &reqXml, CXml &rspXml,CUserMgr* pUserMgr, NSS_MSG_TYPE_E msgType, IVS_INT32 iType, const IVS_CHAR* pCameraCode,const IVS_CHAR* pDomainCode);

	// 通用请求XML增加QueryField字段
	static IVS_INT32 AddQueryFieldForUnifiedFormatQueryXML(const IVS_CHAR* pField,const IVS_CHAR* pValue, CXml& xmlReq);

};

#endif
