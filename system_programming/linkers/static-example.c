#include <stdio.h>

int not_defined_here;
char message[]="hello world";
static int invocation = 0;

void hello_world(int increment){
	static int first_time =0;
	if(increment >= 0){
		puts(message);
		invocation++;
		fprintf(stderr,"I have printed tghe screen %d times",invocation);
		hello_world(increment-1);
	}
	if(first_time ==0){
		fprintf(stderr,"this is the first invocation\n");
		first_time++;
	}
}

int main(){ hello_world(3);}
