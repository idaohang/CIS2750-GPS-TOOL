#include <stdlib.h>
#include <string.h>

#include "exception.h"
#include "gpstool.h"
#include "mark.h"
#include "util.h"

#define NUM_POS_TESTS 4
#define NUM_NEG_TESTS 0

#define AUTOMARK_InfoCheck( testPrefix, sorted ) { \
    GpFile gpf; \
    UTIL_LOAD_GOLD_FILE( testPrefix".gps", gpf, FAIL_POS_TEST ); \
 \
    FILE * fp = fopen( testPrefix"-sinfoout", "w" ); \
 \
    if ( fp == NULL ) { \
        LOG( "Failed to open "testPrefix"-sinfoout for writing\n" ); \
        abort(); \
    } \
 \
    try { \
        int ret = gpsInfo( fp, &gpf ); \
 \
        if ( ret != EXIT_SUCCESS ) { \
            LOG( "Return from gpsInfo was not EXIT_SUCCESS\n" ); \
            FAIL_POS_TEST \
        } \
 \
        fclose( fp ); \
    } catch ( exception ) { \
        LOG( "%s\n", Exception_Names[exception] ); \
        FAIL_POS_TEST \
    } \
 \
    SYSTEM_CHECK( "./goldgpstool -info < "testPrefix".gps > "testPrefix"-ginfoout", EXIT_SUCCESS, FAIL_POS_TEST ); \
 \
    util_InfoOutCheck( testPrefix"-ginfoout" \
                     , testPrefix"-sinfoout" \
                     , sorted ); \
}

int s3( int suite, int weight, int argc, char *argv[] ) {
    int exception;

    START_SUITE( suite, "gpsInfo", weight, NUM_POS_TESTS, NUM_NEG_TESTS );

    int posCtr = 0;
    START_POS_TEST( ++posCtr, "Test on a GpFile with waypts, routes and trkpts that is sorted" );

    AUTOMARK_InfoCheck( "a2-suite3-postest1", 1 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test on a GpFile with waypts, routes and trkpts that is not sorted" );

    AUTOMARK_InfoCheck( "a2-suite3-postest2", 0 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with extent that goes across NS EW boundaries" );

    AUTOMARK_InfoCheck( "a2-suite3-postest3", 1 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test for message suppression with no waypoints" );
   
    AUTOMARK_InfoCheck( "a2-suite3-postest4", -1 );
 
    PASS_POS_TEST

    END_SUITE
}
