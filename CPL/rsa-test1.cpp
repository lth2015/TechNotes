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

    PKey = PublicKey;
    RSA *EncryptRsa = d2i_RSAPublicKey(NULL, (const unsigned char**)&PKey, PublicKeyLen);

    unsigned char InputBuff[64], OutputBuff[64];
    int plen = 0, clen = 0;
    strcpy((char *)InputBuff, "Hello Mushrom Hello Mushroom");

    cout<<"Source Message: "<<InputBuff<<endl;
    cout<<"src length: "<<strlen((char *)InputBuff)<<endl;

//   clen = RSA_public_encrypt(64,  (const unsigned char *)InputBuff, OutputBuff, EncryptRsa, RSA_NO_PADDING);
    clen = RSA_public_encrypt(50,  (const unsigned char *)InputBuff, OutputBuff, EncryptRsa, RSA_PKCS1_PADDING);

    cout<<"After RSA encrypt: "<<OutputBuff<<endl;
    cout<<"cipher length: "<<clen<<endl;

    memset(InputBuff, 0, sizeof(InputBuff));
//    plen = RSA_private_decrypt(64, (const unsigned char*)OutputBuff, InputBuff, ClientRsa, RSA_NO_PADDING);
    plen = RSA_private_decrypt(64, (const unsigned char*)OutputBuff, InputBuff, ClientRsa, RSA_PKCS1_PADDING);

    cout<<"After RSA decrypt: "<<InputBuff<<endl;
    cout<<"plain length: "<<plen<<endl;







}
