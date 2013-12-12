/**
 * QueryCPUID.hpp - 
 * @author: Jonathan Beard
 * @version: Tue Dec 10 14:49:55 2013
 */
#ifndef _QUERYCPUID_HPP_
#define _QUERYCPUID_HPP_  1
#include <cstdint>
#include <cstdlib>

namespace CPUID{

typedef struct{
     uint32_t
         CF      :  1,
                 :  1,
         PF      :  1,
                 :  1,
         AF      :  1,
                 :  1,
         ZF      :  1,
         SF      :  1,
         TF      :  1,
         IF      :  1,
         DF      :  1,
         OF      :  1,
         IOPL    :  2,
         NT      :  1,
                 :  1,
         RF      :  1,
         VM      :  1,
         AC      :  1,
         VIF     :  1,
         VIP     :  1,
         ID      :  1,
                 : 10;
}EFlags;

typedef struct{
   uint32_t
      STEPPING_ID    : 4,
      MODEL          : 4,
      FAMILY_ID      : 4,
      PROC_TYPE      : 2,
      RESERVED       : 2,
      EXT_MOD_ID     : 4,
      EXT_FAM_ID     : 8,
                     : 4;
}VersionInfo;

typedef union{
   VersionInfo bits;
   uint32_t u_int;
}VersionInfoTwo;


typedef struct{
   uint32_t eax, ebx, ecx, edx;
}Reg;

#define AMD AuthenticAMD
#define INTEL GenuineIntel

//*****Level 1, Version Info
#define getSteppingID(eax)    (eax & 0x0000000f)
#define getModel(eax)         (eax >> 4) & 0x0000000f
#define getFamily(eax)        ((eax & 0x00000f00) >> 8)
#define getType(eax)          ((eax & 0x0000f000) >> 12)


//*****END Level 1, Version Info

//ECX REGISTER FEATURE MASKS
#define     CPUID_FEAT_ECX_SSE3          1 << 0 
#define     CPUID_FEAT_ECX_PCLMUL        1 << 1
#define     CPUID_FEAT_ECX_DTES64        1 << 2
#define     CPUID_FEAT_ECX_MONITOR       1 << 3  
#define     CPUID_FEAT_ECX_DS_CPL        1 << 4  
#define     CPUID_FEAT_ECX_VMX           1 << 5  
#define     CPUID_FEAT_ECX_SMX           1 << 6  
#define     CPUID_FEAT_ECX_EST           1 << 7  
#define     CPUID_FEAT_ECX_TM2           1 << 8  
#define     CPUID_FEAT_ECX_SSSE3         1 << 9 
#define     CPUID_FEAT_ECX_CID           1 << 10
#define     CPUID_FEAT_ECX_FMA           1 << 12
#define     CPUID_FEAT_ECX_CX16          1 << 13 
#define     CPUID_FEAT_ECX_ETPRD         1 << 14 
#define     CPUID_FEAT_ECX_PDCM          1 << 15 
#define     CPUID_FEAT_ECX_DCA           1 << 18 
#define     CPUID_FEAT_ECX_SSE4_1        1 << 19 
#define     CPUID_FEAT_ECX_SSE4_2        1 << 20 
#define     CPUID_FEAT_ECX_x2APIC        1 << 21 
#define     CPUID_FEAT_ECX_MOVBE         1 << 22 
#define     CPUID_FEAT_ECX_POPCNT        1 << 23 
#define     CPUID_FEAT_ECX_AES           1 << 25 
#define     CPUID_FEAT_ECX_XSAVE         1 << 26 
#define     CPUID_FEAT_ECX_OSXSAVE       1 << 27 
#define     CPUID_FEAT_ECX_AVX           1 << 28

//EDX REGISTER FEATURE MASKS
#define     CPUID_FEAT_EDX_FPU           1 << 0  
#define     CPUID_FEAT_EDX_VME           1 << 1  
#define     CPUID_FEAT_EDX_DE            1 << 2  
#define     CPUID_FEAT_EDX_PSE           1 << 3  
#define     CPUID_FEAT_EDX_TSC           1 << 4  
#define     CPUID_FEAT_EDX_MSR           1 << 5  
#define     CPUID_FEAT_EDX_PAE           1 << 6  
#define     CPUID_FEAT_EDX_MCE           1 << 7  
#define     CPUID_FEAT_EDX_CX8           1 << 8  
#define     CPUID_FEAT_EDX_APIC          1 << 9  
#define     CPUID_FEAT_EDX_SEP           1 << 11 
#define     CPUID_FEAT_EDX_MTRR          1 << 12 
#define     CPUID_FEAT_EDX_PGE           1 << 13 
#define     CPUID_FEAT_EDX_MCA           1 << 14 
#define     CPUID_FEAT_EDX_CMOV          1 << 15 
#define     CPUID_FEAT_EDX_PAT           1 << 16 
#define     CPUID_FEAT_EDX_PSE36         1 << 17 
#define     CPUID_FEAT_EDX_PSN           1 << 18 
#define     CPUID_FEAT_EDX_CLF           1 << 19 
#define     CPUID_FEAT_EDX_DTES          1 << 21 
#define     CPUID_FEAT_EDX_ACPI          1 << 22 
#define     CPUID_FEAT_EDX_MMX           1 << 23 
#define     CPUID_FEAT_EDX_FXSR          1 << 24 
#define     CPUID_FEAT_EDX_SSE           1 << 25 
#define     CPUID_FEAT_EDX_SSE2          1 << 26 
#define     CPUID_FEAT_EDX_SS            1 << 27 
#define     CPUID_FEAT_EDX_HTT           1 << 28 
#define     CPUID_FEAT_EDX_TM1           1 << 29 
#define     CPUID_FEAT_EDX_IA64          1 << 30
#define     CPUID_FEAT_EDX_PBE           1 << 31


//EAX REGISTER LEVEL INPUTS
/**
 * 
 * output:
 * EAX - highest supported level
 * EBX,ECX,EDX - processor brand ID (12 chars)
 */
#define    CPUID_BASIC     0x0
/**
 * output:
 */
#define    CPUID_LEVEL1    0x1
/*
 * output:
 */
#define    CPUID_LEVEL2    0x2
/**
 * output:
 */
#define    CPUID_LEVEL3    0x3
/**
 * Deterministic Cache Params Leaf
 * 
 * output:
 * 
 */
#define    CPUID_LEVEL4    0x4
/**
 * Monitor/MWait Leaf
 * output: 
 * EAX = (15 downto 0) smallest monitor size
 * EBX = (15 downto 0) largest monitor size
 */
#define    CPUID_LEVEL5    0x5
#define    CPUID_LEVEL6    0x6
#define    CPUID_LEVEL7    0x7
#define    CPUID_LEVEL8    0x8
#define    CPUID_LEVEL9    0x9
#define    CPUID_LEVEL10   0xa
#define    CPUID_LEVEL11   0xb
#define    CPUID_LEVEL12   0xc
#define    CPUID_LEVEL13   0xd
#define    CPUID_LEVEL14   0xe
#define    CPUID_LEVEL15   0xf


//****ALL FOR LEAF 0AH****
//************EAX****************
//checks performance monitoring verison available, bits (7 downto 0)
#define CPUID_0AH_EAX_PERF_MON_ID(eax)                (eax & 0x000000ff)
//Number of general-purpose performance monitoring counter per logical processor, bits (15 downto 8)
#define CPUID_0AH_EAX_NUM_PERF_MON_COUNTER(eax)       (eax & 0x0000ff00) >> 8
//Bit width of general-purpose, performance monitoring counter, bits (23 downto 16)
#define CPUID_0AH_EAX_WIDTH_PERF_MON_COUNTER(eax)     (eax &  0x00ff0000) >> 16
//Length of EBX bit vector to enumerate architectural per-formance monitoring events, bits (31 downto 24)
#define CPUID_0AH_EAX_EBX_BIT_VECTOR_LENGTH(eax)      eax >> 24
//************EBX****************
//core cycle event not available if 0, bit 0
#define CPUID_0AH_EBX_CORE_CYCLE_EVENT_AVAIL(ebx)     ~(ebx ^ 0xfffffffe)
//instruction retired event not available if 0, bit 1
#define CPUID_0AH_EBX_RETIRED_EVENT_AVAIL(ebx)        ~((ebx ^ 0xfffffffd) >> 1)
//reference cycles event not available if 0, bit 2
#define CPUID_0AH_EBX_REF_CYCLES_EVENT_AVAIL(ebx)     ~((ebx ^ 0xfffffffb) >> 2)
//last-level cache reference event not available if 0, bit 3 
#define CPUID_0AH_EBX_CACHE_REF_EVENT_AVAIL(ebx)      ~((ebx ^ 0xfffffff7) >> 3)
//last-level cache misses event not available if 0, bit 4
#define CPUID_0AH_EBX_CACHE_MISSES_EVENT_AVAIL(ebx)   ~((ebx & 0xffffffef) >> 4)
//branch instruction retired event not available if 1, bit 5
#define CPUID_0AH_EBX_INSTRUCTION_REF_EVENT_AVAIL(ebx) ~((ebx ^ 0xffffffdf) >> 5)
//branch mispredict retired event not available if 1, bit 6
#define CPUID_0AH_EBX_MISPREDICT_EVENT_AVAIL(ebx) ~((ebx ^ 0xffffffbf) >> 6)
//************EDX*****************
#define CPUID_0AH_EDX_NUMBER_FIXED_FUNCTION_PERF_COUNTERS(edx) edx & 0x0000000f
#define CPUID_0AH_EDX_BIT_WIDTH_FIXED_FUNCTION_PERF_COUNTERS(edx) (edx & 0x00000ff0) >> 4
//****END LEAF 0AH****

//****LEAF 02H******
#define CPUID_02H_GET_NUMBER_OF_TIMES_TO_COMPLETE_DATA(eax) (eax & 0x000000ff)
#define CPUID_02H_IS_VALID_REGISTER(reg) ~(reg >> 31)
//****END LEAF 02H*****


#endif /* end __QUERYCPUID_H__ */

class QueryCPUID{
public:
   QueryCPUID()            = default;
   virtual ~QueryCPUID()   = default;
   /**
    * getEFlags - return value is EFlags struct
    * @return struct EFlags
    */
   struct EFlags getEFlags();

   /**
    * setEFlags - sets EFlags to values contained in 
    * struct EFlags flags
    * @param   flags - const EFlags
    */
   void setEFlags(const EFlags flags);

   /**
    * isCPUIDSupported - returns true if supported, false if
    * not.
    * @return bool 
    */
   bool isCPUIDSupported();

   /**
    * getCPUID - sets cpuid in output registers based on 
    * input from input_registers.
    * @param   input_registers - struct Reg*
    * @param   output_registers - struct Reg*
    */
   void getCPUID(Reg *input_registers, Reg *output_registers);

   /**
    * getBrandName - get the name of the processor
    * @param   in - struct Reg*
    */
   std::string getBrandName(Reg *in);

   /**
    * getCacheInfo - 
    */
   void getCacheInfo(Reg **cache_info, size_t *count);

protected:
   static inline void zero_registers(Reg *in){
      in->eax = 0x0;
      in->ebx = 0x0;
      in->ecx = 0x0;
      in->edx = 0x0;
   }

};

} /* end namespace CPUID */
#endif /* END _QUERYCPUID_HPP_ */
