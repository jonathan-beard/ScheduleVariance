#include <sys/types.h>
#include <unistd.h>

#include "system_query.h"

#include "process.hpp"
#include "gatekeeper.hpp"
#include "noop_loop_unrolled.hpp"

NoOpLoopUnrolled::NoOpLoopUnrolled( CmdArgs &args ) : Load( args )
{
   frequency = getStatedCPUFrequency();
}

NoOpLoopUnrolled::~NoOpLoopUnrolled()
{
   /* nothing really to do */
}

void 
NoOpLoopUnrolled::Run( Process &p, GateKeeper &g )
{
   for(;
         p.get_curr_iteration() < p.get_iterations();
         p.increment_curr_iteration() )
   {
      g.WaitForGate( "Running" );
      /* A bit hacky but it'll work */
#include "noop_loop_unrolled_load.cpp"         
      g.WaitForGate( "Storing" );
      /**
       * Note: theNoopCount, cylcesbefore, cyclesafter, diff are 
       * defined in the autogenerated load (noop_loop_unrolled_load.cpp) 
       * which is generated by gen_noop_load.pl during compilation.
       */
      NoOpLoopUnrolled::Data d( theNoopCount /* # of noops in loop */,
                        frequency    /* what is says */,
                        cyclesbefore /* # cycles read before loop */,
                        cyclesafter  /* # cycles read after loop */,
                        diff         /* delta between before, after */ );
      p.SetData( (void*) &d );
      g.WaitForGate( "ReadyToStart" );
      g.ResetGate( "Running" );
      g.ResetGate( "Storing" );
      g.ResetGate( "ReadyToStart" );
   }
}


std::ostream&
NoOpLoopUnrolled::PrintHeader( std::ostream &stream )
{
   NoOpLoopUnrolled::Data::PrintHeader( stream );
   return( stream );
}

std::ostream&
NoOpLoopUnrolled::PrintData( std::ostream &stream, void *d )
{
   NoOpLoopUnrolled::Data *d_ptr( reinterpret_cast< 
                                    NoOpLoopUnrolled::Data* >( d ) );
   NoOpLoopUnrolled::Data::PrintData( stream, *d_ptr );                               return( stream );
}
