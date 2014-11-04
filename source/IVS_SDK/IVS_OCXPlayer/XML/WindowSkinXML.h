/*****************************************************************
 filename    :    WindowSkinXML.h
 author      :    ykf75928
 created     :    2013/01/15
 description :    皮肤XML处理类
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/15 初始版本
*****************************************************************/
#pragma once

#include "SDKDef.h"
#include "tinyxml.h"
#include "hwsdk.h"

class CWindowSkinXML
{
private:
	TiXmlDocument m_XMLdoc;
	TiXmlElement *m_pRootElement;
	TiXmlElement *m_pSkinRootElement;
	TiXmlElement *m_pToolBarRootElement;
	TiXmlElement *m_pTitleBarRootElement;
	TiXmlElement *m_pSoundCtrlRootElement;
	TiXmlElement *m_pVideoPaneRootElement;
	TiXmlElement *m_pTVwallStatusBarRootElement;//电视墙的通道状态BAR根节点
	TiXmlElement *m_pTranslucentRootElement;//透明背景的哏结点
	TiXmlElement *m_pDigtalPtzRootElement;//数字云镜的哏结点
public:
//private:
	TiXmlNode * FindElementByAttribute( 
		TiXmlNode * pParentXMLNode, 
		const std::string &strAttrName,
		const std::string &strAttrValue ) const;

	TiXmlNode * FindElementByAttribute(
		TiXmlNode * pParentXMLNode,
		const std::string &strFirstAttrName,
		const std::string &strFirstAttrValue,
		const std::string &strSecondAttrName,
		const std::string &strSecondAttrValue ) const;

	TiXmlNode * FindElementByElementAttribute( 
		TiXmlNode * pParentXMLNode, 
		const std::string &strElementName,
		const std::string &strAttrName,
		const std::string &strAttrValue ) const;

	TiXmlNode * FindElementByElementAttribute(
		TiXmlNode * pParentXMLNode,
		const std::string &strElementName,
		const std::string &strFirstAttrName,
		const std::string &strFirstAttrValue,
		const std::string &strSecondAttrName,
		const std::string &strSecondAttrValue ) const;

	TiXmlNode * FindElementByAttribute( 
		TiXmlNode * pParentXMLNode, 
		const std::string &strAttrName,
		IVS_INT32 iAttrValue ) const;
public:
	CWindowSkinXML(void);
	~CWindowSkinXML(void);

    bool InitXML(const std::string &strCurrentPath);

	IVS_ULONG SetSkin(IVS_INT32 iSkinID);

	TiXmlNode * GetButtonElementByName(
		TiXmlNode * pParentXMLNode,
		const std::string &strAttrName,
		const std::string &strAttrValue) const;

	TiXmlNode * GetButtonElementByName(
		TiXmlNode * pParentXMLNode,
		const std::string &strFirstAttrName,
		const std::string &strFirstAttrValue,
		const std::string &strSecondAttrName,
		const std::string &strSecondAttrValue ) const;

	TiXmlNode * ToolBarGetButtonElementByName(
		const std::string &strButtonName,
		const std::string &strButtonType) const;

	TiXmlNode * TitleBarGetButtonElementByName( 
		const std::string &strButtonName, 
		const std::string &strButtonType ) const;

	TiXmlNode * DigtalPtzGetButtonElementByName( 
		const std::string &strButtonName, 
		const std::string &strButtonType ) const;

	IVS_ULONG TitleBarGetBackGround(std::string &strBGPtth);
	IVS_ULONG ToolBarGetBackGround(std::string &strBGPtth);
	
	/******************************************************************
	function : StatusBarGetBackGround
	description: 获取状态Bar的离线和在线的背景图
	input : std::string & strOnlinePath
	input : std::string & strOfflinePath
	output : NA
	return : IVS_ULONG
	*******************************************************************/
	IVS_ULONG StatusBarGetBackGround(std::string &strOnlinePath,std::string &strOfflinePath);

	IVS_ULONG TranslucentDialogBackGround(std::string &strBGPtth);

	IVS_ULONG GetPathofButtonPictureStatus( 
		TiXmlNode * pParentXMLNode, 
		const std::string &strButtonStatus , 
		std::string &strPngpath ) const;

	IVS_ULONG ToolBarPopMenuHead(std::string &strMenuHeadPath);
	IVS_ULONG ToolBarPopMenuTail(std::string &strMenuTailPath);
	IVS_ULONG ToolBarPopMenuMidl(std::string &strMenuMidlPath);
	IVS_ULONG ToolBarPopMenuHover(std::string &strMenuHoverPath);

	IVS_ULONG VideoPaneVideoBackGround(std::string &strBackGroundPath);
	IVS_ULONG VideoPaneAlarmBackGround(std::string &strBackGroundPath);
	IVS_ULONG InstantReplayBackGround(std::string &strBackGroundPath);
};

