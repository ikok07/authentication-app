//
// Created by Kok_PC on 21.7.2024 г..
//

#include <sodium.h>
#include "../include/encryption.h"

#include <string.h>

int encrypt_text(einfo_t *enc_info, char *str) {
    const size_t min_size = crypto_secretbox_MACBYTES + strlen(str);
    unsigned char encrypted[min_size];
    unsigned char key[crypto_secretbox_KEYBYTES];
    unsigned char nonce[crypto_secretbox_NONCEBYTES];

    randombytes_buf(nonce, sizeof nonce);
    randombytes_buf(key, sizeof key);

    const int result = crypto_secretbox_easy(encrypted, (unsigned char *)str, strlen(str), nonce, key);
    if (result != 0) return -1;

    enc_info->encrypted_msg = malloc(min_size);
    if (enc_info->encrypted_msg == NULL) return -2;

    enc_info->encrypted_size = min_size;
    memcpy(enc_info->nonce, nonce, crypto_secretbox_NONCEBYTES);
    memcpy(enc_info->key, key, crypto_secretbox_KEYBYTES);
    memcpy(enc_info->encrypted_msg, encrypted, min_size);
    return 0;
}

int decrypt_text(char *dest, const einfo_t *enc_info) {
    const size_t decrypted_len = enc_info->encrypted_size - crypto_secretbox_MACBYTES;
    unsigned char decrypted[decrypted_len];

    int result = crypto_secretbox_open_easy(decrypted, enc_info->encrypted_msg, enc_info->encrypted_size, enc_info->nonce, enc_info->key);
    if (result != 0) return result;

    memcpy(dest, decrypted, decrypted_len);
    dest[decrypted_len] = '\0';
    return 0;
}
