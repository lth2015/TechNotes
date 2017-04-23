#include <iostream>
#include <string.h>
#include <malloc.h>
#include <openssl/aes.h>

using namespace std;

int main() {
	char *inmsg = "01maxwell123456aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	//char *inmsg = "01234567890123456";
	unsigned char InBuff[17], OutBuff[17], MediaBuff[16];
	char *outmsg;
	int len_outmsg;

	len_outmsg = strlen(inmsg) + 1;
	outmsg = (char *)malloc(len_outmsg * sizeof(char));
	memset(outmsg, '\0', len_outmsg);

	cout<<"inmsg: "<<inmsg<<endl;
	cout<<"strlen(inmsg): "<<strlen(inmsg)<<endl;

	unsigned char Seed[32];
	strcpy((char *)Seed, "AbCdEfGhIjKlMnOpQrStUvWxYz12345");

	AES_KEY AESEncryptKey, AESDecryptKey;
	AES_set_encrypt_key(Seed, 256, &AESEncryptKey);
	AES_set_decrypt_key(Seed, 256, &AESDecryptKey);

	int num = strlen(inmsg) / 16 + 1;
	int i;
	
	for(i = 0; i < num; i++ ) {
		memset(InBuff, '\0', 16);
		strcpy((char *)InBuff, inmsg + 16 * i);

		cout<<"InBuff-en"<<i<<": "<<InBuff<<endl;
		
		AES_encrypt(InBuff, OutBuff, &AESEncryptKey);	
		memset(InBuff, '\0', 17);			
		AES_decrypt(OutBuff, InBuff, &AESDecryptKey);
		strcat(outmsg, (char *)InBuff);
		cout<<"InBuff-de"<<i<<": "<<InBuff<<endl;
		memset(OutBuff, '\0', 17);
	}	
	
	cout<<"outmsg: "<<outmsg<<endl;
        cout<<"strlen(outmsg): "<<strlen(outmsg)<<endl;
	
	
}
