void getSubstring(char* str, int begin, int end, char* result);
int strLen(char*);
int stringsEqual(char*, char*);
int getFirstWord(char* input, char* output);
void findCommand(char* str);

void main(){
       char input[80];
       char output[80];
       
        while(1){
		syscall(0, ">>");
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
	char file[80];
        char content[13312];
        getFirstWord(&str[endIndex+1], file);


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
	}else	syscall(0, "Command not found.\n");
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

