#define _XOPEN_SOURCE
#include <string.h>
#include <time.h>
#include "exception.h"
#include "mark.h"
#include "gputil.h"
#include "util.h"

#define NUM_POS_TESTS 10
#define NUM_NEG_TESTS 11

#define NEG_ERROR_TEST( desc, fname, error, numLines, lines ) { \
    GpFile ngpf; \
    START_NEG_TEST( ++negCtr, desc ); \
    memset( &ngpf, 0, sizeof( GpFile ) ); \
 \
    FILE * nfp = fopen_check( fname ); \
 \
    try { \
        s = readGpFile( nfp, &ngpf ); \
    } catch ( exception ) { \
        LOG( "%s\n", Exception_Names[exception] ); \
        FAIL_NEG_TEST \
    } \
 \
    if ( s.code != error ) { \
        LOG( "Invalid code %s. Expected %s\n" \
           , GpError_toString( s.code ) \
           , GpError_toString( error ) ); \
        FAIL_NEG_TEST \
    } \
 \
    if ( numLines > 0 ) { \
        int match = 0; \
        for ( int i = 0; i < numLines; ++i ) { \
            if ( lines[i] == s.lineno ) { \
                match = 1; \
                break; \
            } \
        } \
 \
        if ( match == 0 ) { \
            LOG( "Error line no. %d did not match any candidates:" \
               , s.lineno ); \
            for ( int i = 0; i < numLines; ++i ) \
                LOG( " %d", lines[i] ); \
            LOG( "\n" ); \
            FAIL_NEG_TEST \
        } \
    } \
 \
    PASS_NEG_TEST \
}

void GpFile_check( GpFile * gpf
                 , char * dateFormat
                 , int timeZone
                 , char unitHorz
                 , char unitTime
                 , int nwaypts
                 , GpWaypt * waypt
                 , int nroutes
                 , GpRoute ** route
                 , int ntrkpts
                 , GpTrkpt * trkpt ) {
    int exception;

    try {
        if ( strcmp( gpf->dateFormat, dateFormat ) != 0 ) {
            LOG( "dateFormat '%s' did not match '%s'\n"
               , gpf->dateFormat, dateFormat );
            FAIL_POS_TEST
        }

        if ( gpf->timeZone != timeZone ) {
            LOG( "timeZone %d did not match %d\n"
               , gpf->timeZone, timeZone );
            FAIL_POS_TEST
        }

        if ( gpf->unitHorz != unitHorz ) {
            LOG( "unitHorz %c did not match %c\n"
               , gpf->unitHorz, unitHorz );
            FAIL_POS_TEST
        }

        if ( gpf->unitTime != unitTime ) {
            LOG( "unitTime %c did not match %c\n"
               , gpf->unitTime, unitTime );
            FAIL_POS_TEST
        }

        if ( gpf->nwaypts != nwaypts ) {
            LOG( "nwaypts %d did not match %d\n"
               , gpf->nwaypts, nwaypts );
            FAIL_POS_TEST
        }

        for ( int i = 0; i < gpf->nwaypts; ++i ) {
            GpWaypt_equalsCheck( &gpf->waypt[i]
                               , &waypt[i] );
        }

        if ( gpf->nroutes != nroutes ) {
            LOG( "nroutes %d did not match %d\n"
               , gpf->nroutes, nroutes );
            FAIL_POS_TEST
        }

        for ( int i = 0; i < gpf->nroutes; ++i ) {
            GpRoute_equalsCheck( gpf->route[i]
                               , route[i] );
        }

        if ( gpf->ntrkpts != ntrkpts ) {
            LOG( "ntrkpts %d did not match %d\n"
               , gpf->ntrkpts, ntrkpts );
            FAIL_POS_TEST
        }

        for ( int i = 0; i < gpf->ntrkpts; ++i ) {
            GpTrkpt_equalsCheck( &gpf->trkpt[i]
                               , &trkpt[i] );
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }
}

int s5( int suite, int weight, int argc, char *argv[] ) {
    GpStatus s;
    int exception;
    int posCtr = 0;
    GpFile gpf;

    START_SUITE( suite, "readGpFile", weight, NUM_POS_TESTS, NUM_NEG_TESTS );

    START_POS_TEST( ++posCtr, "Test reading a file with default settings (no Settings line) and only one F line" );
    memset( &gpf, 0, sizeof( GpFile ) );

    FILE * fp1 = fopen_check( "a1-suite5-postest1.gps" );

    try {
        s = readGpFile( fp1, &gpf );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( s.code != OK ) {
        LOG( "Invalid code %s. Expected %s\n"
           , GpError_toString( s.code )
           , GpError_toString( OK ) );
        FAIL_POS_TEST
    }

    int nwaypts1 = 3;
    GpWaypt waypts1[nwaypts1];

    GpWaypt_init( &waypts1[0], "BAWDWN", 50.776181, -1.182629, "", 'I', 2, "" );
    GpWaypt_init( &waypts1[1], "CBLPR ", 50.820507, -1.308285, "", 'I', 2, "" );
    GpWaypt_init( &waypts1[2], "CCLSHT", 50.807175, -1.283957, "", 'I', 2, "" );

    GpFile_check( &gpf
                , GP_DATEFORMAT
                , GP_TIMEZONE
                , GP_UNITHORZ
                , GP_UNITTIME
                , nwaypts1
                , waypts1
                , 0
                , NULL
                , 0
                , NULL );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Rewind and use file handle again. (The caller is responsible for eventually freeing filep)" );
    memset( &gpf, 0, sizeof( GpFile ) );

    try {
        rewind( fp1 );

        s = readGpFile( fp1, &gpf );

        if ( s.code != OK ) {
            LOG( "Invalid code %s. Expected %s\n"
               , GpError_toString( s.code )
               , GpError_toString( OK ) );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    GpFile_check( &gpf
                , GP_DATEFORMAT
                , GP_TIMEZONE
                , GP_UNITHORZ
                , GP_UNITTIME
                , nwaypts1
                , waypts1
                , 0
                , NULL
                , 0
                , NULL );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test reading a file with dateformat, timezone and units set to non-default" );
    memset( &gpf, 0, sizeof( GpFile ) );

    FILE * fp3 = fopen_check( "a1-suite5-postest3.gps" );

    try {
        s = readGpFile( fp3, &gpf );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( s.code != OK ) {
        LOG( "Invalid code %s. Expected %s\n"
           , GpError_toString( s.code )
           , GpError_toString( OK ) );
        FAIL_POS_TEST
    }

    GpFile_check( &gpf
                , "%y/%m/%d"
                , 6
                , 'F'
                , 'S'
                , nwaypts1
                , waypts1
                , 0
                , NULL
                , 0
                , NULL );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test that timezone minutes are discarded" );
    memset( &gpf, 0, sizeof( GpFile ) );

    FILE * fp4 = fopen_check( "a1-suite5-postest4.gps" );

    try {
        s = readGpFile( fp4, &gpf );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( s.code != OK ) {
        LOG( "Invalid code %s. Expected %s\n"
           , GpError_toString( s.code )
           , GpError_toString( OK ) );
        FAIL_POS_TEST
    }

    GpFile_check( &gpf
                , "%y/%m/%d"
                , 7
                , 'N'
                , 'H'
                , nwaypts1
                , waypts1
                , 0
                , NULL
                , 0
                , NULL );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test a file with multiple waypoint groups" );
    memset( &gpf, 0, sizeof( GpFile ) );

    FILE * fp5 = fopen_check( "a1-suite5-postest5.gps" );

    try {
        s = readGpFile( fp5, &gpf );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( s.code != OK ) {
        LOG( "Invalid code %s. Expected %s\n"
           , GpError_toString( s.code )
           , GpError_toString( OK ) );
        FAIL_POS_TEST
    }

    int nwaypts5 = 5;
    GpWaypt waypts5[nwaypts5];

    GpWaypt_init( &waypts5[0], "BAWDWN", 50.776181, -1.182629, "", 'I', 2, "" );
    GpWaypt_init( &waypts5[1], "CBLPR ", 50.820507, -1.308285, "", 'I', 2, "" );
    GpWaypt_init( &waypts5[2], "CCLSHT", 50.807175, -1.283957, "", 'I', 2, "" );
    GpWaypt_init( &waypts5[3], "BALDIE", 51.776181, 10.182629, "Symbol  ", 'I', 2, "" );
    GpWaypt_init( &waypts5[4], "FLYER ", 51.820507, 10.308285, "Symbol1 ", 'I', 2, "" );

    GpFile_check( &gpf
                , GP_DATEFORMAT
                , GP_TIMEZONE
                , GP_UNITHORZ
                , GP_UNITTIME
                , nwaypts5
                , waypts5
                , 0
                , NULL
                , 0
                , NULL );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test a file with a route" );
    memset( &gpf, 0, sizeof( GpFile ) );

    FILE * fp6 = fopen_check( "a1-suite5-postest6.gps" );

    try {
        s = readGpFile( fp6, &gpf );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( s.code != OK ) {
        LOG( "Invalid code %s. Expected %s\n"
           , GpError_toString( s.code )
           , GpError_toString( OK ) );
        FAIL_POS_TEST
    }

    int nwaypts6 = 4;
    GpWaypt waypts6[nwaypts6];

    GpWaypt_init( &waypts6[0], "BAWDWN", 50.776181, -1.182629, "", 'I', 2, "" );
    GpWaypt_init( &waypts6[1], "CBLPR ", 50.820507, -1.308285, "", 'I', 2, "" );
    GpWaypt_init( &waypts6[2], "CCLSHT", 50.807175, -1.283957, "", 'I', 2, "" );
    GpWaypt_init( &waypts6[3], "FLYER ", 51.756, 10.283957, "", 'I', 2, "" );

    GpRoute r;

    GpRoute * rp = malloc( sizeof( GpRoute ) + sizeof( r.leg[0] ) * 3 );
    rp->number = 1;
    rp->comment = "";
    rp->npoints = 3;

    rp->leg[0] = 1;
    rp->leg[1] = 3;
    rp->leg[2] = 0;

    GpFile_check( &gpf
                , GP_DATEFORMAT
                , GP_TIMEZONE
                , GP_UNITHORZ
                , GP_UNITTIME
                , nwaypts6
                , waypts6
                , 1
                , &rp
                , 0
                , NULL );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test a file with multiple routes" );
    memset( &gpf, 0, sizeof( GpFile ) );

    FILE * fp7 = fopen_check( "a1-suite5-postest7.gps" );

    try {
        s = readGpFile( fp7, &gpf );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( s.code != OK ) {
        LOG( "Invalid code %s. Expected %s\n"
           , GpError_toString( s.code )
           , GpError_toString( OK ) );
        FAIL_POS_TEST
    }

    GpRoute * rp2 = malloc( sizeof( GpRoute ) + sizeof( r.leg[0] ) * 2 );
    rp2->number = 2;
    rp2->comment = "";
    rp2->npoints = 2;

    rp2->leg[0] = 2;
    rp2->leg[1] = 0;

    GpRoute * routes[2];
    routes[0] = rp;
    routes[1] = rp2;

    GpFile_check( &gpf
                , GP_DATEFORMAT
                , GP_TIMEZONE
                , GP_UNITHORZ
                , GP_UNITTIME
                , nwaypts6
                , waypts6
                , 2
                , routes
                , 0
                , NULL );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test a file with one set of trackpoints" );
    memset( &gpf, 0, sizeof( GpFile ) );

    FILE * fp8 = fopen_check( "a1-suite5-postest8.gps" );

    try {
        s = readGpFile( fp8, &gpf );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( s.code != OK ) {
        LOG( "Invalid code %s. Expected %s\n"
           , GpError_toString( s.code )
           , GpError_toString( OK ) );
        FAIL_POS_TEST
    }

    GpTrkpt t[6];
    struct tm tm={0};
    if ( strptime( "08/10/94 14:16:23", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    GpTrkpt_init( &t[0]
                , 50.733500
                , -1.283383
                , mktime( &tm )
                , 1
                , ""
                , 0
                , 0
                , 0 );

    if ( strptime( "08/10/94 14:16:32", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    GpTrkpt_init( &t[1]
                , 50.733467
                , -1.283050
                , mktime( &tm )
                , 0
                , NULL
                , 9.5
                , 0.024
                , 9 );

    if ( strptime( "08/10/94 14:16:40", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    GpTrkpt_init( &t[2]
                , 50.733300
                , -1.283033
                , mktime( &tm )
                , 0
                , NULL
                , 8.4
                , 0.042
                , 17 );

    GpFile_check( &gpf
                , GP_DATEFORMAT
                , GP_TIMEZONE
                , GP_UNITHORZ
                , GP_UNITTIME
                , 0
                , NULL
                , 0
                , NULL
                , 3
                , t );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test a file with multiple trackpoint groups" );
    memset( &gpf, 0, sizeof( GpFile ) );

    FILE * fp9 = fopen_check( "a1-suite5-postest9.gps" );

    try {
        s = readGpFile( fp9, &gpf );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( s.code != OK ) {
        LOG( "Invalid code %s. Expected %s\n"
           , GpError_toString( s.code )
           , GpError_toString( OK ) );
        FAIL_POS_TEST
    }

    if ( strptime( "08/10/94 14:16:23", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    GpTrkpt_init( &t[3]
                , 50.733500
                , 1.283383
                , mktime( &tm )
                , 1
                , ""
                , 0
                , 0
                , 0 );

    if ( strptime( "08/10/94 14:16:32", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    GpTrkpt_init( &t[4]
                , 50.733467
                , 1.283050
                , mktime( &tm )
                , 0
                , NULL
                , 9.5
                , 0.024
                , 9 );

    if ( strptime( "08/10/94 14:16:40", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    GpTrkpt_init( &t[5]
                , 50.733300
                , 1.283033
                , mktime( &tm )
                , 0
                , NULL
                , 8.4
                , 0.042
                , 17 );

    GpFile_check( &gpf
                , GP_DATEFORMAT
                , GP_TIMEZONE
                , GP_UNITHORZ
                , GP_UNITTIME
                , 0
                , NULL
                , 0
                , NULL
                , 6
                , t );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Test reading a file with \r\n newlines" );
    memset( &gpf, 0, sizeof( GpFile ) );

    FILE * fp10 = fopen_check( "a1-suite5-postest1.gps" );

    try {
        s = readGpFile( fp10, &gpf );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( s.code != OK ) {
        LOG( "Invalid code %s. Expected %s\n"
           , GpError_toString( s.code )
           , GpError_toString( OK ) );
        FAIL_POS_TEST
    }

    GpFile_check( &gpf
                , GP_DATEFORMAT
                , GP_TIMEZONE
                , GP_UNITHORZ
                , GP_UNITTIME
                , nwaypts1
                , waypts1
                , 0
                , NULL
                , 0
                , NULL );

    PASS_POS_TEST

    int negCtr = 0;
    int lines[10];

    lines[0] = 1;
    NEG_ERROR_TEST( "Unknown record type (UNKREC)"
                  , "a1-suite5-negtest1.gps"
                  , UNKREC
                  , 1
                  , lines );

    lines[0] = 3;
    NEG_ERROR_TEST( "Second byte is not a space (BADSEP)"
                  , "a1-suite5-negtest2.gps"
                  , BADSEP
                  , 1
                  , lines );

    lines[0] = 2;
    NEG_ERROR_TEST( "File does not start with GPSU (FILTYP)"
                  , "a1-suite5-negtest3.gps"
                  , FILTYP
                  , 1
                  , lines );

    lines[0] = 3;
    NEG_ERROR_TEST( "File with M that has no WGS64 (DATUM)"
                  , "a1-suite5-negtest4.gps"
                  , DATUM
                  , 1
                  , lines );

    lines[0] = 2;
    NEG_ERROR_TEST( "U without LAT LON DEG (COORD)"
                  , "a1-suite5-negtest5.gps"
                  , COORD
                  , 1
                  , lines );

    NEG_ERROR_TEST( "no 'F' format record prior to data records (NOFORM)"
                  , "a1-suite5-negtest6.gps"
                  , NOFORM
                  , 0
                  , lines );

    lines[0] = 18;
    lines[1] = 12;
    NEG_ERROR_TEST( "Duplicate route number (DUPRT)"
                  , "a1-suite5-negtest7.gps"
                  , DUPRT
                  , 2
                  , lines );

    lines[0] = 6;
    lines[1] = 7;
    NEG_ERROR_TEST( "Required field missing in field definition (FIELD)"
                  , "a1-suite5-negtest8.gps"
                  , FIELD
                  , 2
                  , lines );

    lines[0] = 6;
    lines[1] = 8;
    NEG_ERROR_TEST( "Required data missing (FIELD)"
                  , "a1-suite5-negtest9.gps"
                  , FIELD
                  , 2
                  , lines );

    lines[0] = 7;
    lines[1] = 10;
    NEG_ERROR_TEST( "Excess data beyond the field defined in F line (FIELD)"
                  , "a1-suite5-negtest10.gps"
                  , FIELD
                  , 2
                  , lines );

    GpFile ngpf;
    START_NEG_TEST( ++negCtr, "Use file that has no read permission (IOERR)" );
    memset( &ngpf, 0, sizeof( GpFile ) );

    FILE * nfp = fopen( "a1-suite5-negtest11.gps", "a" );

    try {
        s = readGpFile( nfp, &ngpf );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_NEG_TEST
    }

    if ( s.code != IOERR ) {
        LOG( "Invalid code %s. Expected %s\n"
           , GpError_toString( s.code )
           , GpError_toString( IOERR ) );
        FAIL_NEG_TEST
    }

    PASS_NEG_TEST

    END_SUITE
}
