#
# Copyright (c) 2019 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.20.0)

set(PM_STATIC_YML_FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/boards/pm_static_${BOARD}.yml
  )

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(bdi-coap-client)


FILE(GLOB APP_SOURCES src/main.cpp
			   src/coap_client_utils.c
         src/peripherals/sensors.c
         src/peripherals/BME688.cpp
         src/coap/CoapClient.cpp
         src/communication/CoapChannel.cpp
         src/communication/AbstractChannel.h
         src/mas-abstractions/organization/Organization.cpp
         src/Role.cpp
         src/thread_helper.c
         src/peripherals/gpio.c
         src/peripherals/battery.c
         src/bdi-agent/functions.cpp
         src/parse_xml.cpp)

FILE(GLOB EMB_BDI_AGENT_SOURCES embedded-bdi/lib/agent/*.cpp)
FILE(GLOB EMB_BDI_BDI_SOURCES embedded-bdi/lib/bdi/*.cpp)
FILE(GLOB EMB_BDI_LIB_SOURCES embedded-bdi/lib/lib/*.cpp)
FILE(GLOB EMB_BDI_SYNTAX_SOURCES embedded-bdi/lib/syntax/*.cpp)

# NORDIC SDK APP START

target_sources(app PRIVATE ${APP_SOURCES} ${EMB_BDI_AGENT_SOURCES} ${EMB_BDI_BDI_SOURCES} ${EMB_BDI_LIB_SOURCES} ${EMB_BDI_SYNTAX_SOURCES})

target_include_directories(app PRIVATE src embedded-bdi embedded-bdi/lib interface)

# Preinitialization related to Thingy:53 DFU
target_sources_ifdef(CONFIG_BOARD_THINGY53_NRF5340_CPUAPP app PRIVATE
  boards/thingy53.c
)
target_sources_ifdef(CONFIG_BOARD_THINGY53_NRF5340_CPUAPP_NS app PRIVATE
  boards/thingy53.c
)

# NORDIC SDK APP END

target_sources_ifdef(CONFIG_BT_NUS app PRIVATE src/ble_utils.c)
zephyr_library_include_directories(.)
