// util.c for A2 automarker

#include <ctype.h>

#include "exception.h"
#include "util.h"

void GpFile_compare( GpFile * a
                   , GpFile * b ) {
    GpFile_check( a
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

static void util_infoOutCheckChr( const char * const entity
                                , const char * const gfname
                                , FILE * gfp4
                                , const char * const sfname
                                , FILE * sfp4 ) {
    //Get a non-whitespace char
    char gVal = ' ';
    while ( isspace( gVal ) ) {
        if ( fscanf( gfp4, "%c", &gVal ) != 1 ) {
            LOG( "Failed to read chr from %s\n", gfname );
            abort();
        }
    }

    char sVal = ' ';
    while ( isspace( sVal ) ) {
        if ( fscanf( sfp4, "%c", &sVal ) != 1 ) {
            LOG( "Failed to read chr from %s\n", sfname );
            FAIL_POS_TEST
            sVal = 'x';
        }
    }

    if ( gVal != sVal ) {
        LOG( "'%c' (%d) did not match expected '%c' (%d) for %s output by info\n"
           , sVal, sVal, gVal, gVal, entity );
        FAIL_POS_TEST
    }
/*    else
    {
        LOG("'%c' (%d) did match '%c' (%d) for %s output\n", sVal, sVal,gVal, gVal, entity);
    }
*/
}

static void util_infoOutCheckInt( const char * const entity
                                , const char * const gfname
                                , FILE * gfp4
                                , const char * const sfname
                                , FILE * sfp4 ) {
    int gVal = -1;
    if (fscanf(gfp4, "%d", &gVal) != 1 ) {
        LOG( "Failed to read int from %s (%s)\n", gfname, entity);
       abort();
    }

    int sVal = -1;
    if ( fscanf(sfp4, "%d", &sVal) != 1 ) {
        LOG( "Failed to read int from %s (%s)\n", sfname, entity);
        FAIL_POS_TEST
    }

    if ( gVal != sVal ) {
        LOG( "%d did not match expected %d for number of %s output by info\n"
           , sVal, gVal, entity );
        FAIL_POS_TEST
    }
/*    else
    {
       LOG("%d did match expected %d for number of %s output by info\n", sVal, gVal, entity);
    }
*/
}

static void util_infoOutCheckDbl( const char * const entity
                                , const char * const gfname
                                , FILE * gfp4
                                , const char * const sfname
                                , FILE * sfp4 ) {
    double gVal = -1.0;
    if ( fscanf( gfp4, "%lf", &gVal ) != 1 ) {
        LOG( "Failed to read double from %s\n", gfname );
        abort();
    }

    double sVal = -1.0;
    if ( fscanf( sfp4, "%lf", &sVal ) != 1 ) {
        LOG( "Failed to read double from %s\n", sfname );
        FAIL_POS_TEST
    }

    if ( !FEQUALS( gVal, sVal, TOLERANCE ) ) {
        LOG( "%lf did not match expected %lf for number of %s output by info\n"
           , sVal, gVal, entity );
        FAIL_POS_TEST
    }
/*    else
    {
        LOG("%lf did match expected %lf for number of %s\n", sVal, gVal, entity);
    }
*/
}

static void util_infoOutCheckStr( const char * const entity
                                , const char * const gfname
                                , FILE * gfp4
                                , const char * const sfname
                                , FILE * sfp4 ) {
    char gVal[1024];
    gVal[0] = '\0';
    if ( fscanf( gfp4, "%s", gVal ) != 1 ) {
        LOG( "Failed to read str from %s\n", gfname );
        abort();
    }

    char sVal[1024];
    sVal[0] = '\0';
    if ( fscanf( sfp4, "%s", sVal ) != 1 ) {
        LOG( "Failed to read str from %s\n", sfname );
        FAIL_POS_TEST
    }

    if ( strcmp( gVal, sVal ) != 0 ) {
        LOG( "'%s' did not match expected '%s' for %s output by info\n"
           , sVal, gVal, entity );
        FAIL_POS_TEST
    }
}

void util_InfoOutCheck( const char * const gfname
                      , const char * const sfname
                      , int sorted ) {
    int exception;
    FILE * gfp4 = fopen( gfname, "r" );
    FILE * sfp4 = fopen( sfname, "r" );

    if ( gfp4 == NULL ) {
        LOG( "Could not open a2-suite1-postest4-ginfoout for reading\n" );
        abort();
    }

    if ( sfp4 == NULL ) {
        LOG( "Could not open a2-suite1-postest4-sinfoout for reading\n" );
        FAIL_POS_TEST
    }

    try {
        //Read in number of waypoints
        util_infoOutCheckInt( "waypoints", gfname, gfp4, sfname, sfp4 );
        util_infoOutCheckStr( "'waypoints'", gfname, gfp4, sfname, sfp4 );

        if ( sorted == 1) {
            util_infoOutCheckStr( "'sorted'", gfname, gfp4, sfname, sfp4 );
        }
        else if ( sorted == 0)
        {
            util_infoOutCheckStr( "'not'", gfname, gfp4, sfname, sfp4);
            util_infoOutCheckStr( "'sorted'", gfname, gfp4, sfname, sfp4);
        }

        //Read in number of routes
        util_infoOutCheckInt( "routes", gfname, gfp4, sfname, sfp4 );
        util_infoOutCheckStr( "'routes'", gfname, gfp4, sfname, sfp4 );

        //Read in number of trackpoints
        util_infoOutCheckInt( "trackpoints", gfname, gfp4, sfname, sfp4 );
        util_infoOutCheckStr( "'trackpoints'", gfname, gfp4, sfname, sfp4 );

        //Read in number of tracks
        util_infoOutCheckInt( "tracks", gfname, gfp4, sfname, sfp4 );
        util_infoOutCheckStr( "'tracks'", gfname, gfp4, sfname, sfp4 );

        //Skip "Extent:"
        util_infoOutCheckStr( "'extent'", gfname, gfp4, sfname, sfp4 );

        //Read SW
        util_infoOutCheckStr( "SW extent", gfname, gfp4, sfname, sfp4 );
        util_infoOutCheckChr( "SW longitude", gfname, gfp4, sfname, sfp4 );
        util_infoOutCheckDbl( "SW longitude", gfname, gfp4, sfname, sfp4 );

        util_infoOutCheckChr( "SW latitude", gfname, gfp4, sfname, sfp4 );
        util_infoOutCheckDbl( "SW latitude", gfname, gfp4, sfname, sfp4 );

        //Skip "to"
        util_infoOutCheckStr( "'to'", gfname, gfp4, sfname, sfp4 );

        //Read NE
        util_infoOutCheckStr( "NE extent", gfname, gfp4, sfname, sfp4 );
        util_infoOutCheckChr( "NE longitude", gfname, gfp4, sfname, sfp4 );
        util_infoOutCheckDbl( "NE longitude", gfname, gfp4, sfname, sfp4 );

        util_infoOutCheckChr( "NE latitude", gfname, gfp4, sfname, sfp4 );
        util_infoOutCheckDbl( "NE latitude", gfname, gfp4, sfname, sfp4 );
    } catch ( exception ) {
        LOG( "%s\n", Exception_Names[exception] );
        FAIL_POS_TEST
    }
}


// A1's util.c

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
