#include "stdafx.h"
#include "LayoutMgr.h"
#include "Utility.h"

CLayoutMgr::CLayoutMgr(void)
{
	m_vector_Layout.clear();
}

CLayoutMgr::~CLayoutMgr(void)
{
}

// 布局管理器单例
CLayoutMgr& CLayoutMgr::GetInstance()
{
	static CLayoutMgr LayOutInstance;
	return LayOutInstance;
}

bool CLayoutMgr::isLadderLayout_12( const unsigned long _ulPaneNUm) const
{
	bool bResult = true;
	if (_ulPaneNUm < 11  ||  _ulPaneNUm > 12)
	{
		bResult =  false;
	}
	return bResult;
}

bool CLayoutMgr::isLadderLayout_10( const unsigned long _ulPaneNUm) const
{
	bool bResult = true;
	if (_ulPaneNUm < 9  ||  _ulPaneNUm > 10)
	{
		bResult =  false;
	}
	return bResult;
}

bool CLayoutMgr::isLadderLayout_8( const unsigned long _ulPaneNUm) const
{
	bool bResult = true;
	if (_ulPaneNUm < 7  ||  _ulPaneNUm > 8)
	{
		bResult =  false;
	}
	return bResult;
}

bool CLayoutMgr::isLadderLayout_6( const unsigned long _ulPaneNUm) const
{
	bool bResult = true;
	if (_ulPaneNUm < 4 || _ulPaneNUm > 6)
	{
		bResult =  false;
	}
	return bResult;
}

bool CLayoutMgr::isLadderLayout_16( const unsigned long _ulPaneNUm) const
{
	bool bResult = true;
	if (_ulPaneNUm < 13 || _ulPaneNUm > 16)
	{
		bResult =  false;
	}
	return bResult;
}

bool CLayoutMgr::isLadderLayout_20( const unsigned long _ulPaneNUm) const
{
	bool bResult = true;
	if (_ulPaneNUm < 17 || _ulPaneNUm > 20)
	{
		bResult =  false;
	}
	return bResult;
}

bool CLayoutMgr::isLadderLayout_26( const unsigned long _ulPaneNUm) const
{
	bool bResult = true;
	if (_ulPaneNUm < 21 || _ulPaneNUm > 26)
	{
		bResult =  false;
	}
	return bResult;
}

// Method:    InitLayOut
void CLayoutMgr::InitLayOut()
{
	InitAllLayout();
	InitRegulationLay();
	InitLadderLayOut();
	InitAnomalousLayoutLayOut();
}

// 检查摄像机类型合法性
IVS_ULONG CLayoutMgr::CheckCameraType(IVS_ULONG ulCameraType)const
{
	if ((CAMERA_TYPE_CAVITY_FIXED == ulCameraType)
		|| (CAMERA_TYPE_CAVITY_WITHPTZ == ulCameraType)
		|| (CAMERA_TYPE_BALL == ulCameraType))
	{
		return IVS_SUCCEED;
	}

	BP_RUN_LOG_INF("", "Layout Manager check  camera type error.");
	return IVS_FAIL;
}

// 检查客户端类型合法性
IVS_ULONG CLayoutMgr::CheckClientType(IVS_ULONG ulClientType )const
{
	if ((CLIENT_PC == ulClientType)
		|| (CLIENT_WEB == ulClientType)
        || (CLIENT_MOBILE ==ulClientType)
        || (CLIENT_PC_CHILD ==ulClientType))
	{
		return IVS_SUCCEED;
	}

	BP_RUN_LOG_INF("", "Layout Manager check client type error.");
	return IVS_FAIL;
}

// 检查窗口类型合法性
IVS_ULONG CLayoutMgr::CheckWndType(IVS_ULONG ulWndType)const
{
	if ((WND_TYPE_NORMAL == ulWndType)
		|| (WND_TYPE_REALTIME == ulWndType)
		|| (WND_TYPE_RECORD == ulWndType)
		|| (WND_TYPE_ANALYSIS == ulWndType)
        || (WND_TYPE_REPALY == ulWndType)
        || (WND_TYPE_SENSE == ulWndType))
	{
		return IVS_SUCCEED;
	}

	BP_RUN_LOG_INF("", "Layout Manager check  wnd type error.");
	return IVS_FAIL;
}

bool CLayoutMgr::CheckLayoutType( IVS_ULONG ulLayoutType )
{
	std::vector<IVS_ULONG>::iterator vlIter = 
		find(m_vector_Layout.begin(),m_vector_Layout.end(),ulLayoutType);
	if (vlIter != m_vector_Layout.end())
	{
		return true;
	}

	BP_RUN_LOG_ERR(IVS_PARA_INVALID,"IVS_OCX:: Layout Manager Check Layout Type", "Layout Type Invalid");
	return false;
}

//**********************************************
// Method:    SetLayoutAs10
//	_________________________________________________
//	|						|						|
//	|						|				2		|
//	|		1				|						|
//	|						|						|
//	|						|						|
//	|-----------------------|-----------------------|
//	|			|			|			|			|
//	|		3	|	4		|		5	|		6	|
//	|-----------|-----------|-----------|-----------|
//	|		7	|			|		9	|	10		|
//	|			|	8		|			|			|
//	---------------------------------------------------
//********************************************** 
void CLayoutMgr::SetLayoutAs10( CRect & rectPane,CVideoPaneMgr * pVideoPaneMgr)const
{ 
	CHECK_POINTER_VOID(pVideoPaneMgr);
    // 把传进来的区域分成10个区域，位置也要设定好

    CPoint topLeft = rectPane.TopLeft();
    CPoint bottomRight = rectPane.BottomRight();

    // 两个临时的点，用于初始化rect，每个窗格不一样
    CPoint topLeftTemp;
    CPoint bottomRightTemp;

    // 两坐标平均增长长度
    LONG averageX = rectPane.Width() / 4;
    LONG averageY = rectPane.Height() / 4;
    CRect _Rect;

    //第一个窗格
    topLeftTemp.x = topLeft.x;
    topLeftTemp.y = topLeft.y;
    bottomRightTemp.x = topLeft.x + averageX * 2;
    bottomRightTemp.y = topLeft.y + averageY * 2;

    // 创建窗格
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    //第二个窗格
    topLeftTemp.x = topLeft.x + averageX * 2;
    topLeftTemp.y = topLeft.y;
    bottomRightTemp.x = bottomRight.x;
    bottomRightTemp.y = topLeft.y + averageY * 2;

    // 创建窗格
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    // 下面两行窗格

	topLeftTemp.y = topLeft.y + averageY * 2;
	bottomRightTemp.y = topLeft.y + averageY * 3;

	for (IVS_INT32 iCount = 0; iCount < 4; ++iCount)
	{
		topLeftTemp.x = topLeft.x + averageX *  iCount;
		bottomRightTemp.x = topLeft.x + averageX *(iCount+1);
        _Rect.SetRect(topLeftTemp,bottomRightTemp);
        pVideoPaneMgr->SetVideoPanePosition(_Rect);
	}

	// 下面两行窗格

	topLeftTemp.y = topLeft.y + averageY * 3;
	bottomRightTemp.y = bottomRight.y ;

	for (IVS_INT32 iCount = 0; iCount < 4; ++iCount)
	{
		topLeftTemp.x = topLeft.x + averageX *  iCount;
		bottomRightTemp.x = topLeft.x + averageX *(iCount+1);
        _Rect.SetRect(topLeftTemp,bottomRightTemp);
        pVideoPaneMgr->SetVideoPanePosition(_Rect);
	}
}

void CLayoutMgr::SetLayoutVerticalX( const CRect &rectPane,IVS_INT32 iNum ,CVideoPaneMgr *pVideoPaneMgr)const
{
	if (iNum < 2 || iNum > 8)
	{
		return;
	}

	CHECK_POINTER_VOID(pVideoPaneMgr);

	CPoint topLeftTemp;
	CPoint bottomRightTemp;

	LONG averageY = rectPane.Height() / iNum;
    CRect _Rect;

	topLeftTemp.x = rectPane.left;
	bottomRightTemp.x = rectPane.right;

	for (IVS_INT32 iCount = 0; iCount < iNum; ++iCount)
	{
		topLeftTemp.y = rectPane.top + averageY * iCount;
		bottomRightTemp.y = rectPane.top + averageY*(iCount + 1);
        _Rect.SetRect(topLeftTemp,bottomRightTemp);
        pVideoPaneMgr->SetVideoPanePosition(_Rect);
	}
}

void CLayoutMgr::SetLayoutHorizontalX( const CRect &rectPane,IVS_INT32 iNum ,CVideoPaneMgr *pVideoPaneMgr)const {
	if (iNum < 2 || iNum > 8)
	{
		return;
	}

	CHECK_POINTER_VOID(pVideoPaneMgr);

	CPoint topLeftTemp;
	CPoint bottomRightTemp;

	LONG averageX = rectPane.Width() / iNum;
    CRect _Rect;

	topLeftTemp.y = rectPane.top;
	bottomRightTemp.y = rectPane.bottom;

	for (IVS_INT32 iCount = 0; iCount < iNum; ++iCount)
	{
		topLeftTemp.x = rectPane.left + averageX * iCount;
		bottomRightTemp.x = rectPane.left + averageX*(iCount + 1);
        _Rect.SetRect(topLeftTemp,bottomRightTemp);
        pVideoPaneMgr->SetVideoPanePosition(_Rect);
	}
}

//**********************************************
// Method:    SetLayoutAs13
//---------------------------------------
//|        |        |         |			|
//|   1    |   2    |    3    |		4	|
//|-------------------------------------|
//|        |                  |			|
//|   5    |                  |		7	|
//|--------|        6         |---------|
//|        |                  |			|
//|    8   |                  |		9	|
//|-------------------------------------|
//|    10  |   11   |    12   |		13	|
//|        |        |         |			|
//---------------------------------------
//**********************************************
void CLayoutMgr::SetLayoutAs13( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr)const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);
    CPoint topLeft = rectPane.TopLeft();
    CPoint bottomRight = rectPane.BottomRight();

    // 两个临时的点，用于初始化rect，每个窗格不一样
    CPoint topLeftTemp;
    CPoint bottomRightTemp;

    // 两坐标平均增长长度
    LONG averageX = rectPane.Width() / 4;
    LONG averageY = rectPane.Height() / 4;
    // 第一行窗格
    topLeftTemp.x = topLeft.x;
    topLeftTemp.y = topLeft.y;
    bottomRightTemp.x = topLeft.x + averageX;
    bottomRightTemp.y = topLeft.y + averageY;
    CRect _Rect;

    int i;
    for (i = 0; i < 4; i++)
    {
        _Rect.SetRect(topLeftTemp,bottomRightTemp);
        pVideoPaneMgr->SetVideoPanePosition(_Rect);

        //这个判断是用于当窗格为最右边或最下边时，窗格要贴近边缘，不留下几个像素的黑边
        if (i == 2)
        {
            topLeftTemp.x += averageX;
            bottomRightTemp.x = bottomRight.x;
        } else {
            topLeftTemp.x += averageX;
            bottomRightTemp.x += averageX;
        }
    }

    // 4号窗格
    topLeftTemp.x = topLeft.x;
    topLeftTemp.y = topLeft.y + averageY;
    bottomRightTemp.x = topLeft.x + averageX;
    bottomRightTemp.y = topLeft.y + averageY * 2;

    // 创建窗格
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    // 大窗格（5号）
    topLeftTemp.x = topLeft.x + averageX;
    topLeftTemp.y = topLeft.y + averageY;
    bottomRightTemp.x = topLeft.x + averageX * 3;
    bottomRightTemp.y = topLeft.y + averageY * 3;

    // 创建窗格
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    // 6号窗格
    topLeftTemp.x = topLeft.x + averageX * 3;
    topLeftTemp.y = topLeft.y + averageY;
    bottomRightTemp.x = bottomRight.x;
    bottomRightTemp.y = topLeft.y + averageY * 2;

    // 创建窗格
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    // 7，8号窗格
    topLeftTemp.x = topLeft.x;
    topLeftTemp.y = topLeft.y + averageY * 2;
    bottomRightTemp.x = topLeft.x + averageX;
    bottomRightTemp.y = topLeft.y + averageY * 3;
    for (i = 0; i < 2; i++)
    {
        _Rect.SetRect(topLeftTemp,bottomRightTemp);
        pVideoPaneMgr->SetVideoPanePosition(_Rect);

        topLeftTemp.x += (averageX * 3);
        bottomRightTemp.x = bottomRight.x;
    }

    // 底部一行窗格
    topLeftTemp.x = topLeft.x;
    topLeftTemp.y = topLeft.y + averageY * 3;
    bottomRightTemp.x = topLeft.x + averageX;
    bottomRightTemp.y = bottomRight.y;
    for (i = 0; i < 4; i++)
    {
        _Rect.SetRect(topLeftTemp,bottomRightTemp);
        pVideoPaneMgr->SetVideoPanePosition(_Rect);

        //这个判断是用于当窗格为最右边或最下边时，窗格要贴近边缘，不留下几个像素的黑边
        if (i == 2)
        {
            topLeftTemp.x += averageX;
            bottomRightTemp.x = bottomRight.x;
        }
        else
        {
            topLeftTemp.x += averageX;
            bottomRightTemp.x += averageX;
        }
    }

    return;
}

//**********************************************
// Method:    SetLayoutAs6_1
//	_____________________________
//	|                 |         |
//	|                 |    2    |
//	|       1         |_________|
//	|                 |         |
//	|                 |    3    |
//	|				  |_________|
//	|				  |			|	
//	|                 |    4	|
//	|_________________|_________|
//	|                 |         |
//	|    5            |    6    |
//	|_________________|_________|
//**********************************************
void CLayoutMgr::SetLayoutAs6_1( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);
    // 把传进来的区域分成6个区域，位置也要设定好

    CPoint topLeft = rectPane.TopLeft();
    CPoint bottomRight = rectPane.BottomRight();

    // 两个临时的点，用于初始化rect，每个窗格不一样
    CPoint topLeftTemp;
    CPoint bottomRightTemp;

    // 两坐标平均增长长度
    LONG averageX = rectPane.Width() / 4;
    LONG averageY = rectPane.Height() / 4;
    CRect _Rect;

    //区域1的rect
    topLeftTemp.x = topLeft.x;
    topLeftTemp.y = topLeft.y;
    bottomRightTemp.x = bottomRight.x - averageX;
    bottomRightTemp.y = bottomRight.y - averageY;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    // rect 2-4
    topLeftTemp.x = bottomRight.x - averageX;
    topLeftTemp.y = topLeft.y ;
    bottomRightTemp.x = bottomRight.x;
    bottomRightTemp.y = bottomRight.y - averageY;
    SetLayoutVerticalX(CRect(topLeftTemp, bottomRightTemp),3,pVideoPaneMgr);

    //区域5的rect
    topLeftTemp.x = topLeft.x;
    topLeftTemp.y =  bottomRight.y - averageY;
    bottomRightTemp.x = bottomRight.x - averageX;
    bottomRightTemp.y = bottomRight.y ;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    //区域6的rect
    topLeftTemp.x = bottomRight.x - averageX;
    topLeftTemp.y =  bottomRight.y - averageY;
    bottomRightTemp.x = bottomRight.x;
    bottomRightTemp.y = bottomRight.y ;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);
}

//**********************************************
// Method:    SetLayoutAs12_1
// Qualifier: 4 * 3 
//**********************************************
void CLayoutMgr::SetLayoutAs12_1( CRect &rectPane ,CVideoPaneMgr *pVideoPaneMgr)const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);
	CPoint topLeft = rectPane.TopLeft();

	// 两个临时的点，用于初始化rect，每个窗格不一样
	CPoint topLeftTemp;
	CPoint bottomRightTemp;

	// 两坐标平均增长长度
	LONG averageX = rectPane.Width() / 4;
	LONG averageY = rectPane.Height() / 3;
    CRect _Rect;

	//区域1的rect
	for (IVS_INT32 iCountY = 0; iCountY < 3; ++iCountY)
	{
		for (IVS_INT32 iCountX = 0; iCountX < 4; ++iCountX)
		{
			topLeftTemp.y = topLeft.y + averageY * iCountY;
			bottomRightTemp.y = topLeft.y + averageY * (iCountY +1);

			topLeftTemp.x = topLeft.x + averageX * iCountX;
			bottomRightTemp.x = topLeft.x + averageX * (iCountX + 1) ;
            _Rect.SetRect(topLeftTemp,bottomRightTemp);
            pVideoPaneMgr->SetVideoPanePosition(_Rect);
		}
	}
}

//**********************************************
// Method:    SetLayoutAs9_1
// Qualifier: 宽屏
//	_________________________________________________
//	|			|						|			|
//	|			|						|			|
//	|		1	|						|		3	|
//	|___________|				2		|___________|
//	|			|						|			|
//	|		4	|						|			|
//	|			|						|		5	|
//	|___________|_______________________|___________|
//	|			|			|			|			|
//	|			|			|	8		|	9		|
//	|		6	|		7	|			|			|
//	|___________|___________|___________|___________|									
//********************************************** 
void CLayoutMgr::SetLayoutAs9_1( CRect &rectPane ,CVideoPaneMgr *pVideoPaneMgr)const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);

	CPoint topLeft = rectPane.TopLeft();
	CPoint bottomRight = rectPane.BottomRight();

	// 两个临时的点，用于初始化rect，每个窗格不一样
	CPoint topLeftTemp;
	CPoint bottomRightTemp;

	// 两坐标平均增长长度
	LONG averageX = rectPane.Width() / 4;
	LONG averageY = rectPane.Height() / 3;
    CRect _Rect;

    // Rect 1
	topLeftTemp.x = topLeft.x;
	bottomRightTemp.x = topLeft.x + averageX;
    topLeftTemp.y = topLeft.y;;
    bottomRightTemp.y = topLeft.y + averageY;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    // Rect 2
    topLeftTemp.x = topLeft.x + averageX;
    bottomRightTemp.x = bottomRight.x - averageX;
    topLeftTemp.y = topLeft.y;;
    bottomRightTemp.y = bottomRight.y - averageY;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    // Rect 3
    topLeftTemp.x = bottomRight.x - averageX;
    bottomRightTemp.x = bottomRight.x;
    topLeftTemp.y = topLeft.y;;
    bottomRightTemp.y = topLeft.y + averageY;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    // Rect 4
    topLeftTemp.x = topLeft.x;
    bottomRightTemp.x = topLeft.x + averageX;
    topLeftTemp.y =  topLeft.y + averageY;;
    bottomRightTemp.y = topLeftTemp.y + averageY;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    // Rect 5
    topLeftTemp.x = bottomRight.x - averageX;
    bottomRightTemp.x = bottomRight.x;
    topLeftTemp.y =  topLeft.y + averageY;;
    bottomRightTemp.y = topLeftTemp.y + averageY;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    // Rect 6-9
    topLeftTemp.x = topLeft.x;
    bottomRightTemp.x = bottomRight.x;
    topLeftTemp.y = bottomRight.y - averageY;
    bottomRightTemp.y = bottomRight.y;
    SetLayoutHorizontalX(CRect(topLeftTemp, bottomRightTemp),4,pVideoPaneMgr);
}

//**********************************************
// Method:    SetLayoutAs13_2
//	_________________________________________
//	|               |				|	3	|
//	|		1		|		2		|_______|
//	|				|				|	4	|
//	|_______________|_______________|_______|
//	|               |				|	7	|
//	|			5	|		6		|_______|
//	|				|				|	8	|
//	|_______________|_______________|_______|
//	|	9	|	10	|	11	|	12	|	13	|
//	|_______|_______|_______|_______|_______|
//********************************************** 
void CLayoutMgr::SetLayoutAs13_2( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);
	CPoint topLeft = rectPane.TopLeft();
	CPoint bottomRight = rectPane.BottomRight();

	// 两个临时的点，用于初始化rect，每个窗格不一样
	CPoint topLeftTemp;
	CPoint bottomRightTemp;

	// 两坐标平均增长长度
	LONG averageX = rectPane.Width() / 5;
	LONG averageY = rectPane.Height() / 5;

    // Rect 1-2
	topLeftTemp.x = topLeft.x;
	bottomRightTemp.x = bottomRight.x - averageX;
	topLeftTemp.y = topLeft.y;
	bottomRightTemp.y = topLeft.y + averageY*2;
    SetLayoutHorizontalX(CRect(topLeftTemp, bottomRightTemp),2,pVideoPaneMgr);


    // Rect 3-4
    topLeftTemp.x = bottomRight.x - averageX;
    bottomRightTemp.x = bottomRight.x ;
    topLeftTemp.y = topLeft.y;
    bottomRightTemp.y = topLeft.y + averageY*2;
    SetLayoutVerticalX(CRect(topLeftTemp, bottomRightTemp),2,pVideoPaneMgr);

    // Rect 5-6
    topLeftTemp.x = topLeft.x;
    bottomRightTemp.x = bottomRight.x - averageX;
    topLeftTemp.y = topLeft.y + averageY*2;
    bottomRightTemp.y = bottomRight.y - averageY;
    SetLayoutHorizontalX(CRect(topLeftTemp, bottomRightTemp),2,pVideoPaneMgr);


    // Rect 7-8
    topLeftTemp.x = bottomRight.x - averageX;
    bottomRightTemp.x = bottomRight.x;
    topLeftTemp.y = topLeft.y + averageY*2;
    bottomRightTemp.y = bottomRight.y - averageY;
    SetLayoutVerticalX(CRect(topLeftTemp, bottomRightTemp),2,pVideoPaneMgr);

    // Rect 9-13
    topLeftTemp.x = topLeft.x;
    bottomRightTemp.x = bottomRight.x;
    topLeftTemp.y = bottomRight.y - averageY;
    bottomRightTemp.y = bottomRight.y;
    SetLayoutHorizontalX(CRect(topLeftTemp, bottomRightTemp),5,pVideoPaneMgr);
}

//**********************************************
// Method:    SetLayoutAs17_1
//	_________________________________________
//	|	1	|	2	|	3	|	4	|	5	|
//	|_______|_______|_______|_______|_______|
//	|	6	|               		|	8	|
//	|_______|		 	7	 		|_______|
//	|	9	|						|	11	|
//	|_______|						|_______|
//	|	10	|               		|	12	|
//	|_______|_______________________|_______|
//	|	13	|	14	|	15	|	16	|	17	|
//	|_______|_______|_______|_______|_______|
//**********************************************
void CLayoutMgr::SetLayoutAs17_1( CRect &rectPane ,CVideoPaneMgr *pVideoPaneMgr)const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);
	CPoint topLeft = rectPane.TopLeft();
	CPoint bottomRight = rectPane.BottomRight();

	// 两个临时的点，用于初始化rect，每个窗格不一样
	CPoint topLeftTemp;
	CPoint bottomRightTemp;

	// 两坐标平均增长长度
	LONG averageX = rectPane.Width() / 5;
	LONG averageY = rectPane.Height() / 5;
    CRect _Rect;

    //区域1~5的rect
	topLeftTemp.y = topLeft.y;
    topLeftTemp.x = topLeft.x;
	bottomRightTemp.x = bottomRight.x;
    bottomRightTemp.y = topLeft.y + averageY;

    SetLayoutHorizontalX(CRect(topLeftTemp, bottomRightTemp),5,pVideoPaneMgr);

    //区域6
	topLeftTemp.x = topLeft.x;
	bottomRightTemp.x = topLeft.x + averageX;
    topLeftTemp.y = topLeft.y + averageY;
    bottomRightTemp.y = topLeftTemp.y + averageY;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	//区域7的rect
    topLeftTemp.x = topLeft.x + averageX;
    bottomRightTemp.x = bottomRight.x - averageX;
    topLeftTemp.y = topLeft.y + averageY;
    bottomRightTemp.y = bottomRight.y - averageY;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    //区域8的rect
    topLeftTemp.x = bottomRight.x - averageX;
    bottomRightTemp.x = bottomRight.x;
    topLeftTemp.y = topLeft.y + averageY;
    bottomRightTemp.y = topLeftTemp.y + averageY;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	//区域9~10的rect
    topLeftTemp.x = topLeft.x;
    bottomRightTemp.x = topLeft.x + averageX;
	topLeftTemp.y = topLeft.y + averageY * 2;
	bottomRightTemp.y = bottomRight.y - averageY;

    SetLayoutVerticalX(CRect(topLeftTemp, bottomRightTemp),2,pVideoPaneMgr);

    //区域11~12的rect
    topLeftTemp.x = bottomRight.x - averageX;
    bottomRightTemp.x = bottomRight.x;
    topLeftTemp.y = topLeft.y + averageY * 2;
    bottomRightTemp.y = bottomRight.y - averageY;

    SetLayoutVerticalX(CRect(topLeftTemp, bottomRightTemp),2,pVideoPaneMgr);

    //区域13~17的rect
    topLeftTemp.x = topLeft.x;
    bottomRightTemp.x = bottomRight.x;
	topLeftTemp.y = bottomRight.y  - averageY;
	bottomRightTemp.y = bottomRight.y;
    SetLayoutHorizontalX(CRect(topLeftTemp, bottomRightTemp),5,pVideoPaneMgr);
}

//**********************************************
// Method:    SetLayoutAs4_2
// 	_________________________________
//	|					|			|
//	|					|		2	|
//	|			1		|-----------|
//	|					|			|
//	|					|		3	|
//	|___________________|___________|
//	|								|
//	|							4	|
//	|_______________________________|
//**********************************************
void CLayoutMgr::SetLayoutAs4_2( CRect &rectPane ,CVideoPaneMgr *pVideoPaneMgr)const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);
	CPoint topLeft = rectPane.TopLeft();
	CPoint bottomRight = rectPane.BottomRight();

	// 两个临时的点，用于初始化rect，每个窗格不一样
	CPoint topLeftTemp;
	CPoint bottomRightTemp;

	// 两坐标平均增长长度
	LONG averageX = rectPane.Width() / 3;
	LONG averageY = rectPane.Height() / 3;
    CRect _Rect;

	//区域1的rect
	topLeftTemp.y = topLeft.y;
	bottomRightTemp.y = topLeft.y + averageY * 2;

	topLeftTemp.x = topLeft.x;
	bottomRightTemp.x = topLeft.x + averageX *2 ;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	//区域2的rect
	topLeftTemp.x = topLeft.x + averageX * 2;
	bottomRightTemp.x =bottomRight.x;

	topLeftTemp.y = topLeft.y;
	bottomRightTemp.y = topLeft.y + averageY ;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);
	//区域3的rect
	topLeftTemp.y = topLeft.y + averageY;
	bottomRightTemp.y = topLeft.y + averageY * 2 ;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	//区域2的rect
	topLeftTemp.x = topLeft.x;
	bottomRightTemp.x =bottomRight.x;

	topLeftTemp.y = topLeft.y + averageY * 2;
	bottomRightTemp.y = bottomRight.y ;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);
}

//**********************************************
// Method:    SetLayoutAs4_3
//	---------------------------------
//	|					|			|
//	|					|		2	|
//	|			1		|-----------|
//	|					|			|
//	|					|		3	|
//	|					|-----------|
//	|					|			|
//	|					|		4	|
//	---------------------------------
//**********************************************
void CLayoutMgr::SetLayoutAs4_3( CRect &rectPane ,CVideoPaneMgr *pVideoPaneMgr)const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);
	CPoint topLeft = rectPane.TopLeft();
	CPoint bottomRight = rectPane.BottomRight();

	// 两个临时的点，用于初始化rect，每个窗格不一样
	CPoint topLeftTemp;
	CPoint bottomRightTemp;

	// 两坐标平均增长长度
	LONG averageX = rectPane.Width() / 4;
	LONG averageY = rectPane.Height() / 3;
    CRect _Rect;

	//区域1的rect
	topLeftTemp.y = topLeft.y;
	bottomRightTemp.y =bottomRight.y;

	topLeftTemp.x = topLeft.x;
	bottomRightTemp.x = topLeft.x + averageX *3 ;

    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	//区域2的rect
	topLeftTemp.x = topLeft.x + averageX * 3;
	bottomRightTemp.x =bottomRight.x;

	for (IVS_INT32 iCount = 0; iCount < 3; ++ iCount)
	{
		topLeftTemp.y = topLeft.y + iCount * averageY;
		bottomRightTemp.y = topLeft.y + averageY * (iCount +1) ;
        _Rect.SetRect(topLeftTemp,bottomRightTemp);
        pVideoPaneMgr->SetVideoPanePosition(_Rect);
	}
}

//**********************************************
// Method:    SetLayoutAs3_1
// FullName:  CVideoPaneMgr::SetLayoutAs3_1
//	---------------------------------
//	|				|				|
//	|				|    2			|
//	|       1		|				|
//	|				|    			|
//	|				|				|
//	---------------------------------
//	|								|
//	|								|
//	|				3				|
//	|								|
//	|								|
//	---------------------------------
//**********************************************
void CLayoutMgr::SetLayoutAs3_1( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);
	// 把传进来的区域分成6个区域，位置也要设定好
	CPoint topLeft = rectPane.TopLeft();
	CPoint bottomRight = rectPane.BottomRight();

	// 两个临时的点，用于初始化rect，每个窗格不一样
	CPoint topLeftTemp;
	CPoint bottomRightTemp;

	// 两坐标平均增长长度
	LONG averageX = rectPane.Width() / 2;
	LONG averageY = rectPane.Height() / 2;
    CRect _Rect;

	//区域1的rect
	topLeftTemp.x = topLeft.x;
	topLeftTemp.y = topLeft.y;
	bottomRightTemp.x = topLeft.x + averageX;
	bottomRightTemp.y = topLeft.y + averageY;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	//区域2的rect
	topLeftTemp.x = topLeft.x + averageX;
	bottomRightTemp.x = bottomRight.x;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	//区域3的rect
	topLeftTemp.x = topLeft.x;
	topLeftTemp.y = topLeft.y + averageY;
	bottomRightTemp.x = bottomRight.x;
	bottomRightTemp.y = bottomRight.y;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);
}

//**********************************************
// Method:    SetLayoutAs3_2
//	---------------------------------
//	|								|
//	|				    			|
//	|       1						|
//	|				    			|
//	|								|
//	---------------------------------
//	|				|				|
//	|				|				|
//	|		2		|		3		|
//	|				|				|
//	|				|				|
//	---------------------------------
//**********************************************
void CLayoutMgr::SetLayoutAs3_2( CRect &rectPane ,CVideoPaneMgr *pVideoPaneMgr)const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);
	// 把传进来的区域分成6个区域，位置也要设定好

	CPoint topLeft = rectPane.TopLeft();
	CPoint bottomRight = rectPane.BottomRight();

	// 两个临时的点，用于初始化rect，每个窗格不一样
	CPoint topLeftTemp;
	CPoint bottomRightTemp;

	// 两坐标平均增长长度
	LONG averageX = rectPane.Width() / 2;
	LONG averageY = rectPane.Height() / 2;
    CRect _Rect;

	//区域1的rect
	topLeftTemp.x = topLeft.x;
	topLeftTemp.y = topLeft.y;
	bottomRightTemp.x = bottomRight.x;;
	bottomRightTemp.y = topLeft.y + averageY;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	//区域2的rect
	topLeftTemp.x = topLeft.x;
	topLeftTemp.y = topLeft.y + averageY;
	bottomRightTemp.x = topLeft.x + averageX;
	bottomRightTemp.y = bottomRight.y;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	//区域3的rect
	topLeftTemp.x = topLeft.x + averageX;
	bottomRightTemp.x = bottomRight.x;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);
}

//**********************************************
// Method:    SetLayoutAs6_2
// Qualifier: 宽屏
//	---------------------------------
//	|				|				|
//	|				|    			|
//	|       1		|		2		|
//	|				|    			|
//	|				|				|
//	---------------------------------
//	|		|		|		|		|
//	|	3	|	4	|	5	|	6	|
//	|		|		|		|		|
//	---------------------------------
//**********************************************
void CLayoutMgr::SetLayoutAs6_2( CRect &rectPane ,CVideoPaneMgr *pVideoPaneMgr)const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);
	// 把传进来的区域分成6个区域，位置也要设定好

	CPoint topLeft = rectPane.TopLeft();
	CPoint bottomRight = rectPane.BottomRight();

	// 两个临时的点，用于初始化rect，每个窗格不一样
	CPoint topLeftTemp;
	CPoint bottomRightTemp;

	// 两坐标平均增长长度
	LONG averageX = rectPane.Width() / 4;
	LONG averageY = rectPane.Height() / 8;
    CRect _Rect;

	//区域1的rect
	topLeftTemp.y = topLeft.y;
	bottomRightTemp.y = topLeft.y + averageY * 5;

	topLeftTemp.x = topLeft.x;
	bottomRightTemp.x = topLeft.x + averageX *2 ;
	
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	//区域2的rect
	topLeftTemp.x = topLeft.x + averageX *2;
	bottomRightTemp.x =bottomRight.x;
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	topLeftTemp.y = topLeft.y + averageY * 5;
	bottomRightTemp.y = bottomRight.y;

	//区域3、4、5、6的rect
	for (IVS_INT32 iCount = 0;iCount < 4 ; ++iCount)
	{
		topLeftTemp.x = topLeft.x + averageX * iCount;
		bottomRightTemp.x = topLeft.x + averageX*(iCount +1);
        _Rect.SetRect(topLeftTemp,bottomRightTemp);
        pVideoPaneMgr->SetVideoPanePosition(_Rect);
	}
}

//**********************************************
// Method:    SetLayoutAs6_2
// Qualifier: 3 * 2 
//**********************************************
void CLayoutMgr::SetLayoutAs6_4( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);
	// 把传进来的区域分成6个区域，位置也要设定好

	CPoint topLeft = rectPane.TopLeft();

	// 两个临时的点，用于初始化rect，每个窗格不一样
	CPoint topLeftTemp;
	CPoint bottomRightTemp;

	// 两坐标平均增长长度
	
	LONG averageX = rectPane.Width() / 3;
	LONG averageY = rectPane.Height() / 2;
    CRect _Rect;

	for (LONG iCountY = 0 ; iCountY < 2; ++iCountY)
	{
		topLeftTemp.y = topLeft.y +  averageY*iCountY ;
		bottomRightTemp.y = topLeft.y + averageY * (iCountY + 1);
		for (LONG iCountX = 0; iCountX < 3; ++iCountX)
		{
			topLeftTemp.x = topLeft.x +  averageX*iCountX;
			bottomRightTemp.x = topLeft.x + averageX * (iCountX + 1) ;
            _Rect.SetRect(topLeftTemp,bottomRightTemp);
            pVideoPaneMgr->SetVideoPanePosition(_Rect);
		}
	}
}

//**********************************************
// Method:    SetLayoutAs3_3
//	---------------------------------
//	|					|			|
//	|					|		2	|
//	|					|			|
//	|			1		|-----------|
//	|					|			|
//	|					|		3	|
//	|					|			|
//	---------------------------------
//**********************************************
void CLayoutMgr::SetLayoutAs3_3( CRect &rectPane ,CVideoPaneMgr *pVideoPaneMgr)const
{
	CHECK_POINTER_VOID(pVideoPaneMgr);
	CPoint topLeft = rectPane.TopLeft();
	CPoint bottomRight = rectPane.BottomRight();

	// 两个临时的点，用于初始化rect，每个窗格不一样
	CPoint topLeftTemp;
	CPoint bottomRightTemp;
    CRect _Rect;

	// 两坐标平均增长长度
	LONG averageX = rectPane.Width() / 8;
	LONG averageY = rectPane.Height() / 2;

	//区域1的rect
	topLeftTemp.y = topLeft.y;
	bottomRightTemp.y =bottomRight.y;

	topLeftTemp.x = topLeft.x;
	bottomRightTemp.x = topLeft.x + averageX *5 ;

    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	//区域2的rect
	topLeftTemp.x = topLeft.x + averageX * 5;
	bottomRightTemp.x =bottomRight.x;

	topLeftTemp.y = topLeft.y;
	bottomRightTemp.y = topLeft.y + averageY ;

    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

	//区域3的rect
	topLeftTemp.y = topLeft.y + averageY;
	bottomRightTemp.y = bottomRight.y;

    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);
}

//**********************************************
// Method:    SetLadderLayout
// Qualifier: 阶梯布局
//**********************************************
IVS_ULONG CLayoutMgr::SetLadderLayout( CRect &rectPane,IVS_ULONG ulPaneNUm ,CVideoPaneMgr * pVideoPaneMgr)const
{
	CHECK_POINTER(pVideoPaneMgr,IVS_OPERATE_MEMORY_ERROR);
	LONG _ulPaneNUm = static_cast<LONG>(ulPaneNUm -1);
    if(( _ulPaneNUm < 5 ) || (_ulPaneNUm > 25))
    {
        return IVS_PARA_INVALID;
    }
    //判断 是否为奇数
    if ((_ulPaneNUm%2)== 0)
    {
        return IVS_PARA_INVALID;
    }
    CRect _Rect;
 
    // 两坐标平均增长长度
    LONG averageX = rectPane.Width() / ((_ulPaneNUm +1)/2);
    LONG averageY = rectPane.Height() / ((_ulPaneNUm +1)/2);

    // 两个临时的点，用于初始化rect，每个窗格不一样
    CPoint topLeftTemp;
    CPoint bottomRightTemp;

    //第一个 左上方最大窗格

    topLeftTemp.x = 0;
    topLeftTemp.y = 0;
    bottomRightTemp.x = averageX* ((_ulPaneNUm-1) / 2);
    bottomRightTemp.y = averageY* ((_ulPaneNUm-1) / 2);

    // 创建窗格
    _Rect.SetRect(topLeftTemp,bottomRightTemp);
    pVideoPaneMgr->SetVideoPanePosition(_Rect);

    //右侧 （ulPaneNUm - 1） / 2 个 竖放的窗格
    topLeftTemp.x = averageX* ((_ulPaneNUm-1) / 2);
    bottomRightTemp.x = rectPane.BottomRight().x;
   

    for (LONG iCount = 1; iCount <= (_ulPaneNUm-1) / 2; ++iCount)
    {
        topLeftTemp.y = static_cast<LONG>((iCount-1)* averageY);
        bottomRightTemp.y =  static_cast<LONG>(iCount* averageY);
        _Rect.SetRect(topLeftTemp,bottomRightTemp);
        pVideoPaneMgr->SetVideoPanePosition(_Rect);
    }

    // 底侧 （ulPaneNUm + 1） / 2 个 横放的窗格
    topLeftTemp.y = averageY * ((_ulPaneNUm-1) / 2);
    bottomRightTemp.y = rectPane.BottomRight().y;
    for (LONG iCount = 1; iCount <= (_ulPaneNUm + 1) / 2; ++iCount)
    {
        topLeftTemp.x = (iCount-1) * averageX;
        bottomRightTemp.x = iCount * averageX;
        _Rect.SetRect(topLeftTemp,bottomRightTemp);
        pVideoPaneMgr->SetVideoPanePosition(_Rect);
    }
    return IVS_SUCCEED;
}

bool CLayoutMgr::isRegulationLayOut( IVS_ULONG _ulPaneNUm )
{
	std::vector<IVS_ULONG>::iterator vlIter = 
		find(m_vector_RegulationLayOut.begin(),m_vector_RegulationLayOut.end(),_ulPaneNUm);
	if (vlIter != m_vector_RegulationLayOut.end())
	{
		return true;
	}

	return false;
}

bool CLayoutMgr::isTVWallLayout( IVS_ULONG _ulPaneNUm)const
{
	return (WIND_LAYOUT_TVW==_ulPaneNUm);
}

bool CLayoutMgr::isAnomalousLayOut( IVS_ULONG _ulPaneNUm ) 
{
	std::vector<IVS_ULONG>::iterator vlIter = 
		find(m_vector_AnomalousLayoutLayOut.begin(),m_vector_AnomalousLayoutLayOut.end(),_ulPaneNUm);
	if (vlIter != m_vector_AnomalousLayoutLayOut.end())
	{
		return true;
	}
	return false;
}

IVS_ULONG CLayoutMgr::SetAnomalousLayout( CRect &rectPane ,IVS_ULONG ulPaneNUm ,CVideoPaneMgr *pVideoPaneMgr)const
{
    CHECK_POINTER(pVideoPaneMgr,IVS_FAIL);
	if (IsAnomalousLayoutNum3(ulPaneNUm))
	{
		SetAnomalousLayoutNumEQ3(ulPaneNUm, rectPane,pVideoPaneMgr);
	}

	if(WIND_LAYOUT_4_2 == ulPaneNUm) {
		SetLayoutAs4_2(rectPane,pVideoPaneMgr);
	} else if(WIND_LAYOUT_6_1 == ulPaneNUm) {
		SetLayoutAs6_1(rectPane,pVideoPaneMgr);
	} else if(WIND_LAYOUT_10_1 == ulPaneNUm) {
		SetLayoutAs10(rectPane,pVideoPaneMgr);
	} else if(WIND_LAYOUT_13_1 == ulPaneNUm) {
		SetLayoutAs13(rectPane,pVideoPaneMgr);
	} else if(WIND_LAYOUT_13_2 == ulPaneNUm) {
		SetLayoutAs13_2(rectPane,pVideoPaneMgr);
	} else if(WIND_LAYOUT_17_1 == ulPaneNUm) {
		SetLayoutAs17_1(rectPane,pVideoPaneMgr);
	}  else if(WIND_LAYOUT_4_3 == ulPaneNUm) {
		SetLayoutAs4_3(rectPane,pVideoPaneMgr);
	} else if(WIND_LAYOUT_6_2 == ulPaneNUm) {
		SetLayoutAs6_2(rectPane,pVideoPaneMgr);
	} else if(WIND_LAYOUT_6_4 == ulPaneNUm) {
		SetLayoutAs6_4(rectPane,pVideoPaneMgr);
	} else if(WIND_LAYOUT_9_1 == ulPaneNUm) {
		SetLayoutAs9_1(rectPane,pVideoPaneMgr);
	}  else if(WIND_LAYOUT_12_1 == ulPaneNUm) {
		SetLayoutAs12_1(rectPane,pVideoPaneMgr);
	}  else if( WIND_LAYOUT_3_3 ==ulPaneNUm) {
        SetLayoutAs3_3(rectPane,pVideoPaneMgr);
    }

	return IVS_SUCCEED;
}

bool CLayoutMgr::isLadderLayout( IVS_ULONG _ulPaneNUm ) 
{
	std::vector<IVS_ULONG>::iterator vlIter = 
		find(m_vector_LadderLayOut.begin(),m_vector_LadderLayOut.end(),_ulPaneNUm);
	if (vlIter != m_vector_LadderLayOut.end())
	{
		return true;
	}
	return false;
}

void CLayoutMgr::InitAllLayout()
{
	m_vector_Layout.push_back(WIND_LAYOUT_1_1);
	m_vector_Layout.push_back(WIND_LAYOUT_3_1);
	m_vector_Layout.push_back(WIND_LAYOUT_3_2);
	m_vector_Layout.push_back(WIND_LAYOUT_3_3);
	m_vector_Layout.push_back(WIND_LAYOUT_4_1);
	m_vector_Layout.push_back(WIND_LAYOUT_4_2);
	m_vector_Layout.push_back(WIND_LAYOUT_4_3);
	m_vector_Layout.push_back(WIND_LAYOUT_6_1);
	m_vector_Layout.push_back(WIND_LAYOUT_6_2);
	m_vector_Layout.push_back(WIND_LAYOUT_6_3);
	m_vector_Layout.push_back(WIND_LAYOUT_6_4);
	m_vector_Layout.push_back(WIND_LAYOUT_8_1);
	m_vector_Layout.push_back(WIND_LAYOUT_9_1);
	m_vector_Layout.push_back(WIND_LAYOUT_9_2);
	m_vector_Layout.push_back(WIND_LAYOUT_10_1);
	m_vector_Layout.push_back(WIND_LAYOUT_10_2);
	m_vector_Layout.push_back(WIND_LAYOUT_12_1);
	m_vector_Layout.push_back(WIND_LAYOUT_12_2);
	m_vector_Layout.push_back(WIND_LAYOUT_13_1);
	m_vector_Layout.push_back(WIND_LAYOUT_13_2);
	m_vector_Layout.push_back(WIND_LAYOUT_16_1);
	m_vector_Layout.push_back(WIND_LAYOUT_16_2);
	m_vector_Layout.push_back(WIND_LAYOUT_17_1);
	m_vector_Layout.push_back(WIND_LAYOUT_20_1);
	m_vector_Layout.push_back(WIND_LAYOUT_25_1);
	m_vector_Layout.push_back(WIND_LAYOUT_26_1);
	m_vector_Layout.push_back(WIND_LAYOUT_36_1);
	m_vector_Layout.push_back(WIND_LAYOUT_49_1);
	m_vector_Layout.push_back(WIND_LAYOUT_64_1);
	m_vector_Layout.push_back(WIND_LAYOUT_CUSTOM_1);
	m_vector_Layout.push_back(WIND_LAYOUT_CUSTOM_2);
	m_vector_Layout.push_back(WIND_LAYOUT_CUSTOM_3);
	m_vector_Layout.push_back(WIND_LAYOUT_CUSTOM_4);
	m_vector_Layout.push_back(WIND_LAYOUT_TVW);
}

void CLayoutMgr::InitRegulationLay()
{
	m_vector_RegulationLayOut.push_back(WIND_LAYOUT_1_1);
	m_vector_RegulationLayOut.push_back(WIND_LAYOUT_4_1);
	m_vector_RegulationLayOut.push_back(WIND_LAYOUT_9_2);
	m_vector_RegulationLayOut.push_back(WIND_LAYOUT_16_1);
	m_vector_RegulationLayOut.push_back(WIND_LAYOUT_25_1);
	m_vector_RegulationLayOut.push_back(WIND_LAYOUT_36_1);
	m_vector_RegulationLayOut.push_back(WIND_LAYOUT_49_1);
	m_vector_RegulationLayOut.push_back(WIND_LAYOUT_64_1);
}

void CLayoutMgr::InitLadderLayOut()
{
	m_vector_LadderLayOut.push_back(WIND_LAYOUT_6_3);
	m_vector_LadderLayOut.push_back(WIND_LAYOUT_12_2);
	m_vector_LadderLayOut.push_back(WIND_LAYOUT_16_2);
	m_vector_LadderLayOut.push_back(WIND_LAYOUT_20_1);
	m_vector_LadderLayOut.push_back(WIND_LAYOUT_26_1);
	m_vector_LadderLayOut.push_back(WIND_LAYOUT_8_1);
	m_vector_LadderLayOut.push_back(WIND_LAYOUT_10_2);
}

void CLayoutMgr::InitAnomalousLayoutLayOut()
{
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_3_1);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_3_2);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_3_3);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_4_2);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_4_3);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_6_1);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_6_2);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_6_4);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_9_1);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_10_1);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_13_1);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_13_2);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_17_1);
	m_vector_AnomalousLayoutLayOut.push_back(WIND_LAYOUT_12_1);
}

bool CLayoutMgr::IsAnomalousLayoutNum3( IVS_ULONG ulPaneNUm )const
{
	return ulPaneNUm / 10 == 3;
}

void CLayoutMgr::SetAnomalousLayoutNumEQ3( IVS_ULONG ulPaneNUm, CRect & rectPane, CVideoPaneMgr * pVideoPaneMgr)const
{
    CHECK_POINTER_VOID(pVideoPaneMgr);
	if (WIND_LAYOUT_3_1 == ulPaneNUm) {
		SetLayoutAs3_1(rectPane,pVideoPaneMgr);
	} else if(WIND_LAYOUT_3_2 == ulPaneNUm) {
		SetLayoutAs3_2(rectPane,pVideoPaneMgr);
	} else if(WIND_LAYOUT_3_3 == ulPaneNUm) {
		SetLayoutAs3_3(rectPane,pVideoPaneMgr);
	}
}
