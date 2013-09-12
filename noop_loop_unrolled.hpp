/**
 * noop_loop_unrolled.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Aug 15 06:54:26 2013
 */
#ifndef _NOOP_LOOP_UNROLLED_HPP_
#define _NOOP_LOOP_UNROLLED_HPP_  1

#include <cstdint>
#include "load.hpp"

class Process;
class GateKeeper;

class NoOpLoopUnrolled : public Load
{
public:
   NoOpLoopUnrolled( CmdArgs &args );
   
   virtual ~NoOpLoopUnrolled();

   
   virtual std::ostream& PrintHeader( std::ostream &stream );
   virtual std::ostream& PrintData( std::ostream &stream, void *d );
   


   class Data : public Load::Data{
   public:
      Data();
      
      Data(uint64_t noopCount,
           uint64_t freq,
           uint64_t cyclesStart,
           uint64_t cyclesEnd,
           double   service_time,
           uint64_t delta );
   
      Data( const Data &d );

      static void PrintHeader( std::ostream &stream );

      static void PrintData( std::ostream &stream, Data &d );

      char load_name[ 40 ];
      uint64_t noop_count;
      uint64_t frequency;
      uint64_t cycles_start;
      uint64_t cycles_end;
      uint64_t diff;
      /* nominally accutime, when calibtimed */
      double   service_time;
   };

   virtual bool AllSet();

protected:
   virtual void RunLoad( Process &p, GateKeeper &g ,int64_t i);

private:
   uint64_t frequency;
   double   seconds;
   double   service_time;
};

#endif /* END _NOOP_LOOP_UNROLLED_HPP_ */
