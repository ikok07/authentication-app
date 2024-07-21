//
// Created by Kok_PC on 21.7.2024 г..
//

#include <sodium.h>
#include "../include/encryption.h"

#include <string.h>

int encrypt_text(einfo_t *enc_info, char *str) {
    const size_t min_size = crypto_secretbox_MACBYTES + strlen(str);
    unsigned char encrypted[min_size];
    char nonce_str[crypto_secretbox_NONCEBYTES];
    randombytes_buf(nonce_str, sizeof nonce_str);

    int result = crypto_secretbox_easy(encrypted, (unsigned char *)str, strlen(str), nonce_str, (unsigned char *)getenv("ENCRYPTION_KEY"));
    if (result != 0) return result;

    enc_info->encrypted_msg = malloc(min_size);
    // enc_info->nonce = malloc(crypto_secretbox_NONCEBYTES);

    memcpy_s(enc_info->encrypted_msg, min_size, encrypted, min_size);
    enc_info->encrypted_size = min_size;
    memcpy_s(enc_info->nonce, crypto_secretbox_NONCEBYTES, nonce_str, crypto_secretbox_NONCEBYTES);
    return 0;
}

int decrypt_text(char *dest, const einfo_t enc_info) {
    unsigned char decrypted[enc_info.encrypted_size];
    int result = crypto_secretbox_open_easy(decrypted, enc_info.encrypted_msg, enc_info.encrypted_size, enc_info.nonce, (unsigned char *)getenv("ENCRYPTION_KEY"));

    if (result != 0) return result;

    const size_t decrypted_len = enc_info.encrypted_size - crypto_secretbox_MACBYTES;
    memcpy_s(dest, decrypted_len, decrypted, decrypted_len);
    dest[decrypted_len] = '\0';
    return 0;
}
