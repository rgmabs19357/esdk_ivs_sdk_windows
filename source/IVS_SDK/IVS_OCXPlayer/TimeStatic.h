#ifndef _TIME_STATIC_H__
#define _TIME_STATIC_H__

// CTimeStatic
//标尺控件基类
//#include "CTimeStaticMacro.h"
#include "math.h"

CONST INT32 CURRENT_HOUR   =    0  ;               //默认当前小时
CONST INT32 CURRENT_MIN    =    0  ;               //默认当前分钟
CONST INT32 START_HOUR     =    0   ;               //默认起始小时
CONST INT32 START_MIN      =    0   ;               //默认起始分钟
CONST INT32 END_HOUR       =    24  ;               //默认结束小时
CONST INT32 END_MIN        =    0   ;               //默认结束分钟
CONST INT32 TEXT_SPACE     =    2   ;               //默认小时文本间隔
CONST INT32 TEXT_MIN       =    10  ;               //默认分钟线间隔

CONST INT32 LENGTH_HOUR    =    4;                  //默认小时线长度系数
CONST INT32 LENGTH_MIN     =    9;                  //默认分钟线长度系数
CONST INT32 LENGTH_MIDMIN  =    6;                  //默认分钟中间线长度系数

CONST INT32 MINUTE_COUNT   =    60;                 //每小时60分钟
CONST INT32 HOUR_COUNT     =    24;                 //每天24小时
CONST INT32 SCOND_COUNT    =    3600;                //每小时3600秒
CONST INT32 MINUTE_GAP     =    5;                  //分钟线的空隙
CONST INT32 SCOND_GAP      =    15;                 //秒线的空隙
CONST INT32 TEXT_GAP       =    10;                 //文字之间的空隙
CONST INT32 NONIUS_WIDTH   =    10;                 //游标宽
CONST INT32 NONIUS_HRIGHT  =    10;                 //游标高
CONST INT32 NONIUS_LEFT_OFFSET     =    6;          //左偏移
CONST INT32 NONIUS_BOTTOM_OFFSET   =    14;         //下偏移
CONST INT32 MAX_SCOND_NUM          =    12+1;       //小刻度时十五秒线数目
CONST INT32 WHEEL_SCAL             =    120;        //鼠标滚轮每次滚动移动值

typedef struct
{
	int iHour;
	int iMinute;
	int iScond;
}ST_TIME;

typedef struct
{
	ST_TIME stStartTime;
	ST_TIME stEndTime;
}ST_LAYERTIME;

typedef struct
{
	double dLeft;
	double dRight;
	int iType; //0 表示是黄色的 1表示是绿色的
	ST_LAYERTIME stTime;
}ST_AREAINFO;

typedef enum
{
	TYPE_YELLO = 0, // 黄色层
	TYPE_GREEN  // 绿色层
}EM_LAYERTIME;

//发送消息的结构体
typedef struct
{
	ST_TIME stStartTime;   //当前区域的开始时间
	ST_TIME stEndTime;     //当前区域的结束时间
	ST_TIME stCurrentTime; //当前点中的时间
	int iType;             //当前区域的类型
}ST_TIMER_MSG;

#define  RECORD_PLAY_TIME_ID 5
//刻度类型
typedef enum
{
	TYPE_SCALE_SMALL = 0, //小刻度
	TYPE_SCALE_BIG        //大刻度
}EM_SCALE_TYPE;

class CPlayTimerBar;
class CTimeStatic : public CTranslucentStatic
{
	DECLARE_DYNAMIC(CTimeStatic)

public:
	CTimeStatic();
	virtual ~CTimeStatic();
	virtual void Render(Gdiplus::Graphics& g);
	void Clear();
	void ClearYelloList();
	void ClearGreenList();
	void ClearAreaList();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	ST_TIME PointToTime(const CPoint &point) const;
	void MiniteToTime(ST_TIME &oldtime,const int iMinute) const;
	double TimeToPointX(const ST_TIME &stStarttime);
	bool TimeToRight(ST_TIME &oldtime,const ST_TIME &righttime)const;
	bool TimeToleft(ST_TIME &oldtime,const ST_TIME &lefttime)const;
	void TimeTansZero(ST_TIME &sttime)const;
	void FindRecord();

	void ScondToTime(ST_TIME &oldtime,const int iScond)const;
public:
	bool SetBarTime(const ST_TIME &stStarttime,const ST_TIME &stEndtime);//lint !e830
	bool SetCurrentTime(const ST_TIME &time);//lint !e830
	void TransTime(ST_TIME &newtime,const ST_TIME &oldtime)const;//换算时间
	ST_TIME GetCurrentTime() const;                      //获取当前时间
	void AddLayeTimeRange(int iType,const ST_LAYERTIME &stLayerTime);   //设置时间端
	void SetPlayTimerBar(CPlayTimerBar* pPlayTimerBar);

	void InitTimeRange();

	void MovePaneBig(const CPoint& point);
	void MovePaneSmall(const CPoint& point);
	BOOL IsCanPlayBack(ST_TIMER_MSG &stTimerMsg);
	void StartRecordPlayTimer();
	void StopRecordPlayTime();
	//播放结束
	BOOL IsPlayEnd();
    BOOL IsCurrentPlayEnd();
    void ResetTimer()const;
	void SetCurrentPaneToActivePane();
public:
	CWnd* GetVideoPane() const { return m_pVideoPane; } //lint !e1763  MFC自动生成的的函数
	void SetVideoPane(CWnd * pVideoPane){m_pVideoPane = pVideoPane;}
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
public:
	void Initialize();                      //初始化;
	void Initialize2();                     //小刻度初始化
protected:
	CWnd* m_pVideoPane;
private:
	bool   m_bTrack;
	ST_TIME m_stCurrentTime;          // 当前时间;
	ST_TIME m_stStartTime;            // 起始时间;
	ST_TIME m_stEndTime;              // 终止时间;
	ST_TIME m_stSaveCurrentTime;
    ST_TIME m_stStartPlayTime;        // 开始播放的时间;
	CRect m_cRect;                    // 控件矩形;

	PINT m_pHourLineX;               // 小时点横坐标指针;
	PINT m_pMinLineX;                // 分钟点横坐标指针;
	PINT m_pScondLinX;               // 十五秒线横坐标指针;
	int m_iHourNum;                   // 小时线数目;
	int m_iMinNum;                    // 分钟线数目;
	int m_iHourLength;                // 小时线长;
	int m_iMinLength;                 // 分钟线长;
	int m_iMidMinLength;              // 分钟中间线长;
	double m_dHourSpace;              // 小时间隔;
	double m_dMinSpace;               // 分钟间隔;
	double m_dSconSpace;              // 十五秒线之间的间隔

	bool m_bIsMouseFlag;              // 鼠标标志;
	bool m_bIsInit;                   // 是否初始化;
	double m_iCurrentPointPos;           // 保存当前游标坐标
	double m_iLstPointPos;            // 保存上次的游标位置
	int m_iLastXPos;                  // 保存上次移动的X坐标
	CPlayTimerBar* m_pPlayTimerBar;   // 保存父窗口的指针
	int m_iSizeType;                  // 刻度类型
	bool m_bIsTimer;                  // 定时器启动

	double m_dScondGap;               // 每秒在当前界面上的长度

    //ST_AREAINFO m_AreaInfo;           // 当前播放区域
    ST_LAYERTIME m_Layer;             // 当前播放区域

	CList<ST_LAYERTIME*,ST_LAYERTIME*> m_ListYello;
	CList<ST_LAYERTIME*,ST_LAYERTIME*> m_ListGreen;
	CList<ST_AREAINFO*,ST_AREAINFO*> m_ListArea;
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	HANDLE m_hThread;
    static DWORD WINAPI TimerThread(LPVOID pParam);
	void AddOneScond();
    void SetStartPlayTime(const ST_TIME &stPlaytime);
    bool IsInCurrentArea(const CPoint &point);
    bool IsInCurrentArea(const ST_TIME &time);
    bool FindNextPlayRange(const ST_TIME &CurTime, ST_TIME &NextTime);
    void GetTimeMsg(const ST_TIME& stTime, ST_TIMER_MSG &stTimerMsg);
	void SetCurStreamStop() { m_bCurStreamStop = true; };

private:

	bool m_bCurStreamStop;





};

#endif

