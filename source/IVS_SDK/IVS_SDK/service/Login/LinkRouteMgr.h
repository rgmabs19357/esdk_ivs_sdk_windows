/********************************************************************
 filename    :    LinkRouteMgr.h
 author      :    s00191067
 created     :    2012/11/26
 description :    网络Link路由管理类;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/26 初始版本
*********************************************************************/
#ifndef LINK_ROUTE_MGR
#define LINK_ROUTE_MGR
#include "hwsdk.h"
#include "ivs_error.h"
#include "SDKDef.h"
#include "vos.h"
#include "bp_net_factory.h"

// 连接状态
typedef enum LINKID_STATUS
{
    LINKID_STATUS_OPENNING = 0,     // 新建立
    LINKID_STATUS_ENABLED,               // 可用，即在线但未连接
    LINKID_STATUS_OPENED,                // 连接  
    LINKID_STATUS_CLOSED                 // 断开
}EM_LINKID_STATUS;//lint !e612

// 连接信息类
class CLinkInfo
{
public:
    CLinkInfo()
        : m_iNetElemType(NET_ELE_SMU_NSS)
        , m_iLinkStatus(LINKID_STATUS_OPENED)
    {
    };
    ~CLinkInfo()
    {
    };
    void Init(int iNetElemType, int iLinkStatus, const std::string& strLinkId, const std::string& strDomainCode, const std::string& strDomainIP, const std::string& strNVRCode)
    {
        this->m_iNetElemType = iNetElemType;
        this->m_iLinkStatus = iLinkStatus;
        this->m_strLinkId = strLinkId;
        this->m_strDomainCode = strDomainCode;
        this->m_strNVRCode = strNVRCode;
        this->m_strDomainIP = strDomainIP;
    };
	CLinkInfo& operator = (const CLinkInfo& other)
	{
		if (this == &other)
		{
			return *this;
		}
		this->m_iNetElemType = other.m_iNetElemType;
		this->m_iLinkStatus = other.m_iLinkStatus;
		this->m_strLinkId = other.m_strLinkId;
		this->m_strDomainCode = other.m_strDomainCode;
		this->m_strNVRCode = other.m_strNVRCode;
        this->m_strDomainIP = other.m_strDomainIP;
        return *this;
	};
    // 获取网元类型
    int GetNetElemType() const { return m_iNetElemType; };
    // 设置网元类型
    void SetNetElemType(int iNetElemType) { m_iNetElemType = iNetElemType; };
    // 获取连接状态
    int GetLinkStatus() const { return m_iLinkStatus; };
    // 设置连接状态
    void SetLinkStatus(int iLinkStatus) { m_iLinkStatus = iLinkStatus; };
    // 获取linkId
    std::string GetStrLinkId() const { return m_strLinkId; }; //lint !e1036 !e1536
    // 设置linkId
    void SetStrLinkId(const std::string& strLinkID) { m_strLinkId = strLinkID; };
    // 获取域编码
    std::string GetStrDomainCode() const { return m_strDomainCode; }; //lint !e1036 !e1536
    // 设置域编码
    void SetStrDomainCode(const std::string& strDomainCode) { m_strDomainCode = strDomainCode; };

    std::string GetStrDomainIP() const { return m_strDomainIP; };

    void SetStrDomainIP(const std::string& strDomainIP) {m_strDomainIP = strDomainIP; };

    // 获取NVR编码
    std::string GetStrNVRCode() const { return m_strNVRCode; }; //lint !e1036 !e1536
    // 设置NVR编码
    void SetStrNVRCode(const std::string& strNVRCode) { m_strNVRCode = strNVRCode; };



private:
    int m_iNetElemType;      // 网元类型,参考bp_def.h.取值范围是20105:NET_ELE_SMU_NSS 10105:NET_ELE_SCU_NSS 60105:NET_ELE_OMU_NSS
    int m_iLinkStatus;       // 连接状态 取值范围参考LINKID_STATUS: 0-新建立，未收到异步的建立成功消息；1-连接；2-断开
    std::string m_strLinkId;      // linkId
    std::string m_strDomainCode;  // 域编码
    std::string m_strDomainIP;  // 域编码，用于应对业务过程中，下级域更改IP（域编码不变）
    std::string m_strNVRCode;

};

typedef std::list<CLinkInfo*> LINKID_INFO_LIST;
typedef LINKID_INFO_LIST::iterator LINKID_INFO_LIST_ITER;

typedef std::map<int, LINKID_INFO_LIST*> LINK_ROUTE_MAP;
typedef LINK_ROUTE_MAP::iterator LINK_ROUTE_MAP_ITER;

class CLinkRouteMgr
{
private:
    CLinkRouteMgr(void);

public:
    ~CLinkRouteMgr(void);

    static CLinkRouteMgr& instance()
    {
        static CLinkRouteMgr linkRouteMgr;
        return linkRouteMgr;
    }

	/******************************************************************
	 function   : AddLinkInfo
	 description: 在list中添加一个link( 无iSessionID对应List则先创建List 再添加link)
	 input      : int iSessionID 标识唯一用户
	 input      : const CLinkInfo & oLinkIDInfo 待添加的Link信息
	 output     : NA
	 return     : int 成功返回0，失败返回1
	*******************************************************************/
	int AddLinkInfo(int iSessionID, const CLinkInfo& oLinkIDInfo);
	
	/******************************************************************
	 function   : UpdateLinkInfo
	 description: 更新LinkInfo
	 input      : int iSessionID 标识唯一用户
	 input      : const CLinkInfo& oLinkIDInfo 待更新的Link信息
	 input		：const CLinkInfo& oLinkIDInfo 是否需要更新
	 output     : NA
	 return     : int 成功返回0，失败返回1
	*******************************************************************/
    int UpdateLinkInfo(int iSessionID, const CLinkInfo& oLinkIDInfo, bool bInsertOnly = false);

	/******************************************************************
	 function   : RemoveLinkInfo
	 description: 从list中删除一个link
	 input      : int iSessionID 标识唯一用户
	 input      : const std::string & strLinkId 标识唯一连接
	 output     : NA
	 return     : int 成功返回0，失败返回1
	*******************************************************************/
	int RemoveLinkInfo(int iSessionID, const std::string& strLinkId);
	
	/******************************************************************
	 function   : DeleteLinkList
	 description: 删除一个用户的link List
	 input      : int iSessionID  标识唯一用户
	 output     : NA
	 return     : void
	*******************************************************************/
	void DeleteLinkList(int iSessionID);
	
	/******************************************************************
	 function   : FindSessionIDByLinkID
	 description:  根据linkid 查找session id
	 input      : const std::string & strLinkId 标识唯一连接
	 output     : NA
	 return     : int 标识唯一用户
	*******************************************************************/
	int FindSessionIDByLinkID(const std::string& strLinkId);
	
	/******************************************************************
	 function   : FindLinkIDInfo
	 description:  根据linkID查找link info
	 input      : const std::string & strLinkId 标识唯一连接
	 input      : CLinkInfo & oLinkIDInfo 连接信息
	 output     : NA
	 return     : bool 查询到返回true，未找到返回false
	*******************************************************************/
	bool FindLinkIDInfo(const std::string& strLinkId, CLinkInfo &oLinkIDInfo);
	
    /******************************************************************
     function   : SetLinkIDStatus
     description: 根据linkid 设置link状态
     input      : const std::string & strLinkId 标识唯一连接
     input      : int iLinkStatus 连接状态
     output     : NA
     return     : void
    *******************************************************************/
    void SetLinkIDStatus(const std::string& strLinkId, int iLinkStatus);

    /******************************************************************
     function   : SetSmuLinkIDByDomainCode
     description: 根据域编码更新linkID信息
     input      : int iSessionID
     input      : const std::string & strDomainCode
     input      : const std::string & strLinkID
     input      : int iLinkStatus
     output     : NA
     return     : void
    *******************************************************************/
    void SetLinkIDByDomainCode(int iSessionID, int iNetElemType, const std::string& strDomainCode, const std::string& strLinkID, int iLinkStatus);

    /******************************************************************
     function   : UpdateLocalDomainCode
     description: 更新本域SMU域CODE
     input      : int iSessionID 标识唯一用户
     input      : const std::string & strDomainCode 本域编码
     output     : NA
     return     : void
    *******************************************************************/
    void UpdateLocalDomainCode(int iSessionID, const std::string& strDomainCode);

    /******************************************************************
     function   : GetSmuLinkIDInfoByDomainCode
     description: 根据iSessionId和 域编码 查询linkIDInfo
     input      : int iSessionID
     input      : const std::string & strDomaiCode 域编码
     output      : CLinkInfo & oLinkIDInfo 连接信息
     return     : bool 查询到返回true，未查到返回false
    *******************************************************************/
    bool GetSmuLinkIDInfoByDomainCode(int iSessionID, const std::string &strDomaiCode, CLinkInfo &oLinkIDInfo);

    /******************************************************************
     function   : GetScuLinkByDomainCodeAndNVRCode
     description: 根据iSessionId，域编码和NVRCode 查询linkIDInfo
     input      : int iSessionID 标识唯一用户
     input      : const std::string & strDomaiCode 域编码
     input      : const std::string & strNVRCode NVR编码
     output      : CLinkInfo & oLinkIDInfo 连接信息
     return     : bool 查询到返回true，未查到返回false
    *******************************************************************/
    bool GetScuLinkByDomainCodeAndNVRCode(int iSessionID, const std::string &strDomaiCode, const std::string &strNVRCode, CLinkInfo &oLinkIDInfo);

    /******************************************************************
     function   : GetLinkIDInfoByDomainCode
     description: 根据iSessionId，网元类型iNetElemType和 域编码 查询linkIDInfo
     input      : int iSessionID  标识唯一用户
     input      : int iNetElemType 网元类型
     input      : const std::string & strDomaiCode 域编码
     output      : CLinkInfo & oLinkIDInfo  连接信息
     return     : bool 查询到返回true，未查到返回false
    *******************************************************************/
    bool GetLinkIDInfoByDomainCode(int iSessionID, int iNetElemType, const std::string &strDomaiCode, CLinkInfo &oLinkIDInfo);

	/******************************************************************
     function		: DecideDomainCode
     description	: 根据iSessionId，网元类型iNetElemType判断域编码是否有效
     input				: int iSessionID  标识唯一用户
     input				: int iNetElemType 网元类型
     input				: const std::string & strDomaiCode 域编码
     output			: NA
     return			: 成功返回IVS_SUCCESS，失败返回错误码
    *******************************************************************/
    IVS_INT32 DecideDomainCode(int iSessionID, int iNetElemType, const std::string &strDomaiCode);

    /******************************************************************
     function   : DeleteLinkExceptLocalDomain
     description: 删除除指定连接外的连接
     input      : int iSessionID
     input      : int iNetElemType
     input      : const std::string & strDomaiCode
     output     : NA
     return     : void
    *******************************************************************/
    void DeleteLinkExceptLocalDomain(int iSessionID, int iNetElemType, const std::string &strDomaiCode);

	/******************************************************************
	 function   : ClearAllLink
	 description: 清除所有用户的连接，并释放内存
	 output     : NA
	 return     : void
	*******************************************************************/
	void ClearAllLink();

//private:
    // 填充LinkIDInfo结构体
    //void FillStLinkIDInfo(const LINKID_INFO* pstLinkIdInfo, LINKID_INFO &stLinkIDInfo) const;

    // 填充LinkIDInfo指针内容
    //void FillPointerStLinkIDInfo(LINKID_INFO *&pstLinkIdInfo, const LINKID_INFO &stLinkIDInfo) const;
private:
	LINK_ROUTE_MAP                         m_LinkRouteMap;            // 保存连接信息
    VOS_Mutex*                             m_pMutexLinkRouteMap;      // 保护锁
};
#endif //LINK_ROUTE_MGR
