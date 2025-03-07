# Copyright (c) 2023 Nuvoton Technology Corporation.
#
# SPDX-License-Identifier: Apache-2.0

#zephyr_sources(soc.c tpf_test.c soc.S)
zephyr_sources(soc.c soc.S)

#target_compile_options(app PRIVATE
#    $<FILTER:$<TARGET_PROPERTY:APP_SRCS>,INCLUDE,soc.c> -fomit-frame-pointer -ffreestanding)

zephyr_include_directories(.)

#set(SOC_LINKER_SCRIPT ${ZEPHYR_BASE}/include/zephyr/arch/arm/cortex_m/scripts/linker.ld CACHE INTERNAL "")
set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld CACHE INTERNAL "")
