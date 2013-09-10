/**
 * process.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 16:55:21 2013
 */
#ifndef _PROCESS_HPP_
#define _PROCESS_HPP_  1

#include <ostream>
#include <time.h>

#include "command_arguments.h"
#include "command_option_base.hpp"
#include "command_option_single.tcc"
#include "command_option_multiple.tcc"

class Load;

class Process {
public:
   Process( CmdArgs &cmd );
   virtual ~Process();
   
   
   virtual void Launch( char **argv ) = 0;

   virtual void SetData( void *ptr ) = 0; 

   virtual std::ostream& PrintData( std::ostream &stream ) = 0;
   virtual std::ostream& PrintHeader( std::ostream &stream ) = 0;
   
   int64_t  get_iterations();
   int64_t  get_curr_iteration();
   void     increment_curr_iteration();

protected:
   CmdArgs &cmd_args;
   const time_t  timestamp;

private:
   int64_t  iterations;
   int64_t  curr_iteration;
};
#endif /* END _PROCESS_HPP_ */
