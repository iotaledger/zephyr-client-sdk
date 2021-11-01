#[[
// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0
]]

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(iota_app)

# the directory of external libraries
set(EXT_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")

set(EXT_LIB_SRC
    "${EXT_LIB_DIR}/cjson/cJSON.c" "${EXT_LIB_DIR}/blake2/blake2b-ref.c"
    "${EXT_LIB_DIR}/ed25519-donna/ed25519.c")

set(EXT_LIB_INC "${EXT_LIB_DIR}/cjson" "${EXT_LIB_DIR}/blake2"
                "${EXT_LIB_DIR}/ed25519-donna" "${EXT_LIB_DIR}/uthash")

# the directory of iota client source
set(IOTA_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/iota-c/src")

# iota crypto implementation
set(IOTA_CRYPTO "${IOTA_SRC_DIR}/crypto/iota_crypto.c")

# iota protocol implementation
set(IOTA_CORE
    "${IOTA_SRC_DIR}/core/address.c"
    "${IOTA_SRC_DIR}/core/seed.c"
    "${IOTA_SRC_DIR}/core/models/inputs/utxo_input.c"
    "${IOTA_SRC_DIR}/core/models/outputs/outputs.c"
    "${IOTA_SRC_DIR}/core/models/payloads/indexation.c"
    "${IOTA_SRC_DIR}/core/models/payloads/transaction.c"
    "${IOTA_SRC_DIR}/core/models/message.c"
    "${IOTA_SRC_DIR}/core/utils/bech32.c"
    "${IOTA_SRC_DIR}/core/utils/byte_buffer.c"
    "${IOTA_SRC_DIR}/core/utils/iota_str.c"
    "${IOTA_SRC_DIR}/core/utils/slip10.c")

# iota client API implementation
set(IOTA_CLIENT
    "${IOTA_SRC_DIR}/client/network/http_zephyr.c"
    "${IOTA_SRC_DIR}/client/api/json_utils.c"
    "${IOTA_SRC_DIR}/client/api/message_builder.c"
    "${IOTA_SRC_DIR}/client/api/message.c"
    "${IOTA_SRC_DIR}/client/api/v1/find_message.c"
    "${IOTA_SRC_DIR}/client/api/v1/get_balance.c"
    "${IOTA_SRC_DIR}/client/api/v1/get_health.c"
    "${IOTA_SRC_DIR}/client/api/v1/get_message_children.c"
    "${IOTA_SRC_DIR}/client/api/v1/get_message_metadata.c"
    "${IOTA_SRC_DIR}/client/api/v1/get_message.c"
    "${IOTA_SRC_DIR}/client/api/v1/get_node_info.c"
    "${IOTA_SRC_DIR}/client/api/v1/get_output.c"
    "${IOTA_SRC_DIR}/client/api/v1/get_outputs_from_address.c"
    "${IOTA_SRC_DIR}/client/api/v1/get_tips.c"
    "${IOTA_SRC_DIR}/client/api/v1/response_error.c"
    "${IOTA_SRC_DIR}/client/api/v1/send_message.c")

# simple wallet reference
set(IOTA_WALLET "${IOTA_SRC_DIR}/wallet/wallet.c")

if(CONFIG_ZTEST)
  message("Unit tests is enabled")
  # unit test application, testing iota protocol only
  target_sources(app PRIVATE "tests/test_main.c" ${EXT_LIB_SRC} ${IOTA_CRYPTO}
                             ${IOTA_CORE})
else()
  # add TLS certification
  set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)
  generate_inc_file_for_target(app "src/app-cert.der"
                               "${gen_dir}/app-cert.der.inc")

  # build a full application
  target_sources(
    app
    PRIVATE "src/main.c"
            "src/iota_shell.c"
            "src/wifi.c"
            ${EXT_LIB_SRC}
            ${IOTA_CRYPTO}
            ${IOTA_CORE}
            ${IOTA_CLIENT}
            ${IOTA_WALLET})
endif()

# configure ed25519-donna
target_compile_definitions(
  app
  # ed25519-donna
  PRIVATE ED25519_CUSTOMRANDOM
  PRIVATE ED25519_CUSTOMHASH
  # set iota crypto use mbedtls
  PRIVATE CRYPTO_USE_MBEDTLS)

if(CONFIG_ZTEST)
  target_include_directories(app PUBLIC ${EXT_LIB_INC} ${IOTA_SRC_DIR}
                                        ${CMAKE_CURRENT_SOURCE_DIR}/tests)
else()
  target_include_directories(app PUBLIC ${EXT_LIB_INC} ${IOTA_SRC_DIR}
                                        ${CMAKE_SOURCE_DIR}/src)
endif()
