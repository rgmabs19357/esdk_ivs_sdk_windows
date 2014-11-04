/*****************************************************************
 filename    :    PostEventXML.h
 author      :    yKF75928
 created     :    2012/11/19
 description :    获取窗口消息报文的类;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*****************************************************************/

#ifndef _POSTEVENTXML_H_D1CDDE33_F3A9_4EF5_8875_555FF7E39AD7_
#define _POSTEVENTXML_H_D1CDDE33_F3A9_4EF5_8875_555FF7E39AD7_

#pragma once

#include <afx.h>
#include <string>
#include <map>
#include "hwsdk.h"
#include "SDKDef.h"

#define PostEventXMLImpl PostEventXML::Instance()

class PostEventXML
{
private:
	std::map<ULONGLONG,std::string> m_mapRealtimeToolBarBtnName;
	std::map<ULONGLONG,std::string> m_mapAnalysisToolBarBtnName;
	PostEventXML(void);                 //lint !e1704
public:
	static PostEventXML& Instance();
public:
    ~PostEventXML(void);

    IVS_VOID PostTalkbackEvent(CString &xmlToCU, ULONG ulResult);
	
    /*****************************************************************
         function   : GetActiveWndChangeEventMsgXML
         description: 获得当前激活窗体更改消息报文
         output     : CString & xmlToCU		报文
         input      : ULONG ulWinID			窗口ID
		 return     : NA
    *****************************************************************/
    IVS_VOID GetActiveWndChangeEventMsgXML(CString &xmlToCU, ULONG ulWinID);

	
    /*****************************************************************
         function   : GetExchangeWndEventMsgXML
         description: 获得交换窗体事件消息报文
         output     : CString & xmlToCU   报文
         intput     : ULONG ulSrcWnd      源窗口句柄
                      ULONG ulDstWnd      目标窗口句柄
         return     : NA
    *****************************************************************/
    IVS_VOID GetExchangeWndEventMsgXML(CString &xmlToCU, ULONG ulSrcWnd, ULONG ulDstWnd);

    /*****************************************************************
         function   : GetMouseClickEventXML
         description: 获取鼠标点击事件的消息报文
         output     : xmlToCU		报文
         intput     : ULONG ulWinID 窗口ID
                      ULONG ulX     鼠标所在的X坐标
                      ULONG ulY     鼠标所在的Y坐标
         return     : NA
    *****************************************************************/
    IVS_VOID GetMouseClickEventXML(CString &xmlToCU, ULONG ulWinID, ULONG ulX, ULONG ulY);

    /*****************************************************************
         function   : GetStopButtonClickMsgXML
         description: 获取停止点击的消息报文
         output     : CString & xmlToCU  报文
         output     : ULONG ulWinID      窗口ID
         return     :  NA
    *****************************************************************/
    IVS_VOID GetStopButtonClickMsgXML(CString &xmlToCU, ULONG ulWinID);

	/*****************************************************************
	     function   : GetPTZConfig
	     description: 获取云镜参数报文
	     output     : const std::string & strXML   报文
	     intput     : long * lResultCode           错误码
	                  unsigned int * iSpeed: 1-步长  2-步速
	     return     : 成功返回0, 失败返回1
	*****************************************************************/
	IVS_LONG GetPTZConfig(const std::string &strXML,IVS_LONG *lResultCode , IVS_UINT32 *iSpeed);

    /*****************************************************************
         function   : GetOcxEventMsgStrXML
         description: 获取事件消息保温
         output     : std::string & xmlToCU
         output     : PTCHAR pMsg
         output     : LPCTSTR pMsgSeq
         output     : ULONG ulResultCode
         output     : ULONG ulWndID
         return     : IVS_VOID
    *****************************************************************/
    IVS_VOID GetOcxEventMsgStrXML(std::string &xmlToCU, LPCTSTR pMsg, LPCTSTR pMsgSeq, ULONG ulResultCode, ULONG ulWndID);

	IVS_VOID GetBtnName(std::string &strBtnName ,ULONG ulBarType,ULONGLONG ulBtnTtype);

	IVS_VOID GetToolBarClickEventMsg(std::string &xml,const std::string &strEvent,ULONG ulWndID,const std::string &strStatus);

	IVS_VOID GetLadderLayoutEventMsg(std::string &xml,ULONG ulWndNum);

	IVS_VOID GetToolBarEMapClickEventMsg(std::string &xml,const std::string &strEvent,ULONG ulWndID,CRect& rect);

	IVS_VOID GetVolumeEventMsg(CString &xmlToCU, ULONG ulWndID, ULONG ulEventType, ULONG ulVolumeValue) const;

	IVS_VOID GetInstantReturnEventMsg(std::string &xml,ULONG ulWndID);

	IVS_VOID GetFullSCreenXML(std::string &xmlToCU, ULONG ulResultCode, ULONG ulWndID);
};
#endif // _POSTEVENTXML_H_D1CDDE33_F3A9_4EF5_8875_555FF7E39AD7_

