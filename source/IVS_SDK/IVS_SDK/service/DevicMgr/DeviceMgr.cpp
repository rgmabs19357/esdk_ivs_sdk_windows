/********************************************************************
	filename	: 	DeviceMgr.cpp
	author		:	gwx151720
	created		:	2012/11/01	
	description	:	设备组管理类，提供给cu使用
	copyright	:	Copyright (C) 2012
	history		:	2012/11/01
*********************************************************************/
#include "DeviceMgr.h"
#include "NSSOperator.h"
#include "nss_mt.h"
#include "bp_log.h"
#include "UserMgr.h"
#include "ivs_xml.h"
#include "DeviceMgrXMLProcess.h"
#include "ToolHelp.h"
#include "XmlProcess.h"
#include "IVS_Trace.h"
#include "DomainRouteMgr.h"
#include "SDKInterface.h"
#include "../AlarmMgr/AlarmMgrXMLProcess.h"

const IVS_UINT32 STEP_INDEX = 1000;//查询摄像机/告警设备/语音设备列表时，每次步进1000	

bool bSendLocalCmdFail = false;
bool bSendExdomainCmdFail = false;
std::string strTmpDomainCode = "";

CDeviceMgr::CDeviceMgr(void)
    : m_pUserMgr(NULL)
    , bIsOutAlarm(0)//lint !e1729
    , m_uiVideoChannelTotalNum(0)
	, m_uiVideoChannelTotalNumByDom(0)
    , m_uiAlarmChannelTotalNum(0)
	, m_uiAlarmChannelTotalNumByDom(0)
{
	m_pMutexNVRLock = VOS_CreateMutex();
	m_pMutexRecordTaskLock = VOS_CreateMutex();
	//获取摄像机/告警/语音 列表相关数据锁
	m_pMutexCameraLock = VOS_CreateMutex();
	m_pMutexAlarmLock = VOS_CreateMutex();
    m_pVideoRetriveStatusInfoListMutex = VOS_CreateMutex();
    m_pAlarmRetriveStatusInfoListMutex = VOS_CreateMutex();
    m_VideoChannelRetriveThreadStatus = eM_Thread_Retrive_Status_Init;
    m_AlarmChannelRetriveThreadStatus = eM_Thread_Retrive_Status_Init;
}


CDeviceMgr::~CDeviceMgr(void)
{
	m_pUserMgr = NULL;

    try
	{  
		Clear();
		VOS_DestroyMutex(m_pMutexNVRLock);
		m_pMutexNVRLock = NULL;

		VOS_DestroyMutex(m_pMutexCameraLock);
		m_pMutexCameraLock = NULL;

		VOS_DestroyMutex(m_pMutexAlarmLock);
		m_pMutexAlarmLock = NULL;

		//VOS_DestroyMutex(m_pMutexAudioLock);
		//m_pMutexAudioLock = NULL;

		VOS_DestroyMutex(m_pMutexRecordTaskLock);
		m_pMutexRecordTaskLock = NULL;

        VOS_DestroyMutex(m_pVideoRetriveStatusInfoListMutex);
        m_pVideoRetriveStatusInfoListMutex = NULL;


        VOS_DestroyMutex(m_pAlarmRetriveStatusInfoListMutex);
        m_pAlarmRetriveStatusInfoListMutex = NULL;
		//m_pGetCamerasThread = NULL;


		//m_pGetAlarmsThread = NULL;
		//m_pGetAudiosThread = NULL;
	}
    catch (...)
    {
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "~CDeviceMgr fail", "NA");//lint !e1551
	}
}


void CDeviceMgr::Clear()
{
    // 注意要清空所有申请的链表
    (void)VOS_MutexLock(m_pMutexCameraLock);
	m_vecAllCameraDev.clear();
	(void)VOS_MutexUnlock(m_pMutexCameraLock);

    (void)VOS_MutexLock(m_pMutexAlarmLock);
    m_vecAllAlarmDev.clear();
    (void)VOS_MutexUnlock(m_pMutexAlarmLock);

	// 清理m_DeviceInfiListSelfMap；
	(void)VOS_MutexLock(m_pMutexNVRLock);
	DEVICE_BRIEF_INFO_LIST_MAP_ITER device_list_map_iter;
	device_list_map_iter = m_DeviceInfoListMap.begin();
	while (device_list_map_iter != m_DeviceInfoListMap.end())
	{
		DEVICE_BRIEF_INFO_LIST *pList = dynamic_cast<DEVICE_BRIEF_INFO_LIST*>(device_list_map_iter->second);//lint !e611
		if (NULL == pList)
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "Clear", "pList = NULL");
			(void)VOS_MutexUnlock(m_pMutexNVRLock);
			return;
		}
		//遍历列表中的所有结构并删除之
		IVS_DEVICE_BRIEF_INFO* pDeviceBriefInfo = NULL;
		DEVICE_BRIEF_INFO_LIST_ITER InfoIter = pList->begin();
		DEVICE_BRIEF_INFO_LIST_ITER InfoIterEnd = pList->end();
		for (; InfoIter != InfoIterEnd; InfoIter++)
		{
			pDeviceBriefInfo = (IVS_DEVICE_BRIEF_INFO*)(*InfoIter);
			if (NULL == pDeviceBriefInfo)
			{
				BP_RUN_LOG_ERR(IVS_FAIL, "Clear", "pDeviceBriefInfo = NULL");
				(void)VOS_MutexUnlock(m_pMutexNVRLock);
				return;
			}
			IVS_DELETE(pDeviceBriefInfo);
		}

		IVS_DELETE(pList);
		m_DeviceInfoListMap.erase(device_list_map_iter++);
	}

	m_DeviceInfoListMap.clear();
	(void)VOS_MutexUnlock(m_pMutexNVRLock);
}

void CDeviceMgr::SetUserMgr( CUserMgr *pUserMgr)
{
	m_pUserMgr = pUserMgr;
}

// 增加设备
IVS_INT32 CDeviceMgr::AddDevice(const IVS_CHAR*              pDomainCode, 
                                IVS_UINT32                   uiDeviceNum, 
                                const IVS_DEVICE_OPER_INFO*  pDevList,
                                IVS_DEVICE_OPER_RESULT_LIST* pResultList,
                                IVS_UINT32 uiResultBufferSize)const
{
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDevList, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pResultList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    // 对设备的buffersize进行判断
    IVS_UINT32 uiAllocNum = ((uiResultBufferSize - sizeof(IVS_DEVICE_OPER_RESULT_LIST))/sizeof(IVS_DEVICE_OPER_RESULT))+1;
    if (uiDeviceNum != uiAllocNum)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "uiAllocNum fail", "uiDeviceNum=%u,uiAllocNum=%u", uiDeviceNum, uiAllocNum);
        return IVS_PARA_INVALID;
    }

	// 判断是集群或堆叠
	IVS_DOMAIN_ROUTE stDomainRoute;
    memset(&stDomainRoute, 0, sizeof(stDomainRoute));

	std::string strDomainCode = pDomainCode;
	CDomainRouteMgr& oDomainRoute = m_pUserMgr->GetDomainRouteMgr();
	IVS_INT32 iRet = oDomainRoute.GetDomainRoutebyCode(strDomainCode, stDomainRoute);
	// 集群情况
	if (2 == stDomainRoute.uiDomainType)
	{
		iRet = AddDeviceCluster(pDomainCode, uiDeviceNum, pDevList, pResultList);

		return iRet;
	}
	// 堆叠情况、单机
	iRet = AddDevicePile(pDomainCode, uiDeviceNum, pDevList, pResultList);

	return iRet;
}

// 堆叠下新增设备
IVS_INT32 CDeviceMgr::AddDevicePile(const IVS_CHAR* pDomainCode, 
	IVS_UINT32 uiDeviceNum, 
	const IVS_DEVICE_OPER_INFO* pDevList, 
	IVS_DEVICE_OPER_RESULT_LIST* pResultList)const
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pResultList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_UINT32 uiNum = 0;
	IVS_INT32 iRet = 0;
	// modify by limingjie 2013.6.27
	// 在返回的链表中，给设备数赋值
	pResultList->uiNum = uiDeviceNum;

	while(uiNum < uiDeviceNum)
	{
		IVS_CHAR cNVRCode[IVS_NVR_CODE_LEN + 1] = {0};
		if (0 == strcmp(pDevList[uiNum].stDeviceCfg.cNvrCode, ""))
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "pDevList->stDeviceCfg.cNvrCode fail", "CU send Para fail");
			return IVS_PARA_INVALID;
		}
		if (!CToolsHelp::Memcpy(cNVRCode, IVS_NVR_CODE_LEN, pDevList[uiNum].stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cNVRCode, IVS_NVR_CODE_LEN, pDevList->stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		
		/************************************************************************
		** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
		**              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
		************************************************************************/
		// 初始化密钥交换类对象,完成密钥交换
		CSDKSecurityClient oSecurityClient;
		std::string strLinkID;
		int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, pDomainCode, strLinkID, "SCU", "", cNVRCode);
		if (IVS_SUCCEED != iMultiExchangeKey)
		{
			BP_RUN_LOG_ERR(iMultiExchangeKey, "AddDevice fail", "oSecurityClient Init failed");
			// modify by limingjie 2013.6.27  DTS2013062507914 
			// 不需要直接返回错误，在返回的链表中填入相对应的值，跳出这一轮循环，继续操作。
			pResultList->stCodeInfo[uiNum].uiResult = (IVS_UINT32)iMultiExchangeKey;
			pResultList->stCodeInfo[uiNum].uiSequence = pDevList[uiNum].uiSequence;
			BP_DBG_LOG("uiSequence = %d",pResultList->stCodeInfo[uiNum].uiSequence);
			(void)CToolsHelp::Memcpy(pResultList->stCodeInfo[uiNum].cCode, IVS_DEV_CODE_LEN, pDevList[uiNum].stDeviceCfg.stDeviceInfo.cCode, IVS_DEV_CODE_LEN);
			(void)CToolsHelp::Memcpy(pResultList->stCodeInfo[uiNum].cNvrCode, IVS_NVR_CODE_LEN, pDevList[uiNum].stDeviceCfg.cNvrCode, IVS_NVR_CODE_LEN);
			uiNum++;
			continue;
		}
		// 加密数据后拼装
		std::string strDevPWDCipher;
		std::string strDevRegPWDCipher;
		char cDevPasswdTemp[IVS_PWD_LEN + 1] = {0};
		char cDevRegPWDTemp[IVS_PWD_LEN + 1] = {0};
		if (0 != strcmp(pDevList[uiNum].stDeviceCfg.cDevPasswd, ""))
		{
			// 设备登录密码
			if (!CToolsHelp::Memcpy(cDevPasswdTemp, IVS_PWD_LEN, pDevList[uiNum].stDeviceCfg.cDevPasswd, IVS_PWD_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevPasswdTemp, IVS_PWD_LEN, pDevList->stDeviceCfg.cDevPasswd, IVS_PWD_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevPasswdTemp, strDevPWDCipher))
			{
				BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "AddDevice fail", "Encrypt String failed");
				return IVS_DATA_ENCRYPT_ERROR;
			}
		}
		
		if (0 != strcmp(pDevList[uiNum].stDeviceCfg.cDevRegPasswd, ""))
		{
			// 设备向平台密码
			if (!CToolsHelp::Memcpy(cDevRegPWDTemp, IVS_PWD_LEN, pDevList[uiNum].stDeviceCfg.cDevRegPasswd, IVS_PWD_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevRegPWDTemp, IVS_PWD_LEN, pDevList->stDeviceCfg.cDevRegPasswd, IVS_PWD_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevRegPWDTemp, strDevRegPWDCipher))
			{
				BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "Set MainDevConfig", "Encrypt String failed");
				return IVS_DATA_ENCRYPT_ERROR;
			}
		}

		CXml xml;
		iRet = CDeviceMgrXMLProcess::AddDevicePileGetXML(pDomainCode, pDevList, strDevPWDCipher, strDevRegPWDCipher, uiNum, xml);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "AddDevice GetXML fail", "NA");
			return iRet;
		}
		// 获取流到pReq中
		IVS_UINT32 uLen = 0;
		const IVS_CHAR* pReq = xml.GetXMLStream(uLen);
		CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR); 
		BP_RUN_LOG_INF("AddDevice", "send XML = %s", pReq);
		std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

		//创建要发送的CMD，拼装了NSS消息头
		CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_ADD_MAIN_DEV_REQ, pReq, strSecurityTransID);
		CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
		//发送消息
		CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
		//CCmd *pCmdRsp = CDispatcher::instance().dispatcherSyncMsgOut(pCmd, 120);
		CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
		const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
		HW_DELETE(pCmdRsp);

		if (IVS_SMU_DEV_ADD_ALL_FAIL == iRet || IVS_SMU_DEV_ADD_PART_SUCCEED == iRet || IVS_SUCCEED == iRet)
		{
			CXml xmlRsp;
			if (!xmlRsp.Parse(pRsp))
			{
				BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "xmlRsp.Parse(pRsp) fail", "NA");
				IVS_DELETE(pRsp, MUILI);
				return IVS_OPERATE_MEMORY_ERROR;
			}
			iRet = CDeviceMgrXMLProcess::AddDevicePileParseXML(pResultList, uiNum, xmlRsp);
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "AddDevice ParseXML fail", "NA");
				IVS_DELETE(pRsp, MUILI);
				return iRet;
			}
			IVS_DELETE(pRsp, MUILI);
			uiNum++;
			continue;
		}
		IVS_DELETE(pRsp, MUILI);
		return iRet;
	}

	return iRet;
}

// 集群下新增设备
IVS_INT32 CDeviceMgr::AddDeviceCluster(const IVS_CHAR* pDomainCode, IVS_UINT32 uiDeviceNum, const IVS_DEVICE_OPER_INFO* pDevList, IVS_DEVICE_OPER_RESULT_LIST* pResultList)const
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pResultList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");


	std::vector<std::string> m_vecDevPasswd;// 集群下设备密码密文
	std::vector<std::string> m_vecDevRegPWD;// 集群下设备平台密码密文
    m_vecDevPasswd.clear();
    m_vecDevRegPWD.clear();
	/************************************************************************
	** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
	**              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
	************************************************************************/
	// 初始化密钥交换类对象,完成密钥交换
	CSDKSecurityClient oSecurityClient;
	std::string strLinkID;
	int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, pDomainCode, strLinkID, "CMU", "", "", BP::DOMAIN_CLUSTER);
	if (IVS_SUCCEED != iMultiExchangeKey)
	{
		BP_RUN_LOG_ERR(iMultiExchangeKey, "AddDevice fail", "oSecurityClient Init failed");
		return iMultiExchangeKey;
	}
	IVS_UINT32 uiNum = 0;
	IVS_INT32 iRet = 0;
	while (uiNum < uiDeviceNum)
	{
		// 加密数据后拼装
		std::string strDevPWDCipher;
		std::string strDevRegPWDCipher;
		char cDevPasswdTemp[IVS_PWD_LEN + 1] = {0};
		char cDevRegPWDTemp[IVS_PWD_LEN + 1] = {0};
		if (0 != strcmp(pDevList[uiNum].stDeviceCfg.cDevPasswd, ""))
		{
			// 设备登录密码
			if (!CToolsHelp::Memcpy(cDevPasswdTemp, IVS_PWD_LEN, pDevList[uiNum].stDeviceCfg.cDevPasswd, IVS_PWD_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevPasswdTemp, IVS_PWD_LEN, pDevList->stDeviceCfg.cDevPasswd, IVS_PWD_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevPasswdTemp, strDevPWDCipher))
			{
				BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "AddDevice fail", "Encrypt String failed");
				return IVS_DATA_ENCRYPT_ERROR;
			}
		}
		m_vecDevPasswd.push_back(strDevPWDCipher);

		if (0 != strcmp(pDevList[uiNum].stDeviceCfg.cDevRegPasswd, ""))
		{
			// 设备向平台密码
			if (!CToolsHelp::Memcpy(cDevRegPWDTemp, IVS_PWD_LEN, pDevList[uiNum].stDeviceCfg.cDevRegPasswd, IVS_PWD_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cDevRegPWDTemp, IVS_PWD_LEN, pDevList->stDeviceCfg.cDevRegPasswd, IVS_PWD_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevRegPWDTemp, strDevRegPWDCipher))
			{
				BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "Set MainDevConfig", "Encrypt String failed");
				return IVS_DATA_ENCRYPT_ERROR;
			}
		}
		m_vecDevRegPWD.push_back(strDevRegPWDCipher);
		uiNum++;
	}
	// 增加设备组装xml
	CXml xmlReq;
	iRet = CDeviceMgrXMLProcess::AddDeviceClusterGetXML(pDomainCode, uiDeviceNum, pDevList, m_vecDevPasswd, m_vecDevRegPWD, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "XML.AddDevice Info GetXML is failed", "NA");
		return iRet;
	}
	// 获取流到pReq中
	IVS_UINT32 uLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR); 
	BP_RUN_LOG_INF("AddDevice", "send XML = %s", pReq);
	std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

	//创建要发送的CMD，拼装了NSS消息头
	CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_ADD_MAIN_DEV_REQ, pReq, strSecurityTransID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
	//发送消息
	//CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CCmd *pCmdRsp = CDispatcher::instance().dispatcherSyncMsgOut(pCmd, 120);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	const IVS_CHAR* pRsp = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);
	CHECK_POINTER(pRsp, iRet);
	if (IVS_SMU_DEV_ADD_ALL_FAIL == iRet || IVS_SMU_DEV_ADD_PART_SUCCEED == iRet || IVS_SUCCEED == iRet)
	{
		CXml xmlRsq;
		if (xmlRsq.Parse(pRsp))
		{
			IVS_UINT32 uiTotal = 0;
			iRet = CDeviceMgrXMLProcess::GetDeviceListNum(xmlRsq, uiTotal);
			pResultList->uiNum = uiTotal;
			if (pResultList->uiNum == uiDeviceNum)
			{
				iRet = CDeviceMgrXMLProcess::AddDeviceClusterPraseXML(xmlRsq, pResultList, (IVS_INT32)uiDeviceNum);
			}
			else
			{
				BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Add Device fail", "pResultList->uiNum != uiDeviceNum fail");
				IVS_DELETE(pRsp, MUILI);
				return IVS_SMU_XML_INVALID;
			}
		}
		IVS_DELETE(pRsp, MUILI);
		return IVS_SUCCEED;
	}

	IVS_DELETE(pRsp, MUILI);
	return iRet;
}


// 删除设备
IVS_INT32 CDeviceMgr::DeleteDevice(const IVS_CHAR*  pDomainCode, IVS_UINT32 uiDeviceNum, const IVS_DEVICE_CODE* pDevCodeList , IVS_RESULT_LIST* pResultList , IVS_UINT32 uiResultBufferSize)const
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevCodeList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pResultList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_UINT32 uiTotallDevSize = uiResultBufferSize - sizeof(IVS_RESULT_LIST);
	IVS_UINT32 uiAllocNum = uiTotallDevSize / sizeof(IVS_RESULT_INFO);
	uiAllocNum =uiAllocNum + 1;

    if (uiDeviceNum != uiAllocNum)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "uiAllocNum fail", "uiDeviceNum=%u,uiAllocNum=%u", uiDeviceNum, uiAllocNum);
        return IVS_PARA_INVALID;
    }

	CXml xmlReq;	
    IVS_INT32 iRet = CDeviceMgrXMLProcess::DeleteDeviceGetXML(uiDeviceNum, pDevCodeList, xmlReq, pDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS_SUCCEED != Delete Device GetXML", "NA");
		return iRet;
	}

	IVS_UINT32 xmlLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);	
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	BP_RUN_LOG_INF("DeleteDevice", "send XML = %s", pReq);

	// 构造带域的请求消息，并发送
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_DEL_MAIN_DEV_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(pDomainCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect, 120, 1);

	BP_RUN_LOG_INF("DeleteDevice", "revice XML = %s", strpRsp.c_str());
    if (IVS_SUCCEED == iRet)
    {
        CXml xmlRsp;
        iRet = IVS_XML_INVALID;
        if (xmlRsp.Parse(strpRsp.c_str()))
        {
            iRet = IVS_SMU_XML_INVALID;
			IVS_UINT32 uitotal = 0;
            iRet = CDeviceMgrXMLProcess::GetDeviceListNum(xmlRsp, uitotal);
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "Delete Device fail", "Get DeviceListNum fail");
				return IVS_SMU_XML_INVALID;
			}
			pResultList->uiNum = uitotal;
            if (pResultList->uiNum == uiDeviceNum)
            {
                iRet = CDeviceMgrXMLProcess::DeleteDeviceParseXML(pResultList, uiResultBufferSize, xmlRsp);
            }  
        }
    }

	return iRet;
}

// 查询设备列表
IVS_INT32 CDeviceMgr::QueryDeviceList(IVS_UINT32 uiDeviceType, 
									const IVS_CHAR* pDomainCode,
                                    const IVS_QUERY_UNIFIED_FORMAT *pQueryFormat,
                                    IVS_VOID*                       pBuffer, 
                                    IVS_UINT32                      uiBufferSize)
{        
    CHECK_POINTER(pQueryFormat, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pBuffer,      IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDomainCode,  IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    //对fromindex和toindex进行判断
    if (pQueryFormat->stIndex.uiToIndex < pQueryFormat->stIndex.uiFromIndex)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Query Device List fail", "uiToIndex < uiFromIndex,uiToIndex=%u,uiFromIndex=%u", pQueryFormat->stIndex.uiToIndex, pQueryFormat->stIndex.uiFromIndex);
        return IVS_PARA_INVALID;
    }

    IVS_UINT32 uiReqNum = (pQueryFormat->stIndex.uiToIndex - pQueryFormat->stIndex.uiFromIndex) + 1;
    IVS_INT32 iRet = CheckBufferSize(uiDeviceType, uiReqNum , uiBufferSize);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "IVS_SUCCEED != CheckBufferSize(uiDeviceType, uiReqNum)", "uiDeviceType=%u,uiReqNum=%u", uiDeviceType, uiReqNum);
        return iRet;
    }

    switch (uiDeviceType)
    {
    case DEVICE_TYPE_MAIN:
        {
            IVS_DEVICE_BRIEF_INFO_LIST* pMainList = (IVS_DEVICE_BRIEF_INFO_LIST*) pBuffer;
            return GetMainDeviceList(pDomainCode,pQueryFormat, pMainList, uiBufferSize);
        }        
    default:
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "Device Type is not right value","%u", uiDeviceType);
			break;
    }

    return IVS_XML_INVALID;
}
//取得用户拥有设备的域列表
IVS_INT32 CDeviceMgr::GetDomainListByUserID(DOMAIN_VEC& DmList)const
{
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_SUCCEED;
	CXml xmlReq;
	if(IVS_SUCCEED != CDeviceMgrXMLProcess::GetDomainListGetXml(m_pUserMgr->GetUserID(),xmlReq))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "GetDomainListGetXml() Failed", "NA");
		return IVS_FAIL;
	}
    std::string strSMULinkID;
    iRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get LocalDomainLinkID failed", "NA");
        return iRet;
    }
       CXml xmlRsp;
	iRet = CXmlProcess::GetCommBuildRet(xmlReq, xmlRsp, NSS_GET_DOMAIN_LIST_BY_USER_REQ, BUILSTRING, strSMULinkID);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get CommBuildRet fail", "NA");
		return iRet;
	}
	iRet = CDeviceMgrXMLProcess::GetDomainListParseXml(xmlRsp, DmList);

	return iRet;
}
// 获取设备列表
IVS_INT32 CDeviceMgr::GetDeviceList(IVS_UINT32 uiDeviceType,
	const IVS_INDEX_RANGE* pIndexRange,
	IVS_VOID* pDeviceList,
	IVS_UINT32 uiBufferSize)
{
	CHECK_POINTER(pIndexRange, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDeviceList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 对fromindex和toindex进行判断
	if (0 == pIndexRange->uiToIndex || 0 == pIndexRange->uiFromIndex || pIndexRange->uiToIndex < pIndexRange->uiFromIndex )
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get Device List fail", "uiToIndex=%u,uiFromIndex=%u", pIndexRange->uiToIndex, pIndexRange->uiFromIndex);
		return IVS_PARA_INVALID;
	}

	// 检测输出大小是否正确
	IVS_UINT32 uiReqNum = (pIndexRange->uiToIndex - pIndexRange->uiFromIndex) + 1;
	IVS_INT32 iRet = CheckBufferSize(uiDeviceType, uiReqNum , uiBufferSize);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS_SUCCEED != CheckBufferSize(uiDeviceType, uiReqNum)", "uiDeviceType=%u,uiReqNum=%u", uiDeviceType, uiReqNum);
		return iRet;
	}

	// 只处理DEVICE_TYPE_CAMERA/DEVICE_TYPE_ALARM/DEVICE_TYPE_EXTEND_ALARM三种设备类型
	switch (uiDeviceType)
	{
	case DEVICE_TYPE_CAMERA:
		{
			IVS_CAMERA_BRIEF_INFO_LIST *pCameraList = (IVS_CAMERA_BRIEF_INFO_LIST *)pDeviceList;
			// iRet = GetCameras(pIndexRange,pCameraList,uiBufferSize);
            iRet = GetVideoChannelList(pIndexRange,pCameraList,uiBufferSize);
            
			break;
		}
	//delete by zwx211831, Date: 20140910
	//case DEVICE_TYPE_VOICE:
	//	{
	//		// IVS_AUDIO_BRIEF_INFO_LIST *pAudioList = (IVS_AUDIO_BRIEF_INFO_LIST *)pDeviceList;
	//		// iRet = GetAudios(pIndexRange,pAudioList,uiBufferSize);
	//		break;
	//	}
	case DEVICE_TYPE_ALARM:
		{
			// this->bIsOutAlarm = 0;
			IVS_ALARM_BRIEF_INFO_LIST *pAlarmList = (IVS_ALARM_BRIEF_INFO_LIST *)pDeviceList;
			// iRet = GetAlarms(pIndexRange,pAlarmList,uiBufferSize);
            iRet = GetAlarmChannelList(pIndexRange, pAlarmList, uiBufferSize, uiDeviceType);
			break;
		}
	case DEVICE_TYPE_EXTEND_ALARM:
		{
			// this->bIsOutAlarm = 1;
			IVS_ALARM_BRIEF_INFO_LIST *pAlarmList = (IVS_ALARM_BRIEF_INFO_LIST *)pDeviceList;
			// iRet = GetAlarms(pIndexRange,pAlarmList,uiBufferSize);
            iRet = GetAlarmChannelList(pIndexRange, pAlarmList, uiBufferSize, uiDeviceType);
			break;
		}
	default:
		{
			BP_RUN_LOG_ERR(IVS_DATA_INVALID, "Device Type Error","%u", uiDeviceType);
			iRet = IVS_DATA_INVALID;
			break;
		}
	}

	return iRet;

}


// 获取指定域设备列表
IVS_INT32 CDeviceMgr::GetDomainDeviceList(const IVS_CHAR* pDomainCode,
	IVS_UINT32 uiDeviceType,
	const IVS_INDEX_RANGE* pIndexRange,
	IVS_VOID* pDeviceList,
	IVS_UINT32 uiBufferSize)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pIndexRange, IVS_PARA_INVALID);
	CHECK_POINTER(pDeviceList, IVS_PARA_INVALID);

	// 对fromindex和toindex进行判断
	if (0 == pIndexRange->uiToIndex || 0 == pIndexRange->uiFromIndex || pIndexRange->uiToIndex < pIndexRange->uiFromIndex )
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get Device List fail", "uiToIndex=%u,uiFromIndex=%u", pIndexRange->uiToIndex, pIndexRange->uiFromIndex);
		return IVS_PARA_INVALID;
	}

	// 检测输出大小是否正确
	IVS_UINT32 uiReqNum = (pIndexRange->uiToIndex - pIndexRange->uiFromIndex) + 1;
	IVS_INT32 iRet = CheckBufferSize(uiDeviceType, uiReqNum , uiBufferSize);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS_SUCCEED != CheckBufferSize(uiDeviceType, uiReqNum)", "uiDeviceType=%u,uiReqNum=%u", uiDeviceType, uiReqNum);
		return iRet;
	}

	// 只处理DEVICE_TYPE_CAMERA/DEVICE_TYPE_ALARM/DEVICE_TYPE_EXTEND_ALARM三种设备类型
	switch (uiDeviceType)
	{
	case DEVICE_TYPE_CAMERA:
		{
			IVS_CAMERA_BRIEF_INFO_LIST *pCameraList = (IVS_CAMERA_BRIEF_INFO_LIST *)pDeviceList;
			iRet = GetDomainVideoChannelList(pDomainCode, pIndexRange, pCameraList, uiBufferSize);

			break;
		}
	case DEVICE_TYPE_ALARM:
		{
			IVS_ALARM_BRIEF_INFO_LIST *pAlarmList = (IVS_ALARM_BRIEF_INFO_LIST *)pDeviceList;
			iRet = GetDomainAlarmChannelList(pDomainCode, pIndexRange, pAlarmList, uiBufferSize, uiDeviceType);
			break;
		}
	case DEVICE_TYPE_EXTEND_ALARM:
		{
			IVS_ALARM_BRIEF_INFO_LIST *pAlarmList = (IVS_ALARM_BRIEF_INFO_LIST *)pDeviceList;
			iRet = GetDomainAlarmChannelList(pDomainCode, pIndexRange, pAlarmList, uiBufferSize, uiDeviceType);
			break;
		}
	default:
		{
			BP_RUN_LOG_ERR(IVS_DATA_INVALID, "Device Type Error","%u", uiDeviceType);
			iRet = IVS_DATA_INVALID;
			break;
		}
	}

	return iRet;

}

//// 获取摄像机线程入口
//long __stdcall CDeviceMgr::GetCamerasThreadEntry(void* pArg)
//{
//	IVS_DEBUG_TRACE("");
//
//	IVS_INT32 iRet = IVS_FAIL;
//	CDeviceMgr* pDeviceMgr = (CDeviceMgr*)pArg;
//	CHECK_POINTER(pDeviceMgr,IVS_OPERATE_MEMORY_ERROR);
//	CHECK_POINTER(pDeviceMgr->m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
//
//	(void)VOS_MutexLock(pDeviceMgr->m_pMutexCameraLock);
//	// 线程数据初始化
//	pDeviceMgr->mGetCamerasThreadStatus = eM_Thread_Init;
//	(void)VOS_MutexUnlock(pDeviceMgr->m_pMutexCameraLock);
//
//	// 取得用户拥有设备的域列表
//	iRet = pDeviceMgr->GetDomainListByUserID(pDeviceMgr->m_vecCameraDomain);
//	if(IVS_SUCCEED != iRet)
//	{
//		BP_RUN_LOG_ERR(iRet, "GetDomainList ByUserID return failed", "NA");
//		(void)VOS_MutexLock(pDeviceMgr->m_pMutexCameraLock);
//		// 线程异常返回
//		pDeviceMgr->mGetCamerasThreadStatus = eM_Thread_ExceptionRet;
//		pDeviceMgr->iThreadGetCameraRet = iRet;
//		(void)VOS_MutexUnlock(pDeviceMgr->m_pMutexCameraLock);
//		return iRet;
//	}
//
//	// 查询所有的摄像机
//	IVS_UINT32 iSize = pDeviceMgr->m_vecCameraDomain.size();	
//	// 遍历域编码列表
//	iRet = IVS_SUCCEED;
//	for(IVS_UINT32 i=0;i<iSize;i++)
//	{
//		IVS_INDEX_RANGE stRange;
//		// 每次从1开始
//		stRange.uiFromIndex = 1;
//		stRange.uiToIndex = STEP_INDEX;
//
//		CXml rspXml;
//		std::string strDomainCode = pDeviceMgr->m_vecCameraDomain.at(i);
//		// 一个域下面的所有个数
//		IVS_UINT32 uiSize = 0;
//		// 输出的范围
//		IVS_INDEX_RANGE outRange;
//		outRange.uiFromIndex = 1;
//		outRange.uiToIndex = 1;
//		IVS_UINT32 uiTotal = 0;
//		IVS_UINT32 uiNum = 0;
//		do 
//		{
//			IVS_INT32 iTempRet = pDeviceMgr->GetDeviceListByPage(strDomainCode.c_str(),stRange,DEVICE_TYPE_CAMERA,uiSize,outRange);
//			if (IVS_SUCCEED != iTempRet)
//			{
//                //add by s00191067 将具体错误码返回给CU
//                iRet = iTempRet;
//                // end
//				BP_RUN_LOG_ERR(iTempRet, "", "GetCameras ByPage From Domain[%s] Failed", strDomainCode.c_str());
//				break;
//			}
//
//			iRet = IVS_SUCCEED;
//
//			(void)VOS_MutexLock(pDeviceMgr->m_pMutexCameraLock);
//			// 正在接收
//			pDeviceMgr->mGetCamerasThreadStatus = eM_Thread_DataIsReving;
//			(void)VOS_MutexUnlock(pDeviceMgr->m_pMutexCameraLock);
//
//			// 获取到一个域下的摄像机总数量
//			stRange.uiFromIndex += STEP_INDEX;
//			stRange.uiToIndex += STEP_INDEX;
//
//			if (0 == uiNum)
//			{
//				uiTotal = uiSize;
//			}
////          TODO 是不是需要这样
//// 			if (stRange.uiToIndex >= uiTotal)
//// 			{
//// 				stRange.uiToIndex = uiTotal;
//// 			}
//
//			uiNum++;
//		} while (uiTotal > outRange.uiToIndex);
//	}
//
//	if (0 != pDeviceMgr->m_vecAllCameraDev.size())
//	{
//		iRet = IVS_SUCCEED;
//	}
//
//	(void)VOS_MutexLock(pDeviceMgr->m_pMutexCameraLock);
//
//	pDeviceMgr->iThreadGetCameraRet = iRet;
//	if(iRet == IVS_SUCCEED)
//	{
//		// 线程数据接收完成
//		pDeviceMgr->mGetCamerasThreadStatus = eM_Thread_DataIsReady;
//		bSendLocalCmdFail = false;
//		bSendExdomainCmdFail = false;
//	}
//	else
//	{
//		// 线程异常返回
//		pDeviceMgr->mGetCamerasThreadStatus = eM_Thread_ExceptionRet;
//		bSendLocalCmdFail = false;
//		bSendExdomainCmdFail = false;
//	}	
//	(void)VOS_MutexUnlock(pDeviceMgr->m_pMutexCameraLock);
//
//	return iRet;
//}

// 通过分页查找摄像机/告警设备/语音设备
IVS_INT32 CDeviceMgr::GetDeviceListByPage(const IVS_CHAR* pDomainCode,
	const IVS_INDEX_RANGE &stRange,
	const IVS_UINT32 &uiDevType,
	IVS_UINT32& uiSize,
	IVS_INDEX_RANGE& outRange)
{
	CHECK_POINTER(m_pUserMgr, IVS_PARA_INVALID);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	IVS_DEBUG_TRACE("");

	if (0 == strTmpDomainCode.compare(pDomainCode))
	{
		if (bSendLocalCmdFail && bSendExdomainCmdFail)
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "GetDeviceListByPage fail","Send Local Cmd and Send Exdomain Cmd All Fail.");
			return IVS_FAIL;
		}
	}
	else
	{
		strTmpDomainCode = pDomainCode;
		bSendLocalCmdFail = false;
		bSendExdomainCmdFail = false;
	}


	IVS_INT32 iRet = 0;
	for (IVS_INT32 i = 0; i != 2; i++)
	{
		CXml reqXml;
		iRet = CDeviceMgrXMLProcess::GetDeviceListGetXml(i,
			pDomainCode,
			uiDevType,
			stRange,
			reqXml);		

		// 返回的ret如果失败，则退出不再继续查找
		if(IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Get DeviceList GetXml return Error", "NA");
			return iRet;
		}

		// 请求xml转换成IVS_CHAR
		IVS_UINT32 xmlLen = 0;
		const IVS_CHAR* pReq = reqXml.GetXMLStream(xmlLen);	
		CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

        // 构造带域的请求消息，并发送
		CSendNssMsgInfo sendNssMsgInfo;
		sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
		sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
		sendNssMsgInfo.SetReqID(NSS_GET_USER_CHANNEL_LIST_REQ);
		sendNssMsgInfo.SetReqData(pReq);	

		//只有查询摄像机，才会向本地顶级域查询
		if (DEVICE_TYPE_CAMERA == uiDevType)
		{
			//本域,向登陆域发送
			if (0 == i)
			{
				// 获取本地域编码
				std::string strLocalDomainCode;
				CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
				m_pUserMgr->GetDomainCode(strLocalDomainCode);
				sendNssMsgInfo.SetDomainCode(strLocalDomainCode);
				if (bSendLocalCmdFail)
				{
					//不必再向该域发消息
					continue;
				}
			}
			else 
			{
				sendNssMsgInfo.SetDomainCode(pDomainCode);
				if (bSendExdomainCmdFail)
				{
					//不必再向该域发消息
					continue;
				}
			}
		}
		else
		{
			if (0 == i)
			{
				if (bSendLocalCmdFail)
				{
					//不必再向该域发消息
					continue;
				}
			}
			else
			{
				if (bSendExdomainCmdFail)
				{
					//不必再向该域发消息
					continue;
				}
			}
			sendNssMsgInfo.SetDomainCode(pDomainCode);
		}
				
		std::string strpRsp;
		IVS_INT32 iNeedRedirect = IVS_FAIL;
		iRet = m_pUserMgr->SendCmd(sendNssMsgInfo, strpRsp, iNeedRedirect);
		BP_RUN_LOG_INF("Get DeviceList Log", "revice XML = %s", strpRsp.c_str());
		if(IVS_SUCCEED != iRet && 0 == i)
		{
			//发送本域请求失败，本次刷新不必再发消息
			bSendLocalCmdFail = true;
			BP_RUN_LOG_ERR(iRet, "SendCmd to SMU Return Failed", "NA");
			// modify by limingjie lWX175483 2013-7-15 DTS2013071302702
			// 如果本域请求失败，继续执行外域请求。
			continue;
			//return  iRet;
		}

		CXml xmlRsp;
		iRet = IVS_SUCCEED;
		IVS_UINT32 uiTotal = 0;

		if(xmlRsp.Parse(strpRsp.c_str()))
		{
			switch (uiDevType)
			{
			case DEVICE_TYPE_CAMERA:
				{
					(void)VOS_MutexLock(m_pMutexCameraLock);		
					iRet = CDeviceMgrXMLProcess::GetCamerasParseXml(i,pDomainCode,xmlRsp,&m_vecAllCameraDev,uiTotal,outRange);
					(void)VOS_MutexUnlock(m_pMutexCameraLock);
					break;			
				}
			case DEVICE_TYPE_ALARM:
			case DEVICE_TYPE_EXTEND_ALARM:
				{
					(void)VOS_MutexLock(m_pMutexAlarmLock);		
					iRet = CDeviceMgrXMLProcess::GetAlarmsParseXml(pDomainCode,xmlRsp,&m_vecAllAlarmDev,uiTotal,outRange);
					(void)VOS_MutexUnlock(m_pMutexAlarmLock);
					break;
				}
			case DEVICE_TYPE_VOICE:
				{
					//(void)VOS_MutexLock(m_pMutexAudioLock);		
					//iRet = CDeviceMgrXMLProcess::GetAudiosParseXml(pDomainCode,xmlRsp,&m_vecAllAudioDev,uiTotal,outRange);
					//(void)VOS_MutexUnlock(m_pMutexAudioLock);
					break;
				}
			default:
				{
					iRet = IVS_PARA_INVALID; 
					BP_RUN_LOG_ERR(iRet, "GetDeviceListByPage fail","uiDevType[%d] is invalid",uiDevType);
					break;
				}
			}
			uiSize+=uiTotal;
		}
		else
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "GetDeviceListByPage fail","Send Exdomain Req Cmd Fail or Rsp Cmd is Fail.");
			bSendExdomainCmdFail = true;
		}


		//if (IVS_SUCCEED != iRet&&0 == i)
		//{
		//	BP_RUN_LOG_ERR(iRet, "Get DeviceList ByPage fail","RspXml[%s] from SMU is invalid",strpRsp.c_str());
		//	// modify by limingjie lWX175483 2013-7-15 DTS2013071302702
		//	// 如果本域请求失败，继续执行外域请求。
		//	continue;
		//	//return iRet;
		//}
		if (DEVICE_TYPE_EXTEND_ALARM == uiDevType)
		{
			break;
		}
	}
	
	return  iRet;
}

// 获取告警设备线程入口
//long __stdcall CDeviceMgr::GetAlarmsThreadEntry(void* pArg)
//{
//	CHECK_POINTER(pArg, IVS_OPERATE_MEMORY_ERROR);
//	IVS_DEBUG_TRACE("");
//
//	IVS_INT32 iRet = IVS_FAIL;
//	CDeviceMgr* pDeviceMgr = (CDeviceMgr*)pArg;
//	CHECK_POINTER(pDeviceMgr->m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
//
//	(void)VOS_MutexLock(pDeviceMgr->m_pMutexAlarmLock);
//	// 线程数据初始化
//	pDeviceMgr->mGetAlarmsThreadStatus = eM_Thread_Init;
//	(void)VOS_MutexUnlock(pDeviceMgr->m_pMutexAlarmLock);
//
//	// 取得用户拥有设备的域列表
//	iRet = pDeviceMgr->GetDomainListByUserID(pDeviceMgr->m_vecAlarmDomain);
//	if(IVS_SUCCEED != iRet)
//	{
//		BP_RUN_LOG_ERR(iRet, "GetDomainListByUserID return failed", "NA");
//		(void)VOS_MutexLock(pDeviceMgr->m_pMutexCameraLock);
//		// 线程异常返回
//		pDeviceMgr->mGetAlarmsThreadStatus = eM_Thread_ExceptionRet;
//		pDeviceMgr->iThreadGetAlarmRet = iRet;
//		(void)VOS_MutexUnlock(pDeviceMgr->m_pMutexCameraLock);
//		return iRet;
//	}
//
//	IVS_UINT32 uiDevType = 0;
//	if (0 == pDeviceMgr->bIsOutAlarm)
//	{
//		uiDevType = DEVICE_TYPE_ALARM;
//	}
//	if (1 == pDeviceMgr->bIsOutAlarm)
//	{
//		uiDevType = DEVICE_TYPE_EXTEND_ALARM;
//	}
//	// 查询所有的告警
//	IVS_UINT32 iSize = pDeviceMgr->m_vecAlarmDomain.size();	
//	iRet = IVS_SUCCEED;
//	// 遍历域编码列表
//	for(IVS_UINT32 i=0;i<iSize;i++)
//	{
//		IVS_INDEX_RANGE stRange;
//		// 每次从1开始
//		stRange.uiFromIndex = 1;
//		stRange.uiToIndex = STEP_INDEX;
//
//		CXml rspXml;
//		std::string strDomainCode = pDeviceMgr->m_vecAlarmDomain.at(i);
//		// 一个域下面的所有个数
//		IVS_UINT32 uiSize = 0;
//		// 输出的范围
//		IVS_INDEX_RANGE outRange;
//		outRange.uiFromIndex = 1;
//		outRange.uiToIndex = 1;
//		IVS_UINT32 uiTotal = 0;
//		IVS_UINT32 uiNum = 0;
//		do 
//		{
//			IVS_INT32 iTempRet = pDeviceMgr->GetDeviceListByPage(strDomainCode.c_str(),stRange,uiDevType,uiSize,outRange);
//			if (IVS_SUCCEED != iTempRet)
//			{
//				iRet = iTempRet;
//				BP_RUN_LOG_ERR(iTempRet, "GetDeviceListByPage Failed", "NA");
//				break;
//			}
//			iRet = IVS_SUCCEED;
//
//			(void)VOS_MutexLock(pDeviceMgr->m_pMutexAlarmLock);
//			// 正在接收
//			pDeviceMgr->mGetAlarmsThreadStatus = eM_Thread_DataIsReving;
//			(void)VOS_MutexUnlock(pDeviceMgr->m_pMutexAlarmLock);
//
//			// 获取到一个域下的摄像机总数量
//			stRange.uiFromIndex += STEP_INDEX;
//			stRange.uiToIndex += STEP_INDEX;
//
//			if (0 == uiNum)
//			{
//				uiTotal = uiSize;
//			}
//
//			uiNum++;
//		} while (uiTotal > outRange.uiToIndex);
//	}	
//	if (0 != pDeviceMgr->m_vecAllAlarmDev.size())
//	{
//		iRet = IVS_SUCCEED;
//	}
//
//	(void)VOS_MutexLock(pDeviceMgr->m_pMutexAlarmLock);
//	if(IVS_SUCCEED == iRet)
//	{
//		// 线程数据接收完成
//		pDeviceMgr->mGetAlarmsThreadStatus = eM_Thread_DataIsReady;
//	}
//	else
//	{
//		// 线程数接收异常
//		pDeviceMgr->mGetAlarmsThreadStatus = eM_Thread_ExceptionRet;
//	}
//	
//	pDeviceMgr->iThreadGetAlarmRet = iRet;
//	(void)VOS_MutexUnlock(pDeviceMgr->m_pMutexAlarmLock);
//
//	return iRet;
//}
// 获取语音设备线程入口
//long __stdcall CDeviceMgr::GetAudiosThreadEntry(void* pArg)
//{
//	CHECK_POINTER(pArg, IVS_OPERATE_MEMORY_ERROR);
//	IVS_DEBUG_TRACE("");
//	
//	IVS_INT32 iRet = IVS_FAIL;
//	CDeviceMgr* pDeviceMgr = (CDeviceMgr*)pArg;
//	CHECK_POINTER(pDeviceMgr->m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
//
//	(void)VOS_MutexLock(pDeviceMgr->m_pMutexAudioLock);
//	// 线程数据初始化
//	pDeviceMgr->mGetAudiosThreadStatus = eM_Thread_Init;
//	(void)VOS_MutexUnlock(pDeviceMgr->m_pMutexAudioLock);
//
//	// 取得用户拥有设备的域列表
//	iRet = pDeviceMgr->GetDomainListByUserID(pDeviceMgr->m_vecAudioDomain);
//	if(IVS_SUCCEED != iRet)
//	{
//		BP_RUN_LOG_ERR(iRet, "Get DomainList ByUserID() Failed", "NA");
//		(void)VOS_MutexLock(pDeviceMgr->m_pMutexAudioLock);
//		// 线程异常返回
//		pDeviceMgr->mGetAudiosThreadStatus = eM_Thread_ExceptionRet;
//		pDeviceMgr->iThreadGetAudioRet = iRet;
//		(void)VOS_MutexUnlock(pDeviceMgr->m_pMutexAudioLock);
//		return iRet;
//	}
//
//	// 遍历域编码列表，查询所有的语音设备
//	IVS_UINT32 iSize = pDeviceMgr->m_vecAudioDomain.size();	
//	iRet = IVS_FAIL;
//	for(IVS_UINT32 i=0;i<iSize;i++)
//	{
//		IVS_INDEX_RANGE stRange;
//		// 每次从1开始
//		stRange.uiFromIndex = 1;
//		stRange.uiToIndex = STEP_INDEX;
//
//		CXml rspXml;
//		std::string strDomainCode = pDeviceMgr->m_vecAudioDomain.at(i);
//		// 一个域下面的所有个数
//		IVS_UINT32 uiSize = 0;
//		// 输出的范围
//		IVS_INDEX_RANGE outRange;
//		outRange.uiFromIndex = 1;
//		outRange.uiToIndex = 1;
//		IVS_UINT32 uiTotal = 0;
//		IVS_UINT32 uiNum = 0;
//		do 
//		{
//			IVS_INT32 iTempRet = pDeviceMgr->GetDeviceListByPage(strDomainCode.c_str(),stRange,static_cast<IVS_UINT32>(DEVICE_TYPE_VOICE),uiSize,outRange);
//			if (IVS_SUCCEED != iTempRet)
//			{
//				BP_RUN_LOG_ERR(iTempRet, "Get Audios ThreadEntry fail","GetDeviceListByPage From Domain[%s] Failed",strDomainCode.c_str());
//				break;
//			}
//			iRet = IVS_SUCCEED;
//
//			(void)VOS_MutexLock(pDeviceMgr->m_pMutexAudioLock);
//			// 正在接收
//			pDeviceMgr->mGetAudiosThreadStatus = eM_Thread_DataIsReving;
//			(void)VOS_MutexUnlock(pDeviceMgr->m_pMutexAudioLock);
//
//			// 获取到一个域下的摄像机总数量
//			stRange.uiFromIndex += STEP_INDEX;
//			stRange.uiToIndex += STEP_INDEX;
//
//			if (0 == uiNum)
//			{
//				uiTotal = uiSize;
//			}
//
//			uiNum++;
//		} while (uiTotal >= outRange.uiToIndex);
//	}
//
//	(void)VOS_MutexLock(pDeviceMgr->m_pMutexAudioLock);
//	if(IVS_SUCCEED == iRet)
//	{
//		// 线程数据接收完成
//		pDeviceMgr->mGetAudiosThreadStatus = eM_Thread_DataIsReady;
//	}
//	else
//	{
//		pDeviceMgr->mGetAudiosThreadStatus = eM_Thread_ExceptionRet;
//	}	
//	pDeviceMgr->iThreadGetAudioRet = iRet;
//	(void)VOS_MutexUnlock(pDeviceMgr->m_pMutexAudioLock);
//
//	return iRet;
//}

IVS_INT32 CDeviceMgr::GetVideoChannelList(const IVS_INDEX_RANGE* pQueryRange,
    IVS_CAMERA_BRIEF_INFO_LIST* pList,
    IVS_UINT32 uiBufferSize)
{
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pQueryRange, IVS_PARA_INVALID);
    CHECK_POINTER(pList, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    memset(pList,0,uiBufferSize);
    // 初始化pList
    pList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
    pList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;

    // from 
    if (1 == pQueryRange->uiFromIndex)
    {
         IVS_INDEX_RANGE stRange;
         stRange.uiFromIndex = 1;
         stRange.uiToIndex = STEP_INDEX;
         m_uiVideoChannelTotalNum = 0;
        DOMAIN_VEC dmList;
        IVS_INT32 iRet = GetDomainListByUserID(dmList);
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet, "get domain list by user id", "fail to get domain list");
            return iRet;
        }
        std::string strTransID;
        std::string strTargetDomainCode;
        std::string strSendDomainCode;
        std::string strExdomainFlag;
        DOMAIN_VEC::iterator itor = dmList.begin();
        IVS_CHANNLELIST_RETRIVE_STATUS_INFO retriveStatusInfo;
        memset(&retriveStatusInfo, 0, sizeof(retriveStatusInfo));

        (void)VOS_MutexLock(m_pMutexCameraLock);
        m_vecAllCameraDev.clear();
        (void)VOS_MutexUnlock(m_pMutexCameraLock);

        (void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);
        m_VideoChannelRetriveStatusInfoList.clear();
        m_VideoChannelRetriveThreadStatus = eM_Thread_Retrive_Status_Init;
        (void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);

        for(; dmList.end() != itor; itor++)
        {
            strTargetDomainCode = *itor;
            for (int i = 0; i < 2; i++)
            {
                // 发送本域消息
                CXml reqVideoXml;
                strTransID = CMKTransID::Instance().GenerateTransID();

                if (0 == i)
                {
                    (void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_LOCAL, strTargetDomainCode.c_str(), DEVICE_TYPE_CAMERA, stRange, reqVideoXml);	
                    retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_LOCAL;
                    m_pUserMgr->GetDomainCode(strSendDomainCode);
                    strExdomainFlag = "Local Domain";
                }
                else
                {
                    (void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_OUT, strTargetDomainCode.c_str(), DEVICE_TYPE_CAMERA, stRange, reqVideoXml);	
                    retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_OUT;
                    strSendDomainCode = strTargetDomainCode;
                    strExdomainFlag = "ExDomain";
                }
                retriveStatusInfo.iSendTime = clock();
                iRet = m_pUserMgr->SendAsynMsg(strSendDomainCode.c_str(), NSS_GET_USER_CHANNEL_LIST_REQ, strTransID, &reqVideoXml);

				//modify by zwx211831, Date:20140709, 日志中消息发送的目标域编码打印有误
				BP_RUN_LOG_INF("process get user channel list,", "start to send video channel request,  page index:[%u - %u], target domain:[%s : %s], transID:[%s]", 
					stRange.uiFromIndex, stRange.uiToIndex, strExdomainFlag.c_str(), strTargetDomainCode.c_str(), strTransID.c_str());
				//end

                if (IVS_SUCCEED == iRet)
                {
                    retriveStatusInfo.iSessionID = m_pUserMgr->GetSessionID();
                    retriveStatusInfo.uiChannelType = DEVICE_TYPE_CAMERA;
                    retriveStatusInfo.iThreadStatus = eM_Thread_Retrive_Status_Init;
                    strncpy(retriveStatusInfo.szTargetDomainCode, strTargetDomainCode.c_str(), IVS_DOMAIN_CODE_LEN);
					retriveStatusInfo.iGetChannelType = eM_Get_Device_All;

                    CSDKInterface* pSingleSvrProxy = (CSDKInterface*)(m_pUserMgr->GetSingleSvrProxy());
                    if (NULL != pSingleSvrProxy)
                    {
                        pSingleSvrProxy->AddTransIDChannelListRetriveStatusMap(strTransID, retriveStatusInfo);
                    }

                    (void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);
                    m_VideoChannelRetriveStatusInfoList.push_back(retriveStatusInfo);
                    (void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);
                }
            }
        }

        (void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);
        IVS_UINT32 iStatusInfoListSize = m_VideoChannelRetriveStatusInfoList.size();
        (void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);
        if (iStatusInfoListSize == 0)
        {
            pList->uiTotal = 0;
            return IVS_SUCCEED;
        }

    }


    time_t currentTime = time(0); 
    IVS_INT32 iRet = IVS_SUCCEED;
    while(true) //lint !e716
    {
        switch(m_VideoChannelRetriveThreadStatus)
        {
        case eM_Thread_Retrive_Status_Retriving: // 数据正在获取中
            {
                BP_DBG_LOG("get user channel list, data is retriving.");
                (void)VOS_MutexLock(m_pMutexCameraLock);
                IVS_UINT32 uiChannelListSize = m_vecAllCameraDev.size();
                (void)VOS_MutexUnlock(m_pMutexCameraLock);
                if (uiChannelListSize < pQueryRange->uiToIndex)
                {
                    break;
                }
                iRet = GetVideoChannelInfoListFromCache(pQueryRange, pList);
                return iRet;
            }
        case eM_Thread_Retrive_Status_Ready:  // 数据获取已经结束
            BP_DBG_LOG("get user channel list, data is ready.");
            iRet = GetVideoChannelInfoListFromCache(pQueryRange, pList);
            return iRet;

        case eM_Thread_Retrive_Excepting:  // 出现异常, 但不影响已有数据
            BP_DBG_LOG("get user channel list, data is exception.");
            iRet = GetVideoChannelInfoListFromCache(pQueryRange, pList);
            return iRet;

        default:
            ;
        }
        VOS_Sleep(10);
        if (30 < time(0) - currentTime)  // 默认30秒超时
        {

            return IVS_NET_RECV_TIMEOUT;
        }
        
    }
}


IVS_INT32 CDeviceMgr::GetDomainVideoChannelList(const IVS_CHAR* pDomainCode,
	const IVS_INDEX_RANGE* pQueryRange,
	IVS_CAMERA_BRIEF_INFO_LIST* pList,
	IVS_UINT32 uiBufferSize)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pQueryRange, IVS_PARA_INVALID);
	CHECK_POINTER(pList, IVS_PARA_INVALID);

	IVS_INT32 iRet = IVS_FAIL;

	memset(pList, 0, uiBufferSize);
	// 初始化pList
	pList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
	pList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;

	// from 
	if (1 == pQueryRange->uiFromIndex)
	{
		IVS_INDEX_RANGE stRange;
		stRange.uiFromIndex = 1;
		stRange.uiToIndex = STEP_INDEX;
		m_uiVideoChannelTotalNumByDom = 0;

		std::string strTransID;
		std::string strSendDomainCode;
		std::string strExdomainFlag;

		IVS_CHANNLELIST_RETRIVE_STATUS_INFO retriveStatusInfo;
		memset(&retriveStatusInfo, 0, sizeof(retriveStatusInfo));

		(void)VOS_MutexLock(m_pMutexCameraLock);
		m_vecAllCamDevByDom.clear();
		(void)VOS_MutexUnlock(m_pMutexCameraLock);

		(void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);
		m_VideoChannelRetriveStatusInfoList.clear();
		m_VideoChannelRetriveThreadStatus = eM_Thread_Retrive_Status_Init;
		(void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);

		for (int i = 0; i < 2; i++)
		{
			// 发送本域消息
			CXml reqVideoXml;
			strTransID = CMKTransID::Instance().GenerateTransID();

			if (0 == i)
			{
				(void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_LOCAL, pDomainCode, DEVICE_TYPE_CAMERA, stRange, reqVideoXml);
				retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_LOCAL;
				m_pUserMgr->GetDomainCode(strSendDomainCode);
				strExdomainFlag = "Local Domain";
			}
			else
			{
				(void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_OUT, pDomainCode, DEVICE_TYPE_CAMERA, stRange, reqVideoXml);	
				retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_OUT;
				strSendDomainCode = pDomainCode;
				strExdomainFlag = "ExDomain";
			}
			retriveStatusInfo.iSendTime = clock();
			iRet = m_pUserMgr->SendAsynMsg(strSendDomainCode.c_str(), NSS_GET_USER_CHANNEL_LIST_REQ, strTransID, &reqVideoXml);

			BP_RUN_LOG_INF("process get user channel list,", "start to send video channel request,  page index:[%u - %u], target domain:[%s : %s], transID:[%s]", 
				stRange.uiFromIndex, stRange.uiToIndex, strExdomainFlag.c_str(), pDomainCode, strTransID.c_str());

			if (IVS_SUCCEED == iRet)
			{
				retriveStatusInfo.iSessionID = m_pUserMgr->GetSessionID();
				retriveStatusInfo.uiChannelType = DEVICE_TYPE_CAMERA;
				retriveStatusInfo.iThreadStatus = eM_Thread_Retrive_Status_Init;
				strncpy(retriveStatusInfo.szTargetDomainCode, pDomainCode, IVS_DOMAIN_CODE_LEN);
				retriveStatusInfo.iGetChannelType = eM_Get_Device_ByDomainCode;

				CSDKInterface* pSingleSvrProxy = (CSDKInterface*)(m_pUserMgr->GetSingleSvrProxy());
				if (NULL != pSingleSvrProxy)
				{
					pSingleSvrProxy->AddTransIDChannelListRetriveStatusMap(strTransID, retriveStatusInfo);
				}

				(void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);
				m_VideoChannelRetriveStatusInfoList.push_back(retriveStatusInfo);
				(void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);
			}
		}

		(void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);
		IVS_UINT32 iStatusInfoListSize = m_VideoChannelRetriveStatusInfoList.size();
		(void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);
		if (iStatusInfoListSize == 0)
		{
			pList->uiTotal = 0;
			return IVS_SUCCEED;
		}
	}

	time_t currentTime = time(0); 
	iRet = IVS_SUCCEED;
	while(true)
	{
		switch(m_VideoChannelRetriveThreadStatus)
		{
		case eM_Thread_Retrive_Status_Retriving: // 数据正在获取中
			{
				BP_DBG_LOG("get user channel list, data is retriving.");
				(void)VOS_MutexLock(m_pMutexCameraLock);
				IVS_UINT32 uiChannelListSize = m_vecAllCamDevByDom.size();
				(void)VOS_MutexUnlock(m_pMutexCameraLock);
				if (uiChannelListSize < pQueryRange->uiToIndex)
				{
					break;
				}
				iRet = GetVideoChannelInfoListFromCache(pQueryRange, pList, m_vecAllCamDevByDom);
				return iRet;
			}
		case eM_Thread_Retrive_Status_Ready:  // 数据获取已经结束
			BP_DBG_LOG("get user channel list, data is ready.");
			iRet = GetVideoChannelInfoListFromCache(pQueryRange, pList, m_vecAllCamDevByDom);
			return iRet;

		case eM_Thread_Retrive_Excepting:  // 出现异常, 但不影响已有数据
			BP_DBG_LOG("get user channel list, data is exception.");
			iRet = GetVideoChannelInfoListFromCache(pQueryRange, pList, m_vecAllCamDevByDom);
			return iRet;

		default:
			;
		}
		VOS_Sleep(10);
		if (30 < time(0) - currentTime)  // 默认30秒超时
		{
			return IVS_NET_RECV_TIMEOUT;
		}
	}
}


IVS_INT32 CDeviceMgr::GetAlarmChannelList(const IVS_INDEX_RANGE* pQueryRange,
    IVS_ALARM_BRIEF_INFO_LIST* pList,
    IVS_UINT32 uiBufferSize,
    IVS_UINT32 uiAlarmType)
{
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pQueryRange, IVS_PARA_INVALID);
    CHECK_POINTER(pList, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    memset(pList,0,uiBufferSize);
    // 初始化pList
    pList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
    pList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;

    // from 
    if (1 == pQueryRange->uiFromIndex)
    {
        IVS_INDEX_RANGE stRange;
        stRange.uiFromIndex = 1;
        stRange.uiToIndex = STEP_INDEX;
        m_uiAlarmChannelTotalNum = 0;
        DOMAIN_VEC dmList;
        IVS_INT32 iRet = GetDomainListByUserID(dmList);
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet, "get domain list by user id", "fail to get domain list");
            return iRet;
        }
        std::string strTransID;
        std::string strTargetDomainCode;
        //std::string strSendDomainCode;
        std::string strExdomainFlag;
        //m_pUserMgr->GetDomainCode(strLocalDomainCode);
        DOMAIN_VEC::iterator itor = dmList.begin();
        IVS_CHANNLELIST_RETRIVE_STATUS_INFO retriveStatusInfo;
        memset(&retriveStatusInfo, 0, sizeof(retriveStatusInfo));

        (void)VOS_MutexLock(m_pMutexAlarmLock);
        m_vecAllAlarmDev.clear();
        (void)VOS_MutexUnlock(m_pMutexAlarmLock);

        (void)VOS_MutexLock(m_pAlarmRetriveStatusInfoListMutex);
        m_AlarmChannelRetriveStatusInfoList.clear();
        m_AlarmChannelRetriveThreadStatus = eM_Thread_Retrive_Status_Init;
        (void)VOS_MutexUnlock(m_pAlarmRetriveStatusInfoListMutex);

        for(; dmList.end() != itor; itor++)
        {
            strTargetDomainCode = *itor;
            for (int i = 0; i < 2; i++)
            {
                // 发送本域消息
                CXml reqVideoXml;
                strTransID = CMKTransID::Instance().GenerateTransID();

                if (1== i && DEVICE_TYPE_EXTEND_ALARM == uiAlarmType)
                {
                    // 外部域告警设备不需要向外域（PCG）请求
                    continue;

                }

                if (0 == i)
                {
                    (void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_LOCAL, strTargetDomainCode.c_str(), uiAlarmType, stRange, reqVideoXml);	
                    retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_LOCAL;
                    strExdomainFlag = "Local Domain";
                }
                else
                {
                    (void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_OUT, strTargetDomainCode.c_str(), uiAlarmType, stRange, reqVideoXml);	
                    retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_OUT;
                    strExdomainFlag = "ExDomain";
                }
                retriveStatusInfo.iSendTime = clock();
                iRet = m_pUserMgr->SendAsynMsg(strTargetDomainCode.c_str(), NSS_GET_USER_CHANNEL_LIST_REQ, strTransID, &reqVideoXml);

                BP_RUN_LOG_INF("process get user channel list,", "start to send alarm[%u] request,  page index:[%u - %u], target domain:[%s : %s], transID:[%s]", 
                    uiAlarmType, stRange.uiFromIndex, stRange.uiToIndex, strExdomainFlag.c_str(), retriveStatusInfo.szTargetDomainCode, strTransID.c_str());

                if (IVS_SUCCEED == iRet)
                {
                    retriveStatusInfo.iSessionID = m_pUserMgr->GetSessionID();
                    retriveStatusInfo.uiChannelType = uiAlarmType;
                    retriveStatusInfo.iThreadStatus = eM_Thread_Retrive_Status_Init;
                    strncpy(retriveStatusInfo.szTargetDomainCode, strTargetDomainCode.c_str(), IVS_DOMAIN_CODE_LEN);
					retriveStatusInfo.iGetChannelType = eM_Get_Device_All;

                    CSDKInterface* pSingleSvrProxy = (CSDKInterface*)(m_pUserMgr->GetSingleSvrProxy());
                    if (NULL != pSingleSvrProxy)
                    {
                        pSingleSvrProxy->AddTransIDChannelListRetriveStatusMap(strTransID, retriveStatusInfo);
                    }

                    (void)VOS_MutexLock(m_pAlarmRetriveStatusInfoListMutex);
                    m_AlarmChannelRetriveStatusInfoList.push_back(retriveStatusInfo);
                    (void)VOS_MutexUnlock(m_pAlarmRetriveStatusInfoListMutex);
                }
            }
        }

        (void)VOS_MutexLock(m_pAlarmRetriveStatusInfoListMutex);
        IVS_UINT32 iStatusInfoListSize = m_AlarmChannelRetriveStatusInfoList.size();
        (void)VOS_MutexUnlock(m_pAlarmRetriveStatusInfoListMutex);
        if (iStatusInfoListSize == 0)
        {
            pList->uiTotal = 0;
            return IVS_SUCCEED;
        }

    }


    time_t currentTime = time(0); 
    IVS_INT32 iRet = IVS_SUCCEED;
    while(true) //lint !e716
    {
        switch(m_AlarmChannelRetriveThreadStatus)
        {
        case eM_Thread_Retrive_Status_Retriving: // 数据正在获取中
            {
                BP_DBG_LOG("get user channel list, data is retriving.");
                (void)VOS_MutexLock(m_pMutexCameraLock);
                IVS_UINT32 uiChannelListSize = m_vecAllAlarmDev.size();
                (void)VOS_MutexUnlock(m_pMutexCameraLock);
                if (uiChannelListSize < pQueryRange->uiToIndex)
                {
                    break;
                }
                iRet = GetAlarmChannelInfoListFromCache(pQueryRange, pList);
                return iRet;
            }
        case eM_Thread_Retrive_Status_Ready:  // 数据获取已经结束
            BP_DBG_LOG("get user channel list, data is ready.");
            iRet = GetAlarmChannelInfoListFromCache(pQueryRange, pList);
            return iRet;

        case eM_Thread_Retrive_Excepting:  // 出现异常
            BP_DBG_LOG("get user channel list, data is exception.");
             iRet = GetAlarmChannelInfoListFromCache(pQueryRange, pList);
            return iRet;

        default:
            ;
        }
        VOS_Sleep(10);
        if (30 < time(0) - currentTime)  // 默认30秒超时
        {

            return IVS_NET_RECV_TIMEOUT;
        }

    }
}


IVS_INT32 CDeviceMgr::GetDomainAlarmChannelList(const IVS_CHAR* pDomainCode,
	const IVS_INDEX_RANGE* pQueryRange,
	IVS_ALARM_BRIEF_INFO_LIST* pList,
	IVS_UINT32 uiBufferSize,
	IVS_UINT32 uiAlarmType)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pQueryRange, IVS_PARA_INVALID);
	CHECK_POINTER(pList, IVS_PARA_INVALID);
	
	IVS_INT32 iRet = IVS_FAIL;

	memset(pList,0,uiBufferSize);
	// 初始化pList
	pList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
	pList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;

	// from 
	if (1 == pQueryRange->uiFromIndex)
	{
		IVS_INDEX_RANGE stRange;
		stRange.uiFromIndex = 1;
		stRange.uiToIndex = STEP_INDEX;
		m_uiAlarmChannelTotalNumByDom = 0;

		std::string strTransID;
		std::string strTargetDomainCode;
		std::string strExdomainFlag;
		IVS_CHANNLELIST_RETRIVE_STATUS_INFO retriveStatusInfo;
		memset(&retriveStatusInfo, 0, sizeof(retriveStatusInfo));

		(void)VOS_MutexLock(m_pMutexAlarmLock);
		m_vecAllAlarmDevByDom.clear();
		(void)VOS_MutexUnlock(m_pMutexAlarmLock);

		(void)VOS_MutexLock(m_pAlarmRetriveStatusInfoListMutex);
		m_AlarmChannelRetriveStatusInfoList.clear();
		m_AlarmChannelRetriveThreadStatus = eM_Thread_Retrive_Status_Init;
		(void)VOS_MutexUnlock(m_pAlarmRetriveStatusInfoListMutex);

		strTargetDomainCode = pDomainCode;
		for (int i = 0; i < 2; i++)
		{
			// 发送本域消息
			CXml reqVideoXml;
			strTransID = CMKTransID::Instance().GenerateTransID();

			if (1== i && DEVICE_TYPE_EXTEND_ALARM == uiAlarmType)
			{
				// 外部域告警设备不需要向外域（PCG）请求
				continue;
			}

			if (0 == i)
			{
				(void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_LOCAL, strTargetDomainCode.c_str(), uiAlarmType, stRange, reqVideoXml);	
				retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_LOCAL;
				strExdomainFlag = "Local Domain";
			}
			else
			{
				(void)CDeviceMgrXMLProcess::GetDeviceListGetXml(EXDOMAIN_FLAG_OUT, strTargetDomainCode.c_str(), uiAlarmType, stRange, reqVideoXml);	
				retriveStatusInfo.iExDomainFlag = EXDOMAIN_FLAG_OUT;
				strExdomainFlag = "ExDomain";
			}
			retriveStatusInfo.iSendTime = clock();
			iRet = m_pUserMgr->SendAsynMsg(strTargetDomainCode.c_str(), NSS_GET_USER_CHANNEL_LIST_REQ, strTransID, &reqVideoXml);

			BP_RUN_LOG_INF("process get user channel list,", "start to send alarm[%u] request,  page index:[%u - %u], target domain:[%s : %s], transID:[%s]", 
				uiAlarmType, stRange.uiFromIndex, stRange.uiToIndex, strExdomainFlag.c_str(), strTargetDomainCode.c_str(), strTransID.c_str());

			if (IVS_SUCCEED == iRet)
			{
				retriveStatusInfo.iSessionID = m_pUserMgr->GetSessionID();
				retriveStatusInfo.uiChannelType = uiAlarmType;
				retriveStatusInfo.iThreadStatus = eM_Thread_Retrive_Status_Init;
				strncpy(retriveStatusInfo.szTargetDomainCode, strTargetDomainCode.c_str(), IVS_DOMAIN_CODE_LEN);
				retriveStatusInfo.iGetChannelType = eM_Get_Device_ByDomainCode;

				CSDKInterface* pSingleSvrProxy = (CSDKInterface*)(m_pUserMgr->GetSingleSvrProxy());
				if (NULL != pSingleSvrProxy)
				{
					pSingleSvrProxy->AddTransIDChannelListRetriveStatusMap(strTransID, retriveStatusInfo);
				}

				(void)VOS_MutexLock(m_pAlarmRetriveStatusInfoListMutex);
				m_AlarmChannelRetriveStatusInfoList.push_back(retriveStatusInfo);
				(void)VOS_MutexUnlock(m_pAlarmRetriveStatusInfoListMutex);
			}
		}

		(void)VOS_MutexLock(m_pAlarmRetriveStatusInfoListMutex);
		IVS_UINT32 iStatusInfoListSize = m_AlarmChannelRetriveStatusInfoList.size();
		(void)VOS_MutexUnlock(m_pAlarmRetriveStatusInfoListMutex);
		if (iStatusInfoListSize == 0)
		{
			pList->uiTotal = 0;
			return IVS_SUCCEED;
		}

	}


	time_t currentTime = time(0); 
	iRet = IVS_SUCCEED;
	while(true) //lint !e716
	{
		switch(m_AlarmChannelRetriveThreadStatus)
		{
		case eM_Thread_Retrive_Status_Retriving: // 数据正在获取中
			{
				BP_DBG_LOG("get user channel list, data is retriving.");
				(void)VOS_MutexLock(m_pMutexCameraLock);
				IVS_UINT32 uiChannelListSize = m_vecAllAlarmDevByDom.size();
				(void)VOS_MutexUnlock(m_pMutexCameraLock);
				if (uiChannelListSize < pQueryRange->uiToIndex)
				{
					break;
				}
				iRet = GetAlarmChannelInfoListFromCache(pQueryRange, pList, m_vecAllAlarmDevByDom);
				return iRet;
			}
		case eM_Thread_Retrive_Status_Ready:  // 数据获取已经结束
			BP_DBG_LOG("get user channel list, data is ready.");
			iRet = GetAlarmChannelInfoListFromCache(pQueryRange, pList, m_vecAllAlarmDevByDom);
			return iRet;

		case eM_Thread_Retrive_Excepting:  // 出现异常
			BP_DBG_LOG("get user channel list, data is exception.");
			iRet = GetAlarmChannelInfoListFromCache(pQueryRange, pList, m_vecAllAlarmDevByDom);
			return iRet;

		default:
			;
		}
		VOS_Sleep(10);
		if (30 < time(0) - currentTime)  // 默认30秒超时
		{

			return IVS_NET_RECV_TIMEOUT;
		}
	}
}


IVS_INT32 CDeviceMgr::GetAlarmChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_ALARM_BRIEF_INFO_LIST* pList)
{
    CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pQueryRange, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    IVS_UINT32 uiToIndex = 0;
    (void)VOS_MutexLock(m_pMutexAlarmLock);

    BP_RUN_LOG_INF("get alarm channel list from cache", "cache size:%u, real num:%u, page index[%u - %u]", 
        m_vecAllAlarmDev.size(), m_uiAlarmChannelTotalNum, pQueryRange->uiFromIndex, pQueryRange->uiToIndex);

    if (pQueryRange->uiFromIndex > m_vecAllAlarmDev.size())
    {
        (void)VOS_MutexUnlock(m_pMutexAlarmLock);
        return IVS_SUCCEED;
    }

    if (pQueryRange->uiFromIndex < m_uiAlarmChannelTotalNum && pQueryRange->uiToIndex > m_uiAlarmChannelTotalNum)
    {
        // 说明当前已经达到最后一页，为保证不溢出，需要 pList->uiTotal == m_vecAllCameraDev.size()
        pList->uiTotal = m_vecAllAlarmDev.size();
    }
    else
    {
        pList->uiTotal = m_uiAlarmChannelTotalNum;
    }


    if(pQueryRange->uiToIndex > m_vecAllAlarmDev.size())
    {
        uiToIndex = (IVS_UINT32)((m_vecAllAlarmDev.size() - pQueryRange->uiFromIndex)+1);
    }
    else
    {
        uiToIndex = (IVS_UINT32)((pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1);
    }

    for(IVS_UINT32 i = 0; i< uiToIndex; i++)
    {
        pList->stAlarmBriefInfo[i] = m_vecAllAlarmDev.at(pQueryRange->uiFromIndex + i - 1); 
    }
    (void)VOS_MutexUnlock(m_pMutexAlarmLock);
    return IVS_SUCCEED;
}


IVS_INT32 CDeviceMgr::GetAlarmChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_ALARM_BRIEF_INFO_LIST* pList, ALARM_BRIEF_INFO_VEC &vecAlarmDev)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pQueryRange, IVS_OPERATE_MEMORY_ERROR);
	
	IVS_UINT32 uiToIndex = 0;
	(void)VOS_MutexLock(m_pMutexAlarmLock);

	BP_RUN_LOG_INF("get alarm channel list from cache", "cache size:%u, real num:%u, page index[%u - %u]", 
		vecAlarmDev.size(), m_uiAlarmChannelTotalNumByDom, pQueryRange->uiFromIndex, pQueryRange->uiToIndex);

	if (pQueryRange->uiFromIndex > vecAlarmDev.size())
	{
		(void)VOS_MutexUnlock(m_pMutexAlarmLock);
		return IVS_SUCCEED;
	}

	if (pQueryRange->uiFromIndex < m_uiAlarmChannelTotalNumByDom && pQueryRange->uiToIndex > m_uiAlarmChannelTotalNumByDom)
	{
		// 说明当前已经达到最后一页，为保证不溢出，需要 pList->uiTotal == m_vecAllCameraDev.size()
		pList->uiTotal = vecAlarmDev.size();
	}
	else
	{
		pList->uiTotal = m_uiAlarmChannelTotalNumByDom;
	}


	if(pQueryRange->uiToIndex > vecAlarmDev.size())
	{
		uiToIndex = (IVS_UINT32)((vecAlarmDev.size() - pQueryRange->uiFromIndex)+1);
	}
	else
	{
		uiToIndex = (IVS_UINT32)((pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1);
	}

	for(IVS_UINT32 i = 0; i< uiToIndex; i++)
	{
		pList->stAlarmBriefInfo[i] = vecAlarmDev.at(pQueryRange->uiFromIndex + i - 1); 
	}
	(void)VOS_MutexUnlock(m_pMutexAlarmLock);
	return IVS_SUCCEED;
}


IVS_INT32 CDeviceMgr::GetVideoChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange, IVS_CAMERA_BRIEF_INFO_LIST* pList)
{
    CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pQueryRange, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

    IVS_UINT32 uiToIndex = 0;
    (void)VOS_MutexLock(m_pMutexCameraLock);

    BP_RUN_LOG_INF("GetVideoChannelInfoListFromCache", "cache size:%u, RealNum:%u, query from index:%u, query to index:%u", 
        m_vecAllCameraDev.size(), m_uiVideoChannelTotalNum, pQueryRange->uiFromIndex, pQueryRange->uiToIndex);

    if (pQueryRange->uiFromIndex > m_vecAllCameraDev.size())
    {
        (void)VOS_MutexUnlock(m_pMutexCameraLock);
        return IVS_SUCCEED;
    }

    if (pQueryRange->uiFromIndex < m_uiVideoChannelTotalNum && pQueryRange->uiToIndex > m_uiVideoChannelTotalNum)
    {
        // 说明当前已经达到最后一页，为保证不溢出，需要 pList->uiTotal == m_vecAllCameraDev.size()
        pList->uiTotal = m_vecAllCameraDev.size();
    }
    else
    {
        pList->uiTotal = m_uiVideoChannelTotalNum;
    }
    

    if(pQueryRange->uiToIndex > m_vecAllCameraDev.size())
    {
        uiToIndex = (IVS_UINT32)((m_vecAllCameraDev.size() - pQueryRange->uiFromIndex)+1);
    }
    else
    {
        uiToIndex = (IVS_UINT32)((pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1);
    }

    for(IVS_UINT32 i = 0; i< uiToIndex; i++)
    {
        pList->stCameraBriefInfo[i] = m_vecAllCameraDev.at(pQueryRange->uiFromIndex + i - 1); 
    }
    (void)VOS_MutexUnlock(m_pMutexCameraLock);
    return IVS_SUCCEED;
}


IVS_INT32 CDeviceMgr::GetVideoChannelInfoListFromCache(const IVS_INDEX_RANGE* pQueryRange,
	IVS_CAMERA_BRIEF_INFO_LIST* pList, CAMERA_BRIEF_INFO_VEC &vecCameraDev)
{
	CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pQueryRange, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_UINT32 uiToIndex = 0;
	(void)VOS_MutexLock(m_pMutexCameraLock);

	BP_RUN_LOG_INF("GetVideoChannelInfoListFromCache", "cache size:%u, RealNum:%u, query from index:%u, query to index:%u", 
		vecCameraDev.size(), m_uiVideoChannelTotalNumByDom, pQueryRange->uiFromIndex, pQueryRange->uiToIndex);

	if (pQueryRange->uiFromIndex > vecCameraDev.size())
	{
		(void)VOS_MutexUnlock(m_pMutexCameraLock);
		return IVS_SUCCEED;
	}

	if (pQueryRange->uiFromIndex < m_uiVideoChannelTotalNumByDom && pQueryRange->uiToIndex > m_uiVideoChannelTotalNumByDom)
	{
		// 说明当前已经达到最后一页，为保证不溢出，需要 pList->uiTotal == m_vecAllCameraDev.size()
		pList->uiTotal = m_vecAllCamDevByDom.size();
	}
	else
	{
		pList->uiTotal = m_uiVideoChannelTotalNumByDom;
	}


	if(pQueryRange->uiToIndex > m_vecAllCamDevByDom.size())
	{
		uiToIndex = (IVS_UINT32)((m_vecAllCamDevByDom.size() - pQueryRange->uiFromIndex)+1);
	}
	else
	{
		uiToIndex = (IVS_UINT32)((pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1);
	}

	for(IVS_UINT32 i = 0; i< uiToIndex; i++)
	{
		pList->stCameraBriefInfo[i] = m_vecAllCamDevByDom.at(pQueryRange->uiFromIndex + i - 1); 
	}
	(void)VOS_MutexUnlock(m_pMutexCameraLock);
	return IVS_SUCCEED;
}


void CDeviceMgr::SetChannelListRetriveStatusValue(IVS_UINT32 uiChannelType, 
                                                                                         const IVS_CHAR* pTargetDomainCode, 
                                                                                         IVS_INT32 iExDomainFlag, 
                                                                                         IVS_UINT32 uiRealNum,
                                                                                         IVS_INT32 emThreadStatus,
																						 IVS_INT32 iGetChannelType)
{
    IVS_INT32 iCurrentThreadStatus = eM_Thread_Retrive_Status_Ready;
    IVS_UINT32 uiTotalNum = 0;
    if (DEVICE_TYPE_CAMERA ==uiChannelType )
    {
        (void)VOS_MutexLock(m_pVideoRetriveStatusInfoListMutex);

        std::list<IVS_CHANNLELIST_RETRIVE_STATUS_INFO>::iterator itor = m_VideoChannelRetriveStatusInfoList.begin();
        for (; m_VideoChannelRetriveStatusInfoList.end() != itor; itor++)
        {
            if (itor->iExDomainFlag == iExDomainFlag && 
                0 == strncmp(itor->szTargetDomainCode, pTargetDomainCode, IVS_DOMAIN_CODE_LEN))
            {
                if (eM_Thread_Retrive_Excepting != emThreadStatus)
                {
                    itor->uiRealNum =  uiRealNum;     // 当前分页异常，不影响已有完成分页
                }
                
                itor->iThreadStatus = emThreadStatus;
            }
            uiTotalNum += itor->uiRealNum;
            if (iCurrentThreadStatus > itor->iThreadStatus)
            {
                iCurrentThreadStatus = itor->iThreadStatus;
            }
        }
        m_VideoChannelRetriveThreadStatus = (THREAD_RETRIVE_STATUS)iCurrentThreadStatus;

        if (eM_Get_Device_All == iGetChannelType && uiTotalNum > m_uiVideoChannelTotalNum)
        {
            m_uiVideoChannelTotalNum = uiTotalNum;
        }
		else if (eM_Get_Device_ByDomainCode == iGetChannelType && uiTotalNum > m_uiVideoChannelTotalNumByDom)
		{
			m_uiVideoChannelTotalNumByDom = uiTotalNum;
		}

        (void)VOS_MutexUnlock(m_pVideoRetriveStatusInfoListMutex);

    }
    else
    {
        (void)VOS_MutexLock(m_pAlarmRetriveStatusInfoListMutex);

        std::list<IVS_CHANNLELIST_RETRIVE_STATUS_INFO>::iterator itor = m_AlarmChannelRetriveStatusInfoList.begin();
        for (; m_AlarmChannelRetriveStatusInfoList.end() != itor; itor++)
        {
            if (itor->iExDomainFlag == iExDomainFlag && 
                0 == strncmp(itor->szTargetDomainCode, pTargetDomainCode, IVS_DOMAIN_CODE_LEN))
            {
                if (eM_Thread_Retrive_Excepting != emThreadStatus)
                {
                    itor->uiRealNum =  uiRealNum; // 当前分页异常，不影响已有完成分页
                }

                itor->iThreadStatus = emThreadStatus;
            }
            uiTotalNum += itor->uiRealNum;
            if (iCurrentThreadStatus > itor->iThreadStatus)
            {
                iCurrentThreadStatus = itor->iThreadStatus;
            }
        }
        m_AlarmChannelRetriveThreadStatus = (THREAD_RETRIVE_STATUS)iCurrentThreadStatus;
        if (eM_Get_Device_All == iGetChannelType && uiTotalNum > m_uiAlarmChannelTotalNum)
        {
            m_uiAlarmChannelTotalNum = uiTotalNum;
        }
		else if (eM_Get_Device_ByDomainCode == iGetChannelType && uiTotalNum > m_uiAlarmChannelTotalNumByDom)
		{
			m_uiAlarmChannelTotalNumByDom = uiTotalNum;
		}

        (void)VOS_MutexUnlock(m_pAlarmRetriveStatusInfoListMutex);
    }

}


IVS_INT32 CDeviceMgr::ParseChannelListRsp(const IVS_CHAR* pData, IVS_UINT32 uiChannelType, const IVS_INT32 iExDomainFlag, 
	const IVS_CHAR* pTargetDomainCode, IVS_PAGE_INFO& pageInfo, IVS_UINT32& uiItemSize, IVS_INT32 iGetChannelType)
{
	CHECK_POINTER(pTargetDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pData, IVS_PARA_INVALID);
	IVS_INT32 iRet = IVS_SUCCEED;
	switch(uiChannelType)
	{
	case DEVICE_TYPE_CAMERA:
		{
			std::vector<IVS_CAMERA_BRIEF_INFO> videoChannelVector;

			iRet = CDeviceMgrXMLProcess::ParseVideoChannelList(pTargetDomainCode, pData, iExDomainFlag, pageInfo, videoChannelVector);            
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "process get user channel list rsp", "fail to parse xml.");
				return iRet;
			}
			uiItemSize = videoChannelVector.size();
			(void)VOS_MutexLock(m_pMutexCameraLock);
			if (eM_Get_Device_All == iGetChannelType)
			{
				m_vecAllCameraDev.insert(m_vecAllCameraDev.end(), videoChannelVector.begin(), videoChannelVector.end());
			}
			else if (eM_Get_Device_ByDomainCode == iGetChannelType)
			{
				m_vecAllCamDevByDom.insert(m_vecAllCamDevByDom.end(), videoChannelVector.begin(), videoChannelVector.end());
			}
			(void)VOS_MutexUnlock(m_pMutexCameraLock);
		}

		break;
	case DEVICE_TYPE_EXTEND_ALARM:
	case DEVICE_TYPE_ALARM:
		{
			std::vector<IVS_ALARM_BRIEF_INFO> alarmChannelVector;

			iRet = CDeviceMgrXMLProcess::ParseAlarmChannelList(pTargetDomainCode, pData, iExDomainFlag, pageInfo, alarmChannelVector);            
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "process get user channel list rsp", "fail to parse xml.");
				return iRet;
			}
			uiItemSize = alarmChannelVector.size();
			(void)VOS_MutexLock(m_pMutexAlarmLock);
			if (eM_Get_Device_All == iGetChannelType)
			{
				m_vecAllAlarmDev.insert(m_vecAllAlarmDev.end(), alarmChannelVector.begin(), alarmChannelVector.end());
			}
			else if (eM_Get_Device_ByDomainCode == iGetChannelType)
			{
				m_vecAllAlarmDevByDom.insert(m_vecAllAlarmDevByDom.end(), alarmChannelVector.begin(), alarmChannelVector.end());
			}
			
			(void)VOS_MutexUnlock(m_pMutexAlarmLock);
		}
		break;
	default:
		;
	}
	return IVS_SUCCEED;
}


IVS_INT32 CDeviceMgr::ProcessGetUserChannelListRsp(IVS_CHANNLELIST_RETRIVE_STATUS_INFO& channelListRetriveStatus, /*std::string& strTransID, */ const IVS_CHAR* pData)
{

    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    std::string strSendDomainCode = channelListRetriveStatus.szTargetDomainCode;
    std::string strChannelType;
    switch (channelListRetriveStatus.uiChannelType)
    {
    case DEVICE_TYPE_CAMERA:
        strChannelType = "video channel";
        m_pUserMgr->GetDomainCode(strSendDomainCode);
        break;
    case DEVICE_TYPE_ALARM:
        strChannelType = "alarm channel";
        break;
    case DEVICE_TYPE_EXTEND_ALARM:
        strChannelType = "extend alarm channel";
        break;
    default:
        strChannelType = "invalid channel";
    }

    std:: string strExDomainFlag = (EXDOMAIN_FLAG_LOCAL == channelListRetriveStatus.iExDomainFlag) ? ("Local Domain") : ("ExDomain");

    if (IVS_SUCCEED != channelListRetriveStatus.iLastRetCode)
    {
        BP_RUN_LOG_ERR(channelListRetriveStatus.iLastRetCode, "process get user channel list rsp", 
            "request type:[%s], page index:[%u - %u], target domain:[%s : %s]", 
            strChannelType.c_str(), channelListRetriveStatus.uiFromIndex, channelListRetriveStatus.uiToIndex, 
            strExDomainFlag.c_str(), channelListRetriveStatus.szTargetDomainCode);

        // 当前请求返会错误码，此处不再往该域发送后续分页请求
        SetChannelListRetriveStatusValue(channelListRetriveStatus.uiChannelType, channelListRetriveStatus.szTargetDomainCode, 
            channelListRetriveStatus.iExDomainFlag, 0, eM_Thread_Retrive_Excepting, channelListRetriveStatus.iGetChannelType);

        return channelListRetriveStatus.iLastRetCode;
    }

    IVS_PAGE_INFO pageInfo;
    memset(&pageInfo, 0, sizeof(pageInfo));
    IVS_UINT32 uiListSize = 0;
    long rspTime = clock();
    IVS_INT32 iRet = ParseChannelListRsp(pData, channelListRetriveStatus.uiChannelType, channelListRetriveStatus.iExDomainFlag, 
        channelListRetriveStatus.szTargetDomainCode, pageInfo, uiListSize, channelListRetriveStatus.iGetChannelType);

    if (IVS_SUCCEED != iRet)
    {
        // 当前XML解析返会错误码，此处不再往该域发送后续分页请求
        SetChannelListRetriveStatusValue(channelListRetriveStatus.uiChannelType, channelListRetriveStatus.szTargetDomainCode, 
            channelListRetriveStatus.iExDomainFlag, 0, eM_Thread_Retrive_Excepting, channelListRetriveStatus.iGetChannelType);
        return iRet;
    }

    BP_RUN_LOG_INF("process get user channel list rsp,",  
        "receive [%s] message, real num:[%u], page index:[%u - %u], xml item size:[%u], target domain:[%s : %s], rsp time interval:[%u],  xml parse time:[%u].", 
        strChannelType.c_str(),  pageInfo.uiRealNum, pageInfo.uiFromIndex, pageInfo.uiToIndex, uiListSize, strExDomainFlag.c_str(), 
        channelListRetriveStatus.szTargetDomainCode, (rspTime - channelListRetriveStatus.iSendTime), (clock() - rspTime));

    if (0 == pageInfo.uiRealNum || pageInfo.uiRealNum <= pageInfo.uiToIndex)
    {
        // 数据已经接收完毕
        SetChannelListRetriveStatusValue(channelListRetriveStatus.uiChannelType, channelListRetriveStatus.szTargetDomainCode, 
            channelListRetriveStatus.iExDomainFlag, pageInfo.uiRealNum, eM_Thread_Retrive_Status_Ready, channelListRetriveStatus.iGetChannelType);
        return IVS_SUCCEED;
    }

    // 继续接收数据
    SetChannelListRetriveStatusValue(channelListRetriveStatus.uiChannelType, channelListRetriveStatus.szTargetDomainCode, 
        channelListRetriveStatus.iExDomainFlag, pageInfo.uiRealNum, eM_Thread_Retrive_Status_Retriving, channelListRetriveStatus.iGetChannelType);

    CXml reqReqXml;
    std::string strSendTransID = CMKTransID::Instance().GenerateTransID();
    IVS_INDEX_RANGE stRange;
    stRange.uiFromIndex = pageInfo.uiToIndex + 1;
    stRange.uiToIndex = pageInfo.uiToIndex + STEP_INDEX;
    (void)CDeviceMgrXMLProcess::GetDeviceListGetXml(channelListRetriveStatus.iExDomainFlag,  
        channelListRetriveStatus.szTargetDomainCode, channelListRetriveStatus.uiChannelType, stRange, reqReqXml);	

    BP_RUN_LOG_INF("process get user channel list rsp",   "start to send next page request, page index:[%u - %u].", 
        stRange.uiFromIndex, stRange.uiToIndex);

    iRet = m_pUserMgr->SendAsynMsg(strSendDomainCode.c_str(), NSS_GET_USER_CHANNEL_LIST_REQ, strSendTransID, &reqReqXml);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "process get user channel list rsp,", "fail to send message.");
        // 当前发送返会错误码，此处不再往该域发送后续分页请求
        SetChannelListRetriveStatusValue(channelListRetriveStatus.uiChannelType, channelListRetriveStatus.szTargetDomainCode, 
            channelListRetriveStatus.iExDomainFlag, 0, eM_Thread_Retrive_Excepting, channelListRetriveStatus.iGetChannelType);
        return iRet;
    }

    IVS_CHANNLELIST_RETRIVE_STATUS_INFO retriveStatusInfo = channelListRetriveStatus;
    retriveStatusInfo.iSendTime = clock();
    CSDKInterface* pSingleSvrProxy = (CSDKInterface*)(m_pUserMgr->GetSingleSvrProxy());
    if (NULL != pSingleSvrProxy)
    {
        pSingleSvrProxy->AddTransIDChannelListRetriveStatusMap(strSendTransID, retriveStatusInfo);
    }
    return IVS_SUCCEED;
}

// 获取摄像机
//IVS_INT32 CDeviceMgr::GetCameras(const IVS_INDEX_RANGE* pQueryRange,
//	IVS_CAMERA_BRIEF_INFO_LIST* pList,
//	IVS_UINT32 uiBufferSize)
//{
//	CHECK_POINTER(pQueryRange, IVS_PARA_INVALID);
//	CHECK_POINTER(pList, IVS_PARA_INVALID);
//	IVS_DEBUG_TRACE("");
//
//	memset(pList,0,uiBufferSize);
//	// 初始化pList
//	pList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
//	pList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;
//	IVS_INT32 iRet = IVS_FAIL;
//	// 判断是否要开启重新从头查找
//	IVS_BOOL isNeedStartThread = 0;
//	// from == 1的查找表示要刷新以前查找的缓存
//	if (1 == pQueryRange->uiFromIndex)
//	{		
//		(void)VOS_MutexLock(m_pMutexCameraLock);
//		if(mGetCamerasThreadStatus == eM_Thread_Ready || mGetCamerasThreadStatus == eM_Thread_DataIsReady || mGetCamerasThreadStatus == eM_Thread_ExceptionRet)//从未启动
//		{	
//			// 表明从未开启线程查询，或者上一次的查询已经结束
//			isNeedStartThread = 1;
//		}
//		(void)VOS_MutexUnlock(m_pMutexCameraLock);
//	}
//	else
//	{
//		(void)VOS_MutexLock(m_pMutexCameraLock);
//		THREAD_STATUS status = mGetCamerasThreadStatus;
//		(void)VOS_MutexUnlock(m_pMutexCameraLock);
//		// 从未启动过线程查询,就开始输入不从1的查询表示输入参数错误
//		CHECK_EQUAL(status, eM_Thread_Ready, IVS_PARA_INVALID);
//	}
//	if(isNeedStartThread)
//	{
//		// 清理m_CameraListAll
//		(void)VOS_MutexLock(m_pMutexCameraLock);
//		m_vecCameraDomain.clear();
//		m_vecAllCameraDev.clear();
//		mGetCamerasThreadStatus = eM_Thread_Init;	
//		iThreadGetCameraRet = IVS_FAIL;
//		(void)VOS_MutexUnlock(m_pMutexCameraLock);
//		long lResult = (long)VOS_CreateThread((VOS_THREAD_FUNC)GetCamerasThreadEntry, (void*)this, &m_pGetCamerasThread, VOS_DEFAULT_STACK_SIZE);
//// 		if (IVS_SUCCEED != lResult)
//// 		{
//// 			BP_RUN_LOG_ERR(IVS_FAIL, "VOS_CreateThread to GetCamerasThreadEntry failed", "NA");
//// 			iRet = IVS_FAIL;
//// 			return iRet;
//// 		}
//		CHECK_NOT_EQUAL(lResult, IVS_SUCCEED, IVS_FAIL);
//	}
//
//	// 如果已经查出来，或者查询已出错，就直接返回
//	for (;;)
//	{
//		(void)VOS_MutexLock(m_pMutexCameraLock);
//		if(mGetCamerasThreadStatus == eM_Thread_ExceptionRet)
//		{
//			(void)VOS_MutexUnlock(m_pMutexCameraLock);
//			iRet = iThreadGetCameraRet;
//			break;
//		}
//	// TODO 暂时去掉接受状态，等所有返回后再返回
//// 		else if(mGetCamerasThreadStatus == eM_Thread_DataIsReving)
//// 		{
//// 			if(pQueryRange->uiToIndex <= m_vecAllCameraDev.size())
//// 			{
//// 				IVS_UINT32 uiSize = (pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1;
//// 				iRet = GetCamerasInfo(uiSize, pList, pQueryRange);
//// 				VOS_MutexUnlock(m_pMutexCameraLock);
//// 				break;
//// 			}
//// 		}
//		else if(mGetCamerasThreadStatus == eM_Thread_DataIsReady)
//		{
//			IVS_UINT32 uiToIndex = 0;
//
//            // DTS2013080810040
//            // edit by wanglei 00165153:2013.8.14 如果pQueryRange->uiFromIndex > m_vecAllAudioDev.size()
//            // (IVS_UINT32)((m_vecAllAudioDev.size() - pQueryRange->uiFromIndex)+1);  uiToIndex会出现无穷大, 导致后续进程崩溃
//            if (pQueryRange->uiFromIndex > m_vecAllCameraDev.size())
//            {
//                (void)VOS_MutexUnlock(m_pMutexCameraLock);
//                BP_RUN_LOG_ERR(IVS_DATA_INVALID, "invalid query range from index","iner item max size:%d, query range from:%d, ",
//                    m_vecAllCameraDev.size(), pQueryRange->uiFromIndex);
//                return IVS_SUCCEED;
//            }
//
//			if(pQueryRange->uiToIndex > m_vecAllCameraDev.size())
//			{
//				uiToIndex = (IVS_UINT32)((m_vecAllCameraDev.size() - pQueryRange->uiFromIndex)+1);
//			}
//			else
//			{
//				uiToIndex = (IVS_UINT32)((pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1);
//			}
//			iRet = GetCamerasInfo(uiToIndex, pList, pQueryRange);
//			(void)VOS_MutexUnlock(m_pMutexCameraLock);
//			break;
//		}
//		(void)VOS_MutexUnlock(m_pMutexCameraLock);	
//		// 等待1000
//		(void)ACE_OS::sleep(1);//lint !e1055 !e746
//	}
//
//	/*if(pList->uiTotal > 0)
//	{
//		pList->stIndexRange.uiToIndex = (pQueryRange->uiFromIndex + pList->uiTotal) -1;
//	}*/
//
//	return iRet;
//}


// 获取摄像机列表结构体赋值
//IVS_INT32 CDeviceMgr::GetCamerasInfo(IVS_UINT32 uiSize, IVS_CAMERA_BRIEF_INFO_LIST* pList, const IVS_INDEX_RANGE* pQueryRange)
//{
//	CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
//	CHECK_POINTER(pQueryRange, IVS_OPERATE_MEMORY_ERROR);
//	IVS_DEBUG_TRACE("");
//
//	pList->uiTotal = m_vecAllCameraDev.size();
//	for(IVS_UINT32 i = 0; i< uiSize; i++)
//	{
//		IVS_CAMERA_BRIEF_INFO tempCameraInfo = m_vecAllCameraDev.at(pQueryRange->uiFromIndex + i - 1);
//		pList->stCameraBriefInfo[i].bEnableVoice = tempCameraInfo.bEnableVoice;
//		pList->stCameraBriefInfo[i].uiCameraStatus = tempCameraInfo.uiCameraStatus;
//		//pList->stCameraBriefInfo[i].uiGroupID = tempCameraInfo.uiGroupID;
//		pList->stCameraBriefInfo[i].uiStatus = tempCameraInfo.uiStatus;
//		pList->stCameraBriefInfo[i].uiType = tempCameraInfo.uiType;
//		pList->stCameraBriefInfo[i].uiDevFormType = tempCameraInfo.uiDevFormType;
//		pList->stCameraBriefInfo[i].bIsExDomain = tempCameraInfo.bIsExDomain;
//		pList->stCameraBriefInfo[i].uiNetType = tempCameraInfo.uiNetType;
//		pList->stCameraBriefInfo[i].bSupportIntelligent = tempCameraInfo.bSupportIntelligent;
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cDevGroupCode, IVS_DEVICE_GROUP_LEN, tempCameraInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cDevGroupCode, IVS_DEVICE_GROUP_LEN, tempCameraInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cDevModelType, IVS_DEV_MODEL_LEN, tempCameraInfo.cDevModelType, IVS_DEV_MODEL_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cDevModelType, IVS_DEV_MODEL_LEN, tempCameraInfo.cDevModelType, IVS_DEV_MODEL_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, tempCameraInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, tempCameraInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cCameraLocation, IVS_DESCRIBE_LEN, tempCameraInfo.cCameraLocation, IVS_DESCRIBE_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cCameraLocation, IVS_DESCRIBE_LEN, tempCameraInfo.cCameraLocation, IVS_DESCRIBE_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cCode, IVS_DEV_CODE_LEN, tempCameraInfo.cCode, IVS_DEV_CODE_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cCode, IVS_DEV_CODE_LEN, tempCameraInfo.cCode, IVS_DEV_CODE_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cDomainCode, IVS_DOMAIN_CODE_LEN, tempCameraInfo.cDomainCode, IVS_DOMAIN_CODE_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cDomainCode, IVS_DOMAIN_CODE_LEN, tempCameraInfo.cDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cNvrCode, IVS_NVR_CODE_LEN, tempCameraInfo.cNvrCode, IVS_NVR_CODE_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cNvrCode, IVS_NVR_CODE_LEN, tempCameraInfo.cNvrCode, IVS_NVR_CODE_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cParentCode, IVS_DEV_CODE_LEN, tempCameraInfo.cParentCode, IVS_DEV_CODE_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cParentCode, IVS_DEV_CODE_LEN, tempCameraInfo.cParentCode, IVS_DEV_CODE_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cName, IVS_CAMERA_NAME_LEN, tempCameraInfo.cName, IVS_CAMERA_NAME_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cName, IVS_CAMERA_NAME_LEN, tempCameraInfo.cName, IVS_CAMERA_NAME_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cDevCreateTime, IVS_TIME_LEN, tempCameraInfo.cDevCreateTime, IVS_TIME_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cDevCreateTime, IVS_TIME_LEN, tempCameraInfo.cDevCreateTime, IVS_TIME_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cReserve, 32, tempCameraInfo.cReserve, 32))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cReserve, 32, tempCameraInfo.cReserve, 32) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cDevIp, IVS_IP_LEN, tempCameraInfo.cDevIp, IVS_IP_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cDevIp, IVS_IP_LEN, tempCameraInfo.cDevIp, IVS_IP_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stCameraBriefInfo[i].cDomainCode, IVS_DOMAIN_CODE_LEN, tempCameraInfo.cDomainCode, IVS_DOMAIN_CODE_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stCameraBriefInfo[i].cDomainCode, IVS_DOMAIN_CODE_LEN, tempCameraInfo.cDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//	}
//	return IVS_SUCCEED;
//}


// 获取语音设备列表
//IVS_INT32 CDeviceMgr::GetAudios(const IVS_INDEX_RANGE* pQueryRange,
//	IVS_AUDIO_BRIEF_INFO_LIST* pList,
//	IVS_UINT32 uiBufferSize)
//{
//	CHECK_POINTER(pQueryRange, IVS_PARA_INVALID);
//	CHECK_POINTER(pList, IVS_PARA_INVALID);
//	IVS_DEBUG_TRACE("");
//
//	// 初始化内存
//	memset(pList,0,uiBufferSize);
//
//	// 初始化pList
//	pList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
//	pList->stIndexRange.uiToIndex = pQueryRange->uiFromIndex;
//
//	// 返回值初始化
//	IVS_INT32 iRet = IVS_FAIL;
//
//	// 判断是否要开启重新从头查找语音设备
//	IVS_BOOL isNeedStartAudioThread = 0;
//	// from == 1的查找表示要刷新以前查找的缓存
//	if (1 == pQueryRange->uiFromIndex)
//	{		
//		(void)VOS_MutexLock(m_pMutexAudioLock);
//		if( mGetAudiosThreadStatus == eM_Thread_Ready || 
//			mGetAudiosThreadStatus == eM_Thread_DataIsReady || 
//			mGetAudiosThreadStatus == eM_Thread_ExceptionRet)
//		{	
//			// 表明从未开启查询语音设备列表线程，或者上一次的查询已经结束
//			isNeedStartAudioThread = 1;
//		}
//		(void)VOS_MutexUnlock(m_pMutexAudioLock);
//	}
//	else
//	{
//		(void)VOS_MutexLock(m_pMutexAudioLock);
//		THREAD_STATUS status = mGetAudiosThreadStatus;
//		(void)VOS_MutexUnlock(m_pMutexAudioLock);
//		// “从未启动过线程查询,就开始输入不从1的查询” 表示输入参数错误
//// 		if(status == eM_Thread_Ready)
//// 		{	
//// 			return IVS_PARA_INVALID;
//// 		}		
//		CHECK_EQUAL(status, eM_Thread_Ready, IVS_PARA_INVALID);
//	}
//
//	if(isNeedStartAudioThread)
//	{
//		// 清理m_DomainAudioList/m_AudioListAll，准备重新查询
//		(void)VOS_MutexLock(m_pMutexAudioLock);
//		m_vecAudioDomain.clear();
//		m_vecAllAudioDev.clear();
//		mGetAudiosThreadStatus = eM_Thread_Init;
//		// iThreadGetAudioRet = IVS_FAIL;
//		(void)VOS_MutexUnlock(m_pMutexAudioLock);
//
//		// 开启线程查询
//		long lResult = (long)VOS_CreateThread((VOS_THREAD_FUNC)GetAudiosThreadEntry, (void*)this, &m_pGetAudiosThread, VOS_DEFAULT_STACK_SIZE);
//// 		if (IVS_SUCCEED != lResult)
//// 		{
//// 			iRet = IVS_FAIL;
//// 			BP_RUN_LOG_ERR(iRet, "VOS_CreateThread to GetAudiosThreadEntry() failed", "NA");		
//// 			return iRet;
//// 		}
//		CHECK_NOT_EQUAL(lResult, IVS_SUCCEED, IVS_FAIL);
//	}
//
//	// 如果已经查出来，或者查询已出错，就直接返回
//	for (;;)
//	{
//		(void)VOS_MutexLock(m_pMutexAudioLock);
//		if(mGetAudiosThreadStatus == eM_Thread_ExceptionRet)
//		{
//			iRet = iThreadGetAudioRet;
//			(void)VOS_MutexUnlock(m_pMutexAudioLock);			
//			break;
//		}
//		// TODO 暂时去掉接受状态，等所有返回后再返回
//// 		else if(mGetAudiosThreadStatus == eM_Thread_DataIsReving)
//// 		{
//// 			if(pQueryRange->uiToIndex <= m_vecAllAudioDev.size()  )
//// 			{
//// 				IVS_UINT32 uiSize = (pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1;
//// 				iRet = GetAudiosInfo(uiSize, pList, pQueryRange);
//// 				VOS_MutexUnlock(m_pMutexAudioLock);
//// 				break;
//// 			}
//// 		}
//		else if(mGetAudiosThreadStatus == eM_Thread_DataIsReady)
//		{
//			// 需要返回给调用者的数据的size
//			IVS_UINT32 uiToIndex = 0;
//
//            // DTS2013080810040
//            // edit by wanglei 00165153:2013.8.14 如果pQueryRange->uiFromIndex > m_vecAllAudioDev.size()
//            // (IVS_UINT32)((m_vecAllAudioDev.size() - pQueryRange->uiFromIndex)+1);  uiToIndex会出现无穷大, 导致后续进程崩溃
//            if (pQueryRange->uiFromIndex > m_vecAllAudioDev.size())
//            {
//                (void)VOS_MutexUnlock(m_pMutexAudioLock);
//
//                BP_RUN_LOG_ERR(IVS_DATA_INVALID, "invalid query range from index","iner item max size:%d, query range from:%d, ",
//                    m_vecAllAudioDev.size(), pQueryRange->uiFromIndex);
//
//                return IVS_SUCCEED;
//            }
//
//			if(pQueryRange->uiToIndex > m_vecAllAudioDev.size())
//			{
//				uiToIndex = (IVS_UINT32)((m_vecAllAudioDev.size() - pQueryRange->uiFromIndex)+1);
//			}
//			else
//			{
//				uiToIndex = (IVS_UINT32)((pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1);
//			}
//			iRet = GetAudiosInfo(uiToIndex, pList, pQueryRange);
//			(void)VOS_MutexUnlock(m_pMutexAudioLock);
//			break;
//		}
//		(void)VOS_MutexUnlock(m_pMutexAudioLock);
//		// 等待1秒	
//		(void)ACE_OS::sleep(1);	//;lint !e1055
//	}
//
//	if(pList->uiTotal > 0)
//	{
//		pList->stIndexRange.uiToIndex = (pQueryRange->uiFromIndex + pList->uiTotal) -1;
//	}
//
//	return iRet;
//}

// 获取语音列表结构体赋值
//IVS_INT32 CDeviceMgr::GetAudiosInfo(IVS_UINT32 uiSize, IVS_AUDIO_BRIEF_INFO_LIST* pList, const IVS_INDEX_RANGE* pQueryRange)
//{
//	CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
//	CHECK_POINTER(pQueryRange, IVS_OPERATE_MEMORY_ERROR);
//	IVS_DEBUG_TRACE("");
//
//	for(IVS_UINT32 i = 0; i< uiSize ; i++)
//	{
//		IVS_AUDIO_BRIEF_INFO tempAlarmInfo = m_vecAllAudioDev.at(pQueryRange->uiFromIndex + i - 1);
//		pList->stAudioBriefInfo[i].uiChannelType = tempAlarmInfo.uiChannelType;
//		if (!CToolsHelp::Memcpy(pList->stAudioBriefInfo[i].cDevGroupCode, IVS_DEVICE_GROUP_LEN, tempAlarmInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAudioBriefInfo[i].cDevGroupCode, IVS_DEVICE_GROUP_LEN, tempAlarmInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stAudioBriefInfo[i].cAudioCode, IVS_DEV_CODE_LEN, tempAlarmInfo.cAudioCode, IVS_DEV_CODE_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAudioBriefInfo[i].cAudioCode, IVS_DEV_CODE_LEN, tempAlarmInfo.cAudioCode, IVS_DEV_CODE_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stAudioBriefInfo[i].cAudioName, IVS_ALARM_IN_NAME_LEN, tempAlarmInfo.cAudioName, IVS_ALARM_IN_NAME_LEN))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAudioBriefInfo[i].cAudioName, IVS_ALARM_IN_NAME_LEN, tempAlarmInfo.cAudioName, IVS_ALARM_IN_NAME_LEN) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		if (!CToolsHelp::Memcpy(pList->stAudioBriefInfo[i].cReserve, 32, tempAlarmInfo.cReserve, 32))
//		{
//			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAudioBriefInfo[i].cReserve, 32, tempAlarmInfo.cReserve, 32) fail", "NA");
//			return IVS_ALLOC_MEMORY_ERROR;
//		}
//		pList->uiTotal++;
//	}
//
//	return IVS_SUCCEED;
//}

// 获取告警设备列表
//IVS_INT32 CDeviceMgr::GetAlarms(const IVS_INDEX_RANGE* pQueryRange,
//	IVS_ALARM_BRIEF_INFO_LIST* pList,
//	IVS_UINT32 uiBufferSize)
//{
//	CHECK_POINTER(pQueryRange, IVS_PARA_INVALID);
//	CHECK_POINTER(pList, IVS_PARA_INVALID);
//	IVS_DEBUG_TRACE("");
//
//	// 初始化内存	
//	memset(pList,0,uiBufferSize);
//	// 初始化pList
//	pList->stIndexRange.uiFromIndex = pQueryRange->uiFromIndex;
//	pList->stIndexRange.uiToIndex = pQueryRange->uiToIndex;
//
//	// 返回值初始化
//	IVS_INT32 iRet = IVS_FAIL;
//
//	// 判断是否要开启重新从头查找
//	IVS_BOOL isNeedStartThread = 0;
//	// from == 1的查找表示要刷新以前查找的缓存
//	if (1 == pQueryRange->uiFromIndex)
//	{		
//		(void)VOS_MutexLock(m_pMutexAlarmLock);
//		if(mGetAlarmsThreadStatus == eM_Thread_Ready || mGetAlarmsThreadStatus == eM_Thread_DataIsReady || mGetAlarmsThreadStatus == eM_Thread_ExceptionRet)//从未启动
//		{	
//			// 表明从未开启线程查询，或者上一次的查询已经结束
//			isNeedStartThread = 1;
//		}
//		(void)VOS_MutexUnlock(m_pMutexAlarmLock);
//	}
//	else
//	{
//		(void)VOS_MutexLock(m_pMutexAlarmLock);
//		THREAD_STATUS status = mGetAlarmsThreadStatus;
//		(void)VOS_MutexUnlock(m_pMutexAlarmLock);
//		// 从未启动过线程查询,就开始输入不从1的查询表示输入参数错误
//		CHECK_EQUAL(status, eM_Thread_Ready, IVS_PARA_INVALID);
//	}
//	if(isNeedStartThread)
//	{
//		// 清理m_AlarmDomainList/m_AlarmListAll，准备重新查询
//		(void)VOS_MutexLock(m_pMutexAlarmLock);
//		m_vecAlarmDomain.clear();
//		m_vecAllAlarmDev.clear();
//		mGetAlarmsThreadStatus = eM_Thread_Init;
//		iThreadGetAlarmRet = IVS_FAIL;
//		(void)VOS_MutexUnlock(m_pMutexAlarmLock);
//
//		// 开启线程查询
//		long lResult = (long)VOS_CreateThread((VOS_THREAD_FUNC)GetAlarmsThreadEntry, (void*)this, &m_pGetAlarmsThread, VOS_DEFAULT_STACK_SIZE);
//// 		if (IVS_SUCCEED != lResult)
//// 		{
//// 			BP_RUN_LOG_ERR(IVS_FAIL, "VOS_CreateThread to Get Alarms ThreadEntry failed", "NA");
//// 			iRet = IVS_FAIL;
//// 			return iRet;
//// 		}
//		CHECK_NOT_EQUAL(lResult, IVS_SUCCEED, IVS_FAIL);
//	}
//
//	// 如果已经查出来，或者查询已出错，就直接返回
//	for (;;)
//	{
//		(void)VOS_MutexLock(m_pMutexAlarmLock);
//		if(mGetAlarmsThreadStatus == eM_Thread_ExceptionRet)
//		{
//			(void)VOS_MutexUnlock(m_pMutexAlarmLock);
//			iRet = iThreadGetAlarmRet;
//			break;
//		}
//			// TODO 暂时去掉接受状态，等所有返回后再返回
//// 		else if(mGetAlarmsThreadStatus == eM_Thread_DataIsReving)
//// 		{
//// 			if(pQueryRange->uiToIndex <= m_vecAllAlarmDev.size()  )
//// 			{
//// 				IVS_UINT32 uiSize = (pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1;
//// 				iRet = GetAlarmsInfo(uiSize, pList, pQueryRange);
//// 				VOS_MutexUnlock(m_pMutexAlarmLock);
//// 				break;
//// 			}
//// 		}
//		else if(mGetAlarmsThreadStatus == eM_Thread_DataIsReady)
//		{
//			IVS_UINT32 uiToIndex = 0;
//
//
//            // DTS2013080810040
//            // edit by wanglei 00165153:2013.8.14 如果pQueryRange->uiFromIndex > m_vecAllAudioDev.size()
//            // (IVS_UINT32)((m_vecAllAudioDev.size() - pQueryRange->uiFromIndex)+1);  uiToIndex会出现无穷大, 导致后续进程崩溃
//            if (pQueryRange->uiFromIndex > m_vecAllAlarmDev.size())
//            {
//                (void)VOS_MutexUnlock(m_pMutexAlarmLock);
//
//                BP_RUN_LOG_ERR(IVS_DATA_INVALID, "invalid query range from index","iner item max size:%d, query range from:%d, ",
//                    m_vecAllAlarmDev.size(), pQueryRange->uiFromIndex);
//                return IVS_SUCCEED;
//            }
//
//			if(pQueryRange->uiToIndex > m_vecAllAlarmDev.size())
//			{
//				uiToIndex = (IVS_UINT32)((m_vecAllAlarmDev.size() - pQueryRange->uiFromIndex)+1);
//			}
//			else
//			{
//				uiToIndex = (IVS_UINT32)((pQueryRange->uiToIndex - pQueryRange->uiFromIndex)+1);
//			}
//			iRet = GetAlarmsInfo(uiToIndex, pList, pQueryRange);
//			(void)VOS_MutexUnlock(m_pMutexAlarmLock);
//			break;
//		}
//		(void)VOS_MutexUnlock(m_pMutexAlarmLock);	
//		// 等待1000
//		(void)ACE_OS::sleep(1);
//	}
//
//	/*if(pList->uiTotal > 0)
//	{
//		pList->stIndexRange.uiToIndex = (pQueryRange->uiFromIndex + pList->uiTotal) -1;
//	}*/
//
//	return iRet;
//}

// 获取告警设备列表结构体赋值
IVS_INT32 CDeviceMgr::GetAlarmsInfo(IVS_UINT32 uiSize, IVS_ALARM_BRIEF_INFO_LIST* pList, const IVS_INDEX_RANGE* pQueryRange)
{
	CHECK_POINTER(pList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pQueryRange, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	pList->uiTotal = m_vecAllAlarmDev.size();
	for(IVS_UINT32 i = 0; i < uiSize; i++)
	{
		IVS_ALARM_BRIEF_INFO tempAlarmInfo = m_vecAllAlarmDev.at(pQueryRange->uiFromIndex + i - 1);
		pList->stAlarmBriefInfo[i].uiChannelType = tempAlarmInfo.uiChannelType;
		pList->stAlarmBriefInfo[i].bIsExDomain = tempAlarmInfo.bIsExDomain;
		if (!CToolsHelp::Memcpy(pList->stAlarmBriefInfo[i].cDevGroupCode, IVS_DEVICE_GROUP_LEN, tempAlarmInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAlarmBriefInfo[i].cDevGroupCode, IVS_DEVICE_GROUP_LEN, tempAlarmInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (!CToolsHelp::Memcpy(pList->stAlarmBriefInfo[i].cAlarmCode, IVS_DEV_CODE_LEN, tempAlarmInfo.cAlarmCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAlarmBriefInfo[i].cAlarmCode, IVS_DEV_CODE_LEN, tempAlarmInfo.cAlarmCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (!CToolsHelp::Memcpy(pList->stAlarmBriefInfo[i].cAlarmName, IVS_ALARM_IN_NAME_LEN, tempAlarmInfo.cAlarmName, IVS_ALARM_IN_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAlarmBriefInfo[i].cAlarmName, IVS_ALARM_IN_NAME_LEN, tempAlarmInfo.cAlarmName, IVS_ALARM_IN_NAME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (!CToolsHelp::Memcpy(pList->stAlarmBriefInfo[i].cParentCode, IVS_DEV_CODE_LEN, tempAlarmInfo.cParentCode, IVS_DEV_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAlarmBriefInfo[i].cParentCode, IVS_DEV_CODE_LEN, tempAlarmInfo.cParentCode, IVS_DEV_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (!CToolsHelp::Memcpy(pList->stAlarmBriefInfo[i].cReserve, 32, tempAlarmInfo.cReserve, 32))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAlarmBriefInfo[i].cReserve, 32, tempAlarmInfo.cReserve, 32) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (!CToolsHelp::Memcpy(pList->stAlarmBriefInfo[i].cDomainCode, IVS_DOMAIN_CODE_LEN, tempAlarmInfo.cDomainCode, IVS_DOMAIN_CODE_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pList->stAlarmBriefInfo[i].cDomainCode, IVS_DOMAIN_CODE_LEN, tempAlarmInfo.cDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
	}

	return IVS_SUCCEED;
}

// 获取主设备列表
IVS_INT32 CDeviceMgr::GetMainDeviceList(const IVS_CHAR* pDomainCode,
	const IVS_QUERY_UNIFIED_FORMAT *pQueryFormat, 
	IVS_DEVICE_BRIEF_INFO_LIST* pBuffer, 
	IVS_UINT32 uiBufferSize )
{
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pQueryFormat, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pBuffer, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    CXml xmlReq;
    IVS_QUERY_UNIFIED_FORMAT *pNoConst = const_cast<IVS_QUERY_UNIFIED_FORMAT*>(pQueryFormat);
    // 组装xml，准备发送给smu

    IVS_INT32 iRet = IVS_SUCCEED;
    iRet = CXmlProcess::GetUnifiedFormatQueryXML(pNoConst, xmlReq, pDomainCode);  
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "Get Main DeviceList fail","IVS_SUCCEED != iRet");
        return iRet;
    }

	iRet = CXmlProcess::GetCommSendMgr(xmlReq, m_pUserMgr, NSS_GET_MAIN_DEV_LIST_REQ, BUILSTRING, pDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get CommSendMgr fail", "NA");
		return iRet;
	}

	iRet = CDeviceMgrXMLProcess::GetMainDeviceListParseXML(pBuffer, uiBufferSize, xmlReq);

    return iRet;
}

// 查询主设备详细信息
IVS_INT32 CDeviceMgr::GetDeviceInfo(const IVS_CHAR*  pDomainCode, const IVS_CHAR*  pDevCode,const IVS_DEVICE_INFO* pDeviceInfo)const
{
    CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDeviceInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    CXml xmlReq;
    (IVS_VOID)CDeviceMgrXMLProcess::GetDeviceInfoXML(pDomainCode, pDevCode, xmlReq);
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	IVS_INT32 iRet = CXmlProcess::GetCommBuildRet(xmlReq, NSS_GET_MAIN_DEV_INFO_REQ, BUILSTRING, strSMULinkID);

    return iRet;
}

IVS_INT32 CDeviceMgr::CheckBufferSize( const IVS_UINT32 &uiDevType, const IVS_UINT32 &uiNum, const IVS_UINT32& uiBufferSize)const
{
	IVS_DEBUG_TRACE("");

    IVS_INT32 iRet = IVS_SUCCEED;
    IVS_UINT32 uiCheckBufferSize = 0;

    if (0 == uiNum)
    {
        return IVS_PARA_INVALID;
    }
    
    switch(uiDevType)
    {
    case DEVICE_TYPE_MAIN:
        uiCheckBufferSize = sizeof(IVS_DEVICE_BRIEF_INFO_LIST) + sizeof(IVS_DEVICE_BRIEF_INFO)*(uiNum-1);       
        break;
    case DEVICE_TYPE_CAMERA:
        uiCheckBufferSize = sizeof(IVS_CAMERA_BRIEF_INFO_LIST) + sizeof(IVS_CAMERA_BRIEF_INFO)*(uiNum-1);       
        break;
	case DEVICE_TYPE_ALARM:
	case DEVICE_TYPE_EXTEND_ALARM:
		uiCheckBufferSize = sizeof(IVS_ALARM_BRIEF_INFO_LIST) + sizeof(IVS_ALARM_BRIEF_INFO)*(uiNum-1);       
		break;
	case DEVICE_TYPE_VOICE:
		uiCheckBufferSize = sizeof(IVS_AUDIO_BRIEF_INFO_LIST) + sizeof(IVS_AUDIO_BRIEF_INFO)*(uiNum-1);       
		break;
    default:
        iRet = IVS_PARA_INVALID;
    }

    // 判断是否相等，如若不等，返回输入参数无效
    if (uiCheckBufferSize != uiBufferSize && IVS_SUCCEED == iRet)
    {
        iRet = IVS_PARA_INVALID;
    }

    return iRet;
}

// 根据主设备查询子设备列表
IVS_INT32 CDeviceMgr::GetChannelList( const IVS_CHAR* pDevCode, IVS_DEV_CHANNEL_BRIEF_INFO_LIST* pChannelList, IVS_UINT32 uiBufferSize )const
{
    CHECK_POINTER(pChannelList,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDevCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    IVS_UINT32 uiReqNum = (pChannelList->stIndexRange.uiToIndex - pChannelList->stIndexRange.uiFromIndex) + 1;
    IVS_UINT32 uiBufferNum = (uiBufferSize - sizeof(IVS_DEV_CHANNEL_BRIEF_INFO_LIST))/sizeof(IVS_DEV_CHANNEL_BRIEF_INFO) + 1;

    // 通道最大的数量是256
    if(uiReqNum != uiBufferNum || uiReqNum > IVS_MAX_DEVICE_CHANNEL_NUM)
    {        
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get ChannelList fail", "uiReqNum=%u,uiBufferNum=%u", uiReqNum, uiBufferNum);
        return IVS_OPERATE_MEMORY_ERROR;
    }

    IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1];
    IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];

    (IVS_VOID)CXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
    CXml xmlTemp;
    (IVS_VOID)CDeviceMgrXMLProcess::GetChannelListGetXML(chDomaCode, chDevCode, xmlTemp);

	IVS_INT32 iRet = CXmlProcess::GetCommSendMgr(xmlTemp, m_pUserMgr, NSS_GET_CHANNEL_INFO_REQ, BUILSTRING, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SendCmd fail", "NA");
		return iRet;
	}

	iRet = CDeviceMgrXMLProcess::GetChannelListParseXML(xmlTemp, pChannelList);

    return iRet;
}

// 修改主设备名称
IVS_INT32 CDeviceMgr::SetMainDeviceName(const IVS_CHAR* pDevCode,const IVS_CHAR* pNewDevName)const
{
	CHECK_POINTER(pDevCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNewDevName,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 根据pDevCode解析出域号和设备号
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1];
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
	(IVS_VOID)CXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);

	CXml xmlReq;
	(IVS_VOID)CDeviceMgrXMLProcess::SetMainDeviceNameGetXML(chDomaCode,chDevCode,pNewDevName,xmlReq);

	IVS_INT32 iRet = CXmlProcess::GetCommSendMgr(xmlReq, m_pUserMgr, NSS_SET_MAIN_DEV_NAME_REQ, BUILDRET, chDomaCode);

	return iRet;
}

// 修改摄像机名称
IVS_INT32 CDeviceMgr::SetCameraName(const IVS_CHAR* pCameraCode,const IVS_CHAR* pNewCameraName)const
{
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNewCameraName,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("");

	// 根据pCameraCode解析出域号和设备号
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1];
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	// 组装请求xml
	CXml xmlReq;
	(IVS_VOID)CDeviceMgrXMLProcess::SetCameraNameGetXML(chDomaCode,chDevCode,pNewCameraName,xmlReq);

	IVS_INT32 iRet = CXmlProcess::GetCommSendMgr(xmlReq, m_pUserMgr, NSS_SET_CAMERA_NAME_REQ, BUILDRET, chDomaCode);

	return iRet;
}

// 摄像机信息里面的strCameraCode是带域编码，查询的时候传入的可以是带域编码的 也可以是不带域编码的，默认是不带域编码
IVS_INT32 CDeviceMgr::GetCameraBriefInfobyCode(const std::string& strCameraCode, IVS_CAMERA_BRIEF_INFO& stCameraBriefInfo, bool bHasDomainCode/* = false*/)
{
	IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;
    // 根据设备编码查找摄像机信息
	IVS_UINT32 uiSize = m_vecAllCameraDev.size();
	for(IVS_UINT32 i = 0; i < uiSize; i++)
	{
		IVS_CAMERA_BRIEF_INFO tempCamera = m_vecAllCameraDev.at(i);
		// 这个是带域编码
        IVS_CHAR cTempCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
        (void)CToolsHelp::Memcpy(cTempCameraCode, IVS_DEV_CODE_LEN, tempCamera.cCode, IVS_DEV_CODE_LEN);
		std::string strDstCameraCode = cTempCameraCode; 
		std::string strCmpCameraCode;
		if (bHasDomainCode)
		{
			strCmpCameraCode = strDstCameraCode;
		}
		else
		{
			IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
			IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
			CXmlProcess::ParseDevCode(strDstCameraCode.c_str(), chDevCode, chDomaCode);
			// 这个是不带域编码的
			strCmpCameraCode = chDevCode; 
		}
		if (strCameraCode.compare(strCmpCameraCode) == 0)
		{
			stCameraBriefInfo.bEnableVoice = tempCamera.bEnableVoice;
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cCameraLocation, IVS_DESCRIBE_LEN, tempCamera.cCameraLocation, IVS_DESCRIBE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cCameraLocation, IVS_DESCRIBE_LEN, tempCamera.cCameraLocation, IVS_DESCRIBE_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cCode, IVS_DEV_CODE_LEN, tempCamera.cCode, IVS_DEV_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cCode, IVS_DEV_CODE_LEN, tempCamera.cCode, IVS_DEV_CODE_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cName, IVS_CAMERA_NAME_LEN, tempCamera.cName, IVS_CAMERA_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cName, IVS_CAMERA_NAME_LEN, tempCamera.cName, IVS_CAMERA_NAME_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cNvrCode, IVS_DEV_CODE_LEN, tempCamera.cNvrCode, IVS_DEV_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cNvrCode, IVS_DEV_CODE_LEN, tempCamera.cNvrCode, IVS_DEV_CODE_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cParentCode, IVS_DEV_CODE_LEN, tempCamera.cParentCode, IVS_DEV_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cParentCode, IVS_DEV_CODE_LEN, tempCamera.cParentCode, IVS_DEV_CODE_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cReserve, 32, tempCamera.cReserve, 32))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cReserve, 32, tempCamera.cReserve, 32) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stCameraBriefInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, tempCamera.cDevGroupCode, IVS_DEVICE_GROUP_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stCameraBriefInfo.cDevGroupCode, IVS_DEVICE_GROUP_LEN, tempCamera.cDevGroupCode, IVS_DEVICE_GROUP_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			stCameraBriefInfo.uiCameraStatus = tempCamera.uiCameraStatus;
			//stCameraBriefInfo.uiGroupID = tempCamera.uiGroupID;
			stCameraBriefInfo.uiStatus = tempCamera.uiStatus;
			stCameraBriefInfo.uiType = tempCamera.uiType;
			iRet = IVS_SUCCEED;
			break;
		}
	}	

    return iRet;
}

// 根据域编码找到第一个NVR，并将其移到列表尾部
IVS_INT32 CDeviceMgr::GetFirstNVR(const std::string& strDomainCode, IVS_DEVICE_BASIC_INFO& stDeviceBasicInfo)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;
	// TODO 通用请求; zhouyunping
	(void)VOS_MutexLock(m_pMutexNVRLock);
	// 根据DomainCode找到DEVICE_BRIEF_INFO_LIST
	DEVICE_BRIEF_INFO_LIST_MAP_ITER IpIter;
	IpIter = m_DeviceInfoListMap.find(strDomainCode);

	if (IpIter!=m_DeviceInfoListMap.end())
	{
		DEVICE_BRIEF_INFO_LIST* pDeviceBriefInfoList = dynamic_cast<DEVICE_BRIEF_INFO_LIST*>(IpIter->second);//lint !e611
		if (NULL == pDeviceBriefInfoList)
		{
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get First NVR", "pDeviceBriefInfoList = NULL");
			(void)VOS_MutexUnlock(m_pMutexNVRLock);
			return IVS_OPERATE_MEMORY_ERROR;
		}
		// 获取DEVICE_BRIEF_INFO_LIST的Size
		IVS_UINT32 ulSize = pDeviceBriefInfoList->size();
		// size大于0才有意义
		if (ulSize>0)
		{
			// 取出第一个NVRInfo
			DEVICE_BRIEF_INFO_LIST_ITER InfoIter;
			InfoIter = pDeviceBriefInfoList->begin();
			IVS_DEVICE_BRIEF_INFO* pDeviceBriefInfo = (IVS_DEVICE_BRIEF_INFO*)(*InfoIter);
			if (NULL == pDeviceBriefInfo)
			{
				BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get First NVR", "pDeviceBriefInfo = NULL");
				(void)VOS_MutexUnlock(m_pMutexNVRLock);
				return IVS_OPERATE_MEMORY_ERROR;
			}

			// 删除该项
			pDeviceBriefInfoList->erase(InfoIter);
			// 加入到列表尾部
			pDeviceBriefInfoList->push_back(pDeviceBriefInfo);

			// 拷贝内容到IVS_DEVICE_BASIC_INFO
			stDeviceBasicInfo.uiType = pDeviceBriefInfo->stDeviceInfo.uiType;
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN, pDeviceBriefInfo->stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN, pDeviceBriefInfo->stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN) fail", "NA");
				(void)VOS_MutexUnlock(m_pMutexNVRLock);
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cCode, IVS_DEV_CODE_LEN, pDeviceBriefInfo->stDeviceInfo.cCode, IVS_DEV_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cCode, IVS_DEV_CODE_LEN, pDeviceBriefInfo->stDeviceInfo.cCode, IVS_DEV_CODE_LEN) fail", "NA");
				(void)VOS_MutexUnlock(m_pMutexNVRLock);
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cName, IVS_DEVICE_NAME_LEN, pDeviceBriefInfo->stDeviceInfo.cName, IVS_DEVICE_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cName, IVS_DEVICE_NAME_LEN, pDeviceBriefInfo->stDeviceInfo.cName, IVS_DEVICE_NAME_LEN) fail", "NA");
				(void)VOS_MutexUnlock(m_pMutexNVRLock);
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, pDeviceBriefInfo->stDeviceInfo.cDomainCode, IVS_DOMAIN_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, pDeviceBriefInfo->stDeviceInfo.cDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
				(void)VOS_MutexUnlock(m_pMutexNVRLock);
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, pDeviceBriefInfo->stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, pDeviceBriefInfo->stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN) fail", "NA");
				(void)VOS_MutexUnlock(m_pMutexNVRLock);
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cModel, IVS_DEV_MODEL_LEN, pDeviceBriefInfo->stDeviceInfo.cModel, IVS_DEV_MODEL_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cModel, IVS_DEV_MODEL_LEN, pDeviceBriefInfo->stDeviceInfo.cModel, IVS_DEV_MODEL_LEN) fail", "NA");
				(void)VOS_MutexUnlock(m_pMutexNVRLock);
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.stIP.cIP, IVS_IP_LEN, pDeviceBriefInfo->stDeviceInfo.stIP.cIP, IVS_IP_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.stIP.cIP, IVS_IP_LEN, pDeviceBriefInfo->stDeviceInfo.stIP.cIP, IVS_IP_LEN) fail", "NA");
				(void)VOS_MutexUnlock(m_pMutexNVRLock);
				return IVS_ALLOC_MEMORY_ERROR;
			}
			stDeviceBasicInfo.stIP.uiIPType = pDeviceBriefInfo->stDeviceInfo.stIP.uiIPType;
			stDeviceBasicInfo.uiPort = pDeviceBriefInfo->stDeviceInfo.uiPort;
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cReserve, 32, pDeviceBriefInfo->stDeviceInfo.cReserve, 32))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cReserve, 32, pDeviceBriefInfo->stDeviceInfo.cReserve, 32) fail", "NA");
				(void)VOS_MutexUnlock(m_pMutexNVRLock);
				return IVS_ALLOC_MEMORY_ERROR;
			}
			// 操作成功返回成功
			iRet = IVS_SUCCEED;
		}
	}

	(void)VOS_MutexUnlock(m_pMutexNVRLock);
	// 查找第一个NVR

	// 插入到尾部;

	return iRet;
}

// 根据域编码获取list长度
IVS_UINT32 CDeviceMgr::GetNVRSize(const std::string& strDomainCode)
{
	IVS_UINT32 iSize = 0;
	// TODO 通用请求; zhouyunping
	(void)VOS_MutexLock(m_pMutexNVRLock);

	DEVICE_BRIEF_INFO_LIST* pDeviceBriefInfoList;
	DEVICE_BRIEF_INFO_LIST_MAP_ITER IpIter;
	IpIter = m_DeviceInfoListMap.find(strDomainCode);
	if (IpIter!=m_DeviceInfoListMap.end())
	{
		pDeviceBriefInfoList = dynamic_cast<DEVICE_BRIEF_INFO_LIST*>(IpIter->second);//lint !e611
		if (NULL == pDeviceBriefInfoList)
		{
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get First NVR", "pDeviceBriefInfoList = NULL");
			(void)VOS_MutexUnlock(m_pMutexNVRLock);
			return IVS_OPERATE_MEMORY_ERROR;
		}
		iSize = pDeviceBriefInfoList->size();
	}

	(void)VOS_MutexUnlock(m_pMutexNVRLock);
	return iSize;
}

// 查询网元列表
IVS_INT32 CDeviceMgr::GetNVRList(const IVS_CHAR* pDomainCode,IVS_UINT32 uiServerType,const IVS_INDEX_RANGE* pIndex,IVS_DEVICE_BRIEF_INFO_LIST* pNvrList, IVS_UINT32 uiBufferSize)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNvrList, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("pDomainCode: %s,addr:%d",pDomainCode, &pDomainCode);

	CXml xmlReq;
	// 拼装XML查询请求
	IVS_INT32 iRet = IVS_FAIL;
	iRet = CDeviceMgrXMLProcess::GetNVRListReqXML(pDomainCode,pIndex,uiServerType,xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get NVR List fail","Get NVR List Req XML failed");
		return iRet;
	}
	CXml xmlRsp;
	iRet = CXmlProcess::GetCommSendMgr(xmlReq, xmlRsp, m_pUserMgr, NSS_GET_NVR_LIST_REQ, BUILSTRING, pDomainCode);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "GetNVRList fail","Send Cmd failed");
        return iRet;
    }

	//解析查询请求XML
	iRet = CDeviceMgrXMLProcess::ParseNVRListRsp(xmlRsp,pNvrList,uiBufferSize);

	return iRet;
}

// 获取所有的NVR消息
IVS_INT32 CDeviceMgr::GetAllNVRInfo(const IVS_CHAR* pDomainCode,IVS_UINT32 uiServerType)
{
	CHECK_POINTER(pDomainCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = IVS_FAIL;
	IVS_INDEX_RANGE stIndex;
	memset(&stIndex,0,sizeof(IVS_INDEX_RANGE));
	stIndex.uiFromIndex = 1;
	stIndex.uiToIndex = QUERY_INDEX_COUNT;
	IVS_BOOL bIsQueryContinnue = IVS_FAIL;
	IVS_UINT32 ulQueryCount = 0;
	do 
	{
		//为结果LIST分配内存
		IVS_UINT32 uiBufSize = sizeof(IVS_DEVICE_BRIEF_INFO_LIST) + (stIndex.uiToIndex - stIndex.uiFromIndex) * sizeof(IVS_DEVICE_BRIEF_INFO);
		IVS_CHAR* pNVRListBuf = IVS_NEW(pNVRListBuf, uiBufSize);
		CHECK_POINTER(pNVRListBuf, IVS_OPERATE_MEMORY_ERROR);
		memset(pNVRListBuf, 0, uiBufSize);
		IVS_DEVICE_BRIEF_INFO_LIST* pNvrList = reinterpret_cast<IVS_DEVICE_BRIEF_INFO_LIST*>(pNVRListBuf);//lint !e826
		//CHECK_POINTER(pNvrList, IVS_OPERATE_MEMORY_ERROR);
		//调用查询NVR列表接口进行查询
		iRet = GetNVRList(pDomainCode, uiServerType, &stIndex, pNvrList, uiBufSize);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Get All NVR Info","Query failed");
			IVS_DELETE(pNVRListBuf, MUILI);
			return iRet;
		}

		//临时的设备列表
		DEVICE_BRIEF_INFO_LIST* pDeviceBriefInfoList = IVS_NEW(pDeviceBriefInfoList);//lint !e830
		if (NULL == pDeviceBriefInfoList)
		{
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "NULL == pDeviceBriefInfoList", "NA");
			IVS_DELETE(pNVRListBuf, MUILI);
			return IVS_OPERATE_MEMORY_ERROR;
		}

		//把查询回来的信息全部拷贝到临时列表中
		for (IVS_UINT32 i = 0; i < pNvrList->uiTotal; i++)
		{
			IVS_DEVICE_BRIEF_INFO* ptDstDeviceBriefInfo = IVS_NEW(ptDstDeviceBriefInfo);
			if (NULL == ptDstDeviceBriefInfo)
			{
				BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "NULL == ptDstDeviceBriefInfo", "NA");
				IVS_DELETE(pNVRListBuf, MUILI);
				return IVS_OPERATE_MEMORY_ERROR;
			}

			IVS_DEVICE_BRIEF_INFO& stSrcDeviceBriefInfo = pNvrList->stDeviceBriefInfo[i];
			(void)CToolsHelp::Memcpy(ptDstDeviceBriefInfo, sizeof(IVS_DEVICE_BRIEF_INFO), &stSrcDeviceBriefInfo, sizeof(IVS_DEVICE_BRIEF_INFO));

			(void)VOS_MutexLock(m_pMutexNVRLock);
			pDeviceBriefInfoList->push_back(ptDstDeviceBriefInfo);
			(void)VOS_MutexUnlock(m_pMutexNVRLock);
		}

		(void)VOS_MutexLock(m_pMutexNVRLock);
		DEVICE_BRIEF_INFO_LIST_MAP_ITER IpIter = m_DeviceInfoListMap.find(pDomainCode);
		if (IpIter != m_DeviceInfoListMap.end())
		{
			//如果查到，插入
			DEVICE_BRIEF_INFO_LIST* pDeviceBriefInfoList2 = dynamic_cast<DEVICE_BRIEF_INFO_LIST*>(IpIter->second); //lint !e611
			if (NULL == pDeviceBriefInfoList2)
			{
				BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get All NVR Info", "pDeviceBriefInfoList2 = NULL.");
				(void)VOS_MutexUnlock(m_pMutexNVRLock);
				IVS_DELETE(pNVRListBuf, MUILI);
				return IVS_OPERATE_MEMORY_ERROR;
			}

			//遍历查询到的所有设备信息并拷贝内容
			for (IVS_UINT32 i = 0; i < pNvrList->uiTotal; i++)
			{
				IVS_DEVICE_BRIEF_INFO* ptDstDeviceBriefInfo = IVS_NEW(ptDstDeviceBriefInfo);
				if (NULL == ptDstDeviceBriefInfo)
				{
					BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get All NVR Info", "ptDstDeviceBriefInfo = NULL.");
					(void)VOS_MutexUnlock(m_pMutexNVRLock);
					IVS_DELETE(pNVRListBuf, MUILI);
					return IVS_OPERATE_MEMORY_ERROR;
				}

				IVS_DEVICE_BRIEF_INFO& stSrcDeviceBriefInfo = pNvrList->stDeviceBriefInfo[i];
				(void)CToolsHelp::Memcpy(ptDstDeviceBriefInfo, sizeof(IVS_DEVICE_BRIEF_INFO), &stSrcDeviceBriefInfo, sizeof(IVS_DEVICE_BRIEF_INFO));

				//将当前查询回来的设备信息全出插入到列表
				pDeviceBriefInfoList2->push_back(ptDstDeviceBriefInfo);
			}
		}
		else
		{
			(void)m_DeviceInfoListMap.insert(make_pair(pDomainCode, pDeviceBriefInfoList));
		}

		(void)VOS_MutexUnlock(m_pMutexNVRLock);

		//如果查询回来的总个数比请求结束个数小，终止查询//
		if (pNvrList->uiTotal <= stIndex.uiToIndex)
		{
			bIsQueryContinnue = IVS_SUCCEED;
			IVS_DELETE(pNVRListBuf, MUILI);
			break;
		}
		else
		{
			//否则索引向后移继续查询
			ulQueryCount++;
			stIndex.uiFromIndex += ulQueryCount * QUERY_INDEX_COUNT;
			stIndex.uiToIndex += ulQueryCount * QUERY_INDEX_COUNT;
		}
		IVS_DELETE(pNVRListBuf, MUILI);
	} while (!bIsQueryContinnue);

	return iRet;
}


// 根据域编码和NVRCode获取NVR信息
IVS_INT32 CDeviceMgr::GetNVRInfoByDomainCodeAndNVRCode(const std::string& strDomainCode,const std::string& strNVRCode,IVS_DEVICE_BASIC_INFO& stDeviceBasicInfo)
{
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	// 加锁
	(void)VOS_MutexLock(m_pMutexNVRLock);
	// Map的迭代器
	DEVICE_BRIEF_INFO_LIST_MAP_ITER IpIter;
	// 根据DoaminCode查询Map
	IpIter = m_DeviceInfoListMap.find(strDomainCode);
	// 找到了
	if (IpIter!=m_DeviceInfoListMap.end())
	{
		// 先从Map里根据DomainCode查到DEVICE_BRIEF_INFO_LIST
		DEVICE_BRIEF_INFO_LIST* pDeviceBriefInfoList = dynamic_cast<DEVICE_BRIEF_INFO_LIST*>(IpIter->second);//lint !e611
		if (NULL == pDeviceBriefInfoList)
		{
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get NVR Info By Domain Code And NVR Code", "pDeviceBriefInfoList = NULL.");
			(void)VOS_MutexUnlock(m_pMutexNVRLock);
			return IVS_OPERATE_MEMORY_ERROR;
		}
		DEVICE_BRIEF_INFO_LIST_ITER InfoIter;
		// 获取DEVICE_BRIEF_INFO_LIST的size
		IVS_UINT32 ulSize = pDeviceBriefInfoList->size();
		// size大于0才有意义
		if (ulSize>0)
		{
			// 根据NVRCode去遍历DEVICE_BRIEF_INFO_LIST查到IVS_DEVICE_BASIC_INFO
			//InfoIter = std::_Find_if(pDeviceBriefInfoList->begin(),pDeviceBriefInfoList->end(), finder_t(strNVRCode));
			IVS_DEVICE_BRIEF_INFO* pDeviceBriefInfo = NULL;
			iRet = GetBasicInfoByNVRCode(pDeviceBriefInfo, pDeviceBriefInfoList, stDeviceBasicInfo, InfoIter, strNVRCode);
		}
	}

	//解锁
	(void)VOS_MutexUnlock(m_pMutexNVRLock);

	return iRet;
}

// 根据NVRCode去遍历DEVICE_BRIEF_INFO_LIST查到IVS_DEVICE_BASIC_INFO
IVS_INT32 CDeviceMgr::GetBasicInfoByNVRCode(IVS_DEVICE_BRIEF_INFO*& pDeviceBriefInfo, DEVICE_BRIEF_INFO_LIST* pDeviceBriefInfoList, IVS_DEVICE_BASIC_INFO& stDeviceBasicInfo, DEVICE_BRIEF_INFO_LIST_ITER InfoIter, const std::string& strNVRCode)const
{
	CHECK_POINTER(pDeviceBriefInfoList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	for (InfoIter=pDeviceBriefInfoList->begin();InfoIter != pDeviceBriefInfoList->end();InfoIter++)
	{
		// 找到了,拷贝
		pDeviceBriefInfo = (IVS_DEVICE_BRIEF_INFO*)(*InfoIter);
		CHECK_POINTER(pDeviceBriefInfo,IVS_OPERATE_MEMORY_ERROR);
		if (!strNVRCode.compare(pDeviceBriefInfo->stDeviceInfo.cCode))
		{
			// 拷贝内容
			stDeviceBasicInfo.uiType = pDeviceBriefInfo->stDeviceInfo.uiType;
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN, pDeviceBriefInfo->stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN, pDeviceBriefInfo->stDeviceInfo.cSerialNumber, IVS_DEV_SERIALNO_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cCode, IVS_DEV_CODE_LEN, pDeviceBriefInfo->stDeviceInfo.cCode, IVS_DEV_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cCode, IVS_DEV_CODE_LEN, pDeviceBriefInfo->stDeviceInfo.cCode, IVS_DEV_CODE_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cName, IVS_DEVICE_NAME_LEN, pDeviceBriefInfo->stDeviceInfo.cName, IVS_DEVICE_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cName, IVS_DEVICE_NAME_LEN, pDeviceBriefInfo->stDeviceInfo.cName, IVS_DEVICE_NAME_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, pDeviceBriefInfo->stDeviceInfo.cDomainCode, IVS_DOMAIN_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cDomainCode, IVS_DOMAIN_CODE_LEN, pDeviceBriefInfo->stDeviceInfo.cDomainCode, IVS_DOMAIN_CODE_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			//stDeviceBasicInfo.uiVendorType = pDeviceBriefInfo->stDeviceInfo.uiVendorType;
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, pDeviceBriefInfo->stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN, pDeviceBriefInfo->stDeviceInfo.cVendorType, IVS_MAX_VENDOR_TYPE_NAME_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cModel, IVS_DEV_MODEL_LEN, pDeviceBriefInfo->stDeviceInfo.cModel, IVS_DEV_MODEL_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cModel, IVS_DEV_MODEL_LEN, pDeviceBriefInfo->stDeviceInfo.cModel, IVS_DEV_MODEL_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.stIP.cIP, IVS_IP_LEN, pDeviceBriefInfo->stDeviceInfo.stIP.cIP, IVS_IP_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.stIP.cIP, IVS_IP_LEN, pDeviceBriefInfo->stDeviceInfo.stIP.cIP, IVS_IP_LEN) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			stDeviceBasicInfo.stIP.uiIPType = pDeviceBriefInfo->stDeviceInfo.stIP.uiIPType;

			stDeviceBasicInfo.uiPort = pDeviceBriefInfo->stDeviceInfo.uiPort;
			if (!CToolsHelp::Memcpy(stDeviceBasicInfo.cReserve, 32, pDeviceBriefInfo->stDeviceInfo.cReserve, 32))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(stDeviceBasicInfo.cReserve, 32, pDeviceBriefInfo->stDeviceInfo.cReserve, 32) fail", "NA");
				return IVS_ALLOC_MEMORY_ERROR;
			}
			// 如果查到了，修改返回值
			iRet = IVS_SUCCEED;
		}
	}

	return iRet;
}

// 根据域编码修改设备状态
IVS_INT32 CDeviceMgr::ModifyDevStatusByDomainCode(const std::string& /*strDomainCode*/, const std::string& strCameraCode, const IVS_UINT32& uiStatus, const std::string& strNVRCode)
{
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	// 加锁
	(void)VOS_MutexLock(m_pMutexCameraLock);

	// 根据设备编码查找摄像机信息
	IVS_UINT32 uisize = m_vecAllCameraDev.size();
	for(IVS_UINT32 i = 0;i<uisize;i++)
	{
		IVS_CAMERA_BRIEF_INFO tempCamera = m_vecAllCameraDev.at(i);
		// 这个是带域编码
		std::string strDstCameraCode = tempCamera.cCode; 

		IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
		IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
		CXmlProcess::ParseDevCode(strDstCameraCode.c_str(), chDevCode, chDomaCode);
		// 这个是不带域编码的
		std::string strCmpCameraCode = chDevCode; 

		if (strCameraCode.compare(strCmpCameraCode) == 0)
		{
			m_vecAllCameraDev.at(i).uiStatus = uiStatus;
			memset(m_vecAllCameraDev.at(i).cNvrCode, 0x0,IVS_NVR_CODE_LEN);
			if (!CToolsHelp::Memcpy(m_vecAllCameraDev.at(i).cNvrCode, IVS_NVR_CODE_LEN, strNVRCode.c_str(), IVS_NVR_CODE_LEN))
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(m_vecAllCameraDev.at(i).cNvrCode, IVS_NVR_CODE_LEN, strNVRCode.c_str(), IVS_NVR_CODE_LEN) fail", "NA");
				VOS_MutexUnlock(m_pMutexCameraLock);
				return IVS_ALLOC_MEMORY_ERROR;
			}
			iRet = IVS_SUCCEED;
			break;
		}
	}
	
	// 解锁
	(void)VOS_MutexUnlock(m_pMutexCameraLock);

	return iRet;
}

// 根据域编码干掉对应的列表
IVS_INT32 CDeviceMgr::DeleteNVRListByDomainCode(const std::string& strDomainCode)
{
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	// 加锁
	(void)VOS_MutexLock(m_pMutexNVRLock);
	// Map的迭代器
	DEVICE_BRIEF_INFO_LIST_MAP_ITER IpIter;
	// 根据DoaminCode查询Map
	IpIter = m_DeviceInfoListMap.find(strDomainCode);
	// 找到了
	if (IpIter != m_DeviceInfoListMap.end())
	{
		// 根据域编码遍历map
		DEVICE_BRIEF_INFO_LIST* pDeviceBriefInfoList = dynamic_cast<DEVICE_BRIEF_INFO_LIST*>(IpIter->second);//lint !e611
        if (NULL == pDeviceBriefInfoList)
        {
            BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "pDeviceBriefInfoList fail", "pDeviceBriefInfoList is null");
            m_DeviceInfoListMap.erase(IpIter);
            // 删掉
            (void)VOS_MutexUnlock(m_pMutexNVRLock);
            return IVS_OPERATE_MEMORY_ERROR;
        }

		// 遍历列表中的所有结构并删除之
		IVS_DEVICE_BRIEF_INFO* pDeviceBriefInfo = NULL;
		DEVICE_BRIEF_INFO_LIST_ITER InfoIter = pDeviceBriefInfoList->begin();
		DEVICE_BRIEF_INFO_LIST_ITER InfoIterEnd = pDeviceBriefInfoList->end();
		for (; InfoIter != InfoIterEnd; InfoIter++)
		{
			pDeviceBriefInfo = (IVS_DEVICE_BRIEF_INFO*)(*InfoIter);
            if (NULL == pDeviceBriefInfo)
            {
                continue;
            }
			IVS_DELETE(pDeviceBriefInfo);
		}

		//删除列表指针
		IVS_DELETE(pDeviceBriefInfoList);
        m_DeviceInfoListMap.erase(IpIter);
        iRet = IVS_SUCCEED;
	}
	(void)VOS_MutexUnlock(m_pMutexNVRLock);
	return iRet;
}

// 重启前端设备
IVS_INT32 CDeviceMgr::RestartDevice(const IVS_CHAR* pDeviceCode, IVS_UINT32 uiRebootType)const
{
	CHECK_POINTER(pDeviceCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1];
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
	(IVS_VOID)CXmlProcess::ParseDevCode(pDeviceCode, chDevCode, chDomaCode);

	CXml xmlReq;
	IVS_INT32 iRet = CDeviceMgrXMLProcess::RestartDeviceGetXML(chDomaCode, chDevCode, uiRebootType, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Restart Device GetXML fail","IVS_SUCCEED != RestartDevice");
		return iRet;
	}

	iRet = CXmlProcess::GetCommSendMgr(xmlReq, m_pUserMgr, NSS_DEVICE_REBOOT_REQ, BUILDRET, chDomaCode);

	return iRet;
}

// 启动搜索前端设备
IVS_INT32 CDeviceMgr::StartSearchDevice(const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pSearchInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	std::string strDomainCode;
	if (NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		//为空则默认本域
		m_pUserMgr->GetDomainCode(strDomainCode);
	}
	else
	{
		strDomainCode = pDomainCode;
	}

	IVS_DOMAIN_ROUTE stDomainRoute;
	CCmd *pCmd = NULL;
	CXml xml;
	if (!xml.Parse(pSearchInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pSearchInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
    
	// 获取指定域SMU连接
    std::string strSMULinkID = "";
	IVS_INT32 iRet = m_pUserMgr->GetSMULinkID(strSMULinkID, strDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get SMU LinkID failed", "NA");
		return iRet;
	}
	const IVS_CHAR* pAddp = "Content/SearchInfo/ServerCode";
	iRet = CDeviceMgrXMLProcess::SearchDeviceParseXML(xml, pAddp);
	if (IVS_SUCCEED != iRet)
	{
		CDomainRouteMgr& oDomainRoute = m_pUserMgr->GetDomainRouteMgr();
		iRet = oDomainRoute.GetDomainRoutebyCode(strDomainCode, stDomainRoute);
		std::string strServerCode;
		if (1 == stDomainRoute.uiDomainType)
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "ServerCode is NULL", "ServerCode fail");
			return IVS_XML_INVALID;
		}
		if (2 == stDomainRoute.uiDomainType)
		{
			// 构造带域的请求消息，并发送
			CSendNssMsgInfo sendNssMsgInfo;
			sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
			sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
			sendNssMsgInfo.SetReqID(NSS_CLUSTER_FIND_IDLEST_NVR_REQ);
			sendNssMsgInfo.SetReqData("");	
			sendNssMsgInfo.SetDomainCode(strDomainCode.c_str());
			std::string strpRsp;
			IVS_INT32 iNeedRedirect = IVS_FAIL;
			iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
			if (IVS_SUCCEED != iRet)
			{
				BP_RUN_LOG_ERR(iRet, "SendCmd fail", "NA");
				return iRet;
			}
			CXml pRsqXml;
			if (!pRsqXml.Parse(strpRsp.c_str()))
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pSearchInfo) fail", "NA");
				return IVS_XML_INVALID;
			}
			iRet = CDeviceMgrXMLProcess::FindIdlestNvrParseXML(strServerCode, pRsqXml);
		}
		m_pUserMgr->SetDevServerCode(strServerCode);
		const IVS_CHAR* pAddr = "Content/SearchInfo/ServerCode";
		iRet = CDeviceMgrXMLProcess::ModifyServerCode(xml, strServerCode, pAddr);
		IVS_UINT32 uiLen = 0;
		const IVS_CHAR* pReq = xml.GetXMLStream(uiLen);
		pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_START_SEARCH_DEVICE_REQ, pReq, strSMULinkID);
	}
	else
	{
		pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_START_SEARCH_DEVICE_REQ, pSearchInfo, strSMULinkID);
	}
		
	CHECK_POINTER(pCmd,IVS_OPERATE_MEMORY_ERROR);

	m_pUserMgr->SetDevTransID(pCmd->GetTransID());//lint !e1013 !e1055 !e746 !e64
	// 发送nss协议
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp,IVS_NET_RECV_TIMEOUT);

	// 返回解析报文错误码
	iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);

	return iRet;
}

// 停止搜索前端设备
IVS_INT32 CDeviceMgr::StopSearchDevice(const IVS_CHAR* pDomainCode, const IVS_CHAR* pSearchInfo)const
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pSearchInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	std::string strDomainCode;
	if (NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		//为空则默认本域
		m_pUserMgr->GetDomainCode(strDomainCode);
	}
	else
	{
		strDomainCode = pDomainCode;
	}

    // 获取指定域SMU连接
    std::string strSMULinkID = "";
	IVS_INT32 iRet = m_pUserMgr->GetSMULinkID(strSMULinkID, strDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get SMU LinkID failed", "NA");
		return iRet;
	}
	std::string strTransID;
	std::string strServerCode;
	m_pUserMgr->GetDevTransID(strTransID);
	m_pUserMgr->GetDevServerCode(strServerCode);

	CXml xml;
	if (!xml.Parse(pSearchInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pSearchInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* pAddp = "Content/ServerCode";
	iRet = CDeviceMgrXMLProcess::SearchDeviceParseXML(xml, pAddp);
	if (IVS_SUCCEED != iRet)
	{
		const IVS_CHAR* pAddr = "Content/ServerCode";
		iRet = CDeviceMgrXMLProcess::ModifyServerCode(xml, strServerCode, pAddr);
	}
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xml.GetXMLStream(uiLen);
	// 拼装发送报文
	CCmd *pCmd = CNSSOperator::instance().BuildCmd(strSMULinkID, NET_ELE_SMU_NSS, NSS_STOP_SEARCH_DEVICE_REQ, pReq, strTransID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	// 发送nss协议
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);

	// 返回解析报文错误码
	iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);

	return iRet;
}

// 验证前端设备
IVS_INT32 CDeviceMgr::VerifyDevice(const IVS_CHAR* pDomainCode, const IVS_CHAR* pVerifyInfo, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pVerifyInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");


	std::string strDomainCode;
	if (NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		//为空则默认本域
		m_pUserMgr->GetDomainCode(strDomainCode);
	}
	else
	{
		strDomainCode = pDomainCode;
	}
	CXml xml;
	if (!xml.Parse(pVerifyInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlCode.Parse(pVerifyInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	if (!xml.FindElemEx("Content/ValidateInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlCode.FindElemEx(Content/ValidateInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* pTemp = NULL;
	IVS_CHAR cServerCode[IVS_NVR_CODE_LEN + 1] = {0};
	GET_ELEM_VALUE_CHAR("ServerCode", pTemp, cServerCode, IVS_NVR_CODE_LEN, xml);

	// 判断是集群或堆叠
	IVS_DOMAIN_ROUTE stDomainRoute;
	CDomainRouteMgr& oDomainRoute = m_pUserMgr->GetDomainRouteMgr();
	IVS_INT32 iRet = oDomainRoute.GetDomainRoutebyCode(strDomainCode, stDomainRoute);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Domain Route Info By Code fail", "NA");
		return iRet;
	}
	// 集群情况
	if (2 == stDomainRoute.uiDomainType)
	{
		iRet = VerifyDeviceMsg(strDomainCode.c_str(), cServerCode, pVerifyInfo, pRspXml, BP::DOMAIN_CLUSTER);

		return iRet;
	}
	// 堆叠模式、单机模式
	iRet = VerifyDeviceMsg(strDomainCode.c_str(), cServerCode, pVerifyInfo, pRspXml);

	return iRet;
}


// 验证前端设备(集群、堆叠)
IVS_INT32 CDeviceMgr::VerifyDeviceMsg(const IVS_CHAR* pDomainCode, const IVS_CHAR* pServerCode, const IVS_CHAR* pVerifyInfo, IVS_CHAR** pRspXml, enum BP::DOMAIN_TYPE enDomainType)const
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pServerCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pVerifyInfo, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xml;
	if (!xml.Parse(pVerifyInfo))
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "xml.Parse(pVerifyInfo) fail", "NA");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	IVS_INT32 iRet = IVS_SUCCEED;
	/************************************************************************
    ** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
    **              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
    ************************************************************************/
    
    std::string strLinkID;
    CSDKSecurityClient oSecurityClient;
	std::string strDomainCode = pDomainCode;
	std::string strServerCode;
	// 若是集群则寻找最空闲NVR
	if (0 == strcmp(pServerCode, ""))
	{
		// 构造带域的请求消息，并发送
		CSendNssMsgInfo sendNssMsgInfo;
		sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
		sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
		sendNssMsgInfo.SetReqID(NSS_CLUSTER_FIND_IDLEST_NVR_REQ);
		sendNssMsgInfo.SetReqData("");	
		sendNssMsgInfo.SetDomainCode(strDomainCode.c_str());
		std::string strpRsp;
		IVS_INT32 iNeedRedirect = IVS_FAIL;
		iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "SendCmd fail", "NA");
			return iRet;
		}
		CXml pRsqXml;
		if (!pRsqXml.Parse(strpRsp.c_str()))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.Parse(pSearchInfo) fail", "NA");
			return IVS_XML_INVALID;
		}
		iRet = CDeviceMgrXMLProcess::FindIdlestNvrParseXML(strServerCode, pRsqXml);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "FindIdlestNvrParseXML fail", "NA");
			return iRet;
		}
		const IVS_CHAR* pAddr = "Content/ValidateInfo/ServerCode";
		iRet = CDeviceMgrXMLProcess::ModifyServerCode(xml, strServerCode, pAddr);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "ModifyServerCode fail", "NA");
			return iRet;
		}
	}
	else
	{
		strServerCode = pServerCode;
	}
	// 初始化密钥交换类对象,完成密钥交换
    int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, strDomainCode, strLinkID, "SCU", "", strServerCode.c_str(), enDomainType);
    if (IVS_SUCCEED != iMultiExchangeKey)
    {
        BP_RUN_LOG_ERR(iMultiExchangeKey, "VerifyDevice fail", "oSecurityClient Init failed");
        return iMultiExchangeKey;
    }
	
	if (!xml.FindElemEx("Content/ValidateInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/ValidateInfo) fail", "NA");
		return IVS_XML_INVALID;
	}

	IVS_CHAR cDevPWD[IVS_PWD_LEN + 1] = {0};
	std::string strDevPWDCipher;
	const IVS_CHAR* pTemp = NULL;
	GET_ELEM_VALUE_CHAR("DevPWD", pTemp, cDevPWD, IVS_PWD_LEN, xml);
	if (0 != strcmp(cDevPWD, ""))
	{
		if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevPWD, strDevPWDCipher))
		{
			BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "VerifyDevice fail", "Encrypt String failed");
			return IVS_DATA_ENCRYPT_ERROR;
		}
		if (!xml.FindElem("DevPWD"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(DevPWD) fail", "NA");
			return IVS_XML_INVALID;
		}
		xml.ModifyElemValue(strDevPWDCipher.c_str());
	}
	
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xml.GetXMLStream(uiLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

	//创建要发送的CMD，拼装了NSS消息头
	CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_VALIDATE_DEVICE_REQ, pReq, strSecurityTransID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
	//发送消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);
	
	// 返回解析报文错误码
	*pRspXml = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);

	return iRet;
};


// 查询前端日志
IVS_INT32 CDeviceMgr::GetDevLog(const IVS_CHAR* pQueryXml, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pQueryXml, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	CXml xmlReq;
	IVS_UINT32 xmlLen = 0;
	IVS_INT32 iRet = IVS_SUCCEED;
	if (!xmlReq.Parse(pQueryXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.Parse(pQueryXml) fail", "NA");
		return IVS_XML_INVALID;
	}
	// 根据Subscriber节点值获取域编码
	if (xmlReq.FindElemEx("Content/PageInfo/QueryCond"))
	{
		if (!xmlReq.FindElem("QueryField"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElem(QueryFiel) fail", "NA");
			return IVS_XML_INVALID;
		}

		do 
		{
			(void)xmlReq.IntoElem();
			if (!xmlReq.FindElem("Field"))
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlReq.FindElem(Field) fail", "NA");
				return IVS_XML_INVALID;
			}
			const IVS_CHAR* pDevCode = xmlReq.GetElemValue();

            if (NULL == pDevCode)
            {
                pDevCode = "";
            }
			if (0 == strcmp(pDevCode, "DevCode"))
			{
				if (!xmlReq.FindElem("Value"))
				{
					BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlRsq.FindElem(Value) error", "NA");
					return IVS_XML_INVALID;
				}
				std::string strBigCode;
               
                const IVS_CHAR* pBigCode = xmlReq.GetElemValue();
                if (NULL != pBigCode)
                {
                    strBigCode = pBigCode;
                }

				// 解析DevCode
				iRet = CXmlProcess::ParseDevCode(strBigCode.c_str(), chDevCode, chDomaCode);
				if (IVS_SUCCEED != iRet)
				{
					BP_RUN_LOG_ERR(iRet, "Get DevLog fail", "(ParseDevCode)iRet=%d", iRet);
					return iRet;
				}
				if (!strBigCode.empty())
				{
					xmlReq.ModifyElemValue(chDevCode);
					break;
				}
			}
			xmlReq.OutOfElem();
		} while (xmlReq.NextElem());
	}
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);
    if (NULL == pReq)
    {
        return IVS_XML_INVALID;
    }


	// 构造带域的请求消息，并发送
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_DEV_LOG_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(chDomaCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	
	IVS_CHAR* pRsp = IVS_NEW((IVS_CHAR*&)pRsp, strpRsp.size() + 1);
	CHECK_POINTER(pRsp, IVS_OPERATE_MEMORY_ERROR);
	memset(pRsp, 0x0, strpRsp.size() + 1);
	if (!CToolsHelp::Memcpy(pRsp, strpRsp.size(), strpRsp.c_str(), strpRsp.size()))
	{
		IVS_DELETE(pRsp, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(*pRspXml, strpRsp.size(), strpRsp.c_str(), strpRsp.size()) fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}

    CHECK_POINTER(pRspXml, IVS_OPERATE_MEMORY_ERROR);
	*pRspXml = pRsp;

	return iRet;
}

// 查询当前用户的手动录像任务
IVS_INT32 CDeviceMgr::GetRecordTask(const IVS_UINT32 uiUserId,CDeviceMgr &m_DeviceMgr,IVS_CHAR** pRspXml)
{
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	CXml xmlRsp;
	IVS_UINT32 xmlLen = 0;
	std::string strDomainCode;
	IVS_CHAR chCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	std::string strpRsp;
	std::string strResult = "<Content><CameraList>";
	const IVS_CHAR* szElemValue = NULL;

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_RECORD_TASK_REQ);

	m_vecCameraDomain.clear();
	// 取得用户拥有设备的域列表
	IVS_INT32 iRet = m_DeviceMgr.GetDomainListByUserID(m_DeviceMgr.m_vecCameraDomain);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetDomainList By UserID return failed", "NA");
		return iRet;
	}

	// 查询所有域的摄像机手动录像状态
	IVS_UINT32 iSize = m_DeviceMgr.m_vecCameraDomain.size();	
	CRecordTaskCameraInfo recordTaskCameraInfo;
	IVS_UINT32 uiRecordMethod = 0;
    std::string strUserDomainCode;
	m_pUserMgr->GetDomainCode(strUserDomainCode);
	// 遍历域编码列表
	for(IVS_UINT32 i=0;i<iSize;i++)
	{
		CXml xmlReq;
		strDomainCode = m_DeviceMgr.m_vecCameraDomain.at(i);	
		iRet = CDeviceMgrXMLProcess::GetRecordTaskGetXML(uiUserId,strUserDomainCode.c_str(),strDomainCode.c_str(),xmlReq);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet,"Get Record Task:ReqXML is failed","failed");
			return iRet;
		}
		
		const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);

        if (NULL == pReq)
        {
            return IVS_XML_INVALID;
        }

		sendNssMsgInfo.SetReqData(pReq); 
		sendNssMsgInfo.SetDomainCode(strDomainCode.c_str());
		IVS_INT32 iNeedRedirect = IVS_FAIL;
		// 	sendNssMsgInfo.SetCameraCode(szCameraCode);
		iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

		if(IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Get Record Task From One Domain failed", "NA");
			//return iRet;
		}
		// 若解析xml失败
		if (xmlRsp.Parse(strpRsp.c_str()))
		{
			if(xmlRsp.FindElemEx("Content/CameraList"))
			{
				do 
				{
					xmlRsp.IntoElem();
					(void)xmlRsp.IntoElem();
					GET_ELEM_VALUE_CHAR("CameraCode",szElemValue,chCameraCode,IVS_DEV_CODE_LEN,xmlRsp);
					GET_ELEM_VALUE_NUM_FOR_UINT("RecordMethod",szElemValue,uiRecordMethod,xmlRsp);
					strResult += "<CameraInfo>";
					strResult += "<DomainCode>";
					strResult += strDomainCode;
					strResult += "</DomainCode>";
					strResult += "<CameraCode>";
					strResult += chCameraCode;
					strResult += "</CameraCode>";
					strResult += "<RecordMethod>";
					strResult += CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiRecordMethod));
					strResult += "</RecordMethod>";
					strResult += "</CameraInfo>";

					xmlRsp.OutOfElem();
				} while(xmlRsp.NextElem());
			}
		} 
	}	

	strResult += "</CameraList></Content>";
	(void)IVS_NEW((IVS_CHAR *&)*pRspXml, strResult.size() + 1);
    CHECK_POINTER(*pRspXml, IVS_ALLOC_MEMORY_ERROR);

	memset(*pRspXml, 0, strResult.size() + 1);
	(void)!CToolsHelp::Memcpy(*pRspXml, strResult.size() + 1, strResult.c_str(), strResult.size());
	return iRet;
}

// 查询摄像机码流能力
IVS_INT32 CDeviceMgr::GetDeviceCapability(IVS_UINT32 uiCapabilityType, const IVS_CHAR* pDevCode, IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_CHAR chCameraCode[IVS_SDK_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR chDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pDevCode, chCameraCode, chDomainCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Parse DevCode fail", "NA");
		return iRet;
	}
	CXml xmlReq;
	iRet = CXmlProcess::GetCommConfigGetXML(chCameraCode, chDomainCode, xmlReq, SDKCAMERA);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get DeviceCapability GetXML fail", "NA");
		return iRet;
	}

	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = IVS_FAIL;
	// 拼装发送报文
	CCmd *pCmd = NULL;
	// 拼装发送报文
	switch(uiCapabilityType)
	{
	case DEVICE_ENCODE_CAPABILITY:
        // 获取本域SMU连接
        iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
        if (IVS_SUCCEED != iGetLinkRet)
        {
            BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
            return iGetLinkRet;
        }
		pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_GET_CAMERA_ENCODE_CAPABILITIES_REQ, pReq, strSMULinkID);
		break;
	default:
		{
			BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get DeviceCapability fail", "uiCapabilityType fail");
			iRet = IVS_OPERATE_MEMORY_ERROR;
		}
		return iRet;
	}
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

	// 发送nss协议
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);

	// 返回解析报文错误码
	*pRspXml = CNSSOperator::instance().ParseCmd2XML(pCmdRsp, iRet);
	HW_DELETE(pCmdRsp);

	return iRet;
}

// 获取摄像机预览图接口
IVS_INT32 CDeviceMgr::GetCameraThumbnail(const IVS_CHAR* pCameraCode, IVS_CHAR* pThumbnailURL,IVS_UINT32 uiBufferSize)const
{
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
    CHECK_POINTER(pThumbnailURL, IVS_PARA_INVALID);
	
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	if (uiBufferSize>256)
	{
		return IVS_PARA_INVALID;
	}

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Camera Thumbnail fail", "(ParseDevCode)iRet=%d", iRet);
		return iRet;
	}

	// 发送消息
	// 拼装发送请求XML
	CXml xmlReq;
	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");	
	(void)xmlReq.AddChildElem("ThumbnailList");
	(void)xmlReq.IntoElem();
	(void)xmlReq.AddChildElem("ThumbnailInfo");
	(void)xmlReq.IntoElem();
	(void)xmlReq.AddChildElem("DevCode");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(chDevCode);

	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
    if (NULL == pReq)
    {
        return IVS_XML_INVALID;
    }

	BP_RUN_LOG_INF("Get Camera Thumbnail", "!Rspxml.FindElem(DevInfo)%s",pReq);

	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_CAMERA_THUMBNAIL_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(chDomaCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED == iRet)
	{
		CXml xmlRsp;
		(void)xmlRsp.Parse(strpRsp.c_str());
		const IVS_CHAR* szElemValue = NULL;
		if (!xmlRsp.FindElemEx("Content/ThumbnailList/ThumbnailInfo/ThumbnailURL"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Camera Thumbnail", "!Rspxml.FindElem(DevInfo)");
			return IVS_XML_INVALID;
		}
#ifdef WIN32
		GET_ELEM_VALUE_CHAR("ThumbnailURL", szElemValue, pThumbnailURL, MAX_PATH, xmlRsp);
#else
		GET_ELEM_VALUE_CHAR("ThumbnailURL", szElemValue, pThumbnailURL, PATH_MAX, xmlRsp);
#endif
	}

	return iRet;
}

// 设置前端设备密码
IVS_INT32 CDeviceMgr::SetDevPWD(const IVS_CHAR* pDevCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD)
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevUserName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevRegPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SetDevPWD fail", "(ParseDevCode)iRet=%d", iRet);
		return iRet;
	}

	// 判断是集群或堆叠
	IVS_DOMAIN_ROUTE stDomainRoute;
    memset(&stDomainRoute, 0, sizeof(stDomainRoute));
	std::string strDomainCode = chDomaCode;
	CDomainRouteMgr& oDomainRoute = m_pUserMgr->GetDomainRouteMgr();
	iRet = oDomainRoute.GetDomainRoutebyCode(strDomainCode, stDomainRoute);
	// 集群情况
	if (2 == stDomainRoute.uiDomainType)
	{
		
		iRet = SetDevPWDCluster(chDevCode, chDomaCode, pDevUserName, pDevPWD, pDevRegPWD);

		return iRet;
	}
	// 堆叠模式、单机模式
	iRet = SetDevPWDPile(chDevCode, chDomaCode, pDevUserName, pDevPWD, pDevRegPWD);

	return iRet;
}

// 堆叠下设置前端设备密码
IVS_INT32 CDeviceMgr::SetDevPWDPile(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD)const
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevUserName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevRegPWD, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	std::string strDomainCode = pDomainCode;
	 /************************************************************************
    ** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
    **              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
    ************************************************************************/
    // 初始化密钥交换类对象,完成密钥交换
    std::string strLinkID;
	CSDKSecurityClient oSecurityClient;
    int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, strDomainCode, strLinkID, "SCU", pDevCode);
    if (IVS_SUCCEED != iMultiExchangeKey)
    {
        BP_RUN_LOG_ERR(iMultiExchangeKey, "SetDevPWD fail", "oSecurityClient Init failed");
        return iMultiExchangeKey;
    }
    // 加密数据后拼装
    std::string strDevPWD;
    char cPWDTemp[IVS_PWD_LEN + 1] = {0};
	if (0 != strcmp(pDevPWD, ""))
	{
		if (!CToolsHelp::Memcpy(cPWDTemp, IVS_PWD_LEN, pDevPWD, IVS_PWD_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "SetDevPWD fail", "Memcpy failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (IVS_SUCCEED != oSecurityClient.EncryptString(cPWDTemp, strDevPWD))
		{
			BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "Set DevPWD", "Encrypt String failed");
			return IVS_DATA_ENCRYPT_ERROR;
		}
	}

	std::string strDevRegPWD;
	char cDevRegPWDTemp[IVS_PWD_LEN + 1] = {0};
	if (0 != strcmp(pDevRegPWD, ""))
	{
		if (!CToolsHelp::Memcpy(cDevRegPWDTemp, IVS_PWD_LEN, pDevRegPWD, IVS_PWD_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "SetDevPWD fail", "Memcpy failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevRegPWDTemp, strDevRegPWD))
		{
			BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "Set DevPWD", "Encrypt String failed");
			return IVS_DATA_ENCRYPT_ERROR;
		}
	}

	CXml xmlReq;
	IVS_INT32 iRet = CDeviceMgrXMLProcess::SetDevPWDGetXML(pDomainCode, pDevCode, pDevUserName, strDevPWD, strDevRegPWD, xmlReq); 
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set DevPWD GetXML fail", "NA");
		return iRet;
	}
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

	//创建要发送的CMD，拼装了NSS消息头
	CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, NSS_SET_DEV_PWD_REQ, pReq, strSecurityTransID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
	//发送消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);

	iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);

	return iRet;

}

// 集群下设置前端设备密码
IVS_INT32 CDeviceMgr::SetDevPWDCluster(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD)const
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevUserName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevRegPWD, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 第一次目标域 SCU
	IVS_INT32 iRet = SetDevPWDClusterMsg(pDevCode, pDomainCode, pDevUserName, pDevPWD, pDevRegPWD, NSS_SET_DEV_PWD_REQ, "SCU", pDevCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Send SCU fail", "NA");
		return iRet;
	}
	// 第二次目标域 CMU
	iRet = SetDevPWDClusterMsg(pDevCode, pDomainCode, pDevUserName, pDevPWD, pDevRegPWD, NSS_CLUSTER_SET_DEV_PWD_REQ, "CMU");
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Send CMU fail", "NA");
		iRet = IVS_SDK_SET_CLUSTER_MSG_FAILED;
	}

	return iRet;
}

IVS_INT32 CDeviceMgr::SetDevPWDClusterMsg(const IVS_CHAR* pDevCode, const IVS_CHAR* pDomainCode, const IVS_CHAR* pDevUserName, const IVS_CHAR* pDevPWD, const IVS_CHAR* pDevRegPWD, NSS_MSG_TYPE_E msgType, const char* pszDestModule, const char * pszDeviceCode)const
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevUserName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevRegPWD, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pszDestModule, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	std::string strDomainCode = pDomainCode;
	 /************************************************************************
    ** 密钥交换过程：初始化->生成事务ID->请求公钥->密钥交换(生成的事务ID)->
    **              加密数据->使用密钥交换生成的事务ID构造Cmd->发送消息                                                                   
    ************************************************************************/

    // 初始化密钥交换类对象,完成密钥交换
    std::string strLinkID;
	CSDKSecurityClient oSecurityClient;
    int iMultiExchangeKey = oSecurityClient.MultiDomainExchangeKey(NET_ELE_SMU_NSS, m_pUserMgr, strDomainCode, strLinkID, pszDestModule, pszDeviceCode, "", BP::DOMAIN_CLUSTER);
    if (IVS_SUCCEED != iMultiExchangeKey)
    {
        BP_RUN_LOG_ERR(iMultiExchangeKey, "SetDevPWD fail", "oSecurityClient Init failed");
        return iMultiExchangeKey;
    }
    // 加密数据后拼装
    std::string strDevPWD;
    char cPWDTemp[IVS_PWD_LEN + 1] = {0};
	if (0 != strcmp(pDevPWD, ""))
	{
		if (!CToolsHelp::Memcpy(cPWDTemp, IVS_PWD_LEN, pDevPWD, IVS_PWD_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "SetDevPWD fail", "Memcpy failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (IVS_SUCCEED != oSecurityClient.EncryptString(cPWDTemp, strDevPWD))
		{
			BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "Set DevPWD", "Encrypt String failed");
			return IVS_DATA_ENCRYPT_ERROR;
		}
	}

	std::string strDevRegPWD;
	char cDevRegPWDTemp[IVS_PWD_LEN + 1] = {0};
	if (0 != strcmp(pDevRegPWD, ""))
	{
		if (!CToolsHelp::Memcpy(cDevRegPWDTemp, IVS_PWD_LEN, pDevRegPWD, IVS_PWD_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "SetDevPWD fail", "Memcpy failed");
			return IVS_ALLOC_MEMORY_ERROR;
		}
		if (IVS_SUCCEED != oSecurityClient.EncryptString(cDevRegPWDTemp, strDevRegPWD))
		{
			BP_RUN_LOG_ERR(IVS_DATA_ENCRYPT_ERROR, "Set DevPWD", "Encrypt String failed");
			return IVS_DATA_ENCRYPT_ERROR;
		}
	}

	CXml xmlReq;
	IVS_INT32 iRet = CDeviceMgrXMLProcess::SetDevPWDGetXML(pDomainCode, pDevCode, pDevUserName, strDevPWD, strDevRegPWD, xmlReq); 
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set DevPWD GetXML fail", "NA");
		return iRet;
	}
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
	BP_RUN_LOG_INF("SetDevPWDClusterMsg", "send XML = %s", pReq);
	std::string strSecurityTransID  = oSecurityClient.GetSecurityDataTransID();

	//创建要发送的CMD，拼装了NSS消息头
	CCmd *pCmd = CNSSOperator::instance().BuildCmd(strLinkID, NET_ELE_SMU_NSS, msgType, pReq, strSecurityTransID);
	CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);
	//发送消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);

	iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);
	return iRet;
}

// 导入前端配置文件
IVS_INT32 CDeviceMgr::ImportDevCfgFile(const IVS_CHAR* pDevCode, const IVS_CHAR* pCFGFile)
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCFGFile, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Import DevCfgFile fail", "(ParseDevCode)iRet=%d", iRet);
		return iRet;
	}

	// 文件路径，包含文件名
	std::string strCFGFile = pCFGFile;

	//读取文件
	IVS_UINT32 ifLength = 0;
	ifstream ifCFGFile;
	ifCFGFile.open(strCFGFile.c_str(), ios::binary);
	if(!ifCFGFile)      
	{        
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "file open failed", "NA");
		return IVS_OPEN_FILE_ERROR;
	} 
	// 校验文件大小，不能超过最大
	ifCFGFile.seekg(0, ios::end); //lint !e747
	ifLength = (IVS_UINT32)ifCFGFile.tellg();
	ifCFGFile.seekg(0, ios::beg); //lint !e747
	if (ifLength > CFG_FILE_MAX_LEN)
	{
		BP_RUN_LOG_ERR(IVS_WRITE_FILE_ERROR, "upload logo failed", "file is oversize");
		return IVS_WRITE_FILE_ERROR;
	}
	IVS_CHAR *buffer = IVS_NEW(buffer,ifLength + 1);
	CHECK_POINTER(buffer, IVS_ALLOC_MEMORY_ERROR);
	memset(buffer, 0, ifLength + 1);
	ifCFGFile.read(buffer, ifLength); //lint !e747
	ifCFGFile.close();

	// 拼装文件的TLV
	TNssTLVMsg tlvMsg;
	tlvMsg.usiTag = 0x0004; // 已二进制方式读取
	tlvMsg.uiLength = (IVS_UINT32)ifLength;
	tlvMsg.pszValue = buffer;

	//将文件TLV转换成字符串
	IVS_USHORT usiTag = htons(tlvMsg.usiTag);
	IVS_UINT32 uiLength = htonl(tlvMsg.uiLength);

	IVS_UINT32 uiLen = 0;
	IVS_UINT32 uiMsgBuffer = IVS_SDK_DEV_CODE_LEN + IVS_DOMAIN_CODE_LEN + ifLength  + sizeof(IVS_USHORT) + sizeof(IVS_UINT32) + CFG_FILE_MAX_LEN;
	IVS_CHAR* pMsgBuffer = IVS_NEW(pMsgBuffer, uiMsgBuffer);
	if (NULL == pMsgBuffer)
	{
		IVS_DELETE(buffer, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "UploadLogo", "alloc memory failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(pMsgBuffer, 0, uiMsgBuffer);

	// 域编码
	if (!CToolsHelp::Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, chDomaCode, IVS_DOMAIN_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, chDomaCode, IVS_SDK_DEV_CODE_LEN+1) fail", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	uiLen += IVS_DOMAIN_CODE_LEN;

	// 设备编码
	if (!CToolsHelp::Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, chDevCode, IVS_SDK_DEV_CODE_LEN))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, chDevCode, IVS_DOMAIN_CODE_LEN+1)fail", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	uiLen += IVS_SDK_DEV_CODE_LEN;

	// tag
	if (!CToolsHelp::Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, &usiTag, sizeof(IVS_USHORT)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, usiTag, sizeof(IVS_USHORT)) fail", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	uiLen += sizeof(IVS_USHORT);

	// length
	if (!CToolsHelp::Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, &uiLength, sizeof(IVS_UINT32)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, uiLength, sizeof(IVS_UINT32)) fail", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	uiLen += sizeof(IVS_UINT32);

	// value
	if (!CToolsHelp::Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, buffer, tlvMsg.uiLength))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pMsgBuffer + uiLen, uiMsgBuffer - uiLen, buffer, tlvMsg.uiLength) fail", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	IVS_DELETE(buffer, MUILI);

	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_IMPORT_DEV_CFG_FILE_REQ);
	sendNssMsgInfo.SetReqDataPointer(pMsgBuffer);
	sendNssMsgInfo.SetReqLength(uiMsgBuffer);
	sendNssMsgInfo.SetDomainCode(chDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp, iNeedRedirect);
	IVS_DELETE(pMsgBuffer, MUILI);

	return iRet;
}

// 导出前端配置文件
IVS_INT32 CDeviceMgr::ExportDevCfgFile(const IVS_CHAR* pDevCode, const IVS_CHAR* pCFGFilePath)
{
	CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCFGFilePath, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Export DevCfgFile fail", "(ParseDevCode)iRet=%d", iRet);
		return iRet;
	}

	// 拼装请求XML报文
	CXml xmlReq;
	iRet = CXmlProcess::GetCommConfigGetXML(chDomaCode, chDevCode, xmlReq, SDKDEVICE);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ExportDevCfg GetXML fail", "NA");
		return iRet;
	}
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
    if (NULL == pReq)
    {
        return IVS_XML_INVALID;
    }
	// 发送报文
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_EXPORT_DEV_CFG_FILE_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(chDomaCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp, iNeedRedirect);

	unsigned int ulBuffSize = strpRsp.length();
	if (ulBuffSize < sizeof(uint16_t) + sizeof(uint32_t))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "length error", "NA");
		return IVS_XML_INVALID;
	}

	IVS_CHAR* pRsq = IVS_NEW((IVS_CHAR*&)pRsq, ulBuffSize);
	CHECK_POINTER(pRsq, IVS_OPERATE_MEMORY_ERROR);
	memset(pRsq, 0x0, ulBuffSize);
	if (!CToolsHelp::Memcpy(pRsq, ulBuffSize, strpRsp.c_str(), ulBuffSize))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(pRsq, strpRsp.size(), strpRsp.c_str(), strpRsp.size()) fail", "NA");
		IVS_DELETE(pRsq, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}



	// 处理TLV字段(配置文件(二进制)的处理
	TNssTLVMsg tlvMsg;
	CNSSOperator::instance().ParseData2TLV((void*)pRsq, tlvMsg);//访问越界

	// 判断tag
	if (tlvMsg.usiTag != 0x0004)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "tag error", "NA");
		IVS_DELETE(pRsq, MUILI);
		return IVS_XML_INVALID;
	}

	// 判断二进制文件大小
	if (tlvMsg.uiLength > CFG_FILE_MAX_LEN)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "length error", "NA");
		IVS_DELETE(pRsq, MUILI);
		return IVS_XML_INVALID;
	}

	// 存储路径
	std::string strAddress;
	strAddress.append(pCFGFilePath);
	strAddress.append(chDevCode);
	strAddress.append(".cfg");

	//读取文件
	ofstream ofCFGFile;
	ofCFGFile.open(strAddress.c_str(), ios::binary);
	if (!ofCFGFile)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "ofCFGFile.open fail", "NA");
		IVS_DELETE(pRsq, MUILI);
		return IVS_OPERATE_MEMORY_ERROR;
	}

	ofCFGFile.write (tlvMsg.pszValue, tlvMsg.uiLength);//lint !e747
	ofCFGFile.close();
	IVS_DELETE(pRsq, MUILI);

	return iRet;
}

//设备状态查询
IVS_INT32 CDeviceMgr::ExGetDeviceStatus(const IVS_CHAR* pReqXml,IVS_CHAR** pRspXml)const
{
	CHECK_POINTER(pReqXml,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pRspXml,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	CXml xmlReq;	
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (xmlReq.FindElemEx("Content"))
	{
		xmlReq.IntoElem();
		if (!xmlReq.FindElem("DevCode"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(DevCode)", "NA");
			return IVS_XML_INVALID;
		}
		const IVS_CHAR* pElemValue = xmlReq.GetElemValue();
		iRet = CXmlProcess::ParseDevCode(pElemValue, szCameraCode, szDomainCode);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "ExDomain Get Device Status Req XML DevCode Value", "Parse DevCode failed");
			return iRet;
		}
		xmlReq.ModifyElemValue(szCameraCode);
		xmlReq.OutOfElem();
	}

	// 添加外域编码
	xmlReq.GetRootPos();
	if (!xmlReq.MkAndSetElemValueEx("Content/DomainCode", szDomainCode))
	{
		BP_RUN_LOG_ERR( IVS_OPERATE_MEMORY_ERROR ,"ExDomain Get Device Status:Set DomainCode Value", "add Camera DomainCode failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR *pReqSpace = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqSpace, IVS_OPERATE_MEMORY_ERROR);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_DEVICE_STATUS_REQ);
	sendNssMsgInfo.SetReqData(pReqSpace); 
	sendNssMsgInfo.SetDomainCode(szDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	CXml xmlRsp;
	(void)xmlRsp.Parse(strpRsp.c_str());
	if (xmlRsp.FindElemEx("Content/ShareDevInfo"))
	{

		(void)xmlRsp.IntoElem();
		const char* szElemValue = NULL;
		char cCameraCode[IVS_DEV_CODE_LEN+1]={0};
		if (!xmlRsp.FindElem("LocalDevCode"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xmlRsp.FindElem(LocalDevCode)", "NA");
			return IVS_XML_INVALID;
		}
		GET_ELEM_VALUE_CHAR("LocalDevCode",szElemValue,cCameraCode,IVS_DEV_CODE_LEN,xmlRsp);
		std::string strBigCode;
		strBigCode.append(cCameraCode).append("#").append(szDomainCode);
		xmlRsp.ModifyElemValue(strBigCode.c_str());			
		xmlRsp.OutOfElem();
	}
	IVS_UINT32 uiXmlLen = 0;
	
    const IVS_CHAR* pXmlRsp = xmlRsp.GetXMLStream(uiXmlLen);
    if (NULL != pXmlRsp)
    {
        strpRsp = pXmlRsp;
    }

	(void)IVS_NEW((IVS_CHAR *&)*pRspXml, strpRsp.size() + 1);
	if (NULL == *pRspXml)
	{
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(*pRspXml, 0, strpRsp.size() + 1);
	if (!CToolsHelp::Memcpy(*pRspXml, strpRsp.size() + 1, strpRsp.c_str(), strpRsp.size()))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "ExDomain Get Device Status", "Memcpy failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	return iRet;
}

//设备信息查询
IVS_INT32 CDeviceMgr::ExGetDeviceInfo(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(pReqXml,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	IVS_INT32 iRet = IVS_FAIL;
	CXml xmlReq;	
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,  "xmlReq.Parse(pReqXml) fail", "NA");
		return IVS_XML_INVALID;
	}

	IVS_CHAR szCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_CHAR szDomainCode[IVS_DOMAIN_CODE_LEN + 1] = {0};
	if (xmlReq.FindElemEx("Content"))
	{
		xmlReq.IntoElem();
		if (!xmlReq.FindElem("DevCode"))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(DevCode)", "NA");
			return IVS_XML_INVALID;
		}
		const IVS_CHAR* pElemValue = xmlReq.GetElemValue();
		iRet = CXmlProcess::ParseDevCode(pElemValue, szCameraCode, szDomainCode);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "ExDomain Get Device Info Req XML DevCode Value", "Parse DevCode failed");
			return iRet;
		}
		xmlReq.ModifyElemValue(szCameraCode);
		xmlReq.OutOfElem();
	}

	// 添加外域编码
	xmlReq.GetRootPos();
	if (!xmlReq.MkAndSetElemValueEx("Content/DomainCode", szDomainCode))
	{
		BP_RUN_LOG_ERR( IVS_OPERATE_MEMORY_ERROR ,"ExDomain Get Device Info:Set DomainCode Value", "add Camera DomainCode failed");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	unsigned int xmlLen = 0;
	const IVS_CHAR *pReqSpace = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqSpace, IVS_OPERATE_MEMORY_ERROR);

	//发送nss协议
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_DEVICE_INFO_REQ);
	sendNssMsgInfo.SetReqData(pReqSpace); 
	sendNssMsgInfo.SetDomainCode(szDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	return iRet;
}

// 刷新摄像机列表，同步下级域摄像机列表
IVS_INT32 CDeviceMgr::RefreshCameraList(IVS_INT32 iSessionID)
{
    IVS_DEBUG_TRACE("");

    CXml xmlReq;	
    IVS_INT32 iRet = CDeviceMgrXMLProcess::RefreshCameraListXML(iSessionID, xmlReq);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "IVS_SUCCEED != Get Camera List XML", "NA");
        return iRet;
    }

    IVS_UINT32 xmlLen = 0;
    const IVS_CHAR* pReq = xmlReq.GetXMLStream(xmlLen);	
    CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

    BP_RUN_LOG_INF("RefreshCameraList", "send XML = %s", pReq);

    // 构造请求消息，并向SMU发送
    CSendNssMsgInfo sendNssMsgInfo;
    sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
    sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
    sendNssMsgInfo.SetReqID(NSS_REFRESH_CAMERA_LIST_REQ);
    sendNssMsgInfo.SetReqData(pReq);	
    std::string strpRsp;
    IVS_INT32 iNeedRedirect = IVS_FAIL;
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    iRet = m_pUserMgr->SendCmd(sendNssMsgInfo, strpRsp, iNeedRedirect, 120, 1);

    BP_RUN_LOG_INF("RefreshCameraList", "revice XML = %s", strpRsp.c_str());

    return iRet;
}

//Onvif设备从前端获取能力参数范围
IVS_INT32 CDeviceMgr::GetDeviceEncodeCapability(const IVS_CHAR* pDevCode,
	IVS_STREAM_INFO_LIST* pStreamInfoList)const
{
	CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = IVS_FAIL;
	iRet = CXmlProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Camera Thumbnail fail", "(ParseDevCode)iRet=%d", iRet);
		return iRet;
	}

	// 发送消息
	// 拼装发送请求XML
	CXml xmlReq;
	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");	
	(void)xmlReq.AddChildElem("DomainCode");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(chDomaCode);
	(void)xmlReq.AddElem("VideoInChannelInfo");
	(void)xmlReq.AddChildElem("CameraCode");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(chDevCode);
	xmlReq.OutOfElem();
	xmlReq.OutOfElem();

	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
	if (NULL == pReq)
	{
		return IVS_XML_INVALID;
	}

	BP_RUN_LOG_INF("Get Camera Encode Capabilities", "!Rspxml.FindElem(DevInfo)%s",pReq);

	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_CAMERA_ENCODE_CAPABILITIES_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(chDomaCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;

	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);

	if (IVS_SUCCEED == iRet)
	{
		CXml xmlRsp;

		//解析查询返回的前端编码参数信息
		(void)xmlRsp.Parse(strpRsp.c_str());
		iRet = CDeviceMgrXMLProcess::ParseCameraEncodeCapabilities(xmlRsp,pStreamInfoList);
	}

	return iRet;
}


//视频质量诊断计划查询
IVS_INT32 CDeviceMgr::GetVideoDiagnose(const IVS_CHAR* pDomainCode,
	const IVS_CHAR* pCameraCode,
	IVS_CHAR** pRspXml)const
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pRspXml, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);

	std::string strDomainCode;
	if (NULL == pDomainCode || '\0' == pDomainCode[0])
	{
		//为空则默认本域
		m_pUserMgr->GetDomainCode(strDomainCode);
	}
	else
	{
		strDomainCode = pDomainCode;
	}

	// 解析DevCode
	IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	IVS_INT32 iRet = CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetVideoDiagnose", "ParseDevCode Failed");
		return iRet;
	}

	//获取请求xml流
	CXml xmlReq;
	iRet = CDeviceMgrXMLProcess::GetVideoDiagnoseGetXML(chDomaCode, chDevCode, xmlReq);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetVideoDiagnose", "GetVideoDiagnoseGetXML failed");
		return iRet;
	}
	IVS_UINT32 uiLen = 0;
	const IVS_CHAR* pReq = xmlReq.GetXMLStream(uiLen);
	if (NULL == pReq)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "GetVideoDiagnose", "xmlReq.GetXMLStream failed");
		return IVS_XML_INVALID;
	}

	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_QD_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReq);	
	sendNssMsgInfo.SetDomainCode(strDomainCode);
	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;

	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetVideoDiagnose", "SendCmd failed");
		return iRet;
	}
	IVS_CHAR* pRsp = IVS_NEW((IVS_CHAR*&)pRsp, strpRsp.size() + 1);
	CHECK_POINTER(pRsp, IVS_OPERATE_MEMORY_ERROR);
	memset(pRsp, 0x0, strpRsp.size() + 1);
	if (!CToolsHelp::Memcpy(pRsp, strpRsp.size(), strpRsp.c_str(), strpRsp.size()))
	{
		IVS_DELETE(pRsp, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "GetVideoDiagnose", "Memcpy fail");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	*pRspXml = pRsp;
	return iRet;
}


//视频质量诊断计划配置
IVS_INT32 CDeviceMgr::SetVideoDiagnose(const IVS_CHAR* pReqXml)const
{
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("SetVideoDiagnose, pReqXml: %s", pReqXml);

	//获取目标域编码，如果请求xml中不为空就使用请求xml中的目标域编码，否则使用本地域编码。
	CXml xmlReq;
	std::string strDstDomainCode;
	IVS_INT32 iRet = CAlarmMgrXMLProcess::GetDstDomainCode(xmlReq, pReqXml, strDstDomainCode);
	if (strDstDomainCode.empty())
	{
		m_pUserMgr->GetDomainCode(strDstDomainCode);
	}

	// 构造发送结构体
	CSendNssMsgInfo sendNssMsgInfo;	
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_NOXML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_SET_QD_PLAN_REQ);
	sendNssMsgInfo.SetReqData(pReqXml);	
	sendNssMsgInfo.SetDomainCode(strDstDomainCode);

	std::string strpRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strpRsp,iNeedRedirect);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "SetVideoDiagnose", "SendCmd failed");
	}

	return iRet;
}