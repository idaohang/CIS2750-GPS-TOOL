#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "exception.h"
#include "mark.h"
#include "negutil.h"
#include "util.h"

#define NUM_POS_TESTS 4
#define NUM_NEG_TESTS 4

int s1( int suite, int weight, int argc, char *argv[] ) {
    int posCtr = 0;

    START_SUITE( suite, "Command line tests", weight, NUM_POS_TESTS, NUM_NEG_TESTS );

    START_POS_TEST( ++posCtr, "test to check if it doesn't crash" );

    SYSTEM_CHECK( "./gpstool -info < a2-suite1-postest1.gps", EXIT_SUCCESS, FAIL_POS_TEST );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test for valgrind errors" );

    SYSTEM_CHECK( "valgrind --leak-check=yes --log-file=a2-suite1-postest2-logfile ./gpstool -info < a2-suite1-postest2.gps", EXIT_SUCCESS, FAIL_POS_TEST );

    int status = system( "grep -c 'ERROR SUMMARY: 0 errors from 0 contexts' a2-suite1-postest2-logfile > a2-suite1-postest2-grepcheck" );

    if ( status == -1 ) {
        LOG( "grep failed\n" );
        abort();
    }

    FILE * fp2 = fopen( "a2-suite1-postest2-grepcheck", "r" );

    int numMatches = 0;
    if ( fscanf( fp2, "%d", &numMatches ) != 1 ) {
        LOG( "grep did not return a count\n" );
        abort();
    }

    fclose( fp2 );

    if ( numMatches != 1 ) {
        LOG( "Valgrind reported errors\n" );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test for valgrind memory leaks" );

    SYSTEM_CHECK( "valgrind --leak-check=yes --log-file=a2-suite1-postest3-logfile ./gpstool -info < a2-suite1-postest3.gps", EXIT_SUCCESS, FAIL_POS_TEST );

    status = system( "grep -c 'malloc/free: in use at exit: 0 bytes in 0 blocks.' a2-suite1-postest3-logfile > a2-suite1-postest3-grepcheck" );

    if ( status == -1 ) {
        LOG( "grep failed\n" );
        abort();
    }

    FILE * fp3 = fopen( "a2-suite1-postest3-grepcheck", "r" );

    numMatches = 0;
    if ( fscanf( fp3, "%d", &numMatches ) != 1 ) {
        LOG( "grep did not return a count\n" );
        abort();
    }

    fclose( fp3 );

    if ( numMatches != 1 ) {
        LOG( "Valgrind reported memory leaks\n" );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with -info to see if main can call a subfunction and pass to stdout" );

    SYSTEM_CHECK( "./gpstool -info < a2-suite1-postest4.gps > a2-suite1-postest4-sinfoout", EXIT_SUCCESS, FAIL_POS_TEST );
    SYSTEM_CHECK( "./goldgpstool -info < a2-suite1-postest4.gps > a2-suite1-postest4-ginfoout", EXIT_SUCCESS, FAIL_POS_TEST );

    util_InfoOutCheck( "a2-suite1-postest4-ginfoout"
                     , "a2-suite1-postest4-sinfoout"
                     , 0 );

    PASS_POS_TEST

    int negCtr = 0;

    START_NEG_TEST( ++negCtr, "excess command line arguments gpstool -discard w -info" );

    SYSTEM_CHECK( "./gpstool -discard w -info < a2-suite1-negtest1.gps", EXIT_FAILURE, FAIL_NEG_TEST );

    PASS_NEG_TEST

    START_NEG_TEST( ++negCtr, "invalid command line arguments gpstool -garbage" );

    SYSTEM_CHECK( "./gpstool -garbage < a2-suite1-negtest2.gps", EXIT_FAILURE, FAIL_NEG_TEST );

    PASS_NEG_TEST

    START_NEG_TEST( ++negCtr, "Bad gputil file that readGpFile should error on" );

    SYSTEM_CHECK( "./gpstool -info < a2-suite1-negtest3.gps", EXIT_FAILURE, FAIL_NEG_TEST );

    PASS_NEG_TEST

    START_NEG_TEST( ++negCtr, "Test merge without fileB in existence so that output is just fileA" );

    //gold_readGpFile the input
    GpFile igpf;
    UTIL_LOAD_GOLD_FILE( "a2-suite1-negtest4.gps", igpf, FAIL_NEG_TEST );

    //Run the program

    SYSTEM_CHECK( "./gpstool -merge NONEXISTENTFILE < a2-suite1-negtest4.gps > a2-suite1-negtest4output.gps", EXIT_SUCCESS, FAIL_NEG_TEST );

    /*********CHANGE THIS TO a2-suite1-negtest4output.gps**********
     *********once point matching is worked out          **********/
    GpFile ogpf;
    UTIL_LOAD_GOLD_FILE( "a2-suite1-negtest4.gps", ogpf, FAIL_NEG_TEST );

    //Compare GpFile's
    NEG_GpFile_compare( &igpf
                      , &ogpf );

    PASS_NEG_TEST

    END_SUITE
}
