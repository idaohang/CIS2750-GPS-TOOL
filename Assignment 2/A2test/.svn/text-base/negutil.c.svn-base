// negutil.c

#include "negutil.h"

#include "exception.h"
#include "mark.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void NEG_GpFile_compare( GpFile * a
                       , GpFile * b ) {
    NEG_GpFile_check( a
                , b->dateFormat
                , b->timeZone
                , b->unitHorz
                , b->unitTime
                , b->nwaypts
                , b->waypt
                , b->nroutes
                , b->route
                , b->ntrkpts
                , b->trkpt );
}

void NEG_GpFile_check( GpFile * gpf
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
            FAIL_NEG_TEST
        }

        if ( gpf->timeZone != timeZone ) {
            LOG( "timeZone %d did not match %d\n"
               , gpf->timeZone, timeZone );
            FAIL_NEG_TEST
        }

        if ( gpf->unitHorz != unitHorz ) {
            LOG( "unitHorz %c did not match %c\n"
               , gpf->unitHorz, unitHorz );
            FAIL_NEG_TEST
        }

        if ( gpf->unitTime != unitTime ) {
            LOG( "unitTime %c did not match %c\n"
               , gpf->unitTime, unitTime );
            FAIL_NEG_TEST
        }

        if ( gpf->nwaypts != nwaypts ) {
            LOG( "nwaypts %d did not match %d\n"
               , gpf->nwaypts, nwaypts );
            FAIL_NEG_TEST
        }

        for ( int i = 0; i < gpf->nwaypts; ++i ) {
            NEG_GpWaypt_equalsCheck( &gpf->waypt[i]
                               , &waypt[i] );
        }

        if ( gpf->nroutes != nroutes ) {
            LOG( "nroutes %d did not match %d\n"
               , gpf->nroutes, nroutes );
            FAIL_NEG_TEST
        }

        for ( int i = 0; i < gpf->nroutes; ++i ) {
            NEG_GpRoute_equalsCheck( gpf->route[i]
                               , route[i] );
        }

        if ( gpf->ntrkpts != ntrkpts ) {
            LOG( "ntrkpts %d did not match %d\n"
               , gpf->ntrkpts, ntrkpts );
            FAIL_NEG_TEST
        }

        for ( int i = 0; i < gpf->ntrkpts; ++i ) {
            NEG_GpTrkpt_equalsCheck( &gpf->trkpt[i]
                               , &trkpt[i] );
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_NEG_TEST
    }
}

//OLD UTIL

void * NEG_checkMalloc( size_t s ) {
    void * ret = malloc( s );

    if ( ret == NULL ) {
        LOG( "AUTOMARKER MALLOC FAILED\n" );
        abort();
    }

    return ret;
}

FILE * NEG_fopen_check( char * fname ) {
    FILE * fp = fopen( fname, "r" );

    if ( fp == NULL ) {
        LOG( "AUTOMARKER FATAL Could not open file '%s'\n", fname );
        abort();
    }

    return fp;
}

int NEG_GpCoord_equals( const GpCoord * const a
                  , const GpCoord * const b ) {
    return ( FEQUALS( a->lat, b->lat, TOLERANCE ) && FEQUALS( a->lon, b->lon, TOLERANCE ) );
}

void NEG_GpRoute_equalsCheck( const GpRoute * const a
                        , const GpRoute * const b ) {
    int exception;

    try {
        if ( a->number != b->number ) {
            LOG( "GpRoute number '%d' did not match '%d'\n"
               , a->number, b->number );
            FAIL_NEG_TEST
        }

        if ( strcmp( a->comment, b->comment ) != 0 ) {
            LOG( "GpRoute comment '%s' did not match '%s'\n"
               , a->comment, b->comment );
            FAIL_NEG_TEST
        }

        if ( a->npoints != b->npoints ) {
            LOG( "GpRoute npoints '%d' did not match '%d'\n"
               , a->npoints, b->npoints );
            FAIL_NEG_TEST
        }

        for ( int i = 0; i < b->npoints; ++i ) {
            if ( a->leg[i] != b->leg[i]) {
                LOG( "GpRoute leg[%d] '%d' did not match '%d'\n"
                   , i, a->leg[i], b->leg[i]);
                FAIL_NEG_TEST
            }
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_NEG_TEST
    }
}

void NEG_GpTrkpt_check( const GpTrkpt * const t
                  , const GpCoord * const coord
                  , time_t dateTime
                  , _Bool segFlag
                  , char * comment
                  , float speed
                  , double dist
                  , long duration ) {
    int exception;

    if ( !NEG_GpCoord_equals( &t->coord
                        , coord ) ) {
        LOG( "COORD (%lf,%lf) did not match expected (%lf,%lf)\n"
           , t->coord.lat, t->coord.lon
           , coord->lat, coord->lon );
        FAIL_NEG_TEST
    }

    if ( t->dateTime != dateTime ) {
        LOG( "datetime (%ld) did not match expected (%ld)\n"
           , t->dateTime, dateTime );
        FAIL_NEG_TEST
    }

    if ( t->segFlag != segFlag ) {
        LOG( "segFlag (%d) did not match expected (%d)\n"
           , t->segFlag, segFlag );
        FAIL_NEG_TEST
    }

    if ( comment == NULL ) {
        if ( t->comment != comment ) {
            LOG( "comment was not NULL as expected.\n" );
            FAIL_NEG_TEST
        }
    } else {
        try {
            if ( strcmp( t->comment, comment ) != 0 ) {
                LOG( "comment '%s' did not match '%s'.\n"
                   , t->comment, comment );
                FAIL_NEG_TEST
            }
        } catch ( exception ) {
            LOG( "%s\n", Exception_Names[exception] );
            FAIL_NEG_TEST
        }
    }

    //Only check optional fields if segFlag is false
    if ( !segFlag ) {
        if ( !FEQUALS( t->speed, speed, TOLERANCE ) ) {
            LOG( "Speed %f did not match %f\n"
               , t->speed, speed );
            FAIL_NEG_TEST
        }

        if ( !FEQUALS( t->dist, dist, TOLERANCE ) ) {
            LOG( "dist %lf did not match %lf\n"
               , t->dist, dist );
            FAIL_NEG_TEST
        }

        if ( t->duration != duration ) {
            LOG( "duration %ld did not match %ld\n"
               , t->duration, duration );
            FAIL_NEG_TEST
        }
    }
}

void NEG_GpTrkpt_equalsCheck( const GpTrkpt * const a
                        , const GpTrkpt * const b ) {
    NEG_GpTrkpt_check( a
                 , &b->coord
                 , b->dateTime
                 , b->segFlag
                 , b->comment
                 , b->speed
                 , b->dist
                 , b->duration );
}

void NEG_GpTrkpt_init( GpTrkpt * t
                 , int lat
                 , int lon
                 , time_t dateTime
                 , _Bool segFlag
                 , char * comment
                 , float speed
                 , double dist
                 , long duration ) {
    t->coord.lat = lat;
    t->coord.lon = lon;
    t->dateTime = dateTime;
    t->segFlag = segFlag;
    t->comment = comment;
    t->speed = speed;
    t->dist = dist;
    t->duration = duration;
}

void NEG_GpWaypt_check( const GpWaypt * wp
                  , char * id
                  , const GpCoord * coord
                  , char * symbol
                  , char textChoice
                  , short textPlace
                  , char * comment ) {
    int exception;
    try {
        if ( strcmp( id, wp->ID ) != 0 ) {
            LOG( "ID '%s' did not match '%s'\n"
               , wp->ID, id );
            FAIL_NEG_TEST
        }

        if ( strcmp( symbol, wp->symbol ) != 0 ) {
            LOG( "Symbol '%s', did not match '%s'\n"
               , wp->symbol, symbol );
            FAIL_NEG_TEST
        }

        if ( strcmp( comment, wp->comment ) != 0 ) {
            LOG( "Comment '%s' did not match '%s'\n"
               , wp->comment, comment );
            FAIL_NEG_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_NEG_TEST
    }

    if ( !NEG_GpCoord_equals( &wp->coord
                        , coord ) ) {
        LOG( "COORD (%lf,%lf) did not match expected (%lf,%lf)\n"
           , wp->coord.lat, wp->coord.lon
           , coord->lat, coord->lon );
        FAIL_NEG_TEST
    }

    if ( wp->textChoice != textChoice ) {
        LOG( "Text choice '%c' did not match '%c'\n"
           , wp->textChoice, textChoice );
        FAIL_NEG_TEST
    }

    if ( wp->textPlace != textPlace ) {
        LOG( "Text place '%d', did not match %d\n"
           , wp->textPlace, textPlace );
        FAIL_NEG_TEST
    }
}

void NEG_GpWaypt_equalsCheck( const GpWaypt * const a
                        , const GpWaypt * const b ) {
    NEG_GpWaypt_check( a
                 , b->ID
                 , &b->coord
                 , b->symbol
                 , b->textChoice
                 , b->textPlace
                 , b->comment );
}

void NEG_GpWaypt_init( GpWaypt * wp
                 , char * id
                 , double lat
                 , double lon
                 , char * symbol
                 , char textChoice
                 , char textPlace
                 , char * comment ) {
    wp->ID = id;
    wp->coord.lat = lat;
    wp->coord.lon = lon;

    wp->symbol = symbol;
    wp->textChoice = textChoice;
    wp->textPlace = textPlace;
    wp->comment = comment;
}
