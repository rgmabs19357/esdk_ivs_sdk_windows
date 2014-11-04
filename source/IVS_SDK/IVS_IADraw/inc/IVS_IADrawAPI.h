/********************************************************************
filename 	: IVS_IADrawAPI.h
author   	: z00227377
created 	: 2012/12/03
description : 智能分析画图功能，实现图形的增、删、改、获取、显示
copyright 	: Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history 	: 2012/12/03 初始版本
*********************************************************************/


#ifndef __IADRAW_API_H__
#define __IADRAW_API_H__

#include "IVS_DrawDataType.h"


//对外API接口前缀
#define IVS_DRAW_API extern "C" __declspec(dllexport)

//IADraw句柄
typedef void * DRAW_HANDLE;



/*****************************************************************************
 函数名称：Draw_LibInit
 功能描述：初始化智能分析库
 输入参数：NA  
 输出参数：NA
 返 回 值：INT
 *****************************************************************************/
IVS_DRAW_API INT Draw_LibInit(const char* pLogPath = NULL);


/*****************************************************************************
 函数名称：Draw_LibClean
 功能描述：清理智能分析库
 输入参数：NA  
 输出参数：NA
 返 回 值：VOID
 *****************************************************************************/
IVS_DRAW_API VOID Draw_LibClean();


/*****************************************************************************
 函数名称：Draw_GetHandle
 功能描述：获取IADraw句柄
 输入参数：
  	@handle：       IADraw句柄
 输出参数：NA
 返 回 值：IADraw_HANDLE
*****************************************************************************/
IVS_DRAW_API DRAW_HANDLE Draw_GetHandle();


/*****************************************************************************
 函数名称：Draw_Init
 功能描述：初始化智能分析画图
 输入参数：
  	@handle：       IADraw句柄
	@hMsgWnd：      绘图窗口句柄
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_Init(DRAW_HANDLE handle, HWND hMsgWnd);


/*****************************************************************************
 函数名称：Draw_SetDisplayScaleMode
 功能描述：设置是否满窗口显示
 输入参数：
  	@handle：			IADraw句柄
	@bScaleModeFull：	设置是否满窗口显示
 输出参数：NA 
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_SetDisplayScaleMode(DRAW_HANDLE handle, BOOL bScaleModeFull);


/*****************************************************************************
 函数名称：Draw_GetGraphicsByGroup
 功能描述：按组获取智能分析图形
 输入参数：
  	@handle：			IADraw句柄
	@ulGraphicGroup：	图形组标识
	@pulNum：			表明最大可以获取多少个图形
 输出参数：	 
	@pGraphics：			返回指向输出图形数组的指针
	@pulNum：			获取到的图形个数
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_GetGraphicsByGroup(DRAW_HANDLE handle, 
										  ULONG ulGraphicGroup, 
										  IA_GRAPHICS *pGraphics, 
									 	  ULONG *pulNum);

/*****************************************************************************
 函数名称：Draw_GetIAGraphicsSelected
 功能描述：获取智能分析选中的图形
 输入参数：
  	@handle：			IADraw句柄
 输出参数：	 
	@pGraphic：			返回指向输出图形的指针
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_GetGraphicSelected(DRAW_HANDLE handle, IA_GRAPHICS *pGraphic);

/*****************************************************************************
 函数名称：Draw_GetGraphicsAll
 功能描述：获取智能分析图形
 输入参数：
  	@handle：			IADraw句柄
 输出参数：	 
	@pGraphics：			返回指向输出图形数组的指针
	@pulNum：			获取到的图形个数
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_GetGraphicsAll(DRAW_HANDLE handle, 
									  IA_GRAPHICS *pGraphics, 
								      ULONG *pulNum);


/*****************************************************************************
 函数名称：Draw_SetRelationShip
 功能描述：设置智能分析图形组之间的关系
 输入参数：
  	@handle：				IADraw句柄
	@ulGraphicGroupA：		图形组A标识
	@ulGraphicGroupB：		图形组B标识
	@ulRelationShipType：	图形组A相对于组B的关系（In、Out、NotCross）
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_SetRelationShip(DRAW_HANDLE handle, 
									   ULONG ulGraphicGroupA, 
									   ULONG ulGraphicGroupB, 
									   RELATION_SHIP_TYPE ulRelationShipType);


/*****************************************************************************
 函数名称：Draw_SetGraphics
 功能描述：设置智能分析图形
 输入参数：
  	@handle：       IADraw句柄
	@pGraphics：    指向输入图形数组的指针
	@ulNum：        输入图形个数
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_SetGraphics(DRAW_HANDLE handle, const IA_GRAPHICS *pGraphics, ULONG ulNum, BOOL bPersent);


/*****************************************************************************
 函数名称：Draw_StraightLineStart
 功能描述：智能分析开始画直线
 输入参数：
  	@handle：					IADraw句柄
	@ulNum：						该图形组最多能画图形的个数
	@pIAGraphics：				初始图形的相关属性及初始点
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_StraightLineStart(DRAW_HANDLE handle, 
										 ULONG ulNum, 
										 IA_GRAPHICS *pIAGraphics);

/*****************************************************************************
 函数名称：Draw_StraightLineUpDate
 功能描述：智能分析随鼠标移动更新直线对应坐标
 输入参数：
  	@handle：			IADraw句柄
	@point：				直线终点坐标
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_StraightLineUpDate(DRAW_HANDLE handle,POINT point);

/*****************************************************************************
 函数名称：Draw_StraightLineFinish
 功能描述：智能分析结束画直线并设定终点
 输入参数：
  	@handle：			IADraw句柄
	@point：				直线终点坐标
	@ulGraphicGroup：	图形组标识
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_StraightLineFinish(DRAW_HANDLE handle,POINT point, ULONG /*ulGraphicGroup*/);

/*****************************************************************************
 函数名称：Draw_RectangleStart
 功能描述：智能分析开始画矩形
 输入参数：
	 @handle：					IADraw句柄
	 @ulNum：					该图形组最多能画图形的个数
	 @pIAGraphics：				初始图形的相关属性及初始点
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_RectangleStart(DRAW_HANDLE handle, 
									  ULONG ulNum,
									  IA_GRAPHICS *pIAGraphics);

/*****************************************************************************
 函数名称：Draw_RectangleUpDate
 功能描述：智能分析随鼠标移动更新矩形对应坐标
 输入参数：
  	@handle：			IADraw句柄
	@point：				矩形对顶点坐标
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_RectangleUpDate(DRAW_HANDLE handle, POINT point);

/*****************************************************************************
 函数名称：Draw_RectangleFinish
 功能描述：智能分析结束画矩形并设定最终各顶点
 输入参数：
  	@handle：			IADraw句柄
	@point：				矩形对顶点坐标
	@ulGraphicGroup：	图形组标识
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_RectangleFinish(DRAW_HANDLE handle, POINT point, ULONG /*ulGraphicGroup*/);

/*****************************************************************************
 函数名称：Draw_InsertCurrentToGraphicList
 功能描述：直接插入当前图形
 输入参数：
  	@handle：			IADraw句柄
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_InsertCurrentToGraphicList(DRAW_HANDLE handle);

/*****************************************************************************
 函数名称：Draw_PolygonStart
 功能描述：智能分析开始画多边形
 输入参数：
	 @handle：					IADraw句柄
	 @ulNum：					该图形组最多能画图形的个数
	 @pIAGraphics：				初始图形的相关属性及初始点
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_PolygonStart(DRAW_HANDLE handle, 
									ULONG ulNum, 
									IA_GRAPHICS *pIAGraphics);

/*****************************************************************************
 函数名称：Draw_PolygonUpDate
 功能描述：智能分析随鼠标移动更新多边形最新一条边的终点坐标
 输入参数：
  	@handle：			IADraw句柄
	@point：				多边形最新一条边的终点坐标
	@ulGraphicGroup：	图形组标识
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_PolygonUpDate(DRAW_HANDLE handle, POINT point, ULONG ulGraphicGroup);

/*****************************************************************************
 函数名称：Draw_PolygonSetPoint
 功能描述：智能分析点击确定多边形中间节点位置坐标
 输入参数：
  	@handle：			IADraw句柄
	@point：				多边形中间节点位置坐标
	@ulGraphicGroup：	图形组标识
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_PolygonSetPoint(DRAW_HANDLE handle, POINT point, ULONG ulGraphicGroup);

/*****************************************************************************
 函数名称：Draw_PolygonFinish
 功能描述：智能分析点击确定多边形最后一边并且闭合图形
 输入参数：
  	@handle：			IADraw句柄
	@point：				当前鼠标在图像中坐标（该点不计入多边形内）
	@ulGraphicGroup：	图形组标识
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_PolygonFinish(DRAW_HANDLE handle, POINT point, ULONG ulGraphicGroup);

/*****************************************************************************
 函数名称：Draw_ULineStart
 功能描述：智能分析开始画U型线
 输入参数：
	@handle：					IADraw句柄
	@ulNum：						该图形组最多能画图形的个数
	@pIAGraphics：				初始图形的相关属性及初始点
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_ULineStart(DRAW_HANDLE handle, 
								  ULONG ulNum,
								  IA_GRAPHICS *pIAGraphics);

/*****************************************************************************
 函数名称：Draw_ULineUpDate
 功能描述：智能分析随鼠标移动更新U型线最新一条边的终点坐标
 输入参数：
  	@handle：			IADraw句柄
	@point：				U型线最新一条边的终点坐标
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_ULineUpDate(DRAW_HANDLE handle, POINT point);

/*****************************************************************************
 函数名称：Draw_ULineSetPoint
 功能描述：智能分析点击确定U型框中间节点位置坐标
 输入参数：
  	@handle：			IADraw句柄
	@point：				U型线中间节点位置坐标
	@ulGraphicGroup：	图形组标识
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_ULineSetPoint(DRAW_HANDLE handle, POINT point, ULONG /*ulGraphicGroup*/);

/*****************************************************************************
 函数名称：Draw_ULineFinish
 功能描述：智能分析点击确定U型线最后一边并且结束画U型线
		（若已确定三条边，直接结束画图；若第三条边未确定，则以当前点为U型线第三边终点）
 输入参数：
  	@handle：			IADraw句柄
	@point：				当前鼠标在图像中坐标
	@ulGraphicGroup：	图形组标识
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_ULineFinish(DRAW_HANDLE handle, POINT point, ULONG /*ulGraphicGroup*/);


/*****************************************************************************
 函数名称：Draw_StopDrawGraphics
 功能描述：智能分析结束某图形组的画图（若存在正在画的该组图形，直接丢弃）
 输入参数：
  	@handle：			IADraw句柄
	@ulGraphicGroup：	图形组标识
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_StopDrawGraphics(DRAW_HANDLE handle, ULONG ulGraphicGroup);


/*****************************************************************************
 函数名称：Draw_ShowGraphics
 功能描述：显示所有智能分析图形
 输入参数：
  	@handle：       IADraw句柄
	@hDC：			画图句柄
	@rc：			画图窗口的大小
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_ShowGraphics(DRAW_HANDLE handle, HDC hDC, const RECT *rc);

/*****************************************************************************
 函数名称：Draw_DeleteGraphicsSelected
 功能描述：删除所有选中图形
 输入参数：
  	@handle：			IADraw句柄
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_DeleteGraphicsSelected(DRAW_HANDLE handle);


/*****************************************************************************
 函数名称：Draw_DeleteGraphicsByPoint
 功能描述：根据鼠标点坐标删除智能分析单个图形（若坐标在指定图形组的单个图形内部，删除该图形；若在多个图形重叠区域，则不删除）
 输入参数：
  	@handle：			IADraw句柄
	@ulGraphicGroup：	图形组标识
	@point：				当前鼠标在图像中的坐标点
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_DeleteGraphicsByPoint(DRAW_HANDLE handle, ULONG ulGraphicGroup, POINT point);

/*****************************************************************************
 函数名称：Draw_DeleteGraphicsByGroup
 功能描述：清除指定图形组的所有图形
 输入参数：
 	@handle：			IADraw句柄
	@ulGraphicGroup：	图形组标识
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_DeleteGraphicsByGroup(DRAW_HANDLE handle, ULONG ulGraphicGroup);

/*****************************************************************************
 函数名称：Draw_DeleteGraphics
 功能描述：清除智能分析所有图形
 输入参数：
 	@handle：       IADraw句柄
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_DeleteGraphics(DRAW_HANDLE handle);


/*****************************************************************************
 函数名称：Draw_SelectGraphicByPoint
 功能描述：根据鼠标点坐标选中智能分析单个图形（选择特定组中与鼠标坐标最近的单个图形）
 输入参数：
  	@handle：			IADraw句柄
	@point：				当前鼠标在图像中的坐标点
输出参数：	
	@ulGraphicGroup：	图形组标识
	@bAllowDel：			是否允许删除标记
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_SelectGraphicByPoint(DRAW_HANDLE handle, POINT point, ULONG *ulGraphicGroup, BOOL *bAllowDel);


/*****************************************************************************
 函数名称：Draw_GetCursorType
 功能描述：获取鼠标类型
 输入参数：
  	@handle：			IADraw句柄
	@point：				当前鼠标在图像中的坐标点
输出参数：NA	
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_GetCursorType(DRAW_HANDLE handle, POINT Point);

/*****************************************************************************
 函数名称：Draw_SelectGraphicByGroup
 功能描述：选中指定图形组的所有图形
 输入参数：
 	@handle：			IADraw句柄
	@ulGraphicGroup：	图形组标识
 输出参数：
	@bAllowDel：			是否允许删除标记
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_SelectGraphicByGroup(DRAW_HANDLE handle, ULONG ulGraphicGroup, BOOL *bAllowDel);

/*****************************************************************************
 函数名称：Draw_StartModifyGraphics
 功能描述：智能分析开始修改图形
 输入参数：
 	@handle：			IADraw句柄
	@point：				鼠标起始点击位置坐标
	@MouseEvent：		鼠标点击类型（LEFT_BUTTON_DOWN 、RIGHT_BUTTON_DOWN）
	@ulGraphicGroup：	图形组标识
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_StartModifyGraphics(DRAW_HANDLE handle, POINT point, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup);

/*****************************************************************************
 函数名称：Draw_ModifyShapeUpDate
 功能描述：智能分析随鼠标移动修改图形相应的坐标
 输入参数：
 	@handle：			IADraw句柄
	@ulGraphicGroup：	图形组标识
	@point：				鼠标当前位置
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_ModifyShapeUpDate(DRAW_HANDLE handle, ULONG ulGraphicGroup, POINT point);

/*****************************************************************************
 函数名称：Draw_ModifyFinish
 功能描述：智能分析结束修改图形
 输入参数：
 	@handle：			IADraw句柄
	@ulGraphicGroup：	图形组标识
	@point：				鼠标当前位置
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_ModifyFinish(DRAW_HANDLE handle, ULONG ulGraphicGroup, POINT point);

/*****************************************************************************
 函数名称：Draw_SelectCancel
 功能描述：取消选中智能分析图形
 输入参数：
 	@handle：       IADraw句柄
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_SelectCancel(DRAW_HANDLE handle);


/*****************************************************************************
 函数名称：Draw_IsFinishDrawGraphicGroup
 功能描述：判断是否达到该图形组最大画图数
 输入参数：
 	@handle：			IADraw句柄
	@ulGraphicGroup:	图形组标识
	@ulMaxDrawNum：		图形组最大画图个数
 输出参数：NA
 返 回 值：BOOL
*****************************************************************************/
IVS_DRAW_API BOOL Draw_IsFinishDrawGraphicGroup(DRAW_HANDLE handle, ULONG ulGraphicGroup, ULONG ulMaxDrawNum);


/*****************************************************************************
 函数名称：Draw_FreeHandle
 功能描述：释放IADraw句柄
 输入参数：
	@handle：       IADraw句柄
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_FreeHandle(DRAW_HANDLE handle);

/*****************************************************************************
 函数名称：Draw_RectangleStartByGraphicPoint
 功能描述：智能分析开始画矩形
 输入参数：
	 @handle：					IADraw句柄
	 @ulNum：					该图形组最多能画图形的个数
	 @pIAGraphics：				初始图形的相关属性及初始点
 输出参数：NA
 返 回 值：LONG
*****************************************************************************/
IVS_DRAW_API LONG Draw_RectangleStartByGraphicPoint(DRAW_HANDLE handle, ULONG ulNum, const IA_GRAPHICS *pIAGraphics);

/*****************************************************************************
 函数名称：Draw_RectangleFinishByGraphicPoint
 功能描述：智能分析结束画矩形并设定最终各顶点
 输入参数：
 @handle：			IADraw句柄
 @point：				矩形对顶点坐标
 @ulGraphicGroup：	图形组标识
 输出参数：NA
 返 回 值：LONG
 *****************************************************************************/
IVS_DRAW_API LONG Draw_RectangleFinishByGraphicPoint(DRAW_HANDLE handle, POINT point/*, ULONG ulGraphicGroup*/);

/*****************************************************************************
 函数名称：Draw_WindowPointToGraphicPoint
 功能描述：转换WINDOW坐标点到图像坐标点
 输入参数：
 @handle：			IADraw句柄
 @pPoint：			需要转换的点
 输出参数：NA
 返 回 值：LONG
 *****************************************************************************/
IVS_DRAW_API LONG Draw_WindowPointToGraphicPoint(DRAW_HANDLE handle, POINT* pPoint);

/*****************************************************************************
 函数名称：Draw_StartModifyGraphicsByGraphicPoint
 功能描述：智能分析开始画矩形
 输入参数：
 @handle：			IADraw句柄
 @pPoint：			需要转换的点
 @MouseEvent：
 @ulGraphicGroup：
 输出参数：NA
 返 回 值：LONG
 *****************************************************************************/
IVS_DRAW_API LONG Draw_StartModifyGraphicsByGraphicPoint(DRAW_HANDLE handle, POINT point, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup);

/*****************************************************************************
 函数名称：Draw_ModifyFinishByGraphicPoint
 功能描述：智能分析结束画矩形并设定最终各顶点
 输入参数：
 @handle：			IADraw句柄
 @ulGraphicGroup：  组ID
 @pPoint：			需要转换的点
 输出参数：NA
 返 回 值：LONG
 *****************************************************************************/
IVS_DRAW_API LONG Draw_ModifyFinishByGraphicPoint(DRAW_HANDLE handle, ULONG ulGraphicGroup, POINT point);

/*****************************************************************************
 函数名称：Draw_SelectGraphicByGraphicPoint
 功能描述：智能分析选中
 输入参数：
 @handle：			IADraw句柄
 @pPoint：			需要转换的点
 @ulGraphicGroup：
 @bAllowDel：
 输出参数：NA
 返 回 值：LONG
 *****************************************************************************/
IVS_DRAW_API LONG Draw_SelectGraphicByGraphicPoint(DRAW_HANDLE handle, POINT point, ULONG *ulGraphicGroup, BOOL *bAllowDel);
#endif // __IADRAW_API_H__

