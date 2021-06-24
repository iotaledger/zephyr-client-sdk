// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <sys/printk.h>
#include <zephyr.h>

#if defined(CONFIG_APP_WIFI_AUTO)
#include "wifi.h"
#endif

void main(void) {
  printk("Hello %s\n", CONFIG_BOARD);
#if defined(CONFIG_APP_WIFI_AUTO)
  wifi_connect();
#endif
}
