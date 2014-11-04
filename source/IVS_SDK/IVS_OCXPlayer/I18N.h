/********************************************************************
	filename	: 	IVS_OCXPlayerRes.h
	author		:	z00201790
	created		:	2013/01/24	
	description	:	定义多国语言资源处理的类;
	copyright	:	Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
	history		:	2013/01/24 初始版本;
*********************************************************************/
#ifndef __IVS_I18N_H__
#define __IVS_I18N_H__
#include "vos.h"
#include "SDKDef.h"
typedef std::map<std::string,std::string> MAP_RESOURCE_TO_LANGUAGE;
typedef MAP_RESOURCE_TO_LANGUAGE::iterator MAP_RESOURCE_TO_LANGUAGE_ITER;

class CI18N
{
public:
	CI18N(void);
	~CI18N(void);

	static CI18N& GetInstance()
	{
		static CI18N inst;
		return inst;
	}
public:
	//根据XML的资源路径将XML加载到Map中
	bool SetLanguage(const std::string& strLanguageXMLPath);
	bool GetResourceLanguage(const std::string& strResource,std::string& strLanguage);
	void Clear();

private:
	VOS_Mutex* m_pMutexLock;

	MAP_RESOURCE_TO_LANGUAGE m_mapResource2Language;
};

#endif
