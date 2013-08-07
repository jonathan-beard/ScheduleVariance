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
}

NoOpLoop::~NoOpLoop()
{
   /* nothing to do */
}

std::ostream&
NoOpLoop::PrintHeader( std::ostream &stream )
{
   stream << "Load" << "," << "Distribution" << "," << "ServiceTime";
   stream << "," << "Frequency" << "," << "TicksToSpin" << ",";
   stream << "TargetStopTick" << "," << "ActualStopTick" << "," << "TickDelta";
   return( stream );
}

std::ostream&
NoOpLoop::PrintData( std::ostream &stream, void *d )
{
   NoOpLoop::Data *d_ptr( reinterpret_cast< NoOpLoop::Data* >( d ) );
   stream << d_ptr->load_name << "," << d_ptr->distribution << ",";
   stream << d_ptr->service_time << "," << d_ptr->frequency << ",";
   stream << d_ptr->mean_ticks_to_spin << "," << d_ptr->target_stop_tick << ",";
   stream << d_ptr->actual_stop_tick << ",";
   stream << ( d_ptr->actual_stop_tick - d_ptr->target_stop_tick );
   return( stream );
}

void
NoOpLoop::Run( Process &p )
{  
   mean_ticks_to_spin = (uint64_t) ( (double) service_time * 
                                     (double) frequency );

   for( int64_t it_index( 0 ); it_index < iterations; it_index++ )
   {  
      
      /* initialize timers */
      /* readTimeStampCounter will only work on x86 at the moment */
      const uint64_t tick_to_stop_on( 
            mean_ticks_to_spin + readTimeStampCounter() );
      /* TODO add variable distribution bit here, set outside of loop */
      volatile uint64_t final_tick( 0 );
      p.SetRunning();
      while( tick_to_stop_on >= (final_tick = readTimeStampCounter() ) )
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
      /* now that everyone is done, store the data */
      NoOpLoop::Data d;
      d.distribution = distribution;
      d.service_time = service_time;
      d.frequency    = frequency;
      d.mean_ticks_to_spin = mean_ticks_to_spin;
      d.target_stop_tick    = tick_to_stop_on;
      d.actual_stop_tick    = final_tick;
      p.SetData( (void*)&d,
                 it_index );
      p.SetDone();
      /* wait for store ops to complete */
      while( ! p.EveryoneDone() )
      {
         continue;
      }
   }
   p.SetDone();
}

size_t 
NoOpLoop::GetNumIterations()
{
   return( (size_t) iterations );
}
