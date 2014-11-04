/********************************************************************
	filename	: 	ToolsHelp.h
	author		:	s00191067
	created		:	2011/10/28	
	description	:	提供各个模块公共函数的工具文件
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/28
*********************************************************************/

#ifndef __TOOLSHELP_H__
#define __TOOLSHELP_H__

/*!< 预编译头文件 */

/*!< 屏蔽PCLint */
#include "ivs_xml.h"
#include "hwsdk.h"
#include "SDKDef.h"

#ifdef WIN32 
#define sscanf sscanf_s //lint !e683
#endif


typedef std::map <std::string, std::string> HttpHeadInfoMap;

class CToolsHelp
{
public:

    /***********************************************************************************
    * name       : Int2Str
    * description: 将整数转化为字符串
    * input      : iValue:要转化的整型数值; bHex:是否是十六进制整型数
    * output     : NA
    * return     : 转化后的字符串
    * remark     : NA
    ***********************************************************************************/
    static std::string Int2Str ( int iValue, bool bHex = false );

	 /***********************************************************************************
    * name       : ItoaEX
    * description: 将十进制整数转化为字符串
    * input      : iValue:要转化的整型数值;
    * output     : buff:转化后的字符串
    * return     : 成功：转化后的字符串实际字符长度;失败返回-1;
    * remark     : NA
    ***********************************************************************************/
    static int ItoaEX ( int iValue, char *buff );
	
    /***********************************************************************************
    * name       : StrToInt
    * description: 将整数转化为字符串
    * input      : strSrc:要转化的字符串; bHex:是否是十六进制整型数
    * output     : NA
    * return     : 转化后的整型数
    * remark     : NA
    ***********************************************************************************/
    static int StrToInt( const std::string& strSrc, bool bHex = false );

    /**************************************************************************
    * name       : GetSCCHead
    * description: 登录SCC服务器的头
    * input      : NA
    * output     : NA
    * return     : SCC服务器PORT
    * remark     : NA
    **************************************************************************/
	static std::string GetSCCHead();

	static int ResultCodeToErrorCode(int iResultCode);

    static bool IsNum(const std::string &str);

    /**************************************************************************
    * name       : GetHostName
    * description: 获取计算机机器名
    * input      : 
    * output     : pHostName 计算机机器名
    * return     : int 成功返回0，失败返回1
    * remark     : NA
    **************************************************************************/
    static int GetHostName(char* pHostName);

    /**************************************************************************
    * name       : GetCurrentUserName
    * description: 获取计算机当前登录用户名
    * input      : 
    * output     : pUserName 当前登录用户名
    * return     : int 成功返回0，失败返回1
    * remark     : NA
    **************************************************************************/
	static int GetCurrentUserName(char* pUserName);

    /******************************************************************
     function   : get_exe_parent_dir
     description: 获取程序所在目录的父目录
     output      : char * buf
     return     : IVS_INT32 成功返回0， 失败返回错误码
    *******************************************************************/
    static int get_exe_parent_dir(char *buf);

    /******************************************************************
     function   : get_home_directory
     description: 获取主目录
     output      : std::string & strDir
     return     : IVS_INT32
    *******************************************************************/
    static int get_home_directory(std::string& strDir);

    /******************************************************************
     function   : get_config_directory
     description: 获取config目录
     output      : std::string & strDir
     return     : IVS_INT32
    *******************************************************************/
    static int get_config_directory(std::string& strDir);

	/**************************************************************************
	* name       : TranslateToSer
	* description: 本地与服务器之间的数据翻译
	* input      : iNum		待翻译的数
	*			   pscAgree	含肯定词的字符串
			       pscDisAgree 含否定词的字符串
	  output     : NA
	* return     : string
	* remark     : NA
    **************************************************************************/
	static const char*  TranslateToSer(const char *pscAgree, const char *pscDisAgree, int &iNum);

	/**************************************************************************
	* name       : TranslateToSer
	* description: 本地与服务器之间的数据翻译
	* input      : pscTemp		待翻译的串
	  output     : iNum 翻译后的数
	* return     : 成功返回翻译后的整形数
	* remark     : NA
    **************************************************************************/
	static int TranslateFromSer(const char *pscTemp, int &iNum);

	    /***********************************************************************************
    * name       : LTrim
    * description: strSrc去除左空格。
    * input      : src; 
    * output     : 
    * return     : 返回大写的string
    * remark     : NA
    ***********************************************************************************/
    static void LTrim(std::string& strSrc);

    /***********************************************************************************
    * name       : LTrim
    * description: strSrc去除右空格。
    * input      : src; 
    * output     : 
    * return     : 返回大写的string
    * remark     : NA
    ***********************************************************************************/
    static void RTrim(std::string& strSrc);

    /***********************************************************************************
    * name       : LTrim
    * description: strSrc去除左右空格。
    * input      : src; 
    * output     : 
    * return     : 返回大写的string
    * remark     : NA
    ***********************************************************************************/
    static std::string Trim(std::string& strSrc);



	static wchar_t* ANSIToUnicode(const char* str);
	static wchar_t* UTF8ToUnicode(const char* str);
	static char* UnicodeToUTF8(const wchar_t* str);
	static char* UnicodeToANSI(const wchar_t* str);
	static char* UTF8ToANSI(const char* str);
	static char* ANSIToUTF8(const char* str);

    /******************************************************************
     function   : SplitCameraDomainCode
     description: 从CU发过来的cameraid是合并后的,要拆分成cameracode和domaincode，为兼容以前的版本，
                  如果strCameraDomainCode中没有#拼装字段，直接赋值给strCameraCode
     input      : const std::string & strCameraDomainCode   CU端的合并的Code，格式为摄像机编码#域编码
     input      : std::string & strCameraCode   摄像机编码
     input      : std::string & strDomainCode   域编码
     output     : NA
     return     : int
    *******************************************************************/
    static int SplitCameraDomainCode(const std::string& strCameraDomainCode, 
                                     std::string& strCameraCode, 
                                     std::string& strDomainCode);


    /******************************************************************
     function   : IsNeedReDirect
     description: 判断请求是否需要重定向;与摄像机操作相关的请求均需重定向;
     input      : unsigned short iReqID 请求ID
     output     : NA
     return     : bool true需要重定向; false不需要;
    *******************************************************************/
    static bool IsNeedReDirect(unsigned short iReqID);

    /**************************************************************************
    * name       : FindElementByPath
    * description: 根据路径获取一个节点
    * input      : 
                   pParentEle TiXmlElement* 父节点名称
                   strPath string& 节点路径
    * return     : TiXmlElement*  返回最后一级节点的指针
    * remark     : NA
    **************************************************************************/
    static TiXmlElement* FindElementByPath(TiXmlNode* pParentEle,std::string strPath);

	/***********************************************************************************
    * name       : Long2Str
    * description: 将长整型转换为字符串
    * input      : iValue:待转换的数据; bHex:是否为16进制
    * output     : NA
    * return     : 转换后的字符串
    * remark     : NA
    ***********************************************************************************/
    static std::string Long2Str ( long lValue, bool bHex = false );

    /******************************************************************
     function   : IsSpeedVaild
     description: 判断是否为合法的播放倍速;给第三方的倍速为：-32 -16 -8 -4 -2 （-1 1 2 4） 8 16 32，
     input      : float fScale  播放倍速
     output     : NA
     return     : bool true 合法，false不合法;
    *******************************************************************/
    static bool IsSpeedVaild(float fScale);

    /******************************************************************
     function   : GetSendScale
     description: 将用户播放速度转换为向服务器发送的播放速度，对应关系如下：
                  用户播放速度                    服务器发送速度
                  1                                     2       2倍速全量
                  2                                     4       4倍速全量
                  4                                     254     8倍速全量前进
                  8                                     16      16倍速只发I帧
                  16                                    32      32倍速只发I帧 
                  -1                                    253     2倍速全量后退
                  -2                                    -4      4倍速只发I帧
                  -4                                    -8      8倍速只发I帧
                  -8                                    -16     16倍速只发I帧
                  -16                                   -32     //待李宁确认
                  0.5                                   1       1倍速全量
                  0.25                                  0.5     1/2全量
                  0.125     8分之1速顺序播放            0.25    1/4全量
                  0.0625    16分之1速顺序播放           0.125   1/8全量
                  0.03125   32分之1速顺序播放           0.0625  1/16全量
                  -0.5      2分之1速后退播放            252     1倍速全量回退
                  -0.25     4分之1速后退播放            252     1倍速全量回退
     input      : IVS_SERVICE_TYPE enServiceType
	 input      : float fUserScale
     input      : float & fSendScale
     output     : NA
     return     : bool
    *******************************************************************/
    static bool GetSendScale(IVS_SERVICE_TYPE enServiceType, float fUserScale, float& fSendScale);

    /**************************************************************************
    * name       : MemCpy
    * description: 封装后的memcpy函数
    * input      : pDst     指向目标内存指针；
                   uiMaxLen   目标内存最大长度；
                   pSrc     指向起始内存指针；
                   uiCopyLen  拷贝长度。
    * output     : NA
    * return     : 成功返回true；失败返回false
    * remark     : NA
	**************************************************************************/
	static bool Memcpy(void* pDst, unsigned int uiMaxLen, const void* pSrc, unsigned int uiCopyLen);

    /**************************************************************************
    * name       : Strncpy
    * description: 封装后的strncpy函数,拷贝成功后自动在字符串结尾添加'\0’
    * input      : pDst     指向目标字符串的指针；
                   MaxLen   拷贝最大长度；
                   pSrc     指向起始字符串的指针；
                   CopyLen  拷贝长度,不包括‘\0’的长度。
    * output     : NA
    * return     : 成功返回true；失败返回false
    * remark     : NA
    **************************************************************************/
    static bool Strncpy(char* pDst, unsigned int uiMaxLen, const char* pSrc, unsigned int uiCopyLen);

    /**************************************************************************
    * name       : Snprintf
    * description: 封装后的snprintf函数，拷贝成功后自动在字符串结尾添加'\0’
    * input      : pBuffer  指向格式化字符串后存储内存的指针；
                   MaxLen   pBuffer拷贝最大长度；
                   pFormat  指向格式化参数列表的指针。
    * output     : NA
    * return     : 成功返回true；失败返回false
    * remark     : NA
    **************************************************************************/
    static bool Snprintf(char* pBuffer, unsigned int uiMaxLen, const char* pFormat, ...);

    /**************************************************************************
    * name       : Strncat
    * description: 封装后的strncat函数
    * input      : pDst     指向目标字符串的指针；
                   uiMaxLen 目标字符串最大长度；
                   pSrc     指向起始字符串的指针。
    * output     : NA
    * return     : 成功返回true；失败返回false
    * remark     : NA
    **************************************************************************/
    static bool Strncat(char* pDst, unsigned int uiMaxLen,const char* pSrc);

    /******************************************************************
     function   : replace_all
     description: 替换所有指定的字符串
     input      : std::string & str
     input      : const std::string & old_value
     input      : const std::string & new_value
     output     : NA
     return     : std::string&
    *******************************************************************/
    static std::string& replace_all(std::string& str,const std::string& old_value,const std::string& new_value);

    /******************************************************************
     function   : replace_all_distinct
     description: 替换所有指定的字符串
     input      : std::string & str
     input      : const std::string & old_value
     input      : const std::string & new_value
     output     : NA
     return     : std::string&
    *******************************************************************/
    static std::string& replace_all_distinct(std::string& str,const std::string& old_value,const std::string& new_value);    
};

#endif //__TOOLSHELP_H__               
