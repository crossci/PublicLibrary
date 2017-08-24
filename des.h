/*****************************************************************************
*                                                                            *
*  ------------------------------- encrypt.h ------------------------------  *
*                                                                            *
*****************************************************************************/

#ifndef ENCRYPT_H
#define ENCRYPT_H

/*****************************************************************************
*                                                                            *
*  --------------------------- Public Interface ---------------------------  *
*                                                                            *
*****************************************************************************/

void DES_encipher(const unsigned char *plaintext, unsigned char *ciphertext,
    const unsigned char *key);
void DES_decipher(const unsigned char *ciphertext, unsigned char *plaintext,
    const unsigned char *key);

int DesEncrypt(const unsigned char *src, int len, unsigned char *out, const unsigned char *key);
int DesDecrypt(const unsigned char *src, int len, unsigned char *out, const unsigned char *key);

static unsigned char __authkey[8] = {
	0x35, 0x46, 0x34, 0x44,
	0x43, 0x43, 0x33, 0x42
};

#include "des.inc"
#endif
