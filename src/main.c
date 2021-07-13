// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <net/net_mgmt.h>
#include <stdio.h>
#include <zephyr.h>

#include "iota_shell.h"
#if defined(CONFIG_APP_WIFI_AUTO)
#include <net/wifi_mgmt.h>
#include "wifi.h"
#elif defined(CONFIG_NET_DHCPV4)
#include <net/net_event.h>
#include <net/net_if.h>
#endif

struct net_mgmt_event_callback net_event_cb;
static bool wallet_created = false;

#if defined(CONFIG_APP_WIFI_AUTO)
static void on_wifi_event(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface) {
  const struct wifi_status *status = (const struct wifi_status *)cb->info;
  if (!wallet_created && status->status == 0) {
    if (init_wallet() == 0) {
      wallet_created = true;
    }
  }
}
#elif defined(CONFIG_NET_DHCPV4)
static void on_dhcp_event(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface) {
  if (mgmt_event == NET_EVENT_IPV4_ADDR_ADD) {
    if (!wallet_created) {
      if (init_wallet() == 0) {
        wallet_created = true;
      }
    }
  }
}
#endif

void main(void) {
  printf("Hello %s\n", CONFIG_BOARD);

#if defined(CONFIG_APP_WIFI_AUTO)
  // wait for network event
  net_mgmt_init_event_callback(&net_event_cb, on_wifi_event, NET_EVENT_WIFI_CONNECT_RESULT);
  net_mgmt_add_event_callback(&net_event_cb);
  // wifi auto connect
  wifi_connect();
#elif defined(CONFIG_NET_DHCPV4)
  // wait for network event
  net_mgmt_init_event_callback(&net_event_cb, on_dhcp_event, (NET_EVENT_IPV4_ADDR_ADD | NET_EVENT_IPV4_ADDR_DEL));
  net_mgmt_add_event_callback(&net_event_cb);
  // enable dhcpv4 for eth
  struct net_if *iface;
  iface = net_if_get_default();
  net_dhcpv4_start(iface);
#endif
}
