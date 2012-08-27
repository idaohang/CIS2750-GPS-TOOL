// util.h for A2 automarker

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "mark.h"
#include "gputil.h"	// student's writeGpFile

GpStatus gold_readGpFile( FILE *const gpf, GpFile *filep );

#define TOLERANCE 0.0001

#define UTIL_LOAD_GOLD_FILE( fname, gpf, failure ) { \
    FILE * ulgf_fp = fopen( fname, "r" ); \
 \
    if ( ulgf_fp == NULL ) { \
        LOG( "Failed to open '%s'\n", fname ); \
        failure \
    } else { \
        memset( &gpf, 0, sizeof(GpFile) ); \
        GpStatus ulgf_s = gold_readGpFile( ulgf_fp, &gpf ); \
        if ( ulgf_s.code != OK ) { \
            LOG( "Gold program failed to load '%s'. '%s'\n" \
               , fname, GpError_toString( ulgf_s.code ) ); \
            failure \
        } \
        fclose( ulgf_fp ); \
    } \
}

#define SYSTEM_CHECK( cmd, retcode, FAILURE ) { \
    int sc_status = system( cmd ); \
 \
    if ( sc_status == -1 ) { \
        LOG( "system failed\n" ); \
        abort(); \
    } \
 \
    if ( !WIFEXITED( sc_status ) ) { \
        LOG( "'%s' did not terminate normally\n", cmd ); \
        FAILURE \
    } \
 \
    if ( WEXITSTATUS( sc_status ) != retcode ) { \
        LOG( "gpstool return %d did not match expected %d\n" \
           , WEXITSTATUS( sc_status ), retcode ); \
        FAILURE \
    } \
}

void GpFile_compare( GpFile * a
                   , GpFile * b );
void GpFile_check( GpFile * gpf
                 , char * dateFormat
                 , int timeZone
                 , char unitHorz
                 , char unitTime
                 , int nwaypts
                 , GpWaypt * waypt
                 , int nroutes
                 , GpRoute ** route
                 , int ntrkpts
                 , GpTrkpt * trkpt );

void util_InfoOutCheck( const char * const gfname
                      , const char * const sfname
                      , int sorted );


// A1's util.c

void * checkMalloc( size_t s );
FILE * fopen_check( char * fname );
const char * GpError_toString( GpError e );
int GpCoord_equals( const GpCoord * const a
                  , const GpCoord * const b );
void GpRoute_equalsCheck( const GpRoute * const a
                        , const GpRoute * const b );
void GpTrkpt_check( const GpTrkpt * const t
                  , const GpCoord * const coord
                  , time_t dateTime
                  , _Bool segFlag
                  , char * comment
                  , float speed
                  , double dist
                  , long duration );
void GpTrkpt_equalsCheck( const GpTrkpt * const a
                        , const GpTrkpt * const b );
void GpTrkpt_init( GpTrkpt * t
                 , double lat
                 , double lon
                 , time_t dateTime
                 , _Bool segFlag
                 , char * comment
                 , float speed
                 , double dist
                 , long duration );
void GpWaypt_check( const GpWaypt * wp
                  , char * id
                  , const GpCoord * coord
                  , char * symbol
                  , char textChoice
                  , short textPlace
                  , char * comment );
void GpWaypt_equalsCheck( const GpWaypt * const a
                        , const GpWaypt * const b );
void GpWaypt_init( GpWaypt * wp
                 , char * id
                 , double lat
                 , double lon
                 , char * symbol
                 , char textChoice
                 , char textPlace
                 , char * comment );
