#include <stdlib.h>
#include <string.h>

#include "exception.h"
#include "mark.h"
#include "util.h"
#include "gputil.h"

#define NUM_POS_TESTS 4
#define NUM_NEG_TESTS 0

void GpFile_compareWaypt(GpFile *a, GpFile *b)
{
    int exception;
    try
    {
	    if ( a->nwaypts != b->nwaypts ) {
         	LOG( "nwaypts %d did not match %d\n"
            	, a->nwaypts, b->nwaypts );
         	FAIL_POS_TEST
     	}
    	for ( int i = 0; i < a->nwaypts; ++i ) {
         	GpWaypt_equalsCheck( &a->waypt[i]
                            	, &b->waypt[i] );
    	}
	} catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }
}

void GpFile_compareTrkpt(GpFile *a, GpFile *b)
{
    int exception;
    try
    {
	    if ( a->ntrkpts != b->ntrkpts ) {
         	LOG( "ntrkpts %d did not match %d\n"
            	, a->ntrkpts, b->ntrkpts );
         	FAIL_POS_TEST
     	}
    	for ( int i = 0; i < a->ntrkpts; ++i ) {
         	GpTrkpt_equalsCheck( &a->trkpt[i]
                            	, &b->trkpt[i] );
    	}
	} catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }
}


void GpFile_compareRoute(GpFile *a, GpFile *b)
{
    int exception;
    try
    {
        if ( a->nroutes != b->nroutes ) {
            LOG( "nroutes %d did not match %d\n"
               , a->nroutes, b->nroutes );
            FAIL_POS_TEST
        }

        for ( int i = 0; i < a->nroutes; ++i ) {
            GpRoute_equalsCheck( a->route[i]
                               , b->route[i] );
        }
	} catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }
}

void GpFile_compareHeader(GpFile *a, GpFile *b)
{
    int exception;
    try
    {
        if ( strcmp( a->dateFormat, b->dateFormat ) != 0 ) {
            LOG( "dateFormat '%s' did not match '%s'\n"
               , a->dateFormat, b->dateFormat );
            FAIL_POS_TEST
        }

        if ( a->timeZone != b->timeZone ) {
            LOG( "timeZone %d did not match %d\n"
               , a->timeZone, b->timeZone );
            FAIL_POS_TEST
        }

        if ( a->unitHorz != b->unitHorz ) {
            LOG( "unitHorz %c did not match %c\n"
               , a->unitHorz, b->unitHorz );
            FAIL_POS_TEST
        }

        if ( a->unitTime != b->unitTime ) {
            LOG( "unitTime %c did not match %c\n"
               , a->unitTime, b->unitTime );
            FAIL_POS_TEST
        }
	} catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }
}

int s2( int suite, int weight, int argc, char *argv[] ) {
    int exception;

    START_SUITE( suite, "writeGpFile", weight, NUM_POS_TESTS, NUM_NEG_TESTS );

    START_POS_TEST( 1, "Write a Gpfile -- Header information valid" );

    GpFile orig_gpf;
    UTIL_LOAD_GOLD_FILE( "a2-suite2-postest1.gps", orig_gpf, FAIL_POS_TEST );

    FILE * ofp = fopen( "a2-suite2-postest1out.gps", "w+" );

    if ( ofp == NULL ) {
        LOG( "open output file failed\n" );
        abort();
    }

    try {
        int ret = writeGpFile( ofp, &orig_gpf );

        //They should not have closed the file
        rewind( ofp );
        fseek( ofp, 0, SEEK_END );

        if ( ret == 0 ) {
            LOG( "writeGpFile failed\n" );
            FAIL_POS_TEST
        }

        GpFile after_gpf;
        UTIL_LOAD_GOLD_FILE( "a2-suite2-postest1out.gps", after_gpf, FAIL_POS_TEST );

        GpFile_compareHeader( &orig_gpf
                            , &after_gpf );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST


    START_POS_TEST( 2, "Write a GpFile -- Waypt information valid" );

    GpFile orig_gpf2;
    UTIL_LOAD_GOLD_FILE( "a2-suite2-postest1.gps", orig_gpf2, FAIL_POS_TEST );

    FILE * ofp2 = fopen( "a2-suite2-postest1out.gps", "w+" );

    if ( ofp2 == NULL ) {
        LOG( "open output file failed\n" );
        abort();
    }

    try {
        int ret2 = writeGpFile( ofp2, &orig_gpf2 );

        //They should not have closed the file
        rewind( ofp2 );
        fseek( ofp2, 0, SEEK_END );

        if ( ret2 == 0 ) {
            LOG( "writeGpFile failed\n" );
            FAIL_POS_TEST
        }

        GpFile after_gpf2;
        UTIL_LOAD_GOLD_FILE( "a2-suite2-postest1out.gps", after_gpf2, FAIL_POS_TEST );

        GpFile_compareWaypt( &orig_gpf2
                           , &after_gpf2 );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( 3, "Write a GpFile -- route information valid" );

    GpFile orig_gpf3;
    UTIL_LOAD_GOLD_FILE( "a2-suite2-postest1.gps", orig_gpf3, FAIL_POS_TEST );

    FILE * ofp3 = fopen( "a2-suite2-postest1out.gps", "w+" );

    if ( ofp3 == NULL ) {
        LOG( "open output file failed\n" );
        abort();
    }

    try {
        int ret3 = writeGpFile( ofp3, &orig_gpf3 );

        //They should not have closed the file
        rewind( ofp3 );
        fseek( ofp3, 0, SEEK_END );

        if ( ret3 == 0 ) {
            LOG( "writeGpFile failed\n" );
            FAIL_POS_TEST
        }

        GpFile after_gpf3;
        UTIL_LOAD_GOLD_FILE( "a2-suite2-postest1out.gps", after_gpf3, FAIL_POS_TEST );

        GpFile_compareRoute( &orig_gpf3
                           , &after_gpf3 );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( 4, "Write a GpFile -- trackpoint information valid" );

    GpFile orig_gpf4;
    UTIL_LOAD_GOLD_FILE( "a2-suite2-postest1.gps", orig_gpf4, FAIL_POS_TEST );

    FILE * ofp4 = fopen( "a2-suite2-postest1out.gps", "w+" );

    if ( ofp4 == NULL ) {
        LOG( "open output file failed\n" );
        abort();
    }

    try {
        int ret4 = writeGpFile( ofp4, &orig_gpf4 );

        //They should not have closed the file
        rewind( ofp4 );
        fseek( ofp4, 0, SEEK_END );

        if ( ret4 == 0 ) {
            LOG( "writeGpFile failed\n" );
            FAIL_POS_TEST
        }

        GpFile after_gpf4;
        UTIL_LOAD_GOLD_FILE( "a2-suite2-postest1out.gps", after_gpf4, FAIL_POS_TEST );

        GpFile_compareTrkpt( &orig_gpf4
                           , &after_gpf4 );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    END_SUITE
}
