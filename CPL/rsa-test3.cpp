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

    //char *msg1 = "Hello Mushroom Hello Mushroom";
    char *msg1 = "Hello Mushroom Hello Sheep Hello uglyMushroom Hello Magic Hello Strawberry";
    int len_msg1;
    len_msg1 = strlen(msg1);
    cout<<"msg1 len: "<<len_msg1<<endl;
    
    int num;
    num = len_msg1 / 50 + 1;     

    char *cipher1;
    int len_cipher1;
    len_cipher1 = num * 64; 
    cipher1 = (char *)malloc(len_cipher1 * sizeof(char));
    memset(cipher1, '\0', len_cipher1);


    unsigned char InputBuff[64], OutputBuff[64];
    int i;
    for(i = 0; i < num; i++) {
        memset((char *)InputBuff, '\0', 64);
        memset((char *)OutputBuff, '\0', 64);

    	int clen = 0;
	strcpy((char *)InputBuff, msg1 + 50 * i);
        cout<<"InputBuff["<<i<<"]: "<<InputBuff<<endl;
   	cout<<"InputBuff["<<i<<"]: "<<strlen((char *)InputBuff)<<endl;

	clen = RSA_public_encrypt(50,  (const unsigned char *)InputBuff, OutputBuff, publicKey, RSA_PKCS1_PADDING);
	cout<<"OutputBuff["<<i<<"]: "<<OutputBuff<<endl;
   	cout<<"OutputBuff["<<i<<"]: "<<clen<<endl;

        strcat(cipher1, (char *)OutputBuff);	

    }

    cout<<"cipher1: "<<cipher1<<endl;
    cout<<"len cipher1: "<<len_cipher1<<endl;
    cout<<"strlen cipher1: "<<strlen(cipher1)<<endl;


    char *cipher2;
    int len_cipher2;

    len_cipher2 = len_cipher1;
    cipher2 = (char *)malloc(len_cipher2 * sizeof(char));  
    memset(cipher2, '\0', len_cipher2);

    memcpy(cipher2, cipher1, len_cipher1);
   
    char *msg2;
    int len_msg2;
    len_msg2 = len_cipher2;
    msg2 = (char *)malloc(len_msg2 * sizeof(char));
    memset(msg2, '\0', len_msg2);

    int num2;
    num2 = len_cipher2 / 64;

    unsigned char InBuff[64], OutBuff[64];
    for(i = 0; i < num2; i++) {
	memset((char *)InBuff, '\0', 64);
	memset((char *)OutBuff, '\0', 64);

        strcpy((char *)OutBuff, cipher2 + 64 * i);
	cout<<"OutBuff["<<i<<"]: "<<OutBuff<<endl;
        cout<<"OutBuff["<<i<<"]: "<<strlen((char *)OutBuff)<<endl;
        int plen = 0;
	plen = RSA_private_decrypt(64, (const unsigned char*)OutBuff, InBuff, privateKey, RSA_PKCS1_PADDING);
        cout<<"OutBuff["<<i<<"]: "<<InBuff<<endl;
        cout<<"OutBuff["<<i<<"]: "<<plen<<endl;
	
	strcat(msg2, (char *)InBuff);	
    }
    cout<<"msg2: "<<msg2<<endl;
    cout<<"len msg2: "<<len_msg2<<endl;
    cout<<"strlen msg2: "<<strlen(msg2)<<endl;


}
