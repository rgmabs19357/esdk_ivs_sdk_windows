/********************************************************************
filename 	: IVS_DrawDataType.h
author   	: z00227377
created 	: 2012/12/28
description : 画图功能所包含结构体
copyright 	: Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history 	: 2012/12/28 初始版本
*********************************************************************/
#ifndef __IVS_DRAW_DATATYPE_H__
#define __IVS_DRAW_DATATYPE_H__


#include <Windows.h>

#pragma pack(push, 1)


#define GRAPHICS_STATE_NONE		0	
#define GRAPHICS_STATE_DRAWING	1
#define GRAPHICS_STATE_EDITING	2


#define POINTS_NUM_MAX      100		// 单个图形最大点数
#define CHARACTER_NUM_MAX   99		// 图形组名称最大字符数
#define GRAPHICS_NUM_MAX	60		// 最大画图数
#define FLOAT_TO_LONG_COEFF 1000000 // 浮点数转成长整型系数

#define ARROW_LENTH						40

// 智能分析鼠标显示类型
enum CURSOR_DISPLAY_TYPE{
	CURSOR_TYPE_ARROW = 0,	// 标准箭头
	CURSOR_TYPE_CROSS,		// 十字瞄准线(开始画图)
	CURSOR_TYPE_SIZEALL,	// Four-pointed arrow pointing north, south, east, and west
	CURSOR_TYPE_SIZENESW,	// Double-pointed arrow pointing northeast and southwest 
	CURSOR_TYPE_SIZENS,		// Double-pointed arrow pointing north and south 
	CURSOR_TYPE_SIZENWSE,	// Double-pointed arrow pointing northwest and southeast 
	CURSOR_TYPE_SIZEWE,		// Double-pointed arrow pointing west and east 
};


// 智能分析规则图形绘制类型
enum  GRAPHIC_TYPE {
   IA_GRAPHICS_TYPE_ILLEGAL       = 0,	//非法类型
   IA_GRAPHICS_TYPE_RECTANGLE     = 1,	//矩形
   IA_GRAPHICS_TYPE_POLYGON       = 2,	//多边形
   IA_GRAPHICS_TYPE_STRAIGHT_LINE = 3,	//直线
   IA_GRAPHICS_TYPE_UP_ULINE      = 4,	//U形线
   IA_GRAPHICS_TYPE_DOWN_ULINE    = 5,	//N形线

   // add new types here
   IA_GRAPHICS_TYPE_COUNT         = 6,
};

enum  ARROW_TYPE {
	ARROW_TYPE_LINE_TO_LEFT  = 1,	// 指向左边
	ARROW_TYPE_LINE_TO_RIGHT = 2,	// 指向右边
	ARROW_TYPE_LINE_TO_BOTH  = 3,	// 指向双边

	// add new types here
	ARROW_TYPE_OTHER		 = 4,
};

#if 0
#define IA_GRAPHICS_TYPE_ILLEGAL 0		 //非法类型
#define IA_GRAPHICS_TYPE_RECTANGLE 1     //矩形
#define IA_GRAPHICS_TYPE_POLYGON 2       //多边形
#define IA_GRAPHICS_TYPE_STRAIGHT_LINE 3 //直线
#define IA_GRAPHICS_TYPE_UP_ULINE 4      //U形线
#define IA_GRAPHICS_TYPE_DOWN_ULINE 5    //N形线
#endif


// 图片名称显示位置
enum  GRAPHIC_NAME_POSITION {
	GRAPHIC_NAME_ABOVE     = 0,	//图形上方
	GRAPHIC_NAME_UNDER     = 1,	//图形下方
	GRAPHIC_NAME_INSIDE    = 2,	//图形内部下方

	// add new types here
	GRAPHIC_NAME_OTHER     = 3,
};



// 图形组的位置关系
typedef enum 
{
	IA_GRAPHICS_RELATION_TYPE_NONE,			// 无位置关系
	IA_GRAPHICS_RELATION_TYPE_BE_COVERED,	// 被包含
	IA_GRAPHICS_RELATION_TYPE_COVER,		// 包含
	IA_GRAPHICS_RELATION_TYPE_NOT_CROSS,	// 互相独立不相交
	IA_GRAPHICS_RELATION_TYPE_SMALLER,		// （矩形）长宽都小于
	IA_GRAPHICS_RELATION_TYPE_BIGGER		// （矩形）长宽都大于
}RELATION_SHIP_TYPE;

// 鼠标事件类型
typedef enum 
{
	LEFT_BUTTON_DOWN = 0,	//左键点击
	RIGHT_BUTTON_DOWN		//右键点击
} MOUSE_EVENT_TYPE;

// 颜色结构体
typedef struct
{
	UCHAR	Alpha;		// 透明度（0~255）
	UCHAR	Red;		// R值（0~255）
	UCHAR	Green;		// G值（0~255）
	UCHAR	Blue;		// B值（0~255）
}IA_COLOR;

typedef struct
{
	POINT ArrowHead[2];
	POINT ArrowTail[2];
	POINT ArrowTrangle1[2];
	POINT ArrowTrangle2[2];
}IA_ARROW;

// 单个图形结构体
typedef struct
{
	ULONG		ulGraphicGroup;					// 图形组标识
	IA_COLOR	BgColor;						// 图形背景色
	IA_COLOR	LineColor;						// 图形边线色
	ULONG		ulLineWidth;					// 边线宽度
	ULONG		ulGraphicType;					// 几何图形类型(矩形、多边形、直线，U形)
	ULONG		ulPointsNum;					// 图形点数
	POINT		IAPoints[POINTS_NUM_MAX];		// 点坐标
	CHAR		GraphicGroupName[CHARACTER_NUM_MAX];// 图形组名称 （最大长度99字节）
	ULONG 		NamePosition;					// 图形组名称显示方位（0: 图形上方，1: 图形下方，2:内部下方）
	BOOL		bAllowDelete;					// 是否允许鼠标右键删除
	BOOL		bAllowAddNewPoint;				// 是否允许增加新顶点
	BOOL		bAllowModifySize;				// 是否允许修改矩形大小
	BOOL		bKeepRectShape;					// 是否与原图像宽高比一致
	BOOL		bKeepHorizon;					// 是否直线保持水平
	ULONG		ulArrowType;
	IA_ARROW    IAArrow;
}IA_GRAPHICS;

#pragma pack(pop)

#endif // __IVS_DRAW_DATATYPE_H__
