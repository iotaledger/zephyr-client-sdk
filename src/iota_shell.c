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
#include "client/network/http.h"

#include "client/api/v1/get_node_info.h"
#include "client/api/v1/send_message.h"
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

static void dump_address(iota_wallet_t *w, uint32_t index) {
  byte_t addr_wit_version[IOTA_ADDRESS_BYTES] = {};
  char tmp_bech32_addr[100] = {};

  addr_wit_version[0] = ADDRESS_VER_ED25519;
  wallet_address_by_index(w, index, addr_wit_version + 1);
  address_2_bech32(addr_wit_version, w->bech32HRP, tmp_bech32_addr);
  printf("Addr[%" PRIu32 "]\n", index);
  // print ed25519 address without version filed.
  printf("\t");
  dump_hex_str(addr_wit_version + 1, ED25519_ADDRESS_BYTES);
  // print out
  printf("\t%s\n", tmp_bech32_addr);
}

static int cmd_addr(const struct shell *shell, size_t argc, char **argv) {
  long s = atol(argv[1]);  // starting index
  long n = atol(argv[2]);  // number of addresses
  if (s < 0 || n < 0) {
    shell_print(shell, "invalid index or numbers");
    return -1;
  }

  for (uint32_t idx = s; idx < n; idx++) {
    dump_address(w_ctx, idx);
  }
  return 0;
}

static int cmd_balance(const struct shell *shell, size_t argc, char **argv) {
  long s = atol(argv[1]);  // starting index
  long n = atol(argv[2]);  // number of addresses
  if (s < 0 || n < 0) {
    shell_print(shell, "invalid index or numbers");
    return -1;
  }

  uint64_t value = 0;
  for (uint32_t idx = s; idx < n; idx++) {
    if (wallet_balance_by_index(w_ctx, idx, &value) != 0) {
      LOG_ERR("wallet get balance [%" PRIu32 "]failed\n", idx);
      break;
    }
    dump_address(w_ctx, idx);
    printf("balance: %" PRIu64 "\n", value);
  }

  return 0;
}

/* Creating subcommands (level 1 command) array for command "iota". */
SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_iota, SHELL_CMD(info, NULL, "Display node info", cmd_info),
    SHELL_CMD_ARG(data, NULL, "Send data message.\n Ex: iota data \"my ID\" \"hello iota\"", cmd_data, 3, 0),
    SHELL_CMD_ARG(addr, NULL, "Get addresses from a given range.\n Ex: iota addr 0 5", cmd_addr, 3, 0),
    SHELL_CMD_ARG(balance, NULL, "Get balance from a given range.\n Ex: iota balance 0 5", cmd_balance, 3, 0),
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
  byte_t seed[IOTA_SEED_BYTES] = {};
  LOG_DBG(" ");

  // validating seed config
  if (strcmp(CONFIG_WALLET_SEED, "RANDOM") != 0) {
    if (strlen(CONFIG_WALLET_SEED) != 64) {
      LOG_ERR("seed length is %d, should be 64", strlen(CONFIG_WALLET_SEED));
      return -1;
    }
    if (hex2bin(CONFIG_WALLET_SEED, strlen(CONFIG_WALLET_SEED), seed, sizeof(seed)) == 0) {
      LOG_ERR("convert seed to binary failed");
      return -1;
    }
  } else {
    random_seed(seed);
  }

  // setup wallet
  w_ctx = wallet_create(seed, CONFIG_WALLET_ADDR_PATH);
  if (!w_ctx) {
    LOG_ERR("Creating wallet object failed");
    return -1;
  }

  // http client setup
  if (init_http_client_conf(w_ctx) != 0) {
    wallet_destroy(w_ctx);
    return -1;
  }

  if (wallet_update_bech32HRP(w_ctx) != 0) {
    LOG_ERR("update bech32HRP failed");
    wallet_destroy(w_ctx);
    return -1;
  }

  LOG_DBG("init wallet done");
  return 0;
}
