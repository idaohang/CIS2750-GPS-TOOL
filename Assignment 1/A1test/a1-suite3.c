#include <string.h>
#include "exception.h"
#include "mark.h"
#include "gputil.h"
#include "util.h"

#define NUM_POS_TESTS 2
#define NUM_NEG_TESTS 2

#define NEG_ERROR_TEST( desc, error, buff, fieldDef, wp, nwp ) {\
    GpError e; \
    int exception; \
    GpRoute r; \
 \
    START_NEG_TEST( ++negCtr, desc ) \
    memset( &r, 0, sizeof( GpRoute ) ); \
 \
    GpRoute * rp = malloc( sizeof(GpRoute) + sizeof( int ) * 1 ); \
    rp->npoints++; \
 \
    try { \
        e = scanGpLeg( buff \
                     , fieldDef \
                     , wp \
                     , nwp \
                     , rp ); \
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
    PASS_NEG_TEST \
}

void scanGpLeg_check( const char * fieldDef
                    , int numLegs
                    , char ** legs
                    , int numWpts
                    , GpWaypt * wpts
                    , int * pointIdxs
                    , double * dists ) {	// dists no longer being used
    GpError e;
    int exception;
    GpRoute r;

    memset( &r, 0, sizeof( GpRoute ) );

    GpRoute * rp = malloc( sizeof(GpRoute) );

    try {
        e = scanGpRoute( "R 03"
                       , rp );
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

    rp->npoints = 0;

    try {
        for ( int i = 0; i < numLegs; ++i ) {
            rp->npoints++;
            rp = realloc( rp, sizeof( GpRoute ) + sizeof(int) * ( rp->npoints ) );

            if ( rp == NULL ) {
                LOG( "TEST SUITE FAILED TO MALLOC MEMORY\n" );
                abort();
            }

            e = scanGpLeg( legs[i]
                         , fieldDef
                         , wpts
                         , numWpts
                         , rp );

            if ( e != OK ) {
                LOG( "Invalid error return %s. Expected %s\n"
                   , GpError_toString( e )
                   , GpError_toString( OK ) );
                FAIL_POS_TEST
            }

            //Check all points up to this one to ensure they weren't modified
            for ( int j = 0; j <= i; ++j ) {
                if ( rp->leg[j] != pointIdxs[j] ) {
		    LOG( "On W line %d of %d, leg[%d] of '%d' did not match expected '%d'.\n"
                       , i+1, numLegs, j, rp->leg[j], pointIdxs[j] );
                    FAIL_POS_TEST
                }
            }
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }
}

int s3( int suite, int weight, int argc, char *argv[] ) {
    int posCtr = 0;

    START_SUITE( suite, "scanGpLeg", weight, NUM_POS_TESTS, NUM_NEG_TESTS );

    START_POS_TEST( ++posCtr, "Test simple valid line (only ID) with max field size" );

    /*
    R 03 CALSHT - RYDEPH
    F ID----
    W CALSHT // W CALSHT N50.807175 E001.283957 Waypoint   I CALSHOT
    W NORRIS // W NORRIS N50.765847 E001.258123 Waypoint   I NORRIS
    W PEELWK // W PEELWK N50.748516 E001.223791 Waypoint   I PEEL WRECK
    W RYDEPH // W RYDEPH N50.739681 E001.159965 House      I RYDE PIER HEAD
    */

    int numWpts1 = 5;
    GpWaypt wpts1[numWpts1];
    GpWaypt_init( &wpts1[0], "CALSHT", 50.807175, 1.283957, "", 'I', 2, "" );
    GpWaypt_init( &wpts1[1], "RYDEPH", 50.739681, 1.159965, "", 'I', 2, "" );
    GpWaypt_init( &wpts1[2], "OMG   ", 57.78, 10.159965, "", 'I', 2, "" );
    GpWaypt_init( &wpts1[3], "PEEL  ", 50.748516, 1.223791, "", 'I', 2, "" );
    GpWaypt_init( &wpts1[4], "NORRIS", 50.765847, 1.258123, "", 'I', 2, "" );

    int numLegs1 = 4;
    char * legs1[numLegs1];
    legs1[0] = "W CALSHT";
    legs1[1] = "W NORRIS";
    legs1[2] = "W PEEL  ";
    legs1[3] = "W RYDEPH";

    int pointIdxs1[numLegs1];
    pointIdxs1[0] = 0;
    pointIdxs1[1] = 4;
    pointIdxs1[2] = 3;
    pointIdxs1[3] = 1;

    double dists1[numLegs1];
    dists1[0] = 0.0;
    dists1[1] = 0.0;
    dists1[2] = 0.0;
    dists1[3] = 0.0;

    scanGpLeg_check( "F ID----"
                   , numLegs1
                   , legs1
                   , numWpts1
                   , wpts1
                   , pointIdxs1
                   , dists1 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test with range (ignored)" );
    /*
    R 03 CALSHT - RYDEPH
    F ID---- Range
    W CALSHT 0.5   // W CALSHT N50.807175 E001.283957 Waypoint   I CALSHOT
    W NORRIS 0.75  // W NORRIS N50.765847 E001.258123 Waypoint   I NORRIS
    W PEELWK 0.1   // W PEELWK N50.748516 E001.223791 Waypoint   I PEEL WRECK
    W RYDEPH 1.267 // W RYDEPH N50.739681 E001.159965 House      I RYDE PIER HEAD
    */

    int numWpts2 = 5;
    GpWaypt wpts2[numWpts2];
    GpWaypt_init( &wpts2[0], "CALSHT", 50.807175, 1.283957, "", 'I', 2, "" );
    GpWaypt_init( &wpts2[1], "RYDEPH", 50.739681, 1.159965, "", 'I', 2, "" );
    GpWaypt_init( &wpts2[2], "OMG   ", 57.78, 10.159965, "", 'I', 2, "" );
    GpWaypt_init( &wpts2[3], "PEEL  ", 50.748516, 1.223791, "", 'I', 2, "" );
    GpWaypt_init( &wpts2[4], "NORRIS", 50.765847, 1.258123, "", 'I', 2, "" );

    int numLegs2 = 4;
    char * legs2[numLegs2];
    legs2[0] = "W CALSHT 24.5";
    legs2[1] = "W NORRIS 1.6";
    legs2[2] = "W PEEL   5.2";
    legs2[3] = "W RYDEPH 21";

    int pointIdxs2[numLegs2];
    pointIdxs2[0] = 0;
    pointIdxs2[1] = 4;
    pointIdxs2[2] = 3;
    pointIdxs2[3] = 1;

    double dists2[numLegs2];
    dists2[0] = 0.5;
    dists2[1] = 0.75;
    dists2[2] = 0.89;
    dists2[3] = 1.267;

    scanGpLeg_check( "F ID---- Range"
                   , numLegs2
                   , legs2
                   , numWpts2
                   , wpts2
                   , pointIdxs2
                   , dists2 );

    PASS_POS_TEST

    int negCtr = 0;

    GpWaypt wptsn1[0];

    NEG_ERROR_TEST( "Test blank W line (FIELD)"
                  , FIELD
                  , "W         "
                  , "F ID----"
                  , wptsn1
                  , 0 )

    int numWptsn2 = 5;
    GpWaypt wptsn2[numWptsn2];
    GpWaypt_init( &wptsn2[0], "CALSHT", 50.807175, 1.283957, "", 'I', 2, "" );
    GpWaypt_init( &wptsn2[1], "RYDEPH", 50.739681, 1.159965, "", 'I', 2, "" );
    GpWaypt_init( &wptsn2[2], "PEEL  ", 50.748516, 1.223791, "", 'I', 2, "" );
    GpWaypt_init( &wptsn2[3], "NORRIS", 50.765847, 1.258123, "", 'I', 2, "" );

    NEG_ERROR_TEST( "Test with unknown waypoint (UNKWPT)"
                  , UNKWPT
                  , "W OMGHEY"
                  , "F ID----"
                  , wptsn2
                  , numWptsn2 - 1 ) //Really check if they stop at the end


    END_SUITE
}
