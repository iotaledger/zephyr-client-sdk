/*
        a custom randombytes must implement:

        void ED25519_FN(ed25519_randombytes_unsafe) (void *p, size_t len);

        ed25519_randombytes_unsafe is used by the batch verification function
        to create random scalars
*/

#ifdef __ZEPHYR__
#include <random/rand32.h>

void ED25519_FN(ed25519_randombytes_unsafe)(void *p, size_t len) {
#ifdef CONFIG_ARCH_POSIX
  sys_rand_get(p, len);
#else
  sys_csrand_get(p, len);
#endif
}

#else
#error custom randombytes is not implemented
#endif