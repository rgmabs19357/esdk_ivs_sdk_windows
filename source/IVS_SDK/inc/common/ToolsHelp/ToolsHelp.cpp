/********************************************************************
	filename	: 	ToolsHelp.cpp
	author		:	s00191067
	created		:	2011/10/29	
	description	:	提供各个模块公共函数的工具文件
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/29 初始版本
*********************************************************************/

//#include "ivs_error.h"

/*!< 自身头文件 */
#include "ToolsHelp.h"
#include "vos_config.h"
#include "nss_mt.h"
#include <time.h> 
#include <io.h>
#include "SDKDef.h"

/*!< 宏定义 */
//#define BUF_SIZE				4096
const double EPSINON = 0.00001;  // double值的误差范围;

struct SEND_SPEED
{
    const float fUserScale; // 用户要求的速度
    const float fSendScale; // 发送给服务器的实际速度
};

// 各请求名的表
static SEND_SPEED s_SpeedNames[] =
{
    /*01*/	{ 1,            2},	// 用户1倍速全量，实际2倍速全量
    /*02*/	{ 2,            4},	// 用户2倍速全量，实际4倍速全量
    /*03*/	{ 4,			4}, // 用户4倍速全量，实际8倍速全量前进
    /*04*/	{ 8,           16},	// 用户8倍速全量，实际16倍速只发I帧
    /*05*/	{ 16,          32},	// 用户16倍速全量，实际32倍速只发I帧
    /*06*/	{ -1,         253},	// 用户1倍速全量后退，实际2倍速全量后退
    /*07*/	{ -2,          -4},	// 用户2倍速只发I帧后退，实际4倍速只发I帧
    /*08*/	{ -4,          -8},	// 用户4倍速只发I帧后退，实际8倍速只发I帧
    /*09*/	{ -8,         -16},	// 用户8倍速只发I帧后退，实际16倍速只发I帧
    /*10*/	{ -16,        -32},	// 用户16倍速只发I帧后退，实际32倍速只发I帧
    /*11*/	{ 0.5,          1},	// 用户1/2倍速全量，实际1倍速全量
    /*12*/	{ 0.25,			1},	// 用户1/4倍速全量，实际1倍速全量
    /*13*/	{ 0.125,		1},	// 用户1/8倍速全量，实际1倍速全量
    /*14*/	{ 0.0625,		1},	// 用户1/16倍速全量，实际1倍速全量
    /*15*/	{ 0.03125,		1},	// 用户1/32倍速全量，实际1倍速全量
    /*16*/	{ -0.5,       252},	// 用户1/2倍速全量回退，实际1倍速全量回退
    /*17*/	{ -0.25,      252},	// 用户1/4倍速全量回退，实际1倍速全量回退
};

//将整数转化为字符串
std::string CToolsHelp::Int2Str ( int iValue, bool bHex /*= false*/  )
{
    char szTmp[STR_LENGTH_SIZE];
#if VOS_APP_OS == VOS_OS_WIN32
	if ( bHex )
    {
        (void)_itoa_s( iValue, szTmp, INT_FORMAT_X );
    }
    else
    {
        (void)_itoa_s( iValue, szTmp, INT_FORMAT_D );
    }
#elif VOS_APP_OS == VOS_OS_LINUX //modi by zkf71716 at 2012-8-17
	if(bHex)
	{
		(void)sprintf(szTmp,"%x",iValue);
	}
	else
	{
		if(0 > ItoaEX(iValue,szTmp))
		{
			return "";
		}
	}
#endif
    return std::string( szTmp );
}

std::string CToolsHelp::Long2Str ( long lValue, bool bHex)
{
	char szTmp[STR_LENGTH_SIZE];
#if VOS_APP_OS == VOS_OS_WIN32
	if ( bHex )
	{
		(void)_ltoa_s( lValue, szTmp, INT_FORMAT_X );
	}
	else
	{
		(void)_ltoa_s( lValue, szTmp, INT_FORMAT_D );
	}
#elif VOS_APP_OS == VOS_OS_LINUX //modi by zkf71716 at 2012-8-17
	if(bHex)
	{
		(void)sprintf(szTmp,"%x",lValue);
	}
	else
	{
		if(0 > ItoaEX(lValue,szTmp))
		{
			return "";
		}
	}
#endif
	return std::string( szTmp );
}

 int CToolsHelp::ItoaEX(int iValue, char *buff )//add by zkf71716 at 2012-8-17
 {
	 const unsigned int radix = 10;
	 char* p;
	 unsigned int a;        //every digit
	 char* b;             //start of the digit char
	 char temp;
	 unsigned int u;
	 if(NULL == buff)  //lint !e830
	 {
		return -1;
	 }
	 p = buff;

	 if (iValue < 0)
	 {
		 *p++ = '-';
		 iValue = 0 - iValue;
	 }
	 u = (unsigned int)iValue;

	 b = p;
	 do
	 {
		 a = u % radix;
		 u /= radix;
		 *p++ = a + '0';//lint !e734
	 } while (u > 0);

     int len = 0;
	 len = (int)(p - buff);
	 *p-- = 0;
	 //swap
	 do
	 {
		 temp = *p;
		 *p = *b;
		 *b = temp;
		 --p;
		 ++b;

	 } while (b < p);

	 return len;
 }

// 将整数转化为字符串
int CToolsHelp::StrToInt( const std::string& strSrc, bool bHex /*= false*/ )
{
     int iRet = 0;	

     sscanf( strSrc.c_str(), bHex ? STR_FORMAT_X : STR_FORMAT_D, &iRet);

     return iRet;
}

std::string CToolsHelp::GetSCCHead()
{
	return "";
}
int CToolsHelp::ResultCodeToErrorCode( int iResultCode )
{
	//如果是正数就转成负数
	if (iResultCode > 0)
	{
		int iErrorCode = ~iResultCode;//lint !e502
		iErrorCode = iErrorCode + 1;

		return iErrorCode;
	}

	return iResultCode;
}

bool CToolsHelp::IsNum( const std::string &str )
{
    std::string::const_iterator it = str.begin();
    for (; it != str.end(); ++it)
    {
        if(!isdigit(*it))
        {
            return false;
        }
    }
    return true;
}

/**************************************************************************
* name       : TranslateToSer
* description: 本地与服务器之间的数据翻译
* input      : iNum		待翻译的数
*			   pscAgree	含肯定词的字符串
			   pscDisAgree 含否定词的字符串
* output     : NA
* return     : std::string
* remark     : NA
**************************************************************************/
const char*  CToolsHelp::TranslateToSer(const char *pscAgree, const char *pscDisAgree, int &iNum)
{
		if( 0 > iNum || iNum > 1)
		{
			return "";
		}
		if(iNum)
		{
			return pscAgree;
		}
		else
		{
			return pscDisAgree;
		}
}//lint !e1764

/**************************************************************************
* name       : TranslateToSer
* description: 本地与服务器之间的数据翻译
* input      : pscTemp		待翻译的串
	output     : iNum 翻译后的数
* return     : 成功返回翻译后的整形数
* remark     : NA
**************************************************************************/
int CToolsHelp::TranslateFromSer(const char *pscTemp, int &iNum)
{
	if(NULL == pscTemp)
	{
		return 0;
	}
	if(!strcmp("ON",pscTemp))
	{
		iNum = 1;
	}
	if(!strcmp("OFF",pscTemp))
	{
		iNum = 0;
	}
	return 0;
}

int CToolsHelp::GetHostName(char* pHostName)
{
	if (NULL == pHostName)
	{
		return 1;
	}
    //初始化:如果不初始化，以下代码将无法执行
    WSAData data;
    if(WSAStartup(MAKEWORD(1,1),&data)!=0)
    {
        return 1;
    }

	char host[255] = {0};
    //获取主机名:也可以使用GetComputerName()这个函数
    if(gethostname(host,sizeof(host))==SOCKET_ERROR)
    {
        WSACleanup(); 
        return 1;
    }
	if (!Memcpy(pHostName, 255, host, strlen(host)))
	{
		return 1;
	}
	
    WSACleanup();          
    return 0;    
}

// 获取计算机当前登录用户名
int CToolsHelp::GetCurrentUserName(char* pCurrentUserName)
{
	if (NULL == pCurrentUserName)
	{
		return 1;
	}
	DWORD size= 255;  // 需初始化值
	#ifdef UNICODE
	    wchar_t name[255] = {0};
	#else
	    char name[255] = {0};
	#endif // !UNICODE
	GetUserName(name,&size);
	if (!Memcpy(pCurrentUserName, 255,name, size))
	{
		return 1;
	}
	
	return 0;
}

// 获取程序所在目录的父目录
int CToolsHelp::get_exe_parent_dir(char *buf)
{
#ifdef WIN32
    const char separator = '\\';
#else
    const char separator = '/';
#endif

    //去掉程序名
    for(int i = 0; i < 2; ++i) //循环2次: 第1次找到程序所在目录，第2次找到程序所在目录的父目录
    {
        char *pch = strrchr(buf, separator);
        if (NULL == pch)
        {
            printf("Program no directory.\n");
            return IVS_FAIL;
        }
    
        if (pch == buf)
        {
            printf("Program in root directory.\n");
            return IVS_FAIL;
        }

        *pch = '\0';
        //pch = NULL;
    }
    return IVS_SUCCEED;
}

// 获取主目录
int CToolsHelp::get_home_directory(std::string& dir)
{
	int nRet;
	char buf[1024] = {0};
#ifdef WIN32
	    //int nRet = ::GetCurrentDirectoryA(sizeof(buf) - 1, buf);
	    //printf("GetCurrentDirectory: %s\n", buf);

        nRet = GetModuleFileNameA(NULL, buf, sizeof(buf) - 1);//lint !e713
	    if (0 == nRet)
	    {
	        printf("Can't get directory in which process is.\n");
	        return IVS_FAIL;
	    }
#else
		/*
		linux通过函数或readlink("/proc/pid/exe", buf, buf_size)来获取应用程序的绝对路径（含程序名）
		*/
		readlink("/proc/self/exe", buf, sizeof(buf) - 1);
#endif

        nRet = get_exe_parent_dir(buf);
		if (IVS_SUCCEED != nRet)
		{
			return nRet;
		}

		dir = buf;

	return IVS_SUCCEED;
}

// 获取config目录
int CToolsHelp::get_config_directory(std::string& dir)
{
    int nRet;
	char buf[1024] = {0};
#ifdef WIN32
    const char separator = '\\';
    nRet = GetModuleFileNameA(NULL, buf, sizeof(buf) - 1);//lint !e713
	if (0 == nRet)
	{
	    printf("Can't get directory in which process is.\n");
	    return IVS_FAIL;
	}
#else
	const char separator = '/';
	nRet = GetModuleFilePath("libIVS_SDK.so", buf, sizeof(buf));
	if (IVS_SUCCEED != nRet)
	{
		BP_RUN_LOG_ERR(nRet,"GetModuleFileName Failed ", "failed.");
		return nRet;
	}
	strcat(buf, "/");//默认末尾不带'/'
#endif
    //去掉程序名
    char *pch = strrchr(buf, separator);
    if (NULL == pch)
    {
        printf("Program no directory.\n");
        return IVS_FAIL;
    }
    if (pch == buf)
    {
        printf("Program in root directory.\n");
        return IVS_FAIL;
    }
    *pch = '\0';
	dir = buf;

#ifdef WIN32
    dir += "\\config";
#else
    dir += "/config";
#endif
    return IVS_SUCCEED;
}

void CToolsHelp::LTrim(std::string& strSrc )
{
	if ( !strSrc.empty() )
	{
		std::string::const_iterator iter = strSrc.begin();
		do {} while(strSrc.end() != iter && isspace(*(iter++)));

		size_t len = (static_cast<unsigned int>(iter - strSrc.begin()) - 1);
		if ( iter == strSrc.end() && isspace(*(--iter)) )
		{
			len = std::string::npos;
		}

		// 去除前空格符
		strSrc.erase(0, len);
	}
}

void CToolsHelp::RTrim(std::string& strSrc )
{
	if ( !strSrc.empty() )
	{
		std::string::reverse_iterator iter = strSrc.rbegin();
		do {} while(strSrc.rend() != iter && isspace(*(iter++)));

		size_t pos = (static_cast<unsigned int>(strSrc.rend() - iter) + 1);
		if ( iter == strSrc.rend() && isspace(*(--iter)) )
		{
			pos = 0;
		}

		// 去除后空格符
		strSrc.erase(pos);
	}
}

std::string CToolsHelp::Trim(std::string& strSrc )
{
	LTrim(strSrc);
	RTrim(strSrc);
	return strSrc;
}

wchar_t* CToolsHelp::ANSIToUnicode(const char* str)
{
	unsigned int textlen = 0;
	wchar_t * result = NULL;
	textlen = static_cast<IVS_UINT32>(MultiByteToWideChar( CP_ACP, 0, str,-1, NULL,0 ));
	try
	{
		result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
        if (NULL == result)
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memory is not enough", "NA");
            return NULL;
        }
	}
	catch (...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memory is not enough", "NA");
		//result = NULL;
		return NULL;
	}
	
	memset(result,0,(textlen+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0,str,-1,(LPWSTR)result,textlen );//lint !e713
	return result;
}

char* CToolsHelp::UnicodeToUTF8( const wchar_t* str )
{
	char* result = NULL;
	unsigned int textlen = 0;
	textlen = static_cast<IVS_UINT32>(WideCharToMultiByte( CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL ));
	try
	{
		result =(char *)malloc((textlen+1)*sizeof(char));
        if (NULL == result)
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memory is not enough", "NA");
            return NULL;
        }
	}
	catch (...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memory is not enough", "NA");
		//result = NULL;
		return NULL;
	}
	
	memset(result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( CP_UTF8, 0, str, -1, result, textlen, NULL, NULL );//lint !e713
	return result;
}

char* CToolsHelp::UnicodeToANSI( const wchar_t* str )
{
	char* result = NULL;
	unsigned int textlen = 0;
	textlen = static_cast<IVS_UINT32>(WideCharToMultiByte( CP_ACP, 0, str, -1, NULL, 0, NULL, NULL ));
	try
	{
		result =(char *)malloc((textlen+1)*sizeof(char));
        if (NULL == result)
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memory is not enough", "NA");
            return NULL;
        }
	}
	catch (...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memory is not enough", "NA");
		//result = NULL;
		return NULL;
	}
	
	memset( result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( CP_ACP, 0, str, -1, result, textlen, NULL, NULL );//lint !e713
	return result;
}

wchar_t* CToolsHelp::UTF8ToUnicode( const char* str )
{
	unsigned int textlen = 0;
	wchar_t * result = NULL;
	textlen = static_cast<IVS_UINT32>(MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 ));
	try
	{
		result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
        if (NULL == result)
        {
            BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memory is not enough", "NA");
            return NULL;
        }
	}
	catch (...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memory is not enough", "NA");
		//result = NULL;
		return NULL;
	}
	
	memset(result,0,(textlen+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen );//lint !e713
	return result;
}

char* CToolsHelp::UTF8ToANSI(const char* str)
{
#ifdef WIN32
	wchar_t* unicode = UTF8ToUnicode(str);
	if (NULL == unicode)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "UTF8 To Unicode error", "NA");
		return NULL;
	}
	char* ansi = UnicodeToANSI(unicode);
	free(unicode);
	unicode = NULL;
	return ansi;
#else
	BP_RUN_LOG_INF("UTF8ToANSI", "UTF8ToANSI str = %s", str);
	int outlen = strlen(str); ; 
	char* pout = (char*)malloc(sizeof(char)*(outlen+1));
	strcpy(pout,str);
	BP_RUN_LOG_INF("UTF8ToANSI", "UTF8ToANSI str = %s", pout);
	*(pout+outlen)=0;
	return pout;

#endif
}

char* CToolsHelp::ANSIToUTF8(const char* str)
{

#ifdef WIN32
	wchar_t* unicode = ANSIToUnicode(str);
	if (NULL == unicode)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "ANSI To Unicode error", "NA");
		return NULL;
	}
	char* utf8 = UnicodeToUTF8(unicode);
	free(unicode);
	unicode = NULL;
	return utf8;
#else
	char* result = NULL;
	if (NULL == str)
	{
		result = (char*)malloc(sizeof(char));
		memset(result, 0, sizeof(char));
		return result;
	}
	char* pin = const_cast<char*>(str); 
	size_t inlen = strlen(pin); 
	size_t outlen = 2*inlen; 
	char* pout = (char*)malloc(sizeof(char)*outlen);
	if (NULL == pout)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "memory is not enough", "NA");
		return NULL;
	}
	memset(pout, 0, outlen);
	result = pout;

	iconv_t cd = iconv_open("UTF-8", "GB2312"); 
	if((iconv_t)-1 == cd) 
	{ 
		BP_RUN_LOG_ERR(IVS_FAIL,"iconv_open failed", "NA");
		perror("iconv_open error"); 
		free(pout);
		return NULL; 
	} 

	iconv(cd, NULL, NULL, NULL, NULL);
	size_t res = iconv(cd, &pin, &inlen, &pout, &outlen); 
	if((size_t)-1 == res) 
	{ 
		BP_RUN_LOG_ERR(IVS_FAIL,"iconv failed", "NA");
		perror("iconv error"); 
		iconv_close(cd); 
		free(result);
		return NULL;    
	} 
	iconv_close(cd); 
	return result;
#endif
}

int CToolsHelp::SplitCameraDomainCode(const std::string& strCameraDomainCode, 
                                      std::string& strCameraCode, 
                                      std::string& strDomainCode)
{
    if (strCameraDomainCode.empty())
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"CToolsHelp", "strCameraDomainCode is empty.");
        return IVS_PARA_INVALID;
    }

    unsigned int uiPos = strCameraDomainCode.rfind("#");
    if (std::string::npos == uiPos || 0 == uiPos)
    {
        strCameraCode = strCameraDomainCode;// 兼容没有#拼装的情况;
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"CToolsHelp", "strCameraDomainCode(%s) invalid.", strCameraDomainCode.c_str());
        return IVS_SUCCEED;
    }

    strCameraCode = strCameraDomainCode.substr(0, uiPos);

    if (uiPos + 1 >= strCameraDomainCode.length())
    {
        BP_RUN_LOG_ERR(IVS_SUCCEED,"CToolsHelp", "SplitCameraDomainCode do not have DomainCode.");
        return IVS_SUCCEED;// 兼容部分不存在域编码的情况;
    }

    strDomainCode = strCameraDomainCode.substr(uiPos + 1, strCameraDomainCode.length());

    return IVS_SUCCEED;
}

// 各个需要重定向请求的数组
unsigned short m_needRedirectReqIDs[7] = {NSS_GET_URL_REQ,NSS_GET_RECORD_LIST_REQ,NSS_PTZ_CONTROL_REQ,
											NSS_CLUSTER_STAT_CAMERA_TRACE_REQ,NSS_CLUSTER_GET_CAMERA_TRACE_REQ,
											NSS_GET_VOICE_INFO_REQ,NSS_GET_CAMERA_CFG_REQ};

bool CToolsHelp::IsNeedReDirect(unsigned short usReqID)
{
    bool bRet = false;
 
	// 建立映射表，仅存储需要定向的请求。参考MAU
	int iSize = sizeof(m_needRedirectReqIDs) / sizeof(unsigned short);
	for ( int i = 0; i < iSize; i++ )
	{
		if ( m_needRedirectReqIDs[i] == usReqID )
		{
			bRet = true;
			break;
		}
	}

    return bRet;
}

TiXmlElement* CToolsHelp::FindElementByPath(TiXmlNode* pParentEle,std::string strPath)
{
	TiXmlNode* pChildElem = NULL;
	if(NULL != pParentEle)
	{
		std::string tmpPath = CToolsHelp::Trim(strPath);

		// 查找分隔符
		std::string::size_type siPosIndex = tmpPath.find('/');

		// 如果找到的第一个字符就是 / 则跳过，继续寻找下一个
		while (0 == siPosIndex)
		{
			tmpPath = tmpPath.substr(1);
			tmpPath = CToolsHelp::Trim(tmpPath);
			siPosIndex = tmpPath.find('/');
		}
		// 如果是空的则返回本身
		if (tmpPath.empty())
		{
			pChildElem = pParentEle;
		}
		else if(std::string::npos == siPosIndex )
		{
			pChildElem = pParentEle->FirstChildElement(tmpPath.c_str());
		}
		else
		{
			std::string strElemName = tmpPath.substr(0,siPosIndex);
			std::string strChildPath = tmpPath.substr(siPosIndex + 1);

			strChildPath = CToolsHelp::Trim(strChildPath);

			pChildElem = pParentEle->FirstChildElement(strElemName.c_str());
			if(NULL != pChildElem && !strChildPath.empty())
			{
				pChildElem = CToolsHelp::FindElementByPath(pChildElem,strChildPath); //lint !e1036
			}
		}
	}
	return (NULL == pChildElem) ?NULL: pChildElem->ToElement();
}

bool CToolsHelp::IsSpeedVaild(float fScale)
{
    bool bRet = false;

    float fScaleAbs = abs(fScale);

    if ((fScaleAbs - 32 >= - EPSINON) && (fScaleAbs - 32 <= EPSINON))
    {
        bRet = true;
    }
    else if ((fScaleAbs - 16 >= - EPSINON) && (fScaleAbs - 16 <= EPSINON))
    {
        bRet = true;
    }
    else if ((fScaleAbs - 8 >= - EPSINON) && (fScaleAbs - 8 <= EPSINON))
    {
        bRet = true;
    }
    else if ((fScaleAbs - 4 >= - EPSINON) && (fScaleAbs - 4 <= EPSINON))
    {
        bRet = true;
    }
    else if ((fScaleAbs - 2 >= - EPSINON) && (fScaleAbs - 2 <= EPSINON))
    {
        bRet = true;
    }
    else if ((fScaleAbs - 1 >= - EPSINON) && (fScaleAbs - 1 <= EPSINON))
    {
        bRet = true;
    }
	else if ((fScaleAbs - 0.5 >= - EPSINON) && (fScaleAbs - 0.5 <= EPSINON))
	{
		bRet = true;
	}
	else if ((fScaleAbs - 0.25 >= - EPSINON) && (fScaleAbs - 0.25 <= EPSINON))
	{
		bRet = true;
	}
	else if ((fScaleAbs - 0.125 >= - EPSINON) && (fScaleAbs - 0.125 <= EPSINON))
	{
		bRet = true;
	}
	else if ((fScaleAbs - 0.0625 >= - EPSINON) && (fScaleAbs - 0.0625 <= EPSINON))
	{
		bRet = true;
	}
	else if ((fScaleAbs - 0.03125 >= - EPSINON) && (fScaleAbs - 0.03125 <= EPSINON))
	{
		bRet = true;
	}

    return bRet;
}

// 将用户播放速度转换为向服务器发送的播放速度;
bool CToolsHelp::GetSendScale(IVS_SERVICE_TYPE enServiceType, float fUserScale, float& fSendScale)
{
    bool bRet = false;
    int iSize = sizeof(s_SpeedNames) / sizeof(SEND_SPEED);

    for ( int i = 0; i < iSize; i++ )
    {
        if ( s_SpeedNames[i].fUserScale == fUserScale )
        {
            fSendScale = s_SpeedNames[i].fSendScale;
            bRet = true;
            break;
        }
    }

	// 针对前端回放和前端下载做特殊处理
	// 前端回放和前端下载4倍速控速时还是4倍速发流
	unsigned int uiUserScale = (unsigned int)fUserScale;
	if ((SERVICE_TYPE_PU_PLAYBACK == enServiceType || SERVICE_TYPE_PU_DOWNLOAD == enServiceType)
		&& 4 == uiUserScale)
	{
		fSendScale = 4;
	}

    return bRet;
}

// 封装后的Memcpy函数
bool CToolsHelp::Memcpy(void* pDst,unsigned int uiMaxLen,const void* pSrc, unsigned int uiCopyLen)
{
    bool bRet = false;
    if (NULL == pDst)
    {
        return bRet;
    }
    
    if (uiCopyLen > uiMaxLen)
    {
        return bRet;
    }
    else
    {
        memcpy(pDst, pSrc, uiCopyLen);
        bRet = true;
    }
    return bRet;
}

// 封装后的strncpy函数,拷贝成功后自动在字符串结尾添加'\0’
bool CToolsHelp::Strncpy(char* pDst, unsigned int uiMaxLen, const char* pSrc, unsigned int uiCopyLen)
{
    if (NULL == pSrc)
    {
        return false;
    }

    if (uiCopyLen >= uiMaxLen)
    {
        return false;
    }
    else
    {
        strncpy(pDst, pSrc, uiCopyLen);
        pDst[uiCopyLen] = '\0';
        return true;
    }
}

// 封装后的snprintf函数，拷贝成功后自动在字符串结尾添加'\0’
bool CToolsHelp::Snprintf(char* pBuffer, unsigned int uiMaxLen, const char* pFormat, ...)
{
    va_list ap;
    va_start (ap, pFormat);
    int iResult = ::vsnprintf (pBuffer, uiMaxLen - 1, pFormat, ap);//lint !e838
    va_end (ap);
    if ( -1 == iResult )
    {
        return false;
    }
    else
    {
        pBuffer[iResult] = '\0';
        return true;
    }
}//lint !e838 !e438

// 封装后的strncat函数
bool CToolsHelp::Strncat(char* pDst, unsigned int uiMaxLen,const char* pSrc)
{
    if (uiMaxLen < (strlen(pDst) + strlen(pSrc) + 1))
    {
        return false;
    }
    else
    {
        strncat( pDst, pSrc, strlen(pSrc) );
        return true;
    }
}

// 替换所有指定的字符串例如：将"12212"这个字符串的所有"12"都替换成"21"，21221
std::string& CToolsHelp::replace_all_distinct(std::string& str,const std::string& old_value,const std::string& new_value)    
{    
    for(std::string::size_type   pos(0);   pos!=std::string::npos;   pos+=new_value.length()) {    
        if((pos=str.find(old_value,pos))!=std::string::npos)    
            str.replace(pos,old_value.length(),new_value);    
        else   
            break;    
    }
    return str;    
} 

// 替换所有指定的字符串例如：将"12212"这个字符串的所有"12"都替换成"21"，22211
std::string& CToolsHelp::replace_all(std::string& str,const std::string& old_value,const std::string& new_value)   
{    
    while(true) //lint !e716
    {    
        std::string::size_type pos(0);    
        if((pos=str.find(old_value)) != std::string::npos)    
            str.replace(pos,old_value.length(),new_value);    
        else 
            break;    
    }    
    return str;    
}
