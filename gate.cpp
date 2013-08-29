/**
 * gate.cpp - 
 * @author: Jonathan Beard
 * @version: Wed Aug 28 17:32:47 2013
 */
#include "gate.hpp"
#include "shm.hpp"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cassert>
#include <errno.h>
#include <cinttypes>
#include <iostream>
#include <vector>
#include <cstring>

Gate::Gate(int64_t proc_count) : proc( nullptr ),
                                 procsem( nullptr ),
                                 process_count( proc_count ),
                                 child( false )
{
   assert( proc_count > 0 );
   SHM::GenKey( shm_key, buffer_size );
   SHM::GenKey( sem_key, buffer_size );
   errno = 0;
   proc = reinterpret_cast<proc_t*>( SHM::Init( shm_key,
                                                sizeof( proc_t ),
                                                1,
                                                true ) );
   assert( proc != nullptr );
   errno = 0;
   procsem = sem_open( (char*) sem_key ,
                       (O_CREAT | O_RDWR ), 
                        S_IRWXU,
                        0 );
   if( procsem == SEM_FAILED )
   {
      perror("Failed to open semaphore!!");
      exit( EXIT_FAILURE );
   }
   sem_init( procsem , 1 /* multiprocess */, process_count );
}

Gate::Gate( const Gate &g ) : process_count( g.process_count )
{
   memcpy( shm_key, g.shm_key, sizeof( char ) * buffer_size );
   memcpy( sem_key, g.sem_key, sizeof( char ) * buffer_size );
   child = g.child;
   proc = g.proc;
   procsem = g.procsem;
}

Gate::~Gate()
{
}

void 
Gate::ReOpen()
{
   errno = 0;
   proc = reinterpret_cast< proc_t* >( SHM::Open( shm_key ) );
   assert( proc != nullptr );
   procsem = sem_open( (char*)sem_key ,
                       O_RDWR ); 
   if( procsem == SEM_FAILED )
   {
      perror("Failed to open semaphore!!");
      exit( EXIT_FAILURE );
   }
}

void 
Gate::Barrier()
{
   errno = 0;
   if( sem_wait( procsem ) != 0 )
   {
      perror( "Failed to wait at semaphore!!\n" );
      exit( EXIT_FAILURE );
   }
   proc[0] += 1;
   errno = 0;
   if( sem_post( procsem ) != 0 )
   {
      perror( "Failed to post!!\n" );
      exit( EXIT_FAILURE );
   }
   /* enter spin loop for value */
   while( proc[0] != process_count )
   {
      continue;
   }
}

void
Gate::Reset()
{
   errno = 0;
   if( sem_wait( procsem ) != 0 )
   {
      perror( "Failed to wait at semaphore!!\n" );
      exit( EXIT_FAILURE );
   }
   proc[0] -= 1;
   errno = 0;
   if( sem_post( procsem ) != 0 )
   {
      perror( "Failed to post!!\n" );
      exit( EXIT_FAILURE );
   }
   /* enter spin loop for value */
   while( proc[0] != 0 )
   {
      continue;
   }
   /* now everyone should see proc[0] == 0 */
}

void
Gate::Destroy()
{
   if( child )
   {
      SHM::Close( shm_key, 
                  (void*) proc,
                  sizeof( proc_t ),
                  1,
                  false,
                  false /* don't unlink, we didn't keep file descriptors */ );
      if( sem_close( procsem ) != 0 )
      {
         perror( "Failed to close semaphore!!\n" );
      }
   }
   else
   {
      SHM::Close( shm_key, 
                  (void*) proc,
                  sizeof( proc_t ),
                  1,
                  false /* zero, why not! */,
                  true /*  unlink, we didn't keep file descriptors */ );
      errno = 0;                    
      
      if( sem_unlink( sem_key ) != 0 )
      {
         perror( "Failed to close semaphore!!\n" );
      }
   }
}

std::ostream&
Gate::print( std::ostream &stream )
{
   stream << "Gate Stats:\n";
   stream << "Processes: " << process_count << "\n";
   stream << "IsChild: " << (child ? "True" : "False" ) << "\n";
   stream << "SHM_Key: " << (char*)shm_key << "\n";
   stream << "SEM_Key: " << (char*)sem_key << "\n";
   stream << "Key Buffer Size: " << buffer_size << "\n";
   return( stream );
}
