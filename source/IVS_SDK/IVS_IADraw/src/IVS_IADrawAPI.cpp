#include "IVS_IADrawAPI.h"
#include "IVSCommon.h"
#include "IVS_IADraw.h"
#include "ivs_log.h"
#include "ivs_error.h"


// 检查句柄有效性
#define CHECK_HANDLE(h) \
	do { IVS_ASSERT(h); if (!(h)) return IVS_FAIL;} while(0)


// 初始化智能分析库
IVS_DRAW_API INT Draw_LibInit(const char* pLogPath)
{
	int iRet = 0;
    // 指定目录（多进程场景）
    const char* pLogPathTemp = "../log/";
    if (NULL != pLogPath && 0 != strlen(pLogPath))
    {
        pLogPathTemp = pLogPath;
    }
	iRet = IVS_LogInit(pLogPathTemp, "mp_log.conf" , "IVS_IA_Draw", "ia_draw", 102, NULL);
    return iRet;
}

// 清理智能分析库
IVS_DRAW_API VOID Draw_LibClean()
{
	IVS_LogClean();
    return;
}

// 获取IADraw句柄
IVS_DRAW_API DRAW_HANDLE Draw_GetHandle()
{
	CIADrawer *pIADrawer = IVS_NEW(pIADrawer);
	if (NULL == pIADrawer)
	{
		IVS_LOG(IVS_LOG_ERR, "Get Handle", "ALLOC MEM ERROR.");
		return (DRAW_HANDLE)NULL;
	}
	return (DRAW_HANDLE)pIADrawer;
}

// 初始化智能分析画图
IVS_DRAW_API LONG Draw_Init(DRAW_HANDLE handle, HWND hMsgWnd)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->InitIADrawer(hMsgWnd);
}

// 设置是否满窗口显示
IVS_DRAW_API LONG Draw_SetDisplayScaleMode(DRAW_HANDLE handle, BOOL bScaleModeFull)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	pIADrawer->SetDisplayScaleMode(bScaleModeFull);
	return IVS_SUCCEED;
}


// 按组获取智能分析图形
IVS_DRAW_API LONG Draw_GetGraphicsByGroup(DRAW_HANDLE handle, ULONG ulGraphicGroup, IA_GRAPHICS *pGraphics, ULONG *pulNum)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->GetIAGraphics(ulGraphicGroup, pGraphics, pulNum);
}

// 获取智能分析选中的图形
IVS_DRAW_API LONG Draw_GetGraphicSelected(DRAW_HANDLE handle, IA_GRAPHICS *pGraphic)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->GetIAGraphicSelected(pGraphic);
}

// 获取智能分析所有图形
IVS_DRAW_API LONG Draw_GetGraphicsAll(DRAW_HANDLE handle, IA_GRAPHICS *pGraphics, ULONG *pulNum)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->GetIAGraphics(0, pGraphics, pulNum);
}

// 设置智能分析图形
IVS_DRAW_API LONG Draw_SetGraphics(DRAW_HANDLE handle, const IA_GRAPHICS *pGraphics, ULONG ulNum, BOOL bPersent)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->SetIAGraphics(pGraphics, ulNum, bPersent);
}

// 设置智能分析图形组之间的关系
IVS_DRAW_API LONG Draw_SetRelationShip(DRAW_HANDLE handle, 
									   ULONG ulGraphicGroupA, 
									   ULONG ulGraphicGroupB, 
									   RELATION_SHIP_TYPE ulRelationShipType)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->SetRelationShip(ulGraphicGroupA, ulGraphicGroupB, ulRelationShipType);
}



// 智能分析开始画直线
IVS_DRAW_API LONG Draw_StraightLineStart(DRAW_HANDLE handle, ULONG ulNum, IA_GRAPHICS *pIAGraphics)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;	
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(pIAGraphics->IAPoints))
		return IVS_FAIL;
	return pIADrawer->SetIAStartStatus(ulNum, pIAGraphics);
}

// 智能分析随鼠标移动更新直线对应坐标
IVS_DRAW_API LONG Draw_StraightLineUpDate(DRAW_HANDLE handle, POINT point)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
		return IVS_FAIL;
	return pIADrawer->DrawIAStraightLine(point, FALSE, FALSE);
}

// 智能分析结束画直线并设定终点
IVS_DRAW_API LONG Draw_StraightLineFinish(DRAW_HANDLE handle, POINT point, ULONG /*ulGraphicGroup*/)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
	{
		pIADrawer->InsertCurrentToGraphicList();
// 		pIADrawer->StopDrawGraphics(ulGraphicGroup);
		return IVS_SUCCEED;
	}
	return pIADrawer->DrawIAStraightLine(point, TRUE, TRUE);

}

// 智能分析开始画矩形
IVS_DRAW_API LONG Draw_RectangleStart(DRAW_HANDLE handle, ULONG ulNum, IA_GRAPHICS *pIAGraphics)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(pIAGraphics->IAPoints))
		return IVS_FAIL;
	return pIADrawer->SetIAStartStatus(ulNum, pIAGraphics);
}

// 智能分析随鼠标移动更新矩形对应坐标
IVS_DRAW_API LONG Draw_RectangleUpDate(DRAW_HANDLE handle, POINT point)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
		return IVS_FAIL;
	return pIADrawer->DrawIARectangle(point, FALSE, TRUE);
}

// 智能分析结束画矩形并设定最终各顶点
IVS_DRAW_API LONG Draw_RectangleFinish(DRAW_HANDLE handle, POINT point, ULONG /*ulGraphicGroup*/)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
	{
		pIADrawer->InsertCurrentToGraphicList();
// 		pIADrawer->StopDrawGraphics(ulGraphicGroup);
		return IVS_SUCCEED;
	}
	return pIADrawer->DrawIARectangle(point, TRUE, TRUE);
}

IVS_DRAW_API LONG Draw_InsertCurrentToGraphicList(DRAW_HANDLE handle)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	pIADrawer->InsertCurrentToGraphicList();
	return IVS_SUCCEED;
}

// 智能分析开始画多边形
IVS_DRAW_API LONG Draw_PolygonStart(DRAW_HANDLE handle, ULONG ulNum, IA_GRAPHICS *pIAGraphics)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(pIAGraphics->IAPoints))
		return IVS_FAIL;
	return pIADrawer->SetIAStartStatus(ulNum, pIAGraphics);
}

// 智能分析随鼠标移动更新多边形最新一条边的终点坐标
IVS_DRAW_API LONG Draw_PolygonUpDate(DRAW_HANDLE handle, POINT point, ULONG ulGraphicGroup)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
		return IVS_FAIL;
	return pIADrawer->DrawIAPolygon(point, FALSE, FALSE, ulGraphicGroup);
}

// 智能分析点击确定多边形中间节点位置坐标
IVS_DRAW_API LONG Draw_PolygonSetPoint(DRAW_HANDLE handle, POINT point, ULONG ulGraphicGroup)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
	{
// 		pIADrawer->StopDrawGraphics(ulGraphicGroup);
		return IVS_FAIL;
	}
	return pIADrawer->DrawIAPolygon(point, TRUE, FALSE, ulGraphicGroup);
}

// 智能分析点击确定多边形最后一边并且闭合图形
IVS_DRAW_API LONG Draw_PolygonFinish(DRAW_HANDLE handle, POINT point, ULONG ulGraphicGroup)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
	{
// 		pIADrawer->InsertCurrentToGraphicList();
		pIADrawer->StopDrawGraphics(ulGraphicGroup);
		return IVS_SUCCEED;
	}
	return pIADrawer->DrawIAPolygon(point, TRUE, TRUE, ulGraphicGroup);
}

// 智能分析开始画U型线
IVS_DRAW_API LONG Draw_ULineStart(DRAW_HANDLE handle, ULONG ulNum, IA_GRAPHICS *pIAGraphics)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(pIAGraphics->IAPoints))
		return IVS_FAIL;
	return pIADrawer->SetIAStartStatus(ulNum, pIAGraphics);
}

// 智能分析随鼠标移动更新U型线最新一条边的终点坐标
IVS_DRAW_API LONG Draw_ULineUpDate(DRAW_HANDLE handle, POINT point)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
		return IVS_FAIL;
	return pIADrawer->DrawIAULine(point, FALSE, FALSE, TRUE);
}

// 智能分析点击确定U型框中间节点位置坐标
IVS_DRAW_API LONG Draw_ULineSetPoint(DRAW_HANDLE handle, POINT point, ULONG /*ulGraphicGroup*/)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
	{
// 		pIADrawer->StopDrawGraphics(ulGraphicGroup);
		return IVS_FAIL;
	}
	return pIADrawer->DrawIAULine(point, TRUE, FALSE, TRUE);
}

// 智能分析点击确定U型线最后一边并且结束画U型线
// （若已确定三条边，直接结束画图；若第三条边未确定，则以当前点为U型线第三边终点）
IVS_DRAW_API LONG Draw_ULineFinish(DRAW_HANDLE handle, POINT point, ULONG /*ulGraphicGroup*/)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
	{
		pIADrawer->InsertCurrentToGraphicList();
// 		pIADrawer->StopDrawGraphics(ulGraphicGroup);
		return IVS_SUCCEED;
	}
	return pIADrawer->DrawIAULine(point, TRUE, TRUE, TRUE);
}



// 智能分析结束某图形组的画图（若存在正在画的该组图形，直接丢弃）
IVS_DRAW_API LONG Draw_StopDrawGraphics(DRAW_HANDLE handle, ULONG ulGraphicGroup)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->StopDrawGraphics(ulGraphicGroup);
}




// 显示所有智能分析图形
IVS_DRAW_API LONG Draw_ShowGraphics(DRAW_HANDLE handle,HDC hDC, const RECT *rc)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->ShowIAGraphics(hDC, rc);
}


// 删除所有选中图形
IVS_DRAW_API LONG Draw_DeleteGraphicsSelected(DRAW_HANDLE handle)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->DeleteIAGraphicsSelected();
}

// 根据鼠标点坐标删除智能分析单个图形（若坐标在指定图形组的单个图形内部，删除该图形；若在多个图形重叠区域，则不删除）
IVS_DRAW_API LONG Draw_DeleteGraphicsByPoint(DRAW_HANDLE handle, ULONG ulGraphicGroup, POINT point)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->DeleteIAGraphicsByPoint(ulGraphicGroup, point);
}

// 清除指定图形组的所有图形
IVS_DRAW_API LONG Draw_DeleteGraphicsByGroup(DRAW_HANDLE handle, ULONG ulGraphicGroup)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->DeleteGraphicsByGroup(ulGraphicGroup);
}

// 清除智能分析所有图形
IVS_DRAW_API LONG Draw_DeleteGraphics(DRAW_HANDLE handle)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->DeleteIAGraphicsAll();
}



// 根据鼠标点坐标选中智能分析单个图形（选择特定组中与鼠标坐标最近的单个图形）
IVS_DRAW_API LONG Draw_SelectGraphicByPoint(DRAW_HANDLE handle, POINT point, ULONG *ulGraphicGroup, BOOL *bAllowDel)
{
	CHECK_HANDLE(handle);
	if ((NULL == ulGraphicGroup) || (NULL == bAllowDel))
	{
		return IVS_PARA_INVALID;
	}
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
	{
		return IVS_FAIL;
	}
	return pIADrawer->SelectGraphicByPoint(point, *ulGraphicGroup, *bAllowDel);
}

IVS_DRAW_API LONG Draw_GetCursorType(DRAW_HANDLE handle, POINT Point)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&Point))
	{
		return CURSOR_TYPE_ARROW;
	}
	return pIADrawer->GetCursorType(Point);
}


// 选中指定图形组的所有图形
IVS_DRAW_API LONG Draw_SelectGraphicByGroup(DRAW_HANDLE handle, ULONG ulGraphicGroup, BOOL *bAllowDel)
{
	CHECK_HANDLE(handle);
	if (NULL == bAllowDel)
	{
		return IVS_PARA_INVALID;
	}

	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->SelectGraphicByGroup(ulGraphicGroup, *bAllowDel);
}

// 智能分析开始修改图形
IVS_DRAW_API LONG Draw_StartModifyGraphics(DRAW_HANDLE handle, POINT point, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
		return IVS_FAIL;
	return pIADrawer->StartModifyIAGraphics(point, MouseEvent, ulGraphicGroup);
}

// 智能分析随鼠标移动修改图形相应的坐标
IVS_DRAW_API LONG Draw_ModifyShapeUpDate(DRAW_HANDLE handle, ULONG ulGraphicGroup, POINT point)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
		return IVS_FAIL;
	return pIADrawer->ModifyIAShapeUpDate(point, FALSE, ulGraphicGroup);
}

// 智能分析结束修改图形
IVS_DRAW_API LONG Draw_ModifyFinish(DRAW_HANDLE handle, ULONG ulGraphicGroup, POINT point)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(&point))
	{
		pIADrawer->InsertCurrentToGraphicList();
		return IVS_SUCCEED;
	}
	return pIADrawer->ModifyIAShapeUpDate(point, TRUE, ulGraphicGroup);
}

// 取消选中智能分析图形
IVS_DRAW_API LONG Draw_SelectCancel(DRAW_HANDLE handle)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->SelectCancel();
}

// 判断是否达到该图形组最大画图数
IVS_DRAW_API BOOL Draw_IsFinishDrawGraphicGroup(DRAW_HANDLE handle, ULONG ulGraphicGroup, ULONG ulMaxDrawNum)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->IsFinishDrawGraphicGroup(ulGraphicGroup, ulMaxDrawNum);
}



// 释放IADraw句柄
IVS_DRAW_API LONG Draw_FreeHandle(DRAW_HANDLE handle)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	IVS_DELETE(pIADrawer);
	return IVS_SUCCEED;
}

// add by z00193167
// 根据已知的智能分析结构画图
IVS_DRAW_API LONG Draw_RectangleStartByGraphicPoint(DRAW_HANDLE handle, ULONG ulNum, const IA_GRAPHICS *pIAGraphics)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->SetIAStartStatus(ulNum, pIAGraphics);
}

// 智能分析结束画矩形并设定最终各顶点
IVS_DRAW_API LONG Draw_RectangleFinishByGraphicPoint(DRAW_HANDLE handle, POINT point/*, ULONG ulGraphicGroup*/)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->DrawIARectangle(point, TRUE, TRUE);
}

IVS_DRAW_API LONG Draw_WindowPointToGraphicPoint(DRAW_HANDLE handle, POINT* pPoint)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	if (IVS_SUCCEED != pIADrawer->WindowPointToGraphicPoint(pPoint))
	{
		pIADrawer->InsertCurrentToGraphicList();
		// 		pIADrawer->StopDrawGraphics(ulGraphicGroup);
		return IVS_SUCCEED;
	}
	
	return IVS_SUCCEED;
}

// 智能分析开始修改图形
IVS_DRAW_API LONG Draw_StartModifyGraphicsByGraphicPoint(DRAW_HANDLE handle, POINT point, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->StartModifyIAGraphics(point, MouseEvent, ulGraphicGroup);
}

// 智能分析结束修改图形
IVS_DRAW_API LONG Draw_ModifyFinishByGraphicPoint(DRAW_HANDLE handle, ULONG ulGraphicGroup, POINT point)
{
	CHECK_HANDLE(handle);
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->ModifyIAShapeUpDate(point, TRUE, ulGraphicGroup);
}

// 根据鼠标点坐标选中智能分析单个图形（选择特定组中与鼠标坐标最近的单个图形）
IVS_DRAW_API LONG Draw_SelectGraphicByGraphicPoint(DRAW_HANDLE handle, POINT point, ULONG *ulGraphicGroup, BOOL *bAllowDel)
{
	CHECK_HANDLE(handle);
	if ((NULL == ulGraphicGroup) || (NULL == bAllowDel))
	{
		return IVS_PARA_INVALID;
	}
	CIADrawer *pIADrawer = (CIADrawer *)handle;
	return pIADrawer->SelectGraphicByPoint(point, *ulGraphicGroup, *bAllowDel);
}

