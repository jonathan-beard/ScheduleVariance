/**
 * calibrate.hpp - essentially we're trying to find out how many of the 
 * given load should be run for a given number of seconds.  For our purposes
 * this class will take over execution from the main thread, exit and exec
 * a new copy of main once the changes have been compiled.
 *
 * @author: Jonathan Beard
 * @version: Fri Sep  6 14:51:13 2013
 */
#ifndef _CALIBRATE_HPP_
#define _CALIBRATE_HPP_  1


#include <vector>
#include "sample.hpp"

class Calibrate {
public:
   /**
    * Calibrate - constructor, but might as well be the only
    * public function since it's a call and forget.  If we're
    * calling this then we have the incorrect number of noops
    * in our compiled unrolled loop and we need to restart
    */
   Calibrate( double seconds ); 
   /**
    * Destructor
    */
   ~Calibrate();
private:
    
   std::vector< Sample > samples;
}

#endif /* END _CALIBRATE_HPP_ */
