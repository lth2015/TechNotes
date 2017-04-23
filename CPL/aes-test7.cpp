#include <iostream>
#include <string.h>
#include <malloc.h>
#include <openssl/aes.h>

using namespace std;

int main() {
	unsigned char InBuff[16], OutBuff[16];
	//char *msg1 = "01maxwell123456aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	char *msg1 = "mushroom uglymushroom love sheep strawberry";
	char *plain;
	int len_plain;
	len_plain = strlen(msg1);
	plain = (char *)malloc(len_plain * sizeof(char));
	memset(plain, '\0', len_plain);

	memcpy(plain, msg1, strlen(msg1));

	char *cipher;
	int len_cipher;
	len_cipher = (len_plain / 16 + 1) * 16;
	cipher = (char *)malloc(len_cipher * sizeof(char));
	memset(cipher, '\0', len_cipher);

	unsigned char Seed[32];
	strcpy((char *)Seed, "AbCdEfGhIjKlMnOpQrStUvWxYz12345");

	AES_KEY AESEncryptKey, AESDecryptKey;
	AES_set_encrypt_key(Seed, 256, &AESEncryptKey);
	AES_set_decrypt_key(Seed, 256, &AESDecryptKey);


	int i;
	int num1;
	num1 = len_plain / 16 + 1;

	for(i = 0; i < num1; i++) {
		memset((char *)InBuff, '\0', 16);
		memset((char *)OutBuff, '\0', 16);
		
		if (i == (num1 - 1)) {
			memcpy((char *)InBuff, plain + 16 * i, len_plain % 16);
		} else {	
			memcpy((char *)InBuff, plain + 16 * i, 16);
		}
		
		AES_ecb_encrypt(InBuff, OutBuff, &AESEncryptKey, AES_ENCRYPT);							
		memcpy(cipher + 16 * i, (char *)OutBuff, 16);
	}

	char plain2[100];
	memset(plain2, '\0', 100);

	
	for (i = 0; i < num1; i++) {
		memset((char *)InBuff, '\0', 16);
		memset((char *)OutBuff, '\0', 16);
	
		memcpy((char *)OutBuff, cipher + 16 * i, 16);	
			
		AES_ecb_encrypt(OutBuff, InBuff, &AESDecryptKey, AES_DECRYPT);		
		if(i == (num1 - 1)) {
			memcpy(plain2 + 16 * i, (char *)InBuff, len_plain % 16);
		}else {
			memcpy(plain2 + 16 * i, (char *)InBuff, 16);
		}
	}

	cout<<plain2<<endl;
}
