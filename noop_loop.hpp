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

   virtual void Run( Process &p, GateKeeper &g);

   virtual std::ostream& PrintHeader( std::ostream &stream );

   virtual std::ostream& PrintData( std::ostream &stream, void *d );

   enum Distribution { Deterministic = 0, 
                       Uniform, 
                       Exponential, 
                       HyperExponential,
                       N };
   
   static const char *DistributionString[N];
   

   /* these will be allocated by the calling test harness */
#define LOAD_LENGTH 40
   struct Data : public Load::Data {
      Data() : Load::Data(),
               distribution( Deterministic ),
               service_time( 0.0 ),
               frequency( 0 ),
               mean_ticks_to_spin( 0 ),
               target_stop_tick( 0 ),
               actual_stop_tick( 0 )
      {
         /* default constructor */
      }

      Data( Distribution distro,
            double       serviceTime,
            uint64_t     freq,
            uint64_t     meanTicksToSpin,
            uint64_t     targetTicksToSpin,
            uint64_t     actualStopTick )  : Load::Data(),
                                             distribution( distro ),
                                             service_time( serviceTime ),
                                             frequency( freq ),
                                             mean_ticks_to_spin( meanTicksToSpin ),
                                             target_stop_tick( targetTicksToSpin ),
                                             actual_stop_tick( actualStopTick )
      {
         load_name[0] = 'n';
         load_name[1] = 'o';
         load_name[2] = 'o';
         load_name[3] = 'p';
         load_name[4] = '\0';
      }
    
      Data( const Data &d ) : Load::Data()
      {
         /* not ideal, but it'll work for now */
         for( int i = 0; i < LOAD_LENGTH; i++ )
         {
            load_name[i] = d.load_name[i];
         }
         distribution       = d.distribution;
         service_time       = d.service_time;
         frequency          = d.frequency;
         mean_ticks_to_spin = d.mean_ticks_to_spin;
         target_stop_tick   = d.target_stop_tick;
         actual_stop_tick   = d.actual_stop_tick;
      }

      char     load_name[ LOAD_LENGTH ];
      int64_t  distribution;
      double   service_time;
      uint64_t frequency;
      uint64_t mean_ticks_to_spin;
      uint64_t target_stop_tick;
      uint64_t actual_stop_tick;
   

      static void PrintHeader( std::ostream &stream )
      {
         stream << "Load" << "," << "Distribution" << "," << "ServiceTime";
         stream << "," << "Frequency" << "," << "TicksToSpin" << ",";
         stream << "TargetStopTick" << "," << "ActualStopTick" << "," << "TickDelta";
      }
   
      static void PrintData( std::ostream &stream, Data &d )
      {
         stream << d.load_name << "," << 
            DistributionString[ d.distribution ] << ",";
         stream << d.service_time << "," << d.frequency << ",";
         stream << d.mean_ticks_to_spin << "," << d.target_stop_tick << ",";
         stream << d.actual_stop_tick << ",";
         stream << ( d.actual_stop_tick - d.target_stop_tick );
      }
   };
#undef LOAD_LENGTH

private:
   double         service_time;
   Distribution   distribution;
   uint64_t       mean_ticks_to_spin;
   uint64_t       frequency;
}; 

#endif /* END _NOOP_LOOP_HPP_ */
