// TimeStatic.cpp : implementation file
//

#include "stdafx.h"
//#include "IVS_OCXPlayer.h"
#include "TimeStatic.h"
#include "TranslucentUtility.h"
#include "PlayTimerBar.h"
// CTimeStatic
CCriticalSection g_cs;
IMPLEMENT_DYNAMIC(CTimeStatic, CTranslucentStatic)

CTimeStatic::CTimeStatic()
   : m_bTrack(false)
   , m_pHourLineX(nullptr)
   , m_pMinLineX(nullptr)
   , m_pScondLinX(nullptr)
   , m_iHourNum(0)
   , m_iMinNum(0)
   , m_iHourLength(0)
   , m_iMinLength(0)
   , m_iMidMinLength(0)
   , m_dHourSpace(0.0)
   , m_dMinSpace(0.0)
   , m_dSconSpace(0.0)
   , m_bIsMouseFlag(false)
   , m_bIsInit(false)
   , m_iCurrentPointPos(10)
   , m_iLstPointPos(10)
   , m_iLastXPos(0)
   , m_pPlayTimerBar(nullptr)
   , m_iSizeType(TYPE_SCALE_SMALL)
   , m_bIsTimer(false)
   , m_hThread(NULL)
   , m_bCurStreamStop(false)
{
	m_pVideoPane = NULL;
	memset(&m_stCurrentTime,0x0,sizeof(ST_TIME));
	memset(&m_stStartTime,0x0,sizeof(ST_TIME));
	memset(&m_stEndTime,0x0,sizeof(ST_TIME));
	memset(&m_stSaveCurrentTime,0x0,sizeof(ST_TIME));
    memset(&m_stStartPlayTime, 0x0, sizeof(ST_TIME));
	m_cRect = CRect(0,0,0,0);

	//设置时间线参数
	m_stStartTime.iHour = START_HOUR;
	m_stStartTime.iMinute = START_MIN;
	m_stEndTime.iHour = END_HOUR;
	m_stEndTime.iMinute = END_MIN;
	m_stCurrentTime.iHour = START_HOUR+1;
	m_stCurrentTime.iMinute = START_MIN;
	m_dScondGap = 0.0;
    memset(&m_Layer, 0x0, sizeof(ST_LAYERTIME));
}

void CTimeStatic::SetPlayTimerBar(CPlayTimerBar* pPlayTimerBar)
{
	m_pPlayTimerBar = pPlayTimerBar;
}

CTimeStatic::~CTimeStatic()
{
	try
	{
		Clear();
		ClearAreaList();
		ClearYelloList();
		ClearGreenList();
	}
	catch (...)
	{
	}
}//lint !e1740

void CTimeStatic::Clear()
{
	g_cs.Lock();
	//lint !e831
	if (NULL != m_pHourLineX)
	{
		delete []m_pHourLineX;//lint !e831
		m_pHourLineX = NULL;
	}
	//lint !e831
	if (NULL != m_pMinLineX)
	{
		delete []m_pMinLineX;//lint !e831
		m_pMinLineX = NULL;
	}
	if (NULL != m_pScondLinX)
	{
		delete []m_pScondLinX;
		m_pScondLinX = NULL;
	}
	m_bIsInit = false;
	g_cs.Unlock();
}

void CTimeStatic::ClearYelloList()
{
	g_cs.Lock();

	POSITION pos = m_ListYello.GetHeadPosition();
	POSITION pos1;
	ST_LAYERTIME* pYelloLayer = NULL;
	while(pos!=NULL)
	{
		pos1 = pos;
		pYelloLayer = m_ListYello.GetNext(pos);
		if (NULL != pYelloLayer)
		{
			m_ListYello.RemoveAt(pos1);
			delete (ST_LAYERTIME*)pYelloLayer;
			pYelloLayer = NULL;
		}
	}
    g_cs.Unlock();
}

void CTimeStatic::ClearGreenList()
{
	g_cs.Lock();

	POSITION pos = m_ListGreen.GetHeadPosition();
	POSITION pos1;
	ST_LAYERTIME* pGreenLayer = NULL;
	while(pos!=NULL)
	{
		pos1 = pos;
		pGreenLayer = m_ListGreen.GetNext(pos);
		if (NULL != pGreenLayer)
		{
			m_ListGreen.RemoveAt(pos1);
			delete (ST_LAYERTIME*)pGreenLayer;
			pGreenLayer = NULL;
		}
	}
	g_cs.Unlock();
}

void CTimeStatic::ClearAreaList()
{
	g_cs.Lock();

	POSITION pos = m_ListArea.GetHeadPosition();
	POSITION pos1;
	ST_AREAINFO* pArea = NULL;
	while(pos!=NULL)
	{
		pos1 = pos;
		pArea = m_ListArea.GetNext(pos);
		if (NULL != pArea)
		{
			m_ListArea.RemoveAt(pos1);
			delete (ST_AREAINFO*)pArea;
			pArea = NULL;
		}
	}
	g_cs.Unlock();
}

BEGIN_MESSAGE_MAP(CTimeStatic, CTranslucentStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTimeStatic message handlers

void CTimeStatic::Initialize()
{//lint !e830
	//int nCount = 0;

	//获取控件区
	GetClientRect(&m_cRect);
	//计算每秒的长度,只展示一小时的量
	m_dScondGap = (double)m_cRect.Width()/(1*SCOND_COUNT);
	//m_iHourLength = static_cast<int>(floor((double)(m_cRect.Height())/(double)LENGTH_HOUR));
	//m_iMinLength = static_cast<int>(floor((double)(m_cRect.Height())/(double)LENGTH_MIN));
	//m_iMidMinLength = static_cast<int>(floor((double)(m_cRect.Height())/(double)LENGTH_MIDMIN));

	//m_iHourNum = (m_stEndTime.iHour - m_stStartTime.iHour) + 1;
	//m_dHourSpace = (double)(m_cRect.right - m_cRect.left)/(double)(m_iHourNum-1);

	//m_pHourLineX = new int[m_iHourNum];//lint !e737 !e831
	//for (int nCount=0;nCount<m_iHourNum;nCount++)
	//{
	//	m_pHourLineX[nCount] = static_cast<int>(floor((double)m_cRect.TopLeft().x+m_dHourSpace*(double)nCount));
	//}
	////lint !e831
	//m_iMinNum = (m_iHourNum -1)*(MINUTE_COUNT/TEXT_MIN)*2+1;
	//m_pMinLineX = new int[m_iMinNum];//lint !e737 !e831
	//if ((double)(m_iMinNum - 1) == 0)
	//{
	//	return;
	//}
	//m_dMinSpace = (double)(m_cRect.right - m_cRect.left)/(double)(m_iMinNum - 1);
	//int index;
	//for (index=0;index<m_iMinNum;index++)
	//{
	//	m_pMinLineX[index] = static_cast<int>(floor((double)m_cRect.TopLeft().x+m_dMinSpace*index));
	//}

	m_bIsInit = true;
}

void CTimeStatic::Initialize2()
{
	//获取控件区
	GetClientRect(&m_cRect);
	m_iHourLength = static_cast<int>(floor((double)(m_cRect.Height())/(double)LENGTH_HOUR));
	m_iMinLength = static_cast<int>(floor((double)(m_cRect.Height())/(double)LENGTH_MIN));
	m_iMidMinLength = static_cast<int>(floor((double)(m_cRect.Height())/(double)LENGTH_MIDMIN));

	m_dSconSpace = (double)(m_cRect.right - m_cRect.left)/(double)(MAX_SCOND_NUM-1);
	m_pScondLinX = new int[MAX_SCOND_NUM];//lint !e737 !e831

	for (int nCount=0;nCount<MAX_SCOND_NUM;nCount++)
	{
		m_pScondLinX[nCount] = static_cast<int>(floor((double)m_cRect.TopLeft().x+m_dSconSpace*(double)nCount));
	}
	//lint !e831
	m_bIsInit = true;
}


void CTimeStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	UpdateState();
}

void CTimeStatic::Render(Gdiplus::Graphics& g)
{
	CRect rc;
	GetWindowRect(&rc);//lint !e64 !e118
	GetParent()->ScreenToClient(&rc);//lint !e64 !e118

	Gdiplus::RectF rect;

	rect.X = static_cast<REAL>(rc.left);
	rect.Y = static_cast<REAL>(rc.top);
	rect.Width = static_cast<REAL>(rc.Width());
	rect.Height = static_cast<REAL>(rc.Height());

	if (!m_bIsInit)
	{
		InitTimeRange();
	}
	
	//if (TYPE_SCALE_BIG == m_iSizeType)
	//{
	//	if (!m_bIsInit)
	//		Initialize();
	//	Pen myPen(Gdiplus::Color(0x6E,0x6E ,0x6E));
	//	//画小时刻度
	//	CString text;
	//	for (int i=1;i<m_iHourNum-1;i++)
	//	{
	//		CHECK_POINTER_VOID(m_pHourLineX);
	//		g.DrawLine(&myPen,m_pHourLineX[i],m_cRect.top,m_pHourLineX[i],m_cRect.top+m_iHourLength);
	//	}

	//	//画分钟刻度
	//	for (int i=1;i<m_iMinNum-1;i++)
	//	{
	//		CHECK_POINTER_VOID(m_pMinLineX);
	//		if (i%2==0)
	//			g.DrawLine(&myPen,m_pMinLineX[i],m_cRect.top,m_pMinLineX[i],m_cRect.top+m_iMidMinLength);
	//		else
	//			g.DrawLine(&myPen,m_pMinLineX[i],m_cRect.top,m_pMinLineX[i],m_cRect.top+m_iMinLength);
	//	}

	//	//画时间文字
	//	m_brush.SetColor(Gdiplus::Color(0x6E,0x6E ,0x6E));
	//	for (int i=2;i<m_iMinNum-2;i++)
	//	{
	//		CHECK_POINTER_VOID(m_pMinLineX);
	//		rect.X = static_cast<Gdiplus::REAL>(m_pMinLineX[i] - m_dMinSpace/2);
	//		rect.Y = static_cast<Gdiplus::REAL>(m_cRect.top+2*m_iMinLength);
	//		rect.Height = static_cast<Gdiplus::REAL>(2*m_iMinLength);//lint !e790

	//		ST_TIME stOldTime;
	//		ST_TIME stNewTime;
	//		memset(&stOldTime,0,sizeof(ST_TIME));
	//		memset(&stNewTime,0,sizeof(ST_TIME));
	//		stOldTime.iHour = m_stStartTime.iHour;
	//		stOldTime.iMinute = m_stStartTime.iMinute+i*MINUTE_GAP;
	//		TransTime(stNewTime,stOldTime);

	//		text.Format("%02d:%02d",stNewTime.iHour,stNewTime.iMinute);
	//		WCHAR* wstr = CTranslucentUtility::ttowc(text);
	//		(void)g.DrawString(wstr, static_cast<int >(wcslen(wstr)), m_pFont, rect, &m_format, &m_brush);
	//		if (NULL != wstr)
	//		{
	//			delete wstr;
	//			wstr = NULL;
	//		}
	//		
	//		i++;
	//	}
	//	//lint !e830
	//}
	//else if (TYPE_SCALE_SMALL == m_iSizeType)
	//{
	//	if (!m_bIsInit)
	//		Initialize2();

	//	////画15秒刻度
	//	Pen myPen(Gdiplus::Color(0x6E,0x6E ,0x6E));
	//	CString text;
	//	for (int i=0;i<MAX_SCOND_NUM;i++)
	//	{
	//		CHECK_POINTER_VOID(m_pScondLinX);
	//		g.DrawLine(&myPen,m_pScondLinX[i],m_cRect.top,m_pScondLinX[i],m_cRect.top+m_iHourLength);
	//	}
	//	
	//	//画时间文字
	//	m_brush.SetColor(Gdiplus::Color(0x6E,0x6E ,0x6E));
	//	for (int i=2;i<MAX_SCOND_NUM-2;i++)
	//	{
	//		CHECK_POINTER_VOID(m_pScondLinX);
	//		rect.X = static_cast<Gdiplus::REAL>(m_pScondLinX[i] - m_dSconSpace/2);
	//		rect.Y = static_cast<Gdiplus::REAL>(m_cRect.top+2*m_iMinLength);
	//		rect.Height = static_cast<Gdiplus::REAL>(2*m_iMinLength);//lint !e790

	//		ST_TIME stOldTime;
	//		ST_TIME stNewTime;
	//		memset(&stOldTime,0,sizeof(ST_TIME));
	//		memset(&stNewTime,0,sizeof(ST_TIME));

	//		stOldTime.iHour = m_stStartTime.iHour;
	//		stOldTime.iMinute = m_stStartTime.iMinute;
	//		stOldTime.iScond = m_stStartTime.iScond+i*SCOND_GAP;
	//		TransTime(stNewTime,stOldTime);

	//		text.Format("%02d:%02d:%02d",stNewTime.iHour,stNewTime.iMinute,stNewTime.iScond);
	//		WCHAR* wstr = CTranslucentUtility::ttowc(text);
	//		(void)g.DrawString(wstr, static_cast<int >(wcslen(wstr)), m_pFont, rect, &m_format, &m_brush);
	//		if (NULL != wstr)
	//		{
	//			delete wstr;
	//			wstr = NULL;
	//		}
	//		
	//		i++;
	//	}

	//}
    // 画开始和结束的时间

    CString text;
	rect.X = static_cast<Gdiplus::REAL>(rc.left);
	rect.Height = static_cast<Gdiplus::REAL>(2*LENGTH_MIN);//lint !e790
	text.Format("%02d:%02d:%02d",m_stStartTime.iHour,m_stStartTime.iMinute,m_stStartTime.iScond);
	WCHAR* wstrStart = CTranslucentUtility::ttowc(text);
	if (NULL != wstrStart)
	{
		(void)g.DrawString(wstrStart, static_cast<int >(wcslen(wstrStart)), m_pFont, rect, &m_format, &m_brush);
		delete []wstrStart;
		wstrStart = NULL;
	}

	int iEndTimeLeft = rc.right-60;
	if (iEndTimeLeft>0)
	{
		rect.X = static_cast<Gdiplus::REAL>(rc.right-60);
		rect.Height = static_cast<Gdiplus::REAL>(2*LENGTH_MIN);//lint !e790
		text.Format("%02d:%02d:%02d",m_stEndTime.iHour,m_stEndTime.iMinute,m_stEndTime.iScond);
		WCHAR* wstrEnd = CTranslucentUtility::ttowc(text);
		if (NULL != wstrEnd)
		{
			(void)g.DrawString(wstrEnd, static_cast<int >(wcslen(wstrEnd)), m_pFont, rect, &m_format, &m_brush);
			delete []wstrEnd;
			wstrEnd = NULL;
		}
	}

	//(void)TimeToPointX(m_stCurrentTime);
	//画黑黑的暗槽
	Gdiplus::RectF rectBk;
	rectBk.X = static_cast<Gdiplus::REAL>(rc.left);
	rectBk.Y = static_cast<Gdiplus::REAL>(rc.bottom - TEXT_GAP);
	rectBk.Width = static_cast<Gdiplus::REAL>(rc.Width());
	rectBk.Height = 3;
	Pen myPenBK((ULONG)Color::Black);
	g.DrawRectangle(&myPenBK,rectBk);
	m_brush.SetColor((ULONG)Color::Black);
	g.FillRectangle(&m_brush,rectBk);

	//画第一层底色黄色
	//ClearAreaList();

	//Gdiplus::RectF rect1;
	//Pen myPenLayer1(Gdiplus::Color(0xEF,0xB4 ,0x4D));
	//m_brush.SetColor(Gdiplus::Color(0xEA,0x9D ,0x17));
	//POSITION pos = m_ListYello.GetHeadPosition();
	//ST_LAYERTIME* pLayer = NULL;
	//g_cs.Lock();
	//while(pos!=NULL)
	//{
	//	pLayer = m_ListYello.GetNext(pos);
	//	if (NULL != pLayer)
	//	{
	//		double iYelloLeft = TimeToPointX(pLayer->stStartTime);
	//		double iYelloRight = TimeToPointX(pLayer->stEndTime);
	//		rect1.X = static_cast<Gdiplus::REAL>(iYelloLeft);
	//		rect1.Y = static_cast<Gdiplus::REAL>(rc.bottom - TEXT_GAP);
	//		rect1.Width = static_cast<Gdiplus::REAL>(iYelloRight - iYelloLeft);
	//		rect1.Height = 3;

	//		//插入黄色区域
	//		ST_AREAINFO* pArea = new ST_AREAINFO; //lint !e1713
	//		memset(pArea,0,sizeof(ST_AREAINFO));
	//		pArea->dLeft = iYelloLeft;
	//		pArea->dRight = iYelloRight;
	//		pArea->iType = 0;
	//		pArea->stTime.stStartTime.iHour = pLayer->stStartTime.iHour;
	//		pArea->stTime.stStartTime.iMinute = pLayer->stStartTime.iMinute;
	//		pArea->stTime.stStartTime.iScond = pLayer->stStartTime.iScond;
	//		pArea->stTime.stEndTime.iHour = pLayer->stEndTime.iHour;
	//		pArea->stTime.stEndTime.iMinute = pLayer->stEndTime.iMinute;
	//		pArea->stTime.stEndTime.iScond = pLayer->stEndTime.iScond;

	//		m_ListArea.AddTail(pArea);

	//		g.DrawRectangle(&myPenLayer1,rect1);
	//		g.FillRectangle(&m_brush,rect1);
	//	}
	//}
	//g_cs.Unlock();

	//画第二层绿色
	Gdiplus::RectF rect2;
	Pen myPenLayer2(Gdiplus::Color(0x84,0xA8 ,0x5A));
	m_brush.SetColor(Gdiplus::Color(0x5F,0x8D ,0x28));
	ClearAreaList();
	POSITION pos = m_ListGreen.GetHeadPosition();
	ST_LAYERTIME* pLayer = NULL;
	g_cs.Lock();
	while(pos!=NULL)
	{
		pLayer = m_ListGreen.GetNext(pos);
		if (NULL != pLayer)
		{
			double iGreenLeft = TimeToPointX(pLayer->stStartTime);
			double iGreenRight = TimeToPointX(pLayer->stEndTime);
//             TRACE("绿色区域起始时间[%d:%d:%d],绿色区域结束时间[%d:%d:%d]\n", 
//                 pLayer->stStartTime.iHour,pLayer->stStartTime.iMinute,pLayer->stStartTime.iScond,
//                 pLayer->stEndTime.iHour, pLayer->stEndTime.iMinute, pLayer->stEndTime.iScond);
            //TRACE("绿色区域起始位置:%f, 绿色区域结束位置:%f\n", iGreenLeft, iGreenRight);
            //TRACE("整个进度条的起始时间:%d:%d:%d\n", m_stStartTime.iHour, m_stStartTime.iMinute, m_stStartTime.iScond);
            //TRACE("每秒长度:%f\n", m_dScondGap);
			rect2.X = static_cast<Gdiplus::REAL>(iGreenLeft);
			rect2.Y = static_cast<Gdiplus::REAL>(rc.bottom - TEXT_GAP);
			rect2.Width = static_cast<Gdiplus::REAL>(iGreenRight - iGreenLeft);
			rect2.Height = 3;

			//插入绿色区域
			ST_AREAINFO* pArea = new ST_AREAINFO; //lint !e1712
			memset(pArea,0,sizeof(ST_AREAINFO));
			pArea->dLeft = iGreenLeft;
			pArea->dRight = iGreenRight;
			pArea->iType = 1;
			pArea->stTime.stStartTime.iHour = pLayer->stStartTime.iHour;
			pArea->stTime.stStartTime.iMinute = pLayer->stStartTime.iMinute;
			pArea->stTime.stStartTime.iScond = pLayer->stStartTime.iScond;
			pArea->stTime.stEndTime.iHour = pLayer->stEndTime.iHour;
			pArea->stTime.stEndTime.iMinute = pLayer->stEndTime.iMinute;
			pArea->stTime.stEndTime.iScond = pLayer->stEndTime.iScond;

			m_ListArea.AddTail(pArea);

			g.DrawRectangle(&myPenLayer2,rect2);
			g.FillRectangle(&m_brush,rect2);
		}
	}
	g_cs.Unlock();

	//画滑动游标线
	Pen myPenPointer(Gdiplus::Color(0xEF,0xB4 ,0x4D));
    m_iCurrentPointPos = TimeToPointX(m_stCurrentTime);
	g.DrawLine(&myPenPointer,static_cast<int>(m_iCurrentPointPos),
		rc.top,static_cast<int>(m_iCurrentPointPos),rc.bottom);
    //TRACE("当前游标位置:%f\n", m_iCurrentPointPos);

	//画游标线上的小圈圈
	Gdiplus::RectF Pointerrect;
	Pointerrect.X = static_cast<Gdiplus::REAL>(m_iCurrentPointPos-NONIUS_LEFT_OFFSET);
	Pointerrect.Y = static_cast<Gdiplus::REAL>(rc.bottom - NONIUS_BOTTOM_OFFSET);
	Pointerrect.Width = NONIUS_WIDTH;
	Pointerrect.Height = NONIUS_HRIGHT;

	Pen myPenEllipse((ULONG)Color::Black);
	m_brush.SetColor((ULONG)Color::Gray);
	g.DrawEllipse(&myPenEllipse,Pointerrect);
	g.FillEllipse(&m_brush,Pointerrect);
}

void CTimeStatic::TransTime(ST_TIME &newtime,const ST_TIME &oldtime)const
{
	TimeTansZero(newtime);

	if (TYPE_SCALE_BIG == m_iSizeType)
	{
		if (oldtime.iMinute >= MINUTE_COUNT)
		{
			int iMinite = oldtime.iHour*MINUTE_COUNT + oldtime.iMinute;
			newtime.iHour = iMinite/MINUTE_COUNT;
			newtime.iMinute = iMinite%MINUTE_COUNT;
		}
		else
		{
			newtime.iHour = oldtime.iHour;
			newtime.iMinute = oldtime.iMinute;
		}
	}
	else
	{
		if (oldtime.iScond >= MINUTE_COUNT)
		{
			int iScond = oldtime.iHour*SCOND_COUNT + oldtime.iMinute*MINUTE_COUNT+oldtime.iScond;
			newtime.iHour = iScond/SCOND_COUNT;
			newtime.iMinute = iScond%SCOND_COUNT/MINUTE_COUNT;
			newtime.iScond = (iScond%SCOND_COUNT)%MINUTE_COUNT;
		}
		else
		{
			newtime.iHour = oldtime.iHour;
			newtime.iMinute = oldtime.iMinute;
			newtime.iScond = oldtime.iScond;
		}
	}
}
//lint !e830
bool CTimeStatic::TimeToRight(ST_TIME &oldtime,const ST_TIME &righttime)const
{
	TimeTansZero(oldtime);
	if (TYPE_SCALE_BIG == m_iSizeType)
	{
		int iOldMinite = oldtime.iHour*MINUTE_COUNT + oldtime.iMinute;
		int iRightMinite = righttime.iHour*MINUTE_COUNT + righttime.iMinute;
		int iNewMinite = iRightMinite+iOldMinite;
		if (iNewMinite >= (HOUR_COUNT*MINUTE_COUNT))
		{
			return false;
		}

		oldtime.iHour = (iNewMinite/MINUTE_COUNT)%HOUR_COUNT;
		oldtime.iMinute = iNewMinite%MINUTE_COUNT;
	}
	else if (TYPE_SCALE_SMALL == m_iSizeType)
	{
		int iOldScond = oldtime.iHour*SCOND_COUNT+oldtime.iMinute*MINUTE_COUNT+oldtime.iScond;
		int iRightScond = righttime.iHour*SCOND_COUNT+righttime.iMinute*MINUTE_COUNT+righttime.iScond;
		int iNewScond = iOldScond+iRightScond;
		if (iNewScond >= HOUR_COUNT*SCOND_COUNT)
		{
			return false;
		}

		oldtime.iHour = iNewScond/SCOND_COUNT;
		oldtime.iMinute = (iNewScond%SCOND_COUNT)/MINUTE_COUNT;
		oldtime.iScond = (iNewScond%SCOND_COUNT)%MINUTE_COUNT;
	}

	return true;
}//lint !e1762
//lint !e830
bool CTimeStatic::TimeToleft(ST_TIME &oldtime,const ST_TIME &lefttime)const
{
	if (TYPE_SCALE_BIG == m_iSizeType)
	{
		int iOldMinite = oldtime.iHour*MINUTE_COUNT + oldtime.iMinute;
		int iRightMinite = lefttime.iHour*MINUTE_COUNT + lefttime.iMinute;
		int iNewMinite = iOldMinite - iRightMinite;
		if (iNewMinite < 0)
		{
			return false;
		}

		oldtime.iHour = (iNewMinite/MINUTE_COUNT)%HOUR_COUNT;
		oldtime.iMinute = iNewMinite%MINUTE_COUNT;
	}
	else if (TYPE_SCALE_SMALL == m_iSizeType)
	{
		int iOldScond = oldtime.iHour*SCOND_COUNT+oldtime.iMinute*MINUTE_COUNT+oldtime.iScond;
		int iLeftScond = lefttime.iHour*SCOND_COUNT+lefttime.iMinute*MINUTE_COUNT+lefttime.iScond;
		int iNewScond = iOldScond-iLeftScond;
		if (iNewScond < 0)
		{
			return false;
		}

		oldtime.iHour = iNewScond/SCOND_COUNT;
		oldtime.iMinute = (iNewScond%SCOND_COUNT)/MINUTE_COUNT;
		oldtime.iScond = (iNewScond%SCOND_COUNT)%MINUTE_COUNT;
	}

	return true;
}

void CTimeStatic::TimeTansZero(ST_TIME &sttime)const
{
	if (sttime.iHour == 0 && sttime.iMinute == 0 && sttime.iScond == 0)
	{
		sttime.iHour = 0;
		sttime.iMinute = 0;
		sttime.iScond = 0;
	}
}

void CTimeStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);

	if (!m_bTrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.dwHoverTime = HOVER_DEFAULT;
		tme.hwndTrack = m_hWnd;
		m_bTrack = /*static_cast<bool>(_TrackMouseEvent(&tme))*/_TrackMouseEvent(&tme) == FALSE ? false : true;
	}


	if (m_bIsMouseFlag)
	{
		CRect rc;
		GetWindowRect(&rc);//lint !e64 !e118
		GetParent()->ScreenToClient(&rc);//lint !e64 !e118

		if (point.x > rc.right || point.x < rc.left)
		{
			return;
		}
		
		m_stSaveCurrentTime = m_stCurrentTime;
		m_stCurrentTime = PointToTime(point);
		UpdateState();
	}


	CTranslucentStatic::OnMouseMove(nFlags, point);
}

void CTimeStatic::MovePaneBig(const CPoint& point)
{
	//SetCursor(AfxGetApp()->LoadCursor(IDC_IBEAM));
	//更新刻度
	ST_TIME stAdd;
	memset(&stAdd,0,sizeof(ST_TIME));
	int iMoveStep = 0;
	int iRate = 0;
	int iMinite = 0;//lint !e830
	//int iRangeMinute =  (m_stEndTime.iHour*MINUTE_COUNT+m_stEndTime.iMinute) - (m_stStartTime.iHour*MINUTE_COUNT+m_stStartTime.iMinute);
	// 向右移
	if (point.x == m_iLastXPos)
	{
		return;
	}

	if (point.x > m_iLastXPos)
	{
		iMoveStep = point.x - m_iLastXPos;
		iRate = static_cast<int>(iMoveStep/(m_dMinSpace*2));
		iMinite = iRate*TEXT_GAP;
		MiniteToTime(stAdd,iMinite);

		if (!TimeToRight(m_stEndTime,stAdd))
		{
			//最后一个因为切换的原因出不来需要特殊处理下
			if(24*MINUTE_COUNT == m_stEndTime.iHour*MINUTE_COUNT+m_stEndTime.iMinute+stAdd.iHour*MINUTE_COUNT+stAdd.iMinute)
			{
				(void)TimeToRight(m_stStartTime,stAdd);
				m_stEndTime.iHour = HOUR_COUNT;
				m_stEndTime.iMinute = 0;
				(void)SetBarTime(m_stStartTime,m_stEndTime);
			}
			return;
		}
		if (!TimeToRight(m_stStartTime,stAdd))
		{
			return;
		}
	} 
	//否则向左移
	else
	{
		iMoveStep = m_iLastXPos - point.x;
		iRate = static_cast<int>(iMoveStep/(m_dMinSpace*2));
		iMinite = iRate*TEXT_GAP;
		MiniteToTime(stAdd,iMinite);

		if (!TimeToleft(m_stStartTime,stAdd))
		{
			return;
		}
		if (!TimeToleft(m_stEndTime,stAdd))
		{
			if (0 == m_stEndTime.iHour*MINUTE_COUNT+m_stEndTime.iMinute)
			{
				//最后一个因为切换的原因出不来需要特殊处理下
				m_stEndTime.iHour = HOUR_COUNT;
				m_stEndTime.iMinute = 0;
				(void)TimeToleft(m_stEndTime,stAdd);
				(void)SetBarTime(m_stStartTime,m_stEndTime);
			}
			return;
		}
	}

	(void)SetBarTime(m_stStartTime,m_stEndTime);
}

void CTimeStatic::MovePaneSmall(const CPoint& point)
{
	//SetCursor(AfxGetApp()->LoadCursor(IDC_IBEAM));
	//更新刻度
	ST_TIME stAdd;
	memset(&stAdd,0,sizeof(ST_TIME));
	int iMoveStep = 0;
	int iRate = 0;
	int iScond = 0;//lint !e830
	//int iRangeMinute =  (m_stEndTime.iHour*MINUTE_COUNT+m_stEndTime.iMinute) - (m_stStartTime.iHour*MINUTE_COUNT+m_stStartTime.iMinute);
	// 向右移
	if (point.x == m_iLastXPos)
	{
		return;
	}

	if (point.x > m_iLastXPos)
	{
		iMoveStep = point.x - m_iLastXPos;
		iRate = static_cast<int>(iMoveStep/(m_dSconSpace*2));
		iScond = iRate*SCOND_GAP;
		ScondToTime(stAdd,iScond);

		if (!TimeToRight(m_stEndTime,stAdd))
		{
			//最后一个因为切换的原因出不来需要特殊处理下
			if(24*SCOND_COUNT == m_stEndTime.iHour*SCOND_COUNT+m_stEndTime.iMinute*MINUTE_COUNT+m_stEndTime.iScond+
				stAdd.iHour*SCOND_COUNT+stAdd.iMinute*MINUTE_COUNT+stAdd.iScond)
			{
				(void)TimeToRight(m_stStartTime,stAdd);
				m_stEndTime.iHour = HOUR_COUNT;
				m_stEndTime.iMinute = 0;
				m_stEndTime.iScond = 0;
				(void)SetBarTime(m_stStartTime,m_stEndTime);
			}
			return;
		}
		if (!TimeToRight(m_stStartTime,stAdd))
		{
			return;
		}
	} 
	//否则向左移
	else
	{
		iMoveStep = m_iLastXPos - point.x;
		iRate = static_cast<int>(iMoveStep/(m_dSconSpace*2));
		iScond = iRate*TEXT_GAP;
		ScondToTime(stAdd,iScond);

		if (!TimeToleft(m_stStartTime,stAdd))
		{
			return;
		}
		if (!TimeToleft(m_stEndTime,stAdd))
		{
			if (0 == m_stEndTime.iHour*MINUTE_COUNT+m_stEndTime.iMinute)
			{
				//最后一个因为切换的原因出不来需要特殊处理下
				m_stEndTime.iHour = HOUR_COUNT;
				m_stEndTime.iMinute = 0;
				(void)TimeToleft(m_stEndTime,stAdd);
				(void)SetBarTime(m_stStartTime,m_stEndTime);
			}
			return;
		}
	}

	(void)SetBarTime(m_stStartTime,m_stEndTime);
}


BOOL CTimeStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	return CTranslucentStatic::OnSetCursor(pWnd, nHitTest, message);
}


void CTimeStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bIsMouseFlag = true;
	SetCapture();
	CTranslucentStatic::OnLButtonDown(nFlags, point);
}

void CTimeStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (!m_pPlayTimerBar)
	{
		return;
	}

	if (m_bIsMouseFlag)
	{
		ReleaseCapture();
		
		CRect rc;
		GetWindowRect(&rc);//lint !e64 !e118
		GetParent()->ScreenToClient(&rc);//lint !e64 !e118
    	
		if (point.x > rc.right || point.x < rc.left)
		{
		    m_bIsMouseFlag = false;
			m_pPlayTimerBar->GetPlayTime(m_stCurrentTime);
			UpdateState();
			return;
		}

		m_stCurrentTime = PointToTime(point);
		m_iCurrentPointPos = TimeToPointX(m_stCurrentTime);

		ST_TIMER_MSG stMsg;
		memset(&stMsg,0,sizeof(ST_TIMER_MSG));
		if (!IsCanPlayBack(stMsg))
		{
			m_bIsMouseFlag = false;
			m_pPlayTimerBar->GetPlayTime(m_stCurrentTime);
			UpdateState();
			return;
		}
		(void)SetCurrentTime(PointToTime(point));
		m_bIsMouseFlag = false;
	}

	CTranslucentStatic::OnLButtonUp(nFlags, point);
}


void CTimeStatic::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	m_bIsMouseFlag = false;
	CTranslucentStatic::OnMouseLeave();
}

LRESULT CTimeStatic::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_MOUSELEAVE)
	{
		m_bTrack = false;
	}
	return CTranslucentStatic::WindowProc(message, wParam, lParam);
}


ST_TIME CTimeStatic::PointToTime(const CPoint &point) const
{
	CRect rc;
	GetWindowRect(&rc);//lint !e64 !e118
	GetParent()->ScreenToClient(&rc);//lint !e64 !e118

	ST_TIME time;
	memset(&time,0,sizeof(ST_TIME));

	int iEndScond = m_stEndTime.iHour*SCOND_COUNT+m_stEndTime.iMinute*MINUTE_COUNT+m_stEndTime.iScond;
	int iStartScond = m_stStartTime.iHour*SCOND_COUNT+m_stStartTime.iMinute*MINUTE_COUNT+m_stStartTime.iScond;
	int iScondRange = iEndScond-iStartScond;
	double dRate = (double)point.x/rc.Width();
	//这里有问题进行四舍五入
	int iAddTime = (int)(iScondRange*dRate+0.5);
	int iCurrentScond = iStartScond+iAddTime;
	time.iHour = iCurrentScond/SCOND_COUNT;
	time.iMinute = (iCurrentScond%SCOND_COUNT)/MINUTE_COUNT;
	time.iScond = (iCurrentScond%SCOND_COUNT)%MINUTE_COUNT;

	//if (TYPE_SCALE_BIG == m_iSizeType)
	//{
	//	int iMiniteRange  = (m_stEndTime.iHour*MINUTE_COUNT + m_stEndTime.iMinute) - (m_stStartTime.iHour*MINUTE_COUNT+m_stStartTime.iMinute);
	//	double dRate = (double)point.x/rc.Width();
	//	//这里有问题进行四舍五入
	//	int iAddTime = (int)(iMiniteRange*dRate+0.5);

	//	int dCurrentMinite = m_stStartTime.iHour*MINUTE_COUNT+m_stStartTime.iMinute+iAddTime;
	//	time.iHour = dCurrentMinite/MINUTE_COUNT;
	//	time.iMinute = dCurrentMinite%MINUTE_COUNT;
	//}
	//else if (TYPE_SCALE_SMALL == m_iSizeType)
	//{
	//	int iEndScond = m_stEndTime.iHour*SCOND_COUNT+m_stEndTime.iMinute*MINUTE_COUNT+m_stEndTime.iScond;
	//	int iStartScond = m_stStartTime.iHour*SCOND_COUNT+m_stStartTime.iMinute*MINUTE_COUNT+m_stStartTime.iScond;
	//	int iScondRange = iEndScond-iStartScond;
	//	double dRate = (double)point.x/rc.Width();
	//	//这里有问题进行四舍五入
	//	int iAddTime = (int)(iScondRange*dRate+0.5);

	//	int iCurrentScond = iStartScond+iAddTime;
	//	time.iHour = iCurrentScond/SCOND_COUNT;
	//	time.iMinute = (iCurrentScond%SCOND_COUNT)/MINUTE_COUNT;
	//	time.iScond = (iCurrentScond%SCOND_COUNT)%MINUTE_COUNT;
	//}

	return time;
}

double CTimeStatic::TimeToPointX(const ST_TIME &stStarttime)
{
	CRect rc;
	GetWindowRect(&rc);//lint !e64 !e118
	GetParent()->ScreenToClient(&rc);//lint !e64 !e118

	//每秒的长度
	m_dScondGap = (double)rc.Width()/(1*SCOND_COUNT);
	//计算当前时间的秒数
	int iCurrentScond = stStarttime.iHour*SCOND_COUNT+stStarttime.iMinute*MINUTE_COUNT+stStarttime.iScond;
	int iEndScond = m_stEndTime.iHour*SCOND_COUNT+m_stEndTime.iMinute*MINUTE_COUNT+m_stEndTime.iScond;
	int iStartScond = m_stStartTime.iHour*SCOND_COUNT+m_stStartTime.iMinute*MINUTE_COUNT+m_stStartTime.iScond;
	if (iCurrentScond < iStartScond)
	{
		return 0;
	}

	if (iCurrentScond > iEndScond)
	{
		return rc.Width();
	}

	//计算当前时间的坐标位置
	double dCurrentPos = (iCurrentScond - iStartScond)*m_dScondGap;

	/*double x = 0.0;
	if (TYPE_SCALE_BIG == m_iSizeType)
	{
	int iCurrentMinute = stStarttime.iHour*MINUTE_COUNT+stStarttime.iMinute;
	int iStartMinute = (m_stStartTime.iHour*MINUTE_COUNT+m_stStartTime.iMinute);
	int iEndMinite = (m_stEndTime.iHour*MINUTE_COUNT+m_stEndTime.iMinute);
	if (iCurrentMinute < iStartMinute)
	{
	return 0;
	}

	if (iCurrentMinute > iEndMinite)
	{
	return rc.Width();
	}

	int iRange = iEndMinite - iStartMinute;
	double dRate = ((double)iCurrentMinute - iStartMinute)/iRange;
	x = dRate*(rc.Width());
	}
	else if (TYPE_SCALE_SMALL == m_iSizeType)
	{
	int iCurrentScond = stStarttime.iHour*SCOND_COUNT+stStarttime.iMinute*MINUTE_COUNT+stStarttime.iScond;
	int iEndScond = m_stEndTime.iHour*SCOND_COUNT+m_stEndTime.iMinute*MINUTE_COUNT+m_stEndTime.iScond;
	int iStartScond = m_stStartTime.iHour*SCOND_COUNT+m_stStartTime.iMinute*MINUTE_COUNT+m_stStartTime.iScond;
	if (iCurrentScond < iStartScond)
	{
	return 0;
	}

	if (iCurrentScond > iEndScond)
	{
	return rc.Width();
	}

	int iRange = iEndScond - iStartScond;
	double dRate = ((double)iCurrentScond - iStartScond)/iRange;
	x = dRate*(rc.Width());
	}*/
	
	return dCurrentPos;
}

ST_TIME CTimeStatic::GetCurrentTime()const
{
	return m_stCurrentTime;
};

bool CTimeStatic::SetBarTime(const ST_TIME &stStarttime,const ST_TIME &stEndtime)
{
	int iStartMinute = stStarttime.iHour*MINUTE_COUNT+stStarttime.iMinute;
	int iEndMinute = stEndtime.iHour*MINUTE_COUNT+stEndtime.iMinute;

	if (iEndMinute < iStartMinute || ((iEndMinute - iStartMinute) > MINUTE_COUNT*2))
	{
		return false;
	}

	m_stStartTime.iHour = stStarttime.iHour;
	m_stStartTime.iMinute = stStarttime.iMinute;
	m_stEndTime.iHour = stEndtime.iHour;
	m_stEndTime.iMinute = stEndtime.iMinute;
	//m_iCurrentPointPos = TimeToPointX(m_stCurrentTime);
	Clear();
	Invalidate();
	//lint !e831
	if (m_pPlayTimerBar!=NULL)
	{
		m_pPlayTimerBar->SetBarTime(m_stStartTime,m_stEndTime);
	}
	return true;
}

bool CTimeStatic::SetCurrentTime(const ST_TIME &time)
{
	//g_cs.Lock();
	m_stCurrentTime.iHour = time.iHour;
	m_stCurrentTime.iMinute = time.iMinute;
	m_stCurrentTime.iScond = time.iScond;
	m_iCurrentPointPos = TimeToPointX(m_stCurrentTime);
    TRACE("SetCurrentTime:%d:%d:%d\n",time.iHour, time.iMinute, time.iScond);
	UpdateState();
	m_stSaveCurrentTime.iHour = m_stCurrentTime.iHour;
	m_stSaveCurrentTime.iMinute = m_stCurrentTime.iMinute;
	m_stSaveCurrentTime.iScond = m_stCurrentTime.iScond;
	//g_cs.Unlock();

	if (m_bIsMouseFlag)
	{
		if(!m_bIsTimer)
		{
			return false;
		}

		//FindRecord();
		ST_TIMER_MSG stMsg;
		memset(&stMsg,0,sizeof(ST_TIMER_MSG));
		if (IsCanPlayBack(stMsg))
		{
			KillTimer(RECORD_PLAY_TIME_ID);
            TRACE("kill timer:%d\n", __LINE__);
			int iCurrentScond = stMsg.stCurrentTime.iHour*SCOND_COUNT+stMsg.stCurrentTime.iMinute*MINUTE_COUNT+stMsg.stCurrentTime.iScond;
			//int iStartScond = stMsg.stStartTime.iHour*SCOND_COUNT+stMsg.stStartTime.iMinute*MINUTE_COUNT+stMsg.stStartTime.iScond;
            int iStartScond = m_stStartPlayTime.iHour*SCOND_COUNT + m_stStartPlayTime.iMinute*MINUTE_COUNT + m_stStartPlayTime.iScond;
			int iPos = iCurrentScond - iStartScond; //该段录像的总时间
            TRACE("stStartTime:%d:%d:%d\n",stMsg.stStartTime.iHour, stMsg.stStartTime.iMinute, stMsg.stStartTime.iScond);
            TRACE("stStartPlayTime:%d:%d:%d\n",m_stStartPlayTime.iHour, m_stStartPlayTime.iMinute, m_stStartPlayTime.iScond);
            TRACE("stCurrentTime:%d:%d:%d\n",stMsg.stCurrentTime.iHour, stMsg.stCurrentTime.iMinute, stMsg.stCurrentTime.iScond);
            CHECK_POINTER(m_pPlayTimerBar, false);
            if ( IsInCurrentArea(m_stCurrentTime) )
            {
                if (iPos>=0)
                {
                    m_pPlayTimerBar->PostMessage(WM_TIMERBAR_TIME_MOVE,0,iPos);
                }
                else if( iPos < 0 )//如果小于0需要重新播放录像
                {
                    m_pPlayTimerBar->PostMessage(WM_TIMERBAR_START_PLAY, 0, 0);
                }
            }
            else
            {
                m_pPlayTimerBar->PostMessage(WM_TIMERBAR_START_PLAY, 0, 0);
            }
			
		}
		else
		{
			KillTimer(RECORD_PLAY_TIME_ID);
            TRACE("kill timer:%d\n", __LINE__);
			m_pPlayTimerBar->PostMessage(WM_TIMERBAR_STOP_PLAY,0,0);
		}
	}
	//if (TYPE_SCALE_BIG == m_iSizeType)
	//{
	//	int iStartMinute = m_stStartTime.iHour*MINUTE_COUNT+m_stStartTime.iMinute;
	//	int iEndMinute = m_stEndTime.iHour*MINUTE_COUNT+m_stEndTime.iMinute;
	//	int iCurrentMinute = time.iHour*MINUTE_COUNT+time.iMinute;

	//	if (iCurrentMinute<iStartMinute||iCurrentMinute>iEndMinute)
	//	{
	//		return false;
	//	}

	//	m_stCurrentTime.iHour = time.iHour;
	//	m_stCurrentTime.iMinute = time.iMinute;
	//	m_stCurrentTime.iScond = time.iScond;
	//	m_iCurrentPointPos = TimeToPointX(m_stCurrentTime);

	//	/*if (abs(m_iLstPointPos-m_iCurrentPointPos)>0.01)
	//	{
	//	FindRecord();
	//	if (m_pPlayTimerBar!=NULL)
	//	{
	//	m_pPlayTimerBar->SetCurrentTime(m_stCurrentTime);
	//	Invalidate();
	//	}
	//	m_iLstPointPos = m_iCurrentPointPos;
	//	if (m_bIsTimer)
	//	{
	//	KillTimer(RECORD_PLAY_TIME_ID);
	//	m_bIsTimer = false;
	//	}
	//	return true;
	//	}*/
	//}
	//else if (TYPE_SCALE_SMALL == m_iSizeType)
	//{
	//	int iStartScond = m_stStartTime.iHour*SCOND_COUNT+m_stStartTime.iMinute*MINUTE_COUNT+m_stStartTime.iScond;
	//	int iEndScond = m_stEndTime.iHour*SCOND_COUNT+m_stEndTime.iMinute*MINUTE_COUNT+m_stEndTime.iScond;
	//	int iCurrentScond = time.iHour*SCOND_COUNT+time.iMinute*MINUTE_COUNT+time.iScond;

	//	if (iCurrentScond<iStartScond||iCurrentScond>iEndScond)
	//	{
	//		return false;
	//	}

	//	m_stCurrentTime.iHour = time.iHour;
	//	m_stCurrentTime.iMinute = time.iMinute;
	//	m_stCurrentTime.iScond = time.iScond;

	//	CString str;
	//	str.Format("JJJ:%02d:%02d:%02d",m_stCurrentTime.iHour,
	//		m_stCurrentTime.iMinute,m_stCurrentTime.iScond);
	//	OutputDebugString(str);
	//	m_iCurrentPointPos = TimeToPointX(m_stCurrentTime);
	//	m_pPlayTimerBar->SetCurrentTime(m_stCurrentTime);
	//	
	//	Invalidate();
	//	//播放状态
	//	/*if (abs(m_iLstPointPos-m_iCurrentPointPos)>0.01)
	//	{
	//	FindRecord();
	//	if (m_pPlayTimerBar!=NULL)
	//	{
	//	m_pPlayTimerBar->SetCurrentTime(m_stCurrentTime);
	//	Invalidate();
	//	}
	//	m_iLstPointPos = m_iCurrentPointPos;
	//	if (m_bIsTimer)
	//	{
	//	KillTimer(RECORD_PLAY_TIME_ID);
	//	m_bIsTimer = false;
	//	}
	//	return true;
	//	}*/
	//}
	
	return true;
}

void CTimeStatic::FindRecord()
{
	//g_cs.Lock();

	POSITION pos = m_ListArea.GetHeadPosition();//lint !e830
	//POSITION pos1;
	ST_AREAINFO* pAreaInfo = NULL;
	bool bIsSendMsg = false;
	while(pos!=NULL)
	{
		//pos1 = pos;
		pAreaInfo = m_ListArea.GetNext(pos);
		if (NULL != pAreaInfo)
		{
			if ((m_iCurrentPointPos>=pAreaInfo->dLeft)&&(m_iCurrentPointPos<=pAreaInfo->dRight))
			{
				//表示进入了区域，需要抛送消息上去
				ST_LAYERTIME stLayerTime;
				memset(&stLayerTime,0,sizeof(ST_LAYERTIME));

				stLayerTime.stStartTime.iHour = pAreaInfo->stTime.stStartTime.iHour;
				stLayerTime.stStartTime.iMinute = pAreaInfo->stTime.stStartTime.iMinute;
				stLayerTime.stStartTime.iScond = pAreaInfo->stTime.stStartTime.iScond;
				stLayerTime.stEndTime.iHour = pAreaInfo->stTime.stEndTime.iHour;
				stLayerTime.stEndTime.iMinute = pAreaInfo->stTime.stEndTime.iMinute;
				stLayerTime.stEndTime.iScond = pAreaInfo->stTime.stEndTime.iScond;
		
				if (m_pPlayTimerBar!=NULL)
				{
					m_pPlayTimerBar->SetCurrentTime(m_stCurrentTime);
					m_pPlayTimerBar->SetCurrentRangeAndType(stLayerTime,pAreaInfo->iType);
					bIsSendMsg = true;
				}
			}
			//else if (abs(m_iCurrentPointPos - pAreaInfo->dRight)<0.01)
			//{
			//	if (m_bIsTimer)
			//	{
			//		KillTimer(RECORD_PLAY_TIME_ID);
			//		m_bIsTimer = false;
			//	}
			//}
		}
	}
	//g_cs.Unlock();
	if(bIsSendMsg)
	{
		//WaitForSingleObject(m_hThread,1000);
		//::TerminateThread(m_hThread,0);
		////CloseHandle(m_hThread);
		//m_hThread = NULL;
		CHECK_POINTER_VOID(m_pPlayTimerBar);
		m_pPlayTimerBar->PostMessage(WM_TIMERBAR_TIME_MOVE,0,0);
		//SetTimer(RECORD_PLAY_TIME_ID,1000,NULL);
	}
}//lint !e550

void CTimeStatic::AddLayeTimeRange(int iType,const ST_LAYERTIME &stLayerTime)
{
	switch(iType)
	{
	case TYPE_YELLO:
		{//lint !e831
			ST_LAYERTIME* pYello = new ST_LAYERTIME;//lint !e831
			if (NULL == pYello)//lint !e774
			{
				return;
			}
			memset(pYello,0,sizeof(ST_LAYERTIME));

			pYello->stStartTime.iHour = stLayerTime.stStartTime.iHour;
			pYello->stStartTime.iMinute = stLayerTime.stStartTime.iMinute;
			pYello->stStartTime.iScond = stLayerTime.stStartTime.iScond;
			pYello->stEndTime.iHour = stLayerTime.stEndTime.iHour;
			pYello->stEndTime.iMinute = stLayerTime.stEndTime.iMinute;
			pYello->stEndTime.iScond = stLayerTime.stEndTime.iScond;

			g_cs.Lock();
			m_ListYello.AddTail(pYello);
			g_cs.Unlock();
		}
		break;
	case TYPE_GREEN:
		{//lint !e831
			ST_LAYERTIME* pGreen = new ST_LAYERTIME;//lint !e831
			if (NULL == pGreen)//lint !e774
			{
				return;
			}
			memset(pGreen,0,sizeof(ST_LAYERTIME));

			pGreen->stStartTime.iHour = stLayerTime.stStartTime.iHour;
			pGreen->stStartTime.iMinute = stLayerTime.stStartTime.iMinute;
			pGreen->stStartTime.iScond = stLayerTime.stStartTime.iScond;
			pGreen->stEndTime.iHour = stLayerTime.stEndTime.iHour;
			pGreen->stEndTime.iMinute = stLayerTime.stEndTime.iMinute;
			pGreen->stEndTime.iScond = stLayerTime.stEndTime.iScond;
			g_cs.Lock();
			m_ListGreen.AddTail(pGreen);
			g_cs.Unlock();
		}
		break;
	default:
		break;
	}

	//UpdateState();
	//Invalidate();
}

void CTimeStatic::MiniteToTime(ST_TIME &oldtime,const int iMinute)const
{
	oldtime.iHour = iMinute/MINUTE_COUNT;
	oldtime.iMinute = iMinute%MINUTE_COUNT;
}

void CTimeStatic::ScondToTime(ST_TIME &oldtime,const int iScond) const
{
	oldtime.iHour = iScond/SCOND_COUNT;
	oldtime.iMinute = (iScond%SCOND_COUNT)/MINUTE_COUNT;
	oldtime.iScond = (iScond%SCOND_COUNT)%MINUTE_COUNT;
}

BOOL CTimeStatic::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	//bool bIsNeedChange = false;
	//if (zDelta == WHEEL_SCAL)
	//{
	//	//不是大刻度需要切换大刻度
	//	if (TYPE_SCALE_BIG != m_iSizeType)
	//	{
	//		bIsNeedChange = true;
	//		m_iSizeType = TYPE_SCALE_BIG;
	//	}
	//}
	//else if(zDelta == -WHEEL_SCAL)
	//{
	//	//不是小刻度需要切换小刻度
	//	if (TYPE_SCALE_SMALL != m_iSizeType)
	//	{
	//		bIsNeedChange = true;
	//		m_iSizeType = TYPE_SCALE_SMALL;
	//	}	
	//}

	////只有需要切换的时候才切换
	//if (bIsNeedChange)
	//{
	//	InitTimeRange();
	//}
	
	return CTranslucentStatic::OnMouseWheel(nFlags, zDelta, pt);
}//lint !e1746

void CTimeStatic::InitTimeRange()
{
	// 初始化代码 //
	Clear();

	struct tm local;
	memset(&local, 0, sizeof(tm));
	time_t currenttime;
	//转换UTC时间
	SYSTEMTIME locTm;
	GetLocalTime(&locTm);
	CTime timeCurrent;
	timeCurrent = CTime((int)locTm.wYear,(int)locTm.wMonth,(int)locTm.wDay,
		(int)locTm.wHour,(int)locTm.wMinute,(int)locTm.wSecond,-1);

	currenttime = timeCurrent.GetTime();
	localtime_s(&local, &currenttime);
	currenttime = mktime(&local);

	time_t starttime;
	local.tm_hour -= 1;
	starttime = mktime(&local);

	CTime newCurrentTime(currenttime);
	CTime newStartTime(starttime);

	memset(&m_stStartTime,0,sizeof(ST_TIME));
	m_stStartTime.iHour = newStartTime.GetHour();
	m_stStartTime.iMinute = newStartTime.GetMinute();
	m_stStartTime.iScond = newStartTime.GetSecond();

	//结束时间为当前时间
	memset(&m_stEndTime,0,sizeof(ST_TIME));
	m_stEndTime.iHour = newCurrentTime.GetHour();
	m_stEndTime.iMinute = newCurrentTime.GetMinute();
	m_stEndTime.iScond = newCurrentTime.GetSecond();
	m_bIsInit = true;

	//如果是大刻度
	//if (TYPE_SCALE_BIG == m_iSizeType)
	//{
	//	int iMinute = m_stCurrentTime.iHour*60+m_stCurrentTime.iMinute;

	//	//时间进行取整操作
	//	iMinute = iMinute/10*10;
	//	memset(&m_stStartTime,0,sizeof(ST_TIME));
	//	int iStartMinute = iMinute - 30;
	//	m_stStartTime.iHour = iStartMinute/60;
	//	m_stStartTime.iMinute = iStartMinute%60;
	//	m_stStartTime.iScond = 00;
	//	memset(&m_stEndTime,0,sizeof(ST_TIME));
	//	int iEndMinute = iMinute + 30;
	//	m_stEndTime.iHour = iEndMinute/60;
	//	m_stEndTime.iMinute = iEndMinute%60;
	//	m_stEndTime.iScond = 00;
	//}
	//else if (TYPE_SCALE_SMALL == m_iSizeType)
	//{
	//	//小刻度
	//	int iScond = m_stCurrentTime.iHour*3600+m_stCurrentTime.iMinute*60+m_stCurrentTime.iScond;

	//	//时间进行取整操作
	//	iScond = iScond/10*10;

	//	memset(&m_stStartTime,0,sizeof(ST_TIME));
	//	int iStartTime = iScond - 90;
	//	m_stStartTime.iHour = iStartTime/3600;
	//	m_stStartTime.iMinute = (iStartTime%3600)/60;
	//	m_stStartTime.iScond = (iStartTime%3600)%60;

	//	memset(&m_stEndTime,0,sizeof(ST_TIME));
	//	int iEndTime = iScond + 90;
	//	m_stEndTime.iHour = iEndTime/3600;
	//	m_stEndTime.iMinute = (iEndTime%3600)/60;
	//	m_stEndTime.iScond = (iEndTime%3600)%60;
	//}

	////设置当前的时间区域范围和设置当前时间
	//if (m_pPlayTimerBar!=NULL)
	//{
	//	m_pPlayTimerBar->SetBarTime(m_stStartTime,m_stEndTime);
	//	m_pPlayTimerBar->SetCurrentTime(m_stCurrentTime);
	//}
	//m_bIsInit = false;
	UpdateState();
}


void CTimeStatic::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case RECORD_PLAY_TIME_ID:
		{
			AddOneScond();
		}
		break;
	default:
		break;
	}

	CTranslucentStatic::OnTimer(nIDEvent);
}

void CTimeStatic::AddOneScond()
{
	if (!m_bIsTimer)
	{
		return;
	}

	if (m_bIsMouseFlag)
	{
		return;
	}

    CHECK_POINTER_VOID(m_pPlayTimerBar);

    //判断是否是结尾，如果是就停止回放
    if (IsCurrentPlayEnd())
    {
        // 查找是否下一段时间还有录像
        ST_TIME NextTime;
        memset(&NextTime, 0, sizeof(ST_TIME));
        if (FindNextPlayRange(m_stCurrentTime, NextTime))
        {
            m_bIsMouseFlag = true;
            (void)SetCurrentTime(NextTime);
            m_bIsMouseFlag = false;
        }
        else
        {
            KillTimer(RECORD_PLAY_TIME_ID);
            m_pPlayTimerBar->PostMessage(WM_TIMERBAR_STOP_PLAY,0,0);
            m_bIsTimer = false;
        }
        return;
    }

    ST_TIMER_MSG stTimerMsg;
    memset(&stTimerMsg,0,sizeof(ST_TIMER_MSG));
    if (!IsCanPlayBack(stTimerMsg))
    {
        // 查找是否下一段时间还有录像
        ST_TIME NextTime;
        memset(&NextTime, 0, sizeof(ST_TIME));
        if (FindNextPlayRange(m_stCurrentTime, NextTime))
        {
            m_bIsMouseFlag = true;
            (void)SetCurrentTime(NextTime);
            m_bIsMouseFlag = false;
            return;
        }
        else
        {
            KillTimer(RECORD_PLAY_TIME_ID);
            TRACE("kill timer:%d\n", __LINE__);
            m_pPlayTimerBar->PostMessage(WM_TIMERBAR_STOP_PLAY,0,0);
            m_bIsTimer = false;
        }
    }

    m_pPlayTimerBar->GetPlayTime(m_stCurrentTime);
    UpdateState();
    TRACE("当前时间:%d:%d:%d\n", m_stCurrentTime.iHour, m_stCurrentTime.iMinute, m_stCurrentTime.iScond);
    
}

bool CTimeStatic::FindNextPlayRange(const ST_TIME &CurTime, ST_TIME &NextTime)
{
    double dCur = TimeToPointX(m_stCurrentTime);
    double dMin = TimeToPointX(m_stEndTime);
    g_cs.Lock();

    POSITION pos = m_ListArea.GetHeadPosition();//lint !e830
    ST_AREAINFO* pAreaInfo = NULL;
    bool bFind = false;
    while(pos!=NULL)
    {
        pAreaInfo = m_ListArea.GetNext(pos);
        if (NULL != pAreaInfo)
        {
            if (dCur <= pAreaInfo->dLeft)
            {
                if ( (pAreaInfo->dLeft - dCur) < dMin)
                {
                    dMin = (pAreaInfo->dLeft - dCur);
                    NextTime = pAreaInfo->stTime.stStartTime;
                    bFind = true;
                }
            }
        }
    }
    g_cs.Unlock();
    return bFind;
}//lint !e715

void CTimeStatic::GetTimeMsg(const ST_TIME& stTime, ST_TIMER_MSG &stTimerMsg)
{
    g_cs.Lock();

    POSITION pos = m_ListArea.GetHeadPosition();//lint !e830
    ST_AREAINFO* pAreaInfo = NULL;
    while(pos!=NULL)
    {
        pAreaInfo = m_ListArea.GetNext(pos);
        if (NULL != pAreaInfo)
        {
            if (stTime.iHour == pAreaInfo->stTime.stStartTime.iHour &&
                stTime.iMinute == pAreaInfo->stTime.stStartTime.iMinute &&
                stTime.iScond == pAreaInfo->stTime.stStartTime.iScond)
            {
                stTimerMsg.iType = 1;
                stTimerMsg.stStartTime.iHour = stTime.iHour;
                stTimerMsg.stStartTime.iMinute = stTime.iMinute;
                stTimerMsg.stStartTime.iScond = stTime.iScond;
                stTimerMsg.stEndTime.iHour = pAreaInfo->stTime.stEndTime.iHour;
                stTimerMsg.stEndTime.iMinute = pAreaInfo->stTime.stEndTime.iMinute;
                stTimerMsg.stEndTime.iScond = pAreaInfo->stTime.stEndTime.iScond;
                stTimerMsg.stCurrentTime.iHour = stTime.iHour;
                stTimerMsg.stCurrentTime.iMinute = stTime.iMinute;
                stTimerMsg.stCurrentTime.iScond = stTime.iScond;
                break;
            }
        }
    }
    g_cs.Unlock();
}

BOOL CTimeStatic::IsCanPlayBack(ST_TIMER_MSG &stTimerMsg)
{
	g_cs.Lock();

	POSITION pos = m_ListArea.GetHeadPosition();//lint !e830
	POSITION pos1;
	ST_AREAINFO* pAreaInfo = NULL;
	bool bIsSendMsg = false;
	while(pos!=NULL)
	{
		pos1 = pos;
		pAreaInfo = m_ListArea.GetNext(pos);
		if (NULL != pAreaInfo)
		{
			if (m_iCurrentPointPos>=pAreaInfo->dLeft&&m_iCurrentPointPos<=pAreaInfo->dRight)
			{
				//表示进入了区域，需要抛送消息上去
				ST_LAYERTIME stLayerTime;
				memset(&stLayerTime,0,sizeof(ST_LAYERTIME));

				stLayerTime.stStartTime.iHour = pAreaInfo->stTime.stStartTime.iHour;
				stLayerTime.stStartTime.iMinute = pAreaInfo->stTime.stStartTime.iMinute;
				stLayerTime.stStartTime.iScond = pAreaInfo->stTime.stStartTime.iScond;
				stLayerTime.stEndTime.iHour = pAreaInfo->stTime.stEndTime.iHour;
				stLayerTime.stEndTime.iMinute = pAreaInfo->stTime.stEndTime.iMinute;
				stLayerTime.stEndTime.iScond = pAreaInfo->stTime.stEndTime.iScond;
				bIsSendMsg = true;

				stTimerMsg.iType = 1;
				stTimerMsg.stStartTime.iHour = stLayerTime.stStartTime.iHour;
				stTimerMsg.stStartTime.iMinute = stLayerTime.stStartTime.iMinute;
				stTimerMsg.stStartTime.iScond = stLayerTime.stStartTime.iScond;
				stTimerMsg.stEndTime.iHour = stLayerTime.stEndTime.iHour;
				stTimerMsg.stEndTime.iMinute = stLayerTime.stEndTime.iMinute;
				stTimerMsg.stEndTime.iScond = stLayerTime.stEndTime.iScond;
				stTimerMsg.stCurrentTime.iHour = m_stCurrentTime.iHour;
				stTimerMsg.stCurrentTime.iMinute = m_stCurrentTime.iMinute;
				stTimerMsg.stCurrentTime.iScond = m_stCurrentTime.iScond;
				break;
			}
		}
	}
	g_cs.Unlock();

	return bIsSendMsg;
}//lint !e550

void CTimeStatic::StartRecordPlayTimer()
{
	m_bIsTimer = true;
	m_bCurStreamStop = false;
	
	unsigned int iRet = SetTimer(RECORD_PLAY_TIME_ID,1000,NULL);
    // 保存当前播放区域
    g_cs.Lock();

    POSITION pos = m_ListGreen.GetHeadPosition();
    ST_LAYERTIME* pLayer = NULL;
 
    while(pos!=NULL)
    {
        pLayer = m_ListGreen.GetNext(pos);
        if (NULL != pLayer)
        {
            double dCur = TimeToPointX(m_stCurrentTime);
            double dLeft = TimeToPointX(pLayer->stStartTime);
            double dRight = TimeToPointX(pLayer->stEndTime);
            if ( dCur >= dLeft && dCur <= dRight )
            {
                memcpy(&m_Layer, pLayer, sizeof(ST_LAYERTIME));
                break;
            }
        }
    }
    g_cs.Unlock();
	//if (NULL == m_hThread)
	//{
	//	DWORD dwThreadID = 0;
	//	m_hThread = CreateThread(NULL,0,TimerThread,this,0,&dwThreadID);
	//}
}//lint !e529 !e550

void CTimeStatic::StopRecordPlayTime()
{
	if (m_bIsTimer)
	{
		m_bIsTimer = false;
		KillTimer(RECORD_PLAY_TIME_ID);
        TRACE("kill timer:%d\n", __LINE__);
	}
}

DWORD WINAPI CTimeStatic::TimerThread(LPVOID pParam)
{
	/*CTimeStatic* pStatic = (CTimeStatic*)pParam;

	while(pStatic->m_bIsTimer)
	{
		ST_TIME stCurrent;
		stCurrent.iHour = pStatic->m_stCurrentTime.iHour;
		stCurrent.iMinute = pStatic->m_stCurrentTime.iMinute;
		stCurrent.iScond = pStatic->m_stCurrentTime.iScond;
		Sleep(1000);
		pStatic->m_stCurrentTime.iHour = stCurrent.iHour;
		pStatic->m_stCurrentTime.iMinute = stCurrent.iMinute;
		pStatic->m_stCurrentTime.iScond = stCurrent.iScond;

		pStatic->AddOneScond();
		if (pStatic->IsPlayEnd())
		{
			pStatic->StopRecordPlayTime();
			break;
		}
	}
	pStatic->m_hThread = NULL;*/
	return 0;
}//lint !e818 !e715

BOOL CTimeStatic::IsPlayEnd()
{
	CRect rc;
	GetWindowRect(&rc);//lint !e64 !e118
	GetParent()->ScreenToClient(&rc);//lint !e64 !e118

	ST_TIMER_MSG stTest;
	memset(&stTest,0,sizeof(ST_TIMER_MSG));
	CHECK_POINTER(m_pPlayTimerBar,FALSE);
	if (m_iCurrentPointPos<rc.left || m_iCurrentPointPos >rc.right || abs(m_iCurrentPointPos - rc.Width()) < 0.01)
	{
		m_pPlayTimerBar->StopVideoPlay();
		return TRUE;
	}
	else if ((!IsCanPlayBack(stTest)))
	{
		m_pPlayTimerBar->StopVideoPlay();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CTimeStatic::ResetTimer()const
{
	//Clear();
	//UpdateState();
	//Invalidate();

	//Clear();
	//Invalidate();
	//Invalidate();
	//Invalidate();
	//UpdateState();
	//SetFocus();
	//OnLButtonUp(1,CPoint(0,0));
	//SetCurrentTime(m_stCurrentTime);
	//UpdateState();
	//Clear();
	//ClearGreenList();
}

void CTimeStatic::SetCurrentPaneToActivePane()
{
	CVideoPane* pVideoPane = dynamic_cast<CVideoPane*>(m_pVideoPane);
	CHECK_POINTER_VOID(pVideoPane);
	pVideoPane->SetActivePane();
}

void CTimeStatic::SetStartPlayTime(const ST_TIME &stPlaytime)
{
    m_stStartPlayTime = stPlaytime;
}

bool CTimeStatic::IsInCurrentArea( const CPoint &point )
{
    ST_TIME CurTime = PointToTime(point);
    return IsInCurrentArea(CurTime);
}

bool CTimeStatic::IsInCurrentArea( const ST_TIME &time )
{
    double dCurPos = TimeToPointX(time);
    double dLeft = TimeToPointX(m_Layer.stStartTime);
    double dRight = TimeToPointX(m_Layer.stEndTime);
    if (dCurPos>=dLeft&&dCurPos<=dRight)
    {
        return true;
    }
    else
    {
        return false;
    }
}

BOOL CTimeStatic::IsCurrentPlayEnd()
{
	return (BOOL)m_bCurStreamStop;
#if 0	
	double dRight = TimeToPointX(m_Layer.stEndTime);
	double dCur = TimeToPointX(m_stCurrentTime);
	if (dCur >= dRight)
	{
		return TRUE;
	}
	return FALSE;
#endif 
}//lint !e550 !e1762

