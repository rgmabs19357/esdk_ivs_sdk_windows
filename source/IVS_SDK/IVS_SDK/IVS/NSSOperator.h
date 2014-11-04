/********************************************************************
	filename	: 	NSSOperator.h
	author		:	z00193167
	created		:	2012/11/05	
	description	:	NSS操作类
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/11/05 初始版本
*********************************************************************/
#ifndef NSS_OPERATOR_H
#define NSS_OPERATOR_H
// #include <map>
#include "bp_log.h"
#include "SDKMainCtrl.h"
#include "bp_net_factory.h"
#include "bp_dispatcher.h"
#include "MKTransID.h"
#include "NSSP.h"
#include "Cmd.h"
#include "vos.h"
#include "ivs_xml.h"
#include "hwsdk.h"
#include "SDKDef.h"

// TLV的结构体
#pragma pack(push, 1)
typedef struct NssTLVMsg
{
	uint16_t usiTag;//lint !e601 !e10
	uint32_t uiLength;//lint !e601 !e10
	char* pszValue;
}TNssTLVMsg;
#pragma pack(pop)

class CNSSOperator
{
public: 
	static CNSSOperator& instance();

public:
	// 手动创建连接
	int CreateClientLink(std::string& strNetElementID, int iNetElementType, const char* pszPeerIP, int iPeerPort, const char* pszLocalIP);

    // 手动创建连接（同步）
    int CreateSyncClientLink(int iNetElementType, const char* pszPeerIP, int iPeerPort, std::string& strNetElementID)const;

	// 同步发送发送命令,内存需要在外部释放掉
	CCmd* SendSyncCmd(CCmd* pCmd, uint32_t uiTimeout = 35, IVS_BOOL bTimeout = 0) const; 

	// 同步发送命令的Map表，响应cmd需要释放内存
	IVS_INT32 SendSyncCmd(CmdMap& cmdMap, uint32_t uiTimeout = 35) const; 

	// 取出cmd中的xml,并返回结果码
	char* ParseCmd2XML(CCmd* pCmd, int& iRspCode) const;

	// 只有NSS头的操作调用这个返回一个结果码
	int ParseCmd2NSS(CCmd* pCmd);

	// 获取Cmd中的数据指针
	char* ParseCmd2Data(CCmd* pCmd, int& Len);

	// 拼装发送到SMU的CMD对象
	CCmd* BuildSMUCmd(unsigned short iReqID,const char* pData, int length = 0);
	// 拼装发送到SCU的CMD对象
	CCmd* BuildSCUCmd(unsigned short iReqID,const char* pData);

    // 拼装发送到SMU的CMD对象
    CCmd* BuildSMUCmd(unsigned short iReqID,const char* pData, const string& sLinkId);
    // 拼装发送到SCU的CMD对象
    CCmd* BuildSCUCmd(unsigned short iReqID,const char* pData, const string& sLinkId);
    // 拼装发送到OMU的CMD对象
    CCmd* BuildOMUCmd(unsigned short iReqID,const char* pData, const string& sLinkId);

	// 设置到模块间的linkid，这里在连接建立后会推送消息，完成连接交换
	void SetSMULinkID(std::string linkid){ m_strSMULinkID = linkid; }; //lint !e1746 不需要
	void SetSCULinkID(std::string linkid) { m_strSCULinkID = linkid; } //lint !e1746 不需要
	void SetOMULinkID(std::string linkid) { m_strOMULinkID = linkid; } //lint !e1746 不需要
    //获取各模块linkId，关闭连接需要
    std::string GetSMULinkID() const{return m_strSMULinkID;}; //lint !e1036
    std::string GetSCULinkID() const { return m_strSCULinkID; }//lint !e1036
    std::string GetOMULinkID() const { return m_strOMULinkID; }//lint !e1036

	// 传入一块TLV的数据首地址，转换成TLV结构体，进行了网络字节序转换
	void ParseData2TLV(const void* pData, TNssTLVMsg& tlvMsg)const;

	IVS_INT32 AddLoginIdAndSendCmd(unsigned short iReqID, const IVS_CHAR* pReqXml) const;
	IVS_INT32 AddLoginIdAndSendCmd(unsigned short iReqID, const IVS_CHAR* pReqXml, IVS_CHAR*& pRsqXml) const;

	// 生成cmd对象,内部为XML
	CCmd* BuildCmd(const std::string& strLinkID, int iNetElementType,unsigned short iReqID,const char* pData) const;
	
    // 生成cmd对象,内部为XML,指定事务ID
    CCmd* BuildCmd(const std::string& strLinkID, int iNetElementType,unsigned short iReqID,const char* pData, const std::string& strTransID) const;
    // 将数据直接拼入CMD
	CCmd* BuildCmd(const std::string& strLinkID, int iNetElementType, unsigned short iReqID, const char* pData, int length) const;
	//调用BP代码生成CMD赋值
	int ConfCmd(CCmd* pCmd,unsigned short iReqID,const char* pData, const std::string& strTransID)const;
private: 
	CNSSOperator(void){;}
	~CNSSOperator(void){;} 
	CNSSOperator(const CNSSOperator&); 
	CNSSOperator& operator = (const CNSSOperator&);          

private:
	std::string m_strSMULinkID;
	std::string m_strSCULinkID;
	std::string m_strOMULinkID;

};
#endif //NSS_OPERATOR_H
