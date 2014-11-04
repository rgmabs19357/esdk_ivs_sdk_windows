#include "SDKDecoderMgr.h"
#include <time.h>
#include "ivs_error.h"
#include "bp_log.h"
#include "bp_def.h"
#include "ToolsHelp.h"
#include "IVSCommon.h"

/************************************************************************/
/* 2013-2-19 收到SE 李子刚 通知，SDK解码器在C02中不实现，故将HWPuSDK.h去*/
/* 掉，相关的函数和结构体引用都屏蔽掉，当前 CSDKDecoderMgr 无法使用     */
/************************************************************************/

SDK_DECODER_MGR_LIST CSDKDecoderMgr::m_DecMgrObjList;

CSDKDecoderMgr::CSDKDecoderMgr(void) : m_pCallBackFun(NULL)
									 , m_pUserData(NULL)
									 , m_pVosThread(NULL)
									 , m_bThreadExit(FALSE)
									 , m_lLastLoginTime(0)
									 , m_bInit(FALSE)
{
	m_pDecoderInfoMapMutex = VOS_CreateMutex();
	CSDKDecoderMgr::AddDecMgrObj(this);
}

CSDKDecoderMgr::~CSDKDecoderMgr(void)
{
	m_bThreadExit = TRUE;
	m_pCallBackFun = NULL;
	m_pUserData = NULL;

	
	VOS_ThreadJoin(m_pVosThread);
	try
	{
		CSDKDecoderMgr::RemoveDecMgrObj(this);
		RemoveDecoderInfo();
	}
	catch (...)
	{
	}	
	VOS_DestroyMutex(m_pDecoderInfoMapMutex);
	m_pDecoderInfoMapMutex = NULL;
	if(NULL!=m_pVosThread)
	{
		VOS_free(m_pVosThread);
		m_pVosThread = NULL;
	}
}

IVS_INT32 CSDKDecoderMgr::InitSDK(PCUMW_CALLBACK_NOTIFY pFunNotifyCallBack, void* pUserData)
{
	if(m_bInit)
	{
		return IVS_TVWALL_INIT_ALREADY;
	}
	IVS_INT32 iRet = IVS_SUCCEED;
	CHECK_POINTER(pFunNotifyCallBack, IVS_TVWALL_MEMORY_NULL);
	CHECK_POINTER(pUserData, IVS_TVWALL_MEMORY_NULL);

	//////////////////////////////////////////////////////////////////////////
	//2013-1-28 高书明 先屏蔽HWPuSDK相关函数，与BP的dll冲突
	//初始化解码器SDK
	//char ip[10] = {0};
	//BOOL bInitSDK = IVS_PU_Init(LINK_MODE_MANUAL, ip, 5060);
	////设置回调函数
	//bInitSDK = IVS_PU_EventStatesCallBack((pfGetEventInfoCallBack)SDKEventCallBack) | bInitSDK;
	//if(!bInitSDK)
	//{
	//	iRet = IVS_FAIL;
	//}
	//////////////////////////////////////////////////////////////////////////
	//开启线程
	errno = 0;
	if (VOS_OK != VOS_CreateThread((VOS_THREAD_FUNC)invoke, (void *)this, 
		&m_pVosThread, VOS_DEFAULT_STACK_SIZE))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Create SDK Decoder Mgr thread ", "failed. error(%d):%s", errno, strerror(errno));
		return IVS_FAIL;
	}
	m_pCallBackFun = pFunNotifyCallBack;
	m_pUserData = pUserData;
	m_bThreadExit = FALSE;
	m_bInit = TRUE;
	return iRet;
}//lint !e818

//// 设置解码器信息
//IVS_INT32 CSDKDecoderMgr::SetDecoderInfo(const SDK_DECODER_INFO* pDecoderInfo, IVS_UINT32 uiDecoderNum)
//{
//	IVS_INT32 iRet = IVS_SUCCEED;
//	CHECK_POINTER(pDecoderInfo, IVS_TVWALL_MEMORY_NULL);
//	if(0==uiDecoderNum)
//	{
//		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Set Decoder Info", "Failed, Decoder Num=0");
//		return IVS_PARA_INVALID;
//	}
//	VOS_MutexLock(m_pDecoderInfoMapMutex);
//	for(IVS_UINT32 i=0;i<uiDecoderNum;i++)
//	{
//		SDK_DECODER_INFO_MAP::iterator ite = m_decoderInfoMap.find(pDecoderInfo[i].szDecoderID);
//		if(m_decoderInfoMap.end()!=ite)
//		{
//			SDK_DECODER_INFO* pInfo = dynamic_cast<SDK_DECODER_INFO*>(ite->second);//lint !e611
//			if(NULL!=pInfo)
//			{
//				if((0!=strncmp(pDecoderInfo[i].szDecoderID, pInfo->szDecoderID, IVS_TVWALL_DECODER_CODE_LEN)) ||
//					(0!=strncmp(pDecoderInfo[i].szDecoderIP, pInfo->szDecoderIP, IVS_IP_LEN)) ||
//					(0!=strncmp(pDecoderInfo[i].szUserName, pInfo->szUserName, IVS_NAME_LEN)) ||
//					(0!=strncmp(pDecoderInfo[i].szPWD, pInfo->szPWD, IVS_PWD_LEN)) ||
//					pDecoderInfo[i].usDecoderPort!=pInfo->usDecoderPort)
//				{
//					//////////////////////////////////////////////////////////////////////////
//					//2013-1-28 高书明 先屏蔽HWPuSDK相关函数，与BP的dll冲突
//					//(void)IVS_PU_Logout(pInfo->ulIdentifyID);
//					//////////////////////////////////////////////////////////////////////////
//					pInfo->bOnline = FALSE;
//					pInfo->ulIdentifyID = 0;
//				}
//			}
//			else
//			{
//				m_decoderInfoMap.erase(ite);
//			}
//		}
//		else
//		{
//			SDK_DECODER_INFO* pInfo = IVS_NEW(pInfo);
//			memset(pInfo, 0, sizeof(SDK_DECODER_INFO));
//			if(!CToolsHelp::Memcpy(pInfo, sizeof(SDK_DECODER_INFO), &pDecoderInfo[i], sizeof(SDK_DECODER_INFO)))
//			{
//				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Set Decoder Info", "Memcpy Decoder Info Failed");
//				iRet = IVS_ALLOC_MEMORY_ERROR;
//				break;
//			}
//			m_decoderInfoMap.insert(SDK_DECODER_INFO_MAP::value_type(pInfo->szDecoderID, pInfo));
//		}
//	}
//	//清除没有的Decoder
//	SDK_DECODER_INFO_MAP::iterator ite = m_decoderInfoMap.begin();
//	bool bFind = false;
//	while(m_decoderInfoMap.end()!=ite)
//	{
//		bFind = false;
//		SDK_DECODER_INFO* pInfo = dynamic_cast<SDK_DECODER_INFO*>(ite->second);//lint !e611
//		if(NULL!=pInfo)
//		{
//			for(IVS_UINT32 i=0;i<uiDecoderNum;i++)
//			{
//				if(0==strncmp(pDecoderInfo[i].szDecoderID, pInfo->szDecoderID, IVS_TVWALL_DECODER_CODE_LEN))
//				{
//					bFind = true;
//					break;
//				}
//			}
//			if(!bFind)
//			{
//				//////////////////////////////////////////////////////////////////////////
//				//2013-1-28 高书明 先屏蔽HWPuSDK相关函数，与BP的dll冲突
//				//(void)IVS_PU_Logout(pInfo->ulIdentifyID);
//				//////////////////////////////////////////////////////////////////////////
//				ite = m_decoderInfoMap.erase(ite);
//				IVS_DELETE(pInfo);
//				continue;
//			}
//		}
//		ite++;
//	}
//	VOS_MutexUnlock(m_pDecoderInfoMapMutex);
//	if(IVS_SUCCEED!=iRet)
//	{
//		RemoveDecoderInfo();
//	}
//	
//	//立刻重新登录
//	m_lLastLoginTime = 0;
//	return iRet;
//}

IVS_INT32 CSDKDecoderMgr::CleanupSDK()
{
	if(!m_bInit)
	{
		return IVS_TVWALL_INIT_NO;
	}
	m_bThreadExit = TRUE;
	m_bInit = FALSE;
	VOS_ThreadJoin(m_pVosThread);
	if(NULL!=m_pVosThread)
	{
		VOS_free(m_pVosThread);
		m_pVosThread = NULL;
	}
	//清除登录信息
	(void)VOS_MutexLock(m_pDecoderInfoMapMutex);
	for(SDK_DECODER_INFO_MAP::iterator ite=m_decoderInfoMap.begin();ite!=m_decoderInfoMap.end();ite++)
	{
		SDK_DECODER_INFO* pInfo = dynamic_cast<SDK_DECODER_INFO*>(ite->second);//lint !e611
		if(NULL!=pInfo)
		{
			//////////////////////////////////////////////////////////////////////////
			//2013-1-28 高书明 先屏蔽HWPuSDK相关函数，与BP的dll冲突
			//(void)IVS_PU_Logout(pInfo->ulIdentifyID);
			//////////////////////////////////////////////////////////////////////////
			pInfo->bOnline = FALSE;
			pInfo->ulIdentifyID = 0;
		}
	}
	(void)VOS_MutexUnlock(m_pDecoderInfoMapMutex);
	m_pCallBackFun = NULL;
	m_pUserData = NULL;
	//////////////////////////////////////////////////////////////////////////
	//2013-1-28 高书明 先屏蔽HWPuSDK相关函数，与BP的dll冲突
	//BOOL bSucc = IVS_PU_Cleanup();
	//return bSucc ? IVS_SUCCEED:IVS_FAIL;
	return IVS_FAIL;
	//////////////////////////////////////////////////////////////////////////
}

void CSDKDecoderMgr::invoke(void *argc)
{
	CSDKDecoderMgr* pDecoderMgr = (CSDKDecoderMgr*)argc;
	CHECK_POINTER_VOID(pDecoderMgr);
	while(!pDecoderMgr->m_bThreadExit)
	{
		time_t lNow = time(NULL);
		if(pDecoderMgr->m_lLastLoginTime<(lNow-60))
		{//60一次
			pDecoderMgr->m_lLastLoginTime = lNow;
			pDecoderMgr->LoginToDecoder();
		}
		Sleep(500);
	}
	pDecoderMgr->m_lLastLoginTime = 0;
	VOS_pthread_exit(NULL);
}

IVS_LONG __stdcall CSDKDecoderMgr::SDKEventCallBack(void * /*argc*/)
{
	//////////////////////////////////////////////////////////////////////////
	//2013-1-28 高书明 先屏蔽HWPuSDK相关函数，与BP的dll冲突
	//PU_EVENT_NOTIFY_BODY_U* pstEvent = (PU_EVENT_NOTIFY_BODY_U*)argc;
	//if(NULL==pstEvent)
	//{
	//	return IVS_FAIL;
	//}
	//CSDKDecoderMgr* pDecoder = NULL;
	//for(SDK_DECODER_MGR_LIST::iterator ite=m_DecMgrObjList.begin();ite!=m_DecMgrObjList.end();ite++)
	//{
	//	pDecoder = dynamic_cast<CSDKDecoderMgr*>(*ite);
	//	if(NULL!=pDecoder)
	//	{
	//		if(pDecoder->IsIncludeID(pstEvent->stPuEventCommon.ulIdentifyID))
	//		{
	//			break;
	//		}
	//		else
	//		{
	//			pDecoder = NULL;
	//		}
	//	}
	//}
	//if(NULL==pDecoder)
	//{
	//	return IVS_FAIL;
	//}
	//switch(pstEvent->stPuEventCommon.enEventType)
	//{
	//case PU_EVENT_TYPE_DISCONNECT://断连
	//case PU_EVENT_TYPE_SEND_RECV_ERROR://发送或接收失败
	//	{
	//		pDecoder->NotifyDisConnect(pstEvent->stPuEventCommon.ulIdentifyID);
	//	}
	//	break;
	//default:
	//	{
	//		BP_DBG_LOG("Decoder SDK Don't need to Process, EventType=%d", pstEvent->stPuEventCommon.enEventType);
	//	}
	//	break;
	//}
	//////////////////////////////////////////////////////////////////////////
	return IVS_SUCCEED;
}//lint !e818

IVS_VOID  CSDKDecoderMgr::LoginToDecoder()
{
	//////////////////////////////////////////////////////////////////////////
	//2013-1-28 高书明 先屏蔽HWPuSDK相关函数，与BP的dll冲突
	m_bThreadExit = true;
	//typedef std::list<std::string> STRING_LIST;
	//STRING_LIST offLineList;
	////获取没有在线的解码器
	//VOS_MutexLock(m_pDecoderInfoMapMutex);
	//for(SDK_DECODER_INFO_MAP::iterator ite=m_decoderInfoMap.begin();ite!=m_decoderInfoMap.end();ite++)
	//{
	//	SDK_DECODER_INFO* pInfo = dynamic_cast<SDK_DECODER_INFO*>(ite->second);//lint !e611
	//	if(!pInfo->bOnline)
	//	{
	//		offLineList.push_back(ite->first);
	//	}
	//}
	//VOS_MutexUnlock(m_pDecoderInfoMapMutex);
	//// 登陆解码器，为了防止登陆时间过长，导致锁长时间被占用，每登陆一个释放一次锁
	//for(STRING_LIST::iterator ite=offLineList.begin();ite!=offLineList.end();ite++)
	//{
	//	CUMW_NOTIFY_INFO* pstNotifyInfo = NULL;
	//	//
	//	VOS_MutexLock(m_pDecoderInfoMapMutex);
	//	SDK_DECODER_INFO_MAP::iterator iteInfo = m_decoderInfoMap.find(*ite);
	//	if(m_decoderInfoMap.end()!=iteInfo)
	//	{
	//		SDK_DECODER_INFO* pInfo = dynamic_cast<SDK_DECODER_INFO*>(iteInfo->second);//lint !e611
	//		if(NULL!=pInfo)
	//		{
	//			IVS_ULONG ulID = IVS_PU_Login(pInfo->szDecoderIP, pInfo->usDecoderPort, pInfo->szUserName, pInfo->szPWD);
	//			if(PU_FALSE!=ulID)
	//			{//登陆成功
	//				pstNotifyInfo = IVS_NEW(pstNotifyInfo);
	//				memset(pstNotifyInfo, 0, sizeof(CUMW_NOTIFY_INFO));
	//				CUMW_NOTIFY_REMOTE_DECODER_STATUS* pstRegInfo = IVS_NEW(pstRegInfo);
	//				memset(pstRegInfo, 0, sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS));
	//				if(NULL!=pstNotifyInfo && NULL!=pstRegInfo)//lint !e774
	//				{
	//					pstNotifyInfo->ulNotifyType = CUMW_NOTIFY_TYPE_REMOTE_DECODER_REGISTER;
	//					pstNotifyInfo->pNotifyInfo = pstRegInfo;
	//					pstNotifyInfo->ulNotifyInfoLen = sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS);
	//					IVS_INT32 iRet = GetDecoderRegInfo(ulID, pInfo->szDecoderID, pInfo->szDecoderIP, pstRegInfo);
	//					if(IVS_SUCCEED!=iRet)
	//					{
	//						IVS_DELETE(pstNotifyInfo);
	//						IVS_DELETE(pstRegInfo);
	//					}
	//					else
	//					{
	//						pInfo->bOnline = TRUE;
	//						pInfo->ulIdentifyID = ulID;
	//					}
	//				}
	//				else
	//				{
	//					IVS_DELETE(pstNotifyInfo);
	//					IVS_DELETE(pstRegInfo);
	//					(void)IVS_PU_Logout(ulID);
	//				}
	//			}
	//		}
	//		else
	//		{
	//			m_decoderInfoMap.erase(iteInfo);
	//		}
	//	}
	//	VOS_MutexUnlock(m_pDecoderInfoMapMutex);
	//	//通知上层,为了防止锁长时间被占用，在锁外部调用回调函数
	//	if(NULL!=m_pCallBackFun && NULL!=pstNotifyInfo && NULL!=m_pUserData)
	//	{
	//		(void)m_pCallBackFun(pstNotifyInfo, m_pUserData);
	//	}
	//	if(NULL!=pstNotifyInfo)
	//	{
	//		CUMW_NOTIFY_REMOTE_DECODER_STATUS* pRegInfo = (CUMW_NOTIFY_REMOTE_DECODER_STATUS*)pstNotifyInfo->pNotifyInfo;
	//		IVS_DELETE(pRegInfo);
	//		IVS_DELETE(pstNotifyInfo);
	//	}
	//}
	//////////////////////////////////////////////////////////////////////////
}

IVS_INT32 CSDKDecoderMgr::GetDecoderRegInfo(IVS_ULONG /*ulID*/, const std::string& /*strDecoderID*/, 
											const std::string& /*strDecoderIP*/, 
											CUMW_NOTIFY_REMOTE_DECODER_STATUS* pRegInfo) const
{
	pRegInfo->ulChannelNum = 0;
	return IVS_FAIL;
	//////////////////////////////////////////////////////////////////////////
	//2013-1-28 高书明 先屏蔽HWPuSDK相关函数，与BP的dll冲突
	//pRegInfo->ucDecoderType = CUMW_DECODER_TYPE_SDK;
	//pRegInfo->ulDecoderStatus = CUMW_NOTIFY_TYPE_REMOTE_DECODER_REGISTER;
	//PU_DEC_PORT_INFO_LIST stPortInfoList = {0};
	//BOOL bRet = IVS_PU_GetDecPortInfoList(ulID, &stPortInfoList);
	//if(PU_TRUE!=bRet)
	//{
	//	IVS_ULONG ulErr = IVS_PU_GetLastError();
	//	BP_RUN_LOG_ERR(IVS_FAIL, "Get Decoder Port Info", "Failed, errCode=%lu", ulErr);
	//	return IVS_FAIL;
	//}
	//PU_DEC_CHANNEL_ABILITY_LIST_S stChannelAbilityList = {0};
	//bRet = IVS_PU_GetDecChannelAbility(ulID, &stChannelAbilityList);
	//if(PU_TRUE!=bRet)
	//{
	//	IVS_ULONG ulErr = IVS_PU_GetLastError();
	//	BP_RUN_LOG_ERR(IVS_FAIL, "Get Decoder Channel Info", "Failed, errCode=%lu", ulErr);
	//	return IVS_FAIL;
	//}

	//if(!CToolsHelp::Strncpy(pRegInfo->szDecoderID, sizeof(pRegInfo->szDecoderID), strDecoderID.c_str(), strDecoderID.length()))
	//{
	//	BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy Decoder ID", "Failed");
	//	return IVS_ALLOC_MEMORY_ERROR;
	//}
	//if(!CToolsHelp::Strncpy(pRegInfo->szDecoderName, sizeof(pRegInfo->szDecoderName), strDecoderIP.c_str(), strDecoderIP.length()))
	//{
	//	BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy Decoder Name", "Failed");
	//	return IVS_ALLOC_MEMORY_ERROR;
	//}
	//if(!CToolsHelp::Strncpy(pRegInfo->szDecoderIP, sizeof(pRegInfo->szDecoderIP), strDecoderIP.c_str(), strDecoderIP.length()))
	//{
	//	BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy Decoder IP", "Failed");
	//	return IVS_ALLOC_MEMORY_ERROR;
	//}
	//pRegInfo->ulChannelNum = min(stChannelAbilityList.ulChannelNums, CUMW_DECODER_CHANNEL_MAX_NUM);
	//for(unsigned long i=0;i<pRegInfo->ulChannelNum;i++)
	//{
	//	pRegInfo->stChannelInfoArray[i].ulChannelID = stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulChannelID;
	//	switch(stChannelAbilityList.stChnAttri[i].stChannelPortMap.enPortType)
	//	{
	//	case PORT_TYPE_HDMI:
	//		{
	//			if(!CToolsHelp::Snprintf(pRegInfo->stChannelInfoArray[i].szChannelName, sizeof(pRegInfo->stChannelInfoArray[i].szChannelName), "HDMI-%lu", pRegInfo->stChannelInfoArray[i].ulChannelID))
	//			{
	//				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy Decoder Channel Name", "Failed");
	//				return IVS_ALLOC_MEMORY_ERROR;
	//			}
	//			if(stPortInfoList.ulHDMINum<=stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulPortIndex && stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulPortIndex>0)
	//			{
	//				pRegInfo->stChannelInfoArray[i].ucPortMode = stPortInfoList.stHDMIAttribute[stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulPortIndex-1].enPortMode;
	//			}
	//			else
	//			{
	//				pRegInfo->stChannelInfoArray[i].ucPortMode = PORT_MODE_1;
	//			}
	//		}
	//		break;
	//	case PORT_TYPE_VGA:
	//		{
	//			if(!CToolsHelp::Snprintf(pRegInfo->stChannelInfoArray[i].szChannelName, sizeof(pRegInfo->stChannelInfoArray[i].szChannelName), "VGA-%lu", pRegInfo->stChannelInfoArray[i].ulChannelID))
	//			{
	//				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy Decoder Channel Name", "Failed");
	//				return IVS_ALLOC_MEMORY_ERROR;
	//			}
	//			if(stPortInfoList.ulVGANum<=stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulPortIndex && stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulPortIndex>0)
	//			{
	//				pRegInfo->stChannelInfoArray[i].ucPortMode = stPortInfoList.stVGAAttribute[stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulPortIndex-1].enPortMode;
	//			}
	//			else
	//			{
	//				pRegInfo->stChannelInfoArray[i].ucPortMode = PORT_MODE_1;
	//			}
	//		}
	//		break;
	//	case PORT_TYPE_BNC:
	//		{
	//			if(!CToolsHelp::Snprintf(pRegInfo->stChannelInfoArray[i].szChannelName, sizeof(pRegInfo->stChannelInfoArray[i].szChannelName), "BNC-%lu", pRegInfo->stChannelInfoArray[i].ulChannelID))
	//			{
	//				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy Decoder Channel Name", "Failed");
	//				return IVS_ALLOC_MEMORY_ERROR;
	//			}
	//			if(stPortInfoList.ulBNCNum<=stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulPortIndex && stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulPortIndex>0)
	//			{
	//				pRegInfo->stChannelInfoArray[i].ucPortMode = stPortInfoList.stBNCAttribute[stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulPortIndex-1].enPortMode;
	//			}
	//			else
	//			{
	//				pRegInfo->stChannelInfoArray[i].ucPortMode = PORT_MODE_1;
	//			}
	//		}
	//		break;
	//	case PORT_TYPE_DVI:
	//		{
	//			if(!CToolsHelp::Snprintf(pRegInfo->stChannelInfoArray[i].szChannelName, sizeof(pRegInfo->stChannelInfoArray[i].szChannelName), "DVI-%lu", pRegInfo->stChannelInfoArray[i].ulChannelID))
	//			{
	//				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy Decoder Channel Name", "Failed");
	//				return IVS_ALLOC_MEMORY_ERROR;
	//			}
	//			if(stPortInfoList.ulDVINum<=stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulPortIndex && stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulPortIndex>0)
	//			{
	//				pRegInfo->stChannelInfoArray[i].ucPortMode = stPortInfoList.stDVIAttribute[stChannelAbilityList.stChnAttri[i].stChannelPortMap.ulPortIndex-1].enPortMode;
	//			}
	//			else
	//			{
	//				pRegInfo->stChannelInfoArray[i].ucPortMode = PORT_MODE_1;
	//			}
	//		}
	//		break;
	//	default:
	//		{
	//			if(!CToolsHelp::Snprintf(pRegInfo->stChannelInfoArray[i].szChannelName, sizeof(pRegInfo->stChannelInfoArray[i].szChannelName), "CH-%lu", pRegInfo->stChannelInfoArray[i].ulChannelID))
	//			{
	//				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy Decoder Channel Name", "Failed");
	//				return IVS_ALLOC_MEMORY_ERROR;
	//			}
	//			BP_RUN_LOG_ERR(IVS_FAIL,"Get Decoder Info", "Unknown Port type:%d", stChannelAbilityList.stChnAttri[i].stChannelPortMap.enPortType);
	//			pRegInfo->stChannelInfoArray[i].ucPortMode = PORT_MODE_1;
	//		}
	//		break;
	//	}
	//	
	//}
	//return IVS_SUCCEED;
	//////////////////////////////////////////////////////////////////////////
}
// 是否包含该登录信息解码器
IVS_BOOL CSDKDecoderMgr::IsIncludeID(IVS_ULONG ulIdentifyID)
{
	IVS_BOOL bFind = FALSE;
	(void)VOS_MutexLock(m_pDecoderInfoMapMutex);
	for(SDK_DECODER_INFO_MAP::iterator ite=m_decoderInfoMap.begin();ite!=m_decoderInfoMap.end();ite++)
	{
		SDK_DECODER_INFO* pInfo = dynamic_cast<SDK_DECODER_INFO*>(ite->second);//lint !e611
		if(NULL!=pInfo)
		{
			if(ulIdentifyID==pInfo->ulIdentifyID)
			{
				bFind = TRUE;
				break;
			}
		}
	}
	(void)VOS_MutexUnlock(m_pDecoderInfoMapMutex);
	return bFind;
}

IVS_VOID CSDKDecoderMgr::AddDecMgrObj(CSDKDecoderMgr* pDecoder)
{
	m_DecMgrObjList.push_back(pDecoder);
}

IVS_VOID CSDKDecoderMgr::RemoveDecMgrObj(const CSDKDecoderMgr* pDecoder)
{
	try
	{
		SDK_DECODER_MGR_LIST::iterator ite = m_DecMgrObjList.begin();
		while(m_DecMgrObjList.end()!=ite)
		{
			if(pDecoder==(*ite))
			{
				m_DecMgrObjList.erase(ite);
				break;
			}
			ite++;
		}
	}
	catch (...)
	{
	}
}

// 断连通知
IVS_VOID CSDKDecoderMgr::NotifyDisConnect(IVS_ULONG ulIdentifyID)
{
	std::string strDecoderId;
	(void)VOS_MutexLock(m_pDecoderInfoMapMutex);
	for(SDK_DECODER_INFO_MAP::iterator ite=m_decoderInfoMap.begin();ite!=m_decoderInfoMap.end();ite++)
	{
		SDK_DECODER_INFO* pInfo = dynamic_cast<SDK_DECODER_INFO*>(ite->second);//lint !e611
		if(NULL!=pInfo)
		{
			if(ulIdentifyID==pInfo->ulIdentifyID)
			{
				strDecoderId = pInfo->szDecoderID;
				pInfo->bOnline = FALSE;
				pInfo->ulIdentifyID = 0;
				break;
			}
		}
	}
	(void)VOS_MutexUnlock(m_pDecoderInfoMapMutex);
	if(NULL!=m_pCallBackFun && NULL!=m_pUserData && strDecoderId.length()>0)
	{			
		CUMW_NOTIFY_INFO stNotifyInfo = {0};
		CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO* pstOffline = IVS_NEW(pstOffline);
		if(NULL!=pstOffline)
		{
			stNotifyInfo.ulNotifyType = CUMW_NOTIFY_TYPE_REMOTE_DECODER_DISCONNECT;
			stNotifyInfo.pNotifyInfo = pstOffline;
			stNotifyInfo.ulNotifyInfoLen = sizeof(CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO);
			if(CToolsHelp::Strncpy(pstOffline->szDecoderID, sizeof(pstOffline->szDecoderID), strDecoderId.c_str(), strDecoderId.length()))
			{
				(void)m_pCallBackFun(&stNotifyInfo, m_pUserData);
			}
			else
			{
				BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Decoder disconnect", "Copy Decoder ID Failed");
			}
			IVS_DELETE(pstOffline);
		}
	}
}

// 移除解码器信息
IVS_VOID CSDKDecoderMgr::RemoveDecoderInfo()
{
	(void)VOS_MutexLock(m_pDecoderInfoMapMutex);
	try
	{
		for(SDK_DECODER_INFO_MAP::iterator ite=m_decoderInfoMap.begin();ite!=m_decoderInfoMap.end();ite++)
		{
			SDK_DECODER_INFO* pInfo = dynamic_cast<SDK_DECODER_INFO*>(ite->second);//lint !e611
			if(NULL!=pInfo)
			{
				if(pInfo->bOnline)
				{
					//////////////////////////////////////////////////////////////////////////
					//2013-1-28 高书明 先屏蔽HWPuSDK相关函数，与BP的dll冲突
					//(void)IVS_PU_Logout(pInfo->ulIdentifyID);
					//////////////////////////////////////////////////////////////////////////
					pInfo->bOnline = FALSE;
					pInfo->ulIdentifyID = 0;
				}
				IVS_DELETE(pInfo);
			}
		}
		m_decoderInfoMap.clear();
	}
	catch(...)
	{
		m_decoderInfoMap.clear();
	}

	(void)VOS_MutexUnlock(m_pDecoderInfoMapMutex);
}
