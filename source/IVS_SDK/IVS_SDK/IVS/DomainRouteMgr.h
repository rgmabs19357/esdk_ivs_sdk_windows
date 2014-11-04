/********************************************************************
 filename    :    DomainRouteMgr.h
 author      :    chenxianxiao 00206592
 created     :    2013/01/17
 description :    域路由管理
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/17 初始版本
*********************************************************************/
#ifndef __DOMAIN_ROUTE_MGR__
#define __DOMAIN_ROUTE_MGR__

#include "ivs_error.h"
#include "hwsdk.h"
#include "vos.h"
// #include <vector>
// #include <list>
// #include <map>
#include "ivs_xml.h"
#include "SDKDef.h"

//域路由map
typedef std::map<std::string, IVS_DOMAIN_ROUTE*> DOMAIN_CODE_LIST_MAP;
typedef DOMAIN_CODE_LIST_MAP::iterator DOMAIN_CODE_LIST_MAP_ITER;

typedef std::list<IVS_DOMAIN_ROUTE*> DOMAIN_CODE_LIST;
typedef DOMAIN_CODE_LIST::iterator DOMAIN_CODE_LIST_ITER;

//代理域Map
typedef std::map<std::string, IVS_DOMAIN_ROUTE*>PROXY_DOMAIN_CODEL_MAP;
typedef PROXY_DOMAIN_CODEL_MAP::iterator PROXY_DOMAIN_CODEL_MAP_ITER;

typedef std::list<std::string> SUPER_DOMAIN_CODE_LIST;
typedef SUPER_DOMAIN_CODE_LIST::iterator SUPER_DOMAIN_CODE_LIST_ITER;

typedef std::vector<std::string> DOMAIN_CODE_VEC;

class CUserMgr;  //lint !e763 !e830
class CDomainRouteMgr
{
public:
	CDomainRouteMgr(void);
	~CDomainRouteMgr(void);

	/******************************************************************
	 function   : SetUserMgr
	 description: 设置用户指针
	 input      : CUserMgr * pUserMgr
	 output     : NA
	 return     : void
	*******************************************************************/
	void SetUserMgr(CUserMgr *pUserMgr);
public:
	/******************************************************************
    function : GetDomainRoute
    description: 查询域路由
    input : IVS_UINT32 uiBufferSize
    output : IVS_DOMAIN_ROUTE_LIST * pDomainRouteList
    return : IVS_INT32
    *******************************************************************/
    IVS_INT32 GetDomainRoute(IVS_DOMAIN_ROUTE_LIST* pDomainRouteList, IVS_UINT32 uiBufferSize );

	/******************************************************************
    function : ModifyDomainRoute
    description: 修改域路由表
    input : IVS_DOMAIN_ROUTE pDomainRoute
    output : NA
    return : IVS_INT32
    *******************************************************************/
	IVS_INT32 ModifyDomainRoute(IVS_DOMAIN_ROUTE* pDomainRoute);

	/******************************************************************
    function : GetDomainRoute
    description: 刷新Map信息，重载GetDomainRoute方法，无需返回值
    input : NA
    output : NA
    return : IVS_INT32
    *******************************************************************/
    IVS_INT32 GetDomainRoute();
	
	/******************************************************************
    function : GetExDomainList
    description: 查询外部域列表
	input  : pQueryInfo 通用查询
    output : pRspXml 查询结果
    return : IVS_INT32
    *******************************************************************/
	IVS_INT32 GetExDomainList(const IVS_CHAR* pQueryInfo, IVS_CHAR** pRspXml);

	/******************************************************************
    function : SetShareCatalog
    description:  设置共享目录
    input : pShareInfo 消息XML
    output : NA
    return : IVS_INT32
    *******************************************************************/
	IVS_INT32 SetShareCatalog(const IVS_CHAR* pShareInfo);

	/******************************************************************
    function : GetShareCatalogList
    description: 查询目录共享信息列表
    input : pQueryInfo 查询消息XML
    output : pRspXml 查询结果
    return : IVS_INT32
    *******************************************************************/
	IVS_INT32 GetShareCatalogList(const IVS_CHAR* pQueryInfo,IVS_CHAR** pRspXml);


    /******************************************************************
    function : GetLocalDomainCode
    description: 获取登录SMU的域CODE
    input : NA
    output : strLocalDomainCode 登录的SMU的域CODE
    return : IVS_INT32
    *******************************************************************/
    IVS_INT32 GetLocalDomainCode(std::string& strLocalDomainCode);

    /******************************************************************
    function : GetDomainRouteStruct
    description: 根据map填充结构体
    input : IVS_UINT32 uiBufferSize
    output : IVS_DOMAIN_ROUTE_LIST * pDomainRouteList
    return : IVS_INT32
    *******************************************************************/
    IVS_INT32 GetDomainRouteStruct(IVS_DOMAIN_ROUTE_LIST* pDomainRouteList, IVS_UINT32 uiBufferSize );

    /******************************************************************
     function   : ClearDomainRouteMap
     description: 删除域路由
     output     : NA
     return     : IVS_VOID
    *******************************************************************/
    IVS_VOID ClearDomainRouteMap();

	/******************************************************************
	 function   : ClearProxyDomainRouteMap
	 description: 删除代理域路由
	 output     : NA
	 return     : IVS_VOID
	*******************************************************************/
	IVS_VOID ClearProxyDomainRouteMap();

	/******************************************************************
	 function   : GeDomainRouteParseXML
	 description:  解析获取域路由响应
	 input      : IVS_DOMAIN_ROUTE_LIST * pDomainRouteList
	 input      : CXml & xmlRsp
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 GeDomainRouteParseXML( IVS_DOMAIN_ROUTE_LIST* pDomainRouteList, CXml &xmlRsp);

    /******************************************************************
     function   : FindProxyDomainCode
     description: 根据目标域的域编码查找域路由表，找出对应的代理域;
     input      : const std::string & strDestDomainCode 目标域的域编码;
     output     : std::string & strProxyDomainCode 对应的代理域编码，如果不存在，则为空;
     return     : bool true 存在代理域， false，不存在代理域;
    *******************************************************************/
    bool FindProxyDomainCode(const std::string& strDestDomainCode, std::string& strProxyDomainCode);

    /******************************************************************
     function   : GetDomainRoutebyCode
     description: 根据域编码获取域路由信息;
     input      : const std::string & strDomainCode     域编码;
     output     : IVS_DOMAIN_ROUTE & stDomainRoute      域路由信息;
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 GetDomainRoutebyCode(const std::string& strDomainCode, IVS_DOMAIN_ROUTE& stDomainRoute);

	/******************************************************************
     function   : GetDomainRouteByIP
     description: 根据IP获取域路由信息;
     input      : const std::string& strIP				IP;
     output     : IVS_DOMAIN_ROUTE & stDomainRoute      域路由信息;
     return     : IVS_INT32
    *******************************************************************/
    IVS_INT32 GetDomainRouteByIP(const std::string& strIP, IVS_DOMAIN_ROUTE& stDomainRoute);

	/******************************************************************
     function   : IsInDomainTree
     description: 判断域是否在登陆域及下级域
     input      : strDomainCode 
     output     : 
     return     : bool true成功 false失败
    *******************************************************************/
	bool IsInDomainTree(const std::string& strDomainCode);
	
	/******************************************************************
     function   : GetSameSuperDomainCode
     description: 获取最近的父域节点
     input      : strDomainCode1 域Code1
     input      : strDomainCode2 域Code2
     output     : strSuperDomainCode 父域信息
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    *******************************************************************/
	IVS_INT32 GetSameSuperDomainCode(const std::string& strDomainCode1,const std::string& strDomainCode2,
		std::string& strSuperDomainCode);

	/******************************************************************
     function   : GetAllDomainCode
     description: 获取所有当前域表的域Code
     output     : stDomainCodeVec 所有的域code
     return     : IVS_INT32 IVS_SUCCEED成功 IVS_FAIL失败
    *******************************************************************/
	IVS_INT32 GetAllDomainCode(DOMAIN_CODE_VEC& stDomainCodeVec );
private:
    /******************************************************************
     function   : ModifyProxyDomainRoute
     description: 更新目标域和代理域的映射表;
     output     : NA
     return     : IVS_INT32
    *******************************************************************/
	IVS_INT32 ModifyProxyDomainRoute(IVS_DOMAIN_ROUTE* pDomainRoute);

	/******************************************************************
     function   : SendCmdAndGetInfo
	 description: 发送指令并获取返回信息
	 output     : NA
     return     : IVS_INT32
    *******************************************************************/
	IVS_INT32 SendCmdAndGetInfo(CXml& xmlRsp) const;

	/******************************************************************
	 function   : GetDomainRouteToListParseXML
	 description: 解析域路由到List
	 input      : DOMAIN_CODE_LIST & codeInfoList
	 input      : CXml & xmlRsp
	 output     : NA
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 GetDomainRouteToListParseXML(DOMAIN_CODE_LIST& codeInfoList, CXml &xmlRsp);

	/******************************************************************
	 function   : GetSuperDomainCodeList
	 description: 获取strDomainCode域的所有上级域信息
	 input      : strDomainCode 域Code
	 output     : superDomainCodeList 父域List
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 GetSuperDomainCodeList(SUPER_DOMAIN_CODE_LIST& superDomainCodeList, const std::string& strDomainCode);

	/******************************************************************
	 function   : FindSameSuperDomainCode(调用者保证了list1的size大于等于list2)
	 description: 查询第一个相同父域的域Code
	 input      : superDomainCodeList1 父域List1
	 input      : superDomainCodeList2 父域List2
	 output     : strSuperDomainCode 相同父域
	 return     : IVS_INT32
	*******************************************************************/
	IVS_INT32 FindSameSuperDomainCode(SUPER_DOMAIN_CODE_LIST& superDomainCodeList1,SUPER_DOMAIN_CODE_LIST& superDomainCodeList2,
										std::string& strSuperDomainCode);

	//针对外域相关XML的划分
	IVS_INT32 SplitDevDomainCode(std::string& strBefore, std::string& strAfter);
	//合并设备编码以及域编码
	IVS_INT32 ComplicationDevDomainCode(std::string& strBefore, std::string& strAfter);


private:
	CUserMgr *m_pUserMgr;                               // 用户对象指针
	DOMAIN_CODE_LIST_MAP        m_domainCodeMap;        // 保存域的路由信息
	PROXY_DOMAIN_CODEL_MAP      m_proxyDomainCodeMap;   // 保存代理域的路由信息
};

#endif // __DOMAIN_ROUTE_MGR__
