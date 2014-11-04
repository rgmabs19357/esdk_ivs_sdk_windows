#include "UpgradeMgr.h"
#include "bp_log.h"
#include "UserMgr.h"
#include "IVS_Trace.h"
#include "UpgradeMgrXMLProcess.h"

CUpgradeMgr::CUpgradeMgr(void)
    : m_pUserMgr(NULL)
{

}

CUpgradeMgr::~CUpgradeMgr(void)
{
    m_pUserMgr = NULL;
}

// 设置用户对象类
void CUpgradeMgr::SetUserMgr(CUserMgr* pUserMgr)
{
    m_pUserMgr = pUserMgr;
}


IVS_INT32 CUpgradeMgr::GetCUVersion (const IVS_CU_INFO* pCUInfo, IVS_UPGRADE_INFO* pUpgradeInfo)
{
    CHECK_POINTER(pCUInfo, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pUpgradeInfo, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
   	IVS_DEBUG_TRACE("");

    CXml xmlReq;
    IVS_INT32 iRet = CUpgradeMgrXMLProcess::GetCUVersionReqXML(pCUInfo, xmlReq);

    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Cu Version", "Get Upgrade Info Req XML fail");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    unsigned int xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);	
    CHECK_POINTER(pReq,IVS_OPERATE_MEMORY_ERROR);

    // 不带域，默认发送本域
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_GET_CU_VERSION_REQ);
    sendNssMsgInfo.SetReqData(pReq);	
    string strDomainCode;
    m_pUserMgr->GetDomainCode(strDomainCode);
    sendNssMsgInfo.SetDomainCode(strDomainCode);

    std::string strRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);

#if 0
    strRsp = "<Content> <UpgradeInfo> <IsUpgrade>1</IsUpgrade> \
             <SetupFileMd5>245</SetupFileMd5> <FileSize>112</FileSize> \
             <HttpURL>www.baidu.com</HttpURL></UpgradeInfo> </Content>";
    iRet = 0;
#endif

    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"Get Comm Send Mgr", "fail");
        return iRet;
    }

    // 解析XML查询请求
    iRet = CUpgradeMgrXMLProcess::ParseGetCuVersionRspXML(strRsp, pUpgradeInfo);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet,"parse xml to value is failed","failed");
        return iRet;
    }
    return IVS_SUCCEED;
}