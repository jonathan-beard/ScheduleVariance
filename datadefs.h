#ifndef _DATADEFS_HPP_
#define _DATADEFS_HPP_

#include <cstdlib>
#include <cstdio>

#define _GNU_SOURCE
#include <sched.h>

#define DEFAULT_BUFFER_SIZE 50
#define RNGTYPE gsl_rng_mt19937

enum Settings { NO = 0, YES };
enum Schedulers { SCHED_FIFO,
                  SCHED_RR,
                  SCHED_OTHER,
                  SCHED_BATCH,
                  Unknown };



union TicksToWait{
   double   d;
   uint64_t i;
};

struct ProcSettings{
   ProcSettings() :
      id( 0 ),
      node_number( 0 ),
      which_processor( 0 ),
      cpusetp( nullptr ),
      num_system_processors( 0 ),
      is_child( NO ),
      num_children( 0 ),
      rng( nullptr ),
      mean_ticks_to_wait( 0 ),
      scheduler_num( Unknown ),
      deterministic( NO ),
      run_length( 0 )
   {
      memset( scheduler_string, '\0', DEFAULT_BUFFER_SIZE );
      memset( system_given_scheduler, '\0', DEFUALT_BUFFER_SIZE );
   }


   ~ProcSettings()
   {
      if( cpusetp != nullptr )
      {
         /* free cpuset */
#if (__GLIBC_MINOR__ > 9) && (__GLIBC__ == 2 )
         CPU_FREE( cpusetp );
#else
         free( cpusetp );
#endif
      }
   }


   void
   set_cpu( const int cpu )
   {
#if (__GLIBC_MINOR__ > 9) && (__GLIBC__ == 2 )
      const size_t cpu_allocate_size = CPU_ALLOC_SIZE( 1 );
      CPU_ZERO_S( cpu_allocate_size, cpusetp );
#else
      CPU_ZERO( cpusetp );
#endif
      CPU_SET( cpu, cpusetp );
      errno = 0;
      auto ret_val( sched_setaffinity( 0 /* self */,
                                       sizeof( cpu_set_t ),
                                       cpusetp ) );
      if( ret_val != EXIT_SUCCESS )
      {
         perror( "Failed to set cpu affinity" );
         exit( EXIT_FAILURE );
      }
   }

   void
   init_rng()
   {
      gsl_rng_env_setup();
      rng = gsl_rng_alloc( RNGTYPE );
      FILE *urandom( nullptr );
      errno = 0;
      urandom = fopen( "/dev/urandom", "r" );
      if( urandom == nullptr )
      {
         perror( "Failed to open /dev/urandom" );
         exit( EXIT_FAILURE );
      }
      unsigned long int seed( 0 );
      errno = 0;
      const auto num_read( fread( &seed, 
                           sizeof( unsigned long int ), 
                           1,
                           urandom ) ) ;
      if( num_read != 1 || errno != 0 )
      {
         perror( "Failed to read from /dev/urandom" );
         exit( EXIT_FAILURE );
      }
      fclose( urandom );
      gsl_rng_set( rng, seed );
   }


      
   int32_t     id;
   int32_t     nodeNumber;
   int32_t     which_processor;
   cpu_set_t   *cpusetp;
   uint32_t    num_system_processors;
   int32_t     is_child; /* 1 if true */
   int32_t     num_children;
   gsl_rng     *rng; /* only allocate after fork */
   union       ticks_to_wait mean_ticks_to_wait;
   struct      timespec myquanta;
   char        scheduler_string[50];
   int         scheduler_num;
   char        system_given_scheduler[50];
   int8_t      deterministic; /* 0 if not */
   double      runlength;
};


struct Timer{
   uint64_t tick_to_stop_on; /* when we eventually want our proc to stop */
};

#endif /* _DATADEFS_HPP_ */
