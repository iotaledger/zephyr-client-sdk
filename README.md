# IOTA Client Development Kit for Zephyr RTOS

This Software Development Kit(SDK) is a bundle of IOTA client library and Zephyr RTOS for embedded client development.

For more details please visit [iota.c](https://iota-c-client.readthedocs.io/en/latest/index.html) and [Zephyr](https://docs.zephyrproject.org/latest/introduction/index.html).

# Hardware Requirements

The hardware requirements are depends on the use case, here are recommendations:

* 1MB Flash Memory
* 256KB SRAM
* True Random Number Generator
* HMAC-SHA Hardware Accelerator
* TCP/IP network stack with TLS support

# Work with Zephyr

Please refer to [Getting Started with Zephyr](https://docs.zephyrproject.org/latest/getting_started/index.html) for setting up your development environment.

This is tested on [b_l4s5i_iot01a](https://docs.zephyrproject.org/latest/boards/arm/b_l4s5i_iot01a/doc/index.html) and [stm32f746g_disco](https://docs.zephyrproject.org/latest/boards/arm/stm32f746g_disco/doc/index.html) targets at this moment.

Zephyr targets can be found in [Supported Boards](https://docs.zephyrproject.org/latest/boards/index.html)

Set `ZEPHYR_BASE` before using Zephyr build system.

```shell
export ZEPHYR_BASE="~/zephyrproject/zephyr"
```

## Use `west` tool

```shell
# application configuration
west build -b b_l4s5i_iot01a -t menuconfig
# flash app to target board
west flash
```

## Use CMake and Ninja

```shell
# Use cmake to configure a Ninja-based buildsystem:
cmake -B build -GNinja -DBOARD=b_l4s5i_iot01a

# configure and flash application to target
ninja -C build menuconfig
ninja -C build flash
```

# Work with nRF-Connect SDK

Please refer to [Getting Started with nRF-Connect](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/getting_started.html) for setting up your development environment.

Set `ZEPHYR_BASE` before using Zephyr build system.

```shell
export ZEPHYR_BASE="~/nRF-Connect-SDK/zephyr"
west build -b nrf9160dk_nrf9160@1.0.0 -t menuconfig -- -DUNIT_TESTS=ON
# make sure ZTEST is enabled and stack size is big enough.
# CONFIG_ZTEST=y
# CONFIG_ZTEST_STACKSIZE=5120
west flash
```

Console output

```
*** Booting Zephyr OS build v2.6.0-rc1-ncs1  ***
====Unit Test on nrf9160dk_nrf9160====
Running test suite iota_crypto
===================================================================
START - test_blake2b
 PASS - test_blake2b in 0.146 seconds
===================================================================
START - test_HMACSHA
 PASS - test_HMACSHA in 0.3 seconds
===================================================================
Test suite iota_crypto succeeded
Running test suite iota_core
===================================================================
START - test_address_gen
 PASS - test_address_gen in 0.25 seconds
===================================================================
START - test_message_with_tx
 PASS - test_message_with_tx in 0.11 seconds
===================================================================
START - tx_essence_serialization
[tx_essence_serialize_length:106] an input is needed
 PASS - tx_essence_serialization in 0.6 seconds
===================================================================
Test suite iota_core succeeded
Running test suite iota_bench
===================================================================
START - bench_address_generating
Bench 100 address generation
	min(ms)	max(ms)	avg(ms)	total(ms)
	15	16	15.20	1520
 PASS - bench_address_generating in 1.533 seconds
===================================================================
Test suite iota_bench succeeded
===================================================================
PROJECT EXECUTION SUCCESSFUL
```
