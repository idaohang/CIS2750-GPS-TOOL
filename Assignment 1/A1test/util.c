#include <string.h>

#include "exception.h"
#include "util.h"

void * checkMalloc( size_t s ) {
    void * ret = malloc( s );

    if ( ret == NULL ) {
        LOG( "AUTOMARKER MALLOC FAILED\n" );
        abort();
    }

    return ret;
}

FILE * fopen_check( char * fname ) {
    FILE * fp = fopen( fname, "r" );

    if ( fp == NULL ) {
        LOG( "AUTOMARKER FATAL Could not open file '%s'\n", fname );
        abort();
    }

    return fp;
}

const char * GpError_toString( GpError e ) {
    switch ( e ) {
        case OK:
            return "OK";
        case IOERR:
            return "IOERR";
        case UNKREC:
            return "UNKREC";
        case BADSEP:
            return "BADSEP";
        case FILTYP:
            return "FILTYP";
        case DATUM:
            return "DATUM";
        case COORD:
            return "COORD";
        case NOFORM:
            return "NOFORM";
        case FIELD:
            return "FIELD";
        case VALUE:
            return "VALUE";
        case DUPRT:
            return "DUPRT";
        case UNKWPT:
            return "UNKWPT";
    }

    return "UNKNOWNGPERROR";
}

int GpCoord_equals( const GpCoord * const a
                  , const GpCoord * const b ) {
    return ( FEQUALS( a->lat, b->lat, TOLERANCE ) && FEQUALS( a->lon, b->lon, TOLERANCE ) );
}

void GpRoute_equalsCheck( const GpRoute * const a
                        , const GpRoute * const b ) {
    int exception;

    try {
        if ( a->number != b->number ) {
            LOG( "GpRoute number '%d' did not match '%d'\n"
               , a->number, b->number );
            FAIL_POS_TEST
        }

        if ( strcmp( a->comment, b->comment ) != 0 ) {
            LOG( "GpRoute comment '%s' did not match '%s'\n"
               , a->comment, b->comment );
            FAIL_POS_TEST
        }

        if ( a->npoints != b->npoints ) {
            LOG( "GpRoute npoints '%d' did not match '%d'\n"
               , a->npoints, b->npoints );
            FAIL_POS_TEST
        }

        for ( int i = 0; i < b->npoints; ++i ) {
            if ( a->leg[i] != b->leg[i]) {
                LOG( "GpRoute leg[%d] '%d' did not match '%d'\n"
                   , i, a->leg[i], b->leg[i]);
                FAIL_POS_TEST
            }
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }
}

void GpTrkpt_check( const GpTrkpt * const t
                  , const GpCoord * const coord
                  , time_t dateTime
                  , _Bool segFlag
                  , char * comment
                  , float speed
                  , double dist
                  , long duration ) {
    int exception;

    if ( !GpCoord_equals( &t->coord
                        , coord ) ) {
        LOG( "COORD (%lf,%lf) did not match expected (%lf,%lf)\n"
           , t->coord.lat, t->coord.lon
           , coord->lat, coord->lon );
        FAIL_POS_TEST
    }

    if ( t->dateTime != dateTime ) {
        LOG( "datetime (%ld) did not match expected (%ld)\n"
           , t->dateTime, dateTime );
        FAIL_POS_TEST
    }

    if ( t->segFlag != segFlag ) {
        LOG( "segFlag (%d) did not match expected (%d)\n"
           , t->segFlag, segFlag );
        FAIL_POS_TEST
    }

    if ( comment == NULL ) {
        if ( t->comment != comment ) {
            LOG( "comment was not NULL as expected.\n" );
            FAIL_POS_TEST
        }
    } else {
        try {
            if ( strcmp( t->comment, comment ) != 0 ) {
                LOG( "comment '%s' did not match '%s'.\n"
                   , t->comment, comment );
                FAIL_POS_TEST
            }
        } catch ( exception ) {
            LOG( "%s\n", Exception_Names[exception] );
            FAIL_POS_TEST
        }
    }

    //Only check optional fields if segFlag is false
    if ( !segFlag ) {
        if ( !FEQUALS( t->speed, speed, TOLERANCE ) ) {
            LOG( "Speed %f did not match %f\n"
               , t->speed, speed );
            FAIL_POS_TEST
        }

        if ( !FEQUALS( t->dist, dist, TOLERANCE ) ) {
            LOG( "dist %lf did not match %lf\n"
               , t->dist, dist );
            FAIL_POS_TEST
        }

        if ( t->duration != duration ) {
            LOG( "duration %ld did not match %ld\n"
               , t->duration, duration );
            FAIL_POS_TEST
        }
    }
}

void GpTrkpt_equalsCheck( const GpTrkpt * const a
                        , const GpTrkpt * const b ) {
    GpTrkpt_check( a
                 , &b->coord
                 , b->dateTime
                 , b->segFlag
                 , b->comment
                 , b->speed
                 , b->dist
                 , b->duration );
}

void GpTrkpt_init( GpTrkpt * t
                 , double lat
                 , double lon
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

void GpWaypt_check( const GpWaypt * wp
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
            FAIL_POS_TEST
        }

        if ( strcmp( symbol, wp->symbol ) != 0 ) {
            LOG( "Symbol '%s', did not match '%s'\n"
               , wp->symbol, symbol );
            FAIL_POS_TEST
        }

        if ( strcmp( comment, wp->comment ) != 0 ) {
            LOG( "Comment '%s' did not match '%s'\n"
               , wp->comment, comment );
            FAIL_POS_TEST
        }
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }

    if ( !GpCoord_equals( &wp->coord
                        , coord ) ) {
        LOG( "COORD (%lf,%lf) did not match expected (%lf,%lf)\n"
           , wp->coord.lat, wp->coord.lon
           , coord->lat, coord->lon );
        FAIL_POS_TEST
    }

    if ( wp->textChoice != textChoice ) {
        LOG( "Text choice '%c' did not match '%c'\n"
           , wp->textChoice, textChoice );
        FAIL_POS_TEST
    }

    if ( wp->textPlace != textPlace ) {
        LOG( "Text place '%d', did not match %d\n"
           , wp->textPlace, textPlace );
        FAIL_POS_TEST
    }
}

void GpWaypt_equalsCheck( const GpWaypt * const a
                        , const GpWaypt * const b ) {
    GpWaypt_check( a
                 , b->ID
                 , &b->coord
                 , b->symbol
                 , b->textChoice
                 , b->textPlace
                 , b->comment );
}

void GpWaypt_init( GpWaypt * wp
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
