/*
        a custom randombytes must implement:

        void ED25519_FN(ed25519_randombytes_unsafe) (void *p, size_t len);

        ed25519_randombytes_unsafe is used by the batch verification function
        to create random scalars
*/

#include <random/rand32.h>

void ED25519_FN(ed25519_randombytes_unsafe)(void *p, size_t len) {
  sys_csrand_get(p, len);
}
