// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <shell/shell.h>
#include <zephyr.h>

static int cmd_demo_ping(const struct shell *shell, size_t argc, char **argv) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(shell, "pong");
  return 0;
}

static int cmd_demo_params(const struct shell *shell, size_t argc, char **argv) {
  int cnt;

  shell_print(shell, "argc = %d", argc);
  for (cnt = 0; cnt < argc; cnt++) {
    shell_print(shell, "  argv[%d] = %s", cnt, argv[cnt]);
  }
  return 0;
}

/* Creating subcommands (level 1 command) array for command "demo". */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_iota, SHELL_CMD(params, NULL, "Print params command.", cmd_demo_params),
                               SHELL_CMD(ping, NULL, "Ping command.", cmd_demo_ping), SHELL_SUBCMD_SET_END);
/* Creating root (level 0) command "demo" without a handler */
SHELL_CMD_REGISTER(iota, &sub_iota, "IOTA commands", NULL);
