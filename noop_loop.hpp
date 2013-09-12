/**
 * noop_loop.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:04:00 2013
 */
#ifndef _NOOP_LOOP_HPP_
#define _NOOP_LOOP_HPP_  1

#include "load.hpp"

class Process;
class GateKeeper;

class NoOpLoop : public Load {
public:
   NoOpLoop( CmdArgs &args );
   
   virtual ~NoOpLoop();


   virtual std::ostream& PrintHeader( std::ostream &stream );

   virtual std::ostream& PrintData( std::ostream &stream, void *d );

   enum Distribution { Deterministic = 0, 
                       Uniform, 
                       Exponential, 
                       HyperExponential,
                       N };
   
   static const char *DistributionString[N];
   

   /* these will be allocated by the calling test harness */
   class Data : public Load::Data {
   public:
      Data();

      Data( Distribution distro,
            double       serviceTime,
            uint64_t     freq,
            uint64_t     meanTicksToSpin,
            uint64_t     targetTicksToSpin,
            uint64_t     actualStopTick );
    
      Data( const Data &d );

      static void PrintHeader( std::ostream &stream );
   
      static void PrintData( std::ostream &stream, Data &d );
      
      char     load_name[ 40 ];
      int64_t  distribution;
      double   service_time;
      uint64_t frequency;
      uint64_t mean_ticks_to_spin;
      uint64_t target_stop_tick;
      uint64_t actual_stop_tick;
   };

protected:
   virtual void RunLoad( Process &p, GateKeeper &g, int64_t i);

private:
   double         service_time;
   Distribution   distribution;
   uint64_t       mean_ticks_to_spin;
   uint64_t       frequency;
}; 

#endif /* END _NOOP_LOOP_HPP_ */
