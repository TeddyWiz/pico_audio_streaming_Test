# CMake minimum required version
cmake_minimum_required(VERSION 3.12)

# Find git
find_package(Git)

if(NOT Git_FOUND)
	message(FATAL_ERROR "Could not find 'git' tool for RP2040-HAT-AZURE-C patching")
endif()

message("RP2040-HAT-AZURE-C patch utils found")

set(RP2040_HAT_AZURE_C_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(AZURE_IOT_SDK_C_SRC_DIR "${RP2040_HAT_AZURE_C_SRC_DIR}/libraries/azure-iot-sdk-c")
set(AZURE_IOT_SDK_C_UTILITY_SRC_DIR "${RP2040_HAT_AZURE_C_SRC_DIR}/libraries/azure-iot-sdk-c/c-utility")
set(AZURE_IOT_SDK_C_UAMQP_SRC_DIR "${RP2040_HAT_AZURE_C_SRC_DIR}/libraries/azure-iot-sdk-c/uamqp")
set(AZURE_IOT_SDK_C_UMQTT_SRC_DIR "${RP2040_HAT_AZURE_C_SRC_DIR}/libraries/azure-iot-sdk-c/umqtt")
set(IOLIBRARY_DRIVER_SRC_DIR "${RP2040_HAT_AZURE_C_SRC_DIR}/libraries/ioLibrary_Driver")
set(MBEDTLS_SRC_DIR "${RP2040_HAT_AZURE_C_SRC_DIR}/libraries/mbedtls")
set(PICO_EXTRAS_SRC_DIR "${RP2040_HAT_AZURE_C_SRC_DIR}/libraries/pico-extras")
set(PICO_SDK_SRC_DIR "${RP2040_HAT_AZURE_C_SRC_DIR}/libraries/pico-sdk")
set(PICO_SDK_TINYUSB_SRC_DIR "${RP2040_HAT_AZURE_C_SRC_DIR}/libraries/pico-sdk/lib/tinyusb")
set(RP2040_HAT_AZURE_C_PATCH_DIR "${RP2040_HAT_AZURE_C_SRC_DIR}/patches")

# Delete untracked files in azure-iot-sdk-c
if(EXISTS "${AZURE_IOT_SDK_C_SRC_DIR}/.git")
	message("cleaning azure-iot-sdk-c...")
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${AZURE_IOT_SDK_C_SRC_DIR} clean -fdx)
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${AZURE_IOT_SDK_C_SRC_DIR} reset --hard)
	message("azure-iot-sdk-c cleaned")
endif()

# Delete untracked files in ioLibrary_Driver
if(EXISTS "${IOLIBRARY_DRIVER_SRC_DIR}/.git")
	message("cleaning ioLibrary_Driver...")
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${IOLIBRARY_DRIVER_SRC_DIR} clean -fdx)
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${IOLIBRARY_DRIVER_SRC_DIR} reset --hard)
	message("ioLibrary_Driver cleaned")
endif()

# Delete untracked files in mbedtls
if(EXISTS "${MBEDTLS_SRC_DIR}/.git")
	message("cleaning mbedtls...")
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${MBEDTLS_SRC_DIR} clean -fdx)
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${MBEDTLS_SRC_DIR} reset --hard)
	message("mbedtls cleaned")
endif()

# Delete untracked files in pico-extras
if(EXISTS "${PICO_EXTRAS_SRC_DIR}/.git")
	message("cleaning pico-extras...")
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PICO_EXTRAS_SRC_DIR} clean -fdx)
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PICO_EXTRAS_SRC_DIR} reset --hard)
	message("pico-extras cleaned")
endif()

# Delete untracked files in pico-sdk
if(EXISTS "${PICO_SDK_SRC_DIR}/.git")
	message("cleaning pico-sdk...")
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PICO_SDK_SRC_DIR} clean -fdx)
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PICO_SDK_SRC_DIR} reset --hard)
	message("pico-sdk cleaned")
endif()

execute_process(COMMAND ${GIT_EXECUTABLE} -C ${RP2040_HAT_AZURE_C_SRC_DIR} submodule update --init)

# Delete untracked files in c-utility
if(EXISTS "${AZURE_IOT_SDK_C_UTILITY_SRC_DIR}/.git")
	message("cleaning azure-iot-sdk-c c-utility...")
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${AZURE_IOT_SDK_C_UTILITY_SRC_DIR} clean -fdx)
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${AZURE_IOT_SDK_C_UTILITY_SRC_DIR} reset --hard)
	message("azure-iot-sdk-c c-utility cleaned")
endif()

# Delete untracked files in uamqp
if(EXISTS "${AZURE_IOT_SDK_C_UAMQP_SRC_DIR}/.git")
	message("cleaning azure-iot-sdk-c uamqp...")
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${AZURE_IOT_SDK_C_UAMQP_SRC_DIR} clean -fdx)
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${AZURE_IOT_SDK_C_UAMQP_SRC_DIR} reset --hard)
	message("azure-iot-sdk-c uamqp cleaned")
endif()

# Delete untracked files in umqtt
if(EXISTS "${AZURE_IOT_SDK_C_UMQTT_SRC_DIR}/.git")
	message("cleaning azure-iot-sdk-c umqtt...")
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${AZURE_IOT_SDK_C_UMQTT_SRC_DIR} clean -fdx)
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${AZURE_IOT_SDK_C_UMQTT_SRC_DIR} reset --hard)
	message("azure-iot-sdk-c umqtt cleaned")
endif()

execute_process(COMMAND ${GIT_EXECUTABLE} -C ${AZURE_IOT_SDK_C_SRC_DIR} submodule update --init)

# Delete untracked files in tinyusb
if(EXISTS "${PICO_SDK_TINYUSB_SRC_DIR}/.git")
	message("cleaning pico-sdk tinyusb...")
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PICO_SDK_TINYUSB_SRC_DIR} clean -fdx)
	execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PICO_SDK_TINYUSB_SRC_DIR} reset --hard)
	message("pico-sdk tinyusb cleaned")
endif()

execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PICO_SDK_SRC_DIR} submodule update --init)

# ioLibrary_Driver patch
message("submodules ioLibrary_Driver initialised")

file(GLOB IOLIBRARY_DRIVER_PATCHES 
	"${RP2040_HAT_AZURE_C_PATCH_DIR}/01_iolibrary_driver_ethernet_chip.patch" 
	"${RP2040_HAT_AZURE_C_PATCH_DIR}/02_iolibrary_driver_sntp.patch"	
	)

foreach(IOLIBRARY_DRIVER_PATCH IN LISTS IOLIBRARY_DRIVER_PATCHES)
	message("Running patch ${IOLIBRARY_DRIVER_PATCH}")
	execute_process(
		COMMAND ${GIT_EXECUTABLE} apply --ignore-whitespace ${IOLIBRARY_DRIVER_PATCH}
		WORKING_DIRECTORY ${IOLIBRARY_DRIVER_SRC_DIR}
	)
endforeach()
