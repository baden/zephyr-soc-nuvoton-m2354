# SPDX-License-Identifier: Apache-2.0

#zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/reset.h)

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_RESET_NUMAKER2 reset_numaker.c)
