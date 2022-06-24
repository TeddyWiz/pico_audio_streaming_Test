
#ifndef _SDP_PROTO_H
#define _SDP_PROTO_H

#include "pico/util/datetime.h"

typedef uint16_t (*adc_input_func)(void);

typedef struct Header_address_Part_t
{
    char *display_info;
    uint32_t user_part;
    char *host_part;
    uint32_t tag;
}Header_address_Part;

typedef struct SDP_message_body_t
{
    uint8_t version;
    uint8_t *owner_username;
    uint32_t session_ID;
    uint32_t session_version;
    uint8_t *session_name;
    uint8_t *session_information;
    uint8_t *network_type;
    uint8_t *address_type;
    //uint8_t address[4];
    uint16_t start_time;
    uint16_t stop_time;
    uint8_t *media_type;
    uint16_t media_port;
    uint8_t *media_protocol;
    uint8_t media_format;
    uint8_t *media_attr_fieldname;
    uint8_t *media_attr_mime_type;
    uint16_t media_attr_sample_rate;
    uint8_t fmtp_type;
}SDP_message_body;

typedef struct SDP_Data_t
{
    uint8_t address[4];
    uint8_t *branch;
    uint8_t *rport;
    uint8_t *method;
    Header_address_Part from_data;
    Header_address_Part to_data;
    uint8_t *call_ID;    //call-ID+@sent address
    uint16_t cseq;
    //NC Proxy-Authorization
    uint8_t *content_type;
    uint16_t content_length;
    uint8_t *date;
    uint16_t expires;
    uint8_t max_forward;
    uint8_t *user_agent;
    uint8_t *allow;
    SDP_message_body message_body;
}SDP_Data;

void adc_func_set(adc_input_func func);
uint8_t RTP_Data_Send(void);
//void get_now_SDP_time(uint64_t now_time);
void get_date_data(datetime_t *indate);
void SDP_init(SDP_Data *SDP_data,uint8_t sn, uint8_t *addr, uint16_t port);

uint8_t *SDP_packet_make(SDP_Data *data);
uint8_t SDP_INVITE_Send(void);
uint8_t SDP_BYE_Send(void);

#endif

