/********************************************************************
 filename    :    UserDataMgr.cpp
 author      :    x00192614
 created     :    2013/01/17
 description :    用户数据上传下载
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/17 初始版本
*********************************************************************/
#include "UserDataMgr.h"
#include "UserMgr.h"
#include "openssl/md5.h"
#include "IVS_Trace.h"

CUserDataMgr::CUserDataMgr(void)
    : m_pUserMgr(NULL)
{
    
}


CUserDataMgr::~CUserDataMgr(void)
{
    m_pUserMgr = NULL;
}

void CUserDataMgr::SetUserMgr(CUserMgr *pUserMgr)
{
    m_pUserMgr = pUserMgr;
}

// 上传用户数据到服务器
IVS_INT32 CUserDataMgr::BackupUserData(const IVS_CHAR* pFileName)const
{
    CHECK_POINTER(pFileName, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("filename %s", pFileName);
    int iRet = IVS_FAIL;

	//读取压缩文件
 	IVS_UINT32 ifLength = 0;
 	//std::string ifBuffer;
 	ifstream ifUserFile;
 	ifUserFile.open(pFileName, ios::binary);
    // 校验文件大小
	ifUserFile.seekg(0, ios::end); //lint !e747
	ifLength = (IVS_UINT32)ifUserFile.tellg();
	ifUserFile.seekg(0, ios::beg); //lint !e747
	if (ifLength > MAX_FILE_LENGTH)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "Backup UserData failed", "file is oversize");
		return IVS_FAIL;
	}
	IVS_CHAR *buffer = IVS_NEW((IVS_CHAR* &)buffer, ifLength+1);
	CHECK_POINTER(buffer, IVS_ALLOC_MEMORY_ERROR);
 	memset(buffer, 0, ifLength+1);
 	ifUserFile.read(buffer, ifLength); //lint !e747
 	ifUserFile.close();

	// 拼装文件的TLV
	TNssTLVMsg tlvMsg;
	tlvMsg.usiTag = 0x0003; // 已二进制方式读取
	tlvMsg.uiLength = (IVS_UINT32)ifLength;// + sizeof(uint16_t) + sizeof(uint32_t);
	tlvMsg.pszValue = buffer;
	
	IVS_UINT32 tlvFileBuffer = (IVS_UINT32)(ifLength + (sizeof(IVS_USHORT) + sizeof(IVS_UINT32)));
	//IVS_CHAR *tlvMsgBuffer = new IVS_CHAR [tlvFileBuffer + 1];
	IVS_CHAR* tlvMsgBuffer = IVS_NEW((IVS_CHAR* &)tlvMsgBuffer, tlvFileBuffer + 1);
	if (NULL == tlvMsgBuffer)
	{
		IVS_DELETE(buffer, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Backup User Data", "alloc memory failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(tlvMsgBuffer, 0, tlvFileBuffer + 1);

	//将文件PLV转换成字符串
	uint16_t usiTag = htons(tlvMsg.usiTag);
	uint32_t uiLength = htonl(tlvMsg.uiLength);

// 	uint16_t usiXmlTag = htons(tlvXMLMsg.usiTag);
// 	uint32_t uiXmlLength = htonl(tlvXMLMsg.uiLength);

	IVS_UINT32 uiLen = 0;
    if (!CToolsHelp::Memcpy(tlvMsgBuffer,sizeof(IVS_USHORT), &usiTag, sizeof(IVS_USHORT)))
    {
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(tlvMsgBuffer, MUILI);
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy tlvMsgBuffer", "fail");
        return IVS_ALLOC_MEMORY_ERROR;
    }
	//memcpy(tlvMsgBuffer, &usiXmlTag, sizeof(uint16_t));
	uiLen += sizeof(IVS_USHORT);
    if (!CToolsHelp::Memcpy(tlvMsgBuffer + uiLen, sizeof(IVS_UINT32), &uiLength, sizeof(IVS_UINT32)))
    {
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(tlvMsgBuffer, MUILI);
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy tlvMsgBuffer", "fail");
        return IVS_ALLOC_MEMORY_ERROR;
    }
	//memcpy(tlvMsgBuffer + uiLen, &uiXmlLength, sizeof(uint32_t));
    uiLen += sizeof(IVS_UINT32);
    if (!CToolsHelp::Memcpy(tlvMsgBuffer + uiLen, tlvMsg.uiLength, tlvMsg.pszValue, tlvMsg.uiLength))
    {
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(tlvMsgBuffer, MUILI);
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy tlvMsgBuffer", "fail");
        return IVS_ALLOC_MEMORY_ERROR;
    }
    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(tlvMsgBuffer, MUILI);
        return iGetLinkRet;
    }
    CCmd* pCmd = CNSSOperator::instance().BuildCmd(strSMULinkID, NET_ELE_SMU_NSS, NSS_UPLOAD_USER_CONFIG_FILE_REQ, tlvMsgBuffer, (IVS_INT32)(tlvFileBuffer + 1));
    if (NULL == pCmd)
    {
        IVS_DELETE(buffer, MUILI);
        IVS_DELETE(tlvMsgBuffer, MUILI);
        BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Build Cmd ", "fail");
        return IVS_OPERATE_MEMORY_ERROR;
    }

	//调试用
//	RestoreUserData(pUserDataFile, pCmd);
    //发送消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    if (NULL == pCmdRsp)
    {
        IVS_DELETE(buffer, MUILI);
        IVS_DELETE(tlvMsgBuffer, MUILI);
        BP_RUN_LOG_ERR(IVS_NET_RECV_TIMEOUT, "Send Cmd ", "fail");
        return IVS_NET_RECV_TIMEOUT;
    }
    IVS_DELETE(buffer, MUILI);
    IVS_DELETE(tlvMsgBuffer, MUILI);
    iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
    HW_DELETE(pCmdRsp);
    return iRet;
}

// 下载用户数据
IVS_INT32 CUserDataMgr::RestoreUserData(const IVS_CHAR* pFileName)
{
    CHECK_POINTER(pFileName, IVS_PARA_INVALID);
	CHECK_POINTER(m_pUserMgr,IVS_OPERATE_MEMORY_ERROR);
    IVS_DEBUG_TRACE("filename %s", pFileName);
    CXml reqXml;
    IVS_INT32 iRet = GetRestoreUserDataReqXml(m_pUserMgr->GetLoginId(), reqXml); //lint !e1013 !e1055 !e746 !e64 !e613
	if(IVS_SUCCEED != iRet)
	{		
		BP_RUN_LOG_ERR(iRet, "Backup User Data", "Get RestoreUserData ReqXml fail");
		return iRet;
	}
    IVS_UINT32 iLen = 0;
    const IVS_CHAR* pUserDataTemp = reqXml.GetXMLStream(iLen);
    CHECK_POINTER(pUserDataTemp, IVS_OPERATE_MEMORY_ERROR);
    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
    //创建要发送的CMD，拼装了NSS消息头
    CCmd* pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_DOWNLOAD_USER_CONFIG_FILE_REQ, pUserDataTemp, strSMULinkID);
    CHECK_POINTER(pCmd, IVS_OPERATE_MEMORY_ERROR);

    //发送消息
    CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
    CHECK_POINTER(pCmdRsp, IVS_NET_RECV_TIMEOUT);

	// 先判断返回值
    iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	if (IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "Backup User Data fail", "NA");
		return iRet;
	}
 	IVS_INT32 length = 0;
 	const IVS_CHAR* pRsp =  CNSSOperator::instance().ParseCmd2Data(pCmdRsp, length);
    if (NULL == pRsp)
    {
        BP_RUN_LOG_ERR( IVS_FAIL, "Restore User Data", "SMU response failed");
        HW_DELETE(pCmdRsp);
        return IVS_FAIL;
    }
	//pRsp进行压缩并解压文件
	iRet = GetUserDataResp(pFileName, pRsp);
	IVS_DELETE(pRsp, MUILI);
    HW_DELETE(pCmdRsp);
    return iRet;
}

// 获取下载用户数据请求xml
IVS_INT32 CUserDataMgr::GetRestoreUserDataReqXml(const IVS_CHAR* cLoginId, CXml &reqXml) const
{
    CHECK_POINTER(cLoginId, IVS_OPERATE_MEMORY_ERROR);
    (void)reqXml.AddDeclaration("1.0","UTF-8","");
    (void)reqXml.AddElem("Content");

	reqXml.GetRootPos();
    return IVS_SUCCEED;

}

// 解析备份用户数据响应xml
//IVS_INT32 CUserDataMgr::GetRestoreUserDataParseRspXml(const string &strFailMd5, const IVS_CHAR* pFileName, CXml &rspXml) const
//{
//    CHECK_POINTER(pFileName, IVS_PARA_INVALID);
//    IVS_DEBUG_TRACE("strFailMd5: %s,pFileName: %s", strFailMd5.c_str(), pFileName);
//    const IVS_CHAR* szXMLValue = NULL;
//    if (!rspXml.FindElemEx("Content/FileMD5"))
//    {
//        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get RestoreUserData Parse RspXml", "Content/FileMD5 not exist");
//        return IVS_XML_INVALID;
//    }
//    szXMLValue = rspXml.GetElemValue();
//    if (NULL != szXMLValue)                  
//    {                                                 
//        //(void)CToolsHelp::Strncpy(pUserDataFile, szXMLValue, strlen(szXMLValue)); 
//    } 
//    return IVS_SUCCEED;
//}

// 获取文件的MD5值
std::string CUserDataMgr::GetFileMd5(const IVS_CHAR *pFileName) const
{
    CHECK_POINTER(pFileName, "");
    IVS_DEBUG_TRACE("pFileName:%s", pFileName);
	MD5_CTX md5;
	IVS_INT64 readBuffer = 1024;
	unsigned char md[16]; 
	char tmp[33]={0};
	IVS_UINT32 length,i;
	IVS_CHAR buffer[1024];
	std::string hash="";
	MD5_Init(&md5);
	ifstream fin(pFileName, ios::in|ios::binary);
	while(!fin.eof())
	{
		fin.read(buffer, readBuffer); //lint !e747
		length = (IVS_UINT32)fin.gcount();
		if (length > 0)
		{
			MD5_Update(&md5,buffer, length);
		}
	}

	MD5_Final(md,&md5);
	for(i=0; i<16; i++)
	{
		//sprintf(tmp, "%02X", md[i]);
		(void)snprintf(tmp,32, "%02X", md[i]);
		hash+=(string)tmp;
	}
	return hash; //lint !e252 !e1036
} //lint !e253

// 获取用户数据请求
IVS_INT32 CUserDataMgr::GetUserDataResp(const IVS_CHAR* pFileName, const IVS_CHAR* pResp) const
{
    CHECK_POINTER(pFileName, IVS_PARA_INVALID);
    CHECK_POINTER(pResp, IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("pFileName:%s, pResp:%s", pFileName, pResp);
	//取出MD5值
	IVS_CHAR szMD5[33] = {0};
	if (!CToolsHelp::Memcpy(szMD5, 32, pResp, 32))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy szMD5", "fail");
		return IVS_ALLOC_MEMORY_ERROR;
	}

	//取出文件大小
	IVS_UINT32 iFilelength = 0;
    if (!CToolsHelp::Memcpy(&iFilelength, sizeof(IVS_UINT32), pResp + sizeof(IVS_USHORT) + 32, sizeof(IVS_UINT32)))
    {
        BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy pResp", "fail");
        return IVS_ALLOC_MEMORY_ERROR;
    }
	IVS_UINT32 uiFileLength = ntohl(iFilelength);
	// 这里做个大小保护，如果太大会挂
	if (uiFileLength > MAX_FILE_LENGTH)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "file length oversize","uiFileLength= %d", uiFileLength);
		return IVS_FAIL;
	}

	// 取出文件流
	const IVS_CHAR *pFile = pResp + sizeof(IVS_USHORT) + sizeof(IVS_UINT32) + 32;
	ofstream fUserData (pFileName,ofstream::binary);
	fUserData.write(pFile, (IVS_INT64)uiFileLength);
	fUserData.close();

	// 对比MD5 查看是否正确下载
	std::string strTempMd5 = szMD5;
	const std::string strMd5 = GetFileMd5(pFileName);
	if (0 != strcmp(strMd5.c_str(), strTempMd5.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_SDK_ERR_CHEACKMD5_FAILED, "Get UserData Resp", "write file failed.");
		return IVS_SDK_ERR_CHEACKMD5_FAILED;
	}

	return IVS_SUCCEED;
}

// 上传logo文件
IVS_INT32 CUserDataMgr::UploadLogo(IVS_UINT32 uiLogoType, const IVS_CHAR* pLogoFile)const
{
	CHECK_POINTER(pLogoFile, IVS_PARA_INVALID);
    CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	// logo类型,转成网络字节
	IVS_USHORT u16logType = htons((IVS_USHORT)uiLogoType);
	// logo文件路径，包含文件名
	std::string strLogoFile = pLogoFile;
	// 获取文件后缀名, 最大位10
	std::string strLogoFileSuffix = strLogoFile.substr(strLogoFile.rfind(".") + 1, strlen(strLogoFile.c_str()));
	if (strlen(strLogoFileSuffix.c_str()) > MAX_FILE_SUFFIX_LENGTH)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "file suffix oversize", "NA")
			return IVS_FAIL;
	}

	//读取文件
	IVS_UINT32 ifLength = 0;
	ifstream ifLogoFile;
	ifLogoFile.open(strLogoFile.c_str(), ios::binary);
	if(!ifLogoFile)      
	{        
		BP_RUN_LOG_ERR(IVS_FAIL, "file open failed", "NA")
			return IVS_FAIL;
	} 
	// 校验文件大小，不能超过最大
	ifLogoFile.seekg(0, ios::end); //lint !e747
	ifLength = (IVS_UINT32)ifLogoFile.tellg();
	ifLogoFile.seekg(0, ios::beg); //lint !e747
	if (ifLength > MAX_FILE_LENGTH)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "upload logo failed", "file is oversize");
		return IVS_FAIL;
	}
	IVS_CHAR *buffer = IVS_NEW(buffer,ifLength + 1);
	CHECK_POINTER(buffer, IVS_ALLOC_MEMORY_ERROR);
	memset(buffer, 0, ifLength + 1);
	ifLogoFile.read(buffer, ifLength); //lint !e747
	ifLogoFile.close();

	// 拼装文件的TLV
	TNssTLVMsg tlvMsg;
	tlvMsg.usiTag = 0x0003; // 已二进制方式读取
	tlvMsg.uiLength = (IVS_UINT32)ifLength;
	tlvMsg.pszValue = buffer;

	IVS_UINT32 uiMsgBuffer = ifLength  + 2*sizeof(IVS_USHORT) + sizeof(IVS_UINT32) + MAX_FILE_SUFFIX_LENGTH;
	IVS_CHAR* pMsgBuffer = IVS_NEW(pMsgBuffer, uiMsgBuffer);
	if (NULL == pMsgBuffer)
	{
		IVS_DELETE(buffer, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "UploadLogo", "alloc memory failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(pMsgBuffer, 0, uiMsgBuffer);

	//将文件TLV转换成字符串
	IVS_USHORT usiTag = htons(tlvMsg.usiTag);
	IVS_UINT32 uiLength = htonl(tlvMsg.uiLength);

	IVS_UINT32 uiLen = 0;
	(void)CToolsHelp::Memcpy(pMsgBuffer, sizeof(IVS_USHORT), &u16logType, sizeof(IVS_USHORT));
	uiLen += sizeof(IVS_USHORT);
	(void)CToolsHelp::Memcpy(pMsgBuffer + uiLen, MAX_FILE_SUFFIX_LENGTH, strLogoFileSuffix.c_str(), strlen(strLogoFileSuffix.c_str()));
	uiLen += MAX_FILE_SUFFIX_LENGTH;
	(void)CToolsHelp::Memcpy(pMsgBuffer + uiLen, sizeof(IVS_USHORT), &usiTag, sizeof(IVS_USHORT));
	uiLen += sizeof(IVS_USHORT);
	(void)CToolsHelp::Memcpy(pMsgBuffer + uiLen, sizeof(IVS_UINT32), &uiLength, sizeof(IVS_UINT32));
	uiLen += sizeof(IVS_UINT32);
	(void)CToolsHelp::Memcpy(pMsgBuffer +  uiLen, tlvMsg.uiLength, tlvMsg.pszValue, tlvMsg.uiLength);

    // 获取本域SMU连接
    std::string strSMULinkID;
    IVS_INT32 iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
        return iGetLinkRet;
    }
	
	CCmd* pCmd = CNSSOperator::instance().BuildCmd(strSMULinkID, NET_ELE_SMU_NSS,NSS_UPLOAD_LOGO_REQ, pMsgBuffer, (int)uiMsgBuffer);
	if (NULL == pCmd)
	{
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Build Cmd ", "fail");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	//发送消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	if (NULL == pCmdRsp)
	{
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		BP_RUN_LOG_ERR(IVS_NET_RECV_TIMEOUT, "Send Cmd ", "fail");
		return IVS_NET_RECV_TIMEOUT;
	}
	IVS_DELETE(buffer, MUILI);
	IVS_DELETE(pMsgBuffer, MUILI);
	IVS_INT32 iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);

	return iRet;
}

// 下载logo文件
IVS_INT32 CUserDataMgr::GetLogo(IVS_UINT32 uiLogoType, const IVS_CHAR* pLogoFile)const
{
	CHECK_POINTER(pLogoFile, IVS_PARA_INVALID);
	// 判断文件是否存在，不存在返回失败
	if(_access(pLogoFile,0) != 0)
	{
		BP_RUN_LOG_ERR(IVS_OPEN_FILE_ERROR, "file not exist", "NA");
		return IVS_OPEN_FILE_ERROR;
	}
	// 根据文件全路径，读取文件，并生成MD5
	const std::string strSendMd5 = GetFileMd5(pLogoFile);

	// 拼装请求
	CXml xmlReq;
	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");
	(void)xmlReq.AddChildElem("LogoInfo");
	(void)xmlReq.IntoElem();
	(void)xmlReq.AddChildElem("LogoType");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)uiLogoType).c_str());
	(void)xmlReq.AddElem("MD5");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(strSendMd5.c_str());
	xmlReq.OutOfElem();
	xmlReq.OutOfElem();

	unsigned int uilen = 0;
	const char* pReq = xmlReq.GetXMLStream(uilen);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);
    // 获取本域SMU连接
    std::string strSMULinkID;
	if (NULL == m_pUserMgr)
	{
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Get Logo", "m_pUserMgr = NULL");
		return IVS_OPERATE_MEMORY_ERROR;
	}
    IVS_INT32 iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
        return iGetLinkRet;
    }
	// 构造发送CMD对象
	CCmd* pCmd = CNSSOperator::instance().BuildSMUCmd(NSS_GET_LOGO_REQ,pReq,strSMULinkID);
	if (NULL == pCmd)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "build cmd failed", "NA");
		return IVS_FAIL;
	}
	// 发送nss消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	if (NULL == pCmdRsp)
	{
		BP_RUN_LOG_ERR(IVS_NET_RECV_TIMEOUT, "Send General AuthCmd failed", "msg no response");
		return IVS_NET_RECV_TIMEOUT;
	}

	// 先判断返回值
	IVS_INT32 iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	if (IVS_SUCCEED != iRet)
	{
		HW_DELETE(pCmdRsp);
		BP_RUN_LOG_ERR(iRet, "download failed", "NA");
		return iRet;
	}

	// 解析响应CMD，获取返回码
	int ilen = 0;
	IVS_CHAR* pData = CNSSOperator::instance().ParseCmd2Data(pCmdRsp, ilen);
	HW_DELETE(pCmdRsp);
	if (0 == ilen || NULL == pData)
	{
		BP_RUN_LOG_INF("rsp content length is zero ,file is new", "NA");
		return IVS_SUCCEED;
	}

	IVS_CHAR szMD5[33] = {0};
	(void)CToolsHelp::Memcpy(szMD5, 32 , pData, 32);
	// 解析TLV，将数据保存到目录中
	uint32_t iFilelength = 0;
	if (!CToolsHelp::Memcpy(&iFilelength, sizeof(IVS_UINT32), pData + 32 + sizeof(IVS_USHORT), sizeof(IVS_UINT32)))
	{
		IVS_DELETE(pData, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Copy pResp", "fail");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	IVS_DELETE(pData, MUILI);
	IVS_UINT32 uiFileLength = ntohl(iFilelength);
	const IVS_CHAR *pFile = pData + 32 + (sizeof(IVS_USHORT) + sizeof(IVS_UINT32));
	// pLogoFile重命名文件名为_temp
	std::string strFile = pLogoFile;
	std::string strFilePath = strFile.substr(0, strFile.rfind("\\"));
	std::string strFileWholeName = strFile.substr(strFile.rfind("\\") + 1, strlen(strFile.c_str()));
	std::string strFileName = strFileWholeName.substr(0, strFileWholeName.rfind("."));
	std::string strFileSuffix = strFileWholeName.substr(strFileWholeName.rfind("."), strlen(strFileWholeName.c_str()));
	std::string strFileRename = strFileName;
	strFileRename += "_temp";
	strFileRename += strFileSuffix;
	std::string strRenameFile = strFilePath;
    strRenameFile += "\\";
    strRenameFile +=  strFileRename;
	ofstream fLogoData (strRenameFile.c_str(),ofstream::binary);
	fLogoData.write(pFile, (IVS_INT64)uiFileLength);
	fLogoData.close();
	const std::string strMd5 = GetFileMd5(strRenameFile.c_str());
	if (0 != strcmp(strMd5.c_str(), szMD5))
	{
		// 删除文件
		(void)remove(strRenameFile.c_str());
		BP_RUN_LOG_ERR(IVS_SDK_ERR_CHEACKMD5_FAILED, "Get logo Resp", "md5 not match");
		return IVS_SDK_ERR_CHEACKMD5_FAILED;
	}
	return IVS_SUCCEED;
}

// 上传申明信息
IVS_INT32 CUserDataMgr::UploadStatement(const IVS_CHAR* pLanguageType,const IVS_CHAR* pStatementFile)const
{
	CHECK_POINTER(pLanguageType, IVS_PARA_INVALID);
	CHECK_POINTER(pStatementFile, IVS_PARA_INVALID);
	// 申明信息文件路径，包含文件名
	std::string strStatementFile = pStatementFile;
	// 获取文件后缀名, 最大位10
	std::string strStatementFileSuffix = strStatementFile.substr(strStatementFile.rfind(".") + 1, strlen(strStatementFile.c_str()));
	if (strlen(strStatementFileSuffix.c_str()) > MAX_FILE_SUFFIX_LENGTH)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "file suffix oversize", "NA")
		return IVS_FAIL;
	}

	//读取文件
	IVS_UINT32 ifLength = 0;
	ifstream ifLogoFile;
	ifLogoFile.open(strStatementFile.c_str(), ios::binary);
	if(!ifLogoFile)      
	{        
		BP_RUN_LOG_ERR(IVS_FAIL, "file open failed", "NA")
		return IVS_FAIL;
	} 
	// 校验文件大小，不能超过最大
	ifLogoFile.seekg(0, ios::end); //lint !e747
	ifLength = (IVS_UINT32)ifLogoFile.tellg();
	ifLogoFile.seekg(0, ios::beg); //lint !e747
	if (ifLength > MAX_FILE_LENGTH)
	{
		BP_RUN_LOG_ERR(IVS_FAIL, "upload logo failed", "file is oversize");
		return IVS_FAIL;
	}
	IVS_CHAR *buffer = IVS_NEW(buffer,ifLength + 1);
	CHECK_POINTER(buffer, IVS_ALLOC_MEMORY_ERROR);
	memset(buffer, 0, ifLength + 1);
	ifLogoFile.read(buffer, ifLength); //lint !e747
	ifLogoFile.close();

	// 拼装文件的TLV
	TNssTLVMsg tlvMsg;
	tlvMsg.usiTag = 0x0003; // 以二进制方式存取
	tlvMsg.uiLength = (IVS_UINT32)ifLength;
	tlvMsg.pszValue = buffer;

	IVS_UINT32 uiMsgBuffer = ifLength  + sizeof(IVS_USHORT) + sizeof(IVS_UINT32) + MAX_FILE_SUFFIX_LENGTH + LANGUAGE_TYPE_LENGTH;
	IVS_CHAR* pMsgBuffer = IVS_NEW(pMsgBuffer, uiMsgBuffer);
	if (NULL == pMsgBuffer)
	{
		IVS_DELETE(buffer, MUILI);
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Upload Statement", "alloc memory failed");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	memset(pMsgBuffer, 0, uiMsgBuffer);

	//将文件TLV转换成字符串
	IVS_USHORT usiTag = htons(tlvMsg.usiTag);
	IVS_UINT32 uiLength = htonl(tlvMsg.uiLength);

	IVS_UINT32 uiLen = 0;
	(void)CToolsHelp::Memcpy(pMsgBuffer, LANGUAGE_TYPE_LENGTH, pLanguageType, strlen(pLanguageType));
	uiLen += LANGUAGE_TYPE_LENGTH;
	(void)CToolsHelp::Memcpy(pMsgBuffer + uiLen, MAX_FILE_SUFFIX_LENGTH, strStatementFileSuffix.c_str(), strlen(strStatementFileSuffix.c_str()));
	uiLen += MAX_FILE_SUFFIX_LENGTH;
	(void)CToolsHelp::Memcpy(pMsgBuffer + uiLen, sizeof(IVS_USHORT), &usiTag, sizeof(IVS_USHORT));
	uiLen += sizeof(IVS_USHORT);
	(void)CToolsHelp::Memcpy(pMsgBuffer + uiLen, sizeof(IVS_UINT32), &uiLength, sizeof(IVS_UINT32));
	uiLen += sizeof(IVS_UINT32);
	(void)CToolsHelp::Memcpy(pMsgBuffer +  uiLen, tlvMsg.uiLength, tlvMsg.pszValue, tlvMsg.uiLength);

    // 获取本域SMU连接
    std::string strSMULinkID;

    IVS_INT32 iGetLinkRet = IVS_FAIL;
	if (NULL != m_pUserMgr)
	{
		iGetLinkRet = m_pUserMgr->GetLocalDomainLinkID(NET_ELE_SMU_NSS, strSMULinkID);
	}
	
    if (IVS_SUCCEED != iGetLinkRet)
    {
        BP_RUN_LOG_ERR(iGetLinkRet, "Get LocalDomainLinkID failed", "NA");
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
        return iGetLinkRet;
    }
	// TODO 修改为sendcmd?
	CCmd* pCmd = CNSSOperator::instance().BuildCmd(strSMULinkID, NET_ELE_SMU_NSS, NSS_UPLOAD_STATEMENT_REQ, pMsgBuffer, (int)uiMsgBuffer);
	if (NULL == pCmd)
	{
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		BP_RUN_LOG_ERR(IVS_OPERATE_MEMORY_ERROR, "Build Cmd ", "fail");
		return IVS_OPERATE_MEMORY_ERROR;
	}

	//发送消息
	CCmd *pCmdRsp = CNSSOperator::instance().SendSyncCmd(pCmd);
	if (NULL == pCmdRsp)
	{
		IVS_DELETE(buffer, MUILI);
		IVS_DELETE(pMsgBuffer, MUILI);
		BP_RUN_LOG_ERR(IVS_NET_RECV_TIMEOUT, "Send Cmd ", "fail");
		return IVS_NET_RECV_TIMEOUT;
	}
	IVS_DELETE(buffer, MUILI);
	IVS_DELETE(pMsgBuffer, MUILI);
	IVS_INT32 iRet = CNSSOperator::instance().ParseCmd2NSS(pCmdRsp);
	HW_DELETE(pCmdRsp);

	return iRet;
}

// 获取申明信息
IVS_INT32 CUserDataMgr::GetStatement(const IVS_CHAR* pLanguageType,IVS_CHAR** pStatement)
{
	// 拼装请求
	CXml xmlReq;
	(void)xmlReq.AddDeclaration("1.0","UTF-8","");
	(void)xmlReq.AddElem("Content");
	(void)xmlReq.AddChildElem("StatementInfo");
	(void)xmlReq.IntoElem();
	(void)xmlReq.AddChildElem("LanguageType");
	(void)xmlReq.IntoElem();
	(void)xmlReq.SetElemValue(pLanguageType);
	xmlReq.OutOfElem();
	xmlReq.OutOfElem();

	unsigned int len = 0;
	const char* pReq = xmlReq.GetXMLStream(len);
	CHECK_POINTER(pReq, IVS_OPERATE_MEMORY_ERROR);

	// 构造带域的请求消息，并发送
	CSendNssMsgInfo sendNssMsgInfo;
	sendNssMsgInfo.SetNeedXml(TYPE_NSS_XML);
	sendNssMsgInfo.SetNetElemType(NET_ELE_SMU_NSS);
	sendNssMsgInfo.SetReqID(NSS_GET_STATEMENT_REQ);
	sendNssMsgInfo.SetReqData(pReq);	

	std::string strRsp;
	IVS_INT32 iNeedRedirect = IVS_FAIL;
	CHECK_POINTER(m_pUserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_INT32 iRet = m_pUserMgr->SendCmd(sendNssMsgInfo,strRsp,iNeedRedirect);
	if(IVS_SUCCEED != iRet)
	{
		BP_RUN_LOG_ERR(iRet, "get statement","SendCmd to SMU Return Failed");
		return  iRet;
	}
	
	// 解析xml
	CXml xmlRsp;
	if (!xmlRsp.Parse(strRsp.c_str()))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "parse xml failed","NA");
		return  IVS_SMU_XML_INVALID;
	}

	if (!xmlRsp.FindElemEx("Content/StatementInfo/Statement"))
	{
		BP_RUN_LOG_ERR(IVS_SMU_XML_INVALID, "cannot find Statement","NA");
		return  IVS_SMU_XML_INVALID;
	}
	const char* pStatementValue = xmlRsp.GetElemValue();
	CHECK_POINTER(pStatementValue, IVS_OPERATE_MEMORY_ERROR);
	char* pCopy = IVS_NEW(pCopy, strlen(pStatementValue) + 1); // 在外部释放掉
	memset(pCopy, 0, strlen(pStatementValue) + 1);
	if (!CToolsHelp::Memcpy(pCopy, strlen(pStatementValue) + 1, pStatementValue, strlen(pStatementValue)))
	{
		BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "CToolsHelp::Memcpy pRsp to pResultXml fail", "NA");
		return IVS_ALLOC_MEMORY_ERROR;
	}
	*pStatement = pCopy;
	return IVS_SUCCEED;
}

