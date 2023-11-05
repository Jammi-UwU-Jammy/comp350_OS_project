
#define STR_LIM 80

extern int interrupt(int number, int AX, int BX, int CX, int DX);
void printString(char* );
void printChar(char );
void readString(char* );
void readSector(char* buffer, int sector);

int main(){
	char buffer[512];
	readSector(buffer, 30);
	printString(buffer);

	printString("Done.\n");
	while(1);

}

void readSector(char* buffer, int sector){
	int al = 1, ah = 2, cl = sector+1, ch = 0 , dl = 0x80, dh = 0;
	int ax = ah*256 + al; 
	int cx = ch*256 + cl; 
	int dx = dh*256 + dl;

	interrupt(0x13, ax, buffer, cx, dx);
}

void printChar (char ch){
	char al = ch;
	char ah = 0xe;
	int ax = ah * 256 + al;
	interrupt(0x10, ax, 0, 0, 0);
}

void printString(char* str){
	while (*str) printChar(*str++); //putc(*str++, stdout);
	
	//int i = 0;
	//while ( str[i] != '\0'){
	//	printChar(str[i]);
	//	i++;
	//}
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

