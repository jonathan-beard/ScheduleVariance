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

class Load {
public:
   Load( CmdArgs &args );
   virtual ~Load();
   virtual void Run() = 0;
   virtual std::ostream& Print( std::ostream &stream ) = 0;
   virtual std::ostream& PrintHeader( std::ostream &stream ) = 0;
protected:
   CmdArgs &cmd_args;
};

#endif /* END _LOAD_HPP_ */
