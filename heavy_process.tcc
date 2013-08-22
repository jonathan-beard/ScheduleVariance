/**
 * heavy_process.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:03:38 2013
 */
#ifndef _HEAVY_PROCESS_HPP_
#define _HEAVY_PROCESS_HPP_  1

#include <cstdint>
#include <vector>
#include <cassert>
#include <array>
#include <sys/types.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "procstat.h"
#include "shm.hpp"


#define SHM_KEY_LENGTH 100

#include "process.hpp"

class CmdArgs;

typedef int64_t status_t;

template <class LoadType, class D > class HeavyProcess : public Process {
public:

/* tack this on to whatever struct we're passed in D */
struct Data : public D {
   Data( int64_t iteration, 
         int64_t id,
         D            &data,
         ProcStatusData &d) : it( iteration ),
                              p_id( id ),
                              d( data ) /* assume D has a copy constructor */
   {
      proc_stat_data.voluntary_context_swaps = 
         d.voluntary_context_swaps;
      proc_stat_data.non_voluntary_context_swaps = 
         d.non_voluntary_context_swaps;
   }

   
   int64_t  it;
   int64_t  p_id;
   D        d;
   ProcStatusData proc_stat_data;
};

HeavyProcess( CmdArgs &cmd ) : Process( cmd ),
                               spawn( 1 ),
                               assigned_processor( 1 ),
                               schedule( SCHED_OTHER ),
                               is_offspring( false ),
                               list( nullptr ),
                               the_load( cmd ),
                               process_status( nullptr ),
                               store( nullptr ),
                               my_id( 0 ),
                               p_stat_data( nullptr )
{
   /* register cmd arguments */
   cmd_args.addOption( 
         new Option< int64_t >( spawn,
                                "-p#",
                                "Number of processes to spawn" ) );
   /* add set processor option */
   cmd_args.addOption(
         new Option< int64_t >( assigned_processor,
                                "-core#",
                                "Which processor to run on" ) );

   cmd_args.addOption(
         new Option< int64_t >( schedule,
                                "-sched",
                         std::string("Schedule, should be one of: SCHED_OTHER,")+
                         std::string("SCHED_FIFO,SCHED_RR"),
                                [&](const char *val, bool &succ)
                                {
                                    const std::string str( val );
                                    succ = true;
                                    if( str.compare( "SCHED_OTHER" ) == 0 )
                                    {
                                       return( SCHED_OTHER );
                                    }
                                    else if( str.compare( "SCHED_FIFO" ) == 0 )
                                    {
                                       return( SCHED_FIFO );
                                    }
                                    else if( str.compare( "SCHED_RR" ) == 0 )
                                    {
                                       return( SCHED_RR );
                                    } /* else */
                                    succ = false;
                                    return( SCHED_OTHER );
                                },
                                []( int64_t &val )
                                {
                                    switch( val )
                                    {
                                       case( SCHED_OTHER ):
                                       {
                                          return( "SCHED_OTHER" );
                                       }
                                       break;
                                       case( SCHED_FIFO ):
                                       {
                                          return( "SCHED_FIFO" );
                                       }
                                       break;
                                       case( SCHED_RR ):
                                       {
                                          return( "SCHED_RR" );
                                       }
                                       break;
                                       default:
                                       {
                                          return( "Unknown Scheduler" );
                                       }
                                    }
                                }
                                ) );

   /* this will be for the parent process only */                                
   list = new std::vector< pid_t >();
}

virtual ~HeavyProcess()
{
   if( list != nullptr ) delete( list );
   list = nullptr;
   if( process_status != nullptr )
   {
      SHM::Close( shm_key_sync,
                  process_status,
                  sizeof( status_t ),
                  spawn * ProcessStatus::N /* #items */,
                  false /* no zero */
                );
   }
   process_status = nullptr;
   if( store != nullptr )
   {
      delete( store );
   }
   store = nullptr;
}


virtual void Launch()
{
   /* generate our shared SHM keys */
   char *temp_data( SHM::GenKey() );
   char *temp_sync( SHM::GenKey() );
   strncpy( shm_key_data, temp_data, SHM_KEY_LENGTH );
   strncpy( shm_key_sync,  temp_sync, SHM_KEY_LENGTH );
 
   free( temp_data );
   free( temp_sync );
  
   /* open the process_status shm */
   process_status = (status_t*)
                    SHM::Init( shm_key_sync,
                               sizeof( int64_t ),
                               1 /* only ready at the moment */ );
   
   assert( process_status != nullptr );
   
   /* set the parent's store object */
   assert( store == nullptr );

   store = new (std::nothrow ) Store<Data>( 
                            get_iterations(),
                            my_id /* should be parent in this case */,
                            spawn,
                            shm_key_data );

   assert( store != nullptr );                         

   const int success( 0 );
   /* schedule can be inherited so lets do it here for all */
   errno = success;
   const auto priority( sched_get_priority_max( schedule ) );
   assert( errno == success );
   errno = success;
   const struct sched_param sp = { .sched_priority = priority };
   auto sch_ret_val( sched_setscheduler(0x0 /* this */,
                                        schedule,
                                        &sp ) );
   if( sch_ret_val != success )
   {
      perror( "Failed to set scheduler" );
      exit( EXIT_FAILURE );
   }

   /* pin processor */
   /* the cpuset should be inherited by forked processes */
   cpu_set_t   *cpuset( nullptr );
   const int8_t processors_to_allocate( 1 );
   size_t cpu_allocate_size( -1 );
#if   (__GLIBC_MINOR__ > 9 ) && (__GLIBC__ == 2 )
   cpuset = CPU_ALLOC( processors_to_allocate );
   assert( cpuset != nullptr );
   cpu_allocate_size =  CPU_ALLOC_SIZE( processors_to_allocate );
   CPU_ZERO_S( cpu_allocate_size, cpuset );
#else
   cpu_allocate_size = sizeof( cpu_set_t );
   cpuset = (cpu_set_t*) malloc( cpu_allocate_size );
   assert( cpuset != nullptr );
   CPU_ZERO( cpuset );
#endif
   CPU_SET( assigned_processor,
            cpuset );
   auto setaffinity_ret_val( success );
   errno = success;
   setaffinity_ret_val = sched_setaffinity( 0 /* self */,
                                            cpu_allocate_size,
                                            cpuset );
   if( setaffinity_ret_val != success )
   {
      perror( "Failed to set processor affinity" );
      exit( EXIT_FAILURE );
   }
   /* start iterations loop here */
   for(; get_curr_iteration() < get_iterations(); 
                                             increment_curr_iteration() )
   {
      pid_t child( 0 ); 
      for( int64_t j( 1 ); j < spawn; j++ )
      {
         child = fork();
         switch( child )
         {
            case( 0 /* CHILD */ ):
            {  
               /* tell yourself you're a child */
               is_offspring = true;
               /* open SHM */
               process_status = nullptr;
               process_status = ( status_t* ) SHM::Open( shm_key_sync );
               assert( process_status != nullptr );
               
               my_id = j;
               /* open store */
               store = nullptr;
               store = new (std::nothrow )Store<Data>( 
                                        get_iterations(), 
                                        my_id,
                                        spawn,
                                        shm_key_data );
               assert( store != nullptr );   
               goto END;
            }
            break;
            case( -1 /* FAILURE, THIS IS PARENT */ ):
            {
               std::stringstream err;
               for( pid_t id : *list )
               {
                  if( kill( id, SIGQUIT ) != success )
                  {
                     err << "Failed to kill pid ( " << id << " )\n";
                  }
               }
               const std::string err_str( err.str() );
               if( err_str.length() > 0 )
               {
                  std::cerr << err_str << "\n";
                  
               }
               exit( EXIT_FAILURE );
            }
            break;
            default: /* this is the PARENT */
            {
               if( list == nullptr )
               {
                  list = new std::vector< pid_t >();
               }
               list->push_back( child );
               continue;
            }
         }
      }
      END:;
      delete( p_stat_data );
      p_stat_data = get_context_swaps_for_process( NULL );
      SetStatus( 0                    /* iteration */, 
                 ProcessStatus::READY /* flag */ );
      while( ! IsEveryoneSetTo( 0 /* iteration */, 
                                ProcessStatus::READY /* flag */ ) )
      {
         continue;
      }
      /* lets do something, the load will control the process */
      the_load.Run( *this );
      /* control is now back to here, shutdown shm */
      if( is_offspring ){
         /* takes care of unlinking & closing SHM */
         delete( store );
         shm_unlink( shm_key_sync );
         /* close unmaps memory too */
#if(0)
         SHM::Close( shm_key_sync,
                     (void*) process_status,
                     sizeof( status_t ),
                     spawn * ProcessStatus::N /* nitems */,
                     false /* don't zero */
                   );
#endif                   
         exit( EXIT_SUCCESS );
      }
      /* all the child procs are dead, go back and start this again */
   }
}

virtual std::ostream& 
PrintData( std::ostream &stream )
{
   const int64_t length( spawn * get_iterations() );
   for( int64_t index( 0 ); index < length; index++ )
   {
      /* a little hacky I admit, but it works */
      stream << store->data[index].it << "," << 
      store->data[index].p_id << ",";
      stream << 
         store->data[index].proc_stat_data.voluntary_context_swaps << ",";
      stream << 
         store->data[index].proc_stat_data.non_voluntary_context_swaps 
            << ",";
      the_load.PrintData( stream, 
                          (void*) (&(store->data)[index].d) ) << "\n";
   }
   return( stream );
}

virtual std::ostream& 
PrintHeader( std::ostream &stream )
{
   stream << "Iteration" << "," << "ProcessID" << "," 
      << "VoluntaryContextSwaps";
   stream << "," << "Non-VoluntaryContextswaps" << ",";
   the_load.PrintHeader( stream );
   return( stream );
}

virtual void 
SetData( void *ptr )
{
   assert( store != nullptr );
   D *d_ptr( reinterpret_cast< D* >( ptr ) );
   ProcStatusData *temp( nullptr );
   temp = get_context_swaps_for_process( NULL /* self */ );
   assert( temp != nullptr );
   ProcStatusData diff;
   diff.voluntary_context_swaps = 
      temp->voluntary_context_swaps - 
         p_stat_data->voluntary_context_swaps;
   diff.non_voluntary_context_swaps = 
      temp->non_voluntary_context_swaps - 
         p_stat_data->non_voluntary_context_swaps;
   /* yes there is a whole lot of copying going on */
   int64_t iteration( get_curr_iteration() );
   Data process_data( iteration, 
                      my_id, 
                      *d_ptr, 
                      diff);
   delete( p_stat_data );
   p_stat_data = temp;
   store->Set( &process_data ,
               my_id,
               iteration );
}

virtual void 
SetStatus( int64_t iteration, ProcessStatus flag )
{
   /* flag = -1 would be bad for subsequent code */
   assert( flag >= 0 );
   SetStatus( my_id, iteration, flag );
}

virtual bool 
IsEveryoneSetTo( int64_t iteration, ProcessStatus flag )
{
   /* flag = -1 would be bad for subsequent code */
   assert( flag >= 0 );
   return( CheckAllForStatus( iteration, flag ) );
}

protected:
   int64_t spawn;
   int64_t assigned_processor;
   int64_t schedule;
   bool    is_offspring;

private:

   int64_t* GetStatusIndex( int64_t           id, 
                            int64_t           iteration, 
                            ProcessStatus     flag )
   {
      const int64_t index( id + ( flag * spawn ) );
      return( &process_status[index] );
   }

   bool  CheckAllForStatus( int64_t           iteration,
                            ProcessStatus     flag )
   {
      if( process_status == nullptr )
      {
         return( false );
      }

      for( int64_t index( 0 ); index < spawn; index++ )
      {
         auto *val( GetStatusIndex( index, 
                                    iteration, 
                                    flag ) );
         if( *val != iteration ) return( false );
      }
      return( true );
   }

   void SetStatus( int64_t           id,
                   int64_t           iteration,
                   ProcessStatus     flag )
   {
      auto *val( GetStatusIndex( id, 
                                 iteration,
                                 flag ) );
      *val = iteration;
   }

   /* just remember ptrs are not carried accross fork */
   std::vector< pid_t > *list; 
   LoadType              the_load;
   char                  shm_key_sync[ SHM_KEY_LENGTH ];
   char                  shm_key_data[ SHM_KEY_LENGTH ];

   
   /* array for each process to keep their status */
   int64_t              *process_status;

   template <class T> struct Store{
      Store(int64_t iterations, 
            int64_t id,
            int64_t n_spawn,
            const char *key) : data( nullptr ),
                               nitems( 0 ),
                               shm_key( nullptr )
      {  
         assert( key != nullptr );
         nitems = iterations * n_spawn;
         const int64_t parent( 0 );
         this->iterations = iterations;
         shm_key = strdup( key );
         assert( shm_key != nullptr );
         switch( id ){
            case( parent ):
            {
               /* create shm */
               data = (T*) SHM::Init( shm_key,
                                      sizeof( T ),
                                      nitems );
               assert( data != nullptr );
            }
            break;
            default:
            {
               /* open shm */
               data = (T*) SHM::Open( shm_key );
               assert( data != nullptr );
            }
         }
      }

      ~Store()
      {
         SHM::Close( shm_key,
                     data,
                     sizeof( T ),
                     nitems,
                     false );
         free( shm_key );
      }

      void              Set( T       *input,
                             int64_t id, 
                             int64_t iteration )
      {
         assert( input != nullptr );
         size_t index( CalcIndex( id, iteration ) );
         assert( index < nitems );
         memcpy( &data[index] , 
                 input, 
                 sizeof( T ) );
      }

      T*   Get( int64_t id, int64_t iteration )
      {
         assert( data != nullptr );
         const size_t index( CalcIndex( id, iteration ) );
         assert( index < nitems );
         return( &data[ index ] );
      }

      size_t   CalcIndex( int64_t id, int64_t iteration )
      {
         assert( id >= 0 );
         assert( iteration >= 0 );
         size_t index( 0 );
         if( id == 0 )
         {
            index = iteration;
         }
         else
         {
            index = (id * iterations) + iteration;
         }
         return( index ); 
      }
      
      /* All the data */
      T                *data;
      size_t            nitems;
      char              *shm_key;
      int64_t           iterations;
   }; /* end struct def */
   

   Store<Data>          *store;
   int64_t              my_id;
   ProcStatusData       *p_stat_data;
};

#endif /* END _HEAVY_PROCESS_HPP_ */
