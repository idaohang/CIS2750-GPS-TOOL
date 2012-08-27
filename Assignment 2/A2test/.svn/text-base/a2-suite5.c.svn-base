#include <stdlib.h>

#include "exception.h"
#include "gpstool.h"
#include "mark.h"
#include "util.h"

#define NUM_POS_TESTS 4
#define NUM_NEG_TESTS 0

int qsortCalled = 1;

/* This function gets called because makefile.mark compiles student's
   gpstool.c with -Dqsort=catch_qsort. If they do the sort another way,
   control will not come here. This entry simply forwards the call to the
   real qsort.
*/
void catch_qsort( void * base
          , size_t nmemb
          , size_t size
          , int(*compar)(const void *, const void *) ) {
    qsortCalled = 1;
    qsort( base, nmemb, size, compar );
}


int s5( int suite, int weight, int argc, char *argv[] ) {
    int exception;
    START_SUITE( suite, "gpsSort", weight, NUM_POS_TESTS, NUM_NEG_TESTS );

    int posCtr = 0;
    START_POS_TEST( ++posCtr, "Test on an already sorted list" );

    //Load from ordered file
    GpFile gpf1a;
    UTIL_LOAD_GOLD_FILE( "a2-suite5-postest1.gps", gpf1a, FAIL_POS_TEST );

    //Load from ordered file again, to check against a
    GpFile gpf1b;
    UTIL_LOAD_GOLD_FILE( "a2-suite5-postest1.gps", gpf1b, FAIL_POS_TEST );

    //Pass into gpsSort
    try {
        int ret1 = gpsSort( &gpf1a );

        if ( ret1 != EXIT_SUCCESS ) {
            LOG( "%d did not match expected %d for return\n"
               , ret1, EXIT_SUCCESS );
            FAIL_POS_TEST
        }

        GpFile_compare( &gpf1a
                      , &gpf1b );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test on an unsorted list" );

    //Load from unordered file
    GpFile gpf2a;
    UTIL_LOAD_GOLD_FILE( "a2-suite5-postest2unordered.gps", gpf2a, FAIL_POS_TEST );

    //Load from ordered file, to check against a.  This ensures that they didn't change any other
    //part of GpFile compared to just checking ordering manually
    GpFile gpf2b;
    UTIL_LOAD_GOLD_FILE( "a2-suite5-postest2ordered.gps", gpf2b, FAIL_POS_TEST );

    //Pass into gpsSort
    try {
        qsortCalled = 0;
        int ret2 = gpsSort( &gpf2a );
        if ( qsortCalled == 0 ) {
            LOG( "qsort was not used for sorting\n" );
            FAIL_POS_TEST
        }

        if ( ret2 != EXIT_SUCCESS ) {
            LOG( "%d did not match expected %d for return\n"
               , ret2, EXIT_SUCCESS );
            FAIL_POS_TEST
        }

        GpFile_compare( &gpf2a
                      , &gpf2b );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with an unsorted list that has duplicate waypoints to verify that points are properly realigned" );

    //Load from a file that has duplicate waypoints, and two routes referencing that waypoint
    GpFile gpf3a;
    UTIL_LOAD_GOLD_FILE( "a2-suite5-postest3unordered.gps", gpf3a, FAIL_POS_TEST );

    //Alter the gpFile structure so that each route points to a different occurrence of the waypoint
    int folly1Idx = gpf3a.route[0]->leg[0];
    int folly2Idx = gpf3a.nwaypts - 1;

    gpf3a.route[1]->leg[2] = folly2Idx;

    //Record the addr that each one refers too
    char * folly1Addr = gpf3a.waypt[folly1Idx].ID;
    char * folly2Addr = gpf3a.waypt[folly2Idx].ID;

    try {
        //Sort it
        qsortCalled = 0;
        int ret3 = gpsSort( &gpf3a );
        if ( qsortCalled == 0 ) {
            LOG( "qsort was not used for sorting\n" );
            FAIL_POS_TEST
        }

        if ( ret3 != EXIT_SUCCESS ) {
            LOG( "%d did not match expected %d for return\n"
               , ret3, EXIT_SUCCESS );
            FAIL_POS_TEST
        }

        //Check that each route has been updated and still points to the correct addr

        char * folly1AddrAfter = gpf3a.waypt[gpf3a.route[0]->leg[0]].ID;
        char * folly2AddrAfter = gpf3a.waypt[gpf3a.route[1]->leg[2]].ID;

        if ( folly1AddrAfter != folly1Addr ) {
            LOG( "%p (%s) did not match expected %p (%s) after sort\n"
               , folly1AddrAfter, folly1AddrAfter, folly1Addr, folly1Addr );
        }

        if ( folly2AddrAfter != folly2Addr ) {
            LOG( "%p (%s) did not match expected %p (%s) after sort\n"
               , folly2AddrAfter, folly2AddrAfter, folly2Addr, folly2Addr );
        }

        //Load from expect file (that has waypoints sorted)

        GpFile gpf3b;
        UTIL_LOAD_GOLD_FILE( "a2-suite5-postest3ordered.gps", gpf3b, FAIL_POS_TEST );

        //They could have ordered their point any way depending on qsort, so just copy their
        //indices.  They have to be correct here if they passed, as I check that their indices
        //match the addresses

        //Alter expect so that route numbers align properly

        gpf3b.route[0]->leg[0] = gpf3a.route[0]->leg[0];
        gpf3b.route[1]->leg[2] = gpf3a.route[1]->leg[2];

        //Check whole file to ensure that the sort actually worked

        GpFile_compare( &gpf3a
                      , &gpf3b );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST
 
    START_POS_TEST( ++posCtr, "Test on a file with no waypoints");
    GpFile gpf4a;
    UTIL_LOAD_GOLD_FILE( "a2-suite5-postest4.gps", gpf4a, FAIL_POS_TEST);
    
    GpFile gpf4b;
    UTIL_LOAD_GOLD_FILE( "a2-suite5-postest4.gps", gpf4b, FAIL_POS_TEST);
    
    try{
	int ret4 = gpsSort( &gpf4a );
	if (ret4 != EXIT_SUCCESS){
		LOG( "%d did not match expected %d for return\n", ret4, EXIT_SUCCESS);
		FAIL_POS_TEST
	}
	
	GpFile_compare(&gpf1a, &gpf1b);
    } catch ( exception ) {
	LOG( "%s\n", Exception_Names[exception]);
	FAIL_POS_TEST
    }
    PASS_POS_TEST
    END_SUITE
}
