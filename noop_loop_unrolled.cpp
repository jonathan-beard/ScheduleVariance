#include <sys/types.h>
#include <unistd.h>

#include "system_query.h"
#include "calibrate.hpp"
#include "process.hpp"
#include "gatekeeper.hpp"
#include "noop_loop_unrolled.hpp"

NoOpLoopUnrolled::NoOpLoopUnrolled( CmdArgs &args ) : Load( args ),
                                                      seconds( 0.0 ),
                                                      service_time( 1.0 )
{
   frequency = getStatedCPUFrequency();
#include "noop_loop_unrolled_load_seconds.hpp"
   /* defined in above include */
   seconds = expectedSeconds;
   cmd_args.addOption( new Option< double >( service_time,
                                             "-mu",
                                             "Service rate for load" ) );
}

NoOpLoopUnrolled::~NoOpLoopUnrolled()
{
   /* nothing really to do */
}

void 
NoOpLoopUnrolled::RunLoad( Process &p, GateKeeper &g, int64_t i )
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

bool 
NoOpLoopUnrolled::AllSet()
{
   /* check to see if we need calibration */
   if( seconds != service_time )
   {
      /* need to call calibrate here */
      Calibrate( service_time, cmd_args.getOriginalArguments() );
      /* won't return from this one, however
       * some compilers require a return here */
      return( false );
   }
   return( true );
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
