#include "process.hpp"



Process::Process( CmdArgs &cmd ) : cmd_args( cmd )
{
   /* nothing really to do here */
}

Process::~Process()
{
   /* nothing really to do here */
}

bool
Process::Ready()
{
   return( true );
}

bool
Process::Reset()
{
   return( false );
}
