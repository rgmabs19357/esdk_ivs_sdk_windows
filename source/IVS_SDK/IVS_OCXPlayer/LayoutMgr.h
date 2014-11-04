/*****************************************************************
 filename    :    LayoutMgr.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    布局管理器
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#pragma once

#include "VideoPaneMgr.h"

#define LayoutMgr CLayoutMgr::GetInstance()

class CLayoutMgr
{
private:	//布局容器
	CLayoutMgr(void);    //lint !e1704
	
	std::vector<IVS_ULONG> m_vector_Layout;
	std::vector<IVS_ULONG> m_vector_RegulationLayOut;
	std::vector<IVS_ULONG> m_vector_AnomalousLayoutLayOut;
	std::vector<IVS_ULONG> m_vector_LadderLayOut;
public:
	static CLayoutMgr& GetInstance();
	~CLayoutMgr(void);

	bool isLadderLayout_12( const IVS_ULONG _ulPaneNUm) const;
	bool isLadderLayout_10( const IVS_ULONG _ulPaneNUm) const;
	bool isLadderLayout_8( const IVS_ULONG _ulPaneNUm) const;
	bool isLadderLayout_6( const IVS_ULONG _ulPaneNUm) const;
	bool isLadderLayout_16( const IVS_ULONG _ulPaneNUm) const;
	bool isLadderLayout_20( const IVS_ULONG _ulPaneNUm) const;
	bool isLadderLayout_26( const IVS_ULONG _ulPaneNUm) const;

	bool isLadderLayout( IVS_ULONG _ulPaneNUm) ;
	bool isAnomalousLayOut( IVS_ULONG _ulPaneNUm ) ;
	bool isRegulationLayOut( IVS_ULONG _ulPaneNUm) ;
	bool isTVWallLayout( IVS_ULONG _ulPaneNUm)const;
	//初始化布局
	void InitLayOut();

	void InitAnomalousLayoutLayOut();

	void InitLadderLayOut();

	void InitRegulationLay();

	void InitAllLayout();

	// 检查摄像机类型合法性
	IVS_ULONG CheckCameraType(IVS_ULONG ulCameraType)const;
	// 检查客户端类型合法性
	IVS_ULONG CheckClientType(IVS_ULONG ulClientType)const;
	// 检查窗口类型合法性
	IVS_ULONG CheckWndType(IVS_ULONG ulWndType)const;
	// 检查布局类型是否合法
	bool CheckLayoutType(IVS_ULONG ulLayoutType);

	void SetLayoutAs6_1( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const;
	void SetLayoutAs9_1( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const;
    void SetLayoutAs10( CRect & rectPane ,CVideoPaneMgr * pVideoPaneMgr)const;
	void SetLayoutAs12_1( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const;
	void SetLayoutAs13( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const;
	void SetLayoutAs13_2( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const;
	void SetLayoutAs17_1( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const;
	void SetLayoutAs3_3( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const;
	void SetLayoutAs6_4( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const;
	void SetLayoutAs6_2( CRect &rectPane ,CVideoPaneMgr *pVideoPaneMgr)const;
	void SetLayoutAs3_2( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const;
	void SetLayoutAs3_1( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const;
	void SetLayoutAs4_3( CRect &rectPane ,CVideoPaneMgr *pVideoPaneMgr)const;
	void SetLayoutAs4_2( CRect &rectPane,CVideoPaneMgr *pVideoPaneMgr )const;

	IVS_ULONG SetLadderLayout( CRect &rectPane,IVS_ULONG ulPaneNUm,CVideoPaneMgr *pVideoPaneMgr )const;
	IVS_ULONG SetAnomalousLayout(CRect &rectPane, IVS_ULONG ulPaneNUm,CVideoPaneMgr *pVideoPaneMgr )const;

	void SetAnomalousLayoutNumEQ3( IVS_ULONG ulPaneNUm, CRect & rectPane, CVideoPaneMgr * pVideoPaneMgr)const;

	bool IsAnomalousLayoutNum3( IVS_ULONG ulPaneNUm )const;

	void SetLayoutVerticalX( const CRect &rectPane,IVS_INT32 iNum ,CVideoPaneMgr *pVideoPaneMgr)const;
	void SetLayoutHorizontalX( const CRect &rectPane,IVS_INT32 iNum ,CVideoPaneMgr *pVideoPaneMgr)const;
};

