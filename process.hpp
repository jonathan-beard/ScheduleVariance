/**
 * process.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 16:55:21 2013
 */
#ifndef _PROCESS_HPP_
#define _PROCESS_HPP_  1

#include <ostream>
#include "command_arguments.h"
#include "command_option_base.hpp"
#include "command_option_single.tcc"
#include "command_option_multiple.tcc"

class Load;

class Process {
public:
   Process( CmdArgs &cmd );
   virtual ~Process();
   
   
   virtual void Launch() = 0;

   virtual void SetData( void *ptr, int64_t iteration ) = 0;

   virtual std::ostream& PrintData( std::ostream &stream ) = 0;
   virtual std::ostream& PrintHeader( std::ostream &stream ) = 0;

   
   virtual void Reset( int64_t iteration ) = 0;
  
   virtual void SetRunning( int64_t iteration ) = 0;
   
   virtual void SetReady( int64_t iteration ) = 0;
   virtual bool EveryoneReady( int64_t iteration ) = 0;
   
   virtual void SetDone( int64_t iteration ) = 0;
   virtual bool EveryoneDone( int64_t iteration ) = 0;
   
   virtual void SetWaiting( int64_t iteration ) = 0;
   virtual bool EveryoneWaiting( int64_t iteration ) = 0;
   
   virtual void SetContinuing( int64_t iteration ) = 0;
   virtual bool EveryoneContinuing( int64_t iteration ) = 0;

protected:
   CmdArgs &cmd_args;
};
#endif /* END _PROCESS_HPP_ */
