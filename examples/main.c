/**
 * Copyright (c) 2021 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
  * ----------------------------------------------------------------------------------------------------
  * Includes
  * ----------------------------------------------------------------------------------------------------
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "port_common.h"
/*
#include "wizchip_conf.h"
#include "w5x00_spi.h"
#include "socket.h"
#include "loopback.h"
*/
#include "wizchip_conf.h"
#include "w5x00_spi.h"


#include "dhcp.h"
#include "dns.h"
#include "timer.h"

#include "netif.h"

#include "azure_samples.h"

#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "pico/binary_info.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/util/datetime.h"
#include "hardware/rtc.h"



#include "rtp.h"
#include "sdp_proto.h"
#include "g711.h"
#include "temp_eth_lib.h"

/**
  * ----------------------------------------------------------------------------------------------------
  * Macros
  * ----------------------------------------------------------------------------------------------------
  */
// The application you wish to use should be uncommented
//
//#define APP_TELEMETRY
#define APP_C2D
//#define APP_CLI_X509
//#define APP_PROV_X509
#define TCP_S_SOCKET 0
#define TCP_S_PORT 20000
#define UDP_SOCKET 1
#define UDP_PORT 30000
#define UDP_SPORT 30002//40392//30002
#define TCP_C_SOCKET 2


//adc define
#define ADC_NUM 0
#define ADC_PIN (26 + ADC_NUM)
#define ADC_VREF 3.3
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1))
#define ADC_CLK_VAL  5999//2999 
#define ADC_SAMPLE_COUNT   100
#define DATACNT 2000
/**
  * ----------------------------------------------------------------------------------------------------
  * Variables
  * ----------------------------------------------------------------------------------------------------
  */
// The application you wish to use DHCP mode should be uncommented
//#define _DHCP
static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x68}, // MAC address
        .ip = {192, 168, 15, 125},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 0, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
#ifdef _DHCP
        .dhcp = NETINFO_DHCP // DHCP enable/disable
#else
        // this example uses static IP
        .dhcp = NETINFO_STATIC
#endif
};

typedef struct TCP_S_RSV_DATA_1
{
	uint8_t DestIP[4];
	uint16_t Port;
	uint16_t RCV_Size;
	uint8_t *RCV_DATA;
}TCP_S_RSV_DATA;

typedef struct ADC_DATA_STATUS_t
{
    uint8_t sendPosition;
    uint8_t sendFlag;
    uint8_t sendComplete;
    uint8_t inputPosition;
    uint16_t dataCount;
    uint8_t sendStatus;
    uint8_t DATA[2][2048];
}ADC_DATA_STATUS;

typedef union h2f_var_t
{
    uint32_t hdata;
    float fdata;
}h2f_var;

/* Timer */
static uint16_t g_msec_cnt = 0;

ADC_DATA_STATUS adc_data;

/**
  * ----------------------------------------------------------------------------------------------------
  * Functions
  * ----------------------------------------------------------------------------------------------------
  */
/* Timer callback */
static void repeating_timer_callback(void);

//rtp function
static StatusCode RtpGetRandomCb(uint32_t *random);
void TxRtpSetup();


/**
  * ----------------------------------------------------------------------------------------------------
  * Main
  * ----------------------------------------------------------------------------------------------------
  */
int main()
{
//-----------------------------------------------------------------------------------
// Pico board configuration - W5100S, GPIO, Timer Setting
//-----------------------------------------------------------------------------------
    int8_t networkip_setting = 0;

    int8_t UDP_buff[1048];
    uint16_t TCP_S_status = 0;
    uint16_t UDP_S_status = 0;
    //TCP_S_RSV_DATA *TCP_Server_Buf = 0;
    uint8_t *tcp_rcv_data = 0;
    uint16_t tcp_rcv_size = 0;
    uint8_t UDP_BroadIP[4] = {255,255,255,255};
    uint8_t UDP_TestIP[4] = {192,168,15,2};
    int32_t UDP_ret = 0;
    uint8_t TCP_Client_DestIp[4] = {192, 168, 15, 2};
    uint16_t TCP_Client_Port = 22000;
    uint16_t TCP_C_status = 0;
    uint8_t *tcp_c_rcv_data = 0;
    uint16_t tcp_c_rcv_size = 0;
    int tcp_c_ret = 0;
    uint16_t udp_send_port = 30001;

    //int8_t mic_Data[2048];
    //int8_t mic_Data[1000];
    
    //uint32_t mic_cnt = 0;
    uint8_t data_send_status = 0;
    uint32_t send_count = 0;
    
    uint16_t adc_raw = 0;
    uint16_t adc_raw1 = 0;
    uint32_t i= 0;
    uint16_t random_data = 0;
    uint64_t pre_time_stamp = 0;
    uint64_t now_time_stamp = 0;
    ;
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
    session_data.address[2]=15;
    session_data.address[3]=125;
    session_data.expires = 120;

    session_data.message_body.version = 0;
    session_data.message_body.owner_username = "WIZNET";
    session_data.message_body.session_information ="the session test protocol of wiznet";
    session_data.message_body.session_ID = 24466424;
    session_data.message_body.session_version = 24466418;
    session_data.message_body.address_type = "IP4";
    session_data.message_body.network_type = "IN";
    session_data.message_body.start_time = 0;
    session_data.message_body.stop_time = 0;
    session_data.message_body.media_type = "audio";
    session_data.message_body.media_port = UDP_PORT;
    session_data.message_body.media_protocol = "RTP/AVP";
    session_data.message_body.session_name = "WIZNET Test";
    session_data.message_body.media_format = 8;//11;//0;//11;
    session_data.message_body.media_attr_fieldname = "rtpmap";
    session_data.message_body.media_attr_mime_type = "PCMA";//"PCMU";//"L16";
    session_data.message_body.media_attr_sample_rate = 8000;//16000;
    session_data.message_body.fmtp_type = 78;
    
    datetime_t t = {
            .year  = 2022,
            .month = 05,
            .day   = 24,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 11,
            .min   = 40,
            .sec   = 00
    };
    
    memset(&adc_data, 0, sizeof(ADC_DATA_STATUS));
    adc_data.sendComplete = 1;
    stdio_init_all();

    wizchip_delay_ms(1000 * 3); // wait for 3 seconds

    wizchip_spi_initialize();
    wizchip_cris_initialize();

    wizchip_reset();
    wizchip_initialize();
    wizchip_check();

    wizchip_1ms_timer_initialize(repeating_timer_callback);

    random_data = rand() >> 16;
    //adc init
    bi_decl(bi_program_description("Analog microphone example for Raspberry Pi Pico")); // for picotool
    bi_decl(bi_1pin_with_name(ADC_PIN, "ADC input pin"));
    sleep_ms(1000);
    printf("Starting Program\n");
    adc_init();
    adc_gpio_init( ADC_PIN);
    adc_select_input( ADC_NUM);
    //sleep_ms(1000);
    printf("Ready ADC Program\n");
    //adc fifo 
    
    adc_fifo_setup(
        true,    // Write each completed conversion to the sample FIFO
        true,    // Enable DMA data request (DREQ)
        1,       // DREQ (and IRQ) asserted when at least 1 sample present
        true,   // We won't see the ERR bit because of 8 bit reads; disable.
        false     // Shift each sample to 8 bits when pushing to FIFO
    );
    adc_set_clkdiv(ADC_CLK_VAL);//2999= 16kS/s  (1+999)/48Mhz = 48kS/s   199=240kS/s  239=200kS/s 1087=44118S/s
    sleep_ms(1000);
    printf("Starting capture\n");
    adc_run(true);

    // Start the RTC
    rtc_init();
    rtc_set_datetime(&t);

    //rtp setup
    printf("send RTP IP %d.%d.%d.%d port %d \r\n",UDP_BroadIP[0],UDP_BroadIP[1],UDP_BroadIP[2],UDP_BroadIP[3],UDP_SPORT);
#ifdef _DHCP
    // this example uses DHCP
    networkip_setting = wizchip_network_initialize(true, &g_net_info);
#else
    // this example uses static IP
    networkip_setting = wizchip_network_initialize(false, &g_net_info);
#endif

    // bool cancelled = cancel_repeating_timer(&timer);
    // printf("cancelled... %d\n", cancelled);
    if (networkip_setting)
    {
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// CALL Main Funcion - Azure IoT SDK example funcion
// Select one application.
//-----------------------------------------------------------------------------------
#if 0
#ifdef APP_TELEMETRY
        iothub_ll_telemetry_sample();
#endif // APP_TELEMETRY
#ifdef APP_C2D
        iothub_ll_c2d_sample();
#endif // APP_C2D
#ifdef APP_CLI_X509
        iothub_ll_client_x509_sample();
#endif // APP_CLI_X509
#ifdef APP_PROV_X509
        prov_dev_client_ll_sample();
#endif  // APP_PROV_X509
#endif

//-----------------------------------------------------------------------------------
    }
    else
        printf(" Check your network setting.\n");

    //adc test
    SDP_init(&session_data, UDP_SOCKET, UDP_BroadIP, UDP_SPORT);
    adc_func_set(adc_fifo_get_blocking);
    /* Infinite loop */
    for (;;)
    {
#ifdef _DHCP
        if (0 > wizchip_dhcp_run())
        {
            printf(" Stop Example.\n");

            while (1)
                ;
        }
#endif
        #if 1
        if(data_send_status == 1)
        {
            UDP_ret=RTP_Data_Send();
            now_time_stamp = time_us_64();
            printf("data send : %d[%lld]\r\n",UDP_ret, now_time_stamp - pre_time_stamp);
            get_time_stamp(now_time_stamp - pre_time_stamp);
            pre_time_stamp = now_time_stamp;
        }
        #endif
        UDP_S_status = udps_status(UDP_SOCKET, UDP_buff, UDP_PORT);
        TCP_S_status = TCP_Server(TCP_S_SOCKET, TCP_S_PORT);
        if(TCP_S_status == 17)
        {
            tcp_rcv_data = TCP_S_Recv(TCP_S_SOCKET, &tcp_rcv_size);
            if(tcp_rcv_data != 0)
            {
                printf("rcv[%d] : %s \r\n",tcp_rcv_size, tcp_rcv_data);
                if(UDP_S_status == 0x22)
                {
                    if(UDP_ret < 0)
                    {
                        printf("sendto Error \r\n");
                    }
                }
                if(strncmp(tcp_rcv_data, "start", 5) == 0)
                {
                    printf("data send start\r\n");
                    printf("recv port = %d \r\n", UDP_SPORT);
                    send(TCP_S_SOCKET, "START RTP Streaming\r\n", strlen("START RTP Streaming\r\n"));
                    adc_run(true);
                    adc_fifo_drain();
                    data_send_status = 1;
                    rtc_get_datetime(&t);
                    get_date_data(&t);
                    SDP_INVITE_Send();
                    adc_data.sendStatus = 1;
                }
                else if(strncmp(tcp_rcv_data, "stop", 4) == 0)
                {
                    printf("data send stop \r\n");
                    data_send_status = 0;
                    //adc_data.sendStatus = 0;
                    adc_run(false);
                    send(TCP_S_SOCKET, "STOP RTP Streaming\r\n", strlen("STOP RTP Streaming\r\n"));
                    rtc_get_datetime(&t);
                    get_date_data(&t);
                    SDP_BYE_Send();
                }
                free(tcp_rcv_data);
            }
        }
    }
}

/**
  * ----------------------------------------------------------------------------------------------------
  * Functions
  * ----------------------------------------------------------------------------------------------------
  */
/* Timer callback */
static void repeating_timer_callback(void)
{
    g_msec_cnt++;

#ifdef _DHCP
    if (g_msec_cnt >= 1000 - 1)
    {
        g_msec_cnt = 0;

        wizchip_dhcp_time_handler();
    }
#endif
}


