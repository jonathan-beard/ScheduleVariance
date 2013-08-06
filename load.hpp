/**
 * load.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 16:55:35 2013
 */
#ifndef _LOAD_HPP_
#define _LOAD_HPP_  1


#include <ostream>
#include "command_arguments.h"
#include "command_option_base.hpp"
#include "command_option_single.tcc"
#include "command_option_multiple.tcc"

class Process;


class Load {
public:
   
   struct Data{
      /* empty struct */
   }; 
   
   Load( CmdArgs &args );
   virtual ~Load();
   virtual void Run( Process &p ) = 0;

   virtual size_t GetNumIterations() = 0;

   virtual std::ostream& PrintHeader( std::ostream &stream ) = 0;
   virtual std::ostream& PrintData( std::ostream &stream, void *d ) = 0;
   
protected:
   CmdArgs &cmd_args;
};

#endif /* END _LOAD_HPP_ */
