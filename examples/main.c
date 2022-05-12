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

#include "wizchip_conf.h"
#include "w5x00_spi.h"
#include "socket.h"
#include "loopback.h"

#include "dhcp.h"
#include "dns.h"
#include "timer.h"

#include "netif.h"

#include "azure_samples.h"

#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "pico/binary_info.h"

#include "pico/multicore.h"


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

#define TCP_S_DEBUG_
#define TCP_S_SOCKET 0
#define TCP_S_PORT 20000
#define UDP_SOCKET 1
#define UDP_PORT 30000
#define UDP_SPORT 30001
#define TCP_C_SOCKET 2


//adc define
#define ADC_NUM 0
#define ADC_PIN (26 + ADC_NUM)
#define ADC_VREF 3.3
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1))

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
        .ip = {192, 168, 0, 125},                     // IP address
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

uint16_t TCP_Server(uint8_t sn, uint16_t port);
uint16_t TCP_client(uint8_t sn, uint8_t* destip, uint16_t destport);

//TCP_S_RSV_DATA *TCP_S_Recv(uint8_t sn);
//uint8_t *TCP_S_Recv(uint8_t sn);
uint8_t *TCP_S_Recv(uint8_t sn, uint16_t *rcv_size);
int32_t udps_status(uint8_t sn, uint8_t* buf, uint16_t port);


void core1_entry() {
    uint16_t adc_raw = 0;
    
    adc_init();
    adc_gpio_init( ADC_PIN);
    adc_select_input( ADC_NUM);
    //sleep_ms(1000);
    
    //adc fifo 
    
    adc_fifo_setup(
        true,    // Write each completed conversion to the sample FIFO
        true,    // Enable DMA data request (DREQ)
        1,       // DREQ (and IRQ) asserted when at least 1 sample present
        true,   // We won't see the ERR bit because of 8 bit reads; disable.
        false     // Shift each sample to 8 bits when pushing to FIFO
    );
    //adc_set_clkdiv(999);  // (1+999)/48Mhz = 48kS/s
    
    printf("Starting capture\n");
    adc_run(true);
    while(1)
    {
        if(adc_data.sendStatus == 0)
        {
            if(adc_data.dataCount >= 2048)
            {
                if(adc_data.sendComplete == 1)
                {
                    adc_data.inputPosition =  adc_data.inputPosition == 0 ? 1 : 0;
                    adc_data.dataCount = 0;
                }
                //else
                //    continue;
                
            }
            adc_raw = adc_fifo_get_blocking();
            adc_data.DATA[adc_data.inputPosition][adc_data.dataCount++] = (adc_raw >> 8) & 0x00ff;
            adc_data.DATA[adc_data.inputPosition][adc_data.dataCount++] = adc_raw & 0x00ff;
            printf("%d r:%04x \r\n", adc_data.dataCount, adc_raw);
            
        }
    }
}


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
    int32_t UDP_ret = 0;
    uint8_t TCP_Client_DestIp[4] = {192, 168, 0, 3};
    uint16_t TCP_Client_Port = 22000;
    uint16_t TCP_C_status = 0;
    uint8_t *tcp_c_rcv_data = 0;
    uint16_t tcp_c_rcv_size = 0;
    int tcp_c_ret = 0;

    //int8_t mic_Data[2048];
    int8_t mic_Data[200000];
    
    uint32_t mic_cnt = 0;
    uint8_t data_send_status = 0;
    uint32_t send_count = 0;
    
    uint16_t adc_raw = 0;
    uint16_t adc_raw1 = 0;
    uint32_t i= 0;

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

    //adc init
    bi_decl(bi_program_description("Analog microphone example for Raspberry Pi Pico")); // for picotool
    bi_decl(bi_1pin_with_name(ADC_PIN, "ADC input pin"));
    sleep_ms(1000);
    printf("Starting Program\n");
#if 1
    adc_init();
    adc_gpio_init( ADC_PIN);
    adc_select_input( ADC_NUM);
    //sleep_ms(1000);
    printf("Starting Program\n");
    //adc fifo 
    
    adc_fifo_setup(
        true,    // Write each completed conversion to the sample FIFO
        true,    // Enable DMA data request (DREQ)
        1,       // DREQ (and IRQ) asserted when at least 1 sample present
        true,   // We won't see the ERR bit because of 8 bit reads; disable.
        false     // Shift each sample to 8 bits when pushing to FIFO
    );
    adc_set_clkdiv(1087);// (1+999)/48Mhz = 48kS/s   199=240kS/s  239=200kS/s 1087=44118S/s
    sleep_ms(1000);
    printf("Starting capture\n");
    adc_run(true);
    #endif
    //multicore_launch_core1(core1_entry);
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
        //wizchip_delay_ms(1000); // wait for 1 second
        //loopback_tcps(TCP_S_SOCKET, tcp_buff, TCP_S_PORT);
        #if 1
        //adc
        //adc_raw = adc_read(); // raw voltage from ADC
        //adc fifo
        //adc_raw = adc_fifo_get_blocking();
        #if 1
        if(data_send_status == 1)
        {
            #if 0
            for(i= 0; i<736; i++)
            {
                adc_raw = adc_fifo_get_blocking();
                //adc_raw = adc_read();
                adc_raw1 = (adc_raw&0x0fff) - (1<<10); 
                mic_Data[mic_cnt++] = adc_raw1 & 0x00ff;
                mic_Data[mic_cnt++] = (adc_raw1 >> 8) & 0x00ff;
                #if 0
                adc_raw11.fdata = adc_raw * ADC_CONVERT;
                mic_Data[mic_cnt++] = (adc_raw11.hdata >> 8) & 0x00ff;
                mic_Data[mic_cnt++] = adc_raw11.hdata & 0x00ff;
                #endif
            }
            sendto(UDP_SOCKET, mic_Data, mic_cnt, UDP_BroadIP, UDP_SPORT);
            //printf("send %d\r\n", mic_cnt);
            //printf("0x%04x | 0x%08x %.2f 0x%08x %.2f | %02x %02x | %02x %02x | %02x %02x| \r\n",adc_raw ,adc_raw*ADC_CONVERT, adc_raw*ADC_CONVERT, adc_raw11.hdata, adc_raw11.fdata, mic_Data[0]&0x00ff, mic_Data[1]&0x00ff ,mic_Data[2]&0x00ff, mic_Data[3]&0x00ff, mic_Data[4]&0x00ff, mic_Data[5]&0x00ff);
            //printf("0x%04x  0x%04x | %02x %02x | %02x %02x | %02x %02x| \r\n", adc_raw, adc_raw1, mic_Data[0]&0x00ff, mic_Data[1]&0x00ff ,mic_Data[2]&0x00ff, mic_Data[3]&0x00ff, mic_Data[4]&0x00ff, mic_Data[5]&0x00ff);
            mic_cnt = 0;
            send_count++;
            #else
            /*
            adc_raw = adc_fifo_get_blocking();
            //adc_raw = adc_read();
            adc_raw1 = (adc_raw&0x0fff) - (1<<10);
            mic_Data[mic_cnt++] = adc_raw1 & 0x00ff;
            mic_Data[mic_cnt++] = (adc_raw1 >> 8) & 0x00ff;
            sendto(UDP_SOCKET, mic_Data, 2, UDP_BroadIP, UDP_SPORT);
            send_count++;
            mic_cnt = 0;
            */
            
            for(i= 0; i<250; i++)
            {
                adc_raw = adc_fifo_get_blocking();
                //adc_raw = adc_read();
                adc_raw1 = (adc_raw&0x0fff) - (1<<10);
                mic_Data[mic_cnt++] = adc_raw1 & 0x00ff;
                mic_Data[mic_cnt++] = (adc_raw1 >> 8) & 0x00ff;
                #if 0
                adc_raw11.fdata = adc_raw * ADC_CONVERT;
                mic_Data[mic_cnt++] = (adc_raw11.hdata >> 8) & 0x00ff;
                mic_Data[mic_cnt++] = adc_raw11.hdata & 0x00ff;
                #endif
            }
            //sendto(UDP_SOCKET, mic_Data, mic_cnt, UDP_BroadIP, UDP_SPORT);
            //printf("send %d\r\n", mic_cnt);
            //printf("0x%04x | 0x%08x %.2f 0x%08x %.2f | %02x %02x | %02x %02x | %02x %02x| \r\n",adc_raw ,adc_raw*ADC_CONVERT, adc_raw*ADC_CONVERT, adc_raw11.hdata, adc_raw11.fdata, mic_Data[0]&0x00ff, mic_Data[1]&0x00ff ,mic_Data[2]&0x00ff, mic_Data[3]&0x00ff, mic_Data[4]&0x00ff, mic_Data[5]&0x00ff);
            //printf("0x%04x  0x%04x | %02x %02x | %02x %02x | %02x %02x| \r\n", adc_raw, adc_raw1, mic_Data[0]&0x00ff, mic_Data[1]&0x00ff ,mic_Data[2]&0x00ff, mic_Data[3]&0x00ff, mic_Data[4]&0x00ff, mic_Data[5]&0x00ff);
            mic_cnt = 0;
            send_count++;
            sendto(UDP_SOCKET, mic_Data, 500, UDP_BroadIP, UDP_SPORT);
            /*
            for(i=0; i<1; i++)
            {
                //printf("cnt = %d , %d\r\n",i, i*1472);
                sendto(UDP_SOCKET, mic_Data+(i * 1472), 1472, UDP_BroadIP, UDP_SPORT);
            }*/
            //printf("send = %d \r\n",send_count);
            #endif
            
            
        }
        
        if(send_count >= 2000)  
        {
            UDP_ret = sendto(UDP_SOCKET, "STOP", 5, UDP_BroadIP, UDP_SPORT);
            printf("send finish %d\r\n", send_count);
            data_send_status = 0;
            send_count = 0;
            adc_run(false);
        }
        
        #endif
        //printf("%.2f\n", adc_raw * ADC_CONVERT);

        //TCP_C_status =TCP_client(TCP_C_SOCKET, TCP_Client_DestIp, TCP_Client_Port);
        UDP_S_status = udps_status(UDP_SOCKET, UDP_buff, UDP_PORT);
        TCP_S_status = TCP_Server(TCP_S_SOCKET, TCP_S_PORT);
        #if 0
        if(TCP_C_status == 17)
        {
            tcp_c_rcv_data = TCP_S_Recv(TCP_C_SOCKET, &tcp_c_rcv_size);
            if(tcp_c_rcv_data != 0)
            {
                printf("rcv[%d] : %s \r\n",tcp_c_rcv_size, tcp_c_rcv_data);
                tcp_c_ret = send(TCP_C_SOCKET, tcp_c_rcv_data, tcp_c_rcv_size); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
				if(tcp_c_ret < 0) // Send Error occurred (sent data length < 0)
				{
					close(TCP_C_SOCKET); // socket close
					printf("TCP Client Sedn message Error !! \r\n");
				}
                
                if(strncmp(tcp_c_rcv_data, "ready", 5) == 0)
                {
                    printf("client data recv ready\r\n");
                    data_send_status = 1;
                }
                if(data_send_status ==1)
                {
                    if(mic_cnt > 191999)
                    {
                        data_send_status = 0;
                        tcp_c_ret = send(TCP_C_SOCKET, mic_Data, mic_cnt);
                        printf("client data send complite %d\r\n", tcp_c_ret);
                    }
                }
                free(tcp_c_rcv_data);
            }
        }
        #endif
        if(TCP_S_status == 17)
        {
            //TCP_Server_Buf = TCP_S_Recv(TCP_S_SOCKET);
            //tcp_rcv_data = TCP_S_Recv(TCP_S_SOCKET);
            tcp_rcv_data = TCP_S_Recv(TCP_S_SOCKET, &tcp_rcv_size);
            if(tcp_rcv_data != 0)
            {
                printf("rcv[%d] : %s \r\n",tcp_rcv_size, tcp_rcv_data);
                if(UDP_S_status == 0x22)
                {
                    //UDP_ret = sendto(UDP_SOCKET, tcp_rcv_data, tcp_rcv_size, UDP_BroadIP, UDP_SPORT);
                    if(UDP_ret < 0)
                    {
                        printf("sendto Error \r\n");
                    }
                }
                if(strncmp(tcp_rcv_data, "start", 5) == 0)
                {
                    printf(" data  send start\r\n");
                    //UDP_ret = sendto(UDP_SOCKET, "START", 5, UDP_BroadIP, UDP_SPORT);
                    adc_run(true);
                    adc_fifo_drain();
                    data_send_status = 1;
                    //adc_data.sendStatus = 1;
                }
                else if(strncmp(tcp_rcv_data, "stop", 4) == 0)
                {
                    printf("data send stop \r\n");
                    data_send_status = 0;
                    //adc_data.sendStatus = 0;
                    adc_run(false);
                }
                free(tcp_rcv_data);
            }
            #if 0
            if(TCP_Server_Buf->RCV_Size > 0 )
            {
                printf("size %d : %s|", TCP_Server_Buf->RCV_Size, TCP_Server_Buf->RCV_DATA);
                free(TCP_Server_Buf->RCV_DATA);
            }
            #endif
        }
        #endif
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

uint16_t TCP_Server(uint8_t sn, uint16_t port)
{
   int32_t ret;
   uint8_t destip[4];
    uint16_t destport;

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
            //TCP_S_Recv(sn);
            #if 0
            if(getSn_IR(sn) & Sn_IR_CON)
         {
#ifdef _LOOPBACK_DEBUG_
			getSn_DIPR(sn, destip);
			destport = getSn_DPORT(sn);

			printf("%d:Connected - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
			setSn_IR(sn,Sn_IR_CON);
         }
            #endif
	  	   return 17;
         break;
      case SOCK_CLOSE_WAIT :
#ifdef TCP_S_DEBUG_
         printf("%d:CloseWait\r\n",sn);
#endif
         if((ret = disconnect(sn)) != SOCK_OK) return ret;
#ifdef TCP_S_DEBUG_
         printf("%d:Socket Closed\r\n", sn);
#endif
         break;
      case SOCK_INIT :
#ifdef TCP_S_DEBUG_
    	 printf("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
#endif
         if( (ret = listen(sn)) != SOCK_OK) return ret;
         break;
      case SOCK_CLOSED:
#ifdef TCP_S_DEBUG_
         //printf("%d:TCP server loopback start\r\n",sn);
#endif
         if((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
#ifdef TCP_S_DEBUG_
         printf("%d:Socket opened\r\n",sn);
#endif
         break;
      default:
         break;
   }
   return 1;
}

//TCP_S_RSV_DATA *TCP_S_Recv(uint8_t sn)
uint8_t *TCP_S_Recv(uint8_t sn, uint16_t *rcv_size)
{
    #if 0
    TCP_S_RSV_DATA *temp_data = 0;
    temp_data->RCV_DATA = 0;
    #endif
    uint16_t size;
    int ret;
    uint8_t destip[4];
    uint16_t destport;
    uint8_t *buff=0;
   if(getSn_IR(sn) & Sn_IR_CON)
   {
#ifdef TCP_S_DEBUG_
        printf("Connected in function\r\n");
#if 0
        getSn_DIPR(sn, temp_data->DestIP);
        temp_data->Port = getSn_DPORT(sn);
        printf("%d:Connected - %d.%d.%d.%d : %d\r\n",sn, temp_data->DestIP[0], temp_data->DestIP[1], temp_data->DestIP[2], temp_data->DestIP[3], temp_data->Port);
        #endif
        getSn_DIPR(sn, destip);
        destport = getSn_DPORT(sn);
        printf("%d:Connected - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
        setSn_IR(sn,Sn_IR_CON);
   }
   //if((temp_data->RCV_Size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
   if((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
   {
       if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
       printf("recv Data size : %d \r\n", size);
       buff = (uint8_t *)calloc(size, sizeof(uint8_t));
       if(buff == 0)
       {
            printf("calloc error \r\n");
            return 0;
       }
       ret = recv(sn, buff, size);
       if(ret <= 0) 
       {
            free(buff);
            *rcv_size = ret;
            return 0;      // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
       }
       
    }
   *rcv_size = size;
   return buff;
}


uint16_t TCP_client(uint8_t sn, uint8_t* destip, uint16_t destport)
{
   int32_t ret; // return value for SOCK_ERRORs
   uint16_t size = 0, sentsize=0;

   // Destination (TCP Server) IP info (will be connected)
   // >> loopback_tcpc() function parameter
   // >> Ex)
   //	uint8_t destip[4] = 	{192, 168, 0, 214};
   //	uint16_t destport = 	5000;

   // Port number for TCP client (will be increased)
   static uint16_t any_port = 	50000;

   // Socket Status Transitions
   // Check the W5500 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
#if 0

         if(getSn_IR(sn) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
         {
#ifdef _LOOPBACK_DEBUG_
			printf("%d:Connected to - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
			setSn_IR(sn, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
         }

         //////////////////////////////////////////////////////////////////////////////////////////////
         // Data Transaction Parts; Handle the [data receive and send] process
         //////////////////////////////////////////////////////////////////////////////////////////////
		 if((size = getSn_RX_RSR(sn)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
         {
			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE; // DATA_BUF_SIZE means user defined buffer size (array)
			ret = recv(sn, buf, size); // Data Receive process (H/W Rx socket buffer -> User's buffer)

			if(ret <= 0) return ret; // If the received data length <= 0, receive failed and process end
			size = (uint16_t) ret;
			sentsize = 0;

			// Data sentsize control
			while(size != sentsize)
			{
				ret = send(sn, buf+sentsize, size-sentsize); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
				if(ret < 0) // Send Error occurred (sent data length < 0)
				{
					close(sn); // socket close
					return ret;
				}
				sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
			}
         }
#endif
         return 17;
		 //////////////////////////////////////////////////////////////////////////////////////////////
         break;

      case SOCK_CLOSE_WAIT :
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:CloseWait\r\n",sn);
#endif
         if((ret=disconnect(sn)) != SOCK_OK) return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Socket Closed\r\n", sn);
#endif
         break;

      case SOCK_INIT :
#ifdef _LOOPBACK_DEBUG_
    	 printf("%d:Try to connect to the %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
    	 if( (ret = connect(sn, destip, destport)) != SOCK_OK) return ret;	//	Try to TCP connect to the TCP server (destination)
         break;

      case SOCK_CLOSED:
    	  close(sn);
    	  if((ret=socket(sn, Sn_MR_TCP, any_port++, 0x00)) != sn){
         if(any_port == 0xffff) any_port = 50000;
         return ret; // TCP socket open with 'any_port' port number
        } 
#ifdef _LOOPBACK_DEBUG_
    	 //printf("%d:TCP client loopback start\r\n",sn);
         //printf("%d:Socket opened\r\n",sn);
#endif
         break;
      default:
         break;
   }
   return 1;
}

int32_t udps_status(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t  ret;
   uint16_t size, sentsize;
   uint8_t  destip[4];
   uint16_t destport;

   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(sn)) > 0)
         {
            if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
            ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
            if(ret <= 0)
            {
#ifdef _LOOPBACK_DEBUG_
               printf("%d: recvfrom error. %ld\r\n",sn,ret);
#endif
               return ret;
            }
            size = (uint16_t) ret;
            sentsize = 0;
            while(sentsize != size)
            {
               ret = sendto(sn, buf+sentsize, size-sentsize, destip, destport);
               if(ret < 0)
               {
#ifdef _LOOPBACK_DEBUG_
                  printf("%d: sendto error. %ld\r\n",sn,ret);
#endif
                  return ret;
               }
               sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
         }
         return SOCK_UDP;
         break;
      case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
         //printf("%d:UDP loopback start\r\n",sn);
#endif
         if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;
#ifdef _LOOPBACK_DEBUG_
         printf("%d:Opened, UDP loopback, port [%d]\r\n", sn, port);
#endif
         break;
      default :
         break;
   }
   return 1;
}

