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
#include <map>
#include <string>

/*!< 屏蔽PCLint */
#include "IvsLog.h"

using namespace std;

//typedef std::map <string, string> HttpHeadInfoMap;

class CToolsHelp
{
public:
    /**************************************************************************
    * name       : GetProgramDir
    * description: 获取程序所在路径
    * input      : NA
    * output     : NA
    * return     : 程序所在路径
    * remark     : NA
    **************************************************************************/
    static string GetProgramDir();

    /***********************************************************************************
    * name       : Int2Str
    * description: 将整数转化为字符串
    * input      : iValue:要转化的整型数值; bHex:是否是十六进制整型数
    * output     : NA
    * return     : 转化后的字符串
    * remark     : NA
    ***********************************************************************************/
    static string Int2Str ( int iValue, bool bHex = false );

    /***********************************************************************************
    * name       : Str2Int
    * description: 将整数转化为字符串
    * input      : strSrc:要转化的字符串; bHex:是否是十六进制整型数
    * output     : NA
    * return     : 转化后的整型数
    * remark     : NA
    ***********************************************************************************/
    static int Str2Int( const string& strSrc, bool bHex = false );

    /**************************************************************************
    * name       : ANSI2Unicode
    * description: 字符编码转换，从ANSI到UNICODE
    * input      : src 待转换的字符
    * output     : NA
    * return     : TCHAR* 宽字符
    * remark     : 约定最大字符串长度为1024,采用了Static变量存储结果，可以防止频繁使用时
    产生内存碎片
    **************************************************************************/
    static const WCHAR* ANSI2Unicode(const char* src);

    /**************************************************************************
    * name       : Unicode2ANSI
    * description: 字符编码转换，从UNICODE到ANSI
    * input      : src 待转换的宽字符
    * output     : NA
    * return     : char* Ansi字符
    * remark     : 约定最大字符串长度为1024,采用了Static变量存储结果，可以防止频繁使用时
    产生内存碎片
    **************************************************************************/
    static const char* Unicode2ANSI(const WCHAR* src);

    /**************************************************************************
    * name       : 
    * description: 
    * input      : 
    * output     : 
    * return     : NA
    * remark     : NA
    **************************************************************************/
    static  bool ConvertIPToInt(char *pszIP,unsigned int &iIP);

    /**************************************************************************
    * name       : GetLogLevel
    * description: 取配置文件中的日志级别
    * input      : 
    * output     : 
    * return     : unsigned int
    * remark     : NA
    **************************************************************************/
    static IvsLogLevel GetLogLevel();

	static unsigned int AddrChar2Uint(const char *name);
};

#endif //__TOOLSHELP_H__               
