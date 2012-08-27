#define _XOPEN_SOURCE
#include <string.h>
#include <time.h>
#include "exception.h"
#include "mark.h"
#include "gputil.h"
#include "util.h"

#define NUM_POS_TESTS 4
#define NUM_NEG_TESTS 0

void GpFile_initDefault( GpFile * gpf ) {
    gpf->dateFormat = GP_DATEFORMAT;
    gpf->timeZone = GP_TIMEZONE;
    gpf->unitHorz = GP_UNITHORZ;
    gpf->unitTime = GP_UNITTIME;

    gpf->nwaypts = 0;
    gpf->waypt = NULL;

    gpf->nroutes = 0;
    gpf->route = NULL;

    gpf->ntrkpts = 0;
    gpf->trkpt = NULL;
}

void GpFile_loadTrkpts( GpFile * gpf
                      , time_t * start
                      , time_t * end ) {
    struct tm tm={0};
    if ( strptime( "08/10/94 14:16:23", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    *start = mktime( &tm );

    GpTrkpt_init( &gpf->trkpt[0]
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

    GpTrkpt_init( &gpf->trkpt[1]
                , 50.733467
                , -1.283050
                , mktime( &tm )
                , 0
                , ""
                , 9.5
                , 0.024
                , 9 );

    if ( strptime( "08/10/94 14:16:40", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    GpTrkpt_init( &gpf->trkpt[2]
                , 50.733300
                , -1.283033
                , mktime( &tm )
                , 0
                , ""
                , 8.4
                , 0.042
                , 17 );

    if ( strptime( "08/10/94 14:17:05", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    GpTrkpt_init( &gpf->trkpt[3]
                , 50.732950
                , -1.282816
                , mktime( &tm )
                , 0
                , ""
                , 6.0
                , 0.084
                , 42 );

    if ( strptime( "08/10/94 14:17:15", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    *end = mktime( &tm );

    GpTrkpt_init( &gpf->trkpt[4]
                , 50.732784
                , -1.282866
                , mktime( &tm )
                , 0
                , ""
                , 6.8
                , 0.103
                , 52 );
}

void GpTrack_check( const GpTrack * const t
                  , int seqno
                  , time_t startTrk
                  , time_t endTrk
                  , long duration
                  , double dist
                  , float speed
                  , const GpCoord * NEcorner
                  , const GpCoord * SWcorner
                  , const GpCoord * meanCoord ) {
    int exception;

    try {
        if ( t->seqno != seqno ) {
            LOG( "seqno '%d' did not match '%d'\n"
               , t->seqno, seqno );
            FAIL_POS_TEST
        }

        if ( t->startTrk != startTrk ) {
            LOG( "startTrk '%ld' did not match '%ld'\n"
               , t->startTrk, startTrk );
            FAIL_POS_TEST
        }

        if ( t->endTrk != endTrk ) {
            LOG( "endTrk '%ld' did not match '%ld'\n"
               , t->endTrk, endTrk );
            FAIL_POS_TEST
        }

        if ( t->duration != duration ) {
            LOG( "duration '%ld' did not match '%ld'\n"
               , t->duration, duration );
            FAIL_POS_TEST
        }

        if ( !FEQUALS( t->dist, dist, TOLERANCE ) ) {
            LOG( "dist '%lf' did not match '%lf'\n"
               , t->dist, dist );
            FAIL_POS_TEST
        }

        if ( !FEQUALS( t->speed, speed, TOLERANCE ) ) {
            LOG( "speed '%f' did not match '%f'\n"
               , t->speed, speed );
            FAIL_POS_TEST
        }

        if ( !GpCoord_equals( &t->NEcorner
                            , NEcorner ) ) {
            LOG( "NEcorner (%lf,%lf) did not match (%lf,%lf)\n"
               , t->NEcorner.lat, t->NEcorner.lon
               , NEcorner->lat, NEcorner->lon );
            FAIL_POS_TEST
        }

        if ( !GpCoord_equals( &t->SWcorner
                            , SWcorner ) ) {
            LOG( "SWcorner (%lf,%lf) did not match (%lf,%lf)\n"
               , t->SWcorner.lat, t->SWcorner.lon
               , SWcorner->lat, SWcorner->lon );
            FAIL_POS_TEST
        }

        if ( !GpCoord_equals( &t->meanCoord
                            , meanCoord ) ) {
            LOG( "meanCoord (%lf,%lf) did not match (%lf,%lf)\n"
               , t->meanCoord.lat, t->meanCoord.lon
               , meanCoord->lat, meanCoord->lon );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }
}

int s6( int suite, int weight, int argc, char *argv[] ) {
    int exception;
    int posCtr = 0;
    GpFile gpf;
    GpTrack * tp;

    START_SUITE( suite, "getGpTracks", weight, NUM_POS_TESTS, NUM_NEG_TESTS );

    START_POS_TEST( ++posCtr, "Run on GpFile with no trackpoints, ensure 0 and NULL" );
    memset( &gpf, 0, sizeof( GpFile ) );

    GpFile_initDefault( &gpf );
    tp = malloc( sizeof(GpTrack) );	// see if it gets NULLed out

    try {
        if ( getGpTracks( &gpf, &tp ) != 0 ) {
            LOG( "return did not match 0 with no trackpoints\n" );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( tp != NULL ) {
        LOG( "*tp was not NULL after no trackpoint call\n" );
        FAIL_POS_TEST
    }

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "GpFile with one track for minimal functionality" );
    memset( &gpf, 0, sizeof( GpFile ) );

    GpFile_initDefault( &gpf );
    tp = NULL;

    /*F Latitude   Longitude   Date     Time     S Duration       km   km/h
      T N50.733500 W001.283383 08/10/94 14:16:23 1
      T N50.733467 W001.283050 08/10/94 14:16:32 0  0:00:09    0.024    9.5
      T N50.733300 W001.283033 08/10/94 14:16:40 0  0:00:17    0.042    8.4
      T N50.732950 W001.282816 08/10/94 14:17:05 0  0:00:42    0.084    6.0
      T N50.732784 W001.282866 08/10/94 14:17:15 0  0:00:52    0.103    6.8

    int seqno;          // 1
    time_t startTrk;    // 08/10/94 14:16:23
    time_t endTrk;      // 08/10/94 14:17:15
    long duration;      // 52
    double dist;        // .103
    float speed;        // .103km/52s = 7.130769231
    GpCoord NEcorner;   // N50.733500,W001.282866
    GpCoord SWcorner;   // N50.732784,W001.283383
    GpCoord meanCoord;  // (50.733500+50.732784)/2, (-1.282866+-1.283383)/2
    */

    gpf.ntrkpts = 5;
    gpf.trkpt = malloc( sizeof( GpTrkpt ) * gpf.ntrkpts );
    time_t startTrk2;
    time_t endTrk2;

    GpFile_loadTrkpts( &gpf
                     , &startTrk2
                     , &endTrk2 );

    try {
        if ( getGpTracks( &gpf, &tp ) != 1 ) {
            LOG( "return did not match 1 with one set of trackpoints\n" );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( tp == NULL ) {
        LOG( "tp was NULL after one set of trackpoints call\n" );
        FAIL_POS_TEST
    }

    GpCoord NEcorner2; // N50.733500,W001.282866
    NEcorner2.lat = 50.733500;
    NEcorner2.lon = -1.282866;
    GpCoord SWcorner2;// N50.732784,W001.283383
    SWcorner2.lat = 50.732784;
    SWcorner2.lon = -1.283383;
    GpCoord meanCoord2;
    meanCoord2.lat = ( NEcorner2.lat + SWcorner2.lat ) / 2.0;
    meanCoord2.lon = ( NEcorner2.lon + SWcorner2.lon ) / 2.0;

    GpTrack_check( tp
                 , 1
                 , startTrk2
                 , endTrk2
                 , 52
                 , 0.103
                 , 0.103/52*3600
                 , &NEcorner2
                 , &SWcorner2
                 , &meanCoord2 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Run on GpFile with multiple tracks" );

    memset( &gpf, 0, sizeof( GpFile ) );

    GpFile_initDefault( &gpf );
    tp = NULL;

    /*F Latitude   Longitude   Date     Time     S Duration       km   km/h
      T N50.733500 W001.283383 08/10/94 14:16:23 1
      T N50.733467 W001.283050 08/10/94 14:16:32 0  0:00:09    0.024    9.5
      T N50.733300 W001.283033 08/10/94 14:16:40 0  0:00:17    0.042    8.4
      T N50.732950 W001.282816 08/10/94 14:17:05 0  0:00:42    0.084    6.0
      T N50.732784 W001.282866 08/10/94 14:17:15 0  0:00:52    0.103    6.8
      T N50.704583 W001.291450 09/10/94 12:29:53 1
      T N50.705216 W001.291316 09/10/94 12:30:28 0  0:00:35    0.071    7.3

    int seqno;          // 1
    time_t startTrk;    // 08/10/94 14:16:23
    time_t endTrk;      // 08/10/94 14:17:15
    long duration;      // 52
    double dist;        // .103
    float speed;        // .103km/52s = 7.130769231
    GpCoord NEcorner;   // N50.733500,W001.282866
    GpCoord SWcorner;   // N50.732784,W001.283383
    GpCoord meanCoord;  // (50.733500+50.732784)/2, (-1.282866+-1.283383)/2

    int seqno;          // 6
    time_t startTrk;    // 09/10/94 12:29:53
    time_t endTrk;      // 09/10/94 12:30:28
    long duration;      // 35
    double dist;        // .071
    float speed;        // .071km/35s = 7.302857143
    GpCoord NEcorner;   // N50.705216,W001.291316
    GpCoord SWcorner;   // N50.704583,W001.291450
    GpCoord meanCoord;  // (50.705216+50.704583)/2, (-1.291316+-1.291450)/2
    */

    gpf.ntrkpts = 7;
    gpf.trkpt = malloc( sizeof( GpTrkpt ) * gpf.ntrkpts );
    time_t startTrk3a;
    time_t endTrk3a;
    time_t startTrk3b;
    time_t endTrk3b;

    GpFile_loadTrkpts( &gpf
                     , &startTrk3a
                     , &endTrk3a );

    struct tm tm={0};
    if ( strptime( "09/10/94 12:29:53", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    startTrk3b = mktime( &tm );

    GpTrkpt_init( &gpf.trkpt[5]
                , 50.704583
                , -1.291450
                , mktime( &tm )
                , 1
                , ""
                , 0
                , 0
                , 0 );

    if ( strptime( "09/10/94 12:30:28", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    endTrk3b = mktime( &tm );

    GpTrkpt_init( &gpf.trkpt[6]
                , 50.705216
                , -1.291316
                , mktime( &tm )
                , 0
                , ""
                , 7.3
                , 0.071
                , 35 );

    try {
        if ( getGpTracks( &gpf, &tp ) != 2 ) {
            LOG( "return did not match 2 with two sets of trackpoints\n" );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( tp == NULL ) {
        LOG( "tp was NULL after two sets of trackpoints call\n" );
        FAIL_POS_TEST
    }

    GpTrack_check( &tp[0]
                 , 1
                 , startTrk3a
                 , endTrk3a
                 , 52
                 , 0.103
                 , 0.103/52*3600
                 , &NEcorner2
                 , &SWcorner2
                 , &meanCoord2 );

    GpCoord NEcorner3; // N50.705216,W001.291316
    NEcorner3.lat = 50.705216;
    NEcorner3.lon = -1.291316;
    GpCoord SWcorner3;// N50.704583,W001.291450
    SWcorner3.lat = 50.704583;
    SWcorner3.lon = -1.291450;
    GpCoord meanCoord3;
    meanCoord3.lat = ( NEcorner3.lat + SWcorner3.lat ) / 2.0;
    meanCoord3.lon = ( NEcorner3.lon + SWcorner3.lon ) / 2.0;

    GpTrack_check( &tp[1]
                 , 6
                 , startTrk3b
                 , endTrk3b
                 , 35
                 , 0.071
                 , 0.071/35*3600
                 , &NEcorner3
                 , &SWcorner3
                 , &meanCoord3 );

    PASS_POS_TEST

    START_POS_TEST( ++posCtr, "Trackpoints on opposite sides of 0 lat/lon have correct corners" );

    memset( &gpf, 0, sizeof( GpFile ) );

    GpFile_initDefault( &gpf );
    tp = NULL;

    /*F Latitude   Longitude   Date     Time     S Duration       km   km/h
      T S0.5 W001.29       09/01/02 10:46:23 1
      T S0.1 W000.29       09/01/02 10:47:43 0  0:01:20    0.080    3.6
      T N0.3 E000.29       09/01/02 10:50:30 0  0:04:07    0.318    5.1
      T N0.745 E001.295500 09/01/02 10:52:19 0  0:05:56    0.470    5.0

      int seqno;          // 1
      time_t startTrk;    // 09/01/02 10:46:23
      time_t endTrk;      // 09/01/02 10:52:19
      long duration;      // 0:05:56
      double dist;        // .47
      float speed;        // .47km/(5*60+56)s
      GpCoord NEcorner;   // N0.745,E001.2955
      GpCoord SWcorner;   // S0.5,W001.29
      GpCoord meanCoord;  // (0.745+-0.5)/2, (1.2955+-1.29)/2
    */

    gpf.ntrkpts = 4;
    gpf.trkpt = malloc( sizeof( GpTrkpt ) * gpf.ntrkpts );
    time_t start4;
    time_t end4;

    if ( strptime( "09/01/02 10:46:23", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    start4 = mktime( &tm );

    GpTrkpt_init( &gpf.trkpt[0]
                , -0.5
                , -1.29
                , mktime( &tm )
                , 1
                , ""
                , 0
                , 0
                , 0 );

    if ( strptime( "09/01/02 10:47:43", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    GpTrkpt_init( &gpf.trkpt[1]
                , -0.1
                , -0.29
                , mktime( &tm )
                , 0
                , ""
                , 3.6
                , 0.08
                , 1*60+20 );

    if ( strptime( "09/01/02 10:50:30", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    GpTrkpt_init( &gpf.trkpt[2]
                , 0.3
                , 0.29
                , mktime( &tm )
                , 0
                , ""
                , 5.1
                , 0.318
                , 4*60+7 );

    if ( strptime( "09/01/02 10:52:19", GP_DATEFORMAT" %H:%M:%S", &tm ) == NULL ) {
        LOG( "TEST SUITE FAILED TO CREATE TIME\n" );
        abort();
    }
    tm.tm_isdst = -1;	// DST info not available

    end4 = mktime( &tm );

    GpTrkpt_init( &gpf.trkpt[3]
                , 0.745
                , 1.295500
                , mktime( &tm )
                , 0
                , ""
                , 5.0
                , 0.47
                , 5*60+56 );

    try {
        if ( getGpTracks( &gpf, &tp ) != 1 ) {
            LOG( "return did not match 1 with one set of trackpoints\n" );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( tp == NULL ) {
        LOG( "tp was NULL after two sets of trackpoints call\n" );
        FAIL_POS_TEST
    }

    GpCoord NEcorner4; // N0.745,E001.2955
    NEcorner4.lat = 0.745;
    NEcorner4.lon = 1.2955;
    GpCoord SWcorner4; // S0.5,W001.29
    SWcorner4.lat = -0.5;
    SWcorner4.lon = -1.29;
    GpCoord meanCoord4; // (0.745+-0.5)/2, (1.2955+-1.29)/2
    meanCoord4.lat = ( NEcorner4.lat + SWcorner4.lat ) / 2.0;
    meanCoord4.lon = ( NEcorner4.lon + SWcorner4.lon ) / 2.0;

    GpTrack_check( &tp[0]
                 , 1
                 , start4
                 , end4
                 , 5*60+56
                 , 0.47
                 , 0.47/(5*60+56)*3600
                 , &NEcorner4
                 , &SWcorner4
                 , &meanCoord4 );

    PASS_POS_TEST

    END_SUITE
}
