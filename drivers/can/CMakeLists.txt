# SPDX-License-Identifier: Apache-2.0

#zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/can.h)

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_CAN_NUMAKER2      can_numaker.c)
