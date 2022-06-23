#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sdp_proto.h"

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
    temp_size =sprintf((char *)body_data, "v=%d\r\n", data->message_body.version);
    //printf("vesion : %d\r\n",temp_size);
    body_size += temp_size;
    //temp_size =sprintf((char *)body_data + body_size, "o=SIPPS %ld %ld IN IP4 %d.%d.%d.%d\r\n", data->message_body.session_ID, data->message_body.session_version, data->address[0],data->address[1],data->address[2],data->address[3]);
    temp_size =sprintf((char *)body_data + body_size, "o=%s %ld %ld IN IP4 %d.%d.%d.%d\r\n",data->message_body.owner_username, data->message_body.session_ID, data->message_body.session_version, data->address[0],data->address[1],data->address[2],data->address[3]);
    //printf("owner/creator : %d\r\n",temp_size);
    body_size += temp_size;
    temp_size =sprintf((char *)body_data + body_size, "s=%s\r\nc=IN IP4 %d.%d.%d.%d\r\n",data->message_body.session_name, data->address[0],data->address[1],data->address[2],data->address[3]);
    //printf("session Name & connection Info : %d\r\n",temp_size); the session test protocol of wiznet 
    body_size += temp_size;
    temp_size =sprintf((char *)body_data + body_size, "i=%s\r\n", data->message_body.session_information); 
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
    temp_size =sprintf((char *)body_data + body_size, "a=fmtp:%d %d IN IP4 %d.%d.%d.%d/127\r\n",data->message_body.fmtp_type, data->message_body.media_port, data->address[0],data->address[1],data->address[2],data->address[3]);
    //body_size += temp_size;
    ///temp_size =sprintf((char *)body_data + body_size, "a=sendrecv\r\n");
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
    temp_size =sprintf((char *)(packet_data + current_size), "Date: Wed 22 june 2022 16:37:05  GMT\r\n");
    //printf("Date : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "Contact: <sip:%08lx@%d.%d.%d.%d>\r\n", data->from_data.user_part, data->address[0], data->address[1], data->address[2], data->address[3]);
    //printf("Contact : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "Expires: %d\r\nAccept: %s", data->expires, data->content_type);
    //printf("Expires & Accept : %d\r\n",temp_size);
    current_size += temp_size;
    temp_size =sprintf((char *)(packet_data + current_size), "Max-Forwards: %d\r\nUser-Agent: %s\r\nAllow: %s\r\n\r\n", data->max_forward,data->user_agent, data->allow); 
    //printf("max forwards, User_agent, Allow : %d\r\n",temp_size);
    current_size += temp_size;
    
    //printf("packet[%d]=%s[end]\r\n", current_size, packet_data);
    //printf("cal size : %d, gen size %d \r\n data: [%s]\r\n", request_size, res_size, request_data);
    printf("packet cnt %d + %d =%d\r\n", current_size, body_size, current_size + body_data);
    memcpy(packet_data+current_size, body_data, sizeof(char)*body_size);
    *(packet_data + current_size + body_size) = 0;
    free(body_data);
    //free(res_data1);
    //return 0;
    return packet_data;
}

