/**
 * shm.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Aug  1 14:26:34 2013
 */
#include "shm.hpp"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

void*
SHM::Init( const char *key,
           size_t nbytes /* bytes for each item */,
           size_t items  /* total number of items */)
{
   int fd( EXIT_FAILURE );
   struct stat st;
   memset( &st, 0x0, sizeof( struct stat ) );
   /* TODO fix this with the correct errno code */
   if( stat( key, &st ) != EXIT_FAILURE )
   {
      fprintf( stderr, "File exists, returning!!\n" );
      return( NULL );
   }
   /* set read/write set create if not exists */
   const int flags( O_RDWR | O_CREAT );
   /* set read/write by user */
   const mode_t mode( S_IWUSR | S_IRUSR );
   errno = 0;
   fd  = shm_open( key, flags , mode );
   if( fd == EXIT_FAILURE )
   {
      perror( "Failed to open shared memory!!" );
      /* return, just in case the user did something that needs
       * to be dealt with
       */
      return( NULL );
   }
   /* else begin truncate */
   size_t size( nbytes * items );
   errno = 0;
   if( ftruncate( fd, size ) == EXIT_FAILURE )
   {
      perror( "Failed to truncate shm!!\n" );
      /* unlink the SHM and return null */
      shm_unlink( key );
      return( NULL );
   }
   /* else begin mmap */
   errno = 0;
   void *out( NULL );
   out = mmap( NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
   if( out == MAP_FAILED )
   {
      perror( "Failed to mmap shm region, unlinking and returning!!" );
      shm_unlink( key );
      return( NULL );
   }
   /* everything theoretically went well, lets initialize to zero */
   memset( out, 0x0, size );
   return( out );
}

void*
SHM::Open( const char *key )
{
   assert( key != nullptr );
   int fd( EXIT_FAILURE );
   struct stat st;
   memset( &st, 0x0, sizeof( struct stat ) );
   const int flags( O_RDWR | O_CREAT );
   mode_t mode( 0 );
   errno = 0;
   fd = shm_open( key, flags, mode ); 
   if( fd == EXIT_FAILURE )
   {
      perror( "Failed to open SHM, returning!!" );
      return( NULL );
   }
   /* stat the file to get the size */
   errno = 0;
   if( fstat( fd, &st ) == EXIT_FAILURE )
   {
      perror( "Failed to stat SHM file, unlinking and returning!!" );
      shm_unlink( key );
      return( NULL );
   }
   void *out( NULL );
   errno = 0;
   out = mmap( NULL, 
               st.st_size, 
               (PROT_READ | PROT_WRITE), 
               MAP_SHARED, 
               fd, 
               0 );
   if( out == MAP_FAILED )
   {
      perror( "Failed to mmap SHM file, unlinking and returning!!" );
      shm_unlink( key );
      return( NULL );
   }
   /* done, return mem */
   return( out );
}

bool
SHM::Close( const char *key )
{
   errno = 0;
   return( shm_unlink( key ) == EXIT_SUCCESS );
}
