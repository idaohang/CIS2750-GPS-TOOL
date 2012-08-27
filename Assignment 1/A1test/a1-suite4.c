#define _XOPEN_SOURCE
#include <string.h>
#include <time.h>
#include "exception.h"
#include "mark.h"
#include "gputil.h"
#include "util.h"

#define NUM_POS_TESTS 3
#define NUM_NEG_TESTS 11

#define NEG_ERROR_TEST( desc, error, buff, fieldDef, dateFormat ) \
    START_NEG_TEST( ++negCtr, desc ) \
    memset( &t, 0, sizeof( GpTrkpt ) ); \
 \
    try { \
        e = scanGpTrkpt( buff \
                       , fieldDef \
                       , dateFormat \
                       , &t ); \
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

int s4( int suite, int weight, int argc, char *argv[] ) {
    GpTrkpt t;
    GpError e;
    int exception;

    START_SUITE( suite, "scanGpTrkpt", weight, NUM_POS_TESTS, NUM_NEG_TESTS );
    int posCtr = 0;

    START_POS_TEST( ++posCtr, "Check S with 1, that comment is everything else on the line" );
    memset( &t, 0, sizeof( GpTrkpt ) );

    try {
        e = scanGpTrkpt( "T N55.733500 08/10/97 W10.283383 18:26:23 1 Hello there people!"
                       , "F Latitude   Date     Longitude   Time     S Duration       km   km/h"
                       , "%m/%d/%y"
                       , &t );
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

    GpCoord expectCoord;
    expectCoord.lat = 55.733500;
    expectCoord.lon = -10.283383;

    struct tm tm={0};
    if ( strptime( "08/10/97 18:26:23", "%m/%d/%y %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    GpTrkpt_check( &t
                 , &expectCoord
                 , mktime( &tm )
                 , 1
                 , "Hello there people!"
                 , 0.0
                 , 0.0
                 , 0.0 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Check S with 0 that all data is loaded" );
    memset( &t, 0, sizeof( GpTrkpt ) );

    try {
        e = scanGpTrkpt( "T 94-10-08 S55.732950 E67.7828 7:17:27 0  0:01:12    0.126    6.3"
                       , "F Date     Latitude   Longitude   Time     S Duration       km   km/h"
                       , "%y-%m-%d"
                       , &t );
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

    expectCoord.lat = -55.732950;
    expectCoord.lon = 67.7828;

    strptime( "94-10-08 7:17:27", "%y-%m-%d %H:%M:%S", &tm );
    tm.tm_isdst = -1;	// DST info not available
    GpTrkpt_check( &t
                 , &expectCoord
                 , mktime( &tm )
                 , 0
                 , NULL
                 , 6.3
                 , 0.126
                 , 72 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test that altitude is silently ignored" );
    memset( &t, 0, sizeof( GpTrkpt ) );

    try {
        e = scanGpTrkpt( "T 94-10-08 S55.732950 E67.7828 7:17:27 127 0 0:01:12    0.126    6.3"
                       , "F Date     Latitude   Longitude   Time     Alt S Duration       km   km/h"
                       , "%y-%m-%d"
                       , &t );
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

    expectCoord.lat = -55.732950;
    expectCoord.lon = 67.7828;

    strptime( "94-10-08 7:17:27", "%y-%m-%d %H:%M:%S", &tm );
    tm.tm_isdst = -1;	// DST info not available
    GpTrkpt_check( &t
                 , &expectCoord
                 , mktime( &tm )
                 , 0
                 , NULL
                 , 6.3
                 , 0.126
                 , 72 );

    PASS_POS_TEST

    int negCtr = 0;

    NEG_ERROR_TEST( "Latitude out of range (VALUE)"
                  , VALUE
                  , "T N96.866283 W001.365933 09/01/02 13:21:48 0  2:35:25   17.161    8.3"
                  , "F Latitude   Longitude   Date     Time     S Duration       km   km/h"
                  , GP_DATEFORMAT )

    NEG_ERROR_TEST( "Longitude out of range (VALUE)"
                  , VALUE
                  , "T N80.866283 W200.365933 09/01/02 13:21:48 0  2:35:25   17.161    8.3"
                  , "F Latitude   Longitude   Date     Time     S Duration       km   km/h"
                  , GP_DATEFORMAT )

    NEG_ERROR_TEST( "bad segflag (VALUE)"
                  , VALUE
                  , "T N50.857766 W001.350901 09/09/02 13:06:38 3  2:20:15   15.578    6.8"
                  , "F Latitude   Longitude   Date     Time     S Duration       km   km/h"
                  , GP_DATEFORMAT )

    NEG_ERROR_TEST( "bad speed (VALUE)"
                  , VALUE
                  , "T N50.857766 W001.350901 09/09/02 13:06:38 0  2:20:15 15.578    ASDF"
                  , "F Latitude   Longitude   Date     Time     S Duration       km   km/h"
                  , GP_DATEFORMAT )

    NEG_ERROR_TEST( "bad distance (VALUE)"
                  , VALUE
                  , "T N50.857766 W001.350901 09/09/02 13:06:38 0  2:20:15 ASDF    6.8"
                  , "F Latitude   Longitude   Date     Time     S Duration       km   km/h"
                  , GP_DATEFORMAT )

    NEG_ERROR_TEST( "bad duration (VALUE)"
                  , VALUE
                  , "T N50.857766 W001.350901 09/09/02 13:06:38 0  ADF   15.578    6.8"
                  , "F Latitude   Longitude   Date     Time     S Duration       km   km/h"
                  , GP_DATEFORMAT )

    NEG_ERROR_TEST( "S is 0 but no data afterwards (FIELD)"
                  , FIELD
                  , "T N50.857766 W001.350901 09/09/02 13:06:38 0"
                  , "F Latitude   Longitude   Date     Time     S Duration       km   km/h"
                  , GP_DATEFORMAT )

    NEG_ERROR_TEST( "Test with blank T line (FIELD)"
                  , FIELD
                  , "T                   "
                  , "F Latitude   Longitude   Date     Time     S Duration       km   km/h"
                  , GP_DATEFORMAT )

    NEG_ERROR_TEST( "Test with missing required field (FIELD)"
                  , FIELD
                  , "T N50.857766 09/09/02 13:06:38 0  00:00:12   15.578    6.8"
                  , "F Latitude   Date     Time     S Duration       km   km/h"
                  , GP_DATEFORMAT )

    NEG_ERROR_TEST( "Test with more fields than expected (FIELD)"
                  , FIELD
                  , "T N50.864650 W001.361300 09/01/02 13:19:07 0  2:32:44   16.788    7.4 EXTRA_DATA"
                  , "F Latitude   Longitude   Date     Time     S Duration       km   km/h"
                  , GP_DATEFORMAT )

    NEG_ERROR_TEST( "Test with unknown field in definition (FIELD)"
                  , FIELD
                  , "T N50.864650 W001.361300 09/01/02 13:19:07 0  2:32:44   16.788    7.4 data"
                  , "F Latitude   Longitude   Date     Time     S Duration       km   km/h SARDINES"
                  , GP_DATEFORMAT )

    END_SUITE
}
