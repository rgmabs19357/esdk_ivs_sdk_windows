/********************************************************************
	filename	: 	RealPlayBookmark.h
	author		:	z00201790
	created		:	2012/12/06	
	description	:	定义录像书签管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/06 初始版本;
*********************************************************************/

#ifndef __REALPLAY_BOOKMARK_H__
#define __REALPLAY_BOOKMARK_H__

#include "RealXMLProcess.h"

class CUserMgr;
class CRealPlayBookmark
{
public:
	CRealPlayBookmark(void);
	~CRealPlayBookmark(void);
	void SetUserMgr(CUserMgr *pUserMgr);

	/***********************************************************************************
    * name       : AddBookmark
    * description: 新增实况书签
    * input      : pCameraCode:摄像机编码; pBookmarkTime:书签时间; pNewBookmarkName:新书签名
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 AddBookmark(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime,IVS_UINT32* uiBookmarkID)const;

	/***********************************************************************************
    * name       : ModifyBookmark
    * description: 修改实况书签
    * input      :pCameraCode:摄像机编码; uiBookmarkID:书签ID; pNewBookmarkName:新书签名
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 ModifyBookmark(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName)const;

	/***********************************************************************************
    * name       : DeleteBookmark
    * description: 删除实况书签
    * input      : pCameraCode:摄像机编码; uiBookmarkID:书签ID
    * output     : NA
    * return     : 成功返回0，失败返回负数错误码
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 DeleteBookmark(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID)const;
	

private:
	CUserMgr *m_UserMgr;
};
#endif
