#include <stdint.h>
#include <sys/types.h>

#define L1I 1
#define L1D 2
#define L2 3
#define L3 4

extern inline uint32_t n_processors();

extern inline clock_t getStatedCPUFrequency();

extern inline clock_t getStatedBusFrequency();

extern inline size_t getCacheLineSize();

extern inline size_t getCacheSize(const uint8_t level);

extern inline uint64_t readTimeStampCounter();

extern inline void forcePreviousInstructionsToComplete();

