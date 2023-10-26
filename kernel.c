#include <stdio.h>
#include <ctype.h>

#define STR_LIM 80

extern int interrupt(int, int, int, int, int);
void printString(char* str);
void printChar(char ch);
void readString(char* str);


int main(){
	char line[STR_LIM];
	printString("Enter a line: \n");
	readString(line);
	printString(line);
	while(1);

}

void printChar (char ch){
	char al = ch;
	char ah = 0xe;
	int ax = ah * 256 + al;
	interrupt(0x10, ax, 0, 0, 0);
}

void printString(char* str){
	while (*str) printChar(*str++); //putc(*str++, stdout);
}
void readString(char* string){
	char letter;
	int index = 0;
	while(1){
		letter = interrupt(0x16, 0, 0, 0, 0);
		string[index] = letter; 
		printChar(letter);
		index++;
		
		if (letter == 0xd || index == STR_LIM-2) break;		
	}
	printChar('\n');
	string[index + 1] = '\0';
}

