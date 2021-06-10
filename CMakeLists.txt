# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(iota_app)

option(UNIT_TESTS "Build unit test app" OFF)

set(EXT_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")

set(EXT_LIB_SRC
    "${EXT_LIB_DIR}/cjson/cJSON.c" "${EXT_LIB_DIR}/blake2/blake2b-ref.c"
    "${EXT_LIB_DIR}/ed25519-donna/ed25519.c")

set(EXT_LIB_INC "${EXT_LIB_DIR}/cjson" "${EXT_LIB_DIR}/blake2"
                "${EXT_LIB_DIR}/ed25519-donna")

set(IOTA_SRC "${CMAKE_CURRENT_SOURCE_DIR}/libs/iota-c/src")

set(IOTA_CRYPTO "${IOTA_SRC}/crypto/iota_crypto.c")

message("Unit tests is ${UNIT_TESTS}")

if(UNIT_TESTS)
  target_sources(app PRIVATE "tests/test_main.c" ${EXT_LIB_SRC} ${IOTA_CRYPTO})
else()
  target_sources(app PRIVATE "src/main.c" ${EXT_LIB_SRC} ${IOTA_CRYPTO})
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
