#
# Copyright 2022-2023 NXP
# Copyright 2024 Avnet
#
# SPDX-License-Identifier: Apache-2.0
#

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

list(APPEND DTS_ROOT
  "$ENV{ZEPHYR_BASE}"
)

set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)


if(CONFIG_NXP_IMXRT_BOOT_HEADER)
  zephyr_library()
  if(NOT DEFINED CONFIG_BOARD_MAAX_IMXRT1176_MIMXRT1176_CM7 AND
     NOT DEFINED CONFIG_BOARD_MAAX_IMXRT1176_MIMXRT1176_CM4)
    message(WARNING "It appears you are using the board definition for "
      "the MaaXBoard RT1176, but targeting a custom board. You may need to "
      "update your flash configuration or device configuration data blocks")
  endif()

  set(RT1170_BOARD_NAME "maax_imxrt1176")
  set(RT1170_BOARD_DIR
   ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/maax_imxrt1176)

  if(CONFIG_BOOT_FLEXSPI_NOR)
    zephyr_compile_definitions(XIP_BOOT_HEADER_ENABLE=1)
    zephyr_compile_definitions(BOARD_FLASH_SIZE=CONFIG_FLASH_SIZE*1024)
    zephyr_library_sources(
        ${RT1170_BOARD_DIR}/xip/${RT1170_BOARD_NAME}_flexspi_nor_config.c)
    zephyr_library_include_directories(${RT1170_BOARD_DIR}/xip)
  endif()

  if(CONFIG_EXTERNAL_MEM_CONFIG_DATA)
    # External memory configuration data block (XMCD) for SDRAM.
    message("Including external memory configuration data block (XMCD) for SDRAM")

    zephyr_compile_definitions(XIP_BOOT_HEADER_XMCD_ENABLE=1)
    zephyr_library_sources(${RT1170_BOARD_DIR}/xmcd/xmcd.c)
  else()
    if(CONFIG_SRAM_BASE_ADDRESS EQUAL 0x80000000)
      message(WARNING "You are using SDRAM as RAM but no external memory "
        "configuration data (XMCD) is included. This configuration may not boot")
    endif()
  endif()
endif()

if(CONFIG_MCUX_GPT_TIMER)
  message(WARNING "You appear to be using the GPT hardware timer. "
    "This timer enables lower power modes but at the cost of reduced "
    "hardware timer resolution")
endif()
