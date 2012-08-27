#include <string.h>
#include "exception.h"
#include "mark.h"
#include "gputil.h"
#include "util.h"

#define NUM_POS_TESTS 2
#define NUM_NEG_TESTS 3

#define NEG_ERROR_TEST( desc, buff, error ) \
    START_NEG_TEST( ++negCtr, desc ) \
    memset( &r, 0, sizeof( GpRoute ) ); \
 \
    try { \
        e = scanGpRoute( buff \
                       , &r ); \
    } catch ( exception ) { \
        LOG( "%s\n", Exception_Names[exception] ); \
        FAIL_NEG_TEST \
    } \
 \
    if ( e != error ) { \
        LOG( "Invalid error return %s. Expected %s\n" \
           , GpError_toString( e ) \
           , GpError_toString( error ) ); \
        FAIL_NEG_TEST \
    } \
 \
    PASS_NEG_TEST

int s2( int suite, int weight, int argc, char *argv[] ) {
    GpError e;
    int exception;
    int posCtr = 0;
    GpRoute r;

    START_SUITE( suite, "scanGpRoute", weight, NUM_POS_TESTS, NUM_NEG_TESTS );

    START_POS_TEST( ++posCtr, "Test with route no. & valid comment" );
    memset( &r, 0, sizeof( GpRoute ) );

    try {
        e = scanGpRoute( "R 02 CALSHT - NEEDLE"
                       , &r );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( e != OK ) {
        LOG( "Invalid error return %s. Expected %s\n"
           , GpError_toString( e )
           , GpError_toString( OK ) );
        FAIL_POS_TEST
    }

    if ( r.number != 2 ) {
        LOG( "Route number '%d', did not expected '2'\n"
           , r.number );
        FAIL_POS_TEST
    }

    try {
        if ( strcmp( "CALSHT - NEEDLE", r.comment ) != 0 ) {
            LOG( "Comment '%s', did not match 'CALSHT - NEEDLE'\n"
               , r.comment );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test for default comment ("")" );
    memset( &r, 0, sizeof( GpRoute ) );

    try {
        e = scanGpRoute( "R 07"
                       , &r );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( e != OK ) {
        LOG( "Invalid error return %s. Expected %s\n"
           , GpError_toString( e )
           , GpError_toString( OK ) );
        FAIL_POS_TEST
    }

    if ( r.number != 7 ) {
        LOG( "Route number '%d', did not expected '7'\n"
           , r.number );
        FAIL_POS_TEST
    }

    try {
        if ( strcmp( "", r.comment ) != 0 ) {
            LOG( "Comment '%s', did not match ''\n"
               , r.comment );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    int negCtr = 0;
    NEG_ERROR_TEST( "Test with missing route number (FIELD)"
                  , "R "
                  , FIELD )

    NEG_ERROR_TEST( "Test with negative number for route number (VALUE)"
                  , "R -7"
                  , VALUE )

    NEG_ERROR_TEST( "Test with blank R line (FIELD)"
                  , "R        "
                  , FIELD )

    END_SUITE
}
