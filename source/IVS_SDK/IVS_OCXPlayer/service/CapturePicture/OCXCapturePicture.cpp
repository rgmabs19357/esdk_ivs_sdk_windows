#include "OCXCapturePicture.h"
#include "OCXXmlProcess.h"
#include "OCXCapturePictureXMLProcess.h"
#include "IVS_Trace.h"
#include <time.h>

OCXCapturePicture::OCXCapturePicture(void)
{
}


OCXCapturePicture::~OCXCapturePicture(void)
{
}

// 获取平台抓拍列表
IVS_INT32 OCXCapturePicture::GetSnapshotList(IVS_INT32 iSessionId, const IVS_CHAR* pQueryParam, CString &strResult)
{
	if (NULL == pQueryParam)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Snapshot List", "param is null");
		return IVS_XML_INVALID;
	}

	IVS_DEBUG_TRACE("");
	CXml xmlReq;
	CXml xmlRsp;
 	IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN + 1] = {0};
	if (!xmlReq.Parse(pQueryParam))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"IVS OCX Query Record List Failed", "Reason: Parse pUnifiedQuery to Xml Failed.");
		return IVS_XML_INVALID;
	}

	IVS_QUERY_SNAPSHOT_PARAM pSnapshotParam;
	memset(&pSnapshotParam, 0, sizeof(IVS_QUERY_SNAPSHOT_PARAM));
    IVS_INT32 iRet = OCXCapturePictureXMLProcess::GetSnapshotListParseXml(xmlReq, &pSnapshotParam, cCameraCode);

	if (IVS_SUCCEED !=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Snapshot List", "failed");
		return iRet;
	}
	IVS_UINT32 iTotalNum = (pSnapshotParam.stIndexRange.uiToIndex) - (pSnapshotParam.stIndexRange.uiFromIndex);
	iTotalNum = iTotalNum + 1;

	IVS_UINT32 uiBufSize = sizeof(IVS_SNAPSHOT_INFO_LIST) + (iTotalNum - 1) * sizeof(IVS_SNAPSHOT_INFO);
	IVS_VOID* pBuff = IVS_NEW((IVS_CHAR *&)pBuff, uiBufSize);
	memset(pBuff, 0, uiBufSize);
	IVS_SNAPSHOT_INFO_LIST* pSnapshotList = reinterpret_cast<IVS_SNAPSHOT_INFO_LIST*>(pBuff);
	pSnapshotList->uiTotal = iTotalNum;
	iRet = IVS_SDK_GetSnapshotList(iSessionId, cCameraCode, &pSnapshotParam, pSnapshotList, uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get Snapshot List", "SDK failed");
		IVS_DELETE(pBuff, MUILI);
		return iRet;
	}

	//拼装图片列表xml
	iRet = OCXCapturePictureXMLProcess::GetSnapshotListGetXml(xmlRsp, pSnapshotList, iTotalNum);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"OCX Get Snapshot List Get Xml", "failed");
		IVS_DELETE(pBuff, MUILI);
		return iRet;
	}
	IVS_UINT32 iLen = 0;
	IVS_DELETE(pBuff, MUILI);
	//把xml生成string
	strResult = xmlRsp.GetXMLStream(iLen);
	return iRet;
}

// 设置平台抓拍计划
LONG OCXCapturePicture:: SetSnapshotPlan(const CString& strSnapshotPlan,const IVS_INT32 iSessionID)
{
	IVS_DEBUG_TRACE("");

	CXml xmlReq;
	if (!xmlReq.Parse(strSnapshotPlan))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Set Snapshot Plan Failed.", "Reason:xml Parse Fail.");
		return IVS_XML_INVALID;
	}

	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	IVS_SNAPSHOT_PLAN stSnapshotPlan;
	IVS_CHAR pCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	//memset(pCameraCode,0,sizeof(IVS_CHAR));
	memset(&stSnapshotPlan,0,sizeof(IVS_SNAPSHOT_PLAN));
	if(IVS_SUCCEED!=OCXCapturePictureXMLProcess::SetSnapshotPlanGetXML(pCameraCode,&stSnapshotPlan,xmlReq))
	{
		iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"Set Snapshot Plan", "ErrorCode =%d", iRet);
		return iRet;
	}
	//IVS_UINT32 uiPlanInfoNodeNum = 0;
	//iRet = static_cast<IVS_INT32>(COCXXmlProcess::GetElemCount("Content/SnapshotPlanInfo/PlanList",uiPlanInfoNodeNum,xmlReq));
	//stSnapshotPlan->uiPlanInfoNum = uiPlanInfoNodeNum;

	iRet = IVS_SDK_SetSnapshotPlan(iSessionID,pCameraCode,&stSnapshotPlan);
	return iRet;
}

// 删除平台抓拍计划
IVS_INT32 OCXCapturePicture::DelSnapshotPlan(IVS_INT32 iSessionId, const IVS_CHAR* pCameraCode)
{
	if (NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Delelte Snapshot", "param is null");
		return IVS_XML_INVALID;
	}

	IVS_DEBUG_TRACE("");
	IVS_INT32 iRet = IVS_SDK_DelSnapshotPlan(iSessionId, pCameraCode);

	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "OCX Delelte Snapshot Plan", "IVS SDK Del Snapshot Plan failed");
		return iRet;
	}
	return iRet;
}

//查询平台抓拍计划
CString OCXCapturePicture::GetSnapshotPlan(const CString& strCameraCode,const IVS_INT32 iSessionID)
{
	IVS_DEBUG_TRACE("");

	CString strResult;
	CXml xmlRsp;
	IVS_UINT32 uiXmlLen = 0;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	IVS_SNAPSHOT_PLAN stSnapshotPlan;
	memset(&stSnapshotPlan,0,sizeof(IVS_SNAPSHOT_PLAN));
	CString cCameraCode = strCameraCode;
	IVS_CHAR* pCameraCode = cCameraCode.GetBuffer();

	iRet = IVS_SDK_GetSnapshotPlan(iSessionID,pCameraCode,&stSnapshotPlan);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get Snapshot Plan", "failed");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult;
	}

	iRet = OCXCapturePictureXMLProcess::GetSnapshotPlanGetXML(iRet,pCameraCode,&stSnapshotPlan,xmlRsp);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"Get Snapshot Plan", "Get Snapshot Plan Rsp XML failed");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult;
	}

	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult;
}


// 删除平台抓拍图片
IVS_INT32 OCXCapturePicture::DeleteSnapshot(IVS_INT32 iSessionID, const IVS_CHAR* pCameraCode, IVS_UINT32 uiPictureID)
{
	if (NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Delete Snapshot", "Camera Code is null");
		return IVS_XML_INVALID;
	}

	IVS_DEBUG_TRACE("");
	return IVS_SDK_DeleteSnapshot(iSessionID, pCameraCode, uiPictureID);
}

