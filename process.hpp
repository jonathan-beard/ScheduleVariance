/**
 * process.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 16:55:21 2013
 */
#ifndef _PROCESS_HPP_
#define _PROCESS_HPP_  1

class  CmdArgs;

class Process {
public:
   Process( CmdArgs &cmd );
   virtual void Launch( ) = 0;
   virtual std::ostream& Print( std::ostream &stream ) = 0;

protected:
   CmdArgs &cmd_args;
};
#endif /* END _PROCESS_HPP_ */
