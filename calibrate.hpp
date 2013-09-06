/**
 * calibrate.hpp - essentially we're trying to find out how many of the 
 * given load should be run for a given number of seconds.
 *
 * @author: Jonathan Beard
 * @version: Fri Sep  6 14:51:13 2013
 */
#ifndef _CALIBRATE_HPP_
#define _CALIBRATE_HPP_  1

typedef double sample_index_t
typedef double measure_t


struct Sample{
   Sample( sample_index_t i, measure_t m ) : index( i ),
                                             measure( m )
   {
      /* nothing else to do */
   }

   Sample( const Sample &s )
   {
      index = s.index;
      measure = s.measure;
   }

   sample_index_t index;
   measure_t      measure;
}


#include <vector>

class Calibrate {
public:
   Calibrate( 

private:
   std::vector< Sample > samples;
}

#endif /* END _CALIBRATE_HPP_ */
