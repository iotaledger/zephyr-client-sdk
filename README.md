# IOTA Wallet

# [Build Application](https://docs.zephyrproject.org/latest/application/index.html)


```
$ west build -b b_l4s5i_iot01a
```

```
# Use cmake to configure a Ninja-based buildsystem:
$ cmake -B build -GNinja -DBOARD=reel_board samples/hello_world

# Now run ninja on the generated build system:
$ ninja -C build
```

# Run Application

```
# Use west
$ west flash

# Use ninja
$ ninja flash
```

Serial port baud rate 115200

```
*** Booting Zephyr OS build zephyr-v2.6.0  ***
Hello World! b_l4s5i_iot01a
```