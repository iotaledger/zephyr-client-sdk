// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <zephyr.h>

#include "iota_shell.h"
#if defined(CONFIG_APP_WIFI_AUTO)
#include "wifi.h"
#elif defined(CONFIG_NET_DHCPV4)
#include <net/net_if.h>
#endif

void main(void) {
  printf("Hello %s\n", CONFIG_BOARD);

  if (init_http_client_conf() != 0) {
    return;
  }

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
