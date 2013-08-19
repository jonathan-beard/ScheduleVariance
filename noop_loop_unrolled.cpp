#include "noop_loop_unrolled.hpp"

NoOpLoopUnrolled::NoOpLoopUnrolled( CmdArgs &args ) : NoOpLoop( args )
{
   /* nothing to do here */
}

void 
NoOpLoopUnrolled::Run( Process & p )
{
   int64_t it_index( 0 );
   for(; it_index < iterations; it_index++ )
   {
      p.SetRunning( it_index );
      /* A bit hacky but it'll work */
#include "noop_loop_unrolled_load.cpp"         
      p.SetWaiting( it_index );
      while( ! p.Everyonewaiting( it_index ) )
      {
         continue;
      }
      NopLoopUnrolled::Data d;
      d.distribution = distribution;
      d.service_time = service_time;
      d.frequency    = frequency;
      d.mean_ticks_to_spin = mean_ticks_to_spin;
      d.target_stop_tick   = tick_to_stop_on;
      d.actual_stop_tick   = final_tick;
      p.SetData( (void*) &d ,
                 it_index );
      p.SetContinuing( it_index );
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
}


std::ostream&
NoOpLoopUnrolled::PrintHeader( std::ostream &stream )
{

}
