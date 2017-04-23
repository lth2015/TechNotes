#include <iostream>
using namespace std;

int main()
{
	int a[5] = {1, 2, 3, 4, 5};
	cout<<a<<endl;
	cout<<"--------------------------------------"<<endl;
	for(int i = 0; i < 5; i++) {
		cout<<a[i]<<endl;
	}

	for(int i = -10; i < 10; i++) {
		cout<<a[i]<<endl;
	}
}
