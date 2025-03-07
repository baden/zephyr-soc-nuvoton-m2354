# SPDX-License-Identifier: Apache-2.0

# zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/adc.h)

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_ADC_NUMAKER2         adc_numaker.c)
