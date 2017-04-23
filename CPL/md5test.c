#include<stdio.h>
#include<openssl/md5.h>
#include<string.h>

int main( int argc, char **argv )
{
MD5_CTX ctx;
unsigned char *data="123";
unsigned char md[16];
char *tmp = NULL;
int i;

MD5_Init(&ctx);
MD5_Update(&ctx,data,strlen(data));
MD5_Final(md,&ctx);

tmp = (char *)malloc(35);
memset(tmp, 0x00, 35);
strcpy(tmp, "$1$");

for( i=0; i<16; i++ ){
sprintf(tmp + ( i * 2), "%02x", md[i]);
}
tmp[32] = 0;

printf("%s\n",tmp);
return 0;
}
