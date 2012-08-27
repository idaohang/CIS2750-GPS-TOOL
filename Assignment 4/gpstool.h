/********
gpstool.h -- header file for gpstool.c in Asmt 2
Last updated:  January 28, 2010 03:42:46 PM       
Edited by: Kwok Lau 0595198
Email: kwok@uoguelph.ca
Description: This is the header file required by gpstool.c to be able to run. DO NOT EDIT. 
	     Please see gputil.h for more header files needed by gpstool,c and gputil.c.
********/
#ifndef GPSTOOL_H_
#define GPSTOOL_H_ A2

#include "gputil.h"

int gpsInfo( FILE *const outfile, const GpFile *filep );
int gpsDiscard( GpFile *filep, const char *which );
int gpsSort( GpFile *filep );
int gpsMerge( GpFile *filep, const char *const fnameB );

#endif
