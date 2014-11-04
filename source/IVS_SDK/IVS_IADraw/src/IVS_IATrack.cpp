#include "IVS_IATrack.h"
#include "IVSCommon.h"
#include "ivs_log.h"
#include "ivs_error.h"
#include <windows.h>

#define IA_BASE_LINE_WIDTH 3	// 基本线宽
#define IA_BASE_FONT_SIZE 20	// 基本字体大小
#define IA_FRAMEID_OFFSET_MAX 200	// 显示轨迹时最大时间偏差
#define IA_ROI_LAST_TIME  60000  // 实况敏感区停留最长时间（单位：ms）
#define IA_ALARM_LAST_TIME  500  // 告警区停留最长时间（单位：ms）

#define IA_QUERY_TYPE_REALTIME	1
// #define IA_QUERY_TYPE_RECORD	2


CIATrack::CIATrack()
	:m_bInitTag(FALSE)
	,m_ulCurrentFrameID(0)
	,m_IAROIListLength(0)
	,m_IAAlarmedListLength(0)
	,m_hMsgWnd(NULL)
	,m_ulWndWidth(0)
	,m_ulWndHeight(0)
	,m_ulDCWidth(0)
	,m_ulDCHeight(0)
	,m_lLineWidthAdjust(1)
	,m_ulLineWidth(0)
	,m_ulVideoType(0)
	,m_hFont(NULL)
	,m_ulCurrentFontSize(0)
{
	memset(&m_lf, 0, sizeof(LOGFONT));
}

CIATrack::~CIATrack()
{
	if (NULL != m_hFont)
	{
		DeleteObject(m_hFont); 
		m_hFont = NULL;
	}

}

// 初始化智能分析轨迹
LONG CIATrack::InitIATrack(HWND hMsgWnd, ULONG ulVideoType)
{
	if (m_bInitTag)
	{
		IVS_LOG(IVS_LOG_ERR, "Init IA Track", "[%x]Already Inited", this);
		return IVS_FAIL;
	}
	if (!IsWindow(hMsgWnd))
	{
		IVS_LOG(IVS_LOG_ERR, "Init IA Track", "[%x]hMsgWnd NULL", this);
		return IVS_PARA_INVALID;
	}

	m_hMsgWnd				= hMsgWnd;	// 窗口句柄
	RECT Wndrect;
    memset(&Wndrect, 0, sizeof(Wndrect));
	GetClientRect(m_hMsgWnd, &Wndrect); 
	m_ulWndWidth  = (ULONG)(Wndrect.right  - Wndrect.left);		// 窗口宽度
	m_ulWndHeight = (ULONG)(Wndrect.bottom - Wndrect.top);		// 窗口高度
	// 判断窗口长宽是否为0
	if (( 0 == m_ulWndWidth) || (0 == m_ulWndHeight))
	{
		IVS_LOG(IVS_LOG_ERR, "Init IA Track", "[%x]hMsgWnd Size 0", this);
		return IVS_FAIL;
	}

	m_ulCurrentFrameID		= 0;		// 当前时间戳
	m_IAROIListLength		= 0;		// ROI轨迹消息列表长度
	m_IAAlarmedListLength	= 0;		// Alarmed轨迹消息列表长度
	m_ulDCWidth				= 0;		// 显示图像宽度
	m_ulDCHeight			= 0;		// 显示图像高度
	m_lLineWidthAdjust		= 1;		// 线宽系数，用于分辨率调节
	m_ulLineWidth			= IA_BASE_LINE_WIDTH;		// 图形线宽
	m_ulVideoType			= ulVideoType;
	m_hFont					= NULL;		// 字体句柄
	m_ulCurrentFontSize		= 0;		// 当前字体大小
	m_IAROIList.clear();
	m_IAAlarmedList.clear();
	memset(&m_lf, 0, sizeof(LOGFONT));

	m_bInitTag				= TRUE;
	return IVS_SUCCEED;
}

// 插入智能分析轨迹信息
LONG CIATrack::InsertIALayoutMessage(const IA_LAYOUT_MESSAGE *LayoutMessage)
{
	if (NULL == LayoutMessage)
	{
		IVS_LOG(IVS_LOG_ERR, "Insert IA Layout Message", "[%x]LayoutMessage is NULL", this);
		return IVS_PARA_INVALID;
	}
	IVS_LOG(IVS_LOG_DEBUG, "Insert IA Layout Message", 
		"[%x]Current FrameID: %I64u, Insert FrameID: %I64u", 
		this, m_ulCurrentFrameID, LayoutMessage->ulFrameID);
	IVS_LOG(IVS_LOG_DEBUG, "Insert IA Layout Message", 
		"[%x]Is Percent: %lu", this, LayoutMessage->LayoutElement->AlarmPolyline.ulIsPercent);

	LONG lRet = IVS_FAIL;

	// 删除旧的FrameID节点
	DeleteExpiredMessage(m_IAROIList, m_IAROIListLength, FALSE);
	DeleteExpiredMessage(m_IAAlarmedList, m_IAAlarmedListLength, TRUE);
	
	IA_LAYOUT_MESSAGE* pInputLayoutMessage = (IA_LAYOUT_MESSAGE*)IVS_NEW((CHAR* &)pInputLayoutMessage, sizeof(IA_LAYOUT_MESSAGE));//lint !e826
	if (NULL == pInputLayoutMessage)
	{
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(pInputLayoutMessage, 0, sizeof(IA_LAYOUT_MESSAGE));
	memcpy_s(pInputLayoutMessage, sizeof(IA_LAYOUT_MESSAGE), 
		LayoutMessage, sizeof(IA_LAYOUT_MESSAGE));


	// 新建两个IA_LAYOUT_MESSAGE对象，分别存储ROI和Alarmed信息
	IA_LAYOUT_MESSAGE* pROIMessage = (IA_LAYOUT_MESSAGE*)IVS_NEW((CHAR* &)pROIMessage, sizeof(IA_LAYOUT_MESSAGE));//lint !e826
	if (NULL == pROIMessage)
	{
		IVS_DELETE(pInputLayoutMessage, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	IA_LAYOUT_MESSAGE* pAlarmedMessage = (IA_LAYOUT_MESSAGE*)IVS_NEW((CHAR* &)pAlarmedMessage, sizeof(IA_LAYOUT_MESSAGE));//lint !e826
	if (NULL == pAlarmedMessage)
	{
		IVS_DELETE(pInputLayoutMessage, MUILI);
		IVS_DELETE(pROIMessage, MUILI);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(pROIMessage, 0, sizeof(IA_LAYOUT_MESSAGE));
	pROIMessage->ulFrameID     = pInputLayoutMessage->ulFrameID;
	pROIMessage->ulFrameWidth  = pInputLayoutMessage->ulFrameWidth;
	pROIMessage->ulFrameHeight = pInputLayoutMessage->ulFrameHeight;
	memcpy_s(pAlarmedMessage, sizeof(IA_LAYOUT_MESSAGE), pROIMessage, sizeof(IA_LAYOUT_MESSAGE));
	
	// 对传入的轨迹信息结构体进行ROI/Alarmed拆分
	for (ULONG i = 0; i < pInputLayoutMessage->ulLayoutElementNum; i++)
	{
		// 对Element进行点的坐标转换
		lRet = CoordinateTransform(pInputLayoutMessage->LayoutElement[i], 
			pInputLayoutMessage->ulFrameWidth, pInputLayoutMessage->ulFrameHeight);
		if (IVS_SUCCEED != lRet)
		{
			IVS_DELETE(pInputLayoutMessage, MUILI);
			IVS_DELETE(pROIMessage, MUILI);
			IVS_DELETE(pAlarmedMessage, MUILI);
			return lRet;
		}

		if (IA_LAYER_TYPE_ROI == pInputLayoutMessage->LayoutElement[i].ulLayer)
		{
			memcpy_s(&pROIMessage->LayoutElement[pROIMessage->ulLayoutElementNum], 
					sizeof(IA_LAYOUT_MESSAGE), 
					&pInputLayoutMessage->LayoutElement[i], 
					sizeof(IA_LAYOUT_ELEMENT));
			pROIMessage->ulLayoutElementNum++;
			IVS_LOG(IVS_LOG_DEBUG, "Insert IA Layout Message", "[%x]Insert ROI", this);
			POINT Points[IA_POINTS_NUM_MAX];
			memcpy_s(Points, sizeof(POINT) * IA_POINTS_NUM_MAX, 
				pROIMessage->LayoutElement[0].AlarmPolyline.Points, sizeof(POINT) * IA_POINTS_NUM_MAX);
			IVS_LOG(IVS_LOG_DEBUG, "Insert IA Layout Message", 
				"[%x]ROI Points: (%d,%d); (%d,%d); (%d,%d); (%d,%d).", this, 
				Points[0].x, Points[0].y, Points[1].x, Points[1].y, 
				Points[2].x, Points[2].y, Points[3].x, Points[3].y);

		}
		if (IA_LAYER_TYPE_ALARM == pInputLayoutMessage->LayoutElement[i].ulLayer)
		{
			memcpy_s(&pAlarmedMessage->LayoutElement[pAlarmedMessage->ulLayoutElementNum], 
					sizeof(IA_LAYOUT_MESSAGE), 
					&pInputLayoutMessage->LayoutElement[i], 
					sizeof(IA_LAYOUT_ELEMENT));
			pAlarmedMessage->ulLayoutElementNum++;
			IVS_LOG(IVS_LOG_DEBUG, "Insert IA Layout Message", "[%x]Insert Alarm", this);
			POINT Points[IA_POINTS_NUM_MAX];
			memcpy_s(Points, sizeof(POINT) * IA_POINTS_NUM_MAX, 
				pAlarmedMessage->LayoutElement[0].AlarmPolyline.Points, sizeof(POINT) * IA_POINTS_NUM_MAX);
			IVS_LOG(IVS_LOG_DEBUG, "Insert IA Layout Message", 
				"[%x]Alarmed Points: (%d,%d); (%d,%d); (%d,%d); (%d,%d).", this, 
				Points[0].x, Points[0].y, Points[1].x, Points[1].y, 
				Points[2].x, Points[2].y, Points[3].x, Points[3].y);
		}
	}

	// 分别插入到ROI/Alarmed列表
	lRet = IVS_SUCCEED;
	if (pROIMessage->ulLayoutElementNum) // LayoutMessage中存在ROI层
	{
		lRet = InsertMessageIntoList(*pROIMessage, m_IAROIList, m_IAROIListLength);
		if (IVS_SUCCEED != lRet)
		{
			IVS_DELETE(pInputLayoutMessage, MUILI);
			IVS_DELETE(pROIMessage, MUILI);
			IVS_DELETE(pAlarmedMessage, MUILI);
			return lRet;
		}
	}
	
	if (pAlarmedMessage->ulLayoutElementNum) // LayoutMessage中存在Alarmed层
	{
		lRet = InsertMessageIntoList(*pAlarmedMessage, m_IAAlarmedList, m_IAAlarmedListLength);
	}

	IVS_DELETE(pInputLayoutMessage, MUILI);
	IVS_DELETE(pROIMessage, MUILI);
	IVS_DELETE(pAlarmedMessage, MUILI);
	return lRet;
}

// 显示智能分析轨迹信息
LONG CIATrack::ShowIATrack(HDC hDC, const RECT *DCRect, ULONG64 ulFrameID)
{
	if (NULL == DCRect)
	{
		IVS_LOG(IVS_LOG_ERR, "Show IA Track", "[%x]Invalid Input", this);
		return IVS_PARA_INVALID;
	}
// 	IVS_LOG(IVS_LOG_INFO, "Show IA Track", "[%x]Current FrameID: %I64u", this, ulFrameID);

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
		IVS_LOG(IVS_LOG_ERR, "Show IA Track", "[%x]hMsgWnd Size 0", this);
		return IVS_FAIL;
	}

	// 更新图像大小
	ULONG DCWidth  = (ULONG)(DCRect->right - DCRect->left);
	ULONG DCHeight = (ULONG)(DCRect->bottom - DCRect->top);
	if ((DCWidth != m_ulDCWidth) || (DCHeight != m_ulDCHeight))
	{
		m_ulDCWidth  = DCWidth;
		m_ulDCHeight = DCHeight;
 		IVS_LOG(IVS_LOG_INFO, "Show IA Track", "[%x]Image Size Changed", this);
	}
	// 判断绘图DC长宽是否为0
	if (( 0 == m_ulDCWidth) || (0 == m_ulDCHeight))
	{
		IVS_LOG(IVS_LOG_ERR, "Show IA Track", "[%x]DC Size 0", this);
		return IVS_FAIL;
	}

	if (m_ulDCWidth >= m_ulWndWidth)
	{
		m_lLineWidthAdjust = (LONG)max(1, m_ulDCWidth / m_ulWndWidth);
	} else {
		m_lLineWidthAdjust = - (LONG)(m_ulWndWidth / m_ulDCWidth);
	}
	m_ulLineWidth = (ULONG)max(1, IA_BASE_LINE_WIDTH + m_lLineWidthAdjust);


	if (0 != ulFrameID)
	{
		// 更新当前FrameID
		m_ulCurrentFrameID = ulFrameID;
		// 删除FrameID过期的轨迹消息
		DeleteExpiredMessage(m_IAROIList, m_IAROIListLength, FALSE);
		DeleteExpiredMessage(m_IAAlarmedList, m_IAAlarmedListLength, TRUE);
	}

	// 显示列表中最后节点的轨迹信息
	LONG lRet = ShowIATrackMessage(hDC, m_IAROIList, FALSE); // 显示ROI列表
	if (IVS_SUCCEED != lRet) 
	{
		return lRet;
	}
	lRet = ShowIATrackMessage(hDC, m_IAAlarmedList, TRUE);// 显示Alarmed列表

	return lRet;
}



// 获取当前轨迹信息数量
LONG CIATrack::GetCurrentElementNum()
{
	return (LONG)m_IAAlarmedListLength;
}


/************************************************************************/
/*								内部实现                                  */
/************************************************************************/

// 为Message分配内存
IA_LAYOUT_MESSAGE* CIATrack::AllocateMessageMemory()
{
	IA_LAYOUT_MESSAGE* p = IVS_NEW(p);
	return p;
}

// 为Message释放内存
VOID CIATrack::ReleaseMessageMemory(IA_LAYOUT_MESSAGE* m)
{
	IVS_DELETE(m);
}


// 坐标转换
LONG CIATrack::CoordinateTransform(IA_LAYOUT_ELEMENT &LayoutElement, ULONG ulFrameWidth, ULONG ulFrameHeight)
{
	if (( 0 == ulFrameWidth) || (0 == ulFrameHeight))
	{
		IVS_LOG(IVS_LOG_ERR, "Coordinate Transform", "[%x]FrameSize 0", this);
		return IVS_PARA_INVALID;
	}

	IVS_LOG(IVS_LOG_DEBUG, "Coordinate Transform", 
		"[%x]FrameWidth: %lu, FrameHeight: %lu, DCWidth: %lu, DCHeight: %lu", 
		this, ulFrameWidth, ulFrameHeight, m_ulDCWidth, m_ulDCHeight);

	switch (LayoutElement.ulElementType)
	{
	case IA_ELEMENT_TYPE_TEXT:
		if (LayoutElement.AlarmText.ulIsPercent)
		{
			break;
		}
		LayoutElement.AlarmText.TextPoint.x = 
			LayoutElement.AlarmText.TextPoint.x * (LONG)m_ulDCWidth  / (LONG)ulFrameWidth;
		LayoutElement.AlarmText.TextPoint.y = 
			LayoutElement.AlarmText.TextPoint.y * (LONG)m_ulDCHeight / (LONG)ulFrameHeight;
		break;
	case IA_ELEMENT_TYPE_POLYLINE:
		if (LayoutElement.AlarmPolyline.ulIsPercent)
		{
			break;
		}

		for (ULONG i = 0; i < LayoutElement.AlarmPolyline.ulPointsNum; i++)
		{
			LayoutElement.AlarmPolyline.Points[i].x = 
				LayoutElement.AlarmPolyline.Points[i].x * (LONG)m_ulDCWidth  / (LONG)ulFrameWidth;
			LayoutElement.AlarmPolyline.Points[i].y = 
				LayoutElement.AlarmPolyline.Points[i].y * (LONG)m_ulDCHeight / (LONG)ulFrameHeight;
		}
		break;
	default:
		IVS_LOG(IVS_LOG_ERR, "Coordinate Transform", "[%x]ElementType Invalid", this);
		return IVS_FAIL;
	}
	return IVS_SUCCEED;
}


// 删除列表中过期消息
VOID CIATrack::DeleteExpiredMessage(IA_LAYOUT_MESSAGE_LIST &DestList, ULONG &refListLength, BOOL bIsAlarmed)
{
	CAutoLock Lock(m_Mutex);

	IA_LAYOUT_MESSAGE_LIST_REVERSE_ITER rIter = DestList.rbegin();
	IA_LAYOUT_MESSAGE_LIST_REVERSE_ITER rIterNext = DestList.rbegin();

	// 从尾向头遍历列表
	while (rIter != DestList.rend())
	{
		IA_LAYOUT_MESSAGE *pListMessage = *rIter;
		BOOL bNextExpired = FALSE; // 标记下一个节点是否过期消息
		rIterNext = rIter;
		rIterNext++;
		ULONG64 ulNextFrameID = 0;
		if (rIterNext != DestList.rend())
		{
			IA_LAYOUT_MESSAGE *pListMessageNext = *rIterNext;
			bNextExpired = pListMessageNext->ulFrameID <= m_ulCurrentFrameID ? TRUE : FALSE;
			ulNextFrameID = pListMessageNext->ulFrameID;
		}

		// Alarmed 500ms过期；ROI实况下60000ms过期，录像下保留最后一帧
		if (((pListMessage->ulFrameID  < m_ulCurrentFrameID - IA_ALARM_LAST_TIME) && bIsAlarmed) 
			|| ((pListMessage->ulFrameID  < m_ulCurrentFrameID - IA_ROI_LAST_TIME) && !bIsAlarmed && IA_QUERY_TYPE_REALTIME == m_ulVideoType) 
			|| bNextExpired)
		{
			IVS_LOG(IVS_LOG_DEBUG, "Delete Expired Message", 
				"[%x]Current FrameID is: %I64u, Deleted FrameID is: %I64u, Next FrameID is: %I64u, Layer Type is: %s", 
				this, m_ulCurrentFrameID, pListMessage->ulFrameID, ulNextFrameID, bIsAlarmed ? "Alarm" : "ROI");
			ReleaseMessageMemory(pListMessage);
			rIter = IA_LAYOUT_MESSAGE_LIST_REVERSE_ITER(DestList.erase((++rIter).base()));
			refListLength--;
			continue;
		}
		else
		{
			// 列表按FrameID从大到小排列，无需再向前比较
			break;
		}
	}
	return;
}


// 将按Layer拆分后的消息插入到对应的列表中
LONG CIATrack::InsertMessageIntoList(const IA_LAYOUT_MESSAGE &refNewMessage, IA_LAYOUT_MESSAGE_LIST &DestList, ULONG &refListLength)
{
	// 判断列表中轨迹消息达到上限
	if (IA_MESSAGE_NUM_MAX <= refListLength)
	{
		IVS_LOG(IVS_LOG_ERR, "Insert Message Into List", "[%x]Reach MAX MessageNum", this);
		return IVS_FAIL;
	}

	// 分配新节点内存，并拷贝插入节点内容
	IA_LAYOUT_MESSAGE *pMessage = NULL;
	pMessage = AllocateMessageMemory();
	if (NULL == pMessage)
	{
		IVS_LOG(IVS_LOG_ERR, "Insert Message Into List", "[%x]No Enough Memory", this);
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memcpy_s(pMessage, sizeof(IA_LAYOUT_MESSAGE), 
			&refNewMessage, sizeof(IA_LAYOUT_MESSAGE));


	CAutoLock Lock(m_Mutex);
	
	// 将新轨迹消息按大小顺序插入到列表中
	IA_LAYOUT_MESSAGE_LIST_ITER Iter = DestList.begin();
	IA_LAYOUT_MESSAGE_LIST_ITER End  = DestList.end();
	for (; Iter != End; Iter++)
	{
		IA_LAYOUT_MESSAGE *pListMessage = *Iter;
		if (pListMessage->ulFrameID < pMessage->ulFrameID)
		{
			DestList.insert(Iter, pMessage);
			refListLength++;
			IVS_LOG(IVS_LOG_DEBUG, "Insert Message Into List", 
				"[%x]FrameID: %I64u Inserted", this, pMessage->ulFrameID);
			return IVS_SUCCEED;
		}
		if (pListMessage->ulFrameID == pMessage->ulFrameID)
		{
			memcpy_s(pListMessage, sizeof(IA_LAYOUT_MESSAGE), 
				pMessage, sizeof(IA_LAYOUT_MESSAGE));
			IVS_LOG(IVS_LOG_DEBUG, "Insert Message Into List", 
				"[%x]FrameID: %I64u Updated", this, pMessage->ulFrameID);
			ReleaseMessageMemory(pMessage);
			return IVS_SUCCEED;
		}
	}
	// 在列表中未找到比NewMessage的FrameID小的节点
	DestList.push_back(pMessage);
	refListLength++;
	IVS_LOG(IVS_LOG_DEBUG, "Insert Message Into List", 
		"[%x]FrameID: %I64u Inserted", this, pMessage->ulFrameID);

	return IVS_SUCCEED;
}


// 显示列表中最后节点的轨迹信息
LONG CIATrack::ShowIATrackMessage(HDC hDC, IA_LAYOUT_MESSAGE_LIST &DestList, BOOL bIsAlarmed)
{
	CAutoLock Lock(m_Mutex);

#if 0
    IA_ALARM_POLYLINE polyline;
    memset(&polyline, 0, sizeof(polyline));
    polyline.Points[0].x = 10;
    polyline.Points[0].y = 300;
    polyline.Points[1].x = 600;
    polyline.Points[1].y = 600;
    polyline.ulDirection = 3;
    polyline.ulPointsNum = 2;
    COLORREF ulColor = RGB(0,255,0);
    ShowIATrackPolyline(hDC, polyline, ulColor);
    return 0;
#endif

	if (DestList.empty())
	{
		IVS_LOG(IVS_LOG_DEBUG, "Show IA Track Message", 
			"[%x]%s List Empty", this, bIsAlarmed ? "Alarm" : "ROI");
		return IVS_SUCCEED;
	}
	IA_LAYOUT_MESSAGE_LIST_REVERSE_ITER rIter = DestList.rbegin();
	IA_LAYOUT_MESSAGE *pListMessage = *rIter;
	LONG lRet = IVS_FAIL;

#if 0
	IA_LAYOUT_MESSAGE_LIST_REVERSE_ITER rEndIter = DestList.rend();
	rEndIter--;
	IA_LAYOUT_MESSAGE *pEndListMessage = *rEndIter;

		int textlen = 0;	
		char buf[256] = {0};
		textlen = _snprintf_s(buf, 255, "m_ulCurrentFrameID:%d", m_ulCurrentFrameID);
		TextOut(hDC, 100, 25, buf, textlen);
		textlen = _snprintf_s(buf, 255, "ulFrameID                :%d", pListMessage->ulFrameID);
		TextOut(hDC, 100, 50, buf, textlen);
		textlen = _snprintf_s(buf, 255, "ulFrameIDEnd           :%d", pEndListMessage->ulFrameID);
		TextOut(hDC, 100, 75, buf, textlen);

		textlen = _snprintf_s(buf, 255, "ulFrameID - m_ulCurrentFrameID:%d", 
			pListMessage->ulFrameID - m_ulCurrentFrameID);
		TextOut(hDC, 100, 100, buf, textlen);
		textlen = _snprintf_s(buf, 255, "m_IAROIList.size: %d", 
			m_IAROIList.size());
		TextOut(hDC, 100, 150, buf, textlen);
		textlen = _snprintf_s(buf, 255, "m_IAAlarmedList.size: %d", 
			m_IAAlarmedList.size());
		TextOut(hDC, 100, 200, buf, textlen);
		int textlen = 0;	
		char buf[256] = {0};
		textlen = _snprintf_s(buf, 255, "m_ulDCWidth:%d", m_ulDCWidth);
		TextOut(hDC, 100, 25, buf, textlen);
		textlen = _snprintf_s(buf, 255, "m_ulDCHeight:%d", m_ulDCHeight);
		TextOut(hDC, 100, 50, buf, textlen);
#endif

	// 只叠加IA_FRAMEID_OFFSET_MAX毫秒之内的轨迹图形
	if ((pListMessage->ulFrameID > m_ulCurrentFrameID) 
		&& (IA_FRAMEID_OFFSET_MAX < (pListMessage->ulFrameID - m_ulCurrentFrameID)) 
		/*&& bIsAlarmed*/)
	{
		return IVS_SUCCEED;
	}
	for (ULONG i = 0; i < pListMessage->ulLayoutElementNum; i++)
	{
		lRet = ShowIATrackElement(hDC, pListMessage->LayoutElement[i]);
		if (IVS_SUCCEED != lRet)
		{
			return lRet;
		}
	}
	return IVS_SUCCEED;
}

// 显示单个轨迹Element
LONG CIATrack::ShowIATrackElement(HDC hDC, IA_LAYOUT_ELEMENT &LayoutElememt)
{
	LONG lRet = IVS_FAIL;
	switch (LayoutElememt.ulElementType)
	{
	case IA_ELEMENT_TYPE_TEXT:
		lRet = ShowIATrackText(hDC, LayoutElememt.AlarmText, LayoutElememt.ulColor);
		break;
	case IA_ELEMENT_TYPE_POLYLINE:
		lRet = ShowIATrackPolyline(hDC, LayoutElememt.AlarmPolyline, LayoutElememt.ulColor);
		break;
	default:
		IVS_LOG(IVS_LOG_ERR, "Show IA Track Element", "[%x]ElementType Error", this);
		break;
	}
	return lRet;
}


// 显示轨迹文字信息
LONG CIATrack::ShowIATrackText(HDC hDC, IA_ALARM_TEXT &AlarmText, COLORREF ulColor)
{
	// 当字体大小发生变化时再创建字体
	ULONG ulFontSize = (ULONG)max(1, IA_BASE_FONT_SIZE + m_lLineWidthAdjust) 
						* AlarmText.ulTextScale;
	if (ulFontSize != m_ulCurrentFontSize)
	{
		m_ulCurrentFontSize = ulFontSize;
		LONG lRet = CreatFont(/*AlarmText.ulTextScale*/);
		if (IVS_SUCCEED != lRet)
		{
			return lRet;
		}
	}

	IVS_LOG(IVS_LOG_DEBUG, "Show IATrack Text", 
		"[%x]Is Percent: %lu", this, AlarmText.ulIsPercent);

	// 转换百分比坐标为图像坐标
	if (AlarmText.ulIsPercent)
	{
		IVS_LOG(IVS_LOG_DEBUG, "Show IATrack Text", "[%x]Percent Point: %d, %d", 
			this, AlarmText.TextPoint.x, AlarmText.TextPoint.y);

		AlarmText.TextPoint.x = AlarmText.TextPoint.x * (LONG)m_ulDCWidth  / 100;
		AlarmText.TextPoint.y = AlarmText.TextPoint.y * (LONG)m_ulDCHeight / 100;
		AlarmText.ulIsPercent = 0;
		IVS_LOG(IVS_LOG_DEBUG, "Show IATrack Text", "[%x]After Adjust: %d, %d", 
			this, AlarmText.TextPoint.x, AlarmText.TextPoint.y);

	}

	SetTextColor(hDC, ulColor);	// 字体颜色   
	SetBkMode(hDC, TRANSPARENT);// 背景色(透明)
	SelectObject(hDC,m_hFont);

	TextOut(hDC, AlarmText.TextPoint.x, AlarmText.TextPoint.y, 
		AlarmText.TextContent, (int)AlarmText.ulTextLenth);

	return IVS_SUCCEED;
}

// 计算箭头坐标
LONG CIATrack::CalcuArrowPoints(const IA_ALARM_POLYLINE& PolyLineInfo, IA_ARROW& ArrowPointSet)
{
    memset(&ArrowPointSet, 0, sizeof(ArrowPointSet));
    LONG dStraightLineWidth  = PolyLineInfo.Points[1].x - PolyLineInfo.Points[0].x;
    LONG dStraightLineHeight = PolyLineInfo.Points[1].y - PolyLineInfo.Points[0].y;
    DOUBLE dLenth = (DOUBLE)dStraightLineWidth * (DOUBLE)dStraightLineWidth 
        + (DOUBLE)dStraightLineHeight * (DOUBLE)dStraightLineHeight;
    LONG lStraightLineLenth  = (LONG)sqrt(dLenth);

    for (unsigned int i = 1; i <= 2; i++)
    {
        if (i & PolyLineInfo.ulDirection)
        {
            ArrowPointSet.ArrowTail[i - 1].x = PolyLineInfo.Points[0].x + dStraightLineWidth  / 2;
            ArrowPointSet.ArrowTail[i - 1].y = PolyLineInfo.Points[0].y + dStraightLineHeight / 2;

            LONG dArrowWidth  = (LONG)pow((DOUBLE)-1, (DOUBLE)(i - 1)) * dStraightLineHeight * ARROW_LENTH / lStraightLineLenth;
            LONG dArrowHeight = (LONG)pow((DOUBLE)-1, (DOUBLE)(i))     * dStraightLineWidth  * ARROW_LENTH / lStraightLineLenth;

            ArrowPointSet.ArrowHead[i - 1].x = ArrowPointSet.ArrowTail[i - 1].x + dArrowWidth;
            ArrowPointSet.ArrowHead[i - 1].y = ArrowPointSet.ArrowTail[i - 1].y + dArrowHeight;

            POINT FourFifthOnArrow = {0, 0};
            FourFifthOnArrow.x = ArrowPointSet.ArrowTail[i - 1].x + dArrowWidth  * 4 / 5;
            FourFifthOnArrow.y = ArrowPointSet.ArrowTail[i - 1].y + dArrowHeight * 4 / 5;

            LONG dArrowHeadWidth  = - dArrowHeight / 5;
            LONG dArrowHeadHeight =   dArrowWidth  / 5;

            ArrowPointSet.ArrowTrangle1[i - 1].x = FourFifthOnArrow.x + dArrowHeadWidth;
            ArrowPointSet.ArrowTrangle1[i - 1].y = FourFifthOnArrow.y + dArrowHeadHeight;

            ArrowPointSet.ArrowTrangle2[i - 1].x = FourFifthOnArrow.x - dArrowHeadWidth;
            ArrowPointSet.ArrowTrangle2[i - 1].y = FourFifthOnArrow.y - dArrowHeadHeight;
        }
    }
    return IVS_SUCCEED;
}

LONG CIATrack::ShowArrowShape(HDC hDC, const IA_ARROW &iaArrow, ULONG ulArrowType)
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
    return IVS_SUCCEED;
}

// 显示轨迹图形信息
LONG CIATrack::ShowIATrackPolyline(HDC hDC, IA_ALARM_POLYLINE &AlarmPolyline, COLORREF ulColor)
{
	HBRUSH brush    = (HBRUSH)GetStockObject(NULL_BRUSH);
	HPEN   pen      = CreatePen(PS_SOLID, (int)m_ulLineWidth, ulColor);
	HPEN   oldPen   = (HPEN)SelectObject(hDC, (HGDIOBJ)pen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, (HGDIOBJ)brush);

// 	IVS_LOG(IVS_LOG_DEBUG, "Show IATrack Polyline", 
// 		"[%x]Is Percent: %lu", this, AlarmPolyline.ulIsPercent);

// 	IVS_LOG(IVS_LOG_DEBUG, "Show IATrack Polyline", "[%x]Original Point[0]: %d, %d", 
// 		this, AlarmPolyline.Points[0].x, AlarmPolyline.Points[0].y);
	// 转换百分比坐标为图像坐标
	if (AlarmPolyline.ulIsPercent)
	{
		AlarmPolyline.Points[0].x = AlarmPolyline.Points[0].x * (LONG)m_ulDCWidth  / 100;
		AlarmPolyline.Points[0].y = AlarmPolyline.Points[0].y * (LONG)m_ulDCHeight / 100;
		IVS_LOG(IVS_LOG_DEBUG, "Show IATrack Polyline", "[%x]Adjust Point[0]: %d, %d", 
			this, AlarmPolyline.Points[0].x, AlarmPolyline.Points[0].y);
	}

	MoveToEx(hDC, AlarmPolyline.Points[0].x, AlarmPolyline.Points[0].y, NULL);
	for (ULONG i = 1; i < AlarmPolyline.ulPointsNum; i++)
	{
// 		IVS_LOG(IVS_LOG_DEBUG, "Show IATrack Polyline", "[%x]Original Point[%lu]: %d, %d", 
// 			this, i, AlarmPolyline.Points[i].x, AlarmPolyline.Points[i].y);
		if (AlarmPolyline.ulIsPercent)
		{
			AlarmPolyline.Points[i].x = AlarmPolyline.Points[i].x * (LONG)m_ulDCWidth  / 100;
			AlarmPolyline.Points[i].y = AlarmPolyline.Points[i].y * (LONG)m_ulDCHeight / 100;
			IVS_LOG(IVS_LOG_DEBUG, "Show IATrack Polyline", "[%x]Adjust Point[%lu]: %d, %d", 
				this, i, AlarmPolyline.Points[i].x, AlarmPolyline.Points[i].y);
		}
		LineTo(hDC, AlarmPolyline.Points[i].x, AlarmPolyline.Points[i].y);
	}

	AlarmPolyline.ulIsPercent = 0;

    if (AlarmPolyline.ulDirection)
    {
        IA_ARROW ArrowPointSet;
        memset(&ArrowPointSet, 0, sizeof(ArrowPointSet));
        (void)CalcuArrowPoints(AlarmPolyline, ArrowPointSet);
        (void)ShowArrowShape(hDC, ArrowPointSet, AlarmPolyline.ulDirection);
    }



	// 释放资源
	SelectObject(hDC,oldPen);
	SelectObject(hDC,oldBrush);
	DeleteObject(pen);

	return IVS_SUCCEED;
}


// 创建字体
LONG CIATrack::CreatFont(/*ULONG ulTextScale*/) 
{
	if (NULL != m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}

// 	GetObject(GetStockObject(ANSI_FIXED_FONT), sizeof(LOGFONT), &m_lf); 

	/****************** Windows程序设计提供的算法****************************************
	//字体点值与设备的「逻辑分辨率」相吻合
 	FLOAT cyDpi = (FLOAT) GetDeviceCaps (hDC, LOGPIXELSY) ;
	//字体点值与设备的「逻辑分辨率」不吻合
 	FLOAT cyDpi = (FLOAT) (25.4 * GetDeviceCaps (hDC, VERTRES) / GetDeviceCaps (hDC, VERTSIZE)) ;

 	FLOAT y = 200 * cyDpi / 72;
 	m_lf.lfHeight = - (int)(fabs(y) / 10.0 + 0.5) ; // 需要include<math.h>
	*********************************************************************************/


	/*************************MSDN提供的算法*******************************************
 	m_lf.lfHeight = -MulDiv(20, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	*********************************************************************************/


//   	m_lf.lfHeight = -MulDiv(IA_BASE_FONT_SIZE * (int)ulTextScale, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	memset(&m_lf, 0, sizeof(LOGFONT));
	m_lf.lfHeight = - (LONG)m_ulCurrentFontSize;
// 	m_lf.lfWeight = FW_SEMIBOLD;
	if (12 > m_ulCurrentFontSize)
	{
		m_lf.lfWeight = FW_NORMAL;
	} else if (20 > m_ulCurrentFontSize)
	{
		m_lf.lfWeight = FW_MEDIUM;
	} else if (30 > m_ulCurrentFontSize){
		m_lf.lfWeight = FW_SEMIBOLD;
	} else {
		m_lf.lfWeight = FW_BOLD;
	}

	m_lf.lfCharSet = DEFAULT_CHARSET;
	memcpy_s(m_lf.lfFaceName, sizeof(m_lf.lfFaceName), "Arial\0", strlen("Arial\0"));

	m_hFont = CreateFontIndirect(&m_lf);  

	if (NULL == m_hFont)
	{
		IVS_LOG(IVS_LOG_ERR, "Creat Font", "CreatFont Error");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	return IVS_SUCCEED;
}

