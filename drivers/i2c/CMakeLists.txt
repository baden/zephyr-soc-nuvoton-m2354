# SPDX-License-Identifier: Apache-2.0

# zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/i2c.h)

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_I2C_NUMAKER2		i2c_numaker.c)
