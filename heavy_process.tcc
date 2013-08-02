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

#include <sys/types.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include "shm.hpp"


#define SHM_KEY_LENGTH 100

#include "process.hpp"

class CmdArgs;

template <class LoadType> class HeavyProcess : public Process {
public:
HeavyProcess( CmdArgs &cmd ) : Process( cmd ),
                               spawn( 1 ),
                               assigned_processor( 1 ),
                               schedule( SCHED_OTHER ),
                               child( false ),
                               list( nullptr ),
                               the_load( cmd ),
                               process_status( nullptr ),
                               data( nullptr ),
                               my_id( 0 ),
                               data_size( 0 )
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
                                }) );

   /* this will be for the parent process only */                                
   list = new std::vector< pid_t >();
   InitSyncSHM();
   data_size = the_load.GetDataStructSize();
   InitDataSHM();
}

virtual ~HeavyProcess()
{
   if( list != nullptr ) delete( list );
   list = nullptr;
   if( process_status != nullptr )
   {
      SHM::Close( shm_key );
   }
   process_status = nullptr;
}


virtual void Launch()
{
   /* schedule can be inherited so lets do it here for all */
   errno = EXIT_SUCCESS;
   const auto priority( sched_get_priority_max( schedule ) );
   assert( errno == EXIT_SUCCESS );
   errno = EXIT_SUCCESS;
   const struct sched_param sp = { .sched_priority = priority };
   auto sch_ret_val( sched_setscheduler(0x0 /* this */,
                                        schedule,
                                        &sp ) );
   if( sch_ret_val == EXIT_FAILURE )
   {
      perror( "Failed to set scheduler" );
      exit( EXIT_FAILURE );
   }

   /* pin processor */
   /* the cpuset should be inherited by forked processes */
   cpu_set_t   *cpuset( nullptr );
   const int8_t processes_to_allocate( 1 );
#if   (__GLIBC_MINOR__ > 9 ) && (__GLIBC__ == 2 )
   cpuset = CPU_ALLOC( processes_to_allocate );
   assert( cpuset != nullptr );
   const size_t cpu_allocate_size( CPU_ALLOC_SIZE( processes_to_allocate ) );
   CPU_ZERO_S( cpu_allocate_size, cpuset );
#else
   cpuset = (cpu_set_t*) malloc( sizeof( cpu_set_t ) );
   assert( cpuset != nullptr );
   CPU_ZERO( cpuset );
#endif
   CPU_SET( assigned_processor,
            cpuset );
   auto setaffinity_ret_val( EXIT_SUCCESS );
   errno = EXIT_SUCCESS;
   setaffinity_ret_val = sched_setaffinity( 0 /* self */,
                                            sizeof( cpu_set_t ),
                                            cpuset );
   if( setaffinity_ret_val == EXIT_FAILURE )
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
            child = true;
            /* open SHM */
            process_status = nullptr;
            process_status = (HeavyProcess<LoadType>::ProcessStatus*) 
                              SHM::Open( shm_key_sync );
            assert( process_status != nullptr );

            data = NULL;
            data = SHM::Open( shm_key_data );
            assert( data != NULL );
            my_id = j;
            goto END;
         }
         break;
         case( -1 /* FAILURE, THIS IS PARENT */ ):
         {
            std::stringstream err;
            /* need to keep a list of all child processes and kill them */
            for( pid_t id : *list )
            {
               if( kill( id, SIGQUIT ) != EXIT_SUCCESS )
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
   /* all processes will set their reset mark to zero, eventually */
   assert( Reset() == true );
   while( ! AllReady() )
   {
      continue;
   }
   /* lets do something, the load will control the process */
   the_load.Run( *this );
   /* control is now back to here, shutdown shm */
   SHM::Close( shm_key );
   if( child ) exit( EXIT_SUCCESS );
}

virtual std::ostream& Print( std::ostream &stream )
{
   stream << "Not implemented\n"; 
   return( stream );
}

virtual std::ostream& PrintHeader( std::ostream &stream )
{
   stream << "Not implemented\n"; 
   return( stream );
}

virtual void SetData( void *ptr, size_t nbytes, int64_t iteration )
{
   size_t index( 0 );
   if( my_id == 0 )
   {
      index = iteration;
   }
   else
   {
      index = ( my_id * iteration ) + 1;
   }
   memcpy( &(data[index]), ptr, nbytes );
}

virtual bool Ready()
{
   if( process_status == nullptr )
   {
      return( false );
   }
   return( process_status[ my_id ] == READY );
}

virtual bool AllReady()
{
   if( process_status == nullptr )
   {
      return( false );
   }
   /* we have to wait till everyone is ready */
   for( int64_t index( 0 ); index < spawn; index++ )
   {
      if( process_status[ index ] != READY )
         return( false );
   }
   return( true );
}

virtual bool Reset()
{
   if( process_status == nullptr )
   {
      return( false );
   }
   process_status[ my_id ] = READY;
   return( true );
}

virtual bool ResetAll()
{  
   /* TODO - decide if this is the intended behavior */
   if( process_status == nullptr )
   {
      return( false );
   }
   for( int64_t index( 0 ); index < spawn; index++ )
   {
      if( process_status[ index ] != DONE ){ 
         return( false );
      }
   }
   for( int64_t index( 0 ); index < spawn; index++ )
   {
      process_status[ index ] = READY;
   }
   return( true );
}

virtual void SetRunning()
{
   assert( process_status != nullptr );
   process_status[ my_id ] = RUNNING;
}

virtual void SetDone()
{
   assert( process_status != nullptr );
   process_status[ my_id ] = DONE;
}

virtual void SetWaiting()
{
   assert( process_status != nullptr );
   process_status[ my_id ] = WAITING;
}

virtual bool EveryoneDone()
{
   if( process_status == nullptr )
   {
      return( false );
   }
   for( int64_t index( 0 ); index < spawn; index++ )
   {
      if( process_status[ index ] != DONE ) return( false );
   }
   return( true );
}

virtual bool EveryoneWaiting()
{
   if( process_status == nullptr )
   {
      return( false );
   }
   for( int64_t index( 0 ); index < spawn; index++ )
   {
      if( process_status[ index ] != WAITING ) return( false );
   }
   return( true );
}

protected:
   int64_t spawn;
   int64_t assigned_processor;
   int64_t schedule;
   bool    child;

private:
void InitSyncSHM()
{
   /* set SHM key for all sub-processes of this process to open */
   memset( &shm_key_sync, '\0', SHM_KEY_LENGTH );
   srand( time( nullptr ) );
   int key( rand() );
   /* print the key to shm_key */
   snprintf( shm_key_sync, SHM_KEY_LENGTH, "%d", key );
   process_status = (HeavyProcess<LoadType>::ProcessStatus*)
                    SHM::Init( shm_key_sync,
                               sizeof( ProcessStatus ),
                               spawn );
   assert( process_status != nullptr );
}

void InitDataSHM()
{
   /* set SHM key for all sub-processes of this process to open */
   memset( &shm_key_data, '\0', SHM_KEY_LENGTH );
   srand( time( nullptr ) );
   int key( rand() );
   /* print the key to shm_key */
   snprintf( shm_key_data, SHM_KEY_LENGTH, "%d", key );
   size_t iterations = the_load.GetNumIterations();
   data = (char*) SHM::Init( shm_key_data,
                             data_size,
                             iterations * spawn );
   assert( data != nullptr );
}

   /* just remember ptrs are not carried accross fork */
   std::vector< pid_t > *list; 
   LoadType              the_load;
   char                  shm_key_sync[ SHM_KEY_LENGTH ];
   char                  shm_key_data[ SHM_KEY_LENGTH ];
   enum ProcessStatus : int8_t { NOTREADY = 0, 
                                 READY, 
                                 RUNNING, 
                                 WAITING,
                                 DONE };
   /* array for each process to keep their status */
   ProcessStatus        *process_status;
   char                 *data;
   int64_t              my_id;
   size_t               data_size;
};

#endif /* END _HEAVY_PROCESS_HPP_ */
