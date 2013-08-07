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
#include "shm.hpp"


#define SHM_KEY_LENGTH 100

#include "process.hpp"

class CmdArgs;

template <class LoadType, class D > class HeavyProcess : public Process {
public:

/* tack this on to whatever struct we're passed in D */
struct Data : public D {
   Data( int64_t iteration, 
         int64_t id,
         D       data) : it( iteration ),
                         p_id( id ),
                         d( data ) /* assume D has a copy constructor */
   {
      /* nothing to do */
   }

   
   int64_t  it;
   int64_t  p_id;
   D        d;
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
                               my_id( 0 )
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
}

virtual ~HeavyProcess()
{
   if( list != nullptr ) delete( list );
   list = nullptr;
   if( process_status != nullptr )
   {
      SHM::Close( shm_key_sync,
                  process_status,
                  sizeof( ProcessStatus ),
                  spawn /* #items */,
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
   process_status = (HeavyProcess<LoadType, D>::ProcessStatus*)
                    SHM::Init( shm_key_sync,
                               sizeof( ProcessStatus ),
                               spawn );
   
   assert( process_status != nullptr );
   /* set everyone's status flag to NOTREADY */
   for( int64_t index( 0 ); index < spawn; index++ )
   {
      process_status[index] = NOTREADY;
   }
   /* set the parent's store object */
   assert( store == nullptr );
   store = new Store<Data>( the_load.GetNumIterations(),
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
            process_status = (HeavyProcess<LoadType, D>::ProcessStatus*) 
                              SHM::Open( shm_key_sync );
            assert( process_status != nullptr );
            
            my_id = j;
            /* open store */
            store = nullptr;
            store = new Store<Data>( the_load.GetNumIterations() , 
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
            /* need to keep a list of all child processes and kill them */
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
   SetReady();
   while( ! EveryoneReady() )
   {
      continue;
   }
   /* lets do something, the load will control the process */
   the_load.Run( *this );
   /* control is now back to here, shutdown shm */
   if( is_offspring ){
      /* takes care of unlinking & closing SHM */
      delete( store );
      /* close unmaps memory too */
      SHM::Close( shm_key_sync,
                  (void*) process_status,
                  sizeof( ProcessStatus ),
                  spawn /* nitems */,
                  false /* don't zero */
                );
      exit( EXIT_SUCCESS );
   }
   /* else 
      if( ! is_offspring ) main will call delete 
    */
}

virtual std::ostream& PrintData( std::ostream &stream )
{
   const int64_t length( spawn * the_load.GetNumIterations() );
   for( int64_t index( 0 ); index < length; index++ )
   {
      /* a little hacky I admit, but it works */
      stream << store->data[index].it << "," << 
      store->data[index].p_id << ",";
      the_load.PrintData( stream, (void*) (&(store->data)[index].d) ) << "\n";
   }
   return( stream );
}

virtual std::ostream& PrintHeader( std::ostream &stream )
{
   stream << "Iteration" << "," << "ProcessID" << ",";
   the_load.PrintHeader( stream ) << "\n";
   return( stream );
}

virtual void SetData( void *ptr, int64_t iteration )
{
   assert( store != nullptr );
   assert( iteration >= 0 );
   D *d_ptr( reinterpret_cast< D* >( ptr ) );
   /* yes there is a whole lot of copying going on */
   Data process_data( iteration, my_id, *d_ptr );
   store->Set( &process_data ,
               my_id,
               iteration );
}

virtual void SetReady()
{
   assert( process_status != nullptr );
   process_status[ my_id ] = READY;
}

virtual bool EveryoneReady()
{
   assert( process_status != nullptr );
   /* we have to wait till everyone is ready */
   for( int64_t index( 0 ); index < spawn; index++ )
   {
      if( process_status[ index ] < READY ) return( false );
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
      if( process_status[ index ] < DONE ) return( false );
   }
   return( true );
}

virtual void SetContinuing()
{
   assert( process_status != nullptr );
   process_status[ my_id ] = CONTINUING;
}

virtual bool EveryoneContinuing()
{
   if( process_status == nullptr )
   {
      return( false );
   }
   for( int64_t index( 0 ); index < spawn; index++ )
   {
      if( process_status[ index ] < CONTINUING ) return( false );
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
      if( process_status[ index ] < WAITING ) return( false );
   }
   return( true );
}

protected:
   int64_t spawn;
   int64_t assigned_processor;
   int64_t schedule;
   bool    is_offspring;

private:

   /* just remember ptrs are not carried accross fork */
   std::vector< pid_t > *list; 
   LoadType              the_load;
   char                  shm_key_sync[ SHM_KEY_LENGTH ];
   char                  shm_key_data[ SHM_KEY_LENGTH ];

   /* TODO: this isn't ideal, but it'll work for this instance */
   enum ProcessStatus : int8_t { NOTREADY = 0, 
                                 READY, 
                                 RUNNING, 
                                 WAITING,
                                 CONTINUING,
                                 DONE };
   
   /* array for each process to keep their status */
   ProcessStatus        *process_status;

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
};

#endif /* END _HEAVY_PROCESS_HPP_ */
