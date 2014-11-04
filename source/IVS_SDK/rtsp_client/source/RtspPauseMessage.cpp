/*
 *  RtspPauseMessage.cpp
 *
 *  Created on: 2011-10-11
 *      Author: s00201096
 */
#include <sstream>
#include "RtspRetCode.h"
#include "RtspPauseMessage.h"

CRtspPauseMessage::CRtspPauseMessage()
{
    m_unRtspMethod = RTSP_METHOD_PAUSE;
}

CRtspPauseMessage::~CRtspPauseMessage()
{

}

// ¸²Ð´PauseµÄ·â×°º¯Êý
int CRtspPauseMessage::encodeMessage(std::string &strMessage)
{
    strMessage.clear();
    if (RET_OK != encodeUrl(strMessage))
    {
        return RET_FAIL;
    }
    encodeCSeq(strMessage);
    strMessage += RTSP_TOKEN_STR_SESSION;
    strMessage += "1";
    strMessage += RTSP_END_TAG;
    strMessage += RTSP_END_TAG;
    
    return RET_OK;
}
