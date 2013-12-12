/**
 * QueryCPUID.cpp - 
 * @author: Jonathan Beard
 * @version: Tue Dec 10 14:49:55 2013
 */
#include "QueryCPUID.hpp"

#include <cstdio>
#include <cstdlib>
#include <cinttypes>
#include <cstdint>

#include "queryCPUID.h"



/**
 * getEFlags - returns current processor EFlags with structure
 * EFlags defined above
 */
EFlags getEFlags(){
   EFlags flags;
#if(__i386__ == 1)
   __asm__ volatile("pushfd; pop %[flags]" : [flags] "=rm" (flags));
#endif
#if(__x86_64__ == 1)
   __asm__ volatile("pushfq; pop %[flags]" : [flags] "=rm" (flags));
#endif
   return (flags);
}

/**
 * setEFlags - sets flags to values contained in struct EFlags flags
 *
 */
void setEFlags(const EFlags flags){
#if (__i386__ == 1)
   __asm__ volatile ("pushl %[flags]; pop{fd}" : : [flags] "rmi" (flags));
#endif
#if (__x86_64__ == 1)
   __asm__ volatile ("push %[flags]; pop{fq}" : : [flags] "rmi" (flags));
#endif
}

/**
 * isCPUIdSupported - if we can change the CPU ID flag then it's supported
 * if not then no
 */
uint8_t is_cpu_id_supported(){
   EFlags old = getEFlags();
   EFlags new = old;
   new.ID ^= 1;
   setEFlags(new);
   EFlags check = getEFlags();
   setEFlags(old);

   return new.ID == check.ID;
}


 
/**
 * get_cpuid - sets eax, ebx, ecx, edx values in struct Reg based on input_eax input
 */
void get_cpuid(Reg *input_registers,Reg *output_registers){
#if(__i386__ == 1)
   __asm__ volatile("\
      movl    %[input_eax], %%eax            \n\
      movl    %[input_ecx], %%ecx            \n\
      cpuid                                  \n\
      movl    %%eax, %[eax]                  \n\
      movl    %%ebx, %[ebx]                  \n\
      movl    %%ecx, %[ecx]                  \n\
      movl    %%edx, %[edx]"
      :
      [eax] "=r" (output_registers->eax),
      [ebx] "=r" (output_registers->ebx),
      [ecx] "=r" (output_registers->ecx),
      [edx] "=r" (output_registers->edx)
      :
      [input_eax] "m" (input_registers->eax),   
      [input_ecx] "m" (input_registers->ecx)
      :
      "eax","ebx","ecx","edx"
      );
#endif


#if(__x86_64__ == 1)
   __asm__ volatile("\
      movl    %[input_eax], %%eax             \n\
      movl    %[input_ecx], %%ecx             \n\
      cpuid                                  \n\
      movl    %%eax, %[eax]                   \n\
      movl    %%ebx, %[ebx]                   \n\
      movl    %%ecx, %[ecx]                   \n\
      movl    %%edx, %[edx]"                   
      :
      [eax] "=r" (output_registers->eax),
      [ebx] "=r" (output_registers->ebx),
      [ecx] "=r" (output_registers->ecx),
      [edx] "=r" (output_registers->edx)
      :
      [input_eax] "m" (input_registers->eax),
      [input_ecx] "m" (input_registers->ecx)
      :
      "eax","ebx","ecx","edx"
   );
#endif
}

char* getBrandName(Reg *in){
   typedef union{
      char processor_brand[12];
      uint32_t ebx_ecx_edx[3];
   }brand_name;
   brand_name *out = (brand_name*)malloc(sizeof(brand_name));
   out->ebx_ecx_edx[0] = in->ebx;
   out->ebx_ecx_edx[2] = in->ecx;
   out->ebx_ecx_edx[1] = in->edx;
   return (out->processor_brand);
}

void getCacheInfo(Reg **cache_info, size_t *count )
{
   if(is_cpu_id_supported()){
      Reg in;
      in.eax = CPUID_BASIC;
      get_cpuid(&in,*cache_info );
      if((*cache_info)->eax >= CPUID_LEVEL2){
         in.eax = CPUID_LEVEL2;
         zero_registers(*cache_info);
         zero_registers(&in);
         in.eax = CPUID_LEVEL2;
         get_cpuid(&in,*cache_info);
         int num_iterations = CPUID_02H_GET_NUMBER_OF_TIMES_TO_COMPLETE_DATA(in.eax);       if( num_iterations > 1 )
         {
            *count = num_iterations;
            *cache_info = realloc( *cache_info, sizeof( Reg ) * num_iterations );
         }
         for( int i = 0; i < num_iterations; i++ )
         {
            get_cpuid( &in, &((*cache_info)[i]) );
         }
      }
   }
}
