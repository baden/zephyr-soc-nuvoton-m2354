# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_CLOCK_CONTROL_NUMAKER2_SCC         clock_control_numaker_scc.c)
