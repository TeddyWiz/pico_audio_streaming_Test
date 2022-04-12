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
/* Timer */
static uint16_t g_msec_cnt = 0;

/**
  * ----------------------------------------------------------------------------------------------------
  * Functions
  * ----------------------------------------------------------------------------------------------------
  */
/* Timer callback */
static void repeating_timer_callback(void);

uint16_t TCP_Server(uint8_t sn, uint16_t port);
//TCP_S_RSV_DATA *TCP_S_Recv(uint8_t sn);
//uint8_t *TCP_S_Recv(uint8_t sn);
uint8_t *TCP_S_Recv(uint8_t sn, uint16_t *rcv_size);



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

    int8_t tcp_buff[2048];
    uint16_t TCP_S_status = 0;
    //TCP_S_RSV_DATA *TCP_Server_Buf = 0;
    uint8_t *tcp_rcv_data = 0;
    uint16_t tcp_rcv_size = 0;

    stdio_init_all();

    wizchip_delay_ms(1000 * 3); // wait for 3 seconds

    wizchip_spi_initialize();
    wizchip_cris_initialize();

    wizchip_reset();
    wizchip_initialize();
    wizchip_check();

    wizchip_1ms_timer_initialize(repeating_timer_callback);

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
        TCP_S_status = TCP_Server(TCP_S_SOCKET, TCP_S_PORT);
        if(TCP_S_status == 17)
        {
        
            //TCP_Server_Buf = TCP_S_Recv(TCP_S_SOCKET);
            //tcp_rcv_data = TCP_S_Recv(TCP_S_SOCKET);
            tcp_rcv_data = TCP_S_Recv(TCP_S_SOCKET, &tcp_rcv_size);
            if(tcp_rcv_data != 0)
            {
                printf("rcv[%d] : %s \r\n",tcp_rcv_size, tcp_rcv_data);
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

