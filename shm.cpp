/**
 * shm.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Aug  1 14:26:34 2013
 */
#include "shm.hpp"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <cstdint>
#include <cinttypes>
#include <iostream>
#include "getrandom.h"

char*    
SHM::GenKey()
{
   
   const uint32_t key( getUniformRandom() );
   const size_t buffer_size( 1000 );
   char buffer[ buffer_size ];
   memset( buffer, 
           '\0', 
           buffer_size );

   snprintf( buffer, 
             buffer_size, 
             "%" PRIu32 "", 
             key );
   const size_t length( strlen( buffer ) );
   char *out( nullptr );
   out = (char*) malloc( sizeof( char ) * length + 1 );
   out[ length ] = '\0';
   assert( out != nullptr );
   strncpy( out, 
            buffer,
            length );
   return( out );
}

void*
SHM::Init( const char *key,
           size_t nbytes /* bytes for each item */,
           size_t items  /* total number of items */,
           bool   zero   /* zero mem */ )
{
   assert( key != nullptr );
   const int success( 0 );
   const int failure( -1 );
   int fd( failure  );
   /* essentially we want failure if the file exists already */
   if( access( key, F_OK ) == success )
   {
      fprintf( stderr, "File exists, returning!!\n" );
      return( NULL );
   }
   /* set read/write set create if not exists */
   const int flags( O_RDWR | O_CREAT );
   /* set read/write by user */
   const mode_t mode( S_IWUSR | S_IRUSR );
   errno = success;
   fd  = shm_open( key, 
                   flags, 
                   mode );
   if( fd == failure )
   {
      perror( "Failed to open shared memory!!" );
      /* return, just in case the user did something that needs
       * to be dealt with
       */
      return( NULL );
   }
   /* else begin truncate */
   size_t size( nbytes * items );
   errno = success;
   if( ftruncate( fd, size ) != success )
   {
      perror( "Failed to truncate shm!!\n" );
      /* unlink the SHM and return null */
      shm_unlink( key );
      return( NULL );
   }
   /* else begin mmap */
   errno = success;
   void *out( NULL );
   out = mmap( NULL, 
               size, 
               ( PROT_READ | PROT_WRITE ), 
               MAP_SHARED, 
               fd, 
               0 );
   if( out == MAP_FAILED )
   {
      perror( "Failed to mmap shm region, unlinking and returning!!" );
      shm_unlink( key );
      return( NULL );
   }
   if( zero )
   {
      /* everything theoretically went well, lets initialize to zero */
      memset( out, 0x0, size );
   }
   return( out );
}

/** 
 * Open - opens the shared memory segment with the file
 * descriptor stored at key.
 * @param   key - const char *
 * @return  void* - start of allocated memory, or NULL if
 *                  error
 */
void*
SHM::Open( const char *key )
{
   assert( key != nullptr );
   /* accept no zero length keys */
   assert( strlen( key ) > 0 );
   const int success( 0 );
   const int failure( -1 );
   int fd( failure );
   struct stat st;
   memset( &st, 
           0x0, 
           sizeof( struct stat ) );
   const int flags( O_RDWR | O_CREAT );
   mode_t mode( 0 );
   errno = success;
   fd = shm_open( key, 
                  flags, 
                  mode ); 
   if( fd == failure )
   {
      perror( "Failed to open SHM, returning!!" );
      return( NULL );
   }
   /* stat the file to get the size */
   errno = success;
   if( fstat( fd, &st ) != success )
   {
      perror( "Failed to stat SHM file, unlinking and returning!!" );
      shm_unlink( key );
      return( NULL );
   }
   void *out( NULL );
   errno = success;
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
SHM::Close( const char *key,
            void *ptr,
            size_t nbytes,
            size_t nitems,
            bool zero )
{
   const int success( 0 );
   const size_t size( nbytes * nitems );
   if( zero )
   {
      memset( ptr, 0x0, size );
   }
   errno = success;
   if( munmap( ptr, size ) != success )
   {
      perror( "Failed to unmap shared memory, attempting to close!!" );
   }
   errno = success;
   return( shm_unlink( key ) == success );
}
