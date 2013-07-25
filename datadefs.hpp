#ifndef _DATADEFS_HPP_
#define _DATADEFS_HPP_

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


#define DEFAULT_BUFFER_SIZE 50
#define RNGTYPE gsl_rng_mt19937

enum Settings { NO = 0, YES };
enum Schedulers  { FIFO,
                   RR,
                   OTHER,
                   BATCH,
                   Unknown };



union TicksToWait{
   TicksToWait( uint64_t x ) : i(x){}

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
      run_length( 0.0 )
   {
      memset( scheduler_string, '\0',       DEFAULT_BUFFER_SIZE );
      memset( system_given_scheduler, '\0', DEFAULT_BUFFER_SIZE );
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


      
   int64_t     id;
   int64_t     node_number;
   int64_t     which_processor;
   cpu_set_t   *cpusetp;
   uint64_t    num_system_processors;
   int64_t     is_child; /* 1 if true */
   int64_t     num_children;
   gsl_rng     *rng; /* only allocate after fork */
   union       TicksToWait mean_ticks_to_wait;
   struct      timespec myquanta;
   char        scheduler_string[50];
   int         scheduler_num;
   char        system_given_scheduler[50];
   bool        deterministic; /* 0 if not */
   double      run_length;
};


struct Timer{
   uint64_t tick_to_stop_on; /* when we eventually want our proc to stop */
};

#endif /* _DATADEFS_HPP_ */
