#include "process.hpp"



Process::Process( CmdArgs &cmd ) : cmd_args( cmd ),
                                   iterations( 1 ),
                                   curr_iteration( 0 )
{
   cmd.addOption( new Option< int64_t >( iterations, 
                              "-iterations",
                              "Number of iterations to run load" ) );
}

Process::~Process()
{
   /* nothing really to do here */
}


int64_t
Process::get_iterations()
{
   return( iterations );
}

int64_t
Process::get_curr_iteration()
{
   return( curr_iteration );
}

void
Process::increment_curr_iteration()
{
   curr_iteration++;
}
