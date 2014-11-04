/********************************************************************
filename 	: IVS_IADraw.h
author   	: z00227377
created 	: 2012/12/03
description : 画图显示类
copyright 	: Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history 	: 2012/12/03 初始版本
*********************************************************************/

#ifndef  __CIADRAWER_H__
#define  __CIADRAWER_H__

#include <list>
#include "Lock.h"
#include "IVS_DrawDataType.h"

// 单个图形结构体
typedef struct
{
	ULONG		ulGraphicsID;	// 内部图形ID
	BOOL		bFinish;		// 是否完成绘图标识
	BOOL		bEditing;		// 是否在选中状态标识
	BOOL		bPersent;		// 是否百分比坐标
	IA_GRAPHICS	iaGraphic;		// 对外图形结构体

}IA_INNER_GRAPHICS;

// 智能分析图像列表
typedef std::list<IA_INNER_GRAPHICS*>    IA_GRAPHICS_LIST;
typedef IA_GRAPHICS_LIST::iterator  IA_GRAPHICS_LIST_ITER;

// 单个图形组位置关系结构体
typedef struct
{
	ULONG				ulCurrentGraphicGroup;	// 当前的图形组标识
	ULONG				ulRelativeGraphicGroup;	// 相关的图形组标识
	RELATION_SHIP_TYPE	ulRelationShipType;		// 当前图形组与相关图形组之间的位置关系类型

}IA_RELATIONS;

// 图形组位置关系列表
typedef std::list<IA_RELATIONS*>			 IA_RELATIONS_LIST;
typedef IA_RELATIONS_LIST::iterator  IA_RELATIONS_LIST_ITER;


class CIADrawer
{
public:
	CIADrawer();
	~ CIADrawer();
	/*****************************************************************************
	 函数名称：initIADrawer
	 功能描述：初始化智能分析画图
	 输入参数：
		@hMsgWnd：      绘图窗口句柄
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG InitIADrawer(HWND hMsgWnd);

	/*****************************************************************************
	 函数名称：SetDisplayScaleMode
	 功能描述：设置是否满窗口显示
	 输入参数：
		@bScaleModeFull：			是否满窗口显示
	 输出参数：NA
	 返 回 值：VOID
	 *****************************************************************************/	
	VOID SetDisplayScaleMode(BOOL bScaleModeFull);

	/*****************************************************************************
	 函数名称：WindowPointToGraphicPoint
	 功能描述：转换窗口坐标为图像坐标
	 输入/输出参数：
		@point：			原窗口坐标/转换成为的图像中坐标
	 返 回 值：LONG
	 *****************************************************************************/	
	LONG WindowPointToGraphicPoint(POINT *point);

	/*****************************************************************************
	 函数名称：SetIAGraphics
	 功能描述：设置智能分析图形
	 输入参数：
		@pGraphics：    指向输入图形数组的指针
		@ulNum：        输入图形个数
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG SetIAGraphics(const IA_GRAPHICS *pGraphics, ULONG ulNum, BOOL bPersent);

	/*****************************************************************************
	 函数名称：SetRelationShip
	 功能描述：设置智能分析图形组之间的关系
	 输入参数：
		@ulGraphicGroupA：		图形组A标识
		@ulGraphicGroupB：		图形组B标识
		@ulRelationShipType：	图形组A相对于组B的关系
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG SetRelationShip(ULONG ulGraphicGroupA, ULONG ulGraphicGroupB, RELATION_SHIP_TYPE ulRelationShipType);	

	/*****************************************************************************
	 函数名称：GetIAGraphics
	 功能描述：获取智能分析图形
	 输入参数：
		@ulGraphicGroup：	图形组标识
		@pulNum：			表明最大可以获取多少个图形
	 输出参数：	 
		@pGraphics：			返回指向输出图形数组的指针
		@pulNum：			获取到的图形个数
	 返 回 值：LONG
	 *****************************************************************************/
	LONG GetIAGraphics(ULONG ulGraphicGroup, IA_GRAPHICS *pGraphics, ULONG *pulNum);
	
	/*****************************************************************************
	 函数名称：GetIAGraphicsSelected
	 功能描述：获取选中的智能分析图形
	 输入参数：NA
	 输出参数：	 
		@pGraphics：			返回指向输出图形的指针
	 返 回 值：LONG
	 *****************************************************************************/
	LONG GetIAGraphicSelected(IA_GRAPHICS *pGraphic);

	/*****************************************************************************
	 函数名称：SetIAStartStatus
	 功能描述：设置开始画图的起始状态
	 输入参数：
		@ulNum：						该图形组最多能画图形的个数
		@pIAGraphics：				初始图形的相关属性及初始点
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG SetIAStartStatus(ULONG ulNum, const IA_GRAPHICS *pIAGraphics);

	/*****************************************************************************
	 函数名称：DrawIAStraightLine
	 功能描述：智能分析画直线
	 输入参数：
		@point：				直线终点坐标
		@bFinish:			是否结束画图
		@bIsDrawing:		是否是画图状态（非修改状态）
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG DrawIAStraightLine(POINT point, BOOL bFinish, BOOL bIsDrawing);

	/*****************************************************************************
	 函数名称：DrawIARectangle
	 功能描述：智能分析画矩形
	 输入参数：
		@point：				直线终点坐标
		@bFinish:			是否结束画图
		@bIsDrawing：		是否是画图状态（非修改状态）
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG DrawIARectangle(POINT point, BOOL bFinish, BOOL bIsDrawing);

	/*****************************************************************************
	 函数名称：DrawIAPolygon
	 功能描述：智能分析画多边形
	 输入参数：
		@point：				直线终点坐标
		@bNextPoint：		是否确定一个顶点
		@bFinish:			是否结束画图
		@ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG DrawIAPolygon(POINT point, BOOL bNextPoint, BOOL bFinish, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：DrawIAULine
	 功能描述：智能分析画U型线
	 输入参数：
		@point：				直线终点坐标
		@bNextPoint：		是否确定一个顶点
		@bFinish:			是否结束画图
		@bIsDrawing：		是否是画图状态（非修改状态）
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG DrawIAULine(POINT point, BOOL bNextPoint, BOOL bFinish, BOOL bIsDrawing);

	/*****************************************************************************
	 函数名称：StopDrawGraphics
	 功能描述：智能分析结束某图形组的画图（若存在正在画的该组图形，直接丢弃）
	 输入参数：
		@ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG StopDrawGraphics(ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：ShowIAGraphics
	 功能描述：显示智能分析图形
	 输入参数：
		@hDC：			画图句柄
		@rc：			图像的大小信息
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG ShowIAGraphics(HDC hDC, const RECT *rc);


	/*****************************************************************************
	 函数名称：DeleteIAGraphicsSelected
	 功能描述：删除所有选中图形
	 输入参数：NA
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG DeleteIAGraphicsSelected();

	/*****************************************************************************
	 函数名称：DeleteIAGraphics
	 功能描述：根据鼠标点坐标删除智能分析单个图形
			（若坐标在指定图形组的单个图形内部，删除该图形；若在多个图形重叠区域，则不删除）
	 输入参数：
		@ulGraphicGroup：	图形组标识
		@point：				当前鼠标在图像中的坐标点
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG DeleteIAGraphicsByPoint(ULONG ulGraphicGroup, POINT point);
	
	/*****************************************************************************
	 函数名称：DeleteGraphicsByGroup
	 功能描述：清除指定图形组的所有图形
	 输入参数：
		@ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG DeleteGraphicsByGroup(ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：DeleteIAGraphicsAll
	 功能描述：清除智能分析所有图形
	 输入参数：NA
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG DeleteIAGraphicsAll();

	/*****************************************************************************
	 函数名称：SelectGraphicByPoint
	 功能描述：根据鼠标点坐标选中智能分析单个图形（选择特定组中与鼠标坐标最近的单个图形）
	 输入参数：
		@Point：				当前鼠标在图像中的坐标点
	 输出参数：
		@ulGraphicGroup：	图形组标识
		@bAllowDel：			是否允许删除标记
	 返 回 值：LONG
	*****************************************************************************/
	LONG SelectGraphicByPoint(POINT Point, ULONG &ulGraphicGroup, BOOL &bAllowDel);

	/*****************************************************************************
	 函数名称：GetCursorType
	 功能描述：获取鼠标类型
	 输入参数：
		@Point：				当前鼠标在图像中的坐标点
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG GetCursorType(POINT Point);

	/*****************************************************************************
	 函数名称：SelectGraphicByGroup
	 功能描述：选中指定图形组的所有图形
	 输入参数：
		@ulGraphicGroup：	图形组标识
	 输出参数：
		@bAllowDel：			是否允许删除标记
	 返 回 值：LONG
	*****************************************************************************/
	LONG SelectGraphicByGroup(ULONG ulGraphicGroup, BOOL &bAllowDel);

	/*****************************************************************************
	 函数名称：StartModifyIAGraphics
	 功能描述：智能分析开始修改图形
	 输入参数：
		@point：				鼠标起始点击位置坐标
		@MouseEvent：		鼠标点击类型（LEFT_BUTTON_DOWN 、RIGHT_BUTTON_DOWN）
		@ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG StartModifyIAGraphics(POINT point, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：ModifyIAShapeUpDate
	 功能描述：智能分析随鼠标移动修改图形相应的坐标
	 输入参数：
		@point：				鼠标当前位置
		@bFinish：			是否修改结束
		@ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG ModifyIAShapeUpDate(POINT point, BOOL bFinish, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：ModifyFinishedAndInsert
	 功能描述：智能分析修改图形结束
	 输入参数：
		@lRet：				修改图形中返回码
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG ModifyFinishedAndInsert(LONG lRet);

	/*****************************************************************************
	 函数名称：ModifyCancel
	 功能描述：取消选中智能分析图形
	 输入参数：NA
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG SelectCancel();

	/*****************************************************************************
	 函数名称：InsertCurrentToGraphicList
	 功能描述：将当前的图形插入到画图列表中
	 输入参数：NA
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	VOID InsertCurrentToGraphicList();

	/*****************************************************************************
	 函数名称：IsFinishDrawGraphicGroup
	 功能描述：判断是否达到该图形组最大画图数
	 输入参数：
		@ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：BOOL
	*****************************************************************************/
	BOOL IsFinishDrawGraphicGroup(ULONG ulGraphicGroup, ULONG ulMaxDrawNum);

private:
	
	LONG GetCursorTypeInShape(POINT Point, const IA_GRAPHICS &IAGraphic);
	LONG GetCursorTypePoint(POINT /*Point*/, const IA_GRAPHICS &IAGraphic, ULONG ulIndex);
	LONG GetCursorTypePointInner(POINT Point, POINT PointPre, POINT PointNext);

	VOID GetArrowPoints(IA_GRAPHICS &iaGraphic);
	VOID ShowArrowShape(HDC hDC, const IA_ARROW &iaArrow, ULONG ulArrowType);
	/*****************************************************************************
	 函数名称：GetOppositeType
	 功能描述：获取相反的图形组关系
	 输入参数：
		 @ulRelationShipType：	原图形组关系
	 输出参数：NA
	 返 回 值：RelationShipType
	 *****************************************************************************/
	RELATION_SHIP_TYPE GetOppositeType(RELATION_SHIP_TYPE ulRelationShipType);

	/*****************************************************************************
	 函数名称：InsertIntoRelationShipList
	 功能描述：将位置关系插入到位置关系列表中
	 输入参数：
		@ulGraphicGroupA：		图形组A标识
		@ulGraphicGroupB：		图形组B标识
		@ulRelationShipType：	图形组A相对于组B的关系
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG InsertIntoRelationShipList(ULONG ulGraphicGroupA, ULONG ulGraphicGroupB, RELATION_SHIP_TYPE ulRelationShipType);

	/*****************************************************************************
	 函数名称：FinishAndInsertIntoGraphicList
	 功能描述：若画图结束，插入到画图列表中
	 输入参数：
	 	@bIsDrawing：		是否是画图状态（非修改状态）
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG FinishAndInsertIntoGraphicList(BOOL bIsDrawing);

	/*****************************************************************************
	 函数名称：InsertCurrentToList
	 功能描述：将当前图形插入到画图列表中
	 输入参数：NA
	 输出参数：NA
	 返 回 值：LONG
	*****************************************************************************/
	LONG InsertCurrentToList();

	/*****************************************************************************
	 函数名称：IsSetGraphicsInvalid
	 功能描述：判断设置的智能分析图形是否合法
	 输入参数：
		@pGraphics：    指向输入图形数组的指针
		@ulNum：        输入图形个数
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsSetGraphicsInvalid(const IA_GRAPHICS *pGraphics, ULONG ulNum);

	/*****************************************************************************
	 函数名称：AdjustRectPoint
	 功能描述：根据bKeepRectShape值自动调整输入点坐标
	 输入参数：
		@point：    输入鼠标坐标值
	 输出参数：
		@point：    调整后的坐标值
	 返 回 值：LONG
	 *****************************************************************************/
	LONG AdjustRectPoint(POINT &point);

	/*****************************************************************************
	 函数名称：DrawIAPolygonShape
	 功能描述：画智能分析多边形
	 输入参数：
		@point：				新点坐标
		@bNextPoint：		是否还有下一个点
		@bFinish：			画图是否结束
		@ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG DrawIAPolygonShape(POINT point, BOOL bNextPoint, BOOL bFinish, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：IsDrawIAPolygonLegal
	 功能描述：判断是否画多边形合法
	 输入参数：
		@newPoint：			    新点坐标
		@ulCurrentPointsNum：	当前点数
		@ulGraphicGroup：		图形组标识
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG IsDrawIAPolygonLegal(POINT newPoint, ULONG ulCurrentPointsNum, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：IsPolygonFinishedLegal
	 功能描述：判断多边形画完后整体是否合法
	 输入参数：
		@ulGraphicGroup：	图形组标识
		@bReachMaxPoint:	是否达到最大点数
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG IsPolygonFinishedLegal(ULONG ulGraphicGroup, BOOL bReachMaxPoint);

	/*****************************************************************************
	 函数名称：IsIAPolygonSelfLegal
	 功能描述：判断多边形自身是否合法
	 输入参数：
		@pPoint：		多边形顶点数组
		@ulPointNum：	多边形顶点数
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsIAPolygonSelfLegal(const POINT *pPoint, ULONG ulPointsNum);

	/*****************************************************************************
	 函数名称：DrawIAULineShape
	 功能描述：画智能分析U型线
	 输入参数：
		@point：				终点坐标
		@bNextPoint：		是否还有下一个点
		@bIsDrawing：		是否是画图状态
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG DrawIAULineShape(POINT point, BOOL bNextPoint, BOOL bIsDrawing);

	/*****************************************************************************
	 函数名称：DrawIAUFirstLine
	 功能描述：画智能分析U型线第一条线
	 输入参数：
		@point：				终点坐标
		@bNextPoint：		是否还有下一个点
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG DrawIAUFirstLine(POINT point, BOOL bNextPoint);

	/*****************************************************************************
	 函数名称：DrawIAULineShape
	 功能描述：画智能分析U型线第二条线
	 输入参数：
		@point：				终点坐标
		@bNextPoint：		是否还有下一个点
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG DrawIAUSecondLine(POINT point, BOOL bNextPoint);
	
	/*****************************************************************************
	 函数名称：DrawIAUThirdLine
	 功能描述：画智能分析U型线第三条线
	 输入参数：
		@point：				终点坐标
		@bNextPoint：		是否还有下一个点
		@bIsDrawing：		是否是画图状态
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG DrawIAUThirdLine(POINT point,BOOL bNextPoint, BOOL bIsDrawing);

	/*****************************************************************************
	 函数名称：UpdateWnd
	 功能描述：更新画图窗口
	 输入参数：
		@hDC：			画图句柄
		@rc：			图像的大小信息
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG UpdateWnd(HDC hDC, const RECT *rc); 

	void GetOffset();

	/*****************************************************************************
	 函数名称：IsShowIAShapeInvalid
	 功能描述：判断需要显示的单个图形是否合法
	 输入参数：
		@pGraphics：		图形数组
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsShowIAShapeInvalid(const IA_INNER_GRAPHICS &pIAGraphic);


	/*****************************************************************************
	 函数名称：ShowIAShape
	 功能描述：显示智能分析单个图形
	 输入参数：
		@hDC：			绘图句柄
		@pGraphics：		图形数组
	 输出参数：NA
	 返 回 值：VOID
	 *****************************************************************************/
	VOID ShowIAShape(HDC hDC, IA_INNER_GRAPHICS &pGraphics);

	/*****************************************************************************
	 函数名称：ShowPolyline
	 功能描述：显示智能分析多边形
	 输入参数：
		@hDC：			绘图句柄
		@points：		多边形各顶点坐标数组
		@numberPoints：	多边形顶点个数
		@color：			当前颜色
	 输出参数：NA
	 返 回 值：VOID
	 *****************************************************************************/
	VOID ShowPolyline(HDC hDC, const POINT *points, ULONG numberPoints, COLORREF /*color*/);

	/*****************************************************************************
	 函数名称：ShowGraphicChoosen
	 功能描述：显示选中的智能分析图形
	 输入参数：
		@hDC：			绘图句柄
		@pIAGraphics：	选中的图形
	 输出参数：NA
	 返 回 值：VOID
	 *****************************************************************************/
	VOID ShowGraphicChoosen(HDC hDC, const IA_INNER_GRAPHICS &pIAGraphics);

	/*****************************************************************************
	 函数名称：ShowGraphicName
	 功能描述：显示图形名称
	 输入参数：
		@hDC：			绘图句柄
		@pIAGraphics：	选中的图形
	 输出参数：NA
	 返 回 值：VOID
	 *****************************************************************************/
	VOID ShowGraphicName(HDC hDC, const IA_INNER_GRAPHICS &pIAGraphics);

	/*****************************************************************************
	 函数名称：ShowAlphaBlendArea
	 功能描述：显示图形内半透明填充区域
	 输入参数：
		@hDC：			绘图句柄
		@pIAGraphics：	选中的图形
	 输出参数：NA
	 返 回 值：VOID
	 *****************************************************************************/
	VOID ShowAlphaBlendArea(HDC hDC, const IA_INNER_GRAPHICS &pGraphics);

	/*****************************************************************************
	 函数名称：CreatDCAndBitmap
	 功能描述：创建兼容内存DC和与原图像相同大小的位图
	 输入参数：
		@hDC：			绘图句柄
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG CreatBitmapDC(HDC hDC);	

	LONG CreateIAFont();


	/*****************************************************************************
	 函数名称：IsGraphicToDelete
	 功能描述：判断根据该点是否可以删除该图形
	 输入参数：
		@pIAGraphics：		指向图形的指针
		@point：				当前鼠标点坐标
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsGraphicToDelete(const IA_INNER_GRAPHICS *pIAGraphics, POINT point);

	/*****************************************************************************
	 函数名称：StartModifyIAShapes
	 功能描述：智能分析开始修改图形
	 输入参数：
		@point：				鼠标起始点击位置坐标
		@MouseEvent：		鼠标点击类型（LEFT_BUTTON_DOWN 、RIGHT_BUTTON_DOWN）
		@ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG StartModifyIAShapes(POINT point, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：ModifyIAStraightLine
	 功能描述：智能分析开始修改直线
	 输入参数：
		@point：				鼠标起始点击位置坐标
		@MouseEvent：		鼠标点击类型（LEFT_BUTTON_DOWN 、RIGHT_BUTTON_DOWN）
		@ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG ModifyIAStraightLine(POINT point, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：ModifyIARectangle
	 功能描述：智能分析开始修改矩形
	 输入参数：
		@point：				鼠标起始点击位置坐标
		@pPointNo：			增加节点时指向新节点坐标的指针
		@MouseEvent：		鼠标点击类型（LEFT_BUTTON_DOWN 、RIGHT_BUTTON_DOWN）
		@ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG ModifyIARectangle(POINT point, LONG *pPointNo, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：ModifyIARectangleLine
	 功能描述：智能分析开始修改矩形（修改边：拖动单边、在边上增加节点）
	 输入参数：
		@pPointNo：			增加节点时指向新节点坐标的指针
		@MouseEvent：		鼠标点击类型（LEFT_BUTTON_DOWN 、RIGHT_BUTTON_DOWN）
		@ulPointIndex：		修改的边的前一个顶点坐标数组下标
		@point：				鼠标当前坐标
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG ModifyIARectangleLine(LONG *pPointNo, MOUSE_EVENT_TYPE MouseEvent, ULONG ulPointIndex, POINT point);

	/*****************************************************************************
	 函数名称：ModifyIARectanglePoint
	 功能描述：智能分析开始修改矩形（修改点：左键拖动单个点缩放矩形、右键点击单个点整体拖动）
	 输入参数：
		@MouseEvent：		鼠标点击类型（LEFT_BUTTON_DOWN 、RIGHT_BUTTON_DOWN）
		@ulPointIndex：		修改的顶点坐标数组下标
		@point：				鼠标当前坐标
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG ModifyIARectanglePoint(MOUSE_EVENT_TYPE MouseEvent, ULONG ulPointIndex, POINT /*point*/);

	/*****************************************************************************
	 函数名称：ModifyIAPolygon
	 功能描述：智能分析开始修改多边形
	 输入参数：
		 @point：			鼠标起始点击位置坐标
		 @pPointNo：			指向新节点坐标的指针
		 @MouseEvent：		鼠标点击类型（LEFT_BUTTON_DOWN 、RIGHT_BUTTON_DOWN）
		 @ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG ModifyIAPolygon(POINT point, LONG *pPointNo, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：ModifyIAPolygonPoint
	 功能描述：智能分析开始修改多边形（修改点：左键修改单个点位置、右键删除该点）
	 输入参数：
		@pPointNo：			指向新节点坐标的指针
		@MouseEvent：		鼠标点击类型（LEFT_BUTTON_DOWN 、RIGHT_BUTTON_DOWN）
		@ulGraphicGroup：	图形组标识
		@ulPointIndex：		修改的顶点坐标数组下标
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG ModifyIAPolygonPoint(LONG *pPointNo, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup, ULONG ulPointIndex);
	
	/*****************************************************************************
	 函数名称：ModifyIAPolygonLine
	 功能描述：智能分析开始修改多边形（修改边：左键整体拖动、右键增加新节点）
	 输入参数：
		@pPointNo：			增加节点时指向新节点坐标的指针
		@MouseEvent：		鼠标点击类型（LEFT_BUTTON_DOWN 、RIGHT_BUTTON_DOWN）
		@point：				鼠标当前坐标
		@ulPointIndex：		修改的顶点坐标数组下标
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG ModifyIAPolygonLine(LONG *pPointNo, MOUSE_EVENT_TYPE MouseEvent, POINT point, ULONG ulPointIndex);

	/*****************************************************************************
	 函数名称：ModifyIAULine
	 功能描述：智能分析开始修改U型线
	 输入参数：
		 @point：			鼠标起始点击位置坐标
		 @MouseEvent：		鼠标点击类型（LEFT_BUTTON_DOWN 、RIGHT_BUTTON_DOWN）
		 @ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG ModifyIAULine(POINT point, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：UpdateIAPolygon
	 功能描述：修改中更新多边形图形
	 输入参数：
		 @point：			鼠标起始点击位置坐标
		 @bFinish：			修改是否结束
		 @ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG UpdateIAPolygon(POINT point, BOOL bFinish, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：UpdateIAMove
	 功能描述：整体拖动图形时更新图形
	 输入参数：
		 @point：			鼠标当前坐标
		 @ulGraphicGroup：	图形组标识
		 @bFinish:			是否编辑结束
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG UpdateIAMove(POINT point, ULONG ulGraphicGroup, BOOL bFinish);

	/*****************************************************************************
	 函数名称：UpdateIAPolygonPoint
	 功能描述：拖动多边形单个节点时更新图形
	 输入参数：
		 @point：			鼠标起始点击位置坐标
		 @ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG UpdateIAPolygonPoint(POINT point, ULONG ulGraphicGroup);	
	
	/*****************************************************************************
	 函数名称：UpdateIAULine
	 功能描述：拖动U型线单个节点时更新图形
	 输入参数：
		 @point：			鼠标起始点击位置坐标
		 @bFinish：			修改是否结束
	 输出参数：NA
	 返 回 值：VOID
	 *****************************************************************************/
	LONG UpdateIAULine(POINT point, BOOL bFinish);

	/*****************************************************************************
	 函数名称：IsOnLine
	 功能描述：判断点是否在线上
	 输入参数：
		@pPointA：		线段起点坐标
		@pPointB：		线段终点坐标
		@point：			所判断的点坐标
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsOnLine(const POINT *pPointA, const POINT *pPointB, POINT point);

	/*****************************************************************************
	 函数名称：IsPointOnLine
	 功能描述：判断点是否在线上（内部）
	 输入参数：
		 @pPointA：		线段起点坐标
		 @pPointB：		线段终点坐标
		 @point：		所判断的点坐标
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsPointOnLine(const POINT *pPointA, const POINT *pPointB, POINT point);

	/*****************************************************************************
	 函数名称：IsInShape
	 功能描述：判断点是否在图形内
	 输入参数：
		@pIAGraphic：	指向图形的指针
		@point：			所判断的点坐标
	 输出参数：NA
	 返 回 值：LONG
	 *****************************************************************************/
	LONG IsInShape(const IA_INNER_GRAPHICS *pIAGraphic, POINT point);

	/*****************************************************************************
	 函数名称：IsPointInPolygon
	 功能描述：判断点是否在多边形内
	 输入参数：
		@pPoint：		多边形顶点数组
		@ulPointNum：	多边形顶点数
		@point：			所判断的点坐标
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsPointInPolygon(const POINT *pPoint, ULONG ulPointNum, POINT point );

	/*****************************************************************************
	 函数名称：IsPointInRectangle
	 功能描述：判断点是否在矩形内
	 输入参数：
		@pPoint：		多边形顶点数组
		@ulPointNum：	多边形顶点数
		@point：			所判断的点坐标
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsPointInRectangle(const POINT *pPoint, ULONG /*ulPointNum*/, POINT point);

	/*****************************************************************************
	 函数名称：IsLineSegmentCross
	 功能描述：判断两条线段是否相交(有交点)
	 输入参数：
		@pFirst1：		线1的起始坐标
		@pFirst2：		线2的起始坐标
		@pSecond1：		线1的终点坐标
		@pSecond2：		线2的终点坐标
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsLineSegmentCross(POINT pFirst1, POINT pFirst2, POINT pSecond1, POINT pSecond2);

	/*****************************************************************************
	 函数名称：DistanceToLine
	 功能描述：计算点到直线上最近一点的距离 X*X + Y*Y
	 输入参数：
		@Point：				点坐标
		@PointStart：		线的起始坐标
		@PointEnd：			线的终点坐标
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	LONG DistanceToLine(POINT Point, POINT PointStart, POINT PointEnd);

	/*****************************************************************************
	 函数名称：IsCloseToPoint
	 功能描述：判断点A是否与点B接近
	 输入参数：
		@pointA：		点A坐标
		@pointB：		点B坐标
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsCloseToPoint(POINT pointA, POINT pointB);

	/*****************************************************************************
	 函数名称：IsPointInside
	 功能描述：判断点是否在图像界内
	 输入参数：
		@point：			点坐标
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsPointInside(const POINT *point);

	/*****************************************************************************
	 函数名称：IsInForbiddenArea
	 功能描述：判断点是否在非法区域：寻找相应的RelativeGraphicGroup及与其的RelationshipType
	 输入参数：
		@point：				点坐标
		@ulGraphicGroup：	点所属的图形组标识
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsInForbiddenArea(POINT point, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：isPointInForbiddenArea
	 功能描述：判断点是否在非法区域（内部）：判断点是否在RelativeGraphicGroup图形中
	 输入参数：
		@point：				点坐标
		@ulGraphicGroup：	点所属的图形组标识
		@bIsIn：				图形组关系是否为包含的标志
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsPointInForbiddenArea(POINT point, ULONG ulGraphicGroup, BOOL bIsIn);

	/*****************************************************************************
	 函数名称：IsCrossForbiddenArea
	 功能描述：判断线段是否与非法区域相交：寻找相应的RelativeGraphicGroup及与其的RelationshipType
	 输入参数：
		@newPoint：			线段头节点坐标
		@lastPoint：			线段尾节点坐标
		@ulGraphicGroup：	线段所属的图形组标识
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsCrossForbiddenArea(POINT newPoint, POINT lastPoint, ULONG ulGraphicGroup);	
	
	/*****************************************************************************
	 函数名称：IsLineCrossForbiddenArea
	 功能描述：判断线段是否与非法区域相交（内部）：判断新的一条边是否与RelativeGraphicGroup图形相交
	 输入参数：
		@newPoint：			线段头节点坐标
		@lastPoint：			线段尾节点坐标
		@ulGraphicGroup：	该线段所属的图形组标识
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsLineCrossForbiddenArea(POINT newPoint, POINT lastPoint, ULONG ulGraphicGroup);

	/*****************************************************************************
	 函数名称：IsForbiddenOverlaped
	 功能描述：判断特定的图形是否满足与其他图形位置关系：寻找相应的RelativeGraphicGroup及与其的RelationshipType
	 输入参数：
		@pPoint：			当前图形各个节点的坐标数组
		@ulPointNum：		当前图形节点个数
		@ulGraphicGroup：	当前图形的图形组标识
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsForbiddenOverlaped(const POINT *pPoint, ULONG ulPointNum, ULONG ulGraphicGroup, ULONG ulGraphicType);

	/*****************************************************************************
	 函数名称：IsAreaForbiddenOverlaped
	 功能描述：判断特定的图形是否满足与其他图形位置关系（内部）：遍历IADrawGraphicsList，找到RelativeGraphicGroup的图形
	 输入参数：
		@pPoint：				当前图形各个节点的坐标数组
		@ulPointNum：			当前图形节点个数
		@ulGraphicGroup：		当前图形的图形组标识
		@ulGraphicType：			当前图形的类型
		@ulRelationShipType：	图形组关系
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsAreaForbiddenOverlaped(const POINT *point, ULONG ulPointNum, ULONG ulGraphicGroup, ULONG ulGraphicType, RELATION_SHIP_TYPE ulRelationShipType);

	/*****************************************************************************
	 函数名称：IsShapeForbiddenOverlaped
	 功能描述：判断特定的图形是否满足与其他图形位置关系（内部）：判断该图形是否与RelativeGraphicGroup图形非法重叠
	 输入参数：
		@pIAGraphics：			指向位置相关图形的指针
		@pPoint：				当前图形各个节点的坐标数组
		@ulPointNum：			当前图形节点个数
		@ulGraphicType：			当前图形的类型
		@ulRelationShipType：	图形组关系
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsShapeForbiddenOverlaped(const IA_INNER_GRAPHICS *pIAGraphic, const POINT *pPoint, ULONG ulPointNum, ULONG ulGraphicType, RELATION_SHIP_TYPE ulRelationShipType); 
	
	/*****************************************************************************
	 函数名称：IsRectangleSizeIllegal
	 功能描述：判断矩形长宽是否合法：寻找相应的RelativeGraphicGroup及与其的RelationshipType
	 输入参数：
		@pPoint：			图形顶点数组
		@ulPointNum:		顶点个数
		@ulGraphicGroup：	图形组标识
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsRectangleSizeIllegal(const POINT *pPoint, ULONG ulPointNum, ULONG ulGraphicGroup);
		
	/*****************************************************************************
	 函数名称：IsRectangleEdgeIllegal
	 功能描述：判断矩形长宽是否合法（内部）：遍历IADrawGraphicsList，判断该图形是否与RelativeGraphicGroup图形大小关系正确
	 输入参数：
		@pPoint：				图形顶点数组
		@ulPointNum:			顶点个数
		@ulGraphicGroup：		图形组标识
		@ulRelationShipType：	位置关系类型
	 输出参数：NA
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL IsRectangleEdgeIllegal(const POINT *pPoint, ULONG ulPointNum, ULONG ulGraphicGroup, RELATION_SHIP_TYPE ulRelationShipType);

	/*****************************************************************************
	 函数名称：FindEdgeRectangle
	 功能描述：寻找包含特定图形的最小矩形框
	 输入参数：
		@pPoint：			图形顶点数组
		@ulPointNum:		顶点个数
	 输出参数：
		@rc：				该最小矩形框的各边信息
	 返 回 值：BOOL
	 *****************************************************************************/
	BOOL FindEdgeRectangle(const POINT *pPoint, ULONG ulPointNum, RECT &rc);


private:

	ULONG					m_ulGraphicID;				// 内部图形ID
	ULONG					m_ulMaxDrawNum;				// 智能分析最大画图个数
	ULONG					m_ulCurrentState;			// 当前修改的状态
	ULONG					m_ulCurrentCursor;			// 当前鼠标状态
	ULONG					m_ulCurrentGraphicType;		// 当前所编辑图形的类型
	ULONG					m_ulMaxGroupShapeNum;		// 当前图形组最大图形个数
	HWND					m_hMsgWnd;					// 窗口句柄
	ULONG					m_ulWndWidth;				// 窗口宽度
	ULONG					m_ulWndHeight;				// 窗口高度
	ULONG					m_ulDCWidth;				// 显示图像宽度
	ULONG					m_ulDCHeight;				// 显示图像高度
// 	ULONG					m_ulScreenWidth;			// 屏幕宽度
// 	ULONG					m_ulScreenHeight;			// 屏幕高度

	FLOAT					m_fDiv;						// DC/Wnd 宽比
	LONG					m_lLineWidthAdjust;			// 图形线宽系数（调节分辨率变化）
	LONG					m_lDistanceCloseToLine;
	LONG					m_lArrowLenth;
	LONG					m_lFontSize;

	BOOL					m_bScaleModeFull;			// 是否满窗口显示图像
	LONG					m_lOffsetX;					// 图像显示在窗口中X轴坐标偏差
	LONG					m_lOffsetY;					// 图像显示在窗口中Y轴坐标偏差

	IA_INNER_GRAPHICS		m_IAGraphic;				// 正在画的智能分析图形结构体
	BOOL					m_bActive;					// 当前图形是否有效
	BOOL					m_bWindowPoint;				// 当前图形中坐标点是否为窗口坐标
	BOOL					m_bReversePoint;			// 是否直线编辑过程中坐标反转

	CIVSMutex				m_Mutex;					// 智能分析图形列表锁

	LONG					m_lPointNo;					// 记录图形所修改的顶点下标
	POINT					m_RecordPoint;				// 记录整体移动图形时原鼠标点击点，用于计算位移量
	POINT					m_OriginIAPoints[POINTS_NUM_MAX];	//记录整体移动图形时原图各坐标点
	IA_GRAPHICS_LIST		m_IADrawGraphicsList;		// 智能分析画图图形列表
	IA_RELATIONS_LIST		m_IARelationShipList;		// 智能分析图形组关系列表

	HDC						m_memhdc;					// 兼容内存DC句柄 
	BLENDFUNCTION			m_bf;						// AlphaBlending中的结构体参数
	HBITMAP					m_hbitmap;					// 位图句柄
	BITMAPINFO				m_bmi;						// 位图头部信息
	VOID					*m_pvBits;					// 指向DIB section的指针
	LOGFONT					m_lf;						// 创建字体所需结构体
	HFONT					m_hFont;					// 字体句柄

    ULONG         m_ulUlineModifyIndex;    // U型线当前被修改的顶点(只能是1或2，)编号 DTS2013051709238

};

#endif //__CIADRAWER_H__


