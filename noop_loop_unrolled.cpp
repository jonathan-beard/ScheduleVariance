#include <sys/types.h>
#include <unistd.h>

#include "system_query.h"

#include "process.hpp"
#include "noop_loop_unrolled.hpp"

NoOpLoopUnrolled::NoOpLoopUnrolled( CmdArgs &args ) : Load( args )
{
   frequency = getStatedCPUFrequency();
}

void 
NoOpLoopUnrolled::Run( Process & p )
{
   int64_t it_index( 0 );
   const auto iterations( GetNumIterations() );
   for(; it_index < iterations; it_index++ )
   {
      p.SetStatus( it_index , ProcessStatus::RUNNING );
      /* A bit hacky but it'll work */
#include "noop_loop_unrolled_load.cpp"         
      p.SetStatus( it_index, ProcessStatus::WAITING );
      while( ! p.IsEveryoneSetTo( it_index, ProcessStatus::WAITING ) )
      {
         continue;
      }
      NopLoopUnrolled::Data d( theNoopCount,
                               frequency,
                               cyclesBefore,
                               cyclesAfter,
                               diff );
      p.SetData( (void*) &d ,
                 it_index );
      p.SetContinuing( it_index, ProcessStatus::CONTINUING );
      while( ! p.IsEveryoneSetTo( it_index, ProcessStatus::CONTINUING ) )
      {
         continue;
      }
   }
   p.SetStatus( it_index, ProcessStatus::DONE );
   while( ! p.IsEveryoneSetTo( it_index, ProcessStatus::DONE ) )
   {
      continue;
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
