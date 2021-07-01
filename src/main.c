// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <sys/printk.h>
#include <zephyr.h>

#if defined(CONFIG_APP_WIFI_AUTO)
#include "wifi.h"
#elif defined(CONFIG_NET_DHCPV4)
#include <net/net_if.h>
#endif

void main(void) {
  printk("Hello %s\n", CONFIG_BOARD);
#if defined(CONFIG_APP_WIFI_AUTO)
  // wifi auto connect
  wifi_connect();
#elif defined(CONFIG_NET_DHCPV4)
  // enable dhcpv4 for eth
  struct net_if *iface;
  iface = net_if_get_default();
  net_dhcpv4_start(iface);
#endif
}
