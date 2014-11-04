/*
 *  RtspPauseMessage.h
 *
 *  Created on: 2011-10-11
 *      Author: s00201096
 */

#ifndef __RTSPPAUSEMESSAGE_H_
#define __RTSPPAUSEMESSAGE_H_

#include "RtspMessage.h"

class CRtspPauseMessage : public CRtspMessage
{
public:
    CRtspPauseMessage();
    virtual ~CRtspPauseMessage();

    int encodeMessage(std::string &strMessage);

};

#endif  //__RTSPPAUSEMESSAGE_H_
