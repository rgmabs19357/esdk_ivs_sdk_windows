#include "I18N.h"
#include "ToolsHelp.h"

CI18N::CI18N(void)
	:m_pMutexLock(nullptr)
{
	m_pMutexLock = VOS_CreateMutex();
}

CI18N::~CI18N(void)
{
	try
	{
		Clear();
		VOS_DestroyMutex(m_pMutexLock);
		m_pMutexLock = NULL;
	}
	catch (...)
	{
	}
}

void CI18N::Clear()
{
	try
	{
		m_mapResource2Language.clear();
	}catch (...)
	{}
}

bool CI18N::SetLanguage(const std::string& strLanguageXMLPath)
{
    BP_RUN_LOG_INF("load language xml failed","strLanguageXMLPath %s", strLanguageXMLPath.c_str());
	CXml xml;
	if (!xml.Load(strLanguageXMLPath.c_str(), TIXML_ENCODING_UTF8))
	{
		//表示加载资源XML不成功
		BP_RUN_LOG_ERR(IVS_FAIL,"load language xml failed","NA");
		return false;
	}

	//如果加载成功了，就遍历xml将所有的资源和语言绑定并加入map表
	if (!xml.FindElemEx("Content"))
	{
		//没找到第一个结点
		return false;
	}

	//先进行清除
	Clear();

	do 
	{
		// 获取当前结点
		const char* ResourceNode = xml.GetElem();
		// 获取当前结点的值
		const char* ResourceNodeValue = xml.GetElemValue();
		// 转换编码，防止乱码
		char* xml2ANSI = CToolsHelp::UTF8ToANSI(ResourceNodeValue);
		//把结点和值绑定进行插入
		(void)VOS_MutexLock(m_pMutexLock);
		(void)m_mapResource2Language.insert(std::make_pair(ResourceNode, xml2ANSI));
		(void)VOS_MutexUnlock(m_pMutexLock);
		free(xml2ANSI);
	} while (xml.NextElem());

	return true;
}

//根据资源ID到map表中去遍历获取到资源对应的语言
bool CI18N::GetResourceLanguage(const std::string& strResource,std::string& strLanguage)
{
	bool bRet = false;
	MAP_RESOURCE_TO_LANGUAGE_ITER IpIter;
	(void)VOS_MutexLock(m_pMutexLock);

	IpIter = m_mapResource2Language.find(strResource);
	if (IpIter != m_mapResource2Language.end())
	{
		//找到了
		strLanguage = static_cast<std::string>(IpIter->second);
		bRet = true;
	}
	else
	{
		//没找到
		strLanguage = "";
	}
	(void)VOS_MutexUnlock(m_pMutexLock);
	return bRet;
}
