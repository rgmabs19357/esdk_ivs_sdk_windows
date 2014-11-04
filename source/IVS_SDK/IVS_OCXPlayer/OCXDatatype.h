/*****************************************************************
 filename    :    OCXDatatype.h
 author      :    ykf75928
 created     :    2013/01/16
 description :    控件消息以及宏定义
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/16 初始版本
*****************************************************************/
#ifndef _OCX_DATATYPE__
#define _OCX_DATATYPE__

////控件错误码

#define WM_COMM_EVENT_CALLBACK (WM_USER + 600)  // 公共回调事件

#define WM_EXCEPTION_CALLBACK  (WM_USER + 1)  //异常回调消息
#define WM_ONCAPTURE_FINISHED  (WM_USER + 2) //连续抓拍结束消息
//#define WM_CLOSE_VOD_PLAY      (WM_USER + 3)  //异常发生，要求关闭点播回放
#define WM_ALARM_REPORT_CALLBACK (WM_USER + 4) //告警上报消息
#define WM_ALARM_LINKAGE_CALLBACK (WM_USER + 5) //告警联动消息
#define WM_DEVICE_REPORT_CALLBACK (WM_USER + 6) //告警上报消息
#define WM_STREA_EXCEPTION_CALLBACK (WM_USER + 7) //流异常上报消息
#define WM_DEVICE_ALARM_NOTIFY_CALLBACK (WM_USER + 8) // OMU设备告警上报
#define WM_MANUAL_REPORT_STATE_CALLBACK (WM_USER + 9) //手动录像启动/停止上报消息
#define WM_SYNC_PLAYBACK_MSG        (WM_USER + 10)
#define WM_WATER_ALARM_EXCEPTION_CALLBACK        (WM_USER + 11)
#define WM_BROADCAST_REPORT_CALLBACK        (WM_USER + 12)

#define WM_RESUME_REALPLAY_OK_MSG (WM_USER + 13) // 重连OK
#define WM_TALKBACK_REPORT_CALLBACK           (WM_USER + 14)   // 对讲

#define WM_ASYNC_REALPLAY_MSG   (WM_USER + 15)  // 窗格内异步事件
#define WM_ASYNC_EVENT_MSG      (WM_USER + 16)  // 异步事件上报

#define CAPTURE_DISK_WARNING_SPACE_MB      100  //抓拍告警磁盘空间

#define WM_KEEP_ALIVE_CALLBACK  (WM_USER + 200)  

#define WM_RELOGIN_CALLBACK	(WM_USER + 300) //重新登陆成功后上报事件
#define WM_EVENT_LOCAL_PLAYBACK_CALLBACK    (WM_USER + 400)  // 本地录像回放
#define WM_EVENT_REMOTE_PLAYBACK_CALLBACK   (WM_USER + 401)  // 平台回放事件
#define WM_USER_OFFLINE_CALLBACK   (WM_USER + 402)  // 用户下线事件

// 局部放大的自定义事件
#define WM_MAGNIFY_RECT_MOVE   (WM_USER + 500)    // 矩形移动事件
#define WM_MAGNIFY_RATE_CHANGE   (WM_USER + 501)  // 百分比改变事件
#define WM_MAGNIFY_CLOSE   (WM_USER + 502)         // 局部放大结束
#define WM_MAGNIFY_CLOSE_ALL (WM_USER + 503)    // 关闭所有的局部放大
#define WM_TRANS_BTN_DOWN (WM_USER + 504)    
#define WM_TRANS_BTN_UP (WM_USER + 505)     
#define WM_TURNCRUISE_CLOSE (WM_USER + 506)     // 关闭轮训按钮

//电视墙
#define WM_TVWALL_CALLBACK (WM_USER + 550)     // 电视墙回调事件

//Hook捕捉到退出全屏时消息
#define WM_RESET_FULL_SCREEN (WM_USER + 507)
//重新设置布局消息
#define WM_RESET_WNDLAYOUT (WM_USER + 508)

//电视墙
#define WM_TVWALL_CALLBACK (WM_USER + 550)     // 电视墙回调事件
#define WM_TVWALL_START_REALPLAY_TVWALL_END (WM_USER + 551)     // 启动电视墙实况结束
#define WM_TVWALL_STOP_REALPLAY_TVWALL_END  (WM_USER + 552)     // 停止电视墙实况结束
#define WM_TVWALL_NOTIFY_START_RES          (WM_USER + 553)     // 异步通知上墙结果

#define TIMER_ID_WATER_MASK                   100   //水印校验的基准
#define TIMER_ID_WATER_MASK_START             101   //水印校验timer开始id
#define TIMER_ID_WATER_MASK_END               164   //水印校验timer结束id

const int REALTIMEBAR_BUTTON_NUM_CS = 9;

//事件上报的宏定义
#define EVENT_MOUSE_LEFT_BUTTON_CLICK          10050 //鼠标左键点击事件
#define EVENT_MOUSE_RIGHT_BUTTON_CLICK         10051 //鼠标右键点击事件
#define EVENT_EXCHANGE_WINDOW                  10052 //窗格交换事件
#define EVENT_MAGNIFY_WINDOW                   10053 //放大窗格事件
#define EVENT_MINIFY_WINDOW                    10054 //缩小窗格事件
#define EVENT_ACTIVE_WINDOW_CHANGE             10055 //活动窗格改变事件
#define EVENT_VIDEO_INFO_BTN_CLICK             10056 //视频信息按钮点击事件
#define EVENT_VIDEO_STOP_BTN_CLICK             10057 //关闭视频按钮点击事件
#define EVENT_TOOLBAR_BTN_CLICK                10058 //工具栏按钮点击事件
#define EVENT_ENTER_FULL_SCREEN                10059 //进入全屏事件
#define EVENT_EXIT_FULL_SCREEN                 10060 //退出全屏事件
#define EVENT_BUTTON_SET_ALARM_WND             10061 //设置告警窗口事件
#define EVENT_BUTTON_DEL_ALARM_WND             10062 //退出告警窗口事件
#define EVENT_BUTTON_SET_TURNCRUISE_PRE        10063 //设置轮训窗口向前按钮事件
#define EVENT_BUTTON_DEL_TURNCRUISE_NEXT       10064 //设置轮训窗口向后按钮事件
#define EVENT_LADDER_LAYOUT_WINDOW             10065 //阶梯式布局事件
#define EVENT_LOCAL_CAPTURE_SUCCEED			   10066 // 本地抓拍成功
#define EVENT_LOCAL_CAPTURE_FAILED			   10067 // 本地抓拍失败
#define EVENT_LOCAL_CAPTURE_ALL					10068 // 本地抓拍所有窗格
#define EVENT_INTELLIGENT_SELECT_GRAPHIC		10069 // 选中图形
#define EVENT_INTELLIGENT_UNSELECT_GRAPHIC		10070 // 去选图形
#define EVENT_KEYBOARD_PRESS                   10071 //键盘按下键响应
#define EVENT_KEYBOARD_UP                      10072 //键盘键弹起响应
#define EVENT_SOUND_STATUS					   10073 // 随路语音状态
#define EVENT_LOCAL_CAPTURE_NO_SPACE		   10074 // 本地抓拍空间不足
#define EVENT_POINT_IS_OUTOF_OCX		       10075 // 鼠标Drag移动窗格到OCX之外

typedef enum 
{
	TOOLBAR_BTN_PLAY = 1,
	TOOLBAR_BTN_RECORD = 2,
	TOOLBAR_BTN_CAPTURE = 3,
	TOOLBAR_BTN_LABEL = 4,
	TOOLBAR_BTN_SOUND = 5,
	TOOLBAR_BTN_TALKBACK = 6,
	TOOLBAR_BTN_MANIFY = 7,
	TOOLBAR_BTN_PTZ = 8
}TOOlBAR_BTN_TYPE;

#endif
