#include "PtzControl.h"
#include "UserMgr.h"
#include "nss_mt.h"
#include "NSSOperator.h"
#include "PtzControlXMLProcess.h"
#include "IVS_Trace.h"
#include "ivs_xml.h"

CPtzControl::CPtzControl(void)
	: m_pUserMgr(NULL)
{
	
}

CPtzControl::~CPtzControl(void)
{
	m_pUserMgr = NULL;
}

// 设置用户对象类
void CPtzControl::SetUserMgr(CUserMgr* pUserMgr)
{
	m_pUserMgr = pUserMgr;
}

//获取云台位置信息
IVS_INT32 CPtzControl::GetPtzAbsPosition(const IVS_CHAR* pCameraCode, IVS_PTZ_ABSPOSITION* pPTZPosInfo)
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pPTZPosInfo, IVS_PARA_INVALID);

	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	//拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::GetPtzPositionGetXML(pCameraCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "CPtzControl::GetPtzPositionGetXML is failed", "failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pStatusReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pStatusReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_PTZ_ABS_POSITION_REQ);
	sendNssMsgInfo.SetReqData(pStatusReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	if (IVS_SUCCEED == iNeedRedirect)
	{
		BP_RUN_LOG_INF("sendcmd failed","server not respond");
		iRet = m_pUserMgr->SendRedirectServe(sendNssMsgInfo,strpRsp);
		return iRet;
	}

	//解析XML查询请求
	iRet = PtzControlProcess::GetPtzPositionParseXML(strpRsp, pPTZPosInfo);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"parse xml to value failed","failed");
		return iRet;
	}

	return iRet;
}

// 云镜控制
IVS_INT32 CPtzControl::PtzControl(const IVS_CHAR* pCameraCode, IVS_INT32 iControlCode,const IVS_CHAR* pControlPara1,const IVS_CHAR* pControlPara2, IVS_PTZ_CONTROL_INFO* pPtzControlInfo)
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	CHECK_POINTER(pControlPara1, IVS_PARA_INVALID);
	CHECK_POINTER(pControlPara2, IVS_PARA_INVALID);
	CHECK_POINTER(pPtzControlInfo, IVS_PARA_INVALID);
	
	IVS_DEBUG_TRACE("");
	
	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::PtzControlGetXML(pCameraCode, iControlCode,pControlPara1,pControlPara2,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"CPtzControl::PtzControlGetXML is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pStatusReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pStatusReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_PTZ_CONTROL_REQ);
	sendNssMsgInfo.SetReqData(pStatusReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

    if (IVS_SUCCEED == iNeedRedirect)
    {
        BP_RUN_LOG_INF("sendcmd failed","server not respond");
        iRet = m_pUserMgr->SendRedirectServe(sendNssMsgInfo,strpRsp);
    }

    // NSS消息返回码错误，不解析数据，直接返回错误码


#if 0
    static int pos = 0;
    if (pos++ % 10 < 5)
    {
        strpRsp = "<Content><LockStatus>1</LockStatus><LockUserInfo><UserID>CB2D778A-4CD3-11E3-8000-90E2BA1E518E</UserID><UserName>super</UserName><LoginIP>192.7.20.90</LoginIP><ReleaseTimeRemain>49</ReleaseTimeRemain></LockUserInfo></Content>";
    }
    else
    {
        strpRsp = "<Content><LockStatus>0</LockStatus></Content>"; 
    }
    iRet = 11111;
#endif

    CXml xmlRsp;

    if (!xmlRsp.Parse(strpRsp.c_str()))
    {
        // 优先返回iRet错误码
        if (IVS_SUCCEED != iRet)
        {
            return iRet;
        }
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "ptz control", "fail to parse response xml, camera code:%s, response xml:%s", pCameraCode, strpRsp.c_str());
        return IVS_XML_INVALID;
    }
    
    if (!xmlRsp.FindElemEx("Content/LockStatus"))
    {
        // 优先返回iRet错误码
        if (IVS_SUCCEED != iRet)
        {
            return iRet;
        }
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "ptz control", "fail to parse find lockstatus node in xml, camera code:%s, response xml:%s", pCameraCode, strpRsp.c_str());
        return IVS_XML_INVALID;
    }
    memset(pPtzControlInfo, 0, sizeof(IVS_PTZ_CONTROL_INFO));

    const char* pNodeTemp = NULL;
    pNodeTemp = xmlRsp.GetElemValue();
    if (NULL != pNodeTemp)
    {
        pPtzControlInfo->uiLockStatus =  (IVS_UINT32)atoi(pNodeTemp);
    }
    if (xmlRsp.FindElem("LockUserInfo"))
    {
        (void)xmlRsp.IntoElem();
        GET_ELEM_VALUE_NUM_FOR_UINT("UserID", pNodeTemp, pPtzControlInfo->uiUserID, xmlRsp);
        GET_ELEM_VALUE_CHAR("UserName", pNodeTemp, pPtzControlInfo->cUserName, IVS_NAME_LEN, xmlRsp);
        GET_ELEM_VALUE_CHAR("LoginIP", pNodeTemp, pPtzControlInfo->stIP.cIP, IVS_IP_LEN, xmlRsp);
        GET_ELEM_VALUE_NUM_FOR_UINT("ReleaseTimeRemain", pNodeTemp, pPtzControlInfo->uiReleaseTimeRemain, xmlRsp);
    }
	return iRet;
}

// 增加预置位
IVS_INT32 CPtzControl::AddPTZPreset (const IVS_CHAR* pCameraCode, const IVS_CHAR* pPresetName, IVS_UINT32* uiPresetIndex)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pPresetName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(uiPresetIndex, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::AddPTZPresetGetXML(pCameraCode, pPresetName, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"add ptz get xml is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_ADD_PRESET_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chAddPTZDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chAddPTZDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chAddPTZDomaCode);
	// 返回xml
	std::string strRsp;
	// 发送操作失败，直接返回
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "add ptz preset", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}

	// 解析XML查询请求
	iRet = PtzControlProcess::AddPTZPresetParseXML(strRsp, uiPresetIndex);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"parse xml to value is failed","failed");
		return iRet;
	}

	return iRet;
}

// 删除预置位
IVS_INT32 CPtzControl::DelPTZPreset (const IVS_CHAR* pCameraCode, IVS_UINT32 uiPresetIndex)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	
	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::DelPTZPresetGetXML(pCameraCode, uiPresetIndex, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"del ptz preset GetXML is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_PRESET_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDelPTZDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDelPTZDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chDelPTZDomaCode);
	// 返回xml
	std::string strRsp;
	// 发送操作失败，直接返回
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "del ptz preset", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}

	return iRet;
}

// 修改预置位
IVS_INT32 CPtzControl::ModPTZPreset (const IVS_CHAR* pCameraCode, const IVS_PTZ_PRESET* pPTZPreset)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pPTZPreset, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::ModPTZPresetGetXML(pCameraCode, pPTZPreset, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"mod ptz preset get xml is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_MOD_PRESET_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chModPTZDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chModPTZDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chModPTZDomaCode);
	// 返回xml
	std::string strRsp;
	// 发送操作失败，直接返回
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "mod ptz preset", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}

	return iRet;
}

// 查询预置位列表
IVS_INT32 CPtzControl::GetPTZPresetList (const IVS_CHAR* pCameraCode, IVS_PTZ_PRESET* pPTZPresetList, IVS_UINT32 uiBufferSize, IVS_UINT32* uiPTZPresetNum)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pPTZPresetList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(uiPTZPresetNum, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::GetPTZPresetListGetXML(pCameraCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"get list is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_PRESET_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chGetPTZListDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chGetPTZListDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chGetPTZListDomaCode);
	// 返回xml
	std::string strRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "get list", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}

	// 解析XML查询请求
	iRet = PtzControlProcess::GetPTZPresetListParseXML(strRsp, pPTZPresetList, uiBufferSize, uiPTZPresetNum);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"parse xml to value is failed","failed");
		return iRet;
	}

	return iRet;
}

// 设置看守位
IVS_INT32 CPtzControl::SetGuardPos (const IVS_CHAR* pCameraCode, const IVS_GUARD_POS_INFO* pGuardPosInfo)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pGuardPosInfo, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::SetGuardPosGetXML(pCameraCode, pGuardPosInfo, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"set guardpos get xml is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_SET_GUARD_POSITION_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chSetGuardDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chSetGuardDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chSetGuardDomaCode);
	// 返回xml
	std::string strRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "set guardpos", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}

	return iRet;
}


// 删除看守位
IVS_INT32 CPtzControl::DelGuardPos (const IVS_CHAR* pCameraCode, IVS_UINT32 uiPresetIndex)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::DelGuardPosGetXML(pCameraCode, uiPresetIndex, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"set guardpos get xml is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_GUARD_POSITION_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDelGuardDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDelGuardDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chDelGuardDomaCode);
	// 返回xml
	std::string strRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "set guardpos", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}

	return iRet;
}


// 获取看守位
IVS_INT32 CPtzControl::GetGuardPos (const IVS_CHAR* pCameraCode, IVS_GUARD_POS_INFO* pGuardPosInfo)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pGuardPosInfo, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::GetGuardPosGetXML(pCameraCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"get guardpos get xml is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_GUARD_POSITION_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chGetGuardDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chGetGuardDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chGetGuardDomaCode);
	// 返回xml
	std::string strRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "get guardpos", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}

	// 解析XML查询请求
	iRet = PtzControlProcess::GetGuardPosParseXML(strRsp, pGuardPosInfo);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"parse xml to value is failed","failed");
		return iRet;
	}

	return iRet;
}

// 增加巡航轨迹
IVS_INT32 CPtzControl::AddCruiseTrack (const IVS_CHAR* pCameraCode, const IVS_CRUISE_TRACK* pCuriseTrackInfo, IVS_UINT32* uiTrackIndex)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCuriseTrackInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(uiTrackIndex, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::AddCruiseTrackGetXML(pCameraCode, pCuriseTrackInfo, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"add CruiseTrack get xml is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_ADD_CRUISE_TRACK_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chAddCruiseDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chAddCruiseDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chAddCruiseDomaCode);
	// 返回xml
	std::string strRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "add CruiseTrack", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}

	// 解析XML查询请求
	iRet = PtzControlProcess::AddCruiseTrackGetParseXML(strRsp, uiTrackIndex);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"parse xml to value is failed","failed");
		return iRet;
	}

	return iRet;
}

// 删除巡航轨迹
IVS_INT32 CPtzControl::DelCruiseTrack (const IVS_CHAR* pCameraCode, IVS_UINT32 uiTrackIndex, IVS_UINT32 uiCruiseType)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::DelCruiseTrackGetXML(pCameraCode, uiTrackIndex, uiCruiseType, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"del CruiseTrack GetXML is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_CRUISE_TRACK_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDelCruiseDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDelCruiseDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chDelCruiseDomaCode);
	// 返回xml
	std::string strRsp;
	// 发送操作失败，直接返回
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "del CruiseTrack", "SendCmd operation succeed,failed = %d", iRet);
		return iRet;
	}

	return iRet;
}

// 修改巡航轨迹
IVS_INT32 CPtzControl::ModCruiseTrack (const IVS_CHAR* pCameraCode, const IVS_CRUISE_TRACK* pCuriseTrackInfo)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCuriseTrackInfo, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::ModCruiseTrackGetXML(pCameraCode, pCuriseTrackInfo, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"mod CruiseTrack get xml is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_MOD_CRUISE_TRACK_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chModCruiseDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chModCruiseDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chModCruiseDomaCode);
	// 返回xml
	std::string strRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "mod CruiseTrack", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}

	return iRet;
}

// 查询巡航轨迹列表
IVS_INT32 CPtzControl::GetCruiseTrackList (const IVS_CHAR* pCameraCode, IVS_CRUISE_TRACK* pCruiseTrackList, IVS_UINT32 uiBufferSize, IVS_UINT32* uiCruiseTrackNum)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCruiseTrackList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(uiCruiseTrackNum, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::GetCruiseTrackListGetXML(pCameraCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"get list is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_CRUISE_TRACK_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chGetCruiseListDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chGetCruiseListDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chGetCruiseListDomaCode);
	// 返回xml
	std::string strRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get CruiseTrack List", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}

	// 解析XML查询请求
	iRet = PtzControlProcess::GetCruiseTrackListParseXML(strRsp, pCruiseTrackList, uiBufferSize, uiCruiseTrackNum);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"parse xml to value is failed","failed");
		return iRet;
	}

	return iRet;
}

// 查询巡航轨迹信息
IVS_INT32 CPtzControl::GetCruiseTrack (const IVS_CHAR* pCameraCode, IVS_INT32 uiTrackIndex, IVS_CRUISE_TRACK* pCruiseTrackInfo)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCruiseTrackInfo, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::GetCruiseTrackGetXML(pCameraCode, uiTrackIndex, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"get track is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_CRUISE_TRACK_DETAIL_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chGetCruiseDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chGetCruiseDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chGetCruiseDomaCode);
	// 返回xml
	std::string strRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "get track", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}

	// 解析XML查询请求
	iRet = PtzControlProcess::GetCruiseTrackParseXML(strRsp, pCruiseTrackInfo);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"parse xml to value is failed","failed");
		return iRet;
	}

	return iRet;
}

// 设置巡航计划
IVS_INT32 CPtzControl::SetCruisePlan (const IVS_CHAR* pCameraCode, const IVS_CRUISE_PLAN* pCruisePlan)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCruisePlan, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::SetCruisePlanGetXML(pCameraCode, pCruisePlan, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"SetCruisePlan get xml is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_SET_CRUISE_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chSetCruisePlanDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chSetCruisePlanDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chSetCruisePlanDomaCode);
	// 返回xml
	std::string strRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Ptz Control", "SendCmd operation succeed,rspCode = %d", iRet);
		return iRet;
	}

	return iRet;
}

// 查询巡航计划
IVS_INT32 CPtzControl::GetCruisePlan (const IVS_CHAR* pCameraCode, IVS_CRUISE_PLAN* pCruisePlan)
{
	CHECK_POINTER(pCameraCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCruisePlan, IVS_OPERATE_MEMORY_ERROR);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = PtzControlProcess::GetCruisePlanGetXML(pCameraCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"get CruiseTrack plan get xml is failed","failed");
		return iRet;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR * pReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_CRUISE_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReq);

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chGetCruisePlanDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chGetCruisePlanDomaCode);
	sendNssMsgInfo.SetCameraCode(chDevCode);
	sendNssMsgInfo.SetDomainCode(chGetCruisePlanDomaCode);
	// 返回xml
	std::string strRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	// 发送操作失败，直接返回
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	// NSS消息返回码错误，不解析数据，直接返回错误码
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetCruisePlan", "SendCmd operation failed,rspCode = %d", iRet);
		return iRet;
	}

	// 解析XML查询请求
	iRet = PtzControlProcess::GetCruisePlanParseXML(strRsp, pCruisePlan);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"parse xml to value is failed","failed");
		return iRet;
	}

	return iRet;
}
