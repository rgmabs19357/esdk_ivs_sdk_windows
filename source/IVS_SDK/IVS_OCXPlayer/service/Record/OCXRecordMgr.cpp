#include "OCXRecordMgr.h"
#include "OCXXmlProcess.h"
#include "OCXRecordXMLProcess.h"
#include "IVS_Trace.h"

OCXRecordMgr::OCXRecordMgr(void)
{
}


OCXRecordMgr::~OCXRecordMgr(void)
{
}
//增加录像计划
CString OCXRecordMgr::AddRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID)
{
	IVS_DEBUG_TRACE("");
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlReq;
	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	if (FALSE == xmlReq.Parse(pRecordPlanReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Add Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	IVS_DEVICE_CODE* pDeviceList;
	IVS_UINT32 uiDeviceNum = 0;
	IVS_UINT32 CameraCodeNodeNum = 0;
	iRet = static_cast<IVS_INT32>(COCXXmlProcess::GetElemCount("Content/CameraList",CameraCodeNodeNum,xmlReq));
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Add Record Plan", "Get Elem Count fail,Content/CameraList Not Exist");
	}
	pDeviceList = IVS_NEW((IVS_DEVICE_CODE*&)pDeviceList,CameraCodeNodeNum);

	IVS_RECORD_PLAN stRecordPlan;
	memset(pDeviceList,0,sizeof(IVS_DEVICE_CODE));
	memset(&stRecordPlan,0,sizeof(IVS_RECORD_PLAN));
	if(IVS_SUCCEED!=COCXRecordXMLProcess::AddOrModRecordPlanGetXML(uiDeviceNum,pDeviceList,&stRecordPlan,xmlReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Add Record Plan", "ErrorCode =%d", iRet);
		IVS_DELETE(pDeviceList,MUILI);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);	
		return strResult;
	}

	//为结果LIST分配内存
	IVS_UINT32 uiBufSize = CameraCodeNodeNum*sizeof(IVS_RESULT_LIST) +1 + sizeof(IVS_UINT32);
	IVS_CHAR* pResultListBuf = IVS_NEW(pResultListBuf, uiBufSize);
	if (NULL == pResultListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(iRet,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Add Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	memset(pResultListBuf, 0, uiBufSize);
	IVS_RESULT_LIST* pResultList = reinterpret_cast<IVS_RESULT_LIST*>(pResultListBuf);//lint !e826
	iRet = IVS_SDK_AddRecordPlan(iSessionID,uiDeviceNum,pDeviceList,&stRecordPlan,pResultList,uiBufSize);
	if (iRet==IVS_SUCCEED)
	{
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Add Record Plan Succeed", "ResultCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}
	if (iRet==IVS_PARA_INVALID || iRet==IVS_OPERATE_MEMORY_ERROR
		|| iRet==IVS_SDK_RET_BP_CMD_REQ_INVALID || iRet==IVS_SDK_RET_BP_CMD_RSP_INVALID
		|| iRet==IVS_XML_PACK_ERROR )
	{
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(iRet,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Add Record Plan Fail", "ResultCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	//拼装响应消息结构体为XML
	if(IVS_SUCCEED != COCXRecordXMLProcess::RecordPlanGetRspXML(iRet,pResultList,xmlRsp))
	{
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(IVS_XML_INVALID,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Add Record Plan", "ErrorCode =%d", IVS_XML_INVALID);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	IVS_DELETE(pResultListBuf, MUILI);
	IVS_DELETE(pDeviceList,MUILI);
	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult;
}
//修改录像计划
CString OCXRecordMgr::ModifyRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID)
{
	IVS_DEBUG_TRACE("");
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlReq;
	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	if (FALSE == xmlReq.Parse(pRecordPlanReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Modify Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	IVS_UINT32 uiDeviceNum = 0;
	IVS_DEVICE_CODE* pDeviceList;
	IVS_UINT32 CameraCodeNodeNum = 0;
	iRet = static_cast<IVS_INT32>(COCXXmlProcess::GetElemCount("Content/CameraList",CameraCodeNodeNum,xmlReq));
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Modify Record Plan", "Get Elem Count fail,Content/CameraList Not Exist");
	}
	pDeviceList = IVS_NEW((IVS_DEVICE_CODE*&)pDeviceList,CameraCodeNodeNum);

	IVS_RECORD_PLAN stRecordPlan;
	memset(pDeviceList,0,sizeof(IVS_DEVICE_CODE));
	memset(&stRecordPlan,0,sizeof(IVS_RECORD_PLAN));
	if(IVS_SUCCEED!=COCXRecordXMLProcess::AddOrModRecordPlanGetXML(uiDeviceNum,pDeviceList,&stRecordPlan,xmlReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Modify Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}
	//为结果LIST分配内存
	IVS_UINT32 uiBufSize = CameraCodeNodeNum*sizeof(IVS_RESULT_LIST) + sizeof(IVS_UINT32)+1;
	
	IVS_CHAR* pResultListBuf = IVS_NEW(pResultListBuf, uiBufSize);
	if (NULL == pResultListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(iRet,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Modify Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	memset(pResultListBuf, 0, uiBufSize);

	IVS_RESULT_LIST* pResultList = reinterpret_cast<IVS_RESULT_LIST*>(pResultListBuf);//lint !e826
	iRet = IVS_SDK_ModifyRecordPlan(iSessionID,uiDeviceNum,pDeviceList,&stRecordPlan,pResultList,uiBufSize);
	if (iRet==IVS_SUCCEED)
	{
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Modify Record Plan Succeed", "ResultCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}
	if (iRet==IVS_PARA_INVALID || iRet==IVS_OPERATE_MEMORY_ERROR
		|| iRet==IVS_SDK_RET_BP_CMD_REQ_INVALID || iRet==IVS_SDK_RET_BP_CMD_RSP_INVALID
		|| iRet==IVS_XML_PACK_ERROR )
	{
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(iRet,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Modify Record Plan Fail", "ResultCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	//拼装响应消息结构体为XML
	if(IVS_SUCCEED != COCXRecordXMLProcess::RecordPlanGetRspXML(iRet,pResultList,xmlRsp))
	{
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(IVS_XML_INVALID,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Modify Record Plan", "ErrorCode =%d", IVS_XML_INVALID);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	IVS_DELETE(pResultListBuf, MUILI);
	IVS_DELETE(pDeviceList,MUILI);
	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult;
}
//删除录像计划
CString OCXRecordMgr::DeleteRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID)
{
	IVS_DEBUG_TRACE("");
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlReq;
	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	if (FALSE == xmlReq.Parse(pRecordPlanReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Delete Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	IVS_UINT32 uiDeviceNum = 0;
	IVS_DEVICE_CODE* pDeviceList;
	IVS_UINT32 CameraCodeNodeNum = 0;
	iRet = static_cast<IVS_INT32>(COCXXmlProcess::GetElemCount("Content/CameraList",CameraCodeNodeNum,xmlReq));
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Modify Record Plan", "Get Elem Count fail,Content/CameraList Not Exist");
	}
	pDeviceList = IVS_NEW((IVS_DEVICE_CODE*&)pDeviceList,CameraCodeNodeNum);

	IVS_UINT32 uiRecordMethod = 0;
	memset(pDeviceList,0,sizeof(IVS_DEVICE_CODE));
	if(IVS_SUCCEED!=COCXRecordXMLProcess::DelRecordPlanGetXML(uiDeviceNum,pDeviceList,uiRecordMethod,xmlReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Delete Record Plan", "ErrorCode =%d", iRet);
		IVS_DELETE(pDeviceList,MUILI);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}
	//为结果LIST分配内存
	IVS_UINT32 uiBufSize = CameraCodeNodeNum*sizeof(IVS_RESULT_LIST) + sizeof(IVS_UINT32)+1;
	IVS_CHAR* pResultListBuf = IVS_NEW(pResultListBuf, uiBufSize);
	if (NULL == pResultListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(iRet,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Delete Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	memset(pResultListBuf, 0, uiBufSize);

	IVS_RESULT_LIST* pResultList = reinterpret_cast<IVS_RESULT_LIST*>(pResultListBuf);//lint !e826
	iRet = IVS_SDK_DeleteRecordPlan(iSessionID,uiDeviceNum,pDeviceList,uiRecordMethod,pResultList,uiBufSize);
	if (iRet==IVS_SUCCEED)
	{
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Delete Record Plan Succeed", "ResultCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}
	if (iRet==IVS_PARA_INVALID || iRet==IVS_OPERATE_MEMORY_ERROR
		|| iRet==IVS_SDK_RET_BP_CMD_REQ_INVALID || iRet==IVS_SDK_RET_BP_CMD_RSP_INVALID
		|| iRet==IVS_XML_PACK_ERROR )
	{
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(iRet,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Delete Record Plan Fail", "ResultCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	//拼装响应消息结构体为XML
	if(IVS_SUCCEED != COCXRecordXMLProcess::RecordPlanGetRspXML(iRet,pResultList,xmlRsp))
	{
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(IVS_XML_INVALID,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Delete Record Plan", "ErrorCode =%d", IVS_XML_INVALID);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	IVS_DELETE(pResultListBuf, MUILI);
	IVS_DELETE(pDeviceList,MUILI);
	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult;
}
//查询录像计划
CString OCXRecordMgr::GetRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID)
{
	IVS_DEBUG_TRACE("");
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlReq;
	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	if (FALSE == xmlReq.Parse(pRecordPlanReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Get Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	IVS_CHAR pCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	IVS_RECORD_PLAN stRecordPlan;
	memset(&stRecordPlan,0,sizeof(IVS_RECORD_PLAN));
	IVS_UINT32 uiRecordMethod = 0;
	if(IVS_SUCCEED!=COCXRecordXMLProcess::GetRecordPlanGetXML(pCameraCode,uiRecordMethod,xmlReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Get Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	//为结果LIST分配内存
	IVS_UINT32 uiBufSize = sizeof(IVS_RECORD_PLAN) + sizeof(IVS_UINT32)+1;
	IVS_CHAR* pResultListBuf = IVS_NEW(pResultListBuf, uiBufSize);
	if (NULL == pResultListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Get Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	memset(pResultListBuf, 0, uiBufSize);

	IVS_RESULT_LIST* pResultList = reinterpret_cast<IVS_RESULT_LIST*>(pResultListBuf);//lint !e826
	iRet = IVS_SDK_GetRecordPlan(iSessionID,pCameraCode,uiRecordMethod,&stRecordPlan);
	if (iRet!=IVS_SUCCEED)
	{
		IVS_DELETE(pResultListBuf, MUILI);

		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Get Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	//拼装响应消息结构体为XML
	if(IVS_SUCCEED != COCXRecordXMLProcess::GetRecordPlanGetRspXML(iRet,pCameraCode,&stRecordPlan,xmlRsp))
	{
		IVS_DELETE(pResultListBuf, MUILI);

		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Get Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	IVS_DELETE(pResultListBuf, MUILI);
	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult;
}//lint !e529
//查询录像状态
CString OCXRecordMgr::GetRecordStatus(const CString& pCameraCode,const ULONG ulRecordMethod,const IVS_INT32 m_iSDKSessionID)
{
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_FAIL;

	CString strResult;
	CXml xmlRsp;
	IVS_UINT32 ulRecordStatus = 0;
	IVS_UINT32 uiXmlLen = 0;

	iRet = IVS_SDK_GetRecordStatus(m_iSDKSessionID,pCameraCode,ulRecordMethod,&ulRecordStatus);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get Record Status", "IVS_SDK_GetRecordStatus failed");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult;
	}

	iRet = COCXRecordXMLProcess::GetRecordStatusRspXML(iRet,pCameraCode,ulRecordMethod,ulRecordStatus,xmlRsp);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get Record Status", "GetRecordStatusRspXML failed");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult;
	}

	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult;
}

// 设置摄像机备份参数
IVS_INT32 OCXRecordMgr::SetCameraBackupConfig(IVS_INT32 iSessionID, const IVS_CHAR* pBackupXml)
{
	CHECK_POINTER(pBackupXml, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	return IVS_SDK_SetCameraBackupConfig(iSessionID, pBackupXml);
}

// 获取摄像机备份参数
IVS_INT32 OCXRecordMgr::GetCameraBackupCfg(IVS_INT32 iSessionId, const IVS_CHAR* pCameraCode, CString &strResult)
{
	CHECK_POINTER(pCameraCode, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_CHAR* pBackupXml = NULL;
	
	IVS_INT32 iRet = IVS_SDK_GetCameraBackupConfig(iSessionId, pCameraCode, &pBackupXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Camera Backup Config", "failed");
		if (NULL != pBackupXml)
		{
			IVS_SDK_ReleaseBuffer(pBackupXml);
		}
		return iRet;
	}
	iRet = COCXXmlProcess::GetResultXML(iRet, pBackupXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetResultXML to CU is fail", "NA");
		IVS_SDK_ReleaseBuffer(pBackupXml);
		return iRet;
	}
	(void)IVS_SDK_ReleaseBuffer(pBackupXml);

	return iRet;
}

// 添加录像备份计划
IVS_INT32 OCXRecordMgr::AddRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pBackupPlan)
{
	CHECK_POINTER(pBackupPlan, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	return IVS_SDK_AddRecordBackupPlan(iSessionID, pBackupPlan);
}

// 修改录像备份计划
IVS_INT32 OCXRecordMgr::ModifyRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pBackupPlan)
{
	CHECK_POINTER(pBackupPlan, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	return IVS_SDK_ModifyRecordBackupPlan(iSessionID, pBackupPlan);
}

// 删除录像备份计划
IVS_INT32 OCXRecordMgr::DeleteRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pBackupPlan)
{
	CHECK_POINTER(pBackupPlan, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	return IVS_SDK_DeleteRecordBackupPlan(iSessionID, pBackupPlan);
}

// 查询录像备份计划
IVS_INT32 OCXRecordMgr::GetRecordBackupPlan(IVS_INT32 iSessionID, const IVS_CHAR* pQueryInfo, CString &strResult)
{
	CHECK_POINTER(pQueryInfo, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_CHAR* pBackupPlanXml = NULL;

	IVS_INT32 iRet = IVS_SDK_GetRecordBackupPlan(iSessionID, pQueryInfo, &pBackupPlanXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Record Backup Plan", "failed");
		if (NULL != pBackupPlanXml)
		{
			(void)IVS_SDK_ReleaseBuffer(pBackupPlanXml);
		}
		return iRet;
	}
	iRet = COCXXmlProcess::GetResultXML(iRet, pBackupPlanXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetResultXML to CU is fail", "NA");
		(void)IVS_SDK_ReleaseBuffer(pBackupPlanXml);
		return iRet;
	}
	(void)IVS_SDK_ReleaseBuffer(pBackupPlanXml);

	return iRet;
}
//查询手动录像任务
IVS_INT32 OCXRecordMgr::GetRecordTask(IVS_INT32 iSessionId, CString &strResult)
{
	IVS_DEBUG_TRACE("");
	IVS_CHAR* pRecordTaskXml = NULL;

	IVS_INT32 iRet = IVS_SDK_GetRecordTask(iSessionId, &pRecordTaskXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Record Task", "failed");
		if (NULL != pRecordTaskXml)
		{
			IVS_SDK_ReleaseBuffer(pRecordTaskXml);
		}
		return iRet;
	}
	iRet = COCXXmlProcess::GetResultXML(iRet, pRecordTaskXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetResultXML to CU is fail", "NA");
		IVS_SDK_ReleaseBuffer(pRecordTaskXml);
		return iRet;
	}
	(void)IVS_SDK_ReleaseBuffer(pRecordTaskXml);

	return iRet;
}

// 查询录像备份任务列表
IVS_INT32 OCXRecordMgr::GetRecordBackupTaskList(IVS_INT32 iSessionID, const IVS_CHAR* pQueryInfo, CString &strResult)
{
	CHECK_POINTER(pQueryInfo, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	IVS_CHAR* pBackupTaskListXml = NULL;

	IVS_INT32 iRet = IVS_SDK_GetRecordBackupTaskList(iSessionID, pQueryInfo, &pBackupTaskListXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Record Backup Task List", "failed");
		if (NULL != pBackupTaskListXml)
		{
			(void)IVS_SDK_ReleaseBuffer(pBackupTaskListXml);
		}
		return iRet;
	}
	iRet = COCXXmlProcess::GetResultXML(iRet, pBackupTaskListXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetResultXML to CU is fail", "NA");
		(void)IVS_SDK_ReleaseBuffer(pBackupTaskListXml);
		return iRet;
	}
	(void)IVS_SDK_ReleaseBuffer(pBackupTaskListXml);

	return iRet;
}

// 添加录像备份计划
IVS_INT32 OCXRecordMgr::AddRecordBackupTask(IVS_INT32 iSessionID, const IVS_CHAR* pBackupTask)
{
	CHECK_POINTER(pBackupTask, IVS_XML_INVALID);
	IVS_DEBUG_TRACE("");
	return IVS_SDK_AddRecordBackupTask(iSessionID, pBackupTask);
}

// 查询摄像机计划设置信息
IVS_INT32 OCXRecordMgr::GetCameraPlanInfo(IVS_INT32 iSessionID, const IVS_CHAR* pReqXml, CString &strResult)
{
    CHECK_POINTER(pReqXml, IVS_XML_INVALID);
    IVS_DEBUG_TRACE("");
    IVS_INT32 iRet = IVS_FAIL;

    IVS_UINT32 uiPlanType = 0;
    iRet = COCXRecordXMLProcess::GetCameraPlanInfoParseXML(pReqXml, uiPlanType);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "CXml Parse", "xml parsing error");
        return iRet;
    }

    IVS_CHAR* pRspXml = NULL;
    iRet = IVS_SDK_GetCameraPlanInfo(iSessionID, uiPlanType, &pRspXml);

    if(iRet == IVS_SUCCEED)
    {
        CXml rspXml;
        iRet = COCXXmlProcess::GetResultXML(iRet, pRspXml, strResult);
        if (IVS_SUCCEED != iRet)
        {
            BP_RUN_LOG_ERR(iRet, "Get RestltXml to CU  fail", "NA");
            COCXXmlProcess::GetErrString(strResult, iRet);
            //(void)IVS_SDK_ReleaseBuffer(pRspXml);
        }
        (void)IVS_SDK_ReleaseBuffer(pRspXml);

    }
    else
    {
        if (NULL != pRspXml)
        {
            (void)IVS_SDK_ReleaseBuffer(pRspXml);
        }
        COCXXmlProcess::GetErrString(strResult, iRet);
    }

    return iRet;
}

//配置外域摄像机容灾录像计划
CString OCXRecordMgr::AddExDomainRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID)
{
	IVS_DEBUG_TRACE("");
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlReq;
	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	if (FALSE == xmlReq.Parse(pRecordPlanReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Add ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	IVS_UINT32 uiDeviceNum = 0;
	IVS_DEVICE_CODE* pDeviceList;
	IVS_UINT32 CameraCodeNodeNum = 0;
	IVS_UINT32 uiBufSize = CameraCodeNodeNum*sizeof(IVS_RESULT_LIST) + sizeof(IVS_UINT32)+1;
	iRet = static_cast<IVS_INT32>(COCXXmlProcess::GetElemCount("Content/CameraList",CameraCodeNodeNum,xmlReq));
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Add ExDomain Record Plan", "Get Elem Count fail,Content/CameraList Not Exist");
	}
	pDeviceList = IVS_NEW((IVS_DEVICE_CODE*&)pDeviceList,CameraCodeNodeNum);

	IVS_RECORD_PLAN stRecordPlan;
	memset(pDeviceList,0,sizeof(IVS_DEVICE_CODE));
	memset(&stRecordPlan,0,sizeof(IVS_RECORD_PLAN));
	if(IVS_SUCCEED!=COCXRecordXMLProcess::AddOrModRecordPlanGetXML(uiDeviceNum,pDeviceList,&stRecordPlan,xmlReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Add ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	//为结果LIST分配内存
	IVS_CHAR* pResultListBuf = IVS_NEW(pResultListBuf, uiBufSize);
	if (NULL == pResultListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(iRet,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Add ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	memset(pResultListBuf, 0, uiBufSize);

	IVS_RESULT_LIST* pResultList = reinterpret_cast<IVS_RESULT_LIST*>(pResultListBuf);//lint !e826
	iRet = IVS_SDK_AddExDomainRecordPlan(iSessionID,uiDeviceNum,pDeviceList,&stRecordPlan,pResultList,uiBufSize);
	if (iRet==IVS_SUCCEED)
	{
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Add ExDomain Record Plan Succeed", "ResultCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}
	if (iRet==IVS_PARA_INVALID || iRet==IVS_OPERATE_MEMORY_ERROR
		|| iRet==IVS_SDK_RET_BP_CMD_REQ_INVALID || iRet==IVS_SDK_RET_BP_CMD_RSP_INVALID
		|| iRet==IVS_XML_PACK_ERROR )
	{
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(iRet,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Add ExDomain Record Plan Fail", "ResultCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	//拼装响应消息结构体为XML
	if(IVS_SUCCEED != COCXRecordXMLProcess::RecordPlanGetRspXML(iRet,pResultList,xmlRsp))
	{
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(IVS_XML_INVALID,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Add ExDomain Record Plan", "ErrorCode =%d", IVS_XML_INVALID);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	IVS_DELETE(pResultListBuf, MUILI);
	IVS_DELETE(pDeviceList,MUILI);
	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult;
}
//修改外域摄像机容灾录像计划
CString OCXRecordMgr::ModifyExDomainRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID)
{
	IVS_DEBUG_TRACE("");
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlReq;
	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	if (FALSE == xmlReq.Parse(pRecordPlanReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Modify ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	IVS_UINT32 uiDeviceNum = 0;
	IVS_DEVICE_CODE* pDeviceList;
	IVS_UINT32 CameraCodeNodeNum = 0;
	iRet = static_cast<IVS_INT32>(COCXXmlProcess::GetElemCount("Content/CameraList",CameraCodeNodeNum,xmlReq));
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Modify ExDomain Record Plan", "Get Elem Count fail,Content/CameraList Not Exist");
	}
	pDeviceList = IVS_NEW((IVS_DEVICE_CODE*&)pDeviceList,CameraCodeNodeNum);

	IVS_RECORD_PLAN stRecordPlan;
	memset(pDeviceList,0,sizeof(IVS_DEVICE_CODE));
	memset(&stRecordPlan,0,sizeof(IVS_RECORD_PLAN));
	IVS_UINT32 uiBufSize = CameraCodeNodeNum*sizeof(IVS_RESULT_LIST) + sizeof(IVS_UINT32)+1;
	if(IVS_SUCCEED!=COCXRecordXMLProcess::AddOrModRecordPlanGetXML(uiDeviceNum,pDeviceList,&stRecordPlan,xmlReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Modify ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}
	//为结果LIST分配内存
	IVS_CHAR* pResultListBuf = IVS_NEW(pResultListBuf, uiBufSize);
	if (NULL == pResultListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(iRet,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Modify ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}
	memset(pResultListBuf, 0, uiBufSize);

	IVS_RESULT_LIST* pResultList = reinterpret_cast<IVS_RESULT_LIST*>(pResultListBuf);//lint !e826
	iRet = IVS_SDK_ModExDomainRecordPlan(iSessionID,uiDeviceNum,pDeviceList,&stRecordPlan,pResultList,uiBufSize); //IVS_SDK_ModifyRecordPlan(iSessionID,uiDeviceNum,pDeviceList,&stRecordPlan,pResultList,uiBufSize);
	if (iRet==IVS_SUCCEED)
	{
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Modify ExDomain Record Plan Succeed", "ResultCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}
	if (iRet==IVS_PARA_INVALID || iRet==IVS_OPERATE_MEMORY_ERROR
		|| iRet==IVS_SDK_RET_BP_CMD_REQ_INVALID || iRet==IVS_SDK_RET_BP_CMD_RSP_INVALID
		|| iRet==IVS_XML_PACK_ERROR )
	{
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(iRet,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Modify ExDomain Record Plan Fail", "ResultCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	//拼装响应消息结构体为XML
	if(IVS_SUCCEED != COCXRecordXMLProcess::RecordPlanGetRspXML(iRet,pResultList,xmlRsp))
	{
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(IVS_XML_INVALID,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Modify ExDomain Record Plan", "ErrorCode =%d", IVS_XML_INVALID);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	IVS_DELETE(pResultListBuf, MUILI);
	IVS_DELETE(pDeviceList,MUILI);
	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult;
}
//删除外域摄像机容灾录像计划
CString OCXRecordMgr::DeleteExDomainRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID)
{
	IVS_DEBUG_TRACE("");
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlReq;
	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	if (FALSE == xmlReq.Parse(pRecordPlanReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Delete ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	IVS_UINT32 uiDeviceNum = 0;
	IVS_DEVICE_CODE* pDeviceList;
	IVS_UINT32 CameraCodeNodeNum = 0;
	iRet = static_cast<IVS_INT32>(COCXXmlProcess::GetElemCount("Content/CameraList",CameraCodeNodeNum,xmlReq));
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,  "Modify ExDomain Record Plan", "Get Elem Count fail,Content/CameraList Not Exist");
	}
	pDeviceList = IVS_NEW((IVS_DEVICE_CODE*&)pDeviceList,CameraCodeNodeNum);

	IVS_UINT32 uiRecordMethod = 0;
	memset(pDeviceList,0,sizeof(IVS_DEVICE_CODE));
	if(IVS_SUCCEED!=COCXRecordXMLProcess::DelRecordPlanGetXML(uiDeviceNum,pDeviceList,uiRecordMethod,xmlReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Delete ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	//为结果LIST分配内存
	IVS_UINT32 uiBufSize = sizeof(IVS_UINT32)+ CameraCodeNodeNum*sizeof(IVS_RESULT_LIST) +1;
	IVS_CHAR* pResultListBuf = IVS_NEW(pResultListBuf, uiBufSize);
	if (NULL == pResultListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(iRet,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Delete ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	memset(pResultListBuf, 0, uiBufSize);

	IVS_RESULT_LIST* pResultList = reinterpret_cast<IVS_RESULT_LIST*>(pResultListBuf);//lint !e826
	iRet = IVS_SDK_DelExDomainRecordPlan(iSessionID,uiDeviceNum,pDeviceList,uiRecordMethod,pResultList,uiBufSize);//IVS_SDK_DeleteRecordPlan(iSessionID,uiDeviceNum,pDeviceList,uiRecordMethod,pResultList,uiBufSize);
	if (iRet==IVS_SUCCEED)
	{
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Delete ExDomain Record Plan Succeed", "ResultCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}
	if (iRet==IVS_PARA_INVALID || iRet==IVS_OPERATE_MEMORY_ERROR
		|| iRet==IVS_SDK_RET_BP_CMD_REQ_INVALID || iRet==IVS_SDK_RET_BP_CMD_RSP_INVALID
		|| iRet==IVS_XML_PACK_ERROR )
	{
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(iRet,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Delete ExDomain Record Plan Fail", "ResultCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	//拼装响应消息结构体为XML
	if(IVS_SUCCEED != COCXRecordXMLProcess::RecordPlanGetRspXML(iRet,pResultList,xmlRsp))
	{
		COCXRecordXMLProcess::RecordPlanGetRspXMLBypDeviceList(IVS_XML_INVALID,CameraCodeNodeNum,pDeviceList,xmlRsp);
		BP_RUN_LOG_INF("Delete ExDomain Record Plan", "ErrorCode =%d", IVS_XML_INVALID);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		IVS_DELETE(pResultListBuf, MUILI);
		IVS_DELETE(pDeviceList,MUILI);
		return strResult;
	}

	IVS_DELETE(pResultListBuf, MUILI);
	IVS_DELETE(pDeviceList,MUILI);
	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult;
}
//查询外域摄像机容灾录像计划
CString OCXRecordMgr::GetExDomainRecordPlan(const CString& pRecordPlanReq,const IVS_INT32 iSessionID)
{
	IVS_DEBUG_TRACE("");
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlReq;
	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	if (FALSE == xmlReq.Parse(pRecordPlanReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Get ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	IVS_CHAR pCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	memset(pCameraCode, 0, IVS_DEV_CODE_LEN+1);
	IVS_UINT32 uiRecordMethod = 0;
	IVS_RECORD_PLAN stRecordPlan;
	memset(&stRecordPlan,0,sizeof(IVS_RECORD_PLAN));
	if(IVS_SUCCEED!=COCXRecordXMLProcess::GetRecordPlanGetXML(pCameraCode,uiRecordMethod,xmlReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Get ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	//为结果LIST分配内存
	IVS_UINT32 uiBufSize = sizeof(IVS_RECORD_PLAN) + sizeof(IVS_UINT32)+1;
	IVS_CHAR* pResultListBuf = IVS_NEW(pResultListBuf, uiBufSize);
	if (NULL == pResultListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Get ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}
	memset(pResultListBuf, 0, uiBufSize);

	IVS_RESULT_LIST* pResultList = reinterpret_cast<IVS_RESULT_LIST*>(pResultListBuf);//lint !e826
	iRet = IVS_SDK_GetExDomainRecordPlan(iSessionID,pCameraCode,uiRecordMethod,&stRecordPlan);//IVS_SDK_GetRecordPlan(iSessionID,pCameraCode,uiRecordMethod,&stRecordPlan);
	if (iRet!=IVS_SUCCEED)
	{
		IVS_DELETE(pResultListBuf, MUILI);

		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Get ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	//拼装响应消息结构体为XML
	if(IVS_SUCCEED != COCXRecordXMLProcess::GetRecordPlanGetRspXML(iRet,pCameraCode,&stRecordPlan,xmlRsp))
	{
		IVS_DELETE(pResultListBuf, MUILI);

		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("Get ExDomain Record Plan", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult;
	}

	IVS_DELETE(pResultListBuf, MUILI);
	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult;
}//lint !e529


