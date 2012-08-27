// negutil.h

/* This file is a horrible hack due to a desperate lack of time.  Originally, all tests were POS
 * tests, but the ONE test (Test merge without fileB in existence so that output is just fileA)
 * is a neg test and needs to use these functions.  So, the fastest way is to copy and paste,
 * search and replace.  I feel ashamed.
 *
 */

#define TOLERANCE 0.0001

#include "gputil.h"

void NEG_GpFile_compare( GpFile * a
                       , GpFile * b );
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
                     , GpTrkpt * trkpt );

//OLD UTIL
void * NEG_checkMalloc( size_t s );
FILE * NEG_fopen_check( char * fname );
int NEG_GpCoord_equals( const GpCoord * const a
                      , const GpCoord * const b );
void NEG_GpRoute_equalsCheck( const GpRoute * const a
                            , const GpRoute * const b );
void NEG_GpTrkpt_check( const GpTrkpt * const t
                      , const GpCoord * const coord
                      , time_t dateTime
                      , _Bool segFlag
                      , char * comment
                      , float speed
                      , double dist
                      , long duration );
void NEG_GpTrkpt_equalsCheck( const GpTrkpt * const a
                            , const GpTrkpt * const b );
void NEG_GpTrkpt_init( GpTrkpt * t
                     , int lat
                     , int lon
                     , time_t dateTime
                     , _Bool segFlag
                     , char * comment
                     , float speed
                     , double dist
                     , long duration );
void NEG_GpWaypt_check( const GpWaypt * wp
                      , char * id
                      , const GpCoord * coord
                      , char * symbol
                      , char textChoice
                      , short textPlace
                      , char * comment );
void NEG_GpWaypt_equalsCheck( const GpWaypt * const a
                            , const GpWaypt * const b );
void NEG_GpWaypt_init( GpWaypt * wp
                     , char * id
                     , double lat
                     , double lon
                     , char * symbol
                     , char textChoice
                     , char textPlace
                     , char * comment );
