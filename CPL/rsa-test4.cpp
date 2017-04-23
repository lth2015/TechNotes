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
    //char *msg1 = "Hello Mushroom Hello Sheep Hello uglyMushroom Hello Magic Hello Strawberry";
    //char *msg1 = "02123456780123456789012345678901234567890101030507maxwell2016021820141501234567890123456789012345678901";
    //char *msg1 = "01maxwell4";
    char *msg1 = "0212345678853ae90f0351324bd73ea615e64875172457maxwell2016:02:17:17:40:00826fdc7592e1f5b013fe46259b17fbdd";
    int len_msg1;
    len_msg1 = strlen(msg1);
    cout<<"msg1 len: "<<len_msg1<<endl;
    
    int num;
    num = len_msg1 / 50 + 1;     

    char *cipher1;
    int len_cipher1;
    len_cipher1 = num * 64 + 1;
    cipher1 = (char *)malloc(len_cipher1 * sizeof(char));
    memset(cipher1, '\0', len_cipher1);


    unsigned char InputBuff[65], OutputBuff[65];
    int i;
    for(i = 0; i < num; i++) {
        memset((char *)InputBuff, '\0', 65);
        memset((char *)OutputBuff, '\0', 65);

	if (i == (num - 1)){
		memcpy((char *)InputBuff, msg1 + 50 * i, len_msg1 % 50);
	} else{
		memcpy((char *)InputBuff, msg1 + 50 * i, 50);
	}

    	int clen = 0;
	//strcpy((char *)InputBuff, msg1 + 50 * i);
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

    for(int i = 0; i < 64 * num; i++) {
	if(cipher1[i] == 0) {
		cout<<"FIND 0"<<"  @"<<i<<endl;
		break;
	}
    }

    char *cipher2;
    int len_cipher2;

    len_cipher2 = len_cipher1;
    cipher2 = (char *)malloc(len_cipher2 * sizeof(char));  
    memset(cipher2, '\0', len_cipher2);

    memcpy(cipher2, cipher1, len_cipher1);

    for(int i = 0; i < 64 * num; i++) {
	if(cipher2[i] == 0) {
		cout<<"FIND 0"<<"  @"<<i<<endl;
		break;
	}
    }



   
    char *msg2;
    int len_msg2;
    len_msg2 = len_cipher2;
    msg2 = (char *)malloc(len_msg2 * sizeof(char));
    memset(msg2, '\0', len_msg2);

    int num2;
    num2 = len_cipher2 / 64 + 1;
    //num2 = len_cipher2 / 64;
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
	
        if(i == (num2 - 1)) {
		memcpy(msg2 + i * 50, (char *)InBuff, len_msg2 % 64);  
	}else {
		memcpy(msg2 + i * 50, (char *)InBuff, 64);
	}	
	//strcat(msg2, (char *)InBuff);	
    }
    cout<<"msg2: "<<msg2<<endl;
    cout<<"len msg2: "<<len_msg2<<endl;
    cout<<"strlen msg2: "<<strlen(msg2)<<endl;


}
