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

   virtual void SetReady();
   virtual bool EveryoneReady();
   
   /**
    * Reset - Called by various loads to resest the calling process or thread.
    * @param   bool - reset threads
    */
   virtual bool Reset();

   virtual bool ResetAll();

   virtual void SetRunning();
   virtual void SetDone();
   virtual void SetWaiting();

   virtual bool EveryoneDone();
   virtual bool EveryoneWaiting();

   virtual void SetContinuing();
   virtual bool EveryoneContinuing();

protected:
   CmdArgs &cmd_args;
};
#endif /* END _PROCESS_HPP_ */
