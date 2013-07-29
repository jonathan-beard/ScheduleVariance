/**
 * heavy_process.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:03:38 2013
 */
#ifndef _HEAVY_PROCESS_HPP_
#define _HEAVY_PROCESS_HPP_  1

#include <cstdint>
#include <vector>

#include <sys/types.h>

#include "process.hpp"

class CmdArgs;

class HeavyProcess : public Process {
public:
   HeavyProcess( CmdArgs &cmd );
   virtual ~HeavyProcess();

   virtual void Launch();
   virtual std::ostream& Print( std::ostream &stream );
   
protected:
   static int64_t spawn;
   static int64_t assigned_processor;
   static int64_t schedule;

private:
   std::vector< pid_t > *list; 
};

#endif /* END _HEAVY_PROCESS_HPP_ */
