/**
 * timespec.hpp - 
 * @author: Jonathan Beard
 * @version: Tue Aug 27 08:34:29 2013
 */
#ifndef _TIMESPEC_HPP_
#define _TIMESPEC_HPP_  1

#include <time.h>
#include <string.h>

struct TimeSpec
{
   TimeSpec()
   {
      memset( &time, 0x0, sizeof( struct timespec ) );
   }
   
   TimeSpec( double seconds ) : TimeSpec()
   {
      time.tv_sec += (time_t) seconds;
      time.tv_nsec += (long) (( seconds - time.tv_sec ) * 1e9 );
   }

   TimeSpec( const TimeSpec &t )
   
      time.tv_sec = t.time.tv_sec;
      time.tv_nsec = t.time.tv_nsec;
   }

   ~TimeSpec()
   {
      memset( &time, 0x0, sizeof( struct timespec ) );
   }


   TimeSpec& operator += ( TimeSpec &t )
   {
      time.tv_sec += t.time.tv_sec;
      const long int temp_nsec( time.tv_nsec + t.time.tv_nsec );
      if( temp_nsec < 0 ) /* overflow */
      {
         int64_t remainder( INT64_MAX );
         remainder += temp_nsec;
         remainder += 2;
         time.tv_sec += 1;
         time.tv_nsec = remainder;
      }
      return( *this );
   }

   struct timespec time;
};
#endif /* END _TIMESPEC_HPP_ */
