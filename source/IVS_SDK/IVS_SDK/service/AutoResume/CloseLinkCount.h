/********************************************************************
	filename	: 	CCloseLinkCount.cpp
	author		:	z00201790
	created		:	2012/12/27
	description	:	定义对linkID断开开始计数的管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/27 初始版本;
*********************************************************************/

#ifndef BUSINESS_RESUME_H
#define BUSINESS_RESUME_H

#include "vos.h"
#include "SDKTimer.h"
#include "LockGuard.h"
#include "SDKDef.h"

class CBusinessResume
{
public:
    
    void Init(int iNetElemType, int iPort, int iCount, const std::string& strLinkID, const std::string& strIP)
    {
        this->m_iNetElemType = iNetElemType;
        this->m_iPort = iPort;
        this->m_iCount = iCount;
        this->m_strLinkID = strLinkID;
        this->m_strIP = strIP;
    };

    CBusinessResume& operator = (const CBusinessResume& other)
    {
        if (this == &other)
        {
            return *this;
        }
        this->m_iNetElemType = other.m_iNetElemType;
        this->m_iPort = other.m_iPort;
        this->m_iCount = other.m_iCount;
        this->m_strLinkID = other.m_strLinkID;
        this->m_strIP = other.m_strIP;
        return *this;
    };

    void SetNetElemType(int iNetElemType){ m_iNetElemType = iNetElemType; };
    int GetNetElemType() const { return m_iNetElemType; };
    void SetPort(int iPort) { m_iPort = iPort; };
    int GetPort() const { return m_iPort; };
    void SetCount(int iCount) { m_iCount = iCount; };
    int GetCount() const { return m_iCount; };
    void SetLinkID(const std::string& strLinkID) { m_strLinkID = strLinkID; };
    std::string GetLinkID() const { return m_strLinkID; }; //lint !e1036 !e1536
    void SetIP(const std::string& strIP) { m_strIP = strIP; };
    std::string GetIP() const { return m_strIP; }; //lint !e1036 !e1536

private:
	int m_iNetElemType;
	int m_iPort;
	int m_iCount;
    std::string m_strLinkID;
    std::string m_strIP;
};

typedef std::list<CBusinessResume*> BUSINESSRESUME_LIST;
typedef BUSINESSRESUME_LIST::iterator BUSINESSRESUME_LIST_ITER;

class CUserMgr;
class CCloseLinkCount: public ITrigger
{
public:
	CCloseLinkCount(void);
	~CCloseLinkCount(void);

	virtual void onTrigger(void* /*pArg*/, ULONGLONG /*ullScales*/, TriggerStyle /*style*/);

	void SetUserMgr(CUserMgr *pUserMgr);

public:
	int AddLinkIdToBusinessList(const std::string& strLinkID,int iNetElemType,const std::string& strIP,int iPort);

	int RemoveLinkIdFromBusinessList(const std::string& strLinkID);

private:
	BUSINESSRESUME_LIST m_listBusinessResume;

	VOS_Mutex* m_pMutexLock; //lint !e830

	CUserMgr *m_pUserMgr; //lint !e830
};

#endif
