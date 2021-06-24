#[[
// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0
]]

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(iota_app)

option(UNIT_TESTS "Build unit test app" OFF)

set(EXT_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")

set(EXT_LIB_SRC
    "${EXT_LIB_DIR}/cjson/cJSON.c" "${EXT_LIB_DIR}/blake2/blake2b-ref.c"
    "${EXT_LIB_DIR}/ed25519-donna/ed25519.c")

set(EXT_LIB_INC "${EXT_LIB_DIR}/cjson" "${EXT_LIB_DIR}/blake2"
                "${EXT_LIB_DIR}/ed25519-donna" "${EXT_LIB_DIR}/uthash")

set(IOTA_SRC "${CMAKE_CURRENT_SOURCE_DIR}/libs/iota-c/src")

set(IOTA_CRYPTO "${IOTA_SRC}/crypto/iota_crypto.c")

set(IOTA_CORE
    "${IOTA_SRC}/core/address.c"
    "${IOTA_SRC}/core/seed.c"
    "${IOTA_SRC}/core/models/inputs/utxo_input.c"
    "${IOTA_SRC}/core/models/outputs/outputs.c"
    "${IOTA_SRC}/core/models/payloads/indexation.c"
    "${IOTA_SRC}/core/models/payloads/transaction.c"
    "${IOTA_SRC}/core/models/message.c"
    "${IOTA_SRC}/core/utils/bech32.c"
    "${IOTA_SRC}/core/utils/byte_buffer.c"
    "${IOTA_SRC}/core/utils/iota_str.c"
    "${IOTA_SRC}/core/utils/slip10.c")

message("Unit tests is ${UNIT_TESTS}")

if(UNIT_TESTS)
  target_sources(app PRIVATE "tests/test_main.c" ${EXT_LIB_SRC} ${IOTA_CRYPTO}
                             ${IOTA_CORE})
else()
  target_sources(app PRIVATE "src/main.c" "src/iota_shell.c" "src/wifi.c"
                             ${EXT_LIB_SRC} ${IOTA_CRYPTO} ${IOTA_CORE})
endif()

# configure ed25519-donna
target_compile_definitions(
  app
  # ed25519-donna
  PRIVATE ED25519_CUSTOMRANDOM
  PRIVATE ED25519_CUSTOMHASH
  # iota client
  PRIVATE CRYPTO_USE_MBEDTLS)

if(UNIT_TESTS)
  target_include_directories(app PUBLIC ${EXT_LIB_INC} ${IOTA_SRC}
                                        ${CMAKE_CURRENT_SOURCE_DIR}/tests)
else()
  target_include_directories(app PUBLIC ${EXT_LIB_INC} ${IOTA_SRC})
endif()
