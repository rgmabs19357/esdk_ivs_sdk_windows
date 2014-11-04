/*****************************************************************
 filename    :    ENUM_DEF.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    枚举定义
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#ifndef _ENUM_DEF_H_1CC6D146_6073_43D0_B822_FA383F70B5C6_
#define _ENUM_DEF_H_1CC6D146_6073_43D0_B822_FA383F70B5C6_

#ifdef __cplusplus
namespace ENUMDEF {
#endif
    typedef enum
    {
        COLOR_DARK = 1,
        COLOR_GRAY = 2	
    }COLOR_MODE;

    typedef enum
    {
        NOT_AUTO_CHANGE_LAYOUT = 0,
        AUTO_CHANGE_LAYOUT = 1	
    }AUTO_CHANGE;

    typedef enum 
    {
        PTZ_CONTROL_LEFT = 0,
        PTZ_CONTROL_RIGHT = 1,
        PTZ_CONTROL_UP = 2,
        PTZ_CONTROL_DOWN= 3,
        PTZ_CONTROL_STOP= 4
    }PTZ_CONTROL;

    typedef enum
    {
        PTZ_3D_CLOSE = 0,
        PTZ_3D_OPEN = 1	
    }PTZ_3D_STATE;

    typedef enum 
    {
        TITLEBAR_HIDE = 0,
        TITLEBAR_SHOW = 1    
    }TITLE_BAR_SHOW_STATE;

    typedef enum 
    {
        WATER_MARK_VERIFY_CLOSE = 0,
        WATER_MARK_VERIFY_OPEN = 1
    }WATER_MARK_VERIFY_STATE;

    typedef enum 
    {
        BOTTOMBAR_RECORD_CLOSE = 0,
        BOTTOMBAR_RECORD_OPEN = 1
    }BOTTOM_BAR_RECORD_STATE;

    typedef enum 
    {
        BOTTOMBAR_CAPTURE_CLOSE = 0,
        BOTTOMBAR_CAPTURE_OPEN = 1
    }BOTTOM_BAR_CAPTURE_STATE;

    typedef enum 
    {
        BOTTOMBAR_LABEL_CLOSE = 0,
        BOTTOMBAR_LABEL_OPEN = 1
    }BOTTOM_BAR_LABEL_STATE;

    typedef enum 
    {
        BOTTOMBAR_TALKBACK_CLOSE = 0,
        BOTTOMBAR_TALKBACK_OPEN = 1
    }BOTTOM_BAR_TALKBACK_STATE;

    typedef enum 
    {
        BOTTOMBAR_SOUND_CLOSE = 0,
        BOTTOMBAR_SOUND_OPEN = 1
    }BOTTOMBAR_SOUND_STATE;

    typedef enum 
    {
        BOTTOMBAR_PTZ_CLOSE = 0,
        BOTTOMBAR_PTZ_OPEN = 1
    }BOTTOM_BAR_PTZ_STATE;

    typedef enum 
    {
        BOTTOMBAR_ALARM_CLOSE = 0,
        BOTTOMBAR_ALARM_OPEN = 1
    }BOTTOM_BAR_ALARM_STATE;

	typedef enum 
	{
		BOTTOMBAR_MAGNIFY_CLOSE = 0,
		BOTTOMBAR_MAGNIFY_OPEN = 1
	}BOTTOM_BAR_MAGNIFY_STATE;

    typedef enum 
    {
        BOTTOMBAR_WALL_CLOSE = 0,
        BOTTOMBAR_WALL_OPEN = 1
    }BOTTOM_BAR_WALL_STATE;

    typedef enum 
    {
        BOTTOMBAR_ANALYSIS_CLOSE = 0,
        BOTTOMBAR_ANALYSIS_OPEN = 1
    }BOTTOM_BAR_ANALYSIS_STATE;

    typedef enum 
    {
        REALTIME_REPLAY_CLOSE = 0,
        REALTIME_REPLAY_OPEN = 1
    }REALTIME_REPLAY_STATE;

    typedef enum 
    {
        RECORD_PLAY_CLOSE = 0,
        RECORD_PLAY_OPEN = 1
    }RECORD_PLAY_STATE;

    typedef enum 
    {
        BUTTON_NORMAL  = 0,
        BUTTON_CLICKED = 1,
        BUTTON_UNABLE  = 2
    }BUTTON_STATE;

    typedef enum 
    {
        PAUSE_CLOSE = 0,
        PAUSE_OPEN = 1
    }PAUSE_STATE;

    typedef enum 
    {
        FULL_SCREEN = 1,
        NOT_FULL_SCREEN = 0
    } FULL_SCREEN_STATE;

    typedef enum
    {
        BUTTON_PIC_UNCLICK = 0,
        BUTTON_PIC_CLICKED = 1,
        BUTTON_PIC_UNABLE  = 2
    } BUTTON_PIC_STATE;

    typedef enum 
    {
        REPLAY_LOCAL    = 0,
        REPLAY_PLATFORM = 1,
        REPLAY_PU = 2
    } REPLAY_TYPE_STATE;

    typedef enum 
    {
        MAGNIFY = 1,    //放大
        MINIFY = 0      //缩小
    }MAGNIFY_MINIFY_STATE;

    typedef enum 
    {
        DRAGING = 1,
        NOT_DRAG = 0
    }DRAG_STATE;

    typedef enum 
    {
        WARNING = 1,
        NOT_WARN = 0
    }WARN_STATE;

    typedef enum 
    {
        STATE_TURN = 1,
        STATE_NAVIGATE,
        STATE_RECORD,
        STATE_ALARM,
        STATE_PLAYBACK
    }REALTIME_PIC_STATE;

    // 窗口类型
    typedef enum 
    {
        WND_TYPE_NORMAL   = 0,
        WND_TYPE_REALTIME = 1,
        WND_TYPE_RECORD   = 2,
        WND_TYPE_ANALYSIS = 3,
        WND_TYPE_REPALY   = 4,
        WND_TYPE_SENSE    = 5
    }WND_TYPE;

    // 摄像机类型
    typedef enum 
    {
        CAMERA_TYPE_CAVITY_FIXED   = 1,
        CAMERA_TYPE_CAVITY_WITHPTZ = 2,
        CAMERA_TYPE_BALL           = 3
    }CAMERA_TYPE;


    //标题栏状态图标状态
    typedef enum 
    {
        PIC_STATE_UNNEED       = 0,
        PIC_STATE_ACTIVE       = 1,
        PIC_STATE_NO           = 2,
        PIC_STATE_UNABLE       = 3
    }TITLEBAR_PIC_STATE;

    // 回放速度
    typedef enum 
    {
        PIC_SCALE_UNNEED       = 0,
        PIC_SCALE_ORDER_1      = 1,
        PIC_SCALE_ORDER_2      = 2,
        PIC_SCALE_ORDER_4      = 3,
        PIC_SCALE_ORDER_8      = 4,
        PIC_SCALE_ORDER_16     = 5,
        PIC_SCALE_ORDER_32     = 6,
        PIC_SCALE_INVERT_2     = 7,
        PIC_SCALE_INVERT_4     = 8,
        PIC_SCALE_INVERT_8     = 9,
        PIC_SCALE_INVERT_16    = 10,
        PIC_SCALE_INVERT_32    = 11,
        PIC_SCALE_DIV_2        = 12,
        PIC_SCALE_DIV_4        = 13,
        PIC_SCALE_DIV_8        = 14,
        PIC_SCALE_DIV_16       = 15,
        PIC_SCALE_DIV_32       = 16,
        PIC_SCALE_PAUSE        = 17,
        PIC_SCALE_UNABLE       = 18
    }SCALE_PIC_STATE;

    //鼠标图标
    typedef enum 
    {
        CURSOR_STATE_CLOSE     = 0,
        CURSOR_STATE_MAGNIFY   = 1,
        CURSOR_STATE_PTZ       = 2,
        CURSOR_STATE_3DPTZ     = 3
    }CURSOR_STATE;

    //工具栏按钮类型
    typedef enum 
    {
        BOTTOMBAR_PLAY     = 1,
        BOTTOMBAR_RECORD   = 2,
        BOTTOMBAR_CAPTURE  = 3,
        BOTTOMBAR_LABEL    = 4,
        BOTTOMBAR_TALKBACK = 5,
        BOTTOMBAR_SOUND    = 6,
        BOTTOMBAR_MAGNIFY  = 7,
        BOTTOMBAR_PTZ      = 8,
        BOTTOMBAR_ALARM    = 9,
        BOTTOMBAR_TVWALL   = 10,
        BOTTOMBAR_ANALYSIS = 11,
        TITLEBAR_CLOSE     = 12
    }BOTTOMBAR_BUTTON_TYPE;

	enum TranslucentWndState
	{
		TWS_NORMAL = 0,
		TWS_DISABLED,
		TWS_MOUSEOVER,
		TWS_PRESSED,
		TWS_SELECTED,

		TWS_BUTTON_NUM = TWS_SELECTED + 1,
		TWS_FORCE_DWORD = 0x7fffffff
	};

	typedef enum 
	{
		TYPE_TURNCRUISE_NOTHING = 0,
		TYPE_TURNCRUISE_LEFTBUTTON,
		TYPE_TURNCRUISE_RIFHTBUTTON,
		TYPE_TURNCRUISE_BUTH
	}TYPE_TURNCRUISE_STYLE;

	typedef enum
	{
		TYPE_WND_NOMAL,      // 正常窗口
		TYPE_WND_ALARM,      // 告警窗口
		TYPE_WND_TURNCTUISE,  // 轮训窗口
		TYPE_WND_EMAP,  // 电子地图窗口
		TYPE_WND_MAGNIFY,  // 局部放大窗口
		TYPE_WND_INSTANTBACKPLAY  // 即时回放窗口
	}TYPE_WND;

#ifdef __cplusplus
}
using namespace ENUMDEF;
#endif


#endif // _ENUM_DEF_H_1CC6D146_6073_43D0_B822_FA383F70B5C6_
