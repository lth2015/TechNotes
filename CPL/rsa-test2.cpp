#include <iostream>
#include "openssl/rsa.h"
#include <string.h>

using namespace std;

int main()
{

    RSA *privateKey = RSA_generate_key(512, RSA_F4, NULL, NULL);

    unsigned char PublicKey[512];
    unsigned char *PKey = PublicKey;
    int PublicKeyLen = i2d_RSAPublicKey(privateKey, &PKey);

    PKey = PublicKey;
    RSA *publicKey = d2i_RSAPublicKey(NULL, (const unsigned char**)&PKey, PublicKeyLen);

    char *msg1 = "Hello Mushroom Hello Mushroom";
    int len_msg1;
    len_msg1 = strlen(msg1);

    

    unsigned char InputBuff[64], OutputBuff[64];
    int plen = 0, clen = 0;
    strcpy((char *)InputBuff, msg1);

    cout<<"Source Message: "<<InputBuff<<endl;
    cout<<"src length: "<<strlen((char *)InputBuff)<<endl;

    clen = RSA_public_encrypt(50,  (const unsigned char *)InputBuff, OutputBuff, publicKey, RSA_PKCS1_PADDING);

    cout<<"After RSA encrypt: "<<OutputBuff<<endl;
    cout<<"cipher length: "<<clen<<endl;

    char *cipher;
    int len_cipher;

    len_cipher = 64;
    cipher = (char *)malloc(len_cipher * sizeof(char));  
    memset(cipher, '\0', len_cipher);

    memcpy(cipher, (char *)OutputBuff, 64);
    
    unsigned char oBuff[64];
    memset(oBuff, '\0', 64);

    //strcpy((char *)oBuff, cipher);
    memcpy((char *)oBuff, cipher, 64);

    memset(InputBuff, 0, sizeof(InputBuff));
    //plen = RSA_private_decrypt(64, (const unsigned char*)OutputBuff, InputBuff, ClientRsa, RSA_PKCS1_PADDING);
    plen = RSA_private_decrypt(64, (const unsigned char*)oBuff, InputBuff, privateKey, RSA_PKCS1_PADDING);

    cout<<"After RSA decrypt: "<<InputBuff<<endl;
    cout<<"plain length: "<<plen<<endl;







}
