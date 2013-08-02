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
Process::AllReady()
{
   return( true );
}

bool
Process::Reset()
{
   return( false );
}

bool
Process::ResetAll()
{
   return( false );
}

void 
Process::SetRunning()
{
   /* empty */
}

void
Process::SetDone()
{
   /* empty */
}

void 
Process::SetWaiting()
{
   /* empty */
}

bool
Process::EveryoneDone()
{
   return( false );
}

bool
Process::EveryoneWaiting()
{
   return( false );
}
