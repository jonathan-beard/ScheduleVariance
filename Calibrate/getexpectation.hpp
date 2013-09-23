/**
 * getexpectation.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Sep 19 14:30:50 2013
 */
#ifndef _GETEXPECTATION_HPP_
#define _GETEXPECTATION_HPP_  1

#include <cstdint>

struct ExpectationStatistics
{
   ExpectationStatistics( uint64_t mean, 
                          double stddev ) : mean( mean ),   
                                            stddev( stddev ){};
   ExpectationStatistics( const ExpectationStatistics &other )
   {
      mean = other.mean;
      stddev = other.stddev;
   }

   ~ExpectationStatistics() = default;

   uint64_t mean;
   double   stddev;
};


class GetExpectation {
public:

   static const ExpectationStatistics Cycles( 
                                 std::size_t iterations = 1e6 );

private:
   GetExpectation(){};
   ~GetExpectation(){};
   
   static uint64_t RunLoad();
};

#endif /* END _GETEXPECTATION_HPP_ */
