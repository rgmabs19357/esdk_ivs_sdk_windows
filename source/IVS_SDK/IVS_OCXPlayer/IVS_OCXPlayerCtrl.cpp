 // IVS_OCXPlayerCtrl.cpp : Implementation of the CIVS_OCXPlayerCtrl ActiveX Control class.

#include "stdafx.h"
#include "IVS_OCXPlayer.h"
#include "IVS_OCXPlayerCtrl.h"
#include "IVS_OCXPlayerPropPage.h"
#include "VideoPaneMgr.h"
#include "Favorite.h"
#include "OCXAlarmMgrXMLProcess.h"
#include "OCXDevicMgrXMLProcess.h"
#include "OCXDeviceParaConfig.h"
#include "OCXRecordXMLProcess.h"
#include "OCXUserInfoMgr.h"
#include "OCXUserInfoMgrXMLProcess.h"
#include "OCXRoleMgr.h"
#include "OCXUserGroupMgr.h"
#include "OCXLogMgrXMLProcess.h"
#include "OCXPlayBackXMLProcess.h"
#include "OCXLoginXMLProcess.h"
#include "OCXDeviceGroupMgrXMLProcess.h"
#include "OCXAlarmMgr.h"
#include "OCXDeviceMgr.h"
#include "OCXDeviceGroupMgr.h"
#include "OCXIntelligenceAnalysisMgr.h"
#include "OCXCapturePictureXMLProcess.h"
#include "OCXAlarmLinkageMgr.h"
#include "OCXLogMgr.h"
#include "PostEventXML.h"
#include "LocalConfig.h"
#include "OCXTVWallMgr.h"
#include "OCXRecordMgr.h"
#include "OCXCapturePicture.h"
#include "OCXClientNetworkMgr.h"
#include "OCXReportMgr.h"
#include "OCXMotionDetecMgrXMLProcess.h"
#include "OCXUpgradeMgr.h"
#include "OCXDrawXMLProcess.h"
#include "OCXIAXMLProcess.h"
#include "OCXTVWallXMLProcess.h"
#include "TVWallSyncPlayBackMgr.h"
#include "OCXPtzControlXMLProcess.h"
#include "OCXCapturePicture.h"
#include "GlobalPlayMgr.h"
#include "GlobalIAMessageMgr.h"
#include "SyncPlayBackMgr.h"

#include "IVS_Trace.h"
#include "SDKConfig.h"
#include "OCXRealPlayXMLProcess.h"
#include "LockGuard.h"
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

// 系统生成
IMPLEMENT_DYNCREATE(CIVS_OCXPlayerCtrl, COleControl)

// Message map
BEGIN_MESSAGE_MAP(CIVS_OCXPlayerCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_WM_KEYUP()
    ON_MESSAGE(WM_EXCEPTION_CALLBACK, &CIVS_OCXPlayerCtrl::OnExceptionCallback)
    ON_MESSAGE(WM_ALARM_REPORT_CALLBACK, &CIVS_OCXPlayerCtrl::OnAlarmReportCallBack)
	ON_MESSAGE(WM_BROADCAST_REPORT_CALLBACK, &CIVS_OCXPlayerCtrl::OnBroadcastFailedCallBack)   
    ON_MESSAGE(WM_BROADCAST_REPORT_CALLBACK, &CIVS_OCXPlayerCtrl::OnFileBroadcastReportCallBack)   
    ON_MESSAGE(WM_TALKBACK_REPORT_CALLBACK, &CIVS_OCXPlayerCtrl::OnTalkbackFailedCallBack)

    ON_MESSAGE(WM_ALARM_REPORT_CALLBACK, &CIVS_OCXPlayerCtrl::OnAlarmStatusReportCallBack)
    ON_MESSAGE(WM_ALARM_LINKAGE_CALLBACK, &CIVS_OCXPlayerCtrl::OnAlarmLinkageCallBack)
    ON_MESSAGE(WM_COMM_EVENT_CALLBACK, &CIVS_OCXPlayerCtrl::OnCommEventCallback)
    ON_WM_MOVE()
	ON_MESSAGE(WM_EVENT_LOCAL_PLAYBACK_CALLBACK, &CIVS_OCXPlayerCtrl::OnLocalPlayBackEvent)
    ON_MESSAGE(WM_EVENT_REMOTE_PLAYBACK_CALLBACK, &CIVS_OCXPlayerCtrl::OnRemotePlayBackEvent)
    ON_MESSAGE(WM_USER_OFFLINE_CALLBACK, &CIVS_OCXPlayerCtrl::OnUserOfflineCallBack)
	ON_MESSAGE(WM_MANUAL_REPORT_STATE_CALLBACK, &CIVS_OCXPlayerCtrl::OnManualRecordStateCallBack)
	ON_MESSAGE(WM_RESET_FULL_SCREEN,&CIVS_OCXPlayerCtrl::OnResetFullScreenMsg)
	ON_MESSAGE(WM_RESET_WNDLAYOUT,&CIVS_OCXPlayerCtrl::OnResetSetWndLayoutMsg)
    ON_WM_WINDOWPOSCHANGED()
    ON_MESSAGE(WM_TVWALL_CALLBACK, &CIVS_OCXPlayerCtrl::OnTVWallCallback)
    ON_MESSAGE(WM_TVWALL_START_REALPLAY_TVWALL_END, &CIVS_OCXPlayerCtrl::OnStartTVWallRealPlayEnd)
    ON_MESSAGE(WM_TVWALL_STOP_REALPLAY_TVWALL_END, &CIVS_OCXPlayerCtrl::OnStopTVWallRealPlayEnd)
    ON_MESSAGE(WM_TVWALL_NOTIFY_START_RES, &CIVS_OCXPlayerCtrl::OnNotifyTVWallStartRes)
END_MESSAGE_MAP()

//lint -e786 这部分代码由MS自动产生
// Dispatch map
BEGIN_DISPATCH_MAP(CIVS_OCXPlayerCtrl, COleControl)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_NormalScreenDisplay", dispidIVS_OCX_NormalScreenDisplay, IVS_OCX_NormalScreenDisplay, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_FullScreenDisplay", dispidIVS_OCX_FullScreenDisplay, IVS_OCX_FullScreenDisplay, VT_UI4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetDeviceName", dispidIVS_OCX_SetDeviceName, IVS_OCX_SetDeviceName, VT_I4, VTS_UI4 VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_PlaySound", dispidIVS_OCX_PlaySound, IVS_OCX_PlaySound, VT_UI4, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopSound", dispidIVS_OCX_StopSound, IVS_OCX_StopSound, VT_UI4, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetTitlebar", dispidIVS_OCX_SetTitlebar, IVS_OCX_SetTitlebar, VT_I4, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetMouseWnd",         dispidIVS_OCX_GetMouseWnd, IVS_OCX_GetMouseWnd, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetVersion",          dispidIVS_OCX_GetVersion, IVS_OCX_GetVersion, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_Init", dispidIVS_OCX_Init, IVS_OCX_Init, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_Cleanup", dispidIVS_OCX_Cleanup, IVS_OCX_Cleanup, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopAllRealPlay", dispidIVS_OCX_StopAllRealPlay, IVS_OCX_StopAllRealPlay, VT_UI4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetWndLayout",        dispidIVS_OCX_SetWndLayout, IVS_OCX_SetWndLayout, VT_I4, VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetColor", dispidIVS_OCX_SetColor, IVS_OCX_SetColor, VT_UI4, VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_Login", dispidIVS_OCX_Login, IVS_OCX_Login, VT_I4, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_WinUserLogin", dispidIVS_OCX_WinUserLogin, IVS_OCX_WinUserLogin, VT_I4, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_UI4 VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartVoiceTalkback", dispidIVS_OCX_StartVoiceTalkback, IVS_OCX_StartVoiceTalkback, VT_BSTR, VTS_BSTR VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopVoiceTalkback", dispidIVS_OCX_StopVoiceTalkback, IVS_OCX_StopVoiceTalkback, VT_I4, VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetTalkbackVolume", dispidIVS_OCX_SetTalkbackVolume, IVS_OCX_SetTalkbackVolume, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetTalkbackVolume", dispidIVS_OCX_GetTalkbackVolume, IVS_OCX_GetTalkbackVolume, VT_UI4, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetCameraAlias", dispidIVS_OCX_SetCameraAlias, IVS_OCX_SetCameraAlias, VT_UI4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCameraAlias", dispidIVS_OCX_GetCameraAlias, IVS_OCX_GetCameraAlias, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAllCameraAlias", dispidIVS_OCX_GetAllCameraAlias, IVS_OCX_GetAllCameraAlias, VT_BSTR, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartRealPlay", dispidIVS_OCX_StartRealPlay, IVS_OCX_StartRealPlay, VT_I4, VTS_BSTR VTS_BSTR VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopRealPlay", dispidIVS_OCX_StopRealPlay, IVS_OCX_StopRealPlay, VT_I4, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetFreeWnd", dispidIVS_OCX_GetFreeWnd, IVS_OCX_GetFreeWnd, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetToolbar", dispidIVS_OCX_SetToolbar, IVS_OCX_SetToolbar, VT_I4, VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddUser", dispidIVS_OCX_AddUser, IVS_OCX_AddUser, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteUser", dispidIVS_OCX_DeleteUser, IVS_OCX_DeleteUser, VT_I4, VTS_BSTR VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetUserList", dispidIVS_OCX_GetUserList, IVS_OCX_GetUserList, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_Logout", dispidIVS_OCX_Logout, IVS_OCX_Logout, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetLocalAlarmConfig", dispidIVS_OCX_SetLocalAlarmConfig, IVS_OCX_SetLocalAlarmConfig, VT_UI4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetLocalAlarmConfig", dispidIVS_OCX_GetLocalAlarmConfig, IVS_OCX_GetLocalAlarmConfig, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetLocalRecordConfig", dispidIVS_OCX_SetLocalRecordConfig, IVS_OCX_SetLocalRecordConfig, VT_UI4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetLocalRecordConfig", dispidIVS_OCX_GetLocalRecordConfig, IVS_OCX_GetLocalRecordConfig, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetLocalCaptureConfig", dispidIVS_OCX_SetLocalCaptureConfig, IVS_OCX_SetLocalCaptureConfig, VT_UI4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetLocalCaptureConfig", dispidIVS_OCX_GetLocalCaptureConfig, IVS_OCX_GetLocalCaptureConfig, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetLocalNetWorkConfig", dispidIVS_OCX_SetLocalNetWorkConfig, IVS_OCX_SetLocalNetWorkConfig, VT_UI4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetLocalNetWorkConfig", dispidIVS_OCX_GetLocalNetWorkConfig, IVS_OCX_GetLocalNetWorkConfig, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetLocalPTZConfig", dispidIVS_OCX_SetLocalPTZConfig, IVS_OCX_SetLocalPTZConfig, VT_UI4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetLocalPTZConcig", dispidIVS_OCX_GetLocalPTZConfig, IVS_OCX_GetLocalPTZConfig, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetLocalCameraLinkConfig", dispidIVS_OCX_SetLocalCameraLinkConfig, IVS_OCX_SetLocalCameraLinkConfig, VT_UI4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetLocalCameraLinkConfig", dispidIVS_OCX_GetLocalCameraLinkConfig, IVS_OCX_GetLocalCameraLinkConfig, VT_UI4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelLocalCameraLinkConfig", dispidIVS_OCX_DelLocalCameraLinkConfig, IVS_OCX_DelLocalCameraLinkConfig, VT_UI4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetUserGroupList", dispidIVS_OCX_GetUserGroupList, IVS_OCX_GetUserGroupList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetUserInfo", dispidIVS_OCX_GetUserInfo, IVS_OCX_GetUserInfo, VT_BSTR, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ChangePWD", dispidIVS_OCX_ChangePWD, IVS_OCX_ChangePWD, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ResetPWD", dispidIVS_OCX_ResetPWD, IVS_OCX_ResetPWD, VT_I4, VTS_BSTR VTS_UI4 VTS_BSTR)
  	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddRecordPlan", dispidIVS_OCX_AddRecordPlan, IVS_OCX_AddRecordPlan, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyRecordPlan", dispidIVS_OCX_ModifyRecordPlan, IVS_OCX_ModifyRecordPlan, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteRecordPlan", dispidIVS_OCX_DeleteRecordPlan, IVS_OCX_DeleteRecordPlan, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRecordPlan", dispidIVS_OCX_GetRecordPlan, IVS_OCX_GetRecordPlan, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddDevice", dispidIVS_OCX_AddDevice, IVS_OCX_AddDevice, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_QueryDeviceList", dispidIVS_OCX_QueryDeviceList, IVS_OCX_QueryDeviceList, VT_BSTR, VTS_UI4 VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyUser", dispidIVS_OCX_ModifyUser, IVS_OCX_ModifyUser, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddKeyword", dispidIVS_OCX_AddKeyword, IVS_OCX_AddKeyword, VT_UI4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelKeyword", dispidIVS_OCX_DelKeyword, IVS_OCX_DelKeyword, VT_UI4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetKeyword", dispidIVS_OCX_GetKeyword, IVS_OCX_GetKeyword, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddUserGroup", dispidIVS_OCX_AddUserGroup, IVS_OCX_AddUserGroup, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteUserGroup", dispidIVS_OCX_DeleteUserGroup, IVS_OCX_DeleteUserGroup, VT_I4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyUserGroup", dispidIVS_OCX_ModifyUserGroup, IVS_OCX_ModifyUserGroup, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_MoveUserGroup", dispidIVS_OCX_MoveUserGroup, IVS_OCX_MoveUserGroup, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddRole", dispidIVS_OCX_AddRole, IVS_OCX_AddRole, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteRole", dispidIVS_OCX_DeleteRole, IVS_OCX_DeleteRole, VT_I4, VTS_BSTR VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyRole", dispidIVS_OCX_ModifyRole, IVS_OCX_ModifyRole, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRoleList", dispidIVS_OCX_GetRoleList, IVS_OCX_GetRoleList, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetOnlineUser", dispidIVS_OCX_GetOnlineUser, IVS_OCX_GetOnlineUser, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetOperationLog", dispidIVS_OCX_GetOperationLog, IVS_OCX_GetOperationLog, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddAlarmLevel", dispidIVS_OCX_AddAlarmLevel, IVS_OCX_AddAlarmLevel, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyAlarmLevel", dispidIVS_OCX_ModifyAlarmLevel, IVS_OCX_ModifyAlarmLevel, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteAlarmLevel", dispidIVS_OCX_DeleteAlarmLevel, IVS_OCX_DeleteAlarmLevel, VT_I4, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetRecordSpace", dispidIVS_OCX_SetRecordSpace, IVS_OCX_SetRecordSpace, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteRecordSpace", dispidIVS_OCX_DeleteRecordSpace, IVS_OCX_DeleteRecordSpace, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyRecordSpace", dispidIVS_OCX_ModifyRecordSpace, IVS_OCX_ModifyRecordSpace, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRecordSpace", dispidIVS_OCX_GetRecordSpace, IVS_OCX_GetRecordSpace, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddHelpInfo", dispidIVS_OCX_AddHelpInfo, IVS_OCX_AddHelpInfo, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyHelpInfo", dispidIVS_OCX_ModifyHelpInfo, IVS_OCX_ModifyHelpInfo, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetUserDeviceList", dispidIVS_OCX_SetUserDeviceList, IVS_OCX_SetUserDeviceList, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetUserDeviceList", dispidIVS_OCX_GetUserDeviceList, IVS_OCX_GetUserDeviceList, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SignOutOnlineUser", dispidIVS_OCX_SignOutOnlineUser, IVS_OCX_SignOutOnlineUser, VT_I4, VTS_BSTR VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetCustomWndLayout", dispidIVS_OCX_SetCustomWndLayout, IVS_OCX_SetCustomWndLayout, VT_I4, VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCustomWndLayout", dispidIVS_OCX_GetCustomWndLayout, IVS_OCX_GetCustomWndLayout, VT_BSTR, VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetGPUDecodeAbility", dispidIVS_OCX_GetGPUDecodeAbility, IVS_OCX_GetGPUDecodeAbility, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetNVRList", dispidIVS_OCX_GetNVRList, IVS_OCX_GetNVRList, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDeviceGroup", dispidIVS_OCX_GetDeviceGroup, IVS_OCX_GetDeviceGroup, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDeviceInfo", dispidIVS_OCX_GetDeviceInfo, IVS_OCX_GetDeviceInfo, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteDevice", dispidIVS_OCX_DeleteDevice, IVS_OCX_DeleteDevice, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddRecordPlan", dispidIVS_OCX_AddRecordPlan, IVS_OCX_AddRecordPlan, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyDeviceGroupName", dispidIVS_OCX_ModifyDeviceGroupName, IVS_OCX_ModifyDeviceGroupName, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_MoveDeviceGroup", dispidIVS_OCX_MoveDeviceGroup, IVS_OCX_MoveDeviceGroup, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteDeviceGroup", dispidIVS_OCX_DeleteDeviceGroup, IVS_OCX_DeleteDeviceGroup, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteDeviceFromGroup", dispidIVS_OCX_DeleteDeviceFromGroup, IVS_OCX_DeleteDeviceFromGroup, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAlarmLevelList", dispidIVS_OCX_GetAlarmLevelList, IVS_OCX_GetAlarmLevelList, VT_BSTR, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetDeviceConfig", dispidIVS_OCX_SetDeviceConfig, IVS_OCX_SetDeviceConfig, VT_I4,VTS_I4 VTS_BSTR VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDeviceConfig", dispidIVS_OCX_GetDeviceConfig, IVS_OCX_GetDeviceConfig, VT_BSTR,VTS_I4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetToolbarButtonStatus", dispidIVS_OCX_SetToolbarButtonStatus, IVS_OCX_SetToolbarButtonStatus, VT_I4, VTS_I4 VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ShowTitlebar", dispidIVS_OCX_ShowTitlebar, IVS_OCX_ShowTitlebar, VT_I4, VTS_BOOL)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ShowToolbar", dispidIVS_OCX_ShowToolbar, IVS_OCX_ShowToolbar, VT_I4, VTS_BOOL)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddDeviceGroup", dispidIVS_OCX_AddDeviceGroup, IVS_OCX_AddDeviceGroup, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAlarmTypeList", dispidIVS_OCX_GetAlarmTypeList, IVS_OCX_GetAlarmTypeList, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_Aes256CbcEncode", dispidIVS_OCX_Aes256CbcEncode, IVS_OCX_Aes256CbcEncode, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_Aes256CbcDecode", dispidIVS_OCX_Aes256CbcDecode, IVS_OCX_Aes256CbcDecode, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAlarmLevel", dispidIVS_OCX_GetAlarmLevel, IVS_OCX_GetAlarmLevel, VT_BSTR, VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AllocAreaGuardPlan", dispidIVS_OCX_AllocAreaGuardPlan, IVS_OCX_AllocAreaGuardPlan, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteHelpInfo", dispidIVS_OCX_DeleteHelpInfo, IVS_OCX_DeleteHelpInfo, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_QueryHelpInfo", dispidIVS_OCX_QueryHelpInfo, IVS_OCX_QueryHelpInfo, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetMediaInfo", dispidIVS_OCX_GetMediaInfo, IVS_OCX_GetMediaInfo, VT_BSTR, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetAlarmTypeLevel", dispidIVS_OCX_SetAlarmTypeLevel, IVS_OCX_SetAlarmTypeLevel, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetDisplayScale", dispidIVS_OCX_SetDisplayScale, IVS_OCX_SetDisplayScale, VT_I4, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddFavoriteDevice", dispidIVS_OCX_AddFavoriteDevice, IVS_OCX_AddFavoriteDevice, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelFavoriteDevice", dispidIVS_OCX_DelFavoriteDevice, IVS_OCX_DelFavoriteDevice, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_MoveFavoriteDevice", dispidIVS_OCX_MoveFavoriteDevice, IVS_OCX_MoveFavoriteDevice, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddFavoriteFolder", dispidIVS_OCX_AddFavoriteFolder, IVS_OCX_AddFavoriteFolder, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelFavoriteFolder", dispidIVS_OCX_DelFavoriteFolder, IVS_OCX_DelFavoriteFolder, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModFavoriteFolder", dispidIVS_OCX_ModFavoriteFolder, IVS_OCX_ModFavoriteFolder, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_MoveFavoriteFolder", dispidIVS_OCX_MoveFavoriteFolder, IVS_OCX_MoveFavoriteFolder, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetFavoriteDevice", dispidIVS_OCX_GetFavoriteDevice, IVS_OCX_GetFavoriteDevice, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SubscribeAlarm", dispidIVS_OCX_SubscribeAlarm, IVS_OCX_SubscribeAlarm, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SubscribeAlarmQuery", dispidIVS_OCX_SubscribeAlarmQuery, IVS_OCX_SubscribeAlarmQuery, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDomainRoute", dispidIVS_OCX_GetDomainRoute, IVS_OCX_GetDomainRoute, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyAlarmArea", dispidIVS_OCX_ModifyAlarmArea, IVS_OCX_ModifyAlarmArea, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteAlarmArea", dispidIVS_OCX_DeleteAlarmArea, IVS_OCX_DeleteAlarmArea, VT_I4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAlarmAreaList", dispidIVS_OCX_GetAlarmAreaList, IVS_OCX_GetAlarmAreaList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyAreaGuardPlan", dispidIVS_OCX_ModifyAreaGuardPlan, IVS_OCX_ModifyAreaGuardPlan, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteAreaGuardPlan", dispidIVS_OCX_DeleteAreaGuardPlan, IVS_OCX_DeleteAreaGuardPlan, VT_I4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAreaGuardPlan", dispidIVS_OCX_GetAreaGuardPlan, IVS_OCX_GetAreaGuardPlan, VT_BSTR, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddAlarmArea", dispidIVS_OCX_AddAlarmArea, IVS_OCX_AddAlarmArea, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAlarmAreaInfo", dispidIVS_OCX_GetAlarmAreaInfo, IVS_OCX_GetAlarmAreaInfo, VT_BSTR, VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_UserAuthentication", dispidIVS_OCX_UserAuthentication, IVS_OCX_UserAuthentication, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAlarmLinkage", dispidIVS_OCX_GetAlarmLinkage, IVS_OCX_GetAlarmLinkage, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAlarmLinkageList", dispidIVS_OCX_GetAlarmLinkageList, IVS_OCX_GetAlarmLinkageList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddAlarmLinkage", dispidIVS_OCX_AddAlarmLinkage, IVS_OCX_AddAlarmLinkage, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyAlarmLinkage", dispidIVS_OCX_ModifyAlarmLinkage, IVS_OCX_ModifyAlarmLinkage, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteAlarmLinkage", dispidIVS_OCX_DeleteAlarmLinkage, IVS_OCX_DeleteAlarmLinkage, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddDeviceToGroup", dispidIVS_OCX_AddDeviceToGroup, IVS_OCX_AddDeviceToGroup, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAlarmEventList", dispidIVS_OCX_GetAlarmEventList, IVS_OCX_GetAlarmEventList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartAlarmAreaGuard", dispidIVS_OCX_StartAlarmAreaGuard, IVS_OCX_StartAlarmAreaGuard, VT_I4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopAlarmAreaGuard", dispidIVS_OCX_StopAlarmAreaGuard, IVS_OCX_StopAlarmAreaGuard, VT_I4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetActiveWnd", dispidIVS_OCX_SetActiveWnd, IVS_OCX_SetActiveWnd, VT_UI4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetSelectWnd", dispidIVS_OCX_GetSelectWnd, IVS_OCX_GetSelectWnd, VT_UI4, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAlarmEventInfo", dispidIVS_OCX_GetAlarmEventInfo,IVS_OCX_GetAlarmEventInfo, VT_BSTR, VTS_I4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_QueryRecordList", dispidIVS_OCX_QueryRecordList, IVS_OCX_QueryRecordList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRecordList", dispidIVS_OCX_GetRecordList, IVS_OCX_GetRecordList, VT_BSTR, VTS_BSTR VTS_UI4 VTS_BSTR VTS_BSTR VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_IA", dispidIVS_OCX_IA, IVS_OCX_IA, VT_BSTR, VTS_UI4 VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRoleInfoByRoleID", dispidIVS_OCX_GetRoleInfoByRoleID, IVS_OCX_GetRoleInfoByRoleID, VT_BSTR, VTS_BSTR VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRoleInfoByUserID", dispidIVS_OCX_GetRoleInfoByUserID, IVS_OCX_GetRoleInfoByUserID, VT_BSTR, VTS_BSTR VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDefaultRoleAccountRule", dispidIVS_OCX_GetDefaultRoleAccountRule, IVS_OCX_GetDefaultRoleAccountRule, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetOperationRightGroup", dispidIVS_OCX_GetOperationRightGroup, IVS_OCX_GetOperationRightGroup, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_CheckPWDByRole", dispidIVS_OCX_CheckPWDByRole, IVS_OCX_CheckPWDByRole, VT_I4, VTS_BSTR VTS_UI4 VTS_BSTR VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAutoCreatePWD", dispidIVS_OCX_GetAutoCreatePWD, IVS_OCX_GetAutoCreatePWD, VT_BSTR, VTS_BSTR VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AlarmCommission", dispidIVS_OCX_AlarmCommission, IVS_OCX_AlarmCommission, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCameraByWnd", dispidIVS_OCX_GetCameraByWnd, IVS_OCX_GetCameraByWnd, VT_BSTR, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetPlayQualityMode", dispidIVS_OCX_SetPlayQualityMode, IVS_OCX_SetPlayQualityMode, VT_EMPTY, VTS_UI4 VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRecordPolicyByTime", dispidIVS_OCX_GetRecordPolicy, IVS_OCX_GetRecordPolicyByTime, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AlarmReport", dispidIVS_OCX_AlarmReport, IVS_OCX_AlarmReport, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AlarmConfirm", dispidIVS_OCX_AlarmConfirm, IVS_OCX_AlarmConfirm, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AlarmCancel", dispidIVS_OCX_AlarmCancel, IVS_OCX_AlarmCancel, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_PtzControl", dispidIVS_OCX_PtzControl, IVS_OCX_PtzControl, VT_BSTR, VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetChannelList", dispidIVS_OCX_GetChannelList, IVS_OCX_GetChannelList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRecordStatus", dispidIVS_OCX_GetRecordStatus, IVS_OCX_GetRecordStatus, VT_BSTR, VTS_BSTR VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_EnableGPU", dispidIVS_OCX_EnableGPU, IVS_OCX_EnableGPU, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetRecordPolicyByTime", dispidIVS_OCX_SetRecordPolicyByTime, IVS_OCX_SetRecordPolicyByTime, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetUserID", dispidIVS_OCX_GetUserID, IVS_OCX_GetUserID, VT_BSTR, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_BackupUserData", dispidIVS_OCX_BackupUserData, IVS_OCX_BackupUserData, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_RestoreUserData", dispidIVS_OCX_RestoreUserData, IVS_OCX_RestoreUserData, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_InitTVWall", dispidIVS_OCX_InitTVWall, IVS_OCX_InitTVWall, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_CleanupTVWall", dispidIVS_OCX_CleanupTVWall, IVS_OCX_CleanupTVWall, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetIAToolbar", dispidIVS_OCX_SetIAToolbar, IVS_OCX_SetIAToolbar, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartPlatformPlayBack", dispidIVS_OCX_StartPlatformPlayBack, IVS_OCX_StartPlatformPlayBack, VT_I4, VTS_BSTR VTS_BSTR VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopPlatformPlayBack", dispidIVS_OCX_StopPlatformPlayBack, IVS_OCX_StopPlatformPlayBack, VT_I4, VTS_I4)
    
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartDisasterRecoveryPlayBack", dispidIVS_OCX_StartDisasterRecoveryPlayBack, IVS_OCX_StartDisasterRecoveryPlayBack, VT_I4, VTS_BSTR VTS_BSTR VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopDisasterRecoveryPlayBack", dispidIVS_OCX_StopDisasterRecoveryPlayBack, IVS_OCX_StopDisasterRecoveryPlayBack, VT_I4, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_PlayBackPause", dispidIVS_OCX_PlayBackPause, IVS_OCX_PlayBackPause, VT_I4, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_PlayBackResume", dispidIVS_OCX_PlayBackResume, IVS_OCX_PlayBackResume, VT_I4, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_PlaybackFrameStepForward", dispidIVS_OCX_PlaybackFrameStepForward, IVS_OCX_PlaybackFrameStepForward, VT_I4, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetPlayBackSpeed", dispidIVS_OCX_SetPlayBackSpeed, IVS_OCX_SetPlayBackSpeed, VT_I4, VTS_I4 VTS_R4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetPlayBackSpeed", dispidIVS_OCX_GetPlayBackSpeed, IVS_OCX_GetPlayBackSpeed, VT_BSTR, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetPlayBackTime", dispidIVS_OCX_GetPlayBackTime, IVS_OCX_GetPlayBackTime, VT_BSTR, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetPlayBackStatus", dispidIVS_OCX_GetPlayBackStatus, IVS_OCX_GetPlayBackStatus, VT_BSTR, VTS_I4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetPlayBackTime", dispidIVS_OCX_SetPlayBackTime, IVS_OCX_SetPlayBackTime, VT_I4, VTS_I4 VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartLocalRecord", dispidIVS_OCX_StartLocalRecord, IVS_OCX_StartLocalRecord, VT_UI4, VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopLocalRecord", dispidIVS_OCX_StopLocalRecord, IVS_OCX_StopLocalRecord, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetTVWallList", dispidIVS_OCX_GetTVWallList, IVS_OCX_GetTVWallList, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddTVWall", dispidIVS_OCX_AddTVWall, IVS_OCX_AddTVWall, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyTVWallInfo", dispidIVS_OCX_ModifyTVWallInfo, IVS_OCX_ModifyTVWallInfo, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyTVWallLayout", dispidIVS_OCX_ModifyTVWallLayout, IVS_OCX_ModifyTVWallLayout, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteTVWall", dispidIVS_OCX_DeleteTVWall, IVS_OCX_DeleteTVWall, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_PopupFloderDialog", dispidIVS_OCX_PopupFloderDialog, IVS_OCX_PopupFloderDialog, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartRealPlayTVWall", dispidIVS_OCX_StartRealPlayTVWall, IVS_OCX_StartRealPlayTVWall, VT_I4, VTS_UI4 VTS_BSTR VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopRealPlayTVWall", dispidIVS_OCX_StopRealPlayTVWall, IVS_OCX_StopRealPlayTVWall, VT_I4, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartPlatformPlayBackTVWall", dispidIVS_OCX_StartPlatformPlayBackTVWall, IVS_OCX_StartPlatformPlayBackTVWall, VT_I4, VTS_UI4 VTS_BSTR VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopPlatformPlayBackTVWall", dispidIVS_OCX_StopPlatformPlayBackTVWall, IVS_OCX_StopPlatformPlayBackTVWall, VT_I4, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDeviceList", dispidIVS_OCX_GetDeviceList, IVS_OCX_GetDeviceList, VT_BSTR, VTS_UI4 VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDeviceUserList", dispidIVS_OCX_GetDeviceUserList, IVS_OCX_GetDeviceUserList, VT_BSTR, VTS_BSTR VTS_BSTR VTS_UI4 VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_EnableDraw", dispidIVS_OCX_EnableDraw, IVS_OCX_EnableDraw, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DisableDraw", dispidIVS_OCX_DisableDraw, IVS_OCX_DisableDraw, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetGraphics", dispidIVS_OCX_GetGraphics, IVS_OCX_GetGraphics, VT_BSTR, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetGraphicsByGroup", dispidIVS_OCX_GetGraphicsByGroup, IVS_OCX_GetGraphicsByGroup, VT_BSTR, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetRelationShip", dispidIVS_OCX_SetRelationShip, IVS_OCX_SetRelationShip, VT_I4, VTS_UI4 VTS_UI4 VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetGraphics", dispidIVS_OCX_SetGraphics, IVS_OCX_SetGraphics, VT_I4, VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopDrawGraphics", dispidIVS_OCX_StopDrawGraphics, IVS_OCX_StopDrawGraphics, VT_I4, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteGraphicsSelected", dispidIVS_OCX_DeleteGraphicsSelected, IVS_OCX_DeleteGraphicsSelected, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteGraphicsByGroup", dispidIVS_OCX_DeleteGraphicsByGroup, IVS_OCX_DeleteGraphicsByGroup, VT_I4, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteGraphics", dispidIVS_OCX_DeleteGraphics, IVS_OCX_DeleteGraphics, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SelectGraphicByGroup", dispidIVS_OCX_SelectGraphicByGroup, IVS_OCX_SelectGraphicByGroup, VT_I4, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SelectCancel", dispidIVS_OCX_SelectCancel, IVS_OCX_SelectCancel, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetTVWallByID", dispidIVS_OCX_GetTVWallByID, IVS_OCX_GetTVWallByID, VT_BSTR, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DrawGraphics", dispidIVS_OCX_DrawGraphics, IVS_OCX_DrawGraphics, VT_I4, VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetRecordSearchInfo", dispidIVS_OCX_SetRecordSearchInfo, IVS_OCX_SetRecordSearchInfo, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRecordSearchInfo", dispidIVS_OCX_GetRecordSearchInfo, IVS_OCX_GetRecordSearchInfo, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetTempScenario", dispidIVS_OCX_SetTempScenario, IVS_OCX_SetTempScenario, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetTempScenario", dispidIVS_OCX_GetTempScenario, IVS_OCX_GetTempScenario, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddScenarioFavorite", dispidIVS_OCX_AddScenarioFavorite, IVS_OCX_AddScenarioFavorite, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModScenarioFavorite", dispidIVS_OCX_ModScenarioFavorite, IVS_OCX_ModScenarioFavorite, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelScenarioFavorite", dispidIVS_OCX_DelScenarioFavorite, IVS_OCX_DelScenarioFavorite, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetScenarioFavoriteList", dispidIVS_OCX_GetScenarioFavoriteList, IVS_OCX_GetScenarioFavoriteList, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetScenarioFavorite", dispidIVS_OCX_GetScenarioFavorite, IVS_OCX_GetScenarioFavorite, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_Set3DPTZ", dispidIVS_OCX_Set3DPTZ, IVS_OCX_Set3DPTZ, VT_UI4, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetMainDeviceName", dispidIVS_OCX_SetMainDeviceName, IVS_OCX_SetMainDeviceName, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetCameraName", dispidIVS_OCX_SetCameraName, IVS_OCX_SetCameraName, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetTVWallGroupTurnInspect", dispidIVS_OCX_GetTVWallGroupTurnInspect, IVS_OCX_GetTVWallGroupTurnInspect, VT_BSTR, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddTVWallGroupTurnInspect", dispidIVS_OCX_AddTVWallGroupTurnInspect, IVS_OCX_AddTVWallGroupTurnInspect, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyTVWallGroupTurnInspect", dispidIVS_OCX_ModifyTVWallGroupTurnInspect, IVS_OCX_ModifyTVWallGroupTurnInspect, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteTVWallGroupTurnInspect", dispidIVS_OCX_DeleteTVWallGroupTurnInspect, IVS_OCX_DeleteTVWallGroupTurnInspect, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetTVWallTurnInspect", dispidIVS_OCX_GetTVWallTurnInspect, IVS_OCX_GetTVWallTurnInspect, VT_BSTR, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddTVWallTurnInspect", dispidIVS_OCX_AddTVWallTurnInspect, IVS_OCX_AddTVWallTurnInspect, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyTVWallTurnInspect", dispidIVS_OCX_ModifyTVWallTurnInspect, IVS_OCX_ModifyTVWallTurnInspect, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteTVWallTurnInspect", dispidIVS_OCX_DeleteTVWallTurnInspect, IVS_OCX_DeleteTVWallTurnInspect, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetTurnInspectPlan", dispidIVS_OCX_SetTurnInspectPlan, IVS_OCX_SetTurnInspectPlan, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyTurnInspectPlan", dispidIVS_OCX_ModifyTurnInspectPlan, IVS_OCX_ModifyTurnInspectPlan, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_RebootDevice", dispidIVS_OCX_RebootDevice, IVS_OCX_RebootDevice, VT_I4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetConfigInfo", dispidIVS_OCX_GetConfigInfo, IVS_OCX_GetConfigInfo, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetGraphicsByPercent", dispidIVS_OCX_GetGraphicsByPercent, IVS_OCX_GetGraphicsByPercent, VT_BSTR, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetGraphicsByPercent", dispidIVS_OCX_SetGraphicsByPercent, IVS_OCX_SetGraphicsByPercent, VT_I4, VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddBookmark", dispidIVS_OCX_AddBookmark, IVS_OCX_AddBookmark, VT_BSTR,VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetBookmarkList", dispidIVS_OCX_GetBookmarkList, IVS_OCX_GetBookmarkList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyBookmark", dispidIVS_OCX_ModifyBookmark, IVS_OCX_ModifyBookmark, VT_I4,VTS_BSTR VTS_BSTR VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteBookmark", dispidIVS_OCX_DeleteBookmark, IVS_OCX_DeleteBookmark, VT_I4,VTS_BSTR VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddPTZPreset", dispidIVS_OCX_AddPTZPreset, IVS_OCX_AddPTZPreset, VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelPTZPreset", dispidIVS_OCX_DelPTZPreset, IVS_OCX_DelPTZPreset, VT_I4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModPTZPreset", dispidIVS_OCX_ModPTZPreset, IVS_OCX_ModPTZPreset, VT_I4, VTS_BSTR VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetPTZPresetList", dispidIVS_OCX_GetPTZPresetList, IVS_OCX_GetPTZPresetList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetGuardPos", dispidIVS_OCX_SetGuardPos, IVS_OCX_SetGuardPos, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetGuardPos", dispidIVS_OCX_GetGuardPos, IVS_OCX_GetGuardPos, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddCruiseTrack", dispidIVS_OCX_AddCruiseTrack, IVS_OCX_AddCruiseTrack, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelCruiseTrack", dispidIVS_OCX_DelCruiseTrack, IVS_OCX_DelCruiseTrack, VT_I4, VTS_BSTR VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModCruiseTrack", dispidIVS_OCX_ModCruiseTrack, IVS_OCX_ModCruiseTrack, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCruiseTrackList", dispidIVS_OCX_GetCruiseTrackList, IVS_OCX_GetCruiseTrackList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCruiseTrack", dispidIVS_OCX_GetCruiseTrack, IVS_OCX_GetCruiseTrack, VT_BSTR, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetCruisePlan", dispidIVS_OCX_SetCruisePlan, IVS_OCX_SetCruisePlan, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCruisePlan", dispidIVS_OCX_GetCruisePlan, IVS_OCX_GetCruisePlan, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddCameraSwitch", dispidIVS_OCX_AddCameraSwitch, IVS_OCX_AddCameraSwitch, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelCameraSwitch", dispidIVS_OCX_DelCameraSwitch, IVS_OCX_DelCameraSwitch, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCameraSwitchList", dispidIVS_OCX_GetCameraSwitchList, IVS_OCX_GetCameraSwitchList, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCameraSwitchInfo", dispidIVS_OCX_GetCameraSwitchInfo, IVS_OCX_GetCameraSwitchInfo, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_LockUser", dispidIVS_OCX_LockUser, IVS_OCX_LockUser, VT_I4, VTS_BSTR VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_LocalSnapshot", dispidIVS_OCX_LocalSnapshot, IVS_OCX_LocalSnapshot, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_PlatformSnapshot", dispidIVS_OCX_PlatformSnapshot, IVS_OCX_PlatformSnapshot, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetSnapshotList", dispidIVS_OCX_GetSnapshotList, IVS_OCX_GetSnapshotList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DownloadSnapshot", dispidIVS_OCX_DownloadSnapshot, IVS_OCX_DownloadSnapshot, VT_I4, VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteSnapshot", dispidIVS_OCX_DeleteSnapshot, IVS_OCX_DeleteSnapshot, VT_I4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetSnapshotPlan", dispidIVS_OCX_SetSnapshotPlan, IVS_OCX_SetSnapshotPlan, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetSnapshotPlan", dispidIVS_OCX_GetSnapshotPlan, IVS_OCX_GetSnapshotPlan, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetLogo", dispidIVS_OCX_GetLogo, IVS_OCX_GetLogo, VT_I4, VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_UploadLogo", dispidIVS_OCX_UploadLogo, IVS_OCX_UploadLogo, VT_I4, VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetStatement", dispidIVS_OCX_GetStatement, IVS_OCX_GetStatement, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_UploadStatement", dispidIVS_OCX_UploadStatement, IVS_OCX_UploadStatement, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetWarnState", dispidIVS_OCX_SetWarnState, IVS_OCX_SetWarnState, VT_I4,VTS_UI4 VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartPlatformRecord", dispidIVS_OCX_StartPlatformRecord, IVS_OCX_StartPlatformRecord, VT_UI4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopPlatformRecord", dispidIVS_OCX_StopPlatformRecord, IVS_OCX_StopPlatformRecord, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartPURecord", dispidIVS_OCX_StartPURecord, IVS_OCX_StartPURecord, VT_I4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartAlarmOut", dispidIVS_OCX_StartAlarmOut, IVS_OCX_StartAlarmOut, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopAlarmOut", dispidIVS_OCX_StopAlarmOut, IVS_OCX_StopAlarmOut, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopPURecord", dispidIVS_OCX_StopPURecord, IVS_OCX_StopPURecord, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetCameraBackupConfig", dispidIVS_OCX_SetCameraBackupConfig, IVS_OCX_SetCameraBackupConfig, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCameraBackupConfig", dispidIVS_OCX_GetCameraBackupConfig, IVS_OCX_GetCameraBackupConfig, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetWndType", dispidIVS_OCX_SetWndType, IVS_OCX_SetWndType, VT_I4, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteAlarmWnd", dispidIVS_OCX_DeleteAlarmWnd, IVS_OCX_DeleteAlarmWnd, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetTVWallWndLayout", dispidIVS_OCX_SetTVWallWndLayout, IVS_OCX_SetTVWallWndLayout, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetTVWallPaneStatus", dispidIVS_OCX_SetTVWallPaneStatus, IVS_OCX_SetTVWallPaneStatus, VT_I4, VTS_UI4 VTS_UI4 VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetHelpInfoText", dispidIVS_OCX_GetHelpInfoText, IVS_OCX_GetHelpInfoText, VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetLanguage", dispidIVS_OCX_SetLanguage, IVS_OCX_SetLanguage, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartSearchDevice", dispidIVS_OCX_StartSearchDevice, IVS_OCX_StartSearchDevice, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopSearchDevice", dispidIVS_OCX_StopSearchDevice, IVS_OCX_StopSearchDevice, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_VerifyDevice", dispidIVS_OCX_VerifyDevice, IVS_OCX_VerifyDevice, VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDevLog", dispidIVS_OCX_GetDevLog, IVS_OCX_GetDevLog, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDecoderList", dispidIVS_OCX_GetDecoderList, IVS_OCX_GetDecoderList, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDeviceAlarmList", dispidIVS_OCX_GetDeviceAlarmList, IVS_OCX_GetDeviceAlarmList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCPUMemory", dispidIVS_OCX_GetCPUMemory, IVS_OCX_GetCPUMemory, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddRecordBackupPlan", dispidIVS_OCX_AddRecordBackupPlan, IVS_OCX_AddRecordBackupPlan, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyRecordBackupPlan", dispidIVS_OCX_ModifyRecordBackupPlan, IVS_OCX_ModifyRecordBackupPlan, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteRecordBackupPlan", dispidIVS_OCX_DeleteRecordBackupPlan, IVS_OCX_DeleteRecordBackupPlan, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRecordBackupPlan", dispidIVS_OCX_GetRecordBackupPlan, IVS_OCX_GetRecordBackupPlan, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelSnapshotPlan", dispidIVS_OCX_DelSnapshotPlan, IVS_OCX_DelSnapshotPlan, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDomainList", dispidIVS_OCX_GetDomainList, IVS_OCX_GetDomainList, VT_BSTR, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartRealBroadcast", dispidIVS_OCX_StartRealBroadcast, IVS_OCX_StartRealBroadcast, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopRealBroadcast", dispidIVS_OCX_StopRealBroadcast, IVS_OCX_StopRealBroadcast, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartFileBroadcast", dispidIVS_OCX_StartFileBroadcast, IVS_OCX_StartFileBroadcast, VT_I4, VTS_BSTR VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopFileBroadcast", dispidIVS_OCX_StopFileBroadcast, IVS_OCX_StopFileBroadcast, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddBroadcastDevice", dispidIVS_OCX_AddBroadcastDevice, IVS_OCX_AddBroadcastDevice, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteBroadcastDevice", dispidIVS_OCX_DeleteBroadcastDevice, IVS_OCX_DeleteBroadcastDevice, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetCameraSwitchWnd", dispidIVS_OCX_SetCameraSwitchWnd, IVS_OCX_SetCameraSwitchWnd, VT_I4, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetWndType", dispidIVS_OCX_GetWndType, IVS_OCX_GetWndType, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_IALocusOverly", dispidIVS_OCX_IALocusOverly, IVS_OCX_IALocusOverly, VT_I4, VTS_UI4 VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetExDomainList", dispidIVS_OCX_GetExDomainList, IVS_OCX_GetExDomainList, VT_BSTR,VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ShareCatalog", dispidIVS_OCX_ShareCatalog, IVS_OCX_ShareCatalog, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetShareCatalogList", dispidIVS_OCX_GetShareCatalogList, IVS_OCX_GetShareCatalogList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetIACPUMemory", dispidIVS_OCX_GetIACPUMemory, IVS_OCX_GetIACPUMemory, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_PlaybackFrameStepBackward", dispidIVS_OCX_PlaybackFrameStepBackward, IVS_OCX_PlaybackFrameStepBackward, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartSyncPlay", dispidIVS_OCX_StartSyncPlay, IVS_OCX_StartSyncPlay, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopSyncPlay", dispidIVS_OCX_StopSyncPlay, IVS_OCX_StopSyncPlay, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRecordTask", dispidIVS_OCX_GetRecordTask, IVS_OCX_GetRecordTask, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_QueryPerformance", dispidIVS_OCX_QueryPerformance, IVS_OCX_QueryPerformance, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SignOutUser", dispidIVS_OCX_SignOutUser, IVS_OCX_SignOutUser, VT_I4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartPlatformDownload", dispidIVS_OCX_StartPlatformDownload, IVS_OCX_StartPlatformDownload, VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopPlatformDownload", dispidIVS_OCX_StopPlatformDownload, IVS_OCX_StopPlatformDownload, VT_I4, VTS_UI4)

    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartDisasterRecoveryDownload", dispidIVS_OCX_StartDisasterRecoveryDownload, IVS_OCX_StartDisasterRecoveryDownload, VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopDisasterRecoveryDownload", dispidIVS_OCX_StopDisasterRecoveryDownload, IVS_OCX_StopDisasterRecoveryDownload, VT_I4, VTS_UI4)

	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartPUDownload", dispidIVS_OCX_StartPUDownload, IVS_OCX_StartPUDownload, VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopPUDownload", dispidIVS_OCX_StopPUDownload, IVS_OCX_StopPUDownload, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DownloadPause", dispidIVS_OCX_DownloadPause, IVS_OCX_DownloadPause, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DownloadResume", dispidIVS_OCX_DownloadResume, IVS_OCX_DownloadResume, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDownloadInfo", dispidIVS_OCX_GetDownloadInfo, IVS_OCX_GetDownloadInfo, VT_BSTR, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartPUPlayBack", dispidIVS_OCX_StartPUPlayBack, IVS_OCX_StartPUPlayBack, VT_I4, VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopPUPlayBack", dispidIVS_OCX_StopPUPlayBack, IVS_OCX_StopPUPlayBack, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetTVWallTempScenario", dispidIVS_OCX_SetTVWallTempScenario, IVS_OCX_SetTVWallTempScenario, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetTVWallTempScenario", dispidIVS_OCX_GetTVWallTempScenario, IVS_OCX_GetTVWallTempScenario, VT_BSTR, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRecordBackupTaskList", dispidIVS_OCX_GetRecordBackupTaskList, IVS_OCX_GetRecordBackupTaskList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartRecordBackupTask", dispidIVS_OCX_StartRecordBackupTask, IVS_OCX_StartRecordBackupTask, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopRecordBackupTask", dispidIVS_OCX_StopRecordBackupTask, IVS_OCX_StopRecordBackupTask, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDriverList", dispidIVS_OCX_GetDriverList, IVS_OCX_GetDriverList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetTemplateList", dispidIVS_OCX_GetTemplateList, IVS_OCX_GetTemplateList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelRecordBackupTask", dispidIVS_OCX_DelRecordBackupTask, IVS_OCX_DelRecordBackupTask, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddRecordBackupTask", dispidIVS_OCX_AddRecordBackupTask, IVS_OCX_AddRecordBackupTask, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetEventReceiver", dispidIVS_OCX_SetEventReceiver, IVS_OCX_SetEventReceiver, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetWndDrag", dispidIVS_OCX_SetWndDrag, IVS_OCX_SetWndDrag, VT_I4, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_RefreshWnd", dispidIVS_OCX_RefreshWnd, IVS_OCX_RefreshWnd, VT_EMPTY, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAlarmLinkageAction", dispidIVS_OCX_GetAlarmLinkageAction, IVS_OCX_GetAlarmLinkageAction, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetPWDStrength", dispidIVS_OCX_GetPWDStrength, IVS_OCX_GetPWDStrength, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddCameraSwitchPlan", dispidIVS_OCX_AddCameraSwitchPlan, IVS_OCX_AddCameraSwitchPlan, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelCameraSwitchPlan", dispidIVS_OCX_DelCameraSwitchPlan, IVS_OCX_DelCameraSwitchPlan, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCameraSwitchPlanList", dispidIVS_OCX_GetCameraSwitchPlanList, IVS_OCX_GetCameraSwitchPlanList, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCameraSwitchPlan", dispidIVS_OCX_GetCameraSwitchPlan, IVS_OCX_GetCameraSwitchPlan, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_UnlockRecord", dispidIVS_OCX_UnlockRecord, IVS_OCX_UnlockRecord, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_LockRecord", dispidIVS_OCX_LockRecord, IVS_OCX_LockRecord, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetVideoNumAndResource", dispidIVS_OCX_GetVideoNumAndResource, IVS_OCX_GetVideoNumAndResource, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_UnlockBackupRecord", dispidIVS_OCX_UnlockBackupRecord, IVS_OCX_UnlockBackupRecord, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_LockBackupRecord", dispidIVS_OCX_LockBackupRecord, IVS_OCX_LockBackupRecord, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetMotionRangeDataPlatform", dispidIVS_OCX_GetMotionRangeDataPlatform, IVS_OCX_GetMotionRangeDataPlatform, VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddBookmarkForBackup", dispidIVS_OCX_AddBookmarkForBackup, IVS_OCX_AddBookmarkForBackup, VT_I4,VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetBookmarkListForBackup", dispidIVS_OCX_GetBookmarkListForBackup, IVS_OCX_GetBookmarkListForBackup, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyBookmarkForBackup", dispidIVS_OCX_ModifyBookmarkForBackup, IVS_OCX_ModifyBookmarkForBackup, VT_I4,VTS_BSTR VTS_BSTR VTS_BSTR VTS_UI4 VTS_BSTR)
 	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteBookmarkForBackup", dispidIVS_OCX_DeleteBookmarkForBackup, IVS_OCX_DeleteBookmarkForBackup, VT_I4,VTS_BSTR VTS_BSTR VTS_BSTR VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryDevReport", dispidIVS_OCX_OMUQueryDevReport, IVS_OCX_OMUQueryDevReport, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryDevReportDetail", dispidIVS_OCX_OMUQueryDevReportDetail, IVS_OCX_OMUQueryDevReportDetail, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryRealtimeReport", dispidIVS_OCX_OMUQueryRealtimeReport, IVS_OCX_OMUQueryRealtimeReport, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryRealtimeReportDetail", dispidIVS_OCX_OMUQueryRealtimeReportDetail, IVS_OCX_OMUQueryRealtimeReportDetail, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryQDReport", dispidIVS_OCX_OMUQueryQDReport, IVS_OCX_OMUQueryQDReport, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryQDReportDetail", dispidIVS_OCX_OMUQueryQDReportDetail, IVS_OCX_OMUQueryQDReportDetail, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryRecordPlanReport", dispidIVS_OCX_OMUQueryRecordPlanReport, IVS_OCX_OMUQueryRecordPlanReport, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryRecordPlanReportDetail", dispidIVS_OCX_OMUQueryRecordPlanReportDetail, IVS_OCX_OMUQueryRecordPlanReportDetail, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryRecordIntegralityReport", dispidIVS_OCX_OMUQueryRecordIntegralityReport, IVS_OCX_OMUQueryRecordIntegralityReport, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryRecordIntegralityReportDetail", dispidIVS_OCX_OMUQueryRecordIntegralityReportDetail, IVS_OCX_OMUQueryRecordIntegralityReportDetail, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryPacketLossRateReport", dispidIVS_OCX_OMUQueryPacketLossRateReport, IVS_OCX_OMUQueryPacketLossRateReport, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryPacketLossRateReportDetail", dispidIVS_OCX_OMUQueryPacketLossRateReportDetail, IVS_OCX_OMUQueryPacketLossRateReportDetail, VT_BSTR, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUQueryDiskUsageRateReport", dispidIVS_OCX_OMUQueryDiskUsageRateReport, IVS_OCX_OMUQueryDiskUsageRateReport, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetDevGroupRemark", dispidIVS_OCX_SetDevGroupRemark, IVS_OCX_SetDevGroupRemark, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDevGroupRemark", dispidIVS_OCX_GetDevGroupRemark, IVS_OCX_GetDevGroupRemark, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUAddAlarmLinkageRule", dispidIVS_OCX_OMUAddAlarmLinkageRule, IVS_OCX_OMUAddAlarmLinkageRule, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUModAlarmLinkageRule", dispidIVS_OCX_OMUModAlarmLinkageRule, IVS_OCX_OMUModAlarmLinkageRule, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUDelAlarmLinkageRule", dispidIVS_OCX_OMUDelAlarmLinkageRule, IVS_OCX_OMUDelAlarmLinkageRule, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUGetAlarmLinkageRuleList", dispidIVS_OCX_OMUGetAlarmLinkageRuleList, IVS_OCX_OMUGetAlarmLinkageRuleList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUGetAlarmLinkageRuleDetail", dispidIVS_OCX_OMUGetAlarmLinkageRuleDetail, IVS_OCX_OMUGetAlarmLinkageRuleDetail, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_OMUGetDevTypeByNodeCode", dispidIVS_OCX_OMUGetDevTypeByNodeCode, IVS_OCX_OMUGetDevTypeByNodeCode, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetPrerecordCameraList", dispidIVS_OCX_GetPrerecordCameraList, IVS_OCX_GetPrerecordCameraList, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_VerifyWatermark", dispidIVS_OCX_VerifyWatermark, IVS_OCX_VerifyWatermark, VT_I4, VTS_I4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_WatermarkAlarmConfirm", dispidIVS_OCX_WatermarkAlarmConfirm, IVS_OCX_WatermarkAlarmConfirm, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_WatermarkAlarmCancel", dispidIVS_OCX_WatermarkAlarmCancel, IVS_OCX_WatermarkAlarmCancel, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_QueryWatermarkAlarmList", dispidIVS_OCX_QueryWatermarkAlarmList, IVS_OCX_QueryWatermarkAlarmList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetPaneWndID", dispidIVS_OCX_GetPaneWndID, IVS_OCX_GetPaneWndID, VT_BSTR, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartLocalPlayBack", dispidIVS_OCX_StartLocalPlayBack, IVS_OCX_StartLocalPlayBack, VT_I4, VTS_I4 VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopLocalPlayBack", dispidIVS_OCX_StopLocalPlayBack, IVS_OCX_StopLocalPlayBack, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDeviceCapability", dispidIVS_OCX_GetDeviceCapability, IVS_OCX_GetDeviceCapability, VT_BSTR, VTS_I4 VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartNetKeyBoard", dispidIVS_OCX_StartNetKeyBoard, IVS_OCX_StartNetKeyBoard, VT_I4, VTS_UI4 VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopNetKeyBoard", dispidIVS_OCX_StopNetKeyBoard, IVS_OCX_StopNetKeyBoard, VT_I4, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetNetKeyBoardPort", dispidIVS_OCX_SetNetKeyBoardPort, IVS_OCX_SetNetKeyBoardPort, VT_I4, VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCameraPlanInfo", dispidIVS_OCX_GetCameraPlanInfo, IVS_OCX_GetCameraPlanInfo, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetExDomainRecordPolicy", dispidIVS_OCX_SetExDomainRecordPolicy, IVS_OCX_SetExDomainRecordPolicy, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetExDomainRecordPolicy", dispidIVS_OCX_GetExDomainRecordPolicy, IVS_OCX_GetExDomainRecordPolicy, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddExDomainRecordPlan", dispidIVS_OCX_AddExDomainRecordPlan, IVS_OCX_AddExDomainRecordPlan, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModExDomainRecordPlan", dispidIVS_OCX_ModExDomainRecordPlan, IVS_OCX_ModExDomainRecordPlan, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelExDomainRecordPlan", dispidIVS_OCX_DelExDomainRecordPlan, IVS_OCX_DelExDomainRecordPlan, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetExDomainRecordPlan", dispidIVS_OCX_GetExDomainRecordPlan, IVS_OCX_GetExDomainRecordPlan, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_CheckNewVersion", dispidIVS_OCX_CheckNewVersion, IVS_OCX_CheckNewVersion, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetDeviceGuard", dispidIVS_OCX_SetDeviceGuard, IVS_OCX_SetDeviceGuard, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetLogPath", dispidIVS_OCX_SetLogPath, IVS_OCX_SetLogPath, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SignOutUserByRole", dispidIVS_OCX_SignOutUserByRole, IVS_OCX_SignOutUserByRole, VT_I4, VTS_BSTR VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartBackupPlayBack", dispidIVS_OCX_StartBackupPlayBack, IVS_OCX_StartBackupPlayBack, VT_I4, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopBackupPlayBack", dispidIVS_OCX_StopBackupPlayBack, IVS_OCX_StopBackupPlayBack, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ShowInstantReplayBar", dispidIVS_OCX_ShowInstantReplayBar, IVS_OCX_ShowInstantReplayBar, VT_I4, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCameraThumbnail", dispidIVS_OCX_GetCameraThumbnail, IVS_OCX_GetCameraThumbnail, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAlarmSnapshotList", dispidIVS_OCX_GetAlarmSnapshotList, IVS_OCX_GetAlarmSnapshotList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModLockRecord", dispidIVS_OCX_ModLockRecord, IVS_OCX_ModLockRecord, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ShowPaneBar", dispidIVS_OCX_ShowPaneBar, IVS_OCX_ShowPaneBar, VT_I4, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartBackupDownload", dispidIVS_OCX_StartBackupDownload, IVS_OCX_StartBackupDownload, VT_BSTR,VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopBackupDownload", dispidIVS_OCX_StopBackupDownload, IVS_OCX_StopBackupDownload, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddRealPlayBookmark", dispidIVS_OCX_AddRealPlayBookmark, IVS_OCX_AddRealPlayBookmark, VT_BSTR,VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModifyRealPlayBookmark", dispidIVS_OCX_ModifyRealPlayBookmark, IVS_OCX_ModifyRealPlayBookmark, VT_I4, VTS_BSTR VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteRealPlayBookmark", dispidIVS_OCX_DeleteRealPlayBookmark, IVS_OCX_DeleteRealPlayBookmark, VT_I4, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartSyncPlayTVWall", dispidIVS_OCX_StartSyncPlayTVWall, IVS_OCX_StartSyncPlayTVWall, VT_I4, VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopSyncPlayTVWall", dispidIVS_OCX_StopSyncPlayTVWall, IVS_OCX_StopSyncPlayTVWall, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetPaneEMap", dispidIVS_OCX_SetPaneEMap, IVS_OCX_SetPaneEMap, VT_BSTR, VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetPWDExpireInfo", dispidIVS_OCX_GetPWDExpireInfo, IVS_OCX_GetPWDExpireInfo, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetNetkeyboardList", dispidIVS_OCX_GetNetkeyboardList, IVS_OCX_GetNetkeyboardList, VT_BSTR, VTS_NONE)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SaveNetkeyboardCamera", dispidIVS_OCX_SaveNetkeyboardCamera, IVS_OCX_SaveNetkeyboardCamera, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModNetkeyboardCamera", dispidIVS_OCX_ModNetkeyboardCamera, IVS_OCX_ModNetkeyboardCamera, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ImportDevCfgFile", dispidIVS_OCX_ImportDevCfgFile, IVS_OCX_ImportDevCfgFile, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ExportDevCfgFile", dispidIVS_OCX_ExportDevCfgFile, IVS_OCX_ExportDevCfgFile, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_PlaybackFrameStepForwardTVWall", dispidIVS_OCX_PlaybackFrameStepForwardTVWall, IVS_OCX_PlaybackFrameStepForwardTVWall, VT_I4, VTS_UI4 VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelGuardPos", dispidIVS_OCX_DelGuardPos, IVS_OCX_DelGuardPos, VT_I4, VTS_BSTR)	
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddCameraSwitchPlanTVWall", dispidIVS_OCX_AddCameraSwitchPlanTVWall, IVS_OCX_AddCameraSwitchPlanTVWall, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DelCameraSwitchPlanTVWall", dispidIVS_OCX_DelCameraSwitchPlanTVWall, IVS_OCX_DelCameraSwitchPlanTVWall, VT_I4, VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCameraSwitchPlanListTVWall", dispidIVS_OCX_GetCameraSwitchPlanListTVWall, IVS_OCX_GetCameraSwitchPlanListTVWall, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCameraSwitchPlanTVWall", dispidIVS_OCX_GetCameraSwitchPlanTVWall, IVS_OCX_GetCameraSwitchPlanTVWall, VT_BSTR, VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModLockBackupRecord", dispidIVS_OCX_ModLockBackupRecord, IVS_OCX_ModLockBackupRecord, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_PlayBackPauseTVWall", dispidIVS_OCX_PlayBackPauseTVWall, IVS_OCX_PlayBackPauseTVWall, VT_I4, VTS_UI4 VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_PlayBackResumeTVWall", dispidIVS_OCX_PlayBackResumeTVWall, IVS_OCX_PlayBackResumeTVWall, VT_I4, VTS_UI4 VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartPUPlayBackTVWall", dispidIVS_OCX_StartPUPlayBackTVWall, IVS_OCX_StartPUPlayBackTVWall, VT_I4, VTS_UI4 VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopPUPlayBackTVWall", dispidIVS_OCX_StopPUPlayBackTVWall, IVS_OCX_StopPUPlayBackTVWall, VT_I4, VTS_UI4 VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRecordThumbnail", dispidIVS_OCX_GetRecordThumbnail, IVS_OCX_GetRecordThumbnail, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetPaneWnd", dispidIVS_OCX_GetPaneWnd, IVS_OCX_GetPaneWnd, VT_UI4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetDevPWD", dispidIVS_OCX_SetDevPWD, IVS_OCX_SetDevPWD, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetSkin", dispidIVS_OCX_SetSkin, IVS_OCX_SetSkin, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ResetAlarm", dispidIVS_OCX_ResetAlarm, IVS_OCX_ResetAlarm, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ExEventSubscribe", dispidIVS_OCX_ExEventSubscribe, IVS_OCX_ExEventSubscribe, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetExEventSubscribe", dispidIVS_OCX_GetExEventSubscribe, IVS_OCX_GetExEventSubscribe, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ExCatalogSubscribe", dispidIVS_OCX_ExCatalogSubscribe, IVS_OCX_ExCatalogSubscribe, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetExCatalogSubscribeList", dispidIVS_OCX_GetExCatalogSubscribeList, IVS_OCX_GetExCatalogSubscribeList, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartBackupPlayBackTVWall", dispidIVS_OCX_StartBackupPlayBackTVWall, IVS_OCX_StartBackupPlayBackTVWall, VT_I4, VTS_UI4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopBackuplayBackTVWall", dispidIVS_OCX_StopBackuplayBackTVWall, IVS_OCX_StopBackuplayBackTVWall, VT_I4, VTS_UI4 VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetAbilityInfo", dispidIVS_OCX_GetAbilityInfo, IVS_OCX_GetAbilityInfo, VT_UI4, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetScenarioRecovery", dispidIVS_OCX_SetScenarioRecovery, IVS_OCX_SetScenarioRecovery, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetScenarioRecovery", dispidIVS_OCX_GetScenarioRecovery, IVS_OCX_GetScenarioRecovery, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetGraphicSelected", dispidIVS_OCX_GetGraphicSelected, IVS_OCX_GetGraphicSelected, VT_BSTR, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_EnableExchangePane", dispidIVS_OCX_EnableExchangePane, IVS_OCX_EnableExchangePane, VT_EMPTY,VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetRealTimeTempScenario", dispidIVS_OCX_SetRealTimeTempScenario, IVS_OCX_SetRealTimeTempScenario, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetRecordTempScenario", dispidIVS_OCX_SetRecordTempScenario, IVS_OCX_SetRecordTempScenario, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetEmapTempScenario", dispidIVS_OCX_SetEmapTempScenario, IVS_OCX_SetEmapTempScenario, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRealTimeTempScenario", dispidIVS_OCX_GetRealTimeTempScenario, IVS_OCX_GetRealTimeTempScenario, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetRecordTempScenario", dispidIVS_OCX_GetRecordTempScenario, IVS_OCX_GetRecordTempScenario, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetEmapTempScenario", dispidIVS_OCX_GetEmapTempScenario, IVS_OCX_GetEmapTempScenario, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_KeyEventNotice", dispidIVS_OCX_KeyEventNotice, IVS_OCX_KeyEventNotice, VT_EMPTY, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetVolume", dispidIVS_OCX_SetVolume, IVS_OCX_SetVolume, VT_I4, VTS_I4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetVolume", dispidIVS_OCX_GetVolume, IVS_OCX_GetVolume, VT_UI4, VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetTitleBarStatus", dispidIVS_OCX_SetTitleBarStatus, IVS_OCX_SetTitleBarStatus, VT_UI4, VTS_UI4 VTS_UI4 VTS_UI4)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ModCameraSwitch", dispidIVS_OCX_ModCameraSwitch, IVS_OCX_ModCameraSwitch, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_DeleteBroadcastFile", dispidIVS_OCX_DeleteBroadcastFile, IVS_OCX_DeleteBroadcastFile, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetBroadcastCameraList", dispidIVS_OCX_GetBroadcastCameraList, IVS_OCX_GetBroadcastCameraList, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_AddBroadcastFile", dispidIVS_OCX_AddBroadcastFile, IVS_OCX_AddBroadcastFile, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetBroadcastFileList", dispidIVS_OCX_GetBroadcastFileList, IVS_OCX_GetBroadcastFileList, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ExGetDeviceStatus", dispidIVS_OCX_ExGetDeviceStatus, IVS_OCX_ExGetDeviceStatus, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ExGetDeviceInfo", dispidIVS_OCX_ExGetDeviceInfo, IVS_OCX_ExGetDeviceInfo, VT_I4, VTS_BSTR)
    DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_RefreshCameraList", dispidIVS_OCX_RefreshCameraList, IVS_OCX_RefreshCameraList, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetDeviceEncodeCapabilities", dispidIVS_OCX_GetDeviceEncodeCapabilities, IVS_OCX_GetDeviceEncodeCapabilities, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetCUVersion", dispidIVS_OCX_GetCUVersion, IVS_OCX_GetCUVersion, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartRealPlayTVWall_Async", dispidIVS_OCX_StartRealPlayTVWall_Async, IVS_OCX_StartRealPlayTVWall_Async, VT_I4, VTS_UI4 VTS_BSTR VTS_BSTR VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopRealPlayTVWall_Async", dispidIVS_OCX_StopRealPlayTVWall_Async, IVS_OCX_StopRealPlayTVWall_Async, VT_I4, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StartRealPlay_Async", dispidIVS_OCX_StartRealPlay_Async, IVS_OCX_StartRealPlay_Async, VT_I4, VTS_BSTR VTS_BSTR VTS_I4 VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_StopRealPlay_Async", dispidIVS_OCX_StopRealPlay_Async, IVS_OCX_StopRealPlay_Async, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetLogLevel", dispidIVS_OCX_SetLogLevel, IVS_OCX_SetLogLevel, VT_EMPTY, VTS_UI4)
	
	
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_LoginByTicket", dispidIVS_OCX_LoginByTicket, IVS_OCX_LoginByTicket, VT_I4, VTS_BSTR VTS_BSTR VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetReceiveOMUAlarm", dispidIVS_OCX_SetReceiveOMUAlarm, IVS_OCX_SetReceiveOMUAlarm, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetVideoDiagnose", dispidIVS_OCX_GetVideoDiagnose, IVS_OCX_GetVideoDiagnose, VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetVideoDiagnose", dispidIVS_OCX_SetVideoDiagnose, IVS_OCX_SetVideoDiagnose, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetStreamListByCam", dispidIVS_OCX_GetStreamListByCam, IVS_OCX_GetStreamListByCam, VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_ShutdownStreamByUser", dispidIVS_OCX_ShutdownStreamByUser, IVS_OCX_ShutdownStreamByUser, VT_I4, VTS_BSTR VTS_UI4 VTS_BSTR VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetStreamListByUser", dispidIVS_OCX_GetStreamListByUser, IVS_OCX_GetStreamListByUser, VT_BSTR, VTS_BSTR VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetMicVolume", dispidIVS_OCX_SetMicVolume, IVS_OCX_SetMicVolume, VT_I4, VTS_UI4)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_GetMicVolume", dispidIVS_OCX_GetMicVolume, IVS_OCX_GetMicVolume, VT_UI4, VTS_NONE)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_LocalSnapshotToDir", dispidIVS_OCX_LocalSnapshotToDir, IVS_OCX_LocalSnapshotToDir, VT_I4, VTS_I4 VTS_UI4 VTS_BSTR)
	DISP_FUNCTION_ID(CIVS_OCXPlayerCtrl, "IVS_OCX_SetVideoEffect", dispidIVS_OCX_SetVideoEffect, IVS_OCX_SetVideoEffect, VT_I4, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	END_DISPATCH_MAP()

// Event map
BEGIN_EVENT_MAP(CIVS_OCXPlayerCtrl, COleControl)
    EVENT_CUSTOM_ID("IVS_OCX_Event", eventidIVS_OCX_Event, IVS_OCX_Event, VTS_I4 VTS_BSTR)
    EVENT_CUSTOM_ID("OnOCXInfoButtonClick", eventidOnOCXInfoButtonClick, OnOCXInfoButtonClick, VTS_UI4 VTS_BSTR VTS_UI4 VTS_UI4 VTS_UI4 VTS_UI4 VTS_UI4)
    EVENT_CUSTOM_ID("OnAlarmLinkage", eventidOnAlarmLinkage, OnAlarmLinkage, VTS_BSTR)
    EVENT_CUSTOM_ID("OnAlarmReporter", eventidOnAlarmReporter, OnAlarmReporter, VTS_BSTR)
    EVENT_CUSTOM_ID("OnKeepAlive", eventidOnKeepAlive, OnKeepAlive, VTS_UI4)
	EVENT_CUSTOM_ID("OnUserOffline", eventidOnUserOffline, OnUserOffline, VTS_BSTR)
	EVENT_CUSTOM_ID("IVS_OCX_OnTVWall", eventidOnTVWall, IVS_OCX_OnTVWall, VTS_BSTR)
	EVENT_CUSTOM_ID("IVS_OCX_Windows_Event",eventidOnIVS_OCX_Windows_Event,IVS_OCX_Windows_Event,VTS_I4 VTS_BSTR)
    EVENT_CUSTOM_ID("OnNetKeyBoard", eventidOnNetKeyBoard, OnNetKeyBoard, VTS_BSTR)
END_EVENT_MAP()
//lint +e786 这部分代码由MS自动产生
BEGIN_INTERFACE_MAP(CIVS_OCXPlayerCtrl, COleControl)  
	INTERFACE_PART(CIVS_OCXPlayerCtrl, IID_IObjectSafety, ObjSafe)   //这儿只添加这一句   
END_INTERFACE_MAP()  

// Property pages
BEGIN_PROPPAGEIDS(CIVS_OCXPlayerCtrl, 1)
	PROPPAGEID(CIVS_OCXPlayerPropPage::guid)
END_PROPPAGEIDS(CIVS_OCXPlayerCtrl)

// Initialize class factory and guid
IMPLEMENT_OLECREATE_EX(CIVS_OCXPlayerCtrl, "IVS_OCXPLAYER.IVS_OCXPlayerCtrl.1",
	0x3556A474, 0x8B23, 0x496F, 0x9e, 0x5d, 0x38, 0xf7, 0xb7, 0x46, 0x54, 0xf4)

// Type library ID and version
IMPLEMENT_OLETYPELIB(CIVS_OCXPlayerCtrl, _tlid, _wVerMajor, _wVerMinor);

// Interface IDs
const IID IID_DIVS_OCXPlayer =
{ 0x53E664DD, 0x3322, 0x4638, { 0x80, 0x82, 0x85, 0xC3, 0x97, 0xE7, 0x5D, 0xB1 } };
const IID IID_DIVS_OCXPlayerEvents =
{ 0x8F5A25B5, 0xB120, 0x4C4B, { 0xB7, 0x96, 0x27, 0x90, 0x4B, 0x2F, 0x59, 0x21 } };

// Control type information

static const DWORD _dwIVS_OCXPlayerOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CIVS_OCXPlayerCtrl, IDS_IVS_OCXPLAYER, _dwIVS_OCXPlayerOleMisc);
// 系统生成
// CIVS_OCXPlayerCtrl::CIVS_OCXPlayerCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CIVS_OCXPlayerCtrl

BOOL CIVS_OCXPlayerCtrl::CIVS_OCXPlayerCtrlFactory::UpdateRegistry(BOOL bRegister)
{
    // TODO: Verify that your control follows apartment-model threading rules.
    // Refer to MFC TechNote 64 for more information.
    // If your control does not conform to the apartment-model rules, then
    // you must modify the code below, changing the 6th parameter from
    // afxRegApartmentThreading to 0.

    if (bRegister)
    {
        return AfxOleRegisterControlClass(
            AfxGetInstanceHandle(),
            m_clsid,
            m_lpszProgID,
            IDS_IVS_OCXPLAYER,
            IDB_IVS_OCXPLAYER,
            afxRegApartmentThreading,
            _dwIVS_OCXPlayerOleMisc,
            _tlid,
            _wVerMajor,
            _wVerMinor);
    }
    else
    {
        return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
    }
}

#pragma region CLASS_STATIC_VAR_DEF

int CIVS_OCXPlayerCtrl::g_nNumberOfOcx = 0;

std::list<CIVS_OCXPlayerCtrl *> CIVS_OCXPlayerCtrl::m_OcxList;
CIVSMutex    CIVS_OCXPlayerCtrl::m_csOcxListLock;

std::map<HWND,CIVS_OCXPlayerCtrl *> CIVS_OCXPlayerCtrl::m_hWnd2OcxMap;            //控件的map表
CIVSMutex    CIVS_OCXPlayerCtrl::m_csHWnd2OcxLock;     //控件与窗口句柄的map表的锁

DOWNLOAD_HANDLE_MAP  CIVS_OCXPlayerCtrl::m_DownloadHandleMap;
CIVSMutex     CIVS_OCXPlayerCtrl::m_csDownloadMapLock;

std::map<ULONG, CIVS_OCXPlayerCtrl*>  CIVS_OCXPlayerCtrl::m_VedioDownloadUlport2OCX;      // 录像下载map表(端口->OCX地址)
CIVSMutex    CIVS_OCXPlayerCtrl::m_csVedioDownloadUlport2OCXLock;//录像下载map表锁

BOOL  CIVS_OCXPlayerCtrl::m_bVoiceTalkbacking = FALSE; // 当前语音对讲开启状态
CVideoPane * CIVS_OCXPlayerCtrl::m_pVoiceTalkbackVideoPane = NULL;
ULONG  CIVS_OCXPlayerCtrl::m_ulVoiceTalkbackVolume = PLAYER_AUDIO_MAX_VOLUM;
ULONG  CIVS_OCXPlayerCtrl::m_ulVoiceTalkbackHandle = 0;
std::string CIVS_OCXPlayerCtrl::m_strVoiceTalkbackCameraCode; // 当前语音对讲设备编码

CIVS_OCXPlayerCtrl *  CIVS_OCXPlayerCtrl::m_pOCX = NULL;
HHOOK CIVS_OCXPlayerCtrl::g_hook = nullptr;
bool CIVS_OCXPlayerCtrl::g_IsInitHook = false;
CIVS_OCXPlayerCtrl *  CIVS_OCXPlayerCtrl::m_pVoiceTalkbackOCX = NULL;                         // 语音对讲的ocx
CIVS_OCXPlayerCtrl *  CIVS_OCXPlayerCtrl::m_pVoiceBroadcastOCX = NULL;							//语音广播的ocx

CIVSMutex  CIVS_OCXPlayerCtrl::m_HandleNotifyLock;        //电视墙向CU发消息的锁
CIVS_OCXPlayerCtrl* CIVS_OCXPlayerCtrl::m_pTvwallOcx = NULL; //记录电视墙控件的指针
ULONG CIVS_OCXPlayerCtrl::g_ulSkinType = 1;//默认是黑皮肤

CIVSMutex CIVS_OCXPlayerCtrl::m_csTVWallTempScenarioLock;  //读写临时场景的锁

IVS_INT32 CIVS_OCXPlayerCtrl::m_iOldCallBackKeyValue = 0;           //上一次网络键盘发的云台方向命令
char CIVS_OCXPlayerCtrl::m_cOldSpeed = 0;                           //上一次云台方向速度

#pragma endregion CLASS_STATIC_VAR_DEF

IVS_INT32 CIVS_OCXPlayerCtrl::m_iSDKSessionID = SDK_SESSIONID_INIT;
#define  WM_WEB_MOVE_MIN 49000
#define  WM_WEB_MOVE_MAX 51000

inline void HookProcMove(HWND pMsgWnd,LONG lMsgType)
{
	std::list<CIVS_OCXPlayerCtrl *>::iterator iter = CIVS_OCXPlayerCtrl::m_OcxList.begin();
	for (; iter!=CIVS_OCXPlayerCtrl::m_OcxList.end(); iter++)
	{
		CIVS_OCXPlayerCtrl *pOcx = *iter;
		if (pOcx == nullptr )
		{
			continue;
		}

		CVideoPaneMgr* pVideoPaneMgr =  pOcx->GetVideoPaneMgr();
		if (pVideoPaneMgr != NULL)
		{
			if (lMsgType == WM_MOVE /*|| lMsgType == WM_MOVING*/)
			{
				CWnd* pParent = pOcx->GetParentOwner();
				CHECK_POINTER_VOID(pParent);
				if (pParent->m_hWnd == pMsgWnd)
				{
					HWND hwnd = GetFocus();
					pVideoPaneMgr->ResetBarPosition();
					::SetFocus(hwnd);
				}
				else
				{
					return;
				}
			}
			else
			{
				HWND hwnd = GetFocus();
				pVideoPaneMgr->ResetBarPosition();
				::SetFocus(hwnd);
			}
		}
	}
}//lint !e818

inline void HookProcFullScreen()
{
	std::list<CIVS_OCXPlayerCtrl *>::iterator iter = CIVS_OCXPlayerCtrl::m_OcxList.begin();
	for (; iter!=CIVS_OCXPlayerCtrl::m_OcxList.end(); iter++)
	{
		CIVS_OCXPlayerCtrl *pOcx = *iter;
		if (pOcx == nullptr )
		{
			continue;
		}

		pOcx->PostMessage(WM_RESET_FULL_SCREEN,0,0);
	}
}

inline void HookProcHide()
{
	std::list<CIVS_OCXPlayerCtrl *>::iterator iter = CIVS_OCXPlayerCtrl::m_OcxList.begin();
	for (; iter!=CIVS_OCXPlayerCtrl::m_OcxList.end(); iter++)
	{
		CIVS_OCXPlayerCtrl *pOcx = *iter;
		if (pOcx == nullptr )
		{
			continue;
		}
		CVideoPaneMgr* pVideoPaneMgr =  pOcx->GetVideoPaneMgr();
		if (pVideoPaneMgr != NULL)
		{
			//pVideoPaneMgr->HideAllTranslucenWnd();
			pVideoPaneMgr->HideAllBarOnce();
			//pVideoPaneMgr->ShowTitleBar(FALSE);
			//pVideoPaneMgr->ShowToolBar(FALSE);
			//pVideoPaneMgr->ResetBarPosition();
		}
	}
}

LRESULT CALLBACK  HookProc(int   nCode,   WPARAM   wParam,   LPARAM   lParam)
{
    if(nCode   ==   HC_ACTION)
    {
		CWPSTRUCT* lpCWP=(CWPSTRUCT*)lParam;
        if(lpCWP->message== WM_MOVE /*|| lpCWP->message== WM_MOVING*/ ||(lpCWP->message >= WM_WEB_MOVE_MIN && lpCWP->message <= WM_WEB_MOVE_MAX))
        {
			return 0;
        }
		if (lpCWP->wParam == VK_ESCAPE && lpCWP->message != WM_GETTEXT)
		{
			HookProcFullScreen();
		}
		//if (lpCWP->message == SW_HIDE)
		//{
		//	HookProcHide();
		//}
    }

    if (CIVS_OCXPlayerCtrl::g_hook != nullptr)
    {
        return CallNextHookEx(CIVS_OCXPlayerCtrl::g_hook,nCode,wParam,lParam);
    }
    return 0;
}

static IVS_INT32 GetHwndbyPlayHandle(IVS_ULONG ulPlayHandle, HWND& hWnd)
{
    IVS_INT32 iRet = IVS_FAIL;

    std::map <HWND,CIVS_OCXPlayerCtrl *>::iterator iter = CIVS_OCXPlayerCtrl::m_hWnd2OcxMap.begin();
    std::map <HWND,CIVS_OCXPlayerCtrl *>::iterator iterEnd = CIVS_OCXPlayerCtrl::m_hWnd2OcxMap.end();
    for (; iter != iterEnd; iter++)
    {
        CIVS_OCXPlayerCtrl* pPlayCtrl = dynamic_cast<CIVS_OCXPlayerCtrl*>(iter->second);
        if (NULL == pPlayCtrl)
        {
            continue;
        }

        CVideoPaneMgr* pPaneMgr = pPlayCtrl->GetVideoPaneMgr();
        if (NULL == pPaneMgr)
        {
            continue;
        }

        iRet = pPaneMgr->GetHwndbyPlayHandle(ulPlayHandle, hWnd);
        if (IVS_SUCCEED == iRet)
        {
            return iRet;
        }
    }

    return iRet;
}

// CIVS_OCXPlayerCtrl::CIVS_OCXPlayerCtrl - Constructor
CIVS_OCXPlayerCtrl::CIVS_OCXPlayerCtrl(CWnd* pParent)
    : m_bIsInit(false)
    , m_pVideoPaneMgr(nullptr)
    , m_ulColor(ENUMDEF::COLOR_DARK)
    , m_szCameraCode("")
	, gdiplusToken(0)
{
    InitializeIIDs(&IID_DIVS_OCXPlayer, &IID_DIVS_OCXPlayerEvents);
    m_hwnd2wid.clear();
	m_mapOnWall.clear();
	m_pMutexCameraAlias = VOS_CreateMutex();
} //lint !e818 !e715 !e1401 MFC自动生成的的函数

// CIVS_OCXPlayerCtrl::~CIVS_OCXPlayerCtrl - Destructor
CIVS_OCXPlayerCtrl::~CIVS_OCXPlayerCtrl()
{
    try
    {
		//释放保存的电视墙上墙参数
		for(TVWall_OnWall_Map::iterator iter = m_mapOnWall.begin();iter!=m_mapOnWall.end();iter++)
		{
			OnWallPara *para = iter->second;
			IVS_DELETE(para);
		}
		m_mapOnWall.clear();

        m_pLocalConfig.StopLocalConfigTimer();
        (void)IVS_OCX_Cleanup();

		VOS_DestroyMutex(m_pMutexCameraAlias);
		m_pMutexCameraAlias = NULL;
    } catch (...)
    {}
}//lint !e1740

// CIVS_OCXPlayerCtrl::OnDraw - Drawing function

void CIVS_OCXPlayerCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	//IVS_INFO_TRACE("OCX Address: %x", this);
    CHECK_POINTER_VOID(pdc);
    CHECK_POINTER_VOID(m_pVideoPaneMgr);
    //m_pVideoPaneMgr->ResetBarPosition();
	
}//lint !e715

// CIVS_OCXPlayerCtrl::DoPropExchange - Persistence support

void CIVS_OCXPlayerCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}

// CIVS_OCXPlayerCtrl::OnResetState
void CIVS_OCXPlayerCtrl::OnResetState()
{
	COleControl::OnResetState();
}

// CIVS_OCXPlayerCtrl message handlers
LONG CIVS_OCXPlayerCtrl::IVS_OCX_FullScreenDisplay(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);
    m_pVideoPaneMgr->SetFullScreen();
    return IVS_SUCCEED;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_NormalScreenDisplay(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);
    m_pVideoPaneMgr->ResetFullScreen();
    return IVS_SUCCEED;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_GetMouseWnd(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("OCX Address: %x", this);

    ULONG ulWndID = m_pVideoPaneMgr->GetWndIDByMouse();
    return static_cast<LONG>(ulWndID);
}

DWORD CIVS_OCXPlayerCtrl::GetControlFlags()
{
    DWORD dwFlags = COleControl::GetControlFlags();

    // 控件通过不还原设备上下文中的
    // 原始 GDI 对象，可以优化它的 OnDraw 方法。
    dwFlags |= canOptimizeDraw;
    return dwFlags;
}

BOOL CIVS_OCXPlayerCtrl::PreTranslateMessage(MSG* pMsg)
{
    //处理键盘事件专用
    switch (pMsg->message)
    {
 	case WM_SYSKEYDOWN:
 	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
		(void)WM_KEYUPEvent(pMsg);
		break;
	default:
		break;
    }

    return COleControl::PreTranslateMessage(pMsg);
}

LONG CIVS_OCXPlayerCtrl::WM_KEYUPEvent(const MSG* pMsg)
{
	CHECK_POINTER(m_pVideoPaneMgr,TRUE);
	CVideoPane* pActiveVedioPane = NULL;

	switch (pMsg->wParam)
	{
	case VK_F8:
		{
			//IVS_LOG(LOG_DEBUG, "OCX key F8 is pressed.(function PreTranslateMessage)");
			//转发给分屏窗口键盘消息
			((CVideoPaneMgr*)m_pVideoPaneMgr)->SetFullScreen();
		}
		break;
	case VK_DELETE:
		if (pMsg->message == WM_KEYDOWN)
		{
			// 激活窗口
			pActiveVedioPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetActiveWndPointer();
			if (NULL == pActiveVedioPane)
			{
				break;
			}
			if (MAGNIFY_OPEN == pActiveVedioPane->GetRealtimeMagnifyState())
			{
				::PostMessage(pActiveVedioPane->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (LONG)(pActiveVedioPane->GetCurrentGraphicGroup()));
			}
		}
		break;
	case 'R':
		if (pMsg->message == WM_KEYDOWN)
		{
			// 激活窗口
			pActiveVedioPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetActiveWndPointer();
			if (NULL == pActiveVedioPane)
			{
				break;
			}

			if (NULL != pActiveVedioPane->GetVideoRealtimeBar())
			{
				::PostMessage(pActiveVedioPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_RECORD, BN_CLICKED), (LPARAM)pActiveVedioPane->GetVideoRealtimeBar()->GetRecordBtnHwnd());
			}
		}
		break;
// 	case 0x41://VK_A
// 		if (pMsg->message == WM_KEYDOWN)
// 		{
// 			CVideoPaneMgr *pPanelMgr = (CVideoPaneMgr*)m_pVideoPaneMgr;
// 			pPanelMgr->StartSerialCaptureAll();
// 		}
// 		break;
	default:
		break;
	}

	ULONG ulWinID =  ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetActivePaneID();
	CString strResult;

	COCXPlayBackXMLProcess::GetKeyEventXML(strResult,ulWinID,static_cast<ULONG>(pMsg->wParam));
	// alt按下的情况下 需要使用WM_SYSKEYDOWN
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		IVS_OCX_Windows_Event((long)EVENT_KEYBOARD_PRESS, (LPCTSTR)strResult);
	}
	else if (pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP)
	{
		IVS_OCX_Windows_Event((long)EVENT_KEYBOARD_UP, (LPCTSTR)strResult);
	}
	return IVS_SUCCEED;
}

ULONG CIVS_OCXPlayerCtrl::FullScreenKeyEvent(ULONG wlMsg)
{
	CHECK_POINTER(m_pVideoPaneMgr,TRUE);
	//CVideoPane* pActiveVedioPane = NULL;
	ULONG ulWinID =  ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetActivePaneID();
	CString strResult;
	COCXPlayBackXMLProcess::GetKeyEventXML(strResult,ulWinID,wlMsg);

	IVS_OCX_Windows_Event((long)EVENT_KEYBOARD_UP, (LPCTSTR)strResult);
	IVS_OCX_Windows_Event((long)EVENT_KEYBOARD_PRESS, (LPCTSTR)strResult);
	return IVS_SUCCEED;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_Init(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//IVS_INFO_TRACE("OCX Address: %x", this); del w00210470
	if (NULL == CIVS_OCXPlayerCtrl::m_pOCX)
	{
		BP_RUN_LOG_INF("OCX_Init","CIVS_OCXPlayerCtrl::m_pOCX");
		CIVS_OCXPlayerCtrl::m_pOCX = this;
	}

    ENUMDEF::WND_TYPE ulWndType = WND_TYPE_REALTIME;

    if (m_bIsInit)
    {
        return IVS_SUCCEED;
    }

    // 标志置位
    m_bIsInit = true;
    g_nNumberOfOcx++;

    if (1 == g_nNumberOfOcx)
    {
        m_hWnd2OcxMap.clear();
        m_DownloadHandleMap.clear();
        m_VedioDownloadUlport2OCX.clear();

        if (!g_IsInitHook)
        {
            //HINSTANCE hInst=AfxGetInstanceHandle();//写NULL也行，不知道什么用
            //DWORD threadId = GetCurrentThreadId();//activex控件和IE在同一个线程
			DWORD threadId = GetWindowThreadProcessId(this->m_hWnd,NULL);
            g_hook  = ::SetWindowsHookEx(WH_CALLWNDPROC,HookProc,NULL,threadId);
            if (g_hook != nullptr)
            {
                g_IsInitHook = true;
            }
        }
        // 初始化画图日志
        const char* pLogPath = NULL;
        std::string strLogPath = m_szLogPath;
        if (!strLogPath.empty())
        {
            strLogPath.append("\\");
            pLogPath = strLogPath.c_str();
        }
        Draw_LibInit(pLogPath);
        ::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    }

	m_csOcxListLock.Lock();
    m_OcxList.push_back(this);
	m_csOcxListLock.UnLock();

    //初始化媒体库
    if (IVS_SUCCEED != IVS_SDK_Init())
    {
        BP_RUN_LOG_ERR(IVS_OCX_SDK_INIT_FAIL, "IVS OCX Init Failed.", "Reason:IVS SDK init Failed.");
        return IVS_OCX_SDK_INIT_FAIL;
    }
    int iLoadConfig = CSDKConfig::instance().LoadConfigFile();
    if (IVS_SUCCEED != iLoadConfig)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"LoadConfigFile Failed", "Client will use default config, iLoadConfig = %d",iLoadConfig);
    }
	
	//初始化 窗体布局
	LayoutMgr.InitLayOut();
    CHECK_POINTER(IVS_NEW((CVideoPaneMgr*&)m_pVideoPaneMgr), IVS_ALLOC_MEMORY_ERROR) ;

    CUtility::SetVideoPaneMgr(m_pVideoPaneMgr);
    if (IVS_SUCCEED != m_pVideoPaneMgr->SetOcxCtrl(this))
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"IVS OCX Init Failed.","Reason:VideoPane Manager set ocx ctrl fail.")
        return IVS_OPERATE_MEMORY_ERROR;
    }

    m_pVideoPaneMgr->SetWndType(ulWndType);
    m_pVideoPaneMgr->SetColor(m_ulColor);

    // 初始化CVideoPaneMgr
    if (IVS_SUCCEED != m_pVideoPaneMgr->Init())
    {
        //删除mgr在cleanup里统一处理，此处不作处理
        BP_RUN_LOG_ERR(IVS_SYS_NO_SUPPORT,"IVS OCX Init Failed.","Reason:VideoPane Manager initial Failed.");
        return IVS_SYS_NO_SUPPORT;
    }
    return IVS_SUCCEED;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_Cleanup(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
    CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);

    //如果是实况的ocx,那要关闭语音对讲
    if (WND_TYPE_REALTIME == m_pVideoPaneMgr->GetWndType())
    {
        //(void)IVS_OCX_StopVoiceTalkback();
    }

	CleanupTVWall();

	//移除电视墙事件回调
	(void)IVS_SDK_RemoveCallBackTVWall(m_iSDKSessionID, this);
    if (m_bIsInit)
    {
        m_bIsInit = FALSE;
		
        IVS_DELETE(m_pVideoPaneMgr);
		if (1 == g_nNumberOfOcx)
		{
			IVS_SDK_SetEventCallBack(m_iSDKSessionID, NULL, NULL);
			BP_RUN_LOG_INF("IVS OCX Clean Up Finished.", "OCX Number is %d", g_nNumberOfOcx);
		}
		
        m_csOcxListLock.Lock();
        m_OcxList.remove(this);
        m_csOcxListLock.UnLock();

        if (1 == g_nNumberOfOcx)
        {
            m_hWnd2OcxMap.clear();
            m_DownloadHandleMap.clear();
            m_VedioDownloadUlport2OCX.clear();

            if (g_IsInitHook)
            {
                ::UnhookWindowsHookEx(g_hook);
                if (g_hook == nullptr)
                {
                    g_IsInitHook = false;
                }
            }

            // 关闭GDI+的支持
            GdiplusShutdown(gdiplusToken);

			// 清除画图日志
			Draw_LibClean();
// 			IATrack_LibClean();
			BP_RUN_LOG_INF("OCX_Cleanup","CIVS_OCXPlayerCtrl::m_pOCX=NULL");
			CIVS_OCXPlayerCtrl::m_pOCX = NULL;
        }
        g_nNumberOfOcx--;

        //清除媒体库
        if (IVS_SUCCEED != IVS_SDK_Cleanup())
        {
            BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Clean Up Failed.", "Reason:SDK Cleanup Failed.");
        }
    }
    return IVS_SUCCEED;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_GetVersion(void)const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    WORD nlVersion = MAKEWORD(IVS_OCX_FORTH_VERSION,IVS_OCX_THIRD_VERSION);
    WORD nhVersion = MAKEWORD(IVS_OCX_SECOND_VERSION,IVS_OCX_MAIN_VERSION);
    LONG dTmp;
    dTmp = MAKELONG(nlVersion, nhVersion);
    return dTmp;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddUser(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x", this);

    CString strResult = COCXUserInfoMgr::AddUser(m_iSDKSessionID, pReqXml);
	
	// 清理内存，保证内存中不会有密码的存在
	memset((char*)pReqXml,0,strlen(pReqXml));
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetHelpInfoText(LPCTSTR pAlarmInCode, LPCTSTR pAlarmType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == pAlarmInCode || NULL == pAlarmType)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get HelpInfo Text Failed.", "Reason:pAlarmInCode or pAlarmType is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,pAlarmInCode :%s,pAlarmType :%s", this,pAlarmInCode,pAlarmType);
    // 分配空间
    CString strResult;
    int iRet = IVS_FAIL;
    char* pHelpInfoText = IVS_NEW((char*&)pHelpInfoText,(IVS_UINT32)(IVS_MAX_HELP_LEN + 1));
    if (NULL == pHelpInfoText)
    {
        iRet = IVS_ALLOC_MEMORY_ERROR;
        COCXXmlProcess::GetErrString(strResult,iRet);
        return strResult.AllocSysString();
    }
    memset(pHelpInfoText, 0, IVS_MAX_HELP_LEN + 1);
    // 获取告警帮助信息正文
    iRet = IVS_SDK_GetHelpInfoText(m_iSDKSessionID, pAlarmInCode, pAlarmType, pHelpInfoText, IVS_MAX_HELP_LEN);
    if (IVS_SUCCEED != iRet)
    {
        COCXXmlProcess::GetErrString(strResult,iRet);
        IVS_DELETE(pHelpInfoText, MUILI);
        return strResult.AllocSysString();
    }
    // 拼装xml返回给客户端
    CXml xmlRsp;
    int iGetXMLRet = COCXAlarmMgrXMLProcess::QueryHelpInfoTextGetXML(strResult, pHelpInfoText, iRet, xmlRsp);
    if (IVS_SUCCEED != iGetXMLRet)
    {
        COCXXmlProcess::GetErrString(strResult,iGetXMLRet);
    }
    IVS_DELETE(pHelpInfoText, MUILI);
    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteUser(LPCTSTR pDomainCode, ULONG ulUserID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,UserID :%lu", this, ulUserID);

    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserInfoMgr::DeleteUser(m_iSDKSessionID, pDomainCode, ulUserID);
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyUser(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Modify User Failed.", "Reason:pReqXml is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserInfoMgr::ModifyUser(m_iSDKSessionID, pReqXml);

	// 清理内存，保证内存中不会有密码的存在
	memset((char*)pReqXml,0,strlen(pReqXml));

    return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetUserInfo(LPCTSTR pDomainCode, ULONG ulUserID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,UserID :%lu", this,ulUserID);

    CString strResult = COCXUserInfoMgr::GetUserInfo(m_iSDKSessionID, pDomainCode, ulUserID);
    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_LockUser(LPCTSTR pDomainCode, ULONG ulUserID, ULONG ulLock)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,UserID :%lu,isLock :%lu", this,ulUserID,ulLock);
    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserInfoMgr::LockUser(m_iSDKSessionID, pDomainCode, ulUserID, ulLock);
    return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetUserList(LPCTSTR pUnifiedQuery)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pUnifiedQuery)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get User List Failed.", "Reason:pUnifiedQuery is NULL.");
		return NULL;
	}
    IVS_INFO_TRACE("OCX Address: %x,UnifiedQuery: %s", this,pUnifiedQuery);

    CString strResult = COCXUserInfoMgr::GetUserList(m_iSDKSessionID, pUnifiedQuery);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetOnlineUser(LPCTSTR pUnifiedQuery)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pUnifiedQuery)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get Online User Failed.", "Reason:pUnifiedQuery is NULL.");
		return NULL;
	}
    IVS_INFO_TRACE("OCX Address: %x,UnifiedQuery: %s", this,pUnifiedQuery);

    CString strResult = COCXUserInfoMgr::GetOnlineUser(m_iSDKSessionID, pUnifiedQuery);
    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ChangePWD(LPCTSTR pOldPWD, LPCTSTR pNewPWD)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
	CHECK_POINTER(pOldPWD, IVS_PARA_INVALID);
    CHECK_POINTER(pNewPWD, IVS_PARA_INVALID);

	IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserInfoMgr::ChangePWD(m_iSDKSessionID, pOldPWD, pNewPWD);

	// 防止内存中密码出现
	memset((char*)pOldPWD,0,strlen(pOldPWD));
	memset((char*)pNewPWD,0,strlen(pNewPWD));
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ResetPWD(LPCTSTR pDomainCode, ULONG ulUserID, LPCTSTR pNewPWD)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,UserID: %lu", this,ulUserID);
	CHECK_POINTER(pNewPWD, IVS_PARA_INVALID);

    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserInfoMgr::ResetPWD(m_iSDKSessionID, pDomainCode, ulUserID, pNewPWD);

	// 防止内存中密码出现
	memset((char*)pNewPWD,0,strlen(pNewPWD));
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_CheckPWDByRole(LPCTSTR pDomainCode, ULONG ulRoleID, LPCTSTR pLoginName, LPCTSTR pPWD)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pLoginName)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Check PWD By Role Failed.", "Reason:pLoginName is NULL");
		return IVS_PARA_INVALID;
	}
	if(NULL == pPWD)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Check PWD By Role Failed.", "Reason:pPWD is NULL");
		return IVS_PARA_INVALID;
	}
    IVS_INFO_TRACE("OCX Address: %x,LoginName: %s", this,pLoginName);

    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserInfoMgr::CheckPWDByRole(m_iSDKSessionID, pDomainCode, ulRoleID, pLoginName, pPWD);

	// 防止内存中密码出现
	memset((char*)pPWD,0,strlen(pPWD));
    return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAutoCreatePWD(LPCTSTR pDomainCode, ULONG ulRoleID, LPCTSTR pLoginName)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pLoginName)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get AutoCreate PWD Failed.", "Reasonm:pLoginName is NULL.");
		return NULL;
	}
    IVS_INFO_TRACE("OCX Address: %x,RoleID: %lu,LoginName: %s", this,ulRoleID,pLoginName);
    CString strResult = COCXUserInfoMgr::GetAutoCreatePWD(m_iSDKSessionID, pDomainCode, ulRoleID, pLoginName);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetPWDStrength(LPCTSTR pPWD)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pPWD)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get PWD Strength Failed.", "Reasonm:pPWD is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x", this);
    CString strResult = COCXUserInfoMgr::GetPWDStrength(pPWD);

	// 防止内存中密码出现
	memset((char*)pPWD,0,strlen(pPWD));

    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetPWDExpireInfo(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
    CString strResult = COCXUserInfoMgr::GetPWDExpireInfo(m_iSDKSessionID);
    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetUserDeviceList(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set User DeviceList Failed.", "Reason:pReqXml is NULL.");
		return IVS_PARA_INVALID;
	}
    IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

    IVS_INT32 iRet = IVS_FAIL;

    iRet = COCXUserInfoMgr::SetUserDeviceList(m_iSDKSessionID, pReqXml);
    return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetUserDeviceList(LPCTSTR pUnifiedQuery)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pUnifiedQuery)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get User DeviceList Failed.", "Reason:pUnifiedQuery is NULL");
		return NULL;
	}
    IVS_INFO_TRACE("OCX Address: %x,UnifiedQuery: %s", this,pUnifiedQuery);

    CString strResult = COCXUserInfoMgr::GetUserDeviceList(m_iSDKSessionID, pUnifiedQuery);
    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SignOutUser(LPCTSTR pDomainCode, ULONG ulUserID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,UserID: %lu", this,ulUserID);

    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserInfoMgr::SignOutUser(m_iSDKSessionID, pDomainCode, ulUserID);
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SignOutUserByRole(LPCTSTR pDomainCode, ULONG ulRoleID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,RoleID: %lu", this,ulRoleID);

    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserInfoMgr::SignOutUserByRole(m_iSDKSessionID, pDomainCode, ulRoleID);
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SignOutOnlineUser(LPCTSTR pDomainCode, ULONG ulUserOnlineID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,UserOnlineID: %lu", this,ulUserOnlineID);

    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserInfoMgr::SignOutOnlineUser(m_iSDKSessionID, pDomainCode, ulUserOnlineID);
    return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddRole(LPCTSTR pRoleInfo)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,RoleInfo: %s", this,pRoleInfo);

    CString strResult = COCXRoleMgr::AddRole(m_iSDKSessionID, pRoleInfo);
    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteRole(LPCTSTR pDomainCode, ULONG ulRoleID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,RoleID: %lu", this,ulRoleID);

    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXRoleMgr::DeleteRole(m_iSDKSessionID, pDomainCode, ulRoleID);
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyRole(LPCTSTR pRoleInfo)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,RoleInfo: %s", this,pRoleInfo);

    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXRoleMgr::ModifyRole(m_iSDKSessionID, pRoleInfo);
    return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRoleList(LPCTSTR pDomainCode)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);

    CString strResult = COCXRoleMgr::GetRoleList(m_iSDKSessionID, pDomainCode);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRoleInfoByRoleID(LPCTSTR pDomainCode, ULONG ulRoleID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,RoleID: %lu", this,ulRoleID);

    CString strResult = COCXRoleMgr::GetRoleInfoByRoleID(m_iSDKSessionID, pDomainCode, ulRoleID);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRoleInfoByUserID(LPCTSTR pDomainCode, ULONG ulUserID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,UserID: %lu", this,ulUserID);
    CString strResult = COCXRoleMgr::GetRoleInfoByUserID(m_iSDKSessionID, pDomainCode, ulUserID);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDefaultRoleAccountRule(LPCTSTR pDomainCode)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
    CString strResult = COCXRoleMgr::GetDefaultRoleAccountRule(m_iSDKSessionID, pDomainCode);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetOperationRightGroup(LPCTSTR pDomainCode)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
    CString strResult = COCXRoleMgr::GetOperationRightGroup(m_iSDKSessionID, pDomainCode);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddUserGroup(LPCTSTR pUserGroup)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pUserGroup)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Group Failed.", "Reason:pUserGroup is NULL");
		return NULL;
	}
    IVS_INFO_TRACE("OCX Address: %x,UserGroup: %s", this,pUserGroup);

    CString strResult = COCXUserGroupMgr::AddUserGroup(m_iSDKSessionID, pUserGroup);
    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteUserGroup(LPCTSTR pDomainCode, ULONG ulUserGroupID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,UserGroupID: %lu", this,ulUserGroupID);
    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserGroupMgr::DeleteUserGroup(m_iSDKSessionID, pDomainCode, ulUserGroupID);
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyUserGroup(LPCTSTR pUserGroup)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pUserGroup)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Modify User Group Failed.", "Reason:pUserGroup is NULL");
		return IVS_PARA_INVALID;
	}
    IVS_INFO_TRACE("OCX Address: %x,UserGroup: %s", this,pUserGroup);
    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserGroupMgr::ModifyUserGroup(m_iSDKSessionID, pUserGroup);
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_MoveUserGroup(LPCTSTR pUserGroup)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pUserGroup)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Move User Group Failed.", "Reason:pUserGroup is NULL.");
		return IVS_PARA_INVALID;
	}
    IVS_INFO_TRACE("OCX Address: %x,UserGroup: %s", this,pUserGroup);
    IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUserGroupMgr::MoveUserGroup(m_iSDKSessionID, pUserGroup);
    return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetUserGroupList(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get User Group List Failed.", "Reason:pReqXml is NULL");
		return NULL;
	}
    IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
    CString strResult = COCXUserGroupMgr::GetUserGroupList(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_StopAllRealPlay(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
    if (!m_bIsInit)
    {
        return IVS_FAIL;
    }
    CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);

    ULONG ulRet = ((CVideoPaneMgr*)m_pVideoPaneMgr)->StopAllRealPlay();
    return ulRet;
}
//**********************************************
// Method:    IVS_OCX_SetWndLayout
// FullName:  CIVS_OCXPlayerCtrl::IVS_OCX_SetWndLayout
// Access:    protected
// Returns:   LONG
// Parameter: ULONG lWindowsNum
// Author:    yangdazhao ykf75928
// Modified:	  21:11:2012  by yangdazhao ykf75928
//**********************************************
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetWndLayout(ULONG lWindowsNum)
{
	IVS_INFO_TRACE("OCX Address: %x,WindowsNum: %lu", this,lWindowsNum);
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);

	/*
    if (!LayoutMgr.CheckLayoutType(lWindowsNum))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Wnd Layout Failed.", "Reason:OCX set window layout error,layout type invalid.");
        return IVS_PARA_INVALID;
    }*///多余代码

    //从第一个窗格开始分配位置与大小
    CRect rect;
    GetClientRect(&rect);
	BP_RUN_LOG_INF("Set Wnd Layout","Rect %d %d %d %d",rect.left,rect.top,rect.Width(),rect.Height());
    //调用管理类的设置窗格布局
	//CWnd * pParent = m_pVideoPaneMgr->GetParentWnd();
	//CHECK_POINTER(pParent,IVS_PARA_INVALID);
	//pParent->Invalidate(TRUE);
	//pParent->UpdateWindow();

    if (IVS_SUCCEED != m_pVideoPaneMgr->SetWndLayout(rect, lWindowsNum))
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Set WndLayout Failed.", "Reason:OCX set window layout error.");
        return IVS_FAIL;
    }

	m_pVideoPaneMgr->ResetTVWallStatus();

	//GetClientRect(rect);//重复代码
	//if (IVS_SUCCEED != m_pVideoPaneMgr->MoveOCXToRect(rect))
	//{
	//	BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Set WndLayout Failed.", "Reason:Move OCX To Rect error.");
	//	return IVS_FAIL;
	//}

    return IVS_SUCCEED;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetDeviceName(ULONG ulWndID, LPCTSTR pDevName)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pDevName,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,WndID: %lu,DevName: %s", this,ulWndID,pDevName);
    //第三个判断为传入的设备名称为空
    if (*pDevName == 0)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Device Name Failed.","Reason:DevName is Empty.");
        return IVS_PARA_INVALID;
    }

    ULONG ulRet = m_pVideoPaneMgr->SetDevName(ulWndID, pDevName);
    return static_cast<IVS_INT32>(ulRet);
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetTitlebar(LONG lShowStatus)const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return 0;
}//lint !e715

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_SetColor(ULONG ulColor)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    m_ulColor = static_cast<ENUMDEF::COLOR_MODE>(ulColor);

    return IVS_SUCCEED;
}

// 双击窗格放大或缩小
void CIVS_OCXPlayerCtrl::PostMagnifyOrMinifyEventMsg(ULONG ulMsgSeq, ULONG ulWinID, ULONG ulIsMagnify)
{
    CString xmlToCU;

    xmlToCU = "";
// TODO: GetMagnifyOrMinifyEventMsgXML(xmlToCU,ulWinID);
    if (MAGNIFY == ulIsMagnify)
    {
        IVS_OCX_Windows_Event(EVENT_MAGNIFY_WINDOW,(LPCTSTR)xmlToCU);
    }
    else
    {
        IVS_OCX_Windows_Event(EVENT_MINIFY_WINDOW,(LPCTSTR)xmlToCU);
    }
}//lint !e715

// 窗口交换事件上报
void CIVS_OCXPlayerCtrl::PostExchangeWndEventMsg(ULONG ulMsgSeq, ULONG ulSrcWnd, ULONG ulDstWnd)
{
    CString xmlToCU;

    xmlToCU ="";
    PostEventXMLImpl.GetExchangeWndEventMsgXML(xmlToCU, ulSrcWnd, ulDstWnd);
    IVS_OCX_Windows_Event(EVENT_EXCHANGE_WINDOW, (LPCTSTR)xmlToCU);

	//停止实况或者录像上电视墙
	CHECK_POINTER_VOID(m_pVideoPaneMgr);
	IVS_ULONG ulTVWallID = m_pVideoPaneMgr->GetCurTVWallID();
	if(ulTVWallID > 0)
	{
		//获取当前是实况上墙还是录像上墙
		WND_TYPE emType = m_pVideoPaneMgr->GetTVWallWndType();

		//置两个窗口电视墙状态图标为离线
		CVideoPane* srcPane = GetVideoPanel(ulSrcWnd);
		if(srcPane)
		{
			srcPane->SetTVWallStatusBarStatus(TVWALL_BAR_STAUS_OFFLINE);
			srcPane->SetTVWallStatus(TVWALL_STATUS_NONE);	
			srcPane->SetAlarmButtonStatus((unsigned long long)1);
		}
		CVideoPane* dstPane = GetVideoPanel(ulDstWnd);
		if(dstPane)
		{
			dstPane->SetTVWallStatusBarStatus(TVWALL_BAR_STAUS_OFFLINE);
			dstPane->SetTVWallStatus(TVWALL_STATUS_NONE);	
			dstPane->SetAlarmButtonStatus((unsigned long long)1);
		}

		//判断是否要启动上墙
		OnWallPara *paraSrc =  m_mapOnWall[ulSrcWnd];
		OnWallPara *paraDst =  m_mapOnWall[ulDstWnd];

		if(paraSrc != NULL && paraDst != NULL)//原本两个窗格都已经上墙，则不进行下墙，直接交换上墙即可
		{
			//把参数保存下来
			CString strSrcCamera = paraSrc->strCamera;
			CString strSrcPara = paraSrc->strPara;
			CString strDstCamera = paraDst->strCamera;
			CString strDstPara = paraDst->strPara;

			//删除原始的数据
			IVS_DELETE(paraSrc);
			m_mapOnWall.erase(ulSrcWnd);
			IVS_DELETE(paraDst);
			m_mapOnWall.erase(ulDstWnd);

			//交换上墙电视
			if(WND_TYPE_REALTIME == emType)
			{
				(void)IVS_OCX_StartRealPlayTVWall(ulTVWallID,strSrcCamera,strSrcPara,ulDstWnd);
				(void)IVS_OCX_StartRealPlayTVWall(ulTVWallID,strDstCamera,strDstPara,ulSrcWnd);	
			}
			else if(WND_TYPE_RECORD == emType)
			{
				(void)IVS_OCX_StartPlatformPlayBackTVWall(ulTVWallID,strSrcCamera,strSrcPara,ulDstWnd);
				(void)IVS_OCX_StartPlatformPlayBackTVWall(ulTVWallID,strDstCamera,strDstPara,ulSrcWnd);
			}
	

			BP_RUN_LOG_INF("Exchange TVWall Window Pane End.","SrcPara and DstPara are not NULL.");
		}
		else if(paraSrc != NULL && paraDst == NULL)//一个窗格上墙，另一个窗格未上墙，则一个停止上墙，另一个触发上墙
		{
			//把src参数保存下来
			CString strSrcCamera = paraSrc->strCamera;
			CString strSrcPara = paraSrc->strPara;

			//删除SRC
			IVS_DELETE(paraSrc);
			m_mapOnWall.erase(ulSrcWnd);

			if(WND_TYPE_REALTIME == emType)
			{
				(void)SynStopRealPlayTVWall(ulTVWallID,ulSrcWnd);
				(void)IVS_OCX_StartRealPlayTVWall(ulTVWallID,strSrcCamera,strSrcPara,ulDstWnd);
			}
			else if(WND_TYPE_RECORD == emType)
			{
				(void)IVS_OCX_StopPlatformPlayBackTVWall(ulTVWallID,ulSrcWnd);
				(void)IVS_OCX_StartPlatformPlayBackTVWall(ulTVWallID,strSrcCamera,strSrcPara,ulDstWnd);
			}

			BP_RUN_LOG_INF("Exchange TVWall Window Pane End.","SrcPara is not NULL,But DstPara is NULL.");
		}
		else if(paraSrc == NULL && paraDst != NULL)//情况同上
		{
			//保存dst参数
			CString strDstCamera = paraDst->strCamera;
			CString strDstPara = paraDst->strPara;

			//删除dst参数
			IVS_DELETE(paraDst);
			m_mapOnWall.erase(ulDstWnd);

			if(WND_TYPE_REALTIME == emType)
			{
				(void)SynStopRealPlayTVWall(ulTVWallID,ulDstWnd);
				(void)IVS_OCX_StartRealPlayTVWall(ulTVWallID,strDstCamera,strDstPara,ulSrcWnd);
			}
			else if(WND_TYPE_RECORD == emType)
			{
				(void)IVS_OCX_StopPlatformPlayBackTVWall(ulTVWallID,ulDstWnd);
				(void)IVS_OCX_StartPlatformPlayBackTVWall(ulTVWallID,strDstCamera,strDstPara,ulSrcWnd);
			}
			BP_RUN_LOG_INF("Exchange TVWall Window Pane End.","SrcPara is NULL,But DstPara is not NULL.");
		}
		else//两个窗格均为上墙，无须进行交换上墙操作
		{
			BP_RUN_LOG_INF("Exchange TVWall Window Pane End.","Both SrcPara and DstPara are NULL.");
		}
		
		RefreshOnlineTVWallStatus();
		m_pVideoPaneMgr->UpdateTVWallPaneStatus();
	}
}//lint !e715

//
//void CIVS_OCXPlayerCtrl::UpdateSynWndID(ULONG ulActivePaneID, ULONG ulMouseUpPaneID)
//{
//}//lint !e715

// GetVideoPanel
// 根据窗口ID获取视频窗口类 add by w00210470
CVideoPane *CIVS_OCXPlayerCtrl::GetVideoPanel(ULONG ulWndID)
{
    ASSERT(NULL != m_pVideoPaneMgr);
	if (NULL == m_pVideoPaneMgr)   //lint !e774
	{
		return NULL;
	}
	return m_pVideoPaneMgr->GetWndPointerByID(ulWndID);
}

void CIVS_OCXPlayerCtrl::PostOcxEventMsg( const char * pMsg, ULONG ulMsgSeq, ULONG ulResultCode, const std::string &strCameraCode )const
{
    CString xmlToCU;
    xmlToCU.Empty();

	//string _strCameraCode = strCameraCode;

    //CManageXML::GetOcxEventMsgXML(xmlToCU, pMsg, ulMsgSeq, ulResultCode, strCameraCode);

    BP_RUN_LOG_INF("Post OCX EventMsg End", "xml=%s", xmlToCU.GetBuffer(0));
} //lint !e715

void CIVS_OCXPlayerCtrl::PostOcxEventMsg( const char * pMsg, ULONG ulMsgSeq, ULONG ulResultCode, ULONG ulWndID )const
{
    TCHAR str[32] = {0x0};
    (void)itoa((int)ulMsgSeq,str,10);
    PostOcxEventMsgStr(pMsg, str, ulResultCode, ulWndID);
}

void CIVS_OCXPlayerCtrl::PostOcxEventMsgStr( const char * pMsg, const char *pMsgSeq, ULONG ulResultCode, ULONG ulWndID )const
{
    CHECK_POINTER_VOID(pMsg);
	CHECK_POINTER_VOID(pMsgSeq);
	BP_RUN_LOG_INF("Post OCX EventMsg Start", "msg=%s", pMsg);

    std::string xmlToCU;
    PostEventXMLImpl.GetOcxEventMsgStrXML(xmlToCU, pMsg, pMsgSeq, ulResultCode, ulWndID);

    //上报馈事件给CU

   BP_RUN_LOG_INF("Post OCX EventMsg End", "xml=%s", xmlToCU.c_str());
}

void CIVS_OCXPlayerCtrl::PostGetPlayInfoEventMsg(ULONG ulWinID)
{
    CString strResult;
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Get Media Info Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return;
	}

	CVideoPane* pWnd = m_pVideoPaneMgr->GetWndPointerByID(ulWinID);
	if (NULL == pWnd)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get Media Info Failed.","Reason:Get CVideoPane Error");
		return;
	}
	CPoint pointXY =  pWnd->GetInfoButtonRightBottomPosition();

    COCXPlayBackXMLProcess::GetMediaInfoXML(strResult,ulWinID,pointXY);
    IVS_OCX_Windows_Event(EVENT_VIDEO_INFO_BTN_CLICK,(LPCTSTR)strResult);
    return;
}

void CIVS_OCXPlayerCtrl::PostActiveWndChangeEventMsg( ULONG ulMsgSeq, ULONG ulWinID )
{
    CString xmlToCU("");
    PostEventXMLImpl.GetActiveWndChangeEventMsgXML(xmlToCU,ulWinID);
    IVS_OCX_Windows_Event(EVENT_ACTIVE_WINDOW_CHANGE,(LPCTSTR)xmlToCU);
    return;
}//lint !e715

void CIVS_OCXPlayerCtrl::PostPointMoveOutofOCX(ULONG ulWinID)
{
	CString xmlToCU("");
	PostEventXMLImpl.GetActiveWndChangeEventMsgXML(xmlToCU,ulWinID);
	IVS_OCX_Windows_Event(EVENT_POINT_IS_OUTOF_OCX,(LPCTSTR)xmlToCU);
	return;
}

void CIVS_OCXPlayerCtrl::PostOcxAddRecordLabel( ULONG ulMsgSeq, char * pCameraCode, char * pTime )const
{
} //lint !e818 !e715

void CIVS_OCXPlayerCtrl::PostOcxStopRecordReport( ULONG ulWndID,ULONG ulRecordRet )const
{
}//lint !e715

void CIVS_OCXPlayerCtrl::PostLeftMouseClickEventMsg( ULONG ulMsgSeq, ULONG ulWinID, ULONG ulX, ULONG ulY )
{
    CString xmlToCU;
    xmlToCU = "";
    PostEventXMLImpl.GetMouseClickEventXML(xmlToCU, ulWinID, ulX, ulY);
    IVS_OCX_Windows_Event(EVENT_MOUSE_LEFT_BUTTON_CLICK,(LPCTSTR)xmlToCU);
    return;
}//lint !e715
                                                    
void CIVS_OCXPlayerCtrl::PostEnterFullScreenEventMsg(LPCTSTR pMsg, ULONG ulResultCode, ULONG ulWndID)
{
    std::string xmlToCU;
    PostEventXMLImpl.GetFullSCreenXML(xmlToCU, ulResultCode, ulWndID);
    IVS_OCX_Windows_Event(EVENT_ENTER_FULL_SCREEN,xmlToCU.c_str());
}//lint !e715

void CIVS_OCXPlayerCtrl::PostExitFullScreenEventMsg(LPCTSTR pMsg, ULONG ulResultCode, ULONG ulWndID)
{
    std::string xmlToCU;
    PostEventXMLImpl.GetFullSCreenXML(xmlToCU, ulResultCode, ulWndID);
    IVS_OCX_Windows_Event(EVENT_EXIT_FULL_SCREEN, xmlToCU.c_str());
}//lint !e715

void CIVS_OCXPlayerCtrl::PostRightMouseClickEventMsg( ULONG ulMsgSeq, ULONG ulWinID, ULONG ulX, ULONG ulY )
{
    CString xmlToCU;
    xmlToCU = "";
    PostEventXMLImpl.GetMouseClickEventXML(xmlToCU, ulWinID, ulX, ulY);
    IVS_OCX_Windows_Event(EVENT_MOUSE_RIGHT_BUTTON_CLICK,(LPCTSTR)xmlToCU);
    return;
}//lint !e715

void CIVS_OCXPlayerCtrl::PostTurnCruisePreEventMsg(ULONG ulWinID)
{
	CString strResult;
	COCXPlayBackXMLProcess::GetWndIDXML(strResult,ulWinID);
	IVS_OCX_Windows_Event(EVENT_BUTTON_SET_TURNCRUISE_PRE,(LPCTSTR)strResult);
}

void CIVS_OCXPlayerCtrl::PostTurnCruiseNextEventMsg(ULONG ulWinID)
{
	CString strResult;
	COCXPlayBackXMLProcess::GetWndIDXML(strResult,ulWinID);
	IVS_OCX_Windows_Event(EVENT_BUTTON_DEL_TURNCRUISE_NEXT,(LPCTSTR)strResult);
}


void CIVS_OCXPlayerCtrl::PostEventMsg( char* pMsg, HWND hWnd, ULONG ulMsgType, ULONG ulExceptionCode )const
{
} //lint !e818 !e715 MFC自动生成的的函数

// ULONG ulBarType 1 CTranAnalysisToolBar 2 CTranVideoRealtimeBar
void CIVS_OCXPlayerCtrl::PostToolBarClickEventMsg( ULONG ulBarType,ULONG ulBtnTtype , ULONG ulWndID ,std::string strBtnStatus)
{
	std::string strName;
	PostEventXMLImpl.GetBtnName(strName,ulBarType,(ULONGLONG)ulBtnTtype);
	std::string xmlToCU;
	PostEventXMLImpl.GetToolBarClickEventMsg(xmlToCU,strName, ulWndID,strBtnStatus);
	IVS_OCX_Windows_Event(EVENT_TOOLBAR_BTN_CLICK,xmlToCU.c_str());
}//lint !e1746

void CIVS_OCXPlayerCtrl::PostToolBarEMapClickEventMsg(ULONG ulBarType,ULONG ulBtnTtype , ULONG ulWndID,CRect& rect)
{
	std::string strName;
	PostEventXMLImpl.GetBtnName(strName,ulBarType,(ULONGLONG)ulBtnTtype);
	std::string xmlToCU;
	PostEventXMLImpl.GetToolBarEMapClickEventMsg(xmlToCU,strName, ulWndID,rect);
	IVS_OCX_Windows_Event(EVENT_TOOLBAR_BTN_CLICK,xmlToCU.c_str());
}

void CIVS_OCXPlayerCtrl::PostStopButtonClickMsg( ULONG ulWndID )
{
    CString xmlToCU;
    PostEventXMLImpl.GetStopButtonClickMsgXML(xmlToCU, ulWndID);
    IVS_OCX_Windows_Event(EVENT_VIDEO_STOP_BTN_CLICK,(LPCTSTR)xmlToCU);
    return;
}

CIVS_OCXPlayerCtrl* CIVS_OCXPlayerCtrl::GetOcxPlayerCtrl( ULONG ulWndType )const
{
    CIVS_OCXPlayerCtrl *pOcxPlayer = nullptr;
	CAutoLock __Locker(m_csOcxListLock); //lint !e613

	std::list<CIVS_OCXPlayerCtrl *>::iterator iter = m_OcxList.begin();
    for (; iter!=m_OcxList.end(); iter++)
    {
        CIVS_OCXPlayerCtrl *pOcx = *iter;
        if (NULL != pOcx->GetVideoPaneMgr())
        {
            if (pOcx->GetVideoPaneMgr()->GetWndType() == ulWndType)//lint !e613
            {
                pOcxPlayer = pOcx;
                break;
            }
        }
    }
    return pOcxPlayer;
}

// ReportStopPlayEvent
// 上报播放停止事件 add by w00210470
void CIVS_OCXPlayerCtrl::ReportStopPlayEvent(ULONG ulWndID)
{
	CString cstrXmlToCU;
	COCXPlayBackXMLProcess::GetStreamExceptionMsgXML(cstrXmlToCU, ulWndID, 0, 0);
	IVS_OCX_Event(IVS_EVENT_REMOTE_PLAYBACK_FAILED, (LPCTSTR)cstrXmlToCU);
}

//停止窗口实况播放
ULONG CIVS_OCXPlayerCtrl::StopRealPlayByPane(CVideoPane *pVideoPane)
{
    IVS_INFO_TRACE("OCX Address: %x", this);
	ASSERT(NULL != pVideoPane);
    CHECK_POINTER(pVideoPane, IVS_OPERATE_MEMORY_ERROR); //lint !e774

	/*
	 * DTS2014021302165 
	 * 去除panel的全局注册
	 */
	CGlobalSoundMgr &SoundMgr = CGlobalSoundMgr::Instance();
	if (SoundMgr.GetSoundingPanel() == pVideoPane)
	{
		SoundMgr.Reset();
	}

	//停止局部放大,这个不能放在后面，放在后面，按钮无法响应 add by z00193167
	if (MAGNIFY_OPEN == pVideoPane->GetRealtimeMagnifyState())
	{
		// 发送一个点击事件给放大按钮，让他关闭自己，这里使用同步的sendmessage,不要使用postmessage
		::SendMessage(pVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_MAGNIFY, BN_CLICKED), (LPARAM)pVideoPane->GetVideoRealtimeBar()->GetMagnifyBtnHwnd());
	}

	//停止语音对讲 add by z00193167
	if (BOTTOMBAR_TALKBACK_OPEN == pVideoPane->GetRealtimeTalkbackState())
	{
		// 发送一个点击事件给放大按钮，让他关闭自己，这里使用同步的sendmessage,不要使用postmessage
	//	::SendMessage(pVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_TALKBACK, BN_CLICKED), (LPARAM)pVideoPane->GetVideoRealtimeBar()->GetTalkbackBtnHwnd());
 		pVideoPane->SetRealtimeTalkbackState(BOTTOMBAR_TALKBACK_CLOSE);
       // pVideoPane->GetVideoRealtimeBar()->StopTalkbackSound();
 		// pVideoPane->GetVideoRealtimeBar()->DeleteTalkbackCtrl();
	}

	//停止随路语音 add by z00193167
	if (BOTTOMBAR_SOUND_OPEN == pVideoPane->GetRealtimeSoundState())
	{
		// 发送一个点击事件给放大按钮，让他关闭自己，这里使用同步的sendmessage,不要使用postmessage
//		::SendMessage(pVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_TALKBACK, BN_CLICKED), (LPARAM)pVideoPane->GetVideoRealtimeBar()->GetTalkbackBtnHwnd());
 		pVideoPane->GetVideoRealtimeBar()->StopSound();
 		pVideoPane->GetVideoRealtimeBar()->DeleteSoundCtrl();
	}

	// 关闭云镜 add by z00193167
	if (BOTTOMBAR_PTZ_OPEN == pVideoPane->GetRealtimePtzState())
	{
		// 发送一个点击事件给放大按钮，让他关闭自己，这里使用同步的sendmessage,不要使用postmessage
		::SendMessage(pVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_PTZ, BN_CLICKED), (LPARAM)pVideoPane->GetVideoRealtimeBar()->GetPTZBtnHwnd());
	}

    if (VIDEO_PLAY_STATE_NOT_PLAYING == pVideoPane->GetPlayState() && 
		pVideoPane->GetServiceType() != SERVICE_TYPE_REALVIDEO)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Stop Real Play By Pane Failed.", "Reason:the window is not playing");
		pVideoPane->ResetPane();
		return IVS_SUCCEED;
    }

    ULONG Handle = pVideoPane->GetPlayHandle();
	//ASSERT(0 != Handle);
	if (0 == Handle &&  pVideoPane->GetServiceType() != SERVICE_TYPE_REALVIDEO) //lint !e774 !e944
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Stop Real Play By Pane Failed.","Reason:Try to Stop PANEL:%08x with Handle=0", (ULONG)pVideoPane);
		pVideoPane->ResetPane();
		return IVS_PARA_INVALID;
	}

	// 如果原来被占用成了放大，先删掉这个放大，等停止后再恢复一个窗口回去
// 	IVS_RECT stIArectPercent;
// 	memset(&stIArectPercent,0,sizeof(IVS_RECT));
	//bool bOpen = false;
	//CVideoPane* pMagnifySrcPane = NULL;
	if (BOTTOMBAR_MAGNIFY_OPEN == pVideoPane->GetDstMagnifyState())
	{
		//bOpen = true;
		//stIArectPercent = pVideoPane->GetIArectPercent();
		//pMagnifySrcPane = pVideoPane->GetMagnifySrcPane();
		// 删除掉源窗口的信息
		::SendMessage(pVideoPane->GetMagnifySrcPane()->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (LONG)(pVideoPane));
	}

	// 停止轨迹叠加
	(void)pVideoPane->DisableOverlay(m_iSDKSessionID, Handle);

    IVS_INT32 ulRet = IVS_FAIL;
    IVS_SERVICE_TYPE enServiceType = pVideoPane->GetServiceType();
    if (SERVICE_TYPE_REALVIDEO == enServiceType)
    {

		if (pVideoPane->GetIsLocalRecord())
		{
			// 若本地录像打开，且由本窗口触发则关闭本地录像
			if (pVideoPane->GetIsLocalRecordTrigger())
			{
				ulRet = IVS_OCX_StopLocalRecord(static_cast<LONG>(pVideoPane->GetPaneID()));
				PostLocRecEventGeneric(1, (int)pVideoPane->GetPaneID()); // 1 为录像停止
				Sleep(200); // 等待最后一个录像结束时间上报
			}
			pVideoPane->GetVideoRealtimeBar()->SetRecordBtnState(false);
		}

		pVideoPane->StopRealStreamSync();
#if 0
		CGlobalPlayMgr &playMgr = CGlobalPlayMgr::Instance();
		if (0 == playMgr.UnRegPlayHandle(Handle, pVideoPane))
		{
			// 引用计数减为0, 关闭视频
			IVS_SDK_StopRealPlay(m_iSDKSessionID, Handle);
		}
		else
		{
			IVS_SDK_DelPlayWnd(m_iSDKSessionID, Handle, pVideoPane->GetVideoWnd());
		}

        // 如果当前视频打开随路语音，要关闭
        if (pVideoPane->IsSoundOn())
        {
            IVS_SDK_StopSound(m_iSDKSessionID, Handle);
        }

		//TODO Reset函数
		pVideoPane->SetPlayHandle(0);
		pVideoPane->ResetPane();
#endif 

    }
    else if (SERVICE_TYPE_PLAYBACK == enServiceType)
    {
		if (pVideoPane->IsSyncPlayBack())
		{
			CSyncPlayBackMgr &mgr = CSyncPlayBackMgr::Instance();
			(void)mgr.DeleteSyncPanel(pVideoPane);
			pVideoPane->SetSyncPlayBackMode(false);
			ulRet  = IVS_SUCCEED;
		}
		else
		{
			if (pVideoPane->GetIsPlatRecord())
			{
				ulRet = IVS_SDK_StopPlatformPlayBack(m_iSDKSessionID, pVideoPane->GetPlayHandle());
			}
			else
			{
				ulRet = IVS_SDK_StopLocalPlayBack(m_iSDKSessionID, pVideoPane->GetPlayHandle());
			}
			
			CGlobalPlayMgr &playMgr = CGlobalPlayMgr::Instance();
		    (void)playMgr.UnRegPlayHandle(Handle, pVideoPane);
		}
        if (IVS_SUCCEED != ulRet)
        {
            BP_RUN_LOG_ERR(ulRet, "IVS OCX Stop Platform PlayBack By Pane Failed.", "Reason:stop from SDK fail.");
        }
		pVideoPane->SetPlayHandle(0);
    }
	else if (SERVICE_TYPE_BACKUPRECORD_PLAYBACK == enServiceType)
	{
		if (pVideoPane->IsSyncPlayBack())
		{
			CSyncPlayBackMgr &mgr = CSyncPlayBackMgr::Instance();
			(void)mgr.DeleteSyncPanel(pVideoPane);
			pVideoPane->SetSyncPlayBackMode(false);
			ulRet  = IVS_SUCCEED;
		}
		else
		{
			ulRet = IVS_SDK_StopBackupPlayBack(m_iSDKSessionID, pVideoPane->GetPlayHandle());
		}
		if (IVS_SUCCEED != ulRet)
		{
			BP_RUN_LOG_ERR(ulRet, "IVS OCX Stop Backup PlayBack By Pane Failed.", "Reason:stop from SDK fail.");
		}
		pVideoPane->SetPlayHandle(0);
	}
	else if (SERVICE_TYPE_DISASTEBACKUP_PLAYBACK == enServiceType)
	{
		if (pVideoPane->IsSyncPlayBack())
		{
			CSyncPlayBackMgr &mgr = CSyncPlayBackMgr::Instance();
			(void)mgr.DeleteSyncPanel(pVideoPane);
			pVideoPane->SetSyncPlayBackMode(false);
			ulRet  = IVS_SUCCEED;
		}
		else
		{
			ulRet = IVS_SDK_StopDisasterRecoveryPlayBack(m_iSDKSessionID, pVideoPane->GetPlayHandle());
		}
		if (IVS_SUCCEED != ulRet)
		{
			BP_RUN_LOG_ERR(ulRet, "IVS OCX Stop Disaster PlayBack By Pane Failed.", "Reason:stop from SDK fail.");
		}

		pVideoPane->SetPlayHandle(0);
	}
    else if (SERVICE_TYPE_PU_PLAYBACK == enServiceType)
    {
        ulRet = IVS_SDK_StopPUPlayBack(m_iSDKSessionID, pVideoPane->GetPlayHandle());

        if (IVS_SUCCEED != ulRet)
        {
            BP_RUN_LOG_ERR(ulRet, "IVS OCX Stop PU PlayBack By Pane Failed.", "Reason:stop from SDK fail.");
        }
		pVideoPane->SetPlayHandle(0);
    }
    else
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Stop Real Play By Pane Failed.", "Reason:servicetype error, Videopane's servicetype = %d", (int)enServiceType);
    }


	// 上报播放停止事件
	ReportStopPlayEvent(pVideoPane->GetPaneID());
    
    //pVideoPane->SetPlayHandle(0);
    pVideoPane->SetServiceType(SERVICE_TYPE_REALVIDEO);// 默认初始化为实况;

    //如果是由于控件cleanUp而触发的stopplay，则不会重置窗格
    if (m_bIsInit)
    {
        // 把对应窗格状态清空
        if ((!pVideoPane->GetResumeState())
            && (WND_TYPE_NORMAL != pVideoPane->GetWndType()))
        {
            pVideoPane->SetTitleButtonPic(false);
        }
        pVideoPane->ResetPane();
        pVideoPane->SetRealtimePlayPic(BUTTON_UNABLE);
        pVideoPane->SetCameraType(CAMERA_TYPE_CAVITY_FIXED);
    }

	// 如果自己是被占用成放大窗口，需要重新加一个放大的加上去，避免播放结束关闭完成了之后，放大窗口消失掉
// 	if (bOpen)
// 	{
// 		// 从新画一个上去
// 		CRect IArectPercent;
// 		IArectPercent.bottom = stIArectPercent.bottom;
// 		IArectPercent.left = stIArectPercent.left;
// 		IArectPercent.right = stIArectPercent.right;
// 		IArectPercent.top = stIArectPercent.top;	
// 		if (NULL != pMagnifySrcPane)
// 		{
// 			pMagnifySrcPane->ResumeMagnifyRectByPercent(IArectPercent);
// 		}
// 	}

	return IVS_SUCCEED;
}

int CIVS_OCXPlayerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (COleControl::OnCreate(lpCreateStruct) == -1)
        return -1;

    return IVS_SUCCEED;
}

void CIVS_OCXPlayerCtrl::OnSize(UINT nType, int cx, int cy)
{
	//IVS_INFO_TRACE("OCX Address: %x", this);
    COleControl::OnSize(nType, cx, cy);
	//判断是否已经初始化，未初始化则不响应该动作
	if (!m_bIsInit)
	{
        //del by w00210470 
        //BP_RUN_LOG_INF("IVS_OCX::","CIVS_OCXPlayerCtrl::OnSize End m_bIsInit == FALSE");
		return;
	}

	PostMessage(WM_RESET_WNDLAYOUT,0,0);

	//CHECK_POINTER_VOID(m_pVideoPaneMgr);

	//CRect rect;
	//GetClientRect(rect);

	//if (MAGNIFY == m_pVideoPaneMgr->GetMagnifyState())
	//{
	//	m_pVideoPaneMgr->Magnify(rect);
	//	return;
	//}

	//// 先获取窗格布局类型
	//IVS_WIND_LAYOUT winLayoutType = m_pVideoPaneMgr->GetWindLayOut();

	//// 调用设置窗格布局，重绘所有窗格
	//if (IVS_SUCCEED != m_pVideoPaneMgr->SetWndLayout(rect, winLayoutType))
	//{
	//	BP_RUN_LOG_ERR(IVS_FAIL, "", "OCX change size error,set layout type error.");
	//	return;
	//}
}

// Method:    OnEraseBkgnd
// Qualifier: It is called to prepare an invalidated region for painting.
BOOL CIVS_OCXPlayerCtrl::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
} //lint !e818 !e715 !e1762 MFC自动生成的的函数

void CIVS_OCXPlayerCtrl::OnDestroy()
{
    COleControl::OnDestroy();
    (void)IVS_OCX_Cleanup();
}

// Qualifier: 键盘按键弹起事件
void CIVS_OCXPlayerCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar)
    {
    case VK_F8:
        {
            CHECK_POINTER_VOID(m_pVideoPaneMgr);
            ((CVideoPaneMgr*)m_pVideoPaneMgr)->SetFullScreen();
            break;
        }

    case VK_ESCAPE:
        {
            CHECK_POINTER_VOID(m_pVideoPaneMgr);
            ((CVideoPaneMgr*)m_pVideoPaneMgr)->ResetFullScreen();
            break;
        }
    default:
		break;
    }

    COleControl::OnKeyUp(nChar, nRepCnt, nFlags);
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnExceptionCallback(WPARAM wParam, LPARAM lParam)
{
    return 0;
}//lint !e715 !e1762

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnTalkbackFailedCallBack(WPARAM wParam, LPARAM lParam)
{
    // 对讲异常上报
    CXml xml;
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.IntoElem();
    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue("1"); // 1：平台语音对讲异常
    xml.OutOfElem();
    xml.OutOfElem();
    unsigned int uiLent = 0;
    const char* pszResult = xml.GetXMLStream(uiLent);
    if (NULL != pszResult)
    {
        IVS_OCX_Event((long)wParam, (LPCTSTR)pszResult);
    }
    
    STREAM_EXCEPTION_INFO* pExceptionInfo = (STREAM_EXCEPTION_INFO*)(lParam);
    IVS_DELETE(pExceptionInfo);

    BP_RUN_LOG_INF("TalkBack Fail CallBack", "report string:%s", pszResult);
    return 0;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnBroadcastFailedCallBack(WPARAM wParam, LPARAM lParam)
{

     //IVS_INT32 iRet = IVS_FAIL;
     STREAM_EXCEPTION_INFO* pExceptionInfo = (STREAM_EXCEPTION_INFO*)(lParam);
     
     if (NULL != pExceptionInfo)
     {
         CXml xml;
         (void)xml.AddDeclaration("1.0","UTF-8","");
         (void)xml.AddElem("Content");
         (void)xml.IntoElem();
         (void)xml.AddChildElem("CameraCode");
         (void)xml.IntoElem();
         (void)xml.SetElemValue(pExceptionInfo->szDeviceCode);
         xml.OutOfElem();
         xml.OutOfElem();
         unsigned int uiLent = 0;
         const char* pszResult = xml.GetXMLStream(uiLent);
         if (NULL != pszResult)
         {
             IVS_OCX_Event((long)wParam, (LPCTSTR)pszResult);
         }
         IVS_DELETE(pExceptionInfo);
         BP_RUN_LOG_INF("BroadCast Fail CallBack", "report string:%s", pszResult);
         return IVS_SUCCEED;
     }
     BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"IVS OCX Process BroadcastFailed CallBack.","Reason:pExceptionInfo is NULL");
     return IVS_OPERATE_MEMORY_ERROR;

}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnFileBroadcastReportCallBack(WPARAM wParam, LPARAM lParam)
{
    CXml xml;
    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    unsigned int uiLent = 0;
    const char* pszResult = xml.GetXMLStream(uiLent);
    if (NULL != pszResult)
    {
        IVS_OCX_Event((long)wParam, (LPCTSTR)pszResult);
        BP_RUN_LOG_INF("FileBroadCast Report CallBack", "report string:%s", pszResult);
    }
    
    // add by wanglei 00165153:2013.6.29 函数被调用后，需要手动是否lParam所对应的空间，避免内存泄露
    STREAM_EXCEPTION_INFO* pExceptionInfo = (STREAM_EXCEPTION_INFO*)(lParam);
    IVS_DELETE(pExceptionInfo);
    
    return IVS_SUCCEED;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnAlarmReportCallBack(WPARAM wParam, LPARAM lParam)
{
    //告警回调上报XML
	IVS_INT32 iRet = IVS_FAIL;
	//IVS_ALARM_NOTIFY *pALarm = reinterpret_cast<IVS_ALARM_NOTIFY*>(lParam);
	IVS_ALARM_NOTIFY *pALarm = (IVS_ALARM_NOTIFY*)(lParam);
	if (NULL != pALarm)
	{
		CXml xml;
		if (IVS_SUCCEED == COCXAlarmMgrXMLProcess::AlarmReportGetXML(pALarm,xml))
		{
			unsigned int uiLent = 0;
			const char* pszResult = xml.GetXMLStream(uiLent);
			if (NULL == pszResult)
			{
				IVS_DELETE(pALarm);
				return IVS_OPERATE_MEMORY_ERROR;
			}
			// 打印二进制xml
			IVS_OCX_Event((long)wParam, (LPCTSTR)pszResult);
			IVS_DELETE(pALarm);
			iRet = IVS_SUCCEED;
			return iRet;
		}

		IVS_DELETE(pALarm);
		iRet = IVS_OPERATE_MEMORY_ERROR;
		BP_RUN_LOG_ERR(iRet,"IVS OCX Process Alarm Report Error.","Reason:Get Alarm Report XML Failed");
		return iRet;
	}

	iRet = IVS_OPERATE_MEMORY_ERROR;
	BP_RUN_LOG_ERR(iRet,"IVS OCX Process Alarm Report Error.","Reason:pALarm is NULL");
	return iRet;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnUserOfflineCallBack(WPARAM wParam, LPARAM lParam)
{
    //用户下线回调上报XML
    IVS_INT32 iRet = IVS_FAIL;
    IVS_USER_OFFLINE_INFO *pALarm = (IVS_USER_OFFLINE_INFO*)(lParam);
    if (NULL != pALarm)
    {
        CXml xml;
        iRet = COCXLoginXMLProcess::UserOfflineNotifyGetXML(pALarm, xml);
        if (IVS_SUCCEED == iRet)
        {
            unsigned int uiLent = 0;
            const char* pszResult = xml.GetXMLStream(uiLent);
            IVS_OCX_Event((long)wParam, (LPCTSTR)pszResult);//Fire Event
            IVS_DELETE(pALarm);
            return iRet;
        }

        IVS_DELETE(pALarm);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Process User Offline Error.","Reason:Get UserOfflineNotify XML Failed");
        return iRet;
    }

    iRet = IVS_OPERATE_MEMORY_ERROR;
    BP_RUN_LOG_ERR(iRet,"IVS OCX Process User Offline Error.","Reason:pALarm is NULL");
    return iRet;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnAlarmStatusReportCallBack(WPARAM wParam, LPARAM lParam)
{
	// 告警状态回调上报XML
	IVS_INT32 iRet = IVS_FAIL;
	IVS_ALARM_STATUS_NOTIFY *pALarm = reinterpret_cast<IVS_ALARM_STATUS_NOTIFY*>(lParam);
	if (NULL != pALarm)
	{
		CXml xml;
		if (IVS_SUCCEED == COCXAlarmMgrXMLProcess::GetAlarmStatusGetXML(pALarm, xml))
		{
			unsigned int uiLent = 0;
			const char* pszResult = xml.GetXMLStream(uiLent);
			IVS_OCX_Event((long)wParam, (LPCTSTR)pszResult);
			IVS_DELETE(pALarm);
			iRet = IVS_SUCCEED;
			return iRet;
		}

		IVS_DELETE(pALarm);
		iRet = IVS_OPERATE_MEMORY_ERROR;
		BP_RUN_LOG_ERR(iRet,"IVS OCX Process Alarm Status Report Error.","Reason:Get Alarm Status XML Failed");		
		return iRet;
	}

	iRet = IVS_OPERATE_MEMORY_ERROR;
	BP_RUN_LOG_ERR(iRet,"IVS OCX Process Alarm Status Report Error.","Reason:pALarm is NULL");
	return iRet;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnAlarmLinkageCallBack(WPARAM wParam, LPARAM lParam)
{
	// 联动动作执行通知
	IVS_INT32 iRet = IVS_FAIL;

	IVS_CHAR* pAlarmLikageAction = (IVS_CHAR*)lParam;
	if (NULL != pAlarmLikageAction)
	{
		IVS_OCX_Event((long)wParam, (LPCTSTR)pAlarmLikageAction);
		IVS_DELETE(pAlarmLikageAction, MUILI);
		iRet = IVS_SUCCEED;
		return iRet;
	}

	iRet = IVS_OPERATE_MEMORY_ERROR;
	BP_RUN_LOG_ERR(iRet,"IVS OCX Process Alarm Linkage Error.","Reason:pAlarmLikageAction is NULL");
	return iRet;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnMAUEventCallBack(WPARAM wParam, LPARAM lParam)
{
	// 智能分析
	IVS_INT32 iRet = IVS_FAIL;

	IVS_CHAR* pBALearningSchedule = (IVS_CHAR*)lParam;
	if (NULL != pBALearningSchedule)
	{
		IVS_OCX_Event((long)wParam, (LPCTSTR)pBALearningSchedule);
		IVS_DELETE(pBALearningSchedule, MUILI);
		iRet = IVS_SUCCEED;
		return iRet;
	}

	iRet = IVS_OPERATE_MEMORY_ERROR;
	BP_RUN_LOG_ERR(iRet,"IVS OCX Process BALearningSchedule Error.","Reason:pBALearningSchedule is NULL");
	return iRet;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnManualRecordStateCallBack(WPARAM wParam, LPARAM lParam)
{
	//手动录像启动/停止通知回调上报XML
	IVS_INT32 iRet = IVS_FAIL;
	//IVS_ALARM_NOTIFY *pALarm = reinterpret_cast<IVS_ALARM_NOTIFY*>(lParam);
	IVS_MANUAL_RECORD_STATUS_NOTIFY *pRecord = (IVS_MANUAL_RECORD_STATUS_NOTIFY*)(lParam);
	if (NULL != pRecord)
	{
        CXml xml;
		if (IVS_SUCCEED == COCXRecordXMLProcess::ManualRecordStateGetXML(pRecord,xml))
		{
			unsigned int uiLent = 0;
			const char* pszResult = xml.GetXMLStream(uiLent);
			if (NULL == pszResult)
			{
				IVS_DELETE(pRecord);
				return IVS_OPERATE_MEMORY_ERROR;
			}

			IVS_OCX_Event((long)wParam, (LPCTSTR)pszResult);
			IVS_DELETE(pRecord);
			iRet = IVS_SUCCEED;
			return iRet;
		}

		IVS_DELETE(pRecord);
		iRet = IVS_OPERATE_MEMORY_ERROR;
		BP_RUN_LOG_ERR(iRet,"IVS OCX Process Manual Record State Error.","Reason:Get Manual Record State XML Failed");
		return iRet;
	}

	iRet = IVS_OPERATE_MEMORY_ERROR;
	BP_RUN_LOG_ERR(iRet,"IVS OCX Process Manual Record State Error.","Reason:pRecord is NULL");
	return iRet;
}


afx_msg LRESULT CIVS_OCXPlayerCtrl::OnKeepAliveFailedCallBack(WPARAM wParam,LPARAM lParam)
{
    // 保活失败事件回调上报
    CXml xmlReq;
    (void)COCXAlarmMgrXMLProcess::ReLoginGetXML(xmlReq);
    unsigned int uiLent = 0;
    const char* pszResult = xmlReq.GetXMLStream(uiLent);
    IVS_OCX_Event((long)wParam, pszResult);
    return IVS_SUCCEED;
}//lint !e715

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnReLoginFailedCallBack(WPARAM wParam,LPARAM lParam)
{
    // 重登录失败事件回调上报
    CXml xmlReq;
    (void)COCXAlarmMgrXMLProcess::ReLoginGetXML(xmlReq);
    unsigned int uiLent = 0;
    const char* pszResult = xmlReq.GetXMLStream(uiLent);
    IVS_OCX_Event((long)wParam, pszResult);
    return IVS_SUCCEED;
}//lint !e715


afx_msg LRESULT CIVS_OCXPlayerCtrl::OnReLoginSuccessCallBack(WPARAM wParam,LPARAM lParam)
{
    // 重登录成功事件回调上报
    CXml xmlReq;
    (void)COCXAlarmMgrXMLProcess::ReLoginGetXML(xmlReq);
    unsigned int uiLent = 0;
    const char* pszResult = xmlReq.GetXMLStream(uiLent);
    IVS_OCX_Event((long)wParam, pszResult);
    return IVS_SUCCEED;
}//lint !e715

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnFindDevBackEvent(WPARAM wParam,LPARAM lParam)
{
	// 发现前端设备事件回调上报
	IVS_CHAR *pData = (IVS_CHAR*)lParam;
	if (NULL != pData)
	{
		IVS_OCX_Event((long)wParam, (LPCTSTR)pData);
		IVS_DELETE(pData);
		return IVS_SUCCEED;
	}

	BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Process Alarm Status Report Error.","Reason:pALarm is NULL");
	return IVS_OPERATE_MEMORY_ERROR;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnModeCruiseRecordOverCallBack(WPARAM wParam,LPARAM lParam)
{
	// 轨迹录制结束回调上报
	IVS_CHAR *pData = (IVS_CHAR*)lParam;
	if (NULL != pData)
	{
		IVS_OCX_Windows_Event((long)wParam, (LPCTSTR)pData);
		IVS_DELETE(pData);
		return IVS_SUCCEED;
	}

	BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Process ModeCruiseRecordOver Error.","Reason:pALarm is NULL");
	return IVS_OPERATE_MEMORY_ERROR;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnDevAlarmNotifyCallBack(WPARAM wParam,LPARAM lParam)
{
	// 设备告警回调上报XML
	IVS_INT32 iRet = IVS_FAIL;
	IVS_DEVICE_ALARM_NOTIFY *pDevALarm = (IVS_DEVICE_ALARM_NOTIFY*)(lParam);
	if (NULL != pDevALarm)
	{
		CXml xml;
		if (IVS_SUCCEED == COCXAlarmMgrXMLProcess::DevAlarmNotifyGetXML(xml, pDevALarm))
		{
			unsigned int uiLent = 0;
			const char* pszResult = xml.GetXMLStream(uiLent);
			if (NULL == pszResult)
			{
				IVS_DELETE(pDevALarm);
				return IVS_OPERATE_MEMORY_ERROR;
			}

			IVS_OCX_Event((long)wParam, (LPCTSTR)pszResult);			
			IVS_DELETE(pDevALarm);
			iRet = IVS_SUCCEED;
			return iRet;
		}

		IVS_DELETE(pDevALarm);
		iRet = IVS_OPERATE_MEMORY_ERROR;
		BP_RUN_LOG_ERR(iRet,"IVS OCX Process Device Alarm Notify Error.","Reason:Get Device Alarm Notify XML Failed");
		return iRet;
	}

	iRet = IVS_OPERATE_MEMORY_ERROR;
	BP_RUN_LOG_ERR(iRet,"IVS OCX Process Device Alarm Notify Error.","Reason: pDevALarm is NULL");
	return iRet;
}


afx_msg LRESULT CIVS_OCXPlayerCtrl::OnCommEventCallback(WPARAM wParam, LPARAM lParam)
{
	// 设置线程语言，避免VS2008上抛事件后产生乱码问题
	SetThreadLocale(LOCALE_SYSTEM_DEFAULT);

	//IVS_INFO_TRACE("OCX Address: %x", this);
	if (NULL == CIVS_OCXPlayerCtrl::m_pOCX)
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Process CommEvent Failed", "Reason:m_pOCX is NULL");
		return IVS_FAIL;
	}

	int iType = (int)wParam;
	
	// 保活失败
	switch (iType)
	{
	case IVS_EVENT_BROADCAST_FAILED:    		//语音广播失败
		BP_RUN_LOG_INF("Broadcast failed, report event to cu.", " EventType[%d].", iType);
		OnBroadcastFailedCallBack(wParam, lParam);
		break;

    case IVS_EVENT_TALKBACK_FAILED:        // 语音对讲失败
        BP_RUN_LOG_INF("TalkBack failed, report event to cu", "EventType[%d]", iType);
       OnTalkbackFailedCallBack(wParam, lParam);
       break;

    case IVS_EVENT_FILE_BROADCAST_END:
        //文件广播结束事件上报，不需要xml
        BP_RUN_LOG_INF("File Broadcast is end, report event to cu.", " EventType[%d].", iType);
        //IVS_OCX_Event((long)wParam, NULL);
        OnFileBroadcastReportCallBack(wParam, lParam);
        break;
	case IVS_EVENT_REPORT_ALARM:
		OnAlarmReportCallBack(wParam, lParam);
		break;
    case IVS_EVENT_REPORT_ALARM_STATUS:
        OnAlarmStatusReportCallBack(wParam,lParam);
        break;
    case IVS_EVENT_USER_OFFLINE:
        OnUserOfflineCallBack(wParam, lParam);
        break;
	case IVS_EVENT_DO_LINKAGE_ACTION:
    case IVS_EVENT_STOP_LINKAGE_ACTION:
		OnAlarmLinkageCallBack(wParam, lParam);
		break;
	case IVS_EVENT_IA_BA_LEARNING_SCHEDULE_REPORT:
	case IVS_EVENT_IA_FR_SCHEDULE_REPORT:
	case IVS_EVENT_IA_PUSH_ALARM_LOCUS:
    case IVS_EVENT_IA_ADDPLAN_SCHEDULE_REPORT:
		OnMAUEventCallBack(wParam, lParam);
		break;
	case IVS_EVENT_FIND_DEVICE:
		OnFindDevBackEvent(wParam, lParam);
		break;
	case IVS_EVENT_OMU_REPORT_ALARM:
		OnDevAlarmNotifyCallBack(wParam, lParam);
		break;
	case IVS_EVENT_MANUAL_RECORD_STATE:
		OnManualRecordStateCallBack(wParam, lParam);
		break;
    case IVS_EVENT_KEEP_ALIVE_FAILED:
        OnKeepAliveFailedCallBack(wParam, lParam);
        break;
    case IVS_EVENT_AUTO_CONNECT_SUCCEED:
        OnReLoginSuccessCallBack(wParam, lParam);
        break;
    case IVS_EVENT_LOGIN_FAILED:
        OnReLoginFailedCallBack(wParam, lParam);
        break;
	case IVS_EVENT_MODE_CRUISE_RECORD_OVER:
		OnModeCruiseRecordOverCallBack(wParam, lParam);
		break;
	//case IVS_EVENT_REALPLAY_FAILED:
// 	case IVS_PLAYER_RET_RECV_DATA_ERROR:
// 	case IVS_PLAYER_RET_RECV_DATA_TIMEOUT:
		//{
		//	//实况异常停止
		//	(void)OnExceptionRealPlay(wParam,lParam);
		//}
		//break;
	default:
		{
			try
			{
				char* pszEvent = (char*)lParam;
				if (NULL != pszEvent)
				{
					delete[] pszEvent;
					pszEvent = NULL;
				}
				
			}
			catch(...)
			{//lint !e1775
				BP_RUN_LOG_ERR(IVS_FAIL, "OnCommEventCallback", "in defalut switch eventtype[%d] delete event throw exception.", iType);
			}
		}

		break;
	}

	return IVS_SUCCEED;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnResetFullScreenMsg(WPARAM wParam, LPARAM lParam)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);
	m_pVideoPaneMgr->ResetFullScreen();
	return IVS_SUCCEED;
}//lint !e715

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnResetSetWndLayoutMsg(WPARAM wParam, LPARAM lParam)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);

	CRect rect;
	GetClientRect(rect);

	if (MAGNIFY == m_pVideoPaneMgr->GetMagnifyState())
	{
		m_pVideoPaneMgr->Magnify(rect);
		return IVS_SUCCEED;
	}

	// 先获取窗格布局类型
	IVS_WIND_LAYOUT winLayoutType = m_pVideoPaneMgr->GetWindLayOut();

	// 调用设置窗格布局，重绘所有窗格
	if (IVS_SUCCEED != m_pVideoPaneMgr->SetWndLayout(rect, winLayoutType))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "", "OCX change size error,set layout type error.");
		return IVS_FAIL;
	}
	return IVS_SUCCEED;
}//lint !e715

// OCX_EventHandler
// 处理SDK上报事件
inline void HandlePlayException(int iEventType, void *pEventBuf, size_t iBufSize, void *pUserData)
{
	CGlobalPlayMgr &playMgr = CGlobalPlayMgr::Instance();
	playMgr.HandleRealPlayException(0, iEventType, pEventBuf, iBufSize, pUserData);
}

inline void HandleLocalRecordEvent(int iEventType, void *pEventBuf, size_t iBufSize, void *pUserData)
{
    CGlobalPlayMgr &playMgr = CGlobalPlayMgr::Instance();
    playMgr.HandleLocalRecordException(0, iEventType, pEventBuf, iBufSize, pUserData);
}

inline void HandlePlayBackEvent(int iEventType, void *pEventBuf, size_t iBufSize, void *pUserData)
{
	CGlobalPlayMgr &playMgr = CGlobalPlayMgr::Instance();
	playMgr.HandlePlayBackException(0, iEventType, pEventBuf, iBufSize, pUserData);
}
//定义水印告警接受处理函数，通过全局的播放Mgr寻找播放的对象
inline void HandleWaterAlarmEvent(int iEventType, void *pEventBuf, size_t iBufSize, void *pUserData)
{
	CGlobalPlayMgr &playMgr = CGlobalPlayMgr::Instance();
	playMgr.HandleWaterAlarmEvent(0, iEventType, pEventBuf, iBufSize, pUserData);
}

inline void HandleDownLoadEvent(int iEventType, void *pEventBuf, size_t iBufSize, void *pUserData)
{
	CGlobalPlayMgr &playMgr = CGlobalPlayMgr::Instance();
	playMgr.HandleDownLoadEvent(0, iEventType, pEventBuf, iBufSize, pUserData);
}

inline void HandleRemotePlaybackEvent(void *pEventBuf, size_t iBufSize, void *pUserData)//lint !e830
{
    CIVS_OCXPlayerCtrl * pOcxCtrl = nullptr;
    STREAM_EXCEPTION_INFO* pExceptionInfo =(STREAM_EXCEPTION_INFO*)(pEventBuf);
    if (NULL == pExceptionInfo)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "FunEvent CallBack", "PlayBack pEventBuf cannot transform to STREAM_EXCEPTION_INFO.");
        return;
    }
    HWND hWnd = NULL;
    int iRet = GetHwndbyPlayHandle(pExceptionInfo->ulHandle, hWnd);
    if (IVS_SUCCEED != iRet || NULL == hWnd)
    {
        BP_RUN_LOG_ERR(IVS_OCX_RET_FIND_PLAY_HANDLE_ERROR, "FunEvent CallBack", "GetHwndbyPlayHandle failed. can not find Hwnd by playhandle(%lu)",pExceptionInfo->ulHandle);
        return;
    }

    std::map <HWND, CIVS_OCXPlayerCtrl *>::iterator iter = CIVS_OCXPlayerCtrl::m_hWnd2OcxMap.find(hWnd);
    if (iter == CIVS_OCXPlayerCtrl::m_hWnd2OcxMap.end())
    {
        BP_RUN_LOG_ERR(IVS_OCX_RET_FIND_PLAY_HANDLE_ERROR,"FunEvent CallBack","Function exception callback error, find ocx pointer by port fail,hWnd(%lu)",(unsigned long)hWnd);
        return;
    }

    pOcxCtrl = iter->second;
    CHECK_POINTER_VOID(pOcxCtrl);

    // 注意POST之后释放pEvent的内存空间
    PostMessage(pOcxCtrl->GetSafeHwnd(), WM_EVENT_REMOTE_PLAYBACK_CALLBACK, (WPARAM)hWnd, (LPARAM)pExceptionInfo->iExceptionCode);
    return;
}//lint !e818 !e715

inline void HandleIntelligenceAnalysisEvent(int iEventType, void *pEventBuf, size_t iBufSize, void *pUserData)//lint !e830
{
	IVS_DEBUG_TRACE("EventType: %d", iEventType);

	CGlobalIAMessageMgr &Mgr = CGlobalIAMessageMgr::Instance();
	switch (iEventType)
	{
	case IVS_EVENT_IA_PUSH_ALARM_LOCUS:
		Mgr.IAPushAlarmLocus((IVS_CHAR *)pEventBuf);
		break;
	case IVS_EVENT_IA_BA_LEARNING_SCHEDULE_REPORT:
    case IVS_EVENT_IA_ADDPLAN_SCHEDULE_REPORT:
        break;
	default:
		break;
	}
	return;
}//lint !e818 !e715


inline void HandleCommonEvent(int iEventType, void *pEventBuf, size_t iBufSize, void *pUserData)//lint !e830
{
    char *pEvent = NULL;
    try
    {
        if ((NULL != pEventBuf) && (0 != iBufSize))
        {
            try
            {
                pEvent = new char[(unsigned int)iBufSize];
				memset(pEvent, 0, sizeof(iBufSize));
            }
            catch(...)
            {
                BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "FunEvent CallBack", "new memory failed");
                return;
            }

            if(!CToolsHelp::Memcpy(pEvent,iBufSize, pEventBuf, (unsigned int)iBufSize))
            {
                BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"FunEvent CallBack","Memcpy Failed.");
                delete[] pEvent;
                pEvent = NULL;
                return;
            }
        }
    }
    catch(...)
    {
        return;
    }
    PostMessage(CIVS_OCXPlayerCtrl::m_pOCX->GetSafeHwnd(), WM_COMM_EVENT_CALLBACK, (WPARAM)iEventType, (LPARAM)pEvent);
}//lint !e818 !e715




/*
 *	SDK事件上报的统一入口，分为两类：
 *  一类：需要通知到具体OCX对象的，比如媒体类的实况、回放、录像、下载、智能分析进度等由某OCX触发的，需要改变界面状态等；
 *  二类：SDK的通用事件，与OCX无关，比如告警、设备状态、用户上下线、重登录、保活失败等事件，统一通知到第一个初始化的OCX对象上（即登录时创建的OCX）
 */
void CALLBACK FunEventCallBack(IVS_INT32 iEventType, IVS_VOID *pEventBuf, IVS_UINT32 iBufSize, IVS_VOID *pUserData)
{
	switch(iEventType)
	{
    ////////////////////////////第一类//////////////////////////////////////////////
	case IVS_EVENT_REALPLAY_FAILED:
	case IVS_EVENT_RESUME_REALPLAY_OK:
		HandlePlayException(iEventType, pEventBuf, iBufSize, pUserData);
		break;
	case IVS_EVENT_LOCAL_RECORD_SUCCESS:
	case IVS_EVENT_LOCAL_RECORD_EGENERIC:
		HandleLocalRecordEvent(iEventType, pEventBuf, iBufSize, pUserData);
		break;
	case IVS_EVENT_REMOTE_PLAYBACK_FAILED:
	case IVS_EVENT_LOCAL_PLAYBACK_FAILED:
		//HandleRemotePlaybackEvent(pEventBuf, iBufSize, pUserData);
		HandlePlayBackEvent(iEventType, pEventBuf, iBufSize, pUserData);
		break;

	case IVS_EVENT_DOWNLOAD_FAILED:
	case IVS_EVENT_DOWNLOAD_SUCCESS:
		HandleDownLoadEvent(iEventType, pEventBuf, iBufSize, pUserData);
		break;
//#
	case IVS_EVENT_IA_PUSH_ALARM_LOCUS:
		HandleIntelligenceAnalysisEvent(iEventType, pEventBuf, iBufSize, pUserData);
		break;
		//水印告警类型
	case IVS_EVENT_REPORT_WATERMARK_TIP:
	case IVS_EVENT_REPORT_WATERMARK_ALARM:
		HandleWaterAlarmEvent(iEventType, pEventBuf, iBufSize, pUserData);
		break;
    ////////////////////////////第二类:此类事件通知到第一个注册IVS_OCX_Event事件的OCX对象;//////////////////////////
    case IVS_EVENT_AUTO_CONNECT_SUCCEED:
    case IVS_EVENT_KEEP_ALIVE_FAILED:
    case IVS_EVENT_LOGIN_FAILED:
    case IVS_EVENT_REPORT_ALARM:
    case IVS_EVENT_REPORT_ALARM_STATUS:
    case IVS_EVENT_USER_OFFLINE:
    case IVS_EVENT_DO_LINKAGE_ACTION:
	case IVS_EVENT_STOP_LINKAGE_ACTION:
    case IVS_EVENT_FIND_DEVICE:
    case IVS_EVENT_OMU_REPORT_ALARM:
	case IVS_EVENT_MANUAL_RECORD_STATE:
    //case IVS_EVENT_OTHER:
    case IVS_EVENT_IA_BA_LEARNING_SCHEDULE_REPORT://待张舒璠修改成走具体事件接口后，去除该行;
    case IVS_EVENT_IA_ADDPLAN_SCHEDULE_REPORT:
	case IVS_EVENT_IA_FR_SCHEDULE_REPORT:  
	case IVS_EVENT_MODE_CRUISE_RECORD_OVER:
	case IVS_EVENT_BROADCAST_FAILED:                            // 广播事件
    case IVS_EVENT_TALKBACK_FAILED:                                // 语音对讲事件
    case IVS_EVENT_FILE_BROADCAST_END:
        //如果是文件广播结束事件，则停止广播
        if (IVS_EVENT_FILE_BROADCAST_END == iEventType)
        {
            IVS_SDK_StopFileBroadcast(CIVS_OCXPlayerCtrl::m_iSDKSessionID);
        }
        HandleCommonEvent(iEventType, pEventBuf, iBufSize, pUserData);
        break;
	default:
        {
            //BP_RUN_LOG_ERR(IVS_OCX_EVENT_NO_CATCH, "FunEvent CallBack", "The event did not catched, event:%u.", iEventType);
        }
		break;
	}

	//delete []pEvent;
} //lint !e818 回调默认使用

//afx_msg LRESULT CIVS_OCXPlayerCtrl::OnRealPlayEvent(WPARAM wParam,LPARAM lParam)
//{
//    IVS_OCX_Event(IVS_EVENT_REALPLAY_FAILED, (LPCTSTR)wParam);
//	return S_OK;
//}

void CIVS_OCXPlayerCtrl::ChangeLayoutRealPlay( ULONG ulWndID )
{
   IVS_INFO_TRACE("OCX Address: %x,WndID: %lu", this,ulWndID);
    // 当前布局不够放窗格时，布局升级
    // 先获取窗格个数
    CHECK_POINTER_VOID(m_pVideoPaneMgr);

	// 先获取窗格布局类型
	IVS_WIND_LAYOUT winLayoutType = m_pVideoPaneMgr->GetWindLayOut();
	ULONG ulLayout = m_pVideoPaneMgr->GetNumOfDisplayPane();

    BP_RUN_LOG_INF("IVS OCX Change Layout RealPlay.", "Layout=%u, WinID=%u.", winLayoutType, ulWndID);

    if (ulWndID > ulLayout)
    {
        ulLayout = ulWndID;
        while(IVS_SUCCEED != LayoutMgr.CheckLayoutType(ulLayout))
        {
            ulLayout++;
        }
    } else if (MINIFY == ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetMagnifyState()) {
        return ;
    }

    CRect rect;
    if(NOT_FULL_SCREEN == ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetFullScreenState())
    {
        GetClientRect(rect);
    } else {
        HMONITOR hMonitor = MonitorFromWindow(GetSafeHwnd(),MONITOR_DEFAULTTONEAREST);
        MONITORINFO info;
        info.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(hMonitor,&info);

        rect.top = info.rcMonitor.top;
        rect.left = info.rcMonitor.left;
        rect.bottom = info.rcMonitor.bottom;
        rect.right = info.rcMonitor.right;
    }

    if (IVS_SUCCEED != ((CVideoPaneMgr*)m_pVideoPaneMgr)->SetWndLayout(rect,winLayoutType))
    {
        //此处只打印日志，不作处理
        BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Change Layout Real Play Failed", "Reason:Set window layout fail.");
    }
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_Login(LPCTSTR pUserName, LPCTSTR pPWD, LPCTSTR pServerIP,LPCTSTR pPort, ULONG ulClientType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    //用户名，ip，端口，机器名
    if ((NULL == pUserName) || (NULL == pPWD) || (NULL == pServerIP)  || (NULL == pPort))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Login failed", "Reason:UserName IP Port or MachineName is invalid");
        return IVS_PARA_INVALID;
    }
	IVS_INFO_TRACE("OCX Address: %x,UserName: %s,ServerIP: %s,Port: %s,ClientType: %lu", this,pUserName,pServerIP,pPort,ulClientType);

    //客户端类型校验
    if (CLIENT_TV_WALL < ulClientType)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Login failed", "Reason:ClientType is invalid");
        return IVS_PARA_INVALID;
    }
    //获取登陆消息结构体
    IVS_LOGIN_INFO sLoginInfo;
    memset(&sLoginInfo,0,sizeof(IVS_LOGIN_INFO));
    IVS_INT32 iGetReqRet = COCXLoginXMLProcess::GetLoginReqInfo(pUserName, pPWD, pServerIP, pPort, ulClientType, sLoginInfo);
    if (IVS_SUCCEED !=iGetReqRet)
    {
        return iGetReqRet;
    }

    IVS_INT32 iRet = IVS_SDK_Login(&sLoginInfo, &m_iSDKSessionID);

	//IVS_INT32 iRet = IVS_SUCCEED;
    // 设置事件回调函数
	if (IVS_SUCCEED == iRet||IVS_SMU_USER_FIRST_LOGIN == iRet || IVS_SMU_USER_PWD_NEED_MOD == iRet)
	{
		(void)IVS_SDK_SetEventCallBack(m_iSDKSessionID, FunEventCallBack);

		std::string strUserName = "IVS_";
		strUserName += sLoginInfo.cUserName;
		CLocalConfig::SetLoginName(strUserName);

		std::string strIP = sLoginInfo.stIP.cIP;
		CLocalConfig::SetIP(strIP);

        // 子进程不需要上传下载功能
        if (CLIENT_PC_CHILD != ulClientType && CLIENT_TV_WALL != ulClientType)
        {
            //启动上传、下载文件的功能
            (void)m_pLocalConfig.StartUserData(m_iSDKSessionID, strUserName, strIP);

            // 判断配置文件是否存在；若不存在，拷贝默认配置文件到用户配置文件目录
            std::string strCheckPath = m_pLocalConfig.CheckLocalConfigFileExist(LOCAL_CONFING_FILE_NAME);
            if (strCheckPath.empty())
            {
                std::string moveSrcFile; // 默认文件存放位置
                TCHAR Buffer[BUFSIZE] = {0};
                (void)GetModuleFileName(AfxGetInstanceHandle(),Buffer,BUFSIZE);
                (void)moveSrcFile.append(Buffer);
                moveSrcFile = moveSrcFile.substr(0,moveSrcFile.rfind(_T("\\")));
                moveSrcFile.append(DEFAULT_LOCALCONFIG_PATH);

                std::string moveDstFile = m_pLocalConfig.m_strFileNamePath + "\\" + LOCAL_CONFING_FILE_NAME;
                (void)CopyFile(moveSrcFile.c_str(),moveDstFile.c_str(), FALSE);
            }
        }
		//(void)CLocalConfig::instance().StartUserData(m_iSDKSessionID, strUserName, strIP);
	}
	// 加载默认国际化
	(void)IVS_OCX_SetLanguage(CSDKConfig::instance().GetDefaultLanguageValue().c_str());
	// 清理内存，保证内存中不会有密码的存在
	memset(&sLoginInfo,0,sizeof(IVS_LOGIN_INFO));
	memset((char*)pPWD,0,strlen(pPWD));
	
    return  iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_WinUserLogin(LPCTSTR pUserName, LPCTSTR pPWD, LPCTSTR pServerIP, LPCTSTR pServerPort, LPCTSTR pDomainName, ULONG ulLoginType, ULONG ulClientType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// 密码、服务器IP、端口、域名检验
	CHECK_POINTER(pPWD,IVS_PARA_INVALID);
	CHECK_POINTER(pServerIP,IVS_PARA_INVALID);
	CHECK_POINTER(pServerPort,IVS_PARA_INVALID);
	CHECK_POINTER(pDomainName,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,UserName: %s,ServerIP: %s,SeverPort: %s,DomainName: %s,LoginType: %lu,ClientType: %lu",
		           this,pUserName,pServerIP,pServerPort,pDomainName,ulLoginType,ulClientType);

	//客户端类型校验
	if (CLIENT_TV_WALL < ulClientType)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX WinUser Login Failed.", "Reason:ClientType is invalid");
		return IVS_PARA_INVALID;
	}
	// 登陆类型基本检验
	if (LOGIN_WINDOWS_CURRENT != ulLoginType && LOGIN_WINDOWS_OTHER != ulLoginType)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX WinUser Login Failed.", "Reason:LoginType is invalid");
		return IVS_PARA_INVALID;
	}
	std::string strUserNameTemp = pUserName;
	if (LOGIN_WINDOWS_CURRENT == ulLoginType && (strUserNameTemp.empty()))
	{
		// 获取登陆用户名
		CToolsHelp::GetCurrentUserName((char*)pUserName);
		// 校验用户名是否是域用户
	}

	//获取登陆消息结构体
	IVS_LOGIN_INFO sLoginInfo;
	memset(&sLoginInfo,0,sizeof(IVS_LOGIN_INFO));

	if(!CToolsHelp::Memcpy(sLoginInfo.cUserName,IVS_NAME_LEN, pUserName, strlen(pUserName)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS OCX WinUser Login Failed.","Reason:Memcpy UserName Failed.");
        return IVS_ALLOC_MEMORY_ERROR;
	}
	if(!CToolsHelp::Memcpy(sLoginInfo.pPWD,IVS_PWD_LEN, pPWD, strlen(pPWD)))
	{
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS OCX WinUser Login Failed.","Reason:Memcpy PWD Failed.");
        return IVS_ALLOC_MEMORY_ERROR;
	}
	if(!CToolsHelp::Memcpy(sLoginInfo.cDomainName,IVS_DOMAIN_LEN, pDomainName, strlen(pDomainName)))
	{
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS OCX WinUser Login Failed.","Reason:Memcpy DomainName Failed.");
        return IVS_ALLOC_MEMORY_ERROR;
	}
	sLoginInfo.stIP.uiIPType = static_cast<IVS_UINT32>(IP_V4);
	if(!CToolsHelp::Memcpy(sLoginInfo.stIP.cIP,IVS_IP_LEN, pServerIP, strlen(pServerIP)))
	{
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS OCX WinUser Login Failed.","Reason:Memcpy ServerIP Failed.");
        return IVS_ALLOC_MEMORY_ERROR;
	}
	sLoginInfo.uiPort = static_cast<IVS_UINT32>(atoi(pServerPort));
	sLoginInfo.uiClientType = static_cast<IVS_UINT32>(ulClientType);
	sLoginInfo.uiLoginType = static_cast<IVS_UINT32>(ulLoginType);

	IVS_INT32 iRet = IVS_SDK_WinUserLogin(&sLoginInfo, &m_iSDKSessionID);

	// 设置事件回调函数
	if (IVS_SUCCEED == iRet|| IVS_SMU_USER_FIRST_LOGIN == iRet || IVS_SMU_USER_PWD_NEED_MOD == iRet)
	{
		(void)IVS_SDK_SetEventCallBack(m_iSDKSessionID, FunEventCallBack);

        // 保存用户名和IP，用于本地存储,域用户使用IVS_域名_用户名
        std::string strUserName = "IVS_";
        strUserName += sLoginInfo.cDomainName;
        strUserName += "_";
        strUserName += sLoginInfo.cUserName;
        CLocalConfig::SetLoginName(strUserName);
        std::string strIP = sLoginInfo.stIP.cIP;
        CLocalConfig::SetIP(strIP);

        // 子进程不需要上传下载功能
        if (CLIENT_PC_CHILD != ulClientType && CLIENT_TV_WALL != ulClientType)
        {
            //启动上传、下载文件的功能
            (void)m_pLocalConfig.StartUserData(m_iSDKSessionID, strUserName, strIP);

            // 判断配置文件是否存在；若不存在，拷贝默认配置文件到用户配置文件目录
            std::string strCheckPath = m_pLocalConfig.CheckLocalConfigFileExist(LOCAL_CONFING_FILE_NAME);
            if (strCheckPath.empty())
            {
                std::string moveSrcFile; // 默认文件存放位置
                TCHAR Buffer[BUFSIZE] = {0};
                (void)GetModuleFileName(AfxGetInstanceHandle(),Buffer,BUFSIZE);
                (void)moveSrcFile.append(Buffer);
                moveSrcFile = moveSrcFile.substr(0,moveSrcFile.rfind(_T("\\")));
                moveSrcFile.append(DEFAULT_LOCALCONFIG_PATH);

                std::string moveDstFile = m_pLocalConfig.m_strFileNamePath + "\\" + LOCAL_CONFING_FILE_NAME;
                (void)CopyFile(moveSrcFile.c_str(),moveDstFile.c_str(), FALSE);
            }
        }
	}
    // 加载默认国际化
    (void)IVS_OCX_SetLanguage(CSDKConfig::instance().GetDefaultLanguageValue().c_str());
	// 清理内存，保证内存中不会有密码的存在
	memset(&sLoginInfo,0,sizeof(IVS_LOGIN_INFO));
	memset((char*)pPWD,0,strlen(pPWD));
	return  iRet;
}

// StartRealSteam
// 启动实时流 OCX内部调用接口
LONG CIVS_OCXPlayerCtrl::StartRealSteam(IVS_REALPLAY_PARAM &realplayParam, LPCTSTR pCameraID,
	          CVideoPane *pVideoPane, BOOL bLayoutChange, ULONG ulStreamOpenMode)
{
	ASSERT(NULL != pVideoPane);
	CHECK_POINTER(pVideoPane, IVS_PARA_INVALID); //lint !e774
    CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);

	if (VIDEO_PLAY_STATE_REALTIME == pVideoPane->GetPlayState() ||
		VIDEO_PLAY_STATE_RESTARTING == pVideoPane->GetPlayState())
    {
		// 本窗口已经打开相同视频，直接返回成功
		if (pVideoPane->VerifySameStream(pCameraID, realplayParam.uiStreamType))
		{
			BP_RUN_LOG_INF("Start Real Steam", "Panel Play Same Stream"); 
			return IVS_SUCCEED;
		}
		(void)StopRealPlayByPane(pVideoPane);
    }

	CGlobalPlayMgr &PlayMgr = CGlobalPlayMgr::Instance();
	ULONG ulHandle = PlayMgr.TryRealStreamShare(pCameraID, realplayParam.uiStreamType, pVideoPane);

	if (0 != ulHandle)
	{
		// 判断一下是否在重连
		unsigned long PlayState = PlayMgr.GetPlayState(ulHandle);
		if (VIDEO_PLAY_STATE_RESTARTING == PlayState)
		{   
			(void)PlayMgr.UnRegPlayHandle(ulHandle, pVideoPane);
			// 设置播放句柄0，重新请求一路
			ulHandle = 0;
		}
		else
		{
			// 没有重连就流复用
			BP_RUN_LOG_INF("IVS OCX Start Real Steam", "Share RealStream Handle with %08x", ulHandle);
			IVS_SDK_AddPlayWnd(m_iSDKSessionID, ulHandle, pVideoPane->GetVideoWnd());
			pVideoPane->SetPlayState(PlayState);
		}
	}

	if (0 == ulHandle)
	{
		IVS_INT32 iRet = IVS_SDK_StartRealPlay(m_iSDKSessionID, &realplayParam,
			    pCameraID, pVideoPane->GetVideoWnd(), &ulHandle);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_INF("Start Real Steam", "Start Real Play Fail error = [%d]", iRet);
			pVideoPane->Invalidate(TRUE);
    		return iRet;
    	}

    	(void)PlayMgr.RegRealPlayHandle(ulHandle, pVideoPane, pCameraID, realplayParam.uiStreamType);
		(void)IVS_SDK_SetDisplayScale(m_iSDKSessionID, ulHandle, (int)(m_pVideoPaneMgr->GetDispalyScale()));
		//(void)IVS_SDK_SetDisplayScale(m_iSDKSessionID, ulHandle, PlayMgr.GetDisplayScale());
		pVideoPane->SetPlayState(VIDEO_PLAY_STATE_REALTIME);
    }
// 	else
// 	{
// 
// 	}

	//保存流复用信息
	pVideoPane->SetRealStreamInfo(pCameraID, realplayParam.uiStreamType);
    if (m_pVideoPaneMgr->isAnalysisWndType())
    {
        pVideoPane->SetAnalysisBarIsStartPlay(TRUE);
    }


    pVideoPane->Set3DPTZState(PTZ_3D_CLOSE);
	// 设置是否满窗口显示
    pVideoPane->SetPlayHandle(ulHandle);
    pVideoPane->SetServiceType(SERVICE_TYPE_REALVIDEO);
    pVideoPane->SetTitleButtonPic(true);
    pVideoPane->SetRealtimePlayPic(BUTTON_CLICKED);
	pVideoPane->SetCameraID((char *)pCameraID);
    pVideoPane->EnableALLButton(TRUE);
	pVideoPane->Invalidate(TRUE);

	return IVS_SUCCEED;
}//lint !e715

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartRealPlay(LPCTSTR pMediaPara, LPCTSTR pCameraCode, LONG lWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT(NULL != pMediaPara && NULL != pCameraCode);
	CHECK_POINTER(pMediaPara, IVS_PARA_INVALID);   //lint !e774
    CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);   //lint !e774

	IVS_INFO_TRACE("OCX Address: %x,XML: %s,CAMERA: %s,WndID: %ld", this,pMediaPara,pCameraCode,lWndID);
    if ('\0' == pCameraCode[0])
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start Real Play Failed.", "Reason:CameraCode is Empty");
        return IVS_PARA_INVALID;
    }

    CVideoPane *pVideoPane = GetVideoPanel((ULONG)lWndID);
    ASSERT(NULL != pVideoPane);
	if (NULL == pVideoPane)	  //lint !e774
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start Real Play Failed.", "Reason:Invalid WinID:%ld.", lWndID);
        return IVS_PARA_INVALID;
    }
    

	// 判断窗口是不是被局部放大占用，如果被占用，就关闭局部放大
	if ( BOTTOMBAR_MAGNIFY_OPEN == pVideoPane->GetDstMagnifyState())
	{
		CVideoPane* pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
		if (NULL != pSrcVideoPane)
		{
			::SendMessage(pSrcVideoPane->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (LONG)pVideoPane);
		}
	}

	pVideoPane->CreateBar();

	//如果已经在快速录像回放了
	if (pVideoPane->GetShowInstantStatus())
	{
		//pVideoPane->ResumeRealPlay();
		(void)IVS_OCX_StopRealPlay(lWndID);
		//(void)StopRealPlayByPane(pVideoPane);
	}

    IVS_REALPLAY_PARAM stRealplayParam;
    memset(&stRealplayParam, 0, sizeof(stRealplayParam));

	if (IVS_SUCCEED != COCXRealPlayXMLProcess::GetMediaParamXML(pMediaPara, stRealplayParam))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start Real Play Failed.", "Reason:MediaPara Xml Invalid");
		return IVS_PARA_INVALID;
	}

    BOOL bEnableGPU = FALSE;

    std::string ParamString = m_pLocalConfig.GetCDATA2XML(LOCAL_CONFING_FILE_NAME,NETWORK_CONFIG_ELEMENT);
    BP_RUN_LOG_INF("IVS OCX Get Local NetWork Config.","Value:%s", ParamString.c_str());
    if (IVS_SUCCEED == COCXRealPlayXMLProcess::EnableGPUParseClientConfig(ParamString.c_str(), bEnableGPU))
    {
        (void)IVS_OCX_EnableGPU(bEnableGPU);
    }//lint !e613
	else
	{
		BP_RUN_LOG_INF("IVS OCX Enable GPU Parse Client Config failed.","bEnableGPU:%d", bEnableGPU);
	}

	IVS_INT32 iRet = StartRealSteam(stRealplayParam, pCameraCode, pVideoPane, TRUE, VIDEO_PLAY_STATE_REALTIME);

    CVideoPane* pVideoPane1 = GetVideoPaneMgr()->GetWndPointerByID((ULONG)lWndID);//lint !e613
	CHECK_POINTER(pVideoPane1, IVS_OPERATE_MEMORY_ERROR);
    pVideoPane1->SetRealtimeTalkbackState(BOTTOMBAR_TALKBACK_CLOSE);
    CTranVideoRealtimeBar * pRealTimeToolBar = pVideoPane1->GetVideoRealtimeBar();
    if (NULL != pRealTimeToolBar)
    {
         pRealTimeToolBar->SetTalkbackCtrlState(false);
    }
       

	// 判断这个摄像机是不是被开启了语音，如果开启了 需要设置一个按钮按下的状态
	
    if (0 == strcmp(pCameraCode, m_strVoiceTalkbackCameraCode.c_str()))
	{
		// 查找是否有开启实况的窗口，同步状态

        if (FALSE == pVideoPane1->GetVideoPaneMgr()->IsTalkback())
        {
            // 设置语音开启的状态
            // modi by wKF71044 at [2013-7-5 10:15:45]取消同步
			m_pVoiceTalkbackVideoPane = pVideoPane1;

			m_pVoiceTalkbackVideoPane->SetRealtimeTalkbackState(BOTTOMBAR_TALKBACK_OPEN);
             
			CTranVideoRealtimeBar* pRealtimeBar = m_pVoiceTalkbackVideoPane->GetVideoRealtimeBar();
			if (NULL != pRealtimeBar)
			{
				pRealtimeBar->SetTalkbackCtrlState(true);
			}
        }
	}
	else
	{
		if (NULL != m_pVoiceTalkbackVideoPane && pVideoPane1->GetPaneID() == m_pVoiceTalkbackVideoPane->GetPaneID())
		{
			m_pVoiceTalkbackVideoPane = NULL;
		}
	}

    // 判断这个摄像机是否开启随路语音,开启的话，需要设置按钮为开启
//     if ( GetVideoPaneMgr()->IsSoundStart( pCameraCode ) )
//     {
//         CTranVideoRealtimeBar* pBar = pVideoPane->GetVideoRealtimeBar();
//         if (nullptr != pBar)
//         {
//             pBar->PostMessage(WM_SETSOUND_STATE, 0, 0);
//         }
//     }
	// 按钮过滤，一些按钮需要被过滤，例如局部放大需要只有一个才能用
	DoButtonFilter(lWndID);
	SetFocus();
	return iRet;
}

// IVS_OCX_StopRealPlay
// 停止实时视频流播放
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopRealPlay(LONG lWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);

	CVideoPane *pVideoPane = GetVideoPanel((ULONG)lWndID);
	ASSERT(NULL != pVideoPane);
	if (NULL == pVideoPane)  //lint !e774
	{
	    BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Stop Real Play Failed.", "Reason:Invalid WndID:[%ld]", lWndID);
		return IVS_FAIL;
	}

	//如果已经在快速录像回放了
	if (pVideoPane->GetShowInstantStatus())
	{
		pVideoPane->ResumeRealPlay();
	}

#if 0
	/*****************************************************
	当关闭实况时,关闭随路语音，但是如果有复用，则，当该
	窗格的随路语音打开，则要关闭当前随路语音并打开其他随路语音
	******************************************************/
	//if(NULL != pVideoPane->GetVideoRealtimeBar())
	//{
	//	// 记录下该窗格是否打开随路语音
	//	if(BOTTOMBAR_SOUND_OPEN == pVideoPane->GetRealtimeSoundState())
 //       {
 //           pVideoPane->GetVideoRealtimeBar()->StopSound();
 //           pVideoPane->GetVideoRealtimeBar()->DeleteSoundCtrl();
	//	}
	//}

	//IVS_SDK_StopWndBusinessRestore( pVideoPane->GetVideoWnd() );
#endif

	//pVideoPane->SetResumeState(FALSE);

	return (LONG)StopRealPlayByPane(pVideoPane);
}

ULONG CIVS_OCXPlayerCtrl::SetResumeState(HWND hWnd, BOOL bState)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,State: %d", this,(int)bState);

    if (0 == hWnd)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Resume State Failed.", "Reason:parameter is invalid. hWnd=%lu.", (ULONG)hWnd);
        return IVS_PARA_INVALID;
    }

	CHECK_POINTER(m_pVideoPaneMgr, IVS_OPERATE_MEMORY_ERROR);

	ULONG ulWinID = m_pVideoPaneMgr->GetWindIDByHWND(hWnd);
	CVideoPane *pVideoPane = m_pVideoPaneMgr->GetWndPointerByID(ulWinID);

	CHECK_POINTER(pVideoPane,IVS_OPERATE_MEMORY_ERROR);

	if (bState)
	{
		pVideoPane->SetResumeState(true);
	} else {
		pVideoPane->SetResumeState(false);
	}
	return IVS_SUCCEED;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_GetFreeWnd(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CHECK_POINTER(m_pVideoPaneMgr,IVS_OCX_RET_GET_INFO_ERROR);
	IVS_INFO_TRACE("OCX Address: %x", this);
    ULONG ulWndID = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndIDFree();

    // 窗格ID范围是1到64
    if (!m_pVideoPaneMgr->IsLegalityWndID(ulWndID))
    {
        BP_RUN_LOG_ERR(IVS_OCX_RET_GET_INFO_ERROR, "IVS OCX Get Free Wnd Failed", "Reason:Window ID is invalid.");
        return IVS_OCX_RET_GET_INFO_ERROR;
    }

    BP_RUN_LOG_INF("IVS OCX Get FreeWnd End.","FreeWndID=%u.",ulWndID);

    return static_cast<LONG>(ulWndID);
}

void CIVS_OCXPlayerCtrl::OnMove(int x, int y)
{
    COleControl::OnMove(x, y);
    //CHECK_POINTER_VOID (m_pVideoPaneMgr); //del by w00210470
    if (NULL != m_pVideoPaneMgr)
    {
        //m_pVideoPaneMgr->ResetBarPosition();
    }
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetToolbar(ULONG ulToolbarFlag)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,ToolBarFlag: %lu", this,ulToolbarFlag);
    ULONG _ulToolbarFlag  = IVS_TOOLBAR_SNAPSHOT | IVS_TOOLBAR_LOCAL_RECORD | IVS_TOOLBAR_BOOKMARK | 
        IVS_TOOLBAR_ZOOM | IVS_TOOLBAR_PLAY_RECORD | IVS_TOOLBAR_PLAY_SOUND |
        IVS_TOOLBAR_TALKBACK | IVS_TOOLBAR_VIDEO_TVW | IVS_TOOLBAR_ALARM_WIN | 
        IVS_TOOLBAR_PTZ | IVS_TOOLBAR_IA | IVS_TOOLBAR_OPEN_MAP | IVS_TOOLBAR_WINDOW_MAIN |
        IVS_TOOLBAR_PLAY_QUALITY | IVS_TOOLBAR_PTZ3D;
	if (ulToolbarFlag > _ulToolbarFlag)
	{
		return IVS_PARA_INVALID;
	}

    CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);

	if (IVS_SUCCEED !=  m_pVideoPaneMgr->SetToolbar(ulToolbarFlag))
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Set Tool Bar Failed.","Reason:m_pVideoPaneMgr Set Tool bar Failed.");
		return IVS_FAIL;
	}
    return IVS_SUCCEED;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_Logout(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	// 备份本地文件到服务器,备份失败不影响推出
	IVS_INT32 iBackData = m_pLocalConfig.BackUserData();
	if (IVS_SUCCEED != iBackData)
	{
		BP_RUN_LOG_ERR(iBackData, "IVS OCX Logout Error Occured." , "Reason:Back UserData Failed.");
	}
	// 停止定时器
	m_pLocalConfig.StopLocalConfigTimer();

	// TODO: Add your dispatch handler code here
	IVS_INT32 iRet = IVS_SDK_Logout(m_iSDKSessionID);
    m_iSDKSessionID = SDK_SESSIONID_INIT;
	return iRet;
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_SetLocalAlarmConfig(LPCTSTR pAlarmParm)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pAlarmParm)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Local Alarm Config Failed.", "Reason:pAlarmParm is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,AlarmParm: %s", this,pAlarmParm);
	//CHECK_POINTER(m_pLocalConfig);
	if (!m_pLocalConfig.SetCDATA2XML(LOCAL_CONFING_FILE_NAME, ALARM_CONFIG_ELEMENT, pAlarmParm))
	{
		// TODO 错误码
		BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Set Local Alarm Config Failed.","Reason:Insert cdata to xml failed.");
		return IVS_FAIL;
	}

	return IVS_SUCCEED;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetLocalAlarmConfig(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);

	std::string str = m_pLocalConfig.GetCDATA2XML(LOCAL_CONFING_FILE_NAME,ALARM_CONFIG_ELEMENT);
	BP_RUN_LOG_INF("IVS OCX Get Local Alarm Config End.","Value:%s", str.c_str());

	return _com_util::ConvertStringToBSTR(str.c_str());
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_SetLocalRecordConfig(LPCTSTR pRecordParm)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pRecordParm)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Local Record Config Failed.", "Reason:pRecordParm is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,RecordParm: %s", this,pRecordParm);
	if (!m_pLocalConfig.SetCDATA2XML(LOCAL_CONFING_FILE_NAME, RECORD_CONFIG_ELEMENT, pRecordParm))
	{
		// TODO 错误码
		BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Set Local Record Config Failed.","Reason:Insert cdata to xml failed.");
		return IVS_FAIL;
	}
	return IVS_SUCCEED;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetLocalRecordConfig(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	std::string str = m_pLocalConfig.GetCDATA2XML(LOCAL_CONFING_FILE_NAME,RECORD_CONFIG_ELEMENT);
	BP_RUN_LOG_INF("IVS OCX Get Local Record Config.","Value:%s", str.c_str());

	return _com_util::ConvertStringToBSTR(str.c_str());
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_SetLocalCaptureConfig(LPCTSTR pCaptureParm)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pCaptureParm)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Local Capture Config Failed.", "Reason:pCaptureParm is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,CaptureParm: %s", this,pCaptureParm);
	if (!m_pLocalConfig.SetCDATA2XML(LOCAL_CONFING_FILE_NAME, CAPTURE_CONFIG_ELEMENT, pCaptureParm))
	{
		// TODO 错误码
		BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Set Local Capture Config Failed.","Reason:insert cdata to xml failed");
		return IVS_FAIL;
	}
	return IVS_SUCCEED;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetLocalCaptureConfig(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	std::string str = m_pLocalConfig.GetCDATA2XML(LOCAL_CONFING_FILE_NAME,CAPTURE_CONFIG_ELEMENT);
	BP_RUN_LOG_INF("IVS OCX Get Local Capture Config End.","Value:%s", str.c_str());

	return _com_util::ConvertStringToBSTR(str.c_str());
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_SetLocalNetWorkConfig(LPCTSTR pNetWorkParm)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pNetWorkParm)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Local Net Work Config Failed.", "Reason:pNetWorkParm is NULL");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,NetWorkParm: %s", this,pNetWorkParm);
	if (!m_pLocalConfig.SetCDATA2XML(LOCAL_CONFING_FILE_NAME, NETWORK_CONFIG_ELEMENT, pNetWorkParm))
	{
		// TODO 错误码
		BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Set Local Net Work Config Failed.","Reason:insert cdata to xml failed.");
		return IVS_FAIL;
	}
	return IVS_SUCCEED;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetLocalNetWorkConfig(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	std::string str = m_pLocalConfig.GetCDATA2XML(LOCAL_CONFING_FILE_NAME,NETWORK_CONFIG_ELEMENT);
	BP_RUN_LOG_INF("IVS OCX Get Local NetWork Config.","Value:%s", str.c_str());

	return _com_util::ConvertStringToBSTR(str.c_str());
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_SetLocalPTZConfig(LPCTSTR pPTZParm)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pPTZParm)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set LocalPTZ Config Failed.", "Reason:pPTZParm is NULL");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,PTZParm %s", this,pPTZParm);
	if (!m_pLocalConfig.SetCDATA2XML(LOCAL_CONFING_FILE_NAME, PTZ_CONFIG_ELEMENT, pPTZParm))
	{
		// TODO 错误码
		BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Set LocalPTZ Config Failed.","Reason:Insert cdata to xml failed.");
		return IVS_FAIL;
	}
	return IVS_SUCCEED;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetLocalPTZConfig(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	std::string str = m_pLocalConfig.GetCDATA2XML(LOCAL_CONFING_FILE_NAME,PTZ_CONFIG_ELEMENT);
	BP_RUN_LOG_INF("IVS OCX Get Local PTZ Config.","Value:%s", str.c_str());

	return _com_util::ConvertStringToBSTR(str.c_str());
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_SetLocalCameraLinkConfig(LPCTSTR pCameraCode, ULONG ulLinkMode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Local Camera Link Config Failed.", "Reason:pCameraCode is NULL");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,LinkMode: %lu", this,pCameraCode,ulLinkMode);
	// 判断文件是否存在，不存在就创建一个模板
	std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(LOCAL_CONFING_FILE_NAME);
	if(strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR,"IVS OCX Set Local Camera Link Config Failed.","Reason:file not exist.");
		return IVS_OPEN_FILE_ERROR;
	}

	// 加载XML文件
	TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	CHECK_POINTER(pXmlDoc, IVS_ALLOC_MEMORY_ERROR);
	if (!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		BP_RUN_LOG_INF("load xml failed","rebuild xml");
		// 如果加载失败，重新用模板生成这个XML
		if (!m_pLocalConfig.CreateLocalConfigXML(strPath.c_str()))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR,"IVS OCX Set Local Camera Link Config Failed.","Reason:file not exist.");
			return IVS_CREATE_FILE_ERROR;
		}
		if(!pXmlDoc->LoadFile(strPath.c_str()))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX parse xml Failed.","Reason:load xml failed");
			return IVS_XML_INVALID;
		}
	}
	
	TiXmlElement* pConnListElem = CToolsHelp::FindElementByPath(pXmlDoc,CONNECT_MOD_LIST_ELEMENT);
	if (NULL == pConnListElem)
	{
		IVS_DELETE(pXmlDoc);
		return IVS_XML_INVALID;
	}
	// 遍历是否已经设置了，已经设置了就修改
	TiXmlElement* pNode = pConnListElem->FirstChildElement(CONNECT_MOD_NODE);
	const char* pCode = NULL;
	while(NULL != pNode)
	{
		pCode = pNode->Attribute(DEVCODE_ATTRIBUTE);
		if (NULL == pCode)
		{
			continue;
		}
		// 如果找到一样的code
		if (!strcmp(pCameraCode, pCode))
		{
			if(NULL != pNode->FirstChild() && NULL != pNode->FirstChild()->ToText())
			{
				pNode->FirstChild()->ToText()->SetValue(CToolsHelp::Int2Str((int)ulLinkMode).c_str());
			}
			pXmlDoc->SaveFile(strPath.c_str());
			IVS_DELETE(pXmlDoc);
			return 0 ;
		}
		pNode = pNode->NextSiblingElement(CONNECT_MOD_NODE);
	}

	// 没有找到一样的节点，新增一个节点
	TiXmlElement* pConnElem = NULL;
	try
	{
		pConnElem = new TiXmlElement(CONNECT_MOD_NODE);
		pConnElem->LinkEndChild(new TiXmlText(CToolsHelp::Int2Str((int)ulLinkMode).c_str()));
	}
	catch(...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS OCX Set Local Camera Link Config Failed.","Reason: Try New TiXmlElement Failed.");
		IVS_DELETE(pXmlDoc);
		return IVS_ALLOC_MEMORY_ERROR;
	}

	pConnElem->SetAttribute(DEVCODE_ATTRIBUTE,pCameraCode);
	pConnListElem->LinkEndChild(pConnElem);
	pXmlDoc->SaveFile(strPath.c_str());
	IVS_DELETE(pXmlDoc);

	return IVS_SUCCEED; //lint !e429 随着doc释放
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_GetLocalCameraLinkConfig(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pCameraCode)
	{
		BP_RUN_LOG_INF("IVS OCX Get Local Camera Link Config Failed.", "Reason:pCameraCode is NULL.");
		return MEDIA_TRANS;
	}
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s", this,pCameraCode);
	// 判断文件是否存在，不存在就创建一个模板
	std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(LOCAL_CONFING_FILE_NAME);
	if (strPath.empty())
	{
		BP_RUN_LOG_INF("file not exist","NA");
		return MEDIA_TRANS;
	}

	// 加载XML文件
	TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	CHECK_POINTER(pXmlDoc, IVS_ALLOC_MEMORY_ERROR);
	if (!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		BP_RUN_LOG_INF("load xml failed","rebuild xml");
		// 如果加载失败，重新用模板生成这个XML
		if (!m_pLocalConfig.CreateLocalConfigXML(strPath.c_str()))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_FAIL, "GetLocalCameraLinkConfig","Reason:create file failed");
			return IVS_CREATE_FILE_ERROR;
		}
		if(!pXmlDoc->LoadFile(strPath.c_str()))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX parse xml Failed.","Reason:load xml failed");
			return IVS_XML_INVALID;
		}
	}
	TiXmlElement* pConnListElem = CToolsHelp::FindElementByPath(pXmlDoc,CONNECT_MOD_LIST_ELEMENT);
	if (NULL == pConnListElem)
	{
		IVS_DELETE(pXmlDoc);
		return IVS_XML_INVALID;
	}
	// 遍历是否已经设置了，已经设置了就修改
	TiXmlElement* pNode = pConnListElem->FirstChildElement(CONNECT_MOD_NODE);
	const char* pCode = NULL;
	while(NULL != pNode)
	{
		pCode = pNode->Attribute(DEVCODE_ATTRIBUTE);
		if (NULL == pCode)
		{
			IVS_DELETE(pXmlDoc);
			return IVS_XML_INVALID;
		}
		// 如果找到一样的code
		if (!strcmp(pCameraCode, pCode))
		{
			ULONG uResult = (ULONG)atoi(pNode->GetText());
			IVS_DELETE(pXmlDoc);
			return uResult;
		}
		pNode = pNode->NextSiblingElement(CONNECT_MOD_NODE);
	}
	// 返回默认值
	IVS_DELETE(pXmlDoc);
	return 0;
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_DelLocalCameraLinkConfig(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Delete Local Camera Link Config Failed.", "Reason:pCameraCode is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s", this,pCameraCode);
	// 判断文件是否存在，不存在就创建一个模板
	std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(LOCAL_CONFING_FILE_NAME);
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Delete Local Camera Link Config Failed.","Reason:file not exist");
		return IVS_FAIL;
	}

	// 加载XML文件
	TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	if (!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		// 如果加载失败
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX parse xml Failed.","Reason:load xml failed");
		return IVS_XML_INVALID;
	}

	TiXmlElement* pConnListElem = CToolsHelp::FindElementByPath(pXmlDoc,CONNECT_MOD_LIST_ELEMENT);
	CHECK_POINTER(pConnListElem, IVS_XML_INVALID);
	// 遍历是否已经设置了，已经设置了就修改
	TiXmlElement* pNode = pConnListElem->FirstChildElement(CONNECT_MOD_NODE);
	const char* pCode = NULL;
	while(NULL != pNode)
	{
		pCode = pNode->Attribute(DEVCODE_ATTRIBUTE);
		CHECK_POINTER(pCode, IVS_XML_INVALID);
		// 如果找到一样的code
		if (!strcmp(pCameraCode, pCode))
		{
			pConnListElem->RemoveChild(pNode);
			pXmlDoc->SaveFile(strPath.c_str());
			IVS_DELETE(pXmlDoc);
			return IVS_SUCCEED;
		}
		pNode = pNode->NextSiblingElement(CONNECT_MOD_NODE);
	}
	IVS_DELETE(pXmlDoc);
	return IVS_SUCCEED;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetRecordSearchInfo(LPCTSTR pSearchInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pSearchInfo)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Record Search Info Failed.", "Reason:pSearchInfo is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,SearchInfo: %s", this,pSearchInfo);
	if (!m_pLocalConfig.SetCDATA2XML(LOCAL_CONFING_FILE_NAME, RECORD_SEARCH_INFO_ELEMENT, pSearchInfo))
	{
		// TODO 错误码
		BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Set Record Search Info Failed.","Reason:Insert cdata to xml failed.");
		return IVS_FAIL;
	}

	return IVS_SUCCEED;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRecordSearchInfo(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	std::string str = m_pLocalConfig.GetCDATA2XML(LOCAL_CONFING_FILE_NAME,RECORD_SEARCH_INFO_ELEMENT);
	BP_RUN_LOG_INF("IVS OCX Get Record SearchInfo.","Value:%s", str.c_str());

	// TODO 加上result code
	return _com_util::ConvertStringToBSTR(str.c_str());
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddRecordPlan( LPCTSTR pRecordPlan)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pRecordPlan)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add Record Plan Failed.", "Reason:pRecordPlan is NULL");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x,RecordPlan: %s", this,pRecordPlan);
	CString strResult = OCXRecordMgr::AddRecordPlan(pRecordPlan,m_iSDKSessionID);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_ModifyRecordPlan(LPCTSTR pRecordPlan)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pRecordPlan)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Modify Record Plan Failed.", "Reason:pRecordPlan is NULL");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x,RecordPlan: %s", this,pRecordPlan);
	CString strResult = OCXRecordMgr::ModifyRecordPlan(pRecordPlan,m_iSDKSessionID);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_DeleteRecordPlan(LPCTSTR pDelRecPlanXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pDelRecPlanXML)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Delete Record Plan Failed.", "Reason:pDelRecPlanXML is NULL.");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x,DelRecPlanXML: %s", this,pDelRecPlanXML);
	CString strResult = OCXRecordMgr::DeleteRecordPlan(pDelRecPlanXML,m_iSDKSessionID);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRecordPlan(LPCTSTR pRecordPlanReq)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pRecordPlanReq)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get Record Plan Failed.", "Reason:pRecordPlanReq is NULL");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x,RecordPlanReq: %s", this,pRecordPlanReq);
	CString strResult = OCXRecordMgr::GetRecordPlan(pRecordPlanReq,m_iSDKSessionID);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRecordPolicyByTime(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get Record Policy By Time Failed.", "Reason:pCameraCode is NULL.");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s", this,pCameraCode);
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	//CXml xmlReq;
	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);

	IVS_RECORD_POLICY_TIME stRecordPoly;
	memset(&stRecordPoly,0,sizeof(IVS_RECORD_POLICY_TIME));

	//为结果LIST分配内存
	IVS_UINT32 uiBufSize = sizeof(IVS_RECORD_POLICY_TIME) + sizeof(IVS_UINT32)+1;
	IVS_CHAR* pResultListBuf = IVS_NEW(pResultListBuf, uiBufSize);
	if (NULL == pResultListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS OCX Get Record Policy Failed.","Reason:Try New pResultListBuf Failed.");
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult.AllocSysString();
	}

	memset(pResultListBuf, 0, uiBufSize);

	IVS_RESULT_LIST* pResultList = reinterpret_cast<IVS_RESULT_LIST*>(pResultListBuf);//lint !e826
	iRet = IVS_SDK_GetRecordPolicyByTime(m_iSDKSessionID,pCameraCode,&stRecordPoly);
	if (iRet!=IVS_SUCCEED)
	{
		IVS_DELETE(pResultListBuf, MUILI);

		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Record Policy Failed.","Reason:IVS SDK Get RecordPolicy By Time Failed.");
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult.AllocSysString();
	}

	//拼装响应消息结构体为XML
	if(IVS_SUCCEED != COCXRecordXMLProcess::GetRecordPolicyGetXML(&stRecordPoly,pCameraCode,xmlRsp))
	{
		IVS_DELETE(pResultListBuf, MUILI);

		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("CIVS_OCXPlayerCtrl::IVS_OCX_GetRecordPlan", "ErrorCode =%d", iRet);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Record Policy Failed.","Reason:Get RecordPolicy XML Failed.");
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult.AllocSysString();
	}

	IVS_DELETE(pResultListBuf, MUILI);
	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult.AllocSysString();
}//lint !e529

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetRecordPolicyByTime(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Record Policy By Time Failed.", "Reason:pReqXml is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	//CString strResult;

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Set Record Policy By Time Failed.", "Reason:!xmlReq.Parse(pReqXml)");
		//COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return IVS_XML_INVALID;
	}

	IVS_RECORD_POLICY_TIME sRecordPolicy;
	IVS_CHAR pCameraCode[64] = {0};
	//memset(pCameraCode,0,sizeof(IVS_CHAR));
	memset(&sRecordPolicy,0,sizeof(IVS_RECORD_POLICY_TIME));
	(void)COCXRecordXMLProcess::RecordPolicyPraseXML(&sRecordPolicy,pCameraCode,xmlReq);

	//CXml xmlRsp;
	IVS_INT32 iRet = IVS_SDK_SetRecordPolicyByTime(m_iSDKSessionID,pCameraCode,&sRecordPolicy);

	//if (IVS_SUCCEED == iRet)
	//{
	//	IVS_UINT32 uiLen = 0;
	//	strResult = xmlRsp.GetXMLStream(uiLen);
	//}
	//else
	//{
	//	COCXXmlProcess::GetErrString(strResult, iRet);
	//}
	return (LONG)iRet;
}

/*************************************************************************
	* name       : IVS_OCX_AddDevice
	* description: ocx添加主设备
	* input      :
	* output     : NA
	* return     : 成功返回0，失败返回负数错误码
	* author     ：guandiqun
	* remark     : NA
	* history    : 2012-11-13
	*************************************************************************/
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddDevice(LPCTSTR pReqXml)//lint !e1066
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString strResult;
    if (NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add Device Failed.", "Reason:pReqXml is NULL.");
        COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
        return strResult.AllocSysString();
    }
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

    CXml xmlReq;
    if (!xmlReq.Parse(pReqXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Add Device Failed.", "Reason:!xmlReq.Parse(pReqXml)");
        COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
        return strResult.AllocSysString();
    }

    CXml xmlRsp;
    IVS_INT32 iRet = COCXDeviceMgr::AddDevice(m_iSDKSessionID, xmlReq, xmlRsp);
    if (IVS_SUCCEED == iRet)
    {
        IVS_UINT32 uiLen = 0;
        strResult = xmlRsp.GetXMLStream(uiLen);
    }
    else
    {
        COCXXmlProcess::GetErrString(strResult, iRet);
    }
	// 防止内存中密码出现
	memset((char*)pReqXml,0,strlen(pReqXml));

    return strResult.AllocSysString();//lint !e1013 !e64
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDeviceConfig( LONG iConfigType, LPCTSTR pDevCode )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pDevCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get Device Config Failed.", "Reason:pDevCode is NULL.");
		COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,ConfigType: %ld,DevCode: %s", this,iConfigType,pDevCode);

	CXml xmlTemp;
	IVS_INT32 iRet = COCXDeviceParaConfig::GetDeviceConfig(m_iSDKSessionID, iConfigType, pDevCode, xmlTemp);
	if (IVS_SUCCEED == iRet)
	{
		IVS_UINT32 uiLen = 0;
		strResult = xmlTemp.GetXMLStream(uiLen);
	}
	else
	{
		COCXXmlProcess::GetErrString(strResult, iRet);
	}
	BP_RUN_LOG_INF("IVS OCX Get Device Config End.", "Return Result XML");//lint !e437
	return strResult.AllocSysString();//lint !e1013 !e64
}
//将请求转换为结构体
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetDeviceConfig(LONG iConfigType, LPCTSTR pDevCode, LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NULL == pReqXml || NULL == pDevCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Device Config Failed.", "Reason:NULL == pReqXml || NULL == pDevCode");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,ConfigType: %ld,DevCode: %s,ReqXml: %s", this,iConfigType,pDevCode,pReqXml);

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Set Device Config Failed.", "Reason:!xmlReq.Parse(pReqXml)");
		return IVS_XML_INVALID;
	}
	//unsigned int Len = 0;
	//const IVS_CHAR* pStr = xmlReq.GetXMLStream(Len);

	IVS_INT32 iRet = COCXDeviceParaConfig::SetDeviceConfig(m_iSDKSessionID,iConfigType, (IVS_CHAR*)pDevCode,  xmlReq);

	// 防止内存中密码出现
	memset((char*)pReqXml,0,strlen(pReqXml));

	return (LONG)iRet;
}

// 查询设备，都是使用通用查询请求,只用来查询主设备列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_QueryDeviceList(ULONG ulDeviceType, LPCTSTR pQueryXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString strResult;
    if (NULL == pQueryXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Query Device List Failed.", "Reason:pReqGetList is NULL.");

        COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
        return strResult.AllocSysString();//lint !e1013 !e64
    }
	IVS_INFO_TRACE("OCX Address: %x,DeviceType: %lu,QueryXml: %s", this,ulDeviceType,pQueryXml);

    CXml reqXml;
    //判断是否解析正确
	if (!reqXml.Parse(pQueryXml))
	{
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get MainDevice List Failed.", "Reason:FALSE == reqXml.Parse(pReqGetList)");

        COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
        return strResult.AllocSysString();//lint !e1013 !e64
	}
    CXml xmlRsp;
    IVS_INT32 iRet = COCXDeviceMgr::QueryDeviceList(m_iSDKSessionID, ulDeviceType, reqXml, xmlRsp);
    if (IVS_SUCCEED == iRet)
    {
        IVS_UINT32 uiLen = 0;
        strResult = xmlRsp.GetXMLStream(uiLen);
    }
    else
    {
        COCXXmlProcess::GetErrString(strResult, iRet);
    }

    return strResult.AllocSysString();//lint !e1013 !e64
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_AddKeyword(LPCTSTR pCameraCode, LPCTSTR pDeviceKeyWord)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add Key word Failed.", "Reason:pCameraCode is NULL");
		return IVS_PARA_INVALID;
	}
	if(NULL == pDeviceKeyWord)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add Key word Failed.", "Reason:pDeviceKeyWord is NULL");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,DeviceKeyWord: %s", this,pCameraCode,pDeviceKeyWord);
	// 判断文件是否存在，不存在就创建一个模板
	std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(DEVICE_KEYWORD_FILE_NAME);
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Add Key word Failed.","Reason:file not exist.");
		return IVS_CREATE_FILE_ERROR;
	}

	// 加载XML文件
	TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	CHECK_POINTER(pXmlDoc, IVS_ALLOC_MEMORY_ERROR);
	if (!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		BP_RUN_LOG_INF("load xml failed","rebuild xml");
		// 如果加载失败，重新用模板生成这个XML
		if (!m_pLocalConfig.CreateLocalConfigXML(strPath.c_str()))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Add Key word Failed.","Reason:create file failed");
			return IVS_CREATE_FILE_ERROR;
		}
		if(!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX parse xml Failed.","Reason:load xml failed");
			return IVS_XML_INVALID;
		}
	}

	TiXmlElement* pConnListElem = CToolsHelp::FindElementByPath(pXmlDoc,DEVICE_KEYWORD_ELEMENT);
	if (NULL == pConnListElem)
	{
		IVS_DELETE(pXmlDoc);
		return IVS_XML_INVALID;
	}
	// 遍历是否已经设置了，已经设置了就修改
	TiXmlElement* pNode = pConnListElem->FirstChildElement(DEVICE_KEYWORDD_NODE);
	const char* pCode = NULL;
	while(NULL != pNode)
	{
		pCode = pNode->Attribute(DEVCODE_ATTRIBUTE);
		if (NULL == pCode)
		{
			IVS_DELETE(pXmlDoc);
			return IVS_XML_INVALID;
		}
		// 如果找到一样的code
		if (!strcmp(pCameraCode, pCode))
		{
			pNode->FirstChild()->ToText()->SetValue(pDeviceKeyWord);
			pXmlDoc->SaveFile(strPath.c_str());
			IVS_DELETE(pXmlDoc);
			return 0 ;
		}
		pNode = pNode->NextSiblingElement(DEVICE_KEYWORDD_NODE);
	}

	// 没有找到一样的节点，新增一个节点
	TiXmlElement* pConnElem = NULL;
	try{
		pConnElem = new TiXmlElement(DEVICE_KEYWORDD_NODE);
		pConnElem->LinkEndChild(new TiXmlText(pDeviceKeyWord));
	}
	catch(...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS OCX Add Key word Failed.","Reason:Try New TiXmlElement Failed.");
		IVS_DELETE(pXmlDoc);
		return IVS_ALLOC_MEMORY_ERROR;
	}

	pConnElem->SetAttribute(DEVCODE_ATTRIBUTE,pCameraCode);
	pConnListElem->LinkEndChild(pConnElem);
	pXmlDoc->SaveFile(strPath.c_str());
	IVS_DELETE(pXmlDoc);

	return 0; //lint !e429 随着doc释放
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_DelKeyword(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Del Key word Failed.", "Reason:pCameraCode is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s", this,pCameraCode);
	// 判断文件是否存在，不存在就创建一个模板
	std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(DEVICE_KEYWORD_FILE_NAME);
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Del Key word Failed.","Reason:file not exist");
		return IVS_CREATE_FILE_ERROR;
	}

	// 加载XML文件
	TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	CHECK_POINTER(pXmlDoc, IVS_ALLOC_MEMORY_ERROR);
	if (!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		BP_RUN_LOG_INF("load xml failed","rebuild xml");
		// 如果加载失败，重新用模板生成这个XML
		if (!m_pLocalConfig.CreateLocalConfigXML(strPath.c_str()))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Del Key word Failed.","Reason:create file failed");
			return IVS_CREATE_FILE_ERROR;
		}
		if(!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX parse xml Failed.","Reason:load xml failed");
			return IVS_XML_INVALID;
		}
	}

	TiXmlElement* pConnListElem = CToolsHelp::FindElementByPath(pXmlDoc, DEVICE_KEYWORD_ELEMENT);
	if (NULL == pConnListElem)
	{
		IVS_DELETE(pXmlDoc);
		return IVS_XML_INVALID;
	}
	// 遍历是否已经设置了，已经设置了就修改
	TiXmlElement* pNode = pConnListElem->FirstChildElement(DEVICE_KEYWORDD_NODE);
	const char* pCode = NULL;
	while(NULL != pNode)
	{
		pCode = pNode->Attribute(DEVCODE_ATTRIBUTE);
		if (NULL == pCode)
		{
			IVS_DELETE(pXmlDoc);
			return IVS_XML_INVALID;
		}
		// 如果找到一样的code
		if (!strcmp(pCameraCode, pCode))
		{
			pConnListElem->RemoveChild(pNode);
			pXmlDoc->SaveFile(strPath.c_str());
			IVS_DELETE(pXmlDoc);
			return IVS_SUCCEED;
		}
		pNode = pNode->NextSiblingElement(DEVICE_KEYWORDD_NODE);
	}

	IVS_DELETE(pXmlDoc);
	return IVS_SUCCEED;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetKeyword(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);

	CString cstrStream;

	try
	{
		// 判断文件是否存在，不存在就创建一个模板
		std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(DEVICE_KEYWORD_FILE_NAME);
		if (strPath.empty())
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get Key Word Failed.","Reason:File not exist.");
			return NULL;
		}

		// 读取XML，保存参数
		CXml xml;
		if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			// 如果加载失败，重新用模板生成这个XML
			if (!m_pLocalConfig.CreateLocalConfigXML(strPath.c_str()))
			{
				BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get Key Word Failed.","Reason:Create Local Config xml Failed.");
				return NULL;
			}
			// 如果再失败。。
			if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
			{
				BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get Key Word Failed.","Reason:load xml failed");
				return NULL;
			}
		}

		unsigned int iXMLSize = 0;
		const char* xmlStrame = xml.GetXMLStream(iXMLSize);
		if (NULL == xmlStrame)
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get Key Word Failed.","Get xml stream is null.");
			return NULL;
		}

		cstrStream = xmlStrame;
	}
	catch(...)
	{
		DWORD errCode = ::GetLastError();
		BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get Key Word.","Throw exception, errcode[%lu].", errCode);
	}

	return	cstrStream.AllocSysString();//_com_util::ConvertStringToBSTR(xmlStrame);
}

// 查询操作日志
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetOperationLog(LPCTSTR pUnifiedQuery)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());//lint !e534
	CString strResult;
	// 入参判空
	if (NULL == pUnifiedQuery)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Operation Log Failed","Reason:pUnifiedQuery is NULL");//lint !e534
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,UnifiedQuery: %s", this,pUnifiedQuery);

    IVS_INT32 iRet = COCXLogMgr::GetOperationLog(m_iSDKSessionID, pUnifiedQuery, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Operation Log Failed.", "Reason:LogMgr Get Operation Log Failed.");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}
// 新增告警级别
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddAlarmLevel(LPCTSTR pAlarmLevel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	IVS_UINT32 iNum = 0;
	if (NULL == pAlarmLevel)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Add Alarm Level Failed.","Reason: pAlarmLevel is NULL.");
		COCXAlarmMgrXMLProcess::GetAddAlarmErrString(strResult, iNum, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,AlarmLevel: %s", this,pAlarmLevel);

	COCXAlarmMgr::AddAlarmLevel(m_iSDKSessionID, (IVS_CHAR*)pAlarmLevel, strResult);
	return strResult.AllocSysString();
}

// 修改告警级别
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyAlarmLevel(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Modify Alarm Level Failed.", "Reason:pReqXml is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXAlarmMgr::ModifyAlarmLevel(m_iSDKSessionID, (IVS_CHAR*)pReqXml);
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteAlarmLevel(ULONG ulAlarmLevelID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,AlarmLevelID: %lu", this,ulAlarmLevelID);
	IVS_INT32 iRet = COCXAlarmMgr::DeleteAlarmLevel(m_iSDKSessionID, ulAlarmLevelID);
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetRecordSpace(LPCTSTR pRecordSpace)
{
	if(NULL == pRecordSpace)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Record Space Failed.", "Reason:pRecordSpace is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,RecordSpace: %s", this,pRecordSpace);
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

 //   // 1.STR 转为 XML
	//CXml xml;
	////long iLent = 0;
	//CHECK_POINTER(pRecordSpace,IVS_FAIL);
	//xml.Parse(pRecordSpace);

	//// 2.XML 转为 struct
	//IVS_RECORD_POLICY_TIME sRecordSpace;
 //   IVS_CHAR pCameraCode[64];
 //   memset(pCameraCode,0,sizeof(pCameraCode));
 //   IVS_INT32 pRecordMethod = NULL;
 //   memset(&sRecordSpace,0,sizeof(IVS_RECORD_POLICY_TIME));
	//(void)COCXRecordXMLProcess::RecordSpacePraseXML(&sRecordSpace,pCameraCode,pRecordMethod,xml);

	// 3.传入strct 调用 sdk
	//封装SDK
    /*strncpy_s(sRecordSpace.cCameraCode,pCameraCode,IVS_CAMERA_LEN);*/
	/*IVS_INT32 iRet = IVS_SDK_SetRecordSpace(m_iSDKSessionID ,sRecordSpace->cCameraCode,sRecordSpace);*/
    IVS_INT32 iRet = 0;
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteRecordSpace(LPCTSTR pRecordSpace)
{
	if(NULL == pRecordSpace)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Delete Record Space Failed.", "Reason:pRecordSpace is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,RecordSpace: %s", this,pRecordSpace);
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

 //   //1.STR 转为 XML
	//CXml xml;
	////long iLent = 0;
	//CHECK_POINTER(pRecordSpace,IVS_FAIL);
	//xml.Parse(pRecordSpace);

	////2.XML 转为 struct
	//IVS_RECORD_POLICY_TIME sRecordSpace;
 //   IVS_CHAR pCameraCode[64];
 //   memset(pCameraCode,0,sizeof(IVS_CHAR));

 //   IVS_INT32 pRecordMethod = 0;
 //   memset(&sRecordSpace,0,sizeof(IVS_RECORD_POLICY_TIME));
	//(void)COCXRecordXMLProcess::RecordSpacePraseXML(&sRecordSpace,pCameraCode,pRecordMethod,xml);

	//3.传入strct 调用 sdk
	//封装SDK
    /*sRecordSpace->iRecordMethod = pRecordMethod;*/
    /*strncpy_s(sRecordSpace.cCameraCode,pCameraCode,IVS_CAMERA_LEN);*/
	/*IVS_INT32 iRet = IVS_SDK_SetRecordSpace(m_iSDKSessionID ,sRecordSpace->cCameraCode,sRecordSpace->iRecordMethod);*/
    IVS_INT32 iRet = 0;
	return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyRecordSpace(LPCTSTR pRecordSpace)
{
	if(NULL == pRecordSpace)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Modify Record Space Failed.", "Reason:pRecordSpace is NULL");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,RecordSpace: %s", this,pRecordSpace);
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

 //    //1.STR 转为 XML
	//CXml xml;
	////long iLent = 0;
	//CHECK_POINTER(pRecordSpace,IVS_FAIL);
	//xml.Parse(pRecordSpace);

	////2.XML 转为 struct
	//IVS_RECORD_POLICY_TIME sRecordSpace;
 //   IVS_CHAR pCameraCode[64];
 //   memset(pCameraCode,0,sizeof(IVS_CHAR));
 //   IVS_INT32 pRecordMethod = 0;
 //   memset(&sRecordSpace,0,sizeof(IVS_RECORD_POLICY_TIME));
	//(void)COCXRecordXMLProcess::RecordSpacePraseXML(&sRecordSpace,pCameraCode,pRecordMethod,xml);

	//3.传入strct 调用 sdk
	//封装SDK
    /*strncpy_s(sRecordSpace.cCameraCode,pCameraCode,IVS_CAMERA_LEN);*/
	/*IVS_INT32 iRet = IVS_SDK_ModifyRecordSpace(m_iSDKSessionID ,sRecordSpace->cCameraCode, sRecordSpace);*/
    IVS_INT32 iRet = 0;
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_GetRecordSpace(LPCTSTR pRecordSpace)
{
	if(NULL == pRecordSpace)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get Record Space Failed.", "Reason:pRecordSpace is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,RecordSpace: %s", this,pRecordSpace);
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    //1.STR 转为 XML
	//CXml xml;
	////long iLent = 0;
	//CHECK_POINTER(pRecordSpace,IVS_FAIL);
	//xml.Parse(pRecordSpace);

	////2.XML 转为 struct
	//IVS_RECORD_POLICY_TIME sRecordSpace;
 //   IVS_CHAR pCameraCode[64];
 //   memset(pCameraCode,0,sizeof(IVS_CHAR));
 //   IVS_INT32 pRecordMethod = NULL;
 //   memset(&sRecordSpace,0,sizeof(IVS_RECORD_POLICY_TIME));
	//(void)COCXRecordXMLProcess::RecordSpacePraseXML(&sRecordSpace,pCameraCode,pRecordMethod,xml);

	//3.传入strct 调用 sdk
	//封装SDK
    /*sRecordSpace->iRecordMethod = pRecordMethod;*/
    /*strncpy_s(sRecordSpace.cCameraCode,pCameraCode,IVS_CAMERA_LEN);*/
	/*IVS_INT32 iRet = IVS_SDK_SetRecordSpace(m_iSDKSessionID ,sRecordSpace->cCameraCode,sRecordSpace->iRecordMethod,sRecordSpace);*/
    IVS_INT32 iRet = 0;
    return iRet;
}

// 新增告警帮助
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AddHelpInfo(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add Help Info Failed.", "Reason:pReqXml is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	IVS_INT32 iRet = COCXAlarmMgr::AddHelpInfo(m_iSDKSessionID, (IVS_CHAR*)pReqXml);
    return iRet;
}

// 修改告警帮助
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyHelpInfo(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	IVS_INT32 iRet = COCXAlarmMgr::ModifyHelpInfo(m_iSDKSessionID, (IVS_CHAR*)pReqXml);
    return iRet;
}

// 删除告警帮助
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteHelpInfo(LPCTSTR helpInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,helpInfo: %s", this,helpInfo);

	IVS_INT32 iRet = COCXAlarmMgr::DeleteHelpInfo(m_iSDKSessionID, helpInfo);
	return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetCustomWndLayout(ULONG ulLayoutID, LPCTSTR pLayoutXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pLayoutXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set CustomWnd Layout Failed.", "Reason:pLayoutXml is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,LayoutID: %lu,LayoutXml: %s", this,ulLayoutID,pLayoutXml);

	// 判断文件是否存在，不存在就创建一个模板
	std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(CUSTOM_LAYOUT_CONFIG_FILE_NAME);
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Set CustomWnd Layout Failed.","Reason:file not exist.");
		return IVS_FAIL;
	}

	// 加载XML文件
	TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	CHECK_POINTER(pXmlDoc, IVS_ALLOC_MEMORY_ERROR);
	if (!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		BP_RUN_LOG_INF("load xml failed","rebuild xml");
		// 如果加载失败，重新用模板生成这个XML
		if (!m_pLocalConfig.CreateLocalConfigXML(strPath.c_str()))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Set CustomWnd Layout Failed.","Reason:create file failed");
			return IVS_CREATE_FILE_ERROR;
		}
		if(!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX parse xml Failed.","Reason:load xml failed");
			return IVS_XML_INVALID;
		}
	}

	TiXmlElement* pConnListElem = CToolsHelp::FindElementByPath(pXmlDoc,CUSTOM_LAYOUT_LIST_ELEMENT);
	if (NULL == pConnListElem)
	{
		IVS_DELETE(pXmlDoc);
		BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Set Custom Wnd Layout Failed.","Reason:pConnListElem is NULL");
		return IVS_FAIL;
	}
	// 遍历是否已经设置了，已经设置了就修改
	TiXmlElement* pNode = pConnListElem->FirstChildElement(CUSTOM_LAYOUT_NODE);
	const char* pCode = NULL;
	while(NULL != pNode)
	{
		pCode = pNode->Attribute(LAYOUT_ATTRIBUTE);
		CHECK_POINTER(pCode, IVS_XML_INVALID);
		// 如果找到一样的code
		if (ulLayoutID == (ULONG)atoi(pCode))
		{
			if(NULL != pNode->FirstChild() && NULL != pNode->FirstChild()->ToText())
			{
				pNode->FirstChild()->ToText()->SetCDATA(true);
				pNode->FirstChild()->ToText()->SetValue(pLayoutXml);
			}
			pXmlDoc->SaveFile(strPath.c_str());
			IVS_DELETE(pXmlDoc);
			return IVS_SUCCEED ;
		}
		pNode = pNode->NextSiblingElement(CUSTOM_LAYOUT_NODE);
	}

	// 没有找到一样的节点，新增一个节点
	TiXmlElement* pLayoutElem = NULL;
	TiXmlText* pXmlText = NULL;
	try{
		pLayoutElem = new TiXmlElement(CUSTOM_LAYOUT_NODE);
		pXmlText = new TiXmlText(pLayoutXml);
	}
	catch(...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS OCX Set Custom Wnd Layout Failed.", "Reason:New Memory Failed.");
		IVS_DELETE(pXmlDoc);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	pXmlText->SetCDATA(true);
	pLayoutElem->LinkEndChild(pXmlText);
	// 转换成字符
	char layoutID[64] = {0};
	itoa((int)ulLayoutID,layoutID,10);
	pLayoutElem->SetAttribute(LAYOUT_ATTRIBUTE,layoutID);
	pConnListElem->LinkEndChild(pLayoutElem);
	pXmlDoc->SaveFile(strPath.c_str());
	IVS_DELETE(pXmlDoc);

	return IVS_SUCCEED; //lint !e429
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCustomWndLayout(ULONG ulLayoutID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,LayoutID: %lu", this,ulLayoutID);

	// 判断文件是否存在，不存在就创建一个模板
	std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(CUSTOM_LAYOUT_CONFIG_FILE_NAME);
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get Custom Wnd Layout Failed.","Reason:file not exist");
		return NULL;
	}

	// 加载XML文件
	TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	CHECK_POINTER(pXmlDoc, NULL);
	if (!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		BP_RUN_LOG_INF("load xml failed","rebuild xml");
		// 如果加载失败，重新用模板生成这个XML
		if (!m_pLocalConfig.CreateLocalConfigXML(strPath.c_str()))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get Custom Wnd Layout Failed.","Reason:file create file failed");
			return NULL;
		}
		if(!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Custom Wnd Layout Failed.","Reason:load xml failed");
			return NULL;
		}
	}
	TiXmlElement* pConnListElem = CToolsHelp::FindElementByPath(pXmlDoc,CUSTOM_LAYOUT_LIST_ELEMENT);
	if (NULL == pConnListElem)
	{
		IVS_DELETE(pXmlDoc);
		BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get Custom Wnd Layout Failed.","Reason:pConnListElem is NULL.");
		return NULL;
	}

	// 遍历是否存在这个布局
	TiXmlElement* pNode = pConnListElem->FirstChildElement(CUSTOM_LAYOUT_NODE);
	if (NULL == pNode)
	{
		IVS_DELETE(pXmlDoc);
		return NULL;
	}
	const char* pCode = NULL;
	while(NULL != pNode)
	{
		pCode = pNode->Attribute(LAYOUT_ATTRIBUTE);
		if (NULL == pCode)
		{
			continue;
		}
		// 如果找到一样的code
		if (ulLayoutID == (ULONG)atoi(pCode))
		{
			std::string strLayoutXML = pNode->GetText();
			IVS_DELETE(pXmlDoc);
			return _com_util::ConvertStringToBSTR(strLayoutXML.c_str());
		}
		pNode = pNode->NextSiblingElement(CUSTOM_LAYOUT_NODE);
	}
	// 没有找到返回空串
	IVS_DELETE(pXmlDoc);
	return NULL;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRecordList(LPCTSTR pCameraCode,ULONG lRecordMethod,LPCTSTR pStartTime,LPCTSTR pEndTime,ULONG lFromIndex,ULONG lToIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX GetRecordList Failed.", "Reason:pCameraCode is NULL");
		return NULL;
	}
	if(NULL == pStartTime)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX GetRecordList Failed.", "Reason:pStartTime is NULL");
		return NULL;
	}
	if(NULL == pEndTime)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX GetRecordList Failed.", "Reason:pEndTime is NULL");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,RecordMethod: %lu,StartTime: %s,EndTime: %s,FromIndex: %lu,loIndex: %lu",
		            this,pCameraCode,lRecordMethod,pStartTime,pEndTime,lFromIndex,lToIndex);
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_FAIL);
	IVS_UINT32 uiTotal = lToIndex - lFromIndex;
	IVS_UINT32 uiBufSize = sizeof(IVS_RECORD_INFO_LIST)+sizeof(IVS_RECORD_INFO)*uiTotal;
	IVS_CHAR* pRecordListBuf = IVS_NEW(pRecordListBuf, uiBufSize);
	if (NULL == pRecordListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);

		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Record List Failed.", "Reason:New pRecordListBuf Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	memset(pRecordListBuf, 0, uiBufSize);

	IVS_RECORD_INFO_LIST* pRecordList = (IVS_RECORD_INFO_LIST*)pRecordListBuf;//lint !e826
	IVS_TIME_SPAN stTimeSpan;
	memset(&stTimeSpan,0,sizeof(IVS_TIME_SPAN));
	if(!CToolsHelp::Memcpy(stTimeSpan.cStart,IVS_TIME_LEN,pStartTime,strlen(pStartTime)))
	{
		IVS_DELETE(pRecordListBuf, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS OCX Get Record List Failed.", "Reason:stTimeSpan.cStart Memcpy error.");
		COCXXmlProcess::GetErrString(strResult,IVS_ALLOC_MEMORY_ERROR);
		return strResult.AllocSysString();
	}
	if(!CToolsHelp::Memcpy(stTimeSpan.cEnd,IVS_TIME_LEN,pEndTime,strlen(pEndTime)))
	{
		IVS_DELETE(pRecordListBuf, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS OCX Get Record List Failed", "Reason:stTimeSpan.cEnd Memcpy error.");
		COCXXmlProcess::GetErrString(strResult,IVS_ALLOC_MEMORY_ERROR);
		return strResult.AllocSysString();
	}
	IVS_INDEX_RANGE stIndex;
	memset(&stIndex,0,sizeof(IVS_INDEX_RANGE));
	stIndex.uiFromIndex = static_cast<IVS_UINT32>(lFromIndex);
	stIndex.uiToIndex = static_cast<IVS_UINT32>(lToIndex);

	iRet = IVS_SDK_GetRecordList(m_iSDKSessionID,pCameraCode,static_cast<IVS_INT32>(lRecordMethod),&stTimeSpan,&stIndex,pRecordList,uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pRecordListBuf, MUILI);

		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Record List Failed.", "Reason:SDK Get Record List Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	//拼装响应消息结构体为XML
	iRet = COCXRecordXMLProcess::GetRecordListGetXML(pCameraCode,pRecordList,iRet,xmlRsp);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pRecordListBuf, MUILI);

		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Record List Failed", "Reason:Get Record List XML Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	IVS_DELETE(pRecordListBuf, MUILI);

	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_QueryRecordList(LPCTSTR pUnifiedQuery)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pUnifiedQuery)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Query Record List Failed.", "Reason:pUnifiedQuery is NULL.");
		return NULL;
	}

    IVS_INFO_TRACE("OCX Address: %x,UnifiedQuery: %s", this,pUnifiedQuery);
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlReq;
	CXml xmlRsp;
	IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	if (NULL == xmlReq.Parse(pUnifiedQuery))
	{
		iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);

		BP_RUN_LOG_ERR(iRet,"IVS OCX Query Record List Failed", "Reason: Parse pUnifiedQuery to Xml Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	IVS_INT32 iFieldNum = 0;
	iFieldNum = COCXXmlProcess::GetFieldNum(xmlReq);

	//分配内存
	IVS_VOID* pQueryFiledBuff = NULL;
	iRet = COCXXmlProcess::GetUnifiedFormatQueryParseXML(pQueryFiledBuff,xmlReq,iFieldNum);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Query Record List Failed.", "Reason:Get Unified Format Query Parse XML Failed.", iRet);
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	IVS_QUERY_UNIFIED_FORMAT* pQueryUnifiedFormat = (IVS_QUERY_UNIFIED_FORMAT*)pQueryFiledBuff;

	iRet = COCXRecordXMLProcess::GetQueryUnifiedCameraCode(xmlReq,cCameraCode);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Query Record List Failed", "Reason:Get Query Unified CameraCode XML Failed", iRet);
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	IVS_UINT32 uiTotal = (pQueryUnifiedFormat->stIndex).uiToIndex - (pQueryUnifiedFormat->stIndex).uiFromIndex;
	IVS_UINT32 uiBufSize = sizeof(IVS_RECORD_INFO_LIST)+sizeof(IVS_RECORD_INFO)*uiTotal;
	IVS_CHAR* pRecordListBuf = IVS_NEW(pRecordListBuf, uiBufSize);
	if (NULL == pRecordListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		IVS_DELETE(pQueryFiledBuff, MUILI);

		BP_RUN_LOG_ERR(iRet,"IVS OCX Query Record List Failed.", "Reason:New pRecordListBuf Memory Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	memset(pRecordListBuf, 0, uiBufSize);

	IVS_RECORD_INFO_LIST* pRecordList = (IVS_RECORD_INFO_LIST*)pRecordListBuf;//lint !e826
	iRet = IVS_SDK_QueryRecordList(m_iSDKSessionID,pQueryUnifiedFormat,pRecordList,uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		IVS_DELETE(pRecordListBuf, MUILI);

		BP_RUN_LOG_ERR(iRet,"IVS OCX Query Record List Failed.", "Reason:SDK Query Record List Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	//拼装响应消息结构体为XML
	iRet = COCXRecordXMLProcess::GetRecordListGetXML(cCameraCode,pRecordList,iRet,xmlRsp);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		IVS_DELETE(pRecordListBuf, MUILI);

		BP_RUN_LOG_ERR(iRet,"IVS OCX Query Record List Failed", "Get Record List Response XML Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	IVS_DELETE(pQueryFiledBuff, MUILI);
	IVS_DELETE(pRecordListBuf, MUILI);
	const char* pXml = xmlRsp.GetXMLStream(uiXmlLen);
	if (NULL == pXml)
	{
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	strResult = pXml;

    BP_RUN_LOG_INF("IVS OCX Query Record List.", "Result:%s", pXml);
	return strResult.AllocSysString();
}

BOOL CIVS_OCXPlayerCtrl::IVS_OCX_GetGPUDecodeAbility(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
    BOOL bRet = (BOOL)IVS_SDK_GetGPUDecodeAbility();
    return bRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_EnableGPU(BOOL bEnable)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    IVS_INFO_TRACE("OCX Address: %x,Enable: %d", this,(int)bEnable);
    IVS_SDK_EnableGPU(bEnable);

    return 0;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetNVRList(LPCTSTR pUnifiedQuery)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString strResult;
    if (NULL == pUnifiedQuery)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get NVRList Failed.", "Reason:pReqXmlis NULL.");
        COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
        return strResult.AllocSysString();
    }
	IVS_INFO_TRACE("OCX Address: %x,UnifiedQuery: %s", this,pUnifiedQuery);

	CXml xmlReq;
	if (FALSE == xmlReq.Parse(pUnifiedQuery))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID,"IVS OCX Get NVRList Failed.", "Reason:Parse pUnifiedQuery Failed.");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	COCXDeviceMgr::GetNVRList(m_iSDKSessionID, xmlReq, strResult);
	return strResult.AllocSysString();
}
//查询设备组
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDeviceGroup(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
    if (NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get Device Group Failed.", "Reason:pReqXml is NULL.");
        COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
        return strResult.AllocSysString();//lint !e1013 !e64
    }
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

    CXml reqXml;
    //判断是否解析正确
    if (!reqXml.Parse(pReqXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Device Group Failed.", "Reason:Parse pReqGetList Failed.");
        COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
        return strResult.AllocSysString();//lint !e1013 !e64
    }

    COCXDeviceGroupMgr::GetDeviceGroup(m_iSDKSessionID, reqXml, strResult);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDeviceInfo(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString strResult;
    if (NULL == pReqXml)
    {
        COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Device Info", "Reason:pReqXml is NULL.");
        return strResult.AllocSysString();
    }
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

    IVS_INT32 iRet = IVS_XML_INVALID;

    CXml xmlReq;
    if (!xmlReq.Parse(pReqXml))
    {
        COCXXmlProcess::GetErrString(strResult, iRet);
        BP_RUN_LOG_ERR(iRet, "IVS OCX Get Device Info Failed.", "Parse pReqXml Failed.");
        return strResult.AllocSysString();
    }

    COCXDeviceMgr::GetDeviceInfo(m_iSDKSessionID, xmlReq, strResult);
    return strResult.AllocSysString();
}

/*************************************************************************
* name       : IVS_OCX_DeleteDevice
* description: 删除设备
* input      : LPCTSTR pReqXml
* output     : BSTR
* return     : 返回xml
* anthor     ：guandiqun
* remark     : NA
*************************************************************************/
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_DeleteDevice(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Delete Device Failed.", "Reason:pReqXml is NULL.");
		return NULL;
	}
    IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

    CString strResult;

	COCXDeviceMgr::DeleteDevice(m_iSDKSessionID, strResult, pReqXml);
	return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyDeviceGroupName(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Modify Device Group Name Failed.", "Reason:pReqXml is NULL.");
		return IVS_PARA_INVALID;
	}
    IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);
    CXml xmlReq;
    if (!xmlReq.Parse(pReqXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Modify Device Group Name.", "Reason:Parse pReqXml Failed.");
        return IVS_XML_INVALID;
    }

    IVS_INT32 iRet = COCXDeviceGroupMgr::ModifyDeviceGroupName(m_iSDKSessionID, xmlReq);
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_MoveDeviceGroup(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Move Device Group Failed.", "Reason:pReqXml is NULL.");
		return IVS_PARA_INVALID;
	}
    IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);

    CXml xmlReq;
    if (!xmlReq.Parse(pReqXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Move Device Group Failed.", "Reason:Parse pReqXml Failed.");
        return IVS_XML_INVALID;
    }

    IVS_INT32 iRet = COCXDeviceGroupMgr::MoveDeviceGroup(m_iSDKSessionID, xmlReq);
    return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddDeviceGroup(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString strResult;
    if (NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add Device Group Failed.", "Reason:pReqXml is NULL.");
        COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
        return strResult.AllocSysString();//lint !e1013 !e64
    }
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);

    CXml xmlReq;
    if (!xmlReq.Parse(pReqXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Add Device Group Failed.", "Reason:Parse pReqXml Failed.");
        COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
        return strResult.AllocSysString();//lint !e1013 !e64
    }

    COCXDeviceGroupMgr::AddDeviceGroup(m_iSDKSessionID, xmlReq, strResult);
    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_AddDeviceToGroup(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add Device To Group Failed.", "Reason:pReqXmlis NULL.");
        return IVS_PARA_INVALID;
    }
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);

    CXml xmlReq;
    if (!xmlReq.Parse(pReqXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Add Device To Group Failed.", "Reason:Parse pReqXml Failed.");
        return IVS_XML_INVALID;
    }

    CXml xmlRsp;
    IVS_INT32 iRet = COCXDeviceGroupMgr::AddDeviceToGroup(m_iSDKSessionID, xmlReq);
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteDeviceFromGroup(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Delete Device From Group Failed.", "Reason:pReqXml is NULL.");
        return IVS_PARA_INVALID;
    }
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);

    CXml xmlReq;
    if (!xmlReq.Parse(pReqXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Delete Device From Group Failed.", "Reason:Parse pReqXml Failed.");
        return IVS_XML_INVALID;
    }

    IVS_INT32 iRet = COCXDeviceGroupMgr::DeleteDeviceFromGroup(m_iSDKSessionID, xmlReq);
    return iRet;//lint !e1013 !e64
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteDeviceGroup(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Delete Device Group Failed.", "Reason:pReqXml is NULL.");
        return IVS_PARA_INVALID;
    }
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);

    CXml xmlReq;
    if (!xmlReq.Parse(pReqXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Delete Device Group Failed", "Reason:parse pReqXml Failed.");
        return IVS_XML_INVALID;
    }

    IVS_INT32 iRet = COCXDeviceGroupMgr::DeleteDeviceGroup(m_iSDKSessionID, xmlReq);
    return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAlarmLevelList()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);

	CString strResult;
	COCXAlarmMgr::GetAlarmLevelList(m_iSDKSessionID, strResult);
	return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetToolbarButtonStatus(LONG lWndID, ULONG ulToolbarBtn, ULONG ulButtonStatus)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(m_pVideoPaneMgr,IVS_FAIL);
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld,ToolbarBtn: %lu,ButtonStatus: %lu", this,lWndID,ulToolbarBtn,ulButtonStatus);

	if (!m_pVideoPaneMgr->isPaneExistByID(static_cast<IVS_ULONG>(lWndID)))
	{
		return IVS_PARA_INVALID;
	}

	std::bitset<TOOLBARBTNNUM> bitsetEnableStatus(static_cast<ULONGLONG>(ulToolbarBtn));

// 	if (bitsetEnableStatus.count() != 1)
// 	{
// 		return IVS_PARA_INVALID;
// 	}

	if (ulButtonStatus != 0 && ulButtonStatus != 1 )
	{
		return IVS_PARA_INVALID;
	}
	return m_pVideoPaneMgr->SetToolbarEnableStatus(static_cast<IVS_ULONG>(lWndID),
												static_cast<ULONGLONG>(ulToolbarBtn),
												static_cast<ULONGLONG>( ulButtonStatus));
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ShowTitlebar(VARIANT_BOOL bShow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,Show: %d", this,(int)bShow);
    CHECK_POINTER(m_pVideoPaneMgr,IVS_FAIL);
	if(static_cast<BOOL>(bShow))
	{
		m_pVideoPaneMgr->ShowTitleBar(true);
	}
	else
	{
		m_pVideoPaneMgr->ShowTitleBar(false);
	}
	return IVS_SUCCEED;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ShowToolbar(VARIANT_BOOL bShow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,Show: %d", this,(int)bShow);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_FAIL);
	if(static_cast<BOOL>(bShow))
	{
		m_pVideoPaneMgr->ShowToolBar(true);
	}
	else
	{
		m_pVideoPaneMgr->ShowToolBar(false);
	}
	return IVS_SUCCEED;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAlarmTypeList(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Alarm Type List Failed.","Reason:pReqXml is NULL");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	COCXAlarmMgr::GetAlarmTypeList(m_iSDKSessionID, (IVS_CHAR*)pReqXml, strResult);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_Aes256CbcEncode(LPCTSTR pszPlain)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(pszPlain, NULL);
	IVS_INFO_TRACE("OCX Address: %x", this);
    CString strResult;

	//分配密文空间
	char* cipher_pwd = IVS_NEW(cipher_pwd,128);
	if (NULL == cipher_pwd)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS OCX Aes256Cbc Encode Failed.", "Reason:new memory failed.");
		return strResult.AllocSysString();
	}
	memset(cipher_pwd,0,128);

    IVS_INT32 iRet = IVS_SDK_Aes256CbcEncode(pszPlain,cipher_pwd, (IVS_UINT64)128);
	// 防止密码在内存中显示
	memset((char*)pszPlain, 0, strlen(pszPlain));
    if (IVS_SUCCEED != iRet)
    {
        IVS_DELETE(cipher_pwd, MULTI);
        return NULL;
    }

    //返回密文
    strResult = cipher_pwd;
    IVS_DELETE(cipher_pwd, MULTI);

    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_Aes256CbcDecode(LPCTSTR pszCipher)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(pszCipher, NULL);
	IVS_INFO_TRACE("OCX Address: %x", this);
    CString strResult;

    //分配存明文空间
    char* pszPlain_pwd = IVS_NEW(pszPlain_pwd, 128);
	if (NULL == pszPlain_pwd)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS OCX Aes256Cbc Decode Failed.", "Reason:new memory failed.");
		return strResult.AllocSysString();
	}
	memset(pszPlain_pwd,0,128);
    //解密
    IVS_INT32 iRet = IVS_SDK_Aes256CbcDecode(pszCipher,pszPlain_pwd, (IVS_UINT64)128);
	// 防止密码在内存中显示
	memset((char*)pszCipher, 0, strlen(pszCipher));
    if (IVS_SUCCEED != iRet)
    {
        IVS_DELETE(pszPlain_pwd, MULTI);
		memset(pszPlain_pwd,0,128);
        return NULL;
    }

    //返回明文
    strResult = pszPlain_pwd;
	memset(pszPlain_pwd,0,128);
    IVS_DELETE(pszPlain_pwd, MULTI);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAlarmLevel(LONG uiAlarmLevelID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,AlarmLevelID: %ld", this,uiAlarmLevelID);

	CString strResult;
	COCXAlarmMgr::GetAlarmLevel(m_iSDKSessionID, uiAlarmLevelID, strResult);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_QueryHelpInfo(LPCTSTR helpInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == helpInfo)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == helpInfo", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();

	}
	IVS_INFO_TRACE("OCX Address: %x,HelpInfo: %s", this,helpInfo);

	
	COCXAlarmMgr::QueryHelpInfo(m_iSDKSessionID, helpInfo, strResult);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetMediaInfo(ULONG ulWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,ulWndID: %lu", this,ulWndID);

	CString strResult;

	IVS_INT32 iRet = IVS_FAIL;

	IVS_MEDIA_INFO strMediaInfo;
	CPoint pointXY ;
	memset(&strMediaInfo,0,sizeof(IVS_MEDIA_INFO));
	do
	{
		if (NULL == m_pVideoPaneMgr)
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Get Media Info Failed.", "Reason:m_pVideoPaneMgr is NULL.");
			break;
		}

		CVideoPane* pWnd = m_pVideoPaneMgr->GetWndPointerByID(ulWndID);

		if (NULL == pWnd)
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get Media Info Failed.","Reason:Get CVideoPane Error");
			break;
		}
		//先获取展示的位置，保证位置正确
		pointXY =  pWnd->GetInfoButtonRightBottomPosition();

		unsigned long ulHandle = pWnd->GetPlayHandle();

		iRet = IVS_SDK_GetMediaInfo(m_iSDKSessionID,ulHandle, &strMediaInfo);

		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "IVS OCX GetMediaInfo","Reason:Get media info from sdk Failed.");
			break;
		}
		if (pWnd->GetPlayState()==VIDEO_PLAY_STATE_NOT_PLAYING)
		{
			iRet = IVS_FAIL;
			break;
		}
		
	} while (false); //lint !e717

	COCXPlayBackXMLProcess::GetPlayInfoEventMsgXML(strResult,ulWndID,strMediaInfo,pointXY,iRet);

	return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetAlarmTypeLevel(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXAlarmMgr::SetAlarmTypeLevel(m_iSDKSessionID, (IVS_CHAR*)pReqXml);
	return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetDisplayScale(LONG lDisplayScale)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,DisplayScale: %ld", this,lDisplayScale);
	if (m_pVideoPaneMgr)
	{
		(void)m_pVideoPaneMgr->SetDisplayScale(m_iSDKSessionID, (ULONG)lDisplayScale);
	}
	return IVS_SUCCEED;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddFavoriteDevice(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, NULL);
    IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
    CString strResult;
    std::string strDeviceGUID;
    CXml xmlRsp;
    IVS_INT32 iRet = IVS_FAIL;
    iRet = CFavorite::AddFavoriteDevice(pReqXml, strDeviceGUID);
    if (IVS_SUCCEED != iRet)
    {
        COCXXmlProcess::GetErrString(strResult,iRet);
        return strResult.AllocSysString();
    }
    CFavorite::AddFavoriteDeviceRspXML(strDeviceGUID, xmlRsp);

    unsigned int uiXmlLen = 0;
    strResult = xmlRsp.GetXMLStream(uiXmlLen);
    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_DelFavoriteDevice(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

    return CFavorite::DelFavoriteDevice(pReqXml);
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_MoveFavoriteDevice(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

    return CFavorite::MoveFavoriteDevice(pReqXml);
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddFavoriteFolder(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, NULL);
    IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
    CString strResult;
    std::string strFolderGUID;
    CXml xmlRsp;
    IVS_INT32 iRet = IVS_FAIL;
    iRet = CFavorite::AddFavoriteFolder(pReqXml, strFolderGUID);
    if (IVS_SUCCEED != iRet)
    {
        COCXXmlProcess::GetErrString(strResult,iRet);
        return strResult.AllocSysString();
    }
    CFavorite::AddFavoriteFolderRspXML(strFolderGUID, xmlRsp);

    unsigned int uiXmlLen = 0;
    strResult = xmlRsp.GetXMLStream(uiXmlLen);
    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_DelFavoriteFolder(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
    return CFavorite::DelFavoriteFolder(pReqXml);
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModFavoriteFolder(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

    return CFavorite::ModFavoriteFolder(pReqXml);
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_MoveFavoriteFolder(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

    return CFavorite::MoveFavoriteFolder(pReqXml);
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetFavoriteDevice(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
    CXml xmlRsp;
    CString strResult;
    IVS_INT32 iRet = CFavorite::GetFavoriteDevice(xmlRsp);
    if (IVS_SUCCEED != iRet)
    {
        COCXXmlProcess::GetErrString(strResult, iRet);
        BP_RUN_LOG_ERR(iRet,"IVS OCX Get Favorite Device Failed.", "Reason:Get Favorite Device Xml Failed.");
    }
    else
    {
        IVS_UINT32 uiXmlLen = 0;
        strResult = xmlRsp.GetXMLStream(uiXmlLen);
    }

    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SubscribeAlarm(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	IVS_INT32 iRet = COCXAlarmMgr::SubscribeAlarm(m_iSDKSessionID, (IVS_CHAR*)pReqXml);
	return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_SubscribeAlarmQuery(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Subscribe Alarm Query Failed.", "Reason:pReqXml is NULL.");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	COCXAlarmMgr::SubscribeAlarmQuery(m_iSDKSessionID, (IVS_CHAR*)pReqXml, strResult);
	return strResult.AllocSysString();
}

// 指定防区布防计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AllocAreaGuardPlan(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// 判断入参
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Alloc Area Guard Plan Failed.", "Reason:pReqXml is NULL.");
		return IVS_XML_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	IVS_INT32 iRet = COCXAlarmMgr::AllocAreaGuardPlan(m_iSDKSessionID,pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Alloc Area Guard Plan Failed.", "OCX AlarmMgr Alloc Area Guard Plan Failed.");
		return iRet;
	}
	return IVS_SUCCEED;
}

//查询域路由信息
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDomainRoute(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
    CString strResult;

    COCXDeviceMgr::GetDomainRoute(m_iSDKSessionID, strResult);
    return strResult.AllocSysString();
}

// 修改防区
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyAlarmArea(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	IVS_INT32 iRet = COCXAlarmMgr::ModifyAlarmArea(m_iSDKSessionID, (IVS_CHAR*)pReqXml);
	return iRet;
}

// 删除防区
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteAlarmArea(LPCTSTR pDomainCode,ULONG ulAlarmAreaID)
{
	if (NULL == pDomainCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == pDomainCode", "NA");
		return IVS_XML_INVALID;
	}
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,pDomainCode: %s, AlarmAreaId: %lu", this,pDomainCode, ulAlarmAreaID);

	IVS_INT32 iRet = COCXAlarmMgr::DeleteAlarmArea(m_iSDKSessionID, pDomainCode, ulAlarmAreaID);
	return iRet;
}

// 查询防区列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAlarmAreaList(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Alarm Area List Failed.","Reason:pReqXml is NULL.");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
	COCXAlarmMgr::GetAlarmAreaList(m_iSDKSessionID, (IVS_CHAR*)pReqXml, strResult);
	return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_UserAuthentication(LPCTSTR pPWD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);

	IVS_INT32 iRet = COCXAlarmMgr::UserAuthentication(m_iSDKSessionID, (IVS_CHAR*)pPWD);
	if (NULL != pPWD)
	{
		// 清理内存，保证内存中不会有密码的存在
		memset((char*)pPWD,0,strlen(pPWD));
	}

	return iRet;
}
// 修改防区布防计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyAreaGuardPlan(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// 判断入参
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Modify Area Guard Plan Failed.", "Reason:pReqXml is NULL.");
		return IVS_XML_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	// 调用SDK
	IVS_INT32 iRet = COCXAlarmMgr::ModifyAreaGuardPlan(m_iSDKSessionID,pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Modify Area Guard Plan Failed.", "Reason:OCX AlarmMgr Modify Area Guard Plan Failed.");
		return iRet;
	}
	return IVS_SUCCEED;
}

// 删除防区布防计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteAreaGuardPlan(LPCTSTR pDomainCode,ULONG ulAlarmAreaId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// 判断入参
	if (NULL == pDomainCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Delete Area Guard Plan Failed.", "Reason:pReqXml is NULL.");
		return IVS_XML_INVALID;
	}	
	
	IVS_INFO_TRACE("OCX Address: %x,pDomainCode: %s, AlarmAreaId: %lu", this,pDomainCode, ulAlarmAreaId);

	IVS_INT32 iRet = COCXAlarmMgr::DeleteAreaGuardPlan(m_iSDKSessionID, pDomainCode, ulAlarmAreaId);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX DeleteAreaGuardPlan Failed.", "Reason:IVS_SDK_DeleteAreaGuardPlan Failed.");
		return iRet;
	}
	return IVS_SUCCEED;
}

// 获取防区布防计划
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAreaGuardPlan(LPCTSTR pDomainCode,ULONG ulAlarmAreaId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CString strResult;
	if (NULL == pDomainCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == pDomainCode", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,pDomainCode: %s, AlarmAreaId: %lu", this,pDomainCode, ulAlarmAreaId);
    IVS_INT32 iRet = COCXAlarmMgr::GetAreaGuardPlan(m_iSDKSessionID, pDomainCode, ulAlarmAreaId, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Area Guard Plan Failed.", "Reason:OCX AlarmMgr Get Area Guard Plan Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 增加防区
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddAlarmArea(LPCTSTR pReqXml)
{
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	// 判断入参
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Add Alarm Area Failed.", "Reason:pReqXml is NULL.");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXAlarmMgr::AddAlarmArea(m_iSDKSessionID,pReqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Add Alarm Area Failed.", "Reason:OCX AlarmMgr Add Alarm Area Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 获取防区详细信息
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAlarmAreaInfo(LPCTSTR pDomainCode,ULONG ulAlarmAreaId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;
	if (NULL == pDomainCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == pDomainCode", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,pDomainCode: %s, AlarmAreaId: %lu", this,pDomainCode, ulAlarmAreaId);
	IVS_INT32 iRet = COCXAlarmMgr::GetAlarmAreaInfo(m_iSDKSessionID, pDomainCode, ulAlarmAreaId, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Alarm Area Info Failed.", "IVS SDK Add Alarm Area Failed.");
        COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 增加告警联动
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddAlarmLinkage(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Add Alarm Linkage Failed.", "Reason:pReqXml is NULL.");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	IVS_INT32 iRet = COCXAlarmLinkageMgr::AddAlarmLinkage(m_iSDKSessionID, pReqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Add Alarm Linkage Failed.", "Reason:OCX AlarmLinkageMgr Add Alarm Linkage Failed.");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 修改告警联动策略
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyAlarmLinkage(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Modify Alarm Linkage Failed.", "Reason:pReqXml is NULL.");
		return IVS_XML_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXAlarmLinkageMgr::ModifyAlarmLinkage(m_iSDKSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Modify AlarmLinkage Failed.", "Reason:OCX AlarmLinkageMgr Modify Alarm LinkageFailed.");
		return iRet;
	}

	return 0;
}

// 删除告警联动策略
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteAlarmLinkage(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);

	IVS_INT32 iRet = COCXAlarmLinkageMgr::DeleteAlarmLinkage(m_iSDKSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"IVS OCX Delete Alarm Linkage Failed.", "Reason:OCX AlarmLinkageMgr Delete AlarmLinkage Failed.");
		return iRet;
	}
	return iRet;
}
// 查询告警联动策略详情
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAlarmLinkage(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == pReqXml", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);

	COCXAlarmLinkageMgr::GetAlarmLinkage(m_iSDKSessionID, pReqXml, strResult);
	return strResult.AllocSysString();
}
// 查询告警联动策略列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAlarmLinkageList(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Alarm Linkage List Failed.","Reason:pReqXml is NULL.");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	COCXAlarmLinkageMgr::GetAlarmLinkageList(m_iSDKSessionID, (IVS_CHAR*)pReqXml, strResult);
	return strResult.AllocSysString();
}

// 智能分析统一透传接口
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_IA(ULONG ulInterfaceType, LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	***********pReqXml可以为空*********************
//  CHECK_POINTER(pReqXml, NULL);
	IVS_INFO_TRACE("OCX Address: %x,InterfaceType: %lu,ReqXml: %s", this, ulInterfaceType, NULL != pReqXml ? pReqXml : "NULL");

	CString strResult;
	COcxIntelligenceAnalysisMgr::IntelligenceAnalysis(strResult, m_iSDKSessionID, ulInterfaceType, (IVS_CHAR*)pReqXml);
	return strResult.AllocSysString();
}


// 开始/停止轨迹叠加
LONG CIVS_OCXPlayerCtrl::IVS_OCX_IALocusOverly(ULONG ulWinID, ULONG ulRuleType, ULONG ulStartOverlay)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x, WinID: %lu, RuleType: %lu, StartOverly: %lu", this, ulWinID, ulRuleType, ulStartOverlay);
	
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX IA Locus Overly Failed", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWinID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX IA Locus Overly Failed", "Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	IVS_INT32 iRet = IVS_FAIL;
	if (ulStartOverlay)
	{
		iRet = pVideoPane->EnableOverlay(m_iSDKSessionID, pVideoPane->GetPlayHandle(),ulRuleType);
	} 
	else 
	{
		iRet = pVideoPane->DisableOverlay(m_iSDKSessionID, pVideoPane->GetPlayHandle());
	}
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX IA Locus Overly Failed", "Reason:VideoPane Enable / Disable Overlay Failed.");
	}
	return iRet;
}

// 查询告警历史列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAlarmEventList(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Alarm Event List Failed.", "Reason:pReqXml is NULL.");
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	COCXAlarmMgr::GetAlarmList(m_iSDKSessionID, (IVS_CHAR*)pReqXml, strResult);
	return strResult.AllocSysString();
}

void CIVS_OCXPlayerCtrl::IVS_OCX_SetPlayQualityMode(ULONG ulWndID, LONG lpalyQualityMode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %lu,palyQualityMode: %ld", this,ulWndID,lpalyQualityMode);
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Play Quality Mode Failed." ,"Reason:VideoPaneMgr is NULL.");
		return ;
	}

	CVideoPane* pWnd = m_pVideoPaneMgr->GetWndPointerByID(ulWndID);

	if (NULL == pWnd)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Play Quality Mode Failed.","Reason:Get CVideoPane Error.");
		return ;
	}
	unsigned long ulHandle = pWnd->GetPlayHandle();
	//设置视频播放模式
	int iRet = IVS_SDK_SetPlayQualityMode(m_iSDKSessionID,ulHandle,(int)lpalyQualityMode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Play Quality Mode Failed.","Reason:SDK Set Play Mode Error.");
	}
	return;
}

// 手动布防
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartAlarmAreaGuard(LPCTSTR pDomainCode,ULONG ulAlarmAreaId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NULL == pDomainCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Delete Area Guard Plan Failed.", "Reason:pReqXml is NULL.");
		return IVS_XML_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,AlarmAreaId: %lu", this,ulAlarmAreaId);

	IVS_INT32 iRet = COCXAlarmMgr::StartAlarmAreaGuard(m_iSDKSessionID, pDomainCode, ulAlarmAreaId);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Start Alarm Area Guard Failed.", "Reason:OCX AlarmMgr Start Alarm Area Guard Failed.");
		return iRet;
	}
	return iRet;
}

// 手动撤防
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopAlarmAreaGuard(LPCTSTR pDomainCode,ULONG ulAlarmAreaId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NULL == pDomainCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Delete Area Guard Plan Failed.", "Reason:pReqXml is NULL.");
		return IVS_XML_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,AlarmAreaId: %lu", this,ulAlarmAreaId);

	IVS_INT32 iRet = COCXAlarmMgr::StopAlarmAreaGuard(m_iSDKSessionID, pDomainCode, ulAlarmAreaId);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Stop Alarm Area Guard Failed.", "Reason:OCX AlarmMgr Stop Alarm Area Guard Failed.");
		return iRet;
	}
	return iRet;
}
// 查询历史详情
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAlarmEventInfo(LONG lAlarmLevelID, LPCTSTR pAlarmInCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,AlarmLevelID: %ld", this,lAlarmLevelID);
    
	CString strResult;
    if (NULL == pAlarmInCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Alarm Event Info Failed.", "Reason:pAlarmInCode is NULL.");
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	COCXAlarmMgr::GetAlarmInfo(m_iSDKSessionID, static_cast<IVS_UINT64>(lAlarmLevelID), pAlarmInCode, strResult); //lint !e571
	return strResult.AllocSysString();
}

// 告警授权
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AlarmCommission(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Alarm Commission Failed.", "Reason:pReqXml is NULL.");
		return IVS_XML_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	IVS_INT32 iRet = COCXAlarmMgr::AlarmCommission(m_iSDKSessionID, (IVS_CHAR*)pReqXml);

	// 清理内存，保证内存中不会有密码的存在
	memset((char*)pReqXml,0,strlen(pReqXml));

	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Alarm Commission Failed.", "Reason:OCX AlarmMgr Alarm Commission Failed.");
		return iRet;
	}
	return iRet;
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_SetActiveWnd(ULONG ulWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,WndID: %lu", this,ulWndID);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);

	// 当前布局不够放窗格时，布局升级先获取窗格个数
	ULONG ulLayout = m_pVideoPaneMgr->GetNumOfDisplayPane();

	if (MIN_PANE_NUMBER > ulWndID || ulLayout <ulWndID)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set ActiveWnd Failed.", "Reason:window id invalid.");
		return IVS_PARA_INVALID;
	}

	m_pVideoPaneMgr->SetActivatePane(ulWndID);
	return IVS_SUCCEED;
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_GetSelectWnd(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
    CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);

    ULONG ulWndID = m_pVideoPaneMgr->GetActivePaneID();

    // 窗格ID范围是1到64
    if ((MIN_PANE_NUMBER > ulWndID) || (MAX_PANE_NUMBER < ulWndID))
    {
        BP_RUN_LOG_ERR(IVS_OCX_RET_GET_INFO_ERROR, "IVS OCX Set Active Wnd Failed.","Reason:winid is invalid`.");
        return IVS_OCX_RET_GET_INFO_ERROR;
    }

    BP_RUN_LOG_INF( "IVS OCX Get SelectWnd End", "WndID: %d", ulWndID);
    return ulWndID;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCameraByWnd(ULONG ulWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,WndID: %lu", this,ulWndID);
    CString strResult;
    char result[32]={0};

    if (ulWndID < MIN_PANE_NUMBER || ulWndID > MAX_PANE_NUMBER)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get Camera By Wnd Failed.", "Reason:win id is invalid");
        (void)itoa(IVS_PARA_INVALID,result,10);
        strResult = result;
        return strResult.AllocSysString();
    }

    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Camera By Wnd Failed.", "Reason:video pane manager is null.");
        (void)itoa(IVS_OPERATE_MEMORY_ERROR,result,10);
        strResult = result;
        return strResult.AllocSysString();
    }

    CVideoPane * pWnd = nullptr;
    pWnd = m_pVideoPaneMgr->GetWndPointerByID(ulWndID);
    if (NULL == pWnd)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Camera By Wnd Failed.", "Reason:OCX get window pointer error.");
        (void)itoa(IVS_OPERATE_MEMORY_ERROR,result,10);
        strResult = result;
        return strResult.AllocSysString();
    }

    strResult = pWnd->GetCameraID();

    if (strResult.IsEmpty())
    {
        (void)itoa(IVS_FAIL,result,10);
        strResult = result;
        return strResult.AllocSysString();
    }

    return strResult.AllocSysString();
}

//根据主设备查询子设备列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetChannelList(LPCTSTR pDevCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString strResult;
    if (NULL == pDevCode)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get Channel List Failed.", "Reason:pDevCode is NULL.");
        COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
        return strResult.AllocSysString();
    }
	IVS_INFO_TRACE("OCX Address: %x,DevCode: %s", this,pDevCode);

	COCXDeviceMgr::GetChannelList(m_iSDKSessionID, (IVS_CHAR*)pDevCode, strResult);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_PtzControl(LPCTSTR pCameraCode,LONG lOperateCode, LPCTSTR pControlPara1, LPCTSTR pControlPara2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode, NULL);
	CHECK_POINTER(pControlPara1, NULL);
	CHECK_POINTER(pControlPara2, NULL);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,OperateCode: %ld,ControlPara1: %s,ControlPara2: %s",
		           this,pCameraCode,lOperateCode,pControlPara1,pControlPara2);
    // 为水平扫描或垂直扫描，停止窗格云镜控制
    if ((PTZ_VERTICAL_SCAN == lOperateCode || PTZ_HORIZONTAL_SCAN == lOperateCode) && NULL != m_pVideoPaneMgr)
    {
        std::string strCameraCode = pCameraCode;
        CVideoPane* pVideoPane = m_pVideoPaneMgr->GetWndPointerByCameraCode(strCameraCode);
        if (NULL != pVideoPane)
        {
            if (BOTTOMBAR_PTZ_OPEN == pVideoPane->GetRealtimePtzState())
            {
                // 删除云镜控制
                pVideoPane->DeleteDigtalPtz();
                pVideoPane->SetRealtimePtzState(BOTTOMBAR_PTZ_CLOSE);
                pVideoPane->GetVideoRealtimeBar()->SetPtzBtnState(false);
                pVideoPane->SetCursorState(CURSOR_STATE_CLOSE);
            }
        }
    }

	IVS_INT32 iRet = IVS_FAIL;
	IVS_UINT32 uiLockStatus = 0;
	iRet = IVS_SDK_PtzControl(m_iSDKSessionID,pCameraCode,lOperateCode,pControlPara1,pControlPara2, &uiLockStatus);

	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
	(void)xml.AddElem("LockStatus");
	(void)xml.IntoElem();
	(void)xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)uiLockStatus).c_str());

	unsigned int len = 0;
	const char* pXml = xml.GetXMLStream(len);
	
	return _com_util::ConvertStringToBSTR(pXml);
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRecordStatus(LPCTSTR pCameraCode,ULONG ulRecordMethod)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode, NULL);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,RecordMethod: %lu", this,pCameraCode,ulRecordMethod);
	CString strResult;
	strResult = OCXRecordMgr::GetRecordStatus(pCameraCode,ulRecordMethod,m_iSDKSessionID);
	return strResult.AllocSysString();
}

// 告警确认
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AlarmConfirm(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
	IVS_INT32 iType = 1;
	IVS_INT32 iRet = COCXAlarmMgr::AlarmCancelORConfirm(m_iSDKSessionID, (IVS_CHAR*)pReqXml, iType);
	return iRet;
}

// 告警撤销
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AlarmCancel(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
	IVS_INT32 iType = 0;
	IVS_INT32 iRet = COCXAlarmMgr::AlarmCancelORConfirm(m_iSDKSessionID, (IVS_CHAR*)pReqXml, iType);
	return iRet;
}

// 告警信息上报
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AlarmReport(LPCTSTR pAlarmReport)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NULL == pAlarmReport)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Alarm Report Failed.", "Reason:pAlarmReport is NULL.");
		return IVS_XML_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,AlarmReport: %s", this,pAlarmReport);

	IVS_INT32 iRet = COCXAlarmMgr::AlarmReport(m_iSDKSessionID, (IVS_CHAR*)(pAlarmReport));
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Alarm Report Failed.", "Reason:OCX AlarmMgr Alarm Report Failed.");
		return iRet;
	}
	return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetUserID(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    IVS_INFO_TRACE("OCX Address: %x", this);
    IVS_INT32 iRet = IVS_FAIL;

    CString strResult;
    CXml xmlRsp;
    IVS_UINT32 uiUserID = 0;
    IVS_UINT32 uiXmlLen = 0;

    iRet = IVS_SDK_GetUserID(m_iSDKSessionID, &uiUserID);
    if (IVS_SUCCEED != iRet)
    {
        COCXXmlProcess::GetErrString(strResult,iRet);
        return strResult.AllocSysString();
    }
    COCXLoginXMLProcess::GetUserIDRspXML(iRet,uiUserID, xmlRsp);

    strResult = xmlRsp.GetXMLStream(uiXmlLen);

    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_BackupUserData(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);

    // TODO: Add your dispatch handler code here
//     std::string strPath;
//     IVS_CHAR cPath[MAX_PATH +1] = {0};
//     GetCurrentDirectory(MAX_PATH,cPath);
//     strPath = cPath;
//     strPath += "\\";
//     (void)CLocalConfig::CompressionDir("userDataxml.zip", "C:\\userDataxml", strPath);
//     /*fopen("C:\\userDataxml\\userDataxml.zip","rb");*/
//
//     //打开文件
//     std::ifstream rfile("file.txt",std::ios::in);
//     if(!rfile)
//     {
//         std::cout<<"不可以打开文件"<<std::endl;
// 	exit(1);
//     }
//     //读文件
//     char str[100];
//     rfile.getline(str,100);//读到'\n'终止
//     std::cout<<str<<std::endl;
//     char rch;
//     while(rfile.get(rch))//文件指针指向字符‘\n’的下一个
//     {
//         std::cout.put(rch);
//     }
//     std::cout<<std::endl;
//     //关闭文件
//     rfile.close();
    return 0;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_RestoreUserData(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);

    // TODO: Add your dispatch handler code here
//     std::string strPath;
//     IVS_CHAR cPath[MAX_PATH +1] = {0};
//     GetCurrentDirectory(MAX_PATH,cPath);
//     strPath = cPath;
//     (void)CLocalConfig::DecompressionDir("userDataxml.zip",strPath, "C:\\userDataxml");
    return 0;
}

LONG CIVS_OCXPlayerCtrl::StartSound(ULONG ulWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %lu", this,ulWndID);
    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Start Sound Failed.","Reason:m_pVideoPaneMgr is null.");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWndID);
    CHECK_POINTER(pVideoPane,IVS_OPERATE_MEMORY_ERROR);

    ULONG ulHandle = pVideoPane->GetPlayHandle();
    if (0 == ulHandle)
    {
        BP_RUN_LOG_ERR(IVS_OCX_RET_NO_PLAY_HANDLE, "IVS OCX Start Sound Failed.","Reason:video pane's play handle = 0.");
        return IVS_OCX_RET_NO_PLAY_HANDLE;
    }
	LONG lRet = IVS_SDK_PlaySound(m_iSDKSessionID, ulHandle);
// 	CGlobalSoundMgr &SoundMgr = CGlobalSoundMgr::Instance();
// 	SoundMgr.SetSoundingPanel(pVideoPane);

    // modify by wanglei 00165153:2013.6.28 DTS2013062702103
    // 需要上报随路语音当前状态，否则录像回放跳段会有问题
    if (IVS_SUCCEED == lRet)
    {
        CString xmlToCU;
        xmlToCU ="";
        PostEventXMLImpl.GetVolumeEventMsg(xmlToCU, pVideoPane->GetPaneID(), 1, 0);
        IVS_OCX_Windows_Event(EVENT_SOUND_STATUS, (LPCTSTR)xmlToCU);
    }

    return lRet;
}

LONG CIVS_OCXPlayerCtrl::StopSound(ULONG ulWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %lu", this,ulWndID);
    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Sound Failed","Reason:video pane manager is null.");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWndID);
    CHECK_POINTER(pVideoPane,IVS_OPERATE_MEMORY_ERROR);

    ULONG ulHandle = pVideoPane->GetPlayHandle();
    if (0 == ulHandle)
    {
        BP_RUN_LOG_ERR(IVS_OCX_RET_NO_PLAY_HANDLE, "IVS OCX Stop Sound Failed.","Reason:video pane's play handle = 0.");
        return IVS_OCX_RET_NO_PLAY_HANDLE;
    }
	LONG lRet = IVS_SDK_StopSound(m_iSDKSessionID, ulHandle);
// 	CGlobalSoundMgr &SoundMgr = CGlobalSoundMgr::Instance();
// 	SoundMgr.SetSoundingPanel(NULL);

// 	CString xmlToCU;
// 	xmlToCU ="";
// 	PostEventXMLImpl.GetVolumeEventMsg(xmlToCU, ulWndID, 0, 0);
// 	IVS_OCX_Windows_Event(EVENT_SOUND_STATUS, (LPCTSTR)xmlToCU);

    // modify by wanglei 00165153:2013.6.28 DTS2013062702103
    // 需要上报随路语音当前状态，否则录像回放跳段会有问题
    if (IVS_SUCCEED == lRet)
    {
        CString xmlToCU;
        xmlToCU ="";
        PostEventXMLImpl.GetVolumeEventMsg(xmlToCU, pVideoPane->GetPaneID(), 0, 0);
        IVS_OCX_Windows_Event(EVENT_SOUND_STATUS, (LPCTSTR)xmlToCU);
    }


    return lRet;
}

LONG CIVS_OCXPlayerCtrl::SetVolume(ULONG ulWndID, ULONG ulType, unsigned int uiVolumeValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %lu,VolumeValue: %u", this,ulWndID,uiVolumeValue);
    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set Volume Failed.","Reason:m_pVideoPaneMgr is null.");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWndID);
    CHECK_POINTER(pVideoPane,IVS_OPERATE_MEMORY_ERROR);
	ULONG ulHandle = 0;
	if (SOUNDCTRL_WND_TYPE_SOUND == ulType)
	{
		ulHandle = pVideoPane->GetPlayHandle();
		if (0 == ulHandle)
		{
			BP_RUN_LOG_ERR(IVS_OCX_RET_NO_PLAY_HANDLE, "IVS OCX Start Sound.","Reason:video pane's play handle = 0.");
			return IVS_OCX_RET_NO_PLAY_HANDLE;
		}
	}
	else if (SOUNDCTRL_WND_TYPE_TALKBACK == ulType)
	{
		ulHandle = m_ulVoiceTalkbackHandle;
		if (0 == ulHandle)
		{
			BP_RUN_LOG_ERR(IVS_OCX_RET_NO_PLAY_HANDLE, "IVS OCX Start Sound.","Reason:video pane's play handle = 0.");
			return IVS_OCX_RET_NO_PLAY_HANDLE;
		}
	}
// 	else
// 	{
// 		//return IVS_OCX_RET_NO_PLAY_HANDLE;
// 	}

	
// 	CString xmlToCU;
// 	xmlToCU ="";
// 	PostEventXMLImpl.GetVolumeEventMsg(xmlToCU, ulWndID, 2, (ULONG)uiVolumeValue);
// 	IVS_OCX_Windows_Event(EVENT_SOUND_STATUS, (LPCTSTR)xmlToCU);
	int iRet = IVS_SDK_SetVolume(m_iSDKSessionID,
		ulHandle,
		uiVolumeValue);
	return iRet;
}

LONG CIVS_OCXPlayerCtrl::GetVolume(ULONG ulWndID, ULONG ulType, unsigned int* puiVolumeValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %lu", this,ulWndID);
    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Volume Failed.","Reason:video pane manager is null.");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWndID);
    CHECK_POINTER(pVideoPane,IVS_OPERATE_MEMORY_ERROR);

	ULONG ulHandle = 0;
	if (SOUNDCTRL_WND_TYPE_SOUND == ulType)
	{
		ulHandle = pVideoPane->GetPlayHandle();
		if (0 == ulHandle)
		{
			BP_RUN_LOG_ERR(IVS_OCX_RET_NO_PLAY_HANDLE, "IVS OCX Start Sound.","Reason:video pane's play handle = 0.");
			return IVS_OCX_RET_NO_PLAY_HANDLE;
		}
	}
	else if (SOUNDCTRL_WND_TYPE_TALKBACK == ulType)
	{
		ulHandle = m_ulVoiceTalkbackHandle;
		if (0 == ulHandle)
		{
			BP_RUN_LOG_ERR(IVS_OCX_RET_NO_PLAY_HANDLE, "IVS OCX Start Sound.","Reason:video pane's play handle = 0.");
			return IVS_OCX_RET_NO_PLAY_HANDLE;
		}
	}
	else
	{
		return IVS_OCX_RET_NO_PLAY_HANDLE;
	}

    return IVS_SDK_GetVolume(m_iSDKSessionID,
                             ulHandle,
                             puiVolumeValue);
}

void CIVS_OCXPlayerCtrl::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
    COleControl::OnWindowPosChanged(lpwndpos);

    //CHECK_POINTER_VOID(m_pVideoPaneMgr); del by w00210470
    if (NULL == m_pVideoPaneMgr)
    {
        return;
    }
    
    if((lpwndpos->flags & (SWP_NOREDRAW | SWP_NOMOVE| SWP_NOMOVE| SWP_NOSIZE) )
        == (SWP_NOREDRAW | SWP_NOMOVE| SWP_NOMOVE| SWP_NOSIZE) )
    {
       m_pVideoPaneMgr->HideAllBarOnce();
    }
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_InitTVWall(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address: %x", this);
	IVS_INT32 iRet = COCXTVWallMgr::InitTVWall(m_iSDKSessionID, HandleTVWallNotifyCallBack, this);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Init TVWall Failed.", "Reason: OCX TVWallMgr Init TVWall Failed.");
	}

    m_pTvwallOcx = this;

	return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_CleanupTVWall(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address: %x", this);
	//判断是否要停止同步回放上墙
	CTVWallSyncPlayBackMgr &TVWallSyncMgr = CTVWallSyncPlayBackMgr::Instance();
	if(TVWallSyncMgr.m_bStart)
	{
		(void)TVWallSyncMgr.StopTVwallSyncPlayBack();
	}	
    CleanupTVWall();
	IVS_INT32 iRet =COCXTVWallMgr::ReleaseTVWall(m_iSDKSessionID);

    m_pTvwallOcx = NULL;

	return iRet;
}

typedef struct
{
    CUMW_NOTIFY_INFO* pstNotifyInfo;
    void* pUserData;
} ST_TV_WALL_CALLBACK_INFO;

IVS_INT32 __stdcall CIVS_OCXPlayerCtrl::HandleTVWallNotifyCallBack(CUMW_NOTIFY_INFO* pstNotifyInfo, void* pUserData)
{
    CHECK_POINTER(pstNotifyInfo, IVS_FAIL);
    CHECK_POINTER(pUserData, IVS_FAIL);
    IVS_INFO_TRACE("NotifyType=0x%04X, OCX Address=0x%p", pstNotifyInfo->ulNotifyType, pUserData);

    ST_TV_WALL_CALLBACK_INFO* pstCallback = IVS_NEW(pstCallback);
    CHECK_POINTER(pstCallback, IVS_ALLOC_MEMORY_ERROR);
    memset(pstCallback, 0, sizeof(ST_TV_WALL_CALLBACK_INFO));

    CUMW_NOTIFY_INFO* pstNotify = IVS_NEW(pstNotify);
    if (NULL == pstNotify)
    {
        IVS_DELETE(pstCallback);
        BP_RUN_LOG_ERR(IVS_FAIL, "Pointer is NULL!", "");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    memset(pstNotify, 0, sizeof(CUMW_NOTIFY_INFO));
    memcpy(pstNotify, pstNotifyInfo, sizeof(CUMW_NOTIFY_INFO));


    switch (pstNotifyInfo->ulNotifyType)
    {
    case CUMW_NOTIFY_TYPE_ASYNC_RET:
        {
            CUMW_NOTIFY_ASYNC_RET_EX* pstContent = IVS_NEW(pstContent);
            if (NULL == pstContent)
            {
                IVS_DELETE(pstNotify);
                IVS_DELETE(pstCallback);
                BP_RUN_LOG_ERR(IVS_FAIL, "Pointer is NULL!", "");
                return IVS_ALLOC_MEMORY_ERROR;
            }
            memset(pstContent, 0, sizeof(CUMW_NOTIFY_ASYNC_RET_EX));

            memcpy(pstContent, pstNotifyInfo->pNotifyInfo, sizeof(CUMW_NOTIFY_ASYNC_RET_EX));
            pstNotify->pNotifyInfo = (void*)pstContent;
        }
        break;
    case CUMW_NOTIFY_TYPE_REMOTE_DECODER_REGISTER:
        {
            CUMW_NOTIFY_REMOTE_DECODER_STATUS* pstContent = IVS_NEW(pstContent);
            if (NULL == pstContent)
            {
                IVS_DELETE(pstNotify);
                IVS_DELETE(pstCallback);
                BP_RUN_LOG_ERR(IVS_FAIL, "Pointer is NULL!", "");
                return IVS_ALLOC_MEMORY_ERROR;
            }
            memset(pstContent, 0, sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS));

            memcpy(pstContent, pstNotifyInfo->pNotifyInfo, sizeof(CUMW_NOTIFY_REMOTE_DECODER_STATUS));
            pstNotify->pNotifyInfo = (void*)pstContent;
        }
        break;
    case CUMW_NOTIFY_TYPE_REMOTE_DECODER_UNRIGISTER:
    case CUMW_NOTIFY_TYPE_REMOTE_DECODER_DISCONNECT:
        {
            CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO* pstContent = IVS_NEW(pstContent);
            if (NULL == pstContent)
            {
                IVS_DELETE(pstNotify);
                IVS_DELETE(pstCallback);
                BP_RUN_LOG_ERR(IVS_FAIL, "Pointer is NULL!", "");
                return IVS_ALLOC_MEMORY_ERROR;
            }
            memset(pstContent, 0, sizeof(CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO));

            memcpy(pstContent, pstNotifyInfo->pNotifyInfo, sizeof(CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO));
            pstNotify->pNotifyInfo = (void*)pstContent;
        }
        break;
    case CUMW_NOTIFY_TYPE_RTSP_EXCEPTION:
    case CUMW_NOTIFY_TYPE_DECODER_ERROR:	//断流
        {
            CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO* pstContent = IVS_NEW(pstContent);
            if (NULL == pstContent)
            {
                IVS_DELETE(pstNotify);
                IVS_DELETE(pstCallback);
                BP_RUN_LOG_ERR(IVS_FAIL, "Pointer is NULL!", "");
                return IVS_ALLOC_MEMORY_ERROR;
            }
            memset(pstContent, 0, sizeof(CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO));

            memcpy(pstContent, pstNotifyInfo->pNotifyInfo, sizeof(CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO));
            pstNotify->pNotifyInfo = (void*)pstContent;
        }
        break;
    case CUMW_NOTIFY_TYPE_START_AUTO_RESUME_INFO://启动自动恢复
    case CUMW_NOTIFY_TYPE_AUTO_RESUME_RES: //自动恢复完成
        {
            CUMW_NOTIFY_ASYNC_RET_EX* pstContent = IVS_NEW(pstContent);
            if (NULL == pstContent)
            {
                IVS_DELETE(pstNotify);
                IVS_DELETE(pstCallback);
                BP_RUN_LOG_ERR(IVS_FAIL, "Pointer is NULL!", "");
                return IVS_ALLOC_MEMORY_ERROR;
            }
            memset(pstContent, 0, sizeof(CUMW_NOTIFY_ASYNC_RET_EX));

            memcpy(pstContent, pstNotifyInfo->pNotifyInfo, sizeof(CUMW_NOTIFY_ASYNC_RET_EX));
            pstNotify->pNotifyInfo = (void*)pstContent;
        }
        break;
    default:
        {
            BP_RUN_LOG_WAR("CIVS_OCXPlayerCtrl::HandleTVWallNotifyCallBack", "Unsupported NotifyType(0x%04x)", pstNotifyInfo->ulNotifyType);

            IVS_DELETE(pstCallback);
            IVS_DELETE(pstNotify);

            return IVS_FAIL;
        }
    }

    pstCallback->pUserData = pUserData;
    pstCallback->pstNotifyInfo = pstNotify;

    ::PostMessage(CIVS_OCXPlayerCtrl::m_pOCX->GetSafeHwnd(), WM_TVWALL_CALLBACK, (WPARAM)pstCallback, (LPARAM)pUserData);

    return IVS_SUCCEED;
}//lint !e818

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnTVWallCallback(WPARAM wParam, LPARAM lParam)
{
    BP_DBG_LOG("Enter OnTVWallCallback: NotifyType=0x%04X, OCX Address=0x%04X", wParam, lParam);

    ST_TV_WALL_CALLBACK_INFO* pstCallback = (ST_TV_WALL_CALLBACK_INFO*)wParam;
    if (NULL == pstCallback)
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "Pointer is NULL!", "");
        return IVS_PARA_INVALID;
    }

    if (NULL == pstCallback->pstNotifyInfo)
    {
        IVS_DELETE(pstCallback);
        BP_RUN_LOG_ERR(IVS_FAIL, "Pointer is NULL!", "");
        return IVS_PARA_INVALID;
    }

    if (NULL == pstCallback->pUserData)
    {
        IVS_DELETE(pstCallback->pstNotifyInfo);
        IVS_DELETE(pstCallback);
        BP_RUN_LOG_ERR(IVS_FAIL, "Pointer is NULL!", "");
        return IVS_PARA_INVALID;
    }

    if (NULL == pstCallback->pstNotifyInfo->pNotifyInfo)
    {
        IVS_DELETE(pstCallback->pstNotifyInfo);
        IVS_DELETE(pstCallback);
        BP_RUN_LOG_ERR(IVS_FAIL, "Pointer is NULL!", "");
        return IVS_PARA_INVALID;
    }

    CUMW_NOTIFY_INFO* pstNotifyInfo = pstCallback->pstNotifyInfo;
    void* pUserData = pstCallback->pUserData;

    IVS_DELETE(pstCallback);

    //CHECK_POINTER(pstNotifyInfo, IVS_FAIL);
    //CHECK_POINTER(pUserData, IVS_FAIL);

	IVS_INFO_TRACE("NotifyType=0x%04X, OCX Address=0x%p", pstNotifyInfo->ulNotifyType, pUserData);
	CIVS_OCXPlayerCtrl* pOCXPlayer = (CIVS_OCXPlayerCtrl*)pUserData;
	switch (pstNotifyInfo->ulNotifyType)
	{
	case CUMW_NOTIFY_TYPE_ASYNC_RET:
		{
            CUMW_NOTIFY_ASYNC_RET_EX* pstAsyncRet = (CUMW_NOTIFY_ASYNC_RET_EX*)pstNotifyInfo->pNotifyInfo;
            char* buf = IVS_NEW(buf, 1024*2);
			memset(buf, 0, 1024*2);
            if(IVS_SUCCEED==COCXTVWallXMLProcess::GetInst().GetNotifyAsyncRetXML(*pstAsyncRet, buf, 1024*2))
            {
                //向上层发消息
                pOCXPlayer->IVS_OCX_OnTVWall((LPCTSTR)buf);
            }
            IVS_DELETE(buf, MUILI);
        }
        break;
    case CUMW_NOTIFY_TYPE_REMOTE_DECODER_REGISTER:
        {
            CUMW_NOTIFY_REMOTE_DECODER_STATUS* pstInitInfo	= (CUMW_NOTIFY_REMOTE_DECODER_STATUS*)pstNotifyInfo->pNotifyInfo;
            char* buf = IVS_NEW(buf, 1024*5);
            memset(buf, 0, 1024*5);
            if(IVS_SUCCEED==COCXTVWallXMLProcess::GetInst().GetRemoteDecoderRegXML(*pstInitInfo, buf, 1024*5))
            {
                //向上层发注册消息
                pOCXPlayer->IVS_OCX_OnTVWall((LPCTSTR)buf);
                BP_DBG_LOG("CIVS_OCXPlayerCtrl::HandleTVWallNotifyCallBack, Decoder register, %s", buf);
                //通知电视墙窗格				
                CVideoPaneMgr*  videoPaneMgr= pOCXPlayer->GetVideoPaneMgr();
                if (NULL == videoPaneMgr)
                {
                    IVS_DELETE(pstNotifyInfo);
                    IVS_DELETE(pstInitInfo);
                    IVS_DELETE(buf, MUILI);
                    return IVS_FAIL;
                }

				IVS_ULONG curTVWallID = videoPaneMgr->GetCurTVWallID();

				//根据解码器获取窗格
				TVWall_VPANEID_LIST listWndID;
				listWndID.clear();
				if(IVS_SUCCEED == COCXTVWallMgr::GetTVWallWndIDByDecoderIDChannel(curTVWallID,*pstInitInfo,listWndID))
				{
					//将对应的窗格转换成真实的窗格
					std::vector<IVS_ULONG> vecRealPaneID;
					vecRealPaneID.clear();
					videoPaneMgr->GetRealTVwallPaneID(listWndID,vecRealPaneID);

					//设置TVWallBar背景图为上线
					IVS_UINT32 uSize = vecRealPaneID.size();
					for(IVS_UINT32 i= 0;i<uSize;i++)
					{
						videoPaneMgr->SetTVWallPaneChannelStatus(vecRealPaneID.at(i),TVWALL_BAR_STAUS_ONLINE);
                        /*CVideoPane *pVideoPane = videoPaneMgr->GetWndPointerByID(vecRealPaneID.at(i));
                        if (NULL != pVideoPane)
                        {
                            pVideoPane->ResetBarPosition();
                        }*/
					}					
				}
			}
			IVS_DELETE(buf, MUILI);
		}
        break;
	case CUMW_NOTIFY_TYPE_REMOTE_DECODER_UNRIGISTER:
	case CUMW_NOTIFY_TYPE_REMOTE_DECODER_DISCONNECT:
		{
			CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO* pstInfo
				= (CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO*)pstNotifyInfo->pNotifyInfo;
			char* buf = IVS_NEW(buf, 1024*2);
			memset(buf, 0, 1024*2);
			if(IVS_SUCCEED==COCXTVWallXMLProcess::GetInst().GetRemoteDecoderEndXML(*pstInfo, buf, 1024*2))
			{
				//向上层发消息
				pOCXPlayer->IVS_OCX_OnTVWall((LPCTSTR)buf);

                //通知电视墙窗格				
                CVideoPaneMgr*  videoPaneMgr= pOCXPlayer->GetVideoPaneMgr();
                if (NULL == videoPaneMgr)
                {
                    IVS_DELETE(pstNotifyInfo);
                    IVS_DELETE(pstInfo);
                    IVS_DELETE(buf, MUILI);
                    return IVS_FAIL;
                }

				IVS_ULONG curTVWallID = videoPaneMgr->GetCurTVWallID();

				//根据解码器获取窗格
				std::vector<IVS_ULONG> vecWndID;
				vecWndID.clear();
				CString strDecoderID(pstInfo->szDecoderID);
				if(IVS_SUCCEED == COCXTVWallMgr::GetTVWallWndIDByDecoderID(curTVWallID,strDecoderID,vecWndID))
				{
					//设置TVWallBar背景图为离线
					std::vector<IVS_ULONG> vecRealPaneID;
					vecRealPaneID.clear();
					videoPaneMgr->GetRealTVwallPaneIDByParentID(vecWndID,vecRealPaneID);
					IVS_UINT32 uSize = vecRealPaneID.size();
					for(IVS_UINT32 i= 0;i<uSize;i++)
					{
						ULONG ulWndID = vecRealPaneID.at(i);
                        //videoPaneMgr->SetTVWallPaneChannelStatus(ulWndID,TVWALL_BAR_STAUS_OFFLINE);
						CVideoPane* pVideoPane = videoPaneMgr->GetWndPointerByID(ulWndID);
						if(NULL!=pVideoPane)
						{
                            if(pVideoPane->GetServiceType()==SERVICE_TYPE_REALVIDEO)
                            {
                                if(TVWALL_STATUS_NONE != videoPaneMgr->GetTVWallPaneStatus(ulWndID))
                                {
                                    videoPaneMgr->SetTVWallPaneStatus(ulWndID, TVWALL_STATUS_DEC_DISCONN);
                                }
                            }
                            else
                            {
                                videoPaneMgr->SetTVWallPaneStatus(ulWndID, TVWALL_STATUS_NONE);
                            }
                            // modi by wKF71044 at [2013-7-3 15:55:28] 不显示正在上墙字样
                            pVideoPane->SetTvWallBgState(false);
                            pVideoPane->Invalidate();
                            //pVideoPane->ResetBarPosition(); // 有的时候电视墙通道图标不显示，需要调用这个函数刷新一下
						}
                        videoPaneMgr->SetTVWallPaneChannelStatus(ulWndID,TVWALL_BAR_STAUS_OFFLINE);
					}	
				}
			}
			IVS_DELETE(buf, MUILI);
		}
        break;
    case CUMW_NOTIFY_TYPE_RTSP_EXCEPTION:
    case CUMW_NOTIFY_TYPE_DECODER_ERROR:	//断流
        {
            CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO* pstErrInfo = (CUMW_NOTIFY_TYPE_DECODER_ERROR_INFO*)pstNotifyInfo->pNotifyInfo; 
            CVideoPaneMgr*  pVideoPaneMgr= pOCXPlayer->GetVideoPaneMgr();
            //CHECK_POINTER(pVideoPaneMgr,IVS_PARA_INVALID);
            if (NULL == pVideoPaneMgr)
            {
                IVS_DELETE(pstNotifyInfo);
                IVS_DELETE(pstErrInfo);
                BP_RUN_LOG_ERR(IVS_FAIL, "Pointer is NULL!", "");
                return IVS_FAIL;
            }
            ULONG ulWndID = 0;
            IVS_ULONG curTVWallID = pVideoPaneMgr->GetCurTVWallID();
            TVWall_VPANEID stPaneID = {0};

            BP_DBG_LOG("CIVS_OCXPlayerCtrl::HandleTVWallNotifyCallBack: curTVWallID = %d, pstErrInfo->szDecoderID = %s, pstErrInfo->usChannelNo = %d.",
                curTVWallID, pstErrInfo->szDecoderID, pstErrInfo->usChannelNo);

            if(IVS_SUCCEED==COCXTVWallMgr::GetTVWallWndIDByDecoderIDChannel(curTVWallID, pstErrInfo->szDecoderID, pstErrInfo->usChannelNo, stPaneID))
            {
                ulWndID = pVideoPaneMgr->GetRealIDByVirtualID(stPaneID);
                if(ulWndID>0)
                {
                    CVideoPane* pVideoPane = pVideoPaneMgr->GetWndPointerByID(ulWndID);
                    if(NULL!=pVideoPane)
                    {
                        if(pVideoPane->GetServiceType()==SERVICE_TYPE_REALVIDEO)
                        {
                            pVideoPaneMgr->SetTVWallPaneStatus(ulWndID, TVWALL_STATUS_VIDEO_BROKEN);
                        }
                        else
                        {
                            pVideoPaneMgr->SetTVWallPaneStatus(ulWndID, TVWALL_STATUS_NONE);
                        }
                    }
                }
            }

            {
                std::list<int> listTVWallId;
                (void)COCXTVWallXMLProcess::GetInst().GetTVWallIdList(listTVWallId);

                ULONG ulCurWndID = 0;
                IVS_ULONG ulTempTVWallID = 0;
                std::list<int>::iterator it = listTVWallId.begin();
                for (; it != listTVWallId.end(); it++)
                {
                    int iTVWallID = *it;
                    TVWall_VPANEID stTempPaneID = {0};

                    if(IVS_SUCCEED == COCXTVWallMgr::GetTVWallWndIDByDecoderIDChannel((IVS_ULONG)iTVWallID, pstErrInfo->szDecoderID, pstErrInfo->usChannelNo, stTempPaneID))
                    { 
                        ulCurWndID = pVideoPaneMgr->GetRealIDByVirtualID(stTempPaneID);
                        if(ulCurWndID > 0)
                        {
                            ulTempTVWallID = (IVS_ULONG)iTVWallID;
                            break;
                        }
                    }
                 }

                 BP_DBG_LOG("CIVS_OCXPlayerCtrl::HandleTVWallNotifyCallBack: ulCurWndID = %lu.", ulCurWndID); //lint !e539

                 if ((CUMW_NOTIFY_TYPE_RTSP_EXCEPTION == pstNotifyInfo->ulNotifyType) && 
                    (IVS_PLAYER_RET_RTSP_NORMAL_STOP == pstErrInfo->stErrorCode.ulErrorCode) &&
                    (ulCurWndID > 0) && (NULL != CIVS_OCXPlayerCtrl::m_pTvwallOcx) &&
                    (curTVWallID == ulTempTVWallID))
                 {
					char* buf = IVS_NEW(buf, 512);
					memset(buf, 0, 512);
                    if(IVS_SUCCEED==COCXTVWallXMLProcess::GetInst().GetRemotePlayBackXML((int)ulWndID, pstErrInfo->stErrorCode.ulErrorCode, buf, 512))
                    {
                        BP_DBG_LOG("CIVS_OCXPlayerCtrl::HandleTVWallNotifyCallBack: Send notify msg.");
                        //向上层发消息
                        CIVS_OCXPlayerCtrl::m_pTvwallOcx->IVS_OCX_OnTVWall((LPCTSTR)buf);
                    }
					IVS_DELETE(buf, MUILI);
                 }
            }
        }
        break;
    case CUMW_NOTIFY_TYPE_START_AUTO_RESUME_INFO://启动自动恢复
    case CUMW_NOTIFY_TYPE_AUTO_RESUME_RES: //自动恢复完成
        {
            CUMW_NOTIFY_ASYNC_RET_EX* pstInfo = (CUMW_NOTIFY_ASYNC_RET_EX*)pstNotifyInfo->pNotifyInfo;
            //通知电视墙窗格				
            CVideoPaneMgr*  pVideoPaneMgr= pOCXPlayer->GetVideoPaneMgr();
            //CHECK_POINTER(pVideoPaneMgr,IVS_PARA_INVALID);
            if (NULL == pVideoPaneMgr)
            {
                IVS_DELETE(pstNotifyInfo);
                IVS_DELETE(pstInfo);
                BP_RUN_LOG_ERR(IVS_FAIL, "Pointer is NULL!", "");
                return IVS_FAIL;
            }
            ULONG ulStatus = 0;
            ULONG ulWndID = 0;
            if(CUMW_NOTIFY_TYPE_START_AUTO_RESUME_INFO==pstNotifyInfo->ulNotifyType)
            {
                if(CUMW_BREAKDOWN_REASON_DECODER_OFFLINE==pstInfo->iReason)
                {
                    ulStatus = TVWALL_STATUS_DEC_DISCONN;
                }
                else if(CUMW_BREAKDOWN_REASON_NO_VIDEO==pstInfo->iReason)
                {
                    ulStatus = TVWALL_STATUS_VIDEO_BROKEN;
                }
            }
            else
            {//恢复成功，设置正在播放图标
                ulStatus = TVWALL_STATUS_PLAY;
            }

			IVS_ULONG curTVWallID = pVideoPaneMgr->GetCurTVWallID();
			TVWall_VPANEID stPaneID = {0};
			if(IVS_SUCCEED==COCXTVWallMgr::GetTVWallWndIDByDecoderIDChannel(curTVWallID, pstInfo->szDecoderID, pstInfo->uiChannel, stPaneID))
			{
				ulWndID = pVideoPaneMgr->GetRealIDByVirtualID(stPaneID);
				if(ulWndID>0)
				{
					pVideoPaneMgr->SetTVWallPaneStatus(ulWndID, ulStatus);
				}

                CVideoPane* pVideoPane = pVideoPaneMgr->GetWndPointerByID(ulWndID);
                if(NULL != pVideoPane  &&  CUMW_NOTIFY_TYPE_AUTO_RESUME_RES == pstNotifyInfo->ulNotifyType)
                {
                    pVideoPane->SetTvWallBgState(true);
                    pVideoPane->SetResumeState(false);
                    pVideoPane->Invalidate();
                }

                if (NULL != pVideoPane  &&  CUMW_NOTIFY_TYPE_START_AUTO_RESUME_INFO == pstNotifyInfo->ulNotifyType)
                {
                    pVideoPane->SetResumeState(true);
                    pVideoPane->Invalidate();
                }
			}
		}
        break;
	default:
		{
			BP_RUN_LOG_WAR("CIVS_OCXPlayerCtrl::HandleTVWallNotifyCallBack", "Unsupported NotifyType(0x%04x)", pstNotifyInfo->ulNotifyType);
		}
        break;
	}

    IVS_DELETE(pstNotifyInfo->pNotifyInfo);
    IVS_DELETE(pstNotifyInfo);

    return IVS_FAIL;
}//lint !e1762

const int PTZ_STOP_OCX_VALUE = 0XBD00;
const int PTZ_UP_OCX_VALUE = 0XBD01;
const int PTZ_TELE_OCX_VALUE = 0XBD05;  // 变倍+：    变倍大
const int PTZ_WIDE_OCX_VALUE = 0XBD06;  // 变倍-：    变倍小
const int PTZ_LEFT_UP_OCX_VALUE = 0XBD0B;
const int PTZ_RIGHT_DOWN_OCX_VALUE = 0XBD0E;
char g_szNetKeyBoardXML[1024] = {0};
IVS_INT32 __stdcall CIVS_OCXPlayerCtrl::NetKeyBoardCallBack(int iKeyValue,int iSpeedFirst,int iSpeedSecond, bool bTvWall)
{
	IVS_INFO_TRACE("Event OCX Address: %x, KeyValue: %d", CIVS_OCXPlayerCtrl::m_pOCX, iKeyValue);

	int iModfySpeedFirst = iSpeedFirst;
	if ((PTZ_UP_OCX_VALUE <= iKeyValue && iKeyValue <= PTZ_WIDE_OCX_VALUE)  ||  
		(PTZ_LEFT_UP_OCX_VALUE <= iKeyValue && iKeyValue <= PTZ_RIGHT_DOWN_OCX_VALUE))
	{ 
		int iMin = 0;
		int iMax = 0;

		BP_RUN_LOG_INF("CIVS_OCXPlayerCtrl::NetKeyBoardCallBack", "Recv The Pressed Key Value: cSpeed %d", iSpeedFirst);

		if (PTZ_TELE_OCX_VALUE == iKeyValue  ||  PTZ_WIDE_OCX_VALUE == iKeyValue) //变倍速度
		{
			iModfySpeedFirst = iSpeedFirst; //网络键盘传过来的，按照协议应该是0-7级别
			if (iModfySpeedFirst > 8)
			{
				iModfySpeedFirst = 8;
			}
			if (iModfySpeedFirst < 1)
			{
				iModfySpeedFirst = 0;
			}
		}
		else
		{
			if(IVS_SUCCEED == COCXTVWallXMLProcess::GetInst().GetNetKeyBoardConfig(iMin, iMax))
			{
				int iInterval = (iMax - iMin) + 1;
				iModfySpeedFirst = (iSpeedFirst + 1) * iInterval / 64;
				BP_RUN_LOG_INF("CIVS_OCXPlayerCtrl::NetKeyBoardCallBack", "Modify The Pressed Key Value:: cSpeed %d", iModfySpeedFirst);

				if (iModfySpeedFirst > iMax)
				{
					iModfySpeedFirst = iMax;
				}
				if (iModfySpeedFirst < iMin)
				{
					iModfySpeedFirst = iMin;
				}
			}
			else
			{
				iModfySpeedFirst = (iSpeedFirst + 1) * 10 / 64;
				if (iModfySpeedFirst > 10)
				{
					iModfySpeedFirst = 10;
				}
				if (iModfySpeedFirst < 1)
				{
					iModfySpeedFirst = 1;
				}
			}
		}

		BP_RUN_LOG_INF("CIVS_OCXPlayerCtrl::NetKeyBoardCallBack", "OldCallBackKeyValue:%d, OldSpeed:%d, Cur Speed:%d.",
			m_iOldCallBackKeyValue, m_cOldSpeed, iModfySpeedFirst);

		//过滤掉相同的云台方向速度
		int iCallBackKeyValue = iKeyValue;
		if (iCallBackKeyValue == m_iOldCallBackKeyValue  &&  iModfySpeedFirst == m_cOldSpeed  &&
			0 != m_iOldCallBackKeyValue                  &&  0 != m_cOldSpeed)
		{
			BP_DBG_LOG("CIVS_OCXPlayerCtrl::NetKeyBoardCallBack: Same Para, Not Need Send.");
			return IVS_SUCCEED;
		}

		m_iOldCallBackKeyValue = iCallBackKeyValue;
		m_cOldSpeed = iModfySpeedFirst;//lint !e734
	}

	if (PTZ_STOP_OCX_VALUE == iKeyValue) //收到云台停止命令,清理掉保存的数据
	{
		m_iOldCallBackKeyValue = 0;
		m_cOldSpeed = 0;//lint !e734
	}

    CXml xml;
    char szBuf[32] ={0};

    (void)xml.AddDeclaration("1.0", "UTF-8", "");

    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("MessageType");
    (void)(xml.IntoElem());
    (void)xml.SetElemValue("IVS_OCX_MSG_NETKEYBOARD");
    //xml.OutOfElem();

    (void)xml.AddElem("Netkeyboard");
    (void)xml.AddChildElem("Key");
    (void)(xml.IntoElem());
    (void)_itoa_s( iKeyValue, szBuf, 10 );
    (void)xml.SetElemValue(szBuf);

    (void)xml.AddElem("Value");
    memset(szBuf, 0, sizeof(szBuf));
    (void)_itoa_s( iModfySpeedFirst, szBuf, 10 );
    (void)xml.SetElemValue(szBuf);

    (void)xml.AddElem("WndID");
    memset(szBuf, 0, sizeof(szBuf));
    (void)_itoa_s( iSpeedSecond, szBuf, 10 );
    (void)xml.SetElemValue(szBuf); 
    xml.OutOfElem();
    xml.OutOfElem();

    unsigned int nXMLStreamLength;
    const char* pXml = xml.GetXMLStream(nXMLStreamLength);

     memset(g_szNetKeyBoardXML, 0, sizeof(g_szNetKeyBoardXML));
     memcpy(g_szNetKeyBoardXML, pXml, nXMLStreamLength);
     g_szNetKeyBoardXML[nXMLStreamLength] = '\0';

     //向上层发网络键盘消息
     CIVS_OCXPlayerCtrl::m_pOCX->OnNetKeyBoard(g_szNetKeyBoardXML);

	 BP_DBG_LOG("CIVS_OCXPlayerCtrl::NetKeyBoardCallBack: Send Msg %s.", g_szNetKeyBoardXML);

    return IVS_SUCCEED;
}//lint !e715
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetIAToolbar(ULONG ulToolbarFlag)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,ToolbarFlag: %lu", this,ulToolbarFlag);

    ULONG ulIABTNNUM = IVS_IA_TOOLBAR_DRAW_RECT | IVS_IA_TOOLBAR_DRAW_POLYGON
        | IVS_IA_TOOLBAR_ZOOM_IN | IVS_IA_TOOLBAR_ZOOM_OUT | IVS_IA_TOOLBAR_DRAW_MOVE
        | IVS_IA_TOOLBAR_DISPLAY_MODE |IVS_IA_TOOLBAR_PLAY | IVS_IA_TOOLBAR_DELETE;

    if (ulToolbarFlag > ulIABTNNUM)
    {
        return IVS_PARA_INVALID;
    }

    CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);

    m_pVideoPaneMgr->SetWndType(WND_TYPE_ANALYSIS);

    if (IVS_SUCCEED !=  m_pVideoPaneMgr->SetIAToolbar(ulToolbarFlag))
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Set IAToolbar Failed.","Reason:m_pVideoPaneMgr Set tool bar failed.");
        return IVS_FAIL;
    }
    return IVS_SUCCEED;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartPlatformPlayBack(LPCTSTR pCameraCode, LPCTSTR pPlaybackParam, LONG lWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,PlaybackParam: %s,WndID: %ld", this,pCameraCode,pPlaybackParam,lWndID);

    if (strlen(pCameraCode) == 0)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Platform PlayBack Failed.", "Reason:pCameraCode is empty");
        return IVS_PARA_INVALID;
    }

    if (lWndID < MIN_PANE_NUMBER || lWndID > MAX_PANE_NUMBER)
    {
        BP_RUN_LOG_ERR(IVS_DATA_INVALID,"IVS OCX Start Platform PlayBack Failed.", "Reason:win id is invalid");
        return IVS_DATA_INVALID;
    }

    IVS_PLAYBACK_PARAM stPlaybackParam;
    memset(&stPlaybackParam, 0, sizeof(IVS_PLAYBACK_PARAM));
	if (IVS_SUCCEED != COCXPlayBackXMLProcess::GetPlaybackParam(pPlaybackParam, stPlaybackParam))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Platform PlayBack Failed.", "Reason: Get Playback Param Failed.");
		return IVS_PARA_INVALID;
	}

    if (PROTOCOL_RTP_OVER_TCP != stPlaybackParam.uiProtocolType && PROTOCOL_RTP_OVER_UDP != stPlaybackParam.uiProtocolType)
    {
        BP_RUN_LOG_ERR(IVS_DATA_INVALID,"IVS OCX Start Platform PlayBack Failed.", "Reason:uiProtocolType invalid = %d.", stPlaybackParam.uiProtocolType);
        return IVS_DATA_INVALID;
    }

    bool bRet = CToolsHelp::IsSpeedVaild(stPlaybackParam.fSpeed);
    if (!bRet)
    {
        BP_RUN_LOG_ERR(IVS_DATA_INVALID,"IVS OCX Start Platform PlayBack Failed.", "Reason:fScale invalid. stPlaybackParam = %f.", stPlaybackParam.fSpeed);
        return IVS_DATA_INVALID;
    }

    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Start Platform PlayBack Failed.", "Reason:m_pVideoPaneMgr is NULL.");
        return IVS_FAIL;
    }

    CVideoPane * pVideoPane = m_pVideoPaneMgr->GetWndPointerByID(static_cast<ULONG>(lWndID));

    //指针 句柄为空判断
    CHECK_POINTER(pVideoPane,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pVideoPane->GetVideoWnd(),IVS_OPERATE_MEMORY_ERROR);

	pVideoPane->CreateBar();

	// 判断窗口是不是被局部放大占用，如果被占用，就关闭局部放大
	if ( BOTTOMBAR_MAGNIFY_OPEN == pVideoPane->GetDstMagnifyState())
	{
		CVideoPane* pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
		if (NULL != pSrcVideoPane)
		{
			::SendMessage(pSrcVideoPane->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (LONG)pVideoPane);
		}
	}

    if (VIDEO_PLAY_STATE_NOT_PLAYING != pVideoPane->GetPlayState())
    {
        BP_RUN_LOG_ERR(IVS_OCX_RET_VIDEOPANE_IS_PLAYING_ERROR, "IVS OCX Start Platform PlayBack Failed.", "Reason:Window has played video.");
        return IVS_OCX_RET_VIDEOPANE_IS_PLAYING_ERROR;
    }
    BP_RUN_LOG_INF("IVS OCX Start Platform PlayBack.", "window has played video.cameraid=%s.", pCameraCode);
    //当两个操作都成功后，要把窗格的一些状态修改
    m_csHWnd2OcxLock.Lock();
    (void)m_hWnd2OcxMap.insert(std::make_pair(pVideoPane->GetVideoWnd(), this));
    m_csHWnd2OcxLock.UnLock();

    IVS_INT32 iRet = 0;
    ULONG ulHandle = 0;
    pVideoPane->SetPlayState(VIDEO_PLAY_STATE_VOD);
    pVideoPane->Invalidate(TRUE);

    iRet = IVS_SDK_StartPlatformPlayBack(m_iSDKSessionID, pCameraCode, &stPlaybackParam, pVideoPane->GetVideoWnd(), &ulHandle);
    if (IVS_SUCCEED != iRet)
    {
        pVideoPane->SetPlayState(VIDEO_PLAY_STATE_NOT_PLAYING);
        pVideoPane->Invalidate(TRUE);
        BP_RUN_LOG_ERR(iRet, "IVS OCX Start Platform PlayBack Failed.", "Reason:SDK Start Platform PlayBack Failed.");
        return iRet;
    }
    BP_RUN_LOG_INF("IVS OCX Call SDK StartPlatformPlayBack Succeed.", "CameraID=%s, hwnd=%d, playhandle=%lu.", pCameraCode, pVideoPane->GetVideoWnd(), ulHandle);

    (void)IVS_SDK_SetDisplayScale(m_iSDKSessionID, ulHandle, (int)(m_pVideoPaneMgr->GetDispalyScale()));

    if (m_pVideoPaneMgr->isAnalysisWndType())
    {
        pVideoPane->SetAnalysisBarIsStartPlay(TRUE);
    }

    pVideoPane->SetPlayHandle(ulHandle);
    pVideoPane->SetServiceType(SERVICE_TYPE_PLAYBACK);
    pVideoPane->SetTitleButtonPic(true);
    pVideoPane->SetRealtimePlayPic(BUTTON_CLICKED);
    pVideoPane->SetCameraID((TCHAR *)pCameraCode);
    pVideoPane->EnableALLButton(TRUE);
	pVideoPane->SetIsPlatRecord(true);

	CGlobalPlayMgr &Mgr = CGlobalPlayMgr::Instance();
	(void)Mgr.RegPlayBackHandle(ulHandle, pVideoPane, pCameraCode);
    BP_RUN_LOG_INF("IVS OCX Start Platform PlayBack End.", "Result = %d",  iRet);
	// 按钮过滤，一些按钮需要被过滤，例如局部放大需要只有一个才能用
	DoButtonFilter(lWndID);
	ResumeMagnifyStatus(pVideoPane);

    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopPlatformPlayBack(LONG lWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);

    if (MIN_PANE_NUMBER > lWndID || MAX_PANE_NUMBER < lWndID)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop Platform PlayBack Failed.", "Reason:WndID is Invalid.");
        return IVS_PARA_INVALID;
    }

    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Platform PlayBack Failed.", "Reason:m_pVideoPaneMgr is null");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((ULONG)lWndID);
    if (NULL == pVideoPane)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Platform PlayBack Failed.","Reason:pVideoPane is NULL.");
        return IVS_OPERATE_MEMORY_ERROR;
    }

	/*****************************************************
	当关闭实况时,关闭随路语音，但是如果有复用，则，当该
	窗格的随路语音打开，则要关闭当前随路语音并打开其他随路语音
	******************************************************/
	//if(NULL != pVideoPane->GetVideoRealtimeBar())
	//{
	//	// 记录下该窗格是否打开随路语音
	//	if(BOTTOMBAR_SOUND_OPEN == pVideoPane->GetRealtimeSoundState())
 //       {
 //           pVideoPane->GetVideoRealtimeBar()->StopSound();
 //           pVideoPane->GetVideoRealtimeBar()->DeleteSoundCtrl();
	//	}
	//}

	//IVS_SDK_StopWndBusinessRestore( pVideoPane->GetVideoWnd() );
	(void)SetResumeState(pVideoPane->GetVideoWnd(), FALSE);

    ULONG ulRet = StopPlayBackByPane(pVideoPane, RECORD_METHOD_PLATFORM);
    BP_RUN_LOG_INF("IVS_OCX_StopPlatformPlayBack", "IVS_OCX_StopPlatformPlayBack end. result = %lu", ulRet);
    return 0;
}

// 容灾录像回放
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartDisasterRecoveryPlayBack( LPCTSTR pCameraCode,LPCTSTR pPlaybackParam,LONG lWndID )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
    CHECK_POINTER(pPlaybackParam,IVS_PARA_INVALID);
    IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,PlaybackParam: %s,WndID: %ld", this,pCameraCode,pPlaybackParam,lWndID);

    if (strlen(pCameraCode) == 0)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Platform PlayBack Failed.", "Reason:pCameraCode is empty");
        return IVS_PARA_INVALID;
    }

    if (lWndID < MIN_PANE_NUMBER || lWndID > MAX_PANE_NUMBER)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Platform PlayBack Failed.", "Reason:win id is invalid");
        return IVS_PARA_INVALID;
    }

    IVS_PLAYBACK_PARAM stPlaybackParam;
    memset(&stPlaybackParam, 0, sizeof(IVS_PLAYBACK_PARAM));
	if (IVS_SUCCEED != COCXPlayBackXMLProcess::GetPlaybackParam(pPlaybackParam, stPlaybackParam))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Disaster Recovery PlayBack Failed.", "Reason: Get Playback Param Failed.");
		return IVS_PARA_INVALID;
	}

    if (PROTOCOL_RTP_OVER_TCP != stPlaybackParam.uiProtocolType && PROTOCOL_RTP_OVER_UDP != stPlaybackParam.uiProtocolType)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Platform PlayBack Failed.", "Reason:uiProtocolType invalid = %d.", stPlaybackParam.uiProtocolType);
        return IVS_PARA_INVALID;
    }

    bool bRet = CToolsHelp::IsSpeedVaild(stPlaybackParam.fSpeed);
    if (!bRet)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Platform PlayBack Failed.", "Reason:fScale invalid. stPlaybackParam = %f.", stPlaybackParam.fSpeed);
        return IVS_PARA_INVALID;
    }

    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Start Platform PlayBack Failed.", "Reason:m_pVideoPaneMgr is NULL.");
        return IVS_FAIL;
    }

    CVideoPane * pVideoPane = m_pVideoPaneMgr->GetWndPointerByID(static_cast<ULONG>(lWndID));

    //指针 句柄为空判断
    CHECK_POINTER(pVideoPane,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pVideoPane->GetVideoWnd(),IVS_OPERATE_MEMORY_ERROR);

	pVideoPane->CreateBar();

    if (VIDEO_PLAY_STATE_NOT_PLAYING != pVideoPane->GetPlayState())
    {
        BP_RUN_LOG_ERR(IVS_OCX_RET_VIDEOPANE_IS_PLAYING_ERROR, "IVS OCX Start Platform PlayBack Failed.", "Reason:Window has played video.");
        return IVS_OCX_RET_VIDEOPANE_IS_PLAYING_ERROR;
    }
    BP_RUN_LOG_INF("IVS OCX Start Platform PlayBack.", "window has played video.cameraid=%s.", pCameraCode);
    //当两个操作都成功后，要把窗格的一些状态修改
    m_csHWnd2OcxLock.Lock();
    (void)m_hWnd2OcxMap.insert(std::make_pair(pVideoPane->GetVideoWnd(), this));
    m_csHWnd2OcxLock.UnLock();

    IVS_INT32 iRet = 0;
    ULONG ulHandle = 0;
    pVideoPane->SetPlayState(VIDEO_PLAY_STATE_VOD);
    pVideoPane->Invalidate(TRUE);

    iRet = IVS_SDK_StartDisasterRecoveryPlayBack(m_iSDKSessionID, pCameraCode, &stPlaybackParam, pVideoPane->GetVideoWnd(), &ulHandle);
    if (IVS_SUCCEED != iRet)
    {
        pVideoPane->SetPlayState(VIDEO_PLAY_STATE_NOT_PLAYING);
        pVideoPane->Invalidate(TRUE);
        BP_RUN_LOG_ERR(iRet, "IVS OCX Start Platform PlayBack Failed.", "Reason:SDK Start Platform PlayBack Failed.");
        return iRet;
    }
    BP_RUN_LOG_INF("IVS OCX Call SDK StartPlatformPlayBack Succeed.", "CameraID=%s, hwnd=%d, playhandle=%lu.", pCameraCode, pVideoPane->GetVideoWnd(), ulHandle);

    (void)IVS_SDK_SetDisplayScale(m_iSDKSessionID, ulHandle, (int)(m_pVideoPaneMgr->GetDispalyScale()));

    if (m_pVideoPaneMgr->isAnalysisWndType())
    {
        pVideoPane->SetAnalysisBarIsStartPlay(TRUE);
    }

    pVideoPane->SetPlayHandle(ulHandle);
    pVideoPane->SetServiceType(SERVICE_TYPE_DISASTEBACKUP_PLAYBACK);
    pVideoPane->SetTitleButtonPic(true);
    pVideoPane->SetRealtimePlayPic(BUTTON_CLICKED);
    pVideoPane->SetCameraID((TCHAR *)pCameraCode);
    pVideoPane->EnableALLButton(TRUE);
    //(void)((CVideoPaneMgr*)m_pVideoPaneMgr)->SetDevName((ULONG)lWndID, pCameraCode);

    CGlobalPlayMgr &Mgr = CGlobalPlayMgr::Instance();
    (void)Mgr.RegPlayBackHandle(ulHandle, pVideoPane, pCameraCode);

    BP_RUN_LOG_INF("IVS OCX Start Platform PlayBack End.", "Result = %d",  iRet);
    return iRet;
}

// 容灾录像回放停止
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopDisasterRecoveryPlayBack( LONG lWndID )
{
        AFX_MANAGE_STATE(AfxGetStaticModuleState());

    IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);

    if (MIN_PANE_NUMBER > lWndID || MAX_PANE_NUMBER < lWndID)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop Platform PlayBack Failed.", "Reason:WndID is Invalid.");
        return IVS_PARA_INVALID;
    }

    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Platform PlayBack Failed.", "Reason:m_pVideoPaneMgr is null");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((ULONG)lWndID);
    if (NULL == pVideoPane)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Platform PlayBack Failed.","Reason:pVideoPane is NULL.");
        return IVS_OPERATE_MEMORY_ERROR;
    }

	/*****************************************************
	当关闭实况时,关闭随路语音，但是如果有复用，则，当该
	窗格的随路语音打开，则要关闭当前随路语音并打开其他随路语音
	******************************************************/
	//if(NULL != pVideoPane->GetVideoRealtimeBar())
	//{
	//	// 记录下该窗格是否打开随路语音
	//	if(BOTTOMBAR_SOUND_OPEN == pVideoPane->GetRealtimeSoundState())
 //       {
 //           pVideoPane->GetVideoRealtimeBar()->StopSound();
 //           pVideoPane->GetVideoRealtimeBar()->DeleteSoundCtrl();
	//	}
	//}

	//IVS_SDK_StopWndBusinessRestore( pVideoPane->GetVideoWnd() );
	(void)SetResumeState(pVideoPane->GetVideoWnd(), FALSE);
        
    ULONG ulRet = StopPlayBackByPane(pVideoPane, RECORD_METHOD_DISASTER_RECOVERY);

    BP_RUN_LOG_INF("IVS_OCX_StopPlatformPlayBack", "IVS_OCX_StopPlatformPlayBack end. result = %lu", ulRet);
    return 0;
}

ULONG CIVS_OCXPlayerCtrl::StopPlayBackByPane(CVideoPane * pVideoPane,IVS_INT32 iRecordMethod)
{
    IVS_INFO_TRACE("OCX Address: %x, PlayHandle:%d", this, pVideoPane->GetPlayHandle());
    CHECK_POINTER(pVideoPane,IVS_OPERATE_MEMORY_ERROR);

	StopPlayResource(pVideoPane);

    if (VIDEO_PLAY_STATE_NOT_PLAYING == pVideoPane->GetPlayState())
    {
        BP_RUN_LOG_INF("IVS OCX Stop PlayBack By Pane Failed.", "Reason:the window is not playing.");
        return IVS_SUCCEED;
    }

	CHECK_POINTER(pVideoPane->GetVideoWnd(),IVS_OPERATE_MEMORY_ERROR);

	// 停止轨迹叠加
	(void)pVideoPane->DisableOverlay(m_iSDKSessionID, pVideoPane->GetPlayHandle());

	// 如果原来被占用成了放大，先删掉这个放大，等停止后再恢复一个窗口回去
	IA_GRAPHICS graphicsTemp;
	memset(&graphicsTemp,0,sizeof(IA_GRAPHICS));
	bool bOpen = false;
	CVideoPane* pMagnifySrcPane = NULL;
	if (BOTTOMBAR_MAGNIFY_OPEN == pVideoPane->GetDstMagnifyState())
	{
		bOpen = true;
		pMagnifySrcPane = pVideoPane->GetMagnifySrcPane();

		ULONG ulNum = 1;// 获取一个图形
		(void)pMagnifySrcPane->GetGraphicsByGroup((ULONG)pVideoPane, &graphicsTemp, &ulNum);
		// 删除掉源窗口的信息
		::SendMessage(pVideoPane->GetMagnifySrcPane()->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (LONG)(pVideoPane));
	}

     //停止播放
    IVS_INT32 ulRet = IVS_FAIL;
    if (RECORD_METHOD_PLATFORM == iRecordMethod)
    {
        ulRet = IVS_SDK_StopPlatformPlayBack(m_iSDKSessionID, pVideoPane->GetPlayHandle());
    }
    else if (RECORD_METHOD_PU == iRecordMethod)
    {
        ulRet = IVS_SDK_StopPUPlayBack(m_iSDKSessionID, pVideoPane->GetPlayHandle());
    }
    else if(RECORD_METHOD_DISASTER_RECOVERY == iRecordMethod)
    {
         ulRet = IVS_SDK_StopDisasterRecoveryPlayBack(m_iSDKSessionID, pVideoPane->GetPlayHandle());
    }
	else
	{
		ulRet = IVS_SDK_StopBackupPlayBack(m_iSDKSessionID, pVideoPane->GetPlayHandle());
	}

    if (IVS_SUCCEED != ulRet)
    {
        BP_RUN_LOG_ERR(ulRet, "IVS OCX Stop PlayBack By Pane Failed.",
                       "Reason:SDK Stop PlayBack Failed.");
    }
  
	if (pVideoPane->IsSyncPlayBack())
	{
		CSyncPlayBackMgr &mgr = CSyncPlayBackMgr::Instance();
		(void)mgr.DeleteSyncPanel(pVideoPane);
		pVideoPane->SetSyncPlayBackMode(false);
	}

	if (pVideoPane->GetPlayHandle())
	{
		CGlobalPlayMgr &PlayMgr = CGlobalPlayMgr::Instance();
		(void)PlayMgr.UnRegPlayHandle(pVideoPane->GetPlayHandle(), pVideoPane);
	}

	pVideoPane->SetPlayHandle(0);
	pVideoPane->SetServiceType(SERVICE_TYPE_REALVIDEO);// 默认初始化为实况;

    //如果是由于控件cleanUp而触发的stopplay，则不会重置窗格
    if (m_bIsInit)
    {
        if (pVideoPane->GetIsLocalRecord())
        {
            PostOcxEventMsg("IVS_OCX_MSG_STOP_LOCAL_RECORD",0,0,pVideoPane->GetPaneID());
        }

        // 把对应窗格状态清空
        if ((!pVideoPane->GetResumeState())
            && (WND_TYPE_NORMAL != pVideoPane->GetWndType()))
        {
            pVideoPane->SetTitleButtonPic(false);
        }
        pVideoPane->ResetPane();
        pVideoPane->SetRealtimePlayPic(BUTTON_UNABLE);
        pVideoPane->SetCameraType(CAMERA_TYPE_CAVITY_FIXED);
    }
	//添加录像结束Pane复位
	pVideoPane->ResetPane();

	// 如果自己是被占用成放大窗口，需要重新加一个放大的加上去，避免播放结束关闭完成了之后，放大窗口消失掉
	if (bOpen)
	{
		// 从新画一个上去
		if (NULL != pMagnifySrcPane)
		{
			pMagnifySrcPane->ResumeMagnifyRectByGraphic(graphicsTemp);
		}
	}

    BP_RUN_LOG_INF("IVS OCX Stop Play Back ByPane End.", "Result = %d, winID=%d, hwnd=%d.", ulRet, pVideoPane->GetPaneID(), pVideoPane->GetVideoWnd());

    return (ULONG)ulRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_PlayBackPause(LONG lWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);

    if (MIN_PANE_NUMBER > lWndID || MAX_PANE_NUMBER < lWndID)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop Platform PlayBack Failed.", "Reason:window ID is invalid.");
        return IVS_PARA_INVALID;
    }

    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Platform PlayBack Failed.", "Reason:m_pVideoPaneMgr is null");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((ULONG)lWndID);
    if (NULL == pVideoPane)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Platform PlayBack Failed.","Reason:pVideoPane is NULL.");
        return IVS_OPERATE_MEMORY_ERROR;
    }

	if (pVideoPane->IsSyncPlayBack())
	{
		CSyncPlayBackMgr &mgr = CSyncPlayBackMgr::Instance();
		return  mgr.PauseSyncPlay(); 
	}

    return IVS_SDK_PlayBackPause(m_iSDKSessionID, (ULONG)pVideoPane->GetPlayHandle());

    //int iRet = -1;
    //if( SDK_PLAYBACK_PF == pVideoPane->GetPlayBackType() || SDK_PLAYBACK_PU == pVideoPane->GetPlayBackType() )
    //{
    //    iRet = IVS_SDK_RemotePlayBackPause(m_iSDKSessionID,pVideoPane->GetPlayBackHandle());
    //}
    //else if( SDK_PLAYBACK_LF == pVideoPane->GetPlayBackType())
    //{
    //    iRet = IVS_SDK_LocalPlayBackPause( pVideoPane->GetPlayBackHandle() );
    //}
    //if (PLAYER_RET_OK != iRet)
    //{
    //    IVS_LOG(LOG_INFO, "IVS_OCX_PlayBackPause failed .");
    //    return iRet;
    //}
    //// TODO: Add your dispatch handler code here
    //IVS_LOG(LOG_INFO, "IVS_OCX_PlayBackPause end.");
    //BP_RUN_LOG_INF("IVS OCX Play Back Pause End.", "Result = %d", iRet);
    //return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_PlayBackResume(LONG lWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);

    if (MIN_PANE_NUMBER > lWndID || MAX_PANE_NUMBER < lWndID)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop Platform PlayBack Failed.", "Reason:window ID is invalid.");
        return IVS_PARA_INVALID;
    }

    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Platform PlayBack Failed.", "Reason:m_pVideoPaneMgr is Null");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)lWndID);
    if (NULL == pVideoPane)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Platform PlayBack Failed","Reason:pVideoPane is NULL.");
        return IVS_OPERATE_MEMORY_ERROR;
    }

	if (pVideoPane->IsSyncPlayBack())
	{
		CSyncPlayBackMgr &mgr = CSyncPlayBackMgr::Instance();
		return  mgr.ResumeSyncPlay(); 
	}

    int iRet = IVS_SDK_PlayBackResume(m_iSDKSessionID, (IVS_ULONG)pVideoPane->GetPlayHandle());

    BP_RUN_LOG_INF("IVS OCX Play Back Resume End.", "Result = %d", iRet);
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_PlaybackFrameStepForward(LONG lWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);
	
	CVideoPane * pVideoPane = GetVideoPanel((unsigned long)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "OCX Play back Frame Step Forward Failed","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	if (pVideoPane->IsSyncPlayBack())
	{
		CSyncPlayBackMgr &mgr = CSyncPlayBackMgr::Instance();
		return  mgr.StepPlayForward(); 
	}

	return IVS_SDK_PlayBackFrameStepForward(m_iSDKSessionID, (IVS_ULONG)pVideoPane->GetPlayHandle());
	//return IVS_SDK_PlayBackFrameStepBackward(m_iSDKSessionID, (IVS_ULONG)pVideoPane->GetPlayHandle());
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetPlayBackSpeed(LONG lWndID, FLOAT fSpeed)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address:%x, WndID:%ld  Speed=%f", this, lWndID, fSpeed);

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Platform Speed Failed.", "Reason:m_pVideoPaneMgr is Null");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane *pVideoPane = ((CVideoPaneMgr *)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Platform Speed Failed","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	if (pVideoPane->IsSyncPlayBack())
	{
		CSyncPlayBackMgr &mgr = CSyncPlayBackMgr::Instance();
		return  mgr.SetPlaySpeed(fSpeed); 
	}

	return IVS_SDK_SetPlayBackSpeed(m_iSDKSessionID, (IVS_ULONG)pVideoPane->GetPlayHandle(), fSpeed);
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetPlayBackSpeed(LONG lWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);
	CString strResult;
	int iRet = IVS_SUCCEED;
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Play back Speed Failed.", "Reason:m_pVideoPaneMgr is Null");
		iRet = IVS_OPERATE_MEMORY_ERROR;
		return strResult.AllocSysString();
	}

	CVideoPane *pVideoPane = ((CVideoPaneMgr *)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Play back Speed Failed","Reason:pVideoPane is NULL.");
		iRet = IVS_OPERATE_MEMORY_ERROR;
		return strResult.AllocSysString();
	}

	float fSpeed = 0.0;
	if (pVideoPane->IsSyncPlayBack())
	{
		CSyncPlayBackMgr &mgr = CSyncPlayBackMgr::Instance();
		fSpeed = mgr.GetPlaySpeed(); 
	}
	else
	{
		iRet = IVS_SDK_GetPlayBackSpeed(m_iSDKSessionID, pVideoPane->GetPlayHandle(), &fSpeed);
	}

	// 将回放速度拼成xml返回给界面;
	CXml xmlPlaybackSpeed;
	iRet = COCXPlayBackXMLProcess::GetPlayBackSpeed(fSpeed, iRet, xmlPlaybackSpeed);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get PlayBack Speed Failed.","Reason:Parse Get Play BackTime Result XML Failed.");
	}

	unsigned int uiLen = 0;
	const char* pXml = xmlPlaybackSpeed.GetXMLStream(uiLen);
	if (NULL == pXml)
	{
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	strResult = pXml;

	BP_RUN_LOG_INF("IVS OCX Get Play Back Speed End.", "Retxml = %s, Result = %d", pXml, iRet);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetPlayBackTime(LONG lWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);
    CString strResult;
    CVideoPane * pVideoPane = GetVideoPanel((ULONG)lWndID);
    if (NULL == pVideoPane)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Play BackTime Failed.","Reason:pVideoPane is NULL.");
        return strResult.AllocSysString();
    }

	IVS_PLAYBACK_TIME stPlaybackTime;
	memset(&stPlaybackTime, 0, sizeof(IVS_PLAYBACK_TIME));
	int iRet = IVS_FAIL;
	
	if (pVideoPane->IsSyncPlayBack())
	{
		CSyncPlayBackMgr &Mgr = CSyncPlayBackMgr::Instance();
		iRet = Mgr.GetPlayedTime(stPlaybackTime);
	}
	else
	{
		IVS_ULONG ulPlayHandle = (IVS_ULONG)pVideoPane->GetPlayHandle();
		if (0 == ulPlayHandle)
		{
			BP_RUN_LOG_ERR(IVS_OCX_RET_NO_PLAY_HANDLE, "IVS OCX Get PlayBack Time Failed.","Reason:ulPlayHandle = 0.");
			return strResult.AllocSysString();
		}

		
		iRet = IVS_SDK_GetPlayBackTime(m_iSDKSessionID, ulPlayHandle, &stPlaybackTime);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "IVS OCX Get PlayBack Time Failed.","Reason: SDK Get PlayBack Time Failed.");
		}
	}

	BP_RUN_LOG_INF("IVS OCX Get PlayBack Time","%s %s %s", stPlaybackTime.cPlayBackStartTime, stPlaybackTime.cPlayBackEndTime, 
				   stPlaybackTime.cPlayBackTime);

    // 将回放时间拼成xml返回给界面;
    CXml xmlPlaybackTime;
    iRet = COCXPlayBackXMLProcess::GetPlayBackTime(stPlaybackTime, iRet, xmlPlaybackTime);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get PlayBack Time Failed.","Reason:Parse Get Play BackTime Result XML Failed.");
	}

    unsigned int uiLen = 0;
	const char* pXml = xmlPlaybackTime.GetXMLStream(uiLen);
	if (NULL == pXml)
	{
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	strResult = pXml;

    BP_RUN_LOG_INF("IVS OCX Get Play Back Time End.", "Retxml = %s, Result = %d", pXml, iRet);

    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetPlayBackStatus(LONG lWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);

	CString strResult;
	int iRet = IVS_SUCCEED;
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Play back Status failed.", "Reason:m_pVideoPaneMgr is Null");
		iRet = IVS_OPERATE_MEMORY_ERROR;
		return strResult.AllocSysString();
	}

	CVideoPane *pVideoPane = ((CVideoPaneMgr *)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Play back Status failed.","Reason:pVideoPane is NULL.");
		iRet = IVS_OPERATE_MEMORY_ERROR;
		return strResult.AllocSysString();
	}

	IVS_BOOL bStatus = 0;// 初始化为暂停;
	if (pVideoPane->IsSyncPlayBack())
	{
		CSyncPlayBackMgr &mgr = CSyncPlayBackMgr::Instance();
		bStatus = mgr.GetPlayStatus(); 
	}
	else
	{
		iRet = IVS_SDK_GetPlayBackStatus(m_iSDKSessionID, pVideoPane->GetPlayHandle(), &bStatus);
	}

	// 将回放速度拼成xml返回给界面;
	CXml xmlPlaybackStatus;
	iRet = COCXPlayBackXMLProcess::GetPlayBackStatus(bStatus, iRet, xmlPlaybackStatus);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get PlayBack Status Failed.","Reason:Parse Get Play BackTime Result XML Failed.");
	}

	unsigned int uiLen = 0;
	const char* pXml = xmlPlaybackStatus.GetXMLStream(uiLen);
	if (NULL == pXml)
	{
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	strResult = pXml;
	BP_RUN_LOG_INF("IVS OCX Get Play Back Speed End.", "Retxml = %s, Result = %d", pXml, iRet);
	return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetPlayBackTime(LONG lWndID, ULONG ulTime)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld,Time: %lu", this,lWndID,ulTime);
    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set PlayBack Time Failed.", "Reason:m_pVideoPaneMgr is NULL.");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    if (!m_pVideoPaneMgr->IsLegalityWndID(static_cast<ULONG>(lWndID)))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set PlayBack Time Failed.", "Reason:window ID is Invalid.");
        return IVS_PARA_INVALID;
    }

    CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)lWndID);
    if (NULL == pVideoPane)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set PlayBack Time Failed.","Reason:pVideoPane is NULL.");
        return IVS_OPERATE_MEMORY_ERROR;
    }

	if (pVideoPane->IsSyncPlayBack())
	{
		CSyncPlayBackMgr &SyncMgr = CSyncPlayBackMgr::Instance();
		return SyncMgr.SetPlayTime(ulTime);
	}

    int iRet = IVS_SDK_SetPlayBackTime(m_iSDKSessionID, (IVS_ULONG)pVideoPane->GetPlayHandle(), (IVS_UINT32)ulTime);
    BP_RUN_LOG_INF("IVS OCX Set PlayBack Time End.", "Result = %d", iRet);
    return iRet;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnLocalPlayBackEvent(WPARAM wParam,LPARAM lParam)
{
	// 向界面抛事件;
	CString xmlToCU ="";
	int iExceptionCode = (int)lParam;
	ULONG WinID        = (ULONG)wParam;

	COCXPlayBackXMLProcess::GetStreamExceptionMsgXML(xmlToCU, WinID, iExceptionCode, 0);
	IVS_OCX_Windows_Event(IVS_EVENT_LOCAL_PLAYBACK_FAILED, (LPCTSTR)xmlToCU);

	// 关闭局部放大， add by z00193167
	CVideoPane* pVideoPane = GetVideoPanel((unsigned long)WinID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_INF("close zoom failed","NA");
		return S_OK;
	}

//	if (MAGNIFY_OPEN == pVideoPane->GetRealtimeMagnifyState())
//	{
//		// 发送一个点击事件给放大按钮，让他关闭自己，这里使用同步的sendmessage,不要使用postmessage
//		::SendMessage(pVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_MAGNIFY, BN_CLICKED), (LPARAM)pVideoPane->GetVideoRealtimeBar()->GetMagnifyBtnHwnd());
//	}

	//如果是电视墙，判断同步上墙是否需要进行下墙动作
	if(0 != m_pVideoPaneMgr->GetCurTVWallID())
	{
		CTVWallSyncPlayBackMgr &TVWallSyncMgr = CTVWallSyncPlayBackMgr::Instance();
		(void)TVWallSyncMgr.StopTVWallPlayBackByWndID(WinID);
	}

	return S_OK;
}


afx_msg LRESULT CIVS_OCXPlayerCtrl::OnRemotePlayBackEvent(WPARAM wParam,LPARAM lParam)
{
#if 0 
	
	if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Process Remote PlayBack Event Failed", "Reason:m_pVideoPaneMgr = NULL.");
        return S_FALSE;
    }

    CVideoPane *pVideoPane  = m_pVideoPaneMgr->GetWndPointerByHWND((HWND)wParam);
    pVideoPane->SetPlayState(VIDEO_PLAY_STATE_NOT_PLAYING);

    /*if (IVS_PLAYER_RET_RTSP_DISCONNECT == exceptionInfo->iExceptionCode
    || IVS_PLAYER_RET_RECV_DATA_TIMEOUT == exceptionInfo->iExceptionCode)
    {
    (void)pOcxCtrl->SetResumeState(hWnd, TRUE);
    }*/
    pVideoPane->Invalidate(TRUE);

#endif 

    // 向界面抛事件;
    CString xmlToCU ="";
    int iExceptionCode = (int)lParam;
	ULONG WinID        = (ULONG)wParam;

    COCXPlayBackXMLProcess::GetStreamExceptionMsgXML(xmlToCU, WinID, iExceptionCode, 0);
    IVS_OCX_Windows_Event(IVS_EVENT_REMOTE_PLAYBACK_FAILED, (LPCTSTR)xmlToCU);

	// 关闭局部放大， add by z00193167
// 	CVideoPane* pVideoPane = GetVideoPanel((unsigned long)WinID);
// 	if (NULL == pVideoPane)
// 	{
// 		BP_RUN_LOG_INF("close zoom failed","NA");
// 		return S_OK;
// 	}
// 	// 如果自己是放大窗口，要关闭掉
// 	if (MAGNIFY_OPEN == pVideoPane->GetRealtimeMagnifyState())
// 	{
// 		// 发送一个点击事件给放大按钮，让他关闭自己，这里使用同步的sendmessage,不要使用postmessage
// 		::SendMessage(pVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_MAGNIFY, BN_CLICKED), (LPARAM)pVideoPane->GetVideoRealtimeBar()->GetMagnifyBtnHwnd());
// 	}
// 
// 	// 如果自己是被占用成放大窗口，需要重新加一个放大的加上去，避免播放结束关闭完成了之后，放大窗口消失掉
// 	if (BOTTOMBAR_MAGNIFY_OPEN == pVideoPane->GetDstMagnifyState())
// 	{
// 		IVS_RECT IArectPercent = pVideoPane->GetIArectPercent();
// 		IVS_SDK_AddPlayWindowPartial(m_iSDKSessionID, pVideoPane->GetMagnifyPlayHandle(), pVideoPane->GetVideoWnd(), &IArectPercent);
// 		pVideoPane->SetPrvePlayState(VIDEO_PLAY_STATE_NOT_PLAYING); // 被关闭时，状态设置成不播放
// 		pVideoPane->SetPlayState(VIDEO_PLAY_STATE_REALTIME);
// 		pVideoPane->SetPlayHandle(0);
// 		pVideoPane->SetMagnifyPlayHandle(pVideoPane->GetMagnifyPlayHandle());
// 		pVideoPane->ResetBarPosition();
// 	}

    return S_OK;
}

// 启动OCX绘图
LONG CIVS_OCXPlayerCtrl::IVS_OCX_EnableDraw(ULONG ulWindowID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu", this,ulWindowID);

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Enable Draw Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Enable Draw Failed", "Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return pVideoPane->EnableDraw(m_iSDKSessionID, pVideoPane->GetPlayHandle());
}

// 停止OCX绘图
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DisableDraw(ULONG ulWindowID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu", this,ulWindowID);

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Disable Draw Failed", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Disable Draw Failed.", "Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return pVideoPane->DisableDraw(m_iSDKSessionID, pVideoPane->GetPlayHandle());
}

// 设置图形
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetGraphics(ULONG ulWindowID, LPCTSTR pGraphicsXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pGraphicsXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu,GraphicsXml: %s", this,ulWindowID, pGraphicsXml);
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set Graphics Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set Graphics Failed.","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	// 创建IAGraphic结构体数组，解析XML，赋值给结构
	IA_GRAPHICS* pGraphicsArr = (IA_GRAPHICS*)IVS_NEW((IVS_CHAR* &)pGraphicsArr, GRAPHICS_NUM_MAX * sizeof(IA_GRAPHICS));//lint !e826
	CHECK_POINTER(pGraphicsArr, IVS_ALLOC_MEMORY_ERROR);
	memset(pGraphicsArr, 0, sizeof(IA_GRAPHICS) * GRAPHICS_NUM_MAX);
	ULONG ulNum = 0;
	SIZE sz = {0};
	if (IVS_SUCCEED != COCXDrawXMLProcess::SetGraphicsPraseXML(pGraphicsArr, ulNum, pGraphicsXml, sz, FALSE))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Set Graphics Failed.", "Reason:ParseXml Fail");
		return IVS_PARA_INVALID;
	}
	LONG lRet = pVideoPane->SetGraphics(pGraphicsArr, ulNum, FALSE);
	IVS_DELETE(pGraphicsArr, MUILI);
	return lRet;
}

// 设置智能分析图形组之间的关系
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetRelationShip(ULONG ulWindowID, ULONG ulGraphicGroupA, ULONG ulGraphicGroupB, ULONG ulRelationShipType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu,GraphicGroupA: %lu,GraphicGroupB: %lu,RelationShipType: %lu",
		            this,ulWindowID,ulGraphicGroupA,ulGraphicGroupB,ulRelationShipType);

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set RelationShip Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set RelationShip Failed.","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	if (5 < ulRelationShipType)// RELATION_SHIP_TYPE值只有0~5
	{
		ulRelationShipType = 0;
	}
	RELATION_SHIP_TYPE ulType = (RELATION_SHIP_TYPE)ulRelationShipType;
	return pVideoPane->SetRelationShip(ulGraphicGroupA, ulGraphicGroupB, ulType);
}

// 按百分比坐标设置图形
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetGraphicsByPercent(ULONG ulWindowID, LPCTSTR pGraphicsXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pGraphicsXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu,GraphicsXml: %s", this,ulWindowID,pGraphicsXml);
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set Graphics By Percent Failed.", "Reason: m_pVideoPaneMgr is Null.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	// 根据WindowID找到VideoPane
	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set Graphics By Percent Failed.","Reason: pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	// 通过MediaInfo，获取图像大小，存入sz
	BOOL bPersent = FALSE;
	ULONG ulHandle = pVideoPane->GetPlayHandle();
	IVS_MEDIA_INFO strMediaInfo;
	memset(&strMediaInfo,0,sizeof(IVS_MEDIA_INFO));
	IVS_INT32 iRet = IVS_SDK_GetMediaInfo(m_iSDKSessionID, ulHandle, &strMediaInfo);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_INF("IVS OCX Set Graphics By Percent Failed.","Reason: Get Media Info from SDK Failed");
		bPersent = TRUE;
	}

	SIZE sz = {0};
	sz.cx = (LONG)strMediaInfo.uiVideoWidth;
	sz.cy = (LONG)strMediaInfo.uiVideoHeight;
	if ((0 >= sz.cx) || (0 >= sz.cy))
	{
		BP_RUN_LOG_INF("IVS OCX Set Graphics By Percent Failed.","Reason: Video Size is Zero");
		bPersent = TRUE;
	}

	// 创建IAGraphic结构体数组，解析XML，赋值给结构体
	IA_GRAPHICS* pGraphicsArr = (IA_GRAPHICS*)IVS_NEW((IVS_CHAR* &)pGraphicsArr, GRAPHICS_NUM_MAX * sizeof(IA_GRAPHICS));//lint !e826
	CHECK_POINTER(pGraphicsArr, IVS_ALLOC_MEMORY_ERROR);
	memset(pGraphicsArr, 0, sizeof(IA_GRAPHICS) * GRAPHICS_NUM_MAX);
	ULONG ulNum = 0;
	if (IVS_SUCCEED != COCXDrawXMLProcess::SetGraphicsPraseXML(pGraphicsArr, ulNum, pGraphicsXml, sz, bPersent))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Set Graphics By Percent Failed.", "Reason: ParseXml Failed.");
		return IVS_PARA_INVALID;
	}
	LONG lRet = pVideoPane->SetGraphics(pGraphicsArr, ulNum, bPersent);
	IVS_DELETE(pGraphicsArr, MUILI);
	return lRet;
}

// 按百分比坐标获取图形
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetGraphicsByPercent(ULONG ulWindowID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu", this,ulWindowID);

	CString strResult;
	CXml xmlGraphics;
	unsigned int uiLen = 0;

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Graphics By Percent Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_OPERATE_MEMORY_ERROR, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}
	// 根据WindowID找到VideoPane
	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Graphics By Percent Failed.","Reason:pVideoPane is NULL.");
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_OPERATE_MEMORY_ERROR, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}

	// 通过MediaInfo，获取图像大小，存入sz
	ULONG ulHandle = pVideoPane->GetPlayHandle();
	IVS_MEDIA_INFO strMediaInfo;
	memset(&strMediaInfo,0,sizeof(IVS_MEDIA_INFO));
	IVS_INT32 iRet = IVS_SDK_GetMediaInfo(m_iSDKSessionID, ulHandle, &strMediaInfo);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Graphics By Percent Failed","Reason:Get Media Info from SDK Failed");
		COCXDrawXMLProcess::GetGraphicsGetErrXML(iRet, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}
	SIZE sz = {0};
	sz.cx = (LONG)strMediaInfo.uiVideoWidth;
	sz.cy = (LONG)strMediaInfo.uiVideoHeight;
	if ((0 >= sz.cx) || (0 >= sz.cy))
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get Graphics By Percent Failed.","Reason:Video Size is Zero");
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_FAIL, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}

	// 创建IAGraphic结构体数组，传入SDK获取值
	ULONG ulNum = GRAPHICS_NUM_MAX;
	IA_GRAPHICS* pGraphicsArr = (IA_GRAPHICS*)IVS_NEW((IVS_CHAR* &)pGraphicsArr, GRAPHICS_NUM_MAX * sizeof(IA_GRAPHICS));//lint !e826
	CHECK_POINTER(pGraphicsArr, NULL);
	memset(pGraphicsArr, 0, sizeof(IA_GRAPHICS) * GRAPHICS_NUM_MAX);

	iRet = pVideoPane->GetGraphics(pGraphicsArr, &ulNum);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Graphics By Percent Failed.","Reason:pVideoPane Get Graphics By Percent failed");
		IVS_DELETE(pGraphicsArr, MUILI);
		COCXDrawXMLProcess::GetGraphicsGetErrXML(iRet, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}

	// 根据结构体返回值构建XML，赋值给strResult
	if (IVS_SUCCEED != COCXDrawXMLProcess::GetGraphicsGetXML(pGraphicsArr, iRet, ulNum, xmlGraphics, sz))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS OCX Get Graphics By Percent Failed.", "Reason:Get XML failed.");
		IVS_DELETE(pGraphicsArr, MUILI);
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_ALLOC_MEMORY_ERROR, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}
	IVS_DELETE(pGraphicsArr, MUILI);
	strResult = xmlGraphics.GetXMLStream(uiLen);
	BP_RUN_LOG_INF("IVS OCX Get Graphics By Percent End.", "Result = %d.", iRet);

	return strResult.AllocSysString();
}

// 获取图形
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetGraphics(ULONG ulWindowID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu", this,ulWindowID);

	CString strResult;
	CXml xmlGraphics;
	unsigned int uiLen = 0;

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Graphics Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_OPERATE_MEMORY_ERROR, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Graphics Failed.", "Reason:pVideoPane is NULL.");
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_OPERATE_MEMORY_ERROR, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}

	// 创建IAGraphic结构体数组，传入SDK获取值
	ULONG ulNum = GRAPHICS_NUM_MAX;
	IA_GRAPHICS* pGraphicsArr = (IA_GRAPHICS*)IVS_NEW((IVS_CHAR* &)pGraphicsArr, GRAPHICS_NUM_MAX * sizeof(IA_GRAPHICS));//lint !e826
	CHECK_POINTER(pGraphicsArr, NULL);
	memset(pGraphicsArr, 0, sizeof(IA_GRAPHICS) * GRAPHICS_NUM_MAX);
	IVS_INT32 iRet = pVideoPane->GetGraphics(pGraphicsArr, &ulNum);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Graphics Failed.","Reason:pVideoPane Get Graphics failed.");
		IVS_DELETE(pGraphicsArr, MUILI);
		COCXDrawXMLProcess::GetGraphicsGetErrXML(iRet, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}

	// 根据结构体返回值构建XML，赋值给strResult
	SIZE sz = {0};
	if (IVS_SUCCEED != COCXDrawXMLProcess::GetGraphicsGetXML(pGraphicsArr, iRet, ulNum, xmlGraphics, sz))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS OCX Get Graphics Failed.", "Reason:Get XML failed.");
		IVS_DELETE(pGraphicsArr, MUILI);
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_ALLOC_MEMORY_ERROR, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}
	IVS_DELETE(pGraphicsArr, MUILI);
	strResult = xmlGraphics.GetXMLStream(uiLen);
	BP_RUN_LOG_INF("IVS OCX GetGraphics End.", "Result = %d", iRet);

	return strResult.AllocSysString();
}

// 获取选中的图形
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetGraphicSelected(ULONG ulWindowID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	CXml xmlGraphics;
	unsigned int uiLen = 0;

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Graphics Selected Failed", "Reason:m_pVideoPaneMgrr is NULL.");
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_OPERATE_MEMORY_ERROR, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Graphics Selected Failed.", "Reason:pVideoPane is NULL.");
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_OPERATE_MEMORY_ERROR, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}
	IA_GRAPHICS* pGraphic = (IA_GRAPHICS*)IVS_NEW((char*&)pGraphic,sizeof(IA_GRAPHICS) * GRAPHICS_NUM_MAX);//lint !e826
	//IA_GRAPHICS Graphic[GRAPHICS_NUM_MAX];
	memset(pGraphic, 0, sizeof(IA_GRAPHICS) * GRAPHICS_NUM_MAX);
	IVS_INT32 iRet = pVideoPane->GetGraphicSelected(pGraphic);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Graphics Selected Failed.","Reason: Get Graphics By Group failed.");
		IVS_DELETE(pGraphic, MUILI);
		COCXDrawXMLProcess::GetGraphicsGetErrXML(iRet, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}

	// 根据结构体返回值构建XML，赋值给strResult
	SIZE sz = {0};
	if (IVS_SUCCEED != COCXDrawXMLProcess::GetGraphicsGetXML(pGraphic, iRet, 1, xmlGraphics, sz))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS OCX Get Graphics Selected Failed.", "Reason:Get XML failed.");
		IVS_DELETE(pGraphic, MUILI);
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_ALLOC_MEMORY_ERROR, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}
	IVS_DELETE(pGraphic, MUILI);
	strResult = xmlGraphics.GetXMLStream(uiLen);
	BP_RUN_LOG_INF("IVS OCX Get Graphics Selected End.", "Result = %d", iRet);

	return strResult.AllocSysString();
}

// 按组获取智能分析图形
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetGraphicsByGroup(ULONG ulWindowID, ULONG ulGraphicGroup)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu,GraphicGroup: %lu", this,ulWindowID,ulGraphicGroup);

	CString strResult;

	CXml xmlGraphics;
	unsigned int uiLen = 0;

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Graphics By Group Failed", "Reason:m_pVideoPaneMgrr is NULL.");
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_OPERATE_MEMORY_ERROR, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Graphics By Group Failed.", "Reason:pVideoPane is NULL.");
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_OPERATE_MEMORY_ERROR, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}

	// 创建IAGraphic结构体数组，传入SDK获取值
	ULONG ulNum = GRAPHICS_NUM_MAX;
	IA_GRAPHICS* pGraphicsArr = (IA_GRAPHICS*)IVS_NEW((IVS_CHAR* &)pGraphicsArr, GRAPHICS_NUM_MAX * sizeof(IA_GRAPHICS));//lint !e826
	CHECK_POINTER(pGraphicsArr, NULL);
	memset(pGraphicsArr, 0, sizeof(IA_GRAPHICS) * GRAPHICS_NUM_MAX);
	IVS_INT32 iRet = pVideoPane->GetGraphicsByGroup(ulGraphicGroup, pGraphicsArr, &ulNum);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Graphics By Group Failed.","Reason: Get Graphics By Group failed.");
		IVS_DELETE(pGraphicsArr, MUILI);
		COCXDrawXMLProcess::GetGraphicsGetErrXML(iRet, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}

	// 根据结构体返回值构建XML，赋值给strResult
	SIZE sz = {0};
	if (IVS_SUCCEED != COCXDrawXMLProcess::GetGraphicsGetXML(pGraphicsArr, iRet, ulNum, xmlGraphics, sz))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS OCX Get Graphics By Group Failed.", "Reason:Get XML failed.");
		IVS_DELETE(pGraphicsArr, MUILI);
		COCXDrawXMLProcess::GetGraphicsGetErrXML(IVS_ALLOC_MEMORY_ERROR, xmlGraphics);
		strResult = xmlGraphics.GetXMLStream(uiLen);
		return strResult.AllocSysString();
	}
	IVS_DELETE(pGraphicsArr, MUILI);
	strResult = xmlGraphics.GetXMLStream(uiLen);
	BP_RUN_LOG_INF("IVS OCX Get Graphics By Group End.", "Result = %d", iRet);

	return strResult.AllocSysString();
}

// 智能分析开始画图
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DrawGraphics(ULONG ulWindowID, LPCTSTR pGraphicsXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pGraphicsXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu,GraphicsXml: %s", this,ulWindowID,pGraphicsXml);
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Draw Graphics Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Draw Graphics Failed","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	// 创建IAGraphic结构体数组，解析XML，赋值给结构体
	IA_GRAPHICS GraphicAttribute;
	memset(&GraphicAttribute, 0, sizeof(IA_GRAPHICS));
	ULONG ulGroupDrawMaxNum = 0;

	if (IVS_SUCCEED != COCXDrawXMLProcess::DrawGraphicsPraseXML(GraphicAttribute, ulGroupDrawMaxNum, pGraphicsXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Draw Graphics Failed.", "Reason:Parse Xml Failed.");
		return IVS_PARA_INVALID;
	}

	// 调用函数
	return pVideoPane->SetCurrentGraphicAttribute(&GraphicAttribute, ulGroupDrawMaxNum);
}

// 智能分析结束某图形组的画图
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopDrawGraphics(ULONG ulWindowID, ULONG ulGraphicGroup)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu,GraphicGroup: %lu", this,ulWindowID,ulGraphicGroup);

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Draw Graphics Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Draw Graphics Failed","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return pVideoPane->StopDrawGraphics(ulGraphicGroup);
}

// 删除选中图形
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteGraphicsSelected(ULONG ulWindowID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu", this,ulWindowID);

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Delete Selected Graphics Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Delete Selected Graphics Failed","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return pVideoPane->DeleteGraphicsSelected();
}

// 删除指定图形组的所有图形
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteGraphicsByGroup(ULONG ulWindowID, ULONG ulGraphicGroup)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu,GraphicGroup: %lu", this,ulWindowID,ulGraphicGroup);

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Delete Graphics By Group Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Delete Graphics By Group Failed.","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return pVideoPane->DeleteGraphicsByGroup(ulGraphicGroup);
}

// 删除所有图形
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteGraphics(ULONG ulWindowID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,ulWindowID: %lu", this,ulWindowID);

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Delete Graphics Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Delete Graphics Failed.","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return pVideoPane->DeleteGraphics();
}

// 选中指定图形组的所有图形，准备修改
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SelectGraphicByGroup(ULONG ulWindowID, ULONG ulGraphicGroup)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu,GraphicGroup: %lu", this,ulWindowID,ulGraphicGroup);
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Select Graphic By Group Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Select Graphic By Group Failed.","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return pVideoPane->SelectGraphicByGroup(ulGraphicGroup);
}

// 取消选中智能分析图形
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SelectCancel(ULONG ulWindowID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WindowID: %lu", this,ulWindowID);
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Select Cancel Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)ulWindowID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Select Cancel Failed.","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	return pVideoPane->SelectCancel();
}


void CIVS_OCXPlayerCtrl::PostLocRecEventGeneric(int code, int PanelID)
{
	CXml xml;
	if (IVS_SUCCEED != COCXRecordXMLProcess::LocalRecordBuidlXMLGeneric(code, PanelID, xml))
	{
		return;
	}
	unsigned int uiLength = 0;
	const TCHAR* pszXML = xml.GetXMLStream(uiLength);
	IVS_OCX_Windows_Event(IVS_EVENT_LOCAL_RECORD_EGENERIC, (LPCTSTR)pszXML);
}


ULONG CIVS_OCXPlayerCtrl::IVS_OCX_StartLocalRecord(LPCTSTR pRecordPara, LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);
	// TODO: Add your dispatch handler code here
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Start Local Record Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_PARA_INVALID;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((ULONG)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Start Local Record Failed.","Reason:pVideoPane is NULL.");
		return IVS_PARA_INVALID;
	}

	IVS_LOCAL_RECORD_PARAM RecordParam;
	memset(&RecordParam, 0, sizeof(RecordParam));
	if (NULL == pRecordPara)
	{
		std::string ParamString = m_pLocalConfig.GetCDATA2XML(LOCAL_CONFING_FILE_NAME, RECORD_CONFIG_ELEMENT);
		if (IVS_SUCCEED != COCXRecordXMLProcess::LocalRecordParseClientConfig(RecordParam, ParamString.c_str()))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Start Local Record Failed.", "Reason:ParseXml Failed.");
			return IVS_PARA_INVALID;
		}
	}
	else
	{
		if (IVS_SUCCEED != COCXRecordXMLProcess::LocalRecordParseXML(RecordParam, pRecordPara))
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Start Local Record Failed.", "Reason:ParseXml Failed");
			return IVS_PARA_INVALID;
		}
	}

	int iRet = IVS_SDK_StartLocalRecord(m_iSDKSessionID, &RecordParam, pVideoPane->GetPlayHandle(), NULL);
	
	if (IVS_SUCCEED == iRet)
	{
		// 标记是本窗格触发本地录像，流复用时判别是否需要停止录像
		pVideoPane->SetIsLocalRecordTrigger(true);
		
		// 同步流复用情况下按钮状态
		CGlobalPlayMgr &playMgr = CGlobalPlayMgr::Instance();
		playMgr.SyncButtonState(pVideoPane->GetPlayHandle(), BTN_LOCAL_RECORD, TRUE);
    }
	return (ULONG)iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopLocalRecord(LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);
	// TODO: Add your dispatch handler code here
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Local Record Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_PARA_INVALID;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((ULONG)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Loca lRecord Failed.","Reason:pVideoPane is NULL.");
		return IVS_PARA_INVALID;
	}

	int iRet = IVS_SDK_StopLocalRecord(m_iSDKSessionID, pVideoPane->GetPlayHandle());
	if (IVS_SUCCEED == iRet)
	{
		CGlobalPlayMgr &playMgr = CGlobalPlayMgr::Instance();
		playMgr.SyncButtonState(pVideoPane->GetPlayHandle(), BTN_LOCAL_RECORD, FALSE);
	}
    return (LONG)iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetTVWallList(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	CString strResult;

	COCXTVWallMgr::GetTVWallList(strResult);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddTVWall(LPCTSTR pTVWall)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	CString strResult;
	strResult = COCXTVWallMgr::AddTVWall(pTVWall);
	return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyTVWallInfo(LPCTSTR pTVWall)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pTVWall,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TVWall: %s", this,pTVWall);
	IVS_INT32 iRet = COCXTVWallMgr::ModifyTVWallInfo(pTVWall);
	return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyTVWallLayout(LPCTSTR pTVWall)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pTVWall,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TVWall: %s", this,pTVWall);
	IVS_INT32 iRet = COCXTVWallMgr::ModifyTVWallLayout(pTVWall);
	return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteTVWall(ULONG ulTVWallID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address: %x,TVWall: %lu", this,ulTVWallID);
	IVS_INT32 iRet = COCXTVWallMgr::DeleteTVWall(ulTVWallID);

	return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_PopupFloderDialog(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	CString strResult;

	BROWSEINFO stBrowseFinfo  = {0};
	LPITEMIDLIST lpltemIDList = SHBrowseForFolder(&stBrowseFinfo);

	if (NULL == lpltemIDList)
	{
		return strResult.AllocSysString();
	}

	SHGetPathFromIDList(lpltemIDList, strResult.GetBuffer(MAX_PATH));
	strResult.ReleaseBuffer();

	//释放资源
	IMalloc* pIMalloc = NULL;
	SHGetMalloc(&pIMalloc);
	if (NULL != pIMalloc)
	{
		pIMalloc->Release();
	}

	return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartRealPlayTVWall(ULONG ulTVWallID, LPCTSTR pCameraCode, LPCTSTR pRealplayParam, ULONG ulWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pRealplayParam,IVS_PARA_INVALID);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,CameraCode: %s,RealplayParam: %s,WndID: %lu", this,ulTVWallID,pCameraCode,pRealplayParam,ulWndID);
	(void)IVS_OCX_SetTVWallPaneStatus(ulTVWallID, ulWndID, TVWALL_STATUS_START);
	
    ULONG ulCurTVWallID = m_pVideoPaneMgr->GetCurTVWallID();

	//设置电视墙窗格类型为实况
    if (ulCurTVWallID == ulTVWallID)
    {
	     m_pVideoPaneMgr->SetTVWallWndType(WND_TYPE_REALTIME);
    }

	//ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
	TVWall_VPANEID vPaneID;
	if(!m_pVideoPaneMgr->GetVirtualIDByRealID(ulWndID,vPaneID))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start RealPlay TVWall Failed.", "Reason: WndID Not Exist.");
		return IVS_PARA_INVALID;
	}

	//找到窗格中的VideoPane并创建Bar
	CVideoPane *pVideoPane = GetVideoPanel((ULONG)ulWndID);
	ASSERT(NULL != pVideoPane);
	if (NULL == pVideoPane)	  //lint !e774
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start RealPlay TVWall Failed.", "Reason:Invalid WinID:%ld.", ulWndID);
		return IVS_PARA_INVALID;
	}

	//pVideoPane->CreateBar();

	IVS_INT32 iRet = COCXTVWallMgr::StartRealPlayTVWall(m_iSDKSessionID, ulTVWallID, pCameraCode, pRealplayParam, vPaneID);
	if(IVS_SUCCEED!=iRet)
	{
        if (ulCurTVWallID == ulTVWallID)
        {
		     (void)IVS_OCX_SetTVWallPaneStatus(ulTVWallID, ulWndID, TVWALL_STATUS_NONE);
             m_pVideoPaneMgr->SetTVWallWndType(WND_TYPE_NORMAL);
        }
		//pVideoPane->ResetPane();
		BP_RUN_LOG_ERR(iRet, "IVS OCX Start RealPlay TVWall Failed.", "Reason: OCX TVWallMgr Start RealPlay TVWall Failed.");
	}
	else
	{
        if (ulCurTVWallID == ulTVWallID)
        {
            (void)IVS_OCX_SetTVWallPaneStatus(ulTVWallID, ulWndID, TVWALL_STATUS_PLAY);

            //pVideoPane->SetPlayState(VIDEO_PLAY_STATE_RESTARTING);
            pVideoPane->SetTvWallBgState(true);
            //pVideoPane->Invalidate(TRUE);
        }
	}

	//删除已下墙的数据
	TVWall_OnWall_Map::iterator iter = m_mapOnWall.find(ulWndID);
	if(iter != m_mapOnWall.end())
	{
		OnWallPara *paraSrc = iter->second;
		if(paraSrc)
		{
			IVS_DELETE(paraSrc);			
		}
		m_mapOnWall.erase(ulWndID);
	}

	//插入上墙数据
	OnWallPara* para = NULL;
	(void)IVS_NEW(para);
	if(para)
	{
		para->strCamera = pCameraCode;
		para->strPara = pRealplayParam;
		(void)m_mapOnWall.insert(std::make_pair(ulWndID,para));
	}	
		
	return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopRealPlayTVWall(ULONG ulTVWallID, ULONG ulWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %lu", this,ulTVWallID,ulWndID);
	//ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
	TVWall_VPANEID vPaneID;
	if(!m_pVideoPaneMgr->GetVirtualIDByRealID(ulWndID,vPaneID))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop RealPlay TVWall Failed.", "Reason: WndID Not Exist.");
		return IVS_PARA_INVALID;
	}

    ULONG ulCurTVWallID = m_pVideoPaneMgr->GetCurTVWallID();

	IVS_INT32 iRet = COCXTVWallMgr::StopRealPlayTVWall(m_iSDKSessionID, ulTVWallID, vPaneID);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Stop RealPlay TVWall Failed.", "Reason: OCX TVWallMgr Stop RealPlay TVWall Failed.");
	}
	else
	{
        if (ulCurTVWallID == ulTVWallID)
        {
		    (void)IVS_OCX_SetTVWallPaneStatus(ulTVWallID, ulWndID, TVWALL_STATUS_NONE);

            //找到窗格中的VideoPane
            CVideoPane *pVideoPane = GetVideoPanel((ULONG)ulWndID);
            if (NULL != pVideoPane)	  //lint !e774
            {
                pVideoPane->SetTvWallBgState(false);
                pVideoPane->SetResumeState(false);
                pVideoPane->Invalidate(TRUE);
            }
        }
	}

	//删除已下墙的数据
	TVWall_OnWall_Map::iterator iter = m_mapOnWall.find(ulWndID);
	if(iter != m_mapOnWall.end())
	{
		OnWallPara *paraSrc = iter->second;
		if(paraSrc)
		{
			IVS_DELETE(paraSrc);			
		}
		m_mapOnWall.erase(ulWndID);
	}

    if (IVS_SDK_NOT_PLAY_TVWALL == iRet)
    {
        iRet = IVS_SUCCEED;
    }
	
	return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartPlatformPlayBackTVWall(ULONG ulTVWallID, LPCTSTR pCameraCode, LPCTSTR pPlaybackParam, ULONG ulWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam,IVS_PARA_INVALID);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
    IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,CameraCode: %s,PlaybackParam: %s,WndID: %lu", this,ulTVWallID,pCameraCode,pPlaybackParam,ulWndID);

	//设置电视墙窗格类型为实况
	m_pVideoPaneMgr->SetTVWallWndType(WND_TYPE_RECORD);

	//ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
	TVWall_VPANEID vPaneID;
	if(!m_pVideoPaneMgr->GetVirtualIDByRealID(ulWndID,vPaneID))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start PlatformPlayBack TVWall Failed.", "Reason: WndID Not Exist.");
		return IVS_PARA_INVALID;
	}

	//找到窗格中的VideoPane并创建Bar
	CVideoPane *pVideoPane = GetVideoPanel((ULONG)ulWndID);
	ASSERT(NULL != pVideoPane);
	if (NULL == pVideoPane)	  //lint !e774
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start PlatformPlayBack TVWall Failed.", "Reason:Invalid WinID:%ld.", ulWndID);
		return IVS_PARA_INVALID;
	}

	//pVideoPane->CreateBar();

    IVS_INT32 iRet = COCXTVWallMgr::StartPlatformPlayBackTVWall(m_iSDKSessionID, ulTVWallID, pCameraCode, pPlaybackParam, vPaneID);
    if(IVS_SUCCEED!=iRet)
    {
        BP_RUN_LOG_ERR(iRet, "IVS OCX Start PlatformPlayBack TVWall Failed.", "Reason:OCXTVWallMgr Start Platform PlayBack TVWall Failed.");
		//pVideoPane->ResetPane();
        m_pVideoPaneMgr->SetTVWallWndType(WND_TYPE_NORMAL);
    }
    else
    {
        pVideoPane->SetTvWallBgState(true);
        pVideoPane->Invalidate(TRUE);
    }

	//删除已下墙的数据
	TVWall_OnWall_Map::iterator iter = m_mapOnWall.find(ulWndID);
	if(iter != m_mapOnWall.end())
	{
		OnWallPara *paraSrc = iter->second;
		if(paraSrc)
		{
			IVS_DELETE(paraSrc);			
		}
		m_mapOnWall.erase(ulWndID);
	}

	//新增现有的参数
	OnWallPara* para = NULL;
	(void)IVS_NEW(para);
	if(para)
	{
		para->strCamera = pCameraCode;
		para->strPara = pPlaybackParam;
		(void)m_mapOnWall.insert(std::make_pair(ulWndID,para));
	}

    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopPlatformPlayBackTVWall(ULONG ulTVWallID, ULONG ulWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %lu", this,ulTVWallID,ulWndID);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
	//ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
	TVWall_VPANEID vPaneID;
	memset(&vPaneID,0,sizeof(vPaneID));
	if(!m_pVideoPaneMgr->GetVirtualIDByRealID(ulWndID,vPaneID))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop PlatformPlayBack TVWall Failed.", "Reason: WndID Not Exist.");
	}

    IVS_INT32 iRet = COCXTVWallMgr::StopPlatformPlayBackTVWall(m_iSDKSessionID, ulTVWallID, vPaneID);
    if(IVS_SUCCEED!=iRet)
    {
        BP_RUN_LOG_ERR(iRet, "IVS OCX Stop PlatformPlayBack TVWall Failed.", "Reason:OCXTVWallMgr Stop Platform PlayBack TVWall Failed.");
    }
    else
    {
        //找到窗格中的VideoPane
        CVideoPane *pVideoPane = GetVideoPanel((ULONG)ulWndID);
        if (NULL != pVideoPane)	  //lint !e774
        {
            //pVideoPane->SetPlayState(VIDEO_PLAY_STATE_RESTARTING);
            pVideoPane->SetTvWallBgState(false);
            pVideoPane->Invalidate(TRUE);
        }
    }

	//删除已下墙的数据
	TVWall_OnWall_Map::iterator iter = m_mapOnWall.find(ulWndID);
	if(iter != m_mapOnWall.end())
	{
		OnWallPara *paraSrc = iter->second;
		if(paraSrc)
		{
			IVS_DELETE(paraSrc);			
		}
		m_mapOnWall.erase(ulWndID);
	}

    if (IVS_SDK_NOT_PLAY_TVWALL == iRet)
    {
        iRet = IVS_SUCCEED;
    }

    return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDeviceList(ULONG ulDeviceType, ULONG ulFromIndex, ULONG ulToIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,DeviceType: %lu,ulFromIndex: %lu,ulToIndex: %lu", this,ulDeviceType,ulFromIndex,ulToIndex);

	CString strResult;

	// TODO: Add your dispatch handler code here
	CXml xmlRsp;
	IVS_INT32 iRet = COCXDeviceMgr::GetDeviceList(m_iSDKSessionID, ulDeviceType, ulFromIndex,ulToIndex, xmlRsp);
	if (IVS_SUCCEED == iRet)
	{
		IVS_UINT32 uiLen = 0;
		strResult = xmlRsp.GetXMLStream(uiLen);
	}
	else
	{
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Device List Failed.","Reason:DeviceMgr Get Device List Failed.");
		COCXXmlProcess::GetErrString(strResult, iRet);
	}
	return strResult.AllocSysString();
}

// 获取拥有指定设备权限的用户列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDeviceUserList(LPCTSTR pDomainCode, LPCTSTR pDevCode, ULONG ulIsUserOnline, ULONG ulFromIndex, ULONG ulToIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pDevCode,NULL);
	IVS_INFO_TRACE("OCX Address: %x,DomainCode: %s, DevCode: %s,FromIndex: %ld,ToIndex: %ld", this,pDomainCode, pDevCode,ulFromIndex,ulToIndex);

	CString strResult;

	COCXAlarmMgr::GetDeviceUserList(m_iSDKSessionID, pDomainCode, (IVS_CHAR*)pDevCode, ulIsUserOnline, ulFromIndex, ulToIndex, strResult);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetTVWallByID(ULONG ulTVWallID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address: %x,TVWallID: %lu", this,ulTVWallID);
	CString strResult;
	COCXTVWallMgr::GetTVWallByID(ulTVWallID, strResult);
	return strResult.AllocSysString();
}

// 添加临时场景
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetTempScenario(LPCTSTR pScenarioXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pScenarioXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ScenarioXml: %s", this,pScenarioXml);
	std::string strScenarioInfo;
	IVS_INT32 iResultCode = CFavorite::GetScenarioFavorite(TEMP_SCENARIO_INFO_ID, strScenarioInfo);
	// 如果场景已经存在
	if (iResultCode == IVS_SUCCEED)
	{
		(void)CFavorite::DelScenarioFavorite(TEMP_SCENARIO_INFO_ID);
	}
	std::string strTempScenarioInfoID = TEMP_SCENARIO_INFO_ID;
	return CFavorite::AddScenarioFavorite(pScenarioXml, strTempScenarioInfoID);
}

// 获取临时场景
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetTempScenario(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	std::string str = CFavorite::GetTempScenario(TEMP_SCENARIO_INFO_ID);
	BP_RUN_LOG_INF("IVS OCX Get TempScenario End.","Result: %s", str.c_str());

	return _com_util::ConvertStringToBSTR(str.c_str());
}

// 增加场景
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddScenarioFavorite(LPCTSTR pScenarioXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pScenarioXml,NULL);
	IVS_INFO_TRACE("OCX Address: %x,ScenarioXml: %s", this,pScenarioXml);

	std::string strScenarioID = CFavorite::NewGUID();
	IVS_INT32 iResultCode = CFavorite::AddScenarioFavorite(pScenarioXml, strScenarioID);
	std::string str;
	CFavorite::BulidAddScenarioInfoResult(iResultCode, strScenarioID, str);
	BP_RUN_LOG_INF("IVS OCX Add Scenario Favorite.","Result:%s", str.c_str());
	return _com_util::ConvertStringToBSTR(str.c_str());
}

// 修改场景
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModScenarioFavorite(LPCTSTR pScenarioXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pScenarioXml,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ScenarioXml: %s", this,pScenarioXml);
	return CFavorite::ModScenarioFavorite(pScenarioXml);
}

// 删除场景
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DelScenarioFavorite(LPCTSTR pScenarioID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pScenarioID,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ScenarioID: %s", this,pScenarioID);
	return CFavorite::DelScenarioFavorite(pScenarioID);
}

// 获取场景列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetScenarioFavoriteList(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	std::string strScenarioFavoriteList;
	IVS_INT32 iResultCode = CFavorite::GetScenarioFavoriteList(strScenarioFavoriteList);
	std::string str;
	CFavorite::BulidGetScenarioInfoListResult(iResultCode, strScenarioFavoriteList, str);
	//BP_RUN_LOG_INF("get config","%s", str.c_str()); //log

	return _com_util::ConvertStringToBSTR(str.c_str());
}

// 获取场景详情
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetScenarioFavorite(LPCTSTR pScenarioID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pScenarioID,NULL);
	IVS_INFO_TRACE("OCX Address: %x,ScenarioID: %s", this,pScenarioID);
	std::string strScenarioInfo;
	IVS_INT32 iResultCode = CFavorite::GetScenarioFavorite(pScenarioID, strScenarioInfo);
	std::string str;
	CFavorite::BulidGetScenarioInfoResult(iResultCode, strScenarioInfo, str);
	BP_RUN_LOG_INF("IVS OCX Get Scenario Favorite End.","Result:%s", str.c_str());
	return _com_util::ConvertStringToBSTR(str.c_str());
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_PlaySound(LONG lWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);

    //CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);
    //if ( !m_pVideoPaneMgr->IsLegalityWndID(lWndID))
    //{
    //    return IVS_PARA_INVALID;
    //}

    //CVideoPane * pWnd = nullptr;
    ////入参窗格ID为0表示没有指定窗格，默认使用当前活动窗格
    //pWnd = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(static_cast<ULONG>(lWndID));
    //CHECK_POINTER(pWnd,IVS_OCX_RET_GET_WND_POINTER_ERROR);

    //if (VIDEO_PLAY_STATE_NOT_PLAYING == pWnd->GetPlayState())
    //{
    //    BP_RUN_LOG_ERR(IVS_OCX_RET_VIDEOPANE_ISNOT_PLAYING_ERROR,"IVS_OCX::%s.error id = %d",__FUNCDNAME__,IVS_OCX_RET_VIDEOPANE_ISNOT_PLAYING_ERROR);
    //    return IVS_OCX_RET_VIDEOPANE_ISNOT_PLAYING_ERROR;
    //}
    //HWND hWnd = pWnd->GetVideoWnd();

    //if(m_bVoiceTalkbacking)
    //{
    //    BP_RUN_LOG_ERR(IVS_OCX_RET_VOICE_TALKBACK_HAS_PLAYED_ERROR,"IVS_OCX::%s.error id = %d",__FUNCDNAME__,IVS_OCX_RET_VOICE_TALKBACK_HAS_PLAYED_ERROR);
    //    return  IVS_OCX_RET_VOICE_TALKBACK_HAS_PLAYED_ERROR;
    //}
    //ULONG ulRet = StopSound(hWnd);
    //if (IVS_SUCCEED != ulRet)
    //{
    //    BP_RUN_LOG_ERR(ulRet,"IVS_OCX::%s.error id = %d",__FUNCDNAME__,ulRet);
    //    return ulRet;
    //}
    //pWnd->IsSoundOn(true);

    //// ulRet = IVS_OCX_SetWaveAudio(pDstCode, PLAYER_AUDIO_MAX_VOLUM);
    //if (IVS_SUCCEED != ulRet)
    //{
    //    BP_RUN_LOG_ERR(ulRet,"IVS_OCX::%s.error id = %d",__FUNCDNAME__,ulRet);
    //    return ulRet;
    //}

    //// 分实况和录像
    //if(WND_TYPE_REALTIME == ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndType())
    //{
    //    //ulRet = OCX_SetSoundState(hWnd, TRUE);
    //    if (IVS_SUCCEED != ulRet)
    //    {
    //        BP_RUN_LOG_ERR(ulRet,"IVS_OCX::%s.error id = %d",__FUNCDNAME__,ulRet);
    //        return ulRet;
    //    }
    //} else if(WND_TYPE_RECORD == ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndType()) {
    //    if(IVS_SUCCEED != m_pVideoPaneMgr->SetSoundState(static_cast<ULONG>(lWndID), TRUE))
    //    {
    //        BP_RUN_LOG_ERR(ulRet,"IVS_OCX::%s.error id = %d",__FUNCDNAME__,ulRet);
    //        return IVS_XML_INVALID;
    //    }
    //    pWnd->SetLastSoundState(TRUE);
    //}
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Play Sound Failed.", "Reason: video pane manager is null");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Play Sound Failed.", "Reason: pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CTranVideoRealtimeBar *pRealtimeBar = pVideoPane->GetVideoRealtimeBar();
	if (NULL == pRealtimeBar)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Play Sound Failed.", "Reason: pRealtimeBar is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	pRealtimeBar->StartSound();

    return IVS_SUCCEED;
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_StopSound(LONG lWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);

    //CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);
    //if ( !m_pVideoPaneMgr->IsLegalityWndID(lWndID))
    //{
    //    return IVS_PARA_INVALID;
    //}

    //CVideoPane * pVideoPane = nullptr;
    //pVideoPane = m_pVideoPaneMgr->GetWndPointerByID(static_cast<ULONG>(lWndID));
    //CHECK_POINTER(pVideoPane,IVS_OCX_RET_GET_WND_POINTER_ERROR);

    //// 分实况和录像
    //if(WND_TYPE_REALTIME == ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndType())
    //{
    //    if (pVideoPane->GetRealtimeSoundState() != BOTTOMBAR_SOUND_OPEN)
    //    {
    //        ULONG ulRet = ((CVideoPaneMgr*)m_pVideoPaneMgr)->StopAllSound(WND_TYPE_REALTIME);    //解决窗格交换导致窗格号不正确停止失败问题
    //        if (IVS_SUCCEED != ulRet)
    //        {
    //            BP_RUN_LOG_ERR(ulRet,"","IVS_OCX::%s.error code = %d",__FUNCDNAME__,ulRet);
    //            return ulRet;
    //        }
    //        return IVS_SUCCEED;
    //    }
    //} else if(WND_TYPE_RECORD == ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndType()) {
    //    if (pVideoPane->GetRecordSoundState() != BOTTOMBAR_SOUND_OPEN)
    //    {
    //        ULONG ulRet = ((CVideoPaneMgr*)m_pVideoPaneMgr)->StopAllSound(WND_TYPE_RECORD);    //解决窗格交换导致窗格号不正确问题
    //        if (IVS_SUCCEED != ulRet)
    //        {
    //             BP_RUN_LOG_ERR(ulRet,"","IVS_OCX::%s.error code = %d",__FUNCDNAME__,ulRet);
    //            return ulRet;
    //        }
    //        return IVS_SUCCEED;
    //    }
    //}

    //ULONG ulRet = StopSound(static_cast<ULONG>(lWndID));
    //if (IVS_SUCCEED != ulRet)
    //{
    //    BP_RUN_LOG_ERR(ulRet,"","IVS_OCX::%s.error code = %d",__FUNCDNAME__,ulRet);
    //    return ulRet;
    //}

    //// 分实况和录像
    //if(WND_TYPE_REALTIME == ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndType())
    //{
    //    (void)OCX_SetSoundState(pVideoPane->GetVideoWnd(),FALSE);
    //}
    //else if(WND_TYPE_RECORD == ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndType())
    //{
    //    (void)m_pVideoPaneMgr->SetSoundState(static_cast<ULONG>(lWndID), FALSE);
    //    pVideoPane->SetLastSoundState(FALSE);
    //}

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Sound Failed.", "Reason: video pane manager is null");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Sound Failed.", "Reason: pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CTranVideoRealtimeBar *pRealtimeBar = pVideoPane->GetVideoRealtimeBar();
	if (NULL == pRealtimeBar)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Sound Failed.", "Reason: pRealtimeBar is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	pRealtimeBar->StopSound();

	/*
	 * DTS2014021302165 
	 * 去除panel的全局注册
	 */
	CGlobalSoundMgr &SoundMgr = CGlobalSoundMgr::Instance();
	if (SoundMgr.GetSoundingPanel() == pVideoPane)
	{
		SoundMgr.Reset();
	}

    return IVS_SUCCEED;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetVolume(LONG lWndID, ULONG ulVolumeValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld, VolumeValue: %lu", this,lWndID, ulVolumeValue);

	if (100 < ulVolumeValue)
	{
		return IVS_PARA_INVALID;
	}
	// TODO: Add your dispatch handler code here
	ulVolumeValue = (ULONG)(((float)0xFFFF / 100) * (float)ulVolumeValue);
	ulVolumeValue = (ulVolumeValue << 16) + ulVolumeValue;

	return SetVolume((ULONG)lWndID, SOUNDCTRL_WND_TYPE_SOUND, ulVolumeValue);
}


ULONG CIVS_OCXPlayerCtrl::IVS_OCX_GetVolume(LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);

	// TODO: Add your dispatch handler code here
	unsigned int uiVolumeValue = 0;
	LONG lRet = GetVolume((ULONG)lWndID, SOUNDCTRL_WND_TYPE_SOUND, &uiVolumeValue);
	if (IVS_SUCCEED == lRet)
	{
		uiVolumeValue = (uiVolumeValue >> 16) * 100 / 0xFFFF;
	} else {
		uiVolumeValue = 0;
	}
	return (ULONG)uiVolumeValue;
}


ULONG CIVS_OCXPlayerCtrl::IVS_OCX_Set3DPTZ(ULONG ulWndID, ULONG ulIsStart3D)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,WndID: %lu,IsStart3D: %lu", this,ulWndID,ulIsStart3D);
    CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);
    return m_pVideoPaneMgr->Set3DPTZState(ulWndID,ulIsStart3D);
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetMainDeviceName(LPCTSTR pDevCode, LPCTSTR pNewDevName)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pDevCode,IVS_PARA_INVALID);
	CHECK_POINTER(pNewDevName,IVS_PARA_INVALID);
    IVS_INFO_TRACE("OCX Address: %x,pDevCode: %s,pNewDevName: %s", this,pDevCode,pNewDevName);

    LONG lRet = COCXDeviceMgr::SetMainDeviceName(m_iSDKSessionID,pDevCode,pNewDevName);

    return lRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetCameraName(LPCTSTR pCameraCode, LPCTSTR pNewCameraName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pNewCameraName,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,NewCameraName: %s", this,pCameraCode,pNewCameraName);

	LONG lRet = COCXDeviceMgr::SetCameraName(m_iSDKSessionID,pCameraCode,pNewCameraName);
	return lRet;
}

//查询组轮巡
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetTVWallGroupTurnInspect(ULONG ulTVWallID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult="";
	IVS_INFO_TRACE("OCX Address: %x,TVWallID: %lu", this,ulTVWallID);

	// TODO: Add your dispatch handler code here

	return strResult.AllocSysString();
}

//新增组轮巡
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddTVWallGroupTurnInspect(LPCTSTR pTVWall)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pTVWall,NULL);
	IVS_INFO_TRACE("OCX Address: %x,TVWall: %s", this,pTVWall);
	CString strResult = "";

	// TODO: Add your dispatch handler code here

	return strResult.AllocSysString();
}

//编辑组轮巡
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyTVWallGroupTurnInspect(LPCTSTR pTVWall)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(pTVWall,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TVWall: %s", this,pTVWall);
	return 0;
}

//删除组轮巡
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteTVWallGroupTurnInspect(ULONG ulTVWallID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	IVS_INFO_TRACE("OCX Address: %x,TVWallID: %lu", this,ulTVWallID);
	return 0;
}

//查询轮巡
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetTVWallTurnInspect(ULONG ulTVWallID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult ="";

	// TODO: Add your dispatch handler code here
	IVS_INFO_TRACE("OCX Address: %x,TVWallID: %lu", this,ulTVWallID);
	return strResult.AllocSysString();
}

//新增轮巡
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AddTVWallTurnInspect(LPCTSTR pTVWall)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(pTVWall,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TVWall: %s", this,pTVWall);
	return 0;
}

//编辑轮巡
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyTVWallTurnInspect(LPCTSTR pTVWall)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(pTVWall,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TVWall: %s", this,pTVWall);
	return 0;
}

//删除轮巡
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteTVWallTurnInspect(ULONG ulTVWallID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	IVS_INFO_TRACE("OCX Address: %x,TVWallID: %lu", this,ulTVWallID);
	return 0;
}

//设置轮巡计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetTurnInspectPlan(LPCTSTR pTurnInspectPlanInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(pTurnInspectPlanInfo,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TurnInspectPlanInfo: %s", this,pTurnInspectPlanInfo);
	return 0;
}

//修改轮巡计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyTurnInspectPlan(LPCTSTR pTurnInspectPlanInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(pTurnInspectPlanInfo,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TurnInspectPlanInfo: %s", this,pTurnInspectPlanInfo);
	return 0;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_RebootDevice(LPCTSTR pDeviceCode, ULONG ulRebootType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pDeviceCode,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,DeviceCode: %s", this,pDeviceCode);

	IVS_INT32 iRet = COCXDeviceMgr::RestartDevice(m_iSDKSessionID, pDeviceCode, static_cast<IVS_UINT32>(ulRebootType));

	return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetConfigInfo(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Config Info Failed.", "Reason:pReqXml is NULL.");
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);

	COCXAlarmMgr::GetConfigInfo(m_iSDKSessionID, (IVS_CHAR*)pReqXml, strResult);
	return strResult.AllocSysString();
}

//添加书签
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddBookmark(LPCTSTR pNVRCode,LPCTSTR pCameraCode,LPCTSTR pBookmarkName,LPCTSTR pBookmarkTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pNVRCode,NULL);
	CHECK_POINTER(pCameraCode,NULL);
	//CHECK_POINTER(pBookmarkName,NULL);//和SE确认，书签名称可以为空
	CHECK_POINTER(pBookmarkTime,NULL);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,BookmarkName: %s,BookmarkTime: %s", this,pCameraCode,pBookmarkName,pBookmarkTime);
	CString strResult = "";
	IVS_UINT32 uiXmlLen = 0;
	IVS_INT32 iRet = IVS_FAIL;
	IVS_UINT32 uiBookmarkID = 0;
	CXml xmlRsp;

	iRet = IVS_SDK_AddBookmark(m_iSDKSessionID,pCameraCode,pNVRCode,pBookmarkName,pBookmarkTime,&uiBookmarkID);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"IVS OCX Add Bookmark List Failed", "Reason:IVS SDK Add Bookmark Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	
	//拼装响应消息结构体为XML
	iRet = COCXRecordXMLProcess::AddBookmarkParseXML(uiBookmarkID,iRet,xmlRsp);
	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult.AllocSysString();
}

//查询书签列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetBookmarkList(LPCTSTR pQueryXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pQueryXml,NULL);
	IVS_INFO_TRACE("OCX Address: %x,QueryXml: %s", this,pQueryXml);
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlReq;
	CXml xmlRsp;
	IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	if (NULL == xmlReq.Parse(pQueryXml))
	{
		iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Bookmark List Failed", "Reason:Parse XML Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	IVS_INT32 iFieldNum = 0;
	iFieldNum = COCXXmlProcess::GetFieldNum(xmlReq);

	//分配内存
	IVS_VOID* pQueryFiledBuff = NULL;
	iRet = COCXXmlProcess::GetUnifiedFormatQueryParseXML(pQueryFiledBuff,xmlReq,iFieldNum);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Bookmark List Failed.", "Reason:Parse XML Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	IVS_QUERY_UNIFIED_FORMAT* pQueryUnifiedFormat = (IVS_QUERY_UNIFIED_FORMAT*)pQueryFiledBuff;

	iRet = COCXRecordXMLProcess::GetQueryUnifiedCameraCode(xmlReq,cCameraCode);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Bookmark List Failed", "Reason:Get QueryUnified CameraCode Xml Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	IVS_UINT32 uiTotal = (pQueryUnifiedFormat->stIndex).uiToIndex - (pQueryUnifiedFormat->stIndex).uiFromIndex;
	IVS_UINT32 uiBufSize = sizeof(IVS_RECORD_BOOKMARK_INFO_LIST)+sizeof(IVS_RECORD_BOOKMARK_INFO)*uiTotal;
	IVS_CHAR* pRecordListBuf = NULL;
	try{
		pRecordListBuf = new IVS_CHAR[uiBufSize];
	}
	catch(...)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("IVS OCX Get Bookmark List", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult.AllocSysString();
	}
	memset(pRecordListBuf, 0, uiBufSize);

	IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkInfoList = (IVS_RECORD_BOOKMARK_INFO_LIST*)pRecordListBuf;//lint !e826
	iRet = IVS_SDK_GetBookmarkList(m_iSDKSessionID,pQueryUnifiedFormat,pBookmarkInfoList,uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		IVS_DELETE(pRecordListBuf, MUILI);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Bookmark List Failed.", "Reason:SDK Get Bookmark List Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	//拼装响应消息结构体为XML
	iRet = COCXRecordXMLProcess::GetRecordBookmarkListGetXML(cCameraCode,pBookmarkInfoList,iRet,xmlRsp);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		IVS_DELETE(pRecordListBuf, MUILI);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Bookmark List Failed.", "Reason:Parse pBookmarkInfoList To Xml Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	IVS_DELETE(pQueryFiledBuff, MUILI);
	IVS_DELETE(pRecordListBuf, MUILI);

	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult.AllocSysString();
}

//修改书签
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyBookmark(LPCTSTR pNVRCode,LPCTSTR pCameraCode,ULONG ulBookmarkID,LPCTSTR pNewBookmarkName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pNVRCode,IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	//CHECK_POINTER(pNewBookmarkName,IVS_PARA_INVALID);//和SE确认，书签名称可以为空
	IVS_INFO_TRACE("OCX Address: %x,pNVRCode: %s,CameraCode: %s,BookmarkID: %lu,NewBookmarkName: %s", this,pNVRCode,pCameraCode,ulBookmarkID,pNewBookmarkName);
	IVS_INT32 iRet = IVS_FAIL;
	iRet = IVS_SDK_ModifyBookmark(m_iSDKSessionID,pNVRCode,pCameraCode,ulBookmarkID,pNewBookmarkName);
	return iRet;
}

//删除书签
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteBookmark(LPCTSTR pNVRCode,LPCTSTR pCameraCode,ULONG ulBookmarkID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pNVRCode,IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pNVRCode: %s,CameraCode: %s,BookmarkID: %lu", this,pNVRCode,pCameraCode,ulBookmarkID);
	IVS_INT32 iRet = IVS_FAIL;
	iRet = IVS_SDK_DeleteBookmark(m_iSDKSessionID,pNVRCode,pCameraCode,ulBookmarkID);
	return iRet;
}

// 增加预置位
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddPTZPreset(LPCTSTR pCameraCode, LPCTSTR pPresetName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NULL == pCameraCode || NULL == pPresetName)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "input data is null","NA");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,PresetName: %s", this, pCameraCode, pPresetName);
	CString strResult;

 	IVS_UINT32  uiPresetIndex = 0;
 	IVS_INT32 iRet = IVS_SDK_AddPTZPreset(m_iSDKSessionID, pCameraCode, pPresetName, &uiPresetIndex);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk add ptz preset failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

	std::string strCameraCode = pCameraCode;
	iRet = COCXPtzControlXMLProcess::AddPTZPresetGetXML(strCameraCode, uiPresetIndex, iRet, strResult);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk get guardpos failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 删除预置位
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DelPTZPreset(LPCTSTR pCameraCode, ULONG ulPresetIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ulPresetIndex:%ul", this, ulPresetIndex);
	return IVS_SDK_DelPTZPreset(m_iSDKSessionID, pCameraCode, ulPresetIndex);
}

// 修改预置位
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModPTZPreset(LPCTSTR pCameraCode, ULONG ulPresetIndex, LPCTSTR pPresetName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pPresetName,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,ulPresetIndex:%ul,PresetName: %s", this, pCameraCode, ulPresetIndex, pPresetName);
	
	// 构造输入参数
	IVS_PTZ_PRESET stPTZPreset;
	memset(&stPTZPreset, 0, sizeof(IVS_PTZ_PRESET));
	stPTZPreset.uiPresetIndex = ulPresetIndex;
	if(!CToolsHelp::Memcpy(stPTZPreset.cPresetName, IVS_PRESET_NAME_LEN, pPresetName, strlen(pPresetName)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "mod ptz preset","Copy PresetName failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	return IVS_SDK_ModPTZPreset(m_iSDKSessionID, pCameraCode, &stPTZPreset);
}

// 查询预置位列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetPTZPresetList(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,NULL);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s", this, pCameraCode);
	CString strResult;
	// 构造输入参数
	IVS_UINT32 uiBufferSize = sizeof(IVS_PTZ_PRESET) * IVS_MAX_PRESET_NUM;
	IVS_PTZ_PRESET* pPTZPresetList = (IVS_PTZ_PRESET*)IVS_NEW((IVS_CHAR* &)pPTZPresetList, uiBufferSize); //lint !e826 无问题
	memset(pPTZPresetList, 0, uiBufferSize);
	// 输出参数
	IVS_UINT32 uiPTZPresetNum = 0;
	IVS_INT32 iRet = IVS_SDK_GetPTZPresetList(m_iSDKSessionID, pCameraCode, pPTZPresetList, uiBufferSize, &uiPTZPresetNum);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk get ptz preset list failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

	std::string strCameraCode = pCameraCode;
	iRet = COCXPtzControlXMLProcess::GetPTZPresetListGetXML(strCameraCode, pPTZPresetList, uiPTZPresetNum, iRet, strResult);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk get guardpos failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 设置看守位
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetGuardPos(LPCTSTR pGuardPosInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pGuardPosInfo,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pGuardPosInfo: %s", this, pGuardPosInfo);
	// 输入参数
	std::string strGuardPosInfo = pGuardPosInfo;
	// 输出参数
	std::string strCameraCode;
	IVS_GUARD_POS_INFO stGuardPosInfo;
	memset(&stGuardPosInfo, 0, sizeof(IVS_GUARD_POS_INFO));
	IVS_INT32 iRet = COCXPtzControlXMLProcess::SetGuardPosParseXML(strGuardPosInfo, strCameraCode, &stGuardPosInfo);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "set guard pos parse xml failed", "iRet = %d", iRet);
		return iRet;
	}
	iRet = IVS_SDK_SetGuardPos(m_iSDKSessionID, strCameraCode.c_str(), &stGuardPosInfo);
	return iRet;
}

// 查询看守位信息
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetGuardPos(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,NULL);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s", this, pCameraCode);
	CString strResult;

 	IVS_GUARD_POS_INFO stGuardPosInfo;
	memset(&stGuardPosInfo, 0, sizeof(IVS_GUARD_POS_INFO));
 	IVS_INT32 iRet = IVS_SDK_GetGuardPos(m_iSDKSessionID, pCameraCode, &stGuardPosInfo);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk get guardpos failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	std::string strCameraCode = pCameraCode;
	iRet = COCXPtzControlXMLProcess::GetGuardPosGetXML(strCameraCode, &stGuardPosInfo, iRet, strResult);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk get guardpos failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 增加巡航轨迹信息
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddCruiseTrack(LPCTSTR pCuriseTrackInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCuriseTrackInfo,NULL);
	IVS_INFO_TRACE("OCX Address: %x,CuriseTrackInfo: %s", this, pCuriseTrackInfo);
	CString strResult;
	// 输入参数
	std::string strCuriseTrackInfo =  pCuriseTrackInfo;
	// 输出参数
	std::string strCameraCode;
	IVS_CRUISE_TRACK stCuriseTrackInfo;
	memset(&stCuriseTrackInfo, 0, sizeof(IVS_CRUISE_TRACK));
	
	IVS_INT32 iRet = COCXPtzControlXMLProcess::AddCruiseTrackParseXML(strCuriseTrackInfo, strCameraCode, &stCuriseTrackInfo);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "set add cruise track parse xml failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	IVS_UINT32 uiTrackIndex = 0;
	iRet = IVS_SDK_AddCruiseTrack(m_iSDKSessionID, strCameraCode.c_str(), &stCuriseTrackInfo, &uiTrackIndex);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk add cruise track failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

	iRet = COCXPtzControlXMLProcess::AddCruiseTrackGetXML(uiTrackIndex, iRet, strResult);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk add CruiseTrack failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 删除巡航轨迹信息
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DelCruiseTrack(LPCTSTR pCameraCode, ULONG ulTrackIndex, ULONG ulCruiseType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,NULL);
	IVS_INFO_TRACE("OCX Address: %x,ulTrackIndex: %ul", this, ulTrackIndex);
	return IVS_SDK_DelCruiseTrack(m_iSDKSessionID, pCameraCode, ulTrackIndex, ulCruiseType);
}

// 修改巡航轨迹信息
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModCruiseTrack(LPCTSTR pCuriseTrackInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCuriseTrackInfo,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,CuriseTrackInfo: %s", this, pCuriseTrackInfo);

	std::string strCameraCode;
	IVS_CRUISE_TRACK stCuriseTrackInfo;
	memset(&stCuriseTrackInfo, 0, sizeof(IVS_CRUISE_TRACK));
	std::string strCuriseTrackInfo = pCuriseTrackInfo;
	IVS_INT32 iRet = COCXPtzControlXMLProcess::ModCruiseTrackParseXML(strCuriseTrackInfo, strCameraCode, &stCuriseTrackInfo);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "set mod cruise track parse xml failed", "iRet = %d", iRet);
		return iRet;
	}
	iRet = IVS_SDK_ModCruiseTrack(m_iSDKSessionID, strCameraCode.c_str(), &stCuriseTrackInfo);
	return iRet;
}

// 查询巡航轨迹信息列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCruiseTrackList(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,NULL);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s", this, pCameraCode);
	CString strResult;
	IVS_UINT32 uiBufferSize = IVS_MAX_CURISE_TRACK_NUM * sizeof(IVS_CRUISE_TRACK);
	IVS_CRUISE_TRACK* pCruiseTrackList = (IVS_CRUISE_TRACK*)IVS_NEW((IVS_CHAR* &)pCruiseTrackList, uiBufferSize); //lint !e826 无问题
	memset(pCruiseTrackList, 0, uiBufferSize);
	IVS_UINT32 uiCruiseTrackNum = 0;
	IVS_INT32 iRet = IVS_SDK_GetCruiseTrackList(m_iSDKSessionID, pCameraCode, pCruiseTrackList, uiBufferSize, &uiCruiseTrackNum);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk get cruise track list failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

	std::string strCameraCode = pCameraCode;
	iRet = COCXPtzControlXMLProcess::GetCruiseTrackListGetXML(strCameraCode, pCruiseTrackList, uiCruiseTrackNum, iRet, strResult);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk get guardpos failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 查询巡航轨迹信息
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCruiseTrack(LPCTSTR pCameraCode, ULONG ulTrackIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,NULL);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s, ulTrackIndex:%ul", this, pCameraCode, ulTrackIndex);
	CString strResult;

	IVS_CRUISE_TRACK stCruiseTrackInfo;
	memset(&stCruiseTrackInfo, 0, sizeof(IVS_CRUISE_TRACK));
	IVS_INT32 iRet = IVS_SDK_GetCruiseTrack(m_iSDKSessionID, pCameraCode, (IVS_INT32)ulTrackIndex, &stCruiseTrackInfo);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk get cruise track failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	
	std::string strCameraCode = pCameraCode;
	iRet = COCXPtzControlXMLProcess::GetCruiseTrackGetXML(strCameraCode, &stCruiseTrackInfo, iRet, strResult);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk get guardpos failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 分配巡航计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetCruisePlan(LPCTSTR pCruisePlan)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCruisePlan,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pCruisePlan: %s", this, pCruisePlan);
	// 输入参数
	std::string strCruisePlan = pCruisePlan;
	// 输出参数
	std::string strCameraCode;
	IVS_CRUISE_PLAN* pstCruisePlan = (IVS_CRUISE_PLAN*)IVS_NEW((IVS_CHAR* &)pstCruisePlan, sizeof(IVS_CRUISE_PLAN));//lint !e826
	CHECK_POINTER(pstCruisePlan, IVS_ALLOC_MEMORY_ERROR);
	memset(pstCruisePlan, 0, sizeof(IVS_CRUISE_PLAN));
	IVS_INT32 iRet = COCXPtzControlXMLProcess::SetCruisePlanParseXML(strCruisePlan, strCameraCode, pstCruisePlan);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "set cruise plan parse xml failed", "iRet = %d", iRet);
		IVS_DELETE(pstCruisePlan, MUILI);
		return iRet;
	}
	iRet = IVS_SDK_SetCruisePlan(m_iSDKSessionID, strCameraCode.c_str(), pstCruisePlan);
	IVS_DELETE(pstCruisePlan, MUILI);
	return iRet;
}

// 查询巡航计划
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCruisePlan(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,NULL);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s", this, pCameraCode);
	CString strResult;
	IVS_CRUISE_PLAN* pstCruisePlan = (IVS_CRUISE_PLAN*)IVS_NEW((IVS_CHAR* &)pstCruisePlan, sizeof(IVS_CRUISE_PLAN));//lint !e826
	if (NULL == pstCruisePlan)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "malloc failed", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_ALLOC_MEMORY_ERROR);
		return strResult.AllocSysString();
	}
	memset(pstCruisePlan, 0, sizeof(IVS_CRUISE_PLAN));
	IVS_INT32 iRet = IVS_SDK_GetCruisePlan(m_iSDKSessionID, pCameraCode, pstCruisePlan);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk get cruise track failed", "NA");
		IVS_DELETE(pstCruisePlan, MUILI);
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

	std::string strCameraCode = pCameraCode;
	iRet = COCXPtzControlXMLProcess::GetCruisePlanGetXML(strCameraCode, pstCruisePlan, iRet, strResult);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "sdk get guardpos failed", "NA");
		IVS_DELETE(pstCruisePlan, MUILI);
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	IVS_DELETE(pstCruisePlan, MUILI);
	return strResult.AllocSysString();
}

// 添加轮巡资源
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddCameraSwitch(LPCTSTR pSwitchInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pSwitchInfo,NULL);
	IVS_INFO_TRACE("OCX Address: %x,pSwitchInfo: %s", this, pSwitchInfo);

	std::string strSwitchID = CFavorite::NewGUID();
	IVS_INT32 iResultCode = CFavorite::AddCameraSwitch(pSwitchInfo, strSwitchID);
	std::string str;
	CFavorite::BulidAddCameraSwitchResult(iResultCode, strSwitchID, str);
	BP_RUN_LOG_INF("IVS OCX Add Camera Switch End.","Result XML: %s", str.c_str());
	return _com_util::ConvertStringToBSTR(str.c_str());
}

// 删除轮巡资源
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DelCameraSwitch(LPCTSTR pCameraSwitchID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraSwitchID,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pCameraSwitchID: %s", this, pCameraSwitchID);
	return CFavorite::DelCameraSwitch(pCameraSwitchID);
}

// 获取轮巡资源列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCameraSwitchList(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	std::string strCameraSwitchList;
	IVS_INT32 iResultCode = CFavorite::GetCameraSwitchInfoList(strCameraSwitchList);
	std::string str;
	CFavorite::BulidGetScenarioInfoListResult(iResultCode, strCameraSwitchList, str);
	//BP_RUN_LOG_INF("get config","%s", str.c_str()); //log

	return _com_util::ConvertStringToBSTR(str.c_str());
}

// 查询轮巡资源详情
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCameraSwitchInfo(LPCTSTR pCameraSwitchID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraSwitchID, NULL);
	IVS_INFO_TRACE("OCX Address: %x,pCameraSwitchID: %s", this, pCameraSwitchID);
	std::string strCameraSwitchInfo;
	IVS_INT32 iResultCode = CFavorite::GetCameraSwitchInfo(pCameraSwitchID, strCameraSwitchInfo);
	std::string str;
	CFavorite::BulidGetScenarioInfoResult(iResultCode, strCameraSwitchInfo, str);
	BP_RUN_LOG_INF("IVS OCX Get Camera Switch Info End.","Result: %s", str.c_str());
	return _com_util::ConvertStringToBSTR(str.c_str());
}

// 本地抓拍图片
LONG CIVS_OCXPlayerCtrl::IVS_OCX_LocalSnapshot(LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address: %x, WndID: %ld", this, lWndID);
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Local Snapshot Failed.", "Reason: video pane manager is null");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Local Snapshot Failed.", "Reason: pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	IVS_LOCAL_CAPTURE_PARAM LocalCaptureParam;
	memset(&LocalCaptureParam, 0, sizeof(LocalCaptureParam));
	// 获取本地抓拍参数配置
	std::string ParamString = m_pLocalConfig.GetCDATA2XML(LOCAL_CONFING_FILE_NAME, CAPTURE_CONFIG_ELEMENT);
	if (IVS_SUCCEED != OCXCapturePictureXMLProcess::LocalCaptureParseClientConfig(ParamString.c_str(), LocalCaptureParam))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Local Snapshot Failed.", "Reason:ParseXml Failed.");
		return IVS_XML_INVALID;
	}

	return pVideoPane->LocalSnapshot(m_iSDKSessionID, LocalCaptureParam);
}

// 平台抓拍
LONG CIVS_OCXPlayerCtrl::IVS_OCX_PlatformSnapshot(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s",this,pCameraCode);
	IVS_INT32 iRet = IVS_FAIL;
	iRet = IVS_SDK_PlatformSnapshot(m_iSDKSessionID,pCameraCode);
	return (LONG)iRet;
}

// 获取平台抓拍图片列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetSnapshotList(LPCTSTR pQueryParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;
	CHECK_POINTER(pQueryParam,NULL);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s",this,pQueryParam);
	IVS_INT32 iRet = OCXCapturePicture::GetSnapshotList(m_iSDKSessionID, pQueryParam, strResult);

	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Snapshot List", "NA");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 下载平台抓拍图片
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DownloadSnapshot(ULONG ulPictureID, LPCTSTR pFileName)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here

	return 0;
}//lint !e715 

// 删除平台抓拍图片
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteSnapshot(LPCTSTR pCameraCode, ULONG ulPictureID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pCameraCode: %s", this, pCameraCode);
	return OCXCapturePicture::DeleteSnapshot(m_iSDKSessionID, pCameraCode, ulPictureID);
}

// 设置平台抓拍计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetSnapshotPlan(LPCTSTR pSnapshotPlan)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pSnapshotPlan)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Set Snapshot Plan Failed.", "Reason:pSnapshotPlan is NULL");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,SnapshotPlan: %s", this,pSnapshotPlan);
	IVS_INT32 iRet = OCXCapturePicture::SetSnapshotPlan(pSnapshotPlan,m_iSDKSessionID);
	return iRet;
}

// 查询平台抓拍计划
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetSnapshotPlan(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Snapshot Failed.", "Reason:pCameraCode is NULL.");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s",this,pCameraCode);

	strResult = OCXCapturePicture::GetSnapshotPlan(pCameraCode,m_iSDKSessionID);
	return strResult.AllocSysString();
}

//Logo下载
LONG CIVS_OCXPlayerCtrl::IVS_OCX_GetLogo(ULONG ulLogoType, LPCTSTR pLogoFile)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);

	return IVS_SDK_GetLogo(m_iSDKSessionID, ulLogoType, pLogoFile);
}

//Logo上传
LONG CIVS_OCXPlayerCtrl::IVS_OCX_UploadLogo(ULONG ulLogoType,LPCTSTR pLogoFile)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	return IVS_SDK_UploadLogo(m_iSDKSessionID, ulLogoType, pLogoFile);
}

//获取声明信息
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetStatement(LPCTSTR pLanguageType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	CHECK_POINTER(pLanguageType, NULL);
	CString strResult;
	IVS_CHAR* pStateMent = NULL;
	IVS_INT32 iRet = IVS_SDK_GetStatement(m_iSDKSessionID, pLanguageType, &pStateMent);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetStatement SDK Interface is fail", "NA");
		if (NULL != pStateMent)
		{
			IVS_SDK_ReleaseBuffer(pStateMent);
		}
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("StatementInfo");
	(void)xml.IntoElem();
	(void)xml.AddChildElem("Statement");
	(void)xml.IntoElem();
	(void)xml.SetElemValueCDATA(pStateMent);
	unsigned int uiLen = 0;
	strResult = xml.GetXMLStream(uiLen);

	(void)IVS_SDK_ReleaseBuffer(pStateMent);
	return strResult.AllocSysString();
}

//上传声明信息
LONG CIVS_OCXPlayerCtrl::IVS_OCX_UploadStatement(LPCTSTR pLanguageType,LPCTSTR pStatementFile)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	return IVS_SDK_UploadStatement(m_iSDKSessionID,pLanguageType,pStatementFile);
}	

//设置告警窗口闪红光
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetWarnState(ULONG ulWndID, ULONG ulWarnState, ULONG ulWarnSeconds)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX SetWarnStat: %x, ulWndID: %d,ulWarnState:%d,ulWarnSeconds: %d", this, ulWndID,ulWarnState,ulWarnSeconds);
	if ((MIN_PANE_NUMBER > ulWndID) || (MAX_PANE_NUMBER < ulWndID))
	{
		return IVS_PARA_INVALID;
	}

	if (WARNING != ulWarnState && NOT_WARN != ulWarnState)
	{
		return IVS_PARA_INVALID;
	}

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set Warn State Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_PARA_INVALID;
	}

	CVideoPane * pWnd = NULL;
	//入参窗格ID为0表示没有指定窗格，默认使用当前活动窗格
	pWnd = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWndID);
	if (NULL == pWnd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX SetWarn State Failed.", "Reason:GetWnd is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	//修改为不需要是告警窗口也可以闪
	//if (TYPE_WND_ALARM != pWnd->GetTypeWnd())
	//{
	//	BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX SetWarn State failed.", "Wnd is not Alarm");
	//	return IVS_PARA_INVALID;
	//}

	return (LONG)((CVideoPaneMgr*)m_pVideoPaneMgr)->SetWarnState(ulWndID,ulWarnState,ulWarnSeconds);
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartPlatformRecord(LPCTSTR pCameraCode,ULONG ulRecordTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,RecordTime:%lu", this,pCameraCode,ulRecordTime);
	// TODO: Add your dispatch handler code here
	IVS_INT32 iRet = IVS_FAIL;
	iRet = IVS_SDK_StartPlatformRecord(m_iSDKSessionID,pCameraCode,static_cast<IVS_INT32>(ulRecordTime));
	return iRet;

}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopPlatformRecord (LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s", this,pCameraCode);
	IVS_INT32 iRet = IVS_FAIL;
	iRet = IVS_SDK_StopPlatformRecord(m_iSDKSessionID,pCameraCode);
	return iRet;
}
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartPURecord(LPCTSTR pCameraCode,ULONG ulRecordTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,RecordTime:%lu", this,pCameraCode,ulRecordTime);
	IVS_INT32 iRet = IVS_FAIL;
	iRet = IVS_SDK_StartPURecord(m_iSDKSessionID,pCameraCode,static_cast<IVS_INT32>(ulRecordTime));
	return iRet;
}
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopPURecord (LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s", this,pCameraCode);
	IVS_INT32 iRet = IVS_FAIL;
	iRet = IVS_SDK_StopPURecord(m_iSDKSessionID,pCameraCode);
	return iRet;
}
// 启动开关量输出告警
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartAlarmOut(LPCTSTR pAlarmOutCode)
{
	CHECK_POINTER(pAlarmOutCode, NULL);
	IVS_INFO_TRACE("OCX Address: %x,pCameraSwitchID: %s", this, pAlarmOutCode);
	
	IVS_INT32 iRet = COCXAlarmMgr::StartAlarmOut(m_iSDKSessionID, pAlarmOutCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start Alarm Out fail", "NA");
		return iRet;
	}
	return iRet;
}

// 停止开关量输出告警
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopAlarmOut(LPCTSTR pAlarmOutCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pAlarmOutCode, NULL);
	IVS_INFO_TRACE("OCX Address: %x,pCameraSwitchID: %s", this, pAlarmOutCode);

	IVS_INT32 iRet = COCXAlarmMgr::StopAlarmOut(m_iSDKSessionID, pAlarmOutCode);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Stop Alarm Out fail", "NA");
		return iRet;
	}

	return iRet;

}	

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetCameraBackupConfig(LPCTSTR pBackupXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pBackupXml,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pBackupXml: %s", this, pBackupXml);
	return OCXRecordMgr::SetCameraBackupConfig(m_iSDKSessionID, pBackupXml);
}


BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCameraBackupConfig(LPCTSTR pCameraCode)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,NULL);
	CString strResult;

	IVS_INT32 iRet = OCXRecordMgr::GetCameraBackupCfg(m_iSDKSessionID, pCameraCode, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Camera Backup Config.", "Get Camera Backup Config Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

//设置窗口类型 ulTyle
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetWndType(ULONG ulWindID,ULONG ulTyle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Set WndType: %x, ulWindID: %d", this, ulWindID);
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set WndType Wnd Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_PARA_INVALID;
	}

	CVideoPane * pWnd = NULL;
	//入参窗格ID为0表示没有指定窗格，默认使用当前活动窗格
	pWnd = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWindID);
	if (NULL == pWnd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set Alarm Wnd Failed.", "Reason:GetWnd is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	LONG lRet = SetWndType(pWnd,ulTyle);
	return lRet;
}


ULONG CIVS_OCXPlayerCtrl::SetAlarmState(HWND paneHandle,BOOL bIsOpen)
{
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set Alarm State Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_PARA_INVALID;
	}

	ULONG ulWndID = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWindIDByHWND((HWND)paneHandle);
	if (IVS_SUCCEED != ((CVideoPaneMgr*)m_pVideoPaneMgr)->SetAlarmState(ulWndID, (bIsOpen?true:false)))
	{
		BP_RUN_LOG_ERR(IVS_OCX_RET_SET_WARN_STATE_ERROR, "IVS OCX Set Alarm State Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_OCX_RET_SET_WARN_STATE_ERROR;
	}

	// 向CU上报事件
	CString strToCUXML;
	if (TRUE == bIsOpen)
	{
		COCXPlayBackXMLProcess::GetWndIDXML(strToCUXML,ulWndID);
		IVS_OCX_Windows_Event(EVENT_BUTTON_SET_ALARM_WND,(LPCTSTR)strToCUXML);
	}
	else
	{
		COCXPlayBackXMLProcess::GetWndIDXML(strToCUXML,ulWndID);
		IVS_OCX_Windows_Event(EVENT_BUTTON_DEL_ALARM_WND,(LPCTSTR)strToCUXML);
	}

	return IVS_SUCCEED;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteAlarmWnd(ULONG ulWindID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX DeleteAlarmWnd: %x, ulWindID: %d", this, ulWindID);
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Delete Alarm Wnd Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_PARA_INVALID;
	}

	CVideoPane * pWnd = NULL;
	//入参窗格ID为0表示没有指定窗格，默认使用当前活动窗格
	pWnd = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWindID);
	if (NULL == pWnd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Delete Alarm Wnd Failed.", "Reason:GetWnd is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	if (BOTTOMBAR_ALARM_OPEN != pWnd->GetRealtimeAlarmState())
	{
		BP_RUN_LOG_ERR(IVS_OCX_RET_VIDEOPANE_ISNOT_ALARM_ERROR, "IVS OCX Delete Alarm Wnd Failed.", "Reason:GetWnd is NULL.");
		return IVS_OCX_RET_VIDEOPANE_ISNOT_ALARM_ERROR;
	}

	ULONG ulRet = SetAlarmState(pWnd->GetVideoWnd(), FALSE);	
	if (IVS_SUCCEED != ulRet)
	{
		BP_RUN_LOG_ERR(static_cast<IVS_INT32>(ulRet), "IVS OCX Delete Alarm State Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return static_cast<LONG>(ulRet);
	}

	return IVS_SUCCEED;
}

//重置置所有电视墙窗格状态
void CIVS_OCXPlayerCtrl::ResetAllTVwallPaneStatus()
{
	IVS_INFO_TRACE("OCX Reset All TVWall Pane Status: %x", this);
	CHECK_POINTER_VOID(m_pVideoPaneMgr);

	//获取所有窗格,所有电视墙窗格状态初始化
	MAP_VIDEOPANE wndPaneIDMap;
	(void)m_pVideoPaneMgr->GetAllVideoPane(wndPaneIDMap);
	for(MAP_VIDEOPANE::iterator paneiter = wndPaneIDMap.begin();paneiter != wndPaneIDMap.end();paneiter++)
	{
		CVideoPane* pVideoPane = paneiter->second;//lint !e64
		if(nullptr == pVideoPane)
		{
			break;
		}
		//如果本窗口正在播放实况，则停止实况播放
		if (VIDEO_PLAY_STATE_NOT_PLAYING != pVideoPane->GetPlayState())
		{
			LONG id = static_cast<LONG>(paneiter->first);
			(void)IVS_OCX_StopRealPlay(id);
		}

		//如果是告警窗口，则设置为普通窗口
		if(TYPE_WND_NOMAL != pVideoPane->GetTypeWnd())
		{
			pVideoPane->SetTypeWnd(TYPE_WND_NOMAL);
			(void)m_pVideoPaneMgr->SetAlarmState(static_cast<ULONG>(paneiter->first),false);
		}	

		//去掉显示重连
		if(pVideoPane->GetResumeState())
		{
			pVideoPane->SetResumeState(false);
		}

        //去掉显示上墙背景字幕
        if(pVideoPane->GetTvWallBgState())
        {
            pVideoPane->SetTvWallBgState(false);
        }

		//如果是轮训窗口设置为轮训
		if (pVideoPane->GetTurnCruiseStatus())
		{
			(void)pVideoPane->DeleteTurnCruiseStatus();
		}

		//设置电视墙模式
		(void)pVideoPane->SetTvWallState(true);//lint !e792

		//电视墙通道状态图标为离线，隐藏上墙状态图标
        // modi by wKF71044 at [2013-7-2 11:55:34] 为什么要设置离线？
		//pVideoPane->SetTVWallStatusBarStatus(TVWALL_BAR_STAUS_OFFLINE);
		pVideoPane->SetTVWallStatus(TVWALL_STATUS_NONE);
		pVideoPane->SetAlarmButtonStatus((unsigned long long)1);//告警按钮先置为亮
		pVideoPane->ResetPane();
	}
}

//刷新所有在线的电视墙通道
void CIVS_OCXPlayerCtrl::RefreshOnlineTVWallStatus()
{
	IVS_INFO_TRACE("OCX Refresh Online TVWall Pane Status: %x", this);
	CHECK_POINTER_VOID(m_pVideoPaneMgr);

	//获取在线的解码器
	std::list<CUMW_NOTIFY_REMOTE_DECODER_STATUS> onLineDecoderList;
	IVS_INT32 iRet = COCXTVWallMgr::GetOnlineDecoderIDList(m_iSDKSessionID,onLineDecoderList);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"IVS OCX Refresh TVWall Status Failed.", "Reason:OCX Get Online DecoderID List Failed.");
		return;
	}

	IVS_ULONG curTVWallID = m_pVideoPaneMgr->GetCurTVWallID();
	for(std::list<CUMW_NOTIFY_REMOTE_DECODER_STATUS>::iterator iter = onLineDecoderList.begin();iter!=onLineDecoderList.end();iter++)
	{
		CUMW_NOTIFY_REMOTE_DECODER_STATUS decoder = (*iter);
		TVWall_VPANEID_LIST listWndID;
		listWndID.clear();
		if(IVS_SUCCEED == COCXTVWallMgr::GetTVWallWndIDByDecoderIDChannel(curTVWallID,decoder,listWndID))
		{
			//将对应的窗格转换成真实的窗格
			std::vector<IVS_ULONG> vecRealPaneID;
			vecRealPaneID.clear();
			m_pVideoPaneMgr->GetRealTVwallPaneID(listWndID,vecRealPaneID);

			//设置TVWallBar背景图为上线
			IVS_UINT32 uSize = vecRealPaneID.size();
			for(IVS_UINT32 j = 0;j<uSize;j++)
			{
				m_pVideoPaneMgr->SetTVWallPaneChannelStatus(vecRealPaneID.at(j),TVWALL_BAR_STAUS_ONLINE);
			}					
		}

	}
}
//设置电视墙OCX窗口布局
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetTVWallWndLayout(ULONG ulTVWallID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x, TVWallID: %lu", this, ulTVWallID);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);

    //获取所有窗格,所有电视墙窗格状态初始化
    ResetAllTVwallPaneStatus();

	//电视墙相关OCX，注册电视墙事件回调
	IVS_INT32 iRet = IVS_SDK_AddCallBackTVWall(m_iSDKSessionID, HandleTVWallNotifyCallBack, this);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Add TVWall CallBack", "Failed");
	}

	//从第一个窗格开始分配位置与大小
    CWnd *pWnd = m_pVideoPaneMgr->GetParentWnd();
    CHECK_POINTER(pWnd, IVS_FAIL);
	CRect rect;
	pWnd->GetClientRect(rect);
	IVS_WIND_LAYOUT oldLayOut = m_pVideoPaneMgr->GetWindLayOut();
	//调用管理类的设置窗格布局
	iRet = m_pVideoPaneMgr->SetTVWallWndLayout(rect, ulTVWallID);
	if (IVS_SUCCEED != iRet)
	{
		(void)IVS_OCX_SetWndLayout(oldLayOut);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Set TVWall Layout ", "Failed.");
		return iRet;
	}

    //获取所有窗格,所有电视墙窗格状态初始化
    ResetAllTVwallPaneStatus();

	//将在线解码器关联的电视墙窗格状态置为在线
	RefreshOnlineTVWallStatus();

	return IVS_SUCCEED;
}

//设置电视墙窗格状态
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetTVWallPaneStatus(ULONG ulTVWallID, ULONG ulPaneID, ULONG ulPaneStatus)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	IVS_INFO_TRACE("OCX Address: %x,TVWallID: %lu,PaneID: %lu,PaneStatus: %lu", this,ulTVWallID,ulPaneID,ulPaneStatus);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
	IVS_ULONG ulCurrentTVWallID = m_pVideoPaneMgr->GetCurTVWallID();//当前的电视墙
	if(ulCurrentTVWallID == ulTVWallID)
	{
		m_pVideoPaneMgr->SetTVWallPaneStatus(ulPaneID,ulPaneStatus);
	}
	else
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Set TVWall Pane Status Failed.","Reason:Input TVWallID Invalid.");
		return IVS_PARA_INVALID;
	}

	return IVS_SUCCEED;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetLanguage(LPCTSTR pLanguage)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX SetLanguage: %x, ulWindID: %s", this, pLanguage);
	if ((NULL == pLanguage)
		|| (0 == strlen(pLanguage)))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Set Language Failed.","pLanguage is NULL or pLanguage's length is 0")
		return IVS_PARA_INVALID;
	}

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set Language Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_PARA_INVALID;
	}

	//char szLanguage[1024] = {0};// lint !e732
	//memcpy(szLanguage, pLanguage, sizeof(szLanguage));
	LONG lRet = ((CVideoPaneMgr*)m_pVideoPaneMgr)->SetLanguage(pLanguage);
	if (IVS_SUCCEED != lRet)
	{
		BP_RUN_LOG_ERR(static_cast<IVS_INT32>(lRet), "IVS OCX Set Language Failed.", "Reason:ErrorCode is %d.",lRet);
		return static_cast<LONG>(lRet);
	}

	return IVS_SUCCEED;
}
// 启动搜索前端设备
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartSearchDevice(LPCTSTR pDomainCode, LPCTSTR pSearchInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,pSearchInfo: %s", this, pSearchInfo);

	if (NULL == pDomainCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start SearchDevice fail", "pDomainCode is NULL");
		return IVS_PARA_INVALID;
	}
	if (NULL == pSearchInfo)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Start SearchDevice fail", "pSearchInfo fail");
		return IVS_XML_INVALID;
	}
	IVS_INT32 iRet = COCXDeviceMgr::StartSearchDevice(m_iSDKSessionID, (IVS_CHAR*)pDomainCode, (IVS_CHAR*)pSearchInfo);

	return iRet;
}

// 停止搜索前端设备
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopSearchDevice(LPCTSTR pDomainCode, LPCTSTR pSearchInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,pSearchInfo: %s", this, pSearchInfo);

	if (NULL == pDomainCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Stop SearchDevice fail", "pDomainCode is NULL");
		return IVS_PARA_INVALID;
	}
	if (NULL == pSearchInfo)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Stop SearchDevice fail", "pSearchInfo fail");
		return IVS_XML_INVALID;
	}
	IVS_INT32 iRet = COCXDeviceMgr::StopSearchDevice(m_iSDKSessionID, (IVS_CHAR*)pDomainCode, (IVS_CHAR*)pSearchInfo);

	return iRet;
}

// 验证前端设备
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_VerifyDevice(LPCTSTR pDomainCode, LPCTSTR pVerifyInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,pVerifyInfo: %s", this, pVerifyInfo);
	CString strResult;

	if (NULL == pDomainCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "VerifyDevice fail", "pDomainCode is NULL");
		COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
		return strResult.AllocSysString();
	}
	if (NULL == pVerifyInfo)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "VerifyDevice fail", "pVerifyInfo fail");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	COCXDeviceMgr::VerifyDevice(m_iSDKSessionID, pDomainCode, (IVS_CHAR*)pVerifyInfo, strResult);
	// 防止内存中密码出现
	memset((char*)pVerifyInfo,0,strlen(pVerifyInfo));

	return strResult.AllocSysString();
}

// 查询前端日志
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDevLog(LPCTSTR pQueryXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,pQueryXml: %s", this, pQueryXml);
	CString strResult;

	if (NULL == pQueryXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get DevLog fail", "pQueryXml fail");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	COCXDeviceMgr::GetDevLog(m_iSDKSessionID, (IVS_CHAR*)pQueryXml, strResult);

	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDecoderList(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	CString strResult;

	COCXTVWallMgr::GetDecoderList(m_iSDKSessionID, strResult);
	return strResult.AllocSysString();
}

ULONG CIVS_OCXPlayerCtrl::StartTalkback(ULONG ulWinID)
{
	IVS_INFO_TRACE("OCX Address: %x", this);
	if (0 == ulWinID) return IVS_PARA_INVALID;

	CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);
	
	//如果已经在语音对讲
    if (m_bVoiceTalkbacking)
    {
        return IVS_SUCCEED;
    }

	/*****************************
	右键实现语音对讲
	    1、如果本身在对讲，则返回；
		2、删除其他的语音对讲；
		3、关闭其他的随路语音.
	实现，语音对讲的唯一，声音的唯一
	******************************/
	// 获取当前活动窗格
	CVideoPane * pVideoPane = m_pVideoPaneMgr->GetWndPointerByID(ulWinID);
// 	if(NULL != pVideoPane)
// 	{
// 		if(BOTTOMBAR_SOUND_OPEN == pVideoPane->GetRealtimeTalkbackState())
// 		{
// 			return IVS_SUCCEED;
// 		}
// 	}
	ULONG ulHandle = 0;
	IVS_VOICE_TALKBACK_PARAM stTalkbackParam;
	memset(&stTalkbackParam, 0, sizeof(stTalkbackParam));
	const char* pCameraCode = pVideoPane->GetCameraID();
	// 获取参数
	stTalkbackParam.bDirectFirst = (IVS_BOOL)IVS_OCX_GetLocalCameraLinkConfig(pCameraCode);

	// modify by limingjie lWX175483 2013-7-3
	// 当返回值不为0时，都置为0。逻辑不对。改为当不是优先直连，置为默认值0。
	//if (stTalkbackParam.bDirectFirst != 0 || stTalkbackParam.bDirectFirst != 1)
	//if (stTalkbackParam.bDirectFirst != 1)
	//{
	//	stTalkbackParam.bDirectFirst = 0;
	//}
	stTalkbackParam.bDirectFirst = 0;
	stTalkbackParam.uiProtocolType = 1;

	ULONG ulRet = (ULONG)IVS_SDK_StartVoiceTalkback(m_iSDKSessionID, &stTalkbackParam,pCameraCode, &ulHandle);

    //ULONG ulRet = ((CVideoPaneMgr*)m_pVideoPaneMgr)->StartVoiceTalkback("", 0, 0/*, ulProtocolType*/); //lint !e1773    

    //如果开启成功，那记录变量
    if (IVS_SUCCEED == ulRet)
    {
		m_ulVoiceTalkbackHandle = ulHandle;
        m_bVoiceTalkbacking = TRUE;
        m_pVoiceTalkbackOCX = this;

        // add by wanglei 00165153:2013.6.28 开启成功后，需要保存当前对讲设备编码
        m_strVoiceTalkbackCameraCode = pCameraCode;

        if (NULL != pVideoPane)
        {
            //记录当前正在语音对讲的窗口
            m_pVoiceTalkbackVideoPane = pVideoPane;
// 			if (NULL != m_pVoiceTalkbackVideoPane->GetVideoRealtimeBar())
// 			{
// 				m_pVoiceTalkbackVideoPane->GetVideoRealtimeBar()->SetShowTalkbackSoundCtrl(true);
// 			}
        }
    }

    return ulRet;
}

ULONG CIVS_OCXPlayerCtrl::StopTalkback(ULONG ulWinID)const
{
	if (0 == ulWinID)
	{
		return IVS_PARA_INVALID;
	}
	CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(m_pVoiceTalkbackVideoPane,IVS_OPERATE_MEMORY_ERROR);
	//ULONG ulRet = ((CVideoPaneMgr*)m_pVideoPaneMgr)->StopVoiceTalkback();
	if (NULL != m_pVoiceTalkbackVideoPane->GetVideoRealtimeBar())
	{
		m_pVoiceTalkbackVideoPane->GetVideoRealtimeBar()->SetShowTalkbackSoundCtrl(true);
       // m_pVoiceTalkbackVideoPane->SetRealtimeTalkbackState(BOTTOMBAR_TALKBACK_CLOSE);
        
	}

	ULONG ulRet = (ULONG)IVS_SDK_StopVoiceTalkback(m_iSDKSessionID, m_ulVoiceTalkbackHandle);
	if (IVS_SUCCEED == ulRet)
	{
		//清理语音对讲状态
		m_bVoiceTalkbacking = FALSE;
		m_pVoiceTalkbackOCX = NULL; //这个如果异步，那要放到异步的返回中处理

		m_pVoiceTalkbackVideoPane = NULL;
		m_ulVoiceTalkbackHandle = 0;
		m_strVoiceTalkbackCameraCode = "";
        
	}

	return IVS_SUCCEED;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_StartVoiceTalkback(LPCTSTR pTalkParam, LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
    //CHECK_POINTER(pTalkParam, IVS_PARA_INVALID);

    CString strResult;
    IVS_ULONG ulHandle = 0;

    if (NULL == pTalkParam
        || NULL == pCameraCode)
    {
        COCXXmlProcess::GetTalkbackXML(strResult, IVS_PARA_INVALID, ulHandle);
        return strResult.AllocSysString();
    }

    IVS_VOICE_TALKBACK_PARAM stTalkbackParam;
    memset(&stTalkbackParam, 0, sizeof(stTalkbackParam));

    if (IVS_SUCCEED != COCXPlayBackXMLProcess::GetTalkbackParamXML(pTalkParam, stTalkbackParam))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start Voice Talkback Failed.", "Reason:MediaPara Xml Invalid");
        COCXXmlProcess::GetTalkbackXML(strResult, IVS_PARA_INVALID, ulHandle);
        return strResult.AllocSysString();
    }
    
    if (!m_strVoiceTalkbackCameraCode.empty() && TRUE == m_bVoiceTalkbacking)
    {
        COCXXmlProcess::GetTalkbackXML(strResult, IVS_SDK_TALKBACK_ERR_ALREADY_OPENED, ulHandle);//lint !e613
        return strResult.AllocSysString();
    }

	IVS_INT32 iRet = IVS_FAIL;


	// 查找是否有开启实况的窗口，同步状态
	CVideoPane* pVideoPane = GetVideoPaneMgr()->GetWndPointerByCameraCode(pCameraCode);//lint !e613
	if (NULL != pVideoPane)
	{
		m_pVoiceTalkbackVideoPane = pVideoPane;
		CTranVideoRealtimeBar* pRealtimeBar = m_pVoiceTalkbackVideoPane->GetVideoRealtimeBar();
		if (NULL != pRealtimeBar)
		{
			// 这种方式打开的随路语音 不显示声音控制条
			pRealtimeBar->SetShowTalkbackSoundCtrl(false);
			::SendMessage(pRealtimeBar->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_TALKBACK, BN_CLICKED), (LPARAM)pRealtimeBar->GetTalkbackBtnHwnd());
			iRet = IVS_SUCCEED;
		}
	}
	else
	{
		stTalkbackParam.bDirectFirst = 0;
		iRet = IVS_SDK_StartVoiceTalkback(m_iSDKSessionID, &stTalkbackParam,
			pCameraCode, &ulHandle);

        //  modify by wanglei 00165153:2013.6.10 DTS2013060606556 
        // 开启语音对讲，需要关闭任一随路语音
        if (IVS_SUCCEED == iRet)
        {
            // 保存摄像机编码，用于CU开启语音，但是没有开启实况时，开启实况后同步状态
            m_strVoiceTalkbackCameraCode = pCameraCode;
            m_bVoiceTalkbacking = TRUE;

            CGlobalSoundMgr &SoundMgr = CGlobalSoundMgr::Instance();
            SoundMgr.SetSoundingPanel(NULL);
        }

		// 存句柄用于音量调节
		m_ulVoiceTalkbackHandle = ulHandle;
	}
	COCXXmlProcess::GetTalkbackXML(strResult, iRet, m_ulVoiceTalkbackHandle);
    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopVoiceTalkback(ULONG ulTalkbackHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	IVS_INT32 iRet = IVS_FAIL;
	// 查找是否有开启实况的窗口，同步状态

	if (NULL != m_pVoiceTalkbackVideoPane)
	{


        if (VIDEO_PLAY_STATE_NOT_PLAYING == m_pVoiceTalkbackVideoPane->GetPlayState())
        {
            iRet = IVS_SDK_StopVoiceTalkback(m_iSDKSessionID, ulTalkbackHandle);
            m_pVoiceTalkbackVideoPane->GetVideoPaneMgr()->StopTalkbackSoundForOther();
            m_pVoiceTalkbackVideoPane->SetRealtimeTalkbackState(BOTTOMBAR_TALKBACK_CLOSE);
        }
        else
        {
            CTranVideoRealtimeBar* pRealtimeBar = m_pVoiceTalkbackVideoPane->GetVideoRealtimeBar();
            if (NULL != pRealtimeBar)
            {
                //pRealtimeBar->SetShowTalkbackSoundCtrl(true);
                ::SendMessage(m_pVoiceTalkbackVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_TALKBACK, BN_CLICKED), (LPARAM)pRealtimeBar->GetTalkbackBtnHwnd());
            }
        }
		iRet = IVS_SUCCEED;
	}

	else
	{
		iRet = IVS_SDK_StopVoiceTalkback(m_iSDKSessionID, ulTalkbackHandle);
        
	}
	// 存句柄用于音量调节
	m_ulVoiceTalkbackHandle = 0;
	m_pVoiceTalkbackVideoPane = NULL;
	m_strVoiceTalkbackCameraCode = "";
    // modify by wanglei 00165153:2013.6.28 对讲停止后，需要重置状态
    m_bVoiceTalkbacking = FALSE;
    return iRet;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetTalkbackVolume(ULONG ulVolumeValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	// TODO: Add your dispatch handler code here

	return IVS_SUCCEED;
}//lint !e715


ULONG CIVS_OCXPlayerCtrl::IVS_OCX_GetTalkbackVolume(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	// TODO: Add your dispatch handler code here

	return IVS_SUCCEED;
}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_SetCameraAlias(LPCTSTR pCameraCode, LPCTSTR pAlias)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pAlias,IVS_PARA_INVALID);

	CLockGuard cCameraAliasLock(m_pMutexCameraAlias);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s, Alias: %s", this,pCameraCode,pAlias);
	// 判断文件是否存在，不存在就创建一个模板
	std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(CAMERA_ALIAS_FILE_NAME);
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "IVS OCX Set Camera Alias Failed.","Reason:file not exist.");
		return IVS_OPEN_FILE_ERROR;
	}

	// 加载XML文件
	TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	CHECK_POINTER(pXmlDoc, IVS_ALLOC_MEMORY_ERROR);
	if (!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		BP_RUN_LOG_INF("load xml failed","rebuild xml");
		// 如果加载失败，重新用模板生成这个XML
		if (!m_pLocalConfig.CreateCameraAliasXML(strPath.c_str()))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Set Camera Alias Failed.","Reason:file not exist.");
			return IVS_CREATE_FILE_ERROR;
		}
		if(!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX parse xml Failed.","Reason:load xml failed");
			return IVS_XML_INVALID;
		}
	}
	
	TiXmlElement* pConnListElem = CToolsHelp::FindElementByPath(pXmlDoc,DEVICE_CAMERAALIAS_ELEMENT);
	if (NULL == pConnListElem)
	{
		IVS_DELETE(pXmlDoc);
		return IVS_XML_INVALID;
	}
	// 遍历是否已经设置了，已经设置了就修改
	TiXmlElement* pNode = pConnListElem->FirstChildElement(DEVICE_ALIAS_NODE);
	while(NULL != pNode)
	{
		LPCTSTR pCode = pNode->Attribute(DEVCODE_ATTRIBUTE);
		if (NULL == pCode)
		{
			continue;
		}
		// 如果找到一样的code
		if (!strcmp(pCameraCode, pCode))
		{
			pNode->FirstChild()->ToText()->SetCDATA(true);
			pNode->FirstChild()->ToText()->SetValue(pAlias);
			pXmlDoc->SaveFile(strPath.c_str());
			IVS_DELETE(pXmlDoc);
			return 0 ;
		}
		pNode = pNode->NextSiblingElement(DEVICE_ALIAS_NODE);
	}

	// 没有找到一样的节点，新增一个节点
	TiXmlElement* pConnElem = NULL;
	try{
		pConnElem = new TiXmlElement(DEVICE_ALIAS_NODE);
		pConnElem->LinkEndChild(new TiXmlText(pAlias))->ToText()->SetCDATA(true);
	}
	catch(...)
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "IVS OCX  Add Camera Alias Failed.","Reason:Try New TiXmlElement Failed.");
		IVS_DELETE(pXmlDoc);
		return IVS_ALLOC_MEMORY_ERROR;
	}

	pConnElem->SetAttribute(DEVCODE_ATTRIBUTE,pCameraCode);
	pConnListElem->LinkEndChild(pConnElem);
	pXmlDoc->SaveFile(strPath.c_str());
	IVS_DELETE(pXmlDoc);
	//IVS_DELETE(pConnElem);
	return 0;//lint !e429 pConnElem deleted by pXmlDoc
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCameraAlias(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);

	CLockGuard cCameraAliasLock(m_pMutexCameraAlias);
	CString strResult;
	// 判断文件是否存在，不存在就创建一个模板
	std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(CAMERA_ALIAS_FILE_NAME);
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get Camera Alias Failed.","Reason:File not exist.");
		return NULL;
	}

	TiXmlDocument* pXmlDoc = IVS_NEW(pXmlDoc);
	CHECK_POINTER(pXmlDoc, NULL);
	if (!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		BP_RUN_LOG_INF("load xml failed","rebuild xml");
		// 如果加载失败，重新用模板生成这个XML
		if (!m_pLocalConfig.CreateCameraAliasXML(strPath.c_str()))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get Camera Alias Failed.","Reason:Create Local Config xml Failed.");
			return NULL;
		}
		if(!pXmlDoc->LoadFile(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			IVS_DELETE(pXmlDoc);
			BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get Camera Alias Failed.","Reason:load xml failed");
			return NULL;
		}
	}

	TiXmlElement* pConnListElem = CToolsHelp::FindElementByPath(pXmlDoc,DEVICE_CAMERAALIAS_ELEMENT);
	if (NULL == pConnListElem)
	{
		IVS_DELETE(pXmlDoc);
		return NULL;
	}
	// 遍历是否已经设置了，已经设置了就修改
	TiXmlElement* pNode = pConnListElem->FirstChildElement(DEVICE_ALIAS_NODE);
	while(NULL != pNode)
	{
		LPCTSTR pCode = pNode->Attribute(DEVCODE_ATTRIBUTE);
		if (NULL == pCode)
		{
			continue;
		}
		// 如果找到一样的code
		if (!strcmp(pCameraCode, pCode))
		{
			strResult.Format(_T("%s"),const_cast<TCHAR *>(pNode->FirstChild()->Value()));
			break;
		}
		pNode = pNode->NextSiblingElement(DEVICE_ALIAS_NODE);
	}

	IVS_DELETE(pXmlDoc);
	return	strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAllCameraAlias(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);

	CLockGuard cCameraAliasLock(m_pMutexCameraAlias);
	// 判断文件是否存在，不存在就创建一个模板
	std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(CAMERA_ALIAS_FILE_NAME);
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get All Camera Alias Failed.","Reason:File not exist.");
		return NULL;
	}

	// 读取XML，保存参数
	CXml xml;
	if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		// 如果加载失败，重新用模板生成这个XML
		if (!m_pLocalConfig.CreateCameraAliasXML(strPath.c_str()))
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get All Camera Alias Failed.","Reason:Create Local Config xml Failed.");
			return NULL;
		}
		// 如果再失败。。
		if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Get All Camera Alias Failed.","Reason:load xml failed");
			return NULL;
		}
	}

	unsigned int iXMLSize = 0;
	const char* xmlStrame = xml.GetXMLStream(iXMLSize);
	CHECK_POINTER(xmlStrame,nullptr);

	return	_com_util::ConvertStringToBSTR(xmlStrame);
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDomainList(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	CString strResult;
	IVS_CHAR* pDomainList = NULL;
	IVS_INT32 iRet = IVS_SDK_GetDomainList(m_iSDKSessionID, &pDomainList);
	if (IVS_SUCCEED != iRet || NULL == pDomainList)
	{
		BP_RUN_LOG_ERR(iRet, "GetDomainList SDK Interface is fail", "NA");
		if (NULL != pDomainList)
		{
			IVS_SDK_ReleaseBuffer(pDomainList);
		}
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	// 在content节点下面拼装一个resultcode节点
	std::string strInXml = pDomainList;
	std::string strOutXml;
	iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "add result code failed", "xml error");
		IVS_SDK_ReleaseBuffer(pDomainList);
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

	(void)IVS_SDK_ReleaseBuffer(pDomainList);
	strResult = strOutXml.c_str();
	return strResult.AllocSysString();
}

// 查询设备告警列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDeviceAlarmList(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	// 入参判空
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Device AlarmList Failed","Reason:pReqXml is NULL");//lint !e534
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,UnifiedQuery: %s", this,pReqXml);

	IVS_INT32 iRet = COCXAlarmMgr::GetDeviceAlarmList(m_iSDKSessionID, pReqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Device AlarmList Failed.", "Reason:LogMgr Get Device AlarmList Failed.");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCPUMemory(LPCTSTR pNodeCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	// 入参判空
	if (NULL == pNodeCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Device AlarmList Failed","Reason:pReqXml is NULL");//lint !e534
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,pNodeCode: %s", this,pNodeCode);
	IVS_INT32 iRet = COCXClientNetworkMgr::GetCPUMemory(m_iSDKSessionID, pNodeCode, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get CPU Memory OCX Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 添加录像备份计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AddRecordBackupPlan(LPCTSTR pBackupPlan)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_POINTER(pBackupPlan,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pBackupPlan: %s", this, pBackupPlan);
	return OCXRecordMgr::AddRecordBackupPlan(m_iSDKSessionID, pBackupPlan);
}

// 修改录像备份计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyRecordBackupPlan(LPCTSTR pBackupPlan)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_POINTER(pBackupPlan,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pBackupPlan: %s", this, pBackupPlan);
	return OCXRecordMgr::ModifyRecordBackupPlan(m_iSDKSessionID, pBackupPlan);
}

// 删除录像备份计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteRecordBackupPlan(LPCTSTR pBackupPlan)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_POINTER(pBackupPlan,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pBackupPlan: %s", this, pBackupPlan);
	return OCXRecordMgr::DeleteRecordBackupPlan(m_iSDKSessionID, pBackupPlan);
}

// 查询录像备份计划
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRecordBackupPlan(LPCTSTR pQueryInfo)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pQueryInfo,NULL);
	CString strResult;
	IVS_INT32 iRet = OCXRecordMgr::GetRecordBackupPlan(m_iSDKSessionID, pQueryInfo, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Record Backup Plan", "NA");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 删除平台抓拍计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DelSnapshotPlan(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pCameraCode: %s", this, pCameraCode);
	return OCXCapturePicture::DelSnapshotPlan(m_iSDKSessionID, pCameraCode);
}

// 查询摄像机计划设置信息
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCameraPlanInfo(LPCTSTR pReqXml)const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(pReqXml,NULL);

    CString strResult;
    IVS_INT32 iRet = OCXRecordMgr::GetCameraPlanInfo(m_iSDKSessionID, pReqXml, strResult);
    if (IVS_SUCCEED != iRet)
    {
        BP_RUN_LOG_ERR(iRet, "IVS OCX Get Record Backup Plan", "NA");
        COCXXmlProcess::GetErrString(strResult,iRet);
        return strResult.AllocSysString();
    }
    return strResult.AllocSysString();
}

// 开始实时广播
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartRealBroadcast(void)const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    return IVS_SDK_StartRealBroadcast(m_iSDKSessionID);
}

// 停止实时广播
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopRealBroadcast(void)const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    return IVS_SDK_StopRealBroadcast(m_iSDKSessionID);

}

// 开始文件语音广播
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartFileBroadcast(LPCTSTR pFileName, ULONG ulCycle)const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    CHECK_POINTER(pFileName, IVS_PARA_INVALID);
    return IVS_SDK_StartFileBroadcast(m_iSDKSessionID, pFileName, ulCycle);//lint !e713
}

// 停止文件广播
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopFileBroadcast(void)const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    return IVS_SDK_StopFileBroadcast(m_iSDKSessionID);
}

//添加广播组设备
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AddBroadcastDevice(LPCTSTR pCameraCode)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
	if(NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add Broadcast Device.", "Reason:pCameraCode is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pCameraCode: %s", this,pCameraCode);
	IVS_INT32 iRet = IVS_SDK_AddBroadcastDevice(m_iSDKSessionID, pCameraCode);
	if(IVS_SUCCEED == iRet)
	{
		std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(BROADCAST_CAMERALIST_CAMERA_CODE);
		if (strPath.empty())
		{
			BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Add BroadcastDevice Failed", "NA");
			return IVS_CREATE_FILE_ERROR;
		}
		// 读取XML，保存参数值
		CXml xml;
		if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			//如果加载失败，则重新用模板生成这个XML
			if (!m_pLocalConfig.CreateBroadcastFileListXML(strPath.c_str()))
			{
				BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Add BroadcastDevice Failed.", "NA");
				return IVS_CREATE_FILE_ERROR;
			}
			// 如果再失败则。。。
			if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
			{
				BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "IVS OCX Add BroadcastDevice Failed.", "NA");
				return IVS_OPEN_FILE_ERROR;
			}
		}
		if (!xml.FindElemEx(BROADCAST_CAMERALIST_ELEMENT))
		{
			BP_RUN_LOG_ERR(IVS_FILE_FORMAT_ERROR, "IVS OCX Add BroadcastDevice Failed.", "NA");
			return IVS_FILE_FORMAT_ERROR;
		}
		std::string strCameraCode = pCameraCode;
		// 摄像机列表为空，直接添加
		if (!xml.FindElem("CameraCode"))
		{
			BP_RUN_LOG_INF("Broadcast CameraCode isn't exist,add to xml file", "NA");
			(void)xml.AddChildElem("CameraCode");
			(void)xml.IntoElem();
			(void)xml.SetElemValue(strCameraCode.c_str());
			(void)xml.Save();
		}

		// 键值信息非空，根据键值信息查找对应节点
		do 
		{
			const char* pCameraCodeValue = xml.GetElemValue();
			if (NULL == pCameraCodeValue)
			{
				continue;
			}
			if (0 == strCameraCode.compare(pCameraCodeValue))
			{
				BP_RUN_LOG_INF("CameraCode Info already exist", "NA");
				return IVS_SUCCEED;
			}
		} while (xml.NextElem());
		xml.AddElem("CameraCode");
		xml.SetElemValue(strCameraCode.c_str());

		(void)xml.Save();
	}

	return iRet;
}

//删除广播组设备
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteBroadcastDevice(LPCTSTR pCameraCode)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
	if(NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Delete Broadcast Device.", "Reason:pCameraCode is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pCameraCode: %s", this,pCameraCode);
	IVS_INT32 iRet = IVS_SDK_DeleteBroadcastDevice(m_iSDKSessionID, pCameraCode);
    // modify by wanglei 00165153:2013.5.29 不管广播是否删除成功，本地XML列表都进行清除
	//if(IVS_SUCCEED == iRet)
    // {...}内包含删除本地广播设备XML逻辑
	{
		std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(BROADCAST_CAMERALIST_CAMERA_CODE);
		if (strPath.empty())
		{
			BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX delete Broadcast Device Failed", "NA");
			return IVS_CREATE_FILE_ERROR;
		}
		// 读取XML，保存参数
		CXml xml;
		if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			// 如果加载失败，重新用模板生成这个XML
			if (!m_pLocalConfig.CreateBroadcastCameraListXML(strPath.c_str()))
			{
				BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX delete Broadcast Device Failed.", "NA");
				return IVS_CREATE_FILE_ERROR;
			}
			// 如果再失败。。
			if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
			{
				BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "IVS OCX delete Broadcast Device Failed.", "NA");
				return IVS_OPEN_FILE_ERROR;
			}
		}
		if (!xml.FindElemEx(BROADCAST_CAMERALIST_ELEMENT))
		{
			BP_RUN_LOG_ERR(IVS_FILE_FORMAT_ERROR, "IVS OCX delete Broadcast Device Failed.", "NA");
			return IVS_FILE_FORMAT_ERROR;
		}
		std::string strCameraCode = pCameraCode;
		// 文件列表为空，直接添加
		if (!xml.FindElem("CameraCode"))
		{
			BP_RUN_LOG_INF("Broadcast CameraInfo isn't exist,needn't delete", "NA");
			return IVS_SUCCEED;
		}
		// 查看节点是否已经存在，存在则删除；不存在返回成功。
		do 
		{
			const char* pCameraCodeValue = xml.GetElemValue();
			if (NULL == pCameraCodeValue)
			{
				continue;
			}
			if (0 == strCameraCode.compare(pCameraCodeValue))
			{
				(void)xml.RemoveElem();
				(void)xml.Save();
				return IVS_SUCCEED;
			}
		} while (xml.NextElem());
		BP_RUN_LOG_INF("Broadcast CameraInfo isn't exist,needn't delete", "NA");

    }

	return iRet;
}


//设置轮训窗口
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetCameraSwitchWnd(ULONG ulWndID,ULONG ulStyle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Set CameraSwitch Wnd: %x, ulWndID: %d, ulStype:%u", this, ulWndID, ulStyle);
	if ((MIN_PANE_NUMBER > ulWndID) || (MAX_PANE_NUMBER < ulWndID))
	{
		return IVS_PARA_INVALID;
	}

	if (TYPE_TURNCRUISE_BUTH != ulStyle && TYPE_TURNCRUISE_LEFTBUTTON != ulStyle && TYPE_TURNCRUISE_RIFHTBUTTON != ulStyle && TYPE_TURNCRUISE_NOTHING != ulStyle)
	{
		return IVS_PARA_INVALID;
	}

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Delete TurnCruise Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_PARA_INVALID;
	}

	CVideoPane * pWnd = NULL;
	//入参窗格ID为0表示没有指定窗格，默认使用当前活动窗格
	pWnd = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWndID);
	if (NULL == pWnd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set CameraSwitch Wnd Failed.", "Reason:GetWnd is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	//只有轮训窗口才能设置按钮
	if (TYPE_WND_TURNCTUISE == pWnd->GetTypeWnd())
	{
		return (LONG)((CVideoPaneMgr*)m_pVideoPaneMgr)->SetTurnCruiseStyle(ulWndID,ulStyle);
	}
	else
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX CameraSwitch Wnd Failed.", "Reason:Wnd is not Turn Cruise");
		return IVS_FAIL;
	}
}

//取消轮训窗口
LONG CIVS_OCXPlayerCtrl::IVS_OCX_GetWndType(ULONG ulWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Get WndType Wnd: %x, ulWndID: %d", this, ulWndID);
	if ((MIN_PANE_NUMBER > ulWndID) || (MAX_PANE_NUMBER < ulWndID))
	{
		return IVS_PARA_INVALID;
	}

	if (NULL == m_pVideoPaneMgr)
	{
		return IVS_PARA_INVALID;
	}

	CVideoPane * pWnd = NULL;
	//入参窗格ID为0表示没有指定窗格，默认使用当前活动窗格
	pWnd = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWndID);
	if (NULL == pWnd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set Alarm Wnd Failed.", "Reason:GetWnd is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	return pWnd->GetTypeWnd();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetExDomainList(LPCTSTR pQueryInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	IVS_INT32 iRet = IVS_FAIL;
	IVS_CHAR* pExDomainList = NULL;
	CString strResult;
	iRet = IVS_SDK_GetExDomainList(m_iSDKSessionID, pQueryInfo,&pExDomainList);
	if (IVS_SUCCEED != iRet || NULL == pExDomainList)
	{
		BP_RUN_LOG_ERR(iRet, "Get ExDomainList SDK Interface is fail", "NA");
		if (NULL != pExDomainList)
		{
			IVS_SDK_ReleaseBuffer(pExDomainList);
		}
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	// 在content节点下面拼装一个resultcode节点
	std::string strInXml = pExDomainList;
	std::string strOutXml;
	iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "add result code failed", "xml error");
		IVS_SDK_ReleaseBuffer(pExDomainList);
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	(void)IVS_SDK_ReleaseBuffer(pExDomainList);
	strResult = strOutXml.c_str();
	return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ShareCatalog(LPCTSTR pShareInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_POINTER(pShareInfo,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ShareInfoXML: %s", this, pShareInfo);
	// TODO: Add your dispatch handler code here
	return  IVS_SDK_ShareCatalog(m_iSDKSessionID, pShareInfo);
}


BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetShareCatalogList(LPCTSTR pQueryInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	CString strResult;
	IVS_CHAR* pShareCatalogList = NULL;
	IVS_INT32 iRet = IVS_SDK_GetShareCatalogList(m_iSDKSessionID, pQueryInfo,&pShareCatalogList);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Share CatalogList SDK Interface is fail", "NA");
		if (NULL != pShareCatalogList)
		{
			IVS_SDK_ReleaseBuffer(pShareCatalogList);
		}
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

	// 在content节点下面拼装一个resultcode节点
	bool bIsServerRetError = false;
	if (NULL == pShareCatalogList)
	{
		iRet = IVS_XML_INVALID;
		BP_RUN_LOG_ERR(iRet, "Get Share CatalogList SDK Interface", "Server RetCode[%d] but content is null.", iRet);
		pShareCatalogList = "";//lint !e1776
		bIsServerRetError = true;
	}
	std::string strInXml = pShareCatalogList;
	std::string strOutXml;
	iRet = COCXXmlProcess::AddResultCodeUnderContent(iRet, strInXml, strOutXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "add result code failed", "xml error");
		if (!bIsServerRetError)
		{
			IVS_SDK_ReleaseBuffer(pShareCatalogList);
		}
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

	if (!bIsServerRetError)
	{
		(void)IVS_SDK_ReleaseBuffer(pShareCatalogList);
	}
	
	strResult = strOutXml.c_str();
	return strResult.AllocSysString();
}

//设置敞口类型
LONG CIVS_OCXPlayerCtrl::SetWndType(CVideoPane * pWnd,ULONG ulTyle)
{
	LONG lRet = IVS_FAIL;
	IVS_INFO_TRACE("OCX Set Wnd Type.OCX Address: %x, ulWndType: %d", this, ulTyle);
	CHECK_POINTER(pWnd,IVS_OPERATE_MEMORY_ERROR);

	switch(ulTyle)
	{
		//普通窗口
	case TYPE_WND_NOMAL:
		lRet = SetNoMalWnd(pWnd);
		break;
		//告警窗口
	case TYPE_WND_ALARM:
		lRet = SetAlarmWnd(pWnd);
		break;
		//轮训窗口
	case TYPE_WND_TURNCTUISE:
		lRet = SetTurnCruiseWnd(pWnd);
		break;
	case TYPE_WND_EMAP:
		lRet = SetEMapWnd(pWnd);
		break;
	default:
		break;
	}
	return lRet;
}

//普通实况
LONG CIVS_OCXPlayerCtrl::SetNoMalWnd(CVideoPane * pWnd)
{
	CHECK_POINTER(pWnd,IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("Set NoMalWnd: %x", this);
	LONG ulRet = IVS_FAIL;

	//如果是电子地图
	if (VIDEO_PLAY_STATE_EMAP == pWnd->GetPlayState())
	{
		pWnd->SetPlayState(VIDEO_PLAY_STATE_NOT_PLAYING);
		pWnd->ResetBarPosition();
	}
	//如果是告警状态，清除告警
	if (TYPE_WND_NOMAL== pWnd->GetTypeWnd())
	{
		BP_RUN_LOG_INF("IVS OCX Set NoMal succeed.", "Wnd is NoMal");
		return IVS_SUCCEED;
	}

	if (BOTTOMBAR_ALARM_CLOSE != pWnd->GetRealtimeAlarmState())
	{
		ulRet = static_cast<LONG>(SetAlarmState(pWnd->GetVideoWnd(), FALSE));	
	}

	//如果是轮训状态，清除轮训
	if (pWnd->GetTurnCruiseStatus())
	{
		//ulRet = pWnd->DeleteTurnCruiseStatus();
		pWnd->PostMessage(WM_TURNCRUISE_CLOSE,0,0);
	}

	pWnd->SetTypeWnd(TYPE_WND_NOMAL);

    // add by wanglei 00165153: DTS2013071301530
    // 对组轮巡中某个窗格设置告警窗口后，CU会调用OCX接口，把每个窗格设置为普通类型，设置后OCX需要对窗格Bar进行刷新导致
    pWnd->InvalidateBar();

	ulRet = IVS_SUCCEED;
	return ulRet;
}

//告警窗口
LONG CIVS_OCXPlayerCtrl::SetAlarmWnd(CVideoPane * pWnd)
{
	CHECK_POINTER(pWnd,IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("Set NoMalWnd: %x", this);

	if (TYPE_WND_ALARM == pWnd->GetTypeWnd())
	{
		BP_RUN_LOG_INF("IVS OCX Set AlarmWnd succeed.", "Wnd is Alarm");
		return IVS_SUCCEED;
	}

	if (TYPE_WND_NOMAL != pWnd->GetTypeWnd())
	{
		//不是Nomal窗口，先切换过来
		(void)SetWndType(pWnd,TYPE_WND_NOMAL);
	}

	//if (BOTTOMBAR_ALARM_CLOSE != pWnd->GetRealtimeAlarmState())
	//{
	//	BP_RUN_LOG_ERR(IVS_OCX_RET_VIDEOPANE_ISNOT_ALARM_ERROR, "Set Alarm Wnd Failed.", "Reason:GetWnd is NULL.");
	//	return IVS_OCX_RET_VIDEOPANE_ISNOT_ALARM_ERROR;
	//}

	ULONG ulRet = SetAlarmState(pWnd->GetVideoWnd(), TRUE);	
	if (IVS_SUCCEED != ulRet)
	{
		BP_RUN_LOG_ERR(static_cast<IVS_INT32>(ulRet), "Set Alarm State Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return static_cast<LONG>(ulRet);
	}

	pWnd->SetRealtimePlayPic(BUTTON_UNABLE);
	pWnd->SetTypeWnd(TYPE_WND_ALARM);
	return IVS_SUCCEED;
}

//轮训窗口
LONG CIVS_OCXPlayerCtrl::SetTurnCruiseWnd(CVideoPane * pWnd)
{
	CHECK_POINTER(pWnd,IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("Set NoMalWnd: %x", this);
	if (TYPE_WND_TURNCTUISE == pWnd->GetTypeWnd())
	{
		BP_RUN_LOG_INF("IVS OCX Set TurnCruise succeed.", "Wnd is TurnCruise");
		return IVS_SUCCEED;
	}
	if (TYPE_WND_NOMAL != pWnd->GetTypeWnd())
	{
		//如果不是普通窗口，就先设置为普通窗口
		(void)SetWndType(pWnd,TYPE_WND_NOMAL);
	}
	//如果是轮训窗口设置轮训面板
	if (pWnd->GetTurnCruiseStatus())
	{
		pWnd->CreatemTurnCruisePane();
	}

	pWnd->SetTurnCruiseStatus(true);
	pWnd->SetTypeWnd(TYPE_WND_TURNCTUISE);
	pWnd->InvalidateCruiseBar();

	return IVS_SUCCEED;
}

//电子地图窗口
LONG CIVS_OCXPlayerCtrl::SetEMapWnd(CVideoPane * pWnd)
{
	CHECK_POINTER(pWnd,IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("Set EMapWnd: %x", this);
	if (TYPE_WND_EMAP == pWnd->GetTypeWnd())
	{
		BP_RUN_LOG_INF("IVS OCX Set EMapWnd succeed.", "Wnd is EMapWnd");
		return IVS_SUCCEED;
	}
	if (TYPE_WND_NOMAL != pWnd->GetTypeWnd())
	{
		//如果不是普通窗口，就先设置为普通窗口
		(void)SetWndType(pWnd,TYPE_WND_NOMAL);
	}

	pWnd->SetTypeWnd(TYPE_WND_EMAP);

	return IVS_SUCCEED;
}

// 智能分析CPU查询性能
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetIACPUMemory(LPCTSTR pNodeCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	// 入参判空
	if (NULL == pNodeCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get IACPUMemory Failed","Reason:pNodeCode is NULL");//lint !e534
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,pNodeCode: %s", this,pNodeCode);
	IVS_INT32 iRet = COCXClientNetworkMgr::GetIACPUMemory(m_iSDKSessionID, pNodeCode, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Get IACPUMemory OCX Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

	return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_PlaybackFrameStepBackward(LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here

	CVideoPane * pVideoPane = GetVideoPanel((unsigned long)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "OCX Play back Frame Step Forward Failed","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	if (pVideoPane->IsSyncPlayBack())
	{
		CSyncPlayBackMgr &mgr = CSyncPlayBackMgr::Instance();
		return  mgr.StepPlayBack(); 
	}

	return IVS_SDK_PlayBackFrameStepBackward(m_iSDKSessionID, (IVS_ULONG)pVideoPane->GetPlayHandle());
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartSyncPlay(LPCTSTR pSyncPlayInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_POINTER(pSyncPlayInfo, IVS_PARA_INVALID);

	SyncPlayInfo SyncInfo;
	if (IVS_SUCCEED != COCXPlayBackXMLProcess::ParsePlayBackSync(pSyncPlayInfo, SyncInfo))
	{
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Start Sync Play Back", "Parse Input Xml fail");
		return IVS_XML_INVALID;
	}
	
	CSyncPlayBackMgr &SyncMgr = CSyncPlayBackMgr::Instance();
	PLAY_INFO_ITER iter = SyncInfo.m_PlayInfoList.begin();
	PLAY_INFO_ITER end  = SyncInfo.m_PlayInfoList.end();

	for (; iter != end; iter++)
	{
	    CPlayInfo &PlayInfo = *iter;
		CVideoPane *pPanel = GetVideoPanel(PlayInfo.ulWndID);
		ASSERT(NULL != pPanel);
		if (NULL == pPanel)//lint !e774
		{
			BP_RUN_LOG_ERR(IVS_FAIL, "Start Sync Play Back", 
				"Video Panel ID invalid %lu", PlayInfo.ulWndID);
			return IVS_FAIL;
		}
		pPanel->CreateBar();

		// 判断窗口是不是被局部放大占用，如果被占用，就关闭局部放大
		if ( BOTTOMBAR_MAGNIFY_OPEN == pPanel->GetDstMagnifyState())
		{
			CVideoPane* pSrcVideoPane = pPanel->GetMagnifySrcPane();
			if (NULL != pSrcVideoPane)
			{
				::SendMessage(pSrcVideoPane->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (LONG)pPanel);
			}
		}

		TIME_SPAN_ITER _iter = PlayInfo.m_TimeList.begin();
		TIME_SPAN_ITER _end  = PlayInfo.m_TimeList.end();
		for (; _iter != _end; _iter++)
		{
			TIME_INFO &ti = *_iter;
			REC_TIME_SPAN rts;
			rts.Start = ti.beg;
			rts.End   = ti.end;
			(void)SyncMgr.InsertSyncTask(pPanel, PlayInfo.CameraCode, SyncInfo.m_uProto, static_cast<float>(1.0), rts);
		}
 	}
	return SyncMgr.StartSyncPlay();
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopSyncPlay(void)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CSyncPlayBackMgr &SyncMgr = CSyncPlayBackMgr::Instance();
	SyncMgr.StopSyncPlay();
	return IVS_SUCCEED;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRecordTask()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	CString strResult;

	IVS_INT32 iRet = OCXRecordMgr::GetRecordTask(m_iSDKSessionID, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Record Task Failed.", "IVS SDK Get Record Task Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}


// 查询其他网元性能
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_QueryPerformance(LPCTSTR pNodeCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;
	if (NULL == pNodeCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX QueryPerformance Failed","Reason:pNodeCode is NULL");//lint !e534
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,pNodeCode: %s", this,pNodeCode);
	IVS_INT32 iRet = COCXClientNetworkMgr::QueryPerformance(m_iSDKSessionID, pNodeCode, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "QueryPerformance OCX Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}


//启动平台录像下载
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_StartPlatformDownload(LPCTSTR pCameraCode, LPCTSTR pStartTime, LPCTSTR pEndTime, LPCTSTR pDownloadParamXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	int iResultCode        = IVS_SUCCEED;
	unsigned long ulHandle = 0;
	CXml Xml; 

	if (NULL == pCameraCode || NULL == pStartTime || NULL == pEndTime || NULL == pDownloadParamXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX StartPlatformDownload Failed","");//lint !e534
		(void)COCXRecordXMLProcess::RecordDownLoadBuildXml(ulHandle, IVS_PARA_INVALID, Xml);
		unsigned int uiLen = 0;
		const char *pResult = Xml.GetXMLStream(uiLen);
		strResult = pResult;
		return strResult.AllocSysString();
	}

	IVS_INFO_TRACE("OCX Address: %x CameraCode[%s] StartTime[%s] EndTime[%s] DownloadParamXml[%s]", this, pCameraCode, pStartTime, pEndTime, pDownloadParamXml);

	IVS_DOWNLOAD_PARAM DLParam;
	memset(&DLParam, 0, sizeof(DLParam));
	if (IVS_SUCCEED == COCXRecordXMLProcess::RecordDownLoadParseXml(DLParam, pDownloadParamXml))
	{
		(void)CToolsHelp::Strncpy(DLParam.stTimeSpan.cStart, sizeof(DLParam.stTimeSpan.cStart) - 1, pStartTime, strlen(pStartTime));
		(void)CToolsHelp::Strncpy(DLParam.stTimeSpan.cEnd,   sizeof(DLParam.stTimeSpan.cEnd) - 1, pEndTime, strlen(pEndTime));
	
		iResultCode = IVS_SDK_StartPlatformDownload(m_iSDKSessionID, pCameraCode, &DLParam, &ulHandle);
	}
	else
	{
		iResultCode = IVS_XML_INVALID;
	}

	(void)COCXRecordXMLProcess::RecordDownLoadBuildXml(ulHandle, iResultCode, Xml);//lint !e732
	unsigned int uiLen = 0;
	const char *pResult = Xml.GetXMLStream(uiLen);
	strResult = pResult;

	// 清理内存，保证内存中不会有密码的存在
	memset((char*)pDownloadParamXml,0,strlen(pDownloadParamXml));

	return strResult.AllocSysString();
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopPlatformDownload(ULONG ulDownloadHandle)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	return IVS_SDK_StopPlatformDownload(m_iSDKSessionID, ulDownloadHandle);
}


// 启动容灾录像下载
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_StartDisasterRecoveryDownload (
    LPCTSTR pCameraCode,
    LPCTSTR pStartTime,
    LPCTSTR pEndTime,
    LPCTSTR pDownloadParamXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult;

    // TODO: Add your dispatch handler code here
    IVS_INFO_TRACE("OCX Address: %x,Disaster Recovery Download.", this);

    int iResultCode        = IVS_SUCCEED;
    unsigned long ulHandle = 0;
    CXml Xml; 

    if (NULL == pCameraCode || NULL == pStartTime || NULL == pEndTime || NULL == pDownloadParamXml)
    {
        (void)COCXRecordXMLProcess::RecordDownLoadBuildXml(ulHandle, IVS_PARA_INVALID, Xml);
        unsigned int uiLen = 0;
        const char *pResult = Xml.GetXMLStream(uiLen);
        strResult = pResult;
        return strResult.AllocSysString();
    }

    IVS_DOWNLOAD_PARAM DLParam;
    memset(&DLParam, 0, sizeof(DLParam));
    if (IVS_SUCCEED == COCXRecordXMLProcess::RecordDownLoadParseXml(DLParam, pDownloadParamXml))
    {
        (void)CToolsHelp::Strncpy(DLParam.stTimeSpan.cStart, sizeof(DLParam.stTimeSpan.cStart) - 1, pStartTime, strlen(pStartTime));
        (void)CToolsHelp::Strncpy(DLParam.stTimeSpan.cEnd,   sizeof(DLParam.stTimeSpan.cEnd) - 1, pEndTime, strlen(pEndTime));

        iResultCode = IVS_SDK_StartDisasterRecoveryDownload(m_iSDKSessionID, pCameraCode, &DLParam, &ulHandle);
    }
    else
    {
        iResultCode = IVS_XML_INVALID;
    }

    (void)COCXRecordXMLProcess::RecordDownLoadBuildXml(ulHandle, iResultCode, Xml);//lint !e732
    unsigned int uiLen = 0;
    const char *pResult = Xml.GetXMLStream(uiLen);
    strResult = pResult;

	// 清理内存，保证内存中不会有密码的存在
	memset((char*)pDownloadParamXml,0,strlen(pDownloadParamXml));

    return strResult.AllocSysString();
}

// 容灾录像停止下载
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopDisasterRecoveryDownload (
    ULONG ulDownloadHandle)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x,Disaster Recovery Download Stop.", this);

    // TODO: Add your dispatch handler code here
    return IVS_SDK_StopDisasterRecoveryDownload(m_iSDKSessionID, ulDownloadHandle);
}


BSTR CIVS_OCXPlayerCtrl::IVS_OCX_StartPUDownload(LPCTSTR pCameraCode, LPCTSTR pStartTime, LPCTSTR pEndTime, LPCTSTR pDownloadParamXml)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	// TODO: Add your dispatch handler code here
	int iResultCode        = IVS_SUCCEED;
	unsigned long ulHandle = 0;
	IVS_DOWNLOAD_PARAM DLParam;
	memset(&DLParam, 0, sizeof(DLParam));
	if (IVS_SUCCEED == COCXRecordXMLProcess::RecordDownLoadParseXml(DLParam, pDownloadParamXml))
	{
		(void)CToolsHelp::Strncpy(DLParam.stTimeSpan.cStart, sizeof(DLParam.stTimeSpan.cStart) - 1, pStartTime, strlen(pStartTime));
		(void)CToolsHelp::Strncpy(DLParam.stTimeSpan.cEnd,   sizeof(DLParam.stTimeSpan.cEnd) - 1, pEndTime, strlen(pEndTime));
		iResultCode = IVS_SDK_StartPUDownload(m_iSDKSessionID, pCameraCode, &DLParam, &ulHandle);
	}
	else
	{
		iResultCode = IVS_XML_INVALID;
	}

	CXml Xml;
	(void)COCXRecordXMLProcess::RecordDownLoadBuildXml(ulHandle, iResultCode, Xml);//lint !e732
	unsigned int uiLen = 0;
	const char *pResult = Xml.GetXMLStream(uiLen);
	strResult = pResult;
	if (NULL != pDownloadParamXml)
	{
		// 清理内存，保证内存中不会有密码的存在
		memset((char*)pDownloadParamXml,0,strlen(pDownloadParamXml));
	}

	return strResult.AllocSysString();
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopPUDownload(ULONG ulDownloadHandle)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	return IVS_SDK_StopPUDownload(m_iSDKSessionID, ulDownloadHandle);
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_DownloadPause(ULONG ulDownloadHandle)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	return IVS_SDK_DownloadPause(m_iSDKSessionID, ulDownloadHandle);
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_DownloadResume(ULONG ulDownloadHandle)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	return IVS_SDK_DownloadResume(m_iSDKSessionID, ulDownloadHandle);
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDownloadInfo(ULONG ulDownloadHandle)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	// TODO: Add your dispatch handler code here
	IVS_DOWNLOAD_INFO DownloadInfo;
	memset(&DownloadInfo, 0, sizeof(DownloadInfo));
	int ResultCode = IVS_SDK_GetDownloadInfo(m_iSDKSessionID, ulDownloadHandle, &DownloadInfo);

	CXml Xml;
    (void)COCXRecordXMLProcess::RecordDownLoadInfoBuildXml(ResultCode, DownloadInfo, Xml);
	unsigned int uiLen  = 0;
	const char *pResult = Xml.GetXMLStream(uiLen);
	strResult = pResult;
	return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartPUPlayBack(LPCTSTR pCameraCode, LPCTSTR pPlaybackParam, LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
    CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
    CHECK_POINTER(pPlaybackParam,IVS_PARA_INVALID);
    IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,PlaybackParam: %s,WndID: %ld", this,pCameraCode,pPlaybackParam,lWndID);

    if (strlen(pCameraCode) == 0)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start PU PlayBack Failed.", "Reason:pCameraCode is empty");
        return IVS_PARA_INVALID;
    }

    if (lWndID < MIN_PANE_NUMBER || lWndID > MAX_PANE_NUMBER)
    {
        BP_RUN_LOG_ERR(IVS_DATA_INVALID,"IVS OCX Start PU PlayBack Failed.", "Reason:win id is invalid");
        return IVS_DATA_INVALID;
    }

    IVS_PLAYBACK_PARAM stPlaybackParam;
    memset(&stPlaybackParam, 0, sizeof(IVS_PLAYBACK_PARAM));
    IVS_INT32 iRet = COCXPlayBackXMLProcess::GetPlaybackParam(pPlaybackParam, stPlaybackParam);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"IVS OCX Start PU PlayBack Failed.", "Reason: Get Playback Param Fail.");
		return iRet;
	}

    if (PROTOCOL_RTP_OVER_TCP != stPlaybackParam.uiProtocolType && PROTOCOL_RTP_OVER_UDP != stPlaybackParam.uiProtocolType)
    {
        BP_RUN_LOG_ERR(IVS_DATA_INVALID,"IVS OCX Start PU PlayBack Failed.", "Reason:uiProtocolType invalid = %d.", stPlaybackParam.uiProtocolType);
        return IVS_DATA_INVALID;
    }

    bool bRet = CToolsHelp::IsSpeedVaild(stPlaybackParam.fSpeed);
    if (!bRet)
    {
        BP_RUN_LOG_ERR(IVS_DATA_INVALID,"IVS OCX Start PU PlayBack Failed.", "Reason:fScale invalid. stPlaybackParam = %f.", stPlaybackParam.fSpeed);
        return IVS_DATA_INVALID;
    }

    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Start PU PlayBack Failed.", "Reason:m_pVideoPaneMgr is NULL.");
        return IVS_FAIL;
    }

    CVideoPane * pVideoPane = m_pVideoPaneMgr->GetWndPointerByID(static_cast<ULONG>(lWndID));

    //指针 句柄为空判断
    CHECK_POINTER(pVideoPane,IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pVideoPane->GetVideoWnd(),IVS_OPERATE_MEMORY_ERROR);

	pVideoPane->CreateBar();

	// 判断窗口是不是被局部放大占用，如果被占用，就关闭局部放大
	if ( BOTTOMBAR_MAGNIFY_OPEN == pVideoPane->GetDstMagnifyState())
	{
		CVideoPane* pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
		if (NULL != pSrcVideoPane)
		{
			::SendMessage(pSrcVideoPane->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (LONG)pVideoPane);
		}
	}

    if (VIDEO_PLAY_STATE_NOT_PLAYING != pVideoPane->GetPlayState())
    {
        BP_RUN_LOG_ERR(IVS_OCX_RET_VIDEOPANE_IS_PLAYING_ERROR, "IVS OCX Start PU PlayBack Failed.", "Reason:Window has played video.");
        return IVS_OCX_RET_VIDEOPANE_IS_PLAYING_ERROR;
    }
    BP_RUN_LOG_INF("IVS OCX Start PU PlayBack.", "window has played video.cameraid=%s.", pCameraCode);
    //当两个操作都成功后，要把窗格的一些状态修改
    m_csHWnd2OcxLock.Lock();
    (void)m_hWnd2OcxMap.insert(std::make_pair(pVideoPane->GetVideoWnd(), this));
    m_csHWnd2OcxLock.UnLock();

    //IVS_INT32 iRet = 0;
    ULONG ulHandle = 0;
    pVideoPane->SetPlayState(VIDEO_PLAY_STATE_VOD);
    pVideoPane->Invalidate(TRUE);

    iRet = IVS_SDK_StartPUPlayBack(m_iSDKSessionID, pCameraCode, &stPlaybackParam, pVideoPane->GetVideoWnd(), &ulHandle);
    if (IVS_SUCCEED != iRet)
    {
        pVideoPane->SetPlayState(VIDEO_PLAY_STATE_NOT_PLAYING);
        pVideoPane->Invalidate(TRUE);
        BP_RUN_LOG_ERR(iRet, "IVS OCX Start PU PlayBack Failed.", "Reason:SDK Start PU PlayBack Failed.");
        return iRet;
    }
    BP_RUN_LOG_INF("IVS OCX Call SDK StartPUPlayBack Succeed.", "CameraID=%s, hwnd=%d, playhandle=%lu.", pCameraCode, pVideoPane->GetVideoWnd(), ulHandle);

    (void)IVS_SDK_SetDisplayScale(m_iSDKSessionID, ulHandle, (int)(m_pVideoPaneMgr->GetDispalyScale()));

    if (m_pVideoPaneMgr->isAnalysisWndType())
    {
        pVideoPane->SetAnalysisBarIsStartPlay(TRUE);
    }

    pVideoPane->SetPlayHandle(ulHandle);
    pVideoPane->SetServiceType(SERVICE_TYPE_PU_PLAYBACK);
    pVideoPane->SetTitleButtonPic(true);
    pVideoPane->SetRealtimePlayPic(BUTTON_CLICKED);
    pVideoPane->SetCameraID((TCHAR *)pCameraCode);
    pVideoPane->EnableALLButton(TRUE);

	//前段录像回放需要禁止书签按钮
	pVideoPane->SetBookmarkButtonStatus(0);//lint !e747
    //(void)((CVideoPaneMgr*)m_pVideoPaneMgr)->SetDevName((ULONG)lWndID, pCameraCode);

	CGlobalPlayMgr &Mgr = CGlobalPlayMgr::Instance();
	(void)Mgr.RegPlayBackHandle(ulHandle, pVideoPane, pCameraCode);

    BP_RUN_LOG_INF("IVS OCX Start PU PlayBack End.", "Result = %d",  iRet);
	// 按钮过滤，一些按钮需要被过滤，例如局部放大需要只有一个才能用
	DoButtonFilter(lWndID);
	ResumeMagnifyStatus(pVideoPane);
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopPUPlayBack(LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
    IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);

    if (MIN_PANE_NUMBER > lWndID || MAX_PANE_NUMBER < lWndID)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop PU PlayBack Failed.", "Reason:WndID is Invalid.");
        return IVS_PARA_INVALID;
    }

    if (NULL == m_pVideoPaneMgr)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop PU PlayBack Failed.", "Reason:m_pVideoPaneMgr is null");
        return IVS_OPERATE_MEMORY_ERROR;
    }

    CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((ULONG)lWndID);
    if (NULL == pVideoPane)
    {
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop PU PlayBack Failed.","Reason:pVideoPane is NULL.");
        return IVS_OPERATE_MEMORY_ERROR;
    }

	/*****************************************************
	当关闭实况时,关闭随路语音，但是如果有复用，则，当该
	窗格的随路语音打开，则要关闭当前随路语音并打开其他随路语音
	******************************************************/
	//if(NULL != pVideoPane->GetVideoRealtimeBar())
	//{
	//	// 记录下该窗格是否打开随路语音
	//	if(BOTTOMBAR_SOUND_OPEN == pVideoPane->GetRealtimeSoundState())
 //       {
 //           pVideoPane->GetVideoRealtimeBar()->StopSound();
 //           pVideoPane->GetVideoRealtimeBar()->DeleteSoundCtrl();
	//	}
	//}

	//IVS_SDK_StopWndBusinessRestore( pVideoPane->GetVideoWnd() );
	(void)SetResumeState(pVideoPane->GetVideoWnd(), FALSE);

    ULONG ulRet = StopPlayBackByPane(pVideoPane, RECORD_METHOD_PU);

    BP_RUN_LOG_INF("IVS_OCX_StopPUPlayBack", "IVS_OCX_StopPUPlayBack end. result = %lu", ulRet);
    return static_cast<LONG>(ulRet);
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetTVWallTempScenario(LPCTSTR pScenarioXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(pScenarioXml,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ScenarioXml: %s", this,pScenarioXml);	
	/*char* utf8str = CToolsHelp::ANSIToUTF8(pScenarioXml);*/
	m_csTVWallTempScenarioLock.Lock();
	IVS_INT32 iRet = CFavorite::SetTVwallScenarioFavorite(pScenarioXml);
	m_csTVWallTempScenarioLock.UnLock();
// 	free(utf8str);
// 	utf8str = NULL;
	return iRet;

}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetTVWallTempScenario(ULONG pScenarioID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// TODO: Add your dispatch handler code here
	IVS_INFO_TRACE("OCX Address: %x", this);
	m_csTVWallTempScenarioLock.Lock();
	std::string str = CFavorite::GetTVWallTempScenario(CToolsHelp::Int2Str((int)pScenarioID));
	m_csTVWallTempScenarioLock.UnLock();
// 	char* ansistr = CToolsHelp::UTF8ToANSI(str.c_str());
// 	std::string str1(ansistr);
// 	free(ansistr);
// 	ansistr = NULL;
	return _com_util::ConvertStringToBSTR(str.c_str());
}

// 查询录像备份任务列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRecordBackupTaskList (LPCTSTR pQueryInfo)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pQueryInfo,NULL);
	CString strResult;
	IVS_INT32 iRet = OCXRecordMgr::GetRecordBackupTaskList(m_iSDKSessionID, pQueryInfo, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get Record Backup Task List", "NA");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();//lint !e831
}//lint !e831
//启动/继续录像备份任务
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartRecordBackupTask (LPCTSTR pQueryInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pQueryInfo)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start Record Backup Task Failed.", "Reason:pQueryInfo is NULL");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pQueryInfo: %s", this,pQueryInfo);
	IVS_INT32 iRet = IVS_SDK_StartRecordBackupTask(m_iSDKSessionID, pQueryInfo);
	return iRet;
}
// 暂停/停止录像备份任务
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopRecordBackupTask (LPCTSTR pQueryInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pQueryInfo)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop Record Backup Task Failed.", "Reason:pQueryInfo is NULL");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pQueryInfo: %s", this,pQueryInfo);
	IVS_INT32 iRet = IVS_SDK_StopRecordBackupTask(m_iSDKSessionID, pQueryInfo);
	return iRet;
}

// 删除录像备份任务
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DelRecordBackupTask (LPCTSTR pQueryInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pQueryInfo)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Delete Record Backup Task Failed.", "Reason:pQueryInfo is NULL");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pQueryInfo: %s", this,pQueryInfo);
	IVS_INT32 iRet = IVS_SDK_DeleteRecordBackupTask(m_iSDKSessionID, pQueryInfo);
	return iRet;
}

// 获取设备驱动列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDriverList(LPCTSTR pReqXml)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	return strResult.AllocSysString();
}//lint !e715

// 获取设备模板列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetTemplateList(LPCTSTR pReqXml)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	// TODO: 在此添加调度处理程序代码

	return strResult.AllocSysString();
}//lint !e715

// 新增录像备份任务
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AddRecordBackupTask(LPCTSTR pBackupTask)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_POINTER(pBackupTask,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pBackupTask: %s", this, pBackupTask);
	return OCXRecordMgr::AddRecordBackupTask(m_iSDKSessionID, pBackupTask);
}

// 设置事件接受的OCX对象
void CIVS_OCXPlayerCtrl::IVS_OCX_SetEventReceiver(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	CIVS_OCXPlayerCtrl::m_pOCX = this;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetWndDrag(ULONG ulWndID,ULONG ulEnable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Pane Drag Disable: %x, ulWindID: %d", this, ulWndID);
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Pane Drag Disable Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_PARA_INVALID;
	}

	CVideoPane * pWnd = NULL;
	//入参窗格ID为0表示没有指定窗格，默认使用当前活动窗格
	pWnd = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWndID);
	if (NULL == pWnd)//lint !e831
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set Pane Drag Disable Failed.", "Reason:GetWnd is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CHECK_POINTER(pWnd,IVS_OPERATE_MEMORY_ERROR);//lint !e831 !e774
	//如果是告警状态，清除告警
	pWnd->IsDrag(ulEnable);
	return IVS_SUCCEED;
}

void CIVS_OCXPlayerCtrl::IVS_OCX_RefreshWnd(ULONG ulRefreshType)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//IVS_INFO_TRACE("OCX Address: %x", this);
	if (m_pVideoPaneMgr != NULL)
	{
		if (0 == ulRefreshType)
		{
			m_pVideoPaneMgr->HideAllBarOnce();
			m_pVideoPaneMgr->ResetBarPosition();
		}
		else if (1 == ulRefreshType)
		{
			m_pVideoPaneMgr->HideAllBarOnce();
			//m_pVideoPaneMgr->ResetBarPosition();
			//m_pVideoPaneMgr->ShowToolBar(false);
			//m_pVideoPaneMgr->ShowTitleBar(false);
		}
		else if (2 == ulRefreshType)//自动获取父窗口句柄刷新
		{
			if(NULL!=m_pInPlaceSite)
			{
				HWND hwndParent = 0;
				HRESULT result = m_pInPlaceSite->GetWindow(&hwndParent);
				if(0==result)
				{
					m_pVideoPaneMgr->SetAllBarWindowsLong((ULONG)hwndParent);
				}

			}
		}
		else//外部传入父窗口句柄刷新
		{
			m_pVideoPaneMgr->SetAllBarWindowsLong(ulRefreshType);
		}
	}
	return;
	/*
	std::list<CIVS_OCXPlayerCtrl *>::iterator iter = CIVS_OCXPlayerCtrl::m_OcxList.begin();
	for (; iter!=CIVS_OCXPlayerCtrl::m_OcxList.end(); iter++)
	{
		CIVS_OCXPlayerCtrl *pOcx = *iter;
		if (pOcx == nullptr )
		{
			continue;
		}
		CVideoPaneMgr* pVideoPaneMgr =  pOcx->GetVideoPaneMgr();
		if (pVideoPaneMgr != nullptr)
		{
			if (0 == ulRefreshType)
			{
				pVideoPaneMgr->ResetBarPosition();
			}
			else if (1 == ulRefreshType)
			{
				pVideoPaneMgr->ResetBarPosition();
				pVideoPaneMgr->ShowToolBar(false);
				pVideoPaneMgr->ShowTitleBar(false);
			}
		}
	}
	*/
}

// 查询告警联动动作详情
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAlarmLinkageAction(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Modify Alarm Linkage Failed.", "Reason:pReqXml is NULL.");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXAlarmLinkageMgr::GetAlarmLinkageAction(m_iSDKSessionID, pReqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Alarm Linkage Action fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}


// 添加轮巡计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AddCameraSwitchPlan(LPCTSTR pSwitchPlanInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pSwitchPlanInfo,NULL);
	IVS_INFO_TRACE("OCX Address: %x,pSwitchPlanInfo: %s", this, pSwitchPlanInfo);

	std::string pCameraSwitchID;
	IVS_INT32 iRet = CFavorite::GetCameraSwitchID(pSwitchPlanInfo, pCameraSwitchID);
	if (pCameraSwitchID.empty())
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Add CameraSwitch Plan", "find CameraSwitchID failed");
		return IVS_PARA_INVALID;
	}
	(void)IVS_OCX_DelCameraSwitchPlan(pCameraSwitchID.c_str());

	iRet = CFavorite::AddCameraSwitchPlan(pSwitchPlanInfo);
	return iRet;
}

// 删除轮巡计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DelCameraSwitchPlan(LPCTSTR pCameraSwitchID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraSwitchID,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pCameraSwitchID: %s", this, pCameraSwitchID);
	return CFavorite::DelCameraSwitchPlan(pCameraSwitchID);
}

// 获取轮巡计划列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCameraSwitchPlanList(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	std::string strCameraSwitchPlanList;
	IVS_INT32 iResultCode = CFavorite::GetCameraSwitchPlanList(strCameraSwitchPlanList);
	std::string str;
	CFavorite::BulidGetScenarioInfoListResult(iResultCode, strCameraSwitchPlanList, str);
	//BP_RUN_LOG_INF("get config","%s", str.c_str()); //log

	return _com_util::ConvertStringToBSTR(str.c_str());
}

// 获取轮巡计划
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCameraSwitchPlan(LPCTSTR pCameraSwitchID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraSwitchID, NULL);
	IVS_INFO_TRACE("OCX Address: %x,pCameraSwitchID: %s", this, pCameraSwitchID);
	std::string strCameraSwitchPlan;
	IVS_INT32 iResultCode = CFavorite::GetCameraSwitchPlan(pCameraSwitchID, strCameraSwitchPlan);
	std::string str;
	CFavorite::BulidGetScenarioInfoResult(iResultCode, strCameraSwitchPlan, str);
	BP_RUN_LOG_INF("IVS OCX Get Camera Switch Info End.","Result: %s", str.c_str());
	return _com_util::ConvertStringToBSTR(str.c_str());
}

// 锁定录像
LONG CIVS_OCXPlayerCtrl::IVS_OCX_LockRecord(LPCTSTR pLockXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pLockXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Lock Record Failed.", "Reason:pLockXml is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pLockXml: %s", this,pLockXml);

	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_FAIL);
	// 输入参数
	std::string strLockXml =  pLockXml;
	// 输出参数
	std::string strCameraCode;
	std::string strNVRCode;
	std::string strDomainCode;
	IVS_RECORD_LOCK_INFO pRecordLockInfo;
	memset(&pRecordLockInfo,0,sizeof(IVS_RECORD_LOCK_INFO));
	if(IVS_SUCCEED!=COCXRecordXMLProcess::LockRecordGetXML(strLockXml,strDomainCode,strNVRCode,strCameraCode,&pRecordLockInfo))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Lock Record Failed.", "Reason:Lock Record Get XML Fail.");
		return iRet;
	}

	iRet = IVS_SDK_LockRecord(m_iSDKSessionID,strCameraCode.c_str(),&pRecordLockInfo);
	return iRet;
}

// 解锁录像
LONG CIVS_OCXPlayerCtrl::IVS_OCX_UnlockRecord(LPCTSTR pUnlockXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pUnlockXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX UnLock Record Failed.", "Reason:pUnlockXml is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pLockXml: %s", this,pUnlockXml);
	
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_FAIL);
	// 输入参数
	std::string strUnLockXml =  pUnlockXml;
	// 输出参数
	std::string strCameraCode;
	std::string strNVRCode;
	std::string strDomainCode;
	IVS_RECORD_LOCK_INFO pRecordLockInfo;
	memset(&pRecordLockInfo,0,sizeof(IVS_RECORD_LOCK_INFO));
	if(IVS_SUCCEED!=COCXRecordXMLProcess::UnLockRecordGetXML(strUnLockXml,strDomainCode,strNVRCode,strCameraCode,&pRecordLockInfo))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX UnLock Record Failed.", "Reason:UnLock Record Get XML Fail.");
		return iRet;
	}

	iRet = IVS_SDK_UnLockRecord(m_iSDKSessionID,strCameraCode.c_str(),&pRecordLockInfo);
	return iRet;
}



BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetVideoNumAndResource(LPCTSTR pCameraCode)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pCameraCode)
	{
		COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
		return strResult.AllocSysString();
	}
	// TODO: Add your dispatch handler code here
	IVS_BOOL bReachLimit = FALSE;
	IVS_BOOL bWhetherCited = FALSE;
	IVS_INT32 iRet = IVS_SDK_GetVideoNumAndResource(m_iSDKSessionID, pCameraCode,&bReachLimit,&bWhetherCited);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get VideoNum And Resource SDK Interface is fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	//生成返回的XML
	std::string strOutXml;
	iRet = COCXXmlProcess::GetVideoNumAndResourceXML(iRet,bReachLimit, bWhetherCited, strOutXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get return XML failed", "xml error");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	strResult = strOutXml.c_str();
	return strResult.AllocSysString();
}

// 锁定录像(备份服务器)
LONG CIVS_OCXPlayerCtrl::IVS_OCX_LockBackupRecord(LPCTSTR pLockXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pLockXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Lock Backup Record Failed.", "Reason:pLockXml is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pLockXml: %s", this,pLockXml);
	
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_FAIL);
	// 输入参数
	std::string strLockXml =  pLockXml;
	// 输出参数
	std::string strCameraCode;
	std::string strNVRCode;
	std::string strDomainCode;
	IVS_RECORD_LOCK_INFO pRecordLockInfo;
	memset(&pRecordLockInfo,0,sizeof(IVS_RECORD_LOCK_INFO));
	if(IVS_SUCCEED!=COCXRecordXMLProcess::LockRecordGetXML(strLockXml,strDomainCode,strNVRCode,strCameraCode,&pRecordLockInfo))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Lock Backup Record Failed.", "Reason:Lock Record Get XML Fail.");
		return iRet;
	}

	iRet = IVS_SDK_LockBackupRecord(m_iSDKSessionID,strDomainCode.c_str(),strNVRCode.c_str(),strCameraCode.c_str(),&pRecordLockInfo);
	return iRet;
}

// 解锁录像(备份服务器)
LONG CIVS_OCXPlayerCtrl::IVS_OCX_UnlockBackupRecord(LPCTSTR pUnlockXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pUnlockXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX UnLock Backup Record Failed.", "Reason:pUnlockXml is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pLockXml: %s", this,pUnlockXml);
	
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_FAIL);
	// 输入参数
	std::string strUnLockXml =  pUnlockXml;
	// 输出参数
	std::string strCameraCode;
	std::string strNVRCode;
	std::string strDomainCode;
	IVS_RECORD_LOCK_INFO pRecordLockInfo;
	memset(&pRecordLockInfo,0,sizeof(IVS_RECORD_LOCK_INFO));
	if(IVS_SUCCEED!=COCXRecordXMLProcess::UnLockRecordGetXML(strUnLockXml,strDomainCode,strNVRCode,strCameraCode,&pRecordLockInfo))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX UnLock Backup Record Failed.", "Reason:UnLock Record Get XML Fail.");
		return iRet;
	}

	iRet = IVS_SDK_UnlockBackupRecord(m_iSDKSessionID,strDomainCode.c_str(),strNVRCode.c_str(),strCameraCode.c_str(),&pRecordLockInfo);
	return iRet;
}

// 修改录像锁定信息(备份服务器)
LONG CIVS_OCXPlayerCtrl:: IVS_OCX_ModLockBackupRecord(LPCTSTR pLockXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pLockXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Mod Lock Backup Record Failed.", "Reason:pLockXml is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pLockXml: %s", this,pLockXml);

	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_FAIL);
	// 输入参数
	std::string strLockXml =  pLockXml;
	// 输出参数
	std::string strCameraCode;
	std::string strNVRCode;
	std::string strDomainCode;
	IVS_RECORD_LOCK_INFO pRecordLockInfo;
	memset(&pRecordLockInfo,0,sizeof(IVS_RECORD_LOCK_INFO));
	if(IVS_SUCCEED!=COCXRecordXMLProcess::ModLockRecordGetXML(strLockXml,strDomainCode,strNVRCode,strCameraCode,&pRecordLockInfo))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Mod Lock Backup Record Failed.", "Reason:Mod Lock Record Get XML Fail.");
		return iRet;
	}

	iRet = IVS_SDK_ModLockBackupRecord(m_iSDKSessionID,strDomainCode.c_str(),strNVRCode.c_str(),strCameraCode.c_str(),&pRecordLockInfo);
	return iRet;
}

// 查询平台运动检测幅度数据
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetMotionRangeDataPlatform(LPCTSTR pCameraCode,LPCTSTR pStartTime,LPCTSTR pEndTime,LONG lFromIndex,LONG lToIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Motion Range Data Platform Failed", "Reason:pCameraCode is NULL.");
		return NULL;
	}
	if (NULL == pStartTime)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Motion Range Data Platform Failed.", "Reason:pStartTime is NULL.");
		return NULL;
	}
	if (NULL == pEndTime)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get Motion Range Data Platform Failed.", "Reason:pEndTime is NULL.");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,StartTime: %s,EndTime: %s,FromIndex: %lu,lToIndex: %lu",this,pCameraCode,pStartTime,pEndTime,lFromIndex,lToIndex);

	// pCameraCode是透传到SDK，其它参数需要解析到结构体中
	CString strResult = "";
	IVS_UINT32 uiXmlLen = 0;
	CXml xmlReq;
	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);

	//分配内存
	IVS_UINT32 uiTotal = static_cast<IVS_UINT32>(lToIndex - lFromIndex);
	IVS_UINT32 uiBufSize = sizeof(IVS_MOTION_RANGE_DATA_LIST)+(sizeof(IVS_MOTION_RANGE_DATA_INFO))*uiTotal;
	IVS_CHAR* pMotionRangeDataBuf = IVS_NEW(pMotionRangeDataBuf, uiBufSize);
	if (NULL == pMotionRangeDataBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("IVS OCX Get Motion Range Data Platform", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult.AllocSysString();
	}
	memset(pMotionRangeDataBuf, 0, uiBufSize);
	IVS_MOTION_RANGE_DATA_LIST* pMotionRangeData = (IVS_MOTION_RANGE_DATA_LIST*)pMotionRangeDataBuf; //lint !e826

	IVS_TIME_SPAN stTimeSpan;
	memset(&stTimeSpan,0,sizeof(IVS_TIME_SPAN));
	if(!CToolsHelp::Memcpy(stTimeSpan.cStart,IVS_TIME_LEN,pStartTime,strlen(pStartTime)))
	{
		IVS_DELETE(pMotionRangeDataBuf, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS OCX Get Motion Range Data Platform Failed.", "Reason:stTimeSpan.cStart Memcpy error.");
		COCXXmlProcess::GetErrString(strResult,IVS_ALLOC_MEMORY_ERROR);
		return strResult.AllocSysString();
	}
	if(!CToolsHelp::Memcpy(stTimeSpan.cEnd,IVS_TIME_LEN,pEndTime,strlen(pEndTime)))
	{
		IVS_DELETE(pMotionRangeDataBuf, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR,"IVS OCX Get Motion Range Data Platform Failed", "Reason:stTimeSpan.cEnd Memcpy error.");
		COCXXmlProcess::GetErrString(strResult,IVS_ALLOC_MEMORY_ERROR);
		return strResult.AllocSysString();
	}
	IVS_INDEX_RANGE stIndex;
	memset(&stIndex,0,sizeof(IVS_INDEX_RANGE));
	stIndex.uiFromIndex = static_cast<IVS_UINT32>(lFromIndex);
	stIndex.uiToIndex = static_cast<IVS_UINT32>(lToIndex);

	iRet = IVS_SDK_GetMotionRangeDataPlatform(m_iSDKSessionID,pCameraCode,&stTimeSpan,&stIndex,pMotionRangeData,uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pMotionRangeDataBuf, MUILI);

		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Motion Range Data Platform Failed.", "Reason:SDK IVS OCX Get Motion Range Data Platform Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	//拼装响应消息结构体为XML
	iRet = COCXMotionDetecMgrXMLProcess::ParseGetMotionRangeDataPlatformXml(pStartTime,pEndTime,pMotionRangeData,iRet,xmlRsp);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pMotionRangeDataBuf, MUILI);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Motion Range Data Platform Failed", "Get Motion Range Data Platform Response XML Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	IVS_DELETE(pMotionRangeDataBuf, MUILI);

	const char* pXml = xmlRsp.GetXMLStream(uiXmlLen);
	if (NULL == pXml)
	{
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	strResult = pXml;

	BP_RUN_LOG_INF("IVS OCX Get Motion Range Data Platform.", "Result:%s", pXml);
	return strResult.AllocSysString();
}

//添加书签(备份服务器)
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AddBookmarkForBackup(LPCTSTR pDomainCode,LPCTSTR pNVRCode,LPCTSTR pCameraCode,LPCTSTR pBookmarkName,LPCTSTR pBookmarkTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pDomainCode,IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode,IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	//CHECK_POINTER(pBookmarkName,IVS_PARA_INVALID);//和SE确认，书签名称可以为空
	CHECK_POINTER(pBookmarkTime,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pDomainCode: %s,pNVRCode: %s,CameraCode: %s,BookmarkName: %s,BookmarkTime: %s", this,pDomainCode,pNVRCode,pCameraCode,pBookmarkName,pBookmarkTime);
	IVS_INT32 iRet = IVS_FAIL;
	iRet = IVS_SDK_AddBookmarkForBackup(m_iSDKSessionID,pDomainCode,pNVRCode,pCameraCode,pBookmarkName,pBookmarkTime);
	return iRet;
}

//查询书签列表(备份服务器)
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetBookmarkListForBackup(LPCTSTR pQueryXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pQueryXml,NULL);
	IVS_INFO_TRACE("OCX Address: %x,QueryXml: %s", this,pQueryXml);
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlReq;
	CXml xmlRsp;
	IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	if (NULL == xmlReq.Parse(pQueryXml))
	{
		iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Bookmark List For Backup Failed", "Reason:Parse XML Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	IVS_INT32 iFieldNum = 0;
	iFieldNum = COCXXmlProcess::GetFieldNum(xmlReq);

	//分配内存
	IVS_VOID* pQueryFiledBuff = NULL;
	iRet = COCXXmlProcess::GetUnifiedFormatQueryParseXML(pQueryFiledBuff,xmlReq,iFieldNum);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Bookmark List For Backup Failed.", "Reason:Parse XML Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	IVS_QUERY_UNIFIED_FORMAT* pQueryUnifiedFormat = (IVS_QUERY_UNIFIED_FORMAT*)pQueryFiledBuff;

	iRet = COCXRecordXMLProcess::GetQueryUnifiedCameraCode(xmlReq,cCameraCode);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Bookmark List For Backup Failed", "Reason:Get QueryUnified CameraCode Xml Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	IVS_UINT32 uiTotal = (pQueryUnifiedFormat->stIndex).uiToIndex - (pQueryUnifiedFormat->stIndex).uiFromIndex;
	IVS_UINT32 uiBufSize = sizeof(IVS_RECORD_BOOKMARK_INFO_LIST)+sizeof(IVS_RECORD_BOOKMARK_INFO)*uiTotal;
	IVS_CHAR* pRecordListBuf = NULL;
	try{
		pRecordListBuf = new IVS_CHAR[uiBufSize];
	}
	catch(...)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		COCXXmlProcess::GetErrorResultXML(iRet,xmlRsp);
		BP_RUN_LOG_INF("IVS OCX Get Bookmark List For Backup", "ErrorCode =%d", iRet);
		strResult = xmlRsp.GetXMLStream(uiXmlLen);
		return strResult.AllocSysString();
	}
	memset(pRecordListBuf, 0, uiBufSize);

	IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkInfoList = (IVS_RECORD_BOOKMARK_INFO_LIST*)pRecordListBuf;//lint !e826
	iRet = IVS_SDK_GetBookmarkListForBackup(m_iSDKSessionID,pQueryUnifiedFormat,pBookmarkInfoList,uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		IVS_DELETE(pRecordListBuf, MUILI);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Bookmark List For Backup Failed.", "Reason:SDK Get Bookmark List Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	//拼装响应消息结构体为XML
	iRet = COCXRecordXMLProcess::GetRecordBookmarkListGetXML(cCameraCode,pBookmarkInfoList,iRet,xmlRsp);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		IVS_DELETE(pRecordListBuf, MUILI);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Bookmark List For Backup Failed.", "Reason:Parse pBookmarkInfoList To Xml Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	IVS_DELETE(pQueryFiledBuff, MUILI);
	IVS_DELETE(pRecordListBuf, MUILI);

	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult.AllocSysString();
}

//修改书签(备份服务器)
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyBookmarkForBackup(LPCTSTR pDomainCode,LPCTSTR pNVRCode,LPCTSTR pCameraCode,ULONG ulBookmarkID,LPCTSTR pNewBookmarkName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pDomainCode,IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode,IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	//CHECK_POINTER(pNewBookmarkName,IVS_PARA_INVALID);//和SE确认，书签名称可以为空
	IVS_INFO_TRACE("OCX Address: %x,pDomainCode: %s,pNVRCode: %s,CameraCode: %s,BookmarkID: %lu,NewBookmarkName: %s", this,pDomainCode,pNVRCode,pCameraCode,ulBookmarkID,pNewBookmarkName);
	IVS_INT32 iRet = IVS_FAIL;
	iRet = IVS_SDK_ModifyBookmarkForBackup(m_iSDKSessionID,pDomainCode,pNVRCode,pCameraCode,ulBookmarkID,pNewBookmarkName);
	return iRet;
}

//删除书签(备份服务器)
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteBookmarkForBackup(LPCTSTR pDomainCode,LPCTSTR pNVRCode,LPCTSTR pCameraCode,ULONG ulBookmarkID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pDomainCode,IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode,IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pDomainCode: %s,pNVRCode: %s,CameraCode: %s,BookmarkID: %lu", this,pDomainCode,pNVRCode,pCameraCode,ulBookmarkID);
	IVS_INT32 iRet = IVS_FAIL;
	iRet = IVS_SDK_DeleteBookmarkForBackup(m_iSDKSessionID,pDomainCode,pNVRCode,pCameraCode,ulBookmarkID);
	return iRet;
}



BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryDevReport(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryDevReport(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryDevReportDetail(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryDevReportDetail(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryRealtimeReport(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryRealtimeReport(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryRealtimeReportDetail(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryRealtimeReportDetail(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryQDReport(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryQDReport(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryQDReportDetail(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryQDReportDetail(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryRecordPlanReport(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryRecordPlanReport(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryRecordPlanReportDetail(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryRecordPlanReportDetail(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryRecordIntegralityReport(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryRecordIntegralityReport(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryRecordIntegralityReportDetail(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryRecordIntegralityReportDetail(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryPacketLossRateReport(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryPacketLossRateReport(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryPacketLossRateReportDetail(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryPacketLossRateReportDetail(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUQueryDiskUsageRateReport(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add User Failed.", "Reason:pReqXml is NULL.");
        return NULL;
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml :%s", this, pReqXml);

    CString strResult = COCXReportMgr::OMUQueryDiskUsageRateReport(m_iSDKSessionID, pReqXml);
    return strResult.AllocSysString();
}

// 设置设备组备注信息
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetDevGroupRemark(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX SetDevGroupRemarkp Failed.", "Reason:pReqXmlis NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX SetDevGroupRemarkp Failed.", "Reason:Parse pReqXml Failed.");
		return IVS_XML_INVALID;
	}

	IVS_INT32 iRet = COCXDeviceGroupMgr::SetDevGroupRemark(m_iSDKSessionID, xmlReq);

	return iRet;
}

// 查询设备组备注信息
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDevGroupRemark(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX GetDevGroupRemark Failed.", "Reason:pReqXmlis NULL.");
		COCXXmlProcess::GetErrString(strResult,IVS_PARA_INVALID);
	}
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);

	CXml xmlReq;
	if (!xmlReq.Parse(pReqXml))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX GetDevGroupRemark Failed.", "Reason:Parse pReqXml Failed.");
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
	}

	COCXDeviceGroupMgr::GetDevGroupRemark(m_iSDKSessionID, xmlReq, strResult);

	return strResult.AllocSysString();
}

// 新增告警通知规则
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUAddAlarmLinkageRule(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX OMUAddAlarmLinkageRule Failed","Reason:pReqXml is NULL");//lint !e534
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXClientNetworkMgr::AddAlarmLinkageRule(m_iSDKSessionID, pReqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX OMUAddAlarmLinkageRule Failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 修改告警通知规则接口
LONG CIVS_OCXPlayerCtrl::IVS_OCX_OMUModAlarmLinkageRule(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);

	return COCXClientNetworkMgr::ModAlarmLinkageRule(m_iSDKSessionID, pReqXml);
}

// 删除告警通知规则
LONG CIVS_OCXPlayerCtrl::IVS_OCX_OMUDelAlarmLinkageRule(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);

	return COCXClientNetworkMgr::DelAlarmLinkageRule(m_iSDKSessionID, pReqXml);
}

// 获取告警通知规则列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUGetAlarmLinkageRuleList(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX OMUGetAlarmLinkageRuleList Failed","Reason:pReqXml is NULL");//lint !e534
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXClientNetworkMgr::GetAlarmLinkageRuleList(m_iSDKSessionID, pReqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "OMUGetAlarmLinkageRuleList fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
	}
	return strResult.AllocSysString();
}

// 获取告警通知规则详情
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUGetAlarmLinkageRuleDetail(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX OMUGetAlarmLinkageRuleDetail Failed","Reason:pReqXml is NULL");//lint !e534
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXClientNetworkMgr::GetAlarmLinkageRuleDetail(m_iSDKSessionID, pReqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmLinkageRuleDetail OCX Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 根据网元获取设备类型
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_OMUGetDevTypeByNodeCode(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX OMUGetDevTypeByNodeCode Failed","Reason:pNodeCode is NULL");//lint !e534
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXClientNetworkMgr::GetDevTypeByNodeCode(m_iSDKSessionID, pReqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetDevTypeByNodeCode OCX Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 查询开启预录的摄像机列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetPrerecordCameraList()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	
	CString strResult;
	IVS_CHAR* pPreRecordCameraListXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetPrerecordCameraList(m_iSDKSessionID,&pPreRecordCameraListXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get PreRecord CameraList Fail", "NA");
		if (NULL != pPreRecordCameraListXml)
		{
			(void)IVS_SDK_ReleaseBuffer(pPreRecordCameraListXml);
		}
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	iRet = COCXXmlProcess::GetResultXML(iRet, pPreRecordCameraListXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetResultXML to CU is fail", "NA");
		(void)IVS_SDK_ReleaseBuffer(pPreRecordCameraListXml);
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	(void)IVS_SDK_ReleaseBuffer(pPreRecordCameraListXml);
	return strResult.AllocSysString();
}

//开始/停止水印校验
LONG CIVS_OCXPlayerCtrl::IVS_OCX_VerifyWatermark(LONG lWndID, ULONG ulVerifyWatermark)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,ulWndID: %lu", this,lWndID);

	IVS_INT32 iRet = IVS_FAIL;
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Get Media Info Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_PARA_INVALID;
	}
	CVideoPane* pWnd = m_pVideoPaneMgr->GetWndPointerByID((ULONG)lWndID);
	if (NULL == pWnd)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get Media Info Failed.","Reason:Get CVideoPane Error");
		return IVS_PARA_INVALID;
	}
	unsigned long ulHandle = pWnd->GetPlayHandle();
	iRet = IVS_SDK_SetWaterMarkVerify(m_iSDKSessionID, ulHandle, (IVS_INT32)ulVerifyWatermark);
	if (IVS_SUCCEED == iRet)
	{
		pWnd->SetWaterMarkStatus((ULONG)(!ulVerifyWatermark));
	}

	return iRet;
}

//水印告警确认
LONG CIVS_OCXPlayerCtrl::IVS_OCX_WatermarkAlarmConfirm(LPCTSTR pConfirmInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	if(NULL == pConfirmInfo)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Watermark Alarm Confirm Failed.", "Confirm Info is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_WATERMARK_ALARM_CONFIRM stConfirmInfo;
	memset(&stConfirmInfo,0,sizeof(IVS_WATERMARK_ALARM_CONFIRM));
	IVS_INT32 iRet = COCXAlarmMgrXMLProcess::GetWatermarkAlarmConfirmInfo(pConfirmInfo,&stConfirmInfo);
	if (IVS_SUCCEED != iRet)
	{
		return iRet;
	}
	return IVS_SDK_WatermarkAlarmConfirm(m_iSDKSessionID,&stConfirmInfo);
}

//水印告警撤销
LONG CIVS_OCXPlayerCtrl::IVS_OCX_WatermarkAlarmCancel(LPCTSTR pCancelInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	if(NULL == pCancelInfo)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Watermark Alarm Cancel Failed.", "Cancel Info is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_WATERMARK_ALARM_CANCEL stCancelInfo;
	memset(&stCancelInfo,0,sizeof(IVS_WATERMARK_ALARM_CANCEL));
	IVS_INT32 iRet = COCXAlarmMgrXMLProcess::GetWatermarkAlarmCancelInfo(pCancelInfo,&stCancelInfo);
	if (IVS_SUCCEED != iRet)
	{
		return iRet;
	}
	return IVS_SDK_WatermarkAlarmCancel(m_iSDKSessionID,&stCancelInfo);
}

//水印告警查询
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_QueryWatermarkAlarmList(LPCTSTR pQueryXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	if(NULL == pQueryXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Query Watermark Alarm List Failed.", "QueryXml Info is NULL.");
		return NULL;
	}
	CString strResult;
	IVS_UINT32 uiXmlLen = 0;
	CXml xmlReq;
	CXml xmlRsp;
	//IVS_CHAR cCameraCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_SUCCEED);
	if (NULL == xmlReq.Parse(pQueryXml))
	{
		iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);

		BP_RUN_LOG_ERR(iRet,"IVS OCX Query Watermark Alarm List Failed", "Reason: Parse pUnifiedQuery to Xml Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	IVS_INT32 iFieldNum = 0;
	iFieldNum = COCXXmlProcess::GetFieldNum(xmlReq);

	//分配内存
	IVS_VOID* pQueryFiledBuff = NULL;
	iRet = COCXXmlProcess::GetUnifiedFormatQueryParseXML(pQueryFiledBuff,xmlReq,iFieldNum);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		iRet = static_cast<IVS_INT32>(IVS_XML_INVALID);
		BP_RUN_LOG_ERR(iRet,"IVS OCX Query Watermark Alarm List Failed.", "Reason:Get Unified Format Query Parse XML Failed.", iRet);
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	IVS_QUERY_UNIFIED_FORMAT* pQueryUnifiedFormat = (IVS_QUERY_UNIFIED_FORMAT*)pQueryFiledBuff;

	IVS_UINT32 uiTotal = (pQueryUnifiedFormat->stIndex).uiToIndex - (pQueryUnifiedFormat->stIndex).uiFromIndex;
	IVS_UINT32 uiBufSize = sizeof(IVS_WATERMARK_ALARM_INFO_LIST)+sizeof(IVS_WATERMARK_ALARM_INFO_LIST)*uiTotal;

	IVS_CHAR* pWatermarkAlarmListBuf = IVS_NEW(pWatermarkAlarmListBuf, uiBufSize);
	if (NULL == pWatermarkAlarmListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		IVS_DELETE(pWatermarkAlarmListBuf, MUILI);

		BP_RUN_LOG_ERR(iRet,"IVS OCX Query Watermark Alarm List Failed", "Reason:New pWatermarkAlarmListBuf Memory Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	memset(pWatermarkAlarmListBuf, 0, uiBufSize);

	iRet = IVS_SDK_QueryWatermarkAlarmList(m_iSDKSessionID,pQueryUnifiedFormat,pWatermarkAlarmListBuf,uiBufSize);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		IVS_DELETE(pWatermarkAlarmListBuf, MUILI);

		BP_RUN_LOG_ERR(iRet,"IVS OCX Query Watermark Alarm List Failed.", "Reason:SDK Query Record List Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	IVS_WATERMARK_ALARM_INFO_LIST* pAlarmList = (IVS_WATERMARK_ALARM_INFO_LIST*) pWatermarkAlarmListBuf;//lint !e826
	
	//拼装响应消息结构体为XML
	iRet = COCXAlarmMgrXMLProcess::GetWatermarkAlarmListGetXML(pAlarmList,iRet,xmlRsp);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pQueryFiledBuff, MUILI);
		IVS_DELETE(pWatermarkAlarmListBuf, MUILI);

		BP_RUN_LOG_ERR(iRet,"IVS OCX Query Watermark Alarm List Failed", "Get Record List Response XML Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	const IVS_CHAR* pReq = xmlRsp.GetXMLStream(uiXmlLen);
	strResult = pReq;
	IVS_DELETE(pQueryFiledBuff, MUILI);
	IVS_DELETE(pWatermarkAlarmListBuf, MUILI);

	const char* pXml = xmlRsp.GetXMLStream(uiXmlLen);
	if (NULL == pXml)
	{
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	strResult = pXml;

	BP_RUN_LOG_INF("IVS OCXWatermark Alarm List.", "Result:%s", pXml);
	return strResult.AllocSysString();
}


BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetPaneWndID(ULONG ulTVWallID, ULONG ulPaneID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,TVWallID: %lu,PaneID: %lu", this,ulTVWallID,ulPaneID);
	CString strResult;
	IVS_INT32 iRet = IVS_FAIL;
	if(NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(iRet,"IVS OCX GetPane WndID Failed.","Reason:m_pVideoPaneMgr is NULL.");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

	// TODO: Add your dispatch handler code here
	/*
	IVS_ULONG curTVWallID = m_pVideoPaneMgr->GetCurTVWallID();
	if(0 == curTVWallID)
	{
		iRet = IVS_PARA_INVALID;
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Pane WndID Failed.","Reason:CurTVWallID = 0.");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}	
	if(curTVWallID != ulTVWallID)
	{
		iRet = IVS_PARA_INVALID;
		BP_RUN_LOG_ERR(iRet,"IVS OCX Get Pane WndID Failed.","Reason:CurTVWallID[%lu] != ulTVWallID[%lu]",curTVWallID,ulTVWallID);
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	std::vector<IVS_ULONG> vecWndID;
	vecWndID.clear();
	vecWndID.push_back(ulPaneID);
	std::vector<IVS_ULONG> vecRealPaneID;
	m_pVideoPaneMgr->GetRealTVwallPaneIDByParentID(vecWndID,vecRealPaneID);
	std::string wndIDValue = "";
	IVS_UINT32 vecSize = vecRealPaneID.size();
	for(IVS_UINT32 i = 0;i<vecSize;i++)
	{
		std::string strID = CToolsHelp::Int2Str((int)vecRealPaneID.at(i));
		wndIDValue.append(strID);
		if(i+1 != vecSize)
		{
			wndIDValue.append(",");
		}
		
	}
	*/

	TVWALL_PANE_LIST paneList;
	RECTLIST rectList;
	IVS_UINT32 uiXNum = 0;
	IVS_UINT32 uiYNum = 0;
	iRet = COCXTVWallXMLProcess::GetInst().GetTVWallPaneList(ulTVWallID, paneList, uiXNum, uiYNum);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet,"IVS OCX GetPane WndID Failed.","Reason:Get TVWall PaneList Failed.");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

    // add by wanglei 00165153:2013.7.26 DTS2013071507953
    // 【SDK_多进一出】多进一出布局，开启组轮巡，组轮巡窗格播放窗格错误
    // 原先窗格编号采用 【1】 【2 5 6 7】 【3】 【4】
    // 现在改成【1】 【2 3 4 5】 【6】 【7】
    // 原因是电视墙轮巡采用编号递增访问，方式乱序
    std::vector<IVS_ULONG> vecRealPaneID;	
    for (TVWALL_PANE_LIST::iterator itor = paneList.begin(); itor != paneList.end(); itor++)
    {
        if (itor->ulPaneName == ulPaneID)
        {
            int iChannelSize = (int) itor->listChanel.size();
            if (1 >= iChannelSize)
            {
                vecRealPaneID.push_back(itor->ulWndID);
            }
            else if (4 >= iChannelSize)
            {
                vecRealPaneID.push_back(itor->ulWndID);
                vecRealPaneID.push_back(itor->ulWndID + 1);
                vecRealPaneID.push_back(itor->ulWndID + 2);
                vecRealPaneID.push_back(itor->ulWndID + 3);
            }
            else if (9 >= iChannelSize)
            {
                vecRealPaneID.push_back(itor->ulWndID);
                vecRealPaneID.push_back(itor->ulWndID + 1);
                vecRealPaneID.push_back(itor->ulWndID + 2);
                vecRealPaneID.push_back(itor->ulWndID + 3);
                vecRealPaneID.push_back(itor->ulWndID + 4);
                vecRealPaneID.push_back(itor->ulWndID + 5);
                vecRealPaneID.push_back(itor->ulWndID + 6);
                vecRealPaneID.push_back(itor->ulWndID + 7);
                vecRealPaneID.push_back(itor->ulWndID + 8);
            }
            else if (16 >= iChannelSize)
            {
                vecRealPaneID.push_back(itor->ulWndID);
                vecRealPaneID.push_back(itor->ulWndID + 1);
                vecRealPaneID.push_back(itor->ulWndID + 2);
                vecRealPaneID.push_back(itor->ulWndID + 3);
                vecRealPaneID.push_back(itor->ulWndID + 4);
                vecRealPaneID.push_back(itor->ulWndID + 5);
                vecRealPaneID.push_back(itor->ulWndID + 6);
                vecRealPaneID.push_back(itor->ulWndID + 7);
                vecRealPaneID.push_back(itor->ulWndID + 8);
                vecRealPaneID.push_back(itor->ulWndID + 9);
                vecRealPaneID.push_back(itor->ulWndID + 10);
                vecRealPaneID.push_back(itor->ulWndID + 11);
                vecRealPaneID.push_back(itor->ulWndID + 12);
                vecRealPaneID.push_back(itor->ulWndID + 13);
                vecRealPaneID.push_back(itor->ulWndID + 14);
                vecRealPaneID.push_back(itor->ulWndID + 15);
            }
            else
            {
                vecRealPaneID.clear();
            }
            break;
        }
    }

    // modify by wanglei 00165153:2013.7.26 DTS2013071507953
    // 简化处理，【1】 ，【2 3 4 5】，【6】，【7】,类似这样处理，
	//IVS_UINT32 iParenPaneNum = paneList.size();//父窗格的个数
	//std::list<IVS_ULONG> listSplit1Pane;//会拆分成1个窗格的窗口列表
	//std::list<IVS_ULONG> listSplit4Pane;//会拆分成4个窗格的窗口列表
	//std::list<IVS_ULONG> listSplit9Pane;//会拆分成9个窗格的窗口列表
	//std::list<IVS_ULONG> listSplit16Pane;//会拆分成16个窗格的窗口列表

	//for(TVWALL_PANE_LIST::iterator ite=paneList.begin();ite!=paneList.end();ite++)
	//{
	//	IVS_ULONG ulParentPaneID = ite->ulWndID;
	//	IVS_UINT32 uiNum = ite->uiChannelNum;
	//	if(uiNum <= 1)
	//	{
	//		listSplit1Pane.push_back(ulParentPaneID);
	//	}
	//	else if(uiNum > 1 && uiNum <= 4)
	//	{
	//		listSplit4Pane.push_back(ulParentPaneID);
	//	}
	//	else if(uiNum > 4 && uiNum <= 9)
	//	{
	//		listSplit9Pane.push_back(ulParentPaneID);
	//	}
	//	else if(uiNum > 9 && uiNum <= 16)
	//	{
	//		listSplit16Pane.push_back(ulParentPaneID);
	//	}
	//}
	//
	//for(std::list<IVS_ULONG>::iterator ite=listSplit1Pane.begin();ite!=listSplit1Pane.end();ite++)
	//{		
	//	IVS_ULONG ulWndID = *ite;
	//	if(ulWndID == ulPaneID)
	//	{
	//		vecRealPaneID.push_back(ulPaneID);
	//		break;
	//	}
	//}
	//if(0 == vecRealPaneID.size())
	//{	
	//	for(std::list<IVS_ULONG>::iterator ite=listSplit4Pane.begin();ite!=listSplit4Pane.end();ite++)
	//	{		
	//		IVS_ULONG ulWndID = *ite;
	//		if(ulWndID == ulPaneID)
	//		{
	//			vecRealPaneID.push_back(ulPaneID);
	//			vecRealPaneID.push_back(ulPaneID+1);
	//			vecRealPaneID.push_back(ulPaneID+2);
	//			vecRealPaneID.push_back(ulPaneID+3);
	//			break;
	//		}
	//		iParenPaneNum+=3;//增长3
	//	}
	//}
	//if(0 == vecRealPaneID.size())
	//{	
	//	for(std::list<IVS_ULONG>::iterator ite=listSplit9Pane.begin();ite!=listSplit9Pane.end();ite++)
	//	{		
	//		IVS_ULONG ulWndID = *ite;
	//		if(ulWndID == ulPaneID)
	//		{
	//			vecRealPaneID.push_back(ulPaneID);
	//			vecRealPaneID.push_back(ulPaneID+1);
	//			vecRealPaneID.push_back(ulPaneID+2);
	//			vecRealPaneID.push_back(ulPaneID+3);
	//			vecRealPaneID.push_back(ulPaneID+4);
	//			vecRealPaneID.push_back(ulPaneID+5);
	//			vecRealPaneID.push_back(ulPaneID+6);
	//			vecRealPaneID.push_back(ulPaneID+7);
	//			vecRealPaneID.push_back(ulPaneID+8);				
	//			break;
	//		}
	//		iParenPaneNum+=8;//增长8
	//	}
	//}
	//if(0 == vecRealPaneID.size())
	//{	
	//	for(std::list<IVS_ULONG>::iterator ite=listSplit16Pane.begin();ite!=listSplit16Pane.end();ite++)
	//	{		
	//		IVS_ULONG ulWndID = *ite;
	//		if(ulWndID == ulPaneID)
	//		{
	//			vecRealPaneID.push_back(ulPaneID);
	//			vecRealPaneID.push_back(ulPaneID+1);
	//			vecRealPaneID.push_back(ulPaneID+2);
	//			vecRealPaneID.push_back(ulPaneID+3);
	//			vecRealPaneID.push_back(ulPaneID+4);
	//			vecRealPaneID.push_back(ulPaneID+5);
	//			vecRealPaneID.push_back(ulPaneID+6);
	//			vecRealPaneID.push_back(ulPaneID+7);
	//			vecRealPaneID.push_back(ulPaneID+8);
	//			vecRealPaneID.push_back(ulPaneID+9);
	//			vecRealPaneID.push_back(ulPaneID+10);
	//			vecRealPaneID.push_back(ulPaneID+11);
	//			vecRealPaneID.push_back(ulPaneID+12);
	//			vecRealPaneID.push_back(ulPaneID+13);
	//			vecRealPaneID.push_back(ulPaneID+14);
	//			vecRealPaneID.push_back(ulPaneID+15);				
	//			break;
	//		}
	//		iParenPaneNum+=15;//增长15
	//	}
	//}
	if(0 != vecRealPaneID.size())
	{	
		std::string wndIDValue = "";
		IVS_UINT32 vecSize = vecRealPaneID.size();
		for(IVS_UINT32 i = 0;i<vecSize;i++)
		{
			std::string strID = CToolsHelp::Int2Str((int)vecRealPaneID.at(i));
			wndIDValue.append(strID);
			if(i+1 != vecSize)
			{
				wndIDValue.append(",");
			}

		}

		CXml tvwallXml;
		(void)tvwallXml.AddDeclaration("1.0", "UTF-8", "");
		(void)tvwallXml.AddElem("Content");
		(void)tvwallXml.AddChildElem("ResultCode");
		(void)tvwallXml.IntoElem();
		(void)tvwallXml.SetElemValue("0");
		(void)tvwallXml.AddElem("WndIDs");
		(void)tvwallXml.SetElemValue(wndIDValue.c_str());
		unsigned int uilength = 0;
		strResult = tvwallXml.GetXMLStream(uilength);
		iRet = IVS_SUCCEED;
		BP_RUN_LOG_INF("IVS OCX GetPane WndID Success.","Result:[%s]",wndIDValue.c_str())
		return strResult.AllocSysString();
	}
	else
	{
		iRet = IVS_PARA_INVALID;
		BP_RUN_LOG_ERR(iRet,"IVS OCX GetPane WndID Failed.","Reason:Get PaneID List Failed.");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}

}


// 启动本地录像回放
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartLocalPlayBack(LONG lWndID, LPCTSTR pFileName, LPCTSTR pPlaybackParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here

	if (NULL == pFileName || '\0' == pFileName[0] || NULL == pPlaybackParam)
	{
		return IVS_PARA_INVALID;
	}

	IVS_LOCAL_PLAYBACK_PARAM Param;
	memset(&Param, 0, sizeof(Param));
	if (IVS_SUCCEED != COCXPlayBackXMLProcess::ParseLocalPlayBackParam(pPlaybackParam, Param))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Start Local PlayBack", "Prase Xml Fail");
		return IVS_PARA_INVALID;
    }

	CVideoPane  *pVideoPane  = GetVideoPanel((ULONG)lWndID);
	ASSERT(NULL != pVideoPane);
	CHECK_POINTER(pVideoPane, IVS_PARA_INVALID); //lint !e774

	pVideoPane->CreateBar();
	// 判断窗口是不是被局部放大占用，如果被占用，就关闭局部放大
	if ( BOTTOMBAR_MAGNIFY_OPEN == pVideoPane->GetDstMagnifyState())
	{
		CVideoPane* pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
		if (NULL != pSrcVideoPane)
		{
			::SendMessage(pSrcVideoPane->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (LONG)pVideoPane);
		}
	}
	unsigned long ulHandle = NULL;
	IVS_INT32 iRet = IVS_SDK_StartLocalPlayBack(m_iSDKSessionID, pFileName, 
		  &Param, pVideoPane->GetVideoWnd(), &ulHandle);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Start Local PlayBack", "Start Fail");
		pVideoPane->Invalidate(TRUE);
		return iRet;
	}

	(void)IVS_SDK_SetDisplayScale(m_iSDKSessionID, ulHandle, (int)(m_pVideoPaneMgr->GetDispalyScale()));
	if (m_pVideoPaneMgr->isAnalysisWndType())
	{
		pVideoPane->SetAnalysisBarIsStartPlay(TRUE);
	}

	pVideoPane->Set3DPTZState(PTZ_3D_CLOSE);
	pVideoPane->SetPlayHandle(ulHandle);
	pVideoPane->SetServiceType(SERVICE_TYPE_PLAYBACK);
	pVideoPane->SetTitleButtonPic(true);
	pVideoPane->SetRealtimePlayPic(BUTTON_CLICKED);
	pVideoPane->EnableALLButton(TRUE);
	pVideoPane->Invalidate(TRUE);
	pVideoPane->SetIsPlatRecord(false);
    pVideoPane->SetPlayState(VIDEO_PLAY_STATE_REALTIME);

    // DTS2013101201151
    // add by wanglei 00165153:2013.10.12 本地录像回放，将抓拍置灰（本地录像回放屏蔽抓拍功能）
    pVideoPane->GetVideoRealtimeBar()->SetToolbarEnableStatus((ULONGLONG)IVS_TOOLBAR_SNAPSHOT,(ULONGLONG)0);
    // end by wanglei 00165153

	pVideoPane->ResetBarPosition();

	CGlobalPlayMgr &Mgr = CGlobalPlayMgr::Instance();
	(void)Mgr.RegPlayBackHandle(ulHandle, pVideoPane, pFileName);
	// 按钮过滤，一些按钮需要被过滤，例如局部放大需要只有一个才能用
	DoButtonFilter(lWndID);
	ResumeMagnifyStatus(pVideoPane);

	// 清理内存，保证内存中不会有密码的存在
	memset((char*)pPlaybackParam,0,strlen(pPlaybackParam));

	return IVS_SUCCEED;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopLocalPlayBack(LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);
	CVideoPane * pVideoPane = GetVideoPanel(static_cast<ULONG>(lWndID));
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "StopLocalPlayBack", "lWndID[%ld] Invalid", lWndID);
		return IVS_OPERATE_MEMORY_ERROR;
	}
	// 关闭各种资源
	StopPlayResource(pVideoPane);

	(void)SetResumeState(pVideoPane->GetVideoWnd(), FALSE);

	// 如果原来被占用成了放大，先删掉这个放大，等停止后再恢复一个窗口回去
	IA_GRAPHICS graphicsTemp;
	memset(&graphicsTemp,0,sizeof(IA_GRAPHICS));
	bool bOpen = false;
	CVideoPane* pMagnifySrcPane = NULL;
	if (BOTTOMBAR_MAGNIFY_OPEN == pVideoPane->GetDstMagnifyState())
	{
		bOpen = true;
		pMagnifySrcPane = pVideoPane->GetMagnifySrcPane();

		ULONG ulNum = 1;// 获取一个图形
		(void)pMagnifySrcPane->GetGraphicsByGroup((ULONG)pVideoPane, &graphicsTemp, &ulNum);
		// 删除掉源窗口的信息
		::SendMessage(pVideoPane->GetMagnifySrcPane()->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (LONG)(pVideoPane));
	}

	LONG lRet = (LONG)StopRealPlayByPane(pVideoPane);

	// 如果自己是被占用成放大窗口，需要重新加一个放大的加上去，避免播放结束关闭完成了之后，放大窗口消失掉
	if (bOpen)
	{
		// 从新画一个上去
		if (NULL != pMagnifySrcPane)
		{
			pMagnifySrcPane->ResumeMagnifyRectByGraphic(graphicsTemp);
		}
	}
	return lRet;
}

// 查询摄像机码流能力
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDeviceCapability(LONG lCapabilityType, LPCTSTR pDevCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	if (NULL == pDevCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get DeviceCapability fail", "pDevCode = NULL");
		COCXXmlProcess::GetErrString(strResult,IVS_PARA_INVALID);
	}
	IVS_INFO_TRACE("OCX GetDeviceCapability: %x,lCapabilityType: %l, pDevCode: %s", this, lCapabilityType, pDevCode);

	COCXDeviceMgr::GetDeviceCapability(m_iSDKSessionID, (IVS_UINT32)lCapabilityType, (IVS_CHAR*)pDevCode, strResult);

	return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartNetKeyBoard(ULONG ulNetKeyBoardVendor, ULONG ulPort)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
    // TODO: Add your dispatch handler code here
	m_iOldCallBackKeyValue = 0;
	m_cOldSpeed = 0;

    LONG lRet = IVS_SDK_StartNetKeyBoard(ulNetKeyBoardVendor, ulPort, NetKeyBoardCallBack);//lint !e64
    return lRet;
}//lint !e1762


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopNetKeyBoard(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
    // TODO: Add your dispatch handler code here
    LONG ulRet = IVS_SDK_StopNetKeyBoard();
    return ulRet;
}//lint !e1762


LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetNetKeyBoardPort(ULONG ulPort)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x, ulPort:%lu", this, ulPort);
    // TODO: Add your dispatch handler code here
    LONG lRet = IVS_SDK_SetNetKeyBoardPort(ulPort);
    return lRet;
}//lint !e1762



LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetExDomainRecordPolicy(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	IVS_INT32 iRet = IVS_SDK_SetExDomainRecordPolicy(m_iSDKSessionID, (IVS_CHAR*)pReqXml);
	return iRet;
}
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetExDomainRecordPolicy(LPCTSTR pReqXml)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml,NULL);
	CString strResult;

	IVS_CHAR* pBackupPlanXml = NULL;
	IVS_INT32 iRet = IVS_SDK_GetExDomainRecordPolicy(m_iSDKSessionID, pReqXml, &pBackupPlanXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get ExDomain Record Policy", "failed");
		if (NULL != pBackupPlanXml)
		{
			(void)IVS_SDK_ReleaseBuffer(pBackupPlanXml);
		}
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	iRet = COCXXmlProcess::GetResultXML(iRet, pBackupPlanXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetResultXML to CU is fail", "NA");
		(void)IVS_SDK_ReleaseBuffer(pBackupPlanXml);
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	(void)IVS_SDK_ReleaseBuffer(pBackupPlanXml);
	return strResult.AllocSysString();
}
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_AddExDomainRecordPlan (LPCTSTR pRecordPlanXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pRecordPlanXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add ExDomain Record Plan Failed.", "Reason:pRecordPlanXml is NULL");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x,pRecordPlanXml: %s", this,pRecordPlanXml);
	CString strResult = OCXRecordMgr::AddExDomainRecordPlan(pRecordPlanXml,m_iSDKSessionID);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_ModExDomainRecordPlan(LPCTSTR pRecordPlan)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pRecordPlan)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Modify ExDomain Record Plan Failed.", "Reason:pRecordPlan is NULL");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x,RecordPlan: %s", this,pRecordPlan);
	CString strResult = OCXRecordMgr::ModifyExDomainRecordPlan(pRecordPlan,m_iSDKSessionID);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_DelExDomainRecordPlan(LPCTSTR pDelRecPlanXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pDelRecPlanXML)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Delete ExDomain Record Plan Failed.", "Reason:pDelRecPlanXML is NULL.");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x,DelRecPlanXML: %s", this,pDelRecPlanXML);
	CString strResult = OCXRecordMgr::DeleteExDomainRecordPlan(pDelRecPlanXML,m_iSDKSessionID);
	return strResult.AllocSysString();
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetExDomainRecordPlan(LPCTSTR pRecordPlanListReq)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pRecordPlanListReq)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get ExDomain Record Plan Failed.", "Reason:pRecordPlanListReq is NULL");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x,pRecordPlanListReq: %s", this,pRecordPlanListReq);
	CString strResult = OCXRecordMgr::GetExDomainRecordPlan(pRecordPlanListReq,m_iSDKSessionID);
	return strResult.AllocSysString();
}

// 客户端升级检查
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_CheckNewVersion(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX CheckNewVersion Failed","Reason:pReqXml is NULL");//lint !e534
		COCXXmlProcess::GetErrString(strResult,IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXClientNetworkMgr::CheckNewVersion(m_iSDKSessionID, pReqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "CheckNewVersion Failed", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 设备布撤防
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetDeviceGuard(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == pReqXml", "NA");
		return IVS_XML_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXAlarmMgr::SetDeviceGuard(m_iSDKSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Set Device Guard ocx fail", "NA");
		return iRet;
	}

	return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetLogPath(LPCTSTR pLogPath)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    // 日志还未初始化，不能在此方法打印日志
    if (NULL == pLogPath)
    {
        return IVS_PARA_INVALID;
    }
    IVS_INT32 iRet = IVS_SDK_SetLogPath(pLogPath);
    if (IVS_SUCCEED == iRet)
    {
        m_szLogPath = pLogPath;
    }
    return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartBackupPlayBack(LPCTSTR pDomainCode,LPCTSTR pNVRCode,LPCTSTR pCameraCode, LPCTSTR pPlaybackParam, LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pDomainCode,IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode,IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,NVRCode: %s,CameraCode: %s,PlaybackParam: %s,WndID: %ld", this,pNVRCode,pCameraCode,pPlaybackParam,lWndID);

	if (strlen(pDomainCode) == 0)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Backup PlayBack Failed.", "Reason:pDomainCode is empty");
		return IVS_PARA_INVALID;
	}
	if (strlen(pNVRCode) == 0)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Backup PlayBack Failed.", "Reason:pNVRCode is empty");
		return IVS_PARA_INVALID;
	}
	if (strlen(pCameraCode) == 0)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Backup PlayBack Failed.", "Reason:pCameraCode is empty");
		return IVS_PARA_INVALID;
	}
	if (lWndID < MIN_PANE_NUMBER || lWndID > MAX_PANE_NUMBER)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Backup PlayBack Failed.", "Reason:win id is invalid");
		return IVS_PARA_INVALID;
	}

	IVS_PLAYBACK_PARAM stPlaybackParam;
	memset(&stPlaybackParam, 0, sizeof(IVS_PLAYBACK_PARAM));
	if (IVS_SUCCEED != COCXPlayBackXMLProcess::GetPlaybackParam(pPlaybackParam, stPlaybackParam))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Backup PlayBack Failed.", "Reason: Get Playback Param Failed.");
		return IVS_PARA_INVALID;
	}

	if (PROTOCOL_RTP_OVER_TCP != stPlaybackParam.uiProtocolType && PROTOCOL_RTP_OVER_UDP != stPlaybackParam.uiProtocolType)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Backup PlayBack Failed.", "Reason:uiProtocolType invalid = %d.", stPlaybackParam.uiProtocolType);
		return IVS_PARA_INVALID;
	}

	bool bRet = CToolsHelp::IsSpeedVaild(stPlaybackParam.fSpeed);
	if (!bRet)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX Start Backup PlayBack Failed.", "Reason:fScale invalid. stPlaybackParam = %f.", stPlaybackParam.fSpeed);
		return IVS_PARA_INVALID;
	}

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Start Backup PlayBack Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_FAIL;
	}

	CVideoPane * pVideoPane = m_pVideoPaneMgr->GetWndPointerByID(static_cast<ULONG>(lWndID));

	//指针 句柄为空判断
	CHECK_POINTER(pVideoPane,IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pVideoPane->GetVideoWnd(),IVS_OPERATE_MEMORY_ERROR);

	pVideoPane->CreateBar();
	// 判断窗口是不是被局部放大占用，如果被占用，就关闭局部放大
	if ( BOTTOMBAR_MAGNIFY_OPEN == pVideoPane->GetDstMagnifyState())
	{
		CVideoPane* pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
		if (NULL != pSrcVideoPane)
		{
			::SendMessage(pSrcVideoPane->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (LONG)pVideoPane);
		}
	}

	if (VIDEO_PLAY_STATE_NOT_PLAYING != pVideoPane->GetPlayState())
	{
		BP_RUN_LOG_ERR(IVS_OCX_RET_VIDEOPANE_IS_PLAYING_ERROR, "IVS OCX Start Backup PlayBack Failed.", "Reason:Window has played video.");
		return IVS_OCX_RET_VIDEOPANE_IS_PLAYING_ERROR;
	}
	BP_RUN_LOG_INF("IVS OCX Start Backup PlayBack.", "window has played video.cameraid=%s.", pCameraCode);
	//当两个操作都成功后，要把窗格的一些状态修改
	m_csHWnd2OcxLock.Lock();
	(void)m_hWnd2OcxMap.insert(std::make_pair(pVideoPane->GetVideoWnd(), this));
	m_csHWnd2OcxLock.UnLock();

	IVS_INT32 iRet = 0;
	ULONG ulHandle = 0;
	pVideoPane->SetPlayState(VIDEO_PLAY_STATE_VOD);
	pVideoPane->Invalidate(TRUE);

	iRet = IVS_SDK_StartBackupPlayBack(m_iSDKSessionID,pDomainCode,pNVRCode, pCameraCode, &stPlaybackParam, pVideoPane->GetVideoWnd(), &ulHandle);
	if (IVS_SUCCEED != iRet)
	{
		pVideoPane->SetPlayState(VIDEO_PLAY_STATE_NOT_PLAYING);
		pVideoPane->Invalidate(TRUE);
		BP_RUN_LOG_ERR(iRet, "IVS OCX Start Backup PlayBack Failed.", "Reason:SDK Start Backup PlayBack Failed.");
		return iRet;
	}
	BP_RUN_LOG_INF("IVS OCX Call SDK StartBackupPlayBack Succeed.", "CameraID=%s, hwnd=%d, playhandle=%lu.", pCameraCode, pVideoPane->GetVideoWnd(), ulHandle);

	(void)IVS_SDK_SetDisplayScale(m_iSDKSessionID, ulHandle, (int)(m_pVideoPaneMgr->GetDispalyScale()));

	if (m_pVideoPaneMgr->isAnalysisWndType())
	{
		pVideoPane->SetAnalysisBarIsStartPlay(TRUE);
	}

	pVideoPane->SetPlayHandle(ulHandle);
	pVideoPane->SetServiceType(SERVICE_TYPE_BACKUPRECORD_PLAYBACK);
	pVideoPane->SetTitleButtonPic(true);
	pVideoPane->SetRealtimePlayPic(BUTTON_CLICKED);
	pVideoPane->SetCameraID((TCHAR *)pCameraCode);
	pVideoPane->EnableALLButton(TRUE);
	//(void)((CVideoPaneMgr*)m_pVideoPaneMgr)->SetDevName((ULONG)lWndID, pCameraCode);

	CGlobalPlayMgr &Mgr = CGlobalPlayMgr::Instance();
	(void)Mgr.RegPlayBackHandle(ulHandle, pVideoPane, pCameraCode);

	BP_RUN_LOG_INF("IVS OCX Start Backup PlayBack End.", "Result = %d",  iRet);

	ResumeMagnifyStatus(pVideoPane);
	return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopBackupPlayBack(LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);
	if (MIN_PANE_NUMBER > lWndID || MAX_PANE_NUMBER < lWndID)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop Backup PlayBack Failed.", "Reason:WndID is Invalid.");
		return IVS_PARA_INVALID;
	}
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Backup PlayBack Failed.", "Reason:m_pVideoPaneMgr is null");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((ULONG)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Stop Backup PlayBack Failed.","Reason:pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	(void)SetResumeState(pVideoPane->GetVideoWnd(), FALSE);

	ULONG ulRet = StopPlayBackByPane(pVideoPane, RECORD_METHOD_MBU);

	BP_RUN_LOG_INF("IVS OCX Stop Backup PlayBack", "IVS OCX Stop Backup PlayBack end. result = %lu", ulRet);
	return 0;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ShowInstantReplayBar(ULONG ulWndID,ULONG ulStyle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//接口废弃
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld %ld", this,ulWndID,ulStyle);
	return 0;
	
	//if ((MIN_PANE_NUMBER > ulWndID) || (MAX_PANE_NUMBER < ulWndID))
	//{
	//	return IVS_PARA_INVALID;
	//}

	//if (NULL == m_pVideoPaneMgr)
	//{
	//	BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Show InstantReplay Bar Failed.", "Reason:m_pVideoPaneMgr is NULL.");
	//	return IVS_PARA_INVALID;
	//}

	//CVideoPane * pWnd = NULL;
	////入参窗格ID为0表示没有指定窗格，默认使用当前活动窗格
	//pWnd = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWndID);
	//if (NULL == pWnd)
	//{
	//	BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set CameraSwitch Wnd Failed.", "Reason:GetWnd is NULL.");
	//	return IVS_OPERATE_MEMORY_ERROR;
	//}
	//
	//pWnd->ShowInstantReplayBar(ulStyle);
	//return 0;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCameraThumbnail(LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address: %x,%s", this,pCameraCode);

	CString strResult;

	const IVS_UINT32 ulSize = 256;
	IVS_CHAR cURL[ulSize];
	memset(cURL,0,ulSize);
	IVS_INT32 iRet = IVS_SDK_GetCameraThumbnail(m_iSDKSessionID, pCameraCode, cURL,ulSize);
	CXml xmlRsp;
	if (IVS_SUCCEED != iRet)
	{
		(void)xmlRsp.AddDeclaration("1.0","UTF-8","");
		(void)xmlRsp.AddElem("Content");
		(void)xmlRsp.AddChildElem("ResultCode");
		(void)(xmlRsp.IntoElem());
		(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
		xmlRsp.OutOfElem();
		IVS_UINT32 uiLen = 0;
		strResult = xmlRsp.GetXMLStream(uiLen);
	}
	else
	{
		(void)xmlRsp.AddDeclaration("1.0","UTF-8","");
		(void)xmlRsp.AddElem("Content");
		(void)xmlRsp.AddChildElem("ResultCode");
		(void)(xmlRsp.IntoElem());
		(void)xmlRsp.SetElemValue(CToolsHelp::Int2Str(iRet).c_str());
		(void)xmlRsp.AddElem("ThumbnailURL");
		(void)xmlRsp.SetElemValue(cURL);
		xmlRsp.OutOfElem();
		IVS_UINT32 uiLen = 0;
		strResult = xmlRsp.GetXMLStream(uiLen);
	}

	return strResult.AllocSysString();
}

// 获取告警抓拍图片列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetAlarmSnapshotList(LPCTSTR pQueryParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pQueryParam)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == pQueryParam", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INFO_TRACE("OCX Address: %x, pQueryParam: %s", this, pQueryParam);
	
	IVS_INT32 iRet = COCXAlarmMgr::GetAlarmSnapshotList(m_iSDKSessionID, pQueryParam, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetAlarmSnapshotList SDK Interface fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 修改录像锁定信息
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModLockRecord(LPCTSTR pLockXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pLockXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Mod Lock Record Failed.", "Reason:pLockXml is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,pLockXml: %s", this,pLockXml);

	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_FAIL);
	// 输入参数
	std::string strLockXml =  pLockXml;
	// 输出参数
	std::string strCameraCode;
	std::string strNVRCode;
	std::string strDomainCode;
	IVS_RECORD_LOCK_INFO pRecordLockInfo;
	memset(&pRecordLockInfo,0,sizeof(IVS_RECORD_LOCK_INFO));
	if(IVS_SUCCEED!=COCXRecordXMLProcess::ModLockRecordGetXML(strLockXml,strDomainCode,strNVRCode,strCameraCode,&pRecordLockInfo))
	{
		iRet = static_cast<IVS_INT32>(IVS_PARA_INVALID);
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Mod Lock Record Failed.", "Reason:Lock Record Get XML Fail.");
		return iRet;
	}

	iRet = IVS_SDK_ModLockRecord(m_iSDKSessionID,strCameraCode.c_str(),&pRecordLockInfo);
	return iRet;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_ShowPaneBar(ULONG ulWndID,ULONG ulStyle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld %ld", this,ulWndID,ulStyle);
	if ((MIN_PANE_NUMBER > ulWndID) || (MAX_PANE_NUMBER < ulWndID))
	{
		return IVS_PARA_INVALID;
	}

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Show InstantReplay Bar Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		return IVS_PARA_INVALID;
	}

	CVideoPane * pWnd = NULL;
	//入参窗格ID为0表示没有指定窗格，默认使用当前活动窗格
	pWnd = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWndID);
	if (NULL == pWnd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Show PaneBar Failed.", "Reason:GetWnd is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}
	//设置隐藏该Pane的所有Bar
	CRect rect;
	pWnd->GetClientRect(rect);
	pWnd->SetAllBarStatus(ulStyle);
	pWnd->ResetBarPosition();
	pWnd->InvalidateRect(rect,TRUE);
	return 0;
}

//启动备份录像下载
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_StartBackupDownload(LPCTSTR pDomainCode,LPCTSTR pNVRCode,LPCTSTR pCameraCode, LPCTSTR pStartTime, LPCTSTR pEndTime, LPCTSTR pDownloadParamXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address: %x,pDomainCode: %s,pNVRCode: %s,pCameraCode: %s,pStartTime: %s,pEndTime: %s,pDownloadParamXml: %s", this,
		pDomainCode,pNVRCode,pCameraCode,pStartTime,pEndTime,pDownloadParamXml);

	CString strResult;
	int iResultCode = IVS_SUCCEED;
	unsigned long ulHandle = 0;
	CXml Xml; 

	if (NULL == pDomainCode || NULL == pNVRCode || NULL == pCameraCode || NULL == pStartTime || NULL == pEndTime || NULL == pDownloadParamXml)
	{
		(void)COCXRecordXMLProcess::RecordDownLoadBuildXml(ulHandle, IVS_PARA_INVALID, Xml);
		unsigned int uiLen = 0;
		const char *pResult = Xml.GetXMLStream(uiLen);
		strResult = pResult;
		return strResult.AllocSysString();
	}

	IVS_DOWNLOAD_PARAM DLParam;
	memset(&DLParam, 0, sizeof(DLParam));
	if (IVS_SUCCEED == COCXRecordXMLProcess::RecordDownLoadParseXml(DLParam, pDownloadParamXml))
	{
		(void)CToolsHelp::Strncpy(DLParam.stTimeSpan.cStart, sizeof(DLParam.stTimeSpan.cStart) - 1, pStartTime, strlen(pStartTime));
		(void)CToolsHelp::Strncpy(DLParam.stTimeSpan.cEnd,   sizeof(DLParam.stTimeSpan.cEnd) - 1, pEndTime, strlen(pEndTime));

		iResultCode = IVS_SDK_StartBackupDownload(m_iSDKSessionID,pDomainCode,pNVRCode, pCameraCode, &DLParam, &ulHandle);
	}
	else
	{
		iResultCode = IVS_XML_INVALID;
	}

	(void)COCXRecordXMLProcess::RecordDownLoadBuildXml(ulHandle, iResultCode, Xml);//lint !e732
	unsigned int uiLen = 0;
	const char *pResult = Xml.GetXMLStream(uiLen);
	strResult = pResult;


	// 清理内存，保证内存中不会有密码的存在
	memset((char*)pDownloadParamXml,0,strlen(pDownloadParamXml));

	return strResult.AllocSysString();
}

//停止备份录像下载
LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopBackupDownload(ULONG ulDownloadHandle)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return IVS_SDK_StopBackupDownload(m_iSDKSessionID, ulDownloadHandle);
}

//添加实况书签
BSTR CIVS_OCXPlayerCtrl:: IVS_OCX_AddRealPlayBookmark(LPCTSTR pCameraCode,LPCTSTR pBookmarkName,LPCTSTR pBookmarkTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,NULL);
	//CHECK_POINTER(pBookmarkName,NULL);//和SE确认，书签名称可以为空
	CHECK_POINTER(pBookmarkTime,NULL);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,BookmarkName: %s,BookmarkTime: %s", this,pCameraCode,pBookmarkName,pBookmarkTime);
	CString strResult = "";
	IVS_UINT32 uiXmlLen = 0;
	IVS_INT32 iRet = IVS_FAIL;
	IVS_UINT32 uiBookmarkID = 0;
	CXml xmlRsp;

	iRet = IVS_SDK_AddRealPlayBookmark(m_iSDKSessionID,pCameraCode,pBookmarkName,pBookmarkTime,&uiBookmarkID);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"IVS OCX Add Bookmark List Failed", "Reason:IVS SDK Add Bookmark Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	//拼装响应消息结构体为XML
	iRet = COCXRecordXMLProcess::AddBookmarkParseXML(uiBookmarkID,iRet,xmlRsp);
	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult.AllocSysString();
}


//修改实况书签
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModifyRealPlayBookmark(LPCTSTR pCameraCode,ULONG ulBookmarkID,LPCTSTR pNewBookmarkName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	//CHECK_POINTER(pNewBookmarkName,IVS_PARA_INVALID);//和SE确认，书签名称可以为空
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,BookmarkID: %lu,NewBookmarkName: %s", this,pCameraCode,ulBookmarkID,pNewBookmarkName);
	IVS_INT32 iRet = IVS_FAIL;
	iRet = IVS_SDK_ModifyRealPlayBookmark(m_iSDKSessionID,pCameraCode,ulBookmarkID,pNewBookmarkName);
	return iRet;
}

//删除实况书签
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteRealPlayBookmark(LPCTSTR pCameraCode,ULONG ulBookmarkID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,CameraCode: %s,BookmarkID: %lu", this,pCameraCode,ulBookmarkID);
	IVS_INT32 iRet = IVS_FAIL;
	iRet = IVS_SDK_DeleteRealPlayBookmark(m_iSDKSessionID,pCameraCode,ulBookmarkID);
	return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartSyncPlayTVWall(ULONG ulTVWallID, LPCTSTR pSyncPlayInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(pSyncPlayInfo, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TVWallID: %lu,SyncPlayInfo: %s", this,ulTVWallID,pSyncPlayInfo);	

	if(NULL == m_pVideoPaneMgr)
	{
		return IVS_FAIL;
	}
	if(m_pVideoPaneMgr->GetCurTVWallID() != ulTVWallID)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start TVWall SyncPlayBack Failed.", "Reason:input TVWallID is incorrect.");
		return IVS_PARA_INVALID;
	}

	CTVWallSyncPlayBackMgr &TVWallSyncMgr = CTVWallSyncPlayBackMgr::Instance();
	if(TVWallSyncMgr.m_bStart)
	{
		BP_RUN_LOG_INF("TVWall SyncPlayBack is running.","NA");
		return IVS_SUCCEED;
	}

	//电视墙回放同步上墙的参数与 sdk本身的同步回放参数相同，因此使用相同的xml解析函数
	SyncPlayInfo SyncInfo;
	if (IVS_SUCCEED != COCXPlayBackXMLProcess::ParsePlayBackSync(pSyncPlayInfo, SyncInfo))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Start TVWall SyncPlayBack Failed.", "Reason:Parse Input Xml Failed.");
		return IVS_XML_INVALID;
	}

	

	//遍历同步回放列表，转换成同步任务列表
	PLAY_INFO_ITER iter = SyncInfo.m_PlayInfoList.begin();
	PLAY_INFO_ITER end  = SyncInfo.m_PlayInfoList.end();
	for (; iter != end; iter++)
	{
		CPlayInfo &PlayInfo = *iter;
		CVideoPane *pPane = GetVideoPanel(PlayInfo.ulWndID);
		if (NULL == pPane)//lint !e774
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "Insert TVWall SyncPlayBack Task Failed.","Reason:Video PaneID[%lu] is invalid.", PlayInfo.ulWndID);
			continue;
		}

		//ulWndID为真实的电视墙窗格，根据真实的窗格ID找到虚拟的窗格[X-X]
		TVWall_VPANEID vPaneID;
		if(!m_pVideoPaneMgr->GetVirtualIDByRealID(PlayInfo.ulWndID,vPaneID))
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Insert TVWall SyncPlayBack Task Failed.", "Reason: WndID not Exist.");
			continue;
		}

		//根据虚拟窗格找到TVWall.xml中对应的电视墙参数【解码器和通道】
		IVS_TVWALL_PARAM stTVWallParam = {0};
		IVS_INT32 iRet = COCXTVWallXMLProcess::GetInst().GetDecoderIDAndChannelNo(ulTVWallID, vPaneID, stTVWallParam);
		if(IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "Insert TVWall SyncPlayBack Task Failed.", "Reason:Get DecoderID And ChannelNo Failed.");
			continue;
		}

		//解析时间参数，创建对应的任务插入到任务列表中
		TIME_SPAN_ITER _iter = PlayInfo.m_TimeList.begin();
		TIME_SPAN_ITER _end  = PlayInfo.m_TimeList.end();
		for (; _iter != _end; _iter++)
		{
			TIME_INFO &ti = *_iter;
			REC_TIME_SPAN rts;
			rts.Start = ti.beg;
			rts.End   = ti.end;
			IVS_PLAYBACK_PARAM stPlayBackParam = {0};
			stPlayBackParam.fSpeed = static_cast<float>(1.0);
			FORMAT_TIME(rts.Start, stPlayBackParam.stTimeSpan.cStart);
			FORMAT_TIME(rts.End, stPlayBackParam.stTimeSpan.cEnd);
			stPlayBackParam.uiProtocolType = SyncInfo.m_uProto;
			(void)TVWallSyncMgr.InsertTask(ulTVWallID,m_iSDKSessionID,pPane,PlayInfo.CameraCode,&stTVWallParam,&stPlayBackParam,rts);
		}
	}

	//任务准备好后，开启任务
	return TVWallSyncMgr.StartTVwallSyncPlayBack();
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopSyncPlayTVWall(ULONG ulTVWallID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	IVS_INFO_TRACE("OCX Address: %x,TVWallID: %lu.", this,ulTVWallID);
	CTVWallSyncPlayBackMgr &TVWallSyncMgr = CTVWallSyncPlayBackMgr::Instance();
	return TVWallSyncMgr.StopTVwallSyncPlayBack();
}

//设置窗格为电子地图窗格
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_SetPaneEMap(ULONG ulWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,ulWndID);
	if ((MIN_PANE_NUMBER > ulWndID) || (MAX_PANE_NUMBER < ulWndID))
	{
		COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
		return strResult.AllocSysString();
	}

	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "OCX Set Pane EMap Failed.", "Reason:m_pVideoPaneMgr is NULL.");
		COCXXmlProcess::GetErrString(strResult, IVS_OPERATE_MEMORY_ERROR);
		return strResult.AllocSysString();
	}

	CVideoPane * pWnd = NULL;
	//入参窗格ID为0表示没有指定窗格，默认使用当前活动窗格
	pWnd = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWndID);
	if (NULL == pWnd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Show PaneBar Failed.", "Reason:GetWnd is NULL.");
		COCXXmlProcess::GetErrString(strResult, IVS_OPERATE_MEMORY_ERROR);
		return strResult.AllocSysString();
	}

	CRect rect;
	pWnd->GetClientRect(rect);
	std::string strName;
	PostEventXMLImpl.GetBtnName(strName,2,(ULONGLONG)IVS_TOOLBAR_OPEN_MAP);
	std::string xmlToCU;
	PostEventXMLImpl.GetToolBarEMapClickEventMsg(xmlToCU,strName, (ULONG)pWnd->GetSafeHwnd(),rect);
	strResult = xmlToCU.c_str();
	//pWnd->SetPlayState(VIDEO_PLAY_STATE_EMAP);
	//pWnd->ResetBarPosition();
	//pWnd->InvalidateRect(rect,TRUE);
	return strResult.AllocSysString();
}
// 获取网络键盘摄像机信息
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetNetkeyboardList(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
    CString strResult;

    std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(NET_KEYBOARD_INFO_FILE_NAME);
    if (strPath.empty())
    {
        BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Get NetKeyBoard Info Failed.", "NA");
        COCXXmlProcess::GetErrString(strResult, IVS_CREATE_FILE_ERROR);
        return strResult.AllocSysString();
    }

    // 读取XML，保存参数
    CXml xml;
    if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
    {
        // 如果加载失败，重新用模板生成这个XML
        if (!m_pLocalConfig.CreateLocalConfigXML(strPath.c_str()))
        {
            BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Get NetKeyBoard Info Failed.", "NA");
            COCXXmlProcess::GetErrString(strResult, IVS_CREATE_FILE_ERROR);
            return strResult.AllocSysString();
        }
        // 如果再失败。。
        if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
        {
            BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "IVS OCX Get NetKeyBoard Info Failed.", "NA");
            COCXXmlProcess::GetErrString(strResult, IVS_OPEN_FILE_ERROR);
            return strResult.AllocSysString();
        }
    }

    IVS_UINT32 iXMLSize = 0;
    if (!xml.FindElemEx("Content"))
    {
        BP_RUN_LOG_ERR(IVS_FILE_FORMAT_ERROR, "IVS OCX Get NetKeyBoard Info Failed.", "NA");
        COCXXmlProcess::GetErrString(strResult, IVS_FILE_FORMAT_ERROR);
        return strResult.AllocSysString();
    }
    (void)xml.AddElemBeforeCurNode("ResultCode");
    (void)xml.SetElemValue("0");
    strResult = xml.GetXMLStream(iXMLSize);
    return strResult.AllocSysString();
}

// 设置网络键盘摄像机信息
LONG CIVS_OCXPlayerCtrl::IVS_OCX_SaveNetkeyboardCamera(LPCTSTR pNetboardCameraList)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pNetboardCameraList)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Save NetKeyBoard Info Failed.", "Reason:pNetKeyBoardParam is NULL.");
        return IVS_PARA_INVALID;
    }
    IVS_INFO_TRACE("OCX Address: %x", this);
    //CHECK_POINTER(m_pLocalConfig);
    if (!m_pLocalConfig.SetCDATA2XML(NET_KEYBOARD_INFO_FILE_NAME, NULL, pNetboardCameraList))
    {
        // TODO 错误码
        BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Save NetKeyBoard Info Failed.","Reason:Insert cdata to xml failed.");
        return IVS_FAIL;
    }
    return IVS_SUCCEED;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_DelGuardPos(LPCTSTR pGuardPosInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pGuardPosInfo,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pGuardPosInfo: %s", this, pGuardPosInfo);
	// 输入参数
	std::string strGuardPosInfo = pGuardPosInfo;
	// 输出参数
	std::string strCameraCode;
	IVS_GUARD_POS_INFO stGuardPosInfo;
	memset(&stGuardPosInfo, 0, sizeof(IVS_GUARD_POS_INFO));
	IVS_INT32 iRet = COCXPtzControlXMLProcess::DelGuardPosParseXML(strGuardPosInfo, strCameraCode, &stGuardPosInfo);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "set guard pos parse xml failed", "iRet = %d", iRet);
		return iRet;
	}
	iRet = IVS_SDK_DelGuardPos(m_iSDKSessionID, strCameraCode.c_str(), stGuardPosInfo.uiPresetIndex);
	return iRet;
}

// 设置网络键盘摄像机信息
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModNetkeyboardCamera(LPCTSTR pNetboardCamera)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pNetboardCamera)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Modify NetKeyBoard Info Failed.", "Reason:pNetboardCamera is NULL.");
        return IVS_PARA_INVALID;
    }
    IVS_INFO_TRACE("OCX Address: %x,pNetboardCamera: %s", this,pNetboardCamera);
    CXml xmlReq;
    if (!xmlReq.Parse(pNetboardCamera))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Modify NetKeyBoard Info Failed.", "NA");
        return IVS_PARA_INVALID;
    }
    // 添加摄像机编码
    if (!xmlReq.FindElemEx("Content/NetboardCameraInfo/CameraCode"))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Modify NetKeyBoard Info Failed.", "NA");
        return IVS_PARA_INVALID;
    }
	const char* pCameraCode = xmlReq.GetElemValue();
	CHECK_POINTER(pCameraCode, IVS_XML_INVALID);
    std::string strCameraCode = pCameraCode;
    // 添加键值
    if (!xmlReq.FindElemEx("Content/NetboardCameraInfo/NetkeyboardValue"))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Modify NetKeyBoard Info Failed.", "NA");
        return IVS_PARA_INVALID;
    }
	const char* pNetKeyValue = xmlReq.GetElemValue();
	CHECK_POINTER(pNetKeyValue, IVS_XML_INVALID);
    std::string strNetKeyValue = pNetKeyValue;
    std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(NET_KEYBOARD_INFO_FILE_NAME);
    if (strPath.empty())
    {
        BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Modify NetKeyBoard Info Failed.", "NA");
        return IVS_CREATE_FILE_ERROR;
    }
    // 读取XML，保存参数
    CXml xml;
    if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
    {
        // 如果加载失败，重新用模板生成这个XML
        if (!m_pLocalConfig.CreateNetKeyBoardInfoXML(strPath.c_str()))
        {
            BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Modify NetKeyBoard Info Failed.", "NA");
            return IVS_CREATE_FILE_ERROR;
        }
        // 如果再失败。。
        if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
        {
            BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "IVS OCX Modify NetKeyBoard Info Failed.", "NA");
            return IVS_OPEN_FILE_ERROR;
        }
    }
    if (!xml.FindElemEx("Content/NetboardCameraList"))
    {
        BP_RUN_LOG_ERR(IVS_FILE_FORMAT_ERROR, "IVS OCX Modify NetKeyBoard Info Failed.", "NA");
        return IVS_FILE_FORMAT_ERROR;
    }
    // 键值信息为空，直接添加
    if (!xml.FindElem("NetboardCameraInfo"))
    {
        BP_RUN_LOG_INF("NetKeyBoard Info isn't exist,add to xml file", "NA");
        (void)xml.AddChildElem("NetboardCameraInfo");
        (void)xml.IntoElem();
        (void)xml.AddChildElem("CameraCode");
        (void)xml.IntoElem();
        (void)xml.SetElemValue(strCameraCode.c_str());
        (void)xml.AddElem("NetkeyboardValue");
        (void)xml.SetElemValue(strNetKeyValue.c_str());
        (void)xml.Save();
        return IVS_SUCCEED;
    }
    bool bNetKeyInfoExist = false;
    // 键值信息非空，根据键值查找对应节点
    do 
    {
        (void)xml.IntoElem();
        if (!xml.FindElem("NetkeyboardValue"))
        {
            xml.OutOfElem();
            continue;
        }
        const char* pNetkeyboardValue = xml.GetElemValue();
        if (NULL == pNetkeyboardValue)
        {
            xml.OutOfElem();
            continue;
        }
        if (0 != strNetKeyValue.compare(pNetkeyboardValue))
        {
            xml.OutOfElem();
            continue;
        }
        if (!xml.FindElem("CameraCode"))
        {
            xml.OutOfElem();
            continue;
        }
        // 查找到，更新摄像机编码
        xml.ModifyElemValue(strCameraCode.c_str());
        xml.OutOfElem();
        bNetKeyInfoExist = true;
        break;
    } while (xml.NextElem());
    if (!bNetKeyInfoExist)
    {
        BP_RUN_LOG_INF("NetKeyBoard Info isn't exist,add to xml file", "NA");
        xml.AddElem("NetboardCameraInfo");
        (void)xml.IntoElem();
        xml.AddChildElem("CameraCode");
        (void)xml.IntoElem();
        xml.SetElemValue(strCameraCode.c_str());
        xml.AddElem("NetkeyboardValue");
        xml.SetElemValue(strNetKeyValue.c_str());
    }
    (void)xml.Save();
    return IVS_SUCCEED;
}

// 导入前端配置文件
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ImportDevCfgFile(LPCTSTR pDevCode, LPCTSTR pCFGFile)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCFGFile, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pDevCode: %s,pCFGFile: %s", this, pDevCode, pCFGFile);
	
	IVS_INT32 iRet = IVS_SDK_ImportDevCfgFile(m_iSDKSessionID, (IVS_CHAR*)pDevCode, (IVS_CHAR*)pCFGFile);

	return iRet;
}

// 导出前端配置文件
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ExportDevCfgFile(LPCTSTR pDevCode, LPCTSTR pCFGFilePath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_POINTER(pDevCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCFGFilePath, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pDevCode: %s,pCFGFile: %s", this, pDevCode, pCFGFilePath);

	// 解析DevCode

	if (!MakeSureDirectoryPathExists((IVS_CHAR*)pCFGFilePath))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "ExportDevCfgFile fail", "pCFGFilePath fail");
		return IVS_PARA_INVALID;
	}

	IVS_INT32 iRet = IVS_SDK_ExportDevCfgFile(m_iSDKSessionID, (IVS_CHAR*)pDevCode, (IVS_CHAR*)pCFGFilePath);

	return iRet;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_PlaybackFrameStepForwardTVWall(ULONG ulTVWallID, LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %ld", this,ulTVWallID,lWndID);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
	//lWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
	TVWall_VPANEID vPaneID;
	memset(&vPaneID,0,sizeof(vPaneID));
	if(!m_pVideoPaneMgr->GetVirtualIDByRealID(static_cast<ULONG>(lWndID),vPaneID))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop PlatformPlayBack TVWall Failed.", "Reason: WndID Not Exist.");
	}

	IVS_INT32 iRet = COCXTVWallMgr::PlaybackFrameStepForwardTVWall(m_iSDKSessionID, ulTVWallID, vPaneID);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX TVWall Frame Step", "Failed.");
	}

	return iRet;
}

//  增加电视墙轮巡计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AddCameraSwitchPlanTVWall(LPCTSTR pSwitchPlanInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(pSwitchPlanInfo,NULL);
	IVS_INFO_TRACE("OCX Address: %x,pSwitchPlanInfo: %s", this, pSwitchPlanInfo);

	return CFavorite::AddCameraSwitchPlanTVWall(pSwitchPlanInfo);

}

// 删除电视墙轮巡计划
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DelCameraSwitchPlanTVWall(ULONG ulTVWallID,LPCTSTR pCameraSwitchID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(pCameraSwitchID, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,CameraSwitchID: %s", this,ulTVWallID,pCameraSwitchID);

	return CFavorite::DelCameraSwitchPlanTVWall(ulTVWallID,pCameraSwitchID);
}

// 获取电视墙轮巡计划列表
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCameraSwitchPlanListTVWall(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	IVS_INFO_TRACE("OCX Address: %x", this);

	std::string strTVWallCameraSwitchPlanList;
	IVS_INT32 iResultCode = CFavorite::GetCameraSwitchPlanListTVWall(strTVWallCameraSwitchPlanList);
	std::string str;
	CFavorite::BulidGetScenarioInfoListResult(iResultCode, strTVWallCameraSwitchPlanList, str);
	BP_RUN_LOG_INF("IVS OCX Get TVWall CameraSwitchPlan List End.","Result: %s", str.c_str());
	return _com_util::ConvertStringToBSTR(str.c_str());
}

// 获取电视墙轮巡计划
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCameraSwitchPlanTVWall(ULONG ulTVWallID,LPCTSTR pCameraSwitchID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(pCameraSwitchID, NULL);
	IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,CameraSwitchID: %s", this,ulTVWallID,pCameraSwitchID);

	std::string strTVWallCameraSwitchPlan;
	IVS_INT32 iResultCode = CFavorite::GetCameraSwitchPlanTVWall(ulTVWallID, pCameraSwitchID, strTVWallCameraSwitchPlan);
	std::string str;
	CFavorite::BulidGetScenarioInfoResult(iResultCode, strTVWallCameraSwitchPlan, str);
	BP_RUN_LOG_INF("IVS OCX Get TVWall CameraSwitchPlan End.","Result: %s", str.c_str());
	return _com_util::ConvertStringToBSTR(str.c_str());
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_PlayBackPauseTVWall(ULONG ulTVWallID, LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %ld", this,ulTVWallID,lWndID);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
	//ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
	TVWall_VPANEID vPaneID;
	memset(&vPaneID,0,sizeof(vPaneID));
	if(!m_pVideoPaneMgr->GetVirtualIDByRealID(static_cast<ULONG>(lWndID),vPaneID))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Pause PlayBack TVWall Failed.", "Reason: WndID Not Exist.");
	}

    //处理同步回放
    CTVWallSyncPlayBackMgr &TVWallSyncMgr = CTVWallSyncPlayBackMgr::Instance();
    if(TVWallSyncMgr.m_bStart && TVWallSyncMgr.IsTVWallSyncPlayBack((ULONG)lWndID))//lint !e732
    {
        BP_RUN_LOG_INF("TVWall SyncPlayBack is running.","NA");
        return TVWallSyncMgr.PauseTVWallSyncPlayBack();
    }

	IVS_INT32 iRet = COCXTVWallMgr::PlayBackPauseTVWall(m_iSDKSessionID, ulTVWallID, vPaneID);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Pause PlayBack TVWall", "Failed.");
	}

	return iRet;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_PlayBackResumeTVWall(ULONG ulTVWallID, LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %ld", this,ulTVWallID,lWndID);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
	//ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
	TVWall_VPANEID vPaneID;
	memset(&vPaneID,0,sizeof(vPaneID));
	if(!m_pVideoPaneMgr->GetVirtualIDByRealID(static_cast<ULONG>(lWndID),vPaneID))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Resume PlayBack TVWall Failed.", "Reason: WndID Not Exist.");
	}

    //处理同步回放
    CTVWallSyncPlayBackMgr &TVWallSyncMgr = CTVWallSyncPlayBackMgr::Instance();
    if(TVWallSyncMgr.m_bStart && TVWallSyncMgr.IsTVWallSyncPlayBack((ULONG)lWndID))
    {
        BP_RUN_LOG_INF("TVWall SyncPlayBack is running.","NA");
        return TVWallSyncMgr.ResumeTVWallSyncPlayBack();
    }

	IVS_INT32 iRet = COCXTVWallMgr::PlayBackResumeTVWall(m_iSDKSessionID, ulTVWallID, vPaneID);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Resume PlayBack TVWall", "Failed.");
	}

	return iRet;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartPUPlayBackTVWall(ULONG ulTVWallID, LPCTSTR pCameraCode, LPCTSTR pPlaybackParam, LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam,IVS_PARA_INVALID);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,CameraCode: %s,PlaybackParam: %s,WndID: %ld", this,ulTVWallID,pCameraCode,pPlaybackParam,lWndID);

	//ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
	TVWall_VPANEID vPaneID;
	if(!m_pVideoPaneMgr->GetVirtualIDByRealID(static_cast<ULONG>(lWndID),vPaneID))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start PU PlayBack TVWall Failed.", "Reason: WndID Not Exist.");
		return IVS_PARA_INVALID;
	}

	//找到窗格中的VideoPane并创建Bar
	CVideoPane *pVideoPane = GetVideoPanel((ULONG)lWndID);
	ASSERT(NULL != pVideoPane);
	if (NULL == pVideoPane)	  //lint !e774
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start PU PlayBack TVWall Failed.", "Reason:Invalid WinID:%ld.", lWndID);
		return IVS_PARA_INVALID;
	}

	//pVideoPane->CreateBar();

	IVS_INT32 iRet = COCXTVWallMgr::StartPUPlayBackTVWall(m_iSDKSessionID, ulTVWallID, pCameraCode, pPlaybackParam, vPaneID);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Start PU PlayBack TVWall Failed.", "Reason:OCXTVWallMgr Start Platform PlayBack TVWall Failed.");
		pVideoPane->ResetPane();
	}
    else
    {
        pVideoPane->SetTvWallBgState(true);
        pVideoPane->Invalidate(TRUE);
    }

	return iRet;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopPUPlayBackTVWall(ULONG ulTVWallID, LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %ld", this,ulTVWallID,lWndID);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
	//ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
	TVWall_VPANEID vPaneID;
	memset(&vPaneID,0,sizeof(vPaneID));
	if(!m_pVideoPaneMgr->GetVirtualIDByRealID(static_cast<ULONG>(lWndID),vPaneID))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop PUPlayBack TVWall Failed.", "Reason: WndID Not Exist.");
	}

	IVS_INT32 iRet = COCXTVWallMgr::StopPUPlayBackTVWall(m_iSDKSessionID, ulTVWallID, vPaneID);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Stop PUPlayBack TVWall", "Failed.");
	}
    else
    {
        //找到窗格中的VideoPane
        CVideoPane *pVideoPane = GetVideoPanel((ULONG)lWndID);
        if (NULL != pVideoPane)	  //lint !e774
        {
            //pVideoPane->SetPlayState(VIDEO_PLAY_STATE_RESTARTING);
            pVideoPane->SetTvWallBgState(false);
            pVideoPane->Invalidate(TRUE);
        }
    }

	return iRet;
}

// 录像缩略图抓拍
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRecordThumbnail(LPCTSTR pThumbnailQuery)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString     strResult;
	CXml        ResultXml;
	SNAP_VEC    SnapVec;
	memset(&SnapVec,0,sizeof(SNAP_VEC));

	CRecordSnap SnapInfo;

	if (IVS_SUCCEED != COCXPlayBackXMLProcess::PraseRecordSnapParam(pThumbnailQuery, SnapInfo))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Record Serial Snap", "Xml Fail");
    	(void)COCXPlayBackXMLProcess::BuildRecordSnapXml(ResultXml, IVS_PARA_INVALID, SnapVec);
		
		unsigned int len;
		strResult = ResultXml.GetXMLStream(len);
		return strResult.AllocSysString();
	}

	unsigned long handle = 0;
	IVS_INT32 result = IVS_FAIL;
	IVS_PLAYBACK_PARAM param;
	memset((void*)&param, 0, sizeof(IVS_PLAYBACK_PARAM));
	param.fSpeed = 2;
	param.uiProtocolType  = PROTOCOL_RTP_OVER_TCP;
	memcpy(&param.stTimeSpan, &SnapInfo.RecTimeSpan, sizeof(param.stTimeSpan));

	switch (SnapInfo.RecordType)
	{
	case 0:  // 平台录像
		{
			result = IVS_SDK_StartPlatformPlayBack(m_iSDKSessionID, SnapInfo.CamCode, &param, NULL, &handle);
			if (IVS_SUCCEED != result)
			{
				(void)COCXPlayBackXMLProcess::BuildRecordSnapXml(ResultXml, result, SnapVec);
				unsigned int len;
				strResult = ResultXml.GetXMLStream(len);
				return strResult.AllocSysString();
			}
			else
				break;
		}
	case 2:  // 备份服务器录像
		{
			result = IVS_SDK_StartBackupPlayBack(m_iSDKSessionID, SnapInfo.MbuDomain, SnapInfo.NvrCode, 
				     SnapInfo.CamCode, &param, NULL, &handle);
			if (IVS_SUCCEED != result)
			{
				(void)COCXPlayBackXMLProcess::BuildRecordSnapXml(ResultXml, result, SnapVec);
				unsigned int len;
				strResult = ResultXml.GetXMLStream(len);
				return strResult.AllocSysString();
			}
			else
				break;
		}
	
	case 3:  // 容灾服务器录像
		{
			result = IVS_SDK_StartDisasterRecoveryPlayBack(m_iSDKSessionID, SnapInfo.CamCode, &param, NULL, &handle);
			if (IVS_SUCCEED != result)
			{
				(void)COCXPlayBackXMLProcess::BuildRecordSnapXml(ResultXml, result, SnapVec);
				unsigned int len;
				strResult = ResultXml.GetXMLStream(len);
				return strResult.AllocSysString();
			}else
				break;
		}
	case 100:  // 本地录像
	default: 
		{
			result = IVS_PARA_INVALID;
			(void)COCXPlayBackXMLProcess::BuildRecordSnapXml(ResultXml, result, SnapVec);
			unsigned int len;
			strResult = ResultXml.GetXMLStream(len);
			return strResult.AllocSysString();
		}
	}
	
	for (unsigned int i = 0; i < SnapInfo.SnapPoint.size(); i++)
	{
		char FileName[256] = {0};
	    time_t tick = SnapInfo.SnapPoint[i];
		
		snprintf(FileName, sizeof(FileName) - 1, 
			"%s%s_%llu_%lu.jpg", SnapInfo.FilePath, SnapInfo.CamCode, tick, handle);
		 
		result = IVS_SDK_SetPlayBackTime(m_iSDKSessionID, handle, (unsigned int)(tick - SnapInfo.begine));
		if (IVS_SUCCEED != result)
		{
	        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Record Serial Snap", "Set PlayBackTime");
			break;
    	}

		bool bHit = false;
		while (1)//lint !e716
		{
			Sleep(50);
			IVS_UINT64 tickNow = 0;
		    result = IVS_SDK_GetCurrentFrameTick(m_iSDKSessionID, handle, &tickNow);
		    if (IVS_SUCCEED != result)
		    {
			    break;	
		    }

			if (tickNow  > (unsigned long long)(tick * 1000))  // a little bigger
			{
				bHit = true;
				break;
			}
    	}

		if (!bHit)
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Record Serial Snap", "Xml Fail" );
			break;
		}
			
    	result = IVS_SDK_LocalSnapshot(m_iSDKSessionID, handle, CAPTURE_FORMAT_JPG, FileName);
		if (IVS_SUCCEED != result)
		{
			BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Record Serial Snap", "Snap Fail");
			break;
		}

        RECORD_SNAP_NODE  Snap;
		memset(&Snap, 0, sizeof(Snap));

	
		strncpy(Snap.FileName, FileName, sizeof(Snap.FileName) - 1);

		if (SnapInfo.RecordType != 100)
		{
			struct tm tm;
			gmtime_s(&tm, &tick);
	        (void)_snprintf_s(Snap.SnapTime, sizeof(Snap.SnapTime) - 1, 
					"%4d%02d%02d%02d%02d%02d", 
					tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
					tm.tm_hour, tm.tm_min, tm.tm_sec);
		}
		else
		{
			(void)_snprintf_s(Snap.SnapTime, sizeof(Snap.SnapTime) - 1, "%llu", tick);
		}
		SnapVec.push_back(Snap);
	}

	switch (SnapInfo.RecordType)
	{
	case 0:
		{
			result = IVS_SDK_StopPlatformPlayBack(m_iSDKSessionID, handle);
			if (IVS_SUCCEED != result)
			{
				(void)COCXPlayBackXMLProcess::BuildRecordSnapXml(ResultXml, result, SnapVec);
				unsigned int len;
				strResult = ResultXml.GetXMLStream(len);
				return strResult.AllocSysString();
			}
			else
				break;
		}
	case 2:
		{
			result = IVS_SDK_StartBackupPlayBack(m_iSDKSessionID, SnapInfo.MbuDomain, SnapInfo.NvrCode, 
				SnapInfo.CamCode, &param, NULL, &handle);
			if (IVS_SUCCEED != result)
			{
				(void)COCXPlayBackXMLProcess::BuildRecordSnapXml(ResultXml, result, SnapVec);
				unsigned int len;
				strResult = ResultXml.GetXMLStream(len);
				return strResult.AllocSysString();
			}
			else
				break;
		}
	case 3:
		{
			result = IVS_SDK_StartDisasterRecoveryPlayBack(m_iSDKSessionID, SnapInfo.CamCode, &param, NULL, &handle);
			if (IVS_SUCCEED != result)
			{
				(void)COCXPlayBackXMLProcess::BuildRecordSnapXml(ResultXml, result, SnapVec);
				unsigned int len;
				strResult = ResultXml.GetXMLStream(len);
				return strResult.AllocSysString();
			}
			else
				break;
		}
	case 100:
	default:
		{
			result = IVS_PARA_INVALID;
			(void)COCXPlayBackXMLProcess::BuildRecordSnapXml(ResultXml, result, SnapVec);
			unsigned int len;
			strResult = ResultXml.GetXMLStream(len);
			return strResult.AllocSysString();
		}
	}

	(void)COCXPlayBackXMLProcess::BuildRecordSnapXml(ResultXml, result, SnapVec);
	unsigned int len;
	strResult = ResultXml.GetXMLStream(len);
	return strResult.AllocSysString();
}



ULONG CIVS_OCXPlayerCtrl::IVS_OCX_GetPaneWnd(ULONG ulWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Get Pane Wnd Wnd: %x, ulWndID: %d", this, ulWndID);
	if ((MIN_PANE_NUMBER > ulWndID) || (MAX_PANE_NUMBER < ulWndID))
	{
		return IVS_PARA_INVALID;
	}

	if (NULL == m_pVideoPaneMgr)
	{
		return IVS_PARA_INVALID;
	}

	CVideoPane * pWnd = NULL;
	//入参窗格ID为0表示没有指定窗格，默认使用当前活动窗格
	pWnd = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID(ulWndID);
	if (NULL == pWnd)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Get Pane Wnd Failed.", "Reason:GetWnd is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	return (ULONG)pWnd->GetSafeHwnd();
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetDevPWD(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHECK_POINTER(pReqXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXDeviceMgr::SetDevPWD(m_iSDKSessionID, (IVS_CHAR*)pReqXml);
	// 防止内存中密码出现
	memset((char*)pReqXml,0,strlen(pReqXml));
	return iRet;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetSkin(ULONG ulSkinType)
{ 
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(m_pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);
	IVS_INFO_TRACE("OCX Address: %x,SkinType: %d", this,ulSkinType);
	
	if (OCX_SKIN_TYPE_BLACK == ulSkinType)
	{
		m_ulColor = COLOR_DARK;
	}else if (OCX_SKIN_TYPE_WHITE == ulSkinType)
	{
		m_ulColor = COLOR_GRAY;
	}

	m_pVideoPaneMgr->SetColor(m_ulColor);
	m_pVideoPaneMgr->SetSkin(ulSkinType);
	g_ulSkinType = ulSkinType;
	//IVS_WIND_LAYOUT winLayoutType = m_pVideoPaneMgr->GetWindLayOut();
	//CRect rect;
	//GetClientRect(rect);
	//if (IVS_SUCCEED != m_pVideoPaneMgr->SetWndLayout(rect, winLayoutType))
	//{
	//	return IVS_SYS_NO_SUPPORT;
	//}

	// 先获取窗格布局类型
	//IVS_WIND_LAYOUT winLayoutType = m_pVideoPaneMgr->GetWindLayOut();
	//ULONG ulToolbarFlag = (ULONG)m_pVideoPaneMgr->GetToolBarVisableStatus();
	//bool IsShowTitleBar	= m_pVideoPaneMgr->GetTitleBarShowState();
	//bool IsShowToolbar	= m_pVideoPaneMgr->GetToolBarShowState();
	//ENUMDEF::WND_TYPE ulWndType = m_pVideoPaneMgr->GetWndType();
	//IVS_DELETE(m_pVideoPaneMgr);

	//CRect rect;
	//GetClientRect(rect);
	///*ENUMDEF::WND_TYPE ulWndType = WND_TYPE_REALTIME;*/
	//LayoutMgr.InitLayOut();
	//CHECK_POINTER(IVS_NEW((CVideoPaneMgr*&)m_pVideoPaneMgr), IVS_ALLOC_MEMORY_ERROR) ;

	//CUtility::SetVideoPaneMgr(m_pVideoPaneMgr);
	//if (IVS_SUCCEED != m_pVideoPaneMgr->SetOcxCtrl(this))
	//{
	//	BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR,"IVS OCX Init Failed.","Reason:VideoPane Manager set ocx ctrl fail.")
	//		return IVS_OPERATE_MEMORY_ERROR;
	//}

	//m_pVideoPaneMgr->SetWndType(ulWndType);
	//if (OCX_SKIN_TYPE_BLACK == ulSkinType)
	//{
	//	m_ulColor = COLOR_DARK;
	//}else if (OCX_SKIN_TYPE_WHITE == ulSkinType)
	//{
	//	m_ulColor = COLOR_GRAY;
	//}
	//
	//m_pVideoPaneMgr->SetColor(m_ulColor);
	//m_pVideoPaneMgr->SetSkinType(ulSkinType);
	//// 初始化CVideoPaneMgr
	//if (IVS_SUCCEED != m_pVideoPaneMgr->Init())
	//{
	//	//删除mgr在cleanup里统一处理，此处不作处理
	//	BP_RUN_LOG_ERR(IVS_SYS_NO_SUPPORT,"IVS OCX Init Failed.","Reason:VideoPane Manager initial Failed.");
	//	return IVS_SYS_NO_SUPPORT;
	//}

	//// 调用设置窗格布局，重绘所有窗格
	//if (IVS_SUCCEED != m_pVideoPaneMgr->SetWndLayout(rect, winLayoutType))
	//{
	//	BP_RUN_LOG_ERR(IVS_FAIL, "", "OCX change size error,set layout type error.");
	//	return IVS_SYS_NO_SUPPORT;
	//}

	//// 设置显示状态
	//(void)IVS_OCX_SetToolbar(ulToolbarFlag);
	//(void)IVS_OCX_ShowTitlebar((VARIANT_BOOL)IsShowTitleBar);
	//(void)IVS_OCX_ShowToolbar((VARIANT_BOOL)IsShowToolbar);

	return IVS_SUCCEED;
}

// 报警复位
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ResetAlarm(LPCTSTR pReqXml)//lint !e730
{//lint !e730
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXAlarmMgr::ResetAlarm(m_iSDKSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Reset Alarm OCX Interface fail", "NA");
		return iRet;
	}
	return iRet;
}

// 外部域设备事件订阅
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ExEventSubscribe(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXAlarmMgr::ExEventSubscribe(m_iSDKSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Ex Event Subscribe OCX Interface fail", "NA");
		return iRet;
	}
	return iRet;
}

// 查询外部域设备事件订阅
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetExEventSubscribe(LPCTSTR pReqXml)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == pReqXml", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INT32 iRet = COCXAlarmMgr::GetExEventSubscribe(m_iSDKSessionID, pReqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Ex Event Subscribe OCX fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}

// 外部域设备目录订阅
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ExCatalogSubscribe(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,pReqXml: %s", this,pReqXml);
	IVS_INT32 iRet = COCXAlarmMgr::ExCatalogSubscribe(m_iSDKSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Ex Catalog Subscribe OCX Interface fail", "NA");
		return iRet;
	}
	return iRet;
}

// 查询外部域设备目录订阅
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetExCatalogSubscribeList(LPCTSTR pReqXml)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	if (NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "NULL == pReqXml", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
		return strResult.AllocSysString();
	}
	IVS_INT32 iRet = COCXAlarmMgr::GetExCatalogSubscribeList(m_iSDKSessionID, pReqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Get Ex Catalog Subscribe List OCX fail", "NA");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();
	}
	return strResult.AllocSysString();
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartBackupPlayBackTVWall(ULONG ulTVWallID, LPCTSTR pDomainCode, LPCTSTR pNVRCode, LPCTSTR pCameraCode, LPCTSTR pPlaybackParam, LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pDomainCode,IVS_PARA_INVALID);
	CHECK_POINTER(pNVRCode,IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
	CHECK_POINTER(pPlaybackParam,IVS_PARA_INVALID);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,DomainCode: %s,NVRCode: %d,CameraCode: %s,PlaybackParam: %s,WndID: %ld", 
					this,ulTVWallID,pDomainCode,pNVRCode,pCameraCode,pPlaybackParam,lWndID);

	//ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
	TVWall_VPANEID vPaneID;
	if(!m_pVideoPaneMgr->GetVirtualIDByRealID(static_cast<ULONG>(lWndID),vPaneID))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start Backup PlayBack TVWall Failed.", "Reason: WndID Not Exist.");
		return IVS_PARA_INVALID;
	}

	//找到窗格中的VideoPane并创建Bar
	CVideoPane *pVideoPane = GetVideoPanel((ULONG)lWndID);
	ASSERT(NULL != pVideoPane);
	if (NULL == pVideoPane)	  //lint !e774
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start Backup PlayBack TVWall Failed.", "Reason:Invalid WinID:%ld.", lWndID);
		return IVS_PARA_INVALID;
	}

	//pVideoPane->CreateBar();

	IVS_INT32 iRet = COCXTVWallMgr::StartBackupPlayBackTVWall(m_iSDKSessionID, ulTVWallID, pDomainCode, pNVRCode, pCameraCode, pPlaybackParam, vPaneID);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Start Backup PlayBack TVWall Failed.", "Reason:OCXTVWallMgr Start Platform PlayBack TVWall Failed.");
	}
    else
    {
        pVideoPane->SetTvWallBgState(true);
        pVideoPane->Invalidate(TRUE);
    }

	return iRet;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopBackuplayBackTVWall(ULONG ulTVWallID, LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %ld", this,ulTVWallID,lWndID);
	CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
	//ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
	TVWall_VPANEID vPaneID;
	memset(&vPaneID,0,sizeof(vPaneID));
	if(!m_pVideoPaneMgr->GetVirtualIDByRealID(static_cast<ULONG>(lWndID),vPaneID))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop Backup PlayBack TVWall Failed.", "Reason: WndID Not Exist.");
	}

	IVS_INT32 iRet = COCXTVWallMgr::StopBackupPlayBackTVWall(m_iSDKSessionID, ulTVWallID, vPaneID);
	if(IVS_SUCCEED!=iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Stop Backup PlayBack TVWall", "Failed.");
    }
    else
    {
        //找到窗格中的VideoPane
        CVideoPane *pVideoPane = GetVideoPanel((ULONG)lWndID);
        if (NULL != pVideoPane)	  //lint !e774
        {
            //pVideoPane->SetPlayState(VIDEO_PLAY_STATE_RESTARTING);
            pVideoPane->SetTvWallBgState(false);
            pVideoPane->Invalidate(TRUE);
        }
    }

	return iRet;
}


ULONG CIVS_OCXPlayerCtrl::IVS_OCX_GetAbilityInfo(void)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_UINT32 uiAbility = 0;
	uiAbility = COCXAlarmMgr::GetAbilityInfo(m_iSDKSessionID);
	return uiAbility;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetScenarioRecovery(LONG lRecovery)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// 检查本地配置的文件是否存在，不存在就创建文件，并返回路径
	std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(LOCAL_CONFING_FILE_NAME);
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"create file failed","NA");
		return IVS_CREATE_FILE_ERROR;
	}

	// 读取XML，保存参数
	CXml xml;
	if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"load xml failed","NA");
		return IVS_XML_INVALID;
	}

	if (!xml.FindElemEx("LocalConfig/ScenarioRecovery"))
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"load xml failed","NA");
		return IVS_XML_INVALID;
	}
	// 修改节点值
	xml.ModifyElemValue(CToolsHelp::Int2Str(lRecovery).c_str());

	if (!xml.Save())
	{
		BP_RUN_LOG_ERR(IVS_FAIL,"save xml failed","NA");
		return IVS_WRITE_FILE_ERROR;
	}
	return IVS_SUCCEED;
}
LONG CIVS_OCXPlayerCtrl::IVS_OCX_GetScenarioRecovery(void)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	LONG lScenarioRecovery = 0;
	do 
	{
		// 获取appdat目录
		std::string strPath = m_pLocalConfig.CheckLocalConfigFileExist(LOCAL_CONFING_FILE_NAME);
		if (strPath.empty())
		{
			BP_RUN_LOG_ERR(IVS_FAIL,"file not exist","NA");
			break;
		}

		CXml xml;
		if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			BP_RUN_LOG_ERR(IVS_FAIL,"load file failed","NA");
			break;
		}
		if (xml.FindElemEx("LocalConfig/ScenarioRecovery"))
		{
			const char* pScenarioRecovery = xml.GetElemValue();
			if (NULL != pScenarioRecovery)
			{
				lScenarioRecovery = static_cast<LONG>(atoi(pScenarioRecovery));
			}
		}
	} while (0); //lint !e717

	return lScenarioRecovery;
}

void CIVS_OCXPlayerCtrl::IVS_OCX_EnableExchangePane(ULONG ulStatus)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address: %x,Enable Exchange Pane Status: %ld", this,ulStatus);
	CHECK_POINTER_VOID(m_pVideoPaneMgr);

	m_pVideoPaneMgr->EnableExchangePane(ulStatus);
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetRealTimeTempScenario(LPCTSTR pRealTimeScenarioXml, LPCTSTR pScenarioID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pRealTimeScenarioXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ScenarioXml: %s", this,pRealTimeScenarioXml);
	std::string strScenarioInfo;
	IVS_INT32 iResultCode = CFavorite::GetScenarioFavorite(pScenarioID, strScenarioInfo);
	// 如果场景已经存在
	if (iResultCode == IVS_SUCCEED)
	{
		(void)CFavorite::DelScenarioFavorite(pScenarioID);
	}
	std::string strTempScenarioInfoID = pScenarioID;
	return CFavorite::AddScenarioFavorite(pRealTimeScenarioXml, strTempScenarioInfoID);
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetRecordTempScenario(LPCTSTR pRecordScenarioXml, LPCTSTR pScenarioID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pRecordScenarioXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ScenarioXml: %s", this,pRecordScenarioXml);
	std::string strScenarioInfo;
	IVS_INT32 iResultCode = CFavorite::GetScenarioFavorite(pScenarioID, strScenarioInfo);
	// 如果场景已经存在
	if (iResultCode == IVS_SUCCEED)
	{
		(void)CFavorite::DelScenarioFavorite(pScenarioID);
	}
	std::string strTempScenarioInfoID = pScenarioID;
	return CFavorite::AddScenarioFavorite(pRecordScenarioXml, strTempScenarioInfoID);
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetEmapTempScenario(LPCTSTR pEmapScenarionXml, LPCTSTR pScenarioID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pEmapScenarionXml, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ScenarioXml: %s", this,pEmapScenarionXml);
	std::string strScenarioInfo;
	IVS_INT32 iResultCode = CFavorite::GetScenarioFavorite(pScenarioID, strScenarioInfo);
	// 如果场景已经存在
	if (iResultCode == IVS_SUCCEED)
	{
		(void)CFavorite::DelScenarioFavorite(pScenarioID);
	}
	std::string strTempScenarioInfoID = pScenarioID;
	return CFavorite::AddScenarioFavorite(pEmapScenarionXml, strTempScenarioInfoID);
}


BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRealTimeTempScenario(LPCTSTR pScenarioID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	std::string str = CFavorite::GetTempScenario(pScenarioID);
	BP_RUN_LOG_INF("IVS OCX Get TempScenario End.","Result: %s", str.c_str());

	return _com_util::ConvertStringToBSTR(str.c_str());
}


BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetRecordTempScenario(LPCTSTR pScenarioID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	std::string str = CFavorite::GetTempScenario(pScenarioID);
	BP_RUN_LOG_INF("IVS OCX Get TempScenario End.","Result: %s", str.c_str());

	return _com_util::ConvertStringToBSTR(str.c_str());
}


BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetEmapTempScenario(LPCTSTR pScenarioID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x", this);
	std::string str = CFavorite::GetTempScenario(pScenarioID);
	BP_RUN_LOG_INF("IVS OCX Get TempScenario End.","Result: %s", str.c_str());

	return _com_util::ConvertStringToBSTR(str.c_str());
}


void CIVS_OCXPlayerCtrl::IVS_OCX_KeyEventNotice(ULONG ulCtrlKey, ULONG ulKey)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x, CtrlKey: %lu, Key: %lu", this, ulCtrlKey, ulKey);
	CHECK_POINTER_VOID(m_pVideoPaneMgr);

	switch (ulKey)
	{
	case 0x41://VK_A
		if (0 == ulCtrlKey)
		{
			m_pVideoPaneMgr->StartSerialCaptureAll();
		}
		break;
	default:
		break;
	}

}

ULONG CIVS_OCXPlayerCtrl::IVS_OCX_SetTitleBarStatus(ULONG ulWndID, ULONG ulStatusType, ULONG ulShow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(m_pVideoPaneMgr, IVS_PARA_INVALID);
	CVideoPane* pVideoPane = m_pVideoPaneMgr->GetWndPointerByID(ulWndID);
	CHECK_POINTER(pVideoPane, IVS_PARA_INVALID);
	//保存设置按钮状态初始值
	pVideoPane->SetTitleBarStatus(ulStatusType,ulShow);
    CTranVideoTitleBar* pVideoTitleBar = pVideoPane->GetVideoTitleBar();
    CHECK_POINTER(pVideoTitleBar, IVS_PARA_INVALID);
	switch(ulStatusType)
	{
	case OCX_TITLEBAR_CRUISE:
		pVideoTitleBar->SetCruiseTrackStatus(ulShow);
		break;
	case OCX_TITLEBAR_WARNING:
		pVideoTitleBar->SetAlarmStatus(ulShow);
		break;
	case OCX_TITLEBAR_RECORD:
		pVideoTitleBar->SetRecordStatus(ulShow);
		break;
	case OCX_TITLEBAR_FRAME:
		pVideoTitleBar->SetSingleFrameStatus(ulShow);
		break;
	default:
		break;
	}
    pVideoTitleBar->Invalidate(TRUE);
	return IVS_SUCCEED;
}

// 修改轮巡资源
LONG CIVS_OCXPlayerCtrl::IVS_OCX_ModCameraSwitch(LPCTSTR pSwitchInfo)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
    IVS_INT32 iRet = IVS_FAIL;
    iRet = CFavorite::ModCameraSwitch(pSwitchInfo);
    BP_RUN_LOG_INF("IVS OCX Mod CameraSwitch End.","Result: %d", iRet);

    return iRet;
}

// 停止各种资源，包括局部放大，语音对讲，随路语音等
void CIVS_OCXPlayerCtrl::StopPlayResource(CVideoPane* pVideoPane)const
{
	CHECK_POINTER_VOID(pVideoPane);
	//停止局部放大,这个不能放在后面，放在后面，按钮无法响应
	if (MAGNIFY_OPEN == pVideoPane->GetRealtimeMagnifyState())
	{
		BP_RUN_LOG_INF("MagnifyStatus open ","need resume");
		// 再局部放大还开启的情况下被停止了，所以设置恢复标志
		pVideoPane->SetOpendMagnifyStatus(true);
		pVideoPane->SetOpendMagnifyCameraID(pVideoPane->GetCameraID());
		// 发送一个点击事件给放大按钮，让他关闭自己，这里使用同步的sendmessage,不要使用postmessage
		::SendMessage(pVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_MAGNIFY, BN_CLICKED), (LPARAM)pVideoPane->GetVideoRealtimeBar()->GetMagnifyBtnHwnd());
	}
	else
	{
		// 被设置成恢复了的情况下就不设置了,防止多次调用stop导致没法恢复
		if (!pVideoPane->GetOpendMagnifyStatus())
		{
			pVideoPane->SetOpendMagnifyStatus(false);
			pVideoPane->SetOpendMagnifyCameraID("");
		}
	}
	//停止语音对讲 add by z00193167
	if (BOTTOMBAR_TALKBACK_OPEN == pVideoPane->GetRealtimeTalkbackState())
	{
		// 发送一个点击事件给放大按钮，让他关闭自己，这里使用同步的sendmessage,不要使用postmessage
		::SendMessage(pVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_TALKBACK, BN_CLICKED), (LPARAM)pVideoPane->GetVideoRealtimeBar()->GetTalkbackBtnHwnd());
// 		pVideoPane->GetVideoRealtimeBar()->StopTalkbackSound();
// 		pVideoPane->GetVideoRealtimeBar()->DeleteTalkbackCtrl();
	}

	//停止随路语音 add by z00193167,这个比较特殊，需要控制上面的
	if (BOTTOMBAR_SOUND_OPEN == pVideoPane->GetRealtimeSoundState())
	{
		// 发送一个点击事件给放大按钮，让他关闭自己，这里使用同步的sendmessage,不要使用postmessage
		//::SendMessage(pVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_TALKBACK, BN_CLICKED), (LPARAM)pVideoPane->GetVideoRealtimeBar()->GetTalkbackBtnHwnd());
		pVideoPane->GetVideoRealtimeBar()->StopSound();
		pVideoPane->GetVideoRealtimeBar()->DeleteSoundCtrl();
	}
}


BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetBroadcastCameraList(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IVS_INFO_TRACE("OCX Address: %x", this);
	CString strResult;

	std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(BROADCAST_CAMERALIST_CAMERA_CODE);
	if (strPath.empty())
	{
		BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Get Broadcast CameraList Failed.", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_CREATE_FILE_ERROR);
		return strResult.AllocSysString();
	}

	// 读取XML，保存参数
	CXml xml;
	if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
	{
		// 如果加载失败，重新用模板生成这个XML
		if (!m_pLocalConfig.CreateBroadcastCameraListXML(strPath.c_str()))
		{
			BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Get Broadcast CameraList Failed.", "NA");
			COCXXmlProcess::GetErrString(strResult, IVS_CREATE_FILE_ERROR);
			return strResult.AllocSysString();
		}
		// 如果再失败。。
		if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
		{
			BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "IVS OCX Get Broadcast CameraList Failed.", "NA");
			COCXXmlProcess::GetErrString(strResult, IVS_OPEN_FILE_ERROR);
			return strResult.AllocSysString();
		}
	}

	IVS_UINT32 iXMLSize = 0;
	if (!xml.FindElemEx("Content"))
	{
		BP_RUN_LOG_ERR(IVS_FILE_FORMAT_ERROR, "IVS OCX Get Broadcast CameraList Failed.", "NA");
		COCXXmlProcess::GetErrString(strResult, IVS_FILE_FORMAT_ERROR);
		return strResult.AllocSysString();
	}
	(void)xml.AddElemBeforeCurNode("ResultCode");
	(void)xml.SetElemValue("0");
	strResult = xml.GetXMLStream(iXMLSize);
	return strResult.AllocSysString();
}

// 添加广播文件到本地xml
LONG CIVS_OCXPlayerCtrl::IVS_OCX_AddBroadcastFile(LPCTSTR pFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if(NULL == pFileName)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add BroadcastFile.", "Reason:pFileName is NULL.");
        return IVS_PARA_INVALID;
    }
    IVS_INFO_TRACE("OCX Address: %x,pFileName: %s", this,pFileName);
    std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(BROADCAST_FILELIST_FILE_NAME);
    if (strPath.empty())
    {
        BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Add BroadcastFile Failed", "NA");
        return IVS_CREATE_FILE_ERROR;
    }
    // 读取XML，保存参数
    CXml xml;
    if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
    {
        // 如果加载失败，重新用模板生成这个XML
        if (!m_pLocalConfig.CreateBroadcastFileListXML(strPath.c_str()))
        {
            BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Add BroadcastFile Failed.", "NA");
            return IVS_CREATE_FILE_ERROR;
        }
        // 如果再失败。。
        if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
        {
            BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "IVS OCX Add BroadcastFile Failed.", "NA");
            return IVS_OPEN_FILE_ERROR;
        }
    }
    if (!xml.FindElemEx(BROADCAST_FILELIST_ELEMENT))
    {
        BP_RUN_LOG_ERR(IVS_FILE_FORMAT_ERROR, "IVS OCX Add BroadcastFile Failed.", "NA");
        return IVS_FILE_FORMAT_ERROR;
    }
    std::string strFileName = pFileName;
    // 文件列表为空，直接添加
    if (!xml.FindElem("FileName"))
    {
        BP_RUN_LOG_INF("BroadcastFile Info isn't exist,add to xml file", "NA");
        (void)xml.AddChildElem("FileName");
        (void)xml.IntoElem();
        (void)xml.SetElemValueCDATA(strFileName.c_str());
        (void)xml.Save();
        return IVS_SUCCEED;
    }
    // 查看节点是否已经存在，存在返回；不存在添加。
    do 
    {
        const char* pFileNameValue = xml.GetElemValue();
        if (NULL == pFileNameValue)
        {
            continue;
        }
        if (0 == strFileName.compare(pFileNameValue))
        {
            BP_RUN_LOG_INF("fileName Info already exist", "NA");
            return IVS_SUCCEED;
        }
    } while (xml.NextElem());
    xml.AddElem("FileName");
    xml.SetElemValueCDATA(strFileName.c_str());
    (void)xml.Save();
    return IVS_SUCCEED;
}

// 删除广播文件（本地xml）
LONG CIVS_OCXPlayerCtrl::IVS_OCX_DeleteBroadcastFile(LPCTSTR pFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if(NULL == pFileName)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Add BroadcastFile.", "Reason:pFileName is NULL.");
        return IVS_PARA_INVALID;
    }
    IVS_INFO_TRACE("OCX Address: %x,pFileName: %s", this,pFileName);
    std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(BROADCAST_FILELIST_FILE_NAME);
    if (strPath.empty())
    {
        BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX delete BroadcastFile Failed", "NA");
        return IVS_CREATE_FILE_ERROR;
    }
    // 读取XML，保存参数
    CXml xml;
    if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
    {
        // 如果加载失败，重新用模板生成这个XML
        if (!m_pLocalConfig.CreateBroadcastFileListXML(strPath.c_str()))
        {
            BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX delete BroadcastFile Failed.", "NA");
            return IVS_CREATE_FILE_ERROR;
        }
        // 如果再失败。。
        if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
        {
            BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "IVS OCX delete BroadcastFile Failed.", "NA");
            return IVS_OPEN_FILE_ERROR;
        }
    }
    if (!xml.FindElemEx(BROADCAST_FILELIST_ELEMENT))
    {
        BP_RUN_LOG_ERR(IVS_FILE_FORMAT_ERROR, "IVS OCX delete BroadcastFile Failed.", "NA");
        return IVS_FILE_FORMAT_ERROR;
    }
    std::string strFileName = pFileName;
    // 文件列表为空，直接添加
    if (!xml.FindElem("FileName"))
    {
        BP_RUN_LOG_INF("BroadcastFile Info isn't exist,needn't delete", "NA");
        return IVS_SUCCEED;
    }
    // 查看节点是否已经存在，存在则删除；不存在返回成功。
    do 
    {
        const char* pFileNameValue = xml.GetElemValue();
        if (NULL == pFileNameValue)
        {
            continue;
        }
        if (0 == strFileName.compare(pFileNameValue))
        {
            (void)xml.RemoveElem();
            (void)xml.Save();
            return IVS_SUCCEED;
        }
    } while (xml.NextElem());
    BP_RUN_LOG_INF("BroadcastFile Info isn't exist,needn't delete", "NA");
    return IVS_SUCCEED;
}

// 获取广播文件列表（本地xml）
BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetBroadcastFileList(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("OCX Address: %x", this);
    CString strResult;
    std::string strPath = m_pLocalConfig.MakeSureLocalConfigFileExist(BROADCAST_FILELIST_FILE_NAME);
    if (strPath.empty())
    {
        BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX get BroadcastFileList Failed", "NA");
        COCXXmlProcess::GetErrString(strResult, IVS_CREATE_FILE_ERROR);
        return strResult.AllocSysString();
    }

    // 读取XML，保存参数
    CXml xml;
    if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
    {
        // 如果加载失败，重新用模板生成这个XML
        if (!m_pLocalConfig.CreateBroadcastFileListXML(strPath.c_str()))
        {
            BP_RUN_LOG_ERR(IVS_CREATE_FILE_ERROR, "IVS OCX Get BroadcastFileList Failed.", "NA");
            COCXXmlProcess::GetErrString(strResult, IVS_CREATE_FILE_ERROR);
            return strResult.AllocSysString();
        }
        // 如果再失败。。
        if (!xml.Load(strPath.c_str(), TIXML_ENCODING_LEGACY))
        {
            BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "IVS OCX Get BroadcastFileList Failed.", "NA");
            COCXXmlProcess::GetErrString(strResult, IVS_OPEN_FILE_ERROR);
            return strResult.AllocSysString();
        }
    }

    IVS_UINT32 iXMLSize = 0;
    if (!xml.FindElemEx("Content"))
    {
        BP_RUN_LOG_ERR(IVS_FILE_FORMAT_ERROR, "IVS OCX Get BroadcastFileList Failed.", "NA");
        COCXXmlProcess::GetErrString(strResult, IVS_FILE_FORMAT_ERROR);
        return strResult.AllocSysString();
    }
    (void)xml.AddElemBeforeCurNode("ResultCode");
    (void)xml.SetElemValue("0");
    strResult = xml.GetXMLStream(iXMLSize);
    return strResult.AllocSysString();
}

void CIVS_OCXPlayerCtrl::DoButtonFilter(LONG& ulWndID)
{
	CHECK_POINTER_VOID(m_pVideoPaneMgr);
	m_pVideoPaneMgr->DoButtonFilter(ulWndID);
}

// 恢复局部放大
void CIVS_OCXPlayerCtrl::ResumeMagnifyStatus(CVideoPane* pVideoPane)const
{
	CHECK_POINTER_VOID(pVideoPane);
	// 判断之前是不是开启过放大状态，如果是放大状态 恢复之前的
	if (pVideoPane->GetOpendMagnifyStatus() && 
		(0 == pVideoPane->GetOpendMagnifyCameraID().compare(pVideoPane->GetCameraID())))
	{
		// 5S，如果5秒流都没有过来，就放弃这次恢复
		int iCount = 0;
		while(1) //lint !e716
		{
			if (iCount >= 500)
			{
				break;
			}
			// 获取图像长宽
			SIZE sz = {0};
			(void)pVideoPane->GetMediaSize(sz);

			if (sz.cx != 0 && sz.cy != 0)
			{
				// 判断图像分辨率是否修改
				LONG lHeight = pVideoPane->GetDrawHeight();
				LONG lWidth = pVideoPane->GetDrawWidth();
				if (sz.cx != lWidth || sz.cy != lHeight)
				{
					break;
				}
				// 先按下按钮，再恢复状态
				::SendMessage(pVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_MAGNIFY, BN_CLICKED), (LPARAM)pVideoPane->GetVideoRealtimeBar()->GetMagnifyBtnHwnd());
				pVideoPane->ResumeAllMagnifyRect();
				break;
			}
			iCount++;
			Sleep(10);
		}
		pVideoPane->SetOpendMagnifyStatus(false);
		pVideoPane->SetOpendMagnifyCameraID("");
	}

}

void CIVS_OCXPlayerCtrl::PostSoundEvent()
{
    IVS_OCX_Windows_Event(IVS_EVENT_PLAY_SOUND_FAILED, NULL);
}

void CIVS_OCXPlayerCtrl::PostTalkbackEvent()
{
    CString xmlToCU;
    PostEventXMLImpl.PostTalkbackEvent(xmlToCU, 2);
    IVS_OCX_Windows_Event(IVS_EVENT_TALKBACK_FAILED, (LPCTSTR)xmlToCU);
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_ExGetDeviceStatus(LPCTSTR pReqXml)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pReqXml,NULL);
	CString strResult;

	IVS_CHAR* pDeviceStatusXml = NULL;
	IVS_INT32 iRet = IVS_SDK_ExGetDeviceStatus(m_iSDKSessionID, pReqXml, &pDeviceStatusXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "ExDomain Get Device Status", "failed");
		if (NULL != pDeviceStatusXml)
		{
			(void)IVS_SDK_ReleaseBuffer(pDeviceStatusXml);
		}
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	iRet = COCXXmlProcess::GetResultXML(iRet, pDeviceStatusXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "GetResultXML to CU is fail", "NA");
		(void)IVS_SDK_ReleaseBuffer(pDeviceStatusXml);
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	(void)IVS_SDK_ReleaseBuffer(pDeviceStatusXml);
	return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_ExGetDeviceInfo(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(NULL == pReqXml)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Ex Get Device Info Failed.", "Reason:pReqXml is NULL.");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	IVS_INT32 iRet = IVS_SDK_ExGetDeviceInfo(m_iSDKSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Ex Get Device Info", "failed");
		return iRet;
	}
	return iRet;
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetDeviceEncodeCapabilities(LPCTSTR pDevCode)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CHECK_POINTER(pDevCode,NULL);
	CString strResult;
	CXml xmlRsp;

	IVS_INT32 iRet = IVS_FAIL;
	IVS_UINT32 uiBufSize = sizeof(IVS_STREAM_INFO_LIST);
	IVS_CHAR* pStreamInfoListBuf = IVS_NEW(pStreamInfoListBuf, uiBufSize);
	if (NULL == pStreamInfoListBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);

		BP_RUN_LOG_ERR(iRet,"Get Device Encode Capabilities.", "Reason:New stStreamInfoList Memory Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	memset(pStreamInfoListBuf, 0, uiBufSize);
	IVS_STREAM_INFO_LIST* pStreamInfoList = (IVS_STREAM_INFO_LIST*)pStreamInfoListBuf;//lint !e826

	iRet = IVS_SDK_GetDeviceEncodeCapability(m_iSDKSessionID,pDevCode,pStreamInfoList);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pStreamInfoList,MUILI);
		BP_RUN_LOG_ERR(iRet, "Get Camera Encode Capabilities", "failed");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	IVS_UINT32 uiXmlLen = 0;
	COCXDeviceMgr::GetCameraEncodeCapabilities(pStreamInfoList,iRet,xmlRsp);
	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	IVS_DELETE(pStreamInfoList,MUILI);
	
	return strResult.AllocSysString();
}

ULONG FAR EXPORT CIVS_OCXPlayerCtrl::XObjSafe::AddRef() 
{//lint !e18 ATL实现接口不要PClint
	METHOD_PROLOGUE(CIVS_OCXPlayerCtrl, ObjSafe)//lint !e826
		return pThis->ExternalAddRef();  
}  
ULONG FAR EXPORT CIVS_OCXPlayerCtrl::XObjSafe::Release()  
{//lint !e18 ATL实现接口不要PClint
	METHOD_PROLOGUE(CIVS_OCXPlayerCtrl, ObjSafe)//lint !e826  
		return pThis->ExternalRelease();  
}  
HRESULT FAR EXPORT CIVS_OCXPlayerCtrl::XObjSafe::QueryInterface(  
	REFIID iid, void FAR* FAR* ppvObj)  
{//lint !e18 ATL实现接口不要PClint
	METHOD_PROLOGUE(CIVS_OCXPlayerCtrl, ObjSafe)//lint !e826 
		return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);  
}  
const DWORD dwSupportedBits =   
	INTERFACESAFE_FOR_UNTRUSTED_CALLER |  
	INTERFACESAFE_FOR_UNTRUSTED_DATA;  
const DWORD dwNotSupportedBits = ~ dwSupportedBits;  

HRESULT STDMETHODCALLTYPE   
	CIVS_OCXPlayerCtrl::XObjSafe::GetInterfaceSafetyOptions(   
	REFIID riid,  
	DWORD __RPC_FAR *pdwSupportedOptions,  
	DWORD __RPC_FAR *pdwEnabledOptions)  
{  
	METHOD_PROLOGUE(CIVS_OCXPlayerCtrl, ObjSafe)//lint !e826  
		HRESULT retval = ResultFromScode(S_OK);  

	IUnknown FAR* punkInterface;  
	retval = pThis->ExternalQueryInterface(&riid,   
		(void * *)&punkInterface);//lint !e731 !e713
	if (retval != E_NOINTERFACE) {
		punkInterface->Release();   
	}  

	*pdwSupportedOptions = *pdwEnabledOptions = dwSupportedBits;  
	return retval;   
}  

HRESULT STDMETHODCALLTYPE   
	CIVS_OCXPlayerCtrl::XObjSafe::SetInterfaceSafetyOptions(   
	REFIID riid,  
	DWORD dwOptionSetMask,  
	DWORD dwEnabledOptions)  
{  
	METHOD_PROLOGUE(CIVS_OCXPlayerCtrl, ObjSafe)//lint !e826  

		IUnknown FAR* punkInterface;  
	pThis->ExternalQueryInterface(&riid, (void * *)&punkInterface);  
	if (punkInterface) {   
		punkInterface->Release();  
	}  
	else {  
		return ResultFromScode(E_NOINTERFACE);  
	}   
	if (dwOptionSetMask & dwNotSupportedBits) {   
		return ResultFromScode(E_FAIL);  
	}  

	dwEnabledOptions &= dwSupportedBits;    
	if ((dwOptionSetMask & dwEnabledOptions) !=  
		dwOptionSetMask) {  
			return ResultFromScode(E_FAIL);  
	}          

	return ResultFromScode(S_OK);  
}  

/*************************************************************************
* name       : IVS_OCX_RefreshCameraList
* description: 刷新摄像机列表，同步下级域摄像机列表
* input      : LPCTSTR pReqXml
* output     : NA
* return     : LONG
* anthor     ：
* remark     : NA
*************************************************************************/
LONG CIVS_OCXPlayerCtrl::IVS_OCX_RefreshCameraList()const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("");

    return COCXDeviceMgr::RefreshCameraList(m_iSDKSessionID);
}

/*************************************************************************
* name       : IVS_OCX_SetLogLevel
* description: 设置日志级别
* input      : LPCTSTR pReqXml
* output     : NA
* return     : LONG
* anthor     ：
* remark     : NA
*************************************************************************/
void CIVS_OCXPlayerCtrl::IVS_OCX_SetLogLevel(ULONG ulLogLevel) const
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    IVS_INFO_TRACE("");
    IVS_SDK_SetLogLevel(ulLogLevel);
}

BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetCUVersion(LPCTSTR pReqXml)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult;

    if (NULL == pReqXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Get CU Version Failed.", "Reason:pReqXml is NULL.");
        COCXXmlProcess::GetErrString(strResult, IVS_PARA_INVALID);
        return strResult.AllocSysString();//lint !e1013 !e64
    }
    IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

    CXml reqXml;
    //判断是否解析正确
    if (!reqXml.Parse(pReqXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "IVS OCX Get CU Version Failed.", "Reason:Parse pReqGetList Failed.");
        COCXXmlProcess::GetErrString(strResult, IVS_XML_INVALID);
        return strResult.AllocSysString();//lint !e1013 !e64
    }

	IVS_INT32 iRet = IVS_FAIL;
    iRet = COCXUpgradeMgrMgr::GetCUVersion(m_iSDKSessionID, reqXml, strResult);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX Get CU Version Failed.", "Reason:Parse pReqGetList Failed.");
		COCXXmlProcess::GetErrString(strResult, iRet);
		return strResult.AllocSysString();//lint !e1013 !e64
	}

    return strResult.AllocSysString();
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartRealPlay_Async(LPCTSTR pMediaPara, LPCTSTR pCameraCode, LONG lWndID, ULONG lTransID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT(NULL != pMediaPara && NULL != pCameraCode);
	CHECK_POINTER(pMediaPara, IVS_PARA_INVALID);   //lint !e774
    CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);   //lint !e774

	IVS_INFO_TRACE("OCX Address: %x,XML: %s,CAMERA: %s,WndID: %ld", this,pMediaPara,pCameraCode,lWndID);
    if ('\0' == pCameraCode[0])
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start Real Play Failed.", "Reason:CameraCode is Empty");
        return IVS_PARA_INVALID;
    }

    CVideoPane *pVideoPane = GetVideoPanel((ULONG)lWndID);
    ASSERT(NULL != pVideoPane);
	if (NULL == pVideoPane)	  //lint !e774
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start Real Play Failed.", "Reason:Invalid WinID:%ld.", lWndID);
        return IVS_PARA_INVALID;
    }
    

	// 判断窗口是不是被局部放大占用，如果被占用，就关闭局部放大
	if ( BOTTOMBAR_MAGNIFY_OPEN == pVideoPane->GetDstMagnifyState())
	{
		CVideoPane* pSrcVideoPane = pVideoPane->GetMagnifySrcPane();
		if (NULL != pSrcVideoPane)
		{
			::SendMessage(pSrcVideoPane->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (LONG)pVideoPane);
		}
	}

	pVideoPane->CreateBar();

	//如果已经在快速录像回放了
	if (pVideoPane->GetShowInstantStatus())
	{
		//pVideoPane->ResumeRealPlay();
		(void)IVS_OCX_StopRealPlay(lWndID);
		//(void)StopRealPlayByPane(pVideoPane);
	}

    IVS_REALPLAY_PARAM stRealplayParam;
    memset(&stRealplayParam, 0, sizeof(stRealplayParam));

	if (IVS_SUCCEED != COCXRealPlayXMLProcess::GetMediaParamXML(pMediaPara, stRealplayParam))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start Real Play Failed.", "Reason:MediaPara Xml Invalid");
		return IVS_PARA_INVALID;
	}

    BOOL bEnableGPU = FALSE;

    std::string ParamString = m_pLocalConfig.GetCDATA2XML(LOCAL_CONFING_FILE_NAME,NETWORK_CONFIG_ELEMENT);
    BP_RUN_LOG_INF("IVS OCX Get Local NetWork Config.","Value:%s", ParamString.c_str());
    if (IVS_SUCCEED == COCXRealPlayXMLProcess::EnableGPUParseClientConfig(ParamString.c_str(), bEnableGPU))
    {
        (void)IVS_OCX_EnableGPU(bEnableGPU);
    }//lint !e613
	else
	{
		BP_RUN_LOG_INF("IVS OCX Enable GPU Parse Client Config failed.","bEnableGPU:%d", bEnableGPU);
	}

	IVS_INT32 iRet = pVideoPane->StartRealSteamAsync(stRealplayParam, pCameraCode, TRUE, VIDEO_PLAY_STATE_REALTIME, lTransID);


    CVideoPane* pVideoPane1 = GetVideoPaneMgr()->GetWndPointerByID((ULONG)lWndID);//lint !e613
	CHECK_POINTER(pVideoPane1, IVS_OPERATE_MEMORY_ERROR);

	pVideoPane1->SetRealtimeTalkbackState(BOTTOMBAR_TALKBACK_CLOSE);
    CTranVideoRealtimeBar * pRealTimeToolBar = pVideoPane1->GetVideoRealtimeBar();
    if (NULL != pRealTimeToolBar)
    {
         pRealTimeToolBar->SetTalkbackCtrlState(false);
    }
        

	// 判断这个摄像机是不是被开启了语音，如果开启了 需要设置一个按钮按下的状态
    if (0 == strcmp(pCameraCode, m_strVoiceTalkbackCameraCode.c_str()))
	{
		// 查找是否有开启实况的窗口，同步状态

        if (FALSE == pVideoPane1->GetVideoPaneMgr()->IsTalkback())
        {
            // 设置语音开启的状态
            // modi by wKF71044 at [2013-7-5 10:15:45]取消同步
			m_pVoiceTalkbackVideoPane = pVideoPane1;

			m_pVoiceTalkbackVideoPane->SetRealtimeTalkbackState(BOTTOMBAR_TALKBACK_OPEN);
             
			CTranVideoRealtimeBar* pRealtimeBar = m_pVoiceTalkbackVideoPane->GetVideoRealtimeBar();
			if (NULL != pRealtimeBar)
			{
				pRealtimeBar->SetTalkbackCtrlState(true);
			}
        }
	}
	else
	{
		if (NULL != m_pVoiceTalkbackVideoPane && pVideoPane1->GetPaneID() == m_pVoiceTalkbackVideoPane->GetPaneID())
		{
			m_pVoiceTalkbackVideoPane = NULL;
		}
	}

	return iRet;

#if 0
	DoButtonFilter(lWndID);
	SetFocus();
	return iRet;
#endif 
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopRealPlay_Async(LONG lWndID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IVS_INFO_TRACE("OCX Address: %x,WndID: %ld", this,lWndID);

	CVideoPane *pVideoPane = GetVideoPanel((ULONG)lWndID);
	//ASSERT(NULL != pVideoPane);
	if (NULL == pVideoPane)  //lint !e774
	{
	    BP_RUN_LOG_ERR(IVS_FAIL,"IVS OCX Stop Real Play Failed.", "Reason:Invalid WndID:[%ld]", lWndID);
		return IVS_FAIL;
	}

	//如果已经在快速录像回放了
	if (pVideoPane->GetShowInstantStatus())
	{
		pVideoPane->ResumeRealPlay();
	}
    
	return (LONG)StopRealPlayByPaneEx(pVideoPane);
}


ULONG CIVS_OCXPlayerCtrl::StopRealPlayByPaneEx(CVideoPane * pVideoPane)
{
	IVS_INFO_TRACE("OCX Address: %x", this);
	ASSERT(NULL != pVideoPane);
    CHECK_POINTER(pVideoPane, IVS_OPERATE_MEMORY_ERROR); //lint !e774

	/*
	 * DTS2014021302165 
	 * 去除panel的全局注册
	 */
	CGlobalSoundMgr &SoundMgr = CGlobalSoundMgr::Instance();
	if (SoundMgr.GetSoundingPanel() == pVideoPane)
	{
		SoundMgr.Reset();
	}
	
	//停止局部放大,这个不能放在后面，放在后面，按钮无法响应 add by z00193167
	if (MAGNIFY_OPEN == pVideoPane->GetRealtimeMagnifyState())
	{
		::SendMessage(pVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), 
			WM_COMMAND, MAKEWPARAM(IDC_BTN_TOOL_REALTIME_MAGNIFY, BN_CLICKED), 
			(LPARAM)pVideoPane->GetVideoRealtimeBar()->GetMagnifyBtnHwnd());
	}

	//停止语音对讲 add by z00193167
	if (BOTTOMBAR_TALKBACK_OPEN == pVideoPane->GetRealtimeTalkbackState())
	{
 		pVideoPane->SetRealtimeTalkbackState(BOTTOMBAR_TALKBACK_CLOSE);
	}

	//停止随路语音 add by z00193167
	if (BOTTOMBAR_SOUND_OPEN == pVideoPane->GetRealtimeSoundState())
	{
		pVideoPane->GetVideoRealtimeBar()->StopSound();
 		pVideoPane->GetVideoRealtimeBar()->DeleteSoundCtrl();
	}

	// 关闭云镜 add by z00193167
	if (BOTTOMBAR_PTZ_OPEN == pVideoPane->GetRealtimePtzState())
	{
		::SendMessage(pVideoPane->GetVideoRealtimeBar()->GetSafeHwnd(), WM_COMMAND, 
			MAKEWPARAM(IDC_BTN_TOOL_REALTIME_PTZ, BN_CLICKED), 
			(LPARAM)pVideoPane->GetVideoRealtimeBar()->GetPTZBtnHwnd());
	}

    if (VIDEO_PLAY_STATE_NOT_PLAYING == pVideoPane->GetPlayState())
    {
        BP_RUN_LOG_ERR(IVS_FAIL, "IVS OCX Stop Real Play By Pane Failed.", "Reason:the window is not playing");
		pVideoPane->ResetPane();
		return IVS_SUCCEED;
    }

	// 如果原来被占用成了放大，先删掉这个放大，等停止后再恢复一个窗口回去
	if (BOTTOMBAR_MAGNIFY_OPEN == pVideoPane->GetDstMagnifyState())
	{
		// 删除掉源窗口的信息
		::SendMessage(pVideoPane->GetMagnifySrcPane()->GetSafeHwnd(), WM_MAGNIFY_CLOSE, 0, (LONG)(pVideoPane));
	}

	// 停止轨迹叠加
	(void)pVideoPane->DisableOverlay(m_iSDKSessionID, pVideoPane->GetPlayHandle());

    //IVS_INT32 ulRet = IVS_FAIL;
    IVS_SERVICE_TYPE enServiceType = pVideoPane->GetServiceType();
    if (SERVICE_TYPE_REALVIDEO == enServiceType)
    {

		if (pVideoPane->GetIsLocalRecord())
		{
			// 若本地录像打开，且由本窗口触发则关闭本地录像
			if (pVideoPane->GetIsLocalRecordTrigger())
			{
				(void) IVS_OCX_StopLocalRecord(static_cast<LONG>(pVideoPane->GetPaneID()));
				PostLocRecEventGeneric(1, (int)pVideoPane->GetPaneID()); // 1 为录像停止
				Sleep(200); // 等待最后一个录像结束时间上报
			}
			pVideoPane->GetVideoRealtimeBar()->SetRecordBtnState(false);
		}

		// 如果当前视频打开随路语音，要关闭
		if (pVideoPane->IsSoundOn())
		{
			IVS_SDK_StopSound(m_iSDKSessionID, pVideoPane->GetPlayHandle());
		}

		pVideoPane->StopRealStreamAsync();
    }

	ReportStopPlayEvent(pVideoPane->GetPaneID());      // 上报播放停止事件
    pVideoPane->SetServiceType(SERVICE_TYPE_REALVIDEO);// 默认初始化为实况;

    //如果是由于控件cleanUp而触发的stopplay，则不会重置窗格
    if (m_bIsInit)
    {
        // 把对应窗格状态清空
        if ((!pVideoPane->GetResumeState())
            && (WND_TYPE_NORMAL != pVideoPane->GetWndType()))
        {
            pVideoPane->SetTitleButtonPic(false);
        }
        pVideoPane->ResetPane();
        pVideoPane->SetRealtimePlayPic(BUTTON_UNABLE);
        pVideoPane->SetCameraType(CAMERA_TYPE_CAVITY_FIXED);
    }
	return IVS_SUCCEED;
}

LONG CIVS_OCXPlayerCtrl::IVS_OCX_StartRealPlayTVWall_Async(ULONG ulTVWallID, LPCTSTR pCameraCode, LPCTSTR pRealplayParam, ULONG ulWndID, ULONG ulTransID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(pCameraCode,IVS_PARA_INVALID);
    CHECK_POINTER(pRealplayParam,IVS_PARA_INVALID);
    CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
    IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,CameraCode: %s,RealplayParam: %s,WndID: %lu, TransID: %lu.", this,ulTVWallID,pCameraCode,pRealplayParam,ulWndID, ulTransID);

    if (ulWndID < TVWALL_WND_ID_MIN  ||  ulWndID > TVWALL_WND_ID_MAX)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "start real play error, wnd id invalid!", "");
        return IVS_PARA_INVALID;
    }
    
    return DealStartRealPlayTVWall(ulTVWallID, pCameraCode, pRealplayParam, ulWndID, FALSE, ulTransID);
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_StopRealPlayTVWall_Async(ULONG ulTVWallID, ULONG ulWndID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
    IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %lu", this,ulTVWallID,ulWndID);

    if (ulWndID < TVWALL_WND_ID_MIN  ||  ulWndID > TVWALL_WND_ID_MAX)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "start real play error, wnd id invalid!", "");
        return IVS_PARA_INVALID;
    }

    //ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
    TVWall_VPANEID vPaneID;
    if(!m_pVideoPaneMgr->GetVirtualIDByRealID(ulWndID,vPaneID))
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop RealPlay TVWall Failed.", "Reason: WndID Not Exist.");
        return IVS_PARA_INVALID;
    }

    if (m_TvwallChannelState[ulWndID - 1].bCleanupTVWall)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "start real play error, wnd id invalid!", "");
        return IVS_FAIL;
    }

    //清除链表
    m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.Lock();
    std::list<START_TVWALL_INFO*>::iterator ite = m_TvwallChannelState[ulWndID - 1].StartTvwallInfo.begin();
    while(m_TvwallChannelState[ulWndID - 1].StartTvwallInfo.end() != ite)
    {
        START_TVWALL_INFO* pTempRealPlay = *ite;
        if (NULL == pTempRealPlay)
        {
            BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop RealPlay TVWall Failed.", "Reason: Pointer Is NULL.");
			ite++;
            continue;
        }

        pTempRealPlay->bDel = TRUE;
        //NotifyStartRealPlayTVWallRes(ulTVWallID, ulWndID, IVS_FAIL, pTempRealPlay->ulTransID);
		ite++;
    }
    m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.UnLock();

    //此窗格的正在停止，直接返回成功
    m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();
    if (m_TvwallChannelState[ulWndID - 1].bStopInterfaceRun)
    {
        m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();
        BP_DBG_LOG("stop real play, is stoping!");
        return IVS_SUCCEED;
    }
    m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();

    TVWALL_THREAD_INFO* pThreadInfo = IVS_NEW(pThreadInfo);
    CHECK_POINTER(pThreadInfo, IVS_ALLOC_MEMORY_ERROR);

    pThreadInfo->pOcx = this;
    pThreadInfo->ulWndID = ulWndID;
    pThreadInfo->ulTVWallID = ulTVWallID;

	m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();
    m_TvwallChannelState[ulWndID - 1].bStopThreadWaitRun = true;
	m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();

    BP_DBG_LOG("CIVS_OCXPlayerCtrl::IVS_OCX_StopRealPlayTVWall_Async: ulWndID = %d, pTemp = 0x%p, pParentOcx = 0x%p.", ulWndID, pThreadInfo, this);

    IVS_INT32 iRet = (IVS_INT32)_beginthread(StopRealPlayTVWallThreadFun, 0, pThreadInfo);
    if (-1 == iRet) //-1为失败，其他值成功
    {
        IVS_DELETE(pThreadInfo);
        BP_RUN_LOG_ERR(IVS_CREATE_THREAD_ERROR, "stop real play error, create thread fail!", "");
        return IVS_CREATE_THREAD_ERROR;
    }

    return IVS_SUCCEED;
}

void __cdecl CIVS_OCXPlayerCtrl::StartRealPlayTVWallThreadFun(LPVOID lpvoid)
{
    BP_DBG_LOG("Enter StartRealPlayTVWallThreadFu.");

    CHECK_POINTER_VOID(lpvoid);
    TVWALL_THREAD_INFO* pTemp = (TVWALL_THREAD_INFO*)lpvoid;
    ULONG ulWndID = pTemp->ulWndID;

    CHECK_POINTER_VOID(pTemp->pOcx);
    CIVS_OCXPlayerCtrl* pParentOcx= pTemp->pOcx;

    BP_DBG_LOG("CIVS_OCXPlayerCtrl::StartRealPlayTVWallThreadFun: ulWndID = %d, pTemp = 0x%p, pParentOcx = 0x%p.", ulWndID, pTemp, pParentOcx);

    IVS_DELETE(pTemp);
    pTemp = NULL;

    if (ulWndID < TVWALL_WND_ID_MIN  ||  ulWndID > TVWALL_WND_ID_MAX)
    {
        BP_DBG_LOG("StartRealPlayTVWallThreadFu: err");
        return;
    }

    pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();
    pParentOcx->m_TvwallChannelState[ulWndID - 1].bStartThreadRun = true;
    pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();

    while (!pParentOcx->m_TvwallChannelState[ulWndID - 1].bCleanupTVWall)
    {
        pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.Lock();
        if (pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfo.empty())
        {
            pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.UnLock();
            BP_DBG_LOG("start real play in thread, no start info, no need go on!");
            break;
        }

        START_TVWALL_INFO* pRealPlay = (START_TVWALL_INFO*)pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfo.front/*top*/();
        if (NULL == pRealPlay)
        {
            pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.UnLock();
            BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start RealPlay TVWall Failed.", "Reason: Pointer Is NULL.");
            continue;
        }

        //如果最后一个插入的数据都要删除，那么直接清除全部数据，然后退出
        bool bEnd = false;
        if (TRUE != pRealPlay->bDel)
        {
            pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfo.pop_front/*pop*/();
        }
        else
        {
            bEnd = true;
        }

        //扔掉剩下的那些
        std::list<START_TVWALL_INFO*>::iterator ite = pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfo.begin();
        for (; pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfo.end() != ite; ite++)
        {
            START_TVWALL_INFO* pTempRealPlay = *ite;
            if (NULL == pTempRealPlay)
            {
                BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop RealPlay TVWall Failed.", "Reason: Pointer Is NULL.");
                continue;
            }

            //if (TRUE != pTempRealPlay->bDel)
            //{
            //    pParentOcx->NotifyStartRealPlayTVWallRes(pTempRealPlay->ulTVWallID, ulWndID, IVS_FAIL, pTempRealPlay->ulTransID);
            //}
            
            IVS_DELETE(pTempRealPlay);
        }
        pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfo.clear();
        pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.UnLock();

        if (bEnd)
        {
            break;
        }

        CIVS_OCXPlayerCtrl* pOcx = pRealPlay->pOcx;
        ULONG ulTransID = pRealPlay->ulTransID;
        ULONG ulTVWallID = pRealPlay->ulTVWallID;
        LPCTSTR pCameraCode = pRealPlay->strCameraCode;
        LPCTSTR pRealplayParam = pRealPlay->strRealplayParam;

		if (NULL == pCameraCode  ||  NULL == pOcx  ||  0 == pRealPlay->strCameraCode.GetLength()  ||  0 == pRealPlay->strRealplayParam.GetLength())
        {
            BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start RealPlay TVWall Failed.", "Reason: Para Is Invalid.");
            IVS_DELETE(pRealPlay);
            pParentOcx->NotifyStartRealPlayTVWallRes(ulTVWallID, ulWndID, IVS_PARA_INVALID, ulTransID);
            continue;
        }

        //停止掉之前的播放
        pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();
        if (pParentOcx->m_TvwallChannelState[ulWndID - 1].bWndPlaying)
        {
            pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();
            (void)pOcx->SynStopRealPlayTVWall(ulTVWallID, ulWndID);
        }
        else
        {
            pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();
        }

        IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,CameraCode: %s,RealplayParam: %s,WndID: %lu", pOcx,ulTVWallID,pCameraCode,pRealplayParam,ulWndID);
        (void)pOcx->IVS_OCX_SetTVWallPaneStatus(ulTVWallID, ulWndID, TVWALL_STATUS_START);

        ULONG ulCurTVWallID = pOcx->m_pVideoPaneMgr->GetCurTVWallID();

        //设置电视墙窗格类型为实况
        if (ulCurTVWallID == ulTVWallID)
        {
            pOcx->m_pVideoPaneMgr->SetTVWallWndType(WND_TYPE_REALTIME);
        }

        //ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
        TVWall_VPANEID vPaneID;
        if(!pOcx->m_pVideoPaneMgr->GetVirtualIDByRealID(ulWndID,vPaneID))
        {
            BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start RealPlay TVWall Failed.", "Reason: WndID Not Exist.");
            IVS_DELETE(pRealPlay);
            pOcx->NotifyStartRealPlayTVWallRes(ulTVWallID, ulWndID, IVS_PARA_INVALID, ulTransID);
            continue;
        }

        //找到窗格中的VideoPane并创建Bar
        CVideoPane *pVideoPane = pOcx->GetVideoPanel((ULONG)ulWndID);
        ASSERT(NULL != pVideoPane);
        if (NULL == pVideoPane)	  //lint !e774
        {
            BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start RealPlay TVWall Failed.", "Reason:Invalid WinID:%ld.", ulWndID);
            IVS_DELETE(pRealPlay);
            pOcx->NotifyStartRealPlayTVWallRes(ulTVWallID, ulWndID, IVS_PARA_INVALID, ulTransID);
            continue;
        }

        IVS_INT32 iRet = COCXTVWallMgr::StartRealPlayTVWall(m_iSDKSessionID, ulTVWallID, pCameraCode, pRealplayParam, vPaneID);
        if(IVS_SUCCEED!=iRet)
        {
            if (ulCurTVWallID == ulTVWallID)
            {/*////
                (void)pOcx->IVS_OCX_SetTVWallPaneStatus(ulTVWallID, ulWndID, TVWALL_STATUS_NONE);
                pOcx->m_pVideoPaneMgr->SetTVWallWndType(WND_TYPE_NORMAL);*/
            }
            //pVideoPane->ResetPane();
            BP_RUN_LOG_ERR(iRet, "IVS OCX Start RealPlay TVWall Failed.", "Reason: OCX TVWallMgr Start RealPlay TVWall Failed.");
        }
        else
        {
            if (ulCurTVWallID == ulTVWallID)
            {
                pOcx->PostMessage(WM_TVWALL_START_REALPLAY_TVWALL_END, (WPARAM)ulTVWallID, (LPARAM)ulWndID);
            }
        }

        IVS_DELETE(pRealPlay);

        pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();
        pParentOcx->m_TvwallChannelState[ulWndID - 1].bWndPlaying = true;
        pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();

		//如果CU已经又在这个窗格开启上墙，那么就不返回消息了
		pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.Lock();
		if (pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfo.empty())
		{
		    pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.UnLock();
			pOcx->NotifyStartRealPlayTVWallRes(ulTVWallID, ulWndID, iRet, ulTransID);
		}
		else
		{
			pParentOcx->m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.UnLock();
		}
		
        BP_DBG_LOG("StartRealPlayTVWallThreadFu: end.");
    }

    pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();
    pParentOcx->m_TvwallChannelState[ulWndID - 1].bStartThreadRun = false;
    pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();

    _endthread();
}

void __cdecl CIVS_OCXPlayerCtrl::StopRealPlayTVWallThreadFun(LPVOID lpvoid)
{
    BP_DBG_LOG("Enter StopRealPlayTVWallThreadFu.");

    CHECK_POINTER_VOID(lpvoid);
    TVWALL_THREAD_INFO* pThreadInfo = (TVWALL_THREAD_INFO*)lpvoid;

    CHECK_POINTER_VOID(pThreadInfo->pOcx);
    CIVS_OCXPlayerCtrl* pParentOcx = pThreadInfo->pOcx;

    pParentOcx->m_TvwallChannelState[pThreadInfo->ulWndID - 1].TvWallWndLock.Lock();
    pParentOcx->m_TvwallChannelState[pThreadInfo->ulWndID - 1].bStopInterfaceRun = true;
    pParentOcx->m_TvwallChannelState[pThreadInfo->ulWndID - 1].TvWallWndLock.UnLock();

    ULONG ulWndID = pThreadInfo->ulWndID;
    ULONG ulTVWallID = pThreadInfo->ulTVWallID;
    CIVS_OCXPlayerCtrl* pOcx = pThreadInfo->pOcx;

    IVS_DELETE(pThreadInfo);
    pThreadInfo = NULL;

    CHECK_POINTER_VOID(pOcx->m_pVideoPaneMgr);
    IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %lu", pOcx,ulTVWallID,ulWndID);
    //ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
    TVWall_VPANEID vPaneID;
    if(!pOcx->m_pVideoPaneMgr->GetVirtualIDByRealID(ulWndID,vPaneID))
    {
        pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();
        pParentOcx->m_TvwallChannelState[ulWndID - 1].bStopInterfaceRun = false;
        pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop RealPlay TVWall Failed.", "Reason: WndID Not Exist.");
        _endthread();
        return;
    }

    ULONG ulCurTVWallID = pOcx->m_pVideoPaneMgr->GetCurTVWallID();

    IVS_INT32 iRet = COCXTVWallMgr::StopRealPlayTVWall(m_iSDKSessionID, ulTVWallID, vPaneID);
    if(IVS_SUCCEED!=iRet)
    {
        BP_RUN_LOG_ERR(iRet, "IVS OCX Stop RealPlay TVWall Failed.", "Reason: OCX TVWallMgr Stop RealPlay TVWall Failed.");
	}

	if (ulCurTVWallID == ulTVWallID)
	{
		//等到开始启动上墙线程退出后才发刷新背景的消息，不然会出现背景错误的情况
		bool bSendMsg = false;
		for (int i = 0; i < 1000; i++)
		{
			pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();

			//本窗格的上一次开启上墙流程还没走完
			if (!pParentOcx->m_TvwallChannelState[ulWndID - 1].bStartThreadRun ||
				pParentOcx->m_TvwallChannelState[ulWndID - 1].bCleanupTVWall) 
			{
				pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();
				pOcx->PostMessage(WM_TVWALL_STOP_REALPLAY_TVWALL_END, (WPARAM)ulTVWallID, (LPARAM)ulWndID);
				bSendMsg = true;
				break;
			}
			else
			{
				pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();
				Sleep(10);
			}
		}

		if (!bSendMsg)
		{
			pOcx->PostMessage(WM_TVWALL_STOP_REALPLAY_TVWALL_END, (WPARAM)ulTVWallID, (LPARAM)ulWndID);
		}
	}

    pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();
    pParentOcx->m_TvwallChannelState[ulWndID - 1].bStopInterfaceRun = false;
    pParentOcx->m_TvwallChannelState[ulWndID - 1].bWndPlaying = false;
	pParentOcx->m_TvwallChannelState[ulWndID - 1].bStopThreadWaitRun = false;
    pParentOcx->m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();

    //检查是否需要启动开始上墙的线程
    (void)pParentOcx->DealStartRealPlayTVWall(ulTVWallID, NULL, NULL, ulWndID, TRUE, 0);

    _endthread();
}

LONG CIVS_OCXPlayerCtrl::DealStartRealPlayTVWall(ULONG ulTVWallID, LPCTSTR pCameraCode, LPCTSTR pRealplayParam, ULONG ulWndID, BOOL bStopInterfaceSend, ULONG ulTransID)
{
    IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %lu,TransID: %lu", this, ulTVWallID, ulWndID, ulTransID);

    if (m_TvwallChannelState[ulWndID - 1].bCleanupTVWall)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "start real play error, wnd id invalid!", "");
        return IVS_FAIL;
    }

    m_TvwallChannelState[ulWndID - 1].StartTvWallTheadLock.Lock();

    //如果是停止的时候调用这个接口，那么必须要链表中有开启电视墙的数据才继续走下面的流程
    m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.Lock();
    if (m_TvwallChannelState[ulWndID - 1].StartTvwallInfo.empty() && bStopInterfaceSend)
    {
        m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.UnLock();
        m_TvwallChannelState[ulWndID - 1].StartTvWallTheadLock.UnLock();
        BP_DBG_LOG("DealStartRealPlayTVWall: no start info in list, not need start, ulWndID = %d.", ulWndID);
        return IVS_FAIL;
    }
    m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.UnLock();

    //如果是停止的时候调用这个接口，那么必须要没有停止和开始状态在进行才继续走下面流程
    if (bStopInterfaceSend)
    {
        m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();
        if (m_TvwallChannelState[ulWndID - 1].bStartThreadRun  ||  m_TvwallChannelState[ulWndID - 1].bStopInterfaceRun) 
        {
            m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();
            m_TvwallChannelState[ulWndID - 1].StartTvWallTheadLock.UnLock();
            BP_DBG_LOG("start real play, thread is run, save info, ulWndID = %d.", ulWndID);
            return IVS_FAIL;
        }
        m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();
    }

    if (!bStopInterfaceSend)
    {
        START_TVWALL_INFO* pRealPlayInfo = IVS_NEW(pRealPlayInfo);
        if (NULL == pRealPlayInfo)
        {
            m_TvwallChannelState[ulWndID - 1].StartTvWallTheadLock.UnLock();
            BP_DBG_LOG("start real play, create mem failed.");
            return IVS_ALLOC_MEMORY_ERROR;
        }

        pRealPlayInfo->pOcx = this;
        pRealPlayInfo->ulWndID = ulWndID;
        pRealPlayInfo->ulTVWallID = ulTVWallID;
        pRealPlayInfo->bDel = FALSE;
        pRealPlayInfo->bStopInterfaceSend = bStopInterfaceSend;
        pRealPlayInfo->strCameraCode = pCameraCode;
        pRealPlayInfo->strRealplayParam = pRealplayParam;
        pRealPlayInfo->ulTransID = ulTransID;

        //加入链表中
        m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.Lock();
        m_TvwallChannelState[ulWndID - 1].StartTvwallInfo.push_front(pRealPlayInfo);
        m_TvwallChannelState[ulWndID - 1].StartTvwallInfoLock.UnLock();

        m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();

        //本窗格的上一次开启、停止上墙流程还没走完
        if (m_TvwallChannelState[ulWndID - 1].bStartThreadRun  ||  m_TvwallChannelState[ulWndID - 1].bStopInterfaceRun) 
        {
            m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();
            m_TvwallChannelState[ulWndID - 1].StartTvWallTheadLock.UnLock();
            BP_DBG_LOG("start real play, thread is run, save info, ulWndID = %d.", ulWndID);
            return IVS_SUCCEED;
        }

		if (m_TvwallChannelState[ulWndID - 1].bStopThreadWaitRun)
		{
			m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();
			m_TvwallChannelState[ulWndID - 1].StartTvWallTheadLock.UnLock();
			BP_DBG_LOG("start real play, stop thread wait run, save info, ulWndID = %d.", ulWndID);
			return IVS_SUCCEED;
		}

        m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();
    }

    TVWALL_THREAD_INFO* pThreadInfo = IVS_NEW(pThreadInfo);
    if (NULL == pThreadInfo)
    {
        m_TvwallChannelState[ulWndID - 1].StartTvWallTheadLock.UnLock();
        BP_DBG_LOG("start real play, create mem failed.");
        return IVS_ALLOC_MEMORY_ERROR;
    }

    pThreadInfo->pOcx = this;
    pThreadInfo->ulWndID = ulWndID;
    pThreadInfo->ulTVWallID = ulTVWallID;

    BP_DBG_LOG("DealStartRealPlayTVWall: start thead of starting tvwall, ulWndID = %d.", ulWndID);

    IVS_INT32 iRet = (IVS_INT32)_beginthread(StartRealPlayTVWallThreadFun, 0, pThreadInfo);
    if (-1 == iRet) //-1为失败，其他值成功
    {

        IVS_DELETE(pThreadInfo);

        m_TvwallChannelState[ulWndID - 1].StartTvWallTheadLock.UnLock();
        BP_RUN_LOG_ERR(IVS_CREATE_THREAD_ERROR, "start real play error, create thread fail!", "");
        return IVS_FAIL;
    }

    m_TvwallChannelState[ulWndID - 1].StartTvWallTheadLock.UnLock();

    return IVS_SUCCEED;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnStartTVWallRealPlayEnd(WPARAM wParam, LPARAM lParam)
{
    IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %lu", this,wParam,lParam);

    ULONG ulTVWallID = (ULONG)wParam;
    ULONG ulWndID = (ULONG)lParam;

    (void)IVS_OCX_SetTVWallPaneStatus(ulTVWallID, ulWndID, TVWALL_STATUS_PLAY);

    //找到窗格中的VideoPane
    CVideoPane *pVideoPane = GetVideoPanel((ULONG)ulWndID);
    if (NULL != pVideoPane)	  //lint !e774
    {
        //pVideoPane->SetPlayState(VIDEO_PLAY_STATE_RESTARTING);
        pVideoPane->SetTvWallBgState(true);
        pVideoPane->Invalidate(TRUE);
    }

    return IVS_SUCCEED;
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnStopTVWallRealPlayEnd(WPARAM wParam, LPARAM lParam)
{
    IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %lu", this,wParam,lParam);

    ULONG ulTVWallID = (ULONG)wParam;
    ULONG ulWndID = (ULONG)lParam;

    //找到窗格中的VideoPane并创建Bar
    CVideoPane *pVideoPane = GetVideoPanel((ULONG)ulWndID);
    ASSERT(NULL != pVideoPane);
    if (NULL == pVideoPane)	  //lint !e774
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Start RealPlay TVWall Failed.", "Reason:Invalid WinID:%ld.", ulWndID);
        return IVS_FAIL;
    }

    (void)IVS_OCX_SetTVWallPaneStatus(ulTVWallID, ulWndID, TVWALL_STATUS_NONE);

    pVideoPane->SetTvWallBgState(false);
    pVideoPane->SetResumeState(false);
    pVideoPane->Invalidate(TRUE);

    return IVS_SUCCEED;
}

//停止电视上墙(同步)
LONG CIVS_OCXPlayerCtrl::SynStopRealPlayTVWall(ULONG ulTVWallID, ULONG ulWndID)
{
    //AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CHECK_POINTER(m_pVideoPaneMgr,IVS_PARA_INVALID);
    IVS_INFO_TRACE("OCX Address: %x,TVWAllID: %lu,WndID: %lu", this,ulTVWallID,ulWndID);

    m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();
    m_TvwallChannelState[ulWndID - 1].bStopInterfaceRun = true;
    m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();

    //ulWndID为真实的电视墙窗格，先根据真实的窗格找到虚拟的窗格
    TVWall_VPANEID vPaneID;
    if(!m_pVideoPaneMgr->GetVirtualIDByRealID(ulWndID,vPaneID))
    {
        m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();
        m_TvwallChannelState[ulWndID - 1].bStopInterfaceRun = false;
        m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();

        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop RealPlay TVWall Failed.", "Reason: WndID Not Exist.");
        return IVS_PARA_INVALID;
    }

    ULONG ulCurTVWallID = m_pVideoPaneMgr->GetCurTVWallID();

    IVS_INT32 iRet = COCXTVWallMgr::StopRealPlayTVWall(m_iSDKSessionID, ulTVWallID, vPaneID);
    if(IVS_SUCCEED!=iRet)
    {
        BP_RUN_LOG_ERR(iRet, "IVS OCX Stop RealPlay TVWall Failed.", "Reason: OCX TVWallMgr Stop RealPlay TVWall Failed.");
    }
    else
    {
        if (ulCurTVWallID == ulTVWallID)
        {
            this->PostMessage(WM_TVWALL_STOP_REALPLAY_TVWALL_END, (WPARAM)ulTVWallID, (LPARAM)ulWndID);

            BP_DBG_LOG("SynStopRealPlayTVWall: OCX Address: %x,TVWAllID: %lu,WndID: %lu", this,ulTVWallID,ulWndID);
        }
    }

    if (IVS_SDK_NOT_PLAY_TVWALL == iRet)
    {
        iRet = IVS_SUCCEED;
    }

    m_TvwallChannelState[ulWndID - 1].TvWallWndLock.Lock();
    m_TvwallChannelState[ulWndID - 1].bStopInterfaceRun = false;
    m_TvwallChannelState[ulWndID - 1].bWndPlaying = false;
    m_TvwallChannelState[ulWndID - 1].TvWallWndLock.UnLock();

    return iRet;
}

void CIVS_OCXPlayerCtrl::NotifyStartRealPlayTVWallRes(ULONG ulTVWallID, ULONG ulWndID, IVS_INT32 iErrCode, ULONG ulTransID)
{
    IVS_INFO_TRACE("OCX Address: %x", this);

	if (IVS_TVWALL_STOP_FLAG == iErrCode)
	{
		BP_DBG_LOG("CIVS_OCXPlayerCtrl::NotifyStartRealPlayTVWallRes: not need send msg.");
		return;
	}

    char* buf = IVS_NEW(buf, 512);
    CHECK_POINTER_VOID(buf);
    memset(buf, 0, 512);

    if(IVS_SUCCEED == COCXTVWallXMLProcess::GetInst().GetNotifyStartRealPlayTVWallRetXML(ulTVWallID, ulWndID, iErrCode, ulTransID, buf, 512))
    {
        PostMessage(WM_TVWALL_NOTIFY_START_RES, (WPARAM)buf, (LPARAM)0);
    }
    else
    {
        IVS_DELETE(buf, MUILI);
        BP_DBG_LOG("CIVS_OCXPlayerCtrl::NotifyStartRealPlayTVWallRes: can't send msg.");
    }
}

afx_msg LRESULT CIVS_OCXPlayerCtrl::OnNotifyTVWallStartRes(WPARAM wParam, LPARAM lParam)//lint !e830
{
    IVS_INFO_TRACE("OCX Address: %x", this);

    if (0 == wParam)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Notify TVWal lStart Res Failed.", "Reason: Para Invalid.");
        return IVS_PARA_INVALID;
    }

    char* pszInfo = (char*)wParam;

    //向上层发消息
    IVS_OCX_OnTVWall((LPCTSTR)pszInfo);

    BP_DBG_LOG("CIVS_OCXPlayerCtrl::NotifyStartRealPlayTVWallRes: %s.", pszInfo);

    IVS_DELETE(pszInfo, MUILI);

    return IVS_SUCCEED;
}//lint !e715 


void CIVS_OCXPlayerCtrl::CleanupTVWall(void)
{
    IVS_INFO_TRACE("OCX Address: %x", this);

    for (int i = 1; i <= TVWALL_WND_ID_MAX; i++)
    {
        m_TvwallChannelState[i - 1].TvWallWndLock.Lock();
        m_TvwallChannelState[i - 1].bCleanupTVWall = true;
        m_TvwallChannelState[i - 1].TvWallWndLock.UnLock();
    }

    int iCount = 0;
    while (iCount < 150)
    {
        int i;
        bool bNotAllTheadClose = false;
        for (i = 1; i <= TVWALL_WND_ID_MAX; i++)
        {
            m_TvwallChannelState[i - 1].TvWallWndLock.Lock();
            if (m_TvwallChannelState[i - 1].bStartThreadRun  ||  m_TvwallChannelState[i - 1].bStopInterfaceRun)
            {
                BP_DBG_LOG("CIVS_OCXPlayerCtrl::CleanupTVWall: WND = %d, bStartThreadRun = %d, bStopInterfaceRun = %d.", i, m_TvwallChannelState[i - 1].bStartThreadRun, m_TvwallChannelState[i - 1].bStopInterfaceRun);
                bNotAllTheadClose = true;
                m_TvwallChannelState[i - 1].TvWallWndLock.UnLock();
                break;
            }
            m_TvwallChannelState[i - 1].TvWallWndLock.UnLock();
        }

        if (TVWALL_WND_ID_MAX <= i  &&  !bNotAllTheadClose)
        {
            break;
        }

        Sleep(100);
        iCount++;

        BP_DBG_LOG("CIVS_OCXPlayerCtrl::CleanupTVWall: %d.", iCount);
    }

    for (int j = 1; j <= TVWALL_WND_ID_MAX; j++)
    {
        m_TvwallChannelState[j - 1].StartTvwallInfoLock.Lock();
        std::list<START_TVWALL_INFO*>::iterator ite = m_TvwallChannelState[j - 1].StartTvwallInfo.begin();
        for (; m_TvwallChannelState[j - 1].StartTvwallInfo.end() != ite; ite++)
        {
            START_TVWALL_INFO* pTempRealPlay = *ite;
            if (NULL == pTempRealPlay)
            {
                BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Stop RealPlay TVWall Failed.", "Reason: Pointer Is NULL.");
                continue;
            }

            IVS_DELETE(pTempRealPlay);
        }
        m_TvwallChannelState[j - 1].StartTvwallInfo.clear();
        m_TvwallChannelState[j - 1].StartTvwallInfoLock.UnLock();
    }
}





LONG CIVS_OCXPlayerCtrl::IVS_OCX_LoginByTicket(LPCTSTR pTicketID, LPCTSTR pServerIP, LPCTSTR pPort, ULONG ulClientType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//用户名，ip，端口，机器名
	if ((NULL == pTicketID) || (NULL == pServerIP)  || (NULL == pPort))
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Login By Ticket failed", "Reason:TicketID IP Port or MachineName is invalid");
		return IVS_PARA_INVALID;
	}
	IVS_INFO_TRACE("OCX Address: %x,TicketID: %s,ServerIP: %s,Port: %s,ClientType: %lu", this,pTicketID,pServerIP,pPort,ulClientType);

	//客户端类型校验
	if (CLIENT_TV_WALL < ulClientType)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX Login failed", "Reason:ClientType is invalid");
		return IVS_PARA_INVALID;
	}
	//获取登陆消息结构体
	IVS_LOGIN_INFO_EX sLoginInfoEx;
	memset(&sLoginInfoEx,0,sizeof(IVS_LOGIN_INFO_EX));
	IVS_INT32 iGetReqRet = COCXLoginXMLProcess::GetLoginByTicketReqInfo(pTicketID, pServerIP, pPort, ulClientType, sLoginInfoEx);
	if (IVS_SUCCEED !=iGetReqRet)
	{
		return iGetReqRet;
	}

	IVS_INT32 iRet = IVS_SDK_LoginByTicket(&sLoginInfoEx, &m_iSDKSessionID);

	//IVS_INT32 iRet = IVS_SUCCEED;
	// 设置事件回调函数
	if (IVS_SUCCEED == iRet||IVS_SMU_USER_FIRST_LOGIN == iRet || IVS_SMU_USER_PWD_NEED_MOD == iRet)
	{
		(void)IVS_SDK_SetEventCallBack(m_iSDKSessionID, FunEventCallBack);

		//获取用户信息
		IVS_UINT32 uiUserID;
		iRet = IVS_SDK_GetUserID(m_iSDKSessionID, &uiUserID);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "IVS OCX Login failed", "Reason:Get UserID failed");
			// 备份本地文件到服务器,备份失败不影响退出
			IVS_INT32 iBackData = m_pLocalConfig.BackUserData();
			if (IVS_SUCCEED != iBackData)
			{
				BP_RUN_LOG_ERR(iBackData, "IVS OCX Login failed" , "Reason:Get UserID failed then Backup UserData Failed.");
			}
			IVS_INT32 iLogout = IVS_SDK_Logout(m_iSDKSessionID);
			if (IVS_SUCCEED != iLogout)
			{
				BP_RUN_LOG_ERR(iLogout, "IVS OCX Login failed", "Reason:Get UserID failed then Logout Failed");
				return iLogout;
			}
			return iRet;
		}
		IVS_USER_INFO stUserInfo;
		memset(&stUserInfo, 0, sizeof(IVS_USER_INFO));
		iRet = IVS_SDK_GetUserInfo(m_iSDKSessionID, "", uiUserID, &stUserInfo);
		if (IVS_SUCCEED != iRet)
		{
			BP_RUN_LOG_ERR(iRet, "IVS OCX Login failed", "Reason:Get UserInfo failed");
			// 备份本地文件到服务器,备份失败不影响退出
			IVS_INT32 iBackData = m_pLocalConfig.BackUserData();
			if (IVS_SUCCEED != iBackData)
			{
				BP_RUN_LOG_ERR(iBackData, "IVS OCX Login failed" , "Reason:Get UserID failed then Backup UserData Failed.");
			}
			IVS_INT32 iLogoutRet = IVS_SDK_Logout(m_iSDKSessionID);
			if (IVS_SUCCEED != iLogoutRet)
			{
				BP_RUN_LOG_ERR(iLogoutRet, "IVS OCX Login failed", "Reason:Get UserInfo failed then Logout Failed");
				return iLogoutRet;
			}
			return iRet;
		}

		std::string strUserName = "IVS_";
		strUserName += stUserInfo.cLoginName;
		CLocalConfig::SetLoginName(strUserName);

		std::string strIP = sLoginInfoEx.stIP.cIP;
		CLocalConfig::SetIP(strIP);

		// 子进程不需要上传下载功能
		if (CLIENT_PC_CHILD != ulClientType && CLIENT_TV_WALL != ulClientType)
		{
			//启动上传、下载文件的功能
			(void)m_pLocalConfig.StartUserData(m_iSDKSessionID, strUserName, strIP);

			// 判断配置文件是否存在；若不存在，拷贝默认配置文件到用户配置文件目录
			std::string strCheckPath = m_pLocalConfig.CheckLocalConfigFileExist(LOCAL_CONFING_FILE_NAME);
			if (strCheckPath.empty())
			{
				std::string moveSrcFile; // 默认文件存放位置
				TCHAR Buffer[BUFSIZE] = {0};
				(void)GetModuleFileName(AfxGetInstanceHandle(),Buffer,BUFSIZE);
				(void)moveSrcFile.append(Buffer);
				moveSrcFile = moveSrcFile.substr(0,moveSrcFile.rfind(_T("\\")));
				moveSrcFile.append(DEFAULT_LOCALCONFIG_PATH);

				std::string moveDstFile = m_pLocalConfig.m_strFileNamePath + "\\" + LOCAL_CONFING_FILE_NAME;
				(void)CopyFile(moveSrcFile.c_str(),moveDstFile.c_str(), FALSE);
			}
		}
		//(void)CLocalConfig::instance().StartUserData(m_iSDKSessionID, strUserName, strIP);
	}
	// 加载默认国际化
	(void)IVS_OCX_SetLanguage(CSDKConfig::instance().GetDefaultLanguageValue().c_str());

	return  iRet;
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetReceiveOMUAlarm(LONG bReceive)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	IVS_INFO_TRACE("bReceive: %l", bReceive);

	IVS_INT32 iRet = IVS_SDK_SetReceiveOMUAlarm(m_iSDKSessionID, bReceive);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX SetReceiveOMUAlarm failed", "NA");
	}

	return iRet;
}


BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetVideoDiagnose(LPCTSTR pDomainCode, LPCTSTR pCameraCode)const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	IVS_INFO_TRACE("pDomainCode: %l, pCameraCode: %l", pDomainCode, pCameraCode);

	IVS_CHAR* pRspXml = NULL;
	IVS_INT32 iRet = COCXDeviceMgr::GetVideoDiagnose(m_iSDKSessionID, pDomainCode, pCameraCode, &pRspXml);
	CString strResult;
	if (NULL != pRspXml)
	{
		strResult = pRspXml;
		(void)IVS_SDK_ReleaseBuffer(pRspXml);
	}

	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS OCX GetVideoDiagnose", "IVS_SDK_GetVideoDiagnose failed");
		COCXXmlProcess::GetErrString(strResult, iRet);
	}
	
	return strResult.AllocSysString();
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetVideoDiagnose(LPCTSTR pReqXml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(pReqXml, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x,ReqXml: %s", this,pReqXml);

	IVS_INT32 iRet = COCXDeviceMgr::SetVideoDiagnose(m_iSDKSessionID, pReqXml);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS_OCX_SetVideoDiagnose", "SetVideoDiagnose failed");
	}

	return iRet;
}


BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetStreamListByCam(LPCTSTR pDomainCode, LPCTSTR pCameraCode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	if(NULL == pCameraCode)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID, "IVS OCX GetRecordList Failed.", "Reason:pCameraCode is NULL");
		return NULL;
	}
	IVS_INFO_TRACE("OCX Address: %x, DomainCode: %s, CameraCode: %s", this, pDomainCode, pCameraCode);

	CString strResult;
	IVS_UINT32 uiXmlLen = 0;

	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_FAIL);

	IVS_UINT32 uiBufSize = sizeof(IVS_STREAM_LIST_BY_CAM) + (IVS_MAX_STREAM_USER_INFO_NUM -1) * sizeof(IVS_STREAM_USER_INFO);
	IVS_CHAR* pStreamListByCamBuf = IVS_NEW(pStreamListByCamBuf, uiBufSize);
	if (NULL == pStreamListByCamBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		BP_RUN_LOG_ERR(iRet,"IVS_OCX_GetStreamListByCam", "New pStreamListBuf Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	memset(pStreamListByCamBuf, 0, uiBufSize);
	IVS_STREAM_LIST_BY_CAM *pStreamListByCam = (IVS_STREAM_LIST_BY_CAM *)pStreamListByCamBuf;

	iRet = IVS_SDK_GetStreamListByCam(m_iSDKSessionID, pDomainCode, pCameraCode, uiBufSize, pStreamListByCam);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pStreamListByCamBuf, MUILI);

		BP_RUN_LOG_ERR(iRet,"IVS_OCX_GetStreamListByCam", "IVS_SDK_GetStreamListByCam Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	//拼装响应消息结构体为XML
	iRet = COCXUserInfoMgrXMLProcess::GetStreamListByCamGetXml(pStreamListByCam, iRet, xmlRsp);
	IVS_DELETE(pStreamListByCamBuf, MUILI);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"IVS_OCX_GetStreamListByCam", "GetStreamListByCamGetXml Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult.AllocSysString();
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_ShutdownStreamByUser(LPCTSTR pDomainCode, ULONG ulUserID, LPCTSTR pCameraCode, LONG lStreamType, LONG lDuration)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	CHECK_POINTER(pCameraCode, IVS_XML_INVALID);
	IVS_INFO_TRACE("OCX Address: %x, DomainCode: %s, UserID: %lu, CameraCode: %s, StreamType: %l, Duration: %l", 
		this, pDomainCode, ulUserID, pCameraCode, lStreamType, lDuration);

	IVS_INT32 iRet = IVS_SDK_ShutdownStreamByUser(m_iSDKSessionID, pDomainCode, ulUserID, pCameraCode, lStreamType, lDuration);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "IVS_OCX_ShutdownStreamByUser", "IVS_SDK_ShutdownStreamByUser failed");
	}

	return iRet;
}


BSTR CIVS_OCXPlayerCtrl::IVS_OCX_GetStreamListByUser(LPCTSTR pDomainCode, ULONG ulUserID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	// TODO: Add your dispatch handler code here
	IVS_INFO_TRACE("OCX Address: %x, DomainCode: %s, UserID: %lu", this, pDomainCode, ulUserID);

	IVS_UINT32 uiXmlLen = 0;

	CXml xmlRsp;
	IVS_INT32 iRet = static_cast<IVS_INT32>(IVS_FAIL);

	IVS_UINT32 uiBufSize = sizeof(IVS_STREAM_LIST_BY_USER) + (IVS_MAX_STREAM_CAM_INFO_NUM -1) * sizeof(IVS_STREAM_CAM_INFO);
	IVS_CHAR* pStreamListByUserBuf = IVS_NEW(pStreamListByUserBuf, uiBufSize);
	if (NULL == pStreamListByUserBuf)
	{
		iRet = static_cast<IVS_INT32>(IVS_ALLOC_MEMORY_ERROR);
		BP_RUN_LOG_ERR(iRet,"IVS_OCX_GetStreamListByCam", "New pStreamListBuf Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}
	memset(pStreamListByUserBuf, 0, uiBufSize);
	IVS_STREAM_LIST_BY_USER *pStreamListByUser = (IVS_STREAM_LIST_BY_USER *)pStreamListByUserBuf;

	iRet = IVS_SDK_GetStreamListByUser(m_iSDKSessionID, pDomainCode, ulUserID, uiBufSize, pStreamListByUser);
	if (IVS_SUCCEED != iRet)
	{
		IVS_DELETE(pStreamListByUserBuf, MUILI);

		BP_RUN_LOG_ERR(iRet,"IVS_OCX_GetStreamListByUser", "IVS_SDK_GetStreamListByUser Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	//拼装响应消息结构体为XML
	iRet = COCXUserInfoMgrXMLProcess::GetStreamListByUserGetXml(pStreamListByUser, iRet, xmlRsp);
	IVS_DELETE(pStreamListByUserBuf, MUILI);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet,"IVS_OCX_GetStreamListByCam", "GetStreamListByCamGetXml Failed.");
		COCXXmlProcess::GetErrString(strResult,iRet);
		return strResult.AllocSysString();
	}

	strResult = xmlRsp.GetXMLStream(uiXmlLen);
	return strResult.AllocSysString();
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetMicVolume(ULONG ulVolumeValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here

	IVS_INFO_TRACE("OCX Address: %x, VolumeValue: %lu", this, ulVolumeValue);
	CHECK_POINTER(m_pVideoPaneMgr, IVS_OPERATE_MEMORY_ERROR);

	if (100 < ulVolumeValue)
	{
		BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS OCX SetMicVolume", "VolumeValue is invalid");
		return IVS_PARA_INVALID;
	}

	return m_pVideoPaneMgr->SetMicVolume(ulVolumeValue);
}


ULONG CIVS_OCXPlayerCtrl::IVS_OCX_GetMicVolume(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here

	IVS_INFO_TRACE("OCX Address: %x", this);
	CHECK_POINTER(m_pVideoPaneMgr, IVS_OPERATE_MEMORY_ERROR);

	return m_pVideoPaneMgr->GetMicVolume();
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_LocalSnapshotToDir(LONG lWndID, ULONG ulPictureFormat, LPCTSTR pFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here

	CHECK_POINTER(pFileName, IVS_PARA_INVALID);
	IVS_INFO_TRACE("OCX Address: %x, WndID: %ld, PictureFormat: %d, FileName: %s", this, lWndID, ulPictureFormat, pFileName);
	if (NULL == m_pVideoPaneMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Local Snapshot Failed.", "Reason: video pane manager is null");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	CVideoPane * pVideoPane = ((CVideoPaneMgr*)m_pVideoPaneMgr)->GetWndPointerByID((IVS_ULONG)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Local Snapshot Failed.", "Reason: pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	return pVideoPane->LocalSnapshotToDir(m_iSDKSessionID, ulPictureFormat, pFileName);
}


LONG CIVS_OCXPlayerCtrl::IVS_OCX_SetVideoEffect(LONG lWndID, LONG lBrightValue, LONG lContrastValue, LONG lSaturationValue, LONG lHueValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here

	IVS_INFO_TRACE("OCX Address: %x, WndID: %d, BrightValue: %d, ContrastValue: %d, SaturationValue: %d, HueValue: %d",
		this, lWndID, lBrightValue, lContrastValue, lSaturationValue, lHueValue);

	CVideoPane *pVideoPane = GetVideoPanel((ULONG)lWndID);
	if (NULL == pVideoPane)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "IVS OCX Set Video Effect Failed.", "Reason: pVideoPane is NULL.");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	return pVideoPane->SetVideoEffect(m_iSDKSessionID, lBrightValue, lContrastValue, lSaturationValue, lHueValue);
}
