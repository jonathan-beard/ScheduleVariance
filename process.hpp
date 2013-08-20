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

/* general status flags, add more if you need them */
enum ProcessStatus : int8_t {
                               NOTREADY   = 0,
                               READY     ,
                               RUNNING   ,
                               WAITING   ,
                               CONTINUING,
                               DONE      ,
                               N
};

class Process {
public:
   Process( CmdArgs &cmd );
   virtual ~Process();
   
   
   virtual void Launch() = 0;

   virtual void SetData( void *ptr, int64_t iteration ) = 0;

   virtual std::ostream& PrintData( std::ostream &stream ) = 0;
   virtual std::ostream& PrintHeader( std::ostream &stream ) = 0;

   virtual void SetStatus( int64_t iteration, ProcessStatus flag) = 0;

   virtual bool IsEveryoneSetTo( int64_t iteration, ProcessStatus flag ) = 0;

protected:
   CmdArgs &cmd_args;
};
#endif /* END _PROCESS_HPP_ */
