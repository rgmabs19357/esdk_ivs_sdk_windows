#pragma once
#include "decoderchannel.h"

class CRDAChannel :
	public CDecoderChannel
{
public:
	CRDAChannel(IVS_ULONG ulAsyncSeq, 
				const IVS_CHAR* pDecoderID, 
				IVS_UINT32 uiChannel,
				CTVWallMgr* pTVWallMgr);
	~CRDAChannel(void);
protected:
	// 初始化通道
	virtual IVS_INT32 InitChannel(IVS_UINT32 uiProtocolType, MEDIA_ADDR& stDecoderAddr);
	// 启动通道开始解码
	virtual IVS_INT32 StartChannel(const std::string strURL, const MEDIA_ADDR stSourceMediaAddr);
	// 结束通道解码
	virtual IVS_INT32 StopChannel();
	// 开启通道声音
	virtual IVS_INT32 StartChannelSound();
	// 停止通道声音
	virtual IVS_INT32 StopChannelSound();
	// 设置通道声音音量
	virtual IVS_INT32 SetChannelVolume(IVS_UINT32 /*uiVolumeValue*/);
private:
	CRDAChannel();
	IVS_ULONG m_ulBusinessID;				// 解码器业务ID
	IVS_UCHAR m_ucTransportProtocol;		// 解码器使用的协议
};
