#include <cstdint>
#include "system_query.h"
#include "sample.hpp"

Sample
fifty()
{
  uint64_t highBitsBefore = 0x0, lowBitsBefore = 0x0;
  uint64_t highBitsAfter  = 0x0, lowBitsAfter  = 0x0;
  __asm__ volatile("\
                   lfence                           \n\
                   rdtsc                            \n\
                   movq     %%rax, %[lowb]          \n\
                   movq     %%rdx, %[highb]         \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   nop \n\
                   lfence                           \n\
                   rdtsc                            \n\
                   movq     %%rax, %[lowa]          \n\
                   movq     %%rdx, %[higha]         "
                    :
                    [lowb]    "=r" (lowBitsBefore),
                    [highb]   "=r" (highBitsBefore),
                    [lowa]    "=r" (lowBitsAfter),
                    [higha]   "=r" (highBitsAfter)
                    :
                   /*no inputs*/
                    :
                    "rax","rdx"                   );
  uint64_t cyclesbefore
                             = (lowBitsBefore & 0xffffffff) | (highBitsBefore << 32);
  uint64_t cyclesafter
                             = (lowBitsAfter & 0xffffffff) | (highBitsAfter << 32);
  uint64_t diff = cyclesafter - cyclesbefore;
  const clock_t frequency( getStatedCPUFrequency() );
  Sample sample_output;
  sample_output.index = (sample_index_t) 50;
  sample_output.measure = (sample_index_t) diff / (sample_index_t) frequency;
  return( sample_output );
}
