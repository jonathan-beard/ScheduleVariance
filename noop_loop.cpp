/**
 * noop_loop.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:04:00 2013
 */
#include "noop_loop.hpp"
#include "system_query.h"

NoOpLoop::NoOpLoop( CmdArgs &args ) : Load( args ),
                                      iterations( 1 ),
                                      service_time( 60 ),
                                      deterministic( true ),
                                      distribution( Deterministic ),
                                      mean_ticks_to_spin( 0 )
{
   /* add specific command line arguments */
   cmd_args.addOption( 
         new Option< int64_t >( iterations,
                                "-iterations",
                                "Iterations to run load" ) );

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
   mean_ticks_to_spin = ( service_time * getStatedCPUFrequency() );
}

void
NoOpLoop::Run()
{
   /* initialize timers */
   const uint64_t tick_to_stop_on( mean_ticks_to_spin + readTimeStampCounter() );
   /* TODO add variable distribution bit here, set outside of loop */
   volatile uint64_t final_ticks( 0 );
   while( tick_to_stop_on >= (final_ticks = readTimeStampCounter() ) )
   {
      __asm__ volatile("\
                           nop"
                           :
                           :
                      );
   }
   /* dump output */
   output << service_time << "," << tick_to_stop_on << "," << final_ticks << "\n";
}

std::ostream&
NoOpLoop::Print( std::ostream &stream )
{
   stream << output.str();
   return( stream );
}
