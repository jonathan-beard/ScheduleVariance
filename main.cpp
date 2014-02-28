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

   /* check and see if we're running from tmp */ 
   srand( (int) time( (time_t*) NULL ) );
   char *path = getcwd( (char*)NULL, 0x0 );
   assert( path != (char*) NULL );

   char *tmpdir = getenv( "TMPDIR" );
   if( tmpdir == (char*) NULL )
   {
      tmpdir = "/tmp/";
   }
   else
   {
      char *tmp = (char*) malloc( sizeof( char) * strlen( tmpdir ) + 1 );
      sprintf( temp, "%s/", tmpdir );
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
            std::memset( (char*)buffer[0], '\0', buff_length );
            snprintf( buffer, buff_length, "cp -R %s %s%d", path,
                                                            tmpdir,
                                                            rand_name );
            system( buffer );
            /** change working dir **/
            memset( (char*)buffer[0], '\0', buff_length );
            snprintf( buffer, buff_length, "%s%d", tmpdir, rand_name );
            chdir( buffer );
         }
         /** miss-match, get out **/
         break;
      }
   }

   free( path );

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
      process->PrintHeader( ofs ) << "\n";
   if( ! print_header )
   {
      process->Launch( argv );
      /* child prcesses will have exited */
      /* everybody else should be done at this point, get data */
      process->PrintData( ofs ) << "\n";
   }
   ofs.close();
   delete( process );

   /* now delete self */

   path = getcwd( (char*)NULL, 0x0 );
   assert( path != (char*)NULL );
   //TODO, again, figure out MAX_PATH
   const size_t buff_length = 512 + strlen( path );
   char buffer[ buff_length ];
   std::memset( (char*) buffer[0], '\0', buff_length );
   snprintf( buffer, buff_length, "rm -rf %s", path );
   system( buffer );
   free( path );
   return( EXIT_SUCCESS );
}
