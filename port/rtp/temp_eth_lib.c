#include <stdio.h>
#include <stdlib.h>
#include "temp_eth_lib.h"
#include "wizchip_conf.h"
#include "socket.h"

#define TCP_S_DEBUG_
#define _LOOPBACK_DEBUG_

uint16_t TCP_Server(uint8_t sn, uint16_t port)
{
   int32_t ret;
   uint8_t destip[4];
    uint16_t destport;

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
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

