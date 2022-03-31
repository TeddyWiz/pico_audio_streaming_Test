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

/**
  * ----------------------------------------------------------------------------------------------------
  * Variables
  * ----------------------------------------------------------------------------------------------------
  */
// The application you wish to use DHCP mode should be uncommented
#define _DHCP
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

/* Timer */
static uint16_t g_msec_cnt = 0;

/**
  * ----------------------------------------------------------------------------------------------------
  * Functions
  * ----------------------------------------------------------------------------------------------------
  */
/* Timer callback */
static void repeating_timer_callback(void);

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
        wizchip_delay_ms(1000); // wait for 1 second
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
