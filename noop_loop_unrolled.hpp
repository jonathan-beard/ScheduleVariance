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
   


#define LOAD_LENGTH 40
   struct Data : public Load::Data{
      Data() : Load::Data() , noop_count( 0 ),
                              frequency( 0 ),
                              cycles_start( 0 ),
                              cycles_end( 0 ),
                              diff( 0 )
      {
      }
      
      Data(uint64_t noopCount,
           uint64_t freq,
           uint64_t cyclesStart,
           uint64_t cyclesEnd,
           uint64_t delta ) :  Load::Data(), noop_count( noopCount ),
                                             frequency( freq ),
                                             cycles_start( cyclesStart ),
                                             cycles_end( cyclesEnd ),
                                             diff( delta )
      {
      }
   
      Data( const Data &d ) : Load::Data()
      {
         noop_count = d.noop_count;
         frequency = d.frequency;
         cycles_start = d.cycles_start;
         cycles_end = d.cycles_end;
         diff = d.diff;
      }

      static void PrintHeader( std::ostream &stream )
      {
         stream << "Load, Frequency, NoopCount, CyclesStart, CyclesEnd, Delta";
      }

      static void PrintData( std::ostream &stream, Data &d )
      {
         stream << d.load_name << "," << d.frequency << "," << d.noop_count;
         stream << "," << d.cycles_start << "," << d.cycles_end << ",";
         stream << d.diff;
      }

      const char load_name[LOAD_LENGTH] = "noop_unrolled\0";
      uint64_t noop_count;
      uint64_t frequency;
      uint64_t cycles_start;
      uint64_t cycles_end;
      uint64_t diff;
   };
#undef LOAD_LENGTH

protected:
   virtual void RunLoad( Process &p, GateKeeper &g ,int64_t i);

private:
   uint64_t frequency;
};

#endif /* END _NOOP_LOOP_UNROLLED_HPP_ */
