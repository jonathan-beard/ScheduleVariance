/**
 * noop_loop.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:04:00 2013
 */
#ifndef _NOOP_LOOP_HPP_
#define _NOOP_LOOP_HPP_  1

#include "load.hpp"


class NoOpLoop : public Load {
public:
   NoOpLoop( CmdArgs &args );
   NoOpLoop( const NoOpLoop &l );
   virtual ~NoOpLoop();

   virtual void Run();
   virtual std::ostream& Print( std::ostream &stream );
   virtual std::ostream& PrintHeader( std::ostream &stream );
   enum Distribution { Deterministic, Uniform, Exponential, HyperExponential };
private:
   int64_t        iterations;
   double         service_time;
   bool           deterministic;
   int64_t        distribution;
   uint64_t       mean_ticks_to_spin;
   std::stringstream    output;
}; 

#endif /* END _NOOP_LOOP_HPP_ */
