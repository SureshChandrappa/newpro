#include <stdio.h>

void f();
char x = 'a';
char y= 'b';

int main(){
	f();
	printf("%c %c \n",y,x);
	return 0;
}
