# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(iota_app)

set(EXT_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")

set(EXT_LIB_SRC 
    "${EXT_LIB_DIR}/cjson/cJSON.c"
    "${EXT_LIB_DIR}/blake2/blake2b-ref.c"
    "${EXT_LIB_DIR}/ed25519-donna/ed25519.c"
)

set(EXT_LIB_INC 
    "${EXT_LIB_DIR}/cjson"
    "${EXT_LIB_DIR}/blake2"
    "${EXT_LIB_DIR}/ed25519-donna"
    )

message("EXT SRC: ${EXT_LIB_SRC}")

target_sources(app PRIVATE "src/main.c"
    ${EXT_LIB_SRC})

# configure ed25519-donna
target_compile_definitions(app 
    PRIVATE ED25519_CUSTOMRANDOM
    PRIVATE ED25519_CUSTOMHASH)

target_include_directories(app PUBLIC
    ${EXT_LIB_INC})
