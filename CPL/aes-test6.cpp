#include <iostream>
#include <string.h>
#include <malloc.h>
#include <openssl/aes.h>

using namespace std;

int main() {
	char *inmsg = "01maxwell123456aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	//char *inmsg = "01234567890123456";
	unsigned char InBuff[16], OutBuff[16], tmpBuff[17];
	int i;

	int num1;
	if(strlen(inmsg) % 16 != 0) {
		num1 = (strlen(inmsg) / 16 + 1) * 16;
	}else {
		num1 = strlen(inmsg);
	}

	char *cipher;
	//int len_cipher = 32;
	int len_cipher = num1;
	cipher = (char *)malloc(len_cipher * sizeof(char));
	memset(cipher, '\0', len_cipher);


	unsigned char Seed[32];
	strcpy((char *)Seed, "AbCdEfGhIjKlMnOpQrStUvWxYz12345");

	AES_KEY AESEncryptKey, AESDecryptKey;
	AES_set_encrypt_key(Seed, 256, &AESEncryptKey);
	AES_set_decrypt_key(Seed, 256, &AESDecryptKey);

	for(i = 0; i < 4; i++) {
		memset((char *)InBuff, '\0', 16);
		memset((char *)OutBuff, '\0', 16);
		memcpy((char *)InBuff, inmsg + 15 * i, 15);

		AES_ecb_encrypt(InBuff, OutBuff, &AESEncryptKey, AES_ENCRYPT);	
		memset((char *)InBuff, '\0', 16);
		memcpy(cipher + 16 * i, OutBuff, 16);
		//AES_ecb_encrypt((unsigned char *)cipher, InBuff, &AESDecryptKey, AES_DECRYPT);	
		//cout<<InBuff<<endl;
	}	


	int num2 = strlen(cipher); //sizeof
	char *msg2;
	msg2 = (char *)malloc(num2 * sizeof(char));
	memset(msg2, '\0', num2);
		
	for(i = 0; i < 4; i++) {
		memset((char *)InBuff, '\0', 16);
		memset((char *)OutBuff, '\0', 16);
		memcpy((char *)OutBuff, cipher + 16 * i, 16);
			
		AES_ecb_encrypt(OutBuff, InBuff, &AESDecryptKey, AES_DECRYPT);		
		cout<<InBuff<<endl;
		//strcat(msg2,(char *)InBuff);
		memcpy(msg2 + 15 * i, (char *)InBuff, 15);
	}
	//cout<<strcmp(inmsg, msg2)<<endl;
	cout<<inmsg<<endl;
	cout<<msg2<<endl;
	//cout<<endl;
}
