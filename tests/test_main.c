
// SPDX-License-Identifier: Apache-2.0

#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <ztest.h>

#include "blake2b_data.h"
#include "core/models/message.h"
#include "core/models/payloads/transaction.h"
#include "core/utils/byte_buffer.h"
#include "crypto/iota_crypto.h"

// BLAKE2 hash function
// test vectors: https://github.com/BLAKE2/BLAKE2/tree/master/testvectors
static void test_blake2b(void) {
  uint8_t msg[256] = {};
  uint8_t out_512[64] = {};
  uint8_t out_256[32] = {};

  for (size_t i = 0; i < sizeof(msg); i++) {
    msg[i] = i;
  }

  for (size_t i = 0; i < sizeof(msg); i++) {
    zassert_ok(iota_blake2b_sum(msg, i, out_256, sizeof(out_256)), "");
    zassert_ok(iota_blake2b_sum(msg, i, out_512, sizeof(out_512)), "");
    zassert_mem_equal(blake2b_256[i], out_256, sizeof(out_256), "");
    zassert_mem_equal(blake2b_512[i], out_512, sizeof(out_512), "");
  }
}

// HMAC-SHA-256 and HMAC-SHA-512
// test vectors: https://tools.ietf.org/html/rfc4231#section-4.2
static void test_HMACSHA(void) {
  uint8_t tmp_sha256_out[32] = {};
  uint8_t tmp_sha512_out[64] = {};
  // Test case 1
  uint8_t key_1[32];
  // 20-bytes 0x0b
  memset(key_1, 0x0b, 20);
  memset(key_1 + 20, 0x00, 32 - 20);
  // "Hi There"
  uint8_t data_1[] = {0x48, 0x69, 0x20, 0x54, 0x68, 0x65, 0x72, 0x65};
  uint8_t exp_sha256_out1[] = {0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53, 0x5c, 0xa8, 0xaf,
                               0xce, 0xaf, 0x0b, 0xf1, 0x2b, 0x88, 0x1d, 0xc2, 0x00, 0xc9, 0x83,
                               0x3d, 0xa7, 0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32, 0xcf, 0xf7};
  uint8_t exp_sha512_out1[] = {0x87, 0xaa, 0x7c, 0xde, 0xa5, 0xef, 0x61, 0x9d, 0x4f, 0xf0, 0xb4, 0x24, 0x1a,
                               0x1d, 0x6c, 0xb0, 0x23, 0x79, 0xf4, 0xe2, 0xce, 0x4e, 0xc2, 0x78, 0x7a, 0xd0,
                               0xb3, 0x05, 0x45, 0xe1, 0x7c, 0xde, 0xda, 0xa8, 0x33, 0xb7, 0xd6, 0xb8, 0xa7,
                               0x02, 0x03, 0x8b, 0x27, 0x4e, 0xae, 0xa3, 0xf4, 0xe4, 0xbe, 0x9d, 0x91, 0x4e,
                               0xeb, 0x61, 0xf1, 0x70, 0x2e, 0x69, 0x6c, 0x20, 0x3a, 0x12, 0x68, 0x54};

  zassert_ok(iota_crypto_hmacsha256(key_1, data_1, sizeof(data_1), tmp_sha256_out), "");
  zassert_mem_equal(exp_sha256_out1, tmp_sha256_out, sizeof(exp_sha256_out1), "");

  zassert_ok(iota_crypto_hmacsha512(key_1, data_1, sizeof(data_1), tmp_sha512_out), "");
  zassert_mem_equal(exp_sha512_out1, tmp_sha512_out, sizeof(exp_sha512_out1), "");

  // Test case 2
  // "Jefe"
  uint8_t key_2[32] = {0x4a, 0x65, 0x66, 0x65};
  // "what do ya want for nothing?""
  uint8_t data_2[] = {0x77, 0x68, 0x61, 0x74, 0x20, 0x64, 0x6f, 0x20, 0x79, 0x61, 0x20, 0x77, 0x61, 0x6e,
                      0x74, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x6e, 0x6f, 0x74, 0x68, 0x69, 0x6e, 0x67, 0x3f};
  uint8_t exp_sha256_out2[] = {0x5b, 0xdc, 0xc1, 0x46, 0xbf, 0x60, 0x75, 0x4e,
                               0x6a, 0x04, 0x24, 0x26, 0x08, 0x95, 0x75, 0xc7};
  uint8_t exp_sha512_out2[] = {0x16, 0x4b, 0x7a, 0x7b, 0xfc, 0xf8, 0x19, 0xe2, 0xe3, 0x95, 0xfb, 0xe7, 0x3b,
                               0x56, 0xe0, 0xa3, 0x87, 0xbd, 0x64, 0x22, 0x2e, 0x83, 0x1f, 0xd6, 0x10, 0x27,
                               0x0c, 0xd7, 0xea, 0x25, 0x05, 0x54, 0x97, 0x58, 0xbf, 0x75, 0xc0, 0x5a, 0x99,
                               0x4a, 0x6d, 0x03, 0x4f, 0x65, 0xf8, 0xf0, 0xe6, 0xfd, 0xca, 0xea, 0xb1, 0xa3,
                               0x4d, 0x4a, 0x6b, 0x4b, 0x63, 0x6e, 0x07, 0x0a, 0x38, 0xbc, 0xe7, 0x37};

  zassert_ok(iota_crypto_hmacsha256(key_2, data_2, sizeof(data_2), tmp_sha256_out), "");
  zassert_mem_equal(exp_sha256_out2, tmp_sha256_out, sizeof(exp_sha256_out2), "");

  zassert_ok(iota_crypto_hmacsha512(key_2, data_2, sizeof(data_2), tmp_sha512_out), "");
  zassert_mem_equal(exp_sha512_out2, tmp_sha512_out, sizeof(exp_sha512_out2), "");
}

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
  zassert_ok(hex_2_bin("e57fb750f3a3a67969ece5bd9ae7eef5b2256a818b2aac458941f7274985a410", IOTA_SEED_HEX_BYTES, seed,
                       IOTA_SEED_BYTES),
             "");

  zassert_ok(address_from_path(seed, "m/44'/4218'/0'/0'/0'", addr_from_path), "");
  // dump_hex(addr_from_path, ED25519_ADDRESS_BYTES);

  // ed25519 address to IOTA address
  addr_with_ver[0] = ADDRESS_VER_ED25519;
  memcpy(addr_with_ver + 1, addr_from_path, ED25519_ADDRESS_BYTES);
  // dump_hex(addr_with_ver, IOTA_ADDRESS_BYTES);
  zassert_mem_equal(exp_addr, addr_with_ver, IOTA_ADDRESS_BYTES, "");

  // convert binary address to bech32 with iot HRP
  zassert_ok(address_2_bech32(addr_with_ver, "iot", bech32_addr), "");
  zassert_mem_equal(exp_iot_bech32, bech32_addr, strlen(exp_iot_bech32), "");
  // printf("bech32 [iot]: %s\n", bech32_addr);
  // bech32 to binary address
  zassert_ok(address_from_bech32("iot", bech32_addr, addr_from_bech32), "");
  zassert_mem_equal(addr_with_ver, addr_from_bech32, IOTA_ADDRESS_BYTES, "");

  // convert binary address to bech32 with iota HRP
  zassert_ok(address_2_bech32(addr_with_ver, "iota", bech32_addr), "");
  zassert_mem_equal(exp_iota_bech32, bech32_addr, strlen(exp_iota_bech32), "");
  // printf("bech32 [iota]: %s\n", bech32_addr);
  // bech32 to binary address
  zassert_ok(address_from_bech32("iota", bech32_addr, addr_from_bech32), "");
  zassert_mem_equal(addr_with_ver, addr_from_bech32, IOTA_ADDRESS_BYTES, "");

  // address from ed25519 keypair
  iota_keypair_t seed_keypair = {};
  byte_t ed_addr[ED25519_ADDRESS_BYTES] = {};

  // address from ed25519 public key
  iota_crypto_keypair(seed, &seed_keypair);
  zassert_ok(address_from_ed25519_pub(seed_keypair.pub, ed_addr), "");
  zassert_mem_equal(exp_ed_addr, ed_addr, ED25519_ADDRESS_BYTES, "");
}

static void test_message_with_tx(void) {
  byte_t tx_id0[TRANSACTION_ID_BYTES] = {};
  byte_t addr0[ED25519_ADDRESS_BYTES] = {0x51, 0x55, 0x82, 0xfe, 0x64, 0x8b, 0x0f, 0x10, 0xa2, 0xb2, 0xa1,
                                         0xb9, 0x1d, 0x75, 0x02, 0x19, 0x0c, 0x97, 0x9b, 0xaa, 0xbf, 0xee,
                                         0x85, 0xb6, 0xbb, 0xb5, 0x02, 0x06, 0x92, 0xe5, 0x5d, 0x16};
  byte_t addr1[ED25519_ADDRESS_BYTES] = {0x69, 0x20, 0xb1, 0x76, 0xf6, 0x13, 0xec, 0x7b, 0xe5, 0x9e, 0x68,
                                         0xfc, 0x68, 0xf5, 0x97, 0xeb, 0x33, 0x93, 0xaf, 0x80, 0xf7, 0x4c,
                                         0x7c, 0x3d, 0xb7, 0x81, 0x98, 0x14, 0x7d, 0x5f, 0x1f, 0x92};

  iota_keypair_t seed_keypair = {};
  zassert_ok(hex_2_bin("f7868ab6bb55800b77b8b74191ad8285a9bf428ace579d541fda47661803ff44", 64, seed_keypair.pub,
                       ED_PUBLIC_KEY_BYTES),
             "");
  zassert_ok(hex_2_bin("256a818b2aac458941f7274985a410e57fb750f3a3a67969ece5bd9a"
                       "e7eef5b2f7868ab6bb55800b77b8b74191ad8285"
                       "a9bf428ace579d541fda47661803ff44",
                       128, seed_keypair.priv, ED_PRIVATE_KEY_BYTES),
             "");

  core_message_t *msg = core_message_new();
  zassert_not_null(msg, "");

  transaction_payload_t *tx = tx_payload_new();
  zassert_not_null(tx, "");

  zassert_ok(tx_payload_add_input_with_key(tx, tx_id0, 0, seed_keypair.pub, seed_keypair.priv), "");
  zassert_ok(tx_payload_add_output(tx, OUTPUT_SINGLE_OUTPUT, addr0, 1000), "");
  zassert_ok(tx_payload_add_output(tx, OUTPUT_SINGLE_OUTPUT, addr1, 2779530283276761), "");

  // put tx payload into message
  msg->payload_type = 0;
  msg->payload = tx;

  zassert_ok(core_message_sign_transaction(msg), "");

  // tx_payload_print(tx);

  // free message and sub entities
  core_message_free(msg);
}

static void tx_essence_serialization(void) {
  byte_t exp_essence_byte[128] = {
      0x0,  0x1,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
      0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
      0x2,  0x0,  0x0,  0x0,  0x51, 0x55, 0x82, 0xFE, 0x64, 0x8B, 0xF,  0x10, 0xA2, 0xB2, 0xA1, 0xB9, 0x1D, 0x75, 0x2,
      0x19, 0xC,  0x97, 0x9B, 0xAA, 0xBF, 0xEE, 0x85, 0xB6, 0xBB, 0xB5, 0x2,  0x6,  0x92, 0xE5, 0x5D, 0x16, 0xE8, 0x3,
      0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x69, 0x20, 0xB1, 0x76, 0xF6, 0x13, 0xEC, 0x7B, 0xE5, 0x9E, 0x68,
      0xFC, 0x68, 0xF5, 0x97, 0xEB, 0x33, 0x93, 0xAF, 0x80, 0xF7, 0x4C, 0x7C, 0x3D, 0xB7, 0x81, 0x98, 0x14, 0x7D, 0x5F,
      0x1F, 0x92, 0xD9, 0x59, 0x2D, 0xD3, 0xF7, 0xDF, 0x9,  0x0,  0x0,  0x0,  0x0,  0x0};
  static byte_t addr_a[ED25519_ADDRESS_BYTES] = {0x51, 0x55, 0x82, 0xfe, 0x64, 0x8b, 0x0f, 0x10, 0xa2, 0xb2, 0xa1,
                                                 0xb9, 0x1d, 0x75, 0x02, 0x19, 0x0c, 0x97, 0x9b, 0xaa, 0xbf, 0xee,
                                                 0x85, 0xb6, 0xbb, 0xb5, 0x02, 0x06, 0x92, 0xe5, 0x5d, 0x16};
  static byte_t addr_b[ED25519_ADDRESS_BYTES] = {0x69, 0x20, 0xb1, 0x76, 0xf6, 0x13, 0xec, 0x7b, 0xe5, 0x9e, 0x68,
                                                 0xfc, 0x68, 0xf5, 0x97, 0xeb, 0x33, 0x93, 0xaf, 0x80, 0xf7, 0x4c,
                                                 0x7c, 0x3d, 0xb7, 0x81, 0x98, 0x14, 0x7d, 0x5f, 0x1f, 0x92};
  static byte_t tx_id_empty[TRANSACTION_ID_BYTES] = {};

  transaction_essence_t *essence = tx_essence_new();
  zassert_not_null(essence, "");
  zassert_ok(tx_essence_serialize_length(essence), "");

  // add inputs
  zassert_ok(tx_essence_add_input(essence, tx_id_empty, 0), "");
  zassert_equal(1, utxo_inputs_count(&essence->inputs), "");

  // add outputs
  zassert_ok(tx_essence_add_output(essence, OUTPUT_SINGLE_OUTPUT, addr_a, 1000), "");
  zassert_ok(tx_essence_add_output(essence, OUTPUT_SINGLE_OUTPUT, addr_b, 2779530283276761), "");
  zassert_equal(2, utxo_outputs_count(&essence->outputs), "");

  // get serialized length
  size_t essence_buf_len = tx_essence_serialize_length(essence);
  zassert_not_equal(0, essence_buf_len, "");

  byte_t *essence_buf = (byte_t *)malloc(essence_buf_len);
  zassert_not_null(essence_buf, "");
  zassert_true(tx_essence_serialize(essence, essence_buf) == essence_buf_len, "");
  // dump_hex(essence_buf, essence_buf_len);
  zassert_mem_equal(exp_essence_byte, essence_buf, sizeof(exp_essence_byte), "");
  free(essence_buf);
  tx_essence_free(essence);
}

//=======================Benchmark==================================

#define ADDR_NUMS 100

static void bench_address_generating(void) {
  char path_buf[128] = {};
  byte_t seed[IOTA_SEED_BYTES] = {};
  byte_t ed_addr[IOTA_ADDRESS_BYTES] = {};
  char bech32_addr[IOTA_ADDRESS_HEX_BYTES + 1] = {};
  size_t ret_size = 0;
  int64_t start_time = 0, time_spent = 0;
  int64_t min = 0, max = 0, sum = 0;
  random_seed(seed);

  for (size_t idx = 0; idx < ADDR_NUMS; idx++) {
    ret_size = snprintf(path_buf, 128, "m/44'/4218'/0'/0'/%u'", idx);
    if (ret_size >= 128) {
      path_buf[128 - 1] = '\0';
    }
    ed_addr[0] = 0;
    start_time = k_uptime_get();
    if (address_from_path(seed, path_buf, ed_addr + 1) == 0) {
      if (address_2_bech32(ed_addr, "iota", bech32_addr) != 0) {
        printf("convert to bech32 failed\n");
        break;
      }
      time_spent = k_uptime_delta(&start_time);
      max = (idx == 0 || time_spent > max) ? time_spent : max;
      min = (idx == 0 || time_spent < min) ? time_spent : min;
      sum += time_spent;
      // printf("%u: %"PRId64", max %"PRId64", min %"PRId64"\n", idx, time_spent, max, min);
    } else {
      printf("drive from path failed\n");
      break;
    }
  }
  printf("Bench %d address generation\n\tmin(ms)\tmax(ms)\tavg(ms)\ttotal(ms)\n", ADDR_NUMS);
  printf("\t%" PRId64 "\t%" PRId64 "\t%.2f\t%" PRId64 "\n", min, max, (sum / (float)ADDR_NUMS), sum);
}

void test_main(void) {
  printf("====Unit Test on %s====\n", CONFIG_BOARD);

  // clang-format off
  ztest_test_suite(iota_crypto,
    ztest_unit_test(test_blake2b),
    ztest_unit_test(test_HMACSHA));
  // clang-format on

  ztest_run_test_suite(iota_crypto);

  // clang-format off
  ztest_test_suite(iota_core,
    ztest_unit_test(test_address_gen),
    ztest_unit_test(test_message_with_tx),
    ztest_unit_test(tx_essence_serialization));
  // clang-format on

  ztest_run_test_suite(iota_core);

  // clang-format off
  ztest_test_suite(iota_bench,
    ztest_unit_test(bench_address_generating));
  // clang-format on

  ztest_run_test_suite(iota_bench);
}
