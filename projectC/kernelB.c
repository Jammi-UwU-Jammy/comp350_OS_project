#include <stdio.h>

extern int interrupt(int, int, int, int, int);
void printString(char* str);
void printChar(char ch);
void readString();


int main(){
	//readString();
	//while(1);

	int startVidMem = 0xb800;
	int vidMemOffset = 0x0;
	int white = 0x7;
	char* letters = "Hello world\0";

	while(*letters != 0x0){
		putInMemory(startVidMem, vidMemOffset, *letters);
		vidMemOffset++;
		putInMemory(startVidMem, vidMemOffset, white);
		vidMemOffset++;
		letters++;
	}
	while(1);

}

void printChar (char ch){
	char ah = ch;
	char al = 0xe;
	int ax = ah * 256 + al;
	interrupt(0x10, ax, 0, 0, 0);
}

void printString(char* str){
	while (*str) printChar(*str++); //putc(*str++, stdout);
}

void readString(){
	char letter;
	char* string;


}

