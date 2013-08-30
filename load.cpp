#include "load.hpp"
#include "process.hpp"
#include "gatekeeper.hpp"

Load::Load( CmdArgs &args ) : cmd_args( args )
{
}

Load::~Load()
{
   /* nothing to do here */
}

void
Load::Run( Process &p, GateKeeper &g )
{
   int64_t it( 0 );
   for(; 
      ( it = p.get_curr_iteration() )< p.get_iterations(); 
      p.increment_curr_iteration() )
   {
      RunLoad( p, g, it );
   }
}
