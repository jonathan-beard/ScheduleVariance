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

      /* initialize timers */
      /* TODO add variable distribution bit here, set outside of loop */
      mean_ticks_to_spin = (uint64_t) ( (double) service_time * 
                                        (double) frequency );
      /* readTimeStampCounter will only work on x86 at the moment */
      const uint64_t tick_to_stop_on( 
            mean_ticks_to_spin + readTimeStampCounter() );
      volatile uint64_t final_tick( 0 );
      while( tick_to_stop_on >= (final_tick = readTimeStampCounter() ) )
      {
         __asm__ volatile("\
                              nop"
                              :
                              :
                         );
      }
      /* done with data, drop the load to process */
      NoOpLoopClock::Data d( distribution,
                        service_time,
                        frequency,
                        mean_ticks_to_spin,
                        tick_to_stop_on,
                        final_tick );
      p.SetData( (void*)&d );
}
