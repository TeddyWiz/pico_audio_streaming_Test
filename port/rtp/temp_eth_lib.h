
#ifndef _TEMP_ETH_LIB_H
#define _TEMP_ETH_LIB_H

/* DATA_BUF_SIZE define for Loopback example */
#ifndef DATA_BUF_SIZE
	#define DATA_BUF_SIZE			2048
#endif

uint16_t TCP_Server(uint8_t sn, uint16_t port);
uint16_t TCP_client(uint8_t sn, uint8_t* destip, uint16_t destport);

//TCP_S_RSV_DATA *TCP_S_Recv(uint8_t sn);
//uint8_t *TCP_S_Recv(uint8_t sn);
uint8_t *TCP_S_Recv(uint8_t sn, uint16_t *rcv_size);
int32_t udps_status(uint8_t sn, uint8_t* buf, uint16_t port);

#endif
