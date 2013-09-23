/**
 * getexpectation.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Sep 19 14:30:50 2013
 */
#include <vector>
#include <cmath>
#include <utility>

#include "getexpectation.hpp"


const ExpectationStatistics
GetExpectation::Cycles ( std::size_t iterations )
{
   std::vector< uint64_t > cycle_counts( iterations );
   uint64_t accumulate( 0 );
   /* we're already pinned to a processor at this point, however might update in the future */
   for( std::size_t it( 0 ); it < iterations; it++ )
   {
      const auto val( RunLoad() );
      cycle_counts.push_back( val );
      accumulate += val;
   }
   double mean( 0.0 );
   mean = ( double )accumulate / ( double ) cycle_counts.size();
   long double meanacc( 0.0 );
   for( const uint64_t &v : cycle_counts )
   {
      meanacc += std::pow( ( v - mean ), 2.0 );
   }
   long double stddev( 0.0 );
   stddev = std::sqrt( (double)(  meanacc / ( long double )cycle_counts.size() ) );
   uint64_t mean_out( (uint64_t) ceil( mean ) );
   ExpectationStatistics out( mean_out , stddev );
   return( out );
}

uint64_t
GetExpectation::RunLoad()
{
#include "noop_loop_unrolled_load.cpp"
   /** diff is defined as a uint64_t in the above include file **/
   return( diff );
}
