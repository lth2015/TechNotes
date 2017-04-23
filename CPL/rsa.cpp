#include <iostream>
#include "openssl/rsa.h"
#include <string.h>

using namespace std;

int main()
{

    RSA *ClientRsa = RSA_generate_key(512, RSA_F4, NULL, NULL);

    unsigned char PublicKey[512];
    unsigned char *PKey = PublicKey;
    int PublicKeyLen = i2d_RSAPublicKey(ClientRsa, &PKey);

    printf("PublicKeyBuff, Len=%d\n", PublicKeyLen);
    for(int i = 0; i < PublicKeyLen; i++) {
        printf("0x%02x, ", *(PublicKey + i));
    }
    printf("\n");

    PKey = PublicKey;
    RSA *EncryptRsa = d2i_RSAPublicKey(NULL, (const unsigned char**)&PKey, PublicKeyLen);

    unsigned char InputBuff[64], OutputBuff[64];
    strcpy((char *)InputBuff, "Hello Mushrom");

    cout<<"Source Message: "<<InputBuff<<endl;

    RSA_public_encrypt(64,  (const unsigned char *)InputBuff, OutputBuff, EncryptRsa, RSA_NO_PADDING);

    cout<<"After RSA encrypt: "<<OutputBuff<<endl;

    memset(InputBuff, 0, sizeof(InputBuff));
    RSA_private_decrypt(64, (const unsigned char*)OutputBuff, InputBuff, ClientRsa, RSA_NO_PADDING);

    cout<<"After RSA decrypt: "<<InputBuff<<endl;







}
