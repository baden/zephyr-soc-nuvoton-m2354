# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_property(ALLOW_EMPTY TRUE)

zephyr_library_sources_ifdef(CONFIG_UART_NUMAKER2 uart_numaker.c)
# zephyr_library_sources(uart_numaker.c)
