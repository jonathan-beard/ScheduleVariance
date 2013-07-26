/**
 * heavy_process.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:03:38 2013
 */
#ifndef _HEAVY_PROCESS_HPP_
#define _HEAVY_PROCESS_HPP_  1

#include <cstdint>

#include "process.hpp"

class HeavyProcess : public Process {
public:
   HeavyProcess();
   ~HeavyProcess();

   virtual void Launch();
   virtual std::ostream& Print( std::ostream &stream );
   
   int64_t& get_number_of_children();
   int64_t& get_assigned_processor();
   int64_t& get_load_length();
   int64_t& get_number_of_iterations();

private:
   static int64_t number_of_children;
   static int64_t assigned_processor;
   static int64_t load_length;
   static int64_t number_of_iterations;
};

#endif /* END _HEAVY_PROCESS_HPP_ */
