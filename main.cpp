/**
 * main.cpp - 
 * @author: Jonathan Beard
 * @version: Thu Jul 25 10:58:40 2013
 */
#include <unistd.h>
#include <cstdint>
#include <cinttypes>
#include <fstream>

#include "command_arguments.h"
#include "command_option_single.tcc"
#include "command_option_multiple.tcc"

#include "process.hpp"
#include "heavy_process.tcc"
#include "load.hpp"
#include "noop_loop.hpp"
#include "noop_loop_unrolled.hpp"

void
CleanUp()
{
   char *path = getcwd( (char*)NULL, 0x0 );
   assert( path != (char*)NULL );
   //TODO, again, figure out MAX_PATH
   const size_t buff_length = 512 + strlen( path );
   char buffer[ buff_length ];
   std::memset( &buffer[0], '\0', buff_length );
   snprintf( buffer, buff_length, "rm -rf %s", path );
   errno = 0;
   if( system( buffer ) != 0 )
   {
      perror( "Failed to cleanup /tmp contents, continuing!!" );
   }
   free( path );
}

int
main( int argc, char **argv )
{
   /* get command line arguments */
   CmdArgs cmd( "svar",
                std::cout,
                std::cerr );

   /* add help option */
   static bool help( false );
   std::string output_file( "/tmp/output.csv" );
   cmd.addOption( 
         new Option< bool >( help,
                             "-h",
                             "Print this message",
                             [](const char *x, bool &v){
                                 v = true;
                                 return( true );
                             },
                             nullptr,
                             true ) );
  
   static bool print_header( false );
   cmd.addOption( 
         new Option< bool >( print_header,
                             "-print_header",
                             "Print a header before data" ) );

   cmd.addOption( 
         new Option< std::string >( output_file,
                                    "-f",
                                    "Write output to file" ) );

   /* initialize processes & tests */
   Process *process( nullptr );

   process = 
      new HeavyProcess<NoOpLoopUnrolled, NoOpLoopUnrolled::Data >( cmd );

   /* process args */
   cmd.processArgs( argc, argv );

   /* check help */
   if( help || argc == 1){  cmd.printArgs(); exit( EXIT_SUCCESS ); }

   /** hope we have a locking file system **/
   std::ofstream ofs( output_file, std::ofstream::out | std::ofstream::app );
   if( ! ofs.is_open() )
   {
      std::cerr << "Failed to open output file \"" << output_file << "\", exiting!!\n";
      exit( EXIT_FAILURE );
   }
   if( print_header )
   {
      process->PrintHeader( ofs );
   }
   else
   {
      /* check and see if we're running from tmp */ 
      srand( (int) time( (time_t*) NULL ) );
      char *path = getcwd( (char*)NULL, 0x0 );
      assert( path != (char*) NULL );

      char *tmpdir = getenv( "TMPDIR" );
      if( tmpdir == (char*) NULL )
      {
         const char *tmp = "/tmp/";
         const size_t length = strlen( tmp ) + 1;
         tmpdir = (char*) malloc( sizeof( char ) * length );
         std::strncpy( tmpdir, tmp, length );
      }
      else
      {
         char *tmp = (char*) malloc( sizeof( char) * strlen( tmpdir ) + 1 );
         sprintf( tmp, "%s/", tmpdir );
         free( tmpdir );
         tmpdir = tmp;
      }
      
      /* check first 'n' characters for match */
      const int length = strlen( tmpdir );
      for( int i = 0; i < length; i++ )
      {
         if( path[i] != tmpdir[i] )
         {
            if( i == (length - 1 ) )
            {
               std::cerr << "Somthing is very wrong, bar exec in tmp folder\n";
               exit( EXIT_FAILURE );
            }
            else
            {
               /* copy current folder to tmp */
               int rand_name = rand();
               /** TODO, figure out system MAX_PATH var **/
               const size_t buff_length = 512 + length;
               char buffer[ buff_length ];
               std::memset( &buffer[0], '\0', buff_length );
               snprintf( buffer, buff_length, "cp -R %s %s%d", path,
                                                               tmpdir,
                                                               rand_name );
               errno = 0;
               if( system( buffer ) != 0 )
               {
                  perror( "Failed to copy current folder to /tmp, exiting!!" );
                  exit( EXIT_FAILURE );
               }
               /** change working dir **/
               std::memset( &buffer[0], '\0', buff_length );
               snprintf( buffer, buff_length, "%s%d", tmpdir, rand_name );
               if( chdir( buffer ) != 0 )
               {
                  perror( "Failed to change directory to new copy, exiting!!" );
                  delete( process );
                  //TODO add proper cleanup that doesn't delete pwd
                  exit( EXIT_FAILURE );
               }
            }
            /** miss-match, get out **/
            break;
         }
      }

      free( path );

      process->Launch( argv );
      /* child prcesses will have exited */
      /* everybody else should be done at this point, get data */
      process->PrintData( ofs ) << "\n";
      /* cleanup, files will dissappear after proc exits */
      //TODO theoretically safe since all above should have exited upon failure
      //however should re-write CleanUp to check to make sure its the temp one
      //and not the code base
      CleanUp();
   }
   ofs.close();
   delete( process );
   return( EXIT_SUCCESS );
}
