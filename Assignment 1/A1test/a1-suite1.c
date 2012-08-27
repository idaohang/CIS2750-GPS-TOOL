#include <string.h>
#include "exception.h"
#include "mark.h"
#include "gputil.h"
#include "util.h"

#define NUM_POS_TESTS 11
#define NUM_NEG_TESTS 7

#define NEG_ERROR_TEST( desc, buff, field, error ) \
    START_NEG_TEST( ++negCtr, desc ) \
    memset( &wp, 0, sizeof( GpWaypt ) ); \
 \
    try { \
        e = scanGpWaypt( buff \
                       , field \
                       , &wp ); \
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

int s1( int suite, int weight, int argc, char *argv[] ) {
    GpError e;
    int exception;
    int posCtr = 0;
    GpWaypt wp;

    START_SUITE( suite, "scanGpWaypt", weight, NUM_POS_TESTS, NUM_NEG_TESTS );

    START_POS_TEST( ++posCtr, "Test full field size ID" );
    memset( &wp, 0, sizeof( GpWaypt ) );

    try {
        e = scanGpWaypt( "W BRWDWNAG N50.776181 E001.182629"
                       , "F ID------ Latitude   Longitude"
                       , &wp );
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

    try {
        if ( strcmp( "BRWDWNAG", wp.ID ) != 0 ) {
            LOG( "ID '%s' did not match BRWDWNAG\n"
               , wp.ID );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test simplest line that is valid: ID, Lat, Lon" );
    memset( &wp, 0, sizeof( GpWaypt ) );

    try {
        e = scanGpWaypt( "W OMG    N50.776181 E001.182629"
                       , "F ID---- Latitude   Longitude"
                       , &wp );
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

    GpCoord expect;
    expect.lat = 50.776181;
    expect.lon = 1.182629;
    GpWaypt_check( &wp
                 , "OMG   "
                 , &expect
                 , ""
                 , 'I'
                 , 2
                 , "" );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Check that S and W are converted to negative" );
    memset( &wp, 0, sizeof( GpWaypt ) );

    try {
        e = scanGpWaypt( "W OMG    S13.37147 W27.10171"
                       , "F ID---- Latitude   Longitude"
                       , &wp );
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

    expect.lat = -13.37147;
    expect.lon = -27.10171;
    if ( !GpCoord_equals( &wp.coord
                        , &expect ) ) {
        LOG( "COORD (%lf,%lf) did not match expected (%lf,%lf)\n"
           , wp.coord.lat, wp.coord.lon
           , expect.lat, expect.lon );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Check that T is handled correctly" );
    memset( &wp, 0, sizeof( GpWaypt ) );

    try {
        e = scanGpWaypt( "W OMG    N13.37147 E27.10171 C"
                       , "F ID---- Latitude   Longitude T"
                       , &wp );
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

    if ( wp.textChoice != 'C' ) {
        LOG( "Text choice was '%c', not expected 'C'\n"
           , wp.textChoice );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Check that O is handled correctly" );
    memset( &wp, 0, sizeof( GpWaypt ) );

    try {
        e = scanGpWaypt( "W OMG    N13.37147 E27.10171 NE"
                       , "F ID---- Latitude   Longitude O"
                       , &wp );
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

    if ( wp.textPlace != 1 ) {
        LOG( "Text choice was '%d', not expected 1 (NE)\n"
           , wp.textPlace );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test that alt value is ignored" );
    memset( &wp, 0, sizeof( GpWaypt ) );

    try {
        e = scanGpWaypt( "W OMG    N13.37147 E27.10171 7"
                       , "F ID---- Latitude   Longitude Alt"
                       , &wp );
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

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with explicit comment" );
    memset( &wp, 0, sizeof( GpWaypt ) );

    try {
        e = scanGpWaypt( "W OMG    N13.37147 E27.10171 Hello"
                       , "F ID---- Latitude   Longitude Comment"
                       , &wp );
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

    try {
        if ( strcmp( "Hello", wp.comment ) != 0 ) {
            LOG( "Comment '%s', did not match 'Hello'\n"
               , wp.comment );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with missing comment" );
    memset( &wp, 0, sizeof( GpWaypt ) );

    try {
        e = scanGpWaypt( "W OMG    N13.37147 E27.10171"
                       , "F ID---- Latitude   Longitude Comment"
                       , &wp );
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

    try {
        if ( strcmp( "", wp.comment ) != 0 ) {
            LOG( "Comment '%s', did not match ''\n"
               , wp.comment );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with explicit symbol" );
    memset( &wp, 0, sizeof( GpWaypt ) );

    try {
        e = scanGpWaypt( "W OMG    N13.37147 E27.10171 Apple"
                       , "F ID---- Latitude   Longitude Symbol--"
                       , &wp );
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

    try {
        if ( strcmp( "Apple   ", wp.symbol ) != 0 ) {
            LOG( "Symbol '%s', did not match 'Apple   '\n"
               , wp.symbol );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Repeat minimal good test, but shuffle the field order" );
    memset( &wp, 0, sizeof( GpWaypt ) );

    try {
        e = scanGpWaypt( "W N50.776181 OMG     E001.182629"
                       , "F Latitude ID---- Longitude"
                       , &wp );
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

    expect.lat = 50.776181;
    expect.lon = 1.182629;
    GpWaypt_check( &wp
                 , "OMG   "
                 , &expect
                 , ""
                 , 'I'
                 , 2
                 , "" );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Repeat minimal good test, but with mixed case field defs" );
    memset( &wp, 0, sizeof( GpWaypt ) );

    try {
        e = scanGpWaypt( "W OmG    N50.776181 E001.182629"
                       , "F iD---- LaTiTuDe lOnGiTuDe"
                       , &wp );
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

    expect.lat = 50.776181;
    expect.lon = 1.182629;
    GpWaypt_check( &wp
                 , "OmG   "
                 , &expect
                 , ""
                 , 'I'
                 , 2
                 , "" );

    PASS_POS_TEST

    int negCtr = 0;

    NEG_ERROR_TEST( "Test range that is outside of -90 to 90 for lat (VALUE)"
                  , "W BRWDWN N90.01 E001.182629"
                  , "F ID---- Latitude   Longitude"
                  , VALUE )

    NEG_ERROR_TEST( "Test range that is outside of -180 to 180 for lon (VALUE)"
                  , "W BRWDWN N36.01 W181.182629"
                  , "F ID---- Latitude   Longitude"
                  , VALUE )

    NEG_ERROR_TEST( "Test with required field missing (FIELD)"
                  , "W BRWDWN W46.182629"
                  , "F ID---- Longitude"
                  , FIELD )

    NEG_ERROR_TEST( "Test with blank W line (FIELD)"
                  , "W                             "
                  , "F ID---- Latitude Longitude"
                  , FIELD )

    NEG_ERROR_TEST( "Test with more fields than expected (FIELD)"
                  , "W BRWDWN N50.776181 E001.182629 OMG"
                  , "F ID---- Latitude   Longitude"
                  , FIELD )

    NEG_ERROR_TEST( "Test with unrecognized field def (FIELD)"
                  , "W BRWDWN N50.776181 E001.182629 Hello"
                  , "F ID---- Latitude   Longitude OMG"
                  , FIELD)

    NEG_ERROR_TEST( "Test with non-numeric data for lat/lon (VALUE)"
                  , "W BRWDWN NAME ME"
                  , "F ID---- Latitude   Longitude"
                  , VALUE )

    END_SUITE
}
