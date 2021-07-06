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

# [Build Example](https://docs.zephyrproject.org/latest/application/index.html)

This SDK is tested on [b_l4s5i_iot01a](https://docs.zephyrproject.org/latest/boards/arm/b_l4s5i_iot01a/doc/index.html) and [stm32f746g_disco](https://docs.zephyrproject.org/latest/boards/arm/stm32f746g_disco/doc/index.html) targets at this moment.

Zephyr targets can be found in [Supported Boards](https://docs.zephyrproject.org/latest/boards/index.html)

## Use `west` tool

```
# application configuration
$ west build -b b_l4s5i_iot01a -t menuconfig
# flash app to target board
$ west flash
```

## Use CMake and Ninja

```
# Use cmake to configure a Ninja-based buildsystem:
$ cmake -B build -GNinja -DBOARD=b_l4s5i_iot01a

# configure and flash application to target
$ ninja -C build menuconfig
$ ninja -C build flash
```
