#include "OCXTVWallXMLProcess.h"
#include "ToolsHelp.h"
#include "OCXXmlProcess.h"
#include <ImageHlp.h>
#include "LocalConfig.h"
#include "IVS_Trace.h"

#define TVWALL_FILE_NAME "TVWall.xml"
#define TVWALL_TEMP_SCENARIO "TVSceneTemp.xml"
#define TVWALL_SCENARIO "TVScene.xml"
#define NETKEYBOARD_FILE_NAME "NetKeyBoard.xml"

#define TVWALL_ID_MAX_LENGTH 64		//电视墙ID长度
#define TVWALL_WND_MAX_COUNT 144    //电视墙窗格个数
#define TVWALL_WND_MAX_HOR_COUNT 24 //电视墙水平窗格个数

#define FIND_XML_ELEM_AND_INTO(xmlObj,elemName,errCode) \
{\
	if(!xmlObj.FindElem(elemName))\
	{\
		BP_RUN_LOG_ERR(errCode, "TVWall XML", "Find Element %s Failed", elemName);\
		return errCode;\
	}\
	(void)xmlObj.IntoElem();\
}\

#define FIND_XML_PELEM_AND_INTO(xmlObj,elemName,errCode, pDelete) \
{\
	if(!xmlObj.FindElem(elemName))\
	{\
		BP_RUN_LOG_ERR(errCode, "TVWall XML", "Find Element %s Failed", elemName);\
		try\
		{\
			if(NULL != pDelete)\
			{\
				delete pDelete;\
				pDelete = NULL;\
			}\
		}\
		catch(...)\
		{\
		}\
		return errCode;\
	}\
	(void)xmlObj.IntoElem();\
}\

#define FIND_XML_ELEM_EX(xmlObj,elemName,errCode) \
{\
	if(!xmlObj.FindElemEx(elemName))\
	{\
		BP_RUN_LOG_ERR(errCode, "TVWall XML", "Find Element %s Failed", elemName);\
		return errCode;\
	}\
}\

COCXTVWallXMLProcess::COCXTVWallXMLProcess(void)
{
	m_bSaveinMem = false;
	m_TVWallXmlMutex = VOS_CreateMutex();
}


COCXTVWallXMLProcess::~COCXTVWallXMLProcess(void)
{
	(void)VOS_DestroyMutex(m_TVWallXmlMutex);
	m_TVWallXmlMutex = NULL;
}

COCXTVWallXMLProcess& COCXTVWallXMLProcess::GetInst()
{
	static COCXTVWallXMLProcess Inst;
	return Inst;
}

IVS_VOID COCXTVWallXMLProcess::GetTVWallList(CString &strResult)const
{
	CXml xml;
	bool bSucc = false;

	//获取xml文件全路径名
	std::string strPath = MakeSureTVWallDirExist();
	std::string strPathFile = strPath + TVWALL_FILE_NAME;
	BP_DBG_LOG("COCXTVWallXMLProcess::GetTVWallList, filename=%s", strPathFile.c_str());
	if (!PathFileExists(strPathFile.c_str()))
	{
		(void)xml.AddDeclaration("1.0","UTF-8","");
		(void)xml.AddElem("Content");
		(void)xml.AddChildElem("ResultCode");
		(void)xml.IntoElem();
		(void)xml.SetElemValue("0");
		xml.OutOfElem();
		IVS_UINT32 uiLen = 0;
		const IVS_CHAR* pXml = xml.GetXMLStream(uiLen);
		if(NULL!=pXml)
		{
			strResult = pXml;
		}
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Get TVWall List", "file not exist");
		return;
	}

	if(xml.Load(strPathFile.c_str(), TIXML_ENCODING_LEGACY))
	{
		unsigned int xmlLen = 0;

		try
		{			
			if (xml.FindElemEx("Content"))
			{
				xml.AddElem("ResultCode");
				xml.SetElemValue("0");
				xml.OutOfElem();
			}

			if(xml.FindElemEx("Content/TVWallList"))
			{	  
				//(void)xml.IntoElem();
				while (xml.FindElemValue("TVWall"))
				{		
					(void)xml.IntoElem();	
					if(xml.FindElem("Layout"))
					{
						xml.RemoveElem();
					}
					xml.OutOfElem();
					if(!xml.NextElem())
					{
						xml.OutOfElem();
						break;
					}
				}
			}
		}
		catch (...)
		{
			COCXXmlProcess::GetErrString(strResult, IVS_OPEN_FILE_ERROR);
		}

		const IVS_CHAR* pXmlStream = xml.GetXMLStream(xmlLen);
		if(NULL!=pXmlStream)
		{
			strResult = pXmlStream;
			bSucc = true;
		}
		else
		{
			bSucc = false;
			BP_RUN_LOG_ERR(IVS_FAIL, "TVWall XML", "Get XML Stream Failed");
		}
		CHECK_POINTER_VOID(pXmlStream);
		strResult = pXmlStream;
		bSucc = true;
	}

	if(!bSucc)
	{
		COCXXmlProcess::GetErrString(strResult, IVS_OPEN_FILE_ERROR);
	}
}

IVS_VOID COCXTVWallXMLProcess::GetTVWallByID(IVS_UINT32 uiTVWallID, CString& strResult)const
{
	//获取xml文件全路径名


	std::string strPath = MakeSureTVWallDirExist();
	if(""==strPath)
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Get TVWall By Id", "Dir not exist");
		COCXXmlProcess::GetErrString(strResult, IVS_OPEN_FILE_ERROR);
		return ;
	}
	std::string strPathFile = strPath + TVWALL_FILE_NAME;
	CXml xml;
	IVS_INT32 iRet = IVS_OPEN_FILE_ERROR;
	if(xml.Load(strPathFile.c_str(), TIXML_ENCODING_LEGACY))
	{
		iRet = GetTVWallByID(uiTVWallID, strResult, xml);
	}

	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get TVWall By Id", "Failed");
		COCXXmlProcess::GetErrString(strResult, iRet);
	}
}
CString COCXTVWallXMLProcess::AddTVWall(const CString& strTVWall)
{
	CString strResult;
	//获取xml文件全路径名
	std::string strPath = MakeSureTVWallDirExist();
	if(""==strPath)
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Add TVWall", "make dir failed");
		strResult = COCXTVWallXMLProcess::GetAddTVWallResultXML(0, IVS_OPEN_FILE_ERROR);
		return strResult;
	}
	std::string strPathFile = strPath + TVWALL_FILE_NAME;
	//本地存储的电视墙列表信息xml
	CXml xml;
	if(!xml.Load(strPathFile.c_str(), TIXML_ENCODING_LEGACY))
	{//xml文件不存在
		(void)xml.AddDeclaration("1.0", "UTF-8", "");
	}
	IVS_INT32 iNewID = 0;
	IVS_INT32 iRet = AddTVWall(iNewID, strTVWall, xml);

	(void)VOS_MutexLock(m_TVWallXmlMutex);
	//保存
	if(IVS_SUCCEED==iRet && !xml.Save())
	{
		(void)VOS_MutexUnlock(m_TVWallXmlMutex);
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Add TVWall", "save failed");
		iRet = IVS_OPEN_FILE_ERROR;
	}

	strResult = GetAddTVWallResultXML(iNewID, iRet);

	m_bSaveinMem = false;
	(void)VOS_MutexUnlock(m_TVWallXmlMutex);

	return strResult;
}
IVS_INT32 COCXTVWallXMLProcess::DeleteTVWall(IVS_ULONG uiTVWallID)
{
	//获取xml文件全路径名
	std::string strPath = MakeSureTVWallDirExist();
	if(""==strPath)
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Delete TVWall", "file not exist");
		return IVS_OPEN_FILE_ERROR;
	}
	std::string strPathFile = strPath + TVWALL_FILE_NAME;
	//本地存储的电视墙列表信息xml
	CXml xml;
	if(!xml.Load(strPathFile.c_str(), TIXML_ENCODING_LEGACY))
	{//xml文件不存在
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Delete TVWall", "file not exist");
		return IVS_OPEN_FILE_ERROR;
	}
	IVS_INT32 iRet = DeleteTVWall(uiTVWallID, xml);

	(void)VOS_MutexLock(m_TVWallXmlMutex);

	//保存
	if(IVS_SUCCEED==iRet && !xml.Save())
	{
		(void)VOS_MutexUnlock(m_TVWallXmlMutex);
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Delete TVWall", "failed");
		iRet = IVS_OPEN_FILE_ERROR;
	}

	m_bSaveinMem = false;
	(void)VOS_MutexUnlock(m_TVWallXmlMutex);

	return iRet;
}
IVS_INT32 COCXTVWallXMLProcess::ModifyTVWallInfo(const CString& strResult)
{
	IVS_DEBUG_TRACE("COCXTVWallXMLProcess::ModifyTVWallInfo","Enter");
	//获取xml文件全路径名
	std::string strPath = MakeSureTVWallDirExist();
	if(""==strPath)
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Modify TVWall", "file not exist");
		return IVS_OPEN_FILE_ERROR;
	}
	std::string strPathFile = strPath + TVWALL_FILE_NAME;
	//本地存储的电视墙列表信息xml
	CXml xml;
	if(!xml.Load(strPathFile.c_str(), TIXML_ENCODING_LEGACY))
	{//xml文件不存在
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Modify TVWall", "file not exist");
		return IVS_OPEN_FILE_ERROR;
	}
	IVS_INT32 iRet = ModifyTVWallInfo(strResult, xml);

	(void)VOS_MutexLock(m_TVWallXmlMutex);

	//保存
	if(IVS_SUCCEED==iRet && !xml.Save())
	{
		(void)VOS_MutexUnlock(m_TVWallXmlMutex);
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Modify TVWall", "save failed");
		iRet = IVS_OPEN_FILE_ERROR;
	}

	m_bSaveinMem = false;
	(void)VOS_MutexUnlock(m_TVWallXmlMutex);

	return iRet;
}
IVS_INT32 COCXTVWallXMLProcess::ModifyTVWallLayout(const CString& strResult)
{
	//获取xml文件全路径名
	std::string strPath = MakeSureTVWallDirExist();
	if(""==strPath)
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Modify TVWall Layout", "file not exist");
		return IVS_OPEN_FILE_ERROR;
	}
	std::string strPathFile = strPath + TVWALL_FILE_NAME;
	//本地存储的电视墙列表信息xml
	CXml xml;
	if(!xml.Load(strPathFile.c_str(), TIXML_ENCODING_LEGACY))
	{//xml文件不存在
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Modify TVWall Layout", "file not exist");
		return IVS_OPEN_FILE_ERROR;
	}
	IVS_INT32 iRet = ModifyTVWallLayout(strResult, xml);

	(void)VOS_MutexLock(m_TVWallXmlMutex);

	//保存
    if(IVS_SUCCEED==iRet && !xml.Save())
	{
        bool bRet = false;
        for (int i = 0; i < 10; i++)
        {
            bRet = xml.Save();
            if (bRet)
            {
                BP_DBG_LOG("Modify TVWall Layout Suc. i = %d.", i);
                break;
            }

            Sleep(100);
        }

        if (bRet)
        {
            BP_DBG_LOG("Modify TVWall Layout Suc.");
            iRet = IVS_SUCCEED;
        }
        else
        {
            BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Modify TVWall Layout", "save failed");
            iRet = IVS_OPEN_FILE_ERROR;
        }
	}

	m_bSaveinMem = false;
	(void)VOS_MutexUnlock(m_TVWallXmlMutex);

	return iRet;
}

//根据窗口ID从临时场景获取对应的解码器和通道
IVS_INT32 COCXTVWallXMLProcess::GetDecoderIDAndChannelNo(IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID, IVS_TVWALL_PARAM& stTVWallParam)
{
	IVS_UINT32 uiXNum = 0;
	IVS_UINT32 uiYNum = 0;

	(void)VOS_MutexLock(m_TVWallXmlMutex);

	if (!m_bSaveinMem)
	{
		m_paneList.clear();
		IVS_INT32 iRet = COCXTVWallXMLProcess::GetTVWallPaneList(ulTVWallID, m_paneList, uiXNum, uiYNum);
		if(IVS_SUCCEED!=iRet)
		{
			(void)VOS_MutexUnlock(m_TVWallXmlMutex);
			BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Get DecoderId and Channel", "failed");
			return iRet;
		}

		m_bSaveinMem = true;
	}

	TVWALL_PANE_LIST::iterator it = m_paneList.begin();
	for (; m_paneList.end() != it; it++)
	{
		TVWALL_PANE stPane = *it;
		if (stPane.ulWndID != vPaneID.ulParentWndID)
		{
			continue;
		}

		IVS_UINT32 ulNum = stPane.uiChannelNum;
		TVWALL_PANE_CHANNEL_LIST::iterator ChannelIter = stPane.listChanel.begin();
		for (IVS_UINT32 i=1; i <= ulNum; i++)
		{
			if (i == vPaneID.ulChildWndID)
			{
				stTVWallParam.uiChannel = ChannelIter->ulChannelID;
				memcpy(stTVWallParam.cDecoderCode, stPane.szDecoderID, IVS_TVWALL_DECODER_CODE_LEN);

				(void)VOS_MutexUnlock(m_TVWallXmlMutex);

				return IVS_SUCCEED;
			}

			++ChannelIter;
		}
	}

	BP_RUN_LOG_ERR(IVS_FAIL, "Get DecoderId and Channel", "failed");

	(void)VOS_MutexUnlock(m_TVWallXmlMutex);

	return IVS_FAIL;
}

IVS_INT32 COCXTVWallXMLProcess::AddTVWall(IVS_INT32& iNewID, const CString& strTVWall, CXml& xml)const
{
	CXml addXml;//CU输入的电视墙信息
	bool bSucc = addXml.Parse(strTVWall);
	CHECK_SUCCESS_RET(bSucc, IVS_XML_INVALID);
	//新增TVWall的ID
    iNewID = 0;
	if(!xml.FindElem("Content"))
	{//没有根节点，则增加
		(void)xml.AddElem("Content");
		(void)xml.AddChildElem(TVWALL_LIST_ROOT);
		(void)xml.IntoElem();
	}  
    else 
	{
		(void)xml.IntoElem();
		 (void)xml.FindElem(TVWALL_LIST_ROOT);
		 (void)xml.IntoElem();
	}

	bool bFindInfo = false;
	IVS_INT32 iRet = IVS_FAIL;//lint !e830
	std::list<IVS_INT32> TVWallIDList;
	while(xml.FindElemValue(TVWALL_INFO_ROOT))//FindElem会导致死循环
	{
		bFindInfo = true;
		(void)xml.IntoElem();
		if (xml.FindElem("ID"))
		{ 
			const char* pValue = NULL;
			IVS_INT32 iTemp = 0;
			GET_ELEM_VALUE_NUM("ID",pValue, iTemp, xml); 
			TVWallIDList.push_back(iTemp);
		}
		xml.OutOfElem();
		if (!xml.NextElem())
		{	
			xml.OutOfElem();
			break;
		}
	}
	//xml.OutOfElem();

	/*int x = 0;
	while(x<100)
	{
		//iNewID = (GetTickCount() & 0x7FFFFFFF);//避免与其他机器ID重复，使用GetTickCount()。不能绝对避免
        iNewID = 1;  //因为需求改为了只有一个布局，于是将此ID写死为1.
		std::list<IVS_INT32>::iterator ite = find(TVWallIDList.begin(), TVWallIDList.end(), iNewID);
		if(ite==TVWallIDList.end())
		{
			break;
		}
		x++;
	}
	if(x>100)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Add TVWall", "Failed, Can not get New ID.");
		return IVS_FAIL;
	}*/

    std::list<IVS_INT32>::iterator ite = TVWallIDList.begin();
    for (; TVWallIDList.end() != ite; ite++)
    {
        IVS_INT32 iTVWallID = *ite;
        iRet = DeleteTVWall((IVS_ULONG)iTVWallID, xml);
        if(IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(IVS_FAIL, "Del Decoder Layout failed.","TVWallID = %d", iTVWallID);
            return iRet;
        }
    }

    iNewID = 1;

	{
		if(xml.FindElem(TVWALL_LIST_ROOT))
		{
			xml.IntoElem();
			if(bFindInfo)
			{
				(void)xml.AddElem("TVWall");
			}
			else
			{
				(void)xml.AddChildElem("TVWall");
				(void)xml.IntoElem();
			}
			(void)xml.AddChildElem("ID");
			(void)xml.IntoElem();
			char intStr[32];
			const char* idElemValue = itoa(iNewID,intStr,10);
			(void)xml.SetElemValue(idElemValue);
			(void)xml.AddElem("Name");
			FIND_XML_ELEM_EX(addXml, "Content/TVWall/Name", IVS_XML_INVALID);
			(void)xml.SetElemValue(addXml.GetElemValueEx("Content/TVWall/Name"));
			(void)xml.AddElem("Dsc");			
			if(addXml.FindElemEx("Content/TVWall/Dsc"))
			{
				(void)xml.SetElemValue(addXml.GetElemValueEx("Content/TVWall/Dsc"));
			}

			xml.AddElem("Layout");
			xml.AddChildElem("HorizontalCellCount");
			xml.IntoElem();
			FIND_XML_ELEM_EX(addXml, "Content/TVWall/Layout/HorizontalCellCount", IVS_XML_INVALID);
			xml.SetElemValue(addXml.GetElemValueEx("Content/TVWall/Layout/HorizontalCellCount"));
			xml.AddElem("VerticalCellCount");
			FIND_XML_ELEM_EX(addXml, "Content/TVWall/Layout/VerticalCellCount", IVS_XML_INVALID);
			xml.SetElemValue(addXml.GetElemValueEx("Content/TVWall/Layout/VerticalCellCount"));
			xml.AddElem("PaneNum");
			FIND_XML_ELEM_EX(addXml, "Content/TVWall/Layout/PaneNum", IVS_XML_INVALID);
			xml.SetElemValue(addXml.GetElemValueEx("Content/TVWall/Layout/PaneNum"));
			xml.AddElem("PaneList");
			int i=0;
			(void)addXml.FindElemEx("Content/TVWall/Layout/PaneList");
			while(addXml.FindElemValue("Pane"))
			{
				addXml.IntoElem();
				if(0==i)
				{
					(void)xml.AddChildElem("Pane");
					(void)xml.IntoElem();
				}
				else
				{
					(void)xml.AddElem("Pane");
				}

				//(void)xml.AddChildElem("ChannelCode");
				//(void)xml.IntoElem();
				//FIND_XML_ELEM_AND_INTO(addXml, "ChannelCode", IVS_XML_INVALID);
				//(void)xml.SetElemValue(addXml.GetElemValue());
				//(void)xml.AddElem("ChannelName");
				//FIND_XML_ELEM_AND_INTO(addXml, "ChannelName", IVS_XML_INVALID);
				//(void)xml.SetElemValue(addXml.GetElemValue());

				(void)xml.AddChildElem("DecoderID");
				(void)xml.IntoElem();
				FIND_XML_ELEM_AND_INTO(addXml, "DecoderID", IVS_XML_INVALID);
				(void)xml.SetElemValue(addXml.GetElemValue());
				(void)xml.AddElem("DecoderName");
				FIND_XML_ELEM_AND_INTO(addXml, "DecoderName", IVS_XML_INVALID);
				(void)xml.SetElemValue(addXml.GetElemValue());
				(void)xml.AddElem("IsMerged");
				FIND_XML_ELEM_AND_INTO(addXml, "IsMerged", IVS_XML_INVALID);
				(void)xml.SetElemValue(addXml.GetElemValue());
				(void)xml.AddElem("Name");
				FIND_XML_ELEM_AND_INTO(addXml, "Name", IVS_XML_INVALID);
				(void)xml.SetElemValue(addXml.GetElemValue());
				(void)xml.AddElem("PointHeight");
				FIND_XML_ELEM_AND_INTO(addXml, "PointHeight", IVS_XML_INVALID);
				(void)xml.SetElemValue(addXml.GetElemValue());
				(void)xml.AddElem("PointWidth");
				FIND_XML_ELEM_AND_INTO(addXml, "PointWidth", IVS_XML_INVALID);
				(void)xml.SetElemValue(addXml.GetElemValue());
				(void)xml.AddElem("PointX");
				FIND_XML_ELEM_AND_INTO(addXml, "PointX", IVS_XML_INVALID);
				(void)xml.SetElemValue(addXml.GetElemValue());
				(void)xml.AddElem("PointY");
				FIND_XML_ELEM_AND_INTO(addXml, "PointY", IVS_XML_INVALID);
				(void)xml.SetElemValue(addXml.GetElemValue());				
				//(void)xml.AddElem("ChannelType");
				//FIND_XML_ELEM_AND_INTO(addXml, "ChannelType", IVS_XML_INVALID);
				//(void)xml.SetElemValue(addXml.GetElemValue());

				xml.AddElem("ChannelList");
				int j=0;
				FIND_XML_ELEM_AND_INTO(addXml, "ChannelList", IVS_XML_INVALID);
				while(addXml.FindElemValue("Channel"))
				{
					addXml.IntoElem();
					if(0==j)
					{
						(void)xml.AddChildElem("Channel");
						(void)xml.IntoElem();
					}
					else
					{
						(void)xml.AddElem("Channel");
					}

					(void)xml.AddChildElem("Code");
					(void)xml.IntoElem();
					FIND_XML_ELEM_AND_INTO(addXml, "Code", IVS_XML_INVALID);
					(void)xml.SetElemValue(addXml.GetElemValue());
					(void)xml.AddElem("Name");
					FIND_XML_ELEM_AND_INTO(addXml, "Name", IVS_XML_INVALID);
					(void)xml.SetElemValue(addXml.GetElemValue());
					(void)xml.AddElem("Type");
					FIND_XML_ELEM_AND_INTO(addXml, "Type", IVS_XML_INVALID);
					(void)xml.SetElemValue(addXml.GetElemValue());

					xml.OutOfElem();
					addXml.OutOfElem();

					//下一个节点
					j++;
					if(!addXml.NextElem())
					{
						addXml.OutOfElem();
						xml.OutOfElem();
						break;
					}
				}

				xml.OutOfElem();
				addXml.OutOfElem();
				//下一个节点
				i++;
				if(!addXml.NextElem())
				{
					addXml.OutOfElem();
					break;
				}
			}
			xml.OutOfElem();
			xml.OutOfElem();
			iRet = IVS_SUCCEED;
		}
		xml.OutOfElem();

	}
	return iRet;
}


IVS_INT32 COCXTVWallXMLProcess::DeleteTVWall(IVS_ULONG uiTVWallID, CXml& xml)const
{
	IVS_INT32 iRet = IVS_FAIL;
	if(!xml.FindElem("Content"))
	{
		return IVS_OPEN_FILE_ERROR;
	}
	(void)xml.IntoElem();
	if(!xml.FindElem(TVWALL_LIST_ROOT))
	{
		return IVS_OPEN_FILE_ERROR;
	}
	(void)xml.IntoElem();
	CString strTVWallID;
	strTVWallID.Format("%u", uiTVWallID);
	while(xml.FindElemValue(TVWALL_INFO_ROOT))//FindElem会导致死循环
	{
		const char* pValue = NULL;
		(void)xml.IntoElem();
		if (xml.FindElem("ID"))
		{
			pValue = xml.GetElemValue(); 
		}
		if(pValue && 0==strncmp(strTVWallID, pValue, TVWALL_ID_MAX_LENGTH))
		{//找到节点，删除
			xml.OutOfElem();
			if(!xml.RemoveElem())
			{
				iRet = IVS_FAIL;
			}
			else
			{
				iRet = IVS_SUCCEED;
			}
			break;
		}
		xml.OutOfElem();
		if(!xml.NextElem())
		{
			xml.OutOfElem();
			break;
		}
	}
	xml.OutOfElem();
	return iRet;
}

IVS_INT32 COCXTVWallXMLProcess::ModifyTVWallInfo(const CString& strResult, CXml& xml)const
{
	IVS_DEBUG_TRACE("COCXTVWallXMLProcess::ModifyTVWallInfo2","Enter");
	CXml modXml;//CU输入的电视墙信息

	bool bSucc = modXml.Parse(strResult);
	CHECK_SUCCESS_RET(bSucc, IVS_XML_INVALID);
	FIND_XML_ELEM_AND_INTO(modXml, "Content", IVS_XML_INVALID);
	FIND_XML_ELEM_AND_INTO(modXml, "TVWall", IVS_XML_INVALID);
	//修改TVWall的ID
	const char* pTVWallID = NULL;
	if (modXml.FindElem("ID"))
	{
		pTVWallID = modXml.GetElemValue();
	}
	CHECK_POINTER(pTVWallID, IVS_XML_INVALID);
	IVS_INT32 iRet = IVS_OPEN_FILE_ERROR;

	FIND_XML_ELEM_AND_INTO(xml, "Content", IVS_XML_INVALID);
	FIND_XML_ELEM_AND_INTO(xml, TVWALL_LIST_ROOT, IVS_XML_INVALID);

	while(xml.FindElemValue(TVWALL_INFO_ROOT))//FindElem会导致死循环
	{
		const char* pValue = NULL;
		(void)xml.IntoElem();
		if (xml.FindElem("ID"))
		{
			pValue = xml.GetElemValue(); 
		}
		if(pValue && 0==strncmp(pValue, pTVWallID, TVWALL_ID_MAX_LENGTH))
		{//找到节点，修改信息
			const char* pTemp = NULL;
			FIND_XML_ELEM_AND_INTO(modXml, "Name", IVS_XML_INVALID);
			pTemp = modXml.GetElemValue();
			if(NULL!=pTemp)
			{
				if(xml.FindElem("Name"))
				{
					xml.ModifyElemValue(pTemp);
				}
				pTemp = NULL;
			}
			if(modXml.FindElem("Dsc") && xml.FindElem("Dsc"))
			{
				pTemp = modXml.GetElemValue();
				const char* pDsc = xml.GetElemValue();
				if(NULL!=pTemp && NULL!=pDsc)
				{
					if('\0'!=pTemp[0] && '\0'!=pDsc[0])
					{
						xml.SetElemValue(pTemp);
					}
				}
				pTemp = NULL;
			}
			xml.OutOfElem();
			iRet = IVS_SUCCEED;
			break;
		}
		xml.OutOfElem();
		if(!xml.NextElem())
		{
			xml.OutOfElem();
			break;
		}
	}
	return iRet;
}

IVS_INT32 COCXTVWallXMLProcess::ModifyTVWallLayout(const CString& strResult, CXml& xml)const
{
	CXml modXml;//CU输入的电视墙信息
	if(!modXml.Parse(strResult))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Modify TVWall Layout", "parse XML failed");
		return IVS_XML_INVALID;
	}

	FIND_XML_ELEM_AND_INTO(modXml, "Content", IVS_XML_INVALID);
	FIND_XML_ELEM_AND_INTO(modXml, "TVWall", IVS_XML_INVALID);

	//修改TVWall的ID
	const char* pTVWallID = NULL;
	FIND_XML_ELEM_AND_INTO(modXml, "ID", IVS_XML_INVALID);
	pTVWallID = modXml.GetElemValue();

	CHECK_POINTER(pTVWallID, IVS_XML_INVALID);


    //找到电视墙名称
    CString strTVWall;
    IVS_INT32 iRet = GetTVWallByID((IVS_UINT32)(atoi(pTVWallID)), strTVWall, xml);
    if(IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Get TVWall failed.","TVWallID = %s", pTVWallID);
        return iRet;
    }

    CXml TempXml;
    if(!TempXml.Parse(strTVWall))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Modify TVWall Layout", "parse XML failed");
        return IVS_XML_INVALID;
    }

    const char* pTVWallName = NULL;
    FIND_XML_ELEM_EX(TempXml, "Content/TVWall/Name", IVS_XML_INVALID);
    pTVWallName = TempXml.GetElemValue();
    CHECK_POINTER(pTVWallName, IVS_XML_INVALID);


    //写入新的数据
    xml.GetRootPos();
    FIND_XML_ELEM_AND_INTO(xml, "Content", IVS_XML_INVALID);
    (void)xml.RemoveElem();

    FIND_XML_ELEM_AND_INTO(xml, "Content", IVS_XML_INVALID);
    xml.AddChildElem("TVWallList");
    xml.IntoElem();

    xml.AddChildElem("TVWall");
    xml.IntoElem();

    xml.AddChildElem("ID");
    xml.IntoElem();
    xml.SetElemValue("1");

    xml.AddElem("Name");
    xml.SetElemValue(pTVWallName);

    xml.AddElem("Dsc");//lint !e830
    xml.SetElemValue(modXml.GetElemValueEx("Content/TVWall/Dsc"));

    xml.AddElem("Layout");
    xml.AddChildElem("HorizontalCellCount");
    xml.IntoElem();
    FIND_XML_ELEM_EX(modXml, "Content/TVWall/Layout/HorizontalCellCount", IVS_XML_INVALID);
    xml.SetElemValue(modXml.GetElemValueEx("Content/TVWall/Layout/HorizontalCellCount"));
    xml.AddElem("VerticalCellCount");
    FIND_XML_ELEM_EX(modXml, "Content/TVWall/Layout/VerticalCellCount", IVS_XML_INVALID);
    xml.SetElemValue(modXml.GetElemValueEx("Content/TVWall/Layout/VerticalCellCount"));
    xml.AddElem("PaneNum");
    FIND_XML_ELEM_EX(modXml, "Content/TVWall/Layout/PaneNum", IVS_XML_INVALID);
    xml.SetElemValue(modXml.GetElemValueEx("Content/TVWall/Layout/PaneNum"));
    xml.AddElem("PaneList");
    int i=0;
    (void)modXml.FindElemEx("Content/TVWall/Layout/PaneList");
    while(modXml.FindElemValue("Pane"))
    {
        modXml.IntoElem();
        if(0==i)
        {
            (void)xml.AddChildElem("Pane");
            (void)xml.IntoElem();
        }
        else
        {
            (void)xml.AddElem("Pane");
        }

        //(void)xml.AddChildElem("ChannelCode");
        //(void)xml.IntoElem();
        //FIND_XML_ELEM_AND_INTO(modXml, "ChannelCode", IVS_XML_INVALID);
        //(void)xml.SetElemValue(modXml.GetElemValue());
        //(void)xml.AddElem("ChannelName");
        //FIND_XML_ELEM_AND_INTO(modXml, "ChannelName", IVS_XML_INVALID);
        //(void)xml.SetElemValue(modXml.GetElemValue());
        (void)xml.AddChildElem("DecoderID");
        (void)xml.IntoElem();
        FIND_XML_ELEM_AND_INTO(modXml, "DecoderID", IVS_XML_INVALID);
        (void)xml.SetElemValue(modXml.GetElemValue());
        (void)xml.AddElem("DecoderName");
        FIND_XML_ELEM_AND_INTO(modXml, "DecoderName", IVS_XML_INVALID);
        (void)xml.SetElemValue(modXml.GetElemValue());
        (void)xml.AddElem("IsMerged");
        FIND_XML_ELEM_AND_INTO(modXml, "IsMerged", IVS_XML_INVALID);
        (void)xml.SetElemValue(modXml.GetElemValue());
        (void)xml.AddElem("Name");
        FIND_XML_ELEM_AND_INTO(modXml, "Name", IVS_XML_INVALID);
        (void)xml.SetElemValue(modXml.GetElemValue());
        (void)xml.AddElem("PointHeight");
        FIND_XML_ELEM_AND_INTO(modXml, "PointHeight", IVS_XML_INVALID);
        (void)xml.SetElemValue(modXml.GetElemValue());
        (void)xml.AddElem("PointWidth");
        FIND_XML_ELEM_AND_INTO(modXml, "PointWidth", IVS_XML_INVALID);
        (void)xml.SetElemValue(modXml.GetElemValue());
        (void)xml.AddElem("PointX");
        FIND_XML_ELEM_AND_INTO(modXml, "PointX", IVS_XML_INVALID);
        (void)xml.SetElemValue(modXml.GetElemValue());
        (void)xml.AddElem("PointY");
        FIND_XML_ELEM_AND_INTO(modXml, "PointY", IVS_XML_INVALID);
        (void)xml.SetElemValue(modXml.GetElemValue());
        //(void)xml.AddElem("ChannelType");
        //FIND_XML_ELEM_AND_INTO(modXml, "ChannelType", IVS_XML_INVALID);
        //(void)xml.SetElemValue(modXml.GetElemValue());
        xml.AddElem("ChannelList");
        int j=0;
        FIND_XML_ELEM_AND_INTO(modXml, "ChannelList", IVS_XML_INVALID);
        while(modXml.FindElemValue("Channel"))
        {
            modXml.IntoElem();
            if(0==j)
            {
                (void)xml.AddChildElem("Channel");
                (void)xml.IntoElem();
            }
            else
            {
                (void)xml.AddElem("Channel");
            }

            (void)xml.AddChildElem("Code");
            (void)xml.IntoElem();
            FIND_XML_ELEM_AND_INTO(modXml, "Code", IVS_XML_INVALID);
            (void)xml.SetElemValue(modXml.GetElemValue());
            (void)xml.AddElem("Name");
            FIND_XML_ELEM_AND_INTO(modXml, "Name", IVS_XML_INVALID);
            (void)xml.SetElemValue(modXml.GetElemValue());
            (void)xml.AddElem("Type");
            FIND_XML_ELEM_AND_INTO(modXml, "Type", IVS_XML_INVALID);
            (void)xml.SetElemValue(modXml.GetElemValue());

            xml.OutOfElem();
            modXml.OutOfElem();

            //下一个节点
            j++;
            if(!modXml.NextElem())
            {
                modXml.OutOfElem();
                xml.OutOfElem();
                break;
            }
        }

        xml.OutOfElem();
        modXml.OutOfElem();
        //下一个节点
        i++;
        if(!modXml.NextElem())
        {
            modXml.OutOfElem();
            break;
        }
    }
    xml.OutOfElem();
    iRet = IVS_SUCCEED;

	return iRet;
}

IVS_INT32 COCXTVWallXMLProcess::GetTVWallByID(IVS_UINT32 uiTVWallID, CString& strResult, CXml& xml)const
{
	IVS_INT32 iRet = IVS_FAIL;
	

	try
	{
		FIND_XML_ELEM_EX(xml, "Content/TVWallList", IVS_OPEN_FILE_ERROR);
		CXml* pTvwallXml = new CXml();
		if (NULL == pTvwallXml)//lint !e774
		{
			return iRet;
		}
		(void)pTvwallXml->AddDeclaration("1.0", "UTF-8", "");
		(void)pTvwallXml->AddElem("Content");
		(void)pTvwallXml->AddChildElem("ResultCode");
		(void)pTvwallXml->IntoElem();
		(void)pTvwallXml->SetElemValue("0");
		(void)pTvwallXml->AddElem("TVWall");
		CString strTVWallID;
		strTVWallID.Format("%u", uiTVWallID);
		//while(xml.FindElemValue(TVWALL_INFO_ROOT))//FindElem会导致死循环
		//{
        if (!xml.FindElemValue(TVWALL_INFO_ROOT))
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "get tvwall by id,", "fail to find %s node, uiTVWallID input paraL%u.", TVWALL_INFO_ROOT, uiTVWallID);
            return IVS_XML_INVALID;
        }

		const char* pValue = NULL;
		(void)xml.IntoElem();
		if (xml.FindElem("ID"))
		{
			pValue = xml.GetElemValue(); 
		}
		//if(pValue && 0==strncmp(strTVWallID, pValue, TVWALL_ID_MAX_LENGTH))
		//{//找到节点
		(void)pTvwallXml->AddChildElem("ID");
		(void)pTvwallXml->IntoElem();
		(void)pTvwallXml->SetElemValue(pValue);
		(void)pTvwallXml->AddElem("Name");

		FIND_XML_PELEM_AND_INTO(xml, "Name", IVS_XML_INVALID, pTvwallXml); //lint !e774
		(void)pTvwallXml->SetElemValue(xml.GetElemValue());
		if(xml.FindElem("Dsc"))
		{
			(void)pTvwallXml->AddElem("Dsc");
			(void)pTvwallXml->SetElemValue(xml.GetElemValue());
		}
		if(xml.FindElem("Layout"))
		{
			(void)xml.IntoElem();
			(void)pTvwallXml->AddElem("Layout");
			(void)pTvwallXml->AddChildElem("HorizontalCellCount");
			(void)pTvwallXml->IntoElem();
			FIND_XML_PELEM_AND_INTO(xml, "HorizontalCellCount", IVS_XML_INVALID, pTvwallXml); //lint !e774
			(void)pTvwallXml->SetElemValue(xml.GetElemValue());
			(void)pTvwallXml->AddElem("VerticalCellCount");
			FIND_XML_PELEM_AND_INTO(xml, "VerticalCellCount", IVS_XML_INVALID, pTvwallXml); //lint !e774
			(void)pTvwallXml->SetElemValue(xml.GetElemValue());
			(void)pTvwallXml->AddElem("PaneNum");
			FIND_XML_PELEM_AND_INTO(xml, "PaneNum", IVS_XML_INVALID, pTvwallXml); //lint !e774
			(void)pTvwallXml->SetElemValue(xml.GetElemValue());
			int i=0;
			if(xml.FindElem("PaneList"))
			{
				(void)xml.IntoElem();
				pTvwallXml->AddElem("PaneList");
				while(xml.FindElemValue("Pane"))
				{
					xml.IntoElem();
					if(0==i)
					{
						(void)pTvwallXml->AddChildElem("Pane");
						(void)pTvwallXml->IntoElem();
					}
					else
					{
						(void)pTvwallXml->AddElem("Pane");
					}

					//(void)tvwallXml.AddChildElem("ChannelCode");
					//(void)tvwallXml.IntoElem();
					//FIND_XML_PELEM_AND_INTO(xml, "ChannelCode", IVS_XML_INVALID);
					//(void)tvwallXml.SetElemValue(xml.GetElemValue());
					//(void)tvwallXml.AddElem("ChannelName");
					//FIND_XML_PELEM_AND_INTO(xml, "ChannelName", IVS_XML_INVALID);
					//(void)tvwallXml.SetElemValue(xml.GetElemValue());
					//(void)tvwallXml.AddElem("DecoderID");
					(void)pTvwallXml->AddChildElem("DecoderID");
					(void)pTvwallXml->IntoElem();
					FIND_XML_PELEM_AND_INTO(xml, "DecoderID", IVS_XML_INVALID, pTvwallXml);
					(void)pTvwallXml->SetElemValue(xml.GetElemValue());
					(void)pTvwallXml->AddElem("DecoderName");
					FIND_XML_PELEM_AND_INTO(xml, "DecoderName", IVS_XML_INVALID, pTvwallXml);
					(void)pTvwallXml->SetElemValue(xml.GetElemValue());
					(void)pTvwallXml->AddElem("IsMerged");
					FIND_XML_PELEM_AND_INTO(xml, "IsMerged", IVS_XML_INVALID, pTvwallXml);
					(void)pTvwallXml->SetElemValue(xml.GetElemValue());
					(void)pTvwallXml->AddElem("Name");
					FIND_XML_PELEM_AND_INTO(xml, "Name", IVS_XML_INVALID, pTvwallXml);
					(void)pTvwallXml->SetElemValue(xml.GetElemValue());
					(void)pTvwallXml->AddElem("PointHeight");
					FIND_XML_PELEM_AND_INTO(xml, "PointHeight", IVS_XML_INVALID, pTvwallXml);
					(void)pTvwallXml->SetElemValue(xml.GetElemValue());
					(void)pTvwallXml->AddElem("PointWidth");
					FIND_XML_PELEM_AND_INTO(xml, "PointWidth", IVS_XML_INVALID, pTvwallXml);
					(void)pTvwallXml->SetElemValue(xml.GetElemValue());
					(void)pTvwallXml->AddElem("PointX");
					FIND_XML_PELEM_AND_INTO(xml, "PointX", IVS_XML_INVALID, pTvwallXml);
					(void)pTvwallXml->SetElemValue(xml.GetElemValue());
					(void)pTvwallXml->AddElem("PointY");
					FIND_XML_PELEM_AND_INTO(xml, "PointY", IVS_XML_INVALID, pTvwallXml);
					(void)pTvwallXml->SetElemValue(xml.GetElemValue());
					//(void)tvwallXml.AddElem("ChannelType");
					//FIND_XML_PELEM_AND_INTO(xml, "ChannelType", IVS_XML_INVALID);
					//(void)tvwallXml.SetElemValue(xml.GetElemValue());
					int j = 0;
					if(xml.FindElem("ChannelList"))
					{
						(void)xml.IntoElem();
						pTvwallXml->AddElem("ChannelList");
						while(xml.FindElemValue("Channel"))
						{
							xml.IntoElem();
							if(0 == j)
							{
								(void)pTvwallXml->AddChildElem("Channel");
								(void)pTvwallXml->IntoElem();
							}
							else
							{
								(void)pTvwallXml->AddElem("Channel");
							}

							(void)pTvwallXml->AddChildElem("Code");
							(void)pTvwallXml->IntoElem();
							FIND_XML_PELEM_AND_INTO(xml, "Code", IVS_XML_INVALID, pTvwallXml);
							(void)pTvwallXml->SetElemValue(xml.GetElemValue());
							(void)pTvwallXml->AddElem("Name");
							FIND_XML_PELEM_AND_INTO(xml, "Name", IVS_XML_INVALID, pTvwallXml);
							(void)pTvwallXml->SetElemValue(xml.GetElemValue());
							(void)pTvwallXml->AddElem("Type");
							FIND_XML_PELEM_AND_INTO(xml, "Type", IVS_XML_INVALID, pTvwallXml);
							(void)pTvwallXml->SetElemValue(xml.GetElemValue());

							pTvwallXml->OutOfElem();
							xml.OutOfElem();

							//下一个节点
							j++;
							if(!xml.NextElem())
							{
								xml.OutOfElem();
								pTvwallXml->OutOfElem();
								break;
							}
						}
					}

					pTvwallXml->OutOfElem();
					xml.OutOfElem();
					//下一个节点
					i++;
					if(!xml.NextElem())
					{
						xml.OutOfElem();
						break;
					}
				}
			}
			unsigned int xmlLen = 0;
			const char* pXML = pTvwallXml->GetXMLStream(xmlLen);
			if(NULL!=pXML)
			{
				strResult = pXML;
				iRet = IVS_SUCCEED;
			}
			else
			{
				iRet = IVS_FAIL;
				BP_RUN_LOG_ERR(IVS_FAIL, "TVWall XML", "Get XML Stream Failed");
			}
		}
		xml.OutOfElem();
		//if(!xml.NextElem())
		//{
		//	xml.OutOfElem();
		//	break;
		//}

		if (NULL != pTvwallXml)
		{
			delete pTvwallXml;
			pTvwallXml = NULL;
		}
	}
	catch(...)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "TVWall XML", "Throw exception");
	}

	return iRet;
}

//解码器注册信息
IVS_INT32 COCXTVWallXMLProcess::GetRemoteDecoderRegXML(const CUMW_NOTIFY_REMOTE_DECODER_STATUS& stDecoderInfo, char* pXmlBuf, UINT bufLen)const
{
	CXml xml;

	(void)xml.AddDeclaration("1.0", "UTF-8", "");

	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("MessageType");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue("IVS_OCX_MSG_DECODER_ONLINE");

	AddDecoderNode(xml, stDecoderInfo, false, false);

	xml.OutOfElem();

	unsigned int nXMLStreamLength;
	const char* pXml = xml.GetXMLStream(nXMLStreamLength);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
	if(nXMLStreamLength>=bufLen)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Decoder RegInfo XML", "buffer is small");
		return IVS_FAIL;
	}
	memset(pXmlBuf, 0, bufLen);
	if(!CToolsHelp::Strncpy(pXmlBuf, bufLen, pXml, nXMLStreamLength))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Decoder RegInfo XML", "Copy XML failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	return IVS_SUCCEED;
}

IVS_INT32 COCXTVWallXMLProcess::GetRemoteDecoderEndXML(const CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO& stInitInfo, char* pXmlBuf, UINT bufLen)const
{
	CXml xml;
	char szBuf[CUMW_DECODER_ID_LEN + 1] = {'\0'};
	(void)xml.AddDeclaration("1.0", "UTF-8", "");

	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("MessageType");
	(void)(xml.IntoElem());
	(void)xml.SetElemValue("IVS_OCX_MSG_DECODER_OFFLINE");

	(void)xml.AddElem("Decoder");
	(void)xml.AddChildElem("ID");
	(void)(xml.IntoElem());
	if(!CToolsHelp::Strncpy(szBuf, sizeof(szBuf), stInitInfo.szDecoderID, CUMW_DECODER_ID_LEN))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Decoder Offline XML", "Copy DecoderID failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	(void)xml.SetElemValue(szBuf);

	xml.OutOfElem();
	xml.OutOfElem();

	unsigned int nXMLStreamLength;
	const char* pXml = xml.GetXMLStream(nXMLStreamLength);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
	if(nXMLStreamLength>=bufLen)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Decoder Offline XML", "buffer is small");
		return IVS_FAIL;
	}
	memset(pXmlBuf, 0, bufLen);
	if(!CToolsHelp::Strncpy(pXmlBuf, bufLen, pXml, nXMLStreamLength))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Decoder Offline XML", "Copy XML failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	return IVS_SUCCEED;
}

IVS_INT32 COCXTVWallXMLProcess::GetRemotePlayBackXML(int iWinID, unsigned long ulErrorCode, char* pXmlBuf, UINT bufLen)const
{
    CXml xml;
    (void)xml.AddDeclaration("1.0", "UTF-8", "");

    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("MessageType");
    (void)(xml.IntoElem());
    (void)xml.SetElemValue("IVS_OCX_REMOTE_PLAYBACK");

    (void)xml.AddElem("WndID");
    std::string str = CToolsHelp::Int2Str(iWinID);
    (void)xml.SetElemValue(str.c_str());

    (void)xml.AddElem("ExceptionCode");
     str = CToolsHelp::Int2Str((int)ulErrorCode);
    (void)xml.SetElemValue(str.c_str());

    xml.OutOfElem();

    unsigned int nXMLStreamLength;
    const char* pXml = xml.GetXMLStream(nXMLStreamLength);
    CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
    if(nXMLStreamLength>=bufLen)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Get Decoder Offline XML", "buffer is small");
        return IVS_FAIL;
    }
    memset(pXmlBuf, 0, bufLen);
    if(!CToolsHelp::Strncpy(pXmlBuf, bufLen, pXml, nXMLStreamLength))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Get Decoder Offline XML", "Copy XML failed");
        return IVS_ALLOC_MEMORY_ERROR;
    }

    BP_DBG_LOG("COCXTVWallXMLProcess::GetRemotePlayBackXML: pXmlBuf = %s.", pXmlBuf);

    return IVS_SUCCEED;
}

//视频播放异步通知
IVS_INT32 COCXTVWallXMLProcess::GetNotifyAsyncRetXML(const CUMW_NOTIFY_ASYNC_RET_EX& stAsyncRet, char* pXmlBuf, UINT bufLen)const
{
    CXml xml;
    char szBuf[IVS_DEV_CODE_LEN + 1] = {'\0'};
    (void)xml.AddDeclaration("1.0", "UTF-8", "");

    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("MessageType");
    (void)(xml.IntoElem());
    (void)xml.SetElemValue("IVS_OCX_MSG_ASYNC_RET");

    (void)xml.AddElem("ResultCode");
    (void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<int>(stAsyncRet.iRetCode)).c_str());
    (void)xml.AddElem("CameraCode");
    if(!CToolsHelp::Strncpy(szBuf, sizeof(szBuf), stAsyncRet.szCameraCode, IVS_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Async Return XML", "Copy CameraCode failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    (void)xml.SetElemValue(szBuf);

    xml.OutOfElem();

    unsigned int nXMLStreamLength;
    const char* pXml = xml.GetXMLStream(nXMLStreamLength);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
    if(nXMLStreamLength>=bufLen)
    {
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Async Return XML", "buffer is small");
        return IVS_FAIL;
    }
    memset(pXmlBuf, 0, bufLen);
    if(!CToolsHelp::Strncpy(pXmlBuf, bufLen, pXml, nXMLStreamLength))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Async Return XML", "Copy XML failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
    return IVS_SUCCEED;
}

//根据窗口ID从临时场景获取对应的解码器和通道
IVS_INT32 COCXTVWallXMLProcess::GetDecoderIDAndChannelNo(IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID, IVS_TVWALL_PARAM& stTVWallParam, CXml& xmlTVWall)const
{
	IVS_INT32 iRet = IVS_FAIL;
	char buf[100] = {0};
	ultoa(ulTVWallID, buf, 10);
	FIND_XML_ELEM_EX(xmlTVWall, "Content/TVWallList", IVS_XML_INVALID);
	while(xmlTVWall.FindElemValue(TVWALL_INFO_ROOT))//FindElem会导致死循环
	{
		const char* pValue = NULL;
		(void)xmlTVWall.IntoElem();
		if (xmlTVWall.FindElem("ID"))
		{
			pValue = xmlTVWall.GetElemValue(); 
		}
		if(pValue && 0==strncmp(buf, pValue, TVWALL_ID_MAX_LENGTH))
		{//找到节点
			if(xmlTVWall.FindElem("Layout"))
			{
				(void)xmlTVWall.IntoElem();
				if(xmlTVWall.FindElem("PaneList"))
				{
					(void)xmlTVWall.IntoElem();
					while(xmlTVWall.FindElemValue("Pane"))
					{
						xmlTVWall.IntoElem();
						FIND_XML_ELEM_AND_INTO(xmlTVWall, "Name", IVS_XML_INVALID);
						pValue = xmlTVWall.GetElemValue();
						CHECK_POINTER(pValue, IVS_XML_INVALID);
						CString strWndID;
						strWndID.Format("%lu", vPaneID.ulParentWndID);
						if(0 == strncmp(pValue, strWndID, TVWALL_ID_MAX_LENGTH))
						{
							//找到对应父窗格
							FIND_XML_ELEM_AND_INTO(xmlTVWall, "DecoderID", IVS_XML_INVALID);
							pValue = xmlTVWall.GetElemValue();
							if(!CToolsHelp::Strncpy(stTVWallParam.cDecoderCode, sizeof(stTVWallParam.cDecoderCode), pValue, CUMW_DECODER_ID_LEN))
							{
								return IVS_ALLOC_MEMORY_ERROR;
							}
							//GET_ELEM_VALUE_NUM_FOR_UINT("ChannelCode", pValue, stTVWallParam.uiChannel, xmlTVWall);
							//return IVS_SUCCEED;
							
							if(xmlTVWall.FindElem("ChannelList"))
							{
								(void)xmlTVWall.IntoElem();
								ULONG index = 1;
								while(xmlTVWall.FindElemValue("Channel"))
								{
									xmlTVWall.IntoElem();
									if(index == vPaneID.ulChildWndID)
									{
										if(xmlTVWall.FindElemValue("Code"))
										{
											GET_ELEM_VALUE_NUM_FOR_UINT("Code", pValue, stTVWallParam.uiChannel, xmlTVWall);
											return IVS_SUCCEED;
										}
									}
									index++;

									xmlTVWall.OutOfElem();
									if(!xmlTVWall.NextElem())
									{
										xmlTVWall.OutOfElem();
										break;
									}
								}
							}
						}

						xmlTVWall.OutOfElem();
						if(!xmlTVWall.NextElem())
						{
							xmlTVWall.OutOfElem();
							break;
						}
					}
				}
			}
		}
		xmlTVWall.OutOfElem();
		if(!xmlTVWall.NextElem())
		{
			xmlTVWall.OutOfElem();
			break;
		}
	}
	return iRet;
}

//解析实时浏览参数XML
IVS_INT32 COCXTVWallXMLProcess::RealPlayParamParseXML(const IVS_CHAR* pRealplayParam, IVS_REALPLAY_PARAM& RealplayParam)const
{
	CXml xml;
	if(!xml.Parse(pRealplayParam))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Realplay Param", "failed");
		return IVS_XML_INVALID;
	}
	if(!xml.FindElemEx("Content/RealplayParam"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Realplay Param", "failed");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* pElemValue = NULL;
	GET_ELEM_VALUE_NUM_FOR_UINT("StreamType", pElemValue, RealplayParam.uiStreamType, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("ProtocolType", pElemValue, RealplayParam.uiProtocolType, xml);
	GET_ELEM_VALUE_NUM("DirectFirst", pElemValue, RealplayParam.bDirectFirst, xml);
	GET_ELEM_VALUE_NUM("Multicast", pElemValue, RealplayParam.bMultiCast, xml);
	return IVS_SUCCEED;
}

//解析平台回放参数XML
IVS_INT32 COCXTVWallXMLProcess::PlatformPlaybackParamParseXML(const IVS_CHAR* pPlaybackParam, IVS_PLAYBACK_PARAM& PlaybackParam)const
{
    CXml xml;
    if(!xml.Parse(pPlaybackParam))
    {
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Replay Param", "failed");
        return IVS_XML_INVALID;
    }
    if(!xml.FindElemEx("Content/PlaybackParam"))
    {
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Parse Replay Param", "failed");
        return IVS_XML_INVALID;
    }
    const IVS_CHAR* pElemValue = NULL;
    GET_ELEM_VALUE_NUM_FOR_UINT("ProtocolType", pElemValue, PlaybackParam.uiProtocolType, xml);
    GET_ELEM_VALUE_CHAR("StartTime", pElemValue, PlaybackParam.stTimeSpan.cStart, IVS_TIME_LEN, xml);
    GET_ELEM_VALUE_CHAR("EndTime", pElemValue, PlaybackParam.stTimeSpan.cEnd, IVS_TIME_LEN, xml);
    GET_ELEM_VALUE_FLOAT("Speed", pElemValue, PlaybackParam.fSpeed, xml);
    return IVS_SUCCEED;
}

IVS_INT32 COCXTVWallXMLProcess::GetDecoderInfoList(CUMW_NOTIFY_REMOTE_DECODER_STATUS* pDecoderInfoList, IVS_UINT32 uiDecoderNum, CString& strResult)const
{
	CXml xml;

	(void)xml.AddDeclaration("1.0", "UTF-8", "");

	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(IVS_SUCCEED).c_str());
	(void)xml.AddElem("DecoderList");
	(void)xml.IntoElem();
	for(IVS_UINT32 j=0;j<uiDecoderNum;j++)
	{
		CUMW_NOTIFY_REMOTE_DECODER_STATUS& stDecoderInfo = pDecoderInfoList[j];
		if(0==j)
		{
			AddDecoderNode(xml, stDecoderInfo, true, true);
		}
		else
		{
			AddDecoderNode(xml, stDecoderInfo, false, true);
		}
	}
	xml.OutOfElem();
	xml.OutOfElem();
	unsigned int nXMLStreamLength;
	const char* pXml = xml.GetXMLStream(nXMLStreamLength);
	CHECK_POINTER(pXml, IVS_OPERATE_MEMORY_ERROR);
	strResult = pXml;
	return IVS_SUCCEED;	
}

IVS_INT32 COCXTVWallXMLProcess::GetTVWallPaneList(IVS_ULONG ulTVWallID, TVWALL_PANE_LIST& paneList, IVS_UINT32& uiHonCount, IVS_UINT32& uiVerCount)const
{
	CString strTVWall;
	GetTVWallByID(ulTVWallID, strTVWall);
	CXml xml;
	if(!xml.Parse(strTVWall))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Layout List", "parse xml failed");
		return IVS_FAIL;
	}
	if(!xml.FindElemEx("Content"))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Get Layout List", "Get TVWall failed");
		return IVS_FAIL;
	}

	IVS_INT32 iRet = IVS_FAIL;
	const IVS_CHAR* pElemValue = NULL;
	GET_ELEM_VALUE_NUM("ResultCode", pElemValue, iRet, xml);
	if(IVS_SUCCEED!=iRet)
	{
		return iRet;
	}
	xml.OutOfElem();
	xml.OutOfElem();
	iRet = ParesTVWallLayoutXML2List(xml, paneList, uiHonCount, uiVerCount);
	return iRet;
}

bool CompareName(const TVWALL_PANE& pane1, const TVWALL_PANE& pane2)
{
    if(pane1.ulPaneName<pane2.ulPaneName)
        return true;
    return false;
}

bool ComparePane(const TVWALL_PANE& pane1, const TVWALL_PANE& pane2)
{
	if(pane1.ulWndID<pane2.ulWndID)
		return true;
	return false;
}

IVS_INT32 COCXTVWallXMLProcess::ParesTVWallLayoutXML2List(CXml& xml, TVWALL_PANE_LIST& paneList, IVS_UINT32& uiHorCount, IVS_UINT32& uiVerCount)const
{
	FIND_XML_ELEM_EX(xml, "Content/TVWall/Layout", IVS_XML_INVALID);
	const IVS_CHAR* pElemValue = NULL;
	uiHorCount = 0; //水平窗格数量
	uiVerCount = 0; //垂直窗格数量
	IVS_UINT32 uiSumCount = 0; //总窗格数量
	GET_ELEM_VALUE_NUM_FOR_UINT("HorizontalCellCount", pElemValue, uiHorCount, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("VerticalCellCount", pElemValue, uiVerCount, xml);
	if(0==uiHorCount || TVWALL_WND_MAX_HOR_COUNT<uiHorCount || 0==uiVerCount)
	{
		uiHorCount = 0;
		uiVerCount = 0;
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Layout list", "Layout value invalid");
			return IVS_XML_INVALID;
	}
	FIND_XML_ELEM_AND_INTO(xml, "PaneList", IVS_XML_INVALID);
    
    // add by wanglei 00165153:2013.7.26 DTS2013071507953
    // 【SDK_多进一出】多进一出布局，开启组轮巡，组轮巡窗格播放窗格错误
    // 原先窗格编号采用 【1】 【2 5 6 7】 【3】 【4】
    // 现在改成【1】 【2 3 4 5】 【6】 【7】
    // 原因是电视墙轮巡采用编号递增访问，方式乱序
    // int iWinIDPos = 0;

	while(xml.FindElemValue("Pane"))//FindElem会导致死循环
	{
		(void)xml.IntoElem();
		IVS_UINT32 uiX = 0;
		IVS_UINT32 uiY = 0;
		IVS_UINT32 uiHeight = 0;
		IVS_UINT32 uiWidth = 0;
		TVWALL_PANE stPane = {0};

        // modify by wanglei 00165153:2013.7.26 DTS2013071507953
        // 【SDK_多进一出】多进一出布局，开启组轮巡，组轮巡窗格播放窗格错误
        // 原先窗格编号采用 【1】 【2 5 6 7】 【3】 【4】
        // 现在改成【1】 【2 3 4 5】 【6】 【7】
        // 原因是电视墙轮巡采用编号递增访问，方式乱序
		GET_ELEM_VALUE_NUM_FOR_UINT("Name", pElemValue, stPane.ulPaneName, xml);


        // stPane.ulWndID = stPane.ulPaneName + iWinIDPos;

		GET_ELEM_VALUE_NUM_FOR_UINT("PointX", pElemValue, uiX, xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("PointY", pElemValue, uiY, xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("PointHeight", pElemValue, uiHeight, xml);
		GET_ELEM_VALUE_NUM_FOR_UINT("PointWidth", pElemValue, uiWidth, xml);
		GET_ELEM_VALUE_CHAR("DecoderID", pElemValue, stPane.szDecoderID, IVS_TVWALL_DECODER_CODE_LEN, xml);
		FIND_XML_ELEM_AND_INTO(xml, "ChannelList", IVS_XML_INVALID);
		while(xml.FindElemValue("Channel"))
		{
			(void)xml.IntoElem();
			TVWALL_PANE_CHANNEL stPaneChannel = {0};
			//GET_ELEM_VALUE_CHAR("Code", pElemValue, stPaneChannel.szChannelCode, IVS_TVWALL_DECODER_CODE_LEN, xml);
			GET_ELEM_VALUE_NUM_FOR_UINT("Code", pElemValue, stPaneChannel.ulChannelID, xml);
			GET_ELEM_VALUE_CHAR("Name", pElemValue, stPaneChannel.szChannelName, IVS_TVWALL_DECODER_CODE_LEN, xml);
			GET_ELEM_VALUE_NUM_FOR_UINT("Type", pElemValue, stPaneChannel.uiChannelType, xml);
			stPane.uiChannelNum++;
			stPane.listChanel.push_back(stPaneChannel);
			xml.OutOfElem();
			if(!xml.NextElem())
			{
				xml.OutOfElem();
				break;
			}
		}


		if(0==uiHeight || 0==uiWidth || uiX>=uiHorCount || uiY>=uiVerCount)
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Layout list", "value invalid");
			return IVS_XML_INVALID;
		}
		stPane.rect.left = static_cast<LONG>(uiX);
		stPane.rect.top = static_cast<LONG>(uiY);
		stPane.rect.right = stPane.rect.left + static_cast<LONG>(uiWidth);
		stPane.rect.bottom = stPane.rect.top + static_cast<LONG>(uiHeight);
		uiSumCount += (uiWidth * uiHeight);
		if(TVWALL_WND_MAX_COUNT<uiSumCount)
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "Get Layout List", "windows count larger than %d", TVWALL_WND_MAX_COUNT);
			return IVS_FAIL;
		}
		
		paneList.push_back(stPane);
		xml.OutOfElem();
		if(!xml.NextElem())
		{
			xml.OutOfElem();
			break;
		}
	}


    // add by wanglei 00165153:2013.7.26 DTS2013071507953
    // 【SDK_多进一出】多进一出布局，开启组轮巡，组轮巡窗格播放窗格错误
    // 原先窗格编号采用 【1】 【2 5 6 7】 【3】 【4】
    // 现在改成【1】 【2 3 4 5】 【6】 【7】
    // 原因是电视墙轮巡采用编号递增访问，方式乱序

    // std::vector<IVS_ULONG> vecRealPaneID;	
    paneList.sort(CompareName);
    int iWinIDPos = 0;
    for (TVWALL_PANE_LIST::iterator itor = paneList.begin(); itor != paneList.end(); itor++)
    {

        itor->ulWndID = itor->ulPaneName + (IVS_ULONG)iWinIDPos;
        if (1 >= itor->uiChannelNum)
        {
            iWinIDPos += 0;
        }
        else if (4 >= itor->uiChannelNum)
        {
            iWinIDPos += 3;
        }
        else if (9 >= itor->uiChannelNum)
        {
            iWinIDPos += 8;
        }
        else if (16 >= itor->uiChannelNum)
        {
            iWinIDPos += 15;
        }
        else
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Layout list", "invalid channel num:%d", itor->uiChannelNum);
        }
        
     
    }

    


	paneList.sort(ComparePane);
	return IVS_SUCCEED;
}

CString COCXTVWallXMLProcess::GetAddTVWallResultXML(IVS_INT32 iTVWallID, IVS_INT32 iRet)const
{
	CString strResult;
	CXml xmlRsp;
	(void)xmlRsp.AddDeclaration("1.0","UTF-8","");
	(void)xmlRsp.AddElem("Content");
	(void)xmlRsp.AddChildElem("ResultCode");
	(void)xmlRsp.IntoElem();
	(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	(void)xmlRsp.AddElem("ID");
	(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iTVWallID).c_str());
	xmlRsp.OutOfElem();
	IVS_UINT32 uiLen = 0;
	const char* pXml = xmlRsp.GetXMLStream(uiLen);
	if(NULL!=pXml)
	{
		strResult = pXml;
	}
	return strResult;
}

std::string COCXTVWallXMLProcess::MakeSureTVWallDirExist()const
{
	// 获取appdat目录,如果不存在就创建
	char szPath[MAX_PATH]; 
	memset(szPath,0x0,MAX_PATH);
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, TRUE); 
	//是否有登陆的IP地址
	if(CLocalConfig::m_strLocalstrIP.length()==0)
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Get TVWall Dir", "No Server IP");
		return "";
	}
	// 判断指定的路径是否存在，不存在就创建文件夹
	std::string strPath = std::string(szPath);
	strPath += std::string("\\IVS\\");
	strPath += CLocalConfig::m_strLocalstrIP;
	strPath += std::string("\\TVWall\\");
	if (!MakeSureDirectoryPathExists(strPath.c_str()))
	{
		strPath = "";
	}

	return strPath;//lint !e1036
}

IVS_VOID COCXTVWallXMLProcess::AddDecoderNode(CXml& xml, const CUMW_NOTIFY_REMOTE_DECODER_STATUS& stDecoderInfo, bool bChild, bool bOnline)const
{
	char szBuf[32] ={0};
	if(bChild)
	{
		(void)xml.AddChildElem("Decoder");
		(void)xml.IntoElem();	
		(void)xml.AddChildElem("ID");
		(void)xml.IntoElem();
	}
	else
	{
		(void)xml.AddElem("Decoder");
		(void)xml.AddChildElem("ID");
		(void)xml.IntoElem();
	}
	(void)xml.SetElemValue(stDecoderInfo.szDecoderID);

	(void)xml.AddElem("Name");
	(void)xml.SetElemValue(stDecoderInfo.szDecoderName);

	(void)xml.AddElem("IP");
	(void)xml.SetElemValue(stDecoderInfo.szDecoderIP);

	(void)xml.AddElem("Encoding");
	(void)xml.SetElemValue(stDecoderInfo.szDecoderEncoding);
	if(bOnline)
	{//上报在线状态
		(void)xml.AddElem("IsOnline");
		(void)xml.SetElemValue("1");
	}

	(void)xml.AddElem("VideoPayloadTypes");
	if(stDecoderInfo.ulVideoPayloadTypeNum>0)
	{
		(void)xml.AddChildElem("PayloadType");
		(void)(xml.IntoElem());

		for (unsigned long i = 0; i < stDecoderInfo.ulVideoPayloadTypeNum; i++)
		{
			int x = stDecoderInfo.ucVideoPayloadTypeArray[i];
			memset(szBuf, 0, sizeof(szBuf));
			(void)_itoa_s( x, szBuf, 10 );
			(void)xml.SetElemValue(szBuf);

			if (i < stDecoderInfo.ulVideoPayloadTypeNum - 1)
			{
				(void)xml.AddElem("PayloadType");
			}
		}
		xml.OutOfElem();
	}
	(void)xml.AddElem("AudioPayloadTypes");
	if(stDecoderInfo.ulAudioPayloadTypeNum>0)
	{
		(void)xml.AddChildElem("PayloadType");
		(void)(xml.IntoElem());

		for (unsigned long i = 0; i < stDecoderInfo.ulAudioPayloadTypeNum; i++)
		{
			int x = stDecoderInfo.ucAudioPayloadTypeArray[i];
			memset(szBuf, 0, sizeof(szBuf));
			(void)_itoa_s(x, szBuf, 10 );
			(void)xml.SetElemValue(szBuf); 

			if (i < stDecoderInfo.ulAudioPayloadTypeNum - 1)
			{
				(void)xml.AddElem("PayloadType");
			}
		}
		xml.OutOfElem();
	}
	(void)xml.AddElem("ChannelNum");
	(void)_itoa_s( (int)stDecoderInfo.ulChannelNum, szBuf, 10 );
	(void)xml.SetElemValue(szBuf);
	(void)xml.AddElem("ChannelList");

	if(stDecoderInfo.ulChannelNum>0)
	{
		(void)xml.AddChildElem("Channel");
		(void)(xml.IntoElem());	
		(void)xml.AddChildElem("Code");
		(void)(xml.IntoElem());
		for (unsigned long i = 0; i < stDecoderInfo.ulChannelNum; i++)
		{
			memset(szBuf, 0, sizeof(szBuf));
			(void)_itoa_s( (int)stDecoderInfo.stChannelInfoArray[i].ulChannelID, szBuf, 10 );
			(void)xml.SetElemValue(szBuf);

			(void)xml.AddElem("Name");
			(void)xml.SetElemValue(stDecoderInfo.stChannelInfoArray[i].szChannelName);
			(void)xml.AddElem("ChannelType");
			(void)xml.SetElemValue(CToolsHelp::Int2Str(stDecoderInfo.stChannelInfoArray[i].ucChannelType).c_str());
			xml.OutOfElem();

			if (i < stDecoderInfo.ulChannelNum - 1)
			{
				(void)xml.AddElem("Channel");
				(void)xml.AddChildElem("Code");
				(void)(xml.IntoElem());
			}
		}
		xml.OutOfElem();
	}
	xml.OutOfElem();
}

IVS_INT32 COCXTVWallXMLProcess::GetTVWallIdList(std::list<int>& listTVWallId)const
{
    CXml xmlTVWall;

    CString strResult;
    GetTVWallList(strResult);
    if (0 == strResult.GetLength())
    {
        BP_DBG_LOG("Get TVWall id list failed.");
        return IVS_FAIL;
    }

    if (!xmlTVWall.Parse(strResult.GetBuffer(strResult.GetLength())))
    {
        BP_DBG_LOG("Parse XML failed");
        return IVS_FAIL;
    }

    FIND_XML_ELEM_EX(xmlTVWall, "Content/TVWallList", IVS_XML_INVALID);
    while(xmlTVWall.FindElemValue(TVWALL_INFO_ROOT))//FindElem会导致死循环
    {
        const char* pValue = NULL;
        (void)xmlTVWall.IntoElem();
        if (xmlTVWall.FindElem("ID"))
        {
            pValue = xmlTVWall.GetElemValue(); 

			if (pValue!=NULL)
			{
				IVS_INT32 iTVWallID = atoi(pValue);
				BP_DBG_LOG("COCXTVWallXMLProcess::GetTVWallIdList: get a tvwall is ID %d.", iTVWallID);

				listTVWallId.push_back(iTVWallID);
			}
        }

        xmlTVWall.OutOfElem();
        if(!xmlTVWall.NextElem())
        {
            xmlTVWall.OutOfElem();
            break;
        }
    }

    return IVS_SUCCEED;
}

//开始上墙的异步通知
IVS_INT32 COCXTVWallXMLProcess::GetNotifyStartRealPlayTVWallRetXML(ULONG ulTVWallID, ULONG ulWndID, IVS_INT32 iErrCode, ULONG ulTransID, char* pXmlBuf, UINT bufLen)const
{
    CHECK_POINTER(pXmlBuf,IVS_PARA_INVALID);

    CString strResult;
    CXml xmlRsp;
    (void)xmlRsp.AddDeclaration("1.0","UTF-8","");
    (void)xmlRsp.AddElem("Content");

    (void)xmlRsp.AddChildElem("MessageType");
    (void)xmlRsp.IntoElem();
    (void)xmlRsp.SetElemValue("IVS_OCX_MSG_START_REALPLAY_TVWALL");

    (void)xmlRsp.AddElem("ResultCode");
    (void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iErrCode).c_str());

    (void)xmlRsp.AddElem("StartResult");

    (void)xmlRsp.AddChildElem("TVWallID");
    (void)xmlRsp.IntoElem();
    (void)xmlRsp.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)ulTVWallID).c_str());

    (void)xmlRsp.AddElem("WndID");
    (void)xmlRsp.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)ulWndID).c_str());

    (void)xmlRsp.AddElem("TransID");
    (void)xmlRsp.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)ulTransID).c_str());

    xmlRsp.OutOfElem();
    xmlRsp.OutOfElem();

    IVS_UINT32 uiLen = 0;
    const char* pXml = xmlRsp.GetXMLStream(uiLen);
    if(NULL != pXml  &&  uiLen < bufLen)
    {
        memcpy(pXmlBuf, pXml, uiLen);
        return IVS_SUCCEED;
    }

    return IVS_FAIL;
}

IVS_INT32 COCXTVWallXMLProcess::GetNetKeyBoardConfig(IVS_INT32& iMin, IVS_INT32& iMax)const
{
	// 获取appdat目录,如果不存在就创建
	char szPath[MAX_PATH]; 
	memset(szPath, 0x0, MAX_PATH);
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, TRUE); 
	//是否有登陆的IP地址
	if(CLocalConfig::m_strLocalstrIP.length()==0)
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Get IVS Dir", "No Server IP");
		return IVS_FAIL;
	}
	// 判断指定的路径是否存在，不存在就创建文件夹
	std::string strPath = std::string(szPath);
	strPath += std::string("\\IVS\\");
	strPath += CLocalConfig::m_strLocalstrIP;
	strPath += std::string("\\NetKeyBoard\\");
	if (!MakeSureDirectoryPathExists(strPath.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Get IVS Dir", "Dir not exist: %s", strPath.c_str());
		return IVS_FAIL;
	}

	//获取xml文件全路径名
	std::string strPathFile = strPath + NETKEYBOARD_FILE_NAME;
	CXml xml;
	if(xml.Load(strPathFile.c_str(), TIXML_ENCODING_LEGACY)) //lint !e830
	{
		if (xml.FindElemEx("Content/Min"))
		{ 
			const char* pValue = NULL;                                              
			pValue = xml.GetElemValue();                                                  
			if (NULL != pValue)                                                       
			{                                                                                   
				iMin = atoi(pValue);
				BP_DBG_LOG("COCXTVWallXMLProcess::GetNetKeyBoardConfig: iMin = %d", iMin);
			} 
			else
			{
				BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "COCXTVWallXMLProcess::GetNetKeyBoardConfig", "not find iMin");
				return IVS_FAIL;
			}
		}
		else
		{
			BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "COCXTVWallXMLProcess::GetNetKeyBoardConfig", "not find iMin");
			return IVS_FAIL;
		}
		xml.OutOfElem();

		if (xml.FindElemEx("Content/Max"))
		{ 
			const char* pValue = NULL;                                                  
			pValue = xml.GetElemValue();                                                   
			if (NULL != pValue)                                                            
			{                                                                                   
				iMax = atoi(pValue);    
				BP_DBG_LOG("COCXTVWallXMLProcess::GetNetKeyBoardConfig: iMax = %d", iMax);
			} 
			else
			{
				BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "COCXTVWallXMLProcess::GetNetKeyBoardConfig", "not find iMax");
				return IVS_FAIL;
			}
		}
		else
		{
			BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "COCXTVWallXMLProcess::GetNetKeyBoardConfig", "not find iMax");
			return IVS_FAIL;
		}
	
	}
	else
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Load File Failed!", "%s", strPathFile.c_str());
		return IVS_FAIL;
	}

	if (iMax < 0  || 
		iMin < 0  || 
		(iMax - iMin) < 1 || 
		(iMax - iMin) > 10)
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "Load File Failed!", "%s", strPathFile.c_str());
		return IVS_FAIL;
	}

	return IVS_SUCCEED;
}

