/**
 * noop_loop.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:04:00 2013
 */
#ifndef _NOOP_LOOP_HPP_
#define _NOOP_LOOP_HPP_  1

#include "load.hpp"

class Process;

class NoOpLoop : public Load {
public:
   NoOpLoop( CmdArgs &args );
   NoOpLoop( const NoOpLoop &l );
   virtual ~NoOpLoop();

   virtual void Run( Process &p );
   virtual size_t GetDataStructSize();
   virtual size_t GetNumIterations();

   virtual std::ostream& ReadData( std::ostream &stream,
                                   char *ptr );

   enum Distribution { Deterministic, Uniform, Exponential, HyperExponential };
private:
   int64_t        iterations;
   double         service_time;
   bool           deterministic;
   int64_t        distribution;
   uint64_t       mean_ticks_to_spin;
   std::stringstream    output;
   uint64_t       frequency;
}; 

#endif /* END _NOOP_LOOP_HPP_ */
