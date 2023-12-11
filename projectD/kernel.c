
#define STR_LIM 80

extern int interrupt(int number, int AX, int BX, int CX, int DX);
extern void makeInterrupt21();
extern void putInMemory(int seg, int addr, char ch);
extern void launchProgram(int seg);
extern void makeTimerInterrupt();
extern void handleTimerInterrupt(int seg, int sp);
extern void returnFromTimer(int seg, int sp);
extern void initializeProgram(int seg);
extern int setKernelDataSegment();
extern void restoreDataSegment(int seg);

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
void writeFile(char*, char*, int);
void copyFile(char*, char*);


void PROC_TABLE_init();
void findEmptySectorsNFill(char* mapBuffer, int secNum, char* sectorList);
void copyNAutofillNull(char* from, char* new, int newLen);
int mod(int, int);
void printDec(int);
int readAllSectors(char*, int, char*);
int strLen(char*);
int stringsEqual(char* str1, char* str2);
void getSubstring(char* str, int begin, int end, char* buffer);

int CURRENT_PROC = -1;
int PROC_ACTIVE[8] = {0};
int PROC_SP[8] = {0xff00};

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

	char shellname[6]; 
	shellname[0]='s'; shellname[1]='h'; shellname[2]='e'; shellname[3]='l'; shellname[4]='l'; shellname[5]='\0';
	makeInterrupt21();
	executeProgram(shellname);


	//interrupt(0x21, 4, shellname, 0, 0);


//	char line[80];
//	makeInterrupt21();
//	interrupt(0x21,1,line,0,0);
//	interrupt(0x21,0,line,0,0);

//	char emptyBuffer[512];
//	writeSector(emptyBuffer, 2);	
//	deleteFile("messag");

	//makeInterrupt21();
	//interrupt(0x21, 8, "ABCDEFG", "fxGf\0", 1);

	//copyFile("tstpr1", "LOLO\0");

	makeTimerInterrupt();

	printString("Done");
	while(1);

}

/*==============Functions needed for the assignment===================*/
void PROC_TABLE_init(){
	//???
}

void copyFile(char* fNameOld, char* fNameNew){
	char buffer[13312];
	int sectorsRead;

	readFile(fNameOld, buffer, &sectorsRead);
	writeFile(buffer, fNameNew, 1); //TODO: only write 1 sectorfor now
}

void writeFile(char* buffer, char* fileName, int sectorNum){
	char secBuffer[521], mapBuffer[512];
	char sectorIndices[512-3];
	int i = 0;
	
	readSector(mapBuffer, 1);
	readSector(secBuffer, 2);

	// fill in sectors to map
	findEmptySectorsNFill(mapBuffer, sectorNum, sectorIndices);
	if (strLen(sectorIndices) == 0) return; // if no space, return


	// write to file directory
	for ( ; i < 512 ; i+=32){
		if (secBuffer[i] == '\0') {
			copyNAutofillNull(fileName, secBuffer + i, 7); // fileName
			copyNAutofillNull(sectorIndices, secBuffer + i + 6, 26); // sectors
			writeSector(buffer, sectorIndices[0]); // TODO: only 1 sector right now
			break;
		}
	}

	writeSector(mapBuffer, 1);
	writeSector(secBuffer, 2);
}


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
//		printString("At sector: "); printDec(i+6); printChar('\n');
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
	int dataSeg;
	
	dataSeg = setKernelDataSegment();

	PROC_ACTIVE[CURRENT_PROC] = 0;
	
	while(1);

	restoreDataSegment(dataSeg);

}

int findFreeSegment(){
	int i = 0, freeSeg = -1;
	for ( ; i < 8 ; i++){
		if (PROC_ACTIVE[i] == 0)
			return i;
	}
	return freeSeg;
}

void executeProgram(char* name){
	char buffer[13312]; 
	int i, sectors;
	int segment=-1, segOffset, dataSeg;

	readFile(name, buffer, &sectors);

	dataSeg = setKernelDataSegment();

	for (i=0; i < 8 ; i++){
		if (PROC_ACTIVE[i] == 0){
			segment = i;
			break;
		}
	}

	if (segment == -1){
		printString("No available segments.");
		return;
	}
	segOffset = (segment + 2) * 0x1000;
	for (i=0 ; i < 13312; i++){
 		putInMemory(segOffset, i, buffer[i]);
 	}
	initializeProgram(segOffset);
	PROC_ACTIVE[segment] = 1;
	PROC_SP[segment] = 0xff00;

	restoreDataSegment(dataSeg);
}

// void executeProgram(char* name){
// 	char buffer[13312]; int i, sectors;
// 	readFile(name, buffer, &sectors);
// 	for (i=0 ; i < 13312; i++){
// 		putInMemory(0x2000, i, buffer[i]);
// 	}
// 	if (sectors != 0) {
// 		launchProgram(0x2000);
// 	}
// 	else printString("Can't find program in memory.\n");
// }

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

void handleTimerInterrupt(int seg, int sp){
	int dataSeg, i;

	dataSeg = setKernelDataSegment();
	//	printDec(CURRENT_PROC + 100);
	

	if (CURRENT_PROC != -1){
		PROC_SP[CURRENT_PROC] = sp;
	}

	while (PROC_ACTIVE[CURRENT_PROC] != 1){
		if (CURRENT_PROC+1 == 8)
			CURRENT_PROC=0;
		else CURRENT_PROC++;
	}
	seg = (CURRENT_PROC+2) * 0x1000;
	sp = PROC_SP[CURRENT_PROC];

	restoreDataSegment(dataSeg);	

	returnFromTimer(seg, sp);
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
		case 8:
			writeFile(bx, cx, dx);	break;
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

void copyNAutofillNull(char* from, char* new, int newLen){
	int i = 0 ;
	for (; i < newLen ; i++){
		if (i < newLen - 1) 
			new[i] = from[i];
		else{
			if (strLen(from) < newLen) //old str smaller than new str, fill new str with nulls
				new[i] = '\0';
			else break;		//old str longer than new str, break
		} 
	}
	new[newLen-1] = '\0';
}

void findEmptySectorsNFill(char* mapBuffer, int secNum, char* sectorList){
	char i = 3, j = 0; 
	for ( ; i < 512 ; i+= 1){
		if ((mapBuffer[i] == '\0') && (secNum > 0)){ //if mapslot is empty & stack is not empty
			sectorList[j] = i;
			mapBuffer[i] = 0xFE;
			j++; secNum-- ; //pop
			//printChar(sectorList[i-3]+100);
		}
		if (secNum <= 0) {
			sectorList[j] = '\0';
			break;
		}
	}
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

