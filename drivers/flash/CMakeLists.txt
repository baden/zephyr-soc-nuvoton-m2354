# SPDX-License-Identifier: Apache-2.0

# zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/flash.h)

zephyr_library()

#zephyr_library_sources_ifdef(CONFIG_SOC_FLASH_NUMAKER2 soc_flash_numaker.c)
zephyr_library_sources(soc_flash_numaker.c)
