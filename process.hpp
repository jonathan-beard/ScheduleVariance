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
   virtual std::ostream& Print( std::ostream &stream ) = 0;

   /**
    * Ready - Called by various loads to wait for each process or thread
    * running within the test till ready.  Exact implementation left up to the
    * sub-class.  Default version here will always return true;
    * @return  bool - threads are ready.
    */
   virtual bool Ready();
   /**
    * Reset - Called by various loads to resest all processes or threads.
    * @param   bool - reset threads
    */
   virtual bool Reset();
protected:
   CmdArgs &cmd_args;
};
#endif /* END _PROCESS_HPP_ */
