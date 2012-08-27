#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include"gputil.h"

int main() {

	FILE* inputfile;
	
	GpStatus fileStatus;
	GpFile* outFile;
	
	if((outFile = malloc(sizeof(GpFile))) == NULL) {
		printf("NODE: dynamic allocation of memory failed!\n");
		exit(0);
	}
	/*
	inputfile = fopen("GPS-Test.txt", "r");*/
	inputfile = fopen("testfile.txt", "r");

	/*Check to see if the file can be opened*/
	if(inputfile == NULL) {
		printf("Error: Unable to open file\n");
		exit(0);
	}

	else {
		printf("File successfully opened\n");
	}
	
	fileStatus =  readGpFile(inputfile, outFile);
	
	printf("fileStatus code = %d\n", fileStatus.code);
	printf("fileStatus line = %d\n", fileStatus.lineno);
	
	int trackno=0;

	GpTrack **tp = malloc(sizeof(GpTrack*));

	trackno = getGpTracks(outFile, tp);
	printf("trackno is %d\n", trackno);

	freeGpFile(outFile);
	
	fclose(inputfile);


	return(0);

}
