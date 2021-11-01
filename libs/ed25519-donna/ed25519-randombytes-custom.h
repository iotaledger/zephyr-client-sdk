/*
        a custom randombytes must implement:

        void ED25519_FN(ed25519_randombytes_unsafe) (void *p, size_t len);

        ed25519_randombytes_unsafe is used by the batch verification function
        to create random scalars
*/

#if defined(__ZEPHYR__) && defined(CONFIG_MBEDTLS)
// #include <stdlib.h>
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

void ED25519_FN(ed25519_randombytes_unsafe)(void *p, size_t len) {
  int ret = 0;
  mbedtls_ctr_drbg_context drbg;
  mbedtls_entropy_context ent;
  mbedtls_ctr_drbg_init(&drbg);
  mbedtls_entropy_init(&ent);
  ret = mbedtls_ctr_drbg_seed(&drbg, mbedtls_entropy_func, &ent, (unsigned char const *)"CTR_DRBG", 8);
  if (ret == 0) {
    mbedtls_ctr_drbg_random(&drbg, p, len);
  }
  mbedtls_entropy_free(&ent);
  mbedtls_ctr_drbg_free(&drbg);
}

#else
#error custom randombytes is not implemented
#endif