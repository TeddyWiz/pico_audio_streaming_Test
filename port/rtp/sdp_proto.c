#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sdp_proto.h"
#include "socket.h"
#include "g711.h"
#include "rtp.h"


#define ADC_SAMPLE_COUNT   100



SDP_Data *SDP_session_data;
uint8_t send_socket;
uint8_t *send_addr;
uint16_t send_port;
adc_input_func get_adc_func = NULL;
char Month_list[12][4] = {"Jan","Feb","Mar","Apr", "May", "Jun", "jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char Week_list[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
datetime_t *send_time;
uint8_t *rtp_data = NULL;
uint8_t *rtp_data_current = NULL;



static StatusCode RtpGetRandomCb(uint32_t *random)
{
    *random = (uint32_t)(rand());//-0x80000000;

    return STATUS_OK;
}
void TxRtpSetup()
{
    rtpConfig config;

    memset(&config, 0, sizeof(config));

    config.payloadType = 8;//0;//11;  //8 = G.711 PCMA  11 = L16
    config.getRandomCb = RtpGetRandomCb;
    config.periodicTimestampIncr = (8000  * 8 ) / 1000 * 12.5;//16000 / 1000 * 6.6;//20; //HZ * bit * sec

    if(STATUS_OK != rtpInit(&config))
    {
        printf("RTP Init Failed",0);
    }
}

void adc_func_set(adc_input_func func)
{
    get_adc_func = func;
}
#if 0
void get_now_SDP_time(uint64_t now_time)
{
    now_time_stamp = now_time;
}
#endif

uint8_t RTP_Data_Send(void)
{
    uint16_t adc_raw = 0, adc_raw1 = 0, i;
    uint8_t UDP_ret;
    rtp_data_current = rtp_data + 12;
    for(i= 0; i<ADC_SAMPLE_COUNT; i++)
    {
        adc_raw = get_adc_func();
        adc_raw1 = (adc_raw&0x0fff) - (1<<10);
        *rtp_data_current++ = ALaw_Encode(adc_raw1);
    }
    if (STATUS_OK != rtpAddHeader(rtp_data,12))
    {
        printf("Rtp Add Header failed");
    }
    UDP_ret=sendto(send_socket, rtp_data, ADC_SAMPLE_COUNT + 12, send_addr, send_port);
    return UDP_ret;
}
void get_date_data(datetime_t *indate)
{
    send_time = indate;
}
void SDP_init(SDP_Data *SDP_data,uint8_t sn, uint8_t *addr, uint16_t port)
{
    SDP_session_data = SDP_data;
    send_addr = addr;
    send_port = port;
    send_socket = sn;
    rtp_data = (uint8_t *)calloc(200, sizeof(uint8_t));
    if(rtp_data == 0)
    {
        printf("craet buff error \r\n");
    }
    TxRtpSetup();
}

uint8_t SDP_INVITE_Send(void)
{
    uint8_t *packet_data;
    uint8_t UDP_ret;
    SDP_session_data->method = "INVITE";
    SDP_session_data->to_data.user_part++;
    packet_data = SDP_packet_make(SDP_session_data);
    printf("packet data[%ld]:[%s]\r\n", strlen(packet_data), packet_data);
    //UDP_ret = sendto(UDP_SOCKET, Temp_invite, 1056, UDP_BroadIP, UDP_SPORT);
    UDP_ret = sendto(send_socket, packet_data, strlen(packet_data), send_addr, send_port);
    free(packet_data);
    printf("send invice packet %d\r\n", UDP_ret);
}

uint8_t SDP_BYE_Send(void)
{
    uint8_t *packet_data;
    uint8_t UDP_ret;
    SDP_session_data->method = "BYE";
    packet_data = SDP_packet_make(SDP_session_data);
    printf("packet data[%ld]:[%s]\r\n", strlen(packet_data), packet_data);
    //UDP_ret = sendto(UDP_SOCKET, Temp_invite, 1049, UDP_BroadIP, UDP_SPORT);
    UDP_ret = sendto(send_socket, packet_data, strlen(packet_data), send_addr, send_port);
    free(packet_data);
    printf("send bye packet %d\r\n", UDP_ret);
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
    //temp_size =sprintf((char *)(packet_data + current_size), "Date: Wed 22 june 2022 16:37:05  GMT\r\n");
    temp_size =sprintf((char *)(packet_data + current_size), "Date: %s %02d %s %d %02d:%02d:%02d GMT\r\n", Week_list[send_time->dotw], (int)send_time->day, Month_list[send_time->month], send_time->year, (int)send_time->hour, (int)send_time->min, (int)send_time->sec );
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


//"Date: Wed 22 june 2022 16:37:05  GMT\r\n"
char *make_date_str(int year, char mon, char day, char week, char hour, char min, char sec)
{
   char *temp_data = NULL;
   char res = 0;
   temp_data = calloc(37, sizeof(char));
   if(temp_data == NULL)
   {
      printf("addres Error!!\r\n");
      return 0;
   }
   res = sprintf(temp_data, "Date: %s %02d %s %d %02d:%02d:%02d GMT\r\n", Week_list[week], (int)day, Month_list[mon], year, (int)hour, (int)min, (int)sec );
   if(res <= 0)
   {
      printf("make Error!\r\n");
      free(temp_data);
      return 0;
   }
   return temp_data;
}


