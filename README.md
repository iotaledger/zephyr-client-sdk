# IOTA Wallet

An IOTA wallet development kit uses [Zephyr RTOS](https://www.zephyrproject.org/)

Support Boards:

* [STM32 B-L4S5I-IOT01A](https://www.st.com/en/evaluation-tools/b-l4s5i-iot01a.html)

# [Build Application](https://docs.zephyrproject.org/latest/application/index.html)

Use `west` tool

```
$ west build -b b_l4s5i_iot01a
$ west build -t menuconfig
```

Use CMake and Ninja

```
# Use cmake to configure a Ninja-based buildsystem:
$ cmake -B build -GNinja -DBOARD=b_l4s5i_iot01a

$ ninja -C build menuconfig

# Now run ninja on the generated build system:
$ ninja -C build
```

Clean build

```
$ west build -t clean
$ ninja clean
$ west build -t pristine
$ ninja pristine
```

# Run Application

Run on target board

```
# Use west
$ west flash

# Use ninja
$ ninja flash
```

Debugging

```
$ west debug

$ ninja debug
```

Serial port baud rate 115200

```
*** Booting Zephyr OS build zephyr-v2.6.0  ***
Hello World! b_l4s5i_iot01a
```

# Unit tests

```
# west test for b_l4s5i_iot01a
$ west build -b b_l4s5i_iot01a -- -DCONF_FILE=prj_test.conf -DUNIT_TESTS=ON
$ west flash

# cmake test for b_l4s5i_iot01a
$ cmake -B build -GNinja -DBOARD=b_l4s5i_iot01a -DCONF_FILE=prj_test.conf -DUNIT_TESTS=ON
$ ninja -C build flash
```

# Native build

## setup a virtual network between a Linux host for native_posix target

https://docs.zephyrproject.org/latest/guides/networking/native_posix_setup.html

```
# app 
$ west build -b native_posix -- -DCONF_FILE=prj_native.conf
$ west build -t run

# unit test 
$ west build -b b_l4s5i_iot01a -- -DCONF_FILE=prj_native_test.conf -DUNIT_TESTS=ON
$ west flash
```