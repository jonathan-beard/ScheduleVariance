/**
 * process.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 16:55:21 2013
 */
#ifndef _PROCESS_HPP_
#define _PROCESS_HPP_  1

struct ProcSettings;

class Process {
public:
   virtual void Launch( ProcSettings &s ) = 0;
   virtual std::ostream& Print( std::ostream &stream ) = 0;
};
#endif /* END _PROCESS_HPP_ */
