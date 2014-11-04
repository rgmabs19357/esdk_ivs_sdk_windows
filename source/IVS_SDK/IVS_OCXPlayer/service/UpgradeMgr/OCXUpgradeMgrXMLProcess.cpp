#include "OCXUpgradeMgrXMLProcess.h"
#include "ivs_xml.h"

COCXUpgradeMgrXMLProcess::COCXUpgradeMgrXMLProcess()
{

}
COCXUpgradeMgrXMLProcess::~COCXUpgradeMgrXMLProcess()
{

}
IVS_INT32 COCXUpgradeMgrXMLProcess::GetCUVersionParseReqXML(CXml& xmlReq, IVS_CU_INFO& stCuInfo)
{
    if (!xmlReq.FindElemEx("Content/CUInfo"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get CU Version Parse Req XML", "fail to parse xml");
        return IVS_XML_INVALID;
    }
    const char* szXMLValue = NULL;
    (void)xmlReq.IntoElem();
    GET_ELEM_VALUE_NUM_FOR_UINT("CUType", szXMLValue, stCuInfo.uiCUType, xmlReq);
    GET_ELEM_VALUE_CHAR("CUVersion", szXMLValue,stCuInfo.cCUVersion, IVS_VERSION_LEN, xmlReq);
    xmlReq.OutOfElem();
    return IVS_SUCCEED;
}
IVS_INT32 COCXUpgradeMgrXMLProcess::GetCUVersionRspXML(IVS_UPGRADE_INFO& stUpgradeInfo, IVS_INT32 iRetCord,CXml& xmlRsp)
{
    (void)xmlRsp.AddDeclaration("1.0","UTF-8","");
    (void)xmlRsp.AddElem("Content");
	(void)xmlRsp.AddChildElem("ResultCode");
	(void)xmlRsp.IntoElem();
	(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRetCord).c_str());
    (void)xmlRsp.AddElem("UpgradeInfo");
    (void)xmlRsp.IntoElem();
    (void)xmlRsp.AddChildElem("IsUpgrade");
    (void)xmlRsp.IntoElem();
    (void)xmlRsp.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)stUpgradeInfo.bIsUpgrade).c_str());
    (void)xmlRsp.AddElem("SetupFileMD5");

    IVS_CHAR cSetupFileMd5[IVS_MD5_LEN + 1] = {0};
    strncpy(cSetupFileMd5, stUpgradeInfo.cSetupFileMd5, IVS_MD5_LEN);
    (void)xmlRsp.SetElemValue(cSetupFileMd5);

    (void)xmlRsp.AddElem("FileSize");
    (void)xmlRsp.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)stUpgradeInfo.uiFileSize).c_str());

    (void)xmlRsp.AddElem("HttpURL");
    IVS_CHAR cUrl[IVS_URL_LEN + 1] = {0};
    strncpy(cUrl, stUpgradeInfo.cHttpURL, IVS_URL_LEN);
    (void)xmlRsp.SetElemValue(cUrl);

    xmlRsp.OutOfElem();
    xmlRsp.OutOfElem();

    return IVS_SUCCEED;
}