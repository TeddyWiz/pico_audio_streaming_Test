# ioLibrary_Driver
add_library(SPI_FILES STATIC)

target_sources(SPI_FILES PUBLIC
        ${PORT_DIR}/ioLibrary_Driver/w5x00_spi.c
        )

target_include_directories(SPI_FILES PUBLIC
        ${WIZNET_DIR}/Ethernet
        ${PORT_DIR}/ioLibrary_Driver
        ${PORT_DIR}
        )

target_link_libraries(SPI_FILES PRIVATE
        pico_stdlib
        pico_multicore
        hardware_spi
        hardware_dma
        hardware_clocks
        )

# timer
add_library(TIMER_FILES STATIC)

target_sources(TIMER_FILES PUBLIC
        ${PORT_DIR}/timer/timer.c
        )

target_include_directories(TIMER_FILES PUBLIC
        ${PORT_DIR}/timer
        )

target_link_libraries(TIMER_FILES PRIVATE
        pico_stdlib      
        )

# azure-iot-sdk-c
add_library(AZURE_SDK_PORT_FILES STATIC)

target_sources(AZURE_SDK_PORT_FILES PUBLIC
        ${PORT_DIR}/azure-iot-sdk-c/src/agenttime.c
        ${PORT_DIR}/azure-iot-sdk-c/src/platform.c
        ${PORT_DIR}/azure-iot-sdk-c/src/netif.c
        ${PORT_DIR}/azure-iot-sdk-c/src/netutil.c
        ${PORT_DIR}/azure-iot-sdk-c/src/tcpsocketconnection_c.c
        ${PORT_DIR}/azure-iot-sdk-c/src/tlsio_mbedtls.c
        ${PORT_DIR}/azure-iot-sdk-c/src/lock.c
        ${PORT_DIR}/azure-iot-sdk-c/src/threadapi_nonos.c
        ${PORT_DIR}/azure-iot-sdk-c/src/tickcounter_nonos.c
        )

target_include_directories(AZURE_SDK_PORT_FILES PUBLIC
        ${PORT_DIR}/azure-iot-sdk-c/inc
        ${MBEDTLS_LIB_DIR}/include
        )

target_link_libraries(AZURE_SDK_PORT_FILES PUBLIC
        pico_stdlib
        ETHERNET_FILES
        DHCP_FILES
        DNS_FILES
        SNTP_FILES
        AZURE_SDK_FILES
        )
# rtp
add_library(RTP_FILES STATIC)

target_sources(RTP_FILES PUBLIC
        ${PORT_DIR}/rtp/rtp.c
        ${PORT_DIR}/rtp/sdp_proto.c
        ${PORT_DIR}/rtp/g711.c
        ${PORT_DIR}/rtp/temp_eth_lib.c
        )

target_include_directories(RTP_FILES PUBLIC
        ${PORT_DIR}/rtp
        )

target_link_libraries(RTP_FILES PRIVATE
        pico_stdlib      
        ETHERNET_FILES
        )

