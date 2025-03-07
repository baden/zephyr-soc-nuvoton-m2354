# SPDX-License-Identifier: Apache-2.0

#zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/spi.h)

zephyr_library()
zephyr_library_property(ALLOW_EMPTY TRUE)

zephyr_library_sources_ifdef(CONFIG_SPI_NUMAKER2		spi_numaker.c)
