/********************************************************************************************************
* CIS*2750 Assignment 1 W10
* File Name: gputil.c
* Author: Kwok Lau 0595198
* Email: kwok@uoguelph.ca
* Creation Date: Jan 18 2008
* Description: This file consists of modules to parse GPSU files and report any errors that are found
********************************************************************************************************/
#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdbool.h>
#include<string.h>
#include<assert.h>
#include"gputil.h"
#include<time.h>


#define _BUFFER 1024
#define D fprintf(stderr, "line %d of %s of %s\n", __LINE__, __func__, __FILE__);

typedef struct {
	bool dayFlag;
	bool mthFlag;
	bool yrFlag;
} dateFlags;

void initializeFilep (GpFile* filep);
void makeUpper (char* src);
void printTrkPt(GpTrkpt *tp);
void printWayPt(GpWaypt *wp);
void makeLower (char* src);
char* Trim(char* src, int srcsize);
char* getID(int* index, char* lnBuffer, int FLen);
char* getWord(int* index, char* lnBuffer);
char* getCmmt(int* index, char* lnBuffer);
char* getRoute(int* index, char* lnBuffer);
char* sortDateFormat(char* src);
char* sortDateType (char* src, dateFlags* flags);
char* revertDateType (char* src);
char* getLine(FILE* gpf);
long durInSec(char* duration);

/*  
	Function: readGpFile
	Pre-Conditions:	gpf is assumed to be an open file positioned at the beginning and filep is assumed to be
			a structure with allocated memory
	Post-Conditions:filep will be available for access if the file is read to a normal EOF, however it will
			need to be freed in the main process					  
	Errors returned: IOERR, UNKREC, BADSEP, FILTYP, DATUM, COORD, NOFORM, FIELD, VALUE, DUPRT
			(Please refer to gputil.h for error code definitions)
*/
GpStatus readGpFile(FILE *const gpf, GpFile *filep) {

	GpStatus status;

	int ln=0;
	char* lnBuffer = NULL;
	char* fBuffer = NULL;
	char* strptr = NULL;
	char GPSUBuffer[5];
	char* tzTok = NULL;
	char* unTok = NULL;
	char tzBuffer[5];
	bool fFlag=false;
	bool brFlag=false;
	bool rFlag=false;

	
	status.code = OK;
	status.lineno = 0;

	initializeFilep (filep);

	if(gpf == NULL) {
		status.code = IOERR;
		status.lineno = ln;
		brFlag = true;
	}

	else if(ferror(gpf) != 0) {
		status.code = IOERR;
		status.lineno = ln;
		brFlag = true;
	}

	/*read in a line and malloc the appropriate space for it*/	
	while((lnBuffer = getLine(gpf))!= NULL && brFlag==false) {
		ln++;
/*		printf("Line %d: %s\n", ln, lnBuffer); */
		/*Check second character for a space, if not found return error code BADSEP*/
		if(isalnum(lnBuffer[0])) {
			if ((int)lnBuffer[1] != 32) {
				status.code = BADSEP;
				status.lineno = ln;
				brFlag=true;
			}
			
			else {
				switch(toupper(lnBuffer[0])) {
					
					case 'C':
						break;
						
					case 'A':
						break;
						
					case 'H':
						break;
						
					case 'I':
							sscanf(lnBuffer, "%s %s", GPSUBuffer, GPSUBuffer);
							if(strcmp(GPSUBuffer, "GPSU")!=0) {
								status.code = FILTYP;
								status.lineno = ln;
								brFlag=true;
							}

						break;
						
					case 'S':
						if(strCaseStr(lnBuffer, "DateFormat")!=NULL){
							strptr = sortDateFormat(lnBuffer);
							if(strstr(strptr, "ERR")==NULL){
		
								if(filep != NULL) {
									filep->dateFormat = malloc(sizeof(char)*(strlen(strptr)+1));	
									assert(filep->dateFormat!=NULL);
									
									strcpy(filep->dateFormat, strptr);
								}
		
								else {
									status.code = IOERR;
									status.lineno = ln;
									brFlag = true;
								}
							}

							else {
								status.code = VALUE;
								status.lineno = ln;
								brFlag=true;
							}

							free (strptr);
						}

						if(strCaseStr(lnBuffer, "Timezone")!=NULL){
							/*Search to see if the timezone is + or */
							if(strchr(lnBuffer, '+')!=NULL) {
								strcpy(tzBuffer, "+");
							}
							else if (strchr(lnBuffer, '-')!=NULL){
								strcpy(tzBuffer, "-");
							}

							/*Isolating the HH*/
							tzTok = strtok(lnBuffer, "+-");
							tzTok = strtok(NULL, ":");
		
							if(atoi(tzTok) < 0 || atoi(tzTok) > 12){
								status.code = VALUE;
								status.lineno = ln;
								brFlag=true;
							}

							else {
								strcat(tzBuffer, tzTok);
								filep->timeZone = atoi(tzBuffer);
							}
						}
						
						if(strCaseStr(lnBuffer, "Units")!=NULL){
							unTok = strtok(lnBuffer, "=");
							unTok = strtok(NULL, ",");

								makeUpper(unTok);
								
								switch(unTok[0]) {
									case 'K':
										filep->unitHorz = 'K';
										filep->unitTime = 'H';
										break;

									case 'N':
										filep->unitHorz = 'N';
										filep->unitTime = 'H';
										break;

									case 'S':
										filep->unitHorz = 'S';
										filep->unitTime = 'H';
										break;

									case 'F':
										filep->unitHorz = 'F';
										filep->unitTime = 'S';
										break;

									case 'M':
										filep->unitHorz = 'M';
										filep->unitTime = 'S';
										break;
						
									default:
										status.code = VALUE;
										status.lineno = ln;
										brFlag=true;
								}
	
						}
						break;
			
					case 'M':
						/*Search the line for the string, disregarding case-sensitivity*/
						if(strCaseStr(lnBuffer, "WGS 84")==NULL){
							status.code = DATUM;
							status.lineno = ln;
							brFlag=true;
						}

						break;
					
					case 'U':
						/*Search the line for the string, disregarding case-sensitivity*/
						if(strCaseStr(lnBuffer, "LAT LON DEG")==NULL){
							status.code = COORD;
							status.lineno = ln;
							brFlag=true;
						}
						break;
						
					case 'F':
						if(fBuffer == NULL) {

							fBuffer = malloc(sizeof(char)*strlen(lnBuffer)+1);
							assert(fBuffer!=NULL);
						}

						else {
							free(fBuffer);
							fBuffer = malloc(sizeof(char)*strlen(lnBuffer)+1);
							assert(fBuffer!=NULL);
						}

						strcpy(fBuffer, lnBuffer);
						fFlag=1;
						break;
						
					case 'W':
						if(fFlag!=true) {
							status.code = NOFORM;
							status.lineno = ln;
							brFlag=true;
						}

						else if (rFlag != true) {
							filep->nwaypts += 1;
							filep->waypt = realloc(filep->waypt, sizeof(GpWaypt)*filep->nwaypts);
							assert(filep->waypt!=NULL);
							status.code = scanGpWaypt(lnBuffer, fBuffer, &filep->waypt[filep->nwaypts-1]);
							if(status.code!=OK){
								status.lineno = ln;
								brFlag=true;
							}
						}

						else {

							if(status.code!=OK){
								status.lineno = ln;
								brFlag=true;
							}

							else {
							  	filep->route[filep->nroutes-1]->npoints++;
								filep->route[filep->nroutes-1] = realloc(filep->route[filep->nroutes-1], sizeof(GpRoute) + (sizeof(int)*filep->route[filep->nroutes-1]->npoints));
								assert(filep->route[filep->nroutes-1]!=NULL);

							}
							/*passing in line buffer, f buffer, waypoints, coutn of waypoints, and the most recent route structure created*/
							status.code = scanGpLeg(lnBuffer, fBuffer, filep->waypt, filep->nwaypts, filep->route[filep->nroutes-1]);
						}
						break;
						
					case 'R':

						filep->nroutes += 1;
						filep->route = realloc(filep->route, sizeof(GpRoute*)*filep->nroutes);
						assert(filep->route!=NULL);
						filep->route[filep->nroutes-1] = malloc(sizeof(GpRoute) + sizeof(int)); 
						assert(filep->route[filep->nroutes-1]);

						status.code = scanGpRoute(lnBuffer, filep->route[filep->nroutes-1]);

						if(status.code!=OK){
							status.lineno = ln;
							brFlag=true;
						}

						else {
							rFlag = true;
						}

						if (filep->nroutes > 1){
							for(int i=0; i<filep->nroutes-1; i++) {
								for(int j=i+1; j<filep->nroutes; j++) {
									if(filep->route[i]->number == filep->route[j]->number) {
										status.code = DUPRT;
										status.lineno = ln;
										brFlag=true;
									}
								}
							}
						}

						break;
					
					case 'T':
						if(fFlag!=true) {
							status.code = NOFORM;
							status.lineno = ln;
							brFlag=true;
						}

						else {
							filep->ntrkpts += 1;
							filep->trkpt = realloc(filep->trkpt, sizeof(GpTrkpt)*filep->ntrkpts);
							assert(filep->trkpt!=NULL);
	
							status.code = scanGpTrkpt(lnBuffer, fBuffer, filep->dateFormat, &filep->trkpt[filep->ntrkpts-1]);
							if(status.code!=OK){
								status.lineno = ln;
								brFlag=true;
							}
						}
						break;
						
					default:
						status.code = UNKREC;
						status.lineno = ln;
						brFlag=true;
						break;
					
				}
			}
		}

		else {
			rFlag=false;
/*			printf("empty line at line %d found\n", ln);*/
		}

		/*Clearing lnBuffer*/
		free(lnBuffer);	
		
	}

	if(ln==0) {
		status.code = IOERR;
		status.lineno = ln;
	}
	
/*	printf("File Read Successfully\n");*/
	if(fFlag==1) {
		free(fBuffer);
	}

	return status;	
}

/*Setting enums for scanGpWaypt, scanGpRoute, scanGpTrkpt*/
typedef enum { 
	UNKWN=-1,	//Unknown Field					-1
	END=0,		//End of line					0
	LAT,		//Latitude					1
	LON,		//Longitude					2
	ALT,		//Altitude					3
	DATE,		//Date						4
	TIME,		//Time						5
	DURA,		//Duration					6
	SFLAG,		//S Flag					7
	DIST,		//Distance travelled since last track point	8
	SPD,		//Speed						9
	STPTM,		//Stop Time					10
	SYMB,		//Symbol					11
	ID,		//ID						12
	PNTS,		//PNTS						13
	TFLD,		//T Field					14
	OFLD,		//O Field					15
	CMMT,		//Comment					16
} GpFormat;

/*	Function: scanGpWaypt
	Pre-Conditions: Pointer to an input line, pointer to a stored F-line, pointer to the GpWaypt structure
	Post-Conditions: error code, OK if there are no errors in inputting and a filled out GpWaypt structure
	Errors Returned: VALUE, FIELD (Please refer to gputil.h for error code definitions)
	Comment: The function takes in 2 lines and intreprets them. Then based on the order of information, they
		 fill out the appropriate variables in the GpWaypt structure with the appropriate information.
*/
GpError scanGpWaypt( const char *buff, const char *fieldDef, GpWaypt *wp) {

	GpCoord* coord = NULL;
	char* lnBuffer = NULL;
	char* fBuffer = NULL;
	char* fTok = NULL;
	char* tempTok = NULL;
	char* chkTok = NULL;
	int strPtr = 0;
	int fieldLen=0;
	int fArray[18];
	int errCode=OK;
	int i=0;
	int fCount=0;
	int IDLen=0;
	int SymLen=0;
	bool errFlag=false;
	bool latFlag=false;
	bool lonFlag=false;
	bool IDFlag=false;
	bool cmmtFlag=false;


	lnBuffer = malloc(sizeof(char)* strlen(buff)+1);
	assert(lnBuffer != NULL);
	strcpy(lnBuffer, buff);

	fBuffer = malloc(sizeof(char)* strlen(fieldDef)+1);
	assert(fBuffer != NULL);
	strcpy(fBuffer, fieldDef);

	coord = malloc(sizeof(GpCoord));
	assert(coord != NULL);
	
	wp->ID = strdup("");
	wp->symbol = strdup("");
	wp->textChoice = 'I';
	wp->textPlace = 2;
	wp->comment = strdup("");

	fTok = strtok(fBuffer+1, " \n\r\0");	

	if(fTok == NULL) {
		errCode = FIELD;
		errFlag = true;
	}

	while(fTok!=NULL && errFlag==false) {
		makeUpper(fTok);

		if(strcmp(fTok, "LATITUDE")==0 || strcmp(fTok, "LAT")==0) {

			fArray[fieldLen] = LAT;
			latFlag = true;
		}
		else if(strcmp(fTok, "LONGITUDE" )==0 || strcmp(fTok, "LON")==0) {

			fArray[fieldLen] = LON;
			lonFlag = true;
		}
		else if(strstr(fTok, "ID")!=NULL) {
			fArray[fieldLen] = ID;
			IDLen = strlen(fTok);
			IDFlag = true;
		}
		else if(strstr(fTok, "SYMBOL")!=NULL) {
			fArray[fieldLen] = SYMB;
			SymLen = strlen(fTok);
		}
		else if(strcmp(fTok, "T")==0) {
			fArray[fieldLen] = TFLD;
		}
		else if(strcmp(fTok, "O")==0) {
			fArray[fieldLen] = OFLD;
		}
		else if(strstr(fTok, "ALT")!=NULL) {
			fArray[fieldLen] = ALT;
		}
		else if(strcmp(fTok, "COMMENT")==0) {
			fArray[fieldLen] = CMMT;
			cmmtFlag=true;
		}
		else {
			errCode = FIELD;
			errFlag = true;
		}
		fTok = strtok(NULL, " \n\r\0");
		fieldLen++;	
	}

	if(latFlag!=true || lonFlag!=true || IDFlag!=true) {
		errCode = FIELD;
		errFlag = true;
	}
	
	/*Point at the beginning of the line and increment it pass the initial value "W"*/
	strPtr++;

	while(i<fieldLen && errFlag!=true) {

		if(fArray[i] == LAT){
			if (coord == NULL) {
				errCode = FIELD;
				errFlag = true;
			}

			tempTok = getWord(&strPtr, lnBuffer);

			if(tempTok==NULL) {
				errCode = FIELD;
				errFlag = true;
			}
			
			else {
				/*If the Latitude is North, or +, or implied positive (no sign)*/
				if(toupper(tempTok[0]) == 'N' || tempTok[0] == '+' || isdigit(tempTok[0])) {
					if(strchr(tempTok+1, ',')) {
						errCode = VALUE;
						errFlag = true;
					} 
					else {
						sscanf(tempTok+1, "%lf", &(coord->lat));
						if(coord->lat > 90.0) {
							errCode = VALUE;
							errFlag = true;
						}
					}
				}
	
				/*If the longitude is South, or '-'*/
				else if (toupper(tempTok[0]) == 'S' || tempTok[0] == '-') {
					
					if(strchr(tempTok+1, ',')) {
						errCode = VALUE;
						errFlag = true;
					} 
					else {
						sscanf(tempTok+1, "%lf", &(coord->lat));
						coord->lat = coord->lat * (-1);
						if(coord->lat < -90.0) {
							errCode = VALUE;
							errFlag = true;
						}
					}
				}
	
				else {
					errCode =  FIELD;
					errFlag = true;
				}
				fCount++;
			}
		}

		else if(fArray[i]== LON) {
			if(coord == NULL) {
				errCode = FIELD;
				errFlag = true;
			}

			tempTok = getWord(&strPtr, lnBuffer);

			if(tempTok==NULL) {
				errCode = FIELD;
				errFlag = true;
			}
			
			else {
				/*If the Longitude is North, or +, or implied positive (no sign)*/
				if(toupper(tempTok[0]) == 'E' || tempTok[0] == '+' || isdigit(tempTok[0])) {
					if(strchr(tempTok+1, ',')) {
						errCode = VALUE;
						errFlag = true;
					} 
					else {
						sscanf(tempTok+1, "%lf", &(coord->lon));
						if(coord->lon > 180.0) {
							errCode = VALUE;
							errFlag = true;
						}
					}
				}
	
				/*If the longitude is South, or '-'*/
				else if (toupper(tempTok[0]) == 'W' || tempTok[0] == '-') {
					if(strchr(tempTok+1, ',')) {
						errCode = VALUE;
						errFlag = true;
					} 
					else {
						sscanf(tempTok+1, "%lf", &(coord->lon));
						coord->lon = coord->lon * (-1);
						if(coord->lon < -180.0) {
							errCode = VALUE;
							errFlag = true;
						}
					}
				}
	
				else {
					errCode = VALUE;
					errFlag = true;
				}
				fCount++;
			}
		}

		else if(fArray[i] == ID) {

			tempTok = getID(&strPtr, lnBuffer, IDLen); 

			if(tempTok==NULL) {
				errCode = FIELD;
				errFlag = true;
			}
			
			else {
				if(wp->ID) free(wp->ID);
				wp->ID = strdup(tempTok);
				fCount++;
			}
			if(tempTok) free(tempTok);
		}

		else if(fArray[i] == ALT){
			tempTok = getWord(&strPtr, lnBuffer);

			if(tempTok==NULL) {
				errCode = FIELD;
				errFlag = true;
			}
			
			else {
				fCount++;
			}
			if(tempTok) free(tempTok);
		}

		else if(fArray[i] == TFLD){

			tempTok = getWord(&strPtr, lnBuffer);

			if(tempTok==NULL) {
				errCode = FIELD;
				errFlag = true;
			}
			
			else {
				if(strlen(tempTok) > 1){
					errCode = VALUE;
					errFlag = true;
				}

				/*Refer to the header file for the definition of each character*/
				if(tempTok[0] == '-' ||
					tempTok[0] == 'I'||				
					tempTok[0] == 'C'||
					tempTok[0] == '&'||+
					tempTok[0] == '+'||
					tempTok[0] == '^'){
					wp->textChoice = tempTok[0];
				}
				
				else {
					errCode = VALUE;
					errFlag = true;
				}
				fCount++;
				if(tempTok) free(tempTok);
			}
		}

		else if(fArray[i] == OFLD){

			tempTok = getWord(&strPtr, lnBuffer);

			if(tempTok==NULL) {
				errCode = FIELD;
				errFlag = true;
			}
			
			else {
	
				if(strlen(tempTok) > 2){
					errCode = VALUE;
					errFlag = true;
				}
	
				if(strcmp(tempTok, "N")==0) {
					wp->textPlace = 0;
				}
				
				else if(strcmp(tempTok, "NE")==0) {
					wp->textPlace = 1;
				}
				
				else if(strcmp(tempTok, "E")==0) {
					wp->textPlace = 2;
				}
				
				else if(strcmp(tempTok, "SE")==0) {
					wp->textPlace = 3;
				}
	
				else if(strcmp(tempTok, "S")==0) {
					wp->textPlace = 4;
				}
	
				else if(strcmp(tempTok, "SW")==0) {
					wp->textPlace = 5;
				}
	
				else if(strcmp(tempTok, "W")==0) {
					wp->textPlace = 6;
				}
	
				else if(strcmp(tempTok, "NW")==0) {
					wp->textPlace = 7;
				}
	
				else {
					errCode = VALUE;
					errFlag = true;
				}
				fCount++;
				if(tempTok) free(tempTok);
			}
		}

		else if(fArray[i] == SYMB){

			tempTok = getID(&strPtr, lnBuffer, SymLen);

			if(tempTok==NULL) {
				errCode = FIELD;
				errFlag = true;
			}
			
			else {
				if(wp->symbol) free(wp->symbol);
				wp->symbol = strdup(tempTok);
				if(tempTok) free(tempTok);
			}
			fCount++;

		}
		else if(fArray[i] == CMMT){
			tempTok = getCmmt(&strPtr, lnBuffer);
			if (tempTok) {
				if(wp->comment) free(wp->comment);
				wp->comment = malloc(sizeof(char)*strlen(tempTok)+1);
				assert(wp->comment);
				strcpy(wp->comment, tempTok);
				fCount++;
// 				if(tempTok) free(tempTok);
			}
			else {
				wp->comment = strdup("");
			}
			
		}

		else {
			errCode = FIELD;
			errFlag = true;
		}

		
		i++; 
	}

	if(cmmtFlag==true && errFlag==false) {
			if(fCount != fieldLen && fCount != fieldLen-1){
			errCode = FIELD;
			errFlag=true;
		}
	}

	else if (errFlag==false) {
		if(fCount != fieldLen){

			errCode = FIELD;
			errFlag=true;
		}

		else if((chkTok = getWord(&strPtr, lnBuffer)) != NULL) {
			errCode = FIELD;
			errFlag=true;
		}
	}
	wp->coord = (*coord);
	if(chkTok) free(chkTok);
	if(tempTok) free(tempTok);
	free(fBuffer);
	free(lnBuffer);
	if(coord) free(coord);

// 	printWayPt(wp);

	return errCode;
}

/*	Function: scanGpRoute
	Pre-Conditions: The line buffer holding the line to be read and the pointer to the current route
	Post-Conditions: A route structure with the comment and the route number filled out
	Errors Returned: VALUE FIELD
	Comment: This function sets the basics for the scanGpLeg function when it reads in the index points
*/
GpError scanGpRoute( const char *buff, GpRoute *rp) {
	char* lnBuffer = NULL;
	char* tempTok = NULL;
	int routeNum=-1;
	int errCode=OK;
	int strPtr = 0;
	 int i=0;
	bool errFlag=false;
	
	lnBuffer = malloc(sizeof(char)* strlen(buff)+1);
		assert(lnBuffer != NULL);
	strcpy(lnBuffer, buff);
 	rp->npoints = 0;
	rp->comment = strdup("");


	strPtr++;

	tempTok = getWord(&strPtr ,lnBuffer);
	
	if(tempTok == NULL) {
		errCode = FIELD;
		errFlag = true;
	}
	
	if(errFlag == false){
		/*Checking the first string to ensure it is numbers*/
		for(i=0; i<strlen(tempTok); i++) {
			if(!isdigit(tempTok[i])) {
				errCode = VALUE;
				errFlag = true;
			}
		}
	}
	
	if(errFlag==false){
		routeNum = atoi(tempTok);
		if(routeNum < 0) {
			errCode = VALUE;
			errFlag = true;
		}
		
		else { 
 			rp->number = routeNum;
		}
	}
	
	free(tempTok);

	if(errFlag==false) {
		tempTok = getCmmt(&strPtr, lnBuffer);

		if(tempTok != NULL) {
			free(rp->comment);
			rp->comment = malloc(sizeof(char)* strlen(tempTok)+1);
			assert(rp->comment);
			strcpy(rp->comment, tempTok);
		}
		
		free(tempTok);
	}
	
	free(lnBuffer);

	return errCode;
}

/*	Function: ScanGpLeg
	Pre-Conditions:	A buffered line and a buffered F-line. The Waypoint array with the count and the route array
	Post-Conditions: An element of the Route array filled out with the subscript of the corresponding ID keyword from the Waypoint array
	Errors Returned: FIELD, UNKWPT
	Comments: This function was really hard. Like WOW. Friggin Hard.
*/
GpError scanGpLeg( const char *buff, const char *fieldDef, const GpWaypt *wp, const int nwp, GpRoute *rp ) {
	int errCode=OK;
	int IDLen=0;
	int fArray[18];
	int fieldLen=0;
	int i=0;
	int strPtr=0;
	char* lnBuffer = NULL;
	char* lnTok = NULL;
	char* fBuffer = NULL;
	char* fTok = NULL;\
	char* buffer;
	bool errFlag=false;
	bool mtchFlag=false;

	lnBuffer = malloc(sizeof(char)* strlen(buff)+1);
	assert(lnBuffer != NULL);
	strcpy(lnBuffer, buff);

	fBuffer = malloc(sizeof(char)* strlen(fieldDef)+1);
	assert(fBuffer != NULL);
	strcpy(fBuffer, fieldDef);

	fTok = strtok(fBuffer+1, " \n\r\0");
	if(strstr(fTok, "ID")!=NULL) {
		fArray[fieldLen] = ID;
		IDLen = strlen(fTok);
	}

	else {
		errCode = FIELD;
		errFlag = true;
	}

	strPtr++;

	lnTok = getID(&strPtr, lnBuffer, IDLen);

	if(lnTok == NULL) {
		errCode = FIELD;
		errFlag = true;
	}

	else {
		while(i<nwp && errFlag!=true && mtchFlag!=true) {
			buffer = strdup(wp[i].ID);
// 	printf("buffer is %s. and lnTok is %s.\n", buffer, lnTok);
			if(strcmp(buffer, lnTok)==0){
				rp->leg[rp->npoints-1]=i;
				mtchFlag = true;
			}
			i++;
	
			if(i==nwp && mtchFlag!=true) {
				errCode = UNKWPT;
				errFlag = true;
			}
	
			free(buffer);
		}
	}
	free(lnTok);
	free(lnBuffer);
	free(fBuffer);

	return errCode;
}

/*	Function: scanGpTrkpt
	Pre-Conditions: buff takes in the current line being read and fieldDef takes in the F line that was previously read. dateFormat references the filep structure which
			tells us what format the date format is in, and tp is the track point which we will store the values from the line buffer	
	Post-Conditions:: The function will return any errors encountered and or OK if there are no errors. It will also write into the tp pointer struct
	Errors returned: FIELD, VALUE (Please refer to gputil.h for error code definitions)
*/					 
GpError scanGpTrkpt( const char *buff, const char *fieldDef, const char *dateFormat, GpTrkpt *tp ) {

	GpCoord* coord = NULL;
	struct tm DTStruct={0};
	char* buffTok = NULL;
	char* lnBuffer = NULL;
	char* fBuffer = NULL;
	char* fTok = NULL;
	char* tempTok = NULL;
	char* tempTime = NULL;
	char* tempDate = NULL;
	char* DTFormat = NULL;
	int i=0;
	int k=0;
	int fieldLen=0;
	int fArray[18];
	int errCode=OK;
	int fCount=0;
	long duration;
	bool sFlag=false;
	bool errFlag=false;
	tp->segFlag=false;

	DTStruct.tm_isdst = -1;

	lnBuffer = malloc(sizeof(char)* strlen(buff)+1);
	assert(lnBuffer != NULL);
	strcpy(lnBuffer, buff);

	fBuffer = malloc(sizeof(char)* strlen(fieldDef)+1);
	assert(fBuffer != NULL);
	strcpy(fBuffer, fieldDef);

	DTFormat = malloc(sizeof(char)* strlen(dateFormat)+1);
	assert(DTFormat != NULL);
	strcpy(DTFormat, dateFormat);

	fTok = strtok(fBuffer+1, " \n\r\0");
	if(fTok == NULL) {
		errCode = FIELD;
		errFlag = true;
	}

	/*Stores the format buffer into an enumerated array list which sets the order of subsequential data*/
	while(fTok!=NULL && errFlag == false) {
		
		makeUpper(fTok);

		/*Coordinate*/
		if(strcmp(fTok, "LATITUDE")==0) {
			if(coord == NULL) {
				coord = malloc(sizeof(GpCoord));
				assert(coord != NULL);
			}

			fArray[fieldLen] = LAT;
		}
		else if(strcmp(fTok, "LONGITUDE")==0) {
			if(coord == NULL) {
				coord = malloc(sizeof(GpCoord));
				assert(coord != NULL);
			}

			fArray[fieldLen] = LON;
		}
		else if(strstr(fTok, "ALT")!=NULL) {
			fArray[fieldLen] = ALT;
		}

		/*Date, Time, and Duration*/
		else if(strcmp(fTok, "DATE")==0) {
			fArray[fieldLen] = DATE;
		}
		else if(strcmp(fTok, "TIME")==0) {
			fArray[fieldLen] = TIME;
		}
		else if((strcmp(fTok, "DURATION")==0) || (strcmp(fTok, "SECONDS")==0)) {
			fArray[fieldLen] = DURA;
		}

		/*SFlag*/
		else if(strcmp(fTok, "S")==0) {
			fArray[fieldLen] = SFLAG;
		}

		/*Distance Travelled and Speed*/
		else if(strcmp(fTok, "KM")==0 || strcmp(fTok, "M")==0 || strcmp(fTok, "MILES")==0 || strcmp(fTok, "FT")==0 || strcmp(fTok, "NM")==0) {
			fArray[fieldLen] = DIST;
		}
		else if(strcmp(fTok, "KM/H")==0 || strcmp(fTok, "M/S")==0 || strcmp(fTok, "MPH")==0 || strcmp(fTok, "FT/S")==0 || strcmp(fTok, "KNOTS")==0) {
			fArray[fieldLen] = SPD;
		}

		else {
			errCode = FIELD;
			errFlag = true;
		}
		fTok = strtok(NULL, " \n\r\0");
		fieldLen++;
	}
	
	if(fieldLen < 8) {
		errCode = FIELD;
		errFlag = true;
	}

	/*Grab the first token, +1 increments the pointer pass the first token which would be 'W'*/
	buffTok = strtok(lnBuffer+1, " ");

	if(buffTok == NULL) {
		errCode = FIELD;
		errFlag = true;
	}	
	
	/*Continue Looping while the incrementing i is less than the number of elements in the f-Line, if the s-flag is not raised and the error flag is not raise*/
	while(i!=fieldLen && sFlag!=true && errFlag!=true) {
		tempTok = malloc(sizeof(char) * strlen(buffTok)+1);
			assert(tempTok);
			strcpy(tempTok, buffTok);

			if(fArray[i] == LAT){
				if (coord == NULL) {
					errCode = FIELD;
					errFlag = true;
				}

				/*If the Latitude is North, or +, or implied positive (no sign)*/
				if(toupper(tempTok[0]) == 'N' || tempTok[0] == '+' || isdigit(tempTok[0])) {
					if(strchr(tempTok+1, ',')) {
						errCode = VALUE;
						errFlag = true;
					} 
					else {
						sscanf(tempTok+1, "%lf", &coord->lat);
						if(coord->lat > 90.0) {
							errCode = VALUE;
							errFlag = true;
						}
						fCount++;
					}
				}

				/*If the Latitude is South, or '-'*/
				else if (toupper(tempTok[0]) == 'S' || tempTok[0] == '-') {
					if(strchr(tempTok+1, ',')) {
						errCode = VALUE;
						errFlag = true;
					} 
					else {
						sscanf(tempTok+1, "%lf", &coord->lat);
						coord->lat = coord->lat * (-1);
			
						if(coord->lat < -90.0) {
							errCode = VALUE;
							errFlag = true;
						}
						fCount++;
					}
				}

				else {
					errCode =  VALUE;
					errFlag = true;
				}
			}

			else if(fArray[i]== LON) {
				if(coord == NULL) {
					errCode = FIELD;
					errFlag = true;
				}

				/*If the longitude is North, or +, or implied positive (no sign)*/
				if(toupper(tempTok[0]) == 'E' || tempTok[0] == '+' || isdigit(tempTok[0])) {
					if(strchr(tempTok+1, ',')) {
						errCode = VALUE;
						errFlag = true;
					} 
					else {
						sscanf(tempTok+1, "%lf", &coord->lon);

						if(coord->lon > 180.0) {
							errCode = VALUE;
							errFlag = true;
						}
						fCount++;
					}
				}

				/*If the longitude is South, or '-'*/
				else if (toupper(tempTok[0]) == 'W' || tempTok[0] == '-') {
					if(strchr(tempTok+1, ',')) {
						errCode = VALUE;
						errFlag = true;
					} 
					else {
						sscanf(tempTok+1, "%lf", &coord->lon);
						coord->lon = coord->lon * (-1);

						if(coord->lon < -180.0) {
							errCode = VALUE;
							errFlag = true;
						}
						fCount++;
					}
				}

				else {
					errCode = VALUE;
					errFlag = true;
				}
			}

			else if(fArray[i] == ALT){
				fCount++;
			}


			else if(fArray[i] == DATE) {
				tempDate = malloc(sizeof(char) * strlen( tempTok)+1);
				assert(tempDate);
				strcpy (tempDate, tempTok);
				fCount++;
			}


			else if(fArray[i] == TIME){
				tempTime = malloc(sizeof(char) * strlen( tempTok)+1);
				assert(tempTime);
				strcpy (tempTime, tempTok);
				fCount++;
			}


			else if(fArray[i] == DURA) {
				if(strchr(tempTok, ':')==NULL) {
					for(k=0; k<strlen(tempTok); k++) {
						if(!isdigit(tempTok[k])) {
							duration = -1;
						}
					}
					if(duration!=-1) {
						duration = atoi(tempTok);
					}
				}

				else {
					duration = durInSec(tempTok);
				}
				if(duration == (-1)){
					errCode = VALUE;
					errFlag = true;
				}
				else{
					tp->duration = duration;
				}
				fCount++;
			}


			else if(fArray[i] ==  SFLAG) {
				char* cmtTok = NULL;
				int cmtLen=0;

				/*Convert the string format 1 to an int and compare*/
				if (atoi(tempTok)==1) {
					cmtTok = strtok(NULL, "\n");
					if(cmtTok == NULL) {
						cmtLen=0;
						tp->comment = strdup("");
						assert(tp->comment);
					}

					else {
						cmtLen = strlen(cmtTok);
						tp->comment = malloc(sizeof(char)*(cmtLen+1));
						assert(tp->comment);
						strcpy(tp->comment, cmtTok);
					}

					tp->segFlag = true;
					sFlag=true;
				}
				
				else if (atoi(tempTok)==0) {
					tp->segFlag=false;
					sFlag=false;
					tp->comment = NULL;
					fCount++;
				}
				
				else {
					errCode = VALUE;
					errFlag = true;
				}
			}

			else if(fArray[i] == DIST){
				for(k=0; k<strlen(tempTok); k++) {
					if(!isdigit(tempTok[k]) && tempTok[k] != '.') {
						errCode = VALUE;
						errFlag = true;
					}
				}
				if(errFlag!=true){
					tp->dist = atof(tempTok);
				}
				fCount++;
			}

			else if(fArray[i] ==  SPD){
				for(k=0; k<strlen(tempTok)-1; k++) {
					if(!isdigit(tempTok[k]) && tempTok[k] != '.' && !isspace(tempTok[k])) {

						errCode = VALUE;
						errFlag = true;
					}
				}
				if(errFlag!=true){
					tp->speed = atof(tempTok);
				}
				fCount++;
			}

			else {
				errCode = FIELD;
				errFlag = true;
			}

		free(tempTok);
		buffTok = strtok(NULL, " \t"); 
		i++;
		if(sFlag!=true && buffTok==NULL){
			if(fCount!=fieldLen) {
				errCode = FIELD;
				errFlag = true;
			}
		}
		if(i==fieldLen) {
			if(buffTok!=NULL){ 
				errCode = FIELD;
				errFlag = true;
			}
		}
	}

	/*Takes the time and date strings and converts them into a dateformat as defined by the dateformat line
	  Concatenates the necessary strings and sets it to the dateTime in the tp struct*/
	if(tempTime != NULL && tempDate != NULL) {
		char* dateTime = NULL;


		dateTime = malloc(sizeof(char) * strlen(tempTime) + strlen(tempDate) + 1);
		assert(dateTime);
		strcpy(dateTime, tempDate);
		strcat(dateTime, " ");
		strcat(dateTime, tempTime);
		strcat(DTFormat, " %H:%M:%S");

		strptime(dateTime, DTFormat, &DTStruct);

		tp->dateTime = mktime(&DTStruct);

		free(dateTime);
	}

	else if (errFlag != true){
		errCode = VALUE;
		errFlag = true;
	}

	tp->coord = (*coord);

// 	printTrkPt(tp);


	free(fBuffer);
	free(lnBuffer);
	free(DTFormat);
	if(tempTime) free(tempTime);
	if(tempDate) free(tempDate);	
	if(coord) free(coord);

	return errCode;
}

/*	Function:getGpTracks
	Pre-Conditions: An Opened file pointer and a malloc'ed array of tracks
	Post-Conditions: The array of tracks are filled in
	Comment: Memory allocated in this function are not automatically freed by the freeGpFile function. The user must manually free the memory or risk memory leaks
*/	
int getGpTracks( const GpFile *filep, GpTrack **tp ) {
	
	int i=0;
	int seqnum=0;
	int trkcnt=0;
// 	int notrk=0;
	int ntrack=0;
	double hourDuration=0;
	bool hasData=false;

	GpCoord* NEcorner;
	GpCoord* SWcorner;

	if(filep==NULL || filep->ntrkpts==0) {
		*tp = NULL;
		return 0;
	}
	
	*tp = malloc(sizeof(GpTrack)*0);
	assert(*tp);

	while(i<filep->ntrkpts) {

		if(filep->trkpt[i].segFlag==1) {

			if (hasData == true) {
				(*tp)[ntrack-1].duration = filep->trkpt[i].duration;
				if(filep->unitTime == 'S'){
					(*tp)[ntrack-1].speed = filep->trkpt[i].dist/filep->trkpt[i].duration;
				}
		
				else {
					hourDuration = (filep->trkpt[i].duration)/3600.0;
					(*tp)[ntrack-1].speed = filep->trkpt[i].dist/hourDuration;;
				}
				(*tp)[ntrack-1].dist = filep->trkpt[i].dist;
				(*tp)[ntrack-1].NEcorner = (*NEcorner);
				(*tp)[ntrack-1].SWcorner = (*SWcorner);
				(*tp)[ntrack-1].meanCoord.lat = ((*tp)[ntrack-1].NEcorner.lat + (*tp)[ntrack-1].SWcorner.lat) /2;
				(*tp)[ntrack-1].meanCoord.lon = ((*tp)[ntrack-1].NEcorner.lon + (*tp)[ntrack-1].SWcorner.lon) /2;
	
				if(NEcorner) free(NEcorner);
				if(SWcorner) free(SWcorner); 

				hasData = false;
			}

			NEcorner = malloc(sizeof(GpCoord));
			assert(NEcorner != NULL);
			SWcorner = malloc(sizeof(GpCoord));
			assert(SWcorner != NULL);

			ntrack++;
			trkcnt++;
			seqnum = i+1;
			assert(*tp);
			*tp = realloc(*tp ,sizeof(GpTrack) * trkcnt);
			assert(tp!=NULL);

			(*tp)[ntrack-1].seqno = seqnum;
			(*tp)[ntrack-1].startTrk = filep->trkpt[i].dateTime;
			NEcorner->lat = filep->trkpt[i].coord.lat;
			NEcorner->lon = filep->trkpt[i].coord.lon;
			SWcorner->lat = filep->trkpt[i].coord.lat;
			SWcorner->lon = filep->trkpt[i].coord.lon;


// 			notrk++;
		}

		else {
// printf("Old NEcorner lat is %lf\n",NEcorner->lat);
			if(filep->trkpt[i].coord.lat > NEcorner->lat) {
				NEcorner->lat = filep->trkpt[i].coord.lat;
			}
			
			if (filep->trkpt[i].coord.lat < SWcorner-> lat) {	
				SWcorner->lat = filep->trkpt[i].coord.lat;
			}
/*printf("Old NEcorner lon is %lf\n",NEcorner->lon);*/		
			if(filep->trkpt[i].coord.lon > NEcorner->lon) {
				NEcorner->lon = filep->trkpt[i].coord.lon;
			}

			if (filep->trkpt[i].coord.lon < SWcorner-> lon) {	
				SWcorner->lon = filep->trkpt[i].coord.lon;
			}
			trkcnt++;
		}

		(*tp)[ntrack-1].duration = filep->trkpt[i].duration;
		(*tp)[ntrack-1].endTrk = filep->trkpt[i].dateTime;
		if(filep->unitTime == 'S'){
			(*tp)[ntrack-1].speed = filep->trkpt[i].dist/filep->trkpt[i].duration;
		}

		else {
			hourDuration = (filep->trkpt[i].duration)/3600.0;
			(*tp)[ntrack-1].speed = filep->trkpt[i].dist/hourDuration;;
		}
		(*tp)[ntrack-1].dist = filep->trkpt[i].dist;
		(*tp)[ntrack-1].NEcorner = (*NEcorner);
		(*tp)[ntrack-1].SWcorner = (*SWcorner);
		(*tp)[ntrack-1].meanCoord.lat = ((*tp)[ntrack-1].NEcorner.lat + (*tp)[ntrack-1].SWcorner.lat) /2;
		(*tp)[ntrack-1].meanCoord.lon = ((*tp)[ntrack-1].NEcorner.lon + (*tp)[ntrack-1].SWcorner.lon) /2;

		i++;

	}
	
	if(NEcorner) free(NEcorner);
	if(SWcorner) free(SWcorner);
	return ntrack;
}

int writeGpFile( FILE *const gpf, const GpFile *filep ) {

	char* IDstr=NULL;
	char* Symstr=NULL;
	char* cmtstr=NULL;
	char* dateForm=NULL;
	char horzStr[4];
	char timeStr [4];
	char durBuffer[128];
	char buffer[128];
	char textPlaceStr[2];
	char unitHorz;
	char unitTime;
	char latSign = 'N';
	char lonSign = 'E';
	long durHour=0;
	long durMin=0;
	long durSec=0;
	int line=0;
	int IDLen=0;
	int SymLen=0;
	int i=0;
	int j=0;
	int trackno=0;
	int numLen=0;
	int npnts=0;
	struct tm *trackTime;

	IDstr = strdup("ID");
	Symstr = strdup(" ");
	cmtstr = strdup("");

	strcpy(durBuffer, "\0");

	/* 1. Line with GPSU*/
	fprintf(gpf, "H SOFTWARE NAME & VERSION\n");	line++;
	fprintf(gpf, "I GPSU\n");	line++;
	fprintf(gpf, "\n");	line++;

	/*Convert date format to dd/mm/yy etc. form*/
	dateForm = revertDateType (filep->dateFormat);

	/*2. S lines with settings*/
	fprintf(gpf, "S DateFormat=%s\n", dateForm);	line++;

	if(filep->timeZone < 0){
		fprintf(gpf, "S Timezone=%03d:00\n", filep->timeZone);	line++;
	}

	else {
		fprintf(gpf, "S Timezone=+%02d:00\n", filep->timeZone);	line++;
	}

	/*Determining the units*/
	switch(filep->unitHorz) {
		case 'K':
			unitHorz = 'K';
			unitTime = 'H';
			strcpy(horzStr, "km");
			strcpy(timeStr, "h");
			break;

		case 'N':
			unitHorz = 'N';
			unitTime = 'H';
			strcpy(horzStr, "nm");
			strcpy(timeStr, "h");
			break;

		case 'S':
			unitHorz = 'S';
			unitTime = 'H';
			strcpy(horzStr, "mi");
			strcpy(timeStr, "h");
			break;

		case 'F':
			unitHorz = 'F';
			unitTime = 'S';
			strcpy(horzStr, "ft");
			strcpy(timeStr, "s");
			break;

		case 'M':
			unitHorz = 'M';
			unitTime = 'S';
			strcpy(horzStr, "m");
			strcpy(timeStr, "s");
			break;

		default:
			return(0);
	}

	fprintf(gpf, "S Units=%c,%c\n", unitHorz, unitTime);	line++;
	fprintf(gpf, "\n");	line++;

	/* 3. H and M ,lines with Datum and WGS 84*/
	fprintf(gpf, "H R DATUM\n");	line++;
	fprintf(gpf, "M E               WGS 84 100  0.0000000E+00  0.0000000E+00 0 0 0\n");	line++;
	fprintf(gpf, "\n");	line++;

	fprintf(gpf, "H COORDINATE SYSTEM\n");	line++;
	fprintf(gpf, "U  LAT LON DEG\n");	line++;
	fprintf(gpf, "\n");	line++;

	/*F and W lines with waypoints*/
	if(filep->nwaypts > 0) {
		for(i=0; i<filep->nwaypts; i++) {
			if(strlen(filep->waypt[i].ID) > IDLen) {
				IDLen = strlen(filep->waypt[i].ID);
			}
		}
		
		if(IDLen!=0){
			while(strlen(IDstr) != IDLen) {
				strcat(IDstr, "-");
			}
		}

		if(strlen(filep->waypt[0].symbol)!=0) {
	
			for(i=0; i<filep->nwaypts; i++) {
				if(strlen(filep->waypt[i].symbol) > SymLen) {
					SymLen = strlen(filep->waypt[i].symbol);
				}
			}
		
			if(SymLen!=0) {
				strcpy(Symstr, " Symbol");
				while(strlen(Symstr) != SymLen+1) {
					strcat(Symstr, "-");
			// 		printf("Symstr is %s\n", Symstr);
				}
			}
		}

		for(i=0; i<filep->nwaypts; i++) {
			if(strlen(filep->waypt[i].comment) > 0) {
				strcpy(cmtstr, " Comment");
			}
		}

		/*Printing out the F header line for waypoints*/
		fprintf(gpf, "F %s Latitude   Longitude   %sT O%s\n", IDstr, Symstr, cmtstr); line++;

		/*Printing out the waypoints*/
		for(i=0; i<filep->nwaypts; i++) {	
			switch(filep->waypt[i].textPlace) {
				case 0:
					strcpy(textPlaceStr, "N");	//0
					break;
				case 1:
					strcpy(textPlaceStr, "NE");	//1
					break;
				case 2:
					strcpy(textPlaceStr, "E");	//2 etc.
					break;
				case 3:
					strcpy(textPlaceStr, "SE");
					break;
				case 4:
					strcpy(textPlaceStr, "S");
					break;
				case 5:
					strcpy(textPlaceStr, "SW");
					break;
				case 6:
					strcpy(textPlaceStr, "W");
					break;
				case 7:
					strcpy(textPlaceStr, "NW");
					break;
			}

			/*Determining the signs of the lat and lon*/
			if(filep->waypt[i].coord.lat < 0) {
				filep->waypt[i].coord.lat = filep->waypt[i].coord.lat * (-1);
				latSign = 'S';
			}
	
			else {
				latSign = 'N';
			}
	
			if(filep->waypt[i].coord.lon < 0) {
				filep->waypt[i].coord.lon = filep->waypt[i].coord.lon * (-1);
				lonSign = 'W';
			}

			else {
				lonSign = 'E';
			}

			fprintf(gpf, "W %s %c%09.6lf %c%010.6lf ", filep->waypt[i].ID, latSign, filep->waypt[i].coord.lat, lonSign, filep->waypt[i].coord.lon);
			fprintf(gpf, "%s %c %s %s\n",  filep->waypt[i].symbol, filep->waypt[i].textChoice, textPlaceStr, filep->waypt[i].comment); line++;
		}

		fprintf(gpf, "\n");	line++;

		/* 6. R, F, and W line with routes*/
		if(filep->nroutes > 0) {
			for(i=0; i < filep->nroutes; i++) {
				fprintf(gpf, "R %d %s\n", filep->route[i]->number, filep->route[i]->comment);	line++;
				fprintf(gpf, "F %s\n", IDstr);	line++;
				for(j=0; j<filep->route[i]->npoints; j++) {
					fprintf(gpf, "W %s\n", filep->waypt[filep->route[i]->leg[j]].ID);	line++;
				}
				fprintf(gpf, "\n");	line++;
				
			}
		}
	}

	/* 7. H line with Track Summary*/
	if(filep->ntrkpts > 0){
		GpTrack **tp = malloc(sizeof(GpTrack*));
		trackno = getGpTracks(filep, tp);
	
		fprintf(gpf, "H TRACK SUMMARY\n");	line++;
		fprintf(gpf, "H Track        Pnts. Date        Time     StopTime Duration    %s       %s/%s\n", horzStr, horzStr, timeStr);	line++;

		for(i=0; i<trackno; i++) {
			npnts=0;
			fprintf(gpf, "H ");
	
			/*calculating Track Field length and padding seqno with spaces*/
			sprintf(buffer, "%d", (*tp)[i].seqno);
			numLen = strlen(buffer);
			for(j=0; j<(13-numLen); j++) {
				strcat(buffer, " ");
			}
			fprintf(gpf, "%s", buffer);
			strcpy(buffer, "");
	
			/*calculating Pnts*/
			if(i==(trackno-1)) {
				npnts = (filep->ntrkpts - (*tp)[i].seqno)+1;
			}
				
			/*Last track element*/
			else {
				npnts = (*tp)[i+1].seqno - (*tp)[i].seqno; 
			}
	
			/*calculating Pnts length and padding spaces to match pnts field length*/
			sprintf(buffer, "%d", npnts);
			numLen = strlen(buffer);
			for(j=0; j<(6-numLen); j++) {
				strcat(buffer, " ");
			}
			fprintf(gpf, "%s", buffer);
			strcpy(buffer, "");
	
			/*Create time structure out of time_t*/
			trackTime = localtime(&(*tp)[i].startTrk);
			strftime (buffer, 128, filep->dateFormat, trackTime);
	
			/*concat spaces to match date field length*/
			for(j=0; j<(12-strlen(buffer)); j++) {
				strcat(buffer, " ");
			}
			fprintf(gpf, "%s", buffer);
			strcpy(buffer, "");
	
			/*Derive the start time from the same time structure*/
			strftime (buffer, 128, "%H:%M:%S", trackTime);
	
			/*concat spaces to match start time field*/
			for(j=0; j<(9-strlen(buffer)); j++) {
				strcat(buffer, " ");
			}
			fprintf(gpf, "%s", buffer);
			strcpy(buffer, "");
	
			/*re-create time structure based on endtime*/
			trackTime = localtime(&(*tp)[i].endTrk);
			strftime (buffer, 128, "%H:%M:%S", trackTime);
	
			/*concat spaces to match end time field*/
			for(j=0; j<(9-strlen(buffer)); j++) {
				strcat(buffer, " ");
			}
			fprintf(gpf, "%s", buffer);
			strcpy(buffer, "");
	
			/*Converting Duration back to hours, minutes and seconds*/
			durMin = (*tp)[i].duration/60;
			durHour = durMin/60;
			if(durMin > 60) {
				durMin = durMin%60;
			}
			durSec =  (*tp)[i].duration - durHour*3600 - durMin*60;
	
			/*Converting hours, minutes and seconds into one string HH:MM:SS*/
			sprintf(buffer, "%02ld:", durHour);
			strcat(durBuffer, buffer);
			sprintf(buffer, "%02ld:", durMin);
			strcat(durBuffer, buffer);
			sprintf(buffer, "%02ld", durSec);
			strcat(durBuffer, buffer);
	
			/*Padding spaces on to the string*/
			for(j=0; j<(15-strlen(durBuffer)); j++) {
				strcat(durBuffer, " ");
			}
			fprintf(gpf, "%s", durBuffer);
	
			/*Clearing buffers*/
			strcpy(buffer, "");
			strcpy(durBuffer, "");
	
			/*calculating Distance Field length and padding Distance with spaces*/
			sprintf(buffer, "%.6lf", (*tp)[i].dist);
			numLen = strlen(buffer);
			for(j=0; j<(9-numLen); j++) {
				strcat(buffer, " ");
			}
			fprintf(gpf, "%s", buffer);
			strcpy(buffer, "");
	
			/*printing the last element, speed*/
			fprintf(gpf, "%.6lf\n", (*tp)[i].speed);	line++;
			strcpy(buffer, "");
		}
		fprintf(gpf, "\n");	line++;

		fprintf(gpf, "F Latitude   Longitude   Date        Time     S Duration   %s          %s/%s\n", horzStr, horzStr, timeStr);	line++;

		/* 8. F and T lines with trackpoints */
		for(i=0; i<filep->ntrkpts; i++) {
			/*Determining the signs of the lat and lon*/
			if(filep->trkpt[i].coord.lat < 0) {
				filep->trkpt[i].coord.lat = filep->trkpt[i].coord.lat * (-1);
				latSign = 'S';
			}
	
			else {
				latSign = 'N';
			}
	
			if(filep->trkpt[i].coord.lon < 0) {
				filep->trkpt[i].coord.lon = filep->trkpt[i].coord.lon * (-1);
				lonSign = 'W';
			}

			else {
				lonSign = 'E';
			}

			fprintf(gpf, "T %c%09.6lf %c%010.6lf ", latSign, filep->trkpt[i].coord.lat, lonSign, filep->trkpt[i].coord.lon);

			/*Convert into tm struct*/
			trackTime = localtime(&(filep->trkpt[i].dateTime));
			strftime (buffer, 128, filep->dateFormat, trackTime);
	
			/*concat spaces to match date field length*/
			for(j=0; j<(12-strlen(buffer)); j++) {
				strcat(buffer, " ");
			}
			fprintf(gpf, "%s", buffer);
			strcpy(buffer, "");
	
			/*Derive the start time from the same time structure*/
			strftime (buffer, 128, "%H:%M:%S", trackTime);
	
			/*concat spaces to match start time field*/
			for(j=0; j<(9-strlen(buffer)); j++) {
				strcat(buffer, " ");
			}
			fprintf(gpf, "%s", buffer);
			strcpy(buffer, "");

			fprintf(gpf, "%d ", filep->trkpt[i].segFlag);

			if(filep->trkpt[i].segFlag == true) {
				fprintf(gpf, "%s", filep->trkpt[i].comment);
			}

			else {
				/*Converting Duration back to hours, minutes and seconds*/
				durMin = filep->trkpt[i].duration/60;
				durHour = durMin/60;
				if(durMin >= 60) {
					durMin = durMin%60;
				}
				durSec =  filep->trkpt[i].duration - durHour*3600 - durMin*60;

				/*Converting hours, minutes and seconds into one string HH:MM:SS*/
				sprintf(buffer, "%02ld:", durHour);
				strcat(durBuffer, buffer);
				sprintf(buffer, "%02ld:", durMin);
				strcat(durBuffer, buffer);
				sprintf(buffer, "%02ld", durSec);
				strcat(durBuffer, buffer);

				/*Padding spaces on to the string*/
				for(j=0; j<(13-strlen(durBuffer)); j++) {
					strcat(durBuffer, " ");
				}
				fprintf(gpf, "%s", durBuffer);

				/*Clearing buffers*/
				strcpy(buffer, "");
				strcpy(durBuffer, "");
		
				/*calculating Distance Field length and padding Distance with spaces*/
				sprintf(buffer, "%.6lf   ", filep->trkpt[i].dist);
				numLen = strlen(buffer);
				for(j=0; j<(9-numLen); j++) {
					strcat(buffer, " ");
				}
				fprintf(gpf, "%s", buffer);
				strcpy(buffer, "");
		
				/*printing the last element, speed*/
				fprintf(gpf, "%.6lf", filep->trkpt[i].speed);
				strcpy(buffer, "");
			}

			fprintf(gpf, "\n");	line++;
		}

	}

	free(cmtstr);
	free(IDstr);
	free(Symstr);
	free(dateForm);
	return(line);
}
/*	Function : initializeFilep
	Pre-Conditions: An opened and ready to be read file pointer
	Post-Conditions: Values inside the file are initialized and ready to be used
	Comment: This function initializes all the points as inidicated by the header file sand pre-set values for them 
		 so that initial comparison will not segmentation fault if the line is empty. Refer to header file for macros.
*/	
void initializeFilep (GpFile* filep) {
	filep->dateFormat = malloc(sizeof(GP_DATEFORMAT) + 1);
	assert(filep->dateFormat);
	strcpy(filep->dateFormat, GP_DATEFORMAT);
	filep->dateFormat[strlen(GP_DATEFORMAT)] = '\0'; 

	filep->timeZone = GP_TIMEZONE;

	filep->unitHorz = GP_UNITHORZ;

	filep->unitTime = GP_UNITTIME;

	filep->nwaypts = 0;
	filep->waypt = malloc(sizeof(GpWaypt)*0);

	filep->nroutes = 0;
	filep->route = malloc(sizeof(GpRoute*)*0);

	filep->ntrkpts = 0;
	filep->trkpt = malloc(sizeof(GpTrkpt)*0);
	
}

/*	Function : getLine
	Pre-Conditions: An opened and ready to be read file pointer
	Post-Conditions: A null-terminated string from a line in the file
	Comment: The function takes a line from the file and reads it into a temporary buffer which, if it overflows will realloc space to be able to take in more lines
*/	
char* getLine(FILE* gpf) {
	
	if(feof(gpf) || gpf == NULL) {
		return NULL;
	}

	char* lnBuffer = NULL;
	char buffer[_BUFFER];

	/*clearing buffer*/
	strcpy(buffer, "");
	lnBuffer = malloc(sizeof(char)*1);
	assert(lnBuffer);
	lnBuffer[0] = '\0';

	while(buffer[strlen(lnBuffer)-1] != '\n' && buffer[strlen(lnBuffer)-1] != '\r' && !feof(gpf)){
		if (fgets(buffer, 1024, gpf)==NULL) {
			free (lnBuffer);
			return NULL;
		}
		lnBuffer = realloc(lnBuffer, strlen(lnBuffer) + strlen(buffer) +1);
		assert(lnBuffer);
		strcat(lnBuffer, buffer);
	}

	/*NULL terminate second last character, which should be '\n'*/
	if(lnBuffer[strlen(buffer)-1] == '\n' || buffer[strlen(lnBuffer)-1] != '\r'){
		lnBuffer[strlen(buffer)] = '\0';
	}

	return lnBuffer;
}

/*	Function: makeUpper
	Pre-Conditions: source string
	Post-Conditions: The converted string is now all uppercase
	Comment: This function takes a string and modifies its value by reference and makes all of the characters into upper case*/
void makeUpper (char* src) {

	char* temp = src;

	while(*temp != '\0') {
		*temp = toupper(*temp);
		temp++;
	}
}

/*	Function: makeLower
	Input: source string
	Output:The converted string is now all lowercase
	This function takes a string and modifies its value by reference and makes all of the characters into lower case*/
void makeLower (char* src) {

	char* temp = src;

	while(*temp != '\0') {
		*temp = tolower(*temp);
		temp++;
	}
}

/*	Function: Trim
	Input: source string, allocated buffer to write into, size of source string
	Output:The original string without any spaces or tabs (whitespaces)
	This function takes a string, reads in its value and modifies the buffer value by reference and returns a string without and spaces or tabs(whitespaces)*/
char* Trim(char* src, int srcsize) {

	int i=0;
	char* temp = src;
	char* buffer = NULL;

	buffer = malloc(sizeof(char) * strlen(src)+1);
	assert(buffer);


	while(*temp != '\0'&& i<srcsize-1) {
		if(*temp != ' ' && *temp != '\t' && *temp != '\n' && *temp != '\r'){
			buffer[i++] = *temp;
		}
		temp++;
	}

	/*Append null byte*/
	buffer[i] = '\0';

	return buffer;
}

/*	Function: strCaseStr
	Input: source string, search key
	Output: Pointer to the first occurence of the search key in the source string
	The function performs the same function as strcasestr in which it finds the first instance of a key-string in a string and returns the pointer*/
char* strCaseStr (char* src, char* tgt){
	
	char haystack[strlen(src)];
	char needle[strlen(tgt)];

	strcpy(haystack, src);
	strcpy(needle, tgt);

	makeUpper(haystack);
	makeUpper(needle);

	return(strstr(haystack, needle));
}

/*	Function: sortDateFormat
	Pre-Conditions: Takes in the unformatted date format
	Post-Conditions: formats the date to be able to be used by time_h. View man pages for variable descriptions
	Comment: The function string tokenizes the line first by the '=' then '/' to seperate the date
*/	
char* sortDateFormat(char* src){
	int i=0;
	char* temp = NULL;
	char* buffer;
	char* format;
	dateFlags* flags;

// 	buffer = malloc(sizeof(char)*12);
// 	assert(buffer!=NULL);
	

	flags = malloc(sizeof(dateFlags));
	assert(flags!=NULL);
	flags->dayFlag=false;
	flags->mthFlag=false;
	flags->yrFlag=false;
/*
	buffer[0] = '\0';*/

	temp = strtok(src, "=");
	temp = strtok(NULL, "=");


	buffer = Trim(temp, strlen(temp)+1);

	/*Take in first set of date values and sort*/
	temp = strtok(buffer, "/");

	format = sortDateType(temp, flags);
	strcpy(buffer, format);
	free(format);

	/*Take in 2nd, and 3rd set of date value and sort*/
	while(temp!=NULL && i<2){
		temp = strtok(NULL, "/");

		if(temp!=NULL){
			if(i==1){
				strncpy(temp, temp, strlen(temp)-1);
			}
			format = sortDateType(temp, flags);
			strcat(buffer, "/");
			strcat(buffer, format);
			free(format);
		}
		i++;
	}
	
	free(flags);
	return buffer;
}

/*	Function: sortDateType
	Pre-Conditions: The date string, structure with flags
	Post-Conditions: The appropriate converted date string and the flag structure that is modified with an activated flag
	Comment: This flag converts a date string to a "%*" format which can be read by strftime() duplications of the date is
		 prevented by activating a flag, and checking the flag on each iteration to ensure only one of each date exists
*/
char* sortDateType (char* src, dateFlags* flags){
	
	char* temp;
	int len=0;

	len = strlen(src);

	temp = malloc(sizeof(char)*len+1);
	assert(temp!=NULL);
// printf("src is %s\n", src);

	strcpy(temp, src);

	switch(temp[0]) {
		case 'd':
			if(len != 2){

				strcpy(temp, "ERR");
			}
			else {
// printf("dayflag is %d\n", flags->dayFlag);
				if(strcmp(temp, "dd")==0 && flags->dayFlag!=true){
					strcpy(temp, "%d");
					flags->dayFlag=true;
				}

				else {
					strcpy(temp, "ERR");
				}
			}
			break;

		case 'm':
			if(len!=2 && len!=3){
				strcpy(temp,"ERR");
			}
			else {
				if(strcmp(temp, "mm")==0 && flags->mthFlag!=true){
					strcpy(temp, "%m");
				}

				else if(strcmp(temp, "mmm")==0 && flags->mthFlag!=true) {
					strcpy(temp, "%b");
				}

				else {
					strcpy(temp, "ERR");
				}
			}
			break;

		case 'y':
			if(len!=2 && len!=4){
				strcpy(temp,"ERR");
			}

			else {
				if(strcmp(temp, "yy")==0 && flags->yrFlag!=true){
					strcpy(temp, "%y");
				}

				else if(strcmp(temp, "yyyy")==0 && flags->yrFlag!=true) {
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

char* revertDateType (char* src) {

	char* original = NULL;
	char* temp=NULL;
	char* buffer=NULL;
	int i=0;

	original = malloc(sizeof(char) * strlen(src)+1);
	assert(original);
	strcpy(original, src);

	buffer = malloc(sizeof(char) * 16);
	assert(buffer);
	strcpy(buffer, "");

	temp = strtok(original, "/");

	while (temp!=NULL && i<3) {

		if(strcmp(temp, "%d") == 0) {
			strcat(buffer, "dd");
		}

		else if(strcmp(temp, "%m") == 0) {
			strcat(buffer, "mm");
		}	

		else if(strcmp(temp, "%b") == 0) {
			strcat(buffer, "mmm");
		}

		else if(strcmp(temp, "%y") == 0) {
			strcat(buffer, "yy");
		}

		else if(strcmp(temp, "%Y") == 0) {
			strcat(buffer, "yyyy");
		}		

		if(i!=2) {
			strcat(buffer, "/");
		}

		temp = strtok(NULL, "/");

		i++;
	}

	free(original);
	return buffer;
}

/* 	Function: getID
	Pre-Condition: current tokenizer pointer, an allocated buffer, the liner buffer, and the ID length
	Post-Condition: Modifies the buffer by reference to have the ID string and moves the tokenizer pointer pass the string
	Comment: Takes FLen characters from the index buffer whose current locatiojn is pointed at by the index
*/
char* getID(int* index, char* lnBuffer, int FLen) {
	char* buffer = NULL;
	int i=0;
	
	while(isspace(lnBuffer[*index])) {
		(*index)++;
	}
	buffer = malloc(sizeof(char) * FLen+1);
	assert(buffer);
	/*Pull characters to the length of ID length and advance line pointer*/
	for (i=0;i<FLen;i++){
		buffer[i] = lnBuffer[*index];
		if(isspace(lnBuffer[*index])) {
			strcat(buffer, " ");
/*printf("appending white space\n");*/
		}
		(*index)++;
	}
	
	if (strlen(buffer) == 0) {
		return NULL;
	}
	
	if (strlen(buffer) < FLen) {

		for(i=0; i<(FLen - (int)(strlen(buffer))+2); i++){
			strcat(buffer, " ");
		}

	}

		buffer[FLen] = '\0';
		return buffer;
}

/*
	Function: getWord
	Pre-Condition: The string to be read and the the index of where the pointer should be
	Post-Condition: A null-terminated string
	Comment: getWord works in conjunction with other gets to increment the index when copying. Otherwise, the function could be duplicated by sscanf
*/
char* getWord(int* index, char* lnBuffer) {

	int i=0;
	int j=0;
	int strCnt=0;
	char* buff = NULL;

	if(lnBuffer == NULL) {
		return NULL;
	}

	while(isspace(lnBuffer[*index])) {
		(*index)++;
	}
	i = (*index);
	while(!isspace(lnBuffer[i]) && lnBuffer[i] != '\n' && lnBuffer[i] != '\r' && lnBuffer[i] != '\0') {
		strCnt++;
		i++;
	} 


	buff = malloc(sizeof(char) * 1);
	assert(buff);

	for (j=0;j<strCnt;j++){
		buff[j] = lnBuffer[*index];
		(*index)++;
	}

	buff[strCnt] = '\0';
	
	if (strCnt == 0) {
		return NULL;
	}
	
	else{
		return buff;
	}
}

/*
	Function: getCmmt
	Pre-Condition: The string to be read and the the index of where the pointer should be
	Post-Condition: The entire last part of the line until the newline character
	Comment: getCmmt works in conjunction with other gets to increment the index when copying. Otherwise, the function could be duplicated by sscanf
*/
char* getCmmt(int* index, char* lnBuffer) {

	int len=0;
	char* buffer = NULL;
	char* ptr = lnBuffer;
	char* word = NULL;
	int i=0;
	
	ptr += *index;

	word = strtok(ptr, "\n\r");
	if(word == NULL) {
		buffer = malloc(sizeof(char));
		assert(buffer);
		strcpy(buffer, "");
	}

	else {
		len = strlen(word);

		buffer = malloc(sizeof(char) * len+1);
		assert(buffer);

		strcpy(buffer, word);
	}

	if(strlen(buffer) == 0) {
		return NULL;
	}

	else {
		if(buffer[0] == ' ') {
			i=0;
			do {
				buffer[i] = buffer[i+1];
				i++;
			} while(buffer[i] != '\n' && buffer[i] != '\r' && i<strlen(word));
		}
		return buffer;
	}
}

/*
	Function: durInSec
	Pre-Condition: The string in HH:MM:SS format to be converted to seconds
	Post-Condition: The HH:MM:SS time in just seconds
	Comment: durInSec performs the arithmetic necessary to produce the seconds derived from HH:MM:SS 
*/
long durInSec(char* duration) {

	if(duration == NULL) {
		return -1;
	}

	int i=0;
	char* ptr=NULL;
	long value=0;
	long sec=0;
	
	for(i=0; i<strlen(duration); i++) {
		if(!isdigit(duration[i]) && duration[i] != ':') {
			return (-1);
		}
	}
	
	sscanf(duration, "%ld", &value);
	sec = value * 3600;


	ptr = strchr(duration, ':')+ 1; 

	sscanf(ptr, "%ld", &value);
	sec += value * 60;

	ptr = strchr(ptr, ':') + 1;

	sscanf(ptr, "%ld", &value);
	sec += value;

	return sec;

}

/* Non-vital Function. Prints out waypoints*/
void printWayPt(GpWaypt *wp){

	printf("wp ID is %s\n", wp->ID);
	printf("wp textChoice is %c\n", wp->textChoice);
	printf("wp textPlace is %d\n", wp->textPlace);
	printf("wp lat value is %lf\n", wp->coord.lat);
	printf("wp lon value is %lf\n", wp->coord.lon);
	printf("wp symbol is %s\n", wp->symbol);
	printf("wp comment is %s\n", wp->comment);

}

/*Non-vital Function. Prints out Trackpoints*/
void printTrkPt(GpTrkpt *tp){

	printf("tp lat value is %lf\n", tp->coord.lat);
	printf("tp lon value is %lf\n", tp->coord.lon);
	printf("tp time value is %ld\n", tp->dateTime);
	printf("tp segFlag is %d\n", tp->segFlag);
	printf("tp duration is %ld\n", tp->duration);
	if(tp->segFlag==true) printf("tp comment is %s\n", tp->comment);
	printf("tp distance is %lf\n", tp->dist);
	printf("tp speed is %f\n", tp->speed);

}

// void printTracks(GpTracks **tp) {
/*
	printf("tp[ntrack-1]->endTrk is %d\n", tp[ntrack-1]->endTrk);
	printf("tp[ntrack-1]->speed is %f\n", tp[ntrack-1]->speed);
	printf("tp[ntrack-1]->duration is %d\n", tp[ntrack-1]->duration);
	printf("tp[ntrack-1]->dist is %lf\n", tp[ntrack-1]->dist);
	printf("tp[ntrack-1]->NEcorner.lat is %lf\n", tp[ntrack-1]->NEcorner.lat);
	printf("tp[ntrack-1]->NEcorner.lon is %lf\n", tp[ntrack-1]->NEcorner.lon);
	printf("tp[ntrack-1]->SWcorner.lat is %lf\n", tp[ntrack-1]->SWcorner.lat);
	printf("tp[ntrack-1]->SWcorner.lon is %lf\n", tp[ntrack-1]->SWcorner.lon);
	printf("tp[ntrack-1]->meanCoord.lat is %lf\n", tp[ntrack-1]->meanCoord.lat);
	printf("tp[ntrack-1]->meanCoord.lon is %lf\n", tp[ntrack-1]->meanCoord.lon);*/
// 
// }
/*	Function: freeGpFile
	Pre-Conditions: An opened file pointer to be read
	Post-Conditions: Freeing most, if not all, malloc'ed memory
*/
void freeGpFile(GpFile * filep) {
	int i=0;	


	if(filep->dateFormat != NULL) {
		free(filep->dateFormat);
	}

	if(filep->waypt != NULL) {
		for(i=0; i<filep->nwaypts; i++){
			if((filep->waypt[i].comment)!=NULL){
				free(filep->waypt[i].comment);
			}

			if((filep->waypt[i].ID)!=NULL) {
				free(filep->waypt[i].ID);
			}
			if((filep->waypt[i].symbol)!=NULL) {
				free(filep->waypt[i].symbol);
			}
		}
		free(filep->waypt);

	}

	if(filep->route != NULL) {
		for(i=0; i<filep->nroutes; i++) {
			if((filep->route[i]->comment)!=NULL) {
				free(filep->route[i]->comment);
			}

			if((filep->route[i])!=NULL) {
				free(filep->route[i]);
			}
		}	
		free(filep->route);
	}

	if(filep->trkpt != NULL) {


		for(i=0; i<filep->ntrkpts; i++){
			if(filep->trkpt[i].segFlag==true) {
				if((filep->trkpt[i].comment)!=NULL) {
					free(filep->trkpt[i].comment);
				}
			}

		}
		free(filep->trkpt);
	}

}
