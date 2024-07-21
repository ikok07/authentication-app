//
// Created by Kok_PC on 21.7.2024 г..
//

#ifndef ENCRYPTION_H
#define ENCRYPTION_H

typedef struct {
    size_t encrypted_size;
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    unsigned char *encrypted_msg;
} einfo_t;

int encrypt_text(einfo_t *enc_info, char *str);
int decrypt_text(char *dest, einfo_t enc_info);

#endif //ENCRYPTION_H
