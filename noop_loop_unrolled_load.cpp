uint64_t highBitsBefore = 0x0, lowBitsBefore = 0x0;
uint64_t highBitsAfter  = 0x0, lowBitsAfter  = 0x0;
const uint64_t  expectedMeanCycles = 100;
const double    expectedSTDCycles  = 0.0;
uint64_t theNoopCount   = 100;
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
                  "rax","rdx"                 );
uint64_t cyclesbefore
                     = (lowBitsBefore & 0xffffffff) | (highBitsBefore << 32);
uint64_t cyclesafter
                     = (lowBitsAfter & 0xffffffff) | (highBitsAfter << 32);
int64_t diff = ( cyclesafter - cyclesbefore ) - expectedMeanCycles ;
