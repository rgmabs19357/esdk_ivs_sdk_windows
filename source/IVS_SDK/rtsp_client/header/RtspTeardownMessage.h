/*
 *  RtspTeardownMessage.h
 *
 *  Created on: 2011-10-12
 *      Author: s00201096
 */

#ifndef __RTSPTEARDOWNMESSAGE_H_
#define __RTSPTEARDOWNMESSAGE_H_

#include <string>
#include "RtspMessage.h"

class CRtspTeardownMessage : public CRtspMessage
{
public:
    CRtspTeardownMessage();
    virtual ~CRtspTeardownMessage();

    int encodeMessage(std::string &strMessage);
};

#endif  //__RTSPTEARDOWNMESSAGE_H_
