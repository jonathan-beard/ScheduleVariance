/**
 * noop_loop.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:04:00 2013
 */
#include "noop_loop.hpp"
#include "process.hpp"

#include <sys/types.h>
#include <unistd.h>

#include "system_query.h"

NoOpLoop::NoOpLoop( CmdArgs &args ) : Load( args ),
                                      service_time( 60 ),
                                      deterministic( true ),
                                      distribution( Deterministic ),
                                      mean_ticks_to_spin( 0 ),
                                      frequency( 0 )
{
   /* add specific command line arguments */
   cmd_args.addOption(
         new Option< double >( service_time,
                               "-mu",
                               "Service rate for load" ) );
   cmd_args.addOption(
         new Option< bool >( deterministic,
                          "-deterministic",
                          "Set false if you want an alternate distribution" ) );

   cmd_args.addOption(
         new Option< int64_t >( distribution,
                                "-distribution",
 "Set the distribution, string of: (Deterministic,Uniform,Exponential,HyperExponential" ) );

   /* set mean ticks to spin */
   frequency = getStatedCPUFrequency();
}

NoOpLoop::~NoOpLoop()
{
   /* nothing to do */
}

std::ostream&
NoOpLoop::PrintHeader( std::ostream &stream )
{
   NoOpLoop::Data::PrintHeader( stream );
   return( stream );
}

std::ostream&
NoOpLoop::PrintData( std::ostream &stream, void *d )
{
   NoOpLoop::Data *d_ptr( reinterpret_cast< NoOpLoop::Data* >( d ) );
   NoOpLoop::Data::PrintData( stream, *d_ptr );
   return( stream );
}

void
NoOpLoop::Run( Process &p )
{  
   mean_ticks_to_spin = (uint64_t) ( (double) service_time * 
                                     (double) frequency );

   int64_t it_index( 0 );
   const auto iterations( GetNumIterations() );
   for(; it_index < iterations; it_index++ )
   {  
      /* initialize timers */
      /* readTimeStampCounter will only work on x86 at the moment */
      const uint64_t tick_to_stop_on( 
            mean_ticks_to_spin + readTimeStampCounter() );
      /* TODO add variable distribution bit here, set outside of loop */
      volatile uint64_t final_tick( 0 );
      p.SetRunning( it_index );
      while( tick_to_stop_on >= (final_tick = readTimeStampCounter() ) )
      {
         __asm__ volatile("\
                              nop"
                              :
                              :
                         );
      }
      p.SetWaiting( it_index );
      /* wait for everyone to be waiting */
      while( ! p.EveryoneWaiting( it_index ) )
      {
         continue;
      }
      /* now that everyone is done, store the data */
      NoOpLoop::Data d( distribution,
                        service_time,
                        frequency,
                        mean_ticks_to_spin,
                        tick_to_stop_on,
                        final_tick );
      p.SetData( (void*)&d,
                 it_index );
      p.SetContinuing( it_index );
      /* wait for store ops to complete */
      while( ! p.EveryoneContinuing( it_index ) )
      {
         continue;
      }
   }
   p.SetDone( it_index );
   while( ! p.EveryoneDone( it_index ) )
   {
      continue;
   }
   /* wait for everyone else to finish then exit */
}

size_t 
NoOpLoop::GetNumIterations()
{
   return( (size_t) iterations );
}
