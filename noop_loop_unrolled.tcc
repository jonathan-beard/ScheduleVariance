/**
 * noop_loop_unrolled.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Aug 15 06:54:26 2013
 */
#ifndef _NOOP_LOOP_UNROLLED_HPP_
#define _NOOP_LOOP_UNROLLED_HPP_  1

#include <cstdint>

class NoOpLoopUnrolled : public NoOpLoop
{
   NoOpLoopUnrolled( CmdArgs &args );
   
   virtual void Run( Process & p );
   
   struct Data : public Load::Data{
      Data() : NoOpLoop::Data()
      {
         load_name[4] = '_';
         load_name[5] = 'u';
         load_name[6] = 'n';
         load_name[7] = 'r';
         load_name[8] = 'o';
         load_name[9] = 'l';
         load_name[10] = 'l';
         load_name[11] = 'e';
         load_name[12] = 'd';
         load_name[13] = '\0';
      }
   };

   virtual std::ostream& PrintHeader( std::ostream &stream );
};

#endif /* END _NOOP_LOOP_UNROLLED_HPP_ */
