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
    uint8_t *content_type;
    uint16_t content_length;
    uint8_t *date;
    uint16_t expires;
    uint8_t max_forward;
    uint8_t *user_agent;
    uint8_t *allow;
    SDP_message_body message_body;
}SDP_Data;

uint8_t *SDP_packet_make(SDP_Data *data);

int main()
{

   char *request_L = 0;
   char *message_h = 0;
   char *message_b = 0;
   char *temp_message = 0;
   uint32_t temp_size = 0;
    uint8_t *packet_data;
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
    session_data.allow = "INVITE, ACK, CANCEL, BYE, REFER, OPTiONS, NOTIFY, INFO";
    session_data.address[0]=192;
    session_data.address[1]=168;
    session_data.address[2]=0;
    session_data.address[3]=125;
    session_data.expires = 120;

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
    printf("packet data[%ld]:[%s]\r\n", strlen(packet_data), packet_data);
    free(packet_data);
    printf("end program\r\n");
    
    //temp_size = sizeof(session_data.from_data.display_info) + sizeof(session_data.from_data.host_part);
    //temp_size = strlen(session_data.from_data.host_part);
    //printf("%d : display info : %s, user port : %08x \r\n",temp_size, session_data.from_data.display_info, session_data.from_data.user_part);

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

uint8_t *SDP_packet_make(SDP_Data *data)
{
    uint16_t current_size = 0, temp_size =0, body_size = 0;
    uint8_t *packet_data = 0, *body_data = 0;
    //request_size = strlen(data->method) + strlen(data->to_data.host_part) +14+8;
    //via_size = 5 + 11 + 1 + 16 + 1 +6 + 1 + strlen(data->branch) + 16 + 1 + 5 +2;
    //from_size = 6 +2 + + strlen(data->from_data.display_info) + 6 + 8 + 1 + strlen(data->from_data.host_part) + 1+ 4+6+2;
    body_data = malloc(sizeof(char) * 300);
    if(body_data == 0)
    {
        printf("body data creat error !!\r\n");
        return 0;
    }
    
    packet_data = malloc(sizeof(char) * 1500);
    if(packet_data == 0)
    {
        printf("packet data creat error !!\r\n");
        return 0;
    }
    //body make
    temp_size =sprintf((char *)body_data, "V=%d\r\n", data->message_body.version);
    //printf("vesion : %d\r\n",temp_size);
    body_size += temp_size;
    temp_size =sprintf((char *)body_data + body_size, "o=SIPPS %ld %ld IN IP4 %d.%d.%d.%d\r\n", data->message_body.session_ID, data->message_body.session_version, data->address[0],data->address[1],data->address[2],data->address[3]);
    //printf("owner/creator : %d\r\n",temp_size);
    body_size += temp_size;
    temp_size =sprintf((char *)body_data + body_size, "s=SIP call\r\nc=IN IP4 %d.%d.%d.%d\r\n", data->address[0],data->address[1],data->address[2],data->address[3]);
    //printf("session Name & connection Info : %d\r\n",temp_size);
    body_size += temp_size;
    temp_size =sprintf((char *)body_data + body_size, "t=%d %d\r\n", data->message_body.start_time, data->message_body.stop_time);
    //printf("start stop time : %d\r\n",temp_size);
    body_size += temp_size;
    temp_size =sprintf((char *)body_data + body_size, "m=%s %d %s %d\r\n", data->message_body.media_type, data->message_body.media_port, data->message_body.media_protocol, data->message_body.media_format);
    //printf("meida Description : %d\r\n",temp_size);
    body_size += temp_size;
    temp_size =sprintf((char *)body_data + body_size, "a=%s:%d %s/%d\r\n", data->message_body.media_attr_fieldname, data->message_body.media_format, data->message_body.media_attr_mime_type, data->message_body.media_attr_sample_rate);
    //printf("meida Description : %d\r\n",temp_size);
    body_size += temp_size;
    //printf("message body[ %d ]: %s[end] \r\n",body_size, body_data);
    data->content_length = body_size;
    //packet make
    temp_size =sprintf((char *)packet_data, "%s sip:%08lx@%s SIP/2.0\r\n", data->method, data->to_data.user_part, data->to_data.host_part);
    //printf("request : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "Via: SIP/2.0/UDP %d.%d.%d.%d;branch=%s%d.%d.%d.%d;rport\r\n", data->address[0],data->address[1],data->address[2],data->address[3], data->branch, data->address[0], data->address[1], data->address[2], data->address[3]);
    //printf("via : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "From: \"%s\" <sip:%08lx@%s>;tag=%06lx\r\n", data->from_data.display_info, data->from_data.user_part, data->from_data.host_part, data->from_data.tag);
    //printf("From : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "To: <sip:%08lx@%s>\r\n", data->to_data.user_part, data->to_data.host_part);
    //printf("to : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "Call-ID: %s@%d.%d.%d.%d\r\n", data->call_ID, data->address[0], data->address[1], data->address[2], data->address[3]);
    //printf("call ID : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "CSeq: %d %s\r\n", data->cseq, data->method);
    //printf("cseq : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "Content-Type: %s\r\n", data->content_type);
    //printf("Content-Type : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "Content-Length: %d\r\n", data->content_length);
    //printf("Content-length : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "Date: Mon 07 june 2022 16:56:05  GMT con\r\n");
    //printf("Date : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "Contact: <sip:%08lx@%d.%d.%d.%d\r\n", data->from_data.user_part, data->address[0], data->address[1], data->address[2], data->address[3]);
    //printf("Contact : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "Expires: %d\r\nAccept: %s", data->expires, data->content_type);
    //printf("Expires & Accept : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "Max-Forwards: %d\r\nUser-Agent: %s\r\nAllow: %s\r\n", data->max_forward,data->user_agent, data->allow); 
    //printf("max forwards, User_agent, Allow : %d\r\n",temp_size);
    current_size += temp_size;
    //printf("packet[%d]=%s[end]\r\n", current_size, packet_data);
    //printf("cal size : %d, gen size %d \r\n data: [%s]\r\n", request_size, res_size, request_data);
    memcpy(packet_data+current_size, body_data, sizeof(char)*body_size);
    free(body_data);
    //free(res_data1);
    //return 0;
    return packet_data;
}