/********************************************************************
	filename	: 	RecordBookmark.h
	author		:	z00201790
	created		:	2012/12/06	
	description	:	定义录像书签管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/06 初始版本;
*********************************************************************/

#ifndef __RECORD_BOOKMARK_H__
#define __RECORD_BOOKMARK_H__

#include "RecordXMLProcess.h"

class CUserMgr;
class CRecordBookmark
{
public:
	CRecordBookmark(void);
	~CRecordBookmark(void);
	void SetUserMgr(CUserMgr *pUserMgr);

	/***********************************************************************************
    * name       : AddBookmark
    * description: 新增录像书签
    * input      : pCameraCode:摄像机编码; pBookmarkTime:书签时间; pNewBookmarkName:新书签名
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 AddBookmark(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime,IVS_UINT32* uiBookmarkID)const;

	/***********************************************************************************
    * name       : ModifyBookmark
    * description: 修改录像书签
    * input      :pCameraCode:摄像机编码; uiBookmarkID:书签ID; pNewBookmarkName:新书签名
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 ModifyBookmark(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName)const;

	/***********************************************************************************
    * name       : DeleteBookmark
    * description: 删除录像书签
    * input      : pCameraCode:摄像机编码; uiBookmarkID:书签ID
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 DeleteBookmark(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID)const;
	
	/***********************************************************************************
    * name       : GetBookmarkList
    * description: 查询录像书签列表
    * input      : pUnifiedQuery:查询条件; pBookmarkList:书签查询结果列表; uiBufSize:缓存大小
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 GetBookmarkList(IVS_INT32 iRecordMethod,const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkList,IVS_UINT32 uiBufSize)const;

	/***********************************************************************************
    * name       : GetBookmarkListByBackupServer
    * description: 查询录像书签列表(从备份服务器)
    * input      : pUnifiedQuery:查询条件; pBookmarkList:书签查询结果列表; uiBufSize:缓存大小
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 GetBookmarkListByBackupServer(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkList,IVS_UINT32 uiMaxBufSize)const;

private:
	IVS_INT32 GetBookmarkListByBackupServerForOneDomain(IVS_UINT32 uiFromIndex,IVS_UINT32 uiToIndex,const char* cNVRCode,const char* cMBUDomainCode,
		const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkList,IVS_UINT32 uiMaxBufSize)const;
private:
	CUserMgr *m_UserMgr;

};

#endif
