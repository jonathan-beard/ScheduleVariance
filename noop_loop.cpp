/**
 * noop_loop.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:04:00 2013
 */
#include "noop_loop.hpp"
#include "process.hpp"
#include "gatekeeper.hpp"

#include <sys/types.h>
#include <unistd.h>

#include "system_query.h"

const char *NoOpLoop::DistributionString[N] =
   {
      [Deterministic] = "Deterministic",
      [Uniform]       = "Uniform",
      [Exponential]   = "Exponential",
      [HyperExponential] = "HyperExponential"
   };


NoOpLoop::NoOpLoop( CmdArgs &args ) : Load( args ),
                                      service_time( 60 ),
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
NoOpLoop::RunLoad( Process &p , GateKeeper &g, int64_t i)
{  
      mean_ticks_to_spin = (uint64_t) ( (double) service_time * 
                                        (double) frequency );
      /* readTimeStampCounter will only work on x86 at the moment */
      const uint64_t tick_to_stop_on( 
            mean_ticks_to_spin + readTimeStampCounter() );
      volatile uint64_t final_tick( 0 );
      g.WaitForGate( "Running" );
      while( tick_to_stop_on >= (final_tick = readTimeStampCounter() ) )
      {
         __asm__ volatile("\
                              nop"
                              :
                              :
                         );
      }
      g.WaitForGate( "Storing" );
      /* done with data, drop the load to process */
      NoOpLoop::Data d( distribution,
                        service_time,
                        frequency,
                        mean_ticks_to_spin,
                        tick_to_stop_on,
                        final_tick );
      p.SetData( (void*)&d );
      g.WaitForGate( "ReadyToStart" );
      g.ResetGate( "Running" );
      g.ResetGate( "Storing" );
      g.ResetGate( "ReadyToStart" );
}
