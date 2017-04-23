#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/aes.h>
int main(int argc, char **argv)
{
    unsigned char buf[512];
    unsigned char buf2[512];
    unsigned char buf3[512];
    unsigned char aes_keybuf[32];
    memset(buf,1,sizeof(buf));    
    memset(buf,0,sizeof(buf2));    
    memset(buf,0,sizeof(buf3));
    memset(aes_keybuf,0,sizeof(aes_keybuf));


    AES_KEY aeskey;
    AES_set_encrypt_key(aes_keybuf,256,&aeskey);
    for(int i=0;i<sizeof(buf);i+=16)
    AES_encrypt(buf+i,buf2+i,&aeskey);


    AES_set_decrypt_key(aes_keybuf,256,&aeskey);
    for(int i=0;i<sizeof(buf);i+=16)
    AES_decrypt(buf2+i,buf3+i,&aeskey);


    if(memcmp(buf,buf3,sizeof(buf))==0)
        printf("test success\r\n");
    else
        printf("test fail\r\n");
}
