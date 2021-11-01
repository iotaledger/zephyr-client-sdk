/*
        a custom hash must have a 512bit digest and implement:

        struct ed25519_hash_context;

        void ed25519_hash_init(ed25519_hash_context *ctx);
        void ed25519_hash_update(ed25519_hash_context *ctx, const uint8_t *in, size_t inlen);
        void ed25519_hash_final(ed25519_hash_context *ctx, uint8_t *hash);
        void ed25519_hash(uint8_t *hash, const uint8_t *in, size_t inlen);
*/

#if defined(__ZEPHYR__) && defined(CONFIG_MBEDTLS)
#include "mbedtls/sha512.h"
typedef mbedtls_sha512_context ed25519_hash_context;
void ed25519_hash_init(ed25519_hash_context *ctx) {
  mbedtls_sha512_init(ctx);
  mbedtls_sha512_starts(ctx, 0);
}
void ed25519_hash_update(ed25519_hash_context *ctx, const uint8_t *in, size_t inlen) {
  mbedtls_sha512_update(ctx, in, inlen);
}
void ed25519_hash_final(ed25519_hash_context *ctx, uint8_t *hash) {
  mbedtls_sha512_finish(ctx, hash);
  mbedtls_sha512_free(ctx);
}
void ed25519_hash(uint8_t *hash, const uint8_t *in, size_t inlen) { mbedtls_sha512(in, inlen, hash, 0); }

#else
#error custom hash is not implemented
#endif