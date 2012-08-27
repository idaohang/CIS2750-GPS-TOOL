#define _GNU_SOURCE
#include <string.h>
#include "exception.h"
#include "mark.h"
#include "gputil.h"
#include "util.h"

#define NUM_POS_TESTS 1
#define NUM_NEG_TESTS 0

int s7( int suite, int weight, int argc, char *argv[] ) {
    START_SUITE( suite, "freeGpFile", weight, NUM_POS_TESTS, NUM_NEG_TESTS );

    START_POS_TEST( 1, "Test freeGpFile doesn't blow up" );

    GpFile fp;

    fp.dateFormat = strdup( GP_DATEFORMAT );
    fp.nwaypts = 2;
    fp.waypt = checkMalloc( sizeof( GpWaypt ) * fp.nwaypts );

    fp.waypt[0].ID = strdup( "ID1" );
    fp.waypt[0].symbol = strdup( "SYMBOL1" );
    fp.waypt[0].comment = strdup( "COMMENT1" );

    fp.waypt[1].ID = strdup( "ID2" );
    fp.waypt[1].symbol = strdup( "SYMBOL2" );
    fp.waypt[1].comment = strdup( "COMMENT2" );

    fp.nroutes = 3;

    GpRoute r;
    fp.route = checkMalloc( sizeof( GpRoute * ) * fp.nroutes );
    fp.route[0] = checkMalloc( sizeof( GpRoute ) + sizeof( r.leg[0] ) );
    fp.route[0]->npoints = 1;
    fp.route[0]->comment = strdup( "TEH COMMENHT1" );
    fp.route[1] = checkMalloc( sizeof( GpRoute ) + sizeof( r.leg[0] ) * 2 );
    fp.route[1]->npoints = 2;
    fp.route[1]->comment = strdup( "TEH COMMENHT2" );
    fp.route[2] = checkMalloc( sizeof( GpRoute ) + sizeof( r.leg[0] ) * 3 );
    fp.route[2]->npoints = 3;
    fp.route[2]->comment = strdup( "TEH COMMENHT3" );

    fp.ntrkpts = 1;
    fp.trkpt = checkMalloc( sizeof( GpTrkpt ) * fp.ntrkpts );
    fp.trkpt[0].segFlag = 1;
    fp.trkpt[0].comment = strdup( "A COMMENT" );

    int exception;

    try {
        freeGpFile( &fp );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    END_SUITE
}
