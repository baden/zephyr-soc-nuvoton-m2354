# SPDX-License-Identifier: Apache-2.0

#zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/pwm.h)

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_PWM_NUMAKER2	pwm_numaker.c)
