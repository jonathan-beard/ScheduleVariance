#include "process.hpp"



Process::Process( CmdArgs &cmd ) : cmd_args( cmd )
{
   /* nothing really to do here */
}

Process::~Process()
{
   /* nothing really to do here */
}




void
Process::SetReady()
{
   /* do nothing */
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

bool
Process::EveryoneReady()
{
   return( false );
}


void 
Process::SetContinuing()
{
   /* do nothing */
}

bool 
Process::EveryoneContinuing()
{
   return( false );
}
