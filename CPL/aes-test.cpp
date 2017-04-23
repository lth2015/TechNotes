#include <iostream>
#include <string.h>
#include <malloc.h>
#include <openssl/aes.h>

using namespace std;

int main() {
	//char *msg = "01maxwell123456aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	//char *msg = "0123456789012345";
	//char *msg = "01234567890123456";
	char *msg = "01234567890123456";
	//char *msg  = "01234567890123456789012345678901";
	//unsigned char InBuff[64], OutBuff[64];
	unsigned char InBuff[1], OutBuff[1];
	unsigned char *tmpBuff;
	int len_tmpBuff;

	if(strlen(msg) % 16 != 0) {
		len_tmpBuff = (strlen(msg) / 16 + 1) * 16;
	}else {
		len_tmpBuff = strlen(msg);
	}
//	len_tmpBuff = strlen(msg);
	len_tmpBuff = 128;
	tmpBuff = (unsigned char *)malloc(len_tmpBuff * sizeof(tmpBuff));
	memset(tmpBuff, '\0', len_tmpBuff);

	cout<<"strlen(msg): "<<strlen(msg)<<endl;
	cout<<"len_tmpBuff: "<<len_tmpBuff<<endl;


	strcpy((char *)InBuff, msg);
	
	unsigned char Seed[32];
	strcpy((char *)Seed, "AbCdEfGhIjKlMnOpQrStUvWxYz12345");

	AES_KEY AESEncryptKey, AESDecryptKey;
	AES_set_encrypt_key(Seed, 256, &AESEncryptKey);
	AES_set_decrypt_key(Seed, 256, &AESDecryptKey);

	cout<<"inbuff: "<<InBuff<<endl;
	AES_encrypt(InBuff, OutBuff, &AESEncryptKey);

		
	

	memset(InBuff, '\0', 1);
	
	AES_decrypt(OutBuff, InBuff, &AESDecryptKey);
	//AES_decrypt(OutBuff, tmpBuff, &AESDecryptKey);
	cout<<"inbuff: "<<InBuff<<endl;
	//cout<<"tmpBuff: "<<tmpBuff<<endl;
		
	
}
