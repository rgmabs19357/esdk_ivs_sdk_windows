/*****************************************************************
 filename    :    OCXUpgradeMgrMgr.cpp
 author      :    wanglei 00165153
 created     :    2013/11/13
 description :    升级管理
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/11/13 初始版本
*****************************************************************/

#include "OCXUpgradeMgr.h"
#include "OCXUpgradeMgrXMLProcess.h"
#include "OCXXmlProcess.h"
#include "IVS_Trace.h"

COCXUpgradeMgrMgr::COCXUpgradeMgrMgr()
{
}

COCXUpgradeMgrMgr::~COCXUpgradeMgrMgr()
{
}

 IVS_INT32 COCXUpgradeMgrMgr::GetCUVersion(IVS_INT32 iSessionID, CXml& xmlReq, CString& strResult)
 {
    IVS_DEBUG_TRACE("");
  
    IVS_CU_INFO stCuInfo;
    memset(&stCuInfo, 0, sizeof(stCuInfo));
    IVS_UPGRADE_INFO stUpgradeInfo;
    memset(&stUpgradeInfo, 0, sizeof(stUpgradeInfo));

	IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUpgradeMgrXMLProcess::GetCUVersionParseReqXML(xmlReq, stCuInfo);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get CU Version", "Get CU Version Parse Req XML is fail");
        return iRet;
    }

    iRet = IVS_SDK_GetCUVersion(iSessionID, &stCuInfo, &stUpgradeInfo);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get CU Version", "ivs sdk Get CU Version failed");
        return iRet;
    }
     
     // 拼装响应消息结构体为XML，
    CXml xmlRsp;
    iRet = COCXUpgradeMgrXMLProcess::GetCUVersionRspXML(stUpgradeInfo,iRet,xmlRsp);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get CU Version","Get CU Version XML is fail");
        return iRet;
    }
    IVS_UINT32 xmlLen = 0;
    strResult = xmlRsp.GetXMLStream(xmlLen);
    return IVS_SUCCEED;
 }
