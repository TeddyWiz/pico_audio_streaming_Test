#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned char uint8_t;
#define uint32_t unsigned long
#define uint16_t unsigned short

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
    uint8_t content_type;
    uint16_t content_length;
    uint8_t *date;
    uint16_t expires;
    uint8_t max_forward;
    uint8_t *user_agent;
    uint8_t *allow;
    SDP_message_body message_body;
}SDP_Data;

char *SDP_packet_make(SDP_Data *data);

int main()
{

   char *request_L = 0;
   char *message_h = 0;
   char *message_b = 0;
   char *temp_message = 0;
   uint32_t temp_size = 0;
    char *packet_data;
    SDP_Data session_data;
    session_data.from_data.display_info = "wiznet";
    session_data.from_data.user_part = 0x3796CB71;
    session_data.from_data.host_part = "sip.wiznet.io";
    session_data.from_data.tag = 0xb56e6e;

    session_data.to_data.user_part = 0x35104723;
    session_data.to_data.host_part = "sip.wiznet.io";
    session_data.method = "INVITE";
    session_data.branch = "z9hG4bKnp10144774-4725f980";
    session_data.rport = "rport";
    session_data.call_ID = "11894297-4432a9f8";
    session_data.cseq = 1;
    session_data.content_type = "application/sdp";
    //session_data.content_length = 270;
    session_data.date = "WED 25 May 2022 10:54:25 GMT";
    session_data.max_forward = 70;
    session_data.user_agent = "wiznet Version 0.0.0.0";
    session_data.address[0]=192;
    session_data.address[1]=168;
    session_data.address[2]=0;
    session_data.address[3]=125;

    session_data.message_body.version = 0;
    session_data.message_body.owner_username = "wiznet";
    session_data.message_body.session_ID = 24466422;
    session_data.message_body.session_version = 24466418;
    session_data.message_body.address_type = "IP4";
    session_data.message_body.network_type = "IN";
    session_data.message_body.start_time = 0;
    session_data.message_body.stop_time = 0;
    session_data.message_body.media_type = "audio";
    session_data.message_body.media_port = 30000;
    session_data.message_body.media_protocol = "RTP/AVP";
    session_data.message_body.session_name = "SIP call";
    session_data.message_body.media_format = 0;
    session_data.message_body.media_attr_fieldname = "rtpmap";
    session_data.message_body.media_attr_mime_type = "L16";
    session_data.message_body.media_attr_sample_rate = 8000;

    packet_data = SDP_packet_make(&session_data);
    
    
    //temp_size = sizeof(session_data.from_data.display_info) + sizeof(session_data.from_data.host_part);
    temp_size = strlen(session_data.from_data.host_part);
    printf("%d : display info : %s, user port : %08x \r\n",temp_size, session_data.from_data.display_info, session_data.from_data.user_part);

    #if 0
   request_L = calloc(62, sizeof(char));
   memcpy(request_L, "INVITE sip:3796CB71@sip.cybercity.dk SIP/2.0\r\n", sizeof("INVITE sip:3796CB71@sip.cybercity.dk SIP/2.0\r\n"));
   message_h = calloc(756, sizeof(char));
   memcpy(message_h, "Via: SIP/2.0/UDP 192.168.0.125;branch=z9hG4bKnp10144774-4725f980192.168.0.125;rport\r\n", sizeof("Via: SIP/2.0/UDP 192.168.0.125;branch=z9hG4bKnp10144774-4725f980192.168.0.125;rport\r\n"));
   *(message_h + sizeof("Via: SIP/2.0/UDP 192.168.0.125;branch=z9hG4bKnp10144774-4725f980192.168.0.125;rport\r\n") + 1) = 0;
   temp.request_line = request_L;
   temp.message_header = message_h;
   temp_message = request_L;
   temp_message + 62 = message_h;
   printf("temp %08x, %08x, %08x \r\n", temp, temp.request_line, temp.message_header);
   printf("request = [%s]\r\n", temp.request_line);
   printf("header = [%s]\r\n", temp.message_header);
   printf("temp message [%s]\r\n", temp_message);
   printf("data = [%s] \r\n", (char*)&temp);
   #endif
   return 0;
} 

char *SDP_packet_make(SDP_Data *data)
{
    uint16_t packet_size = 0;
    packet_size = strlen(data->method) + strlen(data->)
    return 0;
}