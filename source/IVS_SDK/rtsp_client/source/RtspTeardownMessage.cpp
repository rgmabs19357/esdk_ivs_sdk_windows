/*
 *  RtspTeardownMessage.cpp
 *
 *  Created on: 2011-10-12
 *      Author: s00201096
 */
#include "Rtsp_Log.h"
#include "RtspTeardownMessage.h"
#include "RtspRetCode.h"

CRtspTeardownMessage::CRtspTeardownMessage()
{
    m_unRtspMethod = RTSP_METHOD_TEARDOWN;
}

CRtspTeardownMessage::~CRtspTeardownMessage()
{

}

int CRtspTeardownMessage::encodeMessage(std::string &strMessage)
{
    strMessage.clear();
    int nRet = encodeUrl(strMessage);
    if (RET_OK != nRet)
    {
        IVS_RUN_LOG_CRI("encode teardown url fail!");
        return RET_FAIL;
    }
    encodeCSeq(strMessage);

    strMessage += RTSP_TOKEN_STR_SESSION;
    strMessage += "1";
    strMessage += RTSP_MSG_END_TAG;

    return RET_OK;
}
