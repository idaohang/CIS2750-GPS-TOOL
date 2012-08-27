#include <stdlib.h>

#include "exception.h"
#include "gpstool.h"
#include "mark.h"
#include "util.h"

#define NUM_POS_TESTS 9
#define NUM_NEG_TESTS 0

#define AUTOMARKER_checkGpFileMerge( testPrefix ) { \
    GpFile gpfa; \
    UTIL_LOAD_GOLD_FILE( testPrefix"a.gps", gpfa, FAIL_POS_TEST ); \
 \
    GpFile gpfexpect; \
    UTIL_LOAD_GOLD_FILE( testPrefix"expect.gps", gpfexpect, FAIL_POS_TEST ); \
 \
    try { \
        int ret = gpsMerge( &gpfa, testPrefix"b.gps" ); \
 \
        if ( ret != EXIT_SUCCESS ) { \
            LOG( "%d did not match expected %d for return\n" \
               , ret, EXIT_SUCCESS ); \
            FAIL_POS_TEST \
        } \
 \
        GpFile_compare( &gpfa \
                      , &gpfexpect ); \
    } catch ( exception ) { \
        LOG( "%s\n", Exception_Names[exception] ); \
        FAIL_POS_TEST \
    } \
}

int s6( int suite, int weight, int argc, char *argv[] ) {
    int exception;
    START_SUITE( suite, "gpsMerge", weight, NUM_POS_TESTS, NUM_NEG_TESTS );

    int posCtr = 0;
    //WAYPOINTS
    START_POS_TEST( ++posCtr, "Test a simple file with matching settings to check that fileA's waypoints come before fileB's" );

    AUTOMARKER_checkGpFileMerge( "a2-suite6-postest1" );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with different max lengths for id's and symbols, check that output is max of the two." );

    AUTOMARKER_checkGpFileMerge( "a2-suite6-postest2" );

    PASS_POS_TEST

    //ROUTES
    START_POS_TEST( ++posCtr, "Test that routes are output in correct order (fileA's before fileB's)" );

    AUTOMARKER_checkGpFileMerge( "a2-suite6-postest3" );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with all fileA's routes inside 0-99 so that routeb should start from 100" );

    AUTOMARKER_checkGpFileMerge( "a2-suite6-postest4" );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with all fileA's routes inside 300-399 so that routeb should start from 400" );

    AUTOMARKER_checkGpFileMerge( "a2-suite6-postest5" );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with some fileA's routes inside 100-299 and some inside 500-599 and ensure that fileB starts at 600" );

    AUTOMARKER_checkGpFileMerge( "a2-suite6-postest6" );

    PASS_POS_TEST

    //TRACKPOINTS

    START_POS_TEST( ++posCtr, "Test output of trackpoints is in correct order (fileA's followed by fileB's)" );

    AUTOMARKER_checkGpFileMerge( "a2-suite6-postest7" );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with a different timezone so that fileB's dates/times are adjusted to fileA's" );

    AUTOMARKER_checkGpFileMerge( "a2-suite6-postest8" );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with a different units so that fileB's horizontal units are adjusted to fileA's" );

    AUTOMARKER_checkGpFileMerge( "a2-suite6-postest9" );

    PASS_POS_TEST

    END_SUITE
}
