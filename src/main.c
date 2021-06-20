// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/printk.h>
#include <zephyr.h>

#include "core/address.h"
#include "core/utils/byte_buffer.h"
#include "crypto/iota_crypto.h"

static void test_address_gen(void) {
  char const *const exp_iot_bech32 = "iot1qpg4tqh7vj9s7y9zk2smj8t4qgvse9um42l7apdkhw6syp5ju4w3v6ffg6n";
  char const *const exp_iota_bech32 = "iota1qpg4tqh7vj9s7y9zk2smj8t4qgvse9um42l7apdkhw6syp5ju4w3v79tf3l";
  byte_t exp_addr[IOTA_ADDRESS_BYTES] = {0x00, 0x51, 0x55, 0x82, 0xfe, 0x64, 0x8b, 0xf,  0x10, 0xa2, 0xb2,
                                         0xa1, 0xb9, 0x1d, 0x75, 0x2,  0x19, 0xc,  0x97, 0x9b, 0xaa, 0xbf,
                                         0xee, 0x85, 0xb6, 0xbb, 0xb5, 0x2,  0x6,  0x92, 0xe5, 0x5d, 0x16};
  byte_t exp_ed_addr[ED25519_ADDRESS_BYTES] = {0x4d, 0xbc, 0x7b, 0x45, 0x32, 0x46, 0x64, 0x20, 0x9a, 0xe5, 0x59,
                                               0xcf, 0xd1, 0x73, 0xc,  0xb,  0xb1, 0x90, 0x5a, 0x7f, 0x83, 0xe6,
                                               0x5d, 0x48, 0x37, 0xa9, 0x87, 0xe2, 0x24, 0xc1, 0xc5, 0x1e};

  byte_t seed[IOTA_SEED_BYTES] = {};
  byte_t addr_from_path[ED25519_ADDRESS_BYTES] = {};
  char bech32_addr[128] = {};
  byte_t addr_with_ver[IOTA_ADDRESS_BYTES] = {};
  byte_t addr_from_bech32[IOTA_ADDRESS_BYTES] = {};

  // convert seed from hex string to binary
  hex_2_bin("e57fb750f3a3a67969ece5bd9ae7eef5b2256a818b2aac458941f7274985a410", IOTA_SEED_HEX_BYTES, seed,
            IOTA_SEED_BYTES);

  address_from_path(seed, "m/44'/4218'/0'/0'/0'", addr_from_path);
  // dump_hex(addr_from_path, ED25519_ADDRESS_BYTES);

  // ed25519 address to IOTA address
  addr_with_ver[0] = ADDRESS_VER_ED25519;
  memcpy(addr_with_ver + 1, addr_from_path, ED25519_ADDRESS_BYTES);
  // dump_hex(addr_with_ver, IOTA_ADDRESS_BYTES);
  // zassert_mem_equal(exp_addr, addr_with_ver, IOTA_ADDRESS_BYTES, "");

  // convert binary address to bech32 with iot HRP
  address_2_bech32(addr_with_ver, "iot", bech32_addr);
  // zassert_mem_equal(exp_iot_bech32, bech32_addr, strlen(exp_iot_bech32), "");
  printf("bech32 [iot]: %s\n", bech32_addr);
  // bech32 to binary address
  address_from_bech32("iot", bech32_addr, addr_from_bech32);
  // zassert_mem_equal(addr_with_ver, addr_from_bech32, IOTA_ADDRESS_BYTES, "");

  // convert binary address to bech32 with iota HRP
  address_2_bech32(addr_with_ver, "iota", bech32_addr);
  // zassert_mem_equal(exp_iota_bech32, bech32_addr, strlen(exp_iota_bech32), "");
  printf("bech32 [iota]: %s\n", bech32_addr);
  // bech32 to binary address
  address_from_bech32("iota", bech32_addr, addr_from_bech32);
  // zassert_mem_equal(addr_with_ver, addr_from_bech32, IOTA_ADDRESS_BYTES, "");

  // address from ed25519 keypair
  iota_keypair_t seed_keypair = {};
  byte_t ed_addr[ED25519_ADDRESS_BYTES] = {};

  // address from ed25519 public key
  iota_crypto_keypair(seed, &seed_keypair);
  address_from_ed25519_pub(seed_keypair.pub, ed_addr);
  // zassert_mem_equal(exp_ed_addr, ed_addr, ED25519_ADDRESS_BYTES, "");
}

void main(void) {
  printf("Hello World! %s\n", CONFIG_BOARD);

  test_address_gen();
}
