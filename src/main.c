// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/printk.h>
#include <zephyr.h>

#include "cJSON.h"
#include "crypto/iota_crypto.h"

char text1[] =
    "{\n"
    "\"name\": \"Jack (\\\"Bee\\\") Nimble\", \n"
    "\"format\": {\"type\":       \"rect\", \n"
    "\"width\":      1920, \n"
    "\"height\":     1080, \n"
    "\"interlace\":  false,\"frame rate\": 24\n"
    "}\n"
    "}";

void main(void) {
  printk("Hello World! %s\n", CONFIG_BOARD);

  cJSON *root = cJSON_Parse(text1);
  if (root) {
    cJSON *name = cJSON_GetObjectItemCaseSensitive(root, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
      printf("%s\n", name->valuestring);
    } else {
      printf("parsing error!!\n");
    }
    cJSON_Delete(root);
  }

  char seed[64] = {};
  iota_crypto_randombytes(seed, sizeof(seed));
  printf("seed: ");
  for (size_t i = 0; i < sizeof(seed); i++) {
    printf("%x, ", seed[i]);
  }
  printf("\n");
}
