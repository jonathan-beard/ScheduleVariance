#include "load.hpp"


Load::Load( CmdArgs &args ) : cmd_args( args ),
                              iterations( 1 ) /* run the puppy once */
{
   cmd_args.addOption( 
      new Option< int64_t >( iterations,
                             "-iterations",
                             "Iterations to run  load" ) );
}

Load::~Load()
{
   /* nothing to do here */
}

int64_t
Load::GetNumIterations()
{
   return( iterations );
}
