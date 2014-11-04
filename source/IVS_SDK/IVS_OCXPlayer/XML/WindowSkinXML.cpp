#include "StdAfx.h"
#include "WindowSkinXML.h"
#include "bp_def.h"
#include "ivs_error.h"

CWindowSkinXML::CWindowSkinXML(void)
    :m_pRootElement(nullptr)
    ,m_pSkinRootElement(nullptr)
    ,m_pToolBarRootElement(nullptr)
    ,m_pTitleBarRootElement(nullptr)
    ,m_pSoundCtrlRootElement(nullptr)
    ,m_pVideoPaneRootElement(nullptr)
	,m_pTVwallStatusBarRootElement(nullptr)
	,m_pTranslucentRootElement(nullptr)
	,m_pDigtalPtzRootElement(nullptr)
{
}

CWindowSkinXML::~CWindowSkinXML(void)
{
    m_pRootElement = nullptr ;
    m_pSkinRootElement = nullptr ;
    m_pToolBarRootElement = nullptr ;
    m_pTitleBarRootElement = nullptr ;
    m_pSoundCtrlRootElement = nullptr ;
    m_pVideoPaneRootElement = nullptr;
} //lint !e1540 XML Element指针 由 m_XMLdoc 释放

IVS_ULONG CWindowSkinXML::SetSkin( IVS_INT32 iSkinID)
{
    m_pSkinRootElement = dynamic_cast<TiXmlElement *>(FindElementByAttribute(m_pRootElement,_T("ID"),iSkinID));
    CHECK_POINTER(m_pSkinRootElement,IVS_FAIL);
    m_pToolBarRootElement =m_pSkinRootElement->FirstChildElement(_T("ToolBar"));
    CHECK_POINTER(m_pToolBarRootElement,IVS_FAIL);
    m_pTitleBarRootElement =m_pSkinRootElement->FirstChildElement(_T("TitleBar"));
    CHECK_POINTER(m_pTitleBarRootElement,IVS_FAIL);
    m_pSoundCtrlRootElement =m_pSkinRootElement->FirstChildElement(_T("SoundCtrl"));
    CHECK_POINTER(m_pSoundCtrlRootElement,IVS_FAIL);
    m_pVideoPaneRootElement =m_pSkinRootElement->FirstChildElement(_T("VideoPane"));
    CHECK_POINTER(m_pVideoPaneRootElement,IVS_FAIL);
	m_pTVwallStatusBarRootElement = m_pSkinRootElement->FirstChildElement(_T("TVWallStatusBar"));
	CHECK_POINTER(m_pTVwallStatusBarRootElement,IVS_FAIL);
	m_pTranslucentRootElement = m_pSkinRootElement->FirstChildElement(_T("TranslucentBG"));
	CHECK_POINTER(m_pTranslucentRootElement,IVS_FAIL);
	m_pDigtalPtzRootElement = m_pSkinRootElement->FirstChildElement(_T("DigtalZoom"));
	CHECK_POINTER(m_pDigtalPtzRootElement,IVS_FAIL);
    return IVS_SUCCEED;
}

TiXmlNode * CWindowSkinXML::FindElementByAttribute(
    TiXmlNode * pParentXMLNode,
    const std::string &strAttrName,
    const std::string &strAttrValue) const
{
    CHECK_POINTER(pParentXMLNode,nullptr);

    if (strAttrName.empty() || strAttrValue.empty())
    {
        return nullptr;
    }

    TiXmlElement * pChildElement = nullptr;

    pChildElement = pParentXMLNode->FirstChildElement();
    for( ; pChildElement; pChildElement = pChildElement->NextSiblingElement () )
    {
        const TCHAR *pcValue = pChildElement->Attribute(strAttrName.c_str());
        if (nullptr == pcValue)
        {
            continue;
        }

        if (0 ==  strAttrValue.compare(0,strAttrValue.length(),pcValue,strAttrValue.length()))
        {
            break;
        }
    }

    return pChildElement;
}

TiXmlNode * CWindowSkinXML::FindElementByAttribute(
    TiXmlNode * pParentXMLNode,
    const std::string &strAttrName,
    IVS_INT32 iAttrValue) const
{
    CHECK_POINTER(pParentXMLNode,nullptr);

    if (strAttrName.empty())
    {
        return nullptr;
    }

    TiXmlElement * pChildElement = nullptr;
    IVS_INT32 iValue;

    pChildElement = pParentXMLNode->FirstChildElement();
    for( ; pChildElement; pChildElement = pChildElement->NextSiblingElement () )
    {
        iValue = 0;
        if (TIXML_SUCCESS != pChildElement->QueryIntAttribute(strAttrName.c_str(),&iValue) )
        {
            continue;
        }

        if (iAttrValue == iValue)
        {
            break;
        }
    }

    return pChildElement;
}

TiXmlNode * CWindowSkinXML::FindElementByAttribute(
    TiXmlNode * pParentXMLNode,
    const std::string &strFirstAttrName,
    const std::string &strFirstAttrValue,
    const std::string &strSecondAttrName,
    const std::string &strSecondAttrValue ) const
{
    CHECK_POINTER(pParentXMLNode,nullptr);

    if (strFirstAttrName.empty() || strFirstAttrValue.empty())
    {
        return nullptr;
    }

    if (strSecondAttrName.empty() || strSecondAttrValue.empty())
    {
        return nullptr;
    }

    TiXmlElement * pChildElement = nullptr;

    pChildElement = pParentXMLNode->FirstChildElement();
    for( ; pChildElement; pChildElement = pChildElement->NextSiblingElement () )
    {
        const TCHAR *pcFirstValue = pChildElement->Attribute(strFirstAttrName.c_str());
        if (nullptr == pcFirstValue)
        {
            continue;
        }

        if (0 !=  strFirstAttrValue.compare(0,strFirstAttrValue.length(),pcFirstValue,strFirstAttrValue.length()))
        {
            continue;
        }

        const TCHAR *pcSecondValue = pChildElement->Attribute(strSecondAttrName.c_str());
        if (nullptr == pcSecondValue)
        {
            continue;
        }

        if (0 ==  strSecondAttrValue.compare(0,strSecondAttrValue.length(),pcSecondValue,strSecondAttrValue.length()))
        {
            break;
        }
    }

    return pChildElement;
}

bool CWindowSkinXML::InitXML( const std::string &strCurrentPath )
{
    std::string strXMLFilePath = strCurrentPath;
    strXMLFilePath.append(_T("\\Skin\\Skins.xml"));
    if (strXMLFilePath.empty())
    {
        return false;
    }
    if(!m_XMLdoc.LoadFile(strXMLFilePath.c_str(), TIXML_ENCODING_LEGACY))return false;
    m_pRootElement = m_XMLdoc.RootElement();
    CHECK_POINTER(m_pRootElement,false);
    return true;
}

TiXmlNode * CWindowSkinXML::GetButtonElementByName(
    TiXmlNode * pParentXMLNode,
    const std::string &strAttrName,
    const std::string &strAttrValue ) const
{
    CHECK_POINTER(pParentXMLNode,nullptr);

    if (strAttrName.empty() || strAttrValue.empty())
    {
        return nullptr;
    }

    return FindElementByElementAttribute(pParentXMLNode,"Button",strAttrName,strAttrValue);
}

TiXmlNode * CWindowSkinXML::GetButtonElementByName(
    TiXmlNode * pParentXMLNode,
    const std::string &strFirstAttrName,
    const std::string &strFirstAttrValue,
    const std::string &strSecondAttrName,
    const std::string &strSecondAttrValue ) const
{
    CHECK_POINTER(pParentXMLNode,nullptr);

    if (strFirstAttrName.empty() || strFirstAttrValue.empty())
    {
        return nullptr;
    }

    if (strSecondAttrName.empty() || strSecondAttrValue.empty())
    {
        return nullptr;
    }

    return FindElementByElementAttribute(
        pParentXMLNode,
        "Button",
        strFirstAttrName,
        strFirstAttrValue,
        strSecondAttrName,
        strSecondAttrValue);
}

TiXmlNode * CWindowSkinXML::FindElementByElementAttribute(
    TiXmlNode * pParentXMLNode,
    const std::string &strElementName,
    const std::string &strAttrName,
    const std::string &strAttrValue ) const
{
    CHECK_POINTER(pParentXMLNode,nullptr);

    if (strAttrName.empty() || strAttrValue.empty() || strElementName.empty())
    {
        return nullptr;
    }

    TiXmlElement * pChildElement = nullptr;

    pChildElement = pParentXMLNode->FirstChildElement(strElementName.c_str());
    for( ; pChildElement; pChildElement = pChildElement->NextSiblingElement (strElementName.c_str()) )
    {
        const TCHAR *pcValue = pChildElement->Attribute(strAttrName.c_str());
        if (nullptr == pcValue)
        {
            continue;
        }

        if (0 ==  strAttrValue.compare(0,strAttrValue.length(),pcValue,strAttrValue.length()))
        {
            break;
        }
    }

    return pChildElement;
}

TiXmlNode * CWindowSkinXML::FindElementByElementAttribute(
    TiXmlNode * pParentXMLNode,
    const std::string &strElementName,
    const std::string &strFirstAttrName,
    const std::string &strFirstAttrValue,
    const std::string &strSecondAttrName,
    const std::string &strSecondAttrValue ) const
{
    CHECK_POINTER(pParentXMLNode,nullptr);

    if (strFirstAttrName.empty() || strFirstAttrValue.empty() || strElementName.empty())
    {
        return nullptr;
    }

    if (strSecondAttrName.empty() || strSecondAttrValue.empty())
    {
        return nullptr;
    }

    TiXmlElement * pChildElement = nullptr;

    pChildElement = pParentXMLNode->FirstChildElement(strElementName.c_str());
    for( ; pChildElement; pChildElement = pChildElement->NextSiblingElement (strElementName.c_str()) )
    {
        const TCHAR *pcFirstValue = pChildElement->Attribute(strFirstAttrName.c_str());
        if (nullptr == pcFirstValue)
        {
            continue;
        }

        if (0 !=  strFirstAttrValue.compare(0,strFirstAttrValue.length(),pcFirstValue,::strlen(pcFirstValue)))
        {
            continue;
        }

        const TCHAR *pcSecondValue = pChildElement->Attribute(strSecondAttrName.c_str());
        if (nullptr == pcSecondValue)
        {
            continue;
        }

        if (0 ==  strSecondAttrValue.compare(0,strSecondAttrValue.length(),pcSecondValue,::strlen(pcSecondValue)))
        {
            break;
        }
    }

    return pChildElement;
}

TiXmlNode * CWindowSkinXML::ToolBarGetButtonElementByName(
    const std::string &strButtonName,
    const std::string &strButtonType ) const
{
    CHECK_POINTER(m_pToolBarRootElement,nullptr);
    if (strButtonName.empty() || strButtonType.empty())
    {
        return nullptr;
    }

    return FindElementByElementAttribute(
        m_pToolBarRootElement,
        "Button",
        "Name",
        strButtonName,
        "Type",
        strButtonType);
}

IVS_ULONG CWindowSkinXML::GetPathofButtonPictureStatus(
    TiXmlNode * pParentXMLNode,
    const std::string &strButtonStatus ,
    std::string &strPngpath
    ) const
{
    CHECK_POINTER(pParentXMLNode,IVS_FAIL);
    if (strButtonStatus.empty())
    {
        return IVS_FAIL;
    }

    TiXmlNode* pParentNode = FindElementByAttribute(pParentXMLNode,_T("Status"),strButtonStatus);
	CHECK_POINTER(pParentNode,IVS_FAIL);
    TiXmlNode* pChildNode = pParentNode->FirstChild();
    CHECK_POINTER(pChildNode,IVS_FAIL);

    strPngpath.clear();
    const TCHAR * pc = pChildNode->Value();
    strPngpath.append(pc);
    if (strPngpath.empty())
    {
        return IVS_FAIL;
    }
    return IVS_SUCCEED;
}

TiXmlNode * CWindowSkinXML::TitleBarGetButtonElementByName(
    const std::string &strButtonName,
    const std::string &strButtonType ) const
{
    CHECK_POINTER(m_pToolBarRootElement,nullptr);
    if (strButtonName.empty() || strButtonType.empty())
    {
        return nullptr;
    }

    return FindElementByElementAttribute(
        m_pTitleBarRootElement,
        "Button",
        "Name",
        strButtonName,
        "Type",
        strButtonType);
}

TiXmlNode * CWindowSkinXML::DigtalPtzGetButtonElementByName(
	const std::string &strButtonName,
	const std::string &strButtonType ) const
{
	CHECK_POINTER(m_pDigtalPtzRootElement,nullptr);
	if (strButtonName.empty() || strButtonType.empty())
	{
		return nullptr;
	}

	return FindElementByElementAttribute(
		m_pDigtalPtzRootElement,
		"Button",
		"Name",
		strButtonName,
		"Type",
		strButtonType);
}

IVS_ULONG CWindowSkinXML::TitleBarGetBackGround(std::string &strBGPtth)
{
    strBGPtth.clear();

    TiXmlElement * pChildElement = nullptr;
    CHECK_POINTER(m_pTitleBarRootElement,IVS_FAIL);
    pChildElement =	m_pTitleBarRootElement->FirstChildElement("BackGround");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    pChildElement = pChildElement->FirstChildElement("Picture");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    TiXmlNode *pChildNode = nullptr;
    pChildNode = pChildElement->FirstChild();
    CHECK_POINTER(pChildNode,IVS_FAIL);

    const TCHAR * pc = pChildNode->Value();
    (void)strBGPtth.append(pc);

    return IVS_SUCCEED;
}

IVS_ULONG CWindowSkinXML::ToolBarGetBackGround(std::string &strBGPtth)
{
    strBGPtth.clear();

    TiXmlElement * pChildElement = nullptr;
    CHECK_POINTER(m_pToolBarRootElement,IVS_FAIL);

    pChildElement =	m_pToolBarRootElement->FirstChildElement("BackGround");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    pChildElement = pChildElement->FirstChildElement("Picture");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    TiXmlNode *pChildNode = nullptr;
    pChildNode = pChildElement->FirstChild();
    CHECK_POINTER(pChildNode,IVS_FAIL);

    const TCHAR * pc = pChildNode->Value();
    (void)strBGPtth.append(pc);

    return IVS_SUCCEED;
}
//获取状态Bar的离线和在线的背景图
IVS_ULONG CWindowSkinXML::StatusBarGetBackGround(std::string &strOnlinePath,std::string &strOfflinePath)
{
	strOnlinePath.clear();

	TiXmlElement * pChildElement = nullptr;
	CHECK_POINTER(m_pTVwallStatusBarRootElement,IVS_FAIL);

	//获取Online的背景图片
	pChildElement =	m_pTVwallStatusBarRootElement->FirstChildElement("Online");
	CHECK_POINTER(pChildElement,IVS_FAIL);

	pChildElement = pChildElement->FirstChildElement("Picture");
	CHECK_POINTER(pChildElement,IVS_FAIL);

	TiXmlNode *pChildNode = nullptr;
	pChildNode = pChildElement->FirstChild();
	CHECK_POINTER(pChildNode,IVS_FAIL);

	const TCHAR * pc = pChildNode->Value();
	(void)strOnlinePath.append(pc);

	//获取Offline的背景图片
	strOfflinePath.clear();
	pChildElement =	m_pTVwallStatusBarRootElement->FirstChildElement("Offline");
	CHECK_POINTER(pChildElement,IVS_FAIL);

	pChildElement = pChildElement->FirstChildElement("Picture");
	CHECK_POINTER(pChildElement,IVS_FAIL);

	//TiXmlNode *pChildNode = nullptr;
	pChildNode = pChildElement->FirstChild();
	CHECK_POINTER(pChildNode,IVS_FAIL);

	pc = pChildNode->Value();
	(void)strOfflinePath.append(pc);

	return IVS_SUCCEED;
}
IVS_ULONG CWindowSkinXML::ToolBarPopMenuHead( std::string &strMenuHeadPath )
{
    strMenuHeadPath.clear();

    TiXmlElement * pChildElement = nullptr;
    CHECK_POINTER(m_pToolBarRootElement,IVS_FAIL);

    pChildElement =	m_pToolBarRootElement->FirstChildElement("Menu");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    pChildElement =	pChildElement->FirstChildElement("MenuHead");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    pChildElement = pChildElement->FirstChildElement("Picture");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    TiXmlNode *pChildNode = nullptr;
    pChildNode = pChildElement->FirstChild();
    CHECK_POINTER(pChildNode,IVS_FAIL);

    const TCHAR * pc = pChildNode->Value();
    (void)strMenuHeadPath.append(pc);

    if (strMenuHeadPath.empty())
    {
        return IVS_FAIL;
    }

    return IVS_SUCCEED;
}

IVS_ULONG CWindowSkinXML::ToolBarPopMenuTail( std::string &strMenuTailPath )
{
    strMenuTailPath.clear();

    TiXmlElement * pChildElement = nullptr;
    CHECK_POINTER(m_pToolBarRootElement,IVS_FAIL);

    pChildElement =	m_pToolBarRootElement->FirstChildElement("Menu");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    pChildElement =	pChildElement->FirstChildElement("MenuTail");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    pChildElement = pChildElement->FirstChildElement("Picture");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    TiXmlNode *pChildNode = nullptr;
    pChildNode = pChildElement->FirstChild();
    CHECK_POINTER(pChildNode,IVS_FAIL);

    const TCHAR * pc = pChildNode->Value();
    (void)strMenuTailPath.append(pc);

    if (strMenuTailPath.empty())
    {
        return IVS_FAIL;
    }

    return IVS_SUCCEED;
}

IVS_ULONG CWindowSkinXML::ToolBarPopMenuMidl( std::string &strMenuMidlPath )
{
    strMenuMidlPath.clear();

    TiXmlElement * pChildElement = nullptr;

    pChildElement =	m_pToolBarRootElement->FirstChildElement("Menu");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    pChildElement =	pChildElement->FirstChildElement("MenuMidl");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    pChildElement = pChildElement->FirstChildElement("Picture");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    TiXmlNode *pChildNode = nullptr;
    pChildNode = pChildElement->FirstChild();
    CHECK_POINTER(pChildNode,IVS_FAIL);

    const TCHAR * pc = pChildNode->Value();
    strMenuMidlPath.append(pc);

    if (strMenuMidlPath.empty())
    {
        return IVS_FAIL;
    }

    return IVS_SUCCEED;
}

IVS_ULONG CWindowSkinXML::ToolBarPopMenuHover( std::string &strMenuHoverPath )
{
    strMenuHoverPath.clear();

    TiXmlElement * pChildElement = nullptr;

    pChildElement =	m_pToolBarRootElement->FirstChildElement("Menu");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    pChildElement =	pChildElement->FirstChildElement("MenuHover");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    pChildElement = pChildElement->FirstChildElement("Picture");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    TiXmlNode *pChildNode = nullptr;
    pChildNode = pChildElement->FirstChild();
    CHECK_POINTER(pChildNode,IVS_FAIL);

    const TCHAR * pc = pChildNode->Value();
    strMenuHoverPath.append(pc);

    if (strMenuHoverPath.empty())
    {
        return IVS_FAIL;
    }

    return IVS_SUCCEED;
}

IVS_ULONG CWindowSkinXML::VideoPaneVideoBackGround( std::string &strBackGroundPath )
{
    strBackGroundPath.clear();

    TiXmlElement * pChildElement = nullptr;

    pChildElement =	m_pVideoPaneRootElement->FirstChildElement("VideoPaneBackGround");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    pChildElement = pChildElement->FirstChildElement("Picture");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    TiXmlNode *pChildNode = nullptr;
    pChildNode = pChildElement->FirstChild();
    CHECK_POINTER(pChildNode,IVS_FAIL);

    const TCHAR * pc = pChildNode->Value();
    strBackGroundPath.append(pc);

    if (strBackGroundPath.empty())
    {
        return IVS_FAIL;
    }

    return IVS_SUCCEED;
}

IVS_ULONG CWindowSkinXML::VideoPaneAlarmBackGround( std::string &strBackGroundPath )
{
    strBackGroundPath.clear();

    TiXmlElement * pChildElement = nullptr;

    pChildElement =	m_pVideoPaneRootElement->FirstChildElement("AlarmBackGround");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    pChildElement = pChildElement->FirstChildElement("Picture");
    CHECK_POINTER(pChildElement,IVS_FAIL);

    TiXmlNode *pChildNode = nullptr;
    pChildNode = pChildElement->FirstChild();
    CHECK_POINTER(pChildNode,IVS_FAIL);

    const TCHAR * pc = pChildNode->Value();
    strBackGroundPath.append(pc);

    if (strBackGroundPath.empty())
    {
        return IVS_FAIL;
    }

    return IVS_SUCCEED;
}

IVS_ULONG CWindowSkinXML::TranslucentDialogBackGround(std::string &strBGPtth)
{
	strBGPtth.clear();
	TiXmlElement * pChildElement = nullptr;
	CHECK_POINTER(m_pTranslucentRootElement,IVS_FAIL);

	//获取Online的背景图片
	pChildElement =	m_pTranslucentRootElement->FirstChildElement("BkGround");
	CHECK_POINTER(pChildElement,IVS_FAIL);

	pChildElement = pChildElement->FirstChildElement("Picture");
	CHECK_POINTER(pChildElement,IVS_FAIL);

	TiXmlNode *pChildNode = nullptr;
	pChildNode = pChildElement->FirstChild();
	CHECK_POINTER(pChildNode,IVS_FAIL);

	const TCHAR * pc = pChildNode->Value();
	(void)strBGPtth.append(pc);

	return IVS_SUCCEED;
}

IVS_ULONG CWindowSkinXML::InstantReplayBackGround(std::string &strBackGroundPath )
{
	strBackGroundPath.clear();

	TiXmlElement * pChildElement = nullptr;

	pChildElement =	m_pVideoPaneRootElement->FirstChildElement("InstantReplayBackGround");
	CHECK_POINTER(pChildElement,IVS_FAIL);

	pChildElement = pChildElement->FirstChildElement("Picture");
	CHECK_POINTER(pChildElement,IVS_FAIL);

	TiXmlNode *pChildNode = nullptr;
	pChildNode = pChildElement->FirstChild();
	CHECK_POINTER(pChildNode,IVS_FAIL);

	const TCHAR * pc = pChildNode->Value();
	strBackGroundPath.append(pc);

	if (strBackGroundPath.empty())
	{
		return IVS_FAIL;
	}

	return IVS_SUCCEED;
}
