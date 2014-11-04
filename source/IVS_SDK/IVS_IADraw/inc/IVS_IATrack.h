/********************************************************************
filename 	: IVS_IATrack.h
author   	: z00227377
created 	: 2012/12/20
description : 轨迹叠加显示类
copyright 	: Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history 	: 2012/12/20 初始版本
*********************************************************************/


#ifndef  __CIATRACK_H__
#define  __CIATRACK_H__

#include <list>
#include "Lock.h"
#include "IVS_IATrackAPI.h"
#include "IVS_DrawDataType.h"

// 轨迹信息层列表
typedef std::list<IA_LAYOUT_MESSAGE*>					IA_LAYOUT_MESSAGE_LIST;
typedef IA_LAYOUT_MESSAGE_LIST::iterator			IA_LAYOUT_MESSAGE_LIST_ITER;
typedef IA_LAYOUT_MESSAGE_LIST::reverse_iterator	IA_LAYOUT_MESSAGE_LIST_REVERSE_ITER;


class CIATrack
{
public:
	CIATrack();
	~CIATrack();

	// 初始化智能分析轨迹
	LONG InitIATrack(HWND hMsgWnd, ULONG ulVideoType);

	// 插入智能分析轨迹信息
	LONG InsertIALayoutMessage(const IA_LAYOUT_MESSAGE *LayoutMessage);

	// 显示智能分析轨迹信息
	LONG ShowIATrack(HDC hDC, const RECT *rc, ULONG64 ulFrameID);

	// 获取当前轨迹信息数量
	LONG GetCurrentElementNum();

protected:

	// 为Message分配内存
	IA_LAYOUT_MESSAGE* AllocateMessageMemory();

	// 为Message释放内存
	VOID ReleaseMessageMemory(IA_LAYOUT_MESSAGE* m);

	// 坐标转换
	LONG CoordinateTransform(IA_LAYOUT_ELEMENT &LayoutElement, ULONG ulFrameWidth, ULONG ulFrameHeight);

	// 删除列表中过期消息
	VOID DeleteExpiredMessage(IA_LAYOUT_MESSAGE_LIST &DestList, ULONG &refListLength, BOOL bIsAlarmed);

	// 将按Layer拆分后的消息插入到对应的列表中
	LONG InsertMessageIntoList(const IA_LAYOUT_MESSAGE &refNewMessage, IA_LAYOUT_MESSAGE_LIST &DestList, ULONG &refListLength);

	// 显示列表中最后节点的轨迹信息
	LONG ShowIATrackMessage(HDC hDC, IA_LAYOUT_MESSAGE_LIST &DestList, BOOL bIsAlarmed);

	// 显示单个轨迹Element
	LONG ShowIATrackElement(HDC hDC, IA_LAYOUT_ELEMENT &LayoutElememt);

	// 显示轨迹文字信息
	LONG ShowIATrackText(HDC hDC, IA_ALARM_TEXT &AlarmText, COLORREF ulColor);

	// 显示轨迹图形信息
	LONG ShowIATrackPolyline(HDC hDC, IA_ALARM_POLYLINE &AlarmPolyline, COLORREF ulColor);
	
    // 计算箭头坐标
    LONG CalcuArrowPoints(const IA_ALARM_POLYLINE& PolyLineInfo, IA_ARROW& ArrowPointSet);

    // 绘制箭头
    LONG ShowArrowShape(HDC hDC, const IA_ARROW &iaArrow, ULONG ulArrowType);

	// 创建字体
	LONG CreatFont(/*ULONG ulFontSize*/);

private:

	BOOL					m_bInitTag;					// 标记是否已经初始化

	ULONG64					m_ulCurrentFrameID;			// 当前时间戳
	ULONG					m_IAROIListLength;			// ROI轨迹消息列表长度
	ULONG					m_IAAlarmedListLength;		// Alarmed轨迹消息列表长度
	IA_LAYOUT_MESSAGE_LIST	m_IAROIList;				// ROI轨迹消息列表
	IA_LAYOUT_MESSAGE_LIST	m_IAAlarmedList;			// Alarmed轨迹消息列表

	HWND					m_hMsgWnd;					// 窗口句柄
	ULONG					m_ulWndWidth;				// 窗口宽度
	ULONG					m_ulWndHeight;				// 窗口高度
	ULONG					m_ulDCWidth;				// 显示图像宽度
	ULONG					m_ulDCHeight;				// 显示图像高度
	LONG					m_lLineWidthAdjust;			// 线宽系数，用于分辨率调节
	ULONG					m_ulLineWidth;				// 图形线宽
	ULONG					m_ulVideoType;				// 视频类型（实况、录像）
	LOGFONT					m_lf;						// 创建字体所需结构体
	HFONT					m_hFont;					// 字体句柄
	ULONG					m_ulCurrentFontSize;		// 当前字体大小
	CIVSMutex				m_Mutex;					// 轨迹消息列表锁

};







#endif //__CIATRACK_H__