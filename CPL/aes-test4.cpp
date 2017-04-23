#include <iostream>
#include <string.h>
#include <malloc.h>
#include <openssl/aes.h>

using namespace std;

int main() {
	//char *inmsg = "01maxwell123456aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	char *inmsg = "01234567890123456";
	//char *inmsg = "abc";
	unsigned char InBuff[16], OutBuff[16], tmpBuff[17];
	char *outmsg;
	int len_outmsg;

	len_outmsg = strlen(inmsg) + 1;
	outmsg = (char *)malloc(len_outmsg * sizeof(char));
	memset(outmsg, '\0', len_outmsg);

	cout<<"inmsg: "<<inmsg<<endl;
	cout<<"strlen(inmsg): "<<strlen(inmsg)<<endl;

        char *cipher;
	int len_cipher;
	if(strlen(inmsg) % 16 != 0){
		len_cipher = (strlen(inmsg) / 16 + 1) * 16;
	}else{
		len_cipher = strlen(inmsg);
	}  
	len_cipher = 48;
	cipher = (char *)malloc(sizeof(char));
	memset(cipher, '\0', len_cipher);
	cout<<"len_cipher: "<<len_cipher<<endl;

	unsigned char Seed[32];
	strcpy((char *)Seed, "AbCdEfGhIjKlMnOpQrStUvWxYz12345");

	AES_KEY AESEncryptKey, AESDecryptKey;
	AES_set_encrypt_key(Seed, 256, &AESEncryptKey);
	AES_set_decrypt_key(Seed, 256, &AESDecryptKey);

	int num = strlen(inmsg) / 16 + 1;
	int i;
	
	for(i = 0; i < num; i++ ) {
		memset(InBuff, '\0', 16);
		memcpy((char *)InBuff, inmsg + 15 * i, 15);
		cout<<"strlen(InBuff): "<<strlen((char *)InBuff)<<endl;
		cout<<"InBuff-en"<<i<<": "<<InBuff<<endl;
		AES_encrypt(InBuff, OutBuff, &AESEncryptKey);	
		memset(InBuff, '\0', 16);			
		strcat(cipher, (char *)OutBuff);
		AES_decrypt(OutBuff, InBuff, &AESDecryptKey);
		strcat(outmsg, (char *)InBuff);
		cout<<"InBuff-de"<<i<<": "<<InBuff<<endl;
		memset(OutBuff, '\0', 16);
	}	
	
	cout<<"outmsg: "<<outmsg<<endl;
        cout<<"strlen(outmsg): "<<strlen(outmsg)<<endl;

	char *msg2;
	int len_msg2;
	len_msg2 = len_cipher + 1;
	msg2 = (char *)malloc(len_msg2 * sizeof(char));        				
	memset(msg2, '\0', len_msg2);

	//unsigned char iBuff[16], oBuff[16];

	
	for(i = 0; i < num; i++ ) {
        	memset(OutBuff, '\0', 16);
        	memset(InBuff, '\0', 16);
        	//strcpy((char *)OutBuff, cipher + 16 * i);
		memcpy((char *)OutBuff, cipher + 16 * i, 16);
        	AES_decrypt(OutBuff, InBuff, &AESDecryptKey);
        	cout<<"InBuff-de["<<i<<"]: "<<InBuff<<endl;
	   		
        	//strcat(msg2, (char *)InBuff);
		memcpy(msg2 + 16 * i, (char *)InBuff, 16);
        }
	cout<<"msg2: "<<msg2<<endl;
	cout<<"strlen(msg2): "<<strlen(msg2)<<endl;

  		
	
}
