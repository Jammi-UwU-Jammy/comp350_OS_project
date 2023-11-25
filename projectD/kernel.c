
#define STR_LIM 80

extern int interrupt(int number, int AX, int BX, int CX, int DX);
extern void makeInterrupt21();
extern void putInMemory(int seg, int addr, char ch);
extern void launchProgram(int seg);

void printString(char* );
void printChar(char );
void readString(char* );
void readSector(char* buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void readFile();
void terminate();
void executeProgram(char*);
void cmdLS();
void writeSector(char*, int);
void deleteFile(char*);

int mod(int, int);
void printDec(int);
int readAllSectors(char*, int, char*);
int strLen(char*);
int stringsEqual(char* str1, char* str2);
void getSubstring(char* str, int begin, int end, char* buffer);

int main(){
	//char buffer[512];
	//readSector(buffer, 30);
	//printString(buffer);
	
//	char line[80];
//	makeInterrupt21();
//	interrupt(0x21, 1, line, 0, 0);	
//	interrupt(0x21, 0, line, 0, 0);


//	char buffer[13312];
//	int sectorsRead;
//	makeInterrupt21();
//	interrupt(0x21, 3, "messag", buffer, &sectorsRead);
//	printString(buffer);

//	cmdLS();


//	char shellname[6]; 
//	shellname[0]='s'; shellname[1]='h'; shellname[2]='e'; shellname[3]='l'; shellname[4]='l'; shellname[5]='\0';
//	executeProgram(shellname);
//	makeInterrupt21();
//	interrupt(0x21, 4, shellname, 0, 0);

//	char line[80];
//	makeInterrupt21();
//	interrupt(0x21,1,line,0,0);
//	interrupt(0x21,0,line,0,0);

//	char emptyBuffer[512];
//	writeSector(emptyBuffer, 2);	
	deleteFile("messag");

	printString("Done");
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
	if (leng1 != leng2) {
		return 0;
	}

     	while (str1[i] == str2[i] && str1[i] != '\0') i++;
	
	if (i == leng1) return 1; else return 0;
};

void getSubstring(char* str, int begin, int end, char* result){
	// interval of [begin, end)
	int i = 0;

	for(i ; i < end-begin; i++){
		result[i] = str[i+begin];
		//printChar(result[i]);
	}
	result[i] = '\0';
}

/*==============Functions needed for the assignment===================*/
void deleteFile(char* fileName){
	char buffer[521], mapBuffer[512];
        int sectorCount, sectorOffset; 
	int i = 0, fileLocation = -1;

	//load map and dir
        readSector(buffer, 2);
	readSector(mapBuffer, 1);

	//find file
        for ( ; i < 512 ; i+=32){
                char file[7]; getSubstring(buffer, i, i+6, file);
                if (stringsEqual(fileName, file) == 1){
                        printString("Found: "); printString(file);
                        printString("\nReading...\n");
                        fileLocation = i; break;
                }
        }

	if (fileLocation < 0) return;
	
	// write to list
	buffer[fileLocation] = '\0'; //first character
	sectorOffset = fileLocation + 6;
        for ( i=0 ; i<32 ; i++){
		printString("At sector: "); printDec(i+6); printChar('\n');
                if (buffer[sectorOffset+i] == 0) break;
		else {
			mapBuffer[buffer[sectorOffset+i]] = '\0';
			buffer[sectorOffset+i] = '\0';
		}
	}
	writeSector(mapBuffer, 1);
	writeSector(buffer, 2);
}

void writeSector(char* ibuffer, int sector){
	int al = 1, ah = 3, cl = sector+1, ch = 0 , dl = 0x80, dh = 0;
        int ax = ah*256 + al;
        int cx = ch*256 + cl;
        int dx = dh*256 + dl;

        interrupt(0x13, ax, ibuffer, cx, dx);

}

void cmdLS(){
	char buffer[521];
        int i = 0;

        readSector(buffer, 2);
        for ( ; i < 512 ; i+=32){
                char file[7], content[13312];
		
		if (buffer[i] == '\0') break;

		getSubstring(buffer, i, i+6, file);
		readAllSectors(buffer, i, content); 
	       	printString(file); printString("\t"); printDec(strLen(content)); printString(" Byte(s)\n");

       	}
}

void terminate(){
	while(1);
}

void executeProgram(char* name){
	char buffer[13312]; int i, sectors;
	readFile(name, buffer, &sectors);
	for (i=0 ; i < 13312; i++){
		putInMemory(0x2000, i, buffer[i]);
	}
	if (sectors != 0) {
		launchProgram(0x2000);
	}
	else printString("Can't find program in memory.\n");
}

void readFile(char* fileName, char* fileBuffer, int* s){
	char buffer[521];
	int sectorCount, i = 0, fileLocation = -1;

	readSector(buffer, 2);
	for ( ; i < 512 ; i+=32){
		char file[7]; getSubstring(buffer, i, i+6, file);
		if (stringsEqual(fileName, file) == 1){
			printString("Found: "); printString(file); 
			printString("\nReading...\n");
			fileLocation = i; break;
		}
	}
	
	if (fileLocation != -1){
		sectorCount = readAllSectors(buffer, fileLocation, fileBuffer);
	}else  	sectorCount = 0;
	*s = sectorCount;	
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
	//printString("Interrupt 21.\n");
	switch(ax){
		case 0:
			printString(bx);	break;
		case 1:
			readString(bx);		break;
		case 2:
			readSector(bx, cx);	break;
		case 3:
			readFile(bx, cx, dx);	break;
		case 4:
			executeProgram(bx);	break;
		case 5:
			terminate();		break;
		case 6:
			writeSector(bx, cx);	break;
		case 7:
			deleteFile(bx);		break;
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
int readAllSectors(char* sectorList, int fileLocation, char* output){
	int i, sector = fileLocation + 6;
        for ( i=0 ; i<26 ; i++){
        	if (sectorList[sector+i] == 0) break;

                readSector(output+512*i, sectorList[sector+i]);
        }
	return i;
}

int mod(int divd, int divr){
	while(divd >= divr) divd -= divr;
	return divd;
}

void printDec(int dec){
	char queue[13312]; int i = 0;
	while (dec > 0){
		queue[i] = mod(dec, 10) + 0x30;
		i++;
		dec /= 10;
	}
	i = strLen(queue);
	while (i >= 0) {
		printChar(queue[i]);
		i--;
	}
}

void printChar (char ch){
	char al = ch;
	char ah = 0xe;
	int ax = ah * 256 + al;
	interrupt(0x10, ax, 0, 0, 0);
}

void printString(char* str){
        while (*str) {
                if (*str == '\n') printChar('\r');
                printChar(*str++); //putc(*str++, stdout);
        }
}
void readString(char* string){
        char letter;
        int index = 0;
        while(1){
                letter = interrupt(0x16, 0, 0, 0, 0);

                if (letter == 0xd || index == STR_LIM-2) break;

                if (letter == 0x8){
			if (index < 1) continue;
                        printChar(0x8); printChar(' '); printChar(0x8);
                        index--;
                }else{
                        string[index] = letter;
                        printChar(letter);
                        index++;
                }
        }
        printString("\n");
        string[index] = '\0';
}

