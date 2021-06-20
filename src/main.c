// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <sys/printk.h>
#include <zephyr.h>

void main(void) { printk("Hello %s\n", CONFIG_BOARD); }
