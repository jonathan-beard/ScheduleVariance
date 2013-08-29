/**
 * gatekeeper.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Aug 29 09:17:03 2013
 */

#include "gatekeeper.hpp"
#include "gate.hpp"

#include <algorithm>

GateKeeper::GateKeeper( const int64_t number_of_processors )
   :  num_processors( number_of_processors )
{
  /* nothing really to do */
}

GateKeeper::~GateKeeper()
{
   for_each( gates.begin(), gates.end(),
      []( std::pair< const std::string, Gate > &g )
      {
         g.second.Destroy();
      });
}

bool
GateKeeper::RegisterGate( const std::string gate_name )
{
   if( GateExists( gate_name ) ) return( false );
   gates.insert( std::make_pair( gate_name, Gate( num_processors ) ) );
   return( true );
}

bool
GateKeeper::WaitForGate( const std::string gate_name )
{
   if(! GateExists( gate_name ) ) return( false );
   const auto barrier( gates.find( gate_name ) );
   barrier->second.Barrier();
   return( true );
}

bool
GateKeeper::ResetGate( const std::string gate_name )
{
   if( ! GateExists( gate_name ) ) return( false );
   const auto gate( gates.find( gate_name ) );
   gate->second.Reset();
   return( true );
}

void
GateKeeper::ResetAllGates()
{
   for_each( gates.begin(), gates.end(),
      []( std::pair< const std::string, Gate > &g )
         {
            g.second.Reset();
         } );
}

void
GateKeeper::HandleFork()
{
   for_each( gates.begin(), gates.end(),
      []( std::pair< const std::string, Gate > &g )
         {
            /* reopen semaphores and shm for each */
            g.second.ReOpen();
         } );
}

bool
GateKeeper::GateExists( const std::string name )
{
   const auto found( gates.find( name ) );
   if( found != gates.cend() )
   {
      return( true );
   }else{
      return( false );
   }
}
