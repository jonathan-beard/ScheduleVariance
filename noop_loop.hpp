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

   virtual size_t GetNumIterations();
  
   virtual std::ostream& PrintHeader( std::ostream &stream );

   virtual std::ostream& PrintData( std::ostream &stream, void *d );

   enum Distribution { Deterministic = 0, 
                       Uniform, 
                       Exponential, 
                       HyperExponential };

   /* these will be allocated by the calling test harness */
   struct Data : public Load::Data {
      Data()  : distribution( Deterministic ),
                service_time( 0.0 ),
                frequency( 0 ),
                mean_ticks_to_spin( 0 ),
                target_stop_tick( 0 ),
                actual_stop_tick( 0 )
      {
         load_name[0] = 'n';
         load_name[1] = 'o';
         load_name[2] = 'o';
         load_name[3] = 'p';
         load_name[4] = '\0';
      }
     
      char     load_name[40];
      int64_t  distribution;
      double   service_time;
      uint64_t frequency;
      uint64_t mean_ticks_to_spin;
      uint64_t target_stop_tick;
      uint64_t actual_stop_tick;
   };

private:
   int64_t        iterations;
   double         service_time;
   bool           deterministic;
   int64_t        distribution;
   uint64_t       mean_ticks_to_spin;
   uint64_t       frequency;
}; 

#endif /* END _NOOP_LOOP_HPP_ */
