/*******************************************************************************
* Copyright (c) 2017, Alan Barr
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice, this
*   list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef __RTP_H__
#define __RTP_H__

#include <stdint.h>
//#include "debug.h"

#define RTP_HEADER_LENGTH       12

typedef enum 
{
    /* Success */
    STATUS_OK,
    /* Argument provided to API was bad */
    STATUS_ERROR_API,
    /* Callback failed */
    STATUS_ERROR_CALLBACK,
    /* Internal error */
    STATUS_ERROR_INTERNAL,
    /* Some form of system input was out of expected range. */
    STATUS_ERROR_EXTERNAL_INPUT,
    /* OS returned Error */
    STATUS_ERROR_OS,
    /* Library returned error */
    STATUS_ERROR_LIBRARY,
    /* LWIP Library returned error */
    STATUS_ERROR_LIBRARY_LWIP,
    /* Error interfacing with HW */
    STATUS_ERROR_HW,
    /* Unexpected timeout */
    STATUS_ERROR_TIMEOUT,
    /* Placeholder for bounds checking */
    STATUS_CODE_ENUM_MAX
} StatusCode;


typedef StatusCode (*rtpGetRandom)(uint32_t *random);

typedef struct {
    /* Value to increment the period time stamp by per RTP packet 
     * transmitted */
    uint32_t periodicTimestampIncr;
    /* Payload type of RTP packet */
    uint8_t payloadType;
    /* Callback to obtain random numbers */
    rtpGetRandom getRandomCb;
} rtpConfig;


/******************************************************************************/

StatusCode rtpInit(const rtpConfig *config);
uint32_t get_ssrc(void);
StatusCode rtpShutdown(void);
StatusCode rtpAddHeader(uint8_t *data,
                       uint32_t length);

#endif /* Header Guard */
