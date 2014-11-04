/********************************************************************
	filename	: 	ToolsHelp.cpp
	author		:	s00191067
	created		:	2011/10/29	
	description	:	提供各个模块公共函数的工具文件
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/29 初始版本
*********************************************************************/
/*!< 预编译头文件 */
#include "stdafx.h"
#include <atlenc.h>     // 不包含则链接失败，找不到PathRemoveFileSpec
#include <shlwapi.h>    // 获取程序路径使用:PathRemoveFileSpec

/*!< 自身头文件 */
#include "ToolsHelp.h"

/*!< 宏定义 */
#define  STR_LENGTH_SIZE	    20
#define	 INT_FORMAT_D		    10
#define  INT_FORMAT_X		    16
#define  STR_FORMAT_D		    "%d"
#define  STR_FORMAT_X		    "%x"
#define SDK_CONFIG_INI_NAME     ("SDKConfig.ini")
#define SDK_LOG_LEVEL           ("LogLevel")
#define ZERO_ASCII_VALUE        48          //0的ASCII码值
#define TEN                     10          //被乘数10

//获取程序所在路径
string CToolsHelp::GetProgramDir()
{
    // 获取程序安装目录 strPath
    CHAR szPathName[MAX_PATH];
    (void)::GetModuleFileName(NULL , szPathName , MAX_PATH);
    (void)::PathRemoveFileSpec(szPathName);
    string strPath = szPathName;
    strPath += "\\";

    return strPath;
}

//将整数转化为字符串
string CToolsHelp::Int2Str ( int iValue, bool bHex /*= false*/  )
{
    char szTmp[STR_LENGTH_SIZE];
    if ( bHex )
    {
        (void)_itoa_s( iValue, szTmp, INT_FORMAT_X );
    }
    else
    {
        (void)_itoa_s( iValue, szTmp, INT_FORMAT_D );
    }	

    return string( szTmp );
}

//将整数转化为字符串
int CToolsHelp::Str2Int( const string& strSrc, bool bHex )
{
    int iRet = 0;	
    if ( bHex )
    {
        (void)sscanf_s ( strSrc.c_str(), STR_FORMAT_X, &iRet );
    }
    else
    {
        (void)sscanf_s ( strSrc.c_str(), STR_FORMAT_D, &iRet );
    }	

    return iRet;
}

//字符编码转换，从ANSI到UNICODE
const WCHAR* CToolsHelp::ANSI2Unicode(const char*src)
{
    static WCHAR tszBuffer[10240];
    memset(tszBuffer,0,10240*sizeof(WCHAR));
    if (NULL == src)
    {
        (void)MultiByteToWideChar(CP_ACP,0, "",
            (int)(strlen("")+1),
            tszBuffer,sizeof(tszBuffer)/sizeof(tszBuffer[0]));
    }
    else
    {
        (void)MultiByteToWideChar(CP_ACP,0, src,
            (int)(strlen(src)+1),
            tszBuffer,sizeof(tszBuffer)/sizeof(tszBuffer[0]));
    }
    return tszBuffer;
}

//字符编码转换，从UNICODE到ANSI
const char* CToolsHelp:: Unicode2ANSI(const WCHAR *src)
{
    static char szBuffer[20480];
	//modify by limingjie lWX175483 :2013-5-24   memset参数写反了
    memset(szBuffer,0,20480);
    if (NULL == src)
    {
        (void)WideCharToMultiByte(CP_ACP,0,L"",-1,szBuffer,20480,NULL,NULL);
    }
    else
    {
        (void)WideCharToMultiByte(CP_ACP,0,src,-1,szBuffer,20480,NULL,NULL);
    }
    return szBuffer;
}


//取配置文件中的日志级别
IvsLogLevel CToolsHelp::GetLogLevel()
{
    char szBuf[MAX_PATH] = {'\0'}; 
    
	//文件路径   
    string strProgrameDir = GetProgramDir();
    string strFileName = strProgrameDir + SDK_CONFIG_INI_NAME;
    string strDefault = Int2Str(LOG_ERROR);

    (void)::GetPrivateProfileString(SDK_LOG_LEVEL,SDK_LOG_LEVEL,strDefault.c_str(),szBuf,MAX_PATH,strFileName.c_str());
    
    string strLogLevel = szBuf;
    IvsLogLevel LogLevel = (IvsLogLevel)Str2Int(strLogLevel);

    return LogLevel;
}

/**************************************************************************
* name       : 
* description: 
* input      : 
* output     : 
* return     : NA
* remark     : NA
**************************************************************************/
bool CToolsHelp::ConvertIPToInt(char *pszIP,unsigned int &iIP)
{

    int iStrLen = strlen(pszIP);

    char *pszTempIP = pszIP;
    int iTempIP = 0;
    int iDotCount = 0;

    if ('.' == *(pszTempIP+iStrLen-1))
    {
        return false;
    }
    for (int i = 0;i <(iStrLen);i++)
    {
        if ('.' == *pszTempIP)
        {
            if (0xFF < iTempIP)
            {
                return false;
            }
            iIP += iTempIP;
            iIP <<= 8; 
            iTempIP = 0;
            iDotCount++;
        }
        else
        {
            iTempIP *= TEN;
            iTempIP += (int)(*pszTempIP - ZERO_ASCII_VALUE);
        }
        pszTempIP++;
    }
    iIP += iTempIP;
    if (3 != iDotCount)
    {
        return false;
    }
    return true;
}

unsigned int CToolsHelp::AddrChar2Uint(const char *name)
{
	unsigned int ret = 0;
	unsigned int segment = 0;
	unsigned int valid = 1;        //地址是否合法

	for (unsigned int i = 0; i < 4; ++i)
	{
		ret <<= 8;
		if (*name != '\0')
		{
			segment = 0;

			while ((*name >= '0') && (*name <= '9'))
			{
				segment *= 10;
				segment += (*name - '0'); 
				name++;
			}

			if (*name != '.' && *name != '\0')
			{
				valid = 0;
				break;
			}

			ret |= segment;

			if (*name == '.')
			{
				++name;
			}
		}
	}
	return valid ? ret : 0;
}