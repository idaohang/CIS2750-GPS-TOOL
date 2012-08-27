#include <stdio.h>
#include <stdlib.h>

#include "gputil.h"
#include "mark.h"

#define TOLERANCE 0.0001

void * checkMalloc( size_t s );
FILE * fopen_check( char * fname );
const char * GpError_toString( GpError e );
int GpCoord_equals( const GpCoord * const a
                  , const GpCoord * const b );
void GpRoute_equalsCheck( const GpRoute * const a
                        , const GpRoute * const b );
void GpTrkpt_check( const GpTrkpt * const t
                  , const GpCoord * const coord
                  , time_t dateTime
                  , _Bool segFlag
                  , char * comment
                  , float speed
                  , double dist
                  , long duration );
void GpTrkpt_equalsCheck( const GpTrkpt * const a
                        , const GpTrkpt * const b );
void GpTrkpt_init( GpTrkpt * t
                 , double lat
                 , double lon
                 , time_t dateTime
                 , _Bool segFlag
                 , char * comment
                 , float speed
                 , double dist
                 , long duration );
void GpWaypt_check( const GpWaypt * wp
                  , char * id
                  , const GpCoord * coord
                  , char * symbol
                  , char textChoice
                  , short textPlace
                  , char * comment );
void GpWaypt_equalsCheck( const GpWaypt * const a
                        , const GpWaypt * const b );
void GpWaypt_init( GpWaypt * wp
                 , char * id
                 , double lat
                 , double lon
                 , char * symbol
                 , char textChoice
                 , char textPlace
                 , char * comment );
