// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <logging/log.h>
LOG_MODULE_REGISTER(app, CONFIG_IOTA_APP_LOG_LEVEL);

#include <inttypes.h>
#include <shell/shell.h>
#include <stdio.h>
#include <zephyr.h>

#include <net/http_client.h>
#include <net/socket.h>

#include "client/api/v1/get_balance.h"
#include "client/api/v1/get_node_info.h"
#include "client/api/v1/send_message.h"
#include "client/network/http.h"
#include "wallet/bip39.h"
#include "wallet/wallet.h"

// IOTA wallet instance
static iota_wallet_t *w_ctx = NULL;

static int cmd_info(const struct shell *shell, size_t argc, char **argv) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  res_node_info_t *info = res_node_info_new();
  if (get_node_info(&w_ctx->endpoint, info) == 0) {
    if (info->is_error == false) {
      // display node info on console
#ifdef CONFIG_NEWLIB_LIBC_FLOAT_PRINTF
      printf(
          "Name: %s\nVersion: %s\nNetwork ID: %s\nBech32HRP: %s\nMessage Pre Sec: %.2f\nisHealthy: %s\nLatest "
          "Milestone Index: %" PRIu64 "\nConfirmed Milestone Index: %" PRIu64 "\nPruning Milestone Index: %" PRIu64
          "\nLatest Milestone Timestamp: %" PRIu64 "\n",
          info->u.output_node_info->name, info->u.output_node_info->version, info->u.output_node_info->network_id,
          info->u.output_node_info->bech32hrp, info->u.output_node_info->msg_pre_sec,
          info->u.output_node_info->is_healthy ? "true" : "false", info->u.output_node_info->latest_milestone_index,
          info->u.output_node_info->confirmed_milestone_index, info->u.output_node_info->pruning_milestone_index,
          info->u.output_node_info->latest_milestone_timestamp);
#else
      printf(
          "Name: %s\nVersion: %s\nNetwork ID: %s\nBech32HRP: %s\nMessage Pre Sec: %.2f\nisHealthy: %s\nLatest "
          "Milestone Index: %" PRIu64 "\nConfirmed Milestone Index: %" PRIu64 "\nPruning Milestone Index: %" PRIu64
          "\nLatest Milestone Timestamp: %" PRIu64 "\n",
          info->u.output_node_info->name, info->u.output_node_info->version, info->u.output_node_info->network_id,
          info->u.output_node_info->bech32hrp, info->u.output_node_info->msg_pre_sec,
          info->u.output_node_info->is_healthy ? "true" : "false", info->u.output_node_info->latest_milestone_index,
          info->u.output_node_info->confirmed_milestone_index, info->u.output_node_info->pruning_milestone_index,
          info->u.output_node_info->latest_milestone_timestamp);
#endif
    } else {
      shell_print(shell, "Err: %s\n", info->u.error->msg);
    }
  } else {
    shell_print(shell, "API request failed\n");
  }

  res_node_info_free(info);
  return 0;
}

static int cmd_data(const struct shell *shell, size_t argc, char **argv) {
  res_send_message_t res = {};
  printf("Sending Index: \"%s\", Message: \"%s\"\n", argv[1], argv[2]);
  // send out text message
  int err = send_indexation_msg(&w_ctx->endpoint, argv[1], argv[2], &res);
  if (err) {
    if (res.is_error) {
      LOG_ERR("Err response: %s", res.u.error->msg);
      res_err_free(res.u.error);
    } else {
      LOG_ERR("send_indexation_msg failed (%d)", err);
    }
  } else {
    printf("message ID: %s\n", res.u.msg_id);
  }
  return 0;
}

static void dump_address(iota_wallet_t *w, uint32_t index, bool is_change) {
  char tmp_bech32_addr[65];
  byte_t tmp_addr[ED25519_ADDRESS_BYTES];

  wallet_bech32_from_index(w, is_change, index, tmp_bech32_addr);
  wallet_address_from_index(w, is_change, index, tmp_addr);

  printf("Addr[%" PRIu32 "]\n", index);
  // print ed25519 address without version filed.
  printf("\t");
  dump_hex_str(tmp_addr, ED25519_ADDRESS_BYTES);
  // print out
  printf("\t%s\n", tmp_bech32_addr);
}

static int cmd_addr(const struct shell *shell, size_t argc, char **argv) {
  bool change = (bool)argv[1];  // address type: 0 or 1
  long s = atol(argv[2]);       // starting index
  long n = atol(argv[3]);       // number of addresses
  if (s < 0 || n < 0) {
    shell_print(shell, "invalid index or numbers");
    return -1;
  }

  for (uint32_t idx = s; idx < s + n; idx++) {
    dump_address(w_ctx, idx, change);
  }
  return 0;
}

static int cmd_balance(const struct shell *shell, size_t argc, char **argv) {
  uint64_t value = 0;
  byte_t addr[IOTA_ADDRESS_BYTES] = {};

  if (argc == 2) {
    // an address hash
    if (strncmp(argv[1], w_ctx->bech32HRP, strlen(w_ctx->bech32HRP)) != 0) {
      shell_print(shell, "invalid address hash");
      return -1;
    }

    if (get_balance(&w_ctx->endpoint, true, argv[1], &value) != 0) {
      shell_print(shell, "get balance failed");
      return -1;
    } else {
      shell_print(shell, "balance: %" PRIu64, value);
    }

  } else {
    bool change = (bool)argv[1];  // address type: 0 or 1
    long s = atol(argv[2]);       // starting index
    long n = atol(argv[3]);       // number of addresses
    if (s < 0 || n < 0) {
      shell_print(shell, "invalid index or numbers");
      return -1;
    }

    for (uint32_t idx = s; idx < s + n; idx++) {
      if (wallet_balance_by_index(w_ctx, change, idx, &value) != 0) {
        LOG_ERR("wallet get balance [%" PRIu32 "]failed\n", idx);
        break;
      }
      dump_address(w_ctx, idx, change);
      printf("balance: %" PRIu64 "\n", value);
    }
  }
  return 0;
}

static int cmd_send(const struct shell *shell, size_t argc, char **argv) {
  char msg_id[IOTA_MESSAGE_ID_HEX_BYTES + 1] = {};
  char const data[] = "sent from iota.c";
  byte_t recv_addr[IOTA_ADDRESS_BYTES] = {};
  long addr_index = atol(argv[1]);      // address index
  char *bech32_addr = (char *)argv[2];  // receiver address in bech32 format
  long value = atol(argv[3]);           // tokens

  if (addr_index < 0 || value <= 0) {
    shell_print(shell, "invalid index or token value");
    return -1;
  }

  if (strncmp(bech32_addr, w_ctx->bech32HRP, strlen(w_ctx->bech32HRP)) != 0) {
    shell_print(shell, "invalid address hash");
    return -1;
  }

  if (strlen(bech32_addr) != IOTA_ADDRESS_HEX_BYTES) {
    shell_print(shell, "invalid address length");
    return -1;
  }

  if (address_from_bech32(w_ctx->bech32HRP, bech32_addr, recv_addr) != 0) {
    shell_print(shell, "convert address to binary failed");
    return -1;
  }

  shell_print(shell, "Sending %ld Mi to %s", value, bech32_addr);
  value = value * 1000000;  // Mi

  // send from address that change is 0
  int err = wallet_send(w_ctx, false, (uint32_t)addr_index, recv_addr + 1, value, "ZephyrWallet", (byte_t *)data,
                        sizeof(data), msg_id, sizeof(msg_id));
  if (err) {
    shell_print(shell, "send transaction failed");
    return -1;
  }

  shell_print(shell, "Message HASH: %s", msg_id);
  return 0;
}

/* Creating subcommands (level 1 command) array for command "iota". */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_iota, SHELL_CMD(info, NULL, "Display node info", cmd_info),
                               SHELL_CMD_ARG(data, NULL,
                                             "Send data message.\n "
                                             "\tEx: iota data \"my ID\" \"hello iota\"",
                                             cmd_data, 3, 0),
                               SHELL_CMD_ARG(addr, NULL,
                                             "Get addresses from a given range.\n "
                                             "\tiota addr [is_change] [start_index] [count]\n "
                                             "\tEx: iota addr 0 0 5",
                                             cmd_addr, 4, 0),
                               SHELL_CMD_ARG(balance, NULL,
                                             "Get balance from an address or a given range.\n "
                                             "\tiota balance [is_change] [start_index] [count]\n "
                                             "\tiota balance [bech32_address]\n "
                                             "\tEx: iota balance 0 0 5\n "
                                             "\tiota balance atoi1qzdglt68p...xtav6e82tp",
                                             cmd_balance, 2, 2),
                               SHELL_CMD_ARG(send, NULL,
                                             "send value transaction to an address\n "
                                             "\tiota send [addr_index] [recv_bech32_addr] [amount]\n "
                                             "\tEx: iota send 0 atoi1qzdglt68p...xtav6e82tp 3",
                                             cmd_send, 4, 0),
                               SHELL_SUBCMD_SET_END);
/* Creating root (level 0) command "iota" without a handler */
SHELL_CMD_REGISTER(iota, &sub_iota, "IOTA client demo commands", NULL);

static int init_http_client_conf(iota_wallet_t *w) {
  char const *const http_prefix = "http://";
  char const *const tls_prefix = "https://";
  char const *const url = CONFIG_IOTA_NODE_URL;

  if (strncmp(url, http_prefix, strlen(http_prefix)) == 0) {
    size_t s = strlen(http_prefix);
    size_t len = strlen(url) - s;
    if (len > sizeof(w->endpoint.host)) {
      LOG_ERR("hostname: %s is too long", url + s);
      return -1;
    }

    memcpy(w->endpoint.host, url + s, len);
    w->endpoint.host[len] = '\0';

    w->endpoint.use_tls = false;
  } else if (strncmp(url, tls_prefix, strlen(tls_prefix)) == 0) {
    size_t s = strlen(tls_prefix);
    size_t len = strlen(url) - s;
    if (len > sizeof(w->endpoint.host)) {
      LOG_ERR("hostname: %s is too long", url + s);
      return -1;
    }

    memcpy(w->endpoint.host, url + s, len);
    w->endpoint.host[len] = '\0';

    w->endpoint.use_tls = true;
  } else {
    LOG_ERR("Err: Invalid URL format: %s", url);
    return -1;
  }
  w->endpoint.port = CONFIG_IOTA_NODE_PORT;
  LOG_INF("Node: %s%s:%d", w->endpoint.use_tls ? tls_prefix : http_prefix, w->endpoint.host, w->endpoint.port);
  return 0;
}

int init_wallet() {
  // buffer holds ramdom mnemonic sentence
  char ms_buf[256] = {};
  LOG_DBG(" ");

  // validating seed config
  if (strcmp(CONFIG_WALLET_MNEMONIC, "RANDOM") == 0) {
    printf("Generating new mnemonic sentence...\n");
    mnemonic_generator(MS_ENTROPY_256, MS_LAN_EN, ms_buf, sizeof(ms_buf));
    printf("###\n%s\n###\n", ms_buf);
    // init wallet with account index 0
    if ((w_ctx = wallet_create(ms_buf, "", 0)) == NULL) {
      LOG_ERR("create wallet failed with random mnemonic");
      return -1;
    }
  } else {
    if ((w_ctx = wallet_create(CONFIG_WALLET_MNEMONIC, "", 0)) == NULL) {
      LOG_ERR("create wallet failed with default mnemonic");
      return -1;
    }
  }

  // http client setup
  if (init_http_client_conf(w_ctx) != 0) {
    LOG_ERR("http client configuration failed");
    wallet_destroy(w_ctx);
    return -1;
  }

  if (wallet_update_bech32HRP(w_ctx) != 0) {
    LOG_ERR("update bech32HRP failed");
    wallet_destroy(w_ctx);
    return -1;
  }

  printf("wallet initialized...OK\n");
  return 0;
}
