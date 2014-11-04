/********************************************************************
filename    : MediaXMLProcess.h
author      : c00206592
created     : 2013/01/31
description : 媒体操作所需的XML生成解析类
copyright   : Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     : 2013/01/31 初始版本
*********************************************************************/

#ifndef __MEDIA_XML_PROCESS_H__
#define __MEDIA_XML_PROCESS_H__

#include "SDKDef.h"

#define ENCRY_KEY_LEN 90 

class CMediaXMLProcess
{
public:
	CMediaXMLProcess();
	~CMediaXMLProcess();
public:
	/***************************************************************************
     函 数 名 : VideoNumAndResourceGetXML
     功能描述 : 查询MU的资源预算是否达到上限及当前摄像机视频流是否被引用
     输出参数 : xml                    请求xml
     返 回 值 : 0 -成功 ;1-失败
    ***************************************************************************/
	static IVS_INT32 VideoNumAndResourceGetXML(const std::string& strCameraCode, CXml &xml);

	/******************************************************************
     function   : ParseVideoReqNumberAndReqRSP
     description: 解析资源预算是否达到上限及当前摄像机视频流是否被引用
     input      : const char * pXML
     output     : iReachLimit 是否达到上限
				  iWhetherCited 是否被引用
     return     : int IVS_SUCCEED成功 IVS_FAIL失败;
    *******************************************************************/
	static IVS_INT32 ParseVideoNumAndResource(CXml& xmlRsq,IVS_INT32& iReachLimit, IVS_INT32& iWhetherCited);

	/***************************************************************************
     函 数 名 : ApplyStreamencryPwdGetXML
     功能描述 : 请求录像加密的工作密钥或者异或因子xml;
     输出参数 : xml                    请求xml
     返 回 值 : 0 -成功 ;1-失败
    ***************************************************************************/
	static IVS_INT32 ApplyStreamencryPwdGetXML(const std::string& strCameraCode,const std::string& strDomainCode,
		const std::string& strDestModule,int& iStreamType,const std::string& strSessionID,const std::string& strSecurityTransID, CXml &xml);

	/******************************************************************
     function   : ParseStreamencryKey
     description: 解析媒体安全算法用到的key的密文值
     input      : const char * pXML
     output      : std::string & strStreamencryKey
     return     : int IVS_SUCCEED成功 IVS_FAIL失败;
    *******************************************************************/
	static IVS_INT32 ParseStreamencryKey(CXml& xmlRsq,char* pStreamencryKey);

	/***************************************************************************
     函 数 名 : GetURLXML
     功能描述 : 将结构体转成xml;
     输入参数 : stUrlMediaInfo         url媒体信息
	 输入参数 ：stSRULoginRsp          登陆信息
     输出参数 : xml                    请求xml
     返 回 值 : 0 -成功 ;1-失败
    ***************************************************************************/
	static IVS_INT32 GetURLXML(const ST_URL_MEDIA_INFO& stUrlMediaInfo,
								const ST_SRU_LOGIN_RSP& /*stSRULoginRsp*/, CXml &xml);

	/***************************************************************************
     函 数 名 : GetURLResponseData
     功能描述 : 解析GetUrlXML
     输入参数 : stMediaRsp         媒体信息
     输出参数 : xmlRsp                    响应xml
     返 回 值 : 0 -成功 ;1-失败
    ***************************************************************************/
	static IVS_INT32 GetURLResponseData(CXml &xmlRsp, ST_MEDIA_RSP& stMediaRsp, bool& bAssociatedAudio);
};
#endif //__MEDIA_XML_PROCESS_H__

