# SPDX-License-Identifier: Apache-2.0

#zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/gpio.h)

zephyr_library()
zephyr_library_property(ALLOW_EMPTY TRUE)

#zephyr_library_sources_ifdef(CONFIG_GPIO_NUMAKER2    gpio_numaker.c)
zephyr_library_sources(gpio_numaker.c)

