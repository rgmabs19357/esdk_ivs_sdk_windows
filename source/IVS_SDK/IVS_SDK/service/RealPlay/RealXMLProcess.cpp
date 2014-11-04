#include "RealXMLProcess.h"
#include "ToolsHelp.h"
#include "IVS_Trace.h"
#include "MediaBase.h"


//添加实况书签的请求XML
IVS_INT32 CRealXMLProcess::AddBookmarkGetXML(const IVS_UINT32 uiUserId,const IVS_CHAR* cUserName,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pDomainCode,
	const IVS_CHAR* pCameraCode,const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime, CXml &xml)
{
	CHECK_POINTER(cUserName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pBookmarkName, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pBookmarkTime, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	IVS_INT32 iRecordMethod = 0;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");

	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDomaCode);


	(void)xml.AddElem("RecordBookmark");
	(void)xml.AddChildElem("CameraCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDevCode); 
	(void)xml.AddElem("RecordMethod");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRecordMethod).c_str()); 
	(void)xml.AddElem("BookmarkName");
	(void)xml.SetElemValue(pBookmarkName); 
	(void)xml.AddElem("BookmarkTime");
	(void)xml.SetElemValue(pBookmarkTime); 
	(void)xml.AddElem("BookmarkCreatorDomain");
	(void)xml.SetElemValue(pUserDomainCode); 
	(void)xml.AddElem("BookmarkCreatorID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserId)).c_str()); 
	(void)xml.AddElem("BookmarkCreatorName");
	(void)xml.SetElemValue(cUserName); 

	xml.OutOfElem();
	return IVS_SUCCEED;
}
//修改实况书签的请求XML
IVS_INT32 CRealXMLProcess::ModifyBookmarkGetXML(const IVS_UINT32 uiUserId,const IVS_CHAR* cUserName,const IVS_CHAR* pUserDomainCode,const IVS_CHAR* pDomainCode,
	const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName, CXml &xml)
{
	CHECK_POINTER(cUserName, IVS_OPERATE_MEMORY_ERROR); 
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pNewBookmarkName, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	std::string strRecordMethod = "RecordMethod";
	IVS_INT32 iRecordMethod = 0;
	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDomaCode); 
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(chDevCode);

	(void)xml.AddElem(strRecordMethod.c_str());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRecordMethod).c_str()); 
	(void)xml.AddElem("RecordBookmark");
	(void)xml.AddChildElem("BookmarkID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiBookmarkID)).c_str()); 
	(void)xml.AddElem("BookmarkName");
	(void)xml.SetElemValue(pNewBookmarkName); 
	(void)xml.AddElem("BookmarkCreatorDomain");
	(void)xml.SetElemValue(pUserDomainCode); 
	(void)xml.AddElem("BookmarkCreatorID");
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiUserId)).c_str()); 
	(void)xml.AddElem("BookmarkCreatorName");
	(void)xml.SetElemValue(cUserName); 

	xml.OutOfElem();
	return IVS_SUCCEED;
}
//删除实况书签的请求XML
IVS_INT32 CRealXMLProcess::DeleteBookmarkGetXML(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pCameraCode,IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");
	std::string strRecordMethod = "RecordMethod";
	IVS_INT32 iRecordMethod = 0;
	// 摄像机编号去掉域编号处理
	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("DomainCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(chDomaCode); 
	(void)xml.AddElem("CameraCode");
	(void)xml.SetElemValue(chDevCode);

	(void)xml.AddElem(strRecordMethod.c_str());
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRecordMethod).c_str()); 
	(void)xml.AddElem("RecordBookmark");
	(void)xml.AddChildElem("BookmarkID");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(static_cast<IVS_INT32>(uiBookmarkID)).c_str()); 

	xml.OutOfElem();
	return IVS_SUCCEED;
}


IVS_INT32 CRealXMLProcess::ParseShutdownNotifyXML(CXml& xmlRsq, IVS_SHUTDOWN_NOTIFY_INFO *pShutdownNotifyInfo)
{
	IVS_DEBUG_TRACE("");
	CHECK_POINTER(pShutdownNotifyInfo, IVS_PARA_INVALID);
	 
	const IVS_CHAR *AttriValue = NULL;   //临时存储单个节点值
	if (!xmlRsq.FindElemEx("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "CRealXMLProcess::ParseShutdownNotiryXML", "Content is null");
		return IVS_XML_INVALID;
	}

	GET_ELEM_VALUE_CHAR("DomainCode", AttriValue, pShutdownNotifyInfo->cDomainCode, IVS_DOMAIN_CODE_LEN, xmlRsq);
	GET_ELEM_VALUE_CHAR("CameraCode", AttriValue, pShutdownNotifyInfo->cCameraCode, IVS_DEV_CODE_LEN, xmlRsq);
	GET_ELEM_VALUE_NUM_FOR_UINT("StreamType", AttriValue, pShutdownNotifyInfo->uiStreamType, xmlRsq);
	GET_ELEM_VALUE_NUM_FOR_UINT("Duration", AttriValue, pShutdownNotifyInfo->uiDuration, xmlRsq);
	return IVS_SUCCEED;
}