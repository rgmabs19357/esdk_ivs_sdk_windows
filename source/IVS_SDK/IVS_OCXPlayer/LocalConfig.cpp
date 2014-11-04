#include "stdafx.h"
#include "LocalConfig.h"
#include "SDKConfig.h"
#include "ToolsHelp.h"
#include "zip.h"
#include "unzip.h"
#include "IVS_SDK.h"

std::string CLocalConfig::m_strLocalLoginName;
std::string CLocalConfig::m_strLocalstrIP;

CLocalConfig::CLocalConfig()
	: m_iSessionID(0)
	, m_bBackUserData(false)
{
	m_pMutexLocalConfigFileLock = VOS_CreateMutex();
}
CLocalConfig::~CLocalConfig()
{
	m_iSessionID = 0;
	m_bBackUserData = false;
	VOS_DestroyMutex(m_pMutexLocalConfigFileLock);
	m_pMutexLocalConfigFileLock = NULL;
}

// 检查本地文件是否存在，如果不存在就根据模板生成一个,本地文件存放在app data/ivs/ip/username/localconfig.xml
std::string CLocalConfig::MakeSureLocalConfigFileExist(const char* pFileName) const
{
	(void)VOS_MutexLock(m_pMutexLocalConfigFileLock);

	// 检查目录是否存在
	std::string strPath = MakeSureLocalConfigDirExist();
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"create dir failed","NA");
		(void)VOS_MutexUnlock(m_pMutexLocalConfigFileLock);
		return "";
	}

	// 判断文件是否存在，不存在就创建
	strPath = strPath + pFileName;
	if(_access(strPath.c_str(),0) != 0)
	{
		bool bRet = false;
		
		if (!strcmp(pFileName, LOCAL_CONFING_FILE_NAME))
		{
			bRet = CreateLocalConfigXML(strPath.c_str());
		}
		else if (!strcmp(pFileName, DEVICE_KEYWORD_FILE_NAME))
		{
			bRet = CreateDeviceKeyWordXML(strPath.c_str());
		}
		else if (!strcmp(pFileName, CUSTOM_LAYOUT_CONFIG_FILE_NAME))
		{
			bRet = CreateCustomLayoutXML(strPath.c_str());
		}
        else if (!strcmp(pFileName, FAVORITE_CONFIG_FILE_NAME))
        {
			bRet = CreateFavoriteConfigXML(strPath.c_str());
        }
		else if (!strcmp(pFileName, SCENARIO_FAVORITE_FILE_NAME))
		{
			bRet = CreateScenarioFavoriteXML(strPath.c_str());
		}
		else if (!strcmp(pFileName,TVWALL_SCENARIO_FAVORITE_FILE_NAME))
		{
			bRet = CreateTVWallScenarioFavoriteXML(strPath.c_str());
		}
		else if (!strcmp(pFileName, CAMERA_SWITCH_FILE_NAME))
		{
			bRet = CreateCameraSwitchXML(strPath.c_str());
		}
		else if (!strcmp(pFileName, CAMERA_ALIAS_FILE_NAME))
		{
			bRet = CreateCameraAliasXML(strPath.c_str());
		}
		else if (!strcmp(pFileName, CAMERRA_SWITCH_PLAN_FINE_NAME))
		{
			bRet = CreateCameraSwitchPlanXML(strPath.c_str());
		}
		else if (!strcmp(pFileName, CAMERRA_SWITCH_PLAN_TVWALL_FILE_NAME))
		{
			bRet = CreateCameraSwitchPlanXMLTVWall(strPath.c_str());
		}
        else if (!strcmp(pFileName, NET_KEYBOARD_INFO_FILE_NAME))
        {
            bRet = CreateNetKeyBoardInfoXML(strPath.c_str());
        }
        else if (!strcmp(pFileName, BROADCAST_FILELIST_FILE_NAME))
        {
            bRet = CreateBroadcastFileListXML(strPath.c_str());
        }
		else if (!strcmp(pFileName, BROADCAST_CAMERALIST_CAMERA_CODE))
		{
			bRet = CreateBroadcastCameraListXML(strPath.c_str());
		}
		else
		{
			// 其他模板
		}

		if (!bRet)
		{
			BP_RUN_LOG_ERR(IVS_FAIL,"create file failed","file name =%s", pFileName);
			(void)VOS_MutexUnlock(m_pMutexLocalConfigFileLock);
			return "";
		}
	}
	(void)VOS_MutexUnlock(m_pMutexLocalConfigFileLock);

	return strPath; //lint !e1036
}

// 检查本地配置的文件夹，路径名 local data/IVS/IP/loginname，如果不存在就创建这个
std::string CLocalConfig::MakeSureLocalConfigDirExist() const
{
	// 获取appdat目录,如果不存在就创建
	char szPath[MAX_PATH]; 
	memset(szPath,0x0,MAX_PATH);
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, TRUE); 
	// 判断指定的路径是否存在，不存在就创建文件夹
	std::string strPath = std::string(szPath) + DIR_SEPARATOR + SOFTWARE_NAME + DIR_SEPARATOR + CLocalConfig::m_strLocalstrIP + DIR_SEPARATOR + CLocalConfig::m_strLocalLoginName + DIR_SEPARATOR; //lint !e1036
	if (!MakeSureDirectoryPathExists(strPath.c_str()))
	{
		strPath = "";
	}
	
	return strPath; //lint !e1036
}

// 检查本地文件是否存在，不存在返回空路径，存在返回文件路径
std::string CLocalConfig::CheckLocalConfigFileExist(const char* pFileName) const
{
	// 获取appdat目录
	char szPath[MAX_PATH]; 
	memset(szPath,0x0,MAX_PATH);
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE);
	std::string strPath = std::string(szPath) + DIR_SEPARATOR + SOFTWARE_NAME + DIR_SEPARATOR + CLocalConfig::m_strLocalstrIP + DIR_SEPARATOR + CLocalConfig::m_strLocalLoginName + DIR_SEPARATOR; //lint !e1036
	strPath += pFileName;
	// 检查文件是否存在，不存在就返回空
	if(_access(strPath.c_str(),0) != 0)
	{
		strPath = "";
	}
	return strPath; //lint !e1036
}

// 创建localconfig.xml的模板文件
bool CLocalConfig::CreateLocalConfigXML(const char* pFilePath) const
{
	std::string strXml = LOCAL_CONFIG_XML_TEMPLATE;
	return CreateXML(pFilePath,strXml.c_str());
}

// 创建devicekeyword.xml的模板文件
bool CLocalConfig::CreateDeviceKeyWordXML(const char* pFilePath) const
{
	std::string strXml = DEVICE_KEYWORD_XML_TEMPLATE;
	return CreateXML(pFilePath,strXml.c_str());
}

// 创建customlayout.xml的模板文件
bool CLocalConfig::CreateCustomLayoutXML(const char* pFilePath) const
{
	std::string strXml = CUSTOM_LAYOUT_CONFIG_XML_TEMPLATE;
	return CreateXML(pFilePath,strXml.c_str());
}

// 创建FavoriteConfig.xml的模板
bool CLocalConfig::CreateFavoriteConfigXML(const char* pFilePath) const
{
    std::string strXml = FAVORITE_CONFIG_XML_TEMPLATE;
    return CreateXML(pFilePath,strXml.c_str());
}

// 创建FavoriteConfig.xml的模板
bool CLocalConfig::CreateScenarioFavoriteXML(const char* pFilePath) const
{
	std::string strXml = SCENARIO_FAVORITE_XML_TEMPLATE;
	return CreateXML(pFilePath,strXml.c_str());
}

// 创建TVWallFavoriteConfig.xml的模板
bool CLocalConfig::CreateTVWallScenarioFavoriteXML(const char* pFilePath) const
{
	std::string strXml = SCENARIO_FAVORITE_XML_TEMPLATE;
	return CreateXML(pFilePath,strXml.c_str());
}

// 创建CameraSwitch.xml的模板
bool CLocalConfig::CreateCameraSwitchXML(const char* pFilePath) const
{
	std::string strXml = CAMERA_SWITCH_XML_TEMPLATE;
	return CreateXML(pFilePath,strXml.c_str());
}

// 创建CameraSwitch.xml的模板
bool CLocalConfig::CreateCameraAliasXML(const char* pFilePath) const
{
	std::string strXml = CAMERA_ALIAS_XML_TEMPLATE;
	return CreateXML(pFilePath,strXml.c_str());
}

bool CLocalConfig::CreateCameraSwitchPlanXML(const char* pFilePath) const
{
	std::string strXml = CAMERA_SWITCH_PLAN_XML_TEMPLATE;
	return CreateXML(pFilePath,strXml.c_str());
}

bool CLocalConfig::CreateCameraSwitchPlanXMLTVWall(const char* pFilePath) const
{
	std::string strXml = CAMERA_SWITCH_PLAN_TVWALL_XML_TEMPLATE;
	return CreateXML(pFilePath,strXml.c_str());
}

// 创建网络键盘信息的模板
bool CLocalConfig::CreateNetKeyBoardInfoXML(const char* pFilePath) const
{
    std::string strXml = NET_KEYBOARD_INFO_XML_TEMPLATE;
    return CreateXML(pFilePath,strXml.c_str());
}

// 创建广播文件列表的模板
bool CLocalConfig::CreateBroadcastFileListXML(const char* pFilePath) const
{
    std::string strXml = BROADCAST_FILELIST_XML_TEMPLATE;
    return CreateXML(pFilePath,strXml.c_str());
}

// 创建语音广播摄像机列表的模板
bool CLocalConfig::CreateBroadcastCameraListXML(const char* pFilePath) const
{
	std::string strXml = BROADCAST_CAMERALIST_XML_TEMPLATE;
	return CreateXML(pFilePath,strXml.c_str());
}

// 在指定路径创建一个XML(本地)
bool CLocalConfig::CreateXML(const char* pFilePath, const char* pStream) const
{
	bool bRet = false;
	do 
	{
		TiXmlDocument pDoc;
		pDoc.Parse(pStream);
		//pDoc.Parse(pStream,NULL, TIXML_ENCODING_LEGACY)
		if (!pDoc.SaveFile(pFilePath))
		{
			BP_RUN_LOG_ERR(IVS_FAIL,"SaveFile failed, file path is:","%s", pFilePath);
			bRet = false;
			break;
		}
		bRet = true;

	} while (0); //lint !e717

	return bRet;
}

void CLocalConfig::SetLoginName(const std::string& strLoginName)
{
	CLocalConfig::m_strLocalLoginName = strLoginName;
}

void CLocalConfig::SetIP(const std::string& strIP)
{
	CLocalConfig::m_strLocalstrIP = strIP;
}

// 获取日志配置文件的目录
IVS_INT32 CLocalConfig::GetConfigDir(std::string& dir)
{
	int nRet = 0;
	char buf[1024] = {0};
	nRet = (int)GetModuleFileName(NULL, buf, sizeof(buf) - 1);
	if(0 == nRet)
	{
		return -1;
	}

	const char separator = '\\';

	char *pch = NULL;
	//去掉程序名
	for(int i = 0; i < 2; ++i) //循环2次: 第1次找到程序所在目录，第2次找到程序所在目录的父目录
	{
		pch = strrchr(buf, separator);
		if((NULL == pch) || (pch == buf))
		{
			return -1;
		}
		*pch = '\0';
		pch = NULL;
	}

	dir = buf;
	dir += "\\config";
	return 0;
}

bool CLocalConfig::SetCDATA2XML(const char* pFileName, const char* pElement, const char* pValue) const
{
	// 检查本地配置的文件是否存在，不存在就创建文件，并返回路径
	std::string strPath = MakeSureLocalConfigFileExist(pFileName);
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"create file failed","NA");
		return false;
	}

	// 读取XML，保存参数
	CXml xml;
	if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		// 如果加载失败，重新用模板生成这个XML
		if (!CreateLocalConfigXML(strPath.c_str()))
		{
			BP_RUN_LOG_ERR(IVS_FAIL,"create xml failed","NA");
			return false;
		}
		// 如果再失败。。
		if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			BP_RUN_LOG_ERR(IVS_FAIL,"load xml failed","NA");
			return false;
		}
	}
    if (NULL == pElement)
    {
        (void)xml.Parse(pValue);
    }
	else if (!xml.SetElemValueCDATAEx(pElement,pValue))
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"set elem value failed","NA");
		return false;
	}

	if (!xml.Save())
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"save xml failed","NA");
		return false;
	}
	
	return true;
}

std::string CLocalConfig::GetCDATA2XML(const char* pFileName, const char* pElement) const
{
	std::string strResult;
	do 
	{
		// 获取appdat目录
		std::string strPath = CheckLocalConfigFileExist(pFileName);
		if (strPath.empty())
		{
			BP_RUN_LOG_ERR(IVS_FAIL,"file not exist","NA");
			strResult = "";
			break;
		}

		CXml xml;
		if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			BP_RUN_LOG_ERR(IVS_FAIL,"load file failed","NA");
			strResult = "";
			break;
		}
        if (NULL == pElement)
        {
            IVS_UINT32 uiLen = 0;
            strResult = xml.GetXMLStream(uiLen);
        }
        else
        {
			const char* pV = xml.GetElemValueEx(pElement);
			if (NULL != pV)
			{
				strResult = pV;
			}
        }
	} while (0); //lint !e717
	
	return strResult; //lint !e1036
}

// 将布局数据的XML转换成可绘图的点,xml点格式 <Coord>0,0,1,1</Coord>
IVS_INT32 CLocalConfig::ParseLayoutXML2List(const char* layoutXML, std::list<CRect> &liRectList, IVS_UINT32& xNum, IVS_UINT32& yNum) const
{
	int iRet = IVS_SUCCEED;
	do 
	{
		CHECK_POINTER(layoutXML, IVS_FAIL);
		// 解析XML
		CXml xml;
		if (!xml.Parse(layoutXML))
		{
			BP_RUN_LOG_ERR(IVS_FAIL,"parse xml failed","NA");
			iRet = IVS_FAIL;
			break;
		}

		if (!xml.FindElemEx("CONTENT/WndList"))
		{
			BP_RUN_LOG_ERR(IVS_FAIL,"find element failed","NA");
			iRet = IVS_FAIL;
			break;
		}
		// 判断有木有这个节点，没有就是布局没值
		if (!xml.FindElem("Coord"))
		{
			BP_RUN_LOG_ERR(IVS_FAIL,"no coord","NA");
			return IVS_FAIL;
		}
		xNum = 0;
        yNum = 0;
		const char* strCoord = NULL;
		do 
		{
			CRect rect;
			strCoord = xml.GetElemValue();
			CHECK_POINTER(strCoord, IVS_FAIL);
			// 遍历四次，获取坐标，如果有坐标错误直接返回失败
			char* pNext = NULL;
			char* token = strtok_s(const_cast<char*>(strCoord), ",", &pNext);
			CHECK_POINTER(token, IVS_FAIL);
			rect.left = atoi(token);

			token = strtok_s(NULL, ",", &pNext);
			CHECK_POINTER(token, IVS_FAIL);
			rect.top = atoi(token);

			token = strtok_s(NULL, ",", &pNext);
			CHECK_POINTER(token, IVS_FAIL);
			rect.right = atoi(token);

			token = strtok_s(NULL, ",", &pNext);
			CHECK_POINTER(token, IVS_FAIL);
			rect.bottom = atoi(token);

			liRectList.push_back(rect);

            if (rect.right > static_cast<IVS_LONG>(xNum))
            {
                xNum = static_cast<IVS_UINT32>(rect.right);
            }

            if (rect.bottom > static_cast<IVS_LONG>(yNum))
            {
                yNum = static_cast<IVS_UINT32>(rect.bottom);
            }
		} while (xml.NextElem());
	} while (0); //lint !e717

	return iRet;
}

// 读取本地文件，获取指定自定义布局的XML
IVS_UINT32 CLocalConfig::GetLayoutXML( unsigned long ulLayoutID, std::string &strlayoutXML ) const
{
	// 判断文件是否存在，存在就返回路径
	std::string strPath = CheckLocalConfigFileExist(CUSTOM_LAYOUT_CONFIG_FILE_NAME);
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR,"file not exist","NA");
		return IVS_OPEN_FILE_ERROR;
	}

	// 加载XML文件
	TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	CHECK_POINTER(pXmlDoc, IVS_ALLOC_MEMORY_ERROR);
	if (!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get LayoutXML Failed.","Reason:Create LayoutXML xml Failed.");
		return IVS_XML_INVALID;
	}
	
	TiXmlElement* pConnListElem = CToolsHelp::FindElementByPath(pXmlDoc,CUSTOM_LAYOUT_LIST_ELEMENT);
	CHECK_POINTER(pConnListElem, IVS_XML_INVALID);
	// 遍历是存在这个节点数据
	TiXmlElement* pNode = pConnListElem->FirstChildElement(CUSTOM_LAYOUT_NODE);
	
	const char* pCode = NULL;
	while(NULL != pNode)
	{
		pCode = pNode->Attribute(LAYOUT_ATTRIBUTE);
		CHECK_POINTER(pCode, IVS_XML_INVALID);
		// 如果找到一样的code
		if (ulLayoutID == static_cast<unsigned int>(atoi(pCode)))
		{
            strlayoutXML.clear();
            strlayoutXML = pNode->GetText();
			IVS_DELETE(pXmlDoc);
			return IVS_SUCCEED;
		}
		pNode = pNode->NextSiblingElement(CUSTOM_LAYOUT_NODE);
	}

	// 释放掉doc,其他部分也都释放了
	IVS_DELETE(pXmlDoc);
	return IVS_FAIL;
}

// 压缩文件夹中的所有内容到指定目录下
IVS_INT32 CLocalConfig::CompressionDir(const std::string& zipFileName, const std::string& strInPath, const std::string& strOutPath)
{
	std::string strSerachPatten=strInPath + "\\" +"*.*";

	// 创建zip包
	std::string strZipPath = strOutPath;
	strZipPath += "\\";
	strZipPath += zipFileName;

	HZIP hz = CreateZip(strZipPath.c_str(),0);
	// 遍历文件夹下面的所有文件，将文件加入到zip中
	struct _finddata_t fileinfo;
	long handle = _findfirst(strSerachPatten.c_str(),&fileinfo);
	if(handle == -1L)
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"path error","NA");
		return IVS_FAIL;
	}
	// 如果不是 .或者..
	std::string filePath = strInPath + "\\" + fileinfo.name;
	if (!(!strcmp(fileinfo.name,".") || !strcmp(fileinfo.name,"..")))
	{
		ZipAdd(hz, fileinfo.name, filePath.c_str());
	}

	while(!_findnext(handle,&fileinfo))
	{
		// 如果不是 .或者..
		//modify by s00193168:2013-6-28。新增电视墙功能：子程里跑轮巡，此功能需要将TVWallScenarioFavorite.xml不再打包上传至服务器。 
		if (!(!strcmp(fileinfo.name,".") || !strcmp(fileinfo.name,"..")) && strcmp(fileinfo.name,"TVWallScenarioFavorite.xml"))
		{
			filePath = strInPath + "\\" + fileinfo.name;
			ZipAdd(hz, fileinfo.name, filePath.c_str());
		}
	}

	_findclose(handle);

	CloseZip(hz);
	return IVS_SUCCEED;
}

// 解压一个压缩文件到一个指定目录
IVS_INT32 CLocalConfig::DecompressionDir(const std::string& zipFileName,const std::string& zipFilePath, const std::string& strOutPath)
{
	std::string zipFile = zipFilePath;
	//zipFile += "\\";
	zipFile += zipFileName;

	HZIP hz = OpenZip(zipFile.c_str(),0);
	ZIPENTRY ze; 
	GetZipItem(hz, -1, &ze); 
	int numitems = ze.index;
	for (int i=0; i < numitems; i++)
	{ 
		GetZipItem(hz,i,&ze);
		UnzipItem(hz,i,ze.name);
		// 移动文件到指定目录,如果存在就覆盖掉，这个解压出来的文件，默认在程序路径

		std::string moveSrcFile = std::string(".\\") + ze.name;
		std::string moveDstFile = strOutPath + "\\" + ze.name;
		MoveFileEx(moveSrcFile.c_str(),moveDstFile.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED); //add by xiongfeng 移动到不同的卷，则复制文件并删除原来的文件
	}
	CloseZip(hz);
	return IVS_SUCCEED;
}

// 文件自动恢复和开启备份定时器
IVS_INT32 CLocalConfig::StartUserData(IVS_INT32 iSessionID, std::string &strLocalLoginName, std::string &strLocalstrIP)
{
	//开启定时器
	StartLocalConfigTimer();

	m_strLocalLoginName = strLocalLoginName;
	m_strLocalstrIP = strLocalstrIP;
	m_iSessionID = iSessionID;

 	//获取文件路径
	m_strFileNamePath = MakeSureLocalConfigDirExist();

 	//调用sdk恢复接口
 	std::string strName; 
	strName.append(strLocalstrIP).append("_").append(strLocalLoginName).append(".zip");
 	std::string strPathAndName = m_strFileNamePath + strName; //lint !e1036

    BP_RUN_LOG_INF("Start UserData", "m_strLocalLoginName: %s,m_LocalstrIP: %s, m_iSessionID: %d, m_strFileNamePath: %s, strPathAndName: %s", m_strLocalLoginName.c_str(),m_strLocalstrIP.c_str(),m_iSessionID,m_strFileNamePath.c_str(),strPathAndName.c_str());
	IVS_INT32 iRet = IVS_SDK_GetUserData(m_iSessionID, strPathAndName.c_str());
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "get user data fail", "NA");
		return iRet;
	}

	//解压缩文件
	iRet = DecompressionDir(strName, m_strFileNamePath, m_strFileNamePath);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "get user data fail", "NA");
	}

	//删除压缩文件
	if (!DeleteFile(strPathAndName.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_SDK_ERR_DELETEZIPFILE_FAILED, "onTrigger", "delete user zip file fail");
	}

	return iRet;
} //lint !e451 !e1764

// 开启自动备份定时器
void CLocalConfig::StartLocalConfigTimer(void)
{
	m_bBackUserData = true; // 定时器开启标志
	m_LocalConfigTimerInst.init(100);
	m_LocalConfigTimerInst.run();
    // 配置文件中单位为分钟，换算60 * 10 * value 单位为秒
    IVS_UINT32 uiBackUpInterval = 600 * CSDKConfig::instance().GetBackUpInterval();
	try
	{
 		m_LocalConfigTimerInst.registerTimer(this , NULL, uiBackUpInterval, enRepeated);
	}
	catch (...)
	{
		return;
	}
}

// 停止自动备份定时器
void CLocalConfig::StopLocalConfigTimer(void)
{
	if (m_bBackUserData)
	{
		m_bBackUserData = false;
		m_LocalConfigTimerInst.cancelTimer(this);
		m_LocalConfigTimerInst.clearTimer();
		m_LocalConfigTimerInst.exit();
	}
}

// 定时器触发调用入口
void CLocalConfig::onTrigger(void* pArg, ULONGLONG ullScales, TriggerStyle style)
{
	(void)BackUserData();
} //lint !e715

// 备份特定目录下的文件
IVS_INT32 CLocalConfig::BackUserData(void)
{
    // 没开启备份功能，不需要备份用户数据
    if (!m_bBackUserData)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Back UserData", "m_bBackUserData is false, needn't backUserData");
        return IVS_FAIL;
    }
	std::string strName;
	strName.append(CLocalConfig::m_strLocalstrIP).append("_").append(CLocalConfig::m_strLocalLoginName).append(".zip");
	std::string strPathAndName;
	strPathAndName.append(m_strFileNamePath).append(strName);

    BP_RUN_LOG_INF("Back UserData", "strName: %s,m_strFileNamePath: %s, strPathAndName: %s", strName.c_str(),m_strFileNamePath.c_str(),strPathAndName.c_str());
	IVS_INT32 iRet = CompressionDir(strName, m_strFileNamePath, m_strFileNamePath);

	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_SDK_ERR_CREATEZIPFILE_FAILED, "CLocalConfig::onTrigger", "CompressionDir failed");
		return iRet;
	}

	iRet = IVS_SDK_SetUserData(m_iSessionID ,strPathAndName.c_str());

	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"onTrigger", "IVS_SDK_SetUserData fail. iRet = %d", iRet);
	}

	//解压文件夹
	//(void)DecompressionDir(strName, m_strFileNamePath, m_strFileNamePath); 

	//删除文件
	if (!DeleteFile(strPathAndName.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_SDK_ERR_DELETEZIPFILE_FAILED, "onTrigger", "delete user zip file fail");
	}	
	return iRet;
}
