/**
 * noop_loop.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:04:00 2013
 */
#include "noop_loop.hpp"
#include "process.hpp"

#include "system_query.h"

NoOpLoop::NoOpLoop( CmdArgs &args ) : Load( args ),
                                      iterations( 1 ),
                                      service_time( 60 ),
                                      deterministic( true ),
                                      distribution( Deterministic ),
                                      mean_ticks_to_spin( 0 ),
                                      frequency( 0 )
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
   frequency = getStatedCPUFrequency();
   mean_ticks_to_spin = ( service_time * frequency );
}

NoOpLoop::~NoOpLoop()
{
   /* nothing to do */
}

void
NoOpLoop::Run( Process &p )
{  
   for( int64_t it_index( 0 ); it_index < iterations; it_index++ )
   {  
      p.SetRunning();
      /* initialize timers */
      const uint64_t tick_to_stop_on( 
            mean_ticks_to_spin + readTimeStampCounter() );
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
      p.SetWaiting();
      /* wait for everyone to be waiting */
      while( ! p.EveryoneWaiting() )
      {
         continue;
      }
   }
   p.SetDone();
}
size_t 
NoOpLoop::GetDataStructSize()
{
   /* gotta know in advance what your output will be */
   return( sizeof( double ) + sizeof( uint64_t ) + sizeof( uint64_t ) );
}

size_t 
NoOpLoop::GetNumIterations()
{

}

std::ostream& 
NoOpLoop::ReadData( std::ostream &stream,
                    char *ptr )
{                    
}
