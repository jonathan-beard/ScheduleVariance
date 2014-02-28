/**
 * heavy_process.hpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 17:03:38 2013
 */
#ifndef _HEAVY_PROCESS_HPP_
#define _HEAVY_PROCESS_HPP_  1

#include <cstdint>
#include <vector>
#include <array>
#include <cassert>
#include <array>
#include <cstring>
#include <sys/types.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "procstat.h"
#include "gatekeeper.hpp"
#include "procwait.hpp"
#include "process.hpp"
#include "shm.hpp"
#include "unameinfo.hpp"
#include "sysinfo.hpp"

class CmdArgs;

typedef int64_t status_t;

template <class LoadType, class D > class HeavyProcess : public Process {
public:

/* tack this on to whatever struct we're passed in D */
struct Data : public D {
   Data( int64_t iteration, 
         int64_t id,
         int64_t spawn,
         D            &data,
         ProcStatusData &d) : it( iteration ),
                                 p_id( id ),
                                 spawn( spawn ),
                                 d( data )
   {
      proc_stat_data.voluntary_context_swaps = 
         d.voluntary_context_swaps;
      proc_stat_data.non_voluntary_context_swaps = 
         d.non_voluntary_context_swaps;
   }

   
   int64_t  it;
   int64_t  p_id;
   int64_t  spawn;
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
         new Option< int64_t>(  assigned_processor,
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
   if( store != nullptr )
   {
      delete( store );
   }
   store = nullptr;
}


virtual void Launch( char **argv )
{
   if( ! the_load.AllSet() )
   {
      /* at this point the load should have printed 
       * a snarky message describing the exact error
       */
      std::cerr << "Exiting!!\n";
      exit( EXIT_FAILURE );
   }
   /* generate our shared SHM keys */
   SHM::GenKey( shm_key_data , shm_key_length );

   /* make a proc_wait object ptr */
   ProcWait *proc_wait = new ProcWait( spawn );

   /* get a gatekeeper object */
   GateKeeper gate_keeper( spawn );
   
   gate_keeper.RegisterGate( "AllReady" );
   gate_keeper.RegisterGate( "Running" );
   gate_keeper.RegisterGate( "Storing" );
   gate_keeper.RegisterGate( "ReadyToStart" );
   gate_keeper.RegisterGate( "Done" );

   /* set the parent's store object */
   assert( store == nullptr );
   store = new (std::nothrow ) Store<Data>( 
                            get_iterations(),
                            my_id /* should be parent in this case */,
                            spawn,
                            shm_key_data );

   /** TODO, fix to cleanup a bit better **/
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
           
            gate_keeper.HandleFork();
            proc_wait = nullptr;
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
            proc_wait->AddProcess( child );
            continue;
         }
      }
   }
END:;      
   delete( p_stat_data );
   p_stat_data = get_context_swaps_for_process( NULL );
   gate_keeper.WaitForGate( "AllReady" );

   /* lets do something, the load will control the process */
   the_load.Run( *this, gate_keeper );
   
   gate_keeper.WaitForGate( "Done" );
   
   /* control is now back to here, shutdown shm */
   if( is_offspring ){
      /* takes care of unlinking & closing SHM */
      delete( store );
      store = nullptr;
      exit( EXIT_SUCCESS );
   }else{
      proc_wait->WaitForChildren();
      delete( list );
      list = nullptr;
      delete( proc_wait );
      proc_wait = nullptr;
   }
}

virtual std::ostream& 
PrintData( std::ostream &stream )
{
   const int64_t length( spawn * get_iterations() );
   const std::string sep( "," );
  
   for( int64_t index( 0 ); index < length; index++ )
   {
      /* a little hacky I admit, but it works */
      stream << timestamp << sep;
      stream << store->data[index].it << sep << 
      store->data[index].p_id << sep;
      stream << 
         store->data[index].proc_stat_data.voluntary_context_swaps << sep;
      stream << 
         store->data[index].proc_stat_data.non_voluntary_context_swaps 
            << sep << store->data[index].spawn << sep;
   

      auto unamedata( SystemInfo::UnameInfo::getFieldData() );
      for( std::string &str : unamedata )
      {
         stream << str << sep;
      }
      
      auto sysfn( SystemInfo::SysInfo::getFieldData() );
      for( std::string &str : sysfn )
      {
         stream << str << sep;
      }
      the_load.PrintData( stream, 
                          (void*) (&(store->data)[index].d) );
      
      stream << "\n";
   }
      

   return( stream );
}

virtual std::ostream& 
PrintHeader( std::ostream &stream )
{
   const std::string sep( "," );
   std::array<std::string,6> fields =
      {
         "TimeStamp",
         "Iteration",
         "ProcessID",
         "VoluntaryContextSwaps",
         "NonVoluntaryContextSwaps",
         "LoadProcesses"
      };
   for( std::string &str : fields )
   {
      stream << str << sep;
   }
   
   
   auto unamefields( SystemInfo::UnameInfo::getFieldNames() );
   for( std::string &str : unamefields )
   {
      stream << str << sep;
   }
   
   auto sysfn( SystemInfo::SysInfo::getFieldNames() );
   for( std::string &str : sysfn )
   {
      stream << str << sep;
   }

   the_load.PrintHeader( stream );
   stream << "\n";
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
#if(0)   
   struct uname un;
   memset( &un, 0x0, sizeof( struct uname ) );
   errno = 0;
   if( uname( &un ) == EXIT_FAILURE )
   {
      perror( "Failed to get uname information!!, setting to all zeros." );
   }
#endif

   Data process_data( iteration, 
                      my_id, 
                      spawn,
                      *d_ptr, 
                      diff);
   delete( p_stat_data );
   p_stat_data = temp;
   store->Set( &process_data ,
               my_id,
               iteration );
}

protected:
   int64_t spawn;
   int64_t assigned_processor;
   int64_t schedule;
   bool    is_offspring;

private:
   /* just remember ptrs are not carried accross fork */
   static const int shm_key_length = 40;
   std::vector< pid_t > *list; 
   LoadType              the_load;
   char                  shm_key_data[ shm_key_length ];

   
   template <class T> struct Store{
      Store(int64_t iterations, 
            int64_t id,
            int64_t n_spawn,
            const char *key) : data( nullptr ),
                               nitems( 0 ),
                               shm_key( nullptr ),
                               store_parent_id( id )
      {  
         assert( key != nullptr );
         nitems = iterations * n_spawn;
         const int64_t parent( 0 );
         this->iterations = iterations;
         shm_key = strdup( key );
         assert( shm_key != nullptr );
         switch( store_parent_id ){
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
                     false,
                     /* unlink if parent */
                     ( store_parent_id == 0 ? true : false ) );
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
      int64_t           store_parent_id;
   }; /* end struct def */
   

   Store<Data>          *store;
   int64_t              my_id;
   ProcStatusData       *p_stat_data;

};

#endif /* END _HEAVY_PROCESS_HPP_ */
