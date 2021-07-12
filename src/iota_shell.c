// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <inttypes.h>
#include <shell/shell.h>
#include <stdio.h>
#include <zephyr.h>

#include <net/http_client.h>
#include <net/socket.h>
#include "client/network/http.h"

#include "client/api/v1/get_node_info.h"
#include "client/api/v1/send_message.h"

static iota_client_conf_t http_conf;

static int cmd_info(const struct shell *shell, size_t argc, char **argv) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  res_node_info_t *info = res_node_info_new();
  if (get_node_info(&http_conf, info) == 0) {
    if (info->is_error == false) {
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
  int err = send_indexation_msg(&http_conf, argv[1], argv[2], &res);
  if (err) {
    if (res.is_error) {
      printf("Err response: %s\n", res.u.error->msg);
      res_err_free(res.u.error);
    }
    printf("send indexation failed\n");
  } else {
    printf("message ID: %s\n", res.u.msg_id);
  }
  return 0;
}

/* Creating subcommands (level 1 command) array for command "iota". */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_iota, SHELL_CMD(info, NULL, "Display node info", cmd_info),
                               SHELL_CMD_ARG(data, NULL, "Send data message.\n Ex: iota data \"my ID\" \"hello iota\"",
                                             cmd_data, 3, 0),
                               SHELL_SUBCMD_SET_END);
/* Creating root (level 0) command "iota" without a handler */
SHELL_CMD_REGISTER(iota, &sub_iota, "IOTA client demo commands", NULL);

int init_http_client_conf() {
  char const *const http_prefix = "http://";
  char const *const tls_prefix = "https://";
  char const *const url = CONFIG_IOTA_NODE_URL;

  memset(&http_conf, 0, sizeof(http_conf));

  if (strncmp(url, http_prefix, strlen(http_prefix)) == 0) {
    size_t s = strlen(http_prefix);
    size_t len = strlen(url) - s;
    if (len > sizeof(http_conf.host)) {
      return -1;
    }

    memcpy(http_conf.host, url + s, len);
    http_conf.host[len] = '\0';

    http_conf.use_tls = false;
  } else if (strncmp(url, tls_prefix, strlen(tls_prefix)) == 0) {
    size_t s = strlen(tls_prefix);
    size_t len = strlen(url) - s;
    if (len > sizeof(http_conf.host)) {
      return -1;
    }

    memcpy(http_conf.host, url + s, len);
    http_conf.host[len] = '\0';

    http_conf.use_tls = true;
  } else {
    printf("Err: Invalid URL format: %s\n", url);
    return -1;
  }
  http_conf.port = CONFIG_IOTA_NODE_PORT;
  printf("Node: %s%s:%d\n", http_conf.use_tls ? tls_prefix : http_prefix, http_conf.host, http_conf.port);
  return 0;
}
