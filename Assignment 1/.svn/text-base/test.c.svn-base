#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>

typedef struct {
	bool dayFlag;
	bool mthFlag;
	bool yrFlag;
} dateFlags;

char* sortDateFormat(char* src);
char* sortDateType (char* src, dateFlags* flags);
void Trim(char* src, char* buffer, int srcsize);

void main(){
	
	char* lnBuffer;

	char* strptr;

	if((lnBuffer = malloc(sizeof(char*)*1024)) == NULL) {
		printf("lnBuffer: dynamic allocation of memory failed!\n");
		exit(0);
	}


	strcpy(lnBuffer, "S Dateformat = dd/mm/yy");

	strptr = sortDateFormat(lnBuffer);

	printf("Dateformat is %s\n", strptr);

	free (strptr);
	free(lnBuffer);

}

char* sortDateFormat(char* src){
	int i=0;
	int j=0;
	char* temp = NULL;
	char* buffer;
	char* format;
	char catstr[12];
	dateFlags* flags;

	if((buffer = malloc(sizeof(char*)*12)) == NULL) {
		printf("lnBuffer: dynamic allocation of memory failed!\n");
		exit(0);
	}

	if((flags = malloc(sizeof(dateFlags))) == NULL) {
		printf("lnBuffer: dynamic allocation of memory failed!\n");
		exit(0);
	}

	buffer[0] = '\0';

	temp = strtok(src, "=");
	temp = strtok(NULL, "=");

	printf("temp is %s\n", temp);

	Trim(temp, buffer, strlen(temp)+1);
	
	printf("Trimmed temp is %s\n", temp);

	temp = strtok(buffer, "/");
	printf("toktemp is %s\n", temp);
	format = sortDateType(temp, flags);
	strcpy(buffer, format);
	free(format);

	while(temp!=NULL){
	temp = strtok(NULL, "/");

		if(temp!=NULL){
			format = sortDateType(temp, flags);
			strcat(buffer, "/");
			strcat(buffer, format);
			free(format);
		}
	}
	
	free(flags);
	return buffer;
}

char* sortDateType (char* src, dateFlags* flags){
	
	char* temp;
	int len=0;

	len = strlen(src);

	printf("Length is %d\n", len);


	if((temp = malloc(sizeof(char*)*len)) == NULL) {
		printf("lnBuffer: dynamic allocation of memory failed!\n");
		exit(0);
	}

	strcpy(temp, src);

	switch(tolower(temp[0])) {
		case 'd':
			if(strlen(temp)!= 2){
				strcpy(temp, "ERR");
			}
			
			else {
				if(strcmp(temp, "dd")==0 && flags->dayFlag==false){
					strcpy(temp, "%d");
					flags->dayFlag=true;
				}

				else {
					strcpy(temp, "ERR");
				}
			}
			break;

		case 'm':
			if(strlen(temp)!=2 && strlen(temp)!=3){
				strcpy(temp,"ERR");
			}
			else {
				if(strcmp(temp, "mm")==0 && flags->mthFlag==false){
					strcpy(temp, "%m");
				}

				else if(strcmp(temp, "mmm")==0 && flags->mthFlag==false) {
					strcpy(temp, "%b");
				}

				else {
					strcpy(temp, "ERR");
				}
			}
			break;

		case 'y':
			if(strlen(temp)!=2 && strlen(temp)!=4){
				strcpy(temp,"ERR");
			}

			else {
				if(strcmp(temp, "yy")==0 && flags->yrFlag==false){
					strcpy(temp, "%y");
				}

				else if(strcmp(temp, "yyyy")==0 && flags->yrFlag==false) {
					strcpy(temp, "%Y");
				}
				else {
					strcpy(temp, "ERR");
				}
			}
			break;
	}

	return temp;
}

void Trim(char* src, char* buffer, int srcsize) {

	int i=0;
	char* temp = src;

	while(*temp != '\0'&& i<srcsize-1) {
		if(*temp != ' ' && *temp != '\t'){
			buffer[i++] = *temp;
		}
		temp++;
	}

	/*Append null byte*/
	buffer[i+1] = '\0';
}

// void makeUpper (char* src, char* Buffer, int sizeBuff) {
// 
// 	if(sizeBuff<1){
// 		return;
// 	}
// 
// 	char* temp = src;
// 	int i=0;
// 
// 	while(*temp != '\0' && i<sizeBuff+1) {
// 		Buffer[i++] = toupper(*temp);
// 		temp++;
// 	}
//}
