/********************************************************************
filename 	: IVS_IATrackAPI.h
author   	: z00227377
created 	: 2012/12/19
description : 智能分析轨迹显示功能，实现轨迹的增删和显示
copyright 	: Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history 	: 2012/12/19 初始版本
*********************************************************************/


#ifndef __IATRACK_API_H__
#define __IATRACK_API_H__

#include <Windows.h>

#pragma pack(push, 1)

#define IA_TEXT_LENGTH_MAX		100		// 文本最大长度
#define IA_POINTS_NUM_MAX		100		// 单个图形最大点数
#define IA_ELEMENT_NUM_MAX		60		// 轨迹信息最大数
#define IA_MESSAGE_NUM_MAX		600		// 轨迹信息列表最大长度

// 轨迹信息Layer层类型
typedef enum 
{
	IA_LAYER_TYPE_NONE,
	IA_LAYER_TYPE_ALARM,	// 轨迹信息为Alarmed
	IA_LAYER_TYPE_ROI		// 轨迹信息为ROI

}IA_LAYER_TYPE;

// 单个轨迹类型
typedef enum 
{
	IA_ELEMENT_TYPE_NONE,
	IA_ELEMENT_TYPE_TEXT,		// 轨迹类型为文本
	IA_ELEMENT_TYPE_POLYLINE	// 轨迹类型为折线图

}IA_ELEMENT_TYPE;


// 智能分析轨迹文字信息
typedef struct
{
	ULONG	ulTextScale;					// 文本缩放比例
	POINT	TextPoint;						// 文本显示位置
	ULONG	ulTextLenth;					// 文本长度
	CHAR	TextContent[IA_TEXT_LENGTH_MAX];// 文本内容
	ULONG   ulIsPercent;					// 是否百分比坐标

}IA_ALARM_TEXT;

// 智能分析轨迹图形信息
typedef struct
{
	ULONG	ulPointsNum;				// 轨迹图形点数
	POINT	Points[IA_POINTS_NUM_MAX];	// 轨迹点数组
	ULONG   ulIsPercent;				// 是否百分比坐标
    ULONG   ulDirection;                // 方向，0：无          1：单向左       2：单向右           3：双向 
}IA_ALARM_POLYLINE;

// 智能分析单个轨迹信息
typedef struct  
{
	IA_LAYER_TYPE		ulLayer;			// 单个轨迹Layer信息
	COLORREF			ulColor;			// 轨迹信息颜色
	IA_ELEMENT_TYPE		ulElementType;		// 轨迹信息类型
	IA_ALARM_TEXT		AlarmText;			// 轨迹所需要显示的文字信息	
	IA_ALARM_POLYLINE	AlarmPolyline;		// 轨迹所需要显示的图形

}IA_LAYOUT_ELEMENT;

// 智能分析轨迹信息
typedef struct  
{
	ULONG64			ulFrameID;					// 时间戳
	ULONG			ulFrameWidth;				// 分辨率(宽)
	ULONG			ulFrameHeight;				// 分辨率(高)
	ULONG			ulLayoutElementNum;			// 轨迹信息数
	IA_LAYOUT_ELEMENT LayoutElement[IA_ELEMENT_NUM_MAX];// 告警轨迹数组

}IA_LAYOUT_MESSAGE;

#pragma pack(pop)

//对外API接口前缀
#define IVS_IATRACK_API extern "C" __declspec(dllexport)

//#define IVS_IATRACK_API extern "C" __declspec(dllimport)
 
//IATrack句柄
typedef void * IATRACK_HANDLE;



/*****************************************************************************
 函数名称：IATrack_LibInit
 功能描述：初始化智能分析库
 输入参数：NA  
 输出参数：NA
 返 回 值：INT
 *****************************************************************************/
//IVS_IATRACK_API INT IATrack_LibInit();


/*****************************************************************************
 函数名称：IATrack_LibClean
 功能描述：清理智能分析库
 输入参数：NA  
 输出参数：NA
 返 回 值：VOID
 *****************************************************************************/
// IVS_IATRACK_API VOID IATrack_LibClean();


/*****************************************************************************
 函数名称：IATrack_GetHandle
 功能描述：获取IATrack句柄
 输入参数：
  	@handle：       IATrack句柄
 输出参数：NA
 返 回 值：IATrack_HANDLE
*****************************************************************************/
IVS_IATRACK_API IATRACK_HANDLE IATrack_GetHandle();


/*****************************************************************************
 函数名称：IATrack_InitIATrack
 功能描述：初始化智能分析轨迹信息
 输入参数：
  	@handle：       IATrack句柄
	@hMsgWnd：      窗口句柄
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_IATRACK_API LONG IATrack_Init(IATRACK_HANDLE handle, HWND hMsgWnd, ULONG ulVideoType);


/*****************************************************************************
 函数名称：IATrack_InsertIALayoutMessage
 功能描述：插入轨迹信息
 输入参数：
  	@handle：			IATrack句柄
	@LayoutMessage：		轨迹消息
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_IATRACK_API LONG IATrack_InsertIALayoutMessage(IATRACK_HANDLE handle, const IA_LAYOUT_MESSAGE *LayoutMessage);


/*****************************************************************************
 函数名称：IATrack_ShowIATrack
 功能描述：显示轨迹信息
 输入参数：
  	@handle：			IATrack句柄
	@hDC：				显示DC
	@rc：				DC大小
	@ulFrameID：			当前时间戳
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_IATRACK_API LONG IATrack_ShowIATrack(IATRACK_HANDLE handle, HDC hDC, const RECT *rc, ULONG64 ulFrameID);


/*****************************************************************************
 函数名称：IATrack_GetCurrentElementNum
 功能描述：获取当前轨迹信息数量
 输入参数：  	
	@handle：			IATrack句柄
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_IATRACK_API LONG IATrack_GetCurrentElementNum(IATRACK_HANDLE handle);


/*****************************************************************************
 函数名称：IATrack_FreeHandle
 功能描述：释放IATrack句柄
 输入参数：
	@handle：       IATrack句柄
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_IATRACK_API LONG IATrack_FreeHandle(IATRACK_HANDLE handle);



#endif // __IATrack_API_H__

