/**
 * noop_loop.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:04:00 2013
 */
#ifndef _NOOP_LOOP_CLOCK_HPP_
#define _NOOP_LOOP_CLOCK_HPP_  1

#include "load.hpp"
#include <time.h>
class Process;

class NoOpLoopClock : public Load {
public:
   NoOpLoopClock( CmdArgs &args );
   
   virtual ~NoOpLoopClock();

   virtual void Run( Process &p );

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
               delta( 0 ),
               clock_resolution( 0 )
      {
         /* default constructor */
      }

      Data( Distribution distro,
            double       serviceTime,
            double       clockDelta,
            double       clockResolution ): Load::Data(),
                                       distribution( distro ),
                                       service_time( serviceTime ),
                                       delta( clockDelta ),
                                       clock_resolution( clockResolution)
      {
         load_name[0] = 'n';
         load_name[1] = 'o';
         load_name[2] = 'o';
         load_name[3] = 'p';
         load_name[4] = '_';
         load_name[5] = 'c';
         load_name[6] = 'l';
         load_name[7] = 'o';
         load_name[8] = 'c';
         load_name[9] = 'k';
         load_name[10] = 'e';
         load_name[11] = 'd';
         load_name[12] = '\0';
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
         delta              = d.delta;
         clock_resolution   = d.clock_resolution;
      }

      char     load_name[ LOAD_LENGTH ];
      int64_t  distribution;
      double   service_time;
      double   delta;
      double   clock_resolution;

      static void PrintHeader( std::ostream &stream )
      {
        stream << "Load" << "," << "Distribution" << "," << "ServiceTime(s)";
        stream << "," << "ClockDelta(s)" << "," << "ClockResolution(s)";
      }
   
      static void PrintData( std::ostream &stream, Data &d )
      {
         stream << d.load_name << "," << 
            DistributionString[ d.distribution ] << ",";
         stream << d.service_time << ",";
         stream << d.delta << "," << d.clock_resolution;
      }
   };
#undef LOAD_LENGTH

private:
   double          service_time;
   Distribution    distribution;
   double          resolution;
   struct timespec loadtime;
}; 

#endif /* END _NOOP_LOOP_HPP_ */
