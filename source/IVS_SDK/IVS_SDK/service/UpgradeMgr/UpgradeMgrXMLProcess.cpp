#include "UpgradeMgrXMLProcess.h"
#include "ToolsHelp.h"
#include "bp_log.h"
#include "ivs_xml.h"
#include "UserMgr.h"
#include "IVS_Trace.h"

CUpgradeMgrXMLProcess::CUpgradeMgrXMLProcess()
{
}

CUpgradeMgrXMLProcess::~CUpgradeMgrXMLProcess()
{
}

IVS_INT32 CUpgradeMgrXMLProcess::GetCUVersionReqXML(const IVS_CU_INFO* pCUInfo, CXml& xml)
{
    CHECK_POINTER(pCUInfo,IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("CUInfo");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("CUType");
    (void)xml.IntoElem();
    (void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)pCUInfo->uiCUType).c_str());
    (void)xml.AddElem("CUVersion");
    
    IVS_CHAR cCUVersion[IVS_VERSION_LEN + 1] = {0};
    strncpy(cCUVersion, pCUInfo->cCUVersion, IVS_VERSION_LEN);
    (void)xml.SetElemValue(cCUVersion);
    xml.OutOfElem();
    xml.OutOfElem();
    return 0;
}

IVS_INT32 CUpgradeMgrXMLProcess::ParseGetCuVersionRspXML(const std::string& strRspXML, IVS_UPGRADE_INFO* pUpgradeInfo)
{
    CHECK_POINTER(pUpgradeInfo,IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    memset(pUpgradeInfo, 0, sizeof(IVS_UPGRADE_INFO));

    IVS_INT32 iRet = IVS_FAIL;
    CXml xml;
    if (!xml.Parse(strRspXML.c_str()))
    {
        BP_RUN_LOG_ERR(iRet, "parse xml failed", "NA");
        return iRet;
    }

    if (!xml.FindElemEx("Content/UpgradeInfo"))
    {
        return IVS_XML_INVALID;
    }
    (void)xml.IntoElem();

    const char* szElemValue = NULL;

	char szIsUpgrade[IVS_URL_LEN+1] = {0};
	GET_ELEM_VALUE_CHAR("IsUpgrade", szElemValue, szIsUpgrade, IVS_URL_LEN,xml);
	IVS_BOOL bIsUpgrade = 0;
	if (0 == strcmp("Y", szIsUpgrade))
	{
		bIsUpgrade = 1;
	}
	else
	{
		bIsUpgrade = 0;
	}

    //GET_ELEM_VALUE_NUM("IsUpgrade", szElemValue, pUpgradeInfo->bIsUpgrade, xml);
	pUpgradeInfo->bIsUpgrade = bIsUpgrade;
    GET_ELEM_VALUE_CHAR("SetupFileMD5", szElemValue, pUpgradeInfo->cSetupFileMd5, IVS_MD5_LEN, xml);
    GET_ELEM_VALUE_NUM_FOR_UINT("FileSize", szElemValue, pUpgradeInfo->uiFileSize, xml);
    GET_ELEM_VALUE_CHAR("HttpURL", szElemValue, pUpgradeInfo->cHttpURL, IVS_URL_LEN, xml);    
    xml.OutOfElem();
    return IVS_SUCCEED;
}