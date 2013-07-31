#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#if (__APPLE__ == 1)
#include <sys/sysctl.h>
#endif

#if (__linux__ == 1)
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#endif

#include "system_query.h"

#ifndef USEPROCCPUINFO
#define USEPROCCPUINFO 0
#endif

/* forward declarations */
#if defined __linux__ && (USEPROCCPUINFO == 0)
extern unsigned long long int __get_clockfreq();
#endif
/* end forward declarations */

inline uint32_t n_processors(){
#ifdef __linux__
   const uint32_t n_procs = get_nprocs();
   return (n_procs);
#elif defined __APPLE__ || __BSD__
   uint32_t mib[4];
   uint32_t n_procs;
   size_t len = sizeof(n_procs);
   /* set the mib for hw.ncpu */
   mib[0] = CTL_HW;
   mib[1] = HW_AVAILCPU;
   /* get the number of CPUs from the system */
   sysctl(mib, 2, &n_procs, &len, NULL, 0);

   if( n_procs < 1 ) 
   {
        mib[1] = HW_NCPU;
        sysctl( mib, 2, &n_procs, &len, NULL, 0 );

        if( n_procs < 1 )
        {
            n_procs = 1;
        }
   }
   return (n_procs);
#else
#warning n_processors undefined for this platform!!
   return (-1);
#endif
}

inline clock_t getStatedCPUFrequency(){
#ifdef __linux__
#if (USEPROCCPUINFO == 1)
   FILE  *fp;
   fp = fopen("/proc/cpuinfo", "r");
   if(fd == NULL)
   {
      fprintf(stderr,"Failed to read proc/cpuinfo!!\n");
      return (-1);
   }
   char *key = (char*) alloca(sizeof(char) * 20);
   char *value (char*) alloca(sizeof(char) * 20);
   while(2 != fscanf(fp,"%s : %s\n", key, value))
   {
      fprintf(stderr,"Key: %s - Value: %s\n",key, value);
   }
   fclose(fd);
   exit(0);
#else
   return ((clock_t)__get_clockfreq());
#endif
#elif defined __APPLE__ || __BSD__
   uint32_t mib[4];
   clock_t freq;
   size_t len = sizeof(freq);
   mib[0] = CTL_HW;
   mib[1] = HW_CPU_FREQ;
   sysctl(mib,2,&freq,&len,NULL,0);
   return (freq);
#else
#warning getStatedCPUFrequency undefined for this platform!!
   return (-1);
#endif
}

inline clock_t getStatedBusFrequency(){
#ifdef __linux__
   return (0);
#elif defined __APPLE__ || __BSD__
   uint32_t mib[4];
   clock_t freq;
   size_t len = sizeof(freq);
   mib[0] = CTL_HW;
   mib[1] = HW_BUS_FREQ;
   sysctl(mib,2,&freq,&len,NULL,0);
   return (freq);
#else
#warning getStatedBusFrequency unsupported for this platform!!
   return (-1);
#endif
}

inline size_t getCacheLineSize(){
#ifdef __linux__
   return (0);
#elif defined __APPLE__ || __BSD__
   uint32_t mib[4];
   clock_t size;
   size_t len = sizeof(size);
   mib[0] = CTL_HW;
   mib[1] = HW_CACHELINE;
   sysctl(mib,2,&size,&len,NULL,0);
   return (size);
#else
#warning getCacheLineSize unsupported for this platform!!
   return (-1);
#endif
}

inline size_t getCacheSize(const uint8_t level){
#ifdef __linux__
   return (0);
#elif defined __APPLE__ || __BSD__
   uint32_t mib[4];
   clock_t size;
   size_t len = sizeof(size);
   mib[0] = CTL_HW;
   switch (level){
      case 1:
         mib[1] = HW_L1ICACHESIZE;
         break;
      case 2:
         mib[1] = HW_L1DCACHESIZE;
         break;
      case 3:
         mib[1] = HW_L2CACHESIZE;
         break;
      case 4:
         mib[1] = HW_L3CACHESIZE;
         break;
      default:
         fprintf(stderr,"Error, invalid level %d!!\n",level);
         exit(-1);
         break;
   }
   sysctl(mib,2,&size,&len,NULL,0);
   return (size);
#else
#warning getCacheSize unsupported for this platform!!
   return (-1);
#endif
}

inline uint64_t readTimeStampCounter()
{
#if __x86_64
   uint64_t highBits = 0x0, lowBits = 0x0;
   __asm__ volatile("\
      lfence                           \n\
      rdtsc                            \n\
      movq     %%rax, %[low]           \n\
      movq     %%rdx, %[high]"          
      :
      /*outputs here*/
      [low]    "=r" (lowBits),
      [high]   "=r" (highBits)
      :
      /*inputs here*/
      :
      /*clobbered registers*/
      "rax","rdx"
   );
   uint64_t cycles = (lowBits & 0xffffffff) | (highBits << 32); 
#else
   uint32_t highBits = 0x0, lowBits = 0x0;
   __asm__ volatile("\
      lfence                           \n\
      rdtsc                            \n\
      movl     %%eax, %[low]           \n\
      movl     %%edx, %[high]"          
      :
      /*outputs here*/
      [low]    "=r" (lowBits),
      [high]   "=r" (highBits)
      :
      /*inputs here*/
      :
      /*clobbered registers*/
      "eax","edx"
   );
   uint64_t cycles = highBits;
   cycles = (lowBits) | (cycles << 32); 
#endif
   return (cycles);
}

inline void forcePreviousInstructionsToComplete()
{
   __asm__ volatile("lfence "::);
}

