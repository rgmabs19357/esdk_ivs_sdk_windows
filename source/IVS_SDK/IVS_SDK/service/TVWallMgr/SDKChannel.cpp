#include "SDKChannel.h"

CSDKChannel::CSDKChannel(IVS_ULONG ulAsyncSeq, 
						const IVS_CHAR* pDecoderID, 
						IVS_UINT32 uiChannel,
						CTVWallMgr* pTVWallMgr) : 
						CDecoderChannel(ulAsyncSeq, pDecoderID, uiChannel, pTVWallMgr)
{

}
CSDKChannel::CSDKChannel()
{

}
CSDKChannel::~CSDKChannel(void)
{

}
