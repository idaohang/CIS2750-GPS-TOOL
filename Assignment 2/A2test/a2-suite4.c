#include <stdlib.h>
#include <string.h>

#include "exception.h"
#include "gpstool.h"
#include "mark.h"
#include "util.h"

#define NUM_POS_TESTS 6
#define NUM_NEG_TESTS 1

static void GpFile_discardCheckNumRemaining( GpFile * gpf
                                           , int nroutes
                                           , int ntrkpts
                                           , int nwaypts
                                           , const char * which
					   , int testType ) {
    int exception;
    try {
        int ret = gpsDiscard( gpf, which );

        if ( ret != EXIT_SUCCESS ) {
            LOG( "%d did not match expected %d for return\n"
               , ret, EXIT_SUCCESS );
	    if(testType == 1)
               FAIL_POS_TEST
            else
               FAIL_NEG_TEST
        }

        if ( gpf->nroutes != nroutes ) {
            LOG( "%d did not match expected %d for nroutes\n"
               , gpf->nroutes, nroutes );
            if(testType == 1)
               FAIL_POS_TEST
            else
               FAIL_NEG_TEST
        }

        if ( gpf->ntrkpts != ntrkpts ) {
            LOG( "%d did not match expected %d for ntrkpts\n"
               , gpf->ntrkpts, ntrkpts );
            if(testType == 1)
               FAIL_POS_TEST
            else 
               FAIL_NEG_TEST
        }

        if ( gpf->nwaypts != nwaypts ) {
            LOG( "%d did not match expected %d for nwaypts\n"
               , gpf->nwaypts, nwaypts );
	    if(testType == 1)
               FAIL_POS_TEST
            else
               FAIL_NEG_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        if(testType == 1)
           FAIL_POS_TEST
        else
           FAIL_NEG_TEST
    }
}

int s4( int suite, int weight, int argc, char *argv[] ) {
    START_SUITE( suite, "gpsDiscard", weight, NUM_POS_TESTS, NUM_NEG_TESTS );

    int posCtr = 0;
    START_POS_TEST( ++posCtr, "Test discarding just waypoints (routes discarded by association)" );

    //Load GpFile with gold program
    GpFile gpf1;
    UTIL_LOAD_GOLD_FILE( "a2-suite4-postest1.gps", gpf1, FAIL_POS_TEST );

    GpFile_discardCheckNumRemaining( &gpf1
                                   , 0
                                   , gpf1.ntrkpts //copied before func run
                                   , 0
                                   , "w"
				   , 1 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test discarding just trackpoints" );

    //Load gpfile with gold program
    GpFile gpf2;
    UTIL_LOAD_GOLD_FILE( "a2-suite4-postest2.gps", gpf2, FAIL_POS_TEST );

    GpFile_discardCheckNumRemaining( &gpf2
                                   , gpf2.nroutes
                                   , 0
                                   , gpf2.nwaypts
                                   , "t"
				   , 1 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test discarding just routes" );

    //Load gpfile with gold program
    GpFile gpf3;
    UTIL_LOAD_GOLD_FILE( "a2-suite4-postest3.gps", gpf3, FAIL_POS_TEST );

    GpFile_discardCheckNumRemaining( &gpf3
                                   , 0
                                   , gpf3.ntrkpts
                                   , gpf3.nwaypts
                                   , "r"
				   , 1 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test discarding BOTH routes and trackpoints" );

    //Load gpfile with gold program
    GpFile gpf4;
    UTIL_LOAD_GOLD_FILE( "a2-suite4-postest4.gps", gpf4, FAIL_POS_TEST );

    GpFile_discardCheckNumRemaining( &gpf4
                                   , 0
                                   , 0
                                   , gpf4.nwaypts
                                   , "rt"
				   , 1 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with different orderings of above" );

    //Load gpfile with gold program
    GpFile gpf5;
    UTIL_LOAD_GOLD_FILE( "a2-suite4-postest5.gps", gpf5, FAIL_POS_TEST );

    GpFile_discardCheckNumRemaining( &gpf5
                                   , 0
                                   , 0
                                   , gpf5.nwaypts
                                   , "tr"
				   , 1 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test discarding all (rtw)" );

    //Load gpfile with gold program
    GpFile gpf6;
    UTIL_LOAD_GOLD_FILE( "a2-suite4-postest6.gps", gpf6, FAIL_POS_TEST );
    GpFile_discardCheckNumRemaining( &gpf6
				   , gpf6.nroutes
				   , gpf6.ntrkpts
				   , gpf6.nwaypts
				   , "rtw"
				   , 1 );
    PASS_POS_TEST

    int negCtr = 0;

    START_NEG_TEST( ++negCtr, "Test for error message on discard all (tw)" );
    
    GpFile gpf7;
    UTIL_LOAD_GOLD_FILE( "a2-suite4-negtest1.gps", gpf7, FAIL_NEG_TEST );
    fpos_t prePos, postPos;
    fgetpos(stderr, &prePos);
    GpFile_discardCheckNumRemaining( &gpf7
                                   , gpf7.nroutes
                                   , gpf7.ntrkpts
                                   , gpf7.nwaypts
                                   , "tw"
				   , 0 );

    fgetpos(stderr, &postPos);
    if(postPos.__pos==prePos.__pos)
    {
      LOG("No error message output to stderr!\n");  
      FAIL_NEG_TEST
    }

    PASS_NEG_TEST

    END_SUITE
}
