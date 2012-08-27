/*
CIS*2750 Assignment 2 W10
gpstool.c -- requires header files "gputil.h" and "gpstool.h within the same directory
Created: 17/2/2010
Author: Kwok Lau 0595198
Email: kwok@uoguelph.ca
Description; gpstool.c allow the management and implementation of .gpsu files from the commandline.It has 4 commandline options that needs to be used with the program.
	     This Program provides information about the GpFile structures created by readGpFile as well as having the options to modify and merge them.
*/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include "gputil.h"
#include "gpstool.h"

#define D fprintf(stderr, "line %d of %s of %s\n", __LINE__, __func__, __FILE__);

char* padSpace(char* src, int Length);
int gpsInfo( FILE *const outfile, const GpFile *filep );
int gpsDiscard( GpFile *filep, const char *which );
int gpsSort( GpFile *filep );
int gpsMerge( GpFile *filep, const char *const fnameB );
char* keepSort(char* which);
int strcmpr(const void *first, const void *second);
int maxLength(char* fileA, char* fileB);
double convertTime ( char fileA, char fileB);
double convertDist ( char fileA, char fileB);

int main(int argc, char* argv[]) {
  
	GpStatus fileStatus;
	int i=0;

	int exitCode=0;
	int outputLines=0;
	char cmdArg[128];
	char* miscArg=NULL;
	GpFile* filePtr;
	char* outFilename;
	char keepArg[4];
	char* keepStr=NULL;
	FILE* outFilep;
	bool outFlag=false;
	bool wFlag=false;
	bool rFlag=false;
	bool tFlag=false;

	keepArg[0] = '\0';

	filePtr = malloc(sizeof(GpFile));
	assert(filePtr);
	
	outFilename = malloc(sizeof(char) * 128);

	/*check to see if there is the pathname and then an additional argument*/
	if(argc < 2) {
		fprintf(stderr, "Fatal Error: Not enough arguments.\n");
		return EXIT_FAILURE;
	}

	else if(argc > 3) {
		fprintf(stderr, "Fatal Error: Invalid or too many arguments.\n");
		return EXIT_FAILURE;
	}

	else {
		/*duplicating the argument into a local variable*/
		strcpy(cmdArg, argv[1]);
		
		if(cmdArg == NULL) {
			fprintf(stderr, "Fatal Error: Error reading argument.\n");
			return EXIT_FAILURE;
		}
	}

// 	i=2;
// 	while(i<=argc && outFlag==false && argv[i]!=NULL) {
// 		if(argv[i] != NULL && strCaseStr(argv[i], ".txt")!=NULL) {
// 			printf("argv[%d] is %s\n",i, argv[i]); 
// 			strcpy(outFilename, argv[i]);
// 			outFilep = fopen(outFilename, "w+");
// 			if(outFilep == NULL) {
// 				fprintf(stderr, "Error: Invalid Write File\n");
// 				return EXIT_FAILURE;
// 			}
// 			outFlag=true;
// 		}
// 		i++;
// 	}

	if(outFlag==false) {
		outFilep = stdout;
	}

	fileStatus =  readGpFile(stdin, filePtr);

	if(fileStatus.code != OK) {
		fprintf(stderr, "Fatal Error: readGpFile returns error %d on line %d\n", fileStatus.code, fileStatus.lineno);
		return EXIT_FAILURE;
	}

	if (strcmp(cmdArg, "-info") == 0) {

		exitCode = gpsInfo(outFilep, filePtr);
	}

	else if (strcmp(cmdArg, "-discard") == 0) {
		miscArg = strdup(argv[2]);
		assert(miscArg);
		if(miscArg == NULL) {
			fprintf(stderr, "Fatal Error: Error reading argument.\n");
			return EXIT_FAILURE;
		}

		exitCode = gpsDiscard(filePtr, miscArg);
		outputLines = writeGpFile(outFilep, filePtr);
// 		printf("Number of Lines outputted: %d\n", outputLines);
	
		if (outputLines==0) {
			fprintf(stderr, "Warning: writing empty output file\n");
			return EXIT_SUCCESS;
		}
	}
	
	else if (strcmp(cmdArg, "-keep") == 0) {
		miscArg = strdup(argv[2]);
		assert(miscArg);
		if(miscArg == NULL) {
			fprintf(stderr, "Fatal Error: Error reading argument.\n");
			return EXIT_FAILURE;
		}

		for(i=0; i<strlen(miscArg); i++) {
			switch(miscArg[i]) {
				case 'w':
					wFlag=true;
					break;
	
				case 'r':
					wFlag=true;
					rFlag=true;
					break;
	
				case 't':
					tFlag=true;
					break;
	
				default:
					fprintf(stderr, "Fatal Error: Invalid argument '%c' found\n", miscArg[i]);
					return EXIT_FAILURE;
					break;
	
			}
		}

		if(wFlag==true) {
			strcat(keepArg, "w");
		}

		if(rFlag==true) {
			strcat(keepArg, "r");
		}

		if(tFlag==true) {
			strcat(keepArg, "t");
		}

		if(wFlag==true && rFlag==true && tFlag==true) {
			outputLines = writeGpFile(outFilep, filePtr);
// 			printf("Number of Lines outputted: %d\n", outputLines);
		
			if (outputLines==0) {
				fprintf(stderr, "Warning: writing empty output file\n");
				return EXIT_SUCCESS;
			}
		}

		else {
			keepStr = keepSort(keepArg);
			exitCode = gpsDiscard(filePtr, keepStr);
			outputLines = writeGpFile(outFilep, filePtr);
	// 		printf("Number of Lines outputted: %d\n", outputLines);
		
			if (outputLines==0) {
				fprintf(stderr, "Warning: writing empty output file\n");
				return EXIT_SUCCESS;
			}

			if (keepStr) free(keepStr);
		}
	}
	
	else if (strcmp(cmdArg, "-sortwp") == 0) {


		gpsSort(filePtr);

		outputLines = writeGpFile(outFilep, filePtr);
// 		printf("Number of Lines outputted: %d\n", outputLines);
	
		if (outputLines==0) {
			fprintf(stderr, "Warning: writing empty output file\n");
			return EXIT_SUCCESS;
		}

	}

	else if (strcmp(cmdArg, "-merge") == 0) { 

		if(miscArg) free(miscArg);
		miscArg = strdup(argv[2]);
		assert(miscArg);
		if(miscArg == NULL) {
			fprintf(stderr, "Fatal Error: Error reading argument.\n");
			return EXIT_FAILURE;
		}

		gpsMerge(filePtr, miscArg);

		outputLines = writeGpFile(outFilep, filePtr);
// 		printf("Number of Lines outputted: %d\n", outputLines);
	
		if (outputLines==0) {
			fprintf(stderr, "Warning: writing empty output file\n");
			return EXIT_SUCCESS;
		}
		
	}	

	else {
		fprintf(stderr, "Error: Invalid or Unexpected argument\n");
		return EXIT_FAILURE;
	}	

	if(outFilep) fclose(outFilep);

	if (miscArg) free(miscArg);
	if(outFilename) free(outFilename);
	if(filePtr) {
		freeGpFile(filePtr);
		free (filePtr);
	}
	return exitCode;
}


/*	Function: gpsInfo
	Pre-Conditions: An output file pointer and a filled in GpFile structure
	Post-Conditions: The number of waypoints, routes, trackpoints and maximum extent of NE and SW all outputted to outfile pointer
	Description: This file goes through the GpFile to fetch the number of waypoints, routes, and trackpoints and also comapres the greatest extent
		     of the latitudes and longitudes between waypoints and trackpoints.
*/
int gpsInfo( FILE *const outfile, const GpFile *filep ){

	double maxNElat=0;
	double maxNElon=0;
	double maxSWlat=0;
	double maxSWlon=0;
	char NElatSign;
	char NElonSign;
	char SWlatSign;
	char SWlonSign;
	int i=0;
	int trackno=0;
	bool sortFlag = true;

	if(filep->ntrkpts>0) {
		GpTrack* tp;
		trackno = getGpTracks(filep, &tp);
	
		maxNElat = (tp)[0].NEcorner.lat;
		maxNElon = (tp)[0].NEcorner.lon;
		maxSWlat = (tp)[0].SWcorner.lat;
		maxSWlon = (tp)[0].SWcorner.lon;
	
		/*Compare all gpTracks corners to find the largest one*/
		for(i=0; i<trackno; i++) {
			if((tp)[i].NEcorner.lat > maxNElat) {
				maxNElat = (tp)[i].NEcorner.lat;
			}
	
			if((tp)[i].NEcorner.lon > maxNElon) {
				maxNElon = (tp)[i].NEcorner.lon;
			}
	
			if((tp)[i].SWcorner.lat < maxSWlat) {
				maxSWlat = (tp)[i].SWcorner.lat;
			}
	
			if((tp)[i].SWcorner.lon < maxSWlon) {
				maxSWlon = (tp)[i].SWcorner.lon;
			}
		}
	
		if(tp) free(tp);
	}

	/*Compare the greatest GpTrack value against all the Waypoint coordinates*/
	if(filep->nwaypts>0) {

		if(filep->ntrkpts==0) {
			maxNElat = filep->waypt[i].coord.lat;
			maxNElon = filep->waypt[i].coord.lon;
			maxSWlat = filep->waypt[i].coord.lat;
			maxSWlon = filep->waypt[i].coord.lon;
		}

		for(i=0; i<filep->nwaypts; i++) {
			if(filep->waypt[i].coord.lat > maxNElat) {
				maxNElat = filep->waypt[i].coord.lat;
			}
		
			if(filep->waypt[i].coord.lon > maxNElon) {
				maxNElon = filep->waypt[i].coord.lon;
			}
	
			if(filep->waypt[i].coord.lat < maxSWlat) {
				maxSWlat = filep->waypt[i].coord.lat;
			}
	
			if(filep->waypt[i].coord.lon < maxSWlon) {
				maxSWlon = filep->waypt[i].coord.lon;
			}
	
		}
	}

	if(maxNElat < 0) {
		NElatSign = 'S';
		maxNElat = maxNElat * (-1);
	}
	
	else {
		NElatSign = 'N';
	}

	if(maxNElon < 0) {
		NElonSign = 'W';
		maxNElon = maxNElon * (-1);
	}

	else {
		NElonSign = 'E';
	}

	if(maxSWlat < 0) {
		SWlatSign = 'S';
		maxSWlat = maxSWlat * (-1);
	}

	else {
		SWlatSign = 'N';
	}

	if(maxSWlon < 0) {
		SWlonSign = 'W';
		maxSWlon = maxSWlon * (-1);
	}

	else {
		SWlonSign = 'E';
	}

	/*Determine if the waypoints are sorted*/
	i=0;
	sortFlag = true;
	while(sortFlag == true && i < filep->nwaypts-1) {
		if(strcmp(filep->waypt[i].ID, filep->waypt[i+1].ID) > 0) {
			sortFlag = false;
		}
		i++;
	}

	fprintf(outfile, "%d waypoints", filep->nwaypts);
	/*If there are no way points, suppress sort message*/
	if(filep->nwaypts > 0) {
		if(sortFlag == true) {
			fprintf(outfile, " (sorted)\n");
		}
		else {
			fprintf(outfile, " (not sorted)\n");
		}
	}

	else {
		fprintf(outfile, "\n");
	}
	
	fprintf(outfile, "%d routes\n", filep->nroutes);
	
	fprintf(outfile, "%d trackpoints\n", filep->ntrkpts);
	
	fprintf(outfile, "%d tracks\n", trackno);

	fprintf(outfile, "Extent: SW %c%lf %c%lf to NE %c%lf %c%lf\n", SWlonSign, maxSWlon, SWlatSign, maxSWlat, NElonSign, maxNElon, NElatSign, maxNElat);


	return EXIT_SUCCESS;
}

/*	Function: gpsDiscard
	Pre-Conditions: A filled in GpFile structure and the string of which type of point to discard
	Post-Conditions: A modified GpFile with the specified type of point discarded
	Errors Returned: EXIT_FAILURE
	Description: gpsDiscard takes in a string from the commandline argument and uses that string to determine what type of point to discard. It returns the pointer back to
		     main after discarding.
*/
int gpsDiscard( GpFile *filep, const char *which ){

	int i=0;
	int j=0;
	bool wFlag=false;
	bool rFlag=false;
	bool tFlag=false;

	for(i=0; i<strlen(which); i++) {
		if(which[i] == 'w') {

			wFlag=true;
			rFlag=true;
		}

		else if(which[i] == 'r' && rFlag == false) {

			rFlag=true;
		}

		else if(which[i] == 't' ) {

			tFlag=true;
		}

		else if (which[i] != 'w' && which[i] == 'r' && which[i] == 't') {
			fprintf(stderr, "Fatal Error: Invalid argument '%c' found\n", which[i]);
			return EXIT_FAILURE;
		}
	}

	if(wFlag==true && rFlag==true && tFlag==true) {
		fprintf(stderr, "Warning: No Components left after discard\n");
		return EXIT_SUCCESS;
	}

	if(wFlag == true) {
		for(j=0; j<filep->nwaypts; j++) {
		if((filep->waypt[j].comment)!=NULL){
				free(filep->waypt[j].comment);
			}

			if((filep->waypt[j].ID)!=NULL) {
				free(filep->waypt[j].ID);
			}
			if((filep->waypt[j].symbol)!=NULL) {
				free(filep->waypt[j].symbol);
			}
		}
		filep->waypt = NULL;
		free(filep->waypt);
		filep->nwaypts=0;

	}

	if(rFlag == true) {

		if(filep->route != NULL) {
			for(j=0; j<filep->nroutes; j++) {
				if((filep->route[j])!=NULL) {
					free(filep->route[j]);
					if((filep->route[j]->comment)!=NULL) {
						free(filep->route[j]->comment);
					}
				}
			}	
			free(filep->route);
		}
		filep->route = NULL;
		filep->nroutes=0;
	}

	if(tFlag == true) {
		if(filep->trkpt != NULL) {
			for(j=0; j<filep->ntrkpts; j++){
				if(filep->trkpt[j].segFlag==true) {
					if((filep->trkpt[j].comment)!=NULL) {
						free(filep->trkpt[j].comment);
					}
				}
	
			}
			free(filep->trkpt);
		}
		filep->trkpt= NULL;
		filep->ntrkpts=0;
	}

	return EXIT_SUCCESS;
}

/*	Function: gpsSort
	Pre-Conditions: An opened and filled in GpFile structure
	Post-Conditions: A modified GpFile with the waypoints sorted based on the quicksort algorithm provided by the standard C library
	Errors Returned: None
	Description: This function ultilizes the standard C library function "qsort" to sort and modify the existing GpFile by sorting its waypoint elements.
*/
int gpsSort( GpFile *filep ){	

	int i=0;
	int j=0;
	int h=0;

	char** unsortedIDs=NULL;
	char thisWaypt[128];

	unsortedIDs = malloc(sizeof(char*)*filep->nwaypts);
	assert(unsortedIDs);
	
	for(i=0; i<filep->nwaypts; i++) {
		unsortedIDs[i] = malloc(sizeof(char)*strlen(filep->waypt[i].ID) + 1);
		assert(unsortedIDs[i]);
		strcpy(unsortedIDs[i], filep->waypt[i].ID);
	}

	qsort(filep->waypt, filep->nwaypts, sizeof(GpWaypt), strcmpr);

	for(i=0; i<filep->nroutes; i++) {
		for(j=0; j<filep->route[i]->npoints; j++) {
			strcpy(thisWaypt, unsortedIDs[filep->route[i]->leg[j]]);

			for(h=0; h<filep->nwaypts; h++) {
				if(strcmp(thisWaypt, filep->waypt[h].ID) == 0) {
					filep->route[i]->leg[j] = h;
				}
			}
		}
	}

	if(unsortedIDs) {
		for(i=0; i<filep->nwaypts; i++) {
			if(unsortedIDs[i]) free(unsortedIDs[i]);
		}
		free(unsortedIDs);
	}

	return EXIT_SUCCESS;

}

/*	Function: gpsMerge
	Pre-Conditions: An opened and filled in GpFile structure and the name of the merging file taken from the command line
	Post-Conditions: A modified GpFile which includes all the information from the merging file
	Errors Returned:EXIT_FAILURE
	Description: This function takes all the information from the merging file and merges it with the first GpFile structure. 
		     The problem of duplicate waypoints, different ID and symbol lengths and date/timezone differences are addressed
*/
int gpsMerge( GpFile *filep, const char *const fnameB ){

	GpFile* fileA = filep;
	GpFile* fileB;
	GpStatus fileStatus;
	FILE* inputfileB;
	int maxIDLen=0;
	int maxSymLen=0;
	int i=0;
	int j=0;
	int oldnwaypts=0;
	int maxRouteno=0;
	int convTZ=0;
	double convDist=0.0;
	double convTime=0.0;
	char* thisStr=NULL;
	char* IDbuffer=NULL;
	char* Symbuffer=NULL;
	char copies;

	fileB = malloc(sizeof(GpFile));
	assert(fileB);

	inputfileB = fopen(fnameB, "r");
	if(inputfileB == NULL) {
		fprintf(stderr, "Fatal Error: Cannot open file B for reading\n");
		return EXIT_FAILURE;
	}

	fileStatus =  readGpFile(inputfileB, fileB);

	if(fileStatus.code != OK) {
		fprintf(stderr, "Fatal Error: readGpFile returns error %d on line %d for file B\n", fileStatus.code, fileStatus.lineno);
		return EXIT_FAILURE;
	}

	if(fileA->nwaypts>0 && fileB->nwaypts>0) {
		fileA->waypt = realloc(fileA->waypt, sizeof(GpWaypt) * (fileA->nwaypts + fileB->nwaypts));
		maxIDLen = maxLength(fileA->waypt[0].ID, fileB->waypt[0].ID);
		maxSymLen = maxLength(fileA->waypt[0].symbol, fileB->waypt[0].symbol);
		IDbuffer = malloc(sizeof(char) * maxIDLen);
		Symbuffer = malloc(sizeof(char) * maxSymLen);

		for(i=0; i<fileB->nwaypts; i++) {
			fileA->waypt[(fileA->nwaypts) + i].textChoice = fileB->waypt[i].textChoice;
			fileA->waypt[(fileA->nwaypts) + i].textPlace = fileB->waypt[i].textPlace;
			fileA->waypt[(fileA->nwaypts) + i].ID = malloc(sizeof(char) * (strlen(fileB->waypt[i].ID)+1));
			assert(fileA->waypt[(fileA->nwaypts) + i].ID);
			strcpy(fileA->waypt[(fileA->nwaypts) + i].ID, fileB->waypt[i].ID);\
			fileA->waypt[(fileA->nwaypts) + i].coord.lat = fileB->waypt[i].coord.lat;
			fileA->waypt[(fileA->nwaypts) + i].coord.lon = fileB->waypt[i].coord.lon;
			fileA->waypt[(fileA->nwaypts) + i].symbol = malloc(sizeof(char) * (strlen(fileB->waypt[i].symbol)+1));
			assert(fileA->waypt[(fileA->nwaypts) + i].symbol);
			strcpy(fileA->waypt[(fileA->nwaypts) + i].symbol, fileB->waypt[i].symbol);
			fileA->waypt[(fileA->nwaypts) + i].comment = malloc(sizeof(char) * (strlen(fileB->waypt[i].comment)+1));
			assert(fileA->waypt[(fileA->nwaypts) + i].comment);
			strcpy(fileA->waypt[(fileA->nwaypts) + i].comment, fileB->waypt[i].comment);
		}
		oldnwaypts = fileA->nwaypts;
		fileA->nwaypts += fileB->nwaypts;

		for(i=0; i<fileA->nwaypts; i++) {
			fileA->waypt[i].ID = padSpace(fileA->waypt[i].ID, maxIDLen);
			fileA->waypt[i].symbol = padSpace(fileA->waypt[i].symbol, maxSymLen);
		}
	}

	/*Duplicates file B's elements into file A*/
	if(fileA->nwaypts==0 && fileB->nwaypts>0) {
		fileA->waypt = realloc(fileA->waypt, sizeof(GpWaypt) * (fileB->nwaypts));

		for(i=0; i<fileB->nwaypts; i++) {
			fileA->waypt[i].ID = strdup(fileB->waypt[i].ID);
			assert(fileA->waypt[i].ID);
			fileA->waypt[i].coord.lat = fileB->waypt[i].coord.lat;
			fileA->waypt[i].coord.lon = fileB->waypt[i].coord.lon;
			fileA->waypt[i].textChoice = fileB->waypt[0].textChoice;
			fileA->waypt[i].textPlace = fileB->waypt[0].textPlace;
			fileA->waypt[i].symbol = strdup(fileB->waypt[i].symbol);
			assert(fileA->waypt[i].symbol);	
			fileA->waypt[i].comment = strdup(fileB->waypt[i].comment);
			assert(fileA->waypt[i].comment);
		}
	}

	thisStr = malloc(sizeof(char) * maxIDLen+1);

	/*Checking through the new filepointer to find duplicate waypoints*/
	for(i=0; i<fileA->nwaypts; i++) {
		copies = '0';
		strcpy(thisStr, fileA->waypt[i].ID);
		for(j=i+1; j<fileA->nwaypts; j++) {
			/*If there is a match, it will append a 0 at the last character*/
			if(strcmp(fileA->waypt[j].ID, thisStr) == 0) {
				fileA->waypt[j].ID[maxIDLen-1] = copies;
				copies++;
			}
		}
	}

	for(i=0; i<fileA->nroutes; i++) {
		if(fileA->route[i]->number > maxRouteno) {
			maxRouteno = fileA->route[i]->number;
		}
	}
	
// 	int remainder=0;

// 	remainder = maxRouteno%100;
// fprintf(stderr, "remainder is %d\n", remainder);
	maxRouteno = maxRouteno / 100;
	maxRouteno += 1;
	maxRouteno = (maxRouteno*100);
	if(fileA->nroutes ==0) maxRouteno = 0;
// 	maxRouteno += 1;

	if(fileA->nroutes>0 && fileB->nroutes>0) {
		fileA->route = realloc(fileA->route, sizeof(GpRoute*) * (fileA->nroutes + fileB->nroutes));

		for(i=0; i<fileB->nroutes; i++) {
			fileA->route[fileA->nroutes + i] = malloc(sizeof(GpRoute) + (sizeof(int) * fileB->route[i]->npoints));
			fileA->route[fileA->nroutes + i]->number = maxRouteno + fileB->route[i]->number%100;
			fileA->route[fileA->nroutes + i]->comment = malloc(sizeof(char) * (strlen(fileB->route[i]->comment)+1));
			assert(fileA->route[fileA->nroutes + i]->comment);
			strcpy(fileA->route[fileA->nroutes + i]->comment, fileB->route[i]->comment);
			fileA->route[fileA->nroutes + i]->npoints = fileB->route[i]->npoints;
			for(j=0; j<fileB->route[i]->npoints; j++) {
				fileB->route[i]->leg[j] += (fileA->nwaypts - fileB->nwaypts );
				fileA->route[fileA->nroutes + i]->leg[j] = fileB->route[i]->leg[j];
			}

		}
	}

	/*Duplicates file B's elements into file A*/
	if(fileA->nroutes==0 && fileB->nroutes>0) {
		fileA->route = realloc(fileA->route, sizeof(GpRoute*) * (fileB->nroutes));
		for(i=0; i<fileB->nroutes; i++) {
			fileA->route[i] = malloc(sizeof(GpRoute) + (sizeof(int) * fileB->route[i]->npoints));
			fileA->route[i]->number = fileB->route[i]->number;
			fileA->route[i]->comment = malloc(sizeof(char) * (strlen(fileB->route[i]->comment)+1));
			assert(fileA->route[i]->comment);
			strcpy(fileA->route[i]->comment, fileB->route[i]->comment);
			fileA->route[i]->npoints = fileB->route[i]->npoints;
			for(j=0; j<fileB->route[i]->npoints; j++) {
				fileA->route[i]->leg[j] = fileB->route[i]->leg[j];
			}
		}
	}
	fileA->nroutes += fileB->nroutes;

	
	if(fileA->ntrkpts>0 && fileB->ntrkpts>0) {
		fileA->trkpt = realloc(fileA->trkpt, sizeof(GpTrkpt) * (fileA->ntrkpts + fileB->ntrkpts));
		convDist = convertDist(fileB->unitHorz, fileA->unitHorz);
		convTime = convertTime(fileB->unitTime, fileA->unitTime);
		convTZ = fileA->timeZone - fileB->timeZone;
		for(i=0; i<fileB->ntrkpts; i++) {
			fileA->trkpt[fileA->ntrkpts + i].coord.lat = fileB->trkpt[i].coord.lat;
			fileA->trkpt[fileA->ntrkpts + i].coord.lon = fileB->trkpt[i].coord.lon;
			fileA->trkpt[fileA->ntrkpts + i].dateTime = fileB->trkpt[i].dateTime + convTZ*3600;	//The datetime needs to be modifed by the timezone difference
			fileA->trkpt[fileA->ntrkpts + i].segFlag = fileB->trkpt[i].segFlag;
			if (fileA->trkpt[fileA->ntrkpts + i].segFlag) {
				fileA->trkpt[fileA->ntrkpts + i].comment = malloc(sizeof(char) * strlen(fileB->trkpt[i].comment)+1);
				assert(fileA->trkpt[fileA->ntrkpts + i].comment);
				strcpy(fileA->trkpt[fileA->ntrkpts + i].comment, fileB->trkpt[i].comment);
			}
 		fileA->trkpt[fileA->ntrkpts + i].speed = ((fileB->trkpt[i].speed / convDist)*convTime);	//speed needs to be converted to file A's specified units
 		fileA->trkpt[fileA->ntrkpts + i].dist = (fileB->trkpt[i].dist / convDist);	//Distance needs to be converted to file A's specified units
 		fileA->trkpt[fileA->ntrkpts + i].duration = (fileB->trkpt[i].duration);
		}

	}
	/*Duplicates file B's elements into file A*/
	else if(fileA->ntrkpts==0 && fileB->ntrkpts>0) {
		for(i=0; i<fileB->ntrkpts; i++) {
			fileA->trkpt = realloc(fileA->trkpt, sizeof(GpTrkpt) * (fileB->ntrkpts));
			fileA->trkpt[i].coord.lat = fileB->trkpt[i].coord.lat;
			fileA->trkpt[i].coord.lon = fileA->trkpt[i].coord.lon;
			fileA->trkpt[i].coord.lon = fileA->trkpt[i].coord.lon;
			fileA->trkpt[i].segFlag = fileB->trkpt[i].segFlag;
			fileA->trkpt[i].comment = malloc(sizeof(char) * strlen(fileB->trkpt[i].comment));
			assert(fileA->trkpt[i].comment);
			strcpy(fileA->trkpt[i].comment, fileB->trkpt[i].comment);
			fileA->trkpt[i].speed = (fileB->trkpt[i].speed);
			fileA->trkpt[i].dist = (fileB->trkpt[i].dist);
			fileA->trkpt[i].duration = (fileB->trkpt[i].duration);
		}
	}

	fileA->ntrkpts += fileB->ntrkpts;

	
	if(fileB) {
		freeGpFile(fileB);
		free(fileB);
	}
	
	if(inputfileB) fclose(inputfileB);
	
	if(IDbuffer) free(IDbuffer);
	
	if(Symbuffer) free(Symbuffer);

	return EXIT_SUCCESS;
	
}

/*
	Function Name: padSpace
	Pre-Conditions: A valid string and its length
	Post-Conditions: The inputted string being passed out with the padded spaces
	Description: This function pads spaces to string for ID and Symbol so that the lengths match the convention
*/
char* padSpace(char* src, int Length) {
	if(src==NULL) {
		fprintf(stdout, "Fatal Error: String is NULL\n");
		return NULL;
	}	

	int i=0;
	char* buffer=NULL;

	buffer = malloc(sizeof(char) * Length);
	assert(buffer);
	strcpy(buffer, src);

	for(i=0; i<(strlen(buffer) - Length); i++) {
		strcat(buffer, " ");
	}

	free(src);
	return buffer;

}

/*
	Function Name: maxLength
	Pre-Conditions: Two strings to be compared 
	Post-Conditions: The longest length of the two strings
	Description: This function uses strcmp to compare the length between the two strings and returns the longest length
*/
int maxLength(char* fileA, char* fileB) {

	if(strlen(fileA) > strlen(fileB)) {
		return strlen(fileA);
	}

	else {
		return strlen(fileB);
	}
}

/*
	Function Name: keepSort
	Pre-Conditions: A valid string of which will have to contain "wrt" or the combination of.
	Post-Conditions: The element to be discarded based on inverting what to "keep"
	Description: This function returns the string which is to be passed into discard to determien what the user will discard from the GpFile structure
*/
char* keepSort(char* which) {

	char* buffer=NULL;

	buffer = malloc(sizeof(char) *128);

	if (strcmp(which, "w")==0) {
		strcpy(buffer, "rt");
	}

	else if (strcmp(which, "wr")==0) {
		strcpy(buffer, "t");
	}

	else if (strcmp(which, "wt")==0) {
		strcpy(buffer, "r");
	}

// 	else if (strcmp(which, "r")==0) {
// 		strcpy(buffer, "wt");
// 	}

// 	else if (strcmp(which, "rt")==0) {
// 		strcpy(buffer, "w");
// 	}

	else if (strcmp(which, "t")==0) {
		strcpy(buffer, "wr");
	}

	else {
		fprintf(stderr, "Error: Unrecognized string\n");
	}

	return buffer;
}

/*A list of conversion units*/
double convertDist ( char fileA, char fileB ) {

    switch (fileA) {
        case ('M'): // Metres
            switch (fileB) {
                case ('K'): 
			return 1000.0; 
			break;
                case ('M'):
			return 1.0; 
			break;
                case ('F'): 
			return 0.3048; 
			break;
                case ('N'): 
			return 1852.00; 
			break;
                case ('S'): 
			return 1609.344; 
			break;
            }
            break;
        case ('K'): // Kilometres
            switch (fileB) {
                case ('K'): 
			return 1.0; 
			break;
                case ('M'): 
			return 0.001; 
			break;
                case ('F'): 
			return 0.0003048; 
			break;
                case ('N'): 
			return 1.852; 
			break;
                case ('S'): 
			return 1.609344; 
			break;
            }
            break;
        case ('F'): // Feet
            switch (fileB) {
                case ('K'): 
			return 3280.8399; 
			break;
                case ('M'): 
			return 3.28084; 
			break;
                case ('F'): 
			return 1.0; 
			break;
                case ('N'): 
			return 6076.1155; 
			break;
                case ('S'): 
			return 5280.0; 
			break;
            }
            break;
        case ('N'): // Nautical Miles
            switch (fileB) {
                case ('K'): 
			return 0.539957; 
			break;
                case ('M'): 
			return 0.000539957; 
			break;
                case ('F'): 
			return 0.000164579; 
			break;
                case ('N'): 
			return 1.0; 
			break;
                case ('S'): 
			return 0.868976; 
			break;
            }
            break;
        case ('S'): // Statutues Miles
            switch (fileB) {
                case ('K'): 
			return 0.621371; 
			break;
                case ('M'): 
			return 0.000621371; 
			break;
                case ('F'): 
			return 0.000189394; 
			break;
                case ('N'): 
			return 1.150779; 
			break;
                case ('S'): 
			return 1.0; 
			break;
            }
            break;
    }

    return 0;
}

/*Converting from Hour to seconds and Vice versa*/
double convertTime ( char fileA, char fileB ) {

    switch (fileA) {
        case ('S'): // Metres
            switch (fileB) {
                case ('S'): 
			return 1.0;
                case ('H'): 
			return 3600.0;
            }
            break;

        case ('H'): // Kilometres
            switch (fileB) {
                case ('S'): 
			return 0.00027778;
                case ('H'): 
			return 1.0;
            }
            break;
    }
    return 0;

}

int strcmpr(const void *a, const void *b) {
	const char **a2 = (const char **)a; 
	const char **b2 = (const char **)b;  
	return strcmp(*a2, *b2);
}
