#include "IVS_IADraw.h"
#include "IVSCommon.h"
#include "ivs_log.h"
#include "ivs_error.h"
#include <math.h>

// 半透明图形显示时需要用到Msimg32.lib
#pragma comment(lib,"Msimg32.lib")

#define CHECK_RESULT(s, _ret)\
	do { if(s) return _ret;}while(0)


#define CHECK_RESULT_TRUE_LOG(s, _FunctionName, _ErrLog)\
	do { if(s)											\
		{IVS_LOG(IVS_LOG_ERR, _FunctionName, _ErrLog);	\
		return TRUE;}									\
	}while(0)


#define POINT_NUM_OF_RECTANGLE			4			// 矩形顶点个数
#define POINT_NUM_OF_ULINE				4			// U型线点数
#define POINTNO_ILLEGAL					-1			// 被修改的图形顶点下标默认值
#define ULine_BOTTOM_WIDTH_MIN			40			// U型线底部水平线最小宽度
#define CLOSE_TO_LINE_DISTANCE_ON_WND	5			// 与点临近的最大距离：X * X + Y * Y
#define CLOSE_TO_LINE_DISTANCE			(m_lDistanceCloseToLine * m_lDistanceCloseToLine)// 与线段临近的最大距离：X * X + Y * Y
#define MAX_DISTANCE_VAL				0x7FFFFFFF	// 最大距离值（LONG型）
#define ARROW_LENTH						40
#define FONT_SIZE						20
#define MAX_RESOLUTIONG_RATIO			4096		// 最大分辨率


//智能分析画图状态
#define IA_DONOTHING						0		// 无
#define IA_MODIFING_MOVING_SHAPE			1		// 整体移动单个图形
#define IA_MODIFING_STRAIGHT_LINE_POINT		2		// 修改直线单个点坐标
#define IA_MODIFING_RECTANGLE_HORIZON_LINE	3		// 修改矩形水平边（单边上下移动）
#define IA_MODIFING_RECTANGLE_VERTICAL_LINE	4		// 修改矩形垂直边（单边左右移动）
#define IA_MODIFING_RECTANGLE_POINT			5		// 修改矩形顶点（整体放大缩小）
#define IA_MODIFING_POLYGON_MOVING_POINT	6		// 修改多边形单个点坐标
#define IA_MODIFING_POLYGON_DELETE_POINT	7		// 删除多边形单个点
#define IA_MODIFING_POLYGON_ADD_POINT		8		// 在多边形某条边上新增顶点
#define IA_MODIFING_ULINE_POINT				9		// 修改U型线端点
#define IA_MODIFING_ULINE_ANGLE				10		// 修改U型线水平线两端的角


CIADrawer::CIADrawer()
	:m_ulGraphicID(0)
	,m_ulMaxDrawNum(0)
	,m_ulCurrentState(IA_DONOTHING)
	,m_ulCurrentCursor(CURSOR_TYPE_ARROW)
	,m_ulCurrentGraphicType(IA_GRAPHICS_TYPE_ILLEGAL)
	,m_ulMaxGroupShapeNum(0)
	,m_hMsgWnd(NULL)
	,m_ulWndWidth(0)
	,m_ulWndHeight(0)
	,m_ulDCWidth(0)
	,m_ulDCHeight(0)
	,m_fDiv(1)
	,m_lLineWidthAdjust(1)
	,m_lDistanceCloseToLine(0)
	,m_lArrowLenth(0)
	,m_lFontSize(0)
	,m_bScaleModeFull(FALSE)
	,m_lOffsetX(0)
	,m_lOffsetY(0)
	,m_bActive(FALSE)
	,m_bWindowPoint(FALSE)
	,m_bReversePoint(FALSE)
	,m_lPointNo(POINTNO_ILLEGAL)
	,m_memhdc(NULL)
	,m_hbitmap(NULL)
	,m_pvBits(NULL)
	,m_hFont(NULL)
	,m_ulUlineModifyIndex(0)
{
	memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));
	memset(&m_RecordPoint, 0, sizeof(POINT));
	memset(m_OriginIAPoints, 0, sizeof(POINT) * POINTS_NUM_MAX);
	memset(&m_bf, 0, sizeof(BLENDFUNCTION));
	memset(&m_bmi, 0, sizeof(BITMAPINFO));
	memset(&m_lf, 0, sizeof(LOGFONT));

}

CIADrawer::~CIADrawer()
{
	if (NULL != m_hbitmap)
	{
		DeleteObject(m_hbitmap);
		m_hbitmap = NULL;
	}
	if (NULL != m_memhdc)
	{
		DeleteDC(m_memhdc);
		m_memhdc = NULL;
	}
	if (NULL != m_hFont)
	{
		DeleteObject(m_hFont); 
		m_hFont = NULL;
	}
	
}

// 初始化智能分析画图
LONG CIADrawer::InitIADrawer(HWND hMsgWnd)
{
	if (!IsWindow(hMsgWnd))
	{
		IVS_LOG(IVS_LOG_ERR, "Init IA Drawer", "hMsgWnd NULL");
		return IVS_PARA_INVALID;
	}
    m_hMsgWnd = hMsgWnd;

	RECT Wndrect;
    memset(&Wndrect, 0, sizeof(Wndrect));
	GetClientRect(m_hMsgWnd, &Wndrect); 
	m_ulWndWidth  = (ULONG)(Wndrect.right  - Wndrect.left);		// 窗口宽度
	m_ulWndHeight = (ULONG)(Wndrect.bottom - Wndrect.top);		// 窗口高度
	// 判断窗口长宽是否为0
	if (( 0 == m_ulWndWidth) || (0 == m_ulWndHeight))
	{
		IVS_LOG(IVS_LOG_ERR, "Init IA Drawer", "hMsgWnd Size 0");
// 		return IVS_FAIL;
	}

	m_ulGraphicID		= 0;
	m_ulDCWidth			= 0;
	m_ulDCHeight		= 0;
	m_fDiv				= 1;
	m_lLineWidthAdjust	= 1;
	m_lDistanceCloseToLine = CLOSE_TO_LINE_DISTANCE_ON_WND;
	m_lArrowLenth		= ARROW_LENTH;
	m_lFontSize			= 0;
	m_bScaleModeFull	= FALSE;
	m_lOffsetX			= 0;
	m_lOffsetY			= 0;

	m_ulCurrentState	= IA_DONOTHING;
	m_ulCurrentCursor	= CURSOR_TYPE_ARROW;
	m_ulCurrentGraphicType = IA_GRAPHICS_TYPE_ILLEGAL;
	m_ulMaxDrawNum		= GRAPHICS_NUM_MAX;
	m_ulMaxGroupShapeNum  = 0;
	m_lPointNo			= POINTNO_ILLEGAL;
	m_RecordPoint.x		= 0;
	m_RecordPoint.y		= 0;
	m_bActive			= FALSE;
	m_bWindowPoint		= FALSE;
	m_bReversePoint		= FALSE;

	memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));
	memset(&m_RecordPoint, 0, sizeof(POINT));
	memset(m_OriginIAPoints, 0, sizeof(POINT) * POINTS_NUM_MAX);
	memset(&m_bf, 0, sizeof(BLENDFUNCTION));
	memset(&m_bmi, 0, sizeof(BITMAPINFO));
	memset(&m_lf, 0, sizeof(LOGFONT));
	m_IADrawGraphicsList.clear();
	m_IARelationShipList.clear();
	
	return IVS_SUCCEED;
}

// 设置是否满窗口显示
VOID CIADrawer::SetDisplayScaleMode(BOOL bScaleModeFull)
{
	m_bScaleModeFull = bScaleModeFull;
}

// 转换窗口坐标为图像坐标
LONG CIADrawer::WindowPointToGraphicPoint(POINT *point)
{
	IVS_TRACE();
	if ((0 == m_ulWndWidth) || (0 == m_ulWndHeight))
	{
		IVS_LOG(IVS_LOG_ERR, "Window Point To Graphic Point", "Window Size is Zero");
		return IVS_FAIL;
	}

	GetOffset();
	point->x = (point->x - m_lOffsetX) * (LONG)m_ulDCWidth  
		/ ((LONG)m_ulWndWidth - 2 * m_lOffsetX);
	point->y = (point->y - m_lOffsetY) * (LONG)m_ulDCHeight 
		/ ((LONG)m_ulWndHeight - 2 * m_lOffsetY);

	// 判断点是否在图像界内
	if (IsPointInside(point))
	{
		return IVS_SUCCEED;
	}
	IVS_LOG(IVS_LOG_ERR, "Window Point To Graphic Point", "Point Outside Graphic");
	return IVS_FAIL;
	
}

// 设置智能分析图形
LONG CIADrawer::SetIAGraphics(const IA_GRAPHICS *pGraphics, ULONG ulNum, BOOL bPersent)
{
	IVS_TRACE();
	if ((NULL == pGraphics) || (0 == ulNum))
	{
		IVS_LOG(IVS_LOG_ERR, "Set IA Graphics", "Invalid Argument");
		return IVS_PARA_INVALID;
	}
	// 判断传入的图形数组中是否有不合法图形
	if (!bPersent && IsSetGraphicsInvalid(pGraphics, ulNum))
	{
		return IVS_PARA_INVALID;
	}

	CAutoLock Lock(m_Mutex);

	LONG lRet = IVS_FAIL;
	memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));
	for (ULONG i = 0; i < ulNum; i++)
	{
		// 判断图形合法后，将INNER图形拷贝到m_IAGraphic结构体中
		memcpy_s(&m_IAGraphic.iaGraphic, sizeof(IA_GRAPHICS), &pGraphics[i], sizeof(IA_GRAPHICS));
		m_IAGraphic.ulGraphicsID = m_ulGraphicID++;
		m_IAGraphic.bFinish      = TRUE;
		m_IAGraphic.bEditing	 = FALSE;
		m_IAGraphic.bPersent	 = bPersent;
		if (m_IAGraphic.iaGraphic.ulArrowType && !bPersent)
		{
			GetArrowPoints(m_IAGraphic.iaGraphic);
		}

		lRet = InsertCurrentToList();
	}
	
	return lRet;
}

// 判断设置的智能分析图形是否合法
BOOL CIADrawer::IsSetGraphicsInvalid(const IA_GRAPHICS *pGraphics, ULONG ulNum)
{
	BOOL bRet = FALSE;
	for (ULONG i = 0; i < ulNum; i++)
	{
		// 判断图形类型是否合法，图形类型编号到5
		bRet = 5 < pGraphics[i].ulGraphicType;
		CHECK_RESULT_TRUE_LOG(bRet, "Is Set Graphics Invalid", "GraphicType Invalid");

		// 设置的图形点数大于最大点数
		bRet = POINTS_NUM_MAX < pGraphics[i].ulPointsNum;
		CHECK_RESULT_TRUE_LOG(bRet, "Is Set Graphics Invalid", "PointsNum Invalid");

		// 找到图形边界矩形
		RECT rc = {0};
		bRet = !FindEdgeRectangle(pGraphics[i].IAPoints, pGraphics[i].ulPointsNum, rc);
		CHECK_RESULT_TRUE_LOG(bRet, "Is Set Graphics Invalid", "Find Edge Rectangle Failed");

		// 判断矩形是否超出最大分辨率范围
		bRet = (0 > rc.left) || (rc.right >= MAX_RESOLUTIONG_RATIO) 
				|| (0 > rc.top) || (rc.bottom >= MAX_RESOLUTIONG_RATIO);
		CHECK_RESULT_TRUE_LOG(bRet, "Is Set Graphics Invalid", "Graphic Outside the Window");

		// 判断设置进来的图形是否与已存在的图形有位置关系的非法重叠
		bRet = IsForbiddenOverlaped(pGraphics[i].IAPoints, pGraphics[i].ulPointsNum, 
				pGraphics[i].ulGraphicGroup, pGraphics[i].ulGraphicType);
		CHECK_RESULT_TRUE_LOG(bRet, "Is Set Graphics Invalid", "Graphic Illegally Overlaped");

		// 判断图形名称位置是否合法（只有0、1、2三种）
		bRet = (pGraphics[i].NamePosition >= GRAPHIC_NAME_OTHER);
		CHECK_RESULT_TRUE_LOG(bRet, "Is Set Graphics Invalid", "NamePosition Invalid");
	}
	return bRet;

}

// 设置智能分析图形组之间的关系
LONG CIADrawer::SetRelationShip(ULONG ulGraphicGroupA, ULONG ulGraphicGroupB, 
								RELATION_SHIP_TYPE ulRelationShipType)
{
	IVS_TRACE();
	// 获取图形位置关系的相反关系,以成对方式存放在关系列表中
	RELATION_SHIP_TYPE ulRelationShipType_Opposite = 
		GetOppositeType(ulRelationShipType);

	CAutoLock Lock(m_Mutex);
	BOOL bNoRecord = TRUE;
	IA_RELATIONS_LIST_ITER RelationIter    = m_IARelationShipList.begin();
	IA_RELATIONS_LIST_ITER RelationIterEnd = m_IARelationShipList.end();
		
	while (RelationIter != RelationIterEnd)
	{
		IA_RELATIONS *pRelationShip = *RelationIter;
		if (NULL == pRelationShip)
		{
			RelationIter = m_IARelationShipList.erase(RelationIter);
			continue;
		}
		RelationIter++;

		// 遍历关系列表,若存在相同的 当前GraphicGroup 和 相关GraphicGroup
		if ((ulGraphicGroupA == pRelationShip->ulCurrentGraphicGroup) 
			&& (ulGraphicGroupB == pRelationShip->ulRelativeGraphicGroup))
		{
			// 若表中对应的类型与新设置的相同,则直接返回
			if (ulRelationShipType == pRelationShip->ulRelationShipType)
			{
				IVS_LOG(IVS_LOG_INFO, "Set RelationShip", "Relation Exists");
				return IVS_SUCCEED;
			}
			// 表中对应类型与新设置的不同,则标识已有记录存在,更新位置关系类型
			bNoRecord = FALSE;
			pRelationShip->ulRelationShipType = ulRelationShipType;
			
			// 若该设置是图形组自身的位置关系,则不需要成对出现,不需要更新其另一对位置关系类型,直接返回 
			if (ulGraphicGroupA == ulGraphicGroupB)
			{
				IVS_LOG(IVS_LOG_INFO, "Set RelationShip", "Relation Updated");
				return IVS_SUCCEED;
			}
			break;
		}
	}
	// 列表中没有新设置的位置关系记录
	if (bNoRecord)
	{
		LONG lRet = IVS_FAIL;
		// 将设置的位置关系插入到位置关系列表中
		lRet = InsertIntoRelationShipList(ulGraphicGroupA, ulGraphicGroupB, 
											ulRelationShipType);	

		// 若不是图形自身的位置关系,还需要插入成对的位置关系节点
		if ((IVS_SUCCEED == lRet) && (ulGraphicGroupA != ulGraphicGroupB))
		{
			lRet = InsertIntoRelationShipList(ulGraphicGroupB, ulGraphicGroupA, 
												ulRelationShipType_Opposite);
		}
		// 无记录处理完，逻辑结束必须返回
		return lRet;
	}

	// 位置关系列表有记录、位置关系类型需要更新、并且不是设置图形组自身的位置关系，
	// 则还需要遍历列表找到与它成对的节点，修改其相应位置关系类型
	RelationIter = m_IARelationShipList.begin();
	while (RelationIter != RelationIterEnd)
	{
		IA_RELATIONS *pRelationShip = *RelationIter;
		if (NULL == pRelationShip)
		{
			RelationIter = m_IARelationShipList.erase(RelationIter);
			continue;
		}
		RelationIter++;
		if ((ulGraphicGroupB == pRelationShip->ulCurrentGraphicGroup) 
			&& (ulGraphicGroupA == pRelationShip->ulRelativeGraphicGroup))
		{
			pRelationShip->ulRelationShipType = ulRelationShipType_Opposite;
			IVS_LOG(IVS_LOG_INFO, "Set RelationShip", "Relation Updated");
			break;
		}
	}
	return IVS_SUCCEED;
}

// 获取相反的图形组关系
RELATION_SHIP_TYPE CIADrawer::GetOppositeType(RELATION_SHIP_TYPE ulRelationShipType) 
{
	RELATION_SHIP_TYPE RetValue = IA_GRAPHICS_RELATION_TYPE_NONE;
	switch (ulRelationShipType)
	{
	case IA_GRAPHICS_RELATION_TYPE_BE_COVERED:
		RetValue = IA_GRAPHICS_RELATION_TYPE_COVER;
		break;
	case IA_GRAPHICS_RELATION_TYPE_COVER:
		RetValue = IA_GRAPHICS_RELATION_TYPE_BE_COVERED;
		break;
	case IA_GRAPHICS_RELATION_TYPE_NOT_CROSS:
		RetValue = IA_GRAPHICS_RELATION_TYPE_NOT_CROSS;
		break;
	case IA_GRAPHICS_RELATION_TYPE_SMALLER:
		RetValue = IA_GRAPHICS_RELATION_TYPE_BIGGER;
		break;
	case IA_GRAPHICS_RELATION_TYPE_BIGGER:
		RetValue = IA_GRAPHICS_RELATION_TYPE_SMALLER;
		break;
	default: // 非法位置关系类型的返回IA_GRAPHICS_RELATION_TYPE_NONE
		break;
	}
	return RetValue;
}

// 将位置关系插入到位置关系列表中
LONG CIADrawer::InsertIntoRelationShipList(ULONG ulGraphicGroupA, ULONG ulGraphicGroupB, RELATION_SHIP_TYPE ulRelationShipType) 
{
	IA_RELATIONS *pRelationShip = NULL;
	if (NULL == IVS_NEW(pRelationShip))
	{
		IVS_LOG(IVS_LOG_ERR, "Set RelationShip", "No Enough Memory");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(pRelationShip, 0, sizeof(IA_RELATIONS));
	pRelationShip->ulCurrentGraphicGroup  = ulGraphicGroupA;
	pRelationShip->ulRelativeGraphicGroup = ulGraphicGroupB;
	pRelationShip->ulRelationShipType	  = ulRelationShipType;
	m_IARelationShipList.push_back(pRelationShip);
	pRelationShip = NULL;
	return IVS_SUCCEED;
}

// 获取智能分析图形（pulNum作为入参为可获取的最大图形数，作为出参为获取到的图形数）
LONG CIADrawer::GetIAGraphics(ULONG ulGraphicGroup, IA_GRAPHICS *pGraphics, ULONG *pulNum)
{
	IVS_TRACE();
	if (NULL == pGraphics || (NULL == pulNum))
	{
		IVS_LOG(IVS_LOG_ERR, "Get IA Graphics", "Invalid Argument");
		return IVS_PARA_INVALID;
	}
    CAutoLock Lock(m_Mutex);
	if (m_IADrawGraphicsList.empty())
	{
		*pulNum = 0;
		IVS_LOG(IVS_LOG_INFO, "Get IA Graphics", "No Graphics");
		return IVS_SUCCEED;
	}

	ULONG ulNum = 0; 
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while (iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}
		if (((ulGraphicGroup == pIAGraphic->iaGraphic.ulGraphicGroup) // 获取对应的GraphicGroup图形
			|| !ulGraphicGroup)			// 当GraphicGroup为0时，获取全部图形
			&& (ulNum < *pulNum))		// 当GraphicGroup的图形数量大于可接受图形数时，不再获取
		{
			memcpy_s(&pGraphics[ulNum++], sizeof(IA_GRAPHICS), &pIAGraphic->iaGraphic, sizeof(IA_GRAPHICS));
		}
		iter++;
	}
	*pulNum = ulNum;
	return IVS_SUCCEED;
}

// 获取选中的图形
LONG CIADrawer::GetIAGraphicSelected(IA_GRAPHICS *pGraphic)
{
	IVS_TRACE();
	if (NULL == pGraphic)
	{
		IVS_LOG(IVS_LOG_ERR, "Get IA Graphics Selected", "Invalid Argument");
		return IVS_PARA_INVALID;
	}
	CAutoLock Lock(m_Mutex);
	if (m_IADrawGraphicsList.empty())
	{
		IVS_LOG(IVS_LOG_INFO, "Get IA Graphics Selected", "No Graphics");
		return IVS_SUCCEED;
	}
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while (iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}
		if (pIAGraphic->bEditing) // 获取正在编辑图形
		{
			memcpy_s(pGraphic, sizeof(IA_GRAPHICS), &pIAGraphic->iaGraphic, sizeof(IA_GRAPHICS));
			return IVS_SUCCEED;
		}
		iter++;
	}
	IVS_LOG(IVS_LOG_INFO, "Get IA Graphics Selected", "No Selected Graphics");
	return IVS_SUCCEED;
}


// 设置开始画图的起始状态
LONG CIADrawer::SetIAStartStatus(ULONG ulNum, const IA_GRAPHICS *pIAGraphics)
{
	IVS_TRACE();
	CAutoLock Lock(m_Mutex);
	if (m_IADrawGraphicsList.size() >= m_ulMaxDrawNum)
	{
		IVS_LOG(IVS_LOG_ERR, "Set IA Start Position", 
			"%u Reach MAX Graphics Num", m_ulMaxDrawNum);
		return IVS_FAIL;
	}
	
	// 将当前图形组最大画图数记录到 m_ulMaxGroupShapeNum
	m_ulMaxGroupShapeNum = ulNum; 
	// 若当前图形组已经到达最大画图数，返回错误
	if (IsFinishDrawGraphicGroup(pIAGraphics->ulGraphicGroup, ulNum))
	{
		IVS_LOG(IVS_LOG_INFO, "Set IA Start Position", "Reach Max Draw Number");
		return IVS_FAIL;
	}

	// 判断起始点是否在非法区域内
	if (IsInForbiddenArea(pIAGraphics->IAPoints[0], pIAGraphics->ulGraphicGroup))
	{
		IVS_LOG(IVS_LOG_ERR, "Set IA Start Position", "Point Invalid");
		return IVS_FAIL;
	}

	memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));
	memcpy_s(&m_IAGraphic.iaGraphic, sizeof(IA_GRAPHICS), pIAGraphics, sizeof(IA_GRAPHICS));
	m_IAGraphic.iaGraphic.ulPointsNum = 1;
	// 第二、第三个点同样初始化为第一个点坐标，避免直线、矩形绘出到（0,0）点的图形
	m_IAGraphic.iaGraphic.IAPoints[1] = pIAGraphics->IAPoints[0];
	m_IAGraphic.iaGraphic.IAPoints[2] = pIAGraphics->IAPoints[0];
	m_IAGraphic.ulGraphicsID	= m_ulGraphicID++;
	m_IAGraphic.bFinish			= FALSE;
	m_IAGraphic.bEditing		= TRUE;
	m_bActive					= TRUE;

	return IVS_SUCCEED;
}

// 智能分析画直线
LONG CIADrawer::DrawIAStraightLine(POINT point, BOOL bFinish, BOOL bIsDrawing)
{
	// 在开始画图时已经激活过m_IAGraphic，否则说明开始画图失败
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA Straight Line", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	// 若当前鼠标点与起始点接近，则画图失败返回
	if (IsCloseToPoint(point, m_IAGraphic.iaGraphic.IAPoints[0]))
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA Straight Line", "Invalid Point");
		return IVS_PARA_INVALID;
	}
	if (m_IAGraphic.iaGraphic.bKeepHorizon)
	{
		point.y = m_IAGraphic.iaGraphic.IAPoints[0].y;
	}
	m_IAGraphic.iaGraphic.IAPoints[1] = point;
	m_IAGraphic.iaGraphic.ulPointsNum = 2;
	m_IAGraphic.bFinish				  = bFinish;

	if (m_IAGraphic.iaGraphic.ulArrowType)
	{
		GetArrowPoints(m_IAGraphic.iaGraphic);
	}

	// 判断是否完成画图，插入图形列表，是否通知回调	
	return FinishAndInsertIntoGraphicList(bIsDrawing);
}

// 计算箭头坐标
VOID CIADrawer::GetArrowPoints(IA_GRAPHICS &iaGraphic)
{
	IA_ARROW iaArrow;
	memset(&iaArrow, 0, sizeof(iaArrow));
	
	LONG dStraightLineWidth  = iaGraphic.IAPoints[1].x - iaGraphic.IAPoints[0].x;
	LONG dStraightLineHeight = iaGraphic.IAPoints[1].y - iaGraphic.IAPoints[0].y;
	DOUBLE dLenth = (DOUBLE)dStraightLineWidth * (DOUBLE)dStraightLineWidth 
					+ (DOUBLE)dStraightLineHeight * (DOUBLE)dStraightLineHeight;
	LONG lStraightLineLenth  = (LONG)sqrt(dLenth);

	for (unsigned int i = 1; i <= 2; i++)
	{
		if (i & iaGraphic.ulArrowType)
		{
			iaArrow.ArrowTail[i - 1].x = iaGraphic.IAPoints[0].x + dStraightLineWidth  / 2;
			iaArrow.ArrowTail[i - 1].y = iaGraphic.IAPoints[0].y + dStraightLineHeight / 2;

			LONG dArrowWidth  = (LONG)pow((DOUBLE)-1, (DOUBLE)(i - 1)) * dStraightLineHeight * m_lArrowLenth / lStraightLineLenth;
			LONG dArrowHeight = (LONG)pow((DOUBLE)-1, (DOUBLE)(i))     * dStraightLineWidth  * m_lArrowLenth / lStraightLineLenth;

			iaArrow.ArrowHead[i - 1].x = iaArrow.ArrowTail[i - 1].x + dArrowWidth;
			iaArrow.ArrowHead[i - 1].y = iaArrow.ArrowTail[i - 1].y + dArrowHeight;

			POINT FourFifthOnArrow = {0, 0};
			FourFifthOnArrow.x = iaArrow.ArrowTail[i - 1].x + dArrowWidth  * 4 / 5;
			FourFifthOnArrow.y = iaArrow.ArrowTail[i - 1].y + dArrowHeight * 4 / 5;

			LONG dArrowHeadWidth  = - dArrowHeight / 5;
			LONG dArrowHeadHeight =   dArrowWidth  / 5;

			iaArrow.ArrowTrangle1[i - 1].x = FourFifthOnArrow.x + dArrowHeadWidth;
			iaArrow.ArrowTrangle1[i - 1].y = FourFifthOnArrow.y + dArrowHeadHeight;

			iaArrow.ArrowTrangle2[i - 1].x = FourFifthOnArrow.x - dArrowHeadWidth;
			iaArrow.ArrowTrangle2[i - 1].y = FourFifthOnArrow.y - dArrowHeadHeight;

		}
	}
	memcpy_s(&iaGraphic.IAArrow, sizeof(IA_ARROW), &iaArrow, sizeof(IA_ARROW));

	return;
}

// 智能分析画矩形
LONG CIADrawer::DrawIARectangle(POINT point, BOOL bFinish, BOOL bIsDrawing)
{
	// 在开始画图时已经激活过m_IAGraphic，否则说明开始画图失败
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA Rectangle", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	// 若当前鼠标点与起始点接近，则画图失败返回
	if (IsCloseToPoint(point, m_IAGraphic.iaGraphic.IAPoints[0]) 
		|| point.x == m_IAGraphic.iaGraphic.IAPoints[0].x 
		|| point.y == m_IAGraphic.iaGraphic.IAPoints[0].y)
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA Rectangle", "Invalid Point");
		return IVS_PARA_INVALID;
	}
	if (IVS_SUCCEED != AdjustRectPoint(point))
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA Rectangle", "Adjust Rect Point Failed");
		return IVS_FAIL;
	}

	// 判断合法前用临时变量记录各个坐标值
	POINT tmp_point[5];
	tmp_point[0]   = m_IAGraphic.iaGraphic.IAPoints[0];
	tmp_point[1].x = point.x;
	tmp_point[1].y = m_IAGraphic.iaGraphic.IAPoints[0].y;
	tmp_point[2]   = point;
	tmp_point[3].x = m_IAGraphic.iaGraphic.IAPoints[0].x;
	tmp_point[3].y = point.y;
	tmp_point[4]   = m_IAGraphic.iaGraphic.IAPoints[0];
	
	// 判断是否与已有的图形位置关系非法重叠
	if (IsForbiddenOverlaped(tmp_point, 5, m_IAGraphic.iaGraphic.ulGraphicGroup, 
							m_IAGraphic.iaGraphic.ulGraphicType)
		|| IsRectangleSizeIllegal(tmp_point, 5, m_IAGraphic.iaGraphic.ulGraphicGroup))
	{
		if (bFinish && bIsDrawing)
		{
			m_bActive = FALSE;
			memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));
		}
		IVS_LOG(IVS_LOG_ERR, "Draw IA Rectangle", "Point Illegal");
		return IVS_FAIL;
	}
	memcpy_s(m_IAGraphic.iaGraphic.IAPoints, 
		sizeof(POINT) * POINTS_NUM_MAX, 
		tmp_point, sizeof(POINT) * 5);
	m_IAGraphic.iaGraphic.ulPointsNum   = 5;
	m_IAGraphic.bFinish				   = bFinish;

	// 判断是否完成画图，插入图形列表，是否通知回调
	return FinishAndInsertIntoGraphicList(bIsDrawing);
}

LONG CIADrawer::AdjustRectPoint(POINT &point)
{
	if (!m_IAGraphic.iaGraphic.bKeepRectShape)
	{
		return IVS_SUCCEED;
	}
	if (0 == m_ulDCWidth || 0 == m_ulDCHeight)
	{
		IVS_LOG(IVS_LOG_ERR, "Adjust Rect Point", "DC Size is Zero");
		return IVS_FAIL;
	}
	POINT tmpPoint = point;
	LONG lWidth  = point.x - m_IAGraphic.iaGraphic.IAPoints[0].x;
	LONG lHeight = point.y - m_IAGraphic.iaGraphic.IAPoints[0].y;
	LONG tag = (LONG)m_ulDCWidth * abs(lHeight) - (LONG)m_ulDCHeight * abs(lWidth);
	if (0 < tag)
	{
		lWidth = (abs(lHeight) * (LONG)m_ulDCWidth / (LONG)m_ulDCHeight) 
			* (lWidth / abs(lWidth));
		tmpPoint.x = m_IAGraphic.iaGraphic.IAPoints[0].x + lWidth;
	}
	if (0 > tag)
	{
		lHeight = (abs(lWidth) * (LONG)m_ulDCHeight / (LONG)m_ulDCWidth) 
			* (lHeight / abs(lHeight));
		tmpPoint.y = m_IAGraphic.iaGraphic.IAPoints[0].y + lHeight;
	}
	if (!IsPointInside(&tmpPoint))
	{
		IVS_LOG(IVS_LOG_ERR, "Adjust Rect Point", "Point Illegal");
		return IVS_FAIL;
	}
	point.x = tmpPoint.x;
	point.y = tmpPoint.y;
	return IVS_SUCCEED;
}

// 智能分析画多边形
LONG CIADrawer::DrawIAPolygon(POINT point, BOOL bNextPoint, BOOL bFinish, ULONG ulGraphicGroup)
{
	// 在开始画图时已经激活过m_IAGraphic，否则说明开始画图失败
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA Polygon", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}

	// 判断所画多边形是否合法
	if (IVS_SUCCEED != DrawIAPolygonShape(point, bNextPoint, bFinish, ulGraphicGroup))
	{
		if (bFinish)
		{
			m_bActive = FALSE;
			memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));
		}
		IVS_LOG(IVS_LOG_ERR, "Draw IA Polygon", "Draw Polygon Error");
		return IVS_FAIL;
	}

	// 判断是否完成画图，插入图形列表，是否通知回调
	return FinishAndInsertIntoGraphicList(TRUE);
}

// 智能分析画U型线
LONG CIADrawer::DrawIAULine(POINT point, BOOL bNextPoint, BOOL bFinish, BOOL bIsDrawing)
{
	// 在开始画图时已经激活过m_IAGraphic，否则说明开始画图失败
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA ULine", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}

	// 若结束时U型线点数不正确,则删除当前图形,直接返回
	if (bFinish && (POINT_NUM_OF_ULINE != m_IAGraphic.iaGraphic.ulPointsNum))
	{
		m_bActive = FALSE;
		memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));
		IVS_LOG(IVS_LOG_ERR, "Draw IA ULine", "PointsNum Illegal");
		return IVS_PARA_INVALID;
	}

	// 判断所画U型线是否合法
	if (IVS_SUCCEED != DrawIAULineShape(point, bNextPoint, bIsDrawing))
	{
		if (bFinish)
		{
			m_bActive = FALSE;
			memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));
		}
		IVS_LOG(IVS_LOG_ERR, "Draw IA ULine", "Draw ULine Error");
		return IVS_FAIL;
	}
	// 判断是否完成画图，插入图形列表，是否通知回调
	return FinishAndInsertIntoGraphicList(bIsDrawing);
}

// 智能分析结束某图形组的画图（若存在正在画的该组图形，直接丢弃）
LONG CIADrawer::StopDrawGraphics(ULONG ulGraphicGroup)
{
	IVS_TRACE();
	CAutoLock Lock(m_Mutex);

	if (m_bActive)
	{
		// 输入的GraphicGroup与当前不一致，返回失败
		if (ulGraphicGroup != m_IAGraphic.iaGraphic.ulGraphicGroup)
		{
			IVS_LOG(IVS_LOG_INFO, "Stop Draw Graphics", "GraphicGroup Not Match");
			return IVS_FAIL;
		}
		m_bActive = FALSE;
		memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));
	}

	(void)SelectCancel();

	return IVS_SUCCEED;
}


// 显示智能分析图形
LONG CIADrawer::ShowIAGraphics(HDC hDC, const RECT *rc)
{
// 	IVS_TRACE();

	// 更新画图窗口
	LONG lRet = UpdateWnd(hDC, rc);
	if (IVS_SUCCEED != lRet)
	{
		IVS_LOG(IVS_LOG_ERR, "Show IA Graphics", "Update Window Failed");
		return lRet;
	}

	CAutoLock Lock(m_Mutex);

	// 显示图形列表中所有图形
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while (iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}

		if (pIAGraphic->bPersent)
		{
			for (ULONG i = 0; i < pIAGraphic->iaGraphic.ulPointsNum; i++)
			{
				pIAGraphic->iaGraphic.IAPoints[i].x 
					= (LONG)(((float)pIAGraphic->iaGraphic.IAPoints[i].x 
						/ FLOAT_TO_LONG_COEFF) * m_ulDCWidth);
				pIAGraphic->iaGraphic.IAPoints[i].y 
					= (LONG)(((float)pIAGraphic->iaGraphic.IAPoints[i].y 
						/ FLOAT_TO_LONG_COEFF) * m_ulDCHeight);
			}
			pIAGraphic->bPersent = FALSE;
		}
		if (pIAGraphic->iaGraphic.ulArrowType)
		{
			GetArrowPoints(pIAGraphic->iaGraphic);
		}
		// 判断图形是否在图像外部
		if (IsShowIAShapeInvalid(*pIAGraphic))
		{
			IVS_DELETE(pIAGraphic);
			iter = m_IADrawGraphicsList.erase(iter);
			IVS_LOG(IVS_LOG_ERR, "Show IA Graphics", "Find Invalid Image and Deleted");
			continue;
		}
		ShowIAShape(hDC, *pIAGraphic);
		iter++;
	}

	// 显示正在画的图形
	if (m_bActive)
	{
		if (m_bWindowPoint)
		{
			lRet = IVS_FAIL;
			lRet = WindowPointToGraphicPoint(&m_IAGraphic.iaGraphic.IAPoints[0]);
			lRet = WindowPointToGraphicPoint(&m_IAGraphic.iaGraphic.IAPoints[1]);
			lRet = WindowPointToGraphicPoint(&m_IAGraphic.iaGraphic.IAPoints[2]);

			if (IVS_SUCCEED != lRet)
			{
				IVS_LOG(IVS_LOG_ERR, "Show IA Graphics", "Current Graphic StartPoint Invalid");
				memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));
				m_bActive = FALSE;
				m_bWindowPoint = FALSE;
				return IVS_FAIL;
			}
			m_bWindowPoint = FALSE;
		}
		ShowIAShape(hDC, m_IAGraphic);
	}
	return IVS_SUCCEED;
}


LONG CIADrawer::UpdateWnd(HDC hDC, const RECT *rc) 
{
	// 更新窗口大小
	RECT Wndrect;
    memset(&Wndrect, 0, sizeof(Wndrect));

	if (GetClientRect(m_hMsgWnd, &Wndrect))
	{	
		m_ulWndWidth  = (ULONG)(Wndrect.right  - Wndrect.left);		// 窗口宽度
		m_ulWndHeight = (ULONG)(Wndrect.bottom - Wndrect.top);		// 窗口高度
	}
	// 判断窗口长宽是否为0
	if (( 0 == m_ulWndWidth) || (0 == m_ulWndHeight))
	{
		IVS_LOG(IVS_LOG_ERR, "Update Window", "hMsgWnd Size 0");
		return IVS_FAIL;
	}

	// 更新图像大小(DC尺寸)
	ULONG DCWidth  = (ULONG)(rc->right - rc->left);
	ULONG DCHeight = (ULONG)(rc->bottom - rc->top);
	// 判断DC长宽是否为0
	if (( 0 == DCWidth) || (0 == DCHeight))
	{
		IVS_LOG(IVS_LOG_ERR, "Update Window", "hDC Size 0");
		return IVS_FAIL;
	}
	if ((DCWidth != m_ulDCWidth) || (DCHeight != m_ulDCHeight))
	{
		m_ulDCWidth  = DCWidth;
		m_ulDCHeight = DCHeight;

		if (IVS_SUCCEED != CreatBitmapDC(hDC))
		{
			return IVS_FAIL;
		}
		IVS_LOG(IVS_LOG_INFO, "Update Window", "Image Size Changed");
	}
	m_bWindowPoint = FALSE;

	GetOffset();
	m_lDistanceCloseToLine = (LONG)max(2, CLOSE_TO_LINE_DISTANCE_ON_WND * m_fDiv);
	m_lArrowLenth		   = (LONG)max(10, ARROW_LENTH * m_fDiv);
	LONG lFontsize		   = (LONG)max(12, FONT_SIZE * m_fDiv);
	if (lFontsize != m_lFontSize)
	{
		IVS_LOG(IVS_LOG_INFO, "Update Window", "Font Size Changed: %d to %d", 
			m_lFontSize, lFontsize);
		m_lFontSize = lFontsize;
		CreateIAFont();
	}

	return IVS_SUCCEED;
}

void CIADrawer::GetOffset() 
{
	if ((0 == m_ulWndWidth) || (0 == m_ulWndHeight))
	{
		IVS_LOG(IVS_LOG_ERR, "Get Offset", "Window Size is Zero");
		return;
	}
	if ((0 == m_ulDCWidth) || (0 == m_ulDCHeight))
	{
		IVS_LOG(IVS_LOG_INFO, "Get Offset", "DC Size is Zero");
		return;
	}
	m_lOffsetX = 0;
	m_lOffsetY = 0;

	if (!m_bScaleModeFull)
	{	
		LONG tag = (LONG)(m_ulDCWidth * m_ulWndHeight) - (LONG)(m_ulDCHeight * m_ulWndWidth);
		if (0 < tag)
		{
			m_lOffsetX = 0;
			m_lOffsetY = (LONG)(m_ulWndHeight - m_ulDCHeight * m_ulWndWidth / m_ulDCWidth) / 2;
		}
		if (0 > tag)
		{
			m_lOffsetX = (LONG)(m_ulWndWidth - m_ulDCWidth * m_ulWndHeight / m_ulDCHeight) / 2;
			m_lOffsetY = 0;
		}
	}
	float fxDiv = (float)m_ulDCWidth / ((float)m_ulWndWidth - (float)m_lOffsetX * 2);
	float fyDiv = (float)m_ulDCHeight / ((float)m_ulWndHeight - (float)m_lOffsetY * 2);
	float fDiv  = fxDiv < fyDiv ? fyDiv : fxDiv;
	if (0 != fDiv - m_fDiv)
	{
		m_fDiv = fDiv;
		IVS_LOG(IVS_LOG_DEBUG, "Get Offset", "xDiv: %f, yDiv %f, Div: %f", fxDiv, fyDiv, m_fDiv);
	}
}



BOOL CIADrawer::IsShowIAShapeInvalid(const IA_INNER_GRAPHICS &pIAGraphic)
{
	BOOL bRet = FALSE;

	RECT rc = {0};
	bRet = !FindEdgeRectangle(pIAGraphic.iaGraphic.IAPoints, pIAGraphic.iaGraphic.ulPointsNum, rc);
	CHECK_RESULT_TRUE_LOG(bRet, "Is Show IA Shape Invalid", "Find Edge Rectangle Failed");

	// 判断图形边界矩形是否超出目前分辨率范围
	bRet = (0 > rc.left) || (rc.right > (LONG)m_ulDCWidth) 
		|| (0 > rc.top) || (rc.bottom > (LONG)m_ulDCHeight);
	CHECK_RESULT_TRUE_LOG(bRet, "Is Show IA Shape Invalid", "Graphic Outside the Window");

	return bRet;
}



// 删除所有选中图形
LONG CIADrawer::DeleteIAGraphicsSelected()
{
	IVS_TRACE();

	CAutoLock Lock(m_Mutex);
	// 遍历图形列表,删除当前GraphicGroup的所有图形
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while (iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}

		if (pIAGraphic->bEditing && 
			pIAGraphic->iaGraphic.bAllowDelete)
		{
			IVS_DELETE(pIAGraphic);
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}
		iter++;
	}

	return IVS_SUCCEED;
}


// 根据鼠标点坐标删除智能分析单个图形
//（若坐标在指定图形组的单个图形内部，删除该图形；若在多个图形重叠区域，则不删除）
LONG CIADrawer::DeleteIAGraphicsByPoint(ULONG ulGraphicGroup, POINT point)
{
	IVS_TRACE();
	CAutoLock Lock(m_Mutex);
	
	int count = 0;
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER delIter = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	IA_INNER_GRAPHICS *pIAGraphic = NULL;
	// 遍历图形列表,当找到符合条件的图形时,用delIter记录节点
	while (iter != iterEnd)
	{
		pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}

		if (IsGraphicToDelete(pIAGraphic, point) 
			&& pIAGraphic->iaGraphic.bAllowDelete 
			&& (ulGraphicGroup == pIAGraphic->iaGraphic.ulGraphicGroup))
		{
			delIter = iter;
			count++;	// 记录符合条件的图形个数
		}
		iter++;
	}

	// 当且仅当只选中一个图形时才删除,若点在多个图形重叠区域则不作任何操作
	if (1 == count) 		
	{
		pIAGraphic = *delIter;
		IVS_DELETE(pIAGraphic);
		m_IADrawGraphicsList.erase(delIter);
		return IVS_SUCCEED;
	}	
	IVS_LOG(IVS_LOG_ERR, "Delete IA Graphics", "No Graphics Choosen" );
	return IVS_FAIL;
}

// 判断根据该点是否可以删除该图形
BOOL CIADrawer::IsGraphicToDelete(const IA_INNER_GRAPHICS *pIAGraphics, POINT point) 
{
	BOOL bFind = FALSE;
	LONG dist = MAX_DISTANCE_VAL;
	LONG distcmp = MAX_DISTANCE_VAL;
	switch (pIAGraphics->iaGraphic.ulGraphicType)
	{
	case IA_GRAPHICS_TYPE_STRAIGHT_LINE: // 点在直线上为选中
		dist = DistanceToLine(point, pIAGraphics->iaGraphic.IAPoints[0],
								pIAGraphics->iaGraphic.IAPoints[1]);
		if (CLOSE_TO_LINE_DISTANCE > dist)
		{
			bFind = TRUE;
		}
		break;
	case IA_GRAPHICS_TYPE_UP_ULINE: // 点在U型线任意一段上为选中
	case IA_GRAPHICS_TYPE_DOWN_ULINE: // 点在U型线任意一段上为选中
		dist    = DistanceToLine(point, pIAGraphics->iaGraphic.IAPoints[0], 
								  pIAGraphics->iaGraphic.IAPoints[1]); 
		distcmp = DistanceToLine(point, pIAGraphics->iaGraphic.IAPoints[1], 
								  pIAGraphics->iaGraphic.IAPoints[2]);
		dist = min(dist, distcmp);
		distcmp = DistanceToLine(point, pIAGraphics->iaGraphic.IAPoints[2], 
								  pIAGraphics->iaGraphic.IAPoints[3]);
		dist = min(dist, distcmp);
		if (CLOSE_TO_LINE_DISTANCE > dist)
		{
			bFind = TRUE;
		}
		break;
	default: // 点在矩形和多边形内为选中
		if (IsInShape(pIAGraphics, point))
		{
			bFind = TRUE;
		}
		break;
	}	
	return bFind;
}


// 清除指定图形组的所有图形
LONG CIADrawer::DeleteGraphicsByGroup(ULONG ulGraphicGroup)
{
	IVS_TRACE();
	CAutoLock Lock(m_Mutex);

	// 遍历图形列表,删除当前被选中的所有图形
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while (iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}

		if (pIAGraphic->iaGraphic.ulGraphicGroup == ulGraphicGroup)
		{
			IVS_DELETE(pIAGraphic);
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}
		iter++;
	}

	return IVS_SUCCEED;
}

// 清除智能分析所有图形
LONG CIADrawer::DeleteIAGraphicsAll()
{
	IVS_TRACE();
	CAutoLock Lock(m_Mutex);

	m_bActive = FALSE;
	memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));

	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while (iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL != pIAGraphic)
		{
			IVS_DELETE(pIAGraphic);
		}
		iter = m_IADrawGraphicsList.erase(iter);
	}
	m_ulGraphicID = 0;
	return IVS_SUCCEED;
}


// 根据鼠标点坐标选中智能分析单个图形（选择特定组中与鼠标坐标最近的单个图形）
LONG CIADrawer::SelectGraphicByPoint(POINT Point, ULONG &ulGraphicGroup, BOOL &bAllowDel)
{
	IA_INNER_GRAPHICS *pGraphMatch = NULL;
	LONG MatchDist = MAX_DISTANCE_VAL;
	CAutoLock Lock(m_Mutex);
	IA_GRAPHICS_LIST_ITER iter = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd  = m_IADrawGraphicsList.end();
	while (iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}
		iter++;
		pIAGraphic->bEditing = FALSE;
#if 1
		// 非该GraphicGroup的图形直接跳过
		if ((ulGraphicGroup != pIAGraphic->iaGraphic.ulGraphicGroup) 
            && (ulGraphicGroup))
		{
			continue;
		}
#endif	
		LONG  distMin  = MAX_DISTANCE_VAL;
		LONG  dist     = 0;
		ULONG i        = 0;
		if (pIAGraphic->iaGraphic.ulPointsNum < 2)
		{
			IVS_LOG(IVS_LOG_ERR, "Select Graphic By Point", "PointNum Invalid");
			return IVS_FAIL;
		}
		// 找到点距离单个图形各个边的最小距离（ulPointsNum > 2 时进入）
		for (i = 0; i < pIAGraphic->iaGraphic.ulPointsNum - 2; i++)
		{
			LONG distToShape = DistanceToLine(Point, pIAGraphic->iaGraphic.IAPoints[i], 
										 pIAGraphic->iaGraphic.IAPoints[i + 1]);
			// 若距离为0，直接标记该图形为选中，退出
			if (0 == distToShape) 
			{
				pIAGraphic->bEditing = TRUE;
                ulGraphicGroup = pIAGraphic->iaGraphic.ulGraphicGroup;
				bAllowDel = pIAGraphic->iaGraphic.bAllowDelete;
				return IVS_SUCCEED;
			}
			if (distToShape <= distMin)
			{
				distMin = distToShape;
			}
		}

		// 直线情况（ulPointsNum == 2）
		dist = DistanceToLine(Point, pIAGraphic->iaGraphic.IAPoints[i], 
							  pIAGraphic->iaGraphic.IAPoints[i + 1]);
		// 若距离为0，直接标记该图形为选中，退出
		if (0 == dist)
		{
			pIAGraphic->bEditing = TRUE;
            ulGraphicGroup = pIAGraphic->iaGraphic.ulGraphicGroup;
			bAllowDel = pIAGraphic->iaGraphic.bAllowDelete;
			return IVS_SUCCEED;
		}
		if (dist <= distMin)
		{
			distMin = dist;
		}

		// 若距离为目前最小，则记录最小距离和当前图形
		if (distMin < MatchDist)
		{
			MatchDist = distMin;
			pGraphMatch = pIAGraphic;
		}
	}

	// 将与点point距离最小的图形置为编辑状态（注释中要求点在图形线附近）
	if (pGraphMatch && MatchDist < CLOSE_TO_LINE_DISTANCE)
	{
		pGraphMatch->bEditing = TRUE;
        ulGraphicGroup = pGraphMatch->iaGraphic.ulGraphicGroup;
		bAllowDel = pGraphMatch->iaGraphic.bAllowDelete;
		return IVS_SUCCEED;
	}

	// 没有选中任何图形
	return IVS_FAIL;
}

LONG CIADrawer::GetCursorType(POINT Point)
{
	CAutoLock Lock(m_Mutex);
	BOOL bFind = FALSE;
	LONG lCursorType = CURSOR_TYPE_ARROW;
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while (iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}
		if (pIAGraphic->bEditing) // 获取正在编辑图形
		{
			lCursorType = GetCursorTypeInShape(Point, pIAGraphic->iaGraphic);
			bFind = TRUE;
			break;
		}
		iter++;
	}
	if (!bFind && m_bActive)
	{
		lCursorType = GetCursorTypeInShape(Point, m_IAGraphic.iaGraphic);
	}
	return lCursorType;
}

LONG CIADrawer::GetCursorTypeInShape(POINT Point, const IA_GRAPHICS &IAGraphic)
{
	// 鼠标在点上
	for(ULONG i = 0; i < IAGraphic.ulPointsNum; i++)
	{
		if (IsCloseToPoint(Point, IAGraphic.IAPoints[i]))
		{
			return GetCursorTypePoint(Point, IAGraphic, i);
		}
	}

	// 鼠标在线上
	ULONG ulPointsNum = IAGraphic.ulPointsNum;
	if (IA_GRAPHICS_TYPE_RECTANGLE != IAGraphic.ulGraphicType 
		&& IA_GRAPHICS_TYPE_POLYGON != IAGraphic.ulGraphicType)
	{
		ulPointsNum--;
	}
	for(ULONG i = 0; i < ulPointsNum; i++)
	{
		if (CLOSE_TO_LINE_DISTANCE > 
			DistanceToLine(Point, IAGraphic.IAPoints[i], IAGraphic.IAPoints[i + 1]))
		{
			return CURSOR_TYPE_SIZEALL;
		}
	}

	return CURSOR_TYPE_ARROW;
}

LONG CIADrawer::GetCursorTypePoint(POINT /*Point*/, const IA_GRAPHICS &IAGraphic, ULONG ulIndex)
{
	ULONG ulPointsNum = IAGraphic.ulPointsNum;
	if (IA_GRAPHICS_TYPE_RECTANGLE == IAGraphic.ulGraphicType 
		|| IA_GRAPHICS_TYPE_POLYGON == IAGraphic.ulGraphicType)
	{
		ulPointsNum--;
	}
	if (ulPointsNum == 0)
	{
		return CURSOR_TYPE_ARROW;
	}
	ULONG ulIndexPre =  (ulIndex + ulPointsNum - 1) % ulPointsNum;
	ULONG ulIndexNext = (ulIndex + ulPointsNum + 1) % ulPointsNum;
	LONG lCursorType = GetCursorTypePointInner(IAGraphic.IAPoints[ulIndex], 
		IAGraphic.IAPoints[ulIndexPre], IAGraphic.IAPoints[ulIndexNext]);
	return lCursorType;
} 

LONG CIADrawer::GetCursorTypePointInner(POINT Point, POINT PointPre, POINT PointNext)
{
	LONG lCursorType = CURSOR_TYPE_ARROW;

	LONG lWidthPre   = Point.x - PointPre.x;
	LONG lHeightPre  = Point.y - PointPre.y;
	LONG lWidthNext  = Point.x - PointNext.x;
	LONG lHeightNext = Point.y - PointNext.y;
	LONG lPre        = lWidthPre  * lHeightPre;
	LONG lNext       = lWidthNext * lHeightNext;
	LONG lAll        = lPre * lNext;
	if (0 <= lAll)
	{
		if (0 < lPre)
		{
			lCursorType = CURSOR_TYPE_SIZENESW; 
		} 
		else if (0 > lPre)
		{
			lCursorType = CURSOR_TYPE_SIZENWSE;
		}
		else
		{
			if (0 < (lWidthPre ^ lHeightNext) || 0 < (lHeightPre ^ lWidthNext))
			{
				lCursorType = CURSOR_TYPE_SIZENWSE;
			}
			else
			{
				lCursorType = CURSOR_TYPE_SIZENESW; 
			}
		}
	} else {
		if (0 <= lHeightPre * lHeightNext)
		{
			lCursorType = CURSOR_TYPE_SIZENS; 
		} else {
			lCursorType = CURSOR_TYPE_SIZEWE;
		}
	}
	return lCursorType;
}


// 选中指定图形组的所有图形
LONG CIADrawer::SelectGraphicByGroup(ULONG ulGraphicGroup, BOOL &bAllowDel)
{
	IVS_TRACE();

	CAutoLock Lock(m_Mutex);
	if (m_IADrawGraphicsList.empty())
	{
		IVS_LOG(IVS_LOG_INFO, "Select Graphic By Group", "No Graphics");
		return IVS_SUCCEED;
	}

	// 遍历图形列表,找出所有该GraphicGroup的图形,置为编辑状态
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	UINT uiCount = 0;
	while (iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}

		if (pIAGraphic->iaGraphic.ulGraphicGroup == ulGraphicGroup)
		{
			if (!uiCount)
			{
				pIAGraphic->bEditing = TRUE;
				bAllowDel = pIAGraphic->iaGraphic.bAllowDelete;
			}
			uiCount++;
		}
		else
		{
			pIAGraphic->bEditing = FALSE;
		}
		iter++;
	}
	return IVS_SUCCEED;
}

// 智能分析开始修改图形
LONG CIADrawer::StartModifyIAGraphics(POINT point, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup)
{
	IVS_TRACE();
	LONG lRet  = IVS_FAIL;	
	CAutoLock Lock(m_Mutex);	
	if (m_IADrawGraphicsList.empty())
	{
		IVS_LOG(IVS_LOG_ERR, "Start Modify IA Graphics", "No Graphics");
		return IVS_FAIL;
	}
	
	// 遍历图形列表,找到当前在编辑状态的图形	
	BOOL bFind = FALSE;
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	IA_INNER_GRAPHICS *pIAGraphic = NULL;
	while (iter != iterEnd)
	{
		pIAGraphic = *iter;	
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}

		if (pIAGraphic->bEditing)
		{
			bFind = TRUE;
			break;
		}
		iter++;
	}

	// 若找到正在编辑的图形,则将其拷贝到m_IAGraphic中,并从列表中删除
	if ((bFind) && (NULL != pIAGraphic))
	{
		memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));
		memcpy_s(&m_IAGraphic, sizeof(IA_INNER_GRAPHICS), pIAGraphic, sizeof(IA_INNER_GRAPHICS));
		m_IAGraphic.bFinish = FALSE;
		m_bActive = TRUE;

		//在List中删除该图形的存储
		IVS_DELETE(pIAGraphic);
		m_IADrawGraphicsList.erase(iter);
	
		// 开始修改图形（确定修改的类型以及根据需要转换坐标）
		lRet = StartModifyIAShapes(point, MouseEvent, ulGraphicGroup);

		// 修改失败将当前m_IAGraphic中图形插入列表中
		lRet = ModifyFinishedAndInsert(lRet);
	}
	return lRet;
}

// 智能分析随鼠标移动修改图形相应的坐标
LONG CIADrawer::StartModifyIAShapes(POINT point, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup) 
{
	LONG lRet = IVS_FAIL;
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Start Modify IA Shapes", "m_IAGraphic is Inactive");
		return lRet;
	}

	switch (m_IAGraphic.iaGraphic.ulGraphicType)
	{
	case IA_GRAPHICS_TYPE_STRAIGHT_LINE:
		lRet = ModifyIAStraightLine(point, MouseEvent, ulGraphicGroup);
		break;
	case IA_GRAPHICS_TYPE_RECTANGLE:
		lRet = ModifyIARectangle(point, &m_lPointNo, MouseEvent, ulGraphicGroup);
		break;
	case IA_GRAPHICS_TYPE_POLYGON:
		lRet = ModifyIAPolygon(point, &m_lPointNo, MouseEvent, ulGraphicGroup);
		break;
	case IA_GRAPHICS_TYPE_UP_ULINE: 
	case IA_GRAPHICS_TYPE_DOWN_ULINE: 
		lRet = ModifyIAULine(point, MouseEvent, ulGraphicGroup);
		break;
	default:
		IVS_LOG(IVS_LOG_ERR, "Start Modify IA Shapes", "GraphicType Invalid");
		break;
	}	
	return lRet;
}


// 智能分析开始修改直线
LONG CIADrawer::ModifyIAStraightLine(POINT point, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup)
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Modify IA Straight Line", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	// 左键点击
	if (LEFT_BUTTON_DOWN == MouseEvent)
	{
		// 靠近直线起点，需要转换起止点坐标，标记当前状态为修改直线的点
		if (IsCloseToPoint(point, m_IAGraphic.iaGraphic.IAPoints[0]))
		{
			m_IAGraphic.iaGraphic.IAPoints[0] = m_IAGraphic.iaGraphic.IAPoints[1];
			m_IAGraphic.iaGraphic.IAPoints[1] = point;
			m_ulCurrentState = IA_MODIFING_STRAIGHT_LINE_POINT;
			if (ARROW_TYPE_LINE_TO_LEFT == m_IAGraphic.iaGraphic.ulArrowType)
			{
				m_IAGraphic.iaGraphic.ulArrowType = ARROW_TYPE_LINE_TO_RIGHT;
			}else if (ARROW_TYPE_LINE_TO_RIGHT == m_IAGraphic.iaGraphic.ulArrowType)
			{
				m_IAGraphic.iaGraphic.ulArrowType = ARROW_TYPE_LINE_TO_LEFT;
			}
			GetArrowPoints(m_IAGraphic.iaGraphic);
			m_bReversePoint = TRUE;
			return IVS_SUCCEED;
		}
		// 靠近直线终点，标记当前状态为修改直线的点
		if (IsCloseToPoint(point, m_IAGraphic.iaGraphic.IAPoints[1]))
		{
			m_IAGraphic.iaGraphic.IAPoints[1] = point;
			m_ulCurrentState = IA_MODIFING_STRAIGHT_LINE_POINT;
			return IVS_SUCCEED;
		}

		// 靠近直线线段，标记当前状态为移动图形
		if (CLOSE_TO_LINE_DISTANCE > 
			DistanceToLine(point, 
			m_IAGraphic.iaGraphic.IAPoints[0], m_IAGraphic.iaGraphic.IAPoints[1]))
		{
			m_RecordPoint = point;
			m_OriginIAPoints[0] = m_IAGraphic.iaGraphic.IAPoints[0];
			m_OriginIAPoints[1] = m_IAGraphic.iaGraphic.IAPoints[1];
			m_ulCurrentState = IA_MODIFING_MOVING_SHAPE;
			m_ulCurrentGraphicType = IA_GRAPHICS_TYPE_STRAIGHT_LINE;
			return IVS_SUCCEED;
		}
	}
	
	// 右键点击,删除直线
	if (RIGHT_BUTTON_DOWN == MouseEvent) 
	{
		if (IVS_SUCCEED == InsertCurrentToList())
		{
			return DeleteIAGraphicsByPoint(ulGraphicGroup, point);
		}
	}
	return IVS_FAIL;
}

// 智能分析开始修改矩形
LONG CIADrawer::ModifyIARectangle(POINT point, LONG *pPointNo, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup)
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Modify IA Rectangle", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	*pPointNo = POINTNO_ILLEGAL; // 标记新顶点的数组下标
	for (ULONG i = 0; i < POINT_NUM_OF_RECTANGLE; i++)
	{
		// 点在矩形顶点附近
		if (IsCloseToPoint(point, m_IAGraphic.iaGraphic.IAPoints[i]))
		{
			if (!m_IAGraphic.iaGraphic.bAllowModifySize)
			{
				break;
			}
			return ModifyIARectanglePoint(MouseEvent, i, point);
		}
	}
	for (ULONG i = 0; i < POINT_NUM_OF_RECTANGLE; i++)
	{
		// 点在矩形边附近
		if (CLOSE_TO_LINE_DISTANCE > 
			DistanceToLine(point, 
			m_IAGraphic.iaGraphic.IAPoints[i], m_IAGraphic.iaGraphic.IAPoints[i + 1]))
		{
			return ModifyIARectangleLine(pPointNo, MouseEvent, i, point);
		}
	}

	// 右键点击矩形内部,删除图形
	if (RIGHT_BUTTON_DOWN == MouseEvent) 
	{
		if (IVS_SUCCEED == InsertCurrentToList())
		{
			return DeleteIAGraphicsByPoint(ulGraphicGroup, point);
		}
	}
	return IVS_FAIL;
}

// 智能分析开始修改矩形（修改点：左键拖动单个点缩放矩形、右键点击单个点整体拖动）
LONG CIADrawer::ModifyIARectanglePoint(MOUSE_EVENT_TYPE MouseEvent, ULONG ulPointIndex, POINT /*point*/) 
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Modify IA Rectangle Point", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	POINT tmp_Points[5];
	memset(tmp_Points, 0, sizeof(tmp_Points));

	// 左键点击，转换对顶点坐标为起始坐标，标记当前状态为按点改变矩形大小
	if (LEFT_BUTTON_DOWN == MouseEvent && m_IAGraphic.iaGraphic.bAllowModifySize)
	{	
		tmp_Points[0] = m_IAGraphic.iaGraphic.IAPoints[(ulPointIndex + 2) % POINT_NUM_OF_RECTANGLE];
		tmp_Points[1] = m_IAGraphic.iaGraphic.IAPoints[(ulPointIndex + 3) % POINT_NUM_OF_RECTANGLE];
		tmp_Points[2] = m_IAGraphic.iaGraphic.IAPoints[(ulPointIndex + 4) % POINT_NUM_OF_RECTANGLE];
		tmp_Points[3] = m_IAGraphic.iaGraphic.IAPoints[(ulPointIndex + 1) % POINT_NUM_OF_RECTANGLE];
		tmp_Points[4] = tmp_Points[0];
		memcpy_s(m_IAGraphic.iaGraphic.IAPoints, sizeof(POINT) * POINTS_NUM_MAX, 
			tmp_Points, sizeof(POINT) * 5);

		m_ulCurrentState = IA_MODIFING_RECTANGLE_POINT;
		return IVS_SUCCEED;
	}
#if 0 // 右键点击点，整体移动变成删除点
	// 右键点击，标记当前状态为整体拖动图形
	if (RIGHT_BUTTON_DOWN == MouseEvent)
	{
		m_RecordPoint = point;
		for (ULONG j = 0; j < 5; j++)
		{
			m_OriginIAPoints[j] = m_IAGraphic.iaGraphic.IAPoints[j];
		}
		m_ulCurrentGraphicType = IA_GRAPHICS_TYPE_RECTANGLE;
		m_ulCurrentState = IA_MODIFING_MOVING_SHAPE;
		return IVS_SUCCEED;
	}
#endif
	// 右键点击，删除矩形单个点
	if (RIGHT_BUTTON_DOWN == MouseEvent && m_IAGraphic.iaGraphic.bAllowAddNewPoint)
	{
		memcpy_s(tmp_Points, sizeof(tmp_Points), 
			m_IAGraphic.iaGraphic.IAPoints, sizeof(tmp_Points));
		// 逐个前移后续顶点,记录在临时数组中
		for (ULONG j = ulPointIndex; j < 4; j++)
		{
			tmp_Points[j] = tmp_Points[j + 1];
		}

		// 闭合最后一个点
		tmp_Points[3] = tmp_Points[0];
		tmp_Points[4].x = 0;
		tmp_Points[4].y = 0;
		memcpy_s(m_IAGraphic.iaGraphic.IAPoints, sizeof(tmp_Points), 
			tmp_Points, sizeof(tmp_Points));
		m_IAGraphic.iaGraphic.ulPointsNum--;
		m_IAGraphic.iaGraphic.ulGraphicType = IA_GRAPHICS_TYPE_POLYGON;
		m_ulCurrentState = IA_MODIFING_POLYGON_DELETE_POINT;
		return IVS_SUCCEED;
	}

	m_IAGraphic.bFinish = TRUE;
	return IVS_FAIL;
}

// 智能分析开始修改矩形（修改边：拖动单边、在边上增加节点）
LONG CIADrawer::ModifyIARectangleLine(LONG *pPointNo, MOUSE_EVENT_TYPE MouseEvent, ULONG ulPointIndex, POINT point) 
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Modify IA Rectangle Line", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	*pPointNo = POINTNO_ILLEGAL;

#if 0// 左键点击点，修改单边变成整体移动
	// 左键点击，转换对定点坐标为起始坐标，标记当前状态为按边修改矩形大小
	if (LEFT_BUTTON_DOWN == MouseEvent)
	{
		POINT tmp_points[5];
		memset(tmp_points, 0, sizeof(tmp_points));

		// 修改矩形的水平边:上下拖动
		if (m_IAGraphic.iaGraphic.IAPoints[ulPointIndex].y 
			== m_IAGraphic.iaGraphic.IAPoints[ulPointIndex+1].y)
		{
			tmp_points[0] = m_IAGraphic.iaGraphic.IAPoints[(ulPointIndex + 2) % POINT_NUM_OF_RECTANGLE];
			tmp_points[1] = m_IAGraphic.iaGraphic.IAPoints[(ulPointIndex + 3) % POINT_NUM_OF_RECTANGLE];
			tmp_points[2] = m_IAGraphic.iaGraphic.IAPoints[(ulPointIndex + 4) % POINT_NUM_OF_RECTANGLE];
			tmp_points[3] = m_IAGraphic.iaGraphic.IAPoints[(ulPointIndex + 1) % POINT_NUM_OF_RECTANGLE];
			tmp_points[4] = tmp_points[0];
			memcpy_s(m_IAGraphic.iaGraphic.IAPoints, sizeof(POINT) * POINTS_NUM_MAX, 
				tmp_points, sizeof(POINT) * 5);

			m_ulCurrentState = IA_MODIFING_RECTANGLE_HORIZON_LINE;
		}
		else //修改矩形的垂直边:左右拖动
		{
			tmp_points[0] = m_IAGraphic.iaGraphic.IAPoints[(ulPointIndex + 3) % POINT_NUM_OF_RECTANGLE];
			tmp_points[1] = m_IAGraphic.iaGraphic.IAPoints[(ulPointIndex + 4) % POINT_NUM_OF_RECTANGLE];
			tmp_points[2] = m_IAGraphic.iaGraphic.IAPoints[(ulPointIndex + 1) % POINT_NUM_OF_RECTANGLE];
			tmp_points[3] = m_IAGraphic.iaGraphic.IAPoints[(ulPointIndex + 2) % POINT_NUM_OF_RECTANGLE];
			tmp_points[4] = tmp_points[0];
			memcpy_s(m_IAGraphic.iaGraphic.IAPoints, sizeof(POINT) * POINTS_NUM_MAX, 
				tmp_points, sizeof(POINT) * 5);

			m_ulCurrentState = IA_MODIFING_RECTANGLE_VERTICAL_LINE;
		}

		return IVS_SUCCEED;
	}
#endif
	// 右键点击，标记当前状态为整体拖动图形
	if (LEFT_BUTTON_DOWN == MouseEvent)
	{
		m_RecordPoint = point;
		for (ULONG j = 0; j < 5; j++)
		{
			m_OriginIAPoints[j] = m_IAGraphic.iaGraphic.IAPoints[j];
		}
		m_ulCurrentGraphicType = IA_GRAPHICS_TYPE_RECTANGLE;
		m_ulCurrentState = IA_MODIFING_MOVING_SHAPE;
		return IVS_SUCCEED;
	}

	// 右键点击,并且该矩形是可以增加顶点变成多边形的,将矩形转换为多边形,标记当前状态为增加多边形顶点
	if ((RIGHT_BUTTON_DOWN == MouseEvent) && m_IAGraphic.iaGraphic.bAllowAddNewPoint)
	{
		m_IAGraphic.iaGraphic.ulGraphicType = IA_GRAPHICS_TYPE_POLYGON;
		for (ULONG j = 5; j > (ulPointIndex + 1); j--)
		{
			m_IAGraphic.iaGraphic.IAPoints[j] = m_IAGraphic.iaGraphic.IAPoints[j - 1];
		}
		m_IAGraphic.iaGraphic.IAPoints[ulPointIndex + 1] = point;
		*pPointNo = (LONG)(ulPointIndex + 1);
		m_IAGraphic.iaGraphic.ulPointsNum++;
		m_ulCurrentState = IA_MODIFING_POLYGON_ADD_POINT;
		return IVS_SUCCEED;
	}
	m_IAGraphic.bFinish = TRUE;
	return IVS_FAIL;
}

// 智能分析开始修改多边形
LONG CIADrawer::ModifyIAPolygon(POINT point, LONG *pPointNo, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup)
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Modify IA Polygon", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	*pPointNo = POINTNO_ILLEGAL;  // 标记新顶点的数组下标
	for (ULONG k = 0; k < m_IAGraphic.iaGraphic.ulPointsNum - 1; k++)
	{
		// 点在多边形顶点附近
		if (IsCloseToPoint(point, m_IAGraphic.iaGraphic.IAPoints[k]))
		{
			return ModifyIAPolygonPoint(pPointNo, MouseEvent, ulGraphicGroup, k);
		}
	}
	for (ULONG k = 0; k < m_IAGraphic.iaGraphic.ulPointsNum - 1; k++)
	{
		// 点在多边形边附近
		if (CLOSE_TO_LINE_DISTANCE > 
			DistanceToLine(point, m_IAGraphic.iaGraphic.IAPoints[k], 
						   m_IAGraphic.iaGraphic.IAPoints[k + 1]))
		{
			return ModifyIAPolygonLine(pPointNo, MouseEvent, point, k);
		}
	}

	// 右键点击多边形内部,删除图形
	if (RIGHT_BUTTON_DOWN == MouseEvent) 
	{
		if (IVS_SUCCEED == InsertCurrentToList())
		{
			return DeleteIAGraphicsByPoint(ulGraphicGroup, point);
		}
	}
	return IVS_FAIL;
}

// 智能分析开始修改多边形（修改点：左键修改单个点位置、右键删除该点）
LONG CIADrawer::ModifyIAPolygonPoint(LONG *pPointNo, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup, ULONG ulPointIndex) 
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Modify IA Polygon Point", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}

	*pPointNo = POINTNO_ILLEGAL;
	ULONG ulPointNum = m_IAGraphic.iaGraphic.ulPointsNum;
	POINT tmp_Points[POINTS_NUM_MAX];
	memset(tmp_Points, 0, sizeof(tmp_Points));
	memcpy_s(tmp_Points, sizeof(POINT) * POINTS_NUM_MAX, 
		m_IAGraphic.iaGraphic.IAPoints, 
		sizeof(POINT) * m_IAGraphic.iaGraphic.ulPointsNum);

	// 左键点击，标记当前状态为修改多边形单个顶点
	if (LEFT_BUTTON_DOWN == MouseEvent)
	{
		*pPointNo = (LONG)ulPointIndex;
		m_ulCurrentState = IA_MODIFING_POLYGON_MOVING_POINT;
		return IVS_SUCCEED;
	}

	// 左键点击，标记当前状态为删除多边形单个顶点
	if (RIGHT_BUTTON_DOWN == MouseEvent)
	{
		// 若多边形点数小于5,则说明为三角形,不再允许删除点
		if (ulPointNum < 5)
		{
			IVS_LOG(IVS_LOG_ERR, "Modify IA Polygon Point", "Reach MIN PointNum");
			return IVS_FAIL;
		}

		// 逐个前移后续顶点,记录在临时数组中
		for (ULONG j = ulPointIndex; j < ulPointNum; j++)
		{
			tmp_Points[j] = tmp_Points[j + 1];
		}
		ulPointNum--;
		// 确保数组不会越界
		if (ulPointNum >= POINTS_NUM_MAX)	
		{
			IVS_LOG(IVS_LOG_ERR, "Modify IA Polygon Point", "Reach MAX PointNum");
			return IVS_FAIL;
		}

		// 闭合最后一个点
		tmp_Points[ulPointNum - 1] = tmp_Points[0];

		// 若临时数组中存储的新多边形顶点信息没有非法情况,则拷贝入m_IAGraphic中
		if ((!IsForbiddenOverlaped(tmp_Points, ulPointNum, ulGraphicGroup, IA_GRAPHICS_TYPE_POLYGON)) 
			&& IsIAPolygonSelfLegal(tmp_Points, ulPointNum))
		{	
			memcpy_s(m_IAGraphic.iaGraphic.IAPoints, sizeof(POINT) * POINTS_NUM_MAX, 
				tmp_Points, sizeof(POINT) * m_IAGraphic.iaGraphic.ulPointsNum);
			m_IAGraphic.iaGraphic.ulPointsNum--;
			m_ulCurrentState = IA_MODIFING_POLYGON_DELETE_POINT;
			return IVS_SUCCEED;
		}
		IVS_LOG(IVS_LOG_ERR, "Modify IA Polygon Point", "New Polygon Illegal");
		return IVS_FAIL;
	}
	return IVS_FAIL;
}

// 智能分析开始修改多边形（修改边：左键整体拖动、右键增加新节点）
LONG CIADrawer::ModifyIAPolygonLine(LONG *pPointNo, MOUSE_EVENT_TYPE MouseEvent, POINT point, ULONG ulPointIndex) 
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Modify IA Polygon Line", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	*pPointNo = POINTNO_ILLEGAL;
	ULONG ulPointNum = m_IAGraphic.iaGraphic.ulPointsNum;
	POINT tmp_Points[POINTS_NUM_MAX];
	memset(tmp_Points, 0, sizeof(tmp_Points));
	memcpy_s(tmp_Points, sizeof(POINT) * POINTS_NUM_MAX, 
		m_IAGraphic.iaGraphic.IAPoints, 
		sizeof(POINT) * m_IAGraphic.iaGraphic.ulPointsNum);

	// 左键点击，标记当前状态为拖动整体图形
	if (LEFT_BUTTON_DOWN == MouseEvent)
	{
		m_RecordPoint = point;
		for (ULONG j = 0; j < ulPointNum; j++)
		{
			m_OriginIAPoints[j] = m_IAGraphic.iaGraphic.IAPoints[j];
		}
		m_ulCurrentGraphicType = IA_GRAPHICS_TYPE_POLYGON;
		m_ulCurrentState = IA_MODIFING_MOVING_SHAPE;
		return IVS_SUCCEED;
	}

	// 左键点击，标记当前状态为增加多边形顶点
	if (RIGHT_BUTTON_DOWN == MouseEvent)
	{
		// PointNum达到上限,不允许再增加顶点
		if (ulPointNum >= POINTS_NUM_MAX)
		{
			IVS_LOG(IVS_LOG_ERR, "Modify IA Polygon Line", "Reach MAX PointNum");
			return IVS_FAIL;
		}

		// 逐个后移后续顶点
		for (ULONG j = ulPointNum; j > (ulPointIndex + 1); j--)
		{
			m_IAGraphic.iaGraphic.IAPoints[j] = m_IAGraphic.iaGraphic.IAPoints[j - 1];
		}
		m_IAGraphic.iaGraphic.IAPoints[ulPointIndex + 1] = point;
		*pPointNo = (LONG)(ulPointIndex + 1);
		m_IAGraphic.iaGraphic.ulPointsNum++;
		m_ulCurrentState = IA_MODIFING_POLYGON_ADD_POINT;
		return IVS_SUCCEED;
	}
	return IVS_FAIL;
}

// 智能分析开始修改U型线
LONG CIADrawer::ModifyIAULine(POINT point, MOUSE_EVENT_TYPE MouseEvent, ULONG ulGraphicGroup)
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Modify IA ULine", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}

	// 左键点击
	if (LEFT_BUTTON_DOWN == MouseEvent)
	{
		// 靠近U型线起点，需要转换起止点坐标，标记当前状态为修改U型线的端点
		if (IsCloseToPoint(point, m_IAGraphic.iaGraphic.IAPoints[0]))
		{
			POINT tmp_point = m_IAGraphic.iaGraphic.IAPoints[1];
			m_IAGraphic.iaGraphic.IAPoints[0] = m_IAGraphic.iaGraphic.IAPoints[3];
			m_IAGraphic.iaGraphic.IAPoints[1] = m_IAGraphic.iaGraphic.IAPoints[2];
			m_IAGraphic.iaGraphic.IAPoints[2] = tmp_point;
			m_IAGraphic.iaGraphic.IAPoints[3] = point;
			m_IAGraphic.iaGraphic.ulPointsNum = POINT_NUM_OF_ULINE;
			m_ulCurrentState = IA_MODIFING_ULINE_POINT;
			return IVS_SUCCEED;
		}

		// 靠近U型线终点,标记当前状态为修改U型线的端点
		if (IsCloseToPoint(point,m_IAGraphic.iaGraphic.IAPoints[3]))
		{
			m_IAGraphic.iaGraphic.ulPointsNum = POINT_NUM_OF_ULINE;
			m_ulCurrentState = IA_MODIFING_ULINE_POINT;
			return IVS_SUCCEED;
		}

		// 靠近U型线中间拐角点,标记当前状态为修改U型线的拐角点
		if (IsCloseToPoint(point,m_IAGraphic.iaGraphic.IAPoints[1]) 
			|| (IsCloseToPoint(point,m_IAGraphic.iaGraphic.IAPoints[2])))
		{
			if (m_IAGraphic.iaGraphic.IAPoints[0].x > m_IAGraphic.iaGraphic.IAPoints[3].x)
			{
				POINT tmp_point            = m_IAGraphic.iaGraphic.IAPoints[0];
				m_IAGraphic.iaGraphic.IAPoints[0] = m_IAGraphic.iaGraphic.IAPoints[3];
				m_IAGraphic.iaGraphic.IAPoints[3] = tmp_point;
				tmp_point                  = m_IAGraphic.iaGraphic.IAPoints[1];
				m_IAGraphic.iaGraphic.IAPoints[1] = m_IAGraphic.iaGraphic.IAPoints[2];
				m_IAGraphic.iaGraphic.IAPoints[2] = tmp_point;
			}
			m_IAGraphic.iaGraphic.ulPointsNum = POINT_NUM_OF_ULINE;
			m_ulCurrentState = IA_MODIFING_ULINE_ANGLE;

            // add by wanglei 00165153
            //  DTS2013051709238 用户标记当前哪个顶点正在被修改（不包括起点和终点）
            m_ulUlineModifyIndex = 2;
			if (IsCloseToPoint(point,m_IAGraphic.iaGraphic.IAPoints[1]))
			{
				m_ulUlineModifyIndex = 1;
			}
            

			return IVS_SUCCEED;
		}

		// 靠近U型线的线段，标记当前状态为移动图形
		for (ULONG k = 0; k < 3; k++)
		{
			if (CLOSE_TO_LINE_DISTANCE < 
				DistanceToLine(point, m_IAGraphic.iaGraphic.IAPoints[k], 
							   m_IAGraphic.iaGraphic.IAPoints[k + 1]))
			{
				continue;
			}
			m_RecordPoint = point;
			for (ULONG j = 0; j < POINT_NUM_OF_ULINE; j++)
			{
				m_OriginIAPoints[j] = m_IAGraphic.iaGraphic.IAPoints[j];
			}
			m_ulCurrentGraphicType = IA_GRAPHICS_TYPE_UP_ULINE;
			m_ulCurrentState = IA_MODIFING_MOVING_SHAPE;
			return IVS_SUCCEED;
		}
		return IVS_FAIL;
	}

	// 右键点击,删除U型线
	if (RIGHT_BUTTON_DOWN == MouseEvent) 
	{
		if (IVS_SUCCEED == InsertCurrentToList())
		{
			return DeleteIAGraphicsByPoint(ulGraphicGroup, point);
		}
	}
	return IVS_FAIL;
}


// 智能分析随鼠标移动修改图形相应的坐标
LONG CIADrawer::ModifyIAShapeUpDate(POINT point, BOOL bFinish, ULONG ulGraphicGroup)
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Modify IA Shape UpDate", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	LONG lRet = IVS_FAIL;

	// 根据修改类型修改具体的图形
	switch (m_ulCurrentState)
	{
	case IA_MODIFING_STRAIGHT_LINE_POINT:
		lRet = DrawIAStraightLine(point, bFinish, FALSE);
		break;
	case IA_MODIFING_MOVING_SHAPE:
		lRet = UpdateIAMove(point, ulGraphicGroup, bFinish);
		break;
	case IA_MODIFING_RECTANGLE_HORIZON_LINE:
		point.x = m_IAGraphic.iaGraphic.IAPoints[1].x;
		lRet = DrawIARectangle(point, bFinish, FALSE);
		break;
	case IA_MODIFING_RECTANGLE_VERTICAL_LINE:
		point.y = m_IAGraphic.iaGraphic.IAPoints[3].y;
		lRet = DrawIARectangle(point, bFinish, FALSE);
		break;
	case IA_MODIFING_RECTANGLE_POINT:
		lRet = DrawIARectangle(point, bFinish, FALSE);
		break;
	case IA_MODIFING_POLYGON_MOVING_POINT:	// 该情况处理同下
	case IA_MODIFING_POLYGON_DELETE_POINT:  // 该情况处理同下
	case IA_MODIFING_POLYGON_ADD_POINT:
		lRet = UpdateIAPolygon(point, bFinish, ulGraphicGroup);
		break;
	case IA_MODIFING_ULINE_POINT:
		lRet = DrawIAULine(point, bFinish, bFinish, FALSE);
		break;
	case IA_MODIFING_ULINE_ANGLE:
		lRet = UpdateIAULine(point, bFinish);
		break;
	default:
		IVS_LOG(IVS_LOG_ERR, "Modify IA Shape UpDate", "Modify Type Invalid");
		break;
	}
	
	// 修改结束插入图形列表
	if (bFinish)
	{
		lRet = ModifyFinishedAndInsert(lRet);
	}
	return lRet;
}

// 智能分析修改图形结束
LONG CIADrawer::ModifyFinishedAndInsert(LONG lRet) 
{
	// 修改成功时都已经插入列表中
	if ((IVS_SUCCEED != lRet) && m_bActive)
	{
		m_IAGraphic.bFinish = TRUE;
		LONG lInnerRet = InsertCurrentToList();
		if (IVS_SUCCEED != lInnerRet)
		{
			return lInnerRet;
		}
	}
	return lRet;
}

// 修改中更新多边形图形
LONG CIADrawer::UpdateIAPolygon(POINT point, BOOL bFinish, ULONG ulGraphicGroup)
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Update IA Polygon", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	LONG lRet = IVS_FAIL;
	switch (m_ulCurrentState)
	{
	case IA_MODIFING_POLYGON_DELETE_POINT:
		lRet = IVS_SUCCEED;
		break;
	case IA_MODIFING_POLYGON_MOVING_POINT:	// 该情况处理同下
	case IA_MODIFING_POLYGON_ADD_POINT:
		lRet = UpdateIAPolygonPoint(point, ulGraphicGroup);
		break;
	default:
		IVS_LOG(IVS_LOG_ERR, "Update IA Polygon", "Modify Type Invalid");
		break;
	}

	if (bFinish && (IVS_SUCCEED == lRet))
	{
		m_ulCurrentState = IA_DONOTHING;
		LONG lInnerRet = InsertCurrentToList();
		if (IVS_SUCCEED != lInnerRet)
		{
			return lInnerRet;
		}
	}
	return lRet;
}

// 整体拖动图形时更新图形
LONG CIADrawer::UpdateIAMove(POINT point, ULONG ulGraphicGroup, BOOL bFinish)
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Update IA Move", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	POINT tmp_Points[POINTS_NUM_MAX];	// 临时数组暂存修改结果
	memset(tmp_Points, 0 , sizeof(POINT) * POINTS_NUM_MAX);
	LONG xDistance = point.x - m_RecordPoint.x;
	LONG yDistance = point.y - m_RecordPoint.y;
	CAutoLock Lock(m_Mutex);

	// 判断移动中当前图形的坐标点是否进入非法区域
	for (ULONG i = 0; i < m_IAGraphic.iaGraphic.ulPointsNum; i++)
	{
		tmp_Points[i].x = m_OriginIAPoints[i].x + xDistance;
		tmp_Points[i].y = m_OriginIAPoints[i].y + yDistance;
		if ((!IsPointInside(&tmp_Points[i])) 
			|| (IsInForbiddenArea(tmp_Points[i], ulGraphicGroup)))
		{
			IVS_LOG(IVS_LOG_ERR, "Update IA Move", "Point Illegal");
			return IVS_FAIL;
		}
	}

	// 判断移动到新位置的当前图形是否与其他图形非法重叠
	if (IsForbiddenOverlaped(tmp_Points, m_IAGraphic.iaGraphic.ulPointsNum, 
								ulGraphicGroup, m_ulCurrentGraphicType))		
	{
		IVS_LOG(IVS_LOG_ERR, "Update IA Move", "Point Illegal");
		return IVS_FAIL;
	}
	
	// 修改合法,将临时数组中的点拷贝到m_IAGraphic中
	memcpy_s(m_IAGraphic.iaGraphic.IAPoints, sizeof(POINT) * POINTS_NUM_MAX, 
		tmp_Points, sizeof(POINT) * m_IAGraphic.iaGraphic.ulPointsNum);

	if (m_IAGraphic.iaGraphic.ulArrowType)
	{
		GetArrowPoints(m_IAGraphic.iaGraphic);
	}

	if (bFinish)
	{
		m_ulCurrentState = IA_DONOTHING;
		return InsertCurrentToList();
	}

	return IVS_SUCCEED;
	
}

// 拖动多边形单个节点时更新图形
LONG CIADrawer::UpdateIAPolygonPoint(POINT point, ULONG ulGraphicGroup)
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Update IA Polygon Point", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	POINT tmp_Points[POINTS_NUM_MAX];
	memset(tmp_Points, 0, sizeof(POINT) * POINTS_NUM_MAX);
	memcpy_s(tmp_Points, sizeof(POINT) * POINTS_NUM_MAX, 
		m_IAGraphic.iaGraphic.IAPoints, 
		sizeof(POINT) * m_IAGraphic.iaGraphic.ulPointsNum);

	// 判断记录新顶点的数组下标是否合法
	if (POINTNO_ILLEGAL == m_lPointNo)
	{
		IVS_LOG(IVS_LOG_ERR, "Update IA Polygon Point", "PointNo Invalid");
		return IVS_FAIL;
	}

	tmp_Points[m_lPointNo] = point;
	// 修改的点为起始点坐标时需要同时修改最后一个点的坐标
	if (0 == m_lPointNo)
	{
		ULONG ulPointNum = m_IAGraphic.iaGraphic.ulPointsNum - 1;
		tmp_Points[ulPointNum] = tmp_Points[0];
	}

	// 判断多边形是否与其他图形非法重叠、多边形自身是否合法
	if ((!IsForbiddenOverlaped(tmp_Points, m_IAGraphic.iaGraphic.ulPointsNum, 
								ulGraphicGroup, IA_GRAPHICS_TYPE_POLYGON)) 
		&& IsIAPolygonSelfLegal(tmp_Points, m_IAGraphic.iaGraphic.ulPointsNum))
	{
		memcpy_s(m_IAGraphic.iaGraphic.IAPoints, sizeof(POINT) * POINTS_NUM_MAX, 
			tmp_Points, sizeof(POINT) * m_IAGraphic.iaGraphic.ulPointsNum);
		return IVS_SUCCEED;
	}
	IVS_LOG(IVS_LOG_INFO, "Update IA Polygon Point", "Point Illegal");
	return IVS_FAIL;
}

// 拖动U型线单个节点时更新图形
LONG CIADrawer::UpdateIAULine(POINT point, BOOL bFinish)
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Update IA ULine", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	
    // add by wanglei 00165153
    //if ((IsCloseToPoint(point,m_IAGraphic.iaGraphic.IAPoints[1])
    //{
    //    m_ulUlineModifyIndex = 1;
    //}
    //else
    //{
    //    m_ulUlineModifyIndex = 2;
    //}
    //if (IsCloseToPoint(point, m_IAGraphic.iaGraphic.IAPoints[1]))
    // add by wanglei 00165153
    //  DTS2013051709238 用户标记当前哪个顶点正在被修改（不包括起点和终点）
    // 原先的IsCloseToPoint(...)局限性太小，很容易鼠标跟踪丢失
	if (m_ulUlineModifyIndex == 1)
	{
		LONG yRange = abs(m_IAGraphic.iaGraphic.IAPoints[1].y 
							- m_IAGraphic.iaGraphic.IAPoints[0].y);
		LONG xLeft  = m_IAGraphic.iaGraphic.IAPoints[0].x;
		LONG xRight = min(m_IAGraphic.iaGraphic.IAPoints[2].x - ULine_BOTTOM_WIDTH_MIN, 
							m_IAGraphic.iaGraphic.IAPoints[0].x + yRange);
		m_IAGraphic.iaGraphic.IAPoints[1].x = max(xLeft,point.x);
		m_IAGraphic.iaGraphic.IAPoints[1].x = min(xRight,m_IAGraphic.iaGraphic.IAPoints[1].x);
	}
    else
	//if (IsCloseToPoint(point, m_IAGraphic.iaGraphic.IAPoints[2]))
	{
		LONG yRange = abs(m_IAGraphic.iaGraphic.IAPoints[2].y 
							- m_IAGraphic.iaGraphic.IAPoints[3].y);
		LONG xLeft  = max(m_IAGraphic.iaGraphic.IAPoints[1].x 
							+ ULine_BOTTOM_WIDTH_MIN, m_IAGraphic.iaGraphic.IAPoints[3].x - yRange);
		LONG xRight = m_IAGraphic.iaGraphic.IAPoints[3].x;
		m_IAGraphic.iaGraphic.IAPoints[2].x = max(xLeft,point.x);
		m_IAGraphic.iaGraphic.IAPoints[2].x = min(xRight,m_IAGraphic.iaGraphic.IAPoints[2].x);
	}

	m_IAGraphic.bFinish = bFinish;
	return FinishAndInsertIntoGraphicList(FALSE);
}


// 取消选中智能分析图形
LONG CIADrawer::SelectCancel()
{
	IVS_TRACE();
	CAutoLock Lock(m_Mutex);

	if (m_IADrawGraphicsList.empty())
	{
		IVS_LOG(IVS_LOG_ERR, "Modify Cancel", "No Graphics");
		return IVS_SUCCEED;
	}

	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while(iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}

		if (pIAGraphic->bEditing)
		{
			pIAGraphic->bEditing = FALSE;
		}
		iter++;
	}
	return IVS_SUCCEED;
}


VOID CIADrawer::InsertCurrentToGraphicList()
{
	if (m_bActive)
	{
		// modify limingjie lWX175483: 2013-5-24 
		// TQE检测 InsertCurrentToList返回值强制转void
		(void)InsertCurrentToList();
	}

}

// 判断是否达到该图形组最大画图数
BOOL CIADrawer::IsFinishDrawGraphicGroup(ULONG ulGraphicGroup, ULONG ulMaxDrawNum) 
{
	ULONG ulCount = 0;
	CAutoLock Lock(m_Mutex);

	// 遍历图形列表,记录当前图形组完成图形数量
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while (iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}

		if (ulGraphicGroup == pIAGraphic->iaGraphic.ulGraphicGroup)
		{
			ulCount++;
		}
		iter++;
	}
	return (ulCount >= ulMaxDrawNum) ? TRUE : FALSE;
}


//************************************************
//===================内部实现======================
//************************************************

// 若画图结束，插入到画图列表中
LONG CIADrawer::FinishAndInsertIntoGraphicList(BOOL bIsDrawing)
{
	// 调用该函数之前已经激活过m_IAGraphic
	if (!m_bActive)
	{
		return IVS_FAIL;
	}
	CAutoLock Lock(m_Mutex);

	if (m_IAGraphic.bFinish)
	{
		LONG lRet = IVS_FAIL;
		// 画图状态下,当前图形组完成图形数量达到最大时
		if (bIsDrawing 
			&& IsFinishDrawGraphicGroup(m_IAGraphic.iaGraphic.ulGraphicGroup, m_ulMaxGroupShapeNum))
		{
			IVS_LOG(IVS_LOG_INFO, "Finish And Insert Into Graphic List", "Reach Max Draw Number");
		}

		m_ulCurrentState = IA_DONOTHING;
		lRet = InsertCurrentToList();
		return lRet;
	}
	return IVS_SUCCEED;
}

LONG CIADrawer::InsertCurrentToList() 
{
	// 申请新图形指针用于插入到图形列表中
	m_IAGraphic.bFinish = TRUE;
	IA_INNER_GRAPHICS *pInsertGraphic = NULL;
	if (NULL == IVS_NEW(pInsertGraphic))
	{
		IVS_LOG(IVS_LOG_ERR, "Insert Current To List", "No Enough Memory");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	if (m_bReversePoint)
	{
		if (ARROW_TYPE_LINE_TO_LEFT == m_IAGraphic.iaGraphic.ulArrowType)
		{
			m_IAGraphic.iaGraphic.ulArrowType = ARROW_TYPE_LINE_TO_RIGHT;
		}else if (ARROW_TYPE_LINE_TO_RIGHT == m_IAGraphic.iaGraphic.ulArrowType)
		{
			m_IAGraphic.iaGraphic.ulArrowType = ARROW_TYPE_LINE_TO_LEFT;
		}
		POINT tmpPoint = m_IAGraphic.iaGraphic.IAPoints[0];
		m_IAGraphic.iaGraphic.IAPoints[0] = m_IAGraphic.iaGraphic.IAPoints[1];
		m_IAGraphic.iaGraphic.IAPoints[1] = tmpPoint;
		GetArrowPoints(m_IAGraphic.iaGraphic);

		m_bReversePoint = FALSE;
	}

	memcpy_s(pInsertGraphic, sizeof(IA_INNER_GRAPHICS), 
		&m_IAGraphic, sizeof(IA_INNER_GRAPHICS));
	m_IADrawGraphicsList.push_back(pInsertGraphic);
	pInsertGraphic = NULL;

	m_bActive = FALSE;
	memset(&m_IAGraphic, 0, sizeof(IA_INNER_GRAPHICS));
	return IVS_SUCCEED;
}

// 画智能分析多边形
LONG CIADrawer::DrawIAPolygonShape(POINT point, BOOL bNextPoint, BOOL bFinish, ULONG ulGraphicGroup)
{
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA Polygon Shape", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}

	// 多边形定点数超过最大点数时返回错误
	if (POINTS_NUM_MAX < m_IAGraphic.iaGraphic.ulPointsNum)
	{
		IVS_LOG(IVS_LOG_DEBUG, "Draw IA Polygon Shape", "Reach to MAX PointNum");
		return IVS_FAIL;
	}
	if (1 == m_IAGraphic.iaGraphic.ulPointsNum)
	{
		m_IAGraphic.iaGraphic.ulPointsNum++;
	}
	ULONG ulIndex = m_IAGraphic.iaGraphic.ulPointsNum;
	
	// 多边形点数在结束时必须 >= 4
	if (bFinish && m_IAGraphic.iaGraphic.ulPointsNum < 4)
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA Polygon Shape", "No Enough Polyline");
		return IVS_FAIL;
	}
	// 判断画多边形新的一条边是否合法
	LONG lResult = IsDrawIAPolygonLegal(point, ulIndex, ulGraphicGroup);
	if (IVS_SUCCEED != lResult)
	{
		IVS_LOG(IVS_LOG_DEBUG, "Draw IA Polygon Shape", "Point Invalid");
		return lResult;
	}

	// 若多边形新点靠近起始点,则自动连接到起始点坐标
	if (IsCloseToPoint(point,m_IAGraphic.iaGraphic.IAPoints[0]))
	{
		point.x = m_IAGraphic.iaGraphic.IAPoints[0].x;
		point.y = m_IAGraphic.iaGraphic.IAPoints[0].y;
	}
	BOOL bReachMaxPoint = FALSE;
	// 是否确定该点为顶点坐标
	if (bNextPoint)
	{		
		if (POINTS_NUM_MAX > ulIndex)
		{
			// 初始化下个点
			m_IAGraphic.iaGraphic.ulPointsNum++;
			m_IAGraphic.iaGraphic.IAPoints[ulIndex].x = point.x;
			m_IAGraphic.iaGraphic.IAPoints[ulIndex].y = point.y;
		}
		else
		{
			bReachMaxPoint = TRUE; //顶点达到最大数量时特殊处理
		}
	}
	// 在m_IAGraphic中记录当前坐标点
	m_IAGraphic.iaGraphic.IAPoints[ulIndex - 1].x = point.x;
	m_IAGraphic.iaGraphic.IAPoints[ulIndex - 1].y = point.y;
	
	// 画多边形结束
	if (bFinish)
	{
		return IsPolygonFinishedLegal(ulGraphicGroup, bReachMaxPoint);
	}
	return IVS_SUCCEED;
}

// 判断多边形画完后整体是否合法
LONG CIADrawer::IsPolygonFinishedLegal(ULONG ulGraphicGroup, BOOL bReachMaxPoint) 
{
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Is Polygon Finished Legal", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}
	m_IAGraphic.bFinish = TRUE;
	
	m_IAGraphic.iaGraphic.ulPointsNum--;
	ULONG ulIndex = m_IAGraphic.iaGraphic.ulPointsNum - 1;

	// 顶点数没有达到最大时
	if ((POINTS_NUM_MAX > m_IAGraphic.iaGraphic.ulPointsNum) && !bReachMaxPoint)
	{
		m_IAGraphic.iaGraphic.ulPointsNum--;
		ulIndex--;
		m_IAGraphic.iaGraphic.IAPoints[ulIndex + 1].x = 0;
		m_IAGraphic.iaGraphic.IAPoints[ulIndex + 1].y = 0;
	 	m_IAGraphic.iaGraphic.IAPoints[ulIndex + 2].x = 0;
	 	m_IAGraphic.iaGraphic.IAPoints[ulIndex + 2].y = 0;
	}

	// 闭合最后的顶点与起始点
	if ((m_IAGraphic.iaGraphic.IAPoints[ulIndex].x != m_IAGraphic.iaGraphic.IAPoints[0].x) 
		|| (m_IAGraphic.iaGraphic.IAPoints[ulIndex].y != m_IAGraphic.iaGraphic.IAPoints[0].y))
	{
		if (POINTS_NUM_MAX <= m_IAGraphic.iaGraphic.ulPointsNum)
		{
			IVS_LOG(IVS_LOG_ERR, "Is Polygon Finished Legal", "Reach to MAX PointNum");
			return FALSE;
		}
		m_IAGraphic.iaGraphic.ulPointsNum++;
		m_IAGraphic.iaGraphic.IAPoints[ulIndex+1].x = m_IAGraphic.iaGraphic.IAPoints[0].x;
		m_IAGraphic.iaGraphic.IAPoints[ulIndex+1].y = m_IAGraphic.iaGraphic.IAPoints[0].y;
	}

	// 判断闭合图形后多边形是否自身合法和位置关系合法
	if (!IsIAPolygonSelfLegal(m_IAGraphic.iaGraphic.IAPoints, m_IAGraphic.iaGraphic.ulPointsNum)
		|| IsForbiddenOverlaped(m_IAGraphic.iaGraphic.IAPoints, 
		m_IAGraphic.iaGraphic.ulPointsNum, ulGraphicGroup, IA_GRAPHICS_TYPE_POLYGON))
	{
		IVS_LOG(IVS_LOG_ERR, "Is Polygon Finished Legal", "Polygon ItSelf Illegal");
		return IVS_FAIL;
	}
	return IVS_SUCCEED;
}

// 判断多边形自身是否合法
BOOL CIADrawer::IsIAPolygonSelfLegal(const POINT *pPoint, ULONG ulPointsNum)
{
	if ((NULL == pPoint) || (4 > ulPointsNum))
	{
		return FALSE;
	}
	// 遍历多边形自身各条线段是否相交
	for (ULONG i = 0; i < ulPointsNum - 2; i++)
	{
		for (ULONG j = i + 2; j < ulPointsNum - 1; j++)
		{
			if ((0 == i) && ((ulPointsNum - 2) == j))
			{
				continue;
			}
			if (IsLineSegmentCross(pPoint[i], pPoint[i + 1], 
				pPoint[j], pPoint[j + 1]))
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}


// 智能分析画单个U形线
LONG CIADrawer::DrawIAULineShape(POINT point, BOOL bNextPoint, BOOL bIsDrawing)
{   
	LONG lRet = IVS_FAIL;
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA ULine Shape", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}

	if (1 == m_IAGraphic.iaGraphic.ulPointsNum)
	{
		m_IAGraphic.iaGraphic.ulPointsNum++;
	}
	
	switch (m_IAGraphic.iaGraphic.ulPointsNum)
	{
	case 2:  // U型线当前点数为2
		lRet = DrawIAUFirstLine(point, bNextPoint);
		break;
	case 3:  // U型线当前点数为3
		lRet = DrawIAUSecondLine(point,bNextPoint);
		break;
	case 4:  // U型线当前点数为4
		lRet = DrawIAUThirdLine(point,bNextPoint, bIsDrawing);
		break;
	default:
		IVS_LOG(IVS_LOG_ERR, "Draw IA ULine Shape", "PointNum Error");
		break;
	}
	return lRet;
}

// 智能分析画U形线第一条线
LONG CIADrawer::DrawIAUFirstLine(POINT point, BOOL bNextPoint) 
{
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA U_FirstLine", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}

	// U型线两边与底边的夹角不能大于135度
	LONG xRange = abs(point.x - m_IAGraphic.iaGraphic.IAPoints[0].x);
	LONG yRange = abs(point.y - m_IAGraphic.iaGraphic.IAPoints[0].y);
	if (xRange > yRange)
	{
		point.x = (point.x > m_IAGraphic.iaGraphic.IAPoints[0].x) 
			? (m_IAGraphic.iaGraphic.IAPoints[0].x + yRange) 
			: (m_IAGraphic.iaGraphic.IAPoints[0].x - yRange);
	}

	// 是否确定该点为U型线第二个点和初始化下一个点
	if (bNextPoint)
	{
		m_IAGraphic.iaGraphic.IAPoints[2] = point;
		m_IAGraphic.iaGraphic.ulPointsNum++;
	}
	m_IAGraphic.iaGraphic.IAPoints[1] = point;
	m_IAGraphic.bFinish     = FALSE;
	return IVS_SUCCEED;
}

// 智能分析画U形线第二条线
LONG CIADrawer::DrawIAUSecondLine(POINT point,BOOL bNextPoint)
{   
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA U_SecondLine", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}

	// U型线第二条线为底边,必须水平
	m_IAGraphic.iaGraphic.IAPoints[2].x = point.x; 
	m_IAGraphic.iaGraphic.IAPoints[2].y = m_IAGraphic.iaGraphic.IAPoints[1].y;

	// U型线两边必须为竖直或外张,根据第一和第二个点的位置确定水平线的左右方向
	if (m_IAGraphic.iaGraphic.IAPoints[1].x > m_IAGraphic.iaGraphic.IAPoints[0].x)
	{
		m_IAGraphic.iaGraphic.IAPoints[2].x 
			= max(point.x, m_IAGraphic.iaGraphic.IAPoints[1].x + ULine_BOTTOM_WIDTH_MIN);
	}
	if (m_IAGraphic.iaGraphic.IAPoints[1].x < m_IAGraphic.iaGraphic.IAPoints[0].x)
	{
		m_IAGraphic.iaGraphic.IAPoints[2].x 
			= min(point.x, m_IAGraphic.iaGraphic.IAPoints[1].x - ULine_BOTTOM_WIDTH_MIN);
	}
	
	// 是否确定该点为U型线第三个点和初始化下一个点
	if (bNextPoint)
	{
		m_IAGraphic.iaGraphic.IAPoints[3] = m_IAGraphic.iaGraphic.IAPoints[2];
		m_IAGraphic.iaGraphic.ulPointsNum++;
	}
	
	m_IAGraphic.bFinish = FALSE;
	return IVS_SUCCEED;

}

// 智能分析画U形线第三条线
LONG CIADrawer::DrawIAUThirdLine(POINT point,BOOL bNextPoint, BOOL bIsDrawing)
{   
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Draw IA U_ThirdLine", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}

	// 根据前三个确定的点,判断第四个点的合法区域,对越界的点做相应限制
	LONG xRange = abs(point.x - m_IAGraphic.iaGraphic.IAPoints[2].x);
	LONG yRange = abs(point.y - m_IAGraphic.iaGraphic.IAPoints[2].y);
	int xflag = (point.x - m_IAGraphic.iaGraphic.IAPoints[2].x) 
		* (m_IAGraphic.iaGraphic.IAPoints[2].x - m_IAGraphic.iaGraphic.IAPoints[0].x);
	int yflag = (point.y - m_IAGraphic.iaGraphic.IAPoints[2].y) 
		* (m_IAGraphic.iaGraphic.IAPoints[2].y - m_IAGraphic.iaGraphic.IAPoints[0].y);
	m_IAGraphic.iaGraphic.IAPoints[3] = point;

	// 判断第四个点的x坐标是否在第一和第三个点之间
	// 0 >= xflag表明在第一和第三个点之间,不满足两边外张的要求
	if (0 >= xflag) 
	{
		m_IAGraphic.iaGraphic.IAPoints[3].x = m_IAGraphic.iaGraphic.IAPoints[2].x;
	}
	else
	{
		// U型线两边与底边的夹角不能大于135度
		if (xRange > yRange)
		{
			if (m_IAGraphic.iaGraphic.IAPoints[2].x > m_IAGraphic.iaGraphic.IAPoints[0].x)
			{
				m_IAGraphic.iaGraphic.IAPoints[3].x = m_IAGraphic.iaGraphic.IAPoints[2].x + yRange;
			}
			else
			{
				m_IAGraphic.iaGraphic.IAPoints[3].x = m_IAGraphic.iaGraphic.IAPoints[2].x - yRange;
			}
		}
	}

	// U型线两边必须在底边同一侧
	if (0 < yflag)
	{
		m_IAGraphic.iaGraphic.IAPoints[3] = m_IAGraphic.iaGraphic.IAPoints[2];
		if (bIsDrawing)
		{
			return IVS_FAIL;
		}
	}
	if (bNextPoint)
	{
		m_IAGraphic.bFinish = TRUE;
	}
	
	return IVS_SUCCEED;
}





// 显示智能分析单个图形
VOID CIADrawer::ShowIAShape(HDC hDC, IA_INNER_GRAPHICS &pGraphics)
{
	IA_GRAPHICS &rGraphic = pGraphics.iaGraphic;
	COLORREF color  = RGB(rGraphic.LineColor.Red,
						rGraphic.LineColor.Green, 
						rGraphic.LineColor.Blue);
	INT	iLineWidth = (INT)max(0.5 >= m_fDiv ? 1 : 2, pGraphics.iaGraphic.ulLineWidth * m_fDiv + 0.5);

// 	INT iLineWidth  = (INT)pGraphics.iaGraphic.ulLineWidth + (INT)m_lLineWidthAdjust;
// 	iLineWidth = (LONG)max(1, iLineWidth);

	HBRUSH brush    = (HBRUSH)GetStockObject(NULL_BRUSH);
	HPEN   pen      = CreatePen(PS_SOLID, iLineWidth, color);
	HPEN   oldPen   = (HPEN)SelectObject(hDC, (HGDIOBJ)pen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, (HGDIOBJ)brush);

	
	if (IA_GRAPHICS_TYPE_RECTANGLE == pGraphics.iaGraphic.ulGraphicType)
	{
		Rectangle(hDC, pGraphics.iaGraphic.IAPoints[0].x, 
				pGraphics.iaGraphic.IAPoints[0].y, 
				pGraphics.iaGraphic.IAPoints[2].x, 
				pGraphics.iaGraphic.IAPoints[2].y);
		
	}
	else
	{
		POINT *points      = (POINT*)pGraphics.iaGraphic.IAPoints;
		ULONG  ulPointsNum = pGraphics.iaGraphic.ulPointsNum;
		ShowPolyline(hDC, points, ulPointsNum, color);
	}
	if (pGraphics.iaGraphic.ulArrowType)
	{
		ShowArrowShape(hDC, pGraphics.iaGraphic.IAArrow, pGraphics.iaGraphic.ulArrowType);
	}
	// 释放资源
	SelectObject(hDC,oldPen);
	SelectObject(hDC,oldBrush);
	DeleteObject(pen);

	// 显示图像正在被编辑的标志
	if (pGraphics.bEditing)
	{
		ShowGraphicChoosen(hDC, pGraphics);
	}

	// 显示图像名称
	ShowGraphicName(hDC, pGraphics);

	// 显示半透明区域
	BOOL bNeedAlphaBlend = (((IA_GRAPHICS_TYPE_RECTANGLE == pGraphics.iaGraphic.ulGraphicType) 
							|| (IA_GRAPHICS_TYPE_POLYGON == pGraphics.iaGraphic.ulGraphicType))
							&& pGraphics.bFinish && pGraphics.iaGraphic.BgColor.Alpha);
	if (bNeedAlphaBlend)
	{	
		ShowAlphaBlendArea(hDC, pGraphics);
	}

	return;
}

VOID CIADrawer::ShowArrowShape(HDC hDC, const IA_ARROW &iaArrow, ULONG ulArrowType)
{
	for (unsigned int i = 1; i <= 2; i++)
	{
		if (i & ulArrowType)
		{
			MoveToEx(hDC, iaArrow.ArrowTail[i - 1].x, iaArrow.ArrowTail[i - 1].y, NULL);
			LineTo(hDC, iaArrow.ArrowHead[i - 1].x, iaArrow.ArrowHead[i - 1].y);
			LineTo(hDC, iaArrow.ArrowTrangle1[i - 1].x, iaArrow.ArrowTrangle1[i - 1].y);
			MoveToEx(hDC, iaArrow.ArrowHead[i - 1].x, iaArrow.ArrowHead[i - 1].y, NULL);
			LineTo(hDC, iaArrow.ArrowTrangle2[i - 1].x, iaArrow.ArrowTrangle2[i - 1].y);
		}
	}
	return;
}

// 显示智能分析多边形
VOID CIADrawer::ShowPolyline(HDC hDC, const POINT *points, ULONG numberPoints, COLORREF /*color*/)
{
	if ((NULL == points) || (POINTS_NUM_MAX < numberPoints))
	{
		IVS_LOG(IVS_LOG_ERR, "Show Polyline", "Input invalid.");
		return;
	}
	MoveToEx(hDC,points[0].x,points[0].y,NULL);
	for (ULONG i=1; i<numberPoints; i++)
	{
		LineTo(hDC,points[i].x, points[i].y);
	}
	return;
}

// 选中智能分析图形
VOID CIADrawer::ShowGraphicChoosen(HDC hDC, const IA_INNER_GRAPHICS &pIAGraphics)
{
	const IA_GRAPHICS &rGraphic = pIAGraphics.iaGraphic;
	COLORREF color  = RGB(rGraphic.LineColor.Red,
						  rGraphic.LineColor.Green, 
						  rGraphic.LineColor.Blue);
	INT iLineWidth = (INT)max(0.5 >= m_fDiv ? 1 : 2, pIAGraphics.iaGraphic.ulLineWidth * m_fDiv + 0.5);
// 	INT iLineWidth  = (INT)pIAGraphics.iaGraphic.ulLineWidth + (INT)m_lLineWidthAdjust;
// 	iLineWidth = (LONG)max(1, iLineWidth);

	HBRUSH   brush    =(HBRUSH)GetStockObject(WHITE_BRUSH);
	HPEN     pen      = CreatePen(PS_SOLID, iLineWidth, color);
	HPEN     oldPen   = (HPEN)SelectObject(hDC, (HGDIOBJ)pen);
	HBRUSH   oldBrush = (HBRUSH)SelectObject(hDC, (HGDIOBJ)brush);

	// 在图形各个顶点画出小方框
	for (ULONG i = 0; i < pIAGraphics.iaGraphic.ulPointsNum; i++)
	{
		// 设置radius为显示修改标记的小正方形半径
		LONG radius = (LONG)iLineWidth + 1;
		Rectangle(hDC, pIAGraphics.iaGraphic.IAPoints[i].x - radius, 
					pIAGraphics.iaGraphic.IAPoints[i].y - radius,
					pIAGraphics.iaGraphic.IAPoints[i].x + radius, 
					pIAGraphics.iaGraphic.IAPoints[i].y + radius);
	}
	SelectObject(hDC,oldPen);
	SelectObject(hDC,oldBrush);
	DeleteObject(pen);	
		
	return;
}

// 显示图形名称
VOID CIADrawer::ShowGraphicName(HDC hDC, const IA_INNER_GRAPHICS &pIAGraphics)
{
	RECT rc;
	if (!FindEdgeRectangle(pIAGraphics.iaGraphic.IAPoints, pIAGraphics.iaGraphic.ulPointsNum, rc))
	{
		return;
	}
	
	if (NULL == m_hFont)
	{
		IVS_LOG(IVS_LOG_ERR, "Show Graphic Name", "m_hFont NULL");
		return;
	}
	const IA_GRAPHICS &rGraphic = pIAGraphics.iaGraphic;
	COLORREF color  = RGB(rGraphic.LineColor.Red,
						  rGraphic.LineColor.Green, 
						  rGraphic.LineColor.Blue);
	SetTextColor(hDC, color); //前景   
	SetBkMode(hDC,TRANSPARENT);
	SelectObject(hDC,m_hFont);
	switch (pIAGraphics.iaGraphic.NamePosition)
	{
	case GRAPHIC_NAME_ABOVE:
		// 距离图形上边界空出5个像素
		TextOut(hDC, rc.left, rc.top + m_lf.lfHeight - 5, 
				pIAGraphics.iaGraphic.GraphicGroupName, 
				(int)strlen(pIAGraphics.iaGraphic.GraphicGroupName));
		break;
	case GRAPHIC_NAME_UNDER:
		// 距离图形下边界空出5个像素
		TextOut(hDC, rc.left, rc.bottom + 5, 
				pIAGraphics.iaGraphic.GraphicGroupName, 
				(int)strlen(pIAGraphics.iaGraphic.GraphicGroupName));
		break;
	case GRAPHIC_NAME_INSIDE:
		TextOut(hDC, rc.left, rc.bottom + m_lf.lfHeight - 5, 
				pIAGraphics.iaGraphic.GraphicGroupName, 
				(int)strlen(pIAGraphics.iaGraphic.GraphicGroupName));
		break;
	default:
		IVS_LOG(IVS_LOG_ERR, "Show Graphic Name", "Name Position Invalid");
		break;
	}

}



// 判断点是否在线段上
BOOL CIADrawer::IsOnLine(const POINT *pPointA, const POINT* pPointB, POINT point)
{  
	LONG CompWidth = (LONG)(m_ulDCWidth / 200); 

	BOOL bRet = IsPointOnLine(pPointA, pPointB, point);
	CHECK_RESULT(bRet, TRUE);

	LONG xflag = (point.x - pPointA->x) * (point.x - pPointB->x);
	LONG yflag = (point.y - pPointA->y) * (point.y - pPointB->y);
	if ((xflag > 0) || (yflag > 0))
	{ 
		return   FALSE; 
	} 

	if (pPointA->x != pPointB->x)
	{ 
		double k  = (double)(pPointB->y - pPointA->y) 
			/ (double)(pPointB->x - pPointA->x); 
		double kt = - 1.0 / k; 
		double xt = (double)((((point.y - pPointB->y) - kt * point.x) + k * pPointB->x) 
			/ (double)(k - kt)); 
		double yt = (double)(point.y - kt * (point.x - xt)); 
		double d  = sqrt((double)(yt - point.y) * (yt - point.y) 
			+ (xt - point.x) * (xt - point.x)); 
		if (d < (double)CompWidth)
		{ 
			return   TRUE; 
		} 
	} 
	return   FALSE; 
}

// 判断点是否在线上（内部）
BOOL CIADrawer::IsPointOnLine(const POINT *pPointA, const POINT *pPointB, POINT point)
{
	LONG CompWidth = static_cast<LONG>(m_ulDCWidth) / 200; 

	if (pPointA->x == pPointB->x)
	{
		LONG yMin = min(pPointA->y, pPointB->y);
		LONG yMax = max(pPointA->y, pPointB->y);
		if ((point.y > yMin) && (point.y < yMax) 
			&& (point.x > pPointA->x - CompWidth) 
			&& (point.x < pPointA->x + CompWidth))
		{ 
			return   TRUE; 
		}
	} 
	if (pPointA->y == pPointB->y)
	{
		LONG xMin = min(pPointA->x, pPointB->x);
		LONG xMax = max(pPointA->x, pPointB->x);
		if ((point.x > xMin) && (point.x < xMax) 
			&& (point.y > pPointA->y - CompWidth) 
			&& (point.y < pPointA->y + CompWidth))
		{ 
			return   TRUE; 
		}
	}
	return FALSE;
}

// 判断点是否在图形内
LONG CIADrawer::IsInShape(const IA_INNER_GRAPHICS *pIAGraphic, POINT point)
{
	if ((NULL == pIAGraphic) || (2 > pIAGraphic->iaGraphic.ulPointsNum))
	{
		IVS_LOG(IVS_LOG_ERR, "Is In Shape", "Input invalid.");
		return IVS_PARA_INVALID;
	}

	if ((IA_GRAPHICS_TYPE_STRAIGHT_LINE == pIAGraphic->iaGraphic.ulGraphicType)
		|| (IA_GRAPHICS_TYPE_UP_ULINE == pIAGraphic->iaGraphic.ulGraphicType) 
		|| (IA_GRAPHICS_TYPE_DOWN_ULINE == pIAGraphic->iaGraphic.ulGraphicType))
	{
		return IVS_FAIL;
	}
	
	if (IA_GRAPHICS_TYPE_RECTANGLE == pIAGraphic->iaGraphic.ulGraphicType)
	{
		return IsPointInRectangle(pIAGraphic->iaGraphic.IAPoints, 
			pIAGraphic->iaGraphic.ulPointsNum, point);
	}
	return IsPointInPolygon(pIAGraphic->iaGraphic.IAPoints, 
		pIAGraphic->iaGraphic.ulPointsNum, point);

}

// 判断点是否在矩形内
BOOL CIADrawer::IsPointInRectangle(const POINT *pPoint, ULONG /*ulPointNum*/, POINT point) 
{
	LONG lLeft   = min(pPoint[0].x, pPoint[2].x);
	LONG lRight  = max(pPoint[0].x, pPoint[2].x);
	LONG lTop    = min(pPoint[0].y, pPoint[2].y);
	LONG lButtom = max(pPoint[0].y, pPoint[2].y);
	return ((point.x >= lLeft) && (point.x <= lRight)
		&& (point.y >= lTop) && (point.y <= lButtom));
}


// 判断点是否在多边形内
// 判断点point是否在多边形内（包括边界）
// 若是多边形，则判断该点向左穿过界面与多边形的交点个数
BOOL CIADrawer::IsPointInPolygon(const POINT *pPoint, ULONG ulPointNum, POINT point) 
{
	INT nCross = 0;
	// 外部已判断ulPointNum，确定不小于2
	for (ULONG i = 0; i < ulPointNum - 1; i++) 
	{
		POINT p0 = pPoint[(i + ulPointNum - 1) % ulPointNum];
		POINT p1 = pPoint[i];  
		POINT p2 = pPoint[(i + 1) % ulPointNum];

		if (p1.y == p2.y)
		{
			BOOL bRet = (point.y == p1.y) 
						&& (point.x >=  min(p1.x, p2.x)) 
						&& (point.x <=  max(p1.x, p2.x));
			CHECK_RESULT(bRet, TRUE);
			continue;
		}

		if ((p1.y == point.y)&&(point.x >= p1.x))
		{
			LONG range = abs(p0.y - p2.y);
			LONG range0 = abs(p0.y - p1.y);
			LONG range2 = abs(p2.y - p1.y);
			if (range <= max(range0, range2))
			{
				nCross++; 
			}
			continue;
		}

		if ( point.y < min(p1.y, p2.y) )
			continue;
		if ( point.y > max(p1.y, p2.y) )
			continue;

		double x = (double)(point.y - p1.y) * (double)(p2.x - p1.x) 
					/ (double)(p2.y - p1.y) + p1.x;

		if (x == point.x)
			return TRUE;

		if (x < point.x) 
			nCross++;
	}
	return (nCross&1);
}




// 画多边形合法
LONG CIADrawer::IsDrawIAPolygonLegal(POINT newPoint, ULONG ulCurrentPointsNum, ULONG ulGraphicGroup)
{
	if (!m_bActive)
	{
		IVS_LOG(IVS_LOG_ERR, "Is Draw IA Polygon Legal", "m_IAGraphic is Inactive");
		return IVS_FAIL;
	}

	if (3 <= ulCurrentPointsNum)
	{
		for (ULONG i = 0; i < ulCurrentPointsNum - 3; i++)
		{
			if (IsLineSegmentCross(m_IAGraphic.iaGraphic.IAPoints[i], 
				m_IAGraphic.iaGraphic.IAPoints[i+1], 
				m_IAGraphic.iaGraphic.IAPoints[ulCurrentPointsNum-2], newPoint)
				&& ((m_IAGraphic.iaGraphic.IAPoints[i].x != newPoint.x) 
				|| (m_IAGraphic.iaGraphic.IAPoints[i].y != newPoint.y)))
			{
				return IVS_FAIL;
			}
		}
	}
	if (IsCrossForbiddenArea(newPoint, 
		m_IAGraphic.iaGraphic.IAPoints[ulCurrentPointsNum - 2], 
		ulGraphicGroup))
	{
		return IVS_FAIL;
	}
	return IVS_SUCCEED;
}



// 判断两条线段是否相交(有交点) 
BOOL CIADrawer::IsLineSegmentCross(POINT pFirst1, POINT pFirst2, POINT pSecond1, POINT pSecond2)  
{  
	//每个线段的两点都在另一个线段的左右不同侧，则能断定线段相交  
	//公式对于向量(x1,y1)->(x2,y2),判断点(x3,y3)在向量的左边,右边,还是线上.  
	//p=x1(y3-y2)+x2(y1-y3)+x3(y2-y1).p<0 左侧,    p=0 线上, p>0 右侧  
	LONG Linep1 = 0, Linep2 = 0;  
	//判断pSecond1和pSecond2是否在pFirst1->pFirst2两侧  
	Linep1 = pFirst1.x * (pSecond1.y - pFirst2.y) +  
		pFirst2.x * (pFirst1.y - pSecond1.y) +  
		pSecond1.x * (pFirst2.y - pFirst1.y); //lint !e838 
	Linep2 = pFirst1.x * (pSecond2.y - pFirst2.y) +  
		pFirst2.x * (pFirst1.y - pSecond2.y) +  
		pSecond2.x * (pFirst2.y - pFirst1.y);//lint !e838  
	// 当两条线段在同一侧时，需要判断线段是否相连
	if ((0 == Linep1) && (0 == Linep2))
	{
		if (!IsOnLine(&pFirst1, &pFirst2, pSecond1) 
			&& !(IsOnLine(&pFirst1, &pFirst2, pSecond2)))
		{
			return FALSE;
		}
	}
	//符号位异或为0:pSecond1和pSecond2在pFirst1->pFirst2同侧 
	if (((Linep1 ^ Linep2) >= 0 ) && !(Linep1 == 0 && Linep2 == 0)) 
	{  
		return FALSE;  
	}

	//判断pFirst1和pFirst2是否在pSecond1->pSecond2两侧  
	Linep1 = pSecond1.x * (pFirst1.y - pSecond2.y) +  
		pSecond2.x * (pSecond1.y - pFirst1.y) +  
		pFirst1.x * (pSecond2.y - pSecond1.y);  
	Linep2 = pSecond1.x * (pFirst2.y - pSecond2.y) +  
		pSecond2.x * (pSecond1.y - pFirst2.y) +  
		pFirst2.x * (pSecond2.y - pSecond1.y); 

	//符号位异或为0:pFirst1和pFirst2在pSecond1->pSecond2同侧  
	if (((Linep1 ^ Linep2) >= 0 ) && !(Linep1 == 0 && Linep2 == 0))
	{  
		return FALSE;  
	}  
	//否则判为相交  
	return TRUE;  
}






// 计算点到直线上最近一点的距离 X*X + Y*Y
LONG CIADrawer::DistanceToLine(POINT Point, POINT PointStart, POINT PointEnd)
{
	if (PointStart.y == PointEnd.y)  // 水平线处理
	{
		LONG left  = min(PointStart.x, PointEnd.x);
		LONG right = max(PointStart.x, PointEnd.x);
		if (Point.x < left)
		{
			return (Point.x - left) * (Point.x - left) + 
				(Point.y - PointStart.y) * (Point.y - PointStart.y);
		}

		if (Point.x > right)
		{
			return (Point.x - right) * (Point.x - right) + 
				(Point.y - PointStart.y) * (Point.y - PointStart.y);
		}
		return (Point.y - PointStart.y) * (Point.y - PointStart.y);
	} 
	else if (PointStart.x == PointEnd.x)  // 垂直线处理
	{
		LONG upper = max(PointStart.y, PointEnd.y);
		LONG lower = min(PointStart.y, PointEnd.y);
		if (Point.y < lower)
		{
			return (lower - Point.y) * (lower - Point.y) + 
				(PointStart.x - Point.x) * (PointStart.x - Point.x);
		}

		if (Point.y > upper)
		{
			return (upper - Point.y) * (upper - Point.y) + 
				(PointStart.x - Point.x) * (PointStart.x - Point.x);
		}
		return (PointStart.x - Point.x) * (PointStart.x - Point.x);
	} 
	else  // 斜线处理
	{
		LONG X0 = 0, Y0 = 0, X1 = 0, Y1 = 0;
		if (PointStart.x <= PointEnd.x)
		{
			X0 = PointStart.x;
			Y0 = PointStart.y;
			X1 = PointEnd.x;
			Y1 = PointEnd.y;
		}
		else
		{
			X0 = PointEnd.x;
			Y0 = PointEnd.y;
			X1 = PointStart.x;
			Y1 = PointStart.y;
		}

		float K = (float)(Y1 - Y0)/(float)(X1 - X0);
		float X = (K * K * X0 + K * (Point.y - Y0) + Point.x)/(1 + K * K);
		float Y = K*(X - X0) + Y0;

		if (X <= X0)
		{
			return (LONG)((Point.x - X0) * (Point.x - X0) + (Point.y - Y0) * (Point.y - Y0));
		}	   

		if (X >= X1)
		{
			return (LONG)((Point.x - X1) * (Point.x - X1) + (Point.y - Y1) * (Point.y - Y1));
		}
		return (LONG)((Point.x - X) * (Point.x - X) + (Point.y - Y) * (Point.y - Y));
	}
}

// 判断点A是否靠近点B
BOOL CIADrawer::IsCloseToPoint(POINT pointA, POINT pointB)
{
	LONG width  = pointA.x - pointB.x;
	LONG height = pointA.y - pointB.y;
	return ((width * width + height * height) < CLOSE_TO_LINE_DISTANCE * 2);
}

// 判断点是否在图像界内
BOOL CIADrawer::IsPointInside(const POINT *point)
{
	return ((0 <= point->x) && (point->x <= ((LONG)m_ulDCWidth)) 
			&& (0 <= point->y) && (point->y <= ((LONG)m_ulDCHeight)));
}

// 判断点是否在非法区域：寻找相应的RelativeGraphicGroup及与其的RelationshipType
BOOL CIADrawer::IsInForbiddenArea(POINT point, ULONG ulGraphicGroup)
{
	BOOL bRet = FALSE;
	CAutoLock Lock(m_Mutex);
	if (m_IARelationShipList.empty())
	{
		return bRet;
	}
	IA_RELATIONS_LIST_ITER RelationIter    = m_IARelationShipList.begin();
	IA_RELATIONS_LIST_ITER RelationIterEnd = m_IARelationShipList.end();
	while (RelationIter != RelationIterEnd)
	{
		IA_RELATIONS *pRelation = *RelationIter;
		if (NULL == pRelation)
		{
			RelationIter = m_IARelationShipList.erase(RelationIter);
			continue;
		}
        RelationIter++;  // modify by xiongfeng 防止死循环
		if (pRelation->ulCurrentGraphicGroup != ulGraphicGroup)
		{
			continue;
		}
		switch (pRelation->ulRelationShipType)
		{
		case IA_GRAPHICS_RELATION_TYPE_BE_COVERED:
			bRet = !IsPointInForbiddenArea(point, pRelation->ulRelativeGraphicGroup, TRUE);
			break;
		case IA_GRAPHICS_RELATION_TYPE_COVER:
		case IA_GRAPHICS_RELATION_TYPE_NOT_CROSS:
			bRet = IsPointInForbiddenArea(point, pRelation->ulRelativeGraphicGroup, FALSE);
			break;
		default:  //其他类型不需处理
			break;
		}
	}
	return bRet;
}

// 判断点是否在非法区域（内部）：判断点是否在RelativeGraphicGroup图形中
BOOL CIADrawer::IsPointInForbiddenArea(POINT point, ULONG ulGraphicGroup, BOOL bIsIn)
{
	BOOL bFind = FALSE;
	if (m_IADrawGraphicsList.empty())
	{
		return bIsIn;
	}
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while(iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}
		iter++;

		if (ulGraphicGroup != pIAGraphic->iaGraphic.ulGraphicGroup) 
		{
			continue;
		}
		bFind = TRUE;
		BOOL bRet = IsInShape(pIAGraphic,point);
		CHECK_RESULT(bRet, TRUE);
	}
	return (bIsIn && !bFind);
}


// 判断线段是否与非法区域相交：寻找相应的RelativeGraphicGroup及与其的RelationshipType
BOOL CIADrawer::IsCrossForbiddenArea(POINT newPoint, POINT lastPoint, ULONG ulGraphicGroup)
{
	CAutoLock Lock(m_Mutex);
	if (m_IARelationShipList.empty())
	{
		return FALSE;
	}
	IA_RELATIONS_LIST_ITER RelationIter    = m_IARelationShipList.begin();
	IA_RELATIONS_LIST_ITER RelationIterEnd = m_IARelationShipList.end();
	while (RelationIterEnd != RelationIter)
	{
		IA_RELATIONS *pRelation = *RelationIter;
		if (NULL == pRelation)
		{
			RelationIter = m_IARelationShipList.erase(RelationIter);
			continue;
		}
		RelationIter++;

		if (pRelation->ulCurrentGraphicGroup != ulGraphicGroup)
		{
			continue;
		}	
		BOOL bRet = IsLineCrossForbiddenArea(newPoint, lastPoint, 
					pRelation->ulRelativeGraphicGroup);
		CHECK_RESULT(bRet, TRUE);

	}
	return FALSE;

}

// 判断线段是否与非法区域相交（内部）：判断新的一条边是否与RelativeGraphicGroup图形相交
BOOL CIADrawer::IsLineCrossForbiddenArea(POINT newPoint, POINT lastPoint, ULONG ulGraphicGroup)
{
	if (m_IADrawGraphicsList.empty())
	{
		return FALSE;
	}
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while(iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}
		iter++;

		if (pIAGraphic->iaGraphic.ulGraphicGroup != ulGraphicGroup)
		{
			continue;
		}
		for(ULONG i = 0; i < pIAGraphic->iaGraphic.ulPointsNum - 1; i++)
		{
			BOOL bRet = IsLineSegmentCross(newPoint, lastPoint, 
						pIAGraphic->iaGraphic.IAPoints[i], 
						pIAGraphic->iaGraphic.IAPoints[i + 1]);
			CHECK_RESULT(bRet, TRUE);
		}
	}
	return FALSE;

}

// 判断特定的图形是否满足与其他图形位置关系：寻找相应的RelativeGraphicGroup及与其的RelationshipType
BOOL CIADrawer::IsForbiddenOverlaped(const POINT *pPoint, ULONG ulPointNum, ULONG ulGraphicGroup, ULONG ulGraphicType)
{
	CAutoLock Lock(m_Mutex);
	if ((NULL == pPoint) || (2 > ulPointNum))
	{
		return FALSE;
	}
	IA_RELATIONS_LIST_ITER RelationIter    = m_IARelationShipList.begin();
	IA_RELATIONS_LIST_ITER RelationIterEnd = m_IARelationShipList.end();
	
	while (RelationIter != RelationIterEnd)
	{
		IA_RELATIONS *pRelation = *RelationIter;
		if (NULL == pRelation)
		{
			RelationIter = m_IARelationShipList.erase(RelationIter);
			continue;
		}
        RelationIter++;
		if (ulGraphicGroup != pRelation->ulCurrentGraphicGroup)
        {
            continue;
        }

		switch (pRelation->ulRelationShipType)
		{
		case IA_GRAPHICS_RELATION_TYPE_NONE:
		case IA_GRAPHICS_RELATION_TYPE_SMALLER:
		case IA_GRAPHICS_RELATION_TYPE_BIGGER:
			continue;
		default: //其他类型需要处理
			break;
		}
		BOOL bRet = IsAreaForbiddenOverlaped(pPoint, ulPointNum, 
			pRelation->ulRelativeGraphicGroup, ulGraphicType, 
			pRelation->ulRelationShipType);
		CHECK_RESULT(bRet, TRUE);
	}
	return FALSE;

}

// 判断特定的图形是否满足与其他图形位置关系（内部）：遍历IADrawGraphicsList，找到RelativeGraphicGroup的图形
BOOL CIADrawer::IsAreaForbiddenOverlaped(const POINT *pPoint, 
										 ULONG ulPointNum, 
										 ULONG ulGraphicGroup, 
										 ULONG ulGraphicType, 
										 RELATION_SHIP_TYPE ulRelationShipType)
{
	// 入参在上层中保证不为空
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while (iter != iterEnd)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}
        iter++;
		if ((ulGraphicGroup != pIAGraphic->iaGraphic.ulGraphicGroup) 
			|| ((IA_GRAPHICS_TYPE_RECTANGLE != pIAGraphic->iaGraphic.ulGraphicType) 
				&& (IA_GRAPHICS_TYPE_POLYGON != pIAGraphic->iaGraphic.ulGraphicType)))
			continue;

		BOOL bisInShape = IsShapeForbiddenOverlaped(pIAGraphic, pPoint, 
							ulPointNum, ulGraphicType, ulRelationShipType);
		CHECK_RESULT(bisInShape, TRUE);
		
	}
	return FALSE;
}

// 判断特定的图形是否满足与其他图形位置关系（内部）：判断当前图形是否与RelativeGraphicGroup图形非法重叠
BOOL CIADrawer::IsShapeForbiddenOverlaped(const IA_INNER_GRAPHICS *pIAGraphic, 
										  const POINT *pPoint, 
										  ULONG ulPointNum, 
										  ULONG ulGraphicType, 
										  RELATION_SHIP_TYPE ulRelationShipType)
{
	// 入参在上层中保证不为空
	for (ULONG i = 0; i < pIAGraphic->iaGraphic.ulPointsNum -1; i++)
	{
		// 判断RelativeGraphicGroup图形的点是否在当前图形中
		BOOL bisInShape = FALSE;
		switch (ulGraphicType)
		{
		case IA_GRAPHICS_TYPE_RECTANGLE:
			bisInShape = IsPointInRectangle(pPoint, ulPointNum, 
				pIAGraphic->iaGraphic.IAPoints[i]);
			break;
		case IA_GRAPHICS_TYPE_POLYGON:
			bisInShape = IsPointInPolygon(pPoint, ulPointNum, 
				pIAGraphic->iaGraphic.IAPoints[i]);
			break;
		default:  //其他类型不需处理
			break;
		}
		if (IA_GRAPHICS_RELATION_TYPE_COVER == ulRelationShipType)
		{
			bisInShape = !bisInShape;
		}
		CHECK_RESULT(bisInShape, TRUE);  // 存在图形的点在非法区域，返回图形非法重叠

		// 判断被包含类型的当前图形的点是否在RelativeGraphicGroup图形中
		//仅需判断一个点即可，后面有相交判断
		BOOL bRet = (!IsInShape(pIAGraphic, pPoint[0]) 
					&& (IA_GRAPHICS_RELATION_TYPE_BE_COVERED == ulRelationShipType));
		CHECK_RESULT(bRet, TRUE);// 被包含的图形存在不在相关图形内的点

		// 判断RelativeGraphicGroup图形的各条边是否与当前图形相交
		for (ULONG j = 0; j < ulPointNum - 1; j++)
		{
			bRet = IsLineSegmentCross(pPoint[j], pPoint[j + 1], 
					pIAGraphic->iaGraphic.IAPoints[i], 
					pIAGraphic->iaGraphic.IAPoints[i + 1]);
			CHECK_RESULT(bRet, TRUE);   // 存在图形的线段与非法区域交叉，返回图形非法重叠
		}
	}
	return FALSE; // 未找到非法重叠的情况
}

// 判断矩形长宽是否合法：寻找相应的RelativeGraphicGroup及与其的RelationshipType
BOOL CIADrawer::IsRectangleSizeIllegal(const POINT *pPoint, 
									   ULONG ulPointNum, 
									   ULONG ulGraphicGroup)
{
	CAutoLock Lock(m_Mutex);
	if (NULL == pPoint)
	{
		return FALSE;
	}
	IA_RELATIONS_LIST_ITER RelationIter    = m_IARelationShipList.begin();
	IA_RELATIONS_LIST_ITER RelationIterEnd = m_IARelationShipList.end();

	while (RelationIterEnd != RelationIter)
	{
		IA_RELATIONS *pRelation = *RelationIter;
		if (NULL == pRelation)
		{
			RelationIter = m_IARelationShipList.erase(RelationIter);
			continue;
		}
		RelationIter++;

		if (ulGraphicGroup != pRelation->ulCurrentGraphicGroup)
			continue;
		
		BOOL bRectangleSizeIllegal = FALSE;
		switch (pRelation->ulRelationShipType)
		{
		case IA_GRAPHICS_RELATION_TYPE_SMALLER:
		case IA_GRAPHICS_RELATION_TYPE_BIGGER:
			bRectangleSizeIllegal = IsRectangleEdgeIllegal(pPoint, ulPointNum, 
									pRelation->ulRelativeGraphicGroup, 
									pRelation->ulRelationShipType);
			break;
		default://其他类型需要处理
			continue;
		}
		CHECK_RESULT(bRectangleSizeIllegal, TRUE);
	}
	return FALSE;
}

// 判断矩形长宽是否合法（内部）：遍历IADrawGraphicsList，判断该图形是否与RelativeGraphicGroup图形大小关系正确
BOOL CIADrawer::IsRectangleEdgeIllegal(const POINT *pPoint, 
										ULONG ulPointNum, 
										ULONG ulGraphicGroup, 
										RELATION_SHIP_TYPE ulRelationShipType)
{
	// 入参在上层中保证不为空
	IA_GRAPHICS_LIST_ITER iter    = m_IADrawGraphicsList.begin();
	IA_GRAPHICS_LIST_ITER iterEnd = m_IADrawGraphicsList.end();
	while (iterEnd != iter)
	{
		IA_INNER_GRAPHICS *pIAGraphic = *iter;
		if (NULL == pIAGraphic)
		{
			iter = m_IADrawGraphicsList.erase(iter);
			continue;
		}
		iter++;

		if ((ulGraphicGroup != pIAGraphic->iaGraphic.ulGraphicGroup) 
			|| (IA_GRAPHICS_TYPE_RECTANGLE != pIAGraphic->iaGraphic.ulGraphicType))
			continue;
		
		RECT rcCurrent = {0};

		//modify by limingjie lWX175483:2013-5-24
		//加入返回值判断

		(void)FindEdgeRectangle(pPoint, ulPointNum, rcCurrent);

		RECT rcRelated = {0};

		(void)FindEdgeRectangle(pIAGraphic->iaGraphic.IAPoints, 
			pIAGraphic->iaGraphic.ulPointsNum, rcRelated);


		LONG lCurrentWeith  = rcCurrent.right  - rcCurrent.left;
		LONG lCurrentHeight = rcCurrent.bottom - rcCurrent.top;
		LONG lRelatedWeith  = rcRelated.right  - rcRelated.left;
		LONG lRelatedHeight = rcRelated.bottom - rcRelated.top;

		BOOL bRet = FALSE;
		bRet = (IA_GRAPHICS_RELATION_TYPE_BIGGER == ulRelationShipType) 
				&& ((lCurrentWeith < lRelatedWeith) 
					|| (lCurrentHeight < lRelatedHeight));
		CHECK_RESULT(bRet, TRUE);

		bRet = (IA_GRAPHICS_RELATION_TYPE_SMALLER == ulRelationShipType) 
				&& ((lCurrentWeith > lRelatedWeith) 
					|| (lCurrentHeight > lRelatedHeight));
		CHECK_RESULT(bRet, TRUE);
	}
	return FALSE;
}




// 寻找包含特定图形的最小矩形框
BOOL CIADrawer::FindEdgeRectangle(const POINT *pPoint, ULONG ulPointNum, RECT &rc)
{
	rc.left   = 0x7FFFFFFF;
	rc.right  = 0;
	rc.top    = 0x7FFFFFFF;
	rc.bottom = 0;
	for (ULONG i = 0; i < ulPointNum; i++)
	{
		rc.left   = min(rc.left, pPoint[i].x);
		rc.right  = max(rc.right, pPoint[i].x);
		rc.top    = min(rc.top, pPoint[i].y);
		rc.bottom = max(rc.bottom,pPoint[i].y);
	}
	CHECK_RESULT((rc.left <= rc.right) && (rc.top <= rc.bottom), TRUE);

	return FALSE;
}

// 显示图形内半透明填充区域
VOID CIADrawer::ShowAlphaBlendArea(HDC hDC, const IA_INNER_GRAPHICS &pGraphics)
{
	RECT rc;
	if (!FindEdgeRectangle(pGraphics.iaGraphic.IAPoints, pGraphics.iaGraphic.ulPointsNum, rc))
	{
		return;
	}
	for (UINT32 y = 0; y < (UINT32)m_ulDCHeight; y++)
		for (UINT32 x = 0; x < (UINT32)m_ulDCWidth; x++)
			((UINT32 *)m_pvBits)[x + y * (UINT32)m_ulDCWidth] = 0x00000000; 
	const IA_GRAPHICS &rGraphic = pGraphics.iaGraphic;
	COLORREF color  = RGB(rGraphic.LineColor.Red,
						  rGraphic.LineColor.Green, 
						  rGraphic.LineColor.Blue);
	HBRUSH brush    = CreateSolidBrush(color);
	HPEN   pen      = CreatePen(PS_SOLID, 1, color);
	HPEN   oldPen   = (HPEN)SelectObject(m_memhdc, (HGDIOBJ)pen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(m_memhdc, (HGDIOBJ)brush);
	HRGN hPolygonRgn = NULL;
	LONG lWidth = rc.right - rc.left;
	LONG lHeight = rc.bottom - rc.top;

	hPolygonRgn = CreatePolygonRgn(pGraphics.iaGraphic.IAPoints, 
		(int)pGraphics.iaGraphic.ulPointsNum - 1, ALTERNATE);
	if (NULL == hPolygonRgn)
	{
		SelectObject(m_memhdc,oldPen);
		SelectObject(m_memhdc,oldBrush);
		DeleteObject(pen);
		DeleteObject(brush);
		return;
	}
	if (!FillRgn(m_memhdc, hPolygonRgn, brush))
	{
		SelectObject(m_memhdc,oldPen);
		SelectObject(m_memhdc,oldBrush);
		DeleteObject(pen);
		DeleteObject(brush);
		DeleteObject(hPolygonRgn);
		return;
	}

	
	UINT32 uAlpha = (UINT32)pGraphics.iaGraphic.BgColor.Alpha << 24;
	UINT32 BitmapBottom = (UINT32)m_ulDCHeight - (UINT32)rc.top;

	for (UINT32 y = (UINT32)m_ulDCHeight - (UINT32)rc.bottom; y < BitmapBottom; y++)
	{
		UINT32 *pLineStart = (UINT32 *)m_pvBits + y * (UINT32)m_ulDCWidth;
		for (UINT32 x = (UINT32)rc.left; x < (UINT32)rc.right; x++)
		{
			register UINT32 *ptr = pLineStart + x;
			register UINT32  val = *ptr;
			if (val)
			{
				*ptr = val| uAlpha; 
			}
		}
	}
	m_bf.BlendOp = AC_SRC_OVER;
	m_bf.BlendFlags = 0;
	m_bf.SourceConstantAlpha = pGraphics.iaGraphic.BgColor.Alpha;  
	m_bf.AlphaFormat = AC_SRC_ALPHA ;  

	AlphaBlend(hDC, rc.left, rc.top, lWidth, lHeight, m_memhdc, rc.left, rc.top, lWidth, lHeight, m_bf);

	SelectObject(m_memhdc,oldPen);
	SelectObject(m_memhdc,oldBrush);
	DeleteObject(pen);	
	DeleteObject(brush);
	DeleteObject(hPolygonRgn);
	
}


// 创建兼容内存DC和与原图像相同大小的位图
LONG CIADrawer::CreatBitmapDC(HDC hDC) 
{
	if (0 == m_ulWndHeight)
	{
		IVS_LOG(IVS_LOG_ERR, "Creat Bitmap DC", "MsgWnd Null");
		return IVS_FAIL;
	}

	if (NULL != m_memhdc)
	{
		DeleteDC(m_memhdc);
		m_memhdc = NULL;
	}
	// create a DC for our bitmap -- the source DC for AlphaBlend 
	m_memhdc = CreateCompatibleDC(hDC);
	if (NULL == m_memhdc)
	{
		IVS_LOG(IVS_LOG_ERR, "Creat Bitmap DC", "No Enough Memory");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	ZeroMemory(&m_bmi, sizeof(BITMAPINFO));

	// setup bitmap info 
	// set the bitmap width and height to 60% of the width and height 
	// of each of the three horizontal areas. Later on, the blending 
	// will occur in the center of each of the three areas.
	m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmi.bmiHeader.biWidth = (LONG)m_ulDCWidth;
	m_bmi.bmiHeader.biHeight = (LONG)m_ulDCHeight;
	m_bmi.bmiHeader.biPlanes = 1;
	m_bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
	m_bmi.bmiHeader.biCompression = BI_RGB;
	m_bmi.bmiHeader.biSizeImage = (DWORD)(m_ulDCWidth * m_ulDCHeight * 4);

	// create our DIB section and select the bitmap into the dc
	m_hbitmap = CreateDIBSection(m_memhdc, &m_bmi, DIB_RGB_COLORS, &m_pvBits, NULL, 0x0);

	if ((NULL == m_hbitmap) || (NULL == m_pvBits))
	{
		IVS_LOG(IVS_LOG_ERR, "Creat Bitmap DC", "No Enough Memory");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	SelectObject(m_memhdc, m_hbitmap);
	for (UINT32 y = 0; y < (UINT32)m_ulDCHeight; y++)
		for (UINT32 x = 0; x < (UINT32)m_ulDCWidth; x++)
			((UINT32 *)m_pvBits)[x + y * (UINT32)m_ulDCWidth] = 0x00000000; 

	return IVS_SUCCEED;
}


LONG CIADrawer::CreateIAFont() 
{
	if (NULL != m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}

	/****************** Windows程序设计提供的算法****************************************
	//字体点值与设备的「逻辑分辨率」相吻合
	FLOAT cyDpi = (FLOAT) GetDeviceCaps (hDC, LOGPIXELSY) ;
	//字体点值与设备的「逻辑分辨率」不吻合
	FLOAT cyDpi = (FLOAT) (25.4 * GetDeviceCaps (hDC, VERTRES) / GetDeviceCaps (hDC, VERTSIZE)) ;

	FLOAT y = 200 * cyDpi / 72;
	m_lf.lfHeight = - (int)(fabs(y) / 10.0 + 0.5) ;
	*********************************************************************************/


	/*************************MSDN提供的算法*******************************************
	m_lf.lfHeight = -MulDiv(20, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	*********************************************************************************/

	// 	INT iHeight = (INT)(15 * m_lLineWidthAdjust);
	// 	m_lf.lfHeight = -MulDiv(iHeight, GetDeviceCaps(hDC, LOGPIXELSY), 72);

	memset(&m_lf, 0, sizeof(LOGFONT));
	m_lf.lfHeight = - m_lFontSize;
	if (12 > m_lFontSize)
	{
		m_lf.lfWeight = FW_LIGHT;
	} else if (20 > m_lFontSize)
	{
		m_lf.lfWeight = FW_MEDIUM;
	} else if (30 > m_lFontSize){
		m_lf.lfWeight = FW_SEMIBOLD;
	} else {
		m_lf.lfWeight = FW_BOLD;
	}
	m_lf.lfCharSet = DEFAULT_CHARSET;
	memcpy_s(m_lf.lfFaceName, sizeof(m_lf.lfFaceName), "Arial\0", strlen("Arial\0"));

	m_hFont = CreateFontIndirect(&m_lf);  
	if (NULL == m_hFont)
	{
		IVS_LOG(IVS_LOG_ERR, "Creat Bitmap DC", "CreatFont Error");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	return IVS_SUCCEED;
}


