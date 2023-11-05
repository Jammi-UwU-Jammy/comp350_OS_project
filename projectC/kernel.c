
#define STR_LIM 80

extern int interrupt(int number, int AX, int BX, int CX, int DX);
extern void makeInterrupt21();
void printString(char* );
void printChar(char );
void readString(char* );
void readSector(char* buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void readFile(char*);

int strLen(char*);
int stringsEqual(char* str1, char* str2);
void getSubstring(char* str, int begin, int end, char* buffer);


int main(){
	//char buffer[512];
	//readSector(buffer, 2);
	//printString(buffer);
	
	//char line[80];
	//makeInterrupt21();
	//interrupt(0x21, 1, line, 0, 0);	
	//interrupt(0x21, 0, line, 0, 0);

	readFile("messag");
	
	printString("\nDone.\n");
	while(1);

}

int strLen(char* str){
	int i = 0;
	while (str[i] != '\0') i++;
	return i;
}

int  stringsEqual(char* str1, char* str2){
     	int i = 0;
    	int leng1 = strLen(str1), leng2 = strLen(str2);

     	while (str1[i] == str2[i] && str1[i] != '\0') i++;
	
	if (i == leng1) return 'Y'; else return 'N';
};

void getSubstring(char* str, int begin, int end, char* result){
	//char resultString[7]; // interval of [begin, end)
	int i = 0;
	for(i ; i < end-begin; i++){
		result[i] = str[i+begin];
	//	printChar(result[i]);
	}
	result[i] = '\0';
}

/*========================================================*/

void readFile(char* fileName){
	char buffer[521];
	int ax = 3, dx;

	int i = 0, fileLocation = -1;
	readSector(buffer, 2);
	for ( ; i < 512 ; i+=32){
		char file[7]; getSubstring(buffer, i, i+6, file);
	
		if (stringsEqual(fileName, file) == 'Y'){
			printString("Found: "); printString(file); 
			fileLocation = i; break;
		}
	}

	if (fileLocation != -1){
		char fileBuffer[13312];
		int i = 0, sector = fileLocation + 6;
		for ( ; i < 32 ; i++){
			if (buffer[sector] == 0) break;
			readSector(fileBuffer+512*i, buffer[sector+i]);
			//printString(fileBuffer+512*i);
		}
	}
	
	//interrupt(0x3, fileName, buffer,);
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
	printString("Interrupt 21.\n");
	switch(ax){
		case 0:
			printString(bx);	break;
		case 1:
			readString(bx);		break;
		case 2:
			readSector(bx, cx);	break;
		default: 
			printString("AX invalid.");
	}
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

