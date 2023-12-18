void getSubstring(char* str, int begin, int end, char* result);
int strLen(char*);
int stringsEqual(char*, char*);
int getFirstWord(char* input, char* output);
void findCommand(char* str);
void readText(char *);
void copyFile(char*, char*);
int mod(int divd, int divr);
void printDec(int dec);
void cmdLS();


void main(){
       char input[80];
       char output[80];

	   enableInterrupts();
       
        while(1){
		syscall(0, "A>> ");
                syscall(1, input);
		syscall(0, "\n");
		findCommand(input);
      	};
}

int getFirstWord(char* input, char* output){
	int index = 0;
	while (input[index] != '\0'){
		if (input[index] == ' ') break;
		output[index] = input[index];
		index++;
	}
	output[index] = '\0';
	return index;
}

void findCommand(char* str){
	char command[80];
	int endIndex = getFirstWord(str, command);
	char file[7];
	char content[13312];
	int thirdOffset = getFirstWord(&str[endIndex+1], file);


	if (stringsEqual(command, "type")){
	//	char file[80];
	//	char content[13312];
	//	getFirstWord(&str[endIndex+1], file);
		
		if (strLen(file) != 0)
		{	syscall(3, file, content, 0);
			syscall(0, content);
		}else syscall("File not found.\n");
	
	}else if(stringsEqual(command, "exec")){
		if (strLen(file) != 0){
			syscall(4, file);
		}else syscall("Program not found.\n");
	}else if (stringsEqual(command, "create")){
		if (strLen(file) != 0){
			char* buffer[200];
			readText(buffer);
			syscall(8, buffer, file, 1);
		}else syscall("Invalid file name.\n");
	}else if (stringsEqual(command, "copy")){
		if (strLen(file) != 0){
			char newFile[7];
			getFirstWord(&str[thirdOffset+endIndex+2], newFile);
			syscall(0, newFile);
			syscall(0, "\n");
			copyFile(file, newFile);
		}else syscall("Can't find file.\n");
	}else if (stringsEqual(command, "dir")){
		cmdLS();
	}else if(stringsEqual(command, "dir")){
		syscall(6);
	}else	syscall(0, "Command not found.\n");
}

void copyFile(char* fNameOld, char* fNameNew){
	char buffer[13312];
	int sectorsRead;

	syscall(3, fNameOld, buffer, &sectorsRead);
	syscall(8, buffer, fNameNew, 1); //TODO: only write 1 sectorfor now
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
}

void getSubstring(char* str, int begin, int end, char* result){
	// interval of [begin, end)
	int i = 0;

	for(i ; i < end-begin; i++){
		result[i] = str[i+begin];
		//printChar(result[i]);
	}
	result[i] = '\0';
}

void readText(char *text){
	int index = 0;
	while(1){
		syscall(1, text + index);
		syscall(0, "\n");
		index = strLen(text);
		text[index] = '\n'; index++;
		if (text[index-1]== '\n' && text[index-2]== '\n') break;
	}
	//syscall(0, text);
}

void cmdLS(){
	char buffer[521];
    int i = 0;

        syscall(2, buffer, 2);
        for ( ; i < 512 ; i+=32){
            char file[7], content[13312];
			int sector, j;
		
			if (buffer[i] == '\0') break;
			getSubstring(buffer, i, i+6, file);
			
			//readAllSectors(buffer, i, content); 
			sector = i + 6;
			for ( j=0 ; j<26 ; j++){
				if (buffer[sector+j] == 0) break;

					syscall(2, content+512*j, buffer[sector+j]);
			}
			syscall(0, file); syscall(0, "\t"); printDec(strLen(content)); syscall(0, " Byte(s)\n");

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
		char* numStr[2];
		numStr[0] = queue[i]; numStr[1] = '\0';
		syscall(0, numStr);
		i--;
	}
}