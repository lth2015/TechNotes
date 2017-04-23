#include <iostream>
#include <string.h>
using namespace std;

int main()
{
	char Opr1[] = "eccbc87e4b5ce2fe28308fd9f2a7baf3";
	char Opr2[] = "6cb81a0a2c93af812fc8799002c5fcdb";
	char Output[128];

	memset(Output, '\0', 128);

	for(int i = 0; i < strlen(Opr1); i++)
	{
		Output[i] = Opr1[i] ^ Opr2[i];
	}

	cout<<"result: "<<endl;
	cout<<Output<<endl;
	cout<<strlen(Output)<<endl;

}
