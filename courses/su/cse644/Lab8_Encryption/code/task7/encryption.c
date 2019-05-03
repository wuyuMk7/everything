#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/err.h>

int encrypt(char *key)
{
  unsigned char *plaintext="This is a top secret.", current_ciphertext[1024];
  int plain_len = strlen(plaintext), cipher_len = 0, total_len = 0;

  unsigned char iv[16] = { 0xaa, 0xbb, 0xcc, 0xdd,
                           0xee, 0xff, 0x00, 0x99, 
                           0x88, 0x77, 0x66, 0x55, 
                           0x44, 0x33, 0x22, 0x11 };
  unsigned char ciphertext[] = { 0x76, 0x4a, 0xa2, 0x6b,
				 0x55, 0xa4, 0xda, 0x65,
				 0x4d, 0xf6, 0xb1, 0x9e, 
				 0x4b, 0xce, 0x00, 0xf4,
				 0xed, 0x05, 0xe0, 0x93, 
				 0x46, 0xfb, 0x0e, 0x76, 
				 0x25, 0x83, 0xcb, 0x7d, 
				 0xa2, 0xac, 0x93, 0xa2 };
  EVP_CIPHER_CTX ctx;
  memset(current_ciphertext, '\0', 1024);

  EVP_CIPHER_CTX_init(&ctx);
  EVP_CipherInit_ex(&ctx, EVP_aes_128_cbc(), NULL, NULL, NULL, 1);
  OPENSSL_assert(EVP_CIPHER_CTX_key_length(&ctx) == 16);
  OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == 16);
  EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, 1);

  if(!EVP_CipherUpdate(&ctx, current_ciphertext, &cipher_len, plaintext, plain_len)) {
    ERR_print_errors_fp(stderr);
    EVP_CIPHER_CTX_cleanup(&ctx);
    return 0;
  }
  total_len += cipher_len;

  if(!EVP_CipherFinal_ex(&ctx, current_ciphertext + total_len, &cipher_len)) {
    ERR_print_errors_fp(stderr);
    EVP_CIPHER_CTX_cleanup(&ctx); 
    return 0;
  }
  total_len += cipher_len;

  //int res = strncmp(ciphertext, current_ciphertext, 32);
  //if (res==0)
    //printf("%s\n\n", key);

  if ((total_len == 32) && (strncmp(ciphertext, current_ciphertext, 32) == 0)) {
    printf("Encryption succeeded. Current key: %s\n", key);
    return 1;
  } else {
    printf("Encryption failed. Key %s passed.\n", key);
  }

  EVP_CIPHER_CTX_cleanup(&ctx);
  return 0;
}

int main(int argc, char* argv)
{
  char *word = NULL, key[16];
  size_t buffer_size = 256;
  ssize_t word_len;

  FILE *words = fopen("words.txt", "r");
  if (words == NULL) {
    perror("File words.txt read error.");
    return 0;
  }

  while((word_len = getline(&word, &buffer_size, words)) != -1) {
    memset(key, '#', 16);
    if (word_len > 16) continue;
    strncpy(key, word, word_len - 1);

    if(encrypt(key) == 1) 
      break;
  }
  
  fclose(words);
  return 0;
}
