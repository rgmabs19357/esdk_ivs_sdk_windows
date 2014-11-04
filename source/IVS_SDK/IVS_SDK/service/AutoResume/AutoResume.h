/********************************************************************
	filename	: 	AutoResume
	author		:	z00193167
	created		:	2012/12/27
	description	:	自动恢复类
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/27 初始版本;
*********************************************************************/
#ifndef AUTO_RESUME_H
#define AUTO_RESUME_H
 
#include "SDKTimer.h"
#include "SDKDef.h"
#include "RealPlay.h"
#include "RealPlayMgr.h"
#include "LockGuard.h"
#include "vos.h"

 class CAutoResume : public ITrigger
 {
 public:
 	CAutoResume(void);
 	~CAutoResume(void);
 
 	virtual void onTrigger(void* /*pArg*/, ULONGLONG /*ullScales*/, TriggerStyle /*style*/);

 	/**************************************************************************
 	* name       : clearCamerInfoList()
 	* description: 清除重连列表
 	* input      : NA
 	* output     : NA
 	* return     : NA
 	* remark     : NA
 	**************************************************************************/
 	void clearCamerInfoList();
 
 	/**************************************************************************
 	* name       : RemoveStopPlayHandle
 	* description: 移除重连的播放句柄
 	* input      : NA
 	* output     : NA
 	* return     : 返回列表
 	* remark     : NA
 	**************************************************************************/
	void RemoveStopPlayHandle(IVS_ULONG ulHandle);

	/**************************************************************************
	* name       : AddCameraInfoList(CAMERAINFO_LIST CamerInfoList)
	* description: 增加一个列表到已有的列表中
	* input      : CAMERAINFO_LIST CamerInfoList 列表
	* output     : NA
	* return     : true - 添加成功 false - 添加失败
	* remark     : NA
	**************************************************************************/
 	bool AddCameraPlayInfo2List(const CAMERA_PLAY_INFO& cameraPlayInfo);


 private:
 	CAMERA_PLAY_INFO_LIST m_CamerPlayInfoList;				//摄像机信息列表
	VOS_Mutex* m_pstAutoResumeMutex;
 };
 
#endif //AUTO_RESUME_H
