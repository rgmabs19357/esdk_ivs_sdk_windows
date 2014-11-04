#ifndef SDK_MAIN_SERVICE_H
#define SDK_MAIN_SERVICE_H
#include "NSSOperator.h"
#include "Cmd.h"
#include "hwsdk.h"

class CSDKMainService
{
public:
	/**************************************************************************
	* name       : ProcessAlarmAlarmNotifyCmd
	* description: 处理告警信息通知Cmd
	* input      : CCmd
	* output     : NA
	* return     : IVS_ALARM_NOTIFY 告警信息结构体
	* remark     : NA
	**************************************************************************/
	static IVS_ALARM_NOTIFY* ProcessAlarmAlarmNotifyCmd(CCmd* pCmd);

	/**************************************************************************
	* name       : ProcessDoLinkageActionCmd
	* description: 处理联动动作执行通知
	* input      : CCmd
	* output     : NA
	* return     : IVS_CHAR* 通知xml
	* remark     : NA
	**************************************************************************/
	//static IVS_CHAR* ProcessDoLinkageActionCmd(CCmd* pCmd);

	/**************************************************************************
	* name       : ProcessBALearningSchedule
	* description: 处理智能分析
	* input      : CCmd
	* output     : NA
	* return     : IVS_CHAR* xml
	* remark     : NA
	**************************************************************************/
	static IVS_CHAR* ProcessMAUCmd(CCmd* pCmd);

    //// 用户下线通知
    //static IVS_INT32 ParseUserOffLineXML(CXml& xmlRsq, IVS_USER_OFFLINE_NOTIFY &stOffLineInfo);

    //static IVS_ALARM_NOTIFY* ProcessUserOffLineCmd(CCmd* pCmd);

	/**************************************************************************
	* name       : ProcessAlarmStatusNotify
	* description: 告警信息状态上报
	* input      : CCmd
	* output     : NA
	* return     : IVS_ALARM_STATUS_NOTIFY* 状态结构体
	* remark     : NA
	**************************************************************************/
	static IVS_ALARM_STATUS_NOTIFY* ProcessAlarmStatusNotify(CCmd* pCmd);

	/**************************************************************************
	* name       : ProcessDevNotify
	* description: 推送发现到的前端设备
	* input      : CCmd
	* output     : NA
	* return     : 发现的设备信息
	* remark     : NA
	**************************************************************************/
	static IVS_CHAR* ProcessDevNotify(CCmd* pCmd);

	/**************************************************************************
	* name       : ProcessDevAlarmNotify
	* description: 设备告警上报
	* input      : CCmd
	* output     : NA
	* return     : IVS_DEVICE_ALARM_NOTIFY* 设备告警上报结构体
	* remark     : NA
	**************************************************************************/
	static IVS_INT32 ProcessDevAlarmNotify(CCmd* pCmd, IVS_DEVICE_ALARM_NOTIFY* pDevAlarmNotify);
	/**************************************************************************
	* name       : ProcessManualRecordStateNotify
	* description: 手动录像信息状态上报
	* input      : CCmd
	* output     : NA
	* return     : IVS_MANUAL_RECORD_STATUS_NOTIFY* 状态结构体
	* remark     : NA
	**************************************************************************/
	static IVS_INT32 ProcessManualRecordStateNotify(CCmd* pCmd,IVS_MANUAL_RECORD_STATUS_NOTIFY& stManualRecordStatusNotify);

	/**************************************************************************
	* name       : ProcessModeCruiseRecordOver
	* description: 模式轨迹录制结束通知
	* input      : CCmd
	* output     : NA
	* return     : 模式轨迹录制结束信息
	* remark     : NA
	**************************************************************************/
	//static IVS_CHAR* ProcessModeCruiseRecordOver(CCmd* pCmd);
	
};


#endif // SDK_MAIN_SERVICE_H
