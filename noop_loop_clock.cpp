/**
 * noop_loop_clock.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:04:00 2013
 */
#include "noop_loop_clock.hpp"
#include "process.hpp"

#include <sys/types.h>
#include <unistd.h>

#include "system_query.h"


const char *NoOpLoopClock::DistributionString[N] =
   {
      [Deterministic] = "Deterministic",
      [Uniform]       = "Uniform",
      [Exponential]   = "Exponential",
      [HyperExponential] = "HyperExponential"
   };


NoOpLoopClock::NoOpLoopClock( CmdArgs &args ) : Load( args ),
                                      service_time( 60 ),
                                      distribution( Deterministic ),
                                      resolution( 0.0 )
{
   /* add specific command line arguments */
   cmd_args.addOption(
         new Option< double >( service_time,
                               "-mu",
                               "Service rate for load" ) );
   cmd_args.addOption(
         new Option< Distribution >( distribution,
                                     "-distribution",
 "Set the distribution, string of: (Deterministic,Uniform,Exponential,HyperExponential",
         [](const char *str, bool &success)
         {  
            success = true;
            if( str == nullptr )
            {
               success = false;
               return( Deterministic );
            }
            else if( strcmp( str, DistributionString[ Deterministic ] ) == 0 )
            {
               return( Deterministic );
            }
            else if( strcmp( str, DistributionString[ Uniform ] ) == 0 )
            {
               return( Uniform );
            }  
            else if( strcmp( str, DistributionString[ Exponential] ) == 0 )
            {
               return( Exponential );
            }
            else if( strcmp( str, DistributionString[ HyperExponential ] ) == 0 )
            {
               return( HyperExponential );
            }
            else
            {
               success = false;
               return( Deterministic );
            }
         },
         []( Distribution &d )
         {
            const std::string out( DistributionString[ d ] );
            return( out );
         } ) );
   struct timespec res;
   memset( &res, 0x0, sizeof( struct timespec ) );
   errno = 0;
   if( clock_getres( CLOCK_REALTIME , &res ) != 0 )
   {
      perror( "Failed to get clock resolution." );
      /* TODO maybe not the best behavior */
      exit( EXIT_FAILURE );
   }
   /* now we have the resolution */
   resolution += res.tv_sec;
   resolution += (res.tv_nsec / 1e9);

   /* set run time, at least initially for deterministic case */
   memset( &loadtime, 0x0, sizeof( struct timespec ) );
   loadtime.tv_sec = (time_t) service_time;
   loadtime.tv_nsec = (long)  ((service_time - loadtime.tv_sec) * 1e9);
}

NoOpLoopClock::~NoOpLoopClock()
{
   /* nothing to do */
}

std::ostream&
NoOpLoopClock::PrintHeader( std::ostream &stream )
{
   NoOpLoopClock::Data::PrintHeader( stream );
   return( stream );
}

std::ostream&
NoOpLoopClock::PrintData( std::ostream &stream, void *d )
{
   NoOpLoopClock::Data *d_ptr( reinterpret_cast< NoOpLoopClock::Data* >( d ) );
   NoOpLoopClock::Data::PrintData( stream, *d_ptr );
   return( stream );
}

void
NoOpLoopClock::Run( Process &p )
{  
      struct timespec end;
      clock_gettime( CLOCK_REALTIME, &end );
      end.tv_sec += loadtime.tv_sec;
      const long temp( end.tv_nsec + loadtime.tv_nsec );
      if( temp <= load_time.tv_nsec )
      {
         end.tv_sec += 1;
      }
      struct timespec curr;
      curr.tv_sec = 0;
      curr.tv_nsec = 0;
      while( true )
      {
         clock_gettime( CLOCK_REALTIME, &curr );
         if( curr >= end ) break;
         __asm__ volatile("\
                              nop"
                              :
                              :
                         );
      }
      
      /* done with data, drop the load to process */
      double delta( 0.0 );
      delta += (  curr.tv_sec - end.tv_sec );
      delta += (  curr.tv_nsec - end.tv_nsec ) / 1e9;

      NoOpLoopClock::Data d( distribution,
                             service_time,
                             delta,
                             resolution );
      p.SetData( (void*)&d );
}
